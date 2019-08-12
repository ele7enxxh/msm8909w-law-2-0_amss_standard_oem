/*===========================================================================

                        D S 7 0 7 _ P K T _ M G R _ I O C T L . C
GENERAL DESCRIPTION
  This file contains functionality for ioctls specific to 1x.

EXTERNALIZED FUNCTIONS
  ds707_pkt_ioctl()
    Handles general ioctls on 1x iface.

  ds707_pkt_phys_link_ioctl()
    Handles ioctls related to phys links. 

  ds707_pkt_ps_flow_ioctl()
    Handles ioctls related to ps flows.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_ioctl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/14    sd     Fix to correct return value of ds707_sys_ioctl() for
                   invalid sus_id.
08/05/14    ms     DSDA Phase 2 changes.
06/26/14    ms     Data+MMS - DSDA changes Phase 1.
01/06/14    vm     Changes to use correct PCMT value during IRAT success and
                   failure
06/03/13    sd     ATCoP decoupling changes
05/03/13    sc     Dont call ps flow go null cmd on a sec flow which is already in NULL state.
11/08/12    ssb    Dun Call Entitlement support using PS_SYS Interface
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA
01/02/12    msh    Coan: Feature cleanup
10/28/11    ls     PDN throttle info notification through SYS IOCTL
10/03/11    ash    Added new ioctl support PS_IFACE_IOCTL_GET_IPV6_REMAINING_RTR_LIFETIME
11/18/11    vpk    HDR SU API cleanup
10/03/11    ash    Added new ioctl support 
                               PS_IFACE_IOCTL_GET_IPV6_REMAINING_RTR_LIFETIME
06/22/11    bd     Changed slotted mode pointer to 
                   ps_iface_ioctl_enable_hdr_slotted_mode. Migrating CL 1674460
05/13/11    dvk    Merged SYS_SYS_MODE_HDR check in Get Data Call Type IOCTL 
                   handler.
                   Merged support to get data call status for SIP/MIP.
04/08/11    dvk    Fix SDB Support Query functionality.
04/07/11    ms     Global variables cleanup.
03/15/11    mg     Global variable cleanup
10/13/10    op     Migrated to MSG 2.0 macros
08/30/10    ls     Fixed Featurization issue
05/14/10    ls     Start HDR/CDMA timer handler only when there is a HDR/CDMA session
                   and phy link is up
04/16/10    ls     Fix the featurization issues
04/14/10    gc     Added featurization to disable DO if necessary
03/30/10    ssh    Added support for ioctls - get call type, data count, set
                   hdr an user id.
03/04/10    spn    Updated the Data Bearer Indications
02/15/09    ls     Initialize epzid in ds3g_pkt_call_info_type
02/13/09    ss     Off target Lint Fixes
12/18/08    ss     Lint Fixes
11/26/08    psng   Fixed featurization issue.
11/10/08    yz     Fixed RX/TX byte count issue.
09/28/08    ms     Fixed compiler warning
09/02/08    ms     IS707B addendum Feature support.
07/07/08    ms     Klocwork Fixes
04/15/08    sc     Changes for MIPv6 bootstrap IOCTL changes
04/14/08    sn     Fixed featurization issue.
10/11/07    ac     Propagate the default CTA timer value to RLP to take 
                   care of multi-mode targets
08/20/07    sk     Enhanced the QoS resource validation function to account 
                   TX watermark and DS flow.
07/11/07    sc     Changed pkt_instance to always use DEFAULT_PKT_INSTANCE in 
                   ds707_pkt_mgr_iface_ioctl_qos_request() and
                   ds707_pkt_mgr_iface_ioctl_qos_request_ex()
05/08/07    as     ds707_pkt_mgr_iface_ioctl_enable_hdr_slotted_mode modified
                   for QCHAT.
05/01/07    sk     QoS Modify support
03/28/07    sk     Added support to handle RF condition
03/08/07    ara    Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X 
03/07/07    sk     Removed PS FLOW RESTORE functionality
03/05/07    sk     Added support for ANSupported Profile change
01/24/07    sy     Added MARQ support for GET_TX_STATUS_IOCTL.
12/21/06    spn    Implementation of the GET_TX_STATUS IOCTL.
11/14/06    rsj    Added changes for JCDMA Session Timer. 
                   Modified ds707_pkt_ioctl to handle 
                   PS_IFACE_IOCTL_SET_SESSION_TIMER
                   and PS_IFACE_IOCTL_GET_SESSION_TIMER. Also created the 
                   two new IOCTLS to control Session Timer : 
                   ds707_pkt_mgr_iface_ioctl_set_session_timer
11/28/06    as     changed return value of GO_ACTIVE and GO_DORMANT ioctl to
                   return success by default.
11/17/06    spn    Merged EIDLE IOCTL Changes
10/19/06    rsj    Added JCDMA 2 low battery event processing. Modified 
                   ds707_pkti_call_ended_hdlr to look at primary tc state
                   and send ORDQ value to CM.
10/23/06    spn    Fail the GET_TX_QUEUE_LEVEL_IOCTL if iface is down, or the 
                   flow is suspended
19/10/06    rsj    Added JCDMA 2 low battery event processing. Modified 
                   ds707_pkt_mgr_iface_ioctl_go_null so that ORDQ
                   value can be passed to ds707_pkti_call_ended_hdlr.
10/05/06    spn    Fixed the GET TX Queue Level IOCTL, to calculate using the
                   ds_flow_ptr's tx_wm_list
09/29/06    sk     Fail RESUME and SUSPEND IOCTL if set_id is 0 and return 
                   correct info code to app.
09/19/06    sk     Fail get_network_supported_qos_profiles IOCTL if QoS is 
                   not available. Also return DS_EQOSUNAWARE to apps.
09/19/06    sa     Added additional error codes for EIDLE IOCTL
09/12/06    swk    Fixed QOS featurization issue
08/28/06    spn    Return error properly while processing the IOCTL
08/15/06    spn    Re-Architected the file to be more modular, no functional 
                   change
07/06/06    sk     QoS 2.0 Support
05/23/06    spn    Moved the GET_NETWORK_SUPPORTED_PROFILES from flow_ioctl
                   to pkt_ioctl, as it is for an iface and not for a flow.
05/04/06    ksu    EMPA: RSP part 2
04/26/06    spn    GET_TX_QUEUE_LEVEL IOCTL could return an invalid number
                   if the subtrahend is greater than the minuend in the 
                   operation performed to get the wm_free_cnt
04/16/06    squ/gr When phys_link_ioctl_go_active is called, if the phys link
                   flow is disabled, the phys link will not be brought up.
03/30/06    spn    Fixed Featurization for FEATURE_HDR_QOS
03/29/06    spn    Changed the IOCTL errors to DS_EQOSUNAWARE if any of the 
                   QoS IOCTLS are failed if QoS is unavailable
03/19/06    spn    Get_TX_QUEUE_LEVEL IOCTL was not working properly for 
                   Rev-0 cases, put in a temporary fix. 
02/23/06    spn    Added a new IOCTL to get the Network (AN) supported QoS
                   profiles.
02/13/06    spn    Extended the DSS_IFACE_IOCTL_GET_TX_QUEUE_LEVEL for 
                   secondary flows.
01/26/06    ksu    EMPA: don't bind secondary flows to primary phys link
12/01/05    mct    Added support for ON_QOS_AWARE_SYSTEM ioctl.
11/19/05    sk     Added handoff functionality
11/07/05   vrk     Call ps_iface_tear_down_cmd multiple times to tear down 
                   ppp if multiple apps are running
10/23/05    sk     Added API bundling support
10/19/05    spn    Added the check for Availability of QoS before we proceed
                   with ioctl request.
09/20/05    mct    Changed qos passed in ioctl to ptr from structure.
09/13/05    vas    Added code for handling Rate params/HDR mode IOCTLs.
08/05/05   sk/vs   Merged the HDR Semi-Dynamic QOS changes from sandbox.
07/29/05    vas    Featurization fix.
07/26/05    vas    Added support for DSS_IFACE_IOCTL_GET_TX_QUEUE_LEVEL
                   Unfeaturized ps_flow_ioctl processing function
05/25/05    atp    Added support for semi-dynamic QOS on DO.
05/23/05    spn    Changed signature of ds707_pkt_ioctl
05/16/05    ks     made temp fixes for HDR_QOS to compile with PS.10.00.00 VU
04/26/05    gr     Added inc_call flag to ds3g_pkt_call_info_type 
03/29/05    gr     Moved the GO_ACTIVE and GO_DORMANT ioctls to phys link
                   ioctl handler since these are operations on phys link
11/22/04    vas    Removed processing of QOS Release. Moved it to phys link 
                   ioctl handler, since this IOCTL is for phys link, not Iface
09/08/04    vas    Added handling of QOS Request/Release Ioctl
09/07/04    sv     Removed DSS_IFACE_IOCTL dependency.
07/12/04    gr     Added IOCTLs for getting and setting the RLP
                   fill rate
06/30/04    gr     Modified GO_NULL IOCTL to use phys_link_gone_ind
04/27/04    kvd    Added calls to set_ipcp_dns_opt in processing
                   DSS_IFACE_IOCTL_IPCP_DNS_OPT
04/11/04    gr     Set the value of force_cdma_only to FALSE for all
                   dormancy re-origination scenarios
03/19/04    ak     Cannot set DSN via ioctl.
01/04/04    ak     Moved ioctl function into this file.
11/11/03    sv     Added ioctl for disabling dns negotiation during ipcp
                   configuration.

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#ifdef FEATURE_DATA_IS707
  #define FEATURE_DSM_WM_CB
  #include "amssassert.h"
  #include "ds707.h"
  #include "ds707_pkt_mgr.h"
  #include "ds707_pkt_mgr_ioctl.h"
  #include "ds707_cta_rlp.h"
  #include "ds707_pkt_mgr_hdlr.h"
  #include "ds707_pkt_mgri.h"
  #include "ds707_so_async.h"
  #include "ds707_drs.h"
  #include "dssocket.h"
  #include "ds_flow_control.h"
  #include "dsat707extctab.h"
  #include "dsat707vend.h"
  #include "dsat707vendctab.h"
  #include "dserrno.h"
  #include "dsrlp_v.h"
  #include "dsrsp.h"
  #include "dss_iface_ioctl.h"
  #include "err.h"
  #include "msg.h"
  #include "data_msg.h"
  #include "ps_iface.h"
  #include "ps_phys_link.h"
  #include "errno.h"
  #include "ds707_pdn_context.h"
  #include "ds707_data_session_profile.h"
#ifdef FEATURE_EHRPD
  #include "ps_ppp_ncp.h"
#endif
#ifdef FEATURE_IS707B_ADDENDUM
  #include "ds707_epzid_util.h"
  #include "ds707_epzid_hyst.h"
#endif 

  #ifdef FEATURE_DATA_IS2000_SDB
    #include "ds707_sdb.h"
    #include "mccdma.h"
  #endif /* FEATURE_DATA_IS2000_SDB */

#ifdef FEATURE_HDR_QOS
#include "ds707_sec_pkt_mgr.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "ps_qos_defs.h"
#include "ps_flow.h"
#include "hdrmrlpcif.h"
#include "ds707_qos_flow_mgr.h" 
#include "hdrcom_api.h"
#include "ds707_sec_pkt_mgr_handoff.h"
#include "ds_1x_stats.h"
#include "hdrpacdsif.h"
#include "hdrpac_mdm.h"
#include "ds707_sec_pkt_mgr_modify.h"
#include "ps_crit_sect.h"
#endif /* FEATURE_HDR_QOS */

  #ifdef FEATURE_HDR
    #include "hdrrlp.h"
    #include "hdrmrlp.h"
    #include "hdrpac.h"
  #endif

#include "ds_1x_stats.h"
#include "ds707_gonulltmr.h"
#include "ds707_wmk.h"
#include "dshdr_an_wmk.h"
#include "ds707_roaming.h"
#include "data_err_suppr.h"

#ifdef FEATURE_DATA_PS_MIPV6
  #include "dshdr_dhcp_mgr.h"
#endif

#ifdef FEATURE_DS_RDUD
  #include "dsrdud.h"
#endif 

#include "dshdr_an_mgr.h"
#include "ds3gmgr.h"
#include "ds_1x_profile.h"
#include "ps_sys_ioctl.h"
#include "ds3gtimer.h"
#include "ds707_devmgr.h"

#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_DATA_WLAN_MAPCON
  #include "ds_sys.h"
#endif

#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101_defs.h"
#endif /* FEATURE_DATA_OPTHO */

extern int ds707_rt_is_clat_supported
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
);

/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
                            VARIABLES
===========================================================================*/
static boolean ds707_pkt_mgr_handoff_1x_handdown;

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                       FLOW IOCTL HANDLER FUNCTIONS 
===========================================================================*/

#ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION      DS707_PKT_MGR_FLOW_IOCTL_DELETE_FLOWS

DESCRIPTION   This function deletes all the flows

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_flow_ioctl_delete_flows
(
  ps_flow_type  ** ps_flow, 
  uint8            num_of_specs,
  ps_iface_type   *iface_ptr
)
{
  int16           flow_errorno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  while (num_of_specs)
  {
    /*----------------------------------------------------------------
      The flow has to be in NULL state to delete the flow. that is why 
      we need to post go null ind
      ----------------------------------------------------------------*/
    ps_flow_go_null_ind( ps_flow[num_of_specs-1], 
                         PS_EIC_NOT_SPECIFIED );

    if ( ps_iface_delete_flow( iface_ptr, 
                               ps_flow[num_of_specs-1],
                               &flow_errorno ) !=0)
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                       "Couldnt Delete PS FLow 0x%x",
                       ps_flow[num_of_specs-1]);
      /*----------------------------------------------------------------
      This is ASSERT condition. There is no recovery if the flow can not 
      be deleted
      ----------------------------------------------------------------*/
      ASSERT(0);
    }
    ps_flow[num_of_specs-1] = NULL;
    num_of_specs--;
  }
} /* ds707_pkt_mgr_flow_ioctl_delete_flows() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_FLOW_IOCTL_RESOURCE_VALIDATE

DESCRIPTION   This function computes the availability of resource
              in the system to support the requested ioctl

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkt_mgr_flow_ioctl_resource_validate
(
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr
)
{
  ps_iface_ioctl_qos_request_ex_type * qos_ioctl_arg_ex;
  ps_iface_ioctl_qos_request_type   *qos_ioctl_arg ;
  qos_spec_type * qos_spec_ptr;
  uint8 fwd_requested_resv_total =0;
  uint8 rev_requested_resv_total =0;
  uint8 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  switch (ioctl_name)
  {
    case PS_IFACE_IOCTL_QOS_REQUEST_EX:
      qos_ioctl_arg_ex = (ps_iface_ioctl_qos_request_ex_type *) argval_ptr;
      for (i = 0; i< qos_ioctl_arg_ex->num_qos_specs; i++)
      {
        qos_spec_ptr = 
                  (qos_spec_type*) ((qos_ioctl_arg_ex->qos_specs_ptr) + i);

        if (qos_spec_ptr->field_mask  & (uint16)QOS_MASK_RX_FLOW)
        {
          fwd_requested_resv_total++;
        }
        if (qos_spec_ptr->field_mask  & (uint16)QOS_MASK_TX_FLOW)
        {
          rev_requested_resv_total++;
        }
      }
      break;

    case PS_IFACE_IOCTL_QOS_REQUEST:
      qos_ioctl_arg = ( ps_iface_ioctl_qos_request_type *) argval_ptr;
      qos_spec_ptr = qos_ioctl_arg->qos_ptr;
      
      if (qos_spec_ptr->field_mask & (int16)QOS_MASK_RX_FLOW)
      {
        fwd_requested_resv_total++;
      }

      if (qos_spec_ptr->field_mask  & (int16)QOS_MASK_TX_FLOW)
      {
        rev_requested_resv_total++; 
      }

      break;

    default:
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                       "Invalid ioctl_name: 0x%d",
                       ioctl_name);
      break;
  }

  return ds707_sec_pkt_mgr_resource_validate(fwd_requested_resv_total,
                   rev_requested_resv_total);
}/* ds707_pkt_mgr_flow_ioctl_resource_validate()*/

/*===========================================================================

FUNCTION      DS707_PKT_MGR_FLOW_IOCTL_CREATE_FLOWS

DESCRIPTION   This function is called to create a new ps_flow given a QOS
              specification.

DEPENDENCIES  None.

RETURN VALUE  TRUE - if we were able to create flows successfully
              FALSE - if we cannot create the flows

SIDE EFFECTS  None.

===========================================================================*/
LOCAL boolean ds707_pkt_mgr_flow_ioctl_create_flows
(
  ps_iface_ioctl_qos_request_ex_type * qos_ioctl_arg_ex,
  ps_iface_type            *iface_ptr
)
{
  uint8             i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(qos_ioctl_arg_ex);
  ASSERT(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(qos_ioctl_arg_ex);

  if (qos_ioctl_arg_ex->num_qos_specs > MAX_QOS_OPERATIONS_PER_IOCTL )
  {
    /*----------------------------------------------------------------
    This check should not crash. This function being the first function 
    to parse the number of qos specs, we just return FALSE. Based on
    the return boolean, the caller has chance to fail to IOCTL
    ----------------------------------------------------------------*/
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "Num QoS specs :%d  not supported", 
                    qos_ioctl_arg_ex->num_qos_specs);
    return FALSE;
  }

  for (i = 0; i< qos_ioctl_arg_ex->num_qos_specs; i++)
  {
    qos_ioctl_arg_ex->flows_ptr[i]   = 
                 ds707_sec_pkt_mgr_create_flow(
                    (qos_spec_type*) ((qos_ioctl_arg_ex->qos_specs_ptr) + i),
                    qos_ioctl_arg_ex->subset_id,
                    iface_ptr,
                    FALSE,
		    qos_ioctl_arg_ex->opcode
                    );
    if (qos_ioctl_arg_ex->flows_ptr[i] == NULL)
    {
      ds707_pkt_mgr_flow_ioctl_delete_flows( qos_ioctl_arg_ex->flows_ptr, 
                                             i,
                                             iface_ptr );
      return FALSE;
    }
  }
  return TRUE; 
} /* ds707_pkt_mgr_flow_ioctl_create_flows() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_FLOWS_CONFIGURE_CMD()

DESCRIPTION    Configures a flow
  
RETURN VALUE   None.

DEPENDENCIES   If function fails, caller is responsible for cleaning up this ps_flow
               THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS   Changes flow's state to FLOW_CONFIGURING
===========================================================================*/
LOCAL void ds707_pkt_mgr_flow_ioctl_flows_configure_cmd 
(
  ps_iface_type  * iface_ptr,
  ps_flow_type  ** ps_flow, 
  uint8            num_of_specs,
  int16          * ioctl_errno,
  void           * client_ptr
)
{
  uint8 i;
  int ret_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(ps_flow);
  ASSERT(ioctl_errno);
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(ps_flow);
  DATA_MDM_ARG_NOT_CONST(client_ptr);

  /*----------------------------------------------------------------
  This is internal function and should never be called with 
  num_of_specs > MAX_QOS_OPERATIONS_PER_IOCTL. If this is the case, 
  it is memory scribble or wrong usage of the internal function
  ----------------------------------------------------------------*/
  ASSERT(num_of_specs <=MAX_QOS_OPERATIONS_PER_IOCTL);

  /*----------------------------------------------------------------------
  This value will be overwritten when entering the for loop. If we don't
  enter the for loop because the num_qos_specs=0, then this is a bad 
  arguements 
  --------------------------------------------------------------------*/
  *ioctl_errno = DS_EBADF;  

  for (i = 0; i <num_of_specs; i++)
  {
    /*----------------------------------------------------------------------
    All the flows should be in NULL state. If the flows are not NULL state,
    there is no recovery. So, ASSERT this condition
    --------------------------------------------------------------------*/
    if (PS_FLOW_GET_STATE(ps_flow[i]) != FLOW_NULL)
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Flow :0x%x is in %d state, not in NULL STATE",
                       ps_flow[i],
                       PS_FLOW_GET_STATE(ps_flow[i]));
      ASSERT(0);
    }

    /*----------------------------------------------------------------------
    Activate this flow. 
    --------------------------------------------------------------------*/
    ret_val =  ps_flow_configure_cmd (ps_flow[i], 
                                      ioctl_errno,
                                      NULL );

    /*----------------------------------------------------------------------
    This is ASSERT condition
    --------------------------------------------------------------------*/
    ASSERT((ret_val == -1) && (*ioctl_errno == DS_EWOULDBLOCK));
  }
} /* ds707_pkt_mgr_flow_ioctl_flows_configure_cmd()*/

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_FLOWS_ACTIVATE_CMD()

DESCRIPTION   Activates/Resumes a flow

RETURN VALUE  None

DEPENDIENCIES If function fails, caller is responsible for cleaning up 
              this ps_flow
              THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS  Changes flow's state to FLOW_ACTIVATING and posts 
              FLOW_ACTIVATING_EV if flow's state is FLOW_NULL, else state is 
              changed to FLOW_RESUMING 
