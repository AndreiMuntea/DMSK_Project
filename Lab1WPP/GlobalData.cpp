#include "trace.h"
#include "GlobalData.tmh"
#include "GlobalData.hpp"

#include <iostream>

GlobalData gGlobalData;



GlobalData::GlobalData()
{
    SolveImports();
}

GlobalData::~GlobalData()
{
    CleanImports();
}

void 
GlobalData::SolveImports()
{
    NtDllModuleHandle = LoadLibrary("ntdll.dll");
    if (!NtDllModuleHandle)
    {
        CleanImports();
        std::wcout << "LoadLibrary ntdll.dll has failed with GLE = " << GetLastError() << std::endl;
        ConsoleAppLogCritical("LoadLibrary ntdll.dll has failed with GLE = %d", GetLastError());
        throw std::exception("LoadLibrary ntdll.dll has failed");
    }

    ZwQueryInformationProcess = (PFUNC_ZwQueryInformationProcess)GetProcAddress(NtDllModuleHandle, "ZwQueryInformationProcess");
    if (!NtDllModuleHandle)
    {
        CleanImports();
        std::wcout << "GetProcAddress ZwQueryInformationProcess failed with GLE = " << GetLastError() << std::endl;
        ConsoleAppLogCritical("LoadLibrary ntdll.dll has failed with GLE = %d", GetLastError());
        throw std::exception("LoadLibrary ntdll.dll has failed");
    }

    ZwReadVirtualMemory = (PFUNC_ZwReadVirtualMemory)GetProcAddress(NtDllModuleHandle, "ZwReadVirtualMemory");
    if (!NtDllModuleHandle)
    {
        CleanImports();
        std::wcout << "GetProcAddress ZwReadVirtualMemory failed with GLE = " << GetLastError() << std::endl;
        ConsoleAppLogCritical("GetProcAddress ZwReadVirtualMemory failed with GLE = %d", GetLastError());
        throw std::exception("LoadLibrary ntdll.dll has failed");
    }
}

void GlobalData::CleanImports()
{
    if (NtDllModuleHandle)
    {
        FreeLibrary(NtDllModuleHandle);
        NtDllModuleHandle = nullptr;
    }

    if (ZwQueryInformationProcess)
    {
        ZwQueryInformationProcess = nullptr;
    }

    if (ZwReadVirtualMemory)
    {
        ZwReadVirtualMemory = nullptr;
    }
}
