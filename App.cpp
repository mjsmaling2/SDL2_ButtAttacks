#include "App.h"
#include "Framerate.h"
#include "common.h"
#include "Player.h"
#include "ptrarray.h"
#include "Font.h"
#include "Projectile.h"
#include "Particle.h"

static const char* app_credits[] = {
	"Butt Attacks v0.5 - Beta : Nov 2026",
	"Usage: Arrow Keys movement",
	"Usage: Space shoot",
	"Usage: Space Re-Start game",
	"By M Smaling",
	"Sound Effects: Retro Games",
	"Usage: SDL2",
	0
};


/*  LinkedList m_projectiles;
    LinkedList m_labels;
    LinkedList m_enemyShips;*/
cApp::cApp()
{
	g_app = this;

	m_renderer = 0;
	m_window = 0;

	m_initialized = 0;

	m_enemyShips = 0;
	
	m_labels = NULL;

    m_enemyShips.m_free_callback = EnemyShip_Delete;
    m_labels.m_free_callback = label_free;
    m_particles.m_free_callback = Particle_Free;
    m_projectiles.m_free_callback = Projectile_Free;

    m_label_id_counter = 1;

    m_game_over_label_id = m_label_id_counter++;
    m_game_start_label_id_temp = m_label_id_counter++;

    m_temp_labels_remove_timeout = 0;

	m_ffps = (float)WIN_FPS;
	m_ifps = WIN_FPS;
}

cApp::~cApp()
{
	g_app = 0;
}


bool cApp::InitApp() {

	int x, y, z;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS)) {
		SDL_Log("SDL Init Error: %s", SDL_GetError());
		return false;
	}

	m_initialized += 1;

	if (SDL_CreateWindowAndRenderer(WIN_DEFAULT_WIDTH, WIN_DEFAULT_HEIGHT,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
			&m_window, &m_renderer)) {
		SDL_Log("Could not create window and renderer\n%s", SDL_GetError());
		return false;
	}

	SDL_GetWindowSize(m_window, &m_width, &m_height);

	m_oldWidth = m_width;
	m_oldHeight = m_height;

	SDL_SetWindowTitle(m_window, "SDL-ButtAttacks");

	if (TTF_Init()) {
		SDL_Log("Could not initialize TTF\n%s", SDL_GetError());
		return false;
	}

	init_font();

	m_initialized += 2;

	int r = IMG_INIT_PNG | IMG_INIT_JPG;
	if (IMG_Init(r) != r) {
		SDL_Log("Could not initialize IMG with flags (%d)\n%s", r, SDL_GetError());
		return false;
	}

	m_initialized += 4;

    // initialize spritesheet.
    if (!m_spritesheet.Init()) {
        SDL_Log("Could not initiazlize spritesheet.");
        return false;
    }

	if (!m_sound.Init()) {
		SDL_Log("Could not initialize sound.");
		return false;
	}

	m_initialized += 8;

	m_should_quit = 0;

	m_should_render = 0;

	g_tile_size = m_tileMap.m_tileSize = 24;	// 24 pixels seems to be okay...

	m_camera_x = 0;
	m_camera_y = 0;

	// set tile colours;
	to_c64_SDL_Colour(11, &Tile_Colours[TILE_BEDROCK]);	// bedrock
	to_c64_SDL_Colour(9, &Tile_Colours[TILE_DIRT]);	// dirt
	to_c64_SDL_Colour(5, &Tile_Colours[TILE_GRASS]);	// grass

	m_players[0].Init();
	m_players[0].m_active = true;

	UpdateTileRatios();

	char buffer[20];
	for (x = 0; x < MAX_PLAYERS; x++) {

		SDL_snprintf(buffer, sizeof(buffer), "Player %d", (x + 1));

		SDL_Log("Buffer %s", buffer);

		cLabel* pLabel = label_new();
		if (pLabel) {
			to_c64_SDL_Colour(1, &pLabel->fg_colour);
			pLabel->px_sz = 10;
			label_set_text(pLabel, buffer);
			//pLabel->x = m_playInfoRects[x].x + 1;
			//pLabel->y = m_playInfoRects[x].y + 1;

            m_labels.Push(pLabel);
		}
	}
	
	UpdateGuiPositions();

	m_state = cApp::PLAYING;

	return true;
}

