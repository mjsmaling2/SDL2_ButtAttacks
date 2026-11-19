#pragma once

#include "framework.h"
#include "common.h"

//
// this is basically an encapsulated SDL_rect, but it will have
// and "adj_x, adj_y" that can be used to return adjusted x/y (and/or w/h) properties...
//
class cRectAdj
{
public:
	cRectAdj(const SDL_Rect& rect);
	cRectAdj();
	virtual ~cRectAdj();


	cRectAdj& operator = (const SDL_Rect& src);
	
	void SetRect(const SDL_Rect& src);

	void ClearAdj();
	
	// returns the adjusted rect....
	void GetRect(SDL_Rect *dest); 

	
	int AbsMaxAdjX(int a, int b);
	int AbsMaxAdjY(int a, int b);
	int AbsMaxAdjW(int a, int b);
	int AbsMaxAdjH(int a, int b);
	int AbsMinAdjX(int a, int b);
	int AbsMinAdjY(int a, int b);
	int AbsMinAdjW(int a, int b);
	int AbsMinAdjH(int a, int b);


	void AdjX(int x) {
		m_rect.x += x;
	}

	void AdjY(int y) {
		m_rect.y += y;
	}
	void AdjW(int w) {
		m_rect.w += w;
	}
	void Adjh(int h) {
		m_rect.h += h;
	}

	int x() const { return m_rect.x; };
	int y() const { return m_rect.y; };
	int w() const { return m_rect.w; };
	int h() const { return m_rect.h; };

	SDL_Rect m_rect;			// the adjusted rect...
	SDL_Rect m_orig_rect;	// the origianl rect...
	int m_last_adj;
protected:
	void AbsMaxAdj(int* dest, int a, int b) {
		m_last_adj = x_AbsMax(a, b);
		*dest += m_last_adj;
	}
	void AbsMinAdj(int* dest, int a, int b) {
		m_last_adj = x_AbsMin(a, b);
		*dest += m_last_adj;
	}
};