===========================================================================*/
LOCAL void ds707_pkt_mgr_flow_ioctl_flows_activate_cmd 
(
  ps_iface_type  * iface_ptr,
  ps_flow_type  ** ps_flow, 
  uint8            num_of_specs,
  int16          * ioctl_errno,
  void           * client_ptr
)
{
  uint8 i;
  int ret_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(ps_flow);
  DATA_MDM_ARG_NOT_CONST(client_ptr);
  ASSERT(ps_flow);
  ASSERT(ioctl_errno);
  /*----------------------------------------------------------------
  This is internal function and should never be called with 
  num_of_specs > MAX_QOS_OPERATIONS_PER_IOCTL. If this is the case, 
  it is memory scribble or wrong usage of the internal function
  ----------------------------------------------------------------*/
  ASSERT(num_of_specs <=MAX_QOS_OPERATIONS_PER_IOCTL);

  /*----------------------------------------------------------------------
  This value will be overwritten when entering the for loop. If we don't
  enter the for loop because the num_qos_specs=0, then this is a bad 
  arguements 
  --------------------------------------------------------------------*/
  *ioctl_errno = DS_EBADF;  

  for (i = 0; i <num_of_specs; i++)
  {
    /*----------------------------------------------------------------------
    All the flows should be in NULL state. If the flows are not NULL state,
    there is no recovery. So, ASSERT this condition
    --------------------------------------------------------------------*/
    if (PS_FLOW_GET_STATE(ps_flow[i]) != FLOW_NULL)
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Flow :0x%x is in %d state, not in NULL STATE",
                       ps_flow[i],
                       PS_FLOW_GET_STATE(ps_flow[i]));
      ASSERT(0);
      return;
    }

    /*----------------------------------------------------------------------
    Activate this flow. 
    --------------------------------------------------------------------*/
    ret_val =  ps_flow_activate_cmd (ps_flow[i], 
                                     ioctl_errno, 
                                     NULL );

    /*----------------------------------------------------------------------
    This is ASSERT condition
    --------------------------------------------------------------------*/
    ASSERT((ret_val == -1) && (*ioctl_errno == DS_EWOULDBLOCK));
  }
} /* ds707_pkt_mgr_flow_ioctl_flow_activate_cmd() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_FLOWS_RESUME_CMD()

DESCRIPTION   Resumes a flow

RETURN VALUE  None.

DEPENDINCIES  If function fails, caller is responsible for cleaning up 
              this ps_flow
              THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS  Changes flow's state to FLOW_RESUMING
===========================================================================*/
LOCAL void ds707_pkt_mgr_flow_ioctl_flows_resume_cmd 
(
  ps_flow_type ** ps_flow, 
  uint8           num_of_specs,
  int16         * ioctl_errno,
  void          * client_ptr
)
{
  int                      ret_val = -1 ;
  uint8                    i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ps_flow);
  DATA_MDM_ARG_NOT_CONST(client_ptr);
  ASSERT(ps_flow);
  ASSERT(ioctl_errno);
  ASSERT(num_of_specs <=MAX_QOS_OPERATIONS_PER_IOCTL);

  /*------------------------------------------------------------------------
  This value will be overwritten when entering the for loop. If we don't 
  enter the for loop because the num_qos_specs=0, then this is a bad 
  arguements 
  -----------------------------------------------------------------------*/
  *ioctl_errno = DS_EBADF;

  for (i = 0; i <num_of_specs; i++)
  {
    ret_val =  ps_flow_resume_cmd (ps_flow[i], ioctl_errno, NULL );
    ASSERT((ret_val == -1) && (*ioctl_errno == DS_EWOULDBLOCK) ); 
  }
} /* ds707_pkt_mgr_flow_ioctl_flows_resume_cmd() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_FLOWS_GO_NULL_CMD()

DESCRIPTION  Releases a flow
  
RETURN VALUE None.

DEPENDENCIES THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS Changes flow's state to FLOW_GOING_NULL
===========================================================================*/
LOCAL void ds707_pkt_mgr_flow_ioctl_flows_go_null_cmd 
(
  ps_flow_type ** ps_flow, 
  uint8           num_of_specs,
  int16         * ioctl_errno,
  void          * client_ptr
)
{
  int                      ret_val = -1 ;
  uint8                    i;
  int16                    flow_errorno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ps_flow);
  DATA_MDM_ARG_NOT_CONST(client_ptr);
  DATA_MDM_ARG_NOT_CONST(ioctl_errno);
  ASSERT(ps_flow);
  ASSERT(ioctl_errno);
  ASSERT(num_of_specs <=MAX_QOS_OPERATIONS_PER_IOCTL);

  for (i = 0; i<num_of_specs; i++)
  {
    if(PS_FLOW_GET_STATE(ps_flow[i]) != FLOW_NULL) //Flow could have gone null and then deleted as part of NON-EPC event handling.
    {
      ret_val = ps_flow_go_null_cmd ( ps_flow[i], &flow_errorno, NULL );
      ASSERT( (ret_val == -1) && (flow_errorno == DS_EWOULDBLOCK));
    }
  }
} /* ds707_pkt_mgr_flow_ioctl_flows_go_null_cmd () */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_FLOWS_RESUME_CMD()

DESCRIPTION   Suspends a flow

RETURN VALUE  None.

DEPENDENCIES  If function fails, caller is responsible for cleaning up 
              this ps_flow
              THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS  Changes flow's state to FLOW_SUSPENDING
===========================================================================*/
LOCAL void ds707_pkt_mgr_flow_ioctl_flows_suspend_cmd 
(
  ps_flow_type ** ps_flow, 
  uint8           num_of_specs,
  int16         * ioctl_errno,
  void          * client_ptr
)
{
  int                      ret_val ;
  uint8                    i;
  int16                    flow_errorno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ps_flow);
  DATA_MDM_ARG_NOT_CONST(client_ptr);
  DATA_MDM_ARG_NOT_CONST(ioctl_errno);
  ASSERT(ps_flow);
  ASSERT(ioctl_errno);
  ASSERT(num_of_specs <=MAX_QOS_OPERATIONS_PER_IOCTL);

  for (i = 0; i<num_of_specs; i++)
  {
    ret_val = ps_flow_suspend_cmd (ps_flow[i], &flow_errorno, NULL );
    ASSERT((ret_val == -1) && (flow_errorno == DS_EWOULDBLOCK));
  }
} /* ds707_pkt_mgr_flow_ioctl_flows_suspend_cmd() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_QOS_MODIFY

DESCRIPTION   Handles the IOCTL to modify the QoS

RETURN VALUE  0  : if the IOCTL is processed successfully
             -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  Posts a Command to DS task to complete the release
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_qos_modify
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  ds707_flow_type               * ds_flow_ptr;
  ds_cmd_type                   * cmd_ptr = NULL;          
  ps_flow_ioctl_qos_modify_type * qos_ioctl_arg;
  ds707_sec_pkt_flow_cmd_type   * sec_flow_cmd_ptr = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  if(ps_errno == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_errno");
    return (-1);
  }
  
  if(argval_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid parameter argval_ptr");
    *ps_errno = DS_EFAULT;
    return (-1);
  }
  
  if(!(PS_FLOW_IS_VALID(ps_flow_ptr)))
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Invalid Flow 0x%x", ps_flow_ptr);
    *ps_errno = DS_EFAULT;
    return (-1);
  }
  
#ifdef FEATURE_NIQ_EHRPD
  /*-------------------------------------------------------------------
  Check if the flow is network initiated QOS flow, if yes, it is not 
  allowed to modify by the ioctl. 
  --------------------------------------------------------------------*/
  if( ds707_sec_pkt_mgr_is_nw_init_flow(ps_flow_ptr) == TRUE )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Modify ioctl is not allowed for network initiated "
                    "QOS flow 0x%x",
                    ps_flow_ptr);
    *ps_errno = DS_EOPNOTSUPP;
    return (-1);
  }
#endif /*FEATURE_NIQ_EHRPD*/

  /*-------------------------------------------------------------------
  If the QoS is not available, fail the IOCTL
  --------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Current System Does not support QoS");
    *ps_errno = DS_EQOSUNAWARE;
    return (-1);
  }

  /*-----------------------------------------------------------------------
  If the flow state is GOING NULL or NULL, ignore the command
  -----------------------------------------------------------------------*/
  if( (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_NULL) ||
      (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_GOING_NULL)||
      (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_CONFIGURING)||
      (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_ACTIVATING)||
      (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_STATE_INVALID))
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "Flow 0x%p on 0x%x state, modify not allowed", 
                     ps_flow_ptr,
                     PS_FLOW_GET_STATE(ps_flow_ptr));
    *ps_errno = DS_EFAULT;
    return (-1);
  }

  /*-----------------------------------------------------------------------
  Check for back to back modify command. DO not allow for back to back
  mdify command from application
  -----------------------------------------------------------------------*/
  if(ds707_sec_pkt_mgr_is_modify_in_progress(
                 (ds707_flow_type *) ps_flow_ptr->client_data_ptr) == TRUE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
               "Flow 0x%x is being modified. Back to back modify not allowed",
               ps_flow_ptr);
    *ps_errno = DS_EFAULT;
    return (-1);
  }
  
  qos_ioctl_arg = ( ps_flow_ioctl_qos_modify_type *) argval_ptr;
  if(ds707_sec_pkt_mgr_modify_flow(
                                    qos_ioctl_arg->qos_ptr,   
                                            /* Qos specification         */
                                    qos_ioctl_arg->subset_id, 
                                            /* ID assigned to filter spec*/
                                    ds707_pkt_get_iface_ptr_from_ps_flow(
                                                               ps_flow_ptr),
                                    ps_flow_ptr) == FALSE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Flow 0x%p could not be modified",
                     ps_flow_ptr);
    *ps_errno = DS_EFAULT;
    return (-1);
  }

  /*-----------------------------------------------------------------
    Sanity check, if app is deleting TX flow spec. This operation
    is not allowed and is documented. Fail the IOCTL
  ------------------------------------------------------------------*/
  /*lint -save -e641 */
  if( (PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_DELETED(ps_flow_ptr)) ||
      (PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_DELETED(ps_flow_ptr)))
  {
     DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                     "Not allowed to delete the TX/RX spec for flow 0x%x",
                     ps_flow_ptr);
     /*-----------------------------------------------------------------
      Call ps flow rejected to clear the resources committed above
     ------------------------------------------------------------------*/
      (void) ps_iface_modify_flow_rejected(
                          ds707_pkt_get_iface_ptr_from_ps_flow(ps_flow_ptr),
                          ps_flow_ptr,
                          PS_EIC_NOT_SPECIFIED,
                          ps_errno);
      *ps_errno = DS_EFAULT;
      return (-1);
  }
  /*lint -restore */
  
  /*----------------------------------------------------------------------
    Get the command buffer. Make sure that if the command is obtained
    successfully, it must post the command. 
  
      **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
  ----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------
  Set the flag of the DS flow to TRUE. This will prevent from back
  to back modification
  ------------------------------------------------------------------*/
  ds707_sec_pkt_mgr_set_modify_in_progress(TRUE, 
                          (ds707_flow_type *)ps_flow_ptr->client_data_ptr);

  /*----------------------------------------------------------------------
  Post the suspend indication for the flows
  ----------------------------------------------------------------------*/
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "QMOD:Suspending flow:0x%x", ps_flow_ptr);

  //ds707_sec_pkt_flow_suspend_ind(ps_flow_ptr,PS_EIC_QOS_INTERNAL_MODIFY_IN_PROGRESS); 
  ds_flow_ptr = ds707_pkt_mgr_get_ds_flow( ps_flow_ptr );
  if(ds_flow_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "ds_flow_ptr is NULL ");
    *ps_errno = DS_EFAULT;
    return (-1);
  }  	
  if(ds707_sec_pkt_mgr_is_aqos_modify_req(ps_flow_ptr))
  {
      ds_flow_ptr->state.air_link_qos_state = 
                                          DS707_FLOW_AIR_LINK_QOS_SUSPENDED;
  }
  if(ds707_sec_pkt_mgr_is_dqos_modify_req(ps_flow_ptr))
  {
      ds_flow_ptr->state.fltr_state = DS707_FLOW_FLTR_INSTALLING;
  }
  
  ps_flow_suspend_ind( ps_flow_ptr, PS_EIC_QOS_INTERNAL_MODIFY_IN_PROGRESS );
  ds_flow_ptr->notified_info_code = PS_EIC_QOS_INTERNAL_MODIFY_IN_PROGRESS;

  if (( ds_flow_ptr->dirn == DS707_DIRECTION_REV )|| 
      ( ds_flow_ptr->dirn == DS707_DIRECTION_BIDIRECTIONAL ))
  {
    /*-----------------------------------------------------------------------
    Empty the watermark. As a side-effect of emptying the watermark, the flow
    will be enabled if the queue level was above the high watermark.
    -----------------------------------------------------------------------*/
    /* Before cleaning up, update tx byte count */
    ds707_wmk_update_tx_count(ds_flow_ptr->tx_wm_list);
    ds707_wmk_cleanup_single_wmk(ds_flow_ptr->tx_wm_list);
  }

  cmd_ptr = 
    ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
  if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
  {
   /*-----------------------------------------------------------------
   Call ps flow rejected to clear the resources committed above
   ------------------------------------------------------------------*/
  (void) ps_iface_modify_flow_rejected(
                       ds707_pkt_get_iface_ptr_from_ps_flow(ps_flow_ptr),
                       ps_flow_ptr,
                       PS_EIC_QOS_INSUFFICIENT_LOCAL_RESOURCES,
                       ps_errno);
   *ps_errno = DS_ENOMEM;
    ASSERT(0); 
    return (-1);
  }
  
  sec_flow_cmd_ptr = 
    (ds707_sec_pkt_flow_cmd_type*) cmd_ptr->cmd_payload_ptr;
  memset(sec_flow_cmd_ptr, 
         0, 
         sizeof(ds707_sec_pkt_flow_cmd_type));

  /*-----------------------------------------------------------------
  Fillup the command buffer and post to DS task
  ------------------------------------------------------------------*/

  cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_MODIFY_CMD;
  sec_flow_cmd_ptr->num_of_flows = 1;
  sec_flow_cmd_ptr->client_data_ptr = NULL;
  sec_flow_cmd_ptr->flow_ptr[0] = ps_flow_ptr;

  ds_put_cmd(cmd_ptr);
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "QOS_MODIFY processed");
  return 0;
} /* ds707_pkt_mgr_flow_ioctl_qos_modify() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_QOS_RELEASE

DESCRIPTION   Handles the IOCTL to release the QoS

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  Posts a Command to DS task to complete the release
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_qos_release
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  int16                             ioctl_errno;
  int                               ret_val = 0; 
  ds_cmd_type                     * cmd_ptr = NULL; 
  ds707_sec_pkt_flow_cmd_type     * sec_flow_cmd_ptr = NULL;        
/*- - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  if(!PS_FLOW_IS_VALID(ps_flow_ptr))
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid PS flow 0x%x", ps_flow_ptr);
    *ps_errno = DS_EINVAL;
    return (-1);
  }

#ifdef FEATURE_NIQ_EHRPD
  /*-------------------------------------------------------------------
  Check if the flow is network initiated QOS flow, if yes, it is not 
  allowed to release by the ioctl. 
  --------------------------------------------------------------------*/
  if( ds707_sec_pkt_mgr_is_nw_init_flow(ps_flow_ptr) == TRUE )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "QOS release ioctl is not allowed for network "
                    "initiated QOS flow 0x%x",
                    ps_flow_ptr);
    *ps_errno = DS_EOPNOTSUPP;
    return (-1);
  }
#endif /*FEATURE_NIQ_EHRPD*/
  /*-----------------------------------------------------------------------
  If the flow state is GOING NULL or NULL, ignore the command
  -----------------------------------------------------------------------*/
  if ( (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_NULL) ||
       (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_GOING_NULL))
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "Flow 0x%p on 0x%x state",
                     ps_flow_ptr,
                     PS_FLOW_GET_STATE(ps_flow_ptr));
    *ps_errno = DS_EFAULT;
    return (-1);
  }
  else
  {
    /*----------------------------------------------------------------------
     Get the command buffer. Make sure that if the command is obtained
     successfully, it must post the command. 
     
         **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
     ----------------------------------------------------------------------*/
    cmd_ptr = 
      ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
    if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
    {
      ASSERT(0);
      *ps_errno = DS_ENOMEM;
      return (-1);
    }
    
    sec_flow_cmd_ptr = 
      (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;
    memset(sec_flow_cmd_ptr, 
           0, 
           sizeof(ds707_sec_pkt_flow_cmd_type));

    ret_val = ps_flow_go_null_cmd ( ps_flow_ptr, &ioctl_errno,NULL );

     
    if ( !((ret_val == -1) && (ioctl_errno == DS_EWOULDBLOCK)) )
    {
      ASSERT(0);
      ds_release_cmd_buf( &cmd_ptr );
      return(-1);
    }


    cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_GO_NULL_CMD;
    sec_flow_cmd_ptr->num_of_flows = 1;
    sec_flow_cmd_ptr->client_data_ptr = NULL;
    sec_flow_cmd_ptr->flow_ptr[0] = ps_flow_ptr;

    ds_put_cmd(cmd_ptr);
  }
  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "QOS_RELEASE processed");
  return 0;
} /* ds707_pkt_mgr_flow_ioctl_qos_release() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_QOS_RELEASE_EX

DESCRIPTION   Handles the IOCTL to release the QoS

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  Posts a Command to DS task to complete the release
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_qos_release_ex
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  int16                                 ioctl_errno;
  ps_flow_ioctl_qos_release_ex_type   * qos_ioctl_arg_release_ex;
  ps_flow_type                        * valid_ps_flow[MAX_QOS_OPERATIONS_PER_IOCTL];
  uint8                                 num_valid_flows =0;
  uint8                                 i;
  ds_cmd_type                         * cmd_ptr = NULL;
  ds707_sec_pkt_flow_cmd_type         * sec_flow_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ps_flow_ptr);

  /*-------------------------------------------------------------------
  If argval pointer is NULL, fail IOCTL
  --------------------------------------------------------------------*/
  if (argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "argval_ptr NULL");
    *ps_errno = DS_EFAULT;
    return(-1);
  }

  qos_ioctl_arg_release_ex = 
                           ( ps_flow_ioctl_qos_release_ex_type *) argval_ptr;

  if (qos_ioctl_arg_release_ex->num_flows >MAX_QOS_OPERATIONS_PER_IOCTL)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "Num flows (%d) is more than max allowed (%d)", 
                    qos_ioctl_arg_release_ex->num_flows ,
                    MAX_QOS_OPERATIONS_PER_IOCTL);
    *ps_errno = DS_EFAULT;
    return(-1);
  }

  /*-------------------------------------------------------------------
  Validate if the bundled request has duplicate entry or invalid entry
  --------------------------------------------------------------------*/
  if (ds707_sec_pkt_mgr_bundle_flows_validate(
                            qos_ioctl_arg_release_ex->flows_ptr, 
                            qos_ioctl_arg_release_ex->num_flows) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Error on bundle flows");
    *ps_errno = DS_EINVAL;
    return(-1);
  }

#ifdef FEATURE_NIQ_EHRPD
  /*-------------------------------------------------------------------
  Check if network initiated QOS flow contained, if yes, it is not 
  allowed to release by the ioctl. 
  --------------------------------------------------------------------*/
  if( ds707_sec_pkt_mgr_if_contain_nw_init_flow(
                             qos_ioctl_arg_release_ex->flows_ptr,
                             qos_ioctl_arg_release_ex->num_flows) == TRUE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "QOS release ioctl is not allowed for network "
                    "initiated QOS flow");
    *ps_errno = DS_EOPNOTSUPP;
    return (-1);
  }
#endif /*FEATURE_NIQ_EHRPD*/

  /*-------------------------------------------------------------------
  Filter out the valid flows
  --------------------------------------------------------------------*/
  for (i = 0; i< qos_ioctl_arg_release_ex->num_flows; i++)
  {
    /*-------------------------------------------------------------------
    Ignore the flow if the flow state is NULL or GOING NULL
    --------------------------------------------------------------------*/
    if ((PS_FLOW_GET_STATE(qos_ioctl_arg_release_ex->flows_ptr[i]) == 
                                                               FLOW_NULL) || 
        (PS_FLOW_GET_STATE(qos_ioctl_arg_release_ex->flows_ptr[i]) == 
                                                           FLOW_GOING_NULL))
    {
      continue;
    }

    /*-------------------------------------------------------------------
    Collect all the valid flows
    --------------------------------------------------------------------*/
    valid_ps_flow[num_valid_flows] = qos_ioctl_arg_release_ex->flows_ptr[i];
    num_valid_flows++;
  }


  if (num_valid_flows == 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "No valid flow");
    return 0;
  }

  /*----------------------------------------------------------------------
  Get the command buffer. Make sure that if the command is obtained
  successfully, it must post the command. 
  
      **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
  ----------------------------------------------------------------------*/
  /*-------------------------------------------------------------------
  Release the flow
  --------------------------------------------------------------------*/
  ds707_pkt_mgr_flow_ioctl_flows_go_null_cmd( valid_ps_flow,
                                              num_valid_flows,
                                              &ioctl_errno,
                                              NULL);

  /*-------------------------------------------------------------------
   Fillup the command buffer and post the command to DS task
  --------------------------------------------------------------------*/
  cmd_ptr = 
     ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
   if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
   {
     ASSERT(0);
     *ps_errno = DS_ENOMEM;
     return (-1);
   }
   
   sec_flow_cmd_ptr = 
     (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;
   memset(sec_flow_cmd_ptr, 
          0, 
          sizeof(ds707_sec_pkt_flow_cmd_type));

  cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_GO_NULL_CMD;
  sec_flow_cmd_ptr->num_of_flows = num_valid_flows;
  sec_flow_cmd_ptr->client_data_ptr = NULL;
  for (i = 0; i< num_valid_flows; i++)
  {
    sec_flow_cmd_ptr->flow_ptr[i] = valid_ps_flow[i];
  }
  ds_put_cmd(cmd_ptr);
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "QOS_RELEASE_EX processed");
  return 0;
} /* ds707_pkt_mgr_flow_ioctl_qos_release_ex() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_QOS_SUSPEND

DESCRIPTION   Handles the IOCTL to suspend the QoS

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  Posts a Command to DS task to complete the suspend
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_qos_suspend
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  int16                                 ioctl_errno;
  int                                   ret_val = 0; 
  boolean                               setid_non_zero;
  ds_cmd_type                         * cmd_ptr = NULL;
  ds707_sec_pkt_flow_cmd_type         * sec_flow_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  if(!PS_FLOW_IS_VALID(ps_flow_ptr))
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid PS flow 0x%x", ps_flow_ptr);
    *ps_errno = DS_EINVAL;
    return (-1);
  }
  
  /*-------------------------------------------------------------------
  If the QoS is not available, fail the IOCTL
  --------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Current System Does not support QoS");
    *ps_errno = DS_EQOSUNAWARE;
    return(-1);
  }

#ifdef FEATURE_NIQ_EHRPD
  /*-------------------------------------------------------------------
  Check if the flow is network initiated QOS flow, if yes, it is not 
  allowed to suspend by the ioctl. 
  --------------------------------------------------------------------*/
  if( ds707_sec_pkt_mgr_is_nw_init_flow(ps_flow_ptr) == TRUE )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "QOS suspend ioctl is not allowed for network "
                    "initiated QOS flow 0x%x",
                    ps_flow_ptr);
    *ps_errno = DS_EOPNOTSUPP;
    return (-1);
  }
