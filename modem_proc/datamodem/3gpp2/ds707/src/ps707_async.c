/*===========================================================================

                        P S 7 0 7 _ A S Y N C 
DESCRIPTION
  This file contains functionality for IS707 async/fax mode-specific
  handlers that deals mainly deals with call control.

EXTERNALIZED FUNCTIONS
  ps707_async_powerup_init()
    Called once at mobile power-up.

  ps707_async_call_init()
    Called as part of per-call init.

  ps707_async_in_rawdata_mode()
    Returns True/False depending on whether a raw data call is in progress.


 Copyright (c) 2002-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps707_async.c_v   1.17   12 Feb 2003 11:21:08   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ps707_async.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what
--------    ---    ------------------------------------------------------------ 
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
04/18/12    jz     Seperate old JCDMA and Simplified JCDMA 
02/08/12    sk     Feature cleanup.
02/11/11    ack    Global Variable Cleanup
11/22/10    sn     Replaced REX premption macros with critical section for 
                   mutual exclusion.
10/13/10    op     Migrated to MSG 2.0 macros
07/26/10    ps     Added code changes to fix the broken Async/Fax support.
04/19/10    gc     klocwork errors
10/06/09    ss     Fixed klocwork errors.
09/04/09    ss     CMI SU level modifications.
03/04/09    sa     AU level CMI modifications.
07/01/08    psng   KlocWork Fixes  
01/22/08    scb/sy Mainlined features under FEATURE_JCDMA
08/27/07    mga    Fixed RVCT Warning 
05/10/07    ac     Lint fix
03/05/07    spn    Removed reference to the tracer field of the DSM Item.
02/12/07    az     Support to request VJ compression during PPP negotiation
12/23/05    ksu    EMPA RSP: two fwd rx queues
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function 
07/22/05    kvd    Added fail_reason to ppp_ev callbacks.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
10/13/04    vp     Removed the byte order of the addresses after a call to 
                   ps_iface_get_peer_addr() and ps_iface_get_addr() as these
                   functions return the addresses in network order now.
09/08/04    vas    QOS related changes
07/21/04    ifk    Replaced ppp_reg_event() calls with ppp_event_deregister()
07/20/04    atp    In cleanup after call, deregistered PPP events(CR46641).
06/16/04    ifk    Replaced ppp_reg_event() with new PPP event functions.
06/11/04    vp     Changes due to changes in PS data structures. Removed
                   inclusion of netuser.h.
03/26/04    vp     Modified reset_tcp call to add RESET (close reason) as
                   argument.
03/16/04    atp    Lint fixes.
01/21/04    sv     Replaced all references to DEF_MSS with PS707_ASYNC_DEF_MSS.
12/02/03    atp    PS now expects port numbers in n/w byte order. (CR37291).
11/12/03    atp    Replaced INTLOCK with TASKLOCK in data_from_tcp_upcall().
11/05/03    rsl    Use dsm_empty_queue to empty ds707_wmk_async_to_sio_tx_q
		   instead of q_get and dsm_free_packet. (CR 36576) 
10/17/03    sv     Modified open_tcp call to pass MSS as an argument.
09/19/03    atp    Added call to ppp_abort() as part of clean up when
                   graceful close of async connection times out.
08/27/03    sv     Modified code to use macro to get/set routing_cache from 
                   scb_ptr
08/14/03    sv     Modified code to use V4mappedV6 address.
08/13/03    ifk    PPP restructuring in preparation for IP6CP.
08/08/03    sv     Restructured the sockets code.
07/22/03    sv     Removed all references to DSM_COOKIE as that went away
                   in the new DSM release.
06/10/03    atp    Initialized ptrs to NULL in process_tcp_send_q() to be on
                   safe side.
05/28/03    ss     Set rwindow to 4*DEF_MSS explicitly so that the rwin size
                   used in async calls is independent of DEF_WND 
05/04/03    ss     Modified code to handle error returned by 
                   dssocki_init_scb()
04/23/03    ss     Modified code to call dssocki_init_scb()
02/12/03    atp    Replaced use of FLOW_ALL for sio flow control.
01/09/03    atp    Replaced dsm_free_buffer() with dsm_free_packet().
12/02/02    atp    Added header, comments, etc.
11/13/02    usb    Replaced DS_IS_FLOW_ENABLED with DS_FLOW_ALL_MASK
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_1X
10/13/02    ak     Added JCDMA code to look for CONNECT during fax calls. 
10/07/02    atp    Added assignment of dummy value to scb's sockfd field.
09/18/02    mvl    Added user_data to ppp event callbacks.  Removed do_mip.
09/17/02    atp    Added routing cache and scb struct to tcb.
08/22/02    atp    Cleanup for shipping.
08/19/02    atp    Support for Um flow control for low-baud SIO links.
08/15/02    atp    Added support for mode-specific SIO wmks.
08/13/02    mvl    Added session handle support for PPP callbacks - made them
                   local to this file.
07/31/02    usb    Removed include of ps.h and psi.h
07/28/02    atp    Sets atcop preprocessor mode on getting CONNECT or 
                   NOCARRIER from IWF.
07/25/02    mvl    Updates for PPP renaming.
07/24/02    atp    Added error checking for ppp function returns.
07/21/02    atp    process_send_q_sig_hdlr() should return TRUE.
07/21/02    atp    Disable flow from sio as soon as nocarrier from IWF.
07/16/02    atp    Set addr type to v4 before getting addr from iface.
07/16/02    atp    Renamed signal PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL.
07/15/02    aku    Added pstimer.h
07/15/02    atp    Added ps707_async_process_tcp_send_q().
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_IS707_ASYNC
#include "ds_flow_control.h"
#include "ds3gsiolib.h"
#include "ds707.h"
#include "ds707_async_defs.h"
#include "ds707_async_mgr.h"
#include "ds707_async_wmk.h"
#include "ds707_wmk.h"
#include "dsati.h"
#include "dsat707extctab.h"
#include "dsat707faxctab.h"
#include "dsm.h"
#include "ps_in.h"
#include "ps_tcp.h"
#include "dstask.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "ps_ip_addr.h"
#include "ps_ppp.h"
#include "ps_svc.h"
#include "ps_iface.h"
#include "ps707_asynci.h"
#include "pstimer.h"
#include "queue.h"
#include "rex.h"
#include "snm.h"
#include "ds707_roaming.h"
#include "data_err_suppr.h"
#include "dss_config.h"
#include "ps_tcp_async.h"
#include "dsutil.h"

/*===========================================================================
                             EXTERNS 
===========================================================================*/


/*===========================================================================
                            TYPEDEFS
===========================================================================*/
/*---------------------------------------------------------------------------
  Timout for TCP close to complete.
---------------------------------------------------------------------------*/
#define PS707_ASYNC_CLOSE_CHK_INTERVAL    1500

/*---------------------------------------------------------------------------
  Timout for PPP close to complete.
---------------------------------------------------------------------------*/
#define PS707_ASYNC_PPP_CLOSE_INTERVAL     700

/*---------------------------------------------------------------------------
  IS-707 Rawdata TCP definitions for the Base Station modem server port and
  the initial Base Station Port number.
---------------------------------------------------------------------------*/
#define RAWDATA_TCP_ACTIVE_PORT         380
#define RAWDATA_TCP_PASSIVE_PORT        379
#define RAWDATA_INITIAL_SOURCE_PORT     0x1000

/*---------------------------------------------------------------------------
  Max length of IAC command including V.42 bis subnegotiation even though
  not currently implemented.
---------------------------------------------------------------------------*/
#define MAX_IAC             20    


/*===========================================================================
                            VARIABLES
===========================================================================*/


/* Declare a task critical section for use in place of TASKLOCK()/TASKFREE(). */
static rex_crit_sect_type ps707_async_data_from_tcp_upcall_crit_sect;

/*---------------------------------------------------------------------------
  Structure that holds state info for async call w.r.t PS task.
---------------------------------------------------------------------------*/
static ps707_async_state_type   ps707_async_state;

/*---------------------------------------------------------------------------
  Open TCP in active/passive mode.
---------------------------------------------------------------------------*/
LOCAL  word ps707_async_tcp_open_mode;

/*---------------------------------------------------------------------------
  Data structure that contains the port numbers for TCP connections.
---------------------------------------------------------------------------*/
LOCAL struct  connection ps707_async_conn;

/*---------------------------------------------------------------------------
  The local TCP port number for async/fax calls.
---------------------------------------------------------------------------*/
LOCAL word    ps707_async_local_port  = RAWDATA_INITIAL_SOURCE_PORT;

/*---------------------------------------------------------------------------
      The following indices and counters are used by
      do_em() and do_iac. They serve as both counters/indices
      and state variables.

      Interpretation as states:

        cnt_xxx == 0 and index_xxx == 0:  Normal state - no
                                          processing in progress

        cnt_xxx != 0:                     Lead-in found - copying
                                          to buffer
            (index_xxx is substate
            for setting length)

        cnt_xxx ==0  and index_xxx != 0:  Sequence complete -
                                          ready to exec
---------------------------------------------------------------------------*/

static unsigned int em_flag   = 0;  /*  EIA-617 sequence found             */
static unsigned int iac_flag  = 0;  /*  IS-99 IAC sequence found           */

static byte cp = '\0';              /*  Previous byte in EIA-617 scan      */


static int cnt_617;                 /*  Number of bytes yet to process in 
                                        current '617' message              */

static int index_617;               /*  Location in buffer for next '617' 
                                        byte.  Also a 'state variable' to
                                        control setting '617' block length */

static int cnt_iac;                 /*  Number of bytes yet to process in 
                                        current 'iac' message              */

static int index_iac;               /*  Location in buffer for next 'iac'
                                        byte.  Also a 'state variable' to
                                        control setting 'iac' block length */

static int sub_neg_iac;             /*  Flag that subnegotiation (IAC_SB) 
                                        is in progress                     */


/*-------------------------------------------------------------------------
  Local buffers for do_em and do_iac.
--------------------------------------------------------------------------*/
LOCAL byte buf_em  [MAX_LINE_SIZE];
LOCAL byte buf_iac [MAX_IAC];

/*-------------------------------------------------------------------------
  PPP event notification handle
--------------------------------------------------------------------------*/
static void *ppp_event_handle;

/*-------------------------------------------------------------------------
  Local Handle for Async class.
--------------------------------------------------------------------------*/
LOCAL int32 Async_handle;

/*---------------------------------------------------------------------------
  Data types needed to communicate with SIO.
---------------------------------------------------------------------------*/
static sio_ioctl_param_type ds707_async_sio_ioctl_param;

/*===========================================================================
                       INTERNAL MACRO DEFINITIONS
===========================================================================*/


/*===========================================================================
                       INTERNAL FUNCTION DECLARATIONS
===========================================================================*/
void ps707_async_ppp_lcp_cb
(
  ppp_dev_enum_type       ppp_dev,               /* Um/Rm/etc              */
  ppp_protocol_e_type     protocol,
  ppp_event_e_type        ppp_event,             /* Down/Up/Resync         */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
);

void ps707_async_ppp_ipcp_cb
(
  ppp_dev_enum_type       ppp_dev,               /* Um/Rm/etc              */
  ppp_protocol_e_type     protocol,
  ppp_event_e_type        ppp_event,             /* Down/Up/Resync         */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
);


