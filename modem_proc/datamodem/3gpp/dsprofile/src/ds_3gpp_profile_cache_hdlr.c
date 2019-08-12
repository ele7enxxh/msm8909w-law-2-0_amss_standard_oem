 /******************************************************************************
 @file    ds_3gpp_profile_cache_hdlr.c
 @brief   

  DESCRIPTION
  Tech specific implementation of 3GPP Profile Management  

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 - 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
****************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/13   vb      Perform operations pertaining to profile cache 
=============================================================================*/


#include "ds_3gpp_profile_cache_hdlr.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_nv_manager.h"
#include "dsumts_subs_mgr.h"
#include "ds_3gpp_profile_api.h"

extern ds_umts_pdp_profile_type
 *ds_umts_pdp_profile_cache_ptr[DS_SUBSCRIPTION_MAX]
    [DS_UMTS_MAX_PDP_PROFILE_NUM+1];
/*---------------------------------------------------------------
     Pointer to the memory allocation from heap.
---------------------------------------------------------------*/ 

static uint8 transient_profile_cnt[DS_SUBSCRIPTION_MAX] = {0};
static uint8 persistent_profile_cnt = 0;

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_OVERRIDE_PDP_TYPE

DESCRIPTION
  The function converts PDP type to Override PDP type

PARAMETERS:
  pdp_type

DEPENDENCIES
  None.

RETURN VALUE 
  ds_profile_db_3gpp_override_home_pdp_protocol

SIDE EFFECTS 
  None

===========================================================================*/
ds_profile_db_3gpp_override_home_pdp_protocol
  ds_3gpp_profile_cache_get_override_pdp_type
(
  ds_umts_pdp_type_enum_type pdp_type
)
{
  ds_profile_db_3gpp_override_home_pdp_protocol  override_pdp_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( pdp_type )
  {
    case DS_UMTS_PDP_IPV4:
      override_pdp_type = DB_3GPP_OVERRIDE_HOME_IPV4;
      break;
    case DS_UMTS_PDP_IPV6:
      override_pdp_type = DB_3GPP_OVERRIDE_HOME_IPV6;
      break;
    case DS_UMTS_PDP_IPV4V6:
      override_pdp_type = DB_3GPP_OVERRIDE_HOME_IPV4V6;
      break;
    default:
      override_pdp_type = DB_3GPP_OVERRIDE_HOME_IP_MAX;
      break;
  }
  return override_pdp_type;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_PDP_TYPE_FRM_OVERRIDE_PDP

DESCRIPTION
  The function converts Override PDP type to pdp type

PARAMETERS:
  override_pdp_type

DEPENDENCIES
  None.

RETURN VALUE 
  ds_umts_pdp_type_enum_type

SIDE EFFECTS 
  None

===========================================================================*/
ds_umts_pdp_type_enum_type
  ds_3gpp_profile_cache_get_pdp_type_frm_override_pdp
(
  ds_profile_db_3gpp_override_home_pdp_protocol  override_pdp_type
)
{
  ds_umts_pdp_type_enum_type  pdp_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( override_pdp_type )
  {
    case DB_3GPP_OVERRIDE_HOME_IPV4:
      pdp_type = DS_UMTS_PDP_IPV4;
      break;
    case DB_3GPP_OVERRIDE_HOME_IPV6:
      pdp_type = DS_UMTS_PDP_IPV6;
      break;
    case DB_3GPP_OVERRIDE_HOME_IPV4V6:
      pdp_type = DS_UMTS_PDP_IPV4V6;
      break;
    default:
      pdp_type = DS_UMTS_PDP_MAX;
      break;
  }
  return pdp_type;
}
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_CHECK_PROFILE_NUMBER

DESCRIPTION
This function checks the validity of profile number. It checks if the profile
number and profile family are in valid range and profile family is defined.
PARAMETERS
  profile_number - profile number passed. This profile number is a two byte
                   number. First byte is the profile family and the second
                   byte is the actual profile number.

DEPENDENCIES
  None

RETURN VALUE
  ds_umts_pdp_profile_status_etype

SIDE EFFECTS
  None
===========================================================================*/
static ds_umts_pdp_profile_status_etype ds_3gpp_profile_cache_check_profile_number
(
  ds_profile_db_profile_num_type  profile_number
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------------- 
    3gpp profiles are valid between 1-16 and greater than 100(EPC profiles).
    -------------------------------------------------------------------------------*/
  if( ((profile_number > 0) && (profile_number <= DS_UMTS_MAX_PDP_PROFILE_NUM)) ||
      (profile_number >= EPC_PROFILE_NUMBER_MIN))
  {
    return DS_UMTS_PDP_SUCCESS;
  }

  DS_3GPP_MSG1_ERROR("Profile number:%d invalid",profile_number);
  return DS_UMTS_PDP_INVALID_PROFILE_NUM;
}   

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_UMTS_PDP_PROFILE_PTR

DESCRIPTION
This function returns the UMTS PDP profile pointer

PARAMETERS
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean ds_3gpp_profile_cache_get_umts_pdp_profile_ptr
(
  ds_profile_db_profile_num_type              profile_number,
  const ds_profile_db_3gpp_tech_params_type  *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type        *gen_params_ptr,
  ds_umts_pdp_profile_type                  **ds_umts_pdp_profile_ptr
)
{
  ds_umts_pdp_profile_status_etype  is_profile_valid = DS_UMTS_PDP_FAIL;
  int8                              cache_index = -1;
  ds_umts_pdp_subs_e_type           subs_id
                                      = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ds_3gpp_tech_params_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR("Tech param is NULL");
    return FALSE;
  }

  if( gen_params_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR("Gen params is NULL");
    return FALSE;
  }

  subs_id
    = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params_ptr->subscription_id);

  is_profile_valid =ds_3gpp_profile_cache_check_profile_number(profile_number);
  if( is_profile_valid != DS_UMTS_PDP_SUCCESS )
  {
    return FALSE;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);
  if (cache_index <= 0)
  {
    return FALSE;
  }

  *ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);
  if( *ds_umts_pdp_profile_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL");
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_ALLOCATE_PROFILE_CACHE

DESCRIPTION
This function allocates memory for profile cache 
 
INPUT 
   poitner(OUT) that would be assigned upon succfull allocation of memory
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE ohterwise
 
SIDE EFFECTS
  None
===========================================================================*/
static boolean ds_3gpp_profile_cache_allocate_profile_cache
(
   ds_umts_pdp_profile_type **ds_umts_pdp_profile_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_umts_pdp_profile_ptr is NULL");
    return FALSE;
  }

  *ds_umts_pdp_profile_ptr = 
            (ds_umts_pdp_profile_type *)modem_mem_alloc(
            (sizeof(ds_umts_pdp_profile_type)), MODEM_MEM_CLIENT_DATA_CRIT);

  if (*ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("modem_mem_alloc failed to allocate memory for new profile");
    return FALSE;
  }

  return TRUE;
}  
 
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_DEALLOCATE_PROFILE_CACHE

DESCRIPTION
This function deallocates memory for profile cache 
 
DEPENDENCIES
  pointer to the profile cache structure

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
static void ds_3gpp_profile_cache_deallocate_profile_cache
(
   ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_umts_pdp_profile_ptr is NULL");
    return;
  }

  modem_mem_free(ds_umts_pdp_profile_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
}    

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_FREE_CACHE_INDEX

DESCRIPTION
This function loops through the cache and returns the free index based on 
valid_context flag. 
 
DEPENDENCIES
  None

RETURN VALUE
  int8 cache_index
  Return a cache index of - 1 if no free cache index is found.
 
SIDE EFFECTS
  None
===========================================================================*/
static int8 ds_3gpp_profile_cache_get_free_cache_index
(
   ds_umts_pdp_subs_e_type              subs_id
)
{
  int8 cache_index = -1;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------- 
    Start with index 1, since cache ranges from 1-24
    ----------------------------------------------------------------------*/

  if(ds_umts_pdp_check_subscription_id(subs_id) == DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return cache_index;
  }

  for (cache_index = 1; cache_index <= DS_UMTS_MAX_PDP_PROFILE_NUM;
        cache_index++)
  {
    ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

    if (ds_umts_pdp_profile_ptr == NULL)
    {
      DS_3GPP_MSG1_HIGH("Free cache index found:%d",cache_index);
      break;
    }
  }

  if (cache_index > DS_UMTS_MAX_PDP_PROFILE_NUM)
  {
    DS_3GPP_MSG0_HIGH("No free cache index");
    cache_index = -1;
  }

  if(cache_index !=-1 && 
     ds_3gpp_profile_cache_allocate_profile_cache(&ds_umts_pdp_profile_ptr))
  {
     DS_UMTS_PDP_PROFILE_CACHE_SET(subs_id, cache_index, ds_umts_pdp_profile_ptr);
  }

  return cache_index;
}   

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_CACHE_PROFILE_NUM_FROM_INDEX

DESCRIPTION
This function gets the profile_number from the cache for the index passed 
if the context_flag is valid 
 
DEPENDENCIES
  None

RETURN VALUE
  int8 profile_num
  -1 if invalid
 
SIDE EFFECTS
  None
===========================================================================*/
int16 ds_3gpp_profile_cache_get_cache_profile_num_from_index
(
   ds_umts_pdp_subs_e_type subs_id,
   uint8 cache_index
)
{
  int16 profile_num = -1;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_umts_pdp_check_subscription_id(subs_id) == DS_UMTS_PDP_INVALID_SUBS_ID)
   {
      return profile_num;
   }

  if (cache_index > DS_UMTS_MAX_PDP_PROFILE_NUM)
  {
    DS_3GPP_MSG1_ERROR("Cache index:%d is not valid",cache_index);
    return profile_num;
  }

  ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (!ds_umts_pdp_profile_ptr->context.valid_flg)
  {
    DS_3GPP_MSG1_ERROR("Cache index:%d is not valid",cache_index);
    return profile_num;
  }

  profile_num = ds_umts_pdp_profile_ptr->profile_number;

  return profile_num;
}   

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_MNC

DESCRIPTION
  The function updates the mnc value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
 None
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_mnc
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask 
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  /*----------------------------------------------------------------------- 
    If tech mask != EPC_PROFILE_TECH_MASK (not EPC) do a blind copy and also set the epc mask
    of the cache ptr, However for an EPC profile do a memcpy only if the
    epc_profile_tech_mask is not set
    ----------------------------------------------------------------------*/
  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
    memset(&(ds_umts_pdp_profile_ptr->mnc),0,sizeof(ds_umts_3gpp_mnc_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->mnc, sizeof(ds_umts_3gpp_mnc_type),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.mnc,
            sizeof(ds_umts_3gpp_mnc_type));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= MNC_MOD;
  }
  else if ((tech_mask == EPC_PROFILE_TECH_MASK) && 
           (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & MNC_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->mnc),0,sizeof(ds_umts_3gpp_mnc_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->mnc,sizeof(ds_umts_3gpp_mnc_type),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.mnc,
            sizeof(ds_umts_3gpp_mnc_type));
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_MCC

DESCRIPTION
  The function updates the mnc value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_mcc
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask 
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL");
    return;
  }

  /*----------------------------------------------------------------------- 
  If tech mask != EPC_PROFILE_TECH_MASK (not EPC) do a blind copy and also set the epc mask
  of the cache ptr, However for an EPC profile do a memcpy only if the
  epc_profile_tech_mask is not set
  ----------------------------------------------------------------------*/

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset(&(ds_umts_pdp_profile_ptr->mcc),0,sizeof(uint16));

    memscpy((void *)&ds_umts_pdp_profile_ptr->mcc,sizeof(uint16),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.mcc,
            sizeof(uint16));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= MCC_MOD;
  }

  else if ((tech_mask == EPC_PROFILE_TECH_MASK) && 
           (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & MCC_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->mcc),0,sizeof(uint16));

    memscpy((void *)&ds_umts_pdp_profile_ptr->mcc, sizeof(uint16),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.mcc,
            sizeof(uint16));

  }
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_APN_BEARER

DESCRIPTION
  The function updates the apn bearer value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_apn_bearer
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask 
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset(&(ds_umts_pdp_profile_ptr->apn_bearer),0,
         sizeof(ds_profile_db_3gpp_apn_bearer_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->apn_bearer, sizeof(uint8),
          (void *)&ds_3gpp_tech_params_ptr->apn_bearer,
          sizeof(uint8));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_SRC_STAT_DESC_MIN

DESCRIPTION
  The function updates the src stat descriptor value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_src_stat_desc_min
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask 
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset(&(ds_umts_pdp_profile_ptr->src_stat_desc_min),0,
         sizeof(ds_profile_db_3gpp_src_stat_desc_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->src_stat_desc_min, sizeof(uint32),
          (void *)&ds_3gpp_tech_params_ptr->src_stat_desc_min,
          sizeof(uint32));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_SRC_STAT_DESC_REQ

DESCRIPTION
  The function updates the src stat descriptor req value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_src_stat_desc_req
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask 
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset(&(ds_umts_pdp_profile_ptr->src_stat_desc_req),0,
         sizeof(ds_profile_db_3gpp_src_stat_desc_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->src_stat_desc_req, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->src_stat_desc_req,
          sizeof(uint32));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_APN_CLASS

DESCRIPTION
  The function updates the APN class descriptor value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_apn_class
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask 
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if(tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset(&(ds_umts_pdp_profile_ptr->apn_class),0,
           sizeof(ds_profile_db_apn_class_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->apn_class, sizeof(uint8),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_class,
            sizeof(uint8));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= APN_CLASS_MOD;

  }
  else if((tech_mask == EPC_PROFILE_TECH_MASK ) && 
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & APN_CLASS_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->apn_class),0,
           sizeof(ds_profile_db_apn_class_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->apn_class, sizeof(uint8),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_class,
            sizeof(uint8));

  }
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_INACTIVITY_TIMER_VAL

DESCRIPTION
  The function updates the inactivity timer value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_inactivity_timer_val
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset(&(ds_umts_pdp_profile_ptr->inactivity_timer_val),0,
         sizeof(ds_profile_db_3gpp_inactivity_timer_val_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->inactivity_timer_val, sizeof(uint32),
          (void *)&ds_3gpp_tech_params_ptr->inactivity_timer_val,
          sizeof(uint32));
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_OPERATOR_RESERVED_PCO

DESCRIPTION
  The function updates the operator reserevd pco value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_operartor_reserved_pco
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask 
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
     memset(&(ds_umts_pdp_profile_ptr->operator_reserved_pco),0,
            sizeof(uint16));

     memscpy((void *)&ds_umts_pdp_profile_ptr->operator_reserved_pco, 
             sizeof(uint16),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.op_pco_id,
             sizeof(uint16));

     ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PCO_ID_MOD;
  }
  else if((tech_mask == EPC_PROFILE_TECH_MASK) &&
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PCO_ID_MOD)))
  {
     memset(&(ds_umts_pdp_profile_ptr->operator_reserved_pco),0,
            sizeof(uint16));

     memscpy((void *)&ds_umts_pdp_profile_ptr->operator_reserved_pco, 
             sizeof(uint16),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.op_pco_id,
             sizeof(uint16));
  }

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_APN_DISABLE_FLAG

DESCRIPTION
  The function updates the APN disable value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_apn_disable_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
    memset(&(ds_umts_pdp_profile_ptr->apn_disable_flag),0,
           sizeof(ds_profile_db_apn_disable_flag_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->apn_disable_flag, sizeof(boolean),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_disable_flag,
            sizeof(boolean));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= APN_DISABLE_FLAG_MOD;

  }
  else if((tech_mask == EPC_PROFILE_TECH_MASK) &&
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & APN_DISABLE_FLAG_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->apn_disable_flag),0,
           sizeof(ds_profile_db_apn_disable_flag_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->apn_disable_flag, sizeof(boolean),
          (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_disable_flag,
          sizeof(boolean));
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_IM_CN_FLAG

DESCRIPTION
  The function updates the IM CN value to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_im_cn_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }


  memset(&(ds_umts_pdp_profile_ptr->im_cn_flag),0,
         sizeof(ds_profile_db_3gpp_im_cn_flag_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->im_cn_flag, sizeof(boolean),
          (void *)&ds_3gpp_tech_params_ptr->im_cn_flag,
          sizeof(boolean));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG

DESCRIPTION
  The function updates the request_pcscf_address_using_dhcp_flag to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_request_pcscf_address_using_dhcp_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

 if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
  memset(&(ds_umts_pdp_profile_ptr->request_pcscf_address_using_dhcp_flag),0,
           sizeof(ds_profile_db_req_dhcp_pcscf_address_flag_type ));

    memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_using_dhcp_flag, 
            sizeof(boolean),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            request_pcscf_address_using_dhcp_flag, sizeof(boolean));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= DHCP_PCSCF_ADDR_MOD;
}
  else if((tech_mask == EPC_PROFILE_TECH_MASK) && 
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & DHCP_PCSCF_ADDR_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->request_pcscf_address_using_dhcp_flag),0,
           sizeof(ds_profile_db_req_dhcp_pcscf_address_flag_type ));

    memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_using_dhcp_flag, 
            sizeof(boolean),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            request_pcscf_address_using_dhcp_flag,sizeof(boolean));
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_REQUEST_DNS_ADDRESS_USING_DHCP_FLAG

