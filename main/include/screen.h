#ifndef SCREEN_H
#define SCREEN_H
#include <inttypes.h>
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "sdkconfig.h"
#include <string.h>
// Screen dimensions

// Screen states
enum SCREEN_STATE_E
{
    SCREEN_HOME,
    SCREEN_BLINK,
    SCREEN_UART,
    SCREEN_SCAN,
    SCREEN_STATE_COUNT // Optional: useful for array sizing and validation
};


// Add other screen-related function declarations as needed
// void screen_task(void *pvParameter);
void screen_init();
void showText(int page, char *txt, bool invert);
void clearLine(int line);
#endif // SCREEN_H