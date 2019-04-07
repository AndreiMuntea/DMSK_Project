#ifndef __KERNEL_OBJECT_HPP__ 
#define __KERNEL_OBJECT_HPP__

class KernelObject
{
public:
    KernelObject() = default;
    virtual ~KernelObject() = default;

    inline bool IsValid() const { return isValid; }
    inline void Validate() { isValid = true; }

protected:
    bool isValid = false;
};

#endif //__KERNEL_OBJECT_HPP__