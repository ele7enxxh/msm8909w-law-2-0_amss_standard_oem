#ifndef HDRRMACI_H
#define HDRRMACI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R   R E V E R S E   T R A F F I C   C H A N N E L   M A C   
                            
                             P R O T O C O L       

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Reverse Traffic Channel 
  Medium Access Control (MAC) Protocol for an HDR access terminal.
   

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.

  Copyright (c) 2005 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrrmaci.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/14   vke     Redesign fix for connection close during QTA gap
09/09/14   rmv     Changed the tDTx Tx power threshold to 20 dBm 
05/19/14   wsh     Cancel prev. RevLinkCarrier override in next TCA
09/25/13   arm     Consolidated Flexconn, sDTX, tDTX and nDTX into one log
                   packet and removed FW specific values.
08/10/13   arm     Provided tDTX Tx Power threshold to FW.
05/10/13   arm     Supported traffic DTX.
02/17/13   arm     Supported network dtx and device only dtx co-existence.
12/09/20   smd     Used AuxDRCCover to detect CSM6850 for DTX. 
11/01/12   smd     Disabled FlexConn and DTX in TAP call.
10/03/12   smd     Added FRAB check for FlexConn.  
09/05/12   smd     Fixes for FlexConn and DTX.
08/23/12   smd     Added updates for Device only DTX and FlexCONN.
06/20/12   smd     Supported Device only DTX and FlexCONN.
01/04/12   rmg     Shifted fw msg receiving code from hdrrmac to hdrtx.
10/27/11   kss     Added support for LTE to DO optimized handoff.
10/21/11   kss     Initialize tx initial power adjust.
03/22/11   arm     Added support for thermal pa backoff feature.
09/17/09   etv     Added support for RMAC0 Q6 interface changes.
12/18/07   kss     Track nom. power and turnaround.
08/08/05   kss     Track Tx start status.
08/02/05   kss     Added link_is_suspended.
04/15/05   kss     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrtx.h"
#ifdef FEATURE_HDR_THERMAL_BACKOFF
#include "ds707_ccm.h"
#endif


/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_HDR_FLEX_CONN
#define HDRRMAC_FLEXCONN_RL_INACTIVE_TIMEOUT_SF_CNT          15
  /* The time out value should be consistant with FMAC timeout value.
   	 100ms = 15 subframes
  */
#define HDRRMAC_FLEXCONN_MAX_ACTIVE_MAC_INDEXES                     25
   /* maximum number of active mac indexes which allows flexconn being enabled */

#define HDRRMAC_FLEXCONN_FRAB_BUSY_THRESH_1   ( -6554 )
  /* FRAB threshold to disable FlexConn mode. -0.2*32768=6554 */

#define HDRRMAC_FLEXCONN_FRAB_BUSY_THRESH_2   ( -3277 )
  /* FRAB threshold to disable FlexConn mode. -0.1*32768=6554 */
#endif /* FEATURE_HDR_FLEX_CONN */

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
#define HDRRMAC_DTX_DRC_LENGTH_MIN_IN_SLOTS                4
   /* Minimal DRC length in slots to enable dtx */
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_TRAFFIC_DTX
#define HDRRMAC_TDTX_FRAB_THRESHOLD        ( -19961  )
  /* FRAB threshold to disable tdtx. -0.6*32768= -19961 */

#define HDRRMAC_TDTX_TX_PWR_THRESH_DB       20
/* Tx power threshold. tDTX should be turned off if tx power is above 20 db */
#endif /* FEATURE_HDR_TRAFFIC_DTX */

/* Protocol state enum type. */

/*
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/
typedef enum
{
  HDRRMAC_INACTIVE_STATE=0,
  HDRRMAC_SETUP_STATE=1,
  HDRRMAC_OPEN_STATE=2
} hdrrmac_state_enum_type;

#ifdef FEATURE_HDR_THERMAL_BACKOFF

/* The enum defines various pa backoff duration patterns */
typedef enum
{
  HDRRMAC_NO_BACKOFF_PATTERN            = 0,  /* (0% backoff duration ratio)   */
  HDRRMAC_FIRST_BACKOFF_PATTERN    = HDRRMAC_NO_BACKOFF_PATTERN,
  HDRRMAC_BACKOFF_PATTERN_ONE      = 1,  /* (35% backoff duration ratio)  */
  HDRRMAC_BACKOFF_PATTERN_TWO      = 2,  /* (65% backoff duration ratio)  */
  HDRRMAC_TOTAL_BACKOFF_PATTERN    = 3,  /* (100% backoff duration ratio) */
  HDRRMAC_LAST_BACKOFF_PATTERN     = HDRRMAC_TOTAL_BACKOFF_PATTERN,
  HDRRMAC_NUM_BACKOFF_PATTERNS
} hdrrmac_thermal_backoff_pattern_enum_type;

