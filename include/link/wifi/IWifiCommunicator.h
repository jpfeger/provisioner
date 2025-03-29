#pragma once

#include <optional>
#include <memory>

#include "lwip/sockets.h"

#include "link/communicator/ICommunicator.h"

class IWifiCommunicator : virtual public ICommunicator
{
public:
  // Constructors, copiers, and destructor

  IWifiCommunicator() = default;

  virtual ~IWifiCommunicator() = default;

  IWifiCommunicator(const IWifiCommunicator &rhs) = default;

  IWifiCommunicator &operator=(const IWifiCommunicator &rhs) = default;

  // Getter and setter

  virtual in_addr_t getIpAddress() const = 0;

  virtual void setIpAddress(in_addr_t &iIpAddress) = 0;
};