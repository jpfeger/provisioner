#pragma once

#include <pthread.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "link/communicator/ISystemCommunicator.h"
#include "system/IFullSystem.h"
#include "system/features.h"

class AbstractCommunicator : virtual public ISystemCommunicator
{
public:
  // Constructors, copiers, and destructor

  AbstractCommunicator(IFullSystem &iFullSystem, const CommunicationTypeEnum &iCommunicationType);

  AbstractCommunicator() = delete;

  ~AbstractCommunicator() = default;

  AbstractCommunicator(const AbstractCommunicator &rhs) = default;

  AbstractCommunicator &operator=(const AbstractCommunicator &rhs) = default;

  // Interface ICommunicator

  const CommunicatorStateId &getState() const override;

  void setState(const CommunicatorStateId &iState) override;

  const CommunicationTypeEnum &getCommunicationType() const override;

  const std::shared_ptr<const Node> &getIdentityNode() const override;

  void setIdentityNode(const std::shared_ptr<const Node> &iIdentityNode) override;

  // Interface ISystemExecuter

  void initializeCommandHandlerCalls() override;

  bool initializeTasks() override;

  void initializePeriodicCalls() override;

  bool wrapUp() override;

  // Getter and setter

  IFullSystem &getSystem();

  bool isEnabled() const;

protected:
  bool mIsEnabled; // When true is active
  CommunicatorStateId mState;
  std::reference_wrapper<IFullSystem> mSystem;
  CommunicationTypeEnum mCommunicationType;  // Wifi or Bluetooth
  std::shared_ptr<const Node> mIdentityNode; // Internal node for broadcasting, external node otherwise
};
