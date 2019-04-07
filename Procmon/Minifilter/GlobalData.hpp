#ifndef __GLOBAL_DATA_HPP__ 
#define __GLOBAL_DATA_HPP__

#include <fltKernel.h>
#include <FltPort.hpp>
#include <cpp_unique_pointer.hpp>

typedef struct _GLOBAL_DATA
{
    PDRIVER_OBJECT DriverObject;
    PFLT_FILTER FilterHandle;
    FLT_REGISTRATION FilterRegistration;
    
    UNICODE_STRING CommunicationPortName;
    Cpp::UniquePointer<FltPort> CommunicationPort;
}GLOBAL_DATA, *PGLOBAL_DATA;

extern GLOBAL_DATA gDrvData;

void GdrvInitGlobalData(_In_ PDRIVER_OBJECT DriverObject);
void GdrvUninitGlobalData();

#endif //__GLOBAL_DATA_HPP__