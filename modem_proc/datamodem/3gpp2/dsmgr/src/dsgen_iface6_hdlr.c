/*===========================================================================

                     D S G E N _ I F A C E 6 _ H D L R . C
                   
DESCRIPTION
  The Data Services sockets network state machine implementation for IPv6. 
  This contains all the functions used by the sockets library to access the 
  network and communicate with the DS task. (This is an INTERNAL sockets
  file).
 
EXTERNALIZED FUNCTIONS
  dssnet6_sm_create()
    Initialize the DSSNET6 State machine
  dssnet6_sm_post_event()
    Post events to the DSSNET6 state machine

 Copyright (c)2003-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/dsgen_iface6_hdlr.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
10/14/14    cx     Changed API of posing event to v6 SM
08/05/14    ms     DSDA Phase 2 changes.
07/18/14    hr     To support default APN
07/14/14    ms     Compiler warning fix.
05/27/14    sc     Avoid posting socket tear down twice during graceful termination
                   in case of dual IP scenario
05/26/14    vm     Perform PPP resync for VSNCP cause code 9 and 11
05/21/14    ms     Move the pdn clear throttling as part of PPP event callback
                   to avoid the race condition between allocation and free.
12/23/13    vm     Stack memory optimization changes
05/20/13    ssb    Reporting VSNCP error code to AP
08/23/12    jz     Gracefully tear down PPP upon receiving PDN ID in use
08/14/12    jee    To fix call failure during eHRPD to HRPD fallback
07/31/12    jee    To reset call end reason for 3gpp after iface down ind
05/03/12    jee    To fix embedded/DUN failure
04/12/12    jee    To propagate correct call end reason from MH
03/08/12    jee    Propagate call end reasons correctly
02/13/12    jz     Fixed PDN fallback didn't finish the state machine process 
                   and didn't set failure code  
01/25/12    msh    ppp_auth_ev_cback posts ppp close event to dssnet6 sm 
                   instead of transtion state  
01/18/12    jz     PDN failure fallback list 
01/02/12   msh     Coan: Feature cleanup 
11/28/11    jee    Fix to handle PPP fail reason
11/11/11    jee    To fix Arbitration issue with v4/v6 calls
10/28/11    ls     PDN throttle info notification through SYS IOCTL
10/25/11    jee    To fix DCTM issue
10/04/11    jee    To handle device manager when hot swap happens
10/03/11    msh    Added call end reason for unsupported IPv6 call over HDR   
09/02/11    ms     Fix error code for unsuccessful IP6 prefix refresh  
07/29/11    jee    Register AUTH failure event for IPv6
06/06/11    ash    Added support for PPP Partial Context
06/13/11    dvk    Merged support for PAP and CHAP auth failure reasons.
04/22/11    hs     Acquire crit section to avoid deadlock with tasks running
                   Route selection
03/28/11    ack    Fix to ignore certain states while processing phy_link_down_ev
03/23/11    sn     Globals cleanup for free floating of tasks.
03/15/11    mg     Global variable cleanup
02/18/11    mg     Clear sys info when all ifaces are down
01/12/11    ls     Transition to CLOSED_STATE if receiving PPP_CLOSE_CMD while
                   in CLOSE_STATE which will clean up the call
01/14/11    ack    Added PHY_LINK_DOWN_EV reg/dereg to dssnet4/6 SM
12/20/10    op     Removed an ASSERT(0) in dssnet6i_process_net_up_ev
12/08/10   op/mg   Protect C2K processing with use_ppp flag
10/14/10    sa     Adding debug messages before ASSERT.
10/12/10    mg     Changes for error code propogation to ps_iface
09/15/10    ms     Fixed compilation error.
09/09/10    op     Migrated to MSG 2.0 macros
09/07/10    op     IPv6 enable/disable feature for HRPD
07/18/10    mg     PDN throttling feature for eHRPD
07/12/10    mga    Added support for v6 iface tear down or PDN tear down based 
                   on current app
06/07/10    ss     Compiler warning fix.
04/28/10    hs     Used the new API to enqueue command to DS task internal queue
04/15/10    ssh    Really clean up iface in dssnet6i_process_net_down_ev
04/05/10    ls     Use ppp_event_handle to ensure PPP registeration that only
                   happens once
12/10/09    ps     Mainlining OMH feature based on new profile management
                   design.
11/06/09    ms     Decouple DHCP start from DHCP trigger.
09/22/09    vrk    Merged LTE changes.
07/29/09    spn    Remember if the call was made on eHRPD or not.
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
03/12/09  ssh/spn  PPP VSNCP interface changes
02/03/09    spn    Support for v6 VSNCP
12/15/08    sn     Fixed compiler warning and lint error.
06/17/08    sn     Support for multiple app profiles for OMH.
05/09/08    sn     Fixed compiler warnings.
01/22/08    scb    Mainlined features under FEATURE_JCDMA
07/23/07    sc     Fixed errors caused by non featurized 707 specific code 
                   in dssnet6i_transition_state()
07/02/07    sc     Added support for Application Profiles and Dormant2 (Iface
                   Lingering) features in dssnet6i_transition_state().
11/02/06    mct    Updated the IPv6 IID generation function params to reflect
                   IPv6 Privacy Extension changes.
02/14/06    sy     Populated the rx_sig_f_ptr to be used for HDLC library 
                   with QOS.
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP 
                   resync during a handup from 1x to DO
07/22/05    kvd    Added fail_reason to ppp_ev_callback.
05/12/05    rr     Added IPV6 support for UMTS.
04/19/05    vp     Changes to retreive IPv6 IID from NV.
04/18/05    mct    V6 iface in configuring state when dssnet6 is in opening 
                   state so RS can be sent. iface_down_ind should always 
                   occur after phys_link goes down unless it's being shared.
03/25/05    ifk    Reset curr_hop_limit to default on link opening and resync
08/17/04    kvd    Read and store default DNS addresses from NV. 
08/10/04    aku    Making sure that when we set the link local addr of the
                   PDSN as the IPv6 DNS server address the address type is
                   set correctly.
08/09/04    mct    Added ps_iface configuring indication to resync state in
                   dssnet6i_transition_state.                   
08/03/04    kvd    Use dsi_get_nv_item to get nv_item as this is in ds 
                   context
08/03/04    kvd    set PDSN as V6 DNS proxy if pdsn_as_ipv6_dns_proxy NV flag
                   is set.
07/29/04    kvd    Set bring_up/tear_down _cmd_f_ptr to ds707_bring_up 
                   /tear_down_cmd respectively.
07/15/04    kvd    Delete unnecessary ps headler file includes.
05/03/05    rc     Added call to is707_get_ppp_auth_info_from_nv() to get 
                   authentication info from NV.
04/21/04    mct    Renamed ds_dssnet6.h dsgen_iface6_hdlr.h.
04/21/04    sv     Added call to delete the associated prefixes for the iface
                   when the state machine transitions to CLOSED_STATE.
03/18/04    aku    Register for Phys link down and gone indications when
                   common dssnet bringup function is call and deregister
                   when transitioning to CLOSED state.
03/15/04    mct    Added call to dssnet6_sm_create to set the address family
                   to v6. Fixed dssnet6_sm_process_event() to call
                   CMDs instead of EVs. Also fixed PPP rx/tx pointers.
03/12/04    aku    Register for phys link up indication only when common 
                   dssnet bringup cmd is called. Deregister this indication
                   when processing the PHYS_LINK_UP_EV.
01/08/04    mvl    Restructured file layout, renamed various functions, and
                   changed initialization check
12/22/03    rc     Created Module
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
  #include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
#include "dstask.h"
#include "dstaski.h"
#include "data_msg.h"

#if defined(FEATURE_DATA_PS) && defined(FEATURE_DATA_PS_IPV6)
  #include "dsgen_iface_hdlr.h"
  #include "dsgen_iface6_hdlr.h"
  #include "ps_iface_addr_v6.h"
  #include "ps_iface.h"
  #include "amssassert.h"
  #include "msg.h"
  #include "ds707_extif.h"

  #ifdef FEATURE_DATA_IS707
    #include "ds707_pkt_mgr.h"
    #include "ds707_pkt_mgr_iface.h"
    #include "ds707_iface4_hdlr.h"
    #include "ds707_sec_pkt_mgr.h"
  #endif

  #include "ds3gmgr.h"
  #include "ds3gutil.h"
  #include "ds3gcfgmgr.h"
  #include "ds3gcfgmgr.h"
  #include "ps_ppp.h"
  #include "ps_ip.h"
  #include "ps_route.h"
  #include "nv.h"

#ifdef FEATURE_DATA_FOR_IMS
#include "ds_3gpp2_vendor.h"
#endif /* FEATURE_DATA_FOR_IMS */

#include "dshdr_dhcp_mgr.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_roaming.h"
#include "ds707_data_session_profile.h"
#include "ds707_sys_chg_hdlr.h"
#include "dsdctm.h"
#include "ds707_pkt_mgr_iface.h"
#include "ds707_pdn_context.h"
#include "ds707_pkt_mgri.h"
#include "ps_phys_link.h"

#ifdef FEATURE_EHRPD
#include "ds707_sec_pkt_mgr_util.h"
#include "ps_ppp_defs.h"
#include "ps_iface_defs.h"
#include "ps_ppp_ncp.h"
#include "ps_ppp_vsncp_3gpp2.h"
#include "ps_svc.h"
#include "ds_ehrpd_pdn_mgr.h"
#include "ds707_devmgr.h"
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_DATA_IS707 */

#include "data_err_suppr.h"
#include "ds_flow_control.h"
#include "ps_ifacei_utils.h"
#include "dsutil.h"
#include "ps_crit_sect.h"
#include "modem_mem.h"
#include "ds_3gpp_pdn_context.h"
#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */
#include "mc.h"
/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
rex_crit_sect_type dsgen_iface6_global_crit_sect = {{NULL}};

/*---------------------------------------------------------------------------
MACRO DSSNET6_SMI_IS_VALID

DESCRIPTION
  Make sure the SM instance passed in is valid
---------------------------------------------------------------------------*/
  #define DSSNET6I_IS_VALID( instance_ptr )                                   \
  (((instance_ptr) != NULL) && ((instance_ptr->this_sm_ptr) == instance_ptr))

#ifdef FEATURE_DATA_IS707
/*---------------------------------------------------------------------------
  Mask for PPP error codes in ps iface net down reasons
---------------------------------------------------------------------------*/
  #define DSSNET6_PPP_ERR_CODE_MASK   0x00070000

/*---------------------------------------------------------------------------
  Mask for VSNCP error codes in ps iface net down reasons
---------------------------------------------------------------------------*/
  #define DSSNET6_VSNCP_ERR_CODE_MASK 0x00080000
#endif

/*===========================================================================

                    FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
static void dssnet6i_transition_state
(
  dssnet6_sm_cb_type    *dssnet_sb_ptr,
  dssnet6i_state_type   new_state
);

static void dssnet6i_handle_ip6_ev_cback
(
  ip6_sm_type             *ip6_inst_ptr,
  ip6_sm_indidcation_type  ind,
  void                    *user_data
);

static void dssnet6i_handle_ppp_ev_cback
(
  ppp_dev_enum_type       ppp_dev,              /* PPP Device              */
  ppp_protocol_e_type     protocol,             /* PPP Protocol            */
  ppp_event_e_type        ppp_event,            /* Down/Up/Resync          */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
);

static void dssnet6i_handle_ppp_auth_ev_cback
(
  ppp_dev_enum_type       ppp_dev,              /* PPP Device             */
  ppp_protocol_e_type     protocol,             /* PPP Protocol           */
  ppp_event_e_type        ppp_event,            /* SUCCESS/FAIL           */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
);

static void dssnet6i_handle_phys_link_ev_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
);

static void dssnet6i_process_phy_link_up_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

static void dssnet6i_process_phy_link_gone_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

static void dssnet6i_process_phy_link_down_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);


static void dssnet6i_process_link_up_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

static void dssnet6i_process_link_down_ev
(
  dssnet6_sm_cb_type *instance_ptr,            /* dssnet6 instance ptr      */
  ds_v6_call_status_type *call_status          /* failure status */
);

static void dssnet6i_process_link_resync_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

static void dssnet6i_process_net_up_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

static void dssnet6i_process_net_down_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

static void dssnet6i_process_ppp_close_cmd_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

static void dssnet6i_process_flow_deleted_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

void dssnet6i_configure_ppp
(
  dssnet6_sm_cb_type *instance_ptr,           /* dssnet6 instance ptr      */
  ppp_dev_opts_type  *ppp_config_ptr
);

static void dssnet6i_process_net_aborted_ev
(
  dssnet6_sm_cb_type *instance_ptr,            /* dssnet6 instance ptr      */
  ds_v6_call_status_type  *call_status

);


static boolean dssnet6i_get_ipv6_prefix_delegation_mode
(
   ps_iface_type *ps_iface_p
);

#ifdef FEATURE_DATA_IS707
ps_iface_net_down_reason_type dssnet6i_convert_ppp_failure_code
(
  ppp_fail_reason_e_type reason
);
#endif

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DSSNET6_INIT

DESCRIPTION   Called once at mobile power-up. Initializes critical section.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssnet6_init(void)
{
  /* Initialize dsgen_iface6 crit section. */
  DS_INIT_CRIT_SECTION( &dsgen_iface6_global_crit_sect );

  return;
}

/*===========================================================================
FUNCTION DSSNET6_SM_CREATE()

DESCRIPTION
  This function is is used to initialize the dssnet6 state machine control 
  block. Called by the mode specific handler. The mode specific handler
  allocates memory for the control block as well as the ps iface.

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnet6_sm_create
(
  dssnet6_sm_cb_type    *instance_ptr,       /* Ptr to DSSNET6 SM instance */
  ps_iface_type         *ps_iface_ptr,       /* Interface Ptr              */
  ip6_sm_type           *ip6_inst_ptr,       /* Ptr to IP6 SM instance     */
  boolean                use_ppp,              /* Interface uses PPP or not  */
  sys_modem_as_id_e_type subs_id
)
{

  struct ps_in6_addr *nv_primary_dns_ptr   = NULL;
  struct ps_in6_addr *nv_secondary_dns_ptr = NULL;


  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return -1;  
  }


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Make sure the DSSNET6 control block and the ps iface are not NULL.
  -------------------------------------------------------------------------*/
  if ((instance_ptr == NULL) || ( ps_iface_ptr == NULL))
  {
    DATA_ERR_FATAL("Invalid Instance ptr or Iface pointer");
    return -1;
  }

  if (instance_ptr->this_sm_ptr == instance_ptr)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "DSSNet6 SM 0x%x already initialized", instance_ptr);
    return 0;
  }

  instance_ptr->state            = DSSNET6I_CLOSED_STATE;
  instance_ptr->ps_iface_ptr     = ps_iface_ptr;
  instance_ptr->ip6_instance     = ip6_inst_ptr;
  instance_ptr->qnc_call         = FALSE;
  instance_ptr->use_ppp          = use_ppp;
  instance_ptr->is_ehrpd_call    = FALSE;   
  instance_ptr->call_end_reason  = PS_NET_DOWN_REASON_NOT_SPECIFIED;

  /*-------------------------------------------------------------------------
   Initialize the PPP event handler to NULL & configure dns server info
  -------------------------------------------------------------------------*/
  instance_ptr->ppp_event_handle        = NULL;

  instance_ptr->pdsn_as_proxy_ipv6_dns_server 
                                    = ds3g_is_pdsn_as_proxy_ipv6_dns_server(subs_id);

  nv_primary_dns_ptr = ds3g_get_ipv6_primary_dns_addr(subs_id);
  nv_secondary_dns_ptr = ds3g_get_ipv6_secondary_dns_addr(subs_id);
  if ((NULL == nv_primary_dns_ptr) || (NULL == nv_secondary_dns_ptr))
  {
    DATA_ERR_FATAL("Invalid primary or secondary dns pointer");
    return -1;
  }

  instance_ptr->nv_primary_dns.in6_u.u6_addr64[0] 
                                    = nv_primary_dns_ptr->in6_u.u6_addr64[0];
  instance_ptr->nv_primary_dns.in6_u.u6_addr64[1] 
                                    = nv_primary_dns_ptr->in6_u.u6_addr64[1];
  instance_ptr->nv_secondary_dns.in6_u.u6_addr64[0] 
                                    = nv_secondary_dns_ptr->in6_u.u6_addr64[0];
  instance_ptr->nv_secondary_dns.in6_u.u6_addr64[1] 
                                    = nv_secondary_dns_ptr->in6_u.u6_addr64[1];

  /*-------------------------------------------------------------------------
   Initialize the PPP resync done callback function ptr to NULL
  -------------------------------------------------------------------------*/
  instance_ptr->ppp_resync_done_cb      = NULL;

  instance_ptr->iface_down_ind_cb   =   NULL;
  /*-------------------------------------------------------------------------
    Setup current iface for ip6
  -------------------------------------------------------------------------*/
  if (-1 == ps_iface_set_addr_family(ps_iface_ptr, IFACE_IPV6_ADDR_FAMILY))
  {
    DATA_ERR_FATAL("ps_iface_set_addr_family() failed, returned -1");
    return -1;
  }

  /*-----------------------------------------------------------------------
    Allocate buffer for Phys link Up Event. Note that the phys link up event 
    is registered only when the common dssnet_start() invokes
    the dssnet4_reg_phys_link_up_event() function.
  -----------------------------------------------------------------------*/
  instance_ptr->phys_link_up_cbuf = 
  ps_iface_alloc_event_cback_buf(dssnet6i_handle_phys_link_ev_cback,
                                 instance_ptr);

  if (instance_ptr->phys_link_up_cbuf == NULL)
  {
    DATA_MSG0(MSG_LEGACY_FATAL,"No PS IFACE Event Buffers Avail");
  }

  /*-------------------------------------------------------------------------
    Register the command handlers for DSSNET related events with PS.
    This is used only for 1x.
  -------------------------------------------------------------------------*/
  if (use_ppp)
  {
    /*-----------------------------------------------------------------------
      Allocate buffer for Phys link Down Event. 
    -----------------------------------------------------------------------*/
    instance_ptr->phys_link_down_cbuf = 
    ps_iface_alloc_event_cback_buf(dssnet6i_handle_phys_link_ev_cback,
                                   instance_ptr);
  
    if (instance_ptr->phys_link_down_cbuf == NULL)
    {
      DATA_MSG0(MSG_LEGACY_FATAL,"No PS IFACE Event Buffers Avail");
    }

    /*-----------------------------------------------------------------------
      Allocate buffer for Phys link Gone Event.    
    -----------------------------------------------------------------------*/
    instance_ptr->phys_link_gone_cbuf = 
    ps_iface_alloc_event_cback_buf(dssnet6i_handle_phys_link_ev_cback,
                                   instance_ptr);

    if (instance_ptr->phys_link_gone_cbuf == NULL)
    {
      DATA_MSG0(MSG_LEGACY_FATAL,"No PS IFACE Event Buffers Avail");
    }
  } /* if(use_ppp) */

  /*-----------------------------------------------------------------------
    Allocate buffer for iface flow deleted ev.    
  -----------------------------------------------------------------------*/
  instance_ptr->iface_flow_deleted_cbuf = 
          ps_iface_alloc_event_cback_buf(dssnet6i_handle_phys_link_ev_cback,
                                         instance_ptr);

  if (instance_ptr->iface_flow_deleted_cbuf == NULL)
  {
    DATA_MSG0(MSG_LEGACY_FATAL,"No PS IFACE Event Buffers Avail");
  }

  /*-------------------------------------------------------------------------
    Register for IP6 UP/DOWN Events
  -------------------------------------------------------------------------*/
  if (-1 == ip6_sm_ind_cback_reg( instance_ptr->ip6_instance,
                            (ip6_sm_cback_type)dssnet6i_handle_ip6_ev_cback,
                            instance_ptr))
  {
    DATA_ERR_FATAL("ip6_sm_ind_cback_reg() returned -1");
  }

  /*-------------------------------------------------------------------------
    This state machine is now initialized
  -------------------------------------------------------------------------*/
  instance_ptr->this_sm_ptr = instance_ptr;

  return 0;

} /* dssnet6_sm_create() */

