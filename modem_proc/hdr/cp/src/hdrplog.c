/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R  P R O T O C O L  L O G    F U N C T I O N S

GENERAL DESCRIPTION

  This module provides logging routines used by the protocols
  in the session layer and the connection layer.

EXTERNALIZED FUNCTIONS (Global)

  hdrplog_log_conn_term_stats
  hdrplog_conn_term_event
  hdrplog_conn_start_event
  hdrplog_inc_conn_term_stats_counter
  hdrplog_reset_conn_term_stats                
  hdrplog_log_conn_state_stats
  hdrplog_inc_conn_state_stats_counter
  hdrplog_reset_conn_state_stats

EXTERNALIZED FUNCTIONS (Regional)


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrplog.c_v   1.14   16 Jan 2003 11:12:16   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrplog.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
03/09/12   cnx     Added two traces to track connection attempt and release.
01/05/12   sju     Added verbose debug f3 support for SAP protocols. 
01/03/11   sju     Added support to enable verbose debug f3 at runtime. 
11/08/11   cnx     Fixed compiler warnings.
08/02/11   cnx     Fixed compiler error.
05/31/11   cnx     Added hdrplog_get_subpkt_protocol_data().
06/25/10   sju     Added support for ConnectionFailureReport based on SAR.
10/27/09   wsh     Fixed compiler warning for ARM compiler
09/09/09   wsh     CMI: to support changes in clk and ts APIs
08/28/09   wsh     CMI: Removing conditional defines from API headers
05/15/09   wsh     Include needed headers to support diag split
02/24/09   wsh     CMI: updated included headers due to header split
12/10/08   smd     Replaced assert.h with amssassert.h
11/20/08   kss     Removed/replaced intlocks.
10/23/08   wsh     Fixed critical/high lint warnings
09/11/07   wsh     Removed lint high warnings
02/12/07   mpa     Fixed RVCT warnings.
10/10/06   hal     Added log packet support for RouteUpdateTriggerCode list
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
11/29/04   grl     Added support for connected state stat logging.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/19/04   mpa     Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
                   Added new session closing state.
05/11/04   mpa     Added hdrplog_srch_debug_display_info().
11/21/03   mpa     Merged in compiler warning fixes 
10/06/03    km     Replaced cm_get_cur_hybr_status() with sd_misc_is_hybr_opr()
01/15/02   mpa     Added hdrsrch.h include
10/28/02   mpa     Added hdrplog_log_rcv_ati_list().
                   Log full uati instead of uati024 throughout.
10/15/02   mpa     Added hdrplog_update_log_state_info().
04/15/02   mpa     Added an include to cm.h to quiet the warning msg.
04/09/02   sh      Added change HDR and exit HDR for redirect and PRL not 
                   preferred abort reason
04/04/02   mpa     Added hybrid_mode to hdrplog_log_state().
12/13/01   mpa     Modified sector info and session attempt logging to 
                   accomodate log on demand (externalized hdrplog_log_session
                   _attempt, added hdrplog_log_dm_sector_info, added hdrplog_
                   powerup_update_session_data for hdramp process init)
10/29/01   mpa     Modified reset functions so a packet will be logged right
                   after the reset occurs. Modified hdrplog_log_page_msg()
                   and added hdrplog_log_dm_page_msg().
10/04/01   vas     Added hdrplog_log_tca_info()
09/10/01   sh      added support for counter reset
08/24/01   sh      Set default rel code to unspecified for conn release log
08/23/01   sh      Added UATI024 for session attempt
07/26/01   sh      Created module

==========================================================================*/

/* <EJECT> */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "customer.h"
#include "log_dmss.h"
#include "log.h"
#include "hdrlogi.h"
#include "hdrtrace.h"
#include "hdrplog.h"
#include "hdrplogsrch.h"
#include "hdralmps.h"
#include "hdrlupc.h"
#include "hdrinits.h"
#include "hdridlec.h"
#include "hdridles.h"
#include "hdrcons.h"
#include "hdrrups.h"
#include "hdrovhd.h"
#include "hdrovhds.h"
#include "hdrsmps.h"
#include "hdrsmp.h"
#include "hdrsmpka.h"
#include "hdrscp.h"
#include "hdrscps.h"
#include "hdramps.h"
#include "hdrts.h"
#include "hdrdebug.h"
#include "amssassert.h"
#include "hdrdsm.h"
#include "hdraddr.h"
#include "hdraddr_v.h"

#include "hdrlmac.h"
#include "hdrhai.h"
#include "hdramac.h"
#include "hdrsrch.h"
#include "hdrstream.h"
#include "time_svc.h"
#include "cm.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "time.h"
#include "hdrlog.h"
#include "hdrrmaclog.h"
#include "hdrslpi.h"
#include "hdrsrchlog.h"
#include "hdrutil.h"
#include "sd_v.h"
#include "hdrscmdb.h"
#include "hdrscmamac.h"
#include "hdrscmrmac3.h"
#include "hdrscmfmac.h"
#include "hdrpcp.h"
#include "hdrslp.h"
#include "fs_lib.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#include "hdrsaptx.h"
#include "hdrsaprx.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global defidleions for constants, macros, types,
variables

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local defidleions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define HDRPLOG_CONN_STATE_STATS_VER 0
  /* Version number of the connected state stats packet. */

#define HDRPLOG_CONN_TERM_STATS_VER 0
  /* Version number of the connected term stats packet. */


#define HDRPLOG_NUM_OF_SLOT_PER_80_MS 48  /* one slot = 1.67 ms */
#define HDRPLOG_80_MS 80

/* connection attempt related define for TCA and RTCACK status */
#define HDRPLOG_CA_RESULT_RCV_TCA                0x10
#define HDRPLOG_CA_RESULT_RCV_TCA_N_RTCACK       0x20
#define HDRPLOG_SA_RESULT_RCV_UATI               0x00
#define HDRPLOG_SA_RESULT_NOT_RCV_UATI           0x01
#define HDRPLOG_NO_PN_FOUND                      0xFFFF
#define HDRPLOG_NO_SECTOR_ID_FOUND               0xFFFFFFFFUL
#define HDRPLOG_NO_COLOR_CODE_FOUND              0xFF
#define HDRPLOG_INVALID_STREAM                   0x0BAD

#define HDRPLOG_RUP_TC_LIST_MAX_ENTRIES          6

typedef enum
{
  HDRPLOG_SESSION_STATE_CLOSE     = 0, /* Inactive State */
  HDRPLOG_SESSION_STATE_AMP_SETUP = 1, /* AMP Setup State */
  HDRPLOG_SESSION_STATE_AT_INIT   = 2, /* AT initiated State */
  HDRPLOG_SESSION_STATE_AN_INIT   = 3, /* AN initiated State */
  HDRPLOG_SESSION_STATE_OPEN      = 4, /* Open state */
  HDRPLOG_SESSION_STATE_CLOSING   = 5  /* Session Closing */
} hdrplog_session_state_enum_type;

typedef enum
{
  HDRPLOG_AT_STATE_INACTIVE  = 0, /* Inactive State */
  HDRPLOG_AT_STATE_ACQ       = 1, /* Acquistion */
  HDRPLOG_AT_STATE_SYNC      = 2, /* Sync */
  HDRPLOG_AT_STATE_IDLE      = 3, /* Idle */
  HDRPLOG_AT_STATE_ACCESS    = 4, /* Access */
  HDRPLOG_AT_STATE_CONNECTED = 5  /* Connected */

} hdrplog_at_state_enum_type;

typedef struct
{
  uint8     trans_id;
  uint8     msg_seq;
  uint8     result;
  uint16    duration;
  uint32    success_count;
  uint32    failure_count;
  uint32    attempts_count;
  uint16    pn;
  uint32    sector_id;  
  uint8     color_code;
  uint8     num_ho;
  qword     start_time;
  qword     end_time;
  boolean   tca_rcvd;
  boolean   conn_deny_rcvd;
  boolean   in_progress;
}hdrplog_conn_attempt_type;

typedef struct
{
  uint8     trans_id;
  uint8     result;
  uint32    rati;
  uint16    duration;
  uint32    success_count;
  uint32    failure_count;
  uint32    attempts_count;
  uint16    pn;
  hdraddr_type full_uati;  
  uint8     color_code;
  qword     start_time;
  qword     end_time;
  boolean   uati_rcvd;
  boolean   in_progress;
}hdrplog_session_attempt_type;

typedef struct
{
  boolean       sector_id_initialized;
  hdraddr_type  sector_id;
  uint8         subnet_mask;    
  uint8         color_code;
  uint16        pn_offset;
} hdrplog_sector_info_type;

typedef struct
{
  hdrlog_reva_rup_tc_list_entry_type entries[HDRPLOG_RUP_TC_LIST_MAX_ENTRIES];

  uint8 list_size;
  uint8 num_entries;
} hdrplog_rutc_list_type;

/* Control block for the Protocol Logging*/

