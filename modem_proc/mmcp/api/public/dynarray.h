/**
  @file dynarray.h

  @brief  A class that implements a dynamic byte array, which grows as needed
          when data is added.
*/
/*
    Copyright (c) 2013-2014,2016 Qualcomm Technologies, Inc.
              All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

All ideas, data and information contained in or disclosed by this
document are confidential and proprietary information of
QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
By accepting this material the recipient agrees that this material
and the information contained therein are held in confidence and in
trust and will not be used, copied, reproduced in whole or in part,
nor its contents revealed in any manner to others without the express
written permission of QUALCOMM Technologies Incorporated.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/dynarray.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
*/


#ifndef _DYNARRAY_H_
#define _DYNARRAY_H_


#include "comdef.h"
#include <sys/types.h>
#include "ref_cnt_obj.h"


#define DYN_ARRAY_NO_MAX_SIZE ((size_t) 0x7fffffff)

typedef struct dyn_byte_array_t dyn_byte_array_t;



/*-------- dyn_byte_array_new --------*/
/**
@brief  Create a new dynamic byte array set.

@param[in]  initSize  Initial size (in bytes) of array.
@param[in]  maxSize   Maximum size (in bytes) to which the array is allowed to
                      grow.  Pass MRE_SET_NO_MAX_SIZE to let the array grow
                      without bounds.
@param[in]  pfnDtor   Pointer to destructor to call for this array.  If NULL is
                      passed, a default destructor will be used that just frees
                      the buffer memory.

@return     Pointer to a newly allocated dynamic array.
*/
dyn_byte_array_t *dyn_byte_array_new(
  size_t            initSize,
  size_t            maxSize,
  destructor_ptr    pfnDtor
);


/*-------- dyn_byte_array_clone --------*/
/**
@brief  Create an exact copy of a dynamic byte array.

@param[in]  pArray  Pointer to the array to make a copy of.

@return
  Pointer to a copy of the source array.  This will have a single reference
  on it.
*/
dyn_byte_array_t *dyn_byte_array_clone(
  dyn_byte_array_t  *pArray
);


/*-------- dyn_byte_array_clear --------*/
/**
@brief  "Clear" a byte array by setting its length to 0.

@param[in]  pArray  Array to clear.

@return
  Nothing.
*/
void  dyn_byte_array_clear(
  dyn_byte_array_t  *pArray
);


/*-------- dyn_byte_array_dtor --------*/
/**
@brief  Destructor for the basic dyn_byte_array_t object.  This should be called
        from any custom destructor for a derived object (such a destructor would
        be passed in the pfnDtor parameter of dyn_byte_array_new).

@param[in]  pObj  Pointer to dyn_byte_array_t object.

@return     None
*/
void dyn_byte_array_dtor(
  void  *pObj
);


/*-------- dyn_byte_array_length --------*/
/**
@brief  Get the number of bytes that are currently in the array.

@param[in]  pArray  Pointer to the dyn_byte_array_t object.

@return
  Number of bytes that are currently in the array.
*/
size_t dyn_byte_array_length(
  dyn_byte_array_t  *pArray
);


/*-------- dyn_byte_array_get_array_ptr --------*/
/**
@brief  Get a pointer to the first byte in the buffer of bytes that are
        in the array.

@param[in]  pArray  Pointer to the dyn_byte_array_t object.

@return     Pointer to the array of bytes contained in the buffer.  Note that
            this may change if bytes are added, so this function should be
            called any time the array is altered.
*/
byte *dyn_byte_array_get_array_ptr(
  dyn_byte_array_t  *pArray
);


/*-------- dyn_byte_array_get_ptr_to_offset --------*/
/**
@brief  Get a pointer to a particular byte offset within the array handled
        by this object.

@param[in]  pArray  Pointer to the dyn_byte_array_t object.

@return     Pointer to the byte at the given offset.
*/
byte *dyn_byte_array_get_ptr_to_offset(
  dyn_byte_array_t  *pArray,
  size_t            offset
);


/*=============================================================================
  Functions to append various data types to the array.  An append operation
  adds the bytes at the end of the array.
=============================================================================*/

/*-------- dyn_byte_array_append_byte --------*/
boolean dyn_byte_array_append_byte(
  dyn_byte_array_t  *pArray,
  byte              data
);

/*-------- dyn_byte_array_append_uint16 --------*/
boolean dyn_byte_array_append_uint16(
  dyn_byte_array_t  *pArray,
  uint16            data
);

