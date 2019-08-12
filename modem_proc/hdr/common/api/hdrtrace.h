#ifndef HDRTRACE_H
#define HDRTRACE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==


         H D R   S O F T W A R E   T R A C E   S E R V I C E S

DESCRIPTION
  This header file contains macros and definitions for HDR software tracing.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrtrace.h_v   1.37   18 Jul 2002 17:09:12   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrtrace.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
02/26/14   vko     FR 17879: Extended EARFCN
12/18/12   vko     Added new event EVENT_HDR_MSG_RX_PAGE
03/09/12   cnx     Added two traces to track connection attempt and release.
09/23/11   smd     SU API cleanup.
07/27/11   cnx     Merged Optimized Handoff changes.
05/31/11   cnx     Fixed compiler errors. 
05/31/11   cnx     Added HDRTRACE_IDLE_CONNECTION_DENIED_REASON,
                   HDRTRACE_IDLE_CONNECTION_SETUP_FAIL_REASON,
                   and HDRTRACE_IDLE_CONNECT_REASON.
10/30/09   pba     Added ehrpd support.
11/21/08   pba     Added events for SLPQH & OverheadCache enhancement features.
10/23/08   wsh     Fixed critical/high lint warnings
06/09/08   rmg     Added EVENT_RMAC_CARRIER_STATE_CHANGED.
05/30/08   wsh     Added EVENT trace when reacq fails and DDARF kicks in
08/07/07   mpa     Fixed Lint errors.
04/30/07   pba     Added HDRTRACE_DOS_MO_DOS_STATUS.
03/06/07   wsh     Removal of RVCT compiler warnings
03/05/07   sju     Added support to handle the same message ID with different 
                   subtype. 
01/08/07   yll     Added support for CSNA.
09/04/05   mpa     Added HDRTRACE_LMAC_UPDATE_QSM_STATUS
08/01/06   sju     Added HDRTRACE_IDLE_SET_SLEEP_DURATION and HDRTRACE_SCM_SESSION_CHANGED.
06/22/06   mpa     Mainlined FEATURE_DIAG_TRACE and FEATURE_HDR_DEBUG_TRACE.
11/14/05   mpa     Added HDRTRACE_OVHD_BCMCS_CHAN_CHANGE
09/05/05   mpa     More BCMCS events, SessionClose event.
04/14/05   mpa     Added new BCMCS events
04/04/05   pba     Removed Lint errors
03/24/05   pba     Fixed compiler warning in off-target testing code
03/24/05   pba     Lint cleanup
02/25/05   pba     Added new event EVENT_HDR_RUP_DIST_BASED_REG
08/31/04   kss     Added ACMAC persistence pass/fail events.
11/05/03   mpa     Added reason argument to HDRTRACE_LMAC_NETWORK_LOST event
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDRPROT SSID
06/11/03   mpa     Added HDRTRACE_HDRSMP_SESSION_CLOSED
07/18/02   mpa     Added hybrid trace events
03/27/02   mpa     Fixed smpka update_timer msg fatal reporting
09/27/01   sh      Removed the in-configue bit from the protocol ID  for SLP
                   related trace msgs
09/07/01   sh      Renamed keep alive related enums
07/09/01   nmn     Added SMPKA trace events
05/17/01   sh      Added subtype for message, state and command fields and
                   changed EVET_HDR to either EVENT_HDR or TRACE_HDR
04/17/01   vas     Corrections in macros.
04/15/01   vas     Added macro for if AP msg ignored due to link being diff.
                   Added HDRTRACE_OVHD_INFO_UPDATED
04/05/01   vas     Corrected HDRTRACE_HMP_SENT_MSG macro
04/04/01   sh      Removed lookup table, changed cmd, msg and state size to 2
                   bytes
03/28/01   vas     Added trace for hdrovhd_get_subnet
11/16/00   dna     Created module.

=============================================================================*/

/* <EJECT> */

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "target.h"
#include "customer.h"
#include "event.h"
#include "comdef.h"
#include "hdrhai.h"
#include "trace.h"
#include "hdrcom_api.h"

/*=============================================================================

                            Data Declarations

=============================================================================*/

typedef struct
{
  char name[20];
  char **cmd_tbl;
  char **msg_tbl;
  char **state_tbl;
}hdrtrace_table_type;

/* External Declarations */
extern hdrtrace_table_type hdrtrace[];

/* See hdrtrace_msg table to see where this number comes from */
#define HDRTRACE_MSG_CONFIG_REQ 51
#define HDRTRACE_ALL_PROTOCOLS 0xff
#define HDRTRACE_APP_DPA    0         /* Default Packet App Bound to AN/SN   */
#define HDRTRACE_APP_MPA_EMPA_MMPA 4 /* MPA/EMPA/MMPA Bound to AN/SN */
/* <EJECT> */

/*===========================================================================

FUNCTION HDRTRACE_REPORT0

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report0( trace_id_enum_type event_id );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT1

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report1(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT2

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report2(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT3

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report3(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT4

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report4(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size,
  uint32 item4,
  uint8 item4_size
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT5

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report5(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size,
  uint32 item4,
  uint8 item4_size,
  uint32 item5,
  uint8 item5_size
);
/*===========================================================================

FUNCTION HDRTRACE_GET_SUBTYPE

DESCRIPTION
  This function gets subtype value which will be used only for the purpose of 
  displaying correct event in QXDM 

DEPENDENCIES
  None

PARAMETERS
  app_subtype - Application subtype

RETURN VALUE
  subtype     -  subtype value used to generate event id

SIDE EFFECTS
  None

===========================================================================*/

