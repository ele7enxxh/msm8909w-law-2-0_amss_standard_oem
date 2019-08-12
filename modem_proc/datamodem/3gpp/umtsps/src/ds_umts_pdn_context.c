 /*!
  @file
  ds_umts_pdn_context.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/src/ds_umts_pdn_context.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/13   nd      Added support for filling PCO of MT calls(primary/secondary)
                   from incoming call request and not from profile(See 23.060).
09/10/12   ss      Added support for IPV4 link MTU IE for Rel10.
08/08/12   nd      Release 10 or above ignore static IP address filling.
08/13/12   ss      Moving efs write operation to dsutil performed during 
                   bootup.
06/19/12   nd      MT PDP pkt call.
11/22/11   sa      Added NV item 3gpp_auto_answer_enable.
10/21/11   nd      Fixed compilation warnings.
10/05/11   ttv     Added DSDS changes for new mode handler.
07/19/11   sa      Added support to override profile values with orig_info
                   for auth_info, ipcp_info and ip_addr.
06/21/11   dvk     Fixed compilation warnings.
04/05/11   sch     Filled DNS server Ipv4 addr req container in PCO
03/24/11   sa      Added MT PDP Auto answer support.
10/01/10   ars     Enabled processing auth parameters from info   
09/17/10   sch     Accepting DS_UMTS_PDP_AUTH_CHAP_OR_PAP as a valid auth type
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)

#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gpp_bearer_context.h"
#include "ps_svc.h"
#include "ps_iface.h"
#include "dsutil.h"
#include "ps_aclrules.h"
#include "ps_route.h"
#include "dsumtspdpreg.h"
#include "dsumtspsmthdlr.h"
#include "ps_ppp_defs.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_nv_manager.h"
#include "ds3gcfgmgr.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_umts_flow_manager.h"
#include "ds_umts_bearer_flow_manager.h"
#include "ds_3gpp_op_pco_hdlr.h"
#ifdef FEATURE_TDSCDMA
#include "tdsrrcdataext.h"
#endif
#include "ds_wcdma_bearer_context.h"
#include "rlcdsapif.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "dsgen_iface6_hdlr.h"
#include "dstask.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ps_iface_ioctl.h"
#include "ps_flow_ioctl.h"
#include "ps_phys_link_ioctl.h"
#include "ps_iface_ipfltr.h"
#include "ps_phys_link.h"
#include "dsumts_rmsm.h"
#include "dhcp_client.h"
#include "dsumtspspco.h"
#include "ds3gmgr.h"
#include "ds_umts_bearer_context.h"
#include "ds_umts_pdn_context.h"
#include "dsutil.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_network_override_hdlr.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include <stringl/stringl.h>
#include "ps_ip_netconfig.h"

#ifdef FEATURE_GSM_DTM
#include "rr_ds_if.h"
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_GPRS
#include "geran_qmi.h"
#include "ds_gprs_bearer_context.h"
#endif

#include "ds3gutil.h"

#define DS_3GPP_UMTS_IPV6_IID_OFFSET 8
#define DS_3GPP_UMTS_DUAL_IP_IPV6_IID_OFFSET 12

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
extern ds_pdn_context_s ds_pdn_context_tbl[DS_3GPP_MAX_PDN_CONTEXT];
/*! @brief UMTS PDN context table */
ds_umts_pdn_context_s umts_pdn_context_tbl[DS_3GPP_MAX_PDN_CONTEXT] = {{NULL}};

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION dsUmtsPdnCntx_activateInd

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
LOCAL boolean dsUmtsPdnCntx_activateInd
( 
  ds_pdn_context_s           *pdn_cntx_p,
  const cm_call_mode_info_s_type   *mode_info_p,
  cm_end_params_s_type       *end_params 
);

/*===========================================================================
FUNCTION dsUmtsPdnCntx_getOrigParams

DESCRIPTION
  This function provides call origination paramters.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration.

SIDE EFFECTS  

===========================================================================*/
LOCAL boolean dsUmtsPdnCntx_getOrigParams
( 
  ds_pdn_context_s                    *pdn_cntx_p,
  ds_3gpp_orig_params_s               *mm_orig_params_p,                          
  const ds_3gpp_pdn_call_info_type    *call_info
);

/*===========================================================================
FUNCTION ds_pdn_cntxt_setupPdpPPPprotoStack

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
LOCAL boolean ds_pdn_cntxt_setup_pdp_ppp_proto_stack
( 
  ds_pdn_context_s               *pdn_cntx_p,
  const ds_umts_pdp_addr_type    *ds_3gpp_pdp_addr_p,  
  const protocol_cfg_options_T   *protocol_config_options_p,
  cm_end_params_s_type           *end_params 
);

/*===========================================================================
FUNCTION DSUMTSPS_PDP_IP_FILL_PPP_PARAMS_PPP_PARAMS_FROM_PROFILE

DESCRIPTION
  This function fills in the PPP authentication and IPCP packet information
  for a PDP-IP call from the PDP profile specified for the call
    
PARAMETERS  
    *orig_params_p   - Ptr to  Call origination parameters 
    pdp_profile_p    - Ptr to the PDP profile parameters
    subs_id          - Subscription Id

DEPENDENCIES
  None.
  
RETURN VALUE
  none  

SIDE EFFECTS
  orig_params_ptr is updated with the PPP packet information from the profile
  
===========================================================================*/
LOCAL boolean dsumtsps_pdp_ip_fill_ppp_params_from_profile
(
  cm_gw_ps_orig_params_s_type  *orig_params_p,
  ds_umts_pdp_profile_type     *profile_p,
  sys_modem_as_id_e_type        subs_id  
);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================
FUNCTION  dsUmtsPdnCntx_verifyPdpType

DESCRIPTION
   This function verifies that PDP type returned in PDP context accept is same 
   as one requested in PDP context request.

PARAMETERS   :
  *mode_info_ptr  - Pointer to the mode-specific call information provided by
                    Call Manager
  pdp_type_ctx_req - PDP type requested in PDP context request
DEPENDENCIES
  None.

RETURN VALUE
   TRUE - PDP type return is same as PDP type received in context accept.
   FALSE - PDP type return is same as PDP type received context accept.

SIDE EFFECTS
  None.
VRK:TODO Need to change for dual IP V4V6 support
===========================================================================*/
boolean dsUmtsPdnCntx_verifyPdpType
(
 const cm_call_mode_info_s_type *mode_info_ptr, 
 ds_pdn_context_s               *pdn_cntx_p
)
{
  ds_umts_pdp_type_enum_type    pdp_type_ctx_accept = DS_UMTS_PDP_MAX;
  byte                          pdp_type_org;
  byte                          pdp_type_num;
  ds_apn_ip_support_type_e      ip_support = DS_IPV4_ONLY;
  ds_umts_pdp_type_enum_type    pdp_type_ctx_req;
  boolean                       result = DS3G_SUCCESS;
  ds_3gpp_iface_s              *ds_iface_v4_p = NULL;
  ds_3gpp_iface_s              *ds_iface_v6_p = NULL;
  sys_modem_as_id_e_type             subs_id=SYS_MODEM_AS_ID_NONE;
  /* check input parameters */
  if (mode_info_ptr == NULL ||(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p)))
  {
    DATA_ERR_FATAL("Invalid input parameters in dsUmtsPdnCntx_verifyPdpType");
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }
  /*-------------------------------------------------------------------------
     Determine the PDP type received from context accept message. 
     TS 24.008: pdp_type_orig is only 4 bits 
  -------------------------------------------------------------------------*/
  pdp_type_org = mode_info_ptr->info.gw_ps_call.pdp_addr.pdp_type_org & 0x0F;
  pdp_type_num = mode_info_ptr->info.gw_ps_call.pdp_addr.pdp_type_num;

  /*-------------------------------------------------------------------------
     Determine requested pdp_type from the profile
  -------------------------------------------------------------------------*/
  pdp_type_ctx_req = pdn_cntx_p->ds_pdn_context_dyn_p ->
                                                pdp_profile.context.pdp_type;

  /*----------------------------------------------------------------------
    We assume the IP support type of this APN to be what we requested,
    this info will be adjusted based on ESM cause code if it is returned
  ----------------------------------------------------------------------*/
  switch(pdp_type_ctx_req)
  {
    case DS_UMTS_PDP_PPP: // lint fallthrough
    case DS_UMTS_PDP_IPV4:
      ip_support = DS_IPV4_SINGLE_BEARER;
      break;
    case DS_UMTS_PDP_IPV6:
      ip_support = DS_IPV6_SINGLE_BEARER;
      break;
    case DS_UMTS_PDP_IPV4V6:
      ip_support = DS_IPV4V6;
      break;
    default:
      DATA_ERR_FATAL("Unknown PDP type.");
  }

  if(mode_info_ptr->info.gw_ps_call.cause.ext_cause != SM_UNKNOWN_CAUSE)
  {
    switch(pdp_type_ctx_req)
    {
      case DS_UMTS_PDP_PPP:
          DS_3GPP_MSG0_HIGH("Received ESM cause in a ppp call, ignored");
          break;
      case DS_UMTS_PDP_IPV4:
        if(mode_info_ptr->info.gw_ps_call.cause.ext_cause == 
           SM_PDP_TYPE_IPV6_ONLY_ALLOWED)
      	{
          DS_3GPP_MSG0_HIGH("Received ESM cause IPV6 only while profile sent out V4");
          ip_support = DS_IPV6_ONLY;
      	  result = DS3G_FAILURE;
      	}
        break;
      case DS_UMTS_PDP_IPV6:
        if(mode_info_ptr->info.gw_ps_call.cause.ext_cause == 
           SM_PDP_TYPE_IPV4_ONLY_ALLOWED)
      	{
          DS_3GPP_MSG0_HIGH("Received ESM cause IPV4 only while profile sent out V6");
          ip_support = DS_IPV4_ONLY;
      	  result = DS3G_FAILURE;
      	}
      	break;
      case DS_UMTS_PDP_IPV4V6:
        if(mode_info_ptr->info.gw_ps_call.cause.ext_cause == 
           SM_PDP_TYPE_IPV4_ONLY_ALLOWED)
        {
          DS_3GPP_MSG0_HIGH("Received ESM cause IPV4 only while profile sent out V4V6");
          ip_support = DS_IPV4_ONLY;
        }
        else if(mode_info_ptr->info.gw_ps_call.cause.ext_cause == 
           SM_PDP_TYPE_IPV6_ONLY_ALLOWED)
        {
          DS_3GPP_MSG0_HIGH("Received ESM cause IPV6 only while profile sent out V4V6");
          ip_support = DS_IPV6_ONLY;
        }
        else if(mode_info_ptr->info.gw_ps_call.cause.ext_cause == 
           SM_SINGLE_ADR_BEARERS_ONLY_ALLOWED)
        {
          DS_3GPP_MSG0_HIGH("Received ESM cause Single address bearer only while sent out V4V6");
          if(mode_info_ptr->info.gw_ps_call.pdp_addr.pdp_type_num == DS_3GPP_PDP_TYPE_IPV4_NUM)
          {
            ip_support = DS_IPV4_SINGLE_BEARER;
          }
          else if (mode_info_ptr->info.gw_ps_call.pdp_addr.pdp_type_num == DS_3GPP_PDP_TYPE_IPV6_NUM)
          {
            ip_support = DS_IPV6_SINGLE_BEARER;
          }
          else
          {
            DS_3GPP_MSG1_HIGH("Incorrect PDN address type: %d, default to v4",
                     mode_info_ptr->info.gw_ps_call.pdp_addr.pdp_type_num);
            result = DS3G_FAILURE;
            return result;
          }         
        }
        else
        {
          DS_3GPP_MSG1_HIGH("Unhandled cause code %d",
                     mode_info_ptr->info.gw_ps_call.cause.ext_cause);
          break;
        }
        ds_pdn_cntxt_dissociate_iface(pdn_cntx_p, ip_support);
        break;
      default:
      	result = DS3G_FAILURE;
        DATA_ERR_FATAL("Unknown PDP type.");
    }
  } // if( ext_cause != SM_UNKNOWN_CAUSE )
  else
  {
    result = ds_umts_pdn_cntxt_hndl_unknown_cause(
                                      pdp_type_ctx_req,
                                      pdp_type_num,
                                      pdn_cntx_p,
                                      &ip_support);

  }
  /*---------------------------------------------------------------------------
  Handle failure from above. Reject call.
  ---------------------------------------------------------------------------*/
  if(result == DS3G_FAILURE)
  {
    DS_3GPP_MSG2_HIGH("IP returned was diff than requested. Req: %d, Ret: %d. Rej call",
                pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type, 
                      ip_support);
    return result;
  }

  /*--------------------------------------------------------------------
      Now that we have determined the IP support returned by the network,
      we need to make sure that this is something that we need. If n/w
      returns an IP type that is not the one that the app requested for
      then we have to reject bearer activation and request a new PDN
      connection with the app requested IP type only.
  --------------------------------------------------------------------*/
  ds_iface_v4_p = 
        pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
                                          [DS_3GPP_PDN_CONTEXT_IPV4_IFACE];

  ds_iface_v6_p = 
        pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
                                          [DS_3GPP_PDN_CONTEXT_IPV6_IFACE];

  /*------------------------------------------------------------------------
      Re-initialize result to TRUE. This is because we are going to check a 
      few more cases.
  ------------------------------------------------------------------------*/
  result = DS3G_SUCCESS;

  switch(ip_support)
  {
    case DS_IPV4_ONLY:
    case DS_IPV4_SINGLE_BEARER:
      if(PS_IFACE_IS_IN_USE(&(ds_iface_v4_p->ps_iface)) != TRUE)
      {
          /*------------------------------------------------------------------
            Reject bearer activation since IP V6 pipe cannot be obtained
            If a V6 pipe were needed, the request would have been made while
            dissociating. It is safe to reject this bearer activation
         ------------------------------------------------------------------*/       
        result = DS3G_FAILURE;
      }
      break;
    case DS_IPV6_ONLY:
    case DS_IPV6_SINGLE_BEARER:
      if(PS_IFACE_IS_IN_USE(&(ds_iface_v6_p->ps_iface)) != TRUE)
      {
          /*------------------------------------------------------------------
            Reject bearer activation since IP V4 pipe cannot be obtained
            If a V4 pipe were needed, the request would have been made while
            dissociating. It is safe to reject this bearer activation
          ------------------------------------------------------------------*/      
        result = DS3G_FAILURE;
      } 
      break;

    default:
      DS_3GPP_MSG0_HIGH("Ignoring IP support IPv4V6");
      break;
  }

  if(result == DS3G_FAILURE)
  {
    DS_3GPP_MSG0_HIGH("Rejecting bearer activation since other IP type was given");
    return result;
  }
    
  /*-------------------------------------------------------------------------
     TS 24.008- 
     If PDP org type is of type ETSI(0x0), pdp_type_num should be
     0x01 for PDP type PPP. All other values are reserved. 
     if PDP org type is of type IETF(0x1), pdp_type_num should be 0x21 (IPV4)
     or 0x57 (IPV6). All other values should be treated as IPv4.
  -------------------------------------------------------------------------*/
  if(pdp_type_org == 0)
  {
    if( pdp_type_num ==  0x01)
    {
      pdp_type_ctx_accept = DS_UMTS_PDP_PPP;
      ip_support = DS_IPV4_ONLY;
      if (pdp_type_ctx_req != DS_UMTS_PDP_PPP)
      {
        DS_3GPP_MSG0_ERROR("Received unexpected pdp type PPP.");
        return DS3G_FAILURE;
      }
    }
    else
    {
      DS_3GPP_MSG1_HIGH("ETSI PDP type not supported %d", pdp_type_num);
      return DS3G_FAILURE;
    }
  } /* pdp_type_org == PPP */
  else if(pdp_type_org == 0x01)
  {
    if( ( pdp_type_num !=  DS_3GPP_PDP_TYPE_IPV6_NUM) &&
        ( pdp_type_num !=  DS_3GPP_PDP_TYPE_IPV4V6_NUM) )
    {
      if(ip_support != DS_IPV4_ONLY &&
         ip_support != DS_IPV4_SINGLE_BEARER)
      {
        DS_3GPP_MSG3_ERROR("IPV4 ret when req was diff. Req: %d, Ret: %d. Rej call",
                pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type, 
                           ip_support,0);
        return DS3G_FAILURE;
      }
      pdp_type_ctx_accept = DS_UMTS_PDP_IPV4;
    }
#ifdef FEATURE_DATA_PS_IPV6
    else if( pdp_type_num ==  DS_3GPP_PDP_TYPE_IPV6_NUM)   
    {
      if(ip_support != DS_IPV6_ONLY &&
         ip_support != DS_IPV6_SINGLE_BEARER)
      {
        DS_3GPP_MSG3_ERROR("IPV6 ret when req was diff. Req: %d, Ret: %d. Rej call",
                pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type, 
                           ip_support,0);
        return DS3G_FAILURE;
      }
      pdp_type_ctx_accept = DS_UMTS_PDP_IPV6;
    }
    else if( pdp_type_num ==  DS_3GPP_PDP_TYPE_IPV4V6_NUM)   
    {
      if(ip_support != DS_IPV4V6)
      {
        DS_3GPP_MSG3_ERROR("IPV4V6 ret when req was diff. Req: %d, Ret: %d. Rej call",
                pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type, 
                           ip_support,0);
        return DS3G_FAILURE;
      }
      pdp_type_ctx_accept = DS_UMTS_PDP_IPV4V6;
    }
#else
    else
    {
      DS_3GPP_MSG0_HIGH("PDP type IPV6 not supported");
      return DS3G_FAILURE;
    }
#endif
  } /* pdp_type_org == IP */  
  else
  { 
    DS_3GPP_MSG1_HIGH("PDP type org not valid %d", pdp_type_org); 
    return DS3G_FAILURE;   
  }
  
  /*-------------------------------------------------------------------------
    If pdp_type_ctx_req is ppp, make sure that accept is also ppp, otherwise
    fail the call
  -------------------------------------------------------------------------*/
  if( pdp_type_ctx_req == DS_UMTS_PDP_PPP &&
      pdp_type_ctx_accept != DS_UMTS_PDP_PPP ) 
  {
    DS_3GPP_MSG0_ERROR("Requested PPP call but NW did not return PPP.");
    return DS3G_FAILURE;
  }


  /*-------------------------------------------------------------------------
    Update APN table with APN name and IP support
  -------------------------------------------------------------------------*/
  if (ds_pdn_cntxt_update_apn_table( pdn_cntx_p->ds_pdn_context_dyn_p->
                                     pdp_profile.context.apn,
                                  ip_support,
                                  (void*)pdn_cntx_p) != TRUE )
  {
    DS_3GPP_MSG0_ERROR("Unable to update APN table, cannot set up the bearer");    
    return DS3G_FAILURE;
  }

  return DS3G_SUCCESS; 
  
}

