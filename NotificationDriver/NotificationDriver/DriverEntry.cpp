#include "includes.hpp"
#include "trace.hpp"
#include "DriverEntry.tmh"

#include "cpp_includes.hpp"
#include "Filter.hpp"
#include "GlobalData.hpp"


EXTERN_C_START

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    RegistryPath
)
{
    WPP_INIT_TRACING(DriverObject, RegistryPath);
    MyDriverLogTrace("We are now in driver entry routine!");

    CppInitializeDestructorsList();
    CppCallStaticConstructors(__crtXca, __crtXcz);

    GdrvInitGlobalData(DriverObject);

    DriverObject->DriverUnload = DriverUnload;

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DeviceObject::IoctlHandleIrpMjCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DeviceObject::IoctlHandleIrpMjClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceObject::IoctlHandleIrpMjDeviceControl;

    /// For simplicity NT_ASSERT to trigger KeBugCheck (dbg builds) if any routine fails to set
    auto status = PsSetCreateProcessNotifyRoutineEx(FltCreateProcessNotifyRoutine, false);
    NT_ASSERT(NT_SUCCESS(status));

    status = CmRegisterCallbackEx(FltRegistryNotifyRoutine, &gDrvData.Altitude, DriverObject, nullptr, &gDrvData.Cookie, nullptr);
    NT_ASSERT(NT_SUCCESS(status));

    status = PsSetLoadImageNotifyRoutine(FltLoadImageNotifyRoutine);
    NT_ASSERT(NT_SUCCESS(status));

    status = ObRegisterCallbacks(&gDrvData.ObCallbackRegistration, &gDrvData.RegistrationHandle);
    NT_ASSERT(NT_SUCCESS(status));

    return STATUS_SUCCESS;
}

void
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    MyDriverLogTrace("We are now in driver unload routine!");

    auto status = PsSetCreateProcessNotifyRoutineEx(FltCreateProcessNotifyRoutine, true);
    NT_ASSERT(NT_SUCCESS(status));
    
    status = CmUnRegisterCallback(gDrvData.Cookie);
    NT_ASSERT(NT_SUCCESS(status));

    status = PsRemoveLoadImageNotifyRoutine(FltLoadImageNotifyRoutine);
    NT_ASSERT(NT_SUCCESS(status));

    ObUnRegisterCallbacks(gDrvData.RegistrationHandle);
    
    GdrvUninitGlobalData();
    CppCallStaticDestructors();

    WPP_CLEANUP(DriverObject);
}

EXTERN_C_END