uint8 hdrtrace_get_subtype( hdrhai_app_subtype_enum_type app_subtype );

/* Protocol specific                         */
/* message, state and command bit structure  */
/* bit 15-10 protocol                        */
/* bit 9-6   sub type                        */
/* bit 5-0   messaeg id, state or command id */
/*                                           */
/* Message applied to all protocols          */
/* bit 15-10 0                               */
/* bit 9-6   0                               */
/* bit 5-0   msg_id - 0x50                   */



/* =======================================================================
 * =======================================================================
 *             Trace macros for target builds using QXDM events
 * =======================================================================
 * ======================================================================= */

#define HDRTRACE_MSG( protocol, msg, state )\
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_MSG_RX, \
              (uint32) HDRTRACE_CMSG(protocol, 0, msg), 2, \
              (uint32) protocol, 1); \
   HDRTRACE_CURRENT_STATE(protocol, state)

#define HDRTRACE_RXMSG_IGNORED_STATE( protocol, msg, state )\
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_RXMSG_IGNORED_STATE, \
              (uint32) HDRTRACE_CMSG(protocol, 0, msg), 2, \
              (uint32) protocol, 1); \
   HDRTRACE_CURRENT_STATE(protocol, state)

#define HDRTRACE_RXMSG_IGNORED_SEQ( protocol, msg, state )\
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_RXMSG_IGNORED_SEQ, \
             (uint32) HDRTRACE_CMSG(protocol, 0, msg), 2, \
             (uint32) protocol, 1); \
   HDRTRACE_CURRENT_STATE(protocol, state)

#define HDRTRACE_TXMSG_DROPPED(protocol, msg)\
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_TXMSG_DROPPED, \
                    (uint32) HDRTRACE_CMSG(protocol, 0, msg), 2, \
                    (uint32) protocol, 1)

#define HDRTRACE_STATE( protocol, from, to )\
   hdrtrace_report3( (trace_id_enum_type) EVENT_HDR_STATE_CHANGE, \
              (uint32) protocol, 1, \
              (uint32) HDRTRACE_CSTATE(protocol, 0, from), 2, \
              (uint32) HDRTRACE_CSTATE(protocol, 0, to), 2)

#define HDRTRACE_STATE_ENTRY( protocol, state )\
   hdrtrace_report2( TRACE_HDR_STATE_ENTRY, \
              (uint32) protocol, 1, \
              (uint32) HDRTRACE_CSTATE(protocol, 0, state), 2)


#define HDRTRACE_TXMSG_ABORTED( protocol, msg, state )\
   hdrtrace_report3( TRACE_HDR_TXMSG_ABORTED, \
              (uint32) HDRTRACE_CMSG(protocol, 0, msg), 2, \
              (uint32) protocol, 1, \
              (uint32) HDRTRACE_CSTATE(protocol, 0, state), 2)

#define HDRTRACE_CMD_IGNORED_STATE( protocol, cmd, sender, state )\
   hdrtrace_report4( TRACE_HDR_CMD_IGNORED_STATE, \
             (uint32) HDRTRACE_CCMD(protocol, 0, cmd), 2, \
             (uint32) HDRTRACE_CSTATE(protocol, 0, state), 2, \
             (uint32) sender, 1, \
             (uint32) protocol, 1)


#define HDRTRACE_IND_IGNORED_STATE( protocol, ind_name, state )\
   hdrtrace_report3( TRACE_HDR_IND_IGNORED_STATE, \
              (uint32) ind_name, 1, \
              (uint32) protocol, 1, \
              (uint32) HDRTRACE_CSTATE(protocol, 0, state), 2)

#define HDRTRACE_INIT_DEFAULT( protocol )\
   hdrtrace_report1( TRACE_HDR_INIT_DEFAULT, \
              (uint32) protocol, 1)

#define HDRTRACE_INIT_NV( protocol )\
   hdrtrace_report1( TRACE_HDR_INIT_NV, \
              (uint32) protocol, 1)


/********
 * ALMP *
 ********/



#define HDRTRACE_ALMP_OBEYING_REDIRECTION() \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_ALMP_OBEYING_REDIRECTION )

#define HDRTRACE_ALMP_CONNECTION_CLOSED() \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_ALMP_CONNECTION_CLOSED )

#define HDRTRACE_HDRMC_SD_TIMER_SIG_ALMP( state )\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_ALMP_T_SD_RESELECT, \
   (uint32) HDRTRACE_CSTATE(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, 0, state),2)

#define HDRTRACE_ALMP_CONNECTION_OPENED()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_ALMP_CONNECTION_OPENED )

#define HDRTRACE_USR_ALMP( usr_cmd, state )\
   HDR_MSG_PROT_2 ( MSG_LEGACY_MED,\
             "ALMP: Usr_cmd=%d State=%d", (mc_name_type) usr_cmd, state)

#define HDRTRACE_ALMP_CONNECTION_IS_OPEN( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_ALMP_CONNECTION_IS_OPEN, \
   (uint32) response, 1, \
   (uint32) HDRTRACE_CSTATE(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, 0, state), 2,\
   (uint32) caller, 1)

