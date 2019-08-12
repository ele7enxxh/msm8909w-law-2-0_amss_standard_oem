/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S I G N A L I N G    A D A P T A T I O N   P R O T O C O L

GENERAL DESCRIPTION
  The Signaling Adaptation Protocol (SAP) is the main state machine
  controlling the session layer of the HDR protocol stack.

EXTERNALIZED FUNCTIONS (Global)


EXTERNALIZED FUNCTIONS (Regional)


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The Signaling Adaptation Protocol must be initialized before any other
  1xEV-DO Protocols which have session-related data.

Copyright (c) 2011 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsap.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/14   cnx     Process NV refresh cmd in tunnel INACTIVE state.
08/04/14   gga     Fixed Klocwork errors
07/29/14   tnp     FR22375: Fix Medium compiler warnings in HDR
07/09/14   cnx     Support NV refresh without reset.
10/11/13   vko     Fixed not sending Alt link close request due to race cond in hdrsaptx
11/11/13   cnx     Check IRAT subtypes before notifying preReg status to LTE.
01/18/14   vko     Fixed not sending pre-reg status when NV67252 is 1
01/28/14   vko     Fixed sending pre-reg status when AT not in tunnel mode
07/08/13   vko     Ignore L2 ACK Failure, when tunnel is not enabled
05/09/13   wsh     Fixed sending multiple UATI, not sending RTC msg in tunnel 
04/03/13   wsh     Fixed not releasing RF during ACT HO.
03/26/13   vko     Do not start SAP backoff timer in all cases of AC failure
02/04/13   cnx     Don't reset tunnel info if AT comes back to the same cell
                   after OOS. 
11/14/12   cnx     Clean SLP queue when UE leaves tunnel mode.
10/07/12   cnx     Clean up RASM before idle handoff to native HDR network.
09/11/12   cnx     Handle TX and RX messages during active handoff
08/31/12   cnx     Ignore AlternateLinkCloseConf received in SAP SETUP state.
07/16/12   cnx     Handle connection request when SAP subtype is 0.
07/16/12   cnx     Fix a bug when set last_cell_prereg_is_allowed flag.
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
04/20/12   cnx     Unsubcribe LTE_RRC_SERVICE_IND.
03/30/12   cnx     Close virtual connection when session is closed.
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/19/12   cnx     Fixed compiler errors.
03/01/12   cnx     Added hdrsap_stop_backoff_timer().
01/27/11   cnx     Support pre-Registration status update.
01/05/12   sju     Added verbose debug f3 support for SAP protocols. 
12/05/11   cnx     Support hdrcp_config_info.conf.
11/28/11   cnx     Support connection abort;
                   Support NV for L2 failure backoff timer and set preReg status.
11/08/11   cnx     Reset pending_alt_link_page_req when sending AlternateLinkOpenReq
11/08/11   cnx     Support Optimized Handoff phase 3.
10/28/11   pba     Optimized handoff phase 2 support
10/10/11   cnx     Fixed bugs in Optimized Handoff session maintainence.
10/03/11   cnx     Added HDRSAP_TUNNEL_MODE_NO_SRV.
09/14/11   cnx     Support retrieving CDMA system time from LTE via message
                   interface during LTE to HDR Optimized Handoff.
                   Moved SAP to CLOSE when AltLinkCloseRequest timeout.
04/11/11   cnx     Fixed compiler errors.
01/01/11   pba     Initial revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "target.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "amssassert.h"
#include "nv.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "qw.h"
#include "fs_lib.h"
#include "time_svc.h"
#include "msgr.h"
#include "lte_cphy_msg.h"
#include "hdrcp_msg.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrutil.h"
#include "hdrsap.h"
#include "hdrsaps.h"
#include "hdrsaptx.h"
#include "hdrsaprx.h"
#include "hdrtrace.h"
#include "hdrmci.h"
#include "hdralmp.h"
#include "hdrscmdb.h"
#include "hdrovhd.h"
#include "hdrovhdi.h"
#include "hdrovhds.h"
#include "hdrbit.h"
#include "hdramp.h"
#include "hdrsmp.h"
#include "hdrscp.h"
#include "hdrslptx.h"
#include "hdrpac_api.h"
#include "hdralmps.h"
#include "hdridles.h"
#include "hdrmc_v.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRSAP_ALT_LINK_OPEN_REQ_MSGID   0x00
#define HDRSAP_ALT_LINK_OPEN_CONF_MSGID  0x01
#define HDRSAP_ALT_LINK_OPEN_FAIL_MSGID  0x02
#define HDRSAP_ALT_LINK_CLOSE_REQ_MSGID  0x03
#define HDRSAP_ALT_LINK_CLOSE_CONF_MSGID 0x04
#define HDRSAP_ALT_LINK_PAGE_REQ_MSGID   0x05
#define HDRSAP_ALT_LINK_OPEN_COMP_MSGID  0x06

#define HDRSAP_ALT_LINK_OPEN_CONF_TIMER_MSECS   5000  
#define HDRSAP_ALT_LINK_CLOSE_CONF_TIMER_MSECS  5000  

/* Pack AlternateLinkOpenReq */
#define HDRSAP_PACK_ALT_LINK_OPEN_REQ_MSG_ID( msg_ptr, msg_id ) \
          HDRBIT_PACK8(msg_ptr, msg_id, 0, 8)

#define HDRSAP_PACK_ALT_LINK_OPEN_REQ_TRANS_ID( msg_ptr, trans_id ) \
          HDRBIT_PACK8(msg_ptr, trans_id, 8, 8)

#define HDRSAP_PACK_ALT_LINK_OPEN_REQ_REASON( msg_ptr, req_reason ) \
          HDRBIT_PACK8(msg_ptr, req_reason, 16, 4)

/* Pack AlternateLinkCloseReq */
#define HDRSAP_PACK_ALT_LINK_CLOSE_REQ_MSG_ID( msg_ptr, msg_id ) \
          HDRBIT_PACK8(msg_ptr, msg_id, 0, 8)

#define HDRSAP_PACK_ALT_LINK_CLOSE_REQ_TRANS_ID( msg_ptr, trans_id ) \
          HDRBIT_PACK8(msg_ptr, trans_id, 8, 8)

/* Pack AlternateLinkCloseConf */
#define HDRSAP_PACK_ALT_LINK_CLOSE_CONF_MSG_ID( msg_ptr, msg_id ) \
          HDRBIT_PACK8(msg_ptr, msg_id, 0, 8)

#define HDRSAP_PACK_ALT_LINK_CLOSE_CONF_TRANS_ID( msg_ptr, trans_id ) \
          HDRBIT_PACK8(msg_ptr, trans_id, 8, 8)

/* Pack AlternateLinkOpenComp */
#define HDRSAP_PACK_ALT_LINK_OPEN_COMP_MSG_ID( msg_ptr, msg_id ) \
          HDRBIT_PACK8(msg_ptr, msg_id, 0, 8)

#define HDRSAP_PACK_ALT_LINK_OPEN_COMP_TRANS_ID( msg_ptr, trans_id ) \
          HDRBIT_PACK8(msg_ptr, trans_id, 8, 8)


#define HDRSAP_ALT_LINK_OPEN_REQ_PRIORITY  40
#define HDRSAP_ALT_LINK_CLOSE_REQ_PRIORITY  40
#define HDRSAP_ALT_LINK_CLOSE_CONF_PRIORITY  40

#define HDRSAP_MSGR_PRIORITY                 1
#define HDRSAP_MSGR_MAX_NUM_MSG              5

/* HDRSAP task */
#define HDRSAP_TASK_PTR hdrmc_tcb_ptr

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define HDRSAP_UNPACK_MSG_ID( msg_ptr, msg_id) \
          (HDRBIT_UNPACK8((msg_ptr), (msg_id), 0, 8))

#define HDRSAP_UNPACK_TRANS_ID( msg_ptr,trans_id) \
          (HDRBIT_UNPACK8((msg_ptr), (trans_id), 8, 8))

#define HDRSAP_BACKOFF_TIMER_CNT_MAX_AC 10

/* Second to ms unit conversion */
#define HDRSAP_SECOND_TO_MS_CONVERSION 1000

/* tx failure backoff timer (in sec) */
static uint16 backoff_time_table_l2_failed[HDRSAP_BACKOFF_TIMER_CNT_MAX_AC] = 
{ 1, 5, 5, 5, 15, 60, 120, 120, 120, 120};

/* Alt link open request message */
typedef struct
{
  uint8    msg_id;
  uint8    trans_id;
  uint8    req_reason;
  uint8    internal_reason; 
  boolean  awaiting_slp_callback;
  boolean  awaiting_tx_failed_ac_ind;

} hdrsap_alt_link_open_req_msg_type;

/* Alt link close request message */
typedef struct
{
  uint8    msg_id;
  uint8    trans_id;

} hdrsap_alt_link_close_req_msg_type;

/* Alt link open complete message */
typedef struct
{
  uint8    trans_id;
  boolean  awaiting_slp_callback;

} hdrsap_alt_link_open_comp_msg_type;

/* Alt link page request message */
typedef struct
{
  boolean  pending_alt_link_page_req;

} hdrsap_alt_link_page_req_msg_type;

typedef struct
{
  hdrsap_alt_link_open_req_msg_type alt_link_open_req;
    /* Struct holding ALT link open req */

  hdrsap_alt_link_close_req_msg_type alt_link_close_req;
    /* Struct holding ALT link close req */

  hdrsap_alt_link_open_comp_msg_type alt_link_open_comp;
    /* Struct holding ALT link open Comp */

  hdrsap_alt_link_page_req_msg_type alt_link_page_req;
    /* Struct holding ALT link page req */

  rex_timer_type state_timer;
    /* SAP state timer */

  rex_timer_type backoff_timer;
    /* TX backoff timer */

  uint8 tx_failure_cnt_ac;
    /* Number of failed tx attempts */

  msgr_client_t msgr_client_id;
    /* Messenger client ID */

  msgr_id_t msg_queue_id;
    /* Message Queue ID */

  hdrhai_tunnel_mode_type tunnel_mode;
    /* Current tunnel mode value */

  lte_rrc_global_cell_id_t cell_id_l2_failed; 
    /* cell id last L2 tx failed on */

  dsm_item_type *rup_msg_item_ptr;       
    /* dsm item pointer to RU message */

  uint8 verbose_debug;

  hdrhai_prereg_status_enum_type set_prereg_status;
    /* At what point UE considers itself to be pre-Registered */

  boolean curr_prereg_status;
    /* Current pre-Registration status */

  boolean abort_connection;
    /* Abort current connection attempt if this flag is set */
 
  hdrsap_ac_msg_enum_type  ac_msg_type;
    /* ac msg type for which L2 ACK has failed */
	
  boolean is_prereg_status_send_pending;
  /* Send pre-reg status if this flag is set */ 
} hdrsap_data_type;

static hdrsap_data_type hdrsap;

LOCAL void hdrsap_send_alt_link_close_req_msg ( void );
LOCAL void hdrsap_process_alt_link_open_timeout( void );

/* <EJECT> */ 
/*===========================================================================

                   MESSAGE PACK / UNPACK MACROS

===========================================================================*/

/****************************************************************************
** All Pack routines take a pointer to a dsm_item and
** a variable of the type belonging to the appropriate message
****************************************************************************/


/***************************************************************************
** Unpack routines.  All take a pointer to a DSM item and a reference to
** a variable of the appropriate type
***************************************************************************/

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRSAP_TIMER_CB

DESCRIPTION
  This function is the callback for all SAP timers. This function is called 
  by REX when the timer expires, and runs in the context of the task that 
  defined the timer (HDRMC), however it runs as an independent thread and
  thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrsap_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, timer_id);
} /* hdridle_timer_cb() */


/*===========================================================================
FUNCTION HDRSAP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to SAP.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrsap_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
)
{
  if ( rex_self() == HDRSAP_TASK_PTR )
  {
    hdrsap_process_ind( ind_name, (hdrind_ind_data_union_type*)ind_data_ptr );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, ind_name,
                     ind_data_ptr );
  }
} /* hdrsap_ind_cb() */


/*=============================================================================
FUNCTION HDRSAP_POWERUP_INIT

DESCRIPTION
  This function commands SAP to Initialize.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_powerup_init ( void )
{
  if ( hdrmc_is_first_init() )
  {
    hdrsap_set_tunnel_mode ( HDRHAI_TUNNEL_MODE_DISABLED );
  }
  hdrsap.tx_failure_cnt_ac = 0;
  hdrsap.abort_connection = FALSE;
  hdrsap.curr_prereg_status = FALSE;
  hdrsap.is_prereg_status_send_pending = FALSE;
  
  if ( hdrmc_is_first_init() )
  {
    rex_def_timer_ex( &hdrsap.state_timer,
                      hdrsap_timer_cb,
                      HDRSAP_STATE_TIMER );

    rex_def_timer_ex( &hdrsap.backoff_timer,
                      hdrsap_timer_cb,
                      HDRSAP_BACKOFF_TIMER );
  }

  /* Read L2 failure backoff time table from EFS NV. */
  if ( (int)efs_get( HDRSAP_BACKOFF_TIME_TABLE_L2_FAILED,
                     &backoff_time_table_l2_failed[0],
                     ( sizeof(uint16)*HDRSAP_BACKOFF_TIMER_CNT_MAX_AC ) )
         == (int)( sizeof(uint16)*HDRSAP_BACKOFF_TIMER_CNT_MAX_AC ) )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Read L2 failure backoff time table successfully" );
  }
  else 
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Read L2 failure backoff time table failed. Use default" );
  }

  /* Read NV which controls the time that HDR notifies LTE about the
   * prereg status. */
  hdrsap.set_prereg_status = HDRHAI_PREREG_STATUS_DEFAULT;

  if ( (int)efs_get( HDRSAP_SET_PREREG_STATUS, &hdrsap.set_prereg_status, 1 ) == 1 )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Read notify preReg status NV successfully" );
    if ( hdrsap.set_prereg_status >= HDRHAI_PREREG_STATUS_MAX )
    {
      hdrsap.set_prereg_status = HDRHAI_PREREG_STATUS_DEFAULT;
    }
  }
  else 
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Read notify preReg status NV failed. Use default" );
  }

} /* hdrsap_powerup_init */

