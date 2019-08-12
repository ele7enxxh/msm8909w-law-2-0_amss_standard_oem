#ifndef _HDRHITCMD_H
#define _HDRHITCMD_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      H D R    H I T   C O M M A N D S

                            H E A D E R   F I L E

                             
GENERAL DESCRIPTION
  This file contains the databases, typedefs, enums and defines used by
  HDR HIT Commands

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
    Initialize ASET pilot information

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

  Copyright (c) 2007-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
    
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                        EDIT HISTORY FOR MODULE


This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/inc/hdrhitcmd.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------   
07/09/14     cnx     Support NV refresh without reset.
09/25/13     wsh     Changes to support BOLT bring up
06/22/10     arm     Changed short packet and MUP enable fields to boolean.
05/26/10     arm     Port non signaling legacy Macros to APIs for FTM. 
09/15/09     kss     Use new PACK() macro for Q6 compatibility.
11/10/08     smd     Replaced errno.h with IxErrno.h  
08/16/07     rmg     Made changes for MC demod support
04/23/07     rmg     Fixed lint warnings
03/28/07     rmg     Created this file 
===========================================================================*/

/* EJECT */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrsrchtypes.h"
#include "hdrrx.h"
#include "IxErrno.h"

/*==========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE
                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

This section contains function prototypes for the functions used in this
module.

==========================================================================*/
/* 0x 0 _ _ _ => Low
 * 0x 1 _ _ _ => Med
 * 0x 2 _ _ _ => High
 * 0x 3 _ _ _ => Error
 * 0x 4 _ _ _ => Fatal
 */
#define HDRHITCMD_STATUS_LOW   0x0000
#define HDRHITCMD_STATUS_MED   0x1000
#define HDRHITCMD_STATUS_HIGH  0x2000
#define HDRHITCMD_STATUS_ERROR 0x3000
#define HDRHITCMD_STATUS_FATAL 0x4000

/* 0x _ 0 _ _ => Acquire Callback
 * 0x _ 1 _ _ => System Lost Callback
 * 0x _ 2 _ _ => System Usability Callback
 */
#define ACQUIRE_CB             0x0000
#define SYSTEM_LOST_CB         0x0100
#define SYSTEM_USABILITY_CB    0x0200

typedef enum
{
  HDRHITCMD_COMMAND_SUCCESS        = 0x1000,
  HDRHITCMD_COMMAND_FAILED         = 0x2001,
  HDRHITCMD_COMMAND_FAILED_NOT_ACQ = 0x2002,
  HDRHITCMD_COMMAND_ISSUED         = 0x1003
} hdrhitcmd_status_type;

typedef enum
{
  /* Acquire Failed */
  HDRHITCMD_ACQUIRE_CB_ACQUIRE_FAILED = 
    (HDRHITCMD_STATUS_HIGH | ACQUIRE_CB) | 
    (0x000F & HDRSRCH_ACQUIRE_FAILED),

  /* Acquired */
  HDRHITCMD_ACQUIRE_CB_ACQUIRED = 
    (HDRHITCMD_STATUS_MED | ACQUIRE_CB) | 
    (0x000F & HDRSRCH_ACQUIRED),

  /* Could not reacquire after sleep */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_LOST_ACQ_AFTER_SLEEP = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_LOST_ACQ_AFTER_SLEEP),

  /* [6.6.5.5.2]  The strongest pilot is too weak for too long */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_RUP_PILOT_SUPERVISION = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_RUP_PILOT_SUPERVISION),

  /* Too long in sync state waiting for sync message */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_SYNC_MSG_TIMEOUT = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_SYNC_MSG_TIMEOUT),

  /* MDSP halted - Internal Error */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_MDSP_HALTED = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_MDSP_HALTED),

  /* Lock in RF Resources was absent for too long */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_NO_RESOURCES = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_NO_RESOURCES),

  /* MSTR slam could not be completed for too long due to unlocked fingers */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_MSTR_SLAM_TIMEOUT = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_MSTR_SLAM_TIMEOUT),

  /* HDR signal is weak - may be moving out of coverage */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_RESELECT  = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_RESELECT),

  /* HDR was told to release the RF lock, if it held it */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_ADVISE_UNLOCK_RF = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_ADVISE_UNLOCK_RF),

  /* HDR coverage is weak at sync state */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_SYNC_WEAK_HDR_COVERAGE = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_SYNC_WEAK_HDR_COVERAGE),

  /* DSP application is not ready */
  HDRHITCMD_SYSTEM_LOST_CB_SYSLOST_DSP_NOT_READY = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_LOST_CB) | 
    (0x000F & HDRSRCH_SYSLOST_DSP_NOT_READY),

  /* HDR signal strength is strong enough to be usable */
  HDRHITCMD_SYSTEM_USABILITY_CB_USABLE_TRUE = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_USABILITY_CB) | 
    (0x000F & HDRSRCH_USABLE_TRUE),

  /* HDR signal strength is not strong enough to be usable */
  HDRHITCMD_SYSTEM_USABILITY_CB_USABLE_FALSE = 
    (HDRHITCMD_STATUS_HIGH | SYSTEM_USABILITY_CB) | 
    (0x000F & HDRSRCH_USABLE_FALSE),

} hdrhitcmd_acquire_status_type;

