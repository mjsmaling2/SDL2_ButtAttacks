#pragma once

#include "framework.h"

typedef struct cLabel {
	SDL_Texture* texture;
	int x, y;
	int px_sz;	// pixel size
	char* m_text;	// the text being used.
	SDL_Color fg_colour;
    int m_id;

	int m_h_align;	// 0 - left (to right) ,1 - center, 2 - right (to left)
	int m_v_align;	// 0 - top, 1 center, 2 bottom.

	int m_alpha;	// 0 - invisible - 255 visible

	Uint64 m_blink; // blink every nth milliseconds...

} cLabel;

cLabel* label_new();

void label_init(struct cLabel* dest);

void label_clear(struct cLabel* dest);
void label_free(void* ptr);

void label_set_colour(struct cLabel* dest, SDL_Colour* colour);
void label_set_position(struct cLabel* dest, int x, int y);
void label_set_pixel_size(struct cLabel* dest, int px);
void label_set_text(struct cLabel* dest, const char* src);

void label_render(struct cLabel* src);
void label_render(void* ptr);

void label_render_multi(struct cLabel* src, int sz = 1);

void label_init(void* src, size_t len);
void label_copy(void* dest, const void* src, size_t sz);
void label_clear(void* dest);
int label_compare(const void* a, const void* b);

void label_setAlpha(struct cLabel* dest, int alpha = 255);

// wrapper around struct
class Label {
public:
	Label(const Label* src);
	Label();
	virtual ~Label();

	Label& operator = (const Label& src);

	void SetColour(SDL_Colour* colour) {
		label_set_colour(&m_data, colour);
	}

	void SetPosition(int x, int y) {
		label_set_position(&m_data, x, y);
	}
	void SetPixelSize(int px) {
		label_set_pixel_size(&m_data, px);
	}
	void SetText(const char* text) {
		label_set_text(&m_data, text);
	}

	void Render() {
		label_render(&m_data);
	}

	void SetAlpha(int alpha) {
		label_setAlpha(&m_data, alpha);
	}

	struct cLabel m_data;
};
