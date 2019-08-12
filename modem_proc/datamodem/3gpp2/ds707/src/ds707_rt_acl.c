/*===========================================================================

   C D M A   2 0 0 0   ( 1 x )   S P E C I F I C   M O D E   H A N D L E R

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
       F O R   R O U T I N G   V I A   T H E   1 x   I N T E R F  A C E

GENERAL DESCRIPTION
  The 1x specific routing ACL is used to enable policy and address
  based routing across the 1x interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_rt_acl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
                      
                             EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/15    sc     Fix to disallow routing on v6 calls when 
                   MIPv6 is enabled 
02/16/15    sc     Pick up non null APN profile for ALWAYS_SHARE_PROFILE
12/09/14    hr     To support ALWAYS_SHARE_PROFILE
11/18/14    skc    Removing FEATURE_UW_FMC
11/04/14    sd     Added new call end reason for subs_id mismatch.
10/29/14    sd     Clear ds707_omh_profile_id in route ACL while in eHRPD mode.
10/14/14    vm     Fix to not use OMH default profile while data call
                   is on eHRPD.
08/05/14    ms     DSDA Phase 2 changes.
07/28/14    hr     Sim swap feature
07/23/14    sd     Fix to use credentials from OMH profile in case of card mode
                   during MIP to SIP fallback.
06/26/14    ms     Data+MMS - DSDA changes Phase 1.
05/22/14    sd     Fix to bypass OMH and LBS related validations if we are in 
                   eHRPD mode, or if both the existing and requesting profile
                   types are not OMH.
04/23/14    sc     Fixing the code to ensure we don't deny the iface with
                   wrong reason code when NVRUIM profiles are used 
06/03/13    sd     ATCoP decoupling changes
07/11/12    sb     Fixed KW Errors
12/02/11    vpk    Deny iface for DUN calls in eHRPD mode.
11/07/11    msh    Fix for iface lookup in HRPD mode 
09/26/11    msh    Fixed Compiler warnings 
08/23/11    vpk    Return proper error code when um iface is in use
08/11/11    var    Resolve Compiler warnings & KW errors.
07/12/11    ls     Set Passive to active flag for socket calls and DUN calls
                   if it is not DUAL IP calls
07/22/11    dvk    Global Variable Cleanup
07/15/11   ms      In RUIM mode, Mobile IP calls should pickup default profile 0 
                   as multiple MIP profile support is not supported. 
07/06/11    dvk    Merged fix to fill the correct profile id in policy info. 
06/24/11    scb    Added support for EFS NV item is_fmc_enabled.
04/21/11    op     Added changes to ensure that HRPD IPv6 calls went through
                   for builds with iWLAN or FMC features turned on.
03/15/11    mg     Global variable cleanup
03/04/11    ls     Fixed CW
01/06/11    scb    Changes for UW FMC Feature
10/15/10    ls     Remove GOING_DOWN check in ds707_rt_acl_use_selected_iface()
09/03/10    ms     Legacy or Non-OMH apps should always pickup default profile
                   instead of first profile.
08/24/10    as     TE/Embedded Call arbitration changes
08/22/10    jy     during handoff, check handoff flag besides checking ehrpd mode
06/17/10    as     Fusion related additions
05/04/10    mg     Dual IP feature
04/12/10    ls     In ds707_rt_is_cfg_shareable(), it shall return TRUE if
                   it is already looked up
03/08/10    ls     If iface is in going_down state, ACL shall reject to 
                   select this iface
02/24/10    ps     Added code changes to by-pass profile compatibility check
                   if AT is in MIP mode and the requesting profileID is NVRUIM.
12/10/09    ps     Mainlining OMH feature based on new profile management
                   design.
07/27/09   spn     Block calls if the profile is not supported on current 
                   system.  
06/02/09    ss     Fixed issue with look up failing for Relay model calls.
06/01/09    spn    Allow apps which set policy with routeable flag ( system
                   apps ) to pass through.  
03/30/09    ss     Fixed issue with look up failing for Relay model calls.
02/09/09    spn    Moved RMSM PDN instance to the RMSM Module
01/22/09   ms      Fixed compilation error.
12/12/08   sn      Fixed off-target lint errors.
11/24/08   psng    Added support to use legacy NV items if number of valid SIP
                   profiles is zero.
11/05/08   psng    Made Changes to use legacy credentials for MIP and MIP to 
                   SIP fallback when OMH is enabled.
09/26/08   psng    Replaced the lookup_only check with bring_up check to 
                   perform Priority/Compatibility check only for bring up cases.
08/20/08   psng    Handled lookup only and routeable cases properly for OMH.
08/20/08   psng    Added profile id look up support for Laptop call for OMH.
06/27/08   rt      Added support for IWLAN_3GPP2 and 1x-v4 interface mutual
                   exclusion.
06/17/08   psng    Added support for multiple app profiles functionality for
                   OMH.
01/04/08   sy/gr   Allow sockets calls while in TE2 network model call.
07/02/07   sc      Added support for Application Profiles and Dormant2 (Iface
                   Lingering) features.
06/13/07   ak/az   Removed all the processing from the ACL_POST_PROC_DEF() function
                   as it is handled by the PS team
08/23/04   mvl     Fixed featurization on the post processing function.
08/18/04   kvd     Added code for blocking the other iface in
                   rt_acl_post_proc().
08/09/04   mct     Added logical iface ACL.
07/30/04   mct     1x ACL must now check and match both ip_pkt and policy
                   families.
03/16/04   mct     Optimized some routing code.
03/15/04   ak      Updated to 3/15/04 IPV6 Build.
02/27/04   mct     Added new checks for v6 update.
01/31/04   usb     Support for lookup_only policy flag
10/14/03   usb     Removed cfg_shareable rule for now to be able to select 
                   other modes when CDMA is not pref mode.
07/21/03   usb     Support for app specified policy information.
05/13/03   usb     Added new rules to handle multiple enabled ifaces 
04/11/03   ak      Updated for new 707 RMSM interfaces.
11/19/02   usb     Added a DENY rule to the ACL used by IS707 pkt iface,this
                   rule denies access to the iface if its already in use
08/30/02   om      Created Module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#ifdef FEATURE_DATA_IS707

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ds707_rt_acl.h"
#include "ps_iface.h"
#include "ps_sys.h"
#include "ps_sys_ioctl.h"
#include "ds707_rmsm.h"
#include "ds3gmgr.h"
#include "ds707_pkt_mgri.h"
#include "ps_ip_addr.h"
#include "ds707_pkt_mgr_iface.h"
#include "ds707_pkt_mgr_hdlr.h"
#include "ds707_nv_util.h"
#include "ds707_pdn_context.h"
#include "ds707_sys_chg_hdlr.h"
#include "ds707_iface4_hdlr.h"
#include "dsgen_iface6_hdlr.h"
#include "ds707_tm_mgr.h"
#ifdef FEATURE_EPC_HANDOFF
#include "dsgen_iface_hdlr.h"
#endif /* FEATURE_EPC_HANDOFF */
#include "ds_1x_profile.h"
#include "dsat707mip.h"
#include "ds707_event_defs.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ds707_data_session_profile.h"

#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
#include "ds707_extif.h"
#include "iwlan_iface_ext.h"
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

#include "data_err_suppr.h"
#include "ps_acli_rules.h"

#include "ds707_pkt_mgr.h"
#include "ds3gutil.h"

/* <EJECT> */
/*===========================================================================

                        FORWARD FUNCTION DECLARATIONS

===========================================================================*/

int ds707_omh_profile_id = DATA_SESSION_PROFILE_ID_INVALID;

static boolean ds707_is_pref_pkt_mode
( 
  acl_policy_info_type *policy_info_ptr,
  sys_sys_mode_e_type mode 
);

static boolean ds707_rt_is_call_allowed
(
  ds707_data_session_profile_id  pending_id,
  ps_iface_type                  *if_ptr,
  acl_policy_info_type           *policy_info_ptr,
  int                            *deny_reason
);

/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

/***  IS707 ACL ***/
/*lint -save -e{57,58,506,641,655,774}*/
ACL_DEF( ds707_rt_acl )
ACL_START
  PASS_QUICK( ACL_DEFAULT_CLASS, IS_POL_INFO_NULL() );
  DENY( DIFFERENT_IFACE_NAME_REQUIRED() );
  DENY( DIFFERENT_IFACE_ID_REQUIRED() );
  DENY_WITH_ANY_REASON( IFACE_NOT_AVAILABLE2(ds707_rt_is_pref_sys));
  DENY_WITH_ANY_REASON( SPECIFIED_CONFIG_NOT_SUPPORTED2
                        (ds707_rt_is_cfg_supported_and_iface_available) );
  DENY( IFACE_UP_REQUIRED_BUT_NOT_UP() );
  PASS( ACL_IFNAME_CLASS, REQUIRED_IFACE_TYPE_IS( CDMA_SN_IFACE ) );
  PASS( ACL_PREF_MODE_CLASS, 
        IFACE_IS_PREF_MODE_DEF2( ds707_is_pref_pkt_mode, SYS_SYS_MODE_CDMA) );
  DENY_WITH_ANY_REASON( IFACE_NOT_AVAILABLE2( ds707_rt_acl_use_selected_iface ));
  PASS( ACL_CONFIG_CLASS, 
        SPECIFIED_CONFIG_IS_SHAREABLE(ds707_rt_is_cfg_shareable) );   
  PASS( ACL_UP_PREF_CLASS, IFACE_UP_PREFERRED_AND_UP() );
  PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_TYPE_IS(IFACE_EPC_GROUP) );
  PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_TYPE_IS( WWAN_GROUP ) );
  PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_TYPE_IS( ANY_DEFAULT_GROUP ) );
  PASS( ACL_DEFAULT_CLASS, ALL );
ACL_END
/*lint -restore*/

/*===========================================================================
FUNCTION DS707_RT_ACL_POST_PROC

DESCRIPTION
   This function performs the post routing lookup actions.  This funtion is 
   called for the interface which is picked up as the most preferred interface 
   during lookup based upon client requirements (policy) and other system 
   parameters.  
   For 1X there is nothing to do right now.
  
PARAMETERS  
  *ipfilter_info_ptr  - Ip filter information
  *policy_info_ptr    - Policy information
  *this_if_ptr        - Ptr to the interface
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_POST_PROC_DEF( ds707_rt_acl_post_proc )
{
  ps_iface_type                 *if_ptr; 
  /* profile id of the new call */
  ds707_data_session_profile_id pending_id;
  /* iface tear down cmd err no */
  int16                         ps_iface_errno;
  /* iface tear down cmd ret val */
  int                           ret_val = 0;
  ds707_event_payload_type      ds707_event_payload;

  /* pointer to pdn context control block */
  ds707_pdn_context_cb_type    *ds707_pdn_cntx_cb_ptr = NULL;

  /* pointer to iface table entry, 2 ifaces for dual ip */
  ds707_iface_table_type       *ds707_iface_tbl_ptr = NULL;
  ip_addr_enum_type             if_ptr_addr_family = IFACE_INVALID_ADDR_FAMILY;
#ifdef FEATURE_EPC_HANDOFF  
  dssnet4_sm_cb_type        * dssnet4_instance_ptr;
#ifdef FEATURE_DATA_PS_IPV6
  dssnet6_sm_cb_type        * dssnet6_instance_ptr;
#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* FEATURE_EPC_HANDOFF */
  
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
  ps_iface_type                *other_iface_ptr;
  ds707_iface_table_type       *ds707_pdn_other_iface_tbl_ptr = NULL;
  ip_addr_enum_type             other_iface_addr_family = IFACE_INVALID_ADDR_FAMILY;
