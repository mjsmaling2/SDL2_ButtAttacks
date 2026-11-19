#include "Rect.h"

cRect::cRect() {
	SDL_memset(&m_rect, 0, sizeof(SDL_Rect));
}

// will return x >= x1 && x <= x2
int cRect::XRange(int x) {
	if (x < m_rect.x)
		return m_rect.x;
	int x2 = X2();
	if (x > x2)
		return x2;
	return x;
}
// will return y >= y1 && y <= y2
int cRect::YRange(int y) {
	if (y < m_rect.y)
		return m_rect.y;
	int y2 = Y2();
	if (y > y2)
		return y2;
	return y;
}

void cRect::GetPositionRange(SDL_Point* dest, int xx, int yy) {
	SDL_assert(dest != NULL);
	dest->x = XRange(xx);
	dest->y = YRange(yy);
}
