/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ P P P _ I P C P . C

GENERAL DESCRIPTION
  Code that supports IPCP negotiation.

EXTERNALIZED FUNCTIONS
  ipcp_save_work_opt()
    This function stores the value of the IPCP option that is passed in. It
    is being saved into the work and work_pdv the want values are saved by
    the caller.
  ipcp_set_net_model_resync_fnc()
    This function sets the callback function pointer for the IPCP established
    function.
  ipcp_ppp_in_resync()
    This function returns the state of the ipcp_ppp_resync_fnc_ptr variable
  ipcp_init()
    This function will Initialize the IPCP configuration structure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 1992-1994 William Allen Simpson
  Licensed to Qualcomm -- 95 Jan 21

  Acknowledgements and correction history may be found in PPP.C

Copyright (c) 1995-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_ipcp.c_v   1.13   13 Feb 2003 14:06:22   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_ipcp.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/14/13    cx     Fix LLVM warnings 
06/20/13    ss     Adding critical section protection to SLHC API invocation.
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
06/21/13    ash    Support MTU upto 2000
05/19/11    ssh    Examine compress bit before adding compress sub-option
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
01/16/11    ss     Added some log messages in ipcp_option/check functions.
08/24/10    ms     Set DNS addresses to default in UP state if they are empty.
06/18/10    jee    Klocwork fixes
06/16/10    ssh    Fixed memory leak, caused due to shifting to dynamic
                   memory allocation in SLHC
03/03/10    pp     Fixed SLHC memory leak. Added IPCP_SLHC_INIT local macro.
10/16/09    mga    Made changes to use send_options_mask
08/31/09    mga    Merged from eHRPD branch
07/14/09    pp     ppp_cb_lock changes.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
05/07/09    pp     CMI Phase-4: SU Level API Effort.
05/06/09    ss     SLHC De-coupling effort.
02/19/09    am     DS Task De-coupling effort.
11/10/08    scb    Fixed QTF compilation warnings.
09/02/08    rnp    Added STA related changes.
05/21/08    scb    Klocwork fixes
05/04/08    scb    Bug fixes caught during PPP STA development
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
12/22/07    scb    Reordered initialization of variables in ipcp_check.
05/14/07    scb    Changed variable names to avoid compilation error for
                   JCDMA_1X builds
04/27/07    scb    Removed LINT suppressions
04/19/07    scb    Added a KDDI PPP compliance requirement to configure
                   Application specific LCP constants
04/12/07    ssh    Disable DNS negotiation for resyncs (JCDMA)
03/23/07    ssh    Moved posting of STARTING event to core FSM
02/27/07    scb    Fixed High LINT errors
01/08/07    ssh    Posting IPCP starting event
10/18/06    ssh    Fixed bug in ipXcp_reject; also fixed setting of
                   compression bit in ipXcp_save_work_opt
07/13/06    ssh    Removed TASKLOCK in ipXcp_opening()
06/27/06    ssh    Setting appropriate compression bit in ipXcp_save_work_opt
04/28/06    ifk    Added PPP critical sections.
04/25/06    msr    L4/Tasklock code review changes
03/05/06    ifk    Moved clearing of option from work mask on receiving
                   a reject into ipcp_reject().  Special handling for
                   compression protocol option.
02/06/06    ssh    Changed TASKLOCKs to critical sections (for L4 builds).
12/15/05    msr    Fixed compilation warnings
12/09/05    sv     Added support for new data path framework.
11/28/05  ifk/ssh  Added support for IPHC and ROHC over PPP under
                   FEATURE_DATA_PS_PPP_IPHC and FEATURE_DATA_PS_PPP_ROHC
11/22/05    ssh    Added function ipcpi_iface_config() for configuring iface
11/05/05    rt     Fixed coverity errors.
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
04/14/05    ifk    Post PPP_DEV_DOWN to snoop when IPCP goes down.
03/25/05    ifk    Fixed fsm_n_option() handling
03/22/05    ssh    Now using new function pppi_report_event_to_diag()
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
02/10/05    ifk    Take out calling of IPCP up when mode is disabled in
                   ipcp_start()
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
12/22/04    rr/ks  Added option_length for primary and secondary nbns address
11/01/04    rr/ks  Added WINS support.
10/13/04    vp     Removed the byte order conversion of addresses as every IP
                   addresses is stored and dealt with in network order.
09/09/04    ifk    Setting of PPP phase to pppNETWORK moved to
                   ppp_auth_complete()
08/18/04    mvl    Some minor cleanup
07/30/04    vp     Changes due to consolidation of MTU and net_info structure
                   in ps_iface.
07/16/04    ifk    Renamed values of cp_mode_type enum
07/07/04    ifk    Removed dsm_kind field from fsm constants
07/05/04    ifk    Use lcp_mru_get() to set the MTU.
06/16/04    ifk    Event notification changed to use new architecture.
06/16/04    jd     Don't call fsm_close to tear down PPP if resync callback
                   returns false - mode controller will take care of this.
06/11/04    vp     Changes for representation of IP addresses as struct ps_in_addr.
06/03/04    ifk    In ipcpi_timer_cb() call ppp_stop() to prevent other NCP
                   being torn down.  Also pass fsm_ptr as argument.
05/05/04    usb    Added ipcp_setup_device to configure device for IP.
04/30/04    usb    Storing peer addr in iface instead of the callback func.
04/30/04    ifk    Post IPCP up event to snoop from ipcp_opening()
04/16/04    ifk    Initialized remote_initiate for ipcp at powerup, in
                   ipcp_stopping() set mode to INVALID.
04/16/04    jd     Check resync function completion status in ipcp_opening,
                   close fsm if resync callback returns failure.
04/08/04    ifk    In ipcp_opening also go through if NCP mode is passive.
03/15/04    ifk    Add call to PPP_DOWN_EV in ipcp_stopping().  In
                   ipcp_opening() only proceed if the FSM is actively opening.
02/24/04    mvl    Added JCDMA Term Req behaviour rather than Abort().
10/25/03    ifk    Added ipcp_set_device_constants(),
                   ipcp_device_constants[]
10/01/03    ifk    Removed extraneous memsets in ipcp_init().
09/16/03    usb    Moved fsm_init() to pppi_start() to handle mobile
                   initiated resync
09/10/03    aku    Changed timeout message to LOG_MSG_INFO1.
08/13/03    ifk    Sperated from LCP negotiation in preparation for IP6CP.
04/02/03    mvl    Complete modularization of the authentication code.
03/27/03    mvl    Added comments to the ipcp_constants data structure.
                   Re-ordered the functions to separate the internal from
                   external.
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added IPCP events posting.
02/11/03    usb    Initializing slhc for each device
02/06/03    mvl    Correctly storing number of VJ slots negotiated during
                   call setup of netmodel call.  Removed check if resyncing
                   when timer expires and removed function to check for
                   resync as it is no longer used.
02/06/03    usb    fixed compilation error byremoving LOCAL from variables
                   used outside this file
11/14/02    jd     removed check if (!MIP) before setting negotiated value
                   to iface IP addr - should always happen if negotiated
09/19/02    mvl    removed ipcp_set_ip_addr_opt().
09/18/02    aku    Removed global Ip_addr.
09/18/02    mvl    Added user_data to ppp event callbacks.  Added explicit
                   configuration for client vs. server behavior.  Removed
                   do_mip.
09/12/02    mvl    mainlined some netmodel code, generified the network model
                   resync callcack.  Removed netmodel featurization.
09/06/02    mvl    fixed assert()s.
09/02/02    mvl    PPP now registering a TX function with ps_iface when IPCP
                   is up.
08/13/02    mvl    Session handle support.
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
07/25/02    mvl    Removed netmodel dependency.  Updates for PPP renaming.
07/15/02    aku    Added inlude for pstimer.h
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Modified ipcp_opening() to do IPCP comletion processing
                   using new architecture.  Also generates event rather than
                   calling dsiface_up() call.
05/15/02   vsk/rc  In ipcp_init(), initialize IPCP will options from what is
                   passed in.
03/05/02    pjb    Finished Accept Listen Code Review Items
02/22/02    pjb    Added accept and listen
02/20/02    sjy    Mobile IP performance now featurized under
                   FEATURE_DS_MOBILE_IP_PERF
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/07/02    vsk    Initialize slhcp to NULL in ipcp_init()
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module, wrap old/new DS
                   Mgr dependencies w/FEATURE_DATA (namely PS to DS cmd i/f)
10/29/01    igt    Force the ppp state to fsmINITIAL in ipcp_init()
09/20/01    mvl    Reset the value of remote.work to 0 in starting() - to
                   ensure that we don't reject the othe guys desires.
09/13/01    mvl    Modified message to indicate that unsupported option was
                   negotiated.  Added copy of want to work in starting.
08/29/01    mvl    Added support for DNS options.
08/19/01    sjy    Referenced the correct IPCP configuration values in
                   ipcp_powerup_init() for when netmodel feature is not
                   defined.
08/12/01    sjy    Featurized reference to do_mip variable in PPP control
                   block
08/02/01    mvl    General code cleanup
07/30/01    mvl    Added set_ip_addr_opt() to add an IP addr and the bit in
                   the mask to the options data structure passed in.
                   Completely revamped the initialization so that LCP can be
                   initialized per interface and with passed in options, or
                   the defaults.
                   Support for assigning addresses on the Um interface
                   Iface IP address only assigned if iface not doing MIP.
07/29/01    na     Fixed ipcp_opening() to use generic interfaces.
07/27/01    mvl    Modifications to use PS timers, checnged the retry timer
                   to not multiply the header value by 1000.
07/24/01    mvl    compiling out parts of fsm_constants_initializer struct to
                   conserve memory, initing fsm.iface field and code cleanup
07/23/01    pjb    Change location/prefix of command names.
06/27/01    na     Removed extra event to Meta SM in ipcp_opening().
                   Now calling dsiface_up() from ipcp_opening()
06/05/01    mvl    Made some fixes to where our IP address was being zeroed
                   out when we were handing off.
06/01/01    mvl    fixed bug where mobile rejects IWF address if different
                   than the address we have stored. Also made sure Mip is
                   only kicked off if QCMIP is != 0.
05/18/01    mvl    Added hook to initiate Mobile IP when IPCP is complete and
                   general clean up.
04/16/01    js     Added inclusion for dsiface.h. Instead of setting Ip_addr
                   directly, dsiface_set_ip_addr() is invoked.
04/06/01    js     Modified ipcp_opening() so that
                   dssocki_socket_event_occurred() is called when Ip_addr is
                   assigned a value. The purpose of this change is to
                   support socket option DSS_SO_IP_ADDR_OK.
02/07/01    snn    In IPCP starting, modified code to request for an IP address
                   even if the current IP address is not Zero. This is needed
                   because, if current IP address != new ip address, we close
                   all the TCP connections.
                   Moved the call to dsstcp_new_ip_upcall() after
                   intitializing the header compression parameters in the
                   function ipcp_opening.
03/22/00   rc/na   Added a call to dsstcp_new_ip_upcall() which is called
                   when a new IP address (different from the old) is
                   received after a PPP resync.
10/28/99    mvl    Added code review changes
08/20/99    ak     commented out code to handle new IP address, since psmgr
                   punts the sockets call on a ppp re-sync anyways.
08/20/99    mvl    Added ipcp_save_work_opt() function
08/18/99    ak     Made fixes so compiles when FEATURE_DS turned off.
08/17/99    smp    Added include files msg.h & err.h.
08/11/99   ak/rc   Code to take care of getting a new IP address.
08/09/99    mvl    Added network model changes.
04/08/99    na     Changed PPP_IPCP_PROTOCOL to DSM_IPCP_PROTOCOL where the
                   protocol was used in the kind field.
03/25/99    hcg    Removed doipcp_default() function, as it was not being
                   referenced anywhere in the code.
01/27/98    ldg    Removed the DS_SLIM switch.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
03/13/97    jjn    included target.h and customer.h
02/19/96    na     UP bug fixes
11/14/96    jjw    Changes to support faster TCP open
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"


#include "msg.h"
#include "amssassert.h"

#include "dsm.h"
#include "ps_byte.h"
#include "pstimer.h"
#include "ps_iface.h"
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_api.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_config.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi_fsm.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi_ipcp.h"
#include "ps_ppp_events.h"
#include "ps_ppp_snpip.h"
#include "ps_ppp_lcp.h"
#include "ps_pppi_lcp.h"
#include "ps_utils.h"

#ifdef FEATURE_DATA_PS_PPP_ROHC
#include "ps_rohc.h"
#endif /* FEATURE_DATA_PS_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
#include "ps_iphc.h"
#endif /* FEATURE_DATA_PS_IPHC */

#include "ds_Utils_DebugMsg.h"


/*===========================================================================

           GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
static ipcp_value_type ipcp_local_want_cb[PPP_MAX_DEV];
static ipcp_value_type ipcp_local_work_cb[PPP_MAX_DEV];
static ipcp_value_type ipcp_remote_want_cb[PPP_MAX_DEV];
static ipcp_value_type ipcp_remote_work_cb[PPP_MAX_DEV];


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
typedef struct ppp_vj_option_type
{
  uint16 protocol;         /* PPP_VJCH_PROTOCOL                            */
  uint8 num_slots;         /* Slots (0-n)                                  */
  uint8 slot_comp;         /* Slots may be compressed (flag)               */
} ppp_vj_option_type;

#ifdef FEATURE_DATA_PS_PPP_IPHC
typedef struct ppp_iphc_option_type
{
  uint16  protocol;        /* PPP_IPHC_PROTOCOL                            */
  uint16  tcp_space;       /* Size of TCP context space                    */
  uint16  non_tcp_space;   /* Size of non-TCP context space                */
  uint16  f_max_period;    /* Max interval between full headers            */
  uint16  f_max_time;      /* Max time between full headers                */
  uint16  max_header;      /* Max header size that can be compressed       */
} ppp_iphc_option_type;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

