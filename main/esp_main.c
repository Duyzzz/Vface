#include "camera.h"
#include "communication.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "networkConfig.h"
#include "screen.h"
#include <stdio.h>
#define PYTHON_SERVER_IP "192.168.12.2"
#define PYTHON_SERVER_PORT 1400
bool isButtonHold = false; // Flag to check if button is held
bool flash_enabled = false;
const uint8_t CAPTURE_BUTTON_PIN = GPIO_NUM_12; // GPIO pin for button
const uint8_t ENABLE_FLASH_PIN = GPIO_NUM_14; // GPIO pin for flash
const uint8_t STATE_LED = GPIO_NUM_2; // GPIO pin for state LED
#define enable_flash_by_gpio !gpio_get_level(ENABLE_FLASH_PIN) // GPIO pin for flash
destination_t pythonServer = {
        .ip = PYTHON_SERVER_IP,
        .port = PYTHON_SERVER_PORT,
};

void send_image(camera_fb_t *fb) {
    // Implement your image sending logic here
    // For example, send the image over a socket connection
    ESP_LOGI("SEND", "Sending image of size: %zu", fb->len);
    send_data(fb->buf, fb->len, &pythonServer);
}
void config_io(){
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE, // Disable interrupt
        .mode = GPIO_MODE_INPUT,        // Set as input mode
        .pin_bit_mask = (1ULL << CAPTURE_BUTTON_PIN), // Set the pin number
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // Disable pull-down mode
        .pull_up_en = GPIO_PULLUP_ENABLE,       // Enable pull-up mode
    };
    gpio_config(&io_conf); // Configure the GPIO pin
    gpio_config_t io_conf_flash = {
        .intr_type = GPIO_INTR_DISABLE, // Disable interrupt
        .mode = GPIO_MODE_INPUT,        // Set as input mode
        .pin_bit_mask = (1ULL << ENABLE_FLASH_PIN), // Set the pin number
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // Disable pull-down mode
        .pull_up_en = GPIO_PULLUP_ENABLE,       // Enable pull-up mode
    };
    gpio_config(&io_conf_flash); // Configure the GPIO pin
    gpio_config_t state_led_conf = {
        .intr_type = GPIO_INTR_DISABLE, // Disable interrupt
        .mode = GPIO_MODE_OUTPUT,       // Set as output mode
        .pin_bit_mask = (1ULL << STATE_LED), // Set the pin number
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // Disable pull-down mode
        .pull_up_en = GPIO_PULLUP_DISABLE,     // Disable pull-up mode
    };
    gpio_config(&state_led_conf); // Configure the GPIO pin
}
void main_task(void *pvParameters) {
    camera_init();
    vTaskDelay(200 / portTICK_PERIOD_MS); // Wait for 1 second before starting main task
    screen_init();
    config_io();
    vTaskDelay(100 / portTICK_PERIOD_MS); // Wait for 2 seconds before starting main task
    showText(4, "     Hello", false); // Show text on the screen
    gpio_set_level(STATE_LED, 1); // Turn on the state LED
    while (true) {
        // Main task code here
        ssize_t command = get_command();
        char *command_str = get_command_string(command);
        if(gpio_get_level(CAPTURE_BUTTON_PIN) == 0 && !isButtonHold) {
            isButtonHold = true; // Set the flag to indicate button is held
            ESP_LOGI("MAIN", "Button pressed");
            // Add your button press logic here
            if (flash_enabled || enable_flash_by_gpio) {
                flash_on(); // Turn on flash
                vTaskDelay(200 / portTICK_PERIOD_MS); // Wait for 100ms
                camera_fb_t *fb = esp_camera_fb_get();
                if (!fb) {
                    ESP_LOGE("MAIN", "Camera capture failed");
                    return;
                }
                send_image(fb); 
                vTaskDelay(20 / portTICK_PERIOD_MS); // Wait for 100ms
                flash_off();                          // Turn off flash
                esp_camera_fb_return(fb);             // Return the frame buffer back to the driver
            } else {
                camera_fb_t *fb = esp_camera_fb_get();
                if (!fb) {
                    ESP_LOGE("MAIN", "Camera capture failed");
                    return;
                }
                send_image(fb); // Send the captured image
                vTaskDelay(40 / portTICK_PERIOD_MS); // Wait for 100ms
                esp_camera_fb_return(fb);
                ESP_LOGI("MAIN", "Flash is disabled, capture without flash");
            }
        }
        if(gpio_get_level(CAPTURE_BUTTON_PIN) == 1 && isButtonHold) {
            isButtonHold = false; // Reset the flag when button is released
            ESP_LOGI("MAIN", "Button released");
        }
        if (strcmp(command_str, "capture") == 0) {
            ESP_LOGI("MAIN", "Capture command received");
            // Add your capture logic here
            if (flash_enabled || enable_flash_by_gpio) {
                flash_on(); // Turn on flash
                camera_fb_t *fb = esp_camera_fb_get();
                if (!fb) {
                    ESP_LOGE("MAIN", "Camera capture failed");
                    return;
                }
                send_image(fb); // Send the captured image
                ESP_LOGI("MAIN", "Image sent successfully");
                vTaskDelay(100 / portTICK_PERIOD_MS); // Wait for 100ms
                flash_off();                          // Turn off flash
                esp_camera_fb_return(fb);             // Return the frame buffer back to the driver
            } else {
                camera_fb_t *fb = esp_camera_fb_get();
                if (!fb) {
                    ESP_LOGE("MAIN", "Camera capture failed");
                    return;
                }
                send_image(fb); // Send the captured image
                ESP_LOGI("MAIN", "Image sent successfully");
                vTaskDelay(100 / portTICK_PERIOD_MS); // Wait for 100ms
                esp_camera_fb_return(fb);
                ESP_LOGI("MAIN", "Flash is disabled, capture without flash");
            }
            reset_command(sizeof(command_str));
        } else if (strcmp(command_str, "enable_flash") == 0) {
            ESP_LOGI("MAIN", "Enable flash command received");
            // Delay for 1 second
            flash_enabled = true;
            ESP_LOGI("MAIN", "Flash enabled");
            reset_command(sizeof(command_str));
        } else if (strcmp(command_str, "disable_flash") == 0) {
            ESP_LOGI("MAIN", "Disable flash command received");
            flash_enabled = false;
            ESP_LOGI("MAIN", "Flash disabled");
            reset_command(sizeof(command_str));
        } else {
            // comand string will be: "{command}?{data}", split by '?'
            char *delimiter = strchr(command_str, '?');
            if (delimiter != NULL) {
                *delimiter = '\0'; // Split the string into two parts
                char *command = command_str;
                char *data = delimiter + 1;
                if (strcmp(command, "Display") == 0) {
                    if (data[0] != '\0') {
                        ESP_LOGI("MAIN", "Display command received with data: %s", data);
                        if(strcmp(data, "Unknown") == 0) {
                            clearLine(4); // Clear the line before displaying new text
                            showText(4, " UNKNOWN HUMAN", false); // Show text on the screen
                        }else if(strcmp(data, "noFace") == 0) {
                            clearLine(4); // Clear the line before displaying new text
                            showText(4, " CANNOT DETECT", false); // Show text on the screen
                        }else {
                            // Display Hello + data
                            char displayText[16];
                            snprintf(displayText, sizeof(displayText), "  Hello %s", data);
                            clearLine(4); // Clear the line before displaying new text
                            showText(4, displayText, false); // Show text on the screen
                        }
                    } else {
                        ESP_LOGI("MAIN", "No data provided for display command");
                    }
                    memset(command_str, 0, 25); // Clear the command string
                }
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for 20 mili-second
    }
}

void app_main(void) {
    configure_network();
    xTaskCreate(data_receive_task, "data_receive_task", 4096, NULL, 5, NULL);
    xTaskCreate(main_task, "main_task", 4096, NULL, 5, NULL);
}
