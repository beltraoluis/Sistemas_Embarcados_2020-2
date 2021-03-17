#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS

osThreadId_t blink_id1, blink_id2;

typedef struct blink_param{
  uint8_t led;
  uint32_t delay;
} BlinkParam;

BlinkParam d1 = {LED1, 100}; 
BlinkParam d2 = {LED2, 100}; 

void blinkTask(void *arg){
  uint8_t state = 0;
  uint32_t tick;
  BlinkParam *params = (BlinkParam *) arg;
  
  while(1){
    tick = osKernelGetTickCount();
    
    state ^= params->led;
    LEDWrite(params->led, state);
    
    osDelayUntil(tick + params->delay);
  } // while
} // blink

void main(void){
  LEDInit(LED4 | LED3 | LED2 | LED1);

  osKernelInitialize();

  blink_id1 = osThreadNew(blinkTask, &d1, NULL);
  blink_id2 = osThreadNew(blinkTask, &d2, NULL);

  if(osKernelGetState() == osKernelReady)
    osKernelStart();

  while(1);
} // main
