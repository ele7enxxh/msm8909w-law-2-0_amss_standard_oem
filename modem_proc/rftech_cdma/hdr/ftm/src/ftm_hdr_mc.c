/*!
  @file
  ftm_hdr_mc.c

  @brief
  This module contains the HDR FTM MC command dispatches and their 
  implementations

  @details


  @addtogroup RF_FTM_HDR_MC
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


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/src/ftm_hdr_mc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/22/14   spa     KW warning fixes
09/04/13   APU     Brought over FTM HDR radio Tx control from Dime.
08/27/13   pk      Fix for Rx0 Power level is returned for Rx1 Chain QRCT
08/20/13   zhw/sty Update FTM HDR radio Tx control interface
04/25/13   sty     Fixed compiler warnings raised due to use of -Wshadow option
08/27/12   zhw     Removed featurization FEATURE_HDR_NON_SIGNALING_MSGR_INTF 
                   Removed unused header files 
05/23/12   zhw     Added support for HDR modulator control API from L1 HDR 
                   Refactored hdr_ftm_* functions to ftm_hdr_* 
09/22/12   APU     Added FTM support for setting the individual pilot carrier 
                   power.
05/16/12   hdz     Fixed compiler warning 
04/28/12   hdz     Added RF support for HDR NS HHO cmd using 
05/18/12   kb/hdz  Added RF support for HDR NS HHO cmd using 
                   FEATURE_HDR_NON_SIGNALING_MSGR_INTF
04/24/12   aro     Compiler warning fix
04/24/12   aro     Interface to return randomized payload for DOrB
03/26/12   jfc     Fix tcb/task references for dime
03/05/12   hdz     Saved drc_cover from GUI to a local variable
03/03/12   hdz     Changed DRC_COVER assigned from hdr_fwlink_rev0.drc_cover
                   to hdr_mod_rtc.drc_cover in hard handoff API
03/01/12   hdz     Added RF support to enable new HDR NS FTM feature:
                   FEATURE_HDR_NON_SIGNALING_MSGR_INTF
01/26/12   hy      Enabled hdr ns revb
01/23/12   hdz     Enabled hdr ns hard handoff for rev0 and reva
01/20/12   hdz     Changed interface of rfm_hdr_ftm_set_tx_pwr()
11/30/11   Saul    HDR non-sig. RevA now functional.
11/03/11   Saul    HDR non-sig. Added RevA framework.
10/27/11   Saul    HDR non-sig. Tx now functional for HDR rev0.
09/16/11   Saul    HDR non-sig initial support. Rx functional. Tx not functional
                   as seems dependencies are not yet available.
05/16/11   shb     Initial revision.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "msg.h"
#include "task.h"
#include "ftm_msg.h"
#include "ftm_task.h"
#include "ftm_cdma_dispatch.h"
#include "ftm_common_control.h"
#include "ftm_cdma_data.h"
#include "rf_hdr_mdsp.h"
#include "rfm_hdr_ftm.h"
#include "rfm_cdma_ftm.h"
#include "hdrhit_msg.h"

sys_channel_type ftm_hdr_chan;
/*! FTM command status */
ftm_cmd_status_type test_cmd_status;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wait for FTM command response

  @details
  Used in HDR MC cases where a command is sent to HDR RX/TX tasks and we wait
  for completion signal from HDR task.
 
  @param void
  None
 
  @return
  return different status
*/