#ifdef FEATURE_DATA_PS_PPP_ROHC
typedef struct ppp_rohc_option_type
{
  uint16  protocol;        /* PPP_ROHC_SCID_PROTOCOL                       */
  uint16  max_cid;         /* Max value of context identifier              */
  uint16  mrru;            /* Max reconstructed reception unit             */
  uint16  max_header;      /* Max header size that can be compressed       */
} ppp_rohc_option_type;
#endif /* FEATURE_DATA_PS_PPP_ROHC */

/*---------------------------------------------------------------------------
  SLHC INIT MACRO: Do either new allocation (or) reset the current instance
---------------------------------------------------------------------------*/
#define IPCP_SLHC_INIT(slhc_handle, r_slots, t_slots)  \
{ \
  if (slhc_handle == NULL) \
  { /* First time allocation */ \
    slhc_handle = slhc_alloc(r_slots, t_slots); \
  } \
  else \
  { /* Memory allocated already, do re-init */ \
    slhc_ioctl_u_type ioctl_data; \
    ioctl_data.options.rslots = r_slots; \
    ioctl_data.options.tslots = t_slots; \
    (void)slhc_ioctl( slhc_handle, \
                      SLHC_IOCTL_CHANNEL_REINITIALIZATION, \
                      &ioctl_data ); \
  } \
}

static net_model_resync_fptr_type ipcp_net_model_resync_fnc[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
  These defaults are defined in the PPP RFCs, and must not be changed
---------------------------------------------------------------------------*/
static ipcp_value_type ipcp_default =
{
  {0},      /* no source address */
  {0},      /* no primary dns server address */
  {0},      /* no seconday dns server address */
  {0},      /* no primary nbns address - for WINS support */
  {0},      /* no secondary nbns address - for WINS support */
  0,      /* no compression protocol */
  {0, 0}           /* VJ - no slots, no slot compression                   */
#ifdef FEATURE_DATA_PS_PPP_ROHC
  ,{0,0,0,0,0}     /* ROHC config options                                  */
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
  ,{0,0,0,0,0}     /* IPHC config options                                  */
#endif /* FEATURE_DATA_PS_PPP_IPHC */
};

/*---------------------------------------------------------------------------
  Note - IPCP_N_PRIMARY_NBNS and IPCP_N_SECONDARY_NBNS are included here as,
  WINS is used by UMTS. For 1X these options are later removed.
---------------------------------------------------------------------------*/
static negotiate_t ipcp_accept = IPCP_N_1ADDRESS    | IPCP_N_COMPRESS |
                                 IPCP_N_COMPRESS_VJ |
#ifdef FEATURE_DATA_PS_PPP_ROHC
                          IPCP_N_COMPRESS_ROHC |
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
                          IPCP_N_COMPRESS_IPHC |
#endif /* FEATURE_DATA_PS_PPP_IPHC */
                          IPCP_N_PRIMARY_DNS  | IPCP_N_SECONDARY_DNS |
                          IPCP_N_PRIMARY_NBNS | IPCP_N_SECONDARY_NBNS;

/*---------------------------------------------------------------------------
  these lengths are the minimum required for the option
---------------------------------------------------------------------------*/
static uint8 option_length[] =
{
  IPCP_COMPRESS_SIZ,        /*   2 - compression */
  IPCP_1ADDRESS_SIZ,        /*   3 - address     */
  IPCP_PRIMARY_DNS_SIZ,     /* 129 - primary dns server address */
  IPCP_SECONDARY_DNS_SIZ,   /* 131 - secondary dns server address */
  IPCP_PRIMARY_NBNS_SIZ,    /* 130 - primary nbns server address */
  IPCP_SECONDARY_NBNS_SIZ   /* 132 - secondary nbns server address */

};

/*---------------------------------------------------------------------------
  these are the PPP options numbers based on the option mask. (N's above)
----------------------------------------------------------------------------*/
static uint8 option_number[] =
{
  IPCP_COMPRESS,            /*   2 - compression */
  IPCP_1ADDRESS,            /*   3 - address     */
  IPCP_PRIMARY_DNS,         /* 129 - primary dns server address */
  IPCP_SECONDARY_DNS,       /* 131 - secondary dns server address */
  IPCP_PRIMARY_NBNS,     /* 130 - primary nbns server address - for WINS */
  IPCP_SECONDARY_NBNS    /* 132 - secondary nbns server address - for WINS */
};

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--*/
static void ipcp_option
(
  ppp_fsm_type      *fsm_ptr,
  void              *vp,
  struct dsm_item_s **item_ref_ptr,
  uint8             o_type
);

static int ipcp_check
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp,
  struct dsm_item_s **item_ref_ptr,
  int               request
);

static int ipcp_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp,
  dsm_item_type     *bp
);

static void ipcp_starting
(
  ppp_fsm_type *fsm_ptr
);

static void ipcp_closing
(
  ppp_fsm_type *fsm_ptr
);

static void ipcp_opening
(
  ppp_fsm_type *fsm_ptr
);

static void ipcp_free
(
  ppp_fsm_type *fsm_ptr
);

static void ipcpi_timer_cb
(
  void *arg_ptr
);

static void ipcpi_iface_config
(
  ppp_dev_enum_type  device
);

/*---------------------------------------------------------------------------
  IPCP constants - this MUST follow the forward declarations because many of
  these functions are used in this data structure.
---------------------------------------------------------------------------*/
static struct fsm_constant_s ipcp_default_constants =
{
  PPP_IPCP_PROTOCOL,        /* Protocol number                      */
  0x00FE,                   /* codes 1-11 recognized                */

  IPCP,                     /* Finite State Machine index           */
  IPCP_REQ_TRY,             /* # tries for configure                */
  IPCP_NAK_TRY,             /* # tries for nak substitutes          */
  IPCP_TERM_TRY,            /* # tries for terminate                */
  IPCP_TIMEOUT,             /* Time for TERM REQ timeouts (msecs)   */
  IPCP_TIMEOUT,             /* Time for C_ACK timeouts (msecs)      */

  IPCP_OPTION_LIMIT,        /* maximum option number                */
  IPCP_N_OPTION_LIMIT,      /* maximum mask number                  */
  option_number,            /* mask table                           */

  PPP_NCP_COMPRESS_IGNORE,  /* compression enable, IGNORE by default*/

  ipcp_free,                /* to free structure                    */
  fsm_down,                 /* layer layer down event               */

  ipcp_starting,            /* this layer start: enter starting     */
  ipcp_opening,             /* this layer opening: enter opening    */
  ipcp_closing,             /* this layer closing: enter closing    */
  ipcp_stopping,            /* this layers stopping: leave opened   */

  ipcp_option,              /* add option to message                */
  ipcp_check,               /* validate option and store if good    */
  ipcp_reject               /* process rx'ed config reject          */
};

static struct fsm_constant_s ipcp_device_constants[PPP_MAX_DEV];
static struct cb_constant_s  ipcp_cb_device_constants[PPP_MAX_DEV];


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION IPCP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for IPCP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ipcp_powerup_init
(
  ppp_dev_enum_type device /* the interface to initialize */
)
{
  ppp_fsm_type *fsm_ptr;
  static ipcp_pdv_type ipcp_pdv[PPP_MAX_DEV];
  ppp_type      *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    set the network model resync function to NULL
  -------------------------------------------------------------------------*/
  ipcp_net_model_resync_fnc[device] = NULL;

  /*-------------------------------------------------------------------------
    reset the FSM for this interface
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
  memset(fsm_ptr, 0, sizeof(ppp_fsm_type));

  /*-------------------------------------------------------------------------
    Copy the default device constants
  -------------------------------------------------------------------------*/
  memscpy(&ipcp_device_constants[device],
          sizeof(ipcp_device_constants[device]),
          &ipcp_default_constants,
          sizeof(ipcp_default_constants));
  /*-------------------------------------------------------------------------
    Copy the default ipcp cb constants
  -------------------------------------------------------------------------*/
  memscpy(&ipcp_cb_device_constants[device],
          sizeof(ipcp_cb_device_constants[device]),
          &pppi_fsm_cb_default_constants,
          sizeof(pppi_fsm_cb_default_constants));

  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants and variable pointer.
  -------------------------------------------------------------------------*/
  fsm_ptr->pdc             = &ipcp_device_constants[device];
  fsm_ptr->cbdc            = &ipcp_cb_device_constants[device];
  fsm_ptr->pdv             = &ipcp_pdv[device];
  fsm_ptr->local.want_pdv  = &ipcp_local_want_cb[device];
  fsm_ptr->local.work_pdv  = &ipcp_local_work_cb[device];
  fsm_ptr->remote.want_pdv = &ipcp_remote_want_cb[device];
  fsm_ptr->remote.work_pdv = &ipcp_remote_work_cb[device];
  fsm_ptr->mode            = PPP_CP_MODE_INVALID;
  fsm_ptr->iface_input     = (ppp_iface_input_f_type) ps_iface_input;
  fsm_ptr->iface_config    = (ppp_iface_config_f_type) ipcpi_iface_config;
  fsm_ptr->timer_alloc_fcn     = (timer_alloc_f_type) ps_timer_alloc;
  fsm_ptr->timer_start_fcn     = (timer_start_f_type) ps_timer_start;
  fsm_ptr->timer_cancel_fcn    = (timer_cancel_f_type) ps_timer_cancel;

  /*-------------------------------------------------------------------------
    Initialize the device field to the device passed in
  -------------------------------------------------------------------------*/
  fsm_ptr->device = device;
  /*-------------------------------------------------------------------------
    Get the timer for this instance of IPCP - return 0 if OK or -1 if failed
  -------------------------------------------------------------------------*/
  fsm_ptr->timer = (*fsm_ptr->timer_alloc_fcn)(ipcpi_timer_cb, fsm_ptr);

  if( fsm_ptr->timer == PS_TIMER_INVALID_HANDLE)
  {
    return -1;
  }
  else
  {
    return 0;
  }

} /* ipcp_powerup_init() */


/*===========================================================================
FUNCTION IPCP_SET_DEVICE_CONSTANTS()

DESCRIPTION
  This function is called at system initialization time to set the
  configurable protocol constants for IPCP on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for IPCP
===========================================================================*/
void ipcp_set_device_constants
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  nv_ppp_configure_data_type *config
)
{
  struct fsm_constant_s *pconst;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( config == NULL || device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_2( "Invalid args: device %d, config 0x%x", device, config );
    ASSERT( 0 );
    return;
  }

  pconst = &ipcp_device_constants[device];

  /*-------------------------------------------------------------------------
    Set the configurable protocol dependent constants
  -------------------------------------------------------------------------*/
  pconst->try_configure  = config->ipcp_req_try;
  pconst->try_nak        = config->ipcp_nak_try;
  pconst->try_terminate  = config->ipcp_term_try;

  if(0 < config->ipcp_term_timeout)
  {
    pconst->term_timeout = config->ipcp_term_timeout;
  }

  if(0 < config->ipcp_ack_timeout)
  {
    pconst->ack_timeout = config->ipcp_ack_timeout;
  }

  pconst->compress_enable =
    (ppp_ncp_compress_enable_e_type)(config->ipcp_compression_enable);

  return;
} /* ipcp_set_device_constants() */



/*===========================================================================
FUNCTION PPP_IPCP_INIT_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  IPCP on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for IPCP.
===========================================================================*/
void ppp_ipcp_init_device_constant
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

  pconst = &ipcp_device_constants[device];

  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants that are configurable
  -------------------------------------------------------------------------*/
  switch(constant)
  {
    case IPCP_ACK_TIMEOUT:
      if( 0 < value )
      {
        pconst->ack_timeout = (uint16)value;
      }
      break;

    case IPCP_REQUEST_TRY:
      if( 0 < value )
      {
        pconst->try_configure = (uint8)value;
      }
      break;

    default:
      LOG_MSG_ERROR_1("ppp_ipcp_init_device_constant: Unknown constant %d",
                      constant);
      break;
  }

  return;
} /* ppp_ipcp_init_device_constant() */



/*===========================================================================
FUNCTION IPCP_INIT()

DESCRIPTION
  This function will Initialize the IPCP configuration structure on the
  given interface(Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_init
(
  ppp_dev_enum_type  device,   /* device to initalize         */
  ppp_dev_opts_type   *opts_ptr /* options to initialize with */
)
{
  ppp_fsm_type *fsm_ptr;
  ipcp_value_type *lwant_ptr;
  ipcp_value_type *lwork_ptr;
  ipcp_value_type *rwant_ptr;
  ipcp_value_type *rwork_ptr;
  ipcp_pdv_type *ipcp_ptr;
  ppp_type *ppp_cb_array = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV ||
     opts_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_1("ipcp_init(device %d)", device);

  ppp_cb_array = fsm_get_ppp_cb_array();
  fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
  lwant_ptr = (ipcp_value_type*)fsm_ptr->local.want_pdv;
  lwork_ptr = (ipcp_value_type*)fsm_ptr->local.work_pdv;
  rwant_ptr = (ipcp_value_type*)fsm_ptr->remote.want_pdv;
  rwork_ptr = (ipcp_value_type*)fsm_ptr->remote.work_pdv;
  ipcp_ptr  = (ipcp_pdv_type *)fsm_ptr->pdv;

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  if (ipcp_ptr->slhcp != NULL)
  {
    slhc_free(ipcp_ptr->slhcp);
  }
  ipcp_ptr->slhcp = NULL;

  /*-------------------------------------------------------------------------
    Copy the variables and mask over - not the local needs to be copied to
    the remote and visa versa
  -------------------------------------------------------------------------*/
  memscpy(rwant_ptr, 
          sizeof(ipcp_value_type),
          &(opts_ptr->ipcp_info.want_vals[FSM_REMOTE]),
          sizeof(ipcp_value_type));
  memscpy(lwant_ptr,
          sizeof(ipcp_value_type),
          &(opts_ptr->ipcp_info.want_vals[FSM_LOCAL]),
          sizeof(ipcp_value_type));
  memscpy(rwork_ptr,
          sizeof(ipcp_value_type),
          &(opts_ptr->ipcp_info.want_vals[FSM_REMOTE]),
          sizeof(ipcp_value_type));
  memscpy(lwork_ptr,
          sizeof(ipcp_value_type),
          &(opts_ptr->ipcp_info.want_vals[FSM_LOCAL]),
          sizeof(ipcp_value_type));

  fsm_ptr->local.want = opts_ptr->ipcp_info.want_mask[FSM_LOCAL];
  fsm_ptr->remote.want = opts_ptr->ipcp_info.want_mask[FSM_REMOTE];
  fsm_ptr->local.will = opts_ptr->ipcp_info.will_mask[FSM_LOCAL];
  fsm_ptr->remote.will = opts_ptr->ipcp_info.will_mask[FSM_REMOTE];

  /*-------------------------------------------------------------------------
    Use the configured value of compression boolean to enable/disable
    negotiation of the compression protocol
  -------------------------------------------------------------------------*/
  switch( (ppp_ncp_compress_enable_e_type)fsm_ptr->pdc->compress_enable )
  {
    case PPP_NCP_COMPRESS_ENABLE:
      fsm_ptr->local.want  |= IPCP_N_COMPRESS;
      fsm_ptr->local.will  |= IPCP_N_COMPRESS;
      fsm_ptr->remote.will |= IPCP_N_COMPRESS;
      break;

    case PPP_NCP_COMPRESS_DISABLE:
      fsm_ptr->local.want  &= ~IPCP_N_COMPRESS;
      fsm_ptr->local.will  &= ~IPCP_N_COMPRESS;
      fsm_ptr->remote.will &= ~IPCP_N_COMPRESS;
      break;

    default:
      break;
  } /* switch */

  /*-------------------------------------------------------------------------
    Make sure that work is setup so that we negotiate for it
  -------------------------------------------------------------------------*/
  fsm_ptr->local.work = fsm_ptr->local.want;
  fsm_ptr->remote.work = fsm_ptr->remote.want;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

} /* ipcp_init() */



