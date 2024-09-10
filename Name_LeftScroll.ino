#include <LedControl.h>

// Define connections to ESP32
#define DIN_PIN 23  // Data In (DIN)
#define CS_PIN 19   // Chip Select (CS)
#define CLK_PIN 18  // Clock (CLK)

// Create a LedControl object
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);  // 1 is the number of MAX7219 modules

// Define characters as a 2D array (7 characters, 8 rows each)
byte Name[7][8] = {
  // S
  {
    0b00111110,  //   #####
    0b01000000,  //  #
    0b01000000,  //  #
    0b00111100,  //   ####
    0b00000010,  //       #
    0b00000010,  //       #
    0b01111100,  //  #####
    0b00000000   // (empty row)
  },
  // H
  {
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b01111110,  //  ######
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b00000000   // (empty row)
  },
  // A
  {
    0b00011000,  //    ##
    0b00100100,  //   #  #
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b01111110,  //  ######
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b00000000   // (empty row)
  },
  // N
  {
    0b01000010,  //  #    #
    0b01100010,  //  ##   #
    0b01010010,  //  # #  #
    0b01001010,  //  #  # #
    0b01000110,  //  #   ##
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b00000000   // (empty row)
  },
  // K
  {
    0b01000100,  //  #   #
    0b01001000,  //  #  #
    0b01010000,  //  # #
    0b01100000,  //  ##
    0b01010000,  //  # #
    0b01001000,  //  #  #
    0b01000100,  //  #   #
    0b00000000   // (empty row)
  },
  // A
  {
    0b00011000,  //    ##
    0b00100100,  //   #  #
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b01111110,  //  ######
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b00000000   // (empty row)
  },
  // R
  {
    0b01111100,  //  #####
    0b01000010,  //  #    #
    0b01000010,  //  #    #
    0b01111100,  //  #####
    0b01010000,  //  # #
    0b01001000,  //  #  #
    0b01000100,  //  #   #
    0b00000000   // (empty row)
  }
};

void setup() {
  // Initialize the MAX7219 module
  lc.shutdown(0, false);  // Wake up display
  lc.setIntensity(0, 8);  // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);     // Clear display register
}

void loop() {
  // Scroll the characters to the left
  scrollLeft(Name, 7);  // Number of characters
  delay(1000);
}

void scrollLeft(byte characters[][8], int numChars) {
  int displayWidth = 8;                   // Width of the display in columns
  int charWidth = 8;                      // Width of each character in columns
  int scrollDelay = 250;                  // Delay between scroll steps (in milliseconds)
  int totalWidth = numChars * charWidth;  // Total width of all characters combined

  for (int offset = 0; offset < totalWidth + displayWidth; offset++) {
    lc.clearDisplay(0);  // Clear display before each new frame

    for (int row = 0; row < 8; row++) {
      byte rowData = 0;

      for (int col = 0; col < displayWidth; col++) {
        int charIndex = (offset + col) / charWidth;
        int bitIndex = (offset + col) % charWidth;

        if (charIndex >= 0 && charIndex < numChars) {
          if (characters[charIndex][row] & (0x80 >> bitIndex)) {
            rowData |= (1 << (7 - col));
          }
        }
      }
      lc.setRow(0, row, rowData);
    }
    delay(scrollDelay);
  }
}