#include "test.hpp"

void 
TestIncrementCounterCallbackNoWait(
    _In_opt_ PVOID Context
)
{
    auto ctx = reinterpret_cast<TestThreadContext*>(Context);
    InterlockedIncrement64(ctx->Counter);
    KeSetEvent(&ctx->Event, 0, false);
}

void 
TestIncrementCounterCallbackWait(
    _In_opt_ PVOID Context
)
{
    LARGE_INTEGER wait = { 0 };
    wait.QuadPart = -90000000;  // 9 seconds
    
    KeDelayExecutionThread(KernelMode, false, &wait);

    auto ctx = reinterpret_cast<TestThreadContext*>(Context);
    InterlockedIncrement64(ctx->Counter);
    KeSetEvent(&ctx->Event, 0, false);
}

void 
TestCleanupCallback(
    _In_opt_ PVOID Context
)
{
    auto ctx = reinterpret_cast<TestThreadContext*>(Context);
    ctx->WasCleanupCalled = true;
}

Lock* TestThreadPoolGetLock(
    _In_ TestThreadPoolLockType LockType
)
{
    switch (LockType)
    {
    case TestThreadPoolLockType::PushLock:
        return new Pushlock();
    case TestThreadPoolLockType::Eresource:
        return new Eresource();
    case TestThreadPoolLockType::Spinlock:
        return new Spinlock();
    default:
        return nullptr;
    }
}

void 
TestThreadPoolCounter(
    _In_ TestThreadPoolLockType LockType
)
{
    const int noContexts = 10000;
    volatile long long counter = 0;

    Lock* lock = TestThreadPoolGetLock(LockType);
    NT_ASSERT(lock != nullptr);

    TestThreadContext* contexts = new TestThreadContext[noContexts];
    NT_ASSERT(contexts != nullptr);

    for (int i = 0; i < noContexts; ++i)
    {
        contexts[i].Counter = &counter;
    }

    {
        ThreadPool tp{ 10, lock };
        
        for (int i = 0; i < noContexts; ++i)
        {
            tp.EnqueueItem(TestIncrementCounterCallbackNoWait, TestCleanupCallback, &contexts[i]);
        }

        for (int i = 0; i < noContexts; ++i)
        {
            KeWaitForSingleObject(&contexts[i].Event, Executive, KernelMode, false, nullptr);
        }
    }

    NT_ASSERT(counter == noContexts);
    delete[] contexts;
    delete lock;
}

void 
TestThreadPoolCancel(
    _In_ TestThreadPoolLockType LockType
)
{
    const int noContexts = 10000;
    volatile long long counter = 0;

    Lock* lock = TestThreadPoolGetLock(LockType);
    NT_ASSERT(lock != nullptr);

    TestThreadContext* contexts = new TestThreadContext[noContexts];
    NT_ASSERT(contexts != nullptr);

    for (int i = 0; i < noContexts; ++i)
    {
        contexts[i].Counter = &counter;
    }

    {
        ThreadPool tp{ 10, lock };

        for (int i = 0; i < noContexts; ++i)
        {
            tp.EnqueueItem(TestIncrementCounterCallbackWait, TestCleanupCallback, &contexts[i]);
        }

        LARGE_INTEGER wait = { 0 };
        wait.QuadPart = -10000000;  // 1 second
        KeDelayExecutionThread(KernelMode, false, &wait);
    }


    for (int i = 10; i < noContexts; ++i)
    {
        NT_ASSERT(contexts[i].WasCleanupCalled == true);
    }

    // NoThreads
    NT_ASSERT(counter == 10);
    delete[] contexts;
    delete lock;
}