DESCRIPTION
  The function updates the request_dns_address_using_dhcp_flag to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_request_dns_address_using_dhcp_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

 if (tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset(&(ds_umts_pdp_profile_ptr->request_dns_address_using_dhcp_flag),0,
           sizeof(ds_profile_db_req_dhcp_dns_address_flag_type ));

    memscpy((void *)&ds_umts_pdp_profile_ptr->request_dns_address_using_dhcp_flag, 
            sizeof(boolean),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            request_dns_address_using_dhcp_flag, sizeof(boolean));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= DHCP_DNS_ADDR_MOD;
  }
  else if((tech_mask == EPC_PROFILE_TECH_MASK) && 
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & DHCP_DNS_ADDR_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->request_dns_address_using_dhcp_flag),0,
           sizeof(ds_profile_db_req_dhcp_dns_address_flag_type ));

    memscpy((void *)&ds_umts_pdp_profile_ptr->request_dns_address_using_dhcp_flag, 
            sizeof(boolean),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            request_dns_address_using_dhcp_flag,sizeof(boolean));
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_REQUEST_PCSCF_ADDRESS_FLAG

DESCRIPTION
  The function updates the request_pcscf_address_flag to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_request_pcscf_address_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset(&(ds_umts_pdp_profile_ptr->request_pcscf_address_flag),0,
           sizeof(ds_profile_db_req_pcscf_address_flag_type ));

    memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_flag, 
            sizeof(boolean),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            request_pcscf_address_flag,sizeof(boolean));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PCSCF_ADDR_MOD;
  }
  else if((tech_mask == EPC_PROFILE_TECH_MASK) && 
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PCSCF_ADDR_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->request_pcscf_address_flag),0,
           sizeof(ds_profile_db_req_pcscf_address_flag_type ));

    memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_flag, 
            sizeof(boolean),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            request_pcscf_address_flag,sizeof(boolean));
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_AUTH_TYPE

DESCRIPTION
  The function updates the auth type to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_auth_type
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
    memscpy( (void*)&ds_umts_pdp_profile_ptr->auth.auth_type, 
             sizeof(ds_profile_db_auth_protocol),
             (void *)&ds_3gpp_tech_params_ptr->
             tech_common_params.auth_info.auth_type,
             sizeof(ds_profile_db_auth_protocol));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= AUTH_TYPE_MOD;

  }
  else if ((tech_mask == EPC_PROFILE_TECH_MASK) && 
           (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_TYPE_MOD)))
  {

    memscpy( (void*)&ds_umts_pdp_profile_ptr->auth.auth_type, 
             sizeof(ds_profile_db_auth_protocol),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
             auth_info.auth_type,
             sizeof(ds_profile_db_auth_protocol));
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_AUTH_PASSWORD

DESCRIPTION
  The function updates the auth password to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_auth_password
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE ;
  sys_modem_as_id_e_type  as_subs_id = SYS_MODEM_AS_ID_NONE;
 
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params->subscription_id);

  as_subs_id = dsumts_subs_mgr_get_as_id(subs_id);

  /*-------------------------------------------------------------------
    Reset the previous password before updating new password.
  -------------------------------------------------------------------*/
  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
    memset((void *)ds_umts_pdp_profile_ptr->auth.password, 
           0, 
           DS_UMTS_MAX_QCPDP_STRING_LEN+1);

    strlcpy( (char *) ds_umts_pdp_profile_ptr->auth.password,
             (char *)ds_3gpp_tech_params_ptr->tech_common_params.auth_info.password,
             sizeof(ds_umts_pdp_profile_ptr->auth.password));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= AUTH_PASSWORD_MOD;
  }
  else if ((tech_mask == EPC_PROFILE_TECH_MASK) && 
           (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_PASSWORD_MOD)))
  {
    memset((void *)ds_umts_pdp_profile_ptr->auth.password, 
           0, 
           DS_UMTS_MAX_QCPDP_STRING_LEN+1);

    strlcpy( (char *) ds_umts_pdp_profile_ptr->auth.password,
             (char *)ds_3gpp_tech_params_ptr->tech_common_params.auth_info.password,
             sizeof(ds_umts_pdp_profile_ptr->auth.password));
  }

  ds_3gpp_profile_post_param_chg_cmd_to_ds_task((uint16)profile_number,
                                                as_subs_id,
                                                NULL,
                                                DS_3GPP_PROFILE_CLR_THROT);

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_AUTH_USERNAME

DESCRIPTION
  The function updates the auth username to cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_auth_username
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE ;
  sys_modem_as_id_e_type  as_subs_id = SYS_MODEM_AS_ID_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params->subscription_id);

  as_subs_id = dsumts_subs_mgr_get_as_id(subs_id);
  /*-------------------------------------------------------------------
    Reset the previous user name before updating the user name
  -------------------------------------------------------------------*/
  DS_3GPP_MSG3_HIGH("Changing AUTH USER NAME",0,0,0);
  DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"New User name : %s", 
           (char *) ds_3gpp_tech_params_ptr->tech_common_params.
           auth_info.username);

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
    memset((void *)ds_umts_pdp_profile_ptr->auth.username, 
           0, 
           DS_UMTS_MAX_QCPDP_STRING_LEN+1);

    strlcpy( (char *) ds_umts_pdp_profile_ptr->auth.username,
             (char *)ds_3gpp_tech_params_ptr->tech_common_params.
             auth_info.username,
             sizeof(ds_umts_pdp_profile_ptr->auth.username));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= AUTH_USERNAME_MOD;

  }
  else if ((tech_mask == EPC_PROFILE_TECH_MASK) && 
           (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_USERNAME_MOD)))
  {

    memset((void *)ds_umts_pdp_profile_ptr->auth.username, 
           0, 
           DS_UMTS_MAX_QCPDP_STRING_LEN+1);

    strlcpy( (char *) ds_umts_pdp_profile_ptr->auth.username,
             (char *)ds_3gpp_tech_params_ptr->tech_common_params.
             auth_info.username,
             sizeof(ds_umts_pdp_profile_ptr->auth.username));
  }

  ds_3gpp_profile_post_param_chg_cmd_to_ds_task((uint16)profile_number,
                                                as_subs_id,
                                                NULL,
                                                DS_3GPP_PROFILE_CLR_THROT);

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_QOS_REQ

DESCRIPTION
  The function updates the UMTS QOS REQ
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_umts_qos_req
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  boolean is_extended = FALSE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }
  
  memset((void *)&ds_umts_pdp_profile_ptr->qos_request_umts, 
         0, 
         sizeof(ds_umts_umts_qos_params_type));

  ds_umts_pdp_profile_ptr->qos_request_umts.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_req_umts;

  ds_3gpp_profile_cache_copy_umts_qos_params_from_db(&ds_umts_pdp_profile_ptr
    ->qos_request_umts,&ds_3gpp_tech_params_ptr->qos_request_umts, is_extended);

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_QOS_REQ_EXTENDED

DESCRIPTION
  The function updates the UMTS QOS REQ Extended
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_umts_qos_req_extended
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  boolean is_extended = TRUE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }
 

  memset((void *)&ds_umts_pdp_profile_ptr->qos_request_umts, 
         0, 
         sizeof(ds_umts_umts_qos_params_type) - sizeof(boolean));
  //Don't need the sig_ind(boolean) in Extended
  ds_umts_pdp_profile_ptr->qos_request_umts.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_req_umts;

  ds_3gpp_profile_cache_copy_umts_qos_params_from_db(&ds_umts_pdp_profile_ptr
    ->qos_request_umts,&ds_3gpp_tech_params_ptr->qos_request_umts, is_extended);

  return;

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_QOS_MIN

DESCRIPTION
  The function updates the UMTS QOS MIN params
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_umts_qos_min
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  boolean is_extended = FALSE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset((void *)&ds_umts_pdp_profile_ptr->qos_minimum_umts, 
         0, 
         sizeof(ds_umts_umts_qos_params_type));

  ds_umts_pdp_profile_ptr->qos_minimum_umts.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_min_umts;

  ds_3gpp_profile_cache_copy_umts_qos_params_from_db(&ds_umts_pdp_profile_ptr
   ->qos_minimum_umts,&ds_3gpp_tech_params_ptr->qos_minimum_umts, is_extended);

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_QOS_MIN_EXTENDED

DESCRIPTION
  The function updates the UMTS QOS MIN EXTENDED params
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_umts_qos_min_extended
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  boolean is_extended = TRUE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset((void *)&ds_umts_pdp_profile_ptr->qos_minimum_umts, 
         0, 
         sizeof(ds_umts_umts_qos_params_type) - sizeof(boolean)); 
  //Don't need the sig_ind(boolean) in Extended

  ds_umts_pdp_profile_ptr->qos_minimum_umts.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_min_umts;

  ds_3gpp_profile_cache_copy_umts_qos_params_from_db(&ds_umts_pdp_profile_ptr
   ->qos_minimum_umts,&ds_3gpp_tech_params_ptr->qos_minimum_umts, is_extended);

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_COPY_GPRS_QOS_PARAMS_TO_DB

DESCRIPTION
  The function copies lte qos params from DB to the cache
    
PARAMETERS: 
   ds_umts_gprs_qos_params_type *ds_umts_gprs_qos_param, 
   const ds_profile_db_3gpp_umts_gprs_qos_params_type *db_gprs_qos_param 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
===========================================================================*/
void ds_3gpp_profile_cache_copy_gprs_qos_params_to_db
(
   const ds_umts_gprs_qos_params_type *ds_umts_gprs_qos_param, 
   ds_profile_db_3gpp_umts_gprs_qos_params_type *db_gprs_qos_param
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_gprs_qos_param == NULL)
  {
    DS_3GPP_MSG0_ERROR("UMTS gprs qos param is NULL");
    return;
  }

  if (db_gprs_qos_param == NULL)
  {
    DS_3GPP_MSG0_ERROR("Profile DB GPRS param is NULL");
    return;
  }

  memscpy((void *)&db_gprs_qos_param->precedence, sizeof(uint32),
          (void *)&ds_umts_gprs_qos_param->precedence,
          sizeof(uint32));

  memscpy((void *)&db_gprs_qos_param->delay, sizeof(uint32),
          (void *)&ds_umts_gprs_qos_param->delay,
          sizeof(uint32));

  memscpy((void *)&db_gprs_qos_param->reliability, sizeof(uint32),
          (void *)&ds_umts_gprs_qos_param->reliability,
          sizeof(uint32));

  memscpy((void *)&db_gprs_qos_param->peak,  sizeof(uint32),
          (void *)&ds_umts_gprs_qos_param->peak,
          sizeof(uint32));

  memscpy((void *)&db_gprs_qos_param->mean,  sizeof(uint32),
          (void *)&ds_umts_gprs_qos_param->mean,
          sizeof(uint32));

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_COPY_GPRS_QOS_PARAMS_FROM_DB

DESCRIPTION
  The function copies lte qos params from DB to the cache
    
PARAMETERS: 
   ds_umts_gprs_qos_params_type *ds_umts_gprs_qos_param, 
   const ds_profile_db_3gpp_umts_gprs_qos_params_type *db_gprs_qos_param 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
===========================================================================*/
void ds_3gpp_profile_cache_copy_gprs_qos_params_from_db
(
   ds_umts_gprs_qos_params_type *ds_umts_gprs_qos_param, 
   const ds_profile_db_3gpp_umts_gprs_qos_params_type *db_gprs_qos_param
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_gprs_qos_param == NULL)
  {
    DS_3GPP_MSG0_ERROR("UMTS gprs qos param is NULL");
    return;
  }

  if (db_gprs_qos_param == NULL)
  {
    DS_3GPP_MSG0_ERROR("Profile DB GPRS param is NULL");
    return;
  }

  memscpy((void *)&ds_umts_gprs_qos_param->precedence, sizeof(uint32),
          (void *)&db_gprs_qos_param->precedence,sizeof(uint32));

  memscpy((void *)&ds_umts_gprs_qos_param->delay, sizeof(uint32),
          (void *)&db_gprs_qos_param->delay,sizeof(uint32));

  memscpy((void *)&ds_umts_gprs_qos_param->reliability, sizeof(uint32),
          (void *)&db_gprs_qos_param->reliability,sizeof(uint32));

  memscpy((void *)&ds_umts_gprs_qos_param->peak, sizeof(uint32),
          (void *)&db_gprs_qos_param->peak,sizeof(uint32));

  memscpy((void *)&ds_umts_gprs_qos_param->mean, sizeof(uint32),
          (void *)&db_gprs_qos_param->mean,sizeof(uint32));

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_COPY_UMTS_QOS_PARAMS_FROM_DB

DESCRIPTION
  The function copies lte qos params from DB to the cache
    
PARAMETERS: 
   ds_umts_qos_params_type *ds_umts_gprs_qos_param, 
  const ds_profile_db_3gpp_umts_qos_params_type *db_umts_qos_param 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
===========================================================================*/
void ds_3gpp_profile_cache_copy_umts_qos_params_to_db
(
   const ds_umts_umts_qos_params_type *ds_umts_qos_param, 
   ds_profile_db_3gpp_umts_qos_params_type *db_umts_qos_param
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_qos_param == NULL)
  {
    DS_3GPP_MSG0_ERROR("UMTS gprs qos param is NULL");
    return;
  }

  if (db_umts_qos_param == NULL)
  {
    DS_3GPP_MSG0_ERROR("Profile DB GPRS param is NULL");
    return;
  }

  memscpy((void *)&db_umts_qos_param->traffic_class, sizeof(ds_umts_qos_tclass_enum_type),
          (void *)&ds_umts_qos_param->traffic_class,
          sizeof(ds_umts_qos_tclass_enum_type));

  memscpy((void *)&db_umts_qos_param->max_ul_bitrate, sizeof(uint32),
          (void *)&ds_umts_qos_param->max_ul_bitrate,
          sizeof(uint32));

  memscpy((void *)&db_umts_qos_param->max_dl_bitrate, sizeof(uint32),
          (void *)&ds_umts_qos_param->max_dl_bitrate,
          sizeof(uint32));

  memscpy((void *)&db_umts_qos_param->gtd_ul_bitrate, sizeof(uint32),
          (void *)&ds_umts_qos_param->gtd_ul_bitrate,
          sizeof(uint32));

  memscpy((void *)&db_umts_qos_param->gtd_dl_bitrate, sizeof(uint32),
          (void *)&ds_umts_qos_param->gtd_dl_bitrate,
          sizeof(uint32));

  memscpy((void *)&db_umts_qos_param->dlvry_order, sizeof( ds_umts_qos_dorder_enum_type),
          (void *)&ds_umts_qos_param->dlvry_order,
          sizeof( ds_umts_qos_dorder_enum_type));

  memscpy((void *)&db_umts_qos_param->max_sdu_size, sizeof(uint32),
          (void *)&ds_umts_qos_param->max_sdu_size,
          sizeof(uint32));

  memscpy((void *)&db_umts_qos_param->sdu_err, sizeof(ds_umts_qos_sdu_error_enum_type),
          (void *)&ds_umts_qos_param->sdu_err,
          sizeof(ds_umts_qos_sdu_error_enum_type));

  memscpy((void *)&db_umts_qos_param->res_biterr, sizeof(ds_umts_qos_res_ber_enum_type),
          (void *)&ds_umts_qos_param->res_biterr,
          sizeof(ds_umts_qos_res_ber_enum_type));
          
  memscpy((void *)&db_umts_qos_param->dlvr_err_sdu, sizeof(ds_umts_qos_sdu_dlvr_enum_type),
          (void *)&ds_umts_qos_param->dlvr_err_sdu,
          sizeof(ds_umts_qos_sdu_dlvr_enum_type));

  memscpy((void *)&db_umts_qos_param->trans_delay, sizeof(uint32),
          (void *)&ds_umts_qos_param->trans_delay,
          sizeof(uint32));

  memscpy((void *)&db_umts_qos_param->thandle_prio, sizeof(uint32),
          (void *)&ds_umts_qos_param->thandle_prio,
          sizeof(uint32));

  memscpy((void *)&db_umts_qos_param->sig_ind, sizeof(boolean),
          (void *)&ds_umts_qos_param->sig_ind,
          sizeof(boolean));

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_COPY_UMTS_QOS_PARAMS_FROM_DB

DESCRIPTION
  The function copies lte qos params from DB to the cache
    
PARAMETERS: 
   ds_umts_qos_params_type *ds_umts_gprs_qos_param, 
   const ds_profile_db_3gpp_umts_qos_params_type *db_umts_qos_param 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