/* The enum defines step timers for pa backoff*/
typedef enum
{
  HDRRMAC_STEP_UP_TIMER,
  HDRRMAC_STEP_DOWN_TIMER
} hdrrmac_thermal_backoff_timer_enum_type;


/* This struct encapsulates the FW params configured by CCM */
typedef struct {

  /* step down timer */
  uint16 step_down_timer_ms;

  /* step down timer = step_up_multiplier * step_down_timer*/
  uint16 step_up_timer_ms;

  /* delay to account for signaling delay between DEM and HDR L1 */
  uint16 delay_time_ms;

  /* Default tx power backoff pattern number to start with */
  uint8  default_tx_power_bo_pattern_number;

  /* Maximum tx power backoff pattern number.*/
  uint8  max_tx_power_bo_pattern_number;

  /* Total backoff period */
  uint16 tx_power_bo_period_ms;
  
} hdrrmac_backoff_params_type;

#endif /* FEATURE_HDR_THERMAL_BACKOFF */

#ifdef FEATURE_LTE_TO_HDR_OH

#define HDRRMAC_NUM_REV_CDMA_CHANNELS_MAX 16
#define HDRRMAC_NUM_PILOTS_MAX            32

typedef struct
{
  /* PilotPN */
  sys_pilot_pn_type pilot_pn;             

  /* OpenLoopAdjust */
  int16  open_loop_adjust;               

  /* InitialAdjust */
  int16  initial_adjust;               

  /* PilotStrengthIncluded */
  boolean pilot_strength_incl;

  /* PilotStrengthNominal */
  uint8  pilot_strength_nominal;          

  /* PilotStrengthCorrectionMin */
  uint8  pilot_strength_correction_min;

  /* PilotStrengthCorrectionMax */
  uint8  pilot_strength_correction_max;   

} hdrrmac_open_loop_params_type;

typedef struct
{
  /* ReverseCDMAChannel */
  sys_channel_type  rev_cdma_chan;

  /* NumPilots */
  uint8 num_pilots;                   

  /* HRPDOpenLoopParameters per PN */
  hdrrmac_open_loop_params_type olp[HDRRMAC_NUM_PILOTS_MAX];  

} hdrrmac_open_loop_params_per_rev_cdma_chan_type;

typedef struct
{
  /* Whether info is valid */
  boolean valid;

  /* NumReverseCDMAChannels */
  uint8 num_rev_cdma_channels;

  /* OpenLoopParams per CDMA channel */
  hdrrmac_open_loop_params_per_rev_cdma_chan_type 
      rev_cdma_ch_olp[HDRRMAC_NUM_REV_CDMA_CHANNELS_MAX];

} hdrrmac_open_loop_params_msg_type;

#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_QTA

typedef struct
{
  /* Whether RMAC deactvate is pending on TA */
  boolean pend_on_ta;
} hdrrmac_tune_away_info_params_type;

#endif /* FEATURE_HDR_QTA */

