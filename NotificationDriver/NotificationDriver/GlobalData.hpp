#ifndef __GLOBAL_DATA_HPP__ 
#define __GLOBAL_DATA_HPP__

#include "includes.hpp"
#include "DeviceObject.hpp"
#include "ProcessCollector.hpp"
#include "StackTrace.hpp"

typedef struct _GLOBAL_DATA
{
    LARGE_INTEGER Cookie;
    UNICODE_STRING Altitude;
    PVOID RegistrationHandle;
    DeviceObject* DeviceObject;
    OB_OPERATION_REGISTRATION ObOperationRegistration[2];
    OB_CALLBACK_REGISTRATION ObCallbackRegistration;

    Pushlock Lock;
    unsigned __int32 ProtectedProcessPid;
    StackTrace* ProcessCreateGoodStackTrace;

    ProcessCollector ProcessCollector;

    PVOID NtBaseAddress;
    SIZE_T NtModuleSize;
}GLOBAL_DATA, *PGLOBAL_DATA;

extern GLOBAL_DATA gDrvData;

void GdrvInitGlobalData(_In_ PDRIVER_OBJECT DriverObject);
void GdrvUninitGlobalData();

#endif //__GLOBAL_DATA_HPP__