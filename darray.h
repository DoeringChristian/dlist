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

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*
 * Darray is an implementation of a Dynamic array in c.
 * Elements in darray can be indexed like a c array.
 * Handeling of size and cap is done in the background.
 *
 * +------+-----+------+
 * | size | cap | data |
 * +------+-----+------+
 *                  ^
 *                  |
 * +--------------------+
 * | darray(_type) _arr |
 * +--------------------+
 *          ^
 *          |
 * Pointer to the darray (_arr_p)
 *
 *
 * Usage example:
 *
 *   darray(int) test;
 *
 *   darray_init(&test);
 *
 *   int i1 = 1;
 *   int i2 = 2;
 *
 *   darray_append(&test, &i1);
 *   darray_append(&test, &i2);
 *   darray_append(&test, &i2);
 *   darray_append(&test, &i2);
 *   darray_append(&test, &i2);
 *   darray_append(&test, &i2);
 *   darray_append(&test, &i2);
 *   darray_insert(&test, &i1, 2);
 *
 *   // Elements can be insertet past the size limit of the darray.
 *   darray_insert(&test, &i1, 20);
 *
 *   // Arrays of elements can also be inserted.
 *   int a[] = {0,2,3,4};
 *   darray_insert_arr(&test, a, 4, 18);
 *
 *   // Elements can be removed. The array only shrinks after reaching half its size.
 *   darray_remove(&test, darray_size(&test)-1);
 *
 *   // Elements can be indexed like a c array.
 *   for(int i = 0;i < darray_size(&test); i++){
 *       printf("%i\n", test[i]);
 *   }
 *   darray_free(&test);
 *   return 0;
 *
 */


/*
 * Definitions of realloc, malloc, free for darray (can be changed to custom allocator)
 */
#define DARRAY_REALLOC(_void_p, _size) realloc(_void_p, _size)
#define DARRAY_MALLOC(_void_p) malloc(_void_p)
#define DARRAY_FREE(_void_p) free(_void_p)

/*
 * Growth factor of the darray (not yet tested)
 */
#define DARRAY_GROWTH_FACTOR 2

/*
 * Defines how much smaller the calculated capacity has to be to 
 * shrink the array than the current one.
 * darray shrinks when cap < header->cap / DARRAY_SHRINK_FACTOR 
 *
 * example: DARRAY_SHRINK_FACTOR = 2, header->size = 36, header->cap = 128, size = 4
 *          darray shrinks when cap < header->cap / DARRAY_SHRINK_FACTOR 
 *          cap = _darray_ciellog2(header->size - size) = 32
 *          => array shrinks.
 *
 *          (array shrinks when cap is smaller than 1/(DARRAY_SHRINK_FACTOR * 2) of header->cap)
 */
#define DARRAY_SHRINK_FACTOR 2

/*
 * Header structure of darray keeps track of the size and cap.
 *
 * @param align: uintmax_t for guarantueeing allignment of header[1] if sizeof(size_t) < sizeof(uintmax_t)
 * @param size: Stores the size of the darray
 * @param cap: Stores curent maximum capacity.
 */
struct darray_header{
    union{
        uintmax_t align;
        struct{
            size_t size, cap;
        };
    };
};

/*
 * Internal macro to get the head of an darray.
 */
#define DARRAY_HEADER(_arr) ((struct darray_header *)(((uint8_t *)(_arr)) - (sizeof(struct darray_header))))

/*
 * Macro for defining a darray as a pointer
 */
#define darray(_type) _type *

/*
 * Function to initialize the darray.
 *
 * This macro has to be called before using a darray since it allocates the memory for the header.
 *
 * @param _arr_p: pointer to the array which shall be initialized.
 * @param _cap:   initial capacity.
 *
 * @return pointer to the header of the array (NULL if failed)
 */
#define darray_init(_arr_p, _cap) _darray_init((void **)(_arr_p), (_cap) * sizeof(**(_arr_p)))

/*
 * Pushes an _elem to the back of the darray. 
 *
 * @param _arr_p: pointer to the darray.
 * @param _elem_p: Pointer to the element which should be pushed back.
 *
 * @return int: 1 if succes, 0 if failed
 */
