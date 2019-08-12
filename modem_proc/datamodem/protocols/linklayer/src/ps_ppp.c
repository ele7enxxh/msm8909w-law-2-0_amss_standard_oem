
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               P S _ P P P . C

GENERAL DESCRIPTION
  All of the external PPP protocol suite interface functions.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp.c_v   1.16   31 Jan 2003 18:46:32   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/23/14    skc    Removing FEATURE_DATA_PS_HDLC_PP
06/06/14    vl     BSS optimization changes
11/03/13    cx     Set bridge device in ppp_start() in order to support 2nd 
                   RM call in PDP-PPP call
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
10/25/12    fn     Remove usage of fatal error log messages
02/08/12    ash    Added NCPI_CB_IS_INITED Macro to check initilization status 
                   of VSNCP control block.
12/22/11    scb    Added support for IPv6 over PPP-DUN calls
11/28/11    jz     ppp control packet on dos(FEATURE_DATA_SEND_PPP_CTL_ON_DOS) 
11/02/11    jee    Fix graceful termination when phys link flow not enabled
10/17/11    ssh    eHRPD Always On support
09/20/11    jee    Fix to handle new call when device manager is cleaning up
09/14/11    ash    Cancel PPP setup times after AUTH success
06/06/11    ash    Added support for PPP Partial Context
05/16/11    ssh    Protect writes to device cache and inited flag
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
04/19/10    ash    Added changes to support Dual IP Feature.
03/09/10    mga    Added ppp_is_device_configured 
11/18/09    jee    Changes for the MMGSDI re-arch
10/01/09    mga    Added ppp_device_reset
08/31/09    mga    Merged from eHRPD branch
07/14/09    pp     ppp_cb_lock changes.
06/09/09    mga    Merged Compiler warnings fixes
05/07/09    pp     CMI Phase-4: SU Level API Effort.
12/12/08    scb    Adding PPP_STARTING_EV_MASK check to deregistration API.
11/19/08    scb    Fixed P2 framer and deframer issues.
11/12/08    scb    Added protocol processor specific functionality.
11/10/08    scb    Fixed QTF compilation warnings.
09/12/08    pp     Metainfo optimizations.
09/02/08    rnp    Added STA related changes.
05/21/08    scb    High LINT error fixes
05/04/08    scb    Bug fixes caught during PPP STA development
04/11/08    scb    Klocwork fixes
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
12/19/07    scb    Added the framer_complete_cb function for the protocol
                   processor framer functionality.
11/26/07    scb    Added the ppp_get_phase() API to obtain the current phase
                   PPP is in. Added for FEATURE_CHUM featurization.
08/27/07    scb    Added the PPP_STARTING_EV_MASK for the PPP_PROTOCOL_AUTH
                   protocol in ppp_event_register as a part of the UNDP 
                   requirements.
09/13/07    scb    Replaced the ppp_update_ipcp_config() function with a
                   more generic ppp_update_proto_config() function.
07/24/07    scb    Modified critical section around session handle check in 
                   ppp_abort() to avert race condition.
07/14/07    scb    Added critical section around session handle check in 
                   ppp_abort() to avert race condition.
07/06/07    scb    Added the auth_proto_mask variable to 
                   ppp_set_auth_server_opts() and set it to perform both PAP
                   and CHAP.
05/14/07    scb    Changed variable names to avoid compilation error for
                   JCDMA_1X builds
04/27/07    scb    Added ppp_get_device_constant() to return default values 
                   for the specified PPP constant,originally populated during
                   startup.
04/27/07    scb    Removed LINT suppressions
04/19/07    scb    Added support for the KDDI PPP compliance requirement
                   to configure Application specific PPP constants
04/12/07    ssh    Disable DNS negotiation for resyncs (JCDMA)
02/28/07    ssh    Removed VJ from IPv6CP code
02/08/07    scb    Fixed Klocwork High errors
01/08/07    ssh    Added PPP_STARTING_EV
11/01/06    ssh    Updated ppp_update_auth_setting to support auth_proto_mask
10/12/06    ssh    JCDMA: Setting abort_flag to FALSE for AN device
08/04/06    ssh    Now using featurized critical sections for event cbacks
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link and added DPL PPP Partial Logging
06/07/06    ssh    Added ppp_get_hdlc_handle()
05/04/06    ssh    Added ppp_update_password() and ppp_update_user_id()
04/28/06    ifk    Added additional critical sections.
04/20/06    ssh    Changed ppp_input() to handle all PPP modes (including
                   pseudonet) by invoking rx_pkt_hdlr_f_ptr
04/05/06    ifk    Added support for initializing IPv6 compression option.
03/06/06    ssh    Fixed HDLC_BAD_HANDLE processing in ppp_input()
02/21/06    ssh    Fixed pointer handling in ppp_input(), change to
                   pppi_config_check().
02/06/06    ssh    Changed TASKLOCKs to critical sections (for L4 builds).
01/18/06    ssh    pppi_config_check() should check for invalid LCP/HDLC mode
01/12/06    ssh    ppp_input() and ppp_output() should take PPP device as arg
12/09/05    sv     Added support for new data path framework.
11/29/05    jd     Call PPP logging macro wrapper to reduce CPU loading
11/29/05    ssh    Moved protocol mode setting from DS context to PS context,
                   fixed session_handle setting in ppp_start()
11/29/05    ssh    Logging incoming packet before discarding in ppp_input()
11/28/05  ifk/ssh  Added support for IPHC and ROHC over PPP under
                   FEATURE_DATA_PS_PPP_IPHC and FEATURE_DATA_PS_PPP_ROHC.
09/01/05    ssh    Changed hdlc_unframe() invokation in ppp_input() to
                   match new semantic
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/11/05    ssh    Changed ppp_set_mode(), now setting PPP mode only in
                   internal function pppi_set_mode()
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
06/29/05    ssh    Fixed logging issue in ppp_input()
06/01/05    rt     Added ps_ppp_logging.h inclusion.
05/31/05    jd     Added legacy PPP logging in ppp_input()
05/19/05    ssh    Error handling for hdlc_unframe() in ppp_input()
04/27/05    mct    Fixed session_handle to assign value to configbuf_ptr, in
                   cases where ppp cleanup doesn not occur.
04/11/05    ssh    Added handling for session_handle wraparound and passing
                   session_handle in config_buf_ptr in ppp_start()
01/19/05    jd     Added ppp_update_auth_setting to update auth requirement
                   at runtime.
11/01/04    rr/ks  Added primary_nbns and secondary_nbns addresses as input
                   parameters to ppp_set_v4_server_opts() for WINS. Used by
                   UMTS. For 1x these are passed in as zeros by the mode
                   handler as 1x currently doesn't support WINS.
10/18/04    ifk    In ppp_set_mode() check for bridge device if in psuedo
                   net or full net mode.
09/30/04    ifk    Changed check for all NCPs being disabled to print a LOG_MSG_INFO2
                   instead of returning an error from pppi_config_check()
09/25/04    vas    Added check for device in ppp_config_check()
07/15/04    ifk    Changed enum values of cp_mode_type, seperated PPP_RESYNC
                   command from PPP_START.  Added protocol to ppp_resync()
06/15/04    ifk    Took out function ppp_reg_event() added ppp_event_alloc(),
                   ppp_event_register(), ppp_event_deregister() and
                   ppp_event_dealloc()
06/11/04    vp     Changes for representation of IP addresses as struct ps_in_addr.
05/03/04    sy     Added rx signal handler to PPP interface. Also added
                   ppp_inout() which uses the frames received on the
                   forward link pass it to the HDLC deframer.
04/30/04    usb    Removed func callback to get peer info,
                   can be retrieved directly from iface_cb.
04/21/04    ifk    Clear an NCP from ncp_mask in ppp_stop() to avoid race
                   condition.
04/09/04    ifk    Check bridge device in ppp_set_mode() to PS task context.
03/15/04    ifk    Added PPP configuration buffer moving code changing ppp_cb
                   to PS context function pppi_start() from ppp_start() and
                   ppp_resync()
02/26/04    usb    Reset iface_ptr only in PPP task context not in ppp_abort.
02/03/04    sv     Changed IP6CP to IPV6CP.
01/31/04    usb    Support for new config item passive_wakeup_on_data.
01/08/04    aku    Changed check in ppp_stop() to include IP6CP.
11/21/03    jd     In ppp_resync(), set all passive protocols to active
                   before restarting PPP.
10/30/03    mvl    changed ppp_get_auth_info() to return auth_id.
10/09/03    ifk    Removed setting of magic number in ppp_get_default_opts()
09/25/03    ifk    Added IPv6CP specific code
09/18/03    jd     In ppp_get_dev_opts(), return auth info if device
                   authenticates, and clear auth info if not.
                   In set_auth_server_opts, if no challenge name provided,
                   set challenge name length to 0.
09/16/03    usb    Passing user data in call to resync event callback.
08/15/03    aku    Use ps_iface macro to access graceful_dormant_close
                   variable
08/13/03    ifk    Event handling update and seperation of LCP and IPCP as
                   precursor for IPv6 support.
05/28/03    usb    Fixed ppp_resync to prevent potential race conditions
05/22/03    usb    Make sure PPP device is configured before changing mode
                   in ppp_set_mode(), also warpped under TASKLOCK.
05/14/03    usb    Corrected auth parameter validation in ppp_i_config.
04/23/03    usb    Fixed ppp_resync() to pass in the correct device and
                   session handle wih the cmd to PS. Fixed
                   ppp_set_v4_server_opts() to not assign DNS addresses if
                   not asked by peer.
04/18/03    vsk    Return NULL if Auth was not performed in ppp_get_auth_info()
04/18/03    vsk    initialize serves_address in ppp_update_ipcp_config()
04/11/03    mvl    Added support for SNOOP_INTERNAL mode to set_mode(), Removed
                   INLINE to fix link issue and added the
                   set_auth_server_opts().
04/08/03    mvl    Cleanup up file
04/07/03    usb    Renamed ppp_set_resync_opts() to
                   ppp_update_ipcp_config(), Added ppp_inject_packet()
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP.
03/10/03    usb    Fixed ppp_set_resnc_options() to complete set of options,
                   Fixed ppp_set_v4_server_opts() to appropriately turn the
                   bits off for LOCAL side and made local_addr optional.
01/27/03    usb    PPP mode is always set in PS context, added
                   PPP_SET_MODE_CMD
12/10/02    usb    Moved setting PPP mode to NONE from ppp_abort to
                   pppi_abort (PS context).
11/14/02    jd/usb Split get_dev_opts into get_dev_setup & get_protocol_opts
                   removed 'swap' param from get dev opts, added
                   get_bridge_dev_opts for this purpose.
                   Added set_resync_opts for getting iface ready for snoop
                   resync (sets will/want with given config)
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user data to ppp config options so that they can be
                   passed on to ppp tx and rx functions
09/19/02    mvl    Added ppp_set_v4_server_opts() to make server config.
                   Making sure that the bridge device is set when switching
                   to psuedo or full net mode.  Fixed some issues with new
                   serves_addr field in configuration.
09/18/02    mvl    Added user_data to ppp event callbacks.  Setting mode in
                   start() and abort() calls, not internally.  Added explicit
                   configuration for client vs. server behavior.  Removed
                   do_mip.
09/12/02    mvl    Added support for a setup timeout, TASKLOCK()ed
                   ppp_set_mode().
09/06/02    mvl    fixed assert()s.
08/13/02    mvl    Session handle support and moved set_mode function from
                   netmdl.
08/06/02    mvl    Reworked authentication.
07/31/02    usb    Renamed psmisc.h to ps_utils.h
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
07/29/02    mvl    Setting do_auth variable when configuring PPP, fixed
                   TASKLOCK()s.
07/25/02    mvl    Fixed bug in get_default_opts() where values were not
                   being set correctly.  Updates for PPP renaming.
07/24/02    mvl    Fixed TASKLOCK() bug, and can now handle stopping/aborting
                   a device that is not running.
07/16/02    mvl    Configuration related fixes.
07/11/02    mvl    Updates for new PPP architecture.
07/02/02    mvl    Changes to decouple from PS.  Added ppp_reg_event().
05/22/02    mvl    created file with all of the external information for
                   users of ppp.
===========================================================================*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "rex.h"
#include "task.h"
#include "dsm.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_task.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_lcp.h"
#include "ps_pppi_lcp.h"
#include "ps_ppp_auth.h"
#include "ps_pppi_auth.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi_ipcp.h"
#include "ps_pppi_fsm.h"
#include "ps_iface.h"
#include "ps_mem.h"
#include "ps_phys_link.h"
#include "ps_system_heap.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ppp_ipv6cp.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ps_utils.h"
#include "ps_hdlc_lib.h"
#include "ps_stat_ppp.h"

#include "ps_logging.h"

#ifdef FEATURE_DATA_PS_PPP_LOGGING
#include "ps_ppp_logging.h"
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

#ifdef FEATURE_DATA_PS_EHRPD
#include "ps_ppp_ncp.h"
#include "ps_ppp_ncpi.h"
#endif /* FEATURE_DATA_PS_EHRPD */

#include "ds_Utils_DebugMsg.h"

#ifdef FEATURE_DATA_PKT_CLASSIFIER
#include "ds707_pkt_classifier.h"
#endif /* FEATURE_DATA_PKT_CLASSIFIER */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#define PPP_UPDATE_PROTO_CONFIG(proto_info, proto_value_type)      \
 {\
  fsm_ptr->local.will  = proto_info.will_mask[FSM_LOCAL]; \
  fsm_ptr->local.want  = proto_info.want_mask[FSM_LOCAL]; \
  fsm_ptr->remote.will = proto_info.will_mask[FSM_REMOTE];\
  fsm_ptr->remote.want = proto_info.want_mask[FSM_REMOTE];\
  fsm_ptr->local.work  = fsm_ptr->local.want;                         \
  fsm_ptr->remote.work = fsm_ptr->remote.want;                        \
  memscpy(fsm_ptr->remote.want_pdv,                                    \
         sizeof(proto_value_type),                                    \
         &(proto_info.want_vals[FSM_REMOTE]),              \
         sizeof(proto_value_type));                                    \
  memscpy(fsm_ptr->local.want_pdv,                                     \
         sizeof(proto_value_type),                                    \
         &(proto_info.want_vals[FSM_LOCAL]),               \
         sizeof(proto_value_type));                                    \
  memscpy(fsm_ptr->remote.work_pdv,                                    \
         sizeof(proto_value_type),                                    \
         &(proto_info.work_vals[FSM_REMOTE]),              \
         sizeof(proto_value_type));                                    \
  memscpy(fsm_ptr->local.work_pdv,                                     \
         sizeof(proto_value_type),                        \
         &(proto_info.work_vals[FSM_LOCAL]),               \
         sizeof(proto_value_type));                        \
 }


/*---------------------------------------------------------------------------
  struct ppp_dev_const_s_type
  Structure for setting/getting the constant parameters on a particular
  interface. 
---------------------------------------------------------------------------*/
typedef struct ppp_dev_const_s
{
  uint8  lcp_try_configure;
  uint16 lcp_ack_timeout;
  uint8  ipcp_try_configure;
  uint16 ipcp_ack_timeout;
  uint8  auth_retry_limit;
  uint16 auth_timeout;
#ifdef FEATURE_DATA_PS_IPV6
  uint8  ipv6cp_try_configure;
  uint16 ipv6cp_ack_timeout;
#endif /* FEATURE_DATA_PS_IPV6 */
} ppp_dev_const_s_type;

static ppp_dev_const_s_type ppp_def_dev_const[PPP_MAX_DEV];


static int pppi_config_check
(
  ppp_dev_opts_type *opts_ptr
);

#ifdef FEATURE_DATA_PS_EHRPD
/*---------------------------------------------------------------------------
  An array to hold device configuration provied by the PPP client via 
  ppp_device_configure(). The configuration is applied when the appropriate
  control protocol needs to be brought up.
---------------------------------------------------------------------------*/
ppp_dev_config_type *ppp_device_config[PPP_MAX_DEV] = {NULL};
#endif /* FEATURE_DATA_PS_EHRPD */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPP_GET_DEFAULT_OPTS()

DESCRIPTION
  This function will populate the options data structure with the default
  values for a PPP session.  The intention is to allow users to get a default
  configuration and modify only the fields that they wish and the fields that
  are set to invalid values (the owner of the configuration structure must
  set these values by hand).

  The following fields are set to invalid values:
    lcp_info.mode   -> PPP_CP_MODE_INVALID
    ipcp_info.mode  -> PPP_CP_MODE_INVALID
    rx_f_ptr        -> NULL
    rx_iface_ptr    -> NULL
    hdlc_mode       -> HDLC_INVALID_MODE
    bridge_dev      -> PPP_INVALID_DEV
    serves_addrs    -> FALSE
    auths_peer      -> FALSE
    rx_signal       -> PPP_INVALID_SIGNAL
    rx_sig_f_ptr    -> pppi_get_data_cback;

  NOTE: authentication is not requested in LCP, the desired local IPCP values
  for compression are used.

PARAMETERS
  config_ptr: configuration structure to fill in with the default information

RETURN VALUE
  The passed structure of type ppp_dev_opts_type is initialized with default
  options.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_get_default_opts
