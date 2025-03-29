#pragma once

#include <string>
#include <memory>
#include "ArduinoJson.h"

#include "system/IFullSystem.h"

namespace internal
{
     extern std::shared_ptr<Node> deviceNode;
}

// Forward declaration
class ISystemCommunicator;

class Node
{
public:
     // Constructors, copiers, and destructor

     Node(const DeviceTypeEnum &iDeviceType,
          unsigned long iUid,
          const std::string &iFullName);

     Node(const DeviceTypeEnum &iDeviceType,
          unsigned long iUid,
          const std::string &iFullName,
          const std::shared_ptr<ISystemCommunicator> &iCommunicator);

     Node(const DeviceTypeEnum &iDeviceType,
          unsigned long iUid,
          const std::string &iFullName,
          const std::string &iShortName,
          const std::string &iIconColor);

     Node(const DeviceTypeEnum &iDeviceType,
          unsigned long iUid,
          const std::string &iFullName,
          const std::string &iShortName,
          const std::string &iIconColor,
          const std::shared_ptr<ISystemCommunicator> &iCommunicator);

     ~Node() = default;

     Node(const Node &rhs) = default;

     Node &operator=(const Node &rhs) = default;

     // Public methods

     bool operator==(const Node &rhs) const;

     bool operator!=(const Node &rhs) const;

     const DeviceTypeEnum &getDeviceType() const;

     void setDeviceType(const DeviceTypeEnum &iType);

     unsigned long getUid() const;

     void setUid(unsigned long iUid);

     const std::string &getFullName() const;

     void setFullName(const std::string &iFullName);

     const std::string &getShortName() const;

     void setShortName(const std::string &iShortName);

     const std::string &getIconColor() const;

     void setIconColor(const std::string &iIconColor);

     const std::shared_ptr<ISystemCommunicator> &getCommunicator() const;

     void setCommunicator(const std::shared_ptr<ISystemCommunicator> &iCommunicator);

     std::string toString() const;

private:
     // Private members

     DeviceTypeEnum mDeviceType;
     unsigned long mUid;     // Either the MAC address for Wifi or BD_ADDR for Bluetooth
     std::string mFullName;  // Fully explanatory name displayed for this device
     std::string mShortName; // Acronym displayed for this device
     std::string mIconColor; // Color of the icons for this device
     std::shared_ptr<ISystemCommunicator> mCommunicator;
};