#endif /*FEATURE_NIQ_EHRPD*/

  /*-------------------------------------------------------------------
  Check if the flow is being modified.If so, fail the IOCTL
  --------------------------------------------------------------------*/
  if( ds707_sec_pkt_mgr_is_modify_in_progress(
                (ds707_flow_type *)( ps_flow_ptr->client_data_ptr)) == TRUE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "Modify in progress for this flow 0x%x, ignore",
                     ps_flow_ptr);
    *ps_errno = DS_EINVAL;
    return (-1);
  }
 
  /*-------------------------------------------------------------------
  If the setid is zero, suspend fail the ioctl
  --------------------------------------------------------------------*/
  if (ds707_sec_pkt_granted_setid_non_zero( ps_flow_ptr, 
                                            &setid_non_zero) == TRUE)
  {
    /*-------------------------------------------------------------------
    If set id = 0, then fail the ioctl
    --------------------------------------------------------------------*/
    if (setid_non_zero == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "ioctl qos resume failed. Couldn't resume flow");
      *ps_errno = DS_EINVAL;
      /*----------------------------------------------------------
      Suspend the flow
      -----------------------------------------------------------*/
      ds707_sec_pkt_flow_suspend_ind(ps_flow_ptr, 
                                     PS_EIC_QOS_NOT_AVAILABLE);

      return -1;
    }
  }
  else
  {
    /*-------------------------------------------------------------------
    The set id was not granted yet. We want to send RESERVATION ON 
    request and start GRANT TIMER.
    --------------------------------------------------------------------*/
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "SET ID has not been granted yet. But process this "
                     "flow");
  }

  /*-------------------------------------------------------------------
  If the flow is already suspended, just return.
  --------------------------------------------------------------------*/
  if (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_SUSPENDED)
  {
    ps_flow_suspend_ind( ps_flow_ptr,
                         PS_EIC_NOT_SPECIFIED); 
    return 0;
  }

  /*-------------------------------------------------------------------
  Check if the reservation is alreay off. In that case notify the app
  --------------------------------------------------------------------*/
  if (ds707_qos_flow_mgr_is_resv_on_for_flow(
                  (ds707_flow_type *)ps_flow_ptr->client_data_ptr) == FALSE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Reservation is OFF");
    ps_flow_suspend_ind( ps_flow_ptr,
                         PS_EIC_NOT_SPECIFIED);
    return 0;
  }

  /*-------------------------------------------------------------------
  The operation is allowed only if the flow is in following state
  --------------------------------------------------------------------*/
  if ((PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_ACTIVATED) || 
      (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_RESUMING))
  {
    /*----------------------------------------------------------------------
    Get the command buffer. Make sure that if the command is obtained
    successfully, it must post the command. 
    
        **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
    ----------------------------------------------------------------------*/

    cmd_ptr = 
      ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
    if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
    {
      ASSERT(0);
      *ps_errno = DS_ENOMEM;
      return (-1);
    }
    
    sec_flow_cmd_ptr = 
      (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;
    memset(sec_flow_cmd_ptr, 
           0, 
           sizeof(ds707_sec_pkt_flow_cmd_type));

    /*-------------------------------------------------------------------
    SUSPEND the flow. If SUSPEND fails, fail the IOCTL
    --------------------------------------------------------------------*/
    ret_val = ps_flow_suspend_cmd ( ps_flow_ptr,
                                    &ioctl_errno,
                                    NULL );

    if ( !((ret_val == -1) && (ioctl_errno == DS_EWOULDBLOCK)) )
    {
      ASSERT(0);
      ds_release_cmd_buf( &cmd_ptr );
      return (-1);
    }
    
    /*-------------------------------------------------------------------
    Fill the command buffer and post to DS task
    --------------------------------------------------------------------*/
    cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_SUSPEND_CMD;
    sec_flow_cmd_ptr->num_of_flows = 1;
    sec_flow_cmd_ptr->client_data_ptr = NULL;
    sec_flow_cmd_ptr->flow_ptr[0] = ps_flow_ptr;
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    ERR ( "The flow is in wrong state :0x%x",
          PS_FLOW_GET_STATE(ps_flow_ptr),
          0,
          0);
    *ps_errno = DS_EFAULT;
    return(-1);
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "QOS_SUSPEND processed");
  return 0;
} /* ds707_pkt_mgr_flow_ioctl_qos_suspend() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_QOS_SUSPEND_EX

DESCRIPTION   Handles the IOCTL to suspend the QoS

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  Posts a Command to DS task to complete the suspend
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_qos_suspend_ex
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  int16                                 ioctl_errno;
  ps_flow_ioctl_qos_suspend_ex_type   * qos_ioctl_arg_suspend_ex;
  ps_flow_type                 * valid_ps_flow[MAX_QOS_OPERATIONS_PER_IOCTL];
  uint8                                 num_valid_flows =0;
  uint8                                 i;
  boolean                               setid_non_zero;
  ds_cmd_type                         * cmd_ptr = NULL; 
  ds707_sec_pkt_flow_cmd_type         * sec_flow_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ps_flow_ptr);

  /*-------------------------------------------------------------------
  If the QoS is not available, fail IOCTL
  --------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Current System Does not support QoS");
    *ps_errno = DS_EQOSUNAWARE;
    return(-1);
  }

  /*-------------------------------------------------------------------
  If argval pointer is NULL, fail IOCTL
  --------------------------------------------------------------------*/
  if (argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "argval_ptr NULL");
    *ps_errno = DS_EFAULT;
    return(-1);

  }
  qos_ioctl_arg_suspend_ex = 
                         ( ps_flow_ioctl_qos_suspend_ex_type *) argval_ptr;

  if (qos_ioctl_arg_suspend_ex->num_flows >MAX_QOS_OPERATIONS_PER_IOCTL)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "Num flows (%d) is more than max allowed (%d)",
                    qos_ioctl_arg_suspend_ex->num_flows,
                    MAX_QOS_OPERATIONS_PER_IOCTL);
    *ps_errno = DS_EFAULT;
    return(-1);
  }

  /*-------------------------------------------------------------------
  Validate if the bundle request has duplicate entry
  --------------------------------------------------------------------*/
  if (ds707_sec_pkt_mgr_bundle_flows_validate(
                     qos_ioctl_arg_suspend_ex->flows_ptr, 
                     qos_ioctl_arg_suspend_ex->num_flows) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Error on bundle flows");
    *ps_errno = DS_EINVAL;
    return(-1);
  }

#ifdef FEATURE_NIQ_EHRPD
  /*-------------------------------------------------------------------
  Check if network initiated QOS flow contained, if yes, it is not 
  allowed to suspend by the ioctl. 
  --------------------------------------------------------------------*/
  if( ds707_sec_pkt_mgr_if_contain_nw_init_flow(
                             qos_ioctl_arg_suspend_ex->flows_ptr,
                             qos_ioctl_arg_suspend_ex->num_flows) == TRUE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "QOS suspend ioctl is not allowed for network "
                    "initiated QOS flow");
    *ps_errno = DS_EOPNOTSUPP;
    return (-1);
  }
#endif /*FEATURE_NIQ_EHRPD*/

  /*-------------------------------------------------------------------
  Filter out the valid flows
  --------------------------------------------------------------------*/
  for (i = 0; i< qos_ioctl_arg_suspend_ex->num_flows; i++)
  {
    /*-------------------------------------------------------------------
    Check if the flow is being modified.If so, fail the IOCTL
    --------------------------------------------------------------------*/
    if(ds707_sec_pkt_mgr_is_modify_in_progress(
        (ds707_flow_type *)( qos_ioctl_arg_suspend_ex->flows_ptr[i]->client_data_ptr)) == TRUE)
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Modify in progress 0x%x",
                       qos_ioctl_arg_suspend_ex->flows_ptr[i]);
      continue;
    }

    if (ds707_sec_pkt_granted_setid_non_zero( 
                            qos_ioctl_arg_suspend_ex->flows_ptr[i], 
                            &setid_non_zero) == TRUE)
    {
      /*-------------------------------------------------------------------
      If set id = 0, then fail the ioctl
      --------------------------------------------------------------------*/
      if (setid_non_zero == FALSE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "ioctl qos resume failed. Couldn't resume flow");
        *ps_errno = DS_EINVAL;
        /*----------------------------------------------------------
        Suspend the flow
        -----------------------------------------------------------*/
        ds707_sec_pkt_flow_suspend_ind(
                                qos_ioctl_arg_suspend_ex->flows_ptr[i], 
                                PS_EIC_QOS_NOT_AVAILABLE);
        return -1;
      }
    }
    else
    {
      /*-------------------------------------------------------------------
      The set id was not granted yet. We want to send RESERVATION ON 
      request and start GRANT TIMER.
      --------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                     "SET ID has not been granted yet. But process this flow");
    }

    if (PS_FLOW_GET_STATE(qos_ioctl_arg_suspend_ex->flows_ptr[i]) == 
                                                             FLOW_SUSPENDED)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Already SUSPENDED");
      ps_flow_suspend_ind( qos_ioctl_arg_suspend_ex->flows_ptr[i],
                           PS_EIC_NOT_SPECIFIED);

      continue;
    }

    /*-------------------------------------------------------------------
    Check if the reservation is alreay off. In that case notify the app
    --------------------------------------------------------------------*/
    if (ds707_qos_flow_mgr_is_resv_on_for_flow((ds707_flow_type *)
           qos_ioctl_arg_suspend_ex->flows_ptr[i]->client_data_ptr) == FALSE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Reservation is OFF");
      ps_flow_suspend_ind( qos_ioctl_arg_suspend_ex->flows_ptr[i], 
                           PS_EIC_NOT_SPECIFIED);
      continue;
    }

    /*-------------------------------------------------------------------
    If the flow is in the following states, ignore the command. Collect 
    only the valid flows
    --------------------------------------------------------------------*/
    if ((PS_FLOW_GET_STATE(qos_ioctl_arg_suspend_ex->flows_ptr[i]) == 
                                                        FLOW_ACTIVATED)  ||
        (PS_FLOW_GET_STATE(qos_ioctl_arg_suspend_ex->flows_ptr[i]) == 
                                                           FLOW_RESUMING) )
    {
      valid_ps_flow[num_valid_flows] = 
                                    qos_ioctl_arg_suspend_ex->flows_ptr[i];
      num_valid_flows++;
    }
  }

  /*-------------------------------------------------------------------
   If not a single flow out of the bundle is processed, return
  --------------------------------------------------------------------*/
  if (num_valid_flows == 0)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "No valid PS flow");
    return 0;
  }

  /*----------------------------------------------------------------------
    Get the command buffer. Make sure that if the command is obtained
    successfully, it must post the command. 
  
      **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
  ----------------------------------------------------------------------*/
    cmd_ptr = 
      ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
    if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
    {
      ASSERT(0);
      *ps_errno = DS_ENOMEM;
      return (-1); 
    }
    
    sec_flow_cmd_ptr = 
      (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;
    memset(sec_flow_cmd_ptr, 
           0, 
           sizeof(ds707_sec_pkt_flow_cmd_type));

  /*-------------------------------------------------------------------
   Suspend the flow
  --------------------------------------------------------------------*/
  ds707_pkt_mgr_flow_ioctl_flows_suspend_cmd(valid_ps_flow,
                                             num_valid_flows,
                                             &ioctl_errno,
                                             NULL);

  cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_SUSPEND_CMD;
  sec_flow_cmd_ptr->num_of_flows = num_valid_flows;
  sec_flow_cmd_ptr->client_data_ptr = NULL;
  for (i = 0; i<num_valid_flows; i++)
  {
    sec_flow_cmd_ptr->flow_ptr[i] = valid_ps_flow[i];
  }
  ds_put_cmd(cmd_ptr);

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "QOS_SUSPEND_EX processed");
  return 0;
} /* ds707_pkt_mgr_flow_ioctl_qos_suspend_ex() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_QOS_RESUME

DESCRIPTION   Handles the IOCTL to resume the QoS

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  Posts a Command to DS task to complete the resume
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_qos_resume
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  int16                         ioctl_errno;
  int                           ret_val = 0; 
  boolean                       setid_non_zero;
  ds_cmd_type                 * cmd_ptr = NULL;  
  ds707_sec_pkt_flow_cmd_type * sec_flow_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  if(!PS_FLOW_IS_VALID(ps_flow_ptr))
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid PS flow 0x%x", ps_flow_ptr);
    *ps_errno = DS_EINVAL;
    return (-1);
  }
  /*-------------------------------------------------------------------
  If the QoS is not available, return
  --------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Current System Does not support QoS");
    *ps_errno = DS_EQOSUNAWARE;
    return(-1);
  }

  if(ds707_sec_pkt_mgr_is_modify_in_progress(
      (ds707_flow_type *)( ps_flow_ptr->client_data_ptr)) == TRUE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Modify in progress 0x%x",
                     ps_flow_ptr);
    *ps_errno = DS_EINVAL;
    return (-1);
  }
  
  if (ds707_sec_pkt_granted_setid_non_zero( ps_flow_ptr, 
                                            &setid_non_zero) == TRUE)
  {
    /*-------------------------------------------------------------------
    If set id = 0, then fail the ioctl
    --------------------------------------------------------------------*/
    if (setid_non_zero == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "ioctl qos resume failed. Couldn't resume flow");
      *ps_errno = DS_EINVAL;
      /*----------------------------------------------------------
      Suspend the flow
      -----------------------------------------------------------*/
      ds707_sec_pkt_flow_suspend_ind(ps_flow_ptr, 
                                     PS_EIC_QOS_NOT_AVAILABLE);
      return -1;
    }
  }
  else
  {
    /*-------------------------------------------------------------------
    The set id was not granted yet. We want to send RESERVATION ON 
    request and start GRANT TIMER.
    --------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "SET ID has not been granted yet. But process this flow");
  }

  /*-------------------------------------------------------------------
  Check if the reservation is already ON and all the QoS conditions 
  are already met. Do not process any further for this flow
  --------------------------------------------------------------------*/
  if (ds707_qos_flow_mgr_is_resv_on_for_flow(
                  (ds707_flow_type *) ps_flow_ptr->client_data_ptr) == TRUE)
  {
    if (ds707_qos_flow_mgr_is_qos_granted_for_flow(
              (ds707_flow_type *) ps_flow_ptr->client_data_ptr) == TRUE)
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Activate 0x%x", ps_flow_ptr);
      ps_flow_activate_ind( ps_flow_ptr, 
                            PS_EIC_NOT_SPECIFIED);
      return 0;
    }
    else
    {
      /*---------------------------------------------------------------------
      This is dead lock condition. It should not happen. If the conditions 
      are in this state, ignore the command. Let the AN take incharge to 
      break the deadlock.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                       "The reservation of flow 0x%x is ON but one of the"
                       " QoS condtion does not meet. No operation",
                       ps_flow_ptr);
      *ps_errno = DS_EINVAL;
      return(-1);
    }
  }

  if ( (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_SUSPENDING)  ||  
       (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_SUSPENDED )
     )
  {
    /*----------------------------------------------------------------------
    Get the command buffer. Make sure that if the command is obtained
    successfully, it must post the command. 
    
        **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
    ----------------------------------------------------------------------*/

    cmd_ptr = 
      ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
    if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
    {
      *ps_errno = DS_ENOMEM;
      ASSERT(0);
      return (-1);
    }
    
    sec_flow_cmd_ptr = 
      (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;
    memset(sec_flow_cmd_ptr, 
           0, 
           sizeof(ds707_sec_pkt_flow_cmd_type));

    ret_val =  ps_flow_resume_cmd ( ps_flow_ptr,
                                    &ioctl_errno,
                                    NULL);

    if ( !((ret_val == -1) && (ioctl_errno == DS_EWOULDBLOCK)) )
    {
      ASSERT(0);
      ds_release_cmd_buf( &cmd_ptr );
      return (-1);
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_RESUME_CMD;
    sec_flow_cmd_ptr->num_of_flows = 1;
    sec_flow_cmd_ptr->client_data_ptr = NULL;
    sec_flow_cmd_ptr->flow_ptr[0] = ps_flow_ptr;
#ifdef FEATURE_NIQ_EHRPD
    /*----------------------------------------------------------------------
    Check if the ps_flow passed is for the network initiated QOS flow or 
    UE initiated QOS flow 
    ----------------------------------------------------------------------*/
    sec_flow_cmd_ptr->is_nw_init_flow = 
                                 ds707_sec_pkt_mgr_is_nw_init_flow(ps_flow_ptr);
#endif /*FEATURE_NIQ_EHRPD*/
    ds_put_cmd(cmd_ptr);
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "QOS_RESUME processed");
  return 0;
} /* ds707_pkt_mgr_flow_ioctl_qos_resume() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_QOS_RESUME_EX

DESCRIPTION   Handles the IOCTL to resume the QoS

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  Posts a Command to DS task to complete the resume
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_qos_resume_ex
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  int16                                 ioctl_errno;
  ps_flow_ioctl_qos_resume_ex_type    * qos_ioctl_arg_resume_ex;
  ps_flow_type               * valid_ps_flow[MAX_QOS_OPERATIONS_PER_IOCTL];
  uint8                                 num_valid_flows =0;
  uint8                                 i;
  boolean                               setid_non_zero;
  ds_cmd_type                         * cmd_ptr = NULL;
#ifdef FEATURE_NIQ_EHRPD
  ps_flow_type               * valid_nw_flow[MAX_QOS_OPERATIONS_PER_IOCTL];
  uint8                                 num_valid_nw_flows =0;
  ps_flow_type               * valid_ue_flow[MAX_QOS_OPERATIONS_PER_IOCTL];
  uint8                                 num_valid_ue_flows =0;
  ds_cmd_type                         * nw_cmd_ptr = NULL;
  ds707_sec_pkt_flow_cmd_type         * ue_sec_flow_cmd_ptr = NULL;
  ds707_sec_pkt_flow_cmd_type         * nw_sec_flow_cmd_ptr = NULL;
#else
  ds707_sec_pkt_flow_cmd_type         * sec_flow_cmd_ptr = NULL;
#endif /*FEATURE_NIQ_EHRPD*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ps_flow_ptr);

  /*-------------------------------------------------------------------
    If the QoS is not available, return
  --------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Current System Does not support QoS");
    *ps_errno = DS_EQOSUNAWARE;
    return(-1);
  }

  /*-------------------------------------------------------------------
  If argval pointer is NULL, fail IOCTL
  --------------------------------------------------------------------*/
  if (argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "argval_ptr NULL");
    *ps_errno = DS_EFAULT;
    return(-1);

  }
  qos_ioctl_arg_resume_ex = ( ps_flow_ioctl_qos_resume_ex_type *) argval_ptr;

  if (qos_ioctl_arg_resume_ex->num_flows > MAX_QOS_OPERATIONS_PER_IOCTL)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "Num flows (%d) is more than max allowed (%d)",
                    qos_ioctl_arg_resume_ex->num_flows,
                    MAX_QOS_OPERATIONS_PER_IOCTL);
    *ps_errno = DS_EFAULT;
    return(-1);
  }

  /*-------------------------------------------------------------------
  Validate the bundle flows for duplicate entry
  --------------------------------------------------------------------*/
  if (ds707_sec_pkt_mgr_bundle_flows_validate(
                             qos_ioctl_arg_resume_ex->flows_ptr, 
                             qos_ioctl_arg_resume_ex->num_flows) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Error on bundle flows");
    *ps_errno = DS_EINVAL;
    return(-1);
  }

  /*-------------------------------------------------------------------
  Filter out the valid flows
  --------------------------------------------------------------------*/
  for (i = 0; i< qos_ioctl_arg_resume_ex->num_flows; i++)
  {
    /*-------------------------------------------------------------------
    If the flow is being modified, filter out this flow
    --------------------------------------------------------------------*/
    if(ds707_sec_pkt_mgr_is_modify_in_progress(
      (ds707_flow_type *)( qos_ioctl_arg_resume_ex->flows_ptr[i]->client_data_ptr)) == TRUE)
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Modify in progress 0x%x",
                       qos_ioctl_arg_resume_ex->flows_ptr[i]);
      continue;
    }
    
    if (ds707_sec_pkt_granted_setid_non_zero( 
                  qos_ioctl_arg_resume_ex->flows_ptr[i], 
                  &setid_non_zero) == TRUE)
    {
      /*-------------------------------------------------------------------
      If set id = 0, then ignore this flow.
      --------------------------------------------------------------------*/
      if (setid_non_zero == FALSE)
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR, "SETID zero 0x%x",
                         qos_ioctl_arg_resume_ex->flows_ptr[i]);
        /*----------------------------------------------------------
        Suspend the flow
        -----------------------------------------------------------*/
        ds707_sec_pkt_flow_suspend_ind(qos_ioctl_arg_resume_ex->flows_ptr[i],
                                       PS_EIC_QOS_NOT_AVAILABLE);
        continue;
      }
    }
    else
    {
      /*-------------------------------------------------------------------
        The set id was not granted yet. We want to send RESERVATION ON 
        request and start GRANT TIMER.
      --------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_LOW,
                       "SET ID not granted yet, process");
    }
    /*-------------------------------------------------------------------
    Check if the reservation is already ON and all the QoS conditions 
    are already met. Do not process any further for this flow
    --------------------------------------------------------------------*/
    if (ds707_qos_flow_mgr_is_resv_on_for_flow(
                     (ds707_flow_type *)qos_ioctl_arg_resume_ex->flows_ptr[i]->client_data_ptr) == TRUE)
    {
      if (ds707_qos_flow_mgr_is_qos_granted_for_flow(
                    (ds707_flow_type *)qos_ioctl_arg_resume_ex->flows_ptr[i]->client_data_ptr) == TRUE)
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Activate 0x%x",
                         qos_ioctl_arg_resume_ex->flows_ptr[i]);

        ps_flow_activate_ind(qos_ioctl_arg_resume_ex->flows_ptr[i], 
                             PS_EIC_NOT_SPECIFIED);
      }
      else
      {
        /*-------------------------------------------------------------------
        This is dead lock condition. It should not happen. If the conditions 
        are in this state, ignore the command. Let the AN take incharge to 
        break the deadlock.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                         "The reservation of flow %x is ON but one of the"
                         " QoS condtion does not meet. No operation",
                         qos_ioctl_arg_resume_ex->flows_ptr[i]);
      }
      continue;
    }

    /*-------------------------------------------------------------------
    If the flow is in the following states, ignore the command. Collect 
    only the valid flows
    --------------------------------------------------------------------*/
    if (PS_FLOW_GET_STATE(qos_ioctl_arg_resume_ex->flows_ptr[i]) == 
                                                          FLOW_SUSPENDING ||
        PS_FLOW_GET_STATE(qos_ioctl_arg_resume_ex->flows_ptr[i]) == 
                                                             FLOW_SUSPENDED)
    {
      valid_ps_flow[num_valid_flows] = qos_ioctl_arg_resume_ex->flows_ptr[i];
      num_valid_flows++;
#ifdef FEATURE_NIQ_EHRPD
      if( ds707_sec_pkt_mgr_is_nw_init_flow(
                            qos_ioctl_arg_resume_ex->flows_ptr[i]) == TRUE )
      {
        valid_nw_flow[num_valid_nw_flows++] = 
                               qos_ioctl_arg_resume_ex->flows_ptr[i];
      }
      else
      {
        valid_ue_flow[num_valid_ue_flows++] = 
                               qos_ioctl_arg_resume_ex->flows_ptr[i];
      }
#endif /*FEATURE_NIQ_EHRPD*/
    }
  }

  /*-------------------------------------------------------------------
   If not a single flow out of the bundle is processed, then return
  --------------------------------------------------------------------*/
  if (num_valid_flows == 0)
  {
    return 0;
  }

  /*-------------------------------------------------------------------
   If FEATURE_NIQ_EHRPD is defined, allocate the cmd buffer for
   the network initiated QOS flows and UE initiated QOS flows seperately
  --------------------------------------------------------------------*/
