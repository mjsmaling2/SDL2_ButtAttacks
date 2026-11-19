// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

// if set/defiend will show some debugging/devleopment stuff in game.
#define DEVELOPMENT_MODE 1

#ifdef DEVELOPMENT_MODE

#define ENABLE_SOUND

#endif

#define PRELOAD_SOUND_FX 1

#if 0
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_stdinc.h>
//#include <SDL2/SDL_mixer.h>

