#pragma once
//
// Logic/class to detect hits
//

#include "framework.h"
#include "LinkedList.h"
#include "array.h"
#include <vector>

struct cLinkedList;
class cPlayer;
class cEnemyShip;
class cTile;
struct cProjectile;

enum {
	HIT_TYPE_NONE = 0,
	HIT_TYPE_PLAYER=1,
	HIT_TYPE_ENEMY_SHIP=2,
    HIT_TYPE_TILE=4,
    HIT_TYPE_PROJECTILE=8,


	HIT_TYPE_ALL = 255
};

typedef struct cHit {
	int m_type;
    SDL_Rect m_intersectRect;    // intersecting rect
	union {
		void* m_ptr;
		cPlayer* m_player;
		cEnemyShip* m_ship;
		cTile* m_tile;
        cProjectile *m_projectile;
	};
} cHit;

// detect hits: type - should be bitwise using the HIT enum above.
// type - see above, test - the rect to test, p - will store results
// here, will clear and set type in list...
extern int Hit_DetectHits(int type, SDL_Rect *text, LinkedList *p, void *exclude = NULL);
extern int Hit_DetectHits(int type, SDL_FRect *text, LinkedList *p, void *exclude = NULL);

class Hits {
public:
    Hits();
    virtual ~Hits();

    void PushExclude(void* ptr);
    int IndexOfExclude(const void* ptr);
       
    void ClearExclude();
    void ClearHits();   // clear hits
    void Clear() {
        ClearExclude();
        ClearHits();
    }; // clear hits and excludes...

    // Detect hits... these do not clear the arrays first...
    int DetectHits(int type, SDL_Rect *rect);

    int DetectPlayerHits(SDL_Rect *rect);
    int DetectShipHits(SDL_Rect *rect);
    int DetectTileHits(SDL_Rect *rect);
    int DetectProjectileHits(SDL_Rect *rect);

    int Size();
    
    cHit* At(int index);

    // sort current hit list to closest...
    // the hits arrays should be populated already...

    bool DetermineClosest(SDL_Point* pt, cHit *dest);

    //cArray m_all_hits;
#if 1
    cArray m_player_hits;
    cArray m_ship_hits;
    cArray m_tile_hits;
    cArray m_projectile_hits;
#endif
    cArray m_exclude_ptrs;

    bool auto_clear;    // clear before detect...
};


//
extern int Hit_IndexOf(LinkedList *p, void *ptr);

// free the linked list (uses x_free)...
//extern void Hit_FreeList(cLinkedList* ptr);