void cApp::UpdateGuiPositions() {
	float avgW = (float)m_width / (float)MAX_PLAYERS;
	int padding = 2, x;

	for (x = 0; x < MAX_PLAYERS; x++) {
		m_playInfoRects[x].w = avgW - (padding * 3);
		m_playInfoRects[x].h = 20;
		m_playInfoRects[x].y = padding;
		m_playInfoRects[x].x = (avgW - m_playInfoRects[x].w) / 2 + x * avgW;
	}

	x = 0;
    cLinkedList* p = m_labels.m_list;
	while (p) {
		cLabel* pLabel = (cLabel *)p->m_ptr;
		pLabel->x = m_playInfoRects[x].x + 1;
		pLabel->y = m_playInfoRects[x].y + 1;

		x++;
		p = p->pNext;
	}

}

void cApp::UpdateTileRatios() {

	//int m_width, m_height;

	g_canvas_width = m_canvas_rect.w = m_width;
	g_canvas_height = m_canvas_rect.h = m_height;
	m_canvas_rect.y = 0;	// would like this centered.
	m_canvas_rect.x = 0;

	m_tilesWinCols = g_canvas_height / g_tile_size;	// how many columns fit on a window.
	m_tilesWinRows = g_canvas_width / g_tile_size;

	m_radarRect.w = m_width / 2;
	m_radarRect.h = m_height / 10;
	m_radarRect.x = (m_width - m_radarRect.w) / 2;
	m_radarRect.y = m_height - (m_radarRect.h + 10);
}