(
  ppp_dev_opts_type *config_ptr
)
{
  lcp_value_type    *lcp_default_opts_vals_ptr;
  ipcp_value_type   *ipcp_default_opts_vals_ptr;
  ipv6cp_value_type *ipv6cp_default_opts_vals_ptr;
  negotiate_t        lcp_accept;
  negotiate_t        ipcp_accept;
  negotiate_t        ipv6cp_accept;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == config_ptr )
  {
    DATA_ERR_FATAL("NULL config_ptr passed");
    return;
  }

  /*------------------------------------------------------------------------- 
    Get LCP default values & accept bitmask.
  -------------------------------------------------------------------------*/
  lcp_default_opts_vals_ptr = lcp_get_default_opts_vals_ptr();
  lcp_accept                = lcp_get_accept_mask();

  /*------------------------------------------------------------------------- 
    Get IPCP default values & accept bitmask.
  -------------------------------------------------------------------------*/
  ipcp_default_opts_vals_ptr = ipcp_get_default_opts_vals_ptr();
  ipcp_accept                = ipcp_get_accept_mask();

  /*------------------------------------------------------------------------- 
    Get IPv6CP default values & accept bitmask.
  -------------------------------------------------------------------------*/
  ipv6cp_default_opts_vals_ptr = ipv6cp_get_default_opts_vals_ptr();
  ipv6cp_accept                = ipv6cp_get_accept_mask();


  memset(config_ptr, 0, sizeof(ppp_dev_opts_type));

  /*-------------------------------------------------------------------------
    The other default values have been set to zero by the memset above, but
    the HDLC mode, the bridge dev and the rx_signal need to be set explicitly
    to their non zero invalid settings.
  -------------------------------------------------------------------------*/
  config_ptr->hdlc_mode  = HDLC_INVALID_MODE;
  config_ptr->bridge_dev = PPP_INVALID_DEV;
  config_ptr->rx_signal  = PPP_INVALID_SIGNAL;
  config_ptr->rx_sig_f_ptr = pppi_get_data_cback;

  /*-------------------------------------------------------------------------
    Set up LCP options to first request defaults - set the mode to invalid,
    then setup the masks: we want to negotiate ACCM, ACFC. PFC and the magic
    number (but NOT auth).  For the peer we will not force them to do
    anything, but will accept anything.  We will accept whatever is defined
    in lcp_accept.
  -------------------------------------------------------------------------*/
  config_ptr->lcp_info.mode = PPP_CP_MODE_INVALID;
  config_ptr->lcp_info.will_mask[FSM_REMOTE] = lcp_accept;
  config_ptr->lcp_info.will_mask[FSM_LOCAL]  = lcp_accept;
  config_ptr->lcp_info.want_mask[FSM_REMOTE] = 0;
  config_ptr->lcp_info.want_mask[FSM_LOCAL]  = LCP_N_ACCM | LCP_N_ACFC |
                                               LCP_N_PFC  | LCP_N_MAGIC;

  /*-------------------------------------------------------------------------
    setup the want and work protocol dependent variables using the default
    values - the only exception being the local want, where we prefer to have
    more optimized values for ACCM
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->lcp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(lcp_value_type),
           lcp_default_opts_vals_ptr,
           sizeof(lcp_value_type));
  config_ptr->lcp_info.want_vals[FSM_LOCAL].accm = 0;
  memscpy((config_ptr->lcp_info.work_vals) + (uint8)FSM_LOCAL,
           sizeof(lcp_value_type),
           (config_ptr->lcp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(lcp_value_type));
  memscpy((config_ptr->lcp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(lcp_value_type),
           lcp_default_opts_vals_ptr,
           sizeof(lcp_value_type));
  memscpy((config_ptr->lcp_info.work_vals) + (uint8)FSM_REMOTE,
           sizeof(lcp_value_type),
           (config_ptr->lcp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(lcp_value_type));

  /*-------------------------------------------------------------------------
    Set up IPCP options to first request defaults - set the mode to active,
    then setupthe masks: we want to negotiate compression, an address and the
    DNS server addresses.  We will accept whatever is defined in ipcp_accept.
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.mode = PPP_CP_MODE_INVALID;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] = ipcp_accept;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL]  = ipcp_accept;
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] = IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL]  = ipcp_accept;

  /*-------------------------------------------------------------------------
    Turn off WINS bits from will and want masks to ensure that 1x is not
    affected by WINS addition
  -------------------------------------------------------------------------*/

  config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~(IPCP_N_PRIMARY_NBNS |
                                                   IPCP_N_SECONDARY_NBNS);
  config_ptr->ipcp_info.will_mask[FSM_LOCAL]  &= ~(IPCP_N_PRIMARY_NBNS |
                                                   IPCP_N_SECONDARY_NBNS);
  config_ptr->ipcp_info.want_mask[FSM_LOCAL]  &= ~(IPCP_N_PRIMARY_NBNS |
                                                   IPCP_N_SECONDARY_NBNS);

  /*-------------------------------------------------------------------------
    setup the want protocol dependent variables using the default values
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->ipcp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(ipcp_value_type),
           ipcp_default_opts_vals_ptr,
           sizeof(ipcp_value_type));
  memscpy((config_ptr->ipcp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(ipcp_value_type),
           ipcp_default_opts_vals_ptr,
           sizeof(ipcp_value_type));

  /*-------------------------------------------------------------------------
    setup the compression protocol information
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].compression = PPP_VJCH_PROTOCOL;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].vj_config.num_slots =
    IPCP_SLOT_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].vj_config.slot_comp =
    IPCP_SLOT_COMPRESS;
#ifdef FEATURE_DATA_PS_PPP_ROHC
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.max_cid =
    IPCP_ROHC_MAX_CID_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.mrru =
    IPCP_ROHC_MRRU_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.max_header =
    IPCP_ROHC_MAX_HEADER_DEFAULT;
  /*-------------------------------------------------------------------------
   RFC 3242 2.2.  PROFILES Suboption
   The set of profiles to be enabled is subject to negotiation.  Most
   initial implementations of ROHC implement profiles 0x0000 to 0x0003.
   This option MUST be supplied.
   Including profiles 0x0000 through 0x0003 for now as RFC suggests.
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.no_of_profiles = 3;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.profiles[0] =
    ROHC_PROFILE_UNCOMPRESSED;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.profiles[1] =
    ROHC_PROFILE_RTP;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.profiles[2] =
    ROHC_PROFILE_UDP;
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.tcp_space =
    IPCP_IPHC_TCP_SPACE_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.non_tcp_space =
    IPCP_IPHC_NON_TCP_SPACE_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.f_max_period =
    IPCP_IPHC_F_MAX_PERIOD_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.f_max_time =
    IPCP_IPHC_F_MAX_TIME_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.max_header =
    IPCP_IPHC_MAX_HEADER_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.rtp_enabled = FALSE;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

  /*-------------------------------------------------------------------------
    Copy the work protocol dependent values from the want values.
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->ipcp_info.work_vals) + (uint8)FSM_LOCAL,
           sizeof(ipcp_value_type),
           (config_ptr->ipcp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(ipcp_value_type));
  memscpy((config_ptr->ipcp_info.work_vals) + (uint8)FSM_REMOTE,
           sizeof(ipcp_value_type),
           (config_ptr->ipcp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(ipcp_value_type));

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Set up IPV6CP options to first request defaults - set the mode to
    invalid, then set up the masks: we want to negotiate compression and
    an iface.    We will accept whatever is defined in ipv6cp_accept.
  -------------------------------------------------------------------------*/
  config_ptr->ipv6cp_info.mode = PPP_CP_MODE_INVALID;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] = ipv6cp_accept;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL]  = ipv6cp_accept;
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] = IPV6CP_N_IFACE_ID;
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL]  = ipv6cp_accept;

  /*-------------------------------------------------------------------------
    setup the want protocol dependent variables using the default values
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->ipv6cp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(ipv6cp_value_type),
           ipv6cp_default_opts_vals_ptr,
           sizeof(ipv6cp_value_type));
  memscpy((config_ptr->ipv6cp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(ipv6cp_value_type),
           ipv6cp_default_opts_vals_ptr,
           sizeof(ipv6cp_value_type));

  /*-------------------------------------------------------------------------
    setup the compression protocol information
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_PPP_ROHC
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.max_cid =
    IPV6CP_ROHC_MAX_CID_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.mrru =
    IPV6CP_ROHC_MRRU_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.max_header =
    IPV6CP_ROHC_MAX_HEADER_DEFAULT;
  /*-------------------------------------------------------------------------
   RFC 3242 2.2.  PROFILES Suboption
   The set of profiles to be enabled is subject to negotiation.  Most
   initial implementations of ROHC implement profiles 0x0000 to 0x0003.
   This option MUST be supplied.
   Including profiles 0x0000 through 0x0003 for now as RFC suggests.
  -------------------------------------------------------------------------*/
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.no_of_profiles = 3;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.profiles[0] =
    ROHC_PROFILE_UNCOMPRESSED;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.profiles[1] =
    ROHC_PROFILE_RTP;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.profiles[2] =
    ROHC_PROFILE_UDP;
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.tcp_space =
    IPV6CP_IPHC_TCP_SPACE_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.non_tcp_space =
    IPV6CP_IPHC_NON_TCP_SPACE_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.f_max_period =
    IPV6CP_IPHC_F_MAX_PERIOD_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.f_max_time =
    IPV6CP_IPHC_F_MAX_TIME_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.max_header =
    IPV6CP_IPHC_MAX_HEADER_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.rtp_enabled = FALSE;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

  /*-------------------------------------------------------------------------
    Copy the work protocol dependent values from the want values.
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->ipv6cp_info.work_vals) + (uint8)FSM_LOCAL,
          sizeof(ipv6cp_value_type),
          (config_ptr->ipv6cp_info.want_vals) + (uint8)FSM_LOCAL,
          sizeof(ipv6cp_value_type));
  memscpy((config_ptr->ipv6cp_info.work_vals) + (uint8)FSM_REMOTE,
           sizeof(ipv6cp_value_type),
           (config_ptr->ipv6cp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(ipv6cp_value_type));
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
    Set up the default values for the world phone related PPP options.
  -------------------------------------------------------------------------*/
  config_ptr->ppp_failure_graceful_close = FALSE;
  config_ptr->set_stabilize_accm_delay = TRUE;
  config_ptr->resync_dns_opts = TRUE;

#ifdef FEATURE_DATA_PS_ENFORCE_AUTH
  /*-------------------------------------------------------------------------
    Set the allow_no_auth to FALSE. This is done to mandate auth for eHRPD.
  -------------------------------------------------------------------------*/
  config_ptr->allow_no_auth = FALSE;
#else
  config_ptr->allow_no_auth = TRUE;
#endif /* FEATURE_DATA_PS_ENFORCE_AUTH */

#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  config_ptr->dos_is_enabled = FALSE;
#endif /*FEATURE_DATA_SEND_PPP_CTL_ON_DOS*/
} /* ppp_get_default_opts() */



/*===========================================================================
FUNCTION PPP_GET_PROTOCOL_OPTS()

DESCRIPTION
  This function will copy the framing options for a particular PPP
  device into the option structure provided by the caller.

PARAMETERS
  device:     PPP device to copy options from
  config_ptr: pointer to configuration structure in which to put information

RETURN VALUE
  -1 in case of error
   0 in case of success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_protocol_opts
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure the specified device is valid
  -------------------------------------------------------------------------*/
  if(device >= PPP_MAX_DEV || NULL == config_ptr)
  {
    LOG_MSG_ERROR_2( "Invalid arguments passed: device %d, config_ptr 0x%x",
                     device, config_ptr );
    ASSERT(0);
    return -1;
  }
  ppp_cb_array = fsm_get_ppp_cb_array();
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  /*-------------------------------------------------------------------------
    Copy the negotiated options - that is the work fields.
    First copy the protocol dependent variables
  -------------------------------------------------------------------------*/
  memscpy(&(config_ptr->lcp_info.work_vals[FSM_LOCAL]),
          sizeof(lcp_value_type),
          ppp_cb_array[device].fsm[LCP].local.work_pdv,
          sizeof(lcp_value_type));
  memscpy(&(config_ptr->lcp_info.work_vals[FSM_REMOTE]),
          sizeof(lcp_value_type),
          ppp_cb_array[device].fsm[LCP].remote.work_pdv,
          sizeof(lcp_value_type));
  memscpy(&(config_ptr->ipcp_info.work_vals[FSM_LOCAL]),
          sizeof(ipcp_value_type),
          ppp_cb_array[device].fsm[IPCP].local.work_pdv,
          sizeof(ipcp_value_type));
  memscpy(&(config_ptr->ipcp_info.work_vals[FSM_REMOTE]),
          sizeof(ipcp_value_type),
          ppp_cb_array[device].fsm[IPCP].remote.work_pdv,
          sizeof(ipcp_value_type));
#ifdef FEATURE_DATA_PS_IPV6
  memscpy(&(config_ptr->ipv6cp_info.work_vals[FSM_LOCAL]),
         sizeof(ipv6cp_value_type),
         ppp_cb_array[device].fsm[IPV6CP].local.work_pdv,
         sizeof(ipv6cp_value_type));
  memscpy(&(config_ptr->ipv6cp_info.work_vals[FSM_REMOTE]),
          sizeof(ipv6cp_value_type),
          ppp_cb_array[device].fsm[IPV6CP].remote.work_pdv,
          sizeof(ipv6cp_value_type));
#endif /* FEATURE_DATA_PS_IPV6 */
  memscpy(&(config_ptr->lcp_info.want_vals[FSM_LOCAL]),
          sizeof(lcp_value_type),
          ppp_cb_array[device].fsm[LCP].local.want_pdv,
          sizeof(lcp_value_type));
  memscpy(&(config_ptr->lcp_info.want_vals[FSM_REMOTE]),
          sizeof(lcp_value_type),
          ppp_cb_array[device].fsm[LCP].remote.want_pdv,
          sizeof(lcp_value_type));
  memscpy(&(config_ptr->ipcp_info.want_vals[FSM_LOCAL]),
          sizeof(ipcp_value_type),
          ppp_cb_array[device].fsm[IPCP].local.want_pdv,
          sizeof(ipcp_value_type));
  memscpy(&(config_ptr->ipcp_info.want_vals[FSM_REMOTE]),
         sizeof(ipcp_value_type),
         ppp_cb_array[device].fsm[IPCP].remote.want_pdv,
         sizeof(ipcp_value_type));
#ifdef FEATURE_DATA_PS_IPV6
  memscpy(&(config_ptr->ipv6cp_info.want_vals[FSM_LOCAL]),
          sizeof(ipv6cp_value_type),
          ppp_cb_array[device].fsm[IPV6CP].local.want_pdv,
          sizeof(ipv6cp_value_type));
  memscpy(&(config_ptr->ipv6cp_info.want_vals[FSM_REMOTE]),
          sizeof(ipv6cp_value_type),
          ppp_cb_array[device].fsm[IPV6CP].remote.want_pdv,
          sizeof(ipv6cp_value_type));
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
    Copy the negotatied masks (will and want)
  -------------------------------------------------------------------------*/
  config_ptr->lcp_info.will_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[LCP].local.will;
  config_ptr->lcp_info.will_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[LCP].remote.will;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[IPCP].local.will;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[IPCP].remote.will;
#ifdef FEATURE_DATA_PS_IPV6
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[IPV6CP].local.will;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[IPV6CP].remote.will;
#endif /* FEATURE_DATA_PS_IPV6 */
  config_ptr->lcp_info.want_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[LCP].local.work;
  config_ptr->lcp_info.want_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[LCP].remote.work;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[IPCP].local.work;
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[IPCP].remote.work;
#ifdef FEATURE_DATA_PS_IPV6
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL] =
    ppp_cb_array[device].fsm[IPV6CP].local.work;
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] =
    ppp_cb_array[device].fsm[IPV6CP].remote.work;
#endif /* FEATURE_DATA_PS_IPV6 */
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  return 0;
} /* ppp_get_protocol_opts() */



/*===========================================================================
FUNCTION PPP_GET_BRIDGE_DEV_PROT_OPTS()

DESCRIPTION
  This function will copy the framing options from a particular PPP
  device's bridged device into the specified config_ptr, swapping the
  local and remote options and values.

  This provides a means to obtain the options required to start ppp on the
  opposite side of an interface such that compatible ppp options are
  negotiated on both devices.

PARAMETERS
  device:     PPP device whose bridge device to copy options from
  config_ptr: pointer to configuration structure in which to put information

RETURN VALUE
  -1 in case of error
   0 in case of success
   The passed config_ptr is filled with the configuration information

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_bridge_dev_prot_opts
(
  ppp_dev_enum_type bridge_dev,
  ppp_dev_opts_type *config_ptr
)
{
  ppp_type *  ppp_cb_ptr;
  ppp_type *  ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure the specified bridge device is valid
  -------------------------------------------------------------------------*/
  if( bridge_dev >= PPP_MAX_DEV || NULL == config_ptr )
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[bridge_dev];

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(bridge_dev) );
  /*-------------------------------------------------------------------------
    Copy the desired options from the bridge device -
    we want the negotiated, i.e. work options.
  -------------------------------------------------------------------------*/
  memscpy(&(config_ptr->lcp_info.want_vals[FSM_REMOTE]),
          sizeof(lcp_value_type),
          ppp_cb_ptr->fsm[LCP].local.work_pdv,
          sizeof(lcp_value_type));
  memscpy(&(config_ptr->lcp_info.want_vals[FSM_LOCAL]),
          sizeof(lcp_value_type),
          ppp_cb_ptr->fsm[LCP].remote.work_pdv,
          sizeof(lcp_value_type));
  memscpy(&(config_ptr->ipcp_info.want_vals[FSM_REMOTE]),
          sizeof(ipcp_value_type),
          ppp_cb_ptr->fsm[IPCP].local.work_pdv,
          sizeof(ipcp_value_type));
  memscpy(&(config_ptr->ipcp_info.want_vals[FSM_LOCAL]),
          sizeof(ipcp_value_type),
          ppp_cb_ptr->fsm[IPCP].remote.work_pdv,
          sizeof(ipcp_value_type));
#ifdef FEATURE_DATA_PS_IPV6
  memscpy(&(config_ptr->ipv6cp_info.want_vals[FSM_REMOTE]),
          sizeof(ipv6cp_value_type),
          ppp_cb_ptr->fsm[IPV6CP].local.work_pdv,
          sizeof(ipv6cp_value_type));
  memscpy(&(config_ptr->ipv6cp_info.want_vals[FSM_LOCAL]),
          sizeof(ipv6cp_value_type),
          ppp_cb_ptr->fsm[IPV6CP].remote.work_pdv,
          sizeof(ipv6cp_value_type));
