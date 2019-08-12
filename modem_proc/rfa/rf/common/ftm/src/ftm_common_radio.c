/*!
  @file
  ftm_common_radio.c

  @brief
  This module contains FTM common Radio Control code which is used to 
  exclusively control Radio for test purposes.
 
  @addtogroup RF_FTM_COMMON_RADIO
  @{
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_radio.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/06/14   aro     Support to override ILPC mode of operation
05/22/14   sty     Move CDMA-specific API into rftech_cdma
02/18/14   vbh     Initial revision 
01/28/14   zhh     Update WCDMA Radio Test Interface 
10/23/13   nrk     Fix compiler warnings
08/19/13   spa     Added FTM_COMMON_RADIO_ENTER_MODE command
07/23/13   aro     Added inteface to do immediate write
07/22/13   aro     Added immediate flag for rfm_set_antenna()
07/18/13   aro     Compiler warning fix
07/17/13   aro     Enabled rfm_set_antenna() FTM testing
07/16/13   aro     Added ASDIV FTM mode testing support
04/02/13   aro     Command for enable debug level for calv3
03/11/13   sar     Updated file for APQ, GNSS only target.
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
03/01/13   aro     Added command to perform HW timer test
01/23/13   aro     Command to perform slave resource configuration in 
                   Tx resource manager
01/18/13   aro     Removed nikel-XPT specific code
12/27/12   cd      Fixed invalid values for common FW override
12/18/12   cd      Add support to override delay value to FW
12/11/12   cri     Move to common DPD buffer allocation
11/20/12   gvn     Featurize LTE for Triton 
11/16/12   Saul    Sample Capture. Use common response.
11/13/12   Saul    Common XPT. Reverted changes made to commit_dpd_data.
11/08/12   cd      Common XPT will override the tables to common FW
10/22/12   Saul    Common XPT. Use bank index/offset from saved DPD data.
10/19/12   Saul    COMMON XPT. Added comments.
10/12/12   Saul    Common XPT. Load DPD framework.
10/09/12   cri     LTE DPD IQ capture MSGR support and bug fixes 
09/21/12   kai     Removed QFE1510 function
09/19/12   Saul    CDMA Sample Capture. Capture type, do leakage flag
08/31/12   cri     Added IQ capture interface for LTE
08/01/12   aro     Added EPT override Test function
07/31/12   aro     Updated single IQ capture interface
06/21/12    vb     Added support for QFE1510
06/21/12   aro     Compiler warning fix
06/13/12   aro     Fixed single instance IQ Capture
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/08/12   aro     EPT IQ capture command
03/18/12   aro     Added function to set the SAR state
03/18/12   aro     Initial Release : Based on SVDO Design

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"


#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_common_radio.h"
#include "rfm_common_sar.h"
#include "ftm_common_control.h"
#include "ftm_msg.h"
#include "ftm_common_xpt.h"
#include "ftm_common_timer.h"
#include "ftm_common_calibration_v3.h"
#include "rfm.h"

#include "rfm_internal.h"

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "ftm_1x_radio.h"
#endif

#include "rfm_ftm.h"

#ifdef FEATURE_LTE
#include "ftm_lte_xpt.h"
#endif


#include "rfcommon_core_xpt.h"
#include "rfcommon_mc.h"

#if defined(FEATURE_HDR)||defined(FEATURE_CDMA1X)
#include "ftm_cdma_radio.h"
#endif

#if defined FEATURE_WCDMA
extern boolean ftm_wcdma_radio_enter_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
);
#endif

/*============================================================================*/
/*!
  @name SAR Control Functions

  @brief
  Functions for FTM to control SAR
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the RF driver SAR state as if the QMI interface had been called.

  @details
  This function simulates what occurs when the modem QMI interface is called
  to set the SAR state.  It can be used to test the DSI SAR behavior of
  the RF driver.

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_common_radio_set_sar_state
(
  void  *ftm_req_data
)
{
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  PACK(struct) ftm_common_set_sar_state_req_s /* Input parameter structure */
  {
    uint8   cmd;
    uint8   sub_sys;
    uint16  mode;
    uint16  cdma_cmd;
    uint16  req_len;
    uint16  rsp_len;
    uint8   sar_state;
  } *sar_state_req_ptr;

  sar_state_req_ptr = (struct ftm_common_set_sar_state_req_s *)ftm_req_data;

  rfm_common_sar_set_state(sar_state_req_ptr->sar_state);

  FTM_MSG_1( FTM_LOW, "ftm_common_radio_set_sar_state: State %d : [Done]",
             sar_state_req_ptr->sar_state );

  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  return ftm_rsp_data;

} /* ftm_common_radio_set_sar_state */