#endif
#endif
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr = NULL;
  ds707_data_session_profile_info_type *current_prof_ptr = NULL;
  ds707_data_session_profile_info_type *requesting_prof_ptr = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if_ptr = (ps_iface_type *) this_if_ptr;
  DATA_IS707_MSG2( MSG_LEGACY_MED, "Iface 0x%x::%d post proc", 
                   if_ptr->name, if_ptr->instance );

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(pkt_info_ptr);
  /*-------------------------------------------------------------------------
     If an UP interface is selected by routing, this function is anyway
     called even if its already in use
  -------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
    If the routing is for lookup only don't block the iface, this allows
    other clients to use the iface if they intend to do so. Else mark the
    iface state appropriately.
  -----------------------------------------------------------------------*/
  if( (!(policy_info_ptr->lookup_only)) )
  {
    pending_id = (ds707_data_session_profile_id)
                     policy_info_ptr->data_session_profile_id;
    if( ds707_data_session_is_nvruim_profiles_available() )
    { 
      if( ds707_omh_profile_id != DATA_SESSION_PROFILE_ID_INVALID &&
          pending_id == DATA_SESSION_DEFAULT_PROFILE )
      {
        pending_id = ds707_omh_profile_id;
        ds707_omh_profile_id = DATA_SESSION_PROFILE_ID_INVALID;
        DATA_IS707_MSG1( MSG_LEGACY_MED, "Set pendingid = %d", 
                         pending_id );        
      }
    }
    DATA_IS707_MSG1( MSG_LEGACY_MED, "Pending profile id %d", 
                     pending_id );
    if ( !PS_IFACE_IS_IN_USE ( if_ptr ) )
    {
      /*-------------------------------------------------------------------
        Get the iface_table_entry for this iface. If the iface_tbl entry is
        not associated with a PDN, allocate a PDN control block.
      -------------------------------------------------------------------*/
      ds707_iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(if_ptr);
      ds707_pdn_cntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr(ds707_iface_tbl_ptr);
      if( ds707_pdn_cntx_cb_ptr == NULL )
      {
        if (pending_id == DATA_SESSION_ALWAYS_SHARE_PROFILE)
        {
          /*-----------------------------------------------------------------
            Ideally we should not hit this condition because, if
            ALWAYS_SHARE_PROFILE is the first call, the pending profile ID
            should have been resolved or rejected in routing ACLs
          -----------------------------------------------------------------*/
          pending_id = 
            ds707_data_session_get_profile_for_always_share_profile(policy_info_ptr);

          /*-----------------------------------------------------------------
            No profile can be used for ALWAYS_SHARE_PROFILE in post_proc.
            Set to default profile here as recovery plan.
          -----------------------------------------------------------------*/
          if (pending_id == DATA_SESSION_PROFILE_ID_INVALID)
          {
            pending_id = DATA_SESSION_DEFAULT_PROFILE;
          }

          policy_info_ptr->data_session_profile_id = pending_id;          

          DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                           "ALWAYS_SHARE_PROFILE for first call bringup, "
                           "overwrite with profile %d",
                           pending_id );
        }

        ds707_pdn_cntx_cb_ptr = ds707_pdncntx_allocate_pdncntx_cb();
        
        if( (!DS707_PDNCNTX_CB_IS_VALID( ds707_pdn_cntx_cb_ptr )) ||
            (!DS707_IFACE_TBL_IS_VALID( ds707_iface_tbl_ptr )) )
        {
          /*-------------------------------------------------------------------
            We should not hit this condition. (Use case: iface available, but
            cannot allocate pdn control block) We cannot have the check of
            having sufficient resources before its known that the selected
            iface is not in use.
            Either change the max pdn and max iface numbers so that we dont
            hit this condition, or gracefully return from here.
            For now ASSERT(0)
          -------------------------------------------------------------------*/
          DATA_ERR_FATAL("pdn cb NULL or iface tbl ptr NULL" );
        }
  
        /*----------------------------------------------------------------------
         Set the address family of the iface to the one specified in the policy.
         Set the PDN control block's v4 / v6 iface table entry to this iface
         depending the iface addr family.
         Remember the address family for the other iface if the pdn is dual ip
         Set this iface's PDN control block pointer.
        ----------------------------------------------------------------------*/ 
        if_ptr_addr_family = 
          ds707_data_session_resolve_iface_addr_family ( pending_id, 
                                                         policy_info_ptr );
  
        if( (if_ptr_addr_family == IFACE_IPV4_ADDR_FAMILY) 
#ifdef FEATURE_DATA_PS_IPV6
            || (if_ptr_addr_family == IFACE_IPV6_ADDR_FAMILY) 
#endif /* FEATURE_DATA_PS_IPV6 */
          )
        {
          DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                           "Setting iface 0x%x::%d addr family to %d", 
                           if_ptr->name, if_ptr->instance, if_ptr_addr_family );
          if(ps_iface_set_addr_family(if_ptr, if_ptr_addr_family) == -1)
          {
            DATA_ERR_FATAL("could not set iface address family");
          }
  
          if( if_ptr_addr_family == IFACE_IPV4_ADDR_FAMILY )
          {
            ds707_pdncntx_set_v4_iface( ds707_pdn_cntx_cb_ptr,
                                        ds707_iface_tbl_ptr );
          }
#ifdef FEATURE_DATA_PS_IPV6
          else
          {
            ds707_pdncntx_set_v6_iface( ds707_pdn_cntx_cb_ptr, 
                                        ds707_iface_tbl_ptr );
          }
#endif /* FEATURE_DATA_PS_IPV6 */  
          ds707_pdncntx_set_pdn_cb_ptr( ds707_iface_tbl_ptr, 
                                        ds707_pdn_cntx_cb_ptr );

          /*-------------------------------------------------------------------
            Set the eHRPD call as TRUE for the dssnet SM. This will be checked
            during routing to decide whether to run HRPD checks or eHRPD
            checks. If it is TRUE then perform eHRPD checks else HRPD checks.
            This will solve the problem where handover is triggered, session
            is not up yet and another call is tried on eHRPD (initial request).
            At this point since first call is already in handover we should
            assume ehrpd and perform those checks. 
          -------------------------------------------------------------------*/
#ifdef FEATURE_EPC_HANDOFF
          if ( ds707_is_policy_handoff_class_epc(policy_info_ptr) )
          {
            DATA_IS707_MSG0( MSG_LEGACY_MED, 
                             " Handover from LTE, Setting is_ehrpd_call to TRUE " );
            if( if_ptr_addr_family == IFACE_IPV4_ADDR_FAMILY )
            {
              dssnet4_instance_ptr = 
                ds707_pdncntx_get_dssnet4_sm_ptr(ds707_iface_tbl_ptr);
              if(dssnet4_instance_ptr)
              {
                dssnet4_instance_ptr->is_ehrpd_call = TRUE;
              }

            }
#ifdef FEATURE_DATA_PS_IPV6
            else
            {
              dssnet6_instance_ptr = 
                ds707_pdncntx_get_dssnet6_sm_ptr(ds707_iface_tbl_ptr);
              if(dssnet6_instance_ptr)
              {
                dssnet6_instance_ptr->is_ehrpd_call = TRUE;
              }

            }
#endif /* FEATURE_DATA_PS_IPV6 */ 
            
          }
#endif
        }
        else 
        {
          DATA_ERR_FATAL("could not resolve iface addr family");
        }
                
        /*---------------------------------------------------------------------
          If profile type is ehrpd and pdn_type is v4v6, reserve another iface
          for this same PDN.
          Set the iface address family to the other ip type and associate it
          with this PDN.
        ---------------------------------------------------------------------*/ 
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
#ifdef FEATURE_DATA_PS_IPV6
        if ( TRUE == ds707_data_session_get_profile( 
                       pending_id, 
                       (ds_profile_subs_id_enum_type)(policy_info_ptr->subs_id),
                       &data_sess_profile_info_ptr ) )
        {
          /*--------------------------------------------------------------------
            If session is not opened, continue allocating the second iface in
            case of dual IP scenario (if profile type is ehrpd and pdn type V4_V6
            If session gets negotiated on hrpd system, the second iface will be
            freed. (Added a check for this when phys link comes up)
             Do not allocate V6 iface in eHRPD, if IPV6 is disabled.
          ---------------------------------------------------------------------*/
          /*lint -save -e655*/
          if( (data_sess_profile_info_ptr->profile_type & PROFILE_EHRPD) 
              &&
              (data_sess_profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6)
              &&
              TRUE == ds3g_is_ipv6_enabled())
          /*lint -restore*/
          {
            DATA_IS707_MSG1( MSG_LEGACY_MED, 
                             "profile type: eHRPD, pdn_type: %d", 
                             data_sess_profile_info_ptr->pdn_type );
    
            ds707_pdn_other_iface_tbl_ptr = ds707_pdncntx_allocate_iface_tbl_entry();
            if( !DS707_IFACE_TBL_IS_VALID( ds707_pdn_other_iface_tbl_ptr ) )
            {
              DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                               "Second iface cannot be allocated for dual IP call" ); 
            }
            else
            {
              other_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl(
                                    ds707_pdn_other_iface_tbl_ptr );
              if( !other_iface_ptr )
              {
                DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Pointer to iface is NULL" );
              }
  
              if( if_ptr_addr_family == IFACE_IPV4_ADDR_FAMILY )
              {  
                other_iface_addr_family = IFACE_IPV6_ADDR_FAMILY;
              }
              else 
              {
                other_iface_addr_family = IFACE_IPV4_ADDR_FAMILY;
              }
                
              DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                               "Setting iface 0x%x::%d addr family to %d", 
                               other_iface_ptr->name, 
                               other_iface_ptr->instance, 
                               other_iface_addr_family );
  
              if( ps_iface_set_addr_family(other_iface_ptr, other_iface_addr_family) 
                  == -1)
              {
                DATA_ERR_FATAL("could not set other iface address family");
              }
    
              if( other_iface_addr_family == IFACE_IPV4_ADDR_FAMILY )
              {
                ds707_pdncntx_set_v4_iface( ds707_pdn_cntx_cb_ptr, 
                                            ds707_pdn_other_iface_tbl_ptr );
              }
              else
              {
                ds707_pdncntx_set_v6_iface( ds707_pdn_cntx_cb_ptr, 
                                            ds707_pdn_other_iface_tbl_ptr );
              }
              ds707_pdncntx_set_pdn_cb_ptr( ds707_pdn_other_iface_tbl_ptr, 
                                            ds707_pdn_cntx_cb_ptr );
  
#ifdef FEATURE_EPC_HANDOFF
              if ( policy_info_ptr->handoff_ctxt.handoff_class == 
                                                      PS_IFACE_HANDOFF_CLASS_EPC )
              {
                DATA_IS707_MSG0( MSG_LEGACY_MED, 
                                 " Handover from LTE, Setting is_ehrpd_call to TRUE " );
                if( other_iface_addr_family == IFACE_IPV4_ADDR_FAMILY )
                {
                  dssnet4_instance_ptr = 
                    ds707_pdncntx_get_dssnet4_sm_ptr(ds707_pdn_other_iface_tbl_ptr);
                  if ( dssnet4_instance_ptr == NULL )
                  {
                     DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                                      "dssnet4_instance_ptr is NULL" );
                  }
                  else
                  {
                     dssnet4_instance_ptr->is_ehrpd_call = TRUE;
                  }
                }
#ifdef FEATURE_DATA_PS_IPV6
                else
                {
                  dssnet6_instance_ptr = 
                    ds707_pdncntx_get_dssnet6_sm_ptr(ds707_pdn_other_iface_tbl_ptr);
                  if ( dssnet6_instance_ptr == NULL )
                  {
                      DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                                       "dssnet6_instance_ptr is NULL" );
                  }
                  else
                  {                
                      dssnet6_instance_ptr->is_ehrpd_call = TRUE;
                  }
                }
#endif /* FEATURE_DATA_PS_IPV6 */ 
              }
