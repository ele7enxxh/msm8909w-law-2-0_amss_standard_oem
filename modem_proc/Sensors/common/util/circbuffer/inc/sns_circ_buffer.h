#ifndef SNS_CIRC_BUFFER_H
#define SNS_CIRC_BUFFER_H

/*============================================================================
  @file sns_circ_buffer.h

  Generic circular buffer utility header file 

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

#include <stdint.h>

/* Circular buffer type */
typedef struct {
    int         size;   /* maximum number of elements           */
    int         count;  /* number of unread elements            */
    int         start;  /* index of oldest element              */
    int *       elems[1];  /* vector of elements                */
} sns_circ_buffer_type;

int sns_circ_buffer_get_mem_size(uint32_t max_items);

void sns_circ_buffer_init(sns_circ_buffer_type *buffer, uint32_t max_items);

int sns_circ_buffer_get_count( sns_circ_buffer_type *buffer );

int * sns_circ_buffer_get_item( sns_circ_buffer_type *buffer );

int * sns_circ_buffer_add_item(sns_circ_buffer_type *buffer, int *elem);

int sns_circ_buffer_is_full(sns_circ_buffer_type *buffer);


#endif /* SNS_CIRC_BUFFER_H */
