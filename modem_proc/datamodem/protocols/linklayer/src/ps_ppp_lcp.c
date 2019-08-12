/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ P P P _ L C P . C

GENERAL DESCRIPTION
  This module contains the PPP protocol suite LCP (link control protocol)
  functions. The module contains the logic to perform the PPP LCP protocol.

EXTERNALIZED FUNCTIONS
  lcp_save_work_opt()
    This function stores the value of the LCP option that is passed in. It is
    being saved into the work and work_pdv the want values are saved by the
    caller.
  lcp_init()
    This function will initialize the LCP configuration structure
  lcp_mru_set()
    This function is called to set the MRU for a PPP device's LCP state
    machine
  lcp_mru_get()
    This function is called to get the MRU for a PPP device
  lcp_set_net_model_resync_fnc()
    This function sets the callback function pointer for the LCP established
    function.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 1992-1994 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21

Copyright (c) 1995-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: MM_DATA/ps_ppp_lcp.c_v   1.12   28 Feb 2003 11:12:56 ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_lcp.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    --------------------------------------------------------- 
06/23/14    cx     Free the old EAP instance if PPP resync happened during 
                   authentication phase
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
05/07/13    cx     Fix compiler errors 
10/25/12    fn     Remove usage of fatal error log messages
05/01/12    sb     Always On UIM Feature Cleanup
04/17/12    vpk    Correct the featurization for FEATURE_HDR_AN_AUTH.
03/08/12    ash    Propagate call end reasons correctly
02/08/12    ash    Added NCPI_CB_IS_INITED Macro to check initilization status 
                   of VSNCP control block.
10/17/11    ssh    eHRPD Always On support
09/30/11    ash    Don't set fail_reason as TIMEOUT for LCP termination
09/27/11    ash    Terminate PPP on LCP failure
04/19/11    op     Fixed klocwork warnings
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
01/28/11    jee    JCDMA feature cleanup & PPP re-sync handling
01/16/11    ss     Added some log messages.
10/04/10    mga    Changes to ensure fail reason is propagated accordingly 
                   when auth is mandatory but not negotiated during LCP.
09/08/10    mga    Changes to propagate correct fail reason to Mode Handler
06/18/10    jee    Klocwork fixes
10/05/09    mga    Merged changes related to diag events
09/29/09    jee    Fixed klocwork errors
08/31/09    mga    Merged from eHRPD branch
07/14/09    pp     ppp_cb_lock changes.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
03/23/09    scb    Fixed bug in lcp_mru_get()
01/08/09    scb    Fixed QTF LINT errors.
11/10/08    scb    Fixed QTF compilation warnings.
09/24/08    scb    Replaced errno with ps_errno
09/02/08    rnp    Added STA related changes.
05/04/08    scb    Bug fixes caught during PPP STA development
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
12/21/07    scb    Removed a NULL check in lcp_check() for *(item_ref_ptr)
09/27/07    ifk    Removed old PPP statistics code.
05/14/07    scb    Changed variable names to avoid compilation error for
                   JCDMA_1X builds
04/27/07    scb    Removed LINT suppressions
04/19/07    scb    Added a KDDI PPP compliance requirement to configure
                   Application specific LCP constants
03/23/07    ssh    Moved posting of STARTING event to core FSM
02/27/07    scb    Fixed High LINT errors
02/08/07    scb    Fixed Klocwork high errors
01/08/07    ssh    Posting LCP starting event
11/01/06    ssh    Changes to allow clients to specify allowed auth protocols
10/10/06    ssh    Added lcp_setup_device(), setting accm in lcp_starting()
07/10/06    ssh    Extract and save the chap-digest in lcp_save_work_opt()
05/25/06    ssh    #defines for LCP option lengths in the option header
04/28/06    ifk    Added PPP critical sections.
04/20/06    ssh    Invoke hdlc_ioctl() in lcp_opening to set negotiated ACCM
03/05/06    ifk    Moved removing of option on receiving reject into
                   lcp_reject().  Special handling for compression protocol
                   option.
11/05/05    rt     Fixed coverity errors.
10/27/05    ssh    Reset auth mode in lcp_opening() if AP not negotiated
08/30/05    ssh    Fixed order of initialization in lcp_default
08/29/05    ssh    Reverted earlier fix for gratuitous PAP-Auth Req
08/15/05    ks     Included UIM files if FEATURE_UIM_SUPPORT_HRPD_AN is
                   defined.
08/01/05    ssh    Added CHAP option handling for zero length password,
                   removed setting LCP auth mode in lcp_option()
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
03/25/05    ifk    Fixed fsm_n_option() handling
03/22/05    ssh    Now using new function pppi_report_event_to_diag()
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
01/12/05    ifk    Added lcp_set_net_model_resync_fnc()
07/16/04    ifk    Renamed values of cp_mode_type enum
07/07/04    ifk    Removed dsm_kind field from fsm constants
07/05/04    ifk    Added lcp_mru_get() and lcp_mru_set() functions
02/24/04    mvl    Added JCDMA Term Req behaviour rather than Abort().
10/24/03    ifk    Added lcp_set_device_constants(), lcp_device_constants[]
10/09/03    ifk    Added setting of magic number in lcp_starting()
10/01/03    ifk    Removed extraneous memsets in lcp_init().
09/17/03    mvl    Featurized the sending of discard request such that it
                   will not happen for JCDMA_1X builds.
09/16/03    usb    Moved fsm_init() to pppi_start() to handle mobile
                   initiated resync
09/10/03    aku    Changed timeout message to LOG_MSG_INFO1.
08/13/03    ifk    Separated LCP and NCP negotiation in preparation for IP6CP
04/16/03    mvl    Resetting auth mode to none if C-Rej for auth received.
04/08/03    mvl    Added support to do server style authentication - fixed
                   event ordering (LCP up was after Auth starting).
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    removed authentication featurization
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added LCP event posting.
02/06/03    mvl    Removed check if resyncing when timer expires, and
                   simplified the timeout hanling.
02/06/03    usb    fixed compilation error by removing LOCAL from variables
                   used outside this file
11/19/02    jd     Fixed compile error for MIP_PERF
11/04/02    mvl    Removed non-auth versions of lcp_opening(). And fixed
                   MTU/MRU mixup.
09/18/02    mvl    Added user_data to ppp event callbacks.
09/12/02    mvl    Mainlined some netmodel code, generified the network model
                   resync callcack.  Removed netmodel featurization.
09/06/02    mvl    fixed assert()s.
08/13/02    mvl    Session handle support and using MTU/MRU from ps iface.
08/06/02    mvl    Reworked authentication.
07/31/02    usb    Renamed psmisc.h to ps_utils.h
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
                   Replaced ppp_stop() with ppp_abort() in timeout functions.
07/25/02    mvl    Removed netmodel dependency.  Updates for PPP renaming.
07/24/02    mvl    Fixed PPP termination bug.  Now when timers expire not
                   talking to DS, but calling ppp_stop().
07/01/02    mvl    Support for new architecture, generating event callbacks.
05/22/02    mvl    Removed non-netmodel ppp_outgoing() calls.
04/17/02    rc     Wrapped code in !FEATURE_DATA_WCDMA_PS as appropriate to
                   build for all targets.
03/04/02    dwp    Move inclusion dsiface.h out of MIP feature.
03/01/02    vas    Updated to allow authentication (PAP/CHAP) parameters to
                   be allocated per iface.
03/01/02    vsk    removed dsm_free_packet() being called in case of
                   ppp_outgoing() returns an error
02/20/02    sjy    Mobile IP performance now featurized under
                   FEATURE_DS_MOBILE_IP_PERF
02/12/02    dwp    Cleanup header inclusion(s) for non DS builds.
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/05/02    vr     Modified ppp_ready to not do IPCP if configured so
12/18/01    rsl    Updated tracer field.
12/01/01    vas    Changed function prototypes to take the ppp iface as a
                   parameter instead of being hardcoded. Made ppp_do_auth
                   part of ppp ctl blk. lcp_chap_info maintained per iface.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module, wrap old/new DS
                   Mgr dependencies w/FEATURE_DATA (namely PS to DS cmd i/f)
11/09/01    jay    Reject authentication if we are in a qnc call
10/29/01    igt    Force the ppp state to fsmINITIAL in lcp_init()
09/25/01    snn    Modified some messages in PAP/CHAP area to give more
                   information.
09/20/01    mvl    Reset the value of remote.work to 0 in starting() - to
                   ensure that we don't reject the othe guys desires.
09/13/01    mvl    Modified message to indicate that unsupported option was
                   negotiated.  Added copy of want to work in starting.
08/29/01  pjb/mvl  Modified the values of option bits to support DNS options.
08/19/01    sjy    Referenced the correct LCP configuration values in
                   lcp_powerup_init() for when netmodel feature is not
                   defined
08/15/01    ss     Removed some lines not needed in lcpi_timer_cb().
08/02/01  na/mvl   Fixed functions that were using the global ppp_cb_ptr,
                   general code cleanup
07/30/01    mvl    Auth option is always included in the lcp_accept var
                   because it is defined to 0 if one of the auth features is
                   not defined
                   Completely revamped the initialization so that IPCP can be
                   initialized per interface and with passed in options, or
                   the defaults.
07/29/01    na     lcp_stopping() now calls dsiface_down().
07/27/01    mvl    Modifications to use PS timers, checnged the retry timer
                   to not multiply the header value by 1000.
