#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "framework.h"

struct spritesheet_item {
    SDL_Rect src_rect;  // src
    SDL_Point focus_pt;
    int type;   // type...
    int nth;    // nth number of type..
    const char* name;   // debugging, name of sprite type...
};

// this will map the 'types' to the spritesheet_item array for a 
// faster reference.
struct spritesheet_map {
    int type;
    int start_i;    // start i index
    int sz;         // size...
};

// this will define some offsets between 'sprites', ie: for
// plane -> propeller, plane -> pilot
struct spritesheet_offset {
    int type_a; // origin point
    int type_b; // dest point
    int nth;
    SDL_Point offset;   // this will be relative from type_a (so: type_b - type_a = offset)
};

enum {
    SPRITESHEET_PLANE = 0,
    SPRITESHEET_PILOT,
    SPRITESHEET_PROPELLER,
    SPRITESHEET_ENEMYSHIP,
    SPRITESHEET_EXPLOSION,
    SPRITESHEET_ENEMYCITY,
    SPRITESHEET_BULLET,


    // icons...
    SPRITESHEET_EXCLAMATION,

    SPRITESHEET_MAX
};



class cSpritesheet {
public:
    cSpritesheet();
    virtual ~cSpritesheet();

    // returns if spritesheet is okay, namely if the sprite SDL_Image
    // is loaded...
    bool Init();
    void Quit();

    SDL_Texture *SoftTexture();

    // image...
    SDL_Texture *m_spritesheet_texture;

    // static...
    static struct spritesheet_item g_spritesheet_items[];
    static int g_spritehheet_items_max;

    struct spritesheet_map g_spritesheet_map[SPRITESHEET_MAX];
    int g_spritesheet_map_sz;

    static struct spritesheet_offset g_spritesheet_offset[];
    int g_spritesheet_offset_sz;

    // will update rect.x,y with spritesheettype, nth
    void GetSpriteSheetSz(SDL_Rect *rect, int spritesheettype, int nth = 0);
    void GetSpriteSheetSz(SDL_FRect* rect, int spritesheettype, int nth = 0);

    struct spritesheet_item *GetMappedSpriteSheetItem(int sprite_type, int nth = 0);

    // get src rect...
    SDL_Rect* GetSpriteSheetSrcRect(int sprite_type, int nth = 0);

    // will assume main renderer...
    // dest - where to render onto the rendere, sprite_type,nth the sprite src
    void RenderSprite(SDL_Rect* dest, int sprite_type, int nth, bool reverse = false);

    // will render based on the camera coordinates (not screen).
    void RenderSpriteCamera(SDL_Rect* dest, int sprite_type, int nth, bool reverse = false);
    void RenderSpriteCamera(SDL_FRect* dest, int sprite_type, int nth, bool reverse = false);

    int DetermineSpriteOffsetIX(int origSpriteType, int offsetSpriteType, int index = 0);
    
    struct spritesheet_offset *GetSpriteSheetOffset(int origSpriteType, int offsetSpriteType, int nth = 0);

    static void DebugLog(const struct spritesheet_item* src);

    static void DebugLog(const struct spritesheet_offset* src);

    static const char* NameType(int type);

private:
    static int m_initialized;
};




#endif // SPRITESHEET_H
