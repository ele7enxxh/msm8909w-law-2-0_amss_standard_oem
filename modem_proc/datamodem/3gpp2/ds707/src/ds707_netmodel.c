/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      D S _  7 0 7 _ N E T M O D E L . C

GENERAL DESCRIPTION

  This contains the code for Data Services network model.

  FEATURES:
    FEATURE_DS_NET_MODEL_DROPS_CALL: if this feature is defined an event that
        would trigger an end-to-end resync will result in the call dropping.
        If the feature is not defined an end-to-end resync will occur.

EXTERNALIZED FUNCTIONS
  These functions are defined in dsnetmdl.h and are externally visible to
  this file.

  ds707_netmdl_reset_ppp()
    reset the TE-IWF PPP by either terminatingf the link or restarting LCP

Copyright (c) 1998-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_netmodel.c_v   1.17   03 Feb 2003 19:25:44   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_netmodel.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
03/25/14    ssb    Reset the netmodel state to INIT before returning
                   SNOOP_CB_SNOOP_AGAIN to PPP
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
07/26/12    msh    Replaced FEATURE_DATA_PS_INTERNAL_AUTH by NV item 
07/11/12    sb     Fixed KW Errors
04/19/12    sb     Feature Cleanup: Always On Flag FEATURE_UIM_SUPPORT_3GPD
04/18/12    jz     Seperate old JCDMA and Simplified JCDMA 
02/24/12    vpk    PAP re-transmission is not supported for network model call
01/02/12    msh    Coan: Feature cleanup
07/04/11    dvk    Added support to read auth credentials from NV for CT RUIM 
                   card
05/20/11    op     Fixed compilation error
03/23/11    op     Data SU API cleanup
03/15/11    mg     Global variable cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
02/11/11  jee/ssh  Cache and restore Um remote's use_same_auth value for resyncs
02/02/11    ms     Send Term-Ack if a Term-Req is recieved from PC while cleaning PPP
                   on Rm.
12/07/10    sn     Fix to read RM NAI (multiple NAI) only if phone is in NV 
                   mode.
10/14/10    ss     Adding debug messages before ASSERT.
10/13/10    op     Migrated to MSG 2.0 macros
06/28/10    ms     Pickup PAP credentials from DUN when AT is in NV mode.
03/30/10    ssh    FEATURE_DATA_PS_PAP_INTERNAL_AUTH: Gobi mainlining
02/05/10    ls     Revert the changes for picking up credntials from DUN on 
                   OMH phone
11/30/09    ps     Fixed the issue of not picking up credentials from DUN when
                   Legacy RUIM card is inserted in an OMH phone.
11/10/09    as     Cleared Windows Compiler warnings for MOB builds
09/18/09    ss     DataCommon CMI SU level impacts.
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
04/08/09    ps     Added support to send LCP TReq when re-negotiated PPP 
                   options mismatch. Applicable for KDDI only.
04/06/09    ss     Fixed Off Target Lint Errors.
03/06/09    psng   Fixed CHAP failure issue when MIP DMU feature is off and AT
                   is in NV mode.
04/27/08    pp     Replaced ps_meta_info_type with ps_rx_meta_info_type for
                   FEATURE_DATA_PS_META_INFO_V2.
08/21/08    cs     KlocWork  fixes 
07/01/08    mv     Merged KlocWork fixes from 1H07 to 1H08
11/20/07    ms     Fixed compiler warnings
10/20/07    ms     Fixed stack overflow problem in netmdli_save_opts with 
                   appropriate bounds check.
07/26/07    sc     Fixed compiler warnings
02/02/07    ssh    Disable DNS negotiation for resyncs (JCDMA)
01/10/07    as     Clearing the Have-we-seen-an-Ack status upon receiving 
                   LCP and IPCP C-Req.
10/17/06    ssh    Forward the LCP C-Ack on bridge dev *before* calling 
                   lcp_setup_device()
10/10/06    ssh    Calling lcp_setup_device when LCP negotiation completes
09/05/06    ks     Moved definition of INTERNAL_AUTH feature outside, as it 
                   is needed outside netmodel.c too.
05/25/06  ssh/squ  Changes to enforce CHAP in SIP pseudonet model calls, 
                   under FEATURE_DATA_PS_INTERNAL_AUTH
01/17/06   gr/ssh  Changes to match ipcp_value_type in ps_ppp_defs.h
12/23/05    ks     Added parameter to ipcp_save_work_opt().
10/20/05   as/vrk  Fixed errors reported by Coverity
04/19/05    kvd    Merged Kavitha's changes from TIP to Dev04.
                   Copy RM NAI into ppp_cb before sending CHAP response.
03/24/05    ssh    Fixed compiler warning in ds707_netmdli_ppp_up_callback()
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
06/11/04    vp     Changes for representation of IP addresses as struct in_addr.
06/03/04    jd     CCP will be rejected on both Rm and Um interfaces if 
                   FEATURE_DATA_PS_CCP_REJECT is turned on
05/25/04    vr     Fixed featurization bug with FEATURE_BCMCS
05/18/04    jk     Added 3gpd feature support - use internal chap auth when
                   authenticating via RUIM
04/30/04    vr     Set Um and Rm to FULL_NETWORK model instead of PSEUDO_NET
                   for BCMCS/QoS support on HDR
05/05/04    usb    Calling ipcp_setup_device to configure device for IP
                   to support INTERNAL mode fr laptop calls.
04/16/04    jd     return completion status from ds707_netmdli_ppp_resynced 
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
                   for NULL bridge iface in ds707_netmdli_ppp_down_callback().
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
11/27/02   na/ak   Fixed compile time problem in ds707_netmdli_ppp_resynced.
11/14/02   jd/usb  Added ds707_netmdl_set_resync_behaviour() to select between 
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
09/03/02    usb    Fixed ds707_netmdli_ppp_down_callback to get iface_ptr from the
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
06/16/02    usb    Changed the function ds707_netmdli_ppp_down_callback based upon
                   new architecture
05/22/02    mvl    changed all of the interface names to MM PS values. 
05/20/02     vr    Changed hdlc_enabled flag to hdlc_mode
05/08/02    mvl    File cleanup and added arg to ppp_outgoing() for new proto
03/16/02    vas    Fixed typo in ds707_netmdli_init_callback_chap_resp()
03/01/02    vas    Updated to allow authentication (PAP/CHAP) parameters to
                   be allocated per iface.
02/20/02    sjy    Mobile IP performance now featurized under 
                   FEATURE_DS_MOBILE_IP_PERF
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/07/02    vsk    Updated error message when dropping pkt in 
                   ds707_netmdli_init_callback_lcp_c_ack
12/21/01    dwp    Let ds707_netmdli_init_callback_lcp_c_ack() work in FULL_NET_MODE
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Use FEATURE_DATA for old/new DS Mgr dependencies. Keep the
                   R interface in FULL NET mode when the U has HDLC disabled.
09/25/01    mvl    Fixed variable decalrations in ds707_netmdli_ppp_resynced() so
                   that only appropriate variables are defined for drop call
                   feature.
09/20/01    mvl    Setting the will field in the save_opts() function because
                   it is not set otherwise (due to PPP restructure).
09/13/01    mvl    Removed optimization from the save_opts() function as it
                   was incompatible with DNS mods.
08/22/01    sjy    Removed duplicate code in ds707_netmdli_compare_lcp_opts(); 
                   it now calls ds707_netmdl_compare_lcp_opts() to carry out the 
                   LCP option comparisons
08/03/01    mvl    Removed Mobile IP specific code because network model is
                   not used to bring up Mobile IP calls.
07/31/01    mvl    Made modification to the IPCP comparison logic to only
                   compare IP addresses if both the Um and Rm wanted to
                   negotiate for IP addresses.
07/30/01    sjy    Added functions ds707_netmdl_compare_lcp_opts() and 
                   ds707_netmdl_compare_ipcp_opts() to compare IPCP and LCP options 
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
06/21/01    mvl    Added PPP meta state variable and ds707_netmdl_get_meta_state()
                   to check state and ds707_netmdl_reset_ppp() function
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
                   Added a new function ds707_netmdli_init_callback_chap_resp for
                   doing the chap call back.
04/12/00    mvl    Added new compile time feature that will cause a call to
                   drop rather than ce an end-to-end resync.
02/17/00    mvl    Fixed some typos that were causing compilation failures if
                   MSG_LOW()s were compiled in.
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
                   ds707_netmdli_ppp_resynced() after the IP address mismatch
06/07/99    snn    In ds707_netmdli_ppp_resynced() function deregistered function 
                   initially, so that it takes effect even if the IP
                   addresses do not match. 
10/04/98  mvl/na   Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "ds3gsiolib.h"
#include "ps_ppp_snoop.h"
#include "dsm.h"
#include "ps_ppp_netmodel.h"
#include "ds707_netmodel.h"
#include "err.h"
#include "memory.h"
#include "msg.h"
#include "data_msg.h"
#include "ps_ppp.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_auth.h"
#include "ps_iface.h"
#include "ps_utils.h"
#include "dsgen_iface_hdlr.h"
#include "ds707_rmsm.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#include "pstimer.h"
#include "ps_ppp_events.h"
#include "ds707_roaming.h"
#include "ds_flow_control.h"
#include "ps_phys_link.h"

#include "nvruimi.h"

/*lint -save -e641, -e655 */
#if !defined(FEATURE_DSM_DUP_ITEMS)
#error internal auth requires DSM_DUP_ITEMS
#endif /* !FEATURE_DSM_DUP_ITEMS */

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  The interface control block array.
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  the current PPP id to use.
---------------------------------------------------------------------------*/
static byte ds707_curr_ppp_id[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
  Runtime configurable resync failure behaviour
---------------------------------------------------------------------------*/
#ifdef FEATURE_DS_NET_MODEL_DROPS_CALL
static boolean ds707_netmdl_drop_call = TRUE;
#else
static boolean ds707_netmdl_drop_call = FALSE;
#endif /* FEATURE_DS_NET_MODEL_DROPS_CALL */

#if defined(FEATURE_DS_MULTIPLE_NAI) 
STATIC void ds707_netmdli_copy_rm_nai
(
  ppp_dev_enum_type device
);
#endif /* FEATURE_DS_MULTIPLE_NAI  */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Variables that maintain snoop/callback state information, such as the
  current LCP and IPCP id numbers.
---------------------------------------------------------------------------*/
static ack_status_enum_type ds707_lcp_c_ack_status  = NO_ACK_RCVD;
static ack_status_enum_type ds707_ipcp_c_ack_status = NO_ACK_RCVD;
/* If Outstanding Ack id is -1, then we dont have to
   send any Term-ack on Rm link to PC 
*/
static int16 outstanding_lcp_t_ack_id = -1;
/*---------------------------------------------------------------------------
  Variable maintaining whether CCP/ILCCP needs to be renegotiated on resync.
  This variable has value 0 if no CCP has been negotiated and a value
  of PPP_CCP_PROTOCOL(0x80FD) or PPP_ILCCP_PROTOCOL(0x80FB) depending on
  which of the two was used.

  The variable currently maintains the config ack sent by the laptop to a 
  request sent by PDSN.  This is because a config ack by laptop means
  the laptop acknowledges the PDSN's request to send compressed packets to
  the PDSN.  On a resync the laptop should be forced to resync to find out
  if the PDSN supports decompression.  The laptop receiving compressed
  packets doesn't matter in this scenario because the new PDSN hasn't
  negotiated CCP and would not send any compressed traffic.
---------------------------------------------------------------------------*/
static uint16 ds707_ccp_update_proto = 0;

/*---------------------------------------------------------------------------
  ppp_chap_resync_passwd is a constant pointer and contains the password to 
  be used during the Resync in the network model call.
  Note that the password used during the Network model resync is different
  from the password used before.
---------------------------------------------------------------------------*/
#define PPP_CHAP_RESYNC_PASSWD  "Packet-One000001"
#define CHAP_RESYNC_PASSWD_LENGTH (sizeof(PPP_CHAP_RESYNC_PASSWD) -1 )

static const char *ppp_chap_resync_passwd = PPP_CHAP_RESYNC_PASSWD;


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                          LOCAL FUNCTION DEFINTIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION DS707_NETMDLI_SAVE_OPTS()

DESCRIPTION
  This function will go through all of the options in a C-Ack and store them
  in the appropriate control block (i.e. LCP vs. IPCP).  It only saves the
  want values for the Um interface, and stores the work and want values for
  the Rm interface.  This is done because the Um work is irrelevant until a
  renegotiation happens, and at that point these options are reset.

DEPENDENCIES
  Expects the data_ptr in the item to point to the code portion of the PPP
  packet.

  Assumes that it will ONLY be call for LCP or IPCP

  lcp_init() and ipcp_init() should have been called before this function.

RETURN VALUE
  RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
  RET_OK    (0)  on sucessful completion of recording all options in packet.

SIDE EFFECTS
  The local portion of the control block for the protocol in question is
  updated to contain the information in the C-Ack.
===========================================================================*/
static sint15 ds707_netmdli_save_opts
(
  dsm_item_type    ** item_head_ptr, /* Ptr to PPP packet                  */
  uint16 protocol,               /* protocol whose options are being saved */
  ppp_fsm_side_type* rm_opt_ptr, /* Ptr to Rm options                      */
  ppp_fsm_side_type* um_opt_ptr  /* Ptr to Um options                      */
)
{
  dsm_item_type* item_ptr;       /* Working ptr to PPP packet              */
  struct option_hdr opt_hdr;     /* option header                          */
  struct config_hdr cfg_hdr;     /* Config hdr: has Code, ID, len          */
  sint31 pkt_len = 0;                /* Length of the entire packet            */
  uint16 offset;                 /* offset from begining of current item   */
  uint16 copy_len;               /* Number of bytes copied into buffer     */
  byte opt_val[PPP_OPT_MAX_LEN]; /* array to copy option value into        */

  memset(&opt_hdr, 0, sizeof(struct option_hdr));
  memset(&cfg_hdr, 0, sizeof(struct config_hdr));
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( item_head_ptr  == NULL ||
      *item_head_ptr == NULL ||
      rm_opt_ptr     == NULL ||
      um_opt_ptr     == NULL )
  {
     DATA_ERR_FATAL("NULL arguments passed");
     return RET_ERROR;
  }

  item_ptr = *item_head_ptr;

  /*-------------------------------------------------------------------------
    Extract the code, id and length fields (4 bytes) into cfg_hdr. Convert
    the length from network to host.
  -------------------------------------------------------------------------*/
  copy_len = CONFIG_HDR_LEN;
  offset   = 0;
  if(dsm_seek_extract(&item_ptr, &offset, &cfg_hdr, &copy_len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "seek_extract() failed - dumping pkt");
    dsm_free_packet(item_head_ptr);
    return RET_ERROR;
  }
  pkt_len = ps_ntohs(cfg_hdr.length);
  pkt_len -= CONFIG_HDR_LEN;
  offset += CONFIG_HDR_LEN;

  /*-------------------------------------------------------------------------
    Reset the negotiated option fields to zero
  -------------------------------------------------------------------------*/
  rm_opt_ptr->work = 0;
  um_opt_ptr->want = 0;
  if(protocol ==  PPP_LCP_PROTOCOL)
  {
    memset(rm_opt_ptr->work_pdv, 0, sizeof(lcp_value_type));
  }

  else /* is IPCP */
  {
    memset(rm_opt_ptr->work_pdv, 0, sizeof(ipcp_value_type));
  } /* if(LCP) */

  /*-------------------------------------------------------------------------
    Go through the entire PPP packet, one option at a time.
  -------------------------------------------------------------------------*/
  while ( pkt_len > 0)
  {
    /*-----------------------------------------------------------------------
      Get the option header, and update the packet length to reflect that the
      header has been grabbed.
    -----------------------------------------------------------------------*/
    copy_len = OPTION_HDR_LEN;
    if(dsm_seek_extract(&item_ptr, &offset, &opt_hdr, &copy_len) == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "seek_extract() failed - dumping pkt");
      dsm_free_packet( item_head_ptr);
      return RET_ERROR;
    }

    if ( opt_hdr.length < OPTION_HDR_LEN)
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "option length %d < required at least "
                      "2 bytes - dumping pkt",
                      opt_hdr.length);
      dsm_free_packet( item_head_ptr);
      return RET_ERROR;
    }

    pkt_len -= opt_hdr.length;

    /*-----------------------------------------------------------------------
      Get the options value IFF it has one.
    -----------------------------------------------------------------------*/
    offset += OPTION_HDR_LEN;
    copy_len =  opt_hdr.length - OPTION_HDR_LEN;
    if ( ( copy_len > 0 ) && ( copy_len <= PPP_OPT_MAX_LEN ) )
    {
      if(dsm_seek_extract(&item_ptr, &offset, opt_val, &copy_len) == FALSE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "seek_extract() failed - dumping pkt");
        dsm_free_packet( item_head_ptr);
        return RET_ERROR;
       }
    }
    offset += copy_len;

    /*-----------------------------------------------------------------------
      Save the option for the protocol in question.
    -----------------------------------------------------------------------*/
    if(protocol == PPP_LCP_PROTOCOL)
    {
      lcp_save_work_opt(rm_opt_ptr, opt_hdr.type, opt_val);
    }

    else /* is IPCP */
    {
      ipcp_save_work_opt(rm_opt_ptr, opt_hdr.type, opt_val, copy_len);
    }

  } /* while(more options) */

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    /*-------------------------------------------------------------------------
      JCDMA requires that DNS options should not be negotiated during a resync,
      so we need to disable DNS negotiation. Modify the Rm work to exclude the
      DNS options before copying it into the other masks.
    -------------------------------------------------------------------------*/
    if(protocol == PPP_IPCP_PROTOCOL)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "netmdli_save_opts, Disabling DNS option in IPCP");
      /*-----------------------------------------------------------------------
        Disable DNS options (primary and secondary)
      -----------------------------------------------------------------------*/
      rm_opt_ptr->work &= ~IPCP_N_PRIMARY_DNS;
      rm_opt_ptr->work &= ~IPCP_N_SECONDARY_DNS;
    }
  }
  /*-------------------------------------------------------------------------
    Save the negotiated work values into the future want values, and also the
    work_pdv into the want_pdv.
  -------------------------------------------------------------------------*/
  rm_opt_ptr->will = rm_opt_ptr->work;
  rm_opt_ptr->want = rm_opt_ptr->work;
  um_opt_ptr->will = rm_opt_ptr->work;
  um_opt_ptr->want = rm_opt_ptr->work;
  um_opt_ptr->work = rm_opt_ptr->work;
  
  if(protocol ==  PPP_LCP_PROTOCOL)
  {
    memscpy(((lcp_value_type*)(rm_opt_ptr->want_pdv)),
            sizeof(lcp_value_type),
            ((lcp_value_type*)(rm_opt_ptr->work_pdv)),
            sizeof(lcp_value_type));
    memscpy(((lcp_value_type*)(um_opt_ptr->want_pdv)),
            sizeof(lcp_value_type),
            ((lcp_value_type*)(rm_opt_ptr->work_pdv)),
            sizeof(lcp_value_type));
    memscpy(((lcp_value_type*)(um_opt_ptr->work_pdv)),
            sizeof(lcp_value_type),
            ((lcp_value_type*)(rm_opt_ptr->work_pdv)),
            sizeof(lcp_value_type));
  }

  else /* is IPCP */
  {
    memscpy(((ipcp_value_type*)(rm_opt_ptr->want_pdv)),
            sizeof(ipcp_value_type),
            ((ipcp_value_type*)(rm_opt_ptr->work_pdv)),
            sizeof(ipcp_value_type));
    memscpy(((ipcp_value_type*)(um_opt_ptr->want_pdv)),
            sizeof(ipcp_value_type),
            ((ipcp_value_type*)(rm_opt_ptr->work_pdv)),
            sizeof(ipcp_value_type));
    memscpy(((ipcp_value_type*)(um_opt_ptr->work_pdv)),
            sizeof(ipcp_value_type),
            ((ipcp_value_type*)(rm_opt_ptr->work_pdv)),
            sizeof(ipcp_value_type));
  } /* if(LCP) */

  /*-------------------------------------------------------------------------
    And we are done.
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, "protocol %x loc opts %x, rem opts %x",
          protocol,
          rm_opt_ptr->work,
          rm_opt_ptr->work);

  return RET_OK;

} /* ds707_netmdli_save_opts() */



  /*lint -esym(715, meta_info_ref_ptr) */