/* HDR RTCMAC Common Parameters */
typedef struct
{
  /* Subtype Value */
  uint16 subtype;

  /* Protocol state variable. */
  hdrrmac_state_enum_type     state;

  /* Reverse silence interval period */
  uint16  silence_period;

  /* Reverse silence interval duration */
  uint8   silence_duration;

  /* Reverse silence interval enable flag */
  boolean silence_enabled;

  /* Status of PA */
  hdrtx_rf_status_enum_type rf_status;   

#ifdef FEATURE_HDR_QTA
  /* current state of tune away */
  hdrrmac_tune_away_info_params_type tune_away_info;
#endif /* FEATURE_HDR_QTA */

  /* Track who has requested reverse link throttling to be disabled. */
  uint16  throttle_disable_requests;

  /* Suspended (for tuneaway) state. True == RTCMAC operation is suspended. */
  boolean link_is_suspended;

  /* Tx start/stop status as indicated by upper layer. */
  boolean tx_started;

  /* Turnaround in dB * 256 */
  int16 turnaround_db256;

  /* Nominal power (open loop from AP) in dB * 256 */
  int16 nominal_pwr_db256;

#ifdef FEATURE_HDR_THERMAL_BACKOFF

  hdrrmac_backoff_params_type thermal_power_bo_params;
    /* Thermal flow control configuration parameters from CCM */

  ds707_ccm_flow_control_cmd_enum_type thermal_power_bo_cmd;
    /* Last thermal flow control command received */

  hdrrmac_thermal_backoff_pattern_enum_type thermal_power_bo_pattern;
    /* Current thermal flow control pattern */

  boolean thermal_power_bo_is_on;
    /* Flow control state variable which indicates whether thermal flow control is 
       turned ON or OFF at any point of time. */

  rex_timer_type thermal_power_bo_step_up_timer;
    /*  Timer to apply Flow control command on a periodic basis */

  rex_timer_type thermal_power_bo_step_down_timer;
    /*  Timer to apply Flow control command on a periodic basis */

  rex_timer_type thermal_power_bo_backoff_timer;
    /*  Timer to apply Flow control command on a periodic basis */

  rex_timer_type thermal_power_bo_non_backoff_timer;
    /*  Timer to apply Flow control command on a periodic basis */

  rex_crit_sect_type thermal_power_bo_crit_sect; 
    /* Critical Section for protecting flow control global state */

#endif /* FEATURE_HDR_THERMAL_BACKOFF */

  boolean tx_adjust_needs_init;
    /* Track whether tx adjust needs to be init'd */

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrrmac_open_loop_params_msg_type olp_msg;
#endif /* FEATURE_LTE_TO_HDR_OH */

#if defined( FEATURE_HDR_FLEX_CONN ) || defined( FEATURE_HDR_DEVICE_ONLY_DTX )
  boolean is_tap_call;
    /* Whether the current call is a tap call */
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_FLEX_CONN
  boolean is_fl_active_flexconn;
    /* Whether FL is active -- for FlexConn */
  boolean is_flexconn_nv_enabled;
   /* Whether flexconn is enabled by NV item */
  boolean flexconn_mode;
   /* Is flexconn mode enabled in firmware */
  boolean flexconn_monitor_started;
   /* Has RMAC started to monitoring conditions to enable/disable flexconn mode */
  uint8   rl_inactive_sf_cnt;
    /* number of subframe when reverse link stays inactive */
  uint16  rl_sf_cnt;
    /* number of subframe since reverse link starts transmitting */
  uint16  mac_indexes_cnt;
    /* number of max indexes, i.e. number of active users, i.e number of FTvalid==1*/
  boolean first_dsc_duty_cycle;
    /* whether phone is transmitting dsc in the first dsc duty cycle after starting tx */
  boolean flexconn_disabled_before_tx_stop;
    /* whether flexconn is disabled because of coming OFS, tune away or RLSI. */
  uint8   flexconn_nest_count;
    /* number of nested start/stop flexconn monitoring */
  boolean is_repointing_msg_rcvd;
    /* Bool to indicate whether SW received  repointing message */
#endif /* FEATURE_HDR_FLEX_CONN */
#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
  boolean is_fl_active_dtx;
   /* Whether FL is active -- for DTX */
  boolean is_dtx_nv_enabled;
   /* Whether dtx is enabled by NV item */
  boolean device_only_dtx_mode;
   /* Is sDTX mode enabled in firmware */
  boolean dtx_monitor_started;
   /* Has RMAC started to monitoring conditions to enable/disable flexconn mode */
  uint8   dtx_frame_offset;
    /* Frame offset for DTX. valid value is 0,1,2,3(?). */
  uint16  dtx_drc_length;
    /* current drc length */
  uint8   dtx_nest_count;
    /* number of nested start/stop flexconn monitoring */
  boolean dtx_drc_boost;
    /* Whether to boost/deboost DRC channel gain */
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */
#ifdef FEATURE_HDR_REVB_DTX_MODE
  boolean network_dtx_mode;
    /* Is network based DTX mode enabled */
#endif /* FEATURE_HDR_REVB_DTX_MODE */

#ifdef FEATURE_HDR_TRAFFIC_DTX
   boolean tdtx_nv_is_enabled;
     /* Whether tDTX is enabled by NV item */
   boolean tdtx_mode;
     /* Is tDTX mode enabled in firmware */
#endif /* FEATURE_HDR_TRAFFIC_DTX */
   boolean cxm_allows_dtx;
     /* If coex manager allows DTX */
   uint16  dtx_config;
     /* Current dtx config programmed to FW */
} hdrrmac_common_type;



