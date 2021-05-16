#ifndef __BTL_MAIN__
#define __BTL_MAIN__

//================================ INCLUDES ====================================
#include "system_tm4c1294.h" // CMSIS-Core
#include "cmsis_os2.h" // CMSIS-RTOS
#include "init.h"
#include "utils.h"
//==============================================================================

//================================ RESOURCES ===================================
osThreadId_t controlId;
osThreadId_t uartRxId;
osThreadId_t uartTxId;
osThreadId_t leftElevatorId;
osThreadId_t centerElevatorId;
osThreadId_t rightElevatorId;
osMessageQueueId_t uartTxQueueId;
osMessageQueueId_t leftQueueId;
osMessageQueueId_t centerQueueId;
osMessageQueueId_t rightQueueId;
//==============================================================================

//================================= GLOBAL =====================================
char msg[BUFFER];
//==============================================================================

typedef struct elevatorParam{
  char elevator;
  osMessageQueueId_t *queue;
} ElevatorParam;

void uartTxTask(void *arg0);
void uartRxTask(void *arg0);
void elevatorTask(void *arg0);

#endif