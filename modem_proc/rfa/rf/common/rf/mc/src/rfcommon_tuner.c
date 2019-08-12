/*!
   @file
   rfcommon_tuner.c

   @brief
   This file contains all Tuner AOL/CL related APIs for the RFCOMMON
   Main Control layer.
 
  @addtogroup RF_MC_TUNER
  @{
   
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rfcommon_tuner.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/14   ndb/yb  SNUM debug code 
09/30/14   ndb     Add "rflm_handle" in rfcommon_tuner_update_cb_data_type
08/28/14   kg      Add tech_id in rfcommon_tuner_update_cb_data_type()
03/12/14   kg      Changes to rfcommon_tuner_open_loop_response_handler to be able
                   to handle upto 4 tuners
12/03/13   zhw     Initial revision

============================================================================*/

#include "modem_mem.h"
#include "rfcommon_tuner.h"
#include "rfcommon_atuner_intf.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to create Tuner Update Event Callback Data structure
 
  @details
  MC layer can use this API to create Tuner Update Event Callback Data structure.
  The API does memory allocation and initialization. MC layer must pass in
  a DM Buffer that is already allocated for Tuner update event
 
  @param tuner_event_buf
  Data Manager Buffer that should have been already allocated by tech. Call back
  data keeps track of this pointer to the tuner event DM buffer and use it later
  to complete a update cycle.

  @param tech_id
  Tech ID

  @param handle_id
  RFLM handle ID
 
  @return tuner_cb_data
  Pointer to Tuner Update Event Callback Data. NULL if memory allocation failed.
 
*/
rfcommon_tuner_update_cb_data_type*
rfcommon_tuner_event_create_cb_data
( 
  void* tuner_event_buf, rfm_mode_enum_type tech_id, uint32 handle_id
)
{
  rfcommon_tuner_update_cb_data_type* tuner_cb_data = NULL;

  if ( tuner_event_buf != NULL )
  {
    /* Allocate memory for call back data and initialze all members to 0 */
    tuner_cb_data = modem_mem_alloc( sizeof(rfcommon_tuner_update_cb_data_type), 
                                     MODEM_MEM_CLIENT_RFA );
    if( tuner_cb_data == NULL )
    {
      RF_MSG( RF_ERROR, "rfcommon_tuner_event_create_cb_data: mem_alloc failed" );
    }
    else
    {
      memset( tuner_cb_data, 0, sizeof(rfcommon_tuner_update_cb_data_type) );

      tuner_cb_data->tech_id = tech_id;

      tuner_cb_data->tuner_update_buffer_ptr = tuner_event_buf;
      tuner_cb_data->handle_id = handle_id;
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfcommon_tuner_event_create_cb_data: "
                      "NULL Tuner Event DM Buffer Pointer" );
  }

  return tuner_cb_data;

}/* rfcommon_tuner_event_create_cb_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed when an AOL event response is received from 
  RFLM 
 
  @details
  This function will be passed to RF-MDSP layer as a callback function to
  be executed after sending mdsp_queue_atuner_update. This function will
  acknowledge the token and free up allocation tuner script buffer and token.
 
  @param cmd_ptr
  Interface compatibilty not used.
   
  @param snum_info
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.
*/
boolean
rfcommon_tuner_open_loop_response_handler
( 
  void *cmd_ptr,
  const rf_dispatch_snum_info_type *snum_info
)
{
  boolean api_status = TRUE;
  rfcommon_tuner_update_cb_data_type *tuner_cb_data;
  uint8 num_tuner_device;
  boolean event_aborted;
  uint32 rflm_handle = 0xFFFFFFFF;
  rfm_mode_enum_type tech_id = RFM_INVALID_MODE;

  /* NULL Pointer check */
  if ( snum_info == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_tuner_open_loop_response_handler: NULL SNUM Data" );
    return FALSE;
  } /* if ( snum_info == NULL ) */

  tuner_cb_data = (rfcommon_tuner_update_cb_data_type*)(snum_info->snum_cb_data);

  /* NULL Pointer check */
  if ( tuner_cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_tuner_open_loop_response_handler: NULL CB Data" );
    return FALSE;
  } /* if ( fws_cfg_cb != NULL ) */

  rflm_handle = ((rfcommon_tuner_update_cb_data_type *)tuner_cb_data)->handle_id;

  tech_id = ((rfcommon_tuner_update_cb_data_type *)tuner_cb_data)->tech_id;

  /* Check if an event is aborted by RFLM. If aborted, do NOT acknowledge token
  and reset CCS event smem status */
  event_aborted = rfcommon_mdsp_tuner_open_loop_check_aborted_event( 
                                      tuner_cb_data->tuner_update_buffer_ptr);

  /* If not aborted, acknowledge Tuner token */
  if ( event_aborted == FALSE )
  {
    for( num_tuner_device = 0; num_tuner_device < (RFC_MAX_DEVICE_INSTANCES_PER_BAND -1); 
         num_tuner_device++  )
    {
      if( tuner_cb_data->tuner_data[num_tuner_device].ant_tuner != NULL )
      {
        api_status &= rfcommon_atuner_manager_acknowledge_script( 
                                               tuner_cb_data->tuner_data[num_tuner_device].ant_tuner, 
                                               tuner_cb_data->tuner_data[num_tuner_device].tuner_token );
      }
    }
  } /* if ( event_aborted == FALSE ) */
  else 
  {
    rfcommon_mdsp_ccs_abort_event( &(tuner_cb_data->tuner_ccs_event_handle) );
  } /* if ( event_aborted == TRUE ) */

  if( api_status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfcommon_tuner_open_loop_response_handler: "
                      "Tuner Driver Ackowledge error" );
  }

  /* Always reset Tuner Open Loop Event Status at the end of response handling
  to allow for the next update */
  
  RF_MSG_3( RF_HIGH,"rfcommon_tuner_open_loop_response_handler: SNUM 0x%08x,"
                    "rflm_handle %X, tech_id: %d", 
                    snum_info->snum, rflm_handle, tech_id);
  
  api_status &= rfcommon_mdsp_tuner_open_loop_reset_status( 
                                      tuner_cb_data->tuner_update_buffer_ptr);

  /* Clean up CCS handle and Callback data object */
  rfcommon_tuner_event_cleanup( tuner_cb_data );

  if ( api_status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfcommon_tuner_open_loop_response_handler: "
                      "Tuner Driver Ackowledge error" );
  }
  return api_status; 
}/* rfcommon_tuner_open_loop_response_handler */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed after mdsp_queue_atuner_update 
 
  @details
  This function will be passed to RF-MDSP layer as a callback function to
  be executed in case mdsp_queue_atuner_update needs to be executed before 
  FW response happens, hence not executing the registered fwrsp call back 
  function. This function free up resources that should have been cleared by the 
  registered fwrsp call back function and also reset all the state variables.
  
  @param registered_cb_data
  callback data that got passed in along with the registered call back. It was 
  meant to be handled by the registered call back but since registered call back 
  never got executed, we handle the call back data here including cleaning up 
