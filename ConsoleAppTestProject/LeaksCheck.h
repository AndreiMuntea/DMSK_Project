#ifndef __LEAKS_CHECK_H__ 
#define __LEAKS_CHECK_H__

#include <crtdbg.h>
#include "CppUnitTest.h"

struct CrtCheckMemory
{
    _CrtMemState state1;
    _CrtMemState state2;
    _CrtMemState state3;
    CrtCheckMemory()
    {
        _CrtMemCheckpoint(&state1);
    }
    ~CrtCheckMemory()
    {
        _CrtMemCheckpoint(&state2);
        if (_CrtMemDifference(&state3, &state1, &state2))
        {
            _CrtMemDumpStatistics(&state3);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::Fail();
        }
    }
};

#endif //__LEAKS_CHECK_H__