/*===========================================================================
FUNCTION   DS707_NETMDLI_INIT_CALLBACK_LCP_C_REQ()

DESCRIPTION
  The callback function that is called when LCP C-Req is found.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds707_netmdli_init_callback_lcp_c_req
(
  ppp_dev_enum_type device,           /* the arrival device               */
  uint16           protocol,         /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
  uint16 len = 1;                   /* the length of a PPP ID is one byte  */
  uint16 offset = 1;                /* offset to use in seek_extract: ID=1 */
  dsm_item_type *item_ptr;          /* pointer to the item                 */
  ppp_dev_enum_type out_device = PPP_MIN_DEV; /* PPP_MIN_DEV is 0*/
  byte pkt_id = 0;                      /* ID field of packet                  */
  uint8 tempValue;
  ppp_type         * ppp_cb_array  = NULL;
  ppp_fsm_type     * um_fsm_ptr    = NULL; 
  dsm_item_type    * reply_pkt     = NULL;
  uint16             auth_proto;       /* Authentication-Protocol in C-Req */
  lcp_value_type     reply_option;     /* CHAP/PAP                         */
  uint8              reply_result;     /* C-Nak/C-Rej if auth proto is PAP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( item_head_ptr           == NULL ||
      *item_head_ptr          == NULL ||
      (*item_head_ptr)->size  == 0    ||
      (*item_head_ptr)->used  == 0 )
  {
     DATA_ERR_FATAL("Invalid DSM item passed");
     return SNOOP_CB_IGNORE_PKT;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();

  if( device   >= PPP_MAX_DEV      ||
      protocol != PPP_LCP_PROTOCOL ||
      ppp_cb_array[device].bridge_dev >= PPP_MAX_DEV )
  {
     if( device >= PPP_MAX_DEV )
     {
       DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                       "Invalid device passed: device %d", device);
     }
     else
     {
       DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                       "Invalid args passed: protocol %d, bridge_dev %d",
                       protocol, ppp_cb_array[device].bridge_dev);
     }
     ASSERT( 0 );
     dsm_free_packet(item_head_ptr);
     return SNOOP_CB_IGNORE_PKT;
  }

  item_ptr = *item_head_ptr;
  out_device = ppp_cb_array[device].bridge_dev;
    
  if(out_device >= PPP_MAX_DEV)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Invalid PPP Device %d", out_device);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }
    
  /*-------------------------------------------------------------------------
    Take note of id (for future C-Ack, or for sending C-Nak/C-Rej)
  -------------------------------------------------------------------------*/
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &pkt_id,
                      &len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "seek_extract() failed - dumping pkt");
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  if ( TRUE == dsgen_iface_hdlr_use_internal_auth() ||
       uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_RUIM || 
       uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM ||
       NVRUIM_NON_3GPD_CARD == nvruim_check_non_3gpd_cdma_card() )
  {
    /*-------------------------------------------------------------------------
      If we need to do encrypted authentication (CHAP) on Um PPP, examine the 
      LCP C-Req to see if it contains PAP for the AP option. 
      NOTE: This needs to happen only for Um.
    -------------------------------------------------------------------------*/
    if( device == PPP_UM_SN_DEV &&
        ppp_cb_array[device].auth.require_encrypted_passwd )
    {
      um_fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      if( fsm_find_option( item_head_ptr, 
                           LCP_AP,
                           &auth_proto,
                           LCP_AP_LEN ) == LCP_AP_LEN )
      {
        if( ps_ntohs( auth_proto ) == PPP_PAP_PROTOCOL )
        {
          DATA_IS707_MSG1(MSG_LEGACY_MED, 
                          "LCP C-Req (id %d), PAP Requested on Um", pkt_id);
 
          /*-------------------------------------------------------------------
            If retries remain, send an LCP C-NAK proposing CHAP, else C-Rej AP
          -------------------------------------------------------------------*/
          if( um_fsm_ptr->retry_nak > 0 )
          {
            DATA_IS707_MSG0(MSG_LEGACY_MED, 
                            "C-Naking PAP, proposing CHAP for Um PPP");
            reply_option.authentication = PPP_CHAP_PROTOCOL;
            reply_option.chap_digest    = PPP_CHAP_MD5;
            reply_result                = CONFIG_NAK;
            um_fsm_ptr->retry_nak--;
          } 
          else
          {
            DATA_IS707_MSG0(MSG_LEGACY_MED, 
                            "Rejecting PPP Authentication-Protocol option");
            reply_option.authentication = PPP_PAP_PROTOCOL;
            reply_result                = CONFIG_REJ;
          }
        
          reply_pkt = dsm_offset_new_buffer( DSM_DS_SMALL_ITEM_POOL, 
                                             PPP_MAX_HDR );
          if( reply_pkt != NULL )
          {
            (*um_fsm_ptr->pdc->option)( um_fsm_ptr,
                                        &reply_option,
                                        &reply_pkt,
                                        LCP_AP );
            (void)fsm_send( um_fsm_ptr, reply_result, pkt_id, reply_pkt );
          }
          else
          {
            DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                         "Couldn't allocate memory for LCP pkt code %d, id %d",
                         reply_result, pkt_id);
          }
          
          /*-------------------------------------------------------------------
            Free the incoming C-Req packet
          -------------------------------------------------------------------*/
          dsm_free_packet( item_head_ptr );
          return SNOOP_CB_IGNORE_PKT;
        } /* if PAP is being proposed */
      }
   
      /*-----------------------------------------------------------------------
        We reach here if the incoming C-Req did not contain PAP; reset the 
        C-NAK retry counter to the original value (obtained from NV elsewhere).
      -----------------------------------------------------------------------*/
      um_fsm_ptr->retry_nak = um_fsm_ptr->try_nak; 
    }/* if Um device and need encrypted passwords */
  }

  /*-------------------------------------------------------------------------
    Only store the ID in the packet IFF the packet hasn't been freed (i.e. a
    C-Rej was made.  If the packet has been freed, then point the head_ptr to
    the new packet that contains the C-Rej.
  -------------------------------------------------------------------------*/
  if ( (out_device < PPP_MAX_DEV) && (*item_head_ptr != NULL) )
  {
    ds707_curr_ppp_id[out_device] = pkt_id;
    /*lint -save -e539 */
    DATA_IS707_MSG3(MSG_LEGACY_MED, "C-Req CB device %d, id %d, proto %x",
                    device, ds707_curr_ppp_id[out_device], protocol);
    /*lint -restore */
  }

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_STARTING event.
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                             device,
                             PPP_PROTOCOL_LCP,
                             PS_PPP_EVENT_UNKNOWN, 
                             PS_PPP_EVENT_STARTING );

  /*-------------------------------------------------------------------------
    Clear the Have-we-seen-an-Ack status, since a new C-Req was just seen
  -------------------------------------------------------------------------*/
  if(device == PPP_RM_DEV )
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "clearing LCP Um_ACK_RCVD rxed C-Req from RM");
    tempValue = (uint8) ds707_lcp_c_ack_status & ~Um_ACK_RCVD;
    ds707_lcp_c_ack_status = (ack_status_enum_type) tempValue;
  }
  else if(device == PPP_UM_SN_DEV )
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "clearing LCP Rm_ACK_RCVD rxed C-Req from UM");
    tempValue = (uint8) ds707_lcp_c_ack_status & ~Rm_ACK_RCVD;
    ds707_lcp_c_ack_status = (ack_status_enum_type) tempValue;
  }

  /*-------------------------------------------------------------------------
    Assign protocol to packet kind field and forward packet.
  -------------------------------------------------------------------------*/
  if ( (device < PPP_MAX_DEV) && (out_device < PPP_MAX_DEV) && ((*item_head_ptr) != NULL))
  {
    /*-----------------------------------------------------------------------
      Change phase to establish for device and out_device
    -----------------------------------------------------------------------*/
    if( ppp_cb_array[device].phase == pppDEAD )
    {
      ppp_cb_array[device].phase = pppESTABLISH;
    }
    if( ppp_cb_array[out_device].phase == pppDEAD )
    {
      ppp_cb_array[out_device].phase = pppESTABLISH;
    }
    DSM_TOUCH_ITEM(*item_head_ptr);
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }

  return SNOOP_CB_IGNORE_PKT;

} /* ds707_netmdli_init_callback_lcp_c_req() */



