#include "TileArray.h"
#include "common.h"

cTileArray::cTileArray() :
    m_tiles(ARRAY_TILE),
    m_hitTiles(ARRAY_INT) {

	m_allocated = 0;

	m_sorted = 0;

    m_hitTiles.AllocateSpace(20);
}

cTileArray::~cTileArray() {
    m_tiles.Clear();
    m_hitTiles.Clear();
}

// will add to end...
void cTileArray::AddItem(int col, int row, int type) {

    cTile tile;

    Tile_Init(&tile, type, col, row);

    m_tiles.PushItem(&tile);

	m_sorted = 0;
}


void cTileArray::AddItem(const cTile& src) {
    m_tiles.PushItem(&src);
	m_sorted = 0;
}

void cTileArray::RemoveItem(int ix) {
    SDL_assert(ix >= 0 && ix < Size());
    m_tiles.RemoveItem(ix);
    if (ix <= m_cursor) {
        int sz = Size();
        if (sz <= m_cursor) {
            m_cursor = SDL_max(0, sz - 1);
        }
    }
    //m_used--;
    //for (int x = ix; x < Size(); x++) {
        //m_pArray[x] = m_pArray[x + 1];
    //}
}

void cTileArray::RemoveItem(cTile* pTile) {
    // find the tile...
    int a, b, i;
    ColumnInfo(pTile->m_col, &a, &b);
    for (i = a; i <= b; i++) {
        if (At(i) == pTile) {
            RemoveItem(i);
            break;
        }
    }
}

// sort array (if needed)
void cTileArray::SoftSort() {

    m_tiles.SoftSort();
#if 0
	if (m_sorted)
		return;

	// do the boolean swap...
    cTile c;
    int lower, upper, swapped, i, x;
    lower = 0;
    upper = m_used -1;

loop_1:
    swapped = 0;
    for (i = lower; i < upper; i++) {
		int cmp = Tile_Cmp(&m_pArray[i], &m_pArray[i + 1]); // cTile::Compare(&m_pArray[i], &m_pArray[i + 1]);
        if (cmp > 0) {
            c = m_pArray[i];
            m_pArray[i] = m_pArray[i + 1];
            m_pArray[i + 1] = c;
            swapped++;
        }
    }

    if (swapped) {
        swapped = 0;
        upper--;

        for (x = upper; x > lower; x--) {
			int cmp = Tile_Cmp(&m_pArray[i - 1], &m_pArray[i]);
            if (cmp > 0) {
                c = m_pArray[i];
                m_pArray[i] = m_pArray[i - 1];
                m_pArray[i - 1] = c;
                swapped++;
            }
        }

        if (swapped) {
            lower++;
            goto loop_1;
        }
    }

	m_sorted = 1;
#endif
}

// find in array.
int cTileArray::FindColumn(int col) {
    int used = Size();
    if (used == 0)
        return -1;

    if (m_cursor >= used || m_cursor < 0)
        m_cursor = used / 2;

    int upper, lower;
    upper = used + 1;
	lower = 0;

loop_here:

    if (TileColumn(m_cursor) == col) {
		// go to first spot.
		while (TileColumn(m_cursor - 1) == col)
			m_cursor--;
		return m_cursor;
	}
    if (TileColumn(m_cursor) < col) {
        lower = m_cursor+1;
	} else { // <
        upper = m_cursor;
	}
	if (lower < upper) {
		m_cursor = (upper + lower) / 2;
		goto loop_here;
	}
	return -1;
}

// determine height (start index, end index) of column

// the start index of a column
int cTileArray::ColumnStartI(int col) {
    int i;
    i = FindColumn(col);
    if (i >= 0) {
        while (TileColumn(i - 1) == col)
            i--;
    }
    return i;
}

// the end index of a column
int cTileArray::ColumnEndI(int col) {
    int i;
    i = FindColumn(col);
    if (i >= 0) {
        while (i < Size() && TileColumn(i + 1) == col)
            i++;
    }
    return i;
}

// the height of a column
int cTileArray::ColumnHeight(int col) {
    int i;
    int retval = 0;
    i = ColumnStartI(col);
    if (i >= 0) {
        retval++;
        while (TileColumn(i+1)==col) {
            i++;
            retval++;
        }
    }
    return retval;
}

