#ifndef __LOCKGUARD_HPP__ 
#define __LOCKGUARD_HPP__

#include "Lock.hpp"

class ExclusiveLockguard : public CppNonPagedObject<DRV_TAG_GLT>
{
public:
    explicit ExclusiveLockguard(Lock* Mutex) : mutex{ Mutex }
    {
        this->mutex->Acquire();
    }

    virtual ~ExclusiveLockguard()
    {
        this->mutex->Release();
    }

private:
    Lock* mutex;
};

class SharedLockguard : public CppNonPagedObject<DRV_TAG_GLT>
{
public:
    explicit SharedLockguard(SharedLock* Mutex) : mutex{ Mutex }
    {
        this->mutex->AcquireShared();
    }

    virtual ~SharedLockguard()
    {
        this->mutex->ReleaseShared();
    }

private:
    SharedLock* mutex;
};

#endif //__LOCKGUARD_HPP__