07/24/01    mvl    compiling out parts of fsm_constants_initializer struct to
                   conserve memory, initing fsm.iface field, changed
                   ppp_close() to pass iface param if MIP is defined and
                   removed obsolete debug macros.
07/23/01    pjb    Return type of dsm_pushdown changed. Change location/
                   prefix of command names.  Changed call to
                   dsm_offset_new_buffer to reserve space for header.
                   Fixed potential memory leak.
07/06/01    mvl    Added Mobile IP support - call dsiface_starting() when LCP
                   starts.
06/08/01    mvl    Added Mobile IP support (reject authentication if in
                   Mobile IP mode) and made formatting changes.
08/25/00    rc     Changed dsm_new_buffer() calls to take pool id as input
                   parameter. So, if pool size is to be passed in as
                   parameter, a new macro DSM_DS_POOL_SIZE is used to return
                   pool id based on pool size.
                   Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK.
07/22/00    snn    Initialized ppp_chap_retry_counter before every CHAP call.
05/09/00    rc     Added code in lcp_init() so that authentication is not
                   done in async/fax calls.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
03/10/00    snn    Added Support for CHAP. They have been added under the
                   feature DS_CHAP feature
10/28/99    mvl    Added code review changes
09/27/99    snn    Fixed commenting style and added comments.
08/26/99    ak     Fixed warnings for surfc and surfp builds.  msg.h and
                   err.h now included judiciously.
08/20/99    mvl    Added the lcp_save_work_opt() function - used to be in
                   ps_ppp_netmodel.c
08/17/99    smp    Added include files msg.h & err.h.
08/10/99    snn    Added support for PAP timeouts.
08/09/99    mvl    Added network model changes.
07/09/99    snn    Added support for PAP implementation
04/08/99    na     Changed PPP_LCP_PROTOCOL to DSM_LCP_PROTOCOL where the
                   protocol was used in the kind field.
01/27/98    ldg    Removed the DS_SLIM switch.
01/26/98    fkm    FEATURE_xxx Updates (via Automatic Scripts)
07/22/97    na     Added support for PAP - under DS_PAP feature.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "amssassert.h"
#include "ps_byte.h"
#include "dsm.h"
#include "ps_ppp.h"
#include "ps_ppp_config.h"
#include "ps_pppi.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi_fsm.h"
#include "ps_ppp_lcp.h"
#include "ps_pppi_lcp.h"
#include "ps_ppp_auth.h"
#include "ps_pppi_auth.h"
#include "ps_hdlc_libi.h"
#include "pstimer.h"
#include "ps_ppp_task.h"
#include "ps_utils.h"
#include "err.h"
#include "msg.h"

#if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MOBILE_IP_PERF)
#include "dsmip_api.h"
#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DS_MOBILE_IP_PERF */

#include "ps_ppp_events.h"
#include "nvruimi.h"
#include "uim.h"
#include "uim_v.h"

#if defined (FEATURE_DATA_PS_EHRPD) && defined (FEATURE_DATA_PS_ENFORCE_AUTH)
#include "ps_ppp_ncpi.h"
#endif /* (FEATURE_DATA_PS_EHRPD) &&  (FEATURE_DATA_PS_ENFORCE_AUTH) */

#include "ds_Utils_DebugMsg.h"
#include "ps_pppi_auth.h"

#ifdef FEATURE_DATA_PKT_CLASSIFIER
#include "ds707_pkt_classifier.h"
#endif /* FEATURE_DATA_PKT_CLASSIFIER */


/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
static lcp_value_type lcp_local_want_cb[PPP_MAX_DEV];
static lcp_value_type lcp_local_work_cb[PPP_MAX_DEV];
static lcp_value_type lcp_remote_want_cb[PPP_MAX_DEV];
static lcp_value_type lcp_remote_work_cb[PPP_MAX_DEV];

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
static net_model_resync_fptr_type lcp_net_model_resync_fnc[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
  These defaults are defined in the PPP RFCs, and must not be changed
---------------------------------------------------------------------------*/
static lcp_value_type lcp_default =
{
  LCP_ACCM_DEFAULT,  /* default ACCM - 0       */
  0UL,               /* no magic number        */
  LCP_MRU_DEFAULT,   /* default MRU            */
  0,                 /* no authentication      */
  0,                 /* no monitoring          */
  0,                 /* no default CHAP digest        */
  PPP_AUTH_DEFAULT,  /* no default protocols for auth */
  FALSE              /* Need not use the same auth after PPP re-sync */
};

static negotiate_t lcp_accept = LCP_N_MRU | LCP_N_ACCM | LCP_N_MAGIC | LCP_N_PFC |
                         LCP_N_ACFC | LCP_N_AP;

/*---------------------------------------------------------------------------
  The minimum and maximum values for the Length field in the option header.

    0                   1
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |    Length     |    Data ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
static uint8 option_length[] =
{
   LCP_MRU_OPTION_SIZE,           /* Maximum-Receive-Unit                  */
   LCP_ACCM_OPTION_SIZE,          /* ACCM                                  */
   LCP_AP_OPTION_SIZE,            /* Authentication-Protocol               */
   LCP_QUALITY_OPTION_SIZE,       /* Quality-Protocol                      */
   LCP_MAGIC_OPTION_SIZE,         /* Magic-Number                          */
   LCP_PFC_OPTION_SIZE,           /* Protocol-Field-Compression            */
   LCP_ACFC_OPTION_SIZE           /* Address-and-Control-Field-Compression */
};

/*---------------------------------------------------------------------------
  these are the PPP options numbers based on the option mask. (N's above)
----------------------------------------------------------------------------*/
static uint8 option_number[] =
{
  LCP_MRU,
  LCP_ACCM,
  LCP_AP,
  LCP_QMP,
  LCP_MAGIC,
  LCP_PFC,
  LCP_ACFC
};

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--*/
static void lcp_option
(
  ppp_fsm_type   *fsm_ptr,
  void           *vp,
  dsm_item_type **item_ref_ptr,
  uint8           o_type
);

static int lcp_check
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp,
  dsm_item_type    **item_ref_ptr,
  int                request
);

static int lcp_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp,
  dsm_item_type     *bp
);

static void lcp_starting
(
  ppp_fsm_type *fsm_ptr
);

static void lcp_stopping
(
  ppp_fsm_type *fsm_ptr
);

static void lcp_closing
(
  ppp_fsm_type *fsm_ptr
);

static void lcp_opening
(
  ppp_fsm_type *fsm_ptr
);

static void lcp_free
(
  ppp_fsm_type *fsm_ptr
);

void lcpi_timer_cb
(
  void *arg_ptr
);

/*---------------------------------------------------------------------------
  LCP constants for the FSM
---------------------------------------------------------------------------*/
static struct fsm_constant_s lcp_default_constants =
{
  PPP_LCP_PROTOCOL,         /* Protocol number                      */
  0x0FFE,                   /* codes 1-11 recognized                */

  LCP,                      /* Finite State Machine index           */
  LCP_REQ_TRY,              /* # tries for configure                */
  LCP_NAK_TRY,              /* # tries for nak substitutes          */
  LCP_TERM_TRY,             /* # tries for terminate                */
#ifdef FEATURE_JCDMA_1X
  LCP_TERM_TO,              /* Time for TERM REQ timeout (msecs)    */
#else
  LCP_TIMEOUT,              /* Time for TERM REQ timeouts (msecs)   */
#endif /* FEATURE_JCDMA_1X */
  LCP_TIMEOUT,              /* Time for C_ACK timeouts (msecs)      */

  LCP_OPTION_LIMIT,         /* maximum option number                */
  LCP_N_OPTION_LIMIT,       /* maximum mask number                  */
  option_number,            /* mask table                           */

  PPP_NCP_COMPRESS_IGNORE,  /* compression enable.  Invalid for LCP */

  lcp_free,                 /* to free structure                    */
  fsm_down,                 /* layer layer down event               */

  lcp_starting,             /* this layer start: enter starting     */
  lcp_opening,              /* this layer opening: enter opening    */
  lcp_closing,              /* this layer closing: enter closing    */
  lcp_stopping,             /* this layers stopping: leave opened   */

  lcp_option,               /* add option to message                */
  lcp_check,                /* validate option and store if good    */
  lcp_reject                /* process rx'ed config reject          */
};

