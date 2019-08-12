/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             H D R    L O G  FUNCTIONS

GENERAL DESCRIPTION

  This module contains general purpose logging routines
  and the sub command dispatcher for diag generated functions

EXTERNALIZED FUNCTIONS
  hdrlog_diag_cmd_dispatch() - diag subsystem command dispatcher
  hdrlog_log_fwd_pkt_snapshot_record() - log the fwd_pkt_snapshot_record pkt
  hdrlog_log_rev_pkt_snapshot_record() - log the rev_pkt_snapshot_record pkt
  hdrlog_log_debug_hex_dump_record()   - log the debug_hex_dump_record pkt

  hdrlog_on_demand_dispatch() - dispatcher will log packets on demand
  hdrlog_powerup_init() - register log pkts to be logged on demand
                            with diag
  hdrlog_set_dipsw_cb() - register callbacks for use with dip switches
  hdrlog_start_conn_state_stats_periodic_timer() - Start connected state
                                                   logging timer.

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrlog.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
07/29/2014   tnp     FR22375: Fix Medium compiler warnings in HDR
10/30/2013  kss/rmv  FR3272 - Modem API for Cost of Power estimate changes
12/19/2013   sat     Changed memcpy() to assignment or memscpy() (safer version).
12/12/2013   sat     Fixed an instance of wrong memset usage.
09/03/2013   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
07/31/2013   sat     Mainlining Modem Statistics Feature
05/22/2013   rmv     Added diag command to get cost of modem power estimate 
03/08/2013   rmv     Added support for stats cdma_evdo_rx0_agc [rx0_dbm], 
                     cdma_evdo_rx1_agc [rx1_dbm] 
01/08/2013   kss     Use Tx index from FW for Tx logging (rather than Rx idx)
03/20/2012   cnx     Fixed compiler errors for Dime.
03/19/2012   ljl     Fixed Dime compiler errors.
10/10/2011   arm     Supported second chain test call feature.
09/06/2011   cnx     Reverted last change.
09/06/2011   cnx     Reverted changes starting from 06/22/10 except for 
                     'Fixed the length of Diag response'.
05/31/2011   cnx     Added HDR_PROTOCOL_DATA command.
01/18/2011   cnx     Fixed the length of Diag response.   
09/16/2010   lyl     Fixed compiler error.
07/12/2010   cnx     Added Diag cmd to change diversity preference in runtime. Fixed CR 239355. 
06/22/2010   ljl     Replaced INTLOCK/INTFREE with REX_DISABLE/ENABLE_PREMPTION.
06/21/2010   cnx     Fixed compiler warnings.
06/01/2010   pba     Added support for FTD stats.
02/20/2010   kss     Initial support for FW logging on MDM9X00
09/18/2009   wsh     CMI: support changes from modem DATA
05/15/2009   wsh     Include needed headers to support diag split
04/16/2009   rkc     Mainlined T_MSM6500.
01/19/2008   etv     Fix DTX featurization issue.
01/09/2009   etv     Log if either Rx or Tx log buffer says carrier is valid.
10/01/2008   wsh     Fixed critical/high lint warnings
08/06/2008   sju     Renamed hdrlog_init() and moved it from diag to this file.
06/18/2008   etv     Added diag command to set/reset DTXMode
03/06/2008   etv     DSP gives Log timestamp in slots insteads of half-slots.
02/26/2008   jyw     Added multiple carrier pilot sets logging.
01/14/2008   etv     Simplified MDSP Slot log design by removing Event type.
12/19/2007   etv     Fixed bug by setting appropriate log event and undef'd
                     unit-test feature.
12/05/2007   etv     Added support for mDSP slot logging.
09/27/2007   grl     Added Rev A. Sleep Info pkt support.
02/12/2007   mpa     Fixed RVCT warnings.
10/16/2006   etv     Logged LOG_HDR_SUBTYPE_STREAM_CONFIG_C log pkt on demand.
10/04/2006   pba     Added HDR_WRITE_DATABASE_TO_EFS command.
07/06/2005   sk      QoS releated code featurized
07/05/2005   ds      Added support for generic MDSP logging
06/24/2005   sk      Added HDR_RST_MRLP_STATS support
04/04/2005   pba     Removed Lint errors
03/24/2005   pba     Lint cleanup
12/07/2004   mpa     Added HDR_RST_CONNECTED_STATE_STATS to the diag reg table.
11/29/2004   grl     Added support for connected state stat logging.
11/19/2004   etv     Added HDR_MCS_CLR_STATS_COMMAND
09/17/2004   kss     Fixed minor bug in hdrlog_log_debug_hex_dump_record().
09/03/2004   sq      Fixed issue related to HDRSRCH packets log_on_demand.
08/04/2004   kss     Use hdrbctest interface for enabling broadcast test app.
06/26/2004   kss     Added HDR_MCS_FLOW_COMMAND, and support to enable and
                     disable soft combining in broadcast.
06/14/2004   ss      (mpa for) Renamed API functions for BCMCS diag commands 
06/07/2004   kss     Removed hdrlog_log_mcs_chan_metrics() function.
05/13/2004   kss     Changed references to hdrbcctrl_ to hdrbccp_.
04/28/2004   kss     Added HDR MCS commands.
11/21/2003   mpa     Merged in compiler warning fixes
11/11/2003   sq      Added 3 new packets( acq_info, reacq_info, sleep_info)
09/10/2003   mpa     Converted F3 messages to use new MSG2.0 HDR PROT SSID
08/11/2003   mpa     Merged in 6500 baseline
08/01/2003   ht      Temp fixes to avoid wdog timeout when sleep is enabled
02/20/2003   lad     Updated to be compatible with MSMSHARED_DIAG.
02/19/2003   mpa     Fixed dip switch NUM boundary
02/10/2003   kss(mpa)Added hdrlog_set_dipsw_cb() and dipsw_cb_list.
12/13/2002   mpa     Do not check validity of dipsw if param not used
11/12/2002   mpa     Added HDR_DIP_SWITCH diag cmd and interface
10/15/2002   mpa     Added HDR_STATE diag cmd
06/10/2002   mpa     Only log ASP and FINGER pkts on demand when not sleeping
03/27/2002   mpa     Only log ASP packet on demand when in connected state
03/12/2002   mpa     Added equipment ID 1 for all log codes registered
                     for logging on demand.
02/08/2002   mpa     Deleted log_on_demand. Added hdrlog_on_demand_dispatch,
                     hdrlog_powerup_init and hdrlog_register_log_pkts to
                     accommodate the new diag log_on_demand.
01/21/2002   mpa     Fully featurized log_on_demand
12/03/2001   mpa     Added support for log_on_demand (temporary until the
                     diag team provides a log packet registration mechanism)
11/12/2001   mpa     Added a subsystem alloc/commit to the dummy reset
                     rsps so Diag will acknowledge that a response was sent.
10/29/2001   mpa     Updated reset cmd hooks in hdrlog_diag_cmd_reset().
                     Deleted all #if 0 blocks of code.
09/12/2001   mpa     created this file
==========================================================================*/



/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "hdrlog.h"
#include "hdrrmac.h"
#include "hdramac.h"
#include "hdrlmac.h"
#include "hdrplog.h"
#include "hdrrx.h"
#include "hdrrlp_api.h"
#include "diagpkt.h"
#include "diagcmd.h"

#ifdef FEATURE_HDR_QOS
#include "hdrmrlp_api.h"
#endif

#include "hdridles.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdrsrchlog.h"
#include "hdrrmaclog.h"
#include "hdrslpi.h"
#ifdef FEATURE_SD20_LOGGING
#include "sd.h"

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif

#endif

#include "mobile.h"
#include "hdrhai.h"
#ifndef FEATURE_HDR_DIME_MODEM
#include "hw.h"
#endif
#include "hdrdebug.h"
#include "err.h"

#include "hdrmdspmcr.h"

#include "diagbuf.h"
#include "hdralmps.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrbctest.h"
#include "hdrbclog.h"
#include "hdrbcmac.h"
#endif

#include "hdrscmdb.h"

#include "rex.h"
#include "hdrscmdb.h"
#include "hdrmci.h"
#include "hdrphy.h"
#include "hdrsrch.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

typedef enum
{
  HDRLOG_CONN_STAT_STATS_TIMER = 0,
  HDRLOG_FTD_APP_REFRESH_TIMER

} hdrlog_timer_name_enum_type;

/* Timer for peridic repors of FTD stats */
LOCAL rex_timer_type hdrlog_ftd_stats_refresh_timer;

/* How often to report FTD stats */
#define HDRLOG_FTD_STATS_REFRESH_TIMER 3000

/* Struct that defines the full set of FTD stats to be collected */
static sys_hdr_information_s_type ftd_info;
/* Bank of 32 dip switches */
hdrlog_dipsw_mask_type dipsw_mask;

/* Timer for periodic reports of connected state stats. */
LOCAL rex_timer_type hdrlog_conn_state_stats_timer;

/* How often we report connected state statistics. */
#define HDRLOG_CONN_STATE_STATS_TIMER_PERIOD 6000

/* Array for holding dip switch function callbacks.
   The array will be initialized to all zeros (NULL) automatically. */
struct
{
  void (*set)(void);
  void (*reset)(void);
} hdrlog_dipsw_cb_list[HDRLOG_DIPSW_NUM];

/* MDSP Slot logging registration type */
typedef struct
{
  hdrlog_mdsp_slot_logging_client_cb_type cb;
    /* Call back registered */
} hdrlog_mdsp_slot_logging_reg_type;

/* mDSP Slot logging Callback Registration table */
hdrlog_mdsp_slot_logging_reg_type
  hdrlog_mdsp_slcb_table [HDRLOG_MAX_NUM_MDSP_SLOT_LOGGING_CLIENTS];

/* Number of clients registered for mDSP slot based logging */
uint8 hdrlog_num_registered_clients = 0;

/* EJECT */
/*=========================================================================

FUNCTION HDRLOG_CONN_STATE_STATS_TIMER_CB

DESCRIPTION
  The connected state statistics timer which directs all tasks to do logging
  that needs to be done periodically while connected.
  
DEPENDENCIES
  None

PARAMETERS
  timer_id - Unused.

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_conn_stat_stats_timer_cb
(
  uint32 timer_id
)
{

  SYS_ARG_NOT_USED(timer_id); /* remove this line when timer_id is used */

  /* Call all connected state stats functions.  This is done in the wrong task,
   * but the data is protected by INT LOCKS.
   */
  hdrsrchlog_log_conn_state_stats();
  hdrplog_log_conn_state_stats();
  hdrrmaclog_log_conn_state_stats();
  hdrslp_log_conn_state_stats();

  /* Set the timer to log after the timer expires again. */
  (void) rex_set_timer( &hdrlog_conn_state_stats_timer, 
                 HDRLOG_CONN_STATE_STATS_TIMER_PERIOD );
} /* hdrlog_conn_stat_stats_timer_cb() */


