#ifndef __IOCTL_ROUTINES_HPP__ 
#define __IOCTL_ROUTINES_HPP__

#include <ntddk.h>

_Must_inspect_result_
_Dispatch_type_(IRP_MJ_CREATE)
NTSTATUS
IoctlHandleIrpMjCreate(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
);

_Must_inspect_result_
_Dispatch_type_(IRP_MJ_CLOSE)
NTSTATUS
IoctlHandleIrpMjClose(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
);

_Must_inspect_result_
_Dispatch_type_(IRP_MJ_READ)
NTSTATUS
IoctlHandleIrpMjRead(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
);

_Must_inspect_result_
_Dispatch_type_(IRP_MJ_WRITE)
NTSTATUS
IoctlHandleIrpMjWrite(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
);

_Must_inspect_result_
_Dispatch_type_(IRP_MJ_CLEANUP)
NTSTATUS
IoctlHandleIrpMjCleanup(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
);

#endif //__IOCTL_ROUTINES_HPP__