LOCAL struct
{
  hdrplog_conn_attempt_type       conn_attempt;
  
  LOG_HDR_PAGE_MSG_C_type         page_msg;  
  
  hdrplog_at_state_enum_type      at_state;
  
  hdrplog_session_state_enum_type session_state;
  
  hdrplog_sector_info_type        sector_info;

  hdrplog_session_attempt_type    session_attempt;
  
  boolean                         access_mode_status;  
    /* AT is in access mode or not */
  
  boolean                         sync_state_status;

  hdrplog_conn_rel_enum_type      normal_rel; 
    /* release reason for the normal release */  

  hdrplog_conn_term_stats_struct_subtype  conn_term_stats;
    /* Logging statistics maintained related to connection terminations. */

  hdrplog_conn_state_stats_struct_subtype conn_state_stats;
    /* Protocol stack connected state stats maintained for logging. */

  hdrplog_rutc_list_type rutc_log;
    /* Log packet for RouteUpdateTriggerCode list */

  /* A 8 bits bitmask. values for HDRPLOG_HDRCP_VERBOSE_DEBUG NV
  * bit7: Reserved
  * bit6: Reserved
  * bit5: Reserved
  * bit4: Reserved
  * bit3: Reserved
  * bit2: SAP        (1: enable verbose debug f3 for SAP, 0: otherwise )
  * bit1: PCP        (1: enable verbose debug f3 for PCP, 0: otherwise )
  * bit0: SLP        (1: enable verbose debug f3 for SLP, 0: otherwise )
  */
  uint8 hdrcp_verbose_debug;
}hdrplog;
 
/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_CONN_ATTEMPT_POWERUP_INIT

DESCRIPTION
  This function initialize the couters/varibales for the Connection Attempt 
  at the start of a session
  
DEPENDENCIES
  Couters are reset when power up
  
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrplog_conn_attempt_powerup_init ( void )
{
  hdrplog.conn_attempt.attempts_count   = 
  hdrplog.conn_attempt.success_count    = 
  hdrplog.conn_attempt.failure_count    =  
  hdrplog.conn_attempt.num_ho           = 0;
  hdrplog.conn_attempt.in_progress      =  
  hdrplog.conn_attempt.tca_rcvd         = 
  hdrplog.conn_attempt.conn_deny_rcvd   = FALSE;
} /* hdrplog_conn_attempt_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_START_CONN_ATTEMPT

DESCRIPTION
  This function report the start of a connection attempt
    
DEPENDENCIES
  None
    
PARAMETERS
  trans_id - transaction id of the connection request message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_start_conn_attempt 
( 
  uint8 trans_id 
)
{

  hdrplog.conn_attempt.in_progress      = TRUE;
  hdrplog.conn_attempt.trans_id         = trans_id;
  hdrplog.conn_attempt.result           =
  hdrplog.conn_attempt.num_ho           = 0;
  hdrplog.conn_attempt.tca_rcvd         = FALSE;
  hdrplog.conn_attempt.conn_deny_rcvd   = FALSE;
  time_get_uptime_ms(hdrplog.conn_attempt.start_time);

} /* hdrplog_start_conn_attempt() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_TCA_INFO

DESCRIPTION
  This function logs information when a Traffic Channel Assignment message
  is received.
    
DEPENDENCIES
  None
    
PARAMETERS
  sho_cell_cnt - Number of pilots received in TCA message
  drc_len - DRC Length
  drc_gain - DRC Channel Gain
  ack_gain - ACK Channel Gain
  frame_offset - Frame Offset

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_tca_info 
( 
  uint8   sho_cell_cnt,
  uint8   drc_len,
  int8    drc_gain,
  int8    ack_gain,
  uint8   frame_offset
)
{
  LOG_HDR_TCA_INFO_C_type *log_ptr = NULL;
    /* log pointer to the log record type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( log_status( LOG_HDR_TCA_INFO_C ) )
  {
    log_ptr = (LOG_HDR_TCA_INFO_C_type *)
                  log_alloc_ex( (log_code_type) LOG_HDR_TCA_INFO_C,
                           (short) sizeof( LOG_HDR_TCA_INFO_C_type ) );
    if ( log_ptr != NULL )
    {
      log_ptr->sho_cell_cnt = sho_cell_cnt;
      log_ptr->drc_len      = drc_len;
      log_ptr->drc_gain     = drc_gain;
      log_ptr->ack_gain     = ack_gain;
      log_ptr->frame_offset = frame_offset;
      log_commit( (log_type *) log_ptr );
      /* submit the log to DIAG */
    }/* if log_ptr */
  }

  hdrplog_inc_conn_state_stats_counter( HDRPLOG_TCA_MESSAGE_COUNT, 1 );
    /* Keep track of the number of TCA messages received for conn state stat
       logging. */

  hdrplog_inc_conn_state_stats_counter( HDRPLOG_TCAM_PILOT_COUNT, sho_cell_cnt );
    /* Keep track of the number of pilots received in TCA messages received
       for conn state stat logging purposes. */

} /* hdrplog_log_tca_info() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_TCA_RCVD

DESCRIPTION
  This function report the reception of a traffic channel assignment
  message
    
DEPENDENCIES
  None
    
PARAMETERS
  msg_seq - message sequence number of the TCA message
  item_ptr - dsm item pointer of the TCA message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_tca_rcvd 
( 
  uint8         msg_seq, 
  dsm_item_type *item_ptr
)
{
  if(hdrplog.conn_attempt.in_progress)
  {
    hdrplog.conn_attempt.tca_rcvd  = TRUE;
    hdrplog.conn_attempt.msg_seq   = msg_seq;
    hdrplog.conn_attempt.result    = HDRPLOG_CA_RESULT_RCV_TCA;
    hdrplog.conn_attempt.pn        = hdrplog_get_pn(item_ptr);
    hdrplog_get_sector_id_n_color_code(
        HDRHAI_ROUTE_UPDATE_PROTOCOL,
        &hdrplog.conn_attempt.sector_id,
        &hdrplog.conn_attempt.color_code);
  }
} /* hdrplog_tca_rcvd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_CONN_DENY_RCVD

DESCRIPTION
  This function report the reception of a connection deny message  
  
    
DEPENDENCIES
  None
    
PARAMETERS
  result   - connection deny reason
  item_ptr - dsm item pointer of the Conenction Deny message


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_conn_deny_rcvd 
( 
  hdrplog_conn_attempt_result_enum_type  reason,
  dsm_item_type *item_ptr
)
{
  if(hdrplog.conn_attempt.in_progress)
  {
    hdrplog.conn_attempt.conn_deny_rcvd  = TRUE;
    hdrplog.conn_attempt.pn              = hdrplog_get_pn(item_ptr);
    hdrplog_get_sector_id_n_color_code(
        HDRHAI_IDLE_STATE_PROTOCOL,
        &hdrplog.conn_attempt.sector_id,
        &hdrplog.conn_attempt.color_code);
    
   hdrplog_end_conn_attempt(reason);

  }

} /* hdrplog_conn_deny_rcvd() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_HO
DESCRIPTION
  This function report the handoff during the connection attempt 
  
    
DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_ho ( void )
{
  if(hdrplog.conn_attempt.in_progress)
  {
    hdrplog.conn_attempt.num_ho++;
  }
} /* hdrplog_log_ho() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_CONN_ATTEMPT

DESCRIPTION
  This function logs the connect attempt record  
    
DEPENDENCIES
  None
    
PARAMETERS
 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrplog_log_conn_attempt( void )
{

  LOG_HDR_CONN_ATTEMPT_C_type *log_ptr;
    /* log pointer to the log record type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  log_ptr = (LOG_HDR_CONN_ATTEMPT_C_type *)
                log_alloc_ex( (log_code_type) LOG_HDR_CONN_ATTEMPT_C,
                           (short) sizeof( LOG_HDR_CONN_ATTEMPT_C_type ) );
  if ( log_ptr )
  {
    log_ptr->trans_id       = hdrplog.conn_attempt.trans_id;
    log_ptr->msg_seq        = hdrplog.conn_attempt.msg_seq;
    log_ptr->result         = hdrplog.conn_attempt.result;
    log_ptr->duration       = hdrplog.conn_attempt.duration;
    log_ptr->success_count  = hdrplog.conn_attempt.success_count;
    log_ptr->failure_count  = hdrplog.conn_attempt.failure_count;
    log_ptr->attempts_count = hdrplog.conn_attempt.attempts_count;
    log_ptr->pn             = hdrplog.conn_attempt.pn;
    log_ptr->sector_id_lsw  = (uint16) (hdrplog.conn_attempt.sector_id & 0xffff);
    log_ptr->sector_id_usw  = (uint8)  (hdrplog.conn_attempt.sector_id >> 16);
    log_ptr->color_code     = hdrplog.conn_attempt.color_code;
    log_ptr->num_ho         = hdrplog.conn_attempt.num_ho;
    log_commit( (log_type *) log_ptr );
      /* submit the log to DIAG */
    
  }/* if log_ptr */

}/* hdrplog_log_conn_attempt() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_END_CONN_ATTEMPT

DESCRIPTION
  This function reports the end of a connection attempt
    
                        
DEPENDENCIES
  None
    
PARAMETERS
  result - result of the connection attempt

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_end_conn_attempt 
( 
  hdrplog_conn_attempt_result_enum_type result 
)
{

  qword time_diff;  /* time difference between the end                        
                      and the start of the conn attempt */
  sys_link_type link_info;  /* current link information */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  
  if(hdrplog.conn_attempt.in_progress==FALSE)
  {
    return;
  }
  
  hdrplog.conn_attempt.attempts_count++;

  time_get_uptime_ms(hdrplog.conn_attempt.end_time);

  qw_sub(time_diff, hdrplog.conn_attempt.end_time, 
         hdrplog.conn_attempt.start_time);

  hdrplog.conn_attempt.duration = 
    (uint16) ((qw_lo(time_diff)/HDRPLOG_80_MS) *
               HDRPLOG_NUM_OF_SLOT_PER_80_MS);
  

  if(result == HDRPLOG_CONN_ATT_SUCCESS)
  {
    hdrplog.conn_attempt.result = HDRPLOG_CA_RESULT_RCV_TCA_N_RTCACK    
                                  | HDRPLOG_CONN_ATT_SUCCESS;
    hdrplog.conn_attempt.success_count++;
  }
  else
  {
    hdrplog.conn_attempt.result |= result;
    hdrplog.conn_attempt.failure_count++;

    if((hdrplog.conn_attempt.conn_deny_rcvd == FALSE) &&
       (hdrplog.conn_attempt.tca_rcvd == FALSE) )
    {
       if (hdrsrch_get_link_info( HDRSRCH_REF_LINK_ID, &link_info ) != E_SUCCESS)
       {
         hdrplog.conn_attempt.pn = HDRPLOG_NO_PN_FOUND;
       }

       hdrplog_get_sector_id_n_color_code(
         HDRHAI_IDLE_STATE_PROTOCOL,
         &hdrplog.conn_attempt.sector_id,
         &hdrplog.conn_attempt.color_code);

       hdrplog_inc_conn_term_stats_counter(
         HDRPLOG_TCA_CONNECTION_SETUP_FAILURE_COUNT, 1 );
         /* Keep track of the number of connection setup failures for 
            logging purposes. */

    }
    else
    {
      hdrplog_inc_conn_term_stats_counter(
        HDRPLOG_RTCACK_CONNECTION_SETUP_FAILURE_COUNT, 1 );
        /* If we received a TCA message but the connection
           still failed log an RTCACK failure since we never setup the RTC. */
    }

    hdrplog_conn_term_event( HDRPLOG_CONN_REL_UNSPECIFIED );
    /* The connection wasn't really lost since it was never connected so lets
       just say that the reason is unspecified. */

    hdrplog_log_conn_term_stats();
      /* Actually send out the connection termination stat log packet here
         when we couldn't establish a connection. */
  }

  hdrplog_log_conn_attempt();

  HDRTRACE_PLOG_CONN_ATTEMPT_IS_ENDED( hdrplog.conn_attempt.result );
  hdrplog.conn_attempt.in_progress = FALSE; 

} /* hdrplog_end_conn_attempt() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_CONN_REL

DESCRIPTION
  This function logs the connection release record  
    
DEPENDENCIES
  None
    
PARAMETERS
  rel_reason - connection release reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_conn_rel
(
  hdrplog_conn_rel_enum_type rel_reason  
)
{

  LOG_HDR_CONN_RELEASE_C_type *log_ptr;
    /* log pointer to the log record type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  log_ptr = (LOG_HDR_CONN_RELEASE_C_type *)
                log_alloc_ex( (log_code_type) LOG_HDR_CONN_RELEASE_C,
                           (short) sizeof( LOG_HDR_CONN_RELEASE_C_type ) );
  if ( log_ptr )
  {
    log_ptr->reason      = rel_reason;
    log_commit( (log_type *) log_ptr );
      /* submit the log to DIAG */
  }/* if log_ptr */

  hdrlog_start_conn_state_stats_periodic_timer( FALSE );
    /* Stop the connected state stats timer */

  /* Send one final protocol stack connection state statistics 
     message and then clear out the last call stats. */
  hdrplog_log_conn_state_stats();
  hdrplog_reset_conn_state_stats( HDRPLOG_LAST_CALL_STATS );

  /* Send one final RMAC connection state statistics 
     message and then clear out the last call stats. */
  hdrrmaclog_log_conn_state_stats();
  hdrrmaclog_reset_conn_state_stats( HDRRMACLOG_LAST_CALL_STATS );

  /* Send one final SLP connection state statistics 
     message and then clear out the last call stats. */
  hdrslp_log_conn_state_stats();
  hdrslp_log_reset_conn_state_stats( HDRSLP_LAST_CALL_STATS );

  /* Send one final search connection state statistics 
     message and then clear out the last call stats. */
  hdrsrchlog_log_conn_state_stats();
  hdrsrchlog_reset_conn_state_stats( HDRSRCHLOG_LAST_CALL_STATS );

  hdrrmaclog_reset_tune_away_rri_info( HDRRMACLOG_LAST_CALL_STATS );
    /* Now reset the tune away RRI statistics but don't send the packet
       out.  We only sent it at the end of a tune away. */


  /* Send the connection termination log message after the connection
     termination code is notified of the call termination. */
  hdrplog_conn_term_event( rel_reason );
  hdrplog_log_conn_term_stats();
  HDRTRACE_PLOG_CONN_IS_TERMINATED( rel_reason );
  hdrplog_reset_conn_term_stats( HDRPLOG_LAST_CALL_STATS );

}/* hdrplog_log_conn_rel() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_SET_NORMAL_CONN_REL_REASON

DESCRIPTION
  This function set the normal connection release reason 
  to either AT or AN release 
    
DEPENDENCIES
  None
    
PARAMETERS
  rel_reason - normal connection release reason. this should be
               either AT or AN release

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_set_normal_conn_rel_reason
(
  hdrplog_conn_rel_enum_type rel_reason  
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( ((rel_reason == HDRPLOG_CONN_REL_AN) ||
               (rel_reason == HDRPLOG_CONN_REL_AT)) );

  hdrplog.normal_rel = rel_reason;
}/* hdrplog_set_normal_conn_rel_reason() */



