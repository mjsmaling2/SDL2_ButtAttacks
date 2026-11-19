#include "array.h"
#include "common.h"
#include "Tile.h"
#include "framework.h"
#include "Hit.h"
#include "Label.h"

// static functions...
void cArray::DefaultCopy(void *dest, const void *src, size_t len) {
    SDL_memcpy(dest, src,len);
}
void cArray::DefaultClear(void *dest) {
    // nothing to do...
}
int cArray::DefaultCmp(const void *a, const void *b, size_t len) {
    return SDL_memcmp(a, b, len);
}
void cArray::DefaultInit(void *dest, size_t len) {
    SDL_memset(dest, 0, len);
}

cArray::cArray(const cArray& src) {
    m_charArray = NULL;
    *this = src;
}


cArray::cArray(int type /*= ARRAY_GENERIC*/)
{
    m_type = type;

    switch(type) {
    case ARRAY_TILE:
        m_copy_func = DefaultCopy;
        m_clear_func = NULL;
        m_cmp_func = Tile_Cmp;
        m_init_func = NULL;
        m_sz_per_item = sizeof(cTile);
        m_bufferItems = 16;
        break;
    case ARRAY_INT:
        m_copy_func = DefaultCopy;
        m_clear_func = NULL;
        m_cmp_func = DefaultCmp;
        m_init_func = NULL;
        m_sz_per_item = sizeof(int);
        m_bufferItems = 16;
        break;
    case ARRAY_CHIT:
        m_copy_func = DefaultCopy;
        m_clear_func = NULL;
        m_cmp_func = DefaultCmp;
        m_init_func = NULL;
        m_sz_per_item = sizeof(cHit);
        m_bufferItems = 256 / m_sz_per_item;
        break;
    case ARRAY_LABEL:
        m_init_func = label_init;
        m_copy_func = label_copy;
        m_clear_func = label_clear;
        m_sz_per_item = sizeof(cLabel);
        m_bufferItems = 256 / m_sz_per_item;

        break;
    case ARRAY_GENERIC:
    default:
        m_copy_func = DefaultCopy;
        m_clear_func = NULL;
        m_cmp_func = NULL;
        m_init_func = NULL;
        m_sz_per_item = 1;
        m_bufferItems = 16;
        break;
    };

    m_charArray = NULL;
    m_used = 0;
    m_allocated = 0;

}


cArray::~cArray() {
    Clear();
}


cArray& cArray::operator = (const cArray& src) {
    if (m_charArray)
        Clear();
    m_used = m_allocated = 0;

    m_copy_func = src.m_copy_func;
    m_clear_func = src.m_clear_func;
    m_cmp_func = src.m_cmp_func;
    m_init_func = src.m_init_func;
    m_sz_per_item = src.m_sz_per_item;
    m_bufferItems = src.m_bufferItems;

    m_type = src.m_type;
    m_sorted = src.m_sorted;

    if (AllocateSpace(src.m_used)) {

        for (int x = 0; x < src.m_used; x++) {
            m_used++;
            SetItem(x, src.AtConst(x));
        }

    }

    return *this;
}


int cArray::IndexOf(const void* ptr) const {
    for (int x = 0; x < m_used; x++) {
        if (AtConst(x) == ptr)
            return x;
    }
    return -1;
#if 0
    const char *p = m_
    
    switch (m_type) {
    case ARRAY_CHIT:
        for (i = 0; i < m_used; i++) {
            if (m_ptrArray[i] == ptr)
                return i;
        }
        break;
    default:
        for (i = 0; i < m_used; i++) {
            if (ptr == AtConts(i))
                return i;
        }
        break;
    }
    return -1;
#endif
}

void cArray::SetItem(int ix, const void* src) {
    if (ix < 0 || ix >= m_used)
        return;
    m_copy_func(At(ix), src, m_sz_per_item);
}

void cArray::PushItem(const void *src) {
    if (AllocateSpace(m_used + 1)) {
        m_copy_func(At(m_used++), src, m_sz_per_item);
        m_sorted = 0;
    }
}

