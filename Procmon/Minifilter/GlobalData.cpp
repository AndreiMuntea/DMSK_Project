#include "GlobalData.hpp"
#include "DriverRoutines.hpp"

GLOBAL_DATA gDrvData;

static UNICODE_STRING gFltPortName = RTL_CONSTANT_STRING(L"\\MyCommunicationPort");

static CONST FLT_OPERATION_REGISTRATION Callbacks[] =
{
    { IRP_MJ_OPERATION_END }
};

static CONST FLT_REGISTRATION FilterRegistration =
{
    sizeof(FLT_REGISTRATION),   //  Size
    FLT_REGISTRATION_VERSION,   //  Version
    0,                          //  Flags

    nullptr,                    //  Context
    Callbacks,                  //  Operation callbacks

    DriverUnload,               //  MiniFilterUnload

    InstanceSetup,              //  InstanceSetup
    InstanceQueryTeardown,      //  InstanceQueryTeardown
    InstanceTeardownStart,      //  InstanceTeardownStart
    InstanceTeardownComplete,   //  InstanceTeardownComplete

    nullptr,                    //  GenerateFileName
    nullptr,                    //  GenerateDestinationFileName
    nullptr                     //  NormalizeNameComponent

};


void GdrvInitGlobalData(_In_ PDRIVER_OBJECT DriverObject)
{
    gDrvData.DriverObject = DriverObject;
    gDrvData.FilterHandle = nullptr;
    gDrvData.FilterRegistration = FilterRegistration;
    gDrvData.CommunicationPortName = gFltPortName;
}

void GdrvUninitGlobalData()
{
    gDrvData.CommunicationPort.Reset();
    gDrvData.CommunicationPortName = { 0,0,nullptr };
}
