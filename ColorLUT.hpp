#pragma once

#include <cstdint>

struct LedLevel
{
  uint16_t warm;
  uint16_t cool;
};

class ColorLUT
{
public:
  static LedLevel CalculateLedLevel(float cct, float brightness, float ledMaxLevel);

  static float MinColorTemp();
  static float MaxColorTemp();

private:
  static constexpr int TABLE_LENGTH = 40;
  
  static const float m_cctTable[TABLE_LENGTH];
  static const float m_warmTable[TABLE_LENGTH];
  static const float m_coolTable[TABLE_LENGTH];
};
