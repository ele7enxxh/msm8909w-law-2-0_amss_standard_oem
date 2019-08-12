/*!
  @file
  ftm_calibration_v3_polling.c

  @brief
  This module contains common RF calibration code for Polling

  @addtogroup RF_COMMON_FTM_CALV3_POLLING
 
  @image html ftm_calibration_sweep_polling.jpg

  @brief
  This module provides a Polling extensions to calibration v3 framework
  @{
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_polling.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/03/13   aro     Compiler wanring fix for GPS-only builds
04/02/13   aro     Migrated to inline Math functions
04/02/13   aro     Command for enable debug level for calv3
04/02/13   aro     Debug message update
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
01/10/13   aro     Fixed time profiling to check polling timeout
12/07/12   aki     Changed ftm_calibration_polling_event_payload_size() to uint8
12/06/12   aki     Added RX threshold aka RX wait functionality
11/21/12   aro     Fixed polling timeout
11/06/12   aro     Fixed buggy argument check
11/05/12   aro     Renamed polling event enum
11/05/12   aro     Updated Compute payload to handle EVENT_EPT_PROCESSING_DONE
11/05/12   aro     Doc Update
11/05/12   aro     Re-organized cal v3 data structure to be more modular
10/24/12   dw      Fixed buggy logic in ftm_calibration_polling_register_event
09/10/12   Saul    Warning fixes.
08/31/12   Saul    CDMA. Added support for EPT_PROCESSING_DONE FW message
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
07/24/12   aro     F3 message update
07/25/12   aro     Added payload for each event
07/20/12   aro     Added support to do polling for EPT expansion
07/19/12   aro     Added polling counter + F3 message update
07/19/12   aro     Added generic call to event callback function
07/19/12   aro     Implementation to process polling opcode
07/19/12   aro     Doxygen update
07/19/12   aro     Function to deinitialize polling data structure
07/19/12   aro     Implemented polling event registration
07/19/12   aro     Enumeration for Polling Events
07/19/12   aro     Added polling parameter initialization
07/19/12   aro     Type name updates
07/18/12   aro     Initial version of polling code

==============================================================================*/

#include "comdef.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_polling.h"
#include "ftm_msg.h"
#include "ftm_common_data.h"
#include "modem_mem.h"
#include "rfcommon_math.h"

#ifdef FEATURE_CDMA1X
#include "ftm_1x_calibration.h"
#endif /* FEATURE_CDMA1X */

#ifdef FEATURE_WCDMA
#include "ftm_wcdma_calv3.h"
#endif

#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_calv3.h"
#endif

#ifdef FEATURE_LTE
#include "rflte_ftm_calibration.h"
#endif

/*============================================================================*/
/*!
  @name Helper functions

  @brief
  This section contains helper functions for FTM polling opcode operation
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute the size of the event payload
 
  @details
  For the given event_id, this function will return the total payload
  size.
 
  @param event_id
  Event ID for which the payload size is to be computed
 
  @return
  Payload size in number of bytes for the given event ID.
*/
static uint8
ftm_calibration_polling_event_payload_size
(
  ftm_cal_polling_event_id event_id 
)
{
  uint8 event_pl_sz = 0;

  /* Argument validation */
  if ( event_id >= POLLING_EVENT_MAX )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_calibration_polling_event_payload_size: "
                          "Invalid Event %d", event_id );
    return 0;
  } /* if ( POLLING_EVENT_MAX >= POLLING_EVENT_MAX ) */

  /* Based on event_id compute the total payload size */
  switch ( event_id )
  {

  case POLLING_EVENT_EPT_EXPANSION_DONE:
    event_pl_sz = sizeof(ftm_cal_polling_event_0_payload_type);
    break;

  case POLLING_EVENT_RXAGC_THRES_REACHED:
    event_pl_sz = sizeof(ftm_cal_polling_event_1_payload_type);
    break;
	
  case POLLING_EVENT_EPT_PROCESSING_DONE:
    event_pl_sz = sizeof(ftm_cal_polling_event_0_payload_type);
    break;

  default:
    event_pl_sz = 0;
    break;

  } /* switch ( event_id ) */

  /* return payload size */
  return event_pl_sz;

} /* ftm_calibration_polling_event_payload_size */

