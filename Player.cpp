#include "Player.h"
#include "spritesheet.h"
#include "common.h"
#include "App.h"
#include "Projectile.h"
#include "Hit.h"
#include "Particle.h"

cPlayer::cPlayer() {
    m_active = false;

    m_propellerDelta = 0;
    // this needs to be delayed (to Init()) so that 
    // the cSpritesheet has a chance to initialize first (upon its constructor).

    //m_hDir = m_vDir = 0;
    SDL_memset(m_moveDir, 0 ,sizeof(m_moveDir));
  //  SDL_memset(m_velocityLastTicks, 0, sizeof(m_velocityLastTicks));

    m_reverse = false;

    m_move_accumulator = 0;

    m_vDir = 0;
    m_hDir = 0;



}

cPlayer::~cPlayer() {

}

void cPlayer::Init() {
    m_planeSprite.SetType(SPRITESHEET_PLANE);
    m_propellerSprite.SetType(SPRITESHEET_PROPELLER);
    m_pilotSprite.SetType(SPRITESHEET_PILOT);

    m_propellerSprite.SetSpriteOffsetType(SPRITESHEET_PLANE);
    m_propellerSprite.SetNth(0);
    m_pilotSprite.SetSpriteOffsetType(SPRITESHEET_PLANE);
    m_pilotSprite.SetNth(0);

    m_vDir = 0;
    m_hDir = 0;

    m_health = 100;
    m_showHealth = 0;


    m_fire = 0;
    m_alt_fire = 0;
    m_fire_timestamp = 0;
    m_alt_fire = 0;
}

void cPlayer::SetActive(bool active) {
    // will add/remove sprites to rendering list...
    m_active = active;
}

void cPlayer::SetVisible(bool show) {
    // has not effect...
}

void cPlayer::Move(int type, int unset) {

    if (unset) {
        m_moveDir[type] = 0;
    } else {
        int offDir = 3 - type;
        if (type == DIR_UP || type == DIR_DOWN) {
            if (m_moveDir[offDir])
                m_moveDir[offDir] = 0;
            else if (m_moveDir[type] < 3)
                m_moveDir[type]++;
        } else {
            if (m_moveDir[type] < 3) {
                m_moveDir[type]++;
                m_moveDir[offDir] = 0;
            }
        }

    }
    switch(type) {
    case DIR_LEFT:
        m_hDir = -m_moveDir[type];
        SetReverse(true);
        break;
    case DIR_RIGHT:
        m_hDir = m_moveDir[type];
        SetReverse(false);
        break;
    case DIR_UP:
        m_vDir = -m_moveDir[type];
        break;
    case DIR_DOWN:
        m_vDir = m_moveDir[type];
        break;
    default:
        break;
    };
}

void cPlayer::OnTick() {
    // adjust the propeller...

    Uint32 curticks, velticks, velticksdelta;

    velticksdelta = 100;

    curticks = g_app->GetEventTimestamp();

    velticks = curticks - velticksdelta;

    int z = 0, x, y;

    int delta;
    delta = g_app->Delta();

    m_propellerDelta += delta;//g_app->Delta();
    if (m_propellerDelta > 50) {
        int nth = m_propellerDelta / 50;

        m_propellerDelta -= (nth * 50);

        nth += m_propellerSprite.m_nth;
        nth = nth % 2;
        m_propellerSprite.SetNth(nth);

        z++;
    }

    int adj_pos = 0;//delta / 100;
    int min = 10;

    //

    m_move_accumulator += delta;

    if (m_move_accumulator >= min) {
        adj_pos = m_move_accumulator / min;
        m_move_accumulator -= (adj_pos * min);

      //  adj_pos = 1;

        int x, y;
        x=m_planeSprite.m_position_rect.x;
        y=m_planeSprite.m_position_rect.y;


        if (m_vDir != 0) {
            y += m_vDir;
            if (y < 0)
                y = 0;
            else if (y > g_app->m_canvas_rect.h)
                y = g_app->m_canvas_rect.h;
        }
        if (m_hDir != 0) {
            x += m_hDir;
            if (x < 0)
                x = 0;
            else if (x > g_app->m_mapWidthPixels)
                x = g_app->m_mapWidthPixels;
        }

        if (m_vDir != 0 || m_hDir != 0) {
            // will have to check for collistions with ground and if the plane is going out of bounds...
            SetPosition(x, y);

            z++;
        }
    }

    if (z) {
        // update positions...
        UpdatePositions();

        CheckTileCollision();

        CheckEnemyCollision();

        g_app->m_should_render++;
    }


    if (m_fire) {
        Fire();
        m_fire = 0; // turned this off here incase the window loses keyboard focus.
    }
    if (m_alt_fire) {
        AltFire();
        m_alt_fire = 0;
    }



    if (m_showHealth < m_health)
        m_showHealth++;
    else if (m_showHealth > m_health)
        m_showHealth--;
}