#define HDRTRACE_ALMP_SYSTEM_IS_ACQUIRED( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_ALMP_SYSTEM_IS_ACQUIRED, \
   (uint32) response, 1, \
   (uint32) HDRTRACE_CSTATE(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, 0, state), 2,\
   (uint32) caller, 1)

#define HDRTRACE_ALMP_LTE_RESELECT( frequency, cell )\
   hdrtrace_report2( TRACE_HDR_ALMP_LTE_RESELECT, \
   (uint32) frequency, sizeof(lte_earfcn_t), \
   (uint32) cell, 2)

/*******************
 * CONNECTED STATE *
 *******************/

#define HDRTRACE_CON_GET_ADVERTISED_SUSPEND_TIME( caller, state ) \
   hdrtrace_report2( TRACE_HDR_CON_GET_ADVERTISED_SUSPEND_TIME, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_CONNECTED_STATE_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

/*******
 * HMP *
 *******/

#define HDRTRACE_HMP_QUEUED_MSG(protocol, msg) \
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_HMP_QUEUED_MSG, \
                    (uint32) HDRTRACE_CMSG(protocol, 0, msg), 2,\
                    (uint32) protocol, 1)

#define HDRTRACE_HMP_SENT_MSG(protocol, msg, chan, is_reliable) \
   hdrtrace_report4( (trace_id_enum_type) EVENT_HDR_HMP_SENT_MSG, \
                      (uint32) HDRTRACE_CMSG(protocol, 0, msg) , 2 ,\
                      (uint32) protocol, 1, \
                      (uint32) chan, 1, \
                      (uint32) is_reliable, 1)

#define HDRTRACE_HMP_ABORTING_ACMAC_ACTIVATION() \
 hdrtrace_report0((trace_id_enum_type) EVENT_HDR_HMP_ABORTING_ACMAC_ACTIVATION)

#define HDRTRACE_HMP_WAITING_TO_ACTIVATE_ACMAC() \
   hdrtrace_report0( TRACE_HDR_HMP_WAITING_TO_ACTIVATE_ACMAC )

#define HDRTRACE_HMP_SESSION_CLOSED(reason, cnt) \
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_HMP_SESSION_CLOSED, \
                     (uint32) reason, 1, \
                     (uint32) cnt, 1)

/**************
 * IDLE STATE *
 **************/


#define HDRTRACE_HDRMC_IDLE_NOT_RCV_CONFIG_RES_IN_2_SEC() \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_IDLE_T_CONFIG_RSP )

#define HDRTRACE_HDRMC_IDLE_CON_SETUP_TIMEOUT() \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_IDLE_T_AT_SETUP)

#define HDRTRACE_HDRMC_IDLE_SUSPEND_TIMER_SIG( state )\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_IDLE_T_SUSPEND, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_IDLE_STATE_PROTOCOL, 0, state), 2)

#define HDRTRACE_HDRMC_IDLE_CONNECTION_DENIED() \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_IDLE_CONNECTION_DENIED )

#define HDRTRACE_IDLE_RESELECT_IS_ALLOWED( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_IDLE_RESELECT_IS_ALLOWED, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_IDLE_STATE_PROTOCOL, 0, state), 2, \
     (uint32) caller, 1)
     
#define HDRTRACE_IDLE_SET_SLEEP_DURATION( slot_cycle, gear) \
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_IDLE_SET_SLEEP_DURATION, \
                     (uint32) slot_cycle, 1, \
                     (uint32) gear, 1)

#define HDRTRACE_IDLE_REACQ_FAIL_DDRAF( fail_count, max_fail, slot_cycle ) \
   hdrtrace_report3( (trace_id_enum_type) EVENT_HDR_IDLE_REACQ_FAIL_DDARF, \
                     (uint32) fail_count, 1, \
                     (uint32) max_fail, 1, \
                     (uint32) slot_cycle, 1 )

#define HDRTRACE_IDLE_CONNECTION_DENIED_REASON( reason ) \
   hdrtrace_report1( (trace_id_enum_type) TRACE_HDR_IDLE_CONNECTION_DENIED_REASON, \
     (uint32) reason, 1 )

#define HDRTRACE_IDLE_CONNECTION_SETUP_FAIL_REASON( reason ) \
   hdrtrace_report1( (trace_id_enum_type) TRACE_HDR_IDLE_CONNECTION_SETUP_FAIL_REASON, \
     (uint32) reason, 1 )

#define HDRTRACE_IDLE_CONNECT_REASON( reason ) \
   hdrtrace_report1( (trace_id_enum_type) TRACE_HDR_IDLE_CONNECT_REASON, \
     (uint32) reason, 1 )

#define HDRTRACE_IDLE_MSG_RX_PAGE() \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_MSG_RX_PAGE )

/**************
 * INIT STATE *
 **************/

#define HDRTRACE_HDRMC_INIT_SYNC_MSG_TIMER_SIG( state )\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_INIT_T_SYNC_ACQ, \
   (uint32) HDRTRACE_CSTATE(HDRHAI_INITIALIZATION_STATE_PROTOCOL, 0, state), 2)

