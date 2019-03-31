#include "ProcessCollector.hpp"
#include "trace.hpp"
#include "ProcessCollector.tmh"
#include "Lockguard.hpp"

ProcessCollector::~ProcessCollector()
{
    ExclusiveLockguard guard(&lock);
    activeProcesses.Flush();
}

void ProcessCollector::InsertProcess(
    _In_ HANDLE Pid
)
{
    ExclusiveLockguard guard(&lock);
    auto process = new Process(Pid);
    if (process)
    {
        activeProcesses.InsertTail(process);
        MyDriverLogTrace("Process with pid %d was inserted", (unsigned __int32)(SIZE_T)Pid);
    }
}

void ProcessCollector::TerminateProcess(
    _In_ HANDLE Pid
)
{
    ExclusiveLockguard guard(&lock);
    auto process = FindProcessUnsafe(Pid);
    if (process)
    {
        activeProcesses.RemoveEntry(process);
        delete process;
        MyDriverLogTrace("Process with pid %d was terminated", (unsigned __int32)(SIZE_T)Pid);
    }
}

Process* ProcessCollector::GetProcess(
    _In_ HANDLE Pid
)
{
    SharedLockguard guard(&lock);
    return FindProcessUnsafe(Pid);
}

void ProcessCollector::InsertModuleForProcess(
    _In_ HANDLE Pid,
    _In_ PUNICODE_STRING ImageName,
    _In_ PVOID ImageBase,
    _In_ SIZE_T ImageSize
)
{
    SharedLockguard guard(&lock);
    auto process = FindProcessUnsafe(Pid);
    if (process)
    {
        process->InsertModule(ImageName, ImageBase, ImageSize);
    }
}

Process* ProcessCollector::FindProcessUnsafe(
    _In_ HANDLE Pid
)
{
    for (auto it = activeProcesses.begin(); it != activeProcesses.end(); ++it)
    {
        auto process = it.GetRawPointer();
        if (process->GetPid() == Pid)
        {
            return process;
        }
    }

    MyDriverLogTrace("Process with pid %d was not found", (unsigned __int32)(SIZE_T)Pid);
    return nullptr;
}