#endif /* FEATURE_DATA_IPV6 */

  /*-------------------------------------------------------------------------
    Copy the working options to work options.
  -------------------------------------------------------------------------*/
  memscpy(&(config_ptr->lcp_info.work_vals[FSM_REMOTE]),
          sizeof(lcp_value_type),
          ppp_cb_ptr->fsm[LCP].local.work_pdv,
          sizeof(lcp_value_type));
  memscpy(&(config_ptr->lcp_info.work_vals[FSM_LOCAL]),
          sizeof(lcp_value_type),
          ppp_cb_ptr->fsm[LCP].remote.work_pdv,
          sizeof(lcp_value_type));
  memscpy(&(config_ptr->ipcp_info.work_vals[FSM_REMOTE]),
          sizeof(ipcp_value_type),
          ppp_cb_ptr->fsm[IPCP].local.work_pdv,
          sizeof(ipcp_value_type));
  memscpy(&(config_ptr->ipcp_info.work_vals[FSM_LOCAL]),
          sizeof(ipcp_value_type),
          ppp_cb_ptr->fsm[IPCP].remote.work_pdv,
          sizeof(ipcp_value_type));
#ifdef FEATURE_DATA_PS_IPV6
  memscpy(&(config_ptr->ipv6cp_info.work_vals[FSM_REMOTE]),
          sizeof(ipv6cp_value_type),
          ppp_cb_ptr->fsm[IPV6CP].local.work_pdv,
          sizeof(ipv6cp_value_type));
  memscpy(&(config_ptr->ipv6cp_info.work_vals[FSM_LOCAL]),
          sizeof(ipv6cp_value_type),
          ppp_cb_ptr->fsm[IPV6CP].remote.work_pdv,
          sizeof(ipv6cp_value_type));
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
    We will only negotiate what the current settings are on the bridge
    device.  Set want to the current settings on bridge device as well.
  -------------------------------------------------------------------------*/
  config_ptr->lcp_info.will_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[LCP].local.work;
  config_ptr->lcp_info.will_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[LCP].remote.work;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[IPCP].local.work;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[IPCP].remote.work;
#ifdef FEATURE_DATA_PS_IPV6
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[IPV6CP].local.work;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[IPV6CP].remote.work;
#endif /* FEATURE_DATS_PS_IPV6 */
  config_ptr->lcp_info.want_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[LCP].local.work;
  config_ptr->lcp_info.want_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[LCP].remote.work;
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[IPCP].local.work;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[IPCP].remote.work;
#ifdef FEATURE_DATA_PS_IPV6
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] =
    ppp_cb_ptr->fsm[IPV6CP].local.work;
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL] =
    ppp_cb_ptr->fsm[IPV6CP].remote.work;
#endif /* FEATURE_DATA_PS_IPV6 */

  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(bridge_dev) );

  return 0;
} /* ppp_get_bridge_dev_prot_opts() */



/*===========================================================================
FUNCTION PPP_GET_AUTH_INFO()

DESCRIPTION
  This function will return a pointer to a data structure containing all of
  th authentication information.  The macro PPP_AUTH_WAS_CHAP() can be used
  to determine if CHAP or PAP was done.  The return value will be NULL if the
  device is out of bounds OR no authentication was performed.

  NOTE: this informaion should be treated as READ-ONLY changing values of the
        pointer information has unspecified effects.

PARAMETERS
  device: the PPP device from which to return the auth information.
  auth_info_ptr: pointer to auth_info_type where the auth_info will be copied.
  auth_id_ptr:   pointer to uint8 where the auth ID will be copied - if
                 passed in as NULL, will be ingored.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_auth_info
(
  ppp_dev_enum_type   device,
  ppp_auth_info_type *auth_info_ptr,
  uint8              *auth_id_ptr
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device  >= PPP_MAX_DEV ||
     auth_info_ptr == NULL)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  if( ppp_cb_array[device].auth.mode != PPP_NO_AUTH )
  {
    /*-----------------------------------------------------------------------
      copy the auth info into the passed in data structure.  If the auth_id
      pointere is valid then copy that auth ID in there as well.
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    memscpy(auth_info_ptr, 
            sizeof(ppp_auth_info_type),
            &(ppp_cb_array[device].auth.info),
            sizeof(ppp_auth_info_type));

    if(auth_id_ptr != NULL)
    {
      *auth_id_ptr = ppp_cb_array[device].auth.active_id;
    }
    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    return 0;
  }
  else
  {
    return -1;
  }

} /* ppp_get_auth_info() */



/*===========================================================================
FUNCTION PPP_GET_DEV_SETUP()

DESCRIPTION
  This function will copy the configuration options for a particular PPP
  device into the option structure passed in.

PARAMETERS
  device: PPP device to copy settings from
  config_ptr: pointer to configuration structure in which to put information

RETURN VALUE
  -1 in case of error
   0 in case of success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_dev_setup
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
)
{
  ppp_type *  ppp_cb_ptr;
  ppp_type *  ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure the specified device is valid
  -------------------------------------------------------------------------*/
  if( device >= PPP_MAX_DEV || NULL == config_ptr )
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get fields from specified device.
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  config_ptr->rx_f_ptr      = ppp_cb_ptr->rx_f_ptr;
  config_ptr->tx_f_ptr      = ppp_cb_ptr->tx_f_ptr;
  config_ptr->rx_user_data  = ppp_cb_ptr->rx_user_data;
  config_ptr->tx_user_data  = ppp_cb_ptr->tx_user_data;
  config_ptr->ipcp_info.rx_iface_ptr  =
                         ppp_cb_ptr->fsm[IPCP].rx_iface_ptr;
#ifdef FEATURE_DATA_PS_IPV6
  config_ptr->ipv6cp_info.rx_iface_ptr  =
                         ppp_cb_ptr->fsm[IPV6CP].rx_iface_ptr;
  config_ptr->ipv6cp_info.mode = ppp_cb_ptr->fsm[IPV6CP].mode;
#endif /* FEATURE_DATA_PS_IPV6 */
  config_ptr->hdlc_mode     = ppp_cb_ptr->hdlc_mode;
  config_ptr->rx_signal     = ppp_cb_ptr->rx_signal;
  config_ptr->rx_sig_f_ptr  = ppp_cb_ptr->rx_sig_f_ptr;
  config_ptr->bridge_dev    = ppp_cb_ptr->bridge_dev;
  config_ptr->serves_addrs  = ppp_cb_ptr->serves_addrs;
  config_ptr->setup_timeout = (uint8)ppp_cb_ptr->setup_timeout;
  config_ptr->lcp_info.mode  = ppp_cb_ptr->fsm[LCP].mode;
  config_ptr->ipcp_info.mode = ppp_cb_ptr->fsm[IPCP].mode;
  config_ptr->passive_wakeup_on_data = ppp_cb_ptr->passive_wakeup_on_data;

  /*-------------------------------------------------------------------------
    if the auth flags indicate that we authenticate the remote side then the
    auths_peer field is true, otherwise not.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE)
  {
    config_ptr->auths_peer = TRUE;
    /*-----------------------------------------------------------------------
      Copy the auth info in this case
    -----------------------------------------------------------------------*/
    config_ptr->auth_info = ppp_cb_ptr->auth.info;
  }
  else
  {
    config_ptr->auths_peer = FALSE;
    /*-----------------------------------------------------------------------
      Null out the auth info in this case
    -----------------------------------------------------------------------*/
    memset(&(config_ptr->auth_info), 0, sizeof(ppp_auth_info_type));
  }
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  return 0;

} /* ppp_get_dev_setup() */



/*===========================================================================
FUNCTION PPP_UPDATE_PROTO_CONFIG()

DESCRIPTION
  This function reconfigures protocol options for the specified PPP device.
  This prepares PPP device for subsequent resyncs after it has already come
  up or allows reconfiguration while PPP is in the middle of coming up.

PARAMETERS
  device:        PPP device to configure
  config_ptr:    configuration to use

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies protocol options for the given PPP device
===========================================================================*/
void ppp_update_proto_config
(
  ppp_dev_enum_type       device,
  ppp_protocol_e_type     protocol,
  ppp_dev_opts_type       *config_ptr
)
{
  ppp_fsm_type * fsm_ptr = NULL;
  ppp_type     * ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure the specified device is valid, the config_ptr is non-NULL
    and the protocol is valid.
  -------------------------------------------------------------------------*/
  if( ( PPP_MAX_DEV <= device ) || ( NULL == config_ptr ) || 
      ( PPP_PROTOCOL_MAX <= protocol ) )
  {
    LOG_MSG_ERROR_2("Invalid params : device %d, protocol %d or NULL config_ptr",
                    device, protocol );
    ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  switch(protocol)
  {
    case PPP_PROTOCOL_LCP:
      fsm_ptr = &(ppp_cb_array[device].fsm[protocol]);
      PPP_UPDATE_PROTO_CONFIG(config_ptr->lcp_info, lcp_value_type);
      break;

    case PPP_PROTOCOL_IPCP:
      fsm_ptr = &(ppp_cb_array[device].fsm[protocol]);
      PPP_UPDATE_PROTO_CONFIG(config_ptr->ipcp_info, ipcp_value_type);
      /*---------------------------------------------------------------------
         Copy the flag which indicates if we are IPCP client or server
      ---------------------------------------------------------------------*/
      ppp_cb_array[device].serves_addrs = config_ptr->serves_addrs;
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case PPP_PROTOCOL_IPV6CP:
      fsm_ptr = &(ppp_cb_array[device].fsm[protocol]);
      PPP_UPDATE_PROTO_CONFIG(config_ptr->ipv6cp_info, ipv6cp_value_type);
      /*---------------------------------------------------------------------
         Copy the flag which indicates if we are IPv6CP client or server
      ---------------------------------------------------------------------*/
      ppp_cb_array[device].serves_addrs = config_ptr->serves_addrs;
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    case PPP_PROTOCOL_AUTH:
      if( ( 0 < config_ptr->auth_info.passwd_len) &&
          ( PPP_MAX_PASSWD_LEN >= config_ptr->auth_info.passwd_len ) )
      {
        memset( ppp_cb_array[device].auth.info.passwd_info,
                0,
                sizeof( ppp_cb_array[device].auth.info.passwd_info ) );
        memscpy( ppp_cb_array[device].auth.info.passwd_info,
                 PPP_MAX_PASSWD_LEN,
                 config_ptr->auth_info.passwd_info,
                 config_ptr->auth_info.passwd_len );
        ppp_cb_array[device].auth.info.passwd_len = 
                config_ptr->auth_info.passwd_len;
      }
      else
      {
        LOG_MSG_ERROR_0("Invalid password len passed to update_proto_config");
      }
      
      if( ( 0 < config_ptr->auth_info.user_id_len ) &&
          ( PPP_MAX_USER_ID_LEN >= config_ptr->auth_info.user_id_len ) )
      {
        memset( ppp_cb_array[device].auth.info.user_id_info,
                0,
                sizeof( ppp_cb_array[device].auth.info.user_id_info ) );
        memscpy( ppp_cb_array[device].auth.info.user_id_info,
                 PPP_MAX_USER_ID_LEN,
                 config_ptr->auth_info.user_id_info,
                 config_ptr->auth_info.user_id_len );
        ppp_cb_array[device].auth.info.user_id_len = 
                config_ptr->auth_info.user_id_len;
      }
      else
      {
        LOG_MSG_ERROR_0("Invalid userid len passed to update_proto_config");
      }
      if( ( 0 < config_ptr->auth_info.challenge_name_len ) &&
          ( PPP_CHAP_CHAL_NAME_LEN >=
            config_ptr->auth_info.challenge_name_len) )
      {
        memset( ppp_cb_array[device].auth.info.challenge_name,
                0,
                sizeof( ppp_cb_array[device].auth.info.challenge_name ) );
        memscpy( ppp_cb_array[device].auth.info.challenge_name,
                 PPP_CHAP_CHAL_NAME_LEN, 
                 config_ptr->auth_info.challenge_name,
                 config_ptr->auth_info.challenge_name_len );
        ppp_cb_array[device].auth.info.challenge_name_len = 
                config_ptr->auth_info.challenge_name_len;
      }
      else
      {
        LOG_MSG_ERROR_0("Invalid challenge name len passed to update_proto_config");
      }
      break;
    default:
      LOG_MSG_ERROR_0("Invalid protocol passed to update_proto_config");
      break;
  }

  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

} /* ppp_update_proto_config() */


/*===========================================================================
FUNCTION PPP_SET_V4_SERVER_OPTS()

DESCRIPTION
  This function will change the configuration passed in to serve IP
  addresses.  This includes setting the serves_addrs field, and setting the
  appropriate IPCP bits. An address must be passed in to be assigned to the
  peer. Other fields could be NULL and if so the corresponding bits are
  turned off.

PARAMETERS
  config_ptr:    configuration to convert
  local_addr:    the local IP address to advertize
  remote_addr:   the IP address to assign to peer
  primary_dns:   the primary DNS address to assign to peer
  secondary_dns: the secondary DNS address to assign to peer

RETURN VALUE
  The passed information is used to setup the passed config_ptr.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_v4_server_opts
(
  ppp_dev_opts_type *config_ptr,
  uint32             local_addr,
  uint32             remote_addr,
  uint32             primary_dns,
  uint32             secondary_dns,
  uint32             primary_nbns,
  uint32             secondary_nbns
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(config_ptr  == NULL || remote_addr == 0)
  {
    LOG_MSG_ERROR_2("Invalid config 0x%x 0x%x", config_ptr, remote_addr);
    return;
  }

  config_ptr->serves_addrs = TRUE;

  /*-------------------------------------------------------------------------
    Set the remote address,  and associated bits in the masks. Since we
    are acting as server we must have an address to assign.
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.work_vals[FSM_REMOTE].address.ps_s_addr = remote_addr;
  config_ptr->ipcp_info.want_vals[FSM_REMOTE].address.ps_s_addr = remote_addr;

  /*-------------------------------------------------------------------------
    if the local address is available, assign the values and set the
    LOCAL bits. Else turn them off since we don't want to ask for our
    address while acting as server.
  -------------------------------------------------------------------------*/
  if(local_addr != 0)
  {
    config_ptr->ipcp_info.want_mask[FSM_LOCAL] |= IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.will_mask[FSM_LOCAL] |= IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.work_vals[FSM_LOCAL].address.ps_s_addr = local_addr;
    config_ptr->ipcp_info.want_vals[FSM_LOCAL].address.ps_s_addr = local_addr;
  }
  else
  {
    config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
  }

  /*-------------------------------------------------------------------------
    If we have dns address to assign to peer then assign the values and set
    the REMOTE bits. LOCAL bits should always be turned off since we don't
    want to ask for DNS addresses while acting as server.
  -------------------------------------------------------------------------*/
  if(primary_dns != 0)
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_PRIMARY_DNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].primary_dns.ps_s_addr =
      primary_dns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].primary_dns.ps_s_addr =
      primary_dns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_DNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_DNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_DNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_DNS;

  if(secondary_dns != 0)
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_SECONDARY_DNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].secondary_dns.ps_s_addr =
      secondary_dns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].secondary_dns.ps_s_addr =
      secondary_dns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_DNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_DNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_DNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_DNS;

  /*-------------------------------------------------------------------------
    If we have nbns address to assign to peer then assign the values and set
    the REMOTE bits. LOCAL bits should always be turned off since we don't
    want to ask for NBNS addresses while acting as server.
  -------------------------------------------------------------------------*/
  if(primary_nbns != 0)
  {
    LOG_MSG_INFO1_1("PRIMARY NBNS ADDRESS ASSIGNED %lx", primary_nbns);
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_PRIMARY_NBNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].primary_nbns.ps_s_addr =
      primary_nbns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].primary_nbns.ps_s_addr =
      primary_nbns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_NBNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_NBNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_NBNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_NBNS;

  if(secondary_nbns != 0)
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_SECONDARY_NBNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].secondary_nbns.ps_s_addr =
      secondary_nbns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].secondary_nbns.ps_s_addr =
      secondary_nbns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_NBNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_NBNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_NBNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_NBNS;

} /* ppp_set_v4_server_opts() */



#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION PPP_SET_V6_SERVER_OPTS()

DESCRIPTION
  This function will change the configuration passed in to serve IP
  addresses.  This includes setting the addresses, serves_addrs, and the
  appropriate IPV6CP bits. A local IID (serves as gateway IID) and a 
  remote IID must be passed in to be assigned to the peer.

  Compression is currently not negotiated and is turned off.

PARAMETERS
  config_ptr:   configuration to convert
  local_iid:    gateway IID
  remote_iid:   IID to be assigned to the peer

RETURN VALUE
  The passed information is used to setup the passed config_ptr.

DEPENDENCIES
  NOTE: The local_iid and remote_iid must be in network byte order.

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_v6_server_opts
(
  ppp_dev_opts_type *config_ptr,
  uint64            local_iid ,
  uint64            remote_iid 
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == config_ptr || 0 == local_iid || 0 == remote_iid )
  {
    LOG_MSG_ERROR_2("ppp_set_v6_server_opts: Invalid config ptr"
                    " local_iid %d remote_iid %d", local_iid, remote_iid);
    return;
  }

  config_ptr->serves_addrs = TRUE;

  /*-------------------------------------------------------------------------
    Set the remote address,  and associated bits in the masks. Since we
    are acting as server we must have an address to assign.
  -------------------------------------------------------------------------*/
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] |= IPV6CP_N_IFACE_ID;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] |= IPV6CP_N_IFACE_ID;

  config_ptr->ipv6cp_info.work_vals[FSM_REMOTE].iid = remote_iid;
  config_ptr->ipv6cp_info.want_vals[FSM_REMOTE].iid = remote_iid;

  /*-------------------------------------------------------------------------
    Set the local address (G/W address),and the associated bits in the masks. 
  -------------------------------------------------------------------------*/
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL] |= IPV6CP_N_IFACE_ID;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL] |= IPV6CP_N_IFACE_ID;

  config_ptr->ipv6cp_info.work_vals[FSM_LOCAL].iid = local_iid;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iid = local_iid;