#endif
              /*-----------------------------------------------------------------
                Set both the ifaces as active
              -----------------------------------------------------------------*/
              PS_IFACE_SET_IS_ACTIVE(if_ptr, TRUE); 
              PS_IFACE_SET_IS_ACTIVE(other_iface_ptr, TRUE); 
            } /* cannot allocate second iface tbl entry */
          } /* Profile type eHRPD & PDN type v4_v6 & IPV6 enabled */
          else 
          {
            DATA_IS707_MSG4( MSG_LEGACY_HIGH, 
                             "Set Iface 0x%x::%d to Passive. Profile PDN type %d,"
                             "IPV6 enabled: %d", 
                             if_ptr->name, if_ptr->instance, 
                             data_sess_profile_info_ptr->pdn_type,
                             ds3g_is_ipv6_enabled());
            PS_IFACE_SET_IS_ACTIVE(if_ptr, FALSE); 
          }
        }
        else
        {
          DATA_IS707_MSG2( MSG_LEGACY_ERROR, 
                           "Invalid pending profile %d Subs ID %d in post proc",
                           pending_id, policy_info_ptr->subs_id );
        }
#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif/* FEATURE_EHRPD */

        /*-----------------------------------------------------------------
          Store the last known good sys as PDN CB orig sys
        -----------------------------------------------------------------*/
        ds707_pdncntx_set_orig_sys(ds707_pdn_cntx_cb_ptr, 
                                   ds707_get_last_good_sys());
      } /* iface_tbl_ptr not associated with any pdn */
      else
      {
        DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                         "Iface 0x%x::%d not in use, but associated with PDN 0x%x", 
                         if_ptr->name, if_ptr->instance, ds707_pdn_cntx_cb_ptr );
      }
    } /* iface not in use */
    
    ds707_data_session_get_iface_requesting_profile(if_ptr,
                                                    &requesting_prof_ptr);

    ds707_data_session_get_iface_current_profile(if_ptr,
                                                 &current_prof_ptr);

    if (pending_id == DATA_SESSION_ALWAYS_SHARE_PROFILE)
    {
      if (current_prof_ptr != NULL)
      {
        pending_id = current_prof_ptr->data_session_profile_id;
      }
      else if (requesting_prof_ptr != NULL)
      {
        pending_id = requesting_prof_ptr->data_session_profile_id;
      }
      else
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                         "PDN 0x%x has invalid profile",
                         ds707_pdn_cntx_cb_ptr );

        pending_id = 
          ds707_data_session_get_profile_for_always_share_profile(policy_info_ptr);

        /*-----------------------------------------------------------------
          No profile can be used for ALWAYS_SHARE_PROFILE in post_proc.
          Set to default profile here as recovery plan.
        -----------------------------------------------------------------*/
        if (pending_id == DATA_SESSION_PROFILE_ID_INVALID)
        {
          pending_id = DATA_SESSION_DEFAULT_PROFILE;
        }
      }

      policy_info_ptr->data_session_profile_id = pending_id;
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "ALWAYS_SHARE_PROFILE sharing iface, "
                       "overwrite with profile %d",
                       pending_id );
    }

    /*-----------------------------------------------------------------------
      Set requesting id for the current data session to the profile id of 
      the new call (pending_id). This is done only if it is the first call.
    -----------------------------------------------------------------------*/
    if( (requesting_prof_ptr == NULL) && (current_prof_ptr == NULL) )
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                   "Setting requesting profile id = %d for first call bringup", 
                   pending_id);
      ds707_data_session_set_iface_requesting_profile(
        if_ptr, 
        pending_id,
        (ds_profile_subs_id_enum_type)(policy_info_ptr->subs_id));

      ds707_event_payload.ds707_event = EVENT_DS707_DATA_CALL_REQ_ACCEPTED;
      ds707_event_payload.info.zero = 0;
      event_report_payload( EVENT_DS707, sizeof(ds707_event_payload), 
                            &ds707_event_payload );
    }

    /*---------------------------------------------------------------------
      For Sockets calls only
        - check if the iface is in lingering state
        - check for the compatibility of the iface profiles and 
          laptop profiles
        - if both the above checks pass then 
        - call ps_iface_tear_down_cmd().
        - update the ids.
          sock_orig is deprecated and removed
    ---------------------------------------------------------------------*/
    if ( !PS_IFACE_IS_CALL_FROM_TETHERED_HOST(if_ptr) )
    {
      if (ps_iface_state(if_ptr) == IFACE_LINGERING)
      {
        /*-------------------------------------------------------------------
          Here we are interested in sock call in lingering state
          so only current profile is needed.
         -------------------------------------------------------------------*/
        if (!(ds707_data_session_compatible_profiles(
              if_ptr,policy_info_ptr,pending_id,current_prof_ptr)))
        {
          ret_val = ps_iface_go_null_cmd ( if_ptr, &ps_iface_errno, NULL );
          if ((ret_val < 0) && (ps_iface_errno != DS_EWOULDBLOCK))
          {
            DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                            "iface tear down fail, errno: %d",ps_iface_errno);
          }
          else
          {
            /*-----------------------------------------------------------------
              Set requesting id (next profile waiting to be brought up) to the 
              profile id of the new call (pending_id).
            -----------------------------------------------------------------*/
            DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                            "Lingering iface tear down in progress"
                            "Setting requesting profile id = pending profile id %d", 
                            pending_id);
            ds707_data_session_set_iface_requesting_profile(
              if_ptr,
              pending_id,
              (ds_profile_subs_id_enum_type)(policy_info_ptr->subs_id));
          }
        }/* if (!(ds707_data_session_compatible_profiles(pending_id,current_id))) */
      }/* if (ps_iface_state(if_ptr) == IFACE_LINGERING) */
    }/* if (policy_info_ptr->is_sock_orig) */

    /*---------------------------------------------------------------------
      Set the Linger timer value according to the data session profile.
      Here we use STATIC_LINGER_CONFIG so that some positive value is set
      for the Linger timer (only if the data session profile allows Linger)
    ---------------------------------------------------------------------*/
    ds707_data_session_set_linger_tmr_val(if_ptr, STATIC_LINGER_CONFIG);
#ifdef FEATURE_EPC_HANDOFF
    /* Check if Context transfer is needed. This is needed in case of EPC
       handoffs                                                            */
    if (dssnet_set_epc_handoff_ctxt(if_ptr, policy_info_ptr) < 0)
    {
      /*-----------------------------------------------------------------------
        This should not fail. In case it fails, we don't want to continue
        with the call. But there is no way to bring down this call. so ASSERT
      -----------------------------------------------------------------------*/
      /*-----------------------------------------------------------------------
        If this ASSERT is replaced by other recovery mechanism, then free the
        memory allocated on the heap inside this function
      -----------------------------------------------------------------------*/
      DATA_ERR_FATAL("Error setting EPC handoff context, fail post proc");
      return;
    }
#endif /* FEATURE_EPC_HANDOFF */
#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
    /*---------------------------------------------------------------------
      Only if the interface is not already in use and increment the total
      (FMC+3GPP2+IWLAN) interface used count.
    ---------------------------------------------------------------------*/
    if ( !PS_IFACE_IS_IN_USE(if_ptr) && 
         PS_IFACE_IS_ADDR_FAMILY_V4(if_ptr) )
    {

     /*
        Verify the IP address family in the policy_info_ptr. Increment only
        if IPv4 or UNSPEC (defaulted to IPv4). Else, ensure that the 
        an F3 indicating IPv6 iface selection is printed.
     */
      if((IP_POL_FAMILY_IS(IFACE_IPV4_ADDR_FAMILY)) ||
            (IP_POL_FAMILY_IS(IFACE_UNSPEC_ADDR_FAMILY )))
      {
        num_used_3gpp2_v4_ifaces++;
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                        "DS707 Post Proc: IWLAN+CDMA IPv6 Iface used");
      }


      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                       "DS707 Post Proc: IWLAN+CDMA used Iface count %d",
                       num_used_3gpp2_v4_ifaces );

      if(num_used_3gpp2_v4_ifaces > DS707_MAX_PKT_CALLS)
      {
        /* This should not happen. ASSERT */
        ASSERT(0);
        return;
      }
    }
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */
  } /* if not just doing lookup */
} /* ds707_rt_acl_post_proc() */

