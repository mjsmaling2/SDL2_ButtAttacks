#include "string.h"
#include <string.h>
#include "common.h"

/*  char *m_buffer;
    int m_used;
protected:

    int m_allocated;*/

cString::cString() {
    m_buffer = 0;
    m_used = 0;
    m_allocated = 0;
}
cString::~cString() {
    if (m_buffer)
        x_free(m_buffer);
}

cString& cString::operator = (const char* src) {
    SetData(src, x_strlen(src));
    return *this;
}

bool cString::Append(const char* src) {
    return Append(src, x_strlen(src));
}
bool cString::Append(const char* src, int bytes) {
    if (bytes) {
        if (!SoftAllocate(m_used + bytes + 1))
            return false;
        SDL_memcpy(m_buffer + m_used, src, bytes);
        m_used += bytes;
        m_buffer[m_used] = 0;
    }
    return true;
}

bool cString::AppendInt(int val) {
    bool retval;

    char buffer[50];
    SDL_snprintf(buffer, 50, "%d");

    return Append(buffer);
}

void cString::SetData(const char* src, int bytes) {
    if (SoftAllocate(bytes) && bytes > 0) {
        SDL_memcpy(m_buffer, src, bytes);
        m_used = bytes;
        m_buffer[m_used] = 0;
    }
}

cString& cString::operator = (const cString& src) {
    SetData(src.m_buffer, src.m_used);
    return *this;
}

bool cString::SoftAllocate(int bytes) {
    if (bytes <= 0) {
        x_free(m_buffer);
        m_buffer = 0;
        m_used = m_allocated = 0;
        return true;
    }
    int newsz = realloc_bytes(bytes+1);
    if (newsz == m_allocated)
        return true;
        
    char* newbuff = (char *) x_realloc(m_buffer, newsz);
    if (!newbuff)
        return false;

    m_buffer = newbuff;
    m_allocated = newsz;

    return true;
}