/*===========================================================================
FUNCTION dsUmtsPdnCntx_activateInd

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
LOCAL boolean dsUmtsPdnCntx_activateInd
( 
  ds_pdn_context_s           *pdn_cntx_p,
  const cm_call_mode_info_s_type   *mode_info_p,
  cm_end_params_s_type       *end_params 
)
{ 
  boolean                          result = DS3G_SUCCESS;  
  ds_bearer_context_s             *bearer_cntx_p = NULL;
  int                              bearer_index = 0;
  ds_umts_pdp_addr_type            ds_3gpp_pdp_addr;
  const protocol_cfg_options_T    *protocol_config_options_p;
  cm_gw_ps_call_info_s_type       *umts_call_info_p;
  ds_flow_manager_s               *flow_manager_ptr;
  sys_modem_as_id_e_type           subs_id=SYS_MODEM_AS_ID_NONE;  

/*--------------------------------------------------------------------------*/
  /* verify input parameters */
  if (mode_info_p == NULL || 
      (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p)))
  {
    DATA_ERR_FATAL("Invalid inpout in dsUmtsPdnCntx_activateInd");
  }
  
  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Ensure that the PDN state is correct. If the PDN state is in GOING_DOWN,
    then we have to reject the bearer activation. 
    This is a case of a race between the Abort PDN connectivity handling and 
    the handling of Activate Bearer Ind. In such a case, we will reject the 
    bearer activation and not send out an abort. As a result of rejecting 
    bearer activation, we will get a CALL_END from CM during which we clean up
    our resources.
  -------------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_GOING_DOWN)
  {
    DS_3GPP_MSG0_HIGH("activateInd in DS_PDN_CONTEXT_STATE_GOING_DOWN - Rejecting");

    for(bearer_index = 0;
        bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
        bearer_index++)
    {
      bearer_cntx_p =\
        (ds_bearer_context_s*)pdn_cntx_p->ds_pdn_context_dyn_p->\
          bearer_context_ptr_tbl[bearer_index];

      if(bearer_cntx_p == NULL)
      {
        /*----------------------------------------------------------------------- 
          This entry in the bearer context table is NULL. Other entries could
          have non-NULL values.
        -----------------------------------------------------------------------*/
        continue;
      }
      ds_bearer_cntxt_set_bearer_state(bearer_cntx_p,
                                   DS_BEARER_CONTEXT_STATE_GOING_DOWN);
    }// end of for loop

    return DS3G_FAILURE;

  } /* If PDN context is in going down */
  /* memset ds_3gpp_pdp_addr */
  memset(&ds_3gpp_pdp_addr, 0,sizeof(ds_umts_pdp_addr_type));
  /*-------------------------------------------------------------------------
     Verify that PDP type received in context accept is same as one
     requested in context request message.
  -------------------------------------------------------------------------*/
  /*lint -e731 */
  if( dsUmtsPdnCntx_verifyPdpType(mode_info_p,pdn_cntx_p) == FALSE )
  /*lint +e731 */
  {
    /*-----------------------------------------------------------------------
      PDP type mismatch. Return DS3G_FAILURE if this happens.
    -----------------------------------------------------------------------*/
     DS_3GPP_MSG0_ERROR("PDP type mismatch");
     pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
     return DS3G_FAILURE;
  } 

  /*-------------------------------------------------------------------------
      get the GW PS call info 
  -------------------------------------------------------------------------*/
  umts_call_info_p =
      (cm_gw_ps_call_info_s_type *)&(mode_info_p->info.gw_ps_call);

  /*----------------------------------------------------------------------
   Retrieve the protocol config options from CM struct mode_info_p
  ----------------------------------------------------------------------*/
  protocol_config_options_p = &(umts_call_info_p->config_options);


  /*-------------------------------------------------------------------------
    Default BCM to be UE only, if PCO contains BCM info from NW, it will be
    updated
  -------------------------------------------------------------------------*/
  flow_manager_ptr = ds_flow_manager_find_flow_manager(pdn_cntx_p);
  bearer_cntx_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;

  if(!ds_flow_manager_validate_flow_manager(flow_manager_ptr))
  {
    DS_3GPP_MSG0_ERROR("Invalid flow_manager_ptr ptr");
  }
  else if(ds_3gpp_nv_manager_get_bcm_info_validation_nv() == FALSE)
  {
    /** Primary set to MS only if BCM flag is set
        Default BCM NV() is set to TRUE. All BCM related actions are as per spec.
        if BCM NV is set to FALSE, UE will not update the BCM to MS only
        from secondary /modify accept if BCM PCO is not present
        It will inherit primary PDP BCM value. 
    **/
    if ( (ds_bearer_cntx_validate_bearer_context(bearer_cntx_p) == TRUE) &&
         (bearer_cntx_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_COMING_UP ))
    {
      DS_3GPP_MSG0_HIGH("BCM_info of primary to UE only, BCM NV is set to FALSE");
      flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = DS_3GPP_BCM_UE_ONLY;
    }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Initilizing BCM_info as UE only");
    flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = DS_3GPP_BCM_UE_ONLY;
  }

  /*-------------------------------------------------------------------------
    Make sure, this is PDP type IP call
  -------------------------------------------------------------------------*/  
  if( (umts_call_info_p->pdp_addr.pdp_type_org & 0x0F) == 0x01    )
  {

    DS_3GPP_MSG0_HIGH("Setting up UMTS PDP IP call");


    if( ( umts_call_info_p->pdp_addr.pdp_type_num !=  DS_3GPP_PDP_TYPE_IPV6_NUM) &&
        ( umts_call_info_p->pdp_addr.pdp_type_num !=  DS_3GPP_PDP_TYPE_IPV4V6_NUM) )
    {
      ds_3gpp_pdp_addr.ip_vsn = DS_IP_V4;
      memscpy( (void *)(&ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv4),
	  	sizeof(uint32),
                umts_call_info_p->pdp_addr.address,
                sizeof(uint32)); 
      /*-------------------------------------------------------------------------
        Validate & Store IP address provided by the NW
      -------------------------------------------------------------------------*/    
      if(ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv4 == 0)
      {
        /*---------------------------------------------------------------------
          No DHCP/static IP support, so reject call_connected
        ----------------------------------------------------------------------*/
        DS_3GPP_MSG0_ERROR(" NULL IP address provided by NW!");      
        return DS3G_FAILURE;
      }  
   
    }
#ifdef FEATURE_DATA_PS_IPV6
    else if( umts_call_info_p->pdp_addr.pdp_type_num ==  DS_3GPP_PDP_TYPE_IPV6_NUM)   
    {
      /*---------------------------------------------------------------------
       IID is the last 64 bits of the IP address. Hence it is copied into 
       the lower 64 bits of the PDP addr field.
       The IPV6 IID is present in octets 0-7 of the field "address" inside 
       the structure pdn_address_T
      ---------------------------------------------------------------------*/
      ds_3gpp_pdp_addr.ip_vsn = DS_IP_V6;
      memscpy((void *)(&(ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv6.\
                           in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET])),
              sizeof(ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv6.\
                           in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET]),              
             &(umts_call_info_p->pdp_addr.address[DS_3GPP_UMTS_IPV6_IID_OFFSET]),
              sizeof(uint64));  
      /*-------------------------------------------------------------------------
        Validate & Store IP address provided by the NW
      -------------------------------------------------------------------------*/    
      if( ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET] == 0)
      {
        /*---------------------------------------------------------------------
          No DHCP/static IP support, so reject call_connected
        ----------------------------------------------------------------------*/
        DS_3GPP_MSG0_ERROR(" NULL IID provided by NW!");      
        return DS3G_FAILURE;
      } 
  
    }
    else if( umts_call_info_p->pdp_addr.pdp_type_num ==  DS_3GPP_PDP_TYPE_IPV4V6_NUM)
    {
      ds_3gpp_pdp_addr.ip_vsn = DS_IP_V4V6;
      memscpy((void *)(&(ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv6.\
             in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET])),
              sizeof(ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv6.\
                     in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET]),
              &(umts_call_info_p->pdp_addr.\
                     address[DS_3GPP_UMTS_DUAL_IP_IPV6_IID_OFFSET]),
	          sizeof(uint64));    

      memscpy( (void *)(&ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv4),
	  	sizeof(uint32),
                umts_call_info_p->pdp_addr.address,              
	        sizeof(uint32)); 
      /*-------------------------------------------------------------------------
        Validate & Store IP address provided by the NW
      -------------------------------------------------------------------------*/    
      if(ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv4 == 0)
      {
        /*---------------------------------------------------------------------
          No DHCP/static IP support, so reject call_connected
        ----------------------------------------------------------------------*/
        DS_3GPP_MSG0_ERROR(" NULL IP address provided by NW!");      
        return DS3G_FAILURE;
      } 
      if(ds_3gpp_pdp_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET] == 0)
      {
        /*---------------------------------------------------------------------
          No DHCP/static IP support, so reject call_connected
        ----------------------------------------------------------------------*/
        DS_3GPP_MSG0_ERROR(" NULL IID provided by NW!");      
        return DS3G_FAILURE;
      } 
    }