#ifdef FEATURE_DATA_PS_PPP_ROHC
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] &= ~IPV6CP_N_COMPRESS_ROHC;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] &= ~IPV6CP_N_COMPRESS_ROHC;
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL] &= ~IPV6CP_N_COMPRESS_ROHC;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL] &= ~IPV6CP_N_COMPRESS_ROHC;
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] &= ~IPV6CP_N_COMPRESS_IPHC;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] &= ~IPV6CP_N_COMPRESS_IPHC;
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL] &= ~IPV6CP_N_COMPRESS_IPHC;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL] &= ~IPV6CP_N_COMPRESS_IPHC;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

} /* ppp_set_v6_server_opts() */
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION PPP_SET_AUTH_SERVER_OPTS()

DESCRIPTION
  This function will setup the configuration to enable the receiving PPP
  instance to authenticate the peer.

PARAMETERS
  config_ptr: pointer to the configuration being modified
  challenge name: string indicating the name that should be sent as part of
      the chap challenge.  This can be NULL or the empty string if no name is
      needed (PAP), or wants to be sent.

RETURN VALUE
  -1 on error
  the number of bytes of challenge_name copied
      NOTE: this may be less than strlen(challenge_name) if that is greater
            than PPP_CHAP_CHAL_NAME_LEN.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_set_auth_server_opts
(
  ppp_dev_opts_type *config_ptr,
  char              *challenge_name
)
{
  unsigned int copy_len = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(config_ptr == NULL)
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set all of the authentication options up to do server style CHAP.
  -------------------------------------------------------------------------*/
  config_ptr->auths_peer = TRUE;
  config_ptr->lcp_info.work_vals[FSM_LOCAL].authentication =
    PPP_CHAP_PROTOCOL;
  config_ptr->lcp_info.work_vals[FSM_LOCAL].chap_digest = PPP_CHAP_MD5;
  config_ptr->lcp_info.work_vals[FSM_LOCAL].auth_proto_mask = 
    (ppp_auth_proto_e_type) ( (uint32)PPP_AUTH_PAP | (uint32)PPP_AUTH_CHAP );
  config_ptr->lcp_info.want_vals[FSM_LOCAL].authentication =
    PPP_CHAP_PROTOCOL;
  config_ptr->lcp_info.want_vals[FSM_LOCAL].chap_digest = PPP_CHAP_MD5;
  config_ptr->lcp_info.want_vals[FSM_LOCAL].auth_proto_mask = 
    (ppp_auth_proto_e_type) ( (uint32)PPP_AUTH_PAP | (uint32)PPP_AUTH_CHAP );
  config_ptr->lcp_info.want_mask[FSM_LOCAL] |= LCP_N_AP;
  config_ptr->lcp_info.will_mask[FSM_LOCAL] |= LCP_N_AP;

  /*-------------------------------------------------------------------------
    If the challenge name is non-NULL and the length is non-zero copy the
    challenge name.  Otherwise, set the challenge name length to 0.
  -------------------------------------------------------------------------*/
  if(challenge_name != NULL)
  {
    copy_len = strlen(challenge_name);
    copy_len = MIN(copy_len, PPP_CHAP_CHAL_NAME_LEN);
    config_ptr->auth_info.challenge_name_len = (uint8)copy_len;
    memscpy(config_ptr->auth_info.challenge_name, 
             PPP_CHAP_CHAL_NAME_LEN, 
             challenge_name, 
             copy_len);
  }
  else
  {
    config_ptr->auth_info.challenge_name_len = 0;
  }

  return (int)copy_len;
} /* ppp_set_auth_server_opts() */

/*===========================================================================
FUNCTION PPP_SET_CP_MODE()

DESCRIPTION
  This function is used to set the CP mode associated with a specific PPP 
  protocol and device.
 
DEPENDENCIES
  None

RETURN VALUE
  0 on success
 -1 on failure

SIDE EFFECTS
  Sets the CP mode in the FSM associated with a specific PPP protocol.
===========================================================================*/
int ppp_set_cp_mode
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type protocol,
  cp_mode_type        cp_mode
)
{
  ppp_type *ppp_cb_array = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( PPP_MAX_DEV <= device || 
      ( PPP_CP_MODE_INVALID > cp_mode  || PPP_CP_MODE_MAX <= cp_mode ) || 
      ( PPP_PROTOCOL_MIN > protocol || PPP_PROTOCOL_MAX <= protocol ) ) 
  {
    LOG_MSG_ERROR("Invalid args: device %d, cp_mode value %d "
                  "protocol value %d", device, cp_mode, protocol );
    ASSERT( 0 );
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();

  LOG_MSG_INFO2("ppp_set_cp_mode: Device %d's FSM[%d] has old cp mode %d",
	   device, protocol, ppp_cb_array[device].fsm[protocol].mode);

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  ppp_cb_array[device].fsm[protocol].mode         =
        pppi_ncp_mode_change( ppp_cb_array[device].fsm[protocol].mode,
                              cp_mode );
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  LOG_MSG_INFO2("ppp_set_cp_mode: Device %d's FSM[%d] has new cp mode %d",
	   device, protocol, ppp_cb_array[device].fsm[protocol].mode);

  return 0;

} /* ppp_set_cp_mode() */

#ifdef FEATURE_DATA_PS_EHRPD
/*===========================================================================
FUNCTION PPP_EVENT_ALLOC_EX()

DESCRIPTION
  This function will allocate a PPP event information structure and
  register callback functions for supported PPP protocols.

PARAMETERS
  (IN) device: PPP device
  (IN) info_ptr: Ptr to event information structure

RETURN VALUE
  Event handle

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void * ppp_event_alloc_ex
(
  ppp_dev_enum_type           device,
  ppp_event_alloc_info_type * info_ptr
)
{
  ppp_event_info_s_type *event_info_ptr = NULL;
  ppp_type              *ppp_cb_ptr;
  ppp_type              *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if a valid PPP device was passed
  -------------------------------------------------------------------------*/
  if( device >= PPP_MAX_DEV || 
      info_ptr == NULL ||
      info_ptr->ncp >= PPP_NCP_NAME_MAX )
  {
    DATA_ERR_FATAL("Invalid params passed");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Check that at least one callback is being registered
  -------------------------------------------------------------------------*/
  if( info_ptr->lcp_cb_f == NULL && 
      info_ptr->auth_cb_f == NULL && 
      info_ptr->ncp_cb_f == NULL )
  {
    DATA_ERR_FATAL("NULL functions passed");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Get the PPP control block for this device
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  /*-------------------------------------------------------------------------
    Get a new event information structure
  -------------------------------------------------------------------------*/
  event_info_ptr = ps_mem_get_buf( PS_MEM_PPP_EVENT_INFO_ITEM_TYPE );

  if( NULL == event_info_ptr )
  {
    LOG_MSG_ERROR_0( "Couldn't allocate event info structure" );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Initialize the event information structure
    -----------------------------------------------------------------------*/
    memset( event_info_ptr, 0, sizeof(ppp_event_info_s_type) );
    PS_ENTER_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device) );
    (void)q_link( &ppp_cb_ptr->event_queue, &event_info_ptr->link );

    event_info_ptr->device = device;

    /*-----------------------------------------------------------------------
      Fill the structure with the passed callbacks and user_data cookies
    -----------------------------------------------------------------------*/
    event_info_ptr->cback[LCP]      = info_ptr->lcp_cb_f;
    event_info_ptr->user_data[LCP]  = info_ptr->lcp_user_data;
    event_info_ptr->cback[AUTH]     = info_ptr->auth_cb_f;
    event_info_ptr->user_data[AUTH] = info_ptr->auth_user_data;

    switch( info_ptr->ncp )
    {
      case PPP_NCP_IPCP:
        event_info_ptr->cback[IPCP]     = info_ptr->ncp_cb_f;
        event_info_ptr->user_data[IPCP] = info_ptr->ncp_user_data;
        event_info_ptr->ncp_handle_ptr  = (void *)info_ptr->ncp_handle_ptr;
        break;

#ifdef FEATURE_DATA_PS_IPV6
      case PPP_NCP_IPV6CP:
        event_info_ptr->cback[IPV6CP]     = info_ptr->ncp_cb_f;
        event_info_ptr->user_data[IPV6CP] = info_ptr->ncp_user_data;
        event_info_ptr->ncp_handle_ptr  = (void *)info_ptr->ncp_handle_ptr;
#endif /* FEATURE_DATA_PS_IPV6 */
        break;

      case PPP_NCP_VSNCP_3GPP2:
        event_info_ptr->cback[VSNCP]     = info_ptr->ncp_cb_f;
        event_info_ptr->user_data[VSNCP] = info_ptr->ncp_user_data;
        event_info_ptr->ncp_handle_ptr  = (void *)info_ptr->ncp_handle_ptr;
        break;

      default:
        /* can't happen! */
        LOG_MSG_ERROR_1("Invalid NCP 0x%x!", info_ptr->ncp);
        ASSERT(0);
        break;
    }/* switch( ncp ) */

    /*-----------------------------------------------------------------------
      Place the event information structure on the event queue for device
    -----------------------------------------------------------------------*/
    q_put( &ppp_cb_ptr->event_queue, &event_info_ptr->link );
    PS_LEAVE_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device) );
  }

  return event_info_ptr;
}
#endif /* FEATURE_DATA_PS_EHRPD */


/*===========================================================================
FUNCTION PPP_EVENT_ALLOC()

DESCRIPTION
  This function will allocate a PPP event information structure and
  register callback functions for supported PPP protocols.

PARAMETERS
  device:           PPP instance to register for
  lcp_cb_f:         LCP callback function to call
  lcp_user_data:    user data to be returned with the LCP callback function
  auth_cb_f:        auth callback function to call
  auth_user_data:   user data to be returned with the auth callback function
  ipcp_cb_f:        IPCP callback function to call
  ipcp_user_data:   user data to be returned with the IPCP callback
  ipv6cp_cb_f:      IPV6CP callback function to call
  ipv6cp_user_data: user data to be returned with the IPV6CP callback

RETURN VALUE
  NULL on error
  handle to the PPP event information structure on success

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a PPP event information structure and places it on the event
  queue.  Initializes event masks for all protocols to 0 and registers
  callbacks for the PPP protocols
===========================================================================*/
void *ppp_event_alloc
(
  ppp_dev_enum_type           device,
  ppp_event_cback_f_ptr_type  lcp_cb_f,
  void                       *lcp_user_data,
  ppp_event_cback_f_ptr_type  auth_cb_f,
  void                       *auth_user_data,
  ppp_event_cback_f_ptr_type  ipcp_cb_f,
  void                       *ipcp_user_data,
  ppp_event_cback_f_ptr_type  ipv6cp_cb_f,
  void                       *ipv6cp_user_data
)
{
  ppp_event_info_s_type *event_info_ptr = NULL;
  ppp_type              *ppp_cb_ptr;
  ppp_type              *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if a valid PPP device passed
  -------------------------------------------------------------------------*/
  if( PPP_MAX_DEV <= device )
  {
    LOG_MSG_ERROR_1( "Invalid device passed %d", device );
    ASSERT( 0 );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Check that at least one callback is being registered
  -------------------------------------------------------------------------*/
  if( NULL == lcp_cb_f && NULL == auth_cb_f && NULL == ipcp_cb_f
#ifdef FEATURE_DATA_PS_IPV6
                       &&
      NULL == ipv6cp_cb_f
#endif /* FEATURE_DATA_PS_IPV6 */
    )
  {
    DATA_ERR_FATAL("NULL functions passed");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Get the PPP control block for this device
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  /*-------------------------------------------------------------------------
    Get a new event information structure
  -------------------------------------------------------------------------*/
  event_info_ptr = ps_mem_get_buf( PS_MEM_PPP_EVENT_INFO_ITEM_TYPE );

  if( NULL == event_info_ptr )
  {
    LOG_MSG_ERROR_0( "Couldn't allocate event info structure" );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Initialize the event information structure
    -----------------------------------------------------------------------*/
    memset( event_info_ptr, 0, sizeof(ppp_event_info_s_type) );
    PS_ENTER_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device) );
    (void)q_link( &ppp_cb_ptr->event_queue, &event_info_ptr->link );

    event_info_ptr->device            = device;

    /*-----------------------------------------------------------------------
      Fill the structure with the passed callbacks and user_data cookies
    -----------------------------------------------------------------------*/
    event_info_ptr->cback[LCP]        = lcp_cb_f;
    event_info_ptr->user_data[LCP]    = lcp_user_data;
    event_info_ptr->cback[AUTH]       = auth_cb_f;
    event_info_ptr->user_data[AUTH]   = auth_user_data;
    event_info_ptr->cback[IPCP]       = ipcp_cb_f;
    event_info_ptr->user_data[IPCP]   = ipcp_user_data;
#ifdef FEATURE_DATA_PS_IPV6
    event_info_ptr->cback[IPV6CP]     = ipv6cp_cb_f;
    event_info_ptr->user_data[IPV6CP] = ipv6cp_user_data;
#endif /* FEATURE_DATA_PS_IPV6 */

    /*-----------------------------------------------------------------------
      Place the event information structure on the event queue for device
    -----------------------------------------------------------------------*/
    q_put( &ppp_cb_ptr->event_queue, &event_info_ptr->link );
    PS_LEAVE_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device) );
  }

  return event_info_ptr;
} /* ppp_event_alloc() */



/*===========================================================================
FUNCTION PPP_EVENT_REGISTER()

DESCRIPTION
  This function will add to an event mask for a PPP protocol.  It will
  generate an UP event if the event mask is registering for an UP event
  and the UP event was previously not registerd for this protocol and the
  associated protocol is already up.

PARAMETERS
  handle      PPP event structure to add this event mask to
  protocol    Protocol to add the event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Adds the passed event mask to the already registered event mask
===========================================================================*/
int32 ppp_event_register
(
  void                  *handle,
  ppp_protocol_e_type    protocol,
  ppp_event_mask_e_type  event_mask
)
{
  ppp_event_info_s_type *event_info_ptr = handle;
  uint16 supported_events = ( (uint16)PPP_DOWN_EV_MASK     |
                              (uint16)PPP_UP_EV_MASK       |
                              (uint16)PPP_RESYNC_EV_MASK   |
                              (uint16)PPP_STARTING_EV_MASK |
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
                              (uint16)PPP_SUBS_LIMITED_TO_V4_EV_MASK |
                              (uint16)PPP_SUBS_LIMITED_TO_V6_EV_MASK |
#endif
                              (uint16)PPP_SUCCESS_EV_MASK  |
                              (uint16)PPP_FAILURE_EV_MASK );

  ppp_type              *ppp_cb_ptr;
  ppp_type              *ppp_cb_array = NULL;
  ppp_fail_reason_e_type fail_reason = PPP_FAIL_REASON_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle ||
      FALSE == ps_mem_is_valid(handle, PS_MEM_PPP_EVENT_INFO_ITEM_TYPE) ||
      PPP_PROTOCOL_MAX <= protocol ||
      0 == (supported_events & (uint16)event_mask) )
  {
    LOG_MSG_ERROR_3( "invalid args: handle 0x%x, protocol %d, event_mask 0x%x",
               handle, protocol, event_mask );
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    If registering a mask for a protocol for which no callback is registerd
    return error.
  -------------------------------------------------------------------------*/
  if( NULL == event_info_ptr->cback[protocol] )
  {
    DATA_ERR_FATAL("Registering event for a protocol with NULL callback");
    return -1;
  }

  /*-------------------------------------------------------------------------
    If protocol is AUTH then we support only PPP_STARTING_EV,
    PPP_FAILURE_EV and PPP_SUCCESS_EV
  -------------------------------------------------------------------------*/
  if( PPP_PROTOCOL_AUTH == protocol &&
      0 == ( (uint16)PPP_STARTING_EV_MASK & (uint16)event_mask ) &&
      0 == ( (uint16)PPP_FAILURE_EV_MASK & (uint16)event_mask) &&
      0 == ( (uint16)PPP_SUCCESS_EV_MASK & (uint16)event_mask ) )
  {
    LOG_MSG_ERROR_1( "invalid mask for AUTH: 0x%x", event_mask );
    ASSERT( 0 );
    return -1;
  }

  if(event_info_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid dev 0x%x in event reg", event_info_ptr->device);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[event_info_ptr->device];

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(event_info_ptr->device) );
  /*-------------------------------------------------------------------------
    If the UP event was not previously registered and we are registering it
    now then generate the UP callback if the protocol is UP.
    TODO: For Up event call callback for active NCPs(VSNCPs).
  -------------------------------------------------------------------------*/
  if( 
     !( (uint16)PPP_UP_EV_MASK & (uint16)event_info_ptr->event_mask[protocol] )
     && ( (uint16)PPP_UP_EV_MASK & (uint16)event_mask )  
     && ( ( ( protocol < (ppp_protocol_e_type)fsmi_Size )&& 
            ( fsmOPENED == ppp_cb_ptr->fsm[protocol].state ) ) 
          || ( PPP_PROTOCOL_AUTH == protocol && 
               pppNETWORK == ppp_cb_ptr->phase ) ) )
  {
    event_info_ptr->cback[protocol]( event_info_ptr->device,
                                     protocol,
                                     PPP_UP_EV,
                                     event_info_ptr->user_data[protocol],
                                     ppp_cb_ptr->session_handle,
                                     fail_reason
                                   );
  }

  /*-------------------------------------------------------------------------
    Set the event_mask for the event information structure
  -------------------------------------------------------------------------*/
  event_info_ptr->event_mask[protocol]  =
    (ppp_event_mask_e_type)( (uint16)event_info_ptr->event_mask[protocol] 
                             | (uint16) event_mask ) ;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(event_info_ptr->device) );

  return 0;
} /* ppp_event_register() */