#define HDRTRACE_HDRMC_INIT_PROTOCOL_MISMATCH()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_INIT_PROTOCOL_MISMATCH )


/*********************
 * OVERHEAD MESSAGES *
 *********************/


#define HDRTRACE_OVHD_INFO_UPDATED( ) \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_OVHD_INFO_CURRENT )

#define HDRTRACE_HDRMC_OVHD_QC_TIMER_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_OVHD_T_QC_SUPERVISION); \
   HDRTRACE_CURRENT_STATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, state)

#define HDRTRACE_HDRMC_OVHD_SP_TIMER_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_OVHD_T_SP_SUPERVISION); \
   HDRTRACE_CURRENT_STATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, state)

#define HDRTRACE_HDRMC_OVHD_AP_TIMER_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_OVHD_T_AP_SUPERVISION); \
   HDRTRACE_CURRENT_STATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, state)

#define HDRTRACE_HDRMC_OVHD_BC_SUP_TIMER( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_OVHD_T_BC_SUPERVISION); \
   HDRTRACE_CURRENT_STATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, state)


#define HDRTRACE_OVHD_IGNORED_MSG_UNEXPECTED_LINK(msg, e_chan, e_pn, r_chan, r_pn) \
   hdrtrace_report5( (trace_id_enum_type) EVENT_HDR_OVHD_IGNORED_MSG_UNEXPECTED_LINK,\
     (uint32) HDRTRACE_CMSG(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, msg), 2,\
     (uint32) e_chan, 2,\
     (uint32) e_pn, 2,\
     (uint32) r_chan, 2,\
     (uint32) r_pn, 2)

#define HDRTRACE_OVHD_IGNORED_SP_MSG_DIFF_SEC_SIG( exp_sig , rx_sig ) \
   hdrtrace_report2((trace_id_enum_type)\
   EVENT_HDR_OVHD_IGNORED_SP_MSG_DIFF_SEC_SIG,\
   (uint32) exp_sig, 4, (uint32) rx_sig, 4 )

#define HDRTRACE_OVHD_IGNORED_AP_MSG_DIFF_ACC_SIG( exp_sig , rx_sig )\
   hdrtrace_report2((trace_id_enum_type)\
   EVENT_HDR_OVHD_IGNORED_AP_MSG_DIFF_ACC_SIG,\
   (uint32) exp_sig, 4, (uint32) rx_sig, 4 )


#define HDRTRACE_OVHD_IGNORED_SP_MSG_DIFF_SEC_ID( ) \
   hdrtrace_report0((trace_id_enum_type)\
   EVENT_HDR_OVHD_IGNORED_SP_MSG_DIFF_SEC_ID)

#define HDRTRACE_OVHD_SP_MSG_RX( ) \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_OVHD_SP_MSG_RX )

#define HDRTRACE_OVHD_AP_MSG_RX( ) \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_OVHD_AP_MSG_RX )

#define HDRTRACE_OVHD_BC_MSG_RX( ) \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_OVHD_BC_MSG_RX )

#define HDRTRACE_OVHD_GET_SP_MSG( caller, state )\
   hdrtrace_report2( TRACE_HDR_OVHD_GET_SP_MSG, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

#define HDRTRACE_OVHD_GET_AP_MSG( caller, state )\
   hdrtrace_report2( TRACE_HDR_OVHD_GET_AP_MSG, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

#define HDRTRACE_OVHD_INFO_IS_UPDATED( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_OVHD_INFO_IS_UPDATED, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

/* Trace for if Access Parameters have been updated.
   If response==FALSE, it implies
   that the AccessParameters Supervision Timer has been started */
#define HDRTRACE_OVHD_AP_IS_UPDATED( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_OVHD_AP_IS_UPDATED, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

#define HDRTRACE_OVHD_ORNL_IS_UPDATED( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_OVHD_ORNL_IS_UPDATED, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

#define HDRTRACE_OVHD_GET_COLOR_CODE( caller, state )\
   hdrtrace_report2( TRACE_HDR_OVHD_GET_COLOR_CODE, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

#define HDRTRACE_OVHD_GET_SECTOR_ID_24( caller, state )\
   hdrtrace_report2( TRACE_HDR_OVHD_GET_SECTOR_ID_24, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

#define HDRTRACE_OVHD_MSG_IGNORED_STATE( msg, state )\
   hdrtrace_report2( TRACE_HDR_OVHD_MSG_IGNORED_STATE, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, msg), 2, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2)

#define HDRTRACE_OVHD_AP_MSG_IGNORED_STATE( msg, state )\
   hdrtrace_report2( TRACE_HDR_OVHD_MSG_IGNORED_STATE, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_AC_MAC_PROTOCOL, 0, msg), 2, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2)

#define HDRTRACE_OVHD_MSG_IGNORED_REDUNDANT( msg )\
   hdrtrace_report1( TRACE_HDR_OVHD_MSG_IGNORED_REDUNDANT, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, msg), 2 )

#define HDRTRACE_OVHD_QC_MSG_RX()\
   hdrtrace_report0( TRACE_HDR_OVHD_QC_MSG_RX )


#define HDRTRACE_OVHD_IGNORED_AP_MSG_UNEXPECTED_LINK( exp_link, rx_link )\
   hdrtrace_report4( TRACE_HDR_OVHD_IGNORED_AP_MSG_UNEXPECTED_LINK,\
                     (uint32) exp_link.channel.chan_num, 2, \
                     (uint32) exp_link.pilot_pn, 2, \
                     (uint32) rx_link.channel.chan_num, 2, \
                     (uint32) rx_link.pilot_pn, 2 )

#define HDRTRACE_OVHD_FIND_CACHED_MSG( search_result )\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_OVHD_FIND_CACHED_MSG, \
                     (uint32) search_result, 1 )

