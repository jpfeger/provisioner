#include <stdio.h>
#include <string.h>
#include <memory>
#include <algorithm>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "ArduinoJson.h"
#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "link/wifi/AbstractWifiCommunicator.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define CONFIG_LWIP_NETBUF_RECVINFO

// Static members
EventGroupHandle_t AbstractWifiCommunicator::mWifiEventGroup = nullptr;
bool AbstractWifiCommunicator::mIsProvisioned = false;

AbstractWifiCommunicator::AbstractWifiCommunicator(IFullSystem &iFullSystem)
    : AbstractCommunicator(iFullSystem, CommunicationTypeEnum::Wifi),
      mControllerNetInfo(),
      mNumberOfWifiConnection(0),
      mNumberOfWifiPassword(0)
{
}

// Interface ICommunicator

bool AbstractWifiCommunicator::operator==(const ICommunicator &rhs) const
{
  bool result = false;

  if (rhs.getCommunicationType() == mCommunicationType)
  {
    const IWifiCommunicator *communicator = dynamic_cast<const IWifiCommunicator *>(&rhs);

    if (communicator != nullptr)
    {
      result = communicator->getIpAddress() == getIpAddress();
    }
    else
    {
      ESP_LOGE("Communicating", "AbstractWifiCommunicator::operator== cannot dynamic cast to IWifiCommunicator pointer!");
    }
  }

  return result;
}

bool AbstractWifiCommunicator::operator!=(const ICommunicator &rhs) const
{
  return !(*this == rhs);
}


// Interface ISystemExecuter

bool AbstractWifiCommunicator::initializeTasks()
{
  bool result(true);

  if (AbstractCommunicator::initializeTasks())
  {
    ESP_LOGI("Communicating", "AbstractWifiCommunicator.initializeTasks");

    mWifiEventGroup = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &AbstractWifiCommunicator::eventHandler, this,
                                                        &mWifiEventHandlerInstance));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &AbstractWifiCommunicator::eventHandler, this,
                                                        &mIpEventHandlerInstance));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &AbstractWifiCommunicator::eventHandler, this,
                                                        &mProvisioningEventHandlerInstance));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(PROTOCOMM_TRANSPORT_BLE_EVENT, ESP_EVENT_ANY_ID, &AbstractWifiCommunicator::eventHandler, this, &mProtocommTransportEventHandlerInstance));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(PROTOCOMM_SECURITY_SESSION_EVENT, ESP_EVENT_ANY_ID, &AbstractWifiCommunicator::eventHandler, this, &mProtocommSecurityEventHandlerInstance));
  }
  else
  {
    result = false;
  }

  return result;
}

void AbstractWifiCommunicator::initializeCommandHandlerCalls()
{
  AbstractCommunicator::initializeCommandHandlerCalls();
}

void AbstractWifiCommunicator::initializePeriodicCalls()
{
  AbstractCommunicator::initializePeriodicCalls();
}

bool AbstractWifiCommunicator::wrapUp()
{
  ESP_LOGI("Running", "AbstractWifiCommunicator.wrapUp");

  AbstractCommunicator::wrapUp();

  esp_err_t error = esp_wifi_stop();

  if (error != ESP_ERR_WIFI_NOT_INIT)
  {
    ESP_ERROR_CHECK(error);
    ESP_ERROR_CHECK(esp_wifi_deinit());

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &mWifiEventHandlerInstance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &mWifiEventHandlerInstance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &mProvisioningEventHandlerInstance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &mProtocommSecurityEventHandlerInstance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &mProtocommTransportEventHandlerInstance));
  }

  return (error == ESP_OK);
}

// Interface IWifiCommunicator

in_addr_t AbstractWifiCommunicator::getIpAddress() const
{
  return mControllerNetInfo.ip.addr;
}

void AbstractWifiCommunicator::setIpAddress(in_addr_t &iIpAddress)
{
  mControllerNetInfo.ip.addr = iIpAddress;
}

// Getter and setter

int AbstractWifiCommunicator::getNumberOfWifiConnection() const
{
  return mNumberOfWifiConnection;
}