/*===========================================================================

FUNCTION HDRLOG_GET_FTD_STATS

DESCRIPTION
  This function collects all FTD stats.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_get_ftd_stats( void )
{
  hdrhai_ati_record_type transmit_ati;
    /* UATI24 data */
  hdrlog_ftd_l1_stats_type l1_ftd_data;
    /* FTD data collected from L1 */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear the data first */
  memset ( &ftd_info, 0, sizeof(sys_hdr_information_s_type) );

  /***********************************************************************
                  Call processing LTD stats
   ************************************************************************/

  /* 1- HDR protocol state – Inactive or Init or Idle or Connected
     2- 3 LSB of the UATI
     3- Color Code for the UATI
     4- Subnet Mask for the UATI 
  */

  /* 1- HDR protocol state – Inactive or Init or Idle or Connected */
  ftd_info.hdr_prot_state = (sys_hdr_prot_state_e_type) hdralmps_get_state();

  /* 2- 3 LSB of the UATI */
  (void) hdramp_get_transmit_ati( &transmit_ati, HDRHAI_LOGGING_MODULE );

  if (transmit_ati.ati_type == HDRHAI_UATI)
  {
    ftd_info.uati_24 = transmit_ati.ati_value;
  }
  else
  {
    ftd_info.uati_24 = 0;
  }

   /* 3- Color Code for the UATI */
  (void) hdramp_get_uati_color_code ( &ftd_info.color_code );

   /* 4- Subnet Mask for the UATI */
  (void) hdramp_get_uati_subnet_mask( &ftd_info.subnet_mask );

  /***********************************************************************
                  L1 FTD stats
   ************************************************************************/

  /* 1- SINR of the serving sector in db
     2- Rx power in dbm
     3- Tx power in dbm
     4- Ec/Io of reference sector in db
     5- Current MAC index 
     6- Channel – Band and frequency information
     7- rx0_agc power in dbm
     8- rx1_agc power in dbm
   */
  hdrsrch_get_ftd_data ( &l1_ftd_data );

  ftd_info.c2i = l1_ftd_data.c2i;
  ftd_info.rx_dbm = l1_ftd_data.rx_dbm;
  ftd_info.tx_dbm = l1_ftd_data.tx_dbm;
  ftd_info.ecio = l1_ftd_data.ecio;
  ftd_info.mac_index = l1_ftd_data.mac_index;
  ftd_info.channel = l1_ftd_data.channel;
  ftd_info.cdma_evdo_rx0_agc = l1_ftd_data.rx0_dbm;
  ftd_info.cdma_evdo_rx1_agc = l1_ftd_data.rx1_dbm;

  ftd_info.changed_bitmask = SYS_HDR_INFO_MASK_ALL;

} /* hdrlog_get_ftd_stats() */

/*===========================================================================
FUNCTION HDRLOG_TIMER_CB

DESCRIPTION
  This function is the callback for all LOG state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrlog_timer_cb
(
  uint32 timer_id
)
{
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "LOG: hdrlog_timer_cb");

  hdrmc_queue_timer(HDRHAI_LOGGING_MODULE, timer_id);

} /* hdrlog_timer_cb() */

/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_RESET

DESCRIPTION
  Diagnostic command - reset command dispatcher.
  Each reset fn is associated with a reset button in QXDM.

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

LOCAL PACKED void * hdrlog_diag_cmd_reset
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  diagpkt_cmd_code_type   subsys_code;
    /* subsystem command code */

  PACKED void * rsp = NULL;
    /* response pointer */

  uint16 rsp_len = sizeof(diagpkt_subsys_header_type);
    /* length of packet response */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  subsys_code = diagpkt_subsys_get_cmd_code( req_pkt );
    /* fetch the command code */

  switch (subsys_code)
  {
    case  HDR_RST_ACCESS_CNT :
      /* cmd 0 - Rev Link Stats Display   */

      hdramac_reset_access_stats();
      break;

    case  HDR_RST_FWD_STATS :
      /* cmd 1 - Fwd Link Stats Display   */

      hdrrx_reset_dm_fwd_stats();
      break;

    case  HDR_RST_RTC_RATE_CNT :
      /* cmd 2 - Rev Link Stats Display   */

      hdrrmac_reset_dm_rtc_rate_count();
      break;

    case  HDR_RST_CONN_ATTEMPT_CNT :
      /* cmd 3 - Protocol Summary Display */

      hdrplog_reset_conn_attempt();
      break;

    case  HDR_RST_PAGE_MSG_CNT :
      /* cmd 4 - Fwd Link Stats Display   */

      hdrplog_reset_page_count();
      break;

    case  HDR_RST_SESSION_ATTEMPT_CNT :
      /* cmd 5 - Protocol Summary Display */

      hdrplog_reset_session_attempt();
      break;

    case  HDR_RST_RLP_STATS :
      /* cmd 6 - RLP Stats Display        */

      hdrrlp_clear_stats();
      break;

    case  HDR_RST_CONNECTED_STATE_STATS:
      /* cmd 13 - Connected State Stats   */

      /* Clear and then log the newly cleared statistics.
       * We clear both connected state stats, connected termination
       * statistics, and tune away stats in response to this packet. 
       */
      hdrplog_reset_conn_state_stats( HDRPLOG_ALL_STATS );
      hdrplog_log_conn_state_stats();

      hdrplog_reset_conn_term_stats( HDRPLOG_ALL_STATS );
      hdrplog_log_conn_term_stats();

      hdrrmaclog_reset_tune_away_rri_info( HDRRMACLOG_ALL_STATS );
      hdrrmaclog_log_tune_away_rri_info();

      hdrrmaclog_reset_conn_state_stats( HDRRMACLOG_ALL_STATS );
      hdrrmaclog_log_conn_state_stats();

      hdrsrchlog_reset_conn_state_stats( HDRSRCHLOG_ALL_STATS );
      hdrsrchlog_log_conn_state_stats();

      hdrsrchlog_reset_tune_away_info();
      hdrsrchlog_log_tune_away_info();

      hdrslp_log_reset_conn_state_stats( HDRSLP_ALL_STATS );
      hdrslp_log_conn_state_stats();
      break;

#ifdef FEATURE_HDR_QOS
    case  HDR_RST_MRLP_STATS :
      /* cmd 14 - MRLP Stats Display        */
      hdrmrlp_clear_stats();
      break;
#endif

    default :
      /* Invalid Otherwise */

      HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Invalid reset command code (%d)",
                      subsys_code);
      rsp = diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len);

  } /* switch subsys_code */

  if (rsp == NULL)
  {
    rsp = diagpkt_subsys_alloc(DIAG_SUBSYS_HDR, subsys_code, rsp_len);
  }

  return rsp;

} /* hdrlog_diag_cmd_reset() */

/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_AT_CONFIG

DESCRIPTION
  Diagnostic command - AT Config requestor .

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

LOCAL PACKED void * hdrlog_diag_cmd_at_config
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  int8  i;
  /* loop variable        */

  hdrdiag_at_config_rsp_type *rsp_ptr = NULL;
    /* return response pointer    */

  const unsigned int rsp_len = sizeof( hdrdiag_at_config_rsp_type );
    /* return response length   */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SYS_ARG_NOT_USED(req_pkt); /*remove this line when req_pkt is used*/

  SYS_ARG_NOT_USED(pkt_len); /*remove this line when pkt_len is used*/

  rsp_ptr = (hdrdiag_at_config_rsp_type *) 
            diagpkt_subsys_alloc( DIAG_SUBSYS_HDR,
                                  HDR_AT_CONFIG,
                                  rsp_len );


  if ( rsp_ptr )
  {
    rsp_ptr->hdr_protocol_rev      = HDRHAI_REVISION_NUMBER;
    rsp_ptr->hdr_firmware_version  = 0; //HDRMDSP_VERSION();-temp fix to avoid wdog TO
    rsp_ptr->at_model              = mob_model;
    rsp_ptr->msm_version           = 0; //hw_version();-temp fix, hw.h is deprecated


    for ( i = 0; i < HDRLOG_VERNO_DATE_STRLEN; i++ )
    {
      rsp_ptr->compile_date[i]     = ver_date[i];
    }

    for ( i = 0; i < HDRLOG_VERNO_TIME_STRLEN; i++ )
    {
      rsp_ptr->release_date[i]     = rel_date[i];
    }

    for ( i = 0; i < HDRLOG_VERNO_DIR_STRLEN; i++ )
    {
      rsp_ptr->software_version[i] = ver_dir[i];
    }
  }

  return rsp_ptr;

} /* hdrlog_diag_cmd_at_config() */


/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_STATE_INFO

DESCRIPTION
  Diagnostic command - State Info requestor.

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

LOCAL PACKED void * hdrlog_diag_cmd_state_info
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_state_rsp_type *rsp_ptr = NULL;
    /* return response pointer    */

  LOG_HDR_STATE_INFO_C_type log_ptr;
    /* get the rsp info through the mirrored log packet */

  const unsigned int rsp_len = sizeof( hdrdiag_state_rsp_type );
    /* return response length   */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SYS_ARG_NOT_USED(req_pkt); /*remove this line when req_pkt is used*/

  SYS_ARG_NOT_USED(pkt_len); /*remove this line when pkt_len is used*/

  rsp_ptr = (hdrdiag_state_rsp_type *)
    diagpkt_subsys_alloc( DIAG_SUBSYS_HDR, HDR_STATE, rsp_len );


  if ( rsp_ptr )
  {
    /* Get the updated state info from the mirror log packet */
    hdrplog_update_log_state_info(&log_ptr);

    /* Update the diag rsp structure */
    rsp_ptr->at_state      = log_ptr.at_state;
    rsp_ptr->session_state = log_ptr.session_state;
    rsp_ptr->almp_state    = log_ptr.almp_state;
    rsp_ptr->init_state    = log_ptr.init_state;
    rsp_ptr->idle_state    = log_ptr.idle_state;
    rsp_ptr->conn_state    = log_ptr.conn_state;
    rsp_ptr->rup_state     = log_ptr.rup_state;
    rsp_ptr->ovhd_state    = log_ptr.ovhd_state;
    rsp_ptr->hybrid_mode   = log_ptr.hybrid_mode;
  }

  return rsp_ptr;

} /* hdrlog_diag_cmd_state_info() */

