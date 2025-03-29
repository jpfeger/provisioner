#pragma once

#include <memory>

// Forward declaration
class CommandRequester;

enum class CommandType
{
  InvalidCommand = 1,

  WifiProvisioningCommand = 100,    // Internal command
  StateNotificationCommand = 106,   // Internal command: notification of various internal state of the system
};

enum class CommandStatus
{
  Failure = 1,
  Abort = 2,
  Pending = 3,
  Success = 4,
};

enum class CommandState
{
  None = 0,
  Created = 1,
  PendingExecution = 2,
  BeingExecuted = 3,
  Executed = 4
};

class IAbstractCommand
{
public:
  // Constructors, copiers, and destructor

  IAbstractCommand() = default;

  virtual ~IAbstractCommand() = default;

  IAbstractCommand(const IAbstractCommand &rhs) = default;

  IAbstractCommand &operator=(const IAbstractCommand &rhs) = default;

  // Getters and setters

  virtual CommandState getState() const = 0;

  virtual void setState(CommandState iState) = 0;

  virtual CommandType getType() const = 0;

  virtual const std::shared_ptr<const CommandRequester> &getRequester() const = 0;
};
