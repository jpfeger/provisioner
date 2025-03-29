#pragma once

#include <thread>
#include <vector>
#include <functional>
#include <optional>
#include <memory>
#include <map>

#include "freertos/FreeRTOS.h"

#include "commands/IAbstractCommand.h"
#include "utilities/CommunicationType.h"

enum class DeviceTypeEnum
{
  None = 0,
  SwitchController = 1,
  LocoController = 2,
  Cellphone = 3
};

enum class SystemStateId
{
  NotBuilt = 1,

  InitializationStart = 10,
  InitializationModeSet = 12,
  InitializationTasks = 13,
  InitializationRegisters = 14,

  // Provisioning
  BleStartProvisioning = 20,
  BleProvisioning = 21,
  BleProvisioned = 22,
  WifiConnectionFailure = 33,

  Failure = 80,
  Down = 81
};

// Forward declarations
class ICommunicator;
class ISystemExecuter;
class Node;

// Definitions
using CommandCallback = std::function<bool(const IAbstractCommand &)>;
using CommandCallbackPair = std::pair<std::reference_wrapper<ISystemExecuter>, CommandCallback>;
using CommandCallbackPairs = std::vector<CommandCallbackPair>;
using CommandCallbackTuple = std::tuple<CommandType, ISystemExecuter *, CommandCallback>;
using CommandCallbackTuples = std::vector<CommandCallbackTuple>;

using PeriodicCallback = std::function<void()>;
using PeriodicCallbacks = std::vector<PeriodicCallback>;
using PeriodicThisCallback = std::pair<std::reference_wrapper<ISystemExecuter>, PeriodicCallback>;
using PeriodicThisCallbacks = std::vector<PeriodicThisCallback>;
using PeriodicThisCallbacksPair = std::pair<TickType_t, PeriodicThisCallbacks>;

class IFullSystem
{
public:
  // Constructors, copiers, and destructor

  IFullSystem() = default;

  virtual ~IFullSystem() = default;

  IFullSystem(const IFullSystem &rhs) = default;

  IFullSystem &operator=(const IFullSystem &rhs) = default;

  // Getter and setter

  virtual const DeviceTypeEnum &getDeviceType() const = 0;

  virtual const SystemStateId &getCurrentState() const = 0;

  virtual void setCurrentState(const SystemStateId &iState) = 0;

  virtual bool isInitializing() const = 0;

  virtual bool isProvisioning() const = 0;

  virtual bool isOperational() const = 0;

  virtual bool isDown() const = 0;

  // Various methods

  virtual void registerToCommands(const CommandCallbackTuples &iTuples) = 0;

  virtual void unregisterToCommands(ISystemExecuter *iExecuter) = 0;

  virtual void registerPeriodicCall(ISystemExecuter *iExecuter, TickType_t iPeriodicity, const PeriodicCallback &iCallback) = 0;

  virtual void unregisterPeriodicCall(ISystemExecuter *iExecuter) = 0;

  virtual void pushCommand(IAbstractCommand *iCommandPtr) = 0;

  virtual void pushCommand(std::unique_ptr<IAbstractCommand> &iCommandPtr) = 0;

  virtual void start() = 0;
};