static struct fsm_constant_s lcp_device_constants[PPP_MAX_DEV];
static struct cb_constant_s  lcp_cb_device_constants[PPP_MAX_DEV];


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION LCP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for LCP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lcp_powerup_init
(
  ppp_dev_enum_type device /* the interface to initialize */
)
{
  ppp_fsm_type   *fsm_ptr;
  ppp_type       *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);

  /*-------------------------------------------------------------------------
    reset the FSM for this interface, and LCP
  -------------------------------------------------------------------------*/
  memset(fsm_ptr, 0, sizeof(ppp_fsm_type));

  /*-------------------------------------------------------------------------
    Copy the default device constants
  -------------------------------------------------------------------------*/
  memscpy(&lcp_device_constants[device],
          sizeof(lcp_device_constants[device]),
          &lcp_default_constants,
          sizeof(lcp_default_constants));
  /*-------------------------------------------------------------------------
    Copy the default lcp cb constants
  -------------------------------------------------------------------------*/
  memscpy(&lcp_cb_device_constants[device],
          sizeof(lcp_cb_device_constants[device]),
          &pppi_fsm_cb_default_constants,
          sizeof(pppi_fsm_cb_default_constants));

  /*-------------------------------------------------------------------------
    set the network model resync function to NULL
  -------------------------------------------------------------------------*/
  lcp_net_model_resync_fnc[device] = NULL;

  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants, variable pointer and the
    work and want value pointers to their allocated memory.
  -------------------------------------------------------------------------*/
  fsm_ptr->pdc             = &lcp_device_constants[device];
  fsm_ptr->cbdc            = &lcp_cb_device_constants[device];
  fsm_ptr->pdv             = NULL;
  fsm_ptr->local.want_pdv  = &(lcp_local_want_cb[device]);
  fsm_ptr->local.work_pdv  = &(lcp_local_work_cb[device]);
  fsm_ptr->remote.want_pdv = &(lcp_remote_want_cb[device]);
  fsm_ptr->remote.work_pdv = &(lcp_remote_work_cb[device]);
  fsm_ptr->mode            = PPP_CP_MODE_INVALID;
  fsm_ptr->timer_alloc_fcn     = (timer_alloc_f_type) ps_timer_alloc;
  fsm_ptr->timer_start_fcn     = (timer_start_f_type) ps_timer_start;
  fsm_ptr->timer_cancel_fcn    = (timer_cancel_f_type) ps_timer_cancel;

  /*-------------------------------------------------------------------------
    Initialize the device field to the device passed in
  -------------------------------------------------------------------------*/
  fsm_ptr->device = device;
  /*-------------------------------------------------------------------------
    Get the timer for this instance of LCP - return 0 if OK or -1 if failed
  -------------------------------------------------------------------------*/
  fsm_ptr->timer = (*fsm_ptr->timer_alloc_fcn)(lcpi_timer_cb, 
                                               ppp_cb_array + (uint8)device);

  if(fsm_ptr->timer == PS_TIMER_INVALID_HANDLE)
  {
    return -1;
  }
  else
  {
    return 0;
  }

} /* lcp_powerup_init() */


/*===========================================================================
FUNCTION LCP_SET_DEVICE_CONSTANTS()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  LCP on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for LCP.
===========================================================================*/
void lcp_set_device_constants
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  nv_ppp_configure_data_type *config
)
{
  struct fsm_constant_s *pconst;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV || NULL == config)
  {
    ASSERT(0);
    return;
  }

  pconst = &lcp_device_constants[device];

  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants that are configurable
  -------------------------------------------------------------------------*/
  pconst->try_configure  = config->lcp_req_try;
  pconst->try_nak        = config->lcp_nak_try;
  pconst->try_terminate  = config->lcp_term_try;

  if( 0 < config->lcp_term_timeout )
  {
    pconst->term_timeout = config->lcp_term_timeout;
  }

  if( 0 < config->lcp_ack_timeout )
  {
    pconst->ack_timeout = config->lcp_ack_timeout;
  }

  return;
} /* lcp_set_device_constants() */



/*===========================================================================
FUNCTION PPP_LCP_INIT_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  LCP on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for LCP.
===========================================================================*/
void ppp_lcp_init_device_constant
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  ppp_init_consts_enum_type constant,
  uint32 value
)
{
  struct fsm_constant_s *pconst = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV || constant >= PPP_MAX_INIT_CONSTS )
  {
    LOG_MSG_ERROR_2( "Invalid args: device %d, constant %d",
                     device, constant );
    ASSERT(0);
    return ;
  }

  pconst = &lcp_device_constants[device];

  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants that are configurable
  -------------------------------------------------------------------------*/
  switch(constant)
  {
    case LCP_ACK_TIMEOUT:
      if( 0 < value )
      {
        pconst->ack_timeout = (uint16)value;
      }
      break;

    case LCP_REQUEST_TRY:
      if( 0 < value )
      {
        pconst->try_configure = (uint8)value;
      }
      break;

    default:
      LOG_MSG_ERROR_1("ppp_lcp_init_device_constant: Unknown constant %d",
                      constant);
      break;
  }

  return;
} /* ppp_lcp_init_device_constant() */



/*===========================================================================
FUNCTION LCP_INIT()

DESCRIPTION
  This function will initialize the LCP configuration structure for the given
  interface (Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_init
(
  ppp_dev_enum_type  device,   /* device to initalize         */
  ppp_dev_opts_type *opts_ptr   /* options to initialize with */
)
{
  ppp_fsm_type *fsm_ptr;
  lcp_value_type *lwant_ptr;
  lcp_value_type *lwork_ptr;
  lcp_value_type *rwant_ptr;
  lcp_value_type *rwork_ptr;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
#endif
  ppp_type *ppp_cb_array = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV ||
     opts_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
  lwant_ptr = (lcp_value_type*)fsm_ptr->local.want_pdv;
  lwork_ptr = (lcp_value_type*)fsm_ptr->local.work_pdv;
  rwant_ptr = (lcp_value_type*)fsm_ptr->remote.want_pdv;
  rwork_ptr = (lcp_value_type*)fsm_ptr->remote.work_pdv;

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-------------------------------------------------------------------------
    Notify M.IP call performance that Um/Rm PPP negotiation is starting.
  -------------------------------------------------------------------------*/
  if(device == PPP_UM_SN_DEV)
  {
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_UM_PPP_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_UM_PPP_DELAY,
                   PERF_START_TS);
  }
  else if(device == PPP_RM_DEV)
  {
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_RM_PPP_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_RM_PPP_DELAY,
                   PERF_START_TS);
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

  /*-------------------------------------------------------------------------
    Use passed in options to initialize things.
  -----------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  memscpy(lwant_ptr,
          sizeof(lcp_value_type),
          &(opts_ptr->lcp_info.want_vals[FSM_LOCAL]),
          sizeof(lcp_value_type));
  memscpy(rwant_ptr,
          sizeof(lcp_value_type),
          &(opts_ptr->lcp_info.want_vals[FSM_REMOTE]),
          sizeof(lcp_value_type));
  memscpy(lwork_ptr,
          sizeof(lcp_value_type),
          &(opts_ptr->lcp_info.want_vals[FSM_LOCAL]),
          sizeof(lcp_value_type));
  memscpy(rwork_ptr,
          sizeof(lcp_value_type),
          &(opts_ptr->lcp_info.want_vals[FSM_REMOTE]),
          sizeof(lcp_value_type));
  fsm_ptr->local.want = opts_ptr->lcp_info.want_mask[FSM_LOCAL];
  fsm_ptr->remote.want = opts_ptr->lcp_info.want_mask[FSM_REMOTE];
  fsm_ptr->local.will = opts_ptr->lcp_info.will_mask[FSM_LOCAL];
  fsm_ptr->remote.will = opts_ptr->lcp_info.will_mask[FSM_REMOTE];

  /*-------------------------------------------------------------------------
    Make sure that work is setup so that we negotiate for what we want.
  -------------------------------------------------------------------------*/
  fsm_ptr->local.work = fsm_ptr->local.want;
  fsm_ptr->remote.work = fsm_ptr->remote.want;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

} /* lcp_init() */



/*===========================================================================
FUNCTION LCP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the LCP option that is passed in. It is
   being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  lcp_init() and ipcp_init() should have been called before this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void lcp_save_work_opt
(
  ppp_fsm_side_type *opts,       /* pointer to where the values are stored */
  byte               opt_num,                         /* the option number */
  byte              *opt_val                    /* the value of the option */
)
{
  /*-------------------------------------------------------------------------
    Get a handle on the two LCP option storing structs.
  -------------------------------------------------------------------------*/
  lcp_value_type *work_opts = (lcp_value_type*)(opts->work_pdv);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Determine the option type
  -------------------------------------------------------------------------*/
  switch(opt_num)
  {
    case LCP_MRU: /* Max Receive Unit */
      work_opts->mru = ps_ntohs((*((uint16*)opt_val)));
      opts->work |= LCP_N_MRU;
      break;

    case LCP_ACCM: /* Async Char Control Mask */
      work_opts->accm = ps_ntohl((*((uint32*)opt_val)));
      opts->work |= LCP_N_ACCM;
      break;

    case LCP_AP: /* Authentication Protocol */
      work_opts->authentication = ps_ntohs((*((uint16*)opt_val)));
      opts->work |= LCP_N_AP;

      /*---------------------------------------------------------------------
        Save the chap-digest (1 byte) for CHAP protocol (RFC 1994)

        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |     Type      |    Length     |     Authentication-Protocol   |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |   Algorithm   |
        +-+-+-+-+-+-+-+-+
      ---------------------------------------------------------------------*/
      if( work_opts->authentication == PPP_CHAP_PROTOCOL )
      {
        work_opts->chap_digest = (*((uint8*)(opt_val + LCP_AP_LEN)));
        LOG_MSG_INFO2_1("CHAP negotiated, chap-digest 0x%x",
                        work_opts->chap_digest);
      }
      break;

    case LCP_QMP: /* quility monitoring protocol */
      work_opts->monitoring = ps_ntohs((*((uint16*)opt_val)));
      opts->work |= LCP_N_QMP;
      break;

    case LCP_MAGIC: /* Magic Number */
      work_opts->magic_number = ps_ntohl((*((uint32*)opt_val)));
      opts->work |= LCP_N_MAGIC;
      break;

    case LCP_PFC: /* Proto Field Compression */
      opts->work |= LCP_N_PFC;
      break;

    case LCP_ACFC: /* Address & Control Field Compression */
      opts->work |= LCP_N_ACFC;
      break;

    default:
      LOG_MSG_INFO2_1("Got unknown option %d - ignoring", opt_num);

  } /* switch(option) */

} /*  lcp_save_work_opt() */