#ifdef FEATURE_NIQ_EHRPD
  if (num_valid_ue_flows != 0)
  {
     cmd_ptr = 
      ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
    if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
    {
      *ps_errno = DS_ENOMEM;
      ASSERT(0);
      return (-1);
    }
    
    ue_sec_flow_cmd_ptr = 
      (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;
    memset(ue_sec_flow_cmd_ptr, 
           0, 
           sizeof(ds707_sec_pkt_flow_cmd_type));
  }

  if(num_valid_nw_flows != 0)
  {
    nw_cmd_ptr = 
      ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
    if ( (NULL == nw_cmd_ptr) || (NULL == nw_cmd_ptr->cmd_payload_ptr) ) 
    {
      *ps_errno = DS_ENOMEM;
      ASSERT(0);
      return (-1);
    }
    
    nw_sec_flow_cmd_ptr = 
      (ds707_sec_pkt_flow_cmd_type *) nw_cmd_ptr->cmd_payload_ptr;
    memset(nw_sec_flow_cmd_ptr, 
           0, 
           sizeof(ds707_sec_pkt_flow_cmd_type));
  }
#else
  /*----------------------------------------------------------------------
  Get the command buffer. Make sure that if the command is obtained
  successfully, it must post the command. 
  
      **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
  ----------------------------------------------------------------------*/
   cmd_ptr = 
      ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
    if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
    {
      *ps_errno = DS_ENOMEM;
      ASSERT(0);
      return (-1);
    }
    
    sec_flow_cmd_ptr = 
      (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;
    memset(sec_flow_cmd_ptr, 
           0, 
           sizeof(ds707_sec_pkt_flow_cmd_type));
#endif /*FEATURE_NIQ_EHRPD*/

  /*-------------------------------------------------------------------
  Resume the flow
  --------------------------------------------------------------------*/
  ds707_pkt_mgr_flow_ioctl_flows_resume_cmd(valid_ps_flow,
                                            num_valid_flows,
                                            &ioctl_errno,
                                            NULL);
  /*-------------------------------------------------------------------
   If FEATURE_NIQ_EHRPD is defined, post the DS cmds for the network
   initiated flows and UE initiated flows seperately.
  --------------------------------------------------------------------*/
#ifdef FEATURE_NIQ_EHRPD
  if (num_valid_ue_flows != 0)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_RESUME_CMD;
    ue_sec_flow_cmd_ptr->num_of_flows = num_valid_ue_flows;
    ue_sec_flow_cmd_ptr->client_data_ptr = NULL;
    ue_sec_flow_cmd_ptr->is_nw_init_flow = FALSE;

    for (i = 0; i<num_valid_ue_flows; i++)
    {
      ue_sec_flow_cmd_ptr->flow_ptr[i] = valid_ue_flow[i];
    }
    ds_put_cmd(cmd_ptr);
  }

  if (num_valid_nw_flows != 0)
  {
    /*-------------------------------------------------------------------
    Resume the flow
    --------------------------------------------------------------------*/
    nw_cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_RESUME_CMD;
    nw_sec_flow_cmd_ptr->num_of_flows = num_valid_nw_flows;
    nw_sec_flow_cmd_ptr->client_data_ptr = NULL;
    nw_sec_flow_cmd_ptr->is_nw_init_flow = TRUE;

    for (i = 0; i<num_valid_nw_flows; i++)
    {
      nw_sec_flow_cmd_ptr->flow_ptr[i] = valid_nw_flow[i];
    }
    ds_put_cmd(nw_cmd_ptr);
  }
#else
  cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_RESUME_CMD;
  sec_flow_cmd_ptr->num_of_flows = num_valid_flows;
  sec_flow_cmd_ptr->client_data_ptr = NULL;

  for (i = 0; i<num_valid_flows; i++)
  {
    sec_flow_cmd_ptr->flow_ptr[i] = valid_ps_flow[i];
  }
  ds_put_cmd(cmd_ptr);
#endif /*FEATURE_NIQ_EHRPD*/

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "QOS_RESUME_EX processed");
  return 0;
} /* ds707_pkt_mgr_flow_ioctl_qos_resume_ex() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_GET_RMAC3_INFO

DESCRIPTION   Handles the IOCTL to return the rmac3 info.

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_get_rmac3_info
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  uint8                                 rlpid;
  uint8                                 rtcmac_flow_id;
  hdrcom_hai_stream_enum_type               stream_id = HDRHAI_STREAM0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( argval_ptr == NULL )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }
  if (hdrcp_stream_stream_is_assigned( HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
     (hdrcp_stream_stream_is_assigned(
                           HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))||
      (hdrcp_stream_stream_is_assigned(
                           HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN ))
      )
  {
    if ( ds707_sec_pkt_rlp_id ( ps_flow_ptr, 
                     DSRLP_VER_HDR_MULTIFLOW_REVERSE, &rlpid ) == FALSE )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Cannot get the RLP ID");
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN))
    {
      stream_id = hdrpac_get_stream_for_app_subtype(
                                       HDRHAI_MULTIFLOW_PACKET_APP_SN);
    }
    else if (hdrcp_stream_stream_is_assigned(
                           HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))
    {
      stream_id = hdrpac_get_stream_for_app_subtype(
                                   HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
    }
    else if (hdrcp_stream_stream_is_assigned(
                               HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN ))
    {
      stream_id = hdrpac_get_stream_for_app_subtype(
                               HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN );
    }

    rtcmac_flow_id = hdrpac_get_rtcmac_flow( stream_id,
                                             rlpid);

    if (rtcmac_flow_id != HDRCP_PCP_INVALID_MAC_FLOW_ID)
    {
      /*lint -e534 return value not required*/
      (void)hdrl1_rmac3_get_mac_info( rtcmac_flow_id,
                                   argval_ptr);
      /*lint -restore*/
      return 0;

    }
    else
    {
      DATA_IS707_MSG2(MSG_LEGACY_ERROR, "Invalid RMAC ID: %d for this RLP %d",
                      rtcmac_flow_id, rlpid);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }
  else
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }
} /* ds707_pkt_mgr_flow_ioctl_get_rmac3_info() */
#endif /* FEATURE_HDR_QOS */

/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_GET_TX_QUEUE_LEVEL

DESCRIPTION   Handles the IOCTL to return TX QUEUE LEVEL

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_get_tx_queue_level
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{  
#ifdef FEATURE_HDR
  ds707_flow_type                     * ds_flow_ptr;
  dsrlp_rlp_identity_type *rlp_id_ptr;
  /* Total amount of data pending New data + retransmission queue */    
  uint32                   total_pending_cnt ; 
  /* Remaining capacity of the watermark before it hits high 
   */    
  uint32                   wm_free_cnt ; 
  /* Amount of data currently present in the watermark 
   */    
  uint32                   current_cnt ; 
#endif /* FEATURE_HDR */
  ps_iface_type         * iface_ptr;
  ps_phys_link_type     * phys_link_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( argval_ptr == NULL )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  iface_ptr      = ds707_pkt_get_iface_ptr_from_ps_flow(ps_flow_ptr);
  phys_link_ptr  = PS_IFACE_GET_PHYS_LINK (iface_ptr);


  /*-----------------------------------------------------------------------
    If the IFACE is not up, or the AT is dormant, then it makes no sense
    to return valid Queue level sizes. As we cannot enqueue into the 
    Watermark in such cases. So it is as if the Queue is full.
  -----------------------------------------------------------------------*/
  if ( ( ps_iface_state(iface_ptr) != IFACE_UP ) ||
       ( ds707_pkt_is_dormant(phys_link_ptr) == TRUE ) )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-----------------------------------------------------------------------
    If the flow is not in activated state, then it makes no sense
    to return valid Queue level sizes. As we cannot enqueue into the 
    Watermark in such cases. So it is as if the Queue is full.
  -----------------------------------------------------------------------*/
  if ( PS_FLOW_GET_STATE(ps_flow_ptr) != FLOW_ACTIVATED )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

#ifdef FEATURE_HDR
  if ( ( hdrcp_stream_stream_is_assigned( HDRHAI_DEF_PACKET_APP_SN ) )
#ifdef FEATURE_HDR_QOS       
       || ( hdrcp_stream_stream_is_assigned( HDRHAI_MULTIFLOW_PACKET_APP_SN ) )
       || ( hdrcp_stream_stream_is_assigned( HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN ))
       || ( hdrcp_stream_stream_is_assigned( HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN  ))
#endif  /* FEATURE_HDR_QOS */       

     )
  {
    ds_flow_ptr  =  ds707_pkt_mgr_get_ds_flow(ps_flow_ptr);
    if ( ds_flow_ptr == NULL )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Cannot get a valid ds_flow_ptr");
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if ( ds_flow_ptr->tx_wm_list == NULL )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "NULL Watermark Pointer");
      *ps_errno = DS_EFAULT;
      return -1;
    }

    rlp_id_ptr = &(ds_flow_ptr->rev_tc_ptr)->rev_rlp.flow;

    /* Number of bytes that can still be filled in the watermark */
    wm_free_cnt = dsrsp_wm_free_cnt( rlp_id_ptr, 
                                     ds_flow_ptr->tx_wm_list);

    /* Number of bytes currently in the watermark */
    current_cnt = dsrsp_wm_current_cnt( rlp_id_ptr,
                                        ds_flow_ptr->tx_wm_list);

    if (hdrrlp_populate_tx_data_q_level(&total_pending_cnt, 
         ((ds707_tc_state_type*) (ds_flow_ptr->rev_tc_ptr))->rev_rlp.flow.flow,
         ((ds707_tc_state_type*) (ds_flow_ptr->rev_tc_ptr))->rev_rlp.flow.ver) == -1)
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "hdrrlp_populate_tx_data_q_level returned error");
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if ( current_cnt > total_pending_cnt )
    {
      DATA_IS707_MSG0(MSG_LEGACY_FATAL, "Invalid Number returned from IOCTL");
      *ps_errno = DS_EFAULT;
      return -1;
    }

    ((ps_flow_ioctl_tx_queue_level_type*) argval_ptr)->current_new_data_cnt =
                                                                current_cnt;
    ((ps_flow_ioctl_tx_queue_level_type*) argval_ptr)->wm_free_cnt = 
                                                                wm_free_cnt;
    ((ps_flow_ioctl_tx_queue_level_type*) argval_ptr)->total_pending_cnt = 
                                                          total_pending_cnt;

    DATA_IS707_MSG3(MSG_LEGACY_MED, "Values returned are %d %d %d", 
      ((ps_flow_ioctl_tx_queue_level_type *)argval_ptr)->wm_free_cnt, 
      ((ps_flow_ioctl_tx_queue_level_type *)argval_ptr)->current_new_data_cnt,
      ((ps_flow_ioctl_tx_queue_level_type *)argval_ptr)->total_pending_cnt);

    return 0;
  }
  else
#endif /* FEATURE_HDR */
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }
}  /* ds707_pkt_mgr_flow_ioctl_get_tx_queue_level() */

  #ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION DS707_PKT_MGR_FLOW_IOCTL_GET_TX_STATUS

DESCRIPTION   Handles the IOCTL to return TX STATUS. Returns if a packet 
              has been dropped on the RLP to which this flow has been bound. 
              Dropping can happen due to
              1. Staleness: Packet does not get to go over the air for
                            the duration set on the packet.
              2. MARQ: Even if we have tried to send it over the air,
                       the packet could not make it to the other side
                       due to physical layer nak.

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS   Resets the boolean flag on the RLP
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_get_tx_status
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  ds707_flow_type                     * ds_flow_ptr;
  dsrlp_rlp_identity_type               rlp_id;
  boolean                               dropped;
  boolean                               marq_dropped;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid argval_ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if ( !(PS_FLOW_IS_VALID(ps_flow_ptr)))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_flow_ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if ( ( hdrcp_stream_stream_is_assigned( HDRHAI_DEF_PACKET_APP_SN ) )
#ifdef FEATURE_HDR_QOS       
       || ( hdrcp_stream_stream_is_assigned( HDRHAI_MULTIFLOW_PACKET_APP_SN ) )
       || ( hdrcp_stream_stream_is_assigned( HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN ))
       || ( hdrcp_stream_stream_is_assigned( HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
#endif  /* FEATURE_HDR_QOS */       
     )
  {
    ds_flow_ptr  =  ds707_pkt_mgr_get_ds_flow(ps_flow_ptr);
    if ( ds_flow_ptr == NULL )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Cannot get a valid ds_flow_ptr");
      *ps_errno = DS_EFAULT;
      return -1;
    }

    if ( ds_flow_ptr->rev_tc_ptr == NULL )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Cannot get a valid rev_tc_ptr");
      *ps_errno = DS_EFAULT;
      return -1;
    }

    rlp_id.flow = (ds_flow_ptr->rev_tc_ptr->rev_rlp).flow.flow;
    rlp_id.ver   = (ds_flow_ptr->rev_tc_ptr->rev_rlp).flow.ver;

    dsrlp_get_dropped_stale_packets( rlp_id, 
                                     &dropped );
    dsrlp_get_marq_info( rlp_id, 
                         &marq_dropped);

    *((ps_flow_ioctl_707_tx_status_type*)argval_ptr) = 
                     (ps_flow_ioctl_707_tx_status_type)dropped ||
                     (ps_flow_ioctl_707_tx_status_type)marq_dropped;
    return 0;
  }
  else
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

} /* ds707_pkt_mgr_flow_ioctl_get_tx_status() */

/*===========================================================================
FUNCTION ds707_pkt_mgr_flow_ioctl_get_inactivity_timer

DESCRIPTION   

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS   Resets the boolean flag on the RLP
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_get_inactivity_timer
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  ds707_flow_type                     * ds_flow_ptr;
  byte                                rscb_index;
  uint32                              inactivity_timer_val_tmp = 0;
  uint32                              inactivity_timer_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid argval_ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if ( PS_FLOW_IS_VALID(ps_flow_ptr) != TRUE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_flow_ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ds_flow_ptr  =  ds707_pkt_mgr_get_ds_flow(ps_flow_ptr);
  if ( ds_flow_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Cannot get a valid ds_flow_ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  inactivity_timer_val = ds_flow_ptr->inactivity_timer_val;

  /*-------------------------------------------------------------------------
  Get the RSCB index for both forward and reverse direction (if applicable).
  Get the larger of the two inactivity timers. 
  This is done because the phys link goes down only when both directions are 
  inactive.
  -------------------------------------------------------------------------*/
  if ((ds_flow_ptr->fwd_tc_ptr != NULL) && 
      (PS_FLOW_GET_STATE (ps_flow_ptr) == FLOW_ACTIVATED ))
  {
    if (ds_flow_ptr->fwd_tc_ptr->fwd_rlp.flow.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
    {
      rscb_index = dsrlp_get_rlp_rscb_index(
                     ds_flow_ptr->fwd_tc_ptr->fwd_rlp.flow.flow, 
                     ds_flow_ptr->fwd_tc_ptr->fwd_rlp.flow.ver);
      if (rscb_index != DSRLPI_INVALID_RSCB_INDEX)
      {
        /* Get the inactivity timer */
        hdrmrlp_get_rlp_inactivity_timer(rscb_index,&inactivity_timer_val_tmp);
        inactivity_timer_val = inactivity_timer_val_tmp;
      
      }/*if DSRLPI_INVALID_RSCB_INDEX*/
    }/*if DSRLP_VER_HDR_MULTIFLOW_FORWARD*/
  } /*ds_flow_ptr->fwd_tc_ptr != NULL*/

  if ((ds_flow_ptr->rev_tc_ptr != NULL) && 
      (PS_FLOW_GET_STATE (ps_flow_ptr) == FLOW_ACTIVATED ))
  {
    if (ds_flow_ptr->rev_tc_ptr->rev_rlp.flow.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
    {
      rscb_index = dsrlp_get_rlp_rscb_index(
                     ds_flow_ptr->rev_tc_ptr->rev_rlp.flow.flow, 
                     ds_flow_ptr->rev_tc_ptr->rev_rlp.flow.ver);
      if (rscb_index != DSRLPI_INVALID_RSCB_INDEX)
      {

        hdrmrlp_get_rlp_inactivity_timer(rscb_index,&inactivity_timer_val_tmp);
        if (inactivity_timer_val_tmp > inactivity_timer_val) 
        {
          inactivity_timer_val = inactivity_timer_val_tmp;
        }
        
      }/*if DSRLPI_INVALID_RSCB_INDEX*/
    }/*if DSRLP_VER_HDR_MULTIFLOW_REVERSE*/      
  }/* ds_flow_ptr->rev_tc_ptr != NULL */

  * ((ps_flow_ioctl_707_inactivity_timer_type*)argval_ptr) = 
    inactivity_timer_val;
  DATA_IS707_MSG1(MSG_LEGACY_MED, "Inactivity timer val = 0x%x",
                  *((ps_flow_ioctl_707_inactivity_timer_type*)argval_ptr));
  return 0;
}

/*===========================================================================
FUNCTION ds707_pkt_mgr_flow_ioctl_set_inactivity_timer

DESCRIPTION   

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS   Resets the boolean flag on the RLP
===========================================================================*/
LOCAL int ds707_pkt_mgr_flow_ioctl_set_inactivity_timer
(
  ps_flow_type         * ps_flow_ptr,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  ds707_flow_type      * ds_flow_ptr;
  byte                 rscb_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid argval_ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if ( PS_FLOW_IS_VALID(ps_flow_ptr) != TRUE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_flow_ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ds_flow_ptr  =  ds707_pkt_mgr_get_ds_flow(ps_flow_ptr);
  if ( ds_flow_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Cannot get a valid ds_flow_ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ds_flow_ptr->inactivity_timer_val = 
    * ((ps_flow_ioctl_707_inactivity_timer_type*)argval_ptr);

  if (ds_flow_ptr->inactivity_timer_val > 0)
  {
    /*--------------------------------------------------------------------
    Set the inactivity timer value for the RLP as the largest inactivity  
    timervalue for all active DS flows bound to it    
    --------------------------------------------------------------------*/
    if ((ds_flow_ptr->fwd_tc_ptr != NULL) &&
        (PS_FLOW_GET_STATE (ps_flow_ptr) == FLOW_ACTIVATED ))
    {
      if (ds_flow_ptr->fwd_tc_ptr->fwd_rlp.flow.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        rscb_index = dsrlp_get_rlp_rscb_index(
                       ds_flow_ptr->fwd_tc_ptr->fwd_rlp.flow.flow, 
                       ds_flow_ptr->fwd_tc_ptr->fwd_rlp.flow.ver);
        if (rscb_index != DSRLPI_INVALID_RSCB_INDEX)
        {
          hdrmrlp_set_rlp_inactivity_timer(rscb_index, ds_flow_ptr->inactivity_timer_val);
        }/*if DSRLPI_INVALID_RSCB_INDEX*/
      }/*if DSRLP_VER_HDR_MULTIFLOW_FORWARD*/
    } /*ds_flow_ptr->fwd_tc_ptr != NULL*/

    if ((ds_flow_ptr->rev_tc_ptr != NULL) && 
        (PS_FLOW_GET_STATE (ps_flow_ptr) == FLOW_ACTIVATED ))
    {
      if (ds_flow_ptr->rev_tc_ptr->rev_rlp.flow.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
      {
        rscb_index = dsrlp_get_rlp_rscb_index(
                       ds_flow_ptr->rev_tc_ptr->rev_rlp.flow.flow, 
                       ds_flow_ptr->rev_tc_ptr->rev_rlp.flow.ver);
        if (rscb_index != DSRLPI_INVALID_RSCB_INDEX)
        {
          hdrmrlp_set_rlp_inactivity_timer(rscb_index, ds_flow_ptr->inactivity_timer_val);
        }/*if DSRLPI_INVALID_RSCB_INDEX*/
      }/*if DSRLP_VER_HDR_MULTIFLOW_REVERSE*/      
    }/* ds_flow_ptr->rev_tc_ptr != NULL */
  }/**/
  

  DATA_IS707_MSG1(MSG_LEGACY_MED, "Inactivity timer val = 0x%x",
                  *((ps_flow_ioctl_707_inactivity_timer_type*)argval_ptr));
  return 0;
}
 #endif /* FEATURE_HDR_QOS */    

/*===========================================================================
                       IFACE IOCTL HANDLER FUNCTIONS 
===========================================================================*/

/*===========================================================================
FUNCTION DS707_PKT_MGR_IFACE_IOCTL_GO_NULL

DESCRIPTION   Handles the IOCTL move the state of iface to NULL i.e. tear 
              down the iface

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  
===========================================================================*/

LOCAL int ds707_pkt_mgr_iface_ioctl_go_null
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  int    ret_val = 0; 

  /*-------------------------------------------------------------------
     For JCDMA 2 low battery event processing, we set the primary
     traffic channel state's ordq value to the one passed by
     OEM. This will be used by ds707_pkti_call_ended_hdlr to return
     ORDQ value to CM.
     ---------------------------------------------------------------------*/
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    ds707_tc_state_type      *tc_state_ptr;
    dss_iface_ioctl_null_arg_type   * release_ptr;

    /*Get Primary Traffic Channel State */
    tc_state_ptr = ds707_get_pri_tc_ptr();
    tc_state_ptr->ordq_value = DS707_ORDQ_NULL;

    DATA_IS707_MSG0( MSG_LEGACY_MED, "JCDMA 2 go null processing");
    if (argval_ptr != NULL)
    {
      release_ptr = (dss_iface_ioctl_null_arg_type * ) argval_ptr;
      if (release_ptr->release_reason_value >= DSS_RELEASE_REASON_MIN &&
          release_ptr->release_reason_value <= DSS_RELEASE_REASON_MAX)
      {
        if (release_ptr->release_reason_value == 
                                       DSS_RELEASE_REASON_LOW_BATTERY)
        {
          tc_state_ptr->ordq_value = DS707_ORDQ_LOW_BATTERY;
          DATA_IS707_MSG0(MSG_LEGACY_MED,
                "Setting Primary Traffic Channel State ORDQ to Low Battery");
        }
        else if (release_ptr->release_reason_value == 
                                               DSS_RELEASE_REASON_NONE)
        {
          tc_state_ptr->ordq_value = DS707_ORDQ_NONE;
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Setting Primary Traffic Channel State ORDQ to None");
        }
      }
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_MED,
                        "Ignoring invalid Release Reason %d passed to go null",
                        release_ptr->release_reason_value);
      }
    }
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "707 pkt ioctl - go NULL");
  ret_val = ps_iface_go_null_cmd(iface_ptr, 
                                 ps_errno,
                                 NULL);

  if (ret_val == 0) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Iface already down");
    return ret_val;
  }
  if ((ret_val < 0) && 
      (*ps_errno != DS_EWOULDBLOCK))
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Iface tear down failed, errno: %d",
                     ps_errno);
    return ret_val;
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Iface tear down in progress");
    return 0;
  }
} /* ds707_pkt_mgr_iface_ioctl_go_null() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_IPCP_DNS_OPT

DESCRIPTION   Handles the IOCTL to set the DNS negotiation option for 
              IPCP

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_ipcp_dns_opt
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  dssnet4_sm_cb_type *dssnet_instance = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(argval_ptr);
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  DATA_IS707_MSG1( MSG_LEGACY_HIGH,  "707 pkt ioctl - IPCP dns option %d",
                   *((boolean*)argval_ptr));

  if (ps_iface_addr_family_is_v4(iface_ptr))
  {
    dssnet_instance =  ds707_pkt_mgr_get_dssnet4_sm_cb (iface_ptr);
    {
      if ( dssnet_instance != NULL )
      {
        dssnet4_set_ipcp_dns_opt( dssnet_instance,
                                  *((boolean*)argval_ptr));
      }
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "ioctl ipcp_dns_opt on iface:0x%x", 
                        iface_ptr);
        *ps_errno = DS_EFAULT;
        return -1;
      }
    }
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_ipcp_dns_opt() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_MDR

DESCRIPTION   Handles the IOCTL to return the MDR Value

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_mdr
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  uint8  qcmdr_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_MDR_VAL,(void *)&qcmdr_val,
                                          DS707_NON_ARRAY);
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "707 pkt ioctl - get mdr=%d", qcmdr_val);
  if ( argval_ptr != NULL )
  {
    (* (unsigned int *) argval_ptr ) = qcmdr_val;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl getmdr failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_mdr() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_MDR

DESCRIPTION   Handles the IOCTL to set the MDR Value

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_set_mdr
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  unsigned int                      tmp_mdr_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(argval_ptr);
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    tmp_mdr_val = (* (unsigned int *) argval_ptr );
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl setmdr failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "707 pkt ioctl - set mdr=%d",
                   tmp_mdr_val);
  if ( tmp_mdr_val > (DS_MDR_MODE_MAX -1)  )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else
  {
    if ( ds707_pkt_mgr_set_mdr_val_and_so_pkt_recal ( tmp_mdr_val ) == FALSE )
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                      "ds707_pkt_mgr_set_mdr_val_and_so_pkt_recal(%d) fail ",
                      tmp_mdr_val);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_set_mdr() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_DORM_TIMER

DESCRIPTION   Handles the IOCTL to return the domancy timer

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_dorm_timer
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  uint32   cta_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,
                                          DS707_NON_ARRAY);
  DATA_MDM_ARG_NOT_CONST(argval_ptr);
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "707 pkt ioctl - get dorm=%d", cta_val);
  if ( argval_ptr != NULL )
  {
    (* (unsigned int *) argval_ptr ) = cta_val;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl getdorm failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_dorm_timer() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_DORM_TIMER

DESCRIPTION   Handles the IOCTL to set the Dormancy timer

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_set_dorm_timer
(
ps_iface_type            *iface_ptr,
void                     *argval_ptr,
sint15                   *ps_errno
)
{
  unsigned int                      tmp_cta_val;
  ps_phys_link_type     *phys_link_ptr;
  uint32                 cta_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MDM_ARG_NOT_CONST(argval_ptr);
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  if ( argval_ptr != NULL )
  {
    tmp_cta_val = (* (unsigned int *) argval_ptr );
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl setdorm failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "707 pkt ioctl - set dorm=%d", tmp_cta_val);
  if ( tmp_cta_val >  255  )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else
  {
    /*------------------------------------------------------------------- 
     If RDUD is active, change the default cta timer value. It will take 
     effect when RDUD expires and the default value is restored. If RDUD
     is not active, change the cta timer value and restart the timer.
    -------------------------------------------------------------------*/
