#pragma once
/*
	A particle or special effect...
	can be an explosion, or a spark for example.
	Something that doesn't really have an effect 
	on the player of enemy ships (in terms of damage).

*/
#include "framework.h"


enum {
	PARTICLE_NONE = 0,
	PARTICLE_EXPLOSION
};

typedef struct cParticle {
	int m_type;
	
    Uint32 m_animationStepDuration;      // animation step
    Uint32 m_elapsed;   // time elapsed
    Uint32 m_duration;  // how long the particle will show
	Uint32 m_intial_delay;		// initial delay of when this should start...

    int m_animationStep;    // animation stepped.
    int m_sprite;           // which sprite: ie: SPRITE_EXPLOSION

    int xx, yy; // x,y coordinates  // assume centre point.

	int m_active;
} cParticle;

extern cParticle *Particle_New(int type);
extern void Particle_Free(void* p);

extern int Particle_OnTick(void *p);

//
// will get the sprite rect, point x1, y1, then ww,hh from sprite.
// will return > 0 if valid sprite, or == 0 if not.
extern int Particle_GetSpriteRect(cParticle *part, SDL_Rect *dest);
