/**
  @file dynarray.c

  @brief  A class that implements a dynamic byte array, which grows as needed
          when data is added.
*/

/*
    Copyright (c) 2013-2014,2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/dynarray.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "dynarray.h"
#include "atomic_ops.h"
#include "err.h"
#include "modem_mem.h"
#include <stringl/stringl.h>


struct dyn_byte_array_t
{
  _REF_CNT_OBJ;
  size_t                     curSize;    /* current size (in bytes) of the array */
  size_t                     bufSize;    /* number of bytes the buffer can hold */
  size_t                     maxSize;    /* maximum size (in bytes) of the array */
  dynarray_growth_strategy   pfnStrategy;/* growth strategy */
  destructor_ptr             pfnDtor;    /* pointer to the destructor for the array */
  byte                      *pBuf;       /* pointer to buffer */
};


static  atomic_word_t memBlocksAllocated;


/*-------- dyn_byte_array_malloc --------*/
static void * dyn_byte_array_malloc(
  size_t  size
)
{
  void  *pMem;

  pMem = modem_mem_alloc(size, MODEM_MEM_CLIENT_MMODE);

  if (NULL == pMem)
  {
    ERR_FATAL("Unable to allocate %d bytes in dyn_byte_array_alloc", size, 0, 0);
  }

  atomic_inc(&memBlocksAllocated);

  return pMem;
}


/*-------- dyn_byte_array_free --------*/
static void dyn_byte_array_free(
  void  *pMem
)
{
  atomic_dec(&memBlocksAllocated);
  modem_mem_free(pMem, MODEM_MEM_CLIENT_MMODE);
}


/*-------- dyn_byte_array_doubling_growth_strategy --------*/
static size_t dyn_byte_array_doubling_growth_strategy(
  size_t  curSize,
  size_t  requiredSize
)
{
  return curSize * 2;
}


/*-------- dyn_byte_array_new --------*/
dyn_byte_array_t * dyn_byte_array_new(
  size_t            initSize,
  size_t            maxSize,
  destructor_ptr    pfnDtor
)
{
  dyn_byte_array_t *pArray;

  pArray = (dyn_byte_array_t *)dyn_byte_array_malloc(sizeof(dyn_byte_array_t));

  if (NULL == pfnDtor)
  {
    pfnDtor = dyn_byte_array_dtor;
  }
  ref_cnt_obj_init(pArray, pfnDtor);

  pArray->curSize     = 0;
  pArray->bufSize     = MIN(initSize, maxSize);
  pArray->maxSize     = maxSize;
  pArray->pfnStrategy = dyn_byte_array_doubling_growth_strategy;
  pArray->pBuf        = dyn_byte_array_malloc(pArray->bufSize);

  return pArray;
}


/*-------- dyn_byte_array_clone --------*/
dyn_byte_array_t *dyn_byte_array_clone(
  dyn_byte_array_t  *pArray
)
{
  dyn_byte_array_t *pClone;

  pClone = (dyn_byte_array_t *)dyn_byte_array_malloc(sizeof(dyn_byte_array_t));

  *pClone = *pArray;

  pClone->_rcobj_.ref_count.value = 1;
  pClone->pBuf = dyn_byte_array_malloc(pClone->bufSize);
  memscpy(pClone->pBuf, pClone->curSize, pArray->pBuf, pArray->curSize);

  return pClone;
}


/*-------- dyn_byte_array_clear --------*/
void  dyn_byte_array_clear(
  dyn_byte_array_t  *pArray
)
{
  pArray->curSize = 0;
}


/*-------- dyn_byte_array_dtor --------*/
void dyn_byte_array_dtor(
  void  *pObj
)
{
  dyn_byte_array_t  *pArray = (dyn_byte_array_t *) pObj;

  if (NULL != pArray->pBuf)
  {
    dyn_byte_array_free(pArray->pBuf);
  }
  dyn_byte_array_free(pArray);
}


/*-------- dyn_byte_array_length --------*/
size_t dyn_byte_array_length(
  dyn_byte_array_t  *pArray
)
{
  return pArray->curSize;
}