/*-------- dyn_byte_array_append_uint32 --------*/
boolean dyn_byte_array_append_uint32(
  dyn_byte_array_t  *pArray,
  uint32            data
);

/*-------- dyn_byte_array_append_ptr --------*/
boolean dyn_byte_array_append_ptr(
  dyn_byte_array_t  *pArray,
  void const        *ptr
);

/*-------- dyn_byte_array_append_bytes --------*/
boolean dyn_byte_array_append_bytes(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t            count
);


/*=============================================================================
  Functions to insert various data types in the array.  An insert operation
  puts the inserted data in the array at a given offset, moving all bytes
  beyond that offset to a higher offset in order to make room.
=============================================================================*/

/*-------- dyn_byte_array_insert_byte --------*/
boolean dyn_byte_array_insert_byte(
  dyn_byte_array_t  *pArray,
  byte              data,
  size_t            offset
);

/*-------- dyn_byte_array_insert_uint16 --------*/
boolean dyn_byte_array_insert_uint16(
  dyn_byte_array_t  *pArray,
  uint16             data,
  size_t            offset
);

/*-------- dyn_byte_array_insert_uint32 --------*/
boolean dyn_byte_array_insert_uint32(
  dyn_byte_array_t  *pArray,
  uint32            data,
  size_t            offset
);

/*-------- dyn_byte_array_insert_ptr --------*/
boolean dyn_byte_array_insert_ptr(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t            offset
);

/*-------- dyn_byte_array_insert_bytes --------*/
boolean dyn_byte_array_insert_bytes(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t            count,
  size_t            offset
);


/*=============================================================================
  Functions to replace data in the array with various data types.  A replace
  operation overwrites the existing data.  The operation will fail if the
  replacement would write past the end of the existing data.
=============================================================================*/

/*-------- dyn_byte_array_replace_byte --------*/
boolean dyn_byte_array_replace_byte(
  dyn_byte_array_t  *pArray,
  byte              data,
  size_t            offset
);

/*-------- dyn_byte_array_replace_uint16 --------*/
boolean dyn_byte_array_replace_uint16(
  dyn_byte_array_t  *pArray,
  uint16             data,
  size_t            offset
);

/*-------- dyn_byte_array_replace_uint32 --------*/
boolean dyn_byte_array_replace_uint32(
  dyn_byte_array_t  *pArray,
  uint32            data,
  size_t            offset
);

/*-------- dyn_byte_array_replace_ptr --------*/
boolean dyn_byte_array_replace_ptr(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t            offset
);

/*-------- dyn_byte_array_replace_bytes --------*/
boolean dyn_byte_array_replace_bytes(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t            count,
  size_t            offset
);


/*=============================================================================
  Deletion from a dynamic array.
=============================================================================*/


/*-------- dyn_byte_array_delete_bytes --------*/
/**
@brief  Delete a given number of bytes at the given offset in the array.

@param[in]  pArray  Pointer to the dyn_byte_array_t object.
@param[in]  offset  Offset in the array at which to start the deletion.
@param[in]  count   Number of bytes to delete.

@return     TRUE if deletion was successful, FALSE if the requested deletion
            would go past the last byte in the array.
*/
boolean dyn_byte_array_delete_bytes(
  dyn_byte_array_t  *pArray,
  size_t            offset,
  size_t            count
);



/*=============================================================================
  For advanced use by clients requiring control over buffer reallocation
  strategy.

  Strategy for buffer growth.  The standard for growing the buffer is to double
  its size when it needs to grow.  This is generally optimal, since it allows
  us to build a large array from an initial small one with a minimum of
  allocations.  If a different strategy is desired, it can be set with this API.

  A growth strategy is a pointer to a function that takes the current size
  and the required size as parameters and returns the new desired size as its
  return value.  The strategy need not (but may) use the required size in the
  computation - the strategy will be called repeatedly with the previous result
  until the required size goal is met.

  Note that simply returning the required size will likely lead to excessive
  memory allocations when adding bytes to the buffer, as each byte that is added
  to a full buffer will require a reallocation of the buffer.
=============================================================================*/

typedef size_t  (*dynarray_growth_strategy)(size_t curSize, size_t requiredSize);

void dyn_byte_array_set_growth_strategy(
  dyn_byte_array_t          *pArray,
  dynarray_growth_strategy  pfnStrategy
);


#endif