ftm_cmd_status_type ftm_hdr_wait_for_resp( void )
{
   ftm_cmd_status_type        cmd_status;
   hdrhit_msg_status_type     response_status;

  /* Wait the FTM command response signal to be set */
  (void) ftm_sig_rex_wait( FTM_CMD_RSP_SIG );

  /* this is a temporary solution, RF should come up with message router solution
     later
  */
  response_status = hdrhitmsg_get_response_status( );
  FTM_MSG_HIGH( "FTM has received response, response_status=%d", 
                 response_status, 0, 0 );

  switch ( response_status )
  {
    case HDRHIT_SUCCESS :
      cmd_status = TEST_SUCCESS;
      break;

    case HDRHIT_INVALID_STATE :
      cmd_status = TEST_INVALID_STATE;
      break;

    case HDRHIT_NO_SUCCESS :
      cmd_status = TEST_NO_SUCCESS;
      break;

    default: 
      cmd_status = TEST_SUCCESS;
      break;
  }

  /* Clear the FTM command response signal */
  (void) rex_clr_sigs( TASK_HANDLE(ftm), FTM_CMD_RSP_SIG );

  return cmd_status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize ftm msg router

  @details
  Initialize ftm msg router and all necessary data structure
 
  @param p_hdrhit_cmd_msg
  pointer to cmd of msg
 
  @param msg_id
  ID of the msg
 
  @return
  None
*/
void ftm_hdr_init_msg
(
  hdrhit_msg_cmd_u *            p_hdrhit_cmd_msg,
    /* pointer to message */
  msgr_umid_type                msg_id
    /* message id */
)
{
   memset(  p_hdrhit_cmd_msg, 0, sizeof( hdrhit_msg_cmd_u ) );
     /* clear the message */
   msgr_init_hdr( &( p_hdrhit_cmd_msg->hdr ), 
                  MSGR_HDR_HIT, 
                  msg_id
                 ); 
    /* Create message header */
}/* ftm_hdr_init_msg */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send ftm msg

  @details
  Send corresponding msg to HDR L1
 
  @param msg
  pointer to the msg
 
  @param len
  Size of msg
 
  @return
  None
*/
void ftm_hdr_send_msg
(
  void *                       msg,
    /* message to be sent */
  uint32                       len
   /* message length */
)
{

  errno_enum_type        e_retval;
    /* error code */

  e_retval = msgr_send( (msgr_hdr_struct_type *) msg, len );

  if ( e_retval != E_SUCCESS )
  {
    FTM_MSG_HIGH( "FTM Send msg to HDR error: 0x%8x", 
               ( ( msgr_hdr_struct_type *) msg)->id, 0, 0 );

  }

}/* ftm_hdr_send_msg */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command for acquiring a pilot

  @details
  Activated HDR SRCH RX/TX tasks and sends command to begin acquisition using
  parameters from param->hdr_acq. Registers and waits for callbacks with HDR 
  to signal completion and success/failure.

  @param *param
  Union of data structures of all FTM HDR MC commands. param->hdr_acq contains
  acquisition specific parameters

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_acquire
(
   ftm_packed_param_type *param
)
{
  hdrhit_msg_cmd_u             hdrhit_cmd_msg;

/*-----------------------------------------------------------------------*/

  /* store hdr band and channel */
  ftm_hdr_chan.band = ( sys_band_class_e_type )(param->hdr_acq.band_class);
  ftm_hdr_chan.chan_num = ( sys_channel_num_type ) param->hdr_acq.cdma_freq;

  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_PIL_ACQ );

  hdrhit_cmd_msg.pil_acq.band_class = 
    ( sys_band_class_e_type )(param->hdr_acq.band_class);
  hdrhit_cmd_msg.pil_acq.chan_num = param->hdr_acq.cdma_freq;
  hdrhit_cmd_msg.pil_acq.diversity_mode = 
    ( hdrhit_hdr_ftm_diversity_mode_type )param->hdr_acq.diversity_mode;

  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_pil_acq_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_PIL_ACQ MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

  FTM_MSG(FTM_HIGH, "FTM Wait for HDR SRCH ACQ rpt");

} /* ftm_hdr_acquire */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command for system synchronization

  @details
  Sends HDR MAC activation command if we have previously acquired successfully
  in forward link. Successful acquisition command must be completed before
  calling this function. If sync is successful, HDR forward link will be in
  HDR_FTM_SYNC_STATE

  @param *param
  Union of data structures of all FTM HDR MC commands. No params are needed
  from this struct for synchronization command

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_sync
(
   ftm_packed_param_type *param
)
{

  hdrhit_msg_cmd_u             hdrhit_cmd_msg;

/*-----------------------------------------------------------------------*/

  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_SYNC );
  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_sync_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_SYNC MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_sync */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command for forward link demod

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_demod_ftc
(
   ftm_packed_param_type *param
)
{
    hdrhit_msg_cmd_u             hdrhit_cmd_msg;

/*-----------------------------------------------------------------------*/

  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_DEMOD_FTC );

  hdrhit_cmd_msg.demod_ftc.hdr_chan = ftm_hdr_chan;
  hdrhit_cmd_msg.demod_ftc.arq_enable = param->hdr_demod_ftc.arq_enable;
  hdrhit_cmd_msg.demod_ftc.drc_lockperiod = param->hdr_demod_ftc.drc_lockperiod;
  hdrhit_cmd_msg.demod_ftc.drc_locklength = param->hdr_demod_ftc.drc_locklength;
  hdrhit_cmd_msg.demod_ftc.mac_index = param->hdr_demod_ftc.mac_index ;
  hdrhit_cmd_msg.demod_ftc.rab_length = param->hdr_demod_ftc.rab_length;
  hdrhit_cmd_msg.demod_ftc.rab_offset = param->hdr_demod_ftc.rab_offset;

  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_demod_ftc_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_DEMOD_FTC MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_demod_ftc */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command for sending access channel

  @details
  HDR forward link must be in SYNC state before attempting to send access
  channel. If not, command will fail. Command will also fail if rev link is
  already in TRAFFIC state. Sets HDR foward link parameters and executes HDR
  commands to send access channel

  @param *param
  Union of data structures of all FTM HDR MC commands. param->hdr_mod_acc
  contains all the parameters needed

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_mod_acc
(
   ftm_packed_param_type *param
)
{
  hdrhit_msg_cmd_u             hdrhit_cmd_msg;
/*----------------------------------------------------------------------------*/


  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_MOD_ACC );

  hdrhit_cmd_msg.mod_acc.open_loop_adjust = param->hdr_mod_acc.open_loop_adjust;
  hdrhit_cmd_msg.mod_acc.access_cycle_duration 
    = param->hdr_mod_acc.access_cycle_duration;
  hdrhit_cmd_msg.mod_acc.ac_txpattern = param->hdr_mod_acc.ac_txpattern;
  hdrhit_cmd_msg.mod_acc.power_step = param->hdr_mod_acc.power_step;
  hdrhit_cmd_msg.mod_acc.probe_initial_adjust
    = param->hdr_mod_acc.probe_initial_adjust;
  hdrhit_cmd_msg.mod_acc.probe_num_step = param->hdr_mod_acc.probe_num_step;
  hdrhit_cmd_msg.mod_acc.preamble_len = param->hdr_mod_acc.preamble_length;
  hdrhit_cmd_msg.mod_acc.aclc_mask_i_hi = param->hdr_mod_acc.aclc_mask_i_hi;
  hdrhit_cmd_msg.mod_acc.aclc_mask_i_lo = param->hdr_mod_acc.aclc_mask_i_lo;
  hdrhit_cmd_msg.mod_acc.aclc_mask_q_hi = param->hdr_mod_acc.aclc_mask_q_hi;
  hdrhit_cmd_msg.mod_acc.aclc_mask_q_lo = param->hdr_mod_acc.aclc_mask_q_lo;

  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_mod_acc_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_MOD_ACC MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_mod_acc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command for sending reverse traffic channel

  @details
  HDR forward link must be in SYNC state before attempting to send reverse
  traffic channel. If still in ENTER or ACQ state, command will fail. 
  Sets HDR foward link parameters and executes HDR commands to send reverse
  traffic channel. Calls RFM APIs to set nominal TX power

  @param *param
  Union of data structures of all FTM HDR MC commands. param->hdr_mod_rtc
  contains all the parameters needed

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_mod_rtc
(
   ftm_packed_param_type *param
)
{
    hdrhit_msg_cmd_u             hdrhit_cmd_msg;
  /*----------------------------------------------------------------------------*/

    ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_MOD_RTC );

    hdrhit_cmd_msg.mod_rtc.hdr_chan = ftm_hdr_chan;
    hdrhit_cmd_msg.mod_rtc.ack_gain = param->hdr_mod_rtc.ack_gain;
    hdrhit_cmd_msg.mod_rtc.dataoffset_nom = param->hdr_mod_rtc.dataoffset_nom;
    hdrhit_cmd_msg.mod_rtc.dataoffset_9k6 = param->hdr_mod_rtc.dataoffset_9k6;
    hdrhit_cmd_msg.mod_rtc.dataoffset_19k2 = param->hdr_mod_rtc.dataoffset_19k2;
    hdrhit_cmd_msg.mod_rtc.dataoffset_38k4 = param->hdr_mod_rtc.dataoffset_38k4;
    hdrhit_cmd_msg.mod_rtc.dataoffset_76k8 = param->hdr_mod_rtc.dataoffset_76k8;
    hdrhit_cmd_msg.mod_rtc.dataoffset_153k6 
      = param->hdr_mod_rtc.dataoffset_153k6;
    hdrhit_cmd_msg.mod_rtc.drc_value = param->hdr_mod_rtc.drc_value;
    hdrhit_cmd_msg.mod_rtc.drc_cover = param->hdr_mod_rtc.drc_cover;
    hdrhit_cmd_msg.mod_rtc.drc_length = param->hdr_mod_rtc.drc_length;
    hdrhit_cmd_msg.mod_rtc.drc_gain = param->hdr_mod_rtc.drc_gain;
    hdrhit_cmd_msg.mod_rtc.drc_gating = param->hdr_mod_rtc.drc_gating;
    hdrhit_cmd_msg.mod_rtc.frame_offset = param->hdr_mod_rtc.frame_offset;
    hdrhit_cmd_msg.mod_rtc.rpc_enable = param->hdr_mod_rtc.rpc_enable;
    hdrhit_cmd_msg.mod_rtc.rpc_step = param->hdr_mod_rtc.rpc_step;
    hdrhit_cmd_msg.mod_rtc.rri = param->hdr_mod_rtc.rri;
    hdrhit_cmd_msg.mod_rtc.rtclc_mask_i_hi = param->hdr_mod_rtc.rtclc_mask_i_hi;
    hdrhit_cmd_msg.mod_rtc.rtclc_mask_i_lo = param->hdr_mod_rtc.rtclc_mask_i_lo;
    hdrhit_cmd_msg.mod_rtc.rtclc_mask_q_hi = param->hdr_mod_rtc.rtclc_mask_q_hi;
    hdrhit_cmd_msg.mod_rtc.rtclc_mask_q_lo = param->hdr_mod_rtc.rtclc_mask_q_lo;
    hdrhit_cmd_msg.mod_rtc.rtc_txduty_cycle 
      = param->hdr_mod_rtc.rtc_txduty_cycle;
    hdrhit_cmd_msg.mod_rtc.rtc_txpattern = param->hdr_mod_rtc.rtc_txpattern;
    hdrhit_cmd_msg.mod_rtc.hdr_ack_mode = param->hdr_mod_rtc.hdr_ack_mode;

    ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_mod_rtc_cmd_msg_t ));

    FTM_MSG_HIGH( "FTM Send HIT_MOD_RTC MSG to HDR: 0x%8x", 
                   hdrhit_cmd_msg.hdr.id, 0, 0 );

}/* ftm_hdr_mod_rtc */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command that deassigns all channels

  @details
  Depending on current state of HDR MAC, calls APIs to deactivate MAC and PHY 
  protocols. Will not do any thing is forward and reverse links are already in
  ENTER state. Will clear ftm_tcb response signal to ensure FTM task is not
  waiting for HDR response

  @param *param
  Union of data structures of all FTM HDR MC commands. No params used from here
  for deassign command

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/
void ftm_hdr_deassign
(
   ftm_packed_param_type *param
)
{

  hdrhit_msg_cmd_u             hdrhit_cmd_msg;

/*-----------------------------------------------------------------------*/
  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_DEASSIGN );
  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_deassign_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_DEASSIGN MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_deassign */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command that puts the searcher in idle mode.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_idle_state
