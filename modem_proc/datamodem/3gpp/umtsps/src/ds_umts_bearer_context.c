
/*!
  @file
  ds_umts_bearer_context.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/src/ds_umts_bearer_context.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/13   nd      Added support for filling PCO of MT calls(primary/secondary)
                   from incoming call request and not from profile(See 23.060).
08/10/12   ss      Delegating RAT specific handling during call_end.
06/19/12   nd      MT PDP pkt call.
06/28/12   ss      Fixed the issue of WCDMA context_transfer_hdlr not getting
                   registered.
03/22/12   ss      Addition of RAB_FAILURE and ESM local cause codes.
10/27/11   nd      Fixed Klocwork errors.
10/05/11   ttv     Added DSDS changes for new mode handler.
09/23/11   ss      Fixed the issue of incorrect mapping of the SM_INTERNAL
                   cause codes in PDP_ACTIVATION_REJECT.
09/13/11   ttv     Changes to add src_stat_desc as profile parameter.
09/09/11   rr      Added support for fallback from Dual IP to single IP when nw rejects 
                   with specific cause codes in UMTS mode
09/05/11   ss      Changes to support RPM.
07/27/11   ttv     Added changes required for source statistics descriptor
                   addition.
07/21/11   ttv     Added changes to map CM_CALL_END_OFFLINE to 
                   PS_NET_DOWN_REASON_OFFLINE.
03/24/11   sa      Added MT PDP Auto answer support. 
03/09/11   ttv     Added code to map the CM call end cause code with PS 
                   cause codes.
03/02/11   ss      Added support for configurable RAB_REESTAB RETRY.
02/23/11   ttv     Added support for watermark tracing.
01/31/11   ss      Added support for reporting bearer tech type in WCDMA 
                   and GPRS.
12/17/10   ss      support for Rel 7 extended QoS values.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys.h"
#include <stringl/stringl.h>
#include "cm.h"
#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)
#include "ds_umts_bearer_context.h"
#include "ds_umts_pdn_context.h"
#include "ds3gsubsmgr.h"
#include "ds3gflowmgr.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "dsumtspsqos.h"
#include "ds3gprofile_validator.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_umts_bearer_flow_manager.h"
#include "ds_umts_flow_manager.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ps_iface_defs.h"
#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */
#ifdef FEATURE_DATA_LTE
#include "ds_eps_bearer_context.h"
#endif /* FEATURE_DATA_LTE */
#include "ps_iface_ioctl.h"
#include "ps_iface_defs.h"
#include "ds_3gpp_op_pco_hdlr.h"
#include "modem_mem.h"
#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#endif /* FEATURE_QXDM_WM_TRACING */

#include "ds3gpp_ext_msg.h"
#include "dsmsgr.h"
#include "dsutil.h"

#include "ds_3gpp_apn_switch_mgr.h"
#include "ds_3gpp_apn_table.h"
#include "rabm_cm.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds_3gpp_cc_sp_throt_config.h"
#include "ds3gutil.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*----------------------------------------------------------------------------
  Global used to store dial string for socket call, max dial string siz with 
  NULL is defined by NV_PKT_DIAL_STRING_SIZE - Defined in ds_3gpp_hdlr.c
----------------------------------------------------------------------------*/


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*---------------------------------------------------------------------------
  This is the UMTS bearer context table that is used to store UMTS bearer
  context related info
---------------------------------------------------------------------------*/
ds_umts_bearer_context_s umts_bearer_context_tbl[DS_3GPP_MAX_BEARER_CONTEXT] = {{NULL}};

/*---------------------------------------------------------------------------
  This is the function table to carry out bearer level functions for WCDMA
  and GPRS modes.

  We define an instance of this structure for all sys modes. We will however
  fill the table for WCDMA and GSM modes only.
---------------------------------------------------------------------------*/
ds_umts_bearer_context_ftbl_s umts_bearer_context_ftbl[DS_3GPPI_SYS_MODE_MAX] = {{NULL}};

#define DS_UMTS_BEARER_CTXT_VFR_CALL(rval, func, subs_id, ...)  \
            DS_3GPPI_VFR_CALL(umts_bearer_context_ftbl, rval, func, subs_id, \
                              __VA_ARGS__)

#define DS_UMTS_BEARER_CTXT_VFR_CALL_NO_ARG(rval, func, subs_id)  \
            DS_3GPPI_VFR_CALL_NO_ARG(umts_bearer_context_ftbl, rval, func, subs_id)

#define DS_UMTS_BEARER_CTXT_VF_CALL(func, subs_id, ...)  \
            DS_3GPPI_VF_CALL(umts_bearer_context_ftbl, func, subs_id, __VA_ARGS__)

#define DS_UMTS_BEARER_CTXT_VFR_CALL_PER_BEARER(rval, func, bearer_context_p, ...)  \
            DS_3GPPI_VFR_CALL_PER_BEARER(umts_bearer_context_ftbl, rval, func, bearer_context_p, __VA_ARGS__)

#define DS_UMTS_BEARER_CTXT_VF_CALL_PER_BEARER(func, bearer_context_p, ...)  \
            DS_3GPPI_VF_CALL_PER_BEARER(umts_bearer_context_ftbl, func, bearer_context_p, __VA_ARGS__)

#define DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER(func, bearer_context_p )  \
            DS_3GPPI_VF_CALL_NO_ARG_PER_BEARER(umts_bearer_context_ftbl, func, bearer_context_p)

/*---------------------------------------------------------------------------
  Variable to store cell service indication.
---------------------------------------------------------------------------*/
static cm_cell_srv_ind_s_type ds_3gpp_cell_srv_ind[DS3GSUBSMGR_SUBS_ID_COUNT];

/*---------------------------------------------------------------------------
  UMTS net down reason type. This type would map the call end reason from CM
  to PS_IFACE cause code
----------------------------------------------------------------------------*/
#define DSUMTS_NET_DOWN_REASON_TYPE      6

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

LOCAL void dsUmtsBearerCntxt_performResume
(
  ds_bearer_context_s*  bearer_context_p
);

#if defined(FEATURE_DATA_LTE)
LOCAL void dsUmtsBearerCntxt_performResumeValidation
(
  sys_sys_mode_e_type           new_mode,
  sys_sys_mode_e_type           old_mode,
  sys_modem_as_id_e_type        subs_id
);
#endif /*FEATURE_DATA_LTE*/

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_BRINGUP

DESCRIPTION
  This function is used to bring up a call. This is called from the mode 
  agnostic bearer context in response to the phys_link_up command.

PARAMETERS
  IN    
    info_p           - Used to extract the dial string.
    call_info_p      - call info such as call_instance(i.e. bearer_inst)
    useDefaultDialStr- This is the boolean which states if we should use the
                       default dial str or the one from the info_p.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS (TRUE) on Successful call initiation with CM
  DS3G_FAILURE (FALSE) on any failure

SIDE EFFECTS
  None.

VRK: TODO we are not using this
===========================================================================*/
LOCAL boolean dsUMTSBearerCntxt_bringUp(void                 *info_p,
                                  ds_3gpp_pdn_call_info_type *call_info_p,                                                      
                                  boolean                    useDefaultDialStr);


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_DECODEPROTOCOLPARAMS

DESCRIPTION
  This function decodes the IMS protocol parameters received for a PDP-IP call.

PARAMETERS
   bearer_context_p- Pointer to the bearer context we are working on
   *mode_info_ptr  - Pointer to the mode-specific call information provided
                     by Call Manager
DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS : Successful setup
  DS3G_FAILURE : Setup error

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsUmtsBearerCntxt_decodeProtocolParams
(
  ds_bearer_context_s            *bearer_context_p,
                                      /* Index into array of call states  */
  const cm_call_mode_info_s_type *mode_info_ptr 
)
{
  word    num_pids    = 0;                       /* Number of Protocol IDs */
  word    protocol_id = 0;                                  /* Protocol ID */
  boolean valid;                          /* Protocol options valid or not */
  byte    index       = 0;                       /* Protocol options index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  valid = mode_info_ptr->info.gw_ps_call.config_options.valid;

  if (valid == FALSE)
  {
    DS_3GPP_MSG1_HIGH("decode params bearer cntx:%x : Valid = FALSE",
              (uint32)bearer_context_p->ds_bearer_context_dyn_p->
                      client_data_ptr);
    return DS3G_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Get the number of protocol IDs received. Max is NUM_PROTOCOL_ID.
  -------------------------------------------------------------------------*/
  num_pids = mode_info_ptr->info.gw_ps_call.config_options.num_protocol_ids;

  /*-------------------------------------------------------------------------
    Process the protocol IDs received.
  -------------------------------------------------------------------------*/
  while (num_pids > 0)
  {
    /*----------------------------------------------------------------------
      Get the Protocol ID.
    ----------------------------------------------------------------------*/
    protocol_id = mode_info_ptr->info.gw_ps_call.config_options.
              procol_info[index].protocol_id[0] << 8;
    protocol_id |= mode_info_ptr->info.gw_ps_call.config_options.
              procol_info[index].protocol_id[1];

    switch (protocol_id)
    {

    case IM_CN_SUBSYSTEM_PROTOCOL:
        DS_3GPP_MSG2_HIGH("For Bearer cntx %d Decode: Protocol ID %d",
             (uint32)bearer_context_p->ds_bearer_context_dyn_p->
                          client_data_ptr,protocol_id);
        /*-------------------------------------------------------------------
           Decode IM CN Subsystem packet
        -------------------------------------------------------------------*/
        if(mode_info_ptr->info.gw_ps_call.config_options.
            procol_info[index].protocol_len != 0)
        {
          DS_3GPP_MSG3_HIGH("Could not decode IM CN pkt. Bearer cntx %d: "
                            "Proto ID %d, protocol len = %d",
                   (uint32)bearer_context_p->ds_bearer_context_dyn_p->
                            client_data_ptr, protocol_id, 
                   mode_info_ptr->info.gw_ps_call.config_options.
                   procol_info[index].protocol_len);
          bearer_context_p->ds_bearer_context_dyn_p->im_cn_subsystem_flag = FALSE;
        }
        else
        {
          bearer_context_p->ds_bearer_context_dyn_p->im_cn_subsystem_flag = TRUE;
        }
        break;

      default:
         DS_3GPP_MSG2_HIGH("Bearer cntx:%d Protocol need not be processed. %d",
                   (uint32)bearer_context_p->ds_bearer_context_dyn_p->
                           client_data_ptr,protocol_id);

    }
    index++;
    num_pids--;
  }

  return DS3G_SUCCESS;
} /* dsUmtsBearerCntxt_decodeProtocolParams() */
#endif // FEATURE_DATA_IMS


/*===========================================================================
STATIC FUNCTION DSUMTSPS_MAP_INTER_LAYER_EVENT_CAUSE_CODES

DESCRIPTION
  

PARAMETERS
  map_rule     - Rule for mapping cause-codes
  *output_ptr  - ptr to location holding converted cause-code
  *input_ptr   - ptr to location with cause-codes to be converted

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
static boolean dsUmtsBearerCntx_map_cause_codes
(
  ds3gpp_inter_layer_map_e map_rule,
  void *output_ptr,
  const void *input_ptr
)
{
  ps_extended_info_code_enum_type *ps_eic_output_ptr; /*PS EXTENDED INFO CODES*/
  ps_iface_net_down_reason_type *ps_indr_output_ptr;  /*PS IFACE NET DOWN REASON */
  ds_3gpp_down_reason_t *down_reason_p;
  
  const sm_cm_status_T *sm_cm_status_input_ptr;
  const sm_status_T *sm_status_input_ptr;
  const cm_call_end_e_type *cm_call_end_e_input_ptr;
  const ds3g_call_info_cmd_type *ds_call_info_ptr;
  const cm_call_mode_info_s_type *mode_info_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(map_rule)
  {
    case CALL_END_DS_CAUSE_CODES_TO_PS_EIC:

      ps_eic_output_ptr = (ps_extended_info_code_enum_type *) output_ptr;
      cm_call_end_e_input_ptr = (const cm_call_end_e_type *) input_ptr;

      DS_3GPP_MSG1_MED("Mapping CALL_END_DS_CAUSE_CODES_TO_PS_EIC, input_cause_code:%d",
              (uint32)(*cm_call_end_e_input_ptr));

      switch(*cm_call_end_e_input_ptr)
      {
        case CM_CALL_END_INTERNAL_ERROR:
        *ps_eic_output_ptr = PS_EIC_QOS_INTERNAL_ERROR;
        break;

        case CM_CALL_END_NO_SRV:
        *ps_eic_output_ptr = PS_EIC_QOS_INTERNAL_CALL_ENDED;
        break;

        default:
          DS_3GPP_MSG0_ERROR("Unable to map SM Int to PS_EIC, Unknown code!");
          *ps_eic_output_ptr = PS_EIC_QOS_INTERNAL_UNKNOWN_CAUSE_CODE;
      }

    break;/*map_rule: CALL_END_DS_CAUSE_CODES_TO_PS_EIC*/

    case CALL_END_CM_CAUSE_CODES_TO_PS_EIC:

      ps_eic_output_ptr = (ps_extended_info_code_enum_type *) output_ptr;
      cm_call_end_e_input_ptr = (const cm_call_end_e_type *) input_ptr;

      DS_3GPP_MSG1_MED("Mapping CALL_END_CM_CAUSE_CODES_TO_PS_EIC, input_cause_code:%d",
              (uint32)(*cm_call_end_e_input_ptr));

      switch(*cm_call_end_e_input_ptr)
      {
        case CM_CALL_END_LL_CAUSE:
        case CM_CALL_END_CLIENT_END:
          *ps_eic_output_ptr = PS_EIC_QOS_INTERNAL_CALL_ENDED;
        break;

        case CM_CALL_END_NETWORK_END:
          *ps_eic_output_ptr = PS_EIC_QOS_NETWORK_CALL_ENDED;
        break;

        default:
          DS_3GPP_MSG0_ERROR("Unable to map SM Int to PS_EIC, Unknown code!");
          *ps_eic_output_ptr = PS_EIC_QOS_INTERNAL_UNKNOWN_CAUSE_CODE;
      }

    break;/*map_rule: CALL_END_CM_CAUSE_CODES_TO_PS_EIC*/

    case PDP_MODIFY_SM_INTERNAL_CAUSE_CODES_TO_PS_EIC:

      ps_eic_output_ptr = (ps_extended_info_code_enum_type *) output_ptr;
      sm_cm_status_input_ptr = (const sm_cm_status_T *) input_ptr;

      DS_3GPP_MSG1_MED("Mapping PDP_MODIFY_SM_INTERNAL_CAUSE_CODES_TO_PS_EIC, input_cause_code:%d",
              (uint32)(*sm_cm_status_input_ptr));

      switch(*sm_cm_status_input_ptr)
      {
        case INVALID_CONNECTIN_ID:
        case INVALID_NSAPI:
        case INVALID_PRI_NSAPI:
        case INVALID_FIELD:
          *ps_eic_output_ptr = PS_EIC_QOS_INVALID_PARAMS;
        break;

        case NO_GPRS_CONTEXT:
        case PDP_LOWERLAYER_ERROR:
        case PDP_DUPLICATE:
        case UE_RAT_CHANGE:
        case PPD_UNKNOWN_REASON:
        case PDP_MODIFY_COLLISION:
        case SNDCP_FAILURE:
          *ps_eic_output_ptr = PS_EIC_QOS_INTERNAL_ERROR;
        break;

        case PDP_ESTABLISH_MAX_TIMEOUT:
        case PDP_ACTIVATE_MAX_TIMEOUT:
        case PDP_MODIFY_MAX_TIMEOUT:
        case PDP_INACTIVE_MAX_TIMEOUT:
          *ps_eic_output_ptr = PS_EIC_QOS_TIMED_OUT_OPERATION;
        break;

        default:/*pass SM code to MSG*/
          DS_3GPP_MSG0_ERROR("Unable to map SM Int to PS_EIC, Unknown code!");
          *ps_eic_output_ptr = PS_EIC_QOS_INTERNAL_UNKNOWN_CAUSE_CODE;
      }
    break;/*map_rule: PDP_MODIFY_SM_INTERNAL_CAUSE_CODES_TO_PS_EIC*/

    case PDP_MODIFY_SM_NETWORK_CAUSE_CODES_TO_PS_EIC:

      ps_eic_output_ptr = (ps_extended_info_code_enum_type *) output_ptr;
      sm_status_input_ptr = (const sm_status_T *) input_ptr;

      DS_3GPP_MSG1_MED("Mapping PDP_MODIFY_SM_NETWORK_CAUSE_CODES_TO_PS_EIC, input_cause_code:%d",
              (uint32)(*sm_status_input_ptr));

      switch(*sm_status_input_ptr)
      {
        case SM_SEMANTIC_ERR_IN_TFT:
        case SM_SYNTACTIC_ERR_IN_TFT:
        case SM_SEMANTIC_ERR_IN_FILTER:
        case SM_SYNTACTIC_ERR_IN_FILTER:
          *ps_eic_output_ptr = PS_EIC_QOS_INVALID_PARAMS;
        break;

        case SM_INSUFFICIENT_RESOURCES:
          *ps_eic_output_ptr = PS_EIC_QOS_INSUFFICIENT_NET_RESOURCES;
        break;

        case SM_SERVICE_OPTION_NOT_SUPPORTED:
        case SM_SERVICE_NOT_SUBSCRIBED:
        case SM_SERVICE_OUT_OR_ORDER:
          *ps_eic_output_ptr = PS_EIC_QOS_NETWORK_SVC_NOT_AVAILABLE;
        break;

        default:
          DS_3GPP_MSG0_ERROR("Unable to map SM Netw to PS_EIC, Unknown code!");
          *ps_eic_output_ptr = PS_EIC_QOS_NETWORK_UNKNOWN_CAUSE_CODE;
      }
    break;/*map_rule: PDP_MODIFY_SM_NETWORK_CAUSE_CODES_TO_PS_EIC*/

    case CALL_END_DS_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES:

      ps_indr_output_ptr = (ps_iface_net_down_reason_type *) output_ptr;
      cm_call_end_e_input_ptr = (const cm_call_end_e_type *) input_ptr;

      DS_3GPP_MSG1_MED("Mapping CALL_END_DS_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES, input_cause_code:%d",
              (uint32)(*cm_call_end_e_input_ptr));
    
      switch(*cm_call_end_e_input_ptr)
      {
        case CM_CALL_END_INTERNAL_ERROR:
        *ps_indr_output_ptr = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        break;

        case CM_CALL_END_OFFLINE:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_OFFLINE;
          break;

        case CM_CALL_END_NO_SRV:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_NO_SRV;

        default:
          DS_3GPP_MSG0_ERROR("EVENT_CAUSE_CODES: Unable to map DS Int to PS_INDR, Unknown code!");
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
      }

    break;/*map_rule: CALL_END_DS_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES*/

    case CALL_END_CM_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES:
      down_reason_p = (ds_3gpp_down_reason_t *) output_ptr;
      down_reason_p->reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
      ps_indr_output_ptr = (ps_iface_net_down_reason_type *) 
                            &(down_reason_p->reason.ps_iface_down_reason);
      ds_call_info_ptr = (const ds3g_call_info_cmd_type *) input_ptr;
      mode_info_ptr = &ds_call_info_ptr->mode_info;

      DS_3GPP_MSG3_MED("Mapping CALL_END_CM_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES, input_cause_code:%d",
              (uint32)(ds_call_info_ptr->end_status), 0, 0);

      switch(ds_call_info_ptr->end_status)
      {
        /* GSM/WCDMA Spec codes */
        case CM_CALL_END_CONF_FAILED:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_CONF_FAILED;
          break;

        case CM_CALL_END_INCOM_REJ:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_INCOM_REJ;
          break;

        case CM_CALL_END_NO_GW_SRV:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_NO_GW_SRV;
          break;

  /* This end status will be used by call manager to end when the pending GW call
     which has not been sent to Nas during IRAT ; We fill the reason_T  to DS_3GPP_EXTENDED_NET_DOWN_REASON_T
    and reason.internal_reason to UE_GW_L_RAT_CHANGE in order to redial */

        case CM_CALL_END_IRAT_PENDING_CALL:
          down_reason_p->reason_T = DS_3GPP_EXTENDED_NET_DOWN_REASON_T;
          down_reason_p->reason.internal_reason = UE_GW_L_RAT_CHANGE;
          break;

        case CM_CALL_END_NO_FULL_SRV:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_NO_SRV;
          break;
        /* Codes not specific to any RAT */
        case CM_CALL_END_LL_CAUSE:

          if (SM_INTERNAL_CAUSE == 
              mode_info_ptr->info.gw_ps_call.pdp_cause_type)
          {
            switch( mode_info_ptr->info.gw_ps_call.cause.int_cause )
            {
              case PDP_ACTIVATE_MAX_TIMEOUT:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PDP_ACTIVATE_MAX_RETRY_FAILED;
                break;

              case SM_ILLEGAL_MS:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_ILLEGAL_MS;
                break;
    
              case SM_ILLEGAL_ME:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_ILLEGAL_ME;
                break;
    
              case SM_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED;
                break;
    
              case SM_GPRS_SERVICES_NOT_ALLOWED:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED;
                break;
    
              case SM_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK;
                break;
    
              case SM_IMPLICITLY_DETACHED:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_IMPLICITLY_DETACHED;
                break;
    
              case SM_PLMN_NOT_ALLOWED:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PLMN_NOT_ALLOWED;
                break;
    
              case SM_LA_NOT_ALLOWED:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_LA_NOT_ALLOWED;
                break;
    
              case SM_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN;
                break;
    
              case NO_GPRS_CONTEXT:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_NO_GPRS_CONTEXT;
                break;
    
              case PDP_DUPLICATE:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PDP_DUPLICATE;
                break;
    
              case UE_RAT_CHANGE:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_UE_RAT_CHANGE;
                break;
    
              /* This will come only for PDP activation reject */
              case SM_CONGESTION:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_CONGESTION;
                break;
    
              /* Both PDP activation reject and deactivation */
              case SM_NO_PDP_CONTEXT_ACTIVATED:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_NO_PDP_CONTEXT_ACTIVATED;
                break;
    
              /* Both PDP activation reject and deactivation */
              case SM_ACCESS_CLASS_DSAC_REJECTION:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_ACCESS_CLASS_DSAC_REJECTION;
                break;

              /* PDP activation reject and deactivation */
              case SM_MESSAGE_EXCEED_MAX_L2_LIMIT:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_MESSAGE_EXCEED_MAX_L2_LIMIT;
                break;
              /*This will come only for NAS/Lowerlayer service request rejects*/
              case SM_NAS_SRV_REQ_FAILURE:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_SM_NAS_SRV_REQ_FAILURE;
                break;
/* This end_status (CM_CALL_END_LL_CAUSE) is used by Nas when it tries to end the GW call which is sent by CM to NAS during irat;
   with end_staus as CM_CALL_END_LL_CAUSE ;with other parameters as UE_GW_L_RAT_CHANGE (mode_info_ptr->info.gw_ps_call.cause.int_cause)
   and SM_INTERNAL_CAUSE (mode_info_ptr->info.gw_ps_call.pdp_cause_type) */
              case UE_GW_L_RAT_CHANGE:
                 down_reason_p->reason_T = DS_3GPP_EXTENDED_NET_DOWN_REASON_T;
                 down_reason_p->reason.internal_reason = UE_GW_L_RAT_CHANGE;
                 break;

              /* No PDP exists with the given Connection Id while doing 
                 Modification/Deactivation OR Activation for already Active PDP */
              case INVALID_CONNECTIN_ID:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_INVALID_CONNECTION_ID;
                break;

              /* NSAPIs exceeds max while PDP activation. Invalid 
                 Modify/Deactivation Request by CM for ConId. While
                 activating Secondary when PDP already active with same
                 connection Id, Reject the ACTIVATE req */
              case INVALID_NSAPI:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_INVALID_NSAPI;
                break;

              /* Primary context for NSAPI Doesn’t exist, Reject the SEC
                 ACTIVATE req con_id */
              case INVALID_PRI_NSAPI:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_INVALID_PRI_NSAPI;
                break;

              /* Unable to Encode OTA message for MT PDP or Deactivate PDP */
              case INVALID_FIELD:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_INVALID_FIELD;
                break;

              /* When RAB is not established by lower layers during 
                 Activation/Modification/Deactivation */
              case RAB_SETUP_FAILURE:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_RAB_SETUP_FAILURE;
                break;

              /* On Expiry of PDP establish timer with MAX (5) retries */
              case PDP_ESTABLISH_MAX_TIMEOUT:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PDP_ESTABLISH_MAX_TIMEOUT;
                break;
               
              /* On Expiry of PDP Modify timer with MAX (4) retries */
              case PDP_MODIFY_MAX_TIMEOUT:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PDP_MODIFY_MAX_TIMEOUT;
                break;

              /* on Expiry of PDP Deactivate timer with MAX (4) reties */
              case PDP_INACTIVE_MAX_TIMEOUT:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PDP_INACTIVE_MAX_TIMEOUT;
                break;

              /* PDP activation failed because of RRC_ABORT or Forbidden PLMN */
              case PDP_LOWERLAYER_ERROR:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PDP_LOWERLAYER_ERROR;
                break;

              /*  Local Deactivation 
                  SM_NATIONAL_ROAMING_NOT_ALLOWED: Roaming not allowed
                  SM_NO_SUITABLE_CELLS_IN_LA: No Suitable Cells In Location Area */
              case PPD_UNKNOWN_REASON:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PPD_UNKNOWN_REASON;
                break;
               
              /* MO PDP Modify collision when MT PDP is already in progress */
              case PDP_MODIFY_COLLISION:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PDP_MODIFY_COLLISION;
                break;
               
              /* PDP_MBMS_REQUEST received when PDP activation is already 
                 PDP_ACTIVE_PENDING on same Conn Id */
              case PDP_MBMS_REQUEST_COLLISION:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_PDP_MBMS_REQUEST_COLLISION;
                break;

              /* MBMS activation is already pending and PDP_MBMS_REQUEST is triggered */
              case MBMS_DUPLICATE:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_MBMS_DUPLICATE;
                break;

              /* Internal cause for call end due to PS DETACH  */
              case SM_PS_DETACHED:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_SM_PS_DETACHED;
                break;

              /* Internal cause to indicate that Radio resource is not available */
              case SM_NO_RADIO_AVAILABLE:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_SM_NO_RADIO_AVAILABLE;
                break;

              /* Internal cause to indicate abort because service is not available */
              case SM_ABORT_SERVICE_NOT_AVAILABLE:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_SM_ABORT_SERVICE_NOT_AVAILABLE;
                break;

              default:
                *ps_indr_output_ptr = PS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
                DS_3GPP_MSG1_ERROR( "unknown SM_INTERNAL cause-code [%d]", 
                                    mode_info_ptr->info.gw_ps_call.cause.int_cause);
            }/* switch() internal cause */
    
            DS_3GPP_MSG1_MED( "SM_INTERNAL_CAUSE, cause_code [%d]",
                              (uint32)(*ps_indr_output_ptr));
          }
          else
          {
            *ps_indr_output_ptr = PS_NET_DOWN_REASON_INTERNAL_CALL_ENDED;
            DS_3GPP_MSG1_MED( "cause_code [%d]",
                              (uint32)(*ps_indr_output_ptr));
          }
          break;

        case CM_CALL_END_CLIENT_END:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_CLIENT_END;
          break;

        case CM_CALL_END_CC_REJECT:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_CALL_CONTROL_REJECTED;
          break;

        case CM_CALL_END_NO_SRV:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_NO_SRV;
          break;

        case CM_CALL_END_FADE:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_FADE;
          break;

        case CM_CALL_END_REL_NORMAL:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_REL_NORMAL;
          break;

        case CM_CALL_END_ACC_IN_PROG:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_ACC_IN_PROG;
          break;

        case CM_CALL_END_ACC_FAIL:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_ACC_FAIL;
          break;

        case CM_CALL_END_REDIR_OR_HANDOFF:
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_REDIR_OR_HANDOFF;
          break;

        case CM_CALL_END_OFFLINE:
         *ps_indr_output_ptr = PS_NET_DOWN_REASON_OFFLINE;
         break;

        case CM_CALL_END_NETWORK_END:
          /* Map directly to SM cause codes */
          DS_3GPP_MSG1_MED( "CM_CALL_END_NETWORK_END, cause_type [%d]",
                   (uint32)(mode_info_ptr->info.gw_ps_call.pdp_cause_type));
          switch( mode_info_ptr->info.gw_ps_call.pdp_cause_type )
          {
            case SM_NETWORK_CAUSE:
            
              if (mode_info_ptr->info.gw_ps_call.cause.ext_cause == 
                    SM_LOCAL_PDP_DEACTIVATION)
              {
                *ps_indr_output_ptr = 
                  PS_NET_DOWN_REASON_SM_INTERNAL_PDP_DEACTIVATION;
              }
              else
              {
              *ps_indr_output_ptr = 
                       (ps_iface_net_down_reason_type)TYPE_REASON_VAL(
                                     DSUMTS_NET_DOWN_REASON_TYPE, 
                                     (mode_info_ptr->info.gw_ps_call.cause.ext_cause));
              }
              DS_3GPP_MSG1_MED( "SM_NETWORK_CAUSE, cause_code [%d]",
                       (uint32)(*ps_indr_output_ptr) );
              break;
                  
            default:
              *ps_indr_output_ptr = PS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
              DS_3GPP_MSG1_ERROR( "unknown cause_type from CM/SM [%d]", 
                           mode_info_ptr->info.gw_ps_call.pdp_cause_type);
          }
          break;

        default:
          DS_3GPP_MSG0_ERROR("Unable to map CM Int to PS_INDR, Unknown code!");
          *ps_indr_output_ptr = PS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
      }

    break;/*map_rule: CALL_END_CM_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES*/

    default:
      DS_3GPP_MSG0_ERROR("Mapping Rule not implemented!");
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_Free

DESCRIPTION
  This function cleans up the data associated with a bearer context and
  frees it. 


PARAMETERS
  IN
    bearer_context_p   - Pointer to the bearer context we are working on
    pdn_context_p      - Pointer to the pdn context
    down_reason        - reason why the call ended.
    ps_eic             - cause code.
    call_was_connected - Whether the call was connected or not

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsUmtsBearerCntxt_Free(

   ds_bearer_context_s        *bearer_context_p,
   ds_pdn_context_s*             pdn_context_p,
   ps_iface_net_down_reason_type down_reason,
   ps_extended_info_code_enum_type ps_eic,
   boolean                    call_was_connected
)
{
   ds_flow_context_s *flow_context_p = NULL;
   ds_3gpp_down_reason_t  net_down_reason;

   
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

   /*  Tear Down flows */
   /* Also Add unbind flow to phys link */ 
   if (ds_bearer_flow_manager_go_null_flow(bearer_context_p) == FALSE)
   {
      DS_3GPP_MSG0_ERROR("Error calling goNullFlow");
   }

    /* Release the memory for the flow context
       and clear the array entries for this bearer do this from a
       flow context context */
   if(ds_bearer_flow_manager_delete_all_flow_context
                                 (pdn_context_p,bearer_context_p) == FALSE)
   {
      DS_3GPP_MSG0_ERROR("Error when deleting flow contexts");
   }

   //Check if there are any pending flow contexts for this bearer.
   //If so free them also.   
   flow_context_p = ds_flow_manager_find_pending_flow_context_for_pdn(
                        bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,
                        bearer_context_p->ds_bearer_context_dyn_p->sdf_id
                    );

   if (flow_context_p != NULL)
   {
      ds_flow_context_ps_flow_go_null_ind(flow_context_p, PS_EIC_NOT_SPECIFIED);

      ds_bearer_flow_manager_delete_flow_context(
         bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,
         bearer_context_p,
         flow_context_p
      );

      ds_flow_manager_remove_pending_flow_context(
         bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,
         flow_context_p
      );      
   }   

   if(bearer_context_p->ds_bearer_context_dyn_p->is_default)
   {
      /*---------------------------------------------------------------------
      Unbind the phys_link from the default flow 
      ---------------------------------------------------------------------*/
      ds_pdn_cntxt_unbind_default_flows((void*)pdn_context_p);
      /*---------------------------------------------------------------------
      Disable signal if this is the last phys link
      ---------------------------------------------------------------------*/
      ps_disable_sig(bearer_context_p->rx_sig);
   }/* bearer_context_p->ds_bearer_context_dyn_p->is_default */    
  
   /*-------------------------------------------------------------------------
       Free the bearer instance
   -------------------------------------------------------------------------*/
   ds_bearer_cntxt_free_bearer_inst(bearer_context_p);

   /*---------------------------------------------------------------------
     Each enqueue func_ptr needs to be reset when bearer inst is freed.
     This needs to be done to avoid race conditions when phys_link_gone is
     posted and flow gets enabled on the phys_link leading to each_enq_f
     called when the bearer is getting freed.
   ---------------------------------------------------------------------*/
   ps_phys_link_gone_ind_ex(&(bearer_context_p->phys_link), ps_eic);
   DS_3GPP_MSG0_HIGH("Phys link gone");     

   ps_phys_link_enable_flow(&(bearer_context_p->phys_link), DS_FLOW_UMTS_MASK);

   /*-------------------------------------------------------------------------
      Send the bearer down indication to PDN context with the down_reason and
      call_was connected flag. The PDN context (based on these) decides an
      appropriate course of action.
   -------------------------------------------------------------------------*/
   DS_3GPP_MSG1_HIGH("BEARER DOWN IND CB for bearer :%x",
                     bearer_context_p);
   net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
   net_down_reason.reason.ps_iface_down_reason = down_reason;
   if (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
       bearer_down_ind_cb != NULL)
   {
      bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
        bearer_down_ind_cb(
         bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,\
		 bearer_context_p,
        net_down_reason,
		 call_was_connected
      );
   }  

} /* dsUmtsBearerCntxt_Free */

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_HandleEnqueuedTxData 
 
DESCRIPTION
  This function handles the specific case when data is enqueued on
  the default bearer which is in the down state but atleast one dedicated bearer
  attached to the same PDP context is up.
 
  This function will try to bring up the default bearer. 
  
PARAMETERS 
  IN 
    wm_p             - Pointer to the watermark info.
    call_back_data   - Pointer to the bearer context.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.

===========================================================================*/
void dsUmtsBearerCntxt_HandleEnqueuedTxData
(
  dsm_watermark_type *wm_p,
  void               *call_back_data
)
{
  ds_bearer_context_s  *bearer_context_p = (ds_bearer_context_s*)call_back_data;
  int16                ps_errno;
  int                  ret_val;
  ds_bearer_context_wm_s *tx_watermark_p = NULL;
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  tx_watermark_p = &(bearer_context_p->tx_wm); 

  //Reset the enqueue pointer.
  (tx_watermark_p->wm_item).each_enqueue_func_ptr = NULL;          
  (tx_watermark_p->wm_item).each_enqueue_func_data = NULL;   
  
  ret_val = ps_phys_link_up_cmd( &(bearer_context_p->
                                   phys_link),
                                 &ps_errno,
                                 NULL );

  if ( ps_errno != DS_EWOULDBLOCK)
  {
     DS_3GPP_MSG1_HIGH("Unable to bring up physical link, ret_val: %d",
                        ret_val);  
     ds_pdn_cntxt_unbind_default_flows(bearer_context_p->
                                       ds_bearer_context_dyn_p
                                       ->pdn_context_ptr);       
     ps_phys_link_gone_ind(&(bearer_context_p->phys_link));    
  }

} /* dsUmtsBearerCntxt_HandleEnqueuedTxData() */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_GO_NULL_HDLR

DESCRIPTION 
  This function is used to cleanup the bearer. After the bearer is cleaned up,
  it calls PDN cleanup function.

PARAMETERS
  IN
    bearer_context_p   - Pointer to the bearer context we are working on
    call_info_p        - DSMGR input which also contains CM mode_info
    call_was_connected - Whether the call was connected or not

DEPENDENCIES
  None.

RETURN VALUE
 None
SIDE EFFECTS
  None.
===========================================================================*/
void dsUmtsBearerCntxt_go_null_hdlr
(
  ds_bearer_context_s        *bearer_context_p,
  ds_3gpp_down_reason_t      down_reason,
  boolean                    call_was_connected
)
{  
  ds_pdn_context_s*                    pdn_context_p;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;  
  sys_modem_as_id_e_type               subs_id = SYS_MODEM_AS_ID_NONE;
/*------------------------------------------------------------------------*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return; 
  }

  DS_3GPP_MSG3_HIGH("Bearer go_null hdlr: call id:%d Bearer Inst:%d",
           bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,\
	   (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,0);

  pdn_context_p = (ds_pdn_context_s*)bearer_context_p->ds_bearer_context_dyn_p
                    ->pdn_context_ptr;

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
  {
    return;
  }

  if(call_was_connected)
  {
    DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
    (      
      deactivateCnf,
      bearer_context_p
    );
  }
  
  bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm = FALSE;

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "Subs Id(CM)",subs_id);
    return;
  }

  /* clear the rab re-establish timer for this bearer */
  DS_BEARER_CNTXT_UNSET_BITMASK(per_subs_info_hndl, bearer_context_p);

  /* Reset the rab_reestab_retry counter for this bearer. */
  DS_BEARER_CNTXT_RESET_RAB_REESTAB_RETRY_CNT(bearer_context_p);
  
  ds_bearer_cntxt_reset_access_barring_retry();

  if(per_subs_info_hndl->uplink_data_bitmask == 0)
  {
    if(per_subs_info_hndl->timer_running == TRUE)
    {
      DS_BEARER_CNTXT_CLR_TIMER (per_subs_info_hndl);
    }
  }

  /*------------------------------------------------------------------------
   * If this was the bearer context that was used to send a reorig request,
   * and we get a call_end for this bearer, reset the 
   * service_request_pending_flag and the service_response_expected flags
   * to FALSE
   * VRK:TODO EPS bearer cntx needs this check?
  ------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp == TRUE)
  {
    per_subs_info_hndl->service_request_pending_flag = FALSE;
    bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp = FALSE;
  }

   /*------------------------------------------------------------------------ 
   If this is the default bearer and other dedicated bearers are still active,
   dont tear down the ps_iface connection. Instead set the physical link state
   to down. Also trap the Tx WM enqueue function so that we can bring up the
   link if the app decides to send data on it.
    ------------------------------------------------------------------------*/
  if ( (bearer_context_p->ds_bearer_context_dyn_p->is_default == TRUE) && 
       (pdn_context_p->ds_pdn_context_dyn_p->num_active_bearer_context > 1) )
  {
     ds_bearer_context_wm_s *tx_watermark_p = &(bearer_context_p->
                                                tx_wm);     
     DS_3GPP_MSG0_HIGH("Default down. Dedicated up");         
     ps_phys_link_down_ind_ex(&(bearer_context_p
                                ->phys_link), 
                              PS_EIC_NOT_SPECIFIED);   

     //Modify the enqueue function to bring the physical link up when data
     // is enqueued     
    (tx_watermark_p->wm_item).each_enqueue_func_ptr = 
                               (wm_cb_type)dsUmtsBearerCntxt_HandleEnqueuedTxData;
    (tx_watermark_p->wm_item).each_enqueue_func_data =
                                                    (void*) bearer_context_p;          
     return;
  }

  dsUmtsBearerCntxt_Free(
     bearer_context_p,
     pdn_context_p,
     DS_3GPP_PS_IFACE_NET_DOWN_REASON (down_reason),
     down_reason.ps_eic,
     call_was_connected
  );

  /*------------------------------------------------------------------------ 
   Check if this is the last remaining dedicated bearer for this PDN.
   If so and we did not properly clean the default bearer earlier, do it now.                                                                          . 
  ------------------------------------------------------------------------*/
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    ds_bearer_context_s *def_bearer_cntxt_p = 
             (ds_bearer_context_s *)pdn_context_p->ds_pdn_context_dyn_p
                                                    ->def_bearer_context_ptr;
    if(ds_bearer_cntx_validate_bearer_context(def_bearer_cntxt_p))

    {
      ps_phys_link_type *phys_link = &(def_bearer_cntxt_p->phys_link);
      //Check to see if default is not dormant.
      if ((PS_PHYS_LINK_GET_STATE(phys_link) == PHYS_LINK_DOWN) &&            
            (def_bearer_cntxt_p->ds_bearer_context_dyn_p->state == 
             DS_BEARER_CONTEXT_STATE_DOWN) && 
            (pdn_context_p->ds_pdn_context_dyn_p->num_active_bearer_context == 1))
       {     
         DS_3GPP_MSG0_HIGH("Freeing the default bearer");
         dsUmtsBearerCntxt_Free(
            def_bearer_cntxt_p,
            pdn_context_p,
            PS_NET_DOWN_REASON_NOT_SPECIFIED,
            PS_EIC_NOT_SPECIFIED,
            TRUE
         );
       }
    } 
  }  

} /* dsUmtsBearerCntxt_go_null_hdlr() */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_DEACTIVATECNF