void cApp::Run() {
    int x, y, z;

	m_frames.Start(WIN_FPS);
	m_should_render++;

	to_SDL_Colour((Uint32) 0x483aaa, &sky_colour_1);
	to_SDL_Colour((Uint32) 0x84c5cc, &sky_colour_2);

	//SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	StartGame();

	m_frames.Delay();	// just do an initial delay... to init the delta variables, etc...

	StartLevel();

    m_cleanUpFlag = 0;

	z = 0;

loop_here:
	m_frames.Delay();
	
	while (SDL_PollEvent(&m_event)) {
		switch(m_event.type) {
		case SDL_QUIT:
            SDL_Log("SDL_QUIT event occured.");
            ShouldQuit();
			break;
        case SDL_KEYDOWN:
            switch(m_event.key.keysym.sym) {
            case SDLK_w:
			case SDLK_UP:
                m_players[0].MoveUp();
                break;
            case SDLK_a:
			case SDLK_LEFT:
                m_players[0].MoveLeft();
                break;
            case SDLK_s:
			case SDLK_DOWN:
                m_players[0].MoveDown();
                break;
            case SDLK_d:
			case SDLK_RIGHT:
                m_players[0].MoveRight();
                break;
            case SDLK_ESCAPE:
                SDL_Log("Escape key pressed.");
                ShouldQuit();
                break;
			case SDLK_SPACE:
                if (m_state == GAME_OVER) {
                    // game over ... restart
                    StartGame();
                    StartLevel();
                } else {
                    m_players[0].FireDown();
                }
				break;
            default:
                break;
            };
            break;
        case SDL_KEYUP:
            switch(m_event.key.keysym.sym) {
            case SDLK_w:
			case SDLK_UP:
         //       m_players[0].VelocityMove(cPlayer::DIR_UP, 0);
                break;
            case SDLK_a:
			case SDLK_LEFT:
         //       m_players[0].VelocityMove(cPlayer::DIR_LEFT, 0);
                break;
            case SDLK_s:
			case SDLK_DOWN:
         //       m_players[0].VelocityMove(cPlayer::DIR_DOWN, 0);
                break;
            case SDLK_d:
			case SDLK_RIGHT:
        //        m_players[0].VelocityMove(cPlayer::DIR_RIGHT, 0);
                break;
            default:
                break;
            };
            break;
		case SDL_WINDOWEVENT:
			switch (m_event.window.event) {
			case SDL_WINDOWEVENT_FOCUS_LOST:
				// go through players and relase fires...
				OnFocusLost();
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				m_should_render++;
				break;
			case SDL_WINDOWEVENT_RESIZED:
				m_should_render++;
				z++;
				break;
			default:
				break;
			};	// switch

			break;
		};	// switch
	}

	if (m_should_quit <= 0) {

		if (z && (--z == 0)) {
			m_oldWidth = m_width;
			m_oldHeight = m_height;

			SDL_GetWindowSize(m_window, &m_width, &m_height);

			// adjust the player/enemy/projectile positions...
			SDL_Point pt;
			int diff_h, diff_w;
			diff_h = m_height - m_oldHeight;
			diff_w = m_width - m_oldWidth;

			for (x = 0; x < MAX_PLAYERS; x++) {
				if (m_players[x].m_active) {
					
					m_players[x].GetPosition(&pt);

					pt.y += diff_h;

					
					m_players[x].SetPosition(
						pt.x,
						pt.y
					);
				}
			}

			cEnemyShip* ship;
			cLinkedList* i;
            for (i = m_enemyShips.m_list; i != NULL; i = i->pNext) {
				ship = (cEnemyShip*)i->m_ptr;
				if (ship->m_isActive) {
					ship->GetPosition(&pt);
					pt.y += diff_h;
					ship->SetPosition(pt.x, pt.y);
				}
			}
			
			UpdateTileRatios();
			UpdateGuiPositions();
		}

		// on tick...
		OnTick();

		if (m_should_quit <= 0 && m_should_render > 0) {
			// render something...

            // test the camera...
            int desired_x;

            SDL_Point playerPt;
            m_players[0].GetPosition(&playerPt);
            desired_x = playerPt.x;
            if (m_players[0].m_moveDir[cPlayer::DIR_RIGHT]) {
                desired_x -= (g_canvas_width * 0.2);
            } else if (m_players[0].m_moveDir[cPlayer::DIR_LEFT]) {
                desired_x -= (g_canvas_width * 0.8);
            }

            if (desired_x < 0)
                desired_x = 0;
            else if (desired_x > m_mapWidthPixels - g_canvas_width)
                desired_x = m_mapWidthPixels- g_canvas_width;

            if (desired_x != m_camera_x) {
                int diff = abs(m_camera_x - desired_x);
                if (diff > 50)
                    diff = 50;
                else if (diff > m_tileMap.m_tileSize)
                    diff = diff / 2 + 1;
                if (m_camera_x > desired_x)
                    m_camera_x -= diff;
                else if (m_camera_x < desired_x)
                    m_camera_x += diff;

                if (m_camera_x < 0)
                    m_camera_x = 0;
                else if (m_camera_x > (m_mapWidthPixels - g_canvas_width))
                    m_camera_x = (m_mapWidthPixels - g_canvas_width);
            }

            SDL_Rect camera_rect, result_rect;
            GetCameraRect(&camera_rect);

			// render sky...

			SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
			SDL_RenderClear(m_renderer);
			SDL_Rect bottom_half;
			bottom_half.w = g_canvas_width;
			bottom_half.h = g_canvas_height >> 1;

			bottom_half.y = 0;

			bottom_half.x = 0;

			SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);

			SetColour(g_c64_colours[14]);
			
			SDL_RenderFillRect(m_renderer, &bottom_half);
			bottom_half.y = bottom_half.h;

			SetColour(g_c64_colours[6]);

			SDL_RenderFillRect(m_renderer, &bottom_half);
			
			// render the tiles...
			RenderTiles();

			SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

			cLinkedList* i;
            for (i = m_enemyShips.m_list; i != NULL; i = i->pNext) {
				cEnemyShip* ship = (cEnemyShip*)i->m_ptr;
				if (ship && ship->m_isActive)
					ship->Render();
			}
            
            // render the players...
            for (x = 0; x < 4; x++) {
				if (m_players[x].m_active) {
					// render the sprite...
					m_players[x].Render();
				}
            }

			// render the projectiles...
			if (m_projectiles.m_list)
				m_projectiles.Map(Projectile_Render);
			
            // render particles here...
            for (i = m_particles.m_list; i != NULL; i=i->pNext) {
                cParticle *part = (cParticle *)i->m_ptr;
                if (part->m_active) {
                    SDL_Rect spriterect;

                    // render this...
                    switch(part->m_type) {
                    case PARTICLE_EXPLOSION:
                        // render the sprite...

                        if (Particle_GetSpriteRect(part, &spriterect)) {
                            Debug_Printf("part\t", &spriterect);
                            Debug_Printf("camera\t", &camera_rect);
                            if (SDL_IntersectRect(&camera_rect, &spriterect, &result_rect)) {
                                spriterect.x -= m_camera_x;
                                m_spritesheet.RenderSprite(&spriterect, part->m_sprite, part->m_animationStep);
                            }
                        }

                        break;
                    };

                }
            }

			RenderRadar();
			RenderPlayerInfo();

			// render text.
            m_labels.Map(label_render);

			SDL_RenderPresent(m_renderer);
			m_should_render = 0;
		}

		goto loop_here;
	}

	SDL_Log("End of cApp::Run() method");
}

