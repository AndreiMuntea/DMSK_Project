#include "IoctlRoutines.hpp"
#include "trace.hpp"
#include "IoctlRoutines.tmh"
#include "DriverGlobals.hpp"



_Use_decl_annotations_
NTSTATUS
IoctlHandleIrpMjCreate(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    MyDriverLogTrace("IoctlHandleIrpMjCreate was called");

    NTSTATUS status = STATUS_SUCCESS;
    auto stackLocation = IoGetCurrentIrpStackLocation(Irp);

    stackLocation->FileObject->FsContext = ExAllocatePoolWithTag(NonPagedPool, sizeof(IO_REMOVE_LOCK), DRV_TAG_RLT);
    if (!stackLocation->FileObject->FsContext)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }
    IoInitializeRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, DRV_TAG_RLT, 0, 0);

Exit:
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

_Use_decl_annotations_
NTSTATUS
IoctlHandleIrpMjClose(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    MyDriverLogTrace("IoctlHandleIrpMjClose was called");

    auto stackLocation = IoGetCurrentIrpStackLocation(Irp);
    ExFreePoolWithTag(stackLocation->FileObject->FsContext, DRV_TAG_RLT);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS
IoctlHandleIrpMjRead(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    MyDriverLogTrace("IoctlHandleIrpMjRead was called");

    return gDrvData.IrpManager->ProcessReadIrp(Irp);
}

_Use_decl_annotations_
NTSTATUS
IoctlHandleIrpMjWrite(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    MyDriverLogTrace("IoctlHandleIrpMjWrite was called");

    return gDrvData.IrpManager->ProcessWriteIrp(Irp);
}

_Use_decl_annotations_
NTSTATUS
IoctlHandleIrpMjCleanup(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    MyDriverLogTrace("IoctlHandleIrpMjCleanup was called");

    return gDrvData.IrpManager->CancelIrp(Irp);
}