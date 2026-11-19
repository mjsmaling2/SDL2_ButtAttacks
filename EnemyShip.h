#pragma once

#include "framework.h"
#include "Sprite.h"
#include "common.h"
#include "App.h"
#include "LinkedList.h"
#include "Hit.h"

class cEnemyShip /* : public cSprite*/ {
public:
	cEnemyShip();
	virtual ~cEnemyShip();

	enum {
		STATE_IDLE = 0,
		STATE_IDLE_MOVE,


		STATE_ALERT,	// been alerted by player.
        STATE_ALERT_FOLLOW, // following towards player...

		STATE_MAX
	};

	void OnTick();
	
    void OnIdleMove();
    void OnAlertMove();

    // check for where player (and other things) is and maybe adjust state/status...
    void Radar();
    int MaybeRadar();

	void MaybeShoot();
	int Shoot();

    // moves towards idle position... can be used for any
    // does not check for collisions...
    int StepMove();


	void UpdatePosition(SDL_Point* pt) {
		m_sprite.GetPosition(&m_last_position);
		m_sprite.SetPosition(pt);
	}

	void SetPosition(int x, int y) {
		m_sprite.SetPosition(x, y);
	}

	void GetPosition(SDL_Point* pt) {
		pt->x = m_sprite.m_position_rect.x;
		pt->y = m_sprite.m_position_rect.y;
	}

	//void GetPositionCentered(SDL_Point* pt);
	void GetPositionCentered(SDL_Point * pt);

	// get position (centered)... note rect->w rect->h will be used to center
	// point also...
	void GetPositionCentered(SDL_Rect* rect);

    void GetRect(SDL_Rect *dest) {
        m_sprite.GetRect(dest);
    }

    void SetSpeedPerSecond(float numb);

	// the ship takes on damage, ie: a player's ship has crashed into this...
	void Damage(int damage);

	// will test if 'test' will hit the sprite.
	int HitDetect(SDL_Rect* test);

	// will determine if the ship will hit something else if adjusting for +x +y position...
	int HitDetectPositionAdj(int x, int y, int hit_type = HIT_TYPE_ALL);
    int HitDetectPoint(SDL_Point *pt, int hit_type = HIT_TYPE_ALL);

	int HitDetectRect(SDL_Rect* rect, int hit_type /*= HIT_TYPE_ALL*/) {
		return m_hits.DetectHits(hit_type, rect);
	}

	void Render();
        
	// will reset/clear a few variables.
	void Clear();

	// if active ship, or inactive...
	bool m_isActive;

	int m_health;
	int m_maxHealth;

	int m_state;
	Uint32 m_wait_idle;
    Uint32 m_radar_wait;    // wait certain amount of time before checking radar again.

	Uint32 m_wait_shoot;


    SDL_Point m_idle_move;  // where the ship is heading to... may not be from an idle, could be the player position.
    int m_h_direction;  // horizontal direction . 1 - right, 0 - left....

    SDL_Point m_last_position;

    SDL_Point m_cur_move;   // where the position will be after a 'step'
	int m_step_did_move;
	cSprite m_sprite;

    float m_speedPerFrame; // speed per second.
	float m_fCurrentDelta;
    SDL_Rect m_intersectHit;

	
	// these should be used temporarily with hit detect routined (ie: dont' rely upon the pointers
	// in the list to remain valid forever )
    Hits m_hits;

	int m_alt_step_movement;

};

extern void EnemyShip_Delete(void* ptr);
extern void EnemyShip_OnTick(void* ptr);
extern int EnemyShip_IsActive(void *ptr);
extern int EnemyShip_IsInActive(void *ptr);
