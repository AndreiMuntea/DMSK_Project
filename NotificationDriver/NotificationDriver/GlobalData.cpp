#include "GlobalData.hpp"
#include "Filter.hpp"

static UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\MyIoctlDeviceName");
static UNICODE_STRING gSymLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyIoctlSymLink");

GLOBAL_DATA gDrvData;

void GdrvInitGlobalData(_In_ PDRIVER_OBJECT DriverObject)
{
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