typedef enum
{
  HDRHITCMD_REV_0,
  HDRHITCMD_REV_A,
  HDRHITCMD_REV_B
  
} hdrhitcmd_system_mode_type;

/*===========================================================================

FUNCTION HDRHITCMD_ACQUIRE

DESCRIPTION
  This procedure implements hdr acquire 

DEPENDENCIES
  None
  
RETURN VALUE
  hdrhitcmd_status_type 

SIDE EFFECTS
  None

===========================================================================*/
typedef PACK(struct)
{
  /* Band class - Cellular or PCS */
  uint16 band_class;

  /* the cdma frequency channel */
  uint16 chan_num; /* OR sys_channel_num_type chan_num if sys.h included */

  /* Acquisition mode */
  uint8 acq_mode;

} hdrhitcmd_acquire_param_type;

typedef
void (*hdrhitcmd_acquire_callback_type)
(
  hdrhitcmd_acquire_status_type callback_result
    /* Callback result indicating acquisition result, system lost reason 
     * or hdr system usability depending on which callback is called
     */
);

hdrhitcmd_status_type hdrhitcmd_acquire
(
  hdrhitcmd_acquire_param_type*   param_ptr,
  hdrhitcmd_acquire_callback_type hithdrcmd_acquire_cb
);

/*===========================================================================

FUNCTION HDRHITCMD_SYNC

DESCRIPTION
  This procedure implements hdr sync

DEPENDENCIES
  None
  
RETURN VALUE
  hdrhitcmd_status_type 

SIDE EFFECTS
  None

===========================================================================*/
typedef PACK(struct)
{
  /* PN offset */
  uint16 pn_offset;

} hdrhitcmd_sync_param_type;

hdrhitcmd_status_type hdrhitcmd_sync
(
  hdrhitcmd_sync_param_type* param_ptr
);


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
typedef PACK(struct)
{
  /* Enable control channel short packets
   *  - Sets AT to Rev A mode
   *  - Enables Rev A CC short packets 
   */
  uint8 enable_cc_short_pkts;

  /* Preamble MAC index for short packet xmission on the control channel
   * 71 -> Default cc short pkt mac index
   */
  uint8 sht_pkt_mac_index;

} hdrhitcmd_cc_config_param_type;

hdrhitcmd_status_type hdrhitcmd_cc_config
(
  hdrhitcmd_cc_config_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_IDLE

DESCRIPTION
  This procedure implements hdr idle

DEPENDENCIES

RETURN VALUE
  hdrhitcmd_status_type 
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  /* dummy idle param - just kept for uniformity */
  uint16 idle_param;

} hdrhitcmd_idle_param_type;