(
   ftm_packed_param_type *param
)
{

 hdrhit_msg_cmd_u             hdrhit_cmd_msg;

/*-----------------------------------------------------------------------*/
  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_IDLE_STATE );
  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_idle_state_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_IDLE_STATE MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_idle_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command that demodulates the forward link
  without putting the reverse link on for Rev0.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_demod_fwlink_rev0
(
   ftm_packed_param_type *param
)
{
  hdrhit_msg_cmd_u             hdrhit_cmd_msg;

/*-----------------------------------------------------------------------*/
  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_DEMOD_FW_LINK_REV0 );

  hdrhit_cmd_msg.demod_fw_link_rev0.hdr_chan = ftm_hdr_chan;
  hdrhit_cmd_msg.demod_fw_link_rev0.drc_value 
    = param->hdr_fwd_link_rel0.drc_value;
  hdrhit_cmd_msg.demod_fw_link_rev0.drc_cover 
    = param->hdr_fwd_link_rel0.drc_cover;
  hdrhit_cmd_msg.demod_fw_link_rev0.arq_enable 
    = param->hdr_fwd_link_rel0.arq_enable;
  hdrhit_cmd_msg.demod_fw_link_rev0.drc_lockperiod 
    = param->hdr_fwd_link_rel0.drc_lockperiod;
  hdrhit_cmd_msg.demod_fw_link_rev0.drc_locklength 
    = param->hdr_fwd_link_rel0.drc_locklength;
  hdrhit_cmd_msg.demod_fw_link_rev0.mac_index 
    = param->hdr_fwd_link_rel0.mac_index;
  hdrhit_cmd_msg.demod_fw_link_rev0.rab_length 
    = param->hdr_fwd_link_rel0.rab_length;
  hdrhit_cmd_msg.demod_fw_link_rev0.rab_offset 
    = param->hdr_fwd_link_rel0.rab_offset;

  ftm_hdr_send_msg( &hdrhit_cmd_msg,
                    sizeof( hdrhit_demod_fw_link_rev0_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_DEMOD_FW_LINK_REV0 MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_demod_fwlink_rev0 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command that enables or disable the 
  demodulation of multi user and short user packet by AT.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_rela_mac_config
(
   ftm_packed_param_type *param
)
{

  hdrhit_msg_cmd_u             hdrhit_cmd_msg;

  /*-----------------------------------------------------------------------*/
  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_RELA_MAC_CONFIG );

  hdrhit_cmd_msg.rela_mac_config.mup_enabled = param->hdr_rela_mac_config.mup;
  hdrhit_cmd_msg.rela_mac_config.sht_pkt_enabled 
    = param->hdr_rela_mac_config.sht_pkt;

  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_rela_mac_config_cmd_msg_t ));
  FTM_MSG_HIGH( "FTM Send HIT_RELA_MAC_CONFIG MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

}/* ftm_hdr_rela_mac_config */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command that demodulates the forward link
  without putting the reverse link on for RevA.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_demod_fwlink_reva
(
   ftm_packed_param_type *param
)
{

  hdrhit_msg_cmd_u             hdrhit_cmd_msg;

  /*-----------------------------------------------------------------------*/
  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_DEMOD_FW_LINK_REVA );

  hdrhit_cmd_msg.demod_fw_link_reva.hdr_chan = ftm_hdr_chan;
  hdrhit_cmd_msg.demod_fw_link_reva.drc_value 
    = param->hdr_fwd_link_rela.drc_value;
  hdrhit_cmd_msg.demod_fw_link_reva.drc_cover 
    = param->hdr_fwd_link_rela.drc_cover;
  hdrhit_cmd_msg.demod_fw_link_reva.arq_enable 
    = param->hdr_fwd_link_rela.arq_enable;
  hdrhit_cmd_msg.demod_fw_link_reva.drc_lockperiod 
    = param->hdr_fwd_link_rela.drc_lockperiod;
  hdrhit_cmd_msg.demod_fw_link_reva.drc_locklength 
    = param->hdr_fwd_link_rela.drc_locklength;
  hdrhit_cmd_msg.demod_fw_link_reva.mac_index 
    = param->hdr_fwd_link_rela.mac_index;
  hdrhit_cmd_msg.demod_fw_link_reva.rab_length 
    = param->hdr_fwd_link_rela.rab_length;
  hdrhit_cmd_msg.demod_fw_link_reva.rab_offset 
    = param->hdr_fwd_link_rela.rab_offset;
  hdrhit_cmd_msg.demod_fw_link_reva.dsc_value 
    = param->hdr_fwd_link_rela.dsc_value;

  ftm_hdr_send_msg( &hdrhit_cmd_msg, 
                    sizeof( hdrhit_demod_fw_link_reva_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_DEMOD_FW_LINK_REV0 MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );

}/* ftm_hdr_demod_fwlink_reva */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command that modulates the reverse link
  in rev A.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_mod_rtc_reva