/*===========================================================================
                       INTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION     PS707_ASYNCI_FORCE_CONNECT_FOR_FAX

DESCRIPTION  It is important to detect the beginning of a fax image, because
             the mobile needs to igore XON-XOFF characters in the middle of
             a fax image.  Usually, the mobile depends on CONNECT carried in
             617 messages to make this decision.  However, Japan IWFs do not
             implement reflection. So, this function goes through every byte
             received to look for CONNECT in plain text.
    
             These function must be deleted when the IWF is fixed to send
             EIA-617 messages, <EM><extend0><len><0x42><"AT+FDT"> following
             <EM><extend0><len><0x62><"CONNECT">.
    
             Note, this does not handle the case where "CONNECT" is split
             across dsm items, that are received from TCP.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
LOCAL void ps707_asynci_force_connect_for_fax
(
  dsm_item_type *in_item_ptr  /*  pointer to input item from protocol stack */
)
{
  int                               i;                    /* loop counter  */
  sio_ioctl_param_type              sio_ioctl_temp;
  int s3_val = 0;
  int s4_val = 0;
  #define CONNECT_MSG_LEN           (2+7+2)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_USED(in_item_ptr);
  DATA_MDM_ARG_NOT_CONST(in_item_ptr);

  s3_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  s4_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);

  /*  JCDMA IWF always issues verbal result code even though 'ATV0' command
  **  has been issued.
  **/
  if( (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_FCLASS_IDX,0,0,NUM_TYPE) != 0 
   && in_item_ptr->used >= CONNECT_MSG_LEN )
  {
    for( i = 0; i <= (in_item_ptr->used - CONNECT_MSG_LEN); i++ )
    {
      if( UPCASE( in_item_ptr->data_ptr[i+2] )  == 'C'
       && UPCASE( in_item_ptr->data_ptr[i+3] )  == 'O'
       && UPCASE( in_item_ptr->data_ptr[i+4] )  == 'N'
       && UPCASE( in_item_ptr->data_ptr[i+5] )  == 'N'
       && UPCASE( in_item_ptr->data_ptr[i+6] )  == 'E'
       && UPCASE( in_item_ptr->data_ptr[i+7] )  == 'C'
       && UPCASE( in_item_ptr->data_ptr[i+8] )  == 'T'
       && in_item_ptr->data_ptr[i]              == (byte) s3_val
       && in_item_ptr->data_ptr[i+1]            == (byte) s4_val
       && in_item_ptr->data_ptr[i+9]            == (byte) s3_val
       && in_item_ptr->data_ptr[i+10]           == (byte) s4_val
        )
      {
        ds3g_siolib_ioctl(SIO_IOCTL_BEGIN_FAX_IMAGE,
                          &sio_ioctl_temp);
        break;
      }
    }  /* for */
  }  /* if */
 
} /* ps707_asynci_force_connect_for_fax() */


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION      PS707_ASYNC_POWERUP_INIT

DESCRIPTION   Called once at mobile power-up.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ps707_async_powerup_init
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_mgr_powerup_init()");

  /*-------------------------------------------------------------------------
    Initialize state variables and flags.
  -------------------------------------------------------------------------*/
  ps707_async_state.protocol_state = PS707_ASYNC_PROTOCOL_CLOSED;
  ps707_async_state.mobile_originated = FALSE;
  ps707_async_state.ptcl_cleanup_timeout_handle= 0;

  (void) q_init (&( ps707_async_state.tcp_send_q ));

  ps707_async_state.tcp_send_q_cnt = 0;
  ps707_async_state.tcp_send_q_flow_enable = TRUE;

  rex_init_crit_sect (&ps707_async_data_from_tcp_upcall_crit_sect);

  /*-------------------------------------------------------------------------
    TCP connection related:
    For LtoM calls, tcp_open is passive. According to IS-707, mobile port
    number is 379 and remote port is between 0x1000 and 0xFFFF(assigned by
    IWF). Initially the port is set to 0, so that the dest port num can
    be read from the first TCP packet.
  -------------------------------------------------------------------------*/
  ps707_async_conn.remote.port  = 0;
  ps707_async_conn.local.port   = dss_htons ( RAWDATA_TCP_PASSIVE_PORT ) ;

  /*-------------------------------------------------------------------------
    Register 707 async-specific command handlers for PS task.
  -------------------------------------------------------------------------*/
  (void)ps_set_cmd_handler ( PS_707_ASYNC_TCP_CTL_OPEN_ACTIVE_CMD,
                       ps707_async_tcp_ctl_open_cmd_hdlr );

  (void)ps_set_cmd_handler ( PS_707_ASYNC_TCP_CTL_OPEN_PASSIVE_CMD,
                       ps707_async_tcp_ctl_open_cmd_hdlr );

  (void)ps_set_cmd_handler ( PS_707_ASYNC_PTCL_ABORT_CMD,
                       ps707_async_ptcl_abort_cmd_hdlr );

  (void)ps_set_cmd_handler ( PS_707_ASYNC_PTCL_CLOSE_CMD,
                       ps707_async_ptcl_close_cmd_hdlr );

  (void)ps_set_cmd_handler ( PS_707_ASYNC_PPP_DOWN_CMD,
                       ps707_async_ppp_down_cmd_hdlr );

  (void)ps_set_cmd_handler ( PS_707_ASYNC_PPP_UP_CMD,
                       ps707_async_ppp_up_cmd_hdlr );

  (void)ps_set_cmd_handler ( PS_707_ASYNC_PPP_RESYNC_CMD,
                       ps707_async_ppp_resync_cmd_hdlr );

  (void)ps_set_cmd_handler ( PS_707_ASYNC_UM_FLOW_ENABLE_CMD,
                       ps707_async_um_flow_enable_cmd_hdlr );

  /*-------------------------------------------------------------------------
    Register 707 async-specific signals for PS task.
  -------------------------------------------------------------------------*/
  (void)ps_set_sig_handler ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL,
                       ps707_async_process_send_q_sig_hdlr, 
                       NULL );

  /*-------------------------------------------------------------------------
    Allocate 707 async-specific timers for PS task.
  -------------------------------------------------------------------------*/
  /*lint -save -e641 conversion of enum to int*/
  ps707_async_state.ptcl_cleanup_timeout_handle = 
              ps_timer_alloc ( ps707_async_ptcl_cleanup_timeout_cb, NULL );
  /*lint -restore*/

  /*lint -save -e641 conversion from enum to int*/
  if ( ps707_async_state.ptcl_cleanup_timeout_handle == PS_TIMER_FAILURE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Timer allocation failed");
    ASSERT(0);
  } 
  /*lint -restore*/

} /* ps707_async_powerup_init() */ 
 
/*===========================================================================

FUNCTION      PS707_ASYNC_CALL_INIT

DESCRIPTION   This function does per call initialization:
              Initialize counters and indices for the EIA 617 and IAC escape
              sequence processing.  Initializes the FDT command seen flag
              for fax flow control.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ps707_async_call_init
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_call_init()");

  cnt_617   = 0;
  index_617 = 0;

  cnt_iac   = 0;
  index_iac = 0;

  sub_neg_iac = 0;

  cp = '\0';

  dsat707_fdt_seen = FALSE;

} /* ps707_async_call_init() */

/*===========================================================================

FUNCTION      PS707_ASYNC_IN_RAWDATA_MODE

DESCRIPTION   Returns True/False depending on whether a raw data call is in
              progress.

DEPENDENCIES  None.

RETURN VALUE  TRUE/FALSE 

SIDE EFFECTS  None.

===========================================================================*/
boolean ps707_async_in_rawdata_mode
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (ps707_async_state.protocol_state == PS707_ASYNC_PROTOCOL_CLOSED )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* ps707_async_in_rawdata_mode */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_TCP_CTL_OPEN_CMD_HDLR

DESCRIPTION   Called in PS TASK context in response to a  request by DS to
              open TCP (and the protocol stack) for async data transfer.

DEPENDENCIES  Called in PS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_tcp_ctl_open_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
)
{
  ppp_dev_opts_type ppp_um_options;     /* Options to configure PPP        */
  int ppp_status = 0;                   /* ppp return status               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MDM_ARG_NOT_CONST(user_data_ptr);

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_tcp_ctl_open_cmd_hdlr()");

  /*-------------------------------------------------------------------------
    Open TCP requested. If not now Opened then proceed to Open TCP.
  -------------------------------------------------------------------------*/
  if (ps707_async_state.protocol_state == PS707_ASYNC_PROTOCOL_CLOSED)
  {

    ps707_async_state.protocol_state = PS707_ASYNC_PROTOCOL_OPENING;
    
    /*-----------------------------------------------------------------------
      Active Open means mobile-originated. Passive open is answering
      a Page. Set ports according to IS-707 methods. Initiate PPP.
    -----------------------------------------------------------------------*/
    if( cmd == PS_707_ASYNC_TCP_CTL_OPEN_ACTIVE_CMD )
    {
      /*lint -save -e641 conversion from enum to int*/
      ps707_async_tcp_open_mode = TCP_ACTIVE;
      /*lint -restore*/
    }
    else  if ( cmd == PS_707_ASYNC_TCP_CTL_OPEN_PASSIVE_CMD )
    {
      /*lint -save -e641 conversion from enum to int*/
      ps707_async_tcp_open_mode = TCP_PASSIVE;
      /*lint -restore*/
    }
    else
    {
      ERR ("Unknown cmd", 0, 0, 0);
    }

    ps707_async_setup_tcp( ps707_async_tcp_open_mode ) ;

    
    /*-----------------------------------------------------------------------
      Configure PPP options and start PPP
    -----------------------------------------------------------------------*/
    ps707_async_config_ppp_um_options ( &ppp_um_options );

    if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
    {
      ds707_configure_VJ_compression_for_PPP(&ppp_um_options);
      ds707_configure_jcdma_params_for_ppp(&ppp_um_options);
    }
    /*Simplified JCDMA: 
      5.1.26 Enable VJ compression   [D1-No.78]*/
    else if(ds707_roaming_is_simplified_jcdma_mode())
    {
      ds707_configure_VJ_compression_for_PPP(&ppp_um_options);
    }

    if ( (ppp_status = ppp_start ( PPP_UM_SN_DEV, &ppp_um_options )) < 0 )
    {
       ERR ("ppp_start() returned %d", ppp_status, 0, 0 );
       ps707_async_cleanup_ptcl();
    }
  }
  else
  {
    ERR ( "TCP already open", 0, 0, 0);
  }
} /* ps707_async_tcp_ctl_open_cmd_hdlr() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_PTCL_ABORT_CMD_HDLR

DESCRIPTION   Called in PS TASK context in response to a  request by DS to
              abort protocols as call dropped abruptly.

DEPENDENCIES  Called in PS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ptcl_abort_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
)
{
  int ppp_status = 0;                   /* ppp return status               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* lint error handler */
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);
  DATA_MDM_ARG_NOT_USED(cmd);

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_ptcl_abort_cmd_hdlr()");

  /*-------------------------------------------------------------------------
    Abort PPP.
  ------------------------------------------------------------------------*/
  if ( (ppp_status = ppp_abort ( PPP_UM_SN_DEV )) < 0 )
  {
     DATA_IS707_MSG1(MSG_LEGACY_HIGH, "ppp_abort() returned %d", ppp_status);
     ps707_async_cleanup_ptcl();
     return;
  }

  ps_disable_sig ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL );
  if ((
       ps_timer_start ( ps707_async_state.ptcl_cleanup_timeout_handle,
                      (int64)PS707_ASYNC_PPP_CLOSE_INTERVAL )
      ) != PS_TIMER_SUCCESS )
  {
    ERR ("ps_timer_start returned fail", 0, 0, 0 );
  }

} /* ps707_async_ptcl_abort_cmd_hdlr() */ 

/*===========================================================================
FUNCTION      PS707_ASYNC_PTCL_CLOSE_CMD_HDLR

DESCRIPTION   Called in PS TASK context in response to a  request by DS to
              close protocols gracefully.

DEPENDENCIES  Called in PS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ptcl_close_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* lint error handler */
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);
  DATA_MDM_ARG_NOT_USED(cmd);

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_ptcl_close_cmd_hdlr()");

  /*-------------------------------------------------------------------------
    We check for the state to avoid a race condition - to avoid
    closing TCP while it is already closing. This race condition
    can happen for example in IS-99 when the IWF has closed TCP
    and at that time the TE2 drops DTR.
  -------------------------------------------------------------------------*/
  if (( ps707_async_state.protocol_state != PS707_ASYNC_PROTOCOL_CLOSING ) &&
      ( ps707_async_state.protocol_state != PS707_ASYNC_PROTOCOL_CLOSED  ) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,  "Closing TCP");
    ps_tcp_async_close_tcp(Async_handle);	
  }

  if ( ps707_async_state.protocol_state == PS707_ASYNC_PROTOCOL_OPEN )
  {
    ps707_async_state.protocol_state = PS707_ASYNC_PROTOCOL_CLOSING;

    ps_disable_sig ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL );

    if (
      (ps_timer_start ( ps707_async_state.ptcl_cleanup_timeout_handle,
                        (int64)PS707_ASYNC_CLOSE_CHK_INTERVAL )
      ) != PS_TIMER_SUCCESS )
    {
      ERR ("ps_timer_start returned fail", 0, 0, 0 );
    }
  }
} /* ps707_async_ptcl_close_cmd_hdlr() */ 

