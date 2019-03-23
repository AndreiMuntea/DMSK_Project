#ifndef __DEVICE_OBJECT_HPP__ 
#define __DEVICE_OBJECT_HPP__

#include "cpp_includes.hpp"
#include "DriverTags.hpp"

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

private:
    void CleanUp();

    PDEVICE_OBJECT deviceObject = nullptr;
    PUNICODE_STRING symbolicLink = nullptr;
    bool isValid = false;
};

#endif //__DEVICE_OBJECT_HPP__