/*===========================================================================
FUNCTION HDRSAP_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the verbose debug setting .
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsap_set_verbose_debug
(
  uint8 verbose_debug_setting
)
{
  hdrsap.verbose_debug = verbose_debug_setting; 
} /* hdrsap_set_verbose_debug() */

/*===========================================================================
FUNCTION HDRSAP_MSGR_START

DESCRIPTION
  Initialize message interface.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_msgr_start( void )
{
  errno_enum_type estatus = E_FAILURE;

  msgr_umid_type umid_list[] =
  {
    LTE_CPHY_CDMA_SYSTIME_CNF,
    LTE_CPHY_HRPD_MEAS_RESULTS_CNF
  };

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  estatus = msgr_client_create( &hdrsap.msgr_client_id );
  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_client_add_mq( "HDR Signaling Adapation Protocol Messenger",
                                &hdrsap.msgr_client_id,
                                (uint8)HDRSAP_MSGR_PRIORITY,
                                (uint32)HDRSAP_MSGR_MAX_NUM_MSG,
                                (uint32)sizeof(lte_cphy_hrpd_meas_results_cnf_s),       
                                &hdrsap.msg_queue_id );
  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_register_block( MSGR_HDR_CP,
                                 &hdrsap.msgr_client_id,
                                 hdrsap.msg_queue_id,
                                 umid_list,
                                 (uint32)sizeof(umid_list)/sizeof(msgr_umid_type) );
  ASSERT( estatus == E_SUCCESS );

} /* hdrsap_msgr_start() */


/*===========================================================================
FUNCTION HDRSAP_MSGR_STOP

DESCRIPTION
  Clean up Signaling Adaptation Protocol message interface by doing the following. 
  - De-register the registered messages. 
  - Delete the messenger Client ID for Decoder module. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_msgr_stop( void )
{
  errno_enum_type  estatus;

  msgr_umid_type umid_list[] =
  {
    LTE_CPHY_CDMA_SYSTIME_CNF,
    LTE_CPHY_HRPD_MEAS_RESULTS_CNF
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  estatus = msgr_deregister_block( MSGR_HDR_CP, 
                                   &hdrsap.msgr_client_id,
                                   umid_list,
                                   (uint32)sizeof(umid_list)/sizeof(msgr_umid_type) );
  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_client_delete( &hdrsap.msgr_client_id );
  ASSERT ( estatus == E_SUCCESS );

} /* hdrsap_msgr_stop() */


/*===========================================================================
FUNCTION HDRSAP_GET_80MS_TIME

DESCRIPTION
  This function gets CDMA system time in 80ms unit.
 
DEPENDENCIES
  hdrsap_msgr_start() has been called.

PARAMETERS
  sys_time - system time retrieved

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_get_80ms_time
( 
  qword sys_time
)
{
  lte_cphy_cdma_systime_req_s sys_time_req;
  lte_cphy_cdma_systime_cnf_s sys_time_conf;
  uint32                      bytes_recvd;
  errno_enum_type             estatus = E_FAILURE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Since LTE M1 will immediately return system time after receiving
   * system time request, we call msgr_receive directly without using
   * any state machine. 
   */
  msgr_init_hdr( &sys_time_req.msgr_hdr, 
                 MSGR_HDR_CP,
                 LTE_CPHY_CDMA_SYSTIME_REQ );

  estatus = msgr_send( &sys_time_req.msgr_hdr, 
                       sizeof( lte_cphy_cdma_systime_req_s ) );
  ASSERT( estatus == E_SUCCESS );

  msgr_receive( &hdrsap.msgr_client_id,
                (uint8 *)&sys_time_conf,
                sizeof( lte_cphy_cdma_systime_cnf_s ),
                &bytes_recvd );

  if ( sys_time_conf.valid )
  {
    /* Time retrieved from LTE is unit of chips. Convert to 80ms unit. */
    qw_set( sys_time, 
            ((sys_time_conf.cdma_systime >> 15) / 3) >> 32,
            ((sys_time_conf.cdma_systime >> 15) / 3) & 0xFFFFFFFF );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "LTE sys time is invalid, Got sys time from TS...");
    (void) time_get( sys_time );
    qw_shift( sys_time, -22 );  
  }

  HDR_MSG_PROT_2( MSG_LEGACY_MED,
                  "HDRSAP: CDMA system time, HIGH 0x%xms, LOW 0x%xms",
                  qw_hi(sys_time), qw_lo(sys_time) );

} /* hdrsap_get_80ms_time() */


/* <EJECT> */ 

/*===========================================================================
FUNCTION HDRSAP_GET_PILOTS

DESCRIPTION
  This function gets HRPD pilot measurement results from LTE ML1
 
DEPENDENCIES
  hdrsap_msgr_start() has been called.

PARAMETERS
  max_pilots - Less than or equal to size of array pointed to by pilots_ptr
  pilots_ptr - Pointer to array of structures comprising all the active and 
               candidate set pilots.  The first pilot is the reference pilot.
  caller - Protocol calling the function, for debug


RETURN VALUE
  Number of pilots returned.

SIDE EFFECTS
  None
===========================================================================*/
int hdrsap_get_pilots
( 
  int max_pilots,
  hdrsrch_pilot_rpt_type * pilots_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  uint8 i = 0;
  lte_cphy_hrpd_meas_results_req_s hrpd_meas_results_req;
  lte_cphy_hrpd_meas_results_cnf_s hrpd_meas_results_cnf;
  uint32 bytes_recvd = 0;
  errno_enum_type estatus = E_FAILURE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Since LTE M1 will immediately return pilots after receiving
   * measurement request, we call msgr_receive directly without using
   * any state machine. 
   */
  msgr_init_hdr( &hrpd_meas_results_req.msgr_hdr, 
                 MSGR_HDR_CP,
                 LTE_CPHY_HRPD_MEAS_RESULTS_REQ );

  estatus = msgr_send( &hrpd_meas_results_req.msgr_hdr, 
                       sizeof( lte_cphy_hrpd_meas_results_req_s ) );
  ASSERT( estatus == E_SUCCESS );

  msgr_receive( &hdrsap.msgr_client_id,
                (uint8 *)&hrpd_meas_results_cnf,
                sizeof( lte_cphy_hrpd_meas_results_cnf_s ),
                &bytes_recvd );

  if ( hrpd_meas_results_cnf.valid )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "LTE returned (%d) HRPD pilots in meas results",
                    hrpd_meas_results_cnf.num_pilots);

    for ( i = 0; ((i < hrpd_meas_results_cnf.num_pilots) && 
                  (i < max_pilots)); i++ )
    {
      pilots_ptr[i].link.channel = hrpd_meas_results_cnf.pilot[i].chan;
      pilots_ptr[i].system_type = HDRSRCH_SYSTYPE_DO;
      pilots_ptr[i].link.pilot_pn = hrpd_meas_results_cnf.pilot[i].pilot_pn;
      pilots_ptr[i].pn_phase = hrpd_meas_results_cnf.pilot[i].pilot_phase;
      pilots_ptr[i].strength = hrpd_meas_results_cnf.pilot[i].pilot_strength;
      pilots_ptr[i].keep = TRUE;
    }

    if (hrpd_meas_results_cnf.num_pilots > max_pilots)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "LTE HRPD meas results (%d) pilots, whereas allowed =(%d)",
                      hrpd_meas_results_cnf.num_pilots, max_pilots);
    }
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "LTE HRPD meas results returned no pilots");
    hrpd_meas_results_cnf.num_pilots = 0;
  }

  return hrpd_meas_results_cnf.num_pilots;
} /* hdrsap_get_pilots() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_ACTIVATE

DESCRIPTION
  This function commands SAP to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_activate
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrsap_cmd_type hdrsap_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRSAP_ACTIVATE_CMD received %d",
                  hdrsaps_get_state());

  /* Fill in all members of the hdrsap_cmd_type for the Activate command */
  hdrsap_cmd.name = HDRSAP_ACTIVATE_CMD;
  hdrsap_cmd.sender = sender;

  if ( rex_self() != HDRSAP_TASK_PTR )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Activate Command from outside HDRMC task!" );
    return;
  }

  hdrmc_queue_cmd( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, &hdrsap_cmd,
                   sizeof (hdrsap_cmd) );
} /* hdrsap_activate */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRSAP_DEACTIVATE

DESCRIPTION
  This function commands HDRSAP to deactivate

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  reason - Deactivate reason

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_deactivate
(
  hdrhai_protocol_name_enum_type sender,
  hdrsap_deact_reason_enum_type reason
)
{
  hdrsap_cmd_type hdrsap_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_3( MSG_LEGACY_MED, 
                  "HDRSAP_DEACTIVATE_CMD rxed from %d (%d), reason %d",
                  sender, hdrsaps_get_state(), reason );

  hdrsap_cmd.name = HDRSAP_DEACTIVATE_CMD;
  hdrsap_cmd.sender = sender;
  hdrsap_cmd.params.deactivate.reason = reason;

  if ( rex_self() == HDRSAP_TASK_PTR )
  {
    hdrsap_process_cmd( &hdrsap_cmd );
  }
  else
  {
    hdrmc_queue_cmd( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, 
                     &hdrsap_cmd, sizeof (hdrsap_cmd) );
  }

} /* hdrsap_deactivate() */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_PROCESS_ACTIVATE

DESCRIPTION
  This function processes SAP activate.

DEPENDENCIES
  None

PARAMETERS
  sap_cmd_ptr - Pointer to the command for SAP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_process_activate
(
  hdrsap_cmd_type * smp_cmd_ptr
)
{
  hdrhai_tunnel_mode_type prev_tunnel_mode;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  prev_tunnel_mode = hdrsap.tunnel_mode;

  /* Enable tunnel mode - now LTE is the active RAT */
  hdrsap_set_tunnel_mode ( HDRHAI_TUNNEL_MODE_ENABLED );

  /* Case 1: There is no session: 
   *         Need to run "session close" procedure to re-initialize SCP with 
   *         IRAT subtyps
   *
   * Case 2: There is session but personality is not IRAT:
   *         After SMP receives HDRIND_OVHD_PREREG_INFO_UPDATED and if 
   *         prereg_is_allowed = 1, need to close session.
   *
   * Case 3: There is session and personality is IRAT:
   *         After AMP receives HDRIND_OVHD_PREREG_INFO_UPDATED and if 
   *         prereg_is_allowed = 1, Restore session if subnet is different 
   *    (TODO: with current implementation, we also restore session if subnet is same,
   *          Check udpated spec. later since this may need to be modified.)
   * We only handle Case 1 in SAP.
   */
  if ( !hdrsmp_session_is_open( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) ) 
  {
    if (hdrsap.verbose_debug)
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "Session is not opened");
    }
    /* Once session is closed re-initialize the SCP with IRAT subtypes */
    hdrscmdb_process_session_closed( HDRHAI_CLOSE_RESET );
  }
  else if ( hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
              == HDRSCP_IRAT_SAP )
  {
    /* Entering tunnel mode the very first time, it's needed to do session maintenance
     * if IRAT subtypes are negotiated */ 
    hdramp_set_entering_tunnel( TRUE );
  }

  if ( prev_tunnel_mode != HDRHAI_TUNNEL_MODE_NO_SRV )
  {
    if (hdrsap.verbose_debug)
    {
      HDR_MSG_PROT_1( MSG_LEGACY_MED, "Previous Tunnel Mode %d",
                      prev_tunnel_mode);
    }
    /* activate almp only if previous tunnel mode is not No Service
     * because we did not deactivate hdralmp when entering no service mode
     */
    hdralmp_activate(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL);
  }
  hdrovhd_activate(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL);

} /* hdrsap_process_activate() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSAP_PROCESS_CLOSE_VIRTUAL_CONNECTION

DESCRIPTION
  This function closes virtual connection if it is open.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
LOCAL void hdrsap_process_close_virtual_connection( void ) 
{
  hdrind_give_ind( HDRIND_SAP_CONNECTION_CLOSED, NULL );
  hdrsaps_change_state( HDRSAPS_CLOSE );

} /* hdrsap_process_close_virtual_connection() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSAP_PROCESS_FAIL_CONNECTION

DESCRIPTION
  This function processes the connection setup failure.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
LOCAL void hdrsap_process_fail_connection( void ) 
{
  hdrind_idle_conn_fail_info_type conn_fail_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_L2_FAILED;
  conn_fail_info.backoff_time = 0;
  hdrsaptx_clr_info_tx(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL);
  hdrind_give_ind( HDRIND_SAP_CONNECTION_FAILED, (void*)&conn_fail_info );
  hdrsaps_change_state( HDRSAPS_CLOSE );  
  hdrsap.alt_link_open_req.awaiting_slp_callback = FALSE;

} /* hdrsap_process_fail_connection() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRSAP_PROCESS_DEACTIVATE

DESCRIPTION
  This function processes SAP deactivate command.

DEPENDENCIES
  None

PARAMETERS
  sap_cmd_ptr - Pointer to the command for SAP to process

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_process_deactivate
(
  hdrsap_cmd_type *smp_cmd_ptr
)
{ 
  /* Ignore the command if tunnel mode is disableed */ 
  if( hdrsap.tunnel_mode == HDRHAI_TUNNEL_MODE_DISABLED )
  {
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
      smp_cmd_ptr->name, smp_cmd_ptr->sender, 
      hdrsaps_get_state() );
    return;
  }

  /* Clear all the timers */  
  ( void ) rex_clr_timer( &hdrsap.state_timer );      
  ( void ) rex_clr_timer( &hdrsap.backoff_timer );

  hdrsap.tx_failure_cnt_ac = 0;

  /* Close virtual connection */
  if ( hdrsaps_get_state() == HDRSAPS_OPEN )
  {
    hdrsap_process_close_virtual_connection();
  }
  else if ( hdrsaps_get_state() == HDRSAPS_SETUP )
  {
    hdrsap_process_fail_connection();
  }
  else
  {
    hdrsaptx_stop_tx( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );
  }

  /* Don't deactivate tunnel mode if we will enter tunnel mode again */
  if ( smp_cmd_ptr->params.deactivate.reason != HDRSAP_DEACT_NO_SRV )
  {
    hdrovhd_deactivate( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );
    hdralmp_deactivate( HDRHAI_ABORT_EXIT_TUNNEL, FALSE, 0, 
                        HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );

    /* Tunnel mode is disabled after ALMP is deactivated */
    hdrsap_set_tunnel_mode( HDRHAI_TUNNEL_MODE_DISABLED );
  }
  else
  {
    hdrsap_set_tunnel_mode( HDRHAI_TUNNEL_MODE_NO_SRV );

    /* In ACT HO, IDLE maybe activated. Need to deactivate */
    if ( ( hdrsap.tunnel_mode == HDRHAI_TUNNEL_MODE_ACTIVE_HO ) &&
         ( hdralmps_get_state() != HDRALMPS_IDLE ) &&
         ( hdridles_get_state() != HDRIDLES_INACTIVE_STATE ) )
    {
      hdridle_deactivate(HDRHAI_ABORT_EXIT_TUNNEL, 
                         HDRHAI_SIGNALING_ADAPTATION_PROTOCOL);
    }

    /* Need to reset tunnel info if LTE cell is changed after OOS return */
    hdrovhd_set_waiting_prereg_after_oos(TRUE);
    hdrovhd_deactivate( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );
  }

} /* hdrsap_process_deactivate() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_IS_ALT_LINK_OPEN_REQ_TRANS_ID_MATCHED

DESCRIPTION
  This function processes the transmission outcome of the alt link open request
  message.

DEPENDENCIES
  None

PARAMETERS
  data    - user data which contains trans ID

RETURN VALUE
  Ture - transaction ID matched for the alt link open request message

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrsap_is_alt_link_open_req_trans_id_matched
(
  void  *data
)
{
  if( hdrsap.alt_link_open_req.trans_id ==  (byte)((dword) data) )
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }

} /* hdrsap_is_alt_link_open_req_trans_id_matched */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_STOP_ALT_LINK_CLOSE_CONF_TIMER