DESCRIPTION
  This function is called from 3gpp_hdlr in response to a call end event.
  This function cleans up the interface Data has with lower layers and frees
  the bearer context. It might in turn invoke operations that are specific to
  W and G modes through the use of the registered function tables for each
  mode.


PARAMETERS
  IN
    bearer_context_p   - Pointer to the bearer context we are working on
    call_info_p        - DSMGR input which also contains CM mode_info
    end_type           - 0:regular,1:abort,2:reject
    call_was_connected - Whether the call was connected or not

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsUmtsBearerCntxt_deactivateCnf(
                            ds_bearer_context_s            *bearer_context_p,
                            const ds3g_call_info_cmd_type  *call_info_p, 
                            uint8                           end_type,
                            boolean                         call_was_connected
                            )
{
  ds_pdn_context_s*                  pdn_context_p = NULL;
  ds_3gpp_down_reason_t              down_reason;
  cm_call_end_e_type                 cm_call_end_cause;
  void                              *pdn_throt_sm_ptr = NULL;
  boolean                            throt_req_flag = FALSE;
  cm_gw_ps_call_info_s_type         *gw_ps_call_info = NULL;
  sys_modem_as_id_e_type             subs_id  = SYS_MODEM_AS_ID_NONE;
  void                              *sm_ptr = NULL;
#ifdef FEATURE_DATA_REL10           
  byte                               *apn = NULL;
  byte                                decoded_apn[DS_UMTS_MAX_APN_STRING_LEN];
#endif /*FEATURE_DATA_REL10*/       
  sys_sys_mode_e_type                sys_sys_mode;
  uint8                              max_active_pdp_context = 0;
  ds_network_overrridden_ip_type_e   override_ip = PDP_UNDEFINED;
  uint32                             rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
                                     /* RATs on which throttling is to be
                                         unblocked */
  ds_3gpp_pdn_throt_ident_info_type  throt_ident_info;
  ds_3gpp_throt_rat_e_type           rat_type = DS_3GPP_THROT_RAT_GLOBAL;
                                     /* RAT on which throttling is to be
                                         performed */
  boolean                            is_attach_apn = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(call_info_p == NULL)
  {
    return;
  }

  gw_ps_call_info = 
          (cm_gw_ps_call_info_s_type*)&(call_info_p->mode_info.info.gw_ps_call);

  down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
  down_reason.reason.ps_iface_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  down_reason.ps_eic = PS_EIC_NOT_SPECIFIED;
  cm_call_end_cause = (cm_call_end_e_type)(call_info_p->end_status);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }
  
  pdn_context_p = (ds_pdn_context_s*)
               (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr);
  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return;
  }

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    sys_sys_mode = bearer_context_p->ds_bearer_context_dyn_p->call_mode;

    rat_type = 
      ds_3gpp_throt_map_cm_sys_mode_to_rat_type
        (bearer_context_p->ds_bearer_context_dyn_p->call_mode);

    rat_mask |= ds_3gpp_map_cm_sys_mode_to_rat_mask(sys_sys_mode);
  }

  /*-------------------------------------------------------------------------
    Make sure the call instance can be ended
  -------------------------------------------------------------------------*/
  if (bearer_context_p->ds_bearer_context_dyn_p->state == 
      DS_BEARER_CONTEXT_STATE_DOWN ||
      bearer_context_p->ds_bearer_context_dyn_p->state == 
      DS_BEARER_CONTEXT_STATE_PEND_REDIAL) 
  {
    DS_3GPP_MSG1_ERROR("Trapped, trying to end already ended call. Bearer instance = "
                       "%d, returning...",
      (uint32)(bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr));
      return;
  }

  pdn_context_p = (ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr);
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return;
  }

  DS_3GPP_MSG2_HIGH("dsUmtsBearerCntxt_deactivateCnf: call id:%d Bearer Inst:%d",
           bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
       (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

#ifdef FEATURE_DATA_LTE
    /*-----------------------------------------------------------------
      We need to check for APN Switching action if this is last standing
      PDN 
    ------------------------------------------------------------------*/

  if((FALSE == dsPdnContext_is_any_other_normal_pdn_up(pdn_context_p, subs_id)) &&
     (ds_bearer_cntxt_is_default(bearer_context_p) == TRUE))
  {
    ds_3gpp_apn_switch_mgr_populate_switching_payload_per_rat(
       bearer_context_p->ds_bearer_context_dyn_p->call_mode,
       DS_DSD_APM_DETACH_NETWORK_CAUSE,
       (uint16)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile_num,
       pdn_context_p->ds_pdn_context_dyn_p->state,
       (cm_call_mode_info_u_type *)&(call_info_p->mode_info.info),
       subs_id);
  }
#endif /*FEATURE_DATA_LTE*/
  /*-------------------------------------------------------------------------
    Map SM Cause Code to PS EIC for CALL END EVENT
    (1) CALL END initiated by DS3G Mgr due to internal error
    (2) CALL END initiated by CM
        (a) By clients of CM
        (b) By Layers Lower to CM or Network
  -------------------------------------------------------------------------*/
  if ( call_info_p->end_type == DS_CM_CALL_END_INTERNAL )
  {
    DS_3GPP_MSG1_HIGH("CALL_ENDED_HDLR: Internal DS Cause call id %d",
             call_info_p->call_id);
 
    if( !dsUmtsBearerCntx_map_cause_codes( 
                 CALL_END_DS_CAUSE_CODES_TO_PS_EIC,
                 (void *)&down_reason.ps_eic,
                 (void *)&(call_info_p->end_status) ) )
    {
      DS_3GPP_MSG1_ERROR("CALL_ENDED_HDLR: Mapping failed for CALL_END_INTERNAL!, call id =%d",
                call_info_p->call_id);
    }

    if(call_info_p->call_cmd_err == CM_CALL_CMD_ERR_INVALID_SIM_STATE_S)
    { 
      down_reason.reason.ps_iface_down_reason = PS_NET_DOWN_REASON_INVALID_SIM_STATE;

      DS_3GPP_MSG1_HIGH("Mapping for CALL_END_INTERNAL! INVALID_SIM_STATE, call id =%d", 
                          call_info_p->call_id);
    }
    else 
    {
      if( !dsUmtsBearerCntx_map_cause_codes( 
                 CALL_END_DS_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES,
                 (void *)&down_reason.reason,
                 (void *)&(call_info_p->end_status) ) )
      {
        DS_3GPP_MSG1_ERROR("CALL_ENDED_HDLR: Mapping failed for CALL_END_INTERNAL!, call id=%d",
                  call_info_p->call_id);
      }
    }
 
  }
  else if( call_info_p->end_type == DS_CM_CALL_END_EXTERNAL )
  {
    DS_3GPP_MSG1_HIGH("CALL_ENDED_HDLR: External CM Cause, call id:%d",
             call_info_p->call_id);
 
    if ( (cm_call_end_cause == CM_CALL_END_CLIENT_END) &&
         (bearer_context_p->ds_bearer_context_dyn_p->down_reason_ps_eic != PS_EIC_NOT_SPECIFIED) )
    {
      down_reason.ps_eic = bearer_context_p->ds_bearer_context_dyn_p->down_reason_ps_eic;
      DS_3GPP_MSG1_HIGH("DS initiated call_end, ps_eic_down_reason: %d",
                                        down_reason.ps_eic);
      bearer_context_p->ds_bearer_context_dyn_p->down_reason_ps_eic = PS_EIC_NOT_SPECIFIED;
    }
    else if ( !dsUmtsBearerCntx_map_cause_codes( 
                      CALL_END_CM_CAUSE_CODES_TO_PS_EIC,
                      (void *)&(down_reason.ps_eic),
                      (void *)&(call_info_p->end_status) ) )
    {
      DS_3GPP_MSG1_ERROR("CALL_ENDED_HDLR: Mapping failed for CALL_END_EXTERNAL!, call id=%d",
                          call_info_p->call_id);
    }

    /*-----------------------------------------------------------------------
      If call was ended by the network and reactivation was requested, notify
      this to the application.
    -----------------------------------------------------------------------*/
    if ( (cm_call_end_cause == CM_CALL_END_CLIENT_END) &&
         (bearer_context_p->ds_bearer_context_dyn_p->down_reason != PS_NET_DOWN_REASON_NOT_SPECIFIED) )
    {
      down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
      down_reason.reason.ps_iface_down_reason = bearer_context_p->ds_bearer_context_dyn_p->down_reason;
      DS_3GPP_MSG1_HIGH("DS initiated call_end, ps_net_down_reason: %d",
                                        down_reason.reason.ps_iface_down_reason);
      bearer_context_p->ds_bearer_context_dyn_p->down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
    }
    else if ( !dsUmtsBearerCntx_map_cause_codes( 
                      CALL_END_CM_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES,
                      (void *)&down_reason,
                      (void *)call_info_p ) )
    {
      DS_3GPP_MSG1_ERROR("CALL_ENDED_HDLR: Mapping failed for CALL_END_EXTERNAL!, call id=%d",
                          call_info_p->call_id);
    }
    if(gw_ps_call_info->pdp_cause_type == SM_NETWORK_CAUSE)
    {
      throt_req_flag = TRUE;
      DS_3GPP_MSG0_HIGH("cause type is SM_NETWORK_CAUSE, setting" 
                        " throt_req_flag");
      DS_3GPP_MSG1_HIGH("External Cause_Code:%d",gw_ps_call_info->cause.ext_cause);
      /*---------------------------------------------------------------
       If REL 11 is enabled and network sends a cause code as IPv4 only
       or IPV6 only, save it in the network override handler
       ---------------------------------------------------------------*/
      if((gw_ps_call_info->cause.ext_cause == SM_PDP_TYPE_IPV6_ONLY_ALLOWED) &&
        (ds_3gppi_is_rel_11_supported()))
      {
        override_ip =  PDP_IPV6;
        ds_3gpp_network_override_hdlr_set_overridden_ip
          (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
           override_ip, subs_id);
      }
      else if((gw_ps_call_info->cause.ext_cause == SM_PDP_TYPE_IPV4_ONLY_ALLOWED) &&
        (ds_3gppi_is_rel_11_supported()))
      {
        override_ip =  PDP_IPV4;
        ds_3gpp_network_override_hdlr_set_overridden_ip
          (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
           override_ip, subs_id);
      }
      else if( (gw_ps_call_info->cause.ext_cause == SM_UNKNOWN_PDP_ADDRESS_OR_TYPE) ||
               (gw_ps_call_info->cause.ext_cause == SM_SERVICE_NOT_SUBSCRIBED) )
      {
        /*-------------------------------------------------------------------
          Check if apn_reject_name is configured in the EFS, Here value is
          read from the cache and not from the EFS.
        -------------------------------------------------------------------*/
        if (ds_3gpp_pdn_throt_validate_apn_reject_name(subs_id))
        {
          /*-----------------------------------------------------------------
            Check if the APN set in the EFS and APN in the PDN context
            are the same.
          -----------------------------------------------------------------*/
		  
		  
          if((subs_id > SYS_MODEM_AS_ID_NONE) &&
		     (strcasecmp((char*)ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn,
                         (char*)pdn_context_p->
                         ds_pdn_context_dyn_p->pdp_profile.context.apn) == 0))
          {
            ds_3gpp_pdn_throt_set_apn_reject_pdn_blocked(TRUE, subs_id);
          }
        }
      }
    }
    else if((gw_ps_call_info->pdp_cause_type == SM_INTERNAL_CAUSE) &&
            (gw_ps_call_info->cause.int_cause == SM_NAS_SRV_REQ_FAILURE) )
    {
      sm_ptr = ds_3gpp_throt_sm_get_throttling_sm(TRUE,subs_id);
      
      if(ds_3gpp_throt_sm_get_is_throttled_flag(sm_ptr,subs_id) == FALSE)
      {
        /*----------------------------------------------------------------------
          Now feed the failure into the system wide throttling state machine.
          Get the timer value to use from the sm
          Call this function with argument TRUE meaning we need to create a sm
          if it does not already exists. 
        ----------------------------------------------------------------------*/
        ds_3gpp_throt_sm_feed_failure(sm_ptr,subs_id,pdn_context_p);
      }
      else
      {
        DS_3GPP_MSG3_HIGH("service request throttling is already running not"
                          " re-starting again", 0, 0, 0);
      }
    }
#ifdef FEATURE_DATA_RPM
    /*-------------------------------------------------------------------------
      Call PDN throttle function to handle connection failure if bearer state
      is COMING_UP
    -------------------------------------------------------------------------*/
    if ((TRUE == ds_3gpp_is_rpm_enabled(subs_id))&&
        (bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_COMING_UP))
    {
      throt_req_flag = FALSE;
      ds_3gpp_pdn_throt_handle_conn_response(FALSE, 
                                             pdn_context_p, 
                                             down_reason.reason.ps_iface_down_reason);
    }
#endif /* FEATURE_DATA_RPM */

    if(ds_3gpp_nv_manager_is_umts_rel_10_enabled())
    {
      
      DS_3GPP_MSG0_HIGH("UMTS NV IS REL10 COMPLIANCE IS SET!!!!");
      if( (down_reason.reason.ps_iface_down_reason == 
                                 PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES) ||
          (down_reason.reason.ps_iface_down_reason == 
                                 PS_NET_DOWN_REASON_UNKNOWN_APN) )
      {
         throt_req_flag = FALSE;
         DS_3GPP_MSG3_HIGH("call_end cause code is release 10 specific, unsetting" 
                           " throt_req_flag", 0, 0, 0);

        /*---------------------------------------------------------------- 
         Perform global APN throttling for these cause codes. Create a 
         PDN throt SM exclusively for globally throttling the given APN
         across all PLMNs.
        ------------------------------------------------------------------*/
        if (gw_ps_call_info->pdp_reject_ind.apn_name.valid)
        {
          apn = gw_ps_call_info->pdp_reject_ind.apn_name.address;
	  DS_3GPP_MSG3_HIGH("Encoded  APN:%s, APN in PDN_ctxt: %s",
                 (char*)apn,
                 (char*)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
                  0);
	  if (apn == NULL)
          {
            DS_3GPP_MSG3_ERROR("APN ptr retrieved from pdn_reject_ind is NULL",
                               0,0,0);
            apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
          }
          else
          {
            memset(decoded_apn, 0, DS_UMTS_MAX_APN_STRING_LEN);
            if(ds_3gpp_pdn_context_decode_apn_name_non_strict(apn,
                      (uint32)gw_ps_call_info->pdp_reject_ind.apn_name.apn_addr_len,
                      decoded_apn) == TRUE)
            {
              apn = decoded_apn;
            }
            else
            {
              DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Unable to decode APN %s"
                                     ", using profile APN", (char*)apn);
              apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;           
            }
          }
        }
        else
        {
          DS_3GPP_MSG3_HIGH("Network Given APN is not valid using profile APN",
                            0, 0, 0);
          apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
        }
  
        DS_3GPP_MSG3_HIGH("APN being throttled: %s, APN in PDN_ctxt: %s",
                 (char*)apn,
                 (char*)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 0);
  
        ds_3gpp_pdn_throt_umts_handle_reject_with_t3396_ie(gw_ps_call_info, 
                                      apn,
                                      down_reason.reason.ps_iface_down_reason,
                                      subs_id,
                                      pdn_context_p);
      }
      else if(down_reason.reason.ps_iface_down_reason == 
                                        PS_NET_DOWN_REASON_UMTS_REATTACH_REQ)
      {
        /*---------------------------------------------------------------------
         bearer context deactivation request with cause #39 “reactivation
         got successfull processed by UE. Clear APN throttling state machine 
         if it was globally blocked i.e. unthrottle the APN
        ----------------------------------------------------------------------*/
        apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
    
        if(*apn == '\0')
        {
          (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_context_p, &apn);
        }

        ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn
          (apn, subs_id, rat_mask );
        if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)apn))
        {
          is_attach_apn = TRUE;
        }
#ifdef FEATURE_DATA_LTE
        if(is_attach_apn)
        {
          ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id);
        }