/*===========================================================================
FUNCTION IPCP_START()

DESCRIPTION
  This function will Check for PPP Network-Layer Protocol Phase

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_start
(
  ppp_type *ppp_cb_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(ppp_cb_ptr == NULL)
  {
    DATA_ERR_FATAL("ipcp_start bad arg: NULL ppp_cb passed");
    return;
  }

  /*-------------------------------------------------------------------------
    If IPCP's mode is active then bring up the IPCP state machine
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_MODE_ACTIVE)
  {
    fsm_up(&(ppp_cb_ptr->fsm[IPCP]));
  }
} /* ipcp_start() */



/*===========================================================================
FUNCTION IPCP_SET_NET_MODEL_RESYNC_FNC()

DESCRIPTION
  This function sets the callback function pointer for the IPCP established
  function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_set_net_model_resync_fnc
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

  ipcp_net_model_resync_fnc[device] = fnc_ptr;

} /* ipcp_set_net_model_resync_fnc() */



/*===========================================================================
FUNCTION IPCP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the IPCP option that is passed in. It is
   being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  ipcp_init() should have been called before this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_save_work_opt
(
  ppp_fsm_side_type *opts,       /* pointer to where the values are stored */
  byte opt_num,                                       /* the option number */
  byte *opt_val,                                /* the value of the option */
  int   opt_val_len
)
{
  ipcp_value_type *work_opts = (ipcp_value_type*)(opts->work_pdv);
#ifdef FEATURE_DATA_PS_PPP_ROHC
  ppp_rohc_config_type  rohc_opt;
  ppp_opt_hdr_type      rohc_subopt;
  int                   i;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
  ppp_iphc_config_type  iphc_opt;
  ppp_opt_hdr_type      iphc_subopt;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Determine option value
  -------------------------------------------------------------------------*/
  switch(opt_num)
  {
  case IPCP_COMPRESS:
    /*-----------------------------------------------------------------------
      Handle the compression option.  The mask will be adjusted externally
    -----------------------------------------------------------------------*/
    work_opts->compression = ps_ntohs((*((uint16*)opt_val)));
    switch (work_opts->compression)
    {
    case PPP_VJCH_PROTOCOL:
      opts->work |= IPCP_N_COMPRESS;
      opts->work |= IPCP_N_COMPRESS_VJ;
      work_opts->vj_config.num_slots = *(opt_val+2) + 1;
      work_opts->vj_config.slot_comp = *(opt_val+3);
      break;

#ifdef FEATURE_DATA_PS_PPP_ROHC
    case PPP_ROHC_SCID_PROTOCOL:
      opts->work |= IPCP_N_COMPRESS;
      opts->work |= IPCP_N_COMPRESS_ROHC;
      memscpy( &rohc_opt,
               sizeof(ppp_rohc_option_type),
               opt_val, 
               sizeof(ppp_rohc_option_type));

      work_opts->rohc_config.max_cid    = ps_ntohs(rohc_opt.max_cid);
      work_opts->rohc_config.mrru       = ps_ntohs(rohc_opt.mrru);
      work_opts->rohc_config.max_header = ps_ntohs(rohc_opt.max_header);

      if (opt_val_len > sizeof(ppp_rohc_option_type))
      {
        opt_val += sizeof(ppp_rohc_option_type);
        memscpy( &rohc_subopt, 
                  sizeof(ppp_opt_hdr_type), 
                  opt_val, 
                  sizeof(ppp_opt_hdr_type));
        switch (rohc_subopt.type)
        {
        case ROHC_SUBOPTION_PROFILES:
          work_opts->rohc_config.no_of_profiles =
            (rohc_subopt.length - sizeof(ppp_opt_hdr_type)) / sizeof(uint16);
          if (work_opts->rohc_config.no_of_profiles > ROHC_PROFILE_MAX)
          {
            LOG_MSG_ERROR_2("Got %d profiles, can support %d",
                      work_opts->rohc_config.no_of_profiles,
                            ROHC_PROFILE_MAX );
            work_opts->rohc_config.no_of_profiles = ROHC_PROFILE_MAX;
          }
          opt_val += sizeof(ppp_opt_hdr_type);
          memscpy( work_opts->rohc_config.profiles,
                   sizeof(work_opts->rohc_config.profiles),
                   opt_val,
                   sizeof(uint16) * work_opts->rohc_config.no_of_profiles);
          for( i=0; i< work_opts->rohc_config.no_of_profiles; i++)
          {
            work_opts->rohc_config.profiles[i] =
              ps_ntohs(work_opts->rohc_config.profiles[i]);
          }
          break;
        default:
          LOG_MSG_ERROR_1("Got unknwon suboption %d in ROHC compression option",
                          rohc_subopt.type );
          break;
        }
      }
      break;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
    case PPP_IPHC_PROTOCOL:
      opts->work |= IPCP_N_COMPRESS;
      opts->work |= IPCP_N_COMPRESS_IPHC;
      memscpy( &iphc_opt,
               sizeof(ppp_iphc_option_type),
               opt_val, 
               sizeof(ppp_iphc_option_type));

      work_opts->iphc_config.tcp_space     = ps_ntohs(iphc_opt.tcp_space);
      work_opts->iphc_config.non_tcp_space = ps_ntohs(iphc_opt.non_tcp_space);
      work_opts->iphc_config.f_max_period  = ps_ntohs(iphc_opt.f_max_period);
      work_opts->iphc_config.f_max_time    = ps_ntohs(iphc_opt.f_max_time);
      work_opts->iphc_config.max_header    = ps_ntohs(iphc_opt.max_header);
      work_opts->iphc_config.rtp_enabled   = FALSE;

      if (opt_val_len > sizeof(ppp_iphc_option_type))
      {
        opt_val += sizeof(ppp_iphc_option_type);
        memscpy( &iphc_subopt, 
                 sizeof(ppp_opt_hdr_type),
                 opt_val, 
                 sizeof(ppp_opt_hdr_type));
        switch (iphc_subopt.type)
        {
        case IPHC_SUBOPTION_RTP_ENABLE:
          work_opts->iphc_config.rtp_enabled = TRUE;
          break;
        default:
          LOG_MSG_ERROR_1("Got unknown suboption %d in IPHC compression option",
                          iphc_subopt.type );
          break;
        }
      }
      break;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
    default:
      LOG_MSG_INFO1_1("Invalid compression protocol %d",
                      work_opts->compression );
      break;
    }
    break;

    /*-----------------------------------------------------------------------
      IP address
    -----------------------------------------------------------------------*/
  case IPCP_1ADDRESS:
    memscpy(&work_opts->address.ps_s_addr, 
             sizeof(work_opts->address.ps_s_addr),
             opt_val,
             sizeof(struct ps_in_addr));
    opts->work |= IPCP_N_1ADDRESS;
    break;

    /*-----------------------------------------------------------------------
      primary dns server address
    -----------------------------------------------------------------------*/
  case IPCP_PRIMARY_DNS:
    memscpy(&work_opts->primary_dns.ps_s_addr,  
            sizeof(work_opts->primary_dns.ps_s_addr),
            opt_val,
            sizeof(struct ps_in_addr));
    opts->work |= IPCP_N_PRIMARY_DNS;
    break;

    /*-----------------------------------------------------------------------
      secondary dns server address
    -----------------------------------------------------------------------*/
  case IPCP_SECONDARY_DNS:
    memscpy(&work_opts->secondary_dns.ps_s_addr,
             sizeof(work_opts->secondary_dns.ps_s_addr),
             opt_val,
             sizeof(struct ps_in_addr));
    opts->work |= IPCP_N_SECONDARY_DNS;
    break;

    /*-----------------------------------------------------------------------
      primary nbns server address - for WINS support
    -----------------------------------------------------------------------*/
  case IPCP_PRIMARY_NBNS:
    memscpy(&work_opts->primary_nbns.ps_s_addr,
             sizeof(work_opts->primary_nbns.ps_s_addr),
             opt_val,
             sizeof(struct ps_in_addr));
    opts->work |= IPCP_N_PRIMARY_NBNS;
    break;

    /*-----------------------------------------------------------------------
      secondary nbns server address - for WINS support
    -----------------------------------------------------------------------*/
  case IPCP_SECONDARY_NBNS:
    memscpy(&work_opts->secondary_nbns.ps_s_addr,
             sizeof(work_opts->secondary_nbns.ps_s_addr),
             opt_val,
             sizeof(struct ps_in_addr));
    opts->work |= IPCP_N_SECONDARY_NBNS;
    break;

    /*-----------------------------------------------------------------------
      default
    -----------------------------------------------------------------------*/
  default:
    LOG_MSG_INFO2_1("Got unknown option %d - ignoring", opt_num);
    break;
  } /* switch(option) */
} /* ipcp_save_work_opt() */


/*===========================================================================
FUNCTION IPCP_SETUP_DEVICE()

DESCRIPTION
  This function performs the necessary IP configuration on the specified
  PPP device and associated iface. Once configured, the device can
  send/receive IP packets to/from the stack above PPP for further
  processing or forwarding.

DEPENDENCIES
  IPCP negotiation on the device must be complete and negotiated
  options/values must be stored in IPCP control block.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_setup_device
(
  ppp_dev_enum_type device
)
{
  ppp_type        *ppp_cb_array = NULL;
  ppp_type        *ppp_cb_ptr;
  ppp_fsm_type    *fsm_ptr;
  ipcp_pdv_type   *ipcp_ptr;
  ipcp_value_type *lworkp;
  ipcp_value_type *rworkp;
  ps_ip_addr_type  ip_addr;
  int              rslots = 0;
  int              tslots = 0;
  ps_phys_link_type * phys_link_ptr;
  ps_phys_link_link_protocol_handle_type link_handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;
  fsm_ptr  = ppp_cb_ptr->fsm + IPCP;
  ipcp_ptr = (ipcp_pdv_type *)(fsm_ptr->pdv);
  lworkp   = (ipcp_value_type *)(fsm_ptr->local.work_pdv);
  rworkp   = (ipcp_value_type *)(fsm_ptr->remote.work_pdv);
  /*-------------------------------------------------------------------------
    Post event to IP snoop SM that device came up
  -------------------------------------------------------------------------*/
  snoop_ip_post_event(device, SNOOP_IP_PPP_DEV_UP_EV);

  /*-------------------------------------------------------------------------
    Store # of compression slots if they were negotiated and initialize the
    SLHC
  -------------------------------------------------------------------------*/
  if(fsm_ptr->local.work & IPCP_N_COMPRESS_VJ)
  {
    rslots = lworkp->vj_config.num_slots;
  }
  if(fsm_ptr->remote.work & IPCP_N_COMPRESS_VJ)
  {
    tslots = rworkp->vj_config.num_slots;
  }
  if(rslots != 0 || tslots != 0)
  {
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    IPCP_SLHC_INIT(ipcp_ptr->slhcp, rslots, tslots);
    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  }

  /*-------------------------------------------------------------------------
    Store local IP address as the iface address and peer's IP address.
  -------------------------------------------------------------------------*/
  if(fsm_ptr->local.work & IPCP_N_1ADDRESS)
  {
    LOG_MSG_INFO3_1("IPCP: Got IPv4 Address = %x", lworkp->address.ps_s_addr);

    /*-----------------------------------------------------------------------
      Initialize the ip_addr vairable
    -----------------------------------------------------------------------*/
    ip_addr.type    = IPV4_ADDR;
    ip_addr.addr.v4.ps_s_addr = lworkp->address.ps_s_addr;
    (void)ps_iface_set_v4_addr(fsm_ptr->rx_iface_ptr, &ip_addr);
  }

  if(fsm_ptr->remote.work & IPCP_N_1ADDRESS)
  {
    fsm_ptr->rx_iface_ptr->v4_net_info.gateway.ps_s_addr = rworkp->address.ps_s_addr;
  }

  /*-------------------------------------------------------------------------
    Set the link protocol to PPP
  -------------------------------------------------------------------------*/
  phys_link_ptr = ps_iface_get_phys_link(fsm_ptr->rx_iface_ptr);
  link_handle.ppp_device = device;
  if( ps_phys_link_set_link_protocol( phys_link_ptr,
                                      PS_PHYS_LINK_LINK_PROTOCOL_PPP,
                                      link_handle) == -1 )
  {
    LOG_MSG_ERROR_1("PPP dev %d failed to set link protocol PPP", device);
  }

  LOG_MSG_INFO1_2("Device(%d):if(0x%x) configured for IPv4",
                  device, fsm_ptr->rx_iface_ptr);
} /* ipcp_setup_device() */


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPCP_OPTION()