void cApp::OnFocusLost() {
	for (int x = 0; x < m_playersUsed; x++) {
		m_players[x].m_alt_fire = 0;
		m_players[x].m_fire = 0;
		//m_players[x].m_moveDir = 0;
	}
}


void cApp::PlayerDies(int ix) {
	//
	// TODO: deactivate player.
	// will need to see if it's a game over and initiate that stage...
	//

    m_players[ix].m_active = false;

    for (int x = 0; x < m_playersUsed; x++) {
        if (m_players[x].m_active)
            return;
    }

    // looks like no players are active.
    GameOver();

}

void cApp::PlayerDies(cPlayer* ptr) {
	for (int x = 0; x < m_playersUsed; x++) {
		if (&m_players[x] == ptr)
			PlayerDies(x);
	}
}

void cApp::GameOver() {
    m_state = GAME_OVER;

    SDL_Color red;
    to_c64_SDL_Colour(2, &red, 255);
    cLabel *label = RenderLabel(m_game_over_label_id, "GAME OVER", 100, 100, 25, &red);
	if (label)
		label->m_blink = 1000;
}


int cApp::IsVisible(const SDL_Rect* rect) {
	SDL_Rect camera_rect;
	GetCameraRect(&camera_rect);
	return SDL_IntersectRect(rect, &camera_rect, NULL);
}

void cApp::StartLevel() {
	int x;

	GenerateMap();

	// generate some enemy ships...
	
    char label[64];

    m_check_enemy_status = 0;

	//m_players[0].Move(cPlayer::DIR_RIGHT, 1);
	m_players[0].MoveRight();
	m_players[0].SetPosition(100, 50);
    m_players[0].m_health = 100;

	// set the enemy ships...
    int ships = 10 * m_current_level;

	SDL_snprintf(label, sizeof(label), "Level: %d; Enemies: %d;", m_current_level, ships);

    SDL_Colour green;
    to_c64_SDL_Colour(3, &green, 255);

    cLabel *plabel = RenderLabel(m_game_start_label_id_temp, label, 100, 200, 12, &green);
	if (plabel) {
		plabel->x = 5;
		plabel->y = WIN_DEFAULT_HEIGHT;
		plabel->m_v_align = 2;
	}

	m_temp_labels_remove_timeout = 0;// SDL_max(m_temp_labels_remove_timeout, 3000);

  //  ships = 1000;
#if 0
	if (!AllocateEnemyShips(ships, 1)) {
        ShouldQuit("COuld not allocate enemy ships)");
		return;
	}
#endif
    int halfMap = m_mapWidthPixels / 2;
    m_enemyShips.Clear();

	for (x = 0; x < ships; x++) {
		cEnemyShip *ship = new cEnemyShip();
		if (!ship) {
			ShouldQuit();
			return;
		}
		ship->m_isActive = true;

        int xx, yy, hit_y;;
        xx = x_rand(halfMap, m_mapWidthPixels - ship->m_sprite.Width());
        yy = x_rand(10, m_height / 2);
		ship->SetPosition(xx, yy);

        hit_y = ship->m_sprite.HitDetectTiles();
        if (hit_y > 0) {
			ship->SetPosition(xx, hit_y - ship->m_sprite.Height() * 2);
        }

		if (ship->m_sprite.m_position_rect.y < 0)
			ship->m_sprite.m_position_rect.y = 0;

        m_enemyShips.Unshift(ship);
	}


    SDL_Rect plrect;
    m_players[0].GetRect(&plrect);

	m_playersUsed = 1;

//    SDL_Log("plrect ");    Debug_Rect(&plrect);

    //m_players[0].SetPosition(100, 50);

	m_frames.Delay();	// just do an initial delay... to init the delta variables, etc...
}

