#include "Hit.h"
#include "LinkedList.h"
#include "framework.h"
#include "common.h"
#include "App.h"
#include "array.h"
#include "Point.h"

static cHit *NewHitItem(int type, void *ptr, SDL_Rect *intersectRect);

cHit *NewHitItem(int type, void *ptr, SDL_Rect *intersectRect) {
    cHit *retval = (cHit *)x_malloc(sizeof(cHit));
    if (retval) {
        retval->m_type = type;
        retval->m_ptr = ptr;
        if (intersectRect)
            SDL_memcpy(&retval->m_intersectRect, intersectRect, sizeof(SDL_Rect));
        else
            SDL_memset(&retval->m_intersectRect, 0, sizeof(SDL_Rect));
    }
    return retval;
}


int Hit_DetectHits(int type, SDL_FRect *text, LinkedList *p, void *exclude) {
    int retval;
    SDL_Rect irect;
    irect.x = text->x;
    irect.y = text->y;
    irect.w = text->w;
    irect.h = text->h;
    retval = Hit_DetectHits(type, &irect, p, exclude);
    return retval;
}

// detect hits: type - should be bitwise using the HIT enum above.
//extern bool Hit_DetectHits(int type, SDL_Rect *text, LinkedList *p);
int Hit_DetectHits(int type, SDL_Rect *rect, LinkedList *src, void* exclude) {
    int retval = 0;
    src->Clear();
    src->m_free_callback = x_free;

    cHit *p;
    SDL_Rect d, r;

	if (type & HIT_TYPE_PLAYER) {
        // go through player
        for (int x = 0; x < g_app->m_playersUsed; x++) {
            if (g_app->m_players[x].m_active && &g_app->m_players[x] != exclude) {
                g_app->m_players[x].GetRect(&d);
                if (SDL_IntersectRect(&d, rect, &r)) {
                    p = NewHitItem(HIT_TYPE_PLAYER, &g_app->m_players[x], &r);
                    if (p) {
                        src->Unshift(p);
                        retval++;
                    }
                }
            }
         }
	}

	if (type & HIT_TYPE_ENEMY_SHIP) {
        cLinkedList *item;
        cEnemyShip *ship;
        for (item = g_app->m_enemyShips.m_list; item != NULL; item = item->pNext) {
            ship = (cEnemyShip *)item->m_ptr;
            if (ship->m_isActive && ship->HitDetect(rect) && exclude != ship) {
                p = NewHitItem(HIT_TYPE_ENEMY_SHIP, ship, &r);
                if (p) {
                    src->Unshift(p);
                    retval++;
                }
            }
        }
    }


	if (type & HIT_TYPE_TILE) {
        int c = g_app->m_tileMap.HitDetect(rect);
        for (int x = 0; x < c; x++) {
            p = NewHitItem(
                    HIT_TYPE_TILE,
                    g_app->m_tileMap.GetHitTile(x),
                    &g_app->m_tileMap.m_hitRect
            );
            if (p) {
                src->Unshift(p);
                retval++;
            }
        }
	}

    return retval;
}

int Hit_IndexOf(LinkedList *p, void *ptr) {
    int retval = 0;
    cLinkedList *pItem = p->m_list;
    while (pItem) {
        if (pItem->m_ptr == ptr)
            return retval;
        retval++;
        pItem = pItem->pNext;
    }
    return -1;
}


Hits::Hits() :
    m_ship_hits(ARRAY_CHIT),
    m_tile_hits(ARRAY_CHIT),
    m_projectile_hits(ARRAY_CHIT),
    m_player_hits(ARRAY_CHIT),
    m_exclude_ptrs(ARRAY_PTR)
{
    auto_clear = true;
    
}

Hits::~Hits() {
    Clear();
}

void Hits::PushExclude(void* ptr) {
    m_exclude_ptrs.push_back(ptr);
}

int Hits::IndexOfExclude(const void* ptr) {
    if (m_exclude_ptrs.m_used <= 0)
        return -1;
    return m_exclude_ptrs.IndexOf(ptr);
}

void Hits::ClearExclude() {
    m_exclude_ptrs.clear();
}
void Hits::ClearHits() {   // clear hits
    m_player_hits.SoftClear();
    m_projectile_hits.SoftClear();
    m_ship_hits.SoftClear();
    m_tile_hits.SoftClear();
}

int Hits::DetectHits(int type, SDL_Rect *rect) {
    int retval = 0;

    if (type & HIT_TYPE_PLAYER)
        retval += DetectPlayerHits(rect);

    if (type & HIT_TYPE_ENEMY_SHIP)
        retval += DetectShipHits(rect);

    if (type & HIT_TYPE_TILE)
        retval += DetectTileHits(rect);

    if (type & HIT_TYPE_PROJECTILE) {
        // TODO...
    }
    return retval;
}

