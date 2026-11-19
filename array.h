#ifndef ARRAY_H
#define ARRAY_H

//
// A generic array.
//
#include "framework.h"

class cTile;
class cHit;

enum {
    ARRAY_GENERIC = 0,
    ARRAY_INT,
    ARRAY_TILE,
    ARRAY_CHIT,
    ARRAY_PTR,   // just a generic ptr

    ARRAY_LABEL
};

class cArray
{
public:
   

    cArray(int type = ARRAY_GENERIC);
    cArray(const cArray &src);

    virtual ~cArray();

    void *At(int ix) {
        return &(m_charArray[ix*m_sz_per_item]);
    };


    cHit* AtHit(int ix);

    const void* AtConst(int ix) const {
        return &(m_charArray[ix * m_sz_per_item]);
    };

    // set item at ix with src...
    void SetItem(int ix, const void* src);

    cArray& operator = (const cArray& src);

    // allocate space, returns true if success, false if wrong.
    bool AllocateSpace(int items);

    void PushItem(const void *src);
    void RemoveItem(int ix);
    void push_back(const void* ptr) {
        PushItem(ptr);
    };

    void Shrink();  // just reduces allocated to used...
    void Clear(); // clear the data.
    void clear() {   // to help be consistent with standard vector.
        Clear();
    };
    void SoftClear();

    // sort.. will force sort... if NULL will use DefaultCmp...
    void Sort(int (*cmp_func)(const void *a, const void *b, size_t len) = NULL);

    // will sort if indicates that array is not sorted, will use defaultcmp if cmp_func is NULL...
    void SoftSort(int (*cmp_func)(const void *a, const void *b, size_t len)=NULL);

    int Size() { return m_used; };
    int size() { return Size(); };  // to help be consistent with standard vector.

    int IndexOf(const void* ptr) const;

    union {
        char *m_charArray;       // the array data.

        // below are convient type pointers...
        int *m_intArray;
        cTile *m_tileArray;
        cHit *m_hitArray;

        void* m_ptrArray;
    };

    int m_used;         // how many items used
    int m_allocated;    // how many items are reserved.

    int m_bufferItems;   //  how many items (not bytes) should be allocated

    int m_sorted;

    int m_type;

    size_t m_sz_per_item;   // size per item

    static void DefaultCopy(void *dest, const void *src, size_t len);
    static void DefaultClear(void *dest);   // clear/free the contents, but not the pointer (*dest).
    static int DefaultCmp(const void *a, const void *b, size_t len);
    static void DefaultInit(void *dest, size_t len);

protected:
    void (*m_copy_func)(void *dest, const void *src, size_t len);
    void (*m_clear_func)(void *src); // clear callback when clear items.
    int (*m_cmp_func)(const void *a, const void *b, size_t len);
    void (*m_init_func)(void *dest, size_t len);
};

#endif // ARRAY_H