/*===========================================================================
FUNCTION   DS707_NETMDLI_INIT_CALLBACK_LCP_C_ACK()

DESCRIPTION
  The callback function that is called when an LCP C-Ack is found.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds707_netmdli_init_callback_lcp_c_ack
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16            protocol,          /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
  dsm_item_type *item_ptr;           /* pointer to the item                */
  ppp_type      *ppp_cb_ptr;         /* ptr to incoming PPP cb             */
  ppp_type      *ppp_cb_array  = NULL;
  uint16 len = 1;                    /* the length of a PPP ID is one byte */
  uint16 offset = 1;  /* offset is one past the start of what we are given */
  byte id = 0;          /* to store the ID of the packet to use for comparison */
  ppp_dev_enum_type out_device;
  lcp_value_type * um_want_pdv = NULL;
  boolean use_same_auth = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( item_head_ptr           == NULL ||
      *item_head_ptr          == NULL ||
      (*item_head_ptr)->size  == 0    ||
      (*item_head_ptr)->used  == 0 )
  {
     DATA_ERR_FATAL("Invalid DSM item passed");
     return SNOOP_CB_IGNORE_PKT;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  if( device   >= PPP_MAX_DEV      ||
      protocol != PPP_LCP_PROTOCOL ||
      ppp_cb_array[device].bridge_dev >= PPP_MAX_DEV )
  {
     if( device >= PPP_MAX_DEV )
     {
       DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                       "Invalid device passed: device %d", device);
     }
     else
     {
       DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                       "Invalid args passed: protocol %d, bridge_dev %d",
                       protocol, ppp_cb_array[device].bridge_dev);
     }
     ASSERT( 0 );
     dsm_free_packet(item_head_ptr);
     return SNOOP_CB_IGNORE_PKT;
  }

  item_ptr   = *item_head_ptr;
  ppp_cb_ptr = ppp_cb_array + device;
  out_device = ppp_cb_ptr->bridge_dev;
  if( out_device >= PPP_MAX_DEV )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Invalid device : out_device %d", 
                     out_device);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Make sure that we are in Psuedo Network mode, or Full Net mode.
    If we are not log error and dump packet
  -------------------------------------------------------------------------*/
  /*lint -save -e661 */
  if(ppp_cb_ptr->mode != PPP_PSEUDO_NET_MODE &&
     ppp_cb_ptr->mode != PPP_FULL_NET_MODE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Saw LCP C-Ack when NOT in Pseudo "
                    "Net/Full Net mode! Dumping packet");
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Get the ID of the packet, then compare it to the current PPP id.  If it
    is the same then we must take note of LCP options done, otherwise ignore
    packet and send it on it's way.
  -------------------------------------------------------------------------*/
  if( dsm_seek_extract(&item_ptr, &offset, &id, &len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "seek_extract() failed - dumping pkt");
    dsm_free_packet( item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, "LCP C-Ack CB device %d, id: %d", device, id);

  if(id == ds707_curr_ppp_id[device])
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, "Saving LCP options for device %d", device);

    /*-----------------------------------------------------------------------
      Save the options in the PPP control block on the Rm side
      NOTE: if we received this C-Ack on the Um it is the TE2 options that
            are accepted, so they are LOCAL for the Um device, the remote
            options are for the Rm interface.  (The same holds true if TE2 is
            replaced with IWF and Um and Rm are swapped).
    -----------------------------------------------------------------------*/
    if(device == PPP_RM_DEV)
    {
      /*---------------------------------------------------------------------
        JCDMA requires we use the same auth protocol in resync as in initial
        establishment. This is achieved via Um remote want's use_same_auth 
        boolean. Cache this value since it will get clobbered with Rm's 
        values in ds707_netmdli_save_opts().
      ---------------------------------------------------------------------*/
      /*Simplified JCDMA: 3.2.1 Use the same PPP Auth type in the 
        reconnection [D1-No.63]*/
      if (ds707_roaming_is_curr_mode_jcdma() ||
          ds707_roaming_is_simplified_jcdma_mode())
      {
        um_want_pdv = 
          (lcp_value_type *)ppp_cb_array[out_device].fsm[LCP].remote.want_pdv;
        if( um_want_pdv != NULL )
        {
           use_same_auth = um_want_pdv->use_same_auth;
        }
      }
      
      if(ds707_netmdli_save_opts( item_head_ptr,
                            protocol,
                            &(ppp_cb_ptr->fsm[LCP].local),
                            &(ppp_cb_array[out_device].fsm[LCP].remote))
         == RET_ERROR)
      {
        return SNOOP_CB_IGNORE_PKT;
      }

      /*---------------------------------------------------------------------
            Restore the cached use_same_auth value for use in resyncs.
           ---------------------------------------------------------------------*/
      /*Simplified JCDMA: 3.2.1 Use the same PPP Auth type in the 
              reconnection [D1-No.63]*/
      if (ds707_roaming_is_curr_mode_jcdma() ||
          ds707_roaming_is_simplified_jcdma_mode())
      {
         if( um_want_pdv != NULL )
         {
            um_want_pdv->use_same_auth = use_same_auth;
         }
      }
    }

    else /* Um device */
    {
      if(ds707_netmdli_save_opts(item_head_ptr,
                           protocol,
                           &(ppp_cb_array[out_device].fsm[LCP].remote),
                           &(ppp_cb_ptr->fsm[LCP].local)) == RET_ERROR)
      {
        return SNOOP_CB_IGNORE_PKT;
      }
    }
    
  } /* if(id matches) */

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_OPEN event. Post an open on the bridged device: 
    the originating device may still need to send config requests.
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                             out_device,
                             PPP_PROTOCOL_LCP,
                             PS_PPP_EVENT_UNKNOWN, 
                             PS_PPP_EVENT_OPEN );  
  /*-------------------------------------------------------------------------
    Forward the packet.  This should happen *before* lcp_setup_device is 
    called (below). lcp_setup_device can change the accm used for framing, 
    and this must not affect the current packet.
  -------------------------------------------------------------------------*/
  if ( (*item_head_ptr) != NULL)
  {
    DSM_TOUCH_ITEM(*item_head_ptr);
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }

  /*-------------------------------------------------------------------------
    Update the LCP C-Ack status. Call lcp_setup_device if LCP C-Acks 
    have been seen on both Rm and Um.
  -------------------------------------------------------------------------*/
  if(id == ds707_curr_ppp_id[device])
  {
    if(device == PPP_RM_DEV)
    {
      if(ds707_lcp_c_ack_status & Rm_ACK_RCVD)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Received two Rm LCP C-Acks!");
      }
      ds707_lcp_c_ack_status |= Rm_ACK_RCVD;
    }

    else /* this came in on Um */
    {
      if(ds707_lcp_c_ack_status & Um_ACK_RCVD)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Received two Um LCP C-Acks!");
      }
      ds707_lcp_c_ack_status |= Um_ACK_RCVD;
    }

    /*-----------------------------------------------------------------------
      Have we seen both acks?
    -----------------------------------------------------------------------*/
    if(ds707_lcp_c_ack_status == BOTH_ACKS_RCVD)
    {
      /*---------------------------------------------------------------------
        LCP is established; call LCP setup function for both Rm and Um.
        Note that the packet received by this callback function has already
        been forwarded to the bridge device. This is important; since 
        lcp_setup_device can change the accm applied for framing and that
        shouldn't affect the current packet.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "LCP now established!");
      lcp_setup_device( device );
      lcp_setup_device( out_device );
    } /* if(both acks received) */
  } /* if(id matches) */
  
  return SNOOP_CB_IGNORE_PKT;

  /*lint -restore */
} /* ds707_netmdli_init_callback_lcp_c_ack() */



/*===========================================================================
FUNCTION   DS707_NETMDLI_INIT_CALLBACK_IPCP_C_REQ()

DESCRIPTION
  The callback function that is called when an IPCP C-Req is found.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds707_netmdli_init_callback_ipcp_c_req
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
  dsm_item_type *item_ptr;     /* pointer to the item                      */
  uint16 len = 1;              /* the length of a PPP ID is one byte       */
  uint16 offset = 1;           /* the ID field is 1 past beginning of data */
  uint8  pkt_id = 0;
  uint8 tempValue;
  ppp_dev_enum_type out_device;
  ppp_type *ppp_cb_array  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( item_head_ptr           == NULL ||
      *item_head_ptr          == NULL ||
      (*item_head_ptr)->size  == 0    ||
      (*item_head_ptr)->used  == 0 )
  {
     DATA_ERR_FATAL("Invalid DSM item passed");
     return SNOOP_CB_IGNORE_PKT;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();

  if( device   >= PPP_MAX_DEV       ||
      protocol != PPP_IPCP_PROTOCOL ||
      ppp_cb_array[device].bridge_dev >= PPP_MAX_DEV )
  {
     if( device >= PPP_MAX_DEV )
     {
       DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                       "Invalid device passed: device %d", device);
     }
     else
     {
       DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                       "Invalid args passed: protocol %d, bridge_dev %d",
                       protocol, ppp_cb_array[device].bridge_dev);
     }
     ASSERT( 0 );
     dsm_free_packet(item_head_ptr);
     return SNOOP_CB_IGNORE_PKT;
  }

  item_ptr   = *item_head_ptr;
  out_device = ppp_cb_array[device].bridge_dev;
  if( out_device >= PPP_MAX_DEV )
  {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid device : out_device %d",
                      out_device);
      dsm_free_packet(item_head_ptr);
      return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Take note of id for future C-Ack
  -------------------------------------------------------------------------*/
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &(pkt_id),
                      &len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "seek_extract() failed - dumping pkt");
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }
  ds707_curr_ppp_id[device] = pkt_id;
  
  DATA_IS707_MSG2(MSG_LEGACY_MED, "Rx Init IPCP CReq ifac %d id %d", device, pkt_id);

  /*-------------------------------------------------------------------------
   Generate a PS_PPP_EVENT_STARTING event.
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                             device,
                             PPP_PROTOCOL_IPCP,
                             PS_PPP_EVENT_UNKNOWN, 
                             PS_PPP_EVENT_STARTING );

  /*-------------------------------------------------------------------------
    Change phase to AUTHENTICATE for device
  -------------------------------------------------------------------------*/
  if( ppp_cb_array[device].phase == pppESTABLISH )
  {
    ppp_cb_array[device].phase = pppAUTHENTICATE;
  }

  /*-------------------------------------------------------------------------
    Clear the Have-we-seen-an-Ack status, since a new C-Req was just seen
  -------------------------------------------------------------------------*/
  if(device == PPP_RM_DEV )
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "clearing IPCP Um_ACK_RCVD rxed C-Req from RM");
    tempValue = ds707_ipcp_c_ack_status & ~Um_ACK_RCVD;
    ds707_ipcp_c_ack_status = (ack_status_enum_type) tempValue;
  }
  else if(device == PPP_UM_SN_DEV )
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "clearing IPCP Rm_ACK_RCVD rxed C-Req from UM");
    
    tempValue = ds707_ipcp_c_ack_status & ~Rm_ACK_RCVD;
    ds707_ipcp_c_ack_status = (ack_status_enum_type) tempValue;
  }

  /*-------------------------------------------------------------------------
    Store protocol in packet kind field and forward to the other interface 
    (other than the receiving interface).
  -------------------------------------------------------------------------*/
  if ((*item_head_ptr) != NULL)
  {
    DSM_TOUCH_ITEM(*item_head_ptr);
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }
  return SNOOP_CB_IGNORE_PKT;

} /* ds707_netmdli_init_callback_ipcp_c_req() */



/*===========================================================================
FUNCTION   DS707_NETMDLI_INIT_CALLBACK_IPCP_C_ACK()

DESCRIPTION
  This is the callback function that is called for IPCP C-Acks.  It handles
  to storing of all of the IPCP parameters for the resync PPP stack.  It also
  will inform the Mobile Node Registration task that PPP has completed (if
  MNR support is compiled in) by invoking a callback function.  Currently the
  name of this function is hardwired, although this might change.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds707_netmdli_init_callback_ipcp_c_ack
(
  ppp_dev_enum_type device,           /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
  dsm_item_type *item_ptr; /* pointer to the item         */
  ppp_type      *ppp_cb_ptr; 
  ppp_type      *ppp_cb_array  = NULL;
  uint16 len = 1;    /* the length of a PPP ID is one byte                 */
  uint16 offset = 1; /* offset is one past the start of what we are given  */
  byte pkt_id = 0;      /* to store the ID of the packet to use for comparison */
  ppp_dev_enum_type out_device;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( item_head_ptr           == NULL ||
      *item_head_ptr          == NULL ||
      (*item_head_ptr)->size  == 0    ||
      (*item_head_ptr)->used  == 0 )
  {
     DATA_ERR_FATAL("Invalid DSM item passed");
     return SNOOP_CB_IGNORE_PKT;
  }

  item_ptr = *item_head_ptr;
  ppp_cb_array = fsm_get_ppp_cb_array();

  if( device   >= PPP_MAX_DEV      ||
      protocol != PPP_IPCP_PROTOCOL ||
      ppp_cb_array[device].bridge_dev >= PPP_MAX_DEV )
  {
     if( device >= PPP_MAX_DEV )
     {
       DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                       "Invalid device passed: device %d", device);
     }
     else
     {
       DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                       "Invalid args passed: protocol %d, bridge_dev %d",
                       protocol, ppp_cb_array[device].bridge_dev);
     }
     ASSERT( 0 );
     dsm_free_packet(item_head_ptr);
     return SNOOP_CB_IGNORE_PKT;
  }


  ppp_cb_ptr = ppp_cb_array + device;
  out_device = ppp_cb_ptr->bridge_dev;

  if( out_device >= PPP_MAX_DEV )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Invalid device : out_device %d", 
                     out_device);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }
  /*-------------------------------------------------------------------------
    Get the ID of the packet, then compare it to the current PPP id.  If it
    is the same then negotiations might be done, otherwise ignore packet and
    send it on it's way.
  -------------------------------------------------------------------------*/
  if( dsm_seek_extract(&item_ptr, &offset, &pkt_id, &len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Seek extract failed - dumping pkt");
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  DATA_IS707_MSG2(MSG_LEGACY_LOW, "IPCP C_Ack, device %d, id =%d", device, pkt_id);

  if(pkt_id == ds707_curr_ppp_id[out_device])
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, "Saving IPCP options for device %d", device);

    /*-----------------------------------------------------------------------
      Save the options in the PPP control block
      NOTE: if we received this C-Ack on the Um it is the TE2 options that
            are accepted, so they are LOCAL for the Um device, the remote
            options are for the Rm interface.  (THE same holds true if TE2 is
            replaced with IWF and Um and Rm are swapped).
    -----------------------------------------------------------------------*/
    if(device == PPP_RM_DEV)
    {
      if(ds707_netmdli_save_opts(item_head_ptr,
                           protocol,
                           &(ppp_cb_ptr->fsm[IPCP].local),
                           &(ppp_cb_array[out_device].fsm[IPCP].remote))
          == RET_ERROR)
      {
        return SNOOP_CB_IGNORE_PKT;
      }
    }

    else /* is from Um interface */
    {
      if(ds707_netmdli_save_opts( item_head_ptr,
                           protocol,
                           &(ppp_cb_array[out_device].fsm[IPCP].remote),
                           &(ppp_cb_ptr->fsm[IPCP].local)) == RET_ERROR)
      {
        return SNOOP_CB_IGNORE_PKT;
      }
    }

    /*-----------------------------------------------------------------------
      Check the interface on which this arrived, to determine the ack status
    -----------------------------------------------------------------------*/
    if(device == PPP_RM_DEV)
    {
      if(ds707_ipcp_c_ack_status & Rm_ACK_RCVD)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Received two Rm IPCP C-Acks!");
      }
      ds707_ipcp_c_ack_status |= Rm_ACK_RCVD;
    }

    else /* this came in on Um */
    {
      if(ds707_ipcp_c_ack_status & Um_ACK_RCVD)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Received two Um IPCP C-Acks!");
      }
      ds707_ipcp_c_ack_status |= Um_ACK_RCVD;
    }

    /*---------------------------------------------------------------------
      Generate a PS_PPP_EVENT_OPEN event.  Post an open on the bridged 
      device: the originating device may still need to send config 
      requests.
    ---------------------------------------------------------------------*/
    pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                               out_device,
                               PPP_PROTOCOL_IPCP,
                               PS_PPP_EVENT_UNKNOWN, 
                               PS_PPP_EVENT_OPEN );    

    /*-----------------------------------------------------------------------
      Have we seen both acks?
    -----------------------------------------------------------------------*/
    if(ds707_ipcp_c_ack_status == BOTH_ACKS_RCVD)
    {
      /*---------------------------------------------------------------------
        We have seen IPCP C-Acks on both interfaces, so we know PPP is
        established. To support INTERNAL mode or laptop calls, the device
        need to be configured for IP. Setup IP configuration for both the 
        devices and change to the PPP upstate.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "PPP now established!, PPP UP: Inform RMSM");

      ipcp_setup_device(device);
      ipcp_setup_device(out_device);

      ds707_rmsm_post_event (DS707_RMSM_E2E_PPP_UP_EV);
    } /* if(both acks received) */

  } /* if(expected ID) */

  /*-------------------------------------------------------------------------
    Assign protocol to packet kind field and forward packet.
  -------------------------------------------------------------------------*/
  if((*item_head_ptr) != NULL)
  {
    DSM_TOUCH_ITEM(*item_head_ptr);
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }

  return SNOOP_CB_IGNORE_PKT;


} /* ds707_netmdli_init_callback_ipcp_c_ack() */



