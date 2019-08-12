#ifndef SDLOG_H
#define SDLOG_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 S Y S T E M   D E T E R M I N A T I O N

                  L O G G I N G   H E A D E R   F I L E

GENERAL DESCRIPTION
  This file provides the log type definitions for SD20.

EXTERNALIZED FUNCTIONS
  None

REGIONAL FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdlog.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/21/11   gm      New log event for SS-timer activity
07/26/10   gm      Enhancement to avoidance/ESS event reporting
05/19/10   gm      Enums for logging avoidance/ESS algroithm
07/10/09   np      Adding LTE Code
02/02/08   ks      Porting SD code to support QDSP 6 based compiler
11/02/06   pk      Added support for SD_LOG_STATE_PWR_SAVE
11/24/04   sj      Added HDR acc_term in event report.
03/29/04   jqi     Increased size of band_pref from word type to dword type.
04/28/03   RC      Added support for event logging.
02/12/02   RC      Moved sd_log_state_e_type from sd.c into this file.
02/08/02   mpa     Renamed uati[16] to sector_id[16].
12/13/01   mpa     Created Module

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "log.h"

/* EJECT */
/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=========================================================================*/

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/* Enum for logging an abstracted state of System Determination.
*/
typedef enum
{

  SD_LOG_STATE_ACQUISITION      = 0,
  SD_LOG_STATE_ALT_ACQUISITION  = 1,
  SD_LOG_STATE_REACQUISITION    = 2,
  SD_LOG_STATE_OPERATION        = 3,
  SD_LOG_STATE_PWR_SAVE         = 4,
  SD_LOG_STATE_MAX

} sd_log_state_e_type;



#ifdef FEATURE_SD20_LOGGING


/*---------------------------------------------------------------------------
                     LOG_SD20_LAST_ACTION_C
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_SD20_LAST_ACTION_C)

  /* Last Action */

  byte act;
    /* sd_ss_act_e_type from sd.h */

  byte acq_band;
    /* If action is acquire, what band */

  word acq_channel;
    /* If action is acquire, what channel (for AMPS, 0=A, 1=B) */

  PACKED union PACKED_POST
  {
    PACKED struct PACKED_POST
    {
      byte blksys;                /* what CDMA block/system */
      byte is_new_mode_or_blksys; /* new mode or block/system? */
    } acq_cdma;

    PACKED struct PACKED_POST
    {
      byte is_amps_2nd_cch;    /* should use strongest or second? */
      word max_redirect_delay; /* Registration randomization backoff */
    } acq_amps;

    PACKED struct PACKED_POST
    {
      byte meas_type; /* RX Power or Pilot? */
      word meas_id;   /* measurement ID */
    } ded_meas;
  } act_info;

  /* System Determination State */

  byte sd_state;  /* Acquisition, Alt Acquisition, Reacquisition, Operation */
  byte srv_is_on; /* If we have a serving system */

  PACKED struct PACKED_POST
  {
    byte is_redir       : 1; /* If we are currently handling redirection */
    byte is_ho          : 1; /* If we are in handoff between systems */
    byte is_reselection : 1; /* If the reselection timer is active */
    byte reserved       : 5; /* Reserved Bits */
  } sd_state_flags;

  /* Current Serving System (valid if srv_is_on == TRUE) */
  byte mode;    /* AMPS, CDMA, or HDR */
  byte band;    /* Cellular, PCS, KPCS, JCDMA */
  byte roam;    /* On, off, or flashing roam indication */
  word channel; /* channel (for AMPS, 0=A, 1=B) */

  /* Variable length based on mode.  If HDR, 16 bytes, otherwise 4 bytes */
  PACKED union PACKED_POST
  {
    PACKED struct PACKED_POST
    {
      word sid; /* SID of CDMA or AMPS system */
      word nid; /* NID of CDMA system */
    } sid_nid;

    byte sector_id[16]; /* SectorID of HDR system.  Could save 12 bytes... */
  } system_id;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                 LOG_SD20_LAST_ACTION_HYBRID_C
---------------------------------------------------------------------------*/

