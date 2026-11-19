#include "mixchunk.h"
#include "Sound.h"
#include "common.h"
#include "App.h"

cMixChunk::cMixChunk() {
    m_type = -1;
    m_chunk = 0;
    m_file = 0;
    m_timestamp = 0;
    m_channel = 0;
    m_references  = 0;
}
cMixChunk::~cMixChunk() {
    Close(1);
}

int cMixChunk::Load() {
    if (!m_chunk) {
        if (m_type < 0 || m_type >= SOUNDFX_MAX)
            return 0;

        // see if it's preloaded...
        if (g_app->m_sound.m_preloaded_files[m_type]) {
            // there is a preloaded file... use the memory chunk instead...

            m_file = SDL_RWFromMem(
                g_app->m_sound.m_preloaded_files[m_type],
                g_app->m_sound.m_preloaded_files_sz[m_type]
            );
        }
        else {

            cString fn;
            g_app->AssetsFile(g_sound_fns[m_type], &fn);
#ifdef USE_EMISCRIPTEN
            // don't
            fn.Append(".ogg");
#else
            fn.Append(".wav");
#endif

            m_file = SDL_RWFromFile(fn.m_buffer, "rw");

            if (!m_file) {
                SDL_Log("Could not load file: %s", fn.m_buffer);
                return 0;
            }
        }

        // load chunk, we'll close the rw file ourselves...
        m_chunk = Mix_LoadWAV_RW(m_file, 0);
        
        if (!m_chunk) {
            SDL_Log("Could not load chunk.");
            return 0;
        }

        m_references++;
    } else {
        m_references++;
    }

    MarkTimestamp();

    return 1;
}

void cMixChunk::Close(int force) {
    if (m_references < 2 || force) {
        if (m_chunk) {
            Mix_FreeChunk(m_chunk);
            m_chunk = 0;
        }
        if (m_file) {
            SDL_RWclose(m_file);
            m_file = 0;
        }
        m_channel = -1;
        m_references = 0;
    } else if (m_references)
        m_references--;
}

void cMixChunk::SetType(int type) {
    if (m_type != type) {
        Close();
        m_type = type;
    }
}


int cMixChunk::PlayChannelTimed(int channel, int repeat /*= 0*/, int mills/* = -1*/) {
    int retval = Mix_PlayChannelTimed(channel, m_chunk, repeat, mills);
    m_channel = channel;
    m_repeat = repeat;
    m_mills = mills;
    MarkTimestamp();
    return retval;
}

void cMixChunk::Stop() {
    // will stop the chunk...
    if (m_chunk) {
        if (m_channel < 0) {
            // find the thingy.
            int max = g_app->m_sound.m_channels_max;
            for (int x = 0; x < max; x++) {
                if (Mix_GetChunk(x) == m_chunk) {
                    Mix_HaltChannel(x);
                }
            }
            m_channel = -1;
        } else {
            Mix_HaltChannel(m_channel);
            m_channel = -1;
        }
    }
}

void cMixChunk::Restart() {
    if (m_chunk) {
        Stop();
        PlayChannelTimed(-1, m_repeat, m_mills);
    }
}