/*===========================================================================
FUNCTION      PS707_ASYNC_PPP_DOWN_CMD_HDLR

DESCRIPTION   This function takes action when PPP goes down.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ppp_down_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* lint error handler */
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);
  DATA_MDM_ARG_NOT_USED(cmd);

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_ppp_down_cmd_hdlr()");

  /*-------------------------------------------------------------------------
    Cancel timer and immediately close protocols.
  -------------------------------------------------------------------------*/
  if ( ps_timer_cancel ( ps707_async_state.ptcl_cleanup_timeout_handle )
       == PS_TIMER_FAILURE )
  {
    ERR ("ps_timer_cancel() failed", 0, 0, 0);
  }
  ps707_async_cleanup_ptcl();

} /* ps707_async_ppp_down_cmd_hdlr() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_PPP_UP_CMD_HDLR

DESCRIPTION   This function takes action when PPP comes up. It initiates the
              opening of TCP connection for Async/Fax service.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ppp_up_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
)
{
  uint16  mss = PS707_ASYNC_DEF_MSS;       /* TCP default MSS              */
  uint16  rwindow = (4 * PS707_ASYNC_DEF_MSS); /* TCP rx window size       */
  ps_ip_addr_type  addr;                   /* IP address                   */
  ds707_async_state_type*   ds707_async_state_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* lint error handler */
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);
  DATA_MDM_ARG_NOT_USED(cmd);

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_ppp_up_cmd_hdlr()");

  /*-------------------------------------------------------------------------
    Async/Fax mode. Initiate the opening of TCP, pull any app. data
    off holding queue and enqueue to Watermark.
  -------------------------------------------------------------------------*/
  if (ps707_async_state.protocol_state != PS707_ASYNC_PROTOCOL_OPENING)
  {
    /*-----------------------------------------------------------------------
      If TCP was already established and PPP resynchs,we just reset TCP.
      The reset function sends a TCP reset segment to other side, sets TCP
      state to CLOSED. This state change is processed by psmgr and we close
      PPP and bring down the call. 
    -----------------------------------------------------------------------*/
    ps_tcp_async_reset_tcp(Async_handle);
    /*-----------------------------------------------------------------------
      Send TCP reset to otherside - will close PPP and bring down call
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    " PPP resynced when IS-99 was in a call");
    return;
  }

  /*------------------------------------------------------------------------- 
    Get the pointer to ds707_async_state
  -------------------------------------------------------------------------*/
  ds707_async_state_ptr = ds707_get_async_state_ptr();

  ps_iface_up_ind ( &(ds707_async_state_ptr->ps_iface) );

  /*-------------------------------------------------------------------------
    Get the Service option & set the Receive window based on it for fax
    By default rwindow is set to 0 letting tcp to set it's default value
    When a non-zero value is specified, tcp uses that as the rwindow
  -------------------------------------------------------------------------*/
  if( DS_ISDATA_G3_FAX ( ds707_async_state_ptr->tc_state.so )  )
  {
    /*-----------------------------------------------------------------------
      For Fax calls, Advertise only a R Window of 3*MSS:
      To workaround the tcp delays due to IWF/Basestation issues
    -----------------------------------------------------------------------*/
    rwindow = 3*PS707_ASYNC_DEF_MSS;
    DATA_IS707_MSG1(MSG_LEGACY_LOW,  "Set rwin to 3*PS707_ASYNC_DEF_MSS, fax call So:%ld",
              ds707_async_state_ptr->tc_state.so);
  }
  else
  {
    /*-----------------------------------------------------------------------
      SO is not fax: Let the default window be set by tcp
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG1(MSG_LEGACY_LOW,  "Set rwin to 0, non fax call So:%ld ",
              ds707_async_state_ptr->tc_state.so);
  }

  ps707_async_state.protocol_state = PS707_ASYNC_PROTOCOL_OPEN;

  ps_enable_sig ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL );
      

  addr.type  = IPV4_ADDR;

  ps_iface_get_addr      ( &(ds707_async_state_ptr->ps_iface), &addr  );
  /*-------------------------------------------------------------------------
    ps_iface_get_addr returns the address in network order already.
  -------------------------------------------------------------------------*/
  if ( addr.type == IPV4_ADDR )
  {
    /*lint -save -e572 Excessive shift value */
    /*lint -save -e740 Macor from non-modem data module access*/
    PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&ps707_async_conn.local.address,
                                 addr.addr.v4.ps_s_addr);
    /*lint -restore*/
    /*lint -restore*/
  }
  else
  {
    ERR_FATAL( "PPP addr invalid", 0, 0, 0);
  }
  ps_iface_get_peer_addr ( &(ds707_async_state_ptr->ps_iface), &addr );
  /*-------------------------------------------------------------------------
    ps_iface_get_peer_addr returns the address in network order already.
  -------------------------------------------------------------------------*/
  if ( addr.type == IPV4_ADDR )
  {
    /*lint -save -e572 Excessive shift value*/
    /*lint -save -e740 Macor from non-modem data module access*/
    PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&ps707_async_conn.remote.address,
                                    addr.addr.v4.ps_s_addr);
    /*lint -restore*/
    /*lint -restore*/
  }
  else
  {
    ERR_FATAL( "PPP addr invalid", 0, 0, 0);
  }
  
  ps_tcp_async_tcp_open( Async_handle,&ps707_async_conn,
                         ps707_async_tcp_open_mode, 
                         rwindow,
                         mss );

  DATA_IS707_MSG0(MSG_LEGACY_HIGH,  "Opening TCP");

} /* ps707_async_ppp_up_cmd_hdlr() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_PPP_RESYNC_CMD_HDLR

DESCRIPTION   This function takes action when there is a PPP resync.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ppp_resync_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Lint error handler*/
  DATA_MDM_ARG_NOT_USED(cmd);
  DATA_MDM_ARG_NOT_USED(user_data_ptr);
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);




  /*-------------------------------------------------------------------------
    If PPP resyncs,we just reset TCP. The reset function sends a TCP reset
    segment to other side, sets TCP state to CLOSED. This state change is
    processed and we close PPP and bring down the call. 
  -------------------------------------------------------------------------*/
  ps_tcp_async_reset_tcp(Async_handle);

  DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ps707_async_ppp_resync_cmd_hdlr():"
                  " PPP resynced when in a call");

} /* ps707_async_ppp_resync_cmd_hdlr() */


