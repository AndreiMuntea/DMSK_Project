#include "includes.hpp"
#include "trace.hpp"
#include "DriverEntry.tmh"

#include "cpp_includes.hpp"
#include "ThreadPool.hpp"
#include "test.hpp"


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

    DriverObject->DriverUnload = DriverUnload;

    MyDriverLogTrace("Running tests with a Spinlock!");
    TestThreadPoolCounter(TestThreadPoolLockType::Spinlock);
    TestThreadPoolCancel(TestThreadPoolLockType::Spinlock);

    MyDriverLogTrace("Running tests with an Eresource!");
    TestThreadPoolCounter(TestThreadPoolLockType::Eresource);
    TestThreadPoolCancel(TestThreadPoolLockType::Eresource);

    MyDriverLogTrace("Running tests with a PushLock!");
    TestThreadPoolCounter(TestThreadPoolLockType::PushLock);
    TestThreadPoolCancel(TestThreadPoolLockType::PushLock);

    return STATUS_SUCCESS;
}

void
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    MyDriverLogTrace("We are now in driver unload routine!");

    CppCallStaticDestructors();

    WPP_CLEANUP(DriverObject);
}

EXTERN_C_END