/*===========================================================================
FUNCTION ds707_rt_policy_allows

DESCRIPTION
  This function checks if policy and current umsm iface allows.

PARAMETERS
  *this_if_ptr        - Ptr to the interface
  *policy_info_ptr    - Policy information
  *deny_reason        - reason for denial
DEPENDENCIES
  None.

RETURN VALUE
  TRUE : Policy supports it
  FALSE: Interface cannot be supported

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds707_rt_policy_allows
(
  ps_iface_type        *iface_ptr,
  acl_policy_info_type *policy_info_ptr,
  int* deny_reason
)
{
   boolean ret_val = TRUE;
   ps_iface_type *rmsm_um_iface = NULL;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   // Deny DUN calls in eHRPD mode
   if (ds707_pkt_is_ehrpd_mode_of_operation() && policy_info_ptr->proc_id == 1)
   {
       DATA_IS707_MSG0( MSG_LEGACY_HIGH,
            "DUN calls are not allowed in eHRPD mode.");
      *deny_reason = ACL_DENY_REASON_DUN_CALL_DISALLOWED;
       return FALSE;
   }
   rmsm_um_iface = ds707_rmsm_get_um_iface();
   /* If an RMSM call is coming up or up or going down */
   if ( PS_IFACE_IS_VALID(rmsm_um_iface) )
   {
      if ( !policy_info_ptr->is_routeable )
      {
         /* Fail the interface if the incoming call does not have the routeable
            flag set */
         
         if (iface_ptr == rmsm_um_iface)
         {
            *deny_reason = ACL_DENY_REASON_IFACE_IN_USE_CFG_MATCH;;
         }
         else
         {
            *deny_reason = ACL_DENY_REASON_POL_NO_MATCH;
         }
         DATA_IS707_MSG1( MSG_LEGACY_HIGH,
               "!policy_info_ptr->is_routeable, returning FALSE with deny_reason: %d", *deny_reason);
         ret_val = FALSE;
      }
   }
   return ret_val;
}
/*===========================================================================
FUNCTION DS707_RT_IS_IFACE_AVAILABLE

DESCRIPTION
  This function Checks to see if the specified interface is available for USE
  
PARAMETERS  
  *this_if_ptr        - Ptr to the interface
  *ipfilter_info_ptr  - Ip filter information
  *policy_info_ptr    - Policy information
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : Interface is available (either idle or in use but shareable)
  FALSE: Interface is NOT available (in use and not shareable)
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_IS_AVAILABLE_DEF2( ds707_rt_is_iface_available )
{
  ps_iface_type *if_ptr = (ps_iface_type *) this_if_ptr;
  ps_iface_type *rmsm_um_iface = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  rmsm_um_iface = ds707_rmsm_get_um_iface();
  /* If an RMSM call is coming up or up or going down */
  if ( PS_IFACE_IS_VALID(rmsm_um_iface) )
  {
    if ( !policy_info_ptr->is_routeable )
    {
      /* Fail the interface if the incoming call does not have the routeable
         flag set */
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "!policy_info_ptr->is_routeable, returning FALSE "
                       "with reason: IN_USE_CFG_MATCH");
      *reason_for_deny = ACL_DENY_REASON_IFACE_IN_USE_CFG_MATCH;
      return FALSE;
    }  
    else if ( rmsm_um_iface != if_ptr )
    {
      /* Fail the Interface if ( RMSM already in use, and is routeable, 
         and the current iface is not the same that RMSM is using ) */
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "rmsm_um_iface != if_ptr, returning FALSE with "
                       "reason: POL_NO_MATCH"); 
      *reason_for_deny = ACL_DENY_REASON_POL_NO_MATCH; 
      return FALSE;
    }
  }
  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(policy_info_ptr);
  DATA_MDM_ARG_NOT_CONST(pkt_info_ptr);
  
  /*-------------------------------------------------------------------------
    The following rules determine if the iface is available/not
    - if interface is not in use, it is available
    - if the interface is LINGERING state, it is always available
    - if interface is in use by a TE2 return FALSE
    - if interface is in use by sockets & routing is also done for 
      sockets, then iface is available : return TRUE
    - if interface is in use by TE2 & routing is done by sockets,
      iface is only available for routeable entities (which can be routed 
      in presence of TE2 calls), return FALSE for all others
    - if interface is in use by TE2 in relay model call, then
      sockets are not allowed to share the interface.
  -------------------------------------------------------------------------*/
  /* iface is in LINGERING state hence it is available */
  if ( ps_iface_state(if_ptr) == IFACE_LINGERING ) 
  {
    return TRUE;
  }
  /* iface not in use hence available */
  else if(!PS_IFACE_IS_IN_USE(if_ptr))
  {
#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
    /*-----------------------------------------------------------------------
      IWLAN_3GPP2 and 1x-v4 mutual exclusion.
      If interface is not in use and the num_used_3gpp2_v4_ifaces is equal 
      to or greater than maximum v4 packet calls allowed on CDMA then that 
      would mean that IWLAN-3GPP2 interface is in use. So return FALSE. That 
      would DENY the routing ACL of this interface.
    -----------------------------------------------------------------------*/
    if( PS_IFACE_IS_ADDR_FAMILY_V4(if_ptr) && 
        num_used_3gpp2_v4_ifaces >= DS707_MAX_PKT_CALLS)
    {

      /* 
        One IPv4 call is already up on CDMA - verify if the new call being
        brought up is a v6 call; return TRUE if so.
      */

      DATA_IS707_MSG2( MSG_LEGACY_MED,
                       "ds707_rt_is_iface_available: Policy IP family v6? %d"
                       "num_used_3gpp2_v4_ifaces %d",
                       IP_POL_FAMILY_IS(IFACE_IPV6_ADDR_FAMILY), 
                       num_used_3gpp2_v4_ifaces );

      if ( num_used_3gpp2_v4_ifaces == DS707_MAX_PKT_CALLS && 
           (IP_POL_FAMILY_IS(IFACE_IPV6_ADDR_FAMILY)) )
      {
        return TRUE;
      }

      /* 
          Return false otherwise, so as to ensure that only 1 IPv4 call is
          allowed at any time on CDMA for HRPD.
      */
      *reason_for_deny = ACL_DENY_REASON_MAX_V4_CONNECTIONS;
      return FALSE;
    }
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

    return TRUE;
  }
  /* iface in use, not available for TE2 */
  /* TODO: activate the follow and pass if it is during handoff */
/*  
  else if(!policy_info_ptr->is_sock_orig)
  {
    return FALSE;
  }
*/  
  /* iface in use but shareable, hence available for sockets */
  else if(PS_IFACE_IS_SHAREABLE(if_ptr))
  {
    return TRUE;
  }
  /* iface in use by TE2 but shreable by routeable sockets */
  else if(policy_info_ptr->is_routeable)
  {
    /* Disable relay model calls for routeable sockets as filtering cannot be 
       supported in relay model.
       Allow lookup to succeed for relay and network model calls as ioctls can 
       still be used. */
    if(ds707_rmsm_is_relay_model_call()&&
       (policy_info_ptr->lookup_only==FALSE))
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "ds707_rmsm_is_relay_model_call()&& (policy_info_ptr->lookup_only==FALSE), returning FALSE");
      *reason_for_deny = ACL_DENY_REASON_IFACE_IN_USE;
      return FALSE;
    }
    return TRUE;
  }
  /* iface in use by TE2 and cannot be shared by non routeable sockets */  
   DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                    "iface in use by TE2 and cannot be shared by non routeable sockets @ end, returning FALSE");
  *reason_for_deny = ACL_DENY_REASON_IFACE_IN_USE_CFG_MATCH;
  return FALSE;

} /* ds707_rt_is_iface_available() */


/*===========================================================================
FUNCTION DS707_RT_IS_CFG_SUPPORTED

DESCRIPTION
   This function checks to see if the configuration requested can be 
   supported by the specified interface ie can the interface be aciveted
   using the specified configuration. This will basically check if the new
   requested call is profile compatible with the current data session.
  
PARAMETERS  
  *this_if_ptr        - Ptr to the interface
  *ipfilter_info_ptr  - Ip filter information
  *policy_info_ptr    - Policy information
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : config can be supported
  FALSE: config cannot be supported (bad config or interface already 
         activated using a different config)
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_CFG_SUPPORTED_DEF2( ds707_rt_is_cfg_supported )
{  
  ps_iface_type                 *if_ptr = (ps_iface_type *) this_if_ptr;
  boolean                       ret_val = FALSE;
  ds707_data_session_profile_id pending_id; /* profile id of the new call */
#ifdef FEATURE_DS_MOBILE_IP
  ds707_mip_profile_type       *ds_static_profile_ptr = NULL;
#endif /*FEATURE_DS_MOBILE_IP*/
  uint8                         mip_val = 0;
  ds707_data_session_profile_info_type
                               *data_sess_prof_ptr = NULL;
  sys_modem_as_id_e_type resolved_subs_id = SYS_MODEM_AS_ID_NONE;
  uint8                         omh_profile_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(pkt_info_ptr);

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(reason_for_deny);

  pending_id = (ds707_data_session_profile_id) 
                        policy_info_ptr->data_session_profile_id;

  if( ds3g_resolve_subs_id(policy_info_ptr->subs_id ,&resolved_subs_id ) == FALSE) 
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,"Error in obataining resolved sub id %d "
                       "Policy Sub ID %d",policy_info_ptr->subs_id,resolved_subs_id);
    return FALSE;
  }

  if ( resolved_subs_id != SYS_MODEM_AS_ID_1 )
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR," resolved sub id %d is not supported"
                                     " for CDMA Call orig Policy Sub ID %d",
                                      policy_info_ptr->subs_id,resolved_subs_id);
    return FALSE;
  }

#ifdef FEATURE_DS_MOBILE_IP
  ds_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }
  mip_val = ds_static_profile_ptr->mip.ds_qcmip_val;

  /*-----------------------------------------------------------------------------
    Bypass profile compatibility check for MIP call, if requested with NV/RUIM 
    profile, as we do not support multiple MIP profiles in NV/RUIM.
  -----------------------------------------------------------------------------*/
  if( ( mip_val > 0 ) && 
      (( pending_id >= DATA_SESSION_NVRUIM_PROFILE_MIN ) && 
       ( pending_id < DATA_SESSION_NVRUIM_PROFILE_MIN + 
                           DATA_SESSION_NVRUIM_MAX_PROFILES )) )
  {
    if( mip_val == 1 )
    {
      omh_profile_index = (uint8)pending_id - DATA_SESSION_NVRUIM_PROFILE_MIN;
      ds707_data_session_set_sip_active_profile_index(omh_profile_index);
      DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                       "MIP value = %d, omh_profile_id = %d, profile_index = %d",
                       mip_val, pending_id, omh_profile_index);
    }

    policy_info_ptr->data_session_profile_id = DATA_SESSION_DEFAULT_PROFILE;
    pending_id = policy_info_ptr->data_session_profile_id;
    ds707_omh_profile_id = pending_id;
  }
  else
#endif /* FEATURE_DS_MOBILE_IP */

  /*---------------------------------------------------------------------------
    In NVRUIM based multiple profiles design, LBS app passes app type instead 
    of profile ID. So check to see if the app is LBS app. If so, check the 
    compatibility of LBS app with existing app. If LBS app is the first app 
    then allow LBS app.
  ---------------------------------------------------------------------------*/
  if( (ds707_data_session_is_nvruim_profiles_available()) &&
      (mip_val == 0) &&
      (ds707_data_session_is_omh_reqd(if_ptr) == TRUE)
    )
  {
    if( policy_info_ptr->app_identifier == LBS_APP_TYPE )
    {
      ret_val = ds707_data_session_compatibility_check( this_if_ptr, 
                                                        policy_info_ptr );

      if( !ret_val )
      {
        *reason_for_deny = ACL_DENY_REASON_IFACE_IN_USE;
      }

      if ( ( ret_val == TRUE ) && 
           ( PS_IFACE_IS_IN_USE ( if_ptr ) ) )
      {
        ret_val = IFACE_AND_POL_FAMILY_MATCH();
        if( !ret_val )
        {
          *reason_for_deny = ACL_DENY_REASON_IP_VERSION_MISMATCH;
        }
      }
      
      return ret_val;
    }

    if( pending_id == DATA_SESSION_DEFAULT_PROFILE )
    {
      pending_id = ds707_data_session_get_default_profile();
      policy_info_ptr->data_session_profile_id = pending_id;
      ds707_omh_profile_id = pending_id;
    }
  }
  else
  {
    ds707_omh_profile_id = DATA_SESSION_PROFILE_ID_INVALID;
  }

  /*-----------------------------------------------------------------------
    For ALWAYS_SHARE_PROFILE, there's no associated parameters. Check if
    IP family matches between the current iface and the policy and skip
    all other checks.
  ------------------------------------------------------------------------*/
  if( pending_id == DATA_SESSION_ALWAYS_SHARE_PROFILE )
  {
    if ( (PS_IFACE_IS_IN_USE ( if_ptr )) || 
         (ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(if_ptr) != NULL) )
    {
      ret_val = IFACE_AND_POL_FAMILY_MATCH();
    }
    else 
    {
      return TRUE;
    }

    if( ret_val == FALSE )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "ds707_rt_is_cfg_supported: IFACE_AND_POL_FAMILY_MATCH "
                       "failed" );
      *reason_for_deny = ACL_DENY_REASON_IP_VERSION_MISMATCH;
      return FALSE;
    }
    else
    {
      /*-----------------------------------------------------------------------
        For ALWAYS_SHARE_PROFILE, skip the rest of checks
      ------------------------------------------------------------------------*/
      return TRUE;
    }
  }

   DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Pending ID is %d",pending_id);

  /*---------------------------------------------------------------------------
    check if a call can be allowed on this iface 
  ---------------------------------------------------------------------------*/
  if( ds707_rt_is_call_allowed (pending_id, if_ptr,
                                policy_info_ptr, reason_for_deny) == TRUE)
  {
  
        ds707_data_session_get_iface_requesting_profile(if_ptr,
                                                        &data_sess_prof_ptr);

    /*--------------------------------------------------------------------------- 
     if the requesting_profile is invalid that means there is no PPP session bringup 
     in progress or queued and we can continue. Also if pending_id and 
     requesting_id are compatible then we can continue. Otherwise we will return
     false.
    ---------------------------------------------------------------------------*/
    if (
         (data_sess_prof_ptr == NULL) ||
         (ds707_data_session_compatible_profiles( if_ptr, 
                                                  policy_info_ptr, 
                                                  pending_id, 
                                                  data_sess_prof_ptr))
       )
    {
      ds707_data_session_get_iface_current_profile(if_ptr,
                                                   &data_sess_prof_ptr);
      /*------------------------------------------------------------------------
       check if the data session profile ids are friends of each other. If they
       are then allow the incoming call request.
      ------------------------------------------------------------------------*/
      if(
          (data_sess_prof_ptr == NULL) ||
          (ds707_data_session_compatible_profiles( if_ptr,
                                                 policy_info_ptr, 
                                                 pending_id, 
                                                   data_sess_prof_ptr )) 
        )
      {
        ret_val = TRUE;
      }
      /*------------------------------------------------------------------------
        If the profiles are not friends then allow the incoming call request
        only when the iface is DOWN or LINGERING
      ------------------------------------------------------------------------*/
      else if (
                (ps_iface_state(if_ptr) == IFACE_DOWN) ||
                (ps_iface_state(if_ptr) == IFACE_LINGERING)
              )
      {
        ret_val = TRUE;
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                         "ds707_rt_is_cfg_supported: profile compatibility "
                         "failed" );
        *reason_for_deny = ACL_DENY_REASON_APN_MISMATCH;
        ret_val = FALSE;
      }
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "ds707_rt_is_cfg_supported: profile compatibility "
                       "failed" );
      *reason_for_deny = ACL_DENY_REASON_APN_MISMATCH;
      ret_val = FALSE;
    }
  }

  /*---------------------------------------------------------------------------
    if IFACE is in USE or is attached to a PDN CB then we have to check for
    IFACE and policy IP type match. For a dual IP case, IFACE may not be in
    use but it may still be attached to a PDN CB which means it has a valid
    IP family set. That's why just checking in use is not enough.
  ---------------------------------------------------------------------------*/
  if ( ( ret_val == TRUE ) &&
       ( (PS_IFACE_IS_IN_USE ( if_ptr )) || 
         (ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(if_ptr) != NULL) ) )
  {
    ret_val = IFACE_AND_POL_FAMILY_MATCH();

    if( ret_val == FALSE )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "ds707_rt_is_cfg_supported: IFACE_AND_POL_FAMILY_MATCH "
                       "failed" );
      *reason_for_deny = ACL_DENY_REASON_IP_VERSION_MISMATCH;
    }
  }
  
  /*---------------------------------------------------------------------------
    Fill the DENY reason with ACL_DENY_REASON_IFACE_IN_USE if the profile is 
    NVRUIM based profile.and no special deny reason has been set till now. This
    will indicate PS to do arbitration between existing app and incoming app.
  ---------------------------------------------------------------------------*/
  if( !ret_val && ds707_data_session_is_nvruim_profile(pending_id) &&
      *reason_for_deny == ACL_DENY_REASON_POL_NO_MATCH )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_rt_is_cfg_supported: ret val is FALSE and "
                     "pending profile is a NVRUIM profile. Deny with "
                     "reason IFACE_IN_USE" ); 
    *reason_for_deny = ACL_DENY_REASON_IFACE_IN_USE;
  }

  if( !ret_val )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "ds707_rt_is_cfg_supported rule failed with deny "
                     "reason : %d", *reason_for_deny);
  }

  return ret_val;
} /* ds707_rt_is_cfg_supported() */

