#include "trace.h"
#include "ProcessUtils.tmh"
#include "ProcessUtils.hpp"
#include "GlobalData.hpp"

#include <Windows.h>
#include <TlHelp32.h>
#include <exception>
#include <string>
#include <iostream>


static void
DumpProcess(
    _In_ DWORD ProcessId
)
{
    PROCESS_BASIC_INFORMATION basicInformation = { 0 };
    PVOID  processParametersAddress = nullptr;
    PVOID commandLineAddress = nullptr;
    SIZE_T  processParametersActualAddress = 0;
    RTL_USER_PROCESS_PARAMETERS userProcessParameters = { 0 };
    SIZE_T noBytesRead = 0;
    std::wstring commandLine;

    auto handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, false, ProcessId);
    if (!handle)
    {
        std::wcout << "\t > Can't display more information. Open Process failed with GLE =  " << GetLastError() << std::endl;
        return;
    }

    // Get the peb address
    auto status = gGlobalData.ZwQueryInformationProcess(handle, ProcessBasicInformation, &basicInformation, sizeof(PROCESS_BASIC_INFORMATION), nullptr);
    if (!NT_SUCCESS(status))
    {
        std::wcout << "\t > Can't display more information. ZwQueryInformationProcess failed with NTSTATUS =  " << status << std::endl;
        goto CleanUp;
    }
    
    // Read the process parameters address 
    processParametersAddress = (PBYTE)basicInformation.PebBaseAddress + FIELD_OFFSET(PEB, ProcessParameters);
    status = gGlobalData.ZwReadVirtualMemory(handle, processParametersAddress, &processParametersActualAddress, sizeof(PVOID), &noBytesRead);
    if (!NT_SUCCESS(status))
    {
        std::wcout << "\t > Can't display more information. ZwReadVirtualMemory failed with NTSTATUS =  " << status << std::endl;
        goto CleanUp;
    }

    // Read the user process parameters
    status = gGlobalData.ZwReadVirtualMemory(handle, (PVOID)processParametersActualAddress, &userProcessParameters, sizeof(RTL_USER_PROCESS_PARAMETERS), &noBytesRead);
    if (!NT_SUCCESS(status))
    {
        std::wcout << "\t > Can't display more information. ZwReadVirtualMemory failed with NTSTATUS =  " << status << std::endl;
        goto CleanUp;
    }

    // Allocate the command line buffer
    commandLine = std::wstring(userProcessParameters.CommandLine.Length, 0);
    commandLineAddress = userProcessParameters.CommandLine.Buffer;

    // Read the actual command line
    status = gGlobalData.ZwReadVirtualMemory(handle, (PVOID)commandLineAddress, &commandLine[0], userProcessParameters.CommandLine.Length, &noBytesRead);
    if (!NT_SUCCESS(status))
    {
        std::wcout << "\t > Can't display more information. ZwReadVirtualMemory failed with NTSTATUS =  " << status << std::endl;
        goto CleanUp;
    }

    std::wcout << "\t > [Command Line] " << commandLine << std::endl;

CleanUp:
    CloseHandle(handle);
}

void 
PuDumpActiveProcessesToolHelp32Snapshot()
{
    PROCESSENTRY32W pe32 = { 0 };

    auto snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshotHandle == INVALID_HANDLE_VALUE)
    {
        std::wcout << "CreateToolhelp32Snapshot failed! GLE=" << GetLastError() << std::endl;
        ConsoleAppLogError("CreateToolhelp32Snapshot failed with GLE=%d", GetLastError());
        throw std::exception("CreateToolhelp32Snapshot failed");
    }
    ConsoleAppLogInfo("CreateToolhelp32Snapshot succeded. Parsing processes...");

    std::wcout << "Parsing processes..." << std::endl;

    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(snapshotHandle, &pe32))
    {
        std::wcout << "Failed to get first process! GLE=" << GetLastError() << std::endl;
        ConsoleAppLogError("Process32FirstW failed with GLE=%d", GetLastError());
        goto CleanUp;
    }

    do
    {
        std::wcout << "=====================================================================" << std::endl;
        std::wcout << "\t > [Process ID] " << pe32.th32ProcessID << std::endl;
        std::wcout << "\t > [Name] " << std::wstring(pe32.szExeFile) << std::endl;
        std::wcout << "\t > [Parent ID] " << pe32.th32ParentProcessID << std::endl;
        std::wcout << "\t > [No Threads] " << pe32.cntThreads << std::endl;

        DumpProcess(pe32.th32ProcessID);

    } while (Process32NextW(snapshotHandle, &pe32));

CleanUp:
    CloseHandle(snapshotHandle);
}

void 
DumpActiveProcessesPsapi()
{
}

void 
DumpActiveProcessesNtQuerySystemInformation()
{
}