void AbstractWifiCommunicator::setNumberOfWifiConnection(int iNumberOfAttempt)
{
  mNumberOfWifiConnection = iNumberOfAttempt;
}

int AbstractWifiCommunicator::getNumberOfWifiPassword() const
{
  return mNumberOfWifiPassword;
}

void AbstractWifiCommunicator::setNumberOfPassword(int iNumberOfAttempt)
{
  mNumberOfWifiPassword = iNumberOfAttempt;
}

void AbstractWifiCommunicator::setControllerNetInfo(const esp_netif_ip_info_t &iNetInfo)
{
  mControllerNetInfo = iNetInfo;
}

EventGroupHandle_t &AbstractWifiCommunicator::getWifiEventGroupHandle()
{
  return mWifiEventGroup;
}

bool AbstractWifiCommunicator::isProvisioned()
{
  return mIsProvisioned;
}

// Public static method

void AbstractWifiCommunicator::initializeWifi()
{
  ESP_LOGI("Running", "AbstractWifiCommunicator.initializeWifi");

  ESP_ERROR_CHECK(esp_netif_init());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifiInitConfig = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifiInitConfig));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

  ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&mIsProvisioned));
  ESP_LOGI("Communicating", "AbstractWifiCommunicator.initializeWifi provisioned state read='%s'",
           mIsProvisioned ? "provisioned" : "NOT provisioned");
}

// Private methods

