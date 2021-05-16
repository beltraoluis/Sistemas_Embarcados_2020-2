#include "utils.h"

int pos = 0;

void goUp(char elev) {
    char command[BUFFER] = "xs\r\0";
    command[0] = elev;  // qual elevador
    sendString(command);
}

void goDown(char elev) {
    char command[BUFFER] = "xd\r\0";
    command[0] = elev;  // qual elevador
    sendString(command);
}

void stop(char elev) {
    char command[BUFFER] = "xp\r\0";
    command[0] = elev;  // qual elevador
    sendString(command);
}

void closeDoor(char elev) {
    char command[BUFFER] = "xf\r\0";
    command[0] = elev;  // qual elevador
    sendString(command);
}

void openDoor(char elev) {
    char command[BUFFER] = "xa\r\0";
    command[0] = elev;  // qual elevador
    sendString(command);
}

void turnOnLight(char elevator, char andar) {
    char command[BUFFER] = "xLx\r\0";
    command[0] = elevator;  // qual elevador
    command[2] = andar;  // qual andar
    sendString(command);
}

void turnOffLight(char elevator, char andar) {
    char command[BUFFER] = "xDx\r\0";
    command[0] = elevator;  // qual elevador
    command[2] = andar;  // qual andar
    sendString(command);
}

int bitSet(int bit, int num) {
    return num |= (1 << bit);
}

int bitClear(int bit, int num) {
    return num &= ~(1 << bit);
}

int bitCheck(int bit, int num) {
    return !!((num >> bit) & 1);
}

char levelMap(int bit) {
    switch (bit) {
        case 0:
            return 'a';
        case 1:
            return 'b';
        case 2:
            return 'c';
        case 3:
            return 'd';
        case 4:
            return 'e';
        case 5:
            return 'f';
        case 6:
            return 'g';
        case 7:
            return 'h';
        case 8:
            return 'i';
        case 9:
            return 'j';
        case 10:
            return 'k';
        case 11:
            return 'l';
        case 12:
            return 'm';
        case 13:
            return 'n';
        case 14:
            return 'o';
        case 15:
            return 'p';
        default:
            return 'a';
    }
}

int bitMap(char *str) {
    if (strlen(str) == 5) { // botoes externos
        if (str[2] == '0') {
            switch (str[3]) { // entre 0 e 9 inclusivo
                case '0':
                    return 0;
                case '1':
                    return 1;
                case '2':
                    return 2;
                case '3':
                    return 3;
                case '4':
                    return 4;
                case '5':
                    return 5;
                case '6':
                    return 6;
                case '7':
                    return 7;
                case '8':
                    return 8;
                case '9':
                    return 9;
                default:
                    return 0;
            }
        } else {
            switch (str[3]) { // entre 10 e 15 inclusivo
                case '0':
                    return 10;
                case '1':
                    return 11;
                case '2':
                    return 12;
                case '3':
                    return 13;
                case '4':
                    return 14;
                case '5':
                    return 15;
                default:
                    return 0;
            }
        }
    } else if (strlen(str) == 2) {
        switch (str[1]) { // andares entre 0 e 9 inclusivo
            case '0':
                return 0;
            case '1':
                return 1;
            case '2':
                return 2;
            case '3':
                return 3;
            case '4':
                return 4;
            case '5':
                return 5;
            case '6':
                return 6;
            case '7':
                return 7;
            case '8':
                return 8;
            case '9':
                return 9;
            default:
                return 0;
        }
    } else if (strlen(str) == 3) {
        if (str[1] == 'I') { // botoes internos
            //turnOnLight(str[0], str[2]);// acender botao
            switch (str[2]) { 
                case 'a': return 0;
                case 'b': return 1;
                case 'c': return 2;
                case 'd': return 3;
                case 'e': return 4;
                case 'f': return 5;
                case 'g': return 6;
                case 'h': return 7;
                case 'i': return 8;
                case 'j': return 9;
                case 'k': return 10;
                case 'l': return 11;
                case 'm': return 12;
                case 'n': return 13;
                case 'o': return 14;
                case 'p': return 15;
                default: return 0;
            }
        }
        switch (str[2]) { // andares entre 10 e 15 inclusivo
            case '0':
                return 10;
            case '1':
                return 11;
            case '2':
                return 12;
            case '3':
                return 13;
            case '4':
                return 14;
            case '5':
                return 15;
            default:
                return 0;
        }
    } else {
        return 0;
    }
}

void sendString(char string[]) {
    for (uint8_t i = 0; i < strlen(string); i++) {
        UARTCharPut(UART0_BASE, string[i]);
    }
}