/*! @} */

/*============================================================================*/
/*!
  @name Event Registration

  @brief
  This section contains functions for FTM polling opcode to perform event
  registration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Register event callback
 
  @details
  This function will register the callback functions for each event. Each
  call will perform dynamic allocation of memory (malloc or realloc) based on 
  the maximum event id requested for registration till the last call.
 
  @param event_id
  Event ID for which the register is to be done
 
  @param event_check_cb
  callback function to be associated with
 
  @param rf_mode
  RFM Mode for which the registration is to be done
 
  @return
  Status of event callback registration
*/
boolean
ftm_calibration_polling_register_event
(
  rfm_mode_enum_type rf_mode,
  ftm_cal_polling_event_id event_id,
  ftm_cal_polling_event_cb_type event_check_cb
)
{
  boolean ret_val = TRUE;
  ftm_calibration_data_type *cal_data; /* Calv3 Data */
  ftm_cal_polling_opcode_data *poll_data;
  ftm_cal_polling_event_list *event_list;

  /* Parameter Validation : Event ID */
  if ( event_id >= POLLING_EVENT_MAX )
  {
    FTM_MSG_1( FTM_ERROR, "POLLING EventReg: Invalid Event ID %d", event_id );
    return FALSE;
  } /* if ( event_id >= POLLING_EVENT_MAX ) */
  
  /* Parameter Validation : Callback */
  if ( event_check_cb == NULL )
  {
    FTM_MSG( FTM_ERROR, "POLLING EventReg: NULL callback" );
    return FALSE;
  } /* if ( event_check_cb == NULL ) */

  /* Parameter Validation : RF Mode */
  if ( rf_mode >= FTM_CALIBRATION_NUM_OF_TECHS )
  {
    FTM_MSG_1( FTM_ERROR, "POLLING EventReg: Invalid RF Mode %d", rf_mode );
    return FALSE;
  } /* if ( rf_mode >= FTM_CALIBRATION_NUM_OF_TECHS ) */

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* NULL pointer check */
  if ( cal_data != NULL )
  {
    /* Shorten pointer to polling data */
    poll_data = 
      &(cal_data->seg_data.oc_payload.polling.oc_data[rf_mode]);

    /* Allocate memory, if not done before */
    if ( poll_data->event_check_cb == NULL )
    {
      poll_data->event_check_cb = 
        modem_mem_alloc( (event_id + 1)*sizeof(ftm_cal_polling_event_list),
                         MODEM_MEM_CLIENT_RFA );
      poll_data->max_event_id = event_id;
    } /* if ( poll_data->event_check_cb == NULL ) */
    else
    {
      /* Check if the realloc needs to be done or not. Re-alloc needs to be
      done, if the new event_id to be registered is greater than the past
      event IDs */
      if ( poll_data->max_event_id < event_id )
      {
        poll_data->event_check_cb =
          modem_mem_realloc( poll_data->event_check_cb,
                             (event_id + 1)*sizeof(ftm_cal_polling_event_list),
                             MODEM_MEM_CLIENT_RFA);
        poll_data->max_event_id = event_id;
      } /* if ! ( poll_data->max_event_id > event_id ) */
    } /* if ! ( poll_data->event_check_cb == NULL ) */

    /* Proceed, only if the mallocs were done successfully */
    if ( poll_data->event_check_cb != NULL )
    {
      event_list = (ftm_cal_polling_event_list*)poll_data->event_check_cb;

      /* Populate the Event List */
      event_list[event_id].event_id = event_id;
      event_list[event_id].event_cb = event_check_cb;
    } /* if ( poll_data->event_check_cb != NULL ) */
    else
    {
      ret_val = FALSE;
      FTM_MSG( FTM_ERROR, "POLLING EventReg: Malloc failed" );
    } /* if ! ( poll_data->event_check_cb != NULL ) */
    
  } /* if ( cal_data != NULL ) */
  else
  {
    ret_val = FALSE;
    FTM_MSG( FTM_ERROR, "POLLING EventReg: NULL data" );
  } /* if ! ( cal_data != NULL ) */
  
  FTM_MSG_3( FTM_MED, "POLLING EventReg: Event %d Mode %d [Status %d]", 
             event_id, rf_mode, ret_val );
    
  return ret_val;

} /* ftm_calibration_polling_register_event */