/* <EJECT> */


/*===========================================================================

FUNCTION HDRPLOG_LOG_DM_PAGE_MSG

DESCRIPTION
  This function logs the received page message to the DM.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_dm_page_msg ( void )
{
  LOG_HDR_PAGE_MSG_C_type   *log_ptr;   /* logging buffer pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  log_ptr = (LOG_HDR_PAGE_MSG_C_type *)
                log_alloc_ex( (log_code_type) LOG_HDR_PAGE_MSG_C,
                           (short) sizeof( LOG_HDR_PAGE_MSG_C_type ) );
  if ( log_ptr )
  {
    log_ptr->hstr           = hdrplog.page_msg.hstr;
    log_ptr->attempts_count = hdrplog.page_msg.attempts_count;
    log_ptr->pn             = hdrplog.page_msg.pn;

    log_commit( (log_type *) log_ptr );
      /* submit the log to DIAG */
  }

} /* hdrplog_log_dm_page_msg() */

/*===========================================================================

FUNCTION HDRPLOG_LOG_PAGE_MSG

DESCRIPTION
  This function processes the received page message and logs

DEPENDENCIES
  None

PARAMETERS
  item_ptr - pointer to the dsm item

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_page_msg
(
  dsm_item_type *item_ptr
)
{
  /* get the hstr */
  hdrplog.page_msg.hstr = (uint16) HDRDSM_GET_TIMESTAMP(item_ptr);

  /* get the PN offset */
  hdrplog.page_msg.pn   = hdrplog_get_pn(item_ptr);

  /* increment the page message attempts count */
  ++hdrplog.page_msg.attempts_count;

  /* log the page message to the dm */
  hdrplog_log_dm_page_msg();

} /* hdrplog_log_page_msg */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRPLOG_PAGE_ATTEMPTS_POWERUP_INIT

DESCRIPTION
  This function reset the page attempts counter  

DEPENDENCIES
  Couter is reset when powerup
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrplog_page_attemps_powerup_init ( void )
{
  hdrplog.page_msg.attempts_count = 0;
}  /* hdrplog_page_attemps_powerup_init */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_SECTOR_INFO

DESCRIPTION
  This function updates and logs the sector information
    
DEPENDENCIES
  hdrplog_log_dm_sector_info
    
PARAMETERS
  sector_id    - 128 bit sector id 
  subnet_mask  - sector subnet mask
  color_code   - color code of the sector
  pn_offset    - sector pn offset
    
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_sector_info
(
  hdraddr_type  sector_id,      
  uint8         subnet_mask,    
  uint8         color_code,
  uint16        pn_offset
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( hdrplog.sector_info.sector_id_initialized == TRUE)
  {
    if ( hdraddr_is_equal( hdrplog.sector_info.sector_id, sector_id) &&
         (hdrplog.sector_info.subnet_mask == subnet_mask) &&
         (hdrplog.sector_info.color_code  == color_code) &&
         (hdrplog.sector_info.pn_offset   == pn_offset)    )
    {
      return;
    }
  }
  else
  {
    hdrplog.sector_info.sector_id_initialized = TRUE;
  }

  hdraddr_set(hdrplog.sector_info.sector_id, sector_id);
  hdrplog.sector_info.subnet_mask = subnet_mask;
  hdrplog.sector_info.color_code  = color_code;
  hdrplog.sector_info.pn_offset   = pn_offset;


  hdrplog_log_dm_sector_info();
   /* log the packet */

} /* hdrplog_log_sector_info */

