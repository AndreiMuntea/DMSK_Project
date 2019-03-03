#ifndef __GLOBAL_DATA_HPP__ 
#define __GLOBAL_DATA_HPP__

#include "CommandInterpreter.hpp"
#include "ThreadPool.hpp"

#include <memory>

struct GlobalData
{
    bool IsApplicationRunning = true;
    CommandInterpreter CommandInterpreter;
    std::shared_ptr<ThreadPool> ThreadPool = nullptr;
};

extern GlobalData gGlobalData;

#endif //__GLOBAL_DATA_HPP__