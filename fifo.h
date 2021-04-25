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

#ifndef FIFO_H
#define FIFO_H

#include <stddef.h>
#include <stdint.h>

typedef unsigned int uint;
/*
 * struct fifo to manage a fifo
 * 
 *    tail        haed
 *       |         |
 *       V         V
 * ----------------------
 * |00000##########00000|
 * ----------------------
 *       |<-data->|
 */
struct fifo {
    size_t size, tail, head;
    uint8_t *data;
};

static inline struct fifo *fifo_init(struct fifo *self, void *data, size_t size);
static inline struct fifo *fifo_copy(struct fifo *dst, const struct fifo *src);
static inline size_t fifo_size(const struct fifo *self);
static inline int fifo_write(struct fifo *self, void *src, size_t size);
static inline int fifo_read(struct fifo *self, void *dst, size_t size);
static inline int fifo_peek(const struct fifo *self, void *dst, size_t size);

static inline struct fifo *fifo_init(struct fifo *self, void *data, size_t size) {
    self->size = size;
    self->data = data;
    self->tail = 0;
    self->head = 0;
    return self;
};

static inline struct fifo *fifo_copy(struct fifo *dst, const struct fifo *src){
    if(dst->size >= src->size){
        fifo_peek(src, dst->data, src->size);
        dst->head = src->size;
        return dst;
    }
    return NULL;
}

static inline size_t fifo_size(const struct fifo *self){
    if (self->head >= self->tail)
        return self->head - self->tail;
    else
        return self->size + self->head - self->tail;
}

static inline int fifo_write(struct fifo *self, void *src, size_t size) {
    if (fifo_size(self) + size >= self->size)
        return 0;
    for (uint i = 0; i < size; i++) {
        self->data[(self->head + i) % self->size] = ((uint8_t *)src)[i];
    }
    self->head = (self->head + size) % self->size;
    return 1;
}

static inline int fifo_read(struct fifo *self, void *dst, size_t size) {
    if (fifo_size(self) < size)
        return 0;
    for (uint i = 0; i < size; i++)
        ((uint8_t *)dst)[i] = self->data[(self->tail + i) % self->size];
    self->tail = (self->tail + size) % self->size;
    return 1;
}

static inline int fifo_peek(const struct fifo *self, void *dst, size_t size){
    if(fifo_size(self) < size)
        return 0;
    for (uint i = 0; i < size; i++)
        ((uint8_t *)dst)[i] = self->data[(self->tail + 1) % self->size];
    return 1;
}

#endif //FIFO_H