/*===========================================================================
FUNCTION DS707_RT_IS_CFG_SUPPORTED_AND_IFACE_AVAILABLE

DESCRIPTION
   This function checks to see if the configuration requested can be 
   supported by the specified interface ie can the interface be aciveted
   using the specified configuration. This will basically check if the new
   requested call is profile compatible with the current data session.
   Then it checks to see if the specified interface is available for USE

PARAMETERS  
  *this_if_ptr        - Ptr to the interface
  *ipfilter_info_ptr  - Ip filter information
  *policy_info_ptr    - Policy information
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : 0) config can be supported and iface available
  FALSE: 1) config can be supported,    but iface not available
            Deny reason: ACL_DENY_REASON_IFACE_IN_USE_CFG_MATCH
         2) config cannot be supported,  but iface available
            Deny reason: same as Deny Reason provided by cfg_supported()
         3) config cannot be supported, and iface not available
            Deny reason: same as Deny Reason provided by cfg_supported()
         

SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_CFG_SUPPORTED_DEF2(ds707_rt_is_cfg_supported_and_iface_available)
{
  int     cfg_supported_deny_reason = 0;
  ds707_iface_table_type *ds707_iface_tbl_ptr = NULL;
  ds707_pdn_context_cb_type *ds707_pdn_cntx_cb_ptr = NULL;

  /* Cases */
  /*  Config NOT supported  Iface NOT available -> Result
  0)  0                     0                      TRUE
  1)  0                1                FALSE, Reason: IFACE_IN_USE_CFG_MATCH 
  2)  1                     0                      FALSE, Reason provided by cfg_supported() 
  3)  1                     1                      FALSE, Reason provided by cfg_supported() 
  */

  ds707_iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(this_if_ptr);
  ds707_pdn_cntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr(ds707_iface_tbl_ptr);
  if (DS707_PDNCNTX_CB_IS_VALID(ds707_pdn_cntx_cb_ptr))
  {
    /* Check if the stored last known good sytem is same as the PDN CB orig sys. 
       DENY if they are different */
    if (ds707_get_last_good_sys() 
        != ds707_pdncntx_get_orig_sys(ds707_pdn_cntx_cb_ptr))
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                  "PDN CB orig sys & last known good sys are not compatible"
                  " - Deny with reason EPC_NONEPC_TRANSITION");
      *reason_for_deny = ACL_DENY_REASON_EPC_NONEPC_TRANSITION;
      return FALSE;
    }
  }

  if( TRUE == ds707_is_term_in_progress() ) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "PPP termination in progress, disallow any call");
    *reason_for_deny = ACL_DENY_REASON_POL_NO_MATCH;
    return FALSE;
  }
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  else if( ds707_nv_util_is_refresh_in_progress(policy_info_ptr->subs_id) ==  TRUE )
  {
     /*Debug mesg*/
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "NV Refresh in progress, disallow any call");
    *reason_for_deny = ACL_DENY_REASON_NV_REFRESH_IN_PROGRESS;
    return FALSE;
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
  if(ds707_rt_is_cfg_supported(this_if_ptr, 
                                       policy_info_ptr, 
                                       pkt_info_ptr, 
                               &cfg_supported_deny_reason) 
     == FALSE )
  {
    /* cases 2) and 3) */
    if(cfg_supported_deny_reason >= 0)
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "cfg_supported returned FALSE, but with invalid DENY "
                       "Reason: %d - default to POL_NO_MATCH", 
                       cfg_supported_deny_reason);
      /* overwrite deny reason to default -1 = POLICY_NO_MATCH */                 
      *reason_for_deny = ACL_DENY_REASON_POL_NO_MATCH;
    }
    else
    {
    *reason_for_deny = cfg_supported_deny_reason;
    }
    
    return FALSE;
  }
  else /* cfg supported */
  {
    if(ds707_rt_policy_allows(this_if_ptr,
            policy_info_ptr, reason_for_deny) == TRUE)
    {
    if(ds707_rt_is_iface_available(this_if_ptr, 
                                   policy_info_ptr, 
                                   pkt_info_ptr,
                                   &cfg_supported_deny_reason) == FALSE)
  {

        if(cfg_supported_deny_reason >= 0)
        {
          DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                           "ds707_rt_is_iface_available returned FALSE, "
                           "but with invalid DENY Reason: %d - default to"
                           "in_use_cfg_match", 
                           cfg_supported_deny_reason);
          /* overwrite deny reason to IN_USE_CFG_MATCH*/
    *reason_for_deny = ACL_DENY_REASON_IFACE_IN_USE_CFG_MATCH;
        }
        else
        {
          *reason_for_deny = cfg_supported_deny_reason;
        }
        DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                         "ds707_rt_is_iface_available returned FALSE, "
                         "with deny reason %d, so returning FALSE",
                         *reason_for_deny);

      return FALSE;
  }
    else /* iface available */
  {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "ds707_rt_is_iface_available returned TRUE");
      /* case 0) */
      return TRUE;
    }
  }
    else 
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "ds707_rt_policy_allows returned FALSE with deny "
                       "reason: %d, returning FALSE", *reason_for_deny);
      return FALSE;
    }
  }
}

/*===========================================================================
FUNCTION DS707_RT_IS_CFG_SHAREABLE

DESCRIPTION
   This function checks to see if the interface can be allowed to share
   one more user of the interface provided the configuration matches.
   
    NOTE: For CDMA ifaces there is no config to compare right now. Add
    it later when v6 or multiple CDMA ifaces show up.   
  
PARAMETERS  
  *this_if_ptr        - Ptr to the interface
  *ipfilter_info_ptr  - Ip filter information
  *policy_info_ptr    - Policy information

    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : Inteface not activated or already activated with same config.
  FALSE: Interface not shareable, using different config
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_CFG_SHAREABLE_DEF( ds707_rt_is_cfg_shareable )
{
  ps_iface_type *if_ptr = (ps_iface_type *) this_if_ptr;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(policy_info_ptr);
  DATA_MDM_ARG_NOT_CONST(pkt_info_ptr);

  /*-------------------------------------------------------------------------
    Check if the profiles are compatible on this IFACE, if they are 
    compatible then increase the priority of this iface.
    It only returns TRUE if it is in USE or it has been looked up in CDMA mode

    This is a PASS Rule
  -------------------------------------------------------------------------*/
  if ((PS_IFACE_IS_IN_USE(if_ptr) || PS_IFACE_IS_LOOKED_UP(if_ptr)) &&
       ds707_is_pref_pkt_mode(policy_info_ptr, SYS_SYS_MODE_CDMA) )
  {
    ret_val =  TRUE;
  }
  
  return ret_val;

} /* ds707_rt_is_cfg_shareable() */

