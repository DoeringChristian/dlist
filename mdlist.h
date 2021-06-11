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

#ifndef MDLIST_H
#define MDLIST_H

/*
 * Usage example:
 * 
 * struct test{
 *      MDLIST_ENTRY(struct test) node;
 *      int i;
 * };
 *
 * int main(){
        struct test *a = malloc(sizeof(struct test)); a->i = 0;
        struct test *b = malloc(sizeof(struct test)); b->i = 1;
        struct test *c = malloc(sizeof(struct test)); c->i = 2;


        MDLIST(struct test, node) list;
        MDLIST_INIT(&list, node);

        MDLIST_PUSH_BEFORE(&list, a, node);
        MDLIST_PUSH_BEFORE(&list, b, node);
        MDLIST_PUSH_BEFORE(&list, c, node);


        MDLIST_FOREACH(struct test *, t, &list, node){
            printf("%i\n", t->i);
        }

        struct test *tmp = NULL;
        MDLIST_FOREACH(struct test *, t, &list, node){
            if(tmp != NULL)
                free(tmp);
            MDLIST_POP(t, node);
            tmp = (void *)t;
        }
        free(tmp);




        return 0;
     * }
 *
 */


/*
 * Define a struct as an MDLIST_ENTRY 
 * 
 * @param _type type of the struct that should be defined as a ENTRY
 *          example:
 *          struct test{
 *              MDLIST_ENTRY(struct test) node;
 *          };
 */
#define MDLIST_ENTRY(_type)\
    struct {\
        _type *next, *prev;\
    }

/*
 * Anonymous struct that holds the head and tail of the list
 *
 * @param _type type of the list nodes
 * @param _field name of the ENTRY
 */
#define MDLIST(_type, _field)\
    struct {\
        MDLIST_ENTRY(_type) _field;\
    }

/*
 * Initializes list 
 *
 * @param _list_p pointer to the list that should be initialized
 * @param _field name of the ENTRY
 */
#define MDLIST_INIT(_list_p, _field){\
    (_list_p)->_field.prev = (void *)(_list_p);\
    (_list_p)->_field.next = (void *)(_list_p);\
}

/*
 * Pushes node after destination node
 *
 * @param _dst_p pointer to destination node after which to push _src_p
 * @param _src_p pointer to source node which should be pushed after _dst_p
 * @param _field name of the ENTRY
 */
#define MDLIST_PUSH_AFTER(_dst_p, _src_p, _field){\
    (_dst_p)->_field.next->_field.prev = (void *)(_src_p);\
    (_src_p)->_field.next = (void *)(_dst_p)->_field.next;\
    (_dst_p)->_field.next = (void *)(_src_p);\
    (_src_p)->_field.prev = (void *)(_dst_p);\
}

/*
 * Pushes node before destination node
 *
 * @param _dst_p pointer to the destination node before which to push _src_p
 * @param _src_p pointer to the source node which should be pushed before _dst_p
 * @param _field name of the ENTRY
 */
#define MDLIST_PUSH_BEFORE(_dst_p, _src_p, _field){\
    (_dst_p)->_field.prev->_field.next = (void *)(_src_p);\
    (_src_p)->_field.prev = (void *)(_dst_p)->_field.prev;\
    (_dst_p)->_field.prev = (void *)(_src_p);\
    (_src_p)->_field.next = (void *)(_dst_p);\
}

/*
 * Pops target node from the list
 *
 * @param _target_p pointer to the node which should be poped from the list
 * @param _field name of the ENTRY
 */
#define MDLIST_POP(_target_p, _field){\
    (_target_p)->_field.prev->_field.next = (_target_p)->_field.next;\
    (_target_p)->_field.next->_field.prev = (_target_p)->_field.prev;\
    (_target_p);\
}

/*
 * Iterates over the list in a forward direction
 *
 * @param _type type of the iteratior pointer
 * @param _iter name of the iterator pointer
 * @param _list_p pointer to the list
 * @param _field name of the ENTRY
 */
#define MDLIST_FOREACH(_type, _iter, _list_p, _field)\
    for(_type (_iter) = (_list_p)->_field.next; (_iter) != (void *)(_list_p); (_iter) = (_iter)->_field.next)

/*
 * Iterates over the list in a reverse direction
 *
 * @param _type type of the iteratior pointer
 * @param _iter name of the iterator pointer
 * @param _list_p pointer to the list
 * @param _field name of the ENTRY
 */
#define MDLIST_FOREACH_REV(_type, _iter, _list_p, _field)\
    for(_type (_iter) = (_list_p)->_field.prev; (_iter) != (void *)(_list_p); (_iter) = (_iter)->_field.prev)

#define MDLIST_EMPTY(_list_p, _field) ((_list_p)->_field.next == (void *)(_list_p))


#endif //MDLIST_H
