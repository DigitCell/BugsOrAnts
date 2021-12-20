#ifndef MAINLOOP_HPP
#define MAINLOOP_HPP

#pragma once

#include "GraphModule.hpp"
#include "environment.hpp"


class MainLoop
{
public:
    MainLoop(int _argc, char** _argv):
        graphModule("particle exp","Squid simulation","some info")
    {
        env=new Environment();
        entry::initApp(&graphModule, _argc, _argv);
    };

    bool run();
    bool runMainLoop=true;
    GraphModule graphModule;
    Environment* env;

};

#endif // MAINLOOP_HPP
