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

_Use_decl_annotations_
NTSTATUS 
IoctlHandleIrpMjDeviceControl(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    IoctlFirstDriverLogInfo("IoctlHandleIrpMjDeviceControl was called");

    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
    {
    case FIRST_IOCTL_CODE:
        IoctlFirstDriverLogInfo("First IOCTL was called!");
        break;
    case SECOND_IOCTL_CODE:
        IoctlFirstDriverLogInfo("Second IOCTL was called!");
        break;
    default:
        IoctlFirstDriverLogWarning("Invalid IOCTL was called!");
        status = STATUS_NOT_SUPPORTED;
        break;
    };

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

_Use_decl_annotations_
NTSTATUS
IoctlHandleIrpMjCreate(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    IoctlFirstDriverLogInfo("IoctlHandleIrpMjCreate was called");

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS
IoctlHandleIrpMjClose(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    IoctlFirstDriverLogInfo("IoctlHandleIrpMjClose was called");

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}