(
   ftm_packed_param_type *param
)
{
    hdrhit_msg_cmd_u             hdrhit_cmd_msg;
  /*----------------------------------------------------------------------------*/

    ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_MOD_RTC_REVA );

    hdrhit_cmd_msg.mod_rtc_reva.hdr_chan = ftm_hdr_chan;
    hdrhit_cmd_msg.mod_rtc_reva.ack_gain = param->hdr_mod_rtc_rela.ack_gain;
    hdrhit_cmd_msg.mod_rtc_reva.drc_value = param->hdr_mod_rtc_rela.drc_value;
    hdrhit_cmd_msg.mod_rtc_reva.drc_cover = param->hdr_mod_rtc_rela.drc_cover;
    hdrhit_cmd_msg.mod_rtc_reva.drc_length = param->hdr_mod_rtc_rela.drc_length;
    hdrhit_cmd_msg.mod_rtc_reva.drc_gain = param->hdr_mod_rtc_rela.drc_gain;
    hdrhit_cmd_msg.mod_rtc_reva.drc_gating = param->hdr_mod_rtc_rela.drc_gating;
    hdrhit_cmd_msg.mod_rtc_reva.dsc_value = param->hdr_mod_rtc_rela.dsc_value;
    hdrhit_cmd_msg.mod_rtc_reva.dsc_length = param->hdr_mod_rtc_rela.dsc_length;
    hdrhit_cmd_msg.mod_rtc_reva.dsc_gain = param->hdr_mod_rtc_rela.dsc_gain;
    hdrhit_cmd_msg.mod_rtc_reva.frame_offset 
      = param->hdr_mod_rtc_rela.frame_offset;
    hdrhit_cmd_msg.mod_rtc_reva.rpc_enable = param->hdr_mod_rtc_rela.rpc_enable;
    hdrhit_cmd_msg.mod_rtc_reva.rpc_step = param->hdr_mod_rtc_rela.rpc_step;
    hdrhit_cmd_msg.mod_rtc_reva.rri = param->hdr_mod_rtc_rela.rri;
    hdrhit_cmd_msg.mod_rtc_reva.rtclc_mask_i_hi 
      = param->hdr_mod_rtc_rela.rtclc_mask_i_hi;
    hdrhit_cmd_msg.mod_rtc_reva.rtclc_mask_i_lo 
      = param->hdr_mod_rtc_rela.rtclc_mask_i_lo;
    hdrhit_cmd_msg.mod_rtc_reva.rtclc_mask_q_hi 
      = param->hdr_mod_rtc_rela.rtclc_mask_q_hi;
    hdrhit_cmd_msg.mod_rtc_reva.rtclc_mask_q_lo 
      = param->hdr_mod_rtc_rela.rtclc_mask_q_lo;
    hdrhit_cmd_msg.mod_rtc_reva.rtc_txpattern 
      = param->hdr_mod_rtc_rela.rtc_txpattern;

    hdrhit_cmd_msg.mod_rtc_reva.hicap_lolatency 
      = param->hdr_mod_rtc_rela.hicap_lolatency;
    hdrhit_cmd_msg.mod_rtc_reva.gain_transition_point 
      = param->hdr_mod_rtc_rela.gain_transition_point;
    hdrhit_cmd_msg.mod_rtc_reva.t2p_pre_transition 
      = param->hdr_mod_rtc_rela.t2p_pre_transition;
    hdrhit_cmd_msg.mod_rtc_reva.t2p_post_transition 
      = param->hdr_mod_rtc_rela.t2p_post_transition;
    hdrhit_cmd_msg.mod_rtc_reva.arq_mode 
      = param->hdr_mod_rtc_rela.arq_mode;
    hdrhit_cmd_msg.mod_rtc_reva.auxpil_min_payload 
      = param->hdr_mod_rtc_rela.auxpil_min_payload;
    hdrhit_cmd_msg.mod_rtc_reva.auxpil_gain 
      = param->hdr_mod_rtc_rela.auxpil_gain;
    hdrhit_cmd_msg.mod_rtc_reva.rri_gain_pre_transition 
      = param->hdr_mod_rtc_rela.rri_gain_pre_transition;
    hdrhit_cmd_msg.mod_rtc_reva.rri_gain_post_transition 
      = param->hdr_mod_rtc_rela.rri_gain_post_transition;

    hdrhit_cmd_msg.mod_rtc_reva.hdr_ack_mode 
      = param->hdr_mod_rtc_rela.hdr_ack_mode;


    ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_mod_rtc_reva_cmd_msg_t ));

    FTM_MSG_HIGH( "FTM Send HIT_MOD_RTC_REA MSG to HDR: 0x%8x", 
                   hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_mod_rtc_reva */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command for sending the access channel 
  for RevA.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_mod_acc_reva
(
   ftm_packed_param_type *param
)
{

    hdrhit_msg_cmd_u             hdrhit_cmd_msg;
  /*----------------------------------------------------------------------------*/


    ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_MOD_ACC_REVA );

    hdrhit_cmd_msg.mod_acc_reva.open_loop_adjust 
      = param->hdr_mod_acc_rela.open_loop_adjust;
    hdrhit_cmd_msg.mod_acc_reva.access_cycle_duration 
      = param->hdr_mod_acc_rela.access_cycle_duration;
    hdrhit_cmd_msg.mod_acc_reva.ac_txpattern 
      = param->hdr_mod_acc_rela.ac_txpattern;
    hdrhit_cmd_msg.mod_acc_reva.power_step 
      = param->hdr_mod_acc_rela.power_step;
    hdrhit_cmd_msg.mod_acc_reva.probe_initial_adjust 
      = param->hdr_mod_acc_rela.probe_initial_adjust;
    hdrhit_cmd_msg.mod_acc_reva.probe_num_step 
      = param->hdr_mod_acc_rela.probe_num_step;
    hdrhit_cmd_msg.mod_acc_reva.preamble_len 
      = param->hdr_mod_acc_rela.preamble_length;
    hdrhit_cmd_msg.mod_acc_reva.aclc_mask_i_hi 
      = param->hdr_mod_acc_rela.aclc_mask_i_hi;
    hdrhit_cmd_msg.mod_acc_reva.aclc_mask_i_lo 
      = param->hdr_mod_acc_rela.aclc_mask_i_lo;
    hdrhit_cmd_msg.mod_acc_reva.aclc_mask_q_hi 
      = param->hdr_mod_acc_rela.aclc_mask_q_hi;
    hdrhit_cmd_msg.mod_acc_reva.aclc_mask_q_lo 
      = param->hdr_mod_acc_rela.aclc_mask_q_lo;

    hdrhit_cmd_msg.mod_acc_reva.enhanced_access_enable 
      = param->hdr_mod_acc_rela.enhanced_access_enable;
    hdrhit_cmd_msg.mod_acc_reva.preamble_length_slots 
      = param->hdr_mod_acc_rela.preamble_length_slots;
    hdrhit_cmd_msg.mod_acc_reva.access_offset 
      = param->hdr_mod_acc_rela.access_offset;
    hdrhit_cmd_msg.mod_acc_reva.sector_access_rate_max 
      = param->hdr_mod_acc_rela.sector_access_rate_max;
    hdrhit_cmd_msg.mod_acc_reva.terminal_access_rate_max 
      = param->hdr_mod_acc_rela.terminal_access_rate_max;
    ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_mod_acc_reva_cmd_msg_t ));

    FTM_MSG_HIGH( "FTM Send HIT_MOD_ACC_REVA MSG to HDR: 0x%8x", 
                   hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_mod_acc_reva */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command that modulates the reverse link
  in rev B.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_mod_rtc_revb
(
   ftm_packed_param_type *param
)
{
    hdrhit_msg_cmd_u             hdrhit_cmd_msg;
  /*----------------------------------------------------------------------------*/

    ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_MOD_RTC_REVB );

    hdrhit_cmd_msg.mod_rtc_revb.hdr_chan = ftm_hdr_chan;
    hdrhit_cmd_msg.mod_rtc_revb.ack_gain = param->hdr_mod_rtc_relb.ack_gain;
    hdrhit_cmd_msg.mod_rtc_revb.dsc_gain = param->hdr_mod_rtc_relb.dsc_gain;
    hdrhit_cmd_msg.mod_rtc_revb.drc_value = param->hdr_mod_rtc_relb.drc_value;
    hdrhit_cmd_msg.mod_rtc_revb.drc_gain = param->hdr_mod_rtc_relb.drc_gain;
    hdrhit_cmd_msg.mod_rtc_revb.rpc_enable = param->hdr_mod_rtc_relb.rpc_enable;
    hdrhit_cmd_msg.mod_rtc_revb.rpc_step = param->hdr_mod_rtc_relb.rpc_step;
    hdrhit_cmd_msg.mod_rtc_revb.hicap_lolatency 
      = param->hdr_mod_rtc_relb.hicap_lolatency;
    hdrhit_cmd_msg.mod_rtc_revb.auxpil_gain 
      = param->hdr_mod_rtc_relb.auxpil_gain;
    hdrhit_cmd_msg.mod_rtc_revb.rri_c0= param->hdr_mod_rtc_relb.rri_c0;
    hdrhit_cmd_msg.mod_rtc_revb.rri_c1 = param->hdr_mod_rtc_relb.rri_c1;
    hdrhit_cmd_msg.mod_rtc_revb.rri_c2 = param->hdr_mod_rtc_relb.rri_c2;
    hdrhit_cmd_msg.mod_rtc_revb.chan_c1 = param->hdr_mod_rtc_relb.chan_c1;
    hdrhit_cmd_msg.mod_rtc_revb.chan_c2 = param->hdr_mod_rtc_relb.chan_c2;
    hdrhit_cmd_msg.mod_rtc_revb.power_offset_c0 
      = param->hdr_mod_rtc_relb.power_offset_c0;
    hdrhit_cmd_msg.mod_rtc_revb.power_offset_c1
      = param->hdr_mod_rtc_relb.power_offset_c1;
    hdrhit_cmd_msg.mod_rtc_revb.power_offset_c2
      = param->hdr_mod_rtc_relb.power_offset_c2;
    hdrhit_cmd_msg.mod_rtc_revb.single_fwd_link_mode
      = param->hdr_mod_rtc_relb.single_fwd_link_mode;
    hdrhit_cmd_msg.mod_rtc_revb.num_carriers 
      = param->hdr_mod_rtc_relb.num_carriers;

    ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_mod_rtc_revb_cmd_msg_t ));

    FTM_MSG_HIGH( "FTM Send HIT_MOD_RTC_REB MSG to HDR: 0x%8x", 
                   hdrhit_cmd_msg.hdr.id, 0, 0 );

} /* ftm_hdr_mod_rtc_revb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command that configures the reverse link
  in rev B.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

void ftm_hdr_config_rtc_revb
(
   ftm_packed_param_type *param
)
{

    hdrhit_msg_cmd_u             hdrhit_cmd_msg;
  /*----------------------------------------------------------------------------*/

  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_CONFIG_RTC_REVB );
  hdrhit_cmd_msg.config_rtc_revb.drc_cover 
    = param->hdr_config_rtc_relb.drc_cover;
  hdrhit_cmd_msg.config_rtc_revb.drc_length 
    = param->hdr_config_rtc_relb.drc_length;
  hdrhit_cmd_msg.config_rtc_revb.drc_gating 
    = param->hdr_config_rtc_relb.drc_gating;
  hdrhit_cmd_msg.config_rtc_revb.dsc_value 
    = param->hdr_config_rtc_relb.dsc_value;
  hdrhit_cmd_msg.config_rtc_revb.dsc_length 
    = param->hdr_config_rtc_relb.dsc_length;

  hdrhit_cmd_msg.config_rtc_revb.frame_offset 
    = param->hdr_config_rtc_relb.frame_offset;
  hdrhit_cmd_msg.config_rtc_revb.rtc_txpattern 
    = param->hdr_config_rtc_relb.rtc_txpattern ;
  hdrhit_cmd_msg.config_rtc_revb.rtclc_mask_i_hi 
    = param->hdr_config_rtc_relb.rtclc_mask_i_hi;
  hdrhit_cmd_msg.config_rtc_revb.rtclc_mask_i_lo 
    = param->hdr_config_rtc_relb.rtclc_mask_i_lo ;
  hdrhit_cmd_msg.config_rtc_revb.rtclc_mask_q_hi 
    = param->hdr_config_rtc_relb.rtclc_mask_q_hi;
  hdrhit_cmd_msg.config_rtc_revb.rtclc_mask_q_lo 
    = param->hdr_config_rtc_relb.rtclc_mask_q_lo;
  hdrhit_cmd_msg.config_rtc_revb.arq_mode 
    = param->hdr_config_rtc_relb.arq_mode;
  hdrhit_cmd_msg.config_rtc_revb.hdr_ack_mode 
    = param->hdr_config_rtc_relb.hdr_ack_mode;
  hdrhit_cmd_msg.config_rtc_revb.auxpil_min_payload 
    = param->hdr_config_rtc_relb.auxpil_min_payload;
  hdrhit_cmd_msg.config_rtc_revb.rri 
    = param->hdr_config_rtc_relb.rri;
  hdrhit_cmd_msg.config_rtc_revb.hicap_lolatency 
    = param->hdr_config_rtc_relb.hicap_lolatency;
  hdrhit_cmd_msg.config_rtc_revb.gain_transition_point 
    = param->hdr_config_rtc_relb.gain_transition_point;
  hdrhit_cmd_msg.config_rtc_revb.t2p_pre_transition 
    = param->hdr_config_rtc_relb.t2p_pre_transition;
  hdrhit_cmd_msg.config_rtc_revb.t2p_post_transition 
    = param->hdr_config_rtc_relb.t2p_post_transition;
  hdrhit_cmd_msg.config_rtc_revb.rri_gain_pre_transition 
    = param->hdr_config_rtc_relb.rri_gain_pre_transition;
  hdrhit_cmd_msg.config_rtc_revb.rri_gain_post_transition 
    = param->hdr_config_rtc_relb.rri_gain_post_transition;

  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_config_rtc_revb_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_CONFIG_RTC_REB MSG to HDR: 0x%8x", 
                 hdrhit_cmd_msg.hdr.id, 0, 0 );
  
} /* ftm_hdr_config_rtc_revb */

