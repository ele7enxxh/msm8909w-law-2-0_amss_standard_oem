/*!
  @file
  ftm_cdma_radio.c

  @brief
  This module contains FTM CDMA Radio Control code which is used to
  exclusively control Radio for test purposes.

  @addtogroup RF_FTM_CDMA_RADIO
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/src/ftm_cdma_radio.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/08/15   rs      KW error fix
05/22/14   sty     Added ftm_cdma_radio_set_antenna()
03/21/14   spa     Off target compilation fix
03/20/14   spa     Initial support to test Rx on/off on RFLM side
08/19/13   spa     Added support for common CDMA enter mode (1x/HDR/SB)
04/18/13   aro     Added FTM debug mode configure
05/03/12   jr      Removed Klocwork warnings
12/01/11   aro     Documentation Update
11/22/11   aro     Added interface to disable iC for debug purpose
11/12/11   aro     Added CDMA driver verification sandbox function
11/12/11   aro     Added command to configure debug mode for CDMA
11/12/11   aro     Added radio command to get current ic power mode
11/12/11   aro     Added radio command to clear ic power mode override
11/12/11   aro     Added radio command to override ic power mode
11/11/11   aro     Initial Release : CDMA Radio Dispatch

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_cdma_radio.h"
#include "ftm_cdma_packet_types.h"
#include "ftm_cdma_control.h"
#include "rfm_cdma_ftm.h"
#include "ftm_msg.h"
#include "ftm_common_control.h"
#include "ftm_cdma_data.h"
#include "rfm_common.h"
#include "rfm_hdr_ftm.h"
#include "rfm_internal.h"

/*============================================================================*/
/*!
  @name CDMA Radio Control Request Packets

  @brief
  This block includes all the Request Packet definitions to be used to send
  FTM commands related to CDMA Radio Control
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_CDMA_RADIO_SET_PM_OVERRIDE Command
*/
typedef ftm_cdma_req_11_type ftm_cdma_radio_power_mode_override_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_CDMA_RADIO_CLEAR_PM_OVERRIDE Command
*/
typedef ftm_cdma_req_1_type ftm_cdma_radio_clear_power_mode_override_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_CDMA_RADIO_GET_PM Command
*/
typedef ftm_cdma_req_1_type ftm_cdma_radio_get_power_mode_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_CDMA_CONFIG_IC_OPERATION Command
*/
typedef ftm_cdma_req_14_type ftm_cdma_radio_config_ic_op_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_CDMA_DRIVER_VER_SANDBOX Command
*/
typedef ftm_cdma_req_13_type ftm_cdma_radio_driver_ver_sandbox_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_CDMA_FORCE_RX_ONOFF Command
*/
typedef ftm_cdma_req_15_type ftm_cdma_radio_force_rx_onoff_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_CDMA_CONFIG_DEBUG_MODE Command
*/
typedef ftm_cdma_req_12_type ftm_cdma_radio_config_debug_mode_req_type;

/*! @} */

/*============================================================================*/
/*!
  @name CDMA Radio Control Response Packets

  @brief
  This block includes all the Response Packet definitions to be used to receive
  reponses for FTM commands related to CDMA 1x Radio Control
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_CDMA_RADIO_SET_PM_OVERRIDE Command
*/
typedef ftm_cdma_rsp_1_type ftm_cdma_radio_power_mode_override_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_CDMA_RADIO_CLEAR_PM_OVERRIDE Command
*/
typedef ftm_cdma_rsp_1_type ftm_cdma_radio_clear_power_mode_override_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_CDMA_RADIO_GET_PM Command
*/
typedef ftm_cdma_rsp_5_type ftm_cdma_radio_get_power_mode_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_CDMA_CONFIG_IC_OPERATION Command
*/
typedef ftm_cdma_rsp_1_type ftm_cdma_radio_config_ic_op_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_CDMA_DRIVER_VER_SANDBOX Command
*/
typedef ftm_cdma_rsp_1_type ftm_cdma_radio_driver_ver_sandbox_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_CDMA_FORCE_RX_ONOFF Command
*/
typedef ftm_cdma_rsp_1_type ftm_cdma_radio_force_rx_onoff_rsp_type;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_CDMA_CONFIG_DEBUG_MODE Command
*/
typedef ftm_cdma_rsp_1_type ftm_cdma_radio_config_debug_mode_rsp_type;

