#ifndef __GLOBAL_DATA_HPP__ 
#define __GLOBAL_DATA_HPP__

#include "CommandInterpreter.hpp"
#include "ThreadPool.hpp"
#include "DynamicImports.hpp"

#include <memory>

struct GlobalData
{
public:
    GlobalData();
    ~GlobalData();

    bool IsApplicationRunning = true;
    CommandInterpreter CommandInterpreter;
    std::shared_ptr<ThreadPool> ThreadPool = nullptr;

    HMODULE NtDllModuleHandle = nullptr;
    PFUNC_ZwQueryInformationProcess ZwQueryInformationProcess = nullptr;
    PFUNC_ZwReadVirtualMemory ZwReadVirtualMemory = nullptr;
    PFUNC_ZwQuerySystemInformation ZwQuerySystemInformation = nullptr;

private:
    void SolveImports();
    void CleanImports();
};

extern GlobalData gGlobalData;

#endif //__GLOBAL_DATA_HPP__