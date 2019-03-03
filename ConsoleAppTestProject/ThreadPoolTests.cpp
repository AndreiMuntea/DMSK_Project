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
    class MockThreadPoolContext : public ThreadContext
    {
    public:
        MockThreadPoolContext(
            _In_ std::shared_ptr<std::vector<int>> Elements,
            _In_ size_t Start,
            _In_ size_t End
        ) : Elements{ Elements },
            Start{ Start },
            End{ End }
        {};

        virtual ~MockThreadPoolContext() = default;

        std::shared_ptr<std::vector<int>> Elements;
        size_t Start;
        size_t End;
    };

    TEST_CLASS(ThreadPoolTests)
    {
    public:

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
            RunTestSquare(-420, 9912, 9999999, 9827, 8);
        }
        
        void RunTestSquare(
            _In_ int Low,
            _In_ int High,
            _In_ size_t Count,
            _In_ size_t PartitionSize,
            _In_ uint8_t NoThreads
        )
        {
            auto initialVector = GetRandomElements(Low, High, Count);
            auto resultVector = std::make_shared<std::vector<int>>(initialVector);
            auto contexts = PartitionVector(resultVector, PartitionSize);
            ThreadPool tp{ NoThreads };

            for (auto& context : contexts)
            {
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
            auto context = std::dynamic_pointer_cast<MockThreadPoolContext>(Context);
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

        std::vector<std::shared_ptr<MockThreadPoolContext>>
        PartitionVector(
            _In_ std::shared_ptr<std::vector<int>> Elements,
            _In_ size_t Limit
        )
        {
            std::vector<std::shared_ptr<MockThreadPoolContext>> contexts;
            size_t i = 0;

            while (i + Limit <= Elements->size())
            {
                contexts.emplace_back(std::make_shared<MockThreadPoolContext>(Elements, i, i + Limit));
                i += Limit;
            }

            if (i != Elements->size())
            {
                contexts.emplace_back(std::make_shared<MockThreadPoolContext>(Elements, i, Elements->size()));
            }

            return contexts;
        }
    };
}