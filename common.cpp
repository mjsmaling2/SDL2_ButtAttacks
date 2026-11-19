#include "common.h"
#include <string.h>
#include <stdlib.h>
#include "App.h"
#include <stdio.h>

const char* g_null_string = "<NULL>";

int g_canvas_width, g_canvas_height, g_tile_size;

cApp* g_app = 0;

Uint32 g_c64_colours[16] = {
        0x000000,
        0xFFFFFF,
        0x880000,
        0xAAFFEE,
        0xCC44CC,
        0x00CC55,
        0x0000AA,
        0xEEEE77,
        0xDD8855,
        0x664400,
        0xFF7777,
        0x333333,
        0x777777,
        0xAAFF66,
        0x0088FF,
        0xBBBBBB
};

Uint32 g_palette[83] = {
    // greys... tint of blue...
    0x000000,       // greys...
    0x121217,       // slight blue to them...
    0x24242d,       //
    0x34343d,
    0x4c4c56,
    0x676772,
    0x888693,
    0xa7a7b2,
    0xc8c8cc,
    0xffffff, // 10th [9]

    // gres, blue navy/blueish...
    0x1d1d2d,
    0x2f2f42,
    0x434356,
    0x62637d,
    0x7e839e,
    0x9bafc6,
    0xc5dcf0,   // 17th [16]

    // blues...  
    0x000059,
    0x000082,
    0x00339a,
    0x0c5bad,
    0x1782c0,   // 22nd

    // sky blues...
    0x0000ff,
    0x00a1ff,
    0x00d5ff,
    0x8aefff,   // 26th

    // greyish...
    0x101617,
    0x182022,
    0x252d31,
    0x354245,
    0x445358,   // 31st

    // army greenish
    0x204242,   // 32
    0x2a4d3b,   
    0x3a5e47,
    0x506e59,
    0x678566,   // 36th

    // green, more yellowish
    0x0b170a,   //
    0x11240f,
    0x2e4920,
    0x547435,
    0x76a44d,
    0x88b55b,
    0x9ecc6a,   // 43rd [42]

    // brown
    0x474737,
    0x6f6f51,   // 45

    // blue to bright greeb
    0x004466,
    0x007a62,
    0x059e51,
    0x1ee41b,
    0xbaff75,   // 50 [49]

    // brown/tan/skin colour...
    0x896c31,
    0x9e8e48,
    0xdaa551,
    0xf3e158,
    0xfadc7d,
    0xffff70,   // 56

    // brown
    0x3d3025,
    0x514432,
    0x776546,
    0xa38354,
    0xd2bf7d,
    0xe5dab7,// 62

    // brown-red tint
    0x9f6a47,   //
    0xad805e,
    0xd2b899,
    0xfce7c3,   // 66

    // brown
    0x291a12,   
    0x38221a,
    0x472d23,
    0x593d2f,
    0x7a5238,
    0xaa7052,   // 72

    // redish
    0x7a1a21,   //
    0xb22a2f,
    0xdb5357,   //75

    // brown/tan
    0xb27e63,
    0xd1b093,   // 77

    // blur-purple-pink...
    0x3c30bf,   //
    0x923bcc,
    0xed49f2,
    0xf873ff,    // 81

    0, 0        // end... (83)
};

void to_SDL_Colour(Uint32 src, SDL_Colour* dest, Uint8 a) {
	Uint8 *x = (Uint8*)&src;
	dest->r = x[2];
	dest->g = x[1];
	dest->b = x[0];
    dest->a = a;// x[3];
}

void to_c64_SDL_Colour(int ix, SDL_Colour* dest, Uint8 alpha) {
    to_SDL_Colour(g_c64_colours[ix], dest, alpha);
}

void *x_malloc(int sz) {
    return SDL_malloc(sz);
}

void x_free(void *ptr) {
    if (ptr)
        SDL_free(ptr);
}

int x_strlen(const char* src) {
    if (src)
        return SDL_strlen(src);
    return 0;
}

int realloc_bytes(int sz) {
    if (sz <= 0)
        return 0;
    if (sz > 1024) {
        sz = (sz / 1024) * 1024 + 1024;
    } else {
        int z = 16;
        while (z < sz) {
            z *= 2;
        }
        sz = z;
    }
    return sz;
}

void *x_realloc(void *base, size_t sz, bool *ok) {
    void *retval;
    retval = SDL_realloc(base, sz);
    if (ok)
        *ok = ((sz && retval) || (sz == 0 && retval == 0));
    return retval;
}

void ShouldQuit() {
    SDL_assert(g_app != 0);
    g_app->ShouldQuit();
}

void Debug_Printf(const char *prefix, SDL_Rect *rect) {
    if (prefix && rect) {
        SDL_Log("%s : % d, % d x % d, % d", prefix, rect->x, rect->y, rect->w, rect->h);
    } else if (prefix) {
        SDL_Log("%s", prefix);
    } else if (rect) {
        SDL_Log("%d,%d x %d,%d", rect->x, rect->y, rect->w, rect->h);
    }
}