boolean ds707_rt_acl_iface_and_policy_family_match(
  ps_iface_type        *iface_ptr, 
  acl_policy_info_type *policy_info_ptr
)
{
  if ( (iface_ptr == NULL) ||
       (policy_info_ptr == NULL) ) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_rt_acl_iface_and_policy_family_match: "
                     "iface_ptr = NULL || policy_info_ptr = NULL" );
    return FALSE;
  }

  if ( IP_POL_FAMILY_IS((int)IFACE_UNSPEC_ADDR_FAMILY) ||
      (IP_POL_FAMILY_IS((int)IFACE_IPV6_ADDR_FAMILY) &&
       ps_iface_addr_family_is_v6(iface_ptr)) ||
      (IP_POL_FAMILY_IS((int)IFACE_IPV4_ADDR_FAMILY) &&
       ps_iface_addr_family_is_v4(iface_ptr)))
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "ds707_rt_acl_iface_and_policy_family_match: "
                     "Iface Family Pol IP Family, (Match)" );
    return TRUE;
  }

  if (PS_IFACE_GET_SUBS_ID(iface_ptr) != PS_SYS_DEFAULT_SUBS &&
      PS_IFACE_GET_SUBS_ID (iface_ptr) == policy_info_ptr->subs_id )
  {
    DATA_IS707_MSG2( MSG_LEGACY_MED, 
                     "ds707_rt_acl_iface_and_policy_subsID_match: "
                     "Iface subs id %d , policy_subs_id %d",
                     PS_IFACE_GET_SUBS_ID(iface_ptr), 
                     policy_info_ptr->subs_id );
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION DS707_RT_ACL_USE_SELECTED_IFACE

DESCRIPTION
   This function checks to see if the selected iface can be used.
   2 ifaces can be used in legacy hrpd mode, one for v4 and one for v6
   In eHRPD mode, we need to check, if there is any other iface up
   with the same address family and same APN.
   For Dual IP, we have have to block the request on a pdn for
   which network returned the other ip type than the one requested in this
   policy, even if the compatibility pass.
   
PARAMETERS  
   *this_if_ptr             pointer to the interface
   *policy_info_ptr         pointer to policy info
   *pkt_info_ptr            pointer to pkt info
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : If selected iface can be used
  FALSE: If selected iface cannot be used
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_IS_AVAILABLE_DEF2( ds707_rt_acl_use_selected_iface )
{
  ps_iface_type *iface_ptr = NULL;
  uint8          i = 0;
  boolean v4_iface_used = FALSE;
  boolean v6_iface_used = FALSE;
  ps_iface_type *v4_iface_ptr = NULL;
  ps_iface_type *v6_iface_ptr = NULL;
  ps_iface_type *if_ptr = (ps_iface_type *) this_if_ptr;
  ds707_pdn_context_cb_type *pdn_cb_ptr = NULL;
  ds707_pdn_context_cb_type *if_pdn_cb_ptr = NULL;

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "ds707_rt_acl_use_selected_iface: Iface 0x%x::%d ", 
                   if_ptr->name, if_ptr->instance ); 

  if ( ds707_pdnctx_are_all_pdns_invalid() == TRUE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "ds707_rt_acl_use_selected_iface: no PDN up, return TRUE");
    return TRUE;
  }
  else
  {
#ifdef FEATURE_EHRPD
    /*--------------------------------------------------------------------
    handle the following cases:
      1. ehrpd stand-alone
         - check if we already in ehrpd mode
         - note that in policy, handoff class is PS_IFACE_HANOFF_CALSS_NONE
      2. handoff (lte->ehrpd)
         - always pass the following check since checking for ehrpd mode should
           return TRUE. this is because that handoff initiation only starts
           after HDR session open per EPC.
      3. handoff (ehrpd->lte)
         - we may not be in ehrpd mode, but still need to check compatibility
           for a lte iface on a plociy from EPC rt acl, so checking the handoff
           flag to make sure the compatibility is checked
      4. ALWAYS SHARE CONNECTION
    -------------------------------------------------------------------*/
    if ( ds707_pkt_is_ehrpd_mode_of_operation() ||
         ds707_pkt_mgr_is_data_on_ehrpd() == TRUE
#ifdef FEATURE_EPC_HANDOFF
          || ds707_is_policy_handoff_class_epc(policy_info_ptr)
#endif
         || (ds707_get_last_good_sys() == DS707_SYS_INFO_EPC_SYSTEM)
       )
    {
      /*-----------------------------------------------------------------------
        For ALWAYS_SHARE_PROFILE, if default PDN is up and IP matches, give
        priority to default PDN. If default PDN not up, allow any PDN that
        is up and IP matches. Otherwise, if no PDN up or all up PDNs do not
        have matching IP type, new PDN needs be brought up using the first 
        non NULL APN with matched IP type.
       
        Note this logic has the legacy limitation to not check iface in going
        down state, and may allow these ifaces. There are recovery mechanisms
        implemented to move iface back to coming up status, but if for some
        reasons DS fail to bring up the iface again, a retry is needed from
        client/application.
      ------------------------------------------------------------------------*/
      if ( policy_info_ptr->data_session_profile_id == 
             DATA_SESSION_ALWAYS_SHARE_PROFILE )
      {
        DATA_IS707_MSG2( MSG_LEGACY_MED, 
                         "ds707_rt_acl_use_selected_iface: "
                         "ALWAYS_SHARE_PROFILE used in ehrpd mode, "
                         "Iface 0x%x::%d ",
                         if_ptr->name, if_ptr->instance );
        if_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(if_ptr);
        for( i = 0; i < DS707_MAX_PDN_INSTANCES; i++ )
        {
          pdn_cb_ptr = ds707_get_pdncntx_cb_from_instance(i);      
          if ( DS707_PDNCNTX_CB_IS_VALID(pdn_cb_ptr) )
          {
            v4_iface_ptr = ds707_pdncntx_get_v4_iface(pdn_cb_ptr);
#ifdef FEATURE_DATA_PS_IPV6      
            v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdn_cb_ptr);
#endif /* FEATURE_DATA_PS_IPV6 */    

            if( IP_POL_FAMILY_IS(IFACE_UNSPEC_ADDR_FAMILY) ||
                ( IP_POL_FAMILY_IS(IFACE_IPV4_ADDR_FAMILY) && 
                  (NULL != v4_iface_ptr) ) ||
                ( IP_POL_FAMILY_IS(IFACE_IPV6_ADDR_FAMILY) && 
                  (NULL != v6_iface_ptr) )
              )
            {
              if ( (NULL == if_pdn_cb_ptr)
                   || 
                  (!DS707_PDNCNTX_CB_IS_VALID(if_pdn_cb_ptr)) )
              {
                DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                                 "PDN %d UP and IP match but this iface not UP,"
                                 " deny this iface", i );
                return FALSE;
              }

              if ( ds707_pdncntx_is_default_pdn(pdn_cb_ptr) )
              {
                if (pdn_cb_ptr == if_pdn_cb_ptr )
                {
                  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                                   "Default PDN UP and IP match, "
                                   "this iface belongs to default PDN, "
                                   "return TURE");
                  return TRUE;
                }
                else
                {
                  DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                                   "Default PDN UP and IP match, but "
                                   "this iface doesn't belong to default PDN," 
                                   "return FALSE");
                  return FALSE;
                }
              }
            }
          }
        }

        if ( (NULL == if_pdn_cb_ptr) 
             || 
             (!DS707_PDNCNTX_CB_IS_VALID(if_pdn_cb_ptr)) )
        {
          policy_info_ptr->data_session_profile_id = 
            ds707_data_session_get_profile_for_always_share_profile(policy_info_ptr);

          DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                           "No PDN UP with matching IP type, new bring up "
                           "selected profile ID %d",
                           policy_info_ptr->data_session_profile_id);
          
          /*-----------------------------------------------------------------
            No profile can be used for ALWAYS_SHARE_PROFILE, return FALSE
          -----------------------------------------------------------------*/
          if ( policy_info_ptr->data_session_profile_id 
                 == DATA_SESSION_PROFILE_ID_INVALID )
          {
            return FALSE;
          }

          /*--------------------------------------------------------------------
            Fall through to check if selected profile is compatible
          -------------------------------------------------------------------*/
        }
        else
        {
          return TRUE;
        }
      }

      /*--------------------------------------------------------------------
        For ehrpd mode, if there is a pdn up with the APN same as
        requested in this policy, pass this iface if it is same as one of
        the iface table entry for this PDN and the address family matches.
      -------------------------------------------------------------------*/
      DATA_IS707_MSG2( MSG_LEGACY_MED, 
                       "ds707_rt_acl_use_selected_iface: ehrpd mode, "
                       "Iface 0x%x::%d ",
                       if_ptr->name, if_ptr->instance );
      for( i = 0; i < DS707_MAX_PDN_INSTANCES; i++ )
      {
        pdn_cb_ptr = ds707_get_pdncntx_cb_from_instance(i);      
        if ( DS707_PDNCNTX_CB_IS_VALID(pdn_cb_ptr) )
        {
          if( ds707_pdncntx_check_compatibility( pdn_cb_ptr, policy_info_ptr ) 
              == TRUE )
          {
            /*-----------------------------------------------------------------
              This PDN is UP with the APN same as the one requested in the
              policy. Check if the if_ptr is same as one of the iface table
              entries associated with this PDN and if the addr family matches
            -----------------------------------------------------------------*/
            DATA_IS707_MSG1( MSG_LEGACY_MED, 
                             "ds707_rt_acl_use_selected_iface: "
                             "PDN context %d UP with same or default APN", i );
            v4_iface_ptr = ds707_pdncntx_get_v4_iface(pdn_cb_ptr);
#ifdef FEATURE_DATA_PS_IPV6      
            v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdn_cb_ptr);
#endif /* FEATURE_DATA_PS_IPV6 */    
 
            if( IFACE_IPV4_ADDR_FAMILY == policy_info_ptr->ip_family &&
                NULL == v4_iface_ptr)
            {
              DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                               "ds707_rt_acl_use_selected_iface: "
                               "Policy requires V4 but PDN context %d "
                               "does not have a V4 iface. Deny with "
                               "reason V4_DISALLOWED_SUBS_LIMITED", i );
              *reason_for_deny = ACL_DENY_REASON_V4_DISALLOWED_SUBS_LIMITED;
              return FALSE;
            }
            else if( IFACE_IPV6_ADDR_FAMILY == policy_info_ptr->ip_family &&
                     NULL == v6_iface_ptr )
            {
              DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                               "ds707_rt_acl_use_selected_iface: "
                               "Policy requires V6 but PDN context %d "
                               "does not have a V6 iface. Deny with "
                               "reason V6_DISALLOWED_SUBS_LIMITED", i );
              *reason_for_deny = ACL_DENY_REASON_V6_DISALLOWED_SUBS_LIMITED;
              return FALSE;
            }
             
            if (if_ptr == v4_iface_ptr) 
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: iface match "
                               "return TRUE" );
              return TRUE;
            }
#ifdef FEATURE_DATA_PS_IPV6
            else
            {
              if(if_ptr == v6_iface_ptr)
              {
                DATA_IS707_MSG0( MSG_LEGACY_MED, 
                                 "ds707_rt_acl_use_selected_iface: iface match "
                                 "return TRUE" );
                return TRUE;
              }
              else
              {
                DATA_IS707_MSG0( MSG_LEGACY_MED, 
                                 "ds707_rt_acl_use_selected_iface: iface mismatch "
                                 "return FALSE" );
                *reason_for_deny = ACL_DENY_REASON_POL_NO_MATCH;
                return FALSE;
              }
            }
#endif /* FEATURE_DATA_PS_IPV6 */
          }
        }
      } /* for loop */
      /*--------------------------------------------------------------------
        ehrpd mode, no pdn up with the same configuration, pass this iface
      --------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_rt_acl_use_selected_iface: "
                       "no PDN up with the same config, return TRUE" );
      return TRUE;
    } /* ehrpd mode */
    else 
