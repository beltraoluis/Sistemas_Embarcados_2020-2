#include <stdbool.h>
#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "driverbuttons.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

#define CHECK_BIT(var,pos) !!((var) & (1<<(pos)))
#define BUFFER_SIZE 8

osThreadId_t taskLed, taskDelay;
osSemaphoreId_t ledSemaphore, delaySemaphore;


int count = 0;
bool debounce = false;
uint32_t tick;

void setLeds(void *arg){
  while(1) {
    osSemaphoreAcquire(ledSemaphore, osWaitForever);
    if(CHECK_BIT(count,0)) LEDOn(LED4); else LEDOff(LED4);
    if(CHECK_BIT(count,1)) LEDOn(LED3); else LEDOff(LED3);
    if(CHECK_BIT(count,2)) LEDOn(LED2); else LEDOff(LED2);
    if(CHECK_BIT(count,3)) LEDOn(LED1); else LEDOff(LED1);
    debounce = false;
  }
}

void ledDelay(void *arg){
  while(1) {
    osSemaphoreAcquire(delaySemaphore, osWaitForever);
    tick = osKernelGetTickCount();
    osDelayUntil(tick + 250);
    osSemaphoreRelease(ledSemaphore);
  }
}

void GPIOJ_Handler(){
  ButtonIntClear(USW1);
  if (debounce) return;
  debounce = true;
  count++;
  osSemaphoreRelease(delaySemaphore);
}

void main(void){
  LEDInit(LED4 | LED3 | LED2 | LED1);
  ButtonInit(USW1);
  ButtonIntClear(USW1);
  ButtonIntEnable(USW1);

  osKernelInitialize();
  
  taskLed = osThreadNew(setLeds, NULL, NULL);
  taskDelay = osThreadNew(ledDelay, NULL, NULL);
  ledSemaphore = osSemaphoreNew(BUFFER_SIZE, 0, NULL);
  delaySemaphore = osSemaphoreNew(BUFFER_SIZE, 0, NULL);
  
  if(osKernelGetState() == osKernelReady) osKernelStart();

  while(1);
} // main
