#ifndef __CPP_CRT_DATA_SECTION_HPP__ 
#define __CPP_CRT_DATA_SECTION_HPP__ 


#include <ntddk.h>
typedef void(__cdecl *PVFV)(void);

extern "C" {
    PVFV __crtXca[];
    PVFV __crtXcz[];
}

void
CppInitializeDestructorsList();

void
CppCallStaticDestructors();

void
CppCallStaticConstructors(
    _In_ PVFV* FirstPVFVEntry,
    _In_ PVFV* LastPVFVEntry
);

int
CppRegisterDestructor(
    _In_ PVFV Destructor
);


#endif //__CPP_CRT_DATA_SECTION_HPP__