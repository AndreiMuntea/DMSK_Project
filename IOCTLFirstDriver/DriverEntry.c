#include <ntddk.h>

#include "trace.h"
#include "DriverEntry.tmh"
#include "ioctl.h"


DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

void
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    IoctlFirstDriverLogTrace("We are now in driver unload routine!");

    IoctlUninitDeviceObject();

    WPP_CLEANUP(DriverObject);
}


NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    WPP_INIT_TRACING(DriverObject, RegistryPath);
    IoctlFirstDriverLogTrace("We are now in driver entry routine!");

    NTSTATUS status = STATUS_UNSUCCESSFUL;

    DriverObject->DriverUnload = DriverUnload;

    status = IoctlInitDeviceObject(DriverObject);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = IoctlHandleIrpMjCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = IoctlHandleIrpMjClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoctlHandleIrpMjDeviceControl;

    return STATUS_SUCCESS;
}