/*===========================================================================
FUNCTION PPP_EVENT_DEREGISTER()

DESCRIPTION
  This function will remove an event mask for a PPP protocol.

PARAMETERS
  handle      PPP event structure to add this event mask to
  protocol    Protocol to add the event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Removes the passed event mask from the already registered event mask
===========================================================================*/
int32 ppp_event_deregister
(
  void                  *handle,
  ppp_protocol_e_type    protocol,
  ppp_event_mask_e_type  event_mask
)
{
  ppp_event_info_s_type *event_info_ptr = handle;
  uint16 supported_events = ( (uint16)PPP_DOWN_EV_MASK     |
                              (uint16)PPP_UP_EV_MASK       |
                              (uint16)PPP_RESYNC_EV_MASK   |
                              (uint16)PPP_STARTING_EV_MASK |
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
                              (uint16)PPP_SUBS_LIMITED_TO_V4_EV_MASK |
                              (uint16)PPP_SUBS_LIMITED_TO_V6_EV_MASK |
#endif
                              (uint16)PPP_SUCCESS_EV_MASK  |
                              (uint16)PPP_FAILURE_EV_MASK );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle ||
      FALSE == ps_mem_is_valid(handle, PS_MEM_PPP_EVENT_INFO_ITEM_TYPE) ||
      PPP_PROTOCOL_MAX <= protocol ||
      0 == ( supported_events & (uint16)event_mask ) )
  {
    LOG_MSG_ERROR_3( "invalid args: handle 0x%x, protocol %d, event_mask 0x%x",
               handle, protocol, event_mask );
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    If protocol is AUTH then we support only PPP_STARTING_EV,
    PPP_FAILURE_EV and PPP_SUCCESS_EV
  -------------------------------------------------------------------------*/
  if( PPP_PROTOCOL_AUTH == protocol &&
      0 == ( (uint16)PPP_STARTING_EV_MASK & (uint16)event_mask ) &&
      0 == ( (uint16)PPP_FAILURE_EV_MASK & (uint16)event_mask ) &&
      0 == ( (uint16)PPP_SUCCESS_EV_MASK & (uint16)event_mask ) )
  {
    LOG_MSG_ERROR_1( "invalid mask for AUTH: 0x%x", event_mask );
    ASSERT( 0 );
    return -1;
  }

  if(event_info_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid dev 0x%x in event dereg", event_info_ptr->device);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Remove the event_mask for the event information structure
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(event_info_ptr->device) );
  event_info_ptr->event_mask[protocol] =
    (ppp_event_mask_e_type)( (uint16)event_info_ptr->event_mask[protocol] 
                             & ~(uint16)event_mask );
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(event_info_ptr->device) );

  return 0;
} /* ppp_event_deregister() */



/*===========================================================================
FUNCTION PPP_EVENT_DEALLOC()

DESCRIPTION
  This function will deallocate a PPP event information structure.

NOTE: This function should not be called in the callback function registered
      for PPP events.

PARAMETERS
  device      PPP device to which this handle belongs
  handle      PPP event structure to deallocate

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Deallocates this PPP event structure.  Sets handle to NULL
===========================================================================*/
int32 ppp_event_dealloc
(
  ppp_dev_enum_type   device,
  void              **handle
)
{
  ppp_event_info_s_type *event_info_ptr;
  ppp_type              *ppp_cb_ptr;
  ppp_type              *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle ||
      FALSE == ps_mem_is_valid(*handle, PS_MEM_PPP_EVENT_INFO_ITEM_TYPE) ||
      PPP_MAX_DEV <= device )
  {
    LOG_MSG_ERROR_2( "invalid args: handle 0x%x, device %d", handle, device );
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get the PPP control block for this device
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  /*-------------------------------------------------------------------------
    Delete the event info structure from the PPP device's event queue
  -------------------------------------------------------------------------*/
  event_info_ptr = *(ppp_event_info_s_type**)handle;
  PS_ENTER_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device)  );
#ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete( &ppp_cb_ptr->event_queue, &event_info_ptr->link );
#else
  q_delete( &event_info_ptr->link );
#endif
  PS_LEAVE_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device) );

  /*-------------------------------------------------------------------------
    Free the event information structure's memory
  -------------------------------------------------------------------------*/
  PS_MEM_FREE( *handle );

  return 0;
} /* ppp_event_dealloc() */

#ifdef FEATURE_DATA_PS_EHRPD
/*===========================================================================
FUNCTION PPP_DEVICE_CONFIGURE()
  
DESCRIPTION
  This function is used to configure the PPP device.
  
PARAMETERS
  (IN) device: PPP device to configure
  (IN) config_ptr: Pointer to device configuration

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_device_configure
(
  ppp_dev_enum_type     device, 
  ppp_dev_config_type * config_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV || 
      config_ptr == NULL    || 
      ( ( config_ptr->bridge_dev < PPP_MAX_DEV ) &&  
        ( config_ptr->bridge_dev == device ) ) )
  {
    DATA_ERR_FATAL("Invalid params to ppp_device_configure");
    return -1;
  }
  
  if( NULL != ppp_device_config[device])
  {
    LOG_MSG_ERROR_0("Device has already been initialized!");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check LCP and HDLC modes for correctness
  -------------------------------------------------------------------------*/
  if( config_ptr->lcp_mode == PPP_CP_MODE_DISABLED ||
      config_ptr->lcp_mode == PPP_CP_MODE_DISCARD  ||
      config_ptr->lcp_mode == PPP_CP_MODE_INVALID  ||
      config_ptr->lcp_mode == PPP_CP_MODE_MAX      ||
      config_ptr->hdlc_mode >= HDLC_INVALID_MODE )
  {
    LOG_MSG_ERROR_0("Invalid protocol mode passed");
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    Set the timeout to min if not set
  -------------------------------------------------------------------------*/
  if( config_ptr->setup_timeout == 0 )
  {
    config_ptr->setup_timeout = PPP_MIN_SETUP_TIME;
  }

  /*-------------------------------------------------------------------------
    Set the setup_final_phase to pppNETWORK if not set.
  -------------------------------------------------------------------------*/
  if( config_ptr->setup_final_phase == 0x0 )
  {
    config_ptr->setup_final_phase = pppNETWORK;
  }

  /*-------------------------------------------------------------------------
    Check AUTH parameters
  -------------------------------------------------------------------------*/
  if( config_ptr->auth_role >= PPP_AUTH_ROLE_MAX )
  {
    LOG_MSG_ERROR_1("Invalid auth mode 0x%x passed", config_ptr->auth_role);
    return -1;  
  }

  if( ( ( config_ptr->auth_role == PPP_AUTH_ROLE_AUTHENTICATEE ) ||
        ( config_ptr->auth_role == PPP_AUTH_ROLE_AUTHENTICATOR ) ) &&
        ( config_ptr->auth_proto_mask == PPP_AUTH_DEFAULT ) )
  {
    LOG_MSG_ERROR_0("No auth protocol specified!");
    return -1;
  }

  if( config_ptr->auth_info.user_id_len        > PPP_MAX_USER_ID_LEN ||
      config_ptr->auth_info.passwd_len         > PPP_MAX_PASSWD_LEN  ||
      config_ptr->auth_info.challenge_len      > PPP_CHAP_CHAL_LEN   ||
      config_ptr->auth_info.challenge_name_len > PPP_CHAP_CHAL_NAME_LEN )
  {
    LOG_MSG_ERROR_0("Invalid Auth configuration param(s)!");
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    Check tx/rx function pointers
  -------------------------------------------------------------------------*/
  if( ( config_ptr->tx_f_ptr == NULL ) || 
      ( config_ptr->rx_sig_f_ptr == NULL ) )
  {
    LOG_MSG_ERROR_0("NULL RX/TX function passed");
    return -1;
  }

  /*-------------------------------------------------------------------------
    TODO: The setting of allow_no_auth should be moved to the client
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_ENFORCE_AUTH
  /*-------------------------------------------------------------------------
    Set the allow_no_auth to FALSE. This is done to mandate auth for eHRPD.
  -------------------------------------------------------------------------*/
  config_ptr->allow_no_auth = FALSE;
#else
  config_ptr->allow_no_auth = TRUE;
#endif /* FEATURE_DATA_PS_ENFORCE_AUTH */

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  if (ds707_is_pkt_classifier_enabled()) 
  {
    config_ptr->allow_no_auth = TRUE;
    LOG_MSG_INFO1_0("Pakcet Classifier: Allow NO_AUTH");
  }
#endif /* FEATURE_DATA_PKT_CLASSIFIER */

  /*-------------------------------------------------------------------------
    All looks good, cache this info
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  ppp_device_config[device] = ps_system_heap_mem_alloc(sizeof(ppp_dev_config_type));

  if(NULL == ppp_device_config[device])
  {
    LOG_MSG_ERROR_0("Memory allocation of ppp_device_config failed");    
    return -1; 
  }

  memscpy( (ppp_device_config[device]), 
           sizeof( ppp_dev_config_type ),
           config_ptr, 
           sizeof( ppp_dev_config_type ));
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  return 0;
} /* ppp_device_configure() */

/*===========================================================================
FUNCTION PPP_DEVICE_UNCONFIGURE()

DESCRIPTION
  This function is used to unconfigure the device.
  
PARAMETERS
  device: the PPP device to be stopped

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS 
  None 
===========================================================================*/
int ppp_device_unconfigure
(
  ppp_dev_enum_type  device
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ppp_device_unconfigure(dev %d)", device);
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  if( FALSE == ppp_is_device_configured(device) )
  {
    LOG_MSG_INFO2_1("ppp_device_unconfigure(%d) on free device!", device);
    return -1;
  }
  else
  {
    /*-------------------------------------------------------------------------
      Clear Device Cache and post Unconfigure command to cleanup PPP.
    -------------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    PS_SYSTEM_HEAP_MEM_FREE(ppp_device_config[device]);
    LOG_MSG_INFO2_2("Freed ppp_device_config %p for device %d",
                     ppp_device_config[device], device); 
    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  } /* if device is not configured */

  return 0;

} /* ppp_device_unconfigure() */

/*===========================================================================
FUNCTION PPP_DEVICE_RESET()
  
DESCRIPTION
  This cleans up the active PPP session and resets the device.
  
PARAMETERS
  (IN) device: PPP device to configure

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_device_reset
(
  ppp_dev_enum_type     device
)
{

  if( device >= PPP_MAX_DEV )
  {
    DATA_ERR_FATAL("Invalid params to ppp_device_reset");
    return -1;
  }

  if(NULL == ppp_device_config[device])
  {
    LOG_MSG_ERROR_0("Device has already been reset!");
    return -1;
  }

  return ( ppp_abort(device ) );
}

/*===========================================================================
FUNCTION PPP_IS_DEVICE_CONFIGURED()
  
DESCRIPTION
  This function is used to check if the PPP device has been configured.
  
PARAMETERS
  (IN) device: PPP device 

RETURN VALUE
  TRUE:  If device has already been configured
  FALSE: If device has not been configured

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ppp_is_device_configured
(
  ppp_dev_enum_type     device
)
{

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid device %d passed to ppp_is_device_configured",
                    device);
    ASSERT(0);
    return FALSE;
  }

  return ((ppp_device_config[device]!= NULL) ? TRUE : FALSE);
}
/*===========================================================================
FUNCTION PPP_DEVICE_STOP()

DESCRIPTION
  This function is responsible for tearing down and cleaning up PPP.
  It does following operation in sequence:
  1. Gracefully teardown NCP
  2. Cleanup NCP control block
  3. Gracefully teardown LCP
  4. Cleanup PPP device
  
PARAMETERS
  device: the PPP device to be stopped

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bqad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  the 'device' field in the configuration structure will be set to the value
  of the 'device' parameter passed in.
===========================================================================*/
int ppp_device_stop
(
  ppp_dev_enum_type  device
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ppp_device_stop(dev %d)", device);
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  if( FALSE == ppp_is_device_configured(device) )
  {
    LOG_MSG_INFO2_1("ppp_device_stop(%d) on free device!", device);
    return -1;
  }
  else
  {
    PPP_SEND_CMD(PPP_DEVICE_STOP_CMD, (void*)device);
  } /* if session_handle != 0 */

  return 0;

} /* ppp_device_stop() */


#endif /* FEATURE_DATA_PS_EHRPD */

/*===========================================================================
FUNCTION PPP_START()

DESCRIPTION
  This function is used by PPP clients to start a particular PPP client with
  a specific configuration.  The caller is ALWAYS required to pass in a
  configuration which should be initialized to some set of values - the
  ppp_get*_opts() function can be used to configure the configuration
  structure.

  Fields that MUST be set in the config structure:
    ipcp_info.rx_iface_ptr || ipv6cp_info.rx_iface_ptr

PARAMETERS
  device:     the PPP device to be started
  config_ptr: pointer to the configuration with which to start PPP device
              'device'

RETURN VALUE
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or some configuration is bad.
  session handle greater then 0 on success.  This can be used to associate
  PPP events with this instance of PPP.

DEPENDENCIES
  None

SIDE EFFECTS
  The configuration that is passed in will be copied to the PPP device
  IMMEDIATELY (if the device is free).  This means that the PPP context is
  modified in the callers task context - it also means that the memory for
  configuration passed in is free as soon as this function returns.
===========================================================================*/
int ppp_start
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
)
{
  static uint16             session_handle = 1;
  ppp_type                 *ppp_cb_ptr;
  ppp_type                 *ppp_cb_array = NULL;
  ppp_config_buffer_s_type *configbuf_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ppp_start(dev %d)", device);
  if(device >= PPP_MAX_DEV || NULL == config_ptr)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];


  /*-------------------------------------------------------------------------
    If a valid configuration is passed for IPCP or IPV6CP then either the PS
    iface pointer currently registered for that NCP should be NULL or the
    same as the passed PS iface pointer
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
#ifdef FEATURE_DATA_PS_IPV6
  if((PPP_CP_MODE_INVALID != config_ptr->ipcp_info.mode &&
      NULL != ppp_cb_ptr->fsm[IPCP].rx_iface_ptr &&
      ppp_cb_ptr->fsm[IPCP].rx_iface_ptr !=
        config_ptr->ipcp_info.rx_iface_ptr) ||
     (PPP_CP_MODE_INVALID != config_ptr->ipv6cp_info.mode &&
      NULL != ppp_cb_ptr->fsm[IPV6CP].rx_iface_ptr &&
      ppp_cb_ptr->fsm[IPV6CP].rx_iface_ptr !=
        config_ptr->ipv6cp_info.rx_iface_ptr))
#else
  if( (PPP_CP_MODE_INVALID != config_ptr->ipcp_info.mode &&
       NULL != ppp_cb_ptr->fsm[IPCP].rx_iface_ptr &&
       ppp_cb_ptr->fsm[IPCP].rx_iface_ptr !=
         config_ptr->ipcp_info.rx_iface_ptr) )
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    LOG_MSG_INFO1_0( "ppp_start() called on protocol already started" );
    PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    return -1;
  }
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device)  );

  /*-------------------------------------------------------------------------
    Sanity check the passed configuration
  -------------------------------------------------------------------------*/
  if( pppi_config_check(config_ptr) < 0 )
  {
    /*TODO: Add config check for PP as well*/
    LOG_MSG_INFO1_0( "Invalid configuration passed" );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get a configuration buffer and send a PPP_START_CMD
  -------------------------------------------------------------------------*/
  configbuf_ptr = (ppp_config_buffer_s_type *)
                    ps_mem_get_buf(PS_MEM_PPP_CMD_BUF_TYPE);

  if( NULL == configbuf_ptr )
  {
    DATA_ERR_FATAL("Memory allocation for PPP config buffer failed");
    return -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Is session_handle about to wrap-around ?
    -----------------------------------------------------------------------*/
    if( 0xFFFF == session_handle )
    {
      LOG_MSG_INFO1_0("PPP Session handle wrapped around");
      session_handle = 1;
    }

    /*-----------------------------------------------------------------------
      Set the session_handle field.
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device)  );
    if( 0 == ppp_cb_ptr->session_handle )
    {
      ppp_cb_ptr->session_handle    = session_handle++;
    }
    configbuf_ptr->session_handle = ppp_cb_ptr->session_handle;

    /*-----------------------------------------------------------------------
      Set the relevant flags in NCP mask
    -----------------------------------------------------------------------*/
    if( ( PPP_CP_ACTIVE  == config_ptr->ipcp_info.mode ) ||
        ( PPP_CP_PASSIVE == config_ptr->ipcp_info.mode ) )
    {
      ppp_cb_ptr->ncp_mask =
        ( ppp_cb_ptr->ncp_mask | ( (uint16)PPP_PROTOCOL_IPCP_MASK ) );
    }
#ifdef FEATURE_DATA_PS_IPV6
    if( PPP_CP_ACTIVE  == config_ptr->ipv6cp_info.mode ||
        PPP_CP_PASSIVE == config_ptr->ipv6cp_info.mode )
    {
      ppp_cb_ptr->ncp_mask =
        ( ppp_cb_ptr->ncp_mask | (uint16)PPP_PROTOCOL_IPV6CP_MASK );
    }
#endif /* FEATURE_DATA_PS_IPV6 */

    ppp_cb_ptr->bridge_dev = config_ptr->bridge_dev;

    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    configbuf_ptr->device        = device;
    configbuf_ptr->dev_opts_null = FALSE;
    memscpy( &configbuf_ptr->dev_opts, 
             sizeof(ppp_dev_opts_type),
             config_ptr, 
             sizeof(ppp_dev_opts_type) );
    PPP_SEND_CMD(PPP_START_CMD, (void*)configbuf_ptr);
  }

  return ppp_cb_ptr->session_handle;
} /* ppp_start() */



/*===========================================================================
FUNCTION PPP_RESYNC()

DESCRIPTION
  This function will restart PPP - forcing it to renegotiate.  It REQUIRES
  that the LCP mode be active.  NOTE: though a configuration can be passed in
  it is NOT possible to change the RX signal to use.

PARAMETERS
  device:     the PPP device to be resync'ed
  config_ptr: pointer to the configuration with which to resync PPP device
              'device'

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or some configuration is bad.

DEPENDENCIES
  None

SIDE EFFECTS
  If configuration is passed in, it will be copied to the PPP device
  IMMEDIATELY (if the device is free).  This means that the PPP context is
  modified in the callers task context - it also means that the memory for
  configuration passed in is free as soon as this function returns.
===========================================================================*/
int ppp_resync
(
  ppp_dev_enum_type    device,
  ppp_protocol_e_type  protocol,
  ppp_dev_opts_type   *config_ptr
)
{
  ppp_type                 *ppp_cb_ptr;
  ppp_config_buffer_s_type *configbuf_ptr;
  ppp_type                 *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( device >= PPP_MAX_DEV ) || 
      ( (ppp_protocol_e_type)fsmi_Size <= protocol ) )
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    Make sure that the device is configured
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_1("ppp_resync on free device(%d)!", device);
    return -1;
  }

  /*-------------------------------------------------------------------------
    If configuration is passed sanity check it.  LCP mode should be ACTIVE
  -------------------------------------------------------------------------*/
  if(NULL != config_ptr &&
     (PPP_CP_MODE_ACTIVE != config_ptr->lcp_info.mode ||
      pppi_config_check(config_ptr) < 0))
  {
    LOG_MSG_INFO1_0("Invalid configuration!");
    return -1;
  } /* if(config_ptr != NULL) */

  /*-------------------------------------------------------------------------
    Return error if trying to resync unopened protocol
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  if(PPP_CP_MODE_ACTIVE  != ppp_cb_ptr->fsm[protocol].mode &&
     PPP_CP_MODE_PASSIVE != ppp_cb_ptr->fsm[protocol].mode)
  {
    LOG_MSG_INFO1_0("Trying to resync unopened protocol");
    PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    return -1;
  }
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  /*-------------------------------------------------------------------------
    Get a config buffer and send the command to the executing task
  -------------------------------------------------------------------------*/
  configbuf_ptr = (ppp_config_buffer_s_type *)
              ps_mem_get_buf(PS_MEM_PPP_CMD_BUF_TYPE);

  if( NULL == configbuf_ptr )
  {
    DATA_ERR_FATAL("Memory allocation for PPP config buffer failed");
    return -1;
  }
  else
  {
    configbuf_ptr->device          = device;
    configbuf_ptr->resync_protocol = protocol;

    if( NULL != config_ptr )
    {
      memscpy( &configbuf_ptr->dev_opts, 
              sizeof(ppp_dev_opts_type),
              config_ptr,
              sizeof(ppp_dev_opts_type) );
      configbuf_ptr->dev_opts_null = FALSE;
    }
    else
    {
      configbuf_ptr->dev_opts_null = TRUE;
    }

    /*-----------------------------------------------------------------------
      we are resyncing so call the callback.
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO2_1 ("Resync PPP protocol: %d", protocol);

    /*-----------------------------------------------------------------------
      Call PPP event callbacks
    -----------------------------------------------------------------------*/
    pppi_call_event_callback( &ppp_cb_ptr->fsm[protocol], 
                              protocol, 
                              PPP_RESYNC_EV );

    PPP_SEND_CMD(PPP_RESYNC_CMD, (void*)configbuf_ptr);
  }

  return 0;
} /* ppp_resync() */



