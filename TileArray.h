#pragma once

#include "framework.h"
#include "Tile.h"
#include "array.h"
#include <vector>

class cTileArray
{
public:
	cTileArray();
	~cTileArray();


    cTile* At(int idx);

	// will add to end...
	void AddItem(int col, int row, int type);

	void AddItem(const cTile& src);
	void RemoveItem(int ix);
	void RemoveItem(cTile* pTile);

	// sort array (if needed)
	void SoftSort();

	// find in array.
	int FindColumn(int col);

	// determine height (start index, end index) of column
	// will return the number of tiles in column...
    int ColumnInfo(int col, int *start_i, int *end_i);
    //int ColumnHeight(int col, int *start_i, int *end_i);

    // the start index of a column
    int ColumnStartI(int col);

    // the end index of a column
    int ColumnEndI(int col);

    // the height of a column
    int ColumnHeight(int col);

	// this will determine the height of the tiles... 
	// x1 - (x position pixels), w - width (in pixels).
	// will return the start column
	// will return the number of 'hits' for the rect...
	// use m_hitTiles* variables to determine more info...
	int HitDetect(SDL_Rect* rect);

	int Allocate(int items);

	// will safely return the column 
    int TileColumn(int idx);

	void TileRect(int idx, SDL_Rect* dest);

    int Size() const {return m_tiles.m_used;};

	//
	// TODO: Need to make the lookup's faster...
	//
    //bool AllocateHitTiles(int space);

    cTile *GetHitTile(int ix);

    SDL_Rect m_hitRect;
public:	// let members be public...
    cArray m_hitTiles;
//    int *m_hitTiles;	// should not be more than 10.
//	int m_hitTilesUsed;
 //   int m_hitTilesAllocated;



	// this will be the top level tile indexes... this will be 
	// a faster lookup for when detecting hits with the ground...
	//int* m_topTiles;

    cArray m_tiles;
    //std::vector<cTile> m_tiles;
    //cTile* m_pArray;
	
    //int m_used;
	int m_sorted;

	int m_cursor;

	int m_tileSize;	// 

protected:
	int m_allocated;
};