/*-------- dyn_byte_array_get_array_ptr --------*/
byte * dyn_byte_array_get_array_ptr(
  dyn_byte_array_t  *pArray
)
{
  return pArray->pBuf;
}


/*-------- dyn_byte_array_get_ptr_to_offset --------*/
byte * dyn_byte_array_get_ptr_to_offset(
  dyn_byte_array_t  *pArray,
  size_t             offset
)
{
  return (offset >= pArray->curSize) ? NULL : dyn_byte_array_get_array_ptr(pArray) + offset;
}


/*-------- dyn_byte_array_get_ptr_to_offset_internal --------*/
static byte * dyn_byte_array_get_ptr_to_offset_internal(
  dyn_byte_array_t  *pArray,
  size_t             offset
)
{
  return dyn_byte_array_get_array_ptr(pArray) + offset;
}


/*-------- dyn_byte_array_set_growth_strategy --------*/
void dyn_byte_array_set_growth_strategy(
  dyn_byte_array_t          *pArray,
  dynarray_growth_strategy   pfnStrategy
)
{
  pArray->pfnStrategy = pfnStrategy;
}


/*-------- dyn_byte_array_grow_buffer --------*/
static boolean dyn_byte_array_grow_buffer(
  dyn_byte_array_t  *pArray,
  size_t             requiredSize
)
{
  size_t   newSize;
  byte    *pNewBuf;

  newSize = MIN(requiredSize, pArray->maxSize);
  pNewBuf = dyn_byte_array_malloc(newSize);

  /*  Copy the array to the new buffer and free the old.
   */
  memscpy(pNewBuf, pArray->curSize, pArray->pBuf, pArray->curSize);
  dyn_byte_array_free(pArray->pBuf);
  pArray->pBuf = pNewBuf;
  pArray->bufSize = newSize;

  return TRUE;
}


/*-------- dyn_byte_array_ensure_space --------*/
static boolean dyn_byte_array_ensure_space(
  dyn_byte_array_t  *pArray,
  size_t             space
)
{
  size_t  requiredSize = pArray->curSize + space;
  size_t  requestedSize;

  /*  If there is still space in the buffer, there is no need to grow it.
   */
  if (requiredSize <= pArray->bufSize)
  {
    return TRUE;
  }

  if (pArray->bufSize >= pArray->maxSize)
  {
    return FALSE;
  }

  /*  Call the strategy function until we get a size greater than
   *  the required size.
   */
  requestedSize = pArray->bufSize;

  do
  {
    requestedSize = pArray->pfnStrategy(requestedSize, requiredSize);
  } while (requestedSize < requiredSize);

  return dyn_byte_array_grow_buffer(pArray, requestedSize);
}


/*-------- dyn_byte_array_delete_bytes --------*/
boolean dyn_byte_array_delete_bytes(
  dyn_byte_array_t  *pArray,
  size_t             offset,
  size_t             count
)
{
  boolean succeeded;
  size_t  offsetOfNextByte = offset + count;

  succeeded = (offsetOfNextByte <= pArray->curSize);

  if (succeeded)
  {
    byte    *pDelete = dyn_byte_array_get_ptr_to_offset_internal(pArray, offset);
    size_t  bytesToMove = pArray->curSize - offsetOfNextByte;
    memsmove(pDelete, bytesToMove, pDelete + count, bytesToMove);
    pArray->curSize -= count;
  }

  return succeeded;
}


/*-------- dyn_byte_array_insert_bytes --------*/
boolean dyn_byte_array_insert_bytes(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t             count,
  size_t             offset
)
{
  boolean succeeded;

  succeeded = (   offset <= pArray->curSize
               && dyn_byte_array_ensure_space(pArray, count) );

  if (succeeded)
  {
    byte  *pInsert = dyn_byte_array_get_ptr_to_offset_internal(pArray, offset);
    size_t  bytesToMove = pArray->curSize - offset;
    if (offset != pArray->curSize)
    {
      memsmove(pInsert + count, bytesToMove, pInsert, bytesToMove);
    }
    memscpy(pInsert, count, ptr, count);
    pArray->curSize += count;
  }

  return succeeded;
}