#define HDRTRACE_OVHD_PRE_REG_IS_ALLOWED( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_OVHD_PRE_REG_IS_ALLOWED, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

 /****************
 * ROUTE UPDATE *
 ****************/

#define HDRTRACE_HDRMC_RUP_CONN_SETUP_TIMER_SIG( state )\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_RUP_T_CONNECTION_SETUP, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_ROUTE_UPDATE_PROTOCOL, 0, state), 2)
     
#define HDRTRACE_RUP_DIST_BASED_REG( sector_id_24 ) \
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_RUP_DIST_BASED_REG, \
                     (uint32) sector_id_24, 3)

/*******
 * SLP *
 *******/

#define HDRTRACE_SLP_MAX_RETRIES(protocol, msg)\
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_SLP_MAX_RETRIES,\
    (unit32) (protocol & 0x7f), 1, \
    (uint32) HDRTRACE_CMSG((protocol & 0x7f), 0, msg), 2)

#define HDRTRACE_SLP_AC_MSG_TX(protocol, msg, subtype)\
   hdrtrace_report2( TRACE_HDR_SLP_AC_MSG_TX,\
       (uint32) (protocol & 0x7f), 1,\
     (uint32) HDRTRACE_CMSG((protocol & 0x7f), \
     hdrtrace_get_subtype((hdrhai_app_subtype_enum_type) subtype), msg), 2)

#define HDRTRACE_SLP_RTC_MSG_TX(protocol, msg, subtype)\
   hdrtrace_report2( TRACE_HDR_SLP_RTC_MSG_TX,\
       (uint32) (protocol & 0x7f), 1,\
     (uint32) HDRTRACE_CMSG((protocol & 0x7f), \
     hdrtrace_get_subtype((hdrhai_app_subtype_enum_type) subtype), msg), 2)

#define HDRTRACE_SLP_ABORT_MSG(protocol, msg)\
   hdrtrace_report2( TRACE_HDR_SLP_ABORT_MSG,\
       (uint32) (protocol & 0x7f), 1,\
       (uint32) HDRTRACE_CMSG((protocol & 0x7f), 0, msg), 2)

#define HDRTRACE_SLP_ACK_MSG( )\
   hdrtrace_report0( TRACE_HDR_SLP_ACK_MSG )

#define HDRTRACE_SLP_SLPQH_TIMER_STARTED() \
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_SLP_SLPQH_TIMER_STARTED )

#define HDRTRACE_SLP_SLPQH_TIMER_STOPPED( reason ) \
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_SLP_SLPQH_TIMER_STOPPED, \
                      (uint32) reason, 1)

#define HDRTRACE_SLP_SLPQH_NUM_PENDING_MSGS( num_pending_msgs ) \
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_SLP_SLPQH_NUM_PENDING_MSGS, \
                     (uint32) num_pending_msgs, 1) 

/********
 * LMAC *
 ********/
#define HDRTRACE_LMAC_ACQ_FAIL_PILOT()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_LMAC_ACQ_FAIL_PILOT )

#define HDRTRACE_LMAC_ACQ_SUCCESS()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_LMAC_ACQ_SUCCESS )

#define HDRTRACE_LMAC_NETWORK_LOST(reason)\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_NETWORK_LOST, \
                     (uint32) reason, 1 )

#define HDRTRACE_LMAC_IDLE_HO(new_pilot)\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_IDLE_HO,\
                     (uint32) new_pilot, 2 )

#define HDRTRACE_LMAC_CHAN_CHANGE_COMPLETE()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_LMAC_CHAN_CHANGE_COMPLETE )

#define HDRTRACE_LMAC_ACCESS_HO_NEEDED(suggested_pilot)\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_ACCESS_HO_NEEDED,\
                     (uint32) suggested_pilot, 2 )

#define HDRTRACE_LMAC_ACCESS_HO_COMPLETE(new_pilot)\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_ACCESS_HO_COMPLETE,\
                     (uint32) new_pilot, 2)

#define HDRTRACE_LMAC_ACQUIRE(channel) \
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_ACQUIRE,\
                     (uint32) channel, 2 )

#define HDRTRACE_LMAC_CHANGING_CC_HASH(cc_hash)\
   hdrtrace_report1((trace_id_enum_type) EVENT_HDR_LMAC_CHANGING_CC_HASH, \
   (uint32) cc_hash, 1)

#define HDRTRACE_LMAC_IDLE_CHAN_CHANGE(channel)\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_IDLE_CHAN_CHANGE,\
                      (uint32) channel, 2 )

#define HDRTRACE_LMAC_WAIT_FOR_ACMAC_ABORT()\
   hdrtrace_report0( TRACE_HDR_LMAC_WAIT_FOR_ACMAC_ABORT )

#define HDRTRACE_LMAC_WAIT_FOR_RTCMAC_STOPTX()\
   hdrtrace_report0( TRACE_HDR_LMAC_WAIT_FOR_RTCMAC_STOPTX )

