#ifndef HDRMULTIRAT_H
#define HDRMULTIRAT_H

/*===========================================================================

                          H D R  M U L T I R A T
                           D E F I N I T I O N S

DESCRIPTION
  This module contains utilities and interface components that HDR core modules
  can use to interact with external entities in implementing multi-RAT support.

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrmultirat.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/15   mbs     Supported QTA using TRM API
03/16/15   wsh     Reduced stack usage to avoid overflow
01/15/15   arm     Fallback to SS or DSDS mode from DR based on initial 
                   multimode config
12/04/14   arm     Changes to support new unified micro priority changes
11/04/14   arm     Band avoidance: move power info saving to hdrsrch task and
                   enable BA only for DSDA mode.
10/06/14   arm     Fixed band avoidance compilation warning.
10/01/14   arm     Changes to support HDR band avoidance.
08/03/14   arm     Changes to request RX_ANY chain in DR mode for idle.
07/29/14   tnp     FR22375: Fix Medium compiler warnings in HDR
07/03/14   arm     Used BG TRAFFIC reason for DR-DSDS traffic.
06/30/14   arm     Changes to support DR-DSDS.
06/04/14   arm     Changes to support DO to G QTA
10/14/13   arm     Send freq id list again to FW in Reacq state.
02/27/14   sat     Sending updated frequency list information to FW after wakeup
02/18/14   bb      Changes to use proper carrier index while retrieving RF power
12/06/13   arm     Supported Div chain frequency and power reporting.
01/24/14   bb      Changes to add more reasons to hdrmultirat_priority_type
                   to match with COEX priorty table indexes
03/15/14   wsh     Change to support T/T and new RF concurrency concern
03/14/14   rmv     Enabled D-LNA in DSDA depending on band combinations 
11/26/13   arm     Handle SSSS->DSDA transition in online mode.
08/19/13   arm     Merged DSDS feature for triton.
07/20/13   arm     Enabled band specific DTX.
07/16/13   arm     Added support for  FW DSDA Rx/Tx log packet logging.
06/13/13   arm     Fixed coex power bias value.
04/22/13   arm     Created module for HDR interaction Multi RAT scenario.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrmsg.h"
#include "hdrdebug.h"
#include "hdrsrchtypes.h"

#include "msgr.h"
#include "comdef.h" /* Interface to the common definitions */
#include "cmd.h"    /* Interface to the command types*/
#include "sys.h"
#include "sys_v.h"

#include "cm.h"

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
#include "cxm.h"
#include "wwan_coex_mgr.h"
#include "hdrfw_msg.h"
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

 This section contains local definitions for constants, macros, typesdefs,
 and other items needed by this module.


===========================================================================*/

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW

#define HDRMULTIRAT_COEX_PWR_TIMER_MS                  20
  /* Interval to report coex rx tx power to MCS (every 20 ms)
     in Idle and traffic state */
#define HDRMULTIRAT_REVERSE_LINK                       CXM_LNK_DRCTN_UL
  /* Uplink -> reverse link */  
#define HDRMULTIRAT_FORWARD_LINK                       CXM_LNK_DRCTN_DL  
  /* Downlink -> forward link */  
#define HDRMULTIRAT_COEX_PWR_BIAS                      100
  /* Bias subtracted from rx tx coex power sampled before going to sleep
     but reported after wakeup in db10*/

#define HDRMULTIRAT_HDR_BANDWIDTH                      1250000

#define HDRMULTIRAT_MAX_NUM_CARRIERS                   HDRSRCH_MAX_DEMOD_CARRIERS_CNT

#define HDRMULTIRAT_DEFAULT_FREQ_ID                    0xFFFFFFF0

#define hdrmultirat_cxm_freq_id_list_type              cxm_freqid_info_ind_s
  /* cxm_freqid_info_ind_s msg is used by MCS CXM to give
     us freq id for every band-chan we use */


