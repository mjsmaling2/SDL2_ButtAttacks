#include "Sprite.h"
#include "spritesheet.h"
#include "App.h"
#include "common.h"

cSprite::cSprite(const cSprite &src) {
    Init();
    *this = src;
}

cSprite::cSprite() {
    Init();
}
cSprite::~cSprite() {

}

void cSprite::Init() {
    m_spriteSheetType = -1;
    m_nth = 0;
    SDL_memset(&m_position_rect, 0, sizeof(SDL_Point));

    m_reverse = false;

    m_priority = 100;
    m_is_shown = false;
    m_offsetTypeIX = -1;

    updateSize();
}

void cSprite::updateSize() {
    if (m_spriteSheetType < 0 || m_spriteSheetType >= SPRITESHEET_MAX) {
        m_position_rect.w = 0;
        m_position_rect.h = 0;
    } else {
        SDL_Rect* srcrect;
        srcrect = g_app->m_spritesheet.GetSpriteSheetSrcRect(m_spriteSheetType, m_nth);
        if (srcrect) {
            m_position_rect.w = srcrect->w;
            m_position_rect.h = srcrect->h;
        } else {
            m_position_rect.w = 0;
            m_position_rect.h = 0;
        }
    }
}

cSprite & cSprite::operator = (const cSprite &src) {
    m_spriteSheetType = src.m_spriteSheetType;
    m_nth = src.m_nth;
    m_reverse = src.m_reverse;
    m_priority = src.m_priority;
    SDL_memcpy(&m_position_rect, &src.m_position_rect, sizeof(SDL_Rect));
    m_offsetTypeIX = src.m_offsetTypeIX;
    m_is_shown = src.m_is_shown;
    
    updateSize();

    return *this;
}

bool cSprite::GetRect(SDL_Rect *dest) {
    if (m_spriteSheetType < 0 || m_spriteSheetType >= SPRITESHEET_MAX) {
        if (dest)
            SDL_memset(dest, 0, sizeof(SDL_Rect));
        return false;
    }

    // need to get position and adjust by point, and also by a 'reverse' flag...
    // this should be updated implicitly by other methods...
    SDL_memcpy(dest, &m_position_rect, sizeof(SDL_Rect));
#if 0
    dest->x = m_position_rect.x;
    dest->y = m_position_rect.y;

    dest->w = m
    g_app->m_spritesheet.GetSpriteSheetSz(dest, m_spriteSheetType, m_nth);
#endif
    return true;
}

int cSprite::Width() {
    return m_position_rect.w;
#if 0
    SDL_Rect* srcrect;
    srcrect = g_app->m_spritesheet.GetSpriteSheetSrcRect(m_spriteSheetType, m_nth);
    if (srcrect)
        return srcrect->w;
    return 0;
#endif
}

void cSprite::SetNth(int nth) {
    m_nth = nth;
    updateSize();
}

void cSprite::Render() {
    SDL_Rect out;
    GetRect(&out);

    out.x -= g_app->m_camera_x;
    g_app->m_spritesheet.RenderSprite(&out, m_spriteSheetType, m_nth, m_reverse);

    // want to draw an outline to just show the hit detection stuff...
    SDL_SetRenderDrawColor(g_app->m_renderer, 255, 255, 0, 128);
    SDL_RenderDrawRect(g_app->m_renderer, &out);
}

// will add/remove from g_app visible sprites...
void cSprite::Show() {
    if (!m_is_shown) {
        m_is_shown = true;
    }
}
void cSprite::Hide() {
    if (m_is_shown) {
        m_is_shown = false;
    }
}

void cSprite::SetPriority(int level) {

}

void cSprite::SetType(int spriteType) {
    m_spriteSheetType=spriteType;

    updateSize();
    // adjust width/height...

   // g_app->m_spritesheet.GetSpriteSheetSz(&m_position_rect, m_spriteSheetType, m_nth);
}

bool cSprite::IsVisible(SDL_Rect* coords) {
    SDL_assert(coords != 0);
    SDL_Rect a, b;
    GetRect(&a);
    if (SDL_IntersectRect(coords, &a, &b) == SDL_TRUE)
        return true;
    return false;
}

bool cSprite::IsScreenVisible() {
    SDL_Rect a;
    g_app->GetCameraRect(&a);
    return IsVisible(&a);
}

