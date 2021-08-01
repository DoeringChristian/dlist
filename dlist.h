#ifndef DLIST_H
#define DLIST_H

#include <stddef.h>

/*
 * Iterate over the list.
 *
 * @param _list_p: pointer to the list to iterate over
 * @param _iter: name of the iterator
 */
#define dlist_foreach(_list_p, _iter)\
    for(struct dlist *_iter = (_list_p)->next; (_iter) != (_list_p); (_iter) = (_iter)->next)

/*
 * Iterate over the list in reverse.
 *
 * @param _list_p: pointer to the list to iterate over
 * @param _iter: name of the iterator
 */
#define dlist_foreach_reverse(_list_p, _iter)\
    for(struct dlist *_iter = (_list_p)->prev; (_iter) != (_list_p); (_iter) = (_iter)->prev)

/*
 * Iterate over the list and pop each element.
 *
 * @param _list_p: pointer to the list to iterate over
 * @param _iter: name of the iterator
 */
#define dlist_popeach(_list_p, _iter)\
    for(struct dlist *(_iter) = dlist_pop((_list_p)->next); (_iter) != (_list_p); (_iter) = dlist_pop((_list_p)->next))

/*
 * Iterate over the list in reverse and pop each element.
 *
 * @param _list_p: pointer to the list to iterate over
 * @param _iter: name of the iterator
 */
#define dlist_popeach_reverse(_list_p, _iter)\
    for(struct dlist *(_iter) = dlist_pop((_list_p)->prev); (_iter) != (_list_p); (_iter) = dlist_pop((_list_p)->prev))

/*
 * Iterate over the list and set _iter to be the content.
 *
 * @param _list_p: pointer to the list to iterate over.
 * @param _iter_p: pointer to a container used to iterate over the list.
 * @param _iter_node_p: pointer to a struct dlist used to iterate over the list.
 */
#define dlist_foreach_cont(_list_p, _iter_p, _iter_node_p)\
    for((_iter_node_p) = (_list_p)->next, (_iter_p) = (void *)(_iter_node_p)->cont; (_iter_node_p) != (_list_p); (_iter_node_p) = (_iter_node_p)->next, (_iter_p) = (void *)(_iter_node_p)->cont)

/*
 * Iterate over the list in reverse and set _iter to be the content.
 *
 * @param _list_p: pointer to the list to iterate over.
 * @param _iter_p: pointer to a container used to iterate over the list.
 * @param _iter_node_p: pointer to a struct dlist used to iterate over the list.
 */
#define dlist_foreach_cont_reverse(_list_p, _iter_p, _iter_node_p)\
    for((_iter_node_p) = (_list_p)->next, (_iter_p) = (void *)(_iter_node_p)->cont; (_iter_node_p) != (_list_p); (_iter_node_p) = (_iter_node_p)->next, (_iter_p) = (void *)(_iter_node_p)->cont)

/*
 * dlist is the node as well as the head of a doubly linked list.
 */
struct dlist{
    void *cont;
    struct dlist *next, *prev;
};

/*
 * dlist_init initializes list (next = prev = self)
 *
 * @param dst: pointer to the node
 * @param cont: pointer to the container struct NULL in case of list_head
 * @return: dst
 */
static inline struct dlist *dlist_init(struct dlist *dst, void *cont){
    dst->cont = cont;
    dst->next = dst;
    dst->prev = dst;
    return dst;
}

/*
 * initializes dlist as head
 *
 * @param dst: pointer to the lsit to be initialized
 * @return: dst
 */
static inline struct dlist *dlist_head_init(struct dlist *dst){
    return dlist_init(dst, NULL);
}

/*
 * dlist_pop pops the target node from the list
 *
 * if the target is the list then it will not change it
 *
 * @param target to be poped from the list
 * @return target if success NULL else
 */
static inline struct dlist *dlist_pop(struct dlist *target){
    if(target != NULL && target->prev != NULL && target->next != NULL){
        target->prev->next = target->next;
        target->next->prev = target->prev;
        return target;
    }
    return NULL;
}

/*
 * dlist_push_after pushes node after the dst node in the list
 *
 * @param dst destination node after which to push src
 * @param src source node to push after dst
 * @return src if success NULL else
 */
static inline struct dlist *dlist_push_after(struct dlist *dst, struct dlist *src){
    if(src != NULL && dst != NULL && dst->next != NULL){
        dst->next->prev = src;
        src->next = dst->next;
        dst->next = src;
        src->prev = dst;
        return src;
    }
    return NULL;
}

/*
 * dlist_push_before pushes node before the dst node in the list
 *
 * @param dst destination node before which to push src
 * @param src source node to push before dst
 * @return src if success NULL else
 */
static inline struct dlist *dlist_push_before(struct dlist *dst, struct dlist *src){
    if(src != NULL && dst != NULL && dst->prev != NULL){
        dst->prev->next = src;
        src->prev = dst->prev;
        dst->prev = src;
        src->next = dst;
        return src;
    }
    return NULL;
}

/*
 * dlist_push_back pushes node at the back of the list
 *
 * @param self pointer to the list
 * @param src list node to be pushed to the lsit
 * @return returns self if succes, NULL else
 */
static inline struct dlist *dlist_push_back(struct dlist *self, struct dlist *src){
    return dlist_push_after(self->prev, src);
}

/*
 * dlist_push_back pushes node at the front of the list
 *
 * @param self pointer to the list
 * @param src list node to be pushed to the lsit
 * @return returns self if succes, NULL else
 */
static inline struct dlist *dlist_push_front(struct dlist *self, struct dlist *src){
    return dlist_push_before(self->next, src);
}

/*
 * dlist_splice_after inserts the nodes of src after the dst node
 *
 * @param dst node after which to insert the nodes of src
 * @param src list, whichs nodes are to be inserted after dst
 * @return first element that has been inserted, if src is empty dst
 */
static inline struct dlist *dlist_splice_after(struct dlist *dst, struct dlist *src){
    if(dst != NULL && src != NULL && src->next != src){
        dst->next->prev = src->prev;
        src->prev->next = dst->next;
        src->next->prev = dst;
        dst->next = src->next;
        src->next = src->prev = src;
    }
    return dst->next;
}

/*
 * dlist_splice_before inserts the nodes of src before the dst node
 *
 * @param dst node after which to insert the nodes of src
 * @param src list, which nodes are to be inserted after dst
 * @retrun returns last inserted element if src is empty returns dst
 */
static inline struct dlist *dlist_splice_before(struct dlist *dst, struct dlist *src){
    if(dst != NULL && src != NULL && src->next != src){
        dst->prev->next = src->next;
        src->next->prev = dst->prev;
        src->prev->next = dst;
        dst->prev = src->prev;
        src->next = src->prev = src;
    }
    return dst->next;
}

/*
 * dlist_length returns length of list
 *
 * @param self pointer to list
 * @return length of list returns 0 if empty
 */
static inline size_t dlist_length(struct dlist *self){
    size_t i = 0;
    dlist_foreach(self, n) i++;
    return i;
}

/*
 * dlist_reverse reverses the content of a list
 *
 * @param self pointer to list that should be reversed
 * @return void
 */
static inline void dlist_reverse(struct dlist *self){
    struct dlist *tmp = self->next;
    self->next = self->prev;
    self->prev = tmp;
    for(struct dlist *node = self->prev; node != self; node = node->prev){
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
}
#endif //DLIST_H
