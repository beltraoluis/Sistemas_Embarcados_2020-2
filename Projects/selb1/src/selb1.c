#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

void main(void){
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); // Habilita GPIO N (LED D1 = PN1)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
  
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
  GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

  const int delay = 3000000;
  
  while(1){
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
    for(int i = 0; i < delay; i++);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
    for(int i = 0; i < delay; i++);
  } // while
} // main