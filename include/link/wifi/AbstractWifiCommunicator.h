#pragma once

#include <map>
#include <optional>
#include <pthread.h>
#include <memory>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "link/communicator/AbstractCommunicator.h"
#include "link/wifi/IWifiCommunicator.h"

class AbstractWifiCommunicator : virtual public AbstractCommunicator, virtual public IWifiCommunicator
{
public:
  // Constructors, copiers, and destructor

  AbstractWifiCommunicator(IFullSystem &iFullSystem);

  AbstractWifiCommunicator() = delete;

  ~AbstractWifiCommunicator() = default;

  AbstractWifiCommunicator(const AbstractWifiCommunicator &rhs) = default;

  AbstractWifiCommunicator &operator=(const AbstractWifiCommunicator &rhs) = default;

  // Interface ICommunicator

  bool operator==(const ICommunicator &rhs) const override;

  bool operator!=(const ICommunicator &rhs) const override;

  // Interface IWifiCommunicator

  in_addr_t getIpAddress() const override;

  void setIpAddress(in_addr_t &iIpAddress) override;

  // Interface ISystemExecuter

  void initializeCommandHandlerCalls();

  void initializePeriodicCalls();

  bool initializeTasks();

  bool wrapUp();

  // Getter and setter

  int getNumberOfWifiConnection() const;

  void setNumberOfWifiConnection(int iNumberOfAttempt);

  int getNumberOfWifiPassword() const;

  void setNumberOfPassword(int iNumberOfAttempt);

  void setControllerNetInfo(const esp_netif_ip_info_t &iNetInfo);

public:

  static void initializeWifi();

  static bool isProvisioned();

  static EventGroupHandle_t &getWifiEventGroupHandle();

protected:
  // Static method

  static void eventHandler(void *iArg, esp_event_base_t iEventBase, int32_t iEventId, void *iEventData);

  // Not saved in configuration
  esp_netif_ip_info_t mControllerNetInfo;

private:
  // Members not saved in configuration
  static EventGroupHandle_t mWifiEventGroup;
  static bool mIsProvisioned;

  esp_event_handler_instance_t mWifiEventHandlerInstance;
  esp_event_handler_instance_t mIpEventHandlerInstance;
  esp_event_handler_instance_t mProvisioningEventHandlerInstance;
  esp_event_handler_instance_t mProtocommSecurityEventHandlerInstance;
  esp_event_handler_instance_t mProtocommTransportEventHandlerInstance;
  int mNumberOfWifiConnection;
  int mNumberOfWifiPassword;
};
