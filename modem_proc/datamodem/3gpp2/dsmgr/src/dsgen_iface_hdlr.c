/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S G E N _ I F A C E _ H D L R . C
                             
                           (formerly dssnet.c)

DESCRIPTION
  Implementation of the external API for the dssnet state machines. This file 
  provides the external API to the dssnet state machines and also has some
  of the common functionality that is used by both the dssnet state machines.

DEPENDENCIES
  The module MUST execute in the DS context.

EXTERNALIZED FUNCTIONS
  dssnet_start()
   This function is registered with ps_iface to bring up the associated iface
   during call establishment.

  dssnet_stop()
   This function is registered with ps_iface to tear down the associated
   iface during call release.

  dssnet_ppp_ncp_setup()
   This function is used by the dssnet state machines to setup the
   corresponding NCPs (IPCP/IP6CP) during PPP establishemnt.

 Copyright (c) 2004-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/dsgen_iface_hdlr.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/09/15    sd     Fallback support for CHAP challenge response calculation.
07/18/14    hr     To support default APN
10/11/12    sd     Compilation Fixes
08/13/12    ss     Moving efs write operation to dsutil performed during 
                   bootup.
04/18/12    jz     Seperate old JCDMA and Simplified JCDMA 
04/12/12    jee    To propagate correct call end reason from MH
02/21/12    msh    Fixed compiler warning due to missing header file 
01/26/12    msh    Read internal auth nv item 
01/16/12    ack    Fix to post a cmd for iface bringup/teardown
07/07/11    vpk    Use NV params for PPP Auth when 
                   FEATURE_DATA_PS_INTERNAL_AUTH is defined
06/06/11    ash    Added support for PPP Partial Context
05/05/11    op     Removed DS707_DATA_SESSION_PROFILE_ID_IS_EHRPD macro
03/14/11    mga    Merged changes to set the MTU accordingly
03/04/11    ls     Fixed CW
01/28/11    jee    JCDMA feature cleanup & PPP re-sync handling
10/14/10    sa     Adding debug messages before ASSERT.
09/09/10    op     Migrated to MSG 2.0 macros
06/28/10    ss     Fixed compiler warnings.
21/04/10    ms     Fixed KW errors.
09/22/09   vrk     Merged LTE related changes.
09/18/09    ss     DataCommon CMI SU level impacts.
05/11/09    ms     Updated CommonData variable names.
03/20/09    ls     dssnet_ppp_setup_lcp() shall use want_vals with FSM_REMOTE
                   when setting authentication protocol mask
03/12/09  ssh/spn  PPP VSNCP interface changes
02/09/09    spn    Added support for VSNCP for eHRPD. 
12/05/08    sk     EAP support for EHRPD
06/17/08    sn     Fixed Klocwork errors.
12/18/07    sn     Fixed compiler warning and lint errors.
12/04/07    sc     Changes to setting LCP, IPCP and AUTH values
08/20/07    sc     Added debug messages for Application Profiles support
07/23/07    sc     Fixed errors caused by non featurized 707 specific code 
                   in dssne_ppp_setup_ncp(), dssne_ppp_setup_lcp() and
                   dssnet_start()
07/02/07    sc     Changed the params passed to ps_iface_phys_link_up_cmd 
                   inside dssnet_start().Also added support for Application 
                   Profiles in dssnet_ppp_setup_ncp().
12/01/06    ks     Setting the auth_proto_mask in ppp_config to indicate
                   auth proto preference for rmnet calls.
09/05/06    ks     Updated dssnet_ppp_setup_ncp() to use new Net params 
                   struct to copy auth params from ps_iface.
03/29/05    ks     Changed inline ps_iface_is_valid() to macro.
09/09/05    jd     Copy PS iface authentication parameters, if specified, to
                   PPP device setup when dsgen_ppp_setup_ncp is called.
07/22/05    kvd    Pass NULL 3rd arg to dssnet4_sm_post_event().
04/20/05    mct    Flow changes. Activate flow before bringing up phys_link.
09/10/04    sb      S-PDP changes
08/23/04    mvl    Fixed featurization issue in ppp_setup_ncp() - function is
                   now setup to do nothing if the build doesn't support IS707
08/17/04    mvl    If IPv6 is not enabled then IPv6CP will be set to disabled
                   mode.
08/01/04    kvd    Replaced GET_PS_IFACE macros with
                   ds707_pkt_get_IPV4_iface_ptr function calls.
07/30/04    kvd    Renamed dssnet_bring_up_cmd, dssnet_tear_down_cmd to
                   dssnet_start & dssnet_stop.
07/30/04    ifk    In dssnet_ppp_setup_ncp() set iface of discard NCP
07/15/04    ifk    Renamed cp_mode_type values
06/11/04    vp     Changes due to changes in PS data structures.
04/27/04    kvd    start mip_perf stats on bring_iface_up.
04/21/04    mct    Renamed ds_dssnet6.h dsgen_iface6_hdlr.h.
04/21/04    mct    Changed PPP_CP_PASSIVE to PPP_CP_IGNORE to ignore NCPs
                   brought up passively for IPV6 IOT.
04/09/04    rc     Fixed featurization problem with IS707 files.
03/31/04    sv     Moved phys link event call back registration after the
                   bringup_phys_link up call.
03/20/04    ak     Changed name at top of file, to match file name change.
03/19/04    ak     Some IPV6 featurization.
03/18/04    aku    Register for Phys link down and gone indications when
                   common dssnet bringup function is call and deregister
                   when transitioning to CLOSED state.
03/12/04    aku    Register for phys link up indication only when common 
                   dssnet bringup cmd is called. Deregister this indication
                   when processing the PHYS_LINK_UP_EV.
