#pragma once

#include "framework.h"

// a sprite - represents a picture/sub image...
// rotations, other details of the image...
// a sprite can also have 'children' with it.
class cSprite
{
public:
    cSprite(const cSprite &src);
	cSprite();
    virtual ~cSprite();

    cSprite & operator = (const cSprite &src);

    // returns the rect if this sprite (not children)...
    bool GetRect(SDL_Rect *dest);

    void GetPosition(SDL_Point *dest) {
        dest->x = m_position_rect.x;
        dest->y = m_position_rect.y;
    }

    // if the sprite rect is within the coords...
    bool IsVisible(SDL_Rect* coords);
    // will use the camera rect to test.
    bool IsScreenVisible();

    void SetType(int spriteType);
    void SetNth(int nth);
    // src offsprite...
    void SetSpriteOffsetType(int src_offsetsprite = -1);

    void SetPosition(const SDL_Point* src);
    void SetPosition(int x, int y);
    
    int Width();
    int Height() {return m_position_rect.h;};

    // will add/remove from g_app visible sprites...
    void Show();
    void Hide();

    // determines if the sprite is hitting a tile...
    // if > 0 will be the y1 position of the highest most tile...
    // if <= 0 will be no hit was detected...
    int HitDetectTiles();

    int HitDetect(SDL_Rect *test);

    // this is the Central Point - X/Y
    int OffX();
    int OffY();

    // the renderering order... the higher the more... default is 100
    // this is used with sorting...
    int m_priority;

	// dest 
    int m_spriteSheetType;  //
    int m_nth;              // nth of sprite type.

    bool m_reverse; // left to right normal, right-to-left reverse...

    // the position rect... this is updated with: SetType, SetNth... SetPosition...
    SDL_Rect m_position_rect;
    SDL_Rect m_intersectRect;
//    cSprite* m_pChildren;   // children array...

    void SetPriority(int level);
    void Render();

    int m_offsetTypeIX;
protected:
    // update internal size (m_position_rect)...
    void updateSize();
private:
    void Init();
    bool m_is_shown;
};
