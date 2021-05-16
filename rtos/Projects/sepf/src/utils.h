#ifndef __BTL_UTILS__
#define __BTL_UTILS__


//================================ INCLUDES ====================================
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
//==============================================================================

//=============================== DEFINITIONS ==================================
#define BUFFER 10
//==============================================================================


void goUp(char elev);
void goDown(char elev);
void stop(char elev);
void closeDoor(char elev);
void openDoor(char elev);
void turnOnLight(char elevator, char andar);
void turnOffLight(char elevator, char andar);
int bitSet(int bit, int num);
int bitClear(int bit, int num);
int bitCheck(int bit, int num);
char levelMap(int bit);
int bitMap(char *str);
void sendString(char string[]);

#endif