#include "Font.h"
#include "string.h"
#include "common.h"
#include "App.h"
#include "LinkedList.h"

static cString m_fontFile;

static cLinkedList* m_fontlist = 0;

// generates font...
void init_font() {
	m_fontlist = 0;
	g_app->AssetsFile("DejaVuSansMono-Bold.ttf", &m_fontFile);
}

void cleanup_font() {
	LinkedList_Delete(m_fontlist, Font_Free);
	m_fontlist = 0;
}

void Font_Free(cFont* ptr) {
	if (ptr) {
		if (ptr->font)
			TTF_CloseFont(ptr->font);
		x_free(ptr);
	}
}
void Font_Free(void* ptr) {
	Font_Free((cFont*)ptr);
}


// clear old fonts...
void clear_old(int max) {
	m_fontlist = LinkedList_RemoveAfter(m_fontlist, max, Font_Free);
}

TTF_Font* get_font(int pt_sz) {
	// see if there is one that matches already, otherwise generate...
	TTF_Font* retval;
	cLinkedList* l;
	l = m_fontlist;

	while (l) {
		cFont* f = (cFont*)l->m_ptr;
		if (f->ptsz == pt_sz)
			return f->font;
		l = l->pNext;
	}

	// did not find one... generate...
	retval = TTF_OpenFont(m_fontFile.m_buffer, pt_sz);
	if (retval) {
		// insert at front of list...
		cFont* f;
		f = (struct cFont *) x_malloc(sizeof(struct cFont));
		f->ptsz = pt_sz;
		f->font = retval;
		m_fontlist = LinkedList_Unshift(m_fontlist, f);
	}
	return retval;
}

TTF_Font* get_font_px(int px) {
	int z = font_pixel_to_pt(px);
	return get_font(z);
}

void clear_fonts(struct cFont* p) {
	// font p...
	m_fontlist = LinkedList_Remove(m_fontlist, p, Font_Free);
}
