#include "spritesheet.h"
#include "common.h"
#include "App.h"
#include <stdio.h>
/*    SPRITESHEET_PLANE = 0,
    SPRITESHEET_PILOT,
    SPRITESHEET_PROPELLER,
    SPRITESHEET_ENEMYSHIP,
    SPRITESHEET_EXPLOSION,
    SPRITESHEET_ENEMYCITY,
    SPRITESHEET_BULLET,*/
const char* g_spritetype_names[] = {
    "Plane",
    "Pilot",
    "Propeller",
    "EnemyShip",
    "Explosion",
    "EnemyCity",
    "Bullet",


    // icons...
    "Exclamation",  // index: 7

    0
};

struct spritesheet_item cSpritesheet::g_spritesheet_items[] = {
    {32, 66, 83, 89, 32, 66, SPRITESHEET_PLANE, 0, g_spritetype_names[0]},

    {113, 52, 135, 73, -1, 73, SPRITESHEET_PILOT, 0, g_spritetype_names[1]},    // -1 will be calculated to centre

    { 154, 68, 179, 90, -1, -1, SPRITESHEET_PROPELLER, 0, g_spritetype_names[2] },
    { 155, 28, 177, 53, -1, -1, SPRITESHEET_PROPELLER, 1, g_spritetype_names[2]},

    {230, 101, 289, 135, 230, 101, SPRITESHEET_ENEMYSHIP, 0, g_spritetype_names[3]},


    {49, 213, 67, 229, -1, -1, SPRITESHEET_EXPLOSION, 0, g_spritetype_names[4]},
    {75, 199, 109, 227, -1, -1,  SPRITESHEET_EXPLOSION, 1, g_spritetype_names[4]},
    {119, 179, 174, 230, -1, -1,SPRITESHEET_EXPLOSION, 2, g_spritetype_names[4]},
    {192, 171, 251, 230, -1, -1, SPRITESHEET_EXPLOSION, 3, g_spritetype_names[4]},
    {278, 172, 332, 232, -1, -1, SPRITESHEET_EXPLOSION, 4, g_spritetype_names[4]},

      {248, 250, 310, 295, -1, -1, SPRITESHEET_ENEMYCITY, 4, g_spritetype_names[5]},

      {91, 75, 97, 80, -1, -1, SPRITESHEET_BULLET, 4, g_spritetype_names[6]},


    {685, 106, 703, 137, -1, -1, SPRITESHEET_EXCLAMATION, 1, g_spritetype_names[7]},

    { 0, 0, 0, 0, 0, 0, SPRITESHEET_MAX, 0, 0} // just to indicate the last one.

};

int cSpritesheet::g_spritehheet_items_max;

struct spritesheet_map g_spritesheet_map[SPRITESHEET_MAX]; // filled by cSpritesheet

struct spritesheet_offset cSpritesheet::g_spritesheet_offset[] = {

    { SPRITESHEET_PLANE, SPRITESHEET_PILOT, 0, (57-32), (75-66) },
    { SPRITESHEET_PLANE, SPRITESHEET_PROPELLER, 0, (78-32), (80-66) },

    {SPRITESHEET_MAX, SPRITESHEET_MAX, 0, -1, -1}   // end
};


int cSpritesheet::m_initialized = 0;

cSpritesheet::cSpritesheet() {
    m_spritesheet_texture = 0;
    //m_initialized = 0;
}

cSpritesheet::~cSpritesheet() {
    Quit();
}

void cSpritesheet::Quit() {
    if (m_spritesheet_texture) {
        SDL_DestroyTexture(m_spritesheet_texture);
        m_spritesheet_texture = 0;
    }

}

SDL_Texture *cSpritesheet::SoftTexture() {
    if (!m_spritesheet_texture) {
        m_spritesheet_texture = IMG_LoadTexture(
            g_app->m_renderer,
            g_app->AssetsFile("sprite-sheet.png")
        );

        if (!m_spritesheet_texture) {
            SDL_Log("Could not load asset image file: %s", SDL_GetError());
        }
    }
    return m_spritesheet_texture;
}