DESCRIPTION
  This function appends in the passed option into the passed PPP message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_option
(
  ppp_fsm_type   *fsm_ptr,
  void           *vp,
  dsm_item_type **item_ref_ptr,
  uint8           opt_num
)
{
  ipcp_value_type *value_p = (ipcp_value_type *)vp;
  ppp_opt_hdr_type      opthdr;
  ppp_vj_option_type    vj_option;
#ifdef FEATURE_DATA_PS_PPP_IPHC
  ppp_iphc_option_type  iphc_option;
  ppp_opt_hdr_type      iphc_suboption;
  int                   ret_val = 0;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
#ifdef FEATURE_DATA_PS_PPP_ROHC
  int                   i;
  ppp_rohc_option_type  rohc_option;
  ppp_opt_hdr_type      rohc_suboption;
  uint16                profiles[ROHC_PROFILE_MAX];
#endif /* FEATURE_DATA_PS_PPP_ROHC */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Macro to add an IPCP option to the message.
    NOTE: It is assumed that a non-NULL value will be passed for param_ptr
  -------------------------------------------------------------------------*/
#define IPCP_ADD_OPTION(pkt_ref_ptr, opt_hdr_ptr, param_ptr, param_size) \
  { \
    int ret; \
    ret = dsm_pushdown_tail( pkt_ref_ptr, opt_hdr_ptr, \
                             sizeof(ppp_opt_hdr_type), \
                             DSM_DS_SMALL_ITEM_POOL ); \
    if (ret != sizeof(ppp_opt_hdr_type)) \
    { \
      LOG_MSG_ERROR_1("Failed to add option %d to IPCP message", \
                (opt_hdr_ptr)->type); \
      return; \
    } \
    ret = dsm_pushdown_tail(pkt_ref_ptr, param_ptr, \
                            (param_size), \
                            DSM_DS_SMALL_ITEM_POOL ); \
    if (ret != (param_size)) \
    { \
      LOG_MSG_ERROR_1("Failed to add option %d to IPCP message", \
                 (opt_hdr_ptr)->type); \
      return; \
    } \
  }

  opthdr.type = opt_num;

  LOG_MSG_INFO2_1( "ipcp_option: optnum = %d", opt_num );

  /*-------------------------------------------------------------------------
    Determine what the option type is, and process appropriately
  -------------------------------------------------------------------------*/
  switch( opt_num )
  {
  case IPCP_1ADDRESS:
    LOG_MSG_INFO2_1( "ipaddr = 0x%x", value_p->address.ps_s_addr );
    opthdr.length = sizeof(ppp_opt_hdr_type) + sizeof(struct ps_in_addr);
    IPCP_ADD_OPTION(item_ref_ptr, &opthdr, &value_p->address.ps_s_addr,
                    sizeof(struct ps_in_addr));
    break;

  case IPCP_PRIMARY_DNS:
    LOG_MSG_INFO2_1( "pridns = 0x%x", value_p->primary_dns.ps_s_addr );
    opthdr.length = sizeof(ppp_opt_hdr_type) + sizeof(struct ps_in_addr);
    IPCP_ADD_OPTION(item_ref_ptr, &opthdr, &value_p->primary_dns.ps_s_addr,
                    sizeof(struct ps_in_addr));
    break;

  case IPCP_SECONDARY_DNS:
    LOG_MSG_INFO2_1( "secdns = 0x%x", value_p->secondary_dns.ps_s_addr );
    opthdr.length = sizeof(ppp_opt_hdr_type) + sizeof(struct ps_in_addr);
    IPCP_ADD_OPTION(item_ref_ptr, &opthdr, &value_p->secondary_dns.ps_s_addr,
                    sizeof(struct ps_in_addr));
    break;

  case IPCP_PRIMARY_NBNS:
    opthdr.length = sizeof(ppp_opt_hdr_type) + sizeof(struct ps_in_addr);
    IPCP_ADD_OPTION(item_ref_ptr, &opthdr, &value_p->primary_nbns.ps_s_addr,
                    sizeof(struct ps_in_addr));
    break;

  case IPCP_SECONDARY_NBNS:
    opthdr.length = sizeof(ppp_opt_hdr_type) + sizeof(struct ps_in_addr);
    IPCP_ADD_OPTION(item_ref_ptr, &opthdr, &value_p->secondary_nbns.ps_s_addr,
                    sizeof(struct ps_in_addr));
    break;

  case IPCP_COMPRESS:
    LOG_MSG_INFO2_1( "compression = 0x%x", value_p->compression );
    if (fsm_ptr->send_options_mask & IPCP_N_COMPRESS)
    {
      if (fsm_ptr->send_options_mask & IPCP_N_COMPRESS_VJ)
      {
        opthdr.length = sizeof(ppp_opt_hdr_type) + sizeof(ppp_vj_option_type);
        vj_option.protocol  = ps_htons(PPP_VJCH_PROTOCOL);
        vj_option.num_slots = value_p->vj_config.num_slots - 1;
        vj_option.slot_comp = value_p->vj_config.slot_comp;
        IPCP_ADD_OPTION(item_ref_ptr, &opthdr, &vj_option,
            sizeof(ppp_vj_option_type));
      }

#ifdef FEATURE_DATA_PS_PPP_ROHC
      if (fsm_ptr->send_options_mask & IPCP_N_COMPRESS_ROHC)
      {
        /*-------------------------------------------------------------------
          RFC 3241: Sec 2.1 Configuration Option Format
          0                   1                   2                   3
          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |     Type      |    Length     |    IP-Compression-Protocol    |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |            MAX_CID            |             MRRU              |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |           MAX_HEADER          |          suboptions...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        -------------------------------------------------------------------*/
        rohc_option.protocol   = ps_htons( PPP_ROHC_SCID_PROTOCOL );
        rohc_option.max_cid    = ps_htons( value_p->rohc_config.max_cid );
        rohc_option.mrru       = ps_htons( value_p->rohc_config.mrru );
        rohc_option.max_header = ps_htons( value_p->rohc_config.max_header );

        opthdr.length = sizeof(ppp_opt_hdr_type)+sizeof(ppp_rohc_option_type);

        /*-------------------------------------------------------------------
          Add RFC 3241 Sec 2.2 PROFILES suboption
        -------------------------------------------------------------------*/
        if(value_p->rohc_config.no_of_profiles != 0)
        {
          rohc_suboption.type    = ROHC_SUBOPTION_PROFILES;
          rohc_suboption.length  = sizeof(ppp_opt_hdr_type) +
            (sizeof(uint16) *
             value_p->rohc_config.no_of_profiles);
          opthdr.length         += rohc_suboption.length;

          for(i=0; i < value_p->rohc_config.no_of_profiles; i++)
          {
            profiles[i] = ps_htons( value_p->rohc_config.profiles[i]);
          }
        }

        IPCP_ADD_OPTION(item_ref_ptr, &opthdr, &rohc_option,
            sizeof(ppp_rohc_option_type));
        if(value_p->rohc_config.no_of_profiles != 0)
        {
          IPCP_ADD_OPTION(item_ref_ptr, &rohc_suboption, profiles,
              rohc_suboption.length - sizeof(ppp_opt_hdr_type));
        }
      }
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
      if (fsm_ptr->send_options_mask & IPCP_N_COMPRESS_IPHC)
      {
        /*-------------------------------------------------------------------
          RFC 2509: Sec 2.1 Configuration Option Format

          0                   1                   2                   3
          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |     Type      |    Length     |    IP-Compression-Protocol    |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |           TCP_SPACE           |         NON_TCP_SPACE         |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |         F_MAX_PERIOD          |          F_MAX_TIME           |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |           MAX_HEADER          |          suboptions...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

        -------------------------------------------------------------------*/
        iphc_option.protocol      = ps_htons( PPP_IPHC_PROTOCOL );
        iphc_option.tcp_space     = ps_htons( value_p->iphc_config.tcp_space);
        iphc_option.non_tcp_space = ps_htons( value_p->iphc_config.non_tcp_space);
        iphc_option.f_max_period  = ps_htons( value_p->iphc_config.f_max_period);
        iphc_option.f_max_time    = ps_htons( value_p->iphc_config.f_max_time);
        iphc_option.max_header    = ps_htons( value_p->iphc_config.max_header );

        opthdr.length = sizeof(ppp_opt_hdr_type)+sizeof(ppp_iphc_option_type);

        /*-------------------------------------------------------------------
          Add RFC 3241 Sec 2.2 PROFILES suboption
        -------------------------------------------------------------------*/
        if(value_p->iphc_config.rtp_enabled == TRUE)
        {
          iphc_suboption.type    = IPHC_SUBOPTION_RTP_ENABLE;
          iphc_suboption.length  = sizeof(ppp_opt_hdr_type);
          opthdr.length         += sizeof(ppp_opt_hdr_type);
        }

        IPCP_ADD_OPTION(item_ref_ptr, &opthdr, &iphc_option,
            sizeof(ppp_iphc_option_type));
        if(value_p->iphc_config.rtp_enabled == TRUE)
        {
          ret_val = dsm_pushdown_tail( item_ref_ptr, &iphc_suboption,
              sizeof(ppp_opt_hdr_type),
              DSM_DS_SMALL_ITEM_POOL );
          if (ret_val != sizeof(ppp_opt_hdr_type))
          {
            LOG_MSG_ERROR_1("Failed to add option %d to IPCP message",
                            (&iphc_suboption)->type );
            return;
          }

        }
      }
#endif /* FEATURE_DATA_PS_PPP_IPHC */
    }
    break;

  default:
    LOG_MSG_INFO1_1( "Got IPCP option %d", opt_num);
    break;
  } /* switch(option) */;

  return;
} /* ipcp_option() */



