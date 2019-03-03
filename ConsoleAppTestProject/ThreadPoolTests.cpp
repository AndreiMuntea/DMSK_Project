#include "stdafx.h"
#include "CppUnitTest.h"
#include "LeaksCheck.h"

#include "..\Lab1WPP\ThreadPool.hpp"

#include <vector>
#include <memory>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ConsoleAppTestProject
{
    class MockThreadPoolSquareContext : public ThreadContext
    {
    public:
        MockThreadPoolSquareContext(
            _In_ std::shared_ptr<std::vector<int>> Elements,
            _In_ size_t Start,
            _In_ size_t End
        ) : Elements{ Elements },
            Start{ Start },
            End{ End }
        {};

        virtual ~MockThreadPoolSquareContext() = default;

        std::shared_ptr<std::vector<int>> Elements;
        size_t Start;
        size_t End;
    };

    class MockThreadPoolCounterContext : public ThreadContext
    {
    public:
        MockThreadPoolCounterContext() = default;
        virtual ~MockThreadPoolCounterContext() = default;

        volatile long long Counter = 0;
    };

    TEST_CLASS(ThreadPoolTests)
    {
    public:

        TEST_METHOD(ThreadPoolEnqueueAfterShutdown)
        {
            CrtCheckMemory __memoryState;
            ThreadPool tp{ 8 };
            tp.Shutdown();

            Assert::ExpectException<std::exception>([&]() {tp.EnqueueItem(nullptr, nullptr); });
        }

        TEST_METHOD(ThreadPoolInitializeWith0Threads)
        {
            CrtCheckMemory __memoryState;
            ThreadPool tp{ 0 };
            Assert::IsTrue(tp.GetNoThreads() == std::thread::hardware_concurrency());
        }

        TEST_METHOD(ThreadPoolInitializeWith200Threads)
        {
            CrtCheckMemory __memoryState;
            ThreadPool tp{ 200 };
            Assert::IsTrue(tp.GetNoThreads() == std::thread::hardware_concurrency());
        }

        TEST_METHOD(ThreadPoolNoThreadsAfterShutdown)
        {
            CrtCheckMemory __memoryState;
            ThreadPool tp{ 5 };
            tp.Shutdown(); 

            Assert::IsTrue(tp.GetNoThreads() == min(5,std::thread::hardware_concurrency()));
        }

        TEST_METHOD(ThreadPoolTestSquare100Elements)
        {
            CrtCheckMemory __memoryState;
            RunTestSquare(-51220, 9952, 100, 3, 2);
        }

        TEST_METHOD(ThreadPoolTestSquare100000Elements)
        {
            CrtCheckMemory __memoryState;
            RunTestSquare(-2051, 99512, 100000, 3, 2);
        }

        TEST_METHOD(ThreadPoolTestSquare9999999Elements)
        {
            CrtCheckMemory __memoryState;
            RunTestSquare(-420, 9912, 9999999, 98270, 8);
        }

        TEST_METHOD(ThreadPoolTestSquare100ElementsDelayedEnqueue)
        {
            CrtCheckMemory __memoryState;
            RunTestSquare(-51220, 9952, 100, 7, 2, 300);
        }

        TEST_METHOD(ThreadPoolTestFinalizeWorkAfterShutdown10Counters2Threads)
        {
            CrtCheckMemory __memoryState; 
            RunTestFinalizeWorkAfterShutdown(2, 10);
        }

        TEST_METHOD(ThreadPoolTestFinalizeWorkAfterShutdown10Counters8Threads)
        {
            CrtCheckMemory __memoryState;
            RunTestFinalizeWorkAfterShutdown(8, 10);
        }

        TEST_METHOD(ThreadPoolTestFinalizeWorkAfterShutdown100Counters8Threads)
        {
            CrtCheckMemory __memoryState;
            RunTestFinalizeWorkAfterShutdown(8, 100);
        }

        static void
        IncrementCounter(
            _In_ std::shared_ptr<ThreadContext> Context
        )
        {
            auto context = std::dynamic_pointer_cast<MockThreadPoolCounterContext>(Context);
            if (!context)
            {
                throw std::exception("Invalid pointer cast");
            }

            Sleep(1000);
            InterlockedIncrement64(&context->Counter);
        }

        void RunTestFinalizeWorkAfterShutdown(
            _In_ uint8_t NoThreads,
            _In_ uint8_t ExpectedCounter
        )
        {
            ThreadPool tp{ NoThreads };
            auto context = std::make_shared<MockThreadPoolCounterContext>();

            for (uint8_t i = 0; i < ExpectedCounter; ++i)
            {
                tp.EnqueueItem(context, IncrementCounter);
            }

            tp.Shutdown();

            Assert::IsTrue(ExpectedCounter == context->Counter);
        }
        
        void RunTestSquare(
            _In_ int Low,
            _In_ int High,
            _In_ size_t Count,
            _In_ size_t PartitionSize,
            _In_ uint8_t NoThreads,
            _In_ DWORD Delay = 0
        )
        {
            auto initialVector = GetRandomElements(Low, High, Count);
            auto resultVector = std::make_shared<std::vector<int>>(initialVector);
            auto contexts = PartitionVector(resultVector, PartitionSize);
            ThreadPool tp{ NoThreads };

            for (auto& context : contexts)
            {
                Sleep(Delay);
                tp.EnqueueItem(context, SquareElements);
            }

            tp.Shutdown();

            Assert::IsTrue(initialVector.size() == resultVector->size());
            for (size_t i = 0; i < initialVector.size(); ++i)
            {
                Assert::IsTrue(initialVector[i] * initialVector[i] == resultVector->at(i));
            }
        }

        static void
        SquareElements(
            _In_ std::shared_ptr<ThreadContext> Context
        )
        {
            auto context = std::dynamic_pointer_cast<MockThreadPoolSquareContext>(Context);
            if (!context)
            {
                throw std::exception("Invalid pointer cast");
            }

            for (size_t i = context->Start; i < context->End; ++i)
            {
                auto& element = context->Elements->at(i);
                element *= element;
            }
        }

        std::vector<int>
        GetRandomElements(
            _In_ int Low,
            _In_ int High,
            _In_ size_t Count
        )
        {
            static std::random_device randomDevice;
            static std::mt19937 engine(randomDevice());

            std::uniform_int_distribution<> distr(Low, High);
            std::vector<int> v(Count, 0);

            for (size_t i = 0; i < Count; ++i)
            {
                v[i] = distr(engine);
            }

            return v;
        }

        std::vector<std::shared_ptr<MockThreadPoolSquareContext>>
        PartitionVector(
            _In_ std::shared_ptr<std::vector<int>> Elements,
            _In_ size_t Limit
        )
        {
            std::vector<std::shared_ptr<MockThreadPoolSquareContext>> contexts;
            size_t i = 0;

            while (i + Limit <= Elements->size())
            {
                contexts.emplace_back(std::make_shared<MockThreadPoolSquareContext>(Elements, i, i + Limit));
                i += Limit;
            }

            if (i != Elements->size())
            {
                contexts.emplace_back(std::make_shared<MockThreadPoolSquareContext>(Elements, i, Elements->size()));
            }

            return contexts;
        }
    };
}