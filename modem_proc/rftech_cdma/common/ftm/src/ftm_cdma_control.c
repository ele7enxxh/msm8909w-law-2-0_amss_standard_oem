/*!
  @file
  ftm_cdma_control.c

  @brief
  This module contains FTM CDMA Control code.
 
  @addtogroup RF_FTM_COMMON_CONTROL
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/src/ftm_cdma_control.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/19/14   spa     Added FTM concurrecny manager check,done in single enable API
04/12/13   aro     Debug enable interfaces to enable calv3 debug mode
03/19/13   APU     Replaced memcpy with memscpy.
10/30/12   spa     Use RFM_DEVICE_3 as diversity device on SV path
10/20/11   adk     Added support for SV diversity.
09/26/11   sar     Fixed KW errors. 
09/12/11   sar     Fixed compiler warning. 
09/12/11   sar     Fixed KW errors. 
08/09/11   aro     Moved send FTM command from CDMA to FTM common
07/27/11   aro     Added generic FTM send command function for CDMA
06/13/11   aro     Added F3 Message
05/24/11   bmg     CDMA 1x SV chain updates
03/28/11   aro     Renamed function to ftm_cdma_send_command
03/28/11   aro     [1] Changed ftm_cdma_create_ftm_q_msg() signature to return
                   flag indicating success and failure
                   [2] Added functionality to send FTM message from this func
03/28/11   aro     Fixes to create FTM Command Queue Message
03/28/11   aro     Added function to create FTM Queue message
02/02/11   aro     Added FTM CDMA Init
02/01/11   aro     Moved malloc function to FTM Common
01/15/11   aro     Initial Release

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
/* memscpy */
#include "stringl.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_cdma_control.h"
#include "ftm_cdma_data.h"
#include "ftm_msg.h"
#include "diagcmd.h"
#include "rf_test_task_v.h"
#include "rfm_cdma_ftm.h"
#include "ftm_common_concurrency_manager.h"
#include "rfm_1x.h"
#include "rfm_hdr.h"

/*============================================================================*/
/*!
  @name FTM Main Control Interface

  @brief
  This section holds all FTM Main Control Interfaces.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common FTM Driver Initialization

  @details
  This function will configure FTM driver state to a known value. Any previous 
  configuration information is lost.  Internal FTM driver sub-modules are also 
  returned to their initial state.  The FTM Driver Initialization process is 
  hardware independent.
 
  @return
  Flag indicating whether FTM Initialization is successful
*/
boolean
ftm_cdma_init
(
  void
)
{
  boolean ret_val = TRUE; /* Return Value */

  /* Initialize FTM CDMA Data Structure */
  ret_val &= ftm_cdma_data_init_device_status();

  FTM_MSG( FTM_LOW, "CDMA Control : initialized" );

  return ret_val;

} /* ftm_cdma_init */


/*! \} */

