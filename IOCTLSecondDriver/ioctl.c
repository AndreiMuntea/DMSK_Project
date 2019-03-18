#include "ioctl.h"
#include "trace.h"
#include "ioctl.tmh"

static UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\MyIoctlDeviceName2");

static PDEVICE_OBJECT gDeviceObject = NULL;

#define IO_REMOVE_LOCK_ALLOC_TAG    'TLR#'  // #RLT - Remove Lock Tag

static __forceinline PIO_REMOVE_LOCK
IoctlGetIoRemoveLock()
{
    return gDeviceObject->DeviceExtension;
}


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
        sizeof(IO_REMOVE_LOCK),         // Device extension size
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

    IoInitializeRemoveLock(IoctlGetIoRemoveLock(), IO_REMOVE_LOCK_ALLOC_TAG, 0, 0);

    return status;
}

VOID
IoctlUninitDeviceObject()
{
    IoAcquireRemoveLock(IoctlGetIoRemoveLock(), NULL);
    IoReleaseRemoveLockAndWait(IoctlGetIoRemoveLock(), NULL);

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

    IoAcquireRemoveLock(IoctlGetIoRemoveLock(), Irp);
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
    IoReleaseRemoveLock(IoctlGetIoRemoveLock(), Irp);

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
    IoAcquireRemoveLock(IoctlGetIoRemoveLock(), Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IoReleaseRemoveLock(IoctlGetIoRemoveLock(), Irp);

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
    IoAcquireRemoveLock(IoctlGetIoRemoveLock(), Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IoReleaseRemoveLock(IoctlGetIoRemoveLock(), Irp);

    return STATUS_SUCCESS;
}