void cApp::OnTick() {
	int x;
	for (x = 0; x < m_playersUsed; x++) {
		if (m_players[x].m_active) {
			m_players[x].OnTick();
		}
	}

    m_enemyShips.Map(EnemyShip_OnTick);
	


    /*
        // check collisions...
        // the player logic already checks if it's hitting a tile.
        // need to check:
            * if a projectile is hitting something
            * if the player is hitting an enemy ship
            * maybe an enemy ship is hitting something else ???
	
		having this logic be in the OnTick (ie: the cPlayer class) functions...
    */

	m_sound.OnTick();

	// go through linked list...
	OnProjectilesTick();

    m_particles.MapFlash(Particle_OnTick);


    if (m_temp_labels_remove_timeout) {
        m_temp_labels_remove_timeout -= SDL_min(m_temp_labels_remove_timeout, g_app->Delta());
        if (m_temp_labels_remove_timeout == 0) {
            // remove temp labels...
     //       RemoveLabel(m_game_over_label_id);
            RemoveLabel(m_game_start_label_id_temp);
        }
    }


    if (m_check_enemy_status) {
        m_check_enemy_status = 0;
        m_enemyShips.MapFlash(EnemyShip_IsInActive);

		// count how many there is
		cLabel* p;
		p = FindLabel(m_game_start_label_id_temp, false);
		if (p) {
			char buffer[64];
			SDL_snprintf(buffer,
				sizeof(buffer),
				"Level %d; Enemies: %d",
				m_current_level,
				m_enemyShips.Size()
			);
			label_set_text(p, buffer);
			//label_set_text(p, )
		}
    }

    if (m_enemyShips.m_list == NULL) {
        // new level ??
        m_current_level++;
        StartLevel();
    }

	if (m_show_credits_timeout) {
		m_show_credits_timeout -= SDL_min(m_show_credits_timeout, Delta());
		if (m_show_credits_timeout == 0) {
			// show next credit (if any).
			if (app_credits[m_credits_label_ix]) {

				cLabel* label = FindLabel(m_credits_label_id, true);
				label_set_text(label, app_credits[m_credits_label_ix]);
				label->px_sz = 16;
				label->x = WIN_DEFAULT_WIDTH - 10;
				label->y = WIN_DEFAULT_HEIGHT - 5;
				label->m_h_align = 2;
				label->m_v_align = 2;
				to_c64_SDL_Colour(7, &label->fg_colour);
				
				//label->m_blink = 500;
				m_show_credits_timeout = 3000;
				m_credits_label_ix++;

				m_show_credits_fade = 500;

				label_setAlpha(label, 0);


			} else {
				RemoveLabel(m_credits_label_id);
			}
		} else if (m_show_credits_timeout > 2000) {
			int diff = (3000 - m_show_credits_timeout);
			diff = SDL_min(500, diff);
			diff = (float)diff / (float)500.0 * 255.0;
			cLabel* label = FindLabel(m_credits_label_id, false);
			if (label)
				label_setAlpha(label, diff);
		} else if (m_show_credits_timeout <= 500) {
			int diff;
			diff = SDL_min(500, m_show_credits_timeout);
			diff = (float)diff / (float)500.0 * 255.0;
			cLabel* label = FindLabel(m_credits_label_id, false);
			if (label)
				label_setAlpha(label, diff);
		}
	}
	
}

void cApp::OnProjectilesTick() {

    m_projectiles.MapFlash(Projectile_OnTick);

}

void cApp::RenderSprite(cSprite *src) {
    SDL_Rect spriterect;

	// not used at the moment... rendered directly from thing.

}

