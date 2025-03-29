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

#include "system/features.h"
#include "system/FullSystem.h"

extern "C"
{
  void app_main()
  {
    ESP_LOGI("Running", "Initializing...");

    FullSystem fullSystem;

    ESP_LOGI("Running", "Initialization done. System is now starting..");

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