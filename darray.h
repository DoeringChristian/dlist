#ifndef DARRAY_H
#define DARRAY_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef unsigned int uint;

#define DARRAY_APPEND(_dst_p, _src_p, _num) darray_append(_dst_p, _src_p, _num, sizeof(*_src_p))
#define DARRAY_INSERT(_dst_p, _src_p, _num, _index) darray_insert(_dst_p, _src_p, _num, _index)

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

static inline size_t darray_size_for(size_t x){
    size_t i; 
    for(i = 1; i <= x; i*=2);
    return i;
}

static inline int darray_change_cap(struct darray *dst, size_t size){
    //size_t cap = darray_size_for(dst->cap >= size ? dst->cap : size);
    size_t cap = darray_size_for(size);
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

static inline int darray_append(struct darray *dst, void *src, size_t num, size_t elem_size){
    if(darray_change_cap(dst, dst->size + num*elem_size)){
        memmove(&dst->arr[dst->size], src, num);
        dst->size += num*elem_size;
        return 1;
    }
    return 0;
}

static inline void *darray_at(struct darray *src, uint index, size_t elem_size){
    return &src->arr[index*elem_size];
}

static inline uint darray_size(struct darray *src, size_t elem_size){
    return src->size / elem_size ;
}

static inline int darray_insert(struct darray *dst, void *src, size_t num, uint index, size_t elem_size){
    if(darray_change_cap(dst, dst->size + num*elem_size)){
        memmove(&dst->arr[(index+num)*elem_size], &dst->arr[index*elem_size], dst->size - (index)*elem_size);
        memmove(&dst->arr[(index)*elem_size], src, num*elem_size);
        dst->size += num*elem_size;
        return 1;
    }
    return 0;
}

static inline int darray_remove(struct darray *dst, size_t num, uint index, size_t elem_size){
    memmove(&dst->arr[(index*elem_size)], &dst->arr[(index+num)*elem_size], dst->size - (index*elem_size));
    dst->size -= num*elem_size;
    if(darray_change_cap(dst, dst->size)){
        return 1;
    }
    return 0;
}

#endif //DARRAY_H
