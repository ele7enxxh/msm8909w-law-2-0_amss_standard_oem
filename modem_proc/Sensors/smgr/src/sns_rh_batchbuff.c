/*=============================================================================
  @file sns_rh_batchbuff.c

  This file implements the Batching Buffer module of Request Handler task.

*******************************************************************************
* Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_batchbuff.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when              who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-11  bd   Move reserved fifo size to uimage
  2016-02-08  gju  Update sns_batchbuff header file name
  2015-12-15  bd   Put sns_rh_batchbuff_handle_conn_busy to uimage
  2015-11-23  pn   Limits number of batchbuffs for reports on busy connections
  2015-11-13  pn   Does not call sns_rh_send_ready_reports() from sns_rh_batchbuff_flush()
  2015-10-28  bd   Use number of samples instead of end_timestamp to limit sample cnt
  2015-10-20  bd   Add reserved fifo size support
  2015-10-19  pn   Report timer is used only for event-sensor batching
  2015-10-12  bd   Add batchbuff block tracking
  2015-09-18  pn   Uses timestamp of last sample sent to determine next indication
  2015-09-11  bd   do not flush to SAM on TCM low mem callback
  2015-09-11  pn   - Half-full batchbuff check is limited to SSC clients
                   - Treats uImg batchbuff allocation fail the same as lowmem callback
                   - Consumes all samples from bigImg batchbuffs before uImg batchbuffs
  2015-09-03  bd   Separate batchbuffer for bimg and uimg
  2015-09-01  bd   Flush indication to sam only if there are enough samples
  2015-08-21  bd   Code cleanup
  2015-08-18  bd   -Move batchbuff low request handling to uimg
                   -Remove batchbuff block before going to suspend mode
  2015-08-13  bd   Change print information
  2015-08-12  bd   Probe elements in the queue before accessing the element
  2015-07-30  bd   Initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_batchbuff.h"
#include "sns_rh.h"
#include "sns_rh_sol.h"
#include "sns_rh_batchbuff.h"
#include "sns_rh_main_uimg.h"
#include "sns_rh_rpt_gen.h"
#include "sns_rh_util.h"
#include "sns_smgr_main.h"
#include "sns_smgr.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//#define SNS_SMGR_BATCHBUFF_DEBUG
#ifdef SNS_SMGR_BATCHBUFF_DEBUG
#define SNS_SMGR_BATCH_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_BATCH_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_BATCH_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_BATCH_DBG3 SNS_SMGR_PRINTF3
#define SNS_SMGR_BATCH_DBG4 SNS_SMGR_PRINTF4
#else
#define SNS_SMGR_BATCH_DBG0(level,msg)
#define SNS_SMGR_BATCH_DBG1(level,msg,p1)
#define SNS_SMGR_BATCH_DBG2(level,msg,p1,p2)
#define SNS_SMGR_BATCH_DBG3(level,msg,p1,p2,p3)
#define SNS_SMGR_BATCH_DBG4(level,msg,p1,p2,p3,p4)
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Macro
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_cb

===========================================================================*/
/*!
  @brief This function is the callback function for batch buffer memory low signal.

  @detail

  @param[i]  mem_type  - The memory type that got low
  @param[i]  threshold    - The threshold associated with the callback function
                            
  @return 
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC 
void sns_rh_batchbuff_cb(sns_batchbuff_mem_type_e mem_type, uint32_t threshold)
{
  SNS_SMGR_BATCH_DBG3(HIGH, "batchbuff_cb - threshold:%u #ubb:%u #bbb:%u",threshold,
    sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_UIMG_MEM),
    sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_BIMG_MEM));
  sns_rh_signal_me(SNS_RH_BATCHBUFF_LOW_SIG);
}

/*===========================================================================
  
  FUNCTION:   sns_rh_batchbuff_init
  
===========================================================================*/
/*!
  @brief This function registers the callback function for batch buffer memory low.

  @detail
 
  @param[i] 
                             
  @return 
 */
/*=========================================================================*/
void sns_rh_batchbuff_init(void)
{
  sns_batchbuff_cb_register(SNS_BATCHBUFF_CLIENT_SMGR,
    SNS_BATCHBUFF_UIMG_MEM,sns_rh_batchbuff_cb,SNS_BATCHBUFF_UIMG_CB_THRESHOLD1);
  sns_batchbuff_cb_register(SNS_BATCHBUFF_CLIENT_SMGR,
    SNS_BATCHBUFF_UIMG_MEM,sns_rh_batchbuff_cb,SNS_BATCHBUFF_UIMG_CB_THRESHOLD2);
  sns_batchbuff_cb_register(SNS_BATCHBUFF_CLIENT_SMGR,
    SNS_BATCHBUFF_BIMG_MEM,sns_rh_batchbuff_cb,SNS_BATCHBUFF_BIMG_CB_THRESHOLD1);
  sns_batchbuff_cb_register(SNS_BATCHBUFF_CLIENT_SMGR,
    SNS_BATCHBUFF_BIMG_MEM,sns_rh_batchbuff_cb,SNS_BATCHBUFF_BIMG_CB_THRESHOLD2);
}


