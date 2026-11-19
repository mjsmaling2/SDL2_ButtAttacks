#include "ptrarray.h"
#include "common.h"

cPtrArray::cPtrArray()
{
    m_array = 0;
    m_used = 0;
    m_allocated = 0;
}
cPtrArray::~cPtrArray() {
    Clear();
}

void cPtrArray::Add(void *item) {
    if (SoftAllocate(m_used + 1)) {
        m_array[m_used++] = item;
    }
}
void cPtrArray::Remove(void *item) {
    int ix;
    ix = IndexOf(item);
    if (ix >= 0)
        RemoveAt(ix);
}
void cPtrArray::RemoveAt(int ix) {
    if (ix >= 0 && ix < m_used) {
        m_used--;
        for (int x = ix; x < m_used; x++)
            m_array[x] = m_array[x+1];
    }
}

void *cPtrArray::At(int ix) {
    if (ix < 0 || ix >= m_used)
        return 0;
    return m_array[ix];
}

int cPtrArray::IndexOf(void *ptr) {
    for (int x = 0; x < m_used; x++) {
        if (m_array[x] == ptr)
            return x;
    }
    return -1;
}

void cPtrArray::Clear() {
    if (m_array)
        x_free(m_array);
    m_array = 0;
    m_allocated = 0;
    m_used = 0;
}

bool cPtrArray::SoftAllocate(int num) {
    if (num <= 0)
        Clear();
    else {
        int desired = realloc_bytes(num);
        if (desired != m_allocated) {
            void *buff = x_realloc(m_array, desired * sizeof(void *));
            if (!buff) {
                ShouldQuit();
                return false;
            }
        }
    }
    return true;
}