/*============================================================================*/
/*!
  @name FTM Task

  @brief
  This section contains the functions related to FTM Task.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create and Push Message for FTM Queue

  @details
  This function will create and push the FTM Command message to be placed on the 
  FTM queue. FTM Data Pointer and FTM Request Packet pointers which are
  dynamically allocated in this function will be freed at the end of
  ftm_req() execution.

  @param device
  RF Device for which FTM Command Message is to be created. This is requried
  to support legacy design. Depending on this device information, Sub-system
  command code is decided (FTM_1X_C or FTM_1X_RX_2_C).

  @param ftm_cmd_param
  FTM command Parameter for which the Messgae is created

  @return
  Flag indicating the success or failure of FTM Queue message creation  
  corresponding to the given FTM command parameter.
*/
boolean
ftm_cdma_send_command
(
  rfm_device_enum_type device,
  ftm_rf_factory_parms *ftm_cmd_param
)
{
  /* Define a generic FTM Data Header. This variabled will be used to populate
  only upto 10 bytes field of ftm_message->ftm_data_ptr. If the command needs 
  more fields beyond 10 bytes, they should be populated by the calling function.
  This fucntion will only populate the common fields among all FTM commands. */
  ftm_pkt_type ftm_data;

  /* Define a Pointer which will be used to cast the ftm_data.header field.
  This is allocated in Stack */
  ftm_diagpkt_subsys_header_type *ftm_data_header;

  /* Allocate a instance of memory for command to be linked on the queue.
  This will be freed when the command is executed by FTM Task. Freeing is done
  at the end of ftm_req() */
  ftm_msg_type *ftm_message;
  ftm_data.header.opaque_header[0] = 0;

  /* Perdorm typecasting for ftm_data.header */
  ftm_data_header = (ftm_diagpkt_subsys_header_type*)
                       (&(ftm_data.header.opaque_header));

  /* Populate FTM Data */
  /* --------------------------------------------------------- */
  ftm_data_header->cmd_code = DIAG_SUBSYS_CMD_F; /* DIAG_SUBSYS_CMD_F */
  ftm_data_header->subsys_id = FTM;  /* FTM : RF Sub System */

  /* Legacy support: There are two FTM subsystem command codes depending on the
  device. Following check is added to support the Legacy subsys command code.*/
  if ( device == RFM_DEVICE_0 ) /* Device 0 */
  {
    ftm_data_header->subsys_cmd_code = FTM_1X_C0_C; /* RF Mode */
  }
  else if ( device == RFM_DEVICE_1 ) /* Device 1 */
  {
    ftm_data_header->subsys_cmd_code = FTM_1X_C1_C; /* RF Mode */
  }
  else if ( device == RFM_DEVICE_2 ) /* Device 2 */
  {
    ftm_data_header->subsys_cmd_code = FTM_1X_C2_C; /* RF Mode */
  }
  else if ( device == RFM_DEVICE_3 ) /* Device 3 */
  {
    ftm_data_header->subsys_cmd_code = FTM_1X_C3_C; /* RF Mode */
  }
  else /* Invalid Device */
  {
    FTM_MSG_2( FTM_ERROR, "ftm_cdma_create_ftm_q_msg : Invalid Device %d for "
               " Command %d", device, ftm_cmd_param->ftm_rf_cmd );
    return FALSE;
  }

  /* Allocate a instance of memory for command to be linked on the queue.
  This will be freed when the command is executed by FTM Task. Freeing is done
  at the end of ftm_req() */
  ftm_message = ftm_malloc(sizeof(ftm_msg_type));

  /* Continue only if the Malloc did not fail */
  if ( ftm_message != NULL )
  {
    /* Allocate memory. This will de-allocated when ftm_req() is executed */
    ftm_message->ftm_data_ptr = (void *)ftm_malloc( sizeof( ftm_pkt_type ) );

    /* Continue only if the Malloc did not fail */
    if ( ftm_message->ftm_data_ptr )
    {
      size_t data_length = 0 ; 

      /* Length of Packet */
      ftm_message->req_pkt_len = sizeof( ftm_pkt_type );

      /* Copy the RF Parameter for the new command */
      data_length = sizeof(ftm_rf_factory_parms) ; 
      memscpy ( &(ftm_data.rf_params), data_length ,
                ftm_cmd_param , data_length ) ;
           
      /* Fill the ftm_data_ptr with the ftm data (ftm_data) */
      data_length = sizeof( ftm_pkt_type ) ;
      memscpy( ftm_message->ftm_data_ptr , data_length ,
              &(ftm_data) , data_length ) ;
      
      /* No-done queue. This will be used in ftm_req() */
      ftm_message->hdr.done_q_ptr = NULL;
      
      /* Put it on the FTM Command queue */
      ftm_cmd( ftm_message );
    }
    else
    {
      /* Since Malloc for FTM Data failed which resulted in not sending
      Command to FTM Queue, Free the memory allocated for FTM Request Packet */
      ftm_free( ftm_message );
    }
  }

  /* Return success */ 
  return TRUE;

} /* ftm_cdma_send_command */

/*! \} */

