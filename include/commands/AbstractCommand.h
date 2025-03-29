#pragma once

#include "commands/IAbstractCommand.h"
#include "commands/CommandRequester.h"

template <CommandType COMMAND_TYPE_CONSTANT, typename DATA>
class AbstractCommand : virtual public IAbstractCommand
{
public:
  // Constructors, copiers, and destructor
  template <typename... Args>
  AbstractCommand(const std::shared_ptr<const CommandRequester> &iCommandRequester, Args... args)
      : AbstractCommand(iCommandRequester, DATA(args...))
  {
  }

  AbstractCommand(const std::shared_ptr<const CommandRequester> &iCommandRequester, const DATA &iData)
      : mData(iData),
        mRequester(iCommandRequester),
        mState(CommandState::None)
  {
  }

  ~AbstractCommand() = default;

  AbstractCommand(const AbstractCommand &rhs) = default;

  AbstractCommand &operator=(const AbstractCommand &rhs) = default;

  // IAbstractCommand interface

  CommandState getState() const override
  {
    return mState;
  }

  void setState(CommandState iState) override
  {
    mState = iState;
  }

  CommandType getType() const override
  {
    return mType;
  }

  const std::shared_ptr<const CommandRequester> &getRequester() const override
  {
    return mRequester;
  }

  // Getters and setters

  const DATA &getData() const
  {
    return mData;
  }

private:
  static CommandType mType;

  DATA mData;
  std::shared_ptr<const CommandRequester> mRequester;
  CommandState mState;
};

template <CommandType COMMAND_TYPE_CONSTANT, typename DATA>
CommandType AbstractCommand<COMMAND_TYPE_CONSTANT, DATA>::mType = COMMAND_TYPE_CONSTANT;