#include "Framerate.h"

cFramerate::cFramerate(int fps) {
	m_fps = fps;
}

cFramerate::~cFramerate() {
	// do nothing...
}

void cFramerate::Start(int fps) {
	m_framecount = 0;
	m_rateticks = (1000.0f / (float)fps);
	m_baseticks = SDL_GetTicks();
	m_lastticks = m_baseticks;
	m_fps = fps;
	m_delta = 0;
}

Uint32 cFramerate::Delay() {
	Uint32 target;
	Uint32 delay;

	m_timestamp_a = m_current = SDL_GetTicks();
    
	m_framecount++;

	target = m_baseticks + (m_framecount * m_rateticks);
	if (m_current < target) {
		delay = target - m_current;
		SDL_Delay(delay);
	} else {
		// reset if over ???
		m_framecount = 0;
		m_baseticks = m_current;
	}

	m_timestamp_b = SDL_GetTicks();
    m_delta = m_timestamp_b - m_lastticks;
	m_lastticks = m_timestamp_b;

	return m_delta;
}
