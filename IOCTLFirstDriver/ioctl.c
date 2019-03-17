#include "ioctl.h"
#include "trace.h"
#include "ioctl.tmh"

static UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\MyIoctlDeviceName");
static UNICODE_STRING gSymbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyIoctlSymLink");

static PDEVICE_OBJECT gDeviceObject = NULL;


_Use_decl_annotations_
NTSTATUS 
IoctlInitDeviceObject(
    _In_  PDRIVER_OBJECT   DriverObject
)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    IoctlFirstDriverLogInfo("Will create a device with name %wZ", &gDeviceName);
    status = IoCreateDevice(
        DriverObject,                   // Driver object
        0,                              // Device extension size
        &gDeviceName,                   // Device name
        FILE_DEVICE_UNKNOWN,            // Device type
        FILE_DEVICE_SECURE_OPEN,        // Device characteristics
        FALSE,                          // Exclusive
        &gDeviceObject                  // Device object
    );

    if (!NT_SUCCESS(status))
    {
        IoctlFirstDriverLogError("IoCreateDevice has failed with status 0x%X", status);
        return status;
    }

    IoctlFirstDriverLogInfo("Will create a symlink for device %wZ with name %wZ", &gDeviceName, &gSymbolicLink);
    status = IoCreateSymbolicLink(&gSymbolicLink, &gDeviceName);
    if (!NT_SUCCESS(status))
    {
        IoctlFirstDriverLogError("IoCreateSymbolicLink has failed with status 0x%X", status);
        goto CleanUp;
    }

CleanUp:
    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(gDeviceObject);
        gDeviceObject = NULL;
    }

    return status;
}

VOID 
IoctlUninitDeviceObject()
{
    IoctlFirstDriverLogInfo("Deleting symbolic link %wZ", &gSymbolicLink);
    NTSTATUS status = IoDeleteSymbolicLink(&gSymbolicLink);
    if (!NT_SUCCESS(status))
    {
        IoctlFirstDriverLogError("IoDeleteSymbolicLink has failed with status 0x%X", status);
    }

    IoctlFirstDriverLogInfo("Uninit IoctlDeviceObject");
    if (gDeviceObject)
    {
        IoDeleteDevice(gDeviceObject);
        gDeviceObject = NULL;
    }
}
