/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      P S _ P P P _ N E T M O D E L . C

GENERAL DESCRIPTION

  This contains the code for Data Services network model.

  FEATURES:
    FEATURE_DS_NET_MODEL_DROPS_CALL: if this feature is defined an event that
        would trigger an end-to-end resync will result in the call dropping.
        If the feature is not defined an end-to-end resync will occur.

EXTERNALIZED FUNCTIONS
  These functions are defined in dsnetmdl.h and are externally visible to
  this file.

  netmdl_set_meta_state()
    Change the Meta state of PPP.

  netmdl_get_meta_state()
    Return the Meta state of PPP.

  netmdl_reset_ppp()
    reset the TE-IWF PPP by either terminatingf the link or restarting LCP

Copyright (c) 1998-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_netmodel.c_v   1.17   03 Feb 2003 19:25:44   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_netmodel.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
05/08/09    pp     CMI Phase-4: SU Level API Effort.
09/17/07    scb    Changed MAX_SNOOPED_MESSAGES from 15 to 25
08/16/05    ks     merged kavitha's ppp_callback changes from tip.
04/19/05    kvd    Merged Kavitha's changes from TIP to Dev04.
                   Copy RM NAI into ppp_cb before sending CHAP response.
03/24/05    ssh    Fixed compiler warning in netmdli_ppp_up_callback()
03/22/05    ssh    Now using new function pppi_report_event_to_diag(),
                   added posting of resync event to diag.
03/21/05    ifk    Set PPP phase on receiving LCP and IPCP config requests 
                   to help in performing SW workaround for HDLC issue.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
03/15/05    ifk    Fixed bugs found in security review.
01/10/05    ifk    Added support for CCP renegotiation on resyncs.
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
11/23/04     jk    Added runtime check for Internal Auth Reg functions
10/14/04    ifk    Added meta info ** to snoop callback function
09/30/04    ifk    Added diag event EVENT_PPP_NETMODEL
09/10/04    ifk    In ppp_down_callback() instead of sending the term req to
                   the bridged PPP explicitly tear it down.
07/05/04    ifk    Removed setting of dsm kind field in call to pppi_outgoing
06/21/04    vr     Set PPP mode to INTERNAL_SNOOP instead of FULL_NET id QOS
                   or BCMCS is supported.
06/16/04    jd     Fix slot-id check bug and check # rev link slots in 
                   compression check in compare_ipcp_opts
06/16/04    jd     Drop call if end-to-end to resync fails due to lack of 
                   memory for spoofed C-REQ
06/11/04    vp     Changes for representation of IP addresses as struct ps_in_addr.
06/03/04    jd     CCP will be rejected on both Rm and Um interfaces if 
                   FEATURE_DATA_PS_CCP_REJECT is turned on
05/25/04    vr     Fixed featurization bug with FEATURE_BCMCS
05/18/04    jk     Added 3gpd feature support - use internal chap auth when
                   authenticating via RUIM
04/30/04    vr     Set Um and Rm to FULL_NETWORK model instead of PSEUDO_NET
                   for BCMCS/QoS support on HDR
05/05/04    usb    Calling ipcp_setup_device to configure device for IP
                   to support INTERNAL mode fr laptop calls.
04/16/04    jd     return completion status from netmdli_ppp_resynced 
11/17/03    igt    Added a "start" for the PERF_TOTAL_HANDOFF delay
10/31/03    usb    Notify 1X RMSM when end to end PPP comes up during a 1X 
                   network model call.
09/16/03    usb    Removed unwanted fsm calls while RESYNCING meta state 
                   transition.
08/19/03    jd     Ensure IPCP complete callback is reset on transition to
                   OOC metastate
08/15/03    aku    Use ps_iface macro to access graceful_dormant_close 
                   variable
08/13/03    ifk    PPP cleanup in preparation for adding IP6CP.
05/13/03    usb    Always enable flow when going to OOC meta state, check
                   for NULL bridge iface in netmdli_ppp_down_callback().
05/05/03    jd     Removed dependency on dsiface.h
04/17/03    mvl    Fixed feature check for DMU and JCDMA as there was a typo
04/11/03    jd     Added wrappers to make sure netmdl unreg's only the snoop
                   callbacks it itself has registered
04/11/03    ak     Updated for new 707 RMSM interfaces.
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    removed authentication featurization
03/19/03    jd     Include pstimer.h for dmu builds to fix compiler warning
03/19/03    mvl    Fixed bug where which options were negotiated was not
                   being stored correctly.
02/02/03    usb    Updated snooop callback functions with new return values
12/30/02   mvl/jd  Added code to do handle CHAP within the handset during 
                   netmodel calls for DMU (under FEATURE_DS_MOBILE_IP_DMU).  
                   Will also swallow CHAP response from laptop.