DESCRIPTION
  This function stops the Alternate link close conf timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_stop_alt_link_close_conf_timer ( void )
{
  if( rex_get_timer( &hdrsap.state_timer) != 0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_LOW, "Stop AlternateLinkCloseConf timer" );
    ( void ) rex_clr_timer( &hdrsap.state_timer );
  }

} /* hdrsap_stop_alt_link_close_conf_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_START_ALT_LINK_CLOSE_CONF_TIMER

DESCRIPTION
  This function starts the AlternateLinkCloseConf timer

DEPENDENCIES
  None

PARAMETERS
  cnt - timer count in millisecond

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_start_alt_link_close_conf_timer
(
  rex_timer_cnt_type cnt
)
{
  HDR_MSG_PROT( MSG_LEGACY_LOW, "Start AlternateLinkCloseConf timer" );
  ( void ) rex_set_timer( &hdrsap.state_timer, cnt);

} /* hdrsap_start_alt_link_close_conf_timer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_IS_ALT_LINK_OPEN_COMP_TRANS_ID_MATCHED

DESCRIPTION
  This function processes the transmission outcome of the alt link open complete
  message.

DEPENDENCIES
  None

PARAMETERS
  data    - user data which contains trans ID

RETURN VALUE
  Ture - transaction ID matched for the alt link open complete message

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrsap_is_alt_link_open_comp_trans_id_matched
(
  void  *data
)
{
  if( hdrsap.alt_link_open_comp.trans_id ==  (byte)((dword) data) )
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }

} /* hdrsap_is_alt_link_open_comp_trans_id_matched */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_STOP_ALT_LINK_OPEN_CONF_TIMER

DESCRIPTION
  This function stops the connection setup timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_stop_alt_link_open_conf_timer ( void )
{
  if( rex_get_timer( &hdrsap.state_timer) !=0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_LOW, "Stop AlternateLinkOpenConf timer" );
    ( void ) rex_clr_timer( &hdrsap.state_timer);
  }
} /* hdrsap_stop_conn_setup_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_START_ALT_LINK_OPEN_CONF_TIMER

DESCRIPTION
  This function starts the AlternateLinkOpenConf timer

DEPENDENCIES
  None

PARAMETERS
  cnt - timer count in millisecond

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_start_alt_link_open_conf_timer
(
  rex_timer_cnt_type cnt
)
{
  HDR_MSG_PROT( MSG_LEGACY_LOW, "Start AlternateLinkOpenConf timer" );
  ( void ) rex_set_timer( &hdrsap.state_timer, cnt);
} /* hdrsap_start_alt_link_open_conf_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_ALT_LINK_OPEN_REQ_MSG_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  message.

DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_alt_link_open_req_msg_tx_outcome_cb
(
  void                 *data,
  hdrerrno_enum_type    status
)
{
  hdrsaps_state_enum_type cur_state = hdrsaps_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !hdrsap.alt_link_open_req.awaiting_slp_callback )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Ignore creq tx outcome cb s:%d, d:%d",
                    status, (uint32)data );
    return;
  }

  if( !hdrsap_is_alt_link_open_req_trans_id_matched(data) )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                    "creq tx outcome cb trans_id mismatch s:%d, d:%d", 
                    status, (uint32)data );
    return;
  }

  hdrsap.alt_link_open_req.awaiting_slp_callback = FALSE;

  if (hdrsap.verbose_debug)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED, "creq tx outcome cb s:%d, d:%d", 
                   status, (uint32)data);
  }

  switch(status)
  {
    case E_SUCCESS:
      /* message transmitted successfully */
      HDRTRACE_TXMSG_ACKED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                            HDRSAP_ALT_LINK_OPEN_REQ_MSGID, cur_state );

      if( cur_state == HDRSAPS_SETUP )
      {
        hdrsap_start_alt_link_open_conf_timer( 
          HDRSAP_ALT_LINK_OPEN_CONF_TIMER_MSECS );
      }
      else
      {
        /* ignore the callback if it is not received in the right state */     
        HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                        "Ignore creq tx outcome cb (wrong state: %d)", 
                        cur_state );
      }
      break;

    case E_ABORTED:
      /* E_ABORTED will never happen in the tunnel mode */
      HDRTRACE_TXMSG_ABORTED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                              HDRSAP_ALT_LINK_OPEN_REQ_MSGID, cur_state );
      break;

    case E_CANCELED:
      /* AlternateLinkOpenConf may come before L2Ack,
       * SLP returns E_CANCELED in this case */
      HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                      "Received ALternateLinkOpenConf before L2Ack: %d", 
                      cur_state );
      break;

    case E_FAILURE:
      if (rex_self() != HDRSAP_TASK_PTR) //L2 ACK failure, not HMP failure
      {
         hdrsap_set_ac_msg_type(HDRSAP_LINK_OPEN_REQ); 
         HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                        "L2 ACK failure : ac msg type set to %d", 
                         HDRSAP_LINK_OPEN_REQ);
       
      }
      HDRTRACE_TXMSG_FAILED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                              HDRSAP_ALT_LINK_OPEN_REQ_MSGID, cur_state );
							  
      if ( cur_state != HDRSAPS_SETUP )
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                        "Received AltLinkOpenReq L2Ack in wrong state: %d", 
                        cur_state );
      }
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Unknown tx outcome cb result: %d", 
                      status );
      break;
  }
} /* hdrsap_alt_link_open_req_msg_tx_outcome_cb() */


/*===========================================================================
FUNCTION HDRSAP_SEND_ALT_LINK_OPEN_REQ_MSG

DESCRIPTION
  This function sends the AlternateLinkOpenReq to the access network.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_send_alt_link_open_req_msg( void )
{
  dsm_item_type  *dsm_ptr; /* MSG buffer pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrsap.alt_link_page_req.pending_alt_link_page_req = FALSE;

  /* Get DSM item buffer */
  dsm_ptr = hdrutil_new_msg_buffer();

  /* Populate AltLinkOpenReq structure */
  hdrsap.alt_link_open_req.msg_id = 
      HDRSAP_ALT_LINK_OPEN_REQ_MSGID;
  hdrsap.alt_link_open_req.trans_id++;

  /* Set the open reason */
  if ( hdrsap.alt_link_open_req.internal_reason & HDRSAP_CR_AN )
  {
    hdrsap.alt_link_open_req.req_reason = 
      HDRSAP_ALT_LINK_OPEN_REQ_REASON_AN_INIT;
  }
  else
  {
    hdrsap.alt_link_open_req.req_reason = 
      HDRSAP_ALT_LINK_OPEN_REQ_REASON_AT_INIT;
  }
  hdrsap.alt_link_open_req.awaiting_slp_callback = TRUE;

  /* Pack AlternateLinkOpenReq */
  HDRSAP_PACK_ALT_LINK_OPEN_REQ_MSG_ID( dsm_ptr, 
                                        hdrsap.alt_link_open_req.msg_id );
  HDRSAP_PACK_ALT_LINK_OPEN_REQ_TRANS_ID( dsm_ptr, 
                                          hdrsap.alt_link_open_req.trans_id );
  HDRSAP_PACK_ALT_LINK_OPEN_REQ_REASON( dsm_ptr, 
                                        hdrsap.alt_link_open_req.req_reason );

  /* Change state to setup */
  hdrsaps_change_state( HDRSAPS_SETUP );

  /* Send the message */
  hdrhmp_send_msg
  (
    HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
    HDRHAI_AC,
    FALSE,
    HDRSAP_ALT_LINK_OPEN_REQ_PRIORITY,
    HDRHAI_IN_USE, /* Instance of protocol sending message */
    dsm_ptr,
    hdrsap_alt_link_open_req_msg_tx_outcome_cb,
    (void *) ((dword) hdrsap.alt_link_open_req.trans_id)
  );

} /* hdrsap_send_alt_link_open_req_msg() */


