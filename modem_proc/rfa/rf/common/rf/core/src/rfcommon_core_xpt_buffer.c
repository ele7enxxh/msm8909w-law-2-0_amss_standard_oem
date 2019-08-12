
/*!
  @file
  rfcommon_core_xpt_buffer.c

  @brief
  Common buffer pool for storing xPT capture data

  @details

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_xpt_buffer.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/14   kai     Add API getting the max number of buffers used
07/24/13   cri     DPD capture buffer reuse support   
04/22/13   jmf     Fix F3 Msg
03/11/13   sar     Updated file for APQ, GNSS only target.
09/17/12   cri     Initial check-in
10/30/12   cri     Updated for SM storage of DPD capture and results

============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "comdef.h"
#include "modem_mem.h"
#include "rfcommon_msg.h"
#include "rfcommon_core_xpt_buffer.h"

/* Global Declarations */
static xpt_buffer_status_type sample_buffer_status[RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT];
static xpt_buffer_status_type results_buffer_status[RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT];
static rf_common_xpt_buffer_id_type capture_index_table[RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT];
static uint8 current_buffer_index;
static uint8 num_xpt_dpd_buffers_in_use = 0;
static uint8 max_num_xpt_dpd_buffers_used = 0;


/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Initialize DPD sample capture buffer
 
  @details
  Clear out status, buffer ID, and pointer information

  @param void
 
  @return void
*/
void rfcommon_core_xpt_init_buffers(void)
{
  int i;

  for(i=0;i<RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT;i++)
  {
    sample_buffer_status[i] = BUFFER_FREE;
  }

  for(i=0;i<RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT;i++)
  {
    results_buffer_status[i] = BUFFER_FREE;
  }

  for(i=0;i<RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT;i++)
  {
    capture_index_table[i].capt_buf_id = 0xFF;
    capture_index_table[i].res_buf_id = 0xFF;
  }
  current_buffer_index = 0;
  num_xpt_dpd_buffers_in_use = 0;
}

/*!
  @brief  
  Get a capture and results buffer
 
  @details
  Search the table of free buffers to find an available index. Reserve the free index
  and pass it back to the caller. 

  @param buffer_ids
 
  @return boolean
*/
boolean rfcommon_core_xpt_get_buffers(rf_common_xpt_buffer_id_type* buffer_ids)
{
  boolean status = TRUE;

  buffer_ids->capt_buf_id = 0xFF;
  buffer_ids->res_buf_id = 0xFF;

  if(sample_buffer_status[current_buffer_index] != BUFFER_FREE ||
     results_buffer_status[current_buffer_index] != BUFFER_FREE )
  {
    RF_MSG_1(RF_ERROR,"rfcommon_core_allocate_buffer - "
                    "Desired buffer %d still in use",current_buffer_index);
    status = FALSE;
  }

  if(status == TRUE)
  {
    buffer_ids->capt_buf_id = current_buffer_index;
    buffer_ids->res_buf_id = current_buffer_index;
    sample_buffer_status[current_buffer_index] = BUFFER_IN_USE;
    results_buffer_status[current_buffer_index] = BUFFER_IN_USE;

    current_buffer_index++;
    if(current_buffer_index == RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT)
    {
      current_buffer_index = 0;
    }
    num_xpt_dpd_buffers_in_use ++;
    if(num_xpt_dpd_buffers_in_use > max_num_xpt_dpd_buffers_used)
      max_num_xpt_dpd_buffers_used = num_xpt_dpd_buffers_in_use;
  }

  return status;
}

/*!
  @brief  
  Free a capture buffer
 
  @details
  Set the capture buffer to free status so the next request for an index can use it

  @param buffer_id

  @return boolean
*/
boolean rfcommon_core_xpt_release_capture_buffer(uint8 buffer_id)
{
  if(buffer_id >= RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT)
  {
    RF_MSG_1(RF_ERROR,"rfcommon_core_release_capture_buffer - "
                      "invalid buffer ID %d", buffer_id); 
    return FALSE;
  }

  if(sample_buffer_status[buffer_id] == BUFFER_FREE)
  {
    RF_MSG_1(RF_HIGH,"rfcommon_core_release_capture_buffer - "
                     "buffer ID %d already free", buffer_id); 
  }

  sample_buffer_status[buffer_id] = BUFFER_FREE;
  capture_index_table[buffer_id].capt_buf_id = 0xFF;

  return TRUE;
}

/*!
  @brief  
  Free a results buffer

  @details
  Set the results buffer to free status so the next request for an index can use it

  @param buffer_id
 
  @return boolean
*/
boolean rfcommon_core_xpt_release_results_buffer(uint8 buffer_id)
{
  if(buffer_id >= RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT)
  {
    RF_MSG_1(RF_ERROR,"rfcommon_core_release_capture_buffer - "
                      "invalid buffer ID %d", buffer_id); 
    return FALSE;
  }

  if(results_buffer_status[buffer_id] == BUFFER_FREE)
  {
    RF_MSG_1(RF_HIGH,"rfcommon_core_release_capture_buffer - "
                     "buffer ID %d already free", buffer_id); 
  }

  results_buffer_status[buffer_id] = BUFFER_FREE;
  capture_index_table[buffer_id].res_buf_id = 0xFF;
  num_xpt_dpd_buffers_in_use--;

  return TRUE;
}

/*!
  @brief  
  Free all DPD sample capture and results buffers
 
  @details
  Clear out status, buffer ID, and pointer information

  @param void
 
  @return void
*/
void rfcommon_core_xpt_free_all_buffers(void)
{
  rfcommon_core_xpt_init_buffers();
}

/*!
  @brief  
  Get the max number of buffers used
 
  @details
  Get the max number of buffers used

  @param void
 
  @return uint8
  Max number of buffers used
*/
uint8 rfcommon_core_xpt_get_max_buffers_used(void)
{
  return max_num_xpt_dpd_buffers_used;
}

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
