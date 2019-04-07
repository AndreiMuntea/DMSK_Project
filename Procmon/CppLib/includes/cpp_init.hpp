#ifndef __CPP_INIT_HPP__ 
#define __CPP_INIT_HPP__

typedef void(__cdecl *PVFV)(void);

extern "C" {
    PVFV __crtXca[];
    PVFV __crtXcz[];
}

namespace Cpp
{
    void CppInitializeLib();

    void CppUninitializeLib();
}

#endif //__CPP_INIT_HPP__