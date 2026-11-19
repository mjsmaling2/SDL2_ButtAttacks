#pragma once

#include "framework.h"

class cHit;

// a generic linked list

typedef struct cLinkedList {
    union {
        void* m_ptr;

        cHit *m_pHit;   // convienent classes...
    };

	cLinkedList* pNext;
} cLinkedList;

// will initialize dest if valid, if null will create a new one initialized...
extern cLinkedList *LinkedList_Init(cLinkedList* dest);

// will delete src and linked lists, will call free_callback on items...
extern void LinkedList_Delete(cLinkedList* src, void (*free_callback)(void *ptr) = 0);

// add to beginning
extern cLinkedList* LinkedList_Unshift(cLinkedList* src, void* ptr);

// add to end.
extern cLinkedList *LinkedList_Push(cLinkedList* src, void* ptr);

// run callback on all elements...
extern void LinkedList_Map(cLinkedList* src, void (*callback)(void* ptr));

// run callback, but will remove if callback returns > 0... will return first linked list item or NULL
extern cLinkedList *LinkedList_MapFlash(cLinkedList* src, int (*callback)(void* ptr), void (*free_callback)(void *ptr) = NULL);

// remove an (just one) item from the list, call free_callback if found.
extern cLinkedList* LinkedList_Remove(cLinkedList* src, void* itrm, void (*free_callback)(void* ptr) = 0);

// remove after on and after nth item
extern cLinkedList* LinkedList_RemoveAfter(cLinkedList* src, int nth, void (*free_callback)(void* ptr) = 0);

extern int LinkedList_Size(cLinkedList* src);


// just a c++ wrapper around the c version..
class LinkedList {
public:
    LinkedList(void (*free_callback)(void *p) = NULL);
    ~LinkedList();

    cLinkedList *m_list;

    void Clear() {
        LinkedList_Delete(m_list, m_free_callback);
        m_list = 0;
    }

    // want to reset list to something...
    void Init(void (*free_callback)(void *p) = NULL) {
        if (m_list)
            Clear();
        m_free_callback = free_callback;
    }

    void Unshift(void *ptr) {
        m_list = LinkedList_Unshift(m_list, ptr);
    }
    void Push(void *ptr) {
        m_list = LinkedList_Push(m_list, ptr);
    }
    void MapFlash(int (*callback)(void *ptr)) {
        m_list = LinkedList_MapFlash(m_list, callback, m_free_callback);
    }
    void Remove(void *item) {
       m_list=LinkedList_Remove(m_list, item, m_free_callback);
    }
    void RemoveAfter(int nth) {
        m_list=LinkedList_RemoveAfter(m_list, nth, m_free_callback);
    }
    void Map(void (*callback)(void *ptr)) {
        LinkedList_Map(m_list, callback);
    }

    int Size() {
        return LinkedList_Size(m_list);
    }

    void (*m_free_callback)(void*p);
};
