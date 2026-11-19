#ifndef COMMON_H
#define COMMON_H

#include "framework.h"

#include <SDL2/SDL_stdinc.h>
//#include "App.h"

extern int g_canvas_width, g_canvas_height;
extern int g_tile_size;	// size - for width and height

extern Uint32 g_c64_colours[16];
extern Uint32 g_palette[83];	// 32

class cApp;
extern cApp *g_app;

extern const char* g_null_string;


// convert sdl colour from src to dest...
extern void to_SDL_Colour(Uint32 src, SDL_Colour *dest, Uint8 alpha = 255);
extern void to_c64_SDL_Colour(int ix, SDL_Colour* dest, Uint8 alpha = 255);

// set the sdl draw colour to the main renderer...
extern void SetRenderDrawColour(const SDL_Colour* src);
extern void SetRenderDrawColour(Uint32 src, Uint8 alpha = 255);
// sets draw colour to renderer...
extern void SetRenderDrawColour(SDL_Renderer *renderer, const SDL_Colour* src);

extern void *x_malloc(int sz);
extern void x_free(void *ptr);
extern void *x_realloc(void *base, size_t sz, bool *ok=0);
extern char* x_strdup(const char* src, char* prev = 0);


// returnes desired amount of bytes (to reduce realloc's usages).
extern int realloc_bytes(int sz);

// returns a number within low and upper (inclusive)
extern int xIntRange(int a, int low, int upper);
	

// return a random number from low to high (inclusive).
extern int x_rand(int low, int high);

extern void* x_allocate_units(int sz_per_item, int desired, void* base, int* used, int* allocated, int buffer = 10);

// returns 0 (if null) or >= 0 if valid src pointer.
extern int x_strlen(const char* src);

extern void ShouldQuit();

extern void SDL_GetRectX2Y2(const SDL_Rect *src, int *x2, int *y2);

// this will expand dest if text x1,y2,w,h is larger than dest...
extern int ExpandOuter_Rect(SDL_Rect *dest, const SDL_Rect *test);

extern void Debug_Printf(const char *prefix, SDL_Rect *rect);
extern void Debug_Char(const char* src, const char *prefix = 0);
extern void Debug_Point(const SDL_Point* pt);
extern void Debug_Rect(const SDL_Rect* rect);
// if empty will return a string containing "NULL"
extern const char* Debug_SafeChar(const char* src);

extern int font_pt_to_pixel(int pt);
extern int font_pixel_to_pt(int pixel);

//
// will return the Abd max a,b, but with the - + intacte... ie:
// x_AbsMax(-10, 5) will return -10.
//
extern int x_AbsMax(int a, int b);
extern int x_AbsMin(int a, int b);
#endif // COMMON_H