#ifdef FEATURE_HDR_BAND_AVOIDANCE

#define HDRMULTIRAT_BAND_AVOID_ENTRIES                       100 /* 2 seconds worth */
  /* Number of power entries to save to select the 10th percentile */

#define HDRMULTIRAT_BAND_AVOID_10PERCENT        (HDRMULTIRAT_BAND_AVOID_ENTRIES/10)
  /* We want the low side of the list for Rx (10% up from the smallest value) */
  
#define HDRMULTIRAT_BAND_AVOID_90PERCENT        (HDRMULTIRAT_BAND_AVOID_ENTRIES*90/100)
  /* We want the high side of the list for Tx (10% down from the largest value). */

#endif /* FEATURE_HDR_BAND_AVOIDANCE */


cxm_activity_table_s  hdrmultirat_cxm_priority_table[MAX_ACTIVITY_TIERS];
  /* Create a local copy of priority table to save it from MCS */

/* Enum for Activity tier based on priority  */
typedef enum
{
  /*! Highest Priority Tier: Should be used for activities which needs to be 
    protected from the other tech, only reacq for HDR.
    For Example: PLL Tuning, AGC Acquisition, W PICH Decode, Any G Tx for 2
      slots etc */
  HDRMULTIRAT_HIGH_PRIORITY_ASDIV = ACTIVITY_TIER_3,

#ifdef FEATURE_HDR_UNIFIED_MICRO_PRIORITY
  
  HDRMULTIRAT_HIGH_PRIORITY = ACTIVITY_TIER_5,
  
  /*! Second lowest Tier: Second highest priority for activities that can be 
    blanked when it conflicts with other tech's higher priority tier.
    e.g. G PCH, G BCCH, Any W Tx*/
  
  HDRMULTIRAT_NORMAL_PRIORITY = ACTIVITY_TIER_10,
  HDRMULTIRAT_LOW_PRIORITY_QTA = ACTIVITY_TIER_30,

  /* End of the list indicator */
#else

  HDRMULTIRAT_HIGH_PRIORITY_5,
  HDRMULTIRAT_HIGH_PRIORITY_10 = ACTIVITY_TIER_10,
  
  /*! Second lowest Tier: Second highest priority for activities that can be 
    blanked when it conflicts with other tech's higher priority tier.
    e.g. G PCH, G BCCH, Any W Tx*/
  
  HDRMULTIRAT_HIGH_PRIORITY_15 = ACTIVITY_TIER_15,
  HDRMULTIRAT_LOWER_PRIORITY_20 = ACTIVITY_TIER_20,
  HDRMULTIRAT_LOWER_PRIORITY_25 = ACTIVITY_TIER_25,
#endif /* FEATURE_HDR_UNIFIED_MICRO_PRIORITY */

  /* End of the list indicator */
  HDRMULTIRAT_MAX_PRIORITIES
}hdrmultirat_priority_type;

/* Structure used to store frequency information received/sent from MCS */
typedef struct
{
   rfm_device_enum_type                rf_device;
     /* The logical chain ID associated with the request */

   sys_channel_type                    channel[HDRMULTIRAT_MAX_NUM_CARRIERS];
	      /* Channel to tune the RF to */

   hdrsrch_demod_idx_type              demod_idx[HDRMULTIRAT_MAX_NUM_CARRIERS];
             /* carrier ID of the channels */

   uint8                               channel_cnt;
     /* Number of channels active*/

   uint32                              freqid[HDRMULTIRAT_MAX_NUM_CARRIERS];
     /* frequency id for each band-chan received from coex manager */

   int16                               coex_power[HDRMULTIRAT_MAX_NUM_CARRIERS];
     /* Coex filtered power stored for each carrier */
  
}hdrmultirat_band_chan_info_type;

