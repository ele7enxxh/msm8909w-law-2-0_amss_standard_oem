/*============================================================================
  @file sns_circ_buffer.c

  Generic circular buffer utility (for pointers) source file

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-18  pk   initial version  

============================================================================*/

#include "sns_circ_buffer.h"

/*=========================================================================
  FUNCTION: sns_circ_buffer_get_mem_size
  =========================================================================*/
/*!
  @brief Calculates the number of bytes needed to store the required number of
         items in the circular buffer

  @param[i] max_items: Desired buffer depth

  @return: Number of bytes needed by circular buffer
*/
/*=======================================================================*/
int sns_circ_buffer_get_mem_size(uint32_t max_items)
{
   int bufSize = sizeof(sns_circ_buffer_type) + (max_items-1)*sizeof(int*);
   return bufSize;
}

/*=========================================================================
  FUNCTION: sns_circ_buffer_get_mem_size
  =========================================================================*/
/*!
  @brief Calculates the number of bytes needed to store the required number of
         items in the circular buffer

  @param[i] max_items: Desired buffer depth

  @return: Number of bytes needed by buffer
*/
/*=======================================================================*/
void sns_circ_buffer_init(sns_circ_buffer_type *buffer, uint32_t max_items)
{
   buffer->size  = max_items;
   buffer->count  = 0;
   buffer->start = 0;
}

/*=========================================================================
  FUNCTION: sns_circ_buffer_get_count
  =========================================================================*/
/*!
  @brief Returns the number of unread elements in the circular buffer

  @param[i] buffer: Pointer to buffer

  @return: Number of bytes needed by circular buffer
*/
/*=======================================================================*/
int sns_circ_buffer_get_count( sns_circ_buffer_type *buffer )
{
   return buffer->count;
}

/*=========================================================================
  FUNCTION: sns_circ_buffer_get_item
  =========================================================================*/
/*!
  @brief Returns the oldest element in the buffer

  @param[i] buffer: Pointer to buffer

  @return: Oldest element in buffer
*/
/*=======================================================================*/
int * sns_circ_buffer_get_item( sns_circ_buffer_type *buffer )
{
   int *elem = buffer->elems[buffer->start];
   buffer->start = (buffer->start + 1) % buffer->size;
   --buffer->count;

   return elem;
}

/*=========================================================================
  FUNCTION: sns_circ_buffer_add_item
  =========================================================================*/
/*!
  @brief Writes an element into the buffer. In case of overflow, the oldest
         element is overwritten.

  @param[i] buffer: Pointer to buffer
  @param[i] elem: Item to be stored

  @return: Returns element that was overwritten in case of an overflow;
           NULL otherwise
*/
/*=======================================================================*/
int * sns_circ_buffer_add_item(sns_circ_buffer_type *buffer, int *elem)
{
   int end = (buffer->start + buffer->count) % buffer->size;
   int * overflowItem = 0;

   if (buffer->count == buffer->size)
   {
      // Get oldest item
      overflowItem = buffer->elems[buffer->start];

      // Adjust start index after overflow
      buffer->start = (buffer->start + 1) % buffer->size;
   }
   else
   {
      ++buffer->count;
   }
   buffer->elems[end] = elem;
   return overflowItem;
}

/*=========================================================================
  FUNCTION: sns_circ_buffer_is_full
  =========================================================================*/
/*!
  @brief Checks if circular buffer is full

  @param[i] buffer: Pointer to buffer

  @return: 1, if buffer is full; 0, otherwise
*/
/*=======================================================================*/
int sns_circ_buffer_is_full(sns_circ_buffer_type *buffer)
{
    return (buffer->count == buffer->size);
}