#else
    else
    {
      DATA_MSG0_ERROR( "PDP type IPV6 not supported");      
      return DS3G_FAILURE;
    }
#endif /* FEATURE_DATA_PS_IPV6 */
    result = ds_pdn_cntxt_setup_pdp_ip_proto_stack(                                                
                                            pdn_cntx_p,
                                            &ds_3gpp_pdp_addr,
                                            protocol_config_options_p,
                                            end_params
                                            );
  }/* if PDP IP */
  else
  {
    DS_3GPP_MSG3_HIGH("Setting up UMTS PDP PPP call",0,0,0);
    result = ds_pdn_cntxt_setup_pdp_ppp_proto_stack(                                                
                                            pdn_cntx_p,
                                            &ds_3gpp_pdp_addr,
                                            protocol_config_options_p,
                                            end_params
                                            );
  }

  if (result == DS3G_SUCCESS)
  {
    /*---------------------------------------------------------------------
      Copy the group ID set by NAS.
      Will be used later for making the secondary calls
    ---------------------------------------------------------------------*/
    pdn_cntx_p->ds_pdn_context_dyn_p->pdp_group_id = mode_info_p->info.gw_ps_call.pdp_group_id;

    DS_3GPP_MSG1_HIGH("Group ID of PDN cntx : %d",
              mode_info_p->info.gw_ps_call.pdp_group_id);    
  }
  else
  {
    DATA_MSG1_ERROR("Failed to setup stack initialization on call connected inst:%x",
        pdn_cntx_p);
  }

  return ( result );
  
}


/*===========================================================================
FUNCTION DS_TDS_REPORT_UL_THROUGHPUT_QUALITY

DESCRIPTION
  This function invokes the TDS  layer functions to get
  the throughput quality

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE   - If throughput quality is obtained from lower layer successfully
  FALSE  - otherwise

SIDE EFFECTS  

===========================================================================*/
boolean ds_tds_report_ul_throughput_quality
(
   sys_modem_as_id_e_type           cm_subs_id,
   ds_3gpp_throughput_quality_s    *ul_throughput_quality
)
{
  boolean                   ret_val                 = FALSE;
#ifdef FEATURE_TDSCDMA
  tdsrrc_ul_rate_info_type  tds_throughput_quality;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Validate Argument
    -------------------------------------------------------------------------*/
  if(ul_throughput_quality == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL arguments to report TDS throughput quality");
    return ret_val;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(cm_subs_id))
  {
    return ret_val;
  }

  memset(&tds_throughput_quality, 0, sizeof(tdsrrc_ul_rate_info_type));
  /*------------------------------------------------------------------------- 
    Call WLI API to get UL stats
    -------------------------------------------------------------------------*/
#ifdef FEATURE_DUAL_WCDMA
  tdsrrc_get_ul_rate_info_sub(cm_subs_id, &tds_throughput_quality);
#else
  tdsrrc_get_ul_rate_info(&tds_throughput_quality);
#endif
  ul_throughput_quality->allowed_ul_rate = tds_throughput_quality.max_allowed_ul_rate;

  if (tds_throughput_quality.plus_flag == TRUE)
  {
    ul_throughput_quality->ul_throughput_quality =
                                          DS_SYS_HIGHER_THROUGHPUT_POSSIBLE;
  }
  else
  {
    ul_throughput_quality->ul_throughput_quality = 
                                          DS_SYS_HIGHER_THROUGHPUT_NOT_POSSIBLE;
  }

  DS_3GPP_MSG2_HIGH("TDS LL reports throughput quality %d UL allowed %d kbps",
                    ul_throughput_quality->ul_throughput_quality,
                    ul_throughput_quality->allowed_ul_rate );
  ret_val = TRUE;
#else
  DS_3GPP_MSG0_ERROR("TDS mode not supported");
  ret_val = FALSE;
#endif
  return ret_val;
}