/* <EJECT> */
/*===========================================================================
FUNCTION HDRSAP_ALT_LINK_CLOSE_REQ_MSG_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  message.

DEPENDENCIES
  None

PARAMETERS
  data   - user data
  status - message transmission status

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdrsap_alt_link_close_req_msg_tx_outcome_cb
(
  void                 *data,
  hdrerrno_enum_type    status
)
{
  hdrsaps_state_enum_type cur_state = hdrsaps_get_state();
  static uint8 close_req_msg_na_cnt = 0; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( status )
  {
    case E_SUCCESS:
      HDRTRACE_TXMSG_ACKED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                            HDRSAP_ALT_LINK_CLOSE_REQ_MSGID, cur_state );

      if( cur_state == HDRSAPS_OPEN )
      {
        hdrsap_start_alt_link_close_conf_timer( 
          HDRSAP_ALT_LINK_CLOSE_CONF_TIMER_MSECS );
      }
      else
      {
        /* ignore the callback if it is not received in the right state */     
        HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                        "Ignore creq tx outcome cb (wrong state: %d)", 
                        cur_state );
      }
      break;

    case E_ABORTED:
      HDRTRACE_TXMSG_ABORTED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                              HDRSAP_ALT_LINK_CLOSE_REQ_MSGID, cur_state );

      /* Close virtual connection scilently. */
      if ( cur_state == HDRSAPS_OPEN )
      {
        hdrsap_process_close_virtual_connection();

        /* If AlternateLinkPageReq is received when SAP is OPEN, close the
         * current virtual connection first and send AlternateLinkOpenRequest
         * in SAP CLOSED state. */
        if ( hdrsap.alt_link_page_req.pending_alt_link_page_req )
        {
          /* The pending_alt_link_page_req will be reset when receiving the
           * AlternateLinkOpenConf. */
          hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_AN;
          hdrsap_send_alt_link_open_req_msg();
        }
      }
      break;
 

    case E_NOT_ALLOWED:	
	  close_req_msg_na_cnt++;
	  HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                        "Alt Link Close request outcome cb. E_NOT_ALLOWED %d times", 
                        close_req_msg_na_cnt );
	  /* This is due to race condition where, alt link close req is queued, 
             but RTC flush happens */
      HDRTRACE_TXMSG_FAILED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                             HDRSAP_ALT_LINK_CLOSE_REQ_MSGID, cur_state );
							 
	  if(close_req_msg_na_cnt == 1 && cur_state == HDRSAPS_OPEN)
	  {
	     /* Try queuing alt link close request one more time */
	     hdrsap_send_alt_link_close_req_msg(); /* This time it should be allowed, 
                                                     as RTC flush already happened */
	  }
	  else
	  {
	    close_req_msg_na_cnt = 0; /* If not allowed more than once,then some problem.
                                         Do same action as E_FAILURE */
	    /* Close virtual connection scilently. */
        if ( cur_state == HDRSAPS_OPEN )
        {
          hdrsap_process_close_virtual_connection();

          /* If AlternateLinkPageReq is received when SAP is OPEN, close the
           * current virtual connection first and send AlternateLinkOpenRequest
           * in SAP CLOSED state. */
          if ( hdrsap.alt_link_page_req.pending_alt_link_page_req )
          {
            /* The pending_alt_link_page_req will be reset when receiving the
             * AlternateLinkOpenConf. */
            hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_AN;
            hdrsap_send_alt_link_open_req_msg();
          }
        }/* if ( cur_state == HDRSAPS_OPEN ) */
	  }/* else */
	  break; 
 
    case E_FAILURE:
    default:
      HDRTRACE_TXMSG_FAILED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                             HDRSAP_ALT_LINK_CLOSE_REQ_MSGID, cur_state );

      /* Close virtual connection scilently. */
      if ( cur_state == HDRSAPS_OPEN )
      {
        hdrsap_process_close_virtual_connection();

        /* If AlternateLinkPageReq is received when SAP is OPEN, close the
         * current virtual connection first and send AlternateLinkOpenRequest
         * in SAP CLOSED state. */
        if ( hdrsap.alt_link_page_req.pending_alt_link_page_req )
        {
          /* The pending_alt_link_page_req will be reset when receiving the
           * AlternateLinkOpenConf. */
          hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_AN;
          hdrsap_send_alt_link_open_req_msg();
        }
      }
      break;
  }

  if(status != E_NOT_ALLOWED)
	close_req_msg_na_cnt = 0; /* Reset the counter */
	  
} /* hdrsap_alt_link_close_req_msg_tx_outcome_cb() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRSAP_SEND_ALT_LINK_CLOSE_REQ_MSG

DESCRIPTION
  This function sends the AlternateLinkCloseReq to the access network.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_send_alt_link_close_req_msg ( void )
{

  dsm_item_type  *dsm_ptr; /* MSG buffer pointer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get DSM item buffer */
  dsm_ptr = hdrutil_new_msg_buffer();

  /* Populate AltLinkOpenReq structure */
  hdrsap.alt_link_close_req.msg_id = 
      HDRSAP_ALT_LINK_CLOSE_REQ_MSGID;
  hdrsap.alt_link_close_req.trans_id++;

  /* Pack AlternateLinkCloseReq */
  HDRSAP_PACK_ALT_LINK_CLOSE_REQ_MSG_ID( dsm_ptr, 
                                         hdrsap.alt_link_close_req.msg_id );
  HDRSAP_PACK_ALT_LINK_CLOSE_REQ_TRANS_ID( dsm_ptr, 
                                          hdrsap.alt_link_close_req.trans_id );

  /* Send the message */
  hdrhmp_send_msg
  (
    HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
    HDRHAI_AC,
    FALSE,
    HDRSAP_ALT_LINK_CLOSE_REQ_PRIORITY,
    HDRHAI_IN_USE, /* Instance of protocol sending message */
    dsm_ptr,
    hdrsap_alt_link_close_req_msg_tx_outcome_cb,
    (void *) ((dword) hdrsap.alt_link_close_req.trans_id)
  );

} /* hdrsap_send_alt_link_close_req_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_SEND_ALT_LINK_CLOSE_CONF_MSG

DESCRIPTION
  This function sends the alternate link close confirmation message

DEPENDENCIES
  None

PARAMETERS
  trans id - transaction assoicated with alt link close req messaage.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_send_alt_link_close_conf_msg
(
  uint8 trans_id
)
{
  dsm_item_type  *dsm_ptr; /* MSG buffer pointer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get DSM item buffer */
  dsm_ptr = hdrutil_new_msg_buffer();

  /* Pack AlternateLinkCloseConf */
  HDRSAP_PACK_ALT_LINK_CLOSE_CONF_MSG_ID( dsm_ptr, 
                                          HDRSAP_ALT_LINK_CLOSE_CONF_MSGID );
  HDRSAP_PACK_ALT_LINK_CLOSE_CONF_TRANS_ID( dsm_ptr, trans_id );

  /* Send the message */
  hdrhmp_send_msg
  (
    HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
    HDRHAI_AC,
    FALSE,
    HDRSAP_ALT_LINK_CLOSE_CONF_PRIORITY,
    HDRHAI_IN_USE, /* Instance of protocol sending message */
    dsm_ptr,
    NULL,
    (void *) ((dword) trans_id)
  );

} /* hdrsap_send_alt_link_close_conf_msg */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_ALT_LINK_OPEN_COMP_MSG_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  message.

DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_alt_link_open_comp_msg_tx_outcome_cb
(
  void                 *data,
  hdrerrno_enum_type    status
)
{
  hdrsaps_state_enum_type cur_state= hdrsaps_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (!hdrsap.alt_link_open_comp.awaiting_slp_callback)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "Ignore tx outcome cb s:%d, d:%d",
                  status, (uint32)data);
    return;
  }

  if(!hdrsap_is_alt_link_open_comp_trans_id_matched(data))
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                   "tx outcome cb trans_id mismatch s:%d, d:%d", 
                   status, (uint32)data);
    return;
  }

  hdrsap.alt_link_open_comp.awaiting_slp_callback = FALSE;

  switch(status)
  {
    case E_SUCCESS:
      /* message transmitted successfully */
      HDRTRACE_TXMSG_ACKED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                            HDRSAP_ALT_LINK_OPEN_COMP_MSGID, cur_state ); /*lint !e506 */

      hdrind_give_ind(HDRIND_SAP_ALT_LINK_COMP_CB_RECEIVED, NULL);
      hdrsaptx_start_tx(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, HDRHAI_RTC);
      break;

    case E_ABORTED:
      HDRTRACE_TXMSG_ABORTED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                              HDRSAP_ALT_LINK_OPEN_COMP_MSGID, cur_state ); /*lint !e506 */
      /* TODO: give ind to hmp to clear pending queue? */
      break;

    case E_FAILURE:
       /* Ignore failure of ALTERNATE LINK OPEN COMPLETE message */
       HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "Ignore : L2 ACK failure for ALT LINK OPEN COMP");    

       hdrind_give_ind(HDRIND_SAP_ALT_LINK_COMP_CB_RECEIVED, NULL);
       hdrsaptx_start_tx(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, HDRHAI_RTC);
    default:
      HDRTRACE_TXMSG_FAILED( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                              HDRSAP_ALT_LINK_OPEN_COMP_MSGID, cur_state ); /*lint !e506 */
      break;
  }

} /* hdrsap_alt_link_open_comp_msg_tx_outcome_cb() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_SEND_ALT_LINK_OPEN_COMP_MSG

DESCRIPTION
  This function sends the alternate link open complete message

DEPENDENCIES
  None

PARAMETERS
  trans id - transaction assoicated with alt link close req messaage.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_send_alt_link_open_comp_msg
(
  uint8 trans_id
)
{
  dsm_item_type  *dsm_ptr; /* MSG buffer pointer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrsap.alt_link_open_comp.awaiting_slp_callback = TRUE;
  hdrsap.alt_link_open_comp.trans_id = trans_id;

  /* Get DSM item buffer */
  dsm_ptr = hdrutil_new_msg_buffer();

  /* Pack AlternateLinkOpenComp */
  HDRSAP_PACK_ALT_LINK_OPEN_COMP_MSG_ID( dsm_ptr, 
                                          HDRSAP_ALT_LINK_OPEN_COMP_MSGID );
  HDRSAP_PACK_ALT_LINK_OPEN_COMP_TRANS_ID( dsm_ptr, trans_id );

  /* Send the message */
  hdrhmp_send_msg
  (
    HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
    HDRHAI_AC, 
    FALSE,
    HDRSAP_ALT_LINK_CLOSE_CONF_PRIORITY,
    HDRHAI_IN_USE, /* Instance of protocol sending message */
    dsm_ptr,
    hdrsap_alt_link_open_comp_msg_tx_outcome_cb,
    (void *) ((dword) trans_id)
  );

} /* hdrsap_send_alt_link_open_comp_msg */


/*===========================================================================
FUNCTION HDRSAP_PROCESS_OPEN_CONNECTION_CMD

DESCRIPTION
  This function processes SAP open connection command.

DEPENDENCIES
  None

PARAMETERS
  sap_cmd_ptr - Pointer to the command for SAP to process

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_process_open_connection_cmd
(
  hdrsap_cmd_type *sap_cmd_ptr
)
{
  hdrsaps_state_enum_type current_state;
  hdrind_idle_conn_fail_info_type conn_fail_info;
  boolean open_conn_fail = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  current_state = hdrsaps_get_state();

  /* TBD, change the backoff timer value */
  if ( hdrsap_get_current_tunnel_mode_value( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
         == HDRHAI_TUNNEL_MODE_ACTIVE_HO )
  {
    /* Cannot open connection when active handoff is in progress */
    conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_ACTIVE_HO;
    conn_fail_info.backoff_time = 0;
    open_conn_fail = TRUE;
  }
  else if ( hdrsap_get_current_tunnel_mode_value( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
              == HDRHAI_TUNNEL_MODE_NO_SRV )
  {
    /* Cannot open connection when there is no LTE service */
    conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_NO_SRV;
    conn_fail_info.backoff_time = 0;
    open_conn_fail = TRUE;
  }
  else if ( !hdrovhd_prereg_is_allowed( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) )
  {
    /* Cannot open connection when there is no LTE service */
    conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_PREREG_NOT_ALLOW;
    conn_fail_info.backoff_time = 0;
    open_conn_fail = TRUE;
  }
  else if ( ( sap_cmd_ptr->params.open_connection.reason ==
                HDRHAI_OPEN_USER_REQ ) && 
            !hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) )
  {
    /* Cannot open connection requested by user when there is no HDR session */
    conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_NO_SESSION;
    conn_fail_info.backoff_time = 0;
    open_conn_fail = TRUE;
  }
  else if ( ( sap_cmd_ptr->params.open_connection.reason ==
                HDRHAI_OPEN_USER_REQ ) && 
            ( hdrscp_get_current_subtype(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL) 
               != HDRSCP_IRAT_SAP ) )
  {
    /* In the standard, 'if the protocol subtype of the Signaling Adaptation
     * Protocol is equal to 0x0000 or if there is no InUse instance of the
     * Signaling Adaptation Protocol, then the access terminal shall queue 
     * the command and execute it when the access terminal enters the Idle 
     * State.' 
     * In our implemenation, we will return failure with specific reason to 
     * upper layer and let upper layer decide what to do. */ 
    conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_NOT_ACTIVE;
    conn_fail_info.backoff_time = 0;
    open_conn_fail = TRUE;
  }

  if ( open_conn_fail )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                    "SAP: Open Connection cmd fails with reason 0x%x, "
                    "backoff timer %d",
                    conn_fail_info.failure_reason, 
                    conn_fail_info.backoff_time );

    /* Usually if the above failure happens, the open connection cmd is
     * requested by DS. Notify DS about the failure */
    hdrind_give_ind( HDRIND_SAP_CONNECTION_FAILED, 
                     (void *)&conn_fail_info );
    return;
  }

  if ( current_state == HDRSAPS_OPEN )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "SAP: Rcvd Open Connection cmd in OPEN state" );

    hdrpac_mdm_sn_init(); 

    hdrind_give_ind( HDRIND_SAP_CONNECTION_ALREADY_OPENED, 
                     (void *)&hdrsap.alt_link_open_req.internal_reason );
  }
  else if ( current_state == HDRSAPS_CLOSE )
  {
    /* Populate the internal open connection reason */
    switch ( sap_cmd_ptr->params.open_connection.reason )
    {  
      case HDRHAI_OPEN_USER_REQ:
        hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_USER;
        break;

      case HDRHAI_OPEN_SEND_RTC_MSG:
        hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_MSG;
        break;

      default:
        break;
    }

    hdrsap_send_alt_link_open_req_msg();
  }
  else
  { 
    ASSERT( current_state == HDRSAPS_SETUP );

    /* Update the open connection reason */
    switch( sap_cmd_ptr->params.open_connection.reason )
    {
      case HDRHAI_OPEN_USER_REQ:
        hdrsap.alt_link_open_req.internal_reason &= ~HDRSAP_CR_ABORT;
        hdrsap.alt_link_open_req.internal_reason |= HDRSAP_CR_USER;
        break;

      case HDRHAI_OPEN_SEND_RTC_MSG:
        hdrsap.alt_link_open_req.internal_reason &= ~HDRSAP_CR_ABORT;
        hdrsap.alt_link_open_req.internal_reason |= HDRSAP_CR_MSG;
        break;

      default:
        break;
    }

    /* Ignore open connection command if SAP is in other state. */ 
    HDR_MSG_PROT( MSG_LEGACY_MED, "Process Open Connection Command Ignored" );
  }

} /* hdrsap_process_open_connection_cmd() */