/*============================================================================*/
/*!
  @name FTM Request/Response Packet Creation Functions

  @brief
  Functions to create Request and/or Response Packets.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create CDMA Response Packet for Diag.

  @details
  This function creates good or bad response packet depending on the status
  passed as argument. In case of bad response packet, the packet includes
  FTM_RSP_BAD_PARM as response command. On the other hand, good packet uses
  FTM_RSP_DO_LEGACY asresponse command.

  @param status
  Flag to indicate whether to create good packet or bad packet.

  @return
  CDMA Response Packet
*/
ftm_rsp_pkt_type
ftm_cdma_create_rsp_pkt
(
  ftm_cdma_rsp_type status
)
{
  /* Good/Bad Response Packet */
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_MAX, 0, NULL, FALSE};

  switch( status )
  {
  case FTM_CDMA_BAD_RSP: /* Create Bad Response Packet */
    rsp_pkt.cmd = FTM_RSP_BAD_PARM;
    break;
  case FTM_CDMA_GOOD_RSP: /* Create Good Response Packet */
    rsp_pkt.cmd = FTM_RSP_DO_LEGACY;
    break;
  case FTM_CDMA_NO_RSP: /* Create No Response Packet */
    rsp_pkt.cmd = FTM_RSP_NO_RSP;
    ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents
    ftmdiag_diag_free() from being called */
    break;
  default:
    FTM_MSG_1( FTM_ERROR, "CDMA Control : Bad FTM Command %d", status );
    break;
  }

  return rsp_pkt;
} /* ftm_cdma_create_rsp_pkt */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Malloc 1xHDR Response Packet for Diag.

  @details
  This function Mallocs 1xHDR Response Packet for Diag.

  @param ftm_req_data
  FTM Request Data

  @param rsp_pkt_size
  Size of Response Packet to allocate sufficient memory

  @return
  Response Packet
*/
ftm_rsp_pkt_type
ftm_cdma_malloc_rsp_pkt
(
  void * ftm_req_data,
  uint16 rsp_pkt_size
)
{
  /* Request Packet : Used extract the FTM Command only */
  ftm_composite_cmd_header_type* header = ftm_req_data;

  ftm_rsp_pkt_type rsp_pkt;
  ftm_composite_cmd_header_type *cdma_rsp;/* Packet Payload for each command */

  /* Create a new reponse packet in the ftm heap */
  rsp_pkt = ftmdiag_create_new_pkt( rsp_pkt_size );  
  if( rsp_pkt.pkt_payload != NULL )
  {
    cdma_rsp = rsp_pkt.pkt_payload;
    cdma_rsp->diag_hdr.cmd_code = header->diag_hdr.cmd_code;
    cdma_rsp->diag_hdr.subsys_id = header->diag_hdr.subsys_id;
    cdma_rsp->diag_hdr.subsys_cmd_code = header->diag_hdr.subsys_cmd_code;
    cdma_rsp->ftm_hdr.cmd_id = header->ftm_hdr.cmd_id;
    cdma_rsp->ftm_hdr.cmd_data_len = header->ftm_hdr.cmd_data_len;
    cdma_rsp->ftm_hdr.cmd_rsp_pkt_size = rsp_pkt_size;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_cdma_malloc_rsp_pkt : rsp_pkt is NULL");
  }
  return rsp_pkt;
} /* ftm_cdma_malloc_rsp_pkt */

/*! \} */

