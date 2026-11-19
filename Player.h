#pragma once
#include "framework.h"
#include "Sprite.h"
// a player has 3 'sprites'..

class cPlayer
{
public:
	cPlayer();
	~cPlayer();

    // used with direction/move...
    enum {
        // this is arranged so that RIGHT - LEFT would become RIGHT,
        // RIGHT - UP = DOWN... etc...
        DIR_LEFT = 0,
        DIR_UP,
        DIR_DOWN,
        DIR_RIGHT,

        DIR_MAX
    };

    // indicates if active...
    void Init();

    void MoveLeft(int unset = 0) {
        Move(DIR_LEFT, unset);
    }
    void MoveRight(int unset = 0) {
        Move(DIR_RIGHT, unset);
    }
    void MoveUp(int unset = 0) {
        Move(DIR_UP, unset);
    }
    void MoveDown(int unset = 0) {
        Move(DIR_DOWN, unset);
    }

    void FireDown();
    void FireRelease();
    void AltFireDown();
    void AltFireRelease();
        
    void SetActive(bool active);
    void SetVisible(bool show);
    
    bool IsReverse();

    // would the sprite appear on the screen if rendererd.
    bool IsVisible();

    // render (to g_app->m_rendererer)
    void Render();

	// the position and size of the player.
    void SetReverse(bool reverse);

    void SetPosition(SDL_Point *pt);
    void SetPosition(int x, int y);

    // type - LEFT... DOWN
    // this will be what way the plane will be going...
    void Move(int type, int unset);

    void Fire();    // fire
    void AltFire(); // alternative fire.

    void Damage(int damage);

    // adjust the movement in this area...
 //   void AdjustMove(int type, int amt = 1);

    // this will indicate which way the user wants to plane to go...
   // void VelocityMove(int type, int enable);

    // update propeller, pilot positions based on reverse,
    // and offsets...
    void UpdatePositions();

    // on tick/delta...
    void OnTick();

    void CheckTileCollision();
    void CheckEnemyCollision();
    void CheckProjectileCollision();

    // will return the plane sprite...
    SDL_Rect *GetPosition(SDL_Point *dest);

    // will return the outer box layout of the sprite(s).
    void GetRect(SDL_Rect *rect);





    // the plane, propelloer and pilot sprites
    cSprite m_planeSprite;
    cSprite m_propellerSprite;
    int m_propellerOffsetIx;
    cSprite m_pilotSprite;
    int m_pilotOffsetIx;

    int m_propellerDelta;

    // currnet moving direction... horizontal/vertical..
    int m_moveDir[4];
    int m_hDir, m_vDir;

    int m_fire, m_alt_fire;
    Uint32 m_fire_timestamp, m_alt_fire_timestmap;

    // health of player... upto 100 ??? not sure.
    int m_health;
    int m_showHealth;

    int m_move_accumulator;

    bool m_reverse;
    bool m_visible;
    bool m_active;
};