/*===========================================================================
FUNCTION DSUMTSPS_PDP_IP_FILL_PPP_PARAMS_PPP_PARAMS_FROM_INFO

DESCRIPTION
  This function fills in the PPP authentication and IPCP packet information
  for a PDP-IP call from the PPP auht information passed in when bringing
  up the interface. This is the case for a Laptop PDP-IP call where in the
  middle of PPP negotiation, we trigger the call setup and pass the ppp info

PARAMETERS
    *orig_params_ptr      - Ptr to  Call origination parameters
    *pdp_profile_parms    - Profile parameters
     ppp_info_ptr         - pointer to the ppp auth/ipcp information
     pdp_call_type        - PDP Type of call to be originated
     subs_id              - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  orig_params_ptr is updated with the PPP packet information from the profile

===========================================================================*/
LOCAL boolean dsumtsps_pdp_ip_fill_ppp_params_from_info
(
  cm_gw_ps_orig_params_s_type        *orig_params_p,
  ds_umts_pdp_profile_type           *profile_p,
  const network_params_info_type     *ppp_info_p,
  sys_modem_as_id_e_type              subs_id
)
{
  boolean                    result = TRUE;
  uint32                     pdp_addr;
  net_cfg_params_mask_e_type valid_fields = 
                             (net_cfg_params_mask_e_type)0x00000000; 
  boolean                    static_ip_specified = FALSE;
  uint8                      rel_version = 
                               ds_3gpp_nv_manager_get_3gpp_rel_version();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check to see if the parameters pdp_profile_parms and ppp_info_ptr are
    valid.
  -------------------------------------------------------------------------*/
  if (profile_p == NULL)
    return DS3G_FAILURE;

  if(ppp_info_p == NULL)
  {
    DS_3GPP_MSG0_HIGH("Fill PP params from Profile");
    return dsumtsps_pdp_ip_fill_ppp_params_from_profile
             (orig_params_p, profile_p, subs_id);
  }
  /*-------------------------------------------------------------------------
   Fill in Protocol specific options : for IP, this would contain the PPP
   pkts (lcp, ipcp ) The purpose of the protocol configuration options
   information element is to transfer external network protocol options
   associated with a PDP context activation . The IE is defined in 10.5.6.3.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
   start off assuming that the field is not valid ( nothing filled in )
  -------------------------------------------------------------------------*/
  orig_params_p->config_options.valid  = FALSE;

  /*-------------------------------------------------------------------------
    Config protocol field is 0x80 : ref: TS24.008 sec 10.5.6.3
    MSB should be 1 (ext field) protocol field b0-b2 should be 0x0
    Initilize this field
  -------------------------------------------------------------------------*/
  orig_params_p->config_options.config_protocol   = PROT_CFG_IE_OCT3_VAL;
  orig_params_p->config_options.num_protocol_ids  = 0;

  /*-------------------------------------------------------------------------
   Check if a valid static IP is requested, if so update origination parameter 
  -------------------------------------------------------------------------*/
  if ( (rel_version < (uint8)DS_3GPP_REL8) &&
       ( profile_p->context.pdp_type != DS_UMTS_PDP_IPV6 ) &&
        (((ppp_info_p->valid_fields & NET_CFG_PRM_IP_ADDR_MASK) && 
        ((pdp_addr = ppp_info_p->ipcp_info.ip_address) != 0)) ||
       ((pdp_addr = profile_p->context.pdp_addr.pdp_addr.pdp_addr_ipv4)!= 0)) )
  {
    DS_3GPP_MSG3_HIGH("Setting PDP Static IP address to :%lx from info/profile",
                      pdp_addr,0,0);
    orig_params_p->pdp_addr.pdp_addr_len  = 4;
    orig_params_p->pdp_addr.address[0] = (byte)( (pdp_addr & 0xff000000) >> 24 );
    orig_params_p->pdp_addr.address[1] = (byte)( (pdp_addr & 0x00ff0000) >> 16 );
    orig_params_p->pdp_addr.address[2] = (byte)( (pdp_addr & 0x0000ff00) >> 8 );
    orig_params_p->pdp_addr.address[3] = (byte)( (pdp_addr & 0x000000ff) );

    static_ip_specified = TRUE;
  }

  /*----------------------------------------------------------------------
   Fill AUTH Parameters
  -------------------------------------------------------------------------*/
  if( (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)||
      (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK)||
      (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK) )
  {
    DS_3GPP_MSG1_HIGH("Inst:%d Auth type %d from Info",ppp_info_p->auth_info.auth_type);

    /*-------------------------------------------------------------------------
      Store the auth type from the ppp info in the pdp profile info.
    -------------------------------------------------------------------------*/
    (void) ds_pdn_cntxt_fill_auth_params_from_info (
                   &(orig_params_p->config_options),
                     profile_p, ppp_info_p, subs_id);
  }
  else
  {
    /*-------------------------------------------------------------------------
      Use the auth info from profile
    -------------------------------------------------------------------------*/
    (void) ds_pdn_cntxt_fill_auth_params_from_profile (
                    &(orig_params_p->config_options),
                      profile_p, subs_id);
  }

  /*-------------------------------------------------------------------------
    Request Primary and Secondary DNS addresses from the network.
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("IPCP DNS options specified");

  if((profile_p->context.pdp_type == DS_UMTS_PDP_IPV4) 
#ifdef FEATURE_DATA_PS_IPV6
     || (profile_p->context.pdp_type == DS_UMTS_PDP_IPV4V6)
#endif
     )
  {
    if( (ppp_info_p->valid_fields & NET_CFG_PRM_PRI_DNS_MASK)||
        (ppp_info_p->valid_fields & NET_CFG_PRM_SEC_DNS_MASK)||
        (ppp_info_p->valid_fields & NET_CFG_PRM_PRI_NBNS_MASK)||
        (ppp_info_p->valid_fields & NET_CFG_PRM_SEC_NBNS_MASK) )
    {
      valid_fields = (net_cfg_params_mask_e_type) (ppp_info_p->valid_fields);
#ifdef FEATURE_GSM_DTM
      /* In case of reduced PCO we need 
         to ignore secondary address requests.
      */
      if (rr_dtm_get_max_LAPDm_octets() != 0)
      {
        valid_fields &= ~(NET_CFG_PRM_SEC_DNS_MASK);
        valid_fields &= ~(NET_CFG_PRM_SEC_NBNS_MASK);
      }
#endif /* FEATURE_GSM_DTM */
  /*---------------------------------------------------------------------------
    We convert the DNS address from Little Endian to Big Endian before
    sending the request to OTA, since QMI sends information in Little Endian
    format, however modifications to the profile are sent in Big Endian format
  ---------------------------------------------------------------------------*/
      result = ds_pdn_cntxt_fill_ipcp_orig_params 
              (
               &(orig_params_p->config_options),
               TRUE,
               ds_3gpp_chng_byte_order(ppp_info_p->ipcp_info.primary_dns),
               ds_3gpp_chng_byte_order(ppp_info_p->ipcp_info.secondary_dns),
               TRUE,
               ds_3gpp_chng_byte_order(ppp_info_p->ipcp_info.primary_nbns),
               ds_3gpp_chng_byte_order(ppp_info_p->ipcp_info.secondary_nbns),
               valid_fields
             );

      if (!result)
      {
        /*---------------------------------------------------------------------
          If an error occurs in encoding pkt, do not include the pkt but
          proceed with call If we return error here, call setup would fail and
          the user would'nt know why?  and all subsequent call attemtps for the
          PDP ctxt would also fail (using the same params)
        ---------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("IPCP packet generation failed to copy from info");
      }
    }
    else
    {
      /*-------------------------------------------------------------------------
        Request Primary and Secondary DNS addresses from the network.
        Since we are populating values from profile, only request DNS address.
      -------------------------------------------------------------------------*/
      valid_fields = (net_cfg_params_mask_e_type)
                     (NET_CFG_PRM_PRI_DNS_MASK | NET_CFG_PRM_SEC_DNS_MASK);
#ifdef FEATURE_GSM_DTM
      /* In case of reduced PCO we need 
         to ignore secondary address requests.
      */
      if (rr_dtm_get_max_LAPDm_octets() != 0)
      {
        valid_fields &= ~(NET_CFG_PRM_SEC_DNS_MASK);
      }
#endif /* FEATURE_GSM_DTM */

      DS_3GPP_MSG3_HIGH("IPCP DNS options from profile P:%lx S:%lx",
                profile_p->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4,
                profile_p->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4,
                0);
      result = ds_pdn_cntxt_fill_ipcp_orig_params
             (
                 &(orig_params_p->config_options),
                 FALSE,
                 profile_p->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4,
                 profile_p->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4,
                 FALSE,
                 profile_p->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4,
                 profile_p->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4,
                 valid_fields
             );
      if (result == FALSE)
      {
        /*-------------------------------------------------------------------
        If an error occurs in encoding pkt, do not include the pkt but
        proceed with call If we return error here, call setup would fail and
        the user would'nt know why?  and all subsequent call attemtps for the
        PDP ctxt would also fail (using the same params)
         ------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("IPCP packet generation failed to copy from profile");
      }
    }

    /*-------------------------------------------------------------------------
      Check if we have to fill this protocol info pkt in the pdp 
      context request based on NV item 3gpp_rel_support,
      if mode is not LTE. For LTE always fill this container.
    -------------------------------------------------------------------------*/

    if ( rel_version < (uint8)DS_3GPP_REL8 )
    {
      DS_3GPP_MSG0_ERROR("IPV4 DNS container not filled, return FALSE"); 
      result = FALSE;
    }
    else
    {

      result = ds_3gpp_pdn_cntxt_fill_ipv4_dns_container
               (
                 &(orig_params_p->config_options)
               );
    }
    if (result == FALSE)
    {
      /*-------------------------------------------------------------------
       If an error occurs in encoding pkt, do not include the pkt but
      proceed with call If we return error here, call setup would fail and
      the user would'nt know why?  and all subsequent call attemtps for the
      PDP ctxt would also fail (using the same params)
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("Could not include DNS Server IPv4 addr req container");
    }
  }

#ifdef FEATURE_DATA_PS_IPV6
  if((profile_p->context.pdp_type == DS_UMTS_PDP_IPV6) ||
        (profile_p->context.pdp_type == DS_UMTS_PDP_IPV4V6))
  {
    result = ds_pdn_cntxt_fill_ipv6_dns_params   
             (
               &(orig_params_p->config_options)
             );


    if (!result)
    {
      /*---------------------------------------------------------------------
        If an error occurs in encoding pkt, do not include the pkt but
        proceed with call If we return error here, call setup would fail and
        the user would'nt know why?  and all subsequent call attemtps for the
        PDP ctxt would also fail (using the same params)
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("IPV6 DNS Genaration Failed");
    }
  }
#endif /* FEATURE_DATA_PS_IPV6*/

#ifndef FEATURE_DATA_NON_DEFAULT_CUSTOMIZATION
  /* Inclusion of IP addr alloc via NAS signaling container 
     will be removed for UMTS if FEATURE_DATA_NON_DEFAULT_CUSTOMIZATION
     is enabled. */
  if ( (profile_p->context.pdp_type != DS_UMTS_PDP_IPV6 ) && (!static_ip_specified) )
  {
    /*-------------------------------------------------------------------------
     We check profile for DHCP, only if static IP addr is not specified.
     Determine IP address allocation setting from the profile and fill the
     PCO accordingly. This determines, if we are using DHCP for addr alloc.       
    -------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------
      Check if we have to fill this protocol info pkt in the pdp 
      context request based on NV item 3gpp_rel_support,
      if mode is not LTE. For LTE always fill this container.
    -------------------------------------------------------------------------*/

    if ( rel_version < (uint8)DS_3GPP_REL8 )
    {
      DS_3GPP_MSG0_ERROR("IP Addr alloc field is not populated"); 
    }
    else
    {
      ds_pdn_cntxt_fill_address_alloc_param(
                                            &(orig_params_p->config_options),
                                              profile_p);
    }

  }
#endif /* FEATURE_DATA_NON_DEFAULT_CUSTOMIZATION */
  return DS3G_SUCCESS;
}

/*===========================================================================
FUNCTION DS_GPRS_REPORT_UL_THROUGHPUT_QUALITY

DESCRIPTION
  This function invokes the GPRS  layer functions to get
  the throughput quality

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE   - If throughput quality is obtained from lower layer successfully
  FALSE  - otherwise

SIDE EFFECTS  

===========================================================================*/
boolean ds_gprs_report_ul_throughput_quality
(
   sys_modem_as_id_e_type           cm_subs_id,
   ds_3gpp_throughput_quality_s    *ul_throughput_quality
)
{
  boolean                   ret_val       = FALSE;
#ifdef FEATURE_GSM_GPRS
  geran_bearer_data_rate_t geran_data_rate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ul_throughput_quality == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL Arguments to report GPRS UL throughput quality");
    return ret_val;
  }
  if (!ds3gsubsmgr_is_subs_id_valid(cm_subs_id))
  {
    return ret_val;
  }
  memset(&geran_data_rate, 0, sizeof(geran_bearer_data_rate_t));
  /*------------------------------------------------------------------------- 
    Get the allowed throughput from LTE Lower layers
    -------------------------------------------------------------------------*/
  geran_get_bearer_data_rates_as_id(&geran_data_rate, cm_subs_id);
  ul_throughput_quality->allowed_ul_rate = 
                        DSUTIL_BITS_TO_KBITS(geran_data_rate.max_tx_bearer_data_rate);

  /*------------------------------------------------------------------------- 
    Currently we do not have API's from Lower layers to get throughput
    quality. Report Unknown
    -------------------------------------------------------------------------*/
  ul_throughput_quality->ul_throughput_quality = DS_SYS_HIGHER_THROUGHPUT_UNKNOWN;

  DS_3GPP_MSG2_HIGH("GERAN LL reports throughput quality %d UL allowed %d kbps",
                    ul_throughput_quality->ul_throughput_quality,
                    ul_throughput_quality->allowed_ul_rate );
  ret_val = TRUE;
#else
  ret_val = FALSE;
#endif
  return ret_val;
}/* ds_gprs_report_ul_throughput_quality*/
/*===========================================================================
FUNCTION DS_WCDMA_REPORT_UL_THROUGHPUT_QUALITY

DESCRIPTION
  This function invokes the WCDMA layer functions to get
  the throughput quality

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE   - If throughput quality is obtained from lower layer successfully
  FALSE  - otherwise

SIDE EFFECTS  

===========================================================================*/
boolean ds_wcdma_report_ul_throughput_quality
(
   sys_modem_as_id_e_type           cm_subs_id,
   ds_3gpp_throughput_quality_s    *ul_throughput_quality
)
{

  boolean                     ret_val               = FALSE;  
#ifdef FEATURE_DATA_WCDMA_PS
  wl1_eul_stats_struct_type   wcdma_eul_stats;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Validate Argument
    -------------------------------------------------------------------------*/
  if(ul_throughput_quality == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid argument to report WCDMA UL t'put quality");
    return ret_val;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(cm_subs_id))
  {
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
    Call WLI API to get UL stats
    -------------------------------------------------------------------------*/
  if (wl1_eul_stats(&wcdma_eul_stats) == TRUE)
  {
    ul_throughput_quality->allowed_ul_rate = wcdma_eul_stats.allowed_datarate;
    
    if (wcdma_eul_stats.increase_data_rate_flag == TRUE)
    {
      ul_throughput_quality->ul_throughput_quality =
                                            DS_SYS_HIGHER_THROUGHPUT_POSSIBLE;
    }
    else
    {
      ul_throughput_quality->ul_throughput_quality = 
                                            DS_SYS_HIGHER_THROUGHPUT_NOT_POSSIBLE;
    }
    DS_3GPP_MSG2_HIGH("WCDMA EUL stats reports UL throughput quality %d UL Allowed"
                      " t'put %lu kbps", ul_throughput_quality->ul_throughput_quality,
                      ul_throughput_quality->allowed_ul_rate);

  }
  else
  {
    ul_throughput_quality->allowed_ul_rate        = 0;
    ul_throughput_quality->ul_throughput_quality  = DS_SYS_HIGHER_THROUGHPUT_UNKNOWN;
    DS_3GPP_MSG2_HIGH("No WCDMA EUL stats. Report throughput Quality %d UL allowed"
                      " tput %lu kbps",
                      ul_throughput_quality->ul_throughput_quality,
                      ul_throughput_quality->allowed_ul_rate );
  }

  ret_val = TRUE;
#else
  DS_3GPP_MSG0_ERROR("TDS mode not supported");
#endif
  return ret_val;
  
}
/*===========================================================================
FUNCTION DSUMTSPS_PDP_IP_FILL_PPP_PARAMS_PPP_PARAMS_FROM_PROFILE

DESCRIPTION
  This function fills in the PPP authentication and IPCP packet information
  for a PDP-IP call from the PDP profile specified for the call
    
PARAMETERS  
    *orig_params_p   - Ptr to  Call origination parameters 
    pdp_profile_p    - Ptr to the PDP profile parameters
    subs_id          - Subscription Id

DEPENDENCIES
  None.
  
RETURN VALUE
  none  

SIDE EFFECTS
  orig_params_ptr is updated with the PPP packet information from the profile
  
===========================================================================*/
LOCAL boolean dsumtsps_pdp_ip_fill_ppp_params_from_profile
(
  cm_gw_ps_orig_params_s_type  *orig_params_p,
  ds_umts_pdp_profile_type     *profile_p,
  sys_modem_as_id_e_type        subs_id  
)
{
  boolean                    result;
  net_cfg_params_mask_e_type valid_fields = 
      (net_cfg_params_mask_e_type)0x00000000; /* IPCP options to be requested*/
  uint32                     pdp_addr =0;
  boolean                    static_ip_specified = FALSE;
  uint8                      rel_version = 
                               ds_3gpp_nv_manager_get_3gpp_rel_version();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Fill in Protocol specific options : for IP, this would contain the PPP 
   pkts (lcp, ipcp ) The purpose of the protocol configuration options
   information element is to transfer external network protocol options 
   associated with a PDP context activation . The IE is defined in 10.5.6.3.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
   start off assuming that the field is not valid ( nothing filled in ) 
  -------------------------------------------------------------------------*/
  orig_params_p->config_options.valid  = FALSE; 
  
  /*-------------------------------------------------------------------------
    Config protocol field is 0x80 : ref: TS24.008 sec 10.5.6.3
    MSB should be 1 (ext field) protocol field b0-b2 should be 0x0 
    Initilize this field
  -------------------------------------------------------------------------*/
  orig_params_p->config_options.config_protocol   = PROT_CFG_IE_OCT3_VAL; 
  orig_params_p->config_options.num_protocol_ids  = 0;
    

  /*-------------------------------------------------------------------------
   Check if a valid static IP is requested, if so update origination parameter 
  -------------------------------------------------------------------------*/
  if ( (rel_version < (uint8)DS_3GPP_REL8) &&
       (profile_p->context.pdp_type != DS_UMTS_PDP_IPV6 ) &&
       ( (pdp_addr = profile_p->context.pdp_addr.pdp_addr.pdp_addr_ipv4)!= 0) )
  {
    DS_3GPP_MSG1_HIGH("Setting PDP Static IP address to :%lx from info/profile",
                      pdp_addr);
    orig_params_p->pdp_addr.pdp_addr_len  = 4;

    orig_params_p->pdp_addr.address[0] = (byte)( (pdp_addr & 0x000000ff) );
    orig_params_p->pdp_addr.address[1] = (byte)( (pdp_addr & 0x0000ff00) >> 8 );
    orig_params_p->pdp_addr.address[2] = (byte)( (pdp_addr & 0x00ff0000) >> 16 );
    orig_params_p->pdp_addr.address[3] = (byte)( (pdp_addr & 0xff000000) >> 24 );

    static_ip_specified = TRUE;
  }
    
  DS_3GPP_MSG3_HIGH(" Auth type %d",profile_p->auth.auth_type,0,0);

  /*-------------------------------------------------------------------------
    Use the auth info from profile
  -------------------------------------------------------------------------*/
  result = ds_pdn_cntxt_fill_auth_params_from_profile (
                  &(orig_params_p->config_options),
                    profile_p, subs_id);

  if(!result) {
    DS_3GPP_MSG0_ERROR("Auth Info generation failed ");
  }

  /*-------------------------------------------------------------------------
    Request Primary and Secondary DNS addresses from the network.
    Since we are populating values from profile, only request DNS address.
  -------------------------------------------------------------------------*/
  valid_fields = (net_cfg_params_mask_e_type)
    (NET_CFG_PRM_PRI_DNS_MASK | NET_CFG_PRM_SEC_DNS_MASK);

#ifdef FEATURE_GSM_DTM
  /* In case of reduced PCO we need 
     to ignore secondary address requests.
  */
  if (rr_dtm_get_max_LAPDm_octets() != 0)
  {
    valid_fields &= ~(NET_CFG_PRM_SEC_DNS_MASK);
    DS_3GPP_MSG0_HIGH("Secondary DNS container omitted");
  }
#endif /* FEATURE_GSM_DTM */

  /*--------------------------------------------------------------------------
    Handle error case and return failure before trying to fill IPV4/IPV6 
	DNS containers.
  ---------------------------------------------------------------------------*/
  if((profile_p->context.pdp_type != DS_UMTS_PDP_IPV4 ) 
#ifdef FEATURE_DATA_PS_IPV6
  && (profile_p->context.pdp_type != DS_UMTS_PDP_IPV6) && 
  (profile_p->context.pdp_type != DS_UMTS_PDP_IPV4V6)
#endif 
     )
    {
     DS_3GPP_MSG1_ERROR("Iface has invalid IP type %d",profile_p->context.pdp_type);
     return DS3G_FAILURE;
    }

  if((profile_p->context.pdp_type == DS_UMTS_PDP_IPV4) 
#ifdef FEATURE_DATA_PS_IPV6
     || (profile_p->context.pdp_type == DS_UMTS_PDP_IPV4V6)
#endif
     )
  {
    DS_3GPP_MSG3_HIGH("IPCP DNS options P:%lx S:%lx",
              profile_p->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4,
              profile_p->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4,
              0);
    result = ds_pdn_cntxt_fill_ipcp_orig_params
             (
               &(orig_params_p->config_options),
               FALSE,
               profile_p->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4,
               profile_p->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4,
               FALSE,
               profile_p->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4,
               profile_p->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4,
               valid_fields
             );
    if (result == FALSE)
    {
      /*-------------------------------------------------------------------
        If an error occurs in encoding pkt, do not include the pkt but
        proceed with call If we return error here, call setup would fail and
        the user would'nt know why?  and all subsequent call attemtps for the
        PDP ctxt would also fail (using the same params)
       ------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("IPCP packet generation failed");
    }

    /*-------------------------------------------------------------------------
      Check if we have to fill this protocol info pkt in the pdp 
      context request based on NV item 3gpp_rel_support,
      if mode is not LTE. For LTE always fill this container.
    -------------------------------------------------------------------------*/
    if ( rel_version < (uint8)DS_3GPP_REL8 )
    {
      DS_3GPP_MSG0_ERROR("IPV4 DNS container not filled"); 
      result = FALSE;
    }
    else
    {
      result = ds_3gpp_pdn_cntxt_fill_ipv4_dns_container
               (
                 &(orig_params_p->config_options)
               );
    }
    if (result == FALSE)
    {
      /*-------------------------------------------------------------------
       If an error occurs in encoding pkt, do not include the pkt but
      proceed with call If we return error here, call setup would fail and
      the user would'nt know why?  and all subsequent call attemtps for the
      PDP ctxt would also fail (using the same params)
      ------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("DNS Server IPv4 addr Req container could not be " \
               "included");
    }
  }
#ifdef FEATURE_DATA_PS_IPV6
  if((profile_p->context.pdp_type == DS_UMTS_PDP_IPV6) ||
          (profile_p->context.pdp_type == DS_UMTS_PDP_IPV4V6))
  {
    result = ds_pdn_cntxt_fill_ipv6_dns_params
             (
               &(orig_params_p->config_options)
             );

    if (result == FALSE)
    {
      /*---------------------------------------------------------------------
        If an error occurs in encoding pkt, do not include the pkt but
        proceed with call If we return error here, call setup would fail and
        the user would'nt know why?  and all subsequent call attemtps for the
        PDP ctxt would also fail (using the same params)
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("IPV6 DNS Genaration Failed");
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

#ifndef FEATURE_DATA_NON_DEFAULT_CUSTOMIZATION
   /* Inclusion of IP addr alloc via NAS signaling container 
     will be removed for UMTS if FEATURE_DATA_NON_DEFAULT_CUSTOMIZATION
     is enabled. */
  if ((profile_p->context.pdp_type != DS_UMTS_PDP_IPV6 ) && (!static_ip_specified) )
  {
    /*-------------------------------------------------------------------------
     We check profile for DHCP, only if static IP addr is not specified.
     Determine IP address allocation setting from the profile and fill the
     PCO accordingly. This determines, if we are using DHCP for addr alloc.       
    -------------------------------------------------------------------------*/
    if ( rel_version < (uint8)DS_3GPP_REL8 )
    {
      DS_3GPP_MSG0_ERROR("IP Addr alloc field is not populated"); 
    }
    else
    {
      ds_pdn_cntxt_fill_address_alloc_param(&(orig_params_p->config_options),
                                              profile_p);
    }

  }
#endif /* FEATURE_DATA_NON_DEFAULT_CUSTOMIZATION */

  return DS3G_SUCCESS;

}

/*===========================================================================
FUNCTION DSUMTSPS_PDP_IP_CALL_ORIGINATE_HDLR

DESCRIPTION
  This function to setup a PDP-IP call and is called by the mode specific
  call handler function dsumtsps_orig_call_hdlr

PARAMETERS
    *orig_params_ptr     - Ptr to  Call origination parameters
    pdp_profile_parms    - pointer to the pdp profile parameters
    pdn_cntx_type        - IP call type to originate
    *orig_info_ptr       - Ptr to Origination information (if any)

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Call parameters filled in.
  FALSE - Error

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsumtsps_pdp_ip_call_originate_hdlr
(
  cm_gw_ps_orig_params_s_type      *orig_params_p,
  ds_pdn_context_s                 *pdn_cntx_p,
  const ds_3gpp_pdn_call_info_type *orig_info_p,
  sys_modem_as_id_e_type           subs_id
)
{
  network_params_info_type *ppp_info=NULL;            /* ppp auth & ipcp info  */
  ds_umts_pdp_type_enum_type pdp_type;  
  ds_network_overrridden_ip_type_e  override_ip = PDP_UNDEFINED;
  void                         *fill_op_pco_ptr= NULL;
  ds_umts_pdp_profile_type     *profile_p = NULL; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }


  DS_3GPP_MSG0_HIGH("Originating Type PDP-IP ");

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return DS3G_FAILURE;
  }

  if((orig_params_p == NULL) || (orig_info_p == NULL))
  {
    DS_3GPP_MSG2_ERROR("orig_params_p:0x%x:orig_info_p:0x%x",
                       orig_params_p,orig_info_p);
    return DS3G_FAILURE;
  }

  profile_p = &(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile);

  if(profile_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Profile_p is NULL");
    return DS3G_FAILURE;
  }

  DS_3GPP_MSG0_HIGH("Originating Type PDP-IP ");

  /*-------------------------------------------------------------------------
   Fill in compression parameters.
  -------------------------------------------------------------------------*/
  orig_params_p->h_comp = ds_pdn_cntxt_conv_header_comp_from_pdp_t_to_sys_t(
                                    &profile_p->context.h_comp);
  orig_params_p->d_comp = ds_pdn_cntxt_conv_data_comp_from_pdp_t_to_sys_t(
                                    &profile_p->context.d_comp);

  /*-------------------------------------------------------------------------
    Fill in PDP address, The IE is defined in 10.5.6.4.
    For PDP type IP with dynamic IP address,
    no address is required in this information element
    If a static IP address is requested in the AT+CGDCONT command, use it
  -------------------------------------------------------------------------*/

  orig_params_p->pdp_addr.valid         = TRUE;
  orig_params_p->pdp_addr.pdp_type_org  = 1;      /* PDP org type - IETF*/

  pdp_type = profile_p->context.pdp_type;
  if(pdp_type == DS_UMTS_PDP_IPV4)
  {

    orig_params_p->pdp_addr.pdp_type_num  = 0x21; /* PDP type - IP-V4   */
    orig_params_p->pdp_addr.pdp_addr_len  = 0;    /* default-dynamic IP */
  }/* if (ds_pdn_context_type_e == DS_UMTS_PDP_TYPE_IPV4) */
  else if(pdp_type == DS_UMTS_PDP_IPV6)
  {
    orig_params_p->pdp_addr.pdp_type_num  = 0x57; /* PDP type - IP-V6   */
    orig_params_p->pdp_addr.pdp_addr_len  = 0;      /* default-dynamic IP */
  }
  else if(pdp_type == DS_UMTS_PDP_IPV4V6)
  {
  #ifdef FEATURE_DUAL_IP_BEARER 
      /* -----------------------------------------------------------------
         For Rel 11+, Check if an apn is network overridden, If it is
         overridden overwrite the IP type that needs to be sent out
         Also update the internal cache and apn table to reflect
         information correctly
       ------------------------------------------------------------------*/
      override_ip = ds_3gpp_network_override_hdlr_get_overridden_ip
                    (profile_p->context.apn, subs_id);

      DS_3GPP_MSG1_HIGH("Network_override_IP is %d", override_ip);

      switch(override_ip)
      {
         case PDP_IPV4:
           orig_params_p->pdp_addr.pdp_type_num  = 0x21;
           profile_p->context.pdp_type = DS_UMTS_PDP_IPV4;
           (void)ds_pdn_cntxt_update_apn_table(pdn_cntx_p->ds_pdn_context_dyn_p
                 ->pdp_profile.context.apn,DS_IPV4_ONLY, pdn_cntx_p); 
           break;

         case PDP_IPV6:
           orig_params_p->pdp_addr.pdp_type_num  = 0x57;
           profile_p->context.pdp_type = DS_UMTS_PDP_IPV6;
           (void)ds_pdn_cntxt_update_apn_table(pdn_cntx_p->ds_pdn_context_dyn_p
                 ->pdp_profile.context.apn,DS_IPV6_ONLY,pdn_cntx_p); 
           break;

         default:
           orig_params_p->pdp_addr.pdp_type_num  = 0x8D;
           break;
      }
      orig_params_p->pdp_addr.pdp_addr_len  = 0; 
    
  #else
    DS_3GPP_MSG0_ERROR("Dual IP is not supported at NAS level, failing the call");
    return DS3G_FAILURE;
#endif /* FEATURE_DUAL_IP_BEARER */
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Iface has invalid IP type %d",pdp_type);
    return DS3G_FAILURE;
  }
  if( (orig_info_p != NULL) &&
      (orig_info_p->info_ptr != NULL) &&
      ((network_params_info_type *)(orig_info_p->info_ptr))->valid_fields )
  {
    ppp_info =  (network_params_info_type *) orig_info_p->info_ptr;
    DS_3GPP_MSG0_HIGH("PPP Auth/IPCP info passed: Using it ");
  }
  if(dsumtsps_pdp_ip_fill_ppp_params_from_info(orig_params_p,
                                               profile_p,
                                               ppp_info,
                                               subs_id
                                               ) == DS3G_FAILURE)
  {
    DS_3GPP_MSG0_ERROR("pdp_ip_fill_ppp_params returned failure");
    return DS3G_FAILURE;
  }

#ifdef FEATURE_DATA_IMS
  /*-------------------------------------------------------------------------
    Get the request_pcscf_address_flag from profile registry to see if pcscf
    address should be requested for this profile.
  -------------------------------------------------------------------------*/
  /* Create a pcscf address request */
  if( ds_pdn_cntxt_gen_pcscf_address_request_packet(\
        &(orig_params_p->config_options), profile_p,subs_id) == DS3G_FAILURE)
  {
    return DS3G_FAILURE;
  }
  
  /*-------------------------------------------------------------------------
    Get the im_cn_flag from call table to see if im cn flag should be
    requested for this call.
    VRK:TODO Need to move to bearer cntx
  -------------------------------------------------------------------------*/
  if(profile_p->im_cn_flag == TRUE)
  {
    if( ds_bearer_cntxt_is_IMCN_call_up(subs_id) == TRUE)
    {
      DATA_MSG0( MSG_LEGACY_ERROR, "Error: Call with IM CN Flag already up");
      return DS3G_FAILURE;
    }
    else
    {
      if(ds_pdn_cntxt_gen_im_cn_subsystem_request_packet(
          &(orig_params_p->config_options)) == DS3G_FAILURE)
      {
        DS_3GPP_MSG0_HIGH("IM CN flag generate failed.");
        return DS3G_FAILURE;
      }
      DS_3GPP_MSG0_HIGH("IM CN flag generate success");
    }
  }
#endif

    /*-------------------------------------------------------------------------
    Fill the BCM container based on NV setting.
    -------------------------------------------------------------------------*/
  ds_pdn_cntxt_fill_bcm_indication(&(orig_params_p->config_options));

  /*-------------------------------------------------------------------------
    Fill IPV4 Link MTU Request if profile pdp_type is IPV4 or IPV4V6
  --------------------------------------------------------------------------*/
  ds_pdn_cntxt_fill_ipv4v6_link_mtu_container(
                               &(orig_params_p->config_options),
                               profile_p, subs_id);
  /*-----------------------------------------------------------------------
    Check if OP PCO contents would need to be populated, If so populate
    the OP PCO contents, query from ATP(only app for now)
  -------------------------------------------------------------------------*/
  fill_op_pco_ptr = ds_3gpp_op_pco_holder_search_apn_entry(pdn_cntx_p);

  if(fill_op_pco_ptr != NULL)
  {

    DS_3GPP_MSG0_HIGH("FILL OP PCO contents for UMTS");

    (void)ds_pdn_cntxt_fill_oper_pco_info_from_app(
       &(orig_params_p->config_options),fill_op_pco_ptr);
   
  }

  /*-------------------------------------------------------------------------
    Fill local address support in TFT container
  --------------------------------------------------------------------------*/
  ds_pdn_cntxt_fill_ipv4v6_local_address_support_container(
                               &(orig_params_p->config_options),
                               profile_p);

  if (ds3g_is_epdg_3g_ip_continuity_enabled 
        ((ps_sys_subscription_enum_type)
          ds3gsubsmgr_subs_id_cm_to_ds(subs_id)) != 0)
  {
    orig_params_p->request_type.valid = TRUE;

    if(pdn_cntx_p->ds_pdn_context_dyn_p->handover_attach_flag == TRUE)
    {
      orig_params_p->request_type.req_type = SM_HANDOVER;
    }
    else
    {
      orig_params_p->request_type.req_type = SM_INITIAL_REQUEST;
    }
  }
  else
  {
    orig_params_p->request_type.valid = FALSE;
  }

   DS_3GPP_MSG2_HIGH("Request Type Valid: %d, Request Type: %d",
                     orig_params_p->request_type.valid,
                     orig_params_p->request_type.req_type);

  return DS3G_SUCCESS;

}/* dsumtsps_pdp_ip_call_originate_hdlr() */

/*===========================================================================
FUNCTION DSUMTSPS_PDP_PPP_CALL_ORIGINATE_HDLR

DESCRIPTION
  This function to setup a PDP-PPP call and is called by the mode specific
  call handler function dsumtsps_orig_call_hdlr

PARAMETERS   :
    *orig_params_ptr   - Ptr to  Call origination parameters
    *pdp_profile_parms - Ptr to PDP profile parameters for the current call

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsumtsps_pdp_ppp_call_originate_hdlr
(
  cm_gw_ps_orig_params_s_type    *orig_params_p,
  const ds_umts_pdp_profile_type *profile_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("Originating Type PDP-PPP call");

  /*-------------------------------------------------------------------------
   fill PDP address : only dynamic addressing supported : overrule
   the address requested in the AT command & request a dynamic IP addr
   the packet data protocol address information element is to identify an
   address associated with a PDP. The IE is defined in 10.5.6.4.
   For PDP type PPP, no address is required inthis information element
  -------------------------------------------------------------------------*/

  orig_params_p->pdp_addr.valid         = TRUE;
  orig_params_p->pdp_addr.pdp_type_org  = 0;       /* PDP org type - ETSI*/
  orig_params_p->pdp_addr.pdp_type_num  = 1;       /* PDP type - PPP     */
  orig_params_p->pdp_addr.pdp_addr_len  = 0;       /* No IP addr req     */
  orig_params_p->pdp_addr.address[0]    = 0;       /* neg at PPP level   */

  /*-------------------------------------------------------------------------
   Fill in Protocol specific options : for PPP, this would be empty
   The purpose of the protocol configuration options
   information element is to transfer external network protocol options
   associated with a PDP context activation . The IE is defined in 10.5.6.3.

   NO protocol specific options for type PPP :
  -------------------------------------------------------------------------*/

  orig_params_p->config_options.valid            = FALSE;

  /*-------------------------------------------------------------------------
   Fill in compression parameters.
  -------------------------------------------------------------------------*/

  orig_params_p->h_comp = SYS_PDP_HEADER_COMP_OFF;
  orig_params_p->d_comp = ds_pdn_cntxt_conv_data_comp_from_pdp_t_to_sys_t(
                                    &profile_p->context.d_comp);

   DS_3GPP_MSG1_HIGH("Request Type Valid: %d, Request Type: %d",
                     orig_params_p->request_type.valid);

  orig_params_p->request_type.valid = FALSE;

  return DS3G_SUCCESS;

} /* dsumtsps_pdp_ppp_call_originate_hdlr() */

/*===========================================================================
FUNCTION dsUmtsPdnCntx_getOrigParams

DESCRIPTION
  This function provides call origination paramters.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration.

SIDE EFFECTS  

===========================================================================*/
LOCAL boolean dsUmtsPdnCntx_getOrigParams
( 
  ds_pdn_context_s                    *pdn_cntx_p,
  ds_3gpp_orig_params_s               *mm_orig_params_p,                          
  const ds_3gpp_pdn_call_info_type    *call_info
)
{ 
  uint32                      profile_number;
  ds_umts_pdp_profile_type    *profile_p;  
  boolean                     interm_result;
  ds_3gpp_iface_s             *ds3gpp_V4_iface_p; /* ptr to 3GPP iface tbl */  
  cm_gw_ps_orig_params_s_type *gw_ps_orig_params_p;
  ds_umts_pdp_type_enum_type  pdp_type;  
  uint8                       apn_encoded_length;
  boolean                     pdp_profile_error = FALSE;
  ds_bearer_context_s         *bearer_cntx_p = NULL;
  boolean                     result = DS3G_SUCCESS;      
  sys_modem_as_id_e_type      subs_id=SYS_MODEM_AS_ID_NONE; 
  ps_iface_type               *ps_iface_ptr = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  gw_ps_orig_params_p = mm_orig_params_p->orig_params_u.gw_ps_orig_params_ptr;  
  
  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }

 
  /*-------------------------------------------------------------------------
    Check if it is a MO or MT call
  -------------------------------------------------------------------------*/
  if(call_info != NULL)
  {
    /*-------------------------------------------------------------------------
    Return this is secondary PDP/Dedicated bearer
      MT secondary is not supported yet.
    -------------------------------------------------------------------------*/
    ps_phys_link_type *phys_link;
    bearer_cntx_p = ds_bearer_cntxt_get_bearer_cntxt((const void *)call_info);
 
    if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
    {
      ASSERT(0);
      return DS3G_FAILURE;
    }

    phys_link = &(bearer_cntx_p->phys_link);

    if ((bearer_cntx_p->ds_bearer_context_dyn_p->is_default != TRUE) ||
        (PS_PHYS_LINK_GET_STATE(phys_link) == PHYS_LINK_RESUMING))
    {
      DS_3GPP_MSG0_HIGH("PDN context orig params not reqd for SecPDP");
      return DS3G_SUCCESS;
    }
  }

  /*-----------------------------------------------------------------------
    Get the pointer to the profile from pdn cntx
  ----------------------------------------------------------------------*/
  profile_number = pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num;    
  profile_p = &(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile);    

  if( profile_p->context.valid_flg != TRUE )
  {
    DS_3GPP_MSG1_HIGH("PDP profile :%d not defined",profile_number);
    pdp_profile_error = TRUE;
  }
  DS_3GPP_MSG3_HIGH("ORIG_CALL_HDLR: Pri call activation, prf num [%d]",
         (byte)gw_ps_orig_params_p->profile_number, 0, 0);

#if 0 // Move to bearer cntx, reqd only for secondary
#ifdef FEATURE_SECONDARY_PDP
    if(!pdp_profile_error && (profile_p->context.secondary_flag == TRUE))
    {
      DS_3GPP_MSG1_HIGH("PDP profile:%d is secondary : error ",pdp_profile_number);
      pdp_profile_error = TRUE;
    }
#endif
#endif //if 0
       
  if( pdp_profile_error)
  {
    DATA_MSG0_ERROR( "PDP profile error : Failing call setup");
    return DS3G_FAILURE;
  }

  /*-----------------------------------------------------------------------
    We have a valid PDP profile now for the call.
    Store the pdp type. 
  -----------------------------------------------------------------------*/
  pdp_type = profile_p->context.pdp_type;

    /* Fail the MT PPP call if TE_MT_PDP is not defined */
#ifndef FEATURE_DATA_TE_MT_PDP
  if((pdp_type == DS_UMTS_PDP_PPP) && (call_info == NULL))
  {   
    DATA_MSG0_ERROR(  "GetOrigParams: FAIL MT, MT not supported for PDP-PPP" );
    return (DS3G_FAILURE);
  }
#endif

  /*-------------------------------------------------------------------------
    Validations for PPP call:
    1. Fail the PPP call, if current_mode is GPRS
    2. Fail the PPP call, if it is sockets call
    // VRK:TODO Need same check for RMNet    
  -------------------------------------------------------------------------*/
  if(pdp_type == DS_UMTS_PDP_PPP)
  {     
    if(ds_3gpp_is_context_pref_pkt_mode(pdn_cntx_p,SYS_SYS_MODE_GSM) == TRUE)
    {
      DATA_MSG0_ERROR(  "PDP type PPP call not allowed in GPRS mode");
      return (DS3G_FAILURE);
    }    

    if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV4_IFACE] != NULL)
    {
      ds3gpp_V4_iface_p =
        pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
        [DS_3GPP_PDN_CONTEXT_IPV4_IFACE];

      ps_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);

      if(PS_IFACE_PROC_ID_LOCAL == PS_IFACE_GET_PROC_ID(ps_iface_ptr))
      {
        DATA_MSG0_ERROR(  "PDP type PPP call not be originated from sockets");
        return (DS3G_FAILURE);
      }
    }
    else
    {
      DATA_MSG0_ERROR(  "V4 IFACE not setup for PDP type PPP call");
      return (DS3G_FAILURE);
    } 
    
  }/* (pdp_type == DS_UMTS_PDP_PPP) */     

  /*-----------------------------------------------------------------------
    Setup the Call Manager origination parameters; 
    dsumtsps_call_orig_param_setup()      
  -----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
    Send the group ID this context is associated with.
    VRK:TODO Send this only for secondary/qos
  -----------------------------------------------------------------------*/
  gw_ps_orig_params_p->pdp_group_id =
           (uint8)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_group_id;
  
  gw_ps_orig_params_p->profile_number = (uint16)profile_number;

  /*-------------------------------------------------------------------------
    Fill in the APN name    
    ds_pdn_cntxt_fillOriginationApn(profile_p,
                               gw_ps_orig_params_p);
  -------------------------------------------------------------------------*/
  

  gw_ps_orig_params_p->apn_name.valid = FALSE;
  memset(gw_ps_orig_params_p->apn_name.address, 0, MAX_APN_ADR_LEN);
  gw_ps_orig_params_p->apn_name.apn_addr_len  = 0;

  apn_encoded_length = ds_pdn_cntxt_store_encoded_apn(
    gw_ps_orig_params_p->apn_name.address, profile_p->context.apn);

  if(apn_encoded_length > 1 )
  {
    gw_ps_orig_params_p->apn_name.valid = TRUE;

    /*-----------------------------------------------------------------------
      Fill the total length of the encoded APN
    -----------------------------------------------------------------------*/
    gw_ps_orig_params_p->apn_name.apn_addr_len = apn_encoded_length;
	DS_3GPP_MSG3_HIGH("Filled in VALID APN Name:length is %d",apn_encoded_length,0,0);
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Profile does not have a VALID APN Name:length=%d",
		apn_encoded_length);	
  }

  memset(&(gw_ps_orig_params_p->config_options), 0, sizeof(protocol_cfg_options_T));
  /*-------------------------------------------------------------------------
    Fill in the rest of the call origination parameters specific to pdp type.
    IP: comp,pdp_addr,auth,ipcp,imcn,pcscf
    PPP: data_comp,pdp_addr(empty)
    VRK:TODO NOW Remove V6 featurization in dsumtspdpreg.h/ds_umts_pdp_type_enum_type
  -------------------------------------------------------------------------*/  
  switch( profile_p->context.pdp_type )
  {
    case DS_UMTS_PDP_IP:
#ifdef FEATURE_DATA_PS_IPV6
    case DS_UMTS_PDP_IPV6:
    case DS_UMTS_PDP_IPV4V6:
#endif /* FEATURE_DATA_PS_IPV6 */
      interm_result =  dsumtsps_pdp_ip_call_originate_hdlr(
                                              gw_ps_orig_params_p,
                                              pdn_cntx_p,                                              
                                              call_info,
                                              subs_id
                                              );
      break;
    case DS_UMTS_PDP_PPP:      
      interm_result =  dsumtsps_pdp_ppp_call_originate_hdlr(
                                              gw_ps_orig_params_p,
                                              profile_p
                                          ); 
      break;
    default:
      DATA_MSG1_ERROR("Invalid PDP type from profile type: %d",
                       profile_p->context.pdp_type );
      return ( DS3G_FAILURE );
  }

  if(interm_result != DS3G_SUCCESS )
  {
    DATA_MSG1_ERROR("Internal err in mode specific call orig type %d",
                     profile_p->context.pdp_type );
    return ( DS3G_FAILURE );
  }

    /*-------------------------------------------------------------------------
      Initialize the ipcp options structure with the default values requested
      by the caller.
      If we don't receive a DNS value from the network, we use these default
      addresses as our DNS addresses.
      For now, the identifer is set to 0.
      VRK:TODO should do this for V6 also
    -------------------------------------------------------------------------*/