/*===========================================================================

FUNCTION HDRPLOG_LOG_DM_SECTOR_INFO

DESCRIPTION
  This function logs the sector information
    
DEPENDENCIES
  None
    
PARAMETERS
  None
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_dm_sector_info( void )
{
  LOG_HDR_SECTOR_INFO_C_type *log_ptr;
      /* log pointer to the log record type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  log_ptr = (LOG_HDR_SECTOR_INFO_C_type *)
                log_alloc_ex( (log_code_type) LOG_HDR_SECTOR_INFO_C,
                           (short) sizeof( LOG_HDR_SECTOR_INFO_C_type ) );
  if ( log_ptr )
  {
    hdraddr_set ( (unsigned char *) log_ptr->sector_id, hdrplog.sector_info.sector_id );
    log_ptr->subnet_mask    = hdrplog.sector_info.subnet_mask;
    log_ptr->color_code     = hdrplog.sector_info.color_code;
    log_ptr->pn_offset      = hdrplog.sector_info.pn_offset;
    log_commit( (log_type *) log_ptr );
      /* submit the log to DIAG */
  } 

} /* hdrplog_log_dm_sector_info */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_SECTOR_INFO_INIT

DESCRIPTION
  This function un-initialized the sector id 

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrplog_sector_info_init ( void )
{
  hdrplog.sector_info.sector_id_initialized = FALSE ;
}  /* hdrplog_sector_info_init */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_GET_SECTOR_ID_N_COLOR_CODE

DESCRIPTION
  This function get teh sector id and color code
  from the OVHD

DEPENDENCIES
  None
    
PARAMETERS
  protocol name - calling protocol name
  sector_id_ptr - pointer to sector id
  color_code_ptr - pointer to color code 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_get_sector_id_n_color_code 
( 
  hdrhai_protocol_name_enum_type protocol_name,
  uint32 * sector_id_ptr,
  uint8  * color_code_ptr
)
{
  if ( hdrovhd_get_sector_id_24( sector_id_ptr, 
       protocol_name ) != E_SUCCESS )
  {
    *sector_id_ptr= HDRPLOG_NO_SECTOR_ID_FOUND;
  }

  if ( hdrovhd_get_color_code( color_code_ptr, 
       protocol_name ) != E_SUCCESS )
  {
    *color_code_ptr= HDRPLOG_NO_COLOR_CODE_FOUND;   
  }

} /* hdrplog_get_sector_id_n_color_code */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_GET_PN
DESCRIPTION
  This function get the PN offset from the dsm item 
    
DEPENDENCIES
  None
    
PARAMETERS
   item_ptr - pointer to the dsm item

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrplog_get_pn
(
  dsm_item_type *item_ptr 
)
{
  hdrsrch_link_id_type      link_id;    /* link id associated with the msg */
  sys_link_type             link_info;  /* link info associated with the msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Get link_id from dsm item 
  ---------------------------------------------------------------------------*/
  link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );

  /*---------------------------------------------------------------------------
  Call LMAC to get link information (ie channel & pn) on which the msg is rcvd
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_LTE_TO_HDR_OH
  /* Don't get link info in tunnel mode since HDR lower layer is inactive. */
  if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_SIGNALING_LINK_PROTOCOL) )
  {
    memset(&link_info, 0, sizeof(sys_link_type));
  }
  else 
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdrlmac_get_link_info( link_id, &link_info );
  }

  return link_info.pilot_pn;
}


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_SESSION_ATTEMPT_POWERUP_INIT

DESCRIPTION
  This function initialize the couters/varibales for the Session Attempt 
  when powerup
  
DEPENDENCIES
  Couters are reset when powerup
  
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrplog_session_attempt_powerup_init ( void )
{
  hdrplog.session_attempt.attempts_count   = 0;
  hdrplog.session_attempt.success_count    = 0;
  hdrplog.session_attempt.color_code       = 0;
  hdrplog.session_attempt.rati             = 0;
  hdrplog.session_attempt.failure_count    = 0; 
  hdrplog.session_attempt.in_progress      = FALSE;
} /* hdrplog_session_attempt_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_SESSION_ATTEMPT

DESCRIPTION
  This function logs the session attempt record  
    
DEPENDENCIES
  None
    
PARAMETERS
 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_session_attempt( void )
{

  LOG_HDR_SESSION_ATTEMPT_C_type *log_ptr;
    /* log pointer to the log record type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  log_ptr = (LOG_HDR_SESSION_ATTEMPT_C_type *)
                log_alloc_ex( (log_code_type) LOG_HDR_SESSION_ATTEMPT_C,
                           (short) sizeof( LOG_HDR_SESSION_ATTEMPT_C_type ) );
  if ( log_ptr )
  {
    log_ptr->trans_id       = hdrplog.session_attempt.trans_id;
    log_ptr->result         = hdrplog.session_attempt.result;
    log_ptr->rati           = hdrplog.session_attempt.rati;
    log_ptr->duration       = hdrplog.session_attempt.duration;
    log_ptr->success_count  = hdrplog.session_attempt.success_count;
    log_ptr->failure_count  = hdrplog.session_attempt.failure_count;
    log_ptr->attempts_count = hdrplog.session_attempt.attempts_count;
    log_ptr->pn             = hdrplog.session_attempt.pn;
    hdraddr_set((unsigned char *) log_ptr->full_uati, hdrplog.session_attempt.full_uati);
    log_ptr->color_code     = hdrplog.session_attempt.color_code;
    log_commit( (log_type *) log_ptr );
      /* submit the log to DIAG */
  }/* if log_ptr */

}/* hdrplog_log_session_attempt() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_POWERUP_UPDATE_SESSION_DATA

DESCRIPTION
  This function updates and reports the session attempt log data upon 
  power up.  The data is retrieved from NV memory upon start up.  
  This is logged only if a session is already open.
    
DEPENDENCIES
  None
    
PARAMETERS
  rati       - RATI used for opening that session (restored from NV)
  full_uati  - full uati restored from NV
  color_code - color code from the uati (restored from NV)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_powerup_update_session_data
( 
  hdraddr_type full_uati,
  uint32 rati,
  uint8  color_code
)
{
  hdrplog.session_attempt.rati         = rati;
  hdraddr_set(hdrplog.session_attempt.full_uati, full_uati);
  hdrplog.session_attempt.color_code   = color_code;

  hdrplog_log_session_attempt();  
} /* hdrplog_powerup_update_session_data() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_START_SESSION_ATTEMPT

DESCRIPTION
  This function report the start of a session attempt
    
DEPENDENCIES
  None
    
PARAMETERS
  trans_id - transaction id of the UATI request message
  rati     - RATI used for opening a session

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_start_session_attempt 
( 
  uint8 trans_id, 
  uint32 rati 
)
{
  hdrplog.session_attempt.in_progress      = TRUE;
  hdrplog.session_attempt.trans_id         = trans_id;
  hdrplog.session_attempt.uati_rcvd        = FALSE;
  hdrplog.session_attempt.rati             = rati;

  time_get_uptime_ms(hdrplog.session_attempt.start_time);
} /* hdrplog_start_session_attempt() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_UATI_RCVD

DESCRIPTION
  This function report the reception of a UATI assignment
  message
    
DEPENDENCIES
  None
    
PARAMETERS
  item_ptr   - pointer to the dsm item
  full_uati  - Full UATI from the UATI assignment message
  color_code - color code from the UATI assignment message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_uati_rcvd 
( 
  dsm_item_type *item_ptr,
  hdraddr_type  full_uati,
  uint8         color_code
)
{
  hdrplog.session_attempt.uati_rcvd  = TRUE;
  hdraddr_set(hdrplog.session_attempt.full_uati, full_uati);
  hdrplog.session_attempt.color_code = color_code;
  hdrplog.session_attempt.pn         = hdrplog_get_pn(item_ptr);

  if((hdrplog.session_attempt.in_progress==FALSE) &&
     (hdrsmp_session_is_open(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL)==TRUE) )
  {
    /* this is not a session attempt but we still use
       the session attempt log packet to update 
       the full UATI filed
    */

    hdrplog.session_attempt.result = HDRPLOG_SA_RESULT_RCV_UATI;
    hdrplog_log_session_attempt();
  }

} /* hdrplog_tca_rcvd() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_END_SESSION_ATTEMPT

DESCRIPTION
  This function report the end of a session attempt
    
DEPENDENCIES
  None
    
PARAMETERS
  result - TRUE session attempt is successful

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_end_session_attempt  
( 
  boolean result 
)
{

  qword time_diff;  /* time difference between the end                        
                      and the start of the conn attempt */
  sys_link_type link_info;  /* current link information */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(hdrplog.session_attempt.in_progress==FALSE)
  {
    return;
  }

  hdrplog.session_attempt.attempts_count++;

  time_get_uptime_ms(hdrplog.session_attempt.end_time);

  qw_sub(time_diff, hdrplog.session_attempt.end_time, 
         hdrplog.session_attempt.start_time);

  hdrplog.session_attempt.duration = 
    (uint16) ((qw_lo(time_diff)/HDRPLOG_80_MS) * 
              HDRPLOG_NUM_OF_SLOT_PER_80_MS);


  if(result == TRUE)
  {
    hdrplog.session_attempt.result = HDRPLOG_SA_RESULT_RCV_UATI;
    hdrplog.session_attempt.success_count++;
  }
  else
  {
    hdrplog.session_attempt.result = HDRPLOG_SA_RESULT_NOT_RCV_UATI;
    hdrplog.session_attempt.failure_count++;

    if( hdrplog.session_attempt.uati_rcvd == FALSE )
    {
       if (hdrsrch_get_link_info( HDRSRCH_REF_LINK_ID, &link_info ) != E_SUCCESS)
       {
         hdrplog.session_attempt.pn=HDRPLOG_NO_PN_FOUND;
       }

       if(hdrovhd_get_color_code( &hdrplog.session_attempt.color_code, 
            HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) != E_SUCCESS )
       {
         hdrplog.session_attempt.color_code= HDRPLOG_NO_COLOR_CODE_FOUND;   
       }
    }
  }

  hdrplog_log_session_attempt();

  hdrplog.session_attempt.in_progress  = FALSE; 


  

} /* hdrplog_end_session_attempt() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_UPDATE_STATE_INFO

DESCRIPTION
  This function updates the state information for logging
    
DEPENDENCIES
  None
    
PARAMETERS
 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrplog_update_state_info ( void )
{
  /* log pointer to the log record type */
  hdralmps_state_enum_type  almp_state; 
  hdrinits_state_enum_type  init_state; 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* session state is a summary state for the session
     layer
  */
  switch(hdrsmps_get_state())
  {
    case HDRSMPS_AMP_SETUP:
      
      hdrplog.session_state = HDRPLOG_SESSION_STATE_AMP_SETUP;
      break;

    case HDRSMPS_OPEN:

      switch(hdrscps_get_state())
      {
        case  HDRSCPS_AT_INIT:

          hdrplog.session_state = HDRPLOG_SESSION_STATE_AT_INIT;
          break;

        case HDRSCPS_AN_INIT:
          
          hdrplog.session_state = HDRPLOG_SESSION_STATE_AN_INIT;
          break;            
        
        default:

          hdrplog.session_state = HDRPLOG_SESSION_STATE_OPEN;
          break;
      }
      break;

    case HDRSMPS_CLOSING:
      hdrplog.session_state = HDRPLOG_SESSION_STATE_CLOSING;
      break;

    default:
      
      hdrplog.session_state = HDRPLOG_SESSION_STATE_CLOSE;
      break;
  }

  almp_state =  hdralmps_get_state();
  init_state =  hdrinits_get_state(); 
  
  if(almp_state != HDRALMPS_INIT)
  {
      hdrplog.sync_state_status = FALSE;
  }
  
  if (almp_state != HDRALMPS_IDLE) 
  {
    /* AT can not be in the access mode while ALMP is 
       not in the idle state */
    hdrplog.access_mode_status = FALSE; 
  }

  /* at state is a summary state for the connection layer */
  switch(almp_state)
  {
    case HDRALMPS_INIT:
      
      if( init_state == HDRINITS_SYNC_STATE )
      {
        hdrplog.at_state = HDRPLOG_AT_STATE_SYNC;
        hdrplog.sync_state_status = TRUE;
      }
      else
      {
            if ((init_state == HDRINITS_INACTIVE_STATE) &&
                  (hdrplog.sync_state_status == TRUE) )
            {
               /* this a transitional situation where INIT state change
                is ahead of the ALMP state change so let AT remain in the Sync 
                  state and wait for the ALMP state change.
                Note: ALMP will change to either IDLE or INACTIVE state
               */    
               hdrplog.at_state = HDRPLOG_AT_STATE_SYNC;
            }
            else
            {
              hdrplog.at_state = HDRPLOG_AT_STATE_ACQ;
              hdrplog.sync_state_status = FALSE;
            }
      }
      break;

    case HDRALMPS_IDLE:
      
      if(hdrplog.access_mode_status == TRUE)
      {
        hdrplog.at_state = HDRPLOG_AT_STATE_ACCESS;
      }
      else
      {
        hdrplog.at_state = HDRPLOG_AT_STATE_IDLE;
      }
      break;

    case HDRALMPS_CONNECTED:

      hdrplog.at_state = HDRPLOG_AT_STATE_CONNECTED;
      break;
    
    default:
      hdrplog.at_state = HDRPLOG_AT_STATE_INACTIVE;
      break;
  }

} /* hdrplog_update_state_info() */

