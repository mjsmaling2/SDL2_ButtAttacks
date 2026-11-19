#include "Point.h"

cPoint::cPoint() {
	SDL_memset(&m_pt, 0, sizeof(SDL_Point));
}

#if 0
int cPoint::DistanceTo(const cPoint& dest, int* w, int* h, int* d) {
	return DistanceTo(dest.m_pt, w, h, d);
}
#endif
int cPoint::DistanceTo(const SDL_Point* dest, int* w, int* h, int* d) {
	SDL_assert(dest != NULL);

	int retval = 0;

	int ww, hh, dd;

	ww = dest->x - m_pt.x;
	hh = dest->y - m_pt.y;

#ifndef USE_QUICK_DISTANCE_CALCULATION 
	dd = (int) sqrt(pow(ww, 2) * pow(hh, 2));
	dd = abs(dd);
#else
	dd = abs(ww) + abs(hh);
#endif

	if (w)
		*w = ww;
	if (h)
		*h = hh;
	if (d)
		*d = dd;

	return dd;
}

void cPoint::Set(const SDL_Rect* rct, const SDL_Point* align) {
	if (align->x < rct->x)
		m_pt.x = rct->x;
	else if (align->x >= rct->x + rct->w)
		m_pt.x = rct->x + rct->w - 1;
	else
		m_pt.x = align->x;
	if (align->y < rct->y)
		m_pt.y = rct->y;
	else if (align->y >= (rct->y + rct->h))
		m_pt.y = rct->y + rct->h - 1;
	else
		m_pt.y = align->y;
}

void cPoint::Set(const SDL_Rect* rect) {
	m_pt.x = rect->x + rect->w / 2;
	m_pt.y = rect->y + rect->h / 2;
}