/*===========================================================================
FUNCTION IPCP_CHECK()

DESCRIPTION
  This function will Check the options, updating the working values.

DEPENDENCIES
  None

RETURN VALUE
  Returns -1 if ran out of data, ACK/NAK/REJ as appropriate.

SIDE EFFECTS
  None
===========================================================================*/
static int ipcp_check
(
  ppp_fsm_type       *fsm_ptr,
  struct option_hdr  *ohp,
  struct dsm_item_s **item_ref_ptr,
  int                 request
)
{
  ppp_fsm_side_type * side_p = NULL;
  ipcp_value_type   * swant_ptr = NULL;
  ipcp_value_type   * swork_ptr = NULL;
  int                 used = 0;
  int option_result = CONFIG_ACK;   /* Assume good values */
  int test;
  int                 n_type;
  ppp_type          * ppp_cb_ptr = NULL;
  ppp_type          * ppp_cb_array = NULL;
  negotiate_t        want_comp_mask;
#ifdef FEATURE_DATA_PS_PPP_ROHC
  int                num_profiles;
  int                i;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
  ppp_opt_hdr_type   subopt_hdr;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( item_ref_ptr == NULL || *item_ref_ptr == NULL )
  {
    LOG_MSG_ERROR_0("Invalid DSM in ipcp_check");
    return -1;
  }

  if( fsm_ptr == NULL              ||
      ohp == NULL                  ||
      ohp->length < OPTION_HDR_LEN ||
      fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in ipcp_check");
    dsm_free_packet(item_ref_ptr);
    return -1;
  }

  used = ohp->length - OPTION_HDR_LEN;
  side_p = (request ? &(fsm_ptr->remote) : &(fsm_ptr->local));
  swant_ptr = (ipcp_value_type *)(side_p->want_pdv);
  swork_ptr = (ipcp_value_type *)(side_p->work_pdv);
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)fsm_ptr->device;

  n_type = fsm_n_option(fsm_ptr->pdc,ohp->type);

  /*-------------------------------------------------------------------------
    see if we allow this option
  -------------------------------------------------------------------------*/
  if ((-1 == n_type) || (sizeof(option_length) <= n_type) ||
      (ohp->type > IPCP_OPTION_LIMIT) || 
      !((side_p->will | side_p->want) & (1 << n_type)))
  {
    /*-----------------------------------------------------------------------
      toss any bytes in option
    -----------------------------------------------------------------------*/
    if (used != dsm_pullup( item_ref_ptr, NULL, (uint16)used ))
    {
      LOG_MSG_ERROR_0("Invalid option in ipcp_check");
      return -1;
    }

    return CONFIG_REJ;
  } /* if(invalid option) */

  /*-------------------------------------------------------------------------
    Check the option length.  The length is bad if it is less than the
    minimum length of the option or greater than the maximum length, except
    for copmression options, which are checked separately.
  -------------------------------------------------------------------------*/
  if ((ohp->length < option_length[n_type]) ||
      (ohp->type != IPCP_COMPRESS && ohp->length > option_length[n_type]))
  {
    /*-----------------------------------------------------------------------
      toss any bytes in option
    -----------------------------------------------------------------------*/
    if (used != dsm_pullup( item_ref_ptr, NULL, used ))
    {
      return -1;
    }
    LOG_MSG_ERROR_0("Invalid option length in ipcp_check");
    return CONFIG_NAK;
  }

  LOG_MSG_INFO2_1( "ipcp_check: opttype = %d", ohp->type );
  switch( ohp->type)
  {
    /*-----------------------------------------------------------------------
      IP address option
    -----------------------------------------------------------------------*/
  case IPCP_1ADDRESS:
    LOG_MSG_INFO2_1( "IPCP_1ADDRESS = 0x%x", swork_ptr->address.ps_s_addr );
    test = dsm_pullup( item_ref_ptr,
               (uint8*)&(swork_ptr->address.ps_s_addr),
               sizeof(struct ps_in_addr));
    if(test != sizeof(struct ps_in_addr) )
    {
      return -1;
    }
    used -= 4;
    if(!request )
    {
      /*---------------------------------------------------------------------
        Accept whatever the peer wants, unless specifically configured to
        refuse.
      ---------------------------------------------------------------------*/
      if(!(side_p->will & IPCP_N_1ADDRESS))
      {
        /*-------------------------------------------------------------------
          no changes allowed
        -------------------------------------------------------------------*/
        swork_ptr->address.ps_s_addr = swant_ptr->address.ps_s_addr;
      }
      else if(swork_ptr->address.ps_s_addr == 0L)
      {
        /*-------------------------------------------------------------------
          set current address
        -------------------------------------------------------------------*/
        if((swork_ptr->address.ps_s_addr = swant_ptr->address.ps_s_addr) == 0L)
        {
          swork_ptr->address.ps_s_addr =
         PS_IFACE_IP_V4_ADDR(ppp_cb_ptr->fsm[IPCP].rx_iface_ptr);
        }
      }
      break;
    } /* if(!request) */

    /*-----------------------------------------------------------------------
      REQUEST:
      If the peer asks for a 0 address and we don't have one for them then we
      have to config reject because we don't have one to assign.
    -----------------------------------------------------------------------*/
    if(swork_ptr->address.ps_s_addr == swant_ptr->address.ps_s_addr &&
       swork_ptr->address.ps_s_addr == 0)
    {
      option_result = CONFIG_REJ;
    }

    if(ppp_cb_array[fsm_ptr->device].serves_addrs == TRUE &&
       swant_ptr->address.ps_s_addr != swork_ptr->address.ps_s_addr           &&
       swant_ptr->address.ps_s_addr != 0)
    {
      swork_ptr->address.ps_s_addr = swant_ptr->address.ps_s_addr;
      option_result = CONFIG_NAK;
    }
    break;

    /*-----------------------------------------------------------------------
      primary DNS address
    -----------------------------------------------------------------------*/
  case IPCP_PRIMARY_DNS:
    LOG_MSG_INFO2_1( "IPCP_PRIMARY_DNS = 0x%x",
                     swork_ptr->primary_dns.ps_s_addr );
    test = dsm_pullup( item_ref_ptr,
               (uint8*)&(swork_ptr->primary_dns.ps_s_addr),
               sizeof(struct ps_in_addr));
    if(test != sizeof(struct ps_in_addr))
    {
      return -1;
    }
    used -= 4;
    if ( !request )
    {
      /*---------------------------------------------------------------------
        Accept whatever the peer wants, unless specifically configured to
        refuse.
      ---------------------------------------------------------------------*/
      if ( !(side_p->will & IPCP_N_PRIMARY_DNS) )
      {
        /*-------------------------------------------------------------------
          no changes allowed
        -------------------------------------------------------------------*/
        swork_ptr->primary_dns.ps_s_addr = swant_ptr->primary_dns.ps_s_addr;
      }
      else if ( swork_ptr->primary_dns.ps_s_addr == 0L )
      {
        /*-------------------------------------------------------------------
          set current address
        -------------------------------------------------------------------*/
        if ( (swork_ptr->primary_dns.ps_s_addr = swant_ptr->primary_dns.ps_s_addr)
             == 0L )
        {
          swork_ptr->primary_dns.ps_s_addr = 0;
        }
      }
      break;
    } /* if(!request) */

    /*-----------------------------------------------------------------------
      Request
    -----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
      If the peer asks for a 0 address and we don't have one for them then we
      have to config reject because we don't have one to assign.
    -----------------------------------------------------------------------*/
    if (swork_ptr->primary_dns.ps_s_addr == swant_ptr->primary_dns.ps_s_addr &&
    swork_ptr->primary_dns.ps_s_addr == 0)
    {
      option_result = CONFIG_REJ;
    }

    /*-----------------------------------------------------------------------
      If on he RM interface, require that the peer use configured address
    -----------------------------------------------------------------------*/
    if (fsm_ptr->device == PPP_RM_DEV &&
    swork_ptr->primary_dns.ps_s_addr != swant_ptr->primary_dns.ps_s_addr &&
    swant_ptr->primary_dns.ps_s_addr != 0L )
    {
      swork_ptr->primary_dns.ps_s_addr = swant_ptr->primary_dns.ps_s_addr;
      option_result = CONFIG_NAK;
    }
    break;

    /*-----------------------------------------------------------------------
      secondary DNS address
    -----------------------------------------------------------------------*/
  case IPCP_SECONDARY_DNS:
    LOG_MSG_INFO2_1( "IPCP_SECONDARY_DNS = 0x%x",
                     swork_ptr->secondary_dns.ps_s_addr );
    test = dsm_pullup( item_ref_ptr,
               (uint8*)&(swork_ptr->secondary_dns.ps_s_addr),
               sizeof(struct ps_in_addr));
    if(test != sizeof(struct ps_in_addr))
    {
      return -1;
    }
    used -= 4;
    if ( !request )
    {
      /*---------------------------------------------------------------------
        Accept whatever the peer wants, unless specifically configured to
        refuse.
      ---------------------------------------------------------------------*/
      if ( !(side_p->will & IPCP_N_SECONDARY_DNS) )
      {
        /*-------------------------------------------------------------------
          no changes allowed
        -------------------------------------------------------------------*/
        swork_ptr->secondary_dns.ps_s_addr = swant_ptr->secondary_dns.ps_s_addr;
      }
      else if ( swork_ptr->secondary_dns.ps_s_addr == 0L )
      {
        /*-------------------------------------------------------------------
          set current address
        -------------------------------------------------------------------*/
        if ((swork_ptr->secondary_dns.ps_s_addr=swant_ptr->secondary_dns.ps_s_addr)
            == 0L )
        {
          swork_ptr->secondary_dns.ps_s_addr = 0;
        }
      }
      break;
    } /* if(!request) */


    /*-----------------------------------------------------------------------
      Request
    -----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
      If the peer asks for a 0 address and we don't have one for them then we
      have to config reject because we don't have one to assign.
    -----------------------------------------------------------------------*/
    if (swork_ptr->secondary_dns.ps_s_addr == swant_ptr->secondary_dns.ps_s_addr &&
    swork_ptr->secondary_dns.ps_s_addr == 0)
    {
      option_result = CONFIG_REJ;
    }

    /*-----------------------------------------------------------------------
      If on he RM interface, require that the peer use configured address
    -----------------------------------------------------------------------*/
    if (fsm_ptr->device == PPP_RM_DEV &&
    swork_ptr->secondary_dns.ps_s_addr != swant_ptr->secondary_dns.ps_s_addr &&
    swant_ptr->secondary_dns.ps_s_addr != 0L )
    {
      swork_ptr->secondary_dns.ps_s_addr = swant_ptr->secondary_dns.ps_s_addr;
      option_result = CONFIG_NAK;
    }
    break;

    /*-----------------------------------------------------------------------
      primary nbns address - for WINS support
    -----------------------------------------------------------------------*/
  case IPCP_PRIMARY_NBNS:
    test = dsm_pullup( item_ref_ptr,
               (uint8*)&(swork_ptr->primary_nbns.ps_s_addr),
               sizeof(struct ps_in_addr));
    if(test != sizeof(struct ps_in_addr))
    {
      return -1;
    }
    used -= 4;
    if ( !request )
    {
      /*---------------------------------------------------------------------
        Accept whatever the peer wants, unless specifically configured to
        refuse.
      ---------------------------------------------------------------------*/
      if ( !(side_p->will & IPCP_N_PRIMARY_NBNS) )
      {
        /*-------------------------------------------------------------------
          no changes allowed
        -------------------------------------------------------------------*/
        swork_ptr->primary_nbns.ps_s_addr = swant_ptr->primary_nbns.ps_s_addr;
      }
      else if ( swork_ptr->primary_nbns.ps_s_addr == 0L )
      {
        /*-------------------------------------------------------------------
          set current address
        -------------------------------------------------------------------*/
        if ((swork_ptr->primary_nbns.ps_s_addr = swant_ptr->primary_nbns.ps_s_addr)
              == 0L )
        {
          swork_ptr->primary_nbns.ps_s_addr = 0;
        }
      }
      break;
    } /* if(!request) */

    /*-----------------------------------------------------------------------
      Request
    -----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
      If the peer asks for a 0 address and we don't have one for them then we
      have to config reject because we don't have one to assign.
    -----------------------------------------------------------------------*/
    if (swork_ptr->primary_nbns.ps_s_addr == swant_ptr->primary_nbns.ps_s_addr &&
    swork_ptr->primary_nbns.ps_s_addr == 0)
    {
      option_result = CONFIG_REJ;
    }

    /*-----------------------------------------------------------------------
      If on he RM interface, require that the peer use configured address
    -----------------------------------------------------------------------*/
    if (fsm_ptr->device == PPP_RM_DEV &&
    swork_ptr->primary_nbns.ps_s_addr != swant_ptr->primary_nbns.ps_s_addr &&
    swant_ptr->primary_nbns.ps_s_addr != 0L )
    {
      swork_ptr->primary_nbns.ps_s_addr = swant_ptr->primary_nbns.ps_s_addr;
      option_result = CONFIG_NAK;
    }
    break;

    /*-----------------------------------------------------------------------
       secondary nbns address - for WINS support
    -----------------------------------------------------------------------*/
  case IPCP_SECONDARY_NBNS:
    test = dsm_pullup( item_ref_ptr,
               (uint8*)&(swork_ptr->secondary_nbns.ps_s_addr),
               sizeof(struct ps_in_addr));
    if(test != sizeof(struct ps_in_addr))
    {
      return -1;
    }
    used -= 4;
    if ( !request )
    {
      /*---------------------------------------------------------------------
        Accept whatever the peer wants, unless specifically configured to
        refuse.
      ---------------------------------------------------------------------*/
      if ( !(side_p->will & IPCP_N_SECONDARY_NBNS) )
      {
        /*-------------------------------------------------------------------
          no changes allowed
        -------------------------------------------------------------------*/
        swork_ptr->secondary_nbns.ps_s_addr = swant_ptr->secondary_nbns.ps_s_addr;
      }
      else if ( swork_ptr->secondary_nbns.ps_s_addr == 0L )
      {
        /*-------------------------------------------------------------------
          set current address
        -------------------------------------------------------------------*/
        if ((swork_ptr->secondary_nbns.ps_s_addr =
               swant_ptr->secondary_nbns.ps_s_addr) == 0L )
        {
          swork_ptr->secondary_nbns.ps_s_addr = 0;
        }
      }
      break;
    } /* if(!request) */


    /*-----------------------------------------------------------------------
      Request
    -----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
      If the peer asks for a 0 address and we don't have one for them then we
      have to config reject because we don't have one to assign.
    -----------------------------------------------------------------------*/
    if (swork_ptr->secondary_nbns.ps_s_addr ==
          swant_ptr->secondary_nbns.ps_s_addr &&
        swork_ptr->secondary_nbns.ps_s_addr == 0)
    {
      option_result = CONFIG_REJ;
    }

    /*-----------------------------------------------------------------------
      If on he RM interface, require that the peer use configured address
    -----------------------------------------------------------------------*/
    if (fsm_ptr->device == PPP_RM_DEV &&
    swork_ptr->secondary_nbns.ps_s_addr != swant_ptr->secondary_nbns.ps_s_addr &&
    swant_ptr->secondary_nbns.ps_s_addr != 0L )
    {
      swork_ptr->secondary_nbns.ps_s_addr = swant_ptr->secondary_nbns.ps_s_addr;
      option_result = CONFIG_NAK;
    }
    break;

  case IPCP_COMPRESS:
    LOG_MSG_INFO2_1( "IPCP_COMPRESS = 0x%x", swork_ptr->compression );
    if((test = dsm_pull16(item_ref_ptr)) == -1 )
    {
      return -1;
    }
    used -= 2;

    /*-----------------------------------------------------------------------
      Clear the compression protocol flags in work if this is a NAK or ACK
    -----------------------------------------------------------------------*/
    if (request == FALSE)
    {
      side_p->work &= ~IPCP_N_COMPRESS_VJ;
#ifdef FEATURE_DATA_PS_PPP_IPHC
      side_p->work &= ~IPCP_N_COMPRESS_IPHC;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
#ifdef FEATURE_DATA_PS_PPP_ROHC
      side_p->work &= ~IPCP_N_COMPRESS_ROHC;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
    }

    switch (test)
    {
    case PPP_VJCH_PROTOCOL:
      /*---------------------------------------------------------------------
        Check if the option length is less than the maximum VJ option length
      ---------------------------------------------------------------------*/
      if ( ohp->length > IPCP_COMPRESS_VJ_SIZ )
      {
        if (used != dsm_pullup( item_ref_ptr, NULL, used ))
        {
          return -1;
        }
        LOG_MSG_ERROR_0("Invalid option length in ipcp_check");
        return CONFIG_NAK;
      }

      /*---------------------------------------------------------------------
        Check if we want or will negotiate VJ.  If not NAK with an acceptable
        alternative compression protocol.  If no compression protocol is
        acceptable reject the compression option.
      ---------------------------------------------------------------------*/
      if (request == TRUE && !(side_p->want & IPCP_N_COMPRESS_VJ) &&
          !(side_p->will & IPCP_N_COMPRESS_VJ))
      {
        want_comp_mask = (side_p->want & ( 0
#ifdef FEATURE_DATA_PS_PPP_ROHC
                                         | IPCP_N_COMPRESS_ROHC
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
                                         | IPCP_N_COMPRESS_IPHC
#endif /* FEATURE_DATA_PS_PPP_IPHC */
                                        ));
        /*lint -e{774} */
        if (want_comp_mask == 0)
        /*lint -restore*/
        {
          option_result = CONFIG_REJ;
          break;
        }
        else
        {
          side_p->work |= want_comp_mask;
          swork_ptr->compression  = swant_ptr->compression;
#ifdef FEATURE_DATA_PS_PPP_ROHC
          swork_ptr->rohc_config  = swant_ptr->rohc_config;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
          swork_ptr->iphc_config  = swant_ptr->iphc_config;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
          option_result = CONFIG_NAK;
          break;
        }
      }

      /*---------------------------------------------------------------------
        VJ is acceptable, process the VJ options.
      ---------------------------------------------------------------------*/
      swork_ptr->compression = (uint16)test;
      test = dsm_pullup( item_ref_ptr, &swork_ptr->vj_config,
                         sizeof(ppp_vj_config_type));
      if(test != sizeof(ppp_vj_config_type) )
      {
        return -1;
      }
      swork_ptr->vj_config.num_slots++;

      if(swork_ptr->vj_config.num_slots < IPCP_SLOT_LO)
      {
        swork_ptr->vj_config.num_slots = IPCP_SLOT_LO;
        option_result = CONFIG_NAK;
      }
      else if (swork_ptr->vj_config.num_slots > IPCP_SLOT_HI)
      {
        swork_ptr->vj_config.num_slots = IPCP_SLOT_HI;
        option_result = CONFIG_NAK;
      }

      if(swork_ptr->vj_config.slot_comp > 1 )
      {
        swork_ptr->vj_config.slot_comp = 1;
        option_result = CONFIG_NAK;
      }
      if (option_result == CONFIG_ACK)
      {
        side_p->work |= IPCP_N_COMPRESS_VJ;
      }
      used -= 2;
      break;

#ifdef FEATURE_DATA_PS_PPP_ROHC
    case PPP_ROHC_SCID_PROTOCOL:
      /*---------------------------------------------------------------------
        Check if we want to or will negotiate ROHC.  If not NAK with an
        acceptable alternative compression protocol.  If no compression
        protocol is acceptable reject the compression option.
      ---------------------------------------------------------------------*/
      if (request == TRUE && !(side_p->want & IPCP_N_COMPRESS_ROHC) &&
          !(side_p->will & IPCP_N_COMPRESS_ROHC))
      {
        want_comp_mask = (side_p->want & ( IPCP_N_COMPRESS_VJ
#ifdef FEATURE_DATA_PS_PPP_IPHC
                                         | IPCP_N_COMPRESS_IPHC
#endif /* FEATURE_DATA_PS_PPP_IPHC */
                                        ));
        if (want_comp_mask == 0)
        {
          option_result = CONFIG_REJ;
        }
        else
        {
          side_p->work |= want_comp_mask;
          swork_ptr->vj_config    = swant_ptr->vj_config;
#ifdef FEATURE_DATA_PS_PPP_IPHC
          swork_ptr->iphc_config  = swant_ptr->iphc_config;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
          option_result = CONFIG_NAK;
        }
        break;
      }

      /*---------------------------------------------------------------------
        ROHC is acceptable, process the ROHC options.
      ---------------------------------------------------------------------*/
      test = dsm_pullup( item_ref_ptr, &swork_ptr->rohc_config,
                         3 * sizeof(uint16));
      if (test != 3*sizeof(uint16))
      {
        return -1;
      }

      swork_ptr->rohc_config.max_cid = ps_ntohs(swork_ptr->rohc_config.max_cid);
      swork_ptr->rohc_config.mrru    = ps_ntohs(swork_ptr->rohc_config.mrru);
      swork_ptr->rohc_config.max_header =
        ps_ntohs(swork_ptr->rohc_config.max_header);

      if (swork_ptr->rohc_config.max_cid < IPCP_ROHC_MAX_CID_LO)
      {
        swork_ptr->rohc_config.max_cid = IPCP_ROHC_MAX_CID_LO;
        option_result = CONFIG_NAK;
      }
      else if (swork_ptr->rohc_config.max_cid > IPCP_ROHC_MAX_CID_HI)
      {
        swork_ptr->rohc_config.max_cid = IPCP_ROHC_MAX_CID_HI;
        option_result = CONFIG_NAK;
      }
      used -= 2;

      /*---------------------------------------------------------------------
        Check if the MRRU value is acceptable
      ---------------------------------------------------------------------*/
      if (swork_ptr->rohc_config.mrru > IPCP_ROHC_MRRU_HI)
      {
        swork_ptr->rohc_config.mrru = IPCP_ROHC_MRRU_HI;
        option_result = CONFIG_NAK;
      }
      used -= 2;

      /*---------------------------------------------------------------------
        Check if the MAX_HEADER value is acceptable
      ---------------------------------------------------------------------*/
      if (swork_ptr->rohc_config.max_header < IPCP_ROHC_MAX_HEADER_LO)
      {
        swork_ptr->rohc_config.max_header = IPCP_ROHC_MAX_HEADER_LO;
        option_result = CONFIG_NAK;
      }
      else if (swork_ptr->rohc_config.mrru > IPCP_ROHC_MRRU_HI)
      {
        swork_ptr->rohc_config.max_header = IPCP_ROHC_MAX_HEADER_HI;
        option_result = CONFIG_NAK;
      }
      used -= 2;

      /*---------------------------------------------------------------------
        Process suboptions if there are any
      ---------------------------------------------------------------------*/
      if(ohp->length > 10)
      {
        ppp_opt_hdr_type subopt_hdr;
        test = dsm_pullup(item_ref_ptr,
                          &subopt_hdr,
                          sizeof(ppp_opt_hdr_type));
        if (test != sizeof(ppp_opt_hdr_type))
        {
          LOG_MSG_ERROR_0("Truncated ROHC suboption");
          return -1;
        }
        used -= sizeof(ppp_opt_hdr_type);
        switch (subopt_hdr.type)
        {
          case ROHC_SUBOPTION_PROFILES:
            num_profiles = (subopt_hdr.length - sizeof(ppp_opt_hdr_type)) /
                             sizeof(uint16);
            /* TODO: If number of profiles requested is larger then our
             * profiles array then for now return an error.  A robust
             * handling would copy the profiles that fit in our array
             * and then NAK the option to limit the profiles.
             */
            if (num_profiles > ROHC_PROFILE_MAX)
            {
              LOG_MSG_ERROR_2("Got %d profiles, can support %d",
                              num_profiles, ROHC_PROFILE_MAX );
              return -1;
            }
            used -= (subopt_hdr.length - sizeof(ppp_opt_hdr_type));
            swork_ptr->rohc_config.no_of_profiles = num_profiles;
            test = dsm_pullup( item_ref_ptr,
                           swork_ptr->rohc_config.profiles,
                               (2 * num_profiles) );
            if( test != (2 * num_profiles) )
            {
              LOG_MSG_INFO1_0("ROHC: Error processing PROFILES suboption");
              return -1;
            }
            for( i=0; i< num_profiles; i++)
            {
              swork_ptr->rohc_config.profiles[i] =
                ps_ntohs(swork_ptr->rohc_config.profiles[i]);
            }
            break;

          default:
            LOG_MSG_INFO1_1("ROHC Unassigned/Reserved Sub-option %d",
                            subopt_hdr.type);
            break;
        }
      }
      if (option_result == CONFIG_ACK)
      {
        side_p->work |= IPCP_N_COMPRESS_ROHC;
      }
      break;
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
    case PPP_IPHC_PROTOCOL:
      /*---------------------------------------------------------------------
        Check if we want to or will negotiate IPHC.  If not NAK with an
        acceptable alternative compression protocol.  If no compression
        protocol is acceptable reject the compression option.
      ---------------------------------------------------------------------*/
      if (request == TRUE && !(side_p->want & IPCP_N_COMPRESS_IPHC) &&
          !(side_p->will & IPCP_N_COMPRESS_IPHC))
      {
        want_comp_mask = (side_p->want & ( IPCP_N_COMPRESS_VJ
#ifdef FEATURE_DATA_PS_PPP_ROHC
                                         | IPCP_N_COMPRESS_ROHC
#endif /* FEATURE_DATA_PS_PPP_ROHC */
                                        ));
        if (want_comp_mask == 0)
        {
          option_result = CONFIG_REJ;
        }
        else
        {
          side_p->work |= want_comp_mask;
          swork_ptr->vj_config    = swant_ptr->vj_config;
#ifdef FEATURE_DATA_PS_PPP_ROHC
          swork_ptr->rohc_config  = swant_ptr->rohc_config;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
          option_result = CONFIG_NAK;
        }
        break;
      }

      /*---------------------------------------------------------------------
        IPHC is acceptable, process IPHC options
      ---------------------------------------------------------------------*/
      test = dsm_pullup(item_ref_ptr, &swork_ptr->iphc_config,
                       5*sizeof(uint16));
      if (test != 5*sizeof(uint16) )
      {
        LOG_MSG_INFO1_0("Received truncated IPHC compression option");
        return -1;
      }

      used -= 5*sizeof(uint16);

      swork_ptr->iphc_config.tcp_space =
        ps_ntohs(swork_ptr->iphc_config.tcp_space);
      swork_ptr->iphc_config.non_tcp_space =
        ps_ntohs(swork_ptr->iphc_config.non_tcp_space);
      swork_ptr->iphc_config.f_max_period =
        ps_ntohs(swork_ptr->iphc_config.f_max_period);
      swork_ptr->iphc_config.f_max_time =
        ps_ntohs(swork_ptr->iphc_config.f_max_time);
      swork_ptr->iphc_config.max_header =
        ps_ntohs(swork_ptr->iphc_config.max_header);

      /*---------------------------------------------------------------------
        Check if the TCP_SPACE value is acceptable
      ---------------------------------------------------------------------*/
      if (swork_ptr->iphc_config.tcp_space < IPCP_IPHC_TCP_SPACE_LO ||
          swork_ptr->iphc_config.tcp_space > IPCP_IPHC_TCP_SPACE_HI)
      {
        swork_ptr->iphc_config.tcp_space = IPCP_IPHC_TCP_SPACE_DEFAULT;
        option_result = CONFIG_NAK;
      }

      /*---------------------------------------------------------------------
        Check if the NON_TCP_SPACE value is acceptable
      ---------------------------------------------------------------------*/
      if (swork_ptr->iphc_config.non_tcp_space <
            IPCP_IPHC_NON_TCP_SPACE_LO ||
          swork_ptr->iphc_config.non_tcp_space >
            IPCP_IPHC_NON_TCP_SPACE_HI)
      {
        swork_ptr->iphc_config.non_tcp_space =
          IPCP_IPHC_NON_TCP_SPACE_DEFAULT;
        option_result = CONFIG_NAK;
      }

      /*---------------------------------------------------------------------
        Check if the F_MAX_PERIOD value is acceptable
      ---------------------------------------------------------------------*/
      if (swork_ptr->iphc_config.f_max_period < IPCP_IPHC_F_MAX_PERIOD_LO ||
          swork_ptr->iphc_config.f_max_period > IPCP_IPHC_F_MAX_PERIOD_HI)
      {
        swork_ptr->iphc_config.f_max_period = IPCP_IPHC_F_MAX_PERIOD_DEFAULT;
        option_result = CONFIG_NAK;
      }

      /*---------------------------------------------------------------------
        Check if the F_MAX_TIME value is acceptable
      ---------------------------------------------------------------------*/
      if (swork_ptr->iphc_config.f_max_time < IPCP_IPHC_F_MAX_TIME_LO ||
          swork_ptr->iphc_config.f_max_time > IPCP_IPHC_F_MAX_TIME_HI)
      {
        swork_ptr->iphc_config.f_max_time = IPCP_IPHC_F_MAX_TIME_DEFAULT;
        option_result = CONFIG_NAK;
      }

      /*---------------------------------------------------------------------
        Check if the MAX_HEADER value is acceptable
      ---------------------------------------------------------------------*/
      if (swork_ptr->iphc_config.max_header < IPCP_IPHC_MAX_HEADER_LO ||
          swork_ptr->iphc_config.max_header > IPCP_IPHC_MAX_HEADER_HI)
      {
        swork_ptr->iphc_config.max_header = IPCP_IPHC_MAX_HEADER_DEFAULT;
        option_result = CONFIG_NAK;
      }

      /*---------------------------------------------------------------------
        Process suboptions if there are any
      ---------------------------------------------------------------------*/
      if(ohp->length > 14)
      {
        test = dsm_pullup(item_ref_ptr,
                          &subopt_hdr,
                          sizeof(ppp_opt_hdr_type));
        if (test != sizeof(ppp_opt_hdr_type) ||
            subopt_hdr.length < sizeof(ppp_opt_hdr_type))
        {
          LOG_MSG_ERROR_0("Malformed IPHC suboption");
          return -1;
        }
        used -= sizeof(ppp_opt_hdr_type);
        switch (subopt_hdr.type)
        {
        case IPHC_SUBOPTION_RTP_ENABLE:
          if (subopt_hdr.length != sizeof(uint16))
          {
            return -1;
          }
          /* TODO:  For now we don't support RTP so NAK the option */
          swork_ptr->iphc_config.rtp_enabled = FALSE;
          option_result = CONFIG_NAK;
          break;

        default:
          LOG_MSG_INFO1_1("IPHC Unassigned/Reserved Sub-option %d",
                          subopt_hdr.type);
          option_result = CONFIG_NAK;
          break;
        }
      }
      if (option_result == CONFIG_ACK)
      {
        side_p->work |= IPCP_N_COMPRESS_IPHC;
      }
      break;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

    default:
      want_comp_mask = (side_p->want & (IPCP_N_COMPRESS_VJ
#ifdef FEATURE_DATA_PS_PPP_ROHC
                                        | IPCP_N_COMPRESS_ROHC
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
                                        | IPCP_N_COMPRESS_IPHC
#endif /* FEATURE_DATA_PS_PPP_IPHC */
                                      ));
      if (want_comp_mask == 0)
      {
        option_result = CONFIG_REJ;
      }
      else
      {
        side_p->work |= want_comp_mask;
        swork_ptr->compression = swant_ptr->compression;
        swork_ptr->vj_config = swant_ptr->vj_config;
#ifdef FEATURE_DATA_PS_PPP_ROHC
        swork_ptr->rohc_config = swant_ptr->rohc_config;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
        swork_ptr->iphc_config = swant_ptr->iphc_config;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
        option_result = CONFIG_NAK;
      }
      break;
    } /* switch(compression type) */
    break;

  default:
    option_result = CONFIG_REJ;
    break;
  } /* switch(option) */

  if(used > 0 )
  {
    /*-----------------------------------------------------------------------
      toss extra bytes in option
    -----------------------------------------------------------------------*/
    if( used != dsm_pullup(item_ref_ptr, NULL, used ) )
    {
      LOG_MSG_ERROR_0("Unable to toss extra bytes in option");
    }
  }
  return (option_result);

} /* ipcp_check() */