/*===========================================================================

FUNCTION HDRPLOG_UPDATE_LOG_STATE_INFO

DESCRIPTION
  This function updates the state information for logging
    
DEPENDENCIES
  None
    
PARAMETERS
 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_update_log_state_info
(  
  LOG_HDR_STATE_INFO_C_type *log_ptr
)
{
  hdrplog_update_state_info();

  log_ptr->at_state = hdrplog.at_state;
  log_ptr->session_state = hdrplog.session_state;
  log_ptr->almp_state = hdralmps_get_state(); 
  log_ptr->init_state = hdrinits_get_state();
  log_ptr->idle_state = hdridles_get_state();
  log_ptr->conn_state = hdrcons_get_state();
  log_ptr->rup_state = hdrrups_get_state();
  log_ptr->ovhd_state = hdrovhds_get_state();

  /* get the current hybrid status
       TRUE  = hybrid on,
       FALSE = hybrid off */
  log_ptr->hybrid_mode = sd_misc_is_hybr_opr();

} /* hdrplog_update_log_state_info() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_STATE_INFO

DESCRIPTION
  This function logs the state information  
    
DEPENDENCIES
  None
    
PARAMETERS
 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_state_info( void )
{

  LOG_HDR_STATE_INFO_C_type *log_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  log_ptr = (LOG_HDR_STATE_INFO_C_type *)
                log_alloc_ex( (log_code_type) LOG_HDR_STATE_INFO_C,
                           (short) sizeof( LOG_HDR_STATE_INFO_C_type ) );
  
  if ( log_ptr )
  {
    /* Update the log structure */
    hdrplog_update_log_state_info( log_ptr );

    /* submit the log to DIAG */ 
    log_commit( (log_type *) log_ptr );
  } 

}/* hdrplog_log_state_info() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_TRANS_ABORT_REASON_FOR_REL

DESCRIPTION
   This function convert HAI's abort reason to appropriate connection
   release reason
  

DEPENDENCIES
  None

PARAMETERS
  abort_reason - Conenction abort reason 
  
RETURN VALUE
  reason for connection release

SIDE EFFECTS
  None

===========================================================================*/

hdrplog_conn_rel_enum_type hdrplog_trans_abort_reason_for_rel
( 
  hdrhai_abort_reason_enum_type abort_reason 
)
{

   hdrplog_conn_rel_enum_type reason = HDRPLOG_CONN_REL_UNSPECIFIED;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (abort_reason)
  {
    case HDRHAI_ABORT_NORMAL_CLOSE:
        reason = hdrplog.normal_rel;
        break;
    case HDRHAI_ABORT_SYS_LOST:
    case HDRHAI_ABORT_SAR_LIMIT:
      reason = HDRPLOG_CONN_REL_SYS_LOST;
      break;
    case  HDRHAI_ABORT_REDIRECT_CHG_HDR:
    case  HDRHAI_ABORT_REDIRECT_EXIT_HDR:
      reason = HDRPLOG_CONN_REL_REDIRECT;
      break;
    case HDRHAI_ABORT_NOT_PREF_CHG_HDR:
    case HDRHAI_ABORT_NOT_PREF_EXIT_HDR:
      reason = HDRPLOG_CONN_REL_NOT_PREFERRED;
      break;
    case HDRHAI_ABORT_OFFLINE:
      reason = HDRPLOG_CONN_REL_OFFLINE;
      break;
    case HDRHAI_ABORT_NAM_CHANGE:
      reason = HDRPLOG_CONN_REL_NAM_CHANGE;
      break;
    case HDRHAI_ABORT_POWERDOWN:
      reason = HDRPLOG_CONN_REL_POWERDOWN;
      break;
    case HDRHAI_ABORT_UNSPECIFIED:
    default:
      reason = HDRPLOG_CONN_REL_UNSPECIFIED;
      break;
  }
    
  return reason;
} /* hdralmp_cedp() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_ACCESS_MODE

DESCRIPTION
  This function set the access mode status  

DEPENDENCIES
  None
    
PARAMETERS
  access_mode_status - TRUE in access mode
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_access_mode
( 
  boolean access_mode_status 
)
{
   hdrplog.access_mode_status = access_mode_status;
   hdrplog_log_state_info();
} /* hdrplog_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_POWERUP_INIT

DESCRIPTION
  This function reset all protocol related counters and statistics   

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_powerup_init ( void )
{
  hdrplog_conn_attempt_powerup_init();
  hdrplog_sector_info_init();
  hdrplog_page_attemps_powerup_init();
  hdrplog_session_attempt_powerup_init();
  hdrplog.access_mode_status = FALSE;
  hdrplog.sync_state_status = FALSE;
  hdrplog.hdrcp_verbose_debug = 0; 
    /* Default to 0 means verbose debug is disable by default  */

  hdrplog_reset_conn_term_stats( HDRPLOG_ALL_STATS );
    /* Clear out all of the connection termination logging stats. */

  hdrplog_reset_conn_state_stats( HDRPLOG_ALL_STATS );
    /* Clear out all of the connected state logging stats. */

  /* Read verbose debug setting from EFS NV. */
  if ( (int)efs_get( HDRPLOG_HDRCP_VERBOSE_DEBUG, &hdrplog.hdrcp_verbose_debug,
                     sizeof(uint8) ) == (int)sizeof(uint8) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                  "Read hdrcp verbose debug successfully %d",
                    hdrplog.hdrcp_verbose_debug );
  }
  else 
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Read hdrcp verbose debug failed. verbose debug off" );
  }

  /* Initialize verbose debug setting for differnt protocols */
  hdrpcp_set_verbose_debug(hdrplog.hdrcp_verbose_debug & HDRPLOG_VERBOSE_DEBUG_PCP);
  hdrslp_set_verbose_debug(hdrplog.hdrcp_verbose_debug & HDRPLOG_VERBOSE_DEBUG_SLP);
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsaptx_set_verbose_debug(hdrplog.hdrcp_verbose_debug & HDRPLOG_VERBOSE_DEBUG_SAP);
  hdrsaprx_set_verbose_debug(hdrplog.hdrcp_verbose_debug & HDRPLOG_VERBOSE_DEBUG_SAP);
  hdrsap_set_verbose_debug(hdrplog.hdrcp_verbose_debug & HDRPLOG_VERBOSE_DEBUG_SAP);
