#ifndef HDRSRCHLOG_H
#define HDRSRCHLOG_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   S E A R C H    L O G    F U N C T I O N S

GENERAL DESCRIPTION

   This file contains the databases, typedefs, enums and defines used in
   the HDR SEARCH logging functions

EXTERNALIZED FUNCTIONS

   hdrsrchlog_reset_tune_away_info
   hdrsrchlog_update_srch_state_info
   hdrsrchlog_log_tune_away_info
   hdrsrchlog_log_tune_away_completion_info                     
   hdrsrchlog_log_conn_state_stats
   hdrsrchlog_inc_conn_state_stats_counter
   hdrsrchlog_inc_search_set_stats_counter
   hdrsrchlog_reset_conn_state_stats
   hdrsrchlog_set_serving_sect_handoff_count
   hdrsrchlog_log_search_record
   hdrsrchlog_log_eq_data
   hdrsrchlog_log_eq_ci_buffer
   hdrsrchlog_update_cc_info
   
REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/inc/hdrsrchlog.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------- 
10/27/15   vke     Added filtered DRC as part of hdrsrchlog_diag_struct
12/10/14   vko     API to retrieve serving DRC rate
11/19/14   wsh     Added DR-DSDS support for ASDiv
06/26/13   wsh     Added Selective TxD feature
05/25/12   wsh     Moved ARD logic into it's own state machine 
11/11/11   arm     HDR idle and access state diversity implemetation.
09/28/11   lyl     Added predicated DRC rate from mdsp logging.
05/31/11   cnx     Added hdrsrchlog_get_subpkt_protocol_data().
11/11/08   grl     Added parameter to hdrsrchlog_update_cc_info().
09/25/08   rkc     Added hdrsrchlog_srch_diagnostics_pilot_sets().
05/09/08   rkc     Added hdrsrchlog_log_mc_search_record()
02/26/08   jyw     Added multiple carrier pilot sets logging.
01/14/08   etv     Refactored the following log pkts for Artemis interface.
                   1. Rel-0 DRC-ARQ Buffer log packet.
                   2. Rev-A DRC-ARQ-DSC Buffer log packet.
                   3. Air link Summary log packet.
                   4. Equalizer C2I log packet.
12/21/07   sb      Added support for multicarrier equalizer data log packet.
09/27/07   grl     Simplified sleep_info logging interface for Rev A.
08/19/07   grl     Resolved Lint High warnings.
10/13/05   ds      Added DRC-ARQ-DSC log support
07/11/05   ds      Updated function header comment 
06/03/05   ds      Added equalizer related logging records
03/18/05   grl     Fixed serving sector handoff logging.
11/29/04   grl     Added connected state stat logging support.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
06/24/04   sq      Added new enum definition to support log_on_demand
05/18/04   ds      Added new HDR debug display log packet 
11/11/03   sq      Added 3 new packets( acq_info, reacq_info, sleep_info)
03/11/03   ajn     Updated Copyright notice
02/08/01   aaj     Added search status log record
02/05/01   aaj     Created this file
==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrsrchsect.h"
#include "hdrsrchset.h"
#include "hdrlogi.h"
#include "hdrlog.h"

#include "timetick.h"
  /* Use time-keeping functions, driven by slow clock   */
  
#include "hdrsrchstate.h"

#include "hdrsrchard.h"
#include "stm.h"

/*==========================================================================


                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

#define HDRSRCH_STATUS_DISPLAY
 /* This feature is for a debug display log record which shows search
    status on QXDM screen */

#define HDRSRCHLOG_PKT_DECODE_FAILURE_SLOT_CNT    0xFF
  /* Value of slot count when packet decode was unsuccessful. */

typedef struct
{
  boolean   acq;
  boolean   reacq;
  boolean   sleep;
}
hdrsrchlog_trigger_struct_type;
  /* Indicates whether a particular log packet has been triggered or not  */

typedef enum
{
  HDRSRCHLOG_ACQ_INFO,
  HDRSRCHLOG_REACQ_INFO,
  HDRSRCHLOG_SLEEP_INFO
}
hdrsrchlog_log_info_type;
  /* Enum definitions for different hdrsrch log packet types              */

