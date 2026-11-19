#include "LinkedList.h"
#include "common.h"
/*
typedef struct cLinkedList {
	void* m_ptr;
	struct LinkedListItem* pNext;
} cLinkedList;
*/
cLinkedList *LinkedList_Init(cLinkedList* dest) {
	if (!dest)
		dest = (cLinkedList *)x_malloc(sizeof(cLinkedList));
	SDL_memset(dest, 0, sizeof(cLinkedList));
	return dest;
}

void LinkedList_Delete(cLinkedList* src, void (*free_callback)(void* ptr)) {
	cLinkedList* p;
	if (free_callback)
		LinkedList_Map(src, free_callback);
	
	while (src) {
		p = src->pNext;
		x_free(src);
		src = p;
	}
}

cLinkedList* LinkedList_Unshift(cLinkedList* src, void* ptr) {
	cLinkedList* retval;
	retval = (cLinkedList*)x_malloc(sizeof(cLinkedList));
	retval->pNext = src;
	retval->m_ptr = ptr;
	return retval;
}
cLinkedList* LinkedList_Push(cLinkedList* src, void* ptr) {
	
	cLinkedList* p;
	p = (cLinkedList*)x_malloc(sizeof(cLinkedList));
	p->pNext = NULL;
	p->m_ptr = ptr;

	if (src) {
		cLinkedList* i = src;
		while (i->pNext) {
			i = i->pNext;
		}
		i->pNext = p;
	} else {
		src = p;
	}
	return src;
}

void LinkedList_Map(cLinkedList* src, void (*callback)(void* ptr)) {
	while (src) {
		callback(src->m_ptr);
		src = src->pNext;
	}
}

cLinkedList* LinkedList_MapFlash(cLinkedList* src, int (*callback)(void* ptr), void (*free_callback)(void* ptr)) {
	cLinkedList* prev = src, * i, *c;
	i = src;
	while (i) {
		int res = callback(i->m_ptr);
		if (res > 0) {
			// delete...
			if (free_callback)
				free_callback(i->m_ptr);

			c = i;

			if (prev == i)
				src = prev = prev->pNext;
			else
				prev->pNext = i->pNext;

			i = c->pNext;

			x_free(c);
		} else {
			prev = i;
			i = i->pNext;
		}
	}
	return src;
}


cLinkedList *LinkedList_Remove(cLinkedList* src, void* item, void (*free_callback)(void* ptr)) {
	cLinkedList* prev, * i;
	prev = NULL;
	i = src;
	while (i) {
		if (i->m_ptr == item) {
			if (free_callback)
				free_callback(i->m_ptr);
			if (prev == NULL)
				src = i->pNext;
			else
				prev->pNext = i->pNext;
			x_free(i);
			break;
		}
		prev = i;
		i = i->pNext;
	}
	return src;
}

cLinkedList* LinkedList_RemoveAfter(cLinkedList* src, int nth, void (*free_callback)(void* ptr)) {
	cLinkedList* i = src, *prev;
	prev = 0;
	while (i && nth) {
		prev = i;
		i = i->pNext;
		nth--;
	}

	if (i && nth == 0) {
		LinkedList_Delete(i, free_callback);
		if (prev) {
			prev->pNext = 0;
		} else {
			src = 0;
		}
	}

	return src;
}


LinkedList::LinkedList(void (*free_callback)(void *p)) {
    m_list =0;
}
LinkedList::~LinkedList() {
    if (m_list)
        LinkedList_Delete(m_list, m_free_callback);
}

int LinkedList_Size(cLinkedList* src) {
	int retval = 0;
	while (src) {
		retval++;
		src = src->pNext;
	}
	return retval;
}
