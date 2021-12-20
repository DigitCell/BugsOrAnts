#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP


#include "constants.hpp"
#include "BugFactory.h"
#include "time.h"

class Environment
{
public:

    Environment();

    float BUG_TIME_MINIMUM = 1;
    float BUG_TIME_MAXIMUM = 4;
    float TIME_STEP_MAX = 0.2f;

    void InitBugs(int numberBugs);

    vector<BugFactory> bugs;


    int RandomInt(int min, int max)
    {
        return std::rand()%((max - min) + 1) + min;
    }


};

#endif // ENVIRONMENT_HPP
