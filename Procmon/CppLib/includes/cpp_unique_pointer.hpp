#ifndef __CPP_UNIQUE_POINTER_HPP__ 
#define __CPP_UNIQUE_POINTER_HPP__ 

#include "cpp_default_delete.hpp"

namespace Cpp
{

    template<typename T, class D = DefaultDelete<T>>
    class UniquePointer
    {
    public:
        UniquePointer() = default;
        UniquePointer(T* Pointer);

        virtual ~UniquePointer();

        UniquePointer(UniquePointer<T, D>&& Other) = delete;
        UniquePointer<T, D>& operator=(UniquePointer<T, D>&& Other) = delete;

        UniquePointer(const UniquePointer<T, D>&) = delete;
        UniquePointer<T, D>& operator=(const UniquePointer<T, D>&) = delete;

        bool IsValid() const;

        void Update(T* NewPointer);
        void Reset();

        T& operator*();
        T* operator->();

    private:
        T* rawPointer = nullptr;
    };

    template<typename T, class D>
    inline UniquePointer<T, D>::UniquePointer(T* Pointer) : 
        rawPointer{ Pointer }
    {
    }

    template<typename T, class D>
    inline bool UniquePointer<T, D>::IsValid() const
    {
        return rawPointer != nullptr;
    }

    template<typename T, class D>
    inline void UniquePointer<T, D>::Update(T * NewPointer)
    {
        UniquePointer<T, D> temp(this->rawPointer);
        this->rawPointer = NewPointer;
    }

    template<typename T, class D>
    inline void UniquePointer<T, D>::Reset()
    {
        Update(nullptr);
    }

    template<typename T, class D>
    inline T& UniquePointer<T, D>::operator*()
    {
        return *rawPointer;
    }

    template<typename T, class D>
    inline T * UniquePointer<T, D>::operator->()
    {
        return rawPointer;
    }

    template<typename T, class D>
    inline UniquePointer<T, D>::~UniquePointer()
    {
        if (rawPointer)
        {
            D()(rawPointer);
            rawPointer = nullptr;
        }
    }

}
#endif //__CPP_UNIQUE_POINTER_HPP__