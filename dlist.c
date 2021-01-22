#include "dlist.h"


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

