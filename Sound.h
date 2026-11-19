#pragma once

#include "framework.h"
#include "mixchunk.h"

enum {
    SOUNDFX_HIT_1 = 0,
    SOUNDFX_SHOOT_1,

    SOUNDFX_LAZER_1,

    SOUNDFX_EXPLOSION_1,


    SOUNDFX_BEEPS,

    SOUNDFX_MAX
};

// sound filenames...
extern const char *g_sound_fns[];

// just a wrapper around Mix_Chunk...

class cSound {
public:

	cSound();
	virtual ~cSound();

	bool Init();
	void CleanUp();

	void OnTick();

	void Pause(bool pause = true);
	bool IsPaused();

	// play sound fx...
	void PlaySoundFx(int type);

	static void OnMixChannelFinished(int channel);

    int m_channels[2];
    int m_playing[2];  // what is it playing : type...

	int m_replay[2];	// replay a sound when stopped...

    //Mix_Chunk *m_chunks[2];
    //SDL_RWops *m_rwops[2];
    Uint32 m_timestamps[2];


	int m_channels_max;

    int m_fx_volume;    // ranges from 0 - 10


    cMixChunk m_mix_chunks[SOUNDFX_MAX];
//#ifdef PRELOAD_SOUND_FX
	char* m_preloaded_files[SOUNDFX_MAX];
	size_t m_preloaded_files_sz[SOUNDFX_MAX];
//#endif
    // how many opens have there been...
    int m_references;

	Uint32 m_cleanupTimestamp;

protected:
	int m_hasInit;
	bool m_isPaused;
};

