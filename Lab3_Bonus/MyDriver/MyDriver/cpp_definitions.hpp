#ifndef __CPP_DEFINITIONS_HPP__ 
#define __CPP_DEFINITIONS_HPP__ 

#include <ntddk.h>

#define MEM_TAG_DEF 'FED#'  // #DEF default allocation tag
#define MEM_TAG_TST 'TST#'  // #TST test allocation tag
#define MEM_TAG_CPP 'PPC#'  // #CPP destructors allocation tag

void* __cdecl
operator new(
    _In_ size_t Size
);

void* __cdecl
operator new[](
    _In_ size_t Size
);

void __cdecl
operator delete(
    _Inout_  PVOID Block
);

void __cdecl
operator delete[](
    _Inout_  PVOID Block
);

extern "C"
{
    extern int _fltused;
}

#endif //__CPP_DEFINITIONS_HPP__