#if 0 /* VRK:TODO Do we need this? */
#ifdef FEATURE_DATA_PS_IPV6    
    ds3gpp_iface_p->ipcp_options.ipcp4_options.identifier = 0;
    ds3gpp_iface_p->ipcp_options.ipcp4_options.gateway_addr = 0;

    ds3gpp_iface_p->ipcp_options.ipcp4_options.primary_dns =
                profile_ptr->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4;

    ds3gpp_iface_p->ipcp_options.ipcp4_options.secondary_dns =
                profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4;

    ds3gpp_iface_p->ipcp_options.ipcp4_options.primary_nbns=
                profile_ptr->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4;

    ds3gpp_iface_p->ipcp_options.ipcp4_options.secondary_nbns =
                profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4;
#else
    ds3gpp_iface_p->ipcp_options.ipcp4_options.identifier = 0;
    ds3gpp_iface_p->ipcp_options.ipcp4_options.gateway_addr = 0;

    ds3gpp_iface_p->ipcp_options.ipcp4_options.primary_dns =
                profile_ptr->dns_addr.primary_dns_addr;

    ds3gpp_iface_p->ipcp_options.ipcp4_options.secondary_dns =
                profile_ptr->dns_addr.secondary_dns_addr;

    ds3gpp_iface_p->ipcp_options.ipcp4_options.primary_nbns=
                profile_ptr->dns_addr.primary_dns_addr;

    ds3gpp_iface_p->ipcp_options.ipcp4_options.secondary_nbns =
                profile_ptr->dns_addr.secondary_dns_addr;