01/12/04    aku    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "data_msg.h"
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST 
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
#include "dsgen_iface_hdlr.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_roaming.h"
#include "ds3gmgr.h"
#include "ds3gutil.h"
#include "ds707_iface4_hdlr.h"
#include "ds707_pkt_mgr.h"
#include "ps_ppp.h"
#include "ds707_pdn_context.h"
#ifdef FEATURE_EPC_HANDOFF
#include "modem_mem.h" 
//#include "ds_epc_hdlr.h"
#include "ps_in.h"
#include "ds707_pkt_mgri.h"
#endif /* FEATURE_EPC_HANDOFF */
#endif /*FEATURE_DATA_IS707*/

#ifdef FEATURE_DATA_PS_IPV6
  #include "dsgen_iface6_hdlr.h"
  #include "ps_iface_addr_v6.h"
  #include "ps_ifacei_utils.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "rex.h"
#include "ps_route.h"

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#ifdef FEATURE_EHRPD
#include "ds707_sec_pkt_mgr.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "dstaski.h"
#include "ps_ppp_vsncp_3gpp2.h"
#include "ds_ehrpd_pdn_mgr.h"
#include "ds707_data_session_profile.h" // double check if this is needed
#ifdef FEATURE_NIQ_EHRPD
#include "ds707_nw_init_qos_mgr.h"
#endif /* FEATURE_NIQ_EHRPD */
#endif /* FEATURE_EHRPD */
#include "data_err_suppr.h"
#include "dsutil.h"
#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/



/*===========================================================================

                         INTERNAL DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DATA_IS707

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSSNETI_FIND_OTHER_IFACE()

DESCRIPTION
  This function is to be used only in 1x/HRPD system only. The assumption is 
  that in 1x/HRPD there can only be one v4 iface and one v6 iface. 
  This function is required to return the "other" iface to be filled up in 
  the ppp_config block, while setting up the NCP, because of a limitation 
  that there is only one ppp_config block for IPv4CP and IPv6CP.

  In case this is the first iface coming up, the other iface ptr can be
  any other valid IFACE ptr.

  In case this is the second iface ( v4 or v6 ) coming up, find the other 
  iface ptr which will be of other type for sure.

PARAMETERS
  Pointer to ps_iface on which to operate

RETURN VALUE
  None

DEPENDENCIES
  Assumes that as of now there are two types of CDMA_SN_IFACES. v4 and v6.
  Specific to 1x/DO only.

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type* dssneti_find_other_iface
(
  ps_iface_type *iface_ptr,
  boolean is_v4_iface
)
{
  ps_iface_type* ret_iface_ptr = NULL;
  uint8 curr_iface_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {
    ret_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index( 
                                                         curr_iface_index );
    /* If the iface is not the one that is passed in and is not of the 
       same address family that is passed in and is in use return the iface */
    if( (ret_iface_ptr != iface_ptr) &&
        (PS_IFACE_IS_IN_USE(ret_iface_ptr) ) &&
        (ps_iface_addr_family_is_v4(ret_iface_ptr) != is_v4_iface ) ) 
    {
      break;
    }
  }

  /* If we did not find an iface, return an iface that is not in use */
  if ( curr_iface_index == DS707_MAX_IFACES )
  {
    ret_iface_ptr = NULL;
    for ( curr_iface_index = 0;
          curr_iface_index < DS707_MAX_IFACES;
          curr_iface_index++ )
    {
      ret_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index( 
                                                         curr_iface_index );
      if ( !PS_IFACE_IS_IN_USE(ret_iface_ptr) )
      {
        break;  
      }
    }
  }

  /*If we did not find an unused iface, so that we assert before returning */
  if ( curr_iface_index == DS707_MAX_IFACES )
  {
    ret_iface_ptr = NULL;
  }

  ASSERT( PS_IFACE_IS_VALID(ret_iface_ptr) );

  return ret_iface_ptr;

} /* dssneti_find_other_iface() */


#endif /* FEATURE_DATA_IS707 */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DSSNET_START()

DESCRIPTION
   This function is registered with ps_iface to bring up the associated iface
   during call establishment.

PARAMETERS
  Pointer to ps_iface on which to operate
  Void ptr to client data

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int dssnet_start
(
  ps_iface_type *this_iface_ptr,
  void* dssnet_instance,
  int16 *p_errno
)
{

  int              ret_val;
  void             *pkt_call_info_ptr = NULL; /*user info*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (NULL == this_iface_ptr)  ||
      (NULL == dssnet_instance) ||
      (NULL == p_errno))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid argument passed");
    return -1;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"Starting dssnet for ps_iface 0x%x:%d",
                  this_iface_ptr->name,
                  this_iface_ptr->instance);

  ps_flow_activate_ind( PS_IFACEI_GET_DEFAULT_FLOW(this_iface_ptr),
                        PS_EIC_NOT_SPECIFIED);

#ifdef FEATURE_DATA_IS707
  
  /*------------------------------------------------------------------------
    pass valid value as user info to phys_link_up_cmd. Create a struct of
    type ds3g_pkt_call_info_type and pass it as user_info. this  function
    populates pkt_call_info and returns void*.
  ------------------------------------------------------------------------*/
  pkt_call_info_ptr = ds707_pkt_mgr_setup_pkt_call_info(this_iface_ptr); 
#endif

  ret_val = ps_iface_phys_link_up_cmd ( this_iface_ptr,
                                        p_errno,
                                        pkt_call_info_ptr );
  if ((ret_val < 0) && ( *p_errno != DS_EWOULDBLOCK))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "phy link up fail,%d phys_link ret" , *p_errno);

    /*-------------------------------------------------------------------------
      Check if phys link is cleanig up (GOING_NULL/GOING_DOWN state) & set 
      is_bringup_phys_link to TRUE. This will be used to bringup the phys link 
      later when it goes NULL/DOWN
    -------------------------------------------------------------------------*/
    if ((DS_ENETCLOSEINPROGRESS == *p_errno) 
        || (DS_ENETGOINGDORMANT  == *p_errno))
    {
#ifdef FEATURE_DATA_PS_IPV6
      if(ps_iface_addr_family_is_v6(this_iface_ptr))
      {
        dssnet6_set_is_bringup_phys_link(
                                (dssnet6_sm_cb_type *)(dssnet_instance), TRUE);
      }
      else
#endif
      {
#ifdef FEATURE_DATA_IS707
        dssnet4_set_is_bringup_phys_link(
                                (dssnet4_sm_cb_type *)(dssnet_instance), TRUE);
#endif /* FEATURE_DATA_IS707 */
      }
    }

    else
    {
    return ret_val;
  }
  }

  /*-------------------------------------------------------------------------
    Register the phys_link_up event for the appropriate dssnet instance.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if(ps_iface_addr_family_is_v6(this_iface_ptr))
  {
    /*-------------------------------------------------------------------------
      Clear the call end reason
    -------------------------------------------------------------------------*/
    dssnet6_reset_ps_call_end_reason((dssnet6_sm_cb_type *)(dssnet_instance));
    dssnet6_reset_local_abort((dssnet6_sm_cb_type *)(dssnet_instance));

    if(-1 == 
         dssnet6_reg_phys_link_events((dssnet6_sm_cb_type *)dssnet_instance))
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Couldn't reg dssnet6 phys_link events");
      ret_val = ps_iface_phys_link_down_cmd( this_iface_ptr, 
                                             p_errno, 
                                             pkt_call_info_ptr );
      if ((ret_val < 0) && ( *p_errno != DS_EWOULDBLOCK))
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "phy link down fail,%d phys_link ret",
                        *p_errno);
        return ret_val;
      }
      return -1;
    }
  }
  else