/* Structure to store all coex related info */
typedef struct
{
   hdrmultirat_band_chan_info_type     rx;
     /* All  Rx info for dsda coex */

   hdrmultirat_band_chan_info_type     tx;
     /* All  Tx info for dsda coex */

   hdrmultirat_band_chan_info_type     div;
     /* Store div chain info for coex */

}hdrmultirat_coex_info_type;

#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

/* Enumerate all device modes */
typedef enum
{
  HDRMULTIRAT_SINGLE_SIM,
  HDRMULTIRAT_DUAL_SIM   
}hdrmultirat_ds_pref_enum_type;

typedef enum
{
  HDRMULTIRAT_SINGLE_STANDBY = HDR_DSDX_STATE_INACTIVE,
  HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY = HDR_DSDS_STATE_ACTIVE,
  HDRMULTIRAT_DUAL_SIM_DUAL_ACTIVE = HDR_DSDA_STATE_ACTIVE
#ifdef FEATURE_HDR_DR_DSDS
  ,HDRMULTIRAT_DR_DUAL_SIM_DUAL_STANDBY = HDR_DR_DSDS_STATE_ACTIVE
#endif 
}hdrmultirat_mode_pref_enum_type;

#ifdef FEATURE_HDR_QTA
/* Enumerate QTA states */
typedef enum
{
  HDRMULTIRAT_QTA_START = HDRFW_QTA_START_CMD,
  HDRMULTIRAT_QTA_END = HDRFW_QTA_END_CMD
}hdrmultirat_qta_state_enum_type;
#endif /* FEATURE_HDR_QTA */

#ifdef FEATURE_HDR_BAND_AVOIDANCE

typedef struct
{
  /* Transmit power  */
  int32 tx_pwr[HDRMULTIRAT_MAX_NUM_CARRIERS][HDRMULTIRAT_BAND_AVOID_ENTRIES];

  /* Receive power  */
  int32 rx_pri_pwr[HDRMULTIRAT_MAX_NUM_CARRIERS][HDRMULTIRAT_BAND_AVOID_ENTRIES];

  /* Diversity Receive power  */
  int32 rx_sec_pwr[HDRMULTIRAT_MAX_NUM_CARRIERS][HDRMULTIRAT_BAND_AVOID_ENTRIES];

  /* store filtered power entries */
  int32 tx_pwr_value[HDRMULTIRAT_MAX_NUM_CARRIERS];
  int32 rx_pri_pwr_value[HDRMULTIRAT_MAX_NUM_CARRIERS];
  int32 rx_sec_pwr_value[HDRMULTIRAT_MAX_NUM_CARRIERS];

  /* count number of tx and rx entries in table */
  uint16  count;
} hdrmultirat_band_avoid_type;

#endif /* FEATURE_HDR_BAND_AVOIDANCE */

/* Structure to store elements common to all Hdr Multi Rat */
typedef struct
{
  hdrmultirat_ds_pref_enum_type        ds_pref;
    /* If we are currently in single sim or dual sim mode */

  hdrmultirat_mode_pref_enum_type      mode_pref;
    /* mode (SS/DSDA/DSDS/DRDSDS mode) we are currently in */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
  hdrmultirat_coex_info_type           coex_info;
    /* Band Chan & Power info needed for coex manager */ 
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
  boolean                              dsda_rx_log_pkt_is_logged;
    /* Flag to indicate if fw rx dsda log pkt is being logged */

  boolean                              dsda_tx_log_pkt_is_logged;
    /* Flag to indicate if fw tx dsda log pkt is being logged */

  boolean                              is_mode_changed;
    /* Did mode change when FW was inactive? */

#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */

  boolean                              frequency_update_pending;
    /* Flag to check after wake up if frequency update was pending */

#ifdef FEATURE_HDR_BAND_AVOIDANCE

  hdrmultirat_band_avoid_type          ba;
    /* Structure to store all band avoidance data */
#endif /* FEATURE_HDR_BAND_AVOIDANCE  */

  /*-------------------------------------------
     Critical section
  -------------------------------------------*/

  rex_crit_sect_type                   crit_sect; 
    /* Critical Section */ 

}hdrmultirat_struct_type;

