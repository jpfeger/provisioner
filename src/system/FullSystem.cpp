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
#include "esp_mac.h"

#include "system/FullSystem.h"
#include "link/wifi/WifiProvisioner.h"
#include "link/node/Node.h"
#include "commands/CommandResult.h"
#include "utilities/CommunicationType.h"
#include "commands/CommandRequester.h"
#include "commands/TemplatedCommands.h"

pthread_mutex_t FullSystem::sMutexCommands = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t FullSystem::sMutexNotifs = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t FullSystem::sMutexPeriodics = PTHREAD_MUTEX_INITIALIZER;

FullSystem::FullSystem()
    : mCurrentState(SystemStateId::NotBuilt),
      mCommandTypeToCallbacksMap(),
      mPeriodicCallMap(),
      mTaskHandle(nullptr)
{
  ESP_ERROR_CHECK(esp_event_loop_create_default());
}

// Interface ISystemExecuter

bool FullSystem::initializeTasks()
{
  ESP_LOGI("Running", "FullSystem.initializeTasks");

  mWifiProvisonerPointer.reset(new WifiProvisioner(*this));

  unsigned char macBase[6] = {0};
  unsigned long *uid = (unsigned long *)macBase;

  esp_read_mac(macBase, ESP_MAC_WIFI_STA);
  macBase[2] ^= macBase[4];
  macBase[3] ^= macBase[5];

  internal::deviceNode->setUid(*uid);
  ESP_LOGI("Running", "FullSystem.initializeTasks internal node Uuid = %x", (unsigned int)*uid);

  bool result = mWifiProvisonerPointer->initializeTasks();

  if (!result)
  {
    ESP_LOGE("Running", "FullSystem.initializeTasks FAILURE!!");
  }

  return result;
}

void FullSystem::initializeCommandHandlerCalls()
{
  ESP_LOGI("Running", "FullSystem.initializeCommandHandlerCalls");

  if (mWifiProvisonerPointer)
  {
    mWifiProvisonerPointer->initializeCommandHandlerCalls();
  }

  registerToCommands(
      {{CommandType::StateNotificationCommand, this, std::bind(&FullSystem::handleStateNotificationCommand, this, std::placeholders::_1)}});
}

void FullSystem::initializePeriodicCalls()
{
  ESP_LOGI("Running", "FullSystem.initializePeriodicCalls");

  if (mWifiProvisonerPointer)
  {
    mWifiProvisonerPointer->initializePeriodicCalls();
  }
}

bool FullSystem::wrapUp()
{
  ESP_LOGI("Running", "FullSystem.wrapUp");

  bool result = mWifiProvisonerPointer->wrapUp();

  unregisterPeriodicCall(this);

  if (mTaskHandle != nullptr)
  {
    vTaskDelete(mTaskHandle);
  }

  return result;
}

// Interface IFullSystem

const DeviceTypeEnum &FullSystem::getDeviceType() const
{
  return internal::deviceNode->getDeviceType();
}

const SystemStateId &FullSystem::getCurrentState() const
{
  return mCurrentState;
}

void FullSystem::setCurrentState(const SystemStateId &iState)
{
  mCurrentState = iState;

  pushCommand(new StateNotificationCommand<SystemStateId>(
      std::make_shared<const CommandRequester>(internal::deviceNode, false),
      mCurrentState));
}

bool FullSystem::isInitializing() const
{
  return mCurrentState == SystemStateId::InitializationStart || mCurrentState == SystemStateId::InitializationTasks || mCurrentState == SystemStateId::InitializationRegisters || mCurrentState == SystemStateId::InitializationModeSet ;
}

bool FullSystem::isProvisioning() const
{
  return mCurrentState == SystemStateId::BleProvisioning;
}

bool FullSystem::isOperational() const
{
  return false;
}

bool FullSystem::isDown() const
{
  return mCurrentState == SystemStateId::Down;
}

void FullSystem::pushCommand(IAbstractCommand *iCommand)
{
  if (pthread_mutex_lock(&sMutexCommands) == 0)
  {
    mCommands.push_back(std::unique_ptr<IAbstractCommand>(iCommand));

    pthread_mutex_unlock(&sMutexCommands);
  }
}

void FullSystem::pushCommand(std::unique_ptr<IAbstractCommand> &iCommandPtr)
{
  pushCommand(iCommandPtr.release());
}

void FullSystem::start()
{
  setCurrentState(SystemStateId::InitializationStart);

  xTaskCreate(checkingTask, "FullSystem checking Task", 12000, this, SYSTEM_TASK_PRIORITY, &mTaskHandle);
}

