/******************************************************************************
  @file    ds_profile_db_handler.c
  @brief   DS PROFILE Database handler implementation

  DESCRIPTION
  This file contains implementation of DS PROFILE DB Handler.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2012-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

01/20/13   am      Implementation for all functions
12/20/12   mg      Created the module. First version of the file.


===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/
#include "ds_profile_db_handler.h"
#include "ds_profile_db.h"
#include "ds_profile.h"
#include "ds_profile_3gpp2.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_os.h"
#include "datamodem_variation.h"



/*===========================================================================      
                  Internal helper structs
===========================================================================*/
/* internal implementation for list node */
typedef struct
{
  ds_profile_db_profile_num_type profile_number;
  char profile_name[DS_PROFILE_DB_MAX_PROFILE_NAME_LEN+1];
} ds_profile_db_list_info_type;

/*===========================================================================      
                  Internal helper functions
===========================================================================*/
static ds_profile_db_supported_tech_mask legacy_tech_to_techmask(
  ds_profile_tech_etype tech
)
{
  switch (tech)
  {
  case(DS_PROFILE_TECH_EPC):
    return (DB_TECH_TO_TECHMASK(DB_TECH_3GPP)  |
            DB_TECH_TO_TECHMASK(DB_TECH_3GPP2) |
            DB_TECH_TO_TECHMASK(DB_TECH_COMMON));                                 
  default:
    return (DB_TECH_TO_TECHMASK(tech));
  }
}

static ds_profile_status_etype legacy_subs_to_db_subs(
  ds_profile_subs_etype subs,
  ds_profile_db_subs_type* db_subs)
{
  switch (subs)
  {
    case(DS_PROFILE_ACTIVE_SUBSCRIPTION_1):
      *db_subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_1;
      break;
    case(DS_PROFILE_ACTIVE_SUBSCRIPTION_2):
      *db_subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_2;
      break;
    case(DS_PROFILE_ACTIVE_SUBSCRIPTION_3):
      *db_subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_3;
      break;
    default:
      return DS_PROFILE_REG_RESULT_ERR_INVAL_SUBS_ID;
  }

  return DS_PROFILE_REG_RESULT_SUCCESS;
}

static ds_profile_db_ident_type legacy_ident_to_db_ident(
  ds_profile_identifier_type ident)
{
  switch (ident)
  {
    case (DS_PROFILE_3GPP_PROFILE_PARAM_PROFILE_NAME):
      return GENERAL_PROFILE_PARAM_PROFILE_NAME;        
    case (DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN):
    case (DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING):      
      return GENERAL_PROFILE_PARAM_APN_NAME;        
    case (DS_PROFILE_3GPP_PROFILE_PARAM_SUBS_ID):
      return GENERAL_PROFILE_PARAM_SUBS_ID;
    case (DS_PROFILE_GENERAL_PROFILE_PARAM_IPV6_PREFIX_DELEGATION_FLAG):
      return GENERAL_PROFILE_PARAM_IPV6_DELEGATION;
    case (DS_PROFILE_GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG):
      return GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG;  
    default:
      return ident;
  }
}

