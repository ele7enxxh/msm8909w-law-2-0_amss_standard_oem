/*===========================================================================
                        D S _ 3 G P P _ R M S M _ A T I F . C 

DESCRIPTION
  This file contains the interface between ATCoP and RMSM interface.
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2009 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/ds_3gpp_rmsm_atif.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---   ----------------------------------------------------------
09/25/12    ss    3GPP MH LTE-Off compilation fixes
09/13/10    sa    Fixed QCDGEN handler to send more data.
05/12/10    sa    Added support for dynamic parameter related commands.
03/15/10    sa    Added support for +CGCMOD command.
01/15/10    sa    Added support for $QCDGEN and +CGPADDR API.
11/11/09    sa    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_3GPP
#include "msg.h"
#include "dsumtspdpreg.h"
#include "ds_3gpp_rmsm_atif.h"
#include "ds_3gpp_rmsm_ati.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_flow_context.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds3gmgr.h"
#include "dsumtspdpregint.h"
#include "dsati.h"
#include "ds3gsubsmgr.h"

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            EXTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGACT_QUERY
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the Status of PDP
    profiles during +CGACT command execution. The function returns the
    activation status of all the valid profiles. 
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: in case of SUCCESS
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_cgact_query
(
  ds_3gpp_atif_profile_status_info_type  *profile_status_info_ptr,
  sys_modem_as_id_e_type                  subs_id 
)
{
  DS_3GPP_MSG0_HIGH("In ds_3gpp_rmsm_atif_cgact_query");
  
  return ds_3gpp_rmsm_ati_cgact_query(profile_status_info_ptr, subs_id);
}/* ds_3gpp_rmsm_atif_cgact_query */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGACT_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for activation or deactivation of profiles
    requested by +CGACT Command. This function returns DSAT_OK if the profile 
    is already activated by other applications or by +CGACT. Only the profile 
    activated by +CGACT context can only be de-activated from activated state.
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: if the activation state and current state of the profile is same.
    DSAT_ASYNC_CMD: Activation or de-activation is in process.
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_cgact_handler
(
  uint32                                profile_number,
  ds_3gpp_atif_cgact_act_mode_e_type    act_state,
  void *                                user_info_ptr,
  sys_modem_as_id_e_type                subs_id
)
{
  boolean valid_flag = FALSE;
  dsat_result_enum_type result= DSAT_ASYNC_CMD;
  ds_3gpp_rmsm_at_instance_type instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG2_HIGH("In ds_3gpp_rmsm_atif_cgact_handler profile_number %d, act state %d ",
                      profile_number,(int)act_state);
#if 0
  /* Check if subs_id passed is ps_subs_id */
  if (subs_id != ds3g_get_ps_subs_id() &&
      ds3gsubsmgr_is_device_mode_multi_sim_standby() == TRUE)
  {
    MSG_ERROR("Non-PS subs_id: %d passed in dial_str_cb, failing call",
               subs_id, 0, 0);
    result = DSAT_ERROR;

    return result;
  }
#endif

  /* Profile validation */
  if (DS_UMTS_PDP_SUCCESS != 
            ds_umts_get_pdp_profile_context_info_is_valid_per_subs( (uint16)profile_number,
                                             dsumts_subs_mgr_get_subs_id(subs_id),
                                                           &valid_flag))
  {
    DS_3GPP_MSG1_ERROR ("Error on PDP profile valid query: %d",profile_number); 
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE; 
  }

  /* state validation */
  switch (act_state)
  {
   case DS_3GPP_ATIF_ACTIVATION:
      result = ds_3gpp_rmsm_ati_cgact_activation( profile_number,
                                                  subs_id,
                                                  user_info_ptr);
      if(result == DSAT_ERROR)
      {
        /* If instance is available, de-allocate it in case of ERROR */
        instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_number, subs_id);
        if ( DS_3GPP_RMSM_AT_UM_MAX_INSTANCES != instance)
        {
          ds_3gpp_rmsm_ati_deallocate_instance(instance);
        }
      }
      break;
   case DS_3GPP_ATIF_DEACTIVATION:
     result = ds_3gpp_rmsm_ati_cgact_deactivation( profile_number,
                                                   subs_id,
                                                 user_info_ptr);
     break;
   default:
     DS_3GPP_MSG0_ERROR("Invalid activation state");
     result = DSAT_ERROR;
     break;
  }
  return result;
}/* ds_3gpp_rmsm_atif_cgact_handler */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_QCDGEN_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for sending uplink packets of data len as 
    requested by $QCDGEN Command. Only the PDP-IP profile activated by +CGACT 
    context can be used for data transfer. DSAT_ASYNC_CMD will returned if
    data transfer is initiated. Once the data transfer is complete, 
    result code is send using dsat_rmsm_done_handler().
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK/DSAT_ASYNC_CMD: in case of Success

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_qcdgen_handler
(
  uint32                                  profile_number,
  sys_modem_as_id_e_type                  subs_id,
  uint32                                  total_data_len,
  void *                                  user_info_ptr
)
{
  DS_3GPP_MSG2_HIGH("In ds_3gpp_rmsm_atif_qcdgen_handler for profile %d and data len %d",
                profile_number,total_data_len);

  if(ds_3gpp_profile_cache_get_cache_index((uint16)profile_number, 
                                           dsumts_subs_mgr_get_subs_id(subs_id)) == -1)
  {
    DS_3GPP_MSG3_ERROR("Profile_number:%d not present in cache",
              profile_number, 0, 0);
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE;
  }


  return ds_3gpp_rmsm_ati_process_qcdgen(profile_number,
                                         subs_id,
                             total_data_len,
                             user_info_ptr);
}/* ds_3gpp_rmsm_atif_qcdgen_handler */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_GET_PDP_ADDR
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the PDP address
    of the activated profile. 
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: in case of SUCCESS
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_get_pdp_addr
(
  uint32                                  profile_number,
  sys_modem_as_id_e_type                  subs_id,
  ds_umts_pdp_addr_type                  *pdp_addr_ptr
)
{
  DS_3GPP_MSG1_HIGH("In ds_3gpp_rmsm_atif_get_pdp_addr for profile num %d",
    profile_number);
  
    /* Profile validation */
  if(ds_3gpp_profile_cache_get_cache_index((uint16)profile_number, 
                                           dsumts_subs_mgr_get_subs_id(subs_id)) == -1)
  {
    DS_3GPP_MSG3_ERROR("Profile_number:%d not present in cache",
              profile_number, 0, 0);
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE;
  }

  return ds_3gpp_rmsm_ati_get_pdp_addr(profile_number,
                                       subs_id,
                                       pdp_addr_ptr);

}/* ds_3gpp_rmsm_atif_get_pdp_addr */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGCMOD_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for modification of profiles
    requested by +CGCMOD Command. This function returns DSAT_ERROR if the 
    profile is not activated by +CGACT. Only the profile.

  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_ASYNC_CMD: Modification is in process.
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_cgcmod_handler
(
  uint32                                profile_number,
  sys_modem_as_id_e_type                subs_id,
  void *                                user_info_ptr
)
{
  DS_3GPP_MSG1_HIGH("In ds_3gpp_rmsm_atif_cgcmod_handler for profile %d",
            profile_number);

  /* Profile validation */
  if(ds_3gpp_profile_cache_get_cache_index((uint16)profile_number, 
                                           dsumts_subs_mgr_get_subs_id(subs_id)) == -1)
  {
    DS_3GPP_MSG3_ERROR("Profile_number:%d not present in cache",
              profile_number, 0, 0);
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE;
  }

  return ds_3gpp_rmsm_ati_cgcmod_handler(profile_number,
                                         subs_id,
                                          user_info_ptr);
}/* ds_3gpp_rmsm_atif_cgcmod_handler */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_READ_DYNAMIC_PARAMS
  
  DESCRIPTION
    This Function is called by ATCoP for reading the dynamic parameters 
    associated with the primary /secondary activated profiles .
    This function returns DSAT_ERROR if the profile is not activated .
    Secondary profile details can be provided only if the context 
    is activated using +CGACT.

  DEPENDENCIES
    None.
  
  RETURN VALUE
    FALSE: in case of Failure
    TRUE: If retrieval of dynamic data is success.
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_atif_read_dynamic_params
(
  uint32                                      profile_number,
  sys_modem_as_id_e_type                      subs_id,
  ds_3gpp_atif_dynamic_e_type                 info_type,
  ds_3gpp_atif_dynamic_params_info_type      *dynamic_param_ptr
)
{
  DS_3GPP_MSG2_HIGH("In ds_3gpp_rmsm_atif_read_dynamic_params for profile %d, info_type %d",
            profile_number,(int)info_type);

      /* Profile validation */
  if((DS_3GPP_ATIF_PRIM_INFO == info_type)
     &&(ds_3gpp_profile_cache_get_cache_index((uint16)profile_number, 
                                              dsumts_subs_mgr_get_subs_id(subs_id)) == -1))
  {
    DS_3GPP_MSG3_ERROR("Profile_number:%d not present in cache",
              profile_number, 0, 0);
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE;
  }

  return ds_3gpp_rmsm_ati_read_dynamic_params(profile_number,
                                              subs_id,
                                               info_type,
                                               dynamic_param_ptr);
}/* ds_3gpp_rmsm_atif_read_dynamic_params */


/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_GET_ALL_ACTIVE_PRIM_PROFILES
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the list of all 
    active primary profiles.
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_atif_get_all_active_prim_profiles
(
  sys_modem_as_id_e_type                subs_id,
  ds_3gpp_atif_prim_profile_list_type * profile_list_info_ptr
)
{
  ASSERT(profile_list_info_ptr != NULL);
  
  ds_pdn_cntx_get_all_pdn_context_profiles(subs_id, profile_list_info_ptr);

}/* ds_3gpp_rmsm_atif_get_all_active_pdn_profiles */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_GET_ALL_ACTIVE_FLOW_PROFILES
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the list of all 
    active flow profiles.
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_atif_get_all_active_flow_profiles
(
  sys_modem_as_id_e_type            subs_id,
  ds_3gpp_atif_flow_cid_list_type * cid_list_info_ptr
)
{
  ASSERT(cid_list_info_ptr != NULL);

  ds_bearer_flow_manager_get_all_cids(subs_id, cid_list_info_ptr);
}/* ds_3gpp_rmsm_atif_get_all_active_flow_profiles */

#endif /* FEATURE_DATA_3GPP */