/*===========================================================================
  
  FUNCTION:   sns_rh_batchbuff_print
  
===========================================================================*/
/*!
  @brief This function shows number of batchbuff blks used by each report in rh.

  @detail This is only for debugging purpose
 
  @param[i] 
                             
  @return 
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_batchbuff_print(void)
{
#ifdef SNS_SMGR_BATCHBUFF_DEBUG_TEMP
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_q_s* rpt_queue_ptr = sns_rh_get_report_queue();
  SMGR_FOR_EACH_Q_ITEM( rpt_queue_ptr, rpt_ptr, rpt_link )
  {
    if ( SNS_RH_REPORT_IS_BATCHING(rpt_ptr) )
    {
      uint16_t i;
      SNS_SMGR_PRINTF2(
        HIGH, "print - rpt_id=%u/0x%x", 
        rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle);
      RH_FOR_EACH_REPORT_ITEM(rpt_ptr,i)
      {
        sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
        SNS_SMGR_PRINTF3(
          LOW, "print - sensor=%u #ubb=%u #bbb=%u", 
          SMGR_SENSOR_ID(item_ptr->ddf_sensor_ptr->sensor_ptr),
          sns_q_cnt(&item_ptr->ubatch_buffer_queue),
          sns_q_cnt(&item_ptr->bbatch_buffer_queue));
      }
    }
  }
  SNS_SMGR_PRINTF2(
    MED, "print - avail #ubb=%u #bbb=%u", 
    sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_UIMG_MEM), 
    sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_BIMG_MEM));
#endif
}
/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_drop_block

===========================================================================*/
/*!
  @brief Drop the batchbuff block with oldest samples

  @detail

  @param[i]  

  @return   none
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC void sns_rh_batchbuff_drop_block(
  sns_rh_rpt_item_s* item_ptr)
{
  sns_rh_batchbuff_s* bb_ptr = NULL;
  if ( sns_q_cnt(&item_ptr->bbatch_buffer_queue) > 0 )
  {
    sns_rh_leave_island();
    bb_ptr = sns_q_check(&item_ptr->bbatch_buffer_queue);
  }
  else if ( sns_q_cnt(&item_ptr->ubatch_buffer_queue) > 0 )
  {
    bb_ptr = sns_q_check(&item_ptr->ubatch_buffer_queue);
  }
  if ( bb_ptr != NULL )
  {
    item_ptr->num_saved_samples -= bb_ptr->num_samples;
    item_ptr->parent_report_ptr->num_saved_samples -= bb_ptr->num_samples;
    sns_q_delete(&bb_ptr->q_link);
    sns_batchbuff_free(SNS_BATCHBUFF_CLIENT_SMGR, bb_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_trim_nonwakeup

===========================================================================*/
/*!
  @brief Remove samples exceeding reserved size for nonwakeup sensors

  @detail

  @param[i]  

  @return   none
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC void sns_rh_batchbuff_trim_nonwakeup(void)
{
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  
  SMGR_FOR_EACH_Q_ITEM( rpt_queue_ptr, rpt_ptr, rpt_link )
  {
    if ( SNS_RH_REPORT_IS_BATCHING(rpt_ptr) && 
         sns_rh_ok_to_trim_nonwakeup_buffer(rpt_ptr))
    {
      uint8_t i;
      RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
      {
        sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
        uint32_t reserved_fifo_size = 
          item_ptr->ddf_sensor_ptr->reserved_sample_cnt_nonwakeup;
        while ( item_ptr->num_saved_samples > 
          (reserved_fifo_size + SNS_RH_BATCHBUFF_MAX_SAMPLE))
        {
          sns_rh_batchbuff_drop_block(item_ptr);
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_flush

===========================================================================*/
/*!
  @brief Flush batching reports to release memory

  @detail

  @param[i]  ssc_only: whether just flush reports to ssc

  @return   none
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC void sns_rh_batchbuff_flush(bool ssc_only)
{
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();
  uint32_t           num_rpts = 0;
  
  SMGR_FOR_EACH_Q_ITEM( rpt_queue_ptr, rpt_ptr, rpt_link )
  {
    if ( SNS_RH_REPORT_IS_BATCHING(rpt_ptr) )
    {
      bool send_rpt = false;

      if ( rpt_ptr->proc_type == SNS_PROC_SSC_V01 )
      {
        uint8_t i;
        RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
        {
          sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];

          /* For internal clients, makes sure there's a minimum number of samples*/
          if ( item_ptr->num_saved_samples >= SNS_RH_BATCHBUFF_FLUSH_THRESHOLD )
          {
            SNS_SMGR_BATCH_DBG3(
              HIGH, "batchbuff_flush - rpt_id=%u/0x%x #saved=%u",
              rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
              item_ptr->num_saved_samples);
            send_rpt = true;
            break;
          }
        }
      }
      else if ( !ssc_only )
      {
        send_rpt = true;
      }
      
      if ( send_rpt )
      {
        if ( sns_rh_add_to_ready_list(rpt_ptr) )
        {
          num_rpts++;
          rpt_ptr->state = RH_RPT_STATE_FLUSHING;
        }
      }
    }
  }
  if ( num_rpts != 0 )
  {
    SNS_SMGR_PRINTF2(MED, "flush - ssc_only=%u #rpts=%u", ssc_only, num_rpts);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_dump

===========================================================================*/
/*!
  @brief This function copies the uimg batch buffer memory block to big image blocks.

  @detail

  @param[i]  item_ptr  - the item spec pointer for the batch memory blocks

  @return   number of uimg batchbuffs that were freed
 */
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_batchbuff_dump(sns_rh_rpt_item_s* item_ptr)
{
  sns_rh_batchbuff_s* bb_ptr;
  uint8_t num_freed_ubb = 0;

  if ( sns_q_cnt(&item_ptr->ubatch_buffer_queue) == 0 )
  {
    return 0; /* dump what? */
  }

  if ( 0 == sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_BIMG_MEM))
  {
    SNS_SMGR_PRINTF0(ERROR, "dump - No bimg batchbuff left!!!");
    sns_rh_batchbuff_flush(false);
  }

  sns_rh_batchbuff_print();

  /* only dumps full batchbuff */
  while ( NULL != (bb_ptr = sns_q_check(&item_ptr->ubatch_buffer_queue)) &&
          bb_ptr->num_valid_samples >= SNS_RH_BATCHBUFF_MAX_SAMPLE )
  {
    sns_q_delete(&bb_ptr->q_link);

    if ( bb_ptr->num_samples > 0 )
    {
      sns_rh_batchbuff_s* new_bb_ptr = 
        sns_batchbuff_malloc(SNS_BATCHBUFF_CLIENT_SMGR, SNS_BATCHBUFF_BIMG_MEM);
      if ( new_bb_ptr != NULL )
      {
        SNS_OS_MEMCOPY(new_bb_ptr, bb_ptr, sizeof(sns_rh_batchbuff_s));
        sns_q_link(new_bb_ptr, &new_bb_ptr->q_link);
        sns_q_put(&item_ptr->bbatch_buffer_queue, &new_bb_ptr->q_link);
        SNS_SMGR_BATCH_DBG4(
          LOW, "dump - #valid=%u #samples=%u ts=%u-%u",
          bb_ptr->num_valid_samples, bb_ptr->num_samples, 
          bb_ptr->samples[bb_ptr->first_sample_idx].timestamp,
          bb_ptr->samples[bb_ptr->num_valid_samples-1].timestamp);
      }
      else
      {
        SNS_SMGR_BATCH_DBG3(
          ERROR, "dump - dropping #samples=%u ts=%u-%u",
          bb_ptr->num_samples, 
          bb_ptr->samples[bb_ptr->first_sample_idx].timestamp,
          bb_ptr->samples[bb_ptr->num_valid_samples-1].timestamp);
      }
    }
    else
    {
      SNS_SMGR_PRINTF3(
        ERROR, "dump - unexpectedly present #samples=%u ts=%u-%u",
        bb_ptr->num_samples, 
        bb_ptr->samples[bb_ptr->first_sample_idx].timestamp,
        bb_ptr->samples[bb_ptr->num_valid_samples-1].timestamp);
    }
    sns_batchbuff_free(SNS_BATCHBUFF_CLIENT_SMGR, bb_ptr);
    num_freed_ubb++;
  }

  sns_rh_batchbuff_print();
  return num_freed_ubb;
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_move_all

===========================================================================*/
/*!
  @brief  Moves all samples from uImage batchbuffs to bigImage batchbuffs
  @param  None
  @return None
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_batchbuff_move_all(void)
{
  uint8_t num_freed_ubb = 0;
  sns_rh_rpt_spec_s* rpt_ptr;
  sns_q_s*           rpt_queue_ptr = sns_rh_get_report_queue();

  SMGR_FOR_EACH_Q_ITEM( rpt_queue_ptr, rpt_ptr, rpt_link )
  {
    if ( SNS_RH_REPORT_IS_BATCHING(rpt_ptr) )
    {
      uint16_t i;
      RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
      {
        num_freed_ubb += sns_rh_batchbuff_dump(rpt_ptr->item_list[i]);
      }
    }
  }
  SNS_SMGR_PRINTF1(LOW, "move_all - freed %u blocks", num_freed_ubb);
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_get_avail_buff

===========================================================================*/
/*!
  @brief Returns a not-yet-full batchbuff of the given mem type

  @param[i]  item_ptr  - the item spec pointer for the batch memory blocks

  @return   
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC 
sns_rh_batchbuff_s *sns_rh_batchbuff_get_avail_buff(
  sns_rh_rpt_item_s*       item_ptr, 
  sns_batchbuff_mem_type_e mem_type)
{
  sns_q_s* q_ptr = (mem_type == SNS_BATCHBUFF_UIMG_MEM) ?
    &item_ptr->ubatch_buffer_queue : &item_ptr->bbatch_buffer_queue;
  sns_rh_batchbuff_s* bb_ptr = sns_q_last_check(q_ptr);

  if ( bb_ptr == NULL || /* queue is empty */
       (bb_ptr->num_valid_samples >= SNS_RH_BATCHBUFF_MAX_SAMPLE) /* block is full */ )
  {
    bb_ptr = sns_batchbuff_malloc(SNS_BATCHBUFF_CLIENT_SMGR, mem_type);
    if ( bb_ptr != NULL )
    {
      sns_q_link(bb_ptr, &bb_ptr->q_link);
      sns_q_put(q_ptr, &bb_ptr->q_link);
    }
    else
    {
      if ( mem_type == SNS_BATCHBUFF_UIMG_MEM )
      {
        if ( sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_UIMG_MEM) == 0 )
        {
          sns_rh_leave_island();
          sns_rh_batchbuff_move_all();
        }
        sns_rh_batchbuff_flush(true);
      }
      else
      {
        sns_rh_batchbuff_flush(false);
      }

      if ( sns_batchbuff_available_block_cnt(mem_type) > 0 )
      {
        bb_ptr = sns_rh_batchbuff_get_avail_buff(item_ptr, mem_type);
      }
    }
  }
  return bb_ptr;
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_get_buff