===========================================================================*/
void ds_3gpp_profile_cache_copy_umts_qos_params_from_db
(
   ds_umts_umts_qos_params_type *ds_umts_qos_param, 
   const ds_profile_db_3gpp_umts_qos_params_type *db_umts_qos_param,
   boolean is_extended
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_qos_param == NULL)
  {
    DS_3GPP_MSG0_ERROR("UMTS gprs qos param is NULL");
    return;
  }

  if (db_umts_qos_param == NULL)
  {
    DS_3GPP_MSG0_ERROR("Profile DB GPRS param is NULL");
    return;
  }

  memscpy((void *)&ds_umts_qos_param->traffic_class, sizeof(ds_umts_qos_tclass_enum_type),
          (void *)&db_umts_qos_param->traffic_class,
          sizeof(ds_umts_qos_tclass_enum_type));

  memscpy((void *)&ds_umts_qos_param->max_ul_bitrate, sizeof(uint32),
          (void *)&db_umts_qos_param->max_ul_bitrate,sizeof(uint32));

  memscpy((void *)&ds_umts_qos_param->max_dl_bitrate, sizeof(uint32),
          (void *)&db_umts_qos_param->max_dl_bitrate,sizeof(uint32));

  memscpy((void *)&ds_umts_qos_param->gtd_ul_bitrate, sizeof(uint32),
          (void *)&db_umts_qos_param->gtd_ul_bitrate,sizeof(uint32));

  memscpy((void *)&ds_umts_qos_param->gtd_dl_bitrate, sizeof(uint32),
          (void *)&db_umts_qos_param->gtd_dl_bitrate,sizeof(uint32));

  memscpy((void *)&ds_umts_qos_param->dlvry_order, sizeof( ds_umts_qos_dorder_enum_type),
          (void *)&db_umts_qos_param->dlvry_order,
          sizeof( ds_umts_qos_dorder_enum_type));

  memscpy((void *)&ds_umts_qos_param->max_sdu_size, sizeof(uint32),
          (void *)&db_umts_qos_param->max_sdu_size,sizeof(uint32));

  memscpy((void *)&ds_umts_qos_param->sdu_err, sizeof(ds_umts_qos_sdu_error_enum_type),
          (void *)&db_umts_qos_param->sdu_err,
          sizeof(ds_umts_qos_sdu_error_enum_type));

  memscpy((void *)&ds_umts_qos_param->res_biterr, sizeof(ds_umts_qos_res_ber_enum_type),
          (void *)&db_umts_qos_param->res_biterr,
          sizeof(ds_umts_qos_res_ber_enum_type));

  memscpy((void *)&ds_umts_qos_param->dlvr_err_sdu, sizeof(ds_umts_qos_sdu_dlvr_enum_type),
          (void *)&db_umts_qos_param->dlvr_err_sdu,
          sizeof(ds_umts_qos_sdu_dlvr_enum_type));

  memscpy((void *)&ds_umts_qos_param->trans_delay, sizeof(uint32),
          (void *)&db_umts_qos_param->trans_delay,
          sizeof(uint32));

  memscpy((void *)&ds_umts_qos_param->thandle_prio, sizeof(uint32),
          (void *)&db_umts_qos_param->thandle_prio,
          sizeof(uint32));

  if (!is_extended) 
  {
    memscpy((void *)&ds_umts_qos_param->sig_ind, sizeof(boolean),
            (void *)&db_umts_qos_param->sig_ind,
            sizeof(boolean));
  }

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_COPY_LTE_QOS_PARAMS_to_DB

DESCRIPTION
  The function copies lte qos params to the cache from DB
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
===========================================================================*/
void ds_3gpp_profile_cache_copy_lte_qos_params_to_db
(
   const ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr, 
   ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache ptr is NULL");
    return;
  }

  if (ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Tech params ptr is NULL");
    return;
  }

  ds_3gpp_tech_params_ptr->is_valid.qos_req_lte =
    ds_umts_pdp_profile_ptr->qos_request_lte.valid_flg;

  memscpy((void *)&ds_3gpp_tech_params_ptr->qos_request_lte.qci, sizeof(ds_3gpp_lte_qci_e_type),
          (void *)&ds_umts_pdp_profile_ptr->qos_request_lte.qci,
          sizeof(ds_3gpp_lte_qci_e_type));

  memscpy((void *)&ds_3gpp_tech_params_ptr->qos_request_lte.g_dl_bit_rate, sizeof(uint32),
          (void *)&ds_umts_pdp_profile_ptr->qos_request_lte.g_dl_bit_rate,
          sizeof(uint32));

  memscpy((void *)&ds_3gpp_tech_params_ptr->qos_request_lte.max_dl_bit_rate, sizeof(uint32),
          (void *)&ds_umts_pdp_profile_ptr->qos_request_lte.max_dl_bit_rate,
          sizeof(uint32));

  memscpy((void *)&ds_3gpp_tech_params_ptr->qos_request_lte.g_ul_bit_rate, sizeof(uint32),
          (void *)&ds_umts_pdp_profile_ptr->qos_request_lte.g_ul_bit_rate,
          sizeof(uint32));

  memscpy((void *)&ds_3gpp_tech_params_ptr->qos_request_lte.max_ul_bit_rate, sizeof(uint32),
          (void *)&ds_umts_pdp_profile_ptr->qos_request_lte.max_ul_bit_rate,
          sizeof(uint32));

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_COPY_LTE_QOS_PARAMS_FROM_DB

DESCRIPTION
  The function copies lte qos params from DB to the cache
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
===========================================================================*/
void ds_3gpp_profile_cache_copy_lte_qos_params_from_db
(
   ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr, 
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache ptr is NULL");
    return;
  }

  if (ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Tech params ptr is NULL");
    return;
  }

  ds_umts_pdp_profile_ptr->qos_request_lte.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_req_lte;

  memscpy((void *)&ds_umts_pdp_profile_ptr->qos_request_lte.qci, sizeof(ds_3gpp_lte_qci_e_type),
          (void *)&ds_3gpp_tech_params_ptr->qos_request_lte.qci,
          sizeof(ds_3gpp_lte_qci_e_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->qos_request_lte.g_dl_bit_rate, sizeof(uint32),
          (void *)&ds_3gpp_tech_params_ptr->qos_request_lte.g_dl_bit_rate,
          sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->qos_request_lte.max_dl_bit_rate, sizeof(uint32),
          (void *)&ds_3gpp_tech_params_ptr->qos_request_lte.max_dl_bit_rate,
          sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->qos_request_lte.g_ul_bit_rate, sizeof(uint32),
          (void *)&ds_3gpp_tech_params_ptr->qos_request_lte.g_ul_bit_rate,
          sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->qos_request_lte.max_ul_bit_rate, sizeof(uint32),
          (void *)&ds_3gpp_tech_params_ptr->qos_request_lte.max_ul_bit_rate,
          sizeof(uint32));

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_LTE_QOS_REQ

DESCRIPTION
  The function updates the LTE QOS params
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_lte_qos_req
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE ;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (gen_params == NULL)
  {
    DS_3GPP_MSG3_ERROR("Gen params is NULL",0,0,0);
    return;
  }

  if (ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Tech param is NULL",0,0,0);
    return;
  }

  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params->subscription_id);

  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);

  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    DS_3GPP_MSG3_ERROR("Profile number:%d invalid",profile_number,0,0);
    return;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
    DS_3GPP_MSG3_ERROR("Cache index is invalid:%d",cache_index,0,0);
    return;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Cache Ptr is NULL",0,0,0);
    return;
  }

  memset((void *)&ds_umts_pdp_profile_ptr->qos_request_lte, 
         0, 
         sizeof(ds_3gpp_lte_qos_params_type));

  ds_3gpp_profile_cache_copy_lte_qos_params_from_db(ds_umts_pdp_profile_ptr,
                                                    ds_3gpp_tech_params_ptr);

  return;

}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_GPRS_QOS_REQ

DESCRIPTION
  The function updates the GPRS QOS REQ params
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_gprs_qos_req
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset((void *)&ds_umts_pdp_profile_ptr->qos_request_gprs, 
         0, 
         sizeof(ds_umts_gprs_qos_params_type));

  ds_umts_pdp_profile_ptr->qos_request_gprs.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_req_gprs;

  ds_3gpp_profile_cache_copy_gprs_qos_params_from_db(&ds_umts_pdp_profile_ptr
                 ->qos_request_gprs,&ds_3gpp_tech_params_ptr->qos_request_gprs);

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_GPRS_QOS_MIN

DESCRIPTION
  The function updates the GPRS QOS MIN params
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_gprs_qos_min
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset((void *)&ds_umts_pdp_profile_ptr->qos_minimum_gprs, 
         0, 
         sizeof(ds_umts_gprs_qos_params_type));

  ds_umts_pdp_profile_ptr->qos_minimum_gprs.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_min_gprs;

  ds_3gpp_profile_cache_copy_gprs_qos_params_from_db(&ds_umts_pdp_profile_ptr
                ->qos_minimum_gprs,&ds_3gpp_tech_params_ptr->qos_minimum_gprs);

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_SUBSCRIPTION_ID

DESCRIPTION
  The function updates the subscription id
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_subscription_id
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->subscription_id, sizeof(ds_profile_db_subs_type),
     (void *)&gen_params->subscription_id,
        sizeof(ds_profile_db_subs_type));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PROFILE_NAME

DESCRIPTION
  The function updates the profile name
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_profile_name
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

    /*-------------------------------------------------------------------
    Reset the previous profile name before updating new profile name.
  -------------------------------------------------------------------*/
  memset((void *)ds_umts_pdp_profile_ptr->profile_name, 
         0, 
         DS_UMTS_MAX_PROFILE_NAME_LEN+1);
  
  /*lint -e534 ignoring return code from function*/
  strlcpy((char *)ds_umts_pdp_profile_ptr->profile_name,(char *)
          gen_params->profile_name,
          sizeof(gen_params->profile_name));
  /*lint +e534*/

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_OVERRIDE_HOME_PDP_TYPE

DESCRIPTION
  The function updates the Override Home pdp type

PARAMETERS: 
  profile_number  --  Profile number passed by profile DB
  *info           --  Contains info about the parameters

DEPENDENCIES
  None.

RETURN VALUE 
ds_3gpp_profile_cache_update_enum_type
 DS_3GPP_PROFILE_CACHE_UPDATE_SUCCESS
   - If param value is updated successfully
 DS_3GPP_PROFILE_CACHE_UPDATE_WITH_NO_CHANGE
 - If param new value is same as old param value
 DS_3GPP_PROFILE_CACHE_UPDATE_WITH_ERROR
 - If something else is wrong with the call

SIDE EFFECTS 
  None

===========================================================================*/
void  ds_3gpp_profile_cache_write_override_home_pdp_type
(
  ds_profile_db_profile_num_type             profile_number,
  const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type       *gen_params,
  ds_profile_db_supported_tech_mask          tech_mask
)
{
  ds_umts_pdp_profile_type   *ds_umts_pdp_profile_ptr =NULL;
  ds_umts_pdp_type_enum_type  pdp_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  pdp_type = ds_3gpp_profile_cache_get_pdp_type_frm_override_pdp(
               ds_3gpp_tech_params_ptr->override_home_ip_type);

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset(&(ds_umts_pdp_profile_ptr->context.override_home_pdp_type),
         0,
         sizeof(ds_umts_pdp_type_enum_type));

  ds_umts_pdp_profile_ptr->context.override_home_pdp_type = pdp_type;

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_TYPE

DESCRIPTION
  The function updates the pdp type
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdp_type
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  /*----------------------------------------------------------------------- 
  If tech mask != EPC_PROFILE_TECH_MASK (not EPC) do a blind copy and also set the epc mask
  of the cache ptr, However for an EPC profile do a memcpy only if the
  epc_profile_tech_mask is not set
  ----------------------------------------------------------------------*/

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset(&(ds_umts_pdp_profile_ptr->context.pdp_type),0,
           sizeof(ds_umts_pdp_type_enum_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_type, 
          sizeof(ds_umts_pdp_type_enum_type),
          (void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_type,
           sizeof(ds_umts_pdp_type_enum_type));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PDP_TYPE_MOD;
  }
  else if ((tech_mask == EPC_PROFILE_TECH_MASK) && 
           (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PDP_TYPE_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->context.pdp_type),0,
           sizeof(ds_umts_pdp_type_enum_type));
    
    switch (ds_3gpp_tech_params_ptr->tech_common_params.pdp_type) 
    {
      case DB_PDP_TYPE_IPV4:
        ds_umts_pdp_profile_ptr->context.pdp_type = DS_UMTS_PDP_IPV4;
        break;
      case DB_PDP_TYPE_IPV6:
        ds_umts_pdp_profile_ptr->context.pdp_type = DS_UMTS_PDP_IPV6;
        break;
      case DB_PDP_TYPE_IPV4V6:
        ds_umts_pdp_profile_ptr->context.pdp_type = DS_UMTS_PDP_IPV4V6;
        break;
      default:
       memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_type, 
                sizeof(ds_umts_pdp_type_enum_type),
                (void *)&ds_3gpp_tech_params_ptr->tech_common_params.pdp_type,
                sizeof(ds_umts_pdp_type_enum_type));
       break;
    }
  }

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_LTE_ROAMING_PDP_TYPE

DESCRIPTION
  The function updates the LTE roaming pdp type
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_lte_roaming_pdp_type
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE ;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (gen_params == NULL)
  {
    DS_3GPP_MSG3_ERROR("Gen params is NULL",0,0,0);
    return;
  }

  if (ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Tech param is NULL",0,0,0);
    return;
  }

  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params->subscription_id);

  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);

  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    DS_3GPP_MSG3_ERROR("Profile number:%d invalid",profile_number,0,0);
    return;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
    DS_3GPP_MSG3_ERROR("Cache index is invalid:%d",cache_index,0,0);
    return;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Cache Ptr is NULL",0,0,0);
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset(&(ds_umts_pdp_profile_ptr->context.lte_roaming_pdp_type),0,
         sizeof(ds_umts_pdp_type_enum_type));
    
  switch (ds_3gpp_tech_params_ptr->lte_pdp_type) 
  {
    case DB_3GPP_ROAMING_IPV4:
      ds_umts_pdp_profile_ptr->context.lte_roaming_pdp_type = DS_UMTS_PDP_IPV4;
      break;
    case DB_3GPP_ROAMING_IPV6:
      ds_umts_pdp_profile_ptr->context.lte_roaming_pdp_type = DS_UMTS_PDP_IPV6;
      break;
   case DB_3GPP_ROAMING_IPV4V6:
     ds_umts_pdp_profile_ptr->context.lte_roaming_pdp_type = DS_UMTS_PDP_IPV4V6;
     break;
  default:
      ds_umts_pdp_profile_ptr->context.lte_roaming_pdp_type = DS_UMTS_PDP_MAX;
      break;
  }

}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_ROAMING_PDP_TYPE

DESCRIPTION
  The function updates the UMTS roaming pdp type
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_umts_roaming_pdp_type
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE ;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (gen_params == NULL)
  {
    DS_3GPP_MSG3_ERROR("Gen params is NULL",0,0,0);
    return;
  }

  if (ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Tech param is NULL",0,0,0);
    return;
  }

  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params->subscription_id);

  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);

  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    DS_3GPP_MSG3_ERROR("Profile number:%d invalid",profile_number,0,0);
    return;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
    DS_3GPP_MSG3_ERROR("Cache index is invalid:%d",cache_index,0,0);
    return;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Cache Ptr is NULL",0,0,0);
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset(&(ds_umts_pdp_profile_ptr->context.umts_roaming_pdp_type),0,
         sizeof(ds_umts_pdp_type_enum_type));
    
  switch (ds_3gpp_tech_params_ptr->umts_pdp_type) 
  {
    case DB_3GPP_ROAMING_IPV4:
      ds_umts_pdp_profile_ptr->context.umts_roaming_pdp_type = DS_UMTS_PDP_IPV4;
      break;
    case DB_3GPP_ROAMING_IPV6:
      ds_umts_pdp_profile_ptr->context.umts_roaming_pdp_type = DS_UMTS_PDP_IPV6;
      break;
   case DB_3GPP_ROAMING_IPV4V6:
     ds_umts_pdp_profile_ptr->context.umts_roaming_pdp_type = DS_UMTS_PDP_IPV4V6;
     break;
  default:
     ds_umts_pdp_profile_ptr->context.umts_roaming_pdp_type = DS_UMTS_PDP_MAX;
      break;
  }

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_APN

DESCRIPTION
  The function updates the pdp context apn
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_pdp_context_apn
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset((void *)ds_umts_pdp_profile_ptr->context.apn,0,
     DS_UMTS_MAX_APN_STRING_LEN+1);

  /*lint -e534 ignoring return code from function*/
  strlcpy((char *)ds_umts_pdp_profile_ptr->context.apn,
          gen_params->apn_name,
          sizeof(gen_params->apn_name));
  /*lint +e534*/

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_IPV4_ADDR_ALLOC

DESCRIPTION
  The function updates the pdp_context_ipv4_addr_alloc
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_pdp_context_ipv4_addr_alloc
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }
  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;


  memscpy((void *)&ds_umts_pdp_profile_ptr->context.ipv4_addr_alloc, 
          sizeof(ds_umts_pdp_ipv4_addr_alloc_e_type),
          (void *)&ds_3gpp_tech_params_ptr->pdp_context.ipv4_addr_alloc,
          sizeof(ds_umts_pdp_ipv4_addr_alloc_e_type));

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_D_COMP

DESCRIPTION
  The function updates the pdp_context_d_comp
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_pdp_context_d_comp
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.d_comp,
          sizeof(ds_umts_pdp_header_comp_e_type),
          (void *)&ds_3gpp_tech_params_ptr->pdp_context.d_comp,
           sizeof(ds_umts_pdp_header_comp_e_type));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_h_COMP

DESCRIPTION
  The function updates the pdp_context_h_comp
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_pdp_context_h_comp
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.h_comp, 
          sizeof(ds_umts_pdp_header_comp_e_type),
          (void *)&ds_3gpp_tech_params_ptr->pdp_context.h_comp,
          sizeof(ds_umts_pdp_header_comp_e_type));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_SECONDARY_FLAG

DESCRIPTION
  The function updates the pdp_context_secondary_flag
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdp_context_secondary_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.secondary_flag, 
          sizeof(boolean),
         (void *)&ds_3gpp_tech_params_ptr->pdp_context.secondary_flag,
         sizeof(boolean));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_NUMBER

