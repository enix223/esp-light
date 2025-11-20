#pragma once

#include "control.h"

namespace cl
{
  class DeviceControllerMqtt : public DeviceController
  {
  public:
    void SetProperties(const Light &request) override;

    void Subscribe(const std::string &device_name, PropertiesChangedCallback callback) override;
  };
} // namespace cl