void cPlayer::CheckTileCollision() {
    SDL_Rect rect;

    GetRect(&rect);

    // see if this collides with tiles...
    if (g_app->m_tileMap.HitDetect(&rect)) {
        // a hit has been detected... see where and adjust the plane...
        // actually want to adjust where the y2 is...
        int hits = 0;
        //int y2 = m_planeSprite.m_position_rect.y + m_planeSprite.m_position_rect.h;

        int ty2, dy2;
        dy2 = rect.y + rect.y;
        
        for (int x = 0; x < g_app->m_tileMap.Size(); x++) {
            // adjust the y1...
            SDL_Rect tileRect;
            cTile *pTile = g_app->m_tileMap.GetHitTile(x);
            if (!pTile)
                break;

            Tile_GetRect(pTile, &tileRect);

            ty2 = tileRect.y;// +tileRect.h;
            if (ty2 < dy2)
                dy2 = (ty2 - g_tile_size);
            /*

            if (SDL_IntersectRect(&rect, &tileRect, 0)) {
                // intersected... lets adjust the plane...
                hits++;

                m_planeSprite.m_position_rect.y -= g_tile_size;
            }*/
        }

        int diff;
        diff = (rect.y + rect.h) - dy2;

        m_planeSprite.m_position_rect.y -= diff;

        UpdatePositions();

        // play a sound...
        g_app->m_sound.PlaySoundFx(SOUNDFX_HIT_1);

        Damage(5);
        
    }


}



void cPlayer::CheckEnemyCollision() {
    SDL_Rect rect;
    GetRect(&rect);

    LinkedList list;

    Hit_DetectHits(HIT_TYPE_ENEMY_SHIP, &rect, &list, this);

    cLinkedList* it;
    for (it = list.m_list; it != NULL; it = it->pNext) {
        cHit* pHit = (cHit *)it->m_ptr;
        
        // has hit the enemy ship and player... what to do???
        // should the player/ship bounce off each other???

        Damage(10);

        // should we affect the enemy ship here ??
        pHit->m_ship->Damage(10);

    }

}

void cPlayer::Damage(int damage) {
    if (m_health) {
        m_health -= SDL_min(damage, m_health);
        if (m_health <= 0) {

            m_health = 0;

           g_app->m_sound.PlaySoundFx(SOUNDFX_EXPLOSION_1);
           
           g_app->PlayerDies(this);

           m_health = 0;
                // need to remove player
           m_active = 0;

            for (int x = 0; x < 10; x++) {

                cParticle* part = Particle_New(PARTICLE_EXPLOSION);
                if (part) {
                    // SDL_memset(part,0, sizeof(cParticle));
                    part->m_sprite = SPRITESHEET_EXPLOSION;
                    part->m_duration = 500;
                    part->m_animationStepDuration = part->m_duration / g_app->m_spritesheet.g_spritesheet_map[SPRITESHEET_EXPLOSION].sz;
                    part->m_animationStep = 0;
                    part->m_active = 1;
                    part->m_intial_delay = x_rand(0, 100);

                    SDL_Rect rect;
                    GetRect(&rect);
                    part->xx = rect.x + (rect.w / 2);
                    part->yy = rect.y + (rect.h / 2);

                    part->xx += (x_rand(0, rect.w * 2) - (rect.w / 2));
                    part->yy += (x_rand(0, rect.h * 2) - (rect.h / 2));

                    g_app->m_particles.Unshift(part);
                }
            }
        }
    }
}

void cPlayer::CheckProjectileCollision() {

}

bool cPlayer::IsVisible() {
    SDL_Rect scrn_rect;
    g_app->GetCameraRect(&scrn_rect);
    if (m_pilotSprite.IsVisible(&scrn_rect))
        return true;
    if (m_planeSprite.IsVisible(&scrn_rect))
        return true;
    if (m_propellerSprite.IsVisible(&scrn_rect))
        return true;
    return false;
}

// render (to g_app->m_rendererer)
void cPlayer::Render() {

    // see if this will display if renderered...
    SDL_Rect scrn_rect;
    g_app->GetCameraRect(&scrn_rect);
    if (m_pilotSprite.IsVisible(&scrn_rect))
        m_pilotSprite.Render();
    if (m_planeSprite.IsVisible(&scrn_rect))
        m_planeSprite.Render();
    if (m_propellerSprite.IsVisible(&scrn_rect))
        m_propellerSprite.Render();

    SDL_Rect out;
    GetRect(&out);
    SDL_SetRenderDrawColor(g_app->m_renderer, 128, 64, 0, 128);
    out.x -= scrn_rect.x;
    SDL_RenderDrawRect(g_app->m_renderer, &out);
}

