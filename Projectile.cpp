#include "Projectile.h"
#include "common.h"
#include "App.h"
#include "Hit.h"
#include "spritesheet.h"

void Projectile_Init(cProjectile* dest) {
	SDL_memset(dest, 0, sizeof(cProjectile));
}

cProjectile* Projectile_New() {
	cProjectile* retval;
	retval = (cProjectile*)x_malloc(sizeof(cProjectile));
	if (retval)
		Projectile_Init(retval);
	return retval;
}

void Projectile_Free(void* dest) {
	Projectile_Free((cProjectile*)dest);
}

void Projectile_Free(cProjectile* dest) {
	if (dest) {
		x_free(dest);
	}
}

int Projectile_OnTick(void* p) {
	return Projectile_OnTick((cProjectile *) p);
}

int Projectile_OnTick(cProjectile* i) {
    SDL_assert(i != NULL && g_app != NULL);
    if (i->duration == 0)
        return 1;

    Uint32 delta = g_app->Delta();
    Uint32 dist = SDL_min(i->duration, delta);

    i->duration -= SDL_min(dist, i->duration);

    i->m_rect.x += i->velocity_h;
    i->m_rect.y += i->velocity_v;

    // see if the projectile has hit something...
    // todo: might need to calculate a range that the
    // project has gone... we'll see with testing...

    // see if any tiles are affected
    LinkedList hitList;
    Hit_DetectHits(
                HIT_TYPE_TILE | HIT_TYPE_PLAYER | HIT_TYPE_ENEMY_SHIP,
                &i->m_rect,
                &hitList,
                i->srcP
    );

    cLinkedList *p;
    for (p = hitList.m_list; p != NULL; p=p->pNext) {
        cHit *h = (cHit *)p->m_ptr;
        switch(h->m_type) {
        case HIT_TYPE_TILE:
			if (h->m_tile->m_type != TILE_BEDROCK) {
				h->m_tile->m_health -= h->m_projectile->hit_damage;
				if (h->m_tile->m_health <= 0) {
					// remove the tile???
					h->m_tile->m_type = TILE_NONE;
					g_app->m_tileMap.RemoveItem(h->m_tile);
				}
			}
            break;
        case HIT_TYPE_ENEMY_SHIP:
			h->m_ship->Damage(h->m_projectile->hit_damage);
            break;
        case HIT_TYPE_PLAYER:
			if (h->m_projectile->srcP != h->m_ptr) {
				// incur damage...

				h->m_player->Damage(h->m_projectile->hit_damage);

			}
            break;
        default:
            break;
        };
		// negate the duration...
		i->duration = 0;
    }


    // move the projectile...
    switch (i->type) {
    case PROJECTILE_BULLET:
        // directional...
//		i->m_rect.x += 1.0;// i->velocity_h;
	//	i->m_rect.y += 0.0;// i->velocity_v;
        break;
    case PROJECTILE_BOMB:
        // bomb should fall...
        break;
    default:
        // do nothing.
        break;
    };

    return 0;
	
#if 0

	cLinkedList* i, * prev;
	Uint32 delta, dist;
	delta = Delat();
	i = m_projectiles;
	prev = 0;
	while (i) {
		cProjectile* proj = (cProjectile*)i->m_ptr;

		// process projectile...
		if (proj->duration > 0) {

			dist = SDL_min(proj->duration, delta);
			proj->duration -= dist;

			// calculate the distance for this.


			// move to next item...

			prev = i;
			i = i->pNext;
		}
		else {
			cLinkedList* cpy;
			cpy = i;

			i = i->pNext;

			if (prev) {
				prev->pNext = i;
			}
			else {
				m_projectiles = i;
			}

			cpy->pNext = NULL;
			LinkedList_Delete(cpy, Projectile_Free);
		}
	}
#endif
}

void Projectile_Render(void* p) {
	cProjectile* i = (cProjectile*)p;
	SDL_FRect visible, dummy;
	g_app->GetCameraRect(&visible);

	if (SDL_IntersectFRect(&visible, &i->m_rect, &dummy)) {

        if (i->colour.a) {
			SetRenderDrawColour(g_app->m_renderer, &i->colour);

			SDL_Rect rct;
			rct.x = i->m_rect.x - g_app->m_camera_x;
			rct.y = i->m_rect.y - g_app->m_camera_y;
			rct.w = i->m_rect.w;
			rct.h = i->m_rect.h;
			SDL_RenderFillRect(g_app->m_renderer, &rct);
		}

		switch (i->type) {
		case PROJECTILE_BULLET:
			// render sprite...
			g_app->m_spritesheet.RenderSpriteCamera(&i->m_rect, SPRITESHEET_BULLET, 0, false);
			break;
		case PROJECTILE_ENEMY_BULLET:
		case PROJECTILE_BOMB:
			// render sprite???
			break;
		default:
			break;
		};
	}
}

void Projectile_SetSrc(cProjectile *dest, void *srcP, int srcType) {
    dest->srcP = srcP;
    dest->srcType = srcType;
}

void Projectile_SetSpeed(cProjectile* dest, float* h, float* v) {
	SDL_assert(dest != NULL);
	if (h)
		dest->velocity_h = *h / g_app->m_ffps;
	if (v)
		dest->velocity_v = *v / g_app->m_ffps;
}