/*===========================================================================
FUNCTION db_status_to_ds_profile_status(
 
DESCRIPTION
  This function is used to map DS_PROFILE_RESULT_TYPE to
  DS_PROFILE_DB_RESULT_TYPE

PARAMETERS 
  db_status - DS_PROFILE_DB_RESULT_TYPE to convert to DS_PROFILE_RESULT_TYPE

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype db_status_to_ds_profile_status(
   ds_profile_db_result_type db_status
)
{
  return (ds_profile_status_etype) db_status;
}

/*===========================================================================
FUNCTION ds_profile_db_handler_dealloc
 
DESCRIPTION
  This function is used to free memory which was allocated for the local copy
  of the profile

PARAMETERS 
  prf_ptr - pointer to local copy of profile

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_dealloc(
  void *prf_ptr
)
{
  return db_status_to_ds_profile_status(ds_profile_db_dealloc(prf_ptr));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_set_param
 
DESCRIPTION
  This function is used to set parameter value in the local copy
  of the profile

PARAMETERS 
  tech     - technology 
  prf_ptr  - pointer to local copy of profile
  ident    - identifier whose value is to be set
  info_ptr - pointer to store value of identifier to be modified

DEPENDENCIES 

  
RETURN VALUE 
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_set_param( 
  ds_profile_tech_etype       tech,
  void                        *prf_ptr,
  ds_profile_identifier_type  ident,
  const ds_profile_info_type  *info_ptr
)
{
  ds_profile_db_info_type* new_info_ptr = (ds_profile_db_info_type*) info_ptr;
  ds_profile_db_ident_type new_ident = legacy_ident_to_db_ident(ident);

  switch (ident)
  {
    case(DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE):
      if (info_ptr->len == sizeof(ds_profile_3gpp_auth_pref_type))
      {
        new_info_ptr->len = 1;
      }
      break;

    case(DS_PROFILE_TECH_COMMON_PROFILE_PARAM_AUTH_PROTOCOL):
      if (info_ptr->len == sizeof(ds_profile_auth_pref_type))
      {
        new_info_ptr->len = 1;
      }
      break;

    default:
      break;
  }

  return db_status_to_ds_profile_status(
     ds_profile_db_set_param(prf_ptr, new_ident, new_info_ptr));  
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_param
 
DESCRIPTION
  This function is used to get parameter value from the local copy
  of the profile

PARAMETERS 
  tech     - technology 
  prf_ptr  - pointer to local copy of profile
  ident    - identifier whose value is to be set
  info_ptr - pointer to store value of identifier to be modified

DEPENDENCIES  
  
RETURN VALUE 
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_param( 
  ds_profile_tech_etype       tech,
  void                        *prf_ptr,
  ds_profile_identifier_type  ident,
  ds_profile_info_type        *info_ptr
)
{
  ds_profile_db_info_type* new_info_ptr = (ds_profile_db_info_type*) info_ptr;
  ds_profile_db_ident_type new_ident = legacy_ident_to_db_ident(ident);

  return db_status_to_ds_profile_status(
     ds_profile_db_get_param(prf_ptr, new_ident, new_info_ptr));    
}

/*===========================================================================
FUNCTION ds_profile_db_handler_create

DESCRIPTION
  This function sends the request to create a new profile
PARAMETERS 
  tech       - technology 
  config_ptr - various configuration parameters for the profile creation
  num_ptr    - pointer to store the returned profile number

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_create(
  ds_profile_tech_etype  tech,
  ds_profile_config_type *config_ptr,
  ds_profile_num_type    *num_ptr       
)
{
  ds_profile_db_profile_managed_type* prf_ptr = NULL;  
  ds_profile_status_etype result = DS_PROFILE_REG_RESULT_SUCCESS;  
  ds_profile_db_creation_params_type params;
  ds_profile_db_subs_type subs = (ds_profile_db_subs_type)ps_sys_get_default_data_subscription();


  ds_profile_db_init_creation_params(&params);
  if (NULL != config_ptr)
  {
    if (config_ptr->config_mask & DS_PROFILE_CONFIG_MASK_SUBS_ID)
    {
      subs = (ds_profile_db_subs_type)config_ptr->subs_id;
    }
  }
  prf_ptr = ds_profile_db_alloc(legacy_tech_to_techmask(tech), subs);
  if (NULL == prf_ptr) 
  {
    DS_PROFILE_LOGE("ds_profile_db_handler_create: "
      "could not allocate structure for the profile", 0);

    result = DS_PROFILE_REG_RESULT_FAIL;
    goto bail;
  }

  if (NULL != config_ptr)
  {    
    /* additional logic may be added here as config gets additional params */
    if (config_ptr->config_mask & DS_PROFILE_CONFIG_MASK_PERSISTENCE) 
    {
      params.persistent = config_ptr->is_persistent;
    }

    if (config_ptr->config_mask & DS_PROFILE_CONFIG_MASK_SUBS_ID) 
    {
      params.subscription_id = (ds_profile_db_subs_type) config_ptr->subs_id;
    }

    if (config_ptr->config_mask & DS_PROFILE_CONFIG_MASK_PROFILE_ID) 
    {
      params.profile_num = config_ptr->num;
    }

    if(config_ptr->config_mask & ~DS_PROFILE_CONFIG_MASK_ALL) 
    {
      DS_PROFILE_LOGE("ds_profile_db_handler_create: "
                      "unknown parameters in config 0x%x", 
                       config_ptr->config_mask);

      result = DS_PROFILE_REG_RESULT_ERR_INVAL;
      goto bail;
    }
  }
  
  result = db_status_to_ds_profile_status(
     ds_profile_db_create(&params, prf_ptr));

  if (DS_PROFILE_REG_RESULT_SUCCESS != result) 
  {
    goto bail;
  }

  *num_ptr = ds_profile_db_get_profile_num(prf_ptr);
  if (DB_UNSPECIFIED_PROFILE_NUM == *num_ptr)
  {
    result = DS_PROFILE_REG_RESULT_FAIL;
  } else 
  {
    result = DS_PROFILE_REG_RESULT_SUCCESS;
  }
  /* fall through */

