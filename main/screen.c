#include "screen.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define LED 2

SSD1306_t dev;
uint8_t *wk;
enum SCREEN_STATE_E currentScreen;
void screen_init()
{
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&dev, 128, 64);
    ssd1306_contrast(&dev, 0xff);
    ssd1306_clear_screen(&dev, false);
    wk = (uint8_t *)malloc(1024);

    if (wk == NULL)
    {
        printf("MALLOC FAIL\n");
        while (1)
        {
            vTaskDelay(1);
        }
    }
    memset(wk, 0x00, 1024);
}

void showText(int page, char *txt, bool invert){
    clearLine(page);
    int len = strlen(txt);
    ssd1306_display_text(&dev, page, txt, len, invert);
    // ssd1306_show_buffer(&dev);
}
void clearLine(int line){
    // Clear the corresponding part of the buffer
    memset(wk + (line * 128), 0x00, 128);  // 128 is SCREEN_WIDTH
    // Update the display with the cleared buffer
    ssd1306_set_buffer(&dev, wk);
    ssd1306_show_buffer(&dev);
}
