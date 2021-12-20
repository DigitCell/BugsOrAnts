#include "mainloop.hpp"


bool MainLoop::run()
{
    int Tick=0;

    while(runMainLoop)
    {

        if(graphModule.StartPausePhysics)
        {
          for(int i=0; i<env->bugs.size();i++)
          {
             BugFactory& bug=env->bugs[i];
             bug.segments[0].update(env->TIME_STEP_MAX,300,300,bug.segments[1]);
             for(int i=1;i<bug.segments.size();i++ )
             {
                 bug.segments[i].update(env->TIME_STEP_MAX,300,300,bug.segments[i-1]);
             }

          }
        }

        if(graphModule.stepGraphPress)
        {
          for(int i=0; i<env->bugs.size();i++)
          {
             BugFactory& bug=env->bugs[i];
             bug.segments[0].update(env->TIME_STEP_MAX,300,300,bug.segments[1]);
             for(int i=1;i<bug.segments.size();i++ )
             {
                 bug.segments[i].update(env->TIME_STEP_MAX,300,300,bug.segments[i-1]);
             }

          }
          graphModule.stepGraphPress=false;
        }


       bool engineState= entry::stepApp(&graphModule, env);
       if(!engineState)
           runMainLoop=false;
       Tick++;


    }

    bool shutdownBool= entry::shutdownApp(&graphModule);
    delete env;
    return shutdownBool;

}