#define HDRTRACE_LMAC_WAKEUP()\
   hdrtrace_report0( TRACE_HDR_LMAC_WAKEUP )

#define HDRTRACE_LMAC_ACCESS_MODE_ENTERED()\
   hdrtrace_report0( TRACE_HDR_LMAC_ACCESS_MODE_ENTERED )

#define HDRTRACE_LMAC_PILOT_MEAS_REQUIRED()\
   hdrtrace_report0( TRACE_HDR_LMAC_PILOT_MEAS_REQUIRED )

#define HDRTRACE_LMAC_CHAN_MEAS_COMPLETE()\
   hdrtrace_report0( TRACE_HDR_LMAC_CHAN_MEAS_COMPLETE )

#define HDRTRACE_LMAC_SLEEP()\
   hdrtrace_report0( TRACE_HDR_LMAC_SLEEP )

#define HDRTRACE_LMAC_UPDATE_BC_STATUS(status)\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_UPDATE_BC_STATUS,\
                     (uint32) status, 1 )

#define HDRTRACE_LMAC_SET_BCMCS_PAGE_CYCLE(page_cycle)\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_SET_BCMCS_PAGE_CYCLE,\
                     (uint32) page_cycle, 1 )

#define HDRTRACE_LMAC_UPDATE_QSM_STATUS(status)\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_LMAC_UPDATE_QSM_STATUS,\
                     (uint32) status, 1 )

/*********
 * CCMAC *
 *********/
#define HDRTRACE_HDRRX_CMAC_TIMER_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_CMAC_T_SUPERVISION); \
   HDRTRACE_CURRENT_STATE(HDRHAI_CC_MAC_PROTOCOL, state)


#define HDRTRACE_CMAC_IS_ACTIVE( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_CMAC_IS_ACTIVE, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_CC_MAC_PROTOCOL, 0,state), 2, \
     (uint32) caller, 1)

/*********
 * ACMAC *
 *********/

#define HDRTRACE_AMAC_START_ACCESS(num_step, max_seqs, init_pwr_div_2)\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMAC_START_ACCESS); \
   hdrtrace_report3( TRACE_HDR_AMAC_START_ACCESS, (uint32) num_step, 4, \
                     (uint32) max_seqs, 4, (uint32) init_pwr_div_2, 4)

#define HDRTRACE_AMAC_PROBING_STOPPED()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMAC_PROBING_STOPPED)

#define HDRTRACE_AMAC_ACCESS_COMPLETE()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMAC_ACCESS_COMPLETE)

#define HDRTRACE_AMAC_ACCESS_ABORTED()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMAC_ACCESS_ABORTED)

#define HDRTRACE_AMAC_MAX_PROBES()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMAC_MAX_PROBES)

#define HDRTRACE_AMAC_IS_ACTIVE( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_AMAC_IS_ACTIVE, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_AC_MAC_PROTOCOL, 0, state), 2, \
     (uint32) caller, 1)

#define HDRTRACE_AMAC_GET_LAST_TX_PWR( caller, state )\
   hdrtrace_report2( TRACE_HDR_AMAC_GET_LAST_TX_PWR, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_AC_MAC_PROTOCOL, 0, state), 2, \
     (uint32) caller, 1)

#define HDRTRACE_AMAC_SENDING_PROBE(tx_pwr_div_2)\
   hdrtrace_report1( TRACE_HDR_AMAC_SENDING_PROBE, \
             (uint32) tx_pwr_div_2, 4)

#define HDRTRACE_AMAC_PERSISTENCE_FAILED()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMAC_PERSISTENCE_FAILED)

#define HDRTRACE_AMAC_PERSISTENCE_PASSED()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMAC_PERSISTENCE_PASSED)


/**********
 * FTCMAC *
 **********/

#define HDRTRACE_FMAC_DROP_PKT()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_FMAC_DROP_PKT )

#define HDRTRACE_FMAC_IS_ACTIVE( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_FMAC_IS_ACTIVE, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_FTC_MAC_PROTOCOL), 0, state), 2, \
     (uint32) caller, 1)


/**********
 * RTCMAC *
 **********/

#define HDRTRACE_HDRTX_RMAC_RATELIM_TIMER_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_RMAC_T_RATE_LIMIT); \
   HDRTRACE_CURRENT_STATE(HDRHAI_RTC_MAC_PROTOCOL, state)


#define HDRTRACE_RMAC_TX_STARTED()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_RMAC_TX_STARTED)

#define HDRTRACE_RMAC_TX_STOPPED()\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_RMAC_TX_STOPPED)

#define HDRTRACE_RMAC_IS_ACTIVE( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_RMAC_IS_ACTIVE, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_RTC_MAC_PROTOCOL, 0, state), 2, \
     (uint32) caller, 1)

#define HDRTRACE_RMAC_CARRIER_STATE_CHANGED( phy_chan, band_class, \
   chan_num, current_state, new_state )\
   hdrtrace_report5( (trace_id_enum_type) EVENT_RMAC_CARRIER_STATE_CHANGED, \
     (uint32) phy_chan, 1, \
     (uint32) band_class, 1, \
     (uint32) chan_num, 2, \
     (uint32) current_state, 1, \
     (uint32) new_state, 1 )

