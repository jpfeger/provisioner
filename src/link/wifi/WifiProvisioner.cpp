#include <stdio.h>
#include <string.h>
#include <memory>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_log.h>
#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>
#include <protocomm_ble.h>

#include "link/wifi/WifiProvisioner.h"
#include "commands/TemplatedCommands.h"

WifiProvisioner::WifiProvisioner(IFullSystem &iFullSystem)
    : AbstractCommunicator(iFullSystem, CommunicationTypeEnum::Wifi),
      AbstractWifiCommunicator(iFullSystem)
{
}

// Interface ISystemExecuter

bool WifiProvisioner::initializeTasks()
{
  return AbstractWifiCommunicator::initializeTasks();
}

void WifiProvisioner::initializeCommandHandlerCalls()
{
  ESP_LOGI("Running", "Provisioner.initializeCommandHandlerCalls");

  AbstractWifiCommunicator::initializeCommandHandlerCalls();

  mSystem.get().registerToCommands(
      {
          {CommandType::WifiProvisioningCommand, this, std::bind(&WifiProvisioner::handleWifiProvisioningCommand, this, std::placeholders::_1)},
      });
}

void WifiProvisioner::initializePeriodicCalls()
{
  AbstractWifiCommunicator::initializePeriodicCalls();
}

bool WifiProvisioner::wrapUp()
{
  ESP_LOGI("Running", "WifiProvisioner.wrapUp");

  return AbstractWifiCommunicator::wrapUp();
}

// Private methods

bool WifiProvisioner::handleWifiProvisioningCommand(const IAbstractCommand &iCommand)
{
  bool result(false);

  const WifiProvisioningCommand *command = dynamic_cast<const WifiProvisioningCommand *>(&iCommand);

  if (command != nullptr)
  {
    if (command->getData())
    {
      ESP_LOGI("Running", "Provisioner::handleWifiProvisioningCommand starting provisioning.");

      provisioning();
    }
  }

  return result;
}

void WifiProvisioner::provisioning()
{
  ESP_LOGI("Provisioning", "Provisioner::provisioning");

  // Configuration for the provisioning manager
  wifi_prov_mgr_config_t config =
      {
          .scheme = wifi_prov_scheme_ble,
          .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM};

  ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

  const std::string &nodeName(internal::deviceNode->getFullName());
  ESP_LOGI("Provisioning", "Provisioner::provisioning service name '%s'", nodeName.c_str());

  wifi_prov_security_t securityLevel = WIFI_PROV_SECURITY_1;
  const char *proofOfPossession = "abcd1234";
  wifi_prov_security1_params_t *securityParameter = proofOfPossession;

  const char *serviceKey = NULL; // ignored when scheme is wifi_prov_scheme_ble
  uint8_t customServiceUuid[] =
      {
          0xb4,
          0xdf,
          0x5a,
          0x1c,
          0x3f,
          0x6b,
          0xf4,
          0xbf,
          0xea,
          0x4a,
          0x82,
          0x03,
          0x04,
          0x90,
          0x1a,
          0x02,
      };

  ESP_LOGI("Provisioning", "Provisioner::provisioning set service Uuid");
  wifi_prov_scheme_ble_set_service_uuid(customServiceUuid);

  ESP_LOGI("Provisioning", "Provisioner::provisioning start provisioning...");
  getSystem().setCurrentState(SystemStateId::BleProvisioning);
  ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(securityLevel, (const void *)securityParameter, nodeName.c_str(), serviceKey));
}