DESCRIPTION
  The function updates the pdp_context_number
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdp_context_number
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset((void *)&ds_umts_pdp_profile_ptr->context.pdp_context_number,0,
     sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_context_number, 
          sizeof(byte),
         (void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_context_number,
         sizeof(byte));

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_ACCESS_CTRL_FLAG

DESCRIPTION
  The function updates the access control flag
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_access_ctrl_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset((void *)&ds_umts_pdp_profile_ptr->context.access_ctrl_flag,0,
     sizeof(ds_umts_pdp_access_ctrl_e_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.access_ctrl_flag, 
          sizeof(ds_umts_pdp_access_ctrl_e_type),
          (void *)&ds_3gpp_tech_params_ptr->pdp_context.access_ctrl_flag,
          sizeof(ds_umts_pdp_access_ctrl_e_type));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V4_DNS_ADDR_PRIMARY

DESCRIPTION
  The function updates the v4 primary dns address
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_v4_dns_addr_primary
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
) 
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.
       pdp_addr.pdp_addr_ipv4,0,sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.pdp_addr.
            pdp_addr_ipv4, sizeof(ds_profile_db_ip_addr_v4_type), 
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            dns_addr.primary_addr.v4.addr, sizeof(ds_profile_db_ip_addr_v4_type));

    ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.ip_vsn =
         (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_ip_version;

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PRIMARY_DNS_ADDR_MOD;

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PRIMARY_DNS_IP_MOD;

  }
  else if ((tech_mask == EPC_PROFILE_TECH_MASK) && 
           (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PRIMARY_DNS_ADDR_MOD)))
  {

    memset((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.
       pdp_addr.pdp_addr_ipv4,0,sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.pdp_addr.
       pdp_addr_ipv4, sizeof(ds_profile_db_ip_addr_v4_type), 
      (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.primary_addr.v4.addr, sizeof(ds_profile_db_ip_addr_v4_type));

    ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.ip_vsn =
       (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_ip_version;

  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_PRIMARY_ID

DESCRIPTION
  The function updates the pdp_context_primary_id
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdp_context_primary_id
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset((void *)&ds_umts_pdp_profile_ptr->context.primary_id,0,
     sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.primary_id, sizeof(byte),
      (void *)&ds_3gpp_tech_params_ptr->pdp_context.primary_id,
      sizeof(byte));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V4_DNS_ADDR_SECONDARY

DESCRIPTION
  The function updates the v4 secondary dns address
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_v4_dns_addr_secondary
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.
      pdp_addr.pdp_addr_ipv4,0,sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.
       pdp_addr_ipv4, sizeof(ds_profile_db_ip_addr_v4_type), 
       (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.secondary_addr.v4, sizeof(ds_profile_db_ip_addr_v4_type));

    ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.ip_vsn =
       (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.secondary_ip_version;

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= SEC_DNS_ADDR_MOD;
    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= SEC_DNS_IP_MOD;
  }
  else if((tech_mask == EPC_PROFILE_TECH_MASK) && 
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & SEC_DNS_ADDR_MOD)))
  {
    memset((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.
    pdp_addr.pdp_addr_ipv4,0,sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.
    pdp_addr_ipv4, sizeof(ds_profile_db_ip_addr_v4_type), 
    (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
    dns_addr.secondary_addr.v4, sizeof(ds_profile_db_ip_addr_v4_type));

    ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.ip_vsn =
       (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.secondary_ip_version;

  } 
} 

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V6_DNS_ADDR_PRIMARY

DESCRIPTION
  The function updates the v6 primary dns address
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_v6_dns_addr_primary
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }


  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.
         pdp_addr.pdp_addr_ipv6.in6_u,0,
         sizeof(ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.
        pdp_addr.pdp_addr_ipv6.in6_u));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.pdp_addr.
     pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), 
     (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
     dns_addr.primary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type));

    ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.ip_vsn =
       (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_ip_version;

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PRIMARY_DNS_ADDR_MOD;
    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PRIMARY_DNS_IP_MOD;
  }
  else if((tech_mask == EPC_PROFILE_TECH_MASK) && 
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PRIMARY_DNS_ADDR_MOD)))
  {

    memset((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.
            pdp_addr.pdp_addr_ipv6.in6_u,0,
            sizeof(ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.
            pdp_addr.pdp_addr_ipv6.in6_u));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.pdp_addr.
            pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), 
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            dns_addr.primary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type));

    ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.ip_vsn =
        (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_ip_version;

  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V6_DNS_ADDR_SECONDARY

DESCRIPTION
  The function updates the v6 secondary dns address
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_v6_dns_addr_secondary
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
    memset((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.
           pdp_addr.pdp_addr_ipv6.in6_u,0,
           sizeof(ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.
           pdp_addr.pdp_addr_ipv6.in6_u));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.
            pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), 
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            dns_addr.secondary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type));
   
    ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.ip_vsn = 
         (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.secondary_ip_version;

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= SEC_DNS_ADDR_MOD;
    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= SEC_DNS_IP_MOD;

  }
  else if ((tech_mask == EPC_PROFILE_TECH_MASK) &&
         (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & SEC_DNS_ADDR_MOD)))
  {
    memset((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.
           pdp_addr.pdp_addr_ipv6.in6_u,0,
           sizeof(ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.
                  pdp_addr.pdp_addr_ipv6.in6_u));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.
       pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.secondary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type));
     
    ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.ip_vsn = 
       (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.secondary_ip_version;
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V6_PDP_ADDR

DESCRIPTION
  The function updates the v6 pdp address of the pdp context
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdp_context_v6_pdp_addr
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset((void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.pdp_addr.
         pdp_addr_ipv6.in6_u,0,
         sizeof(ds_umts_pdp_profile_ptr->context.pdp_addr.pdp_addr.
                pdp_addr_ipv6.in6_u));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.
         pdp_addr.pdp_addr_ipv6.in6_u, sizeof(ds_umts_pdp_addr_type_ipv6),
         (void *)&ds_3gpp_tech_params_ptr->pdp_context.
         pdp_addr.addr.v6,
         sizeof(ds_umts_pdp_addr_type_ipv6));

  ds_umts_pdp_profile_ptr->context.pdp_addr.ip_vsn = 
    (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->pdp_context.pdp_addr.ip_vsn;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V4_PDP_ADDR

DESCRIPTION
  The function updates the v4 pdp address of the pdp context
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdp_context_v4_pdp_addr
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  memset((void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.pdp_addr.pdp_addr_ipv4
      ,0,sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.
      pdp_addr.pdp_addr_ipv4, sizeof(uint32),
     (void *)&ds_3gpp_tech_params_ptr->pdp_context.
     pdp_addr.addr.v4,sizeof(uint32));

  ds_umts_pdp_profile_ptr->context.pdp_addr.ip_vsn = 
    (ds_ip_version_enum_type)ds_3gpp_tech_params_ptr->pdp_context.pdp_addr.ip_vsn;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_EMERGENCY_FLAG

DESCRIPTION
  The function updates the emergency flag info
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_emergency_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->emergency_calls_are_supported,
          sizeof(boolean),
          (void *)&ds_3gpp_tech_params_ptr->emergency_calls_are_supported,
          sizeof(boolean));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_LINGER_PARAMS

DESCRIPTION
  The function updates the linger parameters
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_linger_params
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
    memset((void *)&ds_umts_pdp_profile_ptr->iface_linger_params,0,
     sizeof(ds_umts_linger_params_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->iface_linger_params.linger_timeout_val, 
            sizeof(uint16),
            (void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
            .linger_params.linger_timeout_val,
            sizeof(uint16));

    ds_umts_pdp_profile_ptr->iface_linger_params.valid_flag = 
    ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
    .is_valid;

    ds_umts_pdp_profile_ptr->iface_linger_params.allow_linger_flag = 
    ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
         .linger_params.allow_linger_flag;

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= LINGER_PARAMS_MOD;

  }
  else if((tech_mask == EPC_PROFILE_TECH_MASK) && 
          (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & LINGER_PARAMS_MOD)))
  {
    memset((void *)&ds_umts_pdp_profile_ptr->iface_linger_params,0,
       sizeof(ds_umts_linger_params_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->iface_linger_params.linger_timeout_val, sizeof(uint16),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
           .linger_params.linger_timeout_val,
           sizeof(uint16));

    ds_umts_pdp_profile_ptr->iface_linger_params.valid_flag = 
      ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
           .is_valid;

    ds_umts_pdp_profile_ptr->iface_linger_params.allow_linger_flag = 
      ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
           .linger_params.allow_linger_flag;

  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_TFT_FILTER_ID1

DESCRIPTION
  The function updates the tft filter id parameters
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_tft_filter_id1
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  uint8 index = 0,tft_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset((void *)(ds_umts_pdp_profile_ptr->tft + index),0,
     sizeof(ds_umts_tft_params_type));

  ds_umts_pdp_profile_ptr->tft[tft_index].valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.tft[tft_index];

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].filter_id, sizeof(byte),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].filter_id,
         sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].eval_prec_id, sizeof(byte),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].eval_prec_id,
         sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].prot_num, sizeof(byte),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].prot_num,
         sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.mask.mask, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.mask.mask,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.mask.prefix_len, sizeof(uint8),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.mask.prefix_len,
         sizeof(uint8));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.ip_vsn, sizeof(ds_profile_db_ip_version),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask_ip_ver,
         sizeof(ds_profile_db_ip_version));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.address.
         pdp_addr.pdp_addr_ipv4, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.address.v4,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.address.
         pdp_addr.pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.address.v6,
         sizeof(ds_profile_db_ip_addr_v6_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].dest_port_range, sizeof(ds_umts_port_range_type),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].dest_port_range,
         sizeof(ds_umts_port_range_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_port_range, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_port_range,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].ipsec_spi, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].ipsec_spi,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].tos_mask, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].tos_mask,
         sizeof(uint16));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].flow_label, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].flow_label,
         sizeof(uint32));
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_TFT_FILTER_ID2

DESCRIPTION
  The function updates the tft filter id parameters
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_tft_filter_id2
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  uint8 index = 1, tft_index = 1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memset((void *)(ds_umts_pdp_profile_ptr->tft + index),0,
     sizeof(ds_umts_tft_params_type));

  ds_umts_pdp_profile_ptr->tft[tft_index].valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.tft[tft_index];

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].filter_id, sizeof(byte),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].filter_id,
         sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].eval_prec_id, sizeof(byte),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].eval_prec_id,
         sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].prot_num, sizeof(byte),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].prot_num,
         sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.mask.mask, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.mask.mask,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.mask.prefix_len, sizeof(uint8),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.mask.prefix_len,
         sizeof(uint8));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.ip_vsn, sizeof(ds_profile_db_ip_version),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask_ip_ver,
         sizeof(ds_profile_db_ip_version));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.address.
         pdp_addr.pdp_addr_ipv4, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.address.v4,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.address.
         pdp_addr.pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), 
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.address.v6,
         sizeof(ds_profile_db_ip_addr_v6_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].dest_port_range, sizeof(ds_umts_port_range_type),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].dest_port_range,
         sizeof(ds_umts_port_range_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_port_range, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_port_range,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].ipsec_spi, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].ipsec_spi,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].tos_mask, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].tos_mask,
         sizeof(uint16));

  memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].flow_label,  sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].flow_label,
         sizeof(uint32));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_MAX_PDN_CONN_PER_BLOCK

DESCRIPTION
  The function updates the pdp context apn
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_max_pdn_conn_per_block
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->max_pdn_conn_per_blk, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->max_pdn_conn_per_blk,
         sizeof(uint16));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDN_CONN_TIMER

DESCRIPTION
  The function updates the pdp context apn
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdn_conn_timer
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->max_pdn_conn_time, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->max_pdn_conn_time,
         sizeof(uint16));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDN_REQ_WAIT_TIMER

DESCRIPTION
  The function updates the pdp context apn
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdn_req_wait_timer
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->pdn_req_wait_time, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->pdn_req_wait_time,
         sizeof(uint16));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_USER_APP_DATA