/*===========================================================================
FUNCTION LCP_SETUP_DEVICE()

DESCRIPTION
  This function performs the necessary LCP configuration on the specified
  PPP device.

DEPENDENCIES
  LCP negotiation on the device must be complete and negotiated
  options/values must be stored in LCP control block.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_setup_device
(
  ppp_dev_enum_type device
)
{

  ppp_type             * ppp_cb_ptr = NULL;
  ppp_type             * ppp_cb_array = NULL;
  ppp_fsm_type         * fsm_ptr    = NULL;
  hdlc_ioctl_u_type      ioctl_data;
  hdlc_errno_enum_type   ps_errno;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1( "Invalid device passed %d", device );
    ASSERT( 0 );
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;
  fsm_ptr    = ppp_cb_ptr->fsm + LCP;

  /*-------------------------------------------------------------------------
    If ACCM was negotiated, set it for HDLC framing
  -------------------------------------------------------------------------*/
  if( fsm_ptr->remote.work & LCP_N_ACCM )
  {
    ioctl_data.accm =
        ((lcp_value_type*)fsm_ptr->remote.work_pdv)->accm;

    LOG_MSG_INFO2_2("lcp_setup_device (%d), setting HDLC ACCM = 0x%x",
                    device, ioctl_data.accm);
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS 
    if(ppp_cb_ptr->hdlc_mode == HDLC_A2_AUTONOMOUS)
    {
      pppi_a2_enable_hdlc_autonomous(ioctl_data.accm,
                                     ppp_hdlc_a2_autonomous_enable);      
    }
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */
    if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                    HDLC_IOCTL_SET_ACCM,
                    &ioctl_data,
                    &ps_errno
                  ) != 0 )
    {
      LOG_MSG_ERROR_2("Unable to set HDLC ACCM for dev %d, errno %d",
                      device, ps_errno);
    }
  } /* if ACCM was negotiated */

} /* lcp_setup_device() */


/*===========================================================================
FUNCTION LCP_MRU_GET()

DESCRIPTION
  This function is passed a PPP device and uses that to return the LCP MRU
  associated with the device.

RETURN VALUE
  -1 in case of error or if no MRU is set
  The MRU of the device in case of success

DEPENDENCIES
  LCP should be initialized before this function is called

SIDE EFFECTS
  None
===========================================================================*/
int32 lcp_mru_get
(
  ppp_dev_enum_type device
)
{
  ppp_fsm_type *fsm_ptr = NULL;
  int32         ret     = -1;
  ppp_type     *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify passed device
  -------------------------------------------------------------------------*/
  if( PPP_MAX_DEV <= device )
  {
    DATA_ERR_FATAL("Invalid device passed");
    return ret;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Get the LCP FSM pointer
    -----------------------------------------------------------------------*/
    ppp_cb_array = fsm_get_ppp_cb_array();
    fsm_ptr = &ppp_cb_array[device].fsm[LCP];

    /*-----------------------------------------------------------------------
      If MRU is negotiated then return it else return -1
    -----------------------------------------------------------------------*/
    if( fsm_ptr->remote.work & LCP_N_MRU )
    {
      ret = ((lcp_value_type*)(fsm_ptr->remote.work_pdv))->mru;
    }
  }

  return ret;
} /* lcp_mru_get() */



/*===========================================================================
FUNCTION LCP_MRU_SET()

DESCRIPTION
  This function is passed a PPP device and a value of MRU to set.  It uses
  this information to set the LCP MRU associated with the device.

RETURN VALUE
  -1 in case of error
   0 in case of success

DEPENDENCIES
  LCP should be initialized before this function is called

SIDE EFFECTS
  None
===========================================================================*/
int32 lcp_mru_set
(
  ppp_dev_enum_type device,
  int32             mru
)
{
  ppp_fsm_type *fsm_ptr = NULL;
  int32         ret     = -1;
  ppp_type     *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify passed device
  -------------------------------------------------------------------------*/
  if( PPP_MAX_DEV <= device )
  {
    DATA_ERR_FATAL("Invalid device passed");
    return ret;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Get the LCP FSM pointer
    -----------------------------------------------------------------------*/
    ppp_cb_array = fsm_get_ppp_cb_array();
    fsm_ptr = &ppp_cb_array[device].fsm[LCP];

    /*-----------------------------------------------------------------------
      Set the mru
    -----------------------------------------------------------------------*/
    ((lcp_value_type*)(fsm_ptr->remote.work_pdv))->mru = (uint16)mru;
    ret = 0;
  }

  return ret;
} /* lcp_mru_set() */



/*===========================================================================
FUNCTION LCP_SET_NET_MODEL_RESYNC_FNC()

DESCRIPTION
  This function sets the callback function pointer for the LCP established
  function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_set_net_model_resync_fnc
(
  net_model_resync_fptr_type fnc_ptr,
  ppp_dev_enum_type          device
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  lcp_net_model_resync_fnc[device] = fnc_ptr;

} /* lcp_set_net_model_resync_fnc() */



/*===========================================================================
FUNCTION LCP_UP()

DESCRIPTION
  This function is called when link layer configuration is completed and
  we are ready to move on to the network phase.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_up
(
  ppp_dev_enum_type device
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  if( NULL != lcp_net_model_resync_fnc[device] )
  {
    (void)lcp_net_model_resync_fnc[device](device);
  }

  return;
} /* lcp_up() */



/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION LCP_OPTION()

DESCRIPTION
  This function will add the given option to the DSM item passed in.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_option
(
  ppp_fsm_type    *fsm_ptr,
  void            *vp,
  dsm_item_type **item_head_ptr,
  uint8           opt_num
)
{
  ppp_type *ppp_cb_ptr = NULL;
  ppp_type *ppp_cb_array = NULL;
  lcp_value_type *value_p = NULL;
  dsm_item_type  *item_ptr = NULL;
  register uint8 *cp = NULL;
  int n_type   = 0;
  int opt_len;
  register int used;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(NULL == item_head_ptr || NULL == (*item_head_ptr))
  {
    DATA_ERR_FATAL("lcp_option(): One or more parameters null");
    return;
  }

  if(NULL == fsm_ptr || NULL == vp || NULL == (*item_head_ptr)->data_ptr)
  {
    dsm_free_packet(item_head_ptr);
    DATA_ERR_FATAL("lcp_option(): One or more parameters null");
    return;
  }

  LOG_MSG_INFO2_1("lcp_option: optnum = %d", opt_num);

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)fsm_ptr->device; 
  value_p = (lcp_value_type *)vp;
  item_ptr = *item_head_ptr;
  n_type   = fsm_n_option(fsm_ptr->pdc,opt_num);
  /*-------------------------------------------------------------------------
    this tests for the minimum & maximum option length
  -------------------------------------------------------------------------*/
  if((-1 == n_type) || (sizeof(option_length) <= n_type))
  {
    LOG_MSG_INFO2_1 ("lcp_option : Unrecognized option %d"
                     " can't include!", opt_num);
    dsm_free_packet(item_head_ptr);
    ASSERT(0);
    return;
  }

  opt_len  = option_length[n_type];
  used = opt_len - OPTION_HDR_LEN;

  cp = item_ptr->data_ptr + item_ptr->used;
  *cp++ = opt_num;
  /*-------------------------------------------------------------------------
    Note this length will be modified at the bottom if the option is longer
    than indicated by the default values - as it is if CHAP is enabled.
  -------------------------------------------------------------------------*/
  *cp++ = (uint8)opt_len;

  /*-------------------------------------------------------------------------
    Determine the option type
  -------------------------------------------------------------------------*/
  switch( opt_num )
  {
    case LCP_MRU: /* Max Receive Unit */
      (void)put16(cp, value_p->mru);
      used -= 2;
      break;

    case LCP_ACCM: /* Async Char Control Mask */
      (void)put32(cp, value_p->accm);
      used -= 4;
      break;

    case LCP_AP: /* Authentication Protocol */
      cp = put16(cp, value_p->authentication);
      used -= 2;

      switch(value_p->authentication)
      {
#ifdef FEATURE_DATA_PS_PPP_EAP
       case PPP_EAP_PROTOCOL:
         ppp_cb_ptr->auth.mode = PPP_DO_EAP;
         break;
#endif /*FEATURE_DATA_PS_PPP_EAP*/

       case PPP_CHAP_PROTOCOL:
        ppp_cb_ptr->auth.mode = PPP_DO_CHAP;
        /*-------------------------------------------------------------------
          Copy the CHAP digest. The digest algorithm number is appended after
          the CHAP protocol.
        -------------------------------------------------------------------*/
        *cp++ = value_p->chap_digest;
        used--;
         break;

       /* default case set the auth.mode to PPP_DO_PAP*/
       default:
        ppp_cb_ptr->auth.mode = PPP_DO_PAP;
         break;
      }
      break; /*End of case LCP_AP*/

    case LCP_MAGIC: /* Magic Number */
      (void)put32(cp, value_p->magic_number);
      used -= 4;
      break;

    case LCP_PFC: /* Proto Field Compression */
      break;

    case LCP_ACFC: /* Address & Control Field Compression */
      break;

    default:
      break;
  };

  opt_len                    -= used;
  /* length may be modified */
  item_ptr->data_ptr[item_ptr->used + 1]  = (uint8)opt_len;
  item_ptr->used                   += (uint16)opt_len;

} /* lcp_option() */



