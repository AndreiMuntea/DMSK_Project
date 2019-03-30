#ifndef __LOCK_HPP__ 
#define __LOCK_HPP__

#include "includes.hpp"
#include "cpp_includes.hpp"
#include "DriverTags.hpp"

class Lock : public CppNonPagedObject<DRV_TAG_GLT>
{
public:
    Lock() = default;
    virtual ~Lock() = default;

    virtual void Acquire() = 0;
    virtual void Release() = 0;
};


class Spinlock : public Lock
{
public:
    Spinlock()
    {
        KeInitializeSpinLock(&lock);
    }

    virtual ~Spinlock()
    {
        Acquire();
        Release();
    }

    virtual void Acquire() override
    {
        KeAcquireSpinLock(&lock, &irql);
    }

    virtual void Release() override
    {
        KeReleaseSpinLock(&lock, irql);
    }

private:
    KSPIN_LOCK lock{};
    KIRQL irql{};
};


class Pushlock : public Lock
{
public:
    Pushlock()
    {
        FltInitializePushLock(&lock);
    }

    virtual ~Pushlock()
    {
        Acquire();
        Release();
        FltDeletePushLock(&lock);
    }

    virtual void Acquire() override
    {
        FltAcquirePushLockExclusive(&lock);
    }

    virtual void Release() override
    {
        FltReleasePushLock(&lock);
    }

private:
    EX_PUSH_LOCK lock;
};

class Eresource : public Lock
{
public:
    Eresource()
    {
        ExInitializeResourceLite(&resource);
    }

    virtual ~Eresource()
    {
        Acquire();
        Release();
        ExDeleteResourceLite(&resource);
    }

    virtual void Acquire() override
    {
        ExEnterCriticalRegionAndAcquireResourceExclusive(&resource);
    }

    virtual void Release() override
    {
        ExReleaseResourceAndLeaveCriticalRegion(&resource);
    }

private:
    ERESOURCE resource{};
};

#endif //__LOCK_HPP__