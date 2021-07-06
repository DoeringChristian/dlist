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
#ifndef MDARRAY_H
#define MDARRAY_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*
 * |------+-----+------|
 * | size | cap | data |
 * |------+-----+------|
 *                  ^
 *                  |
 *              Pointer on whitch macros act upon.
 */


struct mdarray_header{
    size_t size, cap;
};

#define MDARRAY(_type) _type *

#define MDARRAY_HEADER(_arr) ((struct mdarray_header *)(((uint8_t *)(_arr)) - (sizeof(struct mdarray_header))))

#define MDARRAY_INIT(_arr) {struct mdarray_header *header = malloc(sizeof(struct mdarray_header)); header->size = 0; header->cap = 0; _arr = (void*)&header[1];}

#define MDARRAY_SIZE(_arr) (MDARRAY_HEADER(_arr)->size / sizeof(*(_arr)))

#if 0
#define _MDARRAY_INC(_arr, _delta)\
    struct mdarray_header *header = MDARRAY_HEADER(_arr);\
    size_t cap = mdarray_ciellog2(header->size+_delta);\
    if(cap != header->cap)\
        header = realloc(header, sizeof(struct mdarray_header)+cap*sizeof(*(_arr)));\

#define MDARRAY_APPEND(_arr, _elem) {\
    struct mdarray_header *header = MDARRAY_HEADER(_arr);\
    size_t cap = mdarray_ciellog2(header->size+1);\
    if(cap != header->cap)\
        header = realloc(header, sizeof(struct mdarray_header)+cap*sizeof(*(_arr)));\
    if(header != NULL){\
        header->cap = cap;\
        (_arr) = (void *)&header[1];\
        (_arr)[header->size] = (_elem);\
        header->size += 1;\
    }\
}

#define MDARRAY_INSERT(_arr, _elem, _index) {\
    struct mdarray_header *header = MDARRAY_HEADER(_arr);\
    size_t cap = mdarray_ciellog2(header->size+1);\
    if(cap != header->cap)\
        header = realloc(header, sizeof(struct mdarray_header)+cap*sizeof(*(_arr)));\
    if(header != NULL){\
        header->cap = cap;\
        (_arr) = (void *)&header[1];\
        memmove(&(_arr)[_index+1], &(_arr)[_index], sizeof(*(_arr)) * (header->size - (_index)));\
        (_arr)[_index] = (_elem);\
        header->size += 1;\
    }\
}

#define MDARRAY_INSERT_ARR(_arr, _src, _src_num, _index)\
    struct mdarray_header *header = MDARRAY_HEADER(_arr);\
    size_t cap = mdarray_ciellog2(header->size+(_src_num));\
    if(cap != header->cap)\
        header = realloc(header, sizeof(struct mdarray_header)+cap*sizeof(*(_arr)));\
    if(header != NULL){\
        header->cap = cap;\
        (_arr) = (void *)&header[1];\
        memmove(&(_arr)[_index+(_src_num)], &(_arr)[_index], sizeof(*(_arr)) * (header->size - (_index)));\
        memmove(&(_arr)[_index], (_src), (_src_num) * sizeof(*(_src)));\
        (_arr)[_index] = (_elem);\
        header->size += (_num);\
    }\

#define MDARRAY_REMOVE(_arr, _num, _index) {\
    struct mdarray_header *header = MDARRAY_HEADER(_arr);\
    memmove(&(_arr)[(_index)], &(_arr)[(_index) + (_num)], sizeof(*(_arr)) * (header->size - ((_index) + (_num))));\
    size_t cap = mdarray_ciellog2(header->size-(_num));\
    if(cap != header->cap)\
        header = realloc(header, sizeof(struct mdarray_header)+cap*sizeof(*(_arr)));\
    if(header != NULL){\
        header->cap = cap;\
        (_arr) = (void *)&header[1];\
        header->size -= (_num);\
    }\
}
#endif

#define MDARRAY_VA_HEAD(_x, ...) (_x)

#define MDARRAY_APPEND(_arr, _elem_p) mdarray_append((void **)&(_arr), &(_elem_p), sizeof(_elem_p))

#define MDARRAY_INSERT(_arr, _elem_p, _index) mdarray_insert((void **)&(_arr), _elem_p, sizeof(*(_elem_p)), (_index)*(sizeof(*(_arr))))

#define MDARRAY_INSERT_ARR(_arr, _elem_p, _num, _index) mdarray_insert((void **)&(_arr), _elem_p, sizeof(*(_elem_p))*(_num), (_index)*(sizeof(*(_arr))))

#define MDARRAY_INSERT_VA(_arr, _index, ...) {\
    typeof(*(_arr)) tmp[] = {__VA_ARGS__};\
    mdarray_insert((void **)&(_arr), tmp, sizeof(tmp), sizeof(*tmp) * (_index));\
}

#define MDARRAY_REMOVE(_arr, _index) mdarray_remove((void **)&(_arr), sizeof(*(_arr)), (_index)*sizeof(*(_arr)))

#define MDARRAY_FREE(_arr) mdarray_free((void **)&(_arr))



static inline size_t mdarray_ciellog2(size_t x){
    size_t i; 
    for(i = 1; i <= x; i*=2);
    return i;
}

static inline int mdarray_insert(void **dst, void *src, size_t src_size, size_t index){
    struct mdarray_header *header = MDARRAY_HEADER(*dst);
    size_t cap = mdarray_ciellog2(header->size+src_size);
    if(cap != header->cap)
        header = realloc(header, sizeof(struct mdarray_header)+cap);
    if(header != NULL){
        header->cap = cap;
        *dst = (void *)&header[1];
        memmove(((uint8_t *)*dst)+src_size+index, ((uint8_t *)*dst)+index, header->size-index);
        memmove(((uint8_t *)*dst)+index, src, src_size);
        header->size += src_size;
        return 1;
    }
    return 0;
}

static inline int mdarray_append(void **dst, void *src, size_t src_size){
    return mdarray_insert(dst, src, src_size, MDARRAY_HEADER(*dst)->size);
}

static inline int mdarray_remove(void **dst, size_t size, size_t index){
    struct mdarray_header *header = MDARRAY_HEADER(*dst);
    memmove(*dst+index, *dst+index+size, header->size-index);
    header->size -= size;
    size_t cap = mdarray_ciellog2(header->size-size);
    if(cap != header->cap)
        header = realloc(header, sizeof(struct mdarray_header)+cap);
    if(header != NULL){
        header->cap = cap;
        *dst = (void *)&header[1];
        return 1;
    }
    return 1;
}

void mdarray_free(void **dst){
    struct mdarray_header *header = MDARRAY_HEADER(*dst);
    free(header);
    *dst = NULL;
}

#endif //MDARRAY_H
