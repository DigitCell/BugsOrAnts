#ifndef BUG_H
#define BUG_H

#include "math.h"
#include "BodyShape.h"
#include "AntNoise.hpp"
#include "math/cpVect.h"
#include "math/randomFunctions.h"
#include "leg.h"
#include "constants.hpp"

class Bug
{
public:

    Bug(cpVect _pos , BodyShape& _body, bool _right, bool _hasLegs, bool _parent, float _followDistance):
        pos(_pos),
        body(_body),
        right(_right),
        hasLegs(_hasLegs),
        parent(_parent),
        followDistance(_followDistance)
    {
        speedScale =rf.RandomFloat(SPEED_SCALE_MIN ,SPEED_SCALE_MAX );
        noiseScale =rf.RandomFloat(NOISE_SCALE_MIN,NOISE_SCALE_MAX);
        speedMax = rf.RandomFloat(SPEED_MAX_MIN ,SPEED_MAX_MAX )/30.f;

        if (!parent)
              sampleMotion();

         makeBody();
    };

    Bug(cpVect _pos , BodyShape& _body, bool _right, bool _hasLegs,  bool _parent, Bug& parentBody, float _followDistance):
        pos(_pos),
        body(_body),
        right(_right),
        hasLegs(_hasLegs),
        parent(_parent),
        followDistance(_followDistance)
    {
        if (parent) {
            pos.x = parentBody.getX() + cos(parentBody.getDirection()) * -followDistance;
            pos.y = parentBody.getY() + sin(parentBody.getDirection()) * -followDistance;
            direction = parentBody.getDirection();
         }

        makeBody();
        //pupilRadius = eyeRadius * Bug.EYE_PUPIL_RATIO;
        //speedScale =rf.RandomFloat(SPEED_SCALE_MIN ,SPEED_SCALE_MAX );
       // noiseScale =rf.RandomFloat(NOISE_SCALE_MIN,NOISE_SCALE_MAX);
       // speedMax = rf.RandomFloat(SPEED_MAX_MIN ,SPEED_MAX_MAX );


    };

    cpVect pos;
    BodyShape& body;
    bool right;
    bool hasLegs;
    bool parent;
    float followDistance;

    float lifetime = 0.10;
    float speed=0;
    float direction=0;
    float blinkDelay = 0;
    bool blink = true;
    vector<Leg> legs;

    AntNoise cubicNoise;
    randomFuntions rf;

    float pupilRadius;
    float speedScale=1.0f;
    float noiseScale;
    float speedMax=1;

    float dx;
    float dy;
    float distance ;

    float getX () { return pos.x;}
    float getY () { return pos.y;}
    float getDirection() { return direction;}
    float getSpeed() { return speed;}

    void sampleMotion(){
       speed =     abs(cubicNoise.cubicNoiseSample1(lifetime * speedScale)) * speedMax;
       direction = abs(cubicNoise.cubicNoiseSample1(
                                                pos.x * noiseScale*10,
                                                pos.y * noiseScale*10)) * M_PI * 2.0f;

       if (!right)
          direction += M_PI;
    };


    void makeBody() {

           auto l = Leg(pos, direction, -body.getLegAngle(), body.getLegLength(),  rf.RandomFloat());
           auto r = Leg(pos, direction,  body.getLegAngle(), body.getLegLength(),  rf.RandomFloat());

           // l.setCounterpart(r);
           // r.setCounterpart(l);

           legs.push_back(l);
           legs.push_back(r);
     };


    void update(float timeStep, int width, int height, Bug& parentBug){
          lifetime += timeStep;

          if (parent) {
              dx = parentBug.getX() - pos.x;
              dy = parentBug.getY() - pos.y;
              distance = sqrt(dx * dx + dy * dy);

              pos.x += (dx / distance) * (distance - followDistance);
              pos.y += (dy / distance) * (distance - followDistance);

              direction = atan2(dy, dx);
              speed = parentBug.getSpeed();
          }
          else {
              pos.x +=cos(direction) * speed * timeStep;
              pos.y +=sin(direction) * speed * timeStep;

            sampleMotion();
/*
              if ((blinkDelay -= timeStep) < 0) {
                  if (blink)
                      blinkDelay = Bug.EYE_BLINK_DELAY_MIN + (Bug.EYE_BLINK_DELAY_MAX - Bug.EYE_BLINK_DELAY_MIN) * Math.random();
                  else
                      blinkDelay = Bug.EYE_BLINK_DURATION;

                  blink = !blink;
              }
              */
          }

          //if (child)
         //     child.update(timeStep, width, height);

         // for (auto& leg:legs)
          //    leg.update(pos, direction, speed * 4, timeStep);

           legs[0].update(pos, direction, speed * 4, timeStep,legs[1]);
           legs[1].update(pos, direction, speed * 4, timeStep,legs[0]);
/*
          return x < -Bug.VISIBILITY_RADIUS ||
              y < -Bug.VISIBILITY_RADIUS ||
              x > Bug.VISIBILITY_RADIUS + width ||
              y > Bug.VISIBILITY_RADIUS + height;
              */
      };
};

#endif // BUG_H