#define darray_push_back(_arr_p, _elem_p) darray_push(_arr_p, _elem_p, darray_size((_arr_p)))


/*
 * Appends an array of _elem to the back of the array.
 *
 * @param _arr_p: Pointer to the darray.
 * @param _elem_p: Pointer to the elements which should be appended.
 * @param _num: number of elements which should be appended.
 *
 * @return int: 1 if succes, 0 if failed
 */

#define darray_append(_arr_p, _elem_p, _num) darray_insert(_arr_p, _elem_p, _num, darray_size((_arr_p)))

/*
 * Inserts an _elem into the darray at _index.
 *
 * Further elements after _index are moved to the right. Capacity is increaced if needed.
 * It is posible to insert elements after the end of the darray. The array is increaced accordingly.
 * Memory in that area is not set to 0 by default.
 *
 * @param _arr_p: pointer to the darray.
 * @param _elem_p: Pointer to the element which should be appended.
 * @param _index: index at which poisition the element should be inserted.
 *
 * @return int: 1 if succes, 0 if failed
 */
#define darray_push(_arr_p, _elem_p, _index) _darray_insert((void **)(_arr_p), _elem_p, sizeof(*(_elem_p)), (_index)*(sizeof(**(_arr_p))))

/*
 * Inserts an array of _elem into the darray at _index.
 *
 * Further elements after _index are moved to the right. Capacity is increaced if needed.
 * It is posible to insert elements after the end of the darray. The array is increaced accordingly.
 * Memory in that area is not set to 0 by default.
 *
 * @param _arr_p: pointer to the darray.
 * @param _elem_p: Pointer to the elements which should be appended.
 * @param _index: index at which poisition the elements should be inserted.
 *
 * @return int: 1 if succes, 0 if failed
 */
#define darray_insert(_arr_p, _elem_p, _num, _index) _darray_insert((void **)(_arr_p), _elem_p, sizeof(*(_elem_p))*(_num), (_index)*(sizeof(**(_arr_p))))

/*
 * Removes an element from the darray.
 *
 * Further elements after _index are moved to the left to close the hole.
 * Elements that are after the end of the darray can not be removed.
 *
 * @param _arr_p: Pointer to the darray.
 * @param _index: Index at which position the element should be removed.
 *
 * @return int: 1 if succes, 0 if failed
 */
#define darray_pop(_arr_p, _index) _darray_remove((void **)(_arr_p), sizeof(**(_arr_p)), (_index)*sizeof(**(_arr_p)))

/*
 * Removes _num elements from the darray
 *
 * Further elements after _index are moved to the left to close the hole.
 * Elements that are after the end of the darray can not be removed.
 *
 * @param _arr_p: Pointer to the darray.
 * @param _num: Number of elements to remove.
 * @param _index: Index at which position the element should be removed.
 *
 * @return int: 1 if succes, 0 if failed
 */
#define darray_remove(_arr_p, _num, _index) _darray_remove((void **)(_arr_p), sizeof(**(_arr_p)) * (_num), (_index)*sizeof(**(_arr_p)))

/*
 * Pops the laste element in the darray.
 *
 * @param_arr_p: Pointer to the darray.
 *
 * @return int: 1 if succes, 0 if failed
 */
#define darray_pop_back(_arr_p) darray_pop(_arr_p, darray_size(_arr_p)-1)

/*
 * Frees the content of array and sets its pointer to 0.
 *
 * @param _arr_p: Pointer to the darray.
 */
#define darray_free(_arr_p) _darray_free((void **)(_arr_p))

/*
 * Returns the size of the darray.
 *
 * @param _arr_p: Pointer to the darray.
 *
 * @return size_t: size of the array as in count of _elem
 */
#define darray_size(_arr_p) (DARRAY_HEADER(*(_arr_p))->size /sizeof(**(_arr_p)))

static inline size_t _darray_ciellog2(size_t x){
    size_t i; 
    for(i = 1; i <= x; i*=DARRAY_GROWTH_FACTOR);
    return i;
}

