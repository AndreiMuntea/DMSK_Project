#ifndef __STACK_TRACE_HPP__ 
#define __STACK_TRACE_HPP__

#include "includes.hpp"
#include "cpp_includes.hpp"
#include "DriverTags.hpp"

class StackTrace : public CppPagedObject<DRV_TAG_STT>
{
public:
    StackTrace();
    ~StackTrace() = default;

    void PrintNtStackTrace();
    bool IsAddressInNtModule(PVOID Address) const;
    bool AreNtTracesSame(const StackTrace& Other);

private:

    LONG  ntStartIdx = 0;
    LONG  ntEndIdx = 0;
    PVOID backtrace[50] = { 0 };
    USHORT stackframes = 0;
};


#endif //__STACK_TRACE_HPP__