#endif /* FEATURE_LTE_TO_HDR_OH */
} /* hdrplog_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_RESET_CONN_ATTEMPT

DESCRIPTION
  This function reset connection attempt statistics   

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_reset_conn_attempt ( void )
{
  hdrplog_conn_attempt_powerup_init();
  hdrplog_log_conn_attempt();
} /* hdrplog_reset_conn_attempt() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_RESET_PAGE_COUNT

DESCRIPTION
  This function reset page count  

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_reset_page_count ( void )
{
  hdrplog_page_attemps_powerup_init();
  hdrplog_log_dm_page_msg();
} /* hdrplog_reset_page_count() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_RESET_SESSION_ATTEMPT

DESCRIPTION
  This function reset session attempt statistics   

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_reset_session_attempt ( void )
{
  hdrplog_session_attempt_powerup_init();
  hdrplog_log_session_attempt();
} /* hdrplog_reset_session_attempt() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_RCV_ATI_LIST

DESCRIPTION
  This function logs the list of currently valid UATIs

DEPENDENCIES
  None
    
PARAMETERS
  rcv_list_ptr - pointer to the list of valid UATIs
  ati_cnt - number of valid UATIs in the list

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_rcv_ati_list
(
  uint32* rcv_list_ptr,
  uint8 ati_cnt
)
{
  uint8 index;
  LOG_HDR_RCV_ATI_LIST_C_type *log_ptr; /* log ptr to the log record type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Allocate Memory for the log buffer */
  log_ptr = (LOG_HDR_RCV_ATI_LIST_C_type *)
            log_alloc_ex( (log_code_type) LOG_HDR_RCV_ATI_LIST_C,
                          sizeof(LOG_HDR_RCV_ATI_LIST_C_type) + (ati_cnt-1) * sizeof(hdrlog_rcv_ati_list_type));
  if (log_ptr)
  {
    /* fill in info for our log record */
    log_ptr->num_records = ati_cnt;
    
    for (index=0; index < ati_cnt; index++)
    {
      log_ptr->uati_value[index] = rcv_list_ptr[index];
    }

    /* commit to diag */
    log_commit(log_ptr);
  }

} /* hdrplog_log_rcv_ati_list() */

/*===========================================================================

FUNCTION HDRPLOG_SRCH_DEBUG_DISPLAY_INFO

DESCRIPTION
  This function fills in protocol related debug information for the 
  LOG_HDR_DEBUG_DISPLAY_C log packet.  This packet is logged by the searcher.
      
DEPENDENCIES
  None
    
PARAMETERS
  log_ptr - pointer to the debug display log record
  
RETURN VALUE
  returns the log packet via the input parameter log_ptr

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_srch_debug_display_info
(
  hdrlog_debug_display_struct_subtype   *log_ptr
)
{
  uint8 index;  
    /* for loop index */
  
  hdrhai_app_subtype_enum_type stream_config[HDRLOG_NUM_STREAMS];
    /* holds a copy of the stream configuration.  Needed because the log_ptr  
       is a pointer to a packed struct but hdrstream_get_stream_config() 
       does not take a packed pointer*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the AT's current state, session and sync info. */
  hdrplog_update_state_info();
  
  log_ptr->prot_state = hdrplog.at_state;
  log_ptr->hdr_session_state = hdrplog.session_state;
    
  /* Update the UATI info */
  log_ptr->uati_info.uati24 =     
    ((hdrplog.session_attempt.full_uati[13] << 16) & 0xff0000) |
    ((hdrplog.session_attempt.full_uati[14] << 8) & 0x00ff00) |
    ( hdrplog.session_attempt.full_uati[15] & 0x0000ff );

  log_ptr->uati_info.color_code = hdrplog.session_attempt.color_code;

  /* Update the Stream Config Info */
   if (hdrstream_get_stream_config(&stream_config[0]))
   {
     for (index=0; index<HDRLOG_NUM_STREAMS; index++)
     {
       log_ptr->stream_config[index] = stream_config[index];
     }
   }
   else
   {
    for (index=0; index<HDRLOG_NUM_STREAMS; index++)
    {
      log_ptr->stream_config[index] = HDRPLOG_INVALID_STREAM;
    }
   }

} /* hdrplog_srch_debug_display_info() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_LOG_CONN_TERM_STATS

DESCRIPTION  
  Logs the connection termination info. It gets a log record
  from the diag log buffer, fills it with the latest 
  connection termination info and commits the log record to
  the diag for delivery.

DEPENDENCIES 
  None

PARAMETERS
  None

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_log_conn_term_stats( void )
{
  LOG_HDR_CONN_TERM_STATS_C_type *log_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  log_ptr = (LOG_HDR_CONN_TERM_STATS_C_type *)
            log_alloc( (log_code_type) LOG_HDR_CONN_TERM_STATS_C,
                       sizeof( LOG_HDR_CONN_TERM_STATS_C_type ) );
    /* Actually log the packet if the log mask is set properly. */

  if ( log_ptr )
  {
    REX_DISABLE_PREMPTION();

#ifdef FEATURE_MEMCPY_REMOVAL
    log_ptr->conn_term_stats = hdrplog.conn_term_stats;
#else
    memcpy( (void *)( &log_ptr->conn_term_stats ),
            (void *)( &hdrplog.conn_term_stats ),
            sizeof( hdrplog_conn_term_stats_struct_subtype ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      /* Copy log info into the log records */

    REX_ENABLE_PREMPTION();

    log_commit( (log_type *) log_ptr );
      /* Now send the packet out the diag port. */
  }
} /* hdrplog_log_conn_term_stats() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_CONN_TERM_EVENT

DESCRIPTION  
  This function records the timestamp of a call termination for call
  statistics tracking purposes.
  
DEPENDENCIES 
  None

PARAMETERS
  rel_reason - The Layer 3 reason for the call termination.

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_conn_term_event
(
  hdrplog_conn_rel_enum_type rel_reason  
)
{
  uint64 time_stamp_delta;
    /* delta between the conn term event and start. */

  time_type time_stamp;
    /* Time stamp of conn term event. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) time_get( time_stamp ); 
    /* Track the time we finished the tune away. */

  REX_DISABLE_PREMPTION();

  hdrplog.conn_term_stats.last_call_release_reason = rel_reason;
    /* What is the reason we are terming the call from L3's perspective. */

  qw_equ( hdrplog.conn_term_stats.end_time_of_last_call, 
          time_stamp );
    /* Record when the call terminated. */

  /* If the call ever actually started then calculate how long the call
     lasted.  If not, then the call attempt failed and there is no
     need to record this information. */
  if ( hdralmps_get_state() == HDRALMPS_CONNECTED )
  {
    time_stamp_delta = 
      hdrutil_calc_duration(
        hdrplog.conn_term_stats.start_time_of_last_call,
        hdrplog.conn_term_stats.end_time_of_last_call ); 
  }
  else
  {
    qw_equ( hdrplog.conn_term_stats.start_time_of_last_call, 
            time_stamp );
      /* The call never actually started so just make the start time
         stamp the same as the end time stamp. */

    time_stamp_delta = 0;
  }

  hdrplog.conn_term_stats.traffic_time_since_call = (uint32) time_stamp_delta;
    /* Keep track of the time since the beginning of the last call. */

  hdrplog.conn_term_stats.traffic_time_since_reset += 
    hdrplog.conn_term_stats.traffic_time_since_call;
    /* Keep track of the time since the AT was reset. */

  hdrplog.conn_term_stats.total_call_terms++;
    /* Record the total number of call terminations. */

  REX_ENABLE_PREMPTION();
} /* hdrplog_conn_term_event() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_CONN_START_EVENT

DESCRIPTION 
  This function notifies the logging module of the start of a new HDR 
  connection for logging statistic tracking purposes. 

DEPENDENCIES 
  None

PARAMETERS
  None

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_conn_start_event( void )
{
  time_type time_stamp;
    /* Time stamp of conn term event. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) time_get( time_stamp ); 
    /* Track the time we finished the tune away. */

  REX_DISABLE_PREMPTION();

  qw_equ( hdrplog.conn_term_stats.start_time_of_last_call, 
          time_stamp );
    /* Record the start time of the call. */

  hdrlog_start_conn_state_stats_periodic_timer( TRUE );
    /* Start the timer so that we log every 6 seconds */

  REX_ENABLE_PREMPTION();
} /* hdrplog_conn_start_event() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_INC_CONN_TERM_STATS_COUNTER

DESCRIPTION 
  This function increments the appropriate connection termination counter
  by the given amount.

DEPENDENCIES 
  None

PARAMETERS
  counter - The counter to increment.
  inc_count - How much to increment the counter by.

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_inc_conn_term_stats_counter
( 
  hdrplog_conn_term_stats_count_type counter,
  uint16                             inc_count
)
{
  REX_DISABLE_PREMPTION();

  hdrplog.conn_term_stats.since_reset_counts[counter] += inc_count;
    /* Increment the specified counter. */

  REX_ENABLE_PREMPTION();
} /* hdrplog_inc_conn_term_stats_counter() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_RESET_CONN_TERM_STATS

DESCRIPTION  
  This function resets the connection termination stats as
  specified by the stats reset parameter.

DEPENDENCIES 
  None

PARAMETERS
  stats_reset - Which statistics we should clear (all or just the last 
                call stats).

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_reset_conn_term_stats
( 
  hdrplog_reset_stats_type stats_reset
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  REX_DISABLE_PREMPTION();

  switch ( (uint8) stats_reset )
  {
    case (uint8) HDRRMACLOG_ALL_STATS:

      memset( (void*)&hdrplog.conn_term_stats, 0, 
              sizeof(hdrplog_conn_term_stats_struct_subtype) );
        /* Reset all of the statistics tracked for this log packet. */

      hdrplog.conn_term_stats.version = HDRPLOG_CONN_TERM_STATS_VER;
        /* Set the version number of the packet for future expandability. */

      break;

    case (uint8) HDRRMACLOG_LAST_CALL_STATS:
      hdrplog.conn_term_stats.traffic_time_since_call = 0;
        /* Reset only the statistics tracked since the start of the
           last call. */

      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Unknown statistics to clear: %d", stats_reset);
      break;
  }

  REX_ENABLE_PREMPTION();
} /* hdrplog_reset_conn_term_stats() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_LOG_CONN_STATE_STATS

DESCRIPTION  
  Logs the connected state stats. It gets a log record
  from the diag log buffer, fills it with the latest 
  connected state info and commits the log record to
  the diag for delivery.

DEPENDENCIES 
  None

PARAMETERS
  None

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_log_conn_state_stats( void )
{
  LOG_HDR_CONN_STATE_PS_STATS_C_type *log_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  log_ptr = (LOG_HDR_CONN_STATE_PS_STATS_C_type *)
            log_alloc( (log_code_type) LOG_HDR_CONN_STATE_PS_STATS_C,
                       sizeof( LOG_HDR_CONN_STATE_PS_STATS_C_type ) );
    /* Actually log the packet if the log mask is set properly. */

  if ( log_ptr )
  {
    REX_DISABLE_PREMPTION();

#ifdef FEATURE_MEMCPY_REMOVAL
    log_ptr->conn_state_ps_stats = hdrplog.conn_state_stats;
#else
    memcpy( (void *)( &log_ptr->conn_state_ps_stats ),
            (void *)( &hdrplog.conn_state_stats ),
            sizeof( hdrplog_conn_state_stats_struct_subtype ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      /* Copy log info into the log record */

    REX_ENABLE_PREMPTION();

    log_commit( (log_type *) log_ptr );
      /* Now send the packet out the diag port. */
  }
} /* hdrplog_log_conn_state_stats() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_INC_CONN_STATE_STATS_COUNTER

DESCRIPTION  
  This function increments the requested connected state 
  counter by the given amount.

DEPENDENCIES 
  None

PARAMETERS
  counter - The counter to increment.
  inc_count - How much to increment the counter by.

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_inc_conn_state_stats_counter
( 
  hdrplog_conn_state_stats_count_type counter,
  uint16                              inc_count
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  REX_DISABLE_PREMPTION();

  hdrplog.conn_state_stats.since_reset_counts[counter] += inc_count;
    /* Increment the specified counter kept since the last AT reset. */

  hdrplog.conn_state_stats.since_call_counts[counter] += inc_count;
    /* Increment the specified counter kept since the beginning of
       the last call. */

  REX_ENABLE_PREMPTION();
} /* hdrplog_inc_conn_state_stats_counter() */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_RESET_CONN_STATE_STATS