/*===========================================================================
FUNCTION LCP_CHECK()

DESCRIPTION
  This function will Check the options, updating the working values.

DEPENDENCIES
  None

RETURN VALUE
  Returns -1 if ran out of data, ACK/NAK/REJ as appropriate.

SIDE EFFECTS
  None
===========================================================================*/
static int lcp_check
(
  ppp_fsm_type      * fsm_ptr,
  struct option_hdr * ohp,
  dsm_item_type    ** item_ref_ptr,
  int                 request
)
{
  lcp_value_type     * lworkp = NULL;
  lcp_value_type     * rworkp  = NULL;
  ppp_fsm_side_type  * side_p = NULL;
  lcp_value_type     * s_want_ptr = NULL;
  lcp_value_type     * s_work_ptr = NULL;
  ppp_type           * ppp_cb_ptr = NULL;
  ppp_type           * ppp_cb_array = NULL;
  int                  used = 0;
  int                  option_result = CONFIG_ACK;   /* Assume good values */
  int                  n_type = 0;
  int16                rx_chap_digest; /* chap digest from peer            */
  int                  pullvar;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( item_ref_ptr == NULL )
  {
    LOG_MSG_ERROR_0("Invalid DSM in lcp_check");
    return -1;
  }

  if( fsm_ptr == NULL              ||
      ohp == NULL                  ||
      ohp->length < OPTION_HDR_LEN ||
      fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in lcp_check");
    dsm_free_packet(item_ref_ptr);
    return -1;
  }

  LOG_MSG_INFO2_1("lcp_check: opttype = %d", ohp->type);

  used   = ohp->length - OPTION_HDR_LEN;
  n_type = fsm_n_option(fsm_ptr->pdc,ohp->type);
#ifdef FEATURE_DATA_PKT_CLASSIFIER
  if (ds707_is_pkt_classifier_enabled() == FALSE) 
#endif/* FEATURE_DATA_PKT_CLASSIFIER */
  {
    lworkp = (lcp_value_type *)fsm_ptr->local.work_pdv;
    rworkp = (lcp_value_type *)fsm_ptr->remote.work_pdv;
  }
  side_p =
    (ppp_fsm_side_type *)(request ? &(fsm_ptr->remote) : &(fsm_ptr->local));
  s_want_ptr = (lcp_value_type *)side_p->want_pdv;
  s_work_ptr = (lcp_value_type *)side_p->work_pdv;
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);

  /*-------------------------------------------------------------------------
    see if we allow this option
  -------------------------------------------------------------------------*/
  if ((-1 == n_type) || (sizeof(option_length) <= n_type) ||
      (ohp->type > LCP_OPTION_LIMIT) ||
      !((side_p->will | side_p->want) & (1 << n_type)))
  {
    /*-----------------------------------------------------------------------
      toss any bytes in option
    -----------------------------------------------------------------------*/
    if (used != dsm_pullup( item_ref_ptr, NULL, used ))
    {
      return -1;
    }
    return CONFIG_REJ;

  } /* if(unsupported option) */

  /*-------------------------------------------------------------------------
    Check the option length.  The length is bad if it is less than the
    minimum length of the option or greater than the maximum length, except
    for AP options, which are checked separately.
  -------------------------------------------------------------------------*/
  if ((ohp->length < option_length[n_type]) ||
      (ohp->type != LCP_AP && ohp->length > option_length[n_type]))
  {
    /*-----------------------------------------------------------------------
      toss any bytes in option
    -----------------------------------------------------------------------*/
    if (used != dsm_pullup( item_ref_ptr, NULL, used ))
    {
      return -1;
    }
    LOG_MSG_ERROR_0("Invalid option length in lcp_check");
    return CONFIG_NAK;
  }

  switch(ohp->type)
  {
  case LCP_MRU: /* max receive unit */
    pullvar = dsm_pull16(item_ref_ptr);
    if(pullvar == -1 )
    {
      return -1;
    }
    s_work_ptr->mru = (uint16)pullvar;
    used -= 2;

    /*-----------------------------------------------------------------------
      Check if new value is appropriate
   -----------------------------------------------------------------------*/
    if(s_work_ptr->mru < LCP_MRU_LO)
    {
      s_work_ptr->mru   = LCP_MRU_LO;
      option_result = CONFIG_NAK;
    }
    else if (s_work_ptr->mru > LCP_MRU_HI)
    {
      s_work_ptr->mru = LCP_MRU_HI;
      option_result = CONFIG_NAK;
    }

    if(request                    &&
       (side_p->want & LCP_N_MRU) &&
       s_work_ptr->mru > s_want_ptr->mru)
    {
      /*---------------------------------------------------------------------
        send a smaller packet anyway
      -----------------------------------------------------------------------*/
      s_work_ptr->mru = s_want_ptr->mru;
    }
    break;

  case LCP_ACCM: /* async control character map */
    s_work_ptr->accm = dsm_pull32(item_ref_ptr);
    used -= 4;

    /*-----------------------------------------------------------------------
      Remote host may ask to escape more control characters than we require,
      but must escape at least the control chars that we require.
    -----------------------------------------------------------------------*/
    if ((side_p->want & LCP_N_ACCM) &&
        s_work_ptr->accm != (s_work_ptr->accm | s_want_ptr->accm))
    {
      s_work_ptr->accm |= s_want_ptr->accm;
      option_result = CONFIG_NAK;
    }
    break;

  case LCP_AP: /* Authentication protocol */
    /*-----------------------------------------------------------------------
      Processing for Authentication-Protocol (AP) option:
      The protocol suggested by the peer is accepted (if allowed), else
      the next allowed protocol is used. If there are no allowed auth
      protocols, the AP option is rejected.
    -----------------------------------------------------------------------*/
    pullvar = dsm_pull16(item_ref_ptr); /* get the protocol */
    if( pullvar == -1 )
    {
      return -1;
    }

    used -= 2;
    /*-----------------------------------------------------------------------
      Check if requested authentication protocol is same as the one requested 
      before PPP resync. If they are not same send NAK
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO1_0("PPP auth check (only for resync)");
    LOG_MSG_INFO1_3("use_same_auth %d, work->auth 0x%x, cur->auth 0x%x",
                    s_want_ptr->use_same_auth,
                    s_work_ptr->authentication,
                    pullvar);
    if((request) 
      && (TRUE == s_want_ptr->use_same_auth) 
      && ((PPP_PAP_PROTOCOL == s_work_ptr->authentication) 
          || (PPP_CHAP_PROTOCOL == s_work_ptr->authentication)
#ifdef FEATURE_DATA_PS_PPP_EAP
          || (PPP_EAP_PROTOCOL == s_work_ptr->authentication)
#endif /* FEATURE_DATA_PS_PPP_EAP */
      ))
    {
      if ((uint16)pullvar != s_work_ptr->authentication)
      {
        LOG_MSG_INFO1_2("C-NAKing, new(0x%x) & old(0x%x) auth protocols are not "
                        "same after PPP resync",
                        pullvar, s_work_ptr->authentication);
        option_result = CONFIG_NAK;
        break;        
      }
    }

    s_work_ptr->authentication = (uint16)pullvar;

    /*-----------------------------------------------------------------------
      Check if requested authentication protocol is PAP or CHAP or Unknown
    -----------------------------------------------------------------------*/
    switch (s_work_ptr->authentication)
    {
    case PPP_PAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Set up to do PAP (if allowed), else C-NAK with CHAP (if allowed),
        else C-NAK with EAP (if allowed), else C-Rej
      ---------------------------------------------------------------------*/
      if(request)
      {
      LOG_MSG_INFO2_0("Received Config. Req. with PAP option");
      }
      else
      {
        LOG_MSG_INFO2_0("Received Config. NAK with PAP option");
      }

      /*---------------------------------------------------------------------
        Check if the option length is less than the maximum PAP option length
      ---------------------------------------------------------------------*/
      if ( ohp->length > LCP_AP_PAP_OPTION_SIZE )
      {
        if (used != dsm_pullup( item_ref_ptr, NULL, used ))
        {
          return -1;
        }
        LOG_MSG_ERROR_0("Invalid option length in lcp_check");
        return CONFIG_NAK;
      }

      if( ( (uint16)s_want_ptr->auth_proto_mask ) & ( (uint16)PPP_AUTH_PAP ) )
      {
        ppp_cb_ptr->auth.mode = PPP_DO_PAP;
      }
      else if( ( (uint16)s_want_ptr->auth_proto_mask ) & 
               ( (uint16)PPP_AUTH_CHAP ) )
      {
        LOG_MSG_INFO2_0("C-NAKing PAP, proposing CHAP");
        s_work_ptr->authentication = PPP_CHAP_PROTOCOL;
        s_work_ptr->chap_digest    = PPP_CHAP_MD5;
        option_result              = CONFIG_NAK;
      }
#ifdef FEATURE_DATA_PS_PPP_EAP
      else if( s_want_ptr->auth_proto_mask & PPP_AUTH_EAP )
      {
        LOG_MSG_INFO2_0("C-NAKing PAP, proposing EAP");
        s_work_ptr->authentication = PPP_EAP_PROTOCOL;
        option_result              = CONFIG_NAK;
      }
#endif /*FEATURE_DATA_PS_PPP_EAP*/
      else
      {
        LOG_MSG_INFO1_2("Dev %d rejecting PPP auth option, auth_proto_mask = 0x%x",
                        fsm_ptr->device, s_want_ptr->auth_proto_mask);
        option_result = CONFIG_REJ;
      }
      break; /* PPP_PAP_PROTOCOL */

    case PPP_CHAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Set up to do CHAP (if allowed), else C-NAK with PAP (if allowed),
        else C-NAK with EAP (if allowed), else C-Rej
      ---------------------------------------------------------------------*/
      if(request)
      {
      LOG_MSG_INFO2_0("Received Config. Req. with CHAP option");
      }
      else
      {
        LOG_MSG_INFO2_0("Received Config. NAK with CHAP option");
      }

      /*---------------------------------------------------------------------
        Check if the option length is less than the maximum CHAP option length
      ---------------------------------------------------------------------*/
      if ( ohp->length > LCP_AP_CHAP_OPTION_SIZE )
      {
        if (used != dsm_pullup( item_ref_ptr, NULL, used ))
        {
          return -1;
        }
        LOG_MSG_ERROR_0("Invalid option length in lcp_check");
        return CONFIG_NAK;
      }

      if( ( (uint16)s_want_ptr->auth_proto_mask ) & ( (uint16)PPP_AUTH_CHAP ) )
      {
        /*-------------------------------------------------------------------
          Setting it to send CHAP pkts in auth. phase.
        -------------------------------------------------------------------*/
        ppp_cb_array[fsm_ptr->device].auth.mode  = PPP_DO_CHAP;

        /*-------------------------------------------------------------------
          In the incoming packet the CHAP protocol has algorithm field
          attached to it, i.e. C223 for CHAP and 05 for MD5 or 80 for MD4.
          Since the current implementation implements only MD5, check if the
          algorithm is MD5.

          Since we pulled out one more byte from the incoming packet reduce
          the used field by one.

          If the algorithm is not MD5, then send a Config NAK to the base
          station and request for the MD5.
        -------------------------------------------------------------------*/
        rx_chap_digest = dsm_pull8(item_ref_ptr);
        if(rx_chap_digest == -1)
        {
          return -1;
        }
        used--;
        if((s_work_ptr->chap_digest = (uint8)rx_chap_digest) != PPP_CHAP_MD5)
        {
          s_work_ptr->chap_digest = PPP_CHAP_MD5;
          option_result           = CONFIG_NAK;
        }
        LOG_MSG_INFO2_2("dev %d got C-req with chap digest: %d",
                        fsm_ptr->device, rx_chap_digest);
      }
      else if( ( (uint16)s_want_ptr->auth_proto_mask ) & 
               ( (uint16)PPP_AUTH_PAP ) )
      {
        LOG_MSG_INFO2_0("C-NAKing CHAP, proposing PAP");
        s_work_ptr->authentication = PPP_PAP_PROTOCOL;
        option_result              = CONFIG_NAK;
      }
#ifdef FEATURE_DATA_PS_PPP_EAP
      else if( s_want_ptr->auth_proto_mask & PPP_AUTH_EAP )
      {
        LOG_MSG_INFO2_0("C-NAKing CHAP, proposing EAP");
        s_work_ptr->authentication = PPP_EAP_PROTOCOL;
        option_result              = CONFIG_NAK;
      }
#endif /*FEATURE_DATA_PS_PPP_EAP*/
      else
      {
        LOG_MSG_INFO1_2("Dev %d rejecting PPP auth option, auth_proto_mask = 0x%x",
                        fsm_ptr->device, s_want_ptr->auth_proto_mask);
        option_result = CONFIG_REJ;
      }
      break; /* PPP_CHAP_PROTOCOL */

#ifdef FEATURE_DATA_PS_PPP_EAP
    case PPP_EAP_PROTOCOL:
      if(request)
      {
        LOG_MSG_INFO2_0("Received Config Req with EAP option");
      }
      else
      {
        LOG_MSG_INFO2_0("Received Config NAK with EAP option");
      }

      /*-------------------------------------------------------------------
        Set up to do EAP (if allowed), else C-Rej
      -------------------------------------------------------------------*/
      if( ohp->length > LCP_AP_EAP_OPTION_SIZE )
      {
        if(used != dsm_pullup( item_ref_ptr, NULL, used ))
        {
          return -1;
        }
        LOG_MSG_ERROR_0("Invalid option length in lcp_check");
        return CONFIG_REJ;
      }

      if( s_want_ptr->auth_proto_mask & PPP_AUTH_EAP )
      {
        /*-------------------------------------------------------------------
           Set Up to do EAP
        -------------------------------------------------------------------*/
        ppp_cb_ptr->auth.mode = PPP_DO_EAP;
      }
      /*-------------------------------------------------------------------
        In eHRPD mode, reject any other Auth protocol except
        EAP.
      -------------------------------------------------------------------*/
      else
      {
        LOG_MSG_INFO1_2("Dev %d rejecting PPP auth option, auth_proto_mask = 0x%x",
                        fsm_ptr->device, s_want_ptr->auth_proto_mask);
        option_result = CONFIG_REJ;
      }
      break; /*PPP_EAP_PROTOCOL*/
#endif /*FEATURE_DATA_PS_PPP_EAP*/


    default:
      /*---------------------------------------------------------------------
        Got an unrecognized authentication protocol.
        Send a C-NAK asking for CHAP/PAP if allowed, else send a C-Rej
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("Rxd Unknown Auth proto:0x%x ",
                      s_work_ptr->authentication);
      if( ( (uint16)s_want_ptr->auth_proto_mask ) & ( (uint16)PPP_AUTH_CHAP ) )
      {
        LOG_MSG_INFO2_0("C-NAKing Unknown auth proto, proposing CHAP");
        s_work_ptr->authentication = PPP_CHAP_PROTOCOL;
        s_work_ptr->chap_digest    = PPP_CHAP_MD5;
        option_result              = CONFIG_NAK;
      }
      else if( ( (uint16)s_want_ptr->auth_proto_mask ) & 
               ( (uint16)PPP_AUTH_PAP ) )
      {
        LOG_MSG_INFO2_0("C-NAKing Unknown auth proto, proposing PAP");
        s_work_ptr->authentication = PPP_PAP_PROTOCOL;
        option_result              = CONFIG_NAK;
      }
      else
      {
        LOG_MSG_INFO1_2("Dev %d rejecting PPP auth option, auth_proto_mask = 0x%x",
                        fsm_ptr->device, s_want_ptr->auth_proto_mask);
        option_result = CONFIG_REJ;
      }
      break; /* default */
    } /* switch(auth type in req) */
    break; /* case LCP_AP */

  case LCP_MAGIC: /* magic number */
    s_work_ptr->magic_number = dsm_pull32(item_ref_ptr);
    used -= 4;
