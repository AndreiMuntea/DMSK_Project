#ifndef __LINKED_LIST_HPP__ 
#define __LINKED_LIST_HPP__

#include "cpp_includes.hpp"
#include "DriverTags.hpp"

class LinkedListEntry : public CppPagedObject<DRV_TAG_LLT>
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

template <class Object>
class LinkedList : public LinkedListEntry
{
public:
    LinkedList();
    virtual ~LinkedList();

    void InsertTail(_In_ __drv_aliasesMem LinkedListEntry* Entry);
    void InsertHead(_In_ __drv_aliasesMem LinkedListEntry* Entry);

    LinkedListEntry* RemoveTail();
    LinkedListEntry* RemoveHead();

    void RemoveEntry(_Inout_ LinkedListEntry* Entry);
    void Flush();

    bool IsListEmpty() const;
    size_t GetNoElements() const;

    class Iterator
    {
    public:
        Iterator(LinkedListEntry* Element) : current{ Element } {};
        ~Iterator() { current = nullptr; }

        Iterator& operator++() { current = current->flink; return *this; }
        Object* GetRawPointer() { return (Object*)current; }

        bool operator==(const Iterator& Other) const { return current == Other.current; }
        bool operator!=(const Iterator& Other) const { return current != Other.current; }

    private:
        LinkedListEntry* current;
    };

    Iterator begin() { return Iterator{ this->flink }; }
    Iterator end() { return Iterator{ this }; }

private:
    void CheckListEntry(_In_ LinkedListEntry* Entry) const;
    void FastFailOnCorruption(_In_ LinkedListEntry* Entry) const;

    size_t noElements = 0;
};

template<class Object>
inline LinkedList<Object>::LinkedList()
{
    static_assert(__is_base_of(LinkedListEntry, Object), "Linked list template should be of type Linked list entry");
}

template<class Object>
inline LinkedList<Object>::~LinkedList()
{
    Flush();
}

template<class Object>
inline void LinkedList<Object>::InsertTail(_In_ __drv_aliasesMem LinkedListEntry* Entry)
{
    FastFailOnCorruption(Entry);

    Entry->blink = this->blink;
    Entry->flink = this;

    this->blink->flink = Entry;
    this->blink = Entry;

    ++noElements;
}

template<class Object>
inline void LinkedList<Object>::InsertHead(_In_ __drv_aliasesMem LinkedListEntry* Entry)
{
    FastFailOnCorruption(Entry);

    Entry->flink = this->flink;
    Entry->blink = this;

    this->flink->blink = Entry;
    this->flink = Entry;

    ++noElements;
}

template<class Object>
inline LinkedListEntry* LinkedList<Object>::RemoveTail()
{
    auto tail = this->blink;
    RemoveEntry(tail);
    return tail;
}

template<class Object>
inline LinkedListEntry* LinkedList<Object>::RemoveHead()
{
    auto head = this->flink;
    RemoveEntry(head);
    return head;
}

template<class Object>
inline void LinkedList<Object>::RemoveEntry(_Inout_ LinkedListEntry* Entry)
{
    FastFailOnCorruption(Entry);

    auto next = Entry->flink;
    auto prev = Entry->blink;

    next->blink = prev;
    prev->flink = next;

    --noElements;
}

template<class Object>
inline void LinkedList<Object>::Flush()
{
    while (!IsListEmpty())
    {
        auto head = RemoveHead();
        delete head;
    }
}

template<class Object>
inline bool LinkedList<Object>::IsListEmpty() const
{
    return noElements == 0;
}

template<class Object>
inline size_t LinkedList<Object>::GetNoElements() const
{
    return noElements;
}

template<class Object>
inline void LinkedList<Object>::CheckListEntry(_In_ LinkedListEntry* Entry) const
{
    UNREFERENCED_PARAMETER(Entry);

    NT_ASSERT(Entry->flink->blink == Entry);
    NT_ASSERT(Entry->blink->flink == Entry);
}

template<class Object>
inline void LinkedList<Object>::FastFailOnCorruption(_In_ LinkedListEntry* Entry) const
{
    CheckListEntry(Entry);
    CheckListEntry(this->flink);
    CheckListEntry(this->blink);
}


#endif //__LINKED_LIST_HPP__
