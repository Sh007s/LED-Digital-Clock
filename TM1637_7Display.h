#ifndef TM1637_7DISPLAY_H
#define TM1637_7DISPLAY_H

#include <TM1637Display.h>

#define CLK 18
#define DIO 23

void init_7dispaly(void);
void showDispaly(float displayTemp_Hum);

#endif