#endif /* FEATURE_DATA_LTE */
      }
    }//ds_3gpp_pdn_get_nv_lte_rel_10

  if(ds_3gppi_is_rel_11_supported())
  {
    if(down_reason.reason.ps_iface_down_reason ==
                          PS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED)
    {
      /*---------------------------------------------------------------------
       if the network returns a cause (#65) maximum number of active pdp is 
       reached cause ,update the global variable with the current 
       number of active pdp context 
      ----------------------------------------------------------------------*/

      max_active_pdp_context = ds_3gpp_pdn_get_current_active_context(subs_id);
      DS_3GPP_MSG1_HIGH("Maximum simultaneous PDP count %d",max_active_pdp_context);
      ds_3gpp_pdn_set_max_simul_active_pdp_context(max_active_pdp_context, subs_id);
    }

    if(down_reason.reason.ps_iface_down_reason == 
                           PS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN )
    {
       /*---------------------------------------------------------------------
        if the network returns a cause (#66) permanently throttle that apn 
        The apn is unblocked only on sim removal and power reboot procedures
       ----------------------------------------------------------------------*/
       pdn_throt_sm_ptr = 
         ds_3gpp_pdn_throt_sm_get_throttle_sm(
           pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
           FALSE,
           TRUE,
           DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
           subs_id);

       ds_3gpp_pdn_throt_set_failure_cause_code(pdn_throt_sm_ptr, 
                                               (void*)PS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN,
                                               DS_UMTS_PDP_IPV4V6);
       ds_3gpp_pdn_throt_handle_perm_conn_failure(pdn_throt_sm_ptr,
	   	                                  (void *)(down_reason.reason.ps_iface_down_reason),
                                                  DS_UMTS_PDP_IPV4V6, 
                                                  subs_id,
                                                  pdn_context_p);
       sys_sys_mode = ds_3gpp_map_radio_access_to_sys_mode(gw_ps_call_info->sys_rat);
       if (sys_sys_mode == SYS_SYS_MODE_NONE) 
       {
         DS_3GPP_MSG1_ERROR("invalid sys_rat: %d", gw_ps_call_info->sys_rat);
       }
       else
       {
         ds_3gppi_throt_sm_set_current_sys_mode(pdn_throt_sm_ptr, 
                                                DS_UMTS_PDP_IPV4V6,
                                                sys_sys_mode);
       }
     }
   } /* rel11 feature */


  /*------------------------------------------------------------------------
    For cause codes PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES and
    PS_NET_DOWN_REASON_UNKNOWN_APN if release 10 throttling is enabled do rel10 
    throttling other wise do regular throttling. Here global_throt_req_flag is 
    to ensure that whrether release 10 throttling is done or not. if rel10 is 
    done for above cause code skip global throttling 
  --------------------------------------------------------------------------*/    
    if( (throt_req_flag == TRUE) &&
        (ds_3gpp_nv_manager_get_global_throttling(subs_id)) &&
        (bearer_context_p->ds_bearer_context_dyn_p->state == 
                               DS_BEARER_CONTEXT_STATE_COMING_UP) )
    {      
      ds_3gpp_pdn_throt_fill_ident_info
      (
        &throt_ident_info, 
        pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
        DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
        subs_id,
        FALSE,
        rat_type
      );

      pdn_throt_sm_ptr = 
        ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, TRUE);
      
      if(pdn_throt_sm_ptr == NULL)
      {
        DS_3GPP_MSG0_ERROR("failed to create PDN throttle SM ");
        return;
      }
      throt_req_flag = FALSE;
      ds_3gpp_bearer_cntxt_global_throttling
                        (
                          pdn_context_p,
                          pdn_throt_sm_ptr,
                          bearer_context_p->ds_bearer_context_dyn_p->call_mode,
                          down_reason.reason.ps_iface_down_reason
                        ); 
    }
    else if((throt_req_flag == TRUE)&&
	   (ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE )&&
	   (ds_3gpp_cc_sp_throt_is_cause_code_configured(
	       (down_reason.reason.ps_iface_down_reason)&(0x0000FFFF),subs_id))&&
	   (bearer_context_p->ds_bearer_context_dyn_p->state == 
                               DS_BEARER_CONTEXT_STATE_COMING_UP))
    {
      pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm(
                          pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
                         FALSE,
                         TRUE,
                         DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, subs_id);
         
      if(pdn_throt_sm_ptr == NULL)
      {
        DS_3GPP_MSG0_ERROR("failed to create PDN throttle SM ");
        return;
      }      
      DS_3GPP_MSG0_HIGH("in deactivatecnf calling ds_bearer_cntxt_cc_sp_throttling");
      ds_bearer_cntxt_cc_sp_throttling(pdn_context_p,
                            pdn_throt_sm_ptr,
                            bearer_context_p->ds_bearer_context_dyn_p->call_mode,
                            down_reason.reason.ps_iface_down_reason);
    }
  
  }
  else
  {
    DS_3GPP_MSG1_ERROR("CALL_ENDED_HDLR: Unknown PDP Cause Type recvd from CM/SM!, call id=%d",
              call_info_p->call_id);
  } 
  
  if (bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {
    bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_PEND_REDIAL;

    DS_3GPP_MSG0_HIGH("Setting the cm call id to be invalid as we are moving to pending "
                      "redial state");
    bearer_context_p->ds_bearer_context_dyn_p->cm_call_id = CM_CALL_ID_INVALID;
    DS_3GPP_MSG2_HIGH("dsUmtsBearerCntxt_deactivateCnf: Bearer set to PEND_REDIAL state"
                      "call id:%d Bearer Inst:%d",  bearer_context_p->
                      ds_bearer_context_dyn_p->cm_call_id,
                      (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
  
    if (bearer_context_p->ds_bearer_context_dyn_p->
        pdn_context_cb_tbl_ptr->bearer_down_ind_cb != NULL)
    {
      bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->bearer_down_ind_cb(\
                     bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,
                     bearer_context_p, 
                                 down_reason,
                                 call_was_connected);
    }
  }
  else
  {
    bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_DOWN;
    dsUmtsBearerCntxt_go_null_hdlr(bearer_context_p,
                                   down_reason, call_was_connected);

  }

  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);

  #ifdef FEATURE_DATA_LTE
  if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) != TRUE))
    {
     ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable(subs_id);
    }
  #endif
} /* dsUmtsBearerCntxt_deactivateCnf() */


/*==========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONV_DATA_RATE_NETW_TO_UMTS

DESCRIPTION
  Converts Netw encoded data rate to UMTS data rate 

PARAMETERS
  (->) byte netw_encoded_data_rate;

DEPENDENCIES
  None.

RETURN VALUE
  uint32 decoded umts_data_rate

LIMITATIONS
  Internal DS routine. Can decode max data rate of 8640 kbps
===========================================================================*/
uint32 dsUmtsBearerCntxt_conv_data_rate_netw_to_umts
(
    byte netw_encoded_data_rate
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        if ( ( netw_encoded_data_rate > 0 ) && ( netw_encoded_data_rate <= 0x3F ) ) 
        {
            return ( netw_encoded_data_rate );
        }
        else if ( ( netw_encoded_data_rate > 0x3F ) && ( netw_encoded_data_rate <= 0x7F ) )
        {
            return ( 64 + netw_encoded_data_rate );
        }
        else if ( ( netw_encoded_data_rate > 0x7F ) && ( netw_encoded_data_rate <= 0xFE ) ) 
        {
            return ( 576 + netw_encoded_data_rate );
        }
        return 0;
} /* dsUmtsBearerCntxt_conv_data_rate_netw_to_umts() */

/*==========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONV_QOS_NETW_TO_UMTS

DESCRIPTION
  Given a set of network allocated QoS, converts them into UMTS
  specific QoS. 

PARAMETERS
  (->) *netw_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *umts_qos_ptr    - Ptr to App QOS in Call table

DEPENDENCIES
  None.

RETURN VALUE
  TRUE on SUCCESS
  FALSE on FAILURE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsBearerCntxt_conv_qos_netw_to_umts
(
  const qos_T                  *netw_qos_ptr,
  ds_umts_umts_qos_params_type *umts_qos_ptr
)
{   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    byte tx_delay_msb;
    byte tx_delay_val;

    if( (umts_qos_ptr == NULL) || (netw_qos_ptr == NULL))
    {
      DS_3GPP_MSG0_ERROR("CONV_NETW_QOS_TO_UMTS_QOS: NULL QoS Ptr");
      return DS3G_FAILURE;
    }


    if ( netw_qos_ptr->valid == FALSE)
    {
      DS_3GPP_MSG0_ERROR("CONV_NETW_QOS_TO_UMTS_QOS: Invalid Netw QoS Ptr");
      return DS3G_FAILURE;
    }

    DS_3GPP_MSG0_HIGH("CONV_NETW_QOS_TO_UMTS_QOS: Starting QoS conversion routine");

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Unset the valid flag
     ----------------------------------------------------------------------*/
    umts_qos_ptr->valid_flg = FALSE;

    /*-----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Traffic Class, same for UL and DL
    -----------------------------------------------------------------------*/
    umts_qos_ptr->traffic_class = (ds_umts_qos_tclass_enum_type)netw_qos_ptr->traffic_class;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Max and Guaranteed Data Rates (kbps), diff for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->max_ul_bitrate = 
        dsUmtsBearerCntxt_conv_data_rate_netw_to_umts( netw_qos_ptr->max_bit_rare_uplink );
    umts_qos_ptr->max_dl_bitrate = 
        dsUmtsBearerCntxt_conv_data_rate_netw_to_umts( netw_qos_ptr->max_bit_rare_downlink );

    umts_qos_ptr->gtd_ul_bitrate = 
        dsUmtsBearerCntxt_conv_data_rate_netw_to_umts( netw_qos_ptr->guarantee_ber_uplink );
    umts_qos_ptr->gtd_dl_bitrate = 
        dsUmtsBearerCntxt_conv_data_rate_netw_to_umts( netw_qos_ptr->guarantee_ber_dwnlink );

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Residual Bit Error Rate, same for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->dlvry_order = (ds_umts_qos_dorder_enum_type)netw_qos_ptr->deliver_order;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Max SDU Size, same for UL and DL
     ----------------------------------------------------------------------*/
    if ( netw_qos_ptr->max_sdu_size <= QOS_MAX_SDU_1500_BYTES ) 
    {
     umts_qos_ptr->max_sdu_size = netw_qos_ptr->max_sdu_size * 10;
    }
    else
    {
        switch ( netw_qos_ptr->max_sdu_size ) 
        {
        case QOS_MAX_SDU_1502_BYTES:
            umts_qos_ptr->max_sdu_size = 1502;
            break;
        case QOS_MAX_SDU_1510_BYTES:
            umts_qos_ptr->max_sdu_size = 1510;
            break;
        case QOS_MAX_SDU_1520_BYTES:
            umts_qos_ptr->max_sdu_size = 1520;
            break;
        default:
            DS_3GPP_MSG1_HIGH("CONV_NETW_QOS_TO_UMTS_QOS: Max SDU Size is out of range:%d, \
                     setting to RESERVED",netw_qos_ptr->max_sdu_size);
            umts_qos_ptr->max_sdu_size = QOS_MAX_SDU_RESERVED;
            break;
        }
    }

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Packet Error Rate, same for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->sdu_err = (ds_umts_qos_sdu_error_enum_type)netw_qos_ptr->sdu_err_ratio;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Residual Bit Error Rate, same for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->res_biterr = (ds_umts_qos_res_ber_enum_type)netw_qos_ptr->residual_ber;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Delivery of err SDU, same for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->dlvr_err_sdu = (ds_umts_qos_sdu_dlvr_enum_type)netw_qos_ptr->deliver_err_sdu;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store transfer delay, same for UL and DL 
     ----------------------------------------------------------------------*/
    umts_qos_ptr->trans_delay = 0;

    /*----------------------------------------------------------------------
      Table 10.5.156/3GPP TS 24.008: Quality of service information element
        
      0 0 0 0 0 1 The Transfer delay is binary coded in 6 bits, using a 
                  granularity of 10 ms giving a range of values from 10 ms 
      0 0 1 1 1 1 to 150 ms in 10 ms increments.

      0 1 0 0 0 0 The transfer delay is 200 ms + 
                  ((the binary coded value in 6 bits - 010000) * 50 ms)
      0 1 1 1 1 1 giving a range of values from 200 ms to 950 ms in 50ms 
                  increments.

      1 0 0 0 0 0 The transfer delay is 1000 ms + 
                  ((the binary coded value in 6 bits - 100000) * 100 ms)
      1 1 1 1 1 0 giving a range of values from 1000 ms to 4000 ms in 100ms 
                  increments.
      1 1 1 1 1 1 Reserved.

      The Transfer delay value is ignored if the Traffic Class is 
      Interactive class or Background class.
    ----------------------------------------------------------------------*/
    if ( ( umts_qos_ptr->traffic_class != DS_UMTS_QOS_TCLASS_INTR ) &&
         ( umts_qos_ptr->traffic_class != DS_UMTS_QOS_TCLASS_BACK )
        ) 
    {
      /* Extract 2 MSBs */
      tx_delay_msb = (netw_qos_ptr->tranfer_delay >> 4) & 0x3; 
      tx_delay_val = (netw_qos_ptr->tranfer_delay & 0xF);

      if (tx_delay_msb == 0) 
      {
        umts_qos_ptr->trans_delay = tx_delay_val * 10;
      }
      else if (tx_delay_msb == 1 )
      {
        umts_qos_ptr->trans_delay = 200 + tx_delay_val * 50;
      }
      else if (tx_delay_msb > 1) 
      {
        tx_delay_val = (netw_qos_ptr->tranfer_delay & 0x1F);
        umts_qos_ptr->trans_delay = 1000 + tx_delay_val * 100;
      }
    }
        
    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Traffic Handling Priority, same for UL and DL 
     ----------------------------------------------------------------------*/
    umts_qos_ptr->thandle_prio = netw_qos_ptr->traffic_priority;

    if(umts_qos_ptr->traffic_class == DS_UMTS_QOS_TCLASS_INTR)
    {
      /*----------------------------------------------------------------------
        SIG_IND: Convert & Store Signalling Indication Bit,same for UL and DL 
      ----------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("converting the signalling indication parameter: sig_ind - %d", 
             netw_qos_ptr->sig_ind);
      umts_qos_ptr->sig_ind = netw_qos_ptr->sig_ind;

    }

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Set the valid flag
     ----------------------------------------------------------------------*/
    umts_qos_ptr->valid_flg = TRUE;

    return TRUE;
}/*dsUmtsBearerCntxt_conv_qos_netw_to_umts*/

/*==========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONV_QOS_NETW_TO_GPRS

DESCRIPTION
  Given a set of network encoded QoS, decode GPRS QoS paramters

PARAMETERS
  (->) *netw_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *gprs_qos_ptr    - Ptr to GPRS QoS Blob in DS Call table

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
 -1 on FAILURE

SIDE EFFECTS


===========================================================================*/

boolean dsUmtsBearerCntxt_conv_qos_netw_to_gprs
(
  const qos_T                  *netw_qos_ptr,
  ds_umts_gprs_qos_params_type *gprs_qos_ptr
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    if ( !DSUMTSBEARERCNTXT_IS_CM_PROV_NETW_QOS_VALID( netw_qos_ptr ) )
    {
      DS_3GPP_MSG0_ERROR("CONV_NETW_QOS_TO_GPRS_QOS: Invalid Netw QoS Ptr");
      return DS3G_FAILURE;
    }

    if ( gprs_qos_ptr == NULL )
    {
      DS_3GPP_MSG0_ERROR("CONV_NETW_QOS_TO_GPRS_QOS: Invalid UMTS QoS Blob Ptr");
      return DS3G_FAILURE;
    }

    DS_3GPP_MSG0_HIGH("CONV_NETW_QOS_TO_GPRS_QOS: Starting QoS conversion routine");

    /*----------------------------------------------------------------------
      Unset the valid flag
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->valid_flg = FALSE;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Precedence Class, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->precedence = netw_qos_ptr->precedence_class;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Delay Class, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->delay = netw_qos_ptr->delay_class;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Reliability Class, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->reliability = netw_qos_ptr->reliability_class;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert peak thruput, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->peak = netw_qos_ptr->peak_throughput;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert mean thruput, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->mean = netw_qos_ptr->mean_throughput;

    /*----------------------------------------------------------------------
      Set the valid flag
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->valid_flg = TRUE;

    return DS3G_SUCCESS;
}/* dsUmtsBearerCntxt_conv_qos_netw_to_gprs() */

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_validatebearer

DESCRIPTION
  This function checks if the umts bearer context has been allocated dynamic memory

PARAMETERS
  umts_bearer_context_p  -umts_bearer_context_p to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/


boolean dsUmtsBearerCntxt_validatebearer
(
  ds_umts_bearer_context_s *umts_bearer_context_p
)
{
  boolean ret_val = FALSE;
  ds_umts_bearer_context_dyn_s *umts_bearer_context_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(umts_bearer_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter passed is INVALID");
    return ret_val;
  }

  umts_bearer_context_dyn_p = umts_bearer_context_p->ds_umts_bearer_context_dyn_p;

  if ((umts_bearer_context_dyn_p != NULL) && 
       umts_bearer_context_dyn_p->cookie == UMTS_BEARER_CNTXT_VALID_COOKIE)

  {
    DS_3GPP_MSG0_MED("Umts Bearer context has dyn memory allocated");
    ret_val = TRUE;
  }

  return ret_val;
}


/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_ACTIVATECNF

DESCRIPTION
  This function is called from 3gpp_hdlr in response to a call connect event.
  This function sets up the interface Data has with lower layers. It also
  gets the QOS values granted by the network and store them in the bearer
  context. It might in turn invoke operations that are specific to W and G
  modes through the use of the registered function tables for each mode.


PARAMETERS
  IN
    bearer_context_p   - Pointer to the bearer context we are working on
    mode_info_p        - Pointer to the CM call info structure
    end_params_p       - Pointer to the end params structure, incase something
                         goes unsuccessful.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - DS3G_SUCCESS on successful lower layer set up.
            DS3G_FAILURE on any failure.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL boolean dsUmtsBearerCntxt_activateCnf
(
  ds_bearer_context_s            *bearer_context_p,
  const cm_call_mode_info_s_type *mode_info_p,
  cm_end_params_s_type           *end_parms_p
)
{
  ds_umts_umts_qos_params_type* umts_qos_p;
  ds_umts_gprs_qos_params_type* gprs_qos_p;
  const qos_T*                  netw_qos_p;
  ds_pdn_context_s*             pdn_context_p;
  boolean                       result = FALSE;
  sys_sys_mode_e_type           network_mode;
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
  qos_spec_type                 *app_qos_p = NULL;
  cm_gw_ps_call_info_s_type     *gw_ps_call_info  = NULL;
  ds_3gpp_internal_cause_code_enum_type icause_code;
#ifdef FEATURE_DATA_REL10
  byte                          *apn = NULL;
  byte                           decoded_apn[DS_UMTS_MAX_APN_STRING_LEN];
#endif /*FEATURE_DATA_REL10*/
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((bearer_context_p  == NULL) || 
     (mode_info_p == NULL) ||
     (end_parms_p == NULL) ||
     (ds_bearer_cntx_validate_bearer_context(bearer_context_p) == FALSE))
  {
    ASSERT(0);
    return DS3G_FAILURE;
  }

  icause_code = DS_3GPP_INTERNAL_CC_INVALID;
  /*--------------------------------------------------------------------------
    Get the PDN context pointer from the bearer_context
  --------------------------------------------------------------------------*/
  pdn_context_p = (ds_pdn_context_s *)(bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr);
  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return DS3G_FAILURE;
  }

  gw_ps_call_info =  (cm_gw_ps_call_info_s_type*)
                     &(mode_info_p->info.gw_ps_call);
  /*------------------------------------------------------------------------- 
    Save the NSAPI ID returned for the call 
  -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->nsapi = gw_ps_call_info->nsapi.nsapi;

  /*------------------------------------------------------------------------- 
    Store the bearer in the Phys Link data structure
  -------------------------------------------------------------------------*/
  PS_PHYS_LINK_SET_BEARER_ID(&(bearer_context_p->phys_link),
                             bearer_context_p->ds_bearer_context_dyn_p->nsapi);

  /*------------------------------------------------------------------------- 
    Move the bearer context to teh UP State 
  -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_UP;

#ifdef FEATURE_DATA_IMS
  /*-------------------------------------------------------------------------
   * Decode PCO for IMS - IM_CN subsystem flag
  -------------------------------------------------------------------------*/
  if(!dsUmtsBearerCntxt_decodeProtocolParams(bearer_context_p,mode_info_p))
  {
    bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_GOING_DOWN;
    return DS3G_FAILURE;
  }
#endif //#ifdef FEATURE_DATA_IMS

  /*-------------------------------------------------------------------------
    Get the QOS parameters that the network provided from CM
  -------------------------------------------------------------------------*/
   

   if(!dsUmtsBearerCntxt_validatebearer(&umts_bearer_context_tbl
     [(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr]))
   {
    DS_3GPP_MSG0_ERROR("Umts Bearer context passed is Invalid");
    return DS3G_FAILURE;
   }

  netw_qos_p = &(gw_ps_call_info->qos);
   

  umts_qos_p = 
    &(umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr].\
		    ds_umts_bearer_context_dyn_p->umts_qos_val);
  gprs_qos_p = 
    &(umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr].\
		    ds_umts_bearer_context_dyn_p->gprs_qos_val);

  /*------------------------------------------------------------------------- 
    Associate the mode specific blob that is going to be populated to the 
    bearer context pointer.
  -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ] =\
    (void*)&umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr];

  /*------------------------------------------------------------------------- 
    Convert the network given QOS params to UMTS parameters and store them in
    the bearer context. Also convert the network QOS parameters to GPRS parms
    and store them in the bearer context. If there is a failure in the conv,
    return FAILURE to the calling function (3gpp_hdlr) so that a call tear
    down is initiated.
  -------------------------------------------------------------------------*/

  if ( !dsUmtsBearerCntxt_conv_qos_netw_to_umts( netw_qos_p,umts_qos_p ) )
  {
    umts_qos_p->valid_flg = FALSE;
    DS_3GPP_MSG1_ERROR("Unable to convert Netw to UMTS QoS, Bearer Inst:%d",
              (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
    bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_GOING_DOWN;
    return DS3G_FAILURE;
  }
  else
  {  
    if( umts_qos_p->traffic_class == DS_UMTS_QOS_TCLASS_CONV ||
        umts_qos_p->traffic_class == DS_UMTS_QOS_TCLASS_STRM )
    {
      /*----------------------------------------------------------------------
        SRC STAT DESC: Convert & Source statiscs descriptor,same for UL and DL 
      ----------------------------------------------------------------------*/
      DS_3GPP_MSG3_HIGH("converting the source statistics parameter: src_stat_desc - %d", 
               netw_qos_p->src_stat_desc, 0, 0);
      umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->
        client_data_ptr].ds_umts_bearer_context_dyn_p->src_stat_desc = 
           netw_qos_p->src_stat_desc;
    }
  }

  if ( !dsUmtsBearerCntxt_conv_qos_netw_to_gprs( netw_qos_p,gprs_qos_p ) )
  {
    gprs_qos_p->valid_flg = FALSE;
    DS_3GPP_MSG1_ERROR("Unable to convert Netw to GPRS QoS, Bearer Inst:%d",
              (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
    bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_GOING_DOWN;
    return DS3G_FAILURE;
  }

  DS_3GPP_MSG1_HIGH("Netw QoS converted to UMTS & GPRS, Bearer Context Inst:%d",
           (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

  if (gw_ps_call_info->qos.valid)
  {
    /*-----------------------------------------------------------------------
     Allocate Heap Memory
    -----------------------------------------------------------------------*/
    app_qos_p = (qos_spec_type*)modem_mem_alloc(
                                       sizeof(qos_spec_type),
                                       MODEM_MEM_CLIENT_DATA);
    if(app_qos_p != NULL)        
    {
      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      /*-------------------------------------------------------------------------
        Get the app specified QOS parameters saved in bearer_flow_manager      
      -------------------------------------------------------------------------*/    
      bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
      if(bflow_manager_p == NULL)
      {
        DS_3GPP_MSG0_ERROR("No Bearer Flow Manager associated with the Bearer");      
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        modem_mem_free(app_qos_p, MODEM_MEM_CLIENT_DATA);
        return DS3G_FAILURE;
      }
    
      ds_umts_bearer_flow_manager_convert_nw_data_rate_to_app( bflow_manager_p,
                                                             (void *)mode_info_p,
                                                                    app_qos_p,
                                                               &icause_code) ;
    
  
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      modem_mem_free(app_qos_p, MODEM_MEM_CLIENT_DATA);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Could not allocate memory to convert qos");      
    }
  }

  if(!bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {    
    /*-------------------------------------------------------------------------  
      For Dedicated Bearer bind the phys_link to Flow. This is called whenever 
      the bearer is newly setup, and can happen for NW-initiated and 
      UE-initiated.     
    -------------------------------------------------------------------------*/

    result = ds_bearer_flow_manager_bind_flow(bearer_context_p);
    if(result == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Bind Flow failed for bearer");
    }
  }

  /*---------------------------------------------------------------------------
    Setup WCDMA/GPRS specific stuff. Get the network mode first. Then invoke
    the registered function from the function table. If the network mode is not
    W or G, then we return failure since we should not have gotten this far in
    the first place.
  ---------------------------------------------------------------------------*/
  network_mode = ds_3gpp_get_apn_current_mode(pdn_context_p);

  if(network_mode != SYS_SYS_MODE_WCDMA &&
     network_mode != SYS_SYS_MODE_GSM
#ifdef FEATURE_TDSCDMA
     &&
     network_mode != SYS_SYS_MODE_TDS
#endif
    )
  {
    DS_3GPP_MSG1_HIGH("Invalid mode. Ignoring call connected. mode = %d", 
             network_mode);
    bearer_context_p->ds_bearer_context_dyn_p->state = 
      DS_BEARER_CONTEXT_STATE_GOING_DOWN;
    return DS3G_FAILURE;
  }


  DS_3GPP_MSG1_HIGH("Bearer Inst :%d Setting up mode related stuff",
                    (uint32)bearer_context_p->ds_bearer_context_dyn_p->
                    client_data_ptr);

  if (gw_ps_call_info->apn_name.valid)
  {
    apn = gw_ps_call_info->apn_name.address;
    memset(decoded_apn, 0, DS_UMTS_MAX_APN_STRING_LEN);

    if((apn != NULL) &&
       ( gw_ps_call_info->apn_name.apn_addr_len <= DS_UMTS_MAX_APN_STRING_LEN )&&
        (ds3g_decode_apn_name(apn,
            (uint32)gw_ps_call_info->apn_name.apn_addr_len,
            decoded_apn) == TRUE))
    {
      bearer_context_p->ds_bearer_context_dyn_p->apn = decoded_apn;
    }
    else
    {
      DS_3GPP_MSG0_ERROR("unable to decode the apn");
      bearer_context_p->ds_bearer_context_dyn_p->apn = 
                          pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
    }
    bearer_context_p->ds_bearer_context_dyn_p->apn = 
                                  gw_ps_call_info->apn_name.address;
  }
  else
  {
    bearer_context_p->ds_bearer_context_dyn_p->apn =
       pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
  }

  DS_UMTS_BEARER_CTXT_VFR_CALL_PER_BEARER
    (      
      result,
      activateCnf,
      bearer_context_p,
      mode_info_p,
      end_parms_p
    );

  return result;
} /* dsUmtsBearerCntxt_activateCnf() */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_ACTIVATION

DESCRIPTION
  This function is responsible for Posting Bearer Up Indication once
  registration with lower layers is incomplete

PARAMETERS
  bearer_context_p: Bearer context which has to be activated

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS: If Successful
  DS3G_FAILURE: If Activation fails

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsBearerCntxt_activation
(
  ds_bearer_context_s               *bearer_context_p
)
{
  ds_pdn_context_s                  *pdn_context_p = NULL;
  sys_modem_as_id_e_type             subs_id = SYS_MODEM_AS_ID_NONE;
  uint32                             rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
  boolean                          is_attach_apn = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return DS3G_FAILURE;
  }

  pdn_context_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return DS3G_FAILURE;
  }

  /*--------------------------------------------------------------------------
   * Send out a phys link up indication only if the lower layers were set up
   * If lower layers are not set up, activateCnf_f returns success but changes
   * the state of the bearer to DORMANT. This should be used to evaluate if
   * we are to send the indication about phys link to PS
  --------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP)
  {
    /*----------------------------------------------------------------------
    Send Physical link UP indicaton if the bearer was moved to UP state as
    a part of the lower layer set up. 
        However if the phys_link is already UP, enable the flow. This could
        be a case of pco_redial or any redial
      ---------------------------------------------------------------------*/   
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    if(PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link)) != PHYS_LINK_UP)
    {
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);      
    ps_phys_link_up_ind(&(bearer_context_p->phys_link));
    }
    else
    {
       PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       ps_phys_link_enable_flow(&(bearer_context_p->phys_link),DS_FLOW_UMTS_MASK);
    }

    /*---------------------------------------------------------------------
      Send PS phys link disable for the bearer if subs is coex flow
      disabled
    ---------------------------------------------------------------------*/
    if( !ds3gflowmgr_get_coex_data_flow_status(subs_id) )
    {
      ps_phys_link_disable_flow(&(bearer_context_p->phys_link),
                                DS_FLOW_COEX_MASK);
    }
  }

  /*--------------------------------------------------------------------------
    Call the bearer up indication that lets PDN context know that the call is
    up.
  --------------------------------------------------------------------------*/
  if (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
      bearer_up_ind_cb != NULL) 
  {
    bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
      bearer_up_ind_cb(bearer_context_p->ds_bearer_context_dyn_p->
                       pdn_context_ptr, bearer_context_p,NULL);
  }    

  if(ds_3gpp_nv_manager_is_umts_rel_10_enabled())
  {
    /*---------------------------------------------------------------------
     PDP context activation got successfully processed by UE.
     Clear APN throttling state machine if it was globally blocked.
    ----------------------------------------------------------------------*/

    if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
    {
      rat_mask |= ds_3gpp_map_cm_sys_mode_to_rat_mask
                   (bearer_context_p->ds_bearer_context_dyn_p->call_mode);
    }

    ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn
      (bearer_context_p->ds_bearer_context_dyn_p->apn, 
       subs_id, rat_mask);

    if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id,
			                     (char*)bearer_context_p->ds_bearer_context_dyn_p->apn))
    {
          is_attach_apn = TRUE;
    }

#ifdef FEATURE_DATA_LTE
    if(is_attach_apn)
    {
      ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id);
    }
#endif /* FEATURE_DATA_LTE */
  }//ds_3gpp_pdn_get_nv_lte_rel_10



  /*-------------------------------------------------------------------------
    Call PDN throttle function to handle connection success
  -------------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_handle_conn_response(TRUE, 
                                         pdn_context_p, 
                                         PS_NET_DOWN_REASON_NOT_SPECIFIED);
  return DS3G_SUCCESS;

}

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONVERT_DATA_RATE_TO_R5_EXT_ENCODE_VAL

DESCRIPTION
  This function converts the given REL5 data rate from an unsigned integer
  to a one byte value as specified in TS24.008 (10.5.6.5)

  NOTE: The return value is a 'byte' so that it can be used as-is to
  encode the QOS parameters in the message.

PARAMETERS
  data_rate : data rate in kbps

DEPENDENCIES
  None.

RETURN VALUE
  one byte value indicating the data rate as per encoding rules

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val
(
  uint32  data_rate
)
{
  uint8 encoded_data_rate=0x0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (data_rate > 8640 && data_rate < 16001)
  {
    /*-----------------------------------------------------------------------
      Data rate = 8600kbps + ( ( 8 bit binary encoded value) * 100kbps)
      giving a range of 8700kbps -16000kbps in 100kbps increments
    -----------------------------------------------------------------------*/
    encoded_data_rate = (uint8)((data_rate-8600)/100);
  }
  else
  {
    encoded_data_rate = QOS_BIT_RATE_SUBSCRIBED;
  }
  DS_3GPP_MSG2_HIGH("Extended Data rate: %d : encoded :%lx",data_rate,encoded_data_rate);
  return encoded_data_rate;
} /*dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val*/

