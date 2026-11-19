#pragma once

#include "framework.h"

typedef struct cTile {
	int m_col, m_row, m_type;
    int m_health;   // health of the tile...
} cTile;

enum {

	TILE_NONE = 0,

	TILE_BEDROCK,
	TILE_DIRT,
	TILE_GRASS,


	// no tile... instead of deleting removing, we'll mark as none...
	

	TILE_MAX
};

extern SDL_Colour Tile_Colours[TILE_MAX];

extern void Tile_Init(cTile *p, int type, int col, int row);

extern int Tile_Cmp(const cTile* a, const cTile* b);
extern int Tile_Cmp(const void *a, const void *b, size_t len);
extern void Tile_GetRect(const cTile* a, SDL_Rect* dest);
#if 0

class cTile {
public:
	cTile(int col, int row, int type) {
		m_col = col;
		m_row = row;
		m_type = type;
	};
	cTile();
	~cTile();

	enum {
		TILE_BEDROCK = 0,
		TILE_DIRT,
		TILE_GRASS,


		TILE_MAX
	};

	int Compare(const cTile &src);

	// will use: g_tile_size
	void GetRect(SDL_Rect* dest);

	int m_col, m_row;
	int m_type;

	static SDL_Colour tile_colours[TILE_MAX];

};
#endif