DESCRIPTION 
  This function resets the connected state stats as
  specified by the stats reset parameter.

DEPENDENCIES 
  None

PARAMETERS
  stats_reset - Which statistics we should clear (all or just the last 
                call stats).

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_reset_conn_state_stats
( 
  hdrplog_reset_stats_type stats_reset
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  REX_DISABLE_PREMPTION();
  switch ( stats_reset )
  {
    case HDRPLOG_ALL_STATS:
      memset( (void*)&hdrplog.conn_state_stats, 0, 
              sizeof(hdrplog_conn_state_stats_struct_subtype) );
        /* Reset all of the statistics tracked for this log packet. */

      hdrplog.conn_state_stats.version = HDRPLOG_CONN_STATE_STATS_VER;
        /* Set the version number of the packet for future expandability. */

      break;

    case HDRPLOG_LAST_CALL_STATS:
      memset( (void*)hdrplog.conn_state_stats.since_call_counts, 0, 
              HDRPLOG_NUM_CONN_STATE_STATS_COUNTS*sizeof(uint32) );
        /* Reset only the statistics tracked since the start of the
           last call. */

      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Unknown statistics to clear: %d", stats_reset);
      break;
  }
  REX_ENABLE_PREMPTION();
} /* hdrplog_reset_conn_state_stats() */

/*===========================================================================

FUNCTION HDRPLOG_RUTC_CLEAR_LIST

DESCRIPTION
  This function clears the RUTC log packet data and sets the list size.

DEPENDENCIES
  None.

PARAMETERS
  route_update_trigger_code_list_size - The negotiated size of the RUTC list

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrplog_rutc_clear_list
( 
  int route_update_trigger_code_list_size 
)
{
  hdrplog_rutc_list_type *log_pkt_ptr = &hdrplog.rutc_log;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( log_status( LOG_HDR_REVA_RUP_TC_LIST_C ) )
  {
    log_pkt_ptr->num_entries = 0;
    log_pkt_ptr->list_size = (uint8)route_update_trigger_code_list_size;
  }
  
} /* hdrplog_rutc_clear_list() */

/*===========================================================================

FUNCTION HDRPLOG_RUTC_ADD_ENTRY

DESCRIPTION
  This function adds an RUTC entry to the RUTC list log packet.

DEPENDENCIES
  None.

PARAMETERS
  subnet_id - The subnet ID of the RUTC list entry
  subnet_mask - The subnet mask of the RUTC list entry
  route_update_trigger_code - The trigger code of the RUTC list entry
  timestamp - The timestamp of the RUTC list entry
  rup_was_acked - The ACK status of the RUTC list entry

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrplog_rutc_add_entry
( 
  hdraddr_type subnet_id,
  uint8 subnet_mask,
  uint16 route_update_trigger_code,
  uint32 timestamp,
  boolean rup_was_acked
)
{
  hdrplog_rutc_list_type *log_pkt_ptr = &hdrplog.rutc_log;
  hdrlog_reva_rup_tc_list_entry_type list_entry;
  int entry = log_pkt_ptr->num_entries;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( log_status( LOG_HDR_REVA_RUP_TC_LIST_C ) )
  {
    if( log_pkt_ptr->num_entries < log_pkt_ptr->list_size )
    {
      list_entry.subnet_mask = subnet_mask;
      list_entry.rup_was_acked = rup_was_acked;
      list_entry.timestamp = timestamp;
      list_entry.trigger_code = route_update_trigger_code;
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( (void *)list_entry.subnet_id, sizeof(hdraddr_type), 
               (void *)subnet_id, sizeof( hdraddr_type ) );
      
      log_pkt_ptr->entries[entry] = list_entry;
#else
      memcpy( (void *)list_entry.subnet_id, (void *)subnet_id, 
        sizeof( hdraddr_type ) );
  
      memcpy( (void *)&(log_pkt_ptr->entries[entry]), (void *)&list_entry, 
        sizeof( hdrlog_reva_rup_tc_list_entry_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
  
      log_pkt_ptr->num_entries++;
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_FATAL, 
        "Logging more RUTC entries than negotiated RUTC list size" );
    }
  }

} /* hdrplog_rutc_add_entry() */