/*===========================================================================
FUNCTION IPCP_REJECT()

DESCRIPTION
  This function will process a configuration reject sent by the other side.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int ipcp_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp,
  dsm_item_type     *bp
)
{
  int         option_index;
  int         ret = 0;
  uint16      proto;
  negotiate_t work_comp_mask;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (-1 == (option_index = fsm_n_option(fsm_ptr->pdc, ohp->type)))
  {
    LOG_MSG_INFO2_3( "ipcp_reject(): dev %d, fsm %d, unrecognized option %d",
             fsm_ptr->device, fsm_ptr->pdc->fsmi, ohp->type);
    ret = -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Handle rejection of compression option by looking at the protocol being
      rejected and clearing only the mask corresponding to it.  If all
      the compression protocols  are disabled due to this rejection,
      clear the compression option flag.
    -----------------------------------------------------------------------*/
    if (ohp->type == IPCP_COMPRESS)
    {
      if(dsm_extract(bp,0,&proto, sizeof(uint16)) != sizeof(uint16) )
      {
        return -1;
      }
      switch(ps_ntohs(proto))
      {
      case PPP_VJCH_PROTOCOL:
        fsm_ptr->local.work &= ~ IPCP_N_COMPRESS_VJ;
        break;
#ifdef FEATURE_DATA_PS_PPP_IPHC
      case PPP_IPHC_PROTOCOL:
        fsm_ptr->local.work &= ~ IPCP_N_COMPRESS_IPHC;
        break;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
#ifdef FEATURE_DATA_PS_PPP_ROHC
      case PPP_ROHC_SCID_PROTOCOL:
        fsm_ptr->local.work &= ~ IPCP_N_COMPRESS_ROHC;
        break;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
      default:
        LOG_MSG_INFO1_1("Received rejection for compression protocol 0x%x",
                        ps_ntohs(proto) );
        break;
      }

      work_comp_mask =
        ( fsm_ptr->local.work & ( IPCP_N_COMPRESS_VJ
#ifdef FEATURE_DATA_PS_PPP_IPHC
                                 | IPCP_N_COMPRESS_IPHC
#endif /* FEATURE_DATA_PS_PPP_IPHC */
#ifdef FEATURE_DATA_PS_PPP_ROHC
                                 | IPCP_N_COMPRESS_ROHC
#endif /* FEATURE_DATA_PS_PPP_ROHC */
                                ) );
      if ( work_comp_mask == 0 )
      {
        fsm_ptr->local.work &= ~(unsigned int)(1 << option_index);
      }
    }
    else
    {
      fsm_ptr->local.work &= ~(unsigned int)(1 << option_index);
    }
  }

  return ret;

} /* ipcp_reject() */


