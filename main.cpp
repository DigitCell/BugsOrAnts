
#include <iostream>
#include "mainloop.hpp"


int _main_(int _argc, char** _argv)
{
    std::cout << "Hello RayMarching!" << std::endl;
    MainLoop mainLoop( _argc, _argv);
    mainLoop.run();

    return 0;

}