#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* if 0 */

  /* VRK:TODO Why do we need to save this, we don't use it..
  dsumtsps_call_tbl[call_instance].protocol_info_index =
                (uint8)gw_ps_orig_params_ptr->config_options.num_protocol_ids;
  */

  return ( result );
  
}

/*===========================================================================
FUNCTION dsUmtsPdnCntx_getAnswerParams

DESCRIPTION
  This function provides call origination paramters.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration.

SIDE EFFECTS  

===========================================================================*/
boolean dsUmtsPdnCntx_getAnswerParams
( 
  ds_pdn_context_s              *pdn_cntx_p,
  cm_ans_params_s_type          *answer_params,
  ds_3gpp_pdn_call_info_type    *call_info
)
{   
  ds_3gpp_orig_params_s          mm_orig_params;
  pdp_address_T                  *offered_mt_address;
  byte new_pdp_type;
  ds_umts_pdn_context_s *umts_pdn_cntxt_p=NULL;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }

  umts_pdn_cntxt_p = ((ds_umts_pdn_context_s*)
         (pdn_cntx_p->ds_pdn_context_dyn_p->vobj_data_ptr_tbl
           [DS_3GPP_UMTS_VOBJ]));

  if(!dsUmtsPdnCntx_validate_pdn_context(umts_pdn_cntxt_p))
  {
    DS_3GPP_MSG0_ERROR("Umts PDN cntxt passed is not valid");
      return DS3G_FAILURE;
  }

  memset(&mm_orig_params, 0, sizeof(ds_3gpp_orig_params_s));

  mm_orig_params.orig_params_u.gw_ps_orig_params_ptr = 
      &(answer_params->ans_params.gw_ps_ans);

  /*-------------------------------------------------------------------------
    Send the associated primary PDP context profile number
  -------------------------------------------------------------------------*/
  answer_params->ans_params.gw_ps_ans.assoc_pri_profile_num =
                     (uint16)(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num);
  
  DS_3GPP_MSG1_HIGH ("ANS_HDLR: Call act for MT req, profile num: %d",
             (byte)answer_params->ans_params.gw_ps_ans.assoc_pri_profile_num);

  
  offered_mt_address =&(umts_pdn_cntxt_p->ds_umts_pdn_context_dyn_p->offered_mt_address);

  /*-----------------------------------------------------------------------
    Setup the Call Manager origination parameters
  -----------------------------------------------------------------------*/
  if (DS3G_SUCCESS !=
       dsUmtsPdnCntx_getOrigParams (
         pdn_cntx_p,
         &mm_orig_params,
         call_info))
  {
    DATA_MSG1_ERROR("Error reported in CM parameter setup pdn_cntx_p :0x%x!",
                     pdn_cntx_p);
    answer_params->ans_params.gw_ps_ans.accept = FALSE;
    return DS3G_FAILURE;
    
  }
  /*-------------------------------------------------------------------------
    Put the offered address for the MT call in the answer params and set the
    accept flag to true.
  -------------------------------------------------------------------------*/
  if(( offered_mt_address != NULL ) && (offered_mt_address->valid == TRUE))
  {
    /*-------------------------------------------------------------------------
     We are not updating apn with profile so apn name will be same as in 
     activation request. The only way apn can differ is when profile has
     NULL apn and requested apn is not NULL(as we match any apn with NULL apn).
    -------------------------------------------------------------------------*/
    new_pdp_type = answer_params->ans_params.gw_ps_ans.pdp_addr.pdp_type_num;
    answer_params->ans_params.gw_ps_ans.pdp_addr = *offered_mt_address;
    answer_params->ans_params.gw_ps_ans.pdp_addr.pdp_type_num = new_pdp_type;
  }

  answer_params->ans_params.gw_ps_ans.accept = TRUE;
  DS_3GPP_MSG3_HIGH("Answering call with pdp_type %d", mm_orig_params.orig_params_u.gw_ps_orig_params_ptr->pdp_addr.pdp_type_num, 0, 0);
  return DS3G_SUCCESS;
 
}
/*===========================================================================
FUNCTION dsUmtsPdnCntx_incomingCall

DESCRIPTION
  This handler is invoked when the CM_CALL_EVENT_INCOM event is rx'ed from
    Call Mgr.
    The return value indicates the action to be taken: answer, notify user or
    reject call. The serial mode is changed only if the return value
    indicates answer.

DEPENDENCIES
   None

RETURN VALUE
   ds3g_incom_e_type value

SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
ds3g_incom_e_type dsUmtsPdnCntx_incomingCall
(
  cm_call_id_type             call_id,
  const ds_cmd_type          *cmd_ptr,
  cm_ans_params_s_type       *ans_params,
  cm_end_params_s_type       *end_params,
  sys_modem_as_id_e_type      subs_id
)
{ 
  ds3g_call_info_cmd_type          *call_info_cmd_ptr = NULL;
  ds_cmd_type                      *incom_cmd_ptr;         /* DS Task msg */
  boolean                          ds_3gpp_auto_answer_enable = FALSE;
  ds_3gpp_pdn_incom_call_info_type *incom_call_info_ptr = NULL;
