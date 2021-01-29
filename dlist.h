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


#ifndef DLIST_H
#define DLIST_H

#include <stdlib.h>

/*
 * _ptr: pointer to member of struct
 * _type: type of struct
 * _member: name of member
 * get container _type struct from _ptr who is _member of _type
*/
#define DLIST_CONTAINER(_ptr, _type, _member) ((_type *)((char*)(_ptr)-(char*)(&((_type*)0)->_member)))

/*
 * container of node can be optained with dlist_container(_node, _type, _member)
 */
#define DLIST_FOR_EACH(_dlist_ptr, _node) for(struct dlist_node *(_node) = (_dlist_ptr)->head; (_node) != NULL; (_node) = (_node)->next)


/*
 * node struct of dlist. listed struct should have it as member
*/
struct dlist_node{
  struct dlist_node *next, *prev;
};

/*
 * dlist header
 */
struct dlist{
  struct dlist_node *head, *tail;
  unsigned int size;
};

/*
 * initialize dlist_node to set next and prev to NULL. has to be called if node is poped from other list and inserted again.
 */
struct dlist_node *dlist_node_init(struct dlist_node *self);

struct dlist *dlist_init(struct dlist *self);

void dlist_push_before(struct dlist *self, struct dlist_node *dst, struct dlist_node *src);

void dlist_push_after(struct dlist *self, struct dlist_node *dst, struct dlist_node *src);

struct dlist_node *dlist_pop(struct dlist *self, struct dlist_node *target);






//code section in header for easier deployment

#ifdef DLIST_HEADER

struct dlist_node *dlist_node_init(struct dlist_node *self){
  self->next = NULL;
  self->prev = NULL;
  return self;
}

struct dlist *dlist_init(struct dlist *self){
  self->head = NULL;
  self->tail = NULL;
  self->size = 0;
  return self;
}

void dlist_push_before(struct dlist *self, struct dlist_node *dst, struct dlist_node *src){
  if(self == NULL || src == NULL){
    return ;
  }
  if(self->head == NULL || self->tail == NULL || self->size == 0){
    src->next = NULL;
    src->prev = NULL;
    self->head = src;
    self->tail = src;
    self->size = 1;
  }
  else if(dst == self->head){
    src->prev = NULL;
    self->head->prev = src;
    src->next = self->head;
    self->head = src;
    self->size++;
  }
  else{
    src->next = dst;
    src->prev = dst->prev;
    dst->prev->next = src;
    dst->prev = src;
    self->size++;
  }
}

void dlist_push_after(struct dlist *self, struct dlist_node *dst, struct dlist_node *src){
  if(self == NULL || src == NULL){
    return ;
  }
  if(self->head == NULL || self->tail == NULL || self->size == 0){
    src->next = NULL;
    src->prev = NULL;
    self->head = src;
    self->tail = src;
    self->size = 1;
  }
  else if(dst == self->tail){
    src->next = NULL;
    self->tail->next = src;
    src->prev = self->tail;
    self->tail = src;
    self->size++;
  }
  else{
    src->prev = dst;
    src->next = dst->next;
    dst->next->prev = src;
    dst->next = src;
    self->size++;
  }
}

struct dlist_node *dlist_pop(struct dlist *self, struct dlist_node *target){
  if(self == NULL || target == NULL){
    return NULL;
  }
  if(target->prev != NULL){
    target->prev->next = target->next;
  }
  if(target->next != NULL){
    target->next->prev = target->prev;
  }
  self->size--;
  return target;
}

#endif


#endif //DLIST_H
