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

#include <stdio.h>
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
 * DLIST_FOREACH(n, &list){
 *      //do something with node which is a pointer to the current n
 * }
 * 
 * //iterating over list with container (and printing):
 * DLIST_FOREACH_CONT(struct container, n, &list, node){
 *      printf("%i\n", n->i);
 * }
 *
 * //poping each node in a list:
 * DLIST_POPEACH(n, &list){
 *      //n has been poped but can be used in the for loop
 * }
 *
 * //poping each node in a list as the container (and freeing it):
 * DLIST_POPEACH_CONT(struct container, n, &list, node){
 *      free(n);
 * }
 * 
 * //iterating over list with container and external definition of iterator in reverse:
 * struct container *tmp;
 * DLIST_FOREACH_REV_CONT_EXT(tmp, &list, node){
 *      printf("%i\n", tmp->i);
 * }
 *
 * //reversing a list:
 * dlist_reverse(&list);
 *
 * //inserting a list after a node
 * //assume that list2 is a list containing nodes
 * dlist_insert_after(&i2.node, &list2);
 * //list2 is now empty since the elements have been poped from it and appended to list
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
 * @param _iter name of the iterator. can be used in the forloop 
 * @param _list list over which to iterate
 */
#define DLIST_FOREACH(_iter, _list)\
        for(struct dlist *(_iter) = (_list)->next;\
        (_iter) != (_list);\
        (_iter) = (_iter)->next)

/*
 * DLIST_FOREACH_EXT same as DLIST_FOREACH but with external definition of _iter
 *
 * @param _iter pointer to iterator
 * @param _list list over which to iterate
 */
#define DLIST_FOREACH_EXT(_iter, _list)\
        for((_iter) = (_list)->next;\
        (_iter) != (_list);\
        (_iter) = (_iter)->next)

/*
 * DLIST_FOREACH_REV for loop macro iterates over the list in reverse direction
 *
 * @param _iter name of the iterator. can be used in the forloop 
 * @param _list list over which to iterate
 */
#define DLIST_FOREACH_REV(_iter, _list)\
        for(struct dlist *(_iter) = (_list)->prev;\
        (_iter) != (_list);\
        (_iter) = (_iter)->prev)

/*
 * DLIST_FOREACH_REV_EXT same as DLIST_FOREACH_REV but with external definition of _iter
 *
 * @param _iter pointer to the iterator
 * @param _list list over which to iterate
 */
#define DLIST_FOREACH_REV_EXT(_iter, _list)\
        for((_iter) = (_list)->prev;\
        (_iter) != (_list);\
        (_iter) = (_iter)->prev)

/*
 * DLIST_FOREACH_CONT for loop macro iterates over the list in forward direction where _iter is the containing struct
 *
 * @param _type type of the iterator, that can be used in the loop
 * @param _iter name of the iterator to use in the loop
 * @param _list list over which to iterate
 * @param _member member name of the node in that struct
 */
#define DLIST_FOREACH_CONT(_type, _iter, _list, _member)\
        for(_type *(_iter) = DLIST_CONT((_list)->next, _type, _member);\
        &(_iter)->_member != (_list);\
        (_iter) = DLIST_CONT((_iter)->_member.next, _type, _member))

/*
 * DLIST_FOREACH_CONT_EXT same as DLIST_FOREACH_CONT but with external definition for _iter
 *
 * @param _iter pointer to the iterator  
 * @param _list list over which to iterate
 * @param _member member name of the node in that struct
 */
#define DLIST_FOREACH_CONT_EXT(_iter, _list, _member)\
        for((_iter) = DLIST_CONT((_list)->next, typeof(*(_iter)), _member);\
        &(_iter)->_member != (_list);\
        (_iter) = DLIST_CONT((_iter)->_member.next, typeof(*(_iter)), _member))