#ifdef FEATURE_DS_RDUD
    if ( dsrdud_state_is_active() )
    {
      ds707_pkt_set_rdud_cta_default_value(tmp_cta_val);
    }
    else
#endif
    {
      cta_val = tmp_cta_val;
      (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CTA_VAL,(void*)&cta_val,0);
#ifdef FEATURE_HDR
      hdrrlp_copy_default_cta_val(cta_val);
#endif 
      if (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP)
      {
#ifdef FEATURE_HDR
        if (ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR)
        {
          hdrrlp_phy_link_up_inactivity_timer_hdlr();
        }
        else
#endif
        {
          ds707_cta_reset_timer(DS707_CTA_START_TIMER,
                           (ds707_get_pri_tc_ptr())->so);
        }
      }
    }
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_set_dorm_timer() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_RLP_ALL_CURR_NAK

DESCRIPTION   Handle the IOCTL to return the number of rounds and the naks 
              per round currently  in use (or last used) in an RLP3 service 
              option call
             

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_rlp_all_curr_nak
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  dss_iface_ioctl_707_rlp_opt_type  *tmp_rlp_opt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl get rlp curr nak failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if ( dsrlp_get_all_cur_naks (
                              tmp_rlp_opt_ptr->rscb_index,
                              (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                              /*lint -e545 bit wise comparator being used*/
                              (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),                              
                              (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                              (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                              /*lint -restore*/
                              ) == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl get rlp curr nak fail");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_rlp_all_curr_nak() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_RLP_ALL_CURR_NAK

DESCRIPTION   Handles the IOCTL to set the number of rounds and the naks per 
              round currently  in use (or last used) in an RLP3 service 
              option call
             

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_set_rlp_all_curr_nak
(
ps_iface_type            *iface_ptr,
void                     *argval_ptr,
sint15                   *ps_errno
)
{
  dss_iface_ioctl_707_rlp_opt_type  *tmp_rlp_opt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl set rlp curr nak failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  if (dsrlp_set_all_cur_naks(
                            tmp_rlp_opt_ptr->rscb_index,
                            /*lint -e545 byte wise comparators being used*/
                            (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                            (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                            (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                            (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                            /*lint -restore*/
                            ) == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl set rlp curr nak fail");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_set_rlp_all_curr_nak() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_RLP_ALL_DEF_NAK

DESCRIPTION  Handles the IOCTL to returns the number of default rounds 
             and the naks per round to be  used when negotiating an RLP3 
             service option
            

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_rlp_all_def_nak
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  dss_iface_ioctl_707_rlp_opt_type  *tmp_rlp_opt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl get rlp def nak failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  if ( dsrlp_get_all_def_naks (
                               /*lint -e545 bytewise comparators being used*/
                              (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                              (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                              (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                              (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                              /*lint -restore*/
                              ) == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl get rlp default nak fail");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_rlp_all_def_nak() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_RLP_ALL_DEF_NAK

DESCRIPTION  Handles the IOCTL to set the number of default rounds 
             and the naks per round to be  used when negotiating an RLP3 
             service option

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_set_rlp_all_def_nak
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  dss_iface_ioctl_707_rlp_opt_type  *tmp_rlp_opt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl set rlp def nak failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if ( dsrlp_set_all_def_naks (
                              /*lint -e545 bytewise comparators being used*/
                              (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                              (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                              (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                              (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                              /*lint restore*/
                              ) == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl set rlp default nak fail");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_set_rlp_all_def_nak() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_RLP_ALL_NEG_NAK

DESCRIPTION   Handles the iocto to return the negotiated parameters. 

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_rlp_all_neg_nak
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  dss_iface_ioctl_707_rlp_opt_type  *tmp_rlp_opt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl get rlp neg nak failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if ( dsrlp_get_all_neg_naks(
                             /*lint -e545 bytewise comparators being used*/
                             (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                             (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                             (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                             (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                             /*lint -restore*/
                             ) == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl get rlp negotiated nak fail");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_rlp_all_neg_nak() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_RLP_FILL_RATE

DESCRIPTION   Handles the IOCTL to return the fill rate for RLP

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_rlp_fill_rate
(
ps_iface_type            *iface_ptr,
void                     *argval_ptr,
sint15                   *ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    (* (uint8 *) argval_ptr ) = dsrlp_get_last_fill_nak_gap();
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl get rlp fill rate failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_rlp_fill_rate() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_RLP_FILL_RATE

DESCRIPTION   Handles the IOCTL to set the fill rate for RLP

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_set_rlp_fill_rate
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  uint8                             tmp_fill_rate;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  if ( argval_ptr != NULL )
  {
    /*-------------------------------------------------------------------
      Will be overriden by rlp_init if this was set while the
      RLP is not initialized. Lower limit of FILL RATE is set
      to a minimum of every 20 frames 
    -------------------------------------------------------------------*/
    tmp_fill_rate = (* ( uint8 *) argval_ptr );
    if ( (tmp_fill_rate < 20) || 
         (tmp_fill_rate > DSRLPI_LAST_FILL_NAK_GAP))
    {
      DATA_IS707_MSG2(MSG_LEGACY_ERROR, "ioctl set rlp fill rate failed. "
                      "MIN 20 MAX %d INPUT %d",
                      DSRLPI_LAST_FILL_NAK_GAP,
                      tmp_fill_rate);

      *ps_errno = DS_EFAULT;
      return -1;
    }
    else
    {
      dsrlp_set_last_fill_nak_gap(tmp_fill_rate);
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl set rlp fill rate failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_set_rlp_fill_rate() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_DATA_BEARER_RATE

DESCRIPTION  Handles the IOCTO to update Data Call Statistics

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_get_data_bearer_rate
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    ret_val = ds_1x_get_bearer_data_stats(
                               (ps_iface_ioctl_data_bearer_rate*)argval_ptr);
  }
  
  if ( ret_val == FALSE ) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl Get Bearer Rate failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_data_bearer_rate() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_BEARER_TECHNOLOGY

DESCRIPTION  Handles the IOCTL to return the Bearer Technology

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_bearer_technology
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( argval_ptr != NULL )
  {
    (void)ds707_pkt_mgr_get_bearer_tech_info(
                               (ps_iface_bearer_technology_type*)argval_ptr);
  }

  if ((NULL == argval_ptr) || !(DS707_PKT_MGR_IS_PS_IFACE_ACTIVE(iface_ptr)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
        "ioctl Get Bearer Technology failed. Null arg ptr or iface not active");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_bearer_technology() */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX

DESCRIPTION  Handles the IOCTL to return the Bearer Technology with new 
             data system info format 

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_get_bearer_technology_ex
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ds707_data_session_profile_info_type *data_sess_prof_ptr = NULL;
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
  sys_modem_as_id_e_type            resolved_subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((NULL == argval_ptr) || !(PS_IFACE_IS_UP(iface_ptr)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
        "ioctl Get Bearer Technology failed. Null arg ptr or iface not up");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
  ds707_pdncntx_get_profile(pdncntx_cb_ptr, &data_sess_prof_ptr);

  if( data_sess_prof_ptr == NULL) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Error in obataining data sesion ptr");
    return -1;
  }

  if( ds3g_resolve_subs_id(PS_IFACE_GET_SUBS_ID(iface_ptr),&resolved_subs_id ) == FALSE) 
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,"Error in obataining resolved sub id %d "
                       "Policy Sub ID %d",PS_IFACE_GET_SUBS_ID(iface_ptr),resolved_subs_id);
    return -1;
  }

  if( !(ds707_is_apn_pref_pkt_mode_ex(data_sess_prof_ptr, SYS_SYS_MODE_CDMA,resolved_subs_id)) &&
      !(ds707_is_apn_pref_pkt_mode_ex(data_sess_prof_ptr, SYS_SYS_MODE_HDR,resolved_subs_id)) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
           "ioctl Get Bearer Technology failed. pref pkt mode not CDMA or HDR");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if ( ds707_pkt_mgr_get_bearer_tech_info_ex(
	      (ds_sys_system_status_info_type*)argval_ptr) == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl Get Bearer Technology failed.");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_bearer_technology_ex() */
#endif

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_RLP_QOS_NA_PRI

DESCRIPTION   Handles the IOCTL to return desired value of QOS 
              non-assured mode


DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_get_rlp_qos_na_pri
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  byte                              tmp_qos_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  /*---------------------------------------------------------------------
    If desired values have been set in QOS, the API should return the
    current desired value. But if it hasn't been set at all, the API
    returns the default desired value, which in this case is the same
    as standard default value.
  ---------------------------------------------------------------------*/
  if ((dsrlp_get_desired_qos_non_assur_pri ( &tmp_qos_val )) == FALSE)
  {
    tmp_qos_val = DSRLP_QOS_NON_ASSUR_DEFAULT_PRI_ADJ;
  }
  if ( argval_ptr != NULL )
  {
    (*(byte *) argval_ptr ) = tmp_qos_val;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl get qos_na_pri failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "707 pkt ioctl - get qos_na_pri=%d", tmp_qos_val);
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_rlp_qos_na_pri() */
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_RLP_QOS_NA_PRI

DESCRIPTION   Handles the IOCTL to set desired value of QOS 
              non-assured mode

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_set_rlp_qos_na_pri
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  byte                              tmp_qos_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  if ( argval_ptr != NULL )
  {
    tmp_qos_val = (* (byte *) argval_ptr );
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl set qos_na_pri failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "707 pkt ioctl - set qos_na_pri=%d", tmp_qos_val);
  if ( tmp_qos_val >  (byte)DS707_QOS_DESIRED_PRI_MAX )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "ioctl set qos_na_pri fail %d out of range",
                    tmp_qos_val);
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else
  {
    (void) ds707_pkt_mgr_set_1x_profile_val(DS707_QCQOSPRI_VAL,
                                            (void *)&tmp_qos_val,0);
    dsrlp_set_desired_qos_non_assur_pri ( tmp_qos_val );
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_set_rlp_qos_na_pri() */
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_DATA_IS2000_SDB
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SDB_SUPPORT_QUERY

DESCRIPTION   Handles IOCTL to return is SDB is allowed or not

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_sdb_support_query
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  dss_iface_ioctl_707_sdb_support_query_type *tmp_sdb_query_ptr;
  uint32                            tmp_sdb_flags;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------
    The argument structure contains two fields. The first is input field
    flags which is the same as that provided to the socket write to
    specify SDB (Eg: MSG_USE_SDB_REACH, MSG_USE_SDB_ACH, etc).
    The second field is the output, when ioctl returns. It's boolean
    and says True if SDB can be supported.  Both BS and MS capability is
    checked.
  ---------------------------------------------------------------------*/
  if ( argval_ptr != NULL )
  {
    tmp_sdb_query_ptr =
    ( dss_iface_ioctl_707_sdb_support_query_type * ) argval_ptr;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl sdb query failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "707 pkt ioctl - sdbflags=%d", tmp_sdb_query_ptr->flags);
  tmp_sdb_query_ptr->can_do_sdb = FALSE;
  /* If it is For CDMA - MC will provide the required info */
  if ( ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_CDMA )
  {
    tmp_sdb_query_ptr->can_do_sdb = mcc_sdb_supported();
  }
  else if( ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR 
#ifdef FEATURE_DATA_OPTHO
           && ds707_s101_query_sm_state() == DS707_S101_SM_NON_TUNNEL
#endif /* FEATURE_DATA_OPTHO */
         )
  {
    tmp_sdb_query_ptr->can_do_sdb = TRUE;
  }
  if ( tmp_sdb_query_ptr->can_do_sdb == TRUE )
  {
    if ( (tmp_sdb_query_ptr->flags & MSG_EXPEDITE)
         == MSG_EXPEDITE)
    {
      tmp_sdb_flags = MSG_EXPEDITE;
    }
    else if ((tmp_sdb_query_ptr->flags & MSG_FAST_EXPEDITE)
             == MSG_FAST_EXPEDITE)
    {
      tmp_sdb_flags = MSG_FAST_EXPEDITE;
    }
    else
    {
      tmp_sdb_flags = tmp_sdb_query_ptr->flags;
    }
    tmp_sdb_query_ptr->can_do_sdb =
    ds707_sdb_supports_SDB_now ( tmp_sdb_flags, NULL );
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_sdb_support_query() */
#endif /* FEATURE_DATA_IS2000_SDB */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_ENABLE_HOLDDOWN

DESCRIPTION   Handles the IOCTL to enable the HoldDown timer

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_enable_holddown
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  boolean                           tmp_enable_holddown_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(argval_ptr);  

  if ( argval_ptr != NULL )
  {
    tmp_enable_holddown_val = ( * (boolean *) argval_ptr );
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl enable holddown failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ds707_pkt_enable_holddown_timer ( tmp_enable_holddown_val );
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_enable_holddown() */

  #ifdef FEATURE_HDR
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_ENABLE_HDR_HPT_MODE

DESCRIPTION   Handles the IOCTL to handle the HDR HPT MODE

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_enable_hdr_hpt_mode
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  boolean                           enable_hdr_hpt_mode = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr != NULL )
  {
    enable_hdr_hpt_mode = ( * (boolean *) argval_ptr );
    if (enable_hdr_hpt_mode == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "707 pkt ioctl - Enable HDR HPT mode ");
      hdrpac_start_high_priority_traffic_mode();
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "707 pkt ioctl - Disable HDR HPT mode ");
      hdrpac_stop_high_priority_traffic_mode();
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ioctl set hdr hybrid mode VT failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_enable_hdr_hpt_mode() */
  #endif /* FEATURE_HDR */

#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_HDR_1X_HANDDOWN

DESCRIPTION   Handles the IOCTL to handle the HDR 1X HANDDOWN

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_hdr_1x_handdown
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl enable handdown failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  *((boolean *)argval_ptr)  =  ds707_pkt_mgr_handoff_1x_handdown;
  
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_hdr_1x_handdown() */
#endif /* FEATURE_HDR */

#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_HDR_1X_HANDDOWN

DESCRIPTION   Handles the IOCTL to handle the HDR 1X HANDDOWN

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_set_hdr_1x_handdown
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl enable handdown failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
#ifdef FEATURE_EHRPD  
  /*----------------------------------------------------------------------
  In eHRPD mode 1x Handdown is not allowed since we dont want immediate 
  handdown to 1x in connected mode. 
  Set Handdown IOCTL will be disabled when in eHRPD mode. 
  ----------------------------------------------------------------------*/
  if (ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "HDR session negotiated with eHRPD personality. "
                     "1x Handdown is not supported");
    *ps_errno = DS_EFAULT;
    return -1;
  }
#endif
  ds707_pkt_mgr_handoff_1x_handdown = ( * (boolean *) argval_ptr );
  
  if(ds707_pkt_mgr_handoff_1x_handdown == TRUE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "707 pkt ioctl - Enable handdown to 1x mode ");
    hdrpac_enable_1x_handdown_mode();
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "707 pkt ioctl - Disable handdown to 1x mode ");
    hdrpac_disable_1x_handdown_mode();
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_set_hdr_1x_handdown() */
#endif /* FEATURE_HDR */


  #ifdef FEATURE_HDR
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_ENABLE_HDR_REV0_RATE_INERTIA

DESCRIPTION   Handles the IOCTL to enable HDR Rev0 Rate Inertia

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_enable_hdr_rev0_rate_inertia
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  boolean                           enable_hdr_rev0_rate_inertia = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  if ( argval_ptr != NULL )
  {
    enable_hdr_rev0_rate_inertia = ( * (boolean *) argval_ptr );
    if (enable_hdr_rev0_rate_inertia == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "707 pkt ioctl - Enable HDR VT params ");
      if ( hdrpac_enable_vt_rate_inertia() != E_SUCCESS )
      {
        *ps_errno = DS_EFAULT;
        return -1;
      }
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "707 pkt ioctl - Disable HDR VT params ");
      if ( hdrpac_disable_vt_rate_inertia() != E_SUCCESS )
      {
        *ps_errno = DS_EFAULT;
        return -1;
      }
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl use hdr VT params failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_hdr_enable_rev0_rate_inertia() */
  #endif /* FEATURE_HDR */

  #ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_QOS_REQUEST

DESCRIPTION   Handles the IOCTL for QoS Request

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_qos_request
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_iface_ioctl_qos_request_type   *qos_ioctl_arg    = NULL;
  ps_flow_type                      *ps_flow_ptr      = NULL;
  int16                              ioctl_errno;
  int                                ret_val; 
  ps_phys_link_type                 *phys_link_ptr;
  ds_cmd_type                       * cmd_ptr = NULL;  
  ds707_sec_pkt_flow_cmd_type       * sec_flow_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if (phys_link_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl fail, phys_link_ptr NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-----------------------------------------------------------------------
  If the QoS is not available, fail IOCTL
  -----------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Current System Does not support QoS");
    *ps_errno = DS_EQOSUNAWARE;
    return(-1);
  }

  /*-----------------------------------------------------------------------
   If the resource is not available, fail IOCTL
  -----------------------------------------------------------------------*/
  if (ds707_pkt_mgr_flow_ioctl_resource_validate( ioctl_name, 
                                                  argval_ptr) == FALSE)
  {
    *ps_errno = DS_ENOMEM; 
    return -1;
  }

  /*-----------------------------------------------------------------------
  Argument pointer validation. If null, fail IOCTL
  -----------------------------------------------------------------------*/
  if (argval_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl qos request failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  /*-----------------------------------------------------------------------
  Create PS flow.
  For QOS_REQUEST, we only support FLOW ACTIVATE but not FLOW CONFIGURE.
  -----------------------------------------------------------------------*/
  qos_ioctl_arg = ( ps_iface_ioctl_qos_request_type *) argval_ptr;
  ps_flow_ptr   = ds707_sec_pkt_mgr_create_flow(qos_ioctl_arg->qos_ptr,
                                                qos_ioctl_arg->subset_id,
                                                iface_ptr,
                                                FALSE,
                                                PS_IFACE_IOCTL_QOS_REQUEST_OP);
  if (ps_flow_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl qos request failed. Couldn't create flow");
    *ps_errno = DS_EFAULT;
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  /*----------------------------------------------------------------------
  The flow should be in NULL state. If the flow is not in NULL state,
  there is no recovery. So, ASSERT this condition
  --------------------------------------------------------------------*/
  if (PS_FLOW_GET_STATE(ps_flow_ptr) != FLOW_NULL)
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR, "0x%x in %d state, not in NULL STATE",
                     ps_flow_ptr, PS_FLOW_GET_STATE(ps_flow_ptr));
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    ASSERT(0);
    return -1;
  }
  /*---------------------------------------------------------------------
  Activate the flow
  -----------------------------------------------------------------------*/
  ret_val =  ps_flow_activate_cmd ( ps_flow_ptr, &ioctl_errno,NULL );

  ASSERT(((ret_val == -1) && (ioctl_errno == DS_EWOULDBLOCK) ));
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  qos_ioctl_arg->flow_ptr = ps_flow_ptr;

  /*----------------------------------------------------------------------
  Get the command buffer. Make sure that if the command is obtained
  successfully, it must post the command. 
  
      **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
  ----------------------------------------------------------------------*/
  cmd_ptr = 
    ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
  if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
  {
    ds707_pkt_mgr_flow_ioctl_delete_flows(&ps_flow_ptr, 
                                          1,
                                          iface_ptr );
    *ps_errno = DS_ENOMEM;
    ASSERT(0);
    return (-1);
  }
  
  sec_flow_cmd_ptr = 
    (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;

  memset(sec_flow_cmd_ptr, 
         0, 
         sizeof(ds707_sec_pkt_flow_cmd_type));

  cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_ACTIVATE_CMD;
  sec_flow_cmd_ptr->num_of_flows = 1;
  sec_flow_cmd_ptr->client_data_ptr = NULL;
  sec_flow_cmd_ptr->flow_ptr[0] = ps_flow_ptr;
  sec_flow_cmd_ptr->ps_iface_ptr = iface_ptr;

  ds_put_cmd(cmd_ptr);

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "QOS_REQUEST processed");

  return 0;
} /* ds707_pkt_mgr_iface_ioctl_qos_request() */
  #endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS 
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_QOS_REQUEST_EX

DESCRIPTION   Handles the IOCTL for a bundled QoS Request

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_qos_request_ex
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_iface_ioctl_qos_request_ex_type *qos_ioctl_arg_ex = NULL;
  int16                               ioctl_errno;
  ps_phys_link_type                  *phys_link_ptr;
  ds_cmd_type                        *cmd_ptr = NULL;  
  uint8                               i;
  ds707_sec_pkt_flow_cmd_type       * sec_flow_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if (phys_link_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl fail, phys_link_ptr NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------
  If the QoS is not available, fail IOCTL
  --------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Current System Does not support QoS");
    *ps_errno = DS_EQOSUNAWARE;
    return(-1);
  }
  /*-----------------------------------------------------------------------
  If the resource is not available, fail the IOCTL
  -----------------------------------------------------------------------*/
  if (ds707_pkt_mgr_flow_ioctl_resource_validate( ioctl_name, 
                                                  argval_ptr) == FALSE)
  {
    *ps_errno = DS_ENOMEM; 
    return -1;
  }

  if (argval_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl qos request failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section); 
  /*-----------------------------------------------------------------------
  Create flows
  -----------------------------------------------------------------------*/
  qos_ioctl_arg_ex = ( ps_iface_ioctl_qos_request_ex_type *) argval_ptr;
  if (ds707_pkt_mgr_flow_ioctl_create_flows( qos_ioctl_arg_ex,
                                             iface_ptr) == FALSE)
  {
    /*-------------------------------------------------------------------
    If Create flows fails, then fail the IOCTL
    ---------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl qos request failed. Couldn't create flow");
    *ps_errno = DS_EFAULT;
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  if (qos_ioctl_arg_ex->opcode == PS_IFACE_IOCTL_QOS_REQUEST_OP)
  {
    /*-----------------------------------------------------------------------
    Activate the flows
    -----------------------------------------------------------------------*/
    ds707_pkt_mgr_flow_ioctl_flows_activate_cmd(
                                             iface_ptr,
                                             qos_ioctl_arg_ex->flows_ptr, 
                                             qos_ioctl_arg_ex->num_qos_specs,
                                             &ioctl_errno,
                                             NULL );
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /*----------------------------------------------------------------------
    Get the command buffer. Make sure that if the command is obtained
    successfully, it must post the command. 
    
        **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
    ----------------------------------------------------------------------*/
    cmd_ptr = 
      ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );

    if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
    {
      ds707_pkt_mgr_flow_ioctl_delete_flows(
                                     qos_ioctl_arg_ex->flows_ptr, 
                                     qos_ioctl_arg_ex->num_qos_specs,
                                     iface_ptr );
      *ps_errno = DS_ENOMEM;
      ASSERT(0);
      return (-1);
    }

    sec_flow_cmd_ptr = 
      (ds707_sec_pkt_flow_cmd_type *) cmd_ptr->cmd_payload_ptr;
    memset(sec_flow_cmd_ptr, 
           0, 
           sizeof(ds707_sec_pkt_flow_cmd_type));
    
    cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_ACTIVATE_CMD;
    sec_flow_cmd_ptr->num_of_flows = qos_ioctl_arg_ex->num_qos_specs;
    sec_flow_cmd_ptr->client_data_ptr = NULL;
    sec_flow_cmd_ptr->ps_iface_ptr = iface_ptr;

    for (i = 0; i<qos_ioctl_arg_ex->num_qos_specs; i++)
    {
      sec_flow_cmd_ptr->flow_ptr[i] = qos_ioctl_arg_ex->flows_ptr[i];
    }

    ds_put_cmd(cmd_ptr);
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "QOS_REQUEST_EX processed");

  }
  else if (qos_ioctl_arg_ex->opcode == PS_IFACE_IOCTL_QOS_CONFIGURE_OP)
  {
    /*-----------------------------------------------------------------------
    Activate the flows
    -----------------------------------------------------------------------*/
    ds707_pkt_mgr_flow_ioctl_flows_configure_cmd(
                                             iface_ptr,
                                             qos_ioctl_arg_ex->flows_ptr, 
                                             qos_ioctl_arg_ex->num_qos_specs,
                                             &ioctl_errno,
                                             NULL );
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /*----------------------------------------------------------------------
     Get the command buffer. Make sure that if the command is obtained
    successfully, it must post the command. 
    
        **** DO NOT RETURN BEFORE CALLING DS_PUT_CMD() ****
    ----------------------------------------------------------------------*/

    cmd_ptr = ds_allocate_cmd_buf( sizeof(ds707_sec_pkt_flow_cmd_type) );
    if ( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) ) 
    {
      ds707_pkt_mgr_flow_ioctl_delete_flows(
                                     qos_ioctl_arg_ex->flows_ptr, 
                                     qos_ioctl_arg_ex->num_qos_specs,
                                     iface_ptr );
      *ps_errno = DS_ENOMEM;
      ASSERT(0);
      return -1;
    }

    sec_flow_cmd_ptr = (ds707_sec_pkt_flow_cmd_type*) cmd_ptr->cmd_payload_ptr; 
    memset(sec_flow_cmd_ptr, 0, sizeof(ds707_sec_pkt_flow_cmd_type));

    cmd_ptr->hdr.cmd_id = DS_CMD_707_SEC_PKT_FLOW_CONFIGURE_CMD;
    sec_flow_cmd_ptr->num_of_flows = qos_ioctl_arg_ex->num_qos_specs;
    sec_flow_cmd_ptr->client_data_ptr = NULL;
    sec_flow_cmd_ptr->ps_iface_ptr = iface_ptr;

    for (i = 0; i<qos_ioctl_arg_ex->num_qos_specs; i++)
    {
      sec_flow_cmd_ptr->flow_ptr[i] = qos_ioctl_arg_ex->flows_ptr[i];
    }
    ds_put_cmd(cmd_ptr);
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "QOS_CONFIGURE_EX processed");
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED, "Invalid opcode :0x%x",
                     qos_ioctl_arg_ex->opcode);
    *ps_errno = DS_EFAULT;
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1; 
  }

  return 0;
} /* ds707_pkt_mgr_iface_ioctl_request_ex() */
  #endif /* FEATURE_HDR_QOS */

  #ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM

DESCRIPTION   Handles the IOCTL Query if the system is on QoS Aware system

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_on_qos_aware_system
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_iface_ioctl_on_qos_aware_system_type *ps_qos_aware_system = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if (argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl qos request failed. Null arg ptr");
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    return -1;
  }

  ps_qos_aware_system = 
  (ps_iface_ioctl_on_qos_aware_system_type *) argval_ptr;

  /*-------------------------------------------------------------------
    If the QoS is not available, return
  --------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    *ps_qos_aware_system = FALSE;
  }
  else
  {
    *ps_qos_aware_system = TRUE;
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_on_qos_aware_system() */
  #endif /* FEATURE_HDR_QOS */

  #ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES

DESCRIPTION   Handles the IOCTL query to return the Network supported QoS
              Profiles

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_network_supported_qos_profiles
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  uint8 i;
  hdrmrlpcif_an_supported_qos_profile_type an_supported_profiles;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ArgValPtr Passed");
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    return -1;
  }

  /*-------------------------------------------------------------------
  If the QoS is not available, fail IOCTL
  --------------------------------------------------------------------*/
  if ( ds707_sec_pkt_is_qos_available_on_system() == FALSE )
  {
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EQOSUNAWARE;
    }
    return(-1);
  }

  /*-------------------------------------------------------------------
  Read the ANSupported profiles
  --------------------------------------------------------------------*/
  ds707_sec_pkt_mgr_get_an_sup_prof(&an_supported_profiles);

  ((dss_iface_ioctl_get_network_supported_qos_profiles_type *)argval_ptr)->profile_count = 
                                         an_supported_profiles.profile_count;

  if (an_supported_profiles.profile_count == 0)
  {
    /*----------------------------------------------------------------------
    If profile count is ZERO, then AT has not negotiated the profile. This
     is a true condition
    -----------------------------------------------------------------------*/
    return 0;
  }

  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "AN supported profiles count = %d",
                   an_supported_profiles.profile_count);

  for (i = 0; i<an_supported_profiles.profile_count ; i++)
  {
    ((dss_iface_ioctl_get_network_supported_qos_profiles_type *)argval_ptr)->profile_value[i] = 
    an_supported_profiles.profiles[i];
  }
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_network_supported_qos_profiles() */
  #endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_BORDER_RF_CONDITIONS

DESCRIPTION   Handles the IOCTL query to return the border RF conditions

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
/* ARGSUSED */
LOCAL int ds707_pkt_mgr_iface_ioctl_get_border_rf_conditions
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( argval_ptr == NULL )
  {
     DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ArgValPtr Passed");
     if(ps_errno != NULL)
     {
       *ps_errno = DS_EFAULT;
     }
     return -1;
  }

  /*-------------------------------------------------------------------
  Read the border RF conditions
  --------------------------------------------------------------------*/
  ((dss_iface_ioctl_rf_conditions_type *)argval_ptr)->rf_conditions = 
                        ds707_sec_pkt_mgr_handoff_get_border_rf_condition();
  (void) ds707_pkt_mgr_get_bearer_tech_info(
          &((dss_iface_ioctl_rf_conditions_type *)argval_ptr)->bearer_tech);
  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_rf_border_conditions() */
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR
#ifdef FEATURE_EIDLE_SCI
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_ENABLE_HDR_SLOTTED_MODE

DESCRIPTION   Handles the IOCTL to enable HDR Slotted Mode

DEPENDENCIES  

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_enable_hdr_slotted_mode
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  uint32                             requested_sm_override;
  hdrerrno_enum_type                 sm_ps_errno;
  hdrcp_scmidle_sm_override_enum_type   slotted_mode_val;
  ps_iface_ioctl_enable_hdr_slotted_mode * slotted_mode_type_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "707 pkt ioctl - enable slot cycle index params");
  if ( argval_ptr != NULL )
  {
    slotted_mode_type_ptr = 
            (ps_iface_ioctl_enable_hdr_slotted_mode *)argval_ptr;
    if ((slotted_mode_type_ptr->get_slotted_mode) == 1)
    {
      /* intent is to get current slotted mode */
      slotted_mode_val = hdrpac_get_current_max_configured_sci();
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                       "Slotted Mode - Returning current SCI: %d ", 
                       slotted_mode_val);
      slotted_mode_type_ptr->slotted_mode_option = (uint32) slotted_mode_val;
      if (slotted_mode_val == HDRSCMIDLE_SM_OVERRIDE_INVALID) 
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                         "Slotted Mode - Returning Error: DS_EINVAL");
        *ps_errno = DS_EINVAL;
        return -1;
      }
      else
      {
        *ps_errno = 0;
        return 0;
      }
    }
    else 
    {
      /* intent is to set slotted mode */
      requested_sm_override = slotted_mode_type_ptr->slotted_mode_option;

      sm_ps_errno = hdrpac_change_eidle_slotted_mode(requested_sm_override);

      switch (sm_ps_errno)
      {
        case E_SUCCESS: 
          /* GAUP sent, SCI set as requested, or will be GAUPed ASAP */
          *ps_errno = 0;
          return 0;

        case E_NOT_SUPPORTED: /* Idle State Protocol subtype not EIDLE */
          *ps_errno = DS_EOPNOTSUPP;
          return -1;

        case E_NO_NET: /* HDR Protocol not idle or connected */
          *ps_errno = DS_ENETNONET;
          return -1;

        case E_OUT_OF_RANGE: /* Requested SCI value out of range */
          *ps_errno = DS_EINVAL;
          return -1;

        default: 
          DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                           "Slotted Mode - hdrpac_change_eidle_slotted_mode"
                           "(%d) failed", 
                           requested_sm_override);
          *ps_errno = DS_EFAULT;
          return -1;
      }

      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Enable HDR slot cycle index params. "
                      "Returning Error: %d", *ps_errno);
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Slotted Mode - ioctl use hdr slot "
                    "cycle option failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
} /* ds707_pkt_mgr_iface_ioctl_enable_hdr_slotted_mode() */
#endif /* FEATURE_EIDLE_SCI */
#endif /* FEATURE_HDR */

#ifdef FEATURE_IS707B_ADDENDUM
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_HAT_TIMER

DESCRIPTION   Handles the IOCTL to set the Hysteresis Activation Timer (HAT)
    
PARAMETERS    argval_ptr: Pointer to timer value of type uint8
              that contains the new timer value to set.
DEPENDENCIES  

RETURN VALUE   0 if command is successfully sent to DS_EPZID_MGR
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_set_hat_timer
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  unsigned int          new_hat_val;
  ds_cmd_type          *cmd_ptr;
  ds707_epzid_hat_type *epzid_hat_ptr = NULL;

  *ps_errno = 0;

  if ( argval_ptr != NULL )
  {
    new_hat_val = (* (unsigned int *) argval_ptr );
  }
   else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl SET HAT failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "707 pkt ioctl EPZID HYST SET HAT =%d", 
                   new_hat_val);

  if ( new_hat_val >  30  )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_epzid_hat_type));
  if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    *ps_errno = DS_ENOMEM;
    return(-1);
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_MGR_SET_HAT_TIMER;
    epzid_hat_ptr = (ds707_epzid_hat_type*)cmd_ptr->cmd_payload_ptr;
    epzid_hat_ptr->timer_val = new_hat_val;
    ds_put_cmd(cmd_ptr);
  }
  return 0;
}
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_HAT_TIMER

DESCRIPTION   Handles the IOCTL to Get the Status of Hysteresis Activation Timer (HAT)
    
PARAMETERS    argval_ptr: Pointer to timer value of type uint8
              that contains the new timer value to set.
              
DEPENDENCIES  

RETURN VALUE   0 if command is successfully sent to DS_EPZID_MGR
                        -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_get_hat_timer
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  int rem_time_to_expire_hat = -1;
 
 *ps_errno = 0;

  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   " HAT is currently Set to %d Seconds", 
                   ds707_epzid_hysteresis_get_hat_timer_in_sec());

  if( argval_ptr != NULL )
  {
     rem_time_to_expire_hat = 
                       (int)ds707_epzid_get_hat_time_to_expire_in_sec();
     if(rem_time_to_expire_hat == 0)
     { 
       rem_time_to_expire_hat = -1;
     }

     DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                     "HAT: Remaining time to expire = %d (running if > 0)",
                     rem_time_to_expire_hat);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl GET HAT failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  (*(int *)argval_ptr ) = rem_time_to_expire_hat;
  return 0;
}
#endif /* FEATURE_IS707B_ADDENDUM */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_DATA_CALL_TYPE

DESCRIPTION   This function indicates if an embedded data call is UP or not 
              and if UP indicates whether it is a SIP or MIP call. This 
              function supports only embedded call status and not DUN call 
              status.
    
PARAMETERS
  iface_ptr  - IFACE for which data call status (SIP/MIP UP or NOT) needs to be 
               indicated.
  argval_ptr - Pointer in which data call status (SIP/MIP UP or NOT) needs to 
               be stored.
  ps_errno   - Pointer in which specific error reason needs to be indicated in 
               case of failures.
              
DEPENDENCIES  None

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reasons are applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.
  DS_EOPNOTSUPP - Indicates system mode is non-C2K, in which case, get SIP/MIP 
                  status is not applicable and so operation not supported 
                  error is informed. Also if IFACE is not a V4 IFACE, this 
                  error is informed.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_get_data_call_type
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_iface_ioctl_data_call_enum_type *data_call_type = NULL;
  sys_sys_mode_e_type                 curr_network_mode = SYS_SYS_MODE_NONE;
  dssnet4_sm_cb_type                 *dssnet4_instance = NULL;
  dssnet6_sm_cb_type                 *dssnet6_instance = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    If argval pointer or errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(ps_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return -1;
  }

  if( (iface_ptr == NULL) || (argval_ptr == NULL) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------
    If sys mode is not 1x/HDR, then return failure.
  --------------------------------------------------------------------*/
  curr_network_mode = ds3g_get_current_network_mode();

  if( (curr_network_mode != SYS_SYS_MODE_CDMA) && 
      (curr_network_mode != SYS_SYS_MODE_HDR) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
               "Current sys mode is not CDMA/HDR. Operation not supported.");
    *ps_errno = DS_EOPNOTSUPP;
    return -1;
  }

  data_call_type = (ps_iface_ioctl_data_call_enum_type *) argval_ptr;
  if( ds707_drs_is_in_mip_call() )
  {
    *data_call_type = PS_IFACE_IOCTL_DATA_CALL_TYPE_MIP;
  }
  else
  {
    if( ps_iface_addr_family_is_v4(iface_ptr) )
    {
      dssnet4_instance = ds707_pkt_mgr_get_dssnet4_sm_cb(iface_ptr);
    }
    else if( ps_iface_addr_family_is_v6(iface_ptr) )
    {
      dssnet6_instance = ds707_pkt_mgr_get_dssnet6_sm_cb(iface_ptr);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "IFACE on which get SIP/MIP status requested is not a V4 \
                      or V6 IFACE. Hence operation not supported.");
      *ps_errno = DS_EOPNOTSUPP;
      return -1;
    }
  }

  if( dssnet4_instance != NULL )
  {
    if( dssnet4_instance->state != DSSNET4I_NET_UP_STATE )
    {
      *data_call_type = PS_IFACE_IOCTL_DATA_CALL_TYPE_NONE;
    }
    else
    { 
      *data_call_type = PS_IFACE_IOCTL_DATA_CALL_TYPE_SIP;
    }
  }

  if( dssnet6_instance != NULL )
  {
    if( dssnet6_instance->state != DSSNET6I_NET_UP_STATE )
    {
      *data_call_type = PS_IFACE_IOCTL_DATA_CALL_TYPE_NONE;
    }
    else
    {
      *data_call_type = PS_IFACE_IOCTL_DATA_CALL_TYPE_SIP;
    }
  }

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Current call type is %d", *data_call_type);

  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_data_call_type() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_DATA_COUNT

DESCRIPTION   Handles the IOCTL to get the data count (txed/rxed bytes)
    
PARAMETERS    argval_ptr: Pointer to data count struct type
              
DEPENDENCIES  

RETURN VALUE -1: Failure
              0: Success

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_get_data_count
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  uint32 rxed_bytes, txed_bytes;
  ps_iface_ioctl_data_count_type *data_count_type = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ps_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ioctl get data count failed. Null arg ptr");
    return -1;
  }

  if (argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ioctl get data count failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  
  if( ds707_pkt_get_data_count( &rxed_bytes, &txed_bytes) != TRUE )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }
  
  data_count_type = (ps_iface_ioctl_data_count_type *) argval_ptr;
  data_count_type->rxed_bytes = rxed_bytes;
  data_count_type->txed_bytes = txed_bytes;
  return 0;
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_HDR_AN_USER_ID

DESCRIPTION   Handles the IOCTL to set the HDR AN user id
    
PARAMETERS    argval_ptr: Pointer to struct
              
DEPENDENCIES  

RETURN VALUE -1: Failure
              0: Success

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_set_hdr_an_user_id
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_iface_ioctl_hdr_an_user_id_type *user_id_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ps_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ioctl set hdr an user id type failed. Null ps_errno");
    return -1;
  }

  if (argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ioctl set hdr an user id type failed. Null arg ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }
 
  user_id_ptr = (ps_iface_ioctl_hdr_an_user_id_type *)argval_ptr;
#ifdef FEATURE_HDR
  if( dshdr_an_set_user_id( user_id_ptr->user_id,
                            user_id_ptr->user_id_len ) != 0 )
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }
#endif /* FEATURE_HDR */
  return 0;
}