bail:
  if (NULL != prf_ptr) 
  {
    (void)db_status_to_ds_profile_status(ds_profile_db_dealloc (prf_ptr));
  }
  
  return result;
}

/*===========================================================================
FUNCTION ds_profile_db_handler_delete

DESCRIPTION
  This function sends the request to release the profile
PARAMETERS 
  tech - technology
  num  - profile number
  boolean - if true, default profile can be deleted

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_delete(
  ds_profile_tech_etype tech,
  ds_profile_num_type   num,
  boolean enable_deleting_default_profile,
  ds_profile_subs_etype subs
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;
  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs, &db_subs)))
  {
    return res;
  }
  return db_status_to_ds_profile_status(ds_profile_db_delete(num, 
                              legacy_tech_to_techmask(tech), 
                                                             (enable_deleting_default_profile == TRUE),
                                                             db_subs));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_profile_read

DESCRIPTION
  This function is used to read a profile from modem to the local copy

PARAMETERS 
  tech    - technology
  num     - profile number
  prf_ptr - pointer to profile blob
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_profile_read(
  ds_profile_tech_etype tech,
  ds_profile_num_type   num,
  void *prf_ptr,
  ds_profile_subs_etype subs
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;
  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs, &db_subs)))
  {
    return res;
  }
  return db_status_to_ds_profile_status(ds_profile_db_read(num, legacy_tech_to_techmask(tech), db_subs, prf_ptr));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_profile_write

DESCRIPTION
  This function is used to write a profile to modem from the local copy

PARAMETERS 
  tech    - technology
  num     - profile number
  prf_ptr - pointer to profile blob
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_profile_write(
  ds_profile_tech_etype tech,
  ds_profile_num_type   num,
  void                  *prf_ptr
)
{
  return db_status_to_ds_profile_status(ds_profile_db_write(prf_ptr));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_reset_profile_to_default

DESCRIPTION
  This function is used to send a request to reset profile to default values.

PARAMETERS 
  tech - technology
  num  - profile number

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_reset_profile_to_default(  
  ds_profile_tech_etype  tech,
  ds_profile_num_type num,
  ds_profile_subs_etype subs
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;
  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs, &db_subs)))
  {
    return res;
  }
  return db_status_to_ds_profile_status(ds_profile_db_reset(num, legacy_tech_to_techmask(tech), db_subs));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_reset_param

DESCRIPTION
  This function is used to send a request to reset a particular identifier 
  value to default.

PARAMETERS 
  tech  - technology
  num   - profile number
  ident - identifier for the parameter

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_reset_param(
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_identifier_type  ident,
  ds_profile_subs_etype subs,
  boolean mark_invalid
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;
  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = legacy_subs_to_db_subs(subs, &db_subs)))
  {
    return res;
  }
  switch ( ident )
  {
    case DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS:
    case DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS_EXTENDED:   
    case DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS:
    case DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS_EXTENDED:   
    case DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ:     
    case DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN:
    case DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS:
    case DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS:
    case DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS:     
    case DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1:
    case DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2:
      return db_status_to_ds_profile_status(
         ds_profile_db_reset_profile_param(num, 
                                           legacy_tech_to_techmask(tech), 
                                          ident, db_subs, mark_invalid));
    case DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME:
    case DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD:
    case DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE:
      if (!mark_invalid) 
      {
         return db_status_to_ds_profile_status(
            ds_profile_db_reset_profile_param(num, 
                                              legacy_tech_to_techmask(tech), 
                                            ident, db_subs, mark_invalid));
      } else
      {
        return DS_PROFILE_REG_RESULT_ERR_INVAL_OP;
      }
    default:
      return DS_PROFILE_REG_RESULT_ERR_INVAL_OP;
  }

}

/*===========================================================================
FUNCTION ds_profile_db_handler_reset_pdp_context

DESCRIPTION
  This function is used to send a request to reset a pdp context 
  value to default.

PARAMETERS 
  tech - technology
  num  - profile number


DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_reset_pdp_context(
  ds_profile_tech_etype       tech,
  ds_profile_num_type num,
  ds_profile_subs_etype subs
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;
  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs, &db_subs)))
  {
    return res;
  }
  return db_status_to_ds_profile_status(ds_profile_db_reset_profile_param(num, legacy_tech_to_techmask(tech), TECH_3GPP_PROFILE_PARAM_APN_CLASS, db_subs, FALSE));
}



/*===========================================================================
FUNCTION ds_profile_db_handler_set_default_profile

DESCRIPTION
  This function is used to set default profile number for a particular
  family

PARAMETERS 
  tech   - technology
  family - type of profile (socket, rmnet, atcop)
  num    - profile number

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_set_default_profile(  
  ds_profile_tech_etype  tech,
  uint32                 family, 
  ds_profile_num_type    num
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;
  ds_profile_subs_etype subs_id = (ds_profile_subs_etype)ps_sys_get_default_data_subscription();

  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs_id, &db_subs)))
  {
    return res;
  }

  return db_status_to_ds_profile_status(
    ds_profile_db_set_default_profile_num_per_subs(num,
                                           (ds_profile_db_tech_type)tech, 
                                                   family,
                                                   db_subs));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_default_profile

DESCRIPTION
  This function is used to get default profile number for a particular
  family

PARAMETERS 
  tech    - technology
  family  - type of profile (socket, rmnet, atcop)
  num_ptr - profile number pointer

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_default_profile(
  ds_profile_tech_etype tech,
  uint32                family, 
  ds_profile_num_type   *num_ptr
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;
  ds_profile_subs_etype subs_id = (ds_profile_subs_etype)ps_sys_get_default_data_subscription();

  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs_id, &db_subs)))
  {
    return res;
  }

  return db_status_to_ds_profile_status(
    ds_profile_db_get_default_profile_num_per_subs(num_ptr,
                                                   (ds_profile_db_tech_type)tech,
                                                   family,
                                                   (ds_profile_db_subs_type)db_subs));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_set_default_profile_per_subs

DESCRIPTION
  This function is used to set default profile number for a particular
  family and subscription.

PARAMETERS
  tech    - technology
  family  - profile family  (embedded or tethered)
  subs_id - subscription id ( subs 1 or subs 2)
  num     - profile number

DEPENDENCIES 
  None
  
RETURN VALUE 
  
SIDE EFFECTS 
  None
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_set_default_profile_per_subs(
  ds_profile_tech_etype  tech,
  uint32                 family,
  ds_profile_subs_etype  subs_id,
  ds_profile_num_type    num
)
{ 
  ds_profile_db_subs_type db_subs; 
  ds_profile_status_etype res;

  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = legacy_subs_to_db_subs(subs_id, &db_subs)))
  {
    return res;
  }

  return db_status_to_ds_profile_status(
     ds_profile_db_set_default_profile_num_per_subs(num, 
                                                    (ds_profile_db_tech_type) tech, 
                                                    family, 
                                                    db_subs));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_default_profile_per_subs

DESCRIPTION
  This function is used to get default profile number for a particular
  family and subscription.

PARAMETERS
  tech    - technology
  family  - profile family  (embedded or tethered)
  subs_id - subscription id ( subs 1 or subs 2)
  num_ptr - profile number pointer

DEPENDENCIES 
  None
  
RETURN VALUE 
  
SIDE EFFECTS 
  None
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_default_profile_per_subs(
  ds_profile_tech_etype   tech,
  uint32                  family,
  ds_profile_subs_etype   subs_id,
  ds_profile_num_type     *num_ptr
)
{
  ds_profile_db_subs_type db_subs; 
  ds_profile_status_etype res;

  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs_id, &db_subs)))
  {
    return res;
  }

  return db_status_to_ds_profile_status(
     ds_profile_db_get_default_profile_num_per_subs(num_ptr, 
                                                    (ds_profile_db_tech_type)tech, 
                                                    family, 
                                                   (ds_profile_db_subs_type)db_subs));
}

/*===========================================================================
FUNCTION ds_profile_db_handler_validate_profile_num

DESCRIPTION
  This function is used to validate a profile number.

PARAMETERS 
  tech - technology
  num  - profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_validate_profile_num(
  ds_profile_tech_etype tech,
  ds_profile_num_type num,
  ds_profile_subs_etype subs
)
{
  ds_profile_db_profile_num_type min_num, max_num;

  ds_profile_status_etype res = (ds_profile_status_etype)
    ds_profile_db_get_profiles_range(legacy_tech_to_techmask(tech), 
                                     &min_num, &max_num);

  if (DS_PROFILE_REG_RESULT_SUCCESS != res)
  {
    return res;
  }

  if ( ( num > max_num ) || ( num < min_num ) )
  {
    return DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  return DS_PROFILE_REG_RESULT_SUCCESS;
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_num_range

DESCRIPTION
  This function is used to get the range of profile numbers

PARAMETERS 
  tech             - technology
  min_num, max_num - pointers to store range (min & max profile numbers)

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
void ds_profile_db_handler_get_num_range(
  ds_profile_tech_etype tech,
  uint16                *min, 
  uint16                *max
)
{
  ds_profile_db_result_type result = 
    ds_profile_db_get_profiles_range(legacy_tech_to_techmask(tech), min, max);

  if (DB_RESULT_SUCCESS != result)
  {
    *min = 0;
    *max = 0;
  }
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_list

DESCRIPTION
  This function is used to form a list of profile numbers depending on input
  value. (All profile numbers or search according to <key, value> pair)

PARAMETERS 
  tech     - technology
  hndl     - list handle
  lst_ptr  - pointer to return list
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_list(
  ds_profile_tech_etype  tech,
  ds_util_list_hndl_type hndl,
  ds_profile_subs_etype subs,
  ds_profile_list_type   *lst_ptr
)
{
  ds_profile_db_result_type res;
  unsigned int i;
  ds_profile_db_profile_managed_type** profiles = NULL;
  uint16 profiles_count;
  ds_profile_db_info_type info;
  ds_profile_db_list_info_type node;
  ds_profile_db_subs_type db_subs;

  memset(&node, 0, sizeof(node));

  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                       (ds_profile_db_result_type)
                                       legacy_subs_to_db_subs(subs, &db_subs)))
  {
    return (ds_profile_status_etype)res;
  }
  /* first get array of profiles matching the criteria */
  switch (lst_ptr->dfn)
  {
    case (DS_PROFILE_LIST_ALL_PROFILES):
      res = ds_profile_db_get_profiles(legacy_tech_to_techmask(tech),
                                       db_subs,
                                       &profiles,
                                       &profiles_count);
      break;
    case (DS_PROFILE_LIST_SEARCH_PROFILES):
      if (DS_PROFILE_TECH_3GPP == tech)
      {
        if ( (lst_ptr->ident != DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN) && 
             (lst_ptr->ident != DS_PROFILE_3GPP_PROFILE_PARAM_APN_CLASS) &&
#ifdef FEATURE_EMERGENCY_PDN
             (lst_ptr->ident != DS_PROFILE_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED) &&
#endif
            (lst_ptr->ident != DS_PROFILE_3GPP_PROFILE_PARAM_SUBS_ID))
        {
           /* backward compatibility, only these are supported for 3GPP technology */
           return DS_PROFILE_REG_RESULT_ERR_INVAL_OP;
        }
      }
      res = ds_profile_db_get_profiles_with_ident(legacy_tech_to_techmask(tech),
                                                  legacy_ident_to_db_ident(lst_ptr->ident),
                                                  (ds_profile_db_info_type*)&(lst_ptr->info),
                                                  db_subs,
                                                  &profiles, 
                                                  &profiles_count);
      break;
    default:
      return DS_PROFILE_REG_RESULT_ERR_INVAL_OP;
  }

  if (DB_RESULT_SUCCESS != res)
  {
    return (ds_profile_status_etype) res;
  }

  /* now add each profile to the list */
  if (!profiles_count)
  {
    res = DB_RESULT_LIST_END;
    goto bail;
  }

  for (i = 0; i < profiles_count; i++)
  {
    if (!profiles[i])
    {
      DS_PROFILE_LOGE("Error in getting profiles", 0);
      res = DB_RESULT_FAIL;
      goto bail;
    }
    node.profile_number = ds_profile_db_get_profile_num(profiles[i]);    
    info.buf = node.profile_name;
    info.len = sizeof(node.profile_name);
    if (DB_RESULT_SUCCESS != 
          (res = ds_profile_db_get_param(profiles[i], GENERAL_PROFILE_PARAM_PROFILE_NAME, &info)))
    {
      goto bail;
    }
    
    if (ds_util_list_add(hndl, &node, sizeof(node)) != DS_SUCCESS)
    {
      res = DB_RESULT_FAIL;
      goto bail;
    }
  }


  res = DB_RESULT_SUCCESS;
  /* fall through */