#endif /* FEATURE_DATA_PS_IPV6 */
  {
#ifdef FEATURE_DATA_IS707
    /*-------------------------------------------------------------------------
      Clear the call end reason
    -------------------------------------------------------------------------*/
    dssnet4_reset_call_end_reason((dssnet4_sm_cb_type *)(dssnet_instance));
    dssnet4_reset_local_abort((dssnet4_sm_cb_type *)(dssnet_instance));

    if(-1 == 
         dssnet4_reg_phys_link_events((dssnet4_sm_cb_type *)dssnet_instance))
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Couldn't reg dssnet4 phys_link events");
      ret_val = ps_iface_phys_link_down_cmd( this_iface_ptr, 
                                             p_errno, 
                                             pkt_call_info_ptr );
      if ((ret_val < 0) && ( *p_errno != DS_EWOULDBLOCK))
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "phy link down fail,%d phys_link ret",
                        *p_errno);
        return ret_val;
      }
      return -1;
    }
#else
    DATA_ERR_FATAL("FEATURE_DATA_IS707 not defined");
#endif /* FEATURE_DATA_IS707 */
  }

  return 0;

} /* dssnet_start() */

/*===========================================================================
FUNCTION DSSNET_STOP()

DESCRIPTION
   This function is registered with ps_iface to tear down the associated
   iface during call establishment.

PARAMETERS
  Pointer to ps_iface on which to operate
  Void ptr to client data

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int dssnet_stop
(
  ps_iface_type *this_iface_ptr,
  void* dssnet_instance
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint error handler*/
  DATA_MDM_ARG_NOT_CONST(this_iface_ptr);

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"Tear down hdlr for ps_iface 0x%x:%d",
                  this_iface_ptr->name,
                  this_iface_ptr->instance);

  /*-------------------------------------------------------------------------
    Need to tear down PPP layer.  When done, PPP will notify DS (dssnet) that
    it is done, at which point it will be safe to bring down the traffic
    channel.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if(ps_iface_addr_family_is_v6(this_iface_ptr))
  {
    if(-1 == dssnet6_sm_post_event((dssnet6_sm_cb_type *)dssnet_instance,
                                   DSSNET6_PPP_CLOSE_CMD_EV,
                                   NULL))
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Could not post DSSNET6_PPP_CLOSE_CMD");
      return -1;
    }
  }
  else
#endif /* FEATURE_DATA_PS_IPV6 */
  {
#ifdef FEATURE_DATA_IS707
    if( this_iface_ptr->name == CDMA_SN_IFACE )
    {
      (void)dssnet4_sm_post_event((dssnet4_sm_cb_type *)dssnet_instance,
                          DSSNET4_PPP_CLOSE_EV,NULL);
    }
#else
    {
      DATA_ERR_FATAL("IFACE passed to dssnet_stop is not CDMA");
    }
#endif /* FEATURE_DATA_IS707 */
  }

  return 0;
} /* dssnet_stop() */

/*===========================================================================
FUNCTION DSSNET_PPP_SETUP_NCP()

DESCRIPTION
   This function is used by the dssnet state machines to setup the
   corresponding NCPs (IPCP/IP6CP) during PPP establishemnt.

PARAMETERS
  Pointer to ppp config type which needs to populated
  Pointer to ps_iface on which to operate
  boolean to indicate whether this is a MIP call.
  boolean to indicate whether to negotiate DNS using PPP (IPCP).
  Pointer to user info.

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int dssnet_ppp_setup_ncp
(
  ppp_dev_opts_type                    *ppp_config_ptr,
  ps_iface_type                        *iface_ptr,
  boolean                              mip_call,
  boolean                              disable_ipcp_dns_opt,
  void                                 *user_info_ptr
)
{
#ifdef FEATURE_DATA_IS707
  ds707_data_session_profile_info_type *data_sess_profile_info = NULL;
  uint16 mtu;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -save -e641 */
  /*lint -save -e731  Suppress the informational warning*/
  if( (ppp_config_ptr == NULL) || 
      (PS_IFACE_IS_VALID(iface_ptr) == FALSE))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "dssnet_ppp_setup_ncp() bad args config_ptr 0x%p, iface 0x%p",
                    ppp_config_ptr, iface_ptr);
    ASSERT(0);
    return -1;
  }
  /*lint -restore*/