/* Which statistics that are being tracked to clear for logging purposes. */
typedef enum 
{
  HDRSRCHLOG_ALL_STATS,
    /* Clear all of the statistics tracked in this log packet. */

  HDRSRCHLOG_LAST_CALL_STATS,
    /* Clear only the statistics tracked related to the last call. */

} hdrsrchlog_reset_stats_type;
         
typedef struct
{
  hdrsrchlog_acq_info_struct_subtype         acq_info;
    /* Log packet payload for ACQ INFO      */

  hdrsrchlog_reacq_info_struct_subtype       reacq_info;
    /* Log packet payload for REACQ INFO    */

  hdrsrchlog_enh_sleep_info_struct_subtype   sleep_info;
    /* Log packet payload for SLEEP INFO    */

  hdrsrchlog_trigger_struct_type             triggered;
    /* trigger indication                   */

  timetick_type                              awake_timer;
    /* Timer used to measure awake duration */

  timetick_type                              reacq_timer;
    /* Timer used to measure RF warmup-tune time before reacq search      */

  timetick_type                              oos_timer;
    /* Timer used to measure how long the AT has been out of service. */

  boolean                                    oos_timer_active;
    /* Whether the oos timer is active. */

  uint32                                     time_oos_ms;
    /* Amount of time the AT was out of service. */

  boolean                                    log_on_demand;
    /* True indicates "log_on_demand" mode of logging                     */

  hdrsrchlog_tune_away_info_struct_subtype   ta_info;
    /* Keeps track of all tune away related logging information. */

  boolean                                    in_tune_away;
    /* Are we in a tune away? When we transition to the traffic
       state this will tell us whether to log a tune away packet
       or not. */

  boolean                                    is_connected;
    /* When a connected state stat is logged, we need to make 
       sure we are actually in the connected state. */

  hdrsrchlog_conn_state_stats_struct_subtype conn_state_stats;
    /* Keeps track of all connected state stat logging information. */

  uint16                                     last_asp_changed_counter;
    /* Keeps track of the last asp change counter register read from the
       MDSP so we can determine the difference between the cur value and
       the last value. */

  uint8                                      pred_drc_rate;
    /* Predicated DRC rate from mdsp logging */

  uint16                                     filt_pred_drc;
    /* Filtered predicated DRC rate */

#ifdef FEATURE_HDR_FL_DATA_ADAPTIVE_RXD
  boolean                                    drc_below_div_on_thres;
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD */

#ifdef FEATURE_HDR_SELECTIVE_TXD
  uint16                                     last_basp_sinr;
    /* Last Best ASP SINR of carier 0, used for sTxD */
#endif /* FEATURE_HDR_SELECTIVE_TXD */
}
hdrsrchlog_diag_struct_type;
  /* Global structure for collecting logging info                         */

#ifdef FEATURE_HDR_SELECTIVE_TXD
typedef struct 
{
  int rx0;
  int rx1;
    /* AGC on PRI/DIV chain */

  uint32 ratchet;
    /* rachet count within last frame */

  uint8 slots;
    /* Number of samples read */

  boolean log_miss;
    /* Possible log miss since last read */
} hdrsrchlog_txd_mdsp_info_type;
#endif /* FEATURE_HDR_SELECTIVE_TXD */

extern hdrsrchlog_diag_struct_type  hdrsrchlog;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_DEBUG_DISPLAY_RECORD