/*===========================================================================
                       PHYS LINK HANDLER FUNCTIONS 
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PHYS_LINK_IOCTL_GO_ACTIVE

DESCRIPTION   Handles the IOCTL to bring up the traffic channel

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
/*lint -save -e715 unused variables*/
int ds707_pkt_mgr_phys_link_ioctl_go_active
(
  ps_phys_link_type        *phys_link_ptr,
  void                     *argval_ptr,
  int16                    *ps_errno
)
{
  ds3g_pkt_call_info_type  pkt_info;
  int16                     ioctl_errno;
  int16                     ret_val;
  ds707_tc_state_type      *tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  tc_state_ptr  = GET_TC_FROM_PHYS_LINK_PTR(phys_link_ptr);

#ifdef FEATURE_DATA_IS707_ASYNC  
  if (ds707_so_async_so_is_async(tc_state_ptr->so) == TRUE)
  {
    ERR ("ioctl goactive fail. Async so %d", tc_state_ptr->so, 0, 0 );
    *ps_errno = DS_EFAULT;
    return -1;
  }
#endif /* FEATURE_DATA_IS707_ASYNC */

  /*---------------------------------------------------------------------
    This struct is passed onto the iface.
  ---------------------------------------------------------------------*/
  pkt_info.drs_bit         = TRUE;
  pkt_info.use_hdown_timer = TRUE;
  pkt_info.new_network     = ds707_pkt_get_current_data_sess_network();
  /*-------------------------------------------------------------------
    For Non TE Based origination force_cdma_only should always be FALSE
  -------------------------------------------------------------------*/
  pkt_info.force_cdma_only = FALSE; 

  /*-------------------------------------------------------------------
    Set the incoming call flag to FALSE
  -------------------------------------------------------------------*/
  pkt_info.inc_call        = FALSE; 

  /*-------------------------------------------------------------------
   Set the epzid flag to FALSE for all Origination attempts
  -------------------------------------------------------------------*/
  pkt_info.epzid           = FALSE;


  if (ds707_pkt_is_dormant(phys_link_ptr) == TRUE)
  {
    if ( ps_phys_link_get_flow_mask (phys_link_ptr) == DS_FLOW_IS_ENABLED )
    {
      ret_val = (int16)ps_phys_link_up_cmd ( phys_link_ptr,
                                      &ioctl_errno,
                                      &pkt_info );
      if ((ret_val < 0) && ( ioctl_errno != DS_EWOULDBLOCK))
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                        "ioctl goactive fail,%d phys_link ret",
                        ioctl_errno);
        *ps_errno = DS_EFAULT;
        return -1;
      }
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "ioctl goactive fail, "
                      "flow was disabled with mask %d", 
                      ps_phys_link_get_flow_mask (phys_link_ptr));
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }
  else
  {
    if ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING) ||
        (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP) ||
        (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP)
        )
    {
      DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                       "ioctl goactive success phys %d, so%d",
                       PS_PHYS_LINK_GET_STATE(phys_link_ptr),
                       tc_state_ptr->so);
 

      /* return success */
      return 0;
    }

    DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                     "ioctl goactive fail phys %d, so%d",
                     PS_PHYS_LINK_GET_STATE(phys_link_ptr),
                     tc_state_ptr->so);
           
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /* return success by default */
  return 0;
} /* ds707_pkt_mgr_phys_link_ioctl_go_active() */