bail:
  if (profiles)
  {
    ds_profile_db_free_profiles (profiles, profiles_count);
  }

  return (ds_profile_status_etype)res;
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_list_node

DESCRIPTION
  This function is used to get info from a particular node in the list

PARAMETERS 
  tech          - technology
  hndl          - iterator handle
  list_info_ptr - pointer to store information to be returned
  
DEPENDENCIES 
  
RETURN VALUE 

SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_list_node(
  ds_profile_tech_etype      tech,
  ds_util_itr_hndl_type      hndl,
  ds_profile_list_info_type  *list_info_ptr
)
{
  ds_profile_db_list_info_type node;
  uint32 info_size = sizeof(node);
  int     result   = -1;

  /* Initialize node */
  memset(&node, 0, sizeof(ds_profile_db_list_info_type) );

  if ( ds_util_itr_get_data(hndl, (void *)&node, &info_size) != DS_SUCCESS)
  {
    DS_PROFILE_LOGE("_3gpp_get_list_node: unable to get node from list", 0);
    return DS_PROFILE_REG_RESULT_FAIL;
  }

  list_info_ptr->num = node.profile_number;

  if ( (list_info_ptr->name->len >= DS_PROFILE_STR_LEN( node.profile_name )) && 
       (list_info_ptr->name->buf != NULL) )
  {
    /* Clear user supplied list_info->name->buf before updating with new value */
    memset( (void *)list_info_ptr->name->buf, 0, list_info_ptr->name->len );

    result = strlcpy( (char *)list_info_ptr->name->buf, 
      (char *)node.profile_name, 
      list_info_ptr->name->len);

    if( (result < 0) || (result >= list_info_ptr->name->len) )
    {
      DS_PROFILE_LOGE("_3gpp_get_list_node: strlcpy failed result %d", result );
      return DS_PROFILE_REG_RESULT_FAIL;
    }
    list_info_ptr->name->len = result;
  } else
  {
    if (DS_PROFILE_TECH_3GPP2 == tech)
  {
      /* ignore this error for 3GPP2 for backward compatibility. 
         In 3GPP2 we are not interested in profile name, just update the correct len */
      list_info_ptr->name->len = DS_PROFILE_STR_LEN( node.profile_name );
      DS_PROFILE_LOGD("_3gpp_get_list_node: user supplied bad buffer for name in list_node, ignoring", 0);
      return DS_PROFILE_REG_RESULT_SUCCESS;
    }
    DS_PROFILE_LOGE("_3gpp_get_list_node: user supplied bad list_node", 0);
    return DS_PROFILE_REG_RESULT_ERR_INVAL;
  }

  return DS_PROFILE_REG_RESULT_SUCCESS;
}

