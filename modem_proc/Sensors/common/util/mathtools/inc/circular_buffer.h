#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

/*============================================================================
  @file circular_buffer.h

  Circular buffer utility header file 

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/mathtools/inc/circular_buffer.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-01-24  ad   initial version  

============================================================================*/

#include <stdint.h>
#include "matrix.h"

/* 
 * since BUF_SIZE is a power of 2,
 * (x & (BUF_SIZE-1)) == (x % BUF_SIZE)
 */
#define BUF_INDEX(index, buf_size) ((index)&(buf_size-1))

typedef struct
{
  matrix_type data;
  int32_t end;
  int32_t window_size;
  int32_t count;
} buffer_type;

int32_t buffer_mem_req(int32_t window_size, int32_t cols);
void buffer_reset(buffer_type *buffer, int32_t window_size, int32_t cols, void *mem);
void buffer_insert(buffer_type *buffer, int32_t const *new_data);
void buffer_sprd(buffer_type *buffer, int32_t *sprd_data);
void buffer_sum(buffer_type *buffer, int32_t *sum_data);
void buffer_sum_sq(buffer_type *buffer, int32_t *sum_sq_data, int32_t q_format);
int32_t buffer_full(buffer_type *buffer);
uint8_t buffer_num_samples( buffer_type *buffer);
void buffer_sum_and_sum_sq(buffer_type *buffer, int32_t *sum_data, int32_t *sum_sq_data, int32_t q_format);
int32_t buffer_size(int32_t window_size);

#endif /* CIRCULAR_BUFFER_H */
