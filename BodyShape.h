#ifndef BODYSHAPE_H
#define BODYSHAPE_H

#include "math.h"
#include "vector"
#include "random"
#include "math/randomFunctions.h"
#include "constants.hpp"

using namespace std;

class BodyShape
{

public:

    BodyShape(bool head)
    {
      length =   rf.RandomFloat(BodyShapeLENGTH_MIN,BodyShapeLENGTH_MAX);
      legAngle = rf.RandomFloat(LEG_ANGLE_MIN,LEG_ANGLE_MAX);
      legFactor =rf.RandomFloat(LEG_FACTOR_MIN ,LEG_FACTOR_MAX);
      thicknessMultiplier = THICKNESS_MULTIPLIER_MIN + (1 - THICKNESS_MULTIPLIER_MIN) * rf.RandomFloat();
     // fill = BodyShape.COLORS[Math.floor(Math.random() * BodyShape.COLORS.length)];
      thickness = 0;

      while (getLength() < length)
      {
          float newThickness = rf.RandomFloat(WIDTH_MIN,WIDTH_MAX) * thicknessMultiplier;

          if (thickness < newThickness)
                thickness = newThickness;

            widths.push_back(newThickness);
        }

    };

    float length;
    vector<float> widths;
    float legAngle;
    float legFactor;
    float thicknessMultiplier;
    //float fill = BodyShape.COLORS[Math.floor(Math.random() * BodyShape.COLORS.length)];
    float thickness = 0;

    randomFuntions rf;

    float getLength()
    {
        return (widths.size() + 1.0f) * LENGTH_SEGMENT;
    };

    float getThickness() {return thickness;}
    float getLegAngle()  {return legAngle;}
    float getLegLength() {return max(LEG_LENGTH_MIN, thickness * LEG_SCALE * legFactor);}



    float getWidthMultiplier(float i)
    {
        return sin((i / (widths.size() - 1)) * M_PI* SINE_WAVE_PORTION);
    };


};

#endif // BODYSHAPE_H
