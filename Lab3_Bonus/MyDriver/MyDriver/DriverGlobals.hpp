#ifndef __DRIVER_GLOBALS_HPP__ 
#define __DRIVER_GLOBALS_HPP__ 

#include <ntddk.h>
#include "cpp_includes.hpp"
#include "DeviceObject.hpp"
#include "IrpManager.hpp"

class GlobalData : public CppNonPagedObject<DRV_TAG_GDT>
{
public:
    GlobalData() = default;
    virtual ~GlobalData();

    _Must_inspect_result_
    NTSTATUS
    InitializeGlobalData(
        _In_ PDRIVER_OBJECT DriverObject,
        _In_ PUNICODE_STRING DeviceName,
        _In_ PUNICODE_STRING SymbolicLink
    );

    void CleanUp();

    DeviceObject* DeviceObject = nullptr;
    IrpManager* IrpManager = nullptr;
};

extern GlobalData gDrvData;

#endif //__DRIVER_GLOBALS_HPP__