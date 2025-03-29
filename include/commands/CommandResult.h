#pragma once

#include <string>
#include <map>

enum class CommandResultEnum
{
  None = 0,
  NoError = 1,
  IllFormedJson = 2,
  IllegalJsonValue = 3,
  NotAllowedToPerform = 4
};

// Forward declaration
class IFullSystem;
class CommandRequester;

class CommandResult
{
public:
  // Constructors, copiers, and destructor

  CommandResult();

  CommandResult(CommandResultEnum iType, const std::string &iExplanation);

  ~CommandResult() = default;

  CommandResult(const CommandResult &rhs) = default;

  CommandResult &operator=(const CommandResult &rhs) = default;

  // Static public methods

  static const std::string toString(const CommandResultEnum &iMode);

  static CommandResultEnum toEnum(const std::string &iMode);

  // Public methods

  void setType(CommandResultEnum iType);

  CommandResultEnum getType() const;

  std::string getTypeAsString() const;

  void setDescription(const std::string &iDescription);

  std::string getDescription() const;

private:
  // Private static members

  static const std::map<CommandResultEnum, std::string> mEnumToStringMap;
  static const std::map<std::string, CommandResultEnum> mStringToEnumMap;

  // Private members

  CommandResultEnum mType;
  std::string mDescription;
};