static inline struct darray_header *_darray_init(void **dst, size_t cap){
    struct darray_header *header = NULL;
    if((header = (struct darray_header *)DARRAY_MALLOC(sizeof(struct darray_header) + cap)) == NULL)
        return NULL;
    header->size = 0;
    header->cap = cap;
    *dst = (void *)&header[1];
    return header;
}

/*
 * _darray_insert function:
 *
 * +--------------+-------------+
 * | before index | after index |
 * +--------------+-------------+
 *                 ^
 *                 |
 *             index
 * 
 * +--------------+-----+-------------+
 * | before index | src | after index |
 * +--------------+-----+-------------+
 *                 ^
 *                 |
 *             index
 *
 * One problem with the current strategy is that if you want to insert and
 * realloc can't increace the current block it copies the content. If the then
 * the dynarray_insert function copies the "after index" block again leading to
 * worse performance.
 *
 * +--------------+-------------+
 * | before index | after index |
 * +--------------+-------------+
 *  |  realloc copies the block to another memory location.
 *  v
 * +--------------+-------------+------------+
 * | before index | after index | free space |
 * +--------------+-------------+------------+
 *                 |
 *                 +-----+ "after index" is copied again to make room for "src". 
 *                       | 
 *                       v
 * +--------------+-----+-------------+------------+
 * | before index | src | after index | free space |
 * +--------------+-----+-------------+------------+
 *
 * If we could know weather realloc copies the block or just expands it we
 * could then copy "after index" directly to the correct position.
 */

/*
 * Function to insert from src of size src_size in dst at index.
 *
 */
static inline int _darray_insert(void **dst, void *src, size_t src_size, size_t index){
    struct darray_header *header = DARRAY_HEADER(*dst);

    size_t target_size = header->size;
    if(index > header->size)
        target_size = index;

    size_t cap = _darray_ciellog2(target_size+src_size);
    // since header is a temporary pointer it should be ok to overwrite it with realloc.
    if(cap > header->cap){
        if((header = (struct darray_header *)DARRAY_REALLOC(header, sizeof(struct darray_header)+cap)) == NULL)
            return 0;
        header->cap = cap;
        *dst = (void *)&header[1];
    }
    // either cap was les or equal to header->cap therefore header is still the same and not NULL
    // or we sucessfully allocated new memory.
    // or we didn't and returned.

    // set memory to zero if index > header->size
    memset(((uint8_t *)*dst)+header->size, 0, target_size-header->size);
    memmove(((uint8_t *)*dst)+src_size+index, ((uint8_t *)*dst)+index, target_size-index);
    memmove(((uint8_t *)*dst)+index, src, src_size);
    header->size = target_size+src_size;
    return 1;
}


/*
 *
 * remove should not be able to fail even if realloc fails.
 */
static inline int _darray_remove(void **dst, size_t size, size_t index){
    struct darray_header *header = DARRAY_HEADER(*dst);
    if(index+size > header->size)
        return 0;
    memmove(((uint8_t *)*dst)+index, ((uint8_t *)*dst)+index+size, header->size-(index + size));
    header->size -= size;
    size_t cap = _darray_ciellog2(header->size);
    // Devided the header-size by DARRAY_SHRINK_FACTOR for histeresis.
    if(cap < header->cap / DARRAY_SHRINK_FACTOR){
        if((header = (struct darray_header *)DARRAY_REALLOC(header, sizeof(struct darray_header)+cap)) == NULL)
            return 1;
        header->cap = cap;
        *dst = (void *)&header[1];
    }
    return 1;
}

static inline void *_darray_pop_back(void **dst, size_t size){
    struct darray_header *header = DARRAY_HEADER(*dst);
    void *ret = ((uint8_t *)*dst)+header->size-size;
    if(!_darray_remove(dst, size, header->size-size)){
        ret = NULL;
    }
    return ret;
}


static inline void _darray_free(void **dst){
    struct darray_header *header = DARRAY_HEADER(*dst);
    DARRAY_FREE(header);
    *dst = NULL;
}

#endif //DARRAY_H
