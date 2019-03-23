#include "IrpManager.hpp"
#include "trace.hpp"
#include "IrpManager.tmh"

IrpManager::IrpManager()
{
    auto status = IoCsqInitialize(
        &pendingReads.Csq,
        IoctlIoCsqInsertIrp,
        IoctlIoCsqRemoveIrp,
        IoctlIoCsqPeekNextIrp,
        IoctlIoCsqAcquireLock,
        IoctlIoCsqReleaseLock,
        IoCsqCompleteCanceledIrp
    );

    if (!NT_SUCCESS(status))
    {
        MyDriverLogError("IoCsqInitialize failed with status 0x%X!", status);
        return;
    }

    InitializeListHead(&pendingReads.List);
    KeInitializeSpinLock(&pendingReads.Lock);

    status = IoCsqInitialize(
        &pendingWrites.Csq,
        IoctlIoCsqInsertIrp,
        IoctlIoCsqRemoveIrp,
        IoctlIoCsqPeekNextIrp,
        IoctlIoCsqAcquireLock,
        IoctlIoCsqReleaseLock,
        IoCsqCompleteCanceledIrp
    );

    if (!NT_SUCCESS(status))
    {
        MyDriverLogError("IoCsqInitialize failed with status 0x%X!", status);
        return;
    }

    InitializeListHead(&pendingWrites.List);
    KeInitializeSpinLock(&pendingWrites.Lock);

    isValid = true;
}

IrpManager::~IrpManager()
{
    // IoCsqRemoveNextIrp will remove first entry when passing nullptr
    // We will free both queues at unload

    auto irp = IoCsqRemoveNextIrp(&pendingReads.Csq, nullptr);
    while (irp)
    {
        irp->IoStatus.Status = STATUS_CANCELLED;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        irp = IoCsqRemoveNextIrp(&pendingReads.Csq, nullptr);
    }

    irp = IoCsqRemoveNextIrp(&pendingWrites.Csq, nullptr);
    while (irp)
    {
        irp->IoStatus.Status = STATUS_CANCELLED;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        irp = IoCsqRemoveNextIrp(&pendingWrites.Csq, nullptr);
    }
}

NTSTATUS 
IrpManager::ProcessReadIrp(
    _In_ PIRP Irp
)
{
    auto stackLocation = IoGetCurrentIrpStackLocation(Irp);
    auto status = IoAcquireRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);
    if (!NT_SUCCESS(status))
    {
        // Lock is in a removed state. That means we have already received cleaned up request for this handle. 
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    // For now we can read or write only sizeof(ULONG).
    if (stackLocation->Parameters.Read.Length != sizeof(ULONG))
    {
        Irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
        Irp->IoStatus.Information = 0;
        IoReleaseRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_BUFFER_TOO_SMALL;
    }

    auto writeIrp = IoCsqRemoveNextIrp(&pendingWrites.Csq, stackLocation->FileObject);
    if (!writeIrp)
    {
        // No pending write => we pend our current read
        IoCsqInsertIrp(&pendingReads.Csq, Irp, nullptr);
        Irp->IoStatus.Status = STATUS_PENDING;
        Irp->IoStatus.Information = sizeof(ULONG);
        IoReleaseRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);
        return STATUS_PENDING;
    }

    // Got a write => complete bot IRPs
    CompleteIrps(Irp, writeIrp);
    IoReleaseRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IoCompleteRequest(writeIrp, IO_NO_INCREMENT);

    return Irp->IoStatus.Status;
}

NTSTATUS 
IrpManager::ProcessWriteIrp(
    _In_ PIRP Irp
)
{
    auto stackLocation = IoGetCurrentIrpStackLocation(Irp);
    auto status = IoAcquireRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);
    if (!NT_SUCCESS(status))
    {
        // Lock is in a removed state. That means we have already received cleaned up request for this handle. 
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    // For now we can read or write only sizeof(ULONG).
    if (stackLocation->Parameters.Write.Length != sizeof(ULONG))
    {
        Irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
        Irp->IoStatus.Information = 0;
        IoReleaseRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_BUFFER_TOO_SMALL;
    }

    auto readIrp = IoCsqRemoveNextIrp(&pendingReads.Csq, stackLocation->FileObject);
    if (!readIrp)
    {
        // No pending read => we pend our current write
        IoCsqInsertIrp(&pendingWrites.Csq, Irp, nullptr);
        Irp->IoStatus.Status = STATUS_PENDING;
        Irp->IoStatus.Information = sizeof(ULONG);
        IoReleaseRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);
        return STATUS_PENDING;
    }

    // Got a read => complete bot IRPs
    CompleteIrps(readIrp, Irp);
    IoReleaseRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);

    IoCompleteRequest(readIrp, IO_NO_INCREMENT);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return Irp->IoStatus.Status;
}