bool cSpritesheet::Init() {
    if (SoftTexture() == 0) {
        return false;
    }
    if (m_initialized++)
        return true;
    // calculate a few things...
    int last_type, nth_counter;
    last_type = -1;
    struct spritesheet_item* p = g_spritesheet_items;
    g_spritehheet_items_max = 0;
    for (p = g_spritesheet_items; p->type != SPRITESHEET_MAX; p++) {

        if (last_type != p->type) {
            last_type = p->type;
            nth_counter = 0;
        } else
            nth_counter++;

        p->nth = nth_counter;
        p->src_rect.w = (p->src_rect.w - p->src_rect.x) + 1;
        p->src_rect.h = (p->src_rect.h - p->src_rect.y) + 1;

        if (p->focus_pt.x < 0)
            p->focus_pt.x = (p->src_rect.w / 2);
        else
            p->focus_pt.x -= p->src_rect.x;
        if (p->focus_pt.y < 0)
            p->focus_pt.y = (p->src_rect.h / 2);
        else
            p->focus_pt.y -= p->src_rect.y;

        cSpritesheet::DebugLog(p);

        g_spritehheet_items_max++;
    }

    if (g_spritesheet_offset_sz <= 0) {
        struct spritesheet_offset* p;
        for (p = g_spritesheet_offset; p->type_a != SPRITESHEET_MAX; p++)
            g_spritesheet_offset_sz++;
    }

//    struct spritesheet_map g_spritesheet_map[SPRITESHEET_MAX];
//    int g_spritesheet_map_sz;

    // set the map indexes... this is slow but it's okay for now...
    struct spritesheet_map *h;
    g_spritesheet_map_sz = 0;
    for (h = g_spritesheet_map; h->type != SPRITESHEET_MAX; h++) {

        h->start_i =-1;
        h->sz = 0;

        int ii = 0;
        for (p = g_spritesheet_items; p->type != SPRITESHEET_MAX; p++) {
            if (p->type == g_spritesheet_map_sz) {
                if (h->start_i < 0) {
                    h->start_i = ii;
                    h->type = p->type;
                }
                h->sz++;
            } else if (h->sz > 0)
                break;
            ii++;
        }

        SDL_Log(
            "g_spritesheet_map [%d] : TYPE: %d; START_I: %d; SZ: %d;",
            g_spritesheet_map_sz, h->type, h->start_i, h->sz
        );

        g_spritesheet_map_sz++;
    }

    // test the offsetness of the propeller and pilot...
    // int DetermineSpriteOffsetIX(int origSpriteType, int offsetSpriteType, int index = 0);

    int x, y;
    /*   int type_a; // origin point
    int type_b; // dest point
    int nth;
    SDL_Point offset;   
    */
    for (x = SPRITESHEET_PLANE; x <= SPRITESHEET_BULLET; x++) {
        for (y = SPRITESHEET_PLANE; y <= SPRITESHEET_BULLET; y++) {
            for (int nth = 0; nth < 4; nth++) {
                struct spritesheet_offset* off = GetSpriteSheetOffset(x, y, nth);
                if (off)
                    DebugLog(off);
            }
        }
    }

    m_initialized++;
    return true;
}

void cSpritesheet::DebugLog(const struct spritesheet_offset* src) {
    if (src) {
        SDL_Log("spritesheet_offset: %s -> %s, %d: %d,%d",
            NameType(src->type_a), NameType(src->type_b),
            src->nth, src->offset.x, src->offset.y
        );
    }
}

struct spritesheet_offset* cSpritesheet::GetSpriteSheetOffset(int origSpriteType, int offsetSpriteType, int nth /*= 0*/) {
    int retval = DetermineSpriteOffsetIX(origSpriteType, offsetSpriteType, nth);
    if (retval < 0)
        return 0;
    return &g_spritesheet_offset[retval];
}

