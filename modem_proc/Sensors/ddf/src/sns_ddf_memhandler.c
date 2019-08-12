/** 
 * @file sns_ddf_memhandler.c
 * @brief A memory allocator object with built-in garbage collection.
 *  
 * Copyright (c) 2010, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ddf/src/sns_ddf_memhandler.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2015-03-26 ps   Updated license header
  2014-09-17 vy   Moved memhandler_free to BigImage
  2014-08-23 vy   Used new DDF apis to allocate/free mem supported in uImage
  
==============================================================================*/

#include "sns_ddf_memhandler.h"
#include "sns_ddf_util.h"


extern void sns_ddf_memhandler_init(sns_ddf_memhandler_s *memhandler);


void* sns_ddf_memhandler_malloc(sns_ddf_memhandler_s* memhandler, uint32_t size)
{
    sns_ddf_memhandler_s* link;
    uint32_t malloc_size;

    /* Check if this is the first allocation. If so, this is the head node and
     * there's no need to create a link, yet. */
    if(memhandler->buff == NULL)
    {
        return (sns_ddf_malloc(&memhandler->buff, size) == SNS_DDF_SUCCESS) ?
            memhandler->buff : NULL;
    }

    link = memhandler->next;

    /* Allocate a new node and its buffer. To optimize, we allocate both the
     * link and its buffer in one call to malloc and then set the buffer pointer
     * appropriately. */
    malloc_size = sizeof(sns_ddf_memhandler_s) + size;
    if(sns_ddf_malloc((void**)&memhandler->next, malloc_size) != SNS_DDF_SUCCESS)
    {
        return NULL;
    }

    memhandler->next->buff = (uint8_t*)memhandler->next + sizeof(sns_ddf_memhandler_s);
    memhandler->next->next = link;
    
    return memhandler->next->buff;
}


void sns_ddf_memhandler_free(sns_ddf_memhandler_s* memhandler)
{
    sns_ddf_memhandler_s* iter;

    if(memhandler->buff == NULL)
    {
        /* Nothing to free. */
        return;
    }

    /* Release the head's memory block. */
    sns_ddf_mfree(memhandler->buff);
    memhandler->buff = NULL;

    /* Traverse the list and release the various links. Note that buff must not
     * be released separately since it wasn't allocated dynamically. */
    iter = memhandler->next;
    while(iter != NULL)
    {
        sns_ddf_memhandler_s* temp = iter;
        iter = iter->next;
        sns_ddf_mfree(temp);
    }

    /* Reset all internals so they can be used safely next time. */
    sns_ddf_memhandler_init(memhandler);
}