/*===========================================================================
FUNCTION DSSNET6_SM_UPDATE()

DESCRIPTION
  This function is is used to update the v6 sm

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnet6_sm_update
(
  dssnet6_sm_cb_type    *instance_ptr,       /* Ptr to DSSNET6 SM instance */
  ps_iface_type         *ps_iface_ptr,       /* Interface Ptr              */
  ip6_sm_type           *ip6_inst_ptr,       /* Ptr to IP6 SM instance     */
  boolean                use_ppp,              /* Interface uses PPP or not  */
  sys_modem_as_id_e_type subs_id
)
{

  struct ps_in6_addr *nv_primary_dns_ptr   = NULL;
  struct ps_in6_addr *nv_secondary_dns_ptr = NULL;


  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return -1;  
  }


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Make sure the DSSNET6 control block and the ps iface are not NULL.
  -------------------------------------------------------------------------*/
  if ((instance_ptr == NULL) || ( ps_iface_ptr == NULL))
  {
    DATA_ERR_FATAL("Invalid Instance ptr or Iface pointer");
    return -1;
  }

   instance_ptr->pdsn_as_proxy_ipv6_dns_server 
                                    = ds3g_is_pdsn_as_proxy_ipv6_dns_server(subs_id);

  nv_primary_dns_ptr = ds3g_get_ipv6_primary_dns_addr(subs_id);
  nv_secondary_dns_ptr = ds3g_get_ipv6_secondary_dns_addr(subs_id);
  if ((NULL == nv_primary_dns_ptr) || (NULL == nv_secondary_dns_ptr))
  {
    DATA_ERR_FATAL("Invalid primary or secondary dns pointer");
    return -1;
  }

  instance_ptr->nv_primary_dns.in6_u.u6_addr64[0] 
                                    = nv_primary_dns_ptr->in6_u.u6_addr64[0];
  instance_ptr->nv_primary_dns.in6_u.u6_addr64[1] 
                                    = nv_primary_dns_ptr->in6_u.u6_addr64[1];
  instance_ptr->nv_secondary_dns.in6_u.u6_addr64[0] 
                                    = nv_secondary_dns_ptr->in6_u.u6_addr64[0];
  instance_ptr->nv_secondary_dns.in6_u.u6_addr64[1] 
                                    = nv_secondary_dns_ptr->in6_u.u6_addr64[1];

  DATA_3GMGR_MSG3(MSG_LEGACY_MED,"DSSNet6 SM 0x%x for iface 0x%x:%d updated",
                  instance_ptr,
                  ps_iface_ptr->name,
                  ps_iface_ptr->instance);



  return 0;

} /* dssnet6_sm_update() */

/*===========================================================================
FUNCTION DSSNET6_SM_POST_EVENT()

DESCRIPTION
  This function is used to post events to the dssnet_sm by other modules.  

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnet6_sm_post_event
(
  dssnet6_sm_cb_type        *instance_ptr,   /* Ptr to DSSNET6 SM instance */
  dssnet6_sm_event_type     event,            /* dssnet6 event type         */
  void                     *event_data
)
{
  ds_cmd_type        *cmd_ptr;                              /* ps cmd type */
  ds_cmd_enum_type    cmd_id;
  ds_v6_call_status_type   *v6_call_status         = NULL;
  ds_v6_call_status_type   *v6_call_status_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(event < DSSNET6_MAX_EV);

  /*-------------------------------------------------------------------------
    Verify that this state machine has been initialied.
  -------------------------------------------------------------------------*/
  if ( !DSSNET6I_IS_VALID(instance_ptr) )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Tried posting to invalid DSSNet6 instance 0x%x",
                    instance_ptr);
    return -1;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "Posting event %d to DSSNet6(0x%x) in state %d.",
                  event,
                  instance_ptr,
                  instance_ptr->state);

  /*-------------------------------------------------------------------------
    Initialize command name to DS_MIN_CMD.
  -------------------------------------------------------------------------*/
  cmd_id = DS_MIN_CMD;

  /*-------------------------------------------------------------------------
    Based on the event type, set the appropriate PS cmd 
  -------------------------------------------------------------------------*/
  switch (event)
  {
    case DSSNET6_PHY_LINK_UP_EV:
      cmd_id = DS_CMD_DSSNET6_PHY_LINK_UP_EV;
      break;

    case DSSNET6_PHY_LINK_GONE_EV:
      cmd_id = DS_CMD_DSSNET6_PHY_LINK_GONE_EV;
      break;

    case DSSNET6_PHY_LINK_DOWN_EV:
      cmd_id = DS_CMD_DSSNET6_PHY_LINK_DOWN_EV;
      break;

    case DSSNET6_LINK_UP_EV:
      cmd_id = DS_CMD_DSSNET6_LINK_UP_EV;
      break;

    case DSSNET6_LINK_DOWN_EV:
      cmd_id = DS_CMD_DSSNET6_LINK_DOWN_EV;
      break;

    case DSSNET6_LINK_RESYNC_EV:
      cmd_id = DS_CMD_DSSNET6_LINK_RESYNC_EV;
      break;

    case DSSNET6_NET_UP_EV:
      cmd_id = DS_CMD_DSSNET6_NET_UP_EV;
      break;

    case DSSNET6_NET_DOWN_EV:
      cmd_id = DS_CMD_DSSNET6_NET_DOWN_EV;
      break;

    case DSSNET6_PPP_CLOSE_CMD_EV:
      cmd_id = DS_CMD_DSSNET6_PPP_CLOSE_EV;
      break;

    case DSSNET6_FLOW_DELETED_EV:
      cmd_id = DS_CMD_DSSNET6_FLOW_DELETED_EV;
      break;

    case DSSNET6_NET_ABORTED_EV:
      cmd_id = DS_CMD_DSSNET6_NET_ABORTED_EV;
      break;
    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -save -e506, -e774 */
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x got unknown event %d", instance_ptr, event);
      ASSERT(0);
      /*lint -restore */

  } /* switch(event) */

  /*-------------------------------------------------------------------------
    Enqueue cmd with DS task if it has been set to a valid value 
  -------------------------------------------------------------------------*/
  if (cmd_id != DS_MIN_CMD)
  {
    if( event == DSSNET6_LINK_DOWN_EV || event == DSSNET6_NET_ABORTED_EV )
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_v6_call_status_type));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return -1;
      }

      v6_call_status_cmd_ptr 
                      = (ds_v6_call_status_type *)(cmd_ptr->cmd_payload_ptr);

      memset(v6_call_status_cmd_ptr, 0, sizeof(ds_v6_call_status_type));

      v6_call_status_cmd_ptr->call_type = DS_CALL_CATEGORY_MAX;
      v6_call_status_cmd_ptr->dssnet_instance = (void *)instance_ptr;
      
      if (event_data != NULL)
      {
        v6_call_status = (ds_v6_call_status_type *)(event_data);

        v6_call_status_cmd_ptr->call_type = v6_call_status->call_type;

        if ( v6_call_status->call_type == DS_CALL_CATEGORY_NON_PPP_TYPE )
        {
          v6_call_status_cmd_ptr->call_end_reason_info.call_status = 
                        v6_call_status->call_end_reason_info.call_status;
        }
#ifdef FEATURE_DATA_IS707
        else if ( v6_call_status->call_type == DS_CALL_CATEGORY_PPP_TYPE )
        {
          v6_call_status_cmd_ptr->call_end_reason_info.ppp_status.ppp_protocol 
            = v6_call_status->call_end_reason_info.ppp_status.ppp_protocol;
          v6_call_status_cmd_ptr->call_end_reason_info.ppp_status.ppp_fail_reason 
            = v6_call_status->call_end_reason_info.ppp_status.ppp_fail_reason;
          v6_call_status_cmd_ptr->call_end_reason_info.ppp_status.dssnet_instance 
            = v6_call_status->call_end_reason_info.ppp_status.dssnet_instance;
        }
#endif
      }
    }

    else
    {
      if( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
      {
        ASSERT(0);
        return -1;
      }

      cmd_ptr->cmd_payload_ptr = (void *)instance_ptr;
    }

    cmd_ptr->hdr.cmd_id  = cmd_id;

    ds_put_cmd_ext( cmd_ptr );
  }

  return 0;
} /* dssnet6_sm_post_event() */


/*===========================================================================
FUNCTION DSSNET4_SM_PROCESS_EVENT()

DESCRIPTION
  This function is used to process events posted to the dssnet4 state 
  machine. 

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet6_sm_process_event
(
  ds_cmd_type *cmd_ptr                          /* Pointer to ds_cmd_type  */
)
{
  dssnet6_sm_cb_type      *instance_ptr       = NULL;
  ds_v6_call_status_type  *v6_call_status_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  if( cmd_ptr->hdr.cmd_id == DS_CMD_DSSNET6_LINK_DOWN_EV || 
      cmd_ptr->hdr.cmd_id == DS_CMD_DSSNET6_NET_ABORTED_EV)
  {
    v6_call_status_ptr = (ds_v6_call_status_type  *)(cmd_ptr->cmd_payload_ptr);

    ASSERT(v6_call_status_ptr->dssnet_instance != NULL);
    instance_ptr = (dssnet6_sm_cb_type *)(v6_call_status_ptr->dssnet_instance);
  }
  else
  {
    instance_ptr = (dssnet6_sm_cb_type *)(cmd_ptr->cmd_payload_ptr);
  }

  switch (cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_DSSNET6_PHY_LINK_UP_EV:
      dssnet6i_process_phy_link_up_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_PHY_LINK_GONE_EV:
      dssnet6i_process_phy_link_gone_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_PHY_LINK_DOWN_EV:
      dssnet6i_process_phy_link_down_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_LINK_UP_EV:
      dssnet6i_process_link_up_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_LINK_DOWN_EV:
      dssnet6i_process_link_down_ev(instance_ptr, v6_call_status_ptr);
      break;

    case DS_CMD_DSSNET6_LINK_RESYNC_EV:
      dssnet6i_process_link_resync_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_NET_UP_EV:
      dssnet6i_process_net_up_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_NET_DOWN_EV:
      dssnet6i_process_net_down_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_PPP_CLOSE_EV:
      dssnet6i_process_ppp_close_cmd_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_FLOW_DELETED_EV:
      dssnet6i_process_flow_deleted_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET6_NET_ABORTED_EV:
      dssnet6i_process_net_aborted_ev(instance_ptr, v6_call_status_ptr);
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Illegal cmd - %d", cmd_ptr->hdr.cmd_id);

      /*---------------------------------------------------------------------
        Sanity check
      ---------------------------------------------------------------------*/
      break;
  }/* end switch() */

  return;
} /* dssnet6_sm_process_event() */

/*===========================================================================
FUNCTION DSSNET6_REG_PHYS_LINK_EVENTS()

DESCRIPTION
  The common dssnet_start() invokes this function to register
  for the phys link events.
  
DEPENDENCIES
  None.

RETURN VALUE
  0: on success
 -1: on failure
 
SIDE EFFECTS
  None.
===========================================================================*/
int dssnet6_reg_phys_link_events
(
  dssnet6_sm_cb_type *instance_ptr
)
{
  int result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Register for PHYS LINK UP event.
  -------------------------------------------------------------------------*/
  DATA_MDM_ARG_NOT_CONST(instance_ptr);
  result = ps_iface_event_cback_reg( instance_ptr->ps_iface_ptr,
                                     IFACE_PHYS_LINK_UP_EV,
                                     instance_ptr->phys_link_up_cbuf);
  if (result < 0)
  {
    return result;
  }

  /*-------------------------------------------------------------------------
    Register for PHYS LINK DOWN event.
  -------------------------------------------------------------------------*/
  if (instance_ptr->use_ppp)
  {
    result = ps_iface_event_cback_reg( instance_ptr->ps_iface_ptr,
                                       IFACE_PHYS_LINK_DOWN_EV,
                                       instance_ptr->phys_link_down_cbuf);
    if (result < 0)
    {
      return result;
    }
  }

  /*-------------------------------------------------------------------------
    Register for PHYS LINK GONE event.
  -------------------------------------------------------------------------*/
  return ps_iface_event_cback_reg( instance_ptr->ps_iface_ptr,
                                   IFACE_PHYS_LINK_GONE_EV,
                                   instance_ptr->phys_link_gone_cbuf);

} /* dssnet6_reg_phys_link_events() */

/*===========================================================================
FUNCTION DSSNET6_REG_PPP_RESYNC_DONE_CB()

DESCRIPTION
  This function registers the call back function to notify the completion of
  a PPP resync (it could be a successful or a failure resync)

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the dssnet instance and pointer to the call back function
  to be registered

RETURN VALUE
  TRUE if registration succeeds and FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssnet6_reg_ppp_resync_done_cb
(
  dssnet6_sm_cb_type *instance_ptr,
  void (* ppp_resync_done_cb)(void *)
)
{
  boolean reg_status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!(DSSNET6I_IS_VALID(instance_ptr)))
  {
    return reg_status;
  }
  if (instance_ptr->state != DSSNET6I_CLOSED_STATE)
  {
    instance_ptr->ppp_resync_done_cb = ppp_resync_done_cb;
    reg_status = TRUE;
  }
  return reg_status;
}

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSSNET6I_PROCESS_FLOW_DELETED_EV()

DESCRIPTION
  This function processes the DSSNET6_FLOW_DELETED_CMD_EV. Changes state to 
  DSSNET6I_CLOSED_STATE if number of secondary flows are 0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_flow_deleted_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*lint -save -e641*/
/*---------------------------------------------------------------------------
  State doesn't matter in this case. The interface can be torn down in any
  of the cases (link_down and phys_link_gone) and the flow_deleted event will
  only be registered in those cases where dssnet is trying to close the state
  machine anyway.
---------------------------------------------------------------------------*/
  DATA_MDM_ARG_NOT_CONST(instance_ptr);
  if (PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) == 0)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                    "IFACE_FLOW_DELETED_EV: Flows on iface 0x%x cleaned up.",
                    instance_ptr->ps_iface_ptr);
    (void)ps_iface_event_cback_dereg(
                                    instance_ptr->ps_iface_ptr,
                                    IFACE_FLOW_DELETED_EV,
                                    instance_ptr->iface_flow_deleted_cbuf
                                    );
#ifdef FEATURE_DATA_IS707
    if ( PS_IFACE_GET_NAME_FROM_ID( PS_IFACE_GET_ID( instance_ptr->ps_iface_ptr ) )
         == CDMA_SN_IFACE )
    {
      ds707_pkt_mgr_iface_start_conn_delay_timer(instance_ptr->ps_iface_ptr);
    }
#endif /* FEATURE_DATA_IS707 */
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                    "IFACE_FLOW_DELETED_EV: Flows still exist on iface 0x%x",
                    instance_ptr->ps_iface_ptr);
  }
/*lint -restore Restore lint error 641*/
} /* dssnet6i_process_flow_deleted_cmd_ev() */


/*===========================================================================
FUNCTION DSSNET6I_PROCESS_PHY_LINK_UP_EV()

DESCRIPTION
  This function processes the DSSNET6_PHY_LINK_UP_EV event. 

  State will be changed to DSSNET6I_WAITING_FOR_NET_UP_STATE 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_phy_link_up_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
#ifdef FEATURE_DATA_IS707
  boolean terminate_call = FALSE;
  boolean handover_attach = FALSE;
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
  ps_iface_type* v4_iface_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_DATA_IS707 */
#ifdef FEATURE_DATA_IS707
  ps_iface_net_down_reason_type call_end_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr = NULL;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "PHY_LINK_UP_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:
      /*---------------------------------------------------------------------
       Traffic channel is establised. Transition to LINK_OPENING_STATE only
       if use_ppp is set i.e. it is a 1x call. In either case deregister
       the phys_link_up event.
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                      "DSSNet6 0x%x waiting for link up ",instance_ptr);
      if (instance_ptr->use_ppp)
      {
        /*---------------------------------------------------------------
          Once the phys-link is up, perform the check again, if the 
          requested profile is allowed to go on the current system.
        ---------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
        if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                  instance_ptr->ps_iface_ptr,
                                                  &data_sess_profile_info_ptr) )
        {
          ds707_data_session_get_iface_current_profile(
                                                  instance_ptr->ps_iface_ptr,
                                                  &data_sess_profile_info_ptr);
        }
      
        pdncntx_cb_ptr = 
          ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(instance_ptr->ps_iface_ptr);
        if ( DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) && 
             (data_sess_profile_info_ptr != NULL) )
        {
#ifdef FEATURE_EPC_HANDOFF
          handover_attach = pdncntx_cb_ptr->handover_attach;
#endif
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
          if ( (ds707_pkt_is_ehrpd_mode_of_operation() == FALSE) &&
               (data_sess_profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6) )
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "dual IP request in hrpd mode" );
            v4_iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);

            DATA_3GMGR_MSG0( MSG_LEGACY_LOW,
                           "set v4 and v6 iface to passive" );
            PS_IFACE_SET_IS_ACTIVE(instance_ptr->ps_iface_ptr, FALSE);
            PS_IFACE_SET_IS_ACTIVE(v4_iface_ptr, FALSE);
 
            instance_ptr->is_ehrpd_call = FALSE;

            if( (!PS_IFACE_IS_IN_USE(v4_iface_ptr)) &&
                (ps_iface_state(v4_iface_ptr) == IFACE_DOWN) )
            {
              DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                              "v4 iface 0x%x:%d not in use, in down state - free v4 iface", 
                              v4_iface_ptr->name, v4_iface_ptr->instance);
              ds707_pdncntx_free_v4_iface(pdncntx_cb_ptr);
            }
            if(!PS_IFACE_IS_IN_USE(instance_ptr->ps_iface_ptr))
            {
              DATA_3GMGR_MSG2( MSG_LEGACY_LOW,
                               "v6 iface 0x%x:%d not in use, transition dssnet6 to closed state",
                               instance_ptr->ps_iface_ptr->name, 
                               instance_ptr->ps_iface_ptr->instance );
              dssnet6i_transition_state(instance_ptr,DSSNET6I_CLOSED_STATE);
              return;
            }
          }
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */
        }
        else
        {
          DATA_3GMGR_MSG2( MSG_LEGACY_LOW, 
                           "PDN cb invalid for iface 0x%x:%d",
                           instance_ptr->ps_iface_ptr->name,
                           instance_ptr->ps_iface_ptr->instance );
          dssnet6i_transition_state(instance_ptr,DSSNET6I_CLOSED_STATE);
          return;
        }
      
        /*--------------------------------------------------------------- 
          DS707_IS_HRPD_IPV6_ENABLED is used to block V6 calls on
          HRPD networks. Certain networks may not support V6 only when in
          HRPD. Those will have this feature turned off. 
          Terminate the call for following combinations
         - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          FEAT EHRPD | eHRPD mode |  IS_HRPD_V6  | Result
          - - - - - - - - - - - - - - - - - - - - - - - - - - -
             0       |     0      |     0        | Disallow
             1       |     0      |     0        | Disallow
         - - - - - - - - -  - - - - - - - - - - - - - - - - - -
          All other cases we allow the V6 call
        ---------------------------------------------------------------*/
        terminate_call = FALSE;
#ifdef FEATURE_EHRPD
        if (ds707_pkt_is_ehrpd_mode_of_operation() == FALSE)