/*===========================================================================
FUNCTION   DS707_NETMDLI_INIT_CALLBACK_PAP_A_REQ()

DESCRIPTION
  This is the callback function that is called when Auth-Req for PAP is
  found. The Incoming packet has 1 byte of Auth-Req, 1 byte of ID, 2 bytes of
  length, 1 byte of user-id length, user-id , 1 byte of password length and
  password.  The User-id and Password are copied from the incoming packet
  into the PAP data structure.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.
  The PAP user-id and the Password of the Mobile are modified with that of 
  the TE2 device. 

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds707_netmdli_init_callback_pap_a_req
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
  dsm_item_type *item_ptr;                          /* pointer to the item */
  ppp_type      *ppp_cb_ptr;
  ppp_type      *ppp_cb_array  = NULL;
  uint16 len;
  uint16 offset;
  ppp_dev_enum_type out_device;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( item_head_ptr           == NULL ||
      *item_head_ptr          == NULL ||
      (*item_head_ptr)->size  == 0    ||
      (*item_head_ptr)->used  == 0 )
  {
     DATA_ERR_FATAL("Invalid DSM item passed");
     return SNOOP_CB_IGNORE_PKT;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  if( device   >= PPP_MAX_DEV      ||
      protocol != PPP_PAP_PROTOCOL ||
      ppp_cb_array[device].bridge_dev >= PPP_MAX_DEV )
  {
     if( device >= PPP_MAX_DEV )
     {
       DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                       "Invalid device passed: device %d", device);
     }
     else
     {
       DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                       "Invalid args passed: protocol %d, bridge_dev %d",
                       protocol, ppp_cb_array[device].bridge_dev);
     }
     ASSERT( 0 );
     dsm_free_packet(item_head_ptr);
     return SNOOP_CB_IGNORE_PKT;
  }

  item_ptr = *item_head_ptr;

  /*-------------------------------------------------------------------------
    Get the device that this user name and password will be forwarded over -
    that is where things will be stored.
  -------------------------------------------------------------------------*/
  out_device = ppp_cb_array[device].bridge_dev;
  ppp_cb_ptr = ppp_cb_array + out_device;

  /*-----------------------------------------------------------------------
    An auth request looks like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | 1 (the value) |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | User-ID Length|  User-Id  ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Passwd-Length |  Password ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+

    Populate the authentication info for the UM device with the info extracted
    from the PAP packet - the total length is of no interest as we will
    extract all of the other fields explicitly.

    Start by extracting the User-ID length - make sure we have enough buffer
    to copy the user ID.
  -------------------------------------------------------------------------*/
  len= PPP_PAP_UID_SIZE;
  offset = PPP_AUTH_CODE_SIZE +
           PPP_AUTH_ID_SIZE   +
           PPP_AUTH_LENGTH_SIZE;
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &ppp_cb_ptr->auth.info.user_id_len,
                      &len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "failed to get user ID length forwarding packet!");
    goto finished;
  }

  if(ppp_cb_ptr->auth.info.user_id_len > PPP_MAX_USER_ID_LEN)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "User ID too long - forwarding packet!");
    ppp_cb_ptr->auth.info.user_id_len = 0;
    goto finished;
  }

  /*-------------------------------------------------------------------------
    We can copy the entire user ID, so extract it.
  -------------------------------------------------------------------------*/
  len = ppp_cb_ptr->auth.info.user_id_len;
  offset += PPP_PAP_UID_SIZE;
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      ppp_cb_ptr->auth.info.user_id_info,
                      &len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "failed to get user ID forwarding packet!");
    ppp_cb_ptr->auth.info.user_id_len = 0;
    goto finished;
  }

  /*-------------------------------------------------------------------------
    Now extract the password length.  Set the offset past the user ID info.
  -------------------------------------------------------------------------*/
  len = PPP_PAP_PWD_SIZE;
  offset += ppp_cb_ptr->auth.info.user_id_len;
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &ppp_cb_ptr->auth.info.passwd_len,
                      &len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "failed to get password length forwarding packet!");
    ppp_cb_ptr->auth.info.user_id_len = 0;
    goto finished;
  }

  /*-------------------------------------------------------------------------
    Make sure we have enough buffer to copy whole thing - if the length is
    zero however, then skip to the end as the seek_extract() will fail as a
    zero read at the end of an item fails.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->auth.info.passwd_len > PPP_MAX_PASSWD_LEN)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Password too long - forwarding packet!");
    ppp_cb_ptr->auth.info.user_id_len = 0;
    ppp_cb_ptr->auth.info.passwd_len = 0;
    goto finished;
  }
  else if(ppp_cb_ptr->auth.info.passwd_len == 0)
  {
    goto finished;
  }

  /*-------------------------------------------------------------------------
    We can copy the entire password, so extract it.
  -------------------------------------------------------------------------*/
  len = ppp_cb_ptr->auth.info.passwd_len;
  offset += PPP_PAP_PWD_SIZE;
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      ppp_cb_ptr->auth.info.passwd_info,
                      &len) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "failed to get passwd forwarding packet!");
    ppp_cb_ptr->auth.info.user_id_len = 0;
    ppp_cb_ptr->auth.info.passwd_len = 0;
  }

 finished:
  DSM_TOUCH_ITEM(*item_head_ptr);
  (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);

  return SNOOP_CB_IGNORE_PKT;

} /* ds707_netmdli_init_callback_pap_a_req() */



/*===========================================================================
FUNCTION   DS707_NETMDLI_INIT_CALLBACK_CHAP_RESP()

DESCRIPTION
This is the callback function that is called when CHAP Response is found. The
Incoming packet has 1 byte of CHAP RESPONSE, 1 byte of ID, 2 bytes of total 
packet length, 1 byte of response length, Hashed response and user-id. The 
User-id will be extracted from the outgoing packet and will be appended to 
the authentication data structure. Password will be a constant value for the 
resync. This will be copied into the PAP data structure.

DEPENDENCIES
This function expects to be called with a dsm_item whose data pointer is 
pointing at the code byte for the ppp packet. The authentication user-id and 
the Password of the Mobile are modified. The user-id will be same as that of 
the TE2 device and Password will be a constant value.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds707_netmdli_init_callback_chap_resp
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16            protocol,          /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info*/
)
{
  /*-------------------------------------------------------------------------
    Contains total length of the CHAP (PPP) packet. This will be read from 
    the incoming CHAP packet. It is 16 bit wide.
  -------------------------------------------------------------------------*/
  uint16 len_of_chap_info = 0;
  
  /*-------------------------------------------------------------------------
    Contains length of the CHAP response (PPP packet without Id, code, 
    user-id etc). This will be read from the incoming CHAP packet. It is 8 
    bits wide.
  -------------------------------------------------------------------------*/
  byte len_of_chap_resp = 0;

  /*-------------------------------------------------------------------------
    len and offset are used to inform "Length of information to extract" 
    and "the offset from which to start" in the incoming PPP packet. Since 
    length of the PPP packet can be 16 bit wide, these are also chosen as 
    16 bit numbers.
  -------------------------------------------------------------------------*/
  uint16 len;
  uint16 offset;

  /*-------------------------------------------------------------------------
    pointer to the item containing the packet.
  -------------------------------------------------------------------------*/
  dsm_item_type *item_ptr = NULL;
  ppp_dev_enum_type out_device;
  ppp_type      *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ppp_cb_array = fsm_get_ppp_cb_array();
  /*lint -save -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_CHAP_PROTOCOL);
  ASSERT(ppp_cb_array[device].bridge_dev < PPP_MAX_DEV);
  /*lint -restore */

  item_ptr = *item_head_ptr;
  /* KW - Not an issue because the ASSERT above ensures 
     that out_device has correct index value */
  out_device = ppp_cb_array[device].bridge_dev;

  /*-------------------------------------------------------------------------
    Extract the total length from the incoming PPP CHAP packet. This length 
    will be used to extract to rest of the information from the packet. 
    Length will be the 3rd byte and 4th byte in the packet from the current 
    position of the pointer. So the offset will be 2 
    
    Number of bytes to read is 2 so the "len" will be 2.
    
    The total length of the CHAP information i.e. 
    len_of_chap_info =    1 byte for Code (i.e CHAP Response)
              + 1 byte for ID 
              + 2 bytes Total packet length 
              + 1 byte for Length of the Response 
              + Length of the CHAP Response in bytes
              + Length of the User-Id in bytes
    will be copied into len_of_chap_info.
  -------------------------------------------------------------------------*/
  len= 2;
  offset = 2;
  if(dsm_seek_extract(&item_ptr,
                       &offset,                 
                       &len_of_chap_info,        
                       &len) == FALSE)       
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Seek extract failed in CHAP call back - dumping pkt");
    dsm_free_packet( item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Extract the length of the CHAP Response field into len_of_chap_resp. 
    This will be 16 (0x10) for the MD5 but may change for other algorithms. 
    Offset should now be 2 past the previous offset, because we should 
    extract from Value Size field (which follows the PPP length field).
    Since Value Size field is only one byte make the len =1; 
  -------------------------------------------------------------------------*/
  offset += 2;        
  len = 1;                            
  if(dsm_seek_extract(&item_ptr,
                        &offset,
                        &len_of_chap_resp,
                        &len) == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Seek extract failed - dumping pkt");
      dsm_free_packet( item_head_ptr);
      return SNOOP_CB_IGNORE_PKT;
    }

  /*-------------------------------------------------------------------------
    Populate the authentication info for the UM device with the info extracted
    from the CHAP packet
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Extract the user_id_len and User_id from the CHAP packet into the 
    authentication data structure. 
    
    Modify the endianness of len_of_chap_info to get exact length. Subtract
    ( 5 + len_chap_response) to remove the code (1 byte), ID (1 byte), PPP 
    packet length ( 2 bytes), Value size (1 byte) and response bytes 
    (len_of_chap_resp).
    
    Offset is now pointing to the value_size field. This should now point 
    to the user_id field. So add "contents of value_size" field + 1
    (1 for Value size field) to obtain the new offset. 
    
    Copy the length of the user_id into the auth_info.user_id_len. 
    Copy the user_id into the auth_info.user_id_info;
  -------------------------------------------------------------------------*/
  len = (ps_ntohs(len_of_chap_info)) - (len_of_chap_resp + 5);
  offset += (len_of_chap_resp + 1);                                   
  /*lint -save -e545 Ignoring Suspicious use of & */
  if(PPP_MAX_USER_ID_LEN < len ||
     dsm_seek_extract(&item_ptr,
                      &offset,
                      &(ppp_cb_array[out_device].auth.info.user_id_info),
                      &len) == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Seek extract failed - dumping pkt");
      dsm_free_packet( item_head_ptr);
      return SNOOP_CB_IGNORE_PKT;
    }
  /*lint -restore*/
  ppp_cb_array[out_device].auth.info.user_id_len = (uint8)len;


  /*-------------------------------------------------------------------------
    Copy the Password and password length into the authentication data 
    structure.
    
    Note that during resync the MS does not use the same password as the 
    TE2. But it uses a constant password. This password is stored in 
    ppp_chap_resync_passwd_info. This should be copied into the password 
    fields of auth_info.
    
    Obtain the size in bytes for the ppp_chap_resync_passwd and store it 
    in auth_info.passwd_len.
    Copy the memcpy(void *dest, const void *src, uint16 count);
  -------------------------------------------------------------------------*/
  ppp_cb_array[out_device].auth.info.passwd_len = CHAP_RESYNC_PASSWD_LENGTH;

  memscpy(ppp_cb_array[out_device].auth.info.passwd_info, 
          PPP_MAX_PASSWD_LEN,
          ppp_chap_resync_passwd,
          ppp_cb_array[out_device].auth.info.passwd_len);

  /*-------------------------------------------------------------------------
    The auth_info structure is now populated with the CHAP information 
    needed for the resync. 
    
    Transmit the original packet over the Um interface.
  -------------------------------------------------------------------------*/
  if ((*item_head_ptr) != NULL)
  {
    DSM_TOUCH_ITEM(*item_head_ptr);
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }

  /*-------------------------------------------------------------------------
    Since we donot have any thing else to process in this packet return FALSE
  -------------------------------------------------------------------------*/
  return SNOOP_CB_IGNORE_PKT;

} /* ds707_netmdli_init_callback_chap */

/*===========================================================================
FUNCTION DS707_NETMDLI_UM_CHAP_CBACK()

DESCRIPTION
  This function is used to support doing CHAP on the handset during a network
  model call.  The packet is duplicated and will forward one copy to the
  laptop, and have the other processed by the CHAP code.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static snoop_cb_ret_val_enum_type ds707_netmdli_um_chap_cback
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info*/
)
{
  ppp_type *      ppp_cb_ptr;
  ppp_type *      ppp_cb_array = NULL;
  dsm_item_type * dup_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  ASSERT(protocol == PPP_CHAP_PROTOCOL);
  ASSERT(device == PPP_UM_SN_DEV);

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "um_chap_cback called with invalid item!");
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    duplicate the packet so that we have on copy to forward and another to
    process locally.

    NOTE: as we are duping the whole packet, the offset should be zero, and
    the length can be set to the max possible length (which is 0xFFFF for a
    uint16).
  -------------------------------------------------------------------------*/
  (void)dsm_dup_packet(&dup_item, *item_head_ptr, 0, 0xFFFF);

  /*-------------------------------------------------------------------------
    Send the original to the laptop and send the dup to the CHAP processing
    function.
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  (void) pppi_outgoing( ppp_cb_array[device].bridge_dev, 
                        protocol, 
                        item_head_ptr, 
                        NULL );

#ifdef FEATURE_DS_MULTIPLE_NAI
  /*---------------------------------------------------------------------------
    Set RM NAI in ppp_cb_array to laptop value before sending the 
    CHAP response. Multiple NAIs are applicable only for NV mode and so check 
    if phone is in NV mode before copying RM NAI.
  ---------------------------------------------------------------------------*/
  if(uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV || 
     uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV)
  {
    ds707_netmdli_copy_rm_nai(device);
  }
#endif /* FEATURE_DS_MULTIPLE_NAI */

  chap_proc(device, &dup_item);

  /*-------------------------------------------------------------------------
    Stop the CHAP timer.  We will not support CHAP response retransmission
    for netmodel CHAP processing (i.e. in TE2 calls).  This is done because
    the CHAP processing is coupled to the internal PPP stack which is not
    keeping track of the end-to-end PPP negotiation done in network model.

    The network server will be responsible for re-sending the 
    (or sending a new) challenge which is RFC 1323 compliant.
  --------------------------------------------------------------------------*/
  ppp_cb_ptr = ppp_cb_array + device;
  (void)ps_timer_cancel(ppp_cb_ptr->fsm[LCP].timer);

  return SNOOP_CB_IGNORE_PKT;

} /* ds707_netmdli_um_chap_cback() */


#ifdef FEATURE_DS_MULTIPLE_NAI
/*===========================================================================
FUNCTION DS707_NETMDLI_COPY_RM_NAI()

DESCRIPTION
  This function is used to copt RM NAI into the user_id field, before
  sending CHAP response. 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdli_copy_rm_nai
(
  ppp_dev_enum_type device
)
{
  ppp_type *        ppp_cb_ptr;
  ppp_type *        ppp_cb_array = NULL;
  nv_stat_enum_type nv_status;
  char *            nai_temp;
  nv_item_type      nv_item;
  byte              nai_len=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + device;

  nv_status = ps_get_nv_item( NV_DS_SIP_RM_NAI_I, &nv_item);

  if( nv_status != NV_DONE_S )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Laptop NAI cannot be retrieved");
    return;
  }

  nai_temp = (char*) nv_item.ds_sip_rm_nai.nai;

  /*-----------------------------------------------------------------------
    Only copy NAI if its length is valid.  If NAI length is > max allowed
    assume a memory/flash scribble and set length to 0.
  -----------------------------------------------------------------------*/
  nai_len = nv_item.ds_sip_rm_nai.nai_length;
  if( nai_len <= NV_MAX_NAI_LENGTH && nai_len > 0 )
  {
    ppp_cb_ptr->auth.info.user_id_len = nai_len;
    memscpy(ppp_cb_ptr->auth.info.user_id_info,
            PPP_MAX_USER_ID_LEN,
            nai_temp,
            nai_len);
  }
} /* ds707_netmdli_copy_rm_nai() */
#endif /* FEATURE_DS_MULTIPLE_NAI */

static snoop_cb_ret_val_enum_type ds707_netmdli_rm_pap_cback
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info*/
)
{
  ppp_type      *ppp_cb_ptr;         /* ptr to PPP cb             */
  ppp_type      *ppp_cb_array = NULL;
  


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(protocol == PPP_PAP_PROTOCOL);
  ASSERT(device == PPP_RM_DEV);

  DATA_IS707_MSG0(MSG_LEGACY_LOW, "Ate PAP Request from TE2");

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "rm_pap_cback called with invalid item!");
    return SNOOP_CB_IGNORE_PKT;
  }

  pap_internal_proc(PPP_UM_SN_DEV,item_head_ptr);

  /*-------------------------------------------------------------------------
    Stop the PAP timer.  We will not support PAP response retransmission
    for netmodel PAP processing (i.e. in TE2 calls).  This is done because
    the PAP processing is coupled to the internal PPP stack which is not
    keeping track of the end-to-end PPP negotiation done in network model.

  --------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + PPP_UM_SN_DEV;
  
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Canceling the LCP timer");
  ps_timer_cancel(ppp_cb_ptr->fsm[LCP].timer);
  
  return SNOOP_CB_IGNORE_PKT;

} /* ds707_netmdli_rm_pap_cback() */

/*===========================================================================
FUNCTION DS707_NETMDLI_RM_CHAP_CBACK()

DESCRIPTION
  This function is used to swallow any chap reponses from the laptop.  
  It is expected to be used in conjunction with the challenge callback.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

DEPENDENCIES
  None

SIDE EFFECTS
  Any packet that is passed in it is freed.
===========================================================================*/
static snoop_cb_ret_val_enum_type ds707_netmdli_rm_chap_cback
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info*/
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(protocol == PPP_CHAP_PROTOCOL);
  ASSERT(device == PPP_RM_DEV);

  DATA_IS707_MSG0(MSG_LEGACY_LOW, "Ate CHAP response from TE2");

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "rm_chap_cback called with invalid item!");
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    We are freeing the item here with the expectation that the Mobile is
    generating the response, so this one is meaningless.
  -------------------------------------------------------------------------*/
  dsm_free_packet(item_head_ptr);

  return SNOOP_CB_IGNORE_PKT;

} /* ds707_netmdli_rm_chap_cback() */

