#include <stdio.h>
#include <string.h>
#include <memory>
#include <algorithm>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "ArduinoJson.h"
#include <wifi_provisioning/manager.h>
#include "driver/gpio.h"

#include "link/communicator/AbstractCommunicator.h"
#include "commands/TemplatedCommands.h"

// Constructors, copiers, and destructor

AbstractCommunicator::AbstractCommunicator(IFullSystem &iFullSystem,
                                           const CommunicationTypeEnum &iCommunicationType)
    : mState(CommunicatorStateId::NotConnected),
      mSystem(iFullSystem),
      mCommunicationType(iCommunicationType),
      mIdentityNode(nullptr)
{
  mIsEnabled = true;
}

std::map<CommunicatorStateId, std::pair<int, int>> mStateOnOffMap;
CommunicationTypeEnum mCommunicationType;

// Interface ICommunicator

const CommunicatorStateId &AbstractCommunicator::getState() const
{
  return mState;
}

void AbstractCommunicator::setState(const CommunicatorStateId &iState)
{
  mState = iState;

  mSystem.get().pushCommand(new StateNotificationCommand<CommunicatorStateId>(
      std::make_shared<const CommandRequester>(internal::deviceNode, false),
      mState));
}

const CommunicationTypeEnum &AbstractCommunicator::getCommunicationType() const
{
  return mCommunicationType;
}

const std::shared_ptr<const Node> &AbstractCommunicator::getIdentityNode() const
{
  return mIdentityNode;
}

void AbstractCommunicator::setIdentityNode(const std::shared_ptr<const Node> &iIdentityNode)
{
  mIdentityNode = iIdentityNode;
}

// Interface ISystemExecuter

bool AbstractCommunicator::initializeTasks()
{
  return true;
}

void AbstractCommunicator::initializeCommandHandlerCalls()
{
  // Empty
}

void AbstractCommunicator::initializePeriodicCalls()
{
  // Empty
}

bool AbstractCommunicator::wrapUp()
{
  return true;
}

// Getter and setter

IFullSystem &AbstractCommunicator::getSystem()
{
  return mSystem.get();
}

bool AbstractCommunicator::isEnabled() const
{
  return mIsEnabled;
}