#endif
        {
#ifdef FEATURE_DATA_PS_IPV6
          if (!ds707_is_hrpd_ipv6_enabled())
          {
            terminate_call = TRUE;
            dssnet6_set_ps_call_end_reason(instance_ptr, 
                                   PS_NET_DOWN_REASON_IPV6_ERR_HRPD_IPV6_DISABLED);
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Call terminated, IPv6 is disabled on HRPD");
          }
#endif /* FEATURE_DATA_PS_IPV6 */
        }

        /*---------------------------------------------------------------------
          Check if PDN CB orig sys is compatible with the current pref sys & 
          check if packet call is allowed with the current configuration
        ---------------------------------------------------------------------*/
        if ((terminate_call == FALSE) &&
            (ds707_pdncntx_is_call_allowed(pdncntx_cb_ptr, &call_end_reason)) &&
            (dssnet_call_allowed ( data_sess_profile_info_ptr,
                                   handover_attach,
                                   &call_end_reason ) == TRUE ) )
        {
#endif /* FEATURE_DATA_IS707 */
          dssnet6i_transition_state(instance_ptr,
                                  DSSNET6I_LINK_OPENING_STATE);
#ifdef FEATURE_DATA_IS707
        }
        else
        {
          /*---------------------------------------------------------------
            Since the Application is not allowed to bring up the call, 
            dont allow it.
          ---------------------------------------------------------------*/
          DATA_3GMGR_MSG0(MSG_LEGACY_MED,"Call not allowed in current config");
          dssnet6_set_ps_call_end_reason(instance_ptr, 
                                         call_end_reason);
          dssnet6i_transition_state(instance_ptr,DSSNET6I_CLOSED_STATE);
        }
#endif /* FEATURE_DATA_IS707 */
      }
      ps_iface_event_cback_dereg(instance_ptr->ps_iface_ptr,
                                 IFACE_PHYS_LINK_UP_EV,
                                 instance_ptr->phys_link_up_cbuf);

      break;

    case DSSNET6I_LINK_OPENING_STATE: 
    case DSSNET6I_WAITING_FOR_NET_UP_STATE: 
    case DSSNET6I_NET_UP_STATE: 
    case DSSNET6I_LINK_CLOSING_STATE: 
    case DSSNET6I_LINK_RESYNC_STATE:
    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x ignoring PHY_LINK_UP_EV in %d state",
                      instance_ptr,
                      instance_ptr->state);
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "DSSNet6 Invalid %d state",
                      (int)instance_ptr->state);
      ASSERT(0);
      break;
  } /* switch */

} /* dssnet6i_process_phy_link_up_ev() */

/*===========================================================================
FUNCTION DSSNET6I_PROCESS_PHY_LINK_GONE_EV()

DESCRIPTION
  This function processes the DSSNET6_PHY_LINK_GONE_EV. It causes a 
  transition to the CLOSED STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_phy_link_gone_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
#ifdef FEATURE_DATA_IS707
  rex_timer_type *conn_delay_timer;
#endif /* FEATURE_DATA_IS707 */
  ds3g_pkt_call_info_type *pkt_call_info_ptr = NULL;
  int16                     p_errno;
  int                       ret_val;
  ps_flow_type             *ps_flow_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*lint -save -e641*/
/*lint -save -e655*/
  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "PHY_LINK_GONE_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

  /*---------------------------------------------------------------------------
    Try to bringup the phys link again if this flag is set. This will be set
    when there is a call bringup attempt when phys link is still being cleaned
    up. After the phys link is torn down completely an attempt to bringup the
    phys link again needs to be tried.
  ---------------------------------------------------------------------------*/
  if (dssnet6_get_is_bringup_phys_link(instance_ptr))
  {
    dssnet6_set_is_bringup_phys_link(instance_ptr, FALSE);

    /*-------------------------------------------------------------------------
      All the flows are unbinded when phys link is cleaned up, so bind the flow
      again. Without this another dssnet handling gone event will clean up the
      phys link again. Activate the flow else the data transfer will fail
    -------------------------------------------------------------------------*/
    if ((NULL != instance_ptr->ps_iface_ptr) && 
        (NULL != instance_ptr->ps_iface_ptr->iface_private.phys_link.array))
    {
      ps_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(instance_ptr->ps_iface_ptr);
      (void) ps_flow_bind_phys_link(ps_flow_ptr,
                        PS_IFACEI_GET_PHYS_LINK(instance_ptr->ps_iface_ptr));
      ps_flow_activate_ind(ps_flow_ptr, PS_EIC_NOT_SPECIFIED);
    }

    else
    {
      /*-----------------------------------------------------------------------
        This should never happen
      -----------------------------------------------------------------------*/
      ASSERT(0);
      return;
    }

    /*------------------------------------------------------------------------
      pass valid value as user info to phys_link_up_cmd. Create a struct of
      type ds3g_pkt_call_info_type and pass it as user_info. this  function
      populates pkt_call_info and returns void*.
    ------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707   
    pkt_call_info_ptr = ds707_pkt_mgr_setup_pkt_call_info(
                                                   instance_ptr->ps_iface_ptr);
#endif /* FEATURE_DATA_IS707 */

    ret_val = ps_iface_phys_link_up_cmd ( instance_ptr->ps_iface_ptr,
                                          &p_errno,
                                          pkt_call_info_ptr );
    if (!((ret_val < 0) && (p_errno != DS_EWOULDBLOCK)))
    {
      /*---------------------------------------------------------------------
        Phys link up cmd successful. No other dssnet state transition needs
        to be done after this
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG0(MSG_LEGACY_MED, "phy link up cmd successful");
      return;
    }
  }

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:

      /* Stop the DHCP instance before giving down indication
       */
#ifdef FEATURE_DATA_IS707
      (void) dshdr_stop_dhcp(instance_ptr->ps_iface_ptr);
#endif /* FEATURE_DATA_IS707 */

      dssnet6i_transition_state(instance_ptr,DSSNET6I_CLOSED_STATE);

      break;

    case DSSNET6I_NET_UP_STATE:
    case DSSNET6I_LINK_RESYNC_STATE:
    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
      /*---------------------------------------------------------------------
        Exit IPv6.
      ---------------------------------------------------------------------*/ 
      /* since the Phys link is gone, we can't send any OTA signalling for V6 stop */
      if (ip6_sm_post_event(instance_ptr->ip6_instance, IP6_SM_ABORT_EV, NULL) 
          == -1)
      {
        DATA_ERR_FATAL("ip6_sm_post_event() failed returned -1");
      }
      /* Fall through */

    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
    case DSSNET6I_LINK_OPENING_STATE:
    case DSSNET6I_LINK_CLOSING_STATE:
      {
        /*-------------------------------------------------------------------
          Call ppp_abort().
        -------------------------------------------------------------------*/
        (void) ppp_abort(PPP_UM_SN_DEV);
        if (PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) == 0)
        {
          /*---------------------------------------------------------------------
          If conn delay timer is running, ignore this event. When the timer 
          expires, the dssnet state will be transitioned
          ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
          if ( PS_IFACE_GET_NAME_FROM_ID( PS_IFACE_GET_ID( instance_ptr->ps_iface_ptr ) )
               == CDMA_SN_IFACE )
          {         
            conn_delay_timer = 
              (rex_timer_type *)ds707_pkt_get_conn_delay_timer_from_iface_ptr(
                instance_ptr->ps_iface_ptr);
            if(rex_get_timer(conn_delay_timer) == 0)
            {
              dssnet6i_transition_state(instance_ptr, DSSNET6I_CLOSED_STATE);
            }
          }
          else
#endif /* FEATURE_DATA_IS707 */
          {
            dssnet6i_transition_state(instance_ptr, DSSNET6I_CLOSED_STATE);
          }
        }
        else
        {
          (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                     IFACE_FLOW_DELETED_EV,
                                     instance_ptr->iface_flow_deleted_cbuf);

#ifdef FEATURE_HDR_QOS
          if(instance_ptr->use_ppp)
          {
            /* silently cleanup all secondary flows */
            ds707_sec_pkt_mgr_cleanup_all_secondary_flows(
                                                 instance_ptr->ps_iface_ptr, 
                                                 FALSE);
          }
#endif /* FEATURE_HDR_QOS */
        }
      }
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DSSNet6 Invalid  %d state",(int)instance_ptr->state);
      ASSERT(0);
      break;
  } /* end switch */
/*lint -restore Restore lint error 655*/
/*lint -restore Restore lint error 641*/
} /* dssnet6i_process_phy_link_gone_ev() */

/*===========================================================================
FUNCTION DSSNET6I_PROCESS_PHY_LINK_DOWN_EV()

DESCRIPTION
  This function processes the DSSNET6_PHY_LINK_DOWN_EV. It causes a 
  transition to the CLOSED STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_phy_link_down_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
  ds3g_pkt_call_info_type  *pkt_call_info_ptr = NULL;
  int16                     p_errno;
  int                       ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "PHY_LINK_DOWN_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

    /*---------------------------------------------------------------------------
    Try to bringup the phys link again if this flag is set. This will be set
    when there is a call bringup attempt when phys link is still being cleaned
    up. After the phys link is torn down completely an attempt to bringup the
    phys link again needs to be tried.
  ---------------------------------------------------------------------------*/
  if (dssnet6_get_is_bringup_phys_link(instance_ptr))
  {
    dssnet6_set_is_bringup_phys_link(instance_ptr, FALSE);

    /*------------------------------------------------------------------------
      pass valid value as user info to phys_link_up_cmd. Create a struct of
      type ds3g_pkt_call_info_type and pass it as user_info. this  function
      populates pkt_call_info and returns void*.
    ------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707    
    pkt_call_info_ptr = ds707_pkt_mgr_setup_pkt_call_info(
                                                   instance_ptr->ps_iface_ptr);
#endif /* FEATURE_DATA_IS707 */

    ret_val = ps_iface_phys_link_up_cmd ( instance_ptr->ps_iface_ptr,
                                          &p_errno,
                                          pkt_call_info_ptr );
    if (!((ret_val < 0) && (p_errno != DS_EWOULDBLOCK)))
    {
      /*---------------------------------------------------------------------
        Phys link up cmd successful. No other dssnet state transition needs
        to be done after this
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG0(MSG_LEGACY_MED, "phy link up cmd successful");
      return;
    }
  }

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:

      /* Stop the DHCP instance before giving down indication
       */
#ifdef FEATURE_DATA_IS707
      (void) dshdr_stop_dhcp(instance_ptr->ps_iface_ptr);
#endif /* FEATURE_DATA_IS707 */
      dssnet6i_transition_state(instance_ptr,DSSNET6I_CLOSED_STATE);

      break;

    case DSSNET6I_NET_UP_STATE:
    case DSSNET6I_LINK_CLOSING_STATE:
    case DSSNET6I_LINK_RESYNC_STATE:
    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
    case DSSNET6I_LINK_OPENING_STATE:
    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
      /*----------------------------------------------------------------------- 
       Once DSSNET6 moves to Link Opening , we must have deregistered the
       PHY_LINK_DOWN_EV.
      -----------------------------------------------------------------------*/
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Got unexpected PHY_LINK_DOWN_EV for DSSNet6 0x%x in %d state",
                      instance_ptr,
                      instance_ptr->state);
      break;

      
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DSSNet6 Invalid  %d state",(int)instance_ptr->state);
      ASSERT(0);
      break;
  } /* end switch */
/*lint -restore Restore lint error 655*/
/*lint -restore Restore lint error 641*/
} /* dssnet6i_process_phy_link_down_ev() */

/*===========================================================================
FUNCTION DSSNET6I_PROCESS_LINK_UP_EV()

DESCRIPTION
  This function processes the DSSNET6_LINK_UP_EV. Changes state to 
  DSSNET6I_WAITING_FOR_NET_UP_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_link_up_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "LINK_UP_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:
      /*---------------------------------------------------------------------
         Due to race condition, there is a possiblity that Phys link gone ev
         and PPP link up ev can be posted in this particular order. When
         DSSNET processes phys link gone ev first, it transitions the SM to
         closed state and posts iface down and cleans up the associated PDN
         When DSSNET processes PPP link up ev here in closed state,
         it ends up moving a stale dssnet SM to wait for Net up state which is
         incorrect. Hence check for iface state here before proceeding further.
      ---------------------------------------------------------------------*/
      if ( instance_ptr->use_ppp == TRUE &&
           instance_ptr->state == DSSNET6I_CLOSED_STATE &&
           (ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_DOWN || 
            ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_GOING_DOWN)
          )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                        "Link up ev recd when DSSNET SM in closed "
                        "state and iface is in down/going down state - "
                        "Ignore");
        break;
      }
      /* Else - Fall through */

    case DSSNET6I_LINK_OPENING_STATE:
    case DSSNET6I_LINK_RESYNC_STATE:
      dssnet6i_transition_state(instance_ptr,
                                DSSNET6I_WAITING_FOR_NET_UP_STATE);      
      break;

    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
    case DSSNET6I_NET_UP_STATE:
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "DSSNet6 0x%x got unexpected LINK_UP_EV in %d state",
                      instance_ptr,
                      instance_ptr->state);
      ASSERT(0);
      break;

    case DSSNET6I_LINK_CLOSING_STATE:
    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                      "DSSNet6 0x%x ignoring LINK_UP_EV in %d state",
                      instance_ptr,
                      instance_ptr->state);
      break;

    default:
      break;
  } /* end switch */

} /* dssnet6i_process_link_up_ev() */

/*===========================================================================
FUNCTION DSSNET6I_PROCESS_LINK_DOWN_EV()

DESCRIPTION
  This function processes the DSSNET6_LINK_DOWN_EV. Changes state to 
  DSSNET6I_CLOSED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_link_down_ev
(
  dssnet6_sm_cb_type *instance_ptr,            /* dssnet6 instance ptr      */
  ds_v6_call_status_type *call_status
)
{
#ifdef FEATURE_DATA_IS707
  rex_timer_type *conn_delay_timer;
  ppp_fail_reason_e_type reason = PPP_FAIL_REASON_NONE;
  ppp_protocol_e_type    protocol = PPP_PROTOCOL_LCP;
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif 
#endif /* FEATURE_DATA_IS707 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((NULL == instance_ptr) || (NULL == call_status))
  {
    DATA_ERR_FATAL("Input parameters are NULL");
    return;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "LINK_DOWN_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

  if ( call_status->call_type == DS_CALL_CATEGORY_NON_PPP_TYPE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                    "LINK_DOWN_EV recd for DSSNet6: call_status %d", 
                    call_status->call_end_reason_info.call_status);
  }
#ifdef FEATURE_DATA_IS707
  else if ( call_status->call_type == DS_CALL_CATEGORY_PPP_TYPE )
  {
    reason = call_status->call_end_reason_info.ppp_status.ppp_fail_reason;
    protocol = call_status->call_end_reason_info.ppp_status.ppp_protocol;
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "LINK_DOWN_EV recd for DSSNet6: protocol %d, reason %d", 
                    protocol, reason);
  }
#endif

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:
      /*-------------------------------------------------------------------
      Enter crit sect to prevent any post proc on this iface 
      while down ind is being posted here
      To avoid any deadlock between DS task and other tasks running Routing,
      acquire the global_ps_crit_section. This ensures that both DS task
      and any other task running Routing and picking up 3GPP iface obtain
      critical sections in the same order 
      This is applicable only for 3GPP. 
      ------------------------------------------------------------------*/
      if (FALSE == instance_ptr->use_ppp)
      {
        /*---------------------------------------------------------------------
          handle LINK_DOWN_EV.
        ---------------------------------------------------------------------*/
        DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                        "DSSNet6 0x%x LINK_DOWN_EV in %d state. Post iface down",
                        instance_ptr,
                        instance_ptr->state);

        if ( call_status->call_type == DS_CALL_CATEGORY_NON_PPP_TYPE )
        {
          dssnet6_set_ps_call_end_reason(instance_ptr, 
          (ps_iface_net_down_reason_type)call_status->call_end_reason_info.call_status);
        }

        PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
        dssnet6_enter_crit_sect();
        if(instance_ptr->iface_down_ind_cb != NULL)
        {
          DATA_MSG3(MSG_LEGACY_LOW,
                    "V6 iface 0x%x:%d (0x%x) is down. Calling iface_down_ind_cb",
                    instance_ptr->ps_iface_ptr->name,
                    instance_ptr->ps_iface_ptr->instance,
                    instance_ptr->ps_iface_ptr);
          instance_ptr->iface_down_ind_cb(instance_ptr->ps_iface_ptr,
                                          instance_ptr->call_end_reason);
        }
        dssnet6_leave_crit_sect();
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

        /*---------------------------------------------------------------------
          Reset call end reason here since it is not done in iface down 
          indication callback for 3GPP
        ---------------------------------------------------------------------*/
        dssnet6_reset_call_end_reason(instance_ptr);
        dssnet6_reset_local_abort(instance_ptr);
      }

      else
      {
        /*---------------------------------------------------------------------
          Ignore LINK_DOWN_EV.
        ---------------------------------------------------------------------*/
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "DSSNet6 0x%x Ignore LINK_DOWN_EV in CLOSED state",
                        instance_ptr);
      }
      break;

    case DSSNET6I_NET_UP_STATE:
    case DSSNET6I_LINK_RESYNC_STATE:
    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
      /*---------------------------------------------------------------------
        Exit IPv6.
      ---------------------------------------------------------------------*/
      if (ip6_sm_post_event(instance_ptr->ip6_instance, IP6_SM_ABORT_EV, NULL) 
          == -1)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                        "Posting ip6_sm_post_event() %d failed returned -1",
                        IP6_SM_ABORT_EV);
        ASSERT(0);
      }

      /*--------------------------------------------------------------------- 
        Set Call end reason here as we would return from this function
        after changing DSSNET6 state.
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
      if (DS_CALL_CATEGORY_PPP_TYPE == call_status->call_type)
      {
        dssnet6_set_ps_call_end_reason(instance_ptr,
                                     dssnet6i_convert_ppp_failure_code(reason));
      }
      else
#endif /* FEATURE_DATA_IS707 */ 
      if (DS_CALL_CATEGORY_NON_PPP_TYPE == call_status->call_type)
      {
        dssnet6_set_ps_call_end_reason(instance_ptr, 
         (ps_iface_net_down_reason_type)call_status->call_end_reason_info.call_status);
      }
      
      dssnet6i_transition_state(instance_ptr,
                                DSSNET6I_WAITING_FOR_NET_DOWN_STATE);

      break;

    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
      /*---------------------------------------------------------------------
          This is possibly a scenario, when Client has initiated a call
          end but, before IP6 SM can post NET_DOWN, the physlink
          was terminated by the network. In such cases, clean up
          the DSSNET6 SM
        ---------------------------------------------------------------------*/
        DATA_3GMGR_MSG1(MSG_LEGACY_MED,"DSSNet6 0x%x Rx'ed LINK_DOWN_EV in"
                        " WAITING FOR NET DOWN state",instance_ptr);
      /* Fall through */
    case DSSNET6I_LINK_OPENING_STATE:
#ifdef FEATURE_DATA_IS707
      if ( call_status->call_type == DS_CALL_CATEGORY_PPP_TYPE )
      {
#ifdef FEATURE_CALL_THROTTLE 
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        {
          /*-------------------------------------------------------------------
            DCTM for AUTH/LCP is handled in device manger, ignore here
          -------------------------------------------------------------------*/ 
          if( instance_ptr->is_ehrpd_call == FALSE )
          {
            if( (reason == PPP_FAIL_REASON_TIMEOUT) || 
                (reason == PPP_FAIL_REASON_OPTION_MISMATCH) )
            {
              DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                              "Posting PPP_FAILURE_EV with protocol %d, "
                              "reason %d to DCTM", protocol, reason);
              dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                         (dsdctm_ppp_protocol_e_type)protocol,
                                         reason );
              ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
            }
            else if ( (reason == PPP_FAIL_REASON_AUTH_FAILURE) || 
                      (reason == PPP_FAIL_REASON_AUTH_PAP_FAILURE) || 
                      (reason == PPP_FAIL_REASON_AUTH_CHAP_FAILURE)
                    )
            {
              DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                              "Posting PPP_FAILURE_EV with DSDCTM_PPP_PROTOCOL_AUTH to DCTM");
              dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                         DSDCTM_PPP_PROTOCOL_AUTH, 
                                         reason );
              ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
            }
          }
        }