DESCRIPTION
  The function updates the user profile class 
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info         --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_user_app_data
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  /*----------------------------------------------------------------------- 
  If tech mask != EPC_PROFILE_TECH_MASK (not EPC) do a blind copy and also set the epc mask
  of the cache ptr, However for an EPC profile do a memcpy only if the
  epc_profile_tech_mask is not set
  ----------------------------------------------------------------------*/

  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {

    memset(&(ds_umts_pdp_profile_ptr->user_app_data),0,sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->user_app_data, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tech_common_params.user_app_data,
         sizeof(uint32));

    ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= USER_APP_DATA_MOD;
  }
  else if ((tech_mask == EPC_PROFILE_TECH_MASK) && 
           (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & USER_APP_DATA_MOD)))
  {
    memset(&(ds_umts_pdp_profile_ptr->user_app_data),0,sizeof(uint32));

      memscpy((void *)&ds_umts_pdp_profile_ptr->user_app_data, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tech_common_params.user_app_data,
         sizeof(uint32));
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_ROAMING_DISALLOWED

DESCRIPTION
  The function updates the roaming_disallowed (enabled/disabled)
  to the cache
    
PARAMETERS: 
  profile_number            --  Profile number passed by profile DB.
  *ds_3gpp_tech_params_ptr  --  Contains info about the parameters
  *gen_params               --  Contains info about generic profile parameters
   tech_mask                --  Profile supported tech mask
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_roaming_disallowed
(
        ds_profile_db_profile_num_type          profile_number,
  const ds_profile_db_3gpp_tech_params_type    *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type          *gen_params,
        ds_profile_db_supported_tech_mask       tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
   {
     return;
   }

  memscpy((void *)&ds_umts_pdp_profile_ptr->roaming_disallowed, sizeof(boolean),
				 (void *)&ds_3gpp_tech_params_ptr->roaming_disallowed,
				 sizeof(boolean));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDN_DISCON_WAIT_TIME

DESCRIPTION
  The function updates the pdn disconnect wait time 
  to the cache
    
PARAMETERS: 
  profile_number            --  Profile number passed by profile DB.
  *ds_3gpp_tech_params_ptr  --  Contains info about the parameters
  *gen_params               --  Contains info about generic profile parameters
   tech_mask                --  Profile supported tech mask
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdn_discon_wait_time
(
        ds_profile_db_profile_num_type          profile_number,
  const ds_profile_db_3gpp_tech_params_type    *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type          *gen_params,
        ds_profile_db_supported_tech_mask       tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
    {
      return;
    }

  memscpy((void *)&ds_umts_pdp_profile_ptr->pdn_discon_wait_time, sizeof(uint8),
				 (void *)&ds_3gpp_tech_params_ptr->pdn_discon_wait_time,
				 sizeof(uint8));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_ENTRY_DELETE

DESCRIPTION
  The function finds the cache entry corresponding to the profile number
  passed and marks the corresponding context as invalid
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  subs_id        -- Subscription ID of the profile 
  transient_profile -- We can only have a maximum of 8 transient profiles.
DEPENDENCIES
  None.
  
RETURN VALUE 
  boolean 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_profile_cache_entry_delete
(
   ds_profile_db_profile_num_type  profile_number,
   ds_umts_pdp_subs_e_type subs_id,
   boolean transient_profile
)
{
  boolean ret_val = FALSE;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    return ret_val;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);
  if (cache_index <= 0)
  {
    return ret_val;
  }

  ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);
  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL, Entry does not exist");
    return ret_val;
  }
  ds_3gpp_profile_cache_deallocate_profile_cache(ds_umts_pdp_profile_ptr);
  DS_UMTS_PDP_PROFILE_CACHE_SET(subs_id, cache_index, NULL);


  if (transient_profile) 
  {
     if (transient_profile_cnt[subs_id] == 0) 
     {
        DS_3GPP_MSG2_ERROR("Transient profile cnt is already %d for subs_id %d ",
                           transient_profile_cnt[subs_id],subs_id);
        return ret_val;
     }
     else
     {
        transient_profile_cnt[subs_id]--;
        DS_3GPP_MSG2_HIGH("New Transient profile cnt is %d for subs_id %d ",
                          transient_profile_cnt[subs_id],subs_id);
     }
  }
  else
  {
     if (persistent_profile_cnt == 0) 
     {
        DS_3GPP_MSG0_ERROR("Persistent profile cnt is already 0 ");
        return ret_val;
     }
     else
     {
        persistent_profile_cnt--;
        DS_3GPP_MSG1_HIGH("New Persistent profile cnt is %d",
                          persistent_profile_cnt);
     }
     
  }

  ret_val = TRUE;
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_ENTRY_DELETE_ALL

DESCRIPTION
  The function finds the cache entry corresponding to the profile number
  passed and marks the corresponding context as invalid
    
PARAMETERS: 
  subs_id        -- Subscription ID of the profile 
DEPENDENCIES
  None.
  
RETURN VALUE 
  boolean 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_entry_delete_all
(
   ds_umts_pdp_subs_e_type subs_id
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  int8 cache_index = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  transient_profile_cnt[subs_id] = 0;
  for (cache_index = 1;
       cache_index <= DS_UMTS_MAX_PDP_PROFILE_NUM;
				cache_index++)
  {
     ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

     if (ds_umts_pdp_profile_ptr != NULL)
     {
        if (ds_umts_pdp_profile_ptr->context.valid_flg) 
        {
           if (ds_umts_pdp_profile_ptr->transience_flag == FALSE) 
           {
              if (persistent_profile_cnt == 0) 
              {
                 DS_3GPP_MSG0_ERROR("Persistent profile cnt is already 0 ");
              }
              else
              {
                 persistent_profile_cnt--;
                 DS_3GPP_MSG1_HIGH("New Persistent profile cnt is %d",
                          persistent_profile_cnt);
              }
     
           }
        }
        ds_3gpp_profile_cache_deallocate_profile_cache(ds_umts_pdp_profile_ptr);
        DS_UMTS_PDP_PROFILE_CACHE_SET(subs_id, cache_index, NULL);
     }
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_ENTRY_CREATE

DESCRIPTION
  The function finds the cache entry corresponding to the profile number
  passed and marks the corresponding context as valid
    
PARAMETERS: 
  profile_number    --  Profile number passed by profile DB.
  subs_id           -- Subscription Id of the profile
  transient_profile -- We can only have a maximum of 8 transient profiles.
DEPENDENCIES
  None.
  
RETURN VALUE 
  boolean 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_profile_cache_entry_create
(
   ds_profile_db_profile_num_type  profile_number,
   ds_umts_pdp_subs_e_type         subs_id,
   boolean                         transient_profile
)
{
  boolean                           ret_val = FALSE;
  ds_umts_pdp_profile_type         *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype  is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    return ret_val;
  }
  cache_index = ds_3gpp_profile_cache_get_free_cache_index(subs_id);

  if (cache_index <= 0)
  {
    DS_3GPP_MSG0_ERROR("Cache index is unavailable");
    return ret_val;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL, Entry does not exist");
    return ret_val;
  }

  memset(ds_umts_pdp_profile_ptr,0,sizeof(ds_umts_pdp_profile_type));

  if (transient_profile) 
  {
     if (transient_profile_cnt[subs_id] >= DS_UMTS_MAX_PDP_TRANSIENT_PROFILE_NUM)
     {
        DS_3GPP_MSG2_ERROR("Transient profile cnt is already %d for subs_id %d, "
                           "can't have more", transient_profile_cnt[subs_id], subs_id);
        return ret_val;
     }
     else
     {
        transient_profile_cnt[subs_id]++;
        DS_3GPP_MSG2_HIGH("New Transient profile cnt is %d for subs_id %d",
                          transient_profile_cnt[subs_id], subs_id);
     }
  }
  else
  {
    if (persistent_profile_cnt >= (DS_UMTS_MAX_PDP_PROFILE_NUM - DS_UMTS_MAX_PDP_TRANSIENT_PROFILE_NUM))
    {
      DS_3GPP_MSG1_ERROR("Persistent profile cnt is already %d, can't have more", persistent_profile_cnt);
      return ret_val;
    }
    else
    {
      persistent_profile_cnt++;
      DS_3GPP_MSG1_HIGH("New Persistent profile cnt is %d",persistent_profile_cnt);
    }
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;
  ds_umts_pdp_profile_ptr->context.pdp_context_number = (byte)profile_number;
  ds_umts_pdp_profile_ptr->profile_number = profile_number;

  /* Gobi specific Customization*/
#ifdef FEATURE_GOBI
/* Set requested UMTS QoS parm, traffic class to Interactive for Gobi2 UMTS builds - CR 155084 */
  ds_umts_pdp_profile_ptr->qos_request_umts.valid_flg = TRUE;
  ds_umts_pdp_profile_ptr->qos_request_umts.traffic_class = DS_UMTS_QOS_TCLASS_INTR;   
#endif

  ds_umts_pdp_profile_ptr->apn_bearer = DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_ALL;
  ds_umts_pdp_profile_ptr->context.valid_flg  = TRUE; 

#ifdef FEATURE_DATA_PS_IPV6
   ds_umts_pdp_profile_ptr->context.pdp_type  = DS_UMTS_PDP_IPV4V6;
#else
   ds_umts_pdp_profile_ptr->context.pdp_type  = DS_UMTS_PDP_IPV4;
#endif /* FEATURE_DATA_PS_IPV6 */
  ds_umts_pdp_profile_ptr->context.pdp_addr.ip_vsn = DS_IP_V4; 

  ds_umts_pdp_profile_ptr->context.lte_roaming_pdp_type  = DS_UMTS_PDP_MAX;
  ds_umts_pdp_profile_ptr->context.umts_roaming_pdp_type  = DS_UMTS_PDP_MAX;
  ds_umts_pdp_profile_ptr->context.override_home_pdp_type  = DS_UMTS_PDP_MAX;

  ds_umts_pdp_profile_ptr->max_pdn_conn_per_blk = DS_UMTS_PDP_DEF_MAX_PDN_CONN_PER_BLK;
  ds_umts_pdp_profile_ptr->pdn_req_wait_time = DS_UMTS_PDP_DEF_PDN_REQ_WAIT_TIME;
  ds_umts_pdp_profile_ptr->max_pdn_conn_time = DS_UMTS_PDP_DEF_MAX_PDN_CONN_TIME;

  ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.ip_vsn = DS_IP_V4;
  ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.ip_vsn = DS_IP_V4;
  ds_umts_pdp_profile_ptr->subscription_id = DS_ACTIVE_SUBSCRIPTION_1;

  ds_umts_pdp_profile_ptr->clat_enabled = FALSE;
  ds_umts_pdp_profile_ptr->roaming_ho_iwlan_to_lte = TRUE;
  ds_umts_pdp_profile_ptr->roaming_ho_lte_to_iwlan = TRUE;

  ret_val = TRUE;
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_ENTRY_RESET

DESCRIPTION
  The function finds the cache entry corresponding to the profile number
  passed and resets to default values
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  subs_id -- Subscription Id of the profile
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  boolean 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_profile_cache_entry_reset
(
   ds_profile_db_profile_num_type  profile_number,
   ds_umts_pdp_subs_e_type subs_id
)
{
  boolean ret_val = FALSE;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
  boolean transience_flag = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    return ret_val;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);
  if (cache_index <= 0)
  {
    return ret_val;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL, Entry does not exist");
    return ret_val;
  }

  transience_flag = ds_umts_pdp_profile_ptr->transience_flag;

  memset((void *)ds_umts_pdp_profile_ptr,0,sizeof(ds_umts_pdp_profile_type));
  ds_umts_pdp_profile_ptr->transience_flag = transience_flag;
  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;
  ds_umts_pdp_profile_ptr->context.pdp_context_number = (byte)profile_number; 
  ds_umts_pdp_profile_ptr->profile_number = profile_number;

    /* Gobi specific Customization*/
#ifdef FEATURE_GOBI
/* Set requested UMTS QoS parm, traffic class to Interactive for Gobi2 UMTS builds - CR 155084 */
  ds_umts_pdp_profile_ptr->qos_request_umts.valid_flg = TRUE;
  ds_umts_pdp_profile_ptr->qos_request_umts.traffic_class = DS_UMTS_QOS_TCLASS_INTR;    
#endif

  ds_umts_pdp_profile_ptr->apn_bearer = DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_ALL;
  ds_umts_pdp_profile_ptr->context.valid_flg  = TRUE; 

#ifdef FEATURE_DATA_PS_IPV6
  ds_umts_pdp_profile_ptr->context.pdp_type  = DS_UMTS_PDP_IPV4V6;
#else
  ds_umts_pdp_profile_ptr->context.pdp_type  = DS_UMTS_PDP_IPV4;
#endif /* FEATURE_DATA_PS_IPV6 */
  ds_umts_pdp_profile_ptr->context.override_home_pdp_type = DS_UMTS_PDP_MAX;

  ds_umts_pdp_profile_ptr->context.lte_roaming_pdp_type = DS_UMTS_PDP_MAX;
  ds_umts_pdp_profile_ptr->context.umts_roaming_pdp_type = DS_UMTS_PDP_MAX;

  ds_umts_pdp_profile_ptr->context.pdp_addr.ip_vsn = DS_IP_V4; 

  ds_umts_pdp_profile_ptr->max_pdn_conn_per_blk = DS_UMTS_PDP_DEF_MAX_PDN_CONN_PER_BLK;
  ds_umts_pdp_profile_ptr->pdn_req_wait_time = DS_UMTS_PDP_DEF_PDN_REQ_WAIT_TIME;
  ds_umts_pdp_profile_ptr->max_pdn_conn_time = DS_UMTS_PDP_DEF_MAX_PDN_CONN_TIME;

  ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.ip_vsn = DS_IP_V4;
  ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.ip_vsn = DS_IP_V4;
  ds_umts_pdp_profile_ptr->subscription_id = DS_ACTIVE_SUBSCRIPTION_1;

  ds_umts_pdp_profile_ptr->clat_enabled = FALSE;
  ds_umts_pdp_profile_ptr->roaming_ho_iwlan_to_lte = TRUE;
  ds_umts_pdp_profile_ptr->roaming_ho_lte_to_iwlan = TRUE;

  ret_val = TRUE;
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_IS_ENTRY_VALID

DESCRIPTION
  The function finds the cache entry corresponding to the profile number
  passed and checks if the corresponding entry is valid
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  subs_id -- Profile Subscription ID
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  boolean 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_profile_cache_is_entry_valid
(
   ds_profile_db_profile_num_type  profile_number,
   ds_umts_pdp_subs_e_type subs_id
)
{
  boolean ret_val = FALSE;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    return ret_val; 
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);
  if (cache_index <= 0)
  {
    return ret_val;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL, Entry does not exist");
    return ret_val;
  }

  ret_val = ds_umts_pdp_profile_ptr->context.valid_flg;

  DS_3GPP_MSG1_MED("Cache entry is %d",ret_val);
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_CONVERT_PROFILE_DB_COMMON_TECH_PARAMS

DESCRIPTION
  The function converts the profile DB common tech parameters into a
  format recognized by 3gpp MH cache
    
PARAMETERS: 
 void*                                     tech_specific_params,              
 ds_umts_pdp_profile_type                 *ds_umts_pdp_profile_ptr,
 ds_profile_db_supported_tech_mask         tech_mask
 
DEPENDENCIES
  None.
  
RETURN VALUE 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_convert_profile_db_common_tech_params
(
   void                                    *tech_specific_params_ptr,             
   ds_umts_pdp_profile_type                 *ds_umts_pdp_profile_ptr,
   ds_profile_db_supported_tech_mask         tech_mask
)
{
  ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache ptr is INVALID, Return NULL");
    return;
  }

  if(tech_specific_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Tech params ptr is NULL");
    return;
  }

  ds_3gpp_tech_params_ptr = (ds_profile_db_3gpp_tech_params_type *)
                            tech_specific_params_ptr;

  /*---------------------------------------------------------------------------- 
    If tech mask != EPC_PROFILE_TECH_MASK (3gpp) store all parameters in the cache, Now compare the
    values passed with the default values, If the values are different from the
    default value, It means that the 3gpp common tech parameter has overridden
    and hence these would need to be stored.
    ----------------------------------------------------------------------------*/
  if (tech_mask != EPC_PROFILE_TECH_MASK)
  {
    /*------------------------------------------------------------------------ 
      Copy Authentication info
     -------------------------------------------------------------------------*/
    memscpy((void *)&ds_umts_pdp_profile_ptr->auth, sizeof(ds_profile_db_auth_info),
       (void *)&ds_3gpp_tech_params_ptr->
       tech_common_params.auth_info,sizeof(ds_profile_db_auth_info));

    /*------------------------------------------------------------------------ 
      Copy DNS Address info
     -------------------------------------------------------------------------*/
    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.pdp_addr.
       pdp_addr_ipv4, sizeof(ds_profile_db_ip_addr_v4_type), (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.primary_addr.v4, sizeof(ds_profile_db_ip_addr_v4_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.pdp_addr.
          pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), (void *)&ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.
       pdp_addr_ipv4, sizeof(ds_profile_db_ip_addr_v4_type), (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.secondary_addr.v4, sizeof(ds_profile_db_ip_addr_v4_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.
       pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.secondary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.ip_vsn, sizeof(ds_profile_db_ip_version), 
       (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.primary_ip_version, sizeof(ds_profile_db_ip_version));

    memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.ip_vsn, sizeof(ds_profile_db_ip_version), 
       (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.secondary_ip_version, sizeof(ds_profile_db_ip_version));


    memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_flag, sizeof(boolean),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
           request_pcscf_address_flag,sizeof(boolean));

    memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_using_dhcp_flag, sizeof(boolean),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
           request_pcscf_address_using_dhcp_flag,sizeof(boolean));

    memscpy((void *)&ds_umts_pdp_profile_ptr->request_dns_address_using_dhcp_flag, sizeof(boolean),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
           request_dns_address_using_dhcp_flag,sizeof(boolean));

    memscpy((void *)&ds_umts_pdp_profile_ptr->apn_disable_flag, sizeof(boolean),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_disable_flag,
           sizeof(boolean));

    memscpy((void *)&ds_umts_pdp_profile_ptr->operator_reserved_pco, sizeof(uint16), 
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.op_pco_id,
           sizeof(uint16));

    memscpy((void *)&ds_umts_pdp_profile_ptr->apn_class, sizeof(uint8),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_class,
           sizeof(uint8));


  /*--------------------------------------------------------------------------------- 
    Iface linger params
    ---------------------------------------------------------------------------------*/

    memscpy((void *)&ds_umts_pdp_profile_ptr->iface_linger_params.linger_timeout_val, sizeof(uint16),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
           .linger_params.linger_timeout_val,
           sizeof(uint16));


    memscpy((void *)&ds_umts_pdp_profile_ptr->iface_linger_params.allow_linger_flag, sizeof(boolean),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
           .linger_params.allow_linger_flag,
           sizeof(boolean));

    memscpy((void *)&ds_umts_pdp_profile_ptr->iface_linger_params.valid_flag, sizeof(boolean),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
           .is_valid,sizeof(boolean));

    memscpy((void *)&ds_umts_pdp_profile_ptr->mcc, sizeof(uint16),
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.mcc,
           sizeof(uint16));

    memscpy((void *)&ds_umts_pdp_profile_ptr->mnc, sizeof(ds_umts_3gpp_mnc_type), 
           (void *)&ds_3gpp_tech_params_ptr->tech_common_params.mnc,
           sizeof(ds_umts_3gpp_mnc_type));

	memscpy((void *)&ds_umts_pdp_profile_ptr->user_app_data, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tech_common_params.user_app_data,
         sizeof(uint32));

   /*----------------------------------------------------------------------------- 
     Set the EPC masks corresponding to the values which are different from the
     default values. If the mask value is set then do not store the common tech 
     parameters for EPC profiles 
     -----------------------------------------------------------------------------*/
    if (ds_3gpp_tech_params_ptr->tech_common_params.apn_disable_flag)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= APN_DISABLE_FLAG_MOD;
    }

    if(ds_3gpp_tech_params_ptr->tech_common_params.apn_class > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= APN_CLASS_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params.linger_params.
        linger_timeout_val > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= LINGER_PARAMS_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.request_pcscf_address_flag)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PCSCF_ADDR_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.request_pcscf_address_using_dhcp_flag)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= DHCP_PCSCF_ADDR_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.request_dns_address_using_dhcp_flag)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= DHCP_DNS_ADDR_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_ip_version
       != DB_IP_V4)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PRIMARY_DNS_IP_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_addr.v4.addr > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PRIMARY_DNS_ADDR_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.secondary_addr.v4.addr > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= SEC_DNS_ADDR_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.secondary_ip_version
         != DB_IP_V4)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= SEC_DNS_IP_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.auth_info.auth_type != 
         DB_DATA_SESS_AUTH_PROTOCOL_NONE)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= AUTH_TYPE_MOD;
    }

    if (strlen((char *)ds_3gpp_tech_params_ptr->tech_common_params.
               auth_info.password) > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= AUTH_PASSWORD_MOD;
    }
        
    if (strlen((char *)ds_3gpp_tech_params_ptr->tech_common_params.
               auth_info.username) > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= AUTH_USERNAME_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.op_pco_id > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= PCO_ID_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.mcc > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= MCC_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.mnc.mnc_digits > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= MNC_MOD;
    }

    if (ds_3gpp_tech_params_ptr->tech_common_params.user_app_data > 0)
    {
      ds_umts_pdp_profile_ptr->epc_profile_tech_mask |= USER_APP_DATA_MOD;
    }

  }
  /*--------------------------------------------------------------------------- 
    For EPC profiles tech common parameters should not be copied if the bit mask
    is set, else copy the values.
    ----------------------------------------------------------------------------*/
  if (tech_mask == EPC_PROFILE_TECH_MASK)
  {

    if(!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & APN_DISABLE_FLAG_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->apn_disable_flag, sizeof(boolean),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_disable_flag,
             sizeof(boolean));
    }

    if(!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & APN_CLASS_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->apn_class, sizeof(uint8),
          (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_class,
          sizeof(uint8));

    }

    if(!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & LINGER_PARAMS_MOD))
    {

    /*--------------------------------------------------------------------------------- 
      Iface linger params
    ---------------------------------------------------------------------------------*/

      memscpy((void *)&ds_umts_pdp_profile_ptr->iface_linger_params.linger_timeout_val, sizeof(uint16),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
             .linger_params.linger_timeout_val,
             sizeof(uint16));


      memscpy((void *)&ds_umts_pdp_profile_ptr->iface_linger_params.allow_linger_flag, sizeof(boolean),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
             .linger_params.allow_linger_flag,
             sizeof(boolean));

      memscpy((void *)&ds_umts_pdp_profile_ptr->iface_linger_params.valid_flag, sizeof(boolean),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
           .is_valid,sizeof(boolean));

    }


    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PCSCF_ADDR_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_flag, sizeof(boolean), 
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
              request_pcscf_address_flag,sizeof(boolean));
    }

    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & DHCP_PCSCF_ADDR_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_using_dhcp_flag, sizeof(boolean), 
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
              request_pcscf_address_using_dhcp_flag,sizeof(boolean));
    }

    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & DHCP_DNS_ADDR_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->request_dns_address_using_dhcp_flag, sizeof(boolean), 
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
              request_dns_address_using_dhcp_flag,sizeof(boolean));
    }
 
    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PRIMARY_DNS_IP_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.ip_vsn,
               sizeof(ds_profile_db_ip_version),
               (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
               dns_addr.primary_ip_version, sizeof(ds_profile_db_ip_version));
    }

    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & SEC_DNS_IP_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.ip_vsn, sizeof(ds_profile_db_ip_version),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
             dns_addr.secondary_ip_version, sizeof(ds_profile_db_ip_version));

    }

    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PRIMARY_DNS_ADDR_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.pdp_addr.
      pdp_addr_ipv4, sizeof(ds_profile_db_ip_addr_v4_type), 
      (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
      dns_addr.primary_addr.v4, sizeof(ds_profile_db_ip_addr_v4_type));

      memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.primary_dns_addr.pdp_addr.
       pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type),
      (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
       dns_addr.primary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type));

    }

    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & SEC_DNS_ADDR_MOD))
    {
      
      memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.
             pdp_addr_ipv4, sizeof(ds_profile_db_ip_addr_v4_type), (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
             dns_addr.secondary_addr.v4, sizeof(ds_profile_db_ip_addr_v4_type));

      memscpy((void *)&ds_umts_pdp_profile_ptr->dns_addr.secondary_dns_addr.pdp_addr.
             pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), (void *)&ds_3gpp_tech_params_ptr->tech_common_params.
             dns_addr.secondary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type));

    }

    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_TYPE_MOD))
    {
      memscpy( (void*)&ds_umts_pdp_profile_ptr->auth.auth_type, sizeof(ds_profile_db_auth_protocol),
          (void *)&ds_3gpp_tech_params_ptr->tech_common_params.auth_info.auth_type,
          sizeof(ds_profile_db_auth_protocol));

    }

    if (!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_PASSWORD_MOD))
    {
      strlcpy( (char *) ds_umts_pdp_profile_ptr->auth.password,
               (char *)ds_3gpp_tech_params_ptr->tech_common_params.auth_info.password,
               sizeof(ds_umts_pdp_profile_ptr->auth.password));
    }

    if(!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_USERNAME_MOD))
    {

      strlcpy( (char *) ds_umts_pdp_profile_ptr->auth.username,
               (char *)ds_3gpp_tech_params_ptr->tech_common_params.
               auth_info.username,
               sizeof(ds_umts_pdp_profile_ptr->auth.username));

    }

    if(!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PCO_ID_MOD))
    {

      memscpy((void *)&ds_umts_pdp_profile_ptr->operator_reserved_pco, sizeof(uint16),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.op_pco_id,
             sizeof(uint16));

    }


    if(!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & MCC_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->mcc,  sizeof(uint16),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.mcc,
             sizeof(uint16));

    }

    if(!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & MNC_MOD))
    {

      memscpy((void *)&ds_umts_pdp_profile_ptr->mnc, sizeof(ds_umts_3gpp_mnc_type),
             (void *)&ds_3gpp_tech_params_ptr->tech_common_params.mnc,
             sizeof(ds_umts_3gpp_mnc_type));

    }

    if(!(ds_umts_pdp_profile_ptr->epc_profile_tech_mask & USER_APP_DATA_MOD))
    {
      memscpy((void *)&ds_umts_pdp_profile_ptr->user_app_data, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tech_common_params.user_app_data,
         sizeof(uint32));
    }
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_CONVERT_PROFILE_DB_PARAMS

