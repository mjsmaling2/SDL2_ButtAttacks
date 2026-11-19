#pragma once

#include "framework.h"
#include "Rect.h"

// if defined will use simple addition for calculating distance...
#define USE_QUICK_DISTANCE_CALCULATION 1

class cPoint {
public:
	cPoint();
	cPoint(const SDL_Point& src) {
		m_pt = src;
	};
	
	
	// will set point to center of rect...
	void Set(const SDL_Rect* rect);

	// x/y will be >= align.x|y && < align.x2|y2
	void Set(const SDL_Rect *rect, const SDL_Point * align);

	void DeterminePoint(const SDL_Rect& rect);

	int DistanceTo(const cPoint& dest, int* w, int* h, int* d) {
		return DistanceTo(&dest.m_pt, w, h, d);
	};
	int DistanceTo(const SDL_Point* dest, int* w, int* h, int* d);
	
	cPoint & operator  = (const SDL_Point & pt) {
		m_pt = pt;
		return *this;
	};


	SDL_Point m_pt;
};

