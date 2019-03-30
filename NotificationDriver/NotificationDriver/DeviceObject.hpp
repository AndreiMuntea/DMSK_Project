#ifndef __DEVICE_OBJECT_HPP__ 
#define __DEVICE_OBJECT_HPP__

#include "cpp_includes.hpp"
#include "DriverTags.hpp"

#define IOCTL_DEVICE_TYPE   56789

#define FIRST_IOCTL_CODE    CTL_CODE(IOCTL_DEVICE_TYPE, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SECOND_IOCTL_CODE   CTL_CODE(IOCTL_DEVICE_TYPE, 0x851, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define THIRD_IOCTL_CODE    CTL_CODE(IOCTL_DEVICE_TYPE, 0x852, METHOD_BUFFERED, FILE_ANY_ACCESS)

class DeviceObject : public CppNonPagedObject<DRV_TAG_DOT>
{
public:
    DeviceObject(
        _In_ PDRIVER_OBJECT DriverObject,
        _In_ PUNICODE_STRING DeviceName,
        _In_ PUNICODE_STRING SymbolicLink
    );

    virtual ~DeviceObject();
    inline bool IsValid() const { return isValid; };

    static NTSTATUS
    IoctlHandleIrpMjCreate(
        _Inout_ PDEVICE_OBJECT DeviceObject,
        _Inout_ PIRP Irp
    );

    static NTSTATUS
    IoctlHandleIrpMjClose(
        _Inout_ PDEVICE_OBJECT DeviceObject,
        _Inout_ PIRP Irp
    );

    static NTSTATUS 
    IoctlHandleIrpMjDeviceControl(
        _Inout_ PDEVICE_OBJECT DeviceObject,
        _Inout_ PIRP Irp
    );

private:
    void CleanUp();
    PIO_REMOVE_LOCK IoctlGetIoRemoveLock();

    static NTSTATUS
    HandleProtectProcess(
        _Inout_ PIRP Irp,
        _Inout_ PIO_STACK_LOCATION StackLocation
    );

    PDEVICE_OBJECT deviceObject = nullptr;
    PUNICODE_STRING symbolicLink = nullptr;
    bool isValid = false;
};

#endif //__DEVICE_OBJECT_HPP__