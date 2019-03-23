#ifndef __TEST_HPP__ 
#define __TEST_HPP__

#include "ThreadPool.hpp"

enum class TestThreadPoolLockType
{
    PushLock = 0,
    Spinlock = 1,
    Eresource = 2 
};


class TestThreadContext : public CppNonPagedObject<DRV_TAG_TTT>
{
public:
    TestThreadContext()
    {
        KeInitializeEvent(&Event, NotificationEvent, false);
    }

    ~TestThreadContext() = default;

    volatile long long* Counter = nullptr;
    bool WasCleanupCalled = false;
    KEVENT Event{};
};

void 
TestIncrementCounterCallbackNoWait(
    _In_opt_ PVOID Context
);

void
TestIncrementCounterCallbackWait(
    _In_opt_ PVOID Context
);

void
TestCleanupCallback(
    _In_opt_ PVOID Context
);

void
TestThreadPoolCounter(
    _In_ TestThreadPoolLockType LockType
);

void
TestThreadPoolCancel(
    _In_ TestThreadPoolLockType LockType
);

#endif //__TEST_HPP__