LOCAL hdrmultirat_struct_type          hdrmultirat;

/* <EJECT> */ 
/*===========================================================================

                        FUNCTION  DEFINITIONS

===========================================================================*/

/*=========================================================================

FUNCTION     : HDRMULTIRAT_POWERUP_INIT

DESCRIPTION  : This function inits frequnce info structure.
               

DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrmultirat_powerup_init( void );

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
/*===========================================================================

FUNCTION HDRMULTIRAT_PROCESS_FREQ_CHANGE

DESCRIPTION
  This function sends band-chan change msg to MCS whenever it changes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_process_freq_change
(
   rfm_device_enum_type                  rf_device,
    /* The logical chain ID */

   const sys_channel_type                *channel,
     /* Channel to tune the RF to */

   cxm_tech_link_direction               direction,
     /* Direction - reverse/forward link */

   uint8                                 channel_cnt
     /* Num channels */
);
/*===========================================================================

FUNCTION HDRMULTIRAT_PROCESS_DIV_CHANGE

DESCRIPTION
  This function informs MCS of any change in diversity mdoe.

DEPENDENCIES
  None
 
INPUT 
 
  boolean div_mode:  If diversity was turned on or off.
 
  rfm_device_enum_type:   The logical chain ID
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_process_div_change
(
  boolean                               div_mode,
    /* Div ON/OFF? */
  
  rfm_device_enum_type                  rf_device
    /* The logical chain ID */

);
/*===========================================================================

FUNCTION HDRMULTIRAT_REPORT_COEX_POWER

DESCRIPTION
  This function reports coex rx - tx power to MCS every 20 ms

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_report_coex_power 
( 
   boolean               save_only
     /* Just save the power or send it to MCS? */ 
);

/*===========================================================================

FUNCTION HDRMULTIRAT_REPORT_COEX_POWER_WITH_BIAS

DESCRIPTION
  This function sends the coes power we saved before goind to sleep + bias
  to MCS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_report_coex_power_with_bias( void );

/*=========================================================================

FUNCTION     : HDRMULTIRAT_GET_MODE_PREF

DESCRIPTION  : This function returns devide mode pref.
               

DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : Device mode -> SS/DSDS/DSDA

SIDE EFFECTS : None

=========================================================================*/

hdrmultirat_mode_pref_enum_type hdrmultirat_get_mode_pref ( void );
/*=========================================================================

FUNCTION     : HDRMULTIRAT_GET_CXM_PRIORITY_TABLE_CB

DESCRIPTION  : This function gets coex manager priority table and saves it.
               

DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrmultirat_get_cxm_priority_table_cb
( 
   cxm_tech_type  tech_id,

   cxm_activity_table_s *activity_tbl
);
/*=========================================================================

FUNCTION     : HDRMULTIRAT_GET_CXM_PRIORITY

DESCRIPTION  : This function returns priority for given priority level from 
               the table. 
               
DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : Priority

SIDE EFFECTS : None

=========================================================================*/

uint32 hdrmultirat_get_cxm_priority
(
   hdrmultirat_priority_type pri_level
);