/* Log type for hybrid is the same as for the standard one */
typedef LOG_SD20_LAST_ACTION_C_type LOG_SD20_LAST_ACTION_HYBRID_C_type;



#endif /* FEATURE_SD20_LOGGING */


/*---------------------------------------------------------------------------
                               SD EVENTS
---------------------------------------------------------------------------*/


/* Enumeration of SS-Actions to be followed by the SS-Client upon
** an SS-Indication function call return.
*/
typedef enum {

  SDLOG_SS_ACT_CONTINUE    = 0,
                        /* Continue with current mode of operation and
                        ** follow the actions that are specified by the
                        ** "continue" parameters */

  SDLOG_SS_ACT_ACQ_CDMA    = 1,
                        /* Attempt acquiring a CDMA system as specified by
                        ** the "CDMA acquisition" parameters */

  SDLOG_SS_ACT_ACQ_AMPS    = 2,
                        /* Attempt acquiring an AMPS system as specified by
                        ** the "AMPS acquisition" parameters */

  SDLOG_SS_ACT_ACQ_HDR     = 3,
                        /* Attempt acquiring an HDR system as specified by
                        ** the "HDR acquisition" parameters */


  SDLOG_SS_ACT_MEAS_DED    = 4,
                        /* Change to initialization state and request a
                        ** dedicated mode channel strength measurement from
                        ** SRCH. When SRCH completes the measurement report,
                        ** call on the SD_SS_IND_MISC_MEAS_RPRT SS-Indication
                        ** function */

  SDLOG_SS_ACT_MEAS_BACK   = 5,
                        /* Stay in current state and request a background
                        ** mode channel strength measurement from SRCH. When
                        ** SRCH completes the measurement report, call on the
                        ** SD_SS_IND_MISC_MEAS_RPRT SS-Indication function */

  SDLOG_SS_ACT_PWR_SAVE    = 6,
                        /* Enter Power-Save mode of operation. Once it has
                        ** actually entered Power-Save mode the SS-Client
                        ** should call on the SD_SS_IND_MISC_PWR_SAVE_ENTER
                        ** SS-Indication function */

  SDLOG_SS_ACT_ACCEPT      = 7,
                        /* Accept BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This SS-Action is only valid for
                        ** Redirection, Channel Assignment or Handoff
                        ** Direction */

  SDLOG_SS_ACT_REJECT      = 8,
                        /* Reject BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This SS-Action is only valid for
                        ** Redirection, Channel Assignment or Handoff
                        ** Direction */

  SDLOG_SS_ACT_RELEASE     = 9,
                        /* Release call. This SS-Action is only valid for
                        ** ITSPM SS-Indication */

  SDLOG_SS_ACT_ACQ_GPS     = 10,
                        /* Attempt acquiring GPS mode. */

  SDLOG_SS_ACT_ACQ_GW      = 11,
                        /* Attempt acquiring a GSM/WCDMA system as specified by
                        ** the "GW acquisition" parameters */

  SDLOG_SS_ACT_GET_NET_GW  = 12,
                        /* Get the available GSM and/or WCDMA systems. */

/*  SDLOG_SS_ACT_PWR_SCAN_GW = 13, */
                        /* Do power scan on GSM and/or WCDMA systems. */
                        /* Obsolete */

  SDLOG_SS_ACT_MAX
} sdlog_ss_act_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for logging events that are reported to SD by the SS-Client and the
** corresponding returned SD actions.
*/
typedef struct {

  /* >>>> SS-Event, as defined by sdss_evt_e_type, being reported to SD.
  ** sdss_evt_e_type is defined in sdss.h
  */
  word    ss_event;

  /* <<<< SS-Action, as defined by sdlog_ss_act_e_type.
  **
  ** 0 = SDLOG_SS_ACT_CONTINUE
  ** 1 = SDLOG_SS_ACT_ACQ_CDMA
  ** 2 = SDLOG_SS_ACT_ACQ_AMPS
  ** 3 = SDLOG_SS_ACT_ACQ_HDR
  ** 4 = SDLOG_SS_ACT_MEAS_DED
  ** 5 = SDLOG_SS_ACT_MEAS_BACK
  ** 6 = SDLOG_SS_ACT_PWR_SAVE
  ** 7 = SDLOG_SS_ACT_ACCEPT
  ** 8 = SDLOG_SS_ACT_REJECT
  ** 9 = SDLOG_SS_ACT_RELEASE
  ** 10 = SDLOG_SS_ACT_ACQ_GPS
  ** 11 = SDLOG_SS_ACT_ACQ_GW
  ** 12 = SDLOG_SS_ACT_GET_NET_GW
  */
  byte    ss_act;

  /* The current SD state, as defined by sd_log_state_e_type.
  ** 0 = ACQUISITION
  ** 1 = ALT_ACQUISITION
  ** 2 = REACQUISITION
  ** 3 = OPERATION
  */
  byte    sd_state;

  /*- - - - - - - - - - - - - - - offset 4  - - - - - - - - - - - - - - - -*/

  /* SS-Action's parameters
  */
  union
  {
    /* CONTINUE parameters.
    */
    struct
    {
      /* SS-Timer. 0 indicates timer is inactive. While MS is on paging
      ** channel, indicates the time in seconds for reselection. While MS is
      ** in power-save, indicate the time in seconds for wake-up.
      */
      word    ss_timer;

      byte    reserved[2];

    } cont;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* CDMA acquisition parameters.
    */
    struct
    {
      /* The band class of the system to acquire.
      */
      byte    band;

      /* The PCS frequency block/cellular system of the system to acquire as
      ** defined sd_blksys_e_type.
      ** 0-5  = PCS block A-F respectively
      ** 6    = Cellular sys A
      ** 7    = Cellular sys B
      */
      byte    blk_sys;

      /* The CDMA channel of the system to acquire.
      */
      word    chan;

    } acq_cdma;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* AMPS acquisition parameters.
    */
    struct
    {
      /* The band class of the system to acquire.
      */
      byte    band;

      /* The cellular system to be acquired 0=A, 1=B.
      */
      byte    sys;

      /* AMPS 2nd strongest CCH - indicate whether AMPS 2nd strongest control
      ** channel (as apposed to strongest control channel) is to be attempted
      ** acquisition 0=Strongest, 1 or greater=2nd strongest.
      */
      byte    is_amps_2nd_cch;

      /* Maximum redirect delay. When the BS redirects the MS from a CDMA
      ** system to an AMPS system (using GSRDM), this field indicate the
      ** maximum redirect delay time, in units of 8 second increments, to be
      ** used in the AMPS registration randomization algorithm.
      **
      ** A value of 0xFF indicates that this field should not be used to
      ** determine the AMPS registration time.
      */
      byte    max_redirect_delay;

    } acq_amps;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* HDR acquisition parameters.
    */
    struct
    {
      /* The band class of the system to acquire.
      */
      byte    band;

      /* HDR acquisition mode as defined by sd_hdr_acq_mode_e_type.
      ** 0 = Only acq if raw power is high enough.
      ** 1 = Micro acquisition
      ** 2 = Shallow acquisition
      ** 3 = Deep acquisition
      ** 4 = Shallow and deep acquisition
      */
      byte    acq_mode;

      /* The HDR channel of the system to acquire.
      */
      word    chan;

    } acq_hdr;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* GSM/WCDMA (GW) acquisition parameters.
    */
    struct
    {
      /* The PLMN of the system to acquire.
      */
      byte    plmn[3];

      /* Network selection mode as defined by
      ** sys_network_selection_mode_e_type.
      ** 0 = AUTOMATIC
      ** 1 = MANUAL
      ** 2 = LIMITED
      */
      byte    net_sel_mode;

    } acq_gwl;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Get network GW parameters.
    */
    struct
    {
      /* Mode preference as defined by sd_ss_mode_pref_e_type.
      */
      byte  mode_pref;

      /* Service domain preference for the request as defined by
      ** sd_ss_srv_domain_pref_e_type.
      ** 0 = Circuit switched service only.
      ** 1 = Packet switched service only.
      ** 2 = Circuit and packet switched service.
      */
      byte  srv_domain_pref;

      byte  reserved[2];

    } get_net_gwl;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } act_prm;

  /*- - - - - - - - - - - - - - - offset 8 - - - - - - - - - - - - - - - -*/

  /* SS-Event's parameters. Note that this field only included when the event
  ** has associated parameters. Specifically this field is being included
  ** when "ss_event" above is one of the following:
  ** SDSS_EVT_USER_SS_PREF=5
  ** SDSS_EVT_CDMA_OPR_ACC_TERM=413
  */
  union
  {
    /* Parameters that comes with System Selection Preference
    ** (ss_event=SDSS_EVT_USER_SS_PREF=5) event.
    */
    struct {

      /* Reason as defined by sd_ss_pref_reas_e_type.
      ** 0 = User changed the SS-Preference.
      ** 1 = Origination starts.
      ** 2 = Normal silent redial.
      ** 3 = Optimized silent redial.
      ** 4 = Origination ends.
      ** 5 = Avoid system.
      ** 6 = User initiated reselection.
      */
      byte reason;

      /* Origination mode as defined by sd_ss_orig_mode_e_type.
      ** 0 = Not in call origination.
      ** 1 = Normal call origination (call other than OTASP or emergency).
      ** 2 = Collocated system origination (operate on collocated system).
      ** 3 = OTASP call origination.
      ** 4 = Emergency call origination.
      ** 5 = Emergency callback mode.
      ** 6 = Manual system selection mode.
      */
      byte orig_mode;

      /* Mode preference as defined by sd_ss_mode_pref_e_type (sd.h).
      */
      word mode_pref;

      /* Band preference as defined by sd_ss_band_pref_e_type (sd.h).
      */
      dword band_pref;

      /* Roam preference as defined by sd_ss_roam_pref_e_type (sd.h).
      */
      byte roam_pref;

      /* Hybrid preference as defined by sd_ss_hybr_pref_e_type (sd.h).
      */
      byte hybr_pref;

    } ss_pref;

    /* Parameters that comes with System Selection Preference
    ** (ss_event=SDSS_EVT_CDMA_OPR_ACC_TERM or
    **  ss_event=SDSS_EVT_HDR_OPR_ACC_TERM) event.
    */
    struct {

      /* Access reason as defined by sd_ss_acc_reas_e_type.
      ** 0 = Registration.
      ** 1 = Origination.
      ** 2 = Page response.
      ** 3 = Data burst.
      ** 4 = Reason other than the above.
      */
      byte reason;

      /* Access termination as defined by sd_ss_acc_term_e_type.
      ** 0 = Access terminates due to failing access persistence test.
      ** 1 = Access terminates due to access state timeout.
      ** 2 = Access terminates due to max access probes being transmitted.
      ** 3 = Access terminates due to receiving reorder order.
      ** 4 = Access terminates due to receiving intercept order.
      ** 5 = Access terminates due to receiving release order.
      ** 6 = Access attempt terminates due to user release.
      ** 7 = Received HDR connection deny, reason = general.
      ** 8 = Received HDR connection deny, reason = network busy.
      ** 9 = Received HDR connection deny, reason = authentication/billing.
      ** 10 = Access terminates due to failure other than one of the above.
      ** 11 = Access attempt terminates due to normal access completion.
      */
      byte term;

      byte reserved[6];

    } acc_term;

  } event_prm;

} sdlog_event_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define SDLOG_INVALID_TIME    0
#define SDLOG_INVALID_COUNT   0x100

