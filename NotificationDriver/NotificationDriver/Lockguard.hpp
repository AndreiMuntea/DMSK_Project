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

#endif //__LOCKGUARD_HPP__