NTSTATUS IrpManager::CancelIrp(_In_ PIRP Irp)
{
    auto stackLocation = IoGetCurrentIrpStackLocation(Irp);

    // This acquire cannot fail because you cannot get more than one cleanup for the same handle.
    auto status = IoAcquireRemoveLock((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);
    NT_VERIFY(NT_SUCCESS(status));

    // Wait for all the threads that are currently dispatching to exit and  prevent any threads dispatching I/O on the same handle beyond this point.
    IoReleaseRemoveLockAndWait((PIO_REMOVE_LOCK)stackLocation->FileObject->FsContext, Irp);

    auto irp = IoCsqRemoveNextIrp(&pendingReads.Csq, stackLocation->FileObject);
    while (irp)
    {
        irp->IoStatus.Status = STATUS_CANCELLED;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        irp = IoCsqRemoveNextIrp(&pendingReads.Csq, stackLocation->FileObject);
    }

    irp = IoCsqRemoveNextIrp(&pendingWrites.Csq, stackLocation->FileObject);
    while (irp)
    {
        irp->IoStatus.Status = STATUS_CANCELLED;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        irp = IoCsqRemoveNextIrp(&pendingWrites.Csq, stackLocation->FileObject);
    }

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

void 
IrpManager::CompleteIrps(
    _In_ PIRP ReadIrp, 
    _In_ PIRP WriteIrp
)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG information = 0;

    __try 
    {
        ProbeForWrite(ReadIrp->UserBuffer, sizeof(ULONG), 1);
        ProbeForRead(WriteIrp->UserBuffer, sizeof(ULONG), 1);

        *(PULONG)ReadIrp->UserBuffer = *(PULONG)WriteIrp->UserBuffer;
        information = sizeof(ULONG);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
    }

    WriteIrp->IoStatus.Status = status;
    WriteIrp->IoStatus.Information = information;

    ReadIrp->IoStatus.Status = status;
    ReadIrp->IoStatus.Information = information;
}


_Use_decl_annotations_
void
IoctlIoCsqInsertIrp(
    _In_ IO_CSQ     *Csq,
    _In_ PIRP       Irp
)
{
    MyDriverLogTrace("IoctlIoCsqInsertIrp was called");

    auto queue = CONTAINING_RECORD(Csq, QUEUE, Csq);
    InsertTailList(&queue->List, &Irp->Tail.Overlay.ListEntry);
}

_Use_decl_annotations_
void
IoctlIoCsqRemoveIrp(
    _In_ IO_CSQ    *Csq,
    _In_ PIRP       Irp
)
{
    UNREFERENCED_PARAMETER(Csq);
    MyDriverLogTrace("IoctlIoCsqRemoveIrp was called");

    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);
}

_Use_decl_annotations_
PIRP
IoctlIoCsqPeekNextIrp(
    _In_ PIO_CSQ Csq,
    _In_ PIRP    Irp,
    _In_ PVOID   PeekContext
)
{
    PIRP irp = nullptr;
    auto queue = CONTAINING_RECORD(Csq, QUEUE, Csq);
    auto start = (Irp)  ? Irp->Tail.Overlay.ListEntry.Flink
                        : queue->List.Flink;

    while (start != &queue->List)
    {
        irp = CONTAINING_RECORD(start, IRP, Tail.Overlay.ListEntry);
        auto irpStack = IoGetCurrentIrpStackLocation(irp);

        // nullptr as FILE_OBJECT will return the very first IRP in queue
        if (!PeekContext)
        {
            break;
        }

        // found an IRP associated with the same FILE_OBJECT 
        if (irpStack->FileObject == (PFILE_OBJECT)(PeekContext))
        {
            break;
        }

        irp = nullptr;
        start = start->Flink;
    }

    return irp;
}

void
IoctlIoCsqAcquireLock(
    _In_ PIO_CSQ Csq,
    _Out_ PKIRQL Irql
)
{
    MyDriverLogTrace("IoctlIoCsqAcquireLock was called");

    auto queue = CONTAINING_RECORD(Csq, QUEUE, Csq);
    KeAcquireSpinLock(&queue->Lock, Irql);
}

void
IoctlIoCsqReleaseLock(
    _In_ PIO_CSQ Csq,
    _In_ KIRQL Irql
)
{

    MyDriverLogTrace("IoctlIoCsqReleaseLock was called");

    auto queue = CONTAINING_RECORD(Csq, QUEUE, Csq);
    KeReleaseSpinLock(&queue->Lock, Irql);
}

void
IoCsqCompleteCanceledIrp(
    _In_ PIO_CSQ Csq,
    _In_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(Csq);
    MyDriverLogTrace("IoCsqCompleteCanceledIrp was called");

    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
}