/*=========================================================================

FUNCTION HDRLOG_GET_SUBPKT_PROTOCOL_DATA_CP

DESCRIPTION
  This function gets the fields value related to HDR CP in subpacket.
  It is a wrapper function of hdrplog_get_subpkt_protocol_data().

DEPENDENCIES
  None

PARAMETERS
  subpkt_ptr - pointer to the subpacket to be set

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
LOCAL void hdrlog_get_subpkt_protocol_data_cp
(
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type *subpkt_ptr
)
{
  hdrplog_get_subpkt_protocol_data(subpkt_ptr);
} /* hdrlog_get_subpkt_protocol_data_cp */

/*=========================================================================

FUNCTION HDRLOG_GET_SUBPKT_PROTOCOL_DATA_MAC

DESCRIPTION
  This function gets the fields value related to HDR MAC in subpacket.
  It is a wrapper function of hdrlmac_get_subpkt_protocol_data().

DEPENDENCIES
  None

PARAMETERS
  subpkt_ptr - pointer to the subpacket to be set

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
LOCAL void hdrlog_get_subpkt_protocol_data_mac
(
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type *subpkt_ptr
)
{
  hdrlmac_get_subpkt_protocol_data(subpkt_ptr);
} /* hdrlog_get_subpkt_protocol_data_mac */

/*=========================================================================

FUNCTION HDRLOG_GET_SUBPKT_PROTOCOL_DATA_SRCH

DESCRIPTION
  This function gets the fields value related to HDR SRCH in subpacket.
  It is a wrapper function of hdrsrchlog_get_subpkt_protocol_data().

DEPENDENCIES
  None

PARAMETERS
  subpkt_ptr - pointer to the subpacket to be set

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
LOCAL void hdrlog_get_subpkt_protocol_data_srch
(
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type *subpkt_ptr
)
{
  hdrsrchlog_get_subpkt_protocol_data(subpkt_ptr);
} /* hdrlog_get_subpkt_protocol_data_srch */

/*=========================================================================

FUNCTION HDRLOG_GET_SUBPKT_PROTOCOL_DATA

DESCRIPTION
  This function gets the fields value in subpacket protocol data.

DEPENDENCIES
  None

PARAMETERS
  subpkt_ptr - pointer to the subpacket to be set

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
LOCAL void hdrlog_get_subpkt_protocol_data
(
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type *subpkt_ptr
)
{
  hdrdiag_hdr_protocol_data_subpkt_header_type *header;

  /* Initialize all field with 0. */
  memset(subpkt_ptr, 0, sizeof(hdrdiag_hdr_protocol_data_subpkt_protocol_data_type));

  /* Set subpacket header */
  header = &(subpkt_ptr->subpkt_header);  
  header->subpkt_id = (uint8)HDRLOG_SUBPKT_HDR_PROTOCOL_DATA;
  header->subpkt_version = 0;
  header->subpkt_size = sizeof(hdrdiag_hdr_protocol_data_subpkt_protocol_data_type);

  /* Fill the subpacket body. */
  hdrlog_get_subpkt_protocol_data_cp(subpkt_ptr);
  hdrlog_get_subpkt_protocol_data_mac(subpkt_ptr);
  hdrlog_get_subpkt_protocol_data_srch(subpkt_ptr);
} /* hdrlog_get_subpkt_protocol_data */

/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_HDR_PROTOCOL_DATA

DESCRIPTION
  Diagnostic command - get HDR protocol data.

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

LOCAL PACKED void * hdrlog_diag_cmd_protocol_data
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_hdr_protocol_data_rsp_type *rsp_ptr = NULL;
    /* return response pointer */

  const unsigned int rsp_len = sizeof( hdrdiag_hdr_protocol_data_rsp_type );
    /* return response length */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rsp_ptr = (hdrdiag_hdr_protocol_data_rsp_type *) diagpkt_subsys_alloc( 
              DIAG_SUBSYS_HDR, HDR_PROTOCOL_DATA, rsp_len );

  if ( rsp_ptr )
  {
    rsp_ptr->gen_pkt_version = 3;
    rsp_ptr->num_subpkts  = 1;
    rsp_ptr->reserved = 0;

    hdrlog_get_subpkt_protocol_data(&(rsp_ptr->subpkt_protocol_data));

    HDR_MSG_PROT_1( MSG_LEGACY_MED, "Retrieve subpkt info successfully. packet len: %d", 
                    rsp_len );
  }

  return rsp_ptr;

} /* hdrlog_diag_cmd_protocol_data() */

#ifdef FEATURE_HDR_MODEM_API
/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_MODEM_API_DATA_REPORT

DESCRIPTION
  Diagnostic command - Retrieve the Modem API data

DEPENDENCIES
  AT should be in Idle or Connected

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

PACKED void * hdrlog_diag_cmd_modem_api_data_report
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_modem_api_data_rsp_type *rsp_ptr = NULL;
    /* return response pointer    */

  const unsigned int rsp_len = sizeof( hdrdiag_modem_api_data_rsp_type );
    /* return response length   */

  hdrlmac_modem_api_data_type modem_api_data;
    /* Modem API data gets filled in this */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SYS_ARG_NOT_USED(req_pkt); /*remove this line when req_pkt is used*/

  SYS_ARG_NOT_USED(pkt_len); /*remove this line when pkt_len is used*/

  rsp_ptr = (hdrdiag_modem_api_data_rsp_type *)
    diagpkt_subsys_alloc( DIAG_SUBSYS_HDR, HDR_MODEM_API_DATA_REPORT, rsp_len );

  if ( rsp_ptr )
  {
    /* Call the Modem API to get the Collected data */
     hdrlmac_get_modem_api_data(&modem_api_data);

     rsp_ptr->cost_of_power_est = modem_api_data.cost_of_modem_power;
  }

  return rsp_ptr;

}  /* hdrlog_diag_cmd_modem_api_data_report() */
#endif /* FEATURE_HDR_MODEM_API */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_PROCESS_DIAG_CMD_IN_HDRMC

DESCRIPTION
  This function is used in the DIAG reg table whenever the DAIG request
  needs to be processed in HDRMC task context. Normally it is processed
  in DIAG task context.
  
DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
LOCAL PACKED void * hdrlog_process_diag_cmd_in_hdrmc
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrlog_cmd_type hdrlog_cmd; 
    /* Build command into local struct */
  diagpkt_cmd_code_type subsys_code;
    /* subsystem command code */
  PACKED void * rsp = NULL;
    /* response pointer */
  uint16 rsp_len = sizeof(diagpkt_subsys_header_type);
    /* length of packet response */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* This function queues the DIAG command to HDRMC event queue for
     processing in HDRMC task context */

  /* Fetch the command code */
  subsys_code = diagpkt_subsys_get_cmd_code( req_pkt );

  HDR_MSG_PROT_1 (MSG_LEGACY_MED,"DIAG_SUBSYTEM_CMD received %d", subsys_code);
  
  /* Fill in all members of the hdrlog_cmd_type that are required for
    processing of DIAG subsytem command */
  hdrlog_cmd.name = (hdrlog_diag_subsys_cmd_enum_type)subsys_code;

  /* This function copies the hdrlog_cmd into a buffer before it returns.*/
  hdrmc_queue_cmd( HDRHAI_LOGGING_MODULE, &hdrlog_cmd, sizeof (hdrlog_cmd) );

  /* Send a response back to DIAG */
  rsp = diagpkt_subsys_alloc(DIAG_SUBSYS_HDR, subsys_code, rsp_len);

  return rsp;

} /* hdrlog_process_diag_cmd_in_hdrmc() */
/* <EJECT> */