#ifdef FEATURE_DATA_PKT_CLASSIFIER
    if (ds707_is_pkt_classifier_enabled() == FALSE) 
#endif/* FEATURE_DATA_PKT_CLASSIFIER */
    {
      /*-----------------------------------------------------------------------
        Ensure that magic numbers are different
      -----------------------------------------------------------------------*/
      if ( s_work_ptr->magic_number == 0L )
      {
        s_work_ptr->magic_number = (uint32)s_work_ptr;
        option_result = CONFIG_NAK;
      }
      else if ( rworkp->magic_number == lworkp->magic_number )
      {
        s_work_ptr->magic_number += msclock();
        option_result = CONFIG_NAK;
      }
    }
    break;

  case LCP_PFC:  /* protocol field compression */
  case LCP_ACFC: /* addr/control field compression */
    /* nothing to store here */
    break;

  default:
    option_result = CONFIG_REJ;
    break;
  } /* switch(option type) */

  if ( used < 0 )
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    if there are extra bytes in option toss them.
  -------------------------------------------------------------------------*/
  if ( used > 0 )
  {
    while( used-- > 0 )
    {
      if ( dsm_pull8(item_ref_ptr) == -1 )
      {
        return -1;
      }
    }
  }

  return (option_result);

} /* lcp_check() */



/*===========================================================================
FUNCTION LCP_REJECT()

DESCRIPTION
  This function will process configuration reject sent by remote host -
  currently doesn't do anything.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int lcp_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *opt_hdr_ptr,
  dsm_item_type     *bp
)
{
  ppp_type *ppp_cb_ptr = (ppp_type*)fsm_ptr; /* assumes fsm is first!      */
  int option_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (-1 == (option_index = fsm_n_option(fsm_ptr->pdc, opt_hdr_ptr->type)))
  {
    LOG_MSG_INFO2_3( "lcp_reject(): dev %d, fsm %d, unrecognized option %d",
             fsm_ptr->device, fsm_ptr->pdc->fsmi, opt_hdr_ptr->type);
    return -1;
  }

  fsm_ptr->local.work &= ~(unsigned int)(1 << option_index);

  /*-------------------------------------------------------------------------
    if the option that was rejected is authentication - then set the auth
    mode to NO_AUTH.  In all other cases do nothing.
  -------------------------------------------------------------------------*/
  if(opt_hdr_ptr->type == LCP_AP)
  {
    ppp_cb_ptr->auth.mode = PPP_NO_AUTH;
  }

  return 0;
} /* lcp_reject() */