DESCRIPTION  : Logs the debug display record. It gets a log record from the 
               diag log buffer, fills it with the latest debug display 
               information and commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_debug_display_record( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_DEBUG_DISPLAY_FILL_REC

DESCRIPTION  : Fills the debug display record with the latest HDR debug 
               display information.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_debug_display_fill_rec ( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_GEN_TA_RECORD

DESCRIPTION  : Fill in the information for the general TA packet.

DEPENDENCIES : TBD

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_gen_ta_record ( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_SEARCH_RECORD

DESCRIPTION  : Logs the searcher record. It gets a log record from the diag
               log buffer, fills it with the latest searcher peak information
               and commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_search_record( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_FING_RECORD

DESCRIPTION  : Logs the finger record. It gets a log record from the diag
               log buffer, fills it with the latest finger information and
               commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_fing_record( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_PILOT_SET_RECORD

DESCRIPTION  : Logs the pilot set record. It gets a log record from the diag
               log buffer, fills it with the latest pilot set information
               and commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_pilot_set_record( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_SRCH_STATUS_RECORD

DESCRIPTION  : TBD

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_srch_status_record( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_SEARCHER_LOGS

DESCRIPTION  : This function logs the searcher dignostics information. Note
               that these functions will log data only if the log mask for
               the packet has been enabled. Log mask will be enabled if the
               QXDM screen/display which shows packet information is active
               or the user has specifically enabled logging for that packet.

INPUT        : TRUE  - Log diagnostics instantly ignoring rate governer
               FALSE - Log diagnostics at restrained rate controlled by dump
                       count

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_srch_diagnostics
(
  boolean instant
    /* 0 : log diagnostics at restrained rate controlled by dump count
       1 : log diagnostics instantly. Ignores rate governer */
);

/*=========================================================================

FUNCTION     : HDRSRCHLOG_SRCH_DIAGNOSTICS_PILOT_SETS

DESCRIPTION  : This function logs the searcher dignostics information which
               contains updated pilot sets information.  This was previously
               part of hdrsrchlog_srch_diagnostics(), but pilot sets
               information was stale, i.e., before pilot set management.  Note
               that these functions will log data only if the log mask for
               the packet has been enabled. Log mask will be enabled if the
               QXDM screen/display which shows packet information is active
               or the user has specifically enabled logging for that packet.

INPUT        : TRUE  - Log diagnostics instantly ignoring rate governer
               FALSE - Log diagnostics at restrained rate controlled by dump
                       count

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_srch_diagnostics_pilot_sets
(
  boolean instant
    /* 0 : log diagnostics at restrained rate controlled by dump count
       1 : log diagnostics instantly. Ignores rate governer */
);

/*=========================================================================

FUNCTION     : HDRSRCHLOG_PN_ROLL_LOGGING

DESCRIPTION  : Logging to be done at PN ROLL ISR context

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none
=========================================================================*/

void hdrsrchlog_pn_roll_logging( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_GET_LINK_SUMMARY_DATA

DESCRIPTION  : Fills in hdrlog_link_summary_type structure with
               one PN roll's worth of data for the Air Link Summary log
               packet.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_get_link_summary_data
(
  hdrlog_link_summary_type *link_data
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_ASP_RECORD

DESCRIPTION  : TBD

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_asp_record( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_HANDOFF_STATE_RECORD

DESCRIPTION  : TBD

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_handoff_state_record ( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_RAB_BUFFER_RECORD

DESCRIPTION  : TBD

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_rab_buffer_record ( void );


/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_HRES_TA_RECORD

DESCRIPTION  : TBD

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_hres_ta_record ( void );

/* <EJECT> */

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_ASET_CHANGE_LOG

DESCRIPTION  : Log packets which are triggered by the change in the ASET

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_aset_change_log( void );

/* <EJECT> */

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_DRC_ARQ_BUFFER_RECORD

DESCRIPTION  : Log DRC and ARQ data read per slot.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_drc_arq_buffer_record ( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_DRC_ARQ_DSC_BUFFER_RECORD

DESCRIPTION  : Log DRC, ARQ and DSC data read per slot.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_drc_arq_dsc_buffer_record ( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_INIT

DESCRIPTION  : Power-up initialization of HDRSRCH log packets

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_init( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_ACQ_INFO

DESCRIPTION  : Logs the acquisition info. It gets a log record from the diag
               log buffer, fills it with the latest acq information and
               commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_acq_info( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_REACQ_INFO

DESCRIPTION  : Logs the re-acquisition info. It gets a log record from the 
               diag log buffer, fills it with the latest acq information and
               commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_reacq_info( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_SLEEP_INFO

DESCRIPTION  : Logs the sleep info. It gets a log record from the diag
               log buffer, fills it with the latest acq information and
               commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_log_sleep_info( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_ON_DEMAND

DESCRIPTION  : Turns on log_on_demand flag before logging the packet, and
               turns off the flag after logging finishes.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_on_demand
(
  hdrsrchlog_log_info_type  log_info_type
    /* Indicates which HDRSRCH log packet interested    */
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_UPDATE_CMAC_EXPIRY_COUNT

DESCRIPTION  : This funtion will increment the cmac expiry count by 1 in 
               the sleep info log packet. This is an access function 
               provided to HDR RX.
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_update_cmac_expiry_count( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_UPDATE_CC_INFO

DESCRIPTION  : This funtion will update the CCstart2CCDec, cc_rate and
               slot_count, force_preamble, cc_mer and other fields in the sleep
               info log packet. This is an access function provided to HDR RX.
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_update_cc_info
( 
  uint16  cc_ccdec, 
    /* start of CC to CC decoding, in half slots      */

  uint8   cc_rate,
    /* control channel rate */

  uint8   slot_count,
    /* Number of slots to decode the control channel  */

  boolean force_preamble_used,
    /* Whether force preamble detection was used in this wakeup. */

  boolean inc_cc_mer,
    /* Whether to count this wakeup as a message error. */

  uint16  packet_num,
    /* The CC packet number relative to the first CC packet. */

  boolean is_subsync
    /* The current packet is a subsynchronous packet. */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_RESET_TUNE_AWAY_INFO

DESCRIPTION  : This function resets all tune away related statistics logged
               to diag.
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_reset_tune_away_info( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_UPDATE_SRCH_STATE_INFO

DESCRIPTION  : This function does the searcher state housekeeping for 
               the search logging APIs.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_update_srch_state_info
(
  hdrsrch_state_enum_type prev_state,
    /* the previous search state before the current transition. */

  hdrsrch_state_enum_type next_state
    /* the next search state we are currently transitioning to. */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_TUNE_AWAY_INFO

DESCRIPTION  : This function sends out the current tune away info log 
               packet.
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_log_tune_away_info( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_TUNE_AWAY_COMPLETION_INFO

DESCRIPTION  : This function should be called when HDR traffic actually
               resumes to send a tune_away info log packet out diag.  

               This function records the time stamp where the tune away
               completes and traffic is actually resumed and then sends
               out the log packet.
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_log_tune_away_completion_info( void );
                                                      
/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_CONN_STATE_STATS

DESCRIPTION  : Logs the connected state stats. It gets a log record
               from the diag log buffer, fills it with the latest 
               connected state info and commits the log record to
               the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_log_conn_state_stats( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_INC_CONN_STATE_STATS_COUNTER

DESCRIPTION  : This function increments the requested connected state 
               counter by the given amount.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_inc_conn_state_stats_counter
( 
  hdrsrchlog_conn_state_stats_count_type counter,
    /* The counter to increment. */

  uint16                                 inc_count
    /* How much to increment the counter by. */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_INC_SEARCH_SET_STATS_COUNTER

DESCRIPTION  : This function tracks the number of pilots in each search
               set.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_inc_search_set_stats_counter
( 
  hdrsrchset_struct_type *hdrsrchset_ptr
    /* Pointer to the search set information */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_RESET_CONN_STATE_STATS

DESCRIPTION  : This function resets the connected state stats as
               specified by the stats reset parameter.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_reset_conn_state_stats
( 
  hdrsrchlog_reset_stats_type stats_reset
    /* Which statistics to clear... all or just the ones related to the 
       last call. */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_SET_SERVING_SECT_HANDOFF_COUNT 

DESCRIPTION  : This function tracks the number of times the serving
               sector was changed via a handoff.  This function tracks
               past values of the counter to determine how many handoffs
               occurred since the last call.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchlog_set_serving_sect_handoff_count
(
  uint16  counter_value,
    /* Report the active serving pilot changed counter. */

  boolean counter_was_reset
    /* Report whether the counter was reset after reading this value. */
);

#ifdef FEATURE_HDR_EQ
/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_EQ_DATA

DESCRIPTION  : Logs the equalizer data. It gets a log record from the diag
               log buffer, fills it with the latest equalizer information and
               commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_eq_data( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_MC_EQ_DATA

DESCRIPTION  : Logs the multi carrier equalizer data. It gets a log record 
               from the diag log buffer, fills it with the latest equalizer
               information and commits the log record to diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_mc_eq_data( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_EQ_CI_BUFFER

DESCRIPTION  : Log equalizer C/I values read per slot.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_eq_ci_buffer ( void );

#endif /* FEATURE_HDR_EQ */

#ifdef FEATURE_HDR_REVB
/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_MC_PILOT_SET_RECORD

DESCRIPTION  : Logs the MC pilot set record. It gets a log record from the diag
               log buffer, fills it with the latest MC pilot set information
               and commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_mc_pilot_set_record ( void );

/*=========================================================================

FUNCTION     : HDRSRCHLOG_LOG_MC_SEARCH_RECORD

DESCRIPTION  : Logs the searcher record. It gets a log record from the diag
               log buffer, fills it with the latest searcher peak information
               and commits the log record to the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchlog_log_mc_search_record ( void );

#endif /* FEATURE_HDR_REVB */

/*=========================================================================

FUNCTION HDRPSRCHLOG_GET_SUBPKT_PROTOCOL_DATA

DESCRIPTION
  This function gets value for the fields, which are related to HDRSRCH, in
  subpacket protocol data. 

DEPENDENCIES
  None

PARAMETERS
  subpkt_ptr - pointer to the subpacket to be set

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchlog_get_subpkt_protocol_data
(
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type *subpkt_ptr
);

/*=========================================================================

FUNCTION HDRPSRCHLOG_GET_PRED_DRC_RATE

DESCRIPTION
  This function gets the value of the predicted DRC rate from MDSP logging.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  pred DRC rate

SIDE EFFECTS
  None

=========================================================================*/
uint8 hdrsrchlog_get_pred_drc_rate( void );

/*=========================================================================

FUNCTION HDRPSRCHLOG_GET_SERV_DRC_RATE

DESCRIPTION
  This function gets the value of the serving DRC rate from MDSP logging.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  serv DRC rate

SIDE EFFECTS
  None

=========================================================================*/
uint8 hdrsrchlog_get_serv_drc_rate( void );

#ifdef FEATURE_HDR_FL_DATA_ADAPTIVE_RXD
/*=========================================================================

FUNCTION HDRSRCHLOG_IS_DRC_BELOW_DIV_ON_THRES

DESCRIPTION
  This function check if the Pred DRC is below DIV ON threshold or
  above DIV OFF threshold. The most recent cross of either threshold
  determines the return value. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  -  If drc has recently fallen below HDRSRCHARD_DIV_ON_DRC
  FALSE -  If drc has recently risen above HDRSRCHARD_DIV_OFF_DRC
 

SIDE EFFECTS
  None

=========================================================================*/
boolean hdrsrchlog_is_drc_below_div_on_thres( void );

/*=========================================================================

FUNCTION HDRSRCHLOG_LOG_ARD_EVENT

DESCRIPTION
  This function logs ARD related events

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchlog_log_ard_event
( 
  hdrsrchard_event_enum_type  event,
  stm_state_type              new_state,
  uint32                      time_left,
  boolean                     div,
  uint8                       drc
);
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD */

#ifdef FEATURE_HDR_SELECTIVE_TXD
/*===========================================================================

FUNCTION     : HDRSRCHLOG_STXD_SET_MDSP_INFO_NEEDED

DESCRIPTION  : This function sets if sTxD state machine is interested 
               in receiving MDSP logging data 

DEPENDENCIES : None

RETURN VALUE : None.

SIDE EFFECTS : None

===========================================================================*/
void hdrsrchlog_stxd_set_mdsp_info_needed
(
  boolean needed
);

/*==========================================================================

FUNCTION  HDRSRCHLOG_GET_LAST_BEST_ASP_SINR

DESCRIPTION
  This function logs the TxD related event and its states
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Time since last FL dec HSTR in ms  

SIDE EFFECTS
  None

==========================================================================*/
uint16 hdrsrchlog_get_last_basp_sinr( void );

/*===========================================================================

FUNCTION     : HDRSRCHLOG_STXD_GET_MDSP_INFO

DESCRIPTION  : This function retrieves the saved MDSP info

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

===========================================================================*/
void hdrsrchlog_stxd_get_mdsp_info
( 
  hdrsrchlog_txd_mdsp_info_type* mdsp_info 
);
#endif /* FEATURE_HDR_SELECTIVE_TXD */

#endif /* HDRSRCHLOG_H */
