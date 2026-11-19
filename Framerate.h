#ifndef FRAMERATE_H
#define FRAMERATE_H

#include "framework.h"

class cFramerate
{
public:
	cFramerate(int fps = 0);
	virtual ~cFramerate();

	void Start() {
		Start(m_fps);
	};

	void Start(int fps);


	// will delay for next frame... will return the delta from
	// last delay to return...
	Uint32 Delay();

	

	// make members public...
	Uint32 m_framecount;	// the framecount (from 0 when Start)
	float m_rateticks;		// rateticks (1000/fps)
	Uint32 m_baseticks;		// the starting base ticks
	Uint32 m_lastticks;		// last ticks (from delay)
	Uint32 m_fps;			// the initiap fps rate variable...

	Uint32 m_delta;			// since last delay... or delta
	Uint32 m_current;		// the currint timer in Delay


	Uint32 m_timestamp_a, m_timestamp_b;	// used in the delay function...
protected:

private:

};

#endif // FRAMERATE_H