/*===========================================================================
FUNCTION DS707_NETMDLI_CALLBACK_CCP_C_REQ()

DESCRIPTION
  This function is used to send CCP rejects for any CCP requests seen

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

DEPENDENCIES
  None

SIDE EFFECTS
  Any packet that is passed in it is freed.
===========================================================================*/
static snoop_cb_ret_val_enum_type ds707_netmdli_callback_ccp_c_req
(
  ppp_dev_enum_type  device,           /* the arrival device               */
  uint16             protocol,         /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
  static byte id = 0;
  PACKED struct PACKED_POST
  {
    byte    code;
    byte    id;
    uint16  length;
    uint16  rejected_protocol;
  } p_rej_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(protocol == PPP_CCP_PROTOCOL || protocol == PPP_ILCCP_PROTOCOL);
  ASSERT(device == PPP_RM_DEV || device == PPP_UM_SN_DEV);

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "callback_ccp_c_req called with invalid item! "
                    "Return SNOOP_CB_IGNORE_PKT");
    return SNOOP_CB_IGNORE_PKT;
  }

  DATA_IS707_MSG3(MSG_LEGACY_MED, "Got CCP (0x%x) msg %d from device %d",
          protocol,
          (byte) *((*item_head_ptr)->data_ptr),
          device );

  /*-------------------------------------------------------------------------
    Build protocol reject and pushdown at head of c-req (this is passed 
    back as 'Rejected-Information' as per RFC 1661.
  -------------------------------------------------------------------------*/
  p_rej_hdr.code   = PROT_REJ;
  p_rej_hdr.id     = id++;
  p_rej_hdr.length = (uint16)dsm_length_packet(*item_head_ptr) + sizeof(p_rej_hdr);
  p_rej_hdr.length = ps_htons( p_rej_hdr.length );
  p_rej_hdr.rejected_protocol = ps_htons(protocol);

  if (sizeof(p_rej_hdr) != dsm_pushdown_packed( item_head_ptr,
                                                (void *) &p_rej_hdr,
                                                sizeof(p_rej_hdr),
                                                DSM_DS_SMALL_ITEM_POOL ) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No memory to send CCP prot-rej!  silently discard");
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Send the protocol reject out the same interface.  memory will be freed
    by pppi_outgoing, even if error return occurs.  We ignore the return
    value here as the action would be the same - to silently discard 
    (and there are enough error messages elsewhere).
  -------------------------------------------------------------------------*/
  (void) pppi_outgoing( device,
                        PPP_LCP_PROTOCOL,
                        item_head_ptr,
                        NULL );

  return SNOOP_CB_IGNORE_PKT;
} /* ds707_netmdli_callback_ccp_c_req() */

/*===========================================================================
FUNCTION DS707_NETMDLI_CALLBACK_CCP_C_ACK()

DESCRIPTION
  This function is used to resync CCP if LCP gets resynced on Um

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

DEPENDENCIES
  None

SIDE EFFECTS
  Any packet that is passed in it is freed.
===========================================================================*/
static snoop_cb_ret_val_enum_type ds707_netmdli_callback_ccp_c_ack
(
  ppp_dev_enum_type  device,           /* the arrival device               */
  uint16             protocol,         /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(protocol == PPP_CCP_PROTOCOL || protocol == PPP_ILCCP_PROTOCOL);
  ASSERT(device == PPP_RM_DEV || device == PPP_UM_SN_DEV);

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "callback_ccp_c_ack called with NULL item! "
                    "Return SNOOP_CB_IGNORE_PKT");
    return SNOOP_CB_IGNORE_PKT;
  }

  DATA_IS707_MSG3(MSG_LEGACY_MED, "Got CCP (0x%x) ACK (%d) from device %d",
                  protocol,
                  (byte) *((*item_head_ptr)->data_ptr),
                  device);

  ds707_ccp_update_proto = protocol;

  return SNOOP_CB_FORWARD_PKT;
} /* ds707_netmdli_callback_ccp_c_ack() */


/*===========================================================================
FUNCTION   DS707_NETMDLI_PPP_UP_CALLBACK()

DESCRIPTION
  The generic call back for the PPP up state.  Any time that a PPP
  configuration packet is seen this callback is invoked.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item. For this information to be useful it is expected that
  the FF03 has been removed from the front and that any leading 00 s have
  been removed from the protocol ID.

RETURN VALUE
  SNOOP_CB_SNOOP_AGAIN: if packet arrived on Rm or if Um is UMTS
                        (it does need to be looked at again)  
  SNOOP_CB_PROCESS_PKT: if packet arrived on Um (it doesn't need to be 
                        looked at again) but it needs to go up the
                        stack for further processing
  
  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds707_netmdli_ppp_up_callback
(
  ppp_dev_enum_type device,           /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
  ppp_protocol_e_type event_protocol;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -save -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_LCP_PROTOCOL || protocol == PPP_IPCP_PROTOCOL);
  /*lint -restore */
 /*-------------------------------------------------------------------------
   Get event_protocol for posting events
  -------------------------------------------------------------------------*/
  switch( protocol )
  {
    case PPP_LCP_PROTOCOL:
      event_protocol = PPP_PROTOCOL_LCP;
      break;

    case PPP_IPCP_PROTOCOL:
      event_protocol = PPP_PROTOCOL_IPCP;
      break;
    
    default:
      event_protocol = PPP_PROTOCOL_MAX;
      break;
  } /* switch(protocol) */

  DATA_IS707_MSG2(MSG_LEGACY_MED, "0x%x rx but device %d PPP UP: Resync", 
                  protocol, device);

  if( device == PPP_RM_DEV )
  {
    /*-----------------------------------------------------------------------
      Change to the init state and return true eventually indicating that
      snoop_proto_msg_detect() needs to be called again.
    -----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
      the packet was on the Rm, change to the INIT
    -----------------------------------------------------------------------*/
    netmdl_set_meta_state(PPP_INIT_META_STATE);

    ds707_rmsm_post_event(DS707_RMSM_RM_RESYNC_EV);

    /*-----------------------------------------------------------------------
      Generate a PS_PPP_EVENT_RESYNC event.
    -----------------------------------------------------------------------*/
    pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                               device,
                               event_protocol,
                               PS_PPP_EVENT_UNKNOWN, 
                               PS_PPP_EVENT_RESYNC );
    return SNOOP_CB_SNOOP_AGAIN;
  }

  else /* from Um */
  {
    /*-----------------------------------------------------------------------
      the packet was on the Um, change to the resync state
    -----------------------------------------------------------------------*/
    ds707_rmsm_post_event(DS707_RMSM_UM_RESYNC_EV);

    /*-----------------------------------------------------------------------
      Generate a PS_PPP_EVENT_RESYNC event.
    -----------------------------------------------------------------------*/
    pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                               device,
                               event_protocol,
                               PS_PPP_EVENT_UNKNOWN, 
                               PS_PPP_EVENT_RESYNC );
    return SNOOP_CB_PROCESS_PKT;
  }
} /* ds707_netmdli_ppp_up_callback() */


/*===========================================================================
FUNCTION   DS707_NETMDLI_PPP_DOWN_CALLBACK()

DESCRIPTION
  A callback function to handle the receiving TERM_REQ from the RM. If the 
  TERM_REQ packet seen from RM when the physical is not completely up 
  (dormant or coming out of dormancy), the type of cleanup performed is 
  based upon the policy set by the interface controller. If the policy is to 
  always perform the cleanup the TERM request is forwarded onto the UM 
  interface otherwise the call is aborted without cleaning up the network 
  resources.  This is to avoid the call origination if we receive TERM_REQ 
  from the RM interface.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item. For this information to be useful it is expected that
  the FF03 has been removed from the front and that any leading 00 s have
  been removed from the protocol ID.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds707_netmdli_ppp_down_callback
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info   */
)
{
  ps_iface_type *bridge_iface_ptr;         /* Get ptr to Um PS Iface       */
  ppp_dev_enum_type   bridge_dev;          /* figure out Um interface      */
  uint16 offset = 1;                /* offset to use in seek_extract: ID=1 */
  uint16 len = 1;                   /* the length of a PPP ID is one byte  */
  dsm_item_type * t_ack_ptr = NULL; /* buffer for Term-Ack                 */
  ppp_fsm_type * fsm_ptr = NULL;    /* LCP fsm                             */
  uint8 pkt_id;
  ppp_type      *ppp_cb_array = NULL;
  ps_phys_link_type   *phys_link_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( device == PPP_RM_DEV );
  ASSERT( protocol == PPP_LCP_PROTOCOL || protocol == PPP_IPCP_PROTOCOL );
  ppp_cb_array = fsm_get_ppp_cb_array();
  ASSERT(ppp_cb_array[device].bridge_dev < PPP_MAX_DEV);
 
  if( item_head_ptr == NULL || *item_head_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL DSM item passed");
    return SNOOP_CB_IGNORE_PKT;
  }
  DATA_IS707_MSG1(MSG_LEGACY_MED, "Received Term Req, prot: %d", protocol);

  /*-------------------------------------------------------------------------
    Get the iface associated with the corresponding UM PPP device
  -------------------------------------------------------------------------*/

  bridge_dev = ppp_cb_array[device].bridge_dev;
  if( bridge_dev >= PPP_MAX_DEV )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Invalid device : out_device %d", 
                     bridge_dev);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }
  bridge_iface_ptr = 
                ppp_cb_array[bridge_dev].fsm[IPCP].rx_iface_ptr;

  switch( protocol )
  {
    case PPP_LCP_PROTOCOL:
      pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                                 device,
                                 PPP_PROTOCOL_LCP,
                                 PS_PPP_EVENT_UNKNOWN, 
                                 PS_PPP_EVENT_CLOSING );      
      break;

    case PPP_IPCP_PROTOCOL:
      pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                                 device,
                                 PPP_PROTOCOL_IPCP,
                                 PS_PPP_EVENT_UNKNOWN, 
                                 PS_PPP_EVENT_CLOSING );
      break;
    default:
      break;
  }

  /*-------------------------------------------------------------------------
    Take note of id for creating (future) T-Ack
  -------------------------------------------------------------------------*/
  if(dsm_seek_extract(item_head_ptr,
                      &offset,
                      &pkt_id,
                      &len) == FALSE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, "seek_extract() failed - dumping pkt");
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  outstanding_lcp_t_ack_id = pkt_id;
  DATA_IS707_MSG1(MSG_LEGACY_MED,"Cached T-Req id %d", outstanding_lcp_t_ack_id );

  /*-------------------------------------------------------------------------
    Do not cleanup PPP on the network side if
    - Um PPP is already terminated OR
    - Um is dormant and not supposed to be closed gracefully
  -------------------------------------------------------------------------*/
  if ( ( bridge_iface_ptr == NULL ) || 
       ( ( ps_iface_phys_link_state(bridge_iface_ptr) != PHYS_LINK_UP ) && 
         ( PS_IFACE_GET_GRACEFUL_CLOSE(bridge_iface_ptr) == FALSE) ) )
  {
    /*-----------------------------------------------------------------------
      Yes, this is the place where we do not want the call origination.
      Free the data buffer and terminate PPP on RM (rather than posting 
      event to rmsm) which will terminate the data call appropriately for 
      all targets.
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Terminating data session");
    dsm_free_packet( item_head_ptr );  

    /*-----------------------------------------------------------------------
      Return a Term-Ack in response
    -----------------------------------------------------------------------*/
    fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
    if( fsm_send( fsm_ptr, 
                  TERM_ACK, 
                  outstanding_lcp_t_ack_id, 
                  t_ack_ptr ) != 0 )
    {
      /* shouldnt happen */
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Error sending T-Ack!");
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Sent T-Ack id %d on Rm", outstanding_lcp_t_ack_id);
    }

    /*-----------------------------------------------------------------------
      RFC 1661 indicates that PPP must wait for atleast one Restart time
      after sending Term-Ack before cleaning up, but we make an exception
      for netmodel case since netmodel doesn't store PPP fsm state, and
      abort immediately.
    -----------------------------------------------------------------------*/
    (void)ppp_abort( PPP_RM_DEV );
    
    /*---------------------------------------------------------------------
      Update flag to indicate that there is no outstanding T-Ack 
    ---------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Resetting T-Ack status. ");      
    outstanding_lcp_t_ack_id = -1;      
  } 
  else 
  {
    /*-----------------------------------------------------------------------
      Still in a call (ppp bridge is still there) so stop Um PPP
    -----------------------------------------------------------------------*/
    switch( protocol )
    {
      case PPP_LCP_PROTOCOL:
        pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                                   bridge_dev,
                                   PPP_PROTOCOL_LCP,
                                   PS_PPP_EVENT_UNKNOWN, 
                                   PS_PPP_EVENT_CLOSING );
        phys_link_ptr = PS_IFACE_GET_PHYS_LINK(bridge_iface_ptr);
        if ( phys_link_ptr == NULL )
        {
           DATA_IS707_MSG0(MSG_LEGACY_ERROR,"PHYS LINK PTR IS NULL!");
        }
        else
        {
           if ((DS_FLOW_CTRL_IS_BIT_SET(
                ps_phys_link_get_flow_mask(phys_link_ptr),
                DS_FLOW_NO_SERVICE_MASK)) != TRUE)
           {        
             (void)ppp_stop(bridge_dev, PPP_PROTOCOL_LCP);
           }
           else
           {
             (void)ppp_abort(bridge_dev);
           }
        }
        break;

      case PPP_IPCP_PROTOCOL:
        pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                                   bridge_dev,
                                   PPP_PROTOCOL_IPCP,
                                   PS_PPP_EVENT_UNKNOWN, 
                                   PS_PPP_EVENT_CLOSING );        
        phys_link_ptr = PS_IFACE_GET_PHYS_LINK(bridge_iface_ptr);
        if ( phys_link_ptr == NULL )
        {
           DATA_IS707_MSG0(MSG_LEGACY_ERROR,"PHYS LINK PTR IS NULL!");
        }
        else
        {
           if ((DS_FLOW_CTRL_IS_BIT_SET(
                ps_phys_link_get_flow_mask(phys_link_ptr),
                DS_FLOW_NO_SERVICE_MASK)) != TRUE)
           {
             (void)ppp_stop(bridge_dev, PPP_PROTOCOL_IPCP);
           }
           else
           {
             (void)ppp_abort(bridge_dev);
           }
        }
        break;

      default:
        break;
    }

    dsm_free_packet( item_head_ptr );
  }
  return SNOOP_CB_IGNORE_PKT;

} /* ds707_netmdli_ppp_down_callback */