/*===========================================================================
FUNCTION      PS707_ASYNC_UM_FLOW_ENABLE_CMD_HDLR

DESCRIPTION   This function takes action when SIO is ready to accept data
              from Um. Applicable for low-baud SIO that's slower than Um.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_um_flow_enable_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* lint error handler */
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);
  DATA_MDM_ARG_NOT_USED(cmd);

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_um_flow_enable_cmd_hdlr()");

  ps_tcp_async_recieve_data(Async_handle);

} /* ps707_async_um_flow_enable_cmd_hdlr() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_PROCESS_SEND_Q_SIG_HDLR

DESCRIPTION   This is the handler for PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL.

DEPENDENCIES  None.

RETURN VALUE  TRUE/FALSE depending on whether to relinquish control. We
              always return TRUE.

SIDE EFFECTS  None.
===========================================================================*/
boolean ps707_async_process_send_q_sig_hdlr
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr  /* Parameter carrying user data (TCP ack piggyback) */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Lint error handler */
  DATA_MDM_ARG_NOT_USED(sig);
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_process_send_q_sig_hdlr()");

  ps707_async_process_tcp_send_q ( FALSE );
  return (TRUE);

} /* ps707_async_process_send_q_sig_hdlr() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_PROCESS_TCP_SEND_Q

DESCRIPTION   This function processes all the items application layers
              want TCP to transmit.  The items sent by the serial port are
              escaped for 617 and Telnet characters, while the 617 cmds
              or responses are enqueued as is into the TCP send queue.
              The argument ack_piggyback means that this function is called
              when a TCP ack has to be sent. If this argument is TRUE, only
              one segment is transmitted, otherwise segments are transmitted
              till the tcp_send_q is empty.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_process_tcp_send_q
(
  boolean ack_piggyback                           /*If TCP ACK piggybacked */
)
{
  dsm_item_type *new_item_ptr = NULL;         /* Used in escaping to store */
                                              /* new item */
  dsm_item_type *temp_item_ptr =NULL;             /* Pointers for escaping */
  dsm_item_type *head_item_ptr = NULL;            /* Pointers for escaping */
  boolean copy_item = FALSE;           /* should the item be copied or not */
  dsm_item_type *item_ptr = NULL;            /* pointer to a DSM item type */
  uint16 i, item_lim ;                           /* Counter for loop */
  dsm_item_type *local_head_ptr = NULL;              /* head of local list */
  dsm_item_type *local_tail_ptr  = NULL;             /* tail of local list */
  word num_bytes_extracted =0;                     /* # of bytes extracted */
  dsm_watermark_type *ds707_wmk_um_SN_fwd_wm_list = NULL;

/*---------------------------------------------------------------------------
  If the number of bytes in a small item is less than the following threshold
  the data bytes of the small item will be copied to the TCP sendq tail.
---------------------------------------------------------------------------*/
#define TCP_SNDQ_SMALL_THRESH 5
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get pointer to ds707_wmk_um_SN_fwd_wm_list
  -------------------------------------------------------------------------*/
  ds707_wmk_um_SN_fwd_wm_list = ds707_get_wmk_um_SN_fwd_wm_list();

  /*-------------------------------------------------------------------------
    If no bytes to send, then it means we want to send an empty ack.
  -------------------------------------------------------------------------*/
  if ( 
       (ps707_async_state.tcp_send_q_cnt == 0) &&
       ( ack_piggyback == TRUE)
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, " Sending TCP ACK");
    ps_tcp_async_tcp_output(Async_handle);
    return;
  }

  /*-------------------------------------------------------------------------
    Loop through each item stored in the tcp_send_q.
  -------------------------------------------------------------------------*/
  while(( item_ptr = ps707_async_tcp_dequeue()) != NULL)
  {

    switch( item_ptr->kind)
    {

      case DSM_PS_RAW_APP_PAYLOAD:
          /*-----------------------------------------------------------------
            Payload received from the serial driver needs to be escaped.
          -----------------------------------------------------------------*/
          item_ptr->kind = DSM_PS_APP_PAYLOAD;

          /*-----------------------------------------------------------------
            Go through each byte in item to see if any bytes need to be
            escaped. Break out of the while loop, as soon as 1st byte that
            needs to be escaped is found.
          -----------------------------------------------------------------*/
          copy_item = FALSE;
          i=0;

          while ( i < item_ptr->used)
          {
            if ( item_ptr->data_ptr[i] == IS99_IAC)
            {
              copy_item = TRUE;
              break;
            }

            else if ( item_ptr->data_ptr[i] == EIA617_EM)
            {
              if ( ( i+1 < item_ptr->used) &&
                   (item_ptr->data_ptr[i+1] == EIA617_EM))
              {
                /*-----------------------------------------------------------
                  No need to break from loop if only a double 617 char. is
                  found. Since two bytes replace two others, no need to split
                  and copy items.
                -----------------------------------------------------------*/
                item_ptr->data_ptr[ i+1] = EIA617_DOUBLE_EM_M2B;
                i++;
              }
              else
              {
                copy_item = TRUE;
                break;
              }
            }
            i++;
          } /* while */

          /*-----------------------------------------------------------------
            If any bytes need to escaped, split up the item and start copying
            the rest of the bytes in the item to a new item.
          -----------------------------------------------------------------*/
          if ( copy_item == TRUE)
          {
            item_lim = i;
            new_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL);
            if (new_item_ptr == NULL)
            {
              dsm_free_packet(&item_ptr);
              dsm_free_packet(&temp_item_ptr);
              dsm_free_packet(&local_head_ptr);
              dsm_free_packet(&head_item_ptr);
              dsm_free_packet(&local_tail_ptr);
              DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Running out of buffers");
              return;
            }

            new_item_ptr->kind = DSM_PS_APP_PAYLOAD;

            head_item_ptr = new_item_ptr;

            /*---------------------------------------------------------------
              Loop through rest of item - copying each byte to the new item
              and escaping as necessary.
            ---------------------------------------------------------------*/
            while ( i < item_ptr->used)
            {
              if ( new_item_ptr->used + 1 >= new_item_ptr->size)
              {
                 temp_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL);
                 if (temp_item_ptr == NULL)
                 {
                   dsm_free_packet(&item_ptr);
                   dsm_free_packet(&new_item_ptr);
                   dsm_free_packet(&local_head_ptr);
                   dsm_free_packet(&local_tail_ptr);
                   dsm_free_packet(&head_item_ptr);
                   DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Running out of buffers");
                   return;
                 }
                 temp_item_ptr->kind = DSM_PS_APP_PAYLOAD;

                 new_item_ptr->pkt_ptr = temp_item_ptr;
                 new_item_ptr = temp_item_ptr;
              }

              new_item_ptr->data_ptr[ new_item_ptr->used++]
                                                  = item_ptr->data_ptr[ i];

              if ( item_ptr->data_ptr[i] == IS99_IAC)
              {
                 new_item_ptr->data_ptr[ new_item_ptr->used++] = IS99_IAC;
              }
              else if ( item_ptr->data_ptr[i] == EIA617_EM)
              {
                if ( ( i+1 < item_ptr->used) &&
                    (item_ptr->data_ptr[i+1] == EIA617_EM))
                {
                  new_item_ptr->data_ptr[ new_item_ptr->used++]
                                                 = EIA617_DOUBLE_EM_M2B;
                  i++;
                }
                else
                {
                  new_item_ptr->data_ptr[ new_item_ptr->used++]
                                                 = EIA617_SINGLE_EM_M2B;
                }
              }
              i++;
             }  /* while loop through item */

           /*---------------------------------------------------------------
             Now link up the items to form a packet chain.
           ---------------------------------------------------------------*/
           if ( item_lim != 0)
           {
             item_ptr->used    = item_lim;
             item_ptr->pkt_ptr = head_item_ptr;
           }
           else
           {
             (void)dsm_free_buffer( item_ptr);
             item_ptr = head_item_ptr;
           }
          } /* if copying the item */

        //lint -fallthrough
       /* fallthru, fallthru */


     case DSM_PS_APP_PAYLOAD:
       /*-------------------------------------------------------------------
         Payload data for TCP TX. If TCP is Open then give the data to
         the protocol for TXing.
       -------------------------------------------------------------------*/
       if ( local_head_ptr == NULL)
       {
         local_head_ptr = item_ptr;
         local_tail_ptr = item_ptr;
         num_bytes_extracted += local_tail_ptr->used;
         item_ptr       = item_ptr->pkt_ptr;
       }

        /*-------------------------------------------------------------------
          Now, go through each item in the new_item_ptr packet chain. If the
          item has less than five bytes, and the there is room to copy, then
          copy the item to the tail. Otherwise, just append to the sendq.
          Note, the copy and free logic was added to conserve the number of
          small items.
        -------------------------------------------------------------------*/

       while ( item_ptr != NULL)
        {

          ASSERT( item_ptr->used <= item_ptr->size);
          temp_item_ptr = item_ptr->pkt_ptr;

          /*----------------------------------------------------------------
            Check if the new_item can be copied to the sendq tail. If so,
            copy the data and then free new_item_ptr.
          -----------------------------------------------------------------*/
          if ((item_ptr->used <= TCP_SNDQ_SMALL_THRESH) &&
              (local_tail_ptr->size - local_tail_ptr->used >=item_ptr->used))
          {
            memscpy(local_tail_ptr->data_ptr+local_tail_ptr->used,
                    item_ptr->used, item_ptr->data_ptr, item_ptr->used);
            local_tail_ptr->used += item_ptr->used;
            num_bytes_extracted  += item_ptr->used;

            /*---------------------------------------------------------------
              Switch the packet pointers of the item being copied. Note, this
              copying ensures that tcb->sndq does not point to an item that
              has been freed.
            ---------------------------------------------------------------*/
            local_tail_ptr->pkt_ptr = item_ptr->pkt_ptr;

            (void)dsm_free_buffer( item_ptr);
          }
          else
          {
            /*--------------------------------------------------------------
              The new item cannot be copied, so just link it to the sendq
              tail ptr.
            --------------------------------------------------------------*/
            local_tail_ptr->pkt_ptr  = item_ptr;
            local_tail_ptr           = item_ptr;
            num_bytes_extracted += local_tail_ptr->used;
          }

          item_ptr = temp_item_ptr;

        } /* while */

       break;

       default:
         ERR_FATAL( "Invalid item type" ,0, 0, 0);

    } /* switch */

    /*-----------------------------------------------------------------------
      If more than DEF_MSS bytes are extracted, send them out on TCP.
      If this is data to be piggybacked on an ack, then exit because we only
      want to send one segment at a time.
      If this is not a piggybacked ack, keep dequeueing items and send on
      TCP.
    -----------------------------------------------------------------------*/
    if ( ( num_bytes_extracted +  ps_tcp_async_outstanding_data() )
        >= PS707_ASYNC_DEF_MSS
       )
    {
      if ( ps707_async_state.protocol_state == PS707_ASYNC_PROTOCOL_OPEN )
      {
        if( ps_tcp_async_send_data( Async_handle, local_head_ptr ) < 0)
        {
          ERR_FATAL("TCP Send Error", 0, 0, 0);
        }
        /*-------------------------------------------------------------------
          If this is either a piggybacked ack or there is more ppp data
          available then exit this function. This function will be called
          to send more TCP acks later.
          Otherwise extract more bytes from the tcp_send_q.
        -------------------------------------------------------------------*/
        if ( 
              ( ack_piggyback == TRUE) 
           || ( !dsm_is_wm_empty ( ds707_wmk_um_SN_fwd_wm_list+DSRSP_ROUTE_A ) )
           )
        {
          return;
        }
        else
        {
          num_bytes_extracted =0;
          local_head_ptr = (local_tail_ptr  = NULL);
        }
      }
      else
      {
        dsm_free_packet( &local_head_ptr);
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Out-of-Seq: TCP Closed");
      }
    }  /* if num_bytes_extracted >= PS707_ASYNC_DEF_MSS */

  } /* while */

  /*-------------------------------------------------------------------------
    Check to ensure something was extracted before sending to send_tcp.
  -------------------------------------------------------------------------*/
  if ( local_head_ptr == NULL)
  {
    return;
  }

  if ( ps707_async_state.protocol_state == PS707_ASYNC_PROTOCOL_OPEN )
  {
    if( ps_tcp_async_send_data( Async_handle, local_head_ptr) < 0)
    {
      ERR_FATAL("TCP Send Error", 0, 0, 0);
    }
  }
  else
  {
    dsm_free_packet( &local_head_ptr);
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Out-of-Seq: TCP Closed");
  }

  return; 

} /* ps707_async_process_tcp_send_q() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_PTCL_CLEANUP_TIMEOUT_CB

DESCRIPTION   Callback when timer expires for protocol cleanup.  By this
              time if TCP hasn't shut down gracefully, calls function to
              forcibly cleanup things.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ptcl_cleanup_timeout_cb
(
  void *user_data_ptr                                          /* unused   */
)
{
  int ppp_status = 0;                   /* ppp return status               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* lint error handler */
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);

  DATA_IS707_MSG0(MSG_LEGACY_MED,  "ps707_async_ptcl_cleanup_timeout_cb()");

  if ( ps_timer_cancel ( ps707_async_state.ptcl_cleanup_timeout_handle )
       == PS_TIMER_FAILURE )
  {
    ERR ( "ps_timer_cancel() failed", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Abort PPP.
  ------------------------------------------------------------------------*/
  if ( (ppp_status = ppp_abort ( PPP_UM_SN_DEV )) < 0 )
  {
     DATA_IS707_MSG1(MSG_LEGACY_HIGH, "ppp_abort() returned %d", ppp_status);
     ps707_async_cleanup_ptcl();
     return;
  }

} /* ps707_async_ptcl_cleanup_timeout_cb() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_PPP_LCP_CB

DESCRIPTION   Callback registered with PPP to indicate LCP events. 
              Although this is executing in PS context, it is
              still recommended to send a cmd to PS to do further processing

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ppp_lcp_cb
(
  ppp_dev_enum_type       ppp_dev,               /* Um/Rm/etc              */
  ppp_protocol_e_type     protocol,
  ppp_event_e_type        ppp_event,             /* Down/Up/Resync         */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Lint error handler */
  DATA_MDM_ARG_NOT_USED(fail_reason);
  DATA_MDM_ARG_NOT_USED(session_handle);
  DATA_MDM_ARG_NOT_USED(user_data);
  DATA_MDM_ARG_NOT_CONST(user_data);



  if (    ( protocol  == PPP_PROTOCOL_LCP ) 
       && ( ppp_dev   == PPP_UM_SN_DEV    ) 
       && ( ppp_event == PPP_DOWN_EV      )  )
  {
    ps_send_cmd ( PS_707_ASYNC_PPP_DOWN_CMD, NULL );
  }
  else
  {
    ERR ("Wrong PPP protocol, event or device", 0, 0, 0 );
  }
} /* ps707_async_ppp_lcp_cb() */

/*===========================================================================
FUNCTION      PS707_ASYNC_PPP_IPCP_CB

DESCRIPTION   Callback registered with PPP to indicate IPCP events.
              Although this is executing in PS context, it is
              still recommended to send a cmd to PS to do further processing

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ppp_ipcp_cb
(
  ppp_dev_enum_type       ppp_dev,               /* Um/Rm/etc              */
  ppp_protocol_e_type     protocol,
  ppp_event_e_type        ppp_event,             /* Down/Up/Resync         */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Lint error handler */
  DATA_MDM_ARG_NOT_USED(fail_reason);
  DATA_MDM_ARG_NOT_USED(session_handle);
  DATA_MDM_ARG_NOT_USED(user_data);
  DATA_MDM_ARG_NOT_CONST(user_data);


  if (    ( protocol   == PPP_PROTOCOL_IPCP )
       && ( ppp_dev    == PPP_UM_SN_DEV     )
       && ( ppp_event  == PPP_UP_EV ||
            ppp_event  == PPP_RESYNC_EV     ) )
  {
    if( ppp_event == PPP_UP_EV )
    {
      ps_send_cmd ( PS_707_ASYNC_PPP_UP_CMD, NULL );
    }
    else
    {
      ps_send_cmd ( PS_707_ASYNC_PPP_RESYNC_CMD, NULL );
    }
  }
  else
  {
    ERR ("Wrong PPP event or device", 0, 0, 0 );
  }

} /* ps707_async_ppp_ipcp_cb() */
 
/*===========================================================================
FUNCTION      PS707_ASYNC_DATA_FROM_TCP_UPCALL

DESCRIPTION   This function will be invoked by the TCP input routine upon
              completion of TCP processing of received payload data bytes.
              This function will send the TCP payload for 617 and telnet
              processing and then output to the serial port.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_data_from_tcp_upcall
(
  dsm_item_type *dsmItemPtr,    /* List head pointer to payload data        */
  uint16        cnt             /* The number of data payload data bytes    */
)
{
  dsm_item_type *get_ptr;      /* pointer for grabbing App data items      */
  sint15        remove_cnt;    /* down counter for removing bytes          */
  ds707_async_state_type*   ds707_async_state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Save original count of data bytes then Queue data bytes to DS from RX
    queue as long as the TX semaphore is set.
  -------------------------------------------------------------------------*/
  remove_cnt = (sint15)cnt;
   
  ds707_async_state = ds707_get_async_state_ptr();

  while (( ds707_async_state->um_flow_enable == TRUE ) && ( remove_cnt > 0 ))
  {
    rex_enter_crit_sect(&ps707_async_data_from_tcp_upcall_crit_sect);
    get_ptr           = dsmItemPtr;
    dsmItemPtr        = get_ptr->pkt_ptr;
    get_ptr->pkt_ptr  = NULL;
    dsmItemPtr       -= get_ptr->used;
    remove_cnt       -= (sint15)get_ptr->used;
    rex_leave_crit_sect(&ps707_async_data_from_tcp_upcall_crit_sect);
    get_ptr->kind     = DSM_PS_APP_PAYLOAD;
  /*-------------------------------------------------------------------------
    Invoke function for command reflection processing and queuing
    of Serial TX data. Note, we call this function within
    the ps task, instead of queueing up data for ds task to do it.
   ------------------------------------------------------------------------*/
    ps707_async_process_data_from_um( get_ptr );
  } /* while */

} /* ps707_async_data_from_tcp_upcall() */ 

