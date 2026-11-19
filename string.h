#ifndef STRING_H
#define STRING_H

#include "framework.h"
// just a simple string/buffer class.

class cString
{
public:
    cString();
    virtual ~cString();

    cString & operator = (const char *src);
    cString& operator = (const cString& src);

    bool Append(const char *src);
    bool Append(const char *src, int bytes);

    bool AppendInt(int val);

    void SetData(const char *src, int bytes);

    bool SoftAllocate(int bytes);

    void Clear() {
        m_used = 0;
        if (m_buffer)
            m_buffer[0] = 0;
    };

    char *m_buffer;
    int m_used;
protected:

    int m_allocated;
};

#endif // STRING_H
