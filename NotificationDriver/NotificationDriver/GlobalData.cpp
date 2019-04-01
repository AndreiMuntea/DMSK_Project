#include "GlobalData.hpp"
#include "Filter.hpp"
#include "DriverTags.hpp"

#include <aux_klib.h>

static UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\MyIoctlDeviceName");
static UNICODE_STRING gSymLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyIoctlSymLink");
static UNICODE_STRING gNtoskrnl = RTL_CONSTANT_STRING(L"\\SystemRoot\\system32\\ntoskrnl.exe");


GLOBAL_DATA gDrvData;

NTSTATUS GdrvpInitializeNtImageBase()
{
    AUX_MODULE_EXTENDED_INFO* modules = nullptr;
    ULONG modulesSize = 0;
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    status = AuxKlibInitialize();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    while (!modules)
    {
        status = AuxKlibQueryModuleInformation(&modulesSize, sizeof(AUX_MODULE_EXTENDED_INFO), nullptr);
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        modules = (AUX_MODULE_EXTENDED_INFO*)ExAllocatePoolWithTag(NonPagedPool, modulesSize, DRV_TAG_MST);
        if (!modules)
        {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        status = AuxKlibQueryModuleInformation(&modulesSize, sizeof(AUX_MODULE_EXTENDED_INFO), modules);
        if (!NT_SUCCESS(status))
        {
            ExFreePoolWithTag(modules, DRV_TAG_MST);
            modules = nullptr;
        }
    }

    for (int i = 0; i < modulesSize / sizeof(AUX_MODULE_EXTENDED_INFO); ++i)
    {
        bool ok = true;

        // FullPathName is a static buffer or 256 UCHARs => ntoskrnl.exe size is lower;
        for (USHORT idx = 0; idx < gNtoskrnl.Length / 2 && ok; ++idx)
        {
            auto c1 = RtlDowncaseUnicodeChar(gNtoskrnl.Buffer[idx]);
            auto c2 = RtlDowncaseUnicodeChar(modules[i].FullPathName[idx]);
            ok = (c1 == c2);
        }
        if (ok)
        {
            gDrvData.NtBaseAddress = modules[i].BasicInfo.ImageBase;
            gDrvData.NtModuleSize = modules[i].ImageSize;
            break;
        }
    }

    if (!gDrvData.NtBaseAddress)
    {
        status = STATUS_NOT_FOUND;
    }

    ExFreePoolWithTag(modules, DRV_TAG_MST);
    return status;
}

void GdrvInitGlobalData(_In_ PDRIVER_OBJECT DriverObject)
{
    gDrvData.NtBaseAddress = nullptr;
    gDrvData.NtModuleSize = 0;

    gDrvData.Cookie = { 0 };
    gDrvData.Altitude = RTL_CONSTANT_STRING(L"320007");
    gDrvData.RegistrationHandle = nullptr;
    gDrvData.ProtectedProcessPid = 0;

    gDrvData.ObOperationRegistration[0].ObjectType = PsProcessType;
    gDrvData.ObOperationRegistration[0].Operations = OB_OPERATION_HANDLE_DUPLICATE | OB_OPERATION_HANDLE_CREATE;
    gDrvData.ObOperationRegistration[0].PreOperation = FltPreProcessCreateRoutine;
    gDrvData.ObOperationRegistration[0].PostOperation = nullptr;

    gDrvData.ObOperationRegistration[1].ObjectType = PsThreadType;
    gDrvData.ObOperationRegistration[1].Operations = OB_OPERATION_HANDLE_DUPLICATE | OB_OPERATION_HANDLE_CREATE;
    gDrvData.ObOperationRegistration[1].PreOperation = FltPreProcessCreateRoutine;
    gDrvData.ObOperationRegistration[1].PostOperation = nullptr;

    gDrvData.ObCallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
    gDrvData.ObCallbackRegistration.Altitude = gDrvData.Altitude;
    gDrvData.ObCallbackRegistration.OperationRegistrationCount = 2;
    gDrvData.ObCallbackRegistration.OperationRegistration = &gDrvData.ObOperationRegistration[0];
    gDrvData.ObCallbackRegistration.RegistrationContext = nullptr;

    gDrvData.DeviceObject = new DeviceObject(DriverObject, &gDeviceName, &gSymLink);
    NT_ASSERT(gDrvData.DeviceObject && gDrvData.DeviceObject->IsValid());

    NT_VERIFY(NT_SUCCESS(GdrvpInitializeNtImageBase()));
}

void GdrvUninitGlobalData()
{
    gDrvData.Cookie = { 0 };
    gDrvData.Altitude = { 0,0,nullptr };
    gDrvData.RegistrationHandle = nullptr;
    gDrvData.ProtectedProcessPid = 0;

    delete gDrvData.DeviceObject;
    gDrvData.DeviceObject = nullptr;

    gDrvData.ObOperationRegistration[0] = { 0 };
    gDrvData.ObOperationRegistration[1] = { 0 };
    gDrvData.ObCallbackRegistration = { 0 };
}