/*===========================================================================
FUNCTION IPCP_STARTING()

DESCRIPTION
  This function will Prepare to begin configuration exchange

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_starting
(
  ppp_fsm_type *fsm_ptr
)
{
  ipcp_value_type *lwant_ptr = (ipcp_value_type*)(fsm_ptr->local.want_pdv);
  ipcp_value_type *lwork_ptr = (ipcp_value_type*)(fsm_ptr->local.work_pdv);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_STARTING event.
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP,
                             fsm_ptr->device,
                             PPP_PROTOCOL_IPCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_STARTING );


  /*-------------------------------------------------------------------------
    If we don't already want a particular address, and we currently have
    another address, retry previous assignment for continuity
  -------------------------------------------------------------------------*/
  if(fsm_ptr->local.work & IPCP_N_1ADDRESS &&
     lwant_ptr->address.ps_s_addr == 0             &&
     lwork_ptr->address.ps_s_addr != 0)
  {
    lwant_ptr->address.ps_s_addr = lwork_ptr->address.ps_s_addr;
  }

  if(fsm_ptr->local.work & IPCP_N_PRIMARY_DNS &&
     lwant_ptr->primary_dns.ps_s_addr == 0 &&
     lwork_ptr->primary_dns.ps_s_addr != 0)
  {
    lwant_ptr->primary_dns.ps_s_addr = lwork_ptr->primary_dns.ps_s_addr;
  }

  if(fsm_ptr->local.work & IPCP_N_SECONDARY_DNS &&
     lwant_ptr->secondary_dns.ps_s_addr == 0 &&
     lwork_ptr->secondary_dns.ps_s_addr != 0)
  {
    lwant_ptr->secondary_dns.ps_s_addr = lwork_ptr->secondary_dns.ps_s_addr;
  }

  /*-------------------------------------------------------------------------
    For WINS support
  -------------------------------------------------------------------------*/
  if(fsm_ptr->local.work & IPCP_N_PRIMARY_NBNS &&
     lwant_ptr->primary_nbns.ps_s_addr == 0 &&
     lwork_ptr->primary_nbns.ps_s_addr != 0)
  {
    LOG_MSG_INFO1_0("PRIMARY NBNS ADDRESS PRESENT");
    lwant_ptr->primary_nbns.ps_s_addr = lwork_ptr->primary_nbns.ps_s_addr;
  }

  if(fsm_ptr->local.work & IPCP_N_SECONDARY_NBNS &&
     lwant_ptr->secondary_nbns.ps_s_addr == 0 &&
     lwork_ptr->secondary_nbns.ps_s_addr != 0)
  {
    lwant_ptr->secondary_nbns.ps_s_addr = lwork_ptr->secondary_nbns.ps_s_addr;
  }

  fsm_ptr->local.work = fsm_ptr->local.want;

  fsm_ptr->remote.work = FALSE;

} /* ipcp_starting() */



/*===========================================================================
FUNCTION IPCP_OPENING()

DESCRIPTION
  This function completes IPCP configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_opening
(
  ppp_fsm_type *fsm_ptr
)
{
  ipcp_pdv_type   *ipcp_ptr = NULL;
  ipcp_value_type *lworkp = NULL;
  ipcp_value_type *rworkp = NULL;
  ppp_type        *ppp_cb_ptr;
  ppp_type        *ppp_cb_array = NULL;
  ppp_dev_enum_type device;
  boolean          iface_up_ok = TRUE;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( fsm_ptr == NULL                 ||
      fsm_ptr->device >= PPP_MAX_DEV  ||
      fsm_ptr->pdv == NULL            ||
      fsm_ptr->local.work_pdv == NULL ||
      fsm_ptr->remote.work_pdv == NULL )
  {
    LOG_MSG_ERROR_0("Invalid fsm_ptr in ipcp_opening");
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  device = fsm_ptr->device;
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  ipcp_ptr = (ipcp_pdv_type *)(fsm_ptr->pdv);
  lworkp = (ipcp_value_type *)(fsm_ptr->local.work_pdv);
  rworkp = (ipcp_value_type *)(fsm_ptr->remote.work_pdv);

  /*-------------------------------------------------------------------------
    reset to total setup timer
  -------------------------------------------------------------------------*/
  (void)ps_timer_cancel(ppp_cb_ptr->setup_timer);

  /*-------------------------------------------------------------------------
    Proceed further only if this was an active bring up.
  -------------------------------------------------------------------------*/
  if( PPP_CP_ACTIVE != fsm_ptr->mode && PPP_CP_PASSIVE != fsm_ptr->mode )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Post event to IP snoop SM that device came up
  -------------------------------------------------------------------------*/
  snoop_ip_post_event(device, SNOOP_IP_PPP_DEV_UP_EV);

  /*-------------------------------------------------------------------------
    Check if a header compression protocol was negotiated in either direction
  -------------------------------------------------------------------------*/
  if ((fsm_ptr->local.work & IPCP_N_COMPRESS) ||
      (fsm_ptr->remote.work & IPCP_N_COMPRESS) )
  {
    /*-----------------------------------------------------------------------
      Store # of compression slots if they were negotiated and initialize the
      SLHC
    -----------------------------------------------------------------------*/
    if (((fsm_ptr->local.work & IPCP_N_COMPRESS_VJ) &&
        (lworkp->vj_config.num_slots != 0)) ||
        ((fsm_ptr->remote.work & IPCP_N_COMPRESS_VJ) &&
        (rworkp->vj_config.num_slots != 0)))
    {
      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      IPCP_SLHC_INIT( ipcp_ptr->slhcp,
                      lworkp->vj_config.num_slots,
                       rworkp->vj_config.num_slots );
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    }

#ifdef FEATURE_DATA_PS_PPP_ROHC
    /*-----------------------------------------------------------------------
      Allocate a ROHC instance if ROHC was negotiated
    -----------------------------------------------------------------------*/
    if ((fsm_ptr->local.work & IPCP_N_COMPRESS_ROHC) ||
        (fsm_ptr->remote.work & IPCP_N_COMPRESS_ROHC))
    {
      pppi_rohc_alloc( fsm_ptr->device,
                       IPCP,
                       &lworkp->rohc_config,
                       &rworkp->rohc_config );
    }
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
    /*-----------------------------------------------------------------------
      Allocate an IPHC instance if IPHC was negotiated
    -----------------------------------------------------------------------*/
    if ((fsm_ptr->local.work & IPCP_N_COMPRESS_IPHC) ||
        (fsm_ptr->remote.work & IPCP_N_COMPRESS_IPHC))
    {
      pppi_iphc_alloc( fsm_ptr->device,
                       IPCP,
                       &lworkp->iphc_config,
                       &rworkp->iphc_config );
    }
#endif /* FEATURE_DATA_PS_PPP_IPHC */
  } /* if hdr compression */

  /*-------------------------------------------------------------------------
    Configure the iface
  -------------------------------------------------------------------------*/
  fsm_ptr->iface_config(device);

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-------------------------------------------------------------------------
    Notify M.IP call performance that Um/Rm PPP is established.
  -------------------------------------------------------------------------*/
  if (device == PPP_UM_SN_DEV)
  {
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_UM_PPP_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_UM_PPP_DELAY,
                   PERF_END_TS);
  }
  else if (device == PPP_RM_DEV)
  {
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_RM_PPP_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_RM_PPP_DELAY,
                   PERF_END_TS);
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

  LOG_MSG_INFO2_1("IPCP established on dev %d", device);

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_OPEN event.
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP,
                             fsm_ptr->device,
                             PPP_PROTOCOL_IPCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_OPEN );

  /*-----------------------------------------------------------------------
    If a callback was registered for resyncs, then execute that code
  -----------------------------------------------------------------------*/
  if( ipcp_net_model_resync_fnc[device] != NULL)
  {
    iface_up_ok = ipcp_net_model_resync_fnc[device](device);
  }

  /*-------------------------------------------------------------------------
    If the ppp up callback is set, call it.
  -------------------------------------------------------------------------*/
  if (iface_up_ok == TRUE)
  {
    /*-----------------------------------------------------------------------
      Call the PPP_UP_EV callbacks on IPCP
    -----------------------------------------------------------------------*/
    pppi_call_event_callback( fsm_ptr, PPP_PROTOCOL_IPCP, PPP_UP_EV );

    /*-----------------------------------------------------------------------
      Simultaneous IPv4 and IPv6 for laptop calls is not currently
      implemented, however, in the future its probably best to only mark
      IPv4 as up if the resync succeeded, as the mode controller is
      probably going to tear down the IPv4 ps iface above next.

      - this should probably be removed since the mask is not cleared
        before resync - should only be cleared when application requesting
        IPv4 closes.
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    ppp_cb_ptr->ncp_mask |= (uint16)PPP_PROTOCOL_IPCP_MASK;
    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  }
  else
  {
    LOG_MSG_INFO2_0 ("Could not negotiate required options during resync");
  }

  if( ppp_cb_ptr->resync_dns_opts == FALSE )
  {
    /*----------------------------------------------------------------------
    JCDMA requires that DNS options should not be negotiated during a
    resync, so make it look like they were never negotiated, and we
    don't want to negotiate them either.
    ----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    LOG_MSG_INFO1_1("ipcp_opening, dev %d : Disabling DNS option in IPCP",
                    fsm_ptr->device);
    /*-----------------------------------------------------------------------
     Disable primary DNS option.
    -----------------------------------------------------------------------*/
    fsm_ptr->local.work   &= ~IPCP_N_PRIMARY_DNS;
    fsm_ptr->local.want   &= ~IPCP_N_PRIMARY_DNS;
    fsm_ptr->local.will   &= ~IPCP_N_PRIMARY_DNS;
    fsm_ptr->remote.work  &= ~IPCP_N_PRIMARY_DNS;
    fsm_ptr->remote.want  &= ~IPCP_N_PRIMARY_DNS;
    fsm_ptr->remote.will  &= ~IPCP_N_PRIMARY_DNS;

    /*-----------------------------------------------------------------------
      Disable secondary DNS option.
    -----------------------------------------------------------------------*/
    fsm_ptr->local.work   &= ~IPCP_N_SECONDARY_DNS;
    fsm_ptr->local.want   &= ~IPCP_N_SECONDARY_DNS;
    fsm_ptr->local.will   &= ~IPCP_N_SECONDARY_DNS;
    fsm_ptr->remote.work  &= ~IPCP_N_SECONDARY_DNS;
    fsm_ptr->remote.want  &= ~IPCP_N_SECONDARY_DNS;
    fsm_ptr->remote.will  &= ~IPCP_N_SECONDARY_DNS;

    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  }
} /* ipcp_opening() */


