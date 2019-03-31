#ifndef __SMART_POINTER_HPP__ 
#define __SMART_POINTER_HPP__

#include "includes.hpp"

/// No thread safety. Should be used with care ("Smart" pointer :D)

template <class T>
class SmartPointer
{
public:
    SmartPointer(T* RawPointer);
    virtual ~SmartPointer();


    SmartPointer(const SmartPointer& Other);
    SmartPointer& operator=(const SmartPointer& Other);
    
    /// Will implemenet when needed
    SmartPointer(SmartPointer&& Other) = delete;
    void operator=(SmartPointer&& Other) = delete;

private:
    long Reference();
    long Dereference();

    T* RawPointer;
    long ReferenceCounter;
};
template<class T>
inline SmartPointer<T>::SmartPointer(T * RawPointer) :
    RawPointer{ RawPointer },
    ReferenceCounter{ 1 }
{
}

template<class T>
inline SmartPointer<T>::~SmartPointer()
{
    Dereference();
}

template<class T>
inline SmartPointer<T>::SmartPointer(const SmartPointer & Other)
{
    this->ReferenceCounter = Other.Reference();
    this->RawPointer = Other.RawPointer();
}

template<class T>
inline SmartPointer<T>& SmartPointer<T>::operator=(const SmartPointer & Other)
{
    this->Dereference();

    this->ReferenceCounter = Other.Reference();
    this->RawPointer = Other.RawPointer;

    return this;
}

template<class T>
inline long SmartPointer<T>::Reference()
{
    ++ReferenceCounter;
    NT_VERIFY(ReferenceCounter > 0);
    return ReferenceCounter;
}

template<class T>
inline long SmartPointer<T>::Dereference()
{
    --ReferenceCounter
    NT_VERIFY(ReferenceCounter >= 0);

    if (ReferenceCounter == 0)
    {
        delete RawPointer;
        RawPointer = nullptr;
    }

    return ReferenceCounter;
}


#endif //__SMART_POINTER_HPP__
