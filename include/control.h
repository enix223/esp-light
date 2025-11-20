#pragma once

#include <string>
#include <functional>
#include "model.h"

namespace cl
{
  using PropertiesChangedCallback = std::function<void(const Light &)>;
  class DeviceController
  {
  public:
    virtual void SetProperties(const Light &request) = 0;

    virtual void Subscribe(const std::string &device_name, PropertiesChangedCallback callback) = 0;
  };
} // namespace cl
