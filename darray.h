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

#ifndef DARRAY_H
#define DARRAY_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef unsigned int uint;

#define DARRAY_APPEND(_arr_p, _elem_p) darray_append((_arr_p), (_elem_p), sizeof(*(_elem_p)))

#define DARRAY_INSERT(_arr_p, _elem_p, _index) darray_insert((_arr_p), (_elem_p), sizeof(*(_elem_p)), (_index)*(sizeof(*(_elem_p))))

#define DARRAY_AT(_arr_p, _type, _index) (_type *)(&(_arr_p)->arr[(_index)*sizeof(_type)])

#define DARRAY_REMOVE(_arr_p, _type, _index) darray_remove(_arr_p, (sizeof(_type)), (_index)*(sizeof(_type)))

#define DARRAY_SIZE(_arr_p, _type) ((_arr_p)->size / (sizeof(_type)))


struct darray{
    uint8_t *arr;
    size_t cap, size;
};

static inline struct darray *darray_init(struct darray *dst){
    dst->arr = NULL;
    dst->cap = 0;
    dst->size = 0;
    return dst;
}

static inline void darray_free(struct darray *dst){
    free(dst->arr);
    dst->arr = NULL;
    dst->size = 0;
    dst->cap = 0;
}

static inline size_t darray_ensure(size_t x){
    size_t i; 
    for(i = 1; i <= x; i*=2);
    return i;
}

static inline int darray_change_cap(struct darray *dst, size_t size){
    //size_t cap = darray_size_for(dst->cap >= size ? dst->cap : size);
    size_t cap = darray_ensure(size);
    void *tmp = NULL;
    if(cap != dst->cap)
        tmp = realloc(dst->arr, cap);
    else
        return 1;
    if(tmp != NULL){
        dst->arr = tmp;
        dst->cap = cap;
        return 1;
    }
    return 0;
}

static inline int darray_append(struct darray *dst, void *src, size_t size){
    if(darray_change_cap(dst, dst->size + size)){
        memmove(&dst->arr[dst->size], src, size);
        dst->size += size;
        return 1;
    }
    return 0;
}

static inline void *darray_at(struct darray *src, uint index){
    return &src->arr[index];
}

static inline int darray_insert(struct darray *dst, void *src, size_t size, uint index){
    if(darray_change_cap(dst, dst->size + size)){
        memmove(&dst->arr[size+index], &dst->arr[index], dst->size - index);
        memmove(&dst->arr[index], src, size);
        dst->size += size;
        return 1;
    }
    return 0;
}

static inline int darray_remove(struct darray *dst, size_t size, uint index){
    memmove(&dst->arr[index], &dst->arr[index+size], size);
    dst->size -= size;
    if(darray_change_cap(dst, dst->size)){
        return 1;
    }
    return 0;
}

#endif //DARRAY_H
