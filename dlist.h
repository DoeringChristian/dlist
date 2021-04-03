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

#include <stddef.h>

/*
 * Usage example:
 *
 * // define a struct which has a dlist node
 * struct container{
 *      struct dlist node;
 *      int i;
 * };
 * ...
 * //define a dlist and initialize it
 * struct dlist list;
 * dlist_init(&list);
 *
 * //alternatively with malloc
 * struct dlist *list = dlist_init(malloc(sizeof(struct dlist)));
 *
 * //initialize instances of the container struct
 * struct container i1 = {.i = 1};
 * struct container i2 = {.i = 2};
 * struct container i3 = {.i = 3};
 *
 * //pushing to the back of the list:
 * dlist_push_after(dlist_tail(&list), &i1.node);
 * //alternatively:
 * dlist_push_after(list.prev, &i1.node);
 *
 * //pushing to the front of the list:
 * dlist_push_before(dlist_head(&list), &i2.node);
 *
 * //iterating over a list:
 * DLIST_FOREACH(&list, n){
 *      //do something with node which is a pointer to the current n
 * }
 * 
 * //iterating over list with container (and printing):
 * DLIST_FOREACH_CONT(&list, n, struct container, node){
 *      printf("%i\n", n->i);
 * }
 *
 * //poping each node in a list:
 * DLIST_POPEACH(&list, n){
 *      //n has been poped but can be used in the for loop
 * }
 *
 * //poping each node in a list as the container (and freeing it):
 * DLIST_POPEACH_CONT(&list, n, struct container, node){
 *      free(n);
 * }
 */


/*
 * DLIST_CONT returns the containing struct in which a dlist node is contained in
 *
 * @param _ptr pointer to a dlist node 
 * @param _type type of the container in which the node is contained 
 * @param _member member name of the node in that struct
 * @return container struct in which the node is contained within
 */
#define DLIST_CONT(_ptr, _type, _member) ((_type *)((char*)(_ptr)-(char*)(&((_type*)0)->_member)))

/*
 * DLIST_FOREACH for loop macro iterates over the list in forward direction
 *
 * @param _list list over which to iterate
 * @param _iter name of the iterator. can be used in the forloop 
 */
#define DLIST_FOREACH(_list, _iter) for(struct dlist *(_iter) = (_list)->next; (_iter) != (_list); (_iter) = (_iter)->next)

/*
 * DLIST_FOREACH_CONT for loop macro iterates over the list in forward direction where _iter is the containing struct
 *
 * @param _list list over which to iterate
 * @param _iter name of the iterator. can be used in the loop to acces content of containing struct
 * @param _type type of the container in which the node is contained 
 * @param _member member name of the node in that struct
 */
#define DLIST_FOREACH_CONT(_list, _iter, _type, _member) for(_type *(_iter) = DLIST_CONT((_list)->next, _type, _member); &(_iter)->_member != (_list); (_iter) = DLIST_CONT((_iter)->_member.next, _type, _member))

/*
 * DLIST_POPEACH pops every element from the list
 *
 * @param _list list whos elements should be poped
 * @param _iter name of the iterator. can be used in the loop
 */
#define DLIST_POPEACH(_list, _iter) for(struct dlist *(_iter) = dlist_pop((_list)->next); (_iter) != (_list); (_iter) = dlist_pop((_list)->next))

/*
 * DLIST_POPEACH_CONT pops every element from the list where _iter is the containing struct (can be used to free list elements)
 *
 * @param _list list whos elements should be poped
 * @param _iter name of the iterator. can be used in the loop
 * @param _type type of the container in which the node is contained 
 * @param _member member name of the node in that struct
 */
#define DLIST_POPEACH_CONT(_list, _iter, _type, _member) for(_type *(_iter) = DLIST_CONT(dlist_pop((_list)->next), _type, _member); &(_iter)->_member != (_list); (_iter) = DLIST_CONT(dlist_pop((_list)->next), _type, _member))

/*
 * double linked list set up in a loop
 *
 * used as node and list
 */
struct dlist{
    struct dlist *next, *prev;
};

/*
 * dlist_init initializes list (next = prev = self)
 *
 * @param self pointer to the list to be initialized
 * @return returns self
 */
static inline struct dlist *dlist_init(struct dlist *self){
    self->next = self->prev = self;
    return self;
}

/*
 * dlist_head returns head of list
 *
 * @param self pointer to the list
 * @return head of the list if list is not empty NULL else
 */
static inline struct dlist *dlist_head(struct dlist *self){
    if(self != NULL && self->next != self)
        return self->next;
    return NULL;
}

/*
 * dlist_tail returns tail of list
 *
 * @param self pointer to the list
 * @return tail of the list if list is not empty NULL else
 */
static inline struct dlist *dlist_tail(struct dlist *self){
    if(self != NULL && self->prev != self)
        return self->prev;
    return NULL;
}

/*
 * dlist_next returns next element of list
 *
 * @param self pointer to the list
 * @param src pointer to the node of which to get the next element
 * @return next element of the list if not last element NULL else
 */
static inline struct dlist *dlist_next(struct dlist *self, struct dlist *src){
    if(self != NULL && src != NULL && src->next != self)
        return src->next;
    return NULL;
}

/*
 * dlist_prev returns previous element of list
 *
 * @param self pointer to the list
 * @param src pointer to the node of which to get the previous element
 * @return prev element of the list if not first element NULL else
 */
static inline struct dlist *dlist_prev(struct dlist *self, struct dlist *src){
    if(self != NULL && src != NULL && src->prev != self)
        return src->prev;
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
    if(src != NULL && dst != NULL){
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
    if(src != NULL && dst != NULL){
        dst->prev->next = src;
        src->prev = dst->prev;
        dst->prev = src;
        src->next = dst;
        return src;
    }
    return NULL;
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
    if(target != NULL){
        target->prev->next = target->next;
        target->next->prev = target->prev;
        return target;
    }
    return NULL;
}

#endif //DLIST_H
