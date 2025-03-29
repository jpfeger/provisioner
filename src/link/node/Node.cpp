
#include "esp_err.h"
#include "esp_log.h"
#include "link/node/Node.h"
#include "system/FullSystem.h"
#include "link/communicator/AbstractCommunicator.h"

namespace internal
{
  std::shared_ptr<Node> deviceNode = std::make_shared<Node>(DeviceTypeEnum::None, 12345678, "SwitchControllerNode");
}

Node::Node(const DeviceTypeEnum &iDeviceType,
           unsigned long iUid,
           const std::string &iFullName) : Node(iDeviceType, iUid, iFullName, "", "", nullptr)
{
}

Node::Node(const DeviceTypeEnum &iDeviceType,
           unsigned long iUid,
           const std::string &iFullName,
           const std::shared_ptr<ISystemCommunicator> &iCommunicator) : Node(iDeviceType, iUid, iFullName, "", "", iCommunicator)
{
}

Node::Node(const DeviceTypeEnum &iDeviceType,
           unsigned long iUid,
           const std::string &iFullName,
           const std::string &iShortName,
           const std::string &iIconColor) : Node(iDeviceType, iUid, iFullName, iShortName, iIconColor, nullptr)
{
}

Node::Node(const DeviceTypeEnum &iDeviceType,
           unsigned long iUid,
           const std::string &iFullName,
           const std::string &iShortName,
           const std::string &iIconColor,
           const std::shared_ptr<ISystemCommunicator> &iCommunicator) : mDeviceType(iDeviceType), mUid(iUid),
                                                                        mFullName(iFullName), mShortName(iShortName), mIconColor(iIconColor),
                                                                        mCommunicator(iCommunicator)
{
}

bool Node::operator==(const Node &rhs) const
{
  return mDeviceType == rhs.getDeviceType() && mUid == rhs.getUid();
}

bool Node::operator!=(const Node &rhs) const
{
  return !(*this == rhs);
}

const DeviceTypeEnum &Node::getDeviceType() const
{
  return mDeviceType;
}

void Node::setDeviceType(const DeviceTypeEnum &iType)
{
  mDeviceType = iType;
}

unsigned long Node::getUid() const
{
  return mUid;
}

void Node::setUid(unsigned long iUid)
{
  mUid = iUid;
}

const std::string &Node::getFullName() const
{
  return mFullName;
}

void Node::setFullName(const std::string &iFullName)
{
  mFullName = iFullName;
}

const std::string &Node::getShortName() const
{
  return mShortName;
}

void Node::setShortName(const std::string &iShortName)
{
  mShortName = iShortName;
}

const std::string &Node::getIconColor() const
{
  return mIconColor;
}

void Node::setIconColor(const std::string &iIconColor)
{
  mIconColor = iIconColor;
}

const std::shared_ptr<ISystemCommunicator> &Node::getCommunicator() const
{
  return mCommunicator;
}

void Node::setCommunicator(const std::shared_ptr<ISystemCommunicator> &iCommunicator)
{
  mCommunicator = iCommunicator;
}

std::string Node::toString() const
{
  std::string result = "uid=" + std::to_string(mUid) + " full name=" + mFullName + " short name=" + mShortName + " icon color=" + mIconColor;

  return result;
}