/*===========================================================================
FUNCTION LCP_STARTING()

DESCRIPTION
  This function will Prepare to begin configuration exchange

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_starting
(
  ppp_fsm_type *fsm_ptr
)
{
  ppp_type            * ppp_cb_ptr = NULL;
  ppp_type            * ppp_cb_array = NULL;
  hdlc_ioctl_u_type     ioctl_data;
  hdlc_errno_enum_type  ps_errno;
  lcp_value_type      * lwant_ptr = NULL;
  lcp_value_type      * lwork_ptr = NULL;
  lcp_value_type      * rwant_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( fsm_ptr == NULL ||
      fsm_ptr->local.want_pdv == NULL  ||
      fsm_ptr->local.work_pdv == NULL  ||
      fsm_ptr->remote.want_pdv == NULL ||
      fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in lcp_starting");
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);
  lwant_ptr = (lcp_value_type*)fsm_ptr->local.want_pdv;
  lwork_ptr = (lcp_value_type*)fsm_ptr->local.work_pdv;
  rwant_ptr = (lcp_value_type*)fsm_ptr->remote.want_pdv;

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_STARTING event
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP,
                             fsm_ptr->device,
                             PPP_PROTOCOL_LCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_STARTING );

  ppp_cb_ptr->phase = pppESTABLISH;

  fsm_ptr->local.work = fsm_ptr->local.want;

  /*-------------------------------------------------------------------------
    Set the magic number
  -------------------------------------------------------------------------*/
  lwant_ptr->magic_number = (uint32)&lcp_accept + msclock();
  lwork_ptr->magic_number = lwant_ptr->magic_number;

  fsm_ptr->remote.work = FALSE;

  /*-------------------------------------------------------------------------
    Set the ACCM to the default HDLC-framing since LCP is starting
  -------------------------------------------------------------------------*/
  ioctl_data.accm = LCP_ACCM_DEFAULT;
  LOG_MSG_INFO2_2("lcp_starting, setting HDLC ACCM = 0x%x for dev %d",
                  ioctl_data.accm, fsm_ptr->device);
  if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                  HDLC_IOCTL_SET_ACCM,
                  &ioctl_data,
                  &ps_errno
                ) != 0 )
  {
    LOG_MSG_ERROR_2("Unable to set HDLC ACCM for dev %d, errno %d",
                    fsm_ptr->device, ps_errno);
  }

  /*-------------------------------------------------------------------------
    Set up the auth_proto_mask bitmask.
    For old clients (which don't set this bitmask in PPP config), if
    req_encr is set, enable CHAP (and any other encrypted protocols.), else,
    enable all protocols.
    For new clients (which do set this bitmask in PPP config), if req_encr
    is set, disable PAP (and any non-encrypted protocols)

    TODO: Checking for require_encrypted_passwd and appropriately setting the
    auth_proto_mask bitmask can be moved to the clients. Once that
    happens, this code can be removed, and appropriate checks inserted in
    pppi_config_check.
  -------------------------------------------------------------------------*/
  if( PPP_AUTH_DEFAULT == rwant_ptr->auth_proto_mask ) /* old clients */
  {
#ifdef FEATURE_DATA_PS_PPP_EAP
    if(ppp_cb_ptr->auth.info.eap_user_id_len > 0)
    {
      rwant_ptr->auth_proto_mask = (ppp_auth_proto_e_type)PPP_AUTH_EAP;
    }
    else
#endif /*FEATURE_DATA_PS_PPP_EAP*/
    {
      if( TRUE == ppp_cb_ptr->auth.require_encrypted_passwd )
      {
        rwant_ptr->auth_proto_mask = PPP_AUTH_CHAP;
      }
      else
      {
        rwant_ptr->auth_proto_mask = 
        (ppp_auth_proto_e_type)((uint16)PPP_AUTH_PAP | (uint16)PPP_AUTH_CHAP);
      }
    }
  }
  else /* new clients */
  {
#ifdef FEATURE_DATA_PS_PPP_EAP
    if(rwant_ptr->auth_proto_mask & PPP_AUTH_EAP)
    {
      rwant_ptr->auth_proto_mask =(ppp_auth_proto_e_type)(PPP_AUTH_EAP);
    }
    else
#endif/*FEATURE_DATA_PS_PPP_EAP*/
    {
      if( TRUE == ppp_cb_ptr->auth.require_encrypted_passwd )
      {
        rwant_ptr->auth_proto_mask =
        (ppp_auth_proto_e_type)( (uint16)( rwant_ptr->auth_proto_mask)
                                 & ( ~(uint16)PPP_AUTH_PAP ) );
      }
    }
  }
#ifdef FEATURE_DATA_PS_PPP_EAP
  if(rwant_ptr->auth_proto_mask != PPP_AUTH_EAP )
  {
#endif /*FEATURE_DATA_PS_PPP_EAP*/
  /*----------------------------------------------------------------------
    Disable CHAP if we have a zero length password and no support for
    a CHAP response from RUIM.
  ----------------------------------------------------------------------*/
  if( ppp_cb_ptr->auth.info.passwd_len == 0 )
  {
    rwant_ptr->auth_proto_mask =  
      (ppp_auth_proto_e_type)( (uint16)(rwant_ptr->auth_proto_mask) 
                               & (~ (uint16)PPP_AUTH_CHAP ) );

#ifdef FEATURE_HDR_AN_AUTH
        if(fsm_ptr->device == PPP_UM_AN_DEV)
        {
           if(UIM_AN_HRPD_NO_SUPPORT != (uim_an_hrpd_support_status)uim_an_hrpd_support())
           {
              rwant_ptr->auth_proto_mask |= PPP_AUTH_CHAP;
           }
        }
        else /* non-AN devices */
#endif /* FEATURE_HDR_AN_AUTH*/
        {
           if((UIM_3GPD_MIP_NV_SIP_NV != (uim_3gpd_support_status)uim_3gpd_support() &&
               UIM_3GPD_MIP_RUIM_SIP_NV != (uim_3gpd_support_status)uim_3gpd_support()))
           {
              rwant_ptr->auth_proto_mask |= PPP_AUTH_CHAP;
           }
       }
  } /* pwd len is zero */
#ifdef FEATURE_DATA_PS_PPP_EAP
  }/*Auth Protocol is not EAP*/
#endif /*FEATURE_DATA_PS_PPP_EAP*/
  LOG_MSG_INFO2_2("lcp_starting, dev (%d) auth_proto_mask 0x%x",
                  fsm_ptr->device, rwant_ptr->auth_proto_mask);

} /* lcp_starting() */


/*===========================================================================
FUNCTION LCP_OPENING()

DESCRIPTION
  This function will be called after configuration negotiation is complete
  and PPP completes the Establish phase. It can go into the network or the
  authentication phase after that.

 If the network negotiated authentication or we need to authenticate the
 peer, then ppp will transition to the authentication phase. Otherwise, PPP
 will transition to the network phase without doing authentication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_opening
(
  ppp_fsm_type *fsm_ptr /* ptr to a FSM structure */
)
{
  ppp_type            * ppp_cb_ptr = NULL;
  ppp_type            * ppp_cb_array = NULL;
  hdlc_ioctl_u_type     ioctl_data;
  hdlc_errno_enum_type  ps_errno;
  ppp_event_auth_proto_payload_type auth_negotiated;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(fsm_ptr == NULL || fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid fsm_ptr in lcp_opening");
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);
  LOG_MSG_INFO2_1("dev %d LCP Up!", fsm_ptr->device);

  /*-------------------------------------------------------------------------
    Delay transition to next phase to allow remote ACCM to settle
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->set_stabilize_accm_delay == TRUE )
  {
    (void)pppi_discard(fsm_ptr->device, NULL);
  }

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_OPEN event
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP,
                             fsm_ptr->device,
                             PPP_PROTOCOL_LCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_OPEN );

  /*-------------------------------------------------------------------------
    If ACCM was negotiated, set it for HDLC framing
  -------------------------------------------------------------------------*/
  if( fsm_ptr->remote.work & LCP_N_ACCM )
  {
    ioctl_data.accm =
        ((lcp_value_type*)fsm_ptr->remote.work_pdv)->accm;

    LOG_MSG_INFO2_1("LCP negotiation finished, setting HDLC ACCM = 0x%x",
                    ioctl_data.accm);
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS 
    if(ppp_cb_ptr->hdlc_mode == HDLC_A2_AUTONOMOUS)
    {
      pppi_a2_enable_hdlc_autonomous(ioctl_data.accm,
                                     ppp_hdlc_a2_autonomous_enable);      
    }
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */
    if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                    HDLC_IOCTL_SET_ACCM,
                    &ioctl_data,
                    &ps_errno
                  ) != 0 )
    {
      LOG_MSG_ERROR_2("Unable to set HDLC ACCM for dev %d, errno %d",
                      fsm_ptr->device, ps_errno);
    }
  } /* if ACCM was negotiated */

  auth_negotiated.ppp_event_device = fsm_ptr->device;

  /*-------------------------------------------------------------------------
    If AP was not negotiated:
      1. For EAP on EHRPD, auth is mandated, terminate all NCPs (ppp_ncp_stop)
      2. Else set auth mode to PPP_NO_AUTH
  -------------------------------------------------------------------------*/
  if( !(ppp_cb_ptr->fsm[LCP].remote.work & LCP_N_AP) &&
      !(ppp_cb_ptr->fsm[LCP].local.work & LCP_N_AP)
    )
  {
#if defined (FEATURE_DATA_PS_EHRPD) && defined (FEATURE_DATA_PS_ENFORCE_AUTH)
    if( ( ppp_cb_ptr->fsm[LCP].remote.want & LCP_N_AP ) &&
        ( ( (lcp_value_type*)fsm_ptr->remote.want_pdv )->auth_proto_mask & \
           PPP_AUTH_EAP ) &&
        ( ppp_cb_ptr->allow_no_auth == FALSE ) )
    {
      LOG_MSG_ERROR_0("Auth reqd, but not negotiated during LCP. Closing LCP");
      ppp_cb_ptr->fsm[LCP].fail_reason = PPP_FAIL_REASON_OPTION_MISMATCH;

      fsm_close(fsm_ptr);
      if( fsm_ptr->state != fsmCLOSING )
      {
        fsm_ptr->mode = PPP_CP_MODE_INVALID;
      }

      /*----------------------------------------------------------------------
        Diag Event indicating the authentication prototcol negotiated
        during LCP.
      ----------------------------------------------------------------------*/
      auth_negotiated.ppp_event_auth = PPP_EVENT_AUTH_NONE;
      event_report_payload( EVENT_AUTH_PROTO,
                            sizeof(auth_negotiated),
                            &auth_negotiated );
      return;
    }
    else
#endif /* FEATURE_DATA_PS_EHRPD && FEATURE_DATA_PS_ENFORCE_AUTH */
    {
      ppp_cb_ptr->auth.mode = PPP_NO_AUTH;
    }
  }

  /*----------------------------------------------------------------------
    Diag Event indicating the authentication prototcol negotiated
    during LCP.
  ----------------------------------------------------------------------*/
  auth_negotiated.ppp_event_auth =
    (ppp_event_auth_proto_type)PS_EVENT_GET_AUTH_PROT_FRM_AUTH_MODE
      ( ppp_cb_ptr->auth.mode );
  if( auth_negotiated.ppp_event_auth != PPP_EVENT_AUTH_INVALID )
  {
    event_report_payload( EVENT_AUTH_PROTO,
                          sizeof(auth_negotiated),
                          &auth_negotiated );
  }

  /*-------------------------------------------------------------------------
    kick off authentication phase
  -------------------------------------------------------------------------*/
  ppp_auth_start(fsm_ptr->device);

} /* lcp_opening() */