// render the bottom radar...
void cApp::RenderRadar() {
	// TODO: make this a bit faster, like, with avgW being calculated before this.

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 200);
	SDL_RenderFillRect(m_renderer, &m_radarRect);
	
	float avgW, avgH, avgPxW;
	avgW = (float) ((float)m_radarRect.w / (float) m_mapLength);
	avgPxW = (float)m_radarRect.w / (float)m_mapWidthPixels;

	avgH = (float)((float) m_radarRect.h / ((float)m_tilesWinRows));
	/*
	m_radarRect.w = WIN_WIDTH / 2;
	m_radarRect.h = WIN_HEIGHT / 10;
	m_radarRect.x = (WIN_WIDTH - m_radarRect.w) / 2;
	m_radarRect.y = WIN_HEIGHT - (m_radarRect.h * 10);
	*/
	SDL_FRect rect;

	int off = (m_event.common.timestamp << 2) % 100;

	float x1, x2;
	x1 = 0.0;
	
	rect.x = 0.0;
	rect.w = avgW;


	// render tiles first...
	SDL_SetRenderDrawColor(m_renderer, 0, 64, 0, 200);
	int x;
	for (x = 0; x < m_mapLength; x++) {
		rect.x = avgW * x + m_radarRect.x;

        rect.h = m_tileMap.ColumnHeight(x) * avgH;// +m_radarRect.y;
		rect.y = (m_radarRect.h + m_radarRect.y) - rect.h;

		SDL_RenderFillRectF(m_renderer, &rect);
	}

	// draw the enemy ships.
	
	// TODO: calculate how big the ships would be on the radar...
	//rect.w = 4;
	//rect.h = 4;// avgH;
	//rect.w = 4;
	avgW = (float) m_radarRect.w / (float) m_mapWidthPixels;
	avgH = (float)m_radarRect.h / (float)m_height;

	cEnemyShip* ship;
	cLinkedList* i;
    for (i = m_enemyShips.m_list; i != NULL; i=i->pNext) {
		ship = (cEnemyShip*)i->m_ptr;
		if (!ship->m_isActive)
			continue;

		SDL_SetRenderDrawColor(m_renderer, 250, 0, 0, 200);

		rect.w = ceilf(ship->m_sprite.m_position_rect.w * avgW);
		rect.h = ceilf(ship->m_sprite.m_position_rect.h * avgH);

		rect.x = ship->m_sprite.m_position_rect.x * avgW + m_radarRect.x;
		rect.y = ship->m_sprite.m_position_rect.y * avgH + m_radarRect.y;

		SDL_RenderFillRectF(m_renderer, &rect);

		// TODO...
#ifdef DEVELOPMENT_MODE
		rect.w = 2;
		rect.h = 2;
		rect.x = ship->m_idle_move.x * avgPxW + m_radarRect.x;
		rect.y = ship->m_idle_move.y * avgH + m_radarRect.y;
		SDL_SetRenderDrawColor(m_renderer, 100, 250, 0, 200);
		SDL_RenderFillRectF(m_renderer, &rect);
#endif
	}

	// render the players.
	
	SDL_SetRenderDrawColor(m_renderer, 128 + off, 128 + off, 128 + off, 255);
	for (x = 0; x < m_playersUsed; x++) {
		SDL_Rect plrect;

		m_players[x].GetRect(&plrect);

		rect.x = plrect.x * avgW + (m_radarRect.x);
		rect.y = plrect.y * avgH + (m_radarRect.y);
		rect.w = avgW * plrect.w;
		rect.h = avgH * plrect.h;

		SDL_RenderFillRectF(m_renderer, &rect);
	}

	// go through projectiles....
	SDL_SetRenderDrawColor(m_renderer, 200 + off, 200 + off, off, 255);
	
	for (i = m_projectiles.m_list; i != NULL; i = i->pNext) {
		cProjectile* proj = (cProjectile*)i->m_ptr;

		rect.x = proj->m_rect.x * avgW + (m_radarRect.x);
		rect.y = proj->m_rect.y * avgH + (m_radarRect.y);
		rect.w = proj->m_rect.w * avgW;
		if (rect.w < 2.0)
			rect.w = 2.0;
		rect.h = proj->m_rect.h * avgH;
		if (rect.h < 2.0)
			rect.h = 2.0;
		SDL_RenderFillRectF(m_renderer, &rect);
	}
	
}

// ie player name health...
void cApp::RenderPlayerInfo() {
	// render the player info...
	SDL_Rect dstrct;
	int padding = 2;
	int x;
	for (x = 0; x < MAX_PLAYERS; x++) {

		SDL_memcpy(&dstrct, &m_playInfoRects[x], sizeof(SDL_Rect));

		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 128);
		SDL_RenderFillRect(m_renderer, &dstrct);

		if (m_players[x].m_active) {

			SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
			SDL_RenderDrawRect(m_renderer, &dstrct);

			dstrct.w -= (padding * 2);
			dstrct.h -= (padding * 2);
			dstrct.x += padding;
			dstrct.y += padding;


			
			int health = (float)dstrct.w / 100.0 * (float) m_players[x].m_showHealth;
			SDL_SetRenderDrawColor(m_renderer, 255, 64, 0, 200);
			dstrct.w = health;
			SDL_RenderFillRect(m_renderer, &dstrct);
			/*
			m_playInfoRects[x].w = avgW - (padding * 3);
			m_playInfoRects[x].h = 50;
			m_playInfoRects[x].y = padding;
			m_playInfoRects[x].x = (avgW - m_playInfoRects[x].w) / 2 * x * avgW;
			*/
		}
	}

}