/*===========================================================================
FUNCTION      DS707_PKT_MGR_PHYS_LINK_IOCTL_go_dormant

DESCRIPTION   Handles the IOCTL to bring down the traffic channel

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
/*ARGSUSED*/
int ds707_pkt_mgr_phys_link_ioctl_go_dormant
(
  ps_phys_link_type        *phys_link_ptr,
  void                     *argval_ptr,
  int16                    *ps_errno
)
{
  int16                     ioctl_errno;
  int16                     ret_val;
  ds707_tc_state_type      *tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(argval_ptr);

  if (0 != ds707_get_current_tx_wmk_cnt() || 
      0 != dshdr_an_get_current_tx_wmk_cnt())
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "707 pkt ioctl - go dormant: SN/AN TX Wmk nonempty. Not "
                     "going dormant"); 
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    This check is to avoid aborting a resuming phys link. Ideally tx wmk
    won't be empty when phys link goes to resuming. However there might be
    cases where Modem wants to bring up traffic channel for some purpose
    without data being queued into the wmk
  -------------------------------------------------------------------------*/
  if (PHYS_LINK_RESUMING == PS_PHYS_LINK_GET_STATE(phys_link_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "707 pkt ioctl - go dormant: Phys link in resuming "
                     "state. Not going dormant");
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Note: There is a possibility that the above two checks didn't catch a
    rare case where one of the protocols (e.g PPP) is performing some
    activity. In that case, we rely on the protocol's retry/recovery
    capability to complete its procedure.
  -------------------------------------------------------------------------*/

  tc_state_ptr  = GET_TC_FROM_PHYS_LINK_PTR(phys_link_ptr);
  if ( ds707_pkti_can_go_dormant ( tc_state_ptr, phys_link_ptr ) == TRUE )
  {
    ret_val = (int16)ps_phys_link_down_cmd(
                                   phys_link_ptr,
                                   &ioctl_errno,
                                   NULL
                                   );

    if ((ret_val < 0) && ( ioctl_errno != DS_EWOULDBLOCK))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "ioctl godorm fail, %d phys_link ret",
                      ioctl_errno);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "ioctl godorm fail phys = %d, so = %d",
                    PS_PHYS_LINK_GET_STATE(phys_link_ptr), tc_state_ptr->so);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /* return success by default */
  return 0;
} /* ds707_pkt_mgr_phys_link_ioctl_go_dormant() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_SESSION_TIMER

DESCRIPTION   Handles the IOCTL to return the GO NULL Timer (Session Timeout)
    
PARAMETERS    argval_ptr: Pointer to an enmerated type of 
              dss_session_timer_type with the timer_select field corresponding
              to select either the 1X timer value, DO timer Value. If Both is specified
              an error is returned.

DEPENDENCIES  

RETURN VALUE   Timer value in minutes on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int16 ds707_pkt_mgr_iface_ioctl_get_session_timer
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  dss_session_timer_type * dss_session_timer_ptr;
  ds707_gonulltmri_config_enum_type timer_select_value = DS707_GONULLTMRI_DO;
  int16 ret_val = -1;
  *ps_errno = 0;
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  if (argval_ptr == NULL)
  {
    return ret_val;
  }
  dss_session_timer_ptr = (dss_session_timer_type *) argval_ptr;

  switch (dss_session_timer_ptr->timer_select)
  {
    case DSS_SESSION_TIMER_1X:
      {
        timer_select_value = DS707_GONULLTMRI_1X;
        break;
      }
    case DSS_SESSION_TIMER_DO:
      {
        timer_select_value = DS707_GONULLTMRI_DO;
        break;
      }
    case DSS_SESSION_TIMER_1X_AND_DO:
      {
        timer_select_value = DS707_GONULLTMRI_1X_AND_DO;
        break;
      }
    default:
      {
        DATA_IS707_MSG1( MSG_LEGACY_MED,
                         "Unknown GO NULL Timer select value %d",
                         dss_session_timer_ptr->timer_select);
        return ret_val;
      }
  }
  ret_val =  ds707_gonulltmr_get_timer(timer_select_value);

  return ret_val;
}


/*===========================================================================
FUNCTION     DS707_PKT_MGR_IFACE_IOCTL_SET_SESSION_TIMER

DESCRIPTION  Handles the IOCTL to set the GO NULL Timer (Session Timeout)
    
PARAMETERS   argval_ptr: Pointer to a dss_session_timer_type structure
             that contains the new timer value to set and the value to select
             either 1X or DO or Both timer values to be set to.
DEPENDENCIES  

RETURN VALUE   Timer value in minutes on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_set_session_timer
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  dss_session_timer_type * dss_session_timer_ptr;
  ds707_gonulltmri_config_enum_type timer_select_value;
  int ret_val = -1;
  *ps_errno = 0;
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if (argval_ptr == NULL)
  {
    return ret_val;
  }

  dss_session_timer_ptr = (dss_session_timer_type *) argval_ptr;

  switch (dss_session_timer_ptr->timer_select)
  {
    case DSS_SESSION_TIMER_1X:
      {
        timer_select_value = DS707_GONULLTMRI_1X;
        break;
      }
    case DSS_SESSION_TIMER_DO:
      {
        timer_select_value = DS707_GONULLTMRI_DO;
        break;
      }
    case DSS_SESSION_TIMER_1X_AND_DO:
      {
        timer_select_value = DS707_GONULLTMRI_1X_AND_DO;
        break;
      }
    default:
      {
        DATA_IS707_MSG1( MSG_LEGACY_MED, 
                         "Unknown GO NULL Timer select value %d",
                         dss_session_timer_ptr->timer_select);
        timer_select_value = DS707_GONULLTMRI_1X_AND_DO;
      }
  }

  ret_val = ds707_gonulltmr_set_timer(
            timer_select_value , 
            dss_session_timer_ptr->timer_val
                                     );

  return ret_val;
}

/* Revisit the featurization later as DHCP and MIPV6 should not be coupled together.
   MIPV6 is a client of DHCP
*/
#ifdef FEATURE_DATA_PS_MIPV6
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_MIP6_BOOTSTRAP_PARAMS

DESCRIPTION   Handles the IOCTL to set the mip6 bootstrap information
    
PARAMETERS    argval_ptr: Pointer to a mip6 bootstrap info type structure
              that contains the nai, nai length, bootstrap option type and 
              hnid vals
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_set_mip6_bootstrap_params
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  int ret_val = -1;
  ds_1x_mip6_profile_type* mip6_profile_type_ptr = NULL;


  if (argval_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Error in setting mip6 bootstrap param");
    return ret_val;
  }

  mip6_profile_type_ptr = (ds_1x_mip6_profile_type *) argval_ptr;

  /*-------------------------------------------------------------------------
    set the MIPv6 params in the data1x profiles
  -------------------------------------------------------------------------*/

  ret_val = dshdr_dhcp_set_mip6_profile (mip6_profile_type_ptr);

  if (ret_val == -1)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "Error in setting mip6 bootstrap param");
  }
  else 
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,
                     "mip6 bootstrap param set successfully");
  }
  return ret_val;
}
#endif /* FEATURE_DATA_PS_MIPV6 */
#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_EPC_HANDOFF_CTXT

DESCRIPTION   Handles the IOCTL to get epc handoff ctxt
    
PARAMETERS    argval_ptr: Pointer to acl_handoff_ctxt_type that will be 
              updated with IP config details (IP addr, DNS, SIP Serv)
              for this iface and the companion iface (in case of dual IP)
              and bearer type (single IP or dual IP bearer)
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_get_epc_handoff_ctxt
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  int ret_val = -1;

  if (ps_errno == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_errno ptr");
    return -1;
  }
  if (!PS_IFACE_IS_VALID(iface_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid IFACE ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (argval_ptr == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid argval ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ret_val = ds707_pdncntx_get_epc_handoff_ctxt(iface_ptr,argval_ptr);

  if (ret_val == -1)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,"Error in getting epc handoff ctxt");
  }
  else 
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,"get epc handoff ctxt successful");
  }
  return ret_val;
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_SET_EPC_HANDOFF_CTXT

DESCRIPTION   Handles the IOCTL to set epc handoff ctxt
    
PARAMETERS    argval_ptr: Pointer to acl_handoff_ctxt_type that contains IP
              config details (IP addr, DNS, SIP Serv) for this iface and the
              companion iface (in case of dual IP)
 
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
static int ds707_pkt_mgr_iface_ioctl_set_epc_handoff_ctxt
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
   int ret_val = -1;

  if (ps_errno == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_errno ptr");
    return -1;
  }
  if (!PS_IFACE_IS_VALID(iface_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid IFACE ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (argval_ptr == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid argval ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ret_val = ds707_pdncntx_set_epc_handoff_ctxt(iface_ptr,argval_ptr);

  if (ret_val == -1)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,"Error in setting epc handoff ctxt");
  }
  else 
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,"set epc handoff ctxt successful");
  }
  return ret_val;

} /* ds707_pkt_mgr_iface_ioctl_set_epc_handoff_ctxt */

#endif /* FEATURE_EPC_HANDOFF */

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_RECONFIG

DESCRIPTION   Handles the IOCTL to reconfigure (resync) NCP connection
    
PARAMETERS    argval_ptr: None
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_reconfig
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  if (ps_errno == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_errno ptr, fail ioctl");
    return -1;
  }
  if (!PS_IFACE_IS_VALID(iface_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid IFACE ptr, fail ioctl");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "processing 707 pkt ioctl - reconfig for IFACE 0x%x:%d",
                   iface_ptr->name, iface_ptr->instance);

  /*-------------------------------------------------------------------------
    This IOCTL is allowed only if the IFACE is in UP or ROUTEABLE state.
  -------------------------------------------------------------------------*/
  if ( !(ps_iface_state(iface_ptr) & (IFACE_UP | IFACE_ROUTEABLE)) )
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "IFACE 0x%x:%d, is not UP or ROUTEABLE",
                     iface_ptr->name, iface_ptr->instance);
    *ps_errno = DS_EFAULT;
    return -1; 
  }

  /*---------------------------------------------------------------------------
    Set the PCMT value to eHRPD standalone value. For the case of e->L handoff 
    failure, EPC will resync SRAT (eHRPD). As part of resync, PCM timer value 
    needs to be set back to eHRPD value. This is necessary as PCM timer value 
    was set to IRAT value when LTE attach init event was received and not after 
    LTE attach was completed. So for handoff failure, PCM timer value needs to 
    be reconfigured back to eHRPD value so that PCM timer expires as per eHRPD
    requirements.
  ---------------------------------------------------------------------------*/
  ds707_devmgr_set_pcmt_to_ehrpd_val();

  if (-1 == ds707_pdncntx_iface_reconfig(iface_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "Can't resync NCP, fail ioctl");
    *ps_errno = DS_EFAULT;
    return -1;
  } 

  DATA_IS707_MSG0( MSG_LEGACY_MED,"reconfig ioctl successful");
  return 0;
}
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_V6_REM_RTR_LIFETIME

DESCRIPTION   Handles the IOCTL to retrive remaining IPv6 router lifetime.
    
PARAMETERS    iface_ptr: iface associated IPv6 PDN 
              argnal_ptr: void pointer to return value
              ps_errno: to indicate any error
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
static int ds707_pkt_mgr_iface_ioctl_get_v6_rem_rtr_lifetime
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  if (ps_errno == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_errno ptr, fail ioctl");
    return -1;
  }

  if (NULL == argval_ptr)
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (!PS_IFACE_IS_VALID(iface_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid IFACE ptr, fail ioctl");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "processing 3GPP2 IOCTL - get remaining router lifetime "
                   "for IFACE 0x%x:%d",
                   iface_ptr->name, iface_ptr->instance);

  if (-1 == ds707_pdncntx_get_v6_rem_rtr_lifetime(iface_ptr, argval_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Get V6 remaining rtr lifetime failed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  return 0;

} /* ds707_pkt_mgr_iface_ioctl_get_v6_rem_rtr_lifetime */
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
FUNCTION      DS707_PKT_MGR_SYS_IOCTL_PREREG_HANDOFF

DESCRIPTION   Handles the IOCTL to start prereg handoff
    
PARAMETERS    argval_ptr: None
              
DEPENDENCIES  

RETURN VALUE   0 on success
              -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
static int ds707_pkt_mgr_sys_ioctl_prereg_handoff
(
  void                     *argval_ptr,
  int16                    *ps_errno
)
{
  ds_cmd_type                       *cmd_ptr = NULL;
  ds707_s101_process_cmd_info_type  *cmd_info_ptr = NULL;

  if(NULL == argval_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Invalid arg passed");
    *ps_errno = DS_EFAULT;
    return(-1);
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_s101_process_cmd_info_type));
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    *ps_errno = DS_ENOMEM;
    return(-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_707_S101_COMPLETE_PREREG_HANDOFF;
  cmd_info_ptr = (ds707_s101_process_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  cmd_info_ptr->prereg_handoff_info = 
    *((ps_sys_ioctl_3gpp2_prereg_handoff_complete_cmd_type*) argval_ptr);

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "Posting DS707_S101_COMPLETE_PREREG_HANDOFF_CMD to DS");
  ds_put_cmd(cmd_ptr);

  return 0;

} /* ds707_pkt_mgr_sys_ioctl_prereg_handoff */
#endif /* FEATURE_DATA_OPTHO */

/*===========================================================================
FUNCTION      DS707_SYS_IOCTL_DUN_CTRL_RESPONSE

DESCRIPTION   Handles DUN CTRL Entitlement Response 
    
PARAMETERS    argval_ptr, ps_errno
              
DEPENDENCIES  None

RETURN VALUE   0 on success
              -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_sys_ioctl_dun_ctrl_response
(
  void                     *argval_ptr,
  int16                    *ps_errno
)
{
  ds_cmd_type	*cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ps_sys_ioctl_dun_ctrl_response_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return -1;
  }

   ds3gi_stop_timer(DS3G_TIMER_DUN_CTRL_ENTL);
   cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_ENTITLEMENT_CHECK_COMPLETE;

   *((ps_sys_ioctl_dun_ctrl_response_type *)(cmd_ptr->cmd_payload_ptr)) 
                    = *((ps_sys_ioctl_dun_ctrl_response_type*) argval_ptr);
   ds_put_cmd_ext(cmd_ptr);

  return 0;
}

#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_IOCTL_GET_MIP_MA_INFO

DESCRIPTION   Handles the IOCTL to get MIP MA info
    
PARAMETERS    argval_ptr: Pointer to ip_addr_type that will be updated with 
                          MIP MA info for this iface 
              
RETURN VALUE   0 on success
               -1 on Failure
===========================================================================*/
LOCAL int ds707_pkt_mgr_iface_ioctl_get_mip_ma_info
(
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ip_addr_type *mip_ma_addr =  NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ps_errno == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_errno ptr");
    return -1;
  }

  if (argval_ptr == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid argval ptr");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /* Check if MIP is valid */
  if (FALSE == mip_config_valid())
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid MIP");
    *ps_errno = DS_EINVAL;
    return -1;
  }

  mip_ma_addr = (ip_addr_type *)argval_ptr;
  mip_ma_addr->type = IPV4_ADDR;
  mip_ma_addr->addr.v4 = mip_get_ma_addr();

  return 0;
} /* ds707_pkt_mgr_iface_ioctl_get_mip_ma_info() */
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_IOCTL

DESCRIPTION   Called by ps_iface to do ioctls specific to 1x pkt iface.

DEPENDENCIES  Registered once, at startup.

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_ioctl
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  int                               ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "707 pkt ioctl: %d", ioctl_name);
  *ps_errno = 0;


  /*-------------------------------------------------------------------------
    Based on the ioctl name, perform the appropriate actions.
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {
    /*-----------------------------------------------------------------------
      Go Null.
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GO_NULL:
      ret_val = ds707_pkt_mgr_iface_ioctl_go_null ( iface_ptr, 
                                                    argval_ptr, ps_errno );
      break;

      /*---------------------------------------------------------------------
        set ipcp dns negotiation option.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_IPCP_DNS_OPT:
      ret_val = ds707_pkt_mgr_iface_ioctl_ipcp_dns_opt( iface_ptr, 
                                                        argval_ptr,
                                                        ps_errno );
      break;

      /*---------------------------------------------------------------------
        Get QCMDR value.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_GET_MDR:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_mdr( iface_ptr, 
                                                   argval_ptr, 
                                                   ps_errno );
      break;

      /*---------------------------------------------------------------------
        Set QCMDR value.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_SET_MDR:
      ret_val = ds707_pkt_mgr_iface_ioctl_set_mdr( iface_ptr, 
                                                   argval_ptr,
                                                   ps_errno );
      break;

      /*---------------------------------------------------------------------
        Get dorm timer value.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_GET_DORM_TIMER:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_dorm_timer( iface_ptr,
                                                          argval_ptr,
                                                          ps_errno );
      break;

      /*---------------------------------------------------------------------
        Set dorm timer value.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_SET_DORM_TIMER:
      ret_val = ds707_pkt_mgr_iface_ioctl_set_dorm_timer( iface_ptr, 
                                                          argval_ptr,
                                                          ps_errno );
      break;

      /*---------------------------------------------------------------------
        Get RLP current NAK policy.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_rlp_all_curr_nak( iface_ptr,
                                                                argval_ptr,
                                                                ps_errno );
      break;

      /*---------------------------------------------------------------------
        Set RLP current NAK policy.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK:
      ret_val = ds707_pkt_mgr_iface_ioctl_set_rlp_all_curr_nak( iface_ptr,
                                                                argval_ptr,
                                                                ps_errno );
      break;

      /*---------------------------------------------------------------------
        Get RLP default NAK policy.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_rlp_all_def_nak( iface_ptr,
                                                               argval_ptr,
                                                               ps_errno );
      break;

      /*---------------------------------------------------------------------
        Set RLP default NAK policy.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK:
      ret_val = ds707_pkt_mgr_iface_ioctl_set_rlp_all_def_nak( iface_ptr,
                                                               argval_ptr,
                                                               ps_errno );
      break;

      /*---------------------------------------------------------------------
        Get RLP negotiated NAK policy.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_rlp_all_neg_nak( iface_ptr, 
                                                               argval_ptr, 
                                                               ps_errno );
      break;

      /*---------------------------------------------------------------------
        Get RLP current FILL rate.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_GET_RLP_FILL_RATE:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_rlp_fill_rate( iface_ptr,
                                                             argval_ptr, 
                                                             ps_errno );
      break;

      /*---------------------------------------------------------------------
        Set RLP current NAK policy.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_SET_RLP_FILL_RATE:
      ret_val = ds707_pkt_mgr_iface_ioctl_set_rlp_fill_rate( iface_ptr,
                                                             argval_ptr, 
                                                             ps_errno );
      break;

    case PS_IFACE_IOCTL_GET_DATA_BEARER_RATE:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_data_bearer_rate( iface_ptr,
                                                                argval_ptr,
                                                                ps_errno );
      break;

    case PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_bearer_technology( iface_ptr,
                                                                 argval_ptr,
                                                                 ps_errno );
      break;

#ifdef FEATURE_DATA_WLAN_MAPCON
    case PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_bearer_technology_ex( iface_ptr,
                                                                    argval_ptr,
                                                                    ps_errno );
      break;
#endif

#ifdef FEATURE_IS2000_REL_A
      /*---------------------------------------------------------------------
        Get desired QOS non-assured priority adjustment.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_rlp_qos_na_pri( iface_ptr, 
                                                              argval_ptr,
                                                              ps_errno );
      break;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A
      /*---------------------------------------------------------------------
        Set desired QOS non-assured priority adjustment.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI:
      ret_val = ds707_pkt_mgr_iface_ioctl_set_rlp_qos_na_pri( iface_ptr, 
                                                              argval_ptr,
                                                              ps_errno );
      break;

#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_DATA_IS2000_SDB
      /*---------------------------------------------------------------------
        SDB support query.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY:
      ret_val = ds707_pkt_mgr_iface_ioctl_sdb_support_query( iface_ptr,
                                                             argval_ptr,
                                                             ps_errno );
      break;
#endif /* FEATURE_DATA_IS2000_SDB */

      /*---------------------------------------------------------------------
        Enable holddown timer.
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_ENABLE_HOLDDOWN:
      ret_val = ds707_pkt_mgr_iface_ioctl_enable_holddown( iface_ptr,
                                                           argval_ptr,
                                                           ps_errno );
      break;

#ifdef FEATURE_HDR
      /*---------------------------------------------------------------------
        Enable or disable the HDR high Priority Traffic mode
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE:
      ret_val = ds707_pkt_mgr_iface_ioctl_enable_hdr_hpt_mode( iface_ptr,
                                                               argval_ptr,
                                                               ps_errno );
      break;

	/*-----------------------------------------------------------------------
      Get 1x handdown
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_GET_HDR_1X_HANDDOWN_OPTION:
    ret_val = ds707_pkt_mgr_iface_ioctl_get_hdr_1x_handdown( iface_ptr,
                                                             argval_ptr,
                                                             ps_errno );
    break;

    /*-----------------------------------------------------------------------
      Set 1x handdown
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_SET_HDR_1X_HANDDOWN_OPTION:
     ret_val = ds707_pkt_mgr_iface_ioctl_set_hdr_1x_handdown( iface_ptr,
                                                              argval_ptr,
                                                              ps_errno );
    break;
#endif /* FEATURE_HDR */ 