void FullSystem::registerToCommands(const CommandCallbackTuples &iTuples)
{
  if (pthread_mutex_lock(&sMutexNotifs) == 0)
  {
    std::for_each(iTuples.begin(), iTuples.end(), [this](CommandCallbackTuple const &tuple)
                  {
                    const CommandType type = std::get<CommandType>(tuple);
                    const CommandCallback &callBack = std::get<CommandCallback>(tuple);
                    ISystemExecuter *executer = std::get<ISystemExecuter *>(tuple);

                    const CommandCallbackPair pair = std::make_pair<>(std::ref(*executer), callBack);
                    auto foundIterator = mCommandTypeToCallbacksMap.find(type);

                    if (foundIterator != mCommandTypeToCallbacksMap.end())
                    {
                      foundIterator->second.push_back(pair);
                    }
                    else
                    {
                      mCommandTypeToCallbacksMap[type] = CommandCallbackPairs{{pair}};
                    } });

    pthread_mutex_unlock(&sMutexNotifs);
  }
}

void FullSystem::unregisterToCommands(ISystemExecuter *iExecuter)
{
  ESP_LOGI("Running", "FullSystem.unregisterToCommands.");

  if (pthread_mutex_lock(&sMutexNotifs) == 0)
  {
    std::for_each(mCommandTypeToCallbacksMap.begin(), mCommandTypeToCallbacksMap.end(), [iExecuter](auto &mapEntry)
                  { CommandCallbackPairs &pairs = mapEntry.second;
      std::erase_if(pairs, [iExecuter](CommandCallbackPair &pair)
                    { return std::addressof(pair.first.get()) == iExecuter; }); });

    pthread_mutex_unlock(&sMutexNotifs);
  }
}

void FullSystem::registerPeriodicCall(ISystemExecuter *iExecuter, TickType_t iPeriodicity, const PeriodicCallback &iCallback)
{
  if (pthread_mutex_lock(&sMutexPeriodics) == 0)
  {
    if (mPeriodicCallMap.contains(iPeriodicity))
    {
      PeriodicThisCallbacksPair &mapPair = mPeriodicCallMap[iPeriodicity];
      mapPair.second.push_back(std::make_pair<>(std::ref(*iExecuter), iCallback));
    }
    else
    {
      PeriodicThisCallbacksPair mapPair = {0, {std::make_pair<>(std::ref(*iExecuter), iCallback)}};
      mPeriodicCallMap.emplace(iPeriodicity, mapPair);
    }

    pthread_mutex_unlock(&sMutexPeriodics);
  }
}

void FullSystem::unregisterPeriodicCall(ISystemExecuter *iExecuter)
{
  ESP_LOGI("Running", "FullSystem.unregisterPeriodicCall.");

  if (pthread_mutex_lock(&sMutexPeriodics) == 0)
  {
    std::for_each(mPeriodicCallMap.begin(), mPeriodicCallMap.end(), [iExecuter](auto &mapEntry)
                  { PeriodicThisCallbacksPair &mapPair = mapEntry.second;
      std::erase_if(mapPair.second, [iExecuter](PeriodicThisCallback &callbackPair)
                    { return std::addressof(callbackPair.first.get()) == iExecuter; }); });

    pthread_mutex_unlock(&sMutexPeriodics);
  }
}

// Public various methods

void FullSystem::periodicCalls()
{
  PeriodicCallbacks callbacks;

  if (pthread_mutex_lock(&sMutexPeriodics) == 0)
  {
    std::for_each(mPeriodicCallMap.begin(), mPeriodicCallMap.end(), [this, &callbacks](auto &mapEntry)
                  {
      PeriodicThisCallbacksPair &mapPair = mapEntry.second;

      if (++mapPair.first >= mapEntry.first)
      {
        mapPair.first = 0;
        std::for_each(mapPair.second.begin(), mapPair.second.end(), [&callbacks](PeriodicThisCallback &callbackPair)
                      { callbacks.push_back(callbackPair.second); });
      } });

    pthread_mutex_unlock(&sMutexPeriodics);
  }

  std::for_each(callbacks.begin(), callbacks.end(), [](PeriodicCallback &callback)
                { callback(); });
}

// Private static methods

