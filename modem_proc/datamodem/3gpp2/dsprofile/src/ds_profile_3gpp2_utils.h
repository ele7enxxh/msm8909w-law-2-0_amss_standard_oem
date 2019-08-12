#ifndef DS_PROFILE_3GPP2_UTILS_H
#define DS_PROFILE_3GPP2_UTILS_H
/******************************************************************************
  @file    ds_profile_3gpp2_utils.h
  @brief

  DESCRIPTION
  Utililities for implementation of 3GPP2 Profile Management. This is for
  internal use only.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsprofile/src/ds_profile_3gpp2_utils.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/14/14   hr      Add support for common PDN and uniform callback operations
03/19/14   hr      Fix APN enabled flag default value 
12/18/13   hr      Created the module. First version of the file.
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "ds_profile_db.h"
#include "ds_profile_3gpp2.h"
#include "ds_profile_3gpp2i.h"
#include "ds_profile_3gpp2_mgr.h"
#include "ds707_data_session_profile.h"
#include "ds707_roaming.h"
#include "ds707_gonulltmr.h"

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION DSI_PROFILE_3GPP2_UTIL_IDENT_VALIDATION

DESCRIPTION
  The function calls ident specific validation function.
    
PARAMETERS: 
  ident: Identifier of the parameter to validate
  info : 
 
DEPENDENCIES
  
RETURN VALUE 
  
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_db_result_type dsi_profile_3gpp2_util_ident_validation
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type*  info
); /* dsi_profile_3gpp2_util_ident_validation */

/*===========================================================================
FUNCTION DSI_PROFILE_3GPP2_UTIL_GEN_IDENT_VALIDATION

DESCRIPTION
  The function calls ident specific validation function.
    
PARAMETERS: 
  ident: Identifier of the parameter to validate
  info : 
 
DEPENDENCIES
  
RETURN VALUE 
  
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_db_result_type dsi_profile_3gpp2_util_gen_ident_validation
(
   const ds_profile_db_3gpp2_tech_params_type *profile
);/* dsi_profile_db_3gpp2_util_gen_ident_validation */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_GET_DEFAULT_PARAMS

DESCRIPTION   

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_get_default_params
(
  ds_profile_db_3gpp2_tech_params_type *tech_params, 
  data_sess_profile_ppp_params_type    *ppp_params_ptr
); /* ds_profile_3gpp2_util_get_default_params */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_CACHE_TO_DB_TYPE_CONVERT

DESCRIPTION   This function converts a profile from 3GPP2 MH cache type to 
              profile DB type.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_cache_to_db_type_convert
(
  ds_profile_db_gen_params_type         *gen_params,
  ds_profile_db_3gpp2_tech_params_type    *tech_params,
  ds707_data_session_profile_info_type  *data_sess_profile
); /* ds_profile_3gpp2_util_cache_to_db_type_convert */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_GET_PARAMS

DESCRIPTION   This function retrieves required values to copy profile

PARAMETERS    profile        : profile DB managed profile type
              gen_params     : C type general parameters
              tech_params    : C type tech specific parameters

DEPENDENCIES  None

RETURN VALUE  TRUE  : Success
              FALSE : Failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds_profile_3gpp2_util_get_params
(
  const ds_profile_db_profile_managed_type *profile,
  ds_profile_db_gen_params_type            *gen_params,
  ds_profile_db_3gpp2_tech_params_type     *tech_params
); /* ds_profile_3gpp2_util_get_params */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_COPY_PROFILE_AND_SET_CUST_MASK

DESCRIPTION   This function copies DB type profiles into MH cache. It sets 
              the cust_val_mask to mark params set by user. For EPC profiles,
              if the tech specific param is not set and has a corresponding
              common param, MH will take the common param value

PARAMETERS    profile        : profile DB managed profile type
              are_apn_fields_chged  : are APN, apn_flag or PDN lable changed
              gen_params            : C type general parameters
              tech_params           : C type tech specific parameters
              common_params         : C type common parameters for EPC profile
              data_sess_profile     : ptr to the cache

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_copy_profile_and_set_cust_mask
(
  const ds_profile_db_profile_managed_type *profile,
  boolean                                  *mandatory_apn_changed,
  ds_profile_db_gen_params_type            *gen_params,
  ds_profile_db_3gpp2_tech_params_type     *tech_params,
  ds_profile_db_common_tech_params_type   **common_params,
  ds707_data_session_profile_info_type     *data_sess_profile
); /* ds_profile_3gpp2_util_copy_profile_and_set_cust_mask */

/*===========================================================================
FUNCTION GET_VALID_MASK_FROM_IDENT

DESCRIPTION
  This function returns the valid mask of the identifier.

PARAMETERS
  param_id : identifier for which mask is to be returned

DEPENDENCIES

RETURN VALUE
  returns the valid mask for the identifier
SIDE EFFECTS

===========================================================================*/
/*lint -save -e656*/

ds_profile_3gpp2_valid_profile_enum_type get_valid_mask_from_ident(
  ds_profile_identifier_type param_id
);

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_CACHE_TO_DB_GET_IS_TECH_PARAM_SET

DESCRIPTION   This function returns a boolean based on tech param being set

PARAMETERS    ident              - ds_profile_db_ident_type 
              data_sess_profile  - ds707_data_session_profile_info_type
              *is_tech_param_set - boolean
 
DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_cache_to_db_get_is_tech_param_set
(
  ds_profile_db_ident_type                ident,
  ds707_data_session_profile_info_type*   data_sess_profile,
  boolean*                                is_tech_param_set
);


#endif /* DS_PROFILE_3GPP2_UTILS_H */
