#ifndef __MODULE_HPP__ 
#define __MODULE_HPP__ 

#include "cpp_includes.hpp"
#include "DriverTags.hpp"
#include "LinkedList.hpp"

class Module : public LinkedListEntry,
               public CppPagedObject<DRV_TAG_IST>
{
public:
    Module(
        _In_ PUNICODE_STRING ImageName,
        _In_ PVOID ImageBase,
        _In_ SIZE_T ImageSize
    );
    virtual ~Module();

    bool IsValid() const;
    bool IsAddressInModule(_In_ PVOID Address) const;


private:
    bool isValid = false;
    UNICODE_STRING imageName = { 0,0,nullptr };
    PVOID imageBase = nullptr;
    SIZE_T imageSize = 0;
};

#endif //__MODULE_HPP__