#endif /* FEATURE_CALL_THROTTLE */
      }
#endif /* FEATURE_DATA_IS707 */

    /* Fall through */

    case DSSNET6I_LINK_CLOSING_STATE:
      {
#ifdef FEATURE_DATA_IS707
        if (DS_CALL_CATEGORY_PPP_TYPE == call_status->call_type)
        {
          dssnet6_set_ps_call_end_reason(instance_ptr,
                                     dssnet6i_convert_ppp_failure_code(reason));
        }
        else
#endif /* FEATURE_DATA_IS707 */ 
        if (DS_CALL_CATEGORY_NON_PPP_TYPE == call_status->call_type)
        {
          dssnet6_set_ps_call_end_reason(instance_ptr, 
          (ps_iface_net_down_reason_type)call_status->call_end_reason_info.call_status);
        }

        if (PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) == 0)
        {
          /*---------------------------------------------------------------------
          If conn delay timer is running, ignore this event. When the timer 
          expires, the dssnet state will be transitioned
        ---------------------------------------------------------------------*/ 
#ifdef FEATURE_DATA_IS707
          if ( PS_IFACE_GET_NAME_FROM_ID( PS_IFACE_GET_ID( instance_ptr->ps_iface_ptr ) )
               == CDMA_SN_IFACE )
          {         
            conn_delay_timer = 
                (rex_timer_type *)ds707_pkt_get_conn_delay_timer_from_iface_ptr(
                    instance_ptr->ps_iface_ptr);
            if(rex_get_timer(conn_delay_timer) == 0)
            {
              dssnet6i_transition_state(instance_ptr, DSSNET6I_CLOSED_STATE);
            }
          }
          else
#endif /* FEATURE_DATA_IS707 */
          {
            dssnet6i_transition_state(instance_ptr, DSSNET6I_CLOSED_STATE);
          }
        }
        else
        {
          (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                     IFACE_FLOW_DELETED_EV,
                                     instance_ptr->iface_flow_deleted_cbuf);

#ifdef FEATURE_HDR_QOS
          if(instance_ptr->use_ppp)
          {
            /* post a command to sec pkt mgr informing of PPP down event */
            ds707_sec_pkt_network_initiated_ppp_down_cmd(
                                             instance_ptr->ps_iface_ptr);
          }
#endif /* FEATURE_HDR_QOS */

        }

        break;
      }

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid state %d",
                      (int)instance_ptr->state);
      ASSERT(0);
  } /* end switch */
/*lint -restore Restore lint error 641*/
} /* dssnet6i_process_link_down_ev() */



/*===========================================================================
FUNCTION DSSNET6I_PROCESS_NET_UP_EV()

DESCRIPTION
  This function processes the DSSNET6_NET_UP_EV. Changes state to 
  DSSNET6I_NET_UP_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_net_up_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif 
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "NET_UP_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:
    case DSSNET6I_LINK_OPENING_STATE:
    case DSSNET6I_NET_UP_STATE:
    case DSSNET6I_LINK_RESYNC_STATE:
    case DSSNET6I_LINK_CLOSING_STATE:
    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x ignoring NET_UP_EV in %d state",
                      instance_ptr,
                      instance_ptr->state);
      break;

    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_CALL_THROTTLE
      if( instance_ptr->use_ppp )
      {
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_MED, "Posting DSSNET6_NET_UP_EV to DCTM");
          dctm_post_v6_call_status( DSDCTM_DSSNET6_NET_UP_EV );
          ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
        }
      }
#endif /* FEATURE_CALL_THROTTLE */
#endif /* FEATURE_DATA_IS707 */
      if (instance_ptr->event_cb != NULL)
      {
        instance_ptr->event_cb(instance_ptr,
                               DSSNET6_NET_UP_EV,
                               instance_ptr->state, 
                               instance_ptr->event_cb_user_data);
      }
      dssnet6i_transition_state(instance_ptr, DSSNET6I_NET_UP_STATE);      
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -save -e506, -e774 */
      ASSERT(0);
      /*lint -restore */
  } /* end switch */

} /* dssnet_process_net_up_ev() */



/*===========================================================================
FUNCTION DSSNET6I_PROCESS_NET_DOWN_EV()

DESCRIPTION
  This function processes the DSSNET6_NET_DOWN_EV. Changes state to 
  DSSNET6I_LINK_CLOSING_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_net_down_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
  int16 ps_errno;
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
  ds707_data_session_profile_info_type *pdn_prof_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif 
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "NET_DOWN_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x got unexpected NET_DOWN_EV in %d state",
                      instance_ptr,
                      instance_ptr->state);
      break;

    case DSSNET6I_LINK_RESYNC_STATE:
    case DSSNET6I_LINK_CLOSING_STATE:
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x ignoring NET_DOWN_EV in %d state",
                      instance_ptr,
                      instance_ptr->state);
      break;

    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_CALL_THROTTLE
      if( instance_ptr->use_ppp )
      {
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Posting DSSNET6_NET_DOWN_EV to DCTM");
          dctm_post_v6_call_status( DSDCTM_DSSNET6_NET_DOWN_EV );
          ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
        }
      }
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
      /*-----------------------------------------------------------------------
        Get the requesting profile id and check if the pdn is throttled. If 
        not allocate a new throttle table entry for this PDN.
      -----------------------------------------------------------------------*/
      if ( instance_ptr->is_ehrpd_call == TRUE ) 
      {
        if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                    instance_ptr->ps_iface_ptr,
                                                    &pdn_prof_ptr) )
        {
           ds707_data_session_get_iface_current_profile(instance_ptr->ps_iface_ptr,
                                                        &pdn_prof_ptr);
        }

        if ( pdn_prof_ptr != NULL )
        {
          ds707_pdncntx_throttle_pdn_iptype( pdn_prof_ptr->data_session_profile_id, 
                                             DS707_PDNCNTX_IP_TYPE_V6,
                                             DS707_PDNCNTX_THROTTLE_TYPE_FAILURE,
                                             PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE);
        }
      }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_DATA_IS707 */

    case DSSNET6I_NET_UP_STATE:
      dssnet6_set_ps_call_end_reason(instance_ptr, 
                                   PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE);
      /* Fall through*/

    case DSSNET6I_LINK_OPENING_STATE:

      /*---------------------------------------------------------------------
        Transition to DSSNET6I_LINK_CLOSING_STATE
        ---------------------------------------------------------------------*/ 
      if (PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) != 0)
      {
        (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                      IFACE_FLOW_DELETED_EV,
                                      instance_ptr->iface_flow_deleted_cbuf);
      }      
      /* call registered callback if exists */
      if (instance_ptr->event_cb != NULL)
      {
        instance_ptr->event_cb(instance_ptr,
                               DSSNET6_NET_DOWN_EV,
                               instance_ptr->state, 
                               instance_ptr->event_cb_user_data);
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        If not using PPP, initiate iface tear down explicitly before 
        transition to link closing state. If using PPP, transition to link
        closing state will tear down iface and do necessary cleanup. 
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      if (!instance_ptr->use_ppp)
      {
        (void) ps_iface_go_null_cmd(instance_ptr->ps_iface_ptr, 
                                    &ps_errno, 
                                    NULL);
      }
      dssnet6i_transition_state(instance_ptr, 
                                DSSNET6I_LINK_CLOSING_STATE);

      break;

    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE: 

      /*---------------------------------------------------------------------
        Transition to DSSNET6I_LINK_CLOSING_STATE
        ---------------------------------------------------------------------*/ 
      if (PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) != 0)
      {
        (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                      IFACE_FLOW_DELETED_EV,
                                      instance_ptr->iface_flow_deleted_cbuf);
      }      
      /* call registered callback if exists */
      if (instance_ptr->event_cb != NULL)
      {
        instance_ptr->event_cb(instance_ptr,
                               DSSNET6_NET_DOWN_EV,
                               instance_ptr->state, 
                               instance_ptr->event_cb_user_data);
      }

      dssnet6i_transition_state(instance_ptr, 
                                DSSNET6I_LINK_CLOSING_STATE);      

      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -save -e506, -e774 */
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Invalid state %d",(int)instance_ptr->state);
      ASSERT(0);
      /*lint -restore */
  } /* end switch */

} /* dssnet6i_process_net_down_ev() */
/*===========================================================================
FUNCTION DSSNET6I_PROCESS_NET_ABORTED_EV()

DESCRIPTION
  This function processes the DSSNET6_NET_ABORTED_EV. Changes state to 
  DSSNET6I_LINK_CLOSING_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_net_aborted_ev
(
  dssnet6_sm_cb_type      *instance_ptr,      /* dssnet6 instance ptr      */
  ds_v6_call_status_type  *call_status
)
{

#ifdef FEATURE_DATA_IS707
  rex_timer_type *conn_delay_timer;
  ppp_fail_reason_e_type reason = PPP_FAIL_REASON_NONE;
  ppp_protocol_e_type    protocol = PPP_PROTOCOL_LCP;
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type             *pdncntx_cb_ptr     = NULL;
#endif 
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "NET_ABORTED_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,instance_ptr->state);

  switch (instance_ptr->state)
  {
    case DSSNET6I_LINK_RESYNC_STATE:
    case DSSNET6I_CLOSED_STATE:
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x got unexpected NET_ABORTED_EV in %d state",
                      instance_ptr,instance_ptr->state);
      break;

    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
      dssnet6_set_ps_call_end_reason(instance_ptr,(ps_iface_net_down_reason_type)
                                 call_status->call_end_reason_info.call_status);
      /*----------------------------------------------------------------------- 
         call registered callback if exists
        -----------------------------------------------------------------------*/
    if (instance_ptr->event_cb != NULL)
    {
      instance_ptr->event_cb(instance_ptr,
                             DSSNET6_NET_ABORTED_EV,
                             instance_ptr->state, 
                             instance_ptr->event_cb_user_data);
    }



    case DSSNET6I_LINK_OPENING_STATE:
 #ifdef FEATURE_DATA_IS707
      if ( call_status->call_type == DS_CALL_CATEGORY_PPP_TYPE )
      {
#ifdef FEATURE_CALL_THROTTLE 
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr);

        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        {
          /*------------------------------------------------------------------- 
            DCTM for AUTH/LCP is handled in device manger, ignore here
            -------------------------------------------------------------------*/ 
          if( instance_ptr->is_ehrpd_call == FALSE )
          {
            if( (reason == PPP_FAIL_REASON_TIMEOUT) || 
                (reason == PPP_FAIL_REASON_OPTION_MISMATCH) )
            {
              DATA_3GMGR_MSG2(MSG_LEGACY_MED, "Posting PPP_FAILURE_EV with"
                              "protocol %d, reason %d to DCTM", protocol, reason);

              dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                          (dsdctm_ppp_protocol_e_type)protocol,
                                          reason );
              ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
            }
            else if ( (reason == PPP_FAIL_REASON_AUTH_FAILURE)     || 
                      (reason == PPP_FAIL_REASON_AUTH_PAP_FAILURE) || 
                      (reason == PPP_FAIL_REASON_AUTH_CHAP_FAILURE)
                     )
             {
               DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                               "Posting PPP_FAILURE_EV with DSDCTM_PPP_PROTOCOL_AUTH to DCTM");
               dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                          DSDCTM_PPP_PROTOCOL_AUTH, 
                                          reason );
               ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
             }
           }
        }
 #endif /* FEATURE_CALL_THROTTLE */
      }
 #endif /* FEATURE_DATA_IS707 */

     /* Fall through */

     case DSSNET6I_LINK_CLOSING_STATE:
       {
 #ifdef FEATURE_DATA_IS707
         if (DS_CALL_CATEGORY_PPP_TYPE == call_status->call_type)
         {
           dssnet6_set_ps_call_end_reason(instance_ptr,
                                      dssnet6i_convert_ppp_failure_code(reason));
         }

         else if (DS_CALL_CATEGORY_NON_PPP_TYPE == call_status->call_type)
         {
           dssnet6_set_ps_call_end_reason(instance_ptr, 
           (ps_iface_net_down_reason_type)call_status->call_end_reason_info.call_status);
         }
 #endif
         if (PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) == 0)
         {
#ifdef FEATURE_DATA_IS707
           /*---------------------------------------------------------------- 
             If conn delay timer is running, ignore this event. When the timer
             expires, the dssnet state will be transitioned
             ----------------------------------------------------------------*/ 
 
           if ( PS_IFACE_GET_NAME_FROM_ID( PS_IFACE_GET_ID( instance_ptr->ps_iface_ptr ) )
                == CDMA_SN_IFACE )
           {         
             conn_delay_timer = 
                 (rex_timer_type *)ds707_pkt_get_conn_delay_timer_from_iface_ptr(
                     instance_ptr->ps_iface_ptr);
             if(rex_get_timer(conn_delay_timer) == 0)
             {
               dssnet6i_transition_state(instance_ptr, DSSNET6I_CLOSED_STATE);
             }
           }
           else
 #endif /* FEATURE_DATA_IS707 */
           {
             dssnet6i_transition_state(instance_ptr, DSSNET6I_CLOSED_STATE);
           }
         }
         else
         {
           (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                      IFACE_FLOW_DELETED_EV,
                                      instance_ptr->iface_flow_deleted_cbuf);

 #ifdef FEATURE_HDR_QOS
           if(instance_ptr->use_ppp)
           {
             /*-------------------------------------------------------------- 
                post a command to sec pkt mgr informing of PPP down event
               --------------------------------------------------------------*/
             ds707_sec_pkt_network_initiated_ppp_down_cmd(
                                              instance_ptr->ps_iface_ptr);
           }
 #endif /* FEATURE_HDR_QOS */

         }        
         break;
       }
    /*----------------------------------------------------------------------- 
      NET_UP_STATE (or) WAITING FOR NET_UP STATE
      -----------------------------------------------------------------------*/
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Invalid state %d while processing NetAbort Ev",
                      (int)instance_ptr->state);
      ASSERT(0);
      break;
  }

}


/*===========================================================================
FUNCTION DSSNET6I_PROCESS_PPP_CLOSE_CMD_EV()

DESCRIPTION
  This function processes the DSSNET6_PPP_CLOSE_CMD_EV. Changes state to 
  DSSNETI_LINK_CLOSING_STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_ppp_close_cmd_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "PPP_CLOSE_CMD_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:
      /*------------------------------------------------------------------- 
         If we are using PPP, we will move to move to waiting_for_link_up
         state immediately from DSSNET6I_CLOSED_STATE on call origination
         In UMTS, there is no DSSNET6I_LINK_OPENING_STATE state, so we   
         receive PPP_CLOSE_CMD_EV in DSSNET6I_CLOSED_STATE. No need to
         tear down IFACE here; cleanup will be done when link down event 
         is received.                
      -------------------------------------------------------------------*/
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x PPP_CLOSE_CMD_EV received in %d state",
                      instance_ptr,
                      instance_ptr->state);
      if (instance_ptr->use_ppp) 
      {
        dssnet6i_transition_state(instance_ptr,
                                  DSSNET6I_CLOSED_STATE);  
      }
      break;

     /*---------------------------------------------------------------------- 
        This is possibly a error handling scenario, where Network has
        terminated the physlink but before the iface's can be cleaned up
        and DSSNET6 SM is awaiting response from IPV6 SM, Mode handler
        receives a call end from the clients resulting in dssnet_stop()
        However, in such cases, since a IPV6 has been previously informed
        to clean up, we can skip sending stop event to ipv6 SM again
       ----------------------------------------------------------------------*/
     case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
       DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                       "DSSNet6 0x%x PPP_CLOSE_CMD_EV received in %d state."
                       "Ignore",instance_ptr,instance_ptr->state);
     break;

    case DSSNET6I_LINK_CLOSING_STATE:
      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                      "DSSNet6 0x%x ignoring PPP_CLOSE_CMD_EV in %d state", 
                      instance_ptr,
                      instance_ptr->state);
      break;

    case DSSNET6I_LINK_RESYNC_STATE:
    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
    case DSSNET6I_NET_UP_STATE:
      /*---------------------------------------------------------------------
        Exit IPv6.
      ---------------------------------------------------------------------*/ 
      if (ip6_sm_post_event(instance_ptr->ip6_instance, IP6_SM_STOP_EV, NULL) 
          == -1)
      {
        DATA_ERR_FATAL("ip6_sm_post_event() failed returned -1");
      }
      dssnet6i_transition_state(instance_ptr,
                                DSSNET6I_WAITING_FOR_NET_DOWN_STATE);
      break;
      /* Fall Through */

    case DSSNET6I_LINK_OPENING_STATE:
      /*---------------------------------------------------------------------
        Transition to DSSNET6I_LINK_CLOSING_STATE
      ---------------------------------------------------------------------*/ 
      if (PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) != 0)
      {
        (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                      IFACE_FLOW_DELETED_EV,
                                      instance_ptr->iface_flow_deleted_cbuf);
      }

      dssnet6i_transition_state(instance_ptr,
                                DSSNET6I_LINK_CLOSING_STATE);      
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Transition state %d",(int)instance_ptr->state);

      ASSERT(0);

  } /* end switch */

} /* dssnet6i_process_ppp_close_cmd_ev() */



/*===========================================================================
FUNCTION DSSNET6I_PROCESS_LINK_RESYNC_EV()

DESCRIPTION
  This function processes the DSSNET_LINK_RESYNC_EV event. Changes State
  to DSSNET6I_LINK_RESYNC_STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_process_link_resync_ev
(
  dssnet6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "LINK_RESYNC_EV recd for DSSNet6 0x%x in state %d",
                  instance_ptr,
                  instance_ptr->state);

  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:
    case DSSNET6I_LINK_OPENING_STATE:
    case DSSNET6I_LINK_CLOSING_STATE:
    case DSSNET6I_LINK_RESYNC_STATE:
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x ignoring LINK_RESYNC_EV in %d state",
                      instance_ptr,
                      instance_ptr->state);
      break;

    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
    case DSSNET6I_NET_UP_STATE:
       dssnet6i_transition_state(instance_ptr, DSSNET6I_LINK_RESYNC_STATE);

      break;

    default:
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid state");
      break;
  } /* end switch */

} /* dssnet_process_link_resync_ev() */

