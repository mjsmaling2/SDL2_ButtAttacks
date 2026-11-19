#include "spritecompound.h"

cSpriteCompound::cSpriteCompound() {
    m_pSprites = 0;
    m_offsetIdxs = 0;
    m_used = m_allocated = 0;
}
cSpriteCompound::~cSpriteCompound() {
    Clear();
}

void cSpriteCompound::Clear() {
    if (m_pSprites) {
        delete [] m_pSprites;
        m_pSprites = 0;
    }
    if (m_offsetIdxs) {
        delete [] m_offsetIdxs;
        m_offsetIdxs = 0;
    }
    m_used = m_allocated = 0;
}

void cSpriteCompound::PushSprite(const cSprite &sprite, int offsetIdx) {
    if (SoftAllocate(m_used + 1)) {
        m_pSprites[m_used] = sprite;
        m_offsetIdxs[m_used] = offsetIdx;
        m_used++;
    }
}
bool cSpriteCompound::SoftAllocate(int amt) {
    if (amt <= 0)
        Clear();
    else if (amt > m_allocated) {
        amt += 2;   // not many sprites are needed for this game.
        cSprite *newbuff = new cSprite[amt];
        if (!newbuff) {
            SDL_Log("Could not allocate %d sprites", amt);
            return false;
        }
        int *newids = new int [amt];
        if (!newids) {
            SDL_Log("Could not allocate %d ints", amt);
            delete [] newbuff;
            return false;
        }

        if (m_pSprites) {
            SDL_assert(m_offsetIdxs!=0);
            for (int x = 0; x < m_used; x++) {
                newbuff[x] = m_pSprites[x];
                newids[x] = m_offsetIdxs[x];
            }
            delete [] m_pSprites;
            delete [] m_offsetIdxs;
        }
        m_pSprites = newbuff;
        m_offsetIdxs = newids;
        m_allocated = amt;
    }
    return true;
}
