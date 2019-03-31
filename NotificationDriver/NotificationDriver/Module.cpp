#include "Module.hpp"

Module::Module(
    _In_ PUNICODE_STRING ImageName,
    _In_ PVOID ImageBase,
    _In_ SIZE_T ImageSize
)
{
    auto buffer = ExAllocatePoolWithTag(PagedPool, ImageName->Length, DRV_TAG_UST);
    if (!buffer)
    {
        return;
    }

    this->imageName.Buffer = (PWCHAR)buffer;
    this->imageName.Length = ImageName->Length;
    this->imageName.MaximumLength = ImageName->Length;
    this->imageBase = ImageBase;
    this->imageSize = ImageSize;
    this->isValid = true;
}

Module::~Module()
{
    if (this->IsValid())
    {
        ExFreePoolWithTag(this->imageName.Buffer, DRV_TAG_UST);
        this->imageName.Buffer = nullptr;
    }
}

bool Module::IsValid() const
{
    return this->isValid;
}

bool Module::IsAddressInModule(
    _In_ PVOID Address
) const
{
    return (unsigned long long)imageBase <= (unsigned long long)Address &&
           (unsigned long long)imageBase + imageSize >= (unsigned long long)Address;
}