===========================================================================*/
/*!
  @brief Get an available batch buffer for specified item spec, allocate block if needed

  @param[i]  item_spec_ptr           - pointer to the item spec

  @return   Pointer to the available batch buffer
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC
sns_rh_batchbuff_s *sns_rh_batchbuff_get_buff(sns_rh_rpt_item_s* item_ptr)
{
  sns_rh_batchbuff_s* bb_ptr = 
    sns_rh_batchbuff_get_avail_buff(item_ptr, SNS_BATCHBUFF_UIMG_MEM);

  if ( bb_ptr == NULL )
  {
    sns_rh_leave_island();
    bb_ptr = sns_rh_batchbuff_get_avail_buff(item_ptr, SNS_BATCHBUFF_BIMG_MEM);
  }
  return bb_ptr;
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_pack_sample

===========================================================================*/
/*!
  @brief Pack sample attribute to samples stored in batch buffer

  @param[i]  sample_ptr                 - pointer to the sample
  @param[i]  item_spec_ptr            - pointer to item spec
  @param[i]  sample_status            - sample status
  @param[i]  timestamp                  - sample time stamp

  @return   None
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE  void sns_rh_batchbuff_pack_sample(
  sns_rh_batchbuff_sample_s*  sample_ptr,
  const sns_rh_rpt_item_s*    item_ptr,
  sns_ddf_status_e            sample_status)
{
  if ( sample_status == SNS_DDF_SUCCESS )
  {
    sample_ptr->quality = item_ptr->quality;
    sample_ptr->flags   = 
      sns_rh_apply_calibration(sample_ptr->data,
                               item_ptr->ddf_sensor_ptr,
                               item_ptr->cal_sel);
  }
  else
  {
    sample_ptr->quality = SNS_SMGR_ITEM_QUALITY_INVALID_NOT_READY_V01;
    sample_ptr->flags   = SNS_SMGR_ITEM_FLAG_INVALID_V01;
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_get_sample

===========================================================================*/
/*!
  @brief Get first (earliest) sample stored in batch buffer 
  
  @param[i]  item_spec_ptr  - pointer to item spec
  @param[i]  next           - get the next sample and release the current one

  @return   Pointer to the sample stored in batch buffer
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC 
sns_rh_batchbuff_sample_s* sns_rh_batchbuff_get_sample(
  sns_rh_rpt_item_s* item_ptr, 
  bool               next)
{
  sns_rh_batchbuff_sample_s* sample_ptr = NULL;
  sns_rh_batchbuff_s* bb_ptr = NULL;

  /* always looks in bigImage queue first */
  while ( sns_q_cnt(&item_ptr->bbatch_buffer_queue) > 0 && bb_ptr == NULL )
  {
    bb_ptr = sns_q_check(&item_ptr->bbatch_buffer_queue);
    if ( bb_ptr != NULL && bb_ptr->num_samples == 0 )
    {
      sns_q_delete(&bb_ptr->q_link);
      sns_batchbuff_free(SNS_BATCHBUFF_CLIENT_SMGR, bb_ptr);
      bb_ptr = NULL;

      {
        uint8_t num_bbb = sns_q_cnt(&item_ptr->bbatch_buffer_queue);
        if ( num_bbb > 0 )
        {
          SNS_SMGR_PRINTF4(
            ERROR, "get_sample - rpt_id=%u/0x%x #saved=%u #bbb=%u",
            item_ptr->parent_report_ptr->rpt_id,
            item_ptr->parent_report_ptr->msg_header.connection_handle,
            item_ptr->num_saved_samples, num_bbb);
        }
      }
    }
  }

  /* looks in uImage queue if none found in bigImage queue */
  if ( bb_ptr == NULL )
  {
    bb_ptr = sns_q_check(&item_ptr->ubatch_buffer_queue);
  }

  if ( bb_ptr != NULL && bb_ptr->num_samples > 0)
  {
    if ( next )
    {
      bb_ptr->first_sample_idx++;
      bb_ptr->num_samples--;
      item_ptr->num_saved_samples--;
      item_ptr->parent_report_ptr->num_saved_samples--;
      if ( bb_ptr->num_samples == 0 )
      {
        sns_q_delete(&bb_ptr->q_link);
        sns_batchbuff_free(SNS_BATCHBUFF_CLIENT_SMGR, bb_ptr);
        bb_ptr = NULL;
        sample_ptr = sns_rh_batchbuff_get_sample(item_ptr, false);
      }
    }
    else
    {
      SNS_SMGR_BATCH_DBG3(
        HIGH, "get_sample - #saved=%u ts=%u-%u",
        item_ptr->num_saved_samples,
        bb_ptr->samples[bb_ptr->first_sample_idx].timestamp, 
        bb_ptr->samples[bb_ptr->num_valid_samples-1].timestamp);
    }
    if ( bb_ptr )
    {
      sample_ptr = &bb_ptr->samples[bb_ptr->first_sample_idx];
    }
  }

  return sample_ptr;
}