/*===========================================================================
FUNCTION LCP_CLOSING()

DESCRIPTION
  This function will Close higher levels in preparation for link shutdown

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_closing
(
  ppp_fsm_type *fsm_ptr
)
{
  ppp_type     * ppp_cb_ptr = NULL;
  ppp_type     * ppp_cb_array = NULL;
  int i;
  ppp_fsm_type *loop_fsm_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(fsm_ptr == NULL || fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid fsm_ptr in lcp_closing");
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);

  /*-------------------------------------------------------------------------
    Free the old EAP authentication instance if PPP resync happen during 
    EAP authentication phase
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_PPP_EAP
  if(ppp_cb_ptr->phase == pppAUTHENTICATE &&
     ppp_cb_ptr->auth.ps_eap_handle != EAP_INVALID_HANDLE)
  { 
    LOG_MSG_INFO2("PPP closed when authentication is not finished."
                   "Free the old EAP handle",0,0,0);
    eap_delete_instance(ppp_cb_ptr->auth.ps_eap_handle);
    ppp_cb_ptr->auth.ps_eap_handle = EAP_INVALID_HANDLE;
  }
#endif /*FEATURE_DATA_PS_PPP_EAP*/
  ppp_cb_ptr->phase = pppTERMINATE;

  /*-------------------------------------------------------------------------
    Bring Down IPCP, IPV6CP.
  -------------------------------------------------------------------------*/
  for(i = LCP + 1; i < fsmi_Size; i++)
  {
    loop_fsm_ptr = &(ppp_cb_ptr->fsm[i]);

    if(loop_fsm_ptr->pdc != NULL && loop_fsm_ptr->pdc->down != NULL)
    {
      (*loop_fsm_ptr->pdc->down)(loop_fsm_ptr);
    }
  }
#ifdef FEATURE_DATA_PS_EHRPD

  /*-------------------------------------------------------------------------
    Bring Down the active NCPs. (VSNCPs for now)
  -------------------------------------------------------------------------*/
  ncpi_tear_down_all_ncps (fsm_ptr->device);
  
  /*-------------------------------------------------------------------------
    Abort the link status determination process if currently going on
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->lsd.config.result_cback != NULL )
  {
    LOG_MSG_INFO2_1("dev %d aborting link status determination",
                    fsm_ptr->device);
    ps_timer_free( ppp_cb_ptr->lsd.timer );
    memset( &ppp_cb_ptr->lsd.config, 0, sizeof( ppp_lsd_config_type ) );
  }
#endif /*FEATURE_DATA_PS_EHRPD*/
} /* lcp_closing() */


/*===========================================================================
FUNCTION LCP_STOPPING()

DESCRIPTION
  This function is called when a Term-Req packet is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_stopping
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_CLOSING event
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP,
                             fsm_ptr->device,
                             PPP_PROTOCOL_LCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSING );

  /*-------------------------------------------------------------------------
    Call ppp_abort() to do final clean up.
  -------------------------------------------------------------------------*/
  (void)ppp_abort(fsm_ptr->device);

} /* lcp_stopping() */


/*===========================================================================
FUNCTION LCPI_TIMER_CB()

DESCRIPTION
  This function handles events for an LCP timeout.  As the same timer is used
  for authentication this must handle that case also.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcpi_timer_cb
(
  void *arg_ptr
)
{
  ppp_type *ppp_cb_ptr = (ppp_type*)arg_ptr;
  ppp_dev_enum_type device = ppp_cb_ptr->fsm[LCP].device;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("dev %d LCP Timer Expired state=%d",
                  device, ppp_cb_ptr->fsm[LCP].state);

  /*-------------------------------------------------------------------------
    LCP/Auth timer has expired, check whether the timer expired during Auth
    or LCP.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->phase == pppAUTHENTICATE)
  {
    if(ppp_auth_timeout(ppp_cb_ptr) == FALSE)
    {
      LOG_MSG_INFO2_1("dev %d auth timed out - quitting!",
                      ppp_cb_ptr->fsm[LCP].device);
    }
  } /* if(auth phase) */
  else
  /*-------------------------------------------------------------------------
    It is in LCP establish phase - just call timeout as that will take the
    appropriate action (i.e. lcp_stopping() will be called and the call will
    be torn down).
  -------------------------------------------------------------------------*/
  {
    if(fsm_timeout(&(ppp_cb_ptr->fsm[LCP])) == FALSE)
    {
      if (ppp_cb_ptr->phase != pppTERMINATE)
      {
        LOG_MSG_INFO2_1("LCP on Dev %d failed to establish!", device);
  
        if( PPP_FAIL_REASON_NONE == ppp_cb_ptr->fsm[LCP].fail_reason )
        {
          ppp_cb_ptr->fsm[LCP].fail_reason = PPP_FAIL_REASON_TIMEOUT;
        }
      }

      /*-------------------------------------------------------------------
        JCDMA requires that an LCP Term req is sent in this case.
      -------------------------------------------------------------------*/
      if( ppp_cb_ptr->ppp_failure_graceful_close == TRUE )
      {
        (void)ppp_stop(device, PPP_PROTOCOL_LCP);
      }
      else
      {
      /*-------------------------------------------------------------------
        Abort - no response from peer so no point ins sending a Term Req
      -------------------------------------------------------------------*/
        (void)ppp_abort(device);
      }
    }
  }
} /* lcpi_timer_cb() */



/*===========================================================================
FUNCTION LCP_FREE()

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_free
(
  ppp_fsm_type *fsm_ptr
)
{
  /* nothing to do */

} /* lcp_free() */


/*===========================================================================
FUNCTION LCP_GET_LOCAL_WANT_CB_VALS()

DESCRIPTION
  Access function LCP local want control block
 
PARAMETERS
  Index: Array index in LCP local want control block array.
 
RETURN VALUE
  LCP local want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_local_want_cb_vals
( 
  uint32 index 
)
{
  return &lcp_local_want_cb[index];
}

/*===========================================================================
FUNCTION LCP_GET_LOCAL_WORK_CB_VALS()

DESCRIPTION
  Access function LCP local work control block
 
PARAMETERS
  Index: Array index in LCP local work control block array.
 
RETURN VALUE
  LCP local work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_local_work_cb_vals
( 
  uint32 index 
)
{
  return &lcp_local_work_cb[index];
}

/*===========================================================================
FUNCTION LCP_GET_REMOTE_WANT_CB_VALS()

DESCRIPTION
  Access function LCP remote want control block
 
PARAMETERS
  Index: Array index in LCP remote want control block array.
 
RETURN VALUE
  LCP remote want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_remote_want_cb_vals
( 
  uint32 index 
)
{
  return &lcp_remote_want_cb[index];
}

/*===========================================================================
FUNCTION LCP_GET_REMOTE_WORK_CB_VALS()

DESCRIPTION
  Access function LCP remote work control block
 
PARAMETERS
  Index: Array index in LCP remote work control block array.
 
RETURN VALUE
  LCP remote work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_remote_work_cb_vals
( 
  uint32 index 
)
{
  return &lcp_remote_work_cb[index];
}

/*===========================================================================
FUNCTION LCP_GET_DEFAULT()

DESCRIPTION
  Access function LCP default values.
 
PARAMETERS
  None
 
RETURN VALUE
  LCP default values

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_value_type* lcp_get_default_opts_vals_ptr(void)
{
  return &lcp_default;
}

/*===========================================================================
FUNCTION LCP_GET_ACCEPT()

DESCRIPTION
  Access function LCP accept bitmask.
 
PARAMETERS
  None
 
RETURN VALUE
  LCP accept bitmask.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
negotiate_t lcp_get_accept_mask(void)
{
  return lcp_accept;
}

