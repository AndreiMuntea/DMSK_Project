#include "trace.h"
#include "CommandInterpreter.tmh"
#include "CommandInterpreter.hpp"
#include "GlobalData.hpp"

#include <iostream>
#include <functional>

CommandInterpreter::CommandInterpreter()
{
    availableCommands.emplace(
        std::piecewise_construct,
        std::make_tuple("Help"),
        std::make_tuple("Displays a list of available commands", [this]() {this->PrintHelpCommand(); })
    );

    availableCommands.emplace(
        std::piecewise_construct,
        std::make_tuple("Start"),
        std::make_tuple("Starts a thread pool with 5 threads", [this]() {this->StartThreadPoolCommand(); })
    );

    availableCommands.emplace(
        std::piecewise_construct,
        std::make_tuple("Stop"),
        std::make_tuple("Stops the previously created thread pool", [this]() {this->StopThreadPoolCommand(); })
    );

    availableCommands.emplace(
        std::piecewise_construct,
        std::make_tuple("Exit"),
        std::make_tuple("Performs a clean exit of the application", [this]() {this->ExitCommand(); })
    );
}

void 
CommandInterpreter::Execute(
    _In_ const std::string& Command
)
{
    std::cout << "Received command " << Command << std::endl;
    ConsoleAppLogInfo("Received command : %s", Command.c_str());

    auto cmdHandler = availableCommands.find(Command);
    if (cmdHandler != availableCommands.end())
    {
        ConsoleAppLogInfo("Handler found for command command : %s", Command.c_str());
        cmdHandler->second.Execute();
    }
    else
    {
        std::cout << "Handler not found for command " << Command << std::endl;
        ConsoleAppLogWarning("Handler not found for command : %s", Command.c_str());
    }
}

void 
CommandInterpreter::PrintHelpCommand()
{
    std::cout << "Available commands: " << std::endl;
    for (const auto& command : this->availableCommands)
    {
        std::cout << "\t > " << command.first << " - " << command.second.GetDescription() << std::endl;
    }
}

void 
CommandInterpreter::ExitCommand()
{
    gGlobalData.IsApplicationRunning = false;
}

void 
CommandInterpreter::StartThreadPoolCommand()
{
    if (gGlobalData.ThreadPool)
    {
        std::cout << "ThreadPool is running. Please stop it first" << std::endl;
        ConsoleAppLogWarning("ThreadPool is already running");
        return;
    }

    std::cout << "Creating a thread pool with 5 threads" << std::endl;
    ConsoleAppLogInfo("Creating a thread pool with 5 threads");
    
    gGlobalData.ThreadPool.reset(new ThreadPool(5));
}

void 
CommandInterpreter::StopThreadPoolCommand()
{
    if (!gGlobalData.ThreadPool)
    {
        std::cout << "ThreadPool is not running. Please start it first" << std::endl;
        ConsoleAppLogWarning("ThreadPool is not running");
        return;
    }

    gGlobalData.ThreadPool->Shutdown();
    gGlobalData.ThreadPool = nullptr;
}