/*===========================================================================
FUNCTION ds_profile_db_handler_cleanup

DESCRIPTION
  This function is used for the final cleanup

PARAMETERS 
  tech          - technology
  
DEPENDENCIES 
  
RETURN VALUE 

SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_cleanup(
  ds_profile_tech_etype      tech
)
{
  /* Should be called during global system shutdown */
  //ds_profile_db_deinit();

  return DS_PROFILE_REG_RESULT_SUCCESS;
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_tech_type

DESCRIPTION
  This function gets the tech type for the given profile num

PARAMETERS
  profile_num : profile number

DEPENDENCIES 
  
RETURN VALUE 
  ds_profile_tech_etype
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_tech_etype ds_profile_db_handler_get_tech_type(
  ds_profile_num_type profile_num,
  ds_profile_subs_etype subs
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;

  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs, &db_subs)))
  {
    //DEBUG add log msg
    return DS_PROFILE_TECH_INVALID;
  }
  return (ds_profile_tech_etype)ds_profile_db_get_tech_type(profile_num, db_subs);
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_persistence

DESCRIPTION
  This function gets the persistence for the given profile num

PARAMETERS
  profile_num : profile number
  is_persistent: persistence flag

DEPENDENCIES 
  
RETURN VALUE 
  ds_profile_status_etype
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_persistence(
  ds_profile_num_type profile_num,
  boolean *is_persistent,
  ds_profile_subs_etype subs
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;

  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs, &db_subs)))
  {
    return res;
  }

  return db_status_to_ds_profile_status(
    ds_profile_db_get_persistence(profile_num, db_subs, 
                                  is_persistent));
}


