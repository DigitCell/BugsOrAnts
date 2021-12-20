#ifndef RANDOMFUNCTIONS_H
#define RANDOMFUNCTIONS_H

#include "random"

class randomFuntions
{
public:

    int RandomInt(int min, int max)
    {
      return rand()%((max - min) + 1) + min;
    }

    float RandomFloat(float M, float N)
    {
      return M + (rand() / ( RAND_MAX / (N-M) ) ) ;
    }

    float RandomFloat()
    {
      return 0.0f+(rand()/(RAND_MAX/1.0f));
    }
};


#endif // RANDOMFUNCTIONS_H