/* Enums for Avoidance/ESS algorithm logging support
*/
typedef enum
{
  SDLOG_AVOID_REPORT_NONE           = 0,
  SDLOG_AVOID_REPORT_ENTER          = 1,
  SDLOG_AVOID_REPORT_EXIT           = 2,
  SDLOG_AVOID_REPORT_REG            = 3,
  SDLOG_AVOID_REPORT_ABSR_ATTEMPT   = 4,
  SDLOG_AVOID_REPORT_AOOS_ATTEMPT   = 5,
  SDLOG_AVOID_REPORT_OSR_ATTEMPT    = 6,
  SDLOG_AVOID_REPORT_ACQ_CNT_CHGD   = 7,
  SDLOG_AVOID_REPORT_MAX

} sdlog_avoid_report_e_type;


/* Enums for avoidance type
*/
typedef enum
{
  SDLOG_AVOID_TYPE_UNTIL_NEW_ACQ                  = 0,
  SDLOG_AVOID_TYPE_TIMED                          = 1,
  SDLOG_AVOID_TYPE_UNTIL_ACQ_FAIL                 = 2,
  SDLOG_AVOID_TYPE_UNTIL_ACQ_FAIL_DUR_CALL_ORIG   = 3,
  SDLOG_AVOID_TYPE_MAP_UNTIL_ACC_SUCCESS          = 4,
  SDLOG_AVOID_TYPE_UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL = 5,
  SDLOG_AVOID_TYPE_CANCEL_ALL                     = 6,
  SDLOG_AVOID_TYPE_MAX

} sdlog_avoid_type_e_type;


