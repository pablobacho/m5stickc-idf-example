#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "m5stickc.h"

static const char *TAG = "MAIN";

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void my_m5_event_handler(void * handler_arg, esp_event_base_t base, int32_t id, void * event_data) {
    if(base != M5BUTTON_EVENT_BASE) {
        return;
    }

    switch(id) {
        case M5BUTTON_BUTTON_A_CLICK:
            TFT_resetclipwin();
            TFT_fillScreen(TFT_WHITE);
            TFT_print("Click!", CENTER, (M5DISPLAY_HEIGHT-24)/2);
            vTaskDelay(1000/portTICK_PERIOD_MS);
            TFT_fillScreen(TFT_WHITE);
        break;
        case M5BUTTON_BUTTON_A_HOLD:
            TFT_resetclipwin();
            TFT_fillScreen(TFT_WHITE);
            TFT_print("Hold!", CENTER, (M5DISPLAY_HEIGHT-24)/2);
            vTaskDelay(1000/portTICK_PERIOD_MS);
            TFT_fillScreen(TFT_WHITE);
        break;
    }
}

void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    // Initialize M5StickC
    // This initializes the event loop, power, button and display
    m5_init();

    // Register for button events
    esp_event_handler_register_with(m5_event_loop, M5BUTTON_EVENT_BASE, M5BUTTON_BUTTON_A_CLICK, my_m5_event_handler, NULL);
    esp_event_handler_register_with(m5_event_loop, M5BUTTON_EVENT_BASE, M5BUTTON_BUTTON_A_HOLD, my_m5_event_handler, NULL);

    font_rotate = 0;
    text_wrap = 0;
    font_transparent = 0;
    font_forceFixed = 0;
    gray_scale = 0;
    TFT_setGammaCurve(DEFAULT_GAMMA_CURVE);
    TFT_setRotation(LANDSCAPE);
    TFT_setFont(DEFAULT_FONT, NULL);
    TFT_resetclipwin();
    TFT_fillScreen(TFT_WHITE);
    _bg = TFT_WHITE;
    _fg = TFT_MAGENTA;
    char backlight_str[6];
    vTaskDelay(5000/portTICK_PERIOD_MS);

    ESP_LOGD(TAG, "Turning backlight off");
    m5display_off();
    vTaskDelay(5000/portTICK_PERIOD_MS);
    ESP_LOGD(TAG, "Turning backlight on");
    m5display_on();
    vTaskDelay(5000/portTICK_PERIOD_MS);

    // Backlight level test
    for(uint8_t i=7; i>0; --i) {
        m5display_set_backlight_level(i);
        TFT_fillScreen(TFT_WHITE);
        sprintf(backlight_str, "%d", i);
        TFT_print("Backlight test", CENTER, (M5DISPLAY_HEIGHT-24)/2 +12);
        TFT_print(backlight_str, CENTER, (M5DISPLAY_HEIGHT-24)/2 -12);
        ESP_LOGD(TAG, "Backlight: %d", i);
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    for(uint8_t i=0; i<=7; ++i) {
        m5display_set_backlight_level(i);
        TFT_fillScreen(TFT_WHITE);
        sprintf(backlight_str, "%d", i);
        TFT_print("Backlight test", CENTER, (M5DISPLAY_HEIGHT-24)/2 +12);
        TFT_print(backlight_str, CENTER, (M5DISPLAY_HEIGHT-24)/2 -12);
        ESP_LOGD(TAG, "Backlight: %d", i);
        vTaskDelay(500/portTICK_PERIOD_MS);
    }

    // Test buttons
    TFT_fillScreen(TFT_WHITE);
    TFT_print("Press or hold button", CENTER, (M5DISPLAY_HEIGHT-24)/2);
}