12/04/02    usb    Wrapped ds707_rmsm.h in FEATURE_DATA_IS707
11/27/02   na/ak   Fixed compile time problem in netmdli_ppp_resynced.
11/14/02   jd/usb  Added netmdl_set_resync_behaviour() to select between 
                   "drop call" (for MIP) and "end-to-end" resync (SimpleIP)
                   when Um resync results in Um-Rm PPP option mismatch.
                   If netmodel drops call on resync, now does it nicely
                   through RMSM event rather than ppp_stop.
11/04/02    mvl    renamed CHAP_RESYNC feature to JCDMA_1X and moved #defines
                   that were only used in this file here from the header.
10/25/02    usb    Modified the condition to go to INIT state while resyncing 
                   for UMTS PPP calls.
09/12/02    mvl    Generified the network model resync 
                   callcack and some general cleanup.  Removed netmodel 
                   featurization.
09/03/02    usb    Fixed netmdli_ppp_down_callback to get iface_ptr from the
                   PPP control block
08/13/02    mvl    Removed the iface_cb_array and have a curr_id array
                   instead (all the other items are in the PPP cb) moved the
                   set/get_mode() functions to pppi.c.
08/07/02    mvl    Fixed the DROPS_CALL feature so now we close gracefully.
08/06/02    mvl    Reworked authentication.
07/25/02    mvl    Updates for PPP renaming.
07/16/02    mvl    Fixed bug in ACCM comparison.
07/15/02    usb    Moved ipcp_c_ack_status initialization to OOC meta state 
                   from INIT meta state.
07/11/02    mvl    Updates for new PPP architecture.
06/16/02    usb    Changed the function netmdli_ppp_down_callback based upon
                   new architecture
05/22/02    mvl    changed all of the interface names to MM PS values. 
05/20/02     vr    Changed hdlc_enabled flag to hdlc_mode
05/08/02    mvl    File cleanup and added arg to ppp_outgoing() for new proto
03/16/02    vas    Fixed typo in netmdli_init_callback_chap_resp()
03/01/02    vas    Updated to allow authentication (PAP/CHAP) parameters to
                   be allocated per iface.
02/20/02    sjy    Mobile IP performance now featurized under 
                   FEATURE_DS_MOBILE_IP_PERF
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/07/02    vsk    Updated error message when dropping pkt in 
                   netmdli_init_callback_lcp_c_ack
12/21/01    dwp    Let netmdli_init_callback_lcp_c_ack() work in FULL_NET_MODE
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Use FEATURE_DATA for old/new DS Mgr dependencies. Keep the
                   R interface in FULL NET mode when the U has HDLC disabled.
09/25/01    mvl    Fixed variable decalrations in netmdli_ppp_resynced() so
                   that only appropriate variables are defined for drop call
                   feature.
09/20/01    mvl    Setting the will field in the save_opts() function because
                   it is not set otherwise (due to PPP restructure).
09/13/01    mvl    Removed optimization from the save_opts() function as it
                   was incompatible with DNS mods.
08/22/01    sjy    Removed duplicate code in netmdli_compare_lcp_opts(); 
                   it now calls netmdl_compare_lcp_opts() to carry out the 
                   LCP option comparisons
08/03/01    mvl    Removed Mobile IP specific code because network model is
                   not used to bring up Mobile IP calls.
07/31/01    mvl    Made modification to the IPCP comparison logic to only
                   compare IP addresses if both the Um and Rm wanted to
                   negotiate for IP addresses.
07/30/01    sjy    Added functions netmdl_compare_lcp_opts() and 
                   netmdl_compare_ipcp_opts() to compare IPCP and LCP options 
                   on UM and RM interfaces.
07/30/01    mvl    Stripped out M.IP code, as network model is not used
                   during call setup.  Made a fix in save_options to save the
                   default options if they were negotiated.
07/27/01    aku    Removed calls to dsiface_mip_reset. No longer required
                   due to M. IP redesign
07/27/01    mvl    Fixed compile time warning.
07/23/01    pjb	   Removed the DS-PS command queue.  Move PS kinds on to PS.
                   Changed calls to dsm_offset_new_buffer to allocate space
		   for headers. 
07/22/01    mvl    removed restriction against putting Rm in internal mode.
07/10/01    na     Added extra debug messages.
07/07/01    mvl    Added M.IP support for the case when the TE is ignoring
                   hardware flow control - packets will be discarded.
06/27/01    mvl    modified debug message to give more info.
06/26/01    mvl    now resetting Meta State to init when resetting PPP.
06/21/01    mvl    Added PPP meta state variable and netmdl_get_meta_state()
                   to check state and netmdl_reset_ppp() function