#ifdef FEATURE_HDR
      /*---------------------------------------------------------------------
        Set the mode for operating VT on a Rev 0 HDR system. This will end up
        sending a GAUP message to the AN for changing the RTCMAC rate 
        transition parameters (may also change the DPA RLP parameters
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INTERIA:
      ret_val = 
          ds707_pkt_mgr_iface_ioctl_enable_hdr_rev0_rate_inertia( iface_ptr,
                                                                  argval_ptr,
                                                                  ps_errno );
      break;
#endif /* FEATURE_HDR */

#ifdef FEATURE_HDR_QOS
    case PS_IFACE_IOCTL_QOS_REQUEST:
      ret_val = ds707_pkt_mgr_iface_ioctl_qos_request( iface_ptr, 
                                                       ioctl_name,
                                                       argval_ptr,
                                                       ps_errno );
      break;      
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
    case PS_IFACE_IOCTL_QOS_REQUEST_EX:
      ret_val = ds707_pkt_mgr_iface_ioctl_qos_request_ex( iface_ptr, 
                                                          ioctl_name,
                                                          argval_ptr, 
                                                          ps_errno );
      break;      
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
    case PS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM:
      ret_val = ds707_pkt_mgr_iface_ioctl_on_qos_aware_system( iface_ptr,
                                                               argval_ptr,
                                                               ps_errno );
      break;
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
      /*---------------------------------------------------------------------
       IOCTL to Query the Network Supported QoS Profiles (AN Supported 
       Profiles) 
       --------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES:
      ret_val = 
         ds707_pkt_mgr_iface_ioctl_get_network_supported_qos_profiles(
                                                                  iface_ptr, 
                                                                  argval_ptr,
                                                                  ps_errno );
      break;
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
  /*-----------------------------------------------------------------------
  IOCTL to Query the RF conditions
  -----------------------------------------------------------------------*/
  case PS_IFACE_IOCTL_GET_RF_CONDITIONS:
    ret_val = ds707_pkt_mgr_iface_ioctl_get_border_rf_conditions(
                                                                iface_ptr, 
                                                                argval_ptr, 
                                                                ps_errno );
  break;
#endif /* FEATURE_HDR_QOS */
    
#ifdef FEATURE_HDR    
#ifdef FEATURE_EIDLE_SCI
      /*---------------------------------------------------------------------
        Set the mode for selecting the slot cycle index on HDR system. This
        will end up sending a GAUP message to the AN for changing the slot  
        cycle index parameters
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE:
      ret_val = 
              ds707_pkt_mgr_iface_ioctl_enable_hdr_slotted_mode( iface_ptr,
                                                                 argval_ptr, 
                                                                 ps_errno );
      break;
#endif /* FEATURE_EIDLE_SCI */
#endif /* FEATURE_HDR */

    case PS_IFACE_IOCTL_GET_SESSION_TIMER:
      {
        /*Simplified JCDMA:
          5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
        if(ds707_roaming_is_curr_mode_jcdma() ||
           ds707_roaming_is_simplified_jcdma_mode())
        {
          ret_val = ds707_pkt_mgr_iface_ioctl_get_session_timer(iface_ptr, 
                                                                argval_ptr,
                                                                ps_errno);
        }
      }
      break;

    case PS_IFACE_IOCTL_SET_SESSION_TIMER:
      {
        /*Simplified JCDMA:
          5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
        if(ds707_roaming_is_curr_mode_jcdma() ||
           ds707_roaming_is_simplified_jcdma_mode())
        {
          ret_val = ds707_pkt_mgr_iface_ioctl_set_session_timer(iface_ptr, 
                                                                argval_ptr,
                                                                ps_errno);
        }
      }
      break;

/* Revisit the featurization later as DHCP and MIPV6 should not be coupled together.
   MIPV6 is a client of DHCP
*/
#ifdef FEATURE_DATA_PS_MIPV6
    case PS_IFACE_IOCTL_SET_MIP6_BOOTSTRAP_PARAMS:
      ret_val = 
            ds707_pkt_mgr_iface_ioctl_set_mip6_bootstrap_params(iface_ptr, 
                                                                argval_ptr,
                                                                ps_errno);
      break;
#endif /* FEATURE_DATA_PS_MIPV6 */

#ifdef FEATURE_IS707B_ADDENDUM
   case PS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER:
   	ret_val = ds707_pkt_mgr_iface_ioctl_set_hat_timer( iface_ptr,
                                                       argval_ptr,
                                                       ps_errno);
	break;
    case PS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER:
   	ret_val = ds707_pkt_mgr_iface_ioctl_get_hat_timer(  iface_ptr,
                                                        argval_ptr,
                                                        ps_errno);
	break;
#endif /* FEATURE_IS707B_ADDENDUM*/
    
    case PS_IFACE_IOCTL_707_GET_DATA_CALL_TYPE:
   	ret_val = ds707_pkt_mgr_iface_ioctl_get_data_call_type( iface_ptr,
                                                            argval_ptr,
                                                            ps_errno );
	break;

    case PS_IFACE_IOCTL_707_GET_DATA_COUNT:
   	ret_val = ds707_pkt_mgr_iface_ioctl_get_data_count( iface_ptr,
                                                        argval_ptr,
                                                        ps_errno );
	break;

    case PS_IFACE_IOCTL_707_SET_HDR_AN_USER_ID:
   	ret_val = ds707_pkt_mgr_iface_ioctl_set_hdr_an_user_id( iface_ptr,
                                                            argval_ptr,
                                                            ps_errno );
	break;
#ifdef FEATURE_EPC_HANDOFF
    case PS_IFACE_IOCTL_GET_EPC_HANDOFF_CTXT:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_epc_handoff_ctxt(iface_ptr,
                                                             argval_ptr,
                                                             ps_errno);
      break;

    case PS_IFACE_IOCTL_SET_HANDOFF_CTXT:
      ret_val = ds707_pkt_mgr_iface_ioctl_set_epc_handoff_ctxt(iface_ptr,
                                                             argval_ptr,
                                                             ps_errno);
      break;
#endif /* FEATURE_EPC_HANDOFF */
#ifdef FEATURE_EHRPD
    case PS_IFACE_IOCTL_RECONFIG:
      ret_val = ds707_pkt_mgr_iface_ioctl_reconfig(iface_ptr,
                                                   argval_ptr,
                                                   ps_errno);
      break;
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_DATA_PS_IPV6
    case PS_IFACE_IOCTL_GET_IPV6_REMAINING_ROUTER_LIFETIME:
      ret_val = ds707_pkt_mgr_iface_ioctl_get_v6_rem_rtr_lifetime(iface_ptr,
                                                                  argval_ptr,
                                                                  ps_errno);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    case PS_IFACE_IOCTL_707_GET_MIP_MA_INFO:
      {
#ifdef FEATURE_DS_MOBILE_IP
        ret_val = ds707_pkt_mgr_iface_ioctl_get_mip_ma_info(argval_ptr,
                                                            ps_errno);
#endif /* FEATURE_DS_MOBILE_IP */
      }
      break;

    /*---------------------------------------------------------------------
      Default
    ---------------------------------------------------------------------*/
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "No such 707pkt ioctl: %d", 
                      ioctl_name);
      *ps_errno = DS_EOPNOTSUPP;
      ret_val = -1;

  } /* switch */

  return ret_val;

} /* ds707_pkt_ioctl() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SYS_IOCTL_INIT

DESCRIPTION   Called by ds707_pkt_mgr_init to register the sys ioctl cb

DEPENDENCIES  Registered once, at startup.

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_sys_ioctl_init
(
  void
)
{
  int16 ps_errno;
  ps_sys_subscription_enum_type subs_id =PS_SYS_PRIMARY_SUBS;

  for ( subs_id = PS_SYS_PRIMARY_SUBS ; subs_id<=PS_SYS_TERTIARY_SUBS; 
        subs_id++)
  {
      if (DSS_SUCCESS != ps_sys_ioctl_register_tech_cback_ex(PS_SYS_TECH_3GPP2,
                                                        ds707_sys_ioctl,
                                                             subs_id,
                                                        &ps_errno))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                      "Error registering sys ioctl callback with PS: %d", 
                      ps_errno);
      ASSERT(0);
    }
  }
}

/*===========================================================================
FUNCTION      DS707_SYS_IOCTL

DESCRIPTION   Called by ps_iface to perform sys specific ioctls

DEPENDENCIES  Registered once, at startup.

RETURN VALUE  0 - on Success 
             -1 - on Failure 

SIDE EFFECTS  None.
===========================================================================*/
int16 ds707_sys_ioctl
(
  ps_sys_ioctl_enum_type   ioctl_name,
  void                     *argval_ptr,
  ps_sys_subscription_enum_type     subs_id,
  int16                    *ps_errno
)
{
  int16             ret_val = 0;
  sys_modem_as_id_e_type as_subs_id=SYS_MODEM_AS_ID_NONE;
  ps_sys_ioctl_3gpp2_set_page_monitor_type *data_ptr = NULL;
  
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "707 SYS ioctl: %d", ioctl_name);

  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "ds707_sys_ioctl: argval_ptr passed in is NULL");
    return -1;
  }

  if (NULL == ps_errno)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "ds707_sys_ioctl: ps_errno passed in is NULL");
    return -1;
  }
  *ps_errno = 0;

//#ifdef FEATURE_DSDA
  if( ds3g_resolve_subs_id((ds_sys_subscription_enum_type)subs_id, &as_subs_id) == FALSE)
  {
   *ps_errno = DS_EFAULT;

    DATA_IS707_MSG2(MSG_LEGACY_ERROR,"Error in obataining resolved sub id %d "
                           "Policy Sub ID %d",subs_id,as_subs_id);
    return -1;
  }
  
    if ( as_subs_id != SYS_MODEM_AS_ID_1 )
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR," resolved sub id %d is not supported"
                                     " for 3GPP2 IOCTLs  Sub ID %d ,resolve sub id %d",
                                      subs_id,as_subs_id);
    return -1;
  }

  
//#endif /*DSDA */
  /*-------------------------------------------------------------------------
    Based on the ioctl name, perform the appropriate actions.
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {
#ifdef FEATURE_DATA_OPTHO //OPTHO_ToDo: Should this be featurized?
    case PS_SYS_IOCTL_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_CMD:
    {
      ret_val = ds707_pkt_mgr_sys_ioctl_prereg_handoff(argval_ptr, ps_errno);
      break;
    }
#endif /* FEATURE_DATA_OPTHO */
    case PS_SYS_IOCTL_PDN_THROTTLE_INFO:
    {
      ret_val = ds707_pdncntx_sys_ioctl_ehrpd_pdn_throttle_info(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_DUN_CTRL_RESPONSE:
    {
      ret_val = ds707_sys_ioctl_dun_ctrl_response(argval_ptr, ps_errno);
      break;
    }

    //PHASE2 IOCTLs update
    case PS_SYS_IOCTL_3GPP2_PRE_DORMANCY_SYS_INFO:
    {
      ret_val = ds707_get_pre_dormancy_params(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_SET_CAM_TIMER:
    {
      ret_val = ds707_set_cam_timer(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_GET_CAM_TIMER:
    {
      ret_val = ds707_get_cam_timer(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_SET_SCRM:
    {
      ret_val = ds707_enable_disable_scrm(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_GET_SCRM:
    {
      ret_val = ds707_is_scrm_enabled(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_SET_RDUD:
    {
      ret_val = ds707_enable_disable_rdud(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_GET_RDUD:
    {
      ret_val = ds707_is_rdud_enabled(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD:
    {
      data_ptr = (ps_sys_ioctl_3gpp2_set_page_monitor_type*)(argval_ptr);
      ret_val = ds707_pkt_mgr_set_evdo_page_monitor_period((uint8)data_ptr->page_monitor_period,
                                                           data_ptr->ps_sys_ioctl_set_evdo_pmp_cb,
                                                           ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_SET_FORCE_LONG_SLEEP:
    {
      ret_val = ds707_pkt_mgr_set_evdo_force_long_sleep(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD:
    {
      ret_val = ds707_pkt_mgr_get_evdo_page_monitor_period(argval_ptr, ps_errno);
      break;
    }
    case PS_SYS_IOCTL_3GPP2_GET_CALL_THROTTLE_INFO:
    {
      ret_val = 
                ds707_get_call_throttle_info(
                          ((ps_sys_ioctl_3gpp2_call_thr_info_hdlr_cb_type*)(argval_ptr))->call_thro_info_f_ptr,
                          ps_errno);
      break;
    }
#if 0
    case QMI_WDS_EVDO_PAGE_MONITOR_PERIOD_RESULT_IND:
#endif

    case PS_SYS_IOCTL_IS_CLAT_SUPPORTED_BY_PROFILE:
      ret_val = ds707_rt_is_clat_supported(argval_ptr, as_subs_id, ps_errno);
      break;

    default:
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "No such 707 sys ioctl: %d", ioctl_name);
      *ps_errno = DS_EOPNOTSUPP;
      ret_val = -1;
      break;
    }
  }
  return ret_val;
} /* ds707_sys_ioctl */

/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_IOCTL

DESCRIPTION   Does this IOCTL processing for the physlink specified

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
/*lint -save -e715 unused variables*/
int ds707_pkt_phys_link_ioctl
(
  ps_phys_link_type        *phys_link_ptr,
  ps_phys_link_ioctl_type  ioctl_name,
  void                     *argval_ptr,
  int16                    *ps_errno
)
{
  int ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "707 pkt phys_link_ioctl: %d",
                   ioctl_name);
  *ps_errno = 0;

  if (phys_link_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl fail, phys_link_ptr NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Based on the ioctl name, perform the appropriate actions.
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {
    /*-----------------------------------------------------------------------
      Go active.
    -----------------------------------------------------------------------*/
    case PS_PHYS_LINK_IOCTL_GO_ACTIVE:
      ret_val = ds707_pkt_mgr_phys_link_ioctl_go_active( phys_link_ptr,
                                                         argval_ptr, 
                                                         ps_errno );
      break;


      /*---------------------------------------------------------------------
        Go dormant.
      ---------------------------------------------------------------------*/
    case PS_PHYS_LINK_IOCTL_GO_DORMANT:
      ret_val = ds707_pkt_mgr_phys_link_ioctl_go_dormant( phys_link_ptr,
                                                          argval_ptr, 
                                                          ps_errno );

      break;


      /*---------------------------------------------------------------------
      Default.
      ---------------------------------------------------------------------*/
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,"No such 707pkt phys_link_ioctl: %d", 
                      ioctl_name);
      *ps_errno = DS_EOPNOTSUPP;
      ret_val = -1 ;

  } /* switch */

  return ret_val;  
} /* ds707_pkt_phys_link_ioctl() */

/*===========================================================================
FUNCTION      DS707_PKT_PS_FLOW_IOCTL

DESCRIPTION   Does the processing for the psflow specific ioctls

DEPENDENCIES  None.

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_pkt_ps_flow_ioctl
(
  ps_flow_type         * ps_flow_ptr,
  ps_flow_ioctl_type     ioctl_name,
  void                 * argval_ptr,
  int16                * ps_errno
)
{
  int ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "707 pkt ps_flow_ioctl: %d",
                   ioctl_name);

  *ps_errno = 0;
  ASSERT(ps_flow_ptr != NULL);
  /*lint -save -e774 */
  if (ps_flow_ptr == NULL)
  /*lint -restore*/
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ioctl fail, flow_ptr NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Based on the ioctl name, perform the appropriate actions.
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {

#ifdef FEATURE_HDR_QOS
    /*-----------------------------------------------------------------------
      QOS Modify
    -----------------------------------------------------------------------*/
    case PS_FLOW_IOCTL_QOS_MODIFY:
    {
      ret_val = ds707_pkt_mgr_flow_ioctl_qos_modify( ps_flow_ptr,
                                                     argval_ptr,
                                                     ps_errno );
    }  
    break;

      /*---------------------------------------------------------------------
      QOS Release
      ---------------------------------------------------------------------*/
    case PS_FLOW_IOCTL_QOS_RELEASE:
      ret_val = ds707_pkt_mgr_flow_ioctl_qos_release( ps_flow_ptr,
                                                      argval_ptr,
                                                      ps_errno );
      break;


      /*---------------------------------------------------------------------
      QOS Release_ex
      ---------------------------------------------------------------------*/
    case PS_FLOW_IOCTL_QOS_RELEASE_EX:
      ret_val = ds707_pkt_mgr_flow_ioctl_qos_release_ex( ps_flow_ptr,
                                                         argval_ptr,
                                                         ps_errno );
      break;

      /*---------------------------------------------------------------------
      QOS SUSPEND IOCTL
      ---------------------------------------------------------------------*/
    case PS_FLOW_IOCTL_QOS_SUSPEND:
      ret_val = ds707_pkt_mgr_flow_ioctl_qos_suspend( ps_flow_ptr,
                                                      argval_ptr,
                                                      ps_errno );
      break;

      /*---------------------------------------------------------------------
        IOCTL QOS_SUSPEND_EX
      ---------------------------------------------------------------------*/
    case PS_FLOW_IOCTL_QOS_SUSPEND_EX:
      ret_val = ds707_pkt_mgr_flow_ioctl_qos_suspend_ex( ps_flow_ptr,
                                                         argval_ptr,
                                                         ps_errno );
      break;

      /*---------------------------------------------------------------------
      QOS Resume
      ---------------------------------------------------------------------*/
    case PS_FLOW_IOCTL_QOS_RESUME: 
      ret_val = ds707_pkt_mgr_flow_ioctl_qos_resume( ps_flow_ptr,
                                                     argval_ptr, 
                                                     ps_errno );
      break;

      /*--------------------------------------------------------------------
      QOS_RESUME_EX IOCTL
      ---------------------------------------------------------------------*/
    case PS_FLOW_IOCTL_QOS_RESUME_EX: 
      ret_val = ds707_pkt_mgr_flow_ioctl_qos_resume_ex( ps_flow_ptr,
                                                        argval_ptr,
                                                        ps_errno );
      break;

    case PS_FLOW_IOCTL_HDR_GET_RMAC3_INFO:
      ret_val = ds707_pkt_mgr_flow_ioctl_get_rmac3_info( ps_flow_ptr, 
                                                         argval_ptr,
                                                         ps_errno );

      break;

    case PS_FLOW_IOCTL_707_GET_INACTIVITY_TIMER:
      ret_val = ds707_pkt_mgr_flow_ioctl_get_inactivity_timer( ps_flow_ptr,
                                                               argval_ptr,
                                                               ps_errno );
      break;

    case PS_FLOW_IOCTL_707_SET_INACTIVITY_TIMER:
      ret_val = ds707_pkt_mgr_flow_ioctl_set_inactivity_timer( ps_flow_ptr,
                                                               argval_ptr,
                                                               ps_errno );
      break;

#endif /* FEATURE_HDR_QOS */
      /*---------------------------------------------------------------------
        Transmit watermark level
      ---------------------------------------------------------------------*/
    case PS_FLOW_IOCTL_GET_TX_QUEUE_LEVEL:
      ret_val = ds707_pkt_mgr_flow_ioctl_get_tx_queue_level( ps_flow_ptr, 
                                                             argval_ptr,
                                                             ps_errno );

      break;
#ifdef FEATURE_HDR_QOS
    case PS_FLOW_IOCTL_707_GET_TX_STATUS:
      ret_val = ds707_pkt_mgr_flow_ioctl_get_tx_status( ps_flow_ptr,
                                                        argval_ptr,
                                                        ps_errno );
      break;        
#endif /* FEATURE_HDR_QOS */    

    /*---------------------------------------------------------------------
      Default
    ---------------------------------------------------------------------*/
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                      "Undefined or Unsupported 707pkt ps_flow_ioctl: %d",
                      ioctl_name);
      *ps_errno = DS_EOPNOTSUPP;
      ret_val = -1;
  } /* switch */

  return ret_val;
} /* ds707_pkt_ps_flow_ioctl() */
/*lint -restore*/
#endif /* FEATURE_DATA_IS707 */