void AbstractWifiCommunicator::eventHandler(void *iArg, esp_event_base_t iEventBase,
                                            int32_t iEventId, void *iEventData)
{
  AbstractWifiCommunicator *communicatorPointer = (AbstractWifiCommunicator *)iArg;

  if (iEventBase == WIFI_EVENT)
  {
    switch (iEventId)
    {
    case WIFI_EVENT_WIFI_READY:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler Wifi is ready");
      break;

    case WIFI_EVENT_STA_START:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler WIFI_EVENT start connection");
      esp_wifi_connect();
      communicatorPointer->setNumberOfWifiConnection(0);
      communicatorPointer->setState(CommunicatorStateId::WifiStarting);
      break;

    case WIFI_EVENT_STA_DISCONNECTED:
    {
      wifi_event_sta_disconnected_t *eventData = (wifi_event_sta_disconnected_t *)iEventData;

      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler WIFI_EVENT connect to the access point failed (reason=%d)",
               eventData->reason);

      int numberOfAttempts = communicatorPointer->getNumberOfWifiConnection();

      if (++numberOfAttempts < MAX_WIFI_CONNECTION_RETRY)
      {
        esp_wifi_connect();
        communicatorPointer->setNumberOfWifiConnection(numberOfAttempts);

        ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler WIFI_EVENT retry to connect to the access point");
      }
      else
      {
        xEventGroupSetBits(communicatorPointer->getWifiEventGroupHandle(), WIFI_FAIL_BIT);
        communicatorPointer->setState(CommunicatorStateId::WifiStartupFailure);
        communicatorPointer->getSystem().setCurrentState(SystemStateId::WifiConnectionFailure);

        ESP_LOGE("Communicating", "AbstractWifiCommunicator::eventHandler all %d attempts to connect failed", MAX_WIFI_CONNECTION_RETRY);
      }
    }
    break;

    case WIFI_EVENT_SCAN_DONE:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler scan of stations done.");
      break;

    case WIFI_EVENT_STA_STOP:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler Wifi service stopped.");
      break;

    case WIFI_EVENT_STA_CONNECTED:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler connection established.");
      break;

    default:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler default WIFI_EVENT event id = %d",
               (int)iEventId);
      break;
    }
  }
  else if (iEventBase == IP_EVENT)
  {
    switch (iEventId)
    {
    case IP_EVENT_STA_GOT_IP:
    {
      ip_event_got_ip_t *event = (ip_event_got_ip_t *)iEventData;

      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler allocated ip " IPSTR, IP2STR(&event->ip_info.ip));

      communicatorPointer->setControllerNetInfo(event->ip_info);
      communicatorPointer->setNumberOfWifiConnection(0);
      communicatorPointer->setState(CommunicatorStateId::WifiIpAddressAssigned);
      xEventGroupSetBits(communicatorPointer->getWifiEventGroupHandle(), WIFI_CONNECTED_BIT);
    }
    break;

    default:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler default IP_EVENT event id = %d",
               (int)iEventId);
      break;
    }
  }
  else if (iEventBase == PROTOCOMM_TRANSPORT_BLE_EVENT)
  {
    switch (iEventId)
    {
    case PROTOCOMM_TRANSPORT_BLE_CONNECTED:
      ESP_LOGI("Provisioning", "AbstractWifiCommunicator::eventHandler BLE transport: Connected!");
      break;

    case PROTOCOMM_TRANSPORT_BLE_DISCONNECTED:
      ESP_LOGI("Provisioning", "AbstractWifiCommunicator::eventHandler BLE transport: Disconnected!");
      break;

    default:
      break;
    }
  }
  else if (iEventBase == WIFI_PROV_EVENT)
  {
    switch (iEventId)
    {
    case WIFI_PROV_INIT:
      ESP_LOGI("Provisioning", "AbstractWifiCommunicator::eventHandler initializing manager.");
      break;

    case WIFI_PROV_START:
      ESP_LOGI("Provisioning", "AbstractWifiCommunicator::eventHandler starting provisioing");
      communicatorPointer->setNumberOfPassword(0);
      break;

    case WIFI_PROV_CRED_RECV:
    {
      wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)iEventData;
      ESP_LOGI("Provisioning", "AbstractWifiCommunicator::eventHandler received Wi-Fi credentials SSID: %s Password : %s",
               (const char *)wifi_sta_cfg->ssid,
               (const char *)wifi_sta_cfg->password);
      communicatorPointer->setNumberOfPassword(0);
      break;
    }

    case WIFI_PROV_CRED_FAIL:
    {
      wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)iEventData;
      ESP_LOGE("Provisioning", "AbstractWifiCommunicator::eventHandler Provisioning failed! reason : %s",
               (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");

      int numberOfAttempts = communicatorPointer->getNumberOfWifiPassword();

      if (++numberOfAttempts < MAX_WIFI_PASSWORD_RETRY)
      {
        communicatorPointer->setNumberOfPassword(numberOfAttempts);
      }
      else
      {
        ESP_LOGI("Provisioning", "AbstractWifiCommunicator::eventHandler Failed to connect with provisioned AP, reseting provisioned credentials");

        // Erase every wifi setting
        // wifi_prov_mgr_reset_sm_state_on_failure();
        // wifi_prov_mgr_reset_sm_state_for_reprovision();
        esp_wifi_restore();
      }
      break;
    }

    case WIFI_PROV_CRED_SUCCESS:
      ESP_LOGI("Provisioning", "AbstractWifiCommunicator::eventHandler provisioning successful");
      communicatorPointer->setNumberOfPassword(0);
      break;

    case WIFI_PROV_END:
      ESP_LOGI("Provisioning", "AbstractWifiCommunicator::eventHandler deinitializing manager.");
      wifi_prov_mgr_deinit();
      communicatorPointer->getSystem().setCurrentState(SystemStateId::BleProvisioned);
      break;

    default:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler default WIFI_PROV_EVENT event id = %d",
               (int)iEventId);
      break;
    }
  }
  else if (iEventBase == PROTOCOMM_SECURITY_SESSION_EVENT)
  {
    switch (iEventId)
    {
    case PROTOCOMM_SECURITY_SESSION_SETUP_OK:
      ESP_LOGI("Communicating", "AbstractWifiCommunicator::eventHandler Secured session established!");
      break;

    case PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS:
      ESP_LOGE("Communicating", "AbstractWifiCommunicator::eventHandler eceived invalid security parameters for establishing secure session!");
      break;

    case PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH:
      ESP_LOGE("Communicating", "AbstractWifiCommunicator::eventHandler Received incorrect username and/or PoP for establishing secure session!");
      break;

    default:
      break;
    }
  }
}