#ifdef FEATURE_DATA_IS707

  /*-------------------------------------------------------------------------
   type cast user info ptr to data sess profile type which has PPP param info
  -------------------------------------------------------------------------*/
  if (user_info_ptr != NULL)
  {
    data_sess_profile_info = 
      (ds707_data_session_profile_info_type*) user_info_ptr;
  }

  /*----------------------------------------------------------------------- 
    Call the function to read the MTU from NV. If the NV is inactive
    then the function will return the default value.
  -----------------------------------------------------------------------*/
  mtu = ds707_pkt_mgr_get_mtu_size_from_nv() ;
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "mtu value from ds707_pkt_mgr_get_mtu_size_from_nv : %d", 
                  mtu );

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,"Configuring :%d 1:PPP IPCP 0:PPP IPV6CP",
                  ps_iface_addr_family_is_v4(iface_ptr));

  if(ps_iface_addr_family_is_v4(iface_ptr))
  {
    /*-----------------------------------------------------------------------
      Set the iface pointers in both NCPs
    -----------------------------------------------------------------------*/
    ppp_config_ptr->ipcp_info.rx_iface_ptr   = iface_ptr;
    ppp_config_ptr->ipcp_info.mode           = PPP_CP_MODE_ACTIVE;
#ifdef FEATURE_DATA_PS_IPV6
    /* Limitation in PPP config block requires the logic to fill in the other
       iface ptr.
       NOTE:  refer to description of function dssneti_find_other_iface() */
    ppp_config_ptr->ipv6cp_info.rx_iface_ptr =
                              dssneti_find_other_iface ( iface_ptr, TRUE );

    /*-----------------------------------------------------------------------
      If the IPv6 interface is not valid then IPv6 is not enabled, so IPv6CP
      should be disabled (which will cause protocol rejects).
    -----------------------------------------------------------------------*/
    if(PS_IFACE_IS_VALID(ppp_config_ptr->ipv6cp_info.rx_iface_ptr))
    {
      ppp_config_ptr->ipv6cp_info.mode = PPP_CP_MODE_DISCARD;
    }

#endif /* FEATURE_DATA_PS_IPV6 */

    if(mip_call)
    {
      /*---------------------------------------------------------------------
        Setup IPCP to not negotiate for IP address.
      ---------------------------------------------------------------------*/
      ppp_config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
      ppp_config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
    }

    /*----------------------------------------------------------------------
      check if the profile id passed is anything other than DEFAULT. If it
      is then get the DNS option from the data session profile structure and
      assign it to disable_ipcp_dns_opt.
    ----------------------------------------------------------------------*/
    if (data_sess_profile_info != NULL) 
    {
        disable_ipcp_dns_opt =
          (data_sess_profile_info->negotiate_dns_server) ? FALSE : TRUE;

      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Data session profile id: %d", 
                      data_sess_profile_info->data_session_profile_id);
    }

    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"IPCP DNS option disabled:%d" ,
                    disable_ipcp_dns_opt);
    if(disable_ipcp_dns_opt == TRUE)
    {
      /*---------------------------------------------------------------------
        Disable primary DNS option.
      ---------------------------------------------------------------------*/
      ppp_config_ptr->ipcp_info.will_mask[FSM_REMOTE] &=
        ~IPCP_N_PRIMARY_DNS;
      ppp_config_ptr->ipcp_info.want_mask[FSM_REMOTE] &=
        ~IPCP_N_PRIMARY_DNS;
      ppp_config_ptr->ipcp_info.will_mask[FSM_LOCAL]  &=
        ~IPCP_N_PRIMARY_DNS;
      ppp_config_ptr->ipcp_info.want_mask[FSM_LOCAL]  &=
        ~IPCP_N_PRIMARY_DNS;

      /*---------------------------------------------------------------------
        Disable secondary DNS option.
      ---------------------------------------------------------------------*/
      ppp_config_ptr->ipcp_info.will_mask[FSM_REMOTE] &=
        ~IPCP_N_SECONDARY_DNS;
      ppp_config_ptr->ipcp_info.want_mask[FSM_REMOTE] &=
        ~IPCP_N_SECONDARY_DNS;
      ppp_config_ptr->ipcp_info.will_mask[FSM_LOCAL]  &=
        ~IPCP_N_SECONDARY_DNS;
      ppp_config_ptr->ipcp_info.want_mask[FSM_LOCAL]  &=
        ~IPCP_N_SECONDARY_DNS;
    }

    /*-----------------------------------------------------------------------
      Set PPP authentication settings from ps_iface auth_info
      (if populated - currently only RmNet does this - later, others should
       or profile should be referenced directly)
    -----------------------------------------------------------------------*/
    /*lint -save -e655*/
    if(iface_ptr->net_cfg_params.valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)
    {
      /* As of now, qmi allows user only 2 options:
         - No auth or
         - user has to accept pap or chap  */
      if(iface_ptr->net_cfg_params.auth_info.auth_type == 
           (uint8) PS_IFACE_AUTH_PREF_NONE)//NO AUTH
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_MED,"Authentication disabled");
        ppp_config_ptr->lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
        ppp_config_ptr->lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP;
        ppp_config_ptr->lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
        ppp_config_ptr->lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;
      }
      else
      {
        /*-------------------------------------------------------------------
          auth proto mask accepts one or more auth protocols,
          today only PAP/CHAP are defined. The onus is on the user to
          set the auth_type in the iface in the correct format for PPP
          to understand. TO DO - move auth_proto_mask enum outside PPP,
          ao that auth_type can be of the same type  
        -------------------------------------------------------------------*/
        ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask =
          (ppp_auth_proto_e_type) iface_ptr->net_cfg_params.auth_info.auth_type;
      }
    }

   /*---------------------------------------------------------------------
     Set the flags for username and password from client to FALSE 
     by default.
   ---------------------------------------------------------------------*/
    ppp_config_ptr->auth_info.is_userid_set_by_client = FALSE;
    ppp_config_ptr->auth_info.is_pwd_set_by_client = FALSE;
    /*-----------------------------------------------------------------------
       check if username, passwd are valid in ps_iface, if so use them
    -----------------------------------------------------------------------*/
    if (!dsgen_iface_hdlr_use_internal_auth())
    {
      if(  iface_ptr->net_cfg_params.valid_fields & 
           NET_CFG_PRM_AUTH_USERNAME_MASK)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Use auth credentials from PS iface");
        ppp_config_ptr->auth_info.user_id_len =
          iface_ptr->net_cfg_params.auth_info.params.user_id_len;
        memset(ppp_config_ptr->auth_info.user_id_info, 0, PPP_MAX_USER_ID_LEN);
        memscpy(ppp_config_ptr->auth_info.user_id_info,
                PPP_MAX_USER_ID_LEN,
                iface_ptr->net_cfg_params.auth_info.params.user_id_info,
                ppp_config_ptr->auth_info.user_id_len);
        ppp_config_ptr->auth_info.is_userid_set_by_client = TRUE;
      }

      if( iface_ptr->net_cfg_params.valid_fields & 
          NET_CFG_PRM_AUTH_PASSWORD_MASK)
      {
        ppp_config_ptr->auth_info.passwd_len =
          iface_ptr->net_cfg_params.auth_info.params.passwd_len;
        memset(ppp_config_ptr->auth_info.passwd_info, 0, PPP_MAX_PASSWD_LEN);
        memscpy(ppp_config_ptr->auth_info.passwd_info,PPP_MAX_PASSWD_LEN,
                iface_ptr->net_cfg_params.auth_info.params.passwd_info,
                ppp_config_ptr->auth_info.passwd_len);
        ppp_config_ptr->auth_info.is_pwd_set_by_client = TRUE;
      }
    }

    /*lint -restore 655*/
    if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
    {
      /*---------------------------------------------------------------------
        set the IPCP timeout and retry  count information from the profile 
        strcuture.
      ---------------------------------------------------------------------*/
      if (data_sess_profile_info != NULL)
      {
        if( ppp_set_device_constant(
            PPP_UM_SN_DEV, 
            IPCP_ACK_TIMEOUT,
            (uint32)(data_sess_profile_info->ipcp_ack_timeout)) 
            < 0 )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "IPCP Retry Timeout value could not be configured");
        }
        else
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"IPCP Retry Timeout value set to %d "
                          "for profile id: %d",
                          data_sess_profile_info->ipcp_ack_timeout,
                          data_sess_profile_info->data_session_profile_id);
        }
        if( ppp_set_device_constant(
            PPP_UM_SN_DEV, 
            IPCP_REQUEST_TRY,
            (uint32)data_sess_profile_info->ipcp_creq_retry_count) 
            < 0 )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "IPCP CReq Retry count value could "
                          "not be configured");
        }
        else
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                          "IPCP CReq Retry count value set to %d "
                          "for profile id: %d",
                          data_sess_profile_info->ipcp_creq_retry_count,
                          data_sess_profile_info->data_session_profile_id);
        }
      } /* data_sess_profile_info != NULL */
    }
    
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Setting v4 iface MTU: %d", mtu );
    PS_IFACE_SET_MTU( iface_ptr, mtu);
  }