DESCRIPTION
  The function converts the profile DB parameters into a format recognized
  by 3gpp MH cache
    
PARAMETERS: 
 void*                                     tech_specific_params,              
 ds_profile_db_gen_params_type*            gen_params 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
 *ds_umts_pdp_profile_type 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_umts_pdp_profile_type *ds_3gpp_profile_cache_convert_profile_db_params
(
   void                                    *tech_specific_params_ptr,              
   ds_profile_db_gen_params_type           *gen_params_ptr
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr = NULL;
  uint8 tft_index = 0;
  ds_profile_db_profile_num_type  profile_number = 0;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
  ds_umts_pdp_subs_e_type               subs_id =
                                          DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(gen_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Gen Parameters is NULL");
    return NULL;
  }

  if(tech_specific_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Tech params ptr is NULL");
    return NULL;
  }

  profile_number = gen_params_ptr->profile_number;
  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params_ptr->subscription_id);

  DS_3GPP_MSG1_MED("Profile number is %d",profile_number);

  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    return NULL;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
    DS_3GPP_MSG1_ERROR("Cache index is invalid:%d",cache_index);
    return NULL;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL, Entry does not exist");
    return NULL;
  }

  ds_3gpp_tech_params_ptr = (ds_profile_db_3gpp_tech_params_type *)
                 tech_specific_params_ptr;

  memset((void *)ds_umts_pdp_profile_ptr,0,sizeof(ds_umts_pdp_profile_type));

  ds_umts_pdp_profile_ptr->transience_flag = !gen_params_ptr->persistent;

  ds_umts_pdp_profile_ptr->tech_mask = gen_params_ptr->tech_mask;

  strlcpy((char *)ds_umts_pdp_profile_ptr->profile_name,(char *)
          gen_params_ptr->profile_name,
          sizeof(gen_params_ptr->profile_name));

  /*------------------------------------------------------------------------ 
    Copy PDP context
   -------------------------------------------------------------------------*/
  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;
  
  /*
  memcpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_context_number,
     (void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_context_number,
     sizeof(byte));
  */

  ds_umts_pdp_profile_ptr->context.pdp_context_number = (byte)profile_number;
  ds_umts_pdp_profile_ptr->profile_number = profile_number;
  switch (ds_3gpp_tech_params_ptr->tech_common_params.pdp_type) 
  {
     case DB_PDP_TYPE_IPV4:
       ds_umts_pdp_profile_ptr->context.pdp_type = DS_UMTS_PDP_IPV4;
       break;
     case DB_PDP_TYPE_IPV6:
       ds_umts_pdp_profile_ptr->context.pdp_type = DS_UMTS_PDP_IPV6;
       break;
     case DB_PDP_TYPE_IPV4V6:
       ds_umts_pdp_profile_ptr->context.pdp_type = DS_UMTS_PDP_IPV4V6;
       break;
     default:
       memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_type, 
           sizeof(ds_umts_pdp_type_enum_type),
       (void *)&ds_3gpp_tech_params_ptr->tech_common_params.pdp_type,
           sizeof(ds_umts_pdp_type_enum_type));
       break;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.h_comp, sizeof(ds_umts_pdp_header_comp_e_type),
     (void *)&ds_3gpp_tech_params_ptr->pdp_context.h_comp,
        sizeof(ds_umts_pdp_header_comp_e_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.d_comp, sizeof(ds_umts_pdp_header_comp_e_type),
     (void *)&ds_3gpp_tech_params_ptr->pdp_context.d_comp,
        sizeof(ds_umts_pdp_header_comp_e_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.ip_vsn, sizeof(ds_profile_db_ip_version),
      (void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_addr.ip_vsn,
      sizeof(ds_profile_db_ip_version));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.
          pdp_addr.pdp_addr_ipv4, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->pdp_context.
         pdp_addr.addr.v4,sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.
         pdp_addr.pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type), 
         (void *)&ds_3gpp_tech_params_ptr->pdp_context.
         pdp_addr.addr.v6,
         sizeof(ds_profile_db_ip_addr_v6_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.secondary_flag, sizeof(boolean),
         (void *)&ds_3gpp_tech_params_ptr->pdp_context.secondary_flag,
         sizeof(boolean));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.primary_id, sizeof(byte),
      (void *)&ds_3gpp_tech_params_ptr->pdp_context.primary_id,
      sizeof(byte));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.access_ctrl_flag, sizeof(ds_umts_pdp_access_ctrl_e_type),
      (void *)&ds_3gpp_tech_params_ptr->pdp_context.access_ctrl_flag,
      sizeof(ds_umts_pdp_access_ctrl_e_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->context.ipv4_addr_alloc, sizeof(ds_umts_pdp_ipv4_addr_alloc_e_type),
      (void *)&ds_3gpp_tech_params_ptr->pdp_context.ipv4_addr_alloc,
      sizeof(ds_umts_pdp_ipv4_addr_alloc_e_type));

  strlcpy((char *)ds_umts_pdp_profile_ptr->context.apn,gen_params_ptr->apn_name,
       sizeof(gen_params_ptr->apn_name));

  ds_umts_pdp_profile_ptr->qos_minimum_gprs.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_min_gprs;

  ds_umts_pdp_profile_ptr->qos_request_gprs.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_req_gprs;

  ds_umts_pdp_profile_ptr->qos_minimum_umts.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_min_umts;

  ds_umts_pdp_profile_ptr->qos_request_umts.valid_flg = 
    ds_3gpp_tech_params_ptr->is_valid.qos_req_umts;

  ds_3gpp_profile_cache_copy_umts_qos_params_from_db(&ds_umts_pdp_profile_ptr
  ->qos_request_umts,&ds_3gpp_tech_params_ptr->qos_request_umts, FALSE);

  ds_3gpp_profile_cache_copy_umts_qos_params_from_db(&ds_umts_pdp_profile_ptr
  ->qos_minimum_umts,&ds_3gpp_tech_params_ptr->qos_minimum_umts, FALSE);

  ds_3gpp_profile_cache_copy_gprs_qos_params_from_db(&ds_umts_pdp_profile_ptr
  ->qos_request_gprs,&ds_3gpp_tech_params_ptr->qos_request_gprs);

  ds_3gpp_profile_cache_copy_gprs_qos_params_from_db(&ds_umts_pdp_profile_ptr
  ->qos_minimum_gprs,&ds_3gpp_tech_params_ptr->qos_minimum_gprs);

  ds_3gpp_profile_cache_copy_lte_qos_params_from_db(ds_umts_pdp_profile_ptr,
                                                    ds_3gpp_tech_params_ptr);


  /*------------------------------------------------------------------------ 
    Copy TFT info
   -------------------------------------------------------------------------*/

  for (tft_index = 0;tft_index < DS_PROFILE_DB_3GPP_MAX_TFT_PARAM_SETS; tft_index++)
  {
    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].valid_flg, sizeof(boolean),
           (void *)&ds_3gpp_tech_params_ptr->is_valid.tft[tft_index],
           sizeof(boolean));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].filter_id, sizeof(byte),
           (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].filter_id,
           sizeof(byte));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].eval_prec_id, sizeof(byte),
           (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].eval_prec_id,
           sizeof(byte));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].prot_num, sizeof(byte),
           (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].prot_num,
           sizeof(byte));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.mask.mask, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.mask.mask,
         sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.mask.prefix_len, sizeof(uint8),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.mask.prefix_len,
         sizeof(uint8));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.ip_vsn, sizeof(ds_profile_db_ip_version),
           (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask_ip_ver,
           sizeof(ds_profile_db_ip_version));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.address.
           pdp_addr.pdp_addr_ipv4, sizeof(uint32),
           (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.address.v4,
           sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.address.
           pdp_addr.pdp_addr_ipv6.in6_u, sizeof(ds_profile_db_ip_addr_v6_type),
           (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.address.v6,
           sizeof(ds_profile_db_ip_addr_v6_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].dest_port_range, sizeof(ds_umts_port_range_type),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].dest_port_range,
         sizeof(ds_umts_port_range_type));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_port_range, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_port_range,
         sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].ipsec_spi, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].ipsec_spi,
         sizeof(uint32));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].tos_mask, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].tos_mask,
         sizeof(uint16));

    memscpy((void *)&ds_umts_pdp_profile_ptr->tft[tft_index].flow_label, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->tft[tft_index].flow_label,
         sizeof(uint32));

  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_using_dhcp_flag, sizeof(boolean), 
         (void *)&ds_3gpp_tech_params_ptr->tech_common_params.request_pcscf_address_using_dhcp_flag,
         sizeof(boolean));

  memscpy((void *)&ds_umts_pdp_profile_ptr->im_cn_flag, sizeof(boolean), 
         (void *)&ds_3gpp_tech_params_ptr->im_cn_flag,
         sizeof(boolean));


  memscpy((void *)&ds_umts_pdp_profile_ptr->emergency_calls_are_supported, sizeof(boolean), 
         (void *)&ds_3gpp_tech_params_ptr->emergency_calls_are_supported,
         sizeof(boolean));


  memscpy((void *)&ds_umts_pdp_profile_ptr->inactivity_timer_val, sizeof(uint32), 
         (void *)&ds_3gpp_tech_params_ptr->inactivity_timer_val,
         sizeof(uint32));



  memscpy((void *)&ds_umts_pdp_profile_ptr->src_stat_desc_req, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->src_stat_desc_req,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->src_stat_desc_min, sizeof(uint32),
         (void *)&ds_3gpp_tech_params_ptr->src_stat_desc_min,
         sizeof(uint32));

  memscpy((void *)&ds_umts_pdp_profile_ptr->apn_bearer, sizeof(uint8),
         (void *)&ds_3gpp_tech_params_ptr->apn_bearer,
         sizeof(uint8));


  memscpy((void *)&ds_umts_pdp_profile_ptr->max_pdn_conn_per_blk, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->max_pdn_conn_per_blk,
         sizeof(uint16));

  memscpy((void *)&ds_umts_pdp_profile_ptr->pdn_req_wait_time, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->pdn_req_wait_time,
         sizeof(uint16));

  memscpy((void *)&ds_umts_pdp_profile_ptr->max_pdn_conn_time, sizeof(uint16),
         (void *)&ds_3gpp_tech_params_ptr->max_pdn_conn_time,
         sizeof(uint16));


  memscpy((void *)&ds_umts_pdp_profile_ptr->roaming_disallowed, sizeof(boolean),
				 (void *)&ds_3gpp_tech_params_ptr->roaming_disallowed,
				 sizeof(boolean));

  memscpy((void *)&ds_umts_pdp_profile_ptr->pdn_discon_wait_time, sizeof(uint8),
				 (void *)&ds_3gpp_tech_params_ptr->pdn_discon_wait_time,
				 sizeof(uint8));

  memscpy((void *)&ds_umts_pdp_profile_ptr->subscription_id,
           sizeof(ds_profile_db_subs_type),
          (void *)&gen_params_ptr->subscription_id,
            sizeof(ds_profile_db_subs_type));

  memscpy((void *)&ds_umts_pdp_profile_ptr->ipv6_prefix_delegation_flag,
           sizeof(boolean),
           (void *)&gen_params_ptr->ipv6_delegation,
           sizeof(boolean));
  memscpy((void *)&ds_umts_pdp_profile_ptr->context.override_home_pdp_type,
          sizeof(ds_umts_pdp_type_enum_type),
          (void *)&ds_3gpp_tech_params_ptr->override_home_ip_type,
          sizeof(ds_umts_pdp_type_enum_type));

  memscpy((void *)ds_umts_pdp_profile_ptr->failure_timers, 
          sizeof(ds_umts_pdp_profile_ptr->failure_timers),
          (void *)ds_3gpp_tech_params_ptr->failure_timers,
          sizeof(ds_3gpp_tech_params_ptr->failure_timers));

  memscpy((void *)&ds_umts_pdp_profile_ptr->clat_enabled,
           sizeof(boolean),
           (void *)&gen_params_ptr->clat_enabled,
           sizeof(boolean));

  memscpy((void *)&ds_umts_pdp_profile_ptr->roaming_ho_iwlan_to_lte, 
          sizeof(boolean),
          (void *)&ds_3gpp_tech_params_ptr->roaming_ho_iwlan_to_lte,
          sizeof(boolean));

  memscpy((void *)&ds_umts_pdp_profile_ptr->roaming_ho_lte_to_iwlan, 
          sizeof(boolean),
          (void *)&ds_3gpp_tech_params_ptr->roaming_ho_lte_to_iwlan,
          sizeof(boolean));

  return ds_umts_pdp_profile_ptr;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_READONLY_GEN_PARAMS

DESCRIPTION
  The function will copy readonly general parameter to umts_cache
    
PARAMETERS:             
 ds_profile_db_gen_params_type           *gen_params_ptr
 
DEPENDENCIES
  None.
  
RETURN VALUE 
 None
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_readonly_gen_params
(           
   ds_profile_db_gen_params_type           *gen_params_ptr
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_profile_db_profile_num_type  profile_number = 0;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
  ds_umts_pdp_subs_e_type           subs_id
                                      =  DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (gen_params_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("write_readonly_gen_params: gen_params_ptr is null");
    return;
  }

  profile_number = gen_params_ptr->profile_number;
  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(
                                              gen_params_ptr->subscription_id);

  DS_3GPP_MSG1_MED("Profile number is %d",profile_number);

  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    return;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);
  if (cache_index <= 0)
  {
    return ;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL, Entry does not exist");
    return;
  }


  ds_umts_pdp_profile_ptr->transience_flag = !gen_params_ptr->persistent;

  ds_umts_pdp_profile_ptr->tech_mask = gen_params_ptr->tech_mask;

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_CONVERT_TO_PROFILE_DB

DESCRIPTION
  The function converts cache params into profile DB params
    
PARAMETERS: 
  ds_profile_db_profile_num_type num, 
  ds_profile_db_3gpp_tech_params_type* tech_params,
  ds_profile_db_gen_params_type* gen_params 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_cache_convert_to_profile_db
