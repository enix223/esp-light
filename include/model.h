#pragma once
#include <cstdint>

namespace cl
{
  struct Light
  {
    // 第几个灯
    uint8_t i;
    // 红色
    uint8_t r;
    // 绿色
    uint8_t g;
    // 蓝色
    uint8_t b;
    // 亮度
    uint8_t a;
  };
} // namespace cl