/*! @} */

/*============================================================================*/
/*!
  @name Polling Initialization

  @brief
  This section contains functions for FTM polling opcode initialization and
  de-initialization.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Initialize Polling param
 
  @details
  This function will initialize all parameter used for polling opcode.
 
  @param cal_data
  Pointer to the data structure containing Cal v3 data
*/
boolean 
ftm_calibration_polling_init
( 
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_polling_data_type *cfg_data;
  #if defined(FEATURE_LTE)||defined(FEATURE_WCDMA)||defined(FEATURE_CDMA1X)||\
      defined(FEATURE_CDMA1X)
  ftm_cal_polling_opcode_data *poll_data;
  #endif

  reg_data.unpack_fp = ftm_calibration_polling_unpack;
  reg_data.process_fp = ftm_calibration_polling_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  cfg_data = &(cal_data->seg_data.oc_payload.polling);

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_POLLING,
                                             reg_data );

  #ifdef FEATURE_CDMA1X
  poll_data = &(cfg_data->oc_data[RFM_1X_MODE]);
  poll_data->polling_init_cb = ftm_1x_polling_event_init;
  poll_data->max_event_id = 0;
  poll_data->event_to_be_done = 0;
  poll_data->elapsed_duration_us = 0;
  poll_data->polling_count = 0;
  if ( poll_data->polling_init_cb != NULL )
  {
    poll_data->polling_init_cb();
  } /* if ( poll_data->polling_init_cb != NULL ) */
  #endif /* FEATURE_CDMA1X */

  #ifdef FEATURE_LTE
  poll_data = &(cfg_data->oc_data[RFM_LTE_MODE]);
  poll_data->polling_init_cb = rflte_ftm_calibration_v3_polling_event_init;
  poll_data->max_event_id = 0;
  poll_data->event_to_be_done = 0;
  poll_data->elapsed_duration_us = 0;
  poll_data->polling_count = 0;
  if ( poll_data->polling_init_cb != NULL )
  {
    poll_data->polling_init_cb();
  } /* if ( poll_data->polling_init_cb != NULL ) */
  #endif /* FEATURE_LTE */

  #ifdef FEATURE_WCDMA
  poll_data = &(cfg_data->oc_data[RFM_IMT_MODE]);
  poll_data->polling_init_cb = ftm_wcdma_calv3_sweep_polling_event_init;
  poll_data->max_event_id = 0;
  poll_data->event_to_be_done = 0;
  poll_data->elapsed_duration_us = 0;
  poll_data->polling_count = 0;
  if ( poll_data->polling_init_cb != NULL )
  {
    poll_data->polling_init_cb();
  } /* if ( poll_data->polling_init_cb != NULL ) */
  #endif /*FEATURE_WCDMA */

  #ifdef FEATURE_TDSCDMA
  poll_data = &(cfg_data->oc_data[RFM_TDSCDMA_MODE]);
  poll_data->polling_init_cb = ftm_tdscdma_calv3_sweep_polling_event_init;
  poll_data->max_event_id = 0;
  poll_data->event_to_be_done = 0;
  poll_data->elapsed_duration_us = 0;
  poll_data->polling_count = 0;
  if ( poll_data->polling_init_cb != NULL )
  {
    poll_data->polling_init_cb();
  } /* if ( poll_data->polling_init_cb != NULL ) */
  #endif /*FEATURE_TDSCDMA */

  return ret_val;

} /* ftm_calibration_polling_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  De-Initialize Polling param
 
  @details
  This function will deinitialize all parameter used for polling opcode.
 
  @param cal_data
  Pointer to the data structure containing Cal v3 data
*/
boolean 
ftm_calibration_polling_deinit
( 
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_polling_data_type *cfg_data;
  #if defined(FEATURE_LTE)||defined(FEATURE_WCDMA)||defined(FEATURE_CDMA1X)||\
      defined(FEATURE_CDMA1X)
  ftm_cal_polling_opcode_data *poll_data;
  #endif

  cfg_data = &(cal_data->seg_data.oc_payload.polling);

  #ifdef FEATURE_CDMA1X
  poll_data = &(cfg_data->oc_data[RFM_1X_MODE]);
  /* Free the memory used for dynamic event registration */
  if ( poll_data->event_check_cb != NULL )
  {
    modem_mem_free( poll_data->event_check_cb, MODEM_MEM_CLIENT_RFA );
    poll_data->event_check_cb = NULL;
  } /* if ( poll_data->event_check_cb != NULL ) */
  #endif /* FEATURE_CDMA1X */

  #ifdef FEATURE_LTE
  poll_data = &(cfg_data->oc_data[RFM_LTE_MODE]);
  /* Free the memory used for dynamic event registration */
  if ( poll_data->event_check_cb != NULL )
  {
    modem_mem_free( poll_data->event_check_cb, MODEM_MEM_CLIENT_RFA );
    poll_data->event_check_cb = NULL;
  } /* if ( poll_data->event_check_cb != NULL ) */
  #endif /* FEATURE_LTE */

  #ifdef FEATURE_WCDMA
  poll_data = &(cfg_data->oc_data[RFM_IMT_MODE]);
  /* Free the memory used for dynamic event registration */
  if ( poll_data->event_check_cb != NULL )
  {
    modem_mem_free( poll_data->event_check_cb, MODEM_MEM_CLIENT_RFA );
    poll_data->event_check_cb = NULL;
  } /* if ( poll_data->event_check_cb != NULL ) */
  #endif /*FEATURE_WCDMA */

  #ifdef FEATURE_TDSCDMA
  poll_data = &(cfg_data->oc_data[RFM_TDSCDMA_MODE]);
  /* Free the memory used for dynamic event registration */
  if ( poll_data->event_check_cb != NULL )
  {
    modem_mem_free( poll_data->event_check_cb, MODEM_MEM_CLIENT_RFA );
    poll_data->event_check_cb = NULL;
  } /* if ( poll_data->event_check_cb != NULL ) */
  #endif /*FEATURE_TDSCDMA */

  return ret_val;
} /* ftm_calibration_polling_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute the Payload size for Polling
 
  @details
  Polling opcode can have variable size based on the Events for polling.
  Thus this function dynamically computes the total payload size based on the
  enabled sub opcode
 
  @param instr_payload
  Pointer to polling instruction payload
 
  @return
  Total size of payload for polling
*/
uint16
ftm_calibration_polling_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  ftm_cal_polling_instr_payload_type const *p_ptr;
  uint16 payload_size = 0; /* Payload Size */
  ftm_cal_polling_events event_bit;
  ftm_cal_polling_events events;

  p_ptr = (ftm_cal_polling_instr_payload_type*)instr_payload;
  events = p_ptr->polling_event;

  payload_size += sizeof(ftm_cal_polling_instr_payload_type);
  for (event_bit = 0; event_bit<POLLING_EVENT_MAX; event_bit++)
  {
    /* Update the Payload size if sub opcode is enabled */
    if( events & (ftm_cal_polling_events)(1 << event_bit) )
    {
      payload_size += ftm_calibration_polling_event_payload_size( event_bit );    
    }
  }

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  /* Print F3 message, if debug mode is enabled */
  if ( p_ptr->debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][POLLING.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_POLLING,
               payload_size );
  } /* if ( p_ptr->debug_mode_en == TRUE ) */

  /* return Payload size */
  return payload_size;

} /* ftm_calibration_polling_unpack */

