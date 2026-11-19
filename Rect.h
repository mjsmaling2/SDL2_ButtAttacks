#pragma once

#include "framework.h"

// just a convienent wrapper around SDL_Rect

class cRect {
public:
	cRect();
	cRect(const SDL_Rect& src) {
		m_rect = src;
	};

	int X2() {
		return m_rect.x + m_rect.w - 1;
	};

	int Y2() {
		return m_rect.y + m_rect.h - 1;
	};

	int X2(const SDL_Rect& src) {
		return src.x + src.w - 1;
	};
	int Y2(const SDL_Rect& src) {
		return src.y + src.h - 1;
	};


	// will return x >= x1 && x <= x2
	int XRange(int x);
	// will return y >= y1 && y <= y2
	int YRange(int y);


	cRect& operator = (const SDL_Rect& src) {
		m_rect = src;
		return *this;
	};

	cRect& operator = (const cRect &src) {
		m_rect = src.m_rect;
		return *this;
	};
	
	void GetPosition(SDL_Point* dest) {
		dest->x = m_rect.x;
		dest->y = m_rect.y;
	};
	void GetPositionRange(SDL_Point* dest, int xx, int yy);
	

	// just +/- x,y coordinates.
	void AdjustPosition(int x, int y) {
		m_rect.x += x;
		m_rect.y += y;
	};

	void SetPosition(const SDL_Rect& rect) {
		m_rect.x = rect.x;
		m_rect.y = rect.y;
	};

	void SetPosition(const cRect& src) {
		SetPosition(src.m_rect);
	};

	SDL_Rect m_rect;
};