/*===========================================================================
FUNCTION      PS707_ASYNC_TCP_STATE_CHANGE_UPCALL_FUNC

DESCRIPTION   This function will be invoked by the TCP input routine when
              the TCP state changes. This function will use the old and the
              new TCP states to determine the processing requirements.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_tcp_state_change_upcall_func
(
  async_tcp_state_async_type state
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
  int ppp_status = 0;                   /* ppp return status               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Do processing for relevent TCP state changes only.
  -------------------------------------------------------------------------*/
  switch( state)
  {
    case ASYNC_TCP_ESTABLISHED:
      /*---------------------------------------------------------------------
        TCP now Established. Inform DS task that TCP ready to send data.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,  "TCP Established");
      ps707_async_state.protocol_state = PS707_ASYNC_PROTOCOL_OPEN;


      /*---------------------------------------------------------------------
        Inform dsmgr that protocols are now open.
      ---------------------------------------------------------------------*/
      if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
      {
        ASSERT(0);
        return;
      }
      else
      {
        cmd_ptr->hdr.cmd_id = DS_CMD_707_ASYNC_PTCL_OPENED;
        ds_put_cmd(cmd_ptr);
      }
      break;

    case ASYNC_TCP_CLOSE_WAIT:
      /*---------------------------------------------------------------------
        TCP now in Closed Wait State.(other side will not send any more data)
        This is the IS-707 method to end a call. Inform DS task of Close then
        proceed to Close TCP connection. Set State timer to clean all protocol
        stuff up in the event the closing process doesn't complete correctly.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,  "TCP Closing");
	  
      ps_tcp_async_close_tcp(Async_handle);

      ps707_async_state.protocol_state = PS707_ASYNC_PROTOCOL_CLOSING;

      ps_disable_sig ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL );
      if (
          ( ps_timer_start ( ps707_async_state.ptcl_cleanup_timeout_handle,
                           (int64)PS707_ASYNC_PPP_CLOSE_INTERVAL ))
          != PS_TIMER_SUCCESS
         )
      {
        ERR ("ps_timer_start returned fail", 0, 0, 0 );
      }
      break;

    case ASYNC_TCP_CLOSED:
      /*---------------------------------------------------------------------
        TCP now fully Closed. Initiate closing of PPP then start timer used
        to check that the Close completed properly.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,  "TCP Fully Closed, Closing PPP");

      if ( (ppp_status = ppp_stop ( PPP_UM_SN_DEV, PPP_PROTOCOL_IPCP )) < 0 )
      {
        ERR ("ppp_stop() returned %d", ppp_status, 0, 0 );
        ps707_async_cleanup_ptcl();
        return;
      }

      ps707_async_state.protocol_state = PS707_ASYNC_PROTOCOL_CLOSING;

      /*---------------------------------------------------------------------
        Start close timer again, because state could change to closed from
        close_wait (if TCP was set up) or from syn_sent ( if TCP weren't
        set up. So, the timer is just bumped up again.
      ---------------------------------------------------------------------*/
      ps_disable_sig ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL );
      if (
          ( ps_timer_cancel( ps707_async_state.ptcl_cleanup_timeout_handle ))
          != PS_TIMER_SUCCESS
         )
      {
        ERR ("ps_timer_cancel returned fail", 0, 0, 0 );
      }
      if (
          ( ps_timer_start ( ps707_async_state.ptcl_cleanup_timeout_handle,
                           (int64)PS707_ASYNC_PPP_CLOSE_INTERVAL ))
            != PS_TIMER_SUCCESS 
         )
      {
        ERR ("ps_timer_start returned fail", 0, 0, 0 );
      }
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_MED,  "TCP State change, now: %d", state);
  }
} /* ps707_async_tcp_state_change_upcall_func() */ 
 
/*===========================================================================
FUNCTION      TCP_TX_WINDOW_CHANGE

DESCRIPTION   This function will be invoked by the TCP input routine when
              the TCP TX window changes to allow more bytes to be sent. This
              function will determine if the Lowater mark has been reached
              for the appropriate Watermark item and invoke the Lowater
              function if appropriate.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_tcp_tx_window_change(void)
{

  /*-------------------------------------------------------------------------
    Invoke Lo water function if the TCP TX unacknowledged byte count is below
    mark.
    NOTE: The passed 'snd_q_depth' parameter is not used. The
    'snd.nxt-snd.una' is a better measure of the Unacknowleged byte count in
    the TCP Send Q.
  -------------------------------------------------------------------------*/
   if ( ( ps707_async_state.tcp_send_q_flow_enable == FALSE) 
        && ( (ps707_async_state.tcp_send_q_cnt +  ps_tcp_async_outstanding_data())
             < 2*PS707_ASYNC_DEF_MSS )
      )
   {
     ps707_async_state.tcp_send_q_flow_enable = TRUE;
     ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_TCP_MASK, DS_FLOW_ENABLE);
   }
} /* ps707_async_tcp_tx_window_change() */ 
 

/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      PS707_ASYNC_CONFIG_PPP_UM_OPTIONS

DESCRIPTION   This function configures PPP options for Um link.

DEPENDENCIES  Assumes default options are already present in options 
              parameter passed in.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_config_ppp_um_options 