void cApp::GetCameraRect(SDL_FRect* dest) {
	dest->x = (float)m_camera_x;
	dest->y = 0.0;
	dest->w = (float)g_canvas_width;
	dest->h = (float)g_canvas_height;
}

void cApp::GetCameraRect(SDL_Rect* dest) {
	dest->x = m_camera_x;
	dest->y = 0;
	dest->w = g_canvas_width;
	dest->h = g_canvas_height;
}


void cApp::RenderTiles() {
	SDL_Rect rect;
	GetCameraRect(&rect);
	
	SDL_Rect tileRect;
	SDL_Colour border_col;

	to_c64_SDL_Colour(15, &border_col);

	// find column
	int col_x1 = m_camera_x / m_tileMap.m_tileSize;
	
	int idx;
	idx = m_tileMap.FindColumn(col_x1);

	// go through tiles...
    while (idx < m_tileMap.Size()) {
		cTile* pTile = m_tileMap.At(idx);

		Tile_GetRect(pTile, &tileRect);
        
        if (pTile->m_type < TILE_MAX && SDL_HasIntersection(&rect, &tileRect) == SDL_TRUE) {
            // render this...
            tileRect.x -= m_camera_x;

            SetColour(&Tile_Colours[pTile->m_type]);

            SDL_RenderFillRect(m_renderer, &tileRect);

            SetColour(&border_col);
            SDL_RenderDrawRect(m_renderer, &tileRect);

        }

        if (tileRect.x >= (rect.x + rect.w))
            break;

		idx++;
	}
}

void cApp::CleanUp() {

    m_spritesheet.Quit();

    m_projectiles.Clear();
    m_labels.Clear();
    m_enemyShips.Clear();

	if (m_initialized & 8) {
		m_sound.CleanUp();
	}
	if (m_initialized & 2) {
		cleanup_font();	// do this first or may cause access violation with TTF_Quit
		TTF_Quit();
	}
	if (m_initialized & 4)
		IMG_Quit();
	if (m_initialized)
		SDL_Quit();



	m_initialized = 0;
}


void cApp::GenerateMap() {
	// soft clear...
    m_tileMap.m_tiles.m_used = 0;

	// add tiles...

	m_mapLength = m_tilesWinCols * (m_current_level + 4);

    m_mapWidthPixels = m_mapLength * m_tileMap.m_tileSize;

	int range = m_tilesWinCols;
	int adj = 3, x, y, z;
	y = adj;
	for (x = 0; x < m_mapLength; x++) {
		 
		if (range <= 0) {
			adj = rand() % 10 + 3;
			range = adj * 2;
			//range = rand() % 10 + 3;
		}
		range--;
		if (y < adj)
			y++;
		else if (y > adj)
			y--;
		
		// add the columns
		//void AddItem(int col, int row, int type);
		z = 0;
		m_tileMap.AddItem(x, z, TILE_BEDROCK);

		while (++z < y)
			m_tileMap.AddItem(x, z, TILE_DIRT);

		m_tileMap.AddItem(x, z, TILE_GRASS);
	}
	
	m_tileMap.SoftSort();
}

void cApp::StartGame() {
	m_current_level = 1;

	// pre allocate some tiles...
	m_tileMap.Allocate(1500);

    m_players[0].m_active = true;
	m_players[0].SetPosition(100, 50);

    RemoveLabel(m_game_over_label_id);

    m_state = PLAYING;

	if (m_creditLabels.Size() == 0) {
		// populate..

		SDL_Colour fg;
		to_c64_SDL_Colour(14, &fg);

		// add some credits/how to...
		struct cLabel label;
		label_init(&label);
		label.x = WIN_DEFAULT_WIDTH - 5;
		label.y = WIN_DEFAULT_HEIGHT - 5;
		label.fg_colour = fg;
		label.m_h_align = label.m_v_align = 2;
		label.px_sz = 18;

		
		int ix = 0;
		for (ix = 0; app_credits[ix] != 0; ix++) {
			label_set_text(&label, app_credits[ix]);
			m_creditLabels.PushItem(&label);
		}

		m_credits_label_id = m_label_id_counter++;
	}
	m_credits_label_ix = 0;
	m_show_credits_timeout = 1000;
}

