#pragma once

#include <string>
#include <map>

enum class CommunicationTypeEnum
{
  Unknown = 0,
  None = 1,
  Wifi = 2,
  Bluetooth = 3,
  Internal = 4,
  Ble_provisioning = 5
};

class CommunicationType
{
public:
  // Constructors, copiers, and destructor

  CommunicationType();

  ~CommunicationType() = default;

  CommunicationType(const CommunicationType &rhs) = delete;

  CommunicationType &operator=(const CommunicationType &rhs) = delete;

  static const std::string toString(const CommunicationTypeEnum &iMode);

  static CommunicationTypeEnum toEnum(const std::string &iMode);

private:
  // Private static members

  static const std::map<CommunicationTypeEnum, std::string> mEnumToStringMap;
  static const std::map<std::string, CommunicationTypeEnum> mStringToEnumMap;
};