( 
  ppp_dev_opts_type *ppp_opts_ptr        /* Ptr to struct to be filled with
                                            ppp options                    */
)
{
  ds707_async_state_type*   ds707_async_state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_config_ppp_um_options()");

  ds707_async_state = ds707_get_async_state_ptr();
  ppp_get_default_opts ( ppp_opts_ptr );

  (ppp_opts_ptr->lcp_info).will_mask[FSM_LOCAL]  &= ~LCP_N_AP;
  (ppp_opts_ptr->lcp_info).will_mask[FSM_REMOTE] &= ~LCP_N_AP;
  (ppp_opts_ptr->lcp_info).want_mask[FSM_LOCAL]  &= ~LCP_N_AP;
  (ppp_opts_ptr->lcp_info).want_mask[FSM_REMOTE] &= ~LCP_N_AP;
  (ppp_opts_ptr->lcp_info).mode = PPP_CP_ACTIVE ;
  
  (ppp_opts_ptr->ipcp_info).will_mask[FSM_LOCAL]  &= ~IPCP_N_PRIMARY_DNS;
  (ppp_opts_ptr->ipcp_info).will_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_DNS;
  (ppp_opts_ptr->ipcp_info).want_mask[FSM_LOCAL]  &= ~IPCP_N_PRIMARY_DNS;
  (ppp_opts_ptr->ipcp_info).want_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_DNS;
  (ppp_opts_ptr->ipcp_info).will_mask[FSM_LOCAL]  &= ~IPCP_N_SECONDARY_DNS;
  (ppp_opts_ptr->ipcp_info).will_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_DNS;
  (ppp_opts_ptr->ipcp_info).want_mask[FSM_LOCAL]  &= ~IPCP_N_SECONDARY_DNS;
  (ppp_opts_ptr->ipcp_info).want_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_DNS;
  (ppp_opts_ptr->ipcp_info).mode = PPP_CP_ACTIVE ;

  ppp_opts_ptr->tx_f_ptr                   = ds707_async_tx_um_SN_data;
  ppp_opts_ptr->rx_f_ptr                   = ds707_async_rx_um_SN_data;
  ppp_opts_ptr->ipcp_info.rx_iface_ptr     = &(ds707_async_state->ps_iface);

  ppp_opts_ptr->hdlc_mode = HDLC_SW;

  if( NULL == ppp_event_handle )
  {
    ppp_event_handle = ppp_event_alloc( PPP_UM_SN_DEV,
                                        ps707_async_ppp_lcp_cb,
                                        NULL,
                                        NULL,
                                        NULL,
                                        ps707_async_ppp_ipcp_cb,
                                        NULL,
                                        NULL,
                                        NULL );
  }

  if( NULL == ppp_event_handle )
  {
    ERR_FATAL( "Unable to acquire a PPP event handle", 0, 0, 0 );
  }
  else
  {
    if( 0 > ppp_event_register( ppp_event_handle,
                                PPP_PROTOCOL_LCP,
                                PPP_DOWN_EV_MASK ) )
    {
      ERR_FATAL ("ppp_event_register returned err", 0, 0, 0 );
    }
    if( 0 > ppp_event_register( ppp_event_handle,
                                PPP_PROTOCOL_IPCP,
                                PPP_UP_EV_MASK ) )
    {
      ERR_FATAL ("ppp_event_register returned err", 0, 0, 0 );
    }
    if( 0 > ppp_event_register( ppp_event_handle,
                                PPP_PROTOCOL_IPCP,
                                PPP_RESYNC_EV_MASK ) )
    {
      ERR_FATAL ("ppp_event_register returned err", 0, 0, 0 );
    }
  }

} /* ps707_async_config_ppp_um_options() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_SETUP_TCP

DESCRIPTION   This function will prepare the TCP and PPP data structures for
              operation.  This includes setting proper TCP port numbers and
              initiating the TCP open.

DEPENDENCIES  This function must be called to initiate protocol operation
              for each data call.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_setup_tcp
(
  word tcp_open_mode           /* type for TCP Open, either Active or Open */
)
{
   ps_async_func_tbl_type  func_table;
   ds707_async_state_type*   ds707_async_state = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  /*-------------------------------------------------------------------------
    Use the passed Open type to determine the port numbers.
  -------------------------------------------------------------------------*/
  /*lint -save -e641 conversion from enum to int*/
  if( tcp_open_mode == TCP_PASSIVE )
  {
    /*-----------------------------------------------------------------------
      For LtoM calls, tcp_open is passive. According to IS-707, mobile port
      number is 379 and remote port is between 0x1000 and 0xFFFF(assigned by
      IWF). Initially the port is set to 0, so that the dest port num can
      be read from the first TCP packet.
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_LOW,  "Setting up TCP Open Passive");
    ps707_async_conn.remote.port  = 0;
    ps707_async_conn.local.port   = dss_htons ( RAWDATA_TCP_PASSIVE_PORT );
    ps707_async_state.mobile_originated = FALSE;
  }/*lint -restore*/
  else 
  {
    /*-----------------------------------------------------------------------
      For MtoL calls, tcp_open is active. According to IS-707, remote port
      number is 380.
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_LOW,  "Setting up TCP Open Active");
    ps707_async_conn.remote.port  = dss_htons ( RAWDATA_TCP_ACTIVE_PORT );
    ps707_async_conn.local.port   = dss_htons ( ps707_async_local_port  );
    ps707_async_state.mobile_originated = TRUE;
  }


  func_table.r_upcall = ps707_async_data_from_tcp_upcall;
  func_table.s_upcall = ps707_async_tcp_state_change_upcall_func;
  func_table.t_upcall = ps707_async_tcp_tx_window_change;

  ds707_async_state = ds707_get_async_state_ptr();

  Async_handle = ps_tcp_async_create_handle( &func_table, 
  	                                         &(ds707_async_state->ps_iface) );
  if ( Async_handle < 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, " TCP create handle failed");
  }
} /* ps707_async_setup_tcp() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_TCP_DEQUEUE

DESCRIPTION   This function removes the first item off the
              tcp_send_q and passes it back to the calling
              function. It also adjusts the byte count of the queue.
              The returned item is a single item and not a packet chain.

DEPENDENCIES  None.

RETURN VALUE  Pointer to item just dequeued.

SIDE EFFECTS  Changes tcp_send_q and tcp_send_q_cnt.
===========================================================================*/
dsm_item_type * ps707_async_tcp_dequeue
(
void
)
{
  dsm_item_type* item_ptr;                          /* Holds dequeued item */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the first item off the queue.
  -------------------------------------------------------------------------*/
   item_ptr = (dsm_item_type *)q_get( &ps707_async_state.tcp_send_q);

  /*-------------------------------------------------------------------------
     Decrement the current queue count.
  -------------------------------------------------------------------------*/
  if ( item_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Enter critical section to protect TCP send queue.
    -----------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&ds707_async_global_crit_sect);

    ps707_async_state.tcp_send_q_cnt -= item_ptr->used;

    /*-----------------------------------------------------------------------
      Leave critical section.
    -----------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&ds707_async_global_crit_sect);
  }

  return( item_ptr);

} /* ps707_async_tcp_dequeue() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_PROCESS_DATA_FROM_UM

DESCRIPTION   This function reads the data stream from the protocol stack.

              It strips off the <IAC> compression negotiation codes and
              transparency sequences.  Responses to the compression
              negotiation are generated, and sent back to the base station
              as <IAC> sequences via dsm_enqueue.  The <IAC> sequence will
              be presented in its entirety to dsm_enqueue.  If enabled by
              the AT+CDR command, an unsolicited status message will be sent
              to the user.

              It then strips out all EIA-617 <EM> in-band signals, and <EM>
              transparency codes. The reflected AT codes transmitted as
              EIA-617 in-band signals are use to call ds_atcop
              (DS_ATCOP_ONLINE). Any spaces remain.

              EIA-617 modem pin status signals are transmitted to the SIO
              driver via the sio_xxx_procedure calls.

              Response messages are removed from the EIA-617 STATUS messages,
              and passed to the serial port.

              The remainder of the data stream is passed to the serial port
              for transmission to the TE2.

DEPENDENCIES  The "AT" characters and the end of line character are assumed
              not to be present in the reflected AT codes carried in in-band
              signals.

              The necessary transparency codes are assumed to have been
              imbedded at the base station.

              'cnt_617', 'index_617', 'cnt_iac', 'index_iac', and
              'sub_neg_iac' must be set to 0  before starting to process
              the input data stream'.This is done by ps707_async_call_init().

              The output items used by do_em and do_iac may be longer than
              the input item.  This is safe for the following reasons:

              The output items are modified by applying DSM_ITEM_HEAD which
              increases its size by the size of the TCP header and footer.

              The maximum size gain in do_iac is 14 bytes. The maximum
              size gain in do_em is 99 bytes.  (See the discussion of
              the individual functions.)

              NB: As long as DSM_ITEM_HEAD increases the available space
                  in the output item by more than the sum of the maximum
                  expansion in do_em and do_iac (113 bytes),  and
                  as long as the header area has _not_ been used in the
                  input item received from the protocol stack,
                  it is safe to allow the output items to expand.

              Both do_em and do_iac terminate when the output exceeds the
              basic size of a large item, even if there is still input to
              process.  They set em_flag or iac_flag so that processing of
              the input item will be resumed after the output item has been
              processed.

RETURN VALUE  None.

SIDE EFFECTS  No side effects except thru call to ds_atcop.
===========================================================================*/
void  ps707_async_process_data_from_um
(
  dsm_item_type *in_item_ptr  /* pointer to input item from protocol stack */
)
{
  uint16 i;
  uint16 j;
  byte c;
  dsm_item_type *out_item_ptr, *next_item_ptr;
  dsm_item_type *dup_item_ptr;
/*  uint16 tmp_len = 0; */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


 /*--------------------------------------------------------------------------
  The design of this function is based on the idea that not all input
  items will contain in-band signals or transparency codes.  To avoid
  unnecessary string copying, each input item is scanned for the in-band
  lead-in characters. If neither lead-in character is found, then the
  data item is simply passed to the output without copying.

  If the IAC lead-in character is found, do_iac () is executed.  The
  IAC signals and transparency codes stripped out and the codes executed.
  Any remaining data is output in a new data item.

  If the EM lead-in character is found, it is processed in the same manner
  using do_em ().

  After the in-band processing, if there is any data left in the main data
  stream, it is output to ds3g_siolib_setup_tx().
 --------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Do a quick scan for in-band signals.
    If none are present just pass on the watermark.
  -------------------------------------------------------------------------*/
  if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
  
    /*-------------------------------------------------------------------------
      Search for "CONNECT" as per JCDMA spec
    -------------------------------------------------------------------------*/
    ps707_asynci_force_connect_for_fax(in_item_ptr);
  }

  /*-------------------------------------------------------------------------
    If neither EIA617 or IS707 message processing is in progress
    from a previous data item, then scan and process transparency in
    place, else just scan and set flags for full processing.

    The index into the item, 'i', propagates from loop to loop, so
    it is initialized here.
  -------------------------------------------------------------------------*/
  i = 0;
  if (! (em_flag  || iac_flag))
  {
    /*-----------------------------------------------------------------------
      Quickly scan for either the EIA617_EM character, or the
      IS707IAC character.  If either is found, drop out of the loop,
      without incrementing the index, so that it points to the
      escape character just found.

      If an escape code is pending, as indicated by cp != '\0',
      the loop is skipped.
    -----------------------------------------------------------------------*/
    if (cp == '\0')
    {
      while (i < in_item_ptr->used)
      {
        c = in_item_ptr->data_ptr[i];
        if (c == EIA617_EM || c == IS99_IAC)
        {
          break;
        }
        ++i;
      }
    }
    /*-----------------------------------------------------------------------
      This loop starts where the previous one left off, or is
      skipped over if the previous one exhausted the data item.

      Transparency codes are processed in place.  The index 'j'
      is used to write data back to the same data item, in order to
      fill in the holes left by the removal of the transparency codes.

      If a code is found that is not a transparency code, then
      the data item is 'duped'.  That part of the data item which
      preceded the escape code is sent to sio.  The rest is sent to
      the message processing functions:  do_iac () and do_em ().

      Note that the case of an escaped <EM><EM> (which becomes
      <EM><EIA617_DOUBLE_EM_B2M>) is also treated as though it is not
      a transparency in the special case in which the <EM> is the
      last byte of one data item and the <EIA617_DOUBLE_EM_B2M>
      is the first byte of the next.  In this case, the two <EM>'s
      cannot be written to the data item in place. do_em() is called
      instead, to process the result into a new data item.  This
      causes minor loss of efficiency.
    -----------------------------------------------------------------------*/

    j = i;
    while (i < in_item_ptr->used)
    {
      c = in_item_ptr->data_ptr[i++];

      if (cp == EIA617_EM)                         /*  Previous was escape */
      {
        cp = '\0';                                 /*  Thru with prev      */

        if (c == EIA617_SINGLE_EM_B2M)
        {
          in_item_ptr->data_ptr[j++] = EIA617_EM;
        }
        else if (c == EIA617_DOUBLE_EM_B2M && i!= 1)
        {
          in_item_ptr->data_ptr[j++] = EIA617_EM;
          in_item_ptr->data_ptr[j++] = EIA617_EM;
        }
        else
        {
          /*-----------------------------------------------------------------
            If there was data before escape, dup and reset the pointers.
          -----------------------------------------------------------------*/
          if (j > 0)
          {
            (void)dsm_dup_packet (&dup_item_ptr, in_item_ptr, 0, j);
            (void)ds3g_siolib_setup_tx ( dup_item_ptr, FALSE );
            in_item_ptr->data_ptr += (i - 2);
            in_item_ptr->used     -= (i - 2);
            i = 0;                 /*  Reset since start moved             */
          }
          /*-----------------------------------------------------------------
            EM was in previous item. Force do_em to start without seeing EM
            initially, just get one more byte and put at start of the buffer.
          -----------------------------------------------------------------*/
          else if (i == 1)
          {
            cnt_617   = 1;
            index_617 = 0;
          }

          em_flag = 1;           /*  Flag item active                      */
          break;                 /*  EXIT LOOP                             */
        }
      } /*  if (cp == EIA617_EM)  */

      /*---------------------------------------------------------------------
        Previous was escape.
      ---------------------------------------------------------------------*/
      else if (cp == IS99_IAC)
      {
        cp = '\0';                                 /*  Thru with prev      */
        if (c == IS99_IAC)
        {
          in_item_ptr->data_ptr[j++] = IS99_IAC;
        }
        else
        {
           /*----------------------------------------------------------------
             If there was data before escape, dup and reset the
             pointers.
           ----------------------------------------------------------------*/
           if (j > 0)
           {
             (void)dsm_dup_packet (&dup_item_ptr, in_item_ptr, 0, j);
             (void)ds3g_siolib_setup_tx ( dup_item_ptr, FALSE );
             in_item_ptr->data_ptr += (i - 2);
             in_item_ptr->used     -= (i - 2);
             i = 0;              /*  Reset since start moved               */
           }
           /*----------------------------------------------------------------
             IAC was in previous item. Force do_iac to start without
             seeing IAC initially, just get one more byte and put
             it at start of the buffer.
           ----------------------------------------------------------------*/
           else if (i == 1)
           {
             cnt_iac   = 1;
             index_iac = 0;
           }

           iac_flag  = 1;          /*  Flag active item                    */
           break;                  /*  EXIT LOOP                           */
        }
      } /*  else if (cp == IS99_IAC)  */

      /*---------------------------------------------------------------------
        Remember escape for next pass.
      ---------------------------------------------------------------------*/
      else if (c == EIA617_EM || c == IS99_IAC)  /*  Remember escape for   */
      {                                          /*  next pass             */
          cp = c;
      }
      /*---------------------------------------------------------------------
        Not escape - copy to out. Clear previous.
      ---------------------------------------------------------------------*/
      else if (! (em_flag || iac_flag))
      {
        in_item_ptr->data_ptr[j++] = c;
        cp = '\0';
      }
    } /*  while (i < in_item_ptr->used) */

    /*-----------------------------------------------------------------------
      If only transparency was processed, fix length.
    -----------------------------------------------------------------------*/
    if (! (em_flag || iac_flag))
    {
      in_item_ptr->used = j;
    }
  }     /*  (! (em_flag  || iac_flag))  */

  /*-------------------------------------------------------------------------
    If the item is not exhausted (ie  i < used), then
    Scan the rest of the item, and set the em_flag and iac_flag as
    needed for message processing.
  -------------------------------------------------------------------------*/
  while (i < in_item_ptr->used)
  {
    c = in_item_ptr->data_ptr[i++];
    if (c  == EIA617_EM)
    {
      em_flag = 1;
    }
    else if(c == IS99_IAC)
    {
      iac_flag = 1;
    }
  }


  /*-------------------------------------------------------------------------
        If there is an IAC code or transparency, process the incoming data,
        otherwise, just copy the pointer.

        Either way, 'next_item_ptr' points to the result of this process.

        'iac_flag' is reset within do_iac() after the complete
        in-band code is processed.  This allows it to stay
        set across multiple input data items, in case the
        in-band signal gets split.

        Since do_iac() may terminate before all the data in in_item has
        been used, it must set 'iac_flag' in that case to allow it
        to restart and process the rest of the input.
  -------------------------------------------------------------------------*/


  /*-------------------------------------------------------------------------
        Loop until in_item is empty, or the pointer has been passed on.
  -------------------------------------------------------------------------*/
  while (in_item_ptr != NULL && in_item_ptr->used > 0)
  {
    if (iac_flag)
    {
      next_item_ptr = ps707_async_do_iac (in_item_ptr);
    }
    else
    {
      next_item_ptr = in_item_ptr;
      in_item_ptr = NULL; /*  Force "while (in_item_ptr ...) to terminate  */
    }
    /*-----------------------------------------------------------------------
        If there is an EM code or transparency, process the incoming data,
        otherwise, just copy the pointer.

        Either way, 'out_item_ptr' points to the result of this process.

        'em_flag' is reset within do_em() after the complete
        in-band code is processed.  This allows it to stay
        set across multiple input data items, in case the
        in-band signal gets split.

        Since do_em() may terminate before all the data in in_item has
        been used, it must set 'em_flag' in that case to allow it
        to restart and process the rest of the input.
    -----------------------------------------------------------------------*/
    /*----------------------------------------------------------------------
        Loop until next_item is empty, or the pointer has been passed on.
    -----------------------------------------------------------------------*/
    while (next_item_ptr != NULL && next_item_ptr->used > 0)
    {
      if (em_flag)
      {
        out_item_ptr = ps707_async_do_em (next_item_ptr);
      }
      else
      {
        out_item_ptr = next_item_ptr;
        /*  Force "while (next_item_ptr ...) to terminate */
        next_item_ptr = NULL;
      }


      if ( out_item_ptr == NULL )
      {     
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Running out of buffers");
      }
      else
      { 
       /*---------------------------------------------------------------------
            If there is any data left in the main data stream,
            output it to the serial port.
            Otherwise, free the data item.
       ---------------------------------------------------------------------*/
        if (out_item_ptr->used != 0)
        {
          (void)ds3g_siolib_setup_tx ( out_item_ptr, FALSE );
        }
        else
        {
          (void) dsm_free_buffer(out_item_ptr);
        }
      }

    }   /*  while (next_item_ptr ...  */

    (void)dsm_free_buffer (next_item_ptr);  /*  Item empty: free it */

  }   /*  while (in_item_ptr ...  */

  (void)dsm_free_buffer (in_item_ptr);        /*  Item empty: free it */

} /* ps707_async_process_data_from_um() */ 
 
/*===========================================================================
FUNCTION      PS707_ASYNC_DO_EM

DESCRIPTION   This function strips out all EIA-617 <EM> in-band signals, and
              <EM> transparency codes. The reflected AT codes transmitted as
              EIA-617 in-band signals are used to call ds_atcop
              (DS_ATCOP_ONLINE). Any spaces remain. Response messages are
              removed from the EIA-617 STATUS messages, and output to a new
              output data item.

              The remainder of the data stream is output to the new output
              data item.

DEPENDENCIES  'cnt_617' and 'index_617' must be set to 0 before starting to
              process the input data stream. This is done by
              ps707_async_call_init().

              The output item may be longer than the input item.
              This is safe for the following reasons:

              Both the input and output items are LARGE items.  With
              exceptions described below, the output item cannot be
              larger than the input item.

              Non EIA-617 data is copied verbatim to the output, so the
              same number of bytes appear at the output as at the input.

              Transparency codes output either 1 or 2 bytes for 2 bytes
              in, so they either shrink or break even.

              STATUS messages have a four byte header which is not output.
              They have two <CRLF> pairs added to the message text (one
              pair before, and one after), so they break even.

              No other EIA-617 messages produce output, so they shrink.

              The only thing that can cause the length of the output item
              to exceed the length of the input item is an EIA-617 message
              which stradles two input items.  In this case, the message
              is accumulated in 'buf_em' until it is complete.  If the
              message is transparency or STATUS, it is output to the output
              item.  The amount of length increase is roughly the length
              of the part of the message which was in the preceeding input
              item.  The upper limit, then is the length of a STATUS message
              text (95 bytes), plus the 4 bytes of <CRLF>.

              The output item is modified by applying DSM_ITEM_HEAD which
              increases its size by the size of the TCP header and footer.

              NB: As long as DSM_ITEM_HEAD increases the available space
                  in the output item by more than 99 bytes,  and
                  as long as the header area has _not_ been used in the
                  input item, it is safe to allow the output item to expand.

              Whenever the size of the output item exceeds the size of the
              unexpanded size of a LARGE item, do_em terminates, so that
              the item can be processed.

RETURN VALUE  Pointer to new output item.

SIDE EFFECTS  'em_flag is set whenever a 617 command is being processed,
              and cleared when the processing is complete.

              Whenever the size of the output item exceeds the size of the
              unexpanded size of a LARGE item, do_em terminates, so that
              the item can be processed. 'em_flag' is set so that do_em
              will be called again to finish processing its input item.
===========================================================================*/
dsm_item_type * ps707_async_do_em
(
  dsm_item_type *in_item_ptr               /*  Pointer to input data item  */
) 
{
  byte c;
  byte * r_ptr;
  byte index = 0;
  word nominal_size;
  int msg_start;
  dsm_item_type *out_item_ptr;  /*  Pointer to output data item            */
  ds707_async_state_type*   ds707_async_state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0(MSG_LEGACY_LOW, "ps707_async_do_em()");

  /*------------------------------------------------------------------------- 
    Get the pointer to ds707_async_state
  -------------------------------------------------------------------------*/
  ds707_async_state = ds707_get_async_state_ptr();

  /*-------------------------------------------------------------------------
        Get a new data item for output, save the original size,
        and expand the item by the size of the TCP header and footer.
  -------------------------------------------------------------------------*/
  out_item_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
  if (out_item_ptr == NULL)
  {
    dsm_free_packet(&in_item_ptr);
    em_flag = 0;
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Running out of buffers");
    return NULL;
  }

  nominal_size = out_item_ptr->size;

  /*-------------------------------------------------------------------------
        Loop until all of the input data has been used, or until the
        size of the output data exceeds the original siz of the item.
  -------------------------------------------------------------------------*/
  while (in_item_ptr->used && out_item_ptr->used <= nominal_size)
  {
    in_item_ptr->used--;
    c = *in_item_ptr->data_ptr++;

   /*------------------------------------------------------------------------
          If 'cnt_617' is zero (and index_617 is zero, see below),
          then we are searching for the lead-in character, and copying
          all others to the ouput.

          If 'cnt_617' is not zero, it is the number of bytes to copy
          to the inband signal buffer.

          'index_617' is the next location to use in the in-band signal
          buffer. It is also the count of characters in the in-band buffer.
    -----------------------------------------------------------------------*/
    if (cnt_617 == 0)
    {
      /*---------------------------------------------------------------------
         Starting a 617 in-band command. Initially, just get one more
         byte and put it at start of buffer. Flag active item in case
         it's split across data items.
      ---------------------------------------------------------------------*/
      if (c == EIA617_EM)
      {
        cnt_617   = 1;
        index_617 = 0;
        em_flag   = 1;
      }
      else
      {
        out_item_ptr->data_ptr[out_item_ptr->used++] = c;
      }
    }

    /*-----------------------------------------------------------------------
       We're saving bytes in a '617' cmd.
    -----------------------------------------------------------------------*/
    else
    {
      if (index_617 >= MAX_LINE_SIZE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "OVERFLOW");
      }

      buf_em[index_617++] = c;
      --cnt_617;

     /*----------------------------------------------------------------------
        1st command byte.
     ----------------------------------------------------------------------*/
      if (index_617 == 1 && (c == EIA617_EXTEND0_B2M
           || c == EIA617_EXTEND1_B2M))
      {
        cnt_617 = 1;       /*  Extended command, get another                */
      }
     /*----------------------------------------------------------------------
        Length byte.
     ----------------------------------------------------------------------*/
      else if (index_617 == 2)
      {
        cnt_617 = c - 0x1F;
      }
    }

   /*------------------------------------------------------------------------
         If 'cnt_617' is zero and 'index_617' is not zero
         (which can only be true if it just happened!)
         then a complete in-band signal is in the in-band
         buffer.  It is 'index_617' bytes long.  The first
         byte in the buffer is the basic command code,
         NOT the lead-in byte.
   ------------------------------------------------------------------------*/
    if (cnt_617 == 0 && index_617 != 0)
    {
      em_flag = 0;

      if (buf_em[1] < 29)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Array index of buf_em is out of bounds");
        buf_em[0] = '\0';
      }
      else
      {
        index = (buf_em[1] - 0x1f) + 2;
        buf_em[index] = '\0';
      }

      switch (buf_em[0])
      {
        case EIA617_EXTEND0_B2M:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_EXTEND0_B2M");
          msg_start = 3;  /*  Bypass extraneous (and illegal) <CR><LF>'s   */
          while (buf_em[msg_start] == '\x0D' || buf_em[msg_start] == '\x0A')
          {
            ++msg_start;
          }

          /*-----------------------------------------------------------------
              Extended code found.  The encoded length is in
              buf_em[1], and the secondary code is in buf_em[2].
              A null character is appended to the end of the buffer.
              The buffer beginning with buf_em[msg_start] is now ready
              for further processing.
          -----------------------------------------------------------------*/
          switch (buf_em[2])
          {
            case EIA617_CONTROL:
              /*-------------------------------------------------------------
                Reflected AT code.
              -------------------------------------------------------------*/
              DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_CONTROL");
              dsat_process_cmd_line ( DSAT_ONLINE_DATA, &buf_em[msg_start] );
              r_ptr = &buf_em[msg_start];           
              break;

            case EIA617_STATUS:
              /*-------------------------------------------------------------

                Status message to serial port.
              -------------------------------------------------------------*/
              DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_STATUS");
              if (dsat_is_status(&buf_em[msg_start], DSAT_CONNECT))
              {
                /*-----------------------------------------------------------
                   Terminate connect message before any speed or protocol
                   indicators which might confuse Windows.
                -----------------------------------------------------------*/
                DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Found connect");
                buf_em[msg_start+7] = '\0';

                if (dsat707_fdt_seen)
                {
                  dsat707_fdt_seen = FALSE;
                  ds3g_siolib_ioctl (  SIO_IOCTL_BEGIN_FAX_IMAGE,
                                       &ds707_async_sio_ioctl_param );
                }
                /*-----------------------------------------------------------
                  This is needed as atcop doesn't consider case of connect
                  coming from IWF. So it doesn't know when it's no longer
                  valid to be abortable.
                -----------------------------------------------------------*/
                dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE,DS3G_SIOLIB_DATA_PORT );
              }

              if (dsat_is_status (&buf_em[msg_start], DSAT_NO_CARRIER))
              {
                DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Found no carrier");
                ds707_async_state->no_carrier_txed = TRUE;
                ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_MASK,
                                               DS_FLOW_DISABLE );
                ps_disable_sig ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL );
                /*-----------------------------------------------------------
                  This is needed as atcop doesn't consider case of no carrier
                  coming from IWF. So it doesn't know when it's no longer
                  valid to be abortable.
                -----------------------------------------------------------*/
                dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE,DS3G_SIOLIB_DATA_PORT );
              }

              if (dsat_is_status (&buf_em[msg_start], DSAT_OK))
              {
                DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Found ok");
                ds707_async_state->ok_result_txed = TRUE;
              }
              r_ptr = &buf_em[3]; /*  pointer to unstripped text           */

              while ((c = *r_ptr++) != '\0')
              {
                out_item_ptr->data_ptr[out_item_ptr->used++] = c;
              }

              /*lint -save -e734 accessing other modules so suppressing*/
              out_item_ptr->data_ptr[out_item_ptr->used++] =
                    (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
              if ((dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_V_IDX,0,0,NUM_TYPE) == 1)  /*  Add <LF> if verbose               */
              {
                out_item_ptr->data_ptr[out_item_ptr->used++] =
                      (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
              }
              /*lint -restore*/
              break;

            default:
              DATA_IS707_MSG0(MSG_LEGACY_MED, "Unknown");
              break;
          }
          break;

        case EIA617_EXTEND1_B2M:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_EXTEND1_B2M");
          switch (buf_em[2])
          {
            case EIA617_RESP_2M:  /*  Data response text                   */
              r_ptr = &buf_em[3]; /*  pointer to text                      */

              while ((c = *r_ptr++) != '\0')
              {
                out_item_ptr->data_ptr[out_item_ptr->used++] = c;
              }

              if (buf_em[1] != 0x7E)  /*  if buffer is full, don't append  */
              {
                /*  <CRLF>.  There may be more.      */
                /*lint -save -e734 accessing other modules so suppressing*/
                out_item_ptr->data_ptr[out_item_ptr->used++] =
                      (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
                out_item_ptr->data_ptr[out_item_ptr->used++] =
                      (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
                /*lint -restore*/
              }
              break;

           default:
             DATA_IS707_MSG0(MSG_LEGACY_MED, "Unknown");
             break;
          }
          break;

        case EIA617_FLOW_OFF:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_FLOW_OFF");
          break;

        case EIA617_FLOW_ON:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_FLOW_ON");
          break;

        /*-------------------------------------------------------------------
              The two following commands are the transparency codes.
        -------------------------------------------------------------------*/
        case EIA617_SINGLE_EM_B2M:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_SINGLE_EM_B2M");
          out_item_ptr->data_ptr[out_item_ptr->used++] = EIA617_EM;
          break;

        case EIA617_DOUBLE_EM_B2M:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_DOUBLE_EM_B2M");
          out_item_ptr->data_ptr[out_item_ptr->used++] = EIA617_EM;
          out_item_ptr->data_ptr[out_item_ptr->used++] = EIA617_EM;
          break;

        case EIA617_EM_109_ON:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_EM_109_ON");
          if ((ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_ON ))
             != DS3G_SIOLIB_OK )
          {
            ERR ( "ds3g_siolib_set_cd_state(ON) returned fail", 0, 0, 0 );
          }
          break;

        case EIA617_EM_109_OFF:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "EIA617_EM_109_OFF");
          if ((ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF ))
             != DS3G_SIOLIB_OK )
          {
            ERR ( "ds3g_siolib_set_cd_state(OFF) returned fail", 0, 0, 0 );
          }
          break;


        default:
          DATA_IS707_MSG1(MSG_LEGACY_MED, "Unknown EM code %x", buf_em[0]);
          break;

      }
      index_617 = 0;
    } /*  if cnt == 0 && index != 0 */
  } /*  while */

  if (in_item_ptr->used)
    {
    em_flag   = 1;                                    /*  Incomplete item  */
    }

  return (out_item_ptr);

} /* ps707_async_do_em() */

