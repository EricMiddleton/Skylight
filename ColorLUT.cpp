#include "ColorLUT.hpp"

#include <algorithm>

LedLevel ColorLUT::CalculateLedLevel(float cct, float brightness, float ledMaxLevel)
{
  brightness = std::max(0.0f, std::min(1.0f, brightness));

  float warmNorm, coolNorm;

  if(cct <= MinColorTemp())
  {
    //At or below min, easy lookup
    warmNorm = m_warmTable[TABLE_LENGTH-1];
    coolNorm = m_coolTable[TABLE_LENGTH-1];
  }
  else if(cct >= MaxColorTemp())
  {
    //At or above max, easy lookup
    warmNorm = m_warmTable[0];
    coolNorm = m_coolTable[0];
  }
  else
  {
    //Linear search through the CCT table to find closest cct value
    // that is <= requested level
    int idx = 1;
    for(; idx < TABLE_LENGTH; ++idx)
    {
      if(m_cctTable[idx] <= cct)
      {
        break;
      }
    }

    if(cct == m_cctTable[idx])
    {
      warmNorm = m_warmTable[idx];
      coolNorm = m_coolTable[idx];
    }
    else
    {
      //Linear interpolation between idx and (idx-1)
      float idxMinusOneScalar = (cct - m_cctTable[idx]) / (m_cctTable[idx-1] - m_cctTable[idx]);
      float idxScalar = 1.0f - idxMinusOneScalar;
      
      warmNorm = m_warmTable[idx]*idxScalar + m_warmTable[idx-1]*idxMinusOneScalar;
      coolNorm = m_coolTable[idx]*idxScalar + m_coolTable[idx-1]*idxMinusOneScalar;
    }
  }

  return
  {
    static_cast<uint16_t>(warmNorm * brightness * ledMaxLevel + 0.5f),
    static_cast<uint16_t>(coolNorm * brightness * ledMaxLevel + 0.5f)
  };
}

float ColorLUT::MinColorTemp()
{
  //Last element in CCT table is min temp
  return m_cctTable[TABLE_LENGTH-1];
}

float ColorLUT::MaxColorTemp()
{
  //First element in CCT table is max temp
  return m_cctTable[0];
}

const float ColorLUT::m_cctTable[] =
{
  6050,
  5892,
  5745,
  5620,
  5505,
  5393,
  5307,
  5220,
  5136,
  5063,
  4992,
  4922,
  4892,
  4839,
  4827,
  4795,
  4762,
  4728,
  4693,
  4656,
  4615,
  4574,
  4528,
  4482,
  4436,
  4385,
  4320,
  4272,
  4210,
  4144,
  4075,
  3999,
  3915,
  3830,
  3735,
  3630,
  3520,
  3394,
  3260,
  3165,
};

const float ColorLUT::m_coolTable[] =
{
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  0.9803921569f,
  0.9411764706f,
  0.9019607843f,
  0.862745098f,
  0.8235294118f,
  0.7843137255f,
  0.7450980392f,
  0.7058823529f,
  0.6666666667f,
  0.6274509804f,
  0.5882352941f,
  0.5490196078f,
  0.5098039216f,
  0.4705882353f,
  0.431372549f,
  0.3921568627f,
  0.3529411765f,
  0.3137254902f,
  0.2745098039f,
  0.2352941176f,
  0.1960784314f,
  0.1568627451f,
  0.1176470588f,
  0.07843137255f,
  0.03921568627f,
  0.f,
};

const float ColorLUT::m_warmTable[] =
{
  0.f,
  0.07843137255f,
  0.1568627451f,
  0.2352941176f,
  0.3137254902f,
  0.3921568627f,
  0.4705882353f,
  0.5490196078f,
  0.6274509804f,
  0.7058823529f,
  0.7843137255f,
  0.862745098f,
  0.9411764706f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
  1.f,
};