/*===========================================================================
FUNCTION   DS707_NETMDLI_COMPARE_LCP_OPTS()

DESCRIPTION
  This function is used to compare LCP options for the various entities
  involved: Old/new IWF and the TE2/MT2.

  The only options that are being compated are: ACCM, ACFC, PFC.  MRU and
  Magic number do not affect the way data is transfered.
  
PARAMETERS
  None
  
RETURN VALUE
  NO_LCP_CHANGED:     None of the options is different
  MOBILE_LCP_CHANGED: The TE2/MT2 are different
  IWF_LCP_CHANGED:    The old/new IWF values are different

  Note: These are bit fields and so can be composed together to generate a
        return value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_opt_change_enum_type ds707_netmdli_compare_lcp_opts
(
  void
)
{
  ppp_type      *ppp_cb_array = NULL;
  ppp_type      *ppp_rm_cb_ptr;
  ppp_type      *ppp_um_cb_ptr;
  ppp_fsm_type *rm;
  ppp_fsm_type *um;
  ppp_fsm_side_type *te2;      /* pointer to te2 options    */
  ppp_fsm_side_type *mt2;      /* pointer to mt2 options    */
  ppp_fsm_side_type *new_iwf;  /* pointer to new IWF options*/
  ppp_fsm_side_type *old_iwf;  /* pointer to old IWF options*/
  lcp_opt_change_enum_type lcp_opts = NO_LCP_CHANGED;  /* return value     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_cb_array = fsm_get_ppp_cb_array();

  /*-------------------------------------------------------------------------
    Pointers to the lcp FSM structures
  -------------------------------------------------------------------------*/
  ppp_rm_cb_ptr = &ppp_cb_array[PPP_RM_DEV];
  ppp_um_cb_ptr = &ppp_cb_array[PPP_UM_SN_DEV];
  rm = &(ppp_rm_cb_ptr->fsm[LCP]);
  um = &(ppp_um_cb_ptr->fsm[LCP]);

  te2 = &(rm->remote);     /* pointer to te2 options    */
  mt2 = &(um->local);      /* pointer to mt2 options    */
  new_iwf = &(um->remote); /* pointer to new IWF options*/
  old_iwf = &(rm->local);  /* pointer to old IWF options*/


  /*-------------------------------------------------------------------------
    Check ACFC is not disabled at the new IWF when old IWF had it enabled
  -------------------------------------------------------------------------*/
  if((old_iwf->work &  LCP_N_ACFC) && !(new_iwf->work & LCP_N_ACFC))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "Old IWF ACFC %d, new IWF ACFC %d are different",
                    (old_iwf->work & LCP_N_ACFC) == LCP_N_ACFC,
                    (new_iwf->work & LCP_N_ACFC) == LCP_N_ACFC);
    lcp_opts |= IWF_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check PFC is not disabled at the new IWF when old IWF had it enabled
  -------------------------------------------------------------------------*/
  if((old_iwf->work &  LCP_N_PFC) && !(new_iwf->work & LCP_N_PFC))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "Old IWF PFC %d, new IWF PFC %d are different",
                    (old_iwf->work & LCP_N_PFC) == LCP_N_PFC,
                    (new_iwf->work & LCP_N_PFC) == LCP_N_PFC);
    lcp_opts |= IWF_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check ACFC is not disabled at the MT2 when TE2 had it enabled
  -------------------------------------------------------------------------*/
  if((mt2->work & LCP_N_ACFC) && !(te2->work & LCP_N_ACFC))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, "MT2 ACFC %d, TE2 ACFC %d are different",
                    (mt2->work & LCP_N_ACFC) == LCP_N_ACFC,
                    (te2->work & LCP_N_ACFC) == LCP_N_ACFC);
    lcp_opts |= MOBILE_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check PFC is not disabled at the MT2 when TE2 had it enabled
  -------------------------------------------------------------------------*/
  if((mt2->work & LCP_N_PFC) && !(te2->work & LCP_N_PFC))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, "MT2 PFC %d, TE2 PFC %d are different",
                    (mt2->work & LCP_N_PFC) == LCP_N_PFC,
                    (te2->work & LCP_N_PFC) == LCP_N_PFC);
    lcp_opts |= MOBILE_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check ACCM negotiation - if this has changed there is a problem
  -------------------------------------------------------------------------*/
  if((mt2->work & LCP_N_ACCM) != (te2->work & LCP_N_ACCM))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "MT2 ACCM neg %d, TE2 ACCM neg %d are different",
                    (mt2->work & LCP_N_ACCM) == LCP_N_ACCM,
                    (te2->work & LCP_N_ACCM) == LCP_N_ACCM);
    lcp_opts |= MOBILE_LCP_CHANGED;
  }

  if((old_iwf->work & LCP_N_ACCM) != (new_iwf->work & LCP_N_ACCM))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "Old IWF ACCM neg %d, New IWF ACCM neg %d are different",
                    (old_iwf->work & LCP_N_ACCM) == LCP_N_ACCM,
                    (new_iwf->work & LCP_N_ACCM) == LCP_N_ACCM);
    lcp_opts |= IWF_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check the ACCM values
  -------------------------------------------------------------------------*/
  if( ((lcp_value_type*)(old_iwf->work_pdv))->accm !=
      ((lcp_value_type*)(new_iwf->work_pdv))->accm )
  {
    DATA_IS707_MSG4(MSG_LEGACY_MED, 
      "Old IWF ACCM val 0x%x:%x, New IWF ACCM val 0x%x:%x are different",
      (uint16)((((lcp_value_type*)(old_iwf->work_pdv))->accm) >> 16),
      (uint16)((((lcp_value_type*)(old_iwf->work_pdv))->accm) & 0x0000FFFF),
      (uint16)((((lcp_value_type*)(new_iwf->work_pdv))->accm) >> 16),
      (uint16)((((lcp_value_type*)(new_iwf->work_pdv))->accm) & 0x0000FFFF));
    lcp_opts |= IWF_LCP_CHANGED;
  }

  if( ((lcp_value_type*)(mt2->work_pdv))->accm !=
      ((lcp_value_type*)(te2->work_pdv))->accm )
  {
    DATA_IS707_MSG4(MSG_LEGACY_LOW, 
            "MT2 ACCM val 0x%x:%x, TE2 ACCM val 0x%x:%x are different",
            (uint16)((((lcp_value_type*)(mt2->work_pdv))->accm) >> 16),
            (uint16)((((lcp_value_type*)(mt2->work_pdv))->accm) &0x0000FFFF),
            (uint16)((((lcp_value_type*)(te2->work_pdv))->accm) >> 16),
            (uint16)((((lcp_value_type*)(te2->work_pdv))->accm) &0x0000FFFF));
    lcp_opts |= MOBILE_LCP_CHANGED;
  }

  return lcp_opts;
} /* ds707_netmdli_compare_lcp_opts() */


/*===========================================================================
FUNCTION   DS707_NETMDL_COMPARE_LCP_OPTS()

DESCRIPTION
  This function is used to compare LCP options for the various entities
  involved (old/new IWF and the TE2/MT2), and sets the network model mode
  on the Um/Rm device to pseudo network if no LCP options on that particular
  interface have been changed.
  
  The only options that are being compated are: ACCM, ACFC, PFC.  MRU and
  Magic number do not affect the way data is transfered.
  
PARAMETERS
  None

RETURN VALUE
    TRUE - Rm and Um LCP options match
    FALSE - Rm and Um LCP options differ    

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_netmdl_compare_lcp_opts
(
  void
)
{
  lcp_opt_change_enum_type lcp_opts;            /* LCP option change value */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  lcp_opts = ds707_netmdli_compare_lcp_opts();

  if (lcp_opts == NO_LCP_CHANGED)
  {
    return TRUE;
  }

  return FALSE;
} /* ds707_netmdl_compare_lcp_opts() */


/*===========================================================================
FUNCTION DS707_NETMDL_COMPARE_IPCP_OPTS()

DESCRIPTION
  This function is used to compare IPCP options for the various entities
  involved: Old/new IWF and the TE2/MT2.

PARAMETERS
  None
  
RETURN VALUE
  TRUE - Rm and Um IPCP options match
  FALSE - Rm and Um IPCP options differ

DEPENDENCIES
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_netmdl_compare_ipcp_opts
(
  void
)
{
  ppp_type      *ppp_cb_array = NULL;
  ppp_type      *um_ppp_ptr;
  ppp_type      *rm_ppp_ptr;

  negotiate_t mt2_ipcp_work;
  negotiate_t te2_ipcp_work;
  negotiate_t mt2_ipcp_want;
  negotiate_t te2_ipcp_want;
  negotiate_t new_iwf_ipcp_work;
  negotiate_t orig_iwf_ipcp_work;

  ipcp_value_type *mt2_ipcp_opts;
  ipcp_value_type *te2_ipcp_opts;
  ipcp_value_type *new_iwf_ipcp_opts;
  ipcp_value_type *orig_iwf_ipcp_opts;

  boolean ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_cb_array = fsm_get_ppp_cb_array();

  um_ppp_ptr = ppp_cb_array + PPP_UM_SN_DEV;
  rm_ppp_ptr = ppp_cb_array + PPP_RM_DEV;
  
  /*-------------------------------------------------------------------------
    Copies of the work values for the various entities (mt2, te2, etc...)
  -------------------------------------------------------------------------*/
  mt2_ipcp_work = um_ppp_ptr->fsm[IPCP].local.work;
  te2_ipcp_work = rm_ppp_ptr->fsm[IPCP].remote.work;
  mt2_ipcp_want = um_ppp_ptr->fsm[IPCP].local.want;
  te2_ipcp_want = rm_ppp_ptr->fsm[IPCP].remote.want;
  new_iwf_ipcp_work = um_ppp_ptr->fsm[IPCP].remote.work;
  orig_iwf_ipcp_work = rm_ppp_ptr->fsm[IPCP].local.work;

  /*-------------------------------------------------------------------------
    Pointers to the work_pdvs for the various entities (mt2, te2, etc...)
  -------------------------------------------------------------------------*/
  mt2_ipcp_opts = 
    (ipcp_value_type*)um_ppp_ptr->fsm[IPCP].local.work_pdv;
  te2_ipcp_opts =
    (ipcp_value_type*)rm_ppp_ptr->fsm[IPCP].remote.work_pdv;
  new_iwf_ipcp_opts =
    (ipcp_value_type*)um_ppp_ptr->fsm[IPCP].remote.work_pdv;
  orig_iwf_ipcp_opts =
    (ipcp_value_type*)rm_ppp_ptr->fsm[IPCP].local.work_pdv;


  /*-----------------------------------------------------------------------
    Check IPCP options.  If any of the following questions can be answered
    true then an end-to-end resync must occur.

    Has the TE2's IP address changed? (make sure it was negotiatied too)
  -----------------------------------------------------------------------*/
  if(((mt2_ipcp_want & IPCP_N_1ADDRESS) ==
      (te2_ipcp_want & IPCP_N_1ADDRESS))     &&
     /*--------------------------------------------------------------------
       Both sides wanted to negotiate an IP address for the mobile - did
       they negotiate for it? did the mobiles address change? 
     --------------------------------------------------------------------*/
      ((mt2_ipcp_work & IPCP_N_1ADDRESS) &&
       (te2_ipcp_work & IPCP_N_1ADDRESS) &&
       (mt2_ipcp_opts->address.ps_s_addr != te2_ipcp_opts->address.ps_s_addr)))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, "New TE2 address 0x%x:%x",
            (uint16)(mt2_ipcp_opts->address.ps_s_addr >> 16),
            (uint16)(mt2_ipcp_opts->address.ps_s_addr & 0x0000ffff));
    DATA_IS707_MSG2(MSG_LEGACY_LOW, "Old TE2 address 0x%x:%x",
            (uint16)(te2_ipcp_opts->address.ps_s_addr >> 16),
            (uint16)(te2_ipcp_opts->address.ps_s_addr & 0x0000ffff));
    ret_val = FALSE;
  }

  /*-----------------------------------------------------------------------
    Has TE2 compression been enabled when it previously wasn't?
  -----------------------------------------------------------------------*/
  else if((mt2_ipcp_work & IPCP_N_COMPRESS) &&
          !(te2_ipcp_work & IPCP_N_COMPRESS))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "TE2 %d, MT2 %d. Mobile VJ comp on when was off!",
                    (te2_ipcp_work & IPCP_N_COMPRESS),
                    (mt2_ipcp_work & IPCP_N_COMPRESS));
    ret_val = FALSE;
  }

  /*-----------------------------------------------------------------------
    Has IWF compression been disabled when it was previously enabled?
  -----------------------------------------------------------------------*/
  else if((orig_iwf_ipcp_work & IPCP_N_COMPRESS) &&
          !(new_iwf_ipcp_work & IPCP_N_COMPRESS))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                 "Original IWF %d, new IWF %d. IWF VJ comp off when was on!",
                 (orig_iwf_ipcp_work & IPCP_N_COMPRESS),
                 (new_iwf_ipcp_work & IPCP_N_COMPRESS));
    ret_val = FALSE;
  }

  /*-----------------------------------------------------------------------
    if IWF compression has always been on check IWF compression options
  -----------------------------------------------------------------------*/
  else if((orig_iwf_ipcp_work & IPCP_N_COMPRESS) &&
          (new_iwf_ipcp_work & IPCP_N_COMPRESS))
  {
    /*---------------------------------------------------------------------
      Are the old and new IWF's using the same compression protocol?
    ---------------------------------------------------------------------*/
    if(orig_iwf_ipcp_opts->compression != new_iwf_ipcp_opts->compression)
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED, 
                      "New IWF compr %x, Old IWF compr %d changed",
                      new_iwf_ipcp_opts->compression,
                      orig_iwf_ipcp_opts->compression);
      ret_val = FALSE;
    }

    /*---------------------------------------------------------------------
      Does the new IWF have less recieve slots than the old IWF?
    ---------------------------------------------------------------------*/
    else if(new_iwf_ipcp_opts->vj_config.num_slots < 
            orig_iwf_ipcp_opts->vj_config.num_slots)
    {
      DATA_IS707_MSG2(MSG_LEGACY_LOW, "New IWF slots %d, Old IWF slots %d. "
                      "More VJ slots on old IWF than new IWF!",
                      new_iwf_ipcp_opts->vj_config.num_slots,
                      orig_iwf_ipcp_opts->vj_config.num_slots);
      ret_val = FALSE;
    }

    /*--------------------------------------------------------------------
      Has IWF slot compression been disabled when it was previously on?
    ---------------------------------------------------------------------*/
    else if(new_iwf_ipcp_opts->vj_config.slot_comp == 0 &&
            orig_iwf_ipcp_opts->vj_config.slot_comp != 0)
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED, 
                      "New IWF slot comp %d, Old IWF slot comp %d. "
                      "IWF slot comp NOW off when was on!",
                      new_iwf_ipcp_opts->vj_config.slot_comp,
                      orig_iwf_ipcp_opts->vj_config.slot_comp);
      ret_val = FALSE;
    }
  } /* if(IWF compression has always been on) */

  /*-----------------------------------------------------------------------
    Has TE2 and MT2 compression been on and has slot compression has been
    enabled when it was previously disabled?
  -----------------------------------------------------------------------*/
  else if((te2_ipcp_work & IPCP_N_COMPRESS) &&
          (mt2_ipcp_work & IPCP_N_COMPRESS))
  {
    /*---------------------------------------------------------------------
      Are the MT2 and TE2 using the same compression protocol?
    ---------------------------------------------------------------------*/
    if(te2_ipcp_opts->compression != mt2_ipcp_opts->compression)
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED, "MT2 compr %x, TE2 compr %d changed",
                      mt2_ipcp_opts->compression,
                      te2_ipcp_opts->compression);
      ret_val = FALSE;
    }

    /*---------------------------------------------------------------------
      Has number of MT2 slots increased?
    ---------------------------------------------------------------------*/
    else if(te2_ipcp_opts->vj_config.num_slots < 
            mt2_ipcp_opts->vj_config.num_slots)
    {
      DATA_IS707_MSG2(MSG_LEGACY_LOW, "New TE2 slots %d, Old TE2 slots %d"
                      "Network transmitting more VJ slots than TE can handle!",
                      mt2_ipcp_opts->vj_config.num_slots,
                      te2_ipcp_opts->vj_config.num_slots);
      ret_val = FALSE;
    }

    /*--------------------------------------------------------------------
      Has MT2 slot compression been enabled when it previously wasn't?
    ---------------------------------------------------------------------*/
    else if(te2_ipcp_opts->vj_config.slot_comp == 0 &&
            mt2_ipcp_opts->vj_config.slot_comp != 0)
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED, 
                      "TE2 slot comp now ON (%d), was OFF (%d)!",
                      mt2_ipcp_opts->vj_config.slot_comp,
                      te2_ipcp_opts->vj_config.slot_comp);
      ret_val = FALSE;
    }
  } /* if(MT2/TE2 compression has always been on) */

  return ret_val;
} /* ds707_netmdl_compare_ipcp_opts() */


/*===========================================================================
  FUNCTION DS707_NETMDL_SET_RESYNC_BEHAVIOUR

  DESCRIPTION
    Accessor function to force resync failure to drop call

  PARAMETERS
    DS707_NETMDL_RESYNC_DLFT      - do the default behaviour
    DS707_NETMDL_RESYNC_DROP_CALL - force call to drop on resync failure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_netmdl_set_resync_behaviour
(
  netmdl_drop_behav_enum_type behav
)
{
  if (behav == NETMDL_RESYNC_DFLT )
  {
#ifdef FEATURE_DS_NET_MODEL_DROPS_CALL
    ds707_netmdl_drop_call = TRUE;
#else
    ds707_netmdl_drop_call = FALSE;
#endif /* FEATURE_DS_NET_MODEL_DROPS_CALL */
  } 
  else if (behav == NETMDL_RESYNC_DROP_CALL )
  {
    ds707_netmdl_drop_call = TRUE;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "You called with bad param %d", behav);
  }
}