#ifdef FEATURE_DATA_TE_MT_PDP
  uint8                     mt_index;
#endif /* FEATURE_DATA_TE_MT_PDP */

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR( "NULL pointer passed" );
    return DS3G_IGNORE;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /*-------------------------------------------------------------------------
    Post ifaceup command if AUTO ANSWER is enabled.    
  -------------------------------------------------------------------------*/  
  if (ds_3gpp_cfg_get_subsid( DS_3GPP_AUTO_ANSWER_ENABLE, 
                      (boolean *)&(ds_3gpp_auto_answer_enable), 
                       sizeof(ds_3gpp_auto_answer_enable),
                       subs_id) > 0) 
  {
    DS_3GPP_MSG1_HIGH("ds_3gpp_auto_answer_enable set to %d in EFS", 
                       ds_3gpp_auto_answer_enable);
  }
  if( TRUE == ds_3gpp_auto_answer_enable )
  {
    incom_cmd_ptr = 
      ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_incom_call_info_type));
    if( (incom_cmd_ptr == NULL) || (incom_cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return DS3G_REJECT_CALL;
    }

    incom_cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_BRING_UP_RSP;
    incom_call_info_ptr = 
      (ds_3gpp_pdn_incom_call_info_type*)incom_cmd_ptr->cmd_payload_ptr;

    incom_call_info_ptr->call_id = call_id;
    incom_call_info_ptr->apn_name = 
      call_info_cmd_ptr->mode_info.info.gw_ps_call.apn_name;
    incom_call_info_ptr->pdp_addr = 
      call_info_cmd_ptr->mode_info.info.gw_ps_call.pdp_addr;
    incom_call_info_ptr->ds_pdp_type = 
      dsumtsps_mt_cvt_nas_pdp_type_to_ds_fmt(
         incom_call_info_ptr->pdp_addr.pdp_type_num);

    ds_put_cmd(incom_cmd_ptr);

    return DS3G_ALERT_USER;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Auto answer not enabled on incomin call");

    /*-------------------------------------------------------------------------
        No embedded call here so no need to check for embedded call type 
    -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_TE_MT_PDP
    if ( dsumts_rmsm_lock_rm() )/* Keep rmsm locked until MT call finishes */
    {
      if( dsumtsps_mt_um_table_store_info( cmd_ptr, call_id, &mt_index ) )
      {
        dsumtsps_mt_umsm_post_ev( mt_index, 
                                  MT_UMSM_CALL_RING_EV, NULL );
        /*-----------------------------------------------------------------------
          RMSM was locked and marked for MT. Command to start RMSM has been 
          enqueued to DS Task, RMSM will now lock itself accordingly 
        -----------------------------------------------------------------------*/
        DS_3GPP_MSG2_HIGH("Rm acquired and marked for MT, ringing TE! cm_call_id\
                           [%d] stored at mt_index %d", call_id, mt_index);
    
        return DS3G_ALERT_USER;
      }
      dsumts_rmsm_unlock_rm();
    }
    /* Rm is not available or store in mt table failed */

    DS_3GPP_MSG1_HIGH("Release Rm and reject MT call! cm_call_id [%d]",
                       call_id);
#endif /* FEATURE_DATA_TE_MT_PDP */
    end_params->call_id = call_id;
    end_params->info_type = CM_CALL_MODE_INFO_GW_PS;
    end_params->end_params.gw_ps_end.cause = SM_FEATURE_NOT_SUPPORTED;
    return DS3G_REJECT_CALL;
  }
}/* dsUmtsPdnCntx_incomingCall */

/*===========================================================================
FUNCTION  DS_UMTS_PDN_CNTXT_BEARER_ACTIVATE_IND

DESCRIPTION
  This function accepts or rejects the dedicated bearer activation. 

PARAMETERS
  Call_id:          CM assigned Call ID.
  mode_info_ptr_i:  Mode specific information
  cm_act_res_p      Not used/passed in as NULL 
  subs_id           Susbcription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_umts_pdn_cntxt_bearer_activate_ind
(
  cm_call_id_type                  call_id,
  const cm_call_mode_info_s_type   *mode_info_ptr_i,
  cm_act_bearer_rsp_s_type         *cm_act_res_p,
  sys_modem_as_id_e_type           subs_id
)
{
  const cm_call_mode_info_s_type *mode_info_p = mode_info_ptr_i;    
  ds_pdn_context_s               *pdn_cntx_p = NULL;
  ds_bearer_context_s            *bearer_cntx_p = NULL;
  ds_bearer_context_s            *def_bearer_cntx_p = NULL;
  boolean                        result = DS3G_SUCCESS;   
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( mode_info_p == NULL )
  {
    DATA_ERR_FATAL("mode_info_p=NULL in pdnCntxBearerActivatedInd");
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Get the bearer context and the PDN context based on whether this is a 
    UE init call or a n/w init call
  -------------------------------------------------------------------------*/
  bearer_cntx_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return DS3G_FAILURE;
  }
  /*-------------------------------------------------------------------------
    We already have a bearer allocated for this call. Hence this is a UE init
    call (could be default or QOS)
  -------------------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)bearer_cntx_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Ensure that the PDN state is correct. If the PDN state is in GOING_DOWN,
    then we have to reject the bearer activation. 
    This is a case of a race between the Abort PDN connectivity handling and 
    the handling of Activate Bearer Ind. In such a case, we will reject the 
    bearer activation and not send out an abort. As a result of rejecting 
    bearer activation, we will get a CALL_END from CM during which we clean up
    our resources.
  -------------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_GOING_DOWN)
  {
    DS_3GPP_MSG0_ERROR("Got call connected while PDN going down - Rejecting");

    ds_bearer_cntxt_set_bearer_state(bearer_cntx_p,
                                   DS_BEARER_CONTEXT_STATE_GOING_DOWN);
    return DS3G_FAILURE;

  } /* If PDN context is in going down */



  /*-------------------------------------------------------------------------
   Verify if this is a dedicated bearer Activation
  -------------------------------------------------------------------------*/
  if(bearer_cntx_p->ds_bearer_context_dyn_p->is_default)
  {
    DS_3GPP_MSG0_ERROR("Received bearer Activ Ind for primary PDP");
    result = DS3G_FAILURE;
    return result;
  }

  /*---------------------------------------------------------------------
    Verify if the default bearer exists.
  -----------------------------------------------------------------------*/
  def_bearer_cntx_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;

  if(!ds_bearer_cntx_validate_bearer_context(def_bearer_cntx_p))
  {
    result = DS3G_FAILURE;
    //lte_info_ptr->esm_cause.valid = TRUE;
    //lte_info_ptr->esm_cause.esm_cause = NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
    return result;
  }    

  /*-------------------------------------------------------------------------
    Invoke the flow manager call activation function to handle bearer 
    activation indication
  -------------------------------------------------------------------------*/
  result = ds_flow_manager_nw_bearer_allocation_ind(
                                                pdn_cntx_p,
                                                bearer_cntx_p,
                                                mode_info_ptr_i);    
  if(result == DS3G_FAILURE)
  {
    DS_3GPP_MSG0_ERROR("Error in accepting the passed flow params");
    /*-------------------------------------------------------------------
       return FAILURE, DS will get call_end from CM
    ---------------------------------------------------------------------*/      
    return DS3G_FAILURE;
  }    

  return result;

} /* dsUmtspdnCntx_bearerActivateInd*/


/*===========================================================================
FUNCTION DS_PDN_CONTEXT_UMTS_INIT

DESCRIPTION
  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsPdnCntx_umtsInit( void )
{   
  
  ds_pdn_context_vftbl_s *umts_pdn_context_ftbl_p;

#ifdef FEATURE_WCDMA
  /* Populate the wcdma vftbl */
  umts_pdn_context_ftbl_p = &(ds_pdn_context_ftbl[DS_3GPPI_SYS_MODE_WCDMA]);
  memset(
      (void*)umts_pdn_context_ftbl_p,
      0x0,
      (sizeof(ds_pdn_context_vftbl_s) )
      );

  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, getOrigParams, \
                        dsUmtsPdnCntx_getOrigParams);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, activateInd, \
                        dsUmtsPdnCntx_activateInd);  
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, bearerActivateInd, \
                        ds_umts_pdn_cntxt_bearer_activate_ind);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, allocate_umts_pdn_context, \
                        dsUmtsPdnCntx_alloc_dyn_mem);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, free_umts_pdn_context, \
                        dsUmtsPdnCntx_free_dyn_mem);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, report_ul_throughput_quality,\
                        ds_wcdma_report_ul_throughput_quality);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, cost_of_modem_power,\
                        ds_wcdma_get_cost_of_modem_power);  
#endif /*FEATURE_WCDMA*/
  
#ifdef FEATURE_GSM_GPRS
  /* Populate the gsm vftbl */
  umts_pdn_context_ftbl_p = &(ds_pdn_context_ftbl[DS_3GPPI_SYS_MODE_GSM]);
  memset(
      (void*)umts_pdn_context_ftbl_p,
      0x0,
      (sizeof(ds_pdn_context_vftbl_s) )
      );
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_GSM, getOrigParams, \
                        dsUmtsPdnCntx_getOrigParams);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_GSM, activateInd, \
                        dsUmtsPdnCntx_activateInd);  
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_GSM, bearerActivateInd, \
                        ds_umts_pdn_cntxt_bearer_activate_ind);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_GSM, allocate_umts_pdn_context, \
                        dsUmtsPdnCntx_alloc_dyn_mem);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_GSM, free_umts_pdn_context, \
                        dsUmtsPdnCntx_free_dyn_mem);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_GSM, report_ul_throughput_quality,\
                        ds_gprs_report_ul_throughput_quality);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_GSM, cost_of_modem_power,\
                        ds_gprs_get_cost_of_modem_power);  
#endif /* FEATURE_GSM_GPRS */

#ifdef FEATURE_TDSCDMA
  /* Populate the tdscdma vftbl */
  umts_pdn_context_ftbl_p = &(ds_pdn_context_ftbl[DS_3GPPI_SYS_MODE_TDS]);
  memset(
      (void*)umts_pdn_context_ftbl_p,
      0x0,
      (sizeof(ds_pdn_context_vftbl_s) )
      );

  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_TDS, getOrigParams, \
                        dsUmtsPdnCntx_getOrigParams);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_TDS, activateInd, \
                        dsUmtsPdnCntx_activateInd);  
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_TDS, bearerActivateInd, \
                        ds_umts_pdn_cntxt_bearer_activate_ind);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_TDS, allocate_umts_pdn_context, \
                        dsUmtsPdnCntx_alloc_dyn_mem);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_TDS, free_umts_pdn_context, \
                        dsUmtsPdnCntx_free_dyn_mem);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_TDS, report_ul_throughput_quality,\
                        ds_tds_report_ul_throughput_quality);
  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_TDS, cost_of_modem_power,\
                        ds_tds_get_cost_of_modem_power);  

#endif/* FEATURE_TDSCDMA */
  /* Setup UMTS specific PDN cntx data blob */
  memset(
      (void*)umts_pdn_context_tbl,
      0x0,
      (sizeof(umts_pdn_context_tbl) )
      );

}
/*===========================================================================
FUNCTION dsUmtsPdnCntx_alloc_dyn_mem

DESCRIPTION
  This function allocates the  dyn memory associated with the index in umts_pdn_context_tbl
 
PARAMETERS 
  int loop_index
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean 
 
SIDE EFFECTS
  None

=============================================================================*/


