#include <stdbool.h>
#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "driverbuttons.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

#define TIME_FRAME 100
#define BLINK_TIME 50000

osThreadId_t taskLed1, taskLed2, taskLed3, taskLed4, taskControl;
osMessageQueueId_t buttonQueue;
osEventFlagsId_t onChange;
osMutexId_t ledMutex;

typedef struct ledParam{
  int index;
  uint8_t led;
  osThreadId_t *task;
} LedParam;

LedParam d1 = {0, LED1, &taskLed1}; 
LedParam d2 = {1, LED2, &taskLed2}; 
LedParam d3 = {2, LED3, &taskLed3}; 
LedParam d4 = {3, LED4, &taskLed4};
LedParam *selected = &d1;
LedParam *lastSelected = &d4;
int PWM[4] = {1, 2, 4, 8};
int hPWM[11];
bool intDisabled = false;
bool blink = false;

void fallEdgeCallback(void *arg){
  uint8_t led = ((LedParam *) arg)->led;
  osMutexAcquire(ledMutex, osWaitForever);
  LEDOff(led);
  osMutexRelease(ledMutex);
}

void risingEdgeCallback(void *arg){
  uint8_t led = ((LedParam *) arg)->led;
  osMutexAcquire(ledMutex, osWaitForever);
  LEDOn(led);
  osMutexRelease(ledMutex);
}

void enableIntCallback(void *arg){
    ButtonIntEnable(USW1 | USW2);
}

void control(void *arg){
    int value;
    osStatus_t status;
  while(1) {
    status = osMessageQueueGet(buttonQueue, &value, NULL, 0U);
    if (status == osOK) {
      if (value==0) {
        lastSelected = selected;
        switch((*selected).index) {
          case 0: selected = &d2; break;
          case 1: selected = &d3; break;
          case 2: selected = &d4; break;
          default: selected = &d1; break;
        }
        int changeFlag = 1<<(*selected).index | 1<<(*lastSelected).index;
        blink = true;
        osEventFlagsSet(onChange, changeFlag);
      } else {
        int index = (*selected).index;
        int pwm = PWM[index];
        pwm++;
        if (pwm > 10) pwm = 0;
        PWM[index] = pwm;
        osEventFlagsSet(onChange, 1<<(*selected).index);
      }
    }
  }
}

void timerLedControl(void *arg, osTimerId_t risingEdgeTimer, osTimerId_t fallEdgeTimer) {
  uint8_t led = ((LedParam *) arg)->led;
  int index = ((LedParam *) arg)->index;
  int selectedIndex = (*selected).index;
  int highTime = hPWM[PWM[index]];
  int lowTime = TIME_FRAME;
  osTimerStop(fallEdgeTimer);
  osTimerStop(risingEdgeTimer);
  if (index == selectedIndex && blink) {
    blink = false;
    LEDOff(led);
    osDelay(BLINK_TIME);
  }
  switch (PWM[index]) {
    case 0:
      LEDOff(led);
      break;
    case 10:
      LEDOn(led);
      break;
    default:
      osTimerStart(fallEdgeTimer, highTime);
      osTimerStart(risingEdgeTimer, lowTime);
      break;
  }
}

void ledPwm(void *arg){
  int index = ((LedParam *) arg)->index;
  osTimerId_t fallEdgeTimer = osTimerNew(fallEdgeCallback, osTimerPeriodic, arg, NULL);
  osTimerId_t risingEdgeTimer = osTimerNew(risingEdgeCallback, osTimerPeriodic, arg, NULL);
  timerLedControl(arg, risingEdgeTimer, fallEdgeTimer);
  while(1) {
    osEventFlagsWait(onChange, 1<<index, osFlagsWaitAny ,osWaitForever);
    timerLedControl(arg, risingEdgeTimer, fallEdgeTimer);
    osEventFlagsClear(onChange, 1<<index);
  }
}

void GPIOJ_Handler(){
  if(!ButtonRead(USW1)) {
    ButtonIntClear(USW1);
    int value = 0;
    osMessageQueuePut(buttonQueue, &value,  0U, 0U);
  } else {
    intDisabled = true;
    ButtonIntClear(USW2);
    int value = 1;
    osMessageQueuePut(buttonQueue, &value,  0U, 0U);
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
  ButtonIntEnable(USW1 | USW2);
  ButtonIntClear(USW1 | USW2);

  osKernelInitialize();
  
  taskControl = osThreadNew(control, NULL, NULL);
  taskLed1 = osThreadNew(ledPwm, &d1, NULL);
  taskLed2 = osThreadNew(ledPwm, &d2, NULL);
  taskLed3 = osThreadNew(ledPwm, &d3, NULL);
  taskLed4 = osThreadNew(ledPwm, &d4, NULL);
  onChange = osEventFlagsNew(NULL);
  buttonQueue = osMessageQueueNew(11, sizeof(int), NULL);
  ledMutex = osMutexNew(NULL);
  
  if(osKernelGetState() == osKernelReady) osKernelStart();

  while(1);
} // main