/*=========================================================================

FUNCTION HDRLOG_IS_DIPSW_SET

DESCRIPTION
  This helper function checks to see if a dip switch is set in the mask.

DEPENDENCIES
  None

PARAMETERS
  TRUE - dip switch is set
  FALSE - dip switch is not set

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/
boolean hdrlog_is_dipsw_set( hdrlog_dipsw_enum_type dipsw )
{

  return ( ((dipsw_mask & dipsw) != 0) ? TRUE : FALSE );

} /* hdrlog_is_dip_switch_set() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_SET_DIPSW_CB

DESCRIPTION
  Installs callback functions which are called when the indicated dip
  switch is set or reset.  This allows clients to use callback functions
  to trigger events rather than polling (although polling is certainly
  supported).

  Note that the callback functions will be run in diag task context.

DEPENDENCIES
  None

PARAMETERS
  dipsw - The dip switch which is to be associated with callbacks.
  set  - Function to call when dip switch is set.
  reset - Function to call when dip switch is reset (cleared).

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_set_dipsw_cb
(
  hdrlog_dipsw_enum_type dipsw,
  void (*set)(void),
  void (*reset)(void)
)
{
  int dipsw_num=0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (dipsw_num = 0; dipsw_num < HDRLOG_DIPSW_NUM; dipsw_num++) 
  {
    if ((dipsw & (1 << dipsw_num)) != 0)
    {
      break;
    }
  }

  if (dipsw_num != HDRLOG_DIPSW_NUM)
  {  
    /* Set callbacks. */
    hdrlog_dipsw_cb_list[dipsw_num].set = set;
    hdrlog_dipsw_cb_list[dipsw_num].reset = reset;
  }

} /* hdrlog_set_dipsw_cb() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_DIP_SWITCH

DESCRIPTION
  Diagnostic command - Dip Switch Set requestor.

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

LOCAL PACKED void * hdrlog_diag_cmd_dip_switch
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_dip_switch_req_type
    *req_ptr = (hdrdiag_dip_switch_req_type *) req_pkt;
    /* copy the request pkt pointer */

  hdrdiag_dip_switch_rsp_type *rsp_ptr = NULL;
    /* return response pointer    */

  const unsigned int rsp_len = sizeof( hdrdiag_dip_switch_rsp_type );
    /* return response length   */

  hdrlog_dipsw_mask_type masked_dipsw;
    /* dip switch to be set or reset */

  hdrlog_dipsw_action_enum_type action;
    /* local copy of action to take */

  int dipsw_num;
    /* Dip switch number (not a bit mask) */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get action */
  action = (hdrlog_dipsw_action_enum_type) req_ptr->action;

  /* Get dip switch number */
  dipsw_num = req_ptr->dipsw;

  /* Convert the dip switch number to a bit mask */
  masked_dipsw = BITMASK(dipsw_num);

  /* Check request packet length */
  if (pkt_len != sizeof(hdrdiag_dip_switch_req_type))
  {
    action = HDRLOG_DIPSW_READ_MASK;
    HDR_MSG_PROT ( MSG_LEGACY_ERROR,
                  "Invalid num of args in dipsw request");
  }

  switch ( action )
  {
    case HDRLOG_DIPSW_SET:
      /* Process request only if the dip switch has been defined */
      if (masked_dipsw <= HDRLOG_DIPSW_MAX && masked_dipsw != 0)
      {
        /* Check that bit was previously set so that callback is only called
           on a 0->1 transition. */
        if ((dipsw_mask & masked_dipsw) == 0)
        {
          dipsw_mask |= masked_dipsw;

          /* Call callback if it exists. */
          if (hdrlog_dipsw_cb_list[dipsw_num].set)
          {
            hdrlog_dipsw_cb_list[dipsw_num].set();
          }
        }

      }
      break;

    case HDRLOG_DIPSW_RESET:
      /* Process request only if the dip switch has been defined */
      if (masked_dipsw <= HDRLOG_DIPSW_MAX && masked_dipsw != 0)

      {
        /* Check that bit was previously clear so that callback is only called
           on a 1->0 transition. */
        if ((dipsw_mask & masked_dipsw) != 0)
        {
          dipsw_mask &= ~masked_dipsw;

          /* Call callback if it exists. */
          if (hdrlog_dipsw_cb_list[dipsw_num].reset)
          {
            hdrlog_dipsw_cb_list[dipsw_num].reset();
          }
        }

      }
      break;

    case HDRLOG_DIPSW_CLEAR_ALL:

      /* Iterate through all dip switches. */
      for( dipsw_num = 1; dipsw_num < HDRLOG_DIPSW_NUM; dipsw_num++)
      {

        /* Convert the dip switch number to a bit mask */
        masked_dipsw = BITMASK(dipsw_num);

        /* Check that bit was previously clear so that callback is only called
           on a 1->0 transistion. */
        if ((dipsw_mask & masked_dipsw) != 0)
        {
          dipsw_mask &= ~masked_dipsw;

          /* Call callback if it exists. */
          if (hdrlog_dipsw_cb_list[dipsw_num].reset)
          {
            hdrlog_dipsw_cb_list[dipsw_num].reset();
          }
        }

      } /* for (dipsw_num ...) */

      dipsw_mask = 0;
      break;

    case HDRLOG_DIPSW_READ_MASK:
      /* No action required, just return the current mask */
      break;

    default:
      ERR("Invalid dip switch command (%d)",req_ptr->action,0,0);
      break;
   } /* switch action */

  /* Create a response */
  rsp_ptr = (hdrdiag_dip_switch_rsp_type *) 
            diagpkt_subsys_alloc( DIAG_SUBSYS_HDR,
                                  HDR_DIP_SWITCH,
                                  rsp_len );

  if ( rsp_ptr )
  {
    /* return the request in the rsp */
    rsp_ptr->action     = req_ptr->action;
    rsp_ptr->dipsw      = req_ptr->dipsw;

    /* return the dip switch mask */
    rsp_ptr->dipsw_mask = dipsw_mask;
  }

  return rsp_ptr;

} /* hdrlog_diag_cmd_dip_switch() */


#ifdef FEATURE_HDR_BCMCS

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_HDR_MCS_FLOW

DESCRIPTION
  Diagnostic command - MCS Enable/Disable test flow.

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

LOCAL PACKED void * hdrlog_diag_cmd_hdr_mcs_flow
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_hdr_mcs_flow_req_type
    *req_ptr = (hdrdiag_hdr_mcs_flow_req_type *) req_pkt;
    /* copy the request pkt pointer */

  hdrdiag_hdr_mcs_flow_req_type *rsp_ptr = NULL;
    /* return response pointer    */

  const unsigned int rsp_len = sizeof( hdrdiag_hdr_mcs_flow_req_type );
    /* return response length   */

  hdrlog_hdr_mcs_flow_action_enum_type action;
    /* local copy of action to take */

  uint32 flow_id;
    /* Flow id in interest */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get action */
  action = (hdrlog_hdr_mcs_flow_action_enum_type) req_ptr->action;

  /* Get flow_id */
  flow_id = req_ptr->flow_id;
  
  /* Check request packet length */
  if (pkt_len != sizeof(hdrdiag_hdr_mcs_flow_req_type))
  {
    if ((pkt_len + 4) == sizeof(hdrdiag_hdr_mcs_flow_req_type))
    {
      flow_id = 0;
    }
    else
    {
      action = HDRLOG_MCS_FLOW_ACTION_INVALID;
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Invalid num of args in MCS cmd");
    }
  }

  switch (action)
  {

    case HDRLOG_MCS_ENABLE_TEST_FLOW:

      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                   "HDR MCS command: enable test flow %d",
                   flow_id);

      /* Request test mode app to enable flow */
      hdrbctest_enable(flow_id);

      break;

    case HDRLOG_MCS_DISABLE_TEST_FLOW:

      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                     "HDR MCS command: disable test flow %d",
                     flow_id); 

      /* Request test mode app to disable flow */
      hdrbctest_disable(flow_id);
      
      break;

    case HDRLOG_MCS_FLOW_ACTION_INVALID:
    default:
      /* Do nothing */
      break;

  }        

  /* Create a response */
  rsp_ptr = (hdrdiag_hdr_mcs_flow_req_type *) 
            diagpkt_subsys_alloc( DIAG_SUBSYS_HDR,
                                  HDR_MCS_FLOW_COMMAND,
                                  rsp_len );


  if ( rsp_ptr )
  {
    /* return the request in the rsp */
    rsp_ptr->action     = req_ptr->action;
    rsp_ptr->flow_id     = req_ptr->flow_id;
  }

  return rsp_ptr;

} /* hdrlog_diag_cmd_hdr_mcs_flow() */


/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_HDR_MCS

DESCRIPTION
  Diagnostic command - MCS Debug related
    - MCS Set ECB Drop Mask
    - MCS Disable/Enable Soft Combining .

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

LOCAL PACKED void * hdrlog_diag_cmd_hdr_mcs_debug
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_hdr_mcs_debug_req_type
    *req_ptr = (hdrdiag_hdr_mcs_debug_req_type *) req_pkt;
    /* copy the request pkt pointer */

  hdrdiag_hdr_mcs_debug_req_type *rsp_ptr = NULL;
    /* return response pointer    */

  const unsigned int rsp_len = sizeof( hdrdiag_hdr_mcs_debug_req_type );
    /* return response length   */

  hdrlog_hdr_mcs_action_enum_type action;
    /* local copy of action to take */

  uint32 payload;
    /* Additional data */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get action */
  action = (hdrlog_hdr_mcs_action_enum_type) req_ptr->action;

  /* Get payload */
  payload = req_ptr->payload;
  
  /* Check request packet length */
  if (pkt_len != sizeof(hdrdiag_hdr_mcs_debug_req_type))
  {
    if ((pkt_len + 4) == sizeof(hdrdiag_hdr_mcs_debug_req_type))
    {
      payload = 0;
    }
    else
    {
      action = HDRLOG_MCS_INVALID;
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Invalid num of args in MCS cmd");
    }
  }

  switch (action)
  {

    case HDRLOG_MCS_SET_ECB_DROP_MASK:
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                   "HDR MCS command: set ecb drop mask %x",
                   payload);              

      /* Payload contains a 32 bit mask for which packets to drop in ECB 
         for testing */
      hdrbclog_set_ecb_drop_mask( payload ); 

      break;

    case HDRLOG_MCS_DISABLE_SOFT_COMBINING:
      HDR_MSG_PROT(MSG_LEGACY_HIGH,
                   "HDR MCS command: disable soft combining");

      /* Disable soft combining */
      hdrbclog_disable_soft_combining(); 

      break;

    case HDRLOG_MCS_ENABLE_SOFT_COMBINING:
      HDR_MSG_PROT(MSG_LEGACY_HIGH,
                   "HDR MCS command: enable soft combining");

      /* Re-enable soft combining */
      hdrbclog_enable_soft_combining(); 

      break;

    case HDRLOG_MCS_INVALID:
    default:
      /* Do nothing */
      break;

  }        

  /* Create a response */
  rsp_ptr = (hdrdiag_hdr_mcs_debug_req_type *) 
            diagpkt_subsys_alloc( DIAG_SUBSYS_HDR,
                                  HDR_MCS_DEBUG_COMMAND,
                                  rsp_len );


  if ( rsp_ptr )
  {
    /* return the request in the rsp */
    rsp_ptr->action     = req_ptr->action;
    rsp_ptr->payload    = req_ptr->payload;
  }

  return rsp_ptr;

} /* hdrlog_diag_cmd_hdr_mcs_debug() */

/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_HDR_MCS_CLR_STATS

DESCRIPTION
  Diagnostic command - MCS Clear Statistics
  This function resets
    1. 1x EV-DO Multicast Packet Rate Metrics
    2. 1x EV-DO Physical Channel Metrics
    3. 1x EV-DO Logical Channel Metrics. 
  by calling appropriate routines defined in hdrrx.c, hdrbcmac.c and hdrbclog.c
  respectively based on the action specified by 'stats_to_reset' mask.

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

