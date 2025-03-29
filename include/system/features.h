#pragma once

// Task priorities
#define SYSTEM_TASK_PRIORITY 1

#define MAX_WIFI_CONNECTION_RETRY 5
#define MAX_WIFI_PASSWORD_RETRY 5

#define PERIODIC_CALL_UNIT_MS (TickType_t)(20 / portTICK_PERIOD_MS) // 20 miliseconds, minimum periodic call period
#define SYSTEM_ACTIVITY_LOG_TIMEOUT (500)                           // 10 seconds logging periodicity