(
  ds_profile_db_profile_num_type num, 
  ds_profile_db_subs_type subs,
  ds_profile_db_3gpp_tech_params_type* ds_3gpp_tech_params_ptr,
  ds_profile_db_gen_params_type* gen_params_ptr
)
{
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  ds_profile_db_result_type ret_val = DB_RESULT_FAIL;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  uint8 tft_index = 0;
  ds_profile_db_profile_num_type  profile_number = num;
  int8 cache_index = -1;
  ds_umts_pdp_subs_e_type           subs_id
                                      = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    return ret_val;
  }

  if (ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Tech param ptr is NULL");
    return ret_val;
  }

  if (gen_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Gen param ptr is NULL");
    return ret_val;
  }

  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(subs);

  if(!ds_3gpp_profile_cache_is_entry_valid(profile_number, subs_id))
  {
    return ret_val;
  }
 
  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);
  if (cache_index <= 0)
  {
    return ret_val;
  }

  ds_umts_pdp_profile_ptr = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL, Entry does not exist");
    return ret_val;
  }

  memset((void *)ds_3gpp_tech_params_ptr,0, 
         sizeof(ds_profile_db_3gpp_tech_params_type));

  memset((void *)gen_params_ptr,0,sizeof(ds_profile_db_gen_params_type));

  gen_params_ptr->profile_number = num;
  gen_params_ptr->persistent = !ds_umts_pdp_profile_ptr->transience_flag;
  gen_params_ptr->subscription_id = subs;

  gen_params_ptr->tech_mask = ds_umts_pdp_profile_ptr->tech_mask;

  strlcpy((char *)gen_params_ptr->profile_name,
          (char *)ds_umts_pdp_profile_ptr->profile_name,
          sizeof(gen_params_ptr->profile_name));

 /*------------------------------------------------------------------------ 
  Copy PDP context
 -------------------------------------------------------------------------*/
  ds_3gpp_tech_params_ptr->is_valid.pdp = 
    ds_umts_pdp_profile_ptr->context.valid_flg;

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_context_number, sizeof(byte),
         (void *)&ds_umts_pdp_profile_ptr->context.pdp_context_number,
          sizeof(byte));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.h_comp, sizeof(ds_umts_pdp_header_comp_e_type),
         (void *)&ds_umts_pdp_profile_ptr->context.h_comp,
         sizeof(ds_umts_pdp_header_comp_e_type));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.d_comp, sizeof(ds_umts_pdp_header_comp_e_type),
         (void *)&ds_umts_pdp_profile_ptr->context.d_comp,
         sizeof(ds_umts_pdp_header_comp_e_type));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.d_comp, sizeof(ds_umts_pdp_header_comp_e_type),
         (void *)&ds_umts_pdp_profile_ptr->context.d_comp,
         sizeof(ds_umts_pdp_header_comp_e_type));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_addr.ip_vsn, sizeof(ds_profile_db_ip_version),
         (void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.ip_vsn,
         sizeof(ds_profile_db_ip_version));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_addr.addr.v4, sizeof(uint32),
         (void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.
         pdp_addr.pdp_addr_ipv4,sizeof(uint32));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_addr.addr.v6, sizeof(ds_profile_db_ip_addr_v6_type),
         (void *)&ds_umts_pdp_profile_ptr->context.pdp_addr.
         pdp_addr.pdp_addr_ipv6.in6_u,sizeof(ds_profile_db_ip_addr_v6_type));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.secondary_flag, sizeof(boolean),
         (void *)&ds_umts_pdp_profile_ptr->context.secondary_flag,
         sizeof(boolean));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.primary_id, sizeof(byte),
         (void *)&ds_umts_pdp_profile_ptr->context.primary_id,
         sizeof(byte));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.access_ctrl_flag, sizeof(ds_umts_pdp_access_ctrl_e_type),
         (void *)&ds_umts_pdp_profile_ptr->context.access_ctrl_flag,
         sizeof(ds_umts_pdp_access_ctrl_e_type));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.ipv4_addr_alloc, sizeof(ds_umts_pdp_ipv4_addr_alloc_e_type),
         (void *)&ds_umts_pdp_profile_ptr->context.ipv4_addr_alloc,
         sizeof(ds_umts_pdp_ipv4_addr_alloc_e_type));

   ds_3gpp_tech_params_ptr->override_home_ip_type =
    ds_3gpp_profile_cache_get_override_pdp_type(
       ds_umts_pdp_profile_ptr->context.override_home_pdp_type);

  switch (ds_umts_pdp_profile_ptr->context.lte_roaming_pdp_type) 
  {
    case DS_UMTS_PDP_IPV4:
      ds_3gpp_tech_params_ptr->lte_pdp_type = DB_3GPP_ROAMING_IPV4;
      break;
    case DS_UMTS_PDP_IPV6:
      ds_3gpp_tech_params_ptr->lte_pdp_type = DB_3GPP_ROAMING_IPV6;
      break;
    case DS_UMTS_PDP_IPV4V6:
      ds_3gpp_tech_params_ptr->lte_pdp_type = DB_3GPP_ROAMING_IPV4V6;
      break;
    default:
      ds_3gpp_tech_params_ptr->lte_pdp_type = DB_3GPP_ROAMING_IP_MAX;
      break;
  }

  switch (ds_umts_pdp_profile_ptr->context.umts_roaming_pdp_type) 
  {
    case DS_UMTS_PDP_IPV4:
      ds_3gpp_tech_params_ptr->umts_pdp_type = DB_3GPP_ROAMING_IPV4;
      break;
    case DS_UMTS_PDP_IPV6:
      ds_3gpp_tech_params_ptr->umts_pdp_type = DB_3GPP_ROAMING_IPV6;
      break;
    case DS_UMTS_PDP_IPV4V6:
      ds_3gpp_tech_params_ptr->umts_pdp_type = DB_3GPP_ROAMING_IPV4V6;
      break;
    default:
      ds_3gpp_tech_params_ptr->umts_pdp_type = DB_3GPP_ROAMING_IP_MAX;
      break;
  } 

    strlcpy((char *)gen_params_ptr->apn_name,
          (char *)ds_umts_pdp_profile_ptr->context.apn,
          sizeof(gen_params_ptr->apn_name));

  /*------------------------------------------------------------------------ 
    Copy PDP Type info
   -------------------------------------------------------------------------*/
 
   if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PDP_TYPE_MOD)
   {
     memscpy((void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_type, sizeof(ds_umts_pdp_type_enum_type),
         (void *)&ds_umts_pdp_profile_ptr->context.pdp_type,
          sizeof(ds_umts_pdp_type_enum_type));
   }
   else
   {
     #ifdef FEATURE_DATA_PS_IPV6
          ds_3gpp_tech_params_ptr->pdp_context.pdp_type = DB_3GPP_IPV4V6;
     #else
          ds_3gpp_tech_params_ptr->pdp_context.pdp_type  = DB_3GPP_IPV6;
     #endif /* FEATURE_DATA_PS_IPV6 */
   }

  /*------------------------------------------------------------------------ 
    Copy Authentication info
   -------------------------------------------------------------------------*/

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_TYPE_MOD)
  {

    memscpy( (void *)&ds_3gpp_tech_params_ptr->tech_common_params.auth_info.auth_type, 
             sizeof(ds_profile_db_auth_protocol),
            (void*)&ds_umts_pdp_profile_ptr->auth.auth_type,
            sizeof(ds_profile_db_auth_protocol));
  }
  else
  {
     ds_3gpp_tech_params_ptr->tech_common_params.auth_info.auth_type = 
       DB_DATA_SESS_AUTH_PROTOCOL_NONE;
  }

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_USERNAME_MOD)
  {

    strlcpy( (char *)ds_3gpp_tech_params_ptr->tech_common_params.
              auth_info.username,
             (char *) ds_umts_pdp_profile_ptr->auth.username,
             sizeof(ds_umts_pdp_profile_ptr->auth.username));

  }
  else
  {
    memset(ds_3gpp_tech_params_ptr->tech_common_params.
        auth_info.username,0,sizeof(ds_umts_pdp_profile_ptr->auth.username));

  }

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & AUTH_PASSWORD_MOD)
  {
    strlcpy( (char *)ds_3gpp_tech_params_ptr->tech_common_params.
               auth_info.password,
             (char *) ds_umts_pdp_profile_ptr->auth.password,
             sizeof(ds_umts_pdp_profile_ptr->auth.password));

  }
  else
  {
    memset(ds_3gpp_tech_params_ptr->tech_common_params.
        auth_info.password,0,sizeof(ds_umts_pdp_profile_ptr->auth.password));

  }



  /*------------------------------------------------------------------------ 
    Copy QOS info
  -------------------------------------------------------------------------*/

  ds_3gpp_tech_params_ptr->is_valid.qos_min_gprs =
      ds_umts_pdp_profile_ptr->qos_minimum_gprs.valid_flg;

  ds_3gpp_tech_params_ptr->is_valid.qos_req_umts =
    ds_umts_pdp_profile_ptr->qos_request_umts.valid_flg;
  
  ds_3gpp_tech_params_ptr->is_valid.qos_min_umts = 
    ds_umts_pdp_profile_ptr->qos_minimum_umts.valid_flg;

  ds_3gpp_tech_params_ptr->is_valid.qos_req_gprs =
    ds_umts_pdp_profile_ptr->qos_request_gprs.valid_flg;

  ds_3gpp_profile_cache_copy_gprs_qos_params_to_db
    (&ds_umts_pdp_profile_ptr->qos_request_gprs,
     &ds_3gpp_tech_params_ptr->qos_request_gprs);

  ds_3gpp_profile_cache_copy_gprs_qos_params_to_db
    (&ds_umts_pdp_profile_ptr->qos_minimum_gprs,
     &ds_3gpp_tech_params_ptr->qos_minimum_gprs);

  ds_3gpp_profile_cache_copy_lte_qos_params_to_db
    (ds_umts_pdp_profile_ptr,ds_3gpp_tech_params_ptr);

  ds_3gpp_profile_cache_copy_umts_qos_params_to_db
    (&ds_umts_pdp_profile_ptr->qos_request_umts,
     &ds_3gpp_tech_params_ptr->qos_request_umts);

  ds_3gpp_profile_cache_copy_umts_qos_params_to_db
    (&ds_umts_pdp_profile_ptr->qos_minimum_umts,
     &ds_3gpp_tech_params_ptr->qos_minimum_umts);


  /*------------------------------------------------------------------------ 
    Copy TFT info
   -------------------------------------------------------------------------*/

  for (tft_index = 0;tft_index < DS_PROFILE_DB_3GPP_MAX_TFT_PARAM_SETS; tft_index++)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->is_valid.tft[tft_index], sizeof(boolean),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].valid_flg,
           sizeof(boolean));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].filter_id, sizeof(byte),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].filter_id,
           sizeof(byte));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].eval_prec_id, sizeof(byte),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].eval_prec_id,
           sizeof(byte));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].prot_num, sizeof(byte),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].prot_num,
           sizeof(byte));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.mask, sizeof(uint32),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.mask.mask,
           sizeof(uint32));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.mask.prefix_len, sizeof(uint8),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.mask.prefix_len,
           sizeof(uint8));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask_ip_ver, sizeof(ds_profile_db_ip_version),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.ip_vsn,
           sizeof(ds_profile_db_ip_version));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.address.v4, sizeof(uint32),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.address.
           pdp_addr.pdp_addr_ipv4,sizeof(uint32));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_addr_mask.address.v6, sizeof(ds_profile_db_ip_addr_v6_type),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_addr_mask.address.
           pdp_addr.pdp_addr_ipv6.in6_u,sizeof(ds_profile_db_ip_addr_v6_type));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].dest_port_range, sizeof(ds_umts_port_range_type),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].dest_port_range,
           sizeof(ds_umts_port_range_type));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].src_port_range, sizeof(uint32),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].src_port_range,
           sizeof(uint32));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].ipsec_spi, sizeof(uint32),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].ipsec_spi,
           sizeof(uint32));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].tos_mask, sizeof(uint16),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].tos_mask,
           sizeof(uint16));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tft[tft_index].flow_label,  sizeof(uint32),
           (void *)&ds_umts_pdp_profile_ptr->tft[tft_index].flow_label,
           sizeof(uint32));
  }

  /*------------------------------------------------------------------------ 
    Copy DNS Address info
   -------------------------------------------------------------------------*/

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PRIMARY_DNS_ADDR_MOD)
  {

    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            dns_addr.primary_addr.v4, sizeof(ds_profile_db_ip_addr_v4_type), 
            (void *)&ds_umts_pdp_profile_ptr->
            dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4, 
            sizeof(ds_profile_db_ip_addr_v4_type));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
             dns_addr.primary_ip_version, sizeof(ds_profile_db_ip_version), 
            (void *)&ds_umts_pdp_profile_ptr->dns_addr.
             primary_dns_addr.ip_vsn,sizeof(ds_profile_db_ip_version));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            dns_addr.primary_addr.v6,sizeof(ds_profile_db_ip_addr_v6_type),
            (void *)&ds_umts_pdp_profile_ptr
            ->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u, 
            sizeof(ds_profile_db_ip_addr_v6_type));
  }
  else
  {
    memset(&ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_addr,0,
           sizeof(ds_profile_db_ip_addr_type));

    ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.primary_ip_version = DB_IP_V4;

  }


  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & SEC_DNS_ADDR_MOD)
  {

    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
         dns_addr.secondary_addr.v4, sizeof(ds_profile_db_ip_addr_v4_type), 
         (void *)&ds_umts_pdp_profile_ptr->dns_addr.
         secondary_dns_addr.pdp_addr.pdp_addr_ipv4, 
         sizeof(ds_profile_db_ip_addr_v4_type));


    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            dns_addr.secondary_addr.v6, sizeof(ds_profile_db_ip_addr_v6_type), 
            (void *)&ds_umts_pdp_profile_ptr->
            dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u, 
            sizeof(ds_profile_db_ip_addr_v6_type));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
            dns_addr.secondary_ip_version, sizeof(ds_profile_db_ip_version),
            (void *)&ds_umts_pdp_profile_ptr->dns_addr.
            secondary_dns_addr.ip_vsn,sizeof(ds_profile_db_ip_version));
  }
  else
  {
    memset(&ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.secondary_addr,0,
           sizeof(ds_profile_db_ip_addr_type));

    ds_3gpp_tech_params_ptr->tech_common_params.dns_addr.secondary_ip_version = DB_IP_V4;
  }

/*--------------------------------------------------------------------------------- 
  Iface linger params
  ---------------------------------------------------------------------------------*/
  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & LINGER_PARAMS_MOD)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
            .linger_params.linger_timeout_val, sizeof(uint16),
            (void *)&ds_umts_pdp_profile_ptr->
            iface_linger_params.linger_timeout_val,
            sizeof(uint16));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
            .linger_params.allow_linger_flag, sizeof(boolean),
           (void *)&ds_umts_pdp_profile_ptr->iface_linger_params.allow_linger_flag,
           sizeof(boolean));

    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
            .is_valid, sizeof(boolean),
            (void *)&ds_umts_pdp_profile_ptr->iface_linger_params.valid_flag,
            sizeof(boolean));
  }
  else
  {

    ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
            .linger_params.linger_timeout_val = 0;

    ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
              .linger_params.allow_linger_flag = FALSE;

    ds_3gpp_tech_params_ptr->tech_common_params.iface_linger_params
            .is_valid = FALSE;

  }

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PCSCF_ADDR_MOD)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
          request_pcscf_address_flag, sizeof(boolean),
          (void *)&ds_umts_pdp_profile_ptr
         ->request_pcscf_address_flag,sizeof(boolean));
  }
  else
  {

    ds_3gpp_tech_params_ptr->tech_common_params.
             request_pcscf_address_flag = FALSE;
  }

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & DHCP_PCSCF_ADDR_MOD)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
          request_pcscf_address_using_dhcp_flag, sizeof(boolean),
          (void *)&ds_umts_pdp_profile_ptr
         ->request_pcscf_address_using_dhcp_flag,sizeof(boolean));
  }
  else
  {

    ds_3gpp_tech_params_ptr->tech_common_params.
             request_pcscf_address_using_dhcp_flag = FALSE;
  }

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & DHCP_DNS_ADDR_MOD)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.
          request_dns_address_using_dhcp_flag, sizeof(boolean),
          (void *)&ds_umts_pdp_profile_ptr
         ->request_dns_address_using_dhcp_flag,sizeof(boolean));
  }
  else
  {

    ds_3gpp_tech_params_ptr->tech_common_params.
             request_dns_address_using_dhcp_flag = FALSE;
  }

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & APN_DISABLE_FLAG_MOD)
  {
    memscpy( (void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_disable_flag,
              sizeof(boolean),
             (void *)&ds_umts_pdp_profile_ptr->apn_disable_flag,
             sizeof(boolean));
  }
  else
  {
    ds_3gpp_tech_params_ptr->tech_common_params.apn_disable_flag = FALSE;
  }


  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & APN_CLASS_MOD)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.apn_class, sizeof(uint8),
            (void *)&ds_umts_pdp_profile_ptr->apn_class,
            sizeof(uint8));
  }
  else
  {
    ds_3gpp_tech_params_ptr->tech_common_params.apn_class = 0; 
  }


  memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.request_pcscf_address_using_dhcp_flag, sizeof(boolean),
         (void *)&ds_umts_pdp_profile_ptr->request_pcscf_address_using_dhcp_flag,
          sizeof(boolean));


  memscpy((void *)&ds_3gpp_tech_params_ptr->im_cn_flag, sizeof(boolean),
         (void *)&ds_umts_pdp_profile_ptr->im_cn_flag,
          sizeof(boolean));

  memscpy((void *)&ds_3gpp_tech_params_ptr->emergency_calls_are_supported, sizeof(boolean),
         (void *)&ds_umts_pdp_profile_ptr->emergency_calls_are_supported,
          sizeof(boolean));

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & PCO_ID_MOD)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.op_pco_id, sizeof(uint16),
            (void *)&ds_umts_pdp_profile_ptr->operator_reserved_pco,
            sizeof(uint16));
  }
  else
  {
    ds_3gpp_tech_params_ptr->tech_common_params.op_pco_id = 0;
  }

  memscpy((void *)&ds_3gpp_tech_params_ptr->inactivity_timer_val, sizeof(uint32), 
         (void *)&ds_umts_pdp_profile_ptr->inactivity_timer_val,
          sizeof(uint32));

  memscpy((void *)&ds_3gpp_tech_params_ptr->src_stat_desc_req, sizeof(uint32), 
         (void *)&ds_umts_pdp_profile_ptr->src_stat_desc_req,
          sizeof(uint32));

  memscpy((void *)&ds_3gpp_tech_params_ptr->src_stat_desc_min, sizeof(uint32), 
         (void *)&ds_umts_pdp_profile_ptr->src_stat_desc_min,
         sizeof(uint32));

  memscpy((void *)&ds_3gpp_tech_params_ptr->apn_bearer, sizeof(uint8), 
         (void *)&ds_umts_pdp_profile_ptr->apn_bearer,
          sizeof(uint8));

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & MCC_MOD)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.mcc, sizeof(uint16), 
            (void *)&ds_umts_pdp_profile_ptr->mcc,
            sizeof(uint16));
  }
  else
  {
    ds_3gpp_tech_params_ptr->tech_common_params.mcc = 0;
  }

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & MNC_MOD)
  {
    memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.mnc, sizeof(ds_umts_3gpp_mnc_type),
            (void *)&ds_umts_pdp_profile_ptr->mnc,
            sizeof(ds_umts_3gpp_mnc_type));
  }
  else
  {
    memset((void *)&ds_3gpp_tech_params_ptr->tech_common_params.mnc,0,
           sizeof(ds_profile_db_mnc_type));
  }

  memscpy((void *)&ds_3gpp_tech_params_ptr->max_pdn_conn_per_blk, sizeof(uint16),  
         (void *)&ds_umts_pdp_profile_ptr->max_pdn_conn_per_blk,
          sizeof(uint16));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdn_req_wait_time, sizeof(uint16),
         (void *)&ds_umts_pdp_profile_ptr->pdn_req_wait_time,
          sizeof(uint16));

  memscpy((void *)&ds_3gpp_tech_params_ptr->max_pdn_conn_time, sizeof(uint16), 
         (void *)&ds_umts_pdp_profile_ptr->max_pdn_conn_time,
          sizeof(uint16));

  memscpy((void *)&ds_3gpp_tech_params_ptr->roaming_disallowed, sizeof(boolean),
				 (void *)&ds_umts_pdp_profile_ptr->roaming_disallowed,
				 sizeof(boolean));

  memscpy((void *)&ds_3gpp_tech_params_ptr->pdn_discon_wait_time, sizeof(uint8),
				 (void *)&ds_umts_pdp_profile_ptr->pdn_discon_wait_time,
				 sizeof(uint8));

  if (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & USER_APP_DATA_MOD)
  {
          memscpy((void *)&ds_3gpp_tech_params_ptr->tech_common_params.user_app_data, sizeof(uint32),
         (void *)&ds_umts_pdp_profile_ptr->user_app_data,
          sizeof(uint32));
    }
    else
    {
            memset((void *)&ds_3gpp_tech_params_ptr->tech_common_params.user_app_data,0,
                                     sizeof(uint32));
    }


  memscpy((void *)&gen_params_ptr->subscription_id, sizeof(ds_profile_db_subs_type),
          (void *)&ds_umts_pdp_profile_ptr->subscription_id,
          sizeof(ds_profile_db_subs_type));

  memscpy((void *)&gen_params_ptr->ipv6_delegation,
          sizeof(boolean),
          (void *)&ds_umts_pdp_profile_ptr->ipv6_prefix_delegation_flag,
          sizeof(boolean));

  memscpy((void *)ds_3gpp_tech_params_ptr->failure_timers, 
          sizeof(ds_3gpp_tech_params_ptr->failure_timers),
          (void *)ds_umts_pdp_profile_ptr->failure_timers,
          sizeof(ds_umts_pdp_profile_ptr->failure_timers));

  memscpy((void *)&gen_params_ptr->clat_enabled,
          sizeof(boolean),
          (void *)&ds_umts_pdp_profile_ptr->clat_enabled,
          sizeof(boolean));

  memscpy((void *)&ds_3gpp_tech_params_ptr->roaming_ho_iwlan_to_lte, 
          sizeof(boolean),
          (void *)&ds_umts_pdp_profile_ptr->roaming_ho_iwlan_to_lte,
          sizeof(boolean));

  memscpy((void *)&ds_3gpp_tech_params_ptr->roaming_ho_lte_to_iwlan, 
          sizeof(boolean),
          (void *)&ds_umts_pdp_profile_ptr->roaming_ho_lte_to_iwlan,
          sizeof(boolean));

  ret_val = DB_RESULT_SUCCESS;
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_DEFAULT_VALUES