hdrhitcmd_status_type hdrhitcmd_idle
(
  hdrhitcmd_idle_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_ASET_INFO

DESCRIPTION
  This procedure implements aset info initialization.  The input is 
  information for upto three subactive sets with one pilot each.

DEPENDENCIES

RETURN VALUE
  hdrhitcmd_status_type 
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  /* Number of subactive sets */
  uint8   num_subactive_sets;

  /* Subactive set containing the control channel */
  uint8   cc_subactive_set_idx;

  /* Band class - Cellular or PCS of channel this pilot is on */
  uint16  subaset_0_band_class;

  /* the cdma frequency channel of channel this pilot is on */
  uint16  subaset_0_chan_num; 

  /* Whether this subaset should not be reported */
  boolean subaset_0_not_reportable;

  /* The firmware demod carrier index */
  uint8   subaset_0_demod_idx;

  /* Pilot PN offset. Range: 0 .. 511 */
  uint16  subaset_0_sector_0_pilot_pn;

  /* Reverse Power Control (RPC) is in softer handoff with prev pilot */
  boolean subaset_0_sector_0_softer_handoff;

  /* MAC index is 6 bits for Rev 0, 7 bits for Rev A,  10 bits for Rev B */
  uint16  subaset_0_sector_0_mac_index;

  /* DRC cover */
  uint8   subaset_0_sector_0_drc_cover;

  /* RL MAC Index is 9 bits for indicating the index used to transmit 
     power control commands.  Used if using RUP subtype 1 */
  uint16  subaset_0_sector_0_rl_mac_index;

  /* RAB MAC Index is 7 bits for indicating the index used to transmit 
     RAB commands.  Used if using RUP subtype 1 */
  uint8   subaset_0_sector_0_rab_mac_index;

  /* Reverse Activity Bit length */
  uint8   subaset_0_sector_0_rab_length;

  /* Reverse Activity Bit offset */
  uint8   subaset_0_sector_0_rab_offset;

  /* Converted dB signed value.  0, -6, -9, -12 or -15 
     0 indicates no RAChannelGain was specified for this pilot */
  int8    subaset_0_sector_0_ra_channel_gain;

  /* TxT2P adjust value in 1/4 dB */
  int8    subaset_0_sector_0_delta_t2p;

  /* The DSC associated with this cell.  This field is only valid if 
     softer_handoff is FALSE */
  uint8   subaset_0_sector_0_dsc;

  /* The pilot group ID identifies if a pilot with the same PN should be
     treated as though it is the same pilot */
  uint8   subaset_0_sector_0_pilot_group_id;

  /* The scheduler group this pilot belongs to */
  uint8   subaset_0_sector_0_scheduler_tag;

  /* Band class - Cellular or PCS of channel this pilot is on */
  uint16  subaset_1_band_class;

  /* the cdma frequency channel of channel this pilot is on */
  uint16  subaset_1_chan_num; 

  /* Whether this subaset should not be reported */
  boolean subaset_1_not_reportable;

  /* The firmware demod carrier index */
  uint8   subaset_1_demod_idx;

  /* Pilot PN offset. Range: 0 .. 511 */
  uint16  subaset_1_sector_0_pilot_pn;

  /* Reverse Power Control (RPC) is in softer handoff with prev pilot */
  boolean subaset_1_sector_0_softer_handoff;

  /* MAC index is 6 bits for Rev 0, 7 bits for Rev A,  10 bits for Rev B */
  uint16  subaset_1_sector_0_mac_index;

  /* DRC cover */
  uint8   subaset_1_sector_0_drc_cover;

  /* RL MAC Index is 9 bits for indicating the index used to transmit 
     power control commands.  Used if using RUP subtype 1 */
  uint16  subaset_1_sector_0_rl_mac_index;

  /* RAB MAC Index is 7 bits for indicating the index used to transmit 
     RAB commands.  Used if using RUP subtype 1 */
  uint8   subaset_1_sector_0_rab_mac_index;

  /* Reverse Activity Bit length */
  uint8   subaset_1_sector_0_rab_length;

  /* Reverse Activity Bit offset */
  uint8   subaset_1_sector_0_rab_offset;

  /* Converted dB signed value.  0, -6, -9, -12 or -15 
     0 indicates no RAChannelGain was specified for this pilot */
  int8    subaset_1_sector_0_ra_channel_gain;

  /* TxT2P adjust value in 1/4 dB */
  int8    subaset_1_sector_0_delta_t2p;

  /* The DSC associated with this cell.  This field is only valid if 
     softer_handoff is FALSE */
  uint8   subaset_1_sector_0_dsc;

  /* The pilot group ID identifies if a pilot with the same PN should be
     treated as though it is the same pilot */
  uint8   subaset_1_sector_0_pilot_group_id;

  /* The scheduler group this pilot belongs to */
  uint8   subaset_1_sector_0_scheduler_tag;

  /* Band class - Cellular or PCS of channel this pilot is on */
  uint16  subaset_2_band_class;

  /* the cdma frequency channel of channel this pilot is on */
  uint16  subaset_2_chan_num; 

  /* Whether this subaset should not be reported */
  boolean subaset_2_not_reportable;

  /* The firmware demod carrier index */
  uint8   subaset_2_demod_idx;

  /* Pilot PN offset. Range: 0 .. 511 */
  uint16  subaset_2_sector_0_pilot_pn;

  /* Reverse Power Control (RPC) is in softer handoff with prev pilot */
  boolean subaset_2_sector_0_softer_handoff;

  /* MAC index is 6 bits for Rev 0, 7 bits for Rev A,  10 bits for Rev B */
  uint16  subaset_2_sector_0_mac_index;

  /* DRC cover */
  uint8   subaset_2_sector_0_drc_cover;

  /* RL MAC Index is 9 bits for indicating the index used to transmit 
     power control commands.  Used if using RUP subtype 1 */
  uint16  subaset_2_sector_0_rl_mac_index;

  /* RAB MAC Index is 7 bits for indicating the index used to transmit 
     RAB commands.  Used if using RUP subtype 1 */
  uint8   subaset_2_sector_0_rab_mac_index;

  /* Reverse Activity Bit length */
  uint8   subaset_2_sector_0_rab_length;

  /* Reverse Activity Bit offset */
  uint8   subaset_2_sector_0_rab_offset;

  /* Converted dB signed value.  0, -6, -9, -12 or -15 
     0 indicates no RAChannelGain was specified for this pilot */
  int8    subaset_2_sector_0_ra_channel_gain;

  /* TxT2P adjust value in 1/4 dB */
  int8    subaset_2_sector_0_delta_t2p;

  /* The DSC associated with this cell.  This field is only valid if 
     softer_handoff is FALSE */
  uint8   subaset_2_sector_0_dsc;

  /* The pilot group ID identifies if a pilot with the same PN should be
     treated as though it is the same pilot */
  uint8   subaset_2_sector_0_pilot_group_id;

  /* The scheduler group this pilot belongs to */
  uint8   subaset_2_sector_0_scheduler_tag;

} hdrhitcmd_aset_info_param_type;