#ifdef FEATURE_NAS_REL7
/*===========================================================================
FUNCTION DSUMTSPS_CONVERT_DATA_RATE_TO_R7_EXT_ENCODE_VAL

DESCRIPTION
  This function converts the given REL7 data rate from an unsigned integer
  to a one byte value as specified in TS24.008 (10.5.6.5)

  NOTE: The return value is a 'byte' so that it can be used as-is to
  encode the QOS parameters in the message.

PARAMETERS
  data_rate : data rate in kbps

DEPENDENCIES
  None.

RETURN VALUE
  one byte value indicating the data rate as per encoding rules

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val
(
  uint32  data_rate
)
{
  uint8 encoded_data_rate=0x0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (data_rate > 8640 && data_rate < 16001)
  {
    /*-----------------------------------------------------------------------
      Data rate = 8600kbps + ( ( 8 bit binary encoded value) * 100kbps)
      giving a range of 8700kbps -16000kbps in 100kbps increments
    -----------------------------------------------------------------------*/
    encoded_data_rate = (uint8)((data_rate-8600)/100);
  } else if (data_rate > 16000 && data_rate < 128001) {
    /*-----------------------------------------------------------------------
      Data rate = 16 Mbps + ((the binary coded value in 8 bits - 01001010) * 1 Mbps)
      giving a range of values from 17 Mbps to 128 Mbps in 1 Mbps increments
    -----------------------------------------------------------------------*/
    encoded_data_rate = (uint8)(((data_rate-16000)/1000)+0x4A);
  } else if (data_rate > 12800 && data_rate < 256001) {
    /*-----------------------------------------------------------------------
      Data rate = 128 Mbps + ((the binary coded value in 8 bits - 10111010) * 2 Mbps)
      giving a range of values from 130 Mbps to 256 Mbps in 2 Mbps increments
    -----------------------------------------------------------------------*/
    encoded_data_rate = (uint8) (((data_rate-128000)/2000)+0xBA);
  }
  else
  {
    encoded_data_rate = QOS_BIT_RATE_SUBSCRIBED;
  }
  DS_3GPP_MSG2_HIGH("Extended Data rate: %d : encoded :%lx",data_rate,encoded_data_rate);
  return encoded_data_rate;
} /* dsumtsps_convert_data_rate_to_r7_ext_encode_val */
#endif /* FEATURE_NAS_REL7 */

/*===========================================================================
FUNCTION DSUMTS_BEARERCNTXT_CONVERT_DATA_RATE_TO_R99_ENCODE_VAL

DESCRIPTION
  This function converts the given data rate from an unsigned integer
  to a one byte value as specified in TS24.008 (10.5.6.5)

  NOTE: The return value is a 'byte' so that it can be used as-is to
  encode the QOS parameters in the message.

PARAMETERS
  data_rate : data rate in kbps

DEPENDENCIES
  None.

RETURN VALUE
  one byte value indicating the data rate as per encoding rules

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val
(
  uint32  data_rate
)
{
  uint8 encoded_data_rate=0x0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_rate < 64 )
  {
    encoded_data_rate = (uint8) data_rate;
  }
  else if(data_rate < 576 )
  {
    /*-----------------------------------------------------------------------
      Data rate = 64kbps + ( ( 8 bit binary encoded value - 0x40) * 8kbps)
      giving a range of 64kbps - 568kbps in 8k increments
    -----------------------------------------------------------------------*/
    encoded_data_rate = (uint8)(( (data_rate - 64) / 8 ) + 0x40);
  }
  else if (data_rate < 8641 )
  {
    /*-----------------------------------------------------------------------
      Data rate = 576kbps + ( ( 8 bit binary encoded value - 0x80) * 64kbps)
      giving a range of 576kbps - 8640kbps in 64k increments
    -----------------------------------------------------------------------*/
    encoded_data_rate = (uint8)(( (data_rate - 576) / 64 ) + 0x80);
  }
  else
  {
    encoded_data_rate = QOS_BIT_RATE_SUBSCRIBED;
  }

  DS_3GPP_MSG2_HIGH("Data rate: %d : encoded :%lx",data_rate,encoded_data_rate);
  return encoded_data_rate;

} /* dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_FILL_ORIGI_R99_QOS_PARAMS

DESCRIPTION
  This function fills up QOS parameters in the
  origination parameters based on the PDP context specified. Only Release
  99 specific QOS parameters are filled in ( octet 6 to 13) based on the
  AT+CGEQREQ parameters


PARAMETERS
  pdp_profile_parms - Ptr to PDP profile parameters for the current call
  *orig_params_ptr  - Ptr to Call origination parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntxt_fill_orig_r99_qos_params
(
    ds_umts_umts_qos_params_type  *req_qos,
    cm_gw_ps_orig_params_s_type   *orig_params_ptr
)
{
  uint32 max_bitrate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    ATCOP would allow the parameters to be a valid value or to be left
    as subscribed (value : 0x0 ).
    Set R99 parameters ( octet 6 to octet 13 )
  ------------------------------------------------------------------------*/
  orig_params_ptr->qos.deliver_err_sdu  = req_qos->dlvr_err_sdu;
  orig_params_ptr->qos.deliver_order = req_qos->dlvry_order;

  /*------------------------------------------------------------------------
    Traffic class : fill as-is : matches coding rules
  ------------------------------------------------------------------------*/
  orig_params_ptr->qos.traffic_class = req_qos->traffic_class;

  /*------------------------------------------------------------------------
    Max SDU Size : < 1500 :encoded as 8 bits having a granularity of 10bytes
                  = 1502 : 0x97,
                  = 1510 : 0x98
                  = 1520 : 0x99
  ------------------------------------------------------------------------*/
  if(req_qos->max_sdu_size <= 1500 )
  {
    orig_params_ptr->qos.max_sdu_size = (uint8)(req_qos->max_sdu_size/10);
  }
  else
  {
    switch (req_qos->max_sdu_size)
    {
      case 1502:
        orig_params_ptr->qos.max_sdu_size = QOS_MAX_SDU_1502_BYTES;
        break;

      case 1510:
        orig_params_ptr->qos.max_sdu_size = QOS_MAX_SDU_1510_BYTES;
        break;

      case 1520:
        orig_params_ptr->qos.max_sdu_size = QOS_MAX_SDU_1520_BYTES;
        break;

      default:
        DS_3GPP_MSG3_HIGH("Max SDU is out of range:%d setting to subscribed",
                  req_qos->max_sdu_size,0,0);
        orig_params_ptr->qos.max_sdu_size = QOS_MAX_SDU_SUBSCRIBED;
        break;
    }
  }

  /*-------------------------------------------------------------------------
    Sdu error ratio : fill in as-is
  -------------------------------------------------------------------------*/
  orig_params_ptr->qos.sdu_err_ratio = req_qos->sdu_err;

  /*-------------------------------------------------------------------------
    Res BER : fill in as-is
  -------------------------------------------------------------------------*/
  orig_params_ptr->qos.residual_ber = req_qos->res_biterr;

  /*-------------------------------------------------------------------------
    Traffic Handling priority : fill in as-is
  -------------------------------------------------------------------------*/
  orig_params_ptr->qos.traffic_priority = (uint8)req_qos->thandle_prio;

  /*-------------------------------------------------------------------------
    Transfer delay : binary encoded in 6 bits : see TS 24.008 sec 10.5.6.5
  -------------------------------------------------------------------------*/
  if( req_qos->trans_delay < 200 )
  {
    /*-----------------------------------------------------------------------
      Range 150-200 is not covered by encoding rules. so round it to 150
      6 bits with a granularity of 10 msec & 10 msec increments
    -----------------------------------------------------------------------*/
    if(req_qos->trans_delay > 150)
    {
      req_qos->trans_delay = 150;
    }
    orig_params_ptr->qos.tranfer_delay = (uint8)(req_qos->trans_delay/10);
  }
  else if( req_qos->trans_delay < 1000 )
  {
    /*-----------------------------------------------------------------------
      Delay is 200 msec + ( ( binary coded 6 bit val - 010000) * 50ms )
    -----------------------------------------------------------------------*/
    orig_params_ptr->qos.tranfer_delay =
                            (uint8)(((req_qos->trans_delay -200)/50) + 0x10);
  }
  else if( req_qos->trans_delay <= 4000 )
  {
    /*-----------------------------------------------------------------------
      Delay is 1000 msec + ( ( binary coded 6 bit val - 100000) * 100ms )
    -----------------------------------------------------------------------*/
    orig_params_ptr->qos.tranfer_delay =
                         (uint8)(((req_qos->trans_delay -1000)/100 ) + 0x20);
  }
  else
  {
    DS_3GPP_MSG0_HIGH("transfer delay out of range: set to subscribed");
    orig_params_ptr->qos.tranfer_delay = QOS_TXFER_DELAY_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Fill the signalling Indication bit if the traffic class is interactive
    and traffic priority is 1.

    Sig Ind Flag should only be set if the traffic class is interactive
    and the traffic handling priority is 1 (This is as per spec 
    23.107 Sec 6.4.3.1).

    Signalling Indication (Yes/No)
    Definition: Indicates the signalling nature of the submitted SDUs.
    This attribute is additional to the other QoS attributes and does
    not over-ride them. This attribute is only defined for the
    interactive traffic class. If signalling indication is set to "Yes",
    the UE should set the traffic handling priority to "1".

  -------------------------------------------------------------------------*/
  if ( (orig_params_ptr->qos.traffic_class == DS_UMTS_QOS_TCLASS_INTR) &&
       (orig_params_ptr->qos.traffic_priority == 0x1) )
  {
    DS_3GPP_MSG1_HIGH("Setting the sig_ind = %d", req_qos->sig_ind);
    orig_params_ptr->qos.sig_ind = req_qos->sig_ind;
  }
  /*-------------------------------------------------------------------------
    Downlink Max bit rates
  -------------------------------------------------------------------------*/
  if (req_qos->max_dl_bitrate > 8640)
  {
    orig_params_ptr->qos.max_bit_rare_downlink= QOS_BIT_RATE_8640KBPS;

    orig_params_ptr->qos.ext_max_bit_rate_downlink=
#ifdef FEATURE_NAS_REL7
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
#else 
      dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val(
#endif /* FEATURE_NAS_REL7 */
        req_qos->max_dl_bitrate);
  }
  else
  {
#ifdef FEATURE_DATA_RATE_MAPPING
    max_bitrate = dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate(
                   req_qos->max_dl_bitrate);
#else
    max_bitrate = req_qos->max_dl_bitrate;
#endif

    orig_params_ptr->qos.max_bit_rare_downlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(max_bitrate);

    orig_params_ptr->qos.ext_max_bit_rate_downlink= QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Downlink Guaranteed bit rates
  -------------------------------------------------------------------------*/
  if (req_qos->gtd_dl_bitrate > 8640)
  {
    orig_params_ptr->qos.guarantee_ber_dwnlink= QOS_BIT_RATE_8640KBPS;

    orig_params_ptr->qos.ext_gtd_bit_rate_downlink=
#ifdef FEATURE_NAS_REL7
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
#else
      dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val(
#endif /* FEATURE_NAS_REL7 */
        req_qos->gtd_dl_bitrate);
  }
  else
  {
    orig_params_ptr->qos.guarantee_ber_dwnlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
        req_qos->gtd_dl_bitrate);

    orig_params_ptr->qos.ext_gtd_bit_rate_downlink= QOS_BIT_RATE_SUBSCRIBED;
  }

#ifdef FEATURE_NAS_REL7
  /*-------------------------------------------------------------------------
    Uplink Max bit rates
  -------------------------------------------------------------------------*/
  if (req_qos->max_ul_bitrate > 8640)
  {
    orig_params_ptr->qos.max_bit_rare_uplink= QOS_BIT_RATE_8640KBPS;

    orig_params_ptr->qos.ext_max_bit_rate_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
        req_qos->max_ul_bitrate);
  }
  else
  {
#ifdef FEATURE_DATA_RATE_MAPPING
    max_bitrate = dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate(
                     req_qos->max_ul_bitrate);
#else
    max_bitrate = req_qos->max_ul_bitrate;
#endif

    orig_params_ptr->qos.max_bit_rare_uplink  =
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(max_bitrate);

    orig_params_ptr->qos.ext_max_bit_rate_uplink= QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Uplink Guaranteed bit rates
  -------------------------------------------------------------------------*/
  if (req_qos->gtd_ul_bitrate > 8640)
  {
    orig_params_ptr->qos.guarantee_ber_uplink= QOS_BIT_RATE_8640KBPS;

    orig_params_ptr->qos.ext_gtd_bit_rate_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
        req_qos->gtd_ul_bitrate);
  }
  else
  {
    orig_params_ptr->qos.guarantee_ber_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
        req_qos->gtd_ul_bitrate);

    orig_params_ptr->qos.ext_gtd_bit_rate_uplink= QOS_BIT_RATE_SUBSCRIBED;
  }
#else
  /*-------------------------------------------------------------------------
    Uplink Max and Guaranteed bit rates
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_RATE_MAPPING
  max_bitrate = dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate(
                   req_qos->max_ul_bitrate);
#else
  max_bitrate = req_qos->max_ul_bitrate;
#endif

  orig_params_ptr->qos.max_bit_rare_uplink  =
    dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(max_bitrate);

  orig_params_ptr->qos.guarantee_ber_uplink =
    dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
      req_qos->gtd_ul_bitrate);
#endif /* FEATURE_NAS_REL7 */

  return;

} /* dsUmtsBearerCntxt_fill_orig_r99_qos_params() */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_FILL_QOS_R99_PARAMS_FROM_R9798_PARAMS

DESCRIPTION
  This function fills up the Release 99 specific QOS parameters in the
  origination parameters based on the R7/98 Qos parameters.
  Octet 6 to Octet 13 of the QOS parameters in PDP context req are
  R99 qos parameters.

PARAMETERS
  req_qos_r9798      - Ptr to app requested QOS parameters
  *orig_params_ptr   - Ptr to Call origination parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

VRK:TODO Needs refactoring, take in qos_T instead of orig_params_s_type
Then same function can be used instead of these two:
dsUmtsBearerCntxt_fill_qos_r99_params_from_r9798_params
dsUmtsBearerCntxt_fill_min_qos_r99_params_from_r9798_params
===========================================================================*/
LOCAL void dsUmtsBearerCntxt_fill_qos_r99_params_from_r9798_params
(
  ds_umts_gprs_qos_params_type  *req_qos_r9798,
  cm_gw_ps_orig_params_s_type    *orig_params_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    R99 parameters (octet 6 to octet 13)deduce R99 params and fill in
    TS23.107 sec 9.1.2.2

    Traffic class
    Interactive (0x3): delay class = 1/2/3
    Bakground  (0x4) : delay class = 4
    Traffic handling priority
    1 : delay class = 1
    2 : delay class = 2
    3 : delay class = 3
  -------------------------------------------------------------------------*/
  if(req_qos_r9798->delay == QOS_DELAY_CLASS_SUBSCRIBED )
  {
    orig_params_ptr->qos.traffic_class = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
                                                               /*Subscribed*/
    orig_params_ptr->qos.traffic_priority = QOS_TH_PRIO_SUBSCRIBED ;
  }
  else if( req_qos_r9798->delay <= QOS_DELAY_CLASS_3 )
  {
    orig_params_ptr->qos.traffic_class  = DS_UMTS_QOS_TCLASS_INTR;
                                                            /* Interactive */
    orig_params_ptr->qos.traffic_priority = (uint8)req_qos_r9798->delay;
  }
  else if (req_qos_r9798->delay == QOS_DELAY_CLASS_4 )
  {
    orig_params_ptr->qos.traffic_class = DS_UMTS_QOS_TCLASS_BACK;
                                                             /* background */
    orig_params_ptr->qos.traffic_priority = QOS_TH_PRIO_SUBSCRIBED ;
  }
  else
  {
    orig_params_ptr->qos.traffic_class = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
                                                              /*Subscribed*/

    orig_params_ptr->qos.traffic_priority = QOS_TH_PRIO_SUBSCRIBED ;
  }

  /*-------------------------------------------------------------------------
    SDU error ratio,
    1*10e-6 : rel. class = 1/2
    1*10e-4 : rel. class = 3
    1*10e-3 : rel. class = 4/5
  -------------------------------------------------------------------------*/
  if(req_qos_r9798->reliability == QOS_REL_CLASS_SUBSCRIBED)
  {
    orig_params_ptr->qos.sdu_err_ratio = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
                                                               /*subscribed*/
  }
  else if(req_qos_r9798->reliability <= QOS_REL_CLASS_2)
  {
    orig_params_ptr->qos.sdu_err_ratio = DS_UMTS_QOS_SDU_ERR_1E6;
                                                               /* 1*10e-6  */
  }
  else if(req_qos_r9798->reliability == QOS_REL_CLASS_3)
  {
    orig_params_ptr->qos.sdu_err_ratio = DS_UMTS_QOS_SDU_ERR_1E4;
                                                               /* 1*10e-4  */
  }
  else if(req_qos_r9798->reliability <= QOS_REL_CLASS_5)
  {
    orig_params_ptr->qos.sdu_err_ratio = DS_UMTS_QOS_SDU_ERR_1E3;
                                                               /*  1*10e-3 */
  }
  else
  {
    orig_params_ptr->qos.sdu_err_ratio = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
                                                               /*subscribed*/
  }

  /*-------------------------------------------------------------------------
    Res BER , Delivery of err SDU
    reliablity class = 1,2,3,4 : Res BER = 1*10e-5, Err SDU = NO
    reliablity class = 5       : Res BER = 4*10e-3, Err SDU = yes
  -------------------------------------------------------------------------*/
  if(req_qos_r9798->reliability == QOS_REL_CLASS_SUBSCRIBED )
  {
    orig_params_ptr->qos.residual_ber = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
                                                             /* subscribed */
    orig_params_ptr->qos.deliver_err_sdu = DS_UMTS_QOS_DORDER_SUBSCRIBE;
                                                             /* subscribed */
  }
  else if(req_qos_r9798->reliability <= QOS_REL_CLASS_4)
  {
    orig_params_ptr->qos.residual_ber = DS_UMTS_QOS_RES_BER_1E5;
                                                 /* residual ber = 1*10e-5 */
    orig_params_ptr->qos.deliver_err_sdu = DS_UMTS_QOS_SDU_DLVR_NO;
                                                           /* err_sdu = no */
  }
  else if(req_qos_r9798->reliability == QOS_REL_CLASS_5)
  {
    orig_params_ptr->qos.residual_ber = DS_UMTS_QOS_RES_BER_4E3;
                                                 /* residual ber = 4*10e-3 */
    orig_params_ptr->qos.deliver_err_sdu = DS_UMTS_QOS_SDU_DLVR_YES;
                                                           /* err_sdu = yes*/
  }
  else
  {
    orig_params_ptr->qos.residual_ber = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
                                                             /* subscribed */
    orig_params_ptr->qos.deliver_err_sdu = DS_UMTS_QOS_SDU_DLVR_SUBSCRIBE;
                                                             /* subscribed */
  }

  /*------------------------------------------------------------------------
    Max Bit rate:
    8    kbps : peak throughput class = 1  ( 1k bytes )
    16   kbps : peak throughput class = 2  ( 2k bytes )
    32   kbps : peak throughput class = 3  ( 4k bytes )
    64   kbps : peak throughput class = 4  (8 k bytes )
    128  kbps : peak throughput class = 5  (16 k bytes )
    256  kbps : peak throughput class = 6  (32 k bytes )
    512  kbps : peak throughput class = 7  (64 k bytes )
    1024 kbps : peak throughput class = 8  (128 k bytes )
    2048 kbps : peak throughput class = 9  (256 k bytes )
  ------------------------------------------------------------------------*/
  switch (req_qos_r9798->peak)
  {
    case QOS_PEAK_TPUT_1KBYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_8KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_8KBPS;
      break;
    case QOS_PEAK_TPUT_2KBYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_16KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_16KBPS;
      break;
    case QOS_PEAK_TPUT_4KBYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_32KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_32KBPS;
      break;
    case QOS_PEAK_TPUT_8KBYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_64KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_64KBPS;
      break;
    case QOS_PEAK_TPUT_16KBYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_128KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_128KBPS;
      break;
    case QOS_PEAK_TPUT_32BYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_256KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_256KBPS;
      break;
    case QOS_PEAK_TPUT_64KBYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_512KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_512KBPS;
      break;
    case QOS_PEAK_TPUT_128KBYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_1024KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_1024KBPS;
      break;
    case QOS_PEAK_TPUT_256KBYTES:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_2048KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_2048KBPS;
      break;
    default:
      orig_params_ptr->qos.max_bit_rare_downlink  = QOS_BIT_RATE_SUBSCRIBED;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Delivery order : always set to  "subscribed"
  -------------------------------------------------------------------------*/
  orig_params_ptr->qos.deliver_order = DS_UMTS_QOS_DORDER_SUBSCRIBE;

  /*-------------------------------------------------------------------------
    Max SDU : always set to 1500 bytes
  -------------------------------------------------------------------------*/
  orig_params_ptr->qos.max_sdu_size = QOS_MAX_SDU_1500_BYTES;

} /*dsUmtsBearerCntxt_fill_qos_r99_params_from_r9798_params()*/


/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_FILL_ORIGINATION_R99_MIN_QOS_PARAMS

DESCRIPTION
  This function fills up Minimum QOS parameters in the
  origination parameters based on the PDP context specified. Only Release
  99 specific QOS parameters are filled in ( octet 6 to 13) based on the
  AT+CGEQREQ parameters


PARAMETERS
  min_qos - Ptr to user specified min_qos
  *orig_params_ptr  - Ptr to Call origination parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

VRK:TODO Needs refactoring, take in qos_T instead of orig_params_s_type
Then same function can be used instead of these two:
dsUmtsBearerCntxt_fill_origination_r99_min_qos_params() 
dsUmtsBearerCntxt_fill_orig_r99_qos_params()
===========================================================================*/
LOCAL void dsUmtsBearerCntxt_fill_origination_r99_min_qos_params
(
    ds_umts_umts_qos_params_type  *min_qos,
    cm_gw_ps_orig_params_s_type   *orig_params_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    ATCOP would allow the parameters to be a valid value or to be left
    as subscribed (value : 0x0 ).
    Set R99 parameters
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.deliver_err_sdu  = min_qos->dlvr_err_sdu;
  orig_params_ptr->minimum_qos.deliver_order = min_qos->dlvry_order;

  /*-------------------------------------------------------------------------
    Traffic class : fill as-is : matches coding rules
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.traffic_class = min_qos->traffic_class;

  /*-------------------------------------------------------------------------
    Max SDU Size : < 1500 :encoded as 8 bits having a granularity of 10bytes
                  = 1502 : 0x97,
                  = 1510 : 0x98
                  = 1520 : 0x99
  -------------------------------------------------------------------------*/
  if(min_qos->max_sdu_size <= 1500 )
  {
    orig_params_ptr->minimum_qos.max_sdu_size =
                                           (uint8)(min_qos->max_sdu_size/10);
  }
  else
  {
    switch (min_qos->max_sdu_size)
    {
      case 1502:
        orig_params_ptr->minimum_qos.max_sdu_size = QOS_MAX_SDU_1502_BYTES;
        break;

      case 1510:
        orig_params_ptr->minimum_qos.max_sdu_size = QOS_MAX_SDU_1510_BYTES;
        break;

      case 1520:
        orig_params_ptr->minimum_qos.max_sdu_size = QOS_MAX_SDU_1520_BYTES;
        break;

      default:
        DS_3GPP_MSG3_HIGH("Max SDU is out of range:%d setting to subscribed",
        min_qos->max_sdu_size,0,0);
        orig_params_ptr->minimum_qos.max_sdu_size = QOS_MAX_SDU_SUBSCRIBED;
        break;
    }
  }

  /*-------------------------------------------------------------------------
    Fill the signalling Indication bit if the traffic class is interactive.
    and the Traffic Handling priority is 1.

    Sig Ind Flag should only be set if the traffic class is interactive
    and the traffic handling priority is 1 (This is as per spec 
    23.107 Sec 6.4.3.1).

    Signalling Indication (Yes/No)
    Definition: Indicates the signalling nature of the submitted SDUs.
    This attribute is additional to the other QoS attributes and does
    not over-ride them. This attribute is only defined for the
    interactive traffic class. If signalling indication is set to "Yes",
    the UE should set the traffic handling priority to "1".
  -------------------------------------------------------------------------*/
  if( (orig_params_ptr->minimum_qos.traffic_class 
                                               == DS_UMTS_QOS_TCLASS_INTR) &&
      (orig_params_ptr->minimum_qos.traffic_priority == 0x1) )
  {
    DS_3GPP_MSG1_HIGH("Setting the sig_ind: %d", min_qos->sig_ind);
    orig_params_ptr->minimum_qos.sig_ind = min_qos->sig_ind;
  }

  /*-------------------------------------------------------------------------
    Downlink Max bit rates
  -------------------------------------------------------------------------*/
  if (min_qos->max_dl_bitrate > 8640)
  {
    orig_params_ptr->minimum_qos.max_bit_rare_downlink= QOS_BIT_RATE_8640KBPS;

    orig_params_ptr->minimum_qos.ext_max_bit_rate_downlink=
#ifdef FEATURE_NAS_REL7
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
#else
      dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val(
#endif /* FEATURE_NAS_REL7 */
        min_qos->max_dl_bitrate);
  }
  else
  {
    orig_params_ptr->minimum_qos.max_bit_rare_downlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                   min_qos->max_dl_bitrate);

    orig_params_ptr->minimum_qos.ext_max_bit_rate_downlink= QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Downlink Guaranteed bit rates
  -------------------------------------------------------------------------*/
  if (min_qos->gtd_dl_bitrate > 8640)
  {
    orig_params_ptr->minimum_qos.guarantee_ber_dwnlink = QOS_BIT_RATE_8640KBPS;

    orig_params_ptr->minimum_qos.ext_gtd_bit_rate_downlink =
#ifdef FEATURE_NAS_REL7
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
#else
      dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val(
#endif /* FEATURE_NAS_REL7 */
        min_qos->gtd_dl_bitrate);
  }
  else
  {
    orig_params_ptr->minimum_qos.guarantee_ber_dwnlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
        min_qos->gtd_dl_bitrate);

    orig_params_ptr->minimum_qos.ext_gtd_bit_rate_downlink= QOS_BIT_RATE_SUBSCRIBED;
  }

#ifdef FEATURE_NAS_REL7
  /*-------------------------------------------------------------------------
    Uplink Max bit rates
  -------------------------------------------------------------------------*/
  if (min_qos->max_ul_bitrate > 8640)
  {
    orig_params_ptr->minimum_qos.max_bit_rare_uplink= QOS_BIT_RATE_8640KBPS;

    orig_params_ptr->minimum_qos.ext_max_bit_rate_uplink =
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
                   min_qos->max_ul_bitrate);
  }
  else
  {
    orig_params_ptr->minimum_qos.max_bit_rare_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                   min_qos->max_ul_bitrate);

    orig_params_ptr->minimum_qos.ext_max_bit_rate_downlink = QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Uplink Guaranteed bit rates
  -------------------------------------------------------------------------*/
  if (min_qos->gtd_ul_bitrate > 8640)
  {
    orig_params_ptr->minimum_qos.guarantee_ber_uplink= QOS_BIT_RATE_8640KBPS;

    orig_params_ptr->minimum_qos.ext_gtd_bit_rate_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
        min_qos->gtd_ul_bitrate);
  }
  else
  {
    orig_params_ptr->minimum_qos.guarantee_ber_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
        min_qos->gtd_ul_bitrate);

    orig_params_ptr->minimum_qos.ext_gtd_bit_rate_uplink= QOS_BIT_RATE_SUBSCRIBED;
  }
#else
  /*-------------------------------------------------------------------------
    Uplink Max and Guaranteed bit rates
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.max_bit_rare_uplink  =
    dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                   min_qos->max_ul_bitrate);

  orig_params_ptr->minimum_qos.guarantee_ber_uplink =
    dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
      min_qos->gtd_ul_bitrate);
#endif /* FEATURE_NAS_REL7 */

  /*-------------------------------------------------------------------------
    Sdu error ratio : fill in as-is
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.sdu_err_ratio = min_qos->sdu_err;

  /*-------------------------------------------------------------------------
    Res BER : fill in as-is
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.residual_ber = min_qos->res_biterr;

  /*-------------------------------------------------------------------------
    Traffic Handling priority : fill in as-is
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.traffic_priority = (uint8)min_qos->thandle_prio;

  /*-------------------------------------------------------------------------
    Transfer delay : binary encoded in 6 bits : see TS 24.008 sec 10.5.6.5
  -------------------------------------------------------------------------*/
  if( min_qos->trans_delay < 200 )
  {
    /*-----------------------------------------------------------------------
      Range 150-200 is not covered by encoding rules. so round it to 150
      6 bits with a granularity of 10 msec & 10 msec increments
    -----------------------------------------------------------------------*/
    if(min_qos->trans_delay > 150)
    {
      min_qos->trans_delay = 150;
    }
    orig_params_ptr->minimum_qos.tranfer_delay =
      (uint8)(min_qos->trans_delay/10);
  }
  else if( min_qos->trans_delay < 1000 )
  {
    /*-----------------------------------------------------------------------
      Delay is 200 msec + ( ( binary coded 6 bit val - 010000) * 50ms )
    -----------------------------------------------------------------------*/
    orig_params_ptr->minimum_qos.tranfer_delay =
      (uint8)(((min_qos->trans_delay -200)/50) + 0x10);
  }
  else if( min_qos->trans_delay <= 4000 )
  {
    /*-----------------------------------------------------------------------
      Delay is 1000 msec + ( ( binary coded 6 bit val - 100000) * 100ms )
    -----------------------------------------------------------------------*/
    orig_params_ptr->minimum_qos.tranfer_delay =
      (uint8)(((min_qos->trans_delay -1000)/100 ) + 0x20);
  }
  else
  {
    DS_3GPP_MSG0_HIGH("transfer delay out of range: set to subscribed");
    orig_params_ptr->minimum_qos.tranfer_delay = QOS_TXFER_DELAY_SUBSCRIBED;
  }

  return;

} /* dsUmtsBearerCntxt_fill_origination_r99_min_qos_params */