/*=========================================================================

FUNCTION      PS707_ASYNC_DO_IAC

DESCRIPTION   This function strips off the <IAC> compression negotiation
              codes and transparency sequences.  Responses to the
              compression negotiation are generated and sent back to the
              base station as <IAC> sequences via dsm_enqueue.  The <IAC>
              sequence will be presented in its entirety to dsm_enqueue.

              The remainder of the data stream and any unsolicited status
              messages is output to a new output data item.

DEPENDENCIES  'cnt_iac' and 'index_iac' must be set to 0 before starting
              to process the input data stream.  This is done by
              ps707_async_call_init().

              The output item may be longer than the input item.
              This is safe for the following reasons:

              Both the input and output items are LARGE items.  With
              exceptions described below, the output item cannot be
              larger than the input item.

              Non IAC data is copied verbatim to the output, so the
              same number of bytes appear at the output as at the input.

              Transparency codes output 1 byte for 2 bytes in, so they
              shrink.

              If the +CDR switch is ON (1), a status message may be
              generated:
              This message is a maximum of 14 bytes long. The IAC
              message generating it is 3 bytes long, so the max gain
              is 11 byte.

              IAC messages stradling two input items can add at most
              2 bytes to the increase caused by the +CDR status message.
              No other IAC messages produce output, so they shrink.

              The output item is modified by applying DSM_ITEM_HEAD which
              increases its size by the size of the TCP header and footer.

              NB: As long as DSM_ITEM_HEAD increases the available space
                  in the output item by more than 14 bytes,  and
                  as long as the header area has _not_ been used in the
                  input item, it is safe for the output item to expand.

RETURN VALUE  Pointer to output data item.

SIDE EFFECTS  'iac_flag is set whenever an IAC command is being processed,
              and cleared when the processing is complete.

              Whenever the size of the output item exceeds the size of the
              unexpanded size of a LARGE item, do_iac terminates, so that
              the item can be processed. 'iac_flag' is set so that do_iac
              will be called again to finish processing its input item.

===========================================================================*/
dsm_item_type * ps707_async_do_iac
(
  dsm_item_type *in_item_ptr               /*  Pointer to input data item  */
)
{
  byte c;
  word nominal_size;
  dsm_item_type *out_item_ptr;             /*  Pointer to output data item */
  dsm_item_type * um_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0(MSG_LEGACY_LOW, "ps707_async_do_iac()");
  /*-------------------------------------------------------------------------
        Get a new data item for output, save the original size,
        and expand the item by the size of the TCP header and footer.
  -------------------------------------------------------------------------*/
  out_item_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
  if (out_item_ptr == NULL)
  {
    dsm_free_packet(&in_item_ptr);
    iac_flag = 0;
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Running out of buffers");
    return NULL;
  }


  nominal_size = out_item_ptr->size;

  /*-------------------------------------------------------------------------
        Loop until all of the input data has been used, or until the
        size of the output data exceeds the original siz of the item.
  -------------------------------------------------------------------------*/
  while (in_item_ptr->used && out_item_ptr->used <= nominal_size)
  {
    in_item_ptr->used--;
    c = *in_item_ptr->data_ptr++;

    /*-----------------------------------------------------------------------
          If 'cnt_iac' is zero (and index_iac is zero, see below),
          then we are searching for the lead-in character, and copying
          all others to the ouput.

          If 'cnt_iac' is not zero, it is the number of bytes to copy
          to the inband signal buffer.

          'index_iac' is the next location to use in the in-band signal
          buffer. It is also the count of characters in the in-band
          buffer.
    -----------------------------------------------------------------------*/
    if (cnt_iac == 0)
    {
      /*---------------------------------------------------------------------
         Starting an IAC command.
      ---------------------------------------------------------------------*/
      if (c == IS99_IAC)
      {
        cnt_iac   = 1;                 /*  get one more to begin with      */
        index_iac = 0;                 /*  init index / count              */
        iac_flag  = 1;                 /*  Flag active item in case it's   */
      }                                /*    across data items             */
      else
      {                                /*  else copy to output             */
        out_item_ptr->data_ptr[out_item_ptr->used++] = c;
      }
    }
    /*-----------------------------------------------------------------------
        IAC command in progress.
    -----------------------------------------------------------------------*/
    else
    {
      if (index_iac >= MAX_IAC)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "OVERFLOW");
      }

      buf_iac[index_iac++] = c;
      --cnt_iac;

      /*---------------------------------------------------------------------
        If sub-negotiations are in progress.
      ---------------------------------------------------------------------*/
      if (sub_neg_iac != 0)
      {
        if (c == IS99_SE)
        {
          cnt_iac = 0;                           /*  No more to copy       */
          sub_neg_iac = 0;                       /*  Sub-negotiation over  */
        }
      }
      else
      {
        if (c == IS99_SB)                      /*  Begin sub_negotiation   */
        {
          cnt_iac = MAX_IAC - 1;               /*  Allow for max length    */
          sub_neg_iac = 1;                     /*  sub-negotiation starts  */
        }
        else if (index_iac == 1 && c != IS99_IAC)
        {
            cnt_iac = 1;                 /*  Not transparency, get another */
        }
      }
    }
    /*-----------------------------------------------------------------------
          If 'cnt_iac' is zero and 'index_iac' is not zero
          (which can only be true if it just happened!)
          then a complete in-band signal is in the in-band
          buffer.  It is 'index_iac' bytes long.  The first
          byte in the buffer is the basic command code,
          NOT the lead-in byte.
    -----------------------------------------------------------------------*/
    if (cnt_iac == 0 && index_iac != 0)
    {
      iac_flag = 0;

      switch (buf_iac[0])
      /*---------------------------------------------------------------------
          Switch on the basic command code.
      ---------------------------------------------------------------------*/
      {
        case IS99_IAC:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "IS99_IAC");
          /*-----------------------------------------------------------------
              Strip transparency byte and ouput.
          -----------------------------------------------------------------*/
          out_item_ptr->data_ptr[out_item_ptr->used++] = IS99_IAC;
          break;

        case IS99_SB:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "IS99_SB");
          /*-----------------------------------------------------------------
                Note:  (See IS-99, 3.2
                When (or if!) subnegotiations are implemented,
                'buf_iac' contains:
                    <IS99_SB>, option, zero or more subnegotiation
                    frames, <IS99_IAC>, <IS99_SE>
                Any bytes containing <IS99_IAC> will be represented
                by an <IS99_IAC><IS99_IAC> combination.
          -----------------------------------------------------------------*/
          /*  For now, fall thru.  SB should never happen, but             */
          /*  if it does, return DONT.  
                                         */
          //lint -fallthrough
        case IS99_DO:
        case IS99_WILL:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "IS99_DO/IS99_WILL");
          /*-----------------------------------------------------------------
              For now, refuse compression.
          -----------------------------------------------------------------*/
          um_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
          if (um_item_ptr == NULL)
          {
            dsm_free_packet(&in_item_ptr);
            DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Running out of buffers");
            return NULL;
          }

          um_item_ptr->data_ptr[0] = IS99_IAC;
          um_item_ptr->data_ptr[1] = IS99_DONT;
          um_item_ptr->data_ptr[2] = buf_iac[1];
          um_item_ptr->used = 3;
          um_item_ptr->kind = DSM_PS_APP_PAYLOAD;

          ds707_async_ps_tcp_enqueue( &um_item_ptr);
          break;

        case IS99_DONT:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "IS99_DONT");
          /*-----------------------------------------------------------------
              Respond with WONT.
              Fall thru to end of negotiation since Base
              shouldn't respond to WONT.
          -----------------------------------------------------------------*/
          um_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
          if (um_item_ptr == NULL)
          {
            dsm_free_packet(&in_item_ptr);
            DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Running out of buffers");
            return NULL;
          }

          um_item_ptr->data_ptr[0] = IS99_IAC;
          um_item_ptr->data_ptr[1] = IS99_WONT;
          um_item_ptr->data_ptr[2] = buf_iac[1];
          um_item_ptr->used = 3;
          um_item_ptr->kind = DSM_PS_APP_PAYLOAD;

          ds707_async_ps_tcp_enqueue( &um_item_ptr);
          //lint -fallthrough
          
        case IS99_WONT:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "IS99_WONT");
          /*-----------------------------------------------------------------
              End of negotiation: do optional unsolicited
              response, if requested.
          -----------------------------------------------------------------*/
          break;

        case IS99_SE:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "IS99_SE");
          /*-----------------------------------------------------------------
               If this code occurs here, it means that the
               subnegotiation overflowed 'buf_iac'.
          -----------------------------------------------------------------*/
          DATA_IS707_MSG0(MSG_LEGACY_MED, "Missed end of sub negot!");
          break;

        default:
          /*-----------------------------------------------------------------
              for the moment, ignore any others.
          -----------------------------------------------------------------*/
          DATA_IS707_MSG1(MSG_LEGACY_MED, "Unknown IAC code %x", buf_em[0]);
          break;
        }
      index_iac = 0;
    } /*  if (cnt_iac == 0 && index_iac != 0) */
  } /*  while */

  if (in_item_ptr->used)
  {
    iac_flag   = 1;                                    /*  Incomplete item */
  }

  return (out_item_ptr);

} /* ps707_async_do_iac() */ 