hdrhitcmd_status_type hdrhitcmd_aset_info
(
  hdrhitcmd_aset_info_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_DEMOD_FTC

DESCRIPTION
  This procedure implements demod fwd traffic channel command

DEPENDENCIES
  Run hdrhitcmd_aset_info() before executing this function

RETURN VALUE
  hdrhitcmd_status_type 
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  /* the data rate of TCHs */
  uint16 demod_0_rate;
  uint16 demod_1_rate;
  uint16 demod_2_rate;

} hdrhitcmd_demod_ftc_param_type;

hdrhitcmd_status_type hdrhitcmd_demod_ftc
(
  hdrhitcmd_demod_ftc_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_EQUALIZER_CONFIG

DESCRIPTION
  This procedure processes a HIT command for equalizer config
  parameters.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/

typedef PACK(struct)
{
  uint8 demod_pref;
  /* 0 => Best of Rake/Eq demod
   * 1 => Rake only demod
   * 2 => Equalizer only demod
   */

  /* Equalizer 0 configuration parameters */
  boolean eq0_enable;
  uint8   eq0_lms_step;
  uint8   eq0_filt_len;
  boolean eq0_div;
  uint8   eq0_mulpass_enable;
  uint8   eq0_mulpass2_lms_step;
  uint8   eq0_mulpass3_lms_step;
  uint8   eq0_multi_pass_c2i_used;

  /* Equalizer 1 configuration parameters */
  boolean eq1_enable;
  uint8   eq1_lms_step;
  uint8   eq1_filt_len;
  boolean eq1_div;
  uint8   eq1_mulpass_enable;
  uint8   eq1_mulpass2_lms_step;               
  uint8   eq1_mulpass3_lms_step;
  uint8   eq1_multi_pass_c2i_used;

  /* Equalizer 2 configuration parameters */
  boolean eq2_enable;
  uint8   eq2_lms_step;
  uint8   eq2_filt_len;
  boolean eq2_div;
  uint8   eq2_mulpass_enable;
  uint8   eq2_mulpass2_lms_step;               
  uint8   eq2_mulpass3_lms_step;
  uint8   eq2_multi_pass_c2i_used;

  /* Equalizer 3 configuration parameters */
  boolean eq3_enable;
  uint8   eq3_lms_step;
  uint8   eq3_filt_len;
  boolean eq3_div;
  uint8   eq3_mulpass_enable;
  uint8   eq3_mulpass2_lms_step;               
  uint8   eq3_mulpass3_lms_step;
  uint8   eq3_multi_pass_c2i_used;

} hdrhitcmd_equalizer_config_param_type;

hdrhitcmd_status_type hdrhitcmd_equalizer_config
( 
  hdrhitcmd_equalizer_config_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_RL_CONFIG

DESCRIPTION
  This procedure processes a HIT command for Rev Link config parameters.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  /* Rev Link Mode */
  /* 0 - Rev 0, 1 - Rev A */
  uint32 rl_mode;

  /* frame offset */
  uint16 frame_offset;
  
  /* I channel MASK */
  uint32 i_mask_hi;
  uint32 i_mask_lo;

  /* Q channel MASK */
  uint32 q_mask_hi;
  uint32 q_mask_lo;

  /* 0->closed loop mode using DSP DRC, 1->override with drc_value */
  uint32 drc_override; 

  /* 0->Var rate mode, 1->Fix rate mode */
  uint32 drc_mode;

  /* override DRC channel value */
  uint32 drc_value; 

  /* DRC cover override: if 1, the drc_cover = drc_cover_value */
  uint32 drc_cover_override;

  /* DRC cover value used if drc_cover_override = 1 */
  uint32 drc_cover_value;

  /* DRC length: 1, 2, 4 or 8 slots */
  uint32 drc_length;

  /* DRC gating enable: 0->nongated, 1->gated mode */
  uint32 drc_gating_enable; 

  /* relative gain of DRC to Pilot in 0.5 DB */
  int8 drc_ch_gain; 

  /* ARQ Mode */
  uint8  arq_mode;

  /* ACK override: 1->override, 0->normal operation */
  uint32 ack_override; 

  /* ACK Mode */
  uint32 ack_mode;

  /* relative gains of ACK to Pilot in 0.5 DB */
  int8 ack_ch_gain; 

  /* initial power level */
  int16  power_level;

  /* DSC channel override: if 1, the transmitted dsc = dsc_value */
  uint32 dsc_ch_override;

  /* DSC value used if dsc_override = 1 */
  uint32 dsc_value;

  /* DSC length: should be a multiple of 8. Default is 96 */
  uint32 dsc_length;

  /* DSC channel gain w.r.t. the pilot - signed with 0.5 dB resolution */
  int8 dsc_ch_gain;

  /* Force early termination: 
   * 0         -> no early termination
   * otherwise -> termination after force_early_terminate slots
   */
  uint32 force_early_terminate;

  /* RPC Enable: 0 -> Disable, 1 -> Enable */
  uint8  rpc_enable;

  /* Reverse Power Control step in 0.5 dB */
  uint8 rpc_step;

} hdrhitcmd_rl_config_param_type;

hdrhitcmd_status_type hdrhitcmd_rl_config
(
  hdrhitcmd_rl_config_param_type* param_ptr
);

hdrhitcmd_status_type hdrhitcmd_enable_ack_mode
(
  hdrfmac_ack_mode_enum_type ack_mode
);


/*===========================================================================

FUNCTION HDRHITCMD_RTCMAC_CONFIG

DESCRIPTION
  This procedure processes a HIT command for RTCMAC configuration.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  uint8  rev_mode;  
    /* 0 - Rev 0, 1 - Rev A */

  uint8  pre_transition_gains[12]; 
    /* data gains (w.r.t to the pilot) before the
     * T2P transition point.
     * 8-bit unsigned in units of 0.25 dB
     */

  uint8  post_transition_gains[12]; 
    /* data gains (w.r.t to the pilot) after the
     * T2P transition point
     * 8-bit unsigned in units of 0.25 dB
     */
  
  uint8  gain_transition_points[12];
    /* This value is 1 less than the number
     * of sub-frames for which the AT uses
     * the pre-transition gains
     */

  uint8  transmission_mode;
  
  uint8  aux_pilot_min_payload; 
    /* minimum RTC rate for which the
     * AT needs to transmit the aux pilot
     */
  
  uint8  aux_pilot_ch_gain; 
    /* aux pilot gain w.r.t. data channel
     * 5-bit 2’s complement in units of 1 dB
     */

  int8  rri_ch_gain_pre_transition[4]; 
    /* rri_ch_gain_pre_transition_n is the
     * rri channel gain for sub-packets
     * transmitted prior to the T2P transition
     * for a gain_transition value of
     * (n + 1) sub-frames
     * 4-bit 2’s complement in units of 1 dB
     */
  
  int8  rri_ch_gain_post_transition[4]; 
    /* rri_ch_gain_post_transition_n is the
     * rri channel gain for sub-packets
     * transmitted following the T2P
     * transition for a gain_transition value
     * of (n + 1) sub-frames
     * 4-bit 2’s complement in units of 1 dB
     */
  
  /* number of transmitted RTC frames                       */
  uint32 num_data_frames; 
    /* number of data frames to be run */

  /* relative gains of data to Pilot in 0.5 DB              */
  uint16 data_offset_nom;
  
  /* relative gains of data 9.6k to Pilot in 0.25 DB        */
  uint16 data_offset_9k6;
  
  /* relative gains of data 19.2k to Pilot in 0.25 DB       */
  uint16 data_offset_19k2;
  
  /* relative gains of data 38.4k to Pilot in 0.25 DB       */
  uint16 data_offset_38k4;
  
  /* relative gains of data 76.8k to Pilot in 0.25 DB       */
  uint16 data_offset_76k8;
  
  /* relative gains of data 153.6k to Pilot in 0.25 DB      */
  uint16 data_offset_153k6;

} hdrhitcmd_rtcmac_config_param_type;