boolean dsUmtsPdnCntx_alloc_dyn_mem(int loop_index)
{
  boolean ret_val = FALSE;
  ds_umts_pdn_context_dyn_s *umts_pdn_context_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((loop_index < 0) || (loop_index >= DS_3GPP_MAX_PDN_CONTEXT))
  {
    DS_3GPP_MSG1_ERROR("Index:%d is out of bounds",loop_index);
    return ret_val;
  }

  umts_pdn_context_dyn_p = (ds_umts_pdn_context_dyn_s *)umts_pdn_context_tbl[loop_index].
                                                         ds_umts_pdn_context_dyn_p;

  if (NULL != umts_pdn_context_dyn_p)
  {
    DS_3GPP_MSG1_ERROR("Dynamic memory already allocated at index:%d",
                        loop_index);

    modem_mem_free(umts_pdn_context_dyn_p, MODEM_MEM_CLIENT_DATA);
    umts_pdn_context_dyn_p = NULL;
    umts_pdn_context_tbl[loop_index].ds_umts_pdn_context_dyn_p = NULL;
  }

  if (umts_pdn_context_dyn_p == NULL)
  {
    umts_pdn_context_dyn_p = (ds_umts_pdn_context_dyn_s *)
    modem_mem_alloc(sizeof(ds_umts_pdn_context_dyn_s), MODEM_MEM_CLIENT_DATA);

    if (umts_pdn_context_dyn_p != NULL)
    {
      memset(umts_pdn_context_dyn_p,0,sizeof(ds_umts_pdn_context_dyn_s));
      umts_pdn_context_dyn_p->cookie = UMTS_PDN_CNTXT_VALID_COOKIE;
      umts_pdn_context_dyn_p->index = loop_index; 
      umts_pdn_context_tbl[loop_index].ds_umts_pdn_context_dyn_p = umts_pdn_context_dyn_p;
      DS_3GPP_MSG1_MED("Umts PDN context dyn mem allocated at index:%d",
                       umts_pdn_context_dyn_p->index);
      ret_val = TRUE;
    }
    else
    {
       DS_3GPP_MSG0_ERROR("Failed to allocate heap memory");
    }
  }

  return ret_val;
}
/*===========================================================================
FUNCTION dsUmtsPdnCntx_free_dyn_mem

DESCRIPTION
  This function frees the  dyn memory associated with the index in umts_pdn_context_tbl
 
PARAMETERS 
  int loop_index
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean 
 
SIDE EFFECTS
  None

=============================================================================*/
boolean dsUmtsPdnCntx_free_dyn_mem(int loop_index)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*----------------------------------------------------------------------------- 
  Don't care about the cookie because we are anyways freeing memory
  -----------------------------------------------------------------------------*/

  boolean ret_val=FALSE;
  ds_umts_pdn_context_s* umts_pdn_context_p=NULL;
  umts_pdn_context_p= &(umts_pdn_context_tbl[loop_index]);
  if((umts_pdn_context_p != NULL) && (umts_pdn_context_p->ds_umts_pdn_context_dyn_p != NULL))
  {
    DS_3GPP_MSG0_MED("Freeing dyn allocated PDN pointer");

    modem_mem_free(umts_pdn_context_p->ds_umts_pdn_context_dyn_p,
                 MODEM_MEM_CLIENT_DATA);

    umts_pdn_context_p->ds_umts_pdn_context_dyn_p = NULL;
    ret_val=TRUE;

  }
  else
  {
    DS_3GPP_MSG0_ERROR("Invalid umts pdn context passed");
  }

  return ret_val;
}

/*===========================================================================
FUNCTION dsUmtsPdnCntx_validate_pdn_context

DESCRIPTION
  This function checks if the umts_pdn_cntxt_p has been allocated dynamic memory

PARAMETERS
  umts_pdn_cntxt_p      - umts pdn context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean dsUmtsPdnCntx_validate_pdn_context
(
   ds_umts_pdn_context_s *umts_pdn_cntxt_p
)
{
  boolean ret_val = FALSE;
  ds_umts_pdn_context_dyn_s  *umts_pdn_cntxt_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(umts_pdn_cntxt_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter passed is INVALID");
    return ret_val;
  }

  umts_pdn_cntxt_dyn_p = umts_pdn_cntxt_p->ds_umts_pdn_context_dyn_p;

  if ((umts_pdn_cntxt_dyn_p != NULL) && 
       umts_pdn_cntxt_dyn_p->cookie == UMTS_PDN_CNTXT_VALID_COOKIE)

  {
    DS_3GPP_MSG0_MED("Umts Pdn context has dyn memory allocated");
    ret_val = TRUE;
  }

  return ret_val;
}


/*===========================================================================
  FUNCTION DSUMTSPS_CONFIGURE_PDP_PPP

  DESCRIPTION
    Gets PPP defaults and configures PPP for a PDP type PPP call.
    Based on the current mode calls function to configure PPP in that mode.

  PARAMETERS
    iface_ptr - Interface ptr for the call

  DEPENDENCIES
    None.

  RETURN VALUE
    0 - if ppp_start succeeds.

  SIDE EFFECTS
    None.

===========================================================================*/
int dsUmtsPdnCntx_configurePdpPpp
(
  ps_iface_type *iface_p,
  ppp_dev_enum_type ppp_dev
)
{  
  int                     ret_val;
  ds_pdn_context_s       *pdn_cntx_p;
  ds_bearer_context_s    *bearer_cntx_p;
  ppp_dev_opts_type      *ppp_config_info = NULL;     /* PPP config information    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the pdn cntx ptr from the iface ptr.
  -------------------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)(((ds_3gpp_iface_s*)\
                 (iface_p->client_data_ptr))->client_data_ptr);

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return -1;
  }

  bearer_cntx_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;  

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return -1;
  }
  /*-------------------------------------------------------------------------
    Get PPP defaults 
  -------------------------------------------------------------------------*/
  ppp_config_info = (ppp_dev_opts_type *)modem_mem_alloc(
                                          sizeof(ppp_dev_opts_type),
                                          MODEM_MEM_CLIENT_DATA_CRIT );

  if (ppp_config_info == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for dsUmtsPdnCntx_configurePdpPpp");
    return -1;
  }

  memset((void *)ppp_config_info, 0, sizeof(ppp_dev_opts_type));

  ppp_get_default_opts( ppp_config_info );

  ppp_config_info->ipcp_info.rx_iface_ptr   = iface_p;
  ppp_config_info->lcp_info.mode  = PPP_CP_PASSIVE;
  ppp_config_info->ipcp_info.mode = PPP_CP_PASSIVE;
  ppp_config_info->hdlc_mode      = HDLC_OFF;
  ppp_config_info->bridge_dev     = ppp_dev;
  
  /* Populated by Bearer cntx */
  //ppp_config_info.rx_f_ptr       = dswpsd_pdp_ppp_rx_um_data_cb;
  //ppp_config_info.rx_user_data   = (void *)call_instance;
  //ppp_config_info.tx_f_ptr       = dswpsd_pdp_ppp_tx_um_data_cb;
  //ppp_config_info.tx_user_data   = (void *)call_instance;
  // ppp_config_info.rx_signal      = dsumtsps_call_tbl[call_instance].rx_sig;
  dsUmtsBearerCntxt_configurePdpPpp( bearer_cntx_p, ppp_config_info ); 
  ret_val = ppp_start( PPP_UM_SN_DEV, ppp_config_info );
  modem_mem_free((void *)ppp_config_info, MODEM_MEM_CLIENT_DATA_CRIT );
  return ret_val;

} /* dsumtsps_configure_pdp_ppp() */

/*===========================================================================
FUNCTION ds_pdn_cntxt_setupPdpPPPprotoStack

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
LOCAL boolean ds_pdn_cntxt_setup_pdp_ppp_proto_stack
( 
  ds_pdn_context_s               *pdn_cntx_p,
  const ds_umts_pdp_addr_type    *ds_3gpp_pdp_addr_p,  
  const protocol_cfg_options_T   *protocol_config_options_p,
  cm_end_params_s_type           *end_params 
)
{    
  ds_3gpp_iface_s *ds3gpp_iface_v4_p = NULL; /* Pointer to the DS v4 Iface struct */
  ps_iface_type *ps_iface_ptr = NULL;
  ps_iface_state_enum_type ps_iface_curr_state = IFACE_STATE_INVALID;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }
  ds3gpp_iface_v4_p = 
    (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
     [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

  ps_iface_ptr = &(ds3gpp_iface_v4_p->ps_iface);

  /* Should not allow iface up indication to be posted if the iface
     state is (going) down, disabled or invalid */

  ps_iface_curr_state = ps_iface_state(ps_iface_ptr);

  if (ps_iface_curr_state != IFACE_DOWN 
      && ps_iface_curr_state != IFACE_GOING_DOWN 
      && ps_iface_curr_state != IFACE_DISABLED 
      && ps_iface_curr_state != IFACE_STATE_INVALID ) 
  {
    dsPdnContext_bringUpIPv4PSIface(ds3gpp_iface_v4_p);
  }
  else
  {
    return DS3G_FAILURE;
  }
  return DS3G_SUCCESS;
} /* ds_pdn_cntxt_setup_pdp_ppp_proto_stack */

/*===========================================================================
FUNCTION ds_umts_Pdn_Cntxt_hndl_unkown_cause


DESCRIPTION
   This function is used to handle cases for CAUSE CODE set to UNKNOWN

PARAMETERS

  Pdp_type_ctx_req: IP type requested to N/W
  pdp_type_num : IP type returned by N/W
  Pdn_cntxt_p  : PDN context pointer
  ip_support   : Updated IP type

DEPENDENCIES
  None.

RETURN VALUE 

  TRUE   DS3G_SUCCESS.
  FALSE  DS3G_FAILURE. 

SIDE EFFECTS

  None.


===========================================================================*/

boolean ds_umts_pdn_cntxt_hndl_unknown_cause
( 
  ds_umts_pdp_type_enum_type    pdp_type_ctx_req,
  byte                          pdp_type_num,
  ds_pdn_context_s              *pdn_cntx_p,
  ds_apn_ip_support_type_e      *ip_support
)
{

  boolean                  result = DS3G_SUCCESS;
  boolean                  is_redial_allowed = FALSE;  
  sys_modem_as_id_e_type   subs_id = SYS_MODEM_AS_ID_NONE;
/*-------------------------------------------------------------------------*/

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id) ||
     (ip_support == NULL))
  {
    DS_3GPP_MSG2_ERROR(" Pass in NULL Pdn Cntxt pointer %d ,ip_support ptr"
                      " %d ",pdn_cntx_p,ip_support);
    return DS3G_FAILURE;
  }

  is_redial_allowed = ds_3gpp_nv_manager_get_is_no_cause_code_redial_set(subs_id);

  DS_3GPP_MSG1_HIGH("Received invalid cause code for v4v6, is redial allowed %d",
                               is_redial_allowed); 

  switch(pdp_type_ctx_req)
  {
    case DS_UMTS_PDP_PPP:
        DS_3GPP_MSG0_HIGH("Received ESM cause in a ppp call, ignored");
        break;

    case DS_UMTS_PDP_IPV4:
        if( pdp_type_num == DS_3GPP_PDP_TYPE_IPV6_NUM )
        {
          DS_3GPP_MSG0_HIGH("UE negotiated IPv4,N/w assigned v6 with no "
                            "cause code");
          result = DS3G_FAILURE;
        }
        break;

    case DS_UMTS_PDP_IPV6:
        if( pdp_type_num == DS_3GPP_PDP_TYPE_IPV4_NUM )
        {
          DS_3GPP_MSG0_HIGH("UE negotiated IPv6,N/w assigned v4 and no "
                            "cause code");
          result = DS3G_FAILURE;
        }
        break;

      /*------------------------------------------------------------------
      sampatha: Corner case: If UE asks for IPV4V6 and N/W assigns a V4 or V6 
      without sending a cause code, then we need to set the 
      ip appropriately.
      -------------------------------------------------------------------*/

    case DS_UMTS_PDP_IPV4V6:
      {
         switch (pdp_type_num)
         {
           case DS_3GPP_PDP_TYPE_IPV4_NUM:
           {
             if (is_redial_allowed)
             {
               *ip_support = DS_IPV4_SINGLE_BEARER;
               DS_3GPP_MSG0_HIGH("UE negotiated IPv4v6, N/w assigned v4 set IP "
                                 "to IPV4 SB");          
             }
             else
             {
               *ip_support =  DS_IPV4_ONLY;
               DS_3GPP_MSG0_HIGH("UE negotiated IPv4v6, N/w assigned v4 set IP "
                                 "to IPV4 ONLY");

             }

             ds_pdn_cntxt_dissociate_iface(pdn_cntx_p, *ip_support);
             break;
           }

           case DS_3GPP_PDP_TYPE_IPV6_NUM:
           {
             if (is_redial_allowed)
             {
               *ip_support = DS_IPV6_SINGLE_BEARER;

               DS_3GPP_MSG0_HIGH("UE negotiated IPv4v6, N/w assigned v6 set IP "
                       "to IPV6 SB");          
             }
             else
             {
               *ip_support =  DS_IPV6_ONLY;
               DS_3GPP_MSG0_HIGH("UE negotiated IPv4v6, N/w assigned v6 set IP "
                                 "to IPV6 ONLY");
             }

             ds_pdn_cntxt_dissociate_iface(pdn_cntx_p, *ip_support);
             break;
           }

           case DS_3GPP_PDP_TYPE_IPV4V6_NUM:
           {
             DS_3GPP_MSG0_MED("UE requested IPv4v6, N/w assigned IPV4V6");
           }
           break;

           default:
             result = DS3G_FAILURE;
             DATA_ERR_FATAL("Unknown PDP type.");
             break;
         }
         break;
      }
      break;

    default:
      result = DS3G_FAILURE;
      DATA_ERR_FATAL("Unknown PDP type.");
      break;  
    }
   return result;

}/*ds_umts_pdn_cntxt_hndl_unknown_cause*/


/*===========================================================================

                                UNIT TEST

===========================================================================*/
#endif /* defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS) */