LOCAL PACKED void * hdrlog_diag_cmd_hdr_mcs_clr_stats
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_hdr_mcs_clr_stats_req_type
    *req_ptr = (hdrdiag_hdr_mcs_clr_stats_req_type *) req_pkt;
    /* copy the request pkt pointer */

  hdrdiag_hdr_mcs_clr_stats_req_type *rsp_ptr = NULL;
    /* return response pointer    */

  const unsigned int rsp_len = sizeof( hdrdiag_hdr_mcs_clr_stats_req_type );
    /* return response length   */

  uint8 stats_to_reset = 0;
    /* mask that specifies the set of MCS logging statistics to reset*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  stats_to_reset =  req_ptr->stats_to_reset;
  
  /* QXDM ignores an input parameter of zero. The following 'if' is 
   * a work around for this anamalous behavior of QXDM. Though zero 
   * is not a valid input for this command, ignoring the input leads 
   * to undefined behavior. Hence, checking request packet length 
   * and printing an error message if none or zero was provided as 
   * argument.*/
  if ( pkt_len != sizeof(hdrdiag_hdr_mcs_clr_stats_req_type) )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "HDR MCS COMMAND: Invalid num of args in MCS cmd");
  }
  /* A valid stats_to_reset item should have at least one of the three 
   * LSBs set.  If not, an error message is printed */
  else if ( (stats_to_reset & 0x07) == 0 )
  {
    HDR_MSG_PROT_1 (  MSG_LEGACY_ERROR,
                      "HDR MCS COMMAND: No metrics indicated for reset (%d)",
                      stats_to_reset );
  }
  else
  {
    /* Reset Multicast Packet Rate metrics if bit 0 of 
     * stats_to_reset is set */
    if ( stats_to_reset & HDRLOG_MCS_CLR_PKT_RATE_METRICS_MASK )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "HDR MCS command: Reset multicast packet rate metrics ");

      hdrrx_clear_bcc_stats();
    }
    
    /* Reset Physical Channel Metrics if bit 1 of stats_to_reset is set */
    if ( stats_to_reset & HDRLOG_MCS_CLR_PHY_CHAN_METRICS_MASK )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "HDR MCS command: Reset physical channel metrics ");

      hdrbcmac_clear_phy_chan_metrics();
    }
    
    /* Reset Logical Channel Metrics if bit 2 of stats_to_reset is set */
    if ( stats_to_reset & HDRLOG_MCS_CLR_LOGI_CHAN_METRICS_MASK )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "HDR MCS command: Reset logical channel metrics ");

      hdrbclog_clear_mcs_chan_metrics();
    }
  }

  /* Create a response */
  rsp_ptr = (hdrdiag_hdr_mcs_clr_stats_req_type *) 
            diagpkt_subsys_alloc( DIAG_SUBSYS_HDR,
                                  HDR_MCS_CLR_STATS_COMMAND,
                                  rsp_len );

  if ( rsp_ptr )
  {
    /* return the request in the rsp */
    rsp_ptr->stats_to_reset     = req_ptr->stats_to_reset;
  }

  return rsp_ptr;

} /* hdrlog_diag_cmd_hdr_mcs_clr_stats */

#endif /* FEATURE_HDR_BCMCS */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_DTX_MODE

DESCRIPTION
  Diagnostic command - DTXMode

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

PACKED void * hdrlog_diag_cmd_dtx_mode
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_dtx_mode_req_type
    *req_ptr = (hdrdiag_dtx_mode_req_type *) req_pkt;
    /* copy the request pkt pointer */

  hdrdiag_dtx_mode_req_type *rsp_ptr = NULL;
    /* return response pointer    */

  const unsigned int rsp_len = sizeof( hdrdiag_dtx_mode_req_type );
    /* return response length   */

  hdrlog_dtx_mode_action_enum_type action;
    /* Action to take */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get action */
  action = (hdrlog_dtx_mode_action_enum_type) req_ptr->action;
  
  /* Check request packet length */
  if (pkt_len != sizeof(hdrdiag_dtx_mode_req_type))
  {
    if ( ( action == HDRLOG_RESET_DTX_MODE ) && 
         ( ( pkt_len + sizeof(hdrlog_dtx_mode_enum_type) ) == 
           sizeof(hdrdiag_dtx_mode_req_type) ) )
    {
      /* Reset action does not need the "mode" argument */
    }
    else
    {
      /* Invalid number of arguments */
      action = HDRLOG_DTX_MODE_ACTION_INVALID;
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Invalid num of args in DTXMode cmd");
    }
  }

  switch (action)
  {
#ifdef FEATURE_HDR_REVB_DTX_MODE
    case HDRLOG_RESET_DTX_MODE:
    case HDRLOG_OVERRIDE_DTX_MODE:
      
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "HDR DTXMode command:  Action: %d DTXMode %d", 
                      action, req_ptr->mode); 

      hdrphy_set_diag_dtx_mode(action, req_ptr->mode);
      break;

    case HDRLOG_DTX_MODE_ACTION_INVALID:

#endif /* FEATURE_HDR_REVB_DTX_MODE */

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                     "Phy diag command not supported: %d", action);
      break;
  }        

  /* Create a response */
  rsp_ptr = (hdrdiag_dtx_mode_req_type *) 
            diagpkt_subsys_alloc( DIAG_SUBSYS_HDR,
                                  HDR_DTX_MODE,
                                  rsp_len );

  if ( rsp_ptr )
  {
    /* return the request in the rsp */
    rsp_ptr->action = req_ptr->action;
    rsp_ptr->mode = req_ptr->mode;
  }

  return rsp_ptr;

} /* hdrlog_diag_cmd_dtx_mode() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_DIAG_CMD_DIV_PREF

DESCRIPTION
  Diagnostic command - Set Diversity Preference

DEPENDENCIES
  None

PARAMETERS
  req_pkt - pointer to a valid request packet
  pkt_len - length of the request packet

RETURN VALUE
  Pointer to the allocated response packet

SIDE EFFECTS
  None

=========================================================================*/

PACKED void * hdrlog_diag_cmd_div_pref
(
  PACKED void   *req_pkt,
  uint16        pkt_len
)
{
  hdrdiag_hdr_div_pref_ctrl_type
    *req_ptr = (hdrdiag_hdr_div_pref_ctrl_type *) req_pkt;
    /* copy the request pkt pointer */

  hdrdiag_hdr_div_pref_ctrl_type *rsp_ptr = NULL;
    /* return response pointer    */

  const unsigned int rsp_len = sizeof( hdrdiag_hdr_div_pref_ctrl_type );
    /* return response length   */

  hdrlog_hdr_div_pref_ctrl_action_enum_type action;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  action = (hdrlog_hdr_div_pref_ctrl_action_enum_type)req_ptr->action;
  
  /* QXDM ignores an input parameter of zero. The following 'if' is 
   * a work around for this anamalous behavior of QXDM. Though zero 
   * is not a valid input for this command, ignoring the input leads 
   * to undefined behavior. Hence, checking request packet length 
   * and printing an error message if none or zero was provided as 
   * argument.*/
  if ( pkt_len != sizeof(hdrdiag_hdr_div_pref_ctrl_type) )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "HDR DIV PREF COMMAND: Invalid num of args in cmd");
  }

  switch (action)
  {

    case HDRLOG_DIV_PREF_ON:
      HDR_MSG_PROT(MSG_LEGACY_HIGH,
                     "HDR DIV PREF command: turn on diversity preference");              

      hdrlmac_runtime_diversity_pref_ctrl( ( hdrlmac_rx_div_enum_type ) HDRLOG_DIV_PREF_ON); 

      break;

    case HDRLOG_DIV_PREF_OFF:
      HDR_MSG_PROT(MSG_LEGACY_HIGH,
                     "HDR DIV PREF command: turn off diversity preference");             

      hdrlmac_runtime_diversity_pref_ctrl( ( hdrlmac_rx_div_enum_type ) HDRLOG_DIV_PREF_OFF);

      break;

#ifdef FEATURE_HDR_SECOND_CHAIN_TEST_CALL

    case HDRLOG_SECOND_CHAIN_ONLY:
      HDR_MSG_PROT(MSG_LEGACY_HIGH,
                     "HDR DIV PREF command: second chain test call");             

      hdrlmac_runtime_diversity_pref_ctrl( ( hdrlmac_rx_div_enum_type ) HDRLOG_SECOND_CHAIN_ONLY);

      break;
#endif /* FEATURE_HDR_SECOND_CHAIN_TEST_CALL */

    default:
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
                   "HDR DIV PREF command: unknown action");

  }
 

  /* Create a response */
  rsp_ptr = (hdrdiag_hdr_div_pref_ctrl_type *) 
            diagpkt_subsys_alloc( DIAG_SUBSYS_HDR,
                                  HDR_DIV_PREF,
                                  rsp_len );

  if ( rsp_ptr )
  {
    /* Return the request in the rsp */
    rsp_ptr->action = req_ptr->action;
  }

  return rsp_ptr;

}  /* hdrlog_diag_cmd_div_pref() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_REGISTER_LOG_PKTS

DESCRIPTION
  This function registers various log packets with diag so they could
  be logged on demand.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
LOCAL void hdrlog_register_log_pkts ( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /*
    ** The maximum number of log codes that can be registered is set
    ** to 64 by default (see LOG_ON_DEMAND_SIZE in diaglog.c) 
    */

    /* LOG_HDR_ASP_C */
    (void) log_on_demand_register(LOG_HDR_ASP_C,
                           hdrlog_on_request_dispatch);

    /* LOG_HDR_FINGER_DATA_C */
    (void) log_on_demand_register(LOG_HDR_FINGER_DATA_C,
                           hdrlog_on_request_dispatch);

    /* LOG_HDR_SESSION_ATTEMPT_C */
    (void) log_on_demand_register(LOG_HDR_SESSION_ATTEMPT_C,
                           hdrlog_on_request_dispatch);

    /* LOG_HDR_PILOT_SETS_C */
    (void) log_on_demand_register(LOG_HDR_PILOT_SETS_C,
                           hdrlog_on_request_dispatch);

    /* LOG_HDR_STATE_INFO_C */
    (void) log_on_demand_register(LOG_HDR_STATE_INFO_C,
                           hdrlog_on_request_dispatch);

    /* LOG_HDR_SECTOR_INFO_C */
    (void) log_on_demand_register(LOG_HDR_SECTOR_INFO_C,
                           hdrlog_on_request_dispatch);

    #ifdef FEATURE_SD20_LOGGING
      /* LOG_SD20_LAST_ACTION_C */
      (void) log_on_demand_register(LOG_SD20_LAST_ACTION_C,
                             hdrlog_on_request_dispatch);

      /* LOG_SD20_LAST_ACTION_HYBRID_C */
      (void) log_on_demand_register(LOG_SD20_LAST_ACTION_HYBRID_C,
                             hdrlog_on_request_dispatch);
    #endif /* FEATURE_SD20_LOGGING */

    /* LOG_HDR_ACQ_INFO_C */
    (void) log_on_demand_register(LOG_HDR_ACQ_INFO_C,
                           hdrlog_on_request_dispatch);

    /* LOG_HDR_REACQ_INFO_C */
    (void) log_on_demand_register(LOG_HDR_REACQ_INFO_C,
                           hdrlog_on_request_dispatch);

    /* LOG_HDR_SLEEP_INFO_C */
    (void) log_on_demand_register(LOG_HDR_SLEEP_INFO_C,
                           hdrlog_on_request_dispatch);

    /* LOG_HDR_SUBTYPE_STREAM_CONFIG_C */
    (void) log_on_demand_register(LOG_HDR_SUBTYPE_STREAM_CONFIG_C,
                           hdrlog_on_request_dispatch);

} /* hdrlog_register_log_pkts() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_ON_REQUEST_DISPATCH

DESCRIPTION
  Diagnostic command - Send a log packet upon request.  A log packet can
  only be sent on demand if it has previously been registered with diag.

DEPENDENCIES
  None

PARAMETERS
  log_code - the log_code of the log packet we would like to log

RETURN VALUE
  log_on_demand_status_enum_type - status of the log request.
  We are currently only supporting the following:
    LOG_ON_DEMAND_SENT_S
    LOG_ON_DEMAND_NOT_SUPPORTED_S

SIDE EFFECTS
  None

=========================================================================*/

