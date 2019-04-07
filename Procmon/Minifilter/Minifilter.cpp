#include <fltKernel.h>
#include <cpp_init.hpp>

#include "DriverRoutines.hpp"
#include "trace.hpp"
#include "Minifilter.tmh"

#include "GlobalData.hpp"

EXTERN_C_START

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, DriverEntry)
    #pragma alloc_text(PAGE, DriverUnload)
    #pragma alloc_text(PAGE, InstanceSetup)
    #pragma alloc_text(PAGE, InstanceQueryTeardown)
    #pragma alloc_text(PAGE, InstanceTeardownStart)
    #pragma alloc_text(PAGE, InstanceTeardownComplete)
#endif

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    RegistryPath
)
{
    WPP_INIT_TRACING(DriverObject, RegistryPath);
    MyDriverLogTrace("We are now in driver entry routine!");

    Cpp::CppInitializeLib();
    GdrvInitGlobalData(DriverObject);

    auto status = ::FltRegisterFilter(DriverObject, &gDrvData.FilterRegistration, &gDrvData.FilterHandle);
    if (!NT_SUCCESS(status))
    {
        MyDriverLogCritical("::FltRegisterFilter failed with status 0x%x", status);
        goto Exit;
    }

    gDrvData.CommunicationPort.Update(new FltPort(gDrvData.FilterHandle, &gDrvData.CommunicationPortName, nullptr));
    if (!gDrvData.CommunicationPort.IsValid() || !gDrvData.CommunicationPort->IsValid())
    {
        ::FltUnregisterFilter(gDrvData.FilterHandle);
        MyDriverLogCritical("Failed to initialize Filter communication port");
        goto Exit;
    }

    status = ::FltStartFiltering(gDrvData.FilterHandle);
    if (!NT_SUCCESS(status))
    {
        ::FltUnregisterFilter(gDrvData.FilterHandle);
        MyDriverLogCritical("::FltStartFiltering failed with status 0x%x", status);
        goto Exit;
    }

Exit:
    if (!NT_SUCCESS(status))
    {
        WPP_CLEANUP(gDrvData.DriverObject);
        GdrvUninitGlobalData();
        Cpp::CppUninitializeLib();
    }
    return status;
}

NTSTATUS
DriverUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(Flags);

    MyDriverLogTrace("We are now in driver unload routine!");

    WPP_CLEANUP(gDrvData.DriverObject);
    
    GdrvUninitGlobalData();
    Cpp::CppUninitializeLib();

    ::FltUnregisterFilter(gDrvData.FilterHandle);

    return STATUS_SUCCESS;
}


NTSTATUS
InstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(VolumeDeviceType);
    UNREFERENCED_PARAMETER(VolumeFilesystemType);


    MyDriverLogTrace("We are now in instance setup routine!");
    return STATUS_SUCCESS;
}

NTSTATUS
InstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    MyDriverLogTrace("We are now in instance query teardown routine!");
    return STATUS_SUCCESS;
}

VOID
InstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Reason
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Reason);

    MyDriverLogTrace("We are now in instance teardown start routine!");
}

VOID
InstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Reason
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Reason);

    MyDriverLogTrace("We are now in instance teardown complete routine!");
}

EXTERN_C_END