/* Type of the payload for avoidance/ESS algorithm logging
*/
typedef struct
{

  /* defined by sdlog_avoid_report_e_type
  */
  byte avoid_report;

  /* Defined by sd_ss_e_type
  ** 0 = SD_SS_MAIN
  ** 1 = SD_SS_HYBR_1
  ** 2 = SD_SS_HYBR_WLAN
  */
  byte ss_type;

  /* Mode, as defined by sd_mode_e_type, of a avoiding system.
  ** 0 = No service.
  ** 1 = AMPS
  ** 2 = CDMA
  ** 3 = HDR
  ** 4 = GPS
  ** 5 = GSM
  ** 6 = WCDMA mode
  */
  byte    mode;

  byte    reserved_1[1];

  /*- - - - - - - - - - - - - - offset 4  - - - - - - - - - - - - - - - - -*/

  union {

    struct {

      /* defined by sd_band_e_type, Band class of a avoiding
      */
      byte band;

      byte reserved_2[1];

      /* defined by sd_chan_type, the CDMA Channel system
      ** (0=A-Sys/1=B-Sys) of a avoiding system.
      */
      word chan;

    } cdma_rec;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    struct {

      /* defined by sd_band_e_type, Band class of a avoiding
      */
      byte band;

      byte reserved_2[1];

      /* defined by sd_chan_type, the HDR Channel system of a avoiding
      ** system.
      */
      word chan;

    } hdr_rec;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } rec;

  /*- - - - - - - - - - - - - - offset 8  - - - - - - - - - - - - - - - - -*/

  /* defined by sdlog_avoid_type_e_type
  */
  byte avoid_type;

  /* Current ABSR count in a BSR
  */
  byte cur_count;

  /* The MAP avoidance better service reselection count
  */
  byte absr_count;

  /* Defined by sys_reg_status_e
  ** 0= SYS_REG_STATUS_UNKNOWN
  ** 1= SYS_REG_STATUS_INVALID
  ** 2= SYS_REG_STATUS_VALID
  */
  byte reg_status;

  /* For avoid_type = SDLOG_AVOID_TYPE_UNTIL_NEW_ACQ,
  ** if invalid its value is SDLOG_INVALID_COUNT
  */
  uint2 new_acq_cnt;

  byte  reserved_3[2];

  /* For avoid_type != SDLOG_AVOID_TYPE_UNTIL_NEW_ACQ
  ** if invalid its value is SDLOG_INVALID_TIME
  */
  dword exp_uptime;

  /* ABSR interval is derived from Tbsr/Tabsr_count_per_bs,
  ** if invalid its value is SDLOG_INVALID_TIME
  */
  dword absr_interval;

} sdlog_avoid_event_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enums for indicating ss-timer activity
*/
typedef enum
{
  SDLOG_SS_TIMER_INVALID  = 0,

  SDLOG_SS_TIMER_STARTED  = 1,
    /* ss-timer has just started by ss-scripts */
  
  SDLOG_SS_TIMER_EXPIRED  = 2,
    /* previously started ss-timer has expired */
  
  SDLOG_SS_TIMER_ALTERED  = 3,
    /* previously started ss-timer is altered */
  
  SDLOG_SS_TIMER_DISABLED = 4,
    /* previously started ss-timer is disabled */
  
  SDLOG_SS_TIMER_MAX
  
}sdlog_ss_timer_status_e_type;

