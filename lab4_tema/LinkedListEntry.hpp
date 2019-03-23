#ifndef __LINKED_LIST_ENTRY_HPP__ 
#define __LINKED_LIST_ENTRY_HPP__

#include "cpp_includes.hpp"
#include "tags.hpp"

class LinkedListEntry : public CppNonPagedObject<DRV_TAG_LLT>
{
public:
    LinkedListEntry()
    {
        flink = this;
        blink = this;
    }

    virtual ~LinkedListEntry() = default;

    LinkedListEntry(const LinkedListEntry& Other) = delete;
    LinkedListEntry(LinkedListEntry&& Other) = delete;


    LinkedListEntry* flink = nullptr;
    LinkedListEntry* blink = nullptr;
};

#endif //__LINKED_LIST_ENTRY_HPP__