/*!
  @brief
  Set HDR reverse link pilot power in non-signaling test mode

  @details
  Still to be implemented - currently empty

  Allows PC control over the HDR Tx AGC by overriding the Tx
  open loop and closed loop inputs to the Tx AGC, and setting
  the desired pilot level to the value specified in the diagnostic
  command.

  The override can also be disabled, to return the Tx AGC to
  normal open loop + closed loop power control.

  The input to this command is a boolean indicating whether
  override is enabled, and the desired override power level in
  1/10th dBm.

  This command will only be honored if the HDR non-signaling
  test mode is currently in reverse traffic.  If the HDR 
  system is not currently transmitting, the request is ignored.

  @param param
  A pointer to the FTM parameter union where the r_tx_power member
  is expected to contain the HDR power override parameters.

  @return
  ftm_cmd_status_type indicating the success or failure of the
  command

  @retval TEST_SUCCESS
  The override control was successfully applied

  @retval TEST_INVALID_STATE
  The Tx power was attempted to be overridden while HDR non-
  signaling was not in the correct state.  No change occurred.
*/
ftm_cmd_status_type
ftm_hdr_set_tx_pwr
(
  ftm_packed_param_type *param
)
{
  ftm_cmd_status_type cmd_status = TEST_NO_SUCCESS;
  boolean rf_status = TRUE;

  FTM_MSG_HIGH("FTM set tx power: must be in rev traffic state",0,0,0 );

  /* Call RFM Function to set CL Tx power to zero */
  rf_status &= rfm_hdr_ftm_set_tx_pwr(  RFM_DEVICE_0, RFM_CDMA_CARRIER_0, 
                                        RFM_CDMA_TX_PWR_CLOSED_LOOP, 0
                                      );

  /* Call RFM Function to set OL Tx power */
  rf_status &= rfm_hdr_ftm_set_tx_pwr(  RFM_DEVICE_0, RFM_CDMA_CARRIER_0, 
                                        RFM_CDMA_TX_PWR_OPEN_LOOP, 
                                        param->r_tx_power.tx_power_10th_dbm
                                      );

 (rf_status == TRUE) ?  (cmd_status = TEST_SUCCESS) 
   : (cmd_status = TEST_NO_SUCCESS);
  
  return cmd_status;

} /* ftm_hdr_set_tx_pwr */

