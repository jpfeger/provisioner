#pragma once

#include <optional>
#include <memory>

#include "ArduinoJson.h"
#include "utilities/CommunicationType.h"

enum class CommunicatorStateId
{
  // All communicators
  NotConnected = 1, // At start up or when connection failed

  // Wifi
  WifiStarting = 2,             // Right after receiving command to start connection
  WifiScanningAccessPoints = 3, // When opening a wifi connection
  WifiIpAddressAssigned = 4,
  WifiStartupFailure = 5, 

  // Wifi provisioner
  ProvisionerUninitialized = 21,
  ProvisionerInitialized = 22,
  Provisioned = 23,
  ProvisionerToBeProvisioned = 24,
  ProvisionerStartProvisioning = 25,
  ProvisionerEndProvisioning = 26,
};

// Forward declaration
class Node;

class ICommunicator
{
public:
  // Constructors, copiers, and destructor

  ICommunicator() = default;

  virtual ~ICommunicator() = default;

  ICommunicator(const ICommunicator &rhs) = default;

  ICommunicator &operator=(const ICommunicator &rhs) = default;

  // Operators

  virtual bool operator==(const ICommunicator &rhs) const = 0;

  virtual bool operator!=(const ICommunicator &rhs) const = 0;

  // Getter and setter

  virtual const CommunicatorStateId &getState() const = 0;

  virtual void setState(const CommunicatorStateId &iState) = 0;

  virtual const CommunicationTypeEnum &getCommunicationType() const = 0;

  virtual const std::shared_ptr<const Node> &getIdentityNode() const = 0;

  virtual void setIdentityNode(const std::shared_ptr<const Node> &iIdentityNode) = 0;
};