hdrhitcmd_status_type hdrhitcmd_rtcmac_config
( 
  hdrhitcmd_rtcmac_config_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_MOD_RL

DESCRIPTION
  This procedure processes a HIT command for enabling the reverse link
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  uint8  data_rate_mode; 
    /* n = 0 : random, n= 1-4: pattern of length n */

  uint32 specified_rate; 
    /* if data_rate_mode = 0, rate is random. if the data_rate_mode = 3
     * and this value is0x000b0c0d, then the rate pattern is d,c,
     * b, d, c, b, d  etc.
     */

  uint32 specified_data_pattern;
    /* if 0xff, then each data byte is 0xff
     */

} hdrhitcmd_mod_rl_param_type;

typedef
void (*hdrhitcmd_mod_rl_callback_type)(void);

hdrhitcmd_status_type hdrhitcmd_mod_rl
(
  hdrhitcmd_mod_rl_param_type* param_ptr,
  hdrhitcmd_mod_rl_callback_type hdrhitcmd_mod_rl_cb
);


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
typedef PACK(struct)
{
  uint32 dummy_param; 
    /* dummy_param - not used */

} hdrhitcmd_deactivate_rtcmac_param_type;

hdrhitcmd_status_type hdrhitcmd_deactivate_rtcmac
(
  hdrhitcmd_deactivate_rtcmac_param_type* param_ptr
);


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
typedef PACK(struct)
{
  uint32 enable_short_pkts;
  uint32 enable_multiuser_pkts;
  uint32 enable_RAB_demod;
  uint32 enable_DRCLock_demod;

} hdrhitcmd_flmac_config_param_type;