void Debug_Char(const char* src, const char* prefix) {
    if (prefix)
        SDL_Log("%s", prefix);
    if (src)
        SDL_Log("%s", src);
    else
        SDL_Log("NULL");
}
void Debug_Point(const SDL_Point* pt) {
    if (pt)
        SDL_Log("%d,%d", pt->x, pt->y);
}
void Debug_Rect(const SDL_Rect* rect) {
    if (rect) {
        SDL_Log("X:%d,Y:%d,W:%d (%d),H:%d (%d)",
            rect->x, rect->y,
            rect->w, (rect->x + rect->w - 1),
            rect->h, (rect->y + rect->h - 1)
        );
    }
}

const char* Debug_SafeChar(const char* src) {
    return src ? src : g_null_string;
}

void* x_allocate_units(int sz_per_item, int desired, void* base, int* used, int* allocated, int buffer /*= 10*/) {
    void* retval;
    if (desired > *allocated) {
        int sz = desired + buffer;
        retval = x_realloc(base, sz * sz_per_item, 0);
        *allocated = sz;
    }
    else if (desired <= 0) {
        if (base)
            x_free(base);
        retval = 0;
        desired = 0;
    }
    else {
        retval = 0;
    }
    *used = desired;
    return retval;
}

int xIntRange(int a, int low, int upper) {
    if (a < low)
        return low;
    else if (a > upper)
        return upper;
    return a;
#if 0
    if (a < low)
        return low;
    if (a > upper)
        return upper;
    return a;
#endif
}

void SDL_GetRectX2Y2(const SDL_Rect *src, int *x2, int *y2) {
    *x2 = src->x + src->w - 1;
    *y2 = src->y + src->h - 1;
}

int ExpandOuter_Rect(SDL_Rect *dest, const SDL_Rect *test) {
    int retval = 0;
    int dx2, dy2, tx2, ty2;
    dx2 = dest->x + dest->w ;
    dy2 = dest->y + dest->h;
    tx2 = test->x + test->w;
    ty2 = test->y + test->h;
    if (test->x < dest->x) {
        dest->x = test->x;
        retval++;
    }
    if (test->y < dest->y) {
        dest->y = test->y;
        retval++;
    }
    if (tx2 > dx2) {
        dx2 = tx2;
        //dest->w = (tx2 - dest->x);
        retval++;
    }
    if (ty2 > dy2) {
        dy2 = ty2;
        //dest->h = (ty2 - dest->y);
        retval++;
    }

    if (retval) {
        dest->w = (dx2 - dest->x);
        dest->h = (dy2 - dest->y);
    }

    return retval;
}

int x_rand(int low, int high) {
    int retval;
    int diff = (high - low) + 1;
    double pc = ((double)diff / (double)RAND_MAX);
    double x = (pc * (double) rand());
    retval = (int) x + low;
    return retval;
}

int font_pt_to_pixel(int pt) {
    float retval;
    retval = (float)pt;
    retval = retval * 0.75;
    return (int)ceilf(retval);
}
int font_pixel_to_pt(int pixel) {
    float retval;
    retval = (float)pixel;
    retval = retval / 0.75;
    return (int)ceilf(retval);
}

char* x_strdup(const char* src, char* prev ) {
    int l = x_strlen(src);
    char* retval;
    if (l) {
        retval = (char *) x_realloc(prev, l + 1);
        if (retval) {
            SDL_memcpy(retval, src, l);
            retval[l] = 0;
        }
    } else {
        x_free(prev);
        retval = 0;
    }
    return retval;
}

int x_AbsMax(int a, int b) {
    int aa, ab;
    aa = SDL_abs(a);
    ab = SDL_abs(b);
    if (aa > ab)
        return a;
    else if (ab > aa)
        return b;
    else if (a > b)
        return a;
    return b;
}

int x_AbsMin(int a, int b) {
    int aa, ab;
    aa = SDL_abs(a);
    ab = SDL_abs(b);
    if (aa < ab)
        return a;
    else if (ab < aa)
        return b;
    else if (a < b)
        return a;
    return b;
}

// set the sdl draw colour to the main renderer...
void SetRenderDrawColour(const SDL_Colour* src) {
    SDL_assert(g_app != NULL);
    SetRenderDrawColour(g_app->m_renderer, src);
}
// sets draw colour to renderer...
void SetRenderDrawColour(SDL_Renderer* renderer, const SDL_Colour* src) {
    SDL_assert(renderer != NULL);
    SDL_assert(src != NULL);
    SDL_SetRenderDrawBlendMode(g_app->m_renderer, src->a == 255 ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, src->r, src->g, src->b, src->a);
}

void SetRenderDrawColour(Uint32 src, Uint8 alpha /*= 255*/) {
    SDL_SetRenderDrawBlendMode(g_app->m_renderer, alpha == 255 ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    Uint8 *rgb = (Uint8 *)&src;
    SDL_SetRenderDrawColor(g_app->m_renderer, rgb[2], rgb[1], rgb[0], alpha);
}

