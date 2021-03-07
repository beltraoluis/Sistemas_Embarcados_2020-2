#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "driverlib/systick.h"
#include "driverleds.h" // Projects/drivers

// MEF com apenas 2 estados e 1 evento temporal que alterna entre eles
// Seleção por evento

#define CHECK_BIT(var,pos) !!((var) & (1<<(pos)))

volatile uint8_t Evento = 0;

void SysTick_Handler(void){
  Evento = 1;
} // SysTick_Handler

void setLeds(int valor){
  if(CHECK_BIT(valor,0)) LEDOn(LED3); else LEDOff(LED3);
  if(CHECK_BIT(valor,1)) LEDOn(LED2); else LEDOff(LED2);
  if(CHECK_BIT(valor,2)) LEDOn(LED1); else LEDOff(LED1);
}

static int Estado[] = {0, 1, 3, 2, 6, 7, 5, 4};

void main(void){
  static int pos = 0; // estado inicial da MEF

  LEDInit(LED1);
  LEDInit(LED2);
  LEDInit(LED3);
  SysTickPeriodSet(12000000); // f = 1Hz para clock = 24MHz
  SysTickIntEnable();
  SysTickEnable();

  while(1){
    if(pos > 7) pos = 0;
    if(Evento){
      Evento = 0;
      switch(Estado[pos]){
        case 0:
          setLeds(Estado[pos]);
          pos++;
          break;
        case 1:
          setLeds(Estado[pos]);
          pos++;
          break;
        case 3:
          setLeds(Estado[pos]);
          pos++;
          break;
        case 2:
          setLeds(Estado[pos]);
          pos++;
          break;
        case 6:
          setLeds(Estado[pos]);
          pos++;
          break;
        case 7:
          setLeds(Estado[pos]);
          pos++;
          break;
        case 5:
          setLeds(Estado[pos]);
          pos++;
          break;
        case 4:
          setLeds(Estado[pos]);
          pos++;
          break;
      } // switch
    } // if
  } // while
} // main