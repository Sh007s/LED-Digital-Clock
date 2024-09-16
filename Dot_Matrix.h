#include <LedControl.h>
#include <Arduino.h>

// Define connections to ESP32
#define DIN_PIN 22  // Data In (DIN)
#define CS_PIN  17  // Chip Select (CS)
#define CLK_PIN 16  // Clock (CLK)

extern byte Sunday[6][8];
extern byte Monday[6][8];
extern byte Tuesday[7][8];
extern byte Wednesday[9][8];
extern byte Thurday[7][8];
extern byte Friday[6][8];
extern byte Saturday[8][8];

extern byte (*Week[7])[8];
extern int counT[7];

void init_DOT_setup();
void scrollLeft(byte characters[][8], int numChars);
