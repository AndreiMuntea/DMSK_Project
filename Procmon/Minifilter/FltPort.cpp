#include "FltPort.hpp"
#include "trace.hpp"
#include "FltPort.tmh"

#include "GlobalData.hpp"
#include <cpp_lockguard.hpp>

FltPort::FltPort(
    _In_ PFLT_FILTER Filter,
    _In_ PUNICODE_STRING PortName,
    _In_ PFUNC_OnMessageCallback OnMessageCallback
) : filter{ Filter },
    onMessageCallback{ OnMessageCallback }
{
    OBJECT_ATTRIBUTES objAttr = { 0 };
    PSECURITY_DESCRIPTOR securityDescriptor = nullptr;

    auto status = ::FltBuildDefaultSecurityDescriptor(&securityDescriptor, FLT_PORT_ALL_ACCESS);
    if (!NT_VERIFY(NT_SUCCESS(status)))
    {
        MyDriverLogError("::FltBuildDefaultSecurityDescriptor has failed with status 0x%x", status);
        return;
    }

    InitializeObjectAttributes(&objAttr, PortName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, nullptr, securityDescriptor);

    status = ::FltCreateCommunicationPort(
        Filter,                                 // Filter   
        &this->serverPort,                      // ServerPort
        &objAttr,                               // ObjectAttributes
        this,                                   // ServerPortCookie
        &FltPort::ConnectNotifyCallback,        // ConnectNotifyCallback
        &FltPort::DisconnectNotifyCallback,     // DisconnectNotifyCallback
        &FltPort::MessageNotifyCallback,        // MessageNotifyCallback
        1                                       // MaxConnections
    );
    if (!NT_SUCCESS(status))
    {
        MyDriverLogError("::FltCreateCommunicationPort has failed with status 0x%x", status);
        ::FltFreeSecurityDescriptor(securityDescriptor);
        return;
    }

    ::FltFreeSecurityDescriptor(securityDescriptor);
    this->Validate();

    MyDriverLogTrace("Created server communication serverPort %wZ", PortName);
}

FltPort::~FltPort()
{
    if (!this->IsValid())
    {
        return;
    }

    Cpp::ExclusiveLockguard guard(&this->lock);

    CloseClientPort();
    CloseServerPort();
}

void FltPort::CloseClientPort()
{
    if (this->clientPort)
    {
        MyDriverLogTrace("Closing client communication port");
        ::FltCloseClientPort(this->filter, &this->clientPort);
        this->clientPort = nullptr;
    }
}

void FltPort::CloseServerPort()
{
    if (this->serverPort)
    {
        MyDriverLogTrace("Closing server communication port");
        ::FltCloseCommunicationPort(this->serverPort);
        this->serverPort = nullptr;
    }
}

NTSTATUS FLTAPI
FltPort::ConnectNotifyCallback(
    _In_ PFLT_PORT ClientPort,
    _In_opt_ PVOID ServerPortCookie,
    _In_reads_bytes_opt_(SizeOfContext) PVOID ConnectionContext,
    _In_ ULONG SizeOfContext,
    _Outptr_result_maybenull_ PVOID *ConnectionPortCookie
)
{
    UNREFERENCED_PARAMETER(ConnectionContext);
    UNREFERENCED_PARAMETER(SizeOfContext);

    MyDriverLogTrace("We are in ConnectNotifyCallback");
    
    FltPort* port = static_cast<FltPort*>(ServerPortCookie);
    Cpp::ExclusiveLockguard guard(&port->lock);

    if (port->clientPort)
    {
        MyDriverLogWarning("A client is already connected");
        return STATUS_ALREADY_REGISTERED;
    }

    port->clientPort = ClientPort;
    *ConnectionPortCookie = port;

    return STATUS_SUCCESS;
}

void FLTAPI
FltPort::DisconnectNotifyCallback(
    _In_opt_ PVOID ConnectionCookie
)
{
    MyDriverLogTrace("We are in DisconnectNotifyCallback");

    FltPort* fltPort = static_cast<FltPort*>(ConnectionCookie);

    Cpp::ExclusiveLockguard guard(&fltPort->lock);
    fltPort->CloseClientPort();
}

NTSTATUS FLTAPI
FltPort::MessageNotifyCallback(
    _In_opt_ PVOID PortCookie,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_to_opt_(OutputBufferLength, *ReturnOutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength,
    _Out_ PULONG ReturnOutputBufferLength
)
{
    MyDriverLogTrace("We are in MessageNotifyCallback");

    FltPort* fltPort = static_cast<FltPort*>(PortCookie);

    Cpp::SharedLockguard guard(&fltPort->lock);
    return fltPort->onMessageCallback(InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, ReturnOutputBufferLength);
}
