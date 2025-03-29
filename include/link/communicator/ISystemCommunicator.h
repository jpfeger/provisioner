#pragma once

#include <map>
#include <pthread.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "link/communicator/ICommunicator.h"
#include "system/ISystemExecuter.h"

class ISystemCommunicator : virtual public ICommunicator, virtual public ISystemExecuter
{
public:
  // Constructors, copiers, and destructor

  ISystemCommunicator() = default;

  virtual ~ISystemCommunicator() = default;

  ISystemCommunicator(const ISystemCommunicator &rhs) = default;

  ISystemCommunicator &operator=(const ISystemCommunicator &rhs) = default;
};