/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_FILL_MIN_QOS_R99_PARAMS_FROM_R9798_PARAMS

DESCRIPTION
  This function fills up the Minimum Release 99 specific QOS parameters in
  the origination parameters based on the R7/98 Qos parameters.

PARAMETERS
  min_qos_r9798  - Ptr to user specified r9798 minimum qos
  *orig_params_ptr   - Ptr to Call origination parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntxt_fill_min_qos_r99_params_from_r9798_params
(
  ds_umts_gprs_qos_params_type  *min_qos_r9798,
  cm_gw_ps_orig_params_s_type   *orig_params_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    R99 parameters

    Traffic class
    Interactive (0x3): delay class = 1/2/3
    Bakground  (0x4) : delay class = 4
    Traffic handling priority
    1 : delay class = 1
    2 : delay class = 2
    3 : delay class = 3
  -------------------------------------------------------------------------*/
  if(min_qos_r9798->delay == QOS_DELAY_CLASS_SUBSCRIBED )
  {
    orig_params_ptr->minimum_qos.traffic_class = DS_UMTS_QOS_TCLASS_SUBSCRIBE;

                                                               /*Subscribed*/
    orig_params_ptr->minimum_qos.traffic_priority = QOS_TH_PRIO_SUBSCRIBED ;
  }
  else if( min_qos_r9798->delay <= QOS_DELAY_CLASS_3 )
  {
    orig_params_ptr->minimum_qos.traffic_class  = DS_UMTS_QOS_TCLASS_INTR;
                                                            /* Interactive */
    orig_params_ptr->minimum_qos.traffic_priority =
                                                 (uint8)min_qos_r9798->delay;
  }
  else if (min_qos_r9798->delay == QOS_DELAY_CLASS_4 )
  {
    orig_params_ptr->minimum_qos.traffic_class = DS_UMTS_QOS_TCLASS_BACK;
                                                             /* background */

    orig_params_ptr->minimum_qos.traffic_priority = QOS_TH_PRIO_SUBSCRIBED ;
  }
  else
  {
    orig_params_ptr->minimum_qos.traffic_class = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
                                                               /*Subscribed*/

    orig_params_ptr->minimum_qos.traffic_priority = QOS_TH_PRIO_SUBSCRIBED ;
  }

  /*-------------------------------------------------------------------------
    SDU error ratio,
    1*10e-6 : rel. class = 1/2
    1*10e-4 : rel. class = 3
    1*10e-3 : rel. class = 4/5
  -------------------------------------------------------------------------*/
  if(min_qos_r9798->reliability == QOS_REL_CLASS_SUBSCRIBED)
  {
    orig_params_ptr->minimum_qos.sdu_err_ratio=DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
                                                               /*subscribed*/
  }
  else if(min_qos_r9798->reliability <= QOS_REL_CLASS_2)
  {
    orig_params_ptr->minimum_qos.sdu_err_ratio = DS_UMTS_QOS_SDU_ERR_1E6;
                                                               /* 1*10e-6  */
  }
  else if(min_qos_r9798->reliability == QOS_REL_CLASS_3)
  {
    orig_params_ptr->minimum_qos.sdu_err_ratio = DS_UMTS_QOS_SDU_ERR_1E4;
                                                               /* 1*10e-4  */
  }
  else if(min_qos_r9798->reliability <= QOS_REL_CLASS_5)
  {
    orig_params_ptr->minimum_qos.sdu_err_ratio = DS_UMTS_QOS_SDU_ERR_1E3;
                                                               /*  1*10e-3 */
  }
  else
  {
    orig_params_ptr->minimum_qos.sdu_err_ratio=DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
                                                               /*subscribed*/
  }

  /*-------------------------------------------------------------------------
    Res BER , Delivery of err SDU
    reliablity class = 1,2,3,4 : Res BER = 1*10e-5, Err SDU = NO
    reliablity class = 5       : Res BER = 4*10e-3, Err SDU = yes
  -------------------------------------------------------------------------*/
  if(min_qos_r9798->reliability == QOS_REL_CLASS_SUBSCRIBED )
  {
    orig_params_ptr->minimum_qos.residual_ber = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
                                                             /* subscribed */
    orig_params_ptr->minimum_qos.deliver_err_sdu = DS_UMTS_QOS_DORDER_SUBSCRIBE;
                                                             /* subscribed */
  }
  else if(min_qos_r9798->reliability <= QOS_REL_CLASS_4)
  {
    orig_params_ptr->minimum_qos.residual_ber = DS_UMTS_QOS_RES_BER_1E5;
                                                 /* residual ber = 1*10e-5 */
    orig_params_ptr->minimum_qos.deliver_err_sdu = DS_UMTS_QOS_SDU_DLVR_NO;
                                                           /* err_sdu = no */
  }
  else if(min_qos_r9798->reliability == QOS_REL_CLASS_5)
  {
    orig_params_ptr->minimum_qos.residual_ber = DS_UMTS_QOS_RES_BER_4E3;
                                                 /* residual ber = 4*10e-3 */
    orig_params_ptr->minimum_qos.deliver_err_sdu = DS_UMTS_QOS_SDU_DLVR_YES;
                                                           /* err_sdu = yes*/
  }
  else
  {
    orig_params_ptr->minimum_qos.residual_ber = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
                                                             /* subscribed */
    orig_params_ptr->minimum_qos.deliver_err_sdu = DS_UMTS_QOS_SDU_DLVR_SUBSCRIBE;
                                                             /* subscribed */
  }

  /*-------------------------------------------------------------------------
    Max Bit rate:
    8    kbps : peak throughput class = 1  ( 1k bytes )
    16   kbps : peak throughput class = 2  ( 2k bytes )
    32   kbps : peak throughput class = 3  ( 4k bytes )
    64   kbps : peak throughput class = 4  (8 k bytes )
    128  kbps : peak throughput class = 5  (16 k bytes )
    256  kbps : peak throughput class = 6  (32 k bytes )
    512  kbps : peak throughput class = 7  (64 k bytes )
    1024 kbps : peak throughput class = 8  (128 k bytes )
    2048 kbps : peak throughput class = 9  (256 k bytes )
  -------------------------------------------------------------------------*/
  switch (min_qos_r9798->peak)
  {
    case QOS_PEAK_TPUT_1KBYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_8KBPS;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_8KBPS;
      break;
    case QOS_PEAK_TPUT_2KBYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_16KBPS;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_16KBPS;
      break;
    case QOS_PEAK_TPUT_4KBYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_32KBPS;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_32KBPS;
      break;
    case QOS_PEAK_TPUT_8KBYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_64KBPS;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_64KBPS;
      break;
    case QOS_PEAK_TPUT_16KBYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_128KBPS;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_128KBPS;
      break;
    case QOS_PEAK_TPUT_32BYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_256KBPS;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_256KBPS;
      break;
    case QOS_PEAK_TPUT_64KBYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_512KBPS;
      orig_params_ptr->qos.max_bit_rare_uplink    = QOS_BIT_RATE_512KBPS;
      break;
    case QOS_PEAK_TPUT_128KBYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_1024KBPS;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_1024KBPS;
      break;
    case QOS_PEAK_TPUT_256KBYTES:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_2048KBPS;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_2048KBPS;
      break;
    default:
      orig_params_ptr->minimum_qos.max_bit_rare_downlink  = QOS_BIT_RATE_SUBSCRIBED;
      orig_params_ptr->minimum_qos.max_bit_rare_uplink    = QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Delivery order : always set to  "subscribed"
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.deliver_order = DS_UMTS_QOS_DORDER_SUBSCRIBE;

  /*-------------------------------------------------------------------------
    Max SDU : always set to 1500 bytes
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.max_sdu_size = QOS_MAX_SDU_1500_BYTES;

} /* dsUmtsBearerCntxt_fill_min_qos_r99_params_from_r9798_params */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_FILL_QOS_R9798_PARAMS_FROM_R99_PARAMS

DESCRIPTION
  This function fills up the Release 97/98 specific QOS parameters in the
  origination and modification parameters based on the R99 Qos parameters.
  Octet 3 to Octet 5 of the QOS parameters in PDP context req are
  R97/98 qos parameters.

PARAMETERS
  *app_req_qos_params - Ptr to requested QoS parameters for the current call
  *ota_encoded_qos_params - Ptr to QOS parameters to be filled for the 
                                current call

DEPENDENCIES
  For origination, assumes that the R99 parameters have been filled in 
  already in the orig_params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntxt_fill_qos_r9798_params_from_r99_params
(
  const ds_umts_umts_qos_params_type *app_req_qos_params,
  qos_T                              *ota_encoded_qos_params
)
{
  uint32  max_bit_rate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    R97/R98 parameters (octet 3 to octet 5)
    Deduce R97/98 params from R99 parameters based on mapping rules defined
    in TS23.107 Section 9.1.2.3and fill in

    Reliability class: Octet 3  :
    2: sdu err <= 10e-5
    3: sdu err 10e-5 < x <= 5*10e-4
    4: sdu err > 5*10e-4  and res ber <= 2*10e-4
    5: sdu err > 5*10e-4  and res ber > 2*10e-4

   Note: Subscribed ( sdu err & res BER ) is the value 0x0. account for it
   in the comparisions
  -------------------------------------------------------------------------*/
  if( app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_SUBSCRIBE )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_SUBSCRIBED;
  }
  else if( (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E5 ) ||
           (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E6 ) )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_2;
  }
  else if (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E4 )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_3;
  }
  else if ( ( (app_req_qos_params->sdu_err < DS_UMTS_QOS_SDU_ERR_1E4 ) ||
              (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E1) ) &&
              (app_req_qos_params->res_biterr >= DS_UMTS_QOS_RES_BER_1E4 ) )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_4;

  }
  else if ( ( (app_req_qos_params->sdu_err < DS_UMTS_QOS_SDU_ERR_1E4 )||
              (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E1) ) &&
            ( (app_req_qos_params->res_biterr < DS_UMTS_QOS_RES_BER_1E4 ) &&
              (app_req_qos_params->res_biterr != DS_UMTS_QOS_RES_BER_SUBSCRIBE ) ) )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_5;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set to subscribed
    -----------------------------------------------------------------------*/
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Delay Class :
    1: Traffic class = conversational or streaming
    1: Traffic class = interactive and Traffic handling pri =1
    2: Traffic class = interactive and Traffic handling pri =2
    3: Traffic class = interactive and Traffic handling pri =3
    4: Traffic class = background
  -------------------------------------------------------------------------*/
  if( app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_SUBSCRIBE )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_SUBSCRIBED;
  }
  else if( app_req_qos_params->traffic_class <= DS_UMTS_QOS_TCLASS_STRM )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_1;
  }
  else if ( (app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_req_qos_params->thandle_prio == 0x1) )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_1;
  }
  else if ( (app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_req_qos_params->thandle_prio == 0x2) )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_2;
  }
  else if ( (app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_req_qos_params->thandle_prio == 0x3) )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_3;
  }
  else if( app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_BACK )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_4;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set to subscribed
    -----------------------------------------------------------------------*/
    ota_encoded_qos_params->delay_class  = QOS_DELAY_CLASS_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    precedence Class :  always set to subscribed (0x0)
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->precedence_class = QOS_PRECED_CLASS_SUBSCRIBED;

  /*-------------------------------------------------------------------------
    Peak Throughput : In case of asymetric bearers the higher of dl/ul
    is selected and used as the max bit rate
    1Kbytes  : max bit rate < 16            (kbits/sec)
    2Kbytes  : max bit rate 16 <= x < 32    (kbits/sec)
    4Kbytes  : max bit rate 32 <= x < 64    (kbits/sec)
    8Kbytes  : max bit rate 64 <= x < 128   (kbits/sec)
    16Kbytes : max bit rate 128 <= x < 256  (kbits/sec)
    32Kbytes : max bit rate 256 <= x < 512  (kbits/sec)
    64Kbytes : max bit rate 512 <= x < 1024 (kbits/sec)
    128Kbytes: max bit rate 1024 <= x <2048 (kbits/sec)
    256Kbytes: max bit rate >= 2048         (kbits/sec)
  Note : max bit rate (ul/dl) specified as 0xff indicates '0' kbits/sec
  -------------------------------------------------------------------------*/
  if ( (ota_encoded_qos_params->max_bit_rare_uplink >
        ota_encoded_qos_params->max_bit_rare_downlink )  &&
       (ota_encoded_qos_params->max_bit_rare_uplink !=
          QOS_BIT_RATE_0KBPS )
     )
  {
    max_bit_rate = ota_encoded_qos_params->max_bit_rare_uplink;
  }
  else
  {
    max_bit_rate = ota_encoded_qos_params->max_bit_rare_downlink;
  }

  /*-------------------------------------------------------------------------
    Start encoding Peak throughput
  -------------------------------------------------------------------------*/
  if ( max_bit_rate ==  QOS_BIT_RATE_0KBPS ||
       max_bit_rate ==  QOS_BIT_RATE_SUBSCRIBED )
  {
   ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_SUBSCRIBED;
  }
  else if( max_bit_rate < QOS_BIT_RATE_16KBPS )
  {
   ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_1KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_32KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_2KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_64KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_4KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_128KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_8KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_256KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_16KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_512KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_32BYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_1024KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_64KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_2048KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_128KBYTES;
  }
  else  
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_256KBYTES;
  }

  /*-------------------------------------------------------------------------
    Mean throughput : always set to best effort (0x1f)
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->mean_throughput      = QOS_MEAN_TPUT_BEST_EFFORT;
  return;

} /* dsUmtsBearerCntxt_fill_qos_r9798_params_from_r99_params */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_FILL_MIN_QOS_R9798_PARAMS_FROM_R99_PARAMS

DESCRIPTION
  This function fills up the Minumum Release 97/98 specific QOS parameters in
  the origination or modification parameters based on the R99 Qos parameters.

PARAMETERS
  *app_min_qos_parms      - Ptr to minimum QOS parameters for the current call
  *ota_encoded_min_qos_params - Ptr to QOS parameters to be filled for the current 
                                    call

DEPENDENCIES
  For origination, assumes that the R99 parameters have been filled in already 
  in the orig_params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  None.
VRK:TODO Same functionality as dsUmtsBearerCntxt_fill_qos_r9798_params_from_r99_params
Get rid of the Duplicate
===========================================================================*/
LOCAL void dsUmtsBearerCntxt_fill_min_qos_r9798_params_from_r99_params
(
  const ds_umts_umts_qos_params_type *app_min_qos_params,
  qos_T                              *ota_encoded_min_qos_params
)
{
  uint32  max_bit_rate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    R97/R98 parameters
    Deduce R97/98 params from R99 parameters based on mapping rules defined
    in TS23.107 Section 9.1.2.3and fill in

    Reliability class:
    2: sdu err <= 10e-5
    3: sdu err 10e-5 < x <= 5*10e-4
    4: sdu err > 5*10e-4  and res ber <= 2*10e-4
    5: sdu err > 5*10e-4  and res ber > 2*10e-4

   Note: Subscribed ( sdu err & res BER ) is the value 0x0. account for it
   in the comparisions
  -------------------------------------------------------------------------*/
  if( app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_SUBSCRIBE )
  {
    ota_encoded_min_qos_params->reliability_class=
                                            (uint8)QOS_REL_CLASS_SUBSCRIBED;
  }
  else if( (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E5 ) ||
           (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E6 ) )
  {
    ota_encoded_min_qos_params->reliability_class  = QOS_REL_CLASS_2;
  }
  else if (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E4 )
  {
    ota_encoded_min_qos_params->reliability_class  = QOS_REL_CLASS_3;
  }
  else if ( ( (app_min_qos_params->sdu_err < DS_UMTS_QOS_SDU_ERR_1E4 ) ||
              (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E1) ) &&
            ( app_min_qos_params->res_biterr >= DS_UMTS_QOS_RES_BER_1E4 ) )
  {
    ota_encoded_min_qos_params->reliability_class  = QOS_REL_CLASS_4;

  }
  else if (((app_min_qos_params->sdu_err < DS_UMTS_QOS_SDU_ERR_1E4 )||
            (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E1) ) &&
           ((app_min_qos_params->res_biterr < DS_UMTS_QOS_RES_BER_1E4 ) &&
            (app_min_qos_params->res_biterr != DS_UMTS_QOS_RES_BER_SUBSCRIBE)))
  {
    ota_encoded_min_qos_params->reliability_class  = QOS_REL_CLASS_5;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set to subscribed
    -----------------------------------------------------------------------*/
    ota_encoded_min_qos_params->reliability_class= QOS_REL_CLASS_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Delay Class :
    1: Traffic class = conversational or streaming
    1: Traffic class = interactive and Traffic handling pri =1
    2: Traffic class = interactive and Traffic handling pri =2
    3: Traffic class = interactive and Traffic handling pri =3
    4: Traffic class = background
  -------------------------------------------------------------------------*/
  if( app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_SUBSCRIBE )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_SUBSCRIBED;
  }
  else if( app_min_qos_params->traffic_class <= DS_UMTS_QOS_TCLASS_STRM )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_1;
  }
  else if ( (app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_min_qos_params->thandle_prio == 0x1) )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_1;
  }
  else if ( (app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_min_qos_params->thandle_prio == 0x2) )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_2;
  }
  else if ( (app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_min_qos_params->thandle_prio == 0x3) )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_3;
  }
  else if( app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_BACK )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_4;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set to subscribed
    -----------------------------------------------------------------------*/
    ota_encoded_min_qos_params->delay_class  = QOS_DELAY_CLASS_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    precedence Class :  always set to subscribed (0x0)
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->precedence_class= QOS_PRECED_CLASS_SUBSCRIBED;

  /*-------------------------------------------------------------------------
    Peak Throughput : In case of asymetric bearers the higher of dl/ul
    is selected and used as the max bit rate
    1Kbytes  : max bit rate < 16            (kbits/sec)
    2Kbytes  : max bit rate 16 <= x < 32    (kbits/sec)
    4Kbytes  : max bit rate 32 <= x < 64    (kbits/sec)
    8Kbytes  : max bit rate 64 <= x < 128   (kbits/sec)
    16Kbytes : max bit rate 128 <= x < 256  (kbits/sec)
    32Kbytes : max bit rate 256 <= x < 512  (kbits/sec)
    64Kbytes : max bit rate 512 <= x < 1024 (kbits/sec)
    128Kbytes: max bit rate 1024 <= x <2048 (kbits/sec)
    256Kbytes: max bit rate >= 2048         (kbits/sec)
  Note : max bit rate (ul/dl) specified as 0xff indicates '0' kbits/sec
  -------------------------------------------------------------------------*/
  if ( (ota_encoded_min_qos_params->max_bit_rare_uplink >
        ota_encoded_min_qos_params->max_bit_rare_downlink )  &&
       (ota_encoded_min_qos_params->max_bit_rare_uplink !=
          QOS_BIT_RATE_0KBPS )
     )
  {
    max_bit_rate = ota_encoded_min_qos_params->max_bit_rare_uplink;
  }
  else
  {
    max_bit_rate = ota_encoded_min_qos_params->max_bit_rare_downlink;
  }

  /*-------------------------------------------------------------------------
    Start encoding Peak throughput
  -------------------------------------------------------------------------*/
  if ( max_bit_rate ==  QOS_BIT_RATE_0KBPS ||
       max_bit_rate ==  QOS_BIT_RATE_SUBSCRIBED )
  {
   ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_SUBSCRIBED;
  }
  else if( max_bit_rate < QOS_BIT_RATE_16KBPS )
  {
   ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_1KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_32KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_2KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_64KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_4KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_128KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_8KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_256KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_16KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_512KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_32BYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_1024KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_64KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_2048KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_128KBYTES;
  }
  else  
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_256KBYTES;
  }

  /*-------------------------------------------------------------------------
    Mean throughput : always set to best effort (0x1f)
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->mean_throughput   = QOS_MEAN_TPUT_BEST_EFFORT;
  return;

} /* dsUmtsBearerCntxt_fill_min_qos_r9798_params_from_r99_params */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_FILL_ORIGINATION_QOS_PARAMS

DESCRIPTION
  This function fills up the QOS parameters in the origination parameters
  based on the PDP context specified

PARAMETERS
  pdp_profile_parms - Ptr to PDP profile parameters for the current call
  *orig_params_ptr  - Ptr to Call origination parameters

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Success
  FALSE - Failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsUmtsBearerCntxt_fill_origination_qos_params