/*
 * DLIST_FOREACH_REV_CONT for loop macro iterates over the list in backwards direction where _iter is the containing struct
 *
 * @param _type type of the iterator, that can be used in the loop
 * @param _iter name of the iterator. can be used in the loop to acces content of containing struct
 * @param _list list over which to iterate
 * @param _member member name of the node in that struct
 */
#define DLIST_FOREACH_REV_CONT(_type, _iter, _list, _member)\
        for(_type *(_iter) = DLIST_CONT((_list)->prev, _type, _member);\
        &(_iter)->_member != (_list);\
        (_iter) = DLIST_CONT((_iter)->_member.prev, _type, _member))

/*
 * DLIST_FOREACH_REV_CONT_EXT same as DLIST_FOREACH_REV_CONT but with external definition of _iter
 *
 * @param _iter name of the iterator. can be used in the loop to acces content of containing struct
 * @param _list list over which to iterate
 * @param _member member name of the node in that struct
 */
#define DLIST_FOREACH_REV_CONT_EXT(_iter, _list, _member)\
        for((_iter) = DLIST_CONT((_list)->prev, typeof(*(_iter)), _member);\
        &(_iter)->_member != (_list);\
        (_iter) = DLIST_CONT((_iter)->_member.prev, typeof(*(_iter)), _member))

/*
 * DLIST_POPEACH pops every element from the list in forward direction
 *
 * @param _iter name of the iterator. can be used in the loop
 * @param _list list whos elements should be poped
 */
#define DLIST_POPEACH(_iter, _list)\
        for(struct dlist *(_iter) = dlist_pop((_list)->next);\
        (_iter) != (_list);\
        (_iter) = dlist_pop((_list)->next))

/*
 * DLIST_POPEACH_EXT same as DLIST_POPEACH but with external definition for _iter
 *
 * @param _iter pointer to the iterator
 * @param _list list whos elements should be poped
 */
#define DLIST_POPEACH_EXT(_iter, _list)\
        for((_iter) = dlist_pop((_list)->next);\
        (_iter) != (_list);\
        (_iter) = dlist_pop((_list)->next))

/*
 * DLIST_POPEACH_REV pops every element from the list in reverse direction
 *
 * @param _iter name of the iterator. can be used in the loop
 * @param _list list whos elements should be poped
 */
#define DLIST_POPEACH_REV(_iter, _list)\
        for(struct dlist *(_iter) = dlist_pop((_list)->prev);\
        (_iter) != (_list);\
        (_iter) = dlist_pop((_list)->prev))

/*
 * DLIST_POPEACH_REV_EXT same as DLIST_POPEACH_REV but with external definition of _iter
 *
 * @param _iter pointer to the iterator
 * @param _list list whos elements should be poped
 */
#define DLIST_POPEACH_REV_EXT(_iter, _list)\
        for((_iter) = dlist_pop((_list)->prev);\
        (_iter) != (_list);\
        (_iter) = dlist_pop((_list)->prev))

/*
 * DLIST_POPEACH_CONT pops every element from the list where _iter is the containing struct (can be used to free list elements)
 *
 * @param _iter name of the iterator. can be used in the loop
 * @param _list list whos elements should be poped
 * @param _member member name of the node in that struct
 */
#define DLIST_POPEACH_CONT(_type, _iter, _list, _member)\
        for(_type *(_iter) = DLIST_CONT(dlist_pop((_list)->next), _type, _member);\
        &(_iter)->_member != (_list);\
        (_iter) = DLIST_CONT(dlist_pop((_list)->next), _type, _member))

/*
 * DLIST_POPEACH_CONT_EXT same as DLIST_POPEACH_CONT but with external definition of _iter
 *
 * @param _iter name of the iterator. can be used in the loop
 * @param _list list whos elements should be poped
 * @param _member member name of the node in that struct
 */
