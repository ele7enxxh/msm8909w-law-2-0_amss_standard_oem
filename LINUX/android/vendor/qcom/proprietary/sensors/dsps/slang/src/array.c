/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: array.c
** DESC: sensor language - slang - generic array data structure
** Revision History
** ================================================================ */
#include "array.h"
#include "literals.h"
#include "sli.h"
#include "slmalloc.h"

#include <stdio.h>

/**
 * @name        new_array
 * @param[i]    num_elements - maximum number of elements
 * @retval      pointer to this array
 */
array *new_array( int32_t num_elements) {

    size_t size = sizeof( array) + ( num_elements * sizeof( array *));
    array *pArray = (array *)slmalloc( size, "new_array");

    if ( pArray == NULL) {
        FatalError( get_literal( OOM));
    }
    pArray->high_water   = -1;
    pArray->num_elements = num_elements;

    return pArray;
}

/**
 * @name        append_array_element
 * @param[i]    pArray - pointer to array
 * @param[i]    pElement - address of element to be appended
 * @retval      pointer to original OR expanded array
 */
array *append_array_element( array *pArray, void *pElement)
{
    array *retval = pArray;

    int32_t high_water = pArray->high_water;
    high_water++;
    if ( high_water >= pArray->num_elements) {
        pArray = expand_array( pArray);
        retval = pArray;
    }
    pArray->high_water = high_water;
    pArray->element[ high_water] = pElement;
    return retval;          /* NULL or pointer to expanded array */
}

/**
 * @name     GetArrayElement
 * @param[i]    pArray
 * @param[i]    elem_index
 */

void *get_array_element( const array *pArray, int elem_index)
{
    void *element = NULL;

    if ( pArray && pArray->high_water >= elem_index) {
        element = pArray->element[ elem_index];
    }

    return element;
}

/**
 * @name        expand_array
 * @param[i]    pArray - pointer to array
 * @retval      NULL or pointer to expanded array
 */
array *expand_array( array *pArray)
{
    int32_t num_elements = pArray->num_elements;
    size_t old_size    = sizeof( array) + ( num_elements * sizeof( array *));

    int32_t new_num_elem = num_elements * 2;
    if ( new_num_elem <= 0) {
        new_num_elem = 1;
    }
    size_t new_size    = sizeof( array) + ( new_num_elem * sizeof( array *));

    array *pNewArray = (array *)slmalloc( new_size, "array.2");

    if ( pNewArray == NULL) {
        FatalError( get_literal( OOM));
    }

    memcpy( pNewArray, pArray, old_size);
    pNewArray->num_elements = new_num_elem;

    pArray->high_water   = -1;          /* free old array*/
    pArray->num_elements = -1;
    slfree( pArray, "array.f1");

    return pNewArray;
}



