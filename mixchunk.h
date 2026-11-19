#ifndef MIXCHUNK_H
#define MIXCHUNK_H

#include "framework.h"
#include <SDL2/SDL_mixer.h>

class cMixChunk
{
public:
    cMixChunk();
    virtual ~cMixChunk();

    int Load();    // will load based on type... will also update time stamp...

    void Close(int force = 0);

    void SetType(int type);

    // just some wrapper functions... assume everything okay with varaibles...
    void SetVolumne(int iv) {
        Mix_VolumeChunk(m_chunk, iv);
    }

    int PlayChannelTimed(int channel, int repeat = 0, int mills = -1);

    void MarkTimestamp() {
        m_timestamp = SDL_GetTicks();
    }

    void Stop();

    void Restart(); // replay the sound.

    int m_type; // see Sound.h
    
    Mix_Chunk *m_chunk;

    int m_channel;

    int m_repeat, m_mills;  // preserved if restart sound...

    SDL_RWops *m_file;

    Uint32 m_timestamp;

    int m_references;   // how many opens...
};

#endif // MIXCHUNK_H
