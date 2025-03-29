#pragma once

#include <map>
#include <list>
#include <pthread.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/sockets.h"
#include "ArduinoJson.h"

#include "system/IFullSystem.h"
#include "link/wifi/AbstractWifiCommunicator.h"

class WifiProvisioner : virtual public AbstractWifiCommunicator
{
public:
  // Constructors, copiers, and destructor

  WifiProvisioner(IFullSystem &iFullSystem);

  ~WifiProvisioner() = default;

  WifiProvisioner(const WifiProvisioner &rhs) = default;

  WifiProvisioner &operator=(const WifiProvisioner &rhs) = default;

  // Interface ISystemExecuter

  void initializeCommandHandlerCalls() override;

  bool initializeTasks() override;

  void initializePeriodicCalls() override;

  bool wrapUp() override;

private:
  // Private methods

  static void eventHandler(void *iArg, esp_event_base_t iEventBase, int32_t iEventId, void *iEventData);

  bool handleWifiProvisioningCommand(const IAbstractCommand &iCommand);

  void provisioning();
};