#ifdef FEATURE_DATA_PS_IPV6
  else
  {
    ppp_config_ptr->ipv6cp_info.mode         = PPP_CP_MODE_ACTIVE;
    ppp_config_ptr->ipcp_info.mode           = PPP_CP_MODE_DISCARD;
    ppp_config_ptr->ipv6cp_info.rx_iface_ptr = iface_ptr;
    /* Limitation in PPP config block requires the logic to fill in the other
       iface ptr.
       NOTE:  refer to description of function dssneti_find_other_iface() */
    ppp_config_ptr->ipcp_info.rx_iface_ptr =
                              dssneti_find_other_iface ( iface_ptr, FALSE );

    if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
    {
      /*----------------------------------------------------------------------
        set the IPCP timeout and retry  count information from the profile 
        strcuture.
      ----------------------------------------------------------------------*/
      if (data_sess_profile_info != NULL) 
      {
        if( ppp_set_device_constant(
            PPP_UM_SN_DEV, 
            IPV6CP_ACK_TIMEOUT,
            (uint32)(data_sess_profile_info->ipcp_ack_timeout)) 
            < 0 )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "IPCP Retry Timeout value could not be configured");
        }
        else
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"IPCP Retry Timeout value set to %d "
                          "for profile id: %d",
                          data_sess_profile_info->ipcp_ack_timeout,
                          data_sess_profile_info->data_session_profile_id);
        }

        if( ppp_set_device_constant(
            PPP_UM_SN_DEV, 
            IPV6CP_REQUEST_TRY,
            (uint32)data_sess_profile_info->ipcp_creq_retry_count) 
            < 0 )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "IPCP CReq Retry count value could not be configured");
        }
        else
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"IPCP CReq Retry count value set to "
                          "%d for profile id: %d",
                          data_sess_profile_info->ipcp_creq_retry_count,
                          data_sess_profile_info->data_session_profile_id);
        }
      } /* data_sess_profile_info != NULL */
    }

    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Setting v6 iface MTU: %d", mtu );
    PS_IFACE_SET_MTU( iface_ptr, mtu);

  }
#endif /* FEATURE_DATA_PS_IPV6 */

#endif /* FEATURE_DATA_IS707 */
/* Lint -restore error 641 */
  return 0;

} /* dssnet_ppp_setup_ncp() */

#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION DSSNET_PPP_SETUP_LCP()

DESCRIPTION
   This function is used by the dssnet state machines to setup the 
   corresponding LCPs during PPP establishemnt.

