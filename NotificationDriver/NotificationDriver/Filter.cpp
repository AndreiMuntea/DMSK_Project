#include "includes.hpp"
#include "Filter.hpp"
#include "trace.hpp"
#include "Filter.tmh"

#include "GlobalData.hpp"
#include "Lockguard.hpp"

#define TERMINATE_ACCESS_RIGHT  0x0001

void FltpHandleProcessCreate(
    _Inout_ PEPROCESS Process,
    _In_ HANDLE ProcessId,
    _In_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    UNREFERENCED_PARAMETER(Process);

    MyDriverLogInfo(
        "[Process Created] [PID] = %d  [ImageName] = %wZ  [CommandLine] = %wZ",
        (unsigned __int32)(SIZE_T)ProcessId,
        CreateInfo->ImageFileName,
        CreateInfo->CommandLine
    );
}

void FltpHandleProcessTerminate(
    _Inout_ PEPROCESS Process,
    _In_ HANDLE ProcessId
)
{
    UNREFERENCED_PARAMETER(Process);
    UNREFERENCED_PARAMETER(ProcessId);

    MyDriverLogInfo(
        "[Process terminated] [PID] = %d",
        (unsigned __int32)(SIZE_T)ProcessId
    );
}


void 
FltCreateProcessNotifyRoutine(
    _Inout_ PEPROCESS Process,
    _In_ HANDLE ProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    (CreateInfo != nullptr) ? FltpHandleProcessCreate(Process, ProcessId, CreateInfo)
                            : FltpHandleProcessTerminate(Process, ProcessId);
}

NTSTATUS 
FltRegistryNotifyRoutine(
    _In_ PVOID CallbackContext,
    _In_opt_ PVOID Argument1,
    _In_opt_ PVOID Argument2
)
{
    UNREFERENCED_PARAMETER(CallbackContext);

    auto regClass = (REG_NOTIFY_CLASS)(ULONG_PTR)(Argument1);
    if (regClass != RegNtPreRenameKey)
    {
        return STATUS_SUCCESS;
    }

    auto regInfo = (REG_RENAME_KEY_INFORMATION*)(Argument2);
    if (!regInfo)
    {
        MyDriverLogInfo("Registry rename operation with no details");
        return STATUS_SUCCESS;
    }

    PCUNICODE_STRING currentKeyName = nullptr;
    auto status = CmCallbackGetKeyObjectIDEx(&gDrvData.Cookie, regInfo->Object, nullptr, &currentKeyName, 0);
    if (!NT_SUCCESS(status))
    {
        MyDriverLogError("CmCallbackGetKeyObjectIDEx has failed with status 0x%x", status);
        return status;
    }

    MyDriverLogInfo("Registry rename. [Current key name] = %wZ [New key name] = %wZ", currentKeyName, regInfo->NewName);

    CmCallbackReleaseKeyObjectIDEx(currentKeyName);
    return STATUS_SUCCESS;
}

void
FltLoadImageNotifyRoutine(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,
    _In_ PIMAGE_INFO ImageInfo
)
{
    UNREFERENCED_PARAMETER(ProcessId);
    if (ImageInfo->SystemModeImage)
    {
        MyDriverLogInfo("Driver image loaded %wZ", FullImageName);
    }
}

OB_PREOP_CALLBACK_STATUS
FltPreProcessCreateRoutine(
    _In_ PVOID RegistrationContext,
    _Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
    UNREFERENCED_PARAMETER(RegistrationContext);
    UNREFERENCED_PARAMETER(OperationInformation);

    HANDLE targetProcessId = 0;

    // Block only UM requests
    if (OperationInformation->KernelHandle == 1)
    {
        return OB_PREOP_SUCCESS;
    }

    if (OperationInformation->ObjectType == *PsProcessType)
    {
        targetProcessId = PsGetProcessId((PEPROCESS)(OperationInformation->Object));
    }
    else if (OperationInformation->ObjectType == *PsThreadType)
    {
        targetProcessId = PsGetThreadProcessId((PETHREAD)(OperationInformation->Object));
    }

    // Ignore requests coming from same process
    if (PsGetCurrentProcessId() == targetProcessId)
    {
        return OB_PREOP_SUCCESS;
    }

    LockGuard guard(&gDrvData.Lock);

    // No pid set
    if (gDrvData.ProtectedProcessPid == 0)
    {
        return OB_PREOP_SUCCESS;
    }

    // Ignore requests to different process
    if (targetProcessId != (HANDLE)gDrvData.ProtectedProcessPid)
    {
        return OB_PREOP_SUCCESS;
    }

    // Remove terminate access right
    if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
    {
        OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~TERMINATE_ACCESS_RIGHT;
    }
    else if (OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
    {
        OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess &= ~TERMINATE_ACCESS_RIGHT;
    }

    return OB_PREOP_SUCCESS;
}