/*! 
  @brief
  Set HDR reverse link pilot power in non-signaling test mode for
  a given carrier

  @details
  This command allows to set HDR reverse link pilot power in
  non-signaling test mode for a given carrier.
  Allows PC control over the HDR Tx AGC by overriding the Tx
  open loop and closed loop inputs to the Tx AGC, and setting
  the desired pilot level to the value specified in the diagnostic
  command.

  The override can also be disabled, to return the Tx AGC to
  normal open loop + closed loop power control.

  The input to this command is a boolean indicating whether
  override is enabled, and the desired override power level in
  1/10th dBm.

  This command will only be honored if the HDR non-signaling
  test mode is currently in reverse traffic.  If the HDR 
  system is not currently transmitting, the request is ignored.

  @param param
  A pointer to the FTM parameter union where the r_tx_power member
  is expected to contain the HDR power override parameters.

  @return
  ftm_cmd_status_type indicating the success or failure of the
  command

  @retval TEST_SUCCESS
  The override control was successfully applied

  @retval TEST_INVALID_STATE
  The Tx power was attempted to be overridden while HDR non-
  signaling was not in the correct state.  No change occurred.
*/
ftm_cmd_status_type
hdr_ftm_set_carrier_tx_pwr
(
  ftm_packed_param_type *param
)
{
  ftm_cmd_status_type cmd_status = TEST_NO_SUCCESS;
  boolean status = TRUE;
  rfm_device_enum_type device = RFM_INVALID_DEVICE ;
  uint8 carrier_index = RFM_CDMA_CARRIER_NUM ; 
  int16 tx_power_dBm10 = 0x7FFF ;

  FTM_MSG_5 ( FTM_LOW , "hdr_ftm_set_tx_pwr(): "
              "Dev: 0x%X , CI: 0x%X , OverRide: 0x%X , Pwr: 0x%X (%d) dBm10" ,
              param->hdr_tx_carrier_power.rfm_device ,
              param->hdr_tx_carrier_power.carrier_index ,
              param->hdr_tx_carrier_power.override_enable ,
              param->hdr_tx_carrier_power.tx_power_10th_dbm ,
              param->hdr_tx_carrier_power.tx_power_10th_dbm
              ) ;

  if ( param->hdr_tx_carrier_power.rfm_device >= RFM_MAX_DEVICES )
  {
    status = FALSE ;
    FTM_MSG_1 ( FTM_LOW , "hdr_ftm_set_tx_pwr(): "
                        "Invalid Device -> %d " ,
                        param->hdr_tx_carrier_power.rfm_device ) ;
  }
  else
  {
    device = 
      (rfm_device_enum_type) param->hdr_tx_carrier_power.rfm_device ;
  }

  if ( param->hdr_tx_carrier_power.carrier_index > RFM_CDMA_CARRIER_NUM )
  {
    status = FALSE ;
    FTM_MSG_1 ( FTM_LOW , "hdr_ftm_set_tx_pwr(): "
                        "Invalid Carrier Index -> %d " ,
                        param->hdr_tx_carrier_power.carrier_index ) ;
  }
  else
  {
    carrier_index = param->hdr_tx_carrier_power.carrier_index ;
  }

  if ( ( param->hdr_tx_carrier_power.tx_power_10th_dbm > 351 ) ||
       ( param->hdr_tx_carrier_power.tx_power_10th_dbm < -599 ) )
  {
    status = FALSE ;
    FTM_MSG_1 ( FTM_LOW , "hdr_ftm_set_tx_pwr(): "
                        "Power out of range -> %d " ,
                        param->hdr_tx_carrier_power.tx_power_10th_dbm ) ;
  }
  else
  {
    tx_power_dBm10 = param->hdr_tx_carrier_power.tx_power_10th_dbm  ;
  }
  
  /* Send the command to RFM if validation went well */
  if ( status != FALSE )
  {
    uint8 carrier_count = carrier_index , max_carriers = carrier_index + 1 ;
    if (RFM_CDMA_CARRIER_CUMULATIVE == carrier_index  )
    {
      carrier_count = 0 ;
      max_carriers = RFM_CDMA_CARRIER_CUMULATIVE ;
    }

    for ( ; carrier_count < max_carriers ; carrier_count++ )
    {
      FTM_MSG_2 ( FTM_LOW , 
                  "Trying to set the power for CI: %d to %d dBm10" ,
                   carrier_count , tx_power_dBm10 ) ;
    
      /* Call RFM Function to set OL Tx power */
      status &= rfm_hdr_ftm_set_tx_pwr (  device , carrier_count , 
                                         RFM_CDMA_TX_PWR_OPEN_LOOP , 
                                         tx_power_dBm10 ) ;
    }
  }

  (status == TRUE) ? 
    (cmd_status = TEST_SUCCESS) : (cmd_status = TEST_NO_SUCCESS) ;
  return cmd_status;
} /* hdr_ftm_set_carrier_tx_pwr  */

