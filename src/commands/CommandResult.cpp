
#include "esp_err.h"
#include "esp_log.h"
#include "ArduinoJson.h"

#include "commands/CommandResult.h"
#include "system/IFullSystem.h"
#include "commands/CommandRequester.h"
#include "link/communicator/ICommunicator.h"

const std::map<CommandResultEnum, std::string> CommandResult::mEnumToStringMap({
    {CommandResultEnum::None, ""},
    {CommandResultEnum::NoError, "no_error"},
    {CommandResultEnum::IllFormedJson, "ill_formed_json"},
    {CommandResultEnum::IllegalJsonValue, "illegal_json_value"},
    {CommandResultEnum::NotAllowedToPerform, "not_allowed_to_perform"},
});

const std::map<std::string, CommandResultEnum> CommandResult::mStringToEnumMap({
    {"", CommandResultEnum::None},
    {"no_error", CommandResultEnum::NoError},
    {"ill_formed_json", CommandResultEnum::IllFormedJson},
    {"illegal_json_value", CommandResultEnum::IllegalJsonValue},
    {"not_allowed_to_perform", CommandResultEnum::NotAllowedToPerform},
});

const std::string CommandResult::toString(const CommandResultEnum &iMode)
{
  std::string result("");

  auto found = mEnumToStringMap.find(iMode);

  if (found != mEnumToStringMap.end())
  {
    result = found->second;
  }

  return result;
}

CommandResultEnum CommandResult::toEnum(const std::string &iMode)
{
  CommandResultEnum result(CommandResultEnum::None);

  auto found = mStringToEnumMap.find(iMode);

  if (found != mStringToEnumMap.end())
  {
    result = found->second;
  }

  return result;
}

CommandResult::CommandResult()
    : CommandResult(CommandResultEnum::NoError, "")
{
}

CommandResult::CommandResult(CommandResultEnum iType, const std::string &iExplanation)
    : mType(iType),
      mDescription(iExplanation)
{
}

// Public methods

void CommandResult::setType(CommandResultEnum iType)
{
  mType = iType;
}

CommandResultEnum CommandResult::getType() const
{
  return mType;
}

std::string CommandResult::getTypeAsString() const
{
  return CommandResult::toString(mType);
}

void CommandResult::setDescription(const std::string &iDescription)
{
  mDescription = iDescription;
}

std::string CommandResult::getDescription() const
{
  return mDescription;
}