/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION HDRRMAC_ENTER_STATE                                        INTERNAL

DESCRIPTION
  This function manages the protocol state transitions.  For valid state
  transitions, the current state is exited by performing any required exit
  processing, then the new state is entered by perfroming any required entry
  processing.

DEPENDENCIES
  None.

PARAMETERS
  new_state - new state to enter.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_enter_state
(
  hdrrmac_state_enum_type new_state
);

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRRMAC_PROCESS_OLP_MSG                                  INTERNAL

DESCRIPTION
  This function processes the HRPDOpenLoopParameters message.

DEPENDENCIES
  None.

PARAMETERS
  rcv_msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrmac_process_olp_msg
(
  dsm_item_type * rcv_msg_ptr
  /*lint -esym(715,rcv_msg_ptr) 
   * Ignore argument(rcv_msg_ptr) not used lint error #715*/
);

/*===========================================================================
FUNCTION HDRRMAC_CALC_PILOT_STRENGTH_CORRECTION

DESCRIPTION
  This function computes pilot strength correction value.

DEPENDENCIES
  None.

PARAMETERS
  carrier, msg_nom, msg_corr_min, msg_corr_max

RETURN VALUE
  Pilot strength correction value.

SIDE EFFECTS
  None.
===========================================================================*/
extern int8 hdrrmac_calc_pilot_strength_correction
(
  uint8 carrier,         /* Carrier to use */
  int8 msg_nom,          /* Nom. value from msg. */
  int8 msg_corr_min,     /* Min corr from msg. */
  int8 msg_corr_max      /* Max corr from msg. */
);

#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_FLEX_CONN

/*==========================================================================

FUNCTION  HDRRMAC_PROCESS_FLEXCONN_REPOINT

DESCRIPTION
  process flexconn_repoing indication from fw
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_process_flexconn_repoint( void );

/*==========================================================================

FUNCTION  HDRRMAC_SET_FLEXCONN_MODE

DESCRIPTION
  This function calls appropriate driver function to send a message to FW to
  enable or disable FLEXCONN mode
    
DEPENDENCIES
  None

INPUTS
  flexconn_mode:  flexconn mode, TRUE means enabling flexconn
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_set_flexconn_mode( boolean flexconn_mode );

#endif /* FEATURE_HDR_FLEX_CONN */
#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
/*==========================================================================

FUNCTION  HDRRMAC_SET_DEVICE_ONLY_DTX_MODE

DESCRIPTION
  This function calls driver to send a message to FW to enable or disable
  DTX mode
    
DEPENDENCIES
  None

INPUTS
  dtx_mode:  dtx mode, TRUE means enabling DTX

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_set_device_only_dtx_mode( boolean dtx_mode );

/*==========================================================================

FUNCTION  HDRRMAC_DTX_UPDATE_DRC_BOOST

DESCRIPTION
  This function sends DTX_CFG message to FW to update drc boost flag
    
DEPENDENCIES
  None

INPUTS
  new_drc_boost_flag:  wheter to boost/deboost DRC channel gain

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac_dtx_update_drc_boost( boolean new_drc_boost_flag );

#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */

/*=============================================================================
FUNCTION HDRRMAC_OVERRIDE_REV_LINK_CARRIER_SET                     EXTERNAL

DESCRIPTION
  This function is a wrapper to rfm_hdr_update_rev_link_carrier_set() which
  updates RF of the reverse link carrier set.

DEPENDENCIES
  None.

PARAMETERS
  enabled - If override to 1 carrier is enabled
  chan_list - List of rev links channels to override to
  chan_cnt  - Number of channels in the list

RETURN VALUE
  None

SIDE EFFECTS
  None.
==============================================================================*/
void hdrrmac_override_rev_link_carrier_set
(
  boolean enabled,
  sys_channel_type chan_list[],
  uint8 chan_cnt
);
#endif /* HDRRMACI_H */

