/*============================================================================
  @file circular_buffer.c

  Circular buffer utility source file

  Copyright (c) 2010 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/mathtools/src/circular_buffer_uimg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-06-05 vy  Initial version, support uImage

============================================================================*/

#include "circular_buffer.h"
#include "fixed_point.h"


void buffer_sum(buffer_type *buffer, int32_t *sum_data)
{
  int32_t old_data, new_data, i, old_data_row, new_data_row;

  old_data_row = BUF_INDEX(buffer->end-buffer->window_size-1, buffer->data.rows);
  new_data_row = BUF_INDEX(buffer->end-1, buffer->data.rows);
  int32_t* row_old = MATRIX_ROW((&buffer->data), old_data_row);
  int32_t* row_new = MATRIX_ROW((&buffer->data), new_data_row);

  for (i=0; i<buffer->data.cols; i++)
  {
     old_data = (*(row_old + i));
     new_data = (*(row_new + i));
    sum_data[i] += (new_data - old_data);
  }
}


void buffer_insert(buffer_type *buffer, int32_t const *new_data)
{
  int32_t i, *row = MATRIX_ROW((&buffer->data), buffer->end);

  for (i=0; i<buffer->data.cols; i++)
  {
    (*(row + i)) = new_data[i];
  }

  buffer->end = BUF_INDEX(buffer->end+1, buffer->data.rows);

  if (buffer->count < buffer->data.rows)
  {
    buffer->count++;
  }
}


uint8_t buffer_num_samples( buffer_type *buffer)
{
  return(buffer->count);
}



void buffer_sum_sq(buffer_type *buffer, int32_t *sum_sq_data, int32_t q_format)
{
  int32_t old_data, new_data, i, old_data_row, new_data_row;

  old_data_row = BUF_INDEX(buffer->end-buffer->window_size-1, buffer->data.rows);
  new_data_row = BUF_INDEX(buffer->end-1, buffer->data.rows);

  int32_t* row_old = MATRIX_ROW((&buffer->data), old_data_row);
  int32_t* row_new = MATRIX_ROW((&buffer->data), new_data_row);

  for (i=0; i<buffer->data.cols; i++)
  {
    old_data = (*(row_old + i));
    new_data = (*(row_new + i));
    sum_sq_data[i] += (FX_MUL(new_data,new_data, q_format, q_format, q_format) -
                       FX_MUL(old_data,old_data, q_format, q_format, q_format));
  }
}

void buffer_sprd(buffer_type *buffer, int32_t *sprd_data)
{
  int32_t i, j, min_index, max_index, start_index, end_index;
  int32_t buf_min, buf_max, cur_val;

  end_index   = buffer->end;
  start_index = end_index-buffer->window_size;

  for (i=0; i<buffer->data.cols; i++)
  {
    max_index = start_index;
    min_index = max_index;

    buf_min = MATRIX_ELEM(&buffer->data, BUF_INDEX(max_index, buffer->data.rows), i);
    buf_max = buf_min;

    for (j=start_index+1; j<end_index; j++)
    {
      cur_val = MATRIX_ELEM(&buffer->data, BUF_INDEX(j, buffer->data.rows), i);

      if (cur_val < buf_min)
      {
        buf_min = cur_val;
        min_index = j;
      }

      if (cur_val > buf_max)
      {
        buf_max = cur_val;
        max_index = j;
      }
    }

    sprd_data[i] = FX_NEG(buf_max-buf_min, max_index<min_index);
  }
}


int32_t buffer_full(buffer_type *buffer)
{
  return (buffer->count >= buffer->window_size);
}


void buffer_sum_and_sum_sq(buffer_type *buffer, int32_t *sum_data, int32_t *sum_sq_data, int32_t q_format)
{
  int32_t old_data, new_data, i, old_data_row, new_data_row;

  old_data_row = BUF_INDEX(buffer->end-buffer->window_size-1, buffer->data.rows);
  new_data_row = BUF_INDEX(buffer->end-1, buffer->data.rows);

  for (i=0; i<buffer->data.cols; i++)
  {
    old_data = MATRIX_ELEM(&buffer->data, old_data_row, i);
    new_data = MATRIX_ELEM(&buffer->data, new_data_row, i);
    sum_data[i] += (new_data - old_data);
    sum_sq_data[i] += (FX_MUL(new_data,new_data, q_format, q_format, q_format) -
                       FX_MUL(old_data,old_data, q_format, q_format, q_format));
  }
}


/* round up to the nearest power of 2 */
/* we need at least one empty spot in the buffer */
int32_t buffer_size(int32_t window_size)
{
  window_size |= window_size >> 1;
  window_size |= window_size >> 2;
  window_size |= window_size >> 4;
  window_size |= window_size >> 8;
  window_size |= window_size >> 16;
  window_size++;

  return window_size;
}


void buffer_reset(buffer_type *buffer, int32_t window_size, int32_t cols, void *mem)
{
  int32_t rows = buffer_size(window_size);

  matrix_reset(&buffer->data, rows, cols, mem);

  buffer->window_size = window_size;
  buffer->end   = 0;
  buffer->count = 0;
  matrix_zero(&buffer->data);
}

int32_t buffer_mem_req(int32_t window_size, int32_t cols)
{
  return matrix_mem_req(buffer_size(window_size), cols);
}