DESCRIPTION
  The function converts cache params into profile DB params
    
PARAMETERS: 
  ds_profile_db_3gpp_tech_params_type* profile
 
dEPENDENCIES
  None.
  
RETURN VALUE 
 void
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_get_default_values
(
  ds_profile_db_3gpp_tech_params_type* tech_profile_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (tech_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Profile parameter is NULL");
    return;
  }

  memset((void *)tech_profile_ptr,0,sizeof(ds_profile_db_3gpp_tech_params_type));

  /* Gobi specific Customization*/
#ifdef FEATURE_GOBI
/* Set requested UMTS QoS parm, traffic class to Interactive for Gobi2 UMTS builds - CR 155084 */
  tech_profile_ptr->is_valid.qos_request_umts     = TRUE;    
  tech_profile_ptr->qos_request_umts.traffic_class = DS_UMTS_QOS_TCLASS_INTR;    
#endif

  tech_profile_ptr->apn_bearer = DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_ALL;
  tech_profile_ptr->is_valid.pdp  = TRUE; 

#ifdef FEATURE_DATA_PS_IPV6
  tech_profile_ptr->tech_common_params.pdp_type  = DB_PDP_TYPE_IPV4V6;
#else
  tech_profile_ptr->tech_common_params.pdp_type  = DB_PDP_TYPE_IPV4;
#endif /* FEATURE_DATA_PS_IPV6 */

  tech_profile_ptr->lte_pdp_type  = DB_3GPP_ROAMING_IP_MAX;
  tech_profile_ptr->umts_pdp_type  = DB_3GPP_ROAMING_IP_MAX;
  tech_profile_ptr->override_home_ip_type = DB_3GPP_OVERRIDE_HOME_IP_MAX;
  tech_profile_ptr->pdp_context.pdp_addr.ip_vsn = DB_IP_V4; 

  tech_profile_ptr->max_pdn_conn_per_blk = DS_UMTS_PDP_DEF_MAX_PDN_CONN_PER_BLK;
  tech_profile_ptr->pdn_req_wait_time = DS_UMTS_PDP_DEF_PDN_REQ_WAIT_TIME;
  tech_profile_ptr->max_pdn_conn_time = DS_UMTS_PDP_DEF_MAX_PDN_CONN_TIME;

  tech_profile_ptr->tech_common_params.dns_addr.primary_ip_version = DB_IP_V4;
  tech_profile_ptr->tech_common_params.dns_addr.secondary_ip_version = DB_IP_V4;

  tech_profile_ptr->roaming_ho_iwlan_to_lte = TRUE;
  tech_profile_ptr->roaming_ho_lte_to_iwlan = TRUE;

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_CHECK_IF_DEFAULT_PROFILE_EXISTS

DESCRIPTION
  The function checks if the default profile exists in the cache
  Returns TRUE if the profile exists
  Returns FALSE otherwise
    
PARAMETERS: 
  ds_profile_db_profile_num_type  profile_number
  ds_umts_pdp_subs_e_type subs_id
 
DEPENDENCIES
  None.
  
RETURN VALUE 
 boolean
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_profile_cache_check_if_default_profile_exists
(
   ds_profile_db_profile_num_type  profile_number, 
   ds_umts_pdp_subs_e_type subs_id
)
{
  boolean is_profile_valid = FALSE;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS)
  {
    return ret_val;
  }

  if(!ds_3gpp_profile_cache_is_entry_valid(profile_number, subs_id))
  {
    return ret_val;
  }

  ret_val = TRUE;
  return ret_val;
}

#if 0
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT

DESCRIPTION
  The function updates the pdp type
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *ds_3gpp_tech_params_ptr         --  ds_profile_db_3gpp_tech_params_type
  *gen_params -- ds_profile_db_gen_params_type
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_pdp_context
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  ds_umts_pdp_profile_ptr->context.valid_flg = TRUE;

  /*lint -e534 ignoring return code from function*/
  memscpy((void *)&ds_umts_pdp_profile_ptr->context.pdp_type,
          sizeof(ds_umts_pdp_type_enum_type),
          (void *)&ds_3gpp_tech_params_ptr->pdp_context.pdp_type,
          sizeof(ds_umts_pdp_type_enum_type));
  /*lint +e534*/

}
#endif

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_IS_EPC_TECH_PARAM_SET

DESCRIPTION
  The function returns if the tech specific parameter was set in the EPC
    
PARAMETERS: 
  profile_number      --  Profile number passed by profile DB.
  subs_id             --  Subscriptions associated with this profile
  ident     	      --  ds_profile_db_ident_type
  *is_tech_param_set  --  boolean
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  ds_profile_db_result_type
  DB_RESULT_FAIL    -- failure
  DB_RESULT_SUCCESS -- success
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_cache_is_epc_tech_param_set
(
  ds_profile_db_profile_num_type num,
  ds_umts_pdp_subs_e_type subs_id,
  ds_profile_db_ident_type       ident,
  boolean *is_tech_param_set
)
{
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  ds_profile_db_result_type ret_val = DB_RESULT_FAIL;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_profile_db_profile_num_type  profile_number = num;
  int8 cache_index = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);
  if (is_profile_valid != DS_UMTS_PDP_SUCCESS) {

    return ret_val;
  }

  if (!ds_3gpp_profile_cache_is_entry_valid(profile_number, subs_id)) {
    return ret_val;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);
  if (cache_index <= 0)
  {
	return DB_RESULT_FAIL;
  }

  ds_umts_pdp_profile_ptr =
  DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL) {
    DS_3GPP_MSG0_ERROR("Cache Ptr is NULL, Entry does not exist");
    return ret_val;
  }
  if (is_tech_param_set == NULL) {
    DS_3GPP_MSG0_ERROR("EPC Mask is NULL, Cannot send");
    return ret_val;
  }
  //The epc_profile_tech_mask is set based on the common tech param ident value
  //by left shifting one for every new ident added.
  *is_tech_param_set = (ds_umts_pdp_profile_ptr->epc_profile_tech_mask & 
                       (0x01 << (COMMON_TECH_PARAM_MAX - ident)))? TRUE: FALSE;
  DS_3GPP_MSG1_MED("Is Tech Param Set: %d",*is_tech_param_set );
  return DB_RESULT_SUCCESS;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_IPV6_DELEGATION

DESCRIPTION
  The function updates the ipv6 delegation
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info     	  --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_ipv6_delegation
(
  ds_profile_db_profile_num_type            profile_number,
  const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type	    *gen_params,
  ds_profile_db_supported_tech_mask         tech_mask
) 
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->ipv6_prefix_delegation_flag, sizeof(boolean),
          (void *)&gen_params->ipv6_delegation,
          sizeof(boolean));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_FAILURE_TIMER

DESCRIPTION
  The function updates the cache with the failure timer value.
    
PARAMETERS: 
  profile_number                    --  Profile number passed by profile DB.
  ds_3gpp_tech_params_ptr    	    --  tech_params
  ds_profile_db_gen_params_type     --  *gen_params
  ds_profile_db_supported_tech_mask --  tech_mask
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  ds_3gpp_profile_cache_update_enum_type 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_failure_timer
(
   ds_profile_db_profile_num_type             profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask          tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (memcmp((void *)&ds_umts_pdp_profile_ptr->failure_timers, 
          (void *)&ds_3gpp_tech_params_ptr->failure_timers,
          sizeof(ds_3gpp_tech_params_ptr->failure_timers)) == 0) 
  {
    DS_3GPP_MSG0_ERROR("New value is same as old value");
    return;
  }

  memscpy((void *)ds_umts_pdp_profile_ptr->failure_timers,
          sizeof(ds_umts_pdp_profile_ptr->failure_timers),
          (void *)ds_3gpp_tech_params_ptr->failure_timers,
          sizeof(ds_3gpp_tech_params_ptr->failure_timers));

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_CLAT_ENABLED

DESCRIPTION
  The function updates the clat enabled flag
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info           --  Contains info about the parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_clat_enabled
(
  ds_profile_db_profile_num_type             profile_number,
  const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type       *gen_params,
  ds_profile_db_supported_tech_mask          tech_mask
)
{
  ds_umts_pdp_profile_type  *ds_umts_pdp_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_3gpp_profile_cache_get_umts_pdp_profile_ptr(profile_number,
                                                      ds_3gpp_tech_params_ptr,
                                                      gen_params,
                                                      &ds_umts_pdp_profile_ptr)
    )
  {
    return;
  }

  if (memcmp((void *)&ds_umts_pdp_profile_ptr->clat_enabled,
             (void *)&gen_params->clat_enabled,
             sizeof(boolean)) == 0) 
  {
    DS_3GPP_MSG0_ERROR("New value is same as old value");
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->clat_enabled,
          sizeof(boolean),
          (void *)&gen_params->clat_enabled,
          sizeof(boolean));

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_ROAMING_HO_IWLAN_TO_LTE

DESCRIPTION
  The function updates the cache with the failure roaming_ho_iwlan_to_lte
  value.
    
PARAMETERS: 
  profile_number                    --  Profile number passed by profile DB.
  ds_3gpp_tech_params_ptr           --  tech_params
  ds_profile_db_gen_params_type     --  *gen_params
  ds_profile_db_supported_tech_mask --  tech_mask
 
DEPENDENCIES
  None.
  
RETURN VALUE 
ds_3gpp_profile_cache_update_enum_type
 DS_3GPP_PROFILE_CACHE_UPDATE_SUCCESS - if param value is updated successfully
 DS_3GPP_PROFILE_CACHE_UPDATE_WITH_NO_CHANGE - if param new value is same as old param value
 DS_3GPP_PROFILE_CACHE_UPDATE_WITH_ERROR - if something else is wrong with the call
 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_write_indent_not_supported
(
  ds_profile_db_profile_num_type             profile_number,
  const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type       *gen_params,
  ds_profile_db_supported_tech_mask          tech_mask
)
{
    return;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_ROAMING_HO_IWLAN_TO_LTE

DESCRIPTION
  The function updates the cache with the failure roaming_ho_iwlan_to_lte
  value.
    
PARAMETERS: 
  profile_number                    --  Profile number passed by profile DB.
  ds_3gpp_tech_params_ptr    	      --  tech_params
  ds_profile_db_gen_params_type     --  *gen_params
  ds_profile_db_supported_tech_mask --  tech_mask
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_roaming_ho_iwlan_to_lte
(
   ds_profile_db_profile_num_type             profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask          tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE ;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (gen_params == NULL) 
  {
    DS_3GPP_MSG3_ERROR("Gen params is NULL", 0, 0, 0);
    return;
  }

  if (ds_3gpp_tech_params_ptr == NULL) 
  {
    DS_3GPP_MSG3_ERROR("Tech param is NULL", 0, 0, 0);
    return;
  }

  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params->subscription_id);

  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);

  if (is_profile_valid != DS_UMTS_PDP_SUCCESS) 
  {
    DS_3GPP_MSG3_ERROR("Profile number:%d invalid", profile_number, 0, 0);
    return;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0) 
  {
    DS_3GPP_MSG3_ERROR("Cache index is invalid:%d", cache_index, 0, 0);
    return;
  }

  ds_umts_pdp_profile_ptr =
     DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL) 
  {
    DS_3GPP_MSG3_ERROR("Cache Ptr is NULL", 0, 0, 0);
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->roaming_ho_iwlan_to_lte, 
          sizeof(boolean),
          (void *)&ds_3gpp_tech_params_ptr->roaming_ho_iwlan_to_lte,
          sizeof(boolean));
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_ROAMING_HO_LTE_TO_IWLAN

DESCRIPTION
  The function updates the cache with the failure roaming_ho_lte_to_iwlan
  value.
    
PARAMETERS: 
  profile_number                    --  Profile number passed by profile DB.
  ds_3gpp_tech_params_ptr    	      --  tech_params
  ds_profile_db_gen_params_type     --  *gen_params
  ds_profile_db_supported_tech_mask --  tech_mask
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_roaming_ho_lte_to_iwlan
(
   ds_profile_db_profile_num_type             profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask          tech_mask
)
{
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype is_profile_valid = DS_UMTS_PDP_FAIL;
  int8 cache_index = -1;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE ;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (gen_params == NULL) 
  {
    DS_3GPP_MSG3_ERROR("Gen params is NULL", 0, 0, 0);
    return;
  }

  if (ds_3gpp_tech_params_ptr == NULL) 
  {
    DS_3GPP_MSG3_ERROR("Tech param is NULL", 0, 0, 0);
    return;
  }

  subs_id = dsumts_subs_mgr_get_subs_id_from_db_subs_id(gen_params->subscription_id);

  is_profile_valid = ds_3gpp_profile_cache_check_profile_number(profile_number);

  if (is_profile_valid != DS_UMTS_PDP_SUCCESS) 
  {
    DS_3GPP_MSG3_ERROR("Profile number:%d invalid", profile_number, 0, 0);
    return;
  }

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0) 
  {
    DS_3GPP_MSG3_ERROR("Cache index is invalid:%d", cache_index, 0, 0);
    return;
  }

  ds_umts_pdp_profile_ptr =
     DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

  if (ds_umts_pdp_profile_ptr == NULL) 
  {
    DS_3GPP_MSG3_ERROR("Cache Ptr is NULL", 0, 0, 0);
    return;
  }

  memscpy((void *)&ds_umts_pdp_profile_ptr->roaming_ho_lte_to_iwlan, 
          sizeof(boolean),
          (void *)&ds_3gpp_tech_params_ptr->roaming_ho_lte_to_iwlan,
          sizeof(boolean));
}
