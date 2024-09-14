#ifndef BUTTON_H
#define BUTTON_H

#include <ezButton.h>

#define BUTTON_PIN1 14          // Define the button pin1 for TImerPuase
#define BUTTON_PIN2 13          // Define the button pin2 for Change the State
#define BUTTON_PIN3 12          // Define the button pin3 for Increment

#define DEBOUNCE_TIME1 50       // Debounce time for the button
#define DEBOUNCE_TIME2 50       // Debounce time for the button
#define DEBOUNCE_TIME3 50       // Debounce time for the button
#define SHORT_PRESS_SEC 1000    // press the SHORT_PRESS_SEC 500 to incremnt the sec
#define MEDIUM_PRESS_MINS 2000  // press the MEDIUM_PRESS_SEC 1000 to incremnt the mins
#define LONG_PRESS_HOURS 3000   // press the LONG_PRESS_SEC 1500 to incremnt the hours

extern ezButton button1;  // Initialize the button
extern ezButton button2;  // Initialize the button
extern ezButton button3;  // Initialize the button

void init_Button(void);

#endif // BUTTON_H