/*! @} */

/*============================================================================*/
/*!
  @name HW Timer Test

  @brief
  This section contains used to test HW timers
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Slave Resource to be in enabled or disabled

  @details
  This function confugres Tx resource manager block to enable or disable
  the managing of slave resource

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_common_radio_test_hw_timer
(
  void  *ftm_req_data
)
{
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  PACK(struct) ftm_common_test_hw_timer_req_s /* Input parameter str */
  {
    uint8   cmd;
    uint8   sub_sys;
    uint16  mode;
    uint16  cdma_cmd;
    uint16  req_len;
    uint16  rsp_len;
    uint16  interval_us;
    uint16  num_of_segments;
    uint8  var_seg_len;
    uint8  timer_flag;
  } *test_hw_timer_req_ptr;

  test_hw_timer_req_ptr = 
       (struct ftm_common_test_hw_timer_req_s *)ftm_req_data;

  ftm_timer_hw_test( test_hw_timer_req_ptr->interval_us,
                     test_hw_timer_req_ptr->num_of_segments,
                     test_hw_timer_req_ptr->var_seg_len,
                     test_hw_timer_req_ptr->timer_flag );

  FTM_MSG_4( FTM_LOW, "ftm_common_radio_test_hw_timer: [Done] - "
             "Interval %d, NumSeg %d, Var %d, TimerFlag %d",
             test_hw_timer_req_ptr->interval_us,
             test_hw_timer_req_ptr->num_of_segments,
             (boolean)test_hw_timer_req_ptr->var_seg_len,
             test_hw_timer_req_ptr->timer_flag );

  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  return ftm_rsp_data;

} /* ftm_common_radio_test_hw_timer */

/*! @} */

/*============================================================================*/
/*!
  @name Cal v3

  @brief
  This section contains functions pertaining to cal v3
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure calv3 debug level

  @details
  This function configures calv3 to appropriate debug level

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_common_radio_calv3_debug
(
  void  *ftm_req_data
)
{
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  PACK(struct) ftm_common_calv3_dbg_ptr_req_s /* Input parameter str */
  {
    uint8   cmd;
    uint8   sub_sys;
    uint16  mode;
    uint16  cdma_cmd;
    uint16  req_len;
    uint16  rsp_len;
    uint16  debug_level;
    uint8   active_flag;
  } *calv3_dbg_ptr;

  calv3_dbg_ptr = 
       (struct ftm_common_calv3_dbg_ptr_req_s *)ftm_req_data;

  ftm_calibration_set_debug_level( 
     (ftm_cal_debug_level_type)calv3_dbg_ptr->debug_level,
     (boolean)calv3_dbg_ptr->active_flag );

  FTM_MSG_1( FTM_LOW, "ftm_common_radio_calv3_debug: [Done] - "
             "Level %d",
             calv3_dbg_ptr->debug_level );

  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  return ftm_rsp_data;

} /* ftm_common_radio_calv3_debug */

/*! @} */

/*============================================================================*/
/*!
  @name Antenna Switch Diversity

  @brief
  This section contains functions pertaining to Antenna Switch Diversity
*/
/*! @{ */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exercise the rfm_set_antenna() API

  @param *ftm_req_data
  FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_common_radio_set_antenna
