#ifndef LEG_H
#define LEG_H

#include "math/cpVect.h"
#include "math/randomFunctions.h"
#include "AntNoise.hpp"
#include "math.h"

struct Leg
{
public:

    float GROUND_THRESHOLD = 0.9;
    Leg(cpVect _pos, float _bugDirection, float _direction, float _length, float _initialProgress ):
        pos(_pos),
        bugDirection(_bugDirection),
        direction(_direction),
        length(_length),
        initialProgress(_initialProgress)
    {
        foot.x = pos.x + cos(bugDirection + direction) * length;
        foot.y = pos.y + sin(bugDirection + direction) * length;

         applyInitialProgress();
    };


    cpVect foot;
    cpVect pos;
    float bugDirection;
    float direction;
    float length;
    float initialProgress;
    bool counterpart =true;

    //let counterpart = null;
     bool onGround = true;
     float dist;
     float dx;
     float dy;


    void applyInitialProgress()
    {
            dist = cos(direction) * length * -2.0f * initialProgress;
            foot.x += cos(bugDirection) * dist;
            foot.y += sin(bugDirection) * dist;
    }


//    this.setCounterpart = leg => counterpart = leg;

    void stepDown()  {
          float dx = foot.x - pos.x;
          float dy = foot.y - pos.y;

          if (sqrt(dx * dx + dy * dy) < length)
              onGround = true;
      };

    void update(cpVect newPos, float bugDirection,  float speed, float timeStep, Leg& counterpart)
    {
          pos= newPos;


          dx = foot.x - pos.x;
          dy = foot.y - pos.y;
          dist = sqrt(dx * dx + dy * dy);

            if (onGround) {
                  if (dist > length) {
                      onGround = false;

                     // if (counterpart)
                         counterpart.stepDown();
                  }
              }
              else {
                  float xAim = pos.x + cos(bugDirection + direction) * length;
                  float yAim = pos.y + sin(bugDirection + direction) * length;
                  float dxAim = xAim - foot.x;
                  float dyAim = yAim - foot.y;
                  float lengthAim =sqrt(dxAim * dxAim + dyAim * dyAim);

                  if (lengthAim < length * (1 - GROUND_THRESHOLD))
                      onGround = true;
                  else {
                      foot.x += (dxAim / lengthAim) * speed * timeStep;
                      foot.y += (dyAim / lengthAim) * speed * timeStep;
                  }
              }
          }


};


#endif // LEG_H