/*===========================================================================
FUNCTION      PS707_ASYNC_CLEANUP_PTCL

DESCRIPTION   This function performs protocol cleanup operations after
              the end of an IS-707 async data call. It gets called either
              upon expiration of a timer when TCP closes gracefully or
              it's explicitly called when DS asks PS to abort protocols
              forcibly.

DEPENDENCIES  Must be called after the termination of protocol operations
              for an IS-707 async data call.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_cleanup_ptcl
(
  void
)
{
  dsm_item_type *item_ptr;
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
  dsm_watermark_type *ds707_wmk_um_SN_fwd_wm_list = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0(MSG_LEGACY_LOW,  "ps707_async_cleanup_ptcl()");

  /*-------------------------------------------------------------------------
    Whenever, protocols close, we always come here.
  ------------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_MASK, DS_FLOW_DISABLE );

  ps_disable_sig ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL );

  if ( ps_timer_cancel ( ps707_async_state.ptcl_cleanup_timeout_handle )
       == PS_TIMER_FAILURE )
  {
    ERR ("ps_timer_cancel() failed", 0, 0, 0);
  }

  ps_tcp_async_reset_tcp(Async_handle);

  ps707_async_state.protocol_state = PS707_ASYNC_PROTOCOL_CLOSED;

  /*-------------------------------------------------------------------------
    As stated in IS-707, increment the port number if the call just ended was
    mobile-originated, if it rolls past 0xFFFF, set to initial port number.
  -------------------------------------------------------------------------*/
  if ( ps707_async_state.mobile_originated == TRUE )
  {
    ps707_async_local_port++;
    if( ps707_async_local_port == 0)
    {
      ps707_async_local_port = RAWDATA_INITIAL_SOURCE_PORT;
    }
  }

  ps707_async_state.mobile_originated   = FALSE;

  /*-----------------------------------------------------------------------
    Deregister from PPP events.
   ----------------------------------------------------------------------*/

  if ( (ppp_event_deregister( ppp_event_handle,
                              PPP_PROTOCOL_LCP,
                              PPP_DOWN_EV_MASK )
       ) < 0 )
  {
    ERR_FATAL ("ppp_event_register returned err", 0, 0, 0 );
  }
  if ( (ppp_event_deregister( ppp_event_handle,
                              PPP_PROTOCOL_IPCP,
                              PPP_UP_EV_MASK )
       ) < 0 )
  {
    ERR_FATAL ("ppp_event_register returned err", 0, 0, 0 );
  }
  if ( (ppp_event_deregister( ppp_event_handle,
                              PPP_PROTOCOL_IPCP,
                              PPP_RESYNC_EV_MASK )
       ) < 0 )
  {
    ERR_FATAL ("ppp_event_register returned err", 0, 0, 0 );
  }

  /*-------------------------------------------------------------------------
    Check queues for any items remaining and delete them
  -------------------------------------------------------------------------*/
  while(( item_ptr = dsm_dequeue( ds707_wmk_get_pri_wm_list() )) != NULL)
  {
    dsm_free_packet( &item_ptr);
  }  
  /*-------------------------------------------------------------------------
    Get pointer to ds707_wmk_um_SN_fwd_wm_list
  -------------------------------------------------------------------------*/
  ds707_wmk_um_SN_fwd_wm_list = ds707_get_wmk_um_SN_fwd_wm_list();
  dsm_empty_queue (ds707_wmk_um_SN_fwd_wm_list+DSRSP_ROUTE_A);
  dsm_empty_queue( ds707_get_async_to_sio_tx_wmk() );
  while(( item_ptr = ps707_async_tcp_dequeue()) != NULL)
  {
    dsm_free_packet( &item_ptr);
  }

  ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_MASK, DS_FLOW_ENABLE );

  DATA_IS707_MSG0(MSG_LEGACY_MED,  "Protocol Cleanup Complete" );

  /*-------------------------------------------------------------------------
    Inform dsmgr that Protocol shutdown is complete.
  -------------------------------------------------------------------------*/
  if( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_707_ASYNC_PTCL_CLOSED;
    ds_put_cmd(cmd_ptr);
  }


} /* ps707_async_cleanup_ptcl() */ 

/*===========================================================================
FUNCTION      PS707_GET_ASYNC_STATE_PTR

DESCRIPTION   Accessor function for ps707_async_state structure

DEPENDENCIES  None.

RETURN VALUE  Returns pointer to ps707_async_state structure.

SIDE EFFECTS  None.
===========================================================================*/
ps707_async_state_type* ps707_get_async_state_ptr
(
  void
)
{
  return &ps707_async_state;
} /* ps707_get_async_state_ptr */

#endif /* FEATURE_DATA_IS707_ASYNC */