#ifndef __BTL_INIT__
#define __BTL_INIT__

//================================ INCLUDES ====================================
#include <stdbool.h>
#include "system_tm4c1294.h" // CMSIS-Core
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
//==============================================================================

extern void UARTStdioIntHandler(void);

void UartInit(void);
void UART0_Handler(void);

#endif