/*! 
  @brief
  Perform a hard hardoff in non-signaling test mode

  @details
  Still to be implemented - currently empty
  Allows PC control to cause HDR to do a hard hand off

  The input to this command is the HDR chan to handoff to

  This command will only be honored if the HDR non-signaling
  test mode is currently in reverse traffic.  If the HDR 
  system is not currently transmitting, the request is ignored.

  @param param
  A pointer to the FTM parameter union where the HDR chan member
  is expected to contain the HDR hard hadoff chan.

  @return
  ftm_cmd_status_type indicating the success or failure of the
  command

  @retval TEST_SUCCESS
  Using QRCT establisd a NS call and perform a HHO.  Note the
  AGC value.  It should be about -104 until the generator chan
  is changed to the HHO chan.  Once the generator is placed on the
  HHO chan the AGC will once again track.

  @retval TEST_INVALID_STATE
  Using QRCT establisd a NS call and perform a HHO.  Note the
  AGC value.  It should be about -104 until the generator chan
  is changed to the HHO chan.  Once the generator is placed on the
  HHO chan the AGC does not track.
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  
*/

void
ftm_hdr_hard_handoff
(
  ftm_packed_param_type *param
)
{
    hdrhit_msg_cmd_u             hdrhit_cmd_msg;
    ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_HARD_HANDOFF );

    hdrhit_cmd_msg.hard_handoff.target_hdr_chan.band = param->hdr_hho.band_class;
    hdrhit_cmd_msg.hard_handoff.target_hdr_chan.chan_num = param->hdr_hho.cdma_freq;

    ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_hard_handoff_cmd_msg_t ));

    FTM_MSG_HIGH( "FTM Send HIT_HARD_HANDOFF MSG to HDR: 0x%8x", 
                   hdrhit_cmd_msg.hdr.id, 0, 0 );
} /* ftm_hdr_hard_handoff */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure to reset HDR forward link stats
*/

uint8 ftm_hdr_reset_fwd_lnk_stats(void)
{
  uint8 status = 0;

  hdrhit_msg_cmd_u             hdrhit_cmd_msg;
  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_RESET_FWD_LNK_STATS );
  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_reset_fwd_lnk_stats_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_RESET_FWD_LNK_STATS to HDR: 0x%8x", 
                   hdrhit_cmd_msg.hdr.id, 0, 0 );

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure to obtain the HDR forward link stats
*/

uint8 ftm_hdr_get_fwd_lnk_stats
(
   uint32 * total_tc_packets,
   uint32 * total_tc_errors
)
{
  uint8    status = 0;
  hdrhit_msg_cmd_u             hdrhit_cmd_msg;

  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_GET_FWD_LNK_STATS );
  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_get_fwd_lnk_stats_cmd_msg_t ));

  FTM_MSG_HIGH( "FTM Send HIT_GET_FWD_LNK_STATS to HDR: 0x%8x", 
                   hdrhit_cmd_msg.hdr.id, 0, 0 );

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This procedure processes a FTM command for getting HDR Rx power values.

  @details
  Not yet implemented. Need to port from test_mc.c

  @param *param
  Union of data structures of all FTM HDR MC commands. 

  @return
  FTM command status indicating success or failure of HDR MC command based on
  status tracked by global: test_cmd_status
*/

