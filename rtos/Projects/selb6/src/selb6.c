#include <stdbool.h>
#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "driverbuttons.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

#define TIME_FRAME 10000


osThreadId_t taskLed1, taskLed2, taskLed3, taskLed4, taskControl;
osMessageQueueId_t ledQueue1, ledQueue2, ledQueue3, ledQueue4;
osTimerId_t fallEdgeTimer, risingEdgeTimer;
osMutexId_t ledMutex;

const osMutexAttr_t LedsMutexAttr = {
  "LedsMutex",                              // human readable mutex name
  osMutexRecursive | osMutexPrioInherit,    // attr_bits
  NULL,                                     // memory for control block   
  0U                                        // size for control block
  };

typedef struct ledParam{
  int index;
  uint8_t led;
  osMessageQueueId_t *queue;
} LedParam;

LedParam d1 = {0, LED1, &ledQueue1}; 
LedParam d2 = {1, LED2, &ledQueue2}; 
LedParam d3 = {2, LED3, &ledQueue3}; 
LedParam d4 = {3, LED4, &ledQueue4};
LedParam *selected = &d1;
int selectedIndex = 0;
int PWM[4] = {2, 4, 8, 10};
int hPWM[11];

void fallEdgeCallback(void *arg){
  osMessageQueueId_t *queue = (*selected).queue;
  int value = 0;
  osMessageQueuePut(*queue, &value, NULL, osWaitForever);;
}

void risingEdgeCallback(void *arg){
  osMessageQueueId_t *queue = (*selected).queue;
  int value = 1;
  osMessageQueuePut(*queue, &value, NULL, osWaitForever);
}

void control(void *arg){
  while(1) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny ,osWaitForever);
    osTimerStop(fallEdgeTimer);
    osTimerStop(risingEdgeTimer);
    int index = (*selected).index;
    osMessageQueueId_t *queue = (*selected).queue;
    int value = 1;
    osMessageQueuePut(*queue, &value, NULL, osWaitForever);
    osTimerStart(fallEdgeTimer, hPWM[PWM[index]]);
    osTimerStart(risingEdgeTimer, TIME_FRAME);
    ButtonIntEnable(USW1 | USW2);
    
  }
}

void ledPwm(void *arg){
  uint8_t led = ((LedParam *) arg)->led;
  osMessageQueueId_t *queue= ((LedParam *) arg)->queue;
  while(1) {
    int value;
    osMessageQueueGet(*queue, &value, NULL, osWaitForever);
    osMutexAcquire(ledMutex, osWaitForever);
    if(value == 1) {
      LEDOn(led);
    }
    else if(value == 0){
      LEDOff(led);
    }
    else {
      osMutexRelease(ledMutex);
    }
  }
}

void GPIOJ_Handler(){
  if(ButtonRead(USW1)) {
    ButtonIntDisable(USW1 | USW2);
    ButtonIntClear(USW1);
    switch(selectedIndex) {
      case 0: selected = &d2; break;
      case 1: selected = &d3; break;
      case 2: selected = &d4; break;
      default: selected = &d1; break;
    }
    selectedIndex = (*selected).index;
    osThreadFlagsSet(taskControl, 0x0001);
  } else if(ButtonRead(USW2)) {
    ButtonIntDisable(USW1 | USW2);
    ButtonIntClear(USW2);
    osThreadFlagsSet(taskControl, 0x0001);
  }
}

void init() {
  for(int i = 0; i<11; i++) {
    hPWM[i] = (int) i*(TIME_FRAME/10);
  }
}

void main(void){
  init();
  LEDInit(LED4 | LED3 | LED2 | LED1);
  ButtonInit(USW1 | USW2);
  ButtonIntClear(USW1 | USW2);
  ButtonIntEnable(USW1 | USW2);

  osKernelInitialize();
  
  taskControl = osThreadNew(control, NULL, NULL);
  taskLed1 = osThreadNew(ledPwm, &d1, NULL);
  taskLed2 = osThreadNew(ledPwm, &d2, NULL);
  taskLed3 = osThreadNew(ledPwm, &d3, NULL);
  taskLed4 = osThreadNew(ledPwm, &d4, NULL);
  ledQueue1 = osMessageQueueNew(11, sizeof(int), NULL);
  ledQueue2 = osMessageQueueNew(11, sizeof(int), NULL);
  ledQueue3 = osMessageQueueNew(11, sizeof(int), NULL);
  ledQueue4 = osMessageQueueNew(11, sizeof(int), NULL);
  ledMutex = osMutexNew(&LedsMutexAttr);
  fallEdgeTimer = osTimerNew(fallEdgeCallback, osTimerPeriodic, NULL, NULL);
  risingEdgeTimer = osTimerNew(risingEdgeCallback, osTimerPeriodic, NULL, NULL);
  
  osThreadFlagsSet(taskControl, 0x0001);
  
  if(osKernelGetState() == osKernelReady) osKernelStart();

  while(1);
} // main