(
    ds_bearer_context_s          *bearer_context_p,
    ds_umts_pdp_profile_type     *pdp_profile_parms,
    ds_3gpp_orig_params_s        *orig_params_p
)
{
  cm_gw_ps_orig_params_s_type   *orig_params_ptr = NULL;
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
  ds_umts_bearer_flow_manager_s *umts_bflow_manager_p = NULL;
  ds_flow_context_s             *flow_context_p = NULL;
  ds_umts_umts_qos_params_type  *req_qos = NULL;
  ds_umts_umts_qos_params_type  *min_qos = NULL;
  ds_umts_gprs_qos_params_type  *req_qos_r9798 = NULL;
  ds_umts_gprs_qos_params_type  *min_qos_r9798 = NULL;
  sys_sys_mode_e_type  umts_network_mode = SYS_SYS_MODE_NONE;
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined FEATURE_WCDMA
    umts_network_mode = SYS_SYS_MODE_WCDMA;
#elif defined FEATURE_TDSCDMA
    umts_network_mode =  SYS_SYS_MODE_TDS;
#elif defined FEATURE_GSM_GPRS
    umts_network_mode = SYS_SYS_MODE_GSM;
#endif
  
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  orig_params_ptr = orig_params_p->orig_params_u.gw_ps_orig_params_ptr;
  /*-------------------------------------------------------------------------
    If QOS params are specified using AT+CGEQREQ or AT+CGQREQ,
    use it else set values to subscribed ( all values set to '0' )
    start off with all values set to subscribed
  -------------------------------------------------------------------------*/
  orig_params_ptr->qos.valid                = TRUE;

  /*-------------------------------------------------------------------------
    request Subscribed QOS : NOTE: All values are set to 0x0 implying
    subscribed value : ref TS24.008 sec 10.5.6.5
  -------------------------------------------------------------------------*/
  orig_params_ptr->qos.delay_class          = 0x0;
  orig_params_ptr->qos.reliability_class    = 0x0;
  orig_params_ptr->qos.peak_throughput      = 0x0;
  orig_params_ptr->qos.precedence_class     = 0x0;
  orig_params_ptr->qos.mean_throughput      = 0x0;
  orig_params_ptr->qos.traffic_class        = 0x0;
  orig_params_ptr->qos.deliver_order        = 0x0;
  orig_params_ptr->qos.deliver_err_sdu      = 0x0;
  orig_params_ptr->qos.max_sdu_size         = 0x0;
  orig_params_ptr->qos.max_bit_rare_uplink  = 0x0;
  orig_params_ptr->qos.max_bit_rare_downlink= 0x0;
  orig_params_ptr->qos.residual_ber         = 0x0;
  orig_params_ptr->qos.sdu_err_ratio        = 0x0;
  orig_params_ptr->qos.tranfer_delay        = 0x0;
  orig_params_ptr->qos.traffic_priority     = 0x0;
  orig_params_ptr->qos.guarantee_ber_uplink = 0x0;
  orig_params_ptr->qos.guarantee_ber_dwnlink= 0x0;
  orig_params_ptr->qos.sig_ind = FALSE;
  orig_params_ptr->qos.src_stat_desc = 0;
  orig_params_ptr->qos.ext_max_bit_rate_downlink= 0x0;
  orig_params_ptr->qos.ext_gtd_bit_rate_downlink= 0x0;
#ifdef FEATURE_NAS_REL7
  orig_params_ptr->qos.ext_max_bit_rate_uplink= 0x0;
  orig_params_ptr->qos.ext_gtd_bit_rate_uplink= 0x0;
#endif /* FEATURE_NAS_REL7 */
  /*-------------------------------------------------------------------------
    If QOS params are specified using AT+CGEQMIN or AT+CGQMIN,
    use it else set values to subscribed ( all values set to '0' )
    start off with all values set to subscribed
  -------------------------------------------------------------------------*/
  orig_params_ptr->qos.valid                = TRUE;

  /*-------------------------------------------------------------------------
    Minimum Subscribed QOS : NOTE: All values are set to 0x0 implying
    subscribed value : ref TS24.008 sec 10.5.6.5
  -------------------------------------------------------------------------*/
  orig_params_ptr->minimum_qos.valid                = TRUE;
  orig_params_ptr->minimum_qos.delay_class          = 0x0;
  orig_params_ptr->minimum_qos.reliability_class    = 0x0;
  orig_params_ptr->minimum_qos.peak_throughput      = 0x0;
  orig_params_ptr->minimum_qos.precedence_class     = 0x0;
  orig_params_ptr->minimum_qos.mean_throughput      = 0x0;
  orig_params_ptr->minimum_qos.traffic_class        = 0x0;
  orig_params_ptr->minimum_qos.deliver_order        = 0x0;
  orig_params_ptr->minimum_qos.deliver_err_sdu      = 0x0;
  orig_params_ptr->minimum_qos.max_sdu_size         = 0x0;
  orig_params_ptr->minimum_qos.max_bit_rare_uplink  = 0x0;
  orig_params_ptr->minimum_qos.max_bit_rare_downlink= 0x0;
  orig_params_ptr->minimum_qos.residual_ber         = 0x0;
  orig_params_ptr->minimum_qos.sdu_err_ratio        = 0x0;
  orig_params_ptr->minimum_qos.tranfer_delay        = 0x0;
  orig_params_ptr->minimum_qos.traffic_priority     = 0x0;
  orig_params_ptr->minimum_qos.guarantee_ber_uplink = 0x0;
  orig_params_ptr->minimum_qos.guarantee_ber_dwnlink= 0x0;
  orig_params_ptr->minimum_qos.sig_ind = FALSE;
  orig_params_ptr->minimum_qos.src_stat_desc = 0;
  orig_params_ptr->minimum_qos.ext_max_bit_rate_downlink= 0x0;
  orig_params_ptr->minimum_qos.ext_gtd_bit_rate_downlink= 0x0;
#ifdef FEATURE_NAS_REL7
  orig_params_ptr->minimum_qos.ext_max_bit_rate_uplink= 0x0;
  orig_params_ptr->minimum_qos.ext_gtd_bit_rate_uplink= 0x0;
#endif /* FEATURE_NAS_REL7 */

  if(bearer_context_p->ds_bearer_context_dyn_p->is_default == TRUE)
  {
    /*-------------------------------------------------------------------------
      Get the QOS parameters from the profile for the default bearer
    -------------------------------------------------------------------------*/
    req_qos = &(pdp_profile_parms->qos_request_umts);
    min_qos = &(pdp_profile_parms->qos_minimum_umts);
    req_qos_r9798 = &(pdp_profile_parms->qos_request_gprs);
    min_qos_r9798 = &(pdp_profile_parms->qos_minimum_gprs);

  }//default_bearer
  else
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    /*-------------------------------------------------------------------------
      Get the app specified QOS parameters saved in bearer_flow_manager
      VRK:TODO Do I need critSect here?
    -------------------------------------------------------------------------*/    
    bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
    if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return DS3G_FAILURE;
    }

    /*-----------------------------------------------------------------------
      Get the pending flow from pdn context
    -----------------------------------------------------------------------*/
    
    flow_context_p = ds_flow_manager_find_pending_flow_context_for_pdn(
                        (ds_pdn_context_s*)bearer_context_p->ds_bearer_context_dyn_p
                        ->pdn_context_ptr,
                         bearer_context_p->ds_bearer_context_dyn_p->sdf_id);
    if(!ds_flow_context_validate_flow(flow_context_p))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return DS3G_FAILURE;
    }
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

    memscpy(&(orig_params_ptr->tft), sizeof(orig_params_ptr->tft),
            &(flow_context_p->ds_flow_context_dyn_p->tft_mod),
            sizeof(tft_type_T));
    
    /*---------------------------------------------------------------------
      Get the mode specific blob.
      WCDMA/GSM/TDSCDMA both point to the same object
    ---------------------------------------------------------------------*/


    umts_bflow_manager_p = (ds_umts_bearer_flow_manager_s *)
                 DS_BEARER_FLOW_MGR_VOBJ(bflow_manager_p,umts_network_mode);
    if((umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p == NULL) ||
       (umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p
                             ->cookie!= UMTS_BEARER_FLOW_MANAGER_VALID_COOKIE))
    {
      DS_3GPP_MSG0_ERROR("Missing UMTS Bearer Flow Manager!");      
      return DS3G_FAILURE;
    }



    min_qos = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->app_sps_min_qos);
    req_qos = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->app_sps_req_qos); 
    req_qos_r9798 = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->gprs_app_req_qos);
    min_qos_r9798 = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->gprs_app_min_qos);

  }//dedicated bearer

  /*-------------------------------------------------------------------------
    Set UMTS (rel 99) and GPRS (rel 97/98) qos parameters if specified via AT
    command (AT+CGEQREQ), (AT+CGQREQ). If both parameters are specified fill
    it as it is. Otherwise convert one set of parameters to the one not
    defined.
  -------------------------------------------------------------------------*/
  if(req_qos->valid_flg)
  {
    DS_3GPP_MSG0_HIGH("AT+CGEQREQ specified: filling user req qos");
    dsUmtsBearerCntxt_fill_orig_r99_qos_params(req_qos, orig_params_ptr);
    /*-------------------------------------------------------------------------
      Fill the Source Statistics  bit if the traffic class is Conversational
      or Streaming.
    -------------------------------------------------------------------------*/
    if ( orig_params_ptr->qos.traffic_class == DS_UMTS_QOS_TCLASS_CONV ||
         orig_params_ptr->qos.traffic_class == DS_UMTS_QOS_TCLASS_STRM )
    {
      DS_3GPP_MSG1_HIGH("Setting the src_stat_desc = %d", pdp_profile_parms->src_stat_desc_req);
      orig_params_ptr->qos.src_stat_desc = (byte)pdp_profile_parms->src_stat_desc_req;
    }

    /*-----------------------------------------------------------------------
      R97/98 parameters( octet 3 to octet 5 ) Spec mandates that we translate
      R99 parameters to corresponding R97 values and fill in if R97/98
      parameters are not specified
    -----------------------------------------------------------------------*/
    if( (req_qos_r9798 == NULL) || !(req_qos_r9798->valid_flg) )
    {
      dsUmtsBearerCntxt_fill_qos_r9798_params_from_r99_params(req_qos,
                                                    &(orig_params_ptr->qos));
    }
  }

  if( (req_qos_r9798 != NULL) && (req_qos_r9798->valid_flg) )
  {
    /*-----------------------------------------------------------------------
      Set GPRS (rel 97/98) qos parameters if specified via AT command
      ATCOP would allow the parameters to be a valid value or to be left
      as subscribed (value : 0x0 ).
      Set R97/98 parameters ( octet 3 to octet 5 )
      Fill parameters as-is from atcop. No further encoding is required
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("AT+CGQREQ specified: filling user req qos");

    orig_params_ptr->qos.reliability_class  = (uint8)req_qos_r9798->reliability;
    orig_params_ptr->qos.delay_class        = (uint8)req_qos_r9798->delay;
    orig_params_ptr->qos.precedence_class   = (uint8)req_qos_r9798->precedence;
    orig_params_ptr->qos.peak_throughput    = (uint8)req_qos_r9798->peak;
    orig_params_ptr->qos.mean_throughput    = (uint8)req_qos_r9798->mean;

    /*-----------------------------------------------------------------------
      R99 parameters ( octet 6 to octet 13 ) Spec mandates that we translate
      R97/98 parameters to corresponding R99 values and fill in if R99
      parameters are not specified
    -----------------------------------------------------------------------*/
    if(!req_qos->valid_flg)
    {
      dsUmtsBearerCntxt_fill_qos_r99_params_from_r9798_params(req_qos_r9798,
                                                   orig_params_ptr);
    }
  }

  /*-------------------------------------------------------------------------
    Set UMTS (rel 99) and GPRS (rel 97/98) qos parameters if specified via AT
    command (AT+CGEQMIN), (AT+CGQMIN). If both parameters are specified fill
    it as it is. Otherwise convert one set of parameters to the one not
    defined.
  -------------------------------------------------------------------------*/
  if(min_qos->valid_flg)
  {
    DS_3GPP_MSG0_HIGH("AT+CGEQMIN specified: filling Minimum qos");
    dsUmtsBearerCntxt_fill_origination_r99_min_qos_params(min_qos,
                                             orig_params_ptr);
    /*-------------------------------------------------------------------------
      Fill the source statistics descriptor if the traffic class is 
      conversational or streaming.
    
      If source statistics descriptor value is 1, then it is speech class.
    -------------------------------------------------------------------------*/
    if( orig_params_ptr->minimum_qos.traffic_class == DS_UMTS_QOS_TCLASS_CONV ||
        orig_params_ptr->minimum_qos.traffic_class == DS_UMTS_QOS_TCLASS_STRM )
    {
      DS_3GPP_MSG1_HIGH("Setting the src_stat_desc: %d", pdp_profile_parms->src_stat_desc_min);
      orig_params_ptr->minimum_qos.src_stat_desc = (byte)pdp_profile_parms->src_stat_desc_min;
    }

    /*-----------------------------------------------------------------------
      R97/98 parameters Spec mandates that we translate
      R99 parameters to corresponding R97 values and fill in if R97/98
      parameters are not specified
    -----------------------------------------------------------------------*/
    if( (min_qos_r9798 == NULL) || !(min_qos_r9798->valid_flg) )
    {
       dsUmtsBearerCntxt_fill_min_qos_r9798_params_from_r99_params(
                                             min_qos,
                                             &orig_params_ptr->minimum_qos );
    }
  }

  if( (min_qos_r9798 != NULL) && (min_qos_r9798->valid_flg) )
  {
    /*-----------------------------------------------------------------------
      Set GPRS (rel 97/98) qos parameters if specified via AT command
      ATCOP would allow the parameters to be a valid value or to be left
      as subscribed (value : 0x0 ).
      Set R97/98 parameters
      Fill parameters as-is from atcop. No further encoding is required
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("AT+CGQREQ specified: filling Minimum qos");

    orig_params_ptr->minimum_qos.reliability_class  =
                                           (uint8)min_qos_r9798->reliability;
    orig_params_ptr->minimum_qos.delay_class        =
                                                 (uint8)min_qos_r9798->delay;
    orig_params_ptr->minimum_qos.precedence_class   =
                                            (uint8)min_qos_r9798->precedence;
    orig_params_ptr->minimum_qos.peak_throughput    =
                                                  (uint8)min_qos_r9798->peak;
    orig_params_ptr->minimum_qos.mean_throughput    =
                                                  (uint8)min_qos_r9798->mean;

    /*-----------------------------------------------------------------------
      R99 parameters Spec mandates that we translate
      R97/98 parameters to corresponding R99 values and fill in if R99
      parameters are not defined
    -----------------------------------------------------------------------*/
    if(!min_qos->valid_flg)
    {
       dsUmtsBearerCntxt_fill_min_qos_r99_params_from_r9798_params(
                                                            min_qos_r9798,
                                                   orig_params_ptr);
    }
  }

  DS_3GPP_MSG0_HIGH("QOS Values req in PDP context");
  DS_3GPP_MSG9_HIGH("delay class:%lx, rel class :%lx, peak :%lx,\
                     precd class:%lx, mean :%lx, traff class:%lx,\
                     del order:%lx, del err sdu:%lx, max sdu :%lx",\
                     orig_params_ptr->qos.delay_class,\
                     orig_params_ptr->qos.reliability_class,\
                     orig_params_ptr->qos.peak_throughput,\
                     orig_params_ptr->qos.precedence_class,\
                     orig_params_ptr->qos.mean_throughput,\
                     orig_params_ptr->qos.traffic_class,\
                     orig_params_ptr->qos.deliver_order,\
                     orig_params_ptr->qos.deliver_err_sdu,\
                     orig_params_ptr->qos.max_sdu_size);

  DS_3GPP_MSG9_HIGH("Max UL rate:%lx, Max DL rate:%lx, Res BER :%lx,\
                     sdu err rat:%lx, txfer delay:%lx, traff prio :%lx,\
                     guar UL rate:%lx, guar DL rate:%lx, Signaling Indication:%1x",\
                     orig_params_ptr->qos.max_bit_rare_uplink,\
                     orig_params_ptr->qos.max_bit_rare_downlink,\
                     orig_params_ptr->qos.residual_ber,\
                     orig_params_ptr->qos.sdu_err_ratio,\
                     orig_params_ptr->qos.tranfer_delay,\
                     orig_params_ptr->qos.traffic_priority,\
                     orig_params_ptr->qos.guarantee_ber_uplink,\
                     orig_params_ptr->qos.guarantee_ber_dwnlink,\
                     orig_params_ptr->qos.sig_ind);

  DS_3GPP_MSG3_HIGH("Source Statistics Desc:%1x, Extended Max DL Rate:%1x,\
                     Extended Gtd DL Rate:%1x",\
                     orig_params_ptr->qos.src_stat_desc,\
                     orig_params_ptr->qos.ext_max_bit_rate_downlink,\
                     orig_params_ptr->qos.ext_gtd_bit_rate_downlink);
#ifdef FEATURE_NAS_REL7
  DS_3GPP_MSG2_HIGH("Extended Max UL Rate:%1x, Extended Gtd UL Rate:%1x",
                     orig_params_ptr->qos.ext_max_bit_rate_uplink,
                     orig_params_ptr->qos.ext_gtd_bit_rate_uplink);
#endif /* FEATURE_NAS_REL7 */

  DS_3GPP_MSG0_HIGH("Minimum QOS Values in PDP context");
  DS_3GPP_MSG9_HIGH("delay class:%lx, rel class :%lx, peak :%lx,\
                     precd class:%lx, mean :%lx, traff class:%lx,\
                     del order  :%lx, del err sdu:%lx, max sdu :%lx",\
                     orig_params_ptr->minimum_qos.delay_class,\
                     orig_params_ptr->minimum_qos.reliability_class,\
                     orig_params_ptr->minimum_qos.peak_throughput,\
                     orig_params_ptr->minimum_qos.precedence_class,\
                     orig_params_ptr->minimum_qos.mean_throughput,\
                     orig_params_ptr->minimum_qos.traffic_class,\
                     orig_params_ptr->minimum_qos.deliver_order,\
                     orig_params_ptr->minimum_qos.deliver_err_sdu,\
                     orig_params_ptr->minimum_qos.max_sdu_size);

  DS_3GPP_MSG9_HIGH("Max UL rate:%lx, Max DL rate:%lx, Res BER :%lx,\
                     sdu err rat:%lx, txfer delay:%lx, traff prio :%lx,\
                     guar UL rate:%lx, guar DL rate:%lx, Signaling Indication:%1x",\
                     orig_params_ptr->minimum_qos.max_bit_rare_uplink,\
                     orig_params_ptr->minimum_qos.max_bit_rare_downlink,\
                     orig_params_ptr->minimum_qos.residual_ber,\
                     orig_params_ptr->minimum_qos.sdu_err_ratio,\
                     orig_params_ptr->minimum_qos.tranfer_delay,\
                     orig_params_ptr->minimum_qos.traffic_priority,\
                     orig_params_ptr->minimum_qos.guarantee_ber_uplink,\
                     orig_params_ptr->minimum_qos.guarantee_ber_dwnlink,\
                     orig_params_ptr->minimum_qos.sig_ind);

  DS_3GPP_MSG3_HIGH("Source Statistics Desc:%1x, Extended Max DL Rate:%1x,Extended Gtd DL Rate:%1x",
                    orig_params_ptr->minimum_qos.src_stat_desc,
                    orig_params_ptr->minimum_qos.ext_max_bit_rate_downlink,
                    orig_params_ptr->minimum_qos.ext_gtd_bit_rate_downlink);
#ifdef FEATURE_NAS_REL7
  DS_3GPP_MSG2_HIGH("Extended Max UL Rate:%1x, Extended Gtd UL Rate:%1x",
                     orig_params_ptr->minimum_qos.ext_max_bit_rate_uplink,
                     orig_params_ptr->minimum_qos.ext_gtd_bit_rate_uplink);
#endif /* FEATURE_NAS_REL7 */
  return DS3G_SUCCESS;
} /*dsUmtsBearerCntxt_fill_origination_qos_params()*/

/*===========================================================================
FUNCTION  DSUMTSBEARERCNTXT_FILL_PCO_PARAMS_FOR_NW_INIT_BEARER

DESCRIPTION
  This function populates the Protocol configuration option(PCO) in activate request.
  It fills the values from bearer context which has incoming request params


PARAMETERS   :
  *orig_params_p            - Ptr to PS call origination parameters.
  *bearer_context_p         - Ptr to the bearer that has to be brough up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_fill_pco_params_for_nw_init_bearer
(
  cm_gw_ps_orig_params_s_type       *orig_params_p,
  ds_bearer_context_s 	            *bearer_context_p
)
{
  /*-------------------------------------------------------------------------
    for NW initiated call, Fill the PCO here
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("Inside fill_pco_params_for_nw_init_bearer:");

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p) || (orig_params_p == NULL) )
  {
    return;
  }
  memset((void*)&(orig_params_p->config_options), 0, sizeof(protocol_cfg_options_T));
  if(dsUmtsBearerCntxt_validatebearer(bearer_context_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ]) &&
     ((ds_umts_bearer_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
       vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ]))->ds_umts_bearer_context_dyn_p->pco.valid)
  {/* PCO is present in incoming request, Fill same in activate request */
  
    /*-------------------------------------------------------------------------
     start off assuming that the field is not valid ( nothing filled in )
    -------------------------------------------------------------------------*/
    memscpy((void*)&(orig_params_p->config_options), 
                    sizeof(protocol_cfg_options_T),
                    (void*)&(((ds_umts_bearer_context_s*)(bearer_context_p->
                    ds_bearer_context_dyn_p->vobj_data_ptr_tbl
                    [DS_3GPP_UMTS_VOBJ]))->ds_umts_bearer_context_dyn_p->pco),
                    sizeof(protocol_cfg_options_T));
    DS_3GPP_MSG0_HIGH("Filled PCO for incoming call");
  }
} /* dsUmtsBearerCntxt_fill_pco_params_for_nw_init_bearer */
/*===========================================================================
FUNCTION  DSUMTSBEARERCNTXT_GETORIGPARAMS

DESCRIPTION
  This function populates the Call Manager API data structure for
  packet call origination. This function is called from 3gpp_hdlr orig call
  handler.


PARAMETERS   :
  *bearer_context_p         - Ptr to the bearer that has to be brough up.
  *orig_params_p            - Ptr to PS call origination parameters.
  *orig_info_p              - Ptr to Origination information (if any)

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, depending upon whether the orig params were successfully filled or
  not.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsUmtsBearerCntxt_getOrigParams
(
  ds_bearer_context_s              *bearer_context_p,\
  ds_3gpp_orig_params_s            *orig_params_p,\
  const ds_3gpp_pdn_call_info_type *orig_info_p
)
{  
  ds_umts_pdp_profile_type*   profile_p;
  ps_phys_link_type           *phys_link;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((bearer_context_p == NULL) ||
     (orig_params_p == NULL) ||
     (orig_info_p == NULL)  ||
     (ds_bearer_cntx_validate_bearer_context(bearer_context_p) == FALSE))
  {
     ASSERT(0);
     return FALSE;
  }

  phys_link = &(bearer_context_p->phys_link);
  /*-------------------------------------------------------------------------
    Get the PDP profile type from the PDN context. This is passed into the
    function to fill up the orig params. Since all the info is filled up from
    profile, there is no need for the orig_info_p. If we do not need this after
    adding support for secondary PDP, we can remove this from the list of
    formal argument to this function.
  -------------------------------------------------------------------------*/
  profile_p = &(((ds_pdn_context_s*)\
	        (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr))->ds_pdn_context_dyn_p->
                pdp_profile);

  if( (bearer_context_p->ds_bearer_context_dyn_p->is_default == TRUE) &&
      (PS_PHYS_LINK_GET_STATE(phys_link) != PHYS_LINK_RESUMING) )
  {
    orig_params_p->orig_params_u.gw_ps_orig_params_ptr->primary_pdp = TRUE;
  }
  else
  {
    orig_params_p->orig_params_u.gw_ps_orig_params_ptr->primary_pdp = FALSE;   
    orig_params_p->orig_params_u.gw_ps_orig_params_ptr->assoc_pri_profile_num=
        (uint16) (((ds_pdn_context_s*)
	  (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr))->ds_pdn_context_dyn_p->pdp_profile_num);    
    orig_params_p->orig_params_u.gw_ps_orig_params_ptr->pdp_group_id =   
        (byte)((ds_pdn_context_s*) (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr))->
      ds_pdn_context_dyn_p->pdp_group_id;                                    
    
  }
  /*-------------------------------------------------------------------------
    To Do: Test for primary also(MT primary call)?check spec for primary. 
    For MT primary call also we should be filling PCO from incoming request?
    (bearer_context_p->is_default == FALSE ) 
  -------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_ANSWER)
  {
    dsUmtsBearerCntxt_fill_pco_params_for_nw_init_bearer(orig_params_p->
                                      orig_params_u.gw_ps_orig_params_ptr,
                                      bearer_context_p);
  }
  /*-------------------------------------------------------------------------
    Fill the QOS params based on PDP context id
  -------------------------------------------------------------------------*/
  return dsUmtsBearerCntxt_fill_origination_qos_params(bearer_context_p,
                                                profile_p,
                                                orig_params_p);  
  
} /* dsUmtsBearerCntxt_getOrigParams() */


/*===========================================================================
FUNCTION  DSUMTSBEARERCNTXT_GETANSWERPARAMS

DESCRIPTION
  This function populates the Call Manager answer params data structure for
  packet call origination. This function simply calls the orig Params
  using the function table.


PARAMETERS   :
  *bearer_context_p         - Ptr to the bearer that has to be brough up.
  *answer_params_p          - Ptr to PS call answer parameters.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE depending on whether the answer params were
  filled up successfully or not.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsBearerCntxt_getAnswerParams
(
  ds_bearer_context_s  *bearer_context_p,
  cm_ans_params_s_type *answer_params_p
)
{
  /*---------------------------------------------------------------------------
   * we are gonna reuse the getOrigParams function to fill in the answer params
   * Hence we are gonna declare a struct of type ds_3gpp_orig_params_s.
  ---------------------------------------------------------------------------*/
  ds_3gpp_orig_params_s   orig_params;

  /*---------------------------------------------------------------------------
   * Since we are reusing the fn to fill in orig params, we need this structure
   * to be passed in. This however plays no role even in the getOrigParams. We 
   * can knock this off after review with the team.
  ---------------------------------------------------------------------------*/
  ds_3gpp_pdn_call_info_type orig_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
   * Now we are gonna point the gw_ps_params structure inside 
   * ds_3gpp_orig_params_s to the answer params that is an input parameter 
   * to this function.
  ---------------------------------------------------------------------------*/
  orig_params.orig_params_u.gw_ps_orig_params_ptr =\
	 &(answer_params_p->ans_params.gw_ps_ans);
  return (dsUmtsBearerCntxt_getOrigParams(bearer_context_p,
                 	                  &orig_params,\
					  &orig_info));
}/* dsUmtsBearerCntxt_getAnswerParams() */

/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_RabReEstabIndHdlr

DESCRIPTION
  This function is used to handle rab restestablish indication from CM.
  It invokes the approp function registered (based upon the mode).

PARAMETERS   :
  *bearer_context_p         - Ptr to the bearer that has to be brought up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsUmtsBearerCntxt_RabReEstabIndHdlr
(
  ds_bearer_context_s* bearer_context_p,
  cm_call_mode_info_s_type  *mode_info_ptr  
)
{
  boolean rval = DS3G_FAILURE;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  DS_UMTS_BEARER_CTXT_VFR_CALL_PER_BEARER
  (      
    rval,
    rab_re_estab_ind,
    bearer_context_p,
    mode_info_ptr    
  ); 
  return rval;

} /*dsUmtsBearerCntxt_RabReEstabIndHdlr()*/

/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_ModePrefChgHdlr

DESCRIPTION
  This function is used to handle rab restestablish indication from CM.
  It invokes the approp function registered (based upon the mode).

PARAMETERS   :
  *bearer_context_p         - Ptr to the bearer that has to be brought up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntxt_ModePrefChgHdlr
(
  ds_bearer_context_s* bearer_context_p
)
{
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
  (      
    mode_pref_change,
    bearer_context_p  
  ); 

  return;
} /*dsUmtsBearerCntxt_ModePrefChgHdlr()*/

/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_suspendIndHdlr

DESCRIPTION
  This function is used to handle suspend indication from CM.  

PARAMETERS   :
  *bearer_context_p   - Ptr to the bearer that has to be brought up.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntxt_suspendIndHdlr
(
  ds_bearer_context_s        *bearer_context_p
)
{


  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
  (    
    suspend_ind,
    bearer_context_p   
  );  
  return;
} /*dsUmtsBearerCntxt_suspendIndHdlr()*/

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_emptywatermark

DESCRIPTION
  This is the function flushes the uplink WM
      
PARAMETERS
  
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  
  
===========================================================================*/
LOCAL void dsUmtsBearerCntxt_emptywatermark
(
  ds_bearer_context_s*        bearer_context_p
)
{
  DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
  (
    empty_watermark,
    bearer_context_p
  );
} /* dsUmtsBearerCntxt_emptywatermark */

/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_resumeIndHdlr

DESCRIPTION
  This function is used to handle resume indication from CM.
  It invokes the approp function registered (based upon the mode).

PARAMETERS     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntxt_resumeIndHdlr
(
  ds_bearer_context_s*        bearer_context_p
)
{
  dsUmtsBearerCntxt_performResume(bearer_context_p);

} /*dsUmtsBearerCntxt_resumeIndHdlr()*/

/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_redialHdlr

DESCRIPTION
  This function is used to redial on all W/G/T
 
PARAMETERS     
  bearer_context_p 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL boolean dsUmtsBearerCntxt_redialHdlr
(
  ds_bearer_context_s*        bearer_context_p
)
{
  ds_pdn_context_s*         pdn_cntx_p = NULL;
  void                     *info_ptr = NULL;
  boolean                   ret_val = FALSE;
 /*-----------------------------------------------------------------------*/

  if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return ret_val;
  }

  pdn_cntx_p = (ds_pdn_context_s*)bearer_context_p->
                                    ds_bearer_context_dyn_p->pdn_context_ptr;

  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return ret_val;
  }

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return ret_val;
  }

  if (pdn_cntx_p->ds_pdn_context_dyn_p->info_null)
  {
    info_ptr = NULL;
  }
  else
  {
    info_ptr = &(pdn_cntx_p->ds_pdn_context_dyn_p->info);
  }

  ret_val = TRUE;
  ds_3gpp_bearer_cntxt_invoke_bearer_bringup((uint32)
                (bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr),
                info_ptr);

  return ret_val;

} /*dsUmtsBearerCntxt_redialHdlr()*/

#if defined(FEATURE_DATA_LTE) && (defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA))
/*===========================================================================
FUNCTION  ds_umts_bearer_cntxt_transfer_setup_hdlr

DESCRIPTION
  This function is used to handle cntxt_transfer indication from CM.
  The setup hdlr is called in the TARGET RAT during L-GW transition.
  It invokes the approp function registered (based upon the mode).

PARAMETERS     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_umts_bearer_cntxt_transfer_setup_hdlr
(
  ds_bearer_context_s*        bearer_context_p
)
{
        
#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_context_s * eps_bearer_context_p = NULL;
#endif /* FEATURE_DATA_LTE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG3_HIGH("In ds_umts_bearer_cntxt_transfer_setup_hdlr for bearer_inst:%d", 
                    (uint32)(bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr),0,0);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  if(!dsUmtsBearerCntxt_validatebearer(&umts_bearer_context_tbl[(uint32)bearer_context_p->
     ds_bearer_context_dyn_p->client_data_ptr]))
   {
    DS_3GPP_MSG0_ERROR("Umts Bearer context passed is Invalid");
    return;
   }

  /*---------------------------------------------------------------------
    Associate the mode specific blob that is going to be populated to 
    the bearer context pointer.
  ---------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ] =\
    (void*)&umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->\
      client_data_ptr];      

  /*------------------------------------------------------------------------- 
    Update the nsapi for this bearer
  -------------------------------------------------------------------------*/
  eps_bearer_context_p = (ds_eps_bearer_context_s *)\
                 (bearer_context_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl
                  [DS_3GPP_EPS_VOBJ]);

  if(ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context_p) == FALSE)
  {
    ASSERT(0);
    return;
  }

  if( (bearer_context_p->ds_bearer_context_dyn_p->nsapi == 0) &&
      (eps_bearer_context_p->ds_eps_bearer_context_dyn_p->eps_bearer_id != 0))
  {    
    bearer_context_p->ds_bearer_context_dyn_p->nsapi = eps_bearer_context_p->
      ds_eps_bearer_context_dyn_p->eps_bearer_id;      
    DS_3GPP_MSG3_HIGH("Using nsapi %d for BearerInst:%d",
        bearer_context_p->ds_bearer_context_dyn_p->nsapi,(uint32)bearer_context_p
                      ->ds_bearer_context_dyn_p->client_data_ptr,0);
  }
  
  ds_umts_bearer_flow_manager_update_flow_on_resume(bearer_context_p);

  DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
  (      
    cntxt_transfer_ind_setup,
    bearer_context_p
  );    
  
  return;
} /*ds_umts_bearer_cntxt_transfer_setup_hdlr()*/

/*===========================================================================
FUNCTION  ds_umts_bearer_cntxt_transfer_release_hdlr

DESCRIPTION
  This function is used to handle cntxt_transfer indication from CM.
  The setup hdlr is called in the TARGET RAT during L-GW transition.
  It invokes the approp function registered (based upon the mode).

PARAMETERS     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_umts_bearer_cntxt_transfer_release_hdlr
(
  ds_bearer_context_s*        bearer_context_p
)
{        

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
  (      
    cntxt_transfer_ind_release,
    bearer_context_p
  );    

  return;
} /*ds_umts_bearer_cntxt_transfer_release_hdlr()*/

#endif /* defined(FEATURE_DATA_LTE) && (FEATURE_GSM_GPRS || FEATURE_DATA_WCDMA_PS || FEATURE_TDSCDMA) */

/*===========================================================================
FUNCTION  DSUMTSBEARERCNTXT_REORIG

DESCRIPTION
  This function is used to reoriginate from dormancy. It invokes the approp
  function registered (based upon the mode).

PARAMETERS   :
  *bearer_context_p         - Ptr to the bearer that has to be brought up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntx_reorig
(
  ds_bearer_context_s* bearer_context_p ,
  dsm_item_type*       reorig_pkt
)
{
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  DS_UMTS_BEARER_CTXT_VF_CALL_PER_BEARER
  (
    dormancyReorig, 
    bearer_context_p,
    reorig_pkt
  );
  return;
} /*dsUmtsBearerCntx_reorig()*/

