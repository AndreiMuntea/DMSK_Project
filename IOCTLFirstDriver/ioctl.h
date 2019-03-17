#ifndef __IOCTL_H__ 
#define __IOCTL_H__

#include <ntddk.h>

#define IOCTL_DEVICE_TYPE   56789

#define FIRST_IOCTL_CODE    CTL_CODE(IOCTL_DEVICE_TYPE, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SECOND_IOCTL_CODE   CTL_CODE(IOCTL_DEVICE_TYPE, 0x851, METHOD_BUFFERED, FILE_ANY_ACCESS)

_Must_inspect_result_
NTSTATUS
IoctlInitDeviceObject(
    _In_  PDRIVER_OBJECT   DriverObject
);

VOID
IoctlUninitDeviceObject();

_Must_inspect_result_
_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
NTSTATUS
IoctlHandleIrpMjDeviceControl(
    _Inout_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
);

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

#endif //__IOCTL_H__ 