#ifndef __CPP_OBJECT_HPP__ 
#define __CPP_OBJECT_HPP__

#include "includes.hpp"

template <typename POOL_TYPE PoolType, typename ULONG Tag>
class CppObject
{
public:
    void* __cdecl operator new(_In_ size_t Size)
    {
        return ExAllocatePoolWithTag(PoolType, Size, Tag);
    }

    void* __cdecl operator new[](_In_ size_t Size)
    {
        return ExAllocatePoolWithTag(PoolType, Size, Tag);
    }

    void __cdecl operator delete(_Inout_ PVOID Block)
    {
        ExFreePoolWithTag(Block, Tag);
    }

    void __cdecl operator delete[](_Inout_ PVOID Block)
    {
        ExFreePoolWithTag(Block, Tag);
    }

private:
};

template <typename ULONG Tag>
class CppPagedObject : public CppObject<PagedPool, Tag>
{
};


template <typename ULONG Tag>
class CppNonPagedObject : public CppObject<NonPagedPool, Tag>
{
};

#endif //__CPP_OBJECT_HPP__