/*===========================================================================
FUNCTION DSSNET6I_HANDLE_PPP_EV_CBACK()

DESCRIPTION
  Called when the PPP comes up/down/resync.  This posts an appropriate event
  to the dssnet6 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_handle_ppp_ev_cback
(
  ppp_dev_enum_type       ppp_dev,               /* PPP Device             */
  ppp_protocol_e_type     protocol,              /* PPP Protocol           */
  ppp_event_e_type        ppp_event,             /* Down/Up/Resync         */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{

  dssnet6_sm_cb_type    *instance_ptr;
#ifdef FEATURE_DATA_IS707
  ds_v6_call_status_type   call_status;
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
  ds707_pdncntx_ppp_event_action event_action = DS707_PDNCNTX_INVALID_ACTION;
  ds707_pdn_context_cb_type     *pdncntx_cb_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
  ds707_data_session_profile_info_type *profile_info_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_DATA_IS707 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH, "Recd ppp dev %d event %d for DSSNet6 0x%x",
                  ppp_dev, ppp_event, user_data );

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,"ppp event %d, protocol %d, reason %d",
                  ppp_event, protocol, fail_reason );

  instance_ptr = (dssnet6_sm_cb_type*)(user_data);

  ASSERT(ppp_dev == PPP_UM_SN_DEV);
  switch (ppp_event)
  {
    case PPP_UP_EV:
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_LINK_UP_EV, NULL);
      break;

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
    case PPP_SUBS_LIMITED_TO_V6_EV:
      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
      if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
      {
        DATA_ERR_FATAL("Cannot get PDN Control Block, do not proceed");
      }

      event_action = ds707_pdncntx_handle_ppp_ev( pdncntx_cb_ptr, 
                                                  instance_ptr->ps_iface_ptr,
                                                  ppp_event );
 
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW, "Event action %d for dssnet6 %x ",
                      event_action, instance_ptr);

      if ( event_action == DS707_PDNCNTX_LINK_UP )
      {
        (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_LINK_UP_EV, NULL);
        break;
      }
      else if ( event_action == DS707_PDNCNTX_PPP_CLOSE )
      {
        /*lint -save -e534 return value not required*/
        (void)dssnet6_sm_post_event( instance_ptr,
                                     DSSNET6_PPP_CLOSE_CMD_EV, NULL);
        /*lint -restore*/
      } /* event_action == DS707_PDNCNTX_PPP_CLOSE */
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Event action %d not supported for "
                        "PPP_SUBS_LIMITED_TO_V6_EV" ,event_action);
        ASSERT(0);        
      }
      break;

    case PPP_SUBS_LIMITED_TO_V4_EV:
      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
      if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
      {
        DATA_ERR_FATAL("Cannot get PDN Control Block, do not proceed");
      }

#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
      /*-----------------------------------------------------------------------
        Get the requesting profile id and check if the pdn is throttled. If 
        not allocate a new throttle table entry for this PDN.
      -----------------------------------------------------------------------*/
      if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                  instance_ptr->ps_iface_ptr,
                                                  &profile_info_ptr) )
      {
        ds707_data_session_get_iface_current_profile(instance_ptr->ps_iface_ptr,
                                                     &profile_info_ptr);
      }
      if ( profile_info_ptr != NULL )
      {
        ds707_pdncntx_throttle_pdn_iptype( profile_info_ptr->data_session_profile_id, 
                                           DS707_PDNCNTX_IP_TYPE_V6,
                                           DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW,
                                           PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V4);  
      }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */

      event_action = ds707_pdncntx_handle_ppp_ev( pdncntx_cb_ptr, 
                                                  instance_ptr->ps_iface_ptr,
                                                  ppp_event );

      DATA_3GMGR_MSG2(MSG_LEGACY_MED,"Event action %d for dssnet6 %x ",
                      event_action, instance_ptr);

      dssnet6_set_ps_call_end_reason(instance_ptr, 
                                     PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V4);
      if ( event_action == DS707_PDNCNTX_PPP_CLOSE )
      {
        (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_PPP_CLOSE_CMD_EV, NULL);
      }
      else if( event_action == DS707_PDNCNTX_LINK_DOWN )
      {
        call_status.call_type = DS_CALL_CATEGORY_PPP_TYPE;
        call_status.call_end_reason_info.ppp_status.ppp_protocol = protocol;
        call_status.call_end_reason_info.ppp_status.ppp_fail_reason = fail_reason;
        call_status.call_end_reason_info.ppp_status.dssnet_instance = instance_ptr;
        (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_LINK_DOWN_EV, &call_status);
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Event action %d not supported for "
                        "PPP_SUBS_LIMITED_TO_V4_EV", event_action);
        ASSERT(0);        
      }
      break;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */

    /* case PPP_FAILURE_EV: */
    case PPP_DOWN_EV:
#ifdef FEATURE_EHRPD
      /*---------------------------------------------------------------------
        Store the PPP fail reason if it already not set in case of AUTH 
        failure on eHRPD. No other processing is required for AUTH case
      ----------------------------------------------------------------------*/ 
      if ((TRUE == instance_ptr->is_ehrpd_call) 
          && (PPP_PROTOCOL_AUTH == protocol))
      {
        dssnet6_set_ps_call_end_reason(instance_ptr,
                               dssnet6i_convert_ppp_failure_code(fail_reason));
        return;
      }
#endif
    /* fall through */

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
      if ( ( instance_ptr->is_ehrpd_call == TRUE ) && 
           ( (instance_ptr->state == DSSNET6I_LINK_OPENING_STATE) ||
             (instance_ptr->state == DSSNET6I_WAITING_FOR_NET_UP_STATE) ) )
      {
        dssnet6_set_ps_call_end_reason( instance_ptr, 
                                 dssnet6i_convert_ppp_failure_code(fail_reason) );
        if( (fail_reason == PPP_FAIL_REASON_VSNCP_TIMEOUT) || 
            (fail_reason == PPP_FAIL_REASON_VSNCP_FAILURE) ||
            ( (fail_reason >= PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MIN) &&
              (fail_reason < PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MAX)  &&
              (fail_reason != PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_ID_IN_USE) &&
              (fail_reason != PPP_FAIL_REASON_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED) &&
              ( !((fail_reason == PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN) &&
                (FALSE == ds707_pdncntx_is_default_apn(profile_info_ptr))) )
            )
          )
        {
          if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                    instance_ptr->ps_iface_ptr,
                                                    &profile_info_ptr) )
          {
            ds707_data_session_get_iface_current_profile(instance_ptr->ps_iface_ptr,
                                                         &profile_info_ptr);
          }
          if ( profile_info_ptr != NULL )
          {
            /*-----------------------------------------------------------------------
              Trigger throttling
            -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_FALLBACK_LIST    
              if(!ds707_pkt_mgr_fb_list_fallback(
                           instance_ptr->ps_iface_ptr,
                           profile_info_ptr->apn_string, 
                           profile_info_ptr->apn_string_len)
                 )
#endif /*FEATURE_DATA_FALLBACK_LIST*/
              {
                /*-----------------------------------------------------------------------
                  Throttle only if we get PPP_DOWN in link_opening or waiting_for_net_up state
                  and no PDN fallback is proposed.
                  Get the requesting profile id and check if the pdn is throttled. If 
                  not allocate a new throttle table entry for this PDN.
                -----------------------------------------------------------------------*/
                ds707_pdncntx_throttle_pdn_iptype( profile_info_ptr->data_session_profile_id, 
                                                   DS707_PDNCNTX_IP_TYPE_V6,
                                                   DS707_PDNCNTX_THROTTLE_TYPE_FAILURE,
                                                   dssnet6i_convert_ppp_failure_code(fail_reason));  
              }
            }        
          }
        }
      /*-----------------------------------------------------------------------
        reconnection not allowed error: Trigger infinite throttling regardless
        of the link state
      -----------------------------------------------------------------------*/
      if (fail_reason == PPP_FAIL_REASON_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED)
      {
        if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                    instance_ptr->ps_iface_ptr,
                                                    &profile_info_ptr) )
        {
          ds707_data_session_get_iface_current_profile(instance_ptr->ps_iface_ptr,
                                                       &profile_info_ptr);
        }
        if ( profile_info_ptr != NULL )
        {
            DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                             "iface6 PPP DOWN with reconnect not allowed!");
            ds707_pdncntx_throttle_pdn_iptype( profile_info_ptr->data_session_profile_id, 
                                               DS707_PDNCNTX_IP_TYPE_V6,
                                               DS707_PDNCNTX_THROTTLE_TYPE_INFINITE,
                                               dssnet6i_convert_ppp_failure_code(fail_reason));
        }
      }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
      call_status.call_type = DS_CALL_CATEGORY_PPP_TYPE;
      call_status.call_end_reason_info.ppp_status.ppp_protocol = protocol;
      call_status.call_end_reason_info.ppp_status.ppp_fail_reason = fail_reason;
      call_status.call_end_reason_info.ppp_status.dssnet_instance = instance_ptr;
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_LINK_DOWN_EV, &call_status);
      /*------------------------------------------------------------------- 
        Required for ehrpd-1x/hrpd handoff. If all ifaces are cleaned,
        move sys_info state to NO_SYSTEM. Next time call is brought up,
        it will be changed to EPC or NON_EPC depending on the system.
        Removed as part of CR 734709
      --------------------------------------------------------------------*/ 
      #if 0
      if ( protocol == PPP_PROTOCOL_LCP )
      {
        ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);
        DATA_3GMGR_MSG0(MSG_LEGACY_MED, "LCP down move sys_info to NO_SYSTEM");
      }
      #endif
#else
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_LINK_DOWN_EV, NULL);
#endif
      break;

    case PPP_RESYNC_EV:
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_LINK_RESYNC_EV, NULL);
      break;

    default:
      DATA_3GMGR_MSG3(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x got unsupported PPP event %d from dev %d",
                      instance_ptr,
                      ppp_event,
                      ppp_dev);
      ASSERT(0);
  }
} /* dssnet6i_handle_ppp_ev_cback() */


/*===========================================================================
FUNCTION DSSNET6I_HANDLE_PPP_AUTH_EV_CBACK()

DESCRIPTION
  Called when the auth fails or succeds.  This posts an appropriate event
  to the dssnet4 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_handle_ppp_auth_ev_cback
(
  ppp_dev_enum_type       ppp_dev,              /* PPP Device             */
  ppp_protocol_e_type     protocol,             /* PPP Protocol           */
  ppp_event_e_type        ppp_event,            /* SUCCESS/FAIL           */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
  dssnet6_sm_cb_type  *instance_ptr;           /* dssnet6 instance ptr      */
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif 
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG3(MSG_LEGACY_MED, "Recd ppp dev %d event %d for DSSNet6 0x%x",
                  ppp_dev, 
                  ppp_event, 
                  user_data );
  
  instance_ptr = (dssnet6_sm_cb_type*)(user_data);
  ASSERT(ppp_dev == PPP_UM_SN_DEV);

  switch(ppp_event)
  {
    case PPP_FAILURE_EV:
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "PPP Auth Failure");
      /*-------------------------------------------------------------------
        Transition to CLOSING state.
      -------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_CALL_THROTTLE
      if (instance_ptr->use_ppp)
      {
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        { 
          if( instance_ptr->is_ehrpd_call == FALSE)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_MED,"Posting PPP_FAILURE_EV with "
                            "DSDCTM_PPP_PROTOCOL_AUTH to DCTM");
            dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                       DSDCTM_PPP_PROTOCOL_AUTH, 
                                       PPP_FAIL_REASON_AUTH_FAILURE );
            ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
          }
        }
      }
#endif /* FEATURE_CALL_THROTTLE */
#endif /* FEATURE_DATA_IS707 */
      (void) dssnet6_sm_post_event( instance_ptr, 
                                    DSSNET6_PPP_CLOSE_CMD_EV, NULL);
      break;

    case PPP_SUCCESS_EV:
       DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "PPP Auth success");
       break;

   default:
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"unknown PPP event on UM for "
                       "protocol: %x", protocol);
   }
} /* dssnet6i_handle_ppp_auth_ev_cback() */


/*===========================================================================
FUNCTION DSSNET6I_HANDLE_PHYS_LINK_EV_CBACK()

DESCRIPTION
  Called when the PHYS LINK goes up/down/gone.  This posts an appropriate
  event to the dssnet state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_handle_phys_link_ev_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
)
{

  dssnet6_sm_cb_type    *instance_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  instance_ptr = (dssnet6_sm_cb_type*)(user_info_ptr);

  DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                  "Recd ps_iface event %d for DSSNet6 0x%x in state %d",
                  event,
                  user_info_ptr,
                  instance_ptr->state);

  switch (event)
  {
    case IFACE_PHYS_LINK_UP_EV: 
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_PHY_LINK_UP_EV, NULL);
      break;

    case IFACE_PHYS_LINK_DOWN_EV: 
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_PHY_LINK_DOWN_EV, NULL);
      break;

    case IFACE_PHYS_LINK_GONE_EV: 
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_PHY_LINK_GONE_EV, NULL);
      break;

    case IFACE_FLOW_DELETED_EV:
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_FLOW_DELETED_EV, NULL);
      break;

    case IFACE_ENABLED_EV: 
    case IFACE_DISABLED_EV: 
    case IFACE_DOWN_EV: 
    case IFACE_ROUTEABLE_EV: 
    case IFACE_UP_EV: 
    case IFACE_FLOW_ENABLED_EV: 
    case IFACE_FLOW_DISABLED_EV: 
    case IFACE_ADDR_CHANGED_EV: 
    case IFACE_DELETE_EV: 
    default:
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x got unsupported PHYS event %d",
                      instance_ptr,
                      event);
      ASSERT(0);
  }
} /* dssnet6i_handle_phys_link_ev_cback() */



/*===========================================================================
FUNCTION DSSNET6I_HANDLE_IP6_EV_CBACK()

DESCRIPTION
  Called when the IP6 comes up/down.  This posts an appropriate event to the
  dssnet6 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet6i_handle_ip6_ev_cback
(
  ip6_sm_type             *ip6_inst_ptr,
  ip6_sm_indidcation_type ind,
  void                    *user_data
)
{

  dssnet6_sm_cb_type    *instance_ptr;
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
    ds707_pdn_throttle_info_type*  throttle_info_ptr = NULL;
    ds707_data_session_profile_info_type *pdn_prof_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_IS707 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ip6_inst_ptr);
  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "Recd IP6 event %d in DSSNet6 0x%x", ind, user_data);

  instance_ptr = (dssnet6_sm_cb_type*)(user_data);

  switch (ind)
  {
    case IP6_SM_DOWN_IND:
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_NET_DOWN_EV, NULL);
      break;

    case IP6_SM_UP_IND:
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
           if( (instance_ptr->is_ehrpd_call == TRUE) && 
               (instance_ptr->state == DSSNET6I_WAITING_FOR_NET_UP_STATE)
             )
           {
             /*-------------------------------------------------------------------
              If this PDN is throttled, clear the v6_count since there is a
              successful v6 data connection.
             -------------------------------------------------------------------*/
             if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                  instance_ptr->ps_iface_ptr,
                                                  &pdn_prof_ptr) )
             {
               ds707_data_session_get_iface_current_profile(
                                                     instance_ptr->ps_iface_ptr,
                                                     &pdn_prof_ptr);
             }
             if (pdn_prof_ptr != NULL)
             {
               throttle_info_ptr = ds707_pdncntx_get_throttle_info_ptr(pdn_prof_ptr);
               if ( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
               {
                 ds707_pdncntx_clear_throttle_count( throttle_info_ptr,
                                                      DS707_PDNCNTX_IP_TYPE_V6 );
               }
             }
           }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_DATA_IS707 */
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_NET_UP_EV, NULL);
      break;

    case IP6_SM_ABORTED_IND:
      (void) dssnet6_sm_post_event(instance_ptr, DSSNET6_NET_ABORTED_EV, NULL);
      break;  
    default:
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x got unsupported IP6 event %d",
                      instance_ptr,
                      ind);
      ASSERT(0);
  }
} /* dssnet6i_handle_ip6_ev_cback() */

/*===========================================================================
FUNCTION DSSNET6I_TRANSITION_STATE()

DESCRIPTION
  This function is called whenever the state changes.
  The variable is set and housekeeping done for each state.
  
  Any change to state should be through this function only.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the state.
===========================================================================*/
static void dssnet6i_transition_state
(
  dssnet6_sm_cb_type     *instance_ptr,          /* DSSNET6 SM instance    */
  dssnet6i_state_type    new_state               /* new state to change to */
)
{
  ppp_dev_opts_type    *ppp_config = NULL;  
  ps_ip_addr_type      peer_addr;
  int                  ret_val;
  int16                ds_errno = DS_ENOERR;
  
#ifdef FEATURE_DATA_IS707
  /* profile parameters to configure PPP */
  ds707_data_session_profile_info_type 
                       *data_sess_profile_info_ptr;
#ifdef FEATURE_EHRPD
  ppp_ncp_config_union_type ncp_config;
  ppp_event_alloc_info_type ppp_event_alloc_info;
  ds707_pdn_context_cb_type *pdncntx_cb_ptr;
  ppp_ncp_handle_type ncp_handle;
  ds707_sys_chg_sm_e  sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
  int16 ps_errno;
#endif /*FEATURE_DATA_EHRPD_DUAL_IP*/
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
  ds707_pdn_throttle_info_type* throttle_info_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
  ps_iface_type *pdn_v4_iface_ptr = NULL;
  uint32 phys_flow_mask = 0;
  ps_phys_link_type *phys_link_ptr=NULL;
 mc_queue_cmd_status_type       queue_status;
 ps_iface_type*                 iface4_ptr = NULL;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Changing the state variable.
  -------------------------------------------------------------------------*/
  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,"DSSNet6 0x%x transition state %d to %d",
                  instance_ptr,
                  instance_ptr->state,
                  new_state);
  instance_ptr->state = new_state;

  /*-------------------------------------------------------------------------
    Executing state based functionality.
  -------------------------------------------------------------------------*/
  switch (instance_ptr->state)
  {
    case DSSNET6I_CLOSED_STATE:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DSSNet6 0x%x IP6 network is down",instance_ptr);

      /*---------------------------------------------------------------------
        Set lcp_auth_disabled to FALSE.
      ---------------------------------------------------------------------*/
      instance_ptr->qnc_call = FALSE;

      /*---------------------------------------------------------------------
        Take the default route through this interface out
      ---------------------------------------------------------------------*/
      (void) route_flush( instance_ptr->ps_iface_ptr);

      /*---------------------------------------------------------------------
        Check to see if ps_iface_ptr is not set to NULL. 
      ---------------------------------------------------------------------*/
      if (instance_ptr->ps_iface_ptr != NULL)
      {
        /*-------------------------------------------------------------------
          Enable flow to apps (Sockets is done using the iface. Enable flow 
          so that laptop calls can be setup now).
        -------------------------------------------------------------------*/
        ps_iface_enable_flow(instance_ptr->ps_iface_ptr, 
                             DS_FLOW_CTRL_DSSNET6_MASK);

        /*-------------------------------------------------------------------
          Delete all prefixes associated with this interface.
        -------------------------------------------------------------------*/
        (void) ps_iface_delete_all_v6_prefixes(instance_ptr->ps_iface_ptr);

        ps_flow_go_null_ind(
                     PS_IFACEI_GET_DEFAULT_FLOW(instance_ptr->ps_iface_ptr),
                     PS_EIC_NOT_SPECIFIED);
        /*-------------------------------------------------------------------
          Get the current network mode.
          Comment the logic.
        -------------------------------------------------------------------*/
        
#ifdef FEATURE_EHRPD
        sys_chg_sm_state = ds707_query_sys_chg_sm_state();
#endif /* FEATURE_EHRPD */
                
        if(PS_IFACE_GET_PHYS_LINK(instance_ptr->ps_iface_ptr) == NULL)
        {
          ret_val = 0;
        }
#ifdef FEATURE_EHRPD
        /*-------------------------------------------------------------------
          If sys chg sm is performing special cleanup, do not interrupt by
          posting phys link go null cmd here. When the cleanup procedure is
          complete, sys chg sm will issue phys link abort that'll end up
          triggering phys link gone ev to DSSNETs. DSSNET SM will then
          transition from closed->closed state and post iface down ind
         ------------------------------------------------------------------*/
       else if (TRUE == instance_ptr->use_ppp &&
                (DS707_SYS_CHG_SM_WAIT_FOR_DORM == sys_chg_sm_state ||
                 DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG == sys_chg_sm_state)
               )
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sys Chg SM performing special" 
                          "cleanup. Do not trigger ps_phys_link_go_null_cmd"); 
          ret_val  = -1;
          ds_errno = DS_EWOULDBLOCK;
        }