// the position and size of the player.
void cPlayer::SetReverse(bool reverse) {
    m_reverse = reverse;

    m_planeSprite.m_reverse = reverse;
    m_pilotSprite.m_reverse = reverse;
    m_propellerSprite.m_reverse = reverse;

    UpdatePositions();
}

bool cPlayer::IsReverse() {
    if (m_moveDir[cPlayer::DIR_LEFT] > m_moveDir[cPlayer::DIR_RIGHT])
        return true;
    return false;
}

void cPlayer::SetPosition(SDL_Point* pt) {

    m_planeSprite.SetPosition(pt);

    // adjust if reverse...
 //   if (m_reverse) {
   //     m_planeSprite.m_position_rect.x -= m_planeSprite.m_position_rect.w;
    //}

    UpdatePositions();
}

void cPlayer::SetPosition(int x, int y) {
    SDL_Point pt = { x ,y };
    SetPosition(&pt);
}

void cPlayer::GetRect(SDL_Rect *dest) {
    SDL_memcpy(dest, &m_planeSprite.m_position_rect, sizeof(SDL_Rect));
    ExpandOuter_Rect(dest, &m_propellerSprite.m_position_rect);
    ExpandOuter_Rect(dest, &m_pilotSprite.m_position_rect);
}

// update the 'sub' sprite positions...
void cPlayer::UpdatePositions() {
    int pilotOffX, pilotOffY, propOffX, propOffY;
    int planeW;

    pilotOffX = m_pilotSprite.OffX();
    propOffX = m_propellerSprite.OffX();

    if (m_reverse) {
        int pilotW, propW;
        pilotW = m_pilotSprite.Width();
        propW = m_propellerSprite.Width();
        //SDL_Rect sz;

        //m_planeSprite.GetRect(&sz);
        planeW = m_planeSprite.Width();

        pilotOffX = planeW - (pilotOffX + pilotW);

        propOffX = planeW - (propOffX + propW);
    }

    pilotOffY = m_pilotSprite.OffY();
    propOffY = m_propellerSprite.OffY();

    m_pilotSprite.SetPosition(
                m_planeSprite.m_position_rect.x + pilotOffX,
                m_planeSprite.m_position_rect.y + pilotOffY
    );
    m_propellerSprite.SetPosition(
                m_planeSprite.m_position_rect.x + propOffX,
                m_planeSprite.m_position_rect.y + propOffY
    );
}

// will return the plane sprite...
SDL_Rect *cPlayer::GetPosition(SDL_Point *dest) {
    SDL_Rect *retval = &m_planeSprite.m_position_rect;
    if (dest) {
        dest->x = retval->x;
        dest->y = retval->y;
    }
    return retval;
}

void cPlayer::FireDown() {
    if (m_fire++ == 0) {
        Fire();
    }
}

void cPlayer::FireRelease() {
    m_fire = 0;
}

void cPlayer::AltFireDown() {
    if (m_alt_fire++ == 0) {
        AltFire();
    }
}

void cPlayer::AltFireRelease() {
    m_alt_fire = 0;
}

void cPlayer::Fire() {
    // trigger a projectile to fire...
    Uint32 can_fire = m_fire_timestamp + 200;
    Uint32 event_ts = g_app->GetEventTimestamp();
    if (event_ts >= can_fire) {

        cProjectile* bullet = Projectile_New();
        bullet->type = PROJECTILE_BULLET;
        bullet->gravity = 0;

        Projectile_SetSrc(bullet, this, PROJECTILE_SRC_PLAYER);
        
        bullet->hit_damage = 10;
        bullet->duration = 1000;    // 

        SDL_Rect rect;
        GetRect(&rect);

        bullet->m_rect.x = rect.x + rect.w;
        bullet->m_rect.y = rect.y + rect.h / 2;
        g_app->m_spritesheet.GetSpriteSheetSz(&bullet->m_rect, SPRITESHEET_BULLET, 0);

        SDL_Log("Player: %d,%d;", rect.x, rect.y);
        SDL_Log("Bullet: %d, %d;", bullet->m_rect.x, bullet->m_rect.y);
        SDL_Log("Camera: %d, %d", g_app->m_camera_x, g_app->m_camera_y);
            
        //bullet->m_rect.w = 5;
        //bullet->m_rect.h = 5;

       // bullet->velocity_h = 800 / WIN_FPS;


        float d;
        d = 800.0;


        if (IsReverse()) {
            // pointing left
            bullet->m_rect.x -= (rect.w + 5);
            //bullet->velocity_h = 0-bullet->velocity_h;
            d = -d;
        }

        m_fire_timestamp = event_ts;

        Projectile_SetSpeed(bullet, &d, 0);

        // push the projectile.
        g_app->m_projectiles.Push(bullet);

        g_app->m_sound.PlaySoundFx(SOUNDFX_SHOOT_1);

    }
}

void cPlayer::AltFire() {

}

