#include "EnemyShip.h"
#include "spritesheet.h"
#include "App.h"
#include "common.h"
#include "Projectile.h"
#include "Particle.h"
#include "Hit.h"
#include "cRectAdj.h"
#include "Rect.h"


void EnemyShip_Delete(void* ptr) {
    cEnemyShip* p = (cEnemyShip*)ptr;
    if (p)
        delete p;
}

void EnemyShip_OnTick(void* ptr) {
    cEnemyShip* p = (cEnemyShip*)ptr;
    if (p && p->m_isActive)
        p->OnTick();
}

int EnemyShip_IsActive(void *ptr) {
    cEnemyShip *p = (cEnemyShip *)ptr;
    return (p && p->m_isActive) ? 1 : 0;
}

int EnemyShip_IsInActive(void *ptr) {
    int retval = EnemyShip_IsActive(ptr);
    return retval ? 0 : 1;
}


cEnemyShip::cEnemyShip()  {
	
	m_isActive = false;

	m_sprite.SetType(SPRITESHEET_ENEMYSHIP);

	m_state = STATE_IDLE;

	m_health = m_maxHealth = 100;

    m_h_direction = 1;

    m_radar_wait = m_wait_idle = x_rand(0, 1000);

    m_alt_step_movement = 0;

    m_wait_shoot = x_rand(1000, 3000);

    m_hits.PushExclude(this);

}

cEnemyShip::~cEnemyShip() {
}

void cEnemyShip::MaybeShoot() {
    if (m_wait_shoot) {
        m_wait_shoot -= SDL_min(m_wait_shoot, g_app->Delta());
        if (m_wait_shoot)
            return;
    }

    // want to see what's around... if the player ship is nearby...
    // if so... see if there is an enemy ship near by (that we won't hit)
    // if so... maybe see if there is a tile that we would hit also...

    // so want to see what is the closest object and what it is...

    cRect check_rect;

    check_rect.m_rect.w = 10;
    check_rect.m_rect.h = 10;

    GetPositionCentered(&check_rect.m_rect);

    Hits hits;

    // range of projectile ???
    int range = 640;

    check_rect.m_rect.h = 10;
    check_rect.m_rect.w = range;

    if (!m_h_direction)
        check_rect.m_rect.x -= range;

    hits.PushExclude(this);

    if (hits.DetectHits(HIT_TYPE_PLAYER | HIT_TYPE_ENEMY_SHIP | HIT_TYPE_TILE, &check_rect.m_rect) <= 0)
        return;
    
    cHit hit;
    SDL_Point pt;

    check_rect.GetPosition(&pt);

    m_wait_shoot = x_rand(1000, 3000);

    if (hits.DetermineClosest(&pt, &hit)) {
        switch (hit.m_type) {
        case HIT_TYPE_PLAYER:
            Shoot();
            break;
        case HIT_TYPE_TILE:
            if (x_rand(0, 100) < 30)
                Shoot();
            break;
        case HIT_TYPE_ENEMY_SHIP:   // do not shoot enemy ship
        default:
            break;
        };
    } else {
        // maybe shoot if low probability.
        if (x_rand(0, 100) < 10)
            Shoot();
    }

}

int cEnemyShip::Shoot() {
    
    cProjectile *projectile;

    projectile = Projectile_New();

    //Projectile_Init(&projectile);

    float h, v;
    h = 640.0;
    v = 0.0;

    if (!m_h_direction)
        h = 0.0 - h;

    projectile->srcP = this;
    projectile->type = PROJECTILE_ENEMY_BULLET;
    projectile->srcType = PROJECTILE_ENEMY_BULLET;
    projectile->gravity = 0;
    projectile->hit_damage = 10;
    projectile->hit_damage_what = PROJECTILE_DAMAGE_ALL;
    projectile->duration = 1000;

    SDL_Point pt;
    GetPosition(&pt);

    projectile->m_rect.x = pt.x;
    projectile->m_rect.y = pt.y + 10;
    projectile->m_rect.w = 10;
    projectile->m_rect.h = 5;

    Projectile_SetSpeed(projectile, &h, &v);

    to_c64_SDL_Colour(10, &projectile->colour, 255);
    
    g_app->m_projectiles.Push(projectile);

    g_app->m_sound.PlaySoundFx(SOUNDFX_LAZER_1);

    return 0;
}

int cEnemyShip::MaybeRadar() {
    int retval = 0;
    if (m_radar_wait) {
        m_radar_wait -= SDL_min(g_app->Delta(), m_radar_wait);
        if (m_radar_wait <= 0) {
            Radar();
            retval++;

            if (m_radar_wait <= 0)
                m_radar_wait = x_rand(3000, 5000);
        }
    }
    return retval;
}