hdrhitcmd_status_type hdrhitcmd_flmac_config
(
  hdrhitcmd_flmac_config_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_AMAC_CONFIG

DESCRIPTION
  This procedure processes a HIT command for Access Channel configuration.
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  uint8  test_access_rate;      
    /* = 0  => Variable Access Rate
     * else => Fixed Access Rate
     */

  uint16 data_bytes;
    /* Payload data byte count 
     * 0 => hdramac.data_bytes does not get overwritten 
     */

  uint8  access_cycle_duration;
    /* Access Cycle Duration */

  int16  probe_initial_adjust;
    /* Probe Initial Adjust  */

  uint8  open_loop_adjust;
    /* Nominal Power         */

  uint8  power_step;
    /* Power Step            */

  uint8  capsule_length_max;
    /* Max number of frames in access capsule */

  uint8  preamble_len;
    /* Preamble Length       */

  uint8  probe_num_step;
    /* Max Number Probes in 1 probe sequence */

  uint8  enable_enhanced_access;
    /* EnhancedAParamsIncluded */

  uint8  preamble_len_slots;
    /* PreambleLengthSlots   */

  uint8  access_offset;
    /* AccessOffset          */

  uint8  sector_access_max_rate;
    /* SectorAccessMaxRate   */

} hdrhitcmd_amac_config_param_type;