/*===========================================================================

FUNCTION HDRMULTIRAT_DSDA_MODE_CHANGE

DESCRIPTION
  This function processes mode pref change to DSDA.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_dsda_mode_change( void );

/*===========================================================================

FUNCTION HDRMULTIRAT_PROCESS_FREQ_ID_LIST

DESCRIPTION
  Process the fre id list rxd from coex manager.

DEPENDENCIES
  None

PARAMETERS
  frequency id list

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_process_freq_id_list 
( 
  hdrmultirat_cxm_freq_id_list_type* freqid_list
);

/* ===========================================================================

FUNCTION HDRMULTIRAT_PRIORITY_SWITCH

DESCRIPTION
  This function gets the priority for the activity level and then sends it to
  FW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_priority_switch
( 
   hdrmultirat_priority_type  priority_level
);

/* ===========================================================================

FUNCTION HDRMULTIRAT_CXM_ALLOWS_DTX

DESCRIPTION
  This function queries coex manager if its OK to enable dtx depending on
  C+G band class/channel combinations with G.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if DTX is allowed
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmultirat_cxm_allows_dtx( void );

/* ===========================================================================

FUNCTION HDRMULTIRAT_CXM_ALLOWS_DLNA

DESCRIPTION
  This function queries coex manager if its OK to enable D-LNA depending on
  C+G band class/channel combinations with G.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if D-LNA is allowed
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmultirat_cxm_allows_dlna( void );
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_FW
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_DSDX_ENABLE_CFG_MSG

DESCRIPTION
  Send DSDX enable msg to FW

DEPENDENCIES
  None

PARAMETERS
  State to send to FW

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_dsdx_enable_cfg_msg 
( 
   hdrmultirat_mode_pref_enum_type state 
);

/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_DSDA_PRIORITY_CFG_MSG

DESCRIPTION
  Send DSDA priority msg to FW

DEPENDENCIES
  None

PARAMETERS
  rx & tx priority

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_dsda_priority_cfg_msg 
( 
  uint32 rx_priority,

  uint32 tx_priority
);
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_DSDX_FREQUENCY_CFG_MSG

DESCRIPTION
  Send DSDA frequency ID list to FW

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_send_dsdx_frequency_cfg_msg ( void );

#ifdef FEATURE_HDR_QTA
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_QTA_CFG_MSG

DESCRIPTION
  Send QTA start/stop msg to FW

DEPENDENCIES
  None

PARAMETERS
  State to send to FW

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_qta_cfg_msg 
( 
   hdrmultirat_qta_state_enum_type state 
);

#ifdef FEATURE_HDR_QTA_THRU_TRM
/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_QTA_STATUS_QUERY_MSG

DESCRIPTION
  Sends QTA status query msg to FW

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_send_qta_status_query_msg ();
#endif /* FEATURE_HDR_QTA_THRU_TRM */
#endif /* FEATURE_HDR_QTA */

#endif /* FEATURE_HDR_MODEM_COEXISTENCE_FW */

/*===========================================================================

FUNCTION HDRMULTIRAT_ENABLE_RX_FW_LOGGING

DESCRIPTION
  Send enable/disable rx logging msg to FW

DEPENDENCIES
  None

PARAMETERS
  None
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_enable_rx_fw_logging ( void );

/*===========================================================================

FUNCTION HDRMULTIRAT_ENABLE_TX_FW_LOGGING

DESCRIPTION
  Send enable TX logging msg to FW

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/

void hdrmultirat_enable_tx_fw_logging ( void );

/*===========================================================================

FUNCTION HDRMULTIRAT_DO_ATOMIC_SEARCH

DESCRIPTION
  This function retruns whether to do atomic search or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if to do atomic search
  FALSE: if we can reschedule searches.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmultirat_do_atomic_search( void );

/*=========================================================================

FUNCTION     : HDRMULTIRAT_SEND_MSG

DESCRIPTION  : This function sends the message to MSGR.

DEPENDENCIES : None

INPUTS       : msg - The message pointer
               len - The message length 

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrmultirat_send_msg
( 
  void                       *msg,
    /* Message to send */

  uint32                     len
    /* The message lenght */
);

#ifdef FEATURE_HDR_DUAL_SIM
/*===========================================================================
FUNCTION HDRMULTIRAT_STANDBY_PREF_CHGD

DESCRIPTION
  This function is called to inform HDRSRCH of standby preference
  has changed. 

DEPENDENCIES
  This function must be called when HDR is offline.
 
PARAMETERS 
  ds_pref - standby preference
 
RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmultirat_standby_pref_chgd
(
  sys_modem_dual_standby_pref_e_type ds_pref /* New standby preferece */
);