void cEnemyShip::Radar() {

    Hits hits;

    SDL_Rect check_rect;
    // make many tiles are in a window width

    int dist = 640;

    check_rect.y = m_sprite.m_position_rect.y - (dist / 2);
    check_rect.h = dist;
    check_rect.w = dist;
    check_rect.x = m_sprite.m_position_rect.w / 2 + m_sprite.m_position_rect.x;
    if (!m_h_direction)
        check_rect.x -= dist;

    switch (m_state) {

    case STATE_IDLE:
    case STATE_IDLE_MOVE:
    default:
        hits.ClearHits();

        if (hits.DetectPlayerHits(&check_rect)) {
            // the player has been detected... that's change the state...
            m_state = STATE_ALERT;
            m_wait_idle = x_rand(500, 1000);


            //hits.m_player_list.

            hits.m_player_hits.m_hitArray[0].m_player->GetPosition(&m_idle_move);

            SetSpeedPerSecond(g_tile_size * 3);

            // if there are none playing play this...
            g_app->m_sound.PlaySoundFx(SOUNDFX_BEEPS);

        } else if (m_state != STATE_IDLE_MOVE) {
            m_state = STATE_IDLE;
            m_wait_idle = x_rand(100, 200);
        }
        break;
    };
#if 0
    if (Hit_DetectHits(HIT_TYPE_ALL, &check_rect, &hits, this)) {
        // there is a player near by...
        cLinkedList *ptr = hits.m_list;
        while (ptr) {
            cHit *pHit = (cHit *)ptr->m_ptr;
            switch(pHit->m_type) {
            case HIT_TYPE_PLAYER:
                // if in idle mode, go towards player...
                if (m_state == STATE_IDLE || m_state == STATE_IDLE_MOVE) {
                    m_state = STATE_ALERT;
                    m_radar_wait = x_rand(500, 1000);
                    m_wait_idle = x_rand(100, 200); // wait a little bit before following direction.
                    pHit->m_player->GetPosition(&m_idle_move);
                }
                break;
            case HIT_TYPE_PROJECTILE:
                // try to avoid projectile???
                break;
            default:
                // do nothing...
                break;
            };

            ptr = ptr->pNext;
        }
    }
#endif
}

int cEnemyShip::StepMove() {
    m_fCurrentDelta += m_speedPerFrame;

    int retval = 0;
    m_step_did_move = 0;
    if (m_fCurrentDelta >= 1.0) {

        int delta = int(m_fCurrentDelta);
        int res;
        m_fCurrentDelta -= delta;

        m_sprite.GetPosition(&m_cur_move);

        int r = 0, r2 = 0;
      //  while (delta && (r || r2)) {
        if (m_alt_step_movement) {
            if (m_idle_move.y > m_sprite.m_position_rect.y) {
                r = SDL_min(m_idle_move.y - m_sprite.m_position_rect.y, delta);
                m_cur_move.y += r;
                delta -= r;
                
            } else if (m_idle_move.y < m_sprite.m_position_rect.y) {
                r = SDL_min(m_sprite.m_position_rect.y - m_idle_move.y, delta);
                m_cur_move.y -= r;
                delta -= r;
            } else {
                r = 0;
            }

            if (r > 10) {
                SDL_Log("r (%d) > 10", r);
            }

            if (delta) {
                if (m_idle_move.x > m_sprite.m_position_rect.x) {
                    r2 = SDL_min(m_idle_move.x - m_sprite.m_position_rect.x, delta);
                    m_cur_move.x += r2;
                    delta -= r2;
                }
                else if (m_idle_move.x < m_sprite.m_position_rect.x) {
                    r2 = SDL_min(m_sprite.m_position_rect.x - m_idle_move.x, delta);
                    m_cur_move.x -= r2;
                    delta -= r2;
                }
                else {
                    r2 = 0;
                }
            }

            if (r2 > 10) {
                SDL_Log("r2 (%d) > 10", r);
            }

            m_alt_step_movement = 0;

        } else {
            // go in different order, move x before y...
            m_alt_step_movement++;

            if (m_idle_move.x > m_sprite.m_position_rect.x) {
                r2 = SDL_min(m_idle_move.x - m_sprite.m_position_rect.x, delta);
                m_cur_move.x += r2;
                delta -= r2;
            }
            else if (m_idle_move.x < m_sprite.m_position_rect.x) {
                r2 = SDL_min(m_sprite.m_position_rect.x - m_idle_move.x, delta);
                m_cur_move.x -= r2;
                delta -= r2;
            }
            else {
                r2 = 0;
            }

            if (delta) {

                if (m_idle_move.y > m_sprite.m_position_rect.y) {
                    r = SDL_min(m_idle_move.y - m_sprite.m_position_rect.y, delta);
                    m_cur_move.y += r;
                    delta -= r;
                }
                else if (m_idle_move.y < m_sprite.m_position_rect.y) {
                    r = SDL_min(m_sprite.m_position_rect.y - m_idle_move.y, delta);
                    m_cur_move.y -= r;
                    delta -= r;
                }
                else {
                    r = 0;
                }
            }

        }

        m_step_did_move += (r + r2);

        retval++;
    }
    return retval;
}

