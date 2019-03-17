#ifndef __IOCTL_H__ 
#define __IOCTL_H__

#include <ntddk.h>

_Must_inspect_result_
NTSTATUS
IoctlInitDeviceObject(
    _In_  PDRIVER_OBJECT   DriverObject
);


VOID
IoctlUninitDeviceObject();

#endif //__IOCTL_H__ 