PARAMETERS
  Pointer to data sess profile type which has PPP param info.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssnet_ppp_setup_lcp
(
  ppp_dev_opts_type                    *ppp_config_ptr,
  ds707_data_session_profile_info_type *data_sess_profile_info
)
{
  /*lint error handler*/
  DATA_MDM_ARG_NOT_CONST(data_sess_profile_info);

  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "dssnet_ppp_setup_lcp() called");

  if(ppp_config_ptr != NULL)
  {
    if (data_sess_profile_info != NULL)
    {
      /*------------------------------------------------------------------------
        If the data session is being configured for DEFAULT profile then dont
        set the auth_type. By default auth_proto_mask is set to PPP_AUTH_DEFAULT
        But if the data session is being configured for profile other than 
        DEFAULT then we will get the auth_type from the profile params strcut
        and assign it to ppp_config auth_type.
      ------------------------------------------------------------------------*/
      if (data_sess_profile_info->data_session_profile_id != 
          DATA_SESSION_DEFAULT_PROFILE
         )
      {

        if (data_sess_profile_info->auth_protocol == DATA_SESS_AUTH_PROTOCOL_PAP)
        {
          ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask = 
            PPP_AUTH_PAP;
          DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                          "Configuring PPP with PAP for profile id: %d",
                          data_sess_profile_info->data_session_profile_id);
        }
        else
        if (data_sess_profile_info->auth_protocol == 
                                                DATA_SESS_AUTH_PROTOCOL_CHAP)
        {
          ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask = 
            PPP_AUTH_CHAP;
          DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                          "Configuring PPP with CHAP for profile id: %d",
                          data_sess_profile_info->data_session_profile_id);
        }
        /*lint -save -e655 */
        else
        if (data_sess_profile_info->auth_protocol == 
                                           DATA_SESS_AUTH_PROTOCOL_PAP_CHAP)
        {
          ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask = 
            (ppp_auth_proto_e_type) (PPP_AUTH_PAP | PPP_AUTH_CHAP);
          DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                          "Configuring PPP with PAP or CHAP for profile id: %d",
                          data_sess_profile_info->data_session_profile_id);
        }
        /*lint -restore */
      } 
    }
  }/*if(ppp_config_ptr != NULL)*/
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "dssnet_ppp_setup_ncp() bad args config_ptr 0x%p",
                    ppp_config_ptr);
    ASSERT(0);
  }

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    if (data_sess_profile_info != NULL) 
    {
      /*---------------------------------------------------------------------
        set the LCP timeout and retry  count information from the profile 
        strcuture.
      ---------------------------------------------------------------------*/
      if( ppp_set_device_constant(
          PPP_UM_SN_DEV, 
          LCP_ACK_TIMEOUT,
          (uint32)(data_sess_profile_info->lcp_ack_timeout)) 
          < 0 )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "LCP Retry Timeout value could not be configured.");
      }
      else
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                        "LCP Retry Timeout value set to %d for profile id: %d",
                        data_sess_profile_info->lcp_ack_timeout,
                        data_sess_profile_info->data_session_profile_id);
      }

      if( ppp_set_device_constant(
          PPP_UM_SN_DEV, 
          LCP_REQUEST_TRY,
          (uint32)data_sess_profile_info->lcp_creq_retry_count) 
          < 0 )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "LCP CReq Retry count value could not be configured");
      }
      else
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, "LCP CReq Retry count value set to "
                        "%d for profile id: %d",
                        data_sess_profile_info->lcp_creq_retry_count,
                        data_sess_profile_info->data_session_profile_id);
      }
      /*---------------------------------------------------------------------
        set the Auth timeout and retry  count information from the profile 
        strcuture.
      ---------------------------------------------------------------------*/
      if( ppp_set_device_constant(
          PPP_UM_SN_DEV, 
          AUTH_TIMEOUT,
          (uint32)(data_sess_profile_info->auth_timeout)) 
          < 0 )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "Auth Retry Timeout value could not be configured");
      }
      else
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, "Auth Retry Timeout value set to %d "
                        "for profile id: %d",
                        data_sess_profile_info->auth_timeout,
                        data_sess_profile_info->data_session_profile_id);
      }

      if( ppp_set_device_constant(
          PPP_UM_SN_DEV, 
          AUTH_RETRY, 
          (uint32)data_sess_profile_info->auth_retry_count) 
          < 0 )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "Auth retry count value could not be configured");
      }
      else
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, "Auth CReq retry count value set to "
                        "%d for profile id: %d",
                        data_sess_profile_info->auth_retry_count,
                        data_sess_profile_info->data_session_profile_id);
      }

      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Setting use_same_auth to TRUE");
      /*-------------------------------------------------------------------
      Authentication needs to be the same after PPP resync (for KDDI)
      -------------------------------------------------------------------*/
      ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].use_same_auth = TRUE;

    }/*if (data_sess_profile_info != NULL) */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Invalid data sess profile info ptr passed to setup_lcp");
    }
  }
  /*Simplified JCDMA: 3.2.1 Use the same PPP Auth type in the 
    reconnection [D1-No.63]*/
  else if(ds707_roaming_is_simplified_jcdma_mode())
  {
    if (ppp_config_ptr != NULL)
    {
      /*-------------------------------------------------------------------
      Authentication needs to be the same after PPP resync (for KDDI)
      -------------------------------------------------------------------*/
      ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].use_same_auth = TRUE;
    }
  }

  return;
} /* dssnet_ppp_setup_lcp */
#endif /*FEATURE_DATA_IS707*/

#if defined(FEATURE_EPC_HANDOFF) && defined(FEATURE_EHRPD)
/*===========================================================================

FUNCTION dssnet_set_epc_handoff_ctxt
DESCRIPTION
  This function is called from MH ACLs to set IP context passed through
  the ACL policy. The transferred IP context will be stored in the MH.
 
PARAMETERS
  
  ps_iface_type* - if_ptr
  acl_policy_info_type* - acl_policy_info_ptr
  
DEPENDENCIES
  None

RETURN VALUE
  0  - If successful tranfer of IP context
  -1 - If error in transferring IP context

SIDE EFFECTS
  None

===========================================================================*/

