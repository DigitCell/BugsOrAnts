#ifndef CUBICNOISE_HPP
#define CUBICNOISE_HPP

#include <cstdint>
#include "math.h"
#include "math/FastNoise.h"
#include "math/cubicNoise.hpp"

class AntNoise
{
public:
    AntNoise():
    cubicNoiseConfig(12,24)
    {
         fastnoise.SetNoiseType(FastNoise::Value);
         fastnoise.SetFrequency(0.5);

    };

    FastNoise fastnoise;

    CubicNoise cubicNoise;
    CubicNoiseConfig cubicNoiseConfig;


    float cubicNoiseSample1F(float x)
    {
        return fastnoise.GetNoise(x,x);
    }

    float cubicNoiseSample1F(float x,float y)
    {
        return fastnoise.GetNoise(x,y);
    }

    float cubicNoiseSample1(float x)
    {
        return cubicNoise.sample(cubicNoiseConfig,x,x);
    }

    float cubicNoiseSample1(float x,float y)
    {
        return cubicNoise.sample(cubicNoiseConfig, x,y);
    }
};


#endif // CUBICNOISE_HPP