/**********
 * SMP *
 **********/

#define HDRTRACE_HDRMC_KA_TIMER_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_SMP_T_KEEP_ALIVE); \
   HDRTRACE_CURRENT_STATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL, state)

#define HDRTRACE_HDRSMP_KA_TIMER_IGNORED_STATE( state )\
   hdrtrace_report1( TRACE_HDR_SMP_T_KEEP_ALIVE_IGNORED_STATE, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL, 0, state), 2)

#define HDRTRACE_HDRSMP_CONFIG_RSP_TIMER_IGNORED_STATE( state ) \
   hdrtrace_report1( TRACE_HDR_SMP_T_CONFIG_RSP_IGNORED_STATE, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_SESSION_MANAGEMENT_PROTOCOL, 0, state), 2)

#define HDRTRACE_HDRSMP_IGNORE_KEEP_ALIVE_RSP_MSG( )\
   hdrtrace_report0( TRACE_HDR_SMP_IGNORE_KEEP_ALIVE_RSP_MSG)

#define HDRTRACE_HDRSMP_SESSION_CLOSED( close_reason )\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_SMP_SESSION_CLOSED, \
     (uint8) close_reason, 1);

/**********
 * SMPKA *
 **********/

#define HDRTRACE_HDRSMPKA_SET_TIMER( )\
  hdrtrace_report0( TRACE_HDR_SMP_SET_T_KEEP_ALIVE)

#define HDRTRACE_HDRSMPKA_TIMER_DISABLED( )\
  hdrtrace_report0( TRACE_HDR_SMP_T_KEEP_ALIVE_DISABLED)

#define HDRTRACE_HDRSMPKA_SET_TIMER_FROM_NV( result )\
  hdrtrace_report1( TRACE_HDR_SMP_SET_T_KEEP_ALIVE_FROM_NV, \
    (uint32) result, 1)

#define HDRTRACE_HDRSMPKA_UPDATE_TIMER( result )\
  hdrtrace_report1( TRACE_HDR_SMP_UPDATE_T_KEEP_ALIVE, \
    (uint32) result, 1)

#define HDRTRACE_HDRSMPKA_CLR_TIMER( )\
  hdrtrace_report0( TRACE_HDR_SMP_CLR_T_KEEP_ALIVE )

#define HDRTRACE_HDRSMPKA_DEF_TIMER( )\
  hdrtrace_report0( TRACE_HDR_SMP_DEF_T_KEEP_ALIVE )



/**********
 * AMP *
 **********/
#define HDRTRACE_HDRAMP_ASSIGN_MSG_IGNORED_FRESH( state )\
 hdrtrace_report0((trace_id_enum_type)EVENT_HDR_AMP_ASSIGN_MSG_IGNORED_FRESH);\
 HDRTRACE_CURRENT_STATE(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, state)

#define HDRTRACE_HDRMC_UATI_ASSIGN_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMP_T_AT_RESPONSE); \
   HDRTRACE_CURRENT_STATE(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, state)

#define HDRTRACE_HDRMC_DUAL_ADDR_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_AMP_T_DUAL_ADDRESS); \
   HDRTRACE_CURRENT_STATE(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, state)

#define HDRTRACE_HDRAMP_RECEIVE_ATI_MATCH( protocol, result )\
   hdrtrace_report2( TRACE_HDR_AMP_RECEIVE_ATI_MATCH, \
     (uint32) protocol, 1, (uint32) result, 1)

#define HDRTRACE_HDRAMP_GET_TRANSMIT_ATI( protocol, result )\
   hdrtrace_report2( TRACE_HDR_AMP_GET_TRANSMIT_ATI, \
     (uint32) protocol, 1, (uint32) result, 1)

#define HDRTRACE_HDRAMP_GET_UATI( protocol, result )\
   hdrtrace_report2( TRACE_HDR_AMP_GET_UATI, \
     (uint32) protocol, 1, (uint32) result, 1)

#define HDRTRACE_HDRAMP_GET_SESSION_SEED( protocol )\
   hdrtrace_report1( TRACE_HDR_AMP_GET_SESSION_SEED, (uint32) protocol, 1)


/**********
 * SCP *
 **********/


#define HDRTRACE_HDRSCP_BEGIN_CONFIGURATION( )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_SCP_BEGIN_CONFIGURATION )

#define HDRTRACE_HDRMC_SCP_CONFIG_RSP_TIMER_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_SCP_T_CONFIG_RSP); \
   HDRTRACE_CURRENT_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL, state)

#define HDRTRACE_HDRMC_AN_INIT_STATE_TIMER_SIG( state )\
   hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_SCP_T_AN_INIT_STATE); \
   HDRTRACE_CURRENT_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL, state)

#define HDRTRACE_HDRSCP_PREVIOUS_SESSION_NEEDED( result )\
   hdrtrace_report1( TRACE_HDR_SCP_PREVIOUS_SESSION_NEEDED, \
     (uint32)result, 1)

#define HDRTRACE_HDRSCP_ATTEMPT_CONFIG_SUBTYPE( result )\
   hdrtrace_report1( TRACE_HDR_SCP_ATTEMPT_CONFIG_SUBTYPE, \
     (uint32)result, 1)