/*===========================================================================

FUNCTION HDRPLOG_RUTC_COMMIT

DESCRIPTION
  This function commits the RUTC list log packet.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrplog_rutc_commit( void )
{
  LOG_HDR_REVA_RUP_TC_LIST_C_type *log_ptr;
  hdrplog_rutc_list_type *log_pkt_ptr = &hdrplog.rutc_log;

  int entry;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  log_ptr = (LOG_HDR_REVA_RUP_TC_LIST_C_type *)
             log_alloc_ex( (log_code_type) LOG_HDR_REVA_RUP_TC_LIST_C, 
             HDRLOG_VAR_SIZEOF( LOG_HDR_REVA_RUP_TC_LIST_C_type,
                                hdrlog_reva_rup_tc_list_entry_type,
                                log_pkt_ptr->num_entries )
             );

  if( log_ptr != NULL )
  {
    log_ptr->version = HDRLOG_REVA_RUP_TC_LIST_LOG_PKT_VER;
    log_ptr->list_size = log_pkt_ptr->list_size;
    log_ptr->num_entries = log_pkt_ptr->num_entries;

    for( entry = 0; entry < log_pkt_ptr->num_entries; entry++ )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      log_ptr->entries[entry] = log_pkt_ptr->entries[entry];
#else
      memcpy( (void *)&(log_ptr->entries[entry]), 
              (void *)&(log_pkt_ptr->entries[entry]),
               sizeof( hdrlog_reva_rup_tc_list_entry_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
    }

    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "Logging RevA RUP TC List; %entries logged", 
      log_pkt_ptr->num_entries );

    log_commit( (log_type *)log_ptr );
  }

} /* hdrplog_rutc_commit() */

/*=========================================================================

FUNCTION HDRPLOG_GET_SUBPKT_PROTOCOL_DATA

DESCRIPTION
  This function gets value for the fields, which are related to HDRCP, in
  subpacket Protocol Data. 

DEPENDENCIES
  None

PARAMETERS
  subpkt_ptr - pointer to the subpacket to be set

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_get_subpkt_protocol_data
(
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type *subpkt_ptr
)
{
  /* Sector ID */
  uint32 sector_id_24 = 0;

  /* Country code */
  uint16 country_code = 0;

  /* LUP config result. */
  hdrlupc_config_result_type *lupc_cfg;

  /* Link info. */
  sys_link_type link_info;

  /* Neighbor set info. */
  hdrovhd_sp_msg_nset_info_type nset_info;

  /* Pilot group ID. */
  uint8 pilot_gid;

  /* SCP config. */
  hdrscp_config_params_type scp_cfg;

  /* AMAC SCM config */
  hdrscmamac_config_struct_type scm_amac;

  /* RMAC config. */
  hdrscmrmac3_config_struct_type scm_rmac;

  /* FMAC SCM config */
  hdrscmfmac_config_struct_type scm_fmac;

  /* AuxiliaryPilotChannelGain info. */
  hdrscmrmac3_aux_pilot_struct_type aux_pilot_info;

  /* AP message */
  hdrovhd_ap_msg_type ap_msg;

  /* Loop index */
  uint8 i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AT status: IDLE, ACCESS, CONNECTED, INACTIVE. */
  hdrplog_update_state_info();
  subpkt_ptr->at_state = (uint8)hdrplog.at_state;

  /* Current PCCC value. */
  subpkt_ptr->current_pccc = hdridlec_get_current_pccc();

  /* Random Handoff attribute. */
  lupc_cfg = (hdrlupc_config_result_type *)hdrlupc_get_config_result_handle();
  subpkt_ptr->ran_ho_is_accept = lupc_cfg->ran_ho.is_accepted;
  subpkt_ptr->ran_ho_value = lupc_cfg->ran_ho.value;

  /* Channel Number, channel band, pilot PN. */
  if ( E_SUCCESS == hdrovhd_get_qc_link(&link_info, HDRHAI_LOGGING_MODULE) )
  {
    subpkt_ptr->channel_num = (uint16)link_info.channel.chan_num;
    subpkt_ptr->channel_band = (uint8)link_info.channel.band;
    subpkt_ptr->pilot_pn = (uint16)link_info.pilot_pn;
  }

  /* Color code. */
  hdrovhd_get_color_code(&subpkt_ptr->color_code, HDRHAI_LOGGING_MODULE);

  /* Least Significant 24 bit of SectorID. */
  hdrovhd_get_sector_id_24(&sector_id_24, HDRHAI_LOGGING_MODULE);
  subpkt_ptr->sector_id_24 = sector_id_24;

  /* Subnet Mask. */
  hdrovhd_get_subnet_mask(&subpkt_ptr->subnet_mask, HDRHAI_LOGGING_MODULE);

  /* Country code. */
  hdrovhd_get_sp_msg_country_code(&country_code, HDRHAI_LOGGING_MODULE);
  subpkt_ptr->country_code = country_code;

  /* Number of Route Update Trigger Code in the list. */
  subpkt_ptr->rutc_list_cnt = (uint8)hdrovhd_get_rutc_list_count();

  /* Neighbor set info. */
  if ( E_SUCCESS == 
         hdrovhd_get_sp_msg_pilot_gid_and_nset_info(&pilot_gid, &nset_info, HDRHAI_LOGGING_MODULE) )
  {
    subpkt_ptr->num_neighbors = nset_info.num_neighbors;
    for ( i = 0; i < HDRLOG_MAX_SP_NEIGHBORS; i++ )
    {
      subpkt_ptr->nset_channel_num[i] = nset_info.nset[i].link.channel.chan_num;
      subpkt_ptr->nset_pilot_pn[i] = nset_info.nset[i].link.pilot_pn;
    }   
  }

  /* Sector access maximum rate */
  if ( E_SUCCESS == 
         hdrovhd_get_ap_msg( &ap_msg, HDRHAI_LOGGING_MODULE ) )
  {
    subpkt_ptr->sector_access_max_rate = ap_msg.sector_access_max_rate;
  }

  /* Current personality. */
  subpkt_ptr->curr_personality = hdrscmdb_get_current_personality();

  /* Number of personalities, config token. */
  hdrscmdb_get_scp_session_info(&scp_cfg);
  subpkt_ptr->personality_count = scp_cfg.personality_count;
  subpkt_ptr->support_gaup_session_config_token = scp_cfg.support_gaup_session_config_token;
	
  /* Protocol subtypes. */
  for ( i = 0; i < HDRLOG_PROTOCOL_TYPE_ENUM_MAX; i++)
  {
    subpkt_ptr->protocol_subtypes[i] = 
      hdrscp_get_current_subtype((hdrhai_protocol_name_enum_type)i);
  }
    	
  /* RMAC config. */
  hdrscmdb_get_rmac_inuse_config_and_verno( &scm_rmac,
                                            sizeof(scm_rmac),
                                            NULL,
                                            0,
                                            HDRSCP_SUB3_RMAC );
  for ( i = 0; i < HDRLOG_RMAC3_MAX_NUM_MAC_FLOWS; i++ )
  {
    subpkt_ptr->rmac3_flow_tx_mode[i] = scm_rmac.flows[i].tx_mode;
  }
  subpkt_ptr->rmac3_rate_1m8_supported = scm_rmac.rate_1m8_supported;

  /* RMAC attributes. */
  hdrscmdb_get_inuse_attrib( HDRHAI_FTC_MAC_PROTOCOL,
                             HDRSCMFMAC_ATTRIB_DRC_LOCK,
                             &subpkt_ptr->drc_trans_offset[0],
                             HDRSCP_ENH_FMAC );  

  hdrscmdb_get_inuse_attrib( HDRHAI_RTC_MAC_PROTOCOL,
                             0x0000,
                             &aux_pilot_info,
                             HDRSCP_SUB3_RMAC );
  subpkt_ptr->aux_pilot_min_payload = aux_pilot_info.min_payload;

  /* AMAC. */
  hdrscmdb_get_inuse_config_and_verno( HDRHAI_AC_MAC_PROTOCOL,
                                       &scm_amac,
                                       NULL,
                                       0 );
  subpkt_ptr->term_acc_rate_max = scm_amac.term_acc_rate_max;
  subpkt_ptr->probe_backoff = scm_amac.init_config.probe_backoff;  
  subpkt_ptr->probe_seq_backoff = scm_amac.init_config.probe_backoff;
  subpkt_ptr->probe_seq_max = scm_amac.init_config.probe_seq_max;

  /* FMAC. */
  hdrscmdb_get_inuse_config_and_verno( HDRHAI_FTC_MAC_PROTOCOL,
                                       &scm_fmac,
                                       NULL,
                                       0 );
  subpkt_ptr->mup_enabled = scm_fmac.mup_enabled;
  subpkt_ptr->null_rate_drc_38_4_enable = scm_fmac.null_rate_drc_38_4_enable;

  /* Session close reason. */
  subpkt_ptr->session_close_reason = (uint8)hdrsmp_get_close_reason();

  /* Keep Alive interval. */
  subpkt_ptr->ka_current_interval = hdrsmpka_get_current_ka_interval();

} /* hdrplog_get_subpkt_protocol_data */