void cEnemyShip::OnIdleMove() {
    //
    // move the ship idly, making sure that the ship won't hit other objects...

    int step_res = StepMove();

    if (!step_res)
        return;

    LinkedList hits;

    if (HitDetectPoint(&m_cur_move, HIT_TYPE_TILE | HIT_TYPE_PROJECTILE)) {
         m_state = STATE_IDLE;
         m_wait_idle =  x_rand(0, 100);
    } else {
        UpdatePosition(&m_cur_move);
        
        g_app->ShouldRender();
        
        if (m_step_did_move <= 0) {
            m_state = STATE_IDLE;
            m_wait_idle = x_rand(0, 1000);
        }
    }
}

int cEnemyShip::HitDetectPositionAdj(int x, int y, int hit_type) {
    SDL_Rect cur_rect;
    cur_rect = m_sprite.m_position_rect;
    cur_rect.x += x;
    cur_rect.y += y;

    return HitDetectRect(&cur_rect, hit_type);
}

int cEnemyShip::HitDetectPoint(SDL_Point *pt, int hit_type) {
    SDL_Rect rect;
    rect.x = pt->x;
    rect.y = pt->y;
    rect.w = m_sprite.m_position_rect.w;
    rect.h = m_sprite.m_position_rect.h;

    return HitDetectRect(&rect, hit_type);
}

void cEnemyShip::SetSpeedPerSecond(float numb) {
    if (numb > 0.0)
        m_speedPerFrame = numb / g_app->m_ffps;
    else
        m_speedPerFrame = 0.0;
    m_fCurrentDelta = 0.0;
}


void cEnemyShip::OnTick() {
	// note: there is logic in cPlayer::Hit detection for handling
	// player ship colliding with enemy ship...

	// if the ship is idle... go somewhere aimlessly...
	switch (m_state) {
    case STATE_ALERT:
        MaybeShoot();
        if (m_wait_idle)
            m_wait_idle -= SDL_min(m_wait_idle, g_app->Delta());
        if (m_wait_idle <= 0) {
            m_radar_wait = 3000;
            m_state = STATE_ALERT_FOLLOW;
        }
        break;
    case STATE_ALERT_FOLLOW:
        MaybeShoot();
        MaybeRadar();
        // need to move towards player...
        if (StepMove()) {
            if (HitDetectPoint(&m_cur_move, HIT_TYPE_TILE) || !m_step_did_move) {
                // 
                m_state = STATE_IDLE;
                m_wait_idle = 500;
            }
            else {
                UpdatePosition(&m_cur_move);
            }
        } else if (!m_step_did_move) {
            // adjust to idle.
            m_state = STATE_IDLE;
            m_wait_idle = 0;
        }
        break;
    case STATE_IDLE_MOVE:
        MaybeRadar();
        OnIdleMove();
        break;
	case STATE_IDLE:
	default:
        MaybeRadar();
        if (m_wait_idle > 0) {
            m_wait_idle -= SDL_min(g_app->Delta(), m_wait_idle);
        } else {
            // go to a position some place...
            // pick a random x/y coordinate to go to...

            
            m_idle_move.x = x_rand(0, g_app->m_mapWidthPixels - m_sprite.m_position_rect.w);
            m_idle_move.y = x_rand(0, g_app->m_height);

            m_h_direction = (m_idle_move.x > m_sprite.m_position_rect.x) ? 1 : 0;

            m_state = STATE_IDLE_MOVE;

            m_radar_wait =  x_rand(500, 1000);

            SetSpeedPerSecond(g_tile_size);
        }
		break;
	};
	
}

