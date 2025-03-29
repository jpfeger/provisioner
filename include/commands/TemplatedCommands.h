#pragma once

#include <memory>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"

#include "commands/AbstractCommand.h"
#include "utilities/CommunicationType.h"
#include "system/IFullSystem.h"

//
// COMMAND WifiProvisioningCommand : true to start provisioning, false otherwise
//

using WifiProvisioningCommand = AbstractCommand<CommandType::WifiProvisioningCommand, bool>;

//
// COMMAND StateNotificationCommand<class NOTIFIED_ENUM> : various states from various classes
//

template <class NOTIFIED_ENUM>
using StateNotificationCommand = AbstractCommand<CommandType::StateNotificationCommand, const NOTIFIED_ENUM &>;