*/
void
rfcommon_tuner_event_cleanup
( 
  void *registered_cb_data
)
{
  rfcommon_tuner_update_cb_data_type *tuner_cb_data;

  tuner_cb_data = (rfcommon_tuner_update_cb_data_type*)(registered_cb_data);

  /* NULL Pointer check */
  if ( tuner_cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_mdsp_tuner_update_abort_cb: NULL CB Data" );
    return;
  } /* if ( fws_cfg_cb != NULL ) */

  /* Clean up CCS event handle */
  rfcommon_mdsp_ccs_cleanup_event( &(tuner_cb_data->tuner_ccs_event_handle) );

  modem_mem_free( tuner_cb_data, MODEM_MEM_CLIENT_RFA );

  return;
}/* rfcommon_tuner_event_cleanup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function for AOL event snum req/res for debug information.
 
  @param client_debug_info
  Pointer to the debug data

  @param snum_debug_info
  Snum debug information 
*/

void rfcommon_aol_tuner_snum_debug_callback
(
  void *client_debug_info, 
  rf_dispatch_snum_debug_info snum_debug_info
)
{
  rfm_mode_enum_type tech_id = RFM_INVALID_MODE;
  uint32 rflm_handle = 0xFFFFFFFF;

  if(client_debug_info == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_aol_tuner_snum_debug_callback: NULL CB Data" );
    return;
  }

  tech_id = ((rfcommon_tuner_update_cb_data_type *)client_debug_info)->tech_id;

  rflm_handle = ((rfcommon_tuner_update_cb_data_type *)client_debug_info)->handle_id;

  RF_MSG_4( RF_MED, "rfcommon_aol_tuner_snum_debug_callback: for Tech_id:%d, rflm_handle:%d, snum:0x%x, spare_snums:%d",
  	              tech_id,rflm_handle,snum_debug_info.snum, snum_debug_info.num_of_spare_snums);

  if ((tech_id == RFCOM_WCDMA_MODE) && (snum_debug_info.num_of_spare_snums<1))
  {
    ERR_FATAL("WCDMA AOL Debug crash: Snum-%d",snum_debug_info.snum,0,0);
  }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function for CL event snum req/res for debug information
 
  @param client_debug_info
  Pointer to the debug data

  @param snum_debug_info
  Snum debug information 
*/

void rfcommon_cl_tuner_snum_debug_callback(
  void *client_debug_info, 
  rf_dispatch_snum_debug_info snum_debug_info
)
{
  rfm_mode_enum_type tech_id = RFM_INVALID_MODE;
  uint32 rflm_handle = 0xFFFFFFFF;

  if(client_debug_info == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_cl_tuner_snum_debug_callback: NULL CB Data" );
    return;
  }

  tech_id = ((rfcommon_tuner_update_cb_data_type *)client_debug_info)->tech_id;

  rflm_handle = ((rfcommon_tuner_update_cb_data_type *)client_debug_info)->handle_id;

  RF_MSG_4( RF_MED, "rfcommon_cl_tuner_snum_debug_callback: for Tech_id:%d, rflm_handle:%d snum:0x%x, num_of_spare_snums:%d",
  	              tech_id,rflm_handle,snum_debug_info.snum,snum_debug_info.num_of_spare_snums);
}