int dssnet_set_epc_handoff_ctxt
(
  ps_iface_type* if_ptr,
  acl_policy_info_type* acl_policy_info_ptr
)
{
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG3(MSG_LEGACY_MED, "ds3gmgr_set_epc_handoff_ctxt: iface_ptr "
                  "0x%x:%d, policy_ptr 0x%x",
                  if_ptr->name,if_ptr->instance,acl_policy_info_ptr);

  if (!PS_IFACE_IS_VALID(if_ptr))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3gmgr_set_epc_handoff_ctxt: Invalid IFACE Ptr");
    return -1;
  }
  if (acl_policy_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3gmgr_set_epc_handoff_ctxt Invalid policy Ptr");
    return -1;
  }
  /*-------------------------------------------------------------------------
    PDN HO flag | Incoming Policy HO flag | Action
          0     |           0             | Only check if handoff context
                |                         | memory is freed - if still
                |                         | allocated free the memory.
   -----------------------------------------------------------------------
         0      |           1             | Allocate memory or reuse if
                |                         | already allocated, and copy over
                |                         | the handoff context.
   -----------------------------------------------------------------------
        1       |           0             | No-op.
   -----------------------------------------------------------------------
        1       |           1             | Allocate memory or reuse if
                |                         | already allocated, and copy over
                |                         | the handoff context.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
     Allocate blob of memory to store the IP context info.
     Allocate a new blob only if already not allocated. This will cover the
     condition where a memory blob was allocated but not de-allocated in
     the previous run.
     IF returning error after this point, make sure to free the memory
     pointed by temp_mem_ptr
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
   Get the PDN CNTX CB for this IFACE.
  -------------------------------------------------------------------------*/
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(if_ptr);
  if (pdncntx_cb_ptr == NULL) 
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Null pdncntx_cb found");
    return -1;
  }
  /*------------------------------------------------------------------------- 
    If the policy specifies handoff class, always store the handoff context
    either allocate memory for the handover config handle, or if already
    allocated reuse the memory block
  -------------------------------------------------------------------------*/
  if (ds707_is_policy_handoff_class_epc(acl_policy_info_ptr))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                    "Handoff context is of type EPC. Store handoff context. "
                    "PDN context handover attach flag: %d",
                    pdncntx_cb_ptr->handover_attach);

    if (pdncntx_cb_ptr->handover_ip_config_handle == NULL) 
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                      "Allocating dynamic memory to store EPC handoff ctxt");
      pdncntx_cb_ptr->handover_ip_config_handle = 
        modem_mem_alloc(sizeof(acl_policy_info_ptr->handoff_ctxt),
                                       MODEM_MEM_CLIENT_DATA_CRIT);
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Dynamic memory already allocated, reuse");
    }
    if (pdncntx_cb_ptr->handover_ip_config_handle != NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Transferring EPC handoff ctxt");
      (void)memscpy(pdncntx_cb_ptr->handover_ip_config_handle,
                    sizeof(acl_policy_info_ptr->handoff_ctxt),
                    &acl_policy_info_ptr->handoff_ctxt,
                    sizeof(acl_policy_info_ptr->handoff_ctxt));
      pdncntx_cb_ptr->handover_attach = TRUE;
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Could not transfer EPC handoff ctxt. "
                      "Failed memory allocation");
      return -1;
    }
  }
  else
  {
    /*------------------------------------------------------------------------- 
      If the policy does not specify handoff, check the handover attach
      flag in the PDN that this iface is associated with. If the handover
      attach flag is TRUE, leave the handoff context as is even if policy
      specifies initial attach.
      Scenario: dual IP PDN is handed over (eg: LTE to eHRPD), with
      only one Iface in use, before PPP starts on eHRPD, another app requests
      connection with second iface, it will request with policy as initial
      attach, but the PDN must come up with handover attach. Hence in the
      routing for the second iface, do not clean the handover context.
    -------------------------------------------------------------------------*/
    if( pdncntx_cb_ptr->handover_attach == TRUE )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, 
                      "Handoff context is not of type EPC, but PDN's handover"
                      "attach flag is TRUE - Retain the handover context in PDN");
    }
    else
    {
      /*----------------------------------------------------------------------
        If handoff context is not of EPC type, and PDN's handover attach flag
        is FALSE, but still memory is allocated, free that memory.
      -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Handoff context is not of type EPC."
                              "PDN context handover attach flag is FALSE");
    if (pdncntx_cb_ptr->handover_ip_config_handle != NULL) 
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                      "Dynamic memory already allocated (not freed up after"
                      " the last call) Freeing up dynamic memory allocated "
                      "for IP context");

      modem_mem_free( pdncntx_cb_ptr->handover_ip_config_handle,
                      MODEM_MEM_CLIENT_DATA_CRIT);
      pdncntx_cb_ptr->handover_ip_config_handle = NULL;
    }
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Setting the hadover attach flag to FALSE");
    pdncntx_cb_ptr->handover_attach = FALSE;
  }
  }
  return 0;
}
#endif
#ifdef FEATURE_DATA_IS707
/*===========================================================================

FUNCTION dssnet_call_allowed
DESCRIPTION
  This function is called from MH ACLs and phys link up processing to
  determine if the packet call is allowed with the current configuration.
  This checks if the profile can be used with the current configuration.
  It also checks for the handed off call.
 
PARAMETERS
  
  
  
DEPENDENCIES
  None

RETURN VALUE
  0  - If successful tranfer of IP context
  -1 - If error in transferring IP context

SIDE EFFECTS
  None

===========================================================================*/
boolean dssnet_call_allowed
(
  ds707_data_session_profile_info_type *prof_ptr,
  boolean                       handover_attach,
  ps_iface_net_down_reason_type *call_end_reason
)

{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity check - should not ever happen
  -------------------------------------------------------------------------*/
  if (prof_ptr == NULL)
  {
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Check if the profile is allowed to go on the current configuration  
  -------------------------------------------------------------------------*/
  ret_val = ds707_data_session_call_allowed(prof_ptr, call_end_reason);
  if (ret_val == FALSE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Profile %d not allowed in current config",
                    prof_ptr->data_session_profile_id);
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Profile %d allowed in current config",
                    prof_ptr->data_session_profile_id);
  }
#ifdef FEATURE_EPC_HANDOFF
  if ((ret_val == TRUE) && (handover_attach == TRUE))
  {
    if (ds707_pkt_is_ehrpd_mode_of_operation() == TRUE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "EPC handover and current mode is eHRPD, continue call");
      ret_val = TRUE;
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "EPC handover but current mode is not eHRPD, "
                      "tear down call");
      *call_end_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
      ret_val = FALSE;
    }
  }
