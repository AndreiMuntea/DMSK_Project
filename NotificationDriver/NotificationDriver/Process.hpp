#ifndef __PROCESS_HPP__ 
#define __PROCESS_HPP__

#include "cpp_includes.hpp"
#include "DriverTags.hpp"
#include "LinkedList.hpp"
#include "Lock.hpp"
#include "Module.hpp"

class Process : public LinkedListEntry,
                public CppPagedObject<DRV_TAG_PST>
{
public:
    Process(_In_ HANDLE Pid) : Pid{ Pid } {}
    virtual ~Process();

    HANDLE GetPid() const { return Pid; }

    void InsertModule(
        _In_ PUNICODE_STRING ImageName,
        _In_ PVOID ImageBase,
        _In_ SIZE_T ImageSize
    );

    Module* FindModule(
        _In_ PVOID Address
    );

private:
    Module* FindModuleUnsafe(
        _In_ PVOID Address
    );

    HANDLE Pid = 0;
    Pushlock Lock;
    LinkedList<Module> Modules;
};

#endif //__PROCESS_HPP__