/*===========================================================================
FUNCTION   DS707_NETMDLI_LCP_RESYNCED()

DESCRIPTION
  This function is called when LCP has been established in a situation where
  CCP needs to be resynced.  It injects a spoofed CCP C-REQ with an invalid
  protocol on the Rm interface to cause an end-to-end CCP renegotiation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
static boolean ds707_netmdli_lcp_resynced
(
  ppp_dev_enum_type device
)
{
  dsm_item_type            *item_ptr = NULL;
  ppp_config_hdr_type       creq_hdr;         /* header for spoofed C_REQ  */
  ppp_opt_hdr_type          opt_hdr;          /* PPP option header         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device != PPP_UM_SN_DEV)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"PPP device is not PPP_UM_SN_DEV," 
                     "device: %d", device);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Build a config request with an invalid option i.e. the reserved value of
    255
  -------------------------------------------------------------------------*/
  creq_hdr.code       = CONFIG_REQ;
  creq_hdr.identifier = 1;
  creq_hdr.length     = sizeof(creq_hdr) + sizeof(opt_hdr);
  creq_hdr.length     = ps_htons( creq_hdr.length );
  opt_hdr.type        = 255;
  opt_hdr.length      = sizeof(opt_hdr);

  /*-------------------------------------------------------------------------
    Pushdown the options first.  dsm_pushdown_packed() allocates the
    dsm item
  -------------------------------------------------------------------------*/
  if (sizeof(opt_hdr) != dsm_pushdown_packed( &item_ptr,
                                              (void *) &opt_hdr,
                                              sizeof(opt_hdr),
                                              DSM_DS_SMALL_ITEM_POOL ) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
        "dsm_pushdown_packed for opt_hdr in generating CCP config-req failed");
    dsm_free_packet(&item_ptr);
    return FALSE;
  }

  if (sizeof(creq_hdr) != dsm_pushdown_packed( &item_ptr,
                                               (void *) &creq_hdr,
                                               sizeof(creq_hdr),
                                               DSM_DS_SMALL_ITEM_POOL ) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
        "dsm_pushdown_packed for CCP hdr in generating CCP config-req failed");
    dsm_free_packet(&item_ptr);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Send the config request out.  Memory will be freed by pppi_outgoing,
    even if error return occurs.  We ignore the return value here as the
    action would be the same - to silently discard
  -------------------------------------------------------------------------*/
  (void) pppi_outgoing( PPP_RM_DEV,
                        ds707_ccp_update_proto,
                        &item_ptr,
                        NULL );

  ds707_ccp_update_proto = 0;
  lcp_set_net_model_resync_fnc( NULL, PPP_UM_SN_DEV );

  return TRUE;
} /* ds707_netmdli_lcp_resynced() */


/*===========================================================================
FUNCTION   DS707_NETMDLI_PPP_RESYNCED()

DESCRIPTION
  This function is called once PPP has been established.  It checks the
  various IPCP and LCP options to determine whether or not an end-to-end
  resync will be required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean ds707_netmdli_ppp_resynced
(
  ppp_dev_enum_type device
)
{
  dsm_item_type *           creq_spoof_ptr;   /* dsm ptr for C-REQ spoof   */
  ppp_config_hdr_type       header;           /* header for spoofing       */
  uint16                    protocol;         /* protocol for spoofing     */
  lcp_opt_change_enum_type  lcp_opts;         /* LCP options change value  */
  boolean                   take_action;      /* do we need to resync?     */  
  boolean                   e2e_resync_failed;/* end to end resync result  */
  ppp_type *                ppp_cb_array = NULL;
  ps_iface_type            *iface_ptr = NULL;
  ps_phys_link_type        *phys_link_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device != PPP_UM_SN_DEV)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"PPP device is not PPP_UM_SN_DEV," 
                     "device: %d", device);
    ASSERT(0);
    return FALSE;
  }

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "PPP dev %d now resynced!", device);

  take_action = FALSE;
  protocol = PPP_LCP_PROTOCOL;
  e2e_resync_failed = FALSE;
  ppp_cb_array = fsm_get_ppp_cb_array();
  /*-------------------------------------------------------------------------
    deregister this callback function
  -------------------------------------------------------------------------*/
  ipcp_set_net_model_resync_fnc( NULL, device );

  /*-------------------------------------------------------------------------
    Check if the LCP options are the same for mobile and IWF
  -------------------------------------------------------------------------*/
  lcp_opts = ds707_netmdli_compare_lcp_opts();
  
  /*-------------------------------------------------------------------------
    If LCP options are unchanged on the mobile or IWF, set that interface
    to pseudo network model mode.
  -------------------------------------------------------------------------*/
  if(lcp_opts & MOBILE_LCP_CHANGED)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "TE2 options changed Um in FN mode");
    take_action = TRUE;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "TE2 options unchanged Um in PN mode");
    ppp_set_mode(PPP_UM_SN_DEV, PPP_INTERNAL_SNOOP_MODE);
  }

  if(lcp_opts & IWF_LCP_CHANGED)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "IWF options changed Um in FN mode");
    take_action = TRUE;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "IWF options unchanged Rm in PN mode");
    /*-----------------------------------------------------------------------
      When the Um (Uu) interface has HDLC disabled, then we run the R
      interface in FULL NETWORK model.
    -----------------------------------------------------------------------*/
    if(ppp_cb_array[PPP_UM_SN_DEV].hdlc_mode == HDLC_OFF)
    {
      ppp_set_mode(PPP_RM_DEV, PPP_FULL_NET_MODE);
    }
    else
    {
      ppp_set_mode(PPP_RM_DEV, PPP_INTERNAL_SNOOP_MODE);
    }
  }

  /*-------------------------------------------------------------------------
    Check if the IPCP options are the same for mobile and IWF
  -------------------------------------------------------------------------*/
  if (ds707_netmdl_compare_ipcp_opts() == FALSE)
  {
    /* Um and Rm stay in PN in this case - should probably fix this */
    take_action = TRUE;
  }

  /*-------------------------------------------------------------------------
    Regardless of whether options match or not, enable the RM flow
  -------------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow(DS_FLOW_NETMODEL_MASK, DS_FLOW_ENABLE);

  if(take_action == TRUE)
  {
    if (!ds707_netmdl_drop_call)
    {
      /*---------------------------------------------------------------------
        force an end-to-end resync do so by spoofing an empty PPP packet.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "PPP Options changed, resync Rm");

      creq_spoof_ptr = dsm_offset_new_buffer( DSM_DS_LARGE_ITEM_POOL, 
         				      PPP_HDR_LEN );
      if(creq_spoof_ptr != NULL)
      {
        ppp_set_mode(PPP_RM_DEV, PPP_PSEUDO_NET_MODE);
        ppp_set_mode(PPP_UM_SN_DEV, PPP_PSEUDO_NET_MODE);

        header.code = CONFIG_REQ;
        header.identifier = ds707_curr_ppp_id[PPP_RM_DEV] - 1;
        /*lint -save -e572
         * for some reason the ntohs() macro is making lint complain about an
         * excessive shift?  Tried all sorts of things to fix it, nothing
         * worked, so turning off that error.
         */
        header.length = ps_ntohs(CONFIG_HDR_LEN);
        /*lint -restore */
        memscpy( creq_spoof_ptr->data_ptr, sizeof(header), 
                 &header, sizeof(header));
        creq_spoof_ptr->used += CONFIG_HDR_LEN;
  
        /*-------------------------------------------------------------------
          Put meta state machine into initialization state - we are 
          resyncing and shove the packet out to the TE2.
        -------------------------------------------------------------------*/
        ds707_rmsm_post_event(DS707_RMSM_RM_RESYNC_EV);

        DSM_TOUCH_ITEM(creq_spoof_ptr);
        (void)pppi_outgoing(PPP_RM_DEV, protocol, &creq_spoof_ptr, NULL);
        
        /*-------------------------------------------------------------------
          Returning TRUE here would cause a problem for MIP calls, 
          but ds707_netmdl_drop_call is always set to TRUE for MIP.
        -------------------------------------------------------------------*/
        return TRUE;

      } /* if(mem alloc worked) */
      else
      {
        /*-------------------------------------------------------------------
          Couldn't allocate DSM item to spoof C-REQ (can't force end to end
          resync) - take note.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "C-Req item allocation failed, not spoofing");
        e2e_resync_failed = TRUE;
      }
    }

    if (ds707_netmdl_drop_call || e2e_resync_failed)
    {
      /*---------------------------------------------------------------------
        Terminate the data session

        Changed behaviour here - will tear down call instead 
        of remaining up with incorrect framing
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "PPP Options changed stopping PPP!");
      /*-------------------------------------------------------------------
        In MIP calls, tell RMSM to bring down Um interface
        In Simple IP calls , this tears down TC without sending TERM-REQ
        except for KDDI, because as per KDDI spec LCP-TERM-REQ needs 
        to be sent.
      -------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Drop call since Um resync failed");

      if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
      {       
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, " Bringing down PPP! ");
        iface_ptr = ppp_cb_array[PPP_UM_SN_DEV].fsm[IPCP].rx_iface_ptr;
        phys_link_ptr = PS_IFACE_GET_PHYS_LINK(iface_ptr);
        if ( phys_link_ptr == NULL )
        {
           DATA_IS707_MSG0(MSG_LEGACY_ERROR,"PHYS LINK PTR IS NULL!");
        }
        else
        {
          if ((DS_FLOW_CTRL_IS_BIT_SET(
               ps_phys_link_get_flow_mask(phys_link_ptr),
               DS_FLOW_NO_SERVICE_MASK)) != TRUE)
          {
            (void) ppp_stop(PPP_UM_SN_DEV, PPP_PROTOCOL_LCP);
          }
          else
          {
            (void)ppp_abort(PPP_UM_SN_DEV);
          }
        }
        return (!take_action);
      }
      
      ds707_rmsm_post_event (DS707_RMSM_UM_PPP_DOWN_EV);
    }
  } /* if(had to resync) */

  /*-------------------------------------------------------------------------
    PPP devices are all in the right mode at this point so  PPP state
    is UP, Inform RMSM
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "PPPP UP: Inform RMSM");
  ds707_rmsm_post_event (DS707_RMSM_E2E_PPP_UP_EV);

  return (!take_action);
} /* ds707_netmdli_ppp_resynced() */

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

                    EXTERNAL FUNCTION DEFINTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION   DS707_NETMDL_REPORT_PPP_CLOSE_DIAG_EVENTS()

DESCRIPTION
  This function posts diag events corresponding to PPP close.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds707_netmdl_report_ppp_close_diag_events
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------
    Post closed for Um LCP
   ------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                             PPP_UM_SN_DEV,
                             PPP_PROTOCOL_LCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSED );
  /*-----------------------------------------------------------------
    Post closed for Um IPCP
  ------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                             PPP_UM_SN_DEV,
                             PPP_PROTOCOL_IPCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSED );
  /*-----------------------------------------------------------------
    Post closed for Rm LCP
  ------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                             PPP_RM_DEV,
                             PPP_PROTOCOL_LCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSED );
  /*-----------------------------------------------------------------
    Post closed for Rm IPCP
  ------------------------------------------------------------------*/

  pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                             PPP_RM_DEV,
                             PPP_PROTOCOL_IPCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSED );

} /* ds707_netmdl_report_ppp_close_diag_events() */

/*===========================================================================
FUNCTION   DS707_NETMDL_RESET_RESYNC_CALLBACKS()

DESCRIPTION
  This function does the processing required on end of PPP call, like
  enalbing flows, resetting PPP callbacks etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdl_reset_resync_callbacks
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set the state variable for this interface
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, "Netmdl: PPP OOC");

  /*-------------------------------------------------------------------------
    Deregister all current callbacks 
  -------------------------------------------------------------------------*/
  netmdl_snoop_unreg_all();

  /*-----------------------------------------------------------------------
     Post PPP CLOSED events to Diag, if call is going NULL.
  -----------------------------------------------------------------------*/
  if(ds707_rmsm_is_null())
  {
    /*---------------------------------------------------------------------
      Post closed for Um LCP
    ---------------------------------------------------------------------*/
    pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                               PPP_UM_SN_DEV,
                               PPP_PROTOCOL_LCP,
                               PS_PPP_EVENT_UNKNOWN,
                               PS_PPP_EVENT_CLOSED );
    /*---------------------------------------------------------------------
      Post closed for Um IPCP
    ---------------------------------------------------------------------*/
    pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                               PPP_UM_SN_DEV,
                               PPP_PROTOCOL_IPCP,
                               PS_PPP_EVENT_UNKNOWN,
                               PS_PPP_EVENT_CLOSED );
    /*---------------------------------------------------------------------
      Post closed for Rm LCP
    ---------------------------------------------------------------------*/
    pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                               PPP_RM_DEV,
                               PPP_PROTOCOL_LCP,
                               PS_PPP_EVENT_UNKNOWN,
                               PS_PPP_EVENT_CLOSED );
    /*---------------------------------------------------------------------
      Post closed for Rm IPCP
    ---------------------------------------------------------------------*/
    pppi_report_event_to_diag( EVENT_PPP_NETMODEL,
                               PPP_RM_DEV,
                               PPP_PROTOCOL_IPCP,
                               PS_PPP_EVENT_UNKNOWN,
                               PS_PPP_EVENT_CLOSED );
  }

  /*-----------------------------------------------------------------------
    If PPP resync failed, flow on RM would have never got enabled since 
    netmodel never get notified.  Hence enable flow during cleanup.
  -----------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow(DS_FLOW_NETMODEL_MASK, DS_FLOW_ENABLE);

  /*-----------------------------------------------------------------------
    clear PPP complete callback function in case PPP resync fails and the
    callback is never cleared in the callback itself.
    Similarly clear lcp complete callback
  -----------------------------------------------------------------------*/
  ipcp_set_net_model_resync_fnc( NULL, PPP_UM_SN_DEV );
  lcp_set_net_model_resync_fnc( NULL, PPP_UM_SN_DEV );

}/* ds707_netmdl_reset_resync_callbacks() */