log_on_demand_status_enum_type hdrlog_on_request_dispatch
(
    log_code_type  log_code
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch ( log_code )
  {
    case  LOG_HDR_STATE_INFO_C :

      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "Forcing a State Info log packet");
      hdrplog_log_state_info();

      break;

    case  LOG_HDR_PILOT_SETS_C :

      HDR_MSG_PROT ( MSG_LEGACY_LOW,
                    "Forcing a Pilot Sets log packet");
      hdrsrchlog_log_pilot_set_record();

      break;

    case  LOG_HDR_SECTOR_INFO_C :

      HDR_MSG_PROT ( MSG_LEGACY_LOW,
                    "Forcing a Sector Info log packet");
       hdrplog_log_dm_sector_info();

      break;

    case  LOG_HDR_SESSION_ATTEMPT_C :

      HDR_MSG_PROT ( MSG_LEGACY_LOW,
                    "Forcing a Session Attempt log packet");
      hdrplog_log_session_attempt();

      break;

    case  LOG_HDR_ASP_C :

      /*-----------------------------------------------------------------------
         We only want to log the ASP packet on demand when HDR is active and
         we are not in sleep state.
         (can't perform a dsp read when the dsp clocks are turned off)
      -----------------------------------------------------------------------*/
      if ( hdralmps_get_state() != HDRALMPS_INACTIVE &&
           hdridles_get_state() != HDRIDLES_SLEEP_STATE )
      {
        HDR_MSG_PROT ( MSG_LEGACY_LOW, "Forcing an ASP log packet");
        hdrsrchlog_log_asp_record();
      }
      else
      {
        return( LOG_ON_DEMAND_FAILED_ATTEMPT_S );
      }

      break;

    case  LOG_HDR_FINGER_DATA_C :

      /*-----------------------------------------------------------------------
         We only want to log the FINGER packet on demand when HDR is active and
         we are not in sleep state.
         (can't perform a dsp read when the dsp clocks are turned off)
      -----------------------------------------------------------------------*/
      if ( hdralmps_get_state() != HDRALMPS_INACTIVE &&
           hdridles_get_state() != HDRIDLES_SLEEP_STATE )
      {
        HDR_MSG_PROT ( MSG_LEGACY_LOW,
                      "Forcing a Finger Data log packet");
        hdrsrchlog_log_fing_record();
      }
      else
      {
        return( LOG_ON_DEMAND_FAILED_ATTEMPT_S );
      }

      break;

    #ifdef FEATURE_SD20_LOGGING
    case LOG_SD20_LAST_ACTION_C :

      HDR_MSG_PROT ( MSG_LEGACY_LOW, "Forcing a SD last action packet");
      sd_ss_log_last_action(SD_SS_MAIN);

      break;

    case LOG_SD20_LAST_ACTION_HYBRID_C:

      HDR_MSG_PROT ( MSG_LEGACY_LOW,
                    "Forcing a SD last action hybrid packet");
      sd_ss_log_last_action(SD_SS_HDR);

      break;
    #endif /* FEATURE_SD20_LOGGING */

    case LOG_HDR_ACQ_INFO_C:

      HDR_MSG_PROT ( MSG_LEGACY_LOW,
                    "Forcing a acq info log packet");

      hdrsrchlog_log_on_demand( HDRSRCHLOG_ACQ_INFO );

      break;

    case LOG_HDR_REACQ_INFO_C:

      HDR_MSG_PROT ( MSG_LEGACY_LOW,
                     "Forcing a reacq info log packet");

      hdrsrchlog_log_on_demand( HDRSRCHLOG_REACQ_INFO );

      break;

    case LOG_HDR_ENH_SLEEP_INFO_C:

      HDR_MSG_PROT ( MSG_LEGACY_LOW,
                     "Forcing a enh sleep info log packet");

      hdrsrchlog_log_on_demand( HDRSRCHLOG_SLEEP_INFO );

      break;

    case LOG_HDR_SUBTYPE_STREAM_CONFIG_C:

      HDR_MSG_PROT ( MSG_LEGACY_HIGH,
                     "Forcing a subtypes and stream config log packet");

      hdrscmdb_log_pers_subtype_stream_config();
      break;


    default:

      /* This case should be caught by diag first, only here for consistency */
      HDR_MSG_PROT_1 (MSG_LEGACY_ERROR,
                      "Log on demand of id %d is not supported" ,log_code);
      return ( LOG_ON_DEMAND_NOT_SUPPORTED_S );
  }

  return ( LOG_ON_DEMAND_SENT_S );

}/* hdrlog_on_request_dispatch() */

/* <EJECT> */

/*=========================================================================

FUNCTION HDRLOG_INIT

DESCRIPTION
  Registers a subsystem command dispatch table with diag.
  Functions registered in the table are called by the diag task when it
  receives a command with code (DIAG_SUBSYS_CMD_F -> DIAG_SUBSYS_HDR).

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
/* Create a dispatch table.
   It will be registered to diag through hdrlog_init() */
static const diagpkt_user_table_entry_type hdrdiag_tbl[] =
{
  {HDR_RST_ACCESS_CNT, HDR_RST_RLP_STATS, hdrlog_diag_cmd_reset},
  {HDR_AT_CONFIG, HDR_AT_CONFIG, hdrlog_diag_cmd_at_config},
  {HDR_STATE, HDR_STATE, hdrlog_diag_cmd_state_info},
  {HDR_DIP_SWITCH, HDR_DIP_SWITCH, hdrlog_diag_cmd_dip_switch},
#ifdef FEATURE_HDR_BCMCS
  {HDR_MCS_FLOW_COMMAND, HDR_MCS_FLOW_COMMAND, hdrlog_diag_cmd_hdr_mcs_flow},
  {HDR_MCS_DEBUG_COMMAND, HDR_MCS_DEBUG_COMMAND, hdrlog_diag_cmd_hdr_mcs_debug},
  {HDR_MCS_CLR_STATS_COMMAND, HDR_MCS_CLR_STATS_COMMAND, hdrlog_diag_cmd_hdr_mcs_clr_stats},
#endif
#ifdef FEATURE_HDR_QOS
  {HDR_RST_CONNECTED_STATE_STATS, HDR_RST_MRLP_STATS, hdrlog_diag_cmd_reset},
#else
  {HDR_RST_CONNECTED_STATE_STATS, HDR_RST_CONNECTED_STATE_STATS, hdrlog_diag_cmd_reset},
#endif /* FEATURE_HDR_QOS */
  {HDR_WRITE_DATABASE_TO_EFS, HDR_WRITE_DATABASE_TO_EFS, hdrlog_process_diag_cmd_in_hdrmc},
  {HDR_DTX_MODE, HDR_DTX_MODE, hdrlog_diag_cmd_dtx_mode},
  {HDR_DIV_PREF, HDR_DIV_PREF, hdrlog_diag_cmd_div_pref},
  {HDR_PROTOCOL_DATA, HDR_PROTOCOL_DATA, hdrlog_diag_cmd_protocol_data}
#ifdef FEATURE_HDR_MODEM_API
  ,{HDR_MODEM_API_DATA_REPORT, HDR_MODEM_API_DATA_REPORT,hdrlog_diag_cmd_modem_api_data_report} 
#endif /* FEATURE_HDR_MODEM_API */
};

LOCAL void hdrlog_register_dispatch_srvs (void)
{

  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_HDR, hdrdiag_tbl);

} /* hdrlog_init() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRLOG_POWERUP_INIT

DESCRIPTION
  This function initializes the diag log on demand.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_powerup_init( void )
{
  /* Initialize hdrlog_dipsw_cb_list */
  memset(&hdrlog_dipsw_cb_list[0], 0, sizeof(hdrlog_dipsw_cb_list));

  /* Registers a subsystem command dispatch table with diag */
  hdrlog_register_dispatch_srvs();

  /* registers with diag the log packets that can be logged on demand */
  hdrlog_register_log_pkts();

  /* Initialize our dip switches to all 'off' */
  dipsw_mask = 0;

  /* Initialize the connected state statistics timer. */
  rex_def_timer_ex( &hdrlog_conn_state_stats_timer, 
                    hdrlog_conn_stat_stats_timer_cb, 
                    HDRLOG_CONN_STAT_STATS_TIMER );

  /* Initialize the FTD stats timer */
  rex_def_timer_ex( &hdrlog_ftd_stats_refresh_timer,
                    hdrlog_timer_cb, 
                    HDRLOG_FTD_APP_REFRESH_TIMER );

} /* hdrlog_powerup_init() */

/* EJECT */
/*=========================================================================

FUNCTION HDRLOG_LOG_FWD_PKT_SNAPSHOT_RECORD

DESCRIPTION
  Logs a TBD snapshot of the forward link packet supplied by the decoder
  at the AT.  As this packet needs to be used for debugging only, the exact
  details of byte fields can be decided on need basis.

DEPENDENCIES
  None

PARAMETERS
  type      - Packet Type (0=Control, 1=Traffic)
  num_bytes - number of bytes in this packet
  data_ptr  - pointer to the data to be logged

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_log_fwd_pkt_snapshot_record
(
        uint8  type,
        uint8  num_bytes,
  const uint8  *data_ptr
)
{
  LOG_HDR_FWD_PKT_SNAPSHOT_C_type   *log_ptr;
    /* log pointer to the Forward Packet Snapshot record type */

  int8  i;
    /* loop variable */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( log_status( LOG_HDR_FWD_PKT_SNAPSHOT_C ) )
  {
    log_ptr = ( LOG_HDR_FWD_PKT_SNAPSHOT_C_type * )
            log_alloc_ex( (log_code_type) LOG_HDR_FWD_PKT_SNAPSHOT_C,
            HDRLOG_VAR_SIZEOF ( LOG_HDR_FWD_PKT_SNAPSHOT_C_type,
                                hdrlog_fwd_pkt_snapshot_type,
                                num_bytes ) );
    if ( log_ptr )
    {
      log_ptr -> pkt_type   = type;
      log_ptr -> pkt_length = num_bytes;

      for ( i = 0; i < num_bytes; i++ )
        /* update the fields in the log record */
      {
        log_ptr -> data[i] = data_ptr[i];
      }

      log_commit( (log_type *) log_ptr);
        /* submit the log to DIAG */
    } /* if log_ptr  */
  } /* if log_status */

} /* hdrlog_log_fwd_pkt_snapshot_record() */