/*-------- dyn_byte_array_insert_byte --------*/
boolean dyn_byte_array_insert_byte(
  dyn_byte_array_t  *pArray,
  byte               data,
  size_t             offset
)
{
  return dyn_byte_array_insert_bytes(pArray, &data, sizeof(byte), offset);
}


/*-------- dyn_byte_array_insert_uint16 --------*/
boolean dyn_byte_array_insert_uint16(
  dyn_byte_array_t  *pArray,
  uint16             data,
  size_t             offset
)
{
  return dyn_byte_array_insert_bytes(pArray, &data, sizeof(uint16), offset);
}


/*-------- dyn_byte_array_insert_uint32 --------*/
boolean dyn_byte_array_insert_uint32(
  dyn_byte_array_t  *pArray,
  uint32             data,
  size_t             offset
)
{
  return dyn_byte_array_insert_bytes(pArray, &data, sizeof(uint32), offset);
}


/*-------- dyn_byte_array_insert_ptr --------*/
boolean dyn_byte_array_insert_ptr(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t             offset
)
{
  return dyn_byte_array_insert_bytes(pArray, &ptr, sizeof(void *), offset);
}


/*-------- dyn_byte_array_append_bytes --------*/
boolean dyn_byte_array_append_bytes(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t             count
)
{
  return dyn_byte_array_insert_bytes(pArray, ptr, count, pArray->curSize);
}


/*-------- dyn_byte_array_append_byte --------*/
boolean dyn_byte_array_append_byte(
  dyn_byte_array_t  *pArray,
  byte               data
)
{
  return dyn_byte_array_append_bytes(pArray, &data, sizeof(byte));
}


/*-------- dyn_byte_array_append_uint16 --------*/
boolean dyn_byte_array_append_uint16(
  dyn_byte_array_t  *pArray,
  uint16             data
)
{
  return dyn_byte_array_append_bytes(pArray, &data, sizeof(uint16));
}


/*-------- dyn_byte_array_append_uint32 --------*/
boolean dyn_byte_array_append_uint32(
  dyn_byte_array_t  *pArray,
  uint32             data
)
{
  return dyn_byte_array_append_bytes(pArray, &data, sizeof(uint32));
}


/*-------- dyn_byte_array_append_ptr --------*/
boolean dyn_byte_array_append_ptr(
  dyn_byte_array_t  *pArray,
  void const        *ptr
)
{
  return dyn_byte_array_append_bytes(pArray, &ptr, sizeof(void *));
}


/*-------- dyn_byte_array_replace_bytes --------*/
boolean dyn_byte_array_replace_bytes(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t             count,
  size_t             offset
)
{
  boolean succeeded;

  succeeded = (offset + count <= pArray->curSize);

  if (succeeded)
  {
    byte  *pInsert = dyn_byte_array_get_ptr_to_offset_internal(pArray, offset);
    memscpy(pInsert, count, ptr, count);
  }

  return succeeded;
}


/*-------- dyn_byte_array_replace_byte --------*/
boolean dyn_byte_array_replace_byte(
  dyn_byte_array_t  *pArray,
  byte               data,
  size_t             offset
)
{
  return dyn_byte_array_replace_bytes(pArray, &data, sizeof(byte), offset);
}


/*-------- dyn_byte_array_replace_uint16 --------*/
boolean dyn_byte_array_replace_uint16(
  dyn_byte_array_t  *pArray,
  uint16             data,
  size_t             offset
)
{
  return dyn_byte_array_replace_bytes(pArray, &data, sizeof(uint16), offset);
}


/*-------- dyn_byte_array_replace_uint32 --------*/
boolean dyn_byte_array_replace_uint32(
  dyn_byte_array_t  *pArray,
  uint32             data,
  size_t             offset
)
{
  return dyn_byte_array_replace_bytes(pArray, &data, sizeof(uint32), offset);
}


/*-------- dyn_byte_array_replace_ptr --------*/
boolean dyn_byte_array_replace_ptr(
  dyn_byte_array_t  *pArray,
  void const        *ptr,
  size_t             offset
  )
{
  return dyn_byte_array_replace_bytes(pArray, &ptr, sizeof(void *), offset);
}