/*===========================================================================
FUNCTION  DSUMTSBEARERCNTXT_GETENDPARAMS

DESCRIPTION
  This function is used to populate the end params structure passed to us by
  CM. This function is called as a part of the call ended handler implemented
  in the 3gpp_hdlr.

PARAMETERS   :
  IN
  *bearer_context_p         - Ptr to the bearer that has to be worked with.
  
  OUT
  *end_params_p             - Ptr to the end params structure passed to us.
  *ds3gi_capp_info_p        - Ptr to the call info structure.
  *delay_disconnect_p       - Ptr to the boolean that lets the caller know if
                              there is a need for a delayed disconnect.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_getEndParams
(
  ds_bearer_context_s              *bearer_context_p,
  cm_end_params_s_type             *end_params,
  ds_3gpp_pdn_call_info_type       *ds3gi_call_info,
  boolean                          *delay_disconnect
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ds3gi_call_info != NULL);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    If the end handler is called for a MT request, MT reject needs to be
    sent. Fill in the cause for MT reject and send a call_end to CM.
  -------------------------------------------------------------------------*/
  if(ds3gi_call_info->mt_flag == TRUE)
  {
    *delay_disconnect = FALSE;
    ds3gi_call_info->mt_flag = FALSE;
    end_params->call_id   = bearer_context_p->ds_bearer_context_dyn_p->cm_call_id;
    end_params->info_type = CM_CALL_MODE_INFO_GW_PS;
    end_params->end_params.gw_ps_end.cause = SM_ACTIVATION_REJECTED_UNSPECIFIED;
    return;
  }

  DS_3GPP_MSG3_HIGH("User End Hdlr: call id:%d BearerInst:%d",
		  bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,\
		  (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,0);

  *delay_disconnect = FALSE;
  end_params->call_id   = bearer_context_p->ds_bearer_context_dyn_p->cm_call_id;
  end_params->info_type = CM_CALL_MODE_INFO_GW_PS;
  end_params->end_params.gw_ps_end.cause = SM_REGULAR_DEACTIVATION;

  /*-------------------------------------------------------------------------
    If call is Primary bring down all activated contexts associated with
    primary otherwise just tear down this phys link and not the iface.
  -------------------------------------------------------------------------*/
  if (bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {
    end_params->end_params.gw_ps_end.tear_down_ind.valid = TRUE;
    end_params->end_params.gw_ps_end.tear_down_ind.ti_flag = TRUE;
  }
  else
  {
    end_params->end_params.gw_ps_end.tear_down_ind.valid = TRUE;
    end_params->end_params.gw_ps_end.tear_down_ind.ti_flag = FALSE;
  }

}/*dsUmtsBearerCntxt_getEndParams()*/



/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_BRINGUP

DESCRIPTION
  This function is used to bring up a call. This is called from the mode 
  agnostic bearer context in response to the phys_link_up command.

PARAMETERS
  IN    
    info_p           - Used to extract the dial string.
    call_info_p      - call info such as call_instance(i.e. bearer_inst)
    useDefaultDialStr- This is the boolean which states if we should use the
                       default dial str or the one from the info_p.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS (TRUE) on Successful call initiation with CM
  DS3G_FAILURE (FALSE) on any failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsUmtsBearerCntxt_bringUp(void                 *info_p,
                                  ds_3gpp_pdn_call_info_type *call_info_p,
                                  boolean                    useDefaultDialStr)
{  
  dsat_dial_val_e_type        dial_str_digits_type = DSAT_DIAL_MAX;
  byte                        dsatumts_pkt_orig_str[5];
  sys_sys_mode_e_type         network_mode;/* System camped on. UMTS/GPRS  */
  boolean                     error_flg = FALSE; 
  network_params_info_type    *ppp_info_ptr;
  ds_pdn_context_s*           pdn_cntx_p = NULL;
  ds_bearer_context_s         *bearer_context_p;
  uint32                      bearer_instance;
  sys_modem_as_id_e_type      subs_id = SYS_MODEM_AS_ID_NONE ;
  byte ds_3gpp_default_dial_str[NV_PKT_DIAL_STRING_SIZE];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  DS_3GPP_MSG0_HIGH("DSUMTSBEARERCNTXT_BRING_UP");  

  bearer_instance = call_info_p->call_instance;
  ASSERT(bearer_instance < DS_3GPP_MAX_BEARER_CONTEXT);
  
  /*---------------------------------------------------------------------------
    Get the bearer context pointer
  ---------------------------------------------------------------------------*/
  bearer_context_p = ds_bearer_cntxt_get_instance_ptr((uint8)bearer_instance);

  if(ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    /* Get the PDN context */
    pdn_cntx_p = (ds_pdn_context_s*)
                     (bearer_context_p->ds_bearer_context_dyn_p->
                      pdn_context_ptr);
  }

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }   

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }

    /*-------------------------------------------------------------------------
       Casting PDP-IP info ptr to extract dial string
     -------------------------------------------------------------------------*/ 

    ppp_info_ptr = (network_params_info_type*)info_p;

    memset(ds_3gpp_default_dial_str, 0, sizeof(ds_3gpp_default_dial_str));

    if (ds_3gpp_cfg_get_subsid( DS_3GPP_DATA_PKT_ORIG_STR,
                      (uint8 *)&(ds_3gpp_default_dial_str),
                       sizeof(ds_3gpp_default_dial_str),
                     subs_id) > 0)
    {
      DS_3GPP_MSG0_HIGH("ds_3gpp_default_dial_str set in EFS");
    }
    else
    {
      (void)strlcpy( (char *)ds_3gpp_default_dial_str,
                       "*98#",
                       NV_PKT_DIAL_STRING_SIZE );
    }
    
    ds_3gpp_default_dial_str[NV_PKT_DIAL_STRING_SIZE - 1] = 0; 


    /*-------------------------------------------------------------------------
      PDN context will find out if the call is orig from sock and populate
      the useDefaultDialStr flag
    -------------------------------------------------------------------------*/
    if ( useDefaultDialStr )
    {
      memscpy( dsatumts_pkt_orig_str, sizeof(dsatumts_pkt_orig_str),
               ds_3gpp_default_dial_str, NV_PKT_DIAL_STRING_SIZE - 1 );
    }
    else
    {
      memscpy( dsatumts_pkt_orig_str, sizeof(dsatumts_pkt_orig_str),
               ppp_info_ptr->dial_string, NV_PKT_DIAL_STRING_SIZE - 1 );
      
    }
    dsatumts_pkt_orig_str[NV_PKT_DIAL_STRING_SIZE - 1] = 0;

    MSG_SPRINTF_1(MSG_SSID_DS, MSG_LEGACY_HIGH, "dsBearerCntxt_bringUp: dial_str %s", dsatumts_pkt_orig_str);    

    /*-------------------------------------------------------------------------
      Get the current network mode.
    -------------------------------------------------------------------------*/
    network_mode = ds_3gpp_get_apn_current_mode (pdn_cntx_p);

    if ( ( SYS_SYS_MODE_GSM != network_mode) && 
         ( SYS_SYS_MODE_WCDMA != network_mode)
  #ifdef FEATURE_TDSCDMA
         &&
         ( SYS_SYS_MODE_TDS != network_mode)
  #endif /*FEATURE_TDSCDMA*/
       )
    {
      DS_3GPP_MSG1_HIGH("Invalid network mode %d",(int)network_mode); 
      return FALSE; 
    } 
  #ifdef FEATURE_DATA_TE_MT_PDP
    if (call_info_p->mt_flag == TRUE)
    {
      DS_3GPP_MSG1_HIGH("Is a mt call pdn_cntx_p->state %d",
                         pdn_cntx_p->ds_pdn_context_dyn_p->state);
      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_ANSWER;
    }
  #endif /* FEATURE_DATA_TE_MT_PDP */
    if( pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_ANSWER)
    {
      /*---------------------------------------------------------------------
        Answer a Data Call
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH("Answer a Packet data call. Mode:%d Bearer Inst:%d",
                network_mode, call_info_p->call_instance);

      if (ds3g_msh_answer_call_ex(bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
                                  pdn_cntx_p->ds_pdn_context_dyn_p->subs_id
                                  ) == DS3G_FAILURE)
      {
        DS_3GPP_MSG2_ERROR("Could not Answer pkt call Mode: %d Bearer Inst:%d",\
             network_mode, call_info_p->call_instance);
        error_flg = TRUE;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
      Originate a Data Call
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH("Originate a Packet data call. Mode:%d Bearer Inst:%d",
              network_mode, call_info_p->call_instance);
    
      /*-------------------------------------------------------------------
        Originate a WCDMA Data Call. 
        Process the dial string for the call for validity else prevent call
        origination to go through if dial string is not valid.
      -------------------------------------------------------------------*/
      dial_str_digits_type = dsat_proc_dial_str(dsatumts_pkt_orig_str,   
                                                dsatumts_pkt_orig_str);
      
      if ( !( ( dial_str_digits_type == DSAT_DIAL_DIGIT ) ||
              ( dial_str_digits_type == DSAT_DIAL_ASCII ) ) )  
      {  
        error_flg = TRUE;
        DS_3GPP_MSG1_ERROR("PHYS_LINK_UP_CMD_HDLR: ABORTING UMTS Pkt Call, INVALID dial string, call_instance:%d",
                  call_info_p->call_instance);
      }
      
      if(error_flg!= TRUE)
      {
        if (ds3g_msh_initiate_call_ex(DS_CALL_TYPE_PKT,
                                   network_mode,
                                   (const byte *)(dsatumts_pkt_orig_str),
                                   (boolean)dial_str_digits_type,
                                   call_info_p,
                                   subs_id
                                  ) == DS3G_FAILURE)
        {
          DS_3GPP_MSG2_ERROR("Could not Originate pkt call Mode: %d Bearer Inst:%d",\
                    network_mode, call_info_p->call_instance);
          error_flg = TRUE;
        }
      }
    }
    if (error_flg)
    {

      DS_3GPP_MSG1_ERROR("Error bringing up Bearer Context for Call Inst %d",
                call_info_p->call_instance);
      
    }
    return !(error_flg);
} /* dsUMTSBearerCntxt_bringUp() */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_TEAR_DOWN_PDN_CONNECTION

DESCRIPTION
  This function is used to tear down a PDN connection.  

PARAMETERS
  bearer_context_p  - pointer to the bearer context that is to be used
                      to end the call
  call_info_p       - call info structure
  info_ptr          - void pointer

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS is all is successful, DS3G_FAILURE otherwise. 
  When this function returns failure, the bearer and the PDN contexts are
  cleaned up from the calling function. 
  
SIDE EFFECTS
  None.

===========================================================================*/

LOCAL boolean dsUmtsBearerCntx_tear_down_pdn_connection
(
  ds_bearer_context_s        *bearer_context_p,
  ds_3gpp_pdn_call_info_type *call_info_p,
  void                       *info_ptr
)
{  
  boolean                   retVal = DS3G_FAILURE;
  sys_modem_as_id_e_type    subs_id; 
  boolean                   local_disconnect = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If PDN connection is active then tear down else do nothing.
  -------------------------------------------------------------------------*/
  switch(bearer_context_p->ds_bearer_context_dyn_p->state)
  {
    case DS_BEARER_CONTEXT_STATE_COMING_UP:  
    case DS_BEARER_CONTEXT_STATE_PEND_RAB_REG:
    case DS_BEARER_CONTEXT_STATE_UP:
    case DS_BEARER_CONTEXT_STATE_UP_DORMANT:
    case DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB:
    case DS_BEARER_CONTEXT_STATE_PEND_REDIAL:
    case DS_BEARER_CONTEXT_STATE_ANSWER:
    {

      /*-------------------------------------------------------------------------
        If ePDG-3G IP Continuity is enabled, check for the local abort flag 
        set by EPC. If flag is set, locally disconnect the PDP within the UE.       
      -------------------------------------------------------------------------*/

      if (ds3g_is_epdg_3g_ip_continuity_enabled
            ((ps_sys_subscription_enum_type)
               ds3gsubsmgr_subs_id_cm_to_ds(subs_id) != 0) &&
          info_ptr != NULL && (info_ptr == (void *)TRUE) )
      {
        DS_3GPP_MSG0_HIGH("Send local disconnect");
        local_disconnect = TRUE;
      }

      /*-------------------------------------------------------------------------
        If call id is invalid, call origination did not reach CM. Do local 
        cleanup. 
      -------------------------------------------------------------------------*/
      if( bearer_context_p->ds_bearer_context_dyn_p->cm_call_id == CM_CALL_ID_INVALID )
      {
        DS_3GPP_MSG1_HIGH("CM Call id invalid for bearer instance %d", \
    		    (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
        retVal = DS3G_FAILURE;
      }
      /*-------------------------------------------------------------------------
       Initiate Call End.
      -------------------------------------------------------------------------*/
      else if (ds3g_msh_hangup_call_ex2
                 (bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
                    call_info_p, subs_id, local_disconnect) == DS3G_FAILURE)
      {
        /*-----------------------------------------------------------------------
          The call has already ended. Do local cleanup
        -----------------------------------------------------------------------*/        
        retVal = DS3G_FAILURE;
      }
      else
      {
        /*---------------------------------------------------------------------
          Update the state so we do not send multiple disconnect req to CM. 
        ---------------------------------------------------------------------*/
        bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_GOING_DOWN;      
        retVal = DS3G_SUCCESS;
      }
    }
    break;

    default:
      DS_3GPP_MSG1_HIGH("Ignoring tear down Pdn Connection. Bearer State: %d",
                 bearer_context_p->ds_bearer_context_dyn_p->state);
      retVal = DS3G_SUCCESS;
      break;

  }
  return retVal;
} /* dsUmtsBearerCntx_tear_down_pdn_connection */

/*==========================================================================
FUNCTION  dsUmtsBearerCntxt_bearerTechTypeHdlr

DESCRIPTION
  This function is used to handle bearer tech type ioctl.
  It invokes the approp function registered (based upon the mode).

PARAMETERS
  *bearer_tech_ptr - Bearer_tech_ptr
  *ps_errno        - ps error code
   subs_id         - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  -1 on error
   0 on success
 
SIDE EFFECTS
  None.
===========================================================================*/
int dsUmtsBearerCntxt_bearerTechTypeHdlr
(
  ps_iface_bearer_technology_type     *bearer_tech_ptr,
  sint15                              *ps_errno,
  sys_modem_as_id_e_type               subs_id
)
{
  int rval = -1;

  DS_UMTS_BEARER_CTXT_VFR_CALL
  (
    rval,
    getBearerTechType,
    subs_id,
    bearer_tech_ptr,
    ps_errno,
    subs_id
  );

  return rval;
} /* dsUmtsBearerCntxt_bearerTechTypeHdlr */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*==========================================================================
FUNCTION  dsUmtsBearerCntxt_bearerTechTypeHdlr

DESCRIPTION
  This function is used to handle bearer tech type ioctl.
  It invokes the approp function registered (based upon the mode).

PARAMETERS
  *bearer_tech_ptr - Bearer_tech_ptr
  *ps_errno        - ps error code
   subs_id         - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  -1 on error
   0 on success
 
SIDE EFFECTS
  None.
===========================================================================*/
int dsUmtsBearerCntxt_bearerTechTypeHdlrEx
(
  ds_sys_system_status_info_type     *bearer_tech_ex_ptr,
  sys_modem_as_id_e_type              subs_id
)
{
  int rval = -1;

  DS_UMTS_BEARER_CTXT_VFR_CALL
  (
    rval,
    getBearerTechTypeEx,
    subs_id,
    bearer_tech_ex_ptr,
    subs_id
  );

  return rval;
} /* dsUmtsBearerCntxt_bearerTechTypeHdlr */

/*==========================================================================
FUNCTION  dsUmtsBearerCntxt_getSoMask

DESCRIPTION
  This function is used to map capability to SO mask info. 

PARAMETERS
   subs_id         - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  SO Mask
 
SIDE EFFECTS
  None.
===========================================================================*/
uint64 dsUmtsBearerCntxt_getSoMask
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint64 so_mask = DS_SYS_SO_EX_UNSPECIFIED;
  /*-------------------------------------------------------------------------*/

  DS_UMTS_BEARER_CTXT_VFR_CALL
  (
    so_mask,
    getSoMask,
    subs_id,
    subs_id
  );

  return so_mask;
} /* dsUmtsBearerCntxt_getSoMask */
#endif /* FEATURE_DATA_WLAN_MAPCON */
/*==========================================================================
FUNCTION  dsUmtsBearerCntxt_bearerRateHdlr

DESCRIPTION
  This function is used to handle bearer rate ioctl query from CM.
  It invokes the approp function registered (based upon the mode).

PARAMETERS   :
   call_inst  - index into call table
  *argval_ptr - return value
  *ps_errno   - ps error code
   subs_id    - Subscription Id

  DEPENDENCIES
  None.

RETURN VALUE
  -1 on error
   0 on success
 
SIDE EFFECTS
  None.

===========================================================================*/
int dsUmtsBearerCntxt_bearerRateHdlr
(
  uint32                   call_inst,
  void                    *argval_ptr,
  sint15                  *ps_errno,
  sys_modem_as_id_e_type   subs_id
)
{
  int rval = -1;

  DS_UMTS_BEARER_CTXT_VFR_CALL
  (
    rval,
    getBearerRate,
    subs_id,
    call_inst,
    argval_ptr,
    ps_errno,
    subs_id
  );
  return rval;
} /* dsUmtsBearerCntxt_bearerRateHdlr */

/*===========================================================================
FUNCTION dsUmtsBearerCntxtGetPsRatMaskForHsStatus

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed HS status.

PARAMETERS
  hs_status  - the HS status
  subs_id    - Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
  
===========================================================================*/
uint32 dsUmtsBearerCntxtGetPsRatMaskForHsStatus
(
  sys_hs_ind_e_type       hs_status,
  sys_modem_as_id_e_type  subs_id
)
{  
  int rval = 0;

  DS_UMTS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    getratMaskforHs,
    subs_id,
    hs_status,
    subs_id
  );

  return rval;
} /* dsUmtsBearerCntxtGetPsRatMaskForHsStatus */

/*===========================================================================
FUNCTION dsUmtsBearerCntxtGetCellSrvInd

DESCRIPTION
  This function returns the cached cell srv ind.

PARAMETERS
  subs_id    - Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE
  cell srv ind 

SIDE EFFECTS
  None.
  
===========================================================================*/
cm_cell_srv_ind_s_type dsUmtsBearerCntxtGetCellSrvInd
(
  sys_modem_as_id_e_type  subs_id
)
{
  cm_cell_srv_ind_s_type rval = {-1,-1,-1,-1};

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return rval;
  }

  return ds_3gpp_cell_srv_ind[ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id)];
} /* dsUmtsBearerCntxtGetCellSrvInd() */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_cell_srv_ind

DESCRIPTION
  This function sets the cached cell srv ind.

PARAMETERS
  cell_srv_ind - passed cell srv ind
  subs_id      - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxtSetCellSrvInd
(
  cm_cell_srv_ind_s_type  cell_srv_ind,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_cell_srv_ind[ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id)] = cell_srv_ind;
} /* dsUmtsBearerCntxtSetCellSrvInd */

/*===========================================================================
FUNCTION dsUmtsBearerCntxtGprsGetratMaskforEgprsSupport

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed egprs support.

PARAMETERS
  egprs_support - egprs_supported or not
  subs_id       - Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dsUmtsBearerCntxtGprsGetratMaskforEgprsSupport
( 
  sys_egprs_support_e_type  egprs_support,
  sys_modem_as_id_e_type    subs_id
)
{  
  int rval = 0;

  DS_UMTS_BEARER_CTXT_VFR_CALL
  (
    rval,
    getratMaskforEgprsSupport,
    subs_id,
    egprs_support
  );

  return rval;
} /* dsUmtsBearerCntxtGprsGetratMaskforEgprsSupport */

/*===========================================================================
FUNCTION dsUmtsBearerCntxt-refresh_actual_throughput

DESCRIPTION
  This function updates the actual throughput for all active bearers on the
  current RAT

PARAMETERS
  ds_bearer_context_s  - Bearer context pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean dsUmtsBearerCntxt_refresh_actual_throughput
(
   ds_bearer_context_s    *bearer_context_p,
   sys_modem_as_id_e_type  cm_subs_id,
   uint32                  sampling_interval
)
{
  boolean ret_val   = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_BEARER_CTXT_VFR_CALL
  (
    ret_val, 
    refresh_actual_throughput, 
    cm_subs_id,
    bearer_context_p,
    cm_subs_id,
    sampling_interval
  );
  
  return ret_val;
} /* dsUmtsBearerCntxt_refresh_actual_throughput */

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_INIT_BEARER_THROUGHPUT_STATS

DESCRIPTION
  This function Initializes the bearer throughput stats for the current
  UMTS bearer

PARAMETERS
  ds_bearer_context_s  - Bearer context pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
  
===========================================================================*/
void dsUmtsBearerCntxt_init_bearer_throughput_stats
(
   ds_bearer_context_s *bearer_context_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
  (
    init_bearer_throughput_stats, 
    bearer_context_p
  );
}
/*===========================================================================
FUNCTION dsUmtsBearerCntxt_purgecmdcbdhlr

DESCRIPTION
  This function used to call technology specific purge cmd cb hdlr.

PARAMETERS
  ds3gpp_purge_cnf_ev_type *ds3gpp_purge_cnf_ev

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.
  
===========================================================================*/
#ifdef FEATURE_WCDMA
void
dsUmtsBearerCntxt_purgecmdcbdhlr
( 
  ds_bearer_context_purge_cnf_ev_type *ds3gpp_purge_cnf_ev
)
{ 
  ds_bearer_context_s    *bearer_context_p = NULL; 
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  /*--------------------------------------------------------------------------*/

  if (ds3gpp_purge_cnf_ev == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds3gpp_purge_cnf_ev is NULL");
    return;
  }

  bearer_context_p = ds3gpp_purge_cnf_ev->bearer_context_p;

  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  DS_UMTS_BEARER_CTXT_VF_CALL
  (
    purgecmdcbhdr, 
    subs_id,
    ds3gpp_purge_cnf_ev
  );
} /* dsUmtsBearerCntxt_purgecmdcbdhlr */

#endif /* FEATURE_WCDMA */
/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_INIT

DESCRIPTION
  This function is used to init the UMTS Bearer context. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_init(void)
{ 
  ds_bearer_context_ftbl_s    *umts_bearer_context_ftbl_p;
  ds3gsubsmgr_subs_id_e_type   subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First fill the WCDMA virtual function table with the appropriate handler
    function pointers. Then fill GPRS/TDSCDMA Vftbl with the same handlers.
    For handlers such as Activate and Deactivate, the respective mode specific
    handlers are called again from UMTS bearer context handlers.

    Orig Param and End Param handlers in UMTS Bearer context file finish the
    operation themselves as there is no mode specific action to be done. Hence
    there is no calling into the mode specific handler functions for these two.
  -------------------------------------------------------------------------*/
  
for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_MAX;
      subs_indx++)
  {
    ds_3gpp_cell_srv_ind[subs_indx].dtm_supp = SYS_DTM_SUPPORT_NOT_AVAIL;
    ds_3gpp_cell_srv_ind[subs_indx].egprs_supp = SYS_EGPRS_SUPPORT_NOT_AVAIL;
    ds_3gpp_cell_srv_ind[subs_indx].hs_ind =
      SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
    ds_3gpp_cell_srv_ind[subs_indx].hs_call_status =
      SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
  }

 #ifdef FEATURE_WCDMA

  umts_bearer_context_ftbl_p = &(ds_bearer_context_ftbl[DS_3GPPI_SYS_MODE_WCDMA]);

  memset(
      (void*)umts_bearer_context_ftbl_p,
      0x0,
      (sizeof(ds_bearer_context_ftbl_s))
      );

  //umts_bearer_context_ftbl_p->dormancyReorig_f = dsUmtsBearerCntx_reorig ;
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, dormancyReorig, \
                        dsUmtsBearerCntx_reorig);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, activateCnf, \
                        dsUmtsBearerCntxt_activateCnf);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, getOrigParams, \
                        dsUmtsBearerCntxt_getOrigParams);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, bring_up, \
                        dsUmtsBearerCntxt_bringUp);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, getEndParams, \
                        dsUmtsBearerCntxt_getEndParams) ;  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, deactivateCnf, \
                        dsUmtsBearerCntxt_deactivateCnf) ;
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, tear_down, \
                        dsUmtsBearerCntx_tear_down_pdn_connection);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, rab_re_estab_ind, \
                        dsUmtsBearerCntxt_RabReEstabIndHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, suspend_ind, \
                        dsUmtsBearerCntxt_suspendIndHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, resume_ind, \
                        dsUmtsBearerCntxt_resumeIndHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, get_bearer_tech_type, \
                        dsUmtsBearerCntxt_bearerTechTypeHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, getCellSrvInd, \
                        dsUmtsBearerCntxtGetCellSrvInd);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, getratMaskforHs, \
                        dsUmtsBearerCntxtGetPsRatMaskForHsStatus);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, setCellSrvInd, \
                        dsUmtsBearerCntxtSetCellSrvInd);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, refresh_actual_throughput, \
                        dsUmtsBearerCntxt_refresh_actual_throughput);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, init_bearer_throughput_stats, \
                        dsUmtsBearerCntxt_init_bearer_throughput_stats);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, redial,\
                        dsUmtsBearerCntxt_redialHdlr);  
#ifdef FEATURE_DATA_WLAN_MAPCON
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, get_bearer_tech_type_ex, \
                        dsUmtsBearerCntxt_bearerTechTypeHdlrEx);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, getSoMask, \
                        dsUmtsBearerCntxt_getSoMask);
#endif /* FEATURE_DATA_WLAN_MAPCON */
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, get_bearer_rate, \
                        dsUmtsBearerCntxt_bearerRateHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, go_null, \
                        dsUmtsBearerCntxt_go_null_hdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, empty_watermark, \
                        dsUmtsBearerCntxt_emptywatermark);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, free_umts_bearer, \
                        dsUmtsBearerCntxt_free_umts_bearer);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, allocate_umts_bearer, \
                        dsUmtsBearerCntxt_allocate_umts_bearer);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, examine_if_throt_needed, \
                        dsUmtsBearerCntxt_examine_if_throt_needed);

#if defined(FEATURE_DATA_LTE) && defined(FEATURE_DATA_WCDMA_PS)
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, cntxt_transfer_ind_setup, \
                                    ds_umts_bearer_cntxt_transfer_setup_hdlr);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, cntxt_transfer_ind_release, \
                                    ds_umts_bearer_cntxt_transfer_release_hdlr);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, resume_validate, \
                        dsUmtsBearerCntxt_performResumeValidation);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, purgecmdcbhdr, \
                        dsUmtsBearerCntxt_purgecmdcbdhlr);
  
#endif /* (FEATURE_DATA_LTE) && (FEATURE_DATA_WCDMA_PS)*/
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, complogfetchiface, \
			ds_umts_bearer_cntxt_comp_log_fetch_iface);

  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, mode_pref_change, \
                        dsUmtsBearerCntxt_ModePrefChgHdlr);


#ifdef FEATURE_QXDM_WM_TRACING
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, report_wm_stats, \
                        dsUmtsBearerCntxt_reportWmStats);
#endif /* FEATURE_QXDM_WM_TRACING */
#endif /* FEATURE_WCDMA */


#ifdef FEATURE_GSM_GPRS
  /*-------------------------------------------------------------------------
    Filling the GPRS virtual function table now. The handlers are the same
  -------------------------------------------------------------------------*/
  umts_bearer_context_ftbl_p = &(ds_bearer_context_ftbl[DS_3GPPI_SYS_MODE_GSM]);

  memset(
      (void*)umts_bearer_context_ftbl_p,
      0x0,
      (sizeof(ds_bearer_context_ftbl_s))
      );

  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, activateCnf, \
                        dsUmtsBearerCntxt_activateCnf);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getOrigParams, \
                        dsUmtsBearerCntxt_getOrigParams);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, bring_up, \
                        dsUmtsBearerCntxt_bringUp);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getEndParams, \
                        dsUmtsBearerCntxt_getEndParams) ;  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, deactivateCnf, \
                        dsUmtsBearerCntxt_deactivateCnf) ;
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, tear_down, \
                        dsUmtsBearerCntx_tear_down_pdn_connection);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, suspend_ind, \
                        dsUmtsBearerCntxt_suspendIndHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, resume_ind, \
                        dsUmtsBearerCntxt_resumeIndHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, get_bearer_tech_type, \
                        dsUmtsBearerCntxt_bearerTechTypeHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getratMaskforEgprsSupport, \
                        dsUmtsBearerCntxtGprsGetratMaskforEgprsSupport);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM,redial,\
                        dsUmtsBearerCntxt_redialHdlr);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getCellSrvInd, \
                        dsUmtsBearerCntxtGetCellSrvInd);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, setCellSrvInd, \
                        dsUmtsBearerCntxtSetCellSrvInd);
#ifdef FEATURE_DATA_WLAN_MAPCON
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, get_bearer_tech_type_ex, \
                        dsUmtsBearerCntxt_bearerTechTypeHdlrEx);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getSoMask, \
                        dsUmtsBearerCntxt_getSoMask);
#endif /* FEATURE_DATA_WLAN_MAPCON */
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, get_bearer_rate, \
                        dsUmtsBearerCntxt_bearerRateHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, go_null, \
                        dsUmtsBearerCntxt_go_null_hdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, empty_watermark, \
                        dsUmtsBearerCntxt_emptywatermark);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, free_umts_bearer, \
                        dsUmtsBearerCntxt_free_umts_bearer);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, allocate_umts_bearer, \
                        dsUmtsBearerCntxt_allocate_umts_bearer);

  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, refresh_actual_throughput, \
                        dsUmtsBearerCntxt_refresh_actual_throughput);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, init_bearer_throughput_stats, \
                        dsUmtsBearerCntxt_init_bearer_throughput_stats);
#if defined(FEATURE_DATA_LTE) && defined(FEATURE_GSM_GPRS)
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, cntxt_transfer_ind_setup, \
                                    ds_umts_bearer_cntxt_transfer_setup_hdlr);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, cntxt_transfer_ind_release, \
                                    ds_umts_bearer_cntxt_transfer_release_hdlr);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, resume_validate, \
                        dsUmtsBearerCntxt_performResumeValidation);								
#endif /* defined(FEATURE_DATA_LTE) && defined(FEATURE_GSM_GPRS) */
 
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, mode_pref_change, \
                        dsUmtsBearerCntxt_ModePrefChgHdlr);
 
#ifdef FEATURE_QXDM_WM_TRACING
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, report_wm_stats, \
                        dsUmtsBearerCntxt_reportWmStats);
#endif /* FEATURE_QXDM_WM_TRACING */
#endif /* FEATURE_GSM_GPRS */

  /*-------------------------------------------------------------------------
    Filling the TDSCDMA virtual function table now. The handlers are same.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_TDSCDMA
  umts_bearer_context_ftbl_p = &(ds_bearer_context_ftbl[DS_3GPPI_SYS_MODE_TDS]);

  memset(
      (void*)umts_bearer_context_ftbl_p,
      0x0,
      (sizeof(ds_bearer_context_ftbl_s))
      );
  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, dormancyReorig, \
                        dsUmtsBearerCntx_reorig);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, activateCnf, \
                        dsUmtsBearerCntxt_activateCnf);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, getOrigParams, \
                        dsUmtsBearerCntxt_getOrigParams);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, bring_up, \
                        dsUmtsBearerCntxt_bringUp);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, getEndParams, \
                        dsUmtsBearerCntxt_getEndParams) ;  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, deactivateCnf, \
                        dsUmtsBearerCntxt_deactivateCnf) ;
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, tear_down, \
                        dsUmtsBearerCntx_tear_down_pdn_connection);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, rab_re_estab_ind, \
                        dsUmtsBearerCntxt_RabReEstabIndHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, suspend_ind, \
                        dsUmtsBearerCntxt_suspendIndHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, resume_ind, \
                        dsUmtsBearerCntxt_resumeIndHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, get_bearer_tech_type, \
                        dsUmtsBearerCntxt_bearerTechTypeHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, getratMaskforHs, \
                        dsUmtsBearerCntxtGetPsRatMaskForHsStatus);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, redial,\
                        dsUmtsBearerCntxt_redialHdlr);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, getCellSrvInd, \
                        dsUmtsBearerCntxtGetCellSrvInd);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, setCellSrvInd, \
                        dsUmtsBearerCntxtSetCellSrvInd);
#ifdef FEATURE_DATA_WLAN_MAPCON
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, get_bearer_tech_type_ex, \
                        dsUmtsBearerCntxt_bearerTechTypeHdlrEx);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, getSoMask, \
                        dsUmtsBearerCntxt_getSoMask);
#endif /* FEATURE_DATA_WLAN_MAPCON */
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, get_bearer_rate, \
                        dsUmtsBearerCntxt_bearerRateHdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, go_null, \
                        dsUmtsBearerCntxt_go_null_hdlr);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, empty_watermark, \
                        dsUmtsBearerCntxt_emptywatermark);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, free_umts_bearer, \
                        dsUmtsBearerCntxt_free_umts_bearer);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, allocate_umts_bearer, \
                        dsUmtsBearerCntxt_allocate_umts_bearer);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, examine_if_throt_needed, \
                        dsUmtsBearerCntxt_examine_if_throt_needed);

  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, refresh_actual_throughput, \
                        dsUmtsBearerCntxt_refresh_actual_throughput);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, init_bearer_throughput_stats, \
                        dsUmtsBearerCntxt_init_bearer_throughput_stats);