#endif /* FEATURE_EHRPD */
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_rt_acl_use_selected_iface: hrpd mode" );
      /*-----------------------------------------------------------------
        If it is not ehrpd mode of operation then we can use only 2 ifaces
        one for v4 and one for v6. If iface is up with the address family 
        requested in the policy, pass only if it is the same iface, 
        fail if any other iface has been selected. This condition 
        ensures that we use 1 iface per address family. 
      -----------------------------------------------------------------*/
      for( i = 0; i < DS707_MAX_IFACES; i++ )
      {
        /*-----------------------------------------------------------------
          If iface is up with v4 / v6, save the iface ptr and set the
          corresponding used flags for the iface.
        -----------------------------------------------------------------*/
        iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(i);
        if( !iface_ptr )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Pointer to iface is NULL" );
          return FALSE;
        }
        if ( (ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr) != NULL) )
        {
          if( ps_iface_addr_family_is_v4(iface_ptr) && !v4_iface_used )
          {
            v4_iface_used = TRUE;
            v4_iface_ptr = iface_ptr;
            DATA_IS707_MSG2( MSG_LEGACY_MED, 
                             "Found V4 Iface 0x%x::%d in use", 
                             iface_ptr->name, iface_ptr->instance );
          }
          if( ps_iface_addr_family_is_v6(iface_ptr) && !v6_iface_used )
          {
            v6_iface_used = TRUE;
            v6_iface_ptr = iface_ptr;
            DATA_IS707_MSG2( MSG_LEGACY_MED, 
                             "Found V6 Iface 0x%x::%d in use", 
                             iface_ptr->name, iface_ptr->instance );
          }
          if ( v4_iface_used && v6_iface_used )
          {
            break;
          }
        }
      }

      if( (v4_iface_used == FALSE) &&
          (v6_iface_used == FALSE) )
      {
        /*-----------------------------------------------------------------
          if no ifaces are in use, return TRUE.
        -----------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "ds707_rt_acl_use_selected_iface: "
                         "hrpd mode, no ifaces in use, return TRUE");
        return TRUE;
      }
      else
      {
        if( (IP_POL_FAMILY_IS(IFACE_IPV4_ADDR_FAMILY)) ||
            (IP_POL_FAMILY_IS(IFACE_UNSPEC_ADDR_FAMILY)) )
        {
          if( v4_iface_used == TRUE )
          {
            /*-----------------------------------------------------------------
              if the policy requests V4 address family, pass this iface if its
              the same as the V4 iface pointer in use.
            -----------------------------------------------------------------*/
            if (v4_iface_ptr == if_ptr) 
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: "
                               "iface in use for v4 same as this_iface_ptr, return TRUE" ); 
              return TRUE;
            }
            else
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: "
                               "iface in use for v4 NOT same as this_iface_ptr, "
                               "return FALSE with deny reason: POL_NO_MATCH" ); 
              *reason_for_deny = ACL_DENY_REASON_POL_NO_MATCH;
              return FALSE;
            }
          }
          else
          {
            /*-----------------------------------------------------------------
              if no iface is in use for V4 call, pass this iface for only if
              its not in use. (This condition prevents the iface being used
              by a V6 call to be passed for this V4 request)
            -----------------------------------------------------------------*/
            if( ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(if_ptr) != NULL )
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: "
                               "no iface in use for v4, this_iface_ptr in use," 
                               "return FALSE with deny reason: IP_VERSION_MISMATCH" ); 
              *reason_for_deny = ACL_DENY_REASON_IP_VERSION_MISMATCH;
              return FALSE;
            }
            else
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: "
                               "no iface in use for v4, this_iface_ptr not in use, return TRUE" ); 
              return TRUE;
            }
          }
        }
        else if( (IP_POL_FAMILY_IS(IFACE_IPV6_ADDR_FAMILY)) ||
                 (IP_POL_FAMILY_IS(IFACE_UNSPEC_ADDR_FAMILY)) )
        {
          if( v6_iface_used == TRUE )
          {
            /*-----------------------------------------------------------------
              if the policy requests V6 address family, pass this iface if its
              the same as the V6 iface pointer in use.
            -----------------------------------------------------------------*/
            if (v6_iface_ptr == if_ptr) 
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: "
                               "iface in use for v6 same as this_iface_ptr, return TRUE" ); 
              return TRUE;
            }
            else
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: "
                               "iface in use for v6 NOT same as this_iface_ptr, "
                               "return FALSE with deny reason: POL_NO_MATCH" ); 
              *reason_for_deny = ACL_DENY_REASON_POL_NO_MATCH;
              return FALSE;
            }
          }
          else
          {
            /*-----------------------------------------------------------------
              if no iface is in use for V6 call, pass this iface for only if
              its not in use. (This condition prevents the iface being used
              by a V4 call to be passed for this V6 request)
            -----------------------------------------------------------------*/
            if( ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(if_ptr) != NULL )
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: "
                               "no iface in use for v6, this_iface_ptr in use, "
                               "return FALSE with deny reason: IP_VERSION_MISMATCH" ); 
              *reason_for_deny = ACL_DENY_REASON_IP_VERSION_MISMATCH;
              return FALSE;
            }
            else
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                               "ds707_rt_acl_use_selected_iface: "
                               "no iface not in use for v6, this_iface_ptr not in use, return TRUE" ); 
              return TRUE;
            }
          }
        } /* addr family in policy*/
      } /* no ifaces in use */
    }
  } /* i != MAX_PDN*/
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "ds707_rt_acl_use_selected_iface: no conditions met, return FALSE" ); 
  return FALSE;
} /* ds707_rt_acl_use_selected_iface */

/*===========================================================================
FUNCTION DS707_RT_IS_PREF_SYS

DESCRIPTION
  This function checks to see if CDMA/HDR is the system available on
  preferred radio
  
PARAMETERS  
  *this_if_ptr        - Ptr to the interface
  *ipfilter_info_ptr  - Ip filter information
  *policy_info_ptr    - Policy information
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : If 1X or DO, one of them, is available
  FALSE: niether is available on preferred radio
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_IS_AVAILABLE_DEF2 ( ds707_rt_is_pref_sys )
{
  ds707_data_session_profile_id pending_id; /* profile id of the new call */
  ds707_data_session_profile_info_type *data_sess_profile_ptr = NULL;
  sys_modem_as_id_e_type            resolved_subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  pending_id = (ds707_data_session_profile_id) 
                 policy_info_ptr->data_session_profile_id;

#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-----------------------------------------------------------------------------
   If its for a Data path routing during handover, then dont deny the iface.
   Skip pref sys check , because pref system would have got changed but still 
   the SRAT iface could be up.
  -------------------------------------------------------------------------------*/
  if ( policy_info_ptr->lookup_only == FALSE && 
       policy_info_ptr->bring_up == FALSE)
   {
     return TRUE;
   }  
#endif  /*FEATURE_DATA_WLAN_MAPCON */

  /*----------------------------------------------------------------------------
    Check if CDMA/HDR is the system available on the preferred radio.
    This check should not be performed for handoff routing. 
  ----------------------------------------------------------------------------*/
#ifdef FEATURE_EPC_HANDOFF
  if(policy_info_ptr->handoff_ctxt.handoff_class != PS_IFACE_HANDOFF_CLASS_NONE)
  {
    return TRUE;
  }
#endif 

  /*----------------------------------------------------------------------------
    For ALWAYS_SHARE_PROFILE, if there's existing PDN up and running, skip
    pref_sys check, otherwise, search for proper profile for first bring up
  ----------------------------------------------------------------------------*/
  if (pending_id == DATA_SESSION_ALWAYS_SHARE_PROFILE)
  {
    if (ds707_pdnctx_are_all_pdns_invalid())
    {
      pending_id = 
        ds707_data_session_get_profile_for_always_share_profile(policy_info_ptr);

      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                      "ALWAYS_SHARE_PROFILE requested when no PDNs up, "
                      "Selected profile %d for first call bringup",
                      pending_id);

      /*-----------------------------------------------------------------
        No profile can be used for ALWAYS_SHARE_PROFILE, return FALSE
      -----------------------------------------------------------------*/
      if ( pending_id == DATA_SESSION_PROFILE_ID_INVALID )
      {
        return FALSE;
      }

      policy_info_ptr->data_session_profile_id = pending_id;
    }
    else
    {
      return TRUE;
    }
  }

  if( FALSE == 
         ds707_data_session_get_profile(
            pending_id,
            (ds_profile_subs_id_enum_type)(policy_info_ptr->subs_id),
            &data_sess_profile_ptr) )
  {
    *reason_for_deny = ACL_DENY_REASON_INVALID_PROFILE;
    return FALSE;
  }

  if( ds3g_resolve_subs_id(policy_info_ptr->subs_id ,&resolved_subs_id ) == FALSE) 
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,"Error in obataining resolved sub id %d "
                       "Policy Sub ID %d",policy_info_ptr->subs_id,resolved_subs_id);
    return FALSE;
  }

  if( !(ds707_is_apn_pref_pkt_mode_ex(data_sess_profile_ptr, SYS_SYS_MODE_CDMA,resolved_subs_id)) &&
      !(ds707_is_apn_pref_pkt_mode_ex(data_sess_profile_ptr, SYS_SYS_MODE_HDR,resolved_subs_id)) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "In ds707_rt_is_pref_sys: pref sys not CDMA or HDR, "
                     "Return FALSE");
    *reason_for_deny = ACL_DENY_REASON_UNPREFERRED_RAT;
    return FALSE;
  }

  return TRUE;
} /* ds707_rt_is_pref_sys */

/*===========================================================================
FUNCTION DS707_IS_PREF_PKT_MODE

DESCRIPTION
  This function checks if the policy is set to prereg. If yes, then
  returns TRUE unconditionally. Else runs the pref mode check
  
PARAMETERS  
  *policy_info_ptr    - Policy information
   mode               - System mode
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : If policy is Prereg or pref mode is CDMA
  FALSE: If pref mode is not CDMA
  
SIDE EFFECTS 
  None.

===========================================================================*/
static boolean ds707_is_pref_pkt_mode
( 
  acl_policy_info_type *policy_info_ptr,
  sys_sys_mode_e_type   mode 
)
{
  ds707_data_session_profile_id pending_id = -1; /* profile id of the new call */
  ds707_data_session_profile_info_type *data_sess_profile_ptr = NULL;
  sys_modem_as_id_e_type        resolved_subs_id = SYS_MODEM_AS_ID_NONE;
  ds_sys_subscription_enum_type policy_subs_id = DS_SYS_DEFAULT_SUBS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( policy_info_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_is_pref_pkt_mode: Null policy info ptr");
    return FALSE;
  }
  else
  {
    pending_id = (ds707_data_session_profile_id) 
                    policy_info_ptr->data_session_profile_id;
#ifdef FEATURE_DATA_OPTHO
    if ( policy_info_ptr->handoff_ctxt.handoff_class == 
                                    PS_IFACE_HANDOFF_CLASS_EPC_PREREG
     )
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "ds707_is_pref_pkt_mode: Policy Set to prereg, "
                       "Skipping pref mode check and returning TRUE");
      return TRUE;
    }
    else
#endif /* FEATURE_DATA_OPTHO */
    {
      /*----------------------------------------------------------------------------
        For ALWAYS_SHARE_PROFILE, if there's existing PDN up and running, skip
        pref_sys check, otherwise, search for proper profile for first call bring up
      ----------------------------------------------------------------------------*/
      if (pending_id == DATA_SESSION_ALWAYS_SHARE_PROFILE)
      {
        if (ds707_pdnctx_are_all_pdns_invalid())
        {
          pending_id = 
          ds707_data_session_get_profile_for_always_share_profile(policy_info_ptr);

          DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                        "ALWAYS_SHARE_PROFILE requested when no PDNs up, "
                        "Selected profile %d for first call bringup",
                        pending_id);

          /*-----------------------------------------------------------------
           No profile can be used for ALWAYS_SHARE_PROFILE, return FALSE
          -----------------------------------------------------------------*/
          if ( pending_id == DATA_SESSION_PROFILE_ID_INVALID )
          {
            return FALSE;
          }

          policy_info_ptr->data_session_profile_id = pending_id;
        }
        else
        {
          return TRUE;
        }
      }

      if( FALSE == 
             ds707_data_session_get_profile(
                  pending_id,
                  (ds_profile_subs_id_enum_type)(policy_info_ptr->subs_id), 
                  &data_sess_profile_ptr) )
      {
        return FALSE;
      }
      policy_subs_id = (ds_sys_subscription_enum_type)policy_info_ptr->subs_id;
      if( ds3g_resolve_subs_id(policy_subs_id, &resolved_subs_id) == FALSE )
      {
        DATA_IS707_MSG2(MSG_LEGACY_ERROR,"Error in obataining resolved sub id %d "
                     "Policy Sub ID %d",policy_info_ptr->subs_id,resolved_subs_id);
        return FALSE;
      }
      return ds707_is_apn_pref_pkt_mode_ex(data_sess_profile_ptr,
                                           mode,resolved_subs_id);
    }
  }
} /* ds707_is_pref_pkt_mode */

