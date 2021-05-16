#include "main.h"

ElevatorParam left = {'e', &leftQueueId};
ElevatorParam center = {'c', &centerQueueId};
ElevatorParam right = {'d', &rightQueueId};

void main(void) {
    
    UartInit();

    osKernelInitialize();

    uartTxQueueId = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
    leftQueueId = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
    centerQueueId = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
    rightQueueId = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
    uartRxId = osThreadNew(uartRxTask, NULL, NULL);
    uartTxId = osThreadNew(uartTxTask, NULL, NULL);
    leftElevatorId = osThreadNew(elevatorTask, &left, NULL);
    centerElevatorId = osThreadNew(elevatorTask, &center, NULL);
    rightElevatorId = osThreadNew(elevatorTask, &right, NULL);

    if (osKernelGetState() == osKernelReady) osKernelStart();

    while (1);
}

// region tasks
void uartTxTask(void *arg0) {
    osStatus_t status;
    char str[BUFFER];
    while (1) {
        status = osMessageQueueGet(uartTxQueueId, str, NULL, 0U);
        if (status == osOK) {
            sendString(str);
        }
    }
}

void uartRxTask(void *arg0) {
    char message[BUFFER];
    char firstStr[] = "er\rcr\rdr\r";
    sendString(firstStr);
    while (1) {
        UARTgets(message, BUFFER);
        switch (message[0]) {
            case 'e':
                osMessageQueuePut(leftQueueId, message, 1, 0);
                break;
            case 'c':
                osMessageQueuePut(centerQueueId, message, 1, 0);
                break;
            case 'd':
                osMessageQueuePut(rightQueueId, message, 1, 0);
                break;
        }
    }
}

void elevatorTask(void *param) {
    osStatus_t status;
    int sobe = 0;
    int desce = 0;
    int atual = 0;
    int estado = 0;
    int bit = 0;
    char elevator = ((ElevatorParam *) param)->elevator;
    char command[BUFFER];
    while (1) {
        status = +osMessageQueueGet( *(((ElevatorParam *) param)->queue), command, NULL, 0U);
        if (status == osOK) {
            if (command[1] == 'A') /* ao abrir a porta */ {
                // if (estado != 0) turnOffLight(elevator, atual);
                // atenção o código a seguir depende da ordem de declaração cuidado ao manipular
                // o primeiro caso deve checar ambas as filas para garantir que se seguir pelo menos
                // uma das filas não está vazia
                if(sobe == 0 && desce == 0) { // se as filas estiverem vazias
                    estado = 0;
                }
                else if (sobe == 0 || sobe < bitSet(atual,0)) {
                    if (sobe > 0) {
                        desce = desce |= sobe;
                        sobe = 0;
                    }
                    estado = -1;
                    osDelay(1000);
                    closeDoor(elevator);
                }
                else if (desce == 0 || desce > bitSet(atual,0)) {
                    if (desce > 0) {
                        sobe = sobe |= desce;
                        desce = 0;
                    }
                    estado = 1;
                    osDelay(1000);
                    closeDoor(elevator);
                }
                else { // nenhuma das filas está vazia
                    osDelay(1000);
                    closeDoor(elevator);
                }
            }
            else if (command[1] == 'F') /* ao fechar a porta */ {
                if (estado == 1) { // está subindo
                    sobe = bitSet(bit, sobe);
                    goUp(elevator);
                } else if (estado == -1) { // está descendo
                    desce = bitSet(bit, desce);
                    goDown(elevator);
                } else {
                    if (sobe != 0) {
                        estado = 1;
                    } else if (desce != 0) {
                        estado = -1;
                    }
                    openDoor(elevator);
                }
            }
            else if (command[1] == 'I') /* botao interno */ {
                bit = bitMap(command);
                if (bit != atual) {
                    if (bit > atual) {
                        sobe = bitSet(bit, sobe);
                        if (estado == 0) {
                            estado = 1;
                            closeDoor(elevator);
                        }
                    } else {
                        desce = bitSet(bit, desce);
                        if (estado == 0) {
                            estado = -1;
                            closeDoor(elevator);
                        }
                    }
                }
            }
            else if (strlen(command) == 5) /* botao externo */ {
                bit = bitMap(command);
                //turnOnLight(elevator,bit);
                if (bit != atual) {
                    if (estado == 0) {
                        if (bit > atual) {
                            sobe = bitSet(bit, sobe);
                            estado = 1;
                            closeDoor(elevator);
                        }
                        else {
                            desce = bitSet(bit, desce);
                            estado = -1;
                            closeDoor(elevator);
                        }
                    }
                    else if (command[4] == 's') {
                        sobe = bitSet(bit, sobe);
                    }
                    else {
                        desce = bitSet(bit, desce);
                    }
                }
            }
            else /* andar */ {
                atual = bitMap(command);
                switch (estado) {
                    case 1:
                        if (bitCheck(atual, sobe)) {
                            stop(elevator);
                            sobe = bitClear(atual, sobe);
                            openDoor(elevator);
                        }
                        break;
                    case -1:
                        if (bitCheck(atual, desce)) {
                            stop(elevator);
                            desce = bitClear(atual, desce);
                            openDoor(elevator);
                        }
                        break;
                    default:
                        openDoor(elevator);
                }
            }
        }
    }
}
// endregion
