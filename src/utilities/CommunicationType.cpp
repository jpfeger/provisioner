
#include "esp_err.h"
#include "esp_log.h"

#include "utilities/CommunicationType.h"

const std::map<CommunicationTypeEnum, std::string> CommunicationType::mEnumToStringMap({
    {CommunicationTypeEnum::None, "no_communication"},
    {CommunicationTypeEnum::Wifi, "wifi_communication"},
    {CommunicationTypeEnum::Bluetooth, "bluetooth_communication"},
    {CommunicationTypeEnum::Internal, "internal_communication"},
    {CommunicationTypeEnum::Ble_provisioning, "bluetooth_provisioning"},
});

const std::map<std::string, CommunicationTypeEnum> CommunicationType::mStringToEnumMap({
    {"no_communication", CommunicationTypeEnum::None},
    {"wifi_communication", CommunicationTypeEnum::Wifi},
    {"bluetooth_communication", CommunicationTypeEnum::Bluetooth},
    {"internal_communication", CommunicationTypeEnum::Internal},
    {"bluetooth_provisioning", CommunicationTypeEnum::Ble_provisioning},
});

const std::string CommunicationType::toString(const CommunicationTypeEnum &iType)
{
  std::string result("");

  auto found = mEnumToStringMap.find(iType);

  if (found != mEnumToStringMap.end())
  {
    result = found->second;
  }

  return result;
}

CommunicationTypeEnum CommunicationType::toEnum(const std::string &iType)
{
  CommunicationTypeEnum result(CommunicationTypeEnum::Unknown);

  auto found = mStringToEnumMap.find(iType);

  if (found != mStringToEnumMap.end())
  {
    result = found->second;
  }

  return result;
}
