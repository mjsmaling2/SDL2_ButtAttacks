#include "Tile.h"
#include "common.h"



#if 0
enum {
	TILE_BEDROCK = 0,
	TILE_DIRT,
	TILE_GRASS,

	TILE_MAX
};
#endif

SDL_Colour Tile_Colours[TILE_MAX];

int Tile_Cmp(const cTile* a, const cTile* b) {
	int retval;
	retval = a->m_col - b->m_col;
	if (retval == 0) {
		retval = a->m_row - b->m_row;
		if (retval == 0)
			retval = a->m_type - b->m_type;
	}
	return retval;
}

int Tile_Cmp(const void *a, const void *b, size_t len) {
    SDL_assert(len == sizeof(cTile));
    return Tile_Cmp((const cTile *)a, (const cTile *)b);
}

void Tile_GetRect(const cTile* a, SDL_Rect* dest) {
	dest->x = a->m_col * g_tile_size;
	dest->w = g_tile_size;
	dest->h = g_tile_size;
	dest->y = g_canvas_height - (a->m_row * g_tile_size);
	dest->y -= g_tile_size;
}

void Tile_Init(cTile *p, int type, int col, int row) {
    switch(type) {
    case TILE_DIRT:
        p->m_health = 25;
        break;
    case TILE_GRASS:
        p->m_health = 10;
        break;
    case TILE_BEDROCK:
    default:
        p->m_health = 1000; // not breakable...
        break;
    };
    p->m_type = type;
    p->m_col = col;
    p->m_row = row;
}

#if 0
SDL_Colour cTile::tile_colours[TILE_MAX];	// static

cTile::cTile() {
	m_col = m_row = m_type = 0;
}
cTile::~cTile() {

}

int cTile::Compare(const cTile &src) {
	int retval;
	retval = m_col - src.m_col;
	if (retval == 0) {
		retval = m_row - src.m_row;
		if (retval == 0)
			retval = m_type - src.m_type;
	}
	return retval;
}

void cTile::GetRect(SDL_Rect* dest) {
	dest->x = m_col * g_tile_size;
	dest->w = g_tile_size;
	dest->h = g_tile_size;
	dest->y = g_canvas_height - (m_row * g_tile_size);
	dest->y -= g_tile_size;
}
#endif