/*============================================================================*/
/*!
  @name Debug

  @brief
  Functions used for CDMA debugging
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Configure CDMA debug Mode
 
  @details
  Depending on the mask, this function will set various CDMA modules to debug
  mode. This API is equivalent to the following in command in QXDM command
  line:
 
  SendRawRequest 75 11 39 0 04 48 0 0 0 0 [device] [debug_mask] [debug_en]
  [device] is of length 1 byte.
  [debug_mask] is 4 byte long. Should be converted to raw bytes from uint32.
  [debug_en] is 4 byte long. Should be converted to raw bytes from uint32.
  To convert from uint32 to the raw request bytes, convert from decimal to hex
  first, then reverse all the bytes.
  e.g.
  debug_mask = 64 (dec) will result in raw bytes: 64 0 0 0 
  e.g.
  [device] = 0, [debug_mask] = 64, [debug_en] = 1 will require a QXDM command:
  SendRawRequest 75 11 39 0 04 48 0 0 0 0 0 64 0 0 0 1 0 0 0
 
  @param debug_mask
  Mask indicating the CDMA module whose debug mode is to be configured.
  The definition of mask is found in #rfm_cdma_ftm_config_debug_mode_mask_type
 
  @param debug_en
  Flag indicating, if the debug mode is to be enabled or not.
 
  @return
  Status indicating, if the debug mode is enabled or not.
*/
boolean
ftm_cdma_configure_debug_mode
(
  rfm_device_enum_type device,
  uint32 debug_mask,
  boolean debug_en
)
{
  boolean ret_val = TRUE;

  /* Handle flags that are not device specific */
  if ( (debug_mask & RFM_CDMA_CONFIG_CALV3_DEBUG) > 0 )
  {
    ftm_cdma_cal_v3_data_type *cal_data = NULL;

    /* Get Calibration Data */
    cal_data = ftm_cdma_data_get_cal_v3_adr(device);

    /* Perform NULL pointer check */
    if ( cal_data != NULL )
    {
      if ( debug_en == TRUE )
      {
        cal_data->debug_en = TRUE;
      } /* if ( debug_en == TRUE ) */
      else
      {
        cal_data->debug_en = FALSE;
      } /* if ! ( debug_en == TRUE ) */
    } /* if ( cal_data != NULL ) */
    else
    {
      ret_val = FALSE;
      FTM_MSG_1( FTM_ERROR, "ftm_cdma_configure_debug_mode: Null Calv3 data"
                            "for device %d", device );
    } /* if ! ( cal_data != NULL ) */
  } /* if ! ( (debug_mask & RFM_CDMA_CONFIG_CALV3_DEBUG) > 0 ) */

  return ret_val;

} /* ftm_cdma_configure_debug_mode */

/*! @} */


/*!
  @brief
  Return the PA switchpoints for given tx device and band
 
  @param rfm_device_enum_type: tx_device.
  Tx device for which PA SWPTS are queried.
 
  @param rfcom_cdma_band_type: band
 
  @param pa_rise_swpts: pointer to an array for PA rise
  switchpoints
 
  @param pa_fall_swpts: pointer to an array for PA fall
  switchpoints
 
  @param active_pa_states: number of active PA states
 
  @retval TRUE if PA switchpoints are populated in NV, FALSE if
  PA switchpoints are not populated in NV
*/