06/20/01    mvl    Fixed several bugs to support M.IP->Simple IP fallback.
06/14/01    mvl    Fixed some debug messages to be more meaningful.
06/04/01  jd/mvl   Resolved Ip_addr byte-ordering issues
05/02/01    na     When PPP negotiation with laptop was complete, meta state
                   was not being set to UP. Now fixed.
05/18/01    mvl    Added Mobile IP support code.
08/25/00    rc     Changed dsm_new_buffer() calls to take pool id as input 
                   parameter. 
06/30/00    ttl    Adds FEATURE_DS_PPP_TERM_REQ_DROP which will avoid a call
                   origination when the call is in dormant idle state in
                   network model mode.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
04/24/00    snn    Renamed ds_prev_nw_ppp_call_had_pap to 
                   ds_prev_nw_ppp_call_had_auth because this flag is now used
                   for both PAP and CHAP.
                   Added a new pointer, ppp_chap_resync_passwd. This will
                   store the default password to be used CHAP resync occurs
                   in N/W model.
                   Added a new function netmdli_init_callback_chap_resp for
                   doing the chap call back.
04/12/00    mvl    Added new compile time feature that will cause a call to
                   drop rather than ce an end-to-end resync.
02/17/00    mvl    Fixed some typos that were causing compilation failures if
                   LOG_MSG_INFO3()s were compiled in.
10/27/99    mvl    Code review clean up and much more efficient
                   implementation of the IPCP C-Ack code.
09/27/99    snn    Rewrote the pap call back function to be more code 
                   efficient.
09/14/99    mvl    Changed a message that was misleading.
09/01/99    mvl    Removed support for full network mode - this included
                   changing the way that options are compared, and removing
                   support for only resyncing IPCP.
08/20/99    mvl    Changed the way that options are saved.  Only saving Um
                   want and Rm work and want.  Um work is not saved.
08/19/99    mvl    Moved the LCP and IPCP option saving routines to ppplcp.c
                   and pppipcp.c respectively.
08/12/99    mvl    Fixed the IPCP option checking after a resync - not all
                   the compression possibilities were being examined.
08/10/99    snn    modified comments to the PAP call back function.
07/28/99    snn    Added support for the PAP.
06/07/99    snn    Disable Rm flow control in the function 
                   netmdli_ppp_resynced() after the IP address mismatch
06/07/99    snn    In netmdli_ppp_resynced() function deregistered function 
                   initially, so that it takes effect even if the IP
                   addresses do not match. 
10/04/98  mvl/na   Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "ps_ppp_snoop.h"
#include "dsm.h"
#include "ps_ppp_netmodel.h"
#include "err.h"
#include "memory.h"
#include "msg.h"
#include "ps_ppp.h"
#include "ps_ppp_defs.h"
#include "ps_pppi.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_auth.h"
#include "ps_iface.h"
#include "ds_Utils_DebugMsg.h"


#include "ps_ppp_events.h"

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Array to keep track of callbacks registered with snoop.

  MAX_SNOOPED_MESSAGES must be increased if more callbacks are added.
---------------------------------------------------------------------------*/
#define MAX_SNOOPED_MESSAGES (25)
static uint32 snoop_id_array[MAX_SNOOPED_MESSAGES];

/*---------------------------------------------------------------------------
  Number of snooped message id's in the netmdl snoop list.  The whole list
  must be deregistered at once, as we can't handle insertion/shift-deletion
---------------------------------------------------------------------------*/
static byte snoop_id_array_entries = 0;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                          LOCAL FUNCTION DEFINTIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION NETMDL_SNOOP_REG()

DESCRIPTION
  Wrapper to register snoop callbacks and remember what was registered so
  we can unregister only those that were registered by netmdl later

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdl_snoop_reg
(
  ppp_dev_enum_type      iface,       /* Interface to snoop on             */
  uint16                 protocol,    /* Procotol to snoop                 */
  byte                   msg_type,    /* Protocol spec. msg to snoop       */
  snoop_cb_f_ptr_type    callback     /* Function to call when msg arrives */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( snoop_id_array_entries < MAX_SNOOPED_MESSAGES );

  /*-------------------------------------------------------------------------
    Snoop the message and add to netmdl's list of snoop'ed messages
  -------------------------------------------------------------------------*/
  snoop_id_array[snoop_id_array_entries++] = 
    snoop_reg_event( iface, protocol, msg_type, callback );

}  /* netmdl_snoop_reg() */



/*===========================================================================
FUNCTION NETMDL_SNOOP_UNREG_ALL()

DESCRIPTION
  Unregister all callbacks previously snooped by netdml.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdl_snoop_unreg_all
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Deregister all previously snooped messages
  -------------------------------------------------------------------------*/
  while( snoop_id_array_entries ) 
  {
    snoop_unreg_event( snoop_id_array[--snoop_id_array_entries] );
  }

  ASSERT( snoop_id_array_entries == 0 );

}  /* netmdl_snoop_unreg_all() */

