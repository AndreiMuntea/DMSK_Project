#include <ntddk.h>

#include "trace.hpp"
#include "DriverEntry.tmh"

#include "cpp_includes.hpp"
#include "DriverGlobals.hpp"
#include "IoctlRoutines.hpp"

static UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\MyPipeDeviceName");
static UNICODE_STRING gSymbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyPipeSymLink");

EXTERN_C_START

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    RegistryPath
)
{
    NTSTATUS status = STATUS_SUCCESS;

    WPP_INIT_TRACING(DriverObject, RegistryPath);
    MyDriverLogTrace("We are now in driver entry routine!");

    DriverObject->DriverUnload = DriverUnload;

    DriverObject->MajorFunction[IRP_MJ_CREATE] = IoctlHandleIrpMjCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = IoctlHandleIrpMjClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = IoctlHandleIrpMjCleanup;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = IoctlHandleIrpMjWrite;
    DriverObject->MajorFunction[IRP_MJ_READ] = IoctlHandleIrpMjRead;

    CppInitializeDestructorsList();
    CppCallStaticConstructors(__crtXca, __crtXcz);

    status = gDrvData.InitializeGlobalData(DriverObject, &gDeviceName, &gSymbolicLink);
    if (!NT_SUCCESS(status))
    {
        goto CleanUp;
    }


CleanUp:
    if (!NT_SUCCESS(status))
    {
        CppCallStaticDestructors();
    }
    return status;
}

void
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    MyDriverLogTrace("We are now in driver unload routine!");

    CppCallStaticDestructors();

    WPP_CLEANUP(DriverObject);
}

EXTERN_C_END