/*===========================================================================
FUNCTION HDRSAP_PROCESS_ABORT_CONNECTION

DESCRIPTION
  This function processes SAP abort connection.

DEPENDENCIES
  None

PARAMETERS
   abort_reason - reason for aborting the open connection request

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdrsap_process_abort_connection
(
  hdrhai_abort_reason_enum_type abort_reason
)
{
  if ( abort_reason == HDRHAI_ABORT_USER )
  {
    if( hdrsap.alt_link_open_req.internal_reason == HDRSAP_CR_USER )
    {
      /* The user was the only reason for the connection. Abort. */
      hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_ABORT;
      hdrsap.abort_connection = TRUE;
    }
    else if ( hdrsap.alt_link_open_req.internal_reason & HDRSAP_CR_USER )
    {
      /* Remove the user bit from the connect reason and continue
       * to setup the connection
       */
      hdrsap.alt_link_open_req.internal_reason &= ~HDRSAP_CR_USER;
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "connect reason: cleared: %d, current value: %d",
                      HDRSAP_CR_USER, 
                      hdrsap.alt_link_open_req.internal_reason );
    }
    else
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                      "SAP: Invalid abort open conn reason %d", 
                      abort_reason );
    }
  }
  else if ( abort_reason == HDRHAI_ABORT_MSG )
  {
    if( hdrsap.alt_link_open_req.internal_reason == HDRSAP_CR_MSG )
    {
      /* The MSG was the only reason for the connection and there is no more
       * msg in the pending_q, Abort. */
      hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_ABORT;
      hdrsap.abort_connection = TRUE;
    }
    else if ( hdrsap.alt_link_open_req.internal_reason & HDRSAP_CR_MSG )
    {
      /* Remove the MSG bit from the connect reason and continue
       * to setup the connection
       */
      hdrsap.alt_link_open_req.internal_reason &= ~HDRSAP_CR_MSG;
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "connect reason: cleared: %d, current value: %d",
                      HDRSAP_CR_MSG, 
                      hdrsap.alt_link_open_req.internal_reason );
    }
    else
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                      "SAP: Invalid abort open conn reason %d", 
                      abort_reason );
    }
  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "SAP: Invalid abort open conn reason %d",
                    abort_reason );
  }

  if ( hdrsap.abort_connection )
  {
    if ( hdrsap.alt_link_open_req.awaiting_slp_callback )
    {
      /* If we are waiting for L2 ACK, handle abort when L2 ACK is received.
       * a. if L2 ACK failed, move to CLOSE state, abort is completed;
       * b. if L2 ACK is successful, just wait for L3 ACK.
       */ 
    }
    else if ( rex_get_timer( &hdrsap.state_timer ) != 0 )
    {
      /* L2 ACK is successful and we are waiting for AlternateLinkOpenConf
       * do nothing here and handle abort when AlternateLinkOpenConf is
       * received or timeout. */
    }
    else
    {
      /* This is not expected since SAP is in SETUP state. */
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "SAP: Invalid abort" );
    }
  }

} /* hdrsap_process_abort_connection() */

/*===========================================================================
FUNCTION HDRSAP_PROCESS_CLOSE_CONNECTION_CMD

DESCRIPTION
  This function processes SAP close connection command.

DEPENDENCIES
  None

PARAMETERS
  sap_cmd_ptr - Pointer to the command for SAP to process

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_process_close_connection_cmd
(
  hdrsap_cmd_type * sap_cmd_ptr
)
{
  hdrsaps_state_enum_type current_state = hdrsaps_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( current_state == HDRSAPS_CLOSE )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Process Close Connection Command Ignored" );
    return;
  }
  else if ( current_state == HDRSAPS_SETUP )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "SAP in SETUP, abort the connection ..." );

    hdrsap_process_abort_connection(
      sap_cmd_ptr->params.close_connection.reason );
    return;
  }

  /* Flush pending ACKs for all reliable messages sent by AN before 
     closing the connection */
  hdrslp_flush_pending_acks();

  /* Flush queue and send out all the pending ACKs before connection is closed. */
  hdrsaptx_start_tx( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, 
                     HDRHAI_RTC );

  HDR_MSG_PROT( MSG_LEGACY_MED, "Process Close Connection Command after RTC flushed" );

  if ( sap_cmd_ptr->params.close_connection.silent_close )
  {
    hdrsap_process_close_virtual_connection();
  }
  else
  {
    hdrsap_send_alt_link_close_req_msg();
  }

} /* hdrsap_process_close_connection_cmd() */

/*===========================================================================
FUNCTION HDRSAP_IDLE_HANDOFF_READY

DESCRIPTION
  This function closes virtual connection before idle handoff.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_idle_handoff_ready( void )
{
  HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                  "Idle handoff ready: sap state (%d)", hdrsaps_get_state() );

  if ( hdrsaps_get_state() == HDRSAPS_OPEN )
  {
    hdrsap_process_close_virtual_connection();
  }
  else if ( hdrsaps_get_state() == HDRSAPS_SETUP )
  {
    hdrsap_process_fail_connection();
  }
  else 
  {
    hdrsaptx_stop_tx( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );
  }

  /* Clear all the timers */  
  ( void ) rex_clr_timer( &hdrsap.state_timer );      
  ( void ) rex_clr_timer( &hdrsap.backoff_timer );

} /* hdrsap_idle_handoff_ready() */

/*===========================================================================
FUNCTION HDRSAP_PROCESS_IDLE_HANDOFF_CMD

DESCRIPTION
  This function processes SAP idle handoff.

DEPENDENCIES
  None

PARAMETERS
  sap_cmd_ptr - Pointer to the command for SAP to process

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_process_idle_handoff_cmd
(
  hdrsap_cmd_type * smp_cmd_ptr
)
{
  boolean last_cell_prereg_is_allowed = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  last_cell_prereg_is_allowed = 
    hdrovhd_prereg_is_allowed( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );

  /* Disable tunnel mode */
  hdrsap_set_tunnel_mode( HDRHAI_TUNNEL_MODE_DISABLED );

  /* Give Idle ho ind before deactiving OMP because ovhd_updated ind from OMP 
   * Should always happen after idle handoff.
   * Return indication so that other protocol can move out of tunnel state.
   */
  hdrind_give_ind( HDRIND_SAP_IDLE_HANDOFF, &last_cell_prereg_is_allowed );

  /* Move OMP from tunnel mode to process all messages state */
  hdrovhd_tunnel_deactivate( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );

} /* hdrsap_process_idle_handoff_cmd */


/*===========================================================================
FUNCTION HDRSAP_PROCESS_ACTIVE_HANDOFF_CMD

DESCRIPTION
  This function processes SAP active handoff.

DEPENDENCIES
  None

PARAMETERS
  sap_cmd_ptr - Pointer to the command for SAP to process

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_process_active_handoff_cmd
(
  hdrsap_cmd_type * smp_cmd_ptr
)
{
  dsm_item_type *rup_msg_item_ptr;       
    /* dsm item pointer to RU message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrscp_get_current_subtype(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL) 
         != HDRSCP_IRAT_SAP )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "SAP subtype is not IRAT, ignore active handoff request" );
    return;
  }

  rup_msg_item_ptr = hdrrup_get_rup_msg( HDRRUP_BUILD_NEW_RUP_MESSAGE, 
                                         HDRRUP_RUP_SIZE_NO_RESTRICTION );

  if ( rup_msg_item_ptr != NULL )
  {
    hdrsap.rup_msg_item_ptr = rup_msg_item_ptr;
  }
  else
  {
    hdrsap.rup_msg_item_ptr = NULL;
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
        "No EV-DO pilots returned by LTE ignoring active handoff request");
    return;
  }

  if ( hdrsaps_get_state() == HDRSAPS_OPEN )
  {
    hdrsap_process_close_virtual_connection();
  }
  else if ( hdrsaps_get_state() == HDRSAPS_SETUP )
  {
    hdrsap_process_fail_connection();
  }
  else 
  {
    hdrsaptx_stop_tx( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );
  }

  /* Set tunnel mode to active handoff */
  hdrsap_set_tunnel_mode ( HDRHAI_TUNNEL_MODE_ACTIVE_HO );

  /* Activate IDLE to enter Tunnel state */
  hdridle_activate( HDRHAI_SAP_RX_MODULE );

  /* Send request to ALMP to open connection that is send RU + CR */
  hdralmp_open_connection( HDRHAI_OPEN_ACTIVE_HO, HDRHAI_SAP_RX_MODULE );

} /* hdrsap_process_active_handoff_cmd */

/* EJECT */

/*===========================================================================

FUNCTION HDRSAP_PROCESS_CONTINUE_ACTIVE_HANDOFF_CMD

DESCRIPTION
 This function continues processing the active handoff request after all protocols
 and SRCH have been activated to process TCA.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsap_process_continue_active_handoff_cmd 
(
  hdrsap_cmd_type * smp_cmd_ptr
)
{
  hdrmc_lte_to_hdr_active_ho_info_type *active_ho_payload;    
    /* Pointer to active handoff request payload (OLP, HSP and TCA) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdrsap_lte_to_hdr_active_ho_is_in_progress())
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
              "Ignoring network acquired ind not in active ho state");
    return;
  }

  /* Move OMP from tunnel mode to process all messages state */
  hdrovhd_tunnel_deactivate( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );

  /* Activate RUP to enter Tunnel state */
  hdrrup_activate( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );

  /* In LTE to HDR active handoff CMAC is not activated until TCA is processed
     because INIT does not go through acquistion procedure rather SRCH is put
     in a mode to directly process TCA. So we need to activate CMAC now. */
  hdrcmac_activate( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );

  /* Set active_ho_payload to poin to the LTE message that contains OLP, HSP 
     and TCA */
  active_ho_payload = smp_cmd_ptr->params.continue_active_ho.ho_msg_ptr;

  /* Now that we are going to process the LTE message that contains DSM 
     item clear the flag that dsm need not be freed once the 
     hdrsaprx_process_lte_dl_msg() is entered it will clear it. If the 
     active handover fails at any point earlier than this dsm item has to
     be freed.
     */
  active_ho_payload->free_dsm_item = FALSE;

  /* Process the OLP, HSP and TCA message passed as mobility from EUTRA message */
  hdrsaprx_process_lte_dl_msg( &(active_ho_payload->msg_payload) );

} /* hdrsap_process_continue_active_handoff_cmd() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to SAP.

DEPENDENCIES
  None

PARAMETERS
  sap_cmd_ptr - Pointer to the command for SAP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_process_cmd
(
  hdrsap_cmd_type * sap_cmd_ptr
)
{
  HDRTRACE_CMD( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, sap_cmd_ptr->name,
    sap_cmd_ptr->sender, hdrsaps_get_state() );

  switch ( sap_cmd_ptr->name )
  {
    case HDRSAP_ACTIVATE_CMD:        /* Begin Opening a Session*/
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Activate Command");
      hdrsap_process_activate( sap_cmd_ptr);
      break;

    case HDRSAP_DEACTIVATE_CMD:      /* Deactivate the protocol */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Deactivate Command");
      hdrsap_process_deactivate(sap_cmd_ptr);
      break;

    case HDRSAP_OPEN_CONNECTION_CMD:      /* Open Connection */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Open Connection Command");
      hdrsap_process_open_connection_cmd( sap_cmd_ptr );
      break;
  
    case HDRSAP_CLOSE_CONNECTION_CMD:      /* Close Connection */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Close Connection Command");
      hdrsap_process_close_connection_cmd( sap_cmd_ptr );
      break;

    case HDRSAP_IDLE_HANDOFF_CMD:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Idle Handoff Command");
      hdrsap_process_idle_handoff_cmd( sap_cmd_ptr );
      break;

    case HDRSAP_ACTIVE_HO_CMD:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Active Handoff Command");
      hdrsap_process_active_handoff_cmd( sap_cmd_ptr );
      break;

    case HDRSAP_CONTINUE_ACTIVE_HO_CMD:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Continue Active Handoff Command");
      hdrsap_process_continue_active_handoff_cmd( sap_cmd_ptr );
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Unrecognized command %d", 
                      sap_cmd_ptr->name );
      break;
  }

} /* hdrsap_process_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_START_BACKOFF_TIMER

DESCRIPTION
  This function starts the backoff timer

DEPENDENCIES
  None

PARAMETERS
  cnt - timer count in millisecond

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
void hdrsap_start_backoff_timer
(
  rex_timer_cnt_type cnt
)
{
  rex_timer_cnt_type backoff_time;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cnt != 0)
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "backoff timer provide by NAS");
    backoff_time = cnt;
    /* tx_failure_cnt_ac is only used to decide the backoff time
     * if backoff time was provided by NAS, we do not need it
     */
    hdrsap.tx_failure_cnt_ac = 0;
  }
  else
  {
    if( hdrsap.tx_failure_cnt_ac == 0 )
    {
    
      HDR_MSG_PROT (MSG_LEGACY_HIGH,
                    "Do not start backoff_timer as tx_failure_cnt_ac is 0");
      return;
    }
    else if (hdrsap.tx_failure_cnt_ac < HDRSAP_BACKOFF_TIMER_CNT_MAX_AC )
    {
      backoff_time = backoff_time_table_l2_failed[hdrsap.tx_failure_cnt_ac -1] *
                      HDRSAP_SECOND_TO_MS_CONVERSION;
      HDR_MSG_PROT_3( MSG_LEGACY_MED, "HDR backoff: fail_cnt %d, id %d, time %d",
                      hdrsap.tx_failure_cnt_ac,
                      hdrsap.tx_failure_cnt_ac -1,
                      backoff_time);
    }
    else
    {
      /* use max backoff time */
      backoff_time = backoff_time_table_l2_failed[HDRSAP_BACKOFF_TIMER_CNT_MAX_AC -1 ] *
                      HDRSAP_SECOND_TO_MS_CONVERSION;
      HDR_MSG_PROT_3( MSG_LEGACY_MED, "HDR backoff: fail_cnt %d, id %d, time %d",
                      hdrsap.tx_failure_cnt_ac,
                      HDRSAP_BACKOFF_TIMER_CNT_MAX_AC - 1,
                      backoff_time);
    }
  }

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "Start backoff timer %dms", backoff_time );
  ( void ) rex_set_timer( &hdrsap.backoff_timer, backoff_time);

} /* hdrsap_start_backoff_timer() */