// int ColumnInfo(int col, int *start_i, int *end_i);
int cTileArray::ColumnInfo(int col, int* start_i, int* end_i) {
    int a, b, r=0;
    a = ColumnStartI(col);
    if (a < 0) {
        b = -1;
    } else {
        b = ColumnEndI(col);
        r = (b -a) + 1;
    }
    if (start_i)
        *start_i = a;
    if (end_i)
        *end_i = b;
    return r;
}

int cTileArray::Allocate(int items) {
    if (items <= 0)
        m_tiles.Clear();
    else
        m_tiles.AllocateSpace(items);
    return m_tiles.m_allocated;
}


cTile* cTileArray::At(int idx) {
//    if (idx < 0 || idx >= m_used)
  //      return NULL;
    SDL_assert(idx >= 0 && idx < m_tiles.m_used);
    return &m_tiles.m_tileArray[idx];
}


void cTileArray::TileRect(int idx, SDL_Rect* dest) {
	cTile* pTile = At(idx);
	if (pTile)
		Tile_GetRect(pTile, dest);
	else if (dest)
		SDL_memset(dest, 0, sizeof(SDL_Rect));
}
#if 0
bool cTileArray::AllocateHitTiles(int space) {
	// int HitDetect(SDL_Rect* rect);
	
	// /* void AllocateHitTiles(int space);

	if (space > m_hitTilesAllocated) {
		int sz = space + 10;
		m_hitTiles = (int*)x_realloc(m_hitTiles, sizeof(int) * sz, 0);
		if (!m_hitTiles) {
			SDL_Log("Could not allocate space for m_hitTiles...");
			ShouldQuit();
			return false;
		}
		m_hitTilesAllocated = sz;
	}
	
	return true;
}
#endif
int cTileArray::HitDetect(SDL_Rect* rect) {
	//
	// TODO: Need to make the lookup's faster...
	//
	int col1 = rect->x / g_tile_size;
	int x2 = (rect->x + rect->w - 1);

	int col2 = x2 / g_tile_size;
	if (x2 % g_tile_size != 0)
		col2 += 1;

	int h, i1, i2;

    m_hitTiles.m_used = 0;

	int x, y;
	SDL_Rect tileRect;
	for (x = col1; x <= col2; x++) {
        h = ColumnInfo(x, &i1, &i2);
        if (i1 < 0)
            continue;
		for (y = i1; y <= i2; y++) {
            
            if (m_tiles.m_tileArray[y].m_type == TILE_NONE)
                continue;
            Tile_GetRect(&m_tiles.m_tileArray[y], &tileRect);
	//		SDL_Log("Testing: x (%d), y (%d) : %d,%d,%d,%d", x, y, tileRect.x, tileRect.y, tileRect.w, tileRect.h);
			if (SDL_HasIntersection(rect, &tileRect)) {
                m_hitTiles.m_intArray[ m_hitTiles.m_used++ ] = y;

                if (m_hitTiles.m_used == 1 ) {
					SDL_memcpy(&m_hitRect, &tileRect, sizeof(SDL_Rect));
				} else {
					if (m_hitRect.x != tileRect.x) {
						tileRect.x = SDL_min(tileRect.x, m_hitRect.x);
						tileRect.w = m_hitRect.x + tileRect.w - m_hitRect.x;
					}
					if (m_hitRect.y != tileRect.y) {
						tileRect.y = SDL_min(tileRect.y, m_hitRect.y);
						tileRect.h = m_hitRect.y + tileRect.h - m_hitRect.y;
					}
				}
			}
		}
	}

    return m_hitTiles.m_used;
}

cTile *cTileArray::GetHitTile(int ix) {
    if (ix < 0 || ix >= m_hitTiles.m_used)
        return 0;
    ix = m_hitTiles.m_intArray[ix];
    if (ix < 0 || ix >= m_tiles.m_allocated)
        return 0;
    return &(m_tiles.m_tileArray[ix]);
}


int cTileArray::TileColumn(int idx) {
    if (idx < 0 || idx >= m_tiles.m_used)
        return -1;
    return m_tiles.m_tileArray[idx].m_col;
}
