#ifndef __PROCESS_COLLECTOR_HPP__ 
#define __PROCESS_COLLECTOR_HPP__ 

#include "Process.hpp"
#include "Lock.hpp"

class ProcessCollector
{
public:
    ProcessCollector() = default;
    ~ProcessCollector();

    void InsertProcess(
        _In_ HANDLE Pid
    );

    void TerminateProcess(
        _In_ HANDLE Pid
    );

    Process* GetProcess(
        _In_ HANDLE Pid
    );

    void InsertModuleForProcess(
        _In_ HANDLE Pid,
        _In_ PUNICODE_STRING ImageName,
        _In_ PVOID ImageBase,
        _In_ SIZE_T ImageSize
    );

private:

    Process* FindProcessUnsafe(
        _In_ HANDLE Pid
    );

    Pushlock lock;
    LinkedList<Process> activeProcesses;
};


#endif //__PROCESS_COLLECTOR_HPP__