/*===========================================================================

FUNCTION HDRSAP_STOP_BACKOFF_TIMER

DESCRIPTION
  This function stops backoff timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_stop_backoff_timer( void )
{
  if( ( rex_get_timer( &hdrsap.backoff_timer ) != 0 ) && 
      hdrovhd_prereg_is_allowed( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) )
  {
    ( void ) rex_clr_timer( &hdrsap.backoff_timer );
    HDR_MSG_PROT( MSG_LEGACY_MED, "Stop backoff timer" );
    hdrind_give_ind( HDRIND_SAP_BACKOFF_TIMER_EXPIRED, NULL );
  }

} /* hdrsap_stop_backoff_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_PROCESS_SAPTX_TRANSMISSION_FAILED_AC_IND

DESCRIPTION
  This function precess transmission failed indication  

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_process_saptx_transmission_failed_ac_ind
(
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  hdrind_idle_conn_fail_info_type conn_fail_info;
  boolean abort_connection = hdrsap.abort_connection;
  hdrsap_ac_msg_enum_type ac_msg_type_l2ack_fail = hdrsap_get_ac_msg_type();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_4( MSG_LEGACY_HIGH, "cause %d, time %d, count %d ac msg type %d",
                  ind_data_ptr->irat_failure_info.cause_type,
                  ind_data_ptr->irat_failure_info.backoff_time,
                  hdrsap.tx_failure_cnt_ac,
                  ac_msg_type_l2ack_fail);

  hdrsap.tx_failure_cnt_ac++;
  
  switch(ac_msg_type_l2ack_fail)
  {
    case HDRSAP_UATI_REQ:	        
      hdrsap_start_backoff_timer( ind_data_ptr->irat_failure_info.backoff_time );
      break;
	  
    case HDRSAP_LINK_OPEN_REQ:	
      conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_L2_FAILED;
      conn_fail_info.backoff_time = 0;
      hdrind_give_ind( HDRIND_SAP_CONNECTION_FAILED, (void*)&conn_fail_info );
      hdrsaps_change_state ( HDRSAPS_CLOSE );      
	 
      if ( hdrsap.alt_link_open_req.internal_reason == HDRSAP_CR_USER )
      {
          /* User is the only reason to open the connection. HDR won't set
           * backoff timer and lets DS retry. */
             HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "DS retries the connection ..." );
	     hdrsap.tx_failure_cnt_ac--; // this case is not counted as failure
      }
      else
      {	      
          if ( !abort_connection )	
          {		      
              hdrsap_start_backoff_timer( ind_data_ptr->irat_failure_info.backoff_time );
          }
      }	
      break;
	 
    default:	  
      break;  
  }  
  hdrsap_set_ac_msg_type(HDRSAP_NO_MSG); //clear pending msg type  
  (void) hdrovhd_get_lte_cell_id( &hdrsap.cell_id_l2_failed );   

} /* hdrsap_process_saptx_transmission_failed_ac_ind() */


/*===========================================================================

FUNCTION HDRSAP_PROCESS_OVHD_PREREG_INFO_UPDATED_IND

DESCRIPTION
  This function processes PreReg updated indication

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_process_ovhd_prereg_info_updated_ind
(
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  lte_rrc_global_cell_id_t cell_id = 0; 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( ( rex_get_timer( &hdrsap.backoff_timer ) != 0 ) && 
      ind_data_ptr->prereg_is_allowed )
  {
    /* Get current cell id */
    ( void ) hdrovhd_get_lte_cell_id( &cell_id );
    HDR_MSG_PROT_2( MSG_LEGACY_MED, "current cell %d, l2_failed cell %d",
                    cell_id, hdrsap.cell_id_l2_failed );

    if ( cell_id != hdrsap.cell_id_l2_failed )
    {
      /* new cell and PreReg is allowed, stop backoff timer and try immediately */
      ( void ) rex_clr_timer( &hdrsap.backoff_timer );
      hdrsap.tx_failure_cnt_ac = 0;
      hdrind_give_ind( HDRIND_SAP_BACKOFF_TIMER_EXPIRED, NULL );
    }
  }

} /* hdrsap_process_ovhd_prereg_info_updated_ind() */


/*===========================================================================

FUNCTION HDRSAP_PROCESS_SET_PREREG_STATUS_IND

DESCRIPTION
  This function processes HDRIND_SAP_SET_PREREG_STATUS indication

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_process_set_prereg_status_ind
(
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  boolean session_is_open = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  session_is_open = ( hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) &&
                        hdrscp_is_personality_irat() );
    
  HDR_MSG_PROT_4( MSG_LEGACY_MED, "preReg status %d, NV %d, ind %d, session %d",
                  hdrsap.curr_prereg_status, hdrsap.set_prereg_status, 
                  ind_data_ptr->prereg_status_event, session_is_open );
  
  /* If there is no HDR session, active handoff can't continue. Notify LTE
   * that pre-Registration status is FALSE. */
  if ( ind_data_ptr->prereg_status_event 
         == HDRHAI_PREREG_STATUS_EVENT_EVDO_SESSION_CLOSED )
  {
    if ( hdrsap.curr_prereg_status )
    {
      hdrsap.curr_prereg_status = FALSE;
      hdrsap_send_pre_reg_status_to_lte( FALSE );
    }
    return;
  }
        
  if ( ind_data_ptr->prereg_status_event == 
         HDRHAI_PREREG_STATUS_EVENT_UATI_ASSIGNED  )
  {
    if ( rex_get_timer( &hdrsap.backoff_timer ) != 0 ) 
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "backoff timer running while UATI is assigned, stop timer");
      rex_clr_timer( &hdrsap.backoff_timer );
    }
  }

  /* Act based on NV setting */
  switch( hdrsap.set_prereg_status )
  {
    case HDRHAI_PREREG_STATUS_UATI_ASSIGNED:
      if ( ind_data_ptr->prereg_status_event 
             == HDRHAI_PREREG_STATUS_EVENT_UATI_ASSIGNED )
      {
        if ( !hdrsap.curr_prereg_status )
        {
          hdrsap.curr_prereg_status = TRUE;
          hdrsap_send_pre_reg_status_to_lte( TRUE );
        }
      }
      break;

    case HDRHAI_PREREG_STATUS_EVDO_SESSION_OPENED:
      if ( ind_data_ptr->prereg_status_event 
             == HDRHAI_PREREG_STATUS_EVENT_EVDO_SESSION_OPENED )
      {
        if ( session_is_open && !hdrsap.curr_prereg_status )
        {
          hdrsap.curr_prereg_status = TRUE;
          hdrsap_send_pre_reg_status_to_lte( TRUE );
        }
      }
      break;

    case HDRHAI_PREREG_STATUS_PPP_SESSION_OPENED:
      if ( ind_data_ptr->prereg_status_event 
             == HDRHAI_PREREG_STATUS_EVENT_PPP_SESSION_OPENED )
      {
        if ( session_is_open && !hdrsap.curr_prereg_status )
        {
          hdrsap.curr_prereg_status = TRUE;
          hdrsap_send_pre_reg_status_to_lte( TRUE );
        }
      }
      else if ( ind_data_ptr->prereg_status_event 
                  == HDRHAI_PREREG_STATUS_EVENT_PPP_SESSION_CLOSED )
      {
        if ( hdrsap.curr_prereg_status )
        {
          hdrsap.curr_prereg_status = FALSE;
          hdrsap_send_pre_reg_status_to_lte( FALSE );
        }
      }
      break;

    case HDRHAI_PREREG_STATUS_IP_SESSION_OPENED:
      if ( ind_data_ptr->prereg_status_event 
             == HDRHAI_PREREG_STATUS_EVENT_VSNCP_ESTABILISHED )
      {
        if ( session_is_open && !hdrsap.curr_prereg_status )
        {
          hdrsap.curr_prereg_status = TRUE;
          hdrsap_send_pre_reg_status_to_lte( TRUE );
        }
      }
      else if ( ind_data_ptr->prereg_status_event 
                  == HDRHAI_PREREG_STATUS_EVENT_VSNCP_RELEASED )
      {
        if ( hdrsap.curr_prereg_status )
        {
          hdrsap.curr_prereg_status = FALSE;
          hdrsap_send_pre_reg_status_to_lte( FALSE );
        }
      }
      break;

    case HDRHAI_PREREG_STATUS_FULL_CONTEXT:
      if ( ind_data_ptr->prereg_status_event 
             == HDRHAI_PREREG_STATUS_EVENT_QOS_ESTABILISHED )
      {
        if ( session_is_open && !hdrsap.curr_prereg_status )
        {
          hdrsap.curr_prereg_status = TRUE;
          hdrsap_send_pre_reg_status_to_lte( TRUE );
        }
      }
      else if ( ind_data_ptr->prereg_status_event 
                  == HDRHAI_PREREG_STATUS_EVENT_QOS_RELEASED )
      {
        if ( hdrsap.curr_prereg_status )
        {
          hdrsap.curr_prereg_status = FALSE;
          hdrsap_send_pre_reg_status_to_lte( FALSE );
        }
      }
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Unknown preReg status NV item %d",
                      hdrsap.set_prereg_status );
      break;
  }

} /* hdrsap_process_set_prereg_status_ind() */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_PROCESS_IND

DESCRIPTION
  This function processes indications given to SAP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for SAP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{

  HDRTRACE_IND( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, ind_name,
    hdrsaps_get_state() );

  switch ( ind_name )
  {
    case HDRIND_SAPTX_TRANSMISSION_CNF_AC: 
      hdrsap.tx_failure_cnt_ac = 0;

      if( rex_get_timer( &hdrsap.backoff_timer) !=0 )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "L2ACK: Stop backoff timer on AC conf." );
        ( void ) rex_clr_timer( &hdrsap.backoff_timer);        
      }
      break;

    case HDRIND_SAPTX_TRANSMISSION_FAILED_AC: 
      hdrsap_process_saptx_transmission_failed_ac_ind(ind_data_ptr);
      break;

    case HDRIND_AMP_PREREG_ZONE_CHANGED:
      switch ( hdrsaps_get_state() )
      {
        case HDRSAPS_SETUP:
          hdrsap_stop_alt_link_open_conf_timer();

        case HDRSAPS_OPEN:
          hdrsap_process_close_virtual_connection();
          break;
          
        default:
          break; 
      }
      break;

    case HDRIND_OVHD_PREREG_INFO_UPDATED: 
      hdrsap_process_ovhd_prereg_info_updated_ind(ind_data_ptr);
      break;

    case HDRIND_SAP_PREREG_STATUS_UPDATED:
      hdrsap_process_set_prereg_status_ind(ind_data_ptr);
      break;

    case HDRIND_SMP_SESSION_CLOSED:
      if ( hdrsaps_get_state() == HDRSAPS_SETUP )
      {
        /* Normally we wait virtual connection opened and then close it.
         * However, we want to close the connection immediately in this case.
         */
        HDR_MSG_PROT( MSG_LEGACY_MED, "Session is closed when SAP in SETUP" );
        hdrsap_process_alt_link_open_timeout();
      }
      else if ( hdrsaps_get_state() == HDRSAPS_OPEN )
      {
        hdrsap_close_connection( HDRHAI_ABORT_UNSPECIFIED, 
                                 HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, TRUE );
      }
      break;

    default:
      ERR ("Unrecognized Indication %d", ind_name, 0, 0);
      break;
  }
} /* hdrsap_process_ind */

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRSAP_PROCESS_ALT_LINK_OPEN_CONF_MSG

DESCRIPTION
  This function processes the alternate link open confirmation message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_process_alt_link_open_conf_msg
(
  dsm_item_type * item_ptr
)
{
  hdrsaps_state_enum_type curr_state = hdrsaps_get_state();
  hdrerrno_enum_type status = E_SUCCESS;
  uint8 trans_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( curr_state != HDRSAPS_SETUP )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Ignore Alt Link Open Conf Message." );
    HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                                  HDRSAP_ALT_LINK_OPEN_CONF_MSGID,
                                  curr_state );
    return;
  }

  status |= HDRSAP_UNPACK_TRANS_ID( item_ptr, &trans_id );

  if ( status != E_SUCCESS )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Error unpacking trans id" );
  }
  else if ( hdrsap.alt_link_open_req.trans_id != trans_id )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Trans ID mismatch" );
  }
  else
  {
    hdrind_give_ind( HDRIND_SAP_CONNECTION_INITIATED, NULL );
    hdrsaps_change_state( HDRSAPS_OPEN );
    hdrind_give_ind( HDRIND_SAP_LINK_ACQUIRED, NULL );
    hdrind_give_ind( HDRIND_SAP_CONNECTION_OPENED, 
                     (void *)&hdrsap.alt_link_open_req.internal_reason );
    hdrsap_send_alt_link_open_comp_msg( trans_id );
  }

} /* hdrsap_process_alt_link_open_conf_msg */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_PROCESS_ALT_LINK_OPEN_FAIL_MSG

DESCRIPTION
  This function processes the alternate link open fail message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_process_alt_link_open_fail_msg
(
  dsm_item_type * item_ptr
)
{
  hdrsaps_state_enum_type curr_state = hdrsaps_get_state();
  hdrerrno_enum_type status = E_SUCCESS;
  uint8 trans_id;
  hdrind_idle_conn_fail_info_type conn_fail_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( curr_state != HDRSAPS_SETUP )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Ignore Alt Link Open Fail Message." );
    HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                                  HDRSAP_ALT_LINK_OPEN_FAIL_MSGID,
                                  curr_state );
    return;
  }

  status |= HDRSAP_UNPACK_TRANS_ID( item_ptr, &trans_id );

  if ( status != E_SUCCESS )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Error unpacking trans id" );
  }
  else if ( hdrsap.alt_link_open_req.trans_id != trans_id )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Trans ID mismatch" );
  }
  else
  {
    conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_L3_FAILED;
    conn_fail_info.backoff_time = 0;
    hdrind_give_ind( HDRIND_SAP_CONNECTION_FAILED, (void*)&conn_fail_info );
    hdrsaps_change_state( HDRSAPS_CLOSE );    
  }

} /* hdrsap_process_alt_link_open_fail_msg */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRSAP_PROCESS_ALT_LINK_CLOSE_REQ_MSG

DESCRIPTION
  This function processes the alternate link open confirmation message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_process_alt_link_close_req_msg
