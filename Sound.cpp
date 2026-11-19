#include "Sound.h"
#include "common.h"
#include "App.h"
#include "framework.h"

/*
    SOUNDFX_HIT_1 = 0,
    SOUNDFX_SHOOT_1,

    SOUNDFX_LAZER_1,

    SOUNDFX_EXPLOSION_1,


    SOUNDFX_BEEPS,

    SOUNDFX_MAX
*/
// sound filenames...
const char *g_sound_fns[] = {
    "8bit_hit_8",       //.wav",        // don't have extension as with emiscripten
    "8bit_gunshot_2",    //.wav",        // we'll want to have OGG, rather than WAV...
//    /media/matthew/5E300E05300DE53D/MyDocs/Development/Projects/SDL2_ButtAttacks/assets/8bit_laser_rico_1.wav
    "8bit_laser_rico_1",

    "8bit_explosion_9",

    "8bit_beeps_8",

    0   // last
};

cSound::cSound() {
    m_hasInit = 0;

    m_channels_max = 2;

    m_fx_volume = 10;

    SDL_memset(m_channels, -1, sizeof(m_channels));

    // will use SDL functions to determine what's playing...
//    for (int x =0; x < m_channels_max; x++)
  //      m_playing[x] = -1;

    SDL_memset(m_timestamps, 0, sizeof(m_timestamps));
    SDL_memset(m_playing, -1, sizeof(m_playing));

    SDL_memset(m_replay, -1, sizeof(m_replay));
#ifdef PRELOAD_SOUND_FX
    SDL_memset(m_preloaded_files, 0, sizeof(m_preloaded_files));
#endif
    m_references  = 0;
    m_cleanupTimestamp = 0;
}

cSound::~cSound() {
    CleanUp();
}

bool cSound::Init() {
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)) {
        SDL_Log("Could not initialize MIXER. %s\n", SDL_GetError());
        m_hasInit += 1;
        return false;
    }

    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096)) {
        SDL_Log("Could not open audio: %s\n", SDL_GetError());
        m_hasInit += 2;
        return false;
    }

    SDL_memset(m_channels,-1, sizeof(m_channels));

    Mix_ChannelFinished(OnMixChannelFinished);

    Pause(true);

    cString fn;
    
    for (int x = 0; x < SOUNDFX_MAX; x++) {
        m_mix_chunks[x].SetType(x);

#ifdef PRELOAD_SOUND_FX
        g_app->AssetsFile(g_sound_fns[x], &fn);
#ifdef USE_EMISCRIPTEN
        fn.Append(".ogg");
#else
        fn.Append(".wav");
#endif
#ifdef __WINDOWS__
        FILE* fp;
        fopen_s(& fp, fn.m_buffer, "rb");
#else
        FILE* fp = fopen(fn.m_buffer, "rb");
#endif

        if (fp) {
            // see how big the file is...
            Uint64 sz;
            fseek(fp, 0, SEEK_END);
            sz = ftell(fp);
            m_preloaded_files_sz[x] = sz;
            m_preloaded_files[x] = (char *)x_malloc(sz);
            if (m_preloaded_files[x]) {
                fseek(fp, 0, SEEK_SET);
                fread(m_preloaded_files[x], m_preloaded_files_sz[x], 1, fp);
            }

            fclose(fp);
        }
#else
        // these should not need to be initialized... but just in case..
        m_preloaded_files[x] = NULL;
        m_preloaded_files_sz[x] = 0;
#endif
    }

    return true;
}

void cSound::OnMixChannelFinished(int channel) {// static
    cSound* pSound = &g_app->m_sound;
    
    SDL_assert(pSound != NULL);
    // clean up in a little bit
    pSound->m_cleanupTimestamp = SDL_GetTicks() + 1000;
    
}

void cSound::CleanUp() {

    // close channels...

    Mix_ChannelFinished(NULL);

    int x;
    for (x = 0; x < m_channels_max; x++) {
        if (m_channels[x] > 0) {
            Mix_HaltChannel(m_channels[x]);
        }
    }

    for (x = 0; x < SOUNDFX_MAX; x++)
        m_mix_chunks[x].Close();

    if (m_hasInit & 2) {
        m_hasInit -= 2;
        Mix_ChannelFinished(0);
    }

    if (m_hasInit & 1) {
        Mix_Quit();
        m_hasInit -= 1;
    }

    for (x = 0; x < SOUNDFX_MAX; x++) {
        if (m_preloaded_files[x]) {
            x_free(m_preloaded_files[x]);
            m_preloaded_files[x] = NULL;
        }
    }
}

void cSound::Pause(bool pause /*= true*/) {
    if (pause != IsPaused())
        SDL_PauseAudio(pause ? 1 : 0);
}

bool cSound::IsPaused() {
    switch (SDL_GetAudioStatus()) {
    case SDL_AUDIO_PLAYING:
        return true;
        break;
    case SDL_AUDIO_STOPPED:
    case SDL_AUDIO_PAUSED:
    default:
        return true;
        break;
    }
}

void cSound::PlaySoundFx(int type) {

    if (type < 0 || type >= SOUNDFX_MAX || m_fx_volume <= 0)
        return;
#ifndef ENABLE_SOUND
    return;
#endif
    Uint32 curticks = SDL_GetTicks();

    int x, empty = -1, empty_channel = -1;
    int found = -1;

    // lets load the mix chunk first, then see if it's playing
    // if so, we will stop and replay it here..
    if (!m_mix_chunks[type].Load()) {
        SDL_Log("Could not load music type: %d", type);
        ShouldQuit();
        return;
    }

    // see if this is being playing already, if so stop it.
    for (x = 0; x < m_channels_max; x++) {
        Mix_Chunk* mix = Mix_GetChunk(x);
        if (mix == m_mix_chunks[type].m_chunk) {
            found++;
            m_mix_chunks[type].Stop();
        }
    }

    // now play for the next available channel...
    float vol = float(128 / 10) * m_fx_volume;
    int iv;
    iv = xIntRange((int)vol, 0, 128);
    m_mix_chunks[type].SetVolumne(iv);
    m_mix_chunks[type].PlayChannelTimed(-1, 0, -1);
}

void cSound::OnTick() {
    if (m_cleanupTimestamp == 0 ||
        m_cleanupTimestamp > g_app->GetEventTimestamp())
        return;

    m_cleanupTimestamp = 0;

    Mix_Chunk* p;

    int x, y, f, z;
    f = 0;
    for (x = 0; x < m_channels_max; x++) {

        if (Mix_Playing(x)) {
            continue;
        }

        p = Mix_GetChunk(x);
        if (p == NULL)
            continue;

        z = -1;
        for (y = 0; y < m_channels_max; y++) {
            if (m_mix_chunks[y].m_chunk == p)
                z = y;
            if (y != x && m_mix_chunks[x].m_chunk &&
                m_mix_chunks[x].m_chunk == p && 
                Mix_Playing(y)) {

                f++;

            }
        }

        if (f <= 0 && z >= 0) {
            // close this chunk.
            m_mix_chunks[z].Stop();
            m_mix_chunks[z].Close(1);
        }
    }
}