void cSprite::SetSpriteOffsetType(int src_offsetsprite /*= -1*/) {
    if (src_offsetsprite < 0) {
        m_offsetTypeIX = -1;
    } else {
        m_offsetTypeIX = g_app->m_spritesheet.DetermineSpriteOffsetIX(src_offsetsprite, m_spriteSheetType, m_nth);

        
        if (m_offsetTypeIX >= 0) {
            cSpritesheet::DebugLog( &cSpritesheet::g_spritesheet_offset[m_offsetTypeIX] );
        }

    }
}

void cSprite::SetPosition(int x, int y) {
   /* if (m_offsetTypeIX >= 0) {
        // for debugging...

        cSpritesheet::DebugLog(&cSpritesheet::g_spritesheet_offset[m_offsetTypeIX]);

        int adj_x, adj_y;

        adj_x = OffX(); //g_app->m_spritesheet.g_spritesheet_offset[m_offsetTypeIX].offset.x;
        adj_y = OffY(); //g_app->m_spritesheet.g_spritesheet_offset[m_offsetTypeIX].offset.y;

        SDL_Log("cSprite::SetPosition %d|%d adj_x: %d, adj_y: %d; X: %d, Y:%d;",
                m_spriteSheetType, m_offsetTypeIX,
                adj_x, adj_y,
                x,y
         );

        // adjust...

        if (m_reverse) {
            adj_x -=
            x -= adj_x;
          //  y -= adj_y;
        } else {
            x += adj_x; //g_app->m_spritesheet.g_spritesheet_offset[m_offsetTypeIX].offset.x;//-
                     //g_app->m_spritesheet.g
        }
         //
        y += adj_y; //g_app->m_spritesheet.g_spritesheet_offset[m_offsetTypeIX].offset.y;

        //}
    }*/
    // adjust for sprite width/height...
//    if (m_reverse) {
    //    x -= Width();
        //y -= m_position
 //   }
    m_position_rect.x = x;
    m_position_rect.y = y;
//    SDL_Log("%d %d,%d", m_spriteSheetType, m_position_rect.x, m_position_rect.y);
}

// this is the Central Point - X/Y
int cSprite::OffX() {
    //struct spritesheet_item *GetMappedSpriteSheetItem(int sprite_type, int nth = 0);
    struct spritesheet_item* p = g_app->m_spritesheet.GetMappedSpriteSheetItem(m_spriteSheetType, m_nth);
    int retval = 0;
    if (m_offsetTypeIX >= 0)
        retval = cSpritesheet::g_spritesheet_offset[m_offsetTypeIX].offset.x;
    retval -= p->focus_pt.x;
    return retval;
    //struct spritesheet_item *p = g_app->m_spritesheet.GetMappedSpriteSheetItem(m_spriteSheetType, m_nth);
    //return p->focus_pt.x;// - p->src_rect.x;
}

int cSprite::OffY() {
    struct spritesheet_item* p = g_app->m_spritesheet.GetMappedSpriteSheetItem(m_spriteSheetType, m_nth);
    int retval = 0;
    if (m_offsetTypeIX >= 0)
        retval = cSpritesheet::g_spritesheet_offset[m_offsetTypeIX].offset.y;
    retval -= p->focus_pt.y;
    return retval;
   // struct spritesheet_item *p = g_app->m_spritesheet.GetMappedSpriteSheetItem(m_spriteSheetType, m_nth);
    //return p->focus_pt.y;// - p->src_rect.y;
}

void cSprite::SetPosition(const SDL_Point* src) {
    SDL_assert(src != 0);
    SetPosition(src->x, src->y);
}

int cSprite::HitDetectTiles() {
    int retval = 0;
    if (g_app->m_tileMap.HitDetect(&m_position_rect)) {
        int row = -1;
        for (int x = 0 ;x < g_app->m_tileMap.Size(); x++) {
            cTile *t;
            t = g_app->m_tileMap.GetHitTile(x);
            if (t && (t->m_row < row || row < 0))
                row = t->m_row;
        }
        if (retval == 0)
            retval++;   // make sure at least 1...
        else
            retval = row * g_tile_size;
    }
    return retval;
}

int cSprite::HitDetect(SDL_Rect *test) {
    if (SDL_IntersectRect(test, &m_position_rect, &m_intersectRect))
        return 1;
    return 0;
}