/*===========================================================================

  FUNCTION:   sns_rh_rpt_gen_generate_buffering_report

===========================================================================*/
/*!
  @brief This function generates buffering report indication messages.

  @detail

  @param[i]  rpt_spec_ptr  - the report spec
  @param[i]  num_samples   - number of samples should be sent
                             
  @return   number of samples sent
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE uint32_t sns_rh_batchbuff_generate_buffering_report(
  sns_rh_rpt_spec_s*  rpt_spec_ptr,
  uint32_t            num_samples,
  uint8_t             ind_type)
{
  uint8_t  i;
  sns_smgr_buffering_ind_msg_v01* ind_ptr;
  uint16_t ind_id = (rpt_spec_ptr->msg_header.svc_num == SNS_SMGR_SVC_ID_V01) ?
    SNS_SMGR_BUFFERING_IND_V01 : SNS_SMGR_EVENT_GATED_BUFFERING_IND_V02;
  /* -------------------------------------------------------------------- */

  SNS_SMGR_BATCH_DBG4(
    HIGH, "gen_buff_rpt - rpt_id=%u/0x%x num_samples=%u ind_type=%u", 
    rpt_spec_ptr->rpt_id, rpt_spec_ptr->msg_header.connection_handle,
    num_samples, ind_type);

  ind_ptr = sns_rh_get_buffering_ind_buf();
  SNS_OS_MEMZERO(ind_ptr, sizeof(sns_smgr_buffering_ind_msg_v01));
  RH_FOR_EACH_REPORT_ITEM( rpt_spec_ptr, i )
  {
    sns_ddf_time_t previous_timestamp;
    sns_smgr_buffering_sample_index_s_v01* idx_ptr = NULL;
    sns_rh_rpt_item_s* item_spec_ptr = rpt_spec_ptr->item_list[i];
    sns_smgr_ddf_sensor_s* ddf_sensor_ptr = item_spec_ptr->ddf_sensor_ptr;
    const sns_rh_batchbuff_sample_s *sample_ptr;
    uint32_t item_num_samples = 0;
    if ( item_spec_ptr->num_saved_samples > 0 )
    {
      item_num_samples = (uint32_t)(item_spec_ptr->sample_cnt_ratio*num_samples+0.5);
    }
    else
    {
      item_num_samples = 0;
    }
      
    
    SNS_SMGR_BATCH_DBG3(
      MED, "gen_buff_rpt - ddf_sensor=%u state=%u effective=0x%x", 
      SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
      ddf_sensor_ptr->sensor_ptr->sensor_state, item_spec_ptr->effective_rate_hz);

    if ( item_spec_ptr->effective_rate_hz == 0 )
    {
      continue;
    }

    previous_timestamp = item_spec_ptr->ts_last_sent;
    idx_ptr = &ind_ptr->Indices[ind_ptr->Indices_len];
    idx_ptr->SensorId       = SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr);
    idx_ptr->DataType       = ddf_sensor_ptr->data_type;
    idx_ptr->FirstSampleIdx = ind_ptr->Samples_len;
    idx_ptr->FirstSampleTimestamp = 0;
    idx_ptr->SamplingRate   = item_spec_ptr->effective_rate_hz;

    if ( sns_q_cnt(&item_spec_ptr->bbatch_buffer_queue) > 0 )
    {
      sns_rh_leave_island();
    }

    sample_ptr = sns_rh_batchbuff_get_sample(item_spec_ptr, false);
    while ( (ind_ptr->Samples_len<ARR_SIZE(ind_ptr->Samples)) &&
            (sample_ptr != NULL) && 
            idx_ptr->SampleCount<item_num_samples)
    {

      sns_smgr_buffering_sample_s_v01* ind_sample_ptr =
        &ind_ptr->Samples[ind_ptr->Samples_len];
      
      SNS_OS_MEMCOPY(ind_sample_ptr->Data, sample_ptr->data, sizeof(sample_ptr->data));
      ind_sample_ptr->Flags = sample_ptr->flags;
      ind_sample_ptr->Quality = sample_ptr->quality;
      if ( idx_ptr->SampleCount == 0 )
      {
        ind_sample_ptr->TimeStampOffset = 0;
        idx_ptr->FirstSampleTimestamp = sample_ptr->timestamp;
      }
      else
      {
        ind_sample_ptr->TimeStampOffset = 
          (uint32_t)(sample_ptr->timestamp - previous_timestamp );
      }
      previous_timestamp = sample_ptr->timestamp;
      idx_ptr->SampleCount++;
      ind_ptr->Samples_len++;
      
      sample_ptr = sns_rh_batchbuff_get_sample(item_spec_ptr, true);
    }

    item_spec_ptr->state = RH_RPT_ITEM_STATE_IDLE;
    item_spec_ptr->ts_last_sent = previous_timestamp;

    SNS_SMGR_BATCH_DBG4(
      LOW, "gen_buff_rpt - sensor=%u #samples=%u first=%u last=%u",
      idx_ptr->SensorId, idx_ptr->SampleCount,
      idx_ptr->FirstSampleTimestamp, item_spec_ptr->ts_last_sent);

    ind_ptr->Indices_len++;
  }/* for each item in report spec */

  sns_profiling_log_qdss(SNS_RH_RPT_IND_START, 4, rpt_spec_ptr->rpt_id, 
                         rpt_spec_ptr->msg_header.connection_handle, 
                         ind_id, rpt_spec_ptr->num_items);

  SNS_SMGR_BATCH_DBG1(HIGH, "batch send ind, sample cnt %u", ind_ptr->Samples_len);
  if ( ind_ptr->Samples_len > 0 )
  {
    bool sent = false;
    
    ind_ptr->ReportId      = rpt_spec_ptr->rpt_id;
    ind_ptr->IndType_valid = true;
    ind_ptr->IndType       = ind_type;

    sent = sns_rh_rpt_gen_send_report_indication(
           rpt_spec_ptr,
           ind_ptr,
           ind_id,
           sizeof(sns_smgr_buffering_ind_msg_v01),
           false);
    if ( sent || sns_rh_mr_conn_busy(rpt_spec_ptr->msg_header.connection_handle) ) 
    {
      rpt_spec_ptr->ind_count++;
      if ( !sent || rpt_spec_ptr->proc_type == SNS_PROC_SSC_V01 )
      {
        SNS_SMGR_PRINTF4(
          HIGH, "send_buf_ind - id=%u/0x%x #samples=%u last_sent[0]=%u",
          ind_ptr->ReportId, rpt_spec_ptr->msg_header.connection_handle,
          ind_ptr->Samples_len, rpt_spec_ptr->item_list[0]->ts_last_sent);
      }
      if ( !sent )
      {
        ind_ptr->Samples_len = 0;
      }
    }
    else
    {
      SNS_SMGR_PRINTF4(
         ERROR, "send_buf_ind - dropping id=%u/0x%x #samples=%u last_sent[0]=%u",
         ind_ptr->ReportId, rpt_spec_ptr->msg_header.connection_handle,
         ind_ptr->Samples_len, rpt_spec_ptr->item_list[0]->ts_last_sent);
      ind_ptr->Samples_len = 0;
    }
  }

  sns_profiling_log_qdss(
    SNS_RH_RPT_IND_END, 2, (ind_ptr->Samples_len > 0), 
    (ind_ptr->Samples_len > 0) ? rpt_spec_ptr->item_list[0]->ts_last_sent : 0);

  return ind_ptr->Samples_len;
}