#if defined(FEATURE_DATA_LTE) && defined(FEATURE_TDSCDMA)
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, cntxt_transfer_ind_setup, \
                                    ds_umts_bearer_cntxt_transfer_setup_hdlr);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, cntxt_transfer_ind_release, \
                                    ds_umts_bearer_cntxt_transfer_release_hdlr);  
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, resume_validate, \
                        dsUmtsBearerCntxt_performResumeValidation);								
#endif /* defined(FEATURE_DATA_LTE) && defined(FEATURE_TDSCDMA) */
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, complogfetchiface, \
		        ds_umts_bearer_cntxt_comp_log_fetch_iface);
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, mode_pref_change, \
                        dsUmtsBearerCntxt_ModePrefChgHdlr);

#ifdef FEATURE_QXDM_WM_TRACING
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, report_wm_stats, \
                        dsUmtsBearerCntxt_reportWmStats);
#endif /* FEATURE_QXDM_WM_TRACING */

#endif /* FEATURE_TDSCDMA */

}/*dsUmtsBearerCntxt_init()*/

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_free_umts_bearer

DESCRIPTION
  This function is used to free the UMTS Bearer context. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsBearerCntxt_free_umts_bearer(
   ds_bearer_context_s*        bearer_context_p
)
{
  int index = -1;
  boolean ret_val=FALSE;
  ds_umts_bearer_context_s *umts_bearer_context = NULL;
  sys_sys_mode_e_type  umts_network_mode = SYS_SYS_MODE_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
#if defined FEATURE_WCDMA
  umts_network_mode = SYS_SYS_MODE_WCDMA;
#elif defined FEATURE_TDSCDMA
  umts_network_mode =  SYS_SYS_MODE_TDS;
#elif defined FEATURE_GSM_GPRS
  umts_network_mode = SYS_SYS_MODE_GSM;
#endif


  /*---------------------------------------------------------------------- 
    Don't bother about the cookie since we are anyways freeing memory
   ----------------------------------------------------------------------*/

  /*---------------------------------------------------------------------- 
    KW Fix: Check bearer_context_p for NULL. Return FALSE if NULL
  ----------------------------------------------------------------------*/

  if((bearer_context_p != NULL) && 
     (bearer_context_p->ds_bearer_context_dyn_p != NULL))
  {
     /* Here we are using only wcdma; this is because irrespective of the mode we always put it in 
    WCDMA  virtual object */

    umts_bearer_context = (ds_umts_bearer_context_s *)
      DS_BEARER_CTXT_VOBJ(bearer_context_p, umts_network_mode);

    if((umts_bearer_context != NULL) && 
       (umts_bearer_context->ds_umts_bearer_context_dyn_p != NULL))
    {
       DS_3GPP_MSG3_HIGH("Freeing Wcdma bearer inst", 0,0,0);
       memset(umts_bearer_context->ds_umts_bearer_context_dyn_p,0x0,sizeof(ds_umts_bearer_context_dyn_s));
       modem_mem_free(umts_bearer_context->ds_umts_bearer_context_dyn_p, MODEM_MEM_CLIENT_DATA);
       umts_bearer_context->ds_umts_bearer_context_dyn_p = NULL;
       ret_val = TRUE;
    }
    else
    {
      index = (uint32)(bearer_context_p->ds_bearer_context_dyn_p->index);
    
      if ((index < 0) || (index >= DS_3GPP_MAX_BEARER_CONTEXT))
      {
        DS_3GPP_MSG3_ERROR("Bearer index invalid: %d, return FALSE", index, 0, 0);
        return ret_val;
      }

      umts_bearer_context = &(umts_bearer_context_tbl[index]);

      if (umts_bearer_context->ds_umts_bearer_context_dyn_p == NULL)
      {
        DS_3GPP_MSG3_HIGH("UMTS Bearer context invalid", 0,0,0);
      }
      else
      {
        memset(umts_bearer_context->ds_umts_bearer_context_dyn_p,
                0x0, sizeof(ds_umts_bearer_context_dyn_s));
        modem_mem_free(umts_bearer_context->ds_umts_bearer_context_dyn_p, 
                       MODEM_MEM_CLIENT_DATA);
        umts_bearer_context->ds_umts_bearer_context_dyn_p = NULL;
        ret_val = TRUE;
      }
    }
  }
   
  return ret_val;   
}
/*===========================================================================
FUNCTION DS_TDS_W_BEARER_CNTXT_CALCULATE_ACTUAL_THROUGHPUT

DESCRIPTION
  This function calculates the actual throughput for all active UMTS bearers 

PARAMETERS
  ds_bearer_context_s*    -  Pointer to the bearer context
  sys_modem_as_id_e_type  -  Subscription ID
  uint32                  -  Sampling interval in msec

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - If the actual throughput values have been calculated successfully
  FALSE - Otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_tds_w_bearer_cntxt_calculate_actual_throughput
(
  ds_bearer_context_s        *bearer_context_p,
  sys_modem_as_id_e_type      cm_subs_id,
  uint32                      sampling_interval
)
{
  boolean       ret_val             = FALSE;
  uint64        prev_tx_wm_cnt      = 0;
  uint64        current_tx_wm_cnt   = 0;
  uint64        prev_rx_wm_cnt      = 0;
  uint64        current_rx_wm_cnt   = 0;  
  uint32        prev_tx_running_avg = 0;
  uint32        prev_rx_running_avg = 0;
  const uint8   MOVING_AVG_COEFF             = 20;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sampling_interval == 0)
  {
    DS_3GPP_MSG0_MED("Not a valid sampling Interval to calculate actual tput");
    return ret_val;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return ret_val;
  }

  DS_3GPP_MSG2_MED("Updating Actual throughput for Bearer 0x%x bearer call "
                    "mode %d",bearer_context_p,
                    bearer_context_p->ds_bearer_context_dyn_p->call_mode);

  prev_rx_wm_cnt        = bearer_context_p->ds_bearer_context_dyn_p->
                          bearer_throughput_stats.total_prev_recd_rx_wm_cnt;
  current_rx_wm_cnt     = bearer_context_p->rx_wm.wm_item.total_rcvd_cnt;

  prev_tx_wm_cnt        = bearer_context_p->ds_bearer_context_dyn_p->
                          bearer_throughput_stats.total_prev_recd_tx_wm_cnt;  
  current_tx_wm_cnt     = bearer_context_p->tx_wm.wm_item.total_rcvd_cnt;

  prev_rx_running_avg   = (uint32)bearer_context_p->ds_bearer_context_dyn_p->
                          bearer_throughput_stats.actual_dl_rate;
  prev_tx_running_avg   = (uint32)bearer_context_p->ds_bearer_context_dyn_p->
                          bearer_throughput_stats.actual_ul_rate;

  /*------------------------------------------------------------------------- 
    Calculate new UL actual throughput rate
    -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->
     bearer_throughput_stats.actual_ul_rate = ds_util_calculate_throughput_moving_avg
                                              (
                                                 prev_tx_running_avg,
                                                 prev_tx_wm_cnt,
                                                 current_tx_wm_cnt,
                                                 sampling_interval,
                                                 MOVING_AVG_COEFF
                                              );
  DS_3GPP_MSG3_HIGH("Prev UL Bytes Cnt transf %lu WM's byte cnt %lu "
                   "Calculated Avg actual t'put %lu kbps",prev_tx_wm_cnt,
                   current_tx_wm_cnt,bearer_context_p->ds_bearer_context_dyn_p->
                          bearer_throughput_stats.actual_ul_rate);
  /*------------------------------------------------------------------------- 
    Calculate the new DL actual throughput rate
    -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->
  bearer_throughput_stats.actual_dl_rate    = ds_util_calculate_throughput_moving_avg
                                              (
                                                 prev_rx_running_avg,
                                                 prev_rx_wm_cnt,
                                                 current_rx_wm_cnt,
                                                 sampling_interval,
                                                 MOVING_AVG_COEFF
                                              );

  /*------------------------------------------------------------------------- 
    Store the new DL and UL Rx WM byte count for the next run
    -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->
    bearer_throughput_stats.total_prev_recd_rx_wm_cnt = current_rx_wm_cnt;

  bearer_context_p->ds_bearer_context_dyn_p->
    bearer_throughput_stats.total_prev_recd_tx_wm_cnt = current_tx_wm_cnt;

  ret_val = TRUE;
  return ret_val;
}


/*===========================================================================
FUNCTION dsUmtsBearerCntxt_allocate_umts_bearer

DESCRIPTION
  This function is used to allocate UMTS Bearer context. 

PARAMETERS
  int index.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/

boolean dsUmtsBearerCntxt_allocate_umts_bearer(int index)
{
   boolean ret_val=FALSE;
   int loop_index = index;

  if ((loop_index < 0) || (loop_index >= DS_3GPP_MAX_BEARER_CONTEXT))
  {
    DS_3GPP_MSG0_ERROR("Index invalid, return FALSE");
    return ret_val;
  }

  if (NULL != umts_bearer_context_tbl[loop_index].ds_umts_bearer_context_dyn_p)
  {
    DS_3GPP_MSG1_HIGH("Freeing already allocated bearer at index %d", loop_index);
    modem_mem_free(umts_bearer_context_tbl[loop_index].ds_umts_bearer_context_dyn_p,
		   MODEM_MEM_CLIENT_DATA);
    umts_bearer_context_tbl[loop_index].ds_umts_bearer_context_dyn_p = NULL;
  }

   umts_bearer_context_tbl[loop_index].ds_umts_bearer_context_dyn_p = 
     (ds_umts_bearer_context_dyn_s *)
   modem_mem_alloc(sizeof(ds_umts_bearer_context_dyn_s),MODEM_MEM_CLIENT_DATA);
   
   if(umts_bearer_context_tbl[loop_index].ds_umts_bearer_context_dyn_p == NULL)
   {
     DS_3GPP_MSG3_ERROR("Heap Memory Allocation failed for client: %d size: %d", 
     MODEM_MEM_CLIENT_DATA,(sizeof(ds_umts_bearer_context_dyn_s)),0);
     return ret_val;
   }

   memset((void *)umts_bearer_context_tbl[loop_index].ds_umts_bearer_context_dyn_p,0x0,
          sizeof(ds_umts_bearer_context_dyn_s));

   umts_bearer_context_tbl[loop_index].ds_umts_bearer_context_dyn_p->cookie =  
     UMTS_BEARER_CNTXT_VALID_COOKIE;
   umts_bearer_context_tbl[loop_index].ds_umts_bearer_context_dyn_p->index = 
     index;
   ret_val=TRUE;

   return ret_val;
}

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_examine_if_throt_needed

DESCRIPTION
  This function is used to examine the service request reject cause code  
  and determine if we need to perform throttling.

PARAMETERS
  call_info_ptr  - Pointer to the call info to find out rejection cause

DEPENDENCIES
  None.

RETURN VALUE
  TRUE   -if throttling is needed
  FALSE  -otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsBearerCntxt_examine_if_throt_needed
(
  dsgwl_ps_call_info_u_type *call_info_ptr,
  sys_modem_as_id_e_type subs_id
)
{
  boolean                    ret_val = FALSE;
  cm_gw_ps_call_info_s_type  *umts_call_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(call_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("call_info_ptr NULL. Return False. No Throttling");
    return FALSE;    
  }

   umts_call_info_ptr = &(call_info_ptr->gw_ps_call);
  if(umts_call_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("umts_call_info_ptr NULL. Return False. No Throttling");
    return FALSE;
  }
  /*--------------------------------------------------------------------------
   If global throttling NV is disabled there shouldn't be any throttling for 
   service reject
  --------------------------------------------------------------------------*/
  if(ds_3gpp_nv_manager_get_global_throttling(subs_id) == FALSE)
  {
    DS_3GPP_MSG0_HIGH("Global Throttling NV71520 not enabled, so no throttling"
                      " for service reject on UMTS");
    return FALSE;
  }
  /*--------------------------------------------------------------------------
    Now determine if any throttling has to be done. Examine the service request
    reject cause code that comes as rab_cause
  --------------------------------------------------------------------------*/

  switch(umts_call_info_ptr->rab_cause)
  {
    case REASON_NONE:
    case ILLEGAL_MS: 
    case ILLEGAL_ME:
    case GPRS_SERVICES_NOT_ALLOWED:
    case MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:
    case IMPLICITLY_DETACHED:
    case PLMN_NOT_ALLOWED:
    case LA_NOT_ALLOWED:
    case NATIONAL_ROAMING_NOT_ALLOWED:
    case NO_SUITABLE_CELLS_IN_LA:
    case CONGESTTION:
    case NO_PDP_CONTEXT_ACTIVATED:
      DS_3GPP_MSG1_HIGH("Rej cause: %d. No back off needed for listed cause",
                          umts_call_info_ptr->rab_cause);
      ret_val = FALSE;
      break;
    default:
      DS_3GPP_MSG1_HIGH("Rej cause: %d. Back off needed for unlisted cause",
                         umts_call_info_ptr->rab_cause);
      /*--------------------------------------------------------------------
        Set ret val to true for unlisted cause because we need to throttling
        for unlisted cause codes
      --------------------------------------------------------------------*/
      ret_val = TRUE;
      break;
  } /* switch on rab_cause */
  
  return ret_val;
} /* dsUmtsBearerCntxt_examine_if_throt_needed */


#ifdef FEATURE_DATA_RATE_MAPPING
/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_MAP_MAX_BIT_RATE_TO_ALLOWED_RATE

DESCRIPTION
  This function maps the max bit rate passed in by the application to
  one of the allowed bit rates.
  Match the request for UL/DL Max data rates to the nearest rate from the
  set {16, 32, 64, 128, 256 and 384} kbps

PARAMETERS
  data_rate : data rate in kbps

DEPENDENCIES
  None.

RETURN VALUE
  maps max bit rate to allowed rate

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate
(
  uint32  data_rate
)
{
  uint32 max_bit_rate=0x0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (data_rate == 0)
  {
    max_bit_rate = QOS_BIT_RATE_SUBSCRIBED;
  }
  else if (data_rate <= 8 )
  {
    /*-----------------------------------------------------------------------
      If data rate is <= 8, set the max bit rate to 8.
    -----------------------------------------------------------------------*/
    max_bit_rate = 8;
  }
  else if (data_rate <= 16 )
  {
    /*-----------------------------------------------------------------------
      If data rate is <= 16, set the max bit rate to 16.
    -----------------------------------------------------------------------*/
    max_bit_rate = 16;
  }
  else if(data_rate <= 32 )
  {
    /*-----------------------------------------------------------------------
      If data rate is > 16 and <= 32, set the max bit rate to 32.
    -----------------------------------------------------------------------*/
    max_bit_rate = 32;
  }
  else if (data_rate <= 64 )
  {
    /*-----------------------------------------------------------------------
      If data rate is > 32 and <= 64, set the max bit rate to 64.
    -----------------------------------------------------------------------*/
    max_bit_rate = 64;
  }
  else if (data_rate <= 128)
  {
    /*-----------------------------------------------------------------------
      If data rate is > 64 and <= 128, set the max bit rate to 128.
    -----------------------------------------------------------------------*/
    max_bit_rate = 128;
  }
  else if (data_rate <=256 )
  {
    /*-----------------------------------------------------------------------
      If data rate is > 128 and <= 256, set the max bit rate to 256.
    -----------------------------------------------------------------------*/
    max_bit_rate = 256;
  }
  else if (data_rate <= 384)
  {
    /*-----------------------------------------------------------------------
      If data rate is > 256 and <= 384, set the max bit rate to 384.
    -----------------------------------------------------------------------*/
    max_bit_rate = 384;
  }
  else
  {
    /*-----------------------------------------------------------------------
       Otherwise, leave the data rate as-is.
    -----------------------------------------------------------------------*/
   max_bit_rate = data_rate;
  }
  DS_3GPP_MSG3_HIGH("data rate: %d : allowed max data rate :%d",data_rate,max_bit_rate,0);

  return max_bit_rate;

} /* dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate */
#endif

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONFIGUREPDPPPP

DESCRIPTION
  Add function header: This is used to fill up the PPP options with tx and rx
  function call backs. This function is called from UMTS PDN context as a part
  of configuring PPP. RMSM PPP calls PDN context to configure PPP which in turn
  results in this call.

PARAMETERS
  IN
    bearer_context_p  - This is the bearer that is used for this PPP call
    ppp_config_info_p - This is the pointer to PPP configuration block.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_configurePdpPpp
(
  ds_bearer_context_s* bearer_context_p,
  ppp_dev_opts_type*   ppp_config_info_p
)
{
  sys_sys_mode_e_type      network_mode;
  sys_modem_as_id_e_type   subs_id = SYS_MODEM_AS_ID_NONE;
  ds_pdn_context_s         *pdn_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  pdn_context_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return;
  }   

  /* Populated by UMTS Bearer cntx */
  ppp_config_info_p->rx_user_data   = (void*)bearer_context_p;
  ppp_config_info_p->tx_user_data   = (void*)bearer_context_p;
  ppp_config_info_p->rx_signal      = bearer_context_p->rx_sig;

  /*--------------------------------------------------------------------------
   * Call the mode specific functions that are going to fill up the Tx and Rx
   * call back functions
  --------------------------------------------------------------------------*/
  network_mode = ds_3gpp_get_apn_current_mode (pdn_context_p);

  if(network_mode != SYS_SYS_MODE_WCDMA 
#ifdef FEATURE_TDSCDMA
     &&
     network_mode != SYS_SYS_MODE_TDS
#endif
    )
  {
    DS_3GPP_MSG1_HIGH("Invalid mode. Cannot register TX and Rx functions. mode = %d", 
             network_mode);
    ASSERT(0);
  }

  DS_UMTS_BEARER_CTXT_VF_CALL
  (
    configurePdpPpp, 
    subs_id,
    bearer_context_p,
    ppp_config_info_p
  );
  
  return;
}/*dsUmtsBearerCntxt_configurePdpPpp()*/

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_SendQOSIndication

DESCRIPTION
  This function sends the traffic class information of a particular bearer to interested
  parties.
  
PARAMETERS  
  bearer_context_p - Bearer Context
  system_rat - Current system information.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsUmtsBearerCntxt_SendQOSIndication
(
   ds_bearer_context_s *bearer_context_p,
   sys_radio_access_tech_e_type system_rat
)
{
  ds_umts_qos_tclass_enum_type traffic_class = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
  sys_modem_as_id_e_type       subs_id;

  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

    if(!dsUmtsBearerCntxt_validatebearer(&umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p
    ->client_data_ptr]))
   {
    DS_3GPP_MSG0_ERROR("Umts Bearer context passed is Invalid");
    return;
   }


  traffic_class = 
    umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr].
        ds_umts_bearer_context_dyn_p->umts_qos_val.traffic_class;

  DS_3GPP_MSG1_MED("Traffic class of bearer is %d", traffic_class);

  if (traffic_class >= DS_UMTS_QOS_TCLASS_CONV &&
       traffic_class <= DS_UMTS_QOS_TCLASS_BACK)
  {    
    ds_3gpp_bearer_qos_info_msg_s qos_info_msg = {{0}};

    if( system_rat == SYS_RAT_UMTS_RADIO_ACCESS )
    {
      qos_info_msg.current_bearer_rat = DS_RAT_WCDMA;
    }
  #ifdef FEATURE_TDSCDMA
    else if( system_rat == SYS_RAT_TDS_RADIO_ACCESS )
    {
      qos_info_msg.current_bearer_rat = DS_RAT_TDS;
    }
  #endif

    qos_info_msg.u.tds_w_qos_info.rab_id = bearer_context_p->ds_bearer_context_dyn_p->nsapi;
    qos_info_msg.u.tds_w_qos_info.traffic_class = 
                        (ds_3gpp_traffic_class_enum_type)traffic_class;
     
    (void) dsmsgrsnd_per_subs_msg_send_ext
       ( 
         DS_3GPP_BEARER_QOS_INFO_IND,
         MSGR_DS_3GPP, 
         (msgr_hdr_struct_type*)(&qos_info_msg),
         sizeof(qos_info_msg),
         SYS_AS_ID_TO_INST_ID(subs_id));
  }

  return;

}

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_REPORT_WM_STATS_CB

DESCRIPTION
  This function internally call wcdma or gsm related function.

PARAMETERS
  bearer_instance - bearer context instance
  wm_stat_ptr     - pointer where we store the wm values

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_reportWmStats
(
  uint32          bearer_instance,
  fc_wm_stat_type *wm_stat_ptr
)
{
  uint32 call_id = 0;
  ds_bearer_context_s    *bearer_context_p = NULL;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  /*----------------------------------------------------------------------*/

  if(wm_stat_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("wm_stat_ptr provided by FC is NULL");
    return;                                               
  }

  /*---------------------------------------------------------------------------
    Get the bearer context pointer
  ---------------------------------------------------------------------------*/
  bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_instance);

 
  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  /*------------------------------------------------------------
    Get the call ID 
  --------------------------------------------------------------*/
  call_id = ds_bearer_cntxt_get_call_id(bearer_context_p);
  
  /*------------------------------------------------------------
    Collect statistics for DL and UL watermarks
  --------------------------------------------------------------*/
  DS_UMTS_BEARER_CTXT_VF_CALL
  (
    report_wm_stats, 
    subs_id,
    bearer_instance,
    call_id,
    wm_stat_ptr
  );
} /* dsUmtsBearerCntxt_reportWmStats */
#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_performResume

DESCRIPTION
  This function is used to handle resume indication from CM.
  This function is also called on sys_change to UMTS in connected mode scenarios
  It invokes the approp functionis registered (based upon the mode).

PARAMETERS     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntxt_performResume
(
  ds_bearer_context_s*        bearer_context_p
)
{
#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_context_s * eps_bearer_context = NULL;
  eps_bearer_id_T           eps_bearer_id = 0;
#endif /*FEATURE_DATA_LTE*/
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE; 
  ds_pdn_context_s*             pdn_context_p = NULL;

  
  DS_3GPP_MSG0_HIGH("In dsUmtsBearerCntxtperformResume");

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_context_p = (ds_pdn_context_s*)
               (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr);
  
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return;
  }
  
  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return;
  }


  if (!bearer_context_p->ds_bearer_context_dyn_p->is_transferable_to_GW)
  {
     //NAS will handle this case.
    DS_3GPP_MSG0_ERROR("is_transferable_to_GW is false NAS will handle this case.");
    return;  
  }
#ifdef FEATURE_DATA_LTE
  eps_bearer_context = (ds_eps_bearer_context_s *)
      DS_BEARER_CTXT_VOBJ(bearer_context_p, SYS_SYS_MODE_LTE);

  if(ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context))     
  {
    eps_bearer_id = eps_bearer_context->ds_eps_bearer_context_dyn_p->eps_bearer_id;    
  }
  /*--------------------------------------------------------------------------------
   Do not teardown the dedicated bearer in normal suspend/resume case(eg: W->OOS->W).
   NSAPI will be zero in IRAT case(eg: L->W). Valid otherwise
  ----------------------------------------------------------------------------------*/

  if( (eps_bearer_id != 0) &&
       (bearer_context_p->ds_bearer_context_dyn_p->is_default == FALSE)&&
       (ds_3gpp_nv_manager_get_l_to_w_qos_continuity_nv() == FALSE))
  {      
     DS_3GPP_MSG1_ERROR("QOS -- L-> GW continuity is not supported, ending call id:%d",
                              bearer_context_p->ds_bearer_context_dyn_p->cm_call_id);
     ds_bearer_cntxt_phys_link_down_cmd_hdlr(
                     (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr, NULL);
  }
  else 
#endif /*FEATURE_DATA_LTE*/
  {
    DS_UMTS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
    (      
      resume_ind,
      bearer_context_p
    );
  }
  return;
}

#ifdef FEATURE_LTE
/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_performResumeValidation

DESCRIPTION
  This function evaluates if performResume must be called.
  A Resummption on UMTS must be made in connected mode scenarios where the
  suspend flag is not set.

PARAMETERS     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void dsUmtsBearerCntxt_performResumeValidation
(
  sys_sys_mode_e_type           new_mode,
  sys_sys_mode_e_type           old_mode,
  sys_modem_as_id_e_type        subs_id
)
{
  ds_bearer_context_s     *bearer_context_p = NULL;
  uint32                   bearer_cntx_inst = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_3GPP_MSG2_LOW("In performResumeValidation,new_mode:%d,old_mode:%d",
                   new_mode, old_mode);
  /*---------------------------------------------------------------------- 
    Invoke performResume only for PSHO case if
    1. Suspended flag is FALSE
    2. New mode is one of W/G/T
    3. Old mode was LTE (This would indicate IRAT)
   -----------------------------------------------------------------------*/
  if(
      (!ds3gpp_get_ps_data_suspend_flag(subs_id)) &&
      (new_mode == SYS_SYS_MODE_GSM ||
       new_mode == SYS_SYS_MODE_WCDMA
#ifdef FEATURE_TDSCDMA
      ||
      new_mode == SYS_SYS_MODE_TDS
#endif /* FEATURE_TDSCDMA */
       ) &&
       (old_mode == SYS_SYS_MODE_LTE)
    )
  {
    /*----------------------------------------------------------------------
     This is one of the modes that we will be interested in
    ----------------------------------------------------------------------*/
    for( bearer_cntx_inst=0;bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;
       bearer_cntx_inst++)
    {
      bearer_context_p = 
        ds_bearer_cntxt_get_instance_ptr((uint8)bearer_cntx_inst);
	  
      if (!ds_bearer_cntx_validate_bearer_subscription_internal(
                                                   bearer_context_p,subs_id))
      {
        continue;
      }
 
      if((bearer_context_p->ds_bearer_context_dyn_p->state == 
                                DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB) ||
         (bearer_context_p->ds_bearer_context_dyn_p->state == 
                              DS_BEARER_CONTEXT_STATE_UP_DORMANT) ||
         (bearer_context_p->ds_bearer_context_dyn_p->state == 
                              DS_BEARER_CONTEXT_STATE_UP))
      {
        dsUmtsBearerCntxt_performResume(bearer_context_p);
      }
    }
  }
  return;
} /* dsUmtsBearerCntxt_performResumeValidation */

#endif//FEATURE_DATA_LTE 

/*===========================================================================
FUNCTION DS_UMTS_BEARER_CNTXT_COMP_LOG_FETCH_IFACE

DESCRIPTION
  This Functions returns the iface corresponding to link control id

PARAMETERS
  logging_info_ptr - the Log information 
  ip_version       - ip version v4 or v6

DEPENDENCIES
  None.

RETURN VALUE
  Iface.
  
SIDE EFFECTS
  None.

===========================================================================*/

ps_iface_type* ds_umts_bearer_cntxt_comp_log_fetch_iface
(
   ps_comp_logging_instance_config_s *logging_info_ptr,
   ip_version_enum_type ip_version
)
{
  ps_iface_type  *iface = NULL;
  sys_modem_as_id_e_type subs_id = ds3g_get_ps_subs_id(); // Vishal: Temporary

  DS_UMTS_BEARER_CTXT_VFR_CALL
  (
    iface,
    complogfetchiface,
    subs_id,
    logging_info_ptr, 
    ip_version
  );

  return iface;
}/* ds_umts_bearer_cntxt_comp_log_fetch_iface */

/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_teardown_all_calls

DESCRIPTION
  This function handles the system change from UMTS to CDMA 
  and tears down the calls existing in UMTS mode

PARAMETERS  
  subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void dsUmtsBearerCntxt_teardown_all_calls
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s         *bearer_context_p = NULL;
  uint32                       bearer_cntx_inst = 0;
  ds_3gpp_pdn_call_info_type   mode_spec_info;
  boolean                      retVal = DS3G_FAILURE;
  /*------------------------------------------------------------------------*/

  for( bearer_cntx_inst=0;bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;
       bearer_cntx_inst++)
  {
    bearer_context_p = 
      ds_bearer_cntxt_get_instance_ptr((uint8)bearer_cntx_inst);

    if(!ds_bearer_cntx_validate_bearer_subscription_internal(bearer_context_p, subs_id))
    {
      continue;
    }    

    if (bearer_context_p->ds_bearer_context_dyn_p->state == 
		                                DS_BEARER_CONTEXT_STATE_ANSWER)
    {
      mode_spec_info.mt_flag = TRUE;
    }
    else
    {
      mode_spec_info.mt_flag = FALSE;
      mode_spec_info.call_instance = (uint32)bearer_context_p->ds_bearer_context_dyn_p
       ->client_data_ptr;
    }
    retVal = dsUmtsBearerCntx_tear_down_pdn_connection(bearer_context_p,
		                                       &mode_spec_info,
		                                       NULL ); 
    if (retVal == DS3G_SUCCESS)
    {
      DS_3GPP_MSG1_HIGH("Teardown PDN connection success,bearer_instance:%d",
                                                     bearer_cntx_inst);
    }
  }
  return;
} /* dsUmtsBearerCntxt_teardown_all_calls */
/*===========================================================================

                                UNIT TEST

===========================================================================*/


#endif /* defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS) */
