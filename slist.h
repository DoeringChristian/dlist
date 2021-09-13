/*
Copyright (c) 2021 Christian Döring
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef SLIST_H
#define SLIST_H

#include <stddef.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef container_of
#define container_of(_ptr, _type, _member) ((_type *)((char*)(_ptr)-(char*)(&((_type*)0)->_member)))
#endif

#define slist_foreach(_list_p, _iter)\
    for(struct slist *_iter = (_list_p)->next; (_iter) != NULL; (_iter) = (_iter)->next)

#define slist_foreach_ext(_list_p, _iter_p)\
    for((_iter_p) = (_list_p)->next; (_iter_p) != NULL; (_iter_p) = (_iter_p)->next)

#define slist_popeach(_list_p, _iter)\
    for(struct slist *_iter = slist_pop_after(_list_p);_iter != NULL && _iter = slist_pop_after(_list_p))

#define slist_popeach_ext(_list_p, _iter_p)\
    for((_iter_p) = slist_pop_after(_list_p);(_iter_p) != NULL && (_iter_p) = slist_pop_after(_list_p))

struct slist{
    struct slist *next;
    void *cont;
};

static inline struct slist *slist_init(struct slist *dst, void *cont){
    dst->next = NULL;
    dst->cont = cont;
    return dst;
}

static inline struct slist *slist_pop_after(struct slist *src){
    if(src != NULL && src->next != NULL){
        struct slist *tmp = src->next;
        src->next = src->next->next;
        return tmp;
    }
    return NULL;
}

static inline struct slist *slist_push_after(struct slist *dst, struct slist *src){
    if(src != NULL && dst != NULL){
        src->next = dst->next;
        dst->next = src;
        return src;
    }
    return NULL;
}

static inline struct slist *slist_push_front(struct slist *self, struct slist *src){
    return slist_push_after(self, src);
}

static inline size_t slist_length(struct slist *self){
    size_t i = 0;

    return i;
}

#endif //SLIST_H