/*===========================================================================

  FUNCTION:   sns_rh_rpt_gen_compute_num_avail_samples

===========================================================================*/
/*!
  @brief Computes number of samples available for given report since the
         previous report indication was sent.

  @detail Also returned are timestamps of oldest sample sent and latest sample available.

  @param[i]  rpt_spec_ptr            - the report for which to compute sample count
  @param[i]  oldest_last_sent_ts_ptr - destination for timestamp of oldest sample sent
  @param[i]  latest_sample_ts_ptr    - destination for timestamp of latest sample available

  @return   The computed number of samples available.
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE  uint32_t sns_rh_batchbuff_compute_num_avail_samples(
  const sns_rh_rpt_spec_s*  rpt_spec_ptr,
  sns_ddf_time_t*           oldest_last_sent_ts_ptr,
  sns_ddf_time_t*           latest_sample_ts_ptr)
{
  uint32_t total_samples = 0;
  uint8_t i;
  /* -------------------------------------------------------------------- */

  *oldest_last_sent_ts_ptr = sns_ddf_get_timestamp();
  *latest_sample_ts_ptr    = rpt_spec_ptr->ts_last_rpt_sent;

  RH_FOR_EACH_REPORT_ITEM( rpt_spec_ptr, i )
  {
    sns_rh_rpt_item_s* item_ptr = rpt_spec_ptr->item_list[i];

    total_samples += item_ptr->num_saved_samples;
    *oldest_last_sent_ts_ptr = TICK_MIN(*oldest_last_sent_ts_ptr, item_ptr->ts_last_sent);
    *latest_sample_ts_ptr    = TICK_MAX(*latest_sample_ts_ptr, item_ptr->ts_last_saved);
  } /*RH_FOR_EACH_REPORT_ITEM*/
  
  if ( total_samples > 0 )
  {
    SNS_SMGR_BATCH_DBG3(
      MED, "avail_sample_cnt=%d oldest=%u latest=%u",
      total_samples, *oldest_last_sent_ts_ptr, *latest_sample_ts_ptr);
  }
  else
  {
    SNS_SMGR_BATCH_DBG3(
      MED, "no samples for rpt_id=%u/0x%x rpt_tick=%u",
      rpt_spec_ptr->rpt_id, rpt_spec_ptr->msg_header.connection_handle,
      rpt_spec_ptr->rpt_tick);
  }
  return total_samples;
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_save_samples

