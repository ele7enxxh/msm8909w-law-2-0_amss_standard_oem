#ifndef DS_PROFILE_3GPP2_MGR_H
#define DS_PROFILE_3GPP2_MGR_H
/******************************************************************************
  @file    ds_profile_3gpp2_mgr.h
  @brief

  DESCRIPTION
  Tech specific implementation of 3GPP2 Profile Management

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/api/ds_profile_3gpp2_mgr.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/18/13   hr      Created the module. First version of the file.
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "ds_profile_db_internal.h"
#include "ds_profile_db.h"
#include "ds707_data_session_profile.h"

/*===========================================================================
                              INTERNAL UTILITIES
===========================================================================*/
/*
typedef struct
{
  ds_profile_db_ident_type  ident,
  ds_profile_type_enum_type valid_mask
}dsi_profile_db_3gpp2_params_valid_mask;
*/ 
 
/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_VALIDATION_FUNC

DESCRIPTION   Validate the profile parameters when creating a profile.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_validation_func
(
  ds_profile_db_ident_type        ident, 
  const ds_profile_db_info_type*  info,
  const void*                     profile_params,
  ...
);/*ds_profile_3gpp2_mgr_validation_func*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_PROFILE_VALIDATION_GENERAL

DESCRIPTION   General validations related to internal (not public) params, 
              invoked during once during profile creation

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_profile_validation_general
(
  const void* profile
);/*ds_profile_3gpp2_mgr_profile_validation_general*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_DEFAULT_PROFILE

DESCRIPTION   Get default parameters.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_get_default_profile
(
  void* profile
);/*ds_profile_3gpp2_mgr_get_default_profile*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_DEFAULT_PROFILE_NUM

DESCRIPTION   Get default profile number.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_default_profile_num
(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_family_type      family
);/*ds_profile_3gpp2_mgr_get_default_profile_num*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_DEFAULT_PROFILE_NUM_PER_SUBS

DESCRIPTION   Get default profile number.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_default_profile_num_per_subs
(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_family_type      family,
  ds_profile_db_subs_type        subs
);/*ds_profile_3gpp2_mgr_get_default_profile_num_per_subs*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_SET_DEFAULT_PROFILE_NUM

DESCRIPTION   Set default profile number.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_set_default_profile_num
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type      family
);/*ds_profile_3gpp2_mgr_set_default_profile_num*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_SET_DEFAULT_PROFILE_NUM_PER_SUBS

DESCRIPTION   Set default profile number.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_set_default_profile_num_per_subs
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type      family,
  ds_profile_db_subs_type        subs
);/* ds_profile_3gpp2_mgr_set_default_profile_num_per_subs */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_PROFILE

DESCRIPTION   Get the parameters of a specific profile.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_profile
(
  ds_profile_db_profile_num_type          num, 
  void*                                   tech_params,
  ds_profile_db_gen_params_type*          gen_params,
  ds_profile_db_subs_type                 subs
);/*ds_profile_3gpp2_mgr_get_profile*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_PROFILES_RANGE

DESCRIPTION   Get the range of a specific profile subtype.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_profiles_range
(
  ds_profile_db_3gpp2_profile_subtype  sub_tech,
  ds_profile_db_profile_num_type*      min_num,
  ds_profile_db_profile_num_type*      max_num
);/*ds_profile_3gpp2_mgr_get_profiles_range*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_FUNC_TABLE_INIT

DESCRIPTION   Register function table with profile DB during MH cache init 
 
PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_func_table_init
(
  void
);/*ds_profile_3gpp2_mgr_func_table_init*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_REGISTER_CB_EVENT

DESCRIPTION   3GPP2 MH will register this callback function for all events 
              from profile DB, and depending on the event, will call the
              appropriate event handler function

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_register_cb_event
(
  void
);/*ds_profile_3gpp2_mgr_register_cb_event*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_IS_TECH_PARAM_SET

DESCRIPTION   Get if the tech specific parameter is set in the EPC profile

PARAMETERS    num                - ds_profile_db_profile_num_type 
              ident              - ds_profile_db_ident_type
              *is_tech_param_set - boolean
DEPENDENCIES  None

RETURN VALUE  ds_profile_db_result_type

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_is_tech_param_set
(
  ds_profile_db_profile_num_type          num,
  ds_profile_db_subs_type                 subs,
  ds_profile_db_ident_type                ident,
  boolean*                                is_tech_param_set
);/*ds_profile_3gpp2_mgr_get_is_tech_param_set*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_NUM_VALID_OMH_PROFILES

DESCRIPTION   Returns number of valid OMH profiles

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  number of valid OMH profiles

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_profile_3gpp2_mgr_get_num_valid_omh_profiles
(
  void
);/*ds_profile_3gpp2_mgr_get_num_valid_omh_profiles*/

#endif /* DS_PROFILE_3GPP2_MGR_H */
