/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      H D R    H I T   C O M M A N D S


GENERAL DESCRIPTION
  This file contains implementation of HDR HIT commands

EXTERNALIZED FUNCTIONS
  hdrhitcmd_enable
    Enable HIT HDR 

  hdrhitcmd_acquire
    Perform acquisition operation
     
  hdrhitcmd_sync
    Perform sync operation`

  hdrhitcmd_idle
    Switch to Idle mode

  hdrhitcmd_cc_config
    Configure control channel

  hdrhitcmd_aset_info
    Initialize ASET information

  hdrhitcmd_demod_ftc
    Demodulate forward traffic channel

  hdrhitcmd_equalizer_config
    Configure equalizer

  hdrhitcmd_rl_config
    Configure reverse link

  hdrhitcmd_rtcmac_config
    Configure RTCMAC

  hdrhitcmd_mod_rl
    Enable reverse link

  hdrhitcmd_flmac_config
    Configure FL MAC

  hdrhitcmd_amac_config
    Configure AMAC

  hdrhitcmd_amac
    Issue AMAC commands

  hdrhitcmd_rotator_config
    Configure rotator

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                        EDIT HISTORY FOR MODULE


This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hdrhitcmd.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------  
04/29/15     vko     Inform TCA processing status, to HDRSRCH during 
                     IDLE_MODE_CMD
10/17/14     ljl     Included amssassert.h. 
03/14/14     dsp     FR19338:Reduce call setup time by skipping Tx RF shutdown.
11/26/13     rmg     Supported enabling carriers in FW without a frame delay.
09/25/13     wsh     Changes to support BOLT bring up
09/04/13     wsh     Changes to support BOLT bring up
09/03/13     wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version   
05/06/13     mbs     Fixed compiler errors for BOLT build
03/15/13     cnx     Fix Triton compiler errors.
01/17/13     rmg     Added support for removal of modulatorConfig field from
                     Rel0_rl_cfg FW message.
03/20/12     arm     Resolve TCB errors for Dime.
03/19/12     ljl     Fixed Dime compiler errors.
01/26/12     ljl     Replaced tramp with DAL.
01/05/12     wsh     Added support for SVLTE+SHDR
08/04/11     ssu     Replace hdr_ftmi_xx with hdrhitmsg_xx using hdrhitmsg APIs.
07/11/11     smd     Fixed a compiler error.
04/28/11     kss     Fixed compiler warning.     
03/24/11     kss     Updated RF SVDO API support.
06/30/10     arm     Remove all FEATURE_RF_BRINGUP_SUPPORT code blocks.
06/24/10     arm     Fixed compiler warnings.
06/22/10     arm     Changed short packet and MUP enable fields to boolean.
06/21/10     cnx     Fixed compiler warnings.
05/26/10     arm     Port non signaling legacy Macros to APIs for FTM.     
05/05/10     smd     Updated with new hdrsrch_acquire() and hdrsrch_synchronize().
03/17/10     ljl     Removed the obsolete include file.
12/01/09     kss     Added QRCT support in builds without HIT.
11/30/09     rmg     Initialized HDR shared memory pointer in start_rl_pilot.
11/23/09     rmg     Featurized hdrhitcmd_start_rl_pilot.
11/23/09     kss     Undef FEATURE_RF_BRINGUP_SUPPORT.
11/20/09     rmg     Added temporary changes to support RF bringup. 
11/16/09     kss     Fix null pointer issue in sync call. 
11/02/09     kss     Enabled demod_ftc on QDSP6.
10/20/09     rmg     Commented out artimis MDSP interaction for QDSP6.
07/17/09     ljl     Passed frame offset to HDR SRCH.
05/14/08     etv     Update AckOverride macro.
12/07/07     rmg     Changes to enable HIT on 7800.
11/11/07     kss     Use disable RPC macro.
10/29/07     ljl     Updated macros for harq mode.
09/27/07     ljl     Updated with Rev B predictor and handoff macros.
09/25/07     rmg     Made changes to support RevA RL testing on RUMI.
09/21/07     etv     Updated macro call for latest fw interface.
08/23/07     ljl     Set to Rev B mode.
08/16/07     rmg     Made changes for MC demod support
07/18/07     rmg     RevB AGC macro changes
07/12/07     ljl     Forced drc and updated asp 0 to demod ftc.
05/24/07     rmg     Undef FEATURE_FACTORY_TESTMODE if FEATURE_RUMI_BRINGUP
                     is defined
05/22/07     rmg     RevB macro changes
04/23/07     rmg     Fixed lint warnings
03/28/07     rmg     Created this file 
===========================================================================*/

/* EJECT */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "amssassert.h"
#include "hdrhitcmd.h"

#include "msm.h"

#include "hdrfw_msg.h"
#include "hdrmsg.h"
#include "hdrfmac.h"
#include "hdrfmaci.h"
#include "hdrsrchmsg.h"

#ifndef FEATURE_HDR_HIT
#include "err.h"
#include "hdrdebug.h"
#include "hdrmod.h"
#include "hdrmdspmcr.h"

/* List of bits for XOR for Q mask bit 0 */
#define HDRRMAC_XOR_LEN 20
LOCAL const byte hdrhitcmd_q_xor_array[] =
{
   0, 1, 2, 4, 5, 6, 9, 15, 16, 17, 18, 20, 21, 24, 25, 26,
  30, 32, 34, 41
};

#endif /* FEATURE_HDR_HIT */

/* Value of Soft/Softer Handoff delay timers for default FMAC */
#define HDRFMAC_DEFAULT_FMAC_HO_DELAY_TIMER     0

/*Default value of nullRateDRC384 for fmac_activate.*/
#define HDRSCMFMAC_DEFAULT_NULL_RATE_DRC_384_ENABLED      0x00

/*Value of MinContinuationSpan in subpackets(slots) */
#define HDRSCMFMAC_DEFAULT_MIN_CONTN_SPAN                 0x04

/* Handoff specific constants */
#define HDRFMACUTIL_MAX_CREDIT                  2
#define HDRFMACUTIL_SOFT_HO_CREDIT_THRESHOLD    2
#define HDRFMACUTIL_SOFTER_HO_CREDIT_THRESHOLD  2

/* TentativeDRCtoTxDRCMap Default Values */
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_01                  0x01
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_02                  0x02
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_03                  0x03
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_04                  0x04
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_05                  0x05
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_06                  0x06
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_07                  0x07
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_08                  0x08
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_09                  0x09
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0A                  0x0a
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0B                  0x0b
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0C                  0x0c
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0D                  0x0d
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0E                  0x0e
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_10                  0x05
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_11                  0x07
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_12                  0x07
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_13                  0x08
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_14                  0x08
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_15                  0x0a
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_16                  0x0b
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_17                  0x0b
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_18                  0x0c
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_19                  0x0e
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_1A                  0x0e
#define HDRSCMFMAC_DEFAULT_TX_DRC_MAP_1B                  0x0e

#ifdef FEATURE_HDR_HIT

#include "err.h"
#include "rex.h"
#include "task.h"
#include "hdrtx.h"
#include "hdrdebug.h"
#include "hdrsrchcmd.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdramac.h"
#include "hdrcmac.h"
#include "hdrrmac.h"
#include "hdrrmaccmn.h"
#include "hdrsrch.h"
#include "hdrsrchstate.h"
#include "hdrsrchstates.h"
#include "hdrsrcheq.h"
#include "hdrsrchtypes.h"
#include "hdrmod.h"
#include "hdrmdspmcr.h"
#include "hdrenc.h"
#include "rf.h"
#include "hdrsrchafc.h"
#if defined(FEATURE_FACTORY_TESTMODE) || defined(FEATURE_RUMI_BRINGUP)
#include "hdrhitmsg.h"
#endif

static boolean hdr_hit_enable = FALSE;

boolean hdrhitcmd_acquired    = FALSE;

hdrhitcmd_acquire_callback_type hdrhitcmd_acquire_callback;

hdrhitcmd_mod_rl_callback_type hdrhitcmd_mod_rl_callback;

extern void hdramac_process_cmd
(
  hdramac_cmd_union_type *cmd_ptr
);

extern void hdrfmac_enable_ack_mode
( 
  uint8 rl_carrier_id,
  hdrfmac_ack_mode_enum_type ack_mode
);

/* List of bits for XOR for Q mask bit 0 */
#define HDRRMAC_XOR_LEN 20
LOCAL const byte hdrhitcmd_q_xor_array[] =
{
   0, 1, 2, 4, 5, 6, 9, 15, 16, 17, 18, 20, 21, 24, 25, 26,
  30, 32, 34, 41
};

/* Structure for storing channel and pilot set */
typedef struct
{
  sys_channel_type         channel;  /* Channel */
  hdrsrch_aset_pilot_type  aset;     /* Active set pilot */
  uint8                    num_subactive_sets; /* Number of subactive sets */
  uint8                    cc_subactive_set_idx; /* subaset with control ch */
  hdrsrch_subactive_set_type subaset[3]; /* Subactive sets */
} hdrhitcmd_struct_type;

static hdrhitcmd_struct_type hdrhit;

/* Structure used for previous packet status */
typedef struct
{
  boolean   prev_pkt_accepted;
    /* Whether the packet loaded in the last subframe was loaded. */

  uint16    prev_pkt_id;
    /* The previous packet's id */

} hdrhitcmd_packet_info_struct_type;

/* Structure used for previous and current packet status */
typedef struct
{
  hdrhitcmd_packet_info_struct_type packet_info;
    /* Previoud packet status */

  uint32   tx_t2p;
    /* The tx_t2p value currently being sent (linear). */

  uint16   tx_pkt_id;
    /* The id of the packet currently being sent. */

  uint16   tx_interlace;
    /* The interlace of the packet currently being sent. */

  uint16   tx_subpacket;
    /* The subpacket of the packet currently being sent. */

  uint16   marq_nak_id;
    /* Id of marq'd packet, if MARQ occured. */

  uint16   subframe_n;
    /* Subframe number */

} hdrhitcmd_tx_pkt_status_type;

/* RTCMAC config */
typedef struct
{
  uint8  rev_mode;
  uint32 frame_count;
  uint8  data_rate[4];
  uint8  rate_mode;
  uint32 data_pattern;
  uint32 frames_to_send;
  uint8  pre_transition_gains[12];
  uint8  post_transition_gains[12];
  uint8  gain_transition_points[12];
  int8   pre_rri_gains[4];
  int8   post_rri_gains[4];
  uint8  tm;
  uint16 precount; /* not used currently */
  uint16 rtcmac_active;
  hdrhitcmd_tx_pkt_status_type packet_status;
} hdrhitcmd_rtcmac_type;

LOCAL hdrhitcmd_rtcmac_type hdrhitcmd_rtcmac;


/* Indicates an undefined or NULL packet */
#define HDRHITCMD_INVALID_PACKET            0xFE

/* Max Rev A payload size */
#define HDRHITCMD_MAX_REVA_PAYLOAD_SIZE     1532

byte hdrhitcmd_rl_packet[HDRHITCMD_MAX_REVA_PAYLOAD_SIZE] = {0};

/* Valid Rev A Physical Layer packet sizes (in octets) excluding 
 * Physical Layer overhead */
LOCAL const uint16 hdrhitcmd_revA_pkt_sizes[HDRRMACCMN_MAX_PAYLOADS] = 
                     { 0,
                       12,
                       28,
                       60,
                       92,
                       124,
                       188,
                       252,
                       380,
                       508,
                       764,
                       1020,
                       1532 };

/* Valid Rel 0 Physical Layer packet sizes (in octets) excluding 
 * Physical Layer overhead */
LOCAL const uint16 hdrhitcmd_rel0_pkt_sizes[HDRRMACCMN_MAX_PAYLOADS] = 
                   { 0,
                     28,
                     60,
                     124,
                     252,
                     508 };

/* Number of slots in a subframe */
#define HDRHITCMD_SLOTS_PER_SUBFRAME        4

/* Open State ISR offset, in chips. */
#define HDRHITCMD_INTERRUPT_OFFSET_CHIPS   (0 * 2048)

/* Data Channel Nominal Gains (in 1/4 dB) */
#define HDRHITCMD_RMAC0_DATA_NOM_GAIN_9K6   15
#define HDRHITCMD_RMAC0_DATA_NOM_GAIN_19K2  27
#define HDRHITCMD_RMAC0_DATA_NOM_GAIN_38K4  39
#define HDRHITCMD_RMAC0_DATA_NOM_GAIN_76K8  53
#define HDRHITCMD_RMAC0_DATA_NOM_GAIN_153K6 74

/*===========================================================================
FUNCTION HDRHITCMD_ENABLE

DESCRIPTION
  This function updates the variable hdr_hit_enable with the status of 
  hit_enable NV item passed from HIT

DEPENDENCIES
  None

RETURN VALUE
  word 

SIDE EFFECTS
  None
===========================================================================*/
void hdrhitcmd_enable
(
  uint8 hit_enable
    /* The status of the NV item - hit_enable */
)
{
  hdrhitcmd_rtcmac.rtcmac_active = 0;

  if (hit_enable == 1)
  {
    hdr_hit_enable = TRUE;
  }
  else
  {
    hdr_hit_enable = FALSE;
  }

} /* hdrhitcmd_enable */


/*===========================================================================
FUNCTION HDRHITCMD_ACTIVATE

DESCRIPTION
  This function activates hdrrx and hdrtx tasks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrhitcmd_activate()
{
  if (hdr_hit_enable)
  {
    /* Enable Rx and Tx */
    ( void ) rex_set_sigs( HDRRX_TASK_PTR, HDRRX_ENTER_HDR_SIG );
    ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );

    hdrsrch_deactivate(NULL);

    hdrsrch_activate();
  }

  return;

} /* hdrhitcmd_activate() */


/*===========================================================================
FUNCTION HDRHITCMD_HDRSRCH_ACQUIRE_CALLBACK

DESCRIPTION
  Callback function passed to HDR acquistision command.
  On successful acquisition prints a diagnostic message
  else deactivates current HDR processes and changes state to
  inactive state.

DEPENDENCIES

RETURN VALUE
  Will be called when pilot is acquired.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrhitcmd_hdrsrch_acquire_callback
(
  const sys_channel_type        *channel,
    /* Channel acquisition was attempted on */

  hdrsrch_acquire_enum_type     acq_result
    /* Acquisition success or failure status */
)
{
  /* If acquisition fails, deactivate current HDR processing
     and go to inactive state */ 
  if ( acq_result == HDRSRCH_ACQUIRED )
  {
    HDR_MSG_HIT( MSG_LEGACY_MED, "Acquired HDR Pilot ...." );

    hdrhitcmd_acquired = TRUE;

    hdrhitcmd_acquire_callback(HDRHITCMD_ACQUIRE_CB_ACQUIRED);

  }
  else
  {
    HDR_MSG_HIT( MSG_LEGACY_HIGH, "HDR Pilot Acq Failed ...." );

    hdrhitcmd_acquired = FALSE;

    hdrsrch_deactivate(NULL);

    hdrhitcmd_acquire_callback(HDRHITCMD_ACQUIRE_CB_ACQUIRE_FAILED);

  }

  return;

} /* hdrhitcmd_hdrsrch_acquire_callback */


/*===========================================================================
FUNCTION HDRHITCMD_HDRSRCH_SYSTEM_LOST_CALLBACK

DESCRIPTION
  Callback function passed to HDR acquistision command.
  Deactivates current HDR processes and changes state to
  inactive state. This is to enable HIT to start again
  from inactive state in case of system loss.

DEPENDENCIES

RETURN VALUE
  Will be called when HDR system gets lost.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrhitcmd_hdrsrch_system_lost_callback 
(  
  hdrsrch_system_lost_enum_type   reason
    /* System lost explanation */
)
{
  HDR_MSG_HIT( MSG_LEGACY_HIGH, \
    "HDR Pilot acquisition failed or system lost" );

  hdrhitcmd_acquired = FALSE;

  hdrsrch_deactivate(NULL);
    /* Deactivate HDR processing and go to inactive state */

  hdrhitcmd_acquire_callback((hdrhitcmd_acquire_status_type)
                             ((HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
                              (0x000F & reason)));

  return;

} /* hdrhitcmd_hdrsrch_system_lost_callback */


/*===========================================================================
FUNCTION HDRHITCMD_HDRSRCH_SYSTEM_USABILITY_CALLBACK

DESCRIPTION
  Callback function passed to HDR acquistision command.

DEPENDENCIES

RETURN VALUE
  Usability

SIDE EFFECTS
  None.
===========================================================================*/
void hdrhitcmd_hdrsrch_system_usability_callback
(
  hdrsrch_system_usable_enum_type   usability
)
{
  HDR_MSG_HIT( MSG_LEGACY_HIGH, \
    "HDR Pilot acquisition usability failed ...." );

  hdrhitcmd_acquired = FALSE;

  hdrhitcmd_acquire_callback((hdrhitcmd_acquire_status_type)
                             ((HDRHITCMD_STATUS_HIGH | SYSTEM_USABILITY_CB) | 
                              (0x000F & usability)));

  return;

} /* hdrhitcmd_hdrsrch_system_usability_callback */


/*===========================================================================
FUNCTION HDRHITCMD_ACQUIRE

DESCRIPTION
  This function implements the acquire command

DEPENDENCIES
  None

RETURN VALUE
  test_result

SIDE EFFECTS
  None
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_acquire
(
  hdrhitcmd_acquire_param_type*   param_ptr,
  hdrhitcmd_acquire_callback_type hdrhitcmd_acquire_cb
)
{
  sys_channel_type channel;
#if defined T_MDM9X00 || defined FEATURE_HDR_DIME_MODEM
  hdrsrch_acquire_params_type srch_acq_params;
#endif /* T_MDM9X00 */

  if (hdr_hit_enable)
  {
    channel.band     = (sys_band_class_e_type) (param_ptr->band_class);
    channel.chan_num = (sys_channel_num_type) (param_ptr->chan_num);

    hdrhit.channel = channel; 
    /* Channel number passed to the hdrhit structure for use during
       hdrhitcmd_demod_ftc command */

    hdrhitcmd_activate();

    hdrhitcmd_acquire_callback = hdrhitcmd_acquire_cb;

     /* Send Command to Search task */
#if defined T_MDM9X00  || defined FEATURE_HDR_DIME_MODEM
    srch_acq_params.channel = channel;
    srch_acq_params.mode = (sys_hdr_acq_mode_e_type) (param_ptr->acq_mode);
    srch_acq_params.acq_cb = 
      (hdrsrch_acquire_cb_type) hdrhitcmd_hdrsrch_acquire_callback;
    srch_acq_params.system_lost_cb = 
      (hdrsrch_system_lost_cb_type) hdrhitcmd_hdrsrch_system_lost_callback;
    srch_acq_params.usability_cb = 
      (hdrsrch_system_usable_cb_type) hdrhitcmd_hdrsrch_system_usability_callback;
    srch_acq_params.num_of_pn = 0;
    hdrsrch_acquire( &srch_acq_params);
#else
    hdrsrch_acquire( &channel, 
      (sys_hdr_acq_mode_e_type) (param_ptr->acq_mode), 
      (hdrsrch_acquire_cb_type) hdrhitcmd_hdrsrch_acquire_callback, 
      (hdrsrch_system_lost_cb_type) hdrhitcmd_hdrsrch_system_lost_callback, 
      (hdrsrch_system_usable_cb_type) 
      hdrhitcmd_hdrsrch_system_usability_callback );
#endif /* T_MDM9X00 */

    HDR_MSG_HIT( MSG_LEGACY_MED, "Wait for HDR SRCH ACQ rpt" );

    return HDRHITCMD_COMMAND_ISSUED;

  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_acquire */


/*===========================================================================
FUNCTION HDRHITCMD_SYNC

DESCRIPTION
  HIT command for sync state entry

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_sync(hdrhitcmd_sync_param_type* param_ptr)
{
  uint16 hstr_count = 0; /* store this in an internal structure if used at
                        multiple places in hdrhitcmd.c */

  qword sync_time;

  if (hdr_hit_enable)
  {

    qw_set(sync_time, 0, 0);

    hdrcmac_get_sync_caps_timestamp(&hstr_count);

    HDR_MSG_HIT_1( MSG_LEGACY_MED, \
      "HDR synchronizing.., hstr=%d", hstr_count);

    hdrsrch_synchronize(sync_time, param_ptr->pn_offset, hstr_count, NULL);

    return HDRHITCMD_COMMAND_SUCCESS;

  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_sync_state */


/*===========================================================================
FUNCTION HDRHITCMD_CC_CONFIG

DESCRIPTION
  This procedure implements hdr control channel config

DEPENDENCIES
  None
  
RETURN VALUE
  hdrhitcmd_status_type 

SIDE EFFECTS
  Sets AT to Rev A/Rev 0 mode
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_cc_config
(
  hdrhitcmd_cc_config_param_type* param_ptr
)
{
  hdrfw_msg_u fw_cmd;
    /* Firmware command */

  errno_enum_type estatus;
    /* Error status */


  if (hdr_hit_enable && hdrhitcmd_acquired)
  {
    msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_FW, HDR_FW_CC_DEMOD_CFG_CMD );
      /* Initialize firmware command for CC demod message */

    if (param_ptr->enable_cc_short_pkts)
    {
      HDR_MSG_HIT( MSG_LEGACY_MED, "Enabling CC short packets [Rev B]" );

      /* Make sure short packets and Rev A are enabled. */
      fw_cmd.cc_demod_cfg.hdrSystemModeConfig = 1;
      fw_cmd.cc_demod_cfg.hdrSystemModeConfig_Valid = 1;
      fw_cmd.cc_demod_cfg.hdrShortPacketMode = 1;

      /* Enable CC short packet in DSP */
      fw_cmd.cc_demod_cfg.ctrl3MACIndex = 0x47;
      fw_cmd.cc_demod_cfg.ctrl4MACIndex = param_ptr->sht_pkt_mac_index;

    }
    else
    {
      HDR_MSG_HIT( MSG_LEGACY_MED, "Disabling CC short packets [Rev 0]" );

      /* Disable short packets and set system mode to rel 0 */
      fw_cmd.cc_demod_cfg.hdrSystemModeConfig = 0;
      fw_cmd.cc_demod_cfg.hdrSystemModeConfig_Valid = 1;
      fw_cmd.cc_demod_cfg.hdrShortPacketMode = 0;

      /* Disable CC short packets. */
      fw_cmd.cc_demod_cfg.ctrl3MACIndex = 0x0000;
      fw_cmd.cc_demod_cfg.ctrl4MACIndex = 0x0000;
    }

    estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
    ASSERT ( estatus == E_SUCCESS );

    return HDRHITCMD_COMMAND_SUCCESS;
  }
  else if (!hdrhitcmd_acquired) /* Not acquired */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, "CC Config Failed: Not Acquired" );

    return HDRHITCMD_COMMAND_FAILED_NOT_ACQ;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_cc_config */


/*===========================================================================
FUNCTION HDRHITCMD_IDLE

DESCRIPTION
  HIT command for HDR idle

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_idle(hdrhitcmd_idle_param_type* param_ptr)
{
  if (hdr_hit_enable)
  {
    if ( hdrsrch_state_id == HDRSRCH_CONNECTED_STATE )
    {
      HDR_MSG_HIT_2( MSG_LEGACY_MED, \
        "HDR Idle State Command Rxed in Traffic : %d %x", 
        param_ptr->idle_param, param_ptr->idle_param );

      hdrsrch_idle_mode( NULL, NULL, FALSE );
        /* Switch PLT from traffic to idle state */
    }
    else
    {
      HDR_MSG_HIT( MSG_LEGACY_HIGH, \
        "Ignore HDR Idle State Command. Not in Traffic" );
    }

    return HDRHITCMD_COMMAND_SUCCESS;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_idle */


/*===========================================================================
FUNCTION HDRHITCMD_ASET_INFO

DESCRIPTION
  This procedure implements aset info initialization.  The input is 
  information for up to three subactive sets with one pilot each.

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_aset_info
(
  hdrhitcmd_aset_info_param_type* param_ptr
)
{
  if (hdr_hit_enable)
  {
    /* Number of subactive sets */
    hdrhit.num_subactive_sets = param_ptr->num_subactive_sets;

    /* Subactive set containing the control channel */
    hdrhit.cc_subactive_set_idx = param_ptr->cc_subactive_set_idx;

    /* Channel */
    hdrhit.subaset[0].channel.band = (sys_band_class_e_type) 
      (param_ptr->subaset_0_band_class);
    hdrhit.subaset[0].channel.chan_num = (sys_channel_num_type)
      (param_ptr->subaset_0_chan_num);

    hdrhit.subaset[1].channel.band = (sys_band_class_e_type) 
      (param_ptr->subaset_1_band_class);
    hdrhit.subaset[1].channel.chan_num = (sys_channel_num_type)
      (param_ptr->subaset_1_chan_num);

    hdrhit.subaset[2].channel.band = (sys_band_class_e_type) 
      (param_ptr->subaset_2_band_class);
    hdrhit.subaset[2].channel.chan_num = (sys_channel_num_type)
      (param_ptr->subaset_2_chan_num);

    /* System type */
    hdrhit.subaset[0].systype = SYS_HDR_CHAN_REC_MODE_IS856; /* HDR */
    hdrhit.subaset[1].systype = SYS_HDR_CHAN_REC_MODE_IS856;
    hdrhit.subaset[2].systype = SYS_HDR_CHAN_REC_MODE_IS856;

    /* Subactive set should not be reported field */
    hdrhit.subaset[0].not_reportable = param_ptr->subaset_0_not_reportable;
    hdrhit.subaset[1].not_reportable = param_ptr->subaset_1_not_reportable;
    hdrhit.subaset[2].not_reportable = param_ptr->subaset_2_not_reportable;

    /* Number of pilots in subactive set */
    hdrhit.subaset[0].num_aset_pilots = 1;
    hdrhit.subaset[1].num_aset_pilots = 1;
    hdrhit.subaset[2].num_aset_pilots = 1;

    /* Firmware demod carrier indices */
    hdrhit.subaset[0].demod_idx = param_ptr->subaset_0_demod_idx;
    hdrhit.subaset[1].demod_idx = param_ptr->subaset_1_demod_idx;
    hdrhit.subaset[2].demod_idx = param_ptr->subaset_2_demod_idx;

    /* Pilot PN */
    hdrhit.subaset[0].aset_list[0].pilot_pn = 
      (sys_pilot_pn_type) (param_ptr->subaset_0_sector_0_pilot_pn);
    hdrhit.subaset[1].aset_list[0].pilot_pn = 
      (sys_pilot_pn_type) (param_ptr->subaset_1_sector_0_pilot_pn);
    hdrhit.subaset[2].aset_list[0].pilot_pn = 
      (sys_pilot_pn_type) (param_ptr->subaset_2_sector_0_pilot_pn);

    /* RPC is in softer handoff with previous pilot */
    hdrhit.subaset[0].aset_list[0].softer_handoff = 
      param_ptr->subaset_0_sector_0_softer_handoff;
    hdrhit.subaset[1].aset_list[0].softer_handoff = 
      param_ptr->subaset_1_sector_0_softer_handoff;
    hdrhit.subaset[2].aset_list[0].softer_handoff = 
      param_ptr->subaset_2_sector_0_softer_handoff;

    /* MAC Index */
    hdrhit.subaset[0].aset_list[0].mac_index = 
      param_ptr->subaset_0_sector_0_mac_index;
    hdrhit.subaset[1].aset_list[0].mac_index = 
      param_ptr->subaset_1_sector_0_mac_index;
    hdrhit.subaset[2].aset_list[0].mac_index = 
      param_ptr->subaset_2_sector_0_mac_index;

    /* DRC Cover */
    hdrhit.subaset[0].aset_list[0].drc_cover = 
      param_ptr->subaset_0_sector_0_drc_cover;
    hdrhit.subaset[1].aset_list[0].drc_cover = 
      param_ptr->subaset_1_sector_0_drc_cover;
    hdrhit.subaset[2].aset_list[0].drc_cover = 
      param_ptr->subaset_2_sector_0_drc_cover;

    /* RL MAC Index */
    hdrhit.subaset[0].aset_list[0].rl_mac_index = 
      param_ptr->subaset_0_sector_0_rl_mac_index;
    hdrhit.subaset[1].aset_list[0].rl_mac_index = 
      param_ptr->subaset_1_sector_0_rl_mac_index;
    hdrhit.subaset[2].aset_list[0].rl_mac_index = 
      param_ptr->subaset_2_sector_0_rl_mac_index;

    /* RAB MAC Index */
    hdrhit.subaset[0].aset_list[0].rab_mac_index = 
      param_ptr->subaset_0_sector_0_rab_mac_index;
    hdrhit.subaset[1].aset_list[0].rab_mac_index = 
      param_ptr->subaset_1_sector_0_rab_mac_index;
    hdrhit.subaset[2].aset_list[0].rab_mac_index = 
      param_ptr->subaset_2_sector_0_rab_mac_index;

    /* Reverse Activity Bit length */
    hdrhit.subaset[0].aset_list[0].rab_length = 
      param_ptr->subaset_0_sector_0_rab_length;
    hdrhit.subaset[1].aset_list[0].rab_length = 
      param_ptr->subaset_1_sector_0_rab_length;
    hdrhit.subaset[2].aset_list[0].rab_length = 
      param_ptr->subaset_2_sector_0_rab_length;

    /* Reverse Activity Bit offset */
    hdrhit.subaset[0].aset_list[0].rab_offset = 
      param_ptr->subaset_0_sector_0_rab_offset;
    hdrhit.subaset[1].aset_list[0].rab_offset = 
      param_ptr->subaset_1_sector_0_rab_offset;
    hdrhit.subaset[2].aset_list[0].rab_offset = 
      param_ptr->subaset_2_sector_0_rab_offset;

    /* RA Channel Gain */
    hdrhit.subaset[0].aset_list[0].ra_channel_gain = 
      param_ptr->subaset_0_sector_0_ra_channel_gain;
    hdrhit.subaset[1].aset_list[0].ra_channel_gain = 
      param_ptr->subaset_1_sector_0_ra_channel_gain;
    hdrhit.subaset[2].aset_list[0].ra_channel_gain = 
      param_ptr->subaset_2_sector_0_ra_channel_gain;

    /* TxT2P adjust value */
    hdrhit.subaset[0].aset_list[0].delta_t2p = 
      param_ptr->subaset_0_sector_0_delta_t2p;
    hdrhit.subaset[1].aset_list[0].delta_t2p = 
      param_ptr->subaset_1_sector_0_delta_t2p;
    hdrhit.subaset[2].aset_list[0].delta_t2p = 
      param_ptr->subaset_2_sector_0_delta_t2p;

    /* DSC associated with the cell */
    hdrhit.subaset[0].aset_list[0].dsc = 
      param_ptr->subaset_0_sector_0_dsc;
    hdrhit.subaset[1].aset_list[0].dsc = 
      param_ptr->subaset_1_sector_0_dsc;
    hdrhit.subaset[2].aset_list[0].dsc = 
      param_ptr->subaset_2_sector_0_dsc;

    /* Pilot group id */
    hdrhit.subaset[0].aset_list[0].pilot_group_id = 
      param_ptr->subaset_0_sector_0_pilot_group_id;
    hdrhit.subaset[1].aset_list[0].pilot_group_id = 
      param_ptr->subaset_1_sector_0_pilot_group_id;
    hdrhit.subaset[2].aset_list[0].pilot_group_id = 
      param_ptr->subaset_2_sector_0_pilot_group_id;

    /* Scheduler group */
    hdrhit.subaset[0].aset_list[0].scheduler_tag = 
      param_ptr->subaset_0_sector_0_scheduler_tag;
    hdrhit.subaset[1].aset_list[0].scheduler_tag = 
      param_ptr->subaset_1_sector_0_scheduler_tag;
    hdrhit.subaset[2].aset_list[0].scheduler_tag = 
      param_ptr->subaset_2_sector_0_scheduler_tag;

    return HDRHITCMD_COMMAND_SUCCESS;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_aset_info */


/*===========================================================================
FUNCTION HDRHITCMD_HDRLMAC_TC_ASSIGN_COMPLETE_CB

DESCRIPTION
  Callback function passed to HDR TC assignment command.

DEPENDENCIES

RETURN VALUE
  Will be called when HDR traffic channel is assigned.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrhitcmd_hdrlmac_tc_assign_complete_cb ( void )
{
  HDR_MSG_HIT( MSG_LEGACY_MED, \
    "HDR traffic channel assigned ...." );

  return;

} /* hdrhitcmd_hdrlmac_tc_assign_complete_cb */


/*===========================================================================
FUNCTION HDRHITCMD_DEMOD_FTC

DESCRIPTION
  HIT command for demodulation of fwd traffic channel

DEPENDENCIES
  Run hdrhitcmd_aset_info() before executing this function

RETURN VALUE
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_demod_ftc
(
  hdrhitcmd_demod_ftc_param_type* param_ptr
)
{

  hdrfw_msg_u fw_cmd;
    /* Firmware command  */

  errno_enum_type estatus;
    /* Error status */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset ( &fw_cmd, 0, sizeof (hdrfw_msg_u) );

  if (hdr_hit_enable && hdrhitcmd_acquired)
  {
    HDR_MSG_HIT( MSG_LEGACY_MED, \
      "Demodulating HDR forward traffic ..." );

    // KSS: Following 4 sets of fields should be set by hdrsrch_chan_and_aset_assign.
    // ...right?
    // {
    /* Write the RPC Cell map to the asp structure */
    //HDRASP_SET_CELL_MAP( HDRMDSP_DEMOD_CARRIER_0, 0, 1 );
    //HDRASP_SET_CELL_MAP( 1, 0, 1 );
    //HDRASP_SET_CELL_MAP( 2, 0, 1 );

    /* Set the RAB Length parameter. */
    //HDRASP_SET_RAB_LENGTH( HDRMDSP_DEMOD_CARRIER_0, 0, 8 ); 
    //HDRASP_SET_RAB_LENGTH( 1, 0, 8 ); 
    //HDRASP_SET_RAB_LENGTH( 2, 0, 8 ); 
     
    /* Set the RAB Offset parameter. The standard specifies that the RAB
       offset is set to the value equal to "RAB OFFSET FIELD * RAB_LEN/8"*/
    //HDRASP_SET_RAB_OFFSET( HDRMDSP_DEMOD_CARRIER_0, 0, 0 );
    //HDRASP_SET_RAB_OFFSET( 1, 0, 0 );
    //HDRASP_SET_RAB_OFFSET( 2, 0, 0 );

    /* Write the Link ID */
    //HDRASP_SET_LINK_ID( HDRMDSP_DEMOD_CARRIER_0, 0, 0x31 );
    //HDRASP_SET_LINK_ID( 1, 0, 0x32 );
    //HDRASP_SET_LINK_ID( 2, 0, 0x33 );
    // }
      
    hdrsrchtc_init_connected_state();

    hdrsrch_chan_and_aset_assign_revb( 
      HDRSRCH_RUP_SUBTYPE_1, 
      HDRSRCH_TUNE_AWAY_NONE,
      hdrhit.num_subactive_sets,
      &hdrhit.subaset[0],
      hdrhit.cc_subactive_set_idx,
      0, /* TBD Set frame offset to HDR SRCH */
      hdrhitcmd_hdrlmac_tc_assign_complete_cb, 
      NULL,
      NULL );

    // KSS Handoff override accomplished by message instead.
    // {
    /* Set DSP to override DRC channel decisions with those 
       recommended by uP */
    //HDRDRC_HANDOFF_CTRL_MICRO();

    /* Set DSP to use uP dictated DRC values */
    //HDRDRC_PREDICTOR_CTRL_MICRO_MC(HDRMDSP_DEMOD_CARRIER_0);
    //HDRDRC_PREDICTOR_CTRL_MICRO_MC(1);
    //HDRDRC_PREDICTOR_CTRL_MICRO_MC(2);

    /* Set the best ASP dictated by the uP in the VAR mode */
    //HDRDRC_SET_VAR_BEST_ASP( 0x0 );
    // }

    msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_FW, HDR_FW_HANDOFF_OVERRIDE_CMD );

    fw_cmd.handoff_override.handoffOverride[0].carrierValid = 1;
    fw_cmd.handoff_override.handoffOverride[0].bestASPDict = 0;
    fw_cmd.handoff_override.handoffOverride[0].handoffOverride.override = 1;
    fw_cmd.handoff_override.handoffOverride[0].handoffOverride.trigger = 0;
    
    fw_cmd.handoff_override.handoffOverride[1].carrierValid = 1;
    fw_cmd.handoff_override.handoffOverride[1].bestASPDict = 0;
    fw_cmd.handoff_override.handoffOverride[1].handoffOverride.override = 1;
    fw_cmd.handoff_override.handoffOverride[1].handoffOverride.trigger = 0;

    fw_cmd.handoff_override.handoffOverride[2].carrierValid = 1;
    fw_cmd.handoff_override.handoffOverride[2].bestASPDict = 0;
    fw_cmd.handoff_override.handoffOverride[2].handoffOverride.override = 1;
    fw_cmd.handoff_override.handoffOverride[2].handoffOverride.trigger = 0;


    estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
    ASSERT ( estatus == E_SUCCESS );

    // KSS: Use set DRC command below.
    // {
    /* Set the ARM dictated DRC value in VAR rate state */
    //HDRDRC_SET_VAR_DRC_VAL_MC( HDRMDSP_DEMOD_CARRIER_0, 
    //                            param_ptr->demod_0_rate );
    //HDRDRC_SET_VAR_DRC_VAL_MC( 1, 
    //                           param_ptr->demod_1_rate );
    //HDRDRC_SET_VAR_DRC_VAL_MC( 2, 
    //                           param_ptr->demod_2_rate );
    // }

    // KSS: Replace with command
    // {
    //HDRDRC_DISABLE_DRC_GATING();

    /* Set drc length */
    //HDRDRC_SET_DRC_LENGTH_MC( HDRMDSP_DEMOD_CARRIER_0, 0x4 );
    //HDRDRC_SET_DRC_LENGTH_MC( 1, 0x4 );
    //HDRDRC_SET_DRC_LENGTH_MC( 2, 0x4 );
    // }

    memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u ) );
      /* Initialize the message data with zeroes */

    msgr_init_hdr (
      &fw_cmd.hdr, MSGR_HDR_FMAC, HDR_FW_FTCMAC_CHANNEL_CFG_CMD );

    fw_cmd.ftcmac_channel_cfg.frameOffset = 0;
    fw_cmd.ftcmac_channel_cfg.fmacGains[0].drcLength = 4;
    fw_cmd.ftcmac_channel_cfg.fmacGains[1].drcLength = 4;
    fw_cmd.ftcmac_channel_cfg.fmacGains[2].drcLength = 4;
       //hdrfmac_drc_length_table[saset_info_ptr->drc_length];

    fw_cmd.ftcmac_channel_cfg.carrierMask = 1;   
       // Note only one carrier for now

    fw_cmd.ftcmac_channel_cfg.drcGating = 0;

    estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
      /* Send the message */

    ASSERT ( estatus == E_SUCCESS );
      
    
    // KSS Done by sync? Why do this again?
    // {
    /* Set Hyperspace Flag (after synchronization): DSP attempts preamble
       detection only on even half slot boundaries, TX is enabled */
    //HDRMSTR_SET_HYPERSPACE_FLAG();
    // }
  
    /* Enable DRC channel in the DSP - done by message below */
    //HDRDRC_ENABLE_DRC_MC( HDRMDSP_DEMOD_CARRIER_0 ); 

    memset ( &fw_cmd, 0, sizeof (hdrfw_msg_u) );

    msgr_init_hdr (
      &fw_cmd.hdr, MSGR_HDR_FMAC, HDR_FW_REL0_RL_CFG_CMD );
      /* Initialize the message header */

    fw_cmd.rel0_rl_cfg.enableDRC = 1;
    fw_cmd.rel0_rl_cfg.fieldMask.enableDRC_Valid = 1;
    
    estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
      /* Send the message */

    ASSERT ( estatus == E_SUCCESS );


    /* Set the max DRC value - use message */
    //HDRDRC_SET_MAX_DRC( HDRHAI_DRC_1536 );

    memset ( &fw_cmd, 0, sizeof (hdrfw_msg_u) );

    msgr_init_hdr (
      &fw_cmd.hdr, MSGR_HDR_FMAC, HDR_FW_MAX_DRC_CFG_CMD );
      /* Initialize the message header */

    fw_cmd.max_drc_cfg.maxDRC = HDRHAI_MAX_REL_A_DRC;
       /* The max allowed DRC is 14 */

    estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
      /* Send the message */

    ASSERT ( estatus == E_SUCCESS );
    
    /* Force to DRC and Mac Index */
    hdrsrch_at_set_drc( 0, 0xc | param_ptr->demod_0_rate );
    hdrsrch_at_set_drc( 1, 0xc | param_ptr->demod_1_rate );
    hdrsrch_at_set_drc( 2, 0xc | param_ptr->demod_2_rate );

    return HDRHITCMD_COMMAND_SUCCESS;
  }
  else if (!hdrhitcmd_acquired) /* Not acquired */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, "Demod FTC Failed: Not Acquired" );

    return HDRHITCMD_COMMAND_FAILED_NOT_ACQ;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_demod_ftc */


/*===========================================================================
FUNCTION HDRHITCMD_EQUALIZER_CONFIG

DESCRIPTION
  HIT command for configuring equalizer
  
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_equalizer_config
(
  hdrhitcmd_equalizer_config_param_type* param_ptr
)
{
  hdrsrcheq_config_struct_type            eq_ptr;
    /* Convenience pointer */

  if (hdr_hit_enable && hdrhitcmd_acquired)
  {
    HDR_MSG_HIT_1( MSG_LEGACY_MED, \
      "HDR Equalizer Config Command %x", param_ptr->demod_pref );
    
  /* Equalizer 0  configuration parameters */ 
    eq_ptr.enable                       = param_ptr->eq0_enable;
    eq_ptr.lms_step                     = param_ptr->eq0_lms_step;
    eq_ptr.filt_len                     = param_ptr->eq0_filt_len;
    eq_ptr.div                          = param_ptr->eq0_div;
    eq_ptr.multipass.enable             = (hdrsrcheq_multipass_enable_enum_type)
                                          (param_ptr->eq0_mulpass_enable);
    eq_ptr.multipass.lms_step[0]        = param_ptr->eq0_mulpass2_lms_step;
    eq_ptr.multipass.lms_step[1]        = param_ptr->eq0_mulpass3_lms_step;
    eq_ptr.multipass.multipass_c2i_used = param_ptr->eq0_multi_pass_c2i_used;
    hdrsrcheq_eq_config( 0, &eq_ptr );

  /* Equalizer 1  configuration parameters */ 
    eq_ptr.enable                       = param_ptr->eq1_enable;
    eq_ptr.lms_step                     = param_ptr->eq1_lms_step;
    eq_ptr.filt_len                     = param_ptr->eq1_filt_len;
    eq_ptr.div                          = param_ptr->eq1_div;
    eq_ptr.multipass.enable             = (hdrsrcheq_multipass_enable_enum_type)
                                          (param_ptr->eq1_mulpass_enable);
    eq_ptr.multipass.lms_step[0]        = param_ptr->eq1_mulpass2_lms_step;
    eq_ptr.multipass.lms_step[1]        = param_ptr->eq1_mulpass3_lms_step;
    eq_ptr.multipass.multipass_c2i_used = param_ptr->eq1_multi_pass_c2i_used;
    hdrsrcheq_eq_config( 1, &eq_ptr );

  /* Equalizer 2  configuration parameters */ 
    eq_ptr.enable                       = param_ptr->eq2_enable;
    eq_ptr.lms_step                     = param_ptr->eq2_lms_step;
    eq_ptr.filt_len                     = param_ptr->eq2_filt_len;
    eq_ptr.div                          = param_ptr->eq2_div;
    eq_ptr.multipass.enable             = (hdrsrcheq_multipass_enable_enum_type)
                                          (param_ptr->eq2_mulpass_enable);
    eq_ptr.multipass.lms_step[0]        = param_ptr->eq2_mulpass2_lms_step;
    eq_ptr.multipass.lms_step[1]        = param_ptr->eq2_mulpass3_lms_step;
    eq_ptr.multipass.multipass_c2i_used = param_ptr->eq2_multi_pass_c2i_used;
    hdrsrcheq_eq_config( 2, &eq_ptr );


  /* Equalizer 3  configuration parameters */ 
    eq_ptr.enable                       = param_ptr->eq3_enable;
    eq_ptr.lms_step                     = param_ptr->eq3_lms_step;
    eq_ptr.filt_len                     = param_ptr->eq3_filt_len;
    eq_ptr.div                          = param_ptr->eq3_div;
    eq_ptr.multipass.enable             = (hdrsrcheq_multipass_enable_enum_type)
                                          (param_ptr->eq3_mulpass_enable);
    eq_ptr.multipass.lms_step[0]        = param_ptr->eq3_mulpass2_lms_step;
    eq_ptr.multipass.lms_step[1]        = param_ptr->eq3_mulpass3_lms_step;
    eq_ptr.multipass.multipass_c2i_used = param_ptr->eq3_multi_pass_c2i_used;
    hdrsrcheq_eq_config( 3, &eq_ptr );

    /* Set demod preference */
    hdrsrcheq_override_mode_pref( (hdrsrcheq_demod_pref_enum_type)
                                  param_ptr->demod_pref );

    HDR_MSG_HIT( MSG_LEGACY_MED, "Equalizer Configuration complete" );

    return HDRHITCMD_COMMAND_SUCCESS;
  }
  else if (!hdrhitcmd_acquired) /* Not acquired */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, "Equalizer Config Failed: Not Acquired" );

    return HDRHITCMD_COMMAND_FAILED_NOT_ACQ;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_equalizer_config */


/*===========================================================================

FUNCTION HDRHITCMD_RL_CONFIG

DESCRIPTION
  HIT command for configuring reverse link
  
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_rl_config
(
  hdrhitcmd_rl_config_param_type* param_ptr
)
{
  qword           i_mask,q_mask;
  uint8           i;
  uint32          i_shift, i_mask_32, xor_result; 

  if (hdr_hit_enable && hdrhitcmd_acquired)
  {
    /* Disable Tx AGC access mode */
    //HDRAGC_DISABLE_ACCESS_MODE();

    qw_set(i_mask , param_ptr->i_mask_hi,param_ptr->i_mask_lo);

    if (param_ptr->q_mask_hi == 0 && param_ptr->q_mask_lo == 0 )
    {
      /* Construct Q mask */
      /* First, figure out what bit 0 should be. */
      xor_result = 0;
      for ( i=0; i<HDRRMAC_XOR_LEN; i++ )
      {
        i_shift = hdrhitcmd_q_xor_array[i];

        if (i_shift > 31 )
        {
          i_mask_32 = qw_hi(i_mask);
          i_shift-=32;
        } else
        {
          i_mask_32 = qw_lo(i_mask);
        }

        xor_result ^= ((i_mask_32 >> i_shift) & 0x00000001);
      }

      /* Set Q to I, shifted by one bit to the left. */
      qw_set(q_mask, qw_hi(i_mask), qw_lo(i_mask));

      qw_shift(q_mask, 1);

      /* Add in last bit. */
      qw_set(q_mask, qw_hi(q_mask) & 0x3ff, (qw_lo(q_mask) | xor_result));

      HDR_MSG_HIT_2( MSG_LEGACY_MED, \
        "I_MASK_U = %x, I_MASK_L = %x", i_mask[1], i_mask[0] );
      HDR_MSG_HIT_2( MSG_LEGACY_MED, \
        "Q_MASK_U = %x, Q_MASK_L = %x", q_mask[1], q_mask[0] );

    }
    else
    {
      qw_set(q_mask , param_ptr->q_mask_hi, param_ptr->q_mask_lo);
    }

    /* Program the user long code mask into the modulator */
    hdrmod_set_pn_long_code_mask( i_mask, q_mask
#ifdef FEATURE_HDR_BOLT_MODEM
      , 1                /* Enable carrier 0 PN generator */
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    /* Inform DSP of the new TX frame offet */
    //HDRDRC_SET_FRAME_OFFSET( param_ptr->frame_offset );

    if ( param_ptr->rl_mode == 0x0 ) /* Rev 0 */
    {
      /* Set the modulator to Rev 0 traffic mode */
      hdrmod_set_traffic_mode( HDRMOD_TRAFFIC_MODE_REV0 );

      /* Set tx frame start offset to the designated slot */
      hdrmod_set_tx_frame_start_offset( ((uint8) (param_ptr->frame_offset)) );

      /* Set the modulator frame interrupt offset */
      hdrmod_set_tx_frame_interrupt_offset( (param_ptr->frame_offset \
        + 16 - 4) & 0xf );

    }
    else if ( param_ptr->rl_mode == 0x1) /* Rev A */
    {
      /* Set the modulator to Rev A traffic mode */
      hdrmod_set_traffic_mode( HDRMOD_TRAFFIC_MODE_REVA );

      hdrmod_set_tx_subframe_interrupt_offset( \
        param_ptr->frame_offset % HDRHITCMD_SLOTS_PER_SUBFRAME );

      hdrmod_set_tx_frame_interrupt_offset( ((param_ptr->frame_offset \
        * 2048) + (4 * 2048) - HDRHITCMD_INTERRUPT_OFFSET_CHIPS) % (4 * 2048) );

    }

    /* Set the modulator transmit frame offset to zero */
    hdrmod_set_tx_frame_start_offset( ((uint8) (param_ptr->frame_offset)) );

    if( param_ptr->drc_override )
    {
      /* Set DSP to override DRC channel decisions with those 
         recommended by uP */
      //HDRDRC_HANDOFF_CTRL_MICRO();

      /* Set DSP to use uP dictated DRC values */
      //HDRDRC_PREDICTOR_CTRL_MICRO_MC(HDRMDSP_DEMOD_CARRIER_0);

      if( param_ptr->drc_gating_enable == 0x1 )
      {
        //HDRDRC_ENABLE_DRC_GATING();
      }
      else
      {
        //HDRDRC_DISABLE_DRC_GATING();
      }

      if( param_ptr->drc_mode == 0x1 )
      {
        /* Fix mode is not supported */
      }
      else
      {
        /* Set the best ASP dictated by the uP in the VAR mode */
        //HDRDRC_SET_VAR_BEST_ASP(0x1); //MDSP_WRITE (HDR_bestAspFLDict, 0x1);
                                      //MDSP_WRITE (HDR_varBestAspRLDict, 0x1);

        /* Set the ARM dictated DRC value in VAR rate state */
        //HDRDRC_SET_VAR_DRC_VAL_MC( HDRMDSP_DEMOD_CARRIER_0, 
        //                        (param_ptr->drc_value & 0x0f) );
      }

      /* Enable DRC channel in the DSP */
      //HDRDRC_ENABLE_DRC_MC(HDRMDSP_DEMOD_CARRIER_0);

      if( param_ptr->drc_cover_override == 0x1)
      {
        //HDRASP_SET_DRC_COVER( HDRMDSP_DEMOD_CARRIER_0, 1, 
        //                      (param_ptr->drc_cover_value & 0x0f) );
      }

      /* Set drc length */
      //HDRDRC_SET_DRC_LENGTH_MC( HDRMDSP_DEMOD_CARRIER_0, 
      //                          param_ptr->drc_length );

      /* Trigger Tx AGC double-buffered parameters to be 
         re-loaded and used by the DSP */
      //HDRAGC_LOAD_TX_AGC_PARAMS();

      if (param_ptr->rl_mode == 0x0) /* Rev 0 specific configuration */
      {
        /* Set Hyperspace Flag (after synchronization): DSP attempts preamble
           detection only on even half slot boundaries, TX is enabled */
        //HDRMSTR_SET_HYPERSPACE_FLAG();

        /* Set the max DRC value (After RTCAck is received and after the 
           Traffic is deactivated) ---- needs hdrhai.h ---- */
        //HDRDRC_SET_MAX_DRC( HDRHAI_MAX_REL_0_DRC );

        /* Enables Rate Control Loop version 0 */
        //HDRDRC_ENABLE_RCL_0();
      }
    }
    else
    {
      /* Set DSP to use its DRC channel decisions */
      //HDRDRC_HANDOFF_CTRL_DSP();

      /* Set DSP to use its own DRC rate decisions */
      //HDRDRC_PREDICTOR_CTRL_DSP_MC(HDRMDSP_DEMOD_CARRIER_0);
    }

    if( param_ptr->rl_mode == 0x1) /* Rev A specific configuration */
    {
      if( param_ptr->ack_override & param_ptr->arq_mode )
      {
        /* Configures Rev A H-ARQ for Mode 1 (ON/OFF Keying) */
        //HDRMDSP_CONFIG_HARQ_MODE_1_MC( HDRMDSP_DEMOD_CARRIER_0 );
      }
      else
      {
        /* Configures Rev A H-ARQ for Mode 0 (BPSK) */
        //HDRMDSP_CONFIG_HARQ_MODE_0_MC( HDRMDSP_DEMOD_CARRIER_0 );
      }
    }

    if( param_ptr->ack_override )
    {
      /* Enable DSP ack override mode. */
      //HDRDSP_ENABLE_ACK_OVERRIDE(HDRMDSP_DEMOD_CARRIER_0);


      hdrfmac_enable_ack_mode(HDRMDSP_DEMOD_CARRIER_0,
                               (hdrfmac_ack_mode_enum_type) (param_ptr->ack_mode) );

    }
    else
    {
      /* Disable DSP ack override mode (Default) */
      //HDRDSP_DISABLE_ACK_OVERRIDE(HDRMDSP_DEMOD_CARRIER_0);
    }

    /* Set DRC offset value for Tx */
    //HDRAGC_SET_DRC_OFFSET_MC( HDRMDSP_DEMOD_CARRIER_0, 
    //                          ((int16)(param_ptr->drc_ch_gain)) );

    /* Set ACK offset value for Tx */
    //HDRAGC_SET_ACK_OFFSET_MC( HDRMDSP_DEMOD_CARRIER_0, 
    //                          ((int16)(param_ptr->ack_ch_gain)) );

    /* Trigger Tx AGC double-buffered parameters to be 
       re-loaded and used by the DSP */
    //HDRAGC_LOAD_TX_AGC_PARAMS();

    if ( param_ptr->rl_mode == 0x1 ) /* Rev A specific configuration */
    {
#ifdef FEATURE_RF_SVDO_API
      /* Set the nominal power as specified by the Access Network - Tx adjust
         is reported based on this value*/
      rfm_hdr_set_nominal_pwr( hdrsrchrf_get_rf_dev(HDRSRCHRF_PRI_CHAIN), 
                               param_ptr->power_level );

      /* Set the Tx power adjustment, in 1/2 dB units */
      rfm_hdr_set_tx_power_adjust( hdrsrchrf_get_rf_dev(HDRSRCHRF_PRI_CHAIN),
                                   0 );
#else
      /* Set the nominal power as specified by the Access Network - Tx adjust
         is reported based on this value*/
      rf_hdr_set_nominal_pwr( param_ptr->power_level );

      /* Set the Tx power adjustment, in 1/2 dB units */
      rf_hdr_set_tx_power( 0x0 );
#endif
    }

#ifdef FEATURE_FACTORY_TESTMODE
    hdrhitmsg_get_rmac_params_ptr()->arq_mode   = param_ptr->arq_mode;
    hdrhitmsg_get_rmac_params_ptr()->rpc_enable = param_ptr->rpc_enable;
#endif /* FEATURE_FACTORY_TESTMODE */

    if( param_ptr->rpc_enable )
    {
      /* Set RPC step size: Macro takes units of 1/2 dB.
         In RPC_step field, 0 = .5 dB steps
                            1 = 1.0 dB steps.
         (Thus 1 is added to convert)  */
      //HDRAGC_SET_RPC_STEP_SIZE( (param_ptr->rpc_step + 1) );

      /* Enable the transmit closed loop power control */
      //HDRAGC_ENABLE_RPC_MC( HDRMDSP_DEMOD_CARRIER_0 );
    }
    else
    {
      if ( param_ptr->rl_mode == 0x0 ) /* Rev 0 specific configuration */
      {
        /* Set Tx AGC closed loop adjustment value - Only valid if 
           RPC is in override */
        //HDRAGC_SET_TX_GAIN_ADJ_MC( HDRMDSP_DEMOD_CARRIER_0, 
        //                           (((uint32)param_ptr->power_level) * 256)
        //                           - (76 * 512) );

        /* Disables the HDR Tx Reverse Power Control loop */
        //HDRAGC_OVERRIDE_RPC_MC( HDRMDSP_DEMOD_CARRIER_0 );
      }
      else if( param_ptr->rl_mode == 0x1) /* Rev A specific configuration */
      {
        /* Disable rpc - Tx AGC value is overridden to 
           the last value set by rf_set_tx_power() */
        //HDRAGC_DISABLE_RPC();
      }
    }

    if( param_ptr->rl_mode == 0x1 ) /* Rev A specific configuration */
    {
      if( param_ptr->dsc_ch_override )
      {
        /* Write DSC for the ASP */
        //HDRASP_SET_DSC( HDRMDSP_DEMOD_CARRIER_0, 1, param_ptr->dsc_value );

        /* Write the DSC Length to mDSP register HDR_dscLength */
        //HDRMDSP_SET_DSC_LENGTH( param_ptr->dsc_length );

        /* Set DSC channel gain w.r.t the pilot.
           Signed with 0.5 dB resolution */
        //HDRMDSP_SET_DSC_CHAN_GAIN_MC( HDRMDSP_DEMOD_CARRIER_0, 
        //                              ((int16)(param_ptr->dsc_ch_gain*2)) );
      }

      if( param_ptr->force_early_terminate )
      {
        //HDRMDSP_ENABLE_REV_A_RL_FORCED_TERMINATION(
        //  param_ptr->force_early_terminate );
      }
    }

    HDR_MSG_HIT( MSG_LEGACY_MED, \
      "RL Configuration complete" );

    return HDRHITCMD_COMMAND_SUCCESS;
  }
  else if (!hdrhitcmd_acquired) /* Not acquired */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, "RL Config Failed: Not Acquired" );

    return HDRHITCMD_COMMAND_FAILED_NOT_ACQ;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_rl_config */


/*===========================================================================
FUNCTION HDRHITCMD_RTCMAC_CONFIG

DESCRIPTION
  HIT command for configuring RTCMAC
  
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_rtcmac_config
(
  hdrhitcmd_rtcmac_config_param_type* param_ptr
)
{
  int i;

  hdrrmac_cmd_union_type rmac_cmd;

#ifdef FEATURE_FACTORY_TESTMODE
  hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type = param_ptr->rev_mode;
#endif /* FEATURE_FACTORY_TESTMODE */

  /* Reset frame count */
  hdrhitcmd_rtcmac.frame_count = 0;

  if (hdr_hit_enable && hdrhitcmd_acquired)
  {
    /* Deactivate RMAC - necessary if the Rev mode changes */
    rmac_cmd.hdr.cmd = HDRRMAC_DEACTIVATE_CMD;
    rmac_cmd.hdr.sender = HDRHAI_PHYSICAL_LAYER_PROTOCOL;
    hdrrmac_process_cmd(&rmac_cmd);

    /* Reactivate RMAC - new Rev mode assigned */
    rmac_cmd.hdr.cmd = HDRRMAC_ACTIVATE_CMD;
    rmac_cmd.hdr.sender = HDRHAI_PHYSICAL_LAYER_PROTOCOL;
    hdrrmac_process_cmd(&rmac_cmd);

    hdrhitcmd_rtcmac.rev_mode = param_ptr->rev_mode;

    hdrhitcmd_rtcmac.frames_to_send = param_ptr->num_data_frames;

    if ( param_ptr->rev_mode == 0x0) /* Rev 0 specific configuration */
    {
      //HDRAGC_SET_DATA_OFFSET(1, HDRHITCMD_RMAC0_DATA_NOM_GAIN_9K6
      //                       + (param_ptr->data_offset_nom * 2)
      //                       + param_ptr->data_offset_9k6 );

      //HDRAGC_SET_DATA_OFFSET(2, HDRHITCMD_RMAC0_DATA_NOM_GAIN_19K2
      //                       + (param_ptr->data_offset_nom * 2)
      //                       + param_ptr->data_offset_19k2 );

      //HDRAGC_SET_DATA_OFFSET(3, HDRHITCMD_RMAC0_DATA_NOM_GAIN_38K4
      //                       + (param_ptr->data_offset_nom * 2)
      //                       + param_ptr->data_offset_38k4 );

      //HDRAGC_SET_DATA_OFFSET(4, HDRHITCMD_RMAC0_DATA_NOM_GAIN_76K8
      //                       + (param_ptr->data_offset_nom * 2)
      //                       + param_ptr->data_offset_76k8 );

      //HDRAGC_SET_DATA_OFFSET(5, HDRHITCMD_RMAC0_DATA_NOM_GAIN_153K6
      //                       + (param_ptr->data_offset_nom * 2)
      //                       + param_ptr->data_offset_153k6 );

    }
    else if ( param_ptr->rev_mode == 0x1) /* Rev A specific configuration */
    {
      for (i=0; i<12; i++)
      {
        hdrhitcmd_rtcmac.pre_transition_gains[i] = \
          param_ptr->pre_transition_gains[i];
        hdrhitcmd_rtcmac.post_transition_gains[i] = \
          param_ptr->post_transition_gains[i];
        hdrhitcmd_rtcmac.gain_transition_points[i] = \
          param_ptr->gain_transition_points[i];
      }

      for (i=0; i<4; i++)
      {
        hdrhitcmd_rtcmac.pre_rri_gains[i] = \
          param_ptr->rri_ch_gain_pre_transition[i];
        hdrhitcmd_rtcmac.post_rri_gains[i] = \
          param_ptr->rri_ch_gain_post_transition[i];
      }

      if (param_ptr->aux_pilot_min_payload < 13) 
      {
        //HDRMDSP_SET_AUX_PILOT_GAIN_MC(HDRMDSP_DEMOD_CARRIER_0, 
        //                              param_ptr->aux_pilot_ch_gain*4);

        //HDRMDSP_SET_AUX_PILOT_MIN_PAYLOAD(param_ptr->aux_pilot_min_payload+1);
      }

      hdrhitcmd_rtcmac.tm = param_ptr->transmission_mode;

#ifdef FEATURE_FACTORY_TESTMODE
      hdrhitmsg_get_rmac_params_ptr()->auxpil_payload_thresh = param_ptr->aux_pilot_min_payload;
      hdrhitmsg_get_rmac_params_ptr()->auxpil_chan_gain = param_ptr->aux_pilot_ch_gain;
      hdrhitmsg_get_rmac_params_ptr()->hicap_lolatency = param_ptr->transmission_mode;
#else
      HDR_MSG_HIT( MSG_LEGACY_ERROR, 
                   "RTCMAC Config Failed: Enable FTM mode for Rev A" );

#endif /* FEATURE_FACTORY_TESTMODE */

    }
    else
    {
      HDR_MSG_HIT( MSG_LEGACY_MED, \
        "RTCMAC Config: Invalid Rev Mode" );

      return HDRHITCMD_COMMAND_FAILED;
    }

    HDR_MSG_HIT( MSG_LEGACY_MED, \
      "RTCMAC Configuration complete" );

    return HDRHITCMD_COMMAND_SUCCESS;

  }
  else if (!hdrhitcmd_acquired) /* Not acquired */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, "RTCMAC Config Failed: Not Acquired" );

    return HDRHITCMD_COMMAND_FAILED_NOT_ACQ;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_rtcmac_config */


/*===========================================================================
FUNCTION HDRHITCMD_RL_ISR 

DESCRIPTION
  Reverse Link ISR - sends Rev 0 frames to encoder

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrhitcmd_rl_isr 
( 
  uint32 unused_param
    /* Parameter */
)
{
  uint16 i;
  uint8  current_rate;
  static uint8 next_rate = 0;

  unused_param;
    /* To avoid compiler warning */

  if ( hdrhitcmd_rtcmac.frame_count < hdrhitcmd_rtcmac.frames_to_send )
  {
    hdrhitcmd_rl_packet[0] = hdrhitcmd_rtcmac.frame_count & 0xff;

    if ( hdrhitcmd_rtcmac.rate_mode != 0 )
    {
      current_rate = hdrhitcmd_rtcmac.data_rate[next_rate];
      next_rate++;
      if ( next_rate >= hdrhitcmd_rtcmac.rate_mode )
      {
        next_rate = 0;
      }
    }
    else
    {
      current_rate = 1;
    }

    if (hdrhitcmd_rtcmac.rev_mode == 0x0) /* Rel 0 */
    {
      for ( i=1; i<hdrhitcmd_rel0_pkt_sizes[current_rate]; i++ )
      {
        hdrhitcmd_rl_packet[i] = hdrhitcmd_rtcmac.data_pattern & 0xff;
      }

      if ((current_rate > 0) && (current_rate <= 5))
      {
        hdrenc_send_frame( hdrhitcmd_rl_packet, 
                           hdrhitcmd_rel0_pkt_sizes[current_rate], 
                           0, 
                           current_rate );

        HDR_MSG_HIT_2( MSG_LEGACY_MED, \
          "hdrhit: Frame Count: %d, Current Rate: %d", \
          hdrhitcmd_rtcmac.frame_count, current_rate );
      }
      else /* current_rate not in range */
      {
        HDR_MSG_HIT( MSG_LEGACY_HIGH, 
          "No pkt loaded this subframe: 0 < current_rate <= 5 not satisfied" );

        HDR_MSG_HIT_1( MSG_LEGACY_HIGH, "Current Rate: %d", current_rate );

      }
    }
    else /* The ISR is not called for Rev A => Invalid Rev mode! */
    {
      HDR_MSG_HIT( MSG_LEGACY_ERROR, "Invalid Rev_Mode!" );

      /* Disable frame ISR*/
      hdrmod_disable_frame_isr();

      /* Disable TX*/
      hdrtx_disable_tx( FALSE );

      /* Mark RMAC active flag as FALSE */
      hdrhitcmd_rtcmac.rtcmac_active = FALSE;

    }

  }
  else if (hdrhitcmd_rtcmac.frame_count == hdrhitcmd_rtcmac.frames_to_send)
  {
    HDR_MSG_HIT_1( MSG_LEGACY_MED, \
      "Done sending %d encoder packets", \
      hdrhitcmd_rtcmac.frames_to_send );

    /* Disable frame ISR*/
    hdrmod_disable_frame_isr();

    /* Disable TX*/
    hdrtx_disable_tx( FALSE );

    /* Mark RMAC active flag as FALSE */
    hdrhitcmd_rtcmac.rtcmac_active = FALSE;

  }
  else
  {
    /* Reset frame count */
    hdrhitcmd_rtcmac.frame_count = 0xFFFFFFFF;

    HDR_MSG_HIT( MSG_LEGACY_HIGH, \
                 "Frame Count > Frames To Send: Frame Count Reset");

  }

  hdrhitcmd_rtcmac.frame_count++;

  /* Add logging */

} /* hdrhitcmd_rl_isr */


/*===========================================================================
FUNCTION HDRHITCMD_TX_ENABLED_CALLBACK

DESCRIPTION
  Callback for RF TX enable power. Sets a signal to indicate to PLT
  that it may proceed..

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrhitcmd_tx_enabled_callback( hdrtx_rf_status_enum_type rf_status )
{
  HDR_MSG_HIT( MSG_LEGACY_MED, "Tx Enabled" );

  if (hdrhitcmd_rtcmac.rev_mode == 0x0) /* Rel 0 */
  {
    /* Install the access channel ISR */
    hdrmod_enable_frame_isr( hdrhitcmd_rl_isr );

    /* Mark RMAC active flag */
    hdrhitcmd_rtcmac.rtcmac_active = TRUE;

    /* Set the modulator Reverse Rate Indicator for pilot-only */
    hdrmod_enable_pa();

    /* PA Status */
    HDR_MSG_HIT_1( MSG_LEGACY_MED, 
                   "PA ON Status...%x", 
                   hdrmod_pa_is_on() );

    /* DRC transmitted on Fwd and Rev links */
    HDR_MSG_HIT_2( MSG_LEGACY_MED, 
                   "Fwd Link DRC: %d, Rev Link DRC: %d", 
                   HDRDRC_GET_DRC_FL(HDRMDSP_DEMOD_CARRIER_0),
                   HDRDRC_GET_DRC_RL_MC(HDRMDSP_DEMOD_CARRIER_0) );

  }
  else /* This should never happen */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, 
                 "hdrhitcmd_tx_enabled_callback: Illegal Rev Mode!!!" );
  }

  hdrhitcmd_mod_rl_callback();

} /* hdrhitcmd_tx_enabled_callback */


/*===========================================================================
FUNCTION HDRHITCMD_MOD_RL

DESCRIPTION
  This procedure processes a HIT command for enabling the reverse link
   
DEPENDENCIES
  To be called after hdrhitcmd_RL_CONFIG and hdrhitcmd_RTCMAC_CONFIG

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_mod_rl
(
  hdrhitcmd_mod_rl_param_type*   param_ptr,
  hdrhitcmd_mod_rl_callback_type hdrhitcmd_mod_rl_cb
)
{
  hdrrmac_cmd_union_type rmac_cmd;

  if (hdr_hit_enable && hdrhitcmd_acquired)
  {
    hdrhitcmd_rtcmac.rate_mode = param_ptr->data_rate_mode;

    hdrhitcmd_rtcmac.data_pattern = param_ptr->specified_data_pattern;

    hdrhitcmd_rtcmac.data_rate[0] = 
      (param_ptr->specified_rate & 0xff);
    hdrhitcmd_rtcmac.data_rate[1] = 
      (param_ptr->specified_rate & 0xff00) >> 8;
    hdrhitcmd_rtcmac.data_rate[2] = 
      (param_ptr->specified_rate & 0xff0000) >> 16;
    hdrhitcmd_rtcmac.data_rate[3] = 
      (param_ptr->specified_rate & 0xff000000) >> 24;

#ifdef FEATURE_FACTORY_TESTMODE
    hdrhitmsg_get_rmac_params_ptr()->rtc_pattern = param_ptr->specified_data_pattern;
    hdrhitmsg_get_rmac_params_ptr()->tx_rri_reva = (param_ptr->specified_rate & 0xff000000) >> 24;
    hdrhitmsg_get_rmac_params_ptr()->tx_payload_reva = 
      hdrhitcmd_revA_pkt_sizes[hdrhitmsg_get_rmac_params_ptr()->tx_rri_reva];
    hdrhitmsg_get_rmac_params_ptr()->gain_transition_point = 
      hdrhitcmd_rtcmac.gain_transition_points[hdrhitmsg_get_rmac_params_ptr()->tx_rri_reva];
    hdrhitmsg_get_rmac_params_ptr()->t2p_pre_transition = 
      hdrhitcmd_rtcmac.pre_transition_gains[hdrhitmsg_get_rmac_params_ptr()->tx_rri_reva];
    hdrhitmsg_get_rmac_params_ptr()->t2p_post_transition = 
      hdrhitcmd_rtcmac.post_transition_gains[hdrhitmsg_get_rmac_params_ptr()->tx_rri_reva];
    hdrhitmsg_get_rmac_params_ptr()->rri_ch_gain_pre_transition = 
      hdrhitcmd_rtcmac.pre_rri_gains[hdrhitmsg_get_rmac_params_ptr()->tx_rri_reva];
    hdrhitmsg_get_rmac_params_ptr()->rri_ch_gain_post_transition = 
      hdrhitcmd_rtcmac.post_rri_gains[hdrhitmsg_get_rmac_params_ptr()->tx_rri_reva];
#endif /* FEATURE_FACTORY_TESTMODE */

    hdrhitcmd_mod_rl_callback = hdrhitcmd_mod_rl_cb;

    if (hdrhitcmd_rtcmac.rtcmac_active)
    {
      HDR_MSG_HIT( MSG_LEGACY_ERROR, 
                   "RTCMAC already active!! Deactivating RMAC" );

      hdrhitcmd_deactivate_rtcmac( NULL );

      return HDRHITCMD_COMMAND_FAILED;

    }
    else
    {
      if (hdrhitcmd_rtcmac.rev_mode == 0x1) /* Rev A */
      {
        rmac_cmd.hdr.cmd = HDRRMAC_START_TX_CMD;
        rmac_cmd.hdr.sender = HDRHAI_PHYSICAL_LAYER_PROTOCOL;
        hdrrmac_process_cmd(&rmac_cmd);

        /* Starting RMAC3 */
        HDR_MSG_HIT( MSG_LEGACY_MED, \
            "Starting RMAC3...." );

        /* Mark RMAC active flag */
        hdrhitcmd_rtcmac.rtcmac_active = TRUE;

      }
      else if (hdrhitcmd_rtcmac.rev_mode == 0x0) /* Rel 0 */
      {
        /* Enable RF TX power */
        hdrtx_enable_tx( hdrhitcmd_tx_enabled_callback );

      }
      else
      {
        HDR_MSG_HIT( MSG_LEGACY_ERROR, 
                     "hdrhitcmd_mod_rl: Illegal Rev Mode!!!" );
      }

      return HDRHITCMD_COMMAND_ISSUED;

    }
    
  }
  else if (!hdrhitcmd_acquired) /* Not acquired */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, "Mod RL Failed: Not Acquired" );

    return HDRHITCMD_COMMAND_FAILED_NOT_ACQ;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_mod_rl */


/*===========================================================================
FUNCTION HDRHITCMD_DEACTIVATE_RTCMAC

DESCRIPTION
  HIT command for deactivating RTCMAC
  
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_deactivate_rtcmac
(
  hdrhitcmd_deactivate_rtcmac_param_type* param_ptr
)
{
  hdrrmac_cmd_union_type rmac_cmd;

  if (hdr_hit_enable && hdrhitcmd_acquired)
  {
    if ( hdrhitcmd_rtcmac.rev_mode == 0x0 ) /* Rev 0 */
    {
      HDR_MSG_HIT( MSG_LEGACY_MED, "Deactivating RTCMAC: Rev 0");

      /* Disable frame ISR*/
      hdrmod_disable_frame_isr();

    }
    else if ( hdrhitcmd_rtcmac.rev_mode == 0x1 ) /* Rev A */
    {
      HDR_MSG_HIT( MSG_LEGACY_MED, "Deactivating RTCMAC: Rev A");

      rmac_cmd.hdr.cmd = HDRRMAC_STOP_TX_CMD;
      rmac_cmd.hdr.sender = HDRHAI_PHYSICAL_LAYER_PROTOCOL;
      hdrrmac_process_cmd(&rmac_cmd);
    }
    else
    {
      HDR_MSG_HIT( MSG_LEGACY_ERROR, 
                   "hdrhitcmd_deactivate_rtcmac: Illegal Rev Mode!!!" );

      return HDRHITCMD_COMMAND_FAILED;
    }

    /* Disable TX*/
    hdrtx_disable_tx( FALSE );

    /* Reset RMAC active flag */
    hdrhitcmd_rtcmac.rtcmac_active = FALSE;

    /* Reset frame count */
    hdrhitcmd_rtcmac.frame_count = 0;

    return HDRHITCMD_COMMAND_SUCCESS;

  }
  else if (!hdrhitcmd_acquired) /* Not acquired */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, 
                 "Deactivate RTCMAC Failed: Not Acquired" );

    return HDRHITCMD_COMMAND_FAILED_NOT_ACQ;
  }

  return HDRHITCMD_COMMAND_FAILED;
}


/*===========================================================================
FUNCTION HDRHITCMD_FLMAC_CONFIG

DESCRIPTION
  This procedure processes a HIT command for FL MAC configuration.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_flmac_config
(
  hdrhitcmd_flmac_config_param_type* param_ptr
)
{
  if (hdr_hit_enable && hdrhitcmd_acquired)
  {
    if(param_ptr->enable_short_pkts == 0x00 )
    {
      //HDRMDSP_DISABLE_SHORT_PKTS();
    }
    else
    {
      //HDRMDSP_ENABLE_SHORT_PKTS();
    }

    if(param_ptr->enable_multiuser_pkts == 0x00 )
    {
      //HDRMDSP_DISABLE_MU_PKTS();
    }
    else
    {
      //HDRMDSP_ENABLE_MU_PKTS();
    }

    if(param_ptr->enable_RAB_demod == 0x00 )
    {
      //HDRMDSP_FREEZE_RAB_MC(HDRMDSP_DEMOD_CARRIER_0);
    }
    else
    {
      //HDRMDSP_UNFREEZE_RAB_MC(HDRMDSP_DEMOD_CARRIER_0);
    }

    if (param_ptr->enable_DRCLock_demod == 0x00)
    {
      //HDRDRC_DISABLE_DRC_MC(HDRMDSP_DEMOD_CARRIER_0);
    }
    else
    {
      //HDRDRC_ENABLE_DRC_MC(HDRMDSP_DEMOD_CARRIER_0);
    }

    /*(param_ptr->enable_FLARQ_demod == 0x00 ) */

    HDR_MSG_HIT( MSG_LEGACY_MED, "FL MAC configuration complete" );

    return HDRHITCMD_COMMAND_SUCCESS;
  }
  else if (!hdrhitcmd_acquired) /* Not acquired */
  {
    HDR_MSG_HIT( MSG_LEGACY_ERROR, "FLMAC Config Failed: Not Acquired" );

    return HDRHITCMD_COMMAND_FAILED_NOT_ACQ;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_flmac_config */


/*===========================================================================
FUNCTION HDRHITCMD_AMAC_CONFIG

DESCRIPTION
  This procedure processes a HIT command for configuring Access Channel
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_amac_config
(
  hdrhitcmd_amac_config_param_type* param_ptr
)
{
  hdramac_override_config_struct_type override_config;

  if (hdr_hit_enable)
  {
    override_config.ap_msg_override.access_cycle_duration  = 
      param_ptr->access_cycle_duration;
    override_config.ap_msg_override.probe_init_adjust      = 
      param_ptr->probe_initial_adjust;
    override_config.ap_msg_override.nominal_pwr            = 
      param_ptr->open_loop_adjust;
    override_config.ap_msg_override.power_step             = 
      param_ptr->power_step;
    override_config.ap_msg_override.capsule_length_max     = 
      param_ptr->capsule_length_max;
    override_config.ap_msg_override.preamble_len           = 
      param_ptr->preamble_len;
    override_config.ap_msg_override.probe_num_step         = 
      param_ptr->probe_num_step;
    override_config.ap_msg_override.enhanced_access_parameters_included = 
      param_ptr->enable_enhanced_access;
    override_config.ap_msg_override.preamble_length_slots  = 
      param_ptr->preamble_len_slots;
    override_config.ap_msg_override.access_offset          = 
      param_ptr->access_offset;
    override_config.ap_msg_override.sector_access_max_rate = 
      param_ptr->sector_access_max_rate;

    override_config.ap_msg_override.apersistence[0]               = 0;
    override_config.ap_msg_override.probe_time_out_adjust         = 0;
    override_config.ap_msg_override.pilot_strength_nominal        = 0;
    override_config.ap_msg_override.pilot_strength_correction_min = 0;
    override_config.ap_msg_override.pilot_strength_correction_max = 0;

    override_config.test_access_rate = 
      param_ptr->test_access_rate;
    override_config.data_bytes       = 
      param_ptr->data_bytes;


    hdramac_override_config( &override_config );

    HDR_MSG_HIT( MSG_LEGACY_MED, "Access Channel configuration complete" );

    return HDRHITCMD_COMMAND_SUCCESS;

  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_amac_config */


/*===========================================================================
FUNCTION HDRHITCMD_AMAC

DESCRIPTION
  This procedure processes a HIT command for Access Channel operations
  - HDRAMAC_DEACTIVATE_CMD,
  - HDRAMAC_ACTIVATE_CMD,
  - HDRAMAC_STOP_CMD,
  - HDRAMAC_START_ATTEMPT_CMD,
  - HDRAMAC_RESET_ATTEMPT_CMD,
  - HDRAMAC_TUNE_AWAY_TIME_CMD,
  - HDRAMAC_TUNE_AWAY_STOP_TX_CMD
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_amac
(
  hdrhitcmd_amac_param_type* param_ptr
)
{
  hdramac_cmd_union_type amac_cmd_shell;

  if (hdr_hit_enable)
  {
    amac_cmd_shell.hdr.cmd = (hdramac_cmd_enum_type) (param_ptr->amac_cmd);
    amac_cmd_shell.hdr.sender = (hdrhai_protocol_name_enum_type) (0); 
    /* 0 => Cmd sent from Physical Layer Protocol */

    hdramac_process_cmd(&amac_cmd_shell);

    HDR_MSG_HIT_1( MSG_LEGACY_MED, \
      "AMAC command: %d issued", param_ptr->amac_cmd );

    return HDRHITCMD_COMMAND_SUCCESS;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_amac */


/*===========================================================================
FUNCTION HDRHITCMD_ROTATOR_CONFIG

DESCRIPTION
  This procedure processes a HIT command for rotator configuration.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_rotator_config
(
  hdrhitcmd_rotator_config_param_type* param_ptr
)
{
  if (hdr_hit_enable)
  {
    hdrsrchafc_override_nv( param_ptr->rot_config );

    HDR_MSG_HIT_1( MSG_LEGACY_MED, \
      "Rotator config: %d applied", param_ptr->rot_config );

    return HDRHITCMD_COMMAND_SUCCESS;
  }

  return HDRHITCMD_COMMAND_FAILED;

} /* hdrhitcmd_rotator_config */


#endif /* FEATURE_HDR_HIT */


/*===========================================================================
FUNCTION HDRHITCMD_START_RL_PILOT

DESCRIPTION
  This function enables reverse link pilot transmission.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_start_rl_pilot( void )
{
  qword           i_mask,q_mask;
  uint8           i;
  uint32          i_shift, i_mask_32, xor_result; 
  hdrfw_msg_u     fw_cmd;      /* Firmware command */
  errno_enum_type estatus;     /* Error status */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrmod_reset();
    /* Reset the modulator when the pilot channel is turned on. */

#if !( defined T_MSM8960 || defined FEATURE_HDR_DIME_MODEM )
  /* Turn ON modem clock */
  HWIO_OUT( MOD_MICRO_CLK_CTL, 0x0F );
#endif

  qw_set(i_mask , 0, 0);

  /* Construct Q mask */
  /* First, figure out what bit 0 should be. */
  xor_result = 0;
  for ( i=0; i<HDRRMAC_XOR_LEN; i++ )
  {
    i_shift = hdrhitcmd_q_xor_array[i];

    if (i_shift > 31 )
    {
      i_mask_32 = qw_hi(i_mask);
      i_shift-=32;
    } else
    {
      i_mask_32 = qw_lo(i_mask);
    }

    xor_result ^= ((i_mask_32 >> i_shift) & 0x00000001);
  }

  /* Set Q to I, shifted by one bit to the left. */
  qw_set(q_mask, qw_hi(i_mask), qw_lo(i_mask));

  qw_shift(q_mask, 1);

  /* Add in last bit. */
  qw_set(q_mask, qw_hi(q_mask) & 0x3ff, (qw_lo(q_mask) | xor_result));

  HDR_MSG_HIT_2( MSG_LEGACY_MED, \
    "I_MASK_U = %x, I_MASK_L = %x", i_mask[1], i_mask[0] );
  HDR_MSG_HIT_2( MSG_LEGACY_MED, \
    "Q_MASK_U = %x, Q_MASK_L = %x", q_mask[1], q_mask[0] );

  /* Program the user long code mask into the modulator */
  hdrmod_set_pn_long_code_mask( i_mask, q_mask
#ifdef FEATURE_HDR_BOLT_MODEM
    , 1                /* Enable carrier 0 PN generator */
#endif /* FEATURE_HDR_BOLT_MODEM */
    );

#ifndef FEATURE_HDR_TRITON_MODEM
#ifndef FEATURE_HDR_BOLT_MODEM
  HWIO_OUT(MOD_MODE, 1); 
#endif /* FEATURE_HDR_BOLT_MODEM */
#endif /* FEATURE_HDR_TRITON_MODEM */

  /* Set the modulator Reverse Rate Indicator for pilot-only */
  hdrmod_enable_pa();

  /* PA Status */
  HDR_MSG_HIT_1( MSG_LEGACY_MED, 
                 "PA ON Status...%x", 
                 hdrmod_pa_is_on() );

  /* Initialize message for firmware carrier enable command */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_AMAC, HDR_FW_RMAC_ENABLE_CARRIER_CMD );

  /* Enable carrier 0 in mDSP */
  fw_cmd.rmac_enable_carrier.carrierMask = 1;
  fw_cmd.rmac_enable_carrier.modulatorConfig[0].enable = 1;
  fw_cmd.rmac_enable_carrier.modulatorConfig[0].carrierMask = 1;

  /* Send RL config message */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  //ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */

  return HDRHITCMD_COMMAND_SUCCESS;

} /* hdrhitcmd_start_rl_pilot */

/*===========================================================================
FUNCTION HDRHITCMD_FMAC_ACTIVATE_MSG

DESCRIPTION
  HIT command for configuring:
  DRCLOCK LENGTH
  DRCLOCK_PERIOD
  hdrSystemModeConfig
  MAX DRC
  hdrShortPacketMode
  preamMUPEnable  
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_fmac_activate_msg
(
  hdrhitcmd_system_mode_type hdrSystemModeConfig,
   /* System mode.
    0: Release 0
    1: Release A
    2: Release B
   */
  uint16 drcLockLength,
    /* The number of times a DRCLock bit is repeated. Must be a power of 2. */ 
  uint16 drcLockPeriod,
    /* Maximum number of credits allowed by the handoff algorithm. Default 128.
       Note: it must be greater than or equal to:
       max(aspnSoftHandoffDelay,aspnSofterHandoffDelay) */
  uint16 maxDRC 
    /*To set the max drc*/
)
{
  hdrfw_msg_u fw_cmd;
    /* Firmware command  */

  errno_enum_type estatus;
    /* Error status */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset ( &fw_cmd, 0, sizeof (hdrfw_msg_u) );
  msgr_init_hdr (
      &fw_cmd.hdr, MSGR_HDR_HIT, HDR_FW_FMAC_ACTIVATE_CMD );


  fw_cmd.fmac_activate.hdrSystemModeConfig = (uint16) hdrSystemModeConfig;
     
  fw_cmd.fmac_activate.softHandoffCreditThresh = 
      HDRFMACUTIL_SOFT_HO_CREDIT_THRESHOLD;
      /* Set Soft Handoff Credit Threshold */

  fw_cmd.fmac_activate.softerHandoffCreditThresh = 
      HDRFMACUTIL_SOFTER_HO_CREDIT_THRESHOLD;
      /* Set Softer Handoff Credit Threshold */

  fw_cmd.fmac_activate.maxCredit = HDRFMACUTIL_MAX_CREDIT;
      /* Program Max credit using soft handoff delay */

  fw_cmd.fmac_activate.softHandoffDelayTimer = 
      HDRFMAC_DEFAULT_FMAC_HO_DELAY_TIMER;
      /* Set SoftHandoffDelayTimer */

  fw_cmd.fmac_activate.softerHandoffDelayTimer = 
      HDRFMAC_DEFAULT_FMAC_HO_DELAY_TIMER;
      /* Set SofterHandoffDelayTimer */

  fw_cmd.fmac_activate.drcLockBackOffLowSinr = 
      HDRFMAC_DRC_LOCK_BACKOFF_20_DB_V;
      /* Set low SINR threshold for drcLock backoff in Rel 0. */

  fw_cmd.fmac_activate.drcLockLength = drcLockLength;

  fw_cmd.fmac_activate.drcLockPeriod = drcLockPeriod;

  fw_cmd.fmac_activate.nFTCMPRestartTx = 
      HDRFMAC_DRC_SLOTS_NEEDED_FOR_RESTART; 
      /* Set DRC supervision timers. Start Tx after 16 good slots. */

  fw_cmd.fmac_activate.nFTCMDRCSupervision = 
      HDRFMAC_DRC_SUPERVISION_SLOTS;
      /* Stop Tx after 144 bad slots */
       
  fw_cmd.fmac_activate.nullRateDRC384 = 
      HDRSCMFMAC_DEFAULT_NULL_RATE_DRC_384_ENABLED;

  fw_cmd.fmac_activate.enableRCL = 0xC000;
        /* Turn on RCL-A+ for Rev A calls */

  fw_cmd.fmac_activate.maxDRC = maxDRC;
     
  estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
     /* Send the message */

  //ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */

  return HDRHITCMD_COMMAND_SUCCESS;
  
} /* hdrhitcmd_fmac_activate_msg */

/*===========================================================================
FUNCTION HDRHITCMD_FMAC_ENHANCED_ACTIVATE_MSG

DESCRIPTION
  HIT command for configuring:
  DRCLOCK LENGTH
  hdrSystemModeConfig
  MAX DRC
  hdrShortPacketMode
  preamMUPEnable  
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_fmac_enahnced_activate_msg
(
  hdrhitcmd_system_mode_type hdrSystemModeConfig,
    /* System mode.
     0: Release 0
     1: Release A
     2: Release B
    */
  uint16 drcLockLength,
    /* The number of times a DRCLock bit is repeated. Must be a power of 2. */ 
  uint16 maxDRC,
    /*To set the max drc*/
  boolean hdrShortPacketMode,
    /*Enable/diisable short packet mode*/
  boolean preamMUPEnable
)
{
  hdrfw_msg_u fw_cmd;
    /* Firmware command  */

  errno_enum_type estatus;
    /* Error status */

  uint8 drc_index;
    /* DRC values from 1 through 14 used as array index */

  uint8 translated_drc;
    /* Translated DRC */

  #ifdef FEATURE_HDR_REVB
  static uint8 const tx_drc_map_default[(HDRHAI_MAX_NUM_REL_B_DRCS-2)] =
  {
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_01,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_02,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_03,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_04,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_05,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_06,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_07,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_08,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_09,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0A,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0B,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0C,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0D,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_0E,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_10,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_11,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_12,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_13,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_14,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_15,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_16,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_17,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_18,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_19,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_1A,
    HDRSCMFMAC_DEFAULT_TX_DRC_MAP_1B
  };
#endif /* FEATURE_HDR_REVB */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset ( &fw_cmd, 0, sizeof (hdrfw_msg_u) );
  msgr_init_hdr (
      &fw_cmd.hdr, MSGR_HDR_HIT, HDR_FW_FMAC_ACTIVATE_CMD );

  fw_cmd.fmac_activate.hdrSystemModeConfig = (uint16) hdrSystemModeConfig;
   
  fw_cmd.fmac_activate.drcLockLength = drcLockLength;

  fw_cmd.fmac_activate.nFTCMPRestartTx = 
    HDRFMAC_DRC_SLOTS_NEEDED_FOR_RESTART;
    /* Stop Tx after 144 bad slots */

  fw_cmd.fmac_activate.nFTCMDRCSupervision = 
    HDRFMAC_DRC_SUPERVISION_SLOTS;
    /* Stop Tx after (144 + drc_supervision_timer x 6) bad slots */

  fw_cmd.fmac_activate.hdrShortPacketMode = (uint16) hdrShortPacketMode;  

  fw_cmd.fmac_activate.preamMUPEnable = (uint16) preamMUPEnable; 

  /* Compute the translated DRC from the DRC translateion offset and
     write to mDSP registers */
    
  for ( drc_index = 0; drc_index < (HDRHAI_MAX_NUM_REL_B_DRCS-2); ++drc_index )
  {
      translated_drc = tx_drc_map_default[drc_index];

      fw_cmd.fmac_activate.drcTranslation[drc_index] = translated_drc;
  }

  fw_cmd.fmac_activate.minContinuationSpan =
      HDRSCMFMAC_DEFAULT_MIN_CONTN_SPAN;
    /* Set MinContinuationSpan in subpackets(slots) */

  /* Set MUP Decode filter Dip switch from last requested choice */
  fw_cmd.fmac_activate.disableMUPDecodeFilter = FALSE;

  /* Write DRC/DSCBoostLength to zero for initial call setup. */
  fw_cmd.fmac_activate.drcBoostLength = 0;
  fw_cmd.fmac_activate.dscBoostLength = 0;

  fw_cmd.fmac_activate.softHandoffDelayTimer = 
    HDRFMAC_DEFAULT_FMAC_HO_DELAY_TIMER;
    /* SoftHandoff Delay timer */

  fw_cmd.fmac_activate.softerHandoffDelayTimer = 
    HDRFMAC_DEFAULT_FMAC_HO_DELAY_TIMER;
    /* Softer Handoff Delay timer */

  fw_cmd.fmac_activate.maxCredit = HDRFMACUTIL_MAX_CREDIT;
    /* Max credit */

  fw_cmd.fmac_activate.softHandoffCreditThresh = 
    HDRFMACUTIL_SOFT_HO_CREDIT_THRESHOLD;
    /* Soft Handoff Credit Threshold */

  fw_cmd.fmac_activate.softerHandoffCreditThresh = 
    HDRFMACUTIL_SOFTER_HO_CREDIT_THRESHOLD;
    /* Softer Handoff Credit Threshold */

  fw_cmd.fmac_activate.drcLockBackOffLowSinr = HDRFMAC_DRC_LOCK_BACKOFF_6_DB_V;
    /* Set Low SINR threshold for drcLock Backoff. Use 6 dB for Rev A.  */

  fw_cmd.fmac_activate.maxOptionalDataRate = 0;
      /* Set MaxOptionalDataRate to force Rev-A Max rate in non-RevB Mode */

  estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
     /* Send the message */

  //ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */

  return HDRHITCMD_COMMAND_SUCCESS;
  
} /* hdrhitcmd_fmac_enhanced_activate_msg */

/*===========================================================================
FUNCTION HDRHITCMD_SET_DISABLE_EARLY_DECODE

DESCRIPTION
  Disables or Enables DSP's early decode mode.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_disable_early_decode
(
  boolean disableEarlyDecode
)
{
  hdrfw_msg_u fw_cmd;
    /* Firmware command  */

  errno_enum_type estatus;
    /* Error status */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msgr_init_hdr (&fw_cmd.hdr, MSGR_HDR_HIT, HDR_FW_DECODE_CTL_CMD );

  fw_cmd.decode_ctl.cancelActivePackets = 0; 
   /* Cancel any active packets. Includes clearning BCC detect also */

  fw_cmd.decode_ctl.singleSlotDecode_Valid = 0;

  fw_cmd.decode_ctl.disableEarlyDecode_Valid = 1 ;

  fw_cmd.decode_ctl.disableEarlyDecode = (uint32) disableEarlyDecode;

  estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
    /* Send the message */

  //ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */
      
  return HDRHITCMD_COMMAND_SUCCESS;

} /* hdrhitcmd_disable_early_decode */

/*===========================================================================
FUNCTION HDRHITCMD_DRC_CFG

DESCRIPTION
  This function sets the DRC parameters:
  enableDRC
  predictor control
  drcDict Values
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_drc_cfg 
(
  boolean drcOverride_Valid,
    /*if the drcOverride valus is valid*/
  boolean drcOverride,
    /*sets the predictor control*/
  uint8 dictDRC[3],
    /*predC[c].DictDRC*/
  boolean enableDRC_Valid,
    /*if the enableDRC values is valid*/
  boolean enableDRC,
    /*enable DRC*/
  int8 num_carriers
    /*Carrier id*/
)
{
  hdrfw_drc_cfg_msg_t               drc_cfg_msg;
    /* DRC config message */

  errno_enum_type estatus;
    /* Error status */

  uint8 i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset( &drc_cfg_msg, 0, sizeof( hdrfw_drc_cfg_msg_t ) );
    /* Clear the message */

  msgr_init_hdr( &drc_cfg_msg.hdr, MSGR_HDR_HIT, 
                 HDR_FW_DRC_CFG_CMD );
    /* Initialize the message header */

  if ( num_carriers > 3 )
  {
    num_carriers = 3;
  }

  for ( i = 0; i<num_carriers; i++ )
  {
    if ( enableDRC_Valid )
    {
      drc_cfg_msg.drcCfg[i].enableDRC = (int8) enableDRC;

      drc_cfg_msg.drcCfg[i].enableDRC_Valid = HDRSRCHMSG_EN_DRC_VALID;
       /* Set Real Variable Rate Mode where Predictor controls the DRC values */
    }

    if ( drcOverride_Valid && (int8) drcOverride == HDRSRCHMSG_DRC_PRED_DSP_CTRL)
    {
      drc_cfg_msg.drcCfg[i].drcOverride = HDRSRCHMSG_DRC_PRED_DSP_CTRL;
      drc_cfg_msg.drcCfg[i].drcOverride_Valid = HDRSRCHMSG_DRC_OVRD_VALID;
       /* Set Real Variable Rate Mode where Predictor controls the DRC values */
    }
    else if ( drcOverride_Valid && (int8) drcOverride == HDRSRCHMSG_DRC_PRED_MICRO_CTRL)
    {
      drc_cfg_msg.drcCfg[i].drcOverride = HDRSRCHMSG_DRC_PRED_MICRO_CTRL;
      drc_cfg_msg.drcCfg[i].drcOverride_Valid = HDRSRCHMSG_DRC_OVRD_VALID;
        /* DRC is uP Controlled */

      drc_cfg_msg.drcCfg[i].dictDRC = dictDRC[i];
      /* Set the dictated DRC value in the message */
    }
  }

  estatus = msgr_send ( &drc_cfg_msg.hdr,sizeof ( hdrfw_drc_cfg_msg_t ) );
  /* Send the DRC config message */

  //ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */
      
  return HDRHITCMD_COMMAND_SUCCESS;

} /* hdrhitcmd_drc_cfg */

/*===========================================================================
FUNCTION HDRHITCMD_MODULATOR_CFG

DESCRIPTION
  Sets Modulator configuration:
  Enable channel MC
  RPC Step Size
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_modulator_cfg 
(
  uint16 carrier,
  boolean enableChannelMC,
  boolean modulatorConfig_Valid,
  uint16 txGainAdjStep,
    /** Step size for closed-loop power control adjustment, Q9
     *  unsigned in dB. 1.5 dB: 768  1dB: 512  (Default) 0.5dB:
     *  256 */
  boolean txGainAdjStep_Valid
)
{
  hdrfw_msg_u fw_cmd_rl_cfg, fw_cmd_enable_carrier;
  /* Firmware commands */

  errno_enum_type estatus;
  /* Error status */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Sanity check - carrier id of carrier to be enabled should not exceed 
     the max supported carrier id. */
  ASSERT(carrier < 3);

   /* Initialize firmware commands */
  memset ( &fw_cmd_rl_cfg, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_rl_cfg.hdr, MSGR_HDR_HIT, HDR_FW_REL0_RL_CFG_CMD );

  memset ( &fw_cmd_enable_carrier, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_HIT, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD );

  /* Enable carrier in mDSP */
  if( modulatorConfig_Valid )
  {
      fw_cmd_enable_carrier.rmac_enable_carrier.carrierMask = 1 << carrier;
      fw_cmd_enable_carrier.rmac_enable_carrier.modulatorConfig[carrier].
        enable = (uint16) enableChannelMC;
      fw_cmd_enable_carrier.rmac_enable_carrier.modulatorConfig[carrier].
        carrierMask = ( 1 << carrier ); 

      /* Send message */
      estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof(hdrfw_msg_u) );
  }
  
  if( txGainAdjStep_Valid )
  {
      /* Step size for closed-loop power control adjustment, Q9 unsigned in dB.
       (Default)  0.5dB: 256 
                   1.5 dB: 768  
                      1dB: 512  
       In RPC_step field, 0 = .5 dB steps 1 = 1.0 dB steps. */
      fw_cmd_rl_cfg.rel0_rl_cfg.txGainAdjStep = ( ( txGainAdjStep ) * 256 );
      fw_cmd_rl_cfg.rel0_rl_cfg.fieldMask.txGainAdjStep_Valid = 1;

      /* Send message */
      estatus = msgr_send ( &fw_cmd_rl_cfg.hdr, sizeof(hdrfw_msg_u) );
  }

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */
 
  return HDRHITCMD_COMMAND_SUCCESS;

} /* hdrhitcmd_modulator_cfg */

/*===========================================================================
FUNCTION HDRHITCMD_HYPERSPACE_CFG

DESCRIPTION
  Sets hyperspace flag
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_hyperspace_cfg 
(
  boolean hyperspace_flag
)
{
  hdrfw_hyperspace_cfg_msg_t    hyperspace_msg;
  /* hyperspace config message */

  errno_enum_type estatus;
  /* Error status */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize message for firmware RL Config command */
  memset ( &hyperspace_msg, 0, sizeof( hdrfw_hyperspace_cfg_msg_t ) );
  msgr_init_hdr ( &hyperspace_msg.hdr, MSGR_HDR_HIT, HDR_FW_HYPERSPACE_CFG_CMD );


  hyperspace_msg.hyperspaceConfig = (uint16)hyperspace_flag;
  
  estatus = msgr_send( (msgr_hdr_struct_type *) &hyperspace_msg,
                        sizeof( hdrfw_hyperspace_cfg_msg_t ) );

  //ASSERT( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */
   
  return HDRHITCMD_COMMAND_SUCCESS;

} /* hdrhitcmd_hyperspace_cfg */

/*===========================================================================
FUNCTION HDRHITCMD_HANDOFF_OVERRIDE

DESCRIPTION
  Sets handoffoff override msg. (OVERRIDE_BASP)
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_handoff_override
(
  uint16 num_carriers,
  uint16 bestASPDict
    /** Software dictated ASP for idle mode override or immediate handoff trigger. */
)
{
  hdrfw_msg_u fw_cmd;
  /* Firmware command  */

  errno_enum_type estatus;
  /* Error status */

  uint8 i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset ( &fw_cmd, 0, sizeof (hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_HIT, HDR_FW_HANDOFF_OVERRIDE_CMD );

  if ( num_carriers > 3 )
  {
    num_carriers = 3;
  }
  
  for (i=0; i<num_carriers;i++)
  { 
    fw_cmd.handoff_override.handoffOverride[i].carrierValid = 1;
    fw_cmd.handoff_override.handoffOverride[i].bestASPDict = bestASPDict;
    fw_cmd.handoff_override.handoffOverride[i].handoffOverride.override = 1;
    fw_cmd.handoff_override.handoffOverride[i].handoffOverride.trigger = 0;
  }
    
  estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
  //ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */

  return HDRHITCMD_COMMAND_SUCCESS;

} /* hdrhitcmd_handoff_override */

/*===========================================================================
FUNCTION HDRHITCMD_ENABLE_DRC_GATING

DESCRIPTION
  To Enable/Disable DRC gating.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrhitcmd_status_type hdrhitcmd_enable_drc_gating
(
    boolean enableDrcGating
)
{
  hdrfw_msg_u fw_cmd;
  /* Firmware command  */

  errno_enum_type estatus;
  /* Error status */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u ) );
    /* Initialize the message data with zeroes */

  msgr_init_hdr (
    &fw_cmd.hdr, MSGR_HDR_HIT, HDR_FW_FTCMAC_CHANNEL_CFG_CMD );

  fw_cmd.ftcmac_channel_cfg.frameOffset = 0;
  fw_cmd.ftcmac_channel_cfg.fmacGains[0].drcLength = 4;
  fw_cmd.ftcmac_channel_cfg.fmacGains[1].drcLength = 4;
  fw_cmd.ftcmac_channel_cfg.fmacGains[2].drcLength = 4;
     //hdrfmac_drc_length_table[saset_info_ptr->drc_length];

  fw_cmd.ftcmac_channel_cfg.carrierMask = 1;   
     // Note only one carrier for now

  fw_cmd.ftcmac_channel_cfg.drcGating = (uint16)enableDrcGating;

  estatus = msgr_send ( &fw_cmd.hdr, sizeof ( hdrfw_msg_u ) );
    /* Send the message */

  //ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_BOLT_MODEM
  estatus = estatus;
#endif /* FEATURE_HDR_BOLT_MODEM */

  return HDRHITCMD_COMMAND_SUCCESS;

} /* hdrhitcmd_enable_drc_gating */
