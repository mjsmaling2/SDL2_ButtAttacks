#pragma once

#include "framework.h"

class cEnemyShip;
class cPlayer;

enum {
	PROJECTILE_NONE = 0,
	PROJECTILE_BULLET = 1,	// horizontal - one direction
    PROJECTILE_BOMB = 10,		// something that falls to the ground
	PROJECTILE_ENEMY_BULLET = 50,



    PROJECTILE_SRC_NONE = 0,
    PROJECTILE_SRC_PLAYER = 1,
    PROJECTILE_SRC_ENEMY,


	// bitwise ... what damage...
	PROJECTILE_DAMAGE_NONE = 0,
	PROJECTILE_DAMAGE_DEFAULT = 0,	// will depend on PROJECTILE_SRC...
	PROJECTILE_DAMAGE_PLAYER = 1,
	PROJECTILE_DAMAGE_ENEMY = 2,
	PROJECTILE_DAMAGE_TILE = 128,
	PROJECTILE_DAMAGE_ALL = 255
};

typedef struct cProjectile {
	int type;	// ie: none, bullet, bomb, etc.

    union {
        void *srcP;	// pointer to the cEnemy or cPlayer depending on type.

        // convienent pointers...
        cPlayer *playerP;
        cEnemyShip *enemyShipP;
    };
    int srcType;		// ie: PROJECTILE_SRC_PLAYER...

	// velocity should be based on pixels per frame... (FPS)
	float velocity_h;	// horizontal way, can be > 0 or < 0, this will also be speed.
	float velocity_v;	// vertical velocity, can be > 0 or < 0

	Uint32 duration;	// how long the projectile should be active for. (in time)
	int hit_damage;	// how much damage would this inflict, maybe 0 for no damage.

	int hit_damage_what;	// what damages : 0 - default...

	SDL_FRect m_rect;	// the point and size of the projectile.
	
	int gravity;	// indicates if gravity effects this.

	SDL_Color colour;

} cProjectile;

extern void Projectile_Init(cProjectile* dest);
extern cProjectile* Projectile_New();
extern void Projectile_Free(void* dest);
extern void Projectile_Free(cProjectile* dest);

extern void Projectile_SetSrc(cProjectile *dest, void *srcP, int srcType);

// this will process the tick, if return > 0 then no activity (should be
// removed from  a linked list for example )
extern int Projectile_OnTick(void* p);
extern int Projectile_OnTick(cProjectile* i);

extern void Projectile_Render(void* p);

// the h/w if set should be speed per second, this will convert to
// pixels per frame... w / FPS, h / FPS
extern void Projectile_SetSpeed(cProjectile* dest, float* h, float* v);


//extern void Projectile_Velocity(cProjectile *src, int )


