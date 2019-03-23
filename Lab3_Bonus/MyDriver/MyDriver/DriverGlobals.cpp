#include "DriverGlobals.hpp"
#include "trace.hpp"
#include "DriverGlobals.tmh"


GlobalData gDrvData{};

_Use_decl_annotations_
GlobalData::~GlobalData()
{
    CleanUp();
}

NTSTATUS
GlobalData::InitializeGlobalData(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING DeviceName,
    _In_ PUNICODE_STRING SymbolicLink
)
{
    if (DeviceObject || IrpManager)
    {
        return STATUS_ADDRESS_ALREADY_EXISTS;
    }

    DeviceObject = new ::DeviceObject(DriverObject, DeviceName, SymbolicLink);
    if (!DeviceObject || !DeviceObject->IsValid())
    {
        MyDriverLogCritical("Not enough resources!");
        CleanUp();
        return STATUS_NO_MEMORY;
    }

    IrpManager = new ::IrpManager();
    if (!IrpManager || !IrpManager->IsValid())
    {
        MyDriverLogCritical("Not enough resources!");
        CleanUp();
        return STATUS_NO_MEMORY;
    }

    return STATUS_SUCCESS;
}

void 
GlobalData::CleanUp()
{
    if (DeviceObject)
    {
        delete DeviceObject;
        DeviceObject = nullptr;
    }

    if (IrpManager)
    {
        delete IrpManager;
        IrpManager = nullptr;
    }
}