hdrhitcmd_status_type hdrhitcmd_amac_config
(
  hdrhitcmd_amac_config_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_AMAC

DESCRIPTION
  This procedure processes a HIT command for Access Channel operation
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  uint8 amac_cmd;
  /* 0 = HDRAMAC_DEACTIVATE_CMD,
   * 1 = HDRAMAC_ACTIVATE_CMD,
   * 2 = HDRAMAC_STOP_CMD,
   * 3 = HDRAMAC_START_ATTEMPT_CMD,
   * 4 = HDRAMAC_RESET_ATTEMPT_CMD,
   * 5 = HDRAMAC_TUNE_AWAY_TIME_CMD,
   * 6 = HDRAMAC_TUNE_AWAY_STOP_TX_CMD
   */
} hdrhitcmd_amac_param_type;

hdrhitcmd_status_type hdrhitcmd_amac
(
  hdrhitcmd_amac_param_type* param_ptr
);


/*===========================================================================

FUNCTION HDRHITCMD_ROTATOR_CONFIG

DESCRIPTION
  This procedure processes a HIT command for rotator configuration
   
DEPENDENCIES
  None.

RETURN VALUE
  hdrhitcmd_status_type
  
SIDE EFFECTS
  None.

===========================================================================*/
typedef PACK(struct)
{
  uint8 rot_config;
  /* 0 = Default [With Rotator    => RVAFC tracking
                  Without Rotator => VAFC  tracking],
   * 1 = VAFC Acq with VAFC tracking,
   * 6 = VAFC Acq with VAFC tracking and PLL off,
   * 2 = VAFC Acq with RVAFC tracking,
   * 3 = VAFC Acq with RVAFC tracking and SRL,
   * 4 = Rot  Acq with RVAFC tracking,
   * 5 = Rot  Acq with RVAFC tracking and SRL
   */

} hdrhitcmd_rotator_config_param_type;

hdrhitcmd_status_type hdrhitcmd_rotator_config
(
  hdrhitcmd_rotator_config_param_type* param_ptr
);


/*===========================================================================
FUNCTION HDRHITCMD_FMAC_ACTIVATE_MSG

DESCRIPTION
  HIT command for configuring:
  DRCLOCK LENGTH
  DRCLOCK_PERIOD
  hdrSystemModeConfig
  MAX DRC
   
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
);

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
    /*Enable/disable short packet mode*/
  boolean preamMUPEnable
  
);

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
);

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
);


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
  /** Step size for closed-loop power control adjustment, Q9 unsigned in dB.
       1.5 dB: 768  1dB: 512  (Default) 0.5dB: 256 */
  boolean txGainAdjStep_Valid
);

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
);

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

);

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
);

/*=========================================================================*/

/*===========================================================================

FUNCTION hitapp_refresh_nv

DESCRIPTION
  This procedure reads NV item and control the availablity of HIT module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hitapp_refresh_nv(void);
#endif /* _HDRHITCMD_H */
