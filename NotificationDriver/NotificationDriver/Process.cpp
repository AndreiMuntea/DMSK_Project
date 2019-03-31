#include "Process.hpp"
#include "trace.hpp"
#include "Process.tmh"
#include "Lockguard.hpp"


Process::~Process()
{
    ExclusiveLockguard guard(&Lock);
    this->Modules.Flush();
}

void Process::InsertModule(
    _In_ PUNICODE_STRING ImageName,
    _In_ PVOID ImageBase,
    _In_ SIZE_T ImageSize
)
{
    auto module = new Module(ImageName, ImageBase, ImageSize);
    if (!module)
    {
        MyDriverLogError("Not enough resources to allocate module!");
        return;
    }

    if (!module->IsValid())
    {
        MyDriverLogError("Invalid module allocated!");
        delete module;
        return;
    }

    ExclusiveLockguard guard(&Lock);
    this->Modules.InsertTail(module);

    MyDriverLogInfo(
        "Module inserted: [PID] = %d, ModuleImageName = %wZ, ImageBase = %p, ImageSize = %d", 
        (unsigned __int32)(SIZE_T)this->Pid, 
        ImageName, 
        ImageBase, 
        (unsigned __int32)ImageSize
    );
}

Module * Process::FindModuleUnsafe(
    _In_ PVOID Address
)
{
    for (auto it = Modules.begin(); it != Modules.end(); ++it)
    {
        auto module = it.GetRawPointer();
        if (module->IsAddressInModule(Address))
        {
            return module;
        }
    }

    MyDriverLogTrace("Module containing address %p was not found", Address);
    return nullptr;
}

Module* Process::FindModule(
    _In_ PVOID Address
)
{
    SharedLockguard guard(&Lock);
    return FindModuleUnsafe(Address);

}
