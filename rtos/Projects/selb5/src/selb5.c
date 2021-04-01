#include <stdbool.h>
#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "driverbuttons.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

#define CHECK_BIT(var,pos) !!((var) & (1<<(pos)))

osThreadId_t task;
osMutexId_t mutex;

const osMutexAttr_t attr = {
  "LedMutex",                               // human readable mutex name
  osMutexRecursive | osMutexPrioInherit,    // attr_bits
  NULL,                                     // memory for control block   
  0U                                        // size for control block
  };

int count = 0;

void setLeds(void *arg){
  while(1) {
    osMutexAcquire(mutex, osWaitForever);
    if(CHECK_BIT(count,0)) LEDOn(LED4); else LEDOff(LED4);
    if(CHECK_BIT(count,1)) LEDOn(LED3); else LEDOff(LED3);
    if(CHECK_BIT(count,2)) LEDOn(LED2); else LEDOff(LED2);
    if(CHECK_BIT(count,3)) LEDOn(LED1); else LEDOff(LED1);
  }
}

void GPIOJ_Handler(){
  count++;
  osMutexRelease(mutex);
  ButtonIntClear(USW1);
}

void main(void){
  LEDInit(LED4 | LED3 | LED2 | LED1);
  ButtonInit(USW1);
  ButtonIntClear(USW1);
  ButtonIntEnable(USW1);

  osKernelInitialize();
  
  task = osThreadNew(setLeds, NULL, NULL);
  mutex = osMutexNew(&attr);
  
   if(osKernelGetState() == osKernelReady) osKernelStart();

  while(1);
} // main