boolean 
ftm_cdma_get_pa_switchpoints
( 
  rfm_device_enum_type      tx_device , 
  rfm_cdma_band_class_type  band , 
  int16                     pa_rise_swpts[] , 
  int16                     pa_fall_swpts[] , 
  int16*                    active_pa_states
) 
{
  return ( 
     rfm_cdma_ftm_get_pa_switchpoints ( tx_device , band , pa_rise_swpts , 
                                    pa_fall_swpts , active_pa_states ) ) ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Validate if FTM concurrency manager allows for the tech to tune to the device
  and band passed in

  @details
  The API tries to emulate what TRM does before granting a particular device to
  L1 for tune. It checks with the FTM concurrency manager if tune is allowed
  on a particular band and device

  @param rf_mode
  Requested mode
 
  @param device
  Device on which requested mode needs to be enabled
 
  @param rf_band
  Requested band
 
  @return
  TRUE if tune is allowed by FTM concurrency manager
  FALSE if tune is not allowed by FTM concurrency manager
 
*/
static boolean
ftm_cdma_control_is_tune_allowed
(
  rfm_mode_enum_type tech,
  rfm_device_enum_type rf_dev,
  rfm_cdma_band_class_type rf_band
)
{
  boolean ret_val; /* return value */
  /* for ftm concurrency manager */
  ftm_concurrency_manager_params_type conmgr_params;
  rfc_rx_path_type rx_path = RFC_PRX_PATH ;  
  rfcom_band_type_u rfcom_band ;  

  /* Populate CDMA member of the union*/
  rfcom_band.cdma_band = rf_band ;

  /* Populate payload for concurrency manager */
  conmgr_params.band = rfcom_band;
  conmgr_params.carrier_index = 0;
  conmgr_params.rfm_dev = rf_dev;
  conmgr_params.tech = tech;

  /* Use DRx path for device 1/3 */
  if ( ( rf_dev == RFM_DEVICE_1 ) || ( rf_dev == RFM_DEVICE_3 ) )
  {
    rx_path = RFC_DRX_PATH ;
  }

  conmgr_params.prx_drx = rx_path;

  /* Check with FTM concurrency manager if the tech is allowed to tune 
  to the device, band combination*/
  ret_val = ftm_concurrency_manager_validate_operation(&conmgr_params);

  /* Throw error if FTM concurrency manager returns failure */
  if ( ret_val == FALSE)
  {
    FTM_MSG_4 ( FTM_ERROR , "ftm_cdma_control_validate_concurrency: "
                 "ftm_concurrency_manager_validate_operation() has failed"
                 "for 1x ||Dev: %d | Band: %d | Mode: %d | Path: %d || " ,
                 rf_dev , rf_band , tech , rx_path ) ;
  }

  /* Return if tune is allowed or not */
  return ret_val;

} /* ftm_cdma_control_validate_concurrency */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the requested tech (1x/HDR) if FTM concurrency manager allows for it
  
  @details
  The API checks with FTM concurrency manager if a tech is allowed to 
  tune to a particular device and band combination, if the concurrency manager
  allows for it, it proceeds to call the "enable" API for the requested tech
 
  @param tech
  Requested tech
 
  @param device
  Device on which requested mode needs to be enabled
 
  @param band_chan
  Requested band/chan from FTM module
 
  @param rxlm_handle
  The rxlm_handle for which tune is requested
 
  @param callback
  A function pointer that will be called when the enabling sequence
  is complete.  This function will be passed the RFM_1X_ENABLE_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.
 
  @return
  TRUE if enable was called on requested tech successfully
  FALSE if enable was failure
 
*/
boolean
ftm_cdma_control_enable_tech
(
  rfm_mode_enum_type tech,
  rfm_device_enum_type rf_dev,
  sys_channel_type band_chan,
  lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  boolean ret_val; /* return value */

  /* Validate tech input */
  if ( (tech == RFM_1X_MODE ) || (tech == RFM_1XEVDO_MODE ))
  {
    if ( ftm_cdma_control_is_tune_allowed( tech, rf_dev, 
                    rf_cdma_sys_band_type_to_rf_band_type (band_chan.band) ))
    {
      if ( tech == RFM_1X_MODE )
      {
        ret_val = (rfm_1x_enable( rf_dev, rxlm_handle, 
                                   band_chan, callback, 
                                   userdata ) >= 0) ;
      }
      else
      {
        ret_val = (rfm_hdr_enable( rf_dev, rxlm_handle, band_chan,
                                    callback, userdata ) >= 0 );
      } 
    }/* if ( ftm_cdma_radio_validate_concurrency( ....*/
    else
    {
      ret_val = FALSE;
      FTM_MSG_3 ( FTM_ERROR , "ftm_cdma_control_enable_tech: "
                   "Concurrency manager returned failure for"
                   " tech:%d for device:%d and band:%d ",
                   tech ,rf_dev ,
                  rf_cdma_sys_band_type_to_rf_band_type (band_chan.band) ) ;
    }
  }/* if ( (tech == RFM_1X_MODE ) || (tech == RFM_1XEVDO_MODE )) */
  else
  {
    ret_val = FALSE;
    FTM_MSG_3 ( FTM_ERROR , "ftm_cdma_control_enable_tech: "
                 "Invalid tech:%d for device:%d and band:%d ",
                 tech ,rf_dev ,
                rf_cdma_sys_band_type_to_rf_band_type (band_chan.band) ) ;
  }

  /* Return if tune is allowed or not */
  return ret_val;

}/* ftm_cdma_control_enable_tech */

#endif /*FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_CDMA1X */
/*! @} */

