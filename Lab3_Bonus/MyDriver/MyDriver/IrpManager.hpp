#ifndef __IRP_MANAGER_HPP__ 
#define __IRP_MANAGER_HPP__

#include "cpp_includes.hpp"
#include "DriverTags.hpp"
#include <ntddk.h>


IO_CSQ_INSERT_IRP IoctlIoCsqInsertIrp;
IO_CSQ_REMOVE_IRP IoctlIoCsqRemoveIrp;
IO_CSQ_PEEK_NEXT_IRP IoctlIoCsqPeekNextIrp;
IO_CSQ_ACQUIRE_LOCK IoctlIoCsqAcquireLock;
IO_CSQ_RELEASE_LOCK IoctlIoCsqReleaseLock;
IO_CSQ_COMPLETE_CANCELED_IRP IoCsqCompleteCanceledIrp;


typedef struct _QUEUE
{
    IO_CSQ Csq;
    LIST_ENTRY List;
    KSPIN_LOCK Lock;
}QUEUE, *PQUEUE;

class IrpManager : public CppNonPagedObject<DRV_TAG_IMT>
{
public:
    IrpManager();
    virtual ~IrpManager();

    inline bool IsValid() const { return isValid; }

    NTSTATUS ProcessReadIrp(_In_ PIRP Irp);
    NTSTATUS ProcessWriteIrp(_In_ PIRP Irp);
    NTSTATUS CancelIrp(_In_ PIRP Irp);

private:
    void CompleteIrps(_In_ PIRP ReadIrp, _In_ PIRP WriteIrp);

    bool isValid = false;
    QUEUE pendingWrites;
    QUEUE pendingReads;
};


#endif //__IRP_MANAGER_HPP__