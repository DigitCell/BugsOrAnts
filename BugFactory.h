#ifndef BUGFACTORY_H
#define BUGFACTORY_H

#include "math.h"
#include "math/cpVect.h"
#include "BodyShape.h"
#include "Bug.h"
#include "math/randomFunctions.h"
#include "constants.hpp"

using namespace std;


class BugFactory
{

public:
    BugFactory(bool _right, cpVect _pos):
        right(_right),
        pos(_pos),
        body(false)
    {
        legs=true;
        bool findLegs=true;

        while(findLegs){
            float lengthRandomizer = rf.RandomFloat();
            length=LENGTH_MIN + floor(lengthRandomizer * lengthRandomizer * lengthRandomizer * (LENGTH_MAX - LENGTH_MIN + 1));
            if(length >= INTERLEAVE_LEGS_THRESHOLD)//&& rf.RandomFloat() < INTERLEAVE_LEGS_CHANCE)
            {
                findLegs=false;
                interleaveLegs=true;
            }
        }

          Bug newBug = Bug(pos, body, right, interleaveLegs, false, 0);
          segments.push_back(newBug);

          for (int i = 1; i < length; ++i) {
            if (interleaveLegs)
                legs = !legs;

            auto newSegment = Bug(cpv(0,0), body, right, interleaveLegs, true,segments[i-1], body.getLength() * SEGMENT_OVERLAP);
            segments.push_back(newSegment);
          }
    };

    cpVect pos;
    BodyShape body;
    bool right=true;

    vector<Bug> segments;

    float length;
    bool interleaveLegs;

    bool legs=true;

    randomFuntions rf;
};


#endif // BUGFACTORY_H