void cArray::RemoveItem(int ix) {
    if (ix >= 0 && ix < m_used) {
        if (m_clear_func)
            m_clear_func(At(ix));
        ix++;
        while (ix < m_used) {
            m_copy_func(At(ix - 1), At(ix), m_sz_per_item);
            ix++;
        }
        m_used--;
    }
}

bool cArray::AllocateSpace(int items) {
    if (items > m_allocated) {
        int allocate = items + m_bufferItems;
        size_t bytes = allocate * m_sz_per_item;
        bool ok;
        m_charArray = (char *)x_realloc(m_charArray, bytes, &ok);
        if (!ok) {
            // something went wrong.
            ShouldQuit();
            return false;
        }
        if (m_init_func) {
            while (m_allocated < allocate)
                m_init_func(At(m_allocated++), m_sz_per_item);
        } else {
            m_allocated = allocate;
        }
    } else if (items == 0) {
        Clear();
    }
    return true;
}

void cArray::Shrink() {
    if (m_allocated > m_used) {
        size_t bytes = m_allocated * m_sz_per_item;
        bool ok;

        m_charArray = (char*)x_realloc(m_charArray, bytes, &ok);

        if (!ok) {
            ShouldQuit();
            return;
        }

        m_allocated = m_used;
    }
} // just reduces allocated to used...

void cArray::Clear() {
    SoftClear();
    
    if (m_charArray) {
        x_free(m_charArray);
        m_charArray = NULL;
    }

    m_allocated = 0;
}

void cArray::SoftClear() {
    if (m_clear_func && m_charArray) {
        for (int x = 0; x < m_used; x++) {
            void* ptr = At(x);
            m_clear_func(ptr);
        }
    }
    m_sorted = 0;
    m_used = 0;
}

void cArray::SoftSort(int (*cmp_func)(const void *a, const void *b, size_t len)) {
    if (m_sorted <= 0) {
        Sort(cmp_func);
        m_sorted++;
    }
}

void cArray::Sort(int (*cmp_func)(const void *a, const void *b, size_t len)) {
    if (cmp_func == NULL) {
        if (m_cmp_func == NULL)
            return;
        cmp_func = m_cmp_func;
    }

    if (m_used <= 0) {
        m_sorted = 1;
        return;
    }

    if (!AllocateSpace(m_used + 1)) {
        return;
    }

    // do the boolean swap...
    int lower, upper, swapped, i, x;
    lower = 0;
    upper = m_used -1;

loop_1:
    swapped = 0;
    for (i = lower; i < upper; i++) {
        int cmp = cmp_func(At(i), At(i+1), m_sz_per_item);//Tile_Cmp(&m_pArray[i], &m_pArray[i + 1]); // cTile::Compare(&m_pArray[i], &m_pArray[i + 1]);
        if (cmp > 0) {
            m_copy_func(At(m_used), At(i), m_sz_per_item);
            m_copy_func(At(i), At(i+1), m_sz_per_item);
            m_copy_func(At(i+1), At(m_used), m_sz_per_item);
            swapped++;
        }
    }

    if (swapped) {
        swapped = 0;
        upper--;

        for (x = upper; x > lower; x--) {
            int cmp = cmp_func(At(i-1), At(i), m_sz_per_item);//(&m_pArray[i - 1], &m_pArray[i]);
            if (cmp > 0) {
                m_copy_func(At(m_used), At(i), m_sz_per_item);
                m_copy_func(At(i), At(i-1), m_sz_per_item);
                m_copy_func(At(i-1), At(m_used), m_sz_per_item);
                swapped++;
            }
        }

        if (swapped) {
            lower++;
            goto loop_1;
        }
    }

    m_sorted = 1;

}

cHit* cArray::AtHit(int ix) {
    if (ix < 0 || ix >= m_used)
        return NULL;
    return &(m_hitArray[ix]);
}