/*===========================================================================
FUNCTION PPP_STOP()

DESCRIPTION
  This function is used by PPP clients to stop a particular PPP device.
  NOTE: if the graceful_stop flag for this PPP device is false, this will
  cause an abort() rather than doing stop() processing.

PARAMETERS
  device: the PPP device to be closeed

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bqad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  the 'device' field in the configuration structure will be set to the value
  of the 'device' parameter passed in.
===========================================================================*/
int ppp_stop
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type protocol
)
{
  ppp_type            *ppp_cb_ptr;
  ppp_type            *ppp_cb_array = NULL;
  int                  ret_val = 0;
  uint32               stop_info = 0x0;  
  ps_iface_type       *iface_ptr;
#ifdef FEATURE_DATA_PS_EHRPD
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type       *ipv6_iface_ptr;
#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* FEATURE_DATA_PS_EHRPD */
  boolean              abort_flag = FALSE;
#ifdef FEATURE_DATA_PS_EHRPD
  uint8          index = 0;
  ncpi_cb_type * ncp_cb_ptr = NULL;  
#endif /* FEATURE_DATA_PS_EHRPD */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_PS_IPV6
  if( ( device >= PPP_MAX_DEV ) ||
      ( ( PPP_PROTOCOL_LCP != protocol    ) &&
        ( PPP_PROTOCOL_IPCP != protocol   ) &&
        ( PPP_PROTOCOL_IPV6CP != protocol ) ) )
#else
  if( ( device >= PPP_MAX_DEV ) ||
      ( ( PPP_PROTOCOL_LCP != protocol  ) &&
        ( PPP_PROTOCOL_IPCP != protocol ) ) )
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    Determine if LCP needs to be brought down.
    The AND operation on the ncp_mask checks if the ncp_mask is reduced to
    zero once the protocol being stopped here is removed from it.  If the
    ncp_mask goes to zero then it means that this is the only NCP protocol
    being run.
  -------------------------------------------------------------------------*/
  if( PPP_PROTOCOL_LCP != protocol )
  {
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    ppp_cb_ptr->ncp_mask &= ~(uint16)( 1 << (uint16)protocol );
    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    if( 0 == ppp_cb_ptr->ncp_mask )
    {
      /*----------------------------------------------------------------------- 
        Propagate the fail reason to the LCP FSM since this is the last NCP.
      -----------------------------------------------------------------------*/
      ppp_cb_ptr->fsm[LCP].fail_reason = ppp_cb_ptr->fsm[protocol].fail_reason;
      protocol = PPP_PROTOCOL_LCP;
    }
  }

  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_1("ppp_stop(%d) on free device!", device);
  }
  else
  {
    /*-----------------------------------------------------------------------
      If we are not supposed to stop gracefully and the physical link is down
      then do abort processing, otherwise send stop_cmd.  Need to check all
      the interfaces.
    -----------------------------------------------------------------------*/
    if( PPP_PROTOCOL_LCP == protocol )
    {
      abort_flag = TRUE;

      /*---------------------------------------------------------------------
        Check the IP interface
      ---------------------------------------------------------------------*/
      iface_ptr = ppp_cb_ptr->fsm[IPCP].rx_iface_ptr;
      if( ( iface_ptr != NULL ) && 
          ( ( ps_iface_phys_link_state( iface_ptr ) == PHYS_LINK_UP ) ||
            ( PS_IFACE_GET_GRACEFUL_CLOSE( iface_ptr ) == TRUE) ) )
      {
        abort_flag = FALSE;
      }

#ifdef FEATURE_DATA_PS_IPV6
      /*---------------------------------------------------------------------
        Check the IPv6 interface
      ---------------------------------------------------------------------*/
      iface_ptr = ppp_cb_ptr->fsm[IPV6CP].rx_iface_ptr;
      if( ( iface_ptr != NULL ) && 
          ( ( ps_iface_phys_link_state( iface_ptr ) == PHYS_LINK_UP ) ||
            ( PS_IFACE_GET_GRACEFUL_CLOSE( iface_ptr ) == TRUE) ) )
      {
        abort_flag = FALSE;
      }
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_EHRPD
      /*---------------------------------------------------------------------
        Check all the VSNP interfaces. This is really needed here only 
        to handle the setup timeout scenario for eHRPD calls (ppp_stop gets 
        called from the setup timer callback for graceful termination.) 
        For HRPD calls, the PS mem indices below should turn out to be not 
        allocated so the for loop below should be a NOOP.
      ---------------------------------------------------------------------*/
      for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
      {
        ncp_cb_ptr =
          (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                               PS_MEM_PPP_NCP_CB_TYPE );
        if( ( NCPI_CB_IS_INITED( ncp_cb_ptr ) == TRUE )  &&
            ( ncp_cb_ptr->device == device ) &&
            ( ncp_cb_ptr->ncp == PPP_NCP_VSNCP_3GPP2 ) )
        {
          /*---------------------------------------------------------------------
            Get ifaces for both ipv4 & ipv6, and verify if either of their
            phy_link_state is UP or Graceful close flag is set. If yes,
            then do graceful termination of PPP by sending LCP term-req.
            Note: rx_iface_ptr corresponds to ipv4 iface, and
            second_rx_iface_ptr corresponds to ipv6 iface.
          ---------------------------------------------------------------------*/
          iface_ptr = ncp_cb_ptr->cb_u.vsncp_3gpp2.fsm.IPV4_IFACE_PTR;
#ifdef FEATURE_DATA_PS_IPV6
          ipv6_iface_ptr = 
               ncp_cb_ptr->cb_u.vsncp_3gpp2.fsm.IPV6_IFACE_PTR;
#endif

          if( ( iface_ptr != NULL ) && 
              ( ( ps_iface_phys_link_state( iface_ptr ) == PHYS_LINK_UP ) ||
                ( PS_IFACE_GET_GRACEFUL_CLOSE( iface_ptr ) == TRUE) ) )
          {
            abort_flag = FALSE;
          }

#ifdef FEATURE_DATA_PS_IPV6
          if( ( ipv6_iface_ptr != NULL ) && 
              ( ( ps_iface_phys_link_state( ipv6_iface_ptr ) == PHYS_LINK_UP ) ||
                ( PS_IFACE_GET_GRACEFUL_CLOSE( ipv6_iface_ptr ) == TRUE) ) )
          {
            abort_flag = FALSE;
          }
#endif
        } 
      } /* for all NCP CBs */
#endif /* FEATURE_DATA_PS_EHRPD */

#if defined( FEATURE_HDR_AN_AUTH )
      /*---------------------------------------------------------------------
        JCDMA requires that LCP Term-Req should be sent for AN device.
      ---------------------------------------------------------------------*/
      if( device == PPP_UM_AN_DEV && 
          ppp_cb_ptr->ppp_failure_graceful_close == TRUE )
      {
        abort_flag = FALSE;
        LOG_MSG_INFO2_2("PPP dev %d, proto 0x%x, setting abort_flag to FALSE",
                        device, protocol);
      }
#endif /* FEATURE_HDR_AN_AUTH */
    } /* if protocol is LCP */


    /*-----------------------------------------------------------------------
      Abort if the abort_flag is set, otherwise send a command to PS to stop
      the relevant protocol
    -----------------------------------------------------------------------*/
    if( TRUE == abort_flag )
    {
      ret_val = ppp_abort( device );
    }
    else
    {
      /*---------------------------------------------------------------------
        Fill the information to be passed to pppi_stop()
        stop_info:
          fill high 16-bits with device      
          fill low 16-bits with protocol
      ---------------------------------------------------------------------*/

      stop_info |= (((uint16)device) << 16);
      stop_info |= ((uint16)protocol); 
      PPP_SEND_CMD(PPP_STOP_CMD, (void*)stop_info);
    }
  } /* if session_handle != 0 */

  return ret_val;
} /* ppp_stop() */



/*===========================================================================
FUNCTION PPP_ABORT()

DESCRIPTION
  This function is used by PPP clients to abort a particular PPP device.

PARAMETERS
  device: the PPP device to be aborted

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bqad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  the 'device' field in the configuration structure will be set to the value
  of the 'device' parameter passed in.
===========================================================================*/
int ppp_abort
(
  ppp_dev_enum_type  device
)
{
  ppp_type             *ppp_cb_ptr;  
  ppp_type             *ppp_cb_array = NULL;
  uint32 abort_info = 0x0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ppp_abort(dev %d)", device);
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  PS_BRANCH_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_1("ppp_abort(%d) on free device!", device);
    PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  }
  else
  {
    PPP_DISABLE_SIG(ppp_cb_ptr->rx_signal);
    /*-----------------------------------------------------------------------
      As the device is being released, reset the session handle to indicate
      this, and set the rx and tx function pointers to the default values.
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->rx_f_ptr           = pppi_default_rx;
    ppp_cb_ptr->tx_f_ptr           = pppi_default_tx;
    ppp_cb_ptr->rx_user_data       = NULL;
    ppp_cb_ptr->tx_user_data       = NULL;
    /*-----------------------------------------------------------------------
      abort_info:
        fill high 16-bits with device      
        fill low 16-bits with session_handle    
    -----------------------------------------------------------------------*/
    abort_info |= (((uint16)device) << 16);
    abort_info |= ((uint16)(ppp_cb_ptr->session_handle));
    ppp_cb_ptr->session_handle     = 0;

    PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    PPP_SEND_CMD(PPP_ABORT_CMD, (void*)abort_info);
  }

  return 0;
} /* ppp_abort() */



/*===========================================================================
FUNCTION PPP_SET_MODE()

DESCRIPTION
  This function takes a new mode and interface as arguments and changes the
  mode of the interface appropriately.
  Will throw an error if:
  (1) Mode switch to PPP_PSEUDO_NET_MODE or PPP_FULL_NET_MODE is requested
      without a bridge device set.
  (2) Mode switch to PPP_RELAY_MODE is requested from a mode other than
      itself or PPP_NONE_MODE.
  (3) Mode switch to PPP_PSEUDO_NET_MODE is requested while running in HDLC
      hardware mode.
  (4) An invalid PPP mode switch is requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_mode
(
  ppp_dev_enum_type  device,             /* device to change              */
  ppp_mode_enum_type new_mode            /* new mode for that device      */
)
{
  ppp_type            *ppp_cb_ptr;
  ppp_type            *ppp_cb_array = NULL;
  uint32 mode_info = 0x0;  
  pppi_mode_info_type  ppp_mode_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_2("ppp_set_mode(%d) on free device(%d)!", new_mode, device);
  }
  else
  {
    LOG_MSG_INFO2_3("dev %d mode from %d to %d",
                    device, ppp_cb_ptr->mode, new_mode);

    switch(new_mode)
    {
      case PPP_PSEUDO_NET_MODE:
      case PPP_FULL_NET_MODE:
      case PPP_RELAY_MODE:
      case PPP_INTERNAL_MODE:
      case PPP_INTERNAL_SNOOP_MODE:
      case PPP_NONE_MODE:
        /*-------------------------------------------------------------------
          Since changing a mode can impact how PPP processes data, it should
          only be changed in the same context in which PPP is running to
          avoid any race conditions. If the caller is calling from the same
          context, set the new mode here else send a cmd to the PPP task.
        -------------------------------------------------------------------*/
        if(PPP_IS_CURR_TASK_OWNER())
        {
          ppp_mode_info.device = (uint16)device;
          ppp_mode_info.mode   = (uint16)new_mode;
          pppi_set_mode(ppp_mode_info);
        }
        else
        {
          /*------------------------------------------------------------------
            mode_info:
              fill high 16-bits with device      
              fill low 16-bits with new_mode    
          ------------------------------------------------------------------*/
          mode_info |= (((uint16)device) << 16);
          mode_info |= ((uint16)new_mode);
          PPP_SEND_CMD(PPP_SET_MODE_CMD, (void*)mode_info);
        }

        break;

      default:
        /*-------------------------------------------------------------------
          Invalid mode!
        -------------------------------------------------------------------*/
        LOG_MSG_ERROR_2("PPP %d, invalid mode %d", device, new_mode);
        ASSERT(0);
    break;
    } /* switch(device mode) */
  }
} /* ppp_set_mode() */

