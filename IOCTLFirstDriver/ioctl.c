#include "ioctl.h"
#include "trace.h"
#include "ioctl.tmh"

static UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\MyIoctlDeviceName");
static UNICODE_STRING gSymbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyIoctlSymLink");
static UNICODE_STRING gDeviceNameSecondDriver = RTL_CONSTANT_STRING(L"\\Device\\MyIoctlDeviceName2");

static PDEVICE_OBJECT gDeviceObject = NULL;
static PDEVICE_OBJECT gDeviceObjectSecondDriver = NULL;
static PFILE_OBJECT gFileObjectSecondDriver = NULL;

static NTSTATUS
IoctlCompletionRoutine(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp,
    _In_opt_ PVOID Context
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Context);

    IoctlFirstDriverLogInfo("Completion Routine for IRP 0x%p was completed with status 0x%X", Irp, Irp->IoStatus.Status);
    return STATUS_SUCCESS;
}

static NTSTATUS 
IoctlPassIrp(
    _Inout_ PIRP Irp
)
{
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoctlFirstDriverLogInfo("Setting Completion routine for IRP 0x%p", Irp);
    IoSetCompletionRoutine(Irp, IoctlCompletionRoutine, NULL, TRUE, TRUE, TRUE);
    return IoCallDriver(gDeviceObjectSecondDriver, Irp);
}

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
        goto CleanUp;
    }

    status = IoGetDeviceObjectPointer(&gDeviceNameSecondDriver, FILE_ALL_ACCESS, &gFileObjectSecondDriver, &gDeviceObjectSecondDriver);
    if (!NT_SUCCESS(status))
    {
        IoctlFirstDriverLogError("IoGetDeviceObjectPointer has failed with status 0x%X", status);
        goto CleanUp;
    }
    gDeviceObject->StackSize = gDeviceObjectSecondDriver->StackSize + 1;

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
        if (gDeviceObject)
        {
            IoDeleteDevice(gDeviceObject);
            gDeviceObject = NULL;
        }
        if (gFileObjectSecondDriver)
        {
            ObDereferenceObject(gFileObjectSecondDriver);
            gFileObjectSecondDriver = NULL;
            gDeviceObjectSecondDriver = NULL;
        }
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

    if (gFileObjectSecondDriver)
    {
        ObDereferenceObject(gFileObjectSecondDriver);
        gFileObjectSecondDriver = NULL;
        gDeviceObjectSecondDriver = NULL;
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

    if (!NT_SUCCESS(status))
    {
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    return IoctlPassIrp(Irp);
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

    return IoctlPassIrp(Irp);
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

    return IoctlPassIrp(Irp);
}