/*===========================================================================
FUNCTION   DS707_NETMDL_REG_PPP_INIT_CALLBACKS()

DESCRIPTION
  This function does the processing required during init of PPP setup for
  netmdl callbacks like registering lcp, ipcp callbacks etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdl_reg_ppp_init_callbacks
(
  void
)
{
  ppp_type      *ppp_cb_array = NULL;
  ppp_type      *rm_ppp_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  rm_ppp_ptr = ppp_cb_array + PPP_RM_DEV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set the state variable for this interface
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, "Netmdl: PPP Init");

  /*-------------------------------------------------------------------------
    Deregister all current callbacks
  -------------------------------------------------------------------------*/
  netmdl_snoop_unreg_all();

  /*-----------------------------------------------------------------------
    Register all of Init state callbacks.
  -----------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
    LCP C-Req on Um and Rm
  -----------------------------------------------------------------------*/
  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_LCP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_init_callback_lcp_c_req);
  netmdl_snoop_reg(PPP_UM_SN_DEV,
                   PPP_LCP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_init_callback_lcp_c_req);
  /*-----------------------------------------------------------------------
    IPCP C-Req on Um and Rm
  -----------------------------------------------------------------------*/
  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_IPCP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_init_callback_ipcp_c_req);
  netmdl_snoop_reg(PPP_UM_SN_DEV,
                   PPP_IPCP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_init_callback_ipcp_c_req);
  /*-----------------------------------------------------------------------
    LCP C-Ack on Um and Rm
  -----------------------------------------------------------------------*/
  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_LCP_PROTOCOL,
                   CONFIG_ACK,
                   ds707_netmdli_init_callback_lcp_c_ack);
  netmdl_snoop_reg(PPP_UM_SN_DEV,
                   PPP_LCP_PROTOCOL,
                   CONFIG_ACK,
                   ds707_netmdli_init_callback_lcp_c_ack);
  /*-----------------------------------------------------------------------
    IPCP C-Ack on Um and Rm
  -----------------------------------------------------------------------*/
  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_IPCP_PROTOCOL,
                   CONFIG_ACK,
                   ds707_netmdli_init_callback_ipcp_c_ack);
  netmdl_snoop_reg(PPP_UM_SN_DEV,
                   PPP_IPCP_PROTOCOL,
                   CONFIG_ACK,
                   ds707_netmdli_init_callback_ipcp_c_ack);

  /*-----------------------------------------------------------------------
    PAP config request on Rm only
  -----------------------------------------------------------------------*/
  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_PAP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_init_callback_pap_a_req);
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    /*-----------------------------------------------------------------------
      CHAP config request on Rm only
    -----------------------------------------------------------------------*/
    netmdl_snoop_reg(PPP_RM_DEV,
                     PPP_CHAP_PROTOCOL,
                     PPP_CHAP_RESPONSE,
                     ds707_netmdli_init_callback_chap_resp);
  }

  if ( TRUE == dsgen_iface_hdlr_use_internal_auth() ||
       uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_RUIM || 
       uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM ||
       NVRUIM_NON_3GPD_CARD == nvruim_check_non_3gpd_cdma_card() )
  {
    /*-----------------------------------------------------------------------
      in the case of DMU the challenge and response callbacks should be used
      as they will guarantee that chap runs on the handset.
    -----------------------------------------------------------------------*/
    netmdl_snoop_reg( PPP_UM_SN_DEV,
                      PPP_CHAP_PROTOCOL,
                      PPP_CHAP_CHALLENGE,
                      ds707_netmdli_um_chap_cback );

    netmdl_snoop_reg( PPP_RM_DEV,
                      PPP_CHAP_PROTOCOL,
                      PPP_CHAP_RESPONSE,
                      ds707_netmdli_rm_chap_cback );

    netmdl_snoop_reg( PPP_RM_DEV,
                      PPP_PAP_PROTOCOL,
                      PPP_PAP_REQ,
                      ds707_netmdli_rm_pap_cback );
  }

  /*-----------------------------------------------------------------------
    CCP (single and multilink/individual link CCP) config request on all
    interfaces.  The ccp_config value is the same on all PPP devices
    For CCP pass through set the callback on a config ack sent by the
    laptop only.  This is because the laptop sending compressed packets
    is the only direction that matters on a resync.
  -----------------------------------------------------------------------*/
  if(PPP_CCP_REJECT == rm_ppp_ptr->ccp_config )
  {
    netmdl_snoop_reg(PPP_RM_DEV,
                      PPP_CCP_PROTOCOL,
                      SNOOP_ANY_MSG,
                      ds707_netmdli_callback_ccp_c_req);
    netmdl_snoop_reg(PPP_UM_SN_DEV,
                      PPP_CCP_PROTOCOL,
                      SNOOP_ANY_MSG,
                      ds707_netmdli_callback_ccp_c_req);
    netmdl_snoop_reg(PPP_RM_DEV,
                      PPP_ILCCP_PROTOCOL,
                      SNOOP_ANY_MSG,
                      ds707_netmdli_callback_ccp_c_req);
    netmdl_snoop_reg(PPP_UM_SN_DEV,
                      PPP_ILCCP_PROTOCOL,
                      SNOOP_ANY_MSG,
                      ds707_netmdli_callback_ccp_c_req);
  }
  else
  {
    netmdl_snoop_reg(PPP_RM_DEV,
                      PPP_CCP_PROTOCOL,
                      CONFIG_ACK,
                      ds707_netmdli_callback_ccp_c_ack);
    netmdl_snoop_reg(PPP_RM_DEV,
                      PPP_ILCCP_PROTOCOL,
                      CONFIG_ACK,
                      ds707_netmdli_callback_ccp_c_ack);
  }

  /*-----------------------------------------------------------------------
    Reset variables -  ds707_lcp_c_ack_status, ds707_ipcp_c_ack_status, 
    ds707_ccp_update_proto and outstanding lcp_t_ack
  -----------------------------------------------------------------------*/
  ds707_lcp_c_ack_status  = NO_ACK_RCVD;
  ds707_ipcp_c_ack_status = NO_ACK_RCVD;
  ds707_ccp_update_proto  = 0;
  DATA_IS707_MSG0(MSG_LEGACY_MED,"Resetting T-Ack status. ");      
  outstanding_lcp_t_ack_id = -1;
  return;

} /* ds707_netmdl_reg_ppp_init_callbacks() */

/*===========================================================================
FUNCTION   DS707_NETMDL_REG_PPP_UP_CALLBACKS()

DESCRIPTION
  This function does the processing required after PPP is UP, like
  resync, lcp callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdl_reg_ppp_up_callbacks
(
  void
)
{
  ppp_type *ppp_cb_array = NULL;
  ppp_type *um_ppp_ptr;
  ppp_type *rm_ppp_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  um_ppp_ptr = ppp_cb_array + PPP_UM_SN_DEV;
  rm_ppp_ptr = ppp_cb_array + PPP_RM_DEV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set the state variable for this interface
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, "Netmdl: PPP UP");

  /*-------------------------------------------------------------------------
    Deregister all current callbacks
  -------------------------------------------------------------------------*/
  netmdl_snoop_unreg_all();

  /*-----------------------------------------------------------------------
    Change to the PPP Up state.  Register all relevant callbacks.
  -----------------------------------------------------------------------*/
  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_LCP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_ppp_up_callback);
  netmdl_snoop_reg(PPP_UM_SN_DEV,
                   PPP_LCP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_ppp_up_callback);

  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_LCP_PROTOCOL,
                   TERM_REQ,
                   ds707_netmdli_ppp_down_callback);

  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_IPCP_PROTOCOL,
                   TERM_REQ,
                   ds707_netmdli_ppp_down_callback);

  netmdl_snoop_reg(PPP_RM_DEV,
                   PPP_IPCP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_ppp_up_callback);
  netmdl_snoop_reg(PPP_UM_SN_DEV,
                   PPP_IPCP_PROTOCOL,
                   CONFIG_REQ,
                   ds707_netmdli_ppp_up_callback);
  if ( TRUE == dsgen_iface_hdlr_use_internal_auth() ||
       uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_RUIM || 
       uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM ||
       NVRUIM_NON_3GPD_CARD == nvruim_check_non_3gpd_cdma_card() )
  {
    /*-----------------------------------------------------------------------
      in the case of DMU the challenge and response callbacks should be used
      as they will guarantee that chap runs on the handset.
    -----------------------------------------------------------------------*/
    netmdl_snoop_reg( PPP_UM_SN_DEV,
                      PPP_CHAP_PROTOCOL,
                      PPP_CHAP_CHALLENGE,
                      ds707_netmdli_um_chap_cback );

    netmdl_snoop_reg( PPP_RM_DEV,
                      PPP_CHAP_PROTOCOL,
                      PPP_CHAP_RESPONSE,
                      ds707_netmdli_rm_chap_cback );

    netmdl_snoop_reg( PPP_RM_DEV,
                      PPP_PAP_PROTOCOL,
                      PPP_PAP_REQ,
                      ds707_netmdli_rm_pap_cback );
  }
  /*-----------------------------------------------------------------------
    CCP (single and multilink/individual link CCP) config request on all
    interfaces.  The ccp_config value is the same on all PPP devices
    For CCP pass through set the callback on a config ack sent by the
    laptop only.  This is because the laptop sending compressed packets
    is the only direction that matters on a resync.
  -----------------------------------------------------------------------*/
  if(PPP_CCP_REJECT == rm_ppp_ptr->ccp_config )
  {
    netmdl_snoop_reg(PPP_RM_DEV,
                     PPP_CCP_PROTOCOL,
                     SNOOP_ANY_MSG,
                     ds707_netmdli_callback_ccp_c_req);
    netmdl_snoop_reg(PPP_UM_SN_DEV,
                     PPP_CCP_PROTOCOL,
                     SNOOP_ANY_MSG,
                     ds707_netmdli_callback_ccp_c_req);
    netmdl_snoop_reg(PPP_RM_DEV,
                     PPP_ILCCP_PROTOCOL,
                     SNOOP_ANY_MSG,
                     ds707_netmdli_callback_ccp_c_req);
    netmdl_snoop_reg(PPP_UM_SN_DEV,
                     PPP_ILCCP_PROTOCOL,
                     SNOOP_ANY_MSG,
                     ds707_netmdli_callback_ccp_c_req);
  }
  else
  {
    netmdl_snoop_reg(PPP_RM_DEV,
                     PPP_CCP_PROTOCOL,
                     CONFIG_ACK,
                     ds707_netmdli_callback_ccp_c_ack);
    netmdl_snoop_reg(PPP_RM_DEV,
                     PPP_ILCCP_PROTOCOL,
                     CONFIG_ACK,
                     ds707_netmdli_callback_ccp_c_ack);
  }

  /*-----------------------------------------------------------------------
    Set up the state machines to look like they are open so we can
    close gracefully if need be.  
  -----------------------------------------------------------------------*/
  um_ppp_ptr->phase = pppNETWORK;
  um_ppp_ptr->fsm[LCP].state  = fsmOPENED;
  um_ppp_ptr->fsm[IPCP].state = fsmOPENED;
  rm_ppp_ptr->phase = pppNETWORK;
  rm_ppp_ptr->fsm[LCP].state  = fsmOPENED;
  rm_ppp_ptr->fsm[IPCP].state = fsmOPENED;


  return;
}/* ds707_netmdl_reg_ppp_up_callbacks() */

/*===========================================================================
FUNCTION   DS707_NETMDL_REG_PPP_RESYNC_CALLBACKS()

DESCRIPTION
  This function does the processing required during init of PPP setup for
  netmdl callbacks like registering lcp, ipcp callbacks etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdl_reg_ppp_resync_callbacks
(
  void
)
{
  ppp_type              *ppp_cb_array = NULL;
  ppp_type              *rm_ppp_ptr;
  qword                  ts_null;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  rm_ppp_ptr = ppp_cb_array + PPP_RM_DEV;

  /*-------------------------------------------------------------------------
    Set the state variable for this interface
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, "Netmdl: PPP Resyncing");

  /*------------------------------------------------------------------------ 
    Initialize ts_null
  ------------------------------------------------------------------------*/
  qw_set(ts_null, 0, 0);

  /*-------------------------------------------------------------------------
    Deregister all current callbacks
  -------------------------------------------------------------------------*/
  netmdl_snoop_unreg_all();

  /*-----------------------------------------------------------------------
    Set the Um interface into internal mode
  -----------------------------------------------------------------------*/
  ppp_set_mode(PPP_UM_SN_DEV, PPP_INTERNAL_MODE);

  /*-----------------------------------------------------------------------
    enable Rm flow control
  -----------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow(DS_FLOW_NETMODEL_MASK, DS_FLOW_DISABLE);

  /*-----------------------------------------------------------------------
    register callback function to be called when LCP completes.  This is
    to renegotiate CCP
  -----------------------------------------------------------------------*/
  if( ds707_ccp_update_proto != 0 )
  {
    lcp_set_net_model_resync_fnc( ds707_netmdli_lcp_resynced, PPP_UM_SN_DEV);
  }

  /*-----------------------------------------------------------------------
    Set callback to reject CCP on a resync in case the PDSN proposes CCP
    and we are set to reject it.  Also set the callbacks for receiving
    acking CCP to ensure that in case of successful renegotiation we
    set the ccp_update flag
  -----------------------------------------------------------------------*/
  if(PPP_CCP_REJECT == rm_ppp_ptr->ccp_config )
  {
    netmdl_snoop_reg(PPP_UM_SN_DEV,
                     PPP_CCP_PROTOCOL,
                     SNOOP_ANY_MSG,
                     ds707_netmdli_callback_ccp_c_req);
    netmdl_snoop_reg(PPP_UM_SN_DEV,
                     PPP_ILCCP_PROTOCOL,
                     SNOOP_ANY_MSG,
                     ds707_netmdli_callback_ccp_c_req);
  }
  else
  {
    netmdl_snoop_reg(PPP_RM_DEV,
                      PPP_CCP_PROTOCOL,
                      CONFIG_ACK,
                      ds707_netmdli_callback_ccp_c_ack);
    netmdl_snoop_reg(PPP_RM_DEV,
                      PPP_ILCCP_PROTOCOL,
                      CONFIG_ACK,
                      ds707_netmdli_callback_ccp_c_ack);
  }

  /*-----------------------------------------------------------------------
    register callback function to be called when IPCP competes (i.e. PPP
    is done.
  -----------------------------------------------------------------------*/
  ipcp_set_net_model_resync_fnc( ds707_netmdli_ppp_resynced, PPP_UM_SN_DEV );

  /*-----------------------------------------------------------------------
    Initialize PPP and start resyncing
  -----------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Resyncing PPP here");

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-----------------------------------------------------------------------
    Notify M.IP call performance that Um PPP resync is starting.
  -----------------------------------------------------------------------*/
  mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_UM_PPP_DELAY);
  mip_perf_delay(mip_perf_info_ptr,
                 PERF_UM_PPP_DELAY,
                 PERF_START_TS);
    
  /*-----------------------------------------------------------------------
    Notify M.IP call performance inter-PDSN handoff is starting if it has
    not been started yet.
  -----------------------------------------------------------------------*/
  mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_HANDOFF_DELAY);
  if (qw_cmp(mip_perf_info_ptr->ts_start, 
               ts_null) == 0) 
  {
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_TOTAL_HANDOFF_DELAY,
                   PERF_START_TS);
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

} /* ds707_netmdl_reg_ppp_resync_callbacks() */
/*lint -restore Restore lint error 641,655*/
/*===========================================================================
FUNCTION   DS707_NETMDL_TERMINATE_RM_PPP()

DESCRIPTION
  This function is used by netmodel clients to terminate Rm PPP (once
  Um PPP has been terminated.) If there is an outstanding LCP Term-Ack that 
  needs to be sent on Rm, it sends it out and aborts PPP. Else, it invokes 
  ppp_stop() to terminate Rm PPP by sending a Term-Req.

DEPENDENCIES
  None

RETURN VALUE
  0: Success
 -1: Failure

SIDE EFFECTS
  None
===========================================================================*/
int ds707_netmdl_terminate_rm_ppp
(
  void
)
{
  dsm_item_type * t_ack_ptr = NULL; /* buffer for Term-Ack                 */
  ppp_fsm_type  * fsm_ptr = NULL;   /* LCP fsm                             */
  ppp_type      * ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*-------------------------------------------------------------------------
    If there is an outstanding LCP T-Ack to be sent, send it and abort PPP,
    else do a ppp_stop
  -------------------------------------------------------------------------*/
  if( outstanding_lcp_t_ack_id != -1)
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED,
                    "Terminate Rm PPP: Using cached T-Req id %d", 
                    outstanding_lcp_t_ack_id );

    /*-----------------------------------------------------------------------
      Send the outstanding Term-Ack 
    -----------------------------------------------------------------------*/
    ppp_cb_array = fsm_get_ppp_cb_array();
    fsm_ptr = &(ppp_cb_array[PPP_RM_DEV].fsm[LCP]);
    if( fsm_send( fsm_ptr, 
                  TERM_ACK, 
                  outstanding_lcp_t_ack_id, 
                  t_ack_ptr ) != 0 )
    {
      /*---------------------------------------------------------------------
        Shouldn't happen. Update flag to indicate that there is no 
        outstanding T-Ack and call ppp_stop()
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Error sending T-Ack! Resetting T-Ack status. ");
      outstanding_lcp_t_ack_id = -1;      
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED,"Sent T-Ack on Rm, Id %d", outstanding_lcp_t_ack_id);
      
      /*---------------------------------------------------------------------
        RFC 1661 indicates that PPP must wait for atleast one Restart time
        after sending Term-Ack before cleaning up, but we make an exception
        for netmodel case since netmodel doesn't store PPP fsm state, and
        abort immediately.
      ---------------------------------------------------------------------*/
      (void)ppp_abort( PPP_RM_DEV );

      /*---------------------------------------------------------------------
        Update flag to indicate that there is no outstanding T-Ack
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_MED,"Resetting T-Ack status. ");
      outstanding_lcp_t_ack_id = -1;
      return 0;
    }
  }

  if( ppp_stop( PPP_RM_DEV, PPP_PROTOCOL_LCP ) != 0 )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"ppp_stop failed!");
    return -1;
  }

  return 0;
} /* ds707_netmdl_terminate_rm_ppp() */
