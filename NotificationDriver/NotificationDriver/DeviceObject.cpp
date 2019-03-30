#include "DeviceObject.hpp"
#include "trace.hpp"
#include "DeviceObject.tmh"

#include "GlobalData.hpp"
#include "Lockguard.hpp"

DeviceObject::DeviceObject(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING DeviceName,
    _In_ PUNICODE_STRING SymbolicLink
)
{
    MyDriverLogTrace("Will create a device with name %wZ", DeviceName);
    auto status = IoCreateDevice(
        DriverObject,                   // Driver object
        sizeof(IO_REMOVE_LOCK),         // Device extension size
        DeviceName,                     // Device name
        FILE_DEVICE_UNKNOWN,            // Device type
        FILE_DEVICE_SECURE_OPEN,        // Device characteristics
        FALSE,                          // Exclusive
        &deviceObject                   // Device object
    );
    if (!NT_SUCCESS(status))
    {
        MyDriverLogCritical("IoCreateDevice failed with status 0x%X", status);
        return;
    }

    MyDriverLogTrace("Will create a symlink for device %wZ with name %wZ", DeviceName, SymbolicLink);
    status = IoCreateSymbolicLink(SymbolicLink, DeviceName);
    if (!NT_SUCCESS(status))
    {
        CleanUp();
        MyDriverLogCritical("IoCreateSymbolicLink has failed with status 0x%X", status);
        return;
    }

    IoInitializeRemoveLock(IoctlGetIoRemoveLock(), DRV_TAG_RLT, 0, 0);

    symbolicLink = SymbolicLink;
    isValid = true;
}

DeviceObject::~DeviceObject()
{
    IoAcquireRemoveLock(IoctlGetIoRemoveLock(), NULL);
    IoReleaseRemoveLockAndWait(IoctlGetIoRemoveLock(), NULL);
    CleanUp();
}

void DeviceObject::CleanUp()
{
    if (symbolicLink)
    {
        IoDeleteSymbolicLink(symbolicLink);
        symbolicLink = nullptr;
    }

    if (deviceObject)
    {
        IoDeleteDevice(deviceObject);
        deviceObject = nullptr;
    }

    isValid = false;
}

PIO_REMOVE_LOCK DeviceObject::IoctlGetIoRemoveLock()
{
    return (PIO_REMOVE_LOCK)deviceObject->DeviceExtension;
}

NTSTATUS 
DeviceObject::HandleProtectProcess(
    _Inout_ PIRP Irp,
    _Inout_ PIO_STACK_LOCATION StackLocation
)
{
    if (StackLocation->Parameters.DeviceIoControl.InputBufferLength != sizeof(unsigned __int32))
    {
        MyDriverLogError("Invalid buffer size. Expected %d. Got %d", sizeof(unsigned __int32), StackLocation->Parameters.DeviceIoControl.InputBufferLength);
        return STATUS_INVALID_BUFFER_SIZE;
    }

    ExclusiveLockguard guard(&gDrvData.Lock);

    gDrvData.ProtectedProcessPid = *(unsigned __int32*)(Irp->AssociatedIrp.SystemBuffer);
    MyDriverLogTrace("Protecting process with pid: %d", gDrvData.ProtectedProcessPid);

    return STATUS_SUCCESS;
}

NTSTATUS
DeviceObject::IoctlHandleIrpMjCreate(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    MyDriverLogTrace("IoctlHandleIrpMjCreate was called");

    IoAcquireRemoveLock(gDrvData.DeviceObject->IoctlGetIoRemoveLock(), Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IoReleaseRemoveLock(gDrvData.DeviceObject->IoctlGetIoRemoveLock(), Irp);

    return STATUS_SUCCESS;
}

NTSTATUS
DeviceObject::IoctlHandleIrpMjClose(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    MyDriverLogTrace("IoctlHandleIrpMjClose was called");
    IoAcquireRemoveLock(gDrvData.DeviceObject->IoctlGetIoRemoveLock(), Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IoReleaseRemoveLock(gDrvData.DeviceObject->IoctlGetIoRemoveLock(), Irp);

    return STATUS_SUCCESS;
}

NTSTATUS
DeviceObject::IoctlHandleIrpMjDeviceControl(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    MyDriverLogTrace("IoctlHandleIrpMjDeviceControl was called");

    IoAcquireRemoveLock(gDrvData.DeviceObject->IoctlGetIoRemoveLock(), Irp);

    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
    {
    case FIRST_IOCTL_CODE:
        MyDriverLogTrace("First IOCTL was called!");
        break;
    case SECOND_IOCTL_CODE:
        MyDriverLogTrace("Second IOCTL was called!");
        break;
    case THIRD_IOCTL_CODE:
        MyDriverLogTrace("Third IOCTL was called!");
        status = HandleProtectProcess(Irp, irpSp);
        break;
    default:
        MyDriverLogWarning("Invalid IOCTL was called!");
        status = STATUS_NOT_SUPPORTED;
        break;
    };


    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IoReleaseRemoveLock(gDrvData.DeviceObject->IoctlGetIoRemoveLock(), Irp);

    return status;
}