/*! @} */

/*============================================================================*/
/*!
  @name CDMA Radio Control Functions

  @brief
  Functions for FTM to control the radio in CDMA Mode of Operation
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override Power Mode

  @details
  This function overrides the Intelliceiver power mode

  @param *ftm_req_data
  FTM Request Data

  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_cdma_radio_power_mode_override
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_cdma_radio_power_mode_override_req_type* header =
    (ftm_cdma_radio_power_mode_override_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cdma_radio_power_mode_override_rsp_type *cdma_rsp; /*Packet Payload*/

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;
  rfm_cdma_power_mode_type power_mode;

  /* Create a new reponse packet in the ftm heap. This will also update first
  10-bytes field in Packet Payload in the FTM response packet. These fields
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2),
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                        sizeof(ftm_cdma_radio_power_mode_override_rsp_type) );

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  power_mode = (rfm_cdma_power_mode_type)header->power_mode;

  /* Call RFM Function to set Tx power */
  rf_status = rfm_cdma_ftm_power_mode_override( device, power_mode );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp =
      (ftm_cdma_radio_power_mode_override_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Update the Response Packet Field */
  if(NULL != cdma_rsp)
  {
    cdma_rsp->rf_status = rf_status;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_2( FTM_LOW, "ftm_cdma_radio_power_mode_override: Dev %d, PM %d "
                      "[Done]", device, power_mode );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_cdma_radio_power_mode_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clear Power Mode Override

  @details
  This function clears the override mode of the Intelliceiver power mode

  @param *ftm_req_data
  FTM Request Data

  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_cdma_radio_clear_power_mode_override
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_cdma_radio_clear_power_mode_override_req_type* header =
    (ftm_cdma_radio_clear_power_mode_override_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cdma_radio_clear_power_mode_override_rsp_type *cdma_rsp;/*Packet Payload*/

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;

  /* Create a new reponse packet in the ftm heap. This will also update first
  10-bytes field in Packet Payload in the FTM response packet. These fields
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2),
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                   sizeof(ftm_cdma_radio_clear_power_mode_override_rsp_type) );

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;

  /* Call RFM Function to set Tx power */
  rf_status = rfm_cdma_ftm_clear_power_mode_override( device );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp =
   (ftm_cdma_radio_clear_power_mode_override_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Update the Response Packet Field */
  if(NULL != cdma_rsp)
  {
    cdma_rsp->rf_status = rf_status;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_1( FTM_LOW, "ftm_cdma_radio_clear_power_mode_override: Dev %d [Done]",
             device );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_cdma_radio_clear_power_mode_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get current Power Mode

  @details
  This function return the current linearity power mode of the RTR chipset.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_cdma_radio_get_power_mode
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_cdma_radio_get_power_mode_req_type* header =
    (ftm_cdma_radio_get_power_mode_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cdma_radio_get_power_mode_rsp_type *cdma_rsp; /*Packet Payload*/

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;

  /* Others */
  rfm_cdma_power_mode_type power_mode;

  /* Create a new reponse packet in the ftm heap. This will also update first
  10-bytes field in Packet Payload in the FTM response packet. These fields
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2),
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_cdma_radio_get_power_mode_rsp_type) );

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;

  /* Call RFM Function to set Tx power */
  power_mode = rfm_cdma_ftm_get_power_mode( device );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_cdma_radio_get_power_mode_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Update the Response Packet Field */
  if(NULL != cdma_rsp)
  {
    cdma_rsp->power_mode = power_mode;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( power_mode >= RFM_CDMA_POWER_MODE_MAX )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_2( FTM_LOW, "ftm_cdma_radio_get_power_mode: Dev %d, PM %d [Done]",
             device, power_mode );

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_cdma_radio_get_power_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable/Disable Intelliciver operation for debug purpose

  @details
  This function will enable or disable the iC operation on a given device

  @param *ftm_req_data
  FTM Request Data

  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_cdma_radio_configure_ic_operation
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_cdma_radio_config_ic_op_req_type* header =
    (ftm_cdma_radio_config_ic_op_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cdma_radio_config_ic_op_rsp_type *cdma_rsp; /*Packet Payload*/

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;
  boolean disable_ic;

  /* Other */
  boolean rfm_ret;

  /* Create a new reponse packet in the ftm heap. This will also update first
  10-bytes field in Packet Payload in the FTM response packet. These fields
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2),
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_cdma_radio_config_ic_op_rsp_type) );

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  disable_ic = (boolean)header->disable_ic;

  /* Call RFM Function to set Tx power */
  rfm_ret = rfm_cdma_ftm_configure_ic_operation( device, disable_ic );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_cdma_radio_config_ic_op_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Update the Response Packet Field */
  if(NULL != cdma_rsp)
  {
    cdma_rsp->rf_status = rfm_ret;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rfm_ret == FALSE )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_2( FTM_LOW, "ftm_cdma_radio_configure_ic_operation: Dev %d, "
                      "RFM ret %d [Done]", device, rfm_ret );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_cdma_radio_configure_ic_operation */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Mimics the FW calls to RFLM, to set the antenna switch to a particular 
  position. The API assumes that the device is in RXTX mode

  @param device
  RF device for which antenna switch is to be performed

  @return
  Response Packet for the API
*/
ftm_rsp_pkt_type
ftm_cdma_radio_set_antenna
(
  rfm_device_enum_type device
)
{
  ftm_rsp_pkt_type ftm_rsp_data; /* return value */
  boolean rv;

  /* Init ftm_rsp_data value to 0 */
  memset( &ftm_rsp_data, 0, sizeof(ftm_rsp_pkt_type) );
  
  rv = rfm_cdma_ftm_set_antenna(device);

  if (rv==FALSE)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;

    FTM_MSG_1( FTM_ERROR, "ftm_cdma_radio_set_antenna: Failed for Dev %d, ", 
               device );
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  }

  return ftm_rsp_data;

} /* ftm_cdma_radio_set_antenna */

/*! @} */

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
  Debug interface to test CDMA driver

  @details
  This function can be used as a sandbox area to add a test code to verify
  CDMA driver. Generic payload to the test code can be sent as 8 chunks of
  payload where each paylaod is of length 32-bit.

  @param *ftm_req_data
  FTM Request Data to verify CDMA driver

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_cdma_driver_verification_sandbox
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_cdma_radio_driver_ver_sandbox_req_type* header =
    (ftm_cdma_radio_driver_ver_sandbox_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cdma_radio_driver_ver_sandbox_rsp_type *cdma_rsp; /* Packet Payload for
                                                         each cmd */

  uint32 payload_0, payload_1, payload_2, payload_3;
  uint32 payload_4, payload_5, payload_6, payload_7;
  int32 ret_val = 1;

  payload_0 = header->payload_0;
  payload_1 = header->payload_1;
  payload_2 = header->payload_2;
  payload_3 = header->payload_3;
  payload_4 = header->payload_4;
  payload_5 = header->payload_5;
  payload_6 = header->payload_6;
  payload_7 = header->payload_7;

  /* Create a new reponse packet in the ftm heap. This will also update first
  10-bytes field in Packet Payload in the FTM response packet. These fields
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2),
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                         sizeof(ftm_cdma_radio_driver_ver_sandbox_rsp_type) );

  /*--------------------------------------------------------------------------*/
  /* START : Place the sandbox code in this area */

  /* Return code for the code should be stored in ret_val as INT32 */
  {
    /*========================================================================*/

    









    
    /*========================================================================*/
  }
  /* END : Place the sandbox code in this area */
  /*--------------------------------------------------------------------------*/

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp =
      (ftm_cdma_radio_driver_ver_sandbox_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Update the Response Packet Field */
  if(NULL != cdma_rsp)
  {
    cdma_rsp->rf_status = ret_val;
  }

  /* Send back a BAD response packet for failures */
  if ( ret_val == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_8( FTM_ERROR, "ftm_cdma_driver_verification_sandbox: Payloads "
               "{ %d, %d, %d, %d, %d, %d, %d, %d } [Failed]",
               payload_0, payload_1, payload_2, payload_3,
               payload_4, payload_5, payload_6, payload_7 );
  }
  else
  {
    FTM_MSG_8( FTM_ERROR, "ftm_cdma_driver_verification_sandbox: Payloads "
               "{ %d, %d, %d, %d, %d, %d, %d, %d } :  [Success]",
               payload_0, payload_1, payload_2, payload_3,
               payload_4, payload_5, payload_6, payload_7 );
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_cdma_driver_verification_sandbox */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure CDMA debug Mode

  @details
  Depending on the mask, this function will set various CDMA modules to debug
  mode.

  @param *ftm_req_data
  FTM Request Data to configure CDMA Debug mode

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_cdma_radio_config_debug_mode
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_cdma_radio_config_debug_mode_req_type* header =
    (ftm_cdma_radio_config_debug_mode_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cdma_radio_config_debug_mode_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  boolean rfm_ret = TRUE;

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;
  uint32 debug_mask;
  boolean debug_en;

  /* Create a new reponse packet in the ftm heap. This will also update first
  10-bytes field in Packet Payload in the FTM response packet. These fields
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2),
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                          sizeof(ftm_cdma_radio_config_debug_mode_rsp_type) );

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  debug_mask = header->debug_mask;
  debug_en = (boolean)header->debug_en;

  /* Call RFM Function to query LNA Data */
  rfm_ret &= rfm_cdma_ftm_configure_debug_mode( device, debug_mask, debug_en );
  rfm_ret &= ftm_cdma_configure_debug_mode( device, debug_mask, debug_en );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp =
      (ftm_cdma_radio_config_debug_mode_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Update the Response Packet Field */
  if(NULL != cdma_rsp)
  {
    cdma_rsp->rf_status = rfm_ret;
  }

  /* Send back a BAD response packet for failures */
  if ( rfm_ret == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_3( FTM_ERROR, "ftm_cdma_radio_config_debug_mode: Dev %d - "
               "Mask 0x%x, En %d [Failed]", device, debug_mask, debug_en );
  }
  else
  {
    FTM_MSG_3( FTM_LOW, "ftm_cdma_radio_config_debug_mode: Dev %d - Mask 0x%x, "
               "En %d [Done]", device, debug_mask, debug_en );
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_cdma_radio_config_debug_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter specified CDMA Mode (1x/HDR/SB-SVDO)

  @details
  Depending on the mode sent in, this API will enter the appropriate mode

  @param device
  Device on which requested mode needs to be enabled
 
  @param rf_mode
  Requested mode
 
  @return
  Response packet for the command
 
  @todo ADD RFSB support 
*/
boolean
ftm_cdma_radio_enter_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
)
{
  boolean ret_val = FALSE; /* return value */
  boolean init_ftm_env = FALSE;
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
 /* Response Variable*/
  int32 rf_status = 0;

  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_radio_enter_mode: Unsupported device %d", 
               device );
    return FALSE;
  }

  /* Enter selected CDMA mode */
  switch ( rf_mode )
  {
    case RFM_1X_MODE:

      /* Set up FTM environment for 1x */
      init_ftm_env = ftm_rfmode_enter( device, FTM_STATE_1X );

      (void)ftm_cdma_data_get_device_status( device, &dev_state );

      /* Call RFM Function to Enter 1x Mode */
      if ( init_ftm_env == TRUE )
      {
        /* Tear Down RF to enter 1x mode */  
        if ( !ftm_1x_tear_down(device) )
        {
           /* Enter 1x Mode */
          rf_status = ( rfm_enter_mode( device, RFM_1X_MODE, NULL, NULL,
                                       dev_state.rxlm_handle) == 0 );
        }
      }/* if ( init_ftm_env == TRUE )*/
      else
      {
        ret_val = FALSE;
      }

    break;

    case RFM_1XEVDO_MODE:

      /* Set up FTM environment for HDR */
      init_ftm_env = ftm_rfmode_enter( device, FTM_STATE_HDR );

      (void)ftm_cdma_data_get_device_status( device, &dev_state );

      /* Call RFM Function to enter HDR mode */
      if ( init_ftm_env == TRUE )
      {
        /* Tear Down RF to enter HDR mode */  
        if ( !ftm_hdr_tear_down(device) )
        {
          rf_status = rfm_enter_mode( device, RFM_1XEVDO_MODE, NULL, NULL,
                                      dev_state.rxlm_handle );
        }
      }/* if ( init_ftm_env == TRUE )*/
      else
      {
        ret_val = FALSE;
      }

    break;

     case RFM_SVDO_MODE:

      /* Set up FTM environment for SVDO */
      init_ftm_env = ftm_rfmode_enter( device, FTM_STATE_SVDO );

      (void)ftm_cdma_data_get_device_status( device, &dev_state );

      /* Call RFM Function to Enter SVDO mode */
      if ( init_ftm_env == TRUE )
      {
        rf_status = -1;/* Return failure, since complete RF support is not 
                          present yet 

        rf_status = rfm_enter_mode( device, RFM_SVDO_MODE, NULL, NULL,
                                    dev_state.rxlm_handle );*/

        FTM_MSG_1( FTM_ERROR, "ftm_cdma_radio_enter_mode:Support pending"
                              " on device: %d ", device);
      }/* if ( init_ftm_env == TRUE )*/
      else
      {
        ret_val = FALSE;
      }
    break;

    default:

    FTM_MSG_2( FTM_ERROR, "ftm_cdma_radio_enter_mode:Unsupported mode: %d,"
                          "on device: %d ", rf_mode, device);
    break;
  }/*switch ( rf_mode )*/
  

  /* Send back a BAD response packet for failures */
  if ( rf_status != 0 )
  {
    ret_val = FALSE;
    FTM_MSG_2( FTM_ERROR, "ftm_cdma_radio_enter_mode:Bad Params! mode: %d,"
                          " and  device: %d ", rf_mode, device);
  }
  else
  {
    /* Everything was done without failures, return success */
    ret_val = TRUE;
  }

  /* Return Response Packet */
  return ret_val;

} /* ftm_cdma_radio_enter_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Test out Rx on off control API in RFLM layer

  @details
  Rx on/off is called by FW for mili or micro sleep, which is basically a way
  of disabling the rx path moementarily. Differentl levels of sleep modify
  the state of rx path differently, micro sleep only disables the LNA path
  while mili will disable PLL as well

  @param device
  Device on which requested mode needs to be enabled
 
  @param rf_mode
  Requested mode
 
  @return
  Response packet for the command
 
*/
ftm_rsp_pkt_type
ftm_cdma_radio_test_rx_on_off
(
  void  *ftm_req_data
)
{
  boolean ret_val = FALSE; /* return value */

  /* Request Packet */
  ftm_cdma_radio_force_rx_onoff_req_type* header =
    (ftm_cdma_radio_force_rx_onoff_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cdma_radio_force_rx_onoff_rsp_type *cdma_rsp; /* Packet Payload for
                                                         each cmd */

  /* Extract info from payload */
  rfm_device_enum_type device = header->device;
  uint32 force_microsleep_enable_disable = header->microsleep_enable;
  uint32 rf_off_ustmr_delta = header->rf_off_ustmr_delta;
  uint32 rf_on_ustmr_delta = header->rf_on_ustmr_delta;
  uint32 duration_half_slots = header->num_halfslots;
  uint32 sleep_mode = header->sleep_mode;
  rfm_mode_enum_type curr_mode; /* Var to store current mode */
  ftm_cdma_data_status_type *dev_status; /* Device Status read Pointer*/

  /* Create a new reponse packet in the ftm heap. This will also update first
  10-bytes field in Packet Payload in the FTM response packet. These fields
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2),
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                         sizeof(ftm_cdma_radio_force_rx_onoff_rsp_type) );

  dev_status = ftm_cdma_data_get_device_ptr(device);

  if ( dev_status != NULL )
  {
    /* Get Current Mode */
    curr_mode = rfm_get_current_mode(device);

    if ( curr_mode == RFM_1XEVDO_MODE )
    {
      /* Device must be in Rx or RxTx state to test Rx on/off */
      if (( dev_status->rf_state == FTM_CDMA_STATE_RX ) ||
         (  dev_status->rf_state == FTM_CDMA_STATE_RXTX ))
      {
        /* Send command to FW to force Rx on/off with requested params */
        ret_val = rfm_hdr_ftm_test_rx_on_off( force_microsleep_enable_disable,
                                              rf_off_ustmr_delta,
                                              rf_on_ustmr_delta, 
                                              duration_half_slots,
                                              sleep_mode );
      }
      else
      {
        RF_MSG_2( RF_ERROR, "ftm_cdma_radio_test_rx_on_off: invalid device state "
                            " %d on device:%d",dev_status->rf_state, 
                           device );
        ret_val = FALSE;
      }
    }/* if ( dev_status_r->rf_mode == RFM_1XEVDO_MODE ) */
    else
    {
      RF_MSG_2( RF_ERROR, "ftm_cdma_radio_test_rx_on_off: Rx on/off not  "
                          "supported for tech %d on device:%d",
                          curr_mode, device );
      ret_val = FALSE;
    }
  }/* if ( dev_status != NULL )*/
  else
  {
    RF_MSG_1( RF_ERROR, "ftm_cdma_radio_test_rx_on_off: NULL data for "
                        "device:%d", device );
    ret_val = FALSE;
  }

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp =
      (ftm_cdma_radio_force_rx_onoff_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Update the Response Packet Field */
  if(NULL != cdma_rsp)
  {
    cdma_rsp->rf_status = ret_val;
  }

  /* Send back a BAD response packet for failures */
  if ( ret_val == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_4( FTM_ERROR, "ftm_cdma_radio_test_rx_on_off:" 
               " micro_sleep_enabled: %d | rf_off_ustmr-delta:%d |"
               " rf_on_ustmr-delta:%d | num_halfslots: %d [Failed]",
               header->microsleep_enable,header->rf_off_ustmr_delta,
               header->rf_on_ustmr_delta, header->num_halfslots );
  }
  else
  {
    FTM_MSG_4( FTM_LOW, "ftm_cdma_radio_test_rx_on_off:" 
               " micro_sleep_enabled: %d | rf_off_ustmr-delta:%d |"
               " rf_on_ustmr-delta:%d | num_halfslots: %d [Success]",
               header->microsleep_enable,header->rf_off_ustmr_delta,
               header->rf_on_ustmr_delta, header->num_halfslots );
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_cdma_radio_test_rx_on_off */

#endif /* FEATURE_FACTORY_TESTMODE */
/*! @} */

#endif /* FEATURE_CDMA1X */

/*! @} */

