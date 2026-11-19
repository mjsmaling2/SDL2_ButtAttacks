#include "Label.h"
#include "App.h"
#include "common.h"
#include "Font.h"
/*
struct cLabel {
	SDL_Texture* texture;
	int x, y;
	int px_sz;	// pixel size
	char* m_text;	// the text being used.
	SDL_Color fg_colour;
};
*/

static void label_cleartexture(struct cLabel* dest);

static void label_update_texture_alpha(struct cLabel* dest);

void label_update_texture_alpha(struct cLabel* dest) {
	if (dest && dest->texture) {
		int alpha = xIntRange(dest->m_alpha, 0, 255);
		SDL_SetTextureAlphaMod(dest->texture, alpha);// alpha);
		//SDL_SetTextureBlendMode(dest->texture, alpha == 255 ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
	}
}

void label_cleartexture(cLabel* dest) {
	if (dest && dest->texture) {
		SDL_DestroyTexture(dest->texture);
		dest->texture = 0;
	}
}

cLabel* label_new() {
	cLabel *retval = (cLabel *)x_malloc(sizeof(cLabel));

	if (retval)
		label_init(retval);

	return retval;
}

void label_init(struct cLabel* dest) {
	SDL_memset(dest, 0, sizeof(struct cLabel));
	dest->m_alpha = 255;
}
void label_clear(struct cLabel* dest) {
	if (dest) {
		label_cleartexture(dest);
		if (dest->m_text)
			x_free(dest->m_text);
		label_init(dest);
		//SDL_memset(dest, 0, sizeof(cLabel));
	}
}

void label_free(void* ptr) {
	if (ptr) {
		label_clear((struct cLabel*)ptr);
		x_free(ptr);
	}
}

void label_set_colour(struct cLabel* dest, SDL_Colour* colour) {
	if (colour && dest) {
		dest->fg_colour = *colour;
		label_cleartexture(dest);
	}
}
void label_set_position(struct cLabel* dest, int x, int y) {
	if (dest) {
		dest->x = x;
		dest->y = y;
	}
}

void label_set_pixel_size(struct cLabel* dest, int px) {
	if (dest) {
		dest->px_sz = px;
		label_cleartexture(dest);
	}
}
void label_set_text(struct cLabel* dest, const char* src) {
	if (dest) {
		dest->m_text = x_strdup(src, dest->m_text);
		label_cleartexture(dest);
	}
}

void label_render(void* p) {
	label_render((struct cLabel*)p);
}

void label_render(struct cLabel* src) {
	SDL_assert(src != NULL);
	if (!src)
		return;

	if (src->m_blink) {
		Uint32 timestamp = g_app->GetEventTimestamp();
		if (timestamp / src->m_blink & 1) {
			return;
		}
	}

	if (!src->texture) {
		if (!src->m_text)
			return;
		// generate texture.
		TTF_Font *font = get_font_px(src->px_sz);
		if (!font)
			return;
			
		SDL_Surface *surface = TTF_RenderText_Blended(font, src->m_text, src->fg_colour);
		if (!surface)
			return;

		src->texture = SDL_CreateTextureFromSurface(g_app->m_renderer, surface);

		label_update_texture_alpha(src);

		SDL_FreeSurface(surface);
#if 0
			TextCacheItem* pItem = new TextCacheItem(text, fg, bg, pFont->m_iSize);
		if (bg.a > 0)
			pItem->m_pTexture = TTF_RenderText_Shaded(pFont->m_pFont, text.c_str(), fg, bg);
		else
			pItem->m_pTexture = TTF_RenderText_Blended(pFont->m_pFont, text.c_str(), fg);

		m_cache.push_back(pItem);

		SDL_Texture* retval;
		retval = pItem->m_pTexture.Ptr();
#endif
		if (!src->texture)
			return;
	}

	SDL_Rect dest;
	dest.x = src->x;
	dest.y = src->y;

	Uint32 format;
	int access;
	SDL_QueryTexture(src->texture, &format, &access, &dest.w, &dest.h);

	switch (src->m_h_align) {
	case 1:	// center
		dest.x -= (dest.w / 2);
		break;
	case 2: // right (to left)
		dest.x -= dest.w;
		break;
	default:	// do nothing;
		break;
	};
	switch (src->m_v_align) {
	case 1:	// center
		dest.y -= (dest.h / 2);
		break;
	case 2:	// bottom
		dest.y -= (dest.h);
		break;
	case 0:
	default:	// do nothing...
		break;
	};
	SDL_RenderCopy(g_app->m_renderer, src->texture, NULL, &dest);
}

void label_render_multi(struct cLabel* src, int sz) {
	for (int x = 0; x < sz; x++) {
		struct cLabel* ptr = &(src[x]);
		label_render(ptr);
	}
}


void label_init(void* src, size_t l) {
	SDL_assert(l == sizeof(cLabel));
	label_init((cLabel*)src);
}
void label_copy(void* dest, const void* src, size_t sz) {
	SDL_assert(sz == sizeof(cLabel));
	cLabel* a;
	const cLabel* b;

	a = (cLabel*)dest;
	b = (const cLabel*)src;

	label_cleartexture(a);

	a->x = b->x;
	a->y = b->y;
	a->px_sz = b->px_sz;
	a->m_text = x_strdup(b->m_text, a->m_text);
	a->fg_colour = b->fg_colour;
	a->m_id = b->m_id;	// should this be copied???
	a->m_h_align = b->m_h_align;
	a->m_v_align = b->m_v_align;
	a->m_blink = b->m_blink;
	a->m_alpha = b->m_alpha;
}


void label_clear(void* dest) {
	label_clear((cLabel*)dest);
}

int label_compare(const void* a, const void* b) {
	const cLabel* pA, * pB;
	pA = (const cLabel*)a;
	pB = (const cLabel*)b;

	// compare by ??? 
	if (pA > pB)
		return 1;
	else if (pA < pB)
		return -1;
	return 0;
}

void label_setAlpha(struct cLabel* dest, int alpha /*= 255*/) {
	SDL_assert(dest != NULL);
	if (alpha != dest->m_alpha) {
		alpha = SDL_max(0, SDL_min(255, alpha));
		dest->m_alpha = alpha;
		label_update_texture_alpha(dest);
	}
}


Label::Label(const Label* src) {
	label_init(&m_data);
	*this = src;
}
Label::Label() {
	label_init(&m_data);
}
Label::~Label() {
	label_clear(&m_data);
}

Label& Label::operator = (const Label& src) {
	label_cleartexture(&m_data);
	label_copy(&m_data, &src.m_data, sizeof(cLabel));
	return *this;
}