(
  dsm_item_type * item_ptr
)
{
  hdrsaps_state_enum_type curr_state = hdrsaps_get_state();
  hdrerrno_enum_type status = E_SUCCESS;
  uint8 trans_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( curr_state == HDRSAPS_CLOSE )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,"Ignore Alt Link Open Fail Message.");
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                                 HDRSAP_ALT_LINK_CLOSE_REQ_MSGID,
                                 curr_state); /*lint !e506 */

    return;
  }

  status |= HDRSAP_UNPACK_TRANS_ID(item_ptr, &trans_id);

  if (status != E_SUCCESS)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Error unpacking trans id");
  }
  else
  {
    hdrsap_send_alt_link_close_conf_msg( trans_id );
    hdrsap_process_close_virtual_connection();
  }

} /* hdrsap_process_alt_link_close_req_msg */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRSAP_PROCESS_ALT_LINK_CLOSE_CONF_MSG

DESCRIPTION
  This function processes the alternate link open confirmation message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrsap_process_alt_link_close_conf_msg
(
  dsm_item_type * item_ptr
)
{
  hdrsaps_state_enum_type curr_state = hdrsaps_get_state();
  hdrerrno_enum_type status = E_SUCCESS;
  uint8 trans_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( curr_state == HDRSAPS_CLOSE )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,"Ignore Alt Link Close Conf Message.");
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                                 HDRSAP_ALT_LINK_CLOSE_CONF_MSGID,
                                 curr_state); /*lint !e506 */

    return;
  }
  else if( curr_state == HDRSAPS_SETUP )
  {
    /* Per standard, UE should handle AlternateLinkCloseConf in SETUP state.
     * The use case is UE sends out AlternateLinkCloseReq in SETUP. However,
     * With our implementation, AlternateLinkCloseReq is delayed to OPEN state 
     * when the request is received in SETUP state. So there is no use case 
     * with our implementation. To avoid the following race condition, we 
     * ignore AlternateLinkCloseConf in SETUP state. 
     * Race condition scenario: 
     * AT sends AlternateLinkCloseReq (trans id 1)
     * AT receives AlternateLinkCloseReq from network
     * AT sends AlternateLinkCloseConf
     * AT enters SAP CLOSE
     * AT enters SAP SETUP
     * AT sends AlternateLinkOpenReq
     * AT receives AlternateLinkCloseConf from network (trans id 1)
     * AT enters SAP CLOSED <-- not expected
     */ 
    HDR_MSG_PROT( MSG_LEGACY_HIGH,
                  "Ignore Alt Link Close Conf Message in SETUP state" );
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                                 HDRSAP_ALT_LINK_CLOSE_CONF_MSGID,
                                 curr_state); /*lint !e506 */
    return;
  }

  status |= HDRSAP_UNPACK_TRANS_ID(item_ptr, &trans_id);

  if (status != E_SUCCESS)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Error unpacking trans id");
  }
  else if (hdrsap.alt_link_close_req.trans_id != trans_id)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Trans ID mismatch");
  }
  else
  {
    hdrsap_stop_alt_link_close_conf_timer();
    hdrsap_process_close_virtual_connection();

    /* If AlternateLinkPageReq is received when SAP is OPEN, close the
     * current virtual connection first and send AlternateLinkOpenRequest
     * in SAP CLOSED state. */
    if ( hdrsap.alt_link_page_req.pending_alt_link_page_req )
    {
      /* The pending_alt_link_page_req will be reset when receiving the
       * AlternateLinkOpenConf. */
      hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_AN;
      hdrsap_send_alt_link_open_req_msg();
    }
  }

} /* hdrsap_process_alt_link_close_conf_msg */


/*===========================================================================
FUNCTION HDRSAP_PROCESS_ALT_PAGE_REQ_MSG

DESCRIPTION
  This function processes the AlternateLinkPageReq message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdrsap_process_alt_page_req_msg
(
  dsm_item_type * item_ptr
)
{
  hdrsaps_state_enum_type current_state = hdrsaps_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SAP is in OPEN state and receives a AlternateLinkPageReq, SAP
   * should close the virtual connection first and process the
   * AlternateLinkPageReq in CLOSED state. */
  if ( current_state == HDRSAPS_OPEN )
  {
    hdrsap.alt_link_page_req.pending_alt_link_page_req = TRUE;
    hdrsap_close_connection( HDRHAI_ABORT_UNSPECIFIED, 
                             HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                             FALSE );
  }
  else if ( current_state == HDRSAPS_SETUP )
  {
    /* set user abort bit to 0 */
    hdrsap.alt_link_open_req.internal_reason &= ~HDRSAP_CR_ABORT;

    /* connection setup is in progress. update the connect_reason */
    hdrsap.alt_link_open_req.internal_reason |= HDRSAP_CR_AN;
  }
  else
  {
    ASSERT( current_state == HDRSAPS_CLOSE );
    hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_AN;
    hdrsap_send_alt_link_open_req_msg();
  }

} /* hdrsap_process_alt_page_req_msg() */


/*===========================================================================
FUNCTION HDRSAP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to the SAP

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
                         
SIDE EFFECTS
  None.
===========================================================================*/
void hdrsap_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  hdrmc_queue_msg( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, HDRHAI_IN_USE, msg_ptr );
} /* hdrsap_msg_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_PROCESS_MSG

DESCRIPTION
  This function processes messages for SAP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG));

  /* MessageID is the first uint8 */
  if (HDRSAP_UNPACK_MSG_ID(item_ptr, &(msg_id)) != E_SUCCESS)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not extract message_id" );
    dsm_free_packet( &item_ptr );
    return;
  }
  else if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Active HO in progress, drop SAP msg!" );
    dsm_free_packet( &item_ptr );
    return;
  }

  HDRTRACE_MSG( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, msg_id,
                hdrsaps_get_state() );
  if (protocol_instance == HDRHAI_IN_USE)
  {
    switch ( msg_id )
    {
      case HDRSAP_ALT_LINK_OPEN_CONF_MSGID:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Alternate link open conf Msg");
        hdrsap_process_alt_link_open_conf_msg( item_ptr );
        break;

      case HDRSAP_ALT_LINK_OPEN_FAIL_MSGID:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Alternate link open fail Msg");
        hdrsap_process_alt_link_open_fail_msg( item_ptr );
        break;

      case HDRSAP_ALT_LINK_CLOSE_REQ_MSGID:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Alternate link close req Msg");
        hdrsap_process_alt_link_close_req_msg( item_ptr );
        break;

      case HDRSAP_ALT_LINK_CLOSE_CONF_MSGID:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Alternate link close conf Msg");
        hdrsap_process_alt_link_close_conf_msg( item_ptr );
        break;

      case HDRSAP_ALT_LINK_PAGE_REQ_MSGID:
        HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Alternate link page req Msg");
        hdrsap_process_alt_page_req_msg( item_ptr );
        break;

      default:
        ERR("HDRSAP unknown in use message %d", msg_id, 0, 0);
        break;
    }
  }
  else /* (protocol_instance == HDRHAI_IN_CONFIG) */
  {
    switch ( msg_id )
    {

      default:
        ERR ("HDRSAP unknown in config message %d", msg_id, 0, 0);
        break;
    }
  }
  dsm_free_packet( &item_ptr );
} /* hdrsap_process_msg */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSAP_PROCESS_ALT_LINK_OPEN_TIMEOUT

DESCRIPTION
  This function processes alt link open timeout.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrsap_process_alt_link_open_timeout( void ) 
{
  hdrind_idle_conn_fail_info_type conn_fail_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( rex_get_timer( &hdrsap.state_timer ) != 0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "Stop AlternateLinkOpenConf timer" );
    ( void ) rex_clr_timer( &hdrsap.state_timer );
  }

  hdrsap.alt_link_open_req.awaiting_slp_callback = FALSE;  

  conn_fail_info.failure_reason = HDRHAI_CONN_FAIL_SAP_TIMEOUT;
  conn_fail_info.backoff_time = 0;
  hdrind_give_ind( HDRIND_SAP_CONNECTION_FAILED, (void*)&conn_fail_info );
  hdrsaps_change_state( HDRSAPS_CLOSE );

} /* hdrsap_process_alt_link_open_timeout() */


/*=============================================================================
FUNCTION HDRSAP_PROCESS_ALT_LINK_CLOSE_TIMEOUT

DESCRIPTION
  This function processes alt link close timeout.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrsap_process_alt_link_close_timeout( void ) 
{
  hdrsap_process_close_virtual_connection();

  /* If AlternateLinkPageReq is received when SAP is OPEN, close the
   * current virtual connection first and send AlternateLinkOpenRequest
   * in SAP CLOSED state. */
  if ( hdrsap.alt_link_page_req.pending_alt_link_page_req )
  {
    /* The pending_alt_link_page_req will be reset when receiving the
     * AlternateLinkOpenConf. */
    hdrsap.alt_link_open_req.internal_reason = HDRSAP_CR_AN;
    hdrsap_send_alt_link_open_req_msg();
  }

} /* hdrsap_process_alt_link_open_timeout() */


/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSAP_PROCESS_BACKOFF_TIMEOUT

DESCRIPTION
  This function processes backoff timeout.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrsap_process_backoff_timeout
( 
  void
) 
{
  if ( hdrovhd_prereg_is_allowed( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) )
  {
    if (hdrsap.verbose_debug)
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, "HDRIND_SAP_BACKOFF_TIMER_EXPIRED");
    }
    hdrind_give_ind(HDRIND_SAP_BACKOFF_TIMER_EXPIRED, NULL);
  }
  else
  {
    if (hdrsap.verbose_debug)
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, "HDRIND_SAP_BACKOFF_TIMER_STOPPED");
    }
    hdrind_give_ind(HDRIND_SAP_BACKOFF_TIMER_STOPPED, NULL);
  }

}   /* hdrsap_process_backoff_timeout */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSAP_PROCESS_TIMER

DESCRIPTION
  This function is an entry point for hdrmc to process the timer message
  that has been queued up.

DEPENDENCIES
  None.
  
PARAMETERS
  timer_id - The id of the expired timer triggering the function.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrsap_process_timer
( 
  uint32 timer_id 
) 
{
  hdrsaps_state_enum_type cur_state = hdrsaps_get_state();

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( timer_id )
  {
    case HDRSAP_STATE_TIMER:

      switch (cur_state)
      {
        case HDRSAPS_SETUP:
          hdrsap_process_alt_link_open_timeout();
          break;

        case HDRSAPS_OPEN:
          hdrsap_process_alt_link_close_timeout();
             
        default:
          break;
      }
      break;

    case HDRSAP_BACKOFF_TIMER:
      hdrsap_process_backoff_timeout();
      break;

    default:
      break;
  }

} /* hdrsap_process_timer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_SET_TUNNEL_MODE

DESCRIPTION
  This function sets the tunnel mode value.

DEPENDENCIES
  None

PARAMETERS
  tunnel_mode - value to set the tunnel mode.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_set_tunnel_mode
( 
  hdrhai_tunnel_mode_type tunnel_mode
)
{
  hdrhai_tunnel_mode_type prev_tunnel_mode;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  prev_tunnel_mode = hdrsap.tunnel_mode;

  /* Diabled tunnel mode if IRAT subtypes are not supported. */
  if ( hdrscp_is_irat_enabled() )
  {
    hdrsap.tunnel_mode = tunnel_mode;
  }
  else
  {
    hdrsap.tunnel_mode = HDRHAI_TUNNEL_MODE_DISABLED;
  }

  /* When tunnel mode is being disabled reset pre-reg allowed flag */
  if ( hdrsap.tunnel_mode == HDRHAI_TUNNEL_MODE_DISABLED )
  {
    hdrovhd_set_pre_reg_is_allowed( FALSE );

    /* Usually OVHD tunnel info is reset when OVHD leaves TUNNEL ACTIVE state.
     * But we do not reset tunnel info when LTE OOS because we need to know 
     * the previous tunnel info and expect to do nothing if entering the 
     * same LTE cell after OOS; if we don't go back to tunnel after LTE OOS, 
     * tunnel info should be reset. */
    if ( prev_tunnel_mode == HDRHAI_TUNNEL_MODE_NO_SRV )
    {
      hdrovhd_reset_tunnel_info();
    }
  }
  
  if(hdrsap_tunnel_mode_is_enabled(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL)&& (hdrsap.is_prereg_status_send_pending == TRUE))
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "is_prereg_status_send_pending TRUE, sending prereg status");
    /* if pre-reg status is pending to be sent, when tunnel mode is enabled */
    hdrsap.is_prereg_status_send_pending = FALSE;
    hdrsap_send_pre_reg_status_to_lte(hdrsap.curr_prereg_status);	   
  }  
  
  /* Give indication if tunnel mode status is changed. */
  if ( tunnel_mode != prev_tunnel_mode )
  {
    hdrind_give_ind( HDRIND_SAP_TUNNELMODE_STATUS_CHANGED, 
                     (void *)&tunnel_mode );
  }

  HDR_MSG_PROT_2( MSG_LEGACY_MED, "Tunnel mode updated %d -> %d",
                  prev_tunnel_mode, hdrsap.tunnel_mode );

} /* hdrsap_set_tunnel_mode() */

/*===========================================================================

FUNCTION HDRSAP_TUNNEL_MODE_IS_ENABLED

DESCRIPTION
  This function returns whether tunnel mode is enabled or not.

DEPENDENCIES
  None

PARAMETERS
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  TRUE is tunnel mode is enabled, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsap_tunnel_mode_is_enabled 
( 
  hdrhai_protocol_name_enum_type caller
)
{

  HDRTRACE_SAP_TUNNEL_IS_ENABLED( caller, 
                                  hdrsaps_get_state(), 
                                  hdrsap.tunnel_mode );

  if ( (hdrsap.tunnel_mode == HDRHAI_TUNNEL_MODE_ENABLED) ||
       (hdrsap.tunnel_mode == HDRHAI_TUNNEL_MODE_ACTIVE_HO) ||
       (hdrsap.tunnel_mode == HDRHAI_TUNNEL_MODE_NO_SRV) )
  {
    return TRUE;
  }
  else
  {
    ASSERT ( hdrsap.tunnel_mode == HDRHAI_TUNNEL_MODE_DISABLED );
    return FALSE;
  }

} /* hdrsap_tunnel_mode_is_enabled() */


