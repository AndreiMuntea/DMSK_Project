#include "StackTrace.hpp"
#include "trace.hpp"
#include "StackTrace.tmh"
#include "includes.hpp"
#include "GlobalData.hpp"

StackTrace::StackTrace()
{
    stackframes = RtlCaptureStackBackTrace(0, 50, backtrace, nullptr);

    for (ntStartIdx = 0; ntStartIdx < stackframes; ++ntStartIdx)
    {
        if (IsAddressInNtModule(backtrace[ntStartIdx]))
        {
            break;
        }
    }

    for (ntEndIdx = ntStartIdx; ntEndIdx < stackframes; ++ntEndIdx)
    {
        if (!IsAddressInNtModule(backtrace[ntEndIdx]))
        {
            --ntEndIdx;
            break;
        }
    }

    if (ntEndIdx == stackframes)
    {
        --ntEndIdx;
    }

    // No nt 
    if (ntEndIdx < ntStartIdx)
    {
        ntEndIdx = 0;
        ntStartIdx = 0;
    }
}

void StackTrace::PrintNtStackTrace()
{
    // format: nt!0x12345678; => 14 wchars per address (28 bytes). maximum 50 addresses ~= 1500 bytes
    
    PWCHAR buffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, 1500, DRV_TAG_UST);
    if (!buffer)
    {
        MyDriverLogError("Not enough resources to display stack!");
        return;
    }

    RtlZeroMemory(buffer, 0);
    UNICODE_STRING ustr = { 1500, 1500, buffer };
    UNICODE_STRING tmp = ustr;

    for (LONG i = ntStartIdx; i < ntEndIdx; ++i)
    {
        RtlUnicodeStringPrintf(&tmp, L"nt!0x%8p;", backtrace[i]);
        tmp.Buffer += 14;
        tmp.Length -= 28;
        tmp.MaximumLength -= 28;
    }

    MyDriverLogTrace("Backtrace: %S", ustr.Buffer);
    ExFreePoolWithTag(buffer, DRV_TAG_UST);
}

bool StackTrace::IsAddressInNtModule(PVOID Address) const
{
    return ((SIZE_T)Address > (SIZE_T)gDrvData.NtBaseAddress &&
            (SIZE_T)Address < (SIZE_T)gDrvData.NtBaseAddress + gDrvData.NtModuleSize);

}

bool StackTrace::AreNtTracesSame(const StackTrace& Other)
{
    // A filter may have been inserted above us, so the indexes could be displaced
    // However number of calls from ntoskrnl should be the same
    if (this->ntEndIdx - this->ntStartIdx + 1 != Other.ntEndIdx - Other.ntStartIdx + 1)
    {
        return false;
    }

    for (LONG i = this->ntStartIdx, j = Other.ntStartIdx; i < this->ntEndIdx; ++i, ++j)
    {
        // ntoskrnl should be mapped at the same address.
        if ((SIZE_T)backtrace[i] != (SIZE_T)Other.backtrace[j])
        {
            return false;
        }
    }

    return true;
}