/*===========================================================================
FUNCTION PPP_FAILOVER_ENABLE()

DESCRIPTION
  This macros will set the failover enum in the specified PPP device and
  set LCP to linger.

DEPENDENCIES
  None

RETURN VALUE
  0 on success
 -1 on failure

SIDE EFFECTS
  Sets failover value in a PPP device and sets the special linger bit in
  the NCP mask.
===========================================================================*/
int ppp_failover_enable
(
  ppp_dev_enum_type   device,
  ppp_failover_e_type failover_value
)
{
  ppp_type *ppp_cb_array = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(PPP_MAX_DEV <= (device) || PPP_FAILOVER_IGNORE == (failover_value))
  {
    LOG_MSG_ERROR_2("Invalid args: device %d, failover_value %d",
                    (device), (failover_value) );
    ASSERT( 0 );
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  ppp_cb_array[device].failover = failover_value;
  ppp_cb_array[device].ncp_mask =
    ( ppp_cb_array[device].ncp_mask | (uint16)PPP_PROTOCOL_LCP_LINGER_MASK );
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  LOG_MSG_INFO2_1("enabled PPP failover, NCP mask is now %x",
                  	 ppp_cb_array[device].ncp_mask);

  return 0;

} /* ppp_failover_enable() */

/*===========================================================================
FUNCTION PPP_FAILOVER_DISABLE()

DESCRIPTION
  This macros will set the failover enum in the specified PPP device and
  clear the special LCP linger bit in the NCP mask.  If ncp_mask goes to
  zero then call ppp_stop()

DEPENDENCIES
  None

RETURN VALUE
  0 on sucess
 -1 on failure

SIDE EFFECTS
  Sets failover value in a PPP device and clears the special linger bit in
  the NCP mask.  If the ncp_mask goes to zero then call ppp_stop()
===========================================================================*/
int ppp_failover_disable
(
  ppp_dev_enum_type device
)
{
  ppp_type *ppp_cb_array = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  boolean stop_ppp = FALSE;
  if(PPP_MAX_DEV <= (device))
  {
    LOG_MSG_ERROR_1("Invalid device %d", (device) );
    ASSERT( 0 );
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  ppp_cb_array[(device)].failover = PPP_FAILOVER_IGNORE;
  ppp_cb_array[(device)].ncp_mask &= ~( (uint16)PPP_PROTOCOL_LCP_LINGER_MASK );
  if( 0 == ppp_cb_array[device].ncp_mask &&
      fsmOPENED == ppp_cb_array[(device)].fsm[LCP].state )
  {
    stop_ppp = TRUE;
  }
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  LOG_MSG_INFO2_1("Disabled PPP failover, NCP mask is now %x",
                  	 ppp_cb_array[(device)].ncp_mask);

  if( TRUE == stop_ppp )
  {
    (void)ppp_stop( (device), PPP_PROTOCOL_LCP );
  }

  return 0;
} /*  ppp_failover_disable() */

/*===========================================================================
FUNCTION PPP_GET_PHASE()

DESCRIPTION
  This function returns the current phase of the interface in question.

DEPENDENCIES
  None

RETURN VALUE
  The phase of the requested interface.

SIDE EFFECTS
  None
===========================================================================*/
ppp_link_phase_type ppp_get_phase
(
  ppp_dev_enum_type device              /* device to check              */
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid dev 0x%x in get phase", device);
    ASSERT(0);
    return pppDEAD;
  }
  ppp_cb_array = fsm_get_ppp_cb_array();
  return ppp_cb_array[device].phase;

} /* ppp_get_phase() */


/*===========================================================================
FUNCTION PPP_GET_MODE()

DESCRIPTION
  This function returns the current mode of the interface in question.

DEPENDENCIES
  None

RETURN VALUE
  The mode of the requested interface.

SIDE EFFECTS
  None
===========================================================================*/
ppp_mode_enum_type ppp_get_mode
(
  ppp_dev_enum_type device              /* device to check              */
)
{
  ppp_type      *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid dev 0x%x in get mode", device);
    ASSERT(0);
    return PPP_INVALID_MODE;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  return ppp_cb_array[device].mode;

} /* ppp_get_mode() */



/*===========================================================================
FUNCTION PPP_INJECT_PACKET()

DESCRIPTION
  This function processes a PPP packet as if the packet has been
  received from the underlying device. The processing is performed as
  soon as this function is called before other packets that are already
  queued in the rx queue of the device. Only exception is the packet that is
  currrently being processed in PS context. This function posts a cmd to PS
  so that packet processing always takes place in PS. This function allows
  replaying previously stored PPP packets.

PARAMETERS
  device:        PPP device to configure
  ppp_pkt_ptr:   Pointer to PPP packet to be injected

RETURN VALUE
  0: on success
 -1: if an invalid or unconfigured PPP device specified

DEPENDENCIES
  This function should not be called while already processing a PPP
  packet. That means this function should not be called from pppi_incoming(),
  pppi_receive(), pppi_hdlc_sw/hw_frame/unframe() functions.
  It expects an unframed PPP packet, without FCS and ASYNC FLAG (7E)

SIDE EFFECTS
  Processes the specified PPP packet out of order
===========================================================================*/
int ppp_inject_packet
(
  ppp_dev_enum_type device,               /* PPP device to pass pkt to     */
  uint16            protocol,             /* Protocol of packet            */
  dsm_item_type     *ppp_pkt_ptr          /* Pointer to PPP pkt            */
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  if(ppp_cb_array[device].session_handle == 0)
  {
    LOG_MSG_INFO2_1("ppp_inject_packet(%d) on free device!", device);
    return -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Put device in lower 16 bit and protocol in upper 16 bit of the
      app_field of the pkt ptr. Post cmd to PS to start processing the
      specified pkt and pass the pkt ptr as the cmd param.
    -----------------------------------------------------------------------*/
    ppp_pkt_ptr->app_field = (protocol << 16) | (uint8)device;
    PPP_SEND_CMD(PPP_INJECT_PKT_CMD, (void*)ppp_pkt_ptr);
  }

  return 0;
} /* ppp_inject_packet() */



/*===========================================================================
FUNCTION PPP_INPUT()

DESCRIPTION
  This function processes the packet received from the lower layer,
  according to the mode of the PPP device for which the packet is received.
  Here is a description of the processing for various modes.

  +--------------------------+--------------------------------------------+
  |        PPP Mode          |           Processing routine               |
  +--------------------------+--------------------------------------------+
  | PPP_NONE_MODE            | pppi_none_mode_hdlr(), frees incoming pkt  |
  +--------------------------+--------------------------------------------+
  | PPP_RELAY_MODE           | pppi_relay_mode_hdlr(), INVALID condition  |
  +--------------------------+--------------------------------------------+
  | PPP_PSEUDO_NET_MODE      | pppi_pseudo_net_mode_hdlr(), snoops and    |
  |                          | unframes (via HDLC library) if needed      |
  +--------------------------+--------------------------------------------+
  | PPP_INTERNAL_MODE        |                                            |
  +--------------------------+  pppi_internal_mode_hdlr(), unframes (if   |
  | PPP_FULL_NET_MODE        |  HDLC is enabled) and dispatches packet up |
  +--------------------------+  the stack                                 |
  | PPP_INTERNAL_SNOOP_MODE  |                                            |
  +--------------------------+--------------------------------------------+

   PPP should be the current task when this function is executed.

PARAMETERS
  device    :  PPP device to pass the packet to
  hdlc inst :  instance of HDLC for which the received packet belongs to
  item_ref_ptr : pointer to the incoming PPP packet

RETURN VALUE
   0 : Successfully unframed the packet.
  -1 : Failed to unframe.

DEPENDENCIES
  This function gets executed in PPP task.

SIDE EFFECTS
  None

===========================================================================*/
int ppp_input
(
  ppp_dev_enum_type   device,               /* PPP device to pass pkt to   */
  void              * hdlc_inst,            /* Handle to HDLC instance     */
  dsm_item_type    ** item_ref_ptr          /* Pointer to PPP pkt          */
)
{
  ppp_type             * ppp_cb_ptr;
  ppp_type             * ppp_cb_array = NULL;

  ps_phys_link_type  * phys_link_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  if( item_ref_ptr == NULL || *item_ref_ptr == NULL )
  {
    LOG_MSG_ERROR_1("Bad input parameters to ppp_input(dev %d)", device);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = (ppp_type*) (&(ppp_cb_array[device]));

  /*-----------------------------------------------------------------------
    Log incoming frames if HDLC mode is not HDLC_OFF
  -----------------------------------------------------------------------*/
  if(ppp_cb_ptr->hdlc_mode != HDLC_OFF)
  {
#ifdef FEATURE_DATA_PS_PPP_LOGGING
    PPP_LOG_FRAMES_EXT (*item_ref_ptr, PPP_LOG_FRAMED_RX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

    phys_link_ptr =
      PS_IFACE_GET_PHYS_LINK(ppp_cb_ptr->fsm[IPCP].rx_iface_ptr);

    if(phys_link_ptr != NULL)
    {
    DPL_LOG_PPP_PARTIAL_RX_PACKET(phys_link_ptr, *item_ref_ptr);
    }
  }


  if(!PPP_IS_CURR_TASK_OWNER())
  {
    ASSERT(0);
    LOG_MSG_ERROR_2("ppp_input(%d) must be in owning task, curr_task:0x%x",
                    device, PPP_CURR_TASK());
    dsm_free_packet(item_ref_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    If session_handle is zero, then discard the packet right here
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->session_handle == 0 )
  {
    LOG_MSG_INFO2_1("ppp_input(%d) on free device!", device);
    dsm_free_packet(item_ref_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Invoke the PPP-mode specific handler to process this packet. See function
    description above for a description of the various handler routines.
  -------------------------------------------------------------------------*/
  ppp_cb_ptr->rx_pkt_hdlr_f_ptr( device, hdlc_inst, item_ref_ptr );

  return 0;
} /* ppp_input() */



/*===========================================================================
FUNCTION PPP_OUTPUT()

DESCRIPTION
  PPP link transmit function

PARAMETERS
  device: PPP device.
  item_ref_ptr: reference to a dsm item pointer: this is payload for PPP
  meta_info_ptr: pointer to a meta info item.
  higher_layer_protocol: Higher layer protocol id.

RETURN VALUE
  Same as pppi_outgoing
   0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_output
(
  ppp_dev_enum_type       device,
  dsm_item_type        ** item_ref_ptr,
  uint16                  higher_layer_protocol,
  ps_tx_meta_info_type  * meta_info_ptr
)
{
  uint16                   ppp_protocol;
  ppp_type                *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Determine protocol type
  -------------------------------------------------------------------------*/
  ppp_protocol =
    (higher_layer_protocol == (uint16)PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IPV4) ?
               PPP_IP_PROTOCOL :
#ifdef FEATURE_DATA_PS_IPV6
               PPP_IPV6_PROTOCOL;
#else
               0;
#endif /* FEATURE_DATA_PS_IPV6 */

  ppp_cb_array = fsm_get_ppp_cb_array();
  if(ppp_cb_array[device].mode == PPP_NONE_MODE)
  {
    LOG_MSG_INFO1_1("PPP %d not configured discading pkt", device);
    dsm_free_packet(item_ref_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  return pppi_outgoing(device, ppp_protocol, item_ref_ptr, meta_info_ptr);
} /* ppp_output() */


/*===========================================================================
FUNCTION PPP_UPDATE_AUTH_SETTING()

DESCRIPTION
  Change the authentication encryption requirement at runtime on a
  particular PPP device.

PARAMETERS
  device:  the device for which the setting is changed
  encrypted_auth_reqd:  TRUE  - require encrypted auth (CHAP)
                        FALSE - allow unencrypted auth (PAP or CHAP)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS/LIMITATION
  ** must only be called from PS context! **

===========================================================================*/
void ppp_update_auth_setting
(
  ppp_dev_enum_type  device,                /* PPP device to reconfigure   */
  boolean            encrypted_auth_reqd
)
{
  lcp_value_type    * rwant_ptr;
  ppp_dev_enum_type   dev;
  ppp_type          * ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }
  ppp_cb_array = fsm_get_ppp_cb_array();
  /*-------------------------------------------------------------------------
    For now, ignore user specified device and set all devices to the
    specified setting for consistency with NV item semantics (global).
  -------------------------------------------------------------------------*/
  for(dev = PPP_MIN_DEV; dev < PPP_MAX_DEV; dev++)
  {
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(dev) );

    ppp_cb_array[dev].auth.require_encrypted_passwd = encrypted_auth_reqd;

    /*-----------------------------------------------------------------------
      Modify the 'want' pdv according to this new setting.

      If encrypted password is required, disable all the non-encrypted
      protocols (only PAP for now). Note that this is not synonymous to
      enabling all the encrypted protocols, since it is possible that
      (say, CHAP) was previously disabled due to zero len password etc.
      Also, we want to allow only those protocols that the client originally
      indicated in the 'want'.

      If encrypted password is not required, no action needs to be taken
      since we simply allow what the client indicated originally in the
      'want'.
    -----------------------------------------------------------------------*/
    rwant_ptr = ppp_cb_array[dev].fsm[LCP].remote.want_pdv;
    if( TRUE == encrypted_auth_reqd )
    {
      rwant_ptr->auth_proto_mask =
        (ppp_auth_proto_e_type)( (uint16)rwant_ptr->auth_proto_mask 
                                 & (~( (uint16)PPP_AUTH_PAP ) ) );
    }

    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(dev) );
  }

} /* ppp_update_auth_setting() */

/*===========================================================================
FUNCTION PPP_UPDATE_PASSWORD()

DESCRIPTION
  This function is used to update the PPP password at runtime on a particular
  PPP device.

PARAMETERS
  device       : the device for which the password needs to be updated
  password     : the new password
  password_len : the length of password

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS/LIMITATION
  ** must only be called from PS context! **

===========================================================================*/
int ppp_update_password
(
  ppp_dev_enum_type    device,
  uint8              * password,
  uint8                password_len
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  if( password == NULL || password_len > PPP_MAX_PASSWD_LEN )
  {
    LOG_MSG_ERROR_1("Invalid params passed to ppp_update_password(dev %d)",
                    device);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  memset( ppp_cb_array[device].auth.info.passwd_info,
          0,
          sizeof( ppp_cb_array[device].auth.info.passwd_info ) );
   memscpy( ppp_cb_array[device].auth.info.passwd_info,
           PPP_MAX_PASSWD_LEN,
           password,
           password_len );
  ppp_cb_array[device].auth.info.passwd_len = password_len;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  return 0;

} /* ppp_update_password() */

/*===========================================================================
FUNCTION PPP_UPDATE_USER_ID()

DESCRIPTION
  This function is used to update the PPP user id at runtime on a particular
  PPP device.

PARAMETERS
  device       : the device for which the user id needs to be updated
  user_id      : the new user id
  user_id_len  : the length of the new user id

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS/LIMITATION
  ** must only be called from PS context! **

===========================================================================*/
int ppp_update_user_id
(
  ppp_dev_enum_type    device,
  uint8              * user_id,
  uint8                user_id_len
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  if( user_id == NULL || user_id_len > PPP_MAX_USER_ID_LEN )
  {
    LOG_MSG_ERROR_1("Invalid params passed to ppp_update_user_id(dev %d)",
                    device);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  memset( ppp_cb_array[device].auth.info.user_id_info,
          0,
          sizeof( ppp_cb_array[device].auth.info.user_id_info ) );
  memscpy( ppp_cb_array[device].auth.info.user_id_info,
           PPP_MAX_USER_ID_LEN,
           user_id,
           user_id_len );
  ppp_cb_array[device].auth.info.user_id_len = user_id_len;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  return 0;

} /* ppp_update_user_id() */

/*===========================================================================
FUNCTION PPP_GET_HDLC_HANDLE()

DESCRIPTION
  Returns an opaque pointer to the HDLC handle for the specified PPP device

PARAMETERS
  None

RETURN VALUE
  An opaque pointer to the device HDLC handle.

DEPENDENCIES
  The HDLC handle should have been allocated before calling this function
  None

SIDE EFFECTS/LIMITATION
  None
===========================================================================*/
void *ppp_get_hdlc_handle
(
  ppp_dev_enum_type  device
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return NULL;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ASSERT( ppp_cb_array[device].hdlc_handle != NULL );
  return ppp_cb_array[device].hdlc_handle;
} /* ppp_get_hdlc_handle() */



/*===========================================================================
FUNCTION PPP_SET_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  PPP on the interface specified.

PARAMETERS
  device       : the device for which the user id needs to be updated
  constant     : the PPP constant enum value that needs to be set
  value        : the value that the constant needs to be set to

RETURN VALUE
   0 on success
  -1 on failure

DEPENDENCIES

SIDE EFFECTS/LIMITATION
  None
===========================================================================*/

int ppp_set_device_constant
(
  ppp_dev_enum_type device,
  ppp_init_consts_enum_type constant,
  uint32 value 
)   
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV || constant >= PPP_MAX_INIT_CONSTS )
  {
    LOG_MSG_ERROR_2( "ppp_set_device_constant: Invalid args: device %d,"
                     " constant %d", device, constant );
    ASSERT(0);
    return -1;
  }

  switch(constant)
  {
    case LCP_ACK_TIMEOUT: 
    case LCP_REQUEST_TRY: 
       ppp_lcp_init_device_constant(device, constant, value);
       break;

    case IPCP_ACK_TIMEOUT: 
    case IPCP_REQUEST_TRY: 
      ppp_ipcp_init_device_constant(device, constant, value);
      break;
                          
    case AUTH_TIMEOUT:
    case AUTH_RETRY: 
      ppp_auth_init_device_constant(device, constant, value);
      break;
                          
#ifdef FEATURE_DATA_PS_IPV6
    case IPV6CP_ACK_TIMEOUT: 
    case IPV6CP_REQUEST_TRY: 
      ppp_ipv6cp_init_device_constant(device, constant, value);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */
    case DEFAULT_LCP_ACK_TIMEOUT: 
       ppp_def_dev_const[device].lcp_ack_timeout = (uint16)value;
       break;
    case DEFAULT_LCP_REQUEST_TRY: 
       ppp_def_dev_const[device].lcp_try_configure = (uint8)value;
       break;
    case DEFAULT_IPCP_ACK_TIMEOUT: 
       ppp_def_dev_const[device].ipcp_ack_timeout = (uint16)value;
       break;
    case DEFAULT_IPCP_REQUEST_TRY: 
       ppp_def_dev_const[device].ipcp_try_configure = (uint8)value;
       break;
    case DEFAULT_AUTH_TIMEOUT:
       ppp_def_dev_const[device].auth_timeout = (uint16)value;
       break;
    case DEFAULT_AUTH_RETRY: 
       ppp_def_dev_const[device].auth_retry_limit = (uint8)value;
       break;
#ifdef FEATURE_DATA_PS_IPV6
    case DEFAULT_IPV6CP_ACK_TIMEOUT: 
       ppp_def_dev_const[device].ipv6cp_ack_timeout = (uint16)value;
       break;
    case DEFAULT_IPV6CP_REQUEST_TRY: 
       ppp_def_dev_const[device].ipv6cp_try_configure = (uint8)value;
       break;
#endif /* FEATURE_DATA_PS_IPV6 */
#ifdef FEATURE_DATA_PS_EHRPD
    case VSNCP_3GPP2_REQ_TRY:
    case VSNCP_3GPP2_TERM_TRY:
    case VSNCP_3GPP2_VSNCP_ACK_TIMEOUT:
    case VSNCP_3GPP2_TERM_TIMEOUT:
       /*--------------------------------------------------------------------
         The VSNCP_3GPP2 constants apply across all PPP devices
        -------------------------------------------------------------------*/
       vsncp_3gpp2_set_constant( constant, value );
       break;
#endif /* FEATURE_DATA_PS_EHRPD */
    default:
      LOG_MSG_ERROR_1("ppp_set_device_constant: Unknown constant %d",
                      constant);
      return -1;
  }
  return 0;
} /* ppp_set_device_constant */



/*===========================================================================
FUNCTION PPP_GET_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to obtain a default device constant value 
  from the ppp_dev_const_s_type structure.

PARAMETERS
  device       : the device for which the user id needs to be updated
  constant     : the PPP constant enum value that needs to be returned
  value        : pointer which will store the default value of the 
                 constant

RETURN VALUE
   0 on success
  -1 on failure

DEPENDENCIES

SIDE EFFECTS/LIMITATION
  None
===========================================================================*/

int ppp_get_device_constant
(
  ppp_dev_enum_type device,
  ppp_init_consts_enum_type constant,
  uint32 *value 
)   
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV || constant >= PPP_MAX_INIT_CONSTS ||
      value  == NULL )
  {
    LOG_MSG_ERROR_3( "ppp_get_device_constant: Invalid args: device %d,"
               " constant %d, value %d", device, constant, value );
    ASSERT(0);
    return -1;
  }

  switch(constant)
  {
    case DEFAULT_LCP_ACK_TIMEOUT: 
       *value = ppp_def_dev_const[device].lcp_ack_timeout;
       break;
    case DEFAULT_LCP_REQUEST_TRY: 
       *value = ppp_def_dev_const[device].lcp_try_configure;
       break;
    case DEFAULT_IPCP_ACK_TIMEOUT: 
       *value = ppp_def_dev_const[device].ipcp_ack_timeout;
       break;
    case DEFAULT_IPCP_REQUEST_TRY: 
       *value = ppp_def_dev_const[device].ipcp_try_configure;
       break;
    case DEFAULT_AUTH_TIMEOUT:
       *value = ppp_def_dev_const[device].auth_timeout;
       break;
    case DEFAULT_AUTH_RETRY: 
       *value = ppp_def_dev_const[device].auth_retry_limit;
       break;
                          
#ifdef FEATURE_DATA_PS_IPV6
    case DEFAULT_IPV6CP_ACK_TIMEOUT: 
       *value = ppp_def_dev_const[device].ipv6cp_ack_timeout;
       break;
    case DEFAULT_IPV6CP_REQUEST_TRY: 
       *value = ppp_def_dev_const[device].ipv6cp_try_configure;
       break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      LOG_MSG_ERROR_1("ppp_get_device_constant: Unsupported constant %d",
                      constant);
      return -1;
  }
  return 0;
} /* ppp_get_device_constant */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_CONFIG_CHECK()

DESCRIPTION
  This function is used by PPP clients to check configuration for a
  particular PPP device.

PARAMETERS
  config_ptr: pointer to the configuration options to check

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int pppi_config_check
(
  ppp_dev_opts_type *config_ptr
)
{
  negotiate_t lcp_accept;
  negotiate_t ipcp_accept;
  negotiate_t ipv6cp_accept;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check TX function pointer
  -------------------------------------------------------------------------*/
  if( config_ptr->tx_f_ptr == NULL )
  {
    LOG_MSG_ERROR_0("NULL TX function passed");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check protocol modes and PS iface pointers.  It is an error if:
      *  LCP mode is DISABLED, DISCARD or INVALID
      *  HDLC is in HDLC_INVALID_MODE
      *  No PS iface is being registered
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if( PPP_CP_MODE_DISABLED   == config_ptr->lcp_info.mode          ||
      PPP_CP_MODE_DISCARD    == config_ptr->lcp_info.mode          ||
      PPP_CP_MODE_INVALID    == config_ptr->lcp_info.mode          ||
      PPP_CP_MODE_MAX        <= config_ptr->lcp_info.mode          ||
      HDLC_INVALID_MODE      <= config_ptr->hdlc_mode              ||
      (NULL == config_ptr->ipcp_info.rx_iface_ptr &&
       NULL == config_ptr->ipv6cp_info.rx_iface_ptr) )
#else
  if( PPP_CP_MODE_DISABLED == config_ptr->lcp_info.mode    ||
      PPP_CP_MODE_DISCARD  == config_ptr->lcp_info.mode    ||
      PPP_CP_MODE_INVALID  == config_ptr->lcp_info.mode    ||
      PPP_CP_MODE_MAX      <= config_ptr->lcp_info.mode    ||
      HDLC_INVALID_MODE    <= config_ptr->hdlc_mode        ||
      NULL                 == config_ptr->ipcp_info.rx_iface_ptr )
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    LOG_MSG_ERROR_0("Invalid protocol mode or PS iface passed");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check NCP modes.  Print a message if:
      *  BOTH NCPs are in a combination of INVALID, DISCARD or DISABLED i.e.
         neither NCP is being configured
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if( (PPP_CP_MODE_DISCARD  == config_ptr->ipcp_info.mode ||
       PPP_CP_MODE_DISABLED == config_ptr->ipcp_info.mode ||
       PPP_CP_MODE_INVALID  == config_ptr->ipcp_info.mode)     &&
      (PPP_CP_MODE_DISCARD  == config_ptr->ipv6cp_info.mode ||
       PPP_CP_MODE_DISABLED == config_ptr->ipv6cp_info.mode ||
       PPP_CP_MODE_INVALID  == config_ptr->ipv6cp_info.mode) )
#else
  if( PPP_CP_MODE_DISCARD  == config_ptr->ipcp_info.mode   ||
      PPP_CP_MODE_DISABLED == config_ptr->ipcp_info.mode   ||
      PPP_CP_MODE_INVALID  == config_ptr->ipcp_info.mode )
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    LOG_MSG_INFO2_0("No NCP is in Active or Passive mode");
  }

  /*------------------------------------------------------------------------- 
    Get LCP accept bitmask.
  -------------------------------------------------------------------------*/
  lcp_accept  = lcp_get_accept_mask();

  /*-------------------------------------------------------------------------
    Check LCP parameters
  -------------------------------------------------------------------------*/
  if( (config_ptr->lcp_info.want_mask[FSM_LOCAL]  & ~lcp_accept)  != 0 ||
      (config_ptr->lcp_info.will_mask[FSM_LOCAL]  & ~lcp_accept)  != 0 ||
      (config_ptr->lcp_info.want_mask[FSM_REMOTE] & ~lcp_accept)  != 0 ||
      (config_ptr->lcp_info.will_mask[FSM_REMOTE] & ~lcp_accept)  != 0 )
  {
    LOG_MSG_ERROR_0("Invalid LCP configuration param(s)!");
    return -1;
  }

  if( (config_ptr->lcp_info.want_mask[FSM_LOCAL] & LCP_N_MAGIC) != 0  &&
      (config_ptr->lcp_info.will_mask[FSM_REMOTE] & LCP_N_MAGIC)  == 0 )
  {
    LOG_MSG_ERROR_0("Invalid LCP configuration param(s)!");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check AUTH parameters
  -------------------------------------------------------------------------*/
  if( config_ptr->auth_info.user_id_len        > PPP_MAX_USER_ID_LEN ||
      config_ptr->auth_info.passwd_len         > PPP_MAX_PASSWD_LEN  ||
      config_ptr->auth_info.challenge_len      > PPP_CHAP_CHAL_LEN   ||
      config_ptr->auth_info.challenge_name_len > PPP_CHAP_CHAL_NAME_LEN )
  {
    LOG_MSG_ERROR_0("Invalid Auth configuration param(s)!");
    return -1;
  }

  /*------------------------------------------------------------------------- 
    Get IPCP accept bitmask.
  -------------------------------------------------------------------------*/
  ipcp_accept  = ipcp_get_accept_mask();

  /*-------------------------------------------------------------------------
    Check IPCP parameters
  -------------------------------------------------------------------------*/
  if( config_ptr->ipcp_info.mode != PPP_CP_MODE_INVALID &&
      (config_ptr->ipcp_info.rx_iface_ptr == NULL ||
       (config_ptr->ipcp_info.want_mask[FSM_LOCAL]  & ~ipcp_accept) != 0 ||
       (config_ptr->ipcp_info.will_mask[FSM_LOCAL]  & ~ipcp_accept) != 0 ||
       (config_ptr->ipcp_info.want_mask[FSM_REMOTE] & ~ipcp_accept) != 0 ||
       (config_ptr->ipcp_info.will_mask[FSM_REMOTE] & ~ipcp_accept) != 0) )
  {
    LOG_MSG_ERROR_0("Invalid IPCP configuration param(s)!");
    return -1;
  }

#ifdef FEATURE_DATA_PS_IPV6
  /*------------------------------------------------------------------------- 
    Get IPv6CP accept bitmask.
  -------------------------------------------------------------------------*/
  ipv6cp_accept  = ipv6cp_get_accept_mask();
  /*-------------------------------------------------------------------------
    Check IPv6CP parameters
  -------------------------------------------------------------------------*/
  if( config_ptr->ipv6cp_info.mode != PPP_CP_MODE_INVALID &&
      (config_ptr->ipv6cp_info.rx_iface_ptr == NULL ||
       (config_ptr->ipv6cp_info.want_mask[FSM_LOCAL]  & ~ipv6cp_accept)
                                                                    != 0 ||
       (config_ptr->ipv6cp_info.will_mask[FSM_LOCAL]  & ~ipv6cp_accept)
                                                                    != 0 ||
       (config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] & ~ipv6cp_accept)
                                                                    != 0 ||
       (config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] & ~ipv6cp_accept)
                                                                    != 0) )
  {
    LOG_MSG_ERROR_0("Invalid IPv6CP configuration param(s)!");
    return -1;
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
     Checking EAP parameters when configured in authenticatee mode.
     Therefore, check if FSM_REMOTE configured correctly
  -------------------------------------------------------------------------*/

  if((config_ptr->lcp_info.will_mask[FSM_REMOTE] & LCP_N_AP) &&
     ( (uint32)config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask & \
        (uint32)PPP_AUTH_EAP) )
  {
    
    /*-----------------------------------------------------------------------
     Verify if the AKA_ALGO type is set appropriately.
     If the AKAALGO type variable is milenage, then ensure that the
     aka_algo_milenage_op_data_len variable is checked to be not
     equal to EAP_MILENAGE_OP_LEN [16 bytes]
    -----------------------------------------------------------------------*/
    if( config_ptr->auth_info.aka_algo_type >= EAP_SIM_AKA_ALGO_MAX )
    {
      LOG_MSG_ERROR_1("Invalid EAP configuration parameter aka_algo_type %d",
                      config_ptr->auth_info.aka_algo_type);
      return -1;
    }

    /*-----------------------------------------------------------------------
      If the USIM card is present, No need to validate the EAP parameters 
      which come from the card
    -----------------------------------------------------------------------*/
    if (TRUE == config_ptr->auth_info.ehrpd_usim_present)
    {
      LOG_MSG_INFO2_0("No need to validate the EAP parameters from the card since"
                      " USIM is present");
      return 0;
    }

    if( config_ptr->auth_info.aka_algo_type == EAP_AKA_ALGO_MILENAGE &&
        config_ptr->auth_info.aka_algo_milenage_op_data_len != 
        EAP_AKA_ALGO_MILENAGE_OP_LEN )
    {
      LOG_MSG_ERROR_2( "Invalid EAP configuration parameter aka_algo_type %d"
           " aka_algo_milenage_op_data_len %d", 
           config_ptr->auth_info.aka_algo_type, 
                       config_ptr->auth_info.aka_algo_milenage_op_data_len );
      return -1;
    }

    if(config_ptr->auth_info.aka_algo_seqnum_arr_num_elements != 
       EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS)
    {
      LOG_MSG_ERROR_0("Invalid EAP configuration parameter");
      return -1;
    }
  }
  return 0;
} /* pppi_config_check() */


/*===========================================================================
FUNCTION PPP_GET_MAX_DEVICE()

DESCRIPTION
  This function will return the PPP_MAX_DEV enum.

PARAMETERS
  None

RETURN VALUE
  ppp_dev_enum_type
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_dev_enum_type ppp_get_max_device()
{
  return   PPP_MAX_DEV;

}/*ppp_get_max_device*/


/*===========================================================================
FUNCTION PPP_GET_PEER_VJ_RESULT()

DESCRIPTION
  This function returns TRUE if VJ compression proposed by the peer is 
  accepted during IPCP negotiation. Returns FALSE otherwise. It assumes that
  IPCP negotiation is complete.

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ppp_get_peer_vj_result
(
  ppp_dev_enum_type device
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid device 0x%x passed", device);
    ASSERT(0);
    return FALSE;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  /*-------------------------------------------------------------------------
    The function assumes that IPCP is complete
  -------------------------------------------------------------------------*/
  if( ppp_cb_array[device].fsm[IPCP].state != fsmOPENED )
  {
    LOG_MSG_ERROR_1("IPCP on dev 0x%x not complete yet!", device);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If the peer's VJ was negotiated succesfully, return TRUE
  -------------------------------------------------------------------------*/
  if( ( ppp_cb_array[device].fsm[IPCP].remote.work & IPCP_N_COMPRESS ) &&
      ( ppp_cb_array[device].fsm[IPCP].remote.work & IPCP_N_COMPRESS_VJ ) )
  {
    return TRUE;
  }

  return FALSE;

} /* ppp_get_peer_vj_result() */

/*===========================================================================
FUNCTION PPP_BUILD_HEADER_IP()

DESCRIPTION
  This function is used to add the PPP header for IP protocol.
  It writes the header bytes to the location pointed to by header_ptr and
  returns actual number of bytes written via the bytes_added_ptr OUT
  parameter.

DEPENDENCIES
  None

RETURN VALUE
 -1: error
  0: succes

SIDE EFFECTS
  None
===========================================================================*/
int ppp_build_header_ip
(
  ppp_dev_enum_type   device,
  uint8             * header_ptr,
  uint8             * bytes_added_ptr
)
{
  uint8 * cp = NULL;
  negotiate_t     negotiated;
  boolean apply_pfc;
  boolean apply_acfc; 
  ppp_type * ppp_cb_array = NULL; 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( device >= PPP_MAX_DEV ) ||
      ( header_ptr == NULL ) ||
      ( bytes_added_ptr == NULL ) )
  {
    DATA_ERR_FATAL("Invalid args");
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  /*-------------------------------------------------------------------------
    The function assumes that IPCP is complete
  -------------------------------------------------------------------------*/
  if( ppp_cb_array[device].fsm[IPCP].state != fsmOPENED )
  {
    LOG_MSG_ERROR_1("IPCP on dev 0x%x not complete yet!", device);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Figure out whether to apply PFC and ACFC per negotiated values
  -------------------------------------------------------------------------*/
  negotiated  = ppp_cb_array[device].fsm[LCP].remote.work;
  apply_pfc = negotiated & LCP_N_PFC;
  apply_acfc = negotiated & LCP_N_ACFC;

  /*-------------------------------------------------------------------------
    Build the PPP header in the local buffer
  -------------------------------------------------------------------------*/
  cp = header_ptr;

  /*-------------------------------------------------------------------------
    Add the address and control fields if ACFC was not negotiated
  -------------------------------------------------------------------------*/
  if(!apply_acfc)
  {
    *cp++ = HDLC_ALL_ADDR;
    *cp++ = HDLC_UI;
  }

  /*-------------------------------------------------------------------------
    Add the MSB (0x00) also if PFC was not negotiated
  -------------------------------------------------------------------------*/
  if(!apply_pfc)
  {
    *cp++ = 0;
  }
  *cp++ = (uint8)(PPP_IP_PROTOCOL);
  *bytes_added_ptr = (cp - header_ptr);
  return 0;
} /* ppp_build_header_ip() */

#ifdef FEATURE_DATA_PS_EHRPD
/*===========================================================================
FUNCTION PPP_GET_DEVICE_CONFIG_PTR()

DESCRIPTION
  Access function for PPP device config cache.
 
PARAMETERS
  None
 
RETURN VALUE
  PPP device config cache.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_dev_config_type* ppp_get_device_config_ptr(uint8 device)
{
  if(device >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("Invalid device %d passed", device);
    return NULL;
  }
  
  return (ppp_device_config[device]);
}


/*===========================================================================
FUNCTION PPP_TRIGGER_LSD()

DESCRIPTION
  This function is used to trigger PPP link status determination. If LCP is 
  already opened, it sends an LCP Echo-request packet. If no Echo-Reply is 
  received in response, the packet is resent per the supplied configuration. 
  The result is indicated via the supplied callback.

PARAMETERS
  device:      PPP device to perform link status determination on
  lsd_config_ptr: Pointer to the Link Status Determination configuration

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_trigger_lsd
(
  ppp_dev_enum_type      device,
  ppp_lsd_config_type  * lsd_config_ptr
)
{
  ppp_type                    * ppp_cb_ptr;
  ppp_trigger_lsd_info_s_type * lsd_info_ptr;
  ppp_type                    * ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure args are valid
  -------------------------------------------------------------------------*/
  if( ( device >= PPP_MAX_DEV ) || 
      ( lsd_config_ptr == NULL ) )
  {
    LOG_MSG_ERROR_2( "Invalid args dev %d, config %p passed to trigger lsd",
                     device, lsd_config_ptr );
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    Make sure device is configured
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_1("ppp_trigger_lsd on free device(%d)!", device);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Make sure config is valid
  -------------------------------------------------------------------------*/
  if( ( lsd_config_ptr->echo_req_attempts == 0 ) ||
      ( lsd_config_ptr->echo_req_timeout == 0 ) ||
      ( lsd_config_ptr->result_cback == NULL ) )
  {
    LOG_MSG_INFO1_0("Invalid configuration!");
    return -1;
  } 

  /*-------------------------------------------------------------------------
    Make sure LCP is Open
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  if(fsmOPENED  != ppp_cb_ptr->fsm[LCP].state )
  {
    LOG_MSG_ERROR_0("LCP not open: Cannot trigger LSD");
    PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    return -1;
  }
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  /*-------------------------------------------------------------------------
    Get a config buffer and send the command to the executing task
  -------------------------------------------------------------------------*/
  lsd_info_ptr = (ppp_trigger_lsd_info_s_type *)
                  ps_mem_get_buf(PS_MEM_PPP_CMD_BUF_TYPE);
  if( NULL == lsd_info_ptr )
  {
    DATA_ERR_FATAL("No memory to service PPP LSD command!");
    return -1;
  }
  else
  {
    lsd_info_ptr->device = device;
    memscpy( &lsd_info_ptr->config,
            sizeof( ppp_lsd_config_type ),
            lsd_config_ptr,
            sizeof( ppp_lsd_config_type ) );
    PPP_SEND_CMD(PPP_TRIGGER_LSD_CMD, (void*)lsd_info_ptr);
  }
  return 0;
} /* ppp_trigger_lsd() */
#endif /* FEATURE_DATA_PS_EHRPD */
