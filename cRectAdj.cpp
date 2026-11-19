#include "cRectAdj.h"

cRectAdj::cRectAdj(const SDL_Rect& rect) {
	SetRect(rect);
}
cRectAdj::cRectAdj() {
	SDL_memset(&m_rect, 0, sizeof(SDL_Rect));
	ClearAdj();
}
cRectAdj::~cRectAdj() {

}

cRectAdj& cRectAdj::operator = (const SDL_Rect& src) {
	SetRect(src);
	return *this;
}

void cRectAdj::SetRect(const SDL_Rect& src /*= true*/) {
	SDL_memcpy(&m_rect, &src, sizeof(SDL_Rect));
	SDL_memcpy(&m_orig_rect, &src, sizeof(SDL_Rect));
}

void cRectAdj::ClearAdj() {
	m_rect = m_orig_rect;
}
// returns the adjusted rect....
void cRectAdj::GetRect(SDL_Rect* dest) {
	*dest = m_rect;
}

int cRectAdj::AbsMaxAdjX(int a, int b) {
	AbsMaxAdj(&m_rect.x, a, b);
	return m_last_adj;
}
int cRectAdj::AbsMaxAdjY(int a, int b) {
	AbsMaxAdj(&m_rect.w, a, b);
	return m_last_adj;
}
int cRectAdj::AbsMaxAdjW(int a, int b) {
	AbsMaxAdj(&m_rect.w, a, b);
	return m_last_adj;
}
int cRectAdj::AbsMaxAdjH(int a, int b) {
	AbsMaxAdj(&m_rect.h, a, b);
	return m_last_adj;
}

int cRectAdj::AbsMinAdjX(int a, int b) {
	AbsMinAdj(&m_rect.x, a, b);
	return m_last_adj;
}
int cRectAdj::AbsMinAdjY(int a, int b) {
	AbsMinAdj(&m_rect.y, a, b);
	return m_last_adj;
}
int cRectAdj::AbsMinAdjW(int a, int b) {
	AbsMinAdj(&m_rect.w, a, b);
	return m_last_adj;
}
int cRectAdj::AbsMinAdjH(int a, int b) {
	AbsMinAdj(&m_rect.h, a, b);
	return m_last_adj;
}