void cApp::SetColour(Uint32 rgb, Uint8 a) {
    SetRenderDrawColour(rgb, a);
}

const char* cApp::AssetsFile(const char* fn, cString* dest) {
	if (dest == NULL)
		dest = &m_tempString;
#ifdef __WINDOWS__
	//D : \MyDocs\Development\Projects\SDL2_ButtAttacks\assets
	*dest = "D:\\MyDocs\\Development\\Projects\\SDL2_ButtAttacks\\";
#else
	*dest = "/home/matthew/Development/Software/SDL2_ButtAttacks/";
#endif
	//dest->Clear();
	dest->Append("assets");
#ifdef __WINDOWS__
	dest->Append("\\");
#else
	dest->Append("/");
#endif
	if (fn)
		dest->Append(fn);
	return dest->m_buffer;
}
#if 0
bool cApp::AllocateEnemyShips(int sz, int clear) {
	if (sz >= m_enemyShipsAllocated) {
        int newsz;
        newsz  = sz + 8;
		cEnemyShip* newships = new cEnemyShip[newsz];
		if (!newships) {
            SDL_Log("Could not allocate cEnemyShip [%d]", sz);
			return false;
		}
		
		if (m_enemyShips) {
			for (int x = 0; x < m_enemyShipsUsed; x++)
				newships[x] = m_enemyShips[x];
			delete[] m_enemyShips;
		}
		m_enemyShips = newships;
		m_enemyShipsAllocated = newsz;
	}
	if (clear) {
		// not sure what to do here...
//		for (int x = 0; x < m_enemyShipsAllocated; x++)
	//		m_enemyShips[x].m_sprite.Cl
	}

    m_enemyShipsUsed = sz;

    return true;
}
#endif

void cApp::ShouldQuit(const char *msg) {
    SDL_Log("cApp::ShouldQuit is called");
    if (msg)
        SDL_Log("%s", msg);
    m_should_quit++;
}

cLabel *cApp::RenderLabel(const char *text, int x1, int y1, int sz, SDL_Colour *fg) {
    cLabel *retval;
    retval = label_new();
    if (retval) {
        if (fg)
            label_set_colour(retval, fg);
        else
            to_SDL_Colour(g_palette[8], &retval->fg_colour, 255);

        label_set_position(retval, x1, y1);
        label_set_text(retval, text);
        label_set_pixel_size(retval ,sz);

        m_labels.Push(retval);
    }
    return retval;
}

cLabel* cApp::FindLabel(int ID, bool create) {
	cLinkedList* i;
	cLabel* p = NULL;
	for (i = m_labels.m_list; i != NULL; i = i->pNext) {
		p = (cLabel*)i->m_ptr;
		if (p->m_id == ID)
			// found it
			return p;
	}
	if (create) {
		p = label_new();
		p->m_id = ID;
		m_labels.Push(p);
		return p;
	}
	return NULL;
}

cLabel *cApp::RenderLabel(int ID, const char *text, int x1, int y1, int sz, SDL_Colour *fg) {
    // see if there is a label with this ID first then modify that...
	cLabel* retval;
	retval = FindLabel(ID);
	
    if (retval == NULL) {
        // create one...
        retval = RenderLabel(text, x1, y1, sz ,fg);
        retval->m_id = ID;
    } else {
        // update...
        label_set_text(retval, text);
        label_set_position(retval, x1, y1);
        label_set_pixel_size(retval, sz);
        label_set_colour(retval, fg);
    }
    return retval;
}

int cApp::RemoveLabel(int id, bool remove_all) {
    // see if there is a label with this ID and remove it...
    if (remove_all) {
        int retval = 0;
        while (RemoveLabel(id, false))
            retval++;
        return retval;
    }

    for (cLinkedList *i = m_labels.m_list; i != NULL; i=i->pNext) {
        cLabel *ptr = (cLabel *)i->m_ptr;
        if (ptr->m_id == id) {
            RemoveLabel(ptr);
            return 1;
        }
    }
    return 0;
}