/*=========================================================================

FUNCTION HDRLOG_LOG_REV_PKT_SNAPSHOT_RECORD

DESCRIPTION
  Logs a TBD snapshot of the reverse link packet supplied by the decoder
  at the AT.  As this packet needs to be used for debugging only, the exact
  details of byte fields can be decided on need basis.

DEPENDENCIES
  None

PARAMETERS
  type      - packet type (0=Access, 1=Traffic)
  num_bytes - number of bytes in the packet
  data_ptr  - pointer to the data this is logged

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_log_rev_pkt_snapshot_record
(
        uint8  type,
        uint8  num_bytes,
  const uint8  *data_ptr
)
{
  LOG_HDR_REV_PKT_SNAPSHOT_C_type   *log_ptr;
    /* log pointer to the Forward Packet Snapshot record type */

  int8                i;
    /* loop variable */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( log_status ( LOG_HDR_REV_PKT_SNAPSHOT_C ) )
  {
    log_ptr   = ( LOG_HDR_REV_PKT_SNAPSHOT_C_type * )
              log_alloc_ex( (log_code_type) LOG_HDR_REV_PKT_SNAPSHOT_C,
              HDRLOG_VAR_SIZEOF ( LOG_HDR_REV_PKT_SNAPSHOT_C_type,
                                  hdrlog_rev_pkt_snapshot_type,
                                  num_bytes ) );

    if ( log_ptr )
    {
      log_ptr->pkt_type   = type;
      log_ptr->pkt_length = num_bytes;

      for ( i = 0; i < num_bytes; i++ )
        /* update the fields in the log record */
      {
        log_ptr->data[i]  = data_ptr[i];
      }

      log_commit( (log_type *) log_ptr);
      /* submit the log to DIAG */

    } /* if log_ptr  */
  } /* if log_status */

} /* hdrlog_log_rev_pkt_snapshot_record() */

/*=========================================================================

FUNCTION HDRLOG_LOG_DEBUG_HEX_DUMP_RECORD

DESCRIPTION
  This packet is a generic HDR log packet that used for debugging
  purposes.

DEPENDENCIES
  None

PARAMETERS
  num_bytes - number of bytes in the packet
  data_ptr  - pointer to the data this is logged

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_log_debug_hex_dump_record
(
       uint16  num_bytes,
  const uint8  *data_ptr
)
{
  LOG_HDR_DEBUG_HEX_DUMP_C_type   *log_ptr;
    /* log pointer to the Debug Hex Dump record type */

  uint16               i;
    /* loop variable */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( log_status( LOG_HDR_DEBUG_HEX_DUMP_C ) )
  {

    log_ptr   = ( LOG_HDR_DEBUG_HEX_DUMP_C_type * )
              log_alloc_ex( (log_code_type) LOG_HDR_DEBUG_HEX_DUMP_C,
              HDRLOG_VAR_SIZEOF ( LOG_HDR_DEBUG_HEX_DUMP_C_type,
                                  hdrlog_debug_hex_dump_type,
                                  num_bytes ) );

    if ( log_ptr )
    {
      log_ptr->pkt_length = num_bytes;

      for ( i = 0; i < num_bytes; i++ )
        /* update the fields in the log record */
      {
        log_ptr->data[i]  = data_ptr[i];
      }

      log_commit( (log_type *) log_ptr);
      /* submit the log to DIAG */

    } /* if log_ptr  */
  } /* if log_status */

} /* hdrlog_log_debug_hex_dump_record() */

/*=========================================================================

FUNCTION HDRLOG_START_CONN_STATE_STATS_PERIODIC_TIMER

DESCRIPTION
  This function starts the connected state statistics timer so that we periodically
  log statistics.

DEPENDENCIES
  None

PARAMETERS
  timer_is_on - Whether to turn the timer on or off.

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_start_conn_state_stats_periodic_timer
(
  boolean timer_is_on
)
{
  /* Either clear or set the timer depending on the argument. */
  if ( timer_is_on )
  {
    (void) rex_set_timer( &hdrlog_conn_state_stats_timer, 
                   HDRLOG_CONN_STATE_STATS_TIMER_PERIOD );
  }
  else
  {
    (void) rex_clr_timer( &hdrlog_conn_state_stats_timer );
  }
} /* hdrlog_start_conn_state_stats_periodic_timer() */


#ifdef FEATURE_MDSP_DEBUG_LOGGING 
/*=========================================================================

FUNCTION HDRLOG_MDSP_DEBUG_RECORD

DESCRIPTION
  This function gets a log record from the diag log buffer, fills it with 
  the latest mdsp debug logging information and commits it for delivery. 
  
DEPENDENCIES
  None

PARAMETERS
  read_offset  - Offset from the start of the MDSP Log buffer
  chunk_length - Length of the MDSP chunk/packet in words

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_mdsp_debug_record
( 
  uint16 read_offset, 
    /* Offset from the start of the MDSP Log buffer */ 

  uint16 chunk_length 
    /* Length of the MDSP chunk/packet in words */
)
{
  LOG_HDR_MDSP_DEBUG_BUFFER_C_type   *log_ptr;
    /* Log pointer to the MDSP Debug record type */

  uint16 i; 
    /* Loop counter */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  log_ptr = ( LOG_HDR_MDSP_DEBUG_BUFFER_C_type * )
              log_alloc_ex( (log_code_type) LOG_HDR_MDSP_DEBUG_BUFFER_C,
              HDRLOG_VAR_SIZEOF ( LOG_HDR_MDSP_DEBUG_BUFFER_C_type,
                                  hdrlog_mdsp_debug_buffer_type,
                                  chunk_length ) );

  if ( log_ptr )
  {
    log_ptr->mdsp_data[0] = chunk_length;
      /* Update the packet length */

    for ( i = 1; i < log_ptr->mdsp_data[0]; i++ )
    {
      log_ptr->mdsp_data[i] = HDRMDSP_GET_MDSP_LOG_DATA( read_offset + i );
    }
    
    log_commit( (log_type *) log_ptr);
      /* Submit the log to DIAG */

  }  /* if log_ptr  */
  
} /* hdrlog_mdsp_debug_record() */
#endif /* FEATURE_MDSP_DEBUG_LOGGING */

/*===========================================================================

FUNCTION HDRLOG_PROCESS_CMD

DESCRIPTION
  This function processes commands given to logging module.

DEPENDENCIES
  None

PARAMETERS
  log_cmd_ptr - Pointer to the command for LOG to process  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrlog_process_cmd( hdrlog_cmd_type * log_cmd_ptr )
{

  /*---------------------------------------------------------------------------
  Determine which command has been received.
  ---------------------------------------------------------------------------*/
  switch ( log_cmd_ptr->name )
  {
    case HDR_WRITE_DATABASE_TO_EFS:
      if (hdrscmdb_write_all_config_data() != E_SUCCESS)
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, "Cannot save session info to EFS");
      }
      break;

    default:
      HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Invalid command code (%d)",
                         log_cmd_ptr->name);
      break;
  }

} /* hdrlog_process_cmd() */

/*===========================================================================

FUNCTION HDRLOG_REGISTER_SLOT_LOGGING_CB

DESCRIPTION
  This function is used by other modules to register a call back for mDSP
  slot based logging.

DEPENDENCIES
  None

PARAMETERS
  client_id - Registering client Identifier
  cb - Registered Call back for mDSP slot based logging.
  event - Event registered for. (Rx or RxTx)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_register_slot_logging_cb
(
  hdrlog_mdsp_slot_logging_client_type client_id,
  hdrlog_mdsp_slot_logging_client_cb_type cb
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( client_id >= HDRLOG_MAX_NUM_MDSP_SLOT_LOGGING_CLIENTS )
  {
    ERR_FATAL( "Registration attempt for invalid Slot logging client: %d", 
               client_id, 0, 0 );
  }

  /*-----------------------------------------------------------------
     ---------------------------------
     |         |       SLCB           |
     |          ----------------------
     |CB       |  NULL  |   Non-NULL  |
     ---------------------------------
     |NULL     |   N    |    Y        |
     ---------------------------------
     |Non-NULL |   Y   |     N        |
     ---------------------------------

     In the above block diagram, SLCB refers to registered callbacks
     and CB refers to the callback that is about to be registered (function
     parameter). We want to proceed with the updating the callback reg
     table and associated fields only for the following two conditions.
     1. If we DO NOT have a valid registration already and we are trying
        to register a non-NULL call back, we proceed with registration
        and increment NumRegisteredClients.
     2. If we DO have a valid registration already and we are trying to
        register a NULL callback (de-register), we proceed with 
        de-registration and decrement the NumRegisteredClients
     All other cases, we don't disturb the registration
  -----------------------------------------------------------------*/

  /* If we do not have any callbacks registered for this client already,
     and we are trying to register a non-NULL call back */
  if ( ( hdrlog_mdsp_slcb_table [client_id].cb == NULL ) &&
       ( cb != NULL ) )
  {
    /* Register callback and increment count */
    hdrlog_mdsp_slcb_table[client_id].cb = cb;
    hdrlog_num_registered_clients++;
  }
  /* Else if we are trying to de-register */
  else if ( ( hdrlog_mdsp_slcb_table [client_id].cb != NULL ) &&
            ( cb == NULL ) )
  {
    /* Deregister call back and decrement count */
    hdrlog_mdsp_slcb_table[client_id].cb = NULL;
    hdrlog_num_registered_clients--;
  }

} /* hdrlog_register_slot_logging_cb */

/*===========================================================================

FUNCTION HDRLOG_PROCESS_MDSP_LOG_EVENT

DESCRIPTION
  This function is called to process mDSP log event. It calls the registered
  call backs from the callback table. 

  This function will be called from ISR context.

DEPENDENCIES
  None

PARAMETERS
  sl_params - Pointer to struct containing the slot logging data info.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_process_mdsp_log_event 
(
  hdrlog_mdsp_slot_logging_data_type *sl_params
)
{

  hdrlog_mdsp_slot_logging_client_type client_idx;
    /* Client index used as loop counter */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( client_idx = HDRLOG_FIRST_MDSP_SLOT_LOGGING_CLIENT; 
        client_idx < HDRLOG_MAX_NUM_MDSP_SLOT_LOGGING_CLIENTS;
        ++client_idx )
  {

    if ( hdrlog_mdsp_slcb_table[client_idx].cb != NULL )
    {
      (hdrlog_mdsp_slcb_table[client_idx].cb)( sl_params );
    }
  } /* for each registered client */
  
} /* hdrlog_process_mdsp_log_event */

