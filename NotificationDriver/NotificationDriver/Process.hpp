#ifndef __PROCESS_HPP__ 
#define __PROCESS_HPP__

#include "cpp_includes.hpp"
#include "DriverTags.hpp"
#include "LinkedList.hpp"
#include "Lock.hpp"

class Process : public LinkedListEntry
{
public:
    Process(_In_ HANDLE Pid) : pid{ Pid } {}
    virtual ~Process() = default;

    inline HANDLE GetPid() const { return pid; }
    
    inline bool WasMainThreadCreated() const { return wasMainThreadCreated; }
    inline void SetMainThreadCreated() { wasMainThreadCreated = true; }

private:
    bool wasMainThreadCreated = false;
    HANDLE pid = 0;
};

#endif //__PROCESS_HPP__