void FullSystem::checkingTask(void *argument)
{
  bool callResult(false);
  SystemStateId previousState = SystemStateId::NotBuilt;
  FullSystem *systemPointer = (FullSystem *)argument;
  int counter = 0;

  do
  {
    SystemStateId currentState = systemPointer->getCurrentState();

    if (currentState != previousState)
    {
      ESP_LOGI("Running", "FullSystem::checkingTask old state=%d to new state=%d", (int)previousState, (int)currentState);

      previousState = currentState;

      ESP_LOGI("Running", "FullSystem.checkingTask memory remaining=%d, memory minimum free=%d",
               heap_caps_get_free_size(MALLOC_CAP_8BIT), heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
    }

    switch (currentState)
    {
    case SystemStateId::InitializationStart:
      systemPointer->setCurrentState(SystemStateId::InitializationModeSet);
      break;

    case SystemStateId::InitializationModeSet:
    {
      ESP_LOGI("Initializing", "FullSystem.checkingTask provisioning the system.");
      AbstractWifiCommunicator::initializeWifi();
      systemPointer->setCurrentState(SystemStateId::InitializationTasks);
    }
    break;

    case SystemStateId::InitializationTasks:
      callResult = systemPointer->initializeTasks();
      systemPointer->setCurrentState(callResult ? SystemStateId::InitializationRegisters : SystemStateId::Failure);
      break;

    case SystemStateId::InitializationRegisters:
      systemPointer->initializeCommandHandlerCalls();
      systemPointer->initializePeriodicCalls();

      ESP_LOGI("Initializing", "FullSystem.checkingTask starting the system with BLE provisioning connection.");
      systemPointer->setCurrentState(SystemStateId::BleStartProvisioning);
      break;

      // PROVISIONING

    case SystemStateId::BleStartProvisioning:
      ESP_LOGI("Initializing", "FullSystem.checkingTask sending provisioning command.");
      systemPointer->pushCommand(new WifiProvisioningCommand(std::make_shared<const CommandRequester>(internal::deviceNode, false), true));
      break;

    case SystemStateId::BleProvisioning:
      // Provisioning in progress, events caught make the transition
      break;

    case SystemStateId::BleProvisioned:
      esp_restart();
      break;

    case SystemStateId::WifiConnectionFailure:
      ESP_LOGE("Initializing", "FullSystem.checkingTask Wifi connection failure! Moving to Failure state.");
      systemPointer->setCurrentState(SystemStateId::Failure);
      break;

      // FAILURE, DOWN, AND UNHANDLED CASE

    case SystemStateId::Failure:
      break; 

    default:
      ESP_LOGE("Initializing", "FullSystem.checkingTask unhandled case! Moving to Failure state.");
      systemPointer->wrapUp();
      systemPointer->setCurrentState(SystemStateId::Failure);
      break;

    case SystemStateId::Down: // Does nothing, will leave main loop
      break;
    }

    systemPointer->popCommands();
    systemPointer->periodicCalls();

    if (++counter > SYSTEM_ACTIVITY_LOG_TIMEOUT)
    {
      counter = 0;

      int8_t power;
      esp_wifi_get_max_tx_power(&power);

      ESP_LOGI("Running", "FullSystem.checkingTask state=%d, memory remaining=%d, memory minimum free=%d, antenna power=%d",
               (int)currentState, heap_caps_get_free_size(MALLOC_CAP_8BIT), heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT), power);
    }

    TickType_t lastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&lastWakeTime, PERIODIC_CALL_UNIT_MS);
  } while (systemPointer->getCurrentState() != SystemStateId::Down);

  vTaskDelete(NULL);
}

// Non static private methods

void FullSystem::popCommands()
{
  while (true)
  {
    if (pthread_mutex_trylock(&sMutexCommands) == 0)
    {
      if (!mCommands.empty())
      {
        std::unique_ptr<IAbstractCommand> commandPtr = std::move(mCommands.front());
        mCommands.pop_front();

        pthread_mutex_unlock(&sMutexCommands);

        if (pthread_mutex_lock(&sMutexNotifs) == 0)
        {
          auto foundIterator = mCommandTypeToCallbacksMap.find(commandPtr->getType());

          if (foundIterator != mCommandTypeToCallbacksMap.end())
          {
            CommandCallbackPairs pairs = foundIterator->second;
            pthread_mutex_unlock(&sMutexNotifs);

            std::for_each(pairs.begin(), pairs.end(), [&commandPtr](const CommandCallbackPair &pair)
                          { pair.second(*commandPtr); });
          }
          else
          {
            pthread_mutex_unlock(&sMutexNotifs);
          }
        }
      }
      else
      {
        pthread_mutex_unlock(&sMutexCommands);
        break;
      }
    }
  }
}

bool FullSystem::handleStateNotificationCommand(const IAbstractCommand &iCommand)
{
  bool result(false);

  const StateNotificationCommand<CommunicatorStateId> *command = dynamic_cast<const StateNotificationCommand<CommunicatorStateId> *>(&iCommand);

  if (command != nullptr)
  {
    switch (command->getData())
    {
      // Wifi general

    case CommunicatorStateId::WifiStarting:
      // Intentional fallthrough
    case CommunicatorStateId::WifiScanningAccessPoints:
      break;

    case CommunicatorStateId::WifiIpAddressAssigned:
      ESP_LOGI("Running", "FullSystem::handleStateNotificationCommand IP address notified.");
      break;

    default:
      break;
    }
  }

  return result;
}
