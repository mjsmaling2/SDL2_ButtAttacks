#ifndef APP_H
#define APP_H

#include "framework.h"

#include "Framerate.h"
#include "TileArray.h"
#include "spritesheet.h"
#include "string.h"
#include "Player.h"
#include "ptrarray.h"
#include "EnemyShip.h"
#include "Sound.h"
#include "Label.h"

#include "LinkedList.h"


// this is the 'canvas' width/height... 
#define WIN_DEFAULT_WIDTH 800
#define WIN_DEFAULT_HEIGHT 600

#define WIN_FPS 50
#define MAX_PLAYERS 4

class cApp
{
public:
	cApp();
	virtual ~cApp();

	enum {
		NO_STATE = 0,
		START_SCREEN,
		PLAYING,
		GAME_OVER
	};

	bool InitApp();
	void Run();
	void CleanUp();

	SDL_Renderer *m_renderer;
    SDL_Window *m_window;

	int m_state;	// current state...

	int m_should_render;

	// current event
	SDL_Event m_event;

	cFramerate m_frames;

	SDL_Rect m_playInfoRects[MAX_PLAYERS];
    cPlayer m_players[MAX_PLAYERS];  // will eventually be 4

	int m_playersUsed;

    void ShouldQuit(const char *msg = 0);

	void OnFocusLost();

#if 0
	bool AllocateEnemyShips(int sz, int clear);
	cEnemyShip *m_enemyShips;
	int m_enemyShipsUsed;
	int m_enemyShipsAllocated;
#endif
	

	cLabel* FindLabel(int ID, bool create = false);
    cLabel *RenderLabel(int ID, const char *text, int x1, int y1, int sz, SDL_Colour *fg);
    cLabel *RenderLabel(const char *text, int x1, int y1, int sz, SDL_Colour *fg);

    void RemoveLabel (cLabel *ptr) {
        m_labels.Remove(ptr);
    }

    // remove label by id... remove_all - should really only be one instance
    int RemoveLabel(int id, bool remove_all = false);

	SDL_Colour sky_colour_1, sky_colour_2;

	void GetCameraRect(SDL_Rect* dest);
	void GetCameraRect(SDL_FRect* dest);

	int Delta() { return m_frames.m_delta; };

    void ShouldRender() {m_should_render++;};

	// shortcut: returns the current event timestamp...
	Uint32 GetEventTimestamp() { return m_event.common.timestamp; };

	// set to main renderer colour
	void SetColour(SDL_Colour* src) {
		SDL_SetRenderDrawColor(m_renderer, src->r, src->g, src->b, src->a);
    }
	void SetColour(Uint32 rgb, Uint8 a = 255);

	// is visible based upon the camera x/y coordinates...
	int IsVisible(const SDL_Rect* rect);

	

	//
	void StartGame();
	
//	void PlayerDies(int ix);
//	void GameOver();

	void StartLevel();

	// generate map
	void GenerateMap();

	void PlayerDies(int ix);
	void PlayerDies(cPlayer* ptr);
	void GameOver();

	// basically to animate stuff... check some things after processing main events...
	void OnTick();
	void OnProjectilesTick();

	void UpdateTileRatios();

	// update gui positions, ie: text/labels.
	void UpdateGuiPositions();

	const char* AssetsFile(const char* fn, cString *dest = 0);

	// tile map...
	cTileArray m_tileMap;

    cSpritesheet m_spritesheet;

	cSound m_sound;

    // current list of sprites to render...
    cSprite **m_pVisibleSprites;    // visible sprites...
    int m_allocated;
    int m_used;

	// currnet level 
	int m_current_level;

	int m_tilesWinCols;	// how many columns fit on a window.
	int m_tilesWinRows;	// how many rows fit in the window.

	int m_mapLength;		// the column width (not pixels, but columns)...
    int m_mapWidthPixels; // the width of the map in pixels...

	int m_camera_x, m_camera_y;	// camera offset... the width/height will be the window ww/hh...

	cString m_tempString;

	SDL_Rect m_radarRect;
	
    int m_width, m_height;      // window width/height...

	int m_oldWidth, m_oldHeight;
	SDL_Rect m_canvas_rect;
	
	//
	// update these later perhaps... to an array would be better.
	// but these are okay for now.
	//
    LinkedList m_projectiles;
    LinkedList m_labels;
    LinkedList m_enemyShips;
	LinkedList m_particles;

	cArray m_creditLabels;

    // remove in active items flag...
    int m_cleanUpFlag;

	float m_ffps;	// float fps
	int m_ifps;		// int fps...


    int m_game_over_label_id;
    int m_game_start_label_id_temp;
    int m_label_id_counter;

    int m_check_enemy_status;  // an enemy ship has been destroyed... check may need to advance level...

    Uint64 m_temp_labels_remove_timeout;

	// show next credits...
	Uint64 m_show_credits_timeout, m_show_credits_fade;
	int m_credits_label_ix;
	int m_credits_label_id;
protected:
	void RenderTiles();
    void RenderSprite(cSprite *src);

	// render the bottom radar...
	void RenderRadar();

	// ie player name health...
	void RenderPlayerInfo();



    int m_should_quit;  // want to track when this is changed ( for debugging/development)
private:
	int m_initialized;
};

#endif // APP_H
