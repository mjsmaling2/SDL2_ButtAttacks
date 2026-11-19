#pragma once

#include "Framerate.h"
#include "common.h"


typedef struct cFont {
	TTF_Font* font;
	int ptsz;	// point sz
} cFont;


// generates font...
extern void init_font();
extern void cleanup_font();

// clear old fonts...
extern void clear_old(int max = 10);

extern void Font_Free(cFont* ptr);
extern void Font_Free(void* ptr);

extern TTF_Font* get_font(int pt_sz);
extern TTF_Font* get_font_px(int px);