/*===========================================================================
FUNCTION IPCP_CLOSING()

DESCRIPTION
  This function will initiate the closing of IPCP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_closing
(
  ppp_fsm_type *fsm_ptr
)
{
  ipcp_pdv_type *ipcp_ptr = (ipcp_pdv_type *)fsm_ptr->pdv;

  /* Free the slhc instance */
  if(ipcp_ptr != NULL)
  {
    if (ipcp_ptr->slhcp != NULL)
    {
      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );
      slhc_free(ipcp_ptr->slhcp);
      ipcp_ptr->slhcp = NULL;
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );
    }
  }
#ifdef FEATURE_DATA_PS_PPP_ROHC
  if ((fsm_ptr->local.work & IPCP_N_COMPRESS_ROHC) ||
      (fsm_ptr->remote.work & IPCP_N_COMPRESS_ROHC))
  {
    pppi_rohc_free( fsm_ptr->device, IPCP );
  }
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
  if ((fsm_ptr->local.work & IPCP_N_COMPRESS_IPHC) ||
      (fsm_ptr->remote.work & IPCP_N_COMPRESS_IPHC))
  {
    pppi_iphc_free( fsm_ptr->device, IPCP );
  }
#endif /* FEATURE_DATA_PS_PPP_IPHC */

  snoop_ip_post_event(fsm_ptr->device, SNOOP_IP_PPP_DEV_DOWN_EV);

} /* ipcp_closing() */


/*===========================================================================
FUNCTION IPCP_STOPPING()

DESCRIPTION
  This function will cleanup IPCP & post down event to MH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_stopping
(
  ppp_fsm_type *fsm_ptr
)
{
  ppp_type  *ppp_cb_ptr = NULL;
  ppp_type  *ppp_cb_array = NULL;
  ipcp_pdv_type *ipcp_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( fsm_ptr == NULL || fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid fsm_ptr in ipcp_stopping");
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[fsm_ptr->device];
  ipcp_ptr = (ipcp_pdv_type *)fsm_ptr->pdv;

  /*-------------------------------------------------------------------------
    Remove the protocol from the ncp mask
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );
  ppp_cb_ptr->ncp_mask =
    (ppp_cb_ptr->ncp_mask) & ( ~( (uint16)PPP_PROTOCOL_IPCP_MASK ) );
  fsm_ptr->mode         = PPP_CP_MODE_INVALID;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );

#ifdef FEATURE_DATA_PS_PPP_ROHC
  if ((fsm_ptr->local.work & IPCP_N_COMPRESS_ROHC) ||
      (fsm_ptr->remote.work & IPCP_N_COMPRESS_ROHC))
  {
    pppi_rohc_free( fsm_ptr->device, IPCP );
  }
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
  if ((fsm_ptr->local.work & IPCP_N_COMPRESS_IPHC) ||
      (fsm_ptr->remote.work & IPCP_N_COMPRESS_IPHC))
  {
    pppi_iphc_free( fsm_ptr->device, IPCP );
  }
#endif /* FEATURE_DATA_PS_PPP_IPHC */

  /* Free slhc instance */
  if ( ((fsm_ptr->local.work & IPCP_N_COMPRESS_VJ)  ||
       (fsm_ptr->remote.work & IPCP_N_COMPRESS_VJ)) &&
       ipcp_ptr != NULL )
  {
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );
    slhc_free(ipcp_ptr->slhcp);
    ipcp_ptr->slhcp = NULL;
    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );
  }

  /*-------------------------------------------------------------------------
    Call the callbacks registered for PPP_DOWN_EV on IPCP
  -------------------------------------------------------------------------*/
  pppi_call_event_callback(fsm_ptr, PPP_PROTOCOL_IPCP, PPP_DOWN_EV);

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_CLOSING event.
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP,
                             fsm_ptr->device,
                             PPP_PROTOCOL_IPCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSING );

  if( NULL== fsm_ptr->timer_cancel_fcn )
  {
    DATA_ERR_FATAL("timer_cancel_fcn is NULL");
    return;
  }
  (void)(*fsm_ptr->timer_cancel_fcn)(fsm_ptr->timer);

} /* ipcp_stopping() */



/*===========================================================================
FUNCTION IPCPI_TIMER_CB()

DESCRIPTION
  This function

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcpi_timer_cb
(
  void *arg_ptr
)
{
  ppp_fsm_type *fsm_ptr = (ppp_fsm_type*)arg_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("dev %d IPCP Timer Expired state =%d",
                  fsm_ptr->device, fsm_ptr->state);


  /*-------------------------------------------------------------------------
    If the PPP FSM has given up AND we are in a network model kill the PPP
    instance.
  -------------------------------------------------------------------------*/
  if(fsm_timeout( fsm_ptr ) == FALSE)
  {
    LOG_MSG_INFO2_1("IPCP on Dev %d failed to establish!", fsm_ptr->device);

    fsm_ptr->fail_reason = PPP_FAIL_REASON_TIMEOUT;
    /*-----------------------------------------------------------------------
      Stop IPCP.  This will cascade to stopping LCP if IPV6CP not up
      An LCP Term req is sent in this case.
    -----------------------------------------------------------------------*/
    (void)ppp_stop(fsm_ptr->device, PPP_PROTOCOL_IPCP);
  }

} /* ipcpi_timer_cb() */


/*===========================================================================
FUNCTION IPCPI_IFACE_CONFIG

DESCRIPTION
  Function to configure PS iface

PARAMETERS
  device           PPP device to configure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcpi_iface_config
(
  ppp_dev_enum_type   device
)
{
  ppp_type        *ppp_cb_ptr;
  ppp_type        *ppp_cb_array = NULL;
  ppp_fsm_type    *fsm_ptr;
  ps_ip_addr_type  ip_addr;
  ipcp_value_type *lworkp;
  ipcp_value_type *rworkp;
  int32            mru;
  ps_phys_link_type * phys_link_ptr;
  ps_phys_link_link_protocol_handle_type link_handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;
  fsm_ptr    = &(ppp_cb_ptr->fsm[IPCP]);
  lworkp     = (ipcp_value_type *)(fsm_ptr->local.work_pdv);
  rworkp     = (ipcp_value_type *)(fsm_ptr->remote.work_pdv);

  /*-------------------------------------------------------------------------
    Store local IP address as the iface address and peer's IP address.
  -------------------------------------------------------------------------*/
  if(fsm_ptr->local.work & IPCP_N_1ADDRESS)
  {
    LOG_MSG_INFO3_1("IPCP: Got IPv4 Address = %x", lworkp->address.ps_s_addr);

    /*-----------------------------------------------------------------------
      Initialize the ip_addr vairable
    -----------------------------------------------------------------------*/
    ip_addr.type           = IPV4_ADDR;
    ip_addr.addr.v4.ps_s_addr = lworkp->address.ps_s_addr;
    (void)ps_iface_set_v4_addr(fsm_ptr->rx_iface_ptr, &ip_addr);
  }

  if(fsm_ptr->remote.work & IPCP_N_1ADDRESS)
  {
    fsm_ptr->rx_iface_ptr->v4_net_info.gateway.ps_s_addr =
                                                     rworkp->address.ps_s_addr;
  }

  /*-------------------------------------------------------------------------
    Set the link protocol as PPP.
  -------------------------------------------------------------------------*/
  phys_link_ptr = ps_iface_get_phys_link(fsm_ptr->rx_iface_ptr);
  link_handle.ppp_device = device;
  if( ps_phys_link_set_link_protocol( phys_link_ptr,
                                 PS_PHYS_LINK_LINK_PROTOCOL_PPP,
                                      link_handle ) == -1 )
  {
    LOG_MSG_ERROR_1("PPP dev %d failed to set link protocol PPP", device);
  }


  /*-------------------------------------------------------------------------
    Set/clear DNS addresses in the iface
  -------------------------------------------------------------------------*/
    fsm_ptr->rx_iface_ptr->v4_net_info.primary_dns.ps_s_addr =
                                               lworkp->primary_dns.ps_s_addr;

    fsm_ptr->rx_iface_ptr->v4_net_info.secondary_dns.ps_s_addr =
                                                 lworkp->secondary_dns.ps_s_addr;

  /*-------------------------------------------------------------------------
    For WINS support
  -------------------------------------------------------------------------*/
  if(lworkp->primary_nbns.ps_s_addr != 0)
  {
    fsm_ptr->rx_iface_ptr->v4_net_info.primary_nbns.ps_s_addr =
                                                lworkp->primary_nbns.ps_s_addr;
  }

  if(lworkp->secondary_nbns.ps_s_addr != 0)
  {
    fsm_ptr->rx_iface_ptr->v4_net_info.secondary_nbns.ps_s_addr =
                                              lworkp->secondary_nbns.ps_s_addr;
  }

  /*-------------------------------------------------------------------------
    Set the MTU
  -------------------------------------------------------------------------*/
  mru = lcp_mru_get(device);
  if( -1 != mru )
  {
    PS_IFACE_SET_MTU(fsm_ptr->rx_iface_ptr, (uint16)mru);
    LOG_MSG_INFO2_1("IPCP MTU set = %d ", PS_IFACE_GET_MTU(fsm_ptr->rx_iface_ptr));
  }
} /* ipcpi_iface_config() */


/*===========================================================================
FUNCTION IPCP_FREE()

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ipcp_free
(
  ppp_fsm_type *fsm_ptr
)
{
  ipcp_pdv_type *ipcp_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check */
  if(fsm_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ipcp_free: Null argument");
    return;
  }

  ipcp_ptr = (ipcp_pdv_type *)fsm_ptr->pdv;

  /* Free slhc instance */
  if(ipcp_ptr != NULL)
  {
    if (ipcp_ptr->slhcp != NULL)
    {
      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );
      slhc_free(ipcp_ptr->slhcp);
      ipcp_ptr->slhcp = NULL;
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );
    }
  }
#ifdef FEATURE_DATA_PS_PPP_ROHC
  if ((fsm_ptr->local.work & IPCP_N_COMPRESS_ROHC) ||
      (fsm_ptr->remote.work & IPCP_N_COMPRESS_ROHC))
  {
    pppi_rohc_free( fsm_ptr->device, IPCP );
  }
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
  if ((fsm_ptr->local.work & IPCP_N_COMPRESS_IPHC) ||
      (fsm_ptr->remote.work & IPCP_N_COMPRESS_IPHC))
  {
    pppi_iphc_free( fsm_ptr->device, IPCP );
  }
#endif /* FEATURE_DATA_PS_PPP_IPHC */

} /* ipcp_free() */

/*===========================================================================
FUNCTION IPCP_GET_LOCAL_WANT_CB_VALS()

DESCRIPTION
  Access function IPCP local want control block
 
PARAMETERS
  Index: Array index in IPCP local want control block array.
 
RETURN VALUE
  IPCP local want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_local_want_cb_vals
( 
  uint32 index 
)
{
  return &ipcp_local_want_cb[index];
}

/*===========================================================================
FUNCTION IPCP_GET_LOCAL_WORK_CB_VALS()

DESCRIPTION
  Access function IPCP local work control block
 
PARAMETERS
  Index: Array index in IPCP local work control block array.
 
RETURN VALUE
  IPCP local work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_local_work_cb_vals
( 
  uint32 index 
)
{
  return &ipcp_local_work_cb[index];
}

/*===========================================================================
FUNCTION IPCP_GET_REMOTE_WANT_CB_VALS()

DESCRIPTION
  Access function IPCP remote want control block
 
PARAMETERS
  Index: Array index in IPCP remote want control block array.
 
RETURN VALUE
  IPCP remote want control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_remote_want_cb_vals
( 
  uint32 index 
)
{
  return &ipcp_remote_want_cb[index];
}

/*===========================================================================
FUNCTION IPCP_GET_REMOTE_WORK_CB_VALS()

DESCRIPTION
  Access function IPCP remote work control block
 
PARAMETERS
  Index: Array index in IPCP remote work control block array.
 
RETURN VALUE
  IPCP remote work control block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_remote_work_cb_vals
( 
  uint32 index 
)
{
  return &ipcp_remote_work_cb[index];
}

/*===========================================================================
FUNCTION IPCP_GET_DEFAULT_PTR()

DESCRIPTION
  Access function IPCP default values.
 
PARAMETERS
  None
 
RETURN VALUE
  IPCP default values

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipcp_value_type* ipcp_get_default_opts_vals_ptr()
{
  return &ipcp_default;
}

/*===========================================================================
FUNCTION IPCP_GET_ACCEPT()

DESCRIPTION
  Access function IPCP accept bitmask.
 
PARAMETERS
  None
 
RETURN VALUE
  IPCP accept bitmask.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
negotiate_t ipcp_get_accept_mask(void)
{
  return ipcp_accept;
}

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION PPP_IPCP_SET_INPUT_FUNCS()

DESCRIPTION
  Set the function pointers in the iface to input packets and config the
  iface.  A value of NULL in the passed funtion pointer causes the function
  pointer in the IPCP control block to be set to default.

DEPENDENCIES
  None

RETURN VALUE
  -1 in case of error
   0 in case of success

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ipcp_set_input_funcs
(
  ppp_dev_enum_type        device,
  ppp_iface_input_f_type   iface_input,
  ppp_iface_config_f_type  iface_config
)
{
  ppp_type *ppp_cb_array = NULL;
  ppp_fsm_type *fsm_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ppp_cb_array = fsm_get_ppp_cb_array();
  fsm_ptr = &ppp_cb_array[device].fsm[IPCP];

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  if (iface_input == NULL)
  {
    fsm_ptr->iface_input = (ppp_iface_input_f_type)ps_iface_input;
  }
  else
  {
    fsm_ptr->iface_input = iface_input;
  }
  if (iface_config == NULL)
  {
    fsm_ptr->iface_config = (ppp_iface_config_f_type)ipcpi_iface_config;
  }
  else
  {
    fsm_ptr->iface_config = iface_config;
  }
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  return 0;
} /* ppp_set_input_funcs() */
#endif /* FEATURE_STA_PPP */