#endif
  return ret_val;
}

/*===========================================================================
FUNCTION DSGEN_IFACE_HDLR_USE_INTERNAL_AUTH()

DESCRIPTION
  Return value 1 means credentials from NV should be used, and 0 means
  credentials are passed in from client. The value of is_internal_auth is set
  on power up by reading value set in nv 

DEPENDENCIES
  None

RETURN VALUE
  1 internal auth should be used
  0 use auth from client

SIDE EFFECTS
  None
===========================================================================*/
boolean dsgen_iface_hdlr_use_internal_auth( void )
{
  return ds3g_get_auth_internal_nv();
}

/*===========================================================================
FUNCTION DSGEN_IFACE_HDLR_INIT()

DESCRIPTION
  This function is called in ds707_pkt_mgr_init to initialize dsgen_iface_hdlr

DEPENDENCIES
  None

RETURN VALUE
  1 internal auth should be used
  0 use auth from client

SIDE EFFECTS
  None
===========================================================================*/
void dsgen_iface_hdlr_init
(
  void
)
{
  return;
}

#ifdef FEATURE_NIQ_EHRPD
/*===========================================================================
FUNCTION DSGEN_IFACE_HANDLE_NIQ_SBCM

DESCRIPTION
  This handles getting S-BCM value from PPP & providing it to
  nw_inint_qos_mgr. This is applicable for n/w init QOS scenario

DEPENDENCIES  None

RETURN VALUE  None
===========================================================================*/
void dsgen_iface_handle_niq_sbcm 
(
   ps_iface_type   *ps_iface_ptr
)
{
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
  ppp_ncp_handle_type ncp_handle;
  uint8 selected_bcm = (uint8)DS707_NW_INIT_QOS_BCM_MS_ONLY;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                                                ps_iface_ptr);
  ncp_handle = ds707_pdncntx_get_ncp_handle(pdncntx_cb_ptr);

  if (0 != ppp_ncp_get_negotiated_param (ncp_handle, 
                                         PPP_NCP_VSNCP_3GPP2_NEGOTIATED_BCM, 
                                         &selected_bcm, 
                                         sizeof(selected_bcm)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to get S-BCM value");
    /*-------------------------------------------------------------------------
     Reset BCM value to the default value: DS707_NW_INIT_QOS_BCM_MS_ONLY.
    -------------------------------------------------------------------------*/
    selected_bcm = (uint8)DS707_NW_INIT_QOS_BCM_MS_ONLY;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, "S-BCM value = %d", selected_bcm);
  ds707_nw_init_qos_mgr_vsncp_success_ind (ps_iface_ptr, selected_bcm);
  return;
} /* dsgen_iface_handle_niq_sbcm() */
#endif /* FEATURE_NIQ_EHRPD */

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION DSGEN_IFACE_HANDLE_VSNCP_DEFAULT_APN_IND

DESCRIPTION
  This function checks if negotiated PDN connection is on default APN from
  VSNCP request and ack message sent from NW. If so, store the default APN
  for future default APN requests

DEPENDENCIES  None

RETURN VALUE  None
===========================================================================*/
void dsgen_iface_handle_vsncp_default_apn_ind
(
   ps_iface_type   *ps_iface_ptr
)
{
  ds707_pdn_context_cb_type   *pdncntx_cb_ptr = NULL;
  ppp_ncp_handle_type          ncp_handle;
  ppp_ncp_negotiated_apn_type  negotiated_apn;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                                                ps_iface_ptr);
  ncp_handle = ds707_pdncntx_get_ncp_handle(pdncntx_cb_ptr);

  if (0 != ppp_ncp_get_negotiated_param (ncp_handle, 
                                         PPP_NCP_VSNCP_3GPP2_NEGOTIATED_APN, 
                                         &negotiated_apn, 
                                         sizeof(negotiated_apn)))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "Failed to get negotiated apn from vsncp" );
    return;
  }

  if ( (negotiated_apn.apn_is_default == TRUE) &&
       (FALSE == ds707_pdncntx_is_default_pdn(pdncntx_cb_ptr)) )
  {
    ds707_pdncntx_set_default_pdn_and_apn( pdncntx_cb_ptr,
                                           (char*)(&negotiated_apn.apn_name),
                                           negotiated_apn.apn_len );
  }
  else if( ( negotiated_apn.apn_is_default == FALSE ) &&
           (TRUE == ds707_pdncntx_is_default_pdn(pdncntx_cb_ptr)) )
  {
    /*--------------------------------------------------------------------
      In case of PPP resync we get back a different default APN than
      previously negotiated, clear the default PDN
     --------------------------------------------------------------------*/
    ds707_pdncntx_reset_default_pdn();
  }

  return;
} /* dsgen_iface_handle_vsncp_default_apn_ind */
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_DATA_IS707 */