ftm_cmd_status_type ftm_hdr_rx_level_report
(
  ftm_packed_param_type *p_param, 
  ftm_rx_level_report_type* ftm_rx_level_report
)
{ 
  ftm_cmd_rx_level_param_type *param = &p_param->rx_level_req_params;
  rf_path_enum_type rx_chain = (rf_path_enum_type) param->rx_chain;
 
  if (rx_chain == RF_PATH_0)
  {
    ftm_rx_level_report->rx_chain = (byte)0x0;
    ftm_rx_level_report->rssi = 
      rfm_hdr_get_rx_agc(RFM_DEVICE_0,RFM_HDR_CARRIER_ID__CUMULATIVE,RFM_HDR_AGC_FORMAT__RAW);
    ftm_rx_level_report->rx_pwr_dbm10 = 
      (int16) rfm_hdr_get_rx_agc(RFM_DEVICE_0,RFM_HDR_CARRIER_ID__CUMULATIVE,RFM_HDR_AGC_FORMAT__DBM10);	
  }
  else if (rx_chain == RF_PATH_1)
  {
    ftm_rx_level_report->rx_chain = (byte)0x01;
    ftm_rx_level_report->rssi = 
      rfm_hdr_get_rx_agc(RFM_DEVICE_1,RFM_HDR_CARRIER_ID__CUMULATIVE,RFM_HDR_AGC_FORMAT__RAW);
    ftm_rx_level_report->rx_pwr_dbm10 = 
      (int16) rfm_hdr_get_rx_agc(RFM_DEVICE_1,RFM_HDR_CARRIER_ID__CUMULATIVE,RFM_HDR_AGC_FORMAT__DBM10);  
  }
  else
  {
    return TEST_INVALID_STATE;
  }
  return TEST_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HDR Test Dispatch function.

  @details
  This function handles requests to run HDR tests by dispatching the
  appropriate functions

  @param *data
  Request Data Packet for HDR Test

  @return
  Response packet to the diag after the Test is done
*/
ftm_rsp_pkt_type
ftm_hdr_mc_dispatch
(
  ftm_mc_pkt_type *ftm
)
{
  ftm_hdr_per_rsp_type   ftm_hdr_per_rsp;
  uint32                 total_tc_packets=0;
  uint32                 total_tc_errors=0;
  ftm_rx_level_rsp_type  ftm_rx_level_rsp;
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_mc_cmd_resp_type ftm_mc_response_data;
  ftm_mc_response_data.diag_hdr = ftm->diag_hdr;
  ftm_mc_response_data.sub_cmd = ftm->ftm_hdr.cmd_id;
  ftm_mc_response_data.status = TEST_SUCCESS;

  FTM_MSG_1(FTM_MED,"FTM HDR dispatching packet id: %d",ftm->ftm_hdr.cmd_id);

  ftm_rfmode_enter(RFM_DEVICE_0, FTM_STATE_HDR_NS);

  switch (ftm->ftm_hdr.cmd_id)
  {
  case HDR_FTM_ACQUIRE:
    ftm_hdr_acquire (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_SYNC:
    ftm_hdr_sync (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_DEMOD_FTC:
    ftm_hdr_demod_ftc (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_MOD_ACC:
    ftm_hdr_mod_acc (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_MOD_RTC:
    ftm_hdr_mod_rtc (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_DEASSIGN:
    ftm_hdr_deassign (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_IDLE_STATE:
    ftm_hdr_idle_state (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_DEMOD_FWLINK_REV0:
    ftm_hdr_demod_fwlink_rev0 (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_RELA_MAC_CONFIG:
    ftm_hdr_rela_mac_config (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_DEMOD_FWLINK_REVA:
     ftm_hdr_demod_fwlink_reva (&ftm->param);
     test_cmd_status =
     ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
     break;

  case HDR_FTM_MOD_RTC_REVA:
     ftm_hdr_mod_rtc_reva (&ftm->param);
     test_cmd_status =
     ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
     break;

  case HDR_FTM_MOD_ACC_REVA:
    ftm_hdr_mod_acc_reva (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status  = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_MOD_RTC_REVB:
    ftm_hdr_mod_rtc_revb (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
	break;

  case HDR_FTM_CONFIG_RTC_REVB:
    ftm_hdr_config_rtc_revb (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
	break;

  case HDR_FTM_REV_SET_PWR:
    test_cmd_status =
    ftm_mc_response_data.status =  ftm_hdr_set_tx_pwr (&ftm->param);
    break;

  case HDR_FTM_SET_CARRIER_TX_PWR:
    test_cmd_status =
    ftm_mc_response_data.status = hdr_ftm_set_carrier_tx_pwr (&ftm->param);
    break;

  case HDR_FTM_HRD_HND_OFF:
    ftm_hdr_hard_handoff (&ftm->param);
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_RST_FWD_LNK_STATS:
    ftm_hdr_reset_fwd_lnk_stats ();
    test_cmd_status =
    ftm_mc_response_data.status = ftm_hdr_wait_for_resp();
    break;

  case HDR_FTM_GET_FWD_LNK_STATS:
    ftm_hdr_get_fwd_lnk_stats (&total_tc_packets,&total_tc_errors);
    test_cmd_status =
    ftm_hdr_per_rsp.status           = ftm_hdr_wait_for_resp();
    hdrhitmsg_get_fwd_lnk_stats( &total_tc_packets,&total_tc_errors );
    ftm_hdr_per_rsp.diag_hdr         = ftm->diag_hdr;
    ftm_hdr_per_rsp.sub_cmd          = ftm->ftm_hdr.cmd_id;
    ftm_hdr_per_rsp.total_tc_packets = total_tc_packets;
    ftm_hdr_per_rsp.total_tc_errors  = total_tc_errors;
    break;

  case FTM_HDR_RX_LEVEL_REQ:
    ftm_rx_level_rsp.diag_hdr         = ftm->diag_hdr;
    ftm_rx_level_rsp.sub_cmd          = ftm->ftm_hdr.cmd_id;
    test_cmd_status =
    ftm_rx_level_rsp.status           = ftm_hdr_rx_level_report (&ftm->param, 
                                                                 &ftm_rx_level_rsp.ftm_rx_report);
    break;

  
  default:
    FTM_MSG(FTM_ERROR,"Unknown FTM MC Command");
    ftm_mc_response_data.status = TEST_INVALID_CMD;
    test_cmd_status = TEST_NO_SUCCESS;
    break;
  }

  switch (ftm_mc_response_data.status)
  {
  case TEST_SUCCESS:
  case TEST_INVALID_STATE:
  case TEST_NO_SUCCESS:
     switch (ftm->ftm_hdr.cmd_id)
      {
      case HDR_FTM_GET_FWD_LNK_STATS:
          rsp_pkt = ftmdiag_create_new_pkt_with_data((byte*)&ftm_hdr_per_rsp, 
                                                     sizeof(ftm_hdr_per_rsp),
                                                     (byte*)NULL, 0);
		  break;
      case FTM_HDR_RX_LEVEL_REQ:
          rsp_pkt = ftmdiag_create_new_pkt_with_data((byte*)&ftm_rx_level_rsp, 
                                                     sizeof(ftm_rx_level_rsp),
                                                     (byte*)NULL, 0);
		  break;

      default:
         rsp_pkt = ftmdiag_create_new_pkt_with_data((byte*)&ftm_mc_response_data, 
                                                    sizeof(ftm_mc_response_data),
                                                    (byte*)NULL, 0);
      }

    break;

  case TEST_INVALID_CMD:
  default:
    test_cmd_status = TEST_NO_SUCCESS;
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
  }
  
  return rsp_pkt;
} /* ftm_hdr_mc_dispatch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sends enable/disable modulator message to hdr. The message includes
  enable/disable and carrier mask
 
  @param carrier mask
  The carrier mask used to determine which carrier is to be activated.
 
  @param skip_dac_toggle
  The API skips DAC toggle if this variable is true.
  The API toggles the DAC if this variable is true.
 
*/

void 
ftm_hdr_mc_control_modulator
(
  uint8 carrier_mask,
  boolean skip_dac_toggle
)
{
  hdrhit_msg_cmd_u hdrhit_cmd_msg;

  ftm_hdr_init_msg( &hdrhit_cmd_msg, HDR_HIT_CMD_MODULATOR_CTL );

  hdrhit_cmd_msg.modulator_ctl.carrier_mask = carrier_mask;
  hdrhit_cmd_msg.modulator_ctl.skip_dac_toggle = skip_dac_toggle;

  ftm_hdr_send_msg( &hdrhit_cmd_msg, sizeof( hdrhit_modulator_ctl_cmd_msg_t ));

  FTM_MSG_HIGH( "ftm_hdr_mc_control_modulator():"
                "FTM Send HDR_HIT_CMD_MODULATOR_CTL to HDR: 0x%8x", 
                   hdrhit_cmd_msg.hdr.id, 0, 0 );
}/* ftm_hdr_mc_control_modulator */
#endif /* FEATURE_CDMA1X */

/*! @} */
