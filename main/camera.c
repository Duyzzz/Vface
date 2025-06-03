#include "camera.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void camera_init(void) {
    // config flash
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;        // Disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;              // Set as output mode
    io_conf.pin_bit_mask = (1ULL << FLASH_PIN);   // Set flash pin
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // Disable pull-down mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;     // Disable pull-up mode
    gpio_config(&io_conf);                        // Apply GPIO configuration
    gpio_set_level(FLASH_PIN, 0);                 // Set flash pin to low (off)
    // config camera
    camera_config_t camera_config = {
            .pin_pwdn = CAM_PIN_PWDN,
            .pin_reset = CAM_PIN_RESET,
            .pin_xclk = CAM_PIN_XCLK,
            .pin_sccb_sda = CAM_PIN_SIOD,
            .pin_sccb_scl = CAM_PIN_SIOC,

            .pin_d7 = CAM_PIN_D7,
            .pin_d6 = CAM_PIN_D6,
            .pin_d5 = CAM_PIN_D5,
            .pin_d4 = CAM_PIN_D4,
            .pin_d3 = CAM_PIN_D3,
            .pin_d2 = CAM_PIN_D2,
            .pin_d1 = CAM_PIN_D1,
            .pin_d0 = CAM_PIN_D0,
            .pin_vsync = CAM_PIN_VSYNC,
            .pin_href = CAM_PIN_HREF,
            .pin_pclk = CAM_PIN_PCLK,

            // XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
            .xclk_freq_hz = 20000000,
            .ledc_timer = LEDC_TIMER_0,
            .ledc_channel = LEDC_CHANNEL_0,

            .pixel_format = PIXFORMAT_JPEG, // YUV422,GRAYSCALE,RGB565,JPEG
            .frame_size = FRAMESIZE_SVGA,   // QVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives
                                            // better frame rates.

            .jpeg_quality = 12, // 0-63, for OV series camera sensors, lower number means higher quality
            .fb_count = 2,      // When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
            .fb_location = CAMERA_FB_IN_PSRAM,
            .grab_mode = CAMERA_GRAB_LATEST,
    };
    while(true){
        esp_err_t err = esp_camera_init(&camera_config);
        if (err == ESP_OK) {
            ESP_LOGI("CAMERA", "Camera initialized successfully");
            break;
        } else {
            ESP_LOGE("CAMERA", "Camera initialization failed: %s, retrying", esp_err_to_name(err));
            vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for 1 second before retrying
        }
    }
}

void flash_on(void) {
    gpio_set_level(FLASH_PIN, 1); // Turn on flash
}
void flash_off(void) {
    gpio_set_level(FLASH_PIN, 0); // Turn off flash
}