#if defined T_MDM9X00 || defined FEATURE_HDR_DIME_MODEM
/*===========================================================================

FUNCTION HDRLOG_MDSP_ISR

DESCRIPTION
  This function is the interrupt service routine for the logging interrupt
  given by the mDSP.

DEPENDENCIES
  None

PARAMETERS
  active_buffer - the active logging buffer 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_mdsp_isr( int active_buffer )
#else
/*===========================================================================

FUNCTION HDRLOG_MDSP_ISR

DESCRIPTION
  This function is the interrupt service routine for the logging interrupt
  given by the mDSP.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_mdsp_isr( void )
#endif 
{

  uint8 rx_log_buff_idx = 0;
    /* Rx log buffer index */

  uint16 expected_log_str = 0;
    /* Expected starting HSTR for this log event (computed based on last
       log event HSTR and log buffer size in slots) */

  uint16 rx_log_str = 0;
    /* STR corresponding to first index of the Rx log buffer */

  uint8 tx_log_buff_idx = 0;
    /* Tx Log Buffer index */

  uint8 rx_tx_cbuff_valid = 0;
    /* Bit mask denoting which one of the log buffers corresponding to demod 
       carriers are valid */

  uint8 rx_cbuff_valid = 0;
    /* Bit mask denoting which one of the log buffers corresponding to demod */

  static uint16 last_log_str = 0;
    /* Half-slot time reference of the log buffer when read during 
       last interrupt */

  static boolean last_log_str_valid = FALSE;
    /* Is Last log HSTR valid? */

  hdrlog_mdsp_slot_logging_data_type sl_params;
    /* Slot logging callback parameters */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If mDSP clock is turned on */
  if ( hdrmdsp_check_dsp_clk() )
  {

#if defined T_MDM9X00 || defined FEATURE_HDR_DIME_MODEM
    rx_log_buff_idx = active_buffer;
      /* Get the Rx log buffer index */
#else
   rx_log_buff_idx = HDRLOG_GET_RX_BUF_INDEX();
     /* Get the Rx log buffer index */
#endif

    rx_log_str = HDRLOG_GET_RX_STR( rx_log_buff_idx );
      /* Read Rx log buffer's first index HSTR */

    /*-----------------------------------------------------------------
              CHECK IF WE MISSED ANY LOG EVENT SINCE LAST READ
     -----------------------------------------------------------------*/

    expected_log_str =
      ((last_log_str + HDRLOG_MDSP_LOG_BUFF_SIZE_SLOTS ) & 0xFFFF);
      /* Find expected HSTR */

    /* Check if we missed a logging event */
    if ( ( last_log_str_valid ) && ( expected_log_str != rx_log_str ) )
    {
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR,
                      "Missed Rx logging event at 0x%x Current log STR: 0x%x",
                      expected_log_str,
                      rx_log_str );
    }

    rx_cbuff_valid = HDRLOG_GET_RX_CARRIER_BUFF_VALID( rx_log_buff_idx );
      /* Get the bit mask denoting valid Rx log buffers for each carrier */

    /*-----------------------------------------------------------------
                          PROCESS LOG BUFFERS
     -----------------------------------------------------------------*/

    /* We process this interrupt only if the Tx interrupt was pending as well */
    if ( HDRLOG_CHECK_TX_LOG_INT_PENDING_FLAG() )
    {

      tx_log_buff_idx = HDRLOG_GET_TX_BUF_INDEX();
        /* Get the Tx log buffer index */

      rx_tx_cbuff_valid =
        ( rx_cbuff_valid | HDRLOG_GET_TX_CARRIER_BUFF_VALID(tx_log_buff_idx) );
        /* Log if either one of the buffer says that the carrier is valid */

      /* If we have any carrier valid */
      if ( rx_tx_cbuff_valid )
      {
        sl_params.slot_time = rx_log_str;
        sl_params.rx_log_buff_idx = rx_log_buff_idx;
        sl_params.tx_log_buff_idx = tx_log_buff_idx;
        sl_params.rx_buff_valid_mask = rx_cbuff_valid;
        sl_params.rx_tx_buff_valid_mask = rx_tx_cbuff_valid;

        hdrlog_process_mdsp_log_event ( &sl_params );
      }

      /* Clear Tx log interrupt pending flag.  */
      HDRLOG_CLEAR_TX_LOG_INT_PENDING_FLAG();

    } /* If Tx Interrupt was pending as well */
    else
    {
      /* Software assumes that Tx log buffer would be ready when Rx interupt
         is given by mDSP. If the Tx log interrupt is not pending already, 
         print a warning */
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Tx logging interrupt not ready! Skip logging STR: 0x%x",
                      rx_log_str );
    }

    last_log_str = rx_log_str;
      /* Remember current log HSTR to track for missed events during next 
         logging event */

    last_log_str_valid = TRUE;
      /* Mark the last_log_hstr as valid */

    /*-----------------------------------------------------------------
        CHECK IF MDSP OVERWROTE WHILE ARM WAS READING FROM THE BUFFER
     -----------------------------------------------------------------*/

    rx_log_str = HDRLOG_GET_RX_STR( rx_log_buff_idx );
      /* Read Rx & Tx log HSTRs again */

    /* If these are not same as what we read already during this log 
       interrupt */
    if ( rx_log_str != last_log_str )
    {
      /* MDSP overwrote Rx/Tx log buffers while ARM was still reading from it. 
         Possible data inconsistency. Print an F3 message to notify */
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                      "FW overwrote log buffs! STRs - [B4: 0x%x, Now: 0x%x]", 
                      last_log_str, 
                      rx_log_str );
    }

  } /* hdrmdsp_check_dsp_clk() */

} /* hdrlog_mdsp_isr */

/*===========================================================================

FUNCTION HDRLOG_ENABLE_MDSP_LOG_ISR

DESCRIPTION
  This function enables mDSP logging ISR if necessary. It checks if there are
  any registered call backs. If there is none, the log ISR is not enabled.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_enable_mdsp_log_isr( void )
{
  
#if defined T_MDM9X00 || defined FEATURE_HDR_DIME_MODEM
  static boolean hdrlog_fw_logging_enabled = FALSE; 
    /* Track logging status. Assumes FW will not disable independently. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If we have non-zero clients registered */
  if (hdrlog_num_registered_clients > 0) 
  {
    if (hdrlog_fw_logging_enabled == FALSE)
    {
      /* Enable FW log buffer logging */
      hdrdec_set_fw_logging_cb( &hdrlog_mdsp_isr );
      hdrlog_fw_logging_enabled = TRUE; 
    }
  }
  else
  {
    if (hdrlog_fw_logging_enabled == TRUE)
    {
      /* Disable FW log buffer logging */
      hdrdec_set_fw_logging_cb( NULL );
      hdrlog_fw_logging_enabled = FALSE;
    }
  }
#else
  /* If we have non-zero clients registered */
  if (hdrlog_num_registered_clients > 0)
  {
    /* Enable log ISR */
    hdrmdsp_set_isr ( HDR_MDSP_RX_LOG_ISR, hdrlog_mdsp_isr );
  }
  else
  {
    /* Disable log ISR */
    hdrmdsp_set_isr ( HDR_MDSP_RX_LOG_ISR, NULL );
  }
#endif /* T_MDM9X00 */

} /* hdrlog_enable_mdsp_log_isr */


/*===========================================================================

FUNCTION HDRLOG_PROCESS_COLLECT_FTD_STATS

DESCRIPTION
  This function processes command to start or stop collecting FTD stats.

DEPENDENCIES
  None

PARAMETERS
  bit_mask - bit mask that indicates what data to collect and report.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_process_collect_ftd_stats( uint32 bit_mask )
{

  if (bit_mask == SYS_HDR_INFO_MASK_ALL)
  {
    /* Collect all data */
    hdrlog_get_ftd_stats();

    /* Set sginal to CM to indicate data is ready */
    cm_stats_set_modem_info_signal( SYS_MODEM_MODULE_HDR );

    /* Start 3 seconds timer to refresh data */
    rex_set_timer( &hdrlog_ftd_stats_refresh_timer, 
                   HDRLOG_FTD_STATS_REFRESH_TIMER );
  }
  else if (bit_mask == SYS_HDR_INFO_MASK_NONE)
  {
    /* Clear timer */
    rex_clr_timer( &hdrlog_ftd_stats_refresh_timer );
  }
  else 
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Unknown bit mask (%d)", bit_mask);
  }

}  /* hdrlog_process_collect_ftd_stats() */

/*===========================================================================

FUNCTION HDRLOG_CM_GET_FTD_STATS_REPORT

DESCRIPTION
  This function is used by CM to get all FTD stats.

DEPENDENCIES
  None

PARAMETERS
  ftd_stats_report - Pointer to location where the FTD stats need to be copied.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_cm_get_ftd_stats_report
( 
  sys_hdr_information_s_type *ftd_stats_report 
)
{
  REX_DISABLE_PREMPTION();

#ifdef FEATURE_MEMCPY_REMOVAL
  *ftd_stats_report = ftd_info;
#else
  memcpy ( ftd_stats_report, &ftd_info, sizeof (sys_hdr_information_s_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  REX_ENABLE_PREMPTION();
} /* hdrlog_cm_get_ftd_stats_report() */

/*===========================================================================

FUNCTION HDRLOG_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for LOG.
  
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlog_process_timer
(
  uint32 timer_id
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Determine which LOG timer has expired.
  -------------------------------------------------------------------------*/
  switch ( (hdrlog_timer_name_enum_type) timer_id )
  {
    case HDRLOG_FTD_APP_REFRESH_TIMER:
      /* Get all FTD stats */ 
      hdrlog_get_ftd_stats();

      /* Set sginal to CM to indicate data is ready */
      cm_stats_set_modem_info_signal( SYS_MODEM_MODULE_HDR );

      /* Set the timer to log after the timer expires again. */
      rex_set_timer( &hdrlog_ftd_stats_refresh_timer, 
                     HDRLOG_FTD_STATS_REFRESH_TIMER );
      break;
    
    default:
      /*-----------------------------------------------------------------------
        Unrecognized Timer:
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "LOG: Invalid Timer ID (%d)",timer_id);

      break;
  }

} /* hdrlog_process_timer() */