/* Enums for reason to start ss-timer
*/
typedef enum
{
  SDLOG_SS_TIMER_REASON_INVALID    = 0,

  SDLOG_SS_TIMER_REASON_WAIT_TIME  = 1,
    /* Generic wait timer introduced in the script */
  
  SDLOG_SS_TIMER_REASON_WAIT_REDIR = 2,
    /* Timer to wait after redirection is recvd */
  
  SDLOG_SS_TIMER_REASON_EOOS_SLEEP = 3,
    /* Timer to for EOOS Sleep */
  
  SDLOG_SS_TIMER_REASON_ABSR       = 4,
    /* Timer for ABSR */
  
  SDLOG_SS_TIMER_REASON_WAIT_COND  = 5,
    /* Timer for conditional wait */
  
  SDLOG_SS_TIMER_REASON_BSR        = 6,
    /* Timer for BSR */
  
  SDLOG_SS_TIMER_REASON_MAX
  
}sdog_ss_timer_reason_e_type;

/* Payload for ss-timer event
*/
typedef struct
{
  /*- - - - - - - - - - - - - - offset 0, size=1 byte  - - - - - - - - - -*/
  byte ss_type;
    /* ss type for which timer is started. Values defined in sd_ss_e_type
    ** 0 = SD_SS_MAIN
    ** 1 = SD_SS_HYBR_1
    ** 2 = SD_SS_HYBR_WLAN
    ** 3 = SD_SS_HYBR_2 
    ** 4 = SD_SS_MAX
    */

  /*- - - - - - - - - - - - - - offset 1, size=1 byte  - - - - - - - - - -*/    
  byte timer_status;
    /* Current status of the timer,
    ** values defined in sdlog_ss_timer_status_e_type
    */

  /*- - - - - - - - - - - - - - offset 2, size=1 byte  - - - - - - - - - -*/    
  byte start_reason;
    /* Reason for starting the timer,
    ** values defined in sdog_ss_timer_reason_e_type
    */
  
  byte pad_byte1;

  /*- - - - - - - - - - - - - - offset 4, size=4 bytes - - - - - - - - - -*/
  dword cur_uptime;
    /* Current uptime */

  /*- - - - - - - - - - - - - - offset 8, size=4 bytes - - - - - - - - - -*/
  dword exp_uptime;
    /* expiry uptime if status is active */
    
}sdlog_ss_timer_event_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for logging Service Indicator (SI) changes.
*/
typedef struct {

  /* System determination state as defined by sd_log_state_e_type.
  ** 0 = ACQUISITION
  ** 1 = ALT_ACQUISITION
  ** 2 = REACQUISITION
  ** 3 = OPERATION
  ** 4 = PWR_SAVE
  */
  byte    sd_state;

  /* Indicates the current service status as defined by
  ** sys_srv_status_e_type
  **
  ** 0 = No service
  ** 1 = Limited service.
  ** 2 = Full service.
  **
  ** NOTE! When the value of this field is = 0 the value of all the fields
  ** that follows this field is undefined.
  */
  byte    srv_status;

  /* Serving system information (valid if srv_status != 0).
  */

  /* Mode, as defined by sd_mode_e_type, of the serving system.
  ** 0 = No service.
  ** 1 = AMPS
  ** 2 = CDMA
  ** 3 = HDR
  ** 4 = GPS
  ** 5 = GSM
  ** 6 = WCDMA mode
  */
  byte    mode;

  /* Band class of the serving system.
  */
  byte    band;

  /*- - - - - - - - - - - - - - - offset 4  - - - - - - - - - - - - - - - -*/

  /* The CDMA Channel/AMPS system (0=A-Sys/1=B-Sys) of the serving system.
  */
  word    chan;

  /* Roam indicator, as defined by TSB 58, of the serving system.
  ** 0 = Roaming indicator is OFF.
  ** 1 = Roaming indicator is ON.
  ** 2 = Roaming indicator is flashing
  ** 3-256 = As defined by TSB 58.
  */
  byte    roam;

  /* PRL designation of the serving system as defined by sdss_prl_e_type.
  ** 0 = Negative system.
  ** 1 = Unavailable system (not in PRL and available systems are forbidden).
  ** 2 = Available system (not in PRL and available systems are allowed).
  ** 3 = Preferred, but not the most preferred system in its GEO
  ** 4 = Most preferred system in its GEO
  */
  byte    prl_desig;

  /*- - - - - - - - - - - - - - - offset 8  - - - - - - - - - - - - - - - -*/

  /* System ID.
  */
  union
  {
    /* SID and NID of a CDMA or AMPS system.
    */
    struct {
      word sid;
      word nid;
    } cdma;

    /* 4 LSBs of the SectorID of an HDR system.
    */
    struct {
      byte  sector_id[4];
    } hdr;

    /* PLMN of a GSM or WCDMA system.
    */
    struct
    {
      byte  plmn[3];
      byte  reserved[1];
    } gw;

  } sys_id;

  /*- - - - - - - - - - - - - - - offset 12 - - - - - - - - - - - - - - - -*/

  /* Redirection indicator as specified by sd_ss_redir_e_type.
  ** 0 = Not in redirection.
  ** 1 = CDMA Directed Service Redirection.
  ** 2 = CDMA Global Service Redirection.
  ** 3 = CDMA NDSS Redirection.
  ** 4 = HDR QC Redirection.
  ** 5 = HDR Directed Redirection.
  */
  byte    redir;

  /* Handoff/channel assignment as specified by sd_ss_ho_e_type.
  ** 0 = Not in handoff.
  ** 1 = CDMA channel assignment to CDMA paging channel.
  ** 2 = CDMA channel assignment to CDMA traffic channel.
  ** 3 = CDMA handoff while in CDMA traffic.

  ** 4 = CDMA channel assignment to AMPS system (A/B).
  ** 5 = CDMA channel assignment to AMPS voice channel.
  ** 6 = CDMA handoff to AMPS voice channel.
  ** 7 = AMPS channel assignment to voice channel.
  */
  byte    handoff;

  ///* SS-Timer. 0 indicates timer is inactive. While MS is on paging channel
  //** indicates the time for reselection. While MS is in power-save,
  //** indicate the time for wake-up.
  //*/
  //word    ss_timer;

} sdlog_si_s_type;

 /* Type for BSR start event	EVENT_SD_HYBR2_BSR_START
 */