#define DLIST_POPEACH_CONT_EXT(_iter, _list, _member)\
        for((_iter) = DLIST_CONT(dlist_pop((_list)->next), typeof(*(_iter)), _member);\
        &(_iter)->_member != (_list);\
        (_iter) = DLIST_CONT(dlist_pop((_list)->next), typeof(*(_iter)), _member))

/*
 * DLIST_POPEACH_REV_CONT pops every element from the list where _iter is the containing struct (can be used to free list elements)
 *
 * @param _type type of the container in which the node is contained 
 * @param _iter name of the iterator. can be used in the loop
 * @param _list list whos elements should be poped
 * @param _member member name of the node in that struct
 */
#define DLIST_POPEACH_REV_CONT(_type, _iter, _list, _member)\
        for(_type *(_iter) = DLIST_CONT(dlist_pop((_list)->prev), _type, _member);\
        &(_iter)->_member != (_list);\
        (_iter) = DLIST_CONT(dlist_pop((_list)->prev), _type, _member))

/*
 * DLIST_POPEACH_REV_CONT_EXT same as DLIST_POPEACH_REV_CONT but with external definition of _iter
 *
 * @param _iter name of the iterator. can be used in the loop
 * @param _list list whos elements should be poped
 * @param _member member name of the node in that struct
 */
#define DLIST_POPEACH_REV_CONT_EXT(_iter, _list, _member)\
        for((_iter) = DLIST_CONT(dlist_pop((_list)->next), typeof(*(_iter)), _member);\
        &(_iter)->_member != (_list);\
        (_iter) = DLIST_CONT(dlist_pop((_list)->next), typeof(*(_iter)), _member))

typedef unsigned int uint;

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
 * @return head of the list if list is not empty, pointer to self else
 */
static inline struct dlist *dlist_head(struct dlist *self){
    if(self != NULL)
        return self->next;
    return NULL;
}

/*
 * dlist_tail returns tail of list
 *
 * @param self pointer to the list
 * @return tail of the list if list is not empty, pointer to self else
 */
static inline struct dlist *dlist_tail(struct dlist *self){
    if(self != NULL)
        return self->prev;
    return NULL;
}

/*
 * dlist_empty returns 0 if list is not empty
 *
 * @param self pointer to the list
 * @return 0 if list is not empty not 0 if list is empty (return self->next == self;)
 */

static inline int dlist_empty(struct dlist *self){
    return self->next == self;
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
    return dlist_push_after(self->next, src);
}

/*
 * dlist_push_back pushes node at the front of the list
 *
 * @param self pointer to the list
 * @param src list node to be pushed to the lsit
 * @return returns self if succes, NULL else
 */
static inline struct dlist *dlist_push_front(struct dlist *self, struct dlist *src){
    return dlist_push_before(self->prev, src);
}

/*
 * dlist_insert_after inserts the nodes of src after the dst node
 *
 * @param dst node after which to insert the nodes of src
 * @param src list, whichs nodes are to be inserted after dst
 * @return first element that has been inserted, if src is empty dst
 */
static inline struct dlist *dlist_insert_after(struct dlist *dst, struct dlist *src){
    struct dlist *tmp = dst;
    DLIST_POPEACH_REV_EXT(tmp, src){
        dlist_push_after(dst, tmp);
    }
    return tmp;
}

/*
 * dlist_insert_after inserts the nodes of src before the dst node
 *
 * @param dst node after which to insert the nodes of src
 * @param src list, which nodes are to be inserted after dst
 * @retrun returns last inserted element if src is empty returns dst
 */
static inline struct dlist *dlist_insert_before(struct dlist *dst, struct dlist *src){
    struct dlist *tmp = dst;
    DLIST_POPEACH_EXT(tmp, src){
        dlist_push_before(dst, tmp);
    }
    return tmp;
}

/*
 * dlist_length returns length of list
 *
 * @param self pointer to list
 * @return length of list returns 0 if empty
 */
static inline uint dlist_length(struct dlist *self){
    uint i = 0;
    DLIST_FOREACH(n, self) i++;
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