#endif /* FEATURE_EHRPD */
        else
        {
          ret_val = ps_phys_link_go_null_cmd (
                     PS_IFACE_GET_PHYS_LINK(instance_ptr->ps_iface_ptr),
                     &ds_errno, 
                     NULL);
        }
        if (ret_val == 0)
        {
          /* Stop the DHCP instance before giving down indication */
#ifdef FEATURE_DATA_IS707
           (void) dshdr_stop_dhcp(instance_ptr->ps_iface_ptr);
#endif /* FEATURE_DATA_IS707 */
          /*-----------------------------------------------------------------
            No need to wait for phys_link_down - Send a ps_iface_down_ind. 
          -----------------------------------------------------------------*/
          dssnet6_get_ps_call_end_reason(instance_ptr);
          if(instance_ptr->ps_iface_ptr != NULL)
          {
            DATA_3GMGR_MSG3(MSG_LEGACY_HIGH, 
                            "Posting iface down for 0x%x:%d, call end reason: 0x%x", 
                            instance_ptr->ps_iface_ptr->name, 
                            instance_ptr->ps_iface_ptr->instance,
                            instance_ptr->call_end_reason);
          }
         /*-------------------------------------------------------------------
           Enter crit sect to prevent any post proc on this iface 
           while down ind is being posted here.
           To avoid any deadlock between DS task and other tasks running Routing,
           acquire the global_ps_crit_section. This ensures that both DS task
           and any other task running Routing and picking up 3GPP iface obtain
           critical sections in the same order
         ------------------------------------------------------------------*/
          PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
          dssnet6_enter_crit_sect();
          if(instance_ptr->iface_down_ind_cb != NULL)
          {
            DATA_MSG3(MSG_LEGACY_HIGH,"V6 iface 0x%x:%d (0x%x) is down. Calling iface_down_ind_cb",
                      instance_ptr->ps_iface_ptr->name,
                      instance_ptr->ps_iface_ptr->instance,
                      instance_ptr->ps_iface_ptr);
            instance_ptr->iface_down_ind_cb(instance_ptr->ps_iface_ptr,
                                        instance_ptr->call_end_reason);
          }
          dssnet6_leave_crit_sect();
          PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

          if (instance_ptr->use_ppp)
          {
#ifdef FEATURE_DATA_IS707
            /*---------------------------------------------------------------------
              If all ifaces are down, & the current system is not EPC system, 
              change sys_info to NO_SYSTEM. This change is also done when MH gets 
              PPP down ev with LCP protocol. Below change is required when 
              ppp_abort is called, and MH deregisters from PPP events. MH will 
              not get PPP down ev, and sys info will not be cleared. In case of 
              EPC this will be handled by device manager
            ---------------------------------------------------------------------*/
            if((ds707_pkt_mgr_all_ifaces_down()==TRUE)
                && (DS707_SYS_INFO_EPC_SYSTEM 
                                   != ds707_sys_chg_hdlr_get_current_system()))
            {
               DATA_3GMGR_MSG0( MSG_LEGACY_HIGH, 
                               "all ifaces are in down state, clear sys_info to NO_SYSTEM" );
              ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);
            }
            else
            {
               DATA_3GMGR_MSG1( MSG_LEGACY_HIGH, 
                               "all ifaces are not down or in EPC system, no change to sys_info %d",
                               ds707_sys_chg_hdlr_get_current_system() );
            } 
 #endif
          /*-----------------------------------------------------------------
            Callbacks for phys link events are registered with PS only if ppp 
            is used. Deregister Phys link gone event.
          -----------------------------------------------------------------*/
            ps_iface_event_cback_dereg(instance_ptr->ps_iface_ptr,
                                       IFACE_PHYS_LINK_GONE_EV,
                                       instance_ptr->phys_link_gone_cbuf);
            ps_iface_event_cback_dereg(instance_ptr->ps_iface_ptr,
                                       IFACE_PHYS_LINK_UP_EV,
                                       instance_ptr->phys_link_up_cbuf);
            ps_iface_event_cback_dereg(instance_ptr->ps_iface_ptr,
                                       IFACE_PHYS_LINK_DOWN_EV,
                                       instance_ptr->phys_link_down_cbuf);
 #ifdef FEATURE_DATA_IS707
            /* If NCP resync was in progress, reset ncp_resync_started flag */
            pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
            if ( DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) &&
                 ds707_pdncntx_get_ncp_resync_started(pdncntx_cb_ptr) == TRUE
               )
            {
                ds707_pdncntx_set_ncp_resync_started(pdncntx_cb_ptr, FALSE);
            }
 #endif
          }

          else
          {
            /*-----------------------------------------------------------------
              Reset call end reason here since it is not done in iface down 
              indication callback for 3GPP
            -----------------------------------------------------------------*/
            dssnet6_reset_call_end_reason(instance_ptr);
            dssnet6_reset_local_abort(instance_ptr);
          }
        }

        if ((ret_val < 0) && ( ds_errno != DS_EWOULDBLOCK))
        {
          DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                          "phy link down fail,%d phys_link ret" , ds_errno);
        }

        /*-------------------------------------------------------------------
          Invoke the ppp_resync_done_cb to notify the completion of the PPP
          resync (either successfully or failed resync). Also unregister the
          call back right after invoking it.
        -------------------------------------------------------------------*/
        if (instance_ptr->ppp_resync_done_cb != NULL)
        {
          instance_ptr->ppp_resync_done_cb(instance_ptr);
          instance_ptr->ppp_resync_done_cb = NULL;
        }

        /*-------------------------------------------------------------------
          Set default DNS values in iface. PPP will update these with the 
          network assigned values if DNS gets negotiated.
          // Need to add this to get IPv6 addresses.
        -------------------------------------------------------------------*/
        //dssnet6_sm_set_dns(instance_ptr, primary_v6_dns, secondary_v6_dns);
        //  instance_ptr->ps_iface_ptr->v6_net_info.primary_dns = 
        //    dsat6_qcdns_val[0];
        //  instance_ptr->ps_iface_ptr->v6_net_info.secondary_dns = 
        //    dsat6_qcdns_val[1];

        if((instance_ptr->ppp_event_handle) != NULL)
        {
         /*-------------------------------------------------------------------
            De-registering of events is not required since we deallocate the 
            PPP event handle. If done it does not have any adverse effect
          -------------------------------------------------------------------*/
          if (FALSE == instance_ptr->is_ehrpd_call)
          {
            (void) ppp_event_deregister( 
                               instance_ptr->ppp_event_handle,
                               PPP_PROTOCOL_LCP,
                               (ppp_event_mask_e_type) ( PPP_DOWN_EV_MASK | 
                                                         PPP_RESYNC_EV_MASK) );

            (void) ppp_event_deregister(
                               instance_ptr->ppp_event_handle,
                               PPP_PROTOCOL_IPV6CP,
                               (ppp_event_mask_e_type) ( PPP_UP_EV_MASK | 
                                                         PPP_RESYNC_EV_MASK | 
                                                         PPP_DOWN_EV_MASK) );
          }  
  
          else
          {
#ifdef FEATURE_EHRPD
              (void) ppp_event_deregister( 
                      instance_ptr->ppp_event_handle,
                      PPP_PROTOCOL_VSNCP,
                      (ppp_event_mask_e_type)( PPP_UP_EV_MASK | 
                                               PPP_RESYNC_EV_MASK | 
                                               PPP_DOWN_EV_MASK 
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
                                               | PPP_SUBS_LIMITED_TO_V4_EV_MASK 
                                               | PPP_SUBS_LIMITED_TO_V6_EV_MASK 
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
                                              ) );
#endif /* FEATURE_EHRPD */
          }

          (void) ppp_event_deregister(instance_ptr->ppp_event_handle,
                                      PPP_PROTOCOL_AUTH,
                                      PPP_FAILURE_EV_MASK);

          if ( ppp_event_dealloc( PPP_UM_SN_DEV,
                                  &(instance_ptr->ppp_event_handle) ) == -1 )
          {
            DATA_MSG0(MSG_LEGACY_FATAL," Unable to deallcoc PPP event Handle");
          }
          instance_ptr->ppp_event_handle = NULL;
        }

        if (instance_ptr->use_ppp)
        {

#ifdef FEATURE_EHRPD
          if (instance_ptr->is_ehrpd_call == TRUE)
          {
            instance_ptr->is_ehrpd_call = FALSE;
          }
#endif /*FEATURE_EHRPD */
        }
      }

      dssnet6_set_is_bringup_phys_link(instance_ptr, FALSE);
      break;


    case DSSNET6I_LINK_OPENING_STATE:
      /*---------------------------------------------------------------------
        Disable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow(instance_ptr->ps_iface_ptr,
                            DS_FLOW_CTRL_DSSNET6_MASK);

#ifdef FEATURE_DATA_IS707
      if ( instance_ptr->use_ppp == TRUE )
      {

      /*-----------------------------------------------------------------
        Deregister Phys link down event, since we no longer need
        it after Link Opening state
      -----------------------------------------------------------------*/
        DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                        "Iface in Link Opening state, Dereg IFACE_PHYS_LINK_DOWN_EV");
        ps_iface_event_cback_dereg(instance_ptr->ps_iface_ptr,
                                         IFACE_PHYS_LINK_DOWN_EV,
                                         instance_ptr->phys_link_down_cbuf);

        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, 
                        "re-setting graceful_ppp_close for IFACE: 0x%x:%d",
                        (instance_ptr->ps_iface_ptr)->name,
                        (instance_ptr->ps_iface_ptr)->instance);
         /*---------------------------------------------------------------------
          Set PPP termination policy.
        ---------------------------------------------------------------------*/
        if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
        {
          PS_IFACE_SET_GRACEFUL_CLOSE(instance_ptr->ps_iface_ptr, FALSE);
        }
        else
        {
          PS_IFACE_SET_GRACEFUL_CLOSE(instance_ptr->ps_iface_ptr, TRUE);
        }
      }

      /*----------------------------------------------------------------------
        get the requesting_profile_id for the associated iface
      ------------------------------------------------------------------------*/
      if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                  instance_ptr->ps_iface_ptr,
                                                  &data_sess_profile_info_ptr) )
      {
        ds707_data_session_get_iface_current_profile(
                                                instance_ptr->ps_iface_ptr,
        &data_sess_profile_info_ptr);
      }

#ifdef FEATURE_EHRPD
      if( ds707_pkt_is_ehrpd_mode_of_operation() ) 
      {
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
        /*---------------------------------------------------------------------
          if PDN is throttled, and v6 throttle timer is running, or, the PDN
          is infinitely throttled, do not allow this call.
        ---------------------------------------------------------------------*/
        throttle_info_ptr = ds707_pdncntx_get_throttle_info_ptr(data_sess_profile_info_ptr);
        if ( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
        {
          if( (PS_IFACE_IS_IN_USE(instance_ptr->ps_iface_ptr)) &&
              ((rex_get_timer(&(throttle_info_ptr->ipv6_throttle_info.throttle_timer)) != 0) ||
              (DS707_PDNCNTX_THROTTLE_TYPE_INFINITE == 
                         throttle_info_ptr->ipv6_throttle_info.throttle_type))
            )
          {
            /* Set PDN IPV6 Throttled info to PS end reason */
            if (throttle_info_ptr->ipv6_throttle_info.throttle_type 
                  == DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW)
            {
              dssnet6_set_ps_call_end_reason(
                instance_ptr, 
                PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED );
            }
            else if(throttle_info_ptr->ipv6_throttle_info.throttle_type 
                  == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE )
            {
              dssnet6_set_ps_call_end_reason(
                instance_ptr, 
                PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED );
            }
            else if(throttle_info_ptr->ipv6_throttle_info.throttle_type 
                  == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE )
            {
              DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                               "V6: PDN infinitely throttled because of reconnection disallowed");
              dssnet6_set_ps_call_end_reason(
                instance_ptr, 
                PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED );
            }

            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "PDN throttled for V6, posting "
                            "DSSNET6_LINK_DOWN_EV");
            (void)dssnet6_sm_post_event( instance_ptr, DSSNET6_LINK_DOWN_EV, NULL);
            return; 
          }
        }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */ 
        /*---------------------------------------------------------------------
          When system changes from 1x<->DO, and DS hasnt received IDM change
          from CM, if a call is attempted on EPC, we should not allow the
          call till the non-EPC PPP is torn down. Hence set system to EPC
          only if there is no other call on non-EPC.
        ---------------------------------------------------------------------*/
        if( ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_NON_EPC_SYSTEM )
        {
          DATA_3GMGR_MSG0( MSG_LEGACY_LOW,
                           "EPC call requested when system is non-EPC, posting link_down");
          (void)dssnet6_sm_post_event( instance_ptr, DSSNET6_LINK_DOWN_EV,NULL);
          return;
        }

        instance_ptr->is_ehrpd_call = TRUE;

        /*---------------------------------------------------------------------
          If the Mode-Handler has allocated atleast one PDN it means that the 
          PPP device is configured and need not be configured again. Device 
          manager is used for configuring PPP device & registering LCP/AUTH
        ---------------------------------------------------------------------*/
        if (0 != ds707_devmgr_start(PPP_UM_SN_DEV, data_sess_profile_info_ptr))
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, 
                          "PPP configure Device Failed, do not proceed");
         /*----------------------------------------------------------------
            PPP configuration failed. Post the appropriate event to cleanup
            call since PPP will not give any other indications. 
         ---------------------------------------------------------------*/
          (void) dssnet6_sm_post_event( instance_ptr, 
                                        DSSNET6_LINK_DOWN_EV,
                                        NULL );
          return;
        }

        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
        if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Cannot get PDN Control Block, do not proceed");
          (void)dssnet6_sm_post_event( instance_ptr,
                                       DSSNET6_LINK_DOWN_EV,NULL);
          return; 
        }

         /*--------------------------------------------------------------------
          Configure NCP only if it is the first DSSNET that is coming up
        ---------------------------------------------------------------------*/   
        if( ! ( ds707_pdncntx_get_ncp_started( pdncntx_cb_ptr ) ) )
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, 
                          "ds707_pdncntx_configure_vsncp: dssnet6 0x%x, "
                          "pdn context cb 0x%x", instance_ptr, pdncntx_cb_ptr);
          if ( ds707_pdncntx_configure_vsncp( pdncntx_cb_ptr,
                                             &(ncp_config.vsncp_3gpp2),
                                             data_sess_profile_info_ptr ) 
                                             == FALSE )
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, 
                            "VSNCP configure Device Failed, do not proceed");
             /*--------------------------------------------------------------
              PPP configuration failed.  Post the appropriate event to
              cleanup call since PPP will not give any other indications. 
            ---------------------------------------------------------------*/
            dssnet6_set_ps_call_end_reason(instance_ptr, 
                                   PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_CONFIG_FAILURE);
            (void) dssnet6_sm_post_event( instance_ptr, DSSNET6_LINK_DOWN_EV, NULL );
            return;
          }
          /*-----------------------------------------------------------------
            Since we are in the v6 Statemachine, make sure that the
            profile does configure the VSNCP with the correct req_addr_type
          -----------------------------------------------------------------*/
          if ( ( ncp_config.vsncp_3gpp2.option_vals.req_addr_type 
                   != VSNCP_3GPP2_PDN_TYPE_IPV6 ) && 
               ( ncp_config.vsncp_3gpp2.option_vals.req_addr_type 
                   != VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ) )
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Invalid Req_addr_type in the "
                            "profile, do not proceed");
             /*----------------------------------------------------------------
              PPP configuration failed.  Post the appropriate event to cleanup
              call since PPP will not give any other indications. 
            -----------------------------------------------------------------*/
            (void) dssnet6_sm_post_event( instance_ptr, DSSNET6_LINK_DOWN_EV, NULL);
            return;
          }

          ncp_handle = ppp_ncp_alloc( PPP_UM_SN_DEV,
                                      PPP_NCP_VSNCP_3GPP2,
                                      &ncp_config );
          
          if ( ncp_handle == NULL )
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                            "Cannot Allocate NCP, do not proceed");
            /*-----------------------------------------------------------------
              PPP configuration failed.  Post the appropriate event to cleanup
              call since PPP will not give any other indications. 
            -----------------------------------------------------------------*/
            (void) dssnet6_sm_post_event( instance_ptr, DSSNET6_LINK_DOWN_EV, NULL);
            return;
          }
          ds707_pdncntx_set_ncp_handle(pdncntx_cb_ptr, ncp_handle);
        }/* if NCP not started */

        /*------------------------------------------------------------------
          eHRPD - Register only for VSNCP events & AUTH failure event
        -------------------------------------------------------------------*/   
        if ( instance_ptr->ppp_event_handle == NULL )
        {
          ppp_event_alloc_info.ncp = PPP_NCP_VSNCP_3GPP2;
          ppp_event_alloc_info.ncp_cb_f = dssnet6i_handle_ppp_ev_cback;
          ppp_event_alloc_info.ncp_user_data = instance_ptr;
          ppp_event_alloc_info.ncp_handle_ptr = 
            ds707_pdncntx_get_ncp_handle(pdncntx_cb_ptr);

          ppp_event_alloc_info.auth_cb_f = dssnet6i_handle_ppp_ev_cback;
          ppp_event_alloc_info.auth_user_data = instance_ptr;

          instance_ptr->ppp_event_handle = ppp_event_alloc_ex( PPP_UM_SN_DEV,
                                                               &ppp_event_alloc_info );

          if ( NULL == instance_ptr->ppp_event_handle )
          {
            DATA_MSG0(MSG_LEGACY_FATAL,"Unable to acquire a PPP event handle");
          }

          (void) ppp_event_register( instance_ptr->ppp_event_handle,
                                     PPP_PROTOCOL_VSNCP,
                                     (ppp_event_mask_e_type)( 
                                            PPP_UP_EV_MASK | 
                                            PPP_RESYNC_EV_MASK | 
                                            PPP_DOWN_EV_MASK 
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
                                            | PPP_SUBS_LIMITED_TO_V4_EV_MASK 
                                            | PPP_SUBS_LIMITED_TO_V6_EV_MASK 
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
                                                             ) );

          /*------------------------------------------------------------------- 
            Register for AUTH failure event so that dssnet6 can store the call 
            end reason
          -------------------------------------------------------------------*/ 
          (void) ppp_event_register(instance_ptr->ppp_event_handle,
                                    PPP_PROTOCOL_AUTH,
                                    PPP_FAILURE_EV_MASK);
        }

        /*--------------------------------------------------------------------- 
          Start NCP only if it is the first DSSNET that is coming up
        ---------------------------------------------------------------------*/ 
        if ( !( ds707_pdncntx_get_ncp_started( pdncntx_cb_ptr ) ) )        
        {
          if ( ppp_ncp_start( ds707_pdncntx_get_ncp_handle(pdncntx_cb_ptr) ) < 0 )
          {
             DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                             "VSNCP Start Failed, do not proceed");
             /*----------------------------------------------------------------
              PPP configuration failed.  Post the appropriate event to cleanup
              call since PPP will not give any other indications. 
            -----------------------------------------------------------------*/
            (void) dssnet6_sm_post_event( instance_ptr, DSSNET6_LINK_DOWN_EV, NULL );
            return;
          }

          /*-------------------------------------------------------------------
            Call process VSNCP started, here the ncp_started flag is set.
          -------------------------------------------------------------------*/
          ds707_pdncntx_process_vsncp_started( pdncntx_cb_ptr );

          /*-------------------------------------------------------------------
            Set the call bringup mode flag
          -------------------------------------------------------------------*/
          ds707_pdncntx_set_call_bringup_mode(pdncntx_cb_ptr);

        }/* NCP not started*/
      }
      else
