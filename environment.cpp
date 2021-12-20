#include "environment.hpp"

Environment::Environment()
{
    InitBugs(numberOfBugs);
}

void Environment::InitBugs(int numberBugs)
{
    for(int i=0; i<numberBugs;i++)
    {
        BugFactory tempBug=BugFactory(true,cpv(RandomInt(-150,150),RandomInt(-150,150)));
        bugs.push_back(tempBug);

    }

}
