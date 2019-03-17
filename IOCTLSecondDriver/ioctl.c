#include "ioctl.h"
#include "trace.h"
#include "ioctl.tmh"

static UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\MyIoctlDeviceName");

static PDEVICE_OBJECT gDeviceObject = NULL;


_Use_decl_annotations_
NTSTATUS
IoctlInitDeviceObject(
    _In_  PDRIVER_OBJECT   DriverObject
)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    IoctlSecondDriverLogInfo("Will create a device with name %wZ", &gDeviceName);
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
        IoctlSecondDriverLogError("IoCreateDevice has failed with status 0x%X", status);
        return status;
    }

    return status;
}

VOID
IoctlUninitDeviceObject()
{
    IoctlSecondDriverLogInfo("Uninit IoctlDeviceObject");
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
    IoctlSecondDriverLogInfo("IoctlHandleIrpMjDeviceControl was called");

    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
    {
    case FIRST_IOCTL_CODE:
        IoctlSecondDriverLogInfo("First IOCTL was called!");
        break;
    case SECOND_IOCTL_CODE:
        IoctlSecondDriverLogInfo("Second IOCTL was called!");
        break;
    default:
        IoctlSecondDriverLogWarning("Invalid IOCTL was called!");
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
    IoctlSecondDriverLogInfo("IoctlHandleIrpMjCreate was called");

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
    IoctlSecondDriverLogInfo("IoctlHandleIrpMjClose was called");

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}