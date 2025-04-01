#include <lwip/sockets.h>
#include <esp_log.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "system/features.h"
#include "system/FullSystem.h"

extern "C"
{
  void app_main()
  {
    ESP_LOGI("Running", "Initializing...");

    FullSystem fullSystem;

    ESP_LOGI("Running", "Initialization done. System is now starting..");

    esp_err_t result = nvs_flash_init();

    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
      ESP_LOGI("Running", "InitFlash NVS partition was truncated and needs to be erased.");

      ESP_ERROR_CHECK(nvs_flash_erase());
      result = nvs_flash_init();

      ESP_ERROR_CHECK(result);
    }

    fullSystem.start();

    while (true)
    {
      // For watchdog
      const TickType_t duration = (100 / portTICK_PERIOD_MS);
      vTaskDelay(duration);
    }

    fullSystem.wrapUp();

    ESP_LOGI("Running", "Exiting the system.");
  }
}