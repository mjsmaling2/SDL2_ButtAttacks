#ifndef PTRARRAY_H
#define PTRARRAY_H

class cSprite;

/*
 * Just a simple pointer array, does not allocate/destruct the pointer objects.
 */
class cPtrArray
{
public:
    cPtrArray();
    virtual ~cPtrArray();

    void Add(void *item);
    void Remove(void *item);
    void RemoveAt(int ix);
    void *At(int ix);

    cSprite *SpriteAt(int ix) {
        return (cSprite *)At(ix);
    }

    int IndexOf(void *ptr);

    void Clear();

    bool SoftAllocate(int num);

    void **m_array;//ptr array
    int m_used;
private:
    int m_allocated;
};

#endif // PTRARRAY_H
