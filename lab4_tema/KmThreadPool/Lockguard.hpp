#ifndef __LOCKGUARD_HPP__ 
#define __LOCKGUARD_HPP__

#include "Lock.hpp"

class LockGuard : public CppNonPagedObject<DRV_TAG_GLT>
{
public:
    explicit LockGuard(Lock* Mutex) : mutex{ Mutex }
    {
        this->mutex->Acquire();
    }

    virtual ~LockGuard()
    {
        this->mutex->Release();
    }

private:
    Lock* mutex;
};

#endif //__LOCKGUARD_HPP__