int Hits::DetectPlayerHits(SDL_Rect *rect) {

    if (auto_clear)
        m_player_hits.SoftClear();

    int retval = m_player_hits.size();
    cHit hit;
    SDL_Rect d;
    for (int x = 0; x < g_app->m_playersUsed; x++) {
        if (g_app->m_players[x].m_active && IndexOfExclude (& g_app->m_players[x]) < 0) {
            g_app->m_players[x].GetRect(&d);
            if (SDL_IntersectRect(&d, rect, &hit.m_intersectRect)) {
                hit.m_type = HIT_TYPE_PLAYER;
                hit.m_ptr = &g_app->m_players[x];
                m_player_hits.push_back(&hit);
            }
        }
    }
    return m_player_hits.size() - retval;
}

int Hits::DetectShipHits(SDL_Rect *rect) {

    if (auto_clear)
        m_ship_hits.SoftClear();


    cLinkedList *item;
    cEnemyShip *ship;
    cHit hit;
    int retval = m_ship_hits.size();
    for (item = g_app->m_enemyShips.m_list; item != NULL; item = item->pNext) {
        ship = (cEnemyShip *)item->m_ptr;
        if (ship->m_isActive && ship->HitDetect(rect) && IndexOfExclude(ship) < 0) {
            hit.m_type = HIT_TYPE_ENEMY_SHIP;
            hit.m_ptr = ship;
            hit.m_intersectRect = ship->m_intersectHit;
            m_ship_hits.push_back(&hit);
        }
    }
    return m_ship_hits.size() - retval;
}

int Hits::DetectTileHits(SDL_Rect *rect) {

    if (auto_clear)
        m_tile_hits.SoftClear();

    int retval = m_tile_hits.size();
    int c = g_app->m_tileMap.HitDetect(rect);
    cHit hit;
    for (int x = 0; x < c; x++) {
        hit.m_ptr = g_app->m_tileMap.GetHitTile(x);
        
        if (IndexOfExclude(hit.m_ptr) < 0) {
            hit.m_type = HIT_TYPE_TILE;
            hit.m_intersectRect = g_app->m_tileMap.m_hitRect;
            m_tile_hits.push_back(&hit);
        }
    }
    return m_tile_hits.size() - retval;
}

int Hits::DetectProjectileHits(SDL_Rect *rect) {
    // TODO...
    if (auto_clear)
        m_projectile_hits.SoftClear();
    int retval = m_projectile_hits.Size();


    return m_projectile_hits.Size() - retval;
}

int Hits::Size() {
    int retval = 0;
    retval += m_player_hits.size();
    retval += m_ship_hits.size();
    retval += m_tile_hits.size();
    retval += m_projectile_hits.size();
    return retval;
}

// sort current hit list to closest...
bool Hits::DetermineClosest(SDL_Point* pt, cHit* dest) {
    SDL_assert(pt != NULL);

    int i, sz;
    int dummy_w, dummy_h, dummy_d;
    cHit* pHit, *pFound = NULL;
    int dist;
    cPoint hitPoint, foundPoint;

    sz = Size();
    for (i = 0; i < sz; i++) {
        cHit* pHit = At(i);
        SDL_assert(pHit != NULL);

        hitPoint.Set(&pHit->m_intersectRect, pt);

        if (pFound == NULL) {
            pFound = pHit;
            foundPoint = hitPoint;
            dist = abs(foundPoint.DistanceTo(pt, &dummy_w, &dummy_h, &dummy_d));
        }
        else {
            int testDistance;
            testDistance = hitPoint.DistanceTo(pt, &dummy_w, &dummy_h, &dummy_d);
            if (testDistance < dist) {
                dist = testDistance;
                pFound = pHit;
                foundPoint = hitPoint;
            }
        }
    }

    if (!pFound)
        return false;
    
    if (dest)
        *dest = *pFound;
    return true;
}

cHit* Hits::At(int index) {
    if (index < m_player_hits.size())
        return m_player_hits.AtHit(index);
    index -= m_player_hits.size();
    if (index < m_ship_hits.size())
        return m_ship_hits.AtHit(index);
    index -= m_ship_hits.size();
    if (index < m_tile_hits.size())
        return m_tile_hits.AtHit(index);
    index -= m_tile_hits.size();
    if (index < m_projectile_hits.size())
        return m_projectile_hits.AtHit(index);
    return NULL;
}