/*===========================================================================

FUNCTION HDRSAP_ALT_LINK_OPEN_COMP_TX_CB_IS_RECEIVED

DESCRIPTION
  This function returns whether AltLinkOpenComp msg 
  tx outcome cb is received or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE is msg outcome cb is received, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsap_alt_link_open_comp_tx_cb_is_received( void )
{

  return !hdrsap.alt_link_open_comp.awaiting_slp_callback;

} /* hdrsap_alt_link_open_comp_tx_cb_is_received() */


/*===========================================================================
FUNCTION HDRSAP_OPEN_CONNECTION

DESCRIPTION
  This function commands SAP to open a connection.

DEPENDENCIES
  None

PARAMETERS 
  reason - The reason the connection is being opened 
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_open_connection
(
  hdrhai_open_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
)
{
  hdrsap_cmd_type hdrsap_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW,  "hdrsap_open_connection" );

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdrsap_cmd.name = HDRSAP_OPEN_CONNECTION_CMD;
  hdrsap_cmd.sender = sender;
  hdrsap_cmd.params.open_connection.reason = reason;

  if ( rex_self() == HDRSAP_TASK_PTR )
  {
    hdrsap_process_cmd( &hdrsap_cmd );
  }
  else
  {
    /* Currently no other task ever asks SAP to open the connection, so
     * we should never get here
     */
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "Attempt to open connection from outside MC(%d)!", sender );
    hdrmc_queue_cmd( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, &hdrsap_cmd,
                     sizeof( hdrsap_cmd_type ) );
  }

} /* hdrsap_open_connection() */

/*===========================================================================
FUNCTION HDRSAP_CLOSE_CONNECTION

DESCRIPTION
  This function commands SAP to close a connection.

DEPENDENCIES
  None

PARAMETERS
  abort_reason - reason to abort connection 
  sender - The protocol giving this command (only for software trace)
  silent_close - whether to send AltLinkCloseReq or not

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_close_connection
(
  hdrhai_abort_reason_enum_type abort_reason,
  hdrhai_protocol_name_enum_type sender,
  boolean silent_close
)
{
  hdrsap_cmd_type hdrsap_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW,  "hdrsap_close_connection" );

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdrsap_cmd.name = HDRSAP_CLOSE_CONNECTION_CMD;
  hdrsap_cmd.sender = sender;
  hdrsap_cmd.params.close_connection.reason = abort_reason;
  hdrsap_cmd.params.close_connection.silent_close = silent_close;

  if ( rex_self() == HDRSAP_TASK_PTR )
  {
    hdrsap_process_cmd( &hdrsap_cmd );
  }
  else
  {
    /* Currently no other task ever asks SAP to close the connection, so
     * we should never get here
     */
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "Attempt to close connection from outside MC(%d)!", sender );
    hdrmc_queue_cmd( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, &hdrsap_cmd,
                     sizeof( hdrsap_cmd_type ) );
  }

} /* hdrsap_close_connection() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_CONNECTION_IS_OPEN

DESCRIPTION
  Boolean access function, can be called from any task to check if a
  SAP connection is open.  

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a SAP connection is open, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsap_connection_is_open( hdrhai_protocol_name_enum_type caller )
{
  /* current state */
  const hdrsaps_state_enum_type cur_state = hdrsaps_get_state(  );
  boolean conn_is_open = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cur_state == HDRSAPS_OPEN && 
      hdrsap_alt_link_open_comp_tx_cb_is_received())
  {
    conn_is_open = TRUE;
  }

  HDRTRACE_SAP_CONNECTION_IS_OPEN( caller, cur_state, conn_is_open );

  return conn_is_open;
} /* hdrsap_connection_is_open() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRSAP_IDLE_HANDOFF

DESCRIPTION
  This function commands SAP to initiate idle handoff procedure.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_idle_handoff
(
  hdrhai_protocol_name_enum_type sender
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrsap_cmd_type hdrsap_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (hdrsap.verbose_debug)
  {
    HDR_MSG_PROT (MSG_LEGACY_MED,  "hdrsap_idle_handoff");
  }

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdrsap_cmd.name = HDRSAP_IDLE_HANDOFF_CMD;
  hdrsap_cmd.sender = sender;

  if ( rex_self() == HDRSAP_TASK_PTR )
  {
    hdrsap_process_cmd( &hdrsap_cmd );
  }
  else
  {
    /* Currently no other task ever asks SAP to close the connection, so
     * we should never get here
     */
    ERR( "Attempt to idle handoff from outside MC!", 0, 0, 0 );
    hdrmc_queue_cmd( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, &hdrsap_cmd,
         sizeof( hdrsap_cmd_type ) );
  }
} /* hdrsap_idle_handoff() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRSAP_CGI_READY

DESCRIPTION
  This function moves protocols to correct state to get CGI

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_cgi_ready( void )
{
  /* Move OMP from tunnel mode to process all messages state */
  hdrovhd_tunnel_deactivate( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL );

} /* hdrsap_cgi_ready() */

/* <EJECT> */ 

/*===========================================================================
FUNCTION HDRSAP_ACTIVE_HANDOFF

DESCRIPTION
  This function commands SAP to process active handoff from LTE.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_active_handoff
(
  hdrhai_protocol_name_enum_type sender
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrsap_cmd_type hdrsap_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdrsap_active_handoff");

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdrsap_cmd.name = HDRSAP_ACTIVE_HO_CMD;
  hdrsap_cmd.sender = sender;

  if ( rex_self() == HDRSAP_TASK_PTR )
  {
    hdrsap_process_cmd( &hdrsap_cmd );
  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                    "Attempt to active handoff from outside MC(%d)!",
                    sender );
    hdrmc_queue_cmd( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, &hdrsap_cmd,
                     sizeof(hdrsap_cmd_type) );
  }
} /* hdrsap_active_handoff() */

/* <EJECT> */ 

/*===========================================================================
FUNCTION HDRSAP_CONTINUE_ACTIVE_HANDOFF

DESCRIPTION
  This function commands SAP to continue processing active handoff after 
  ALMP has entered IDLE state.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  payload - pointer to dsm item that has the OLP + HSP + TCA messages.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_continue_active_handoff
(
  hdrhai_protocol_name_enum_type sender,
  hdrmc_lte_to_hdr_active_ho_info_type *active_ho_payload
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrsap_cmd_type hdrsap_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdrsap_continue_active_handoff");

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdrsap_cmd.name = HDRSAP_CONTINUE_ACTIVE_HO_CMD;
  hdrsap_cmd.sender = sender;
  hdrsap_cmd.params.continue_active_ho.ho_msg_ptr = active_ho_payload;

  if ( rex_self() == HDRSAP_TASK_PTR )
  {
    hdrsap_process_cmd( &hdrsap_cmd );
  }
  else
  {
    /* Currently no other task ever asks SAP to close the connection, so
     * we should never get here
     */
    ERR( "Attempt to idle handoff from outside MC!", 0, 0, 0 );
    hdrmc_queue_cmd( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, &hdrsap_cmd,
         sizeof( hdrsap_cmd_type ) );
  }
} /* hdrsap_continue_active_handoff() */

/* <EJECT> */ 

/*===========================================================================
FUNCTION HDRSAP_BACKOFF_TIMER_IS_ACTIVE

DESCRIPTION
  This function checks if backoff timer is still running.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - backoff timer is running
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrsap_backoff_timer_is_active
(
  hdrhai_protocol_name_enum_type sender
)
{
  boolean timer_is_running = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( rex_get_timer( &hdrsap.backoff_timer ) != 0 )
  {
    timer_is_running = TRUE;
  }
                        
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "backoff timer status: %d", timer_is_running );

  return timer_is_running;

} /* hdrsap_backoff_timer_is_active() */

/*===========================================================================
FUNCTION HDRSAP_LTE_TO_HDR_ACTIVE_HO_IS_IN_PROGRESS

DESCRIPTION
  This function returns whether LTE to HDR active handoff is in progress or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean hdrsap_lte_to_hdr_active_ho_is_in_progress( void ) 
{
  if (hdrsap.tunnel_mode == HDRHAI_TUNNEL_MODE_ACTIVE_HO)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* hdrsap_lte_to_hdr_active_ho_is_in_progress() */

/*===========================================================================
FUNCTION HDRSAP_GET_CURRENT_TUNNEL_MODE_VALUE

DESCRIPTION
  This function returns the current tunnel mode value.

DEPENDENCIES
  None

PARAMETERS
  caller - protocol that is invoking the function

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

hdrhai_tunnel_mode_type hdrsap_get_current_tunnel_mode_value 
( 
  hdrhai_protocol_name_enum_type caller 
)
{
  if (hdrsap.verbose_debug)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "Tunnel mode = %d", hdrsap.tunnel_mode);
  }

  return hdrsap.tunnel_mode;

} /* hdrsap_get_current_tunnel_mode_value() */

/*===========================================================================
FUNCTION HDRSAP_SEND_PRE_REG_STATUS_TO_LTE

DESCRIPTION
  This function sends current pre-reg status to LTE

DEPENDENCIES
  None

PARAMETERS
  ue_is_preregistered - current pre-reg status

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsap_send_pre_reg_status_to_lte
( 
  boolean ue_is_preregistered
)
{
  hdrcp_msg_ind_u prereg_status_ind;
    /* Message to inform LTE about Pre-reg status */

  errno_enum_type estatus = E_FAILURE;
    /* Error status */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!hdrsap_tunnel_mode_is_enabled(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL)) /* if tunnel mode is not enabled, set pending TRUE */
  {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "tunnel mode not enabled. NOT sending prereg status");
       hdrsap.is_prereg_status_send_pending = TRUE;
	   return;
  }
  
  msgr_init_hdr ( &prereg_status_ind.hrpd_prereg_status.hdr, 
                  MSGR_HDR_CP,
                  HDR_CP_HRPD_PREREG_STATUS_IND );

  prereg_status_ind.hrpd_prereg_status.ue_is_registered = ue_is_preregistered;

  estatus = msgr_send ( &prereg_status_ind.hrpd_prereg_status.hdr,
                        sizeof ( hdrcp_msg_ind_u ) );

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "Pre-reg status= %d sent to LTE", ue_is_preregistered );

  ASSERT ( estatus == E_SUCCESS );

} /* hdrsap_send_pre_reg_status_to_lte() */


/*===========================================================================
FUNCTION HDRSAP_SET_PREREG_STATUS

DESCRIPTION
  This function notifies HDR about any events that could change the
  preRegistration status.

DEPENDENCIES
  None

PARAMETERS
  hdrhai_prereg_status_event_enum_type - what type of event it is

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsap_set_prereg_status
( 
  hdrhai_prereg_status_event_enum_type prereg_status_event
)
{
  if(!hdrscp_is_irat_enabled()) /*NV4964 is read in hdrscp_powerup_init()*/
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "IRAT NV is not enabled ");
    return;
  }
  
  if ( prereg_status_event >= HDRHAI_PREREG_STATUS_EVENT_MAX )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Unknown preReg status event %d",
                    prereg_status_event );
    return;
  }

  hdrind_give_ind( HDRIND_SAP_PREREG_STATUS_UPDATED, 
                   (void *)&prereg_status_event );

} /* hdrsap_set_prereg_status() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_GET_RUP_MSG

DESCRIPTION
  This function returns a pointer to DSM item that contains the stored RU
  message.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Pointer to DSM item containing RU message.

SIDE EFFECTS
  None
===========================================================================*/

dsm_item_type* hdrsap_get_rup_msg ( void )
{
  return hdrsap.rup_msg_item_ptr;
} /* hdrsap_get_rup_msg */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_CLAER_RUP_MSG

DESCRIPTION
  This function clears RUP message pointer stored in HDRSAP.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsap_clear_rup_msg ( void )
{
  hdrsap.rup_msg_item_ptr = NULL;
} /* hdrsap_clear_rup_msg */

/*===========================================================================
FUNCTION HDRSAP_GET_TIME_SEND_PREREG_STATUS

DESCRIPTION
  This function returns when UE considers itself to be pre-Registered and
  accordingly set pre-Registration status to TRUE.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  At what point the UE considers itself to be pre-Registered.

SIDE EFFECTS
  None
===========================================================================*/
hdrhai_prereg_status_enum_type hdrsap_get_time_set_prereg_status( void )
{
  return hdrsap.set_prereg_status;

} /* hdrsap_get_time_send_prereg_status() */


/*===========================================================================
FUNCTION HDRSAP_RESET_ABORT_CONNECTION

DESCRIPTION
  This function resets the abort connection flag.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_reset_abort_connection( void )
{
  hdrsap.abort_connection = FALSE;

} /* hdrsap_reset_abort_connection() */

/*===========================================================================

FUNCTION HDRSAP_CHANGE_AC_MSG_TYPE

DESCRIPTION
  Called to set the access channel message, for which L2ACK 
  is failed

DEPENDENCIES
  None

PARAMETERS
  new_msg_type - hdrsap_ac_msg_enum_type

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_set_ac_msg_type ( hdrsap_ac_msg_enum_type new_msg_type )
{
     hdrsap.ac_msg_type = new_msg_type;
}

/*===========================================================================

FUNCTION HDRSAP_GET_AC_MSG_TYPE

DESCRIPTION
  Called to retrieve the current access channel message, for which L2ACK
  is failed

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  new_msg_type - hdrsap_ac_msg_enum_type

SIDE EFFECTS
  None

===========================================================================*/
hdrsap_ac_msg_enum_type hdrsap_get_ac_msg_type( void )
{
    return hdrsap.ac_msg_type;
}

#endif /* FEATURE_LTE_TO_HDR_OH */