typedef struct {
  uint8 main_sys_mode;
  /* System's mode on MAIN stack
  **  0  =  No service
  **  2  =  CDMA 
  **  3  =  GSM
  **  4  =  HDR 
  **  5  = WCDMA
  **  9  = LTE
  ** 11 = TDS
  */
  uint8 main_srv_status;
  /* current service status
  **  0 =  No service
  **  1 =  Limited service
  **  2 =  Service available
  **  3 =  Limited regional service
  **  4 =  Power Save
  */

  uint8 hybr1_srv_status;
  /* current service status
  **  0 =  No service
  **  1 =  Limited service
  **  2 =  Service available
  **  3 =  Limited regional service
  **  4 =  Power Save
  */

  uint8 hybr2_srv_status;
  /* current service status
  **  0 =  No service
  **  1 =  Limited service
  **  2 =  Service available
  **  3 =  Limited regional service
  **  4 =  Power Save
  */

}sdlog_bsr_start_info_s_type;


/* Type for BSR end EVENT_SD_HYBR2_BSR_END
*/
typedef struct {
  
  uint8 bsr_status;
  /* BSR status
  **  0 =  Failure
  **  1 =  Success
  */
}sdlog_bsr_end_info_s_type;

/* Type for full service req event EVENT_FULL_SRV_EMC_CALL
*/
typedef struct {
  
  sys_modem_as_id_e_type as_id;
  /* as_id
  ** -1= SYS_MODEM_AS_ID_NONE
  **  0 =  SYS_MODEM_AS_ID_1
  **  1 =  SYS_MODEM_AS_ID_2
  **  2 =  SYS_MODEM_AS_ID_3
  */
}sdlog_full_srv_req_emerg_s_type;


#endif /* SDLOG_H */