#endif /* FEATURE_EHRPD */  
#endif /* FEATURE_DATA_IS707 */
      {
#ifdef FEATURE_DATA_IS707
        /*---------------------------------------------------------------------
          When system changes from 1x<->DO, and DS hasnt received IDM change
          from CM, if a call is attempted on non-EPC, we should not allow the
          call till the EPC PPP is torn down. Hence set system to non-EPC
          only if there is no other call on EPC.
        ---------------------------------------------------------------------*/
        if( ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_EPC_SYSTEM )
        {
          DATA_3GMGR_MSG0( MSG_LEGACY_LOW,
                           "NON-EPC call requested when system is EPC, posting link_down");
          (void)dssnet6_sm_post_event( instance_ptr, DSSNET6_LINK_DOWN_EV,NULL);
          return;
        }
        ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NON_EPC_SYSTEM);
#endif
        
        instance_ptr->is_ehrpd_call = FALSE;
         
        /*-------------------------------------------------------------------
          Allocate memory for ppp_config
        -------------------------------------------------------------------*/
        ppp_config = (ppp_dev_opts_type*)
                     modem_mem_alloc(sizeof(ppp_dev_opts_type), 
                                     MODEM_MEM_CLIENT_DATA_CRIT);
        if(ppp_config == NULL)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "dssnet6i_transition_state():"
                          "memory allocation failed for ppp_config");
          (void)dssnet6_sm_post_event(instance_ptr,DSSNET6_LINK_DOWN_EV,NULL);
          return;   
        }
        memset((void *)ppp_config, 0, sizeof(ppp_dev_opts_type));

        dssnet6i_configure_ppp ( instance_ptr, ppp_config );

        /*---------------------------------------------------------------------
          Setup IPv6CP configuration
        ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
        (void) dssnet_ppp_setup_ncp(ppp_config,
                                  instance_ptr->ps_iface_ptr,
                                  FALSE,
                                  FALSE,
                                  data_sess_profile_info_ptr);

        /*---------------------------------------------------------------------
          Setup LCP configuration
        ---------------------------------------------------------------------*/
        dssnet_ppp_setup_lcp(ppp_config,
                           data_sess_profile_info_ptr);
#endif /* FEATURE_DATA_IS707 */

        /*-------------------------------------------------------------------
          Register for PPP UP/DOWN/RESYNC Events.
        -------------------------------------------------------------------*/
        if ( instance_ptr->ppp_event_handle == NULL )
        {
          instance_ptr->ppp_event_handle = ppp_event_alloc( PPP_UM_SN_DEV,
                                                 dssnet6i_handle_ppp_ev_cback,
                                                 instance_ptr,
                                                 dssnet6i_handle_ppp_auth_ev_cback,
                                                 instance_ptr,
                                                 NULL,
                                                 NULL,
                                                 dssnet6i_handle_ppp_ev_cback,
                                                 instance_ptr );
          if ( NULL == instance_ptr->ppp_event_handle )
          {
            DATA_MSG0(MSG_LEGACY_FATAL,"Unable to acquire a PPP event handle");
            modem_mem_free(ppp_config,MODEM_MEM_CLIENT_DATA_CRIT);
            (void)dssnet6_sm_post_event(instance_ptr,DSSNET6_LINK_DOWN_EV,NULL);
            return;
          }


          (void)ppp_event_register(instance_ptr->ppp_event_handle,
                                   PPP_PROTOCOL_LCP,
                                   (ppp_event_mask_e_type)(PPP_DOWN_EV_MASK |
                                                           PPP_RESYNC_EV_MASK));

          (void)ppp_event_register(instance_ptr->ppp_event_handle,
                                   PPP_PROTOCOL_AUTH,
                                   PPP_FAILURE_EV_MASK);

          (void)ppp_event_register(instance_ptr->ppp_event_handle,
                                   PPP_PROTOCOL_IPV6CP,
                                   (ppp_event_mask_e_type)(PPP_UP_EV_MASK |
                                                           PPP_RESYNC_EV_MASK |
                                                           PPP_DOWN_EV_MASK));
        }

        if(ppp_start(PPP_UM_SN_DEV, ppp_config) < 0)
        {
          /*-----------------------------------------------------------------
            PPP configuration failed.  Post the appropriate event to cleanup
            call since PPP will not give any other indications. 
          -----------------------------------------------------------------*/
          (void) dssnet6_sm_post_event( instance_ptr, DSSNET6_LINK_DOWN_EV, NULL);
        }
        modem_mem_free(ppp_config, MODEM_MEM_CLIENT_DATA_CRIT);
      }  
      break;

    case DSSNET6I_WAITING_FOR_NET_UP_STATE:
      if ((instance_ptr->is_ehrpd_call) && (instance_ptr->use_ppp))
      {
#if defined(FEATURE_DATA_IS707) && defined(FEATURE_NIQ_EHRPD)
      /*---------------------------------------------------------------------
        Handle S-BCM for n/w init QOS
      ---------------------------------------------------------------------*/
        dsgen_iface_handle_niq_sbcm(instance_ptr->ps_iface_ptr);
#endif /* FEATURE_DATA_IS707 && FEATURE_NIQ_EHRPD */

#ifdef FEATURE_EHRPD
      /*---------------------------------------------------------------------
          Check default APN indication in VSNCP
        ---------------------------------------------------------------------*/
        dsgen_iface_handle_vsncp_default_apn_ind(instance_ptr->ps_iface_ptr);
#endif /* FEATURE_EHRPD */
      }

      /*---------------------------------------------------------------------
        Start IPv6
      ---------------------------------------------------------------------*/
      ps_iface_configuring_ind(instance_ptr->ps_iface_ptr);

#ifdef FEATURE_DATA_OPTHO
      if ((instance_ptr->use_ppp) && 
          (instance_ptr->is_ehrpd_call) &&
          (ds707_s101_query_sm_state() != DS707_S101_SM_NON_TUNNEL))
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "IPV6 tunnel call. Setting IP6 SM to passive start");
        if ((ip6_sm_set_passive(instance_ptr->ip6_instance)) == -1)
        {
          DATA_ERR_FATAL("ip6_sm_set_passive() failed ");
        }
      }
#endif /* FEATURE_DATA_OPTHO */
#ifdef FEATURE_DATA_PS_DHCPV6
      ip6_sm_set_prefix_delegation_mode(instance_ptr->ip6_instance,
                                        dssnet6i_get_ipv6_prefix_delegation_mode(instance_ptr->ps_iface_ptr));

      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Prefix Delegation type returned %d",
                      dssnet6i_get_ipv6_prefix_delegation_mode(instance_ptr->ps_iface_ptr));
#endif

      if ((ip6_sm_post_event( instance_ptr->ip6_instance,
                              IP6_SM_START_EV, NULL)) == -1)
      {
        DATA_ERR_FATAL("ip6_sm_post_event() failed ");
      }

      break;


    case DSSNET6I_NET_UP_STATE:    
      /*---------------------------------------------------------------------
        Check if ready for 1x tuneaway and update MMODE
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707      
      pdncntx_cb_ptr = 
        ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(instance_ptr->ps_iface_ptr);
      iface4_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
      if((ps_iface_state(iface4_ptr) != IFACE_COMING_UP) &&
         (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA)) 
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                        "Posting MC_TUNE_AWAY_CNTL_F - Ready for TuneAway");
        queue_status = mc_queue_cmd(MC_TUNE_AWAY_CNTL_F); 
        if (queue_status == MC_FREE_QUEUE_EMPTY) 
        {
          ERR_FATAL( "No MC command links available", 0, 0, 0);
        }
        else if (queue_status != MC_CMD_QUEUED) 
        {
          ERR_FATAL( "Unknown status return from mc_queue_cmd()", 0, 0, 0);
        }
      }
#endif /* FEATURE_DATA_IS707 */

      /*---------------------------------------------------------------------
        Send iface up indication.
      ---------------------------------------------------------------------*/
      ps_iface_up_ind (instance_ptr->ps_iface_ptr);

#ifdef FEATURE_DATA_IS707

      ds707_pkt_mgr_check_coex_data_status(instance_ptr->ps_iface_ptr);

      if ((instance_ptr->ps_iface_ptr != NULL) &&
          (instance_ptr->ps_iface_ptr->name == CDMA_SN_IFACE))
      {
        /*------------------------------------------------------------------
          Starting DHCP only for 3GPP2, 3GPP handles it in a different place
        ------------------------------------------------------------------*/
        dshdr_dhcp_start (instance_ptr->ps_iface_ptr);
      }
#endif /* FEATURE_DATA_IS707 */

      /*---------------------------------------------------------------------
        Invoke the ppp_resync_done_cb to notify the completion of the PPP
        resync (either successfully or failed resync). Also unregister the
        call back right after invoking it.
      ---------------------------------------------------------------------*/
      if (instance_ptr->ppp_resync_done_cb != NULL)
      {
        instance_ptr->ppp_resync_done_cb(instance_ptr);
        instance_ptr->ppp_resync_done_cb = NULL;
      }

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_EHRPD
      if ( instance_ptr->use_ppp == TRUE )
      {
        /* If NCP resync was in progress, reset ncp_resync_started flag */
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
        if ( DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) &&
             ds707_pdncntx_get_ncp_resync_started(pdncntx_cb_ptr) == TRUE
           )
        {
            ds707_pdncntx_set_ncp_resync_started(pdncntx_cb_ptr, FALSE);
        }
      }
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_DATA_IS707 */
      /*---------------------------------------------------------------------
        Trigget DHCP negotiation
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
      dshdr_dhcp_trigger_negotiation (instance_ptr->ps_iface_ptr);
#endif /* FEATURE_DATA_IS707 */
      /*---------------------------------------------------------------------
        Enable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_enable_flow( instance_ptr->ps_iface_ptr, 
                            DS_FLOW_CTRL_DSSNET6_MASK);

      if (instance_ptr->pdsn_as_proxy_ipv6_dns_server)
      {
        peer_addr.type = IPV6_ADDR;
        ps_iface_get_peer_addr ( instance_ptr->ps_iface_ptr,&peer_addr);
        instance_ptr->ps_iface_ptr->v6_net_info.primary_dns = 
        peer_addr.addr.v6;
        instance_ptr->ps_iface_ptr->v6_net_info.secondary_dns = 
        peer_addr.addr.v6;
      }

      break;

    case DSSNET6I_LINK_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Close PPP.
      ---------------------------------------------------------------------*/
      if (instance_ptr->use_ppp)
      {
#ifdef FEATURE_DATA_IS707
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                                instance_ptr->ps_iface_ptr);
        if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                          "Cannot get PDN Control Block, do not proceed");
          return; 
        }

#ifdef FEATURE_EHRPD
        if (instance_ptr->is_ehrpd_call == TRUE)
        {
          if ( ds707_pdncntx_get_ncp_started( pdncntx_cb_ptr ) )
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_MED, "NCP still up, terminate v6 "
                            "iface or complete PDN accordingly");
            if( (ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_EPC_SYSTEM )
              &&
              (ds707_pkt_is_ehrpd_mode_of_operation() == TRUE)
              )
            {
              DATA_3GMGR_MSG0(MSG_LEGACY_MED, "Current system EPC");
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
              pdn_v4_iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
              if( (! PS_IFACEI_IS_IN_ARBITRATION(instance_ptr->ps_iface_ptr))&&
                  (PS_IFACE_IS_VALID(pdn_v4_iface_ptr)) &&
                  (PS_IFACE_IS_IN_USE(pdn_v4_iface_ptr)) &&
                  (!ds707_is_term_in_progress()) )
              {
                DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                                "V4 iface in use, tear down only v6");
                (void) ps_iface_go_null_cmd( instance_ptr->ps_iface_ptr,
                                             &ps_errno, 
                                             NULL );
                dssnet6i_transition_state( 
                  instance_ptr, 
                  DSSNET6I_CLOSED_STATE );
                break;
              }
              else
#endif/*FEATURE_DATA_EHRPD_DUAL_IP*/
              {
                DATA_3GMGR_MSG0(MSG_LEGACY_MED, 
                                "Current system EPC, gracefull teardown");
                if (
                     (instance_ptr->local_abort != TRUE) 
                       && 
                      (
                         (PHYS_LINK_UP == 
                                ps_iface_phys_link_state(instance_ptr->ps_iface_ptr) )
                          ||
                         (TRUE == 
                      PS_IFACE_GET_GRACEFUL_CLOSE(instance_ptr->ps_iface_ptr))
                       )
                    )
                {
                  if(ppp_ncp_stop(
                          ds707_pdncntx_get_ncp_handle( pdncntx_cb_ptr )) < 0)
                  {
                    /*-----------------------------------------------------------
                     Since ppp_ncp_stop failed, we wont get link down from PPP.
                     Post link_down to ourselves to cleanup the DSSNET
                    -----------------------------------------------------------*/
                    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"call to ncp_stop failed!"
                                                    "Post link_down to cleanup"
                                                    "dssnet6");
                    (void) dssnet6_sm_post_event( instance_ptr, 
                                                  DSSNET6_LINK_DOWN_EV, 
                                                  NULL );
                    /*----------------------------------------------------------- 
                     Below we set ncp_started to false. So we need to return
                     here itself. Reason: In a dual IP scenario if for some
                     reason the PPP device is already cleaned up and if we attempt
                     to close both dssnet4 & 6, the first dssnet that processes 
                     ppp_close_cmd comes here and attempts to stop/abort ncp 
                     and gets a failure. If it doesn't return here and sets the 
                     ncp_started to false, then the next dssnet will eternally 
                     wait to get link_down from PPP. Return here so the other 
                     dssnet will also attempt stop/abort ncp and cleanup by itself
                     Same logic applies to below calls to ncp abort
                    -----------------------------------------------------------*/
                    return;
                  }
                }
                else
                {
                  if(ppp_ncp_abort(
                          ds707_pdncntx_get_ncp_handle( pdncntx_cb_ptr )) < 0)
                  {
                    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"call to ncp_abort failed!"
                                                    "Post link_down to cleanup"
                                                    "dssnet6");
                    (void) dssnet6_sm_post_event( instance_ptr, 
                                                  DSSNET6_LINK_DOWN_EV, 
                                                  NULL );
                    return;
                  }
                }
              }
            }/*if EPC system*/
            else
            {
              /*------------------------------------------------------------- 
                Check if phone is in OOS. If it is & v4 iface is in use, tear
                down v6 only
              -------------------------------------------------------------*/
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
              if (FALSE == ds707_get_has_phone_acquired_signal())
              {
                pdn_v4_iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);

                if((! PS_IFACEI_IS_IN_ARBITRATION(instance_ptr->ps_iface_ptr))
                   && (PS_IFACE_IS_VALID(pdn_v4_iface_ptr)) 
                   && (PS_IFACE_IS_IN_USE(pdn_v4_iface_ptr))
                   && (!ds707_is_term_in_progress()) )
                {
                  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                                  "V4 iface in use, tear down only v6");
                  (void) ps_iface_go_null_cmd( instance_ptr->ps_iface_ptr,
                                               &ps_errno, 
                                               NULL );
                  dssnet6i_transition_state(instance_ptr, 
                                            DSSNET6I_CLOSED_STATE);
                  break;
                }
              }
#endif/*FEATURE_DATA_EHRPD_DUAL_IP*/

              /*------------------------------------------------------------- 
                Since system is not eHRPD anymore, call ncp_abort. Don't
                call ppp_abort / ppp_device_reset since this will bring
                down entire PPP i.e. other NCPs as well. Tearing down other
                NCPs is a problem in case of eHRPD to LTE HO, since one PDN
                is transferred independent of other PDNs. So when one PDN
                is successfully handedover and we want to abort it on eHRPD
                side, other PDNs may still be in the process of HO. That
                time we dont want to abort PPP (LCP). Instead it is better
                to abort only this NCP. PPP module will take care of aborting
                LCP when the last NCP is aborted by MH. 
              -------------------------------------------------------------*/
              DATA_3GMGR_MSG0(MSG_LEGACY_MED, 
                              "Current system NON-EPC, silent teardown");
              if(ppp_ncp_abort(
                          ds707_pdncntx_get_ncp_handle( pdncntx_cb_ptr )) < 0)
              {
                DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"call to ncp_abort failed!"
                                                "Post link_down to cleanup"
                                                "dssnet6");
                (void) dssnet6_sm_post_event( instance_ptr, 
                                              DSSNET6_LINK_DOWN_EV, 
                                              NULL );
                return;
              }
            }

            ds707_pdncntx_set_ncp_started(pdncntx_cb_ptr, FALSE);
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"NCP already down");
          }
        }
        else
#endif /*FEATURE_EHRPD */
        {
          phys_link_ptr =  PS_IFACE_GET_PHYS_LINK(instance_ptr->ps_iface_ptr);
          if(phys_link_ptr != NULL )
          {
            phys_flow_mask = ps_phys_link_get_flow_mask(phys_link_ptr);
          }
          DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                          "DSSNet6 0x%x Closing PPP on Iface" ,instance_ptr);
          if ((ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_NON_EPC_SYSTEM)
             && ((DS_FLOW_CTRL_IS_BIT_SET(
                 phys_flow_mask,
                 DS_FLOW_NO_SERVICE_MASK)) != TRUE))
#endif /* FEATURE_DATA_IS707 */
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_MED, "Current system NON-EPC & "
                            "flow mask not set to no service, gracefull teardown");
            (void) ppp_stop(PPP_UM_SN_DEV, PPP_PROTOCOL_IPV6CP);
          }
#ifdef FEATURE_DATA_IS707
          else
          {
            /*------------------------------------------------------------- 
              Check if phone is in OOS. If it is & v4 iface is in use, tear
              down v6 only
            -------------------------------------------------------------*/
            if (FALSE == ds707_get_has_phone_acquired_signal())
            {
              pdn_v4_iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);

              if((! PS_IFACEI_IS_IN_ARBITRATION(instance_ptr->ps_iface_ptr))
                 && (PS_IFACE_IS_VALID(pdn_v4_iface_ptr)) 
                 && (PS_IFACE_IS_IN_USE(pdn_v4_iface_ptr)))
              {
                DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                                "V4 iface in use, tear down only v6");
                (void) ppp_stop(PPP_UM_SN_DEV, PPP_PROTOCOL_IPV6CP);
                break;
              }
            }

            DATA_3GMGR_MSG0(MSG_LEGACY_MED, "Current system EPC or "
                            "flow mask set to no service, silent teardown");
            (void) ppp_abort(PPP_UM_SN_DEV);
          }
        }
#endif /* FEATURE_DATA_IS707 */
      }

      /*---------------------------------------------------------------------
        Disable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow( instance_ptr->ps_iface_ptr, 
                             DS_FLOW_CTRL_DSSNET6_MASK);
      break;


    case DSSNET6I_LINK_RESYNC_STATE:
      /*---------------------------------------------------------------------
        Disable flow to apps, set curr_hop_limit to default
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow( instance_ptr->ps_iface_ptr, 
                             DS_FLOW_CTRL_DSSNET6_MASK);

      ps_iface_configuring_ind(instance_ptr->ps_iface_ptr);

      instance_ptr->ps_iface_ptr->net_info.net_ip.v6.curr_hop_limit =
      IP_DEF_TTL;

      /*---------------------------------------------------------------------
        Exit IPv6.
      ---------------------------------------------------------------------*/ 
      if ((ip6_sm_post_event( instance_ptr->ip6_instance,
                              IP6_SM_STOP_EV, NULL)) == -1)
      {
        DATA_ERR_FATAL("ip6_sm_post_event() failed ");
      }

      if (instance_ptr->use_ppp)
      {
#if defined (FEATURE_EHRPD) && defined (FEATURE_DATA_IS707)
        if(instance_ptr->is_ehrpd_call == TRUE)
        {
          pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                        instance_ptr->ps_iface_ptr);
          if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
          {
            DATA_ERR_FATAL("Cannot get PDN Instance to set call bringup mode");
            return; 
          }
          /*-------------------------------------------------------------------
              Set the call bringup mode flag again since PPP resync happened
          -------------------------------------------------------------------*/
          ds707_pdncntx_set_call_bringup_mode(pdncntx_cb_ptr);

          /*-------------------------------------------------------------------
            Set NCP resync started flag to TRUE
          -------------------------------------------------------------------*/
          /* Companion iface will also go into resync and might have already 
             set this flag */
          if (ds707_pdncntx_get_ncp_resync_started(pdncntx_cb_ptr) == FALSE)
          {
            ds707_pdncntx_set_ncp_resync_started(pdncntx_cb_ptr, TRUE);
          }
        }
