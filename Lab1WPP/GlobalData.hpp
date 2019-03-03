#ifndef __GLOBAL_DATA_HPP__ 
#define __GLOBAL_DATA_HPP__

#include "CommandInterpreter.hpp"

struct GlobalData
{
    bool IsApplicationRunning = true;
    CommandInterpreter CommandInterpreter;
};

extern GlobalData gGlobalData;

#endif //__GLOBAL_DATA_HPP__