===========================================================================*/
/*!
  @brief Processes samples from depot and saves them to batch buffer 

  @param [i]  item_ptr  - the report item for which new samples might be available

  @return     true if new samples were saved
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE bool sns_rh_batchbuff_save_samples(sns_rh_rpt_item_s* item_ptr)
{
  bool new_samples_saved = false;
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = item_ptr->ddf_sensor_ptr;
  const sns_smgr_sample_s *sample_ptr;
  sns_rh_batchbuff_s* bb_ptr = NULL;

  sns_ddf_time_t latest_sample_ts = sns_rh_get_latest_sample_timestamp(item_ptr);

  if ( !TICK1_GTR_TICK2(latest_sample_ts, 
                        item_ptr->last_processed_sample_timestamp) ||
       !TICK1_GTR_TICK2(latest_sample_ts, item_ptr->ts_last_saved) )
  {
    return new_samples_saved;
  }

  SNS_SMGR_BATCH_DBG4(
    HIGH, "save_samples - rpt_id=%u/0x%x ddf_sensor=%u last_sent=%u", 
    item_ptr->parent_report_ptr->rpt_id, 
    item_ptr->parent_report_ptr->msg_header.connection_handle,
    SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr), item_ptr->ts_last_sent);
  SNS_SMGR_BATCH_DBG4(
    MED, "save_samples - #saved=%u last_saved=%u last_proc=%u latest=%u", 
    item_ptr->num_saved_samples, item_ptr->ts_last_saved,
    item_ptr->last_processed_sample_timestamp, latest_sample_ts);

  sns_os_mutex_pend(ddf_sensor_ptr->depot_mutex_ptr, 0, NULL);

  if ( ddf_sensor_ptr->bimg_depot_ptr == item_ptr->depot_ptr )
  {
    sns_rh_leave_island();
  }

  sample_ptr = sns_rh_rpt_gen_init_item_depot_iter(item_ptr);
  if ( sample_ptr != NULL )
  {
    bb_ptr = sns_rh_batchbuff_get_buff(item_ptr);
    if ( NULL == bb_ptr )
    {
      SNS_SMGR_PRINTF0(FATAL, "save_samples - no batchbuff available!!!");
      sns_os_mutex_post(ddf_sensor_ptr->depot_mutex_ptr);
      return  new_samples_saved;
    }
  }

  while ( (sample_ptr != NULL) &&
          TICK1_GEQ_TICK2(SMGR_SAMPLE_TIMESTAMP(sample_ptr), item_ptr->ts_last_saved) && 
          ((SMGR_SAMPLE_STATUS(sample_ptr) == SNS_DDF_SUCCESS) || item_ptr->samples_sent) )
  {
    sns_ddf_status_e sample_status;
    uint8_t sample_idx;

    if ( bb_ptr->num_valid_samples >= SNS_RH_BATCHBUFF_MAX_SAMPLE )
    {
      bb_ptr = sns_rh_batchbuff_get_buff(item_ptr);
      sample_idx = 0;
      if ( NULL == bb_ptr )
      {
        break;
      }
    }
    sample_idx = bb_ptr->first_sample_idx + bb_ptr->num_samples;
    sample_status = 
      sns_rh_process_sample(item_ptr, sample_ptr, 
                            &bb_ptr->samples[sample_idx].timestamp,
                            bb_ptr->samples[sample_idx].data);
    SNS_SMGR_BATCH_DBG3(
      MED, "save_samples - ts=%u result=%u idx=%u", 
      bb_ptr->samples[sample_idx].timestamp, sample_status, sample_idx);
      
    if ( (sample_status == SNS_DDF_SUCCESS) ||
         (sample_status == SNS_DDF_EINVALID_DATA) )
    {
      sns_rh_batchbuff_pack_sample(&bb_ptr->samples[sample_idx],
                                   item_ptr, sample_status);
      bb_ptr->num_valid_samples++;
      bb_ptr->num_samples++;
      item_ptr->num_saved_samples++;
      item_ptr->parent_report_ptr->num_saved_samples++;
      item_ptr->samples_sent  = true;
      item_ptr->ts_last_saved = bb_ptr->samples[sample_idx].timestamp;
      new_samples_saved = true;

      if ( item_ptr->effective_rate_hz < FX_ONE_Q16 ||
           item_ptr->parent_report_ptr->event_sensors_only )
      {
        SNS_SMGR_PRINTF3(
          LOW, "save_sample - rpt_id=%u/0x%x ts=%u", 
          item_ptr->parent_report_ptr->rpt_id, 
          item_ptr->parent_report_ptr->msg_header.connection_handle,
          item_ptr->ts_last_saved);
      }

      RH_UPDATE_INTERP_TIMESTAMP(&item_ptr->interpolator_obj);
    }
    else
    {
      sample_ptr = sns_smgr_depot_get_sample(&item_ptr->depot_iter, 1);
    }
  }
  SNS_SMGR_BATCH_DBG2(
    MED, "save_samples - #saved=%u last_saved=%u", 
    item_ptr->num_saved_samples, item_ptr->ts_last_saved);

  sns_os_mutex_post(ddf_sensor_ptr->depot_mutex_ptr);

  return new_samples_saved;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_batchbuff_low_request

===========================================================================*/
/*!
  @brief  Respond to SNS_RH_BATCHBUFF_LOW_SIG
  @param  None
  @return None
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE void sns_rh_process_batchbuff_low_request(void)
{
  uint16_t ubb_blk = sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_UIMG_MEM);

  SNS_SMGR_BATCH_DBG2(
    HIGH, "batchbuff_low - avail #ubb=%u #bbb=%u", 
    ubb_blk, sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_BIMG_MEM));
  
  if ( ubb_blk <= SNS_BATCHBUFF_UIMG_CB_THRESHOLD1 )
  {
    if ( ubb_blk <= SNS_BATCHBUFF_UIMG_CB_THRESHOLD2 )
    {
      sns_rh_leave_island();
      sns_rh_batchbuff_move_all();
      sns_rh_enter_island();
    }
  }

  if ( sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_BIMG_MEM) <= 
       SNS_BATCHBUFF_BIMG_CB_THRESHOLD1 )
  {
    sns_rh_batchbuff_trim_nonwakeup();
  }
  if ( sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_BIMG_MEM) <= 
       SNS_BATCHBUFF_BIMG_CB_THRESHOLD2 )
  {
    sns_rh_batchbuff_flush(false);
  }

  SNS_SMGR_BATCH_DBG2(
    MED, "batchbuff_low - avail #ubb=%u #bbb=%u", 
    sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_UIMG_MEM), 
    sns_batchbuff_available_block_cnt(SNS_BATCHBUFF_BIMG_MEM));
}

/*===========================================================================

  FUNCTION:   sns_rh_batchbuff_handle_conn_busy

===========================================================================*/
/*!
  @brief  Keeps only limited number of batchbuffs for the given report
  @param[io] rpt_ptr - the report whose connection is busy
  @return    None
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE void sns_rh_batchbuff_handle_conn_busy(sns_rh_rpt_spec_s* rpt_ptr)
{
  uint8_t i;
  RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
  {
    /* keeps only a limited number of batchbuffs */
    sns_rh_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
    uint32_t num_saved_samples = item_ptr->num_saved_samples;
    uint32_t max_to_save = item_ptr->samples_per_report << 1;
    while ( item_ptr->num_saved_samples > max_to_save &&
            item_ptr->num_saved_samples > SNS_RH_BATCHBUFF_MAX_SAMPLE )
    {
      sns_rh_batchbuff_drop_block(item_ptr);
    }
    if ( num_saved_samples > item_ptr->num_saved_samples )
    {
      SNS_SMGR_PRINTF3(
        HIGH, "handle_conn_busy - rpt_id=%u/0x%x dropped %u samples", 
        rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle, 
        num_saved_samples - item_ptr->num_saved_samples);
    }
  }
}

