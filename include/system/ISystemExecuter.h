#pragma once

#include "freertos/FreeRTOS.h"

class ISystemExecuter
{
public:
  // Constructors, copiers, and destructor

  ISystemExecuter() = default;

  virtual ~ISystemExecuter() = default;

  ISystemExecuter(const ISystemExecuter &rhs) = default;

  ISystemExecuter &operator=(const ISystemExecuter &rhs) = default;

  // Interface

  virtual bool initializeTasks() = 0;

  virtual void initializeCommandHandlerCalls() = 0;

  virtual void initializePeriodicCalls() = 0;

  virtual bool wrapUp() = 0;
};
