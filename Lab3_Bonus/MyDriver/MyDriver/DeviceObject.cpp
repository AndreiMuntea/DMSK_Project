#include "DeviceObject.hpp"
#include "trace.hpp"
#include "DeviceObject.tmh"

DeviceObject::DeviceObject(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING DeviceName,
    _In_ PUNICODE_STRING SymbolicLink
)
{
    MyDriverLogTrace("Will create a device with name %wZ", DeviceName);
    auto status = IoCreateDevice(
        DriverObject,                   // Driver object
        0,                              // Device extension size
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

    symbolicLink = SymbolicLink;
    isValid = true;
}

DeviceObject::~DeviceObject()
{
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