/*===========================================================================
FUNCTION DS707_RT_IS_CALL_ALLOWED

DESCRIPTION
  This is a wrapper function that checks various items to ensure if a
  call can proceed on this iface or not
  
PARAMETERS  
  profile_id         - 3GPP2 profile ID for this PDN
  if_ptr             - Interface
  policy_info_ptr    - Policy Information
  deny_reason        - Reason for deny
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE : If call is allowed
  FALSE: Otherwise
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds707_rt_is_call_allowed
(
  ds707_data_session_profile_id  pending_id,
  ps_iface_type                  *if_ptr,
  acl_policy_info_type           *policy_info_ptr,
  int                            *deny_reason
)
{
  ds707_data_session_profile_id profile_reg_index = -1;
  ps_iface_net_down_reason_type call_end_reason = 
                                          PS_NET_DOWN_REASON_INTERNAL_ERROR;
  ds707_data_session_profile_info_type *data_sess_prof_ptr = NULL;
  ps_iface_addr_family_type             resolve_ip_type = IPV4_ADDR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------
    Check current thermal mitigation action and deny call if needed. 
  -------------------------------------------------------------------------*/

  if( FALSE == ds707_tm_mgr_is_call_allowed(policy_info_ptr->apn_type) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Thermal mitigation in progress."
                                     " call not allowed");
    *deny_reason = ACL_DENY_REASON_THERMAL_MITIGATION;
    return FALSE;
  }

  /*------------------------------------------------------------------------
    check if the data_session_id set by the aplication is valid or not. 
  -------------------------------------------------------------------------*/
  profile_reg_index = ds707_data_session_is_valid_profile(pending_id);
  
  if( profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,"Invalid Profile ID %d",pending_id);
    *deny_reason = ACL_DENY_REASON_INVALID_PROFILE;
    return FALSE;
  }

  /*------------------------------------------------------------------------
    check for IP version match between the iface and the Profile IP type
  -------------------------------------------------------------------------*/
  if( ds707_data_session_ip_version_match( pending_id, 
                                           policy_info_ptr, 
                                           if_ptr) == FALSE )
  {
    *deny_reason = ACL_DENY_REASON_IP_VERSION_MISMATCH;
    return FALSE;
  }

  if( ds707_data_session_subs_id_match( pending_id,
                                        policy_info_ptr) == FALSE )
  {
    *deny_reason = ACL_DENY_REASON_SUBS_ID_MISMATCH;
    return FALSE;
  }

  /*------------------------------------------------------------------------
    If session is not open, no more check necessary. Return TRUE
  -------------------------------------------------------------------------*/
  if( ds707_sys_chg_hdlr_is_sess_open() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"HDR session not open");
    return TRUE;
  }

#ifdef FEATURE_EPC_HANDOFF
  /*------------------------------------------------------------------------
    If policy handoff class is EPC, no more check necessary. Return TRUE
  -------------------------------------------------------------------------*/
  if( ds707_is_policy_handoff_class_epc(policy_info_ptr) == TRUE )
  {
    /* print msg not added since above func itself prints the detail */
    return TRUE;
  }
#endif /* FEATURE_EPC_HANDOFF */

  /*------------------------------------------------------------------------
    Check if call is allowed with the provided details
  -------------------------------------------------------------------------*/
  if ( FALSE == ds707_data_session_get_profile(
                  pending_id, 
                  (ds_profile_subs_id_enum_type)(policy_info_ptr->subs_id),
                  &data_sess_prof_ptr) )
  {
    *deny_reason = ACL_DENY_REASON_INVALID_PROFILE;
    return FALSE;
  }

  resolve_ip_type = 
      ds707_data_session_resolve_iface_addr_family(pending_id, policy_info_ptr);
  /*-------------------------------------------------------------------------
   If current iface is throttled, no call is allowed on this iface. 
   DENY routing.  
  -------------------------------------------------------------------------*/
  if ( ds707_pdncntx_is_iface_throttled(data_sess_prof_ptr, resolve_ip_type) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "PDN is throttled. NO call allowed. Return FALSE. ");
    *deny_reason = 
    ds707_pdncntx_throttling_set_acl_deny_reason(data_sess_prof_ptr, resolve_ip_type);
    return FALSE;
  }

   /*------------------------------------------------------------------------
    If IP FAMILY is IPV6 and IPV6 NV(1896) is disabled, deny routing 
    for 1X/HRPD and eHRPD
   -------------------------------------------------------------------------*/
  if ( (FALSE == ds3g_is_ipv6_enabled()) && 
       (policy_info_ptr->ip_family == IFACE_IPV6_ADDR_FAMILY) ) 
  {
    *deny_reason = ACL_DENY_REASON_V6_DISABLED; 
    return FALSE;
  }

  /*------------------------------------------------------------------------
    If IP FAMILY is IPV6 and MIPV6 NV is enabled, deny routing
  -------------------------------------------------------------------------*/

  if ( (TRUE == ds707_nv_util_get_mipv6_nv()) && 
       (policy_info_ptr->ip_family == IFACE_IPV6_ADDR_FAMILY) ) 
  {
    *deny_reason = ACL_DENY_REASON_MIP_V6_DISALLOWED; 
    return FALSE;
  }

  if( ds707_data_session_call_allowed( data_sess_prof_ptr, 
                                       &call_end_reason) == TRUE )
  {
    /* print msg not added since above func itself prints the detail */
    return TRUE;
  }
  else
  {
    switch(call_end_reason)
    {
      case PS_NET_DOWN_REASON_MANDATORY_APN_DISABLED:
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_data_session_call_allowed check failed with deny "
                    "reason: ACL_DENY_REASON_MANDATORY_APN_DISABLED"); 
        *deny_reason = ACL_DENY_REASON_MANDATORY_APN_DISABLED;
        break;

      case PS_NET_DOWN_REASON_APN_DISABLED:
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_data_session_call_allowed check failed with deny "
                    "reason: ACL_DENY_REASON_APN_DISABLED"); 
        *deny_reason = ACL_DENY_REASON_APN_DISABLED;
        break;

      case PS_NET_DOWN_REASON_UNPREFERRED_RAT:
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_data_session_call_allowed check failed with deny "
                    "reason: ACL_DENY_REASON_UNPREFERRED_RAT"); 
        *deny_reason = ACL_DENY_REASON_UNPREFERRED_RAT;
        break;

      default:
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "No Specific end reason provided. Mapping to "
                        "ACL_DENY_REASON_POL_NO_MATCH "); 
        *deny_reason = ACL_DENY_REASON_POL_NO_MATCH; 
        break;
    }
    return FALSE;
  }

} /* ds707_rt_is_call_allowed */

/*===========================================================================
FUNCTION DS707_RT_IS_CLAT_SUPPORTED

DESCRIPTION
  This function returns the CLAT_SUPPORTED field for the passed profile or
  resolved profile passed in Policy

PARAMETERS
  arg_val_ptr - IOCTL payload pointer
  subs_id     - subscription id
  ps_error    - incase any error occured, details of it

DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
       DS_EFAULT   if NULL pointers passed.
       DS_ENETDOWN if PDN contexts are not valid.

SIDE EFFECTS
  None.
===========================================================================*/
int ds707_rt_is_clat_supported
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
)
{
  ps_sys_ioctl_clat_support_type  *clat_supp_ptr = NULL;
  acl_policy_info_type            *policy_info_ptr = NULL;
  ds707_data_session_profile_id    policy_prof_id = 0;
  sys_modem_as_id_e_type           resolved_subs_id = SYS_MODEM_AS_ID_NONE;
  boolean                          is_clat_enabled = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ps_errno == NULL )
  {
    return -1;
  }

  if (arg_val_ptr == NULL)
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  clat_supp_ptr   = (ps_sys_ioctl_clat_support_type*)arg_val_ptr;
  policy_info_ptr = clat_supp_ptr->acl_policy_info_ptr;

  /*---------------------------------------------------------------------
    Fail IOCTL if current preferred mode per APN is not 3GPP2
  ---------------------------------------------------------------------*/
  if( (!(ds707_is_pref_pkt_mode(policy_info_ptr, SYS_SYS_MODE_CDMA))) &&
      (!(ds707_is_pref_pkt_mode(policy_info_ptr, SYS_SYS_MODE_HDR))) )
  {
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get Profile ID from policy
  -------------------------------------------------------------------------*/
  policy_prof_id = policy_info_ptr->data_session_profile_id;

  /*-------------------------------------------------------------------------
    Get the resolved subs id
  -------------------------------------------------------------------------*/
  if( ds3g_resolve_subs_id(policy_info_ptr->subs_id ,&resolved_subs_id ) 
        == FALSE) 
  {
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Resolve if profile id is Always_Share_Profile
  -------------------------------------------------------------------------*/
  if ( policy_prof_id  == DATA_SESSION_ALWAYS_SHARE_PROFILE )
  {
    policy_prof_id 
      = ds707_data_session_get_profile_for_always_share_profile(policy_info_ptr);
  }

  /*-------------------------------------------------------------------------
    Get CLAT value from the profile id
  -------------------------------------------------------------------------*/
  if( TRUE != ds707_data_session_is_clat_enabled(
                               policy_prof_id,
                               ds707_data_session_get_profile_subs_id(resolved_subs_id),
                               &is_clat_enabled) )
  {
    *ps_errno = DS_EINVAL;
    return -1;
  }

  clat_supp_ptr->is_clat_supported = is_clat_enabled;
  *ps_errno = DS_ENOERR;
  return 0;
} /* ds707_rt_is_clat_supported() */

/***  IS707 ANY ACL ***/
/*lint -save -e{57,58,506,641,655,774}*/
ACL_DEF( ds707_any_acl )
ACL_START
  PASS_QUICK( ACL_DEFAULT_CLASS, IS_POL_INFO_NULL() );
  DENY( DIFFERENT_IFACE_NAME_REQUIRED() );
  DENY( DIFFERENT_IFACE_ID_REQUIRED() );
  DENY_WITH_ANY_REASON( IFACE_NOT_AVAILABLE2(ds707_rt_is_iface_available) );
  DENY( IFACE_UP_REQUIRED_BUT_NOT_UP() );
  DENY ( NOT(IP_POL_FAMILY_IS(IFACE_ANY_ADDR_FAMILY)) );
  DENY( NOT( IS_PKT_INFO_NULL() OR IFACE_AND_PKT_FAMILY_MATCH() ) );
  PASS( ACL_IFNAME_CLASS, REQUIRED_IFACE_TYPE_IS( CDMA_SN_IFACE ) );
  PASS( ACL_PREF_MODE_CLASS,
        IFACE_IS_PREF_MODE( ds3g_is_pref_pkt_mode, SYS_SYS_MODE_CDMA) );
  DENY_WITH_ANY_REASON( SPECIFIED_CONFIG_NOT_SUPPORTED2
  	                (ds707_rt_is_cfg_supported) );
  PASS( ACL_CONFIG_CLASS,
        SPECIFIED_CONFIG_IS_SHAREABLE(ds707_rt_is_cfg_shareable) );
  PASS( ACL_UP_PREF_CLASS, IFACE_UP_PREFERRED_AND_UP() );
  PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_TYPE_IS( WWAN_GROUP ) );
  PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_TYPE_IS( ANY_DEFAULT_GROUP ) );
  PASS( ACL_DEFAULT_CLASS, ALL );
ACL_END
  /*lint -restore */
#endif /* FEATURE_DATA_IS707 */