int cSpritesheet::DetermineSpriteOffsetIX(int origSpriteType, int offsetSpriteType, int index/* = 0*/) {
    int retval = -1;
    for (int x = 0; x < g_spritesheet_offset_sz; x++) {
        if (g_spritesheet_offset[x].type_a == origSpriteType &&
                g_spritesheet_offset[x].type_b == offsetSpriteType &&
            g_spritesheet_offset[x].nth == index) {
            retval = x;
            break;
        }
    }
    return retval;
}

void cSpritesheet::GetSpriteSheetSz(SDL_Rect* rect, int spritesheettype, int nth /*= 0*/) {

    SDL_Rect* srcrect;
    srcrect = GetSpriteSheetSrcRect(spritesheettype, nth);

    SDL_assert(srcrect);
    if (srcrect) {
        rect->w = srcrect->w;
        rect->h = srcrect->h;
    }
}

void cSpritesheet::GetSpriteSheetSz(SDL_FRect* rect, int spritesheettype, int nth /*= 0*/) {
    SDL_Rect* prect;
    prect = GetSpriteSheetSrcRect(spritesheettype, nth);
    if (prect) {
        rect->w = (int)prect->w;
        rect->h = (int)prect->h;
    }
}


SDL_Rect* cSpritesheet::GetSpriteSheetSrcRect(int sprite_type, int nth /*= 0*/) {
    struct spritesheet_item *p;
    p = GetMappedSpriteSheetItem(sprite_type, nth);
    return p ? &p->src_rect : 0;
}

//void RenderSprite(SDL_Rect* dest, int sprite_type, int nth, bool reverse = false);
void cSpritesheet::RenderSprite(SDL_Rect* dest, int sprite_type, int nth, bool reverse) {
    SDL_Renderer* renderer = g_app->m_renderer;
    if (renderer == 0 || m_spritesheet_texture == 0)
        return;

    SDL_Rect* src = GetSpriteSheetSrcRect(sprite_type, nth);
    if (src) {
        if (reverse)
            SDL_RenderCopyEx(renderer, m_spritesheet_texture, src, dest, 0.0, 0, SDL_FLIP_HORIZONTAL);
        else
            SDL_RenderCopy(renderer, m_spritesheet_texture, src, dest);
    }
}

void cSpritesheet::RenderSpriteCamera(SDL_Rect* dest, int sprite_type, int nth, bool reverse /*= false*/) {
    SDL_Rect d;
    d.x = dest->x - g_app->m_camera_x;
    d.y = dest->y - g_app->m_camera_y;
    d.w = dest->w;
    d.h = dest->h;
    RenderSprite(&d, sprite_type, nth, reverse);
}
void cSpritesheet::RenderSpriteCamera(SDL_FRect* dest, int sprite_type, int nth, bool reverse/* = false*/) {
    SDL_Rect d;
    d.x = dest->x;
    d.y = dest->y;
    d.h = dest->h;
    d.w = dest->w;
    RenderSpriteCamera(&d, sprite_type, nth, reverse);
}


struct spritesheet_item *cSpritesheet::GetMappedSpriteSheetItem(int sprite_type, int nth /*= 0*/) {
    struct spritesheet_item *retval;
    int ix = g_spritesheet_map[sprite_type].start_i + nth;
    retval = & g_spritesheet_items[ix];
    SDL_assert(ix >= 0 && ix < g_spritehheet_items_max);
    return retval;
}

void cSpritesheet::DebugLog(const struct spritesheet_item* src) { // static
/*   SDL_Rect src_rect;  // src
    SDL_Point focus_pt;
    int type;   // type...
    int nth;    // nth number of type..
    const char* name;   // debugging, name of sprite type...*/

    SDL_Log("spritesheet_item: %s rect: %d,%d x %d,%d; point: %d,%d; nth: %d",
        Debug_SafeChar(src->name),
        src->src_rect.x, src->src_rect.y, src->src_rect.w, src->src_rect.h,
        src->focus_pt.x, src->focus_pt.y,
        src->nth
    );
}

const char* cSpritesheet::NameType(int type) {
    if (type < 0 || type >= SPRITESHEET_MAX)
        return g_null_string;
    return g_spritetype_names[type];
}