#define HDRTRACE_HDRSCP_GET_SUBTYPE( protocol, subtype)\
   hdrtrace_report2( TRACE_HDR_SCP_GET_SUBTYPE, \
     (uint32)protocol, 1, (uint32)subtype, 2)

#define HDRTRACE_HDRSCP_RESTORE_FROM_NV( protocol, result )\
   hdrtrace_report2( TRACE_HDR_SCP_RESTORE_FROM_NV, \
     (uint32)protocol, 1, (uint32)result, 1)

#define HDRTRACE_HDRSCP_SESSION_IS_OPEN( protocol, result )\
   hdrtrace_report2( TRACE_HDR_SCP_SESSION_IS_OPEN, \
     (uint32)protocol, 1, (uint32)result, 1)

#define HDRTRACE_HDRSCP_GET_TOKEN( protocol, state, token )\
 hdrtrace_report3( TRACE_HDR_SCP_GET_TOKEN, \
 (uint32) protocol, 1,\
 (uint32) HDRTRACE_CSTATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL, 0, state), 2,\
 (uint32) token, 2)

/***********
 * HYBRID *
 ***********/
#define HDRTRACE_HYBRID_POWER_SAVE( ) \
  hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_HYBRID_POWER_SAVE )

#define HDRTRACE_DEEP_SLEEP( ) \
  hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_DEEP_SLEEP )

#define HDRTRACE_RESELECTION( ) \
  hdrtrace_report0( (trace_id_enum_type) EVENT_HDR_RESELECTION )

/*******************
 * D2L reselection *
 *******************/
#define HDRTRACE_LTE_RESEL_STATUS( status ) \
   hdrtrace_report1( (trace_id_enum_type) TRACE_HDR_LTE_RESEL_STATUS, \
     (uint32) status, 1)

/***********
 *  BCMCS  *
 ***********/
#define HDRTRACE_BCMCS_FLOW_STATE( flow_id, from, to )\
   hdrtrace_report3( (trace_id_enum_type) EVENT_HDR_BCMCS_FLOW_STATE_CHANGE, \
              (uint32) flow_id, 4, \
              (uint32) from, 1, \
              (uint32) to, 1)

#define HDRTRACE_BCMCS_FLOW_STATE_ENTRY( flow_id, state )\
   hdrtrace_report2( TRACE_HDR_BCMCS_FLOW_STATE_ENTRY, \
              (uint32) flow_id, 4, \
              (uint32) state, 1)

#define HDRTRACE_OVHD_BCMCS_CHAN_CHANGE( channel, flow_id )\
   hdrtrace_report2( (trace_id_enum_type) EVENT_HDR_OVHD_BCMCS_CHAN_CHANGE, \
                     (uint32) channel, 2, \
                     (uint32) flow_id, 4)

/***********
 *  SCM  *
 ***********/
#define HDRTRACE_SCM_SESSION_CHANGED( change_reason )\
   hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_SCM_SESSION_CHANGED, \
     (uint32) change_reason, 1)
     
/***********
 *  CSNA   *
 ***********/
#define HDRTRACE_CSNA_DUP_3G1X_SRV_MSG( msg_seq )\
   hdrtrace_report1( (trace_id_enum_type) TRACE_HDR_CSNA_DUP_3G1X_SRV_MSG, \
     (uint32) msg_seq, 1)

/***********
 *  DOS    *
 ***********/
#define HDRTRACE_DOS_MO_DOS_STATUS( msg_seq, route, tx_status )\
   hdrtrace_report3( (trace_id_enum_type) EVENT_HDR_DOS_MO_DOS_STATUS, \
     (uint32) msg_seq, 1, \
     (uint32) route, 1, \
     (uint32) tx_status, 1)

/***********
 * MRLP    *
 ***********/
#define HDRTRACE_MRLP_EHRPD_PERSONALITY_IS_ACTIVE( ehrpd_is_active ) \
     hdrtrace_report1( (trace_id_enum_type) EVENT_HDR_MRLP_EHRPD_PERSONALITY_IS_ACTIVE, \
         (uint32)ehrpd_is_active, 1)

/***********
 * SAP    *
 ***********/
#define HDRTRACE_SAP_TUNNEL_IS_ENABLED( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_SAP_TUNNEL_IS_ENABLED, \
     (uint32) response, 1, \
     (uint32) HDRTRACE_CSTATE(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, 0, state), 2,\
     (uint32) caller, 1)

#define HDRTRACE_SAP_CONNECTION_IS_OPEN( caller, state, response )\
   hdrtrace_report3( TRACE_HDR_SAP_CONNECTION_IS_OPEN, \
   (uint32) response, 1, \
   (uint32) HDRTRACE_CSTATE(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, 0, state), 2,\
   (uint32) caller, 1)

/***********
 * LOG     *
 ***********/
#define HDRTRACE_PLOG_CONN_ATTEMPT_IS_ENDED( reason ) \
   hdrtrace_report1( (trace_id_enum_type) TRACE_HDR_CONN_ATTEMPT_ENDED, \
     (uint32) reason, 1 )

#define HDRTRACE_PLOG_CONN_IS_TERMINATED( reason ) \
   hdrtrace_report1( (trace_id_enum_type) TRACE_HDR_CONN_TERMINATED, \
     (uint32) reason, 1 )

#endif /* HDRTRACE_H */