#endif /*FEATURE_EHRPD && FEATURE_DATA_IS707 */
      }

      break;

    case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
      ps_iface_disable_flow( instance_ptr->ps_iface_ptr, 
                             DS_FLOW_CTRL_DSSNET6_MASK);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid state %d ", 
                      (int)instance_ptr->state);
      /*lint -save -e506, -e774 */
      ASSERT(0);
      /*lint -restore */
  } /* switch(state) */

} /* dssnet6i_transition_state() */

/*===========================================================================
FUNCTION DSSNET6I_CONFIGURE_PPP()

DESCRIPTION
 Configure the PPP Parameters
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssnet6i_configure_ppp
(
  dssnet6_sm_cb_type *instance_ptr,            /* dssnet6 instance ptr      */
  ppp_dev_opts_type *ppp_config_ptr
)
{
  uint64               iface_id = 0;             /* 64 bit interface id    */
  uint64               local_iid;
  ds3g_ipv6_iid_type   iid_info;
  int16                ds_errno;
#ifdef FEATURE_DATA_IS707
  ds707_data_session_profile_info_type *data_sess_prof_ptr = NULL;
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
#endif /*FEATURE_DATA_IS707*/

  sys_modem_as_id_e_type            resolved_subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!(DSSNET6I_IS_VALID(instance_ptr)) || (NULL == ppp_config_ptr))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid dssnet6 instance_ptr");
    return;
  }

  /*----------------------------------------------------------------------
   Setup PPP configuration options. 
  ---------------------------------------------------------------------*/
  ppp_get_default_opts(ppp_config_ptr);

  ppp_config_ptr->hdlc_mode = HDLC_HW_PREF;
  ppp_config_ptr->lcp_info.mode = PPP_CP_ACTIVE;
#ifdef FEATURE_DATA_IS707
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                                  instance_ptr->ps_iface_ptr);
  ds707_pdncntx_get_profile(pdncntx_cb_ptr, &data_sess_prof_ptr);
#endif /*FEATURE_DATA_IS707*/

  if( ds3g_resolve_subs_id(PS_IFACE_GET_SUBS_ID(instance_ptr->ps_iface_ptr),
                           &resolved_subs_id )       ==   FALSE) 
  {
    DATA_IS707_MSG2(MSG_LEGACY_LOW,"Error in obataining resolved sub id %d "
                       "Policy Sub ID %d",PS_IFACE_GET_SUBS_ID(instance_ptr->ps_iface_ptr)
                       ,resolved_subs_id);
    ASSERT(0);
  }

#ifdef FEATURE_DATA_IS707
#if 0 
  /* Not required to check for subs_id, because the pref pkt_mode_ex API
  Would return false if CDMA is not supported on the passed subs_id .
  This is also to support if CDMA can be supported on SUBS_ID 2 */
  if ( resolved_subs_id != SYS_MODEM_AS_ID_1 ) //DSDA do we need this check?
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR," resolved sub id %d is not supported"
                                     " for CDMA Call orig Policy Sub ID %d",
                                      PS_IFACE_GET_SUBS_ID(instance_ptr->ps_iface_ptr)
                                      ,resolved_subs_id);
      return FALSE;
  }
#endif 
  if (ds707_is_apn_pref_pkt_mode_ex(data_sess_prof_ptr, SYS_SYS_MODE_CDMA,resolved_subs_id))
  {
    // As of PS12, rx_f_ptr is no longer used, as it is PPP specific.
    // Instead, we always use rx_sig_f_ptr, even for PPP flows.
    ppp_config_ptr->rx_f_ptr       = NULL;
    ppp_config_ptr->rx_sig_f_ptr = ds707_rx_UM_SN_sig_hdlr;
    ppp_config_ptr->tx_f_ptr = ds707_pkt_tx_um_SN_data;
    /*-----------------------------------------------------------------------
      Setting max ppp setup time to 40 sec. This is needed in case there is
      no response from the network during auth phase then this timer will
      termiate the PPP session on the AT side.
    -----------------------------------------------------------------------*/
    ppp_config_ptr->setup_timeout = 40;
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_is_pref_pkt_mode is not CDMA");
  }
#endif /*FEATURE_DATA_IS707*/
  if (instance_ptr->qnc_call == TRUE)
  {
    /*-------------------------------------------------------------------
     Disable PPP auth. 
    -------------------------------------------------------------------*/
    ppp_config_ptr->lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
    ppp_config_ptr->lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP;
    ppp_config_ptr->lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
    ppp_config_ptr->lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;
  }
#ifdef FEATURE_DATA_IS707
  else if (ds707_is_apn_pref_pkt_mode_ex(data_sess_prof_ptr, SYS_SYS_MODE_CDMA,resolved_subs_id))
  {
    /*-------------------------------------------------------------------
      Retrieve auth info from NV  
    -------------------------------------------------------------------*/
    is707_get_ppp_auth_info_from_nv( &(ppp_config_ptr->auth_info), 
                                     ds3gcfgmgr_read_legacy_nv_ex );
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_is_pref_pkt_mode is not CDMA");
  }
#endif /*FEATURE_DATA_IS707*/

  /*---------------------------------------------------------------------
    Retrieve ipv6 iid from NV if set otherwise generate randomly.
  ---------------------------------------------------------------------*/
  ds3g_get_ipv6_iid_info( resolved_subs_id, &iid_info );
  if( iid_info.iid_enum_type == IPV6_USER_IID)
  {
    local_iid = iid_info.iid_val;
    iface_id  = ps_htonll(local_iid);
  }
  else
  {
    /*-------------------------------------------------------------------
      Use randomly generated iid if we are unable to read the NV or
      we read the NV item and the type if iid is random.
    -------------------------------------------------------------------*/
    (void) ps_iface_generate_ipv6_iid(instance_ptr->ps_iface_ptr,
                                      &iface_id,
                                      &ds_errno);
  }

  ppp_config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iid = iface_id;
  
  instance_ptr->ps_iface_ptr->v6_net_info.primary_dns 
  = instance_ptr->nv_primary_dns;
  instance_ptr->ps_iface_ptr->v6_net_info.secondary_dns
  = instance_ptr->nv_secondary_dns;
  instance_ptr->ps_iface_ptr->net_info.net_ip.v6.curr_hop_limit =
  IP_DEF_TTL;

#ifdef FEATURE_DATA_IS707
  if (ds707_roaming_is_curr_mode_jcdma() == TRUE) 
  {
    ds707_configure_jcdma_params_for_ppp(ppp_config_ptr);
  }
#endif /* FEATURE_DATA_IS707 */
/*lint -restore Restore lint error 641*/
} /* dssnet6i_configure_ppp() */

/*=========================================================================
FUNCTION       DSSNET6_SET_PS_CALL_END_REASON

DESCRIPTION    This function sets the call end reason for the dssnet6 
               instance ptr 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_set_ps_call_end_reason
(
  dssnet6_sm_cb_type *instance_ptr,           /* dssnet6 instance ptr      */
  ps_iface_net_down_reason_type call_end_reason
)
{
  if ( (instance_ptr != NULL) &&
       (instance_ptr->call_end_reason == PS_NET_DOWN_REASON_NOT_SPECIFIED) )
  {
    instance_ptr->call_end_reason = call_end_reason;
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, 
                    "call end reason for dssnet6 0x%x set to 0x%x", 
                    instance_ptr, call_end_reason);
  }
  else
  {
    if (instance_ptr != NULL)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, 
                      "call end reason already set in inst_ptr: %d", 
                      instance_ptr->call_end_reason);
    }
  }
} /* dssnet6_set_ps_call_end_reason() */

/*=========================================================================
FUNCTION       DSSNET6_SET_PS_CALL_END_REASON

DESCRIPTION    This function sets the local abort for the dssnet6 
               instance ptr 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_set_ps_local_abort
(
  dssnet6_sm_cb_type *instance_ptr,           /* dssnet6 instance ptr      */
  boolean             local_abort             /* local abort              */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (instance_ptr == NULL)
  {
    DATA_ERR_FATAL("Instance pointer for dssnet6_set_ps_local_abort is NULL");
  }
  else
  {
    instance_ptr->local_abort = local_abort;
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, 
                    "local abort for dssnet6 0x%x,set to %d", 
                    instance_ptr,
                    local_abort);
  }
} /* dssnet6_set_ps_call_end_reason() */
/*=========================================================================
FUNCTION       DSSNET6_RESET_PS_CALL_END_REASON

DESCRIPTION    This function resets the call end reason for the dssnet6 
               instance ptr 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_reset_ps_call_end_reason
(
  dssnet6_sm_cb_type *instance_ptr
)
{
  if (instance_ptr != NULL)
  {
    instance_ptr->call_end_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, 
                    "call end reason for dssnet6 0x%x is reset", 
                    instance_ptr);
  }
} /* dssnet6_reset_ps_call_end_reason() */

/*===========================================================================
FUNCTION DSSNET6_GET_PS_CALL_END_REASON

DESCRIPTION    This function gets end_call_reason to ps. If CM has set call 
               end reason, propogate that. 

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void dssnet6_get_ps_call_end_reason
(
  dssnet6_sm_cb_type *instance_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!(DSSNET6I_IS_VALID(instance_ptr)))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid dssnet6 instance_ptr");
    return;
  }

  /* no fail reason is set, get CM call end reason */
  if (instance_ptr->call_end_reason == PS_NET_DOWN_REASON_NOT_SPECIFIED)
  {
#ifdef FEATURE_DATA_IS707
    if (instance_ptr->use_ppp == TRUE)
    {
      dssnet6_set_ps_call_end_reason(instance_ptr, 
                                    ds707_pkt_mgr_get_last_call_end_reason());
      DATA_3GMGR_MSG1(MSG_LEGACY_MED, 
                      "Convert CM call end reason to PS call end reason 0x%x ", 
                      instance_ptr->call_end_reason);
    }
#endif
  }
} /* dssnet6_get_ps_call_end_reason() */

#ifdef FEATURE_DATA_IS707
/*=========================================================================
FUNCTION       DSSNET6I_CONVERT_PPP_FAILURE_CODE

DESCRIPTION    This function converts the ppp failure code to 
               corresponding ps iface net down reason 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
ps_iface_net_down_reason_type dssnet6i_convert_ppp_failure_code
(
  ppp_fail_reason_e_type reason
)
{
  ps_iface_net_down_reason_type end_call_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;

  if ( (reason >= PPP_FAIL_REASON_VSNCP_TIMEOUT) &&
       (reason < PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MAX) )
  {
    end_call_reason = (ps_iface_net_down_reason_type)(DSSNET6_VSNCP_ERR_CODE_MASK | (int32)reason );
  }
  else
  {
    if ( ((reason > PPP_FAIL_REASON_NONE) &&
          (reason <= PPP_FAIL_REASON_OPTION_MISMATCH)) || 
         ((reason >= PPP_FAIL_REASON_AUTH_PAP_FAILURE) &&
          (reason < PPP_FAIL_REASON_MAX))
       )
    {
      end_call_reason = (ps_iface_net_down_reason_type)(DSSNET6_PPP_ERR_CODE_MASK | (int32)reason );
    }
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, 
                  "dssnet6: PPP error code: %d, corresponding PS net down reason: 0x%x ", 
                  reason, end_call_reason);

  return end_call_reason;
}
#endif

/*=========================================================================
FUNCTION       DSGEN_IFACE6_HDLR_PROCESS_CONN_DELAY_TIMER_EV

DESCRIPTION    This function handles the connection release delay timer expiry
               event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dsgen_iface6_hdlr_process_conn_delay_timer_ev
(
  dssnet6_sm_cb_type  *instance_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*-------------------------------------------------------------------
  Transition the state to CLOSED_STATE
  -------------------------------------------------------------------*/
  if( instance_ptr != NULL )
  {
    dssnet6i_transition_state(instance_ptr, DSSNET6I_CLOSED_STATE);
  }
} /* dsgen_iface6_hdlr_process_conn_delay_timer_ev() */

/*=========================================================================
FUNCTION       DSSNET6_EVENT_CALLBACK_REG

DESCRIPTION    This function handles the connection release delay timer expiry
               event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
int dssnet6_event_callback_reg
(
  dssnet6_sm_cb_type  *instance_ptr,
  dssnet6_event_cback_type  cback,
  void              *user_data
)
{
  if (instance_ptr == NULL)
  {
    return -1;
  }
  instance_ptr->event_cb = cback;
  instance_ptr->event_cb_user_data = user_data;

  return 0;
}

/*=========================================================================
FUNCTION       dssnet6_enter_crit_sect

DESCRIPTION    This function enters the dssnet6 crit sect
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_enter_crit_sect ( void )
{
  rex_enter_crit_sect(&dsgen_iface6_global_crit_sect);
}

/*=========================================================================
FUNCTION       dssnet6_leave_crit_sect

DESCRIPTION    This function leaves the dssnet6 crit sect
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_leave_crit_sect ( void )
{
  rex_leave_crit_sect(&dsgen_iface6_global_crit_sect);
}

/*=========================================================================
FUNCTION       dssnet6_iface_down_ind_callback_reg

DESCRIPTION    This function registers for v6 iface down ind event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
int dssnet6_iface_down_ind_callback_reg
(
  dssnet6_sm_cb_type           *instance_ptr,
  dssnet6_iface_down_ind_cb_type  cb_fn
)
{
  if (instance_ptr == NULL)
  {
    return -1;
  }
  instance_ptr->iface_down_ind_cb = cb_fn;

  return 0;
}

/*=========================================================================
FUNCTION       DSSNET6_GET_INSTANCE_STATE

DESCRIPTION    Returns dssnet6 instance state
               
DEPENDENCIES   NONE

RETURN VALUE   Failure - DSSNET6I_MAX_STATE
               Success - dssnet6 instance state

SIDE EFFECTS   NONE
=========================================================================*/
dssnet6i_state_type dssnet6_get_instance_state
(
  dssnet6_sm_cb_type *dssnet6_sm_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == dssnet6_sm_cb)
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "dssnet6_sm_cb is NULL");
    return DSSNET6I_MAX_STATE;
  }

  return (dssnet6_sm_cb->state);

} /* dssnet6_get_instance_state() */

/*=========================================================================
FUNCTION       DSSNET6_GET_INSTANCE_CALL_END_REASON

DESCRIPTION    Returns dssnet6 instance call end reason
               
DEPENDENCIES   NONE

RETURN VALUE   Failure - PS_NET_DOWN_REASON_MAX
               Success - dssnet6 instance call end reason

SIDE EFFECTS   NONE
=========================================================================*/
ps_iface_net_down_reason_type dssnet6_get_instance_call_end_reason
(
  dssnet6_sm_cb_type *dssnet6_sm_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == dssnet6_sm_cb)
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "dssnet6_sm_cb is NULL");
    return PS_NET_DOWN_REASON_MAX;
  }

  return (dssnet6_sm_cb->call_end_reason);

} /* dssnet6_get_instance_call_end_reason() */

/*=========================================================================
FUNCTION       DSSNET6_RESET_CALL_END_REASON

DESCRIPTION    This function resets end_call_reason to 
               PS_NET_DOWN_REASON_NOT_SPECIFIED 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_reset_call_end_reason
(
  dssnet6_sm_cb_type            *dssnet6_sm_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == dssnet6_sm_cb)
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW,
                    "dssnet6_sm_cb is NULL. Can't reset call "
                    "end reason");
    return;
  }

  dssnet6_sm_cb->call_end_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  return;

} /* dssnet6_set_instance_call_end_reason() */


/*=========================================================================
FUNCTION       DSSNET6_RESET_LOCAL_ABORT

DESCRIPTION    This function resets local_abort to 
               FALSE 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_reset_local_abort
(
  dssnet6_sm_cb_type            *dssnet6_sm_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == dssnet6_sm_cb)
  {
    DATA_ERR_FATAL("dssnet6_sm_cb is NULL. Can't reset call local abort");
  }
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, 
                    "local abort for dssnet6 0x%x,reset to FALSE", 
                    dssnet6_sm_cb);
  dssnet6_sm_cb->local_abort = FALSE;
  return;

} /* dssnet6_reset_local_abort() */

/*=========================================================================
FUNCTION       DSSNET6_SET_IS_BRINGUP_PHYS_LINK

DESCRIPTION    Set is_bringup_phys_link flag which will be used to remember 
               that phys link needs to be brought up again after it is
               cleaned up. This is set to TRUE when phys link bringup fails
               due to phys link being in going_null state
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_set_is_bringup_phys_link
(
  dssnet6_sm_cb_type            *instance_ptr,
  boolean                        is_bringup_phys_link
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!(DSSNET6I_IS_VALID(instance_ptr)))
  {
    DATA_ERR_FATAL("dssnet6_sm_cb is not valid");
    return;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED, 
                  "dssnet6 0x%x, set is_bringup_phys_link = %d",
                  instance_ptr, is_bringup_phys_link);
  instance_ptr->is_bringup_phys_link = is_bringup_phys_link;
  return;
} /* dssnet6_set_is_bringup_phys_link() */

/*=========================================================================
FUNCTION       DSSNET6_GET_IS_BRINGUP_PHYS_LINK

DESCRIPTION    Get is_bringup_phys_link flag, which will be used to remember 
               that phys link needs to be brought up again after it is
               cleaned up.
               
DEPENDENCIES   NONE

RETURN VALUE   is_bringup_phys_link value

SIDE EFFECTS   NONE
=========================================================================*/
boolean dssnet6_get_is_bringup_phys_link
(
  dssnet6_sm_cb_type            *instance_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!(DSSNET6I_IS_VALID(instance_ptr)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "instance_ptr is not valid");
    return FALSE;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED, 
                  "dssnet6 0x%x, get is_bringup_phys_link = %d",
                  instance_ptr, instance_ptr->is_bringup_phys_link);
  return (instance_ptr->is_bringup_phys_link);
} /* dssnet6_get_is_bringup_phys_link() */

/*=========================================================================
FUNCTION       DSSNET6I_GET_IPV6_PREFIX_DELEGATION_MODE

DESCRIPTION    This utility returns if the IPV6 prefix delegation 
               is enabled 
               
DEPENDENCIES   NONE

RETURN VALUE 
    TRUE  - If IPV6 Prefix delegation is enabled
    FALSe - If IPV6 prefix delegation is not enabled

SIDE EFFECTS   NONE
=========================================================================*/
static boolean dssnet6i_get_ipv6_prefix_delegation_mode
(
  ps_iface_type       *ps_iface_p
)
{

  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ps_iface_p == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "PS iface is NULL"); 
    return ret_val;
  }

  if (ps_iface_p->name == CDMA_SN_IFACE)
  {
#ifdef FEATURE_DATA_IS707  
    ret_val = ds707_data_session_get_iface_is_v6_delegation_enabled(ps_iface_p);
#endif /* FEATURE_DATA_IS707 */
  }
  else if (ps_iface_p->name == UMTS_IFACE)
  {
    ret_val = ds_3gpp_get_ipv6_prefix_delegation_mode(ps_iface_p);
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"IPV6 prefix delegation mode not supported"
                                     "on PS iface type 0x%x",ps_iface_p);
  }
  return ret_val;
}

#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
