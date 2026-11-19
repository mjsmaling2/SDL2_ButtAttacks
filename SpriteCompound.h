#ifndef SPRITECOMPOUND_H
#define SPRITECOMPOUND_H

#include "Sprite.h"

class cSpriteCompound
{
public:
    cSpriteCompound();
    virtual ~cSpriteCompound();

    void Clear();
    void PushSprite(const cSprite &sprite, int offsetIdx);
    bool SoftAllocate(int amt);

    cSprite *m_pSprites;
    int *m_offsetIdxs;

    int m_used;
protected:
    int m_allocated;
};

#endif // SPRITECOMPOUND_H
