#pragma once

#include <thread>
#include <vector>
#include <map>
#include <pthread.h>
#include <memory>

#include "esp_system.h"
#include "ArduinoJson.h"

#include "system/IFullSystem.h"
#include "link/wifi/WifiProvisioner.h"
#include "link/communicator/ICommunicator.h"

// Forward declarations
class CommandRequester;
class IExecutiveCommunicator;

class FullSystem : virtual public IFullSystem, virtual public ISystemExecuter
{
public:
  // Constructors, copiers, and destructor

  FullSystem();

  ~FullSystem() = default;

  FullSystem(const FullSystem &rhs) = default;

  FullSystem &operator=(const FullSystem &rhs) = default;

  // Interface IFullSystem

  const DeviceTypeEnum &getDeviceType() const;

  const SystemStateId &getCurrentState() const override;

  void setCurrentState(const SystemStateId &iState) override;

  bool isInitializing() const override;

  bool isProvisioning() const override;

  bool isOperational() const override;

  bool isDown() const override;

  void registerToCommands(const CommandCallbackTuples &iTuples) override;

  void unregisterToCommands(ISystemExecuter *iInitializer) override;

  void registerPeriodicCall(ISystemExecuter *iInitializer, TickType_t iPeriodicity, const PeriodicCallback &iCallback) override;

  void unregisterPeriodicCall(ISystemExecuter *iInitializer) override;

  void pushCommand(std::unique_ptr<IAbstractCommand> &iCommandPtr) override;

  void pushCommand(IAbstractCommand *iCommand) override;

  void start();

  // Interface ISystemExecuter

  bool initializeTasks() override;

  void initializeCommandHandlerCalls() override;

  void initializePeriodicCalls() override;

  bool wrapUp() override;

private:
  // Private static method

  static void checkingTask(void *argument);

  // Private various methods

  void popCommands();

  void periodicCalls();

  bool handleStateNotificationCommand(const IAbstractCommand &iCommand);

  // static private members

  static pthread_mutex_t sMutexCommands;
  static pthread_mutex_t sMutexNotifs;
  static pthread_mutex_t sMutexPeriodics;

  SystemStateId mCurrentState;         // Current state of the system
  std::map<CommandType, CommandCallbackPairs> mCommandTypeToCallbacksMap;
  std::map<TickType_t, PeriodicThisCallbacksPair> mPeriodicCallMap;
  std::shared_ptr<WifiProvisioner> mWifiProvisonerPointer;
  std::list<std::unique_ptr<IAbstractCommand>> mCommands;
  TaskHandle_t mTaskHandle;    // Handle on recurrent task
};