/*! @} */

/*============================================================================*/
/*!
  @name OpCode Processing

  @brief
  This section contains functions for FTM polling opcode processing
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Polling Instructions
 
  @details
  This is the main handler function for polling Opcode which will process the
  polling payload.
 
  @param cal_data
  Pointer to calibration v3 dats structure
 
  @param payload
  Pointer to polling paylaod instruction
  
  @return
  Status indicating the status of polling processing
*/
boolean
ftm_calibration_polling_process
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  ftm_cal_polling_events event_mask; /* Variable to store Event Mask */
  ftm_cal_polling_events event_flag; /* Variable to store the Event Flag for
  the segment. This will be modified in the while loop. */
  ftm_cal_polling_opcode_data *poll_data;
  boolean event_met = FALSE;
  boolean cease_polling = FALSE;
  ftm_cal_polling_event_list *event_list;
  uint8 event_idx;
  ftm_cal_polling_instr_payload_type const *payload;
  uint8 *event_payload;
  static ftm_calibration_segment_duration_type poll_int = 0;
  ftm_cal_polling_data_type *cfg_data;
  rfm_mode_enum_type current_rf_mode;

  /* Parameter Validation : Cal Data */
  if ( ( cal_data == NULL ) || ( oc_disp == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][POLLING.{0x%x}] "
               "Process :  NULL cal data",
               FTM_CAL_OPCODE_POLLING );
    return FALSE;
  } /* if ( cal_data == NULL ) */

  payload = (ftm_cal_polling_instr_payload_type*)(oc_disp->payload_ptr);

  /* Parameter Validation : Instruction Payload */
  if ( payload == NULL )
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][POLLING.{0x%x}] "
               "Process :  NULL payload",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_POLLING );
    return FALSE;
  } /* if ( instr_payload == NULL ) */

  current_rf_mode = cal_data->status.current_rf_mode;

  event_payload = (uint8*)payload;
  cfg_data = &(cal_data->seg_data.oc_payload.polling);

  /* Continue only if the current_rf_mode is valid */
  if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS )
  {
    /* Shorten Polling Data */
    poll_data = &(cfg_data->oc_data[current_rf_mode]);

    /* NULL Pointer check */
    if ( poll_data == NULL )
    {
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][POLLING.{0x%x}] "
                 "Process :  NULL Poll Data",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_POLLING );
      return FALSE;
    } /* if ( poll_data == NULL ) */

    /* Action based on PAUSE_TO_POLL flag. TRUE means it is in the middle of
    several polling segment. FALSE means it is the first polling segment, */
    if ( cal_data->status.pause_to_poll == TRUE )
    {
      /* MIDDLE OF POLLING SEGMENTS */

      /* Increment the polling counter */
      poll_data->polling_count++;

      /* Make sure that the polling interval has not change. If it has changed,
      then it is an error condition */
      if ( poll_int != cal_data->seg_data.interval )
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][POLLING.{0x%x}] "
                   "Process : Polling duration changed [%d -> %d]",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_POLLING,
                   poll_int,
                   cal_data->seg_data.interval );
        return FALSE;
      } /* if ( poll_int != cal_data->seg_data.interval ) */

    } /* if ( cal_data->status.pause_to_poll == TRUE ) */
    else
    {
      /* FIRST POLLING SEGMENT */

      /* Based on if polling is active or not, update event_to_be_done */
      poll_data->elapsed_duration_us = 0;
      poll_data->event_to_be_done = payload->polling_event;
      cal_data->status.pause_to_poll = TRUE;
      poll_data->polling_count = 0;

      /* Store the polling interval in the segment This should never change
      throughout the polling during. If a different number is received, it is
      an error condition */
      poll_int = cal_data->seg_data.interval;
      FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][POLLING.{0x%x}] "
                 "Process : Polling Started",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_POLLING );
    } /* if ! ( cal_data->status.pause_to_poll == TRUE ) */

    /* Get the Event Flag for the current instruction */
    event_flag = poll_data->event_to_be_done;

    /* Get the event list */
    event_list = (ftm_cal_polling_event_list*)poll_data->event_check_cb;

    /* Update event_payload to point to the start of event payload */
    event_payload += sizeof(ftm_cal_polling_instr_payload_type);

    /* Loop until Event Flag is zero */  
    while ( event_flag != 0 )
    {
      /* Initialize event_met to FALSE */
      event_met = FALSE;

      /* For the current unprocessed Event Flag. The first bit will be 
      from LSB. */
      event_mask = inline_rf_extract_first_one_bit_mask( (uint32) event_flag );

      /* Call the event callback function only if the event mask is non-zero,
      i.e. only if they atleast one uncompleted action */
      if ( event_mask != 0 )
      {
        /* Extract the location of the first-one-bit LSB. This
        number will be same as the array index in event_list array. */
        event_idx = inline_rf_extract_first_one_bit_lsb_location( event_mask );

        /* Call the callback function */
        if ( ( event_idx < POLLING_EVENT_MAX ) && 
             ( event_list[event_idx].event_cb != NULL ) )
        {

          /* Execute the event callback function */
          event_met = (event_list[event_idx].event_cb( payload,
                                               (void*)event_payload ) == TRUE);

          /* Update event_payload to point to next event payload*/
          event_payload += ftm_calibration_polling_event_payload_size(event_idx);

        } /* if ( ( event_idx < POLLING_EVENT_MAX ) && 
             ( event_list[event_idx].event_cb != NULL ) ) */
        else
        {
          FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][POLLING.{0x%x}] "
                     "Process : CB skipped EVENT Index %d",
                     cal_data->seg_data.cal_instance,
                     FTM_CAL_OPCODE_POLLING,
                     event_idx );
          ret_val = FALSE;
          break; /* Break while loop */
        }
      } /* if ( event_mask != 0 ) */

      /* If the event is completed then update the event_to_be_done flag */
      if ( event_met == TRUE )
      {
        poll_data->event_to_be_done &= (~event_mask);
        FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][POLLING.{0x%x}] "
                   "Process : EVENT 0x%x done",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_POLLING,
                   event_mask );
      } /* if ( event_met == TRUE ) */

      /* Update Event Flag to set the processed bit to zero. */
      event_flag = event_flag & (~event_mask);

    } /* while ( event_flag != 0 ) */

    /* Get the elapsed time and accumulate the function time as 
    elapsed_duration_us */
    poll_data->elapsed_duration_us = (poll_data->polling_count * poll_int);

    /* Check if all the events are met or not */
    if ( poll_data->event_to_be_done == 0 ) 
    {
      /* All events are done. Thus flag to cease the polling */
      cease_polling = TRUE;
      FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][POLLING.{0x%x}] "
                 "Process : All Events done",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_POLLING );
    } /* if ( poll_data->event_to_be_done == 0 ) */
    else if ( ( payload->timeout_us != FTM_CAL_POLLING_INFINITE ) &&
              ( poll_data->elapsed_duration_us >= payload->timeout_us ) )
    {
      FTM_MSG_4( FTM_HIGH, "[calv3][Segment %3d][POLLING.{0x%x}] "
                 "Process : Timing out (T %d us TO %d us)",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_POLLING,
                 poll_data->elapsed_duration_us,
                 payload->timeout_us );
      cease_polling = TRUE;
    } /* else if ( ( poll_data->timeout_duration_us != FTM_CAL_POLLING_INFINITE )
        &&
      ( poll_data->elapsed_duration_us >= poll_data->timeout_duration_us ) ) */

    /* Cease polling if needed */
    if ( cease_polling == TRUE )
    {
      cal_data->status.pause_to_poll = FALSE;
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][POLLING.{0x%x}] "
                 "Process : POLL'ed for %d us",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_POLLING,
                 poll_data->elapsed_duration_us );
    } /* if ( cease_polling == TRUE ) */

    if ( payload->debug_mode_en == TRUE )
    {
      FTM_MSG_6( FTM_HIGH, "[calv3][Segment %3d][POLLING.{0x%x}] "
                 "Process : Event 0x%x Polling Segment %d [ %d of %d us ]",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_POLLING,
                 payload->polling_event,
                 poll_data->polling_count, 
                 poll_data->elapsed_duration_us,
                 payload->timeout_us );
    } /* if ( payload->debug_mode_en == TRUE ) */
  } /* if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */
  else
  {
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][POLLING.{0x%x}] "
               "Process : Bad RFM Mode %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_POLLING,
               current_rf_mode );
    ret_val = FALSE;
  } /* if !( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */


  /* return */
  return ret_val;

} /* ftm_calibration_polling_process */

/*! @} */

/*! @} */


