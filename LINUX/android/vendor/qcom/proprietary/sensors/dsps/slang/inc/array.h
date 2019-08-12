/* ===================================================================
** ** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: array.h
** DESC: sensor language - slang - generic array data structure
** Revision History
** ================================================================ */
#ifndef ARRAY_H
#define ARRAY_H

#include <stdbool.h>    // true, false
#include <stdint.h>
#include <string.h>     // NULL

typedef struct array {
    int32_t    high_water;             /* index of last used element: -1 == empty */
    int32_t    num_elements;
    void       *element[];             /* array of pointers */
} array;

array *new_array( int32_t num_elements);
array *append_array_element( array *pArray, void *pElement);
void  *get_array_element( const array *pArray, int elem_index);
array *expand_array( array *pArray);

#endif  // ARRAY_H