void cEnemyShip::Render() {
    SDL_Rect rect;

	if (m_sprite.IsScreenVisible()) {
        m_sprite.m_reverse = !m_h_direction;
// SDL_Rect* GetSpriteSheetSrcRect(int sprite_type, int nth = 0);
        SDL_Rect *pRect;

        // if there is an alert lets.... do a background highlight just so we can see it...
#if 0
        rect = m_sprite.m_position_rect;

        switch (m_state) {
        case STATE_IDLE_MOVE:
            g_app->SetColour(g_c64_colours[14], 128);
            break;
        case STATE_ALERT:
            g_app->SetColour(g_c64_colours[2], 128);
            break;
        case STATE_ALERT_FOLLOW:
            g_app->SetColour(g_c64_colours[10], 128);
            break;
        case STATE_IDLE:
        default:
            g_app->SetColour(g_c64_colours[8], 128);
            break;
        };
        rect.x -= g_app->m_camera_x;
        SDL_RenderFillRect(g_app->m_renderer, &rect);
#endif

        // void RenderSprite(SDL_Rect* dest, int sprite_type, int nth, bool reverse = false);

        switch(m_state) {
        case STATE_ALERT:
            // render exclamation graphic...

            pRect = g_app->m_spritesheet.GetSpriteSheetSrcRect(SPRITESHEET_EXCLAMATION, 0);

            rect.x = m_sprite.m_position_rect.x + m_sprite.m_position_rect.w / 2;
            rect.y = m_sprite.m_position_rect.y - pRect->h;
            rect.h = pRect->h;
            rect.w = pRect->w;
            rect.w -= pRect->w / 2;

            g_app->m_spritesheet.RenderSpriteCamera(&rect, SPRITESHEET_EXCLAMATION, 0);

            break;
        default:
            break;
        };


		m_sprite.Render();

		// render the health status of the ship...
		
		int h = 5;
		rect.y = m_sprite.m_position_rect.y - h;
		rect.x = m_sprite.m_position_rect.x - g_app->m_camera_x;
		rect.w = m_sprite.m_position_rect.w;
		rect.h = h;

		SDL_SetRenderDrawColor(g_app->m_renderer, 0, 0, 0, 200);
		SDL_RenderFillRect(g_app->m_renderer, &rect);

		int w = (rect.w - 2);
		rect.y += 1;
		rect.x += 1;
		rect.w =  ( ( (float) w / (float) m_maxHealth) * (float) m_health)  ;
		rect.h -= 2;

		SDL_SetRenderDrawColor(g_app->m_renderer, 255, 0, 0, 200);
		SDL_RenderFillRect(g_app->m_renderer, &rect);


        // lets also render the desired position that the enemy ship wishes to go...
        
	}

    rect.x = m_idle_move.x - 5;
    rect.y = m_idle_move.y - 5;
    rect.w = 10;
    rect.h = 10;

    if (g_app->IsVisible(&rect)) {
        rect.x -= g_app->m_camera_x;
        SDL_SetRenderDrawColor(g_app->m_renderer, 0, 200, 0, 200);
        SDL_RenderFillRect(g_app->m_renderer, &rect);
    }
}



void cEnemyShip::Clear() {
	m_isActive = false;
}



int cEnemyShip::HitDetect(SDL_Rect *test) {
    SDL_Rect src;
    m_sprite.GetRect(&src);
    if (SDL_IntersectRect(test, &src, &m_intersectHit))
        return 1;
    return 0;
}


void cEnemyShip::Damage(int damage) {

    m_health -= SDL_min(damage, m_health);

    if (m_health <= 0 && m_isActive) {

        g_app->m_check_enemy_status++;

		// should remove the ship from array/list... add explosion sound...
		m_isActive = false;

        g_app->m_sound.PlaySoundFx(SOUNDFX_EXPLOSION_1);

        cParticle *part = Particle_New(PARTICLE_EXPLOSION);
        if (part) {
           // SDL_memset(part,0, sizeof(cParticle));
            part->m_sprite = SPRITESHEET_EXPLOSION;
            part->m_duration = 500;
            part->m_animationStepDuration = part->m_duration / g_app->m_spritesheet.g_spritesheet_map[SPRITESHEET_EXPLOSION].sz;
            part->m_animationStep = 0;
            part->m_active = 1;

            SDL_Rect rect;
            GetRect(&rect);
            part->xx = rect.x + (rect.w / 2);
            part->yy = rect.y + (rect.h / 2);

            g_app->m_particles.Unshift(part);
        }
	}
}

void cEnemyShip::GetPositionCentered(SDL_Point * pt) {
    SDL_assert(pt != NULL);

    SDL_Rect cur;
    
    GetRect(&cur);
    
    cur.x += (cur.w / 2);
    cur.y += (cur.h / 2);

    pt->x = cur.x;
    pt->y = cur.y;
}

void cEnemyShip::GetPositionCentered(SDL_Rect* rect) {
    SDL_assert(rect != NULL);

    SDL_Point pt;
    
    SDL_Rect cur;
    GetRect(&cur);

    cur.x += ((cur.w + rect->w) / 2);
    cur.y += ((cur.h + rect->h) / 2);

    rect->x = cur.x;
    rect->y = cur.y;
}