/*===========================================================================
FUNCTION ds_profile_db_handler_get_param
 
DESCRIPTION
  This function is used to get parameter value from the local copy
  of the profile

PARAMETERS 
  tech     - technology 
  prf_ptr  - pointer to local copy of profile
  ident    - identifier whose value is to be set
  info_ptr - pointer to store value of identifier to be modified

DEPENDENCIES  
  
RETURN VALUE 
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_param_in_use( 
  void                        *prf_ptr,
  ds_profile_identifier_type  ident,
  ds_profile_info_type        *info_ptr,
  ds_profile_tech_etype tech_type,
  ds_profile_subs_etype subs
)
{
  ds_profile_db_info_type* new_info_ptr = (ds_profile_db_info_type*) info_ptr;
  ds_profile_db_ident_type new_ident = legacy_ident_to_db_ident(ident);
  ds_profile_db_subs_type db_subs;

  legacy_subs_to_db_subs(subs, &db_subs);

  return db_status_to_ds_profile_status(
    ds_profile_db_get_param_in_use(prf_ptr, new_ident, new_info_ptr,
                                   (ds_profile_db_tech_type)tech_type, 
                                   db_subs));
}


/*===========================================================================
FUNCTION ds_profile_db_handler_get_tech_type

DESCRIPTION
  This function gets the validity of the profile given profile num, tech_type
  and subscription.

PARAMETERS
  profile_num : profile number
  tech_type   : technology of the profile
  subs_id     : subscription id
  is_valid    : flag to determine the validity

DEPENDENCIES 
  
RETURN VALUE 
  ds_profile_tech_etype
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_is_valid_profile(
  ds_profile_num_type profile_num, 
  ds_profile_subs_etype subs_id, 
  ds_profile_tech_etype tech_type, 
  boolean *is_valid
)
{
  ds_profile_db_subs_type db_subs;
  ds_profile_status_etype res;

  if (DS_PROFILE_REG_RESULT_SUCCESS != (res = 
                                        legacy_subs_to_db_subs(subs_id, &db_subs)))
  {
    return (ds_profile_status_etype)DS_PROFILE_TECH_INVALID;
  }
  return (ds_profile_status_etype)(ds_profile_db_is_valid_profile(profile_num, db_subs, 
                                        (ds_profile_db_tech_type)tech_type, is_valid) );
}

/*===========================================================================
FUNCTION ds_profile_db_handler_get_epc_support_val
 
DESCRIPTION
  This function is used to get the value if EPC profiles are supported
  by the modem

PARAMETERS 

DEPENDENCIES  
  
RETURN VALUE 
   TRUE:  if supported
   FALSE: otherwise
 
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_profile_db_handler_get_epc_support_val
(
  void 
)
{
   return ds_profile_db_get_epc_support_val();
}