/*===========================================================================
FUNCTION HDRMULTIRAT_IS_DSDS_ENABLED

DESCRIPTION
  This function returns if DSDS is enabled currently

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  True - if DSDS is enabled
  False - if DSDS is disabled

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmultirat_is_dsds_enabled( void );

/*===========================================================================
FUNCTION HDRMULTIRAT_IS_DSDA_ENABLED

DESCRIPTION
  This function returns if DSDA enabled currently

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  True - if DSDA is enabled
  False - if DSDA is disabled

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmultirat_is_dsda_enabled( void );

#ifdef FEATURE_HDR_DR_DSDS
/*===========================================================================
FUNCTION HDRMULTIRAT_DR_MODE_CHANGE

DESCRIPTION
  This function is called to inform HDRSRCH DR-DSDS <-> DSDS mode changed.

DEPENDENCIES
  None
 
PARAMETERS 
  hdrmultirat_mode_pref_enum_type
 
RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmultirat_dr_mode_change
(
  hdrmultirat_mode_pref_enum_type mode
);

/*===========================================================================
FUNCTION HDRMULTIRAT_IS_DR_DSDS_ENABLED

DESCRIPTION
  This function returns if DR-DSDS is enabled currently

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  True - if DR-DSDS is enabled
  False - if DR-DSDS is disabled

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmultirat_is_dr_dsds_enabled( void );

/*===========================================================================
FUNCTION HDRMULTIRAT_IS_RF_TX_CAPABLE

DESCRIPTION
  This function checks with TRM if the chain we have is TX capable.

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  True - if we currently own TX capable chain
  False - if we currently own RX only chain

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmultirat_is_rf_tx_capable( void );

#endif /* FEATURE_HDR_DR_DSDS */

#endif /* FEATURE_HDR_DUAL_SIM */

#ifdef FEATURE_HDR_BAND_AVOIDANCE
/*===========================================================================

FUNCTION HDRMULTIRAT_PROCESS_BAND_AVOID_IND

DESCRIPTION
  Process the band avoidance indication from MCS.
  Currently for DO, no implementation is required to
  force handover due to band avoidance.
  Required to send NACK when MCS expects a response to perform Band Avoidance

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_process_band_avoid_ind ( void );

/*===========================================================================

FUNCTION HDRMULTIRAT_SEND_BA_FREQ_MSG

DESCRIPTION
  This function sends band-chan change msg to MCS for Band Avoidance.
  Tech L1 must only send serving frequency in CONNECTED mode
  Tech L1 must send empty serving frequency otherwise


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmultirat_send_ba_freq_msg( void );

/*==========================================================================

FUNCTION        HDRMULTIRAT_INIT_BA_DATA

DESCRIPTION
  This function intializes counts for band avoindace power reporting

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrmultirat_init_ba_data( void );

/*==========================================================================

FUNCTION        HDRMULTIRAT_SAVE_BA_POWER

DESCRIPTION
  This function saves power for band avoidance.
 
  The expectation for filtered Rx power is to select the 10th percentile in
  a sampling over two seconds (the frequency at which the message is to
  be sent). From an implementation perspective, this means that the tech
  must maintain a sorted sampling over these two seconds.
  For example, if 20 samples are taken,
  the tech would report (.10)*20 = 2nd entry in the sorted list.

  The filtered Tx power should be 90th percentile over time.
 
  Algortihm to obtain rx/tx power:

  Save power sample every 20ms.
  When we have 100 samples, do a sort on the array and grab the 10th sample.
  We want the low side of the list for Rx (10% up from the smallest value)
  and the high side of the list for Tx (10% down from the largest value).
  

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
errno_enum_type hdrmultirat_save_ba_power( void );

#endif /* FEATURE_HDR_BAND_AVOIDANCE */


#endif /* hdrmultirat_H */

