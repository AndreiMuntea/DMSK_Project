#include "stdafx.h"
#include "CppUnitTest.h"
#include "LeaksCheck.h"

#include "..\Lab1WPP\ThreadPool.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ConsoleAppTestProject
{		
	TEST_CLASS(ThreadPoolTests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
            CrtCheckMemory __memoryState;

            ThreadPool tp{ 5 };
		}

	};
}