(
  void  *ftm_req_data
)
{
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  boolean rfm_ret;
  PACK(struct) ftm_common_set_antenna_req_s /* Input parameter structure */
  {
    uint8   cmd;
    uint8   sub_sys;
    uint16  mode;
    uint16  cdma_cmd;
    uint16  req_len;
    uint16  rsp_len;
    uint8   device;
    uint8   position;
    uint8   action_imm;
  } *antenna_req_ptr;

  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY; /* start with good return value */

  antenna_req_ptr = (struct ftm_common_set_antenna_req_s *)ftm_req_data;

  if ( (boolean)antenna_req_ptr->action_imm == TRUE )
  {
    rfm_ret = rfm_set_antenna_imm( antenna_req_ptr->device, 
                                   antenna_req_ptr->position,
                                   NULL, 
                                   NULL );
  } /* if ( (boolean)antenna_req_ptr->action_imm == TRUE ) */
  else
  {
    rfm_mode_enum_type mode = rfm_get_current_mode(antenna_req_ptr->device) ;
    rfm_ret = rfm_set_antenna( antenna_req_ptr->device, 
                               antenna_req_ptr->position,
                               NULL, 
                               NULL );

    /* call tech-specific ASDIV API */
    switch ( mode )
    {
      case RFM_1X_MODE:
      case RFM_1XEVDO_MODE:
      case RFM_SVDO_MODE:
      #if defined(FEATURE_HDR)||defined(FEATURE_CDMA1X)
        /* Call CDMA specific API to set antenna position */
        ftm_rsp_data = ftm_cdma_radio_set_antenna( antenna_req_ptr->device );
      #endif
      break;

      default:
      break;
    } /* switch ( mode ) */

  } /* if ! ( (boolean)antenna_req_ptr->action_imm == TRUE ) */

  FTM_MSG_4( FTM_LOW, "ftm_common_radio_set_antenna: [Status %d] "
             "Device %d, Position %d, Action IMM %d : [Done]", 
             rfm_ret,
             antenna_req_ptr->device, 
             antenna_req_ptr->position,
             antenna_req_ptr->action_imm );

  /* No response required, just return the original request. */
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  return ftm_rsp_data;

} /* ftm_common_radio_set_antenna */

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
  Debug interface to test Common driver

  @details
  This function can be used as a sandbox area to add a test code to verify
  common driver. Generic payload to the test code can be sent as 8 chunks of
  payload where each paylaod is of length 32-bit.

  @param *ftm_req_data
  FTM Request Data to verify common driver

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_common_driver_verification_sandbox
(
  void  *ftm_req_data
)
{
  PACK(struct) ftm_common_driver_ver_sandbox_req_s /* Input parameter structure */
  {
    uint8 cmd;
    uint8 sub_sys;
    uint16 mode;
    uint16 cdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint32 payload_0;
    uint32 payload_1;
    uint32 payload_2;
    uint32 payload_3;
    uint32 payload_4;
    uint32 payload_5;
    uint32 payload_6;
    uint32 payload_7;
    uint32 payload_8;
    uint32 payload_9;
    uint32 payload_10;
    uint32 payload_11;
  } *driver_veri_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */

  uint32 payload_0, payload_1, payload_2, payload_3;
  uint32 payload_4, payload_5, payload_6, payload_7;
  uint32 payload_8, payload_9, payload_10, payload_11;
  int32 ret_val = 1;

  /* Request Packet */
  driver_veri_req_ptr = 
      (struct ftm_common_driver_ver_sandbox_req_s *)ftm_req_data;

  payload_0 = driver_veri_req_ptr->payload_0;
  payload_1 = driver_veri_req_ptr->payload_1;
  payload_2 = driver_veri_req_ptr->payload_2;
  payload_3 = driver_veri_req_ptr->payload_3;
  payload_4 = driver_veri_req_ptr->payload_4;
  payload_5 = driver_veri_req_ptr->payload_5;
  payload_6 = driver_veri_req_ptr->payload_6;
  payload_7 = driver_veri_req_ptr->payload_7;
  payload_8 = driver_veri_req_ptr->payload_8;
  payload_9 = driver_veri_req_ptr->payload_9;
  payload_10 = driver_veri_req_ptr->payload_10;
  payload_11 = driver_veri_req_ptr->payload_11;

  /*--------------------------------------------------------------------------*/
  /* START : Place the sandbox code in this area */

  /* Return code for the code should be stored in ret_val as INT32 */
  {
    /*========================================================================*/














    /*========================================================================*/
  }
  /* END : Place the sandbox code in this area */
  /*--------------------------------------------------------------------------*/

  /* No response required, just return the original request. */
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  /* Send back a BAD response packet for failures */
  if ( ret_val == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_9( FTM_ERROR, "ftm_common_driver_verification_sandbox: Payloads "
                "{ %d, %d, %d, %d, %d, %d, %d, %d, %d } [Failed]",
                payload_0, payload_1, payload_2, payload_3,
                payload_4, payload_5, payload_6, payload_7,
                payload_8 );
	
    FTM_MSG_3( FTM_ERROR, "ftm_common_driver_verification_sandbox: Payloads "
                "{ %d, %d, %d } [Failed]",payload_9, payload_10, payload_11 );
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
    FTM_MSG_9( FTM_HIGH, "ftm_common_driver_verification_sandbox: Payloads "
                "{ %d, %d, %d, %d, %d, %d, %d, %d, %d } [Success]",
                payload_0, payload_1, payload_2, payload_3,
                payload_4, payload_5, payload_6, payload_7,
                payload_8 );
	
    FTM_MSG_3( FTM_HIGH, "ftm_common_driver_verification_sandbox: Payloads "
                "{ %d, %d, %d } [Success]",payload_9, payload_10, payload_11 );
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_common_driver_verification_sandbox */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic API to enter a particular tech in FTM

  @details
  This API is used to enter any tech in FTM mode. The data sent in is parsed and 
  the call flow is directed to the requiested tech and the device info. 

  @param *ftm_req_data
  FTM Request Data to verify common driver

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_common_radio_enter_mode
(
  void  *ftm_req_data
)
{
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  rfm_mode_enum_type rf_mode;/* 1x/HDR/SB-VDO*/
  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;
  boolean ret_val;

  PACK(struct) ftm_common_enter_mode_req_s /* Input parameter structure */
  {
    uint8 cmd;
    uint8 sub_sys;
    uint16  mode;
    uint16  cdma_cmd;
    uint16  req_len;
    uint16  rsp_len;
    uint8   device;
    uint8   rf_mode;
  } *enter_mode_req_ptr;

  enter_mode_req_ptr = (struct ftm_common_enter_mode_req_s *)ftm_req_data;

  rf_mode = (rfm_mode_enum_type)(enter_mode_req_ptr->rf_mode);
  device =  (rfm_device_enum_type)(enter_mode_req_ptr->device);

  switch ( rf_mode )
  {
    #if defined(FEATURE_HDR)||defined(FEATURE_CDMA1X)
    case RFM_1X_MODE:
    case RFM_1XEVDO_MODE:
    case RFM_SVDO_MODE:
      /* Call CDMA specific API to enter mode requested */
      ret_val = ftm_cdma_radio_enter_mode( device, rf_mode);
    break;
    #endif

    #if defined FEATURE_WCDMA
    case RFM_IMT_MODE:
      ret_val = ftm_wcdma_radio_enter_mode(device, rf_mode);
    break;
    #endif

    default:

      FTM_MSG_2( FTM_ERROR, "ftm_common_radio_enter_mode: Unsupported RF "
                           "mode: %d on Device: %d", rf_mode, device );
      ret_val = FALSE;
    break;
  }/* switch ( rf_mode )*/

  /* No response required, just return the original request. */
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  if ( ret_val == FALSE )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_2( FTM_ERROR, "ftm_common_radio_enter_mode:Failed to enter mode %d" 
               " on device %d", rf_mode, device); 
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
    FTM_MSG_2( FTM_LOW, "ftm_common_radio_enter_mode:Entered mode %d" 
               " on device %d", rf_mode, device); 
  }

  return ftm_rsp_data;

}/* ftm_common_radio_enter_mode */

/*! @} */

/*============================================================================*/
/*!
  @name FBRx

  @brief
  Functions used for FBRx debugging
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure calv3 debug level

  @details
  This function configures calv3 to appropriate debug level

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_common_fbrx_iplc_override
(
  void  *ftm_req_data
)
{
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  PACK(struct) ftm_common_fbrx_iplc_override_req_s /* Input parameter str */
  {
    uint8   cmd;
    uint8   sub_sys;
    uint16  mode;
    uint16  cdma_cmd;
    uint16  req_len;
    uint16  rsp_len;
    uint8   device;
    uint8   ilpc_enable;
  } *fbrx_iplc_override_ptr;

  fbrx_iplc_override_ptr = 
       (struct ftm_common_fbrx_iplc_override_req_s *)ftm_req_data;

  rfm_ftm_fbrx_iplc_override( fbrx_iplc_override_ptr->device, 
                              fbrx_iplc_override_ptr->ilpc_enable  );

  FTM_MSG_2( FTM_HIGH, "ftm_common_fbrx_iplc_override: [Done] - Device %d, "
             "override_disable %d",
             fbrx_iplc_override_ptr->device,
             fbrx_iplc_override_ptr->ilpc_enable );

  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  return ftm_rsp_data;

} /* ftm_common_fbrx_iplc_override */


/*! @} */


/*! @} */


#endif /*FEATURE_FACTORY_TESTMODE */


