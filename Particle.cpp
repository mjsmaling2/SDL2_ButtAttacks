#include "Particle.h"
#include "common.h"
#include "App.h"

cParticle* Particle_New(int type) {
	cParticle* retval;
	retval = (cParticle *)x_malloc(sizeof(cParticle));
	if (retval) {
		SDL_memset(retval, 0, sizeof(cParticle));
		retval->m_type = type;
	}
	return retval;
}
void Particle_Free(void* p) {
	if (p)
		x_free(p);
}

int Particle_OnTick(void *p) {

    cParticle *part = (cParticle *)p;

    if (part->m_active <= 0)
        return 1;

    int l = part->m_animationStep;
    Uint32 delta;
    delta = g_app->Delta();


    if (part->m_intial_delay > 0) {
        Uint32 adj = SDL_min(delta, part->m_intial_delay);
        part->m_intial_delay -= adj;
        
        if (part->m_intial_delay > 0)
            return 0;

        delta -= adj;
    }

    part->m_elapsed += delta;


    if (part->m_elapsed > part->m_duration) {
        part->m_active = 0;
        part->m_elapsed = part->m_duration;
    }

    switch(part->m_type) {
    case PARTICLE_EXPLOSION:
        // do something here...
        part->m_animationStep = part->m_elapsed / part->m_animationStepDuration;

        break;
    default:
        part->m_active = 0;
        break;
    };

    if (l != part->m_animationStep)
        g_app->ShouldRender();

    return 0;
}

int Particle_GetSpriteRect(cParticle *part, SDL_Rect *dest) {
    int retval = 0;

    // make sure sprite and nth is valid.
    if (part->m_sprite < 0 ||  part->m_sprite >= SPRITESHEET_MAX)
        return retval;

    // make sure animation step is wthin range also.
    if (part->m_animationStep < 0 || part->m_animationStep >= g_app->m_spritesheet.g_spritesheet_map[part->m_sprite].sz)
        return retval;

    // all good... we can get the sz.
    retval++;
    if (dest) {
        g_app->m_spritesheet.GetSpriteSheetSz(dest, part->m_sprite, part->m_animationStep);

        dest->x = part->xx - dest->w / 2;
        dest->y = part->yy - dest->h / 2;
    }

    return retval;
}
