 /******************************************************************************
 @file    ds_3gpp_profile_api.c
 @brief   

  DESCRIPTION
  Tech specific implementation of 3GPP Profile Management  

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
****************************************************************************/
/*===========================================================================
17
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/13   vb      API to talk to Profile DB
=============================================================================*/

#include "ds_3gpp_profile_api.h"
#include "modem_mem.h"
#include "ds_3gpp_profile_cache_hdlr.h"
#include "ds_3gpp_profile_validator.h"
#include "ds_profile_db.h"
#include "ds_profile_db_internal.h"
#include "dsumtspdpregint.h"
#include "dsumtspdpreg.h"
#include "ds_profile_db_handler.h"
#include "ds3gmgr.h"
#include "ds_3gppi_utils.h"
#include "dstask_v.h"    
#include "ds3gcfgmgr.h"
#include "ds_3gpp_nv_manager.h"

#ifdef FEATURE_DUAL_SIM
#include "dsumts_subs_mgr.h"
#include "ds3gmgr.h"
#endif

extern validate_3gpp_fn_type dsi_profile_3gpp_modem_get_validation_func
(
  ds_profile_db_ident_type ident
);

extern void dsi_profile_3gpp_modem_write_ident_info
(
  uint16 profile_number,
  ds_profile_db_ident_type ident,
  const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type       *gen_params,
  ds_profile_db_supported_tech_mask tech_mask,
  boolean apn_param_check,
  ds_umts_pdp_profile_type *old_pdp_profile_data             
);

extern ds_profile_status_etype dsi_profile_3gpp_get_default_profile(
  uint32                   family, 
  ds_profile_num_type     *num
);


extern ds_profile_status_etype dsi_profile_3gpp_set_default_profile(
  uint32                   family, 
  ds_profile_num_type      num
);

extern ds_profile_status_etype dsi_profile_3gpp_get_default_profile_per_subs(
  uint32                  family,
  ds_profile_subs_etype  subs_id,
  ds_profile_num_type     *num
);

extern ds_umts_pdp_profile_status_etype 
dsi_profile_3gpp_set_default_profile_number_per_subs
(
  ds_umts_profile_family_e_type        family,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                               profile_number
);

extern boolean dsi_profile_3gpp_get_apn_changed_param
( 
  void
);

extern void dsi_profile_3gpp_reset_apn_changed_param
( 
  void
);


/*-------------------------------------------------------------------------- 
  STATIC/GLOBAL variables
  --------------------------------------------------------------------------*/

static ds_profile_db_callback_id_type ds_3gpp_callback_id = 0;
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CREATE_CMD_HDLR

DESCRIPTION
   Posted by MH in response to profile_create event received from Profile DB
                        
PARAMETERS: 
   ds_profile_db_profile_managed_type* profiles
   ds_profile_db_tech_type tech_type;
   ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_p;
   ds_profile_db_gen_params_type *ds_profile_db_gen_params_p;
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_create_hdlr
(
   ds_profile_db_profile_managed_type **profiles,
   uint16 profile_count
)
{
  boolean ret_val = FALSE;
  uint16 profile_number = 0;
  ds_profile_db_ident_type *ident_ptr = NULL;
  ds_profile_db_ident_type *gen_ident_ptr = NULL;
  uint16 ident_count = 0;
  uint16 gen_ident_count = 0;
  int ident_index = 0;
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr = NULL;
  ds_profile_db_gen_params_type *ds_profile_db_gen_params_ptr = NULL;
  ds_profile_db_tech_type tech_type = DB_TECH_3GPP;
  ds_profile_db_supported_tech_mask tech_mask = 0;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  boolean apn_param_check = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*------------------------------------------------------------------------- 
     Get the array of changed identifiers and for every changed identifier
     get the changed parameters
  -------------------------------------------------------------------------*/

  if(profiles == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params is NULL");
    goto func_exit;
  }

  ds_3gpp_tech_params_ptr = (ds_profile_db_3gpp_tech_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_3gpp_tech_params_type), MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_3gpp_tech_params_ptr failed");
    goto func_exit;
  }
  memset(ds_3gpp_tech_params_ptr, 0, sizeof(ds_profile_db_3gpp_tech_params_type));

  ds_profile_db_gen_params_ptr = (ds_profile_db_gen_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_gen_params_type), MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_profile_db_gen_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_profile_db_gen_params_ptr failed");
    goto func_exit;
  }
  memset(ds_profile_db_gen_params_ptr, 0, sizeof(ds_profile_db_gen_params_type));

  result_type = ds_profile_db_get_tech_params(*profiles,tech_type,(void *)
                                                  ds_3gpp_tech_params_ptr,
                                                  ds_profile_db_gen_params_ptr);


  if(result_type != DB_RESULT_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Cannot create the cache entry, DB Error: %d", result_type);
    
  }
  subs_id =  dsumts_subs_mgr_get_subs_id_from_db_subs_id(ds_profile_db_gen_params_ptr->
                                             subscription_id);

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     DS_3GPP_MSG1_ERROR("Invalid Subs id:%d",subs_id);
     goto func_exit;
  }

  ret_val = ds_3gpp_profile_cache_entry_create(
            ds_profile_db_gen_params_ptr->profile_number,
            subs_id,
            !ds_profile_db_gen_params_ptr->persistent);

  if(ret_val)
  {

    ds_3gpp_profile_cache_write_readonly_gen_params
                            (ds_profile_db_gen_params_ptr);

    profile_number = ds_profile_db_gen_params_ptr->profile_number;

    result_type = ds_profile_db_get_changed_idents(*profiles,
                  DB_TECH_TO_TECHMASK(tech_type),&ident_ptr,
                  &ident_count,&gen_ident_ptr,&gen_ident_count);

    DS_3GPP_MSG2_HIGH("Ident_cnt:%d, Gen_Ident_cnt:%d",ident_count,gen_ident_count);

    if(result_type != DB_RESULT_SUCCESS)
    {
      DS_3GPP_MSG1_ERROR("Cannot get the identifiers from Profile DB, DB Error %d",
                          result_type);
    }

    /*------------------------------------------------------------------------------- 
      Handle ident ptr for 3gpp common params
      -------------------------------------------------------------------------------*/

    if(ident_ptr != NULL)
    {
      for(ident_index = 0; ident_index < ident_count; ident_index++)
      {
        if((ident_ptr + ident_index) == NULL)
        {
          continue;   
        }
        dsi_profile_3gpp_modem_write_ident_info(profile_number,
                                      *(ident_ptr + ident_index),
				      ds_3gpp_tech_params_ptr,
                                      ds_profile_db_gen_params_ptr,
				      tech_mask,
				      apn_param_check,
				      NULL);
      }
    }

      /*------------------------------------------------------------------------------- 
      Handle gen ident ptr
      -------------------------------------------------------------------------------*/
    if(gen_ident_ptr != NULL)
    {
      for(ident_index = 0; ident_index < gen_ident_count; ident_index++)
      {          
        if((gen_ident_ptr + ident_index) == NULL)
        {
          continue;   
        }  
        dsi_profile_3gpp_modem_write_ident_info(profile_number,
                                   *(gen_ident_ptr + ident_index),ds_3gpp_tech_params_ptr,
                                  ds_profile_db_gen_params_ptr,tech_mask,
				  apn_param_check,
				  NULL);
       }
    }

    /*---------------------------------------------------------------------------------
      Get the tech_mask, If the profile is of EPC type(tech_mask == EPC_PROFILE_TECH_MASK), fetch the tech
      common parameters for non 3gpp portions too
      -------------------------------------------------------------------------------------------*/
    tech_mask = ds_profile_db_get_supported_tech_mask(*profiles);

    if(tech_mask == EPC_PROFILE_TECH_MASK)
    {
      tech_type = DB_TECH_COMMON;
      result_type = ds_profile_db_get_tech_params(*profiles,tech_type,(void *)
                                &(ds_3gpp_tech_params_ptr->tech_common_params),
                                                      ds_profile_db_gen_params_ptr);

      if(result_type != DB_RESULT_SUCCESS)
      {
        DS_3GPP_MSG1_ERROR("Cannot get the C profile params from DB, DB Error: %d", 
                           result_type);
      }

      ds_profile_db_free_idents(ident_ptr);
      ds_profile_db_free_idents(gen_ident_ptr);

      result_type = ds_profile_db_get_changed_idents(*profiles,
                                                       DB_TECH_TO_TECHMASK(tech_type),
                                                       &ident_ptr,
                                                       &ident_count,
                                                       &gen_ident_ptr,
                                                       &gen_ident_count);

      DS_3GPP_MSG2_HIGH("Ident_cnt:%d, Gen_Ident_cnt:%d",ident_count,gen_ident_count);

      if(result_type != DB_RESULT_SUCCESS)
      {
        DS_3GPP_MSG1_ERROR("Cannot get the identifiers from Profile DB, DB Error: %d",
                            result_type);
      }

      /*------------------------------------------------------------------------------- 
        Handle ident ptr for tech common params(non 3gpp part)
      -------------------------------------------------------------------------------*/
      if(ident_ptr != NULL)
      {
        for(ident_index = 0; ident_index < ident_count; ident_index++)
        { 
          if((ident_ptr + ident_index) == NULL)
          {
            continue;   
          }             
          dsi_profile_3gpp_modem_write_ident_info(profile_number,
                                   *(ident_ptr + ident_index),ds_3gpp_tech_params_ptr,
                                   ds_profile_db_gen_params_ptr,tech_mask,
				   apn_param_check,
				   NULL);
        }
      }
    }
  }

/*-------------------------------------------------------------------------------
Clean up function
-----------------------------------------------------------------------------------*/
  func_exit:
     /*-------------------------------------------------------------------------- 
       Free the identifiers and managed profile
      --------------------------------------------------------------------------*/
    if (ds_3gpp_tech_params_ptr != NULL) 
    {
      modem_mem_free(ds_3gpp_tech_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }

    if (ds_profile_db_gen_params_ptr != NULL) 
    {
      modem_mem_free(ds_profile_db_gen_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }
      
     ds_profile_db_free_idents(ident_ptr);
     ds_profile_db_free_idents(gen_ident_ptr);
     ds_profile_db_free_profiles(profiles,profile_count);

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_POST_PARAM_CHG_CMD_TO_DS_TASK

DESCRIPTION
   This would post a command which would be processed in DS task
                        
PARAMETERS: 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_post_param_chg_cmd_to_ds_task
(
   uint16                      profile_num,
   int8                        subs_id,
   void                       *user_data,
   ds_3gpp_profile_mask_e      mask
)
{
   ds_cmd_type                           *cmd_ptr = NULL;
   ds_3gpp_profile_db_param_chg_type     *param_chg_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_profile_db_param_chg_type));

  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PROFILE_DB_PARAM_CHG;

  param_chg_ptr = 
      (ds_3gpp_profile_db_param_chg_type*)cmd_ptr->cmd_payload_ptr;

  param_chg_ptr->user_data = user_data;
  param_chg_ptr->profile_num = profile_num;
  param_chg_ptr->subs_id = (uint8)subs_id;
  param_chg_ptr->mask = mask;
  DS_3GPP_MSG0_HIGH("Posting APN param chg cmd");
     
  ds_put_cmd_ext2(cmd_ptr);

}/*ds_3gpp_profile_post_param_chg_cmd_to_ds_task*/

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_MODIFY_CMD_HDLR

DESCRIPTION
   Posted by MH in response to profile_modify event received from Profile DB
                        
PARAMETERS: 
   ds_profile_db_profile_managed_type* profiles
   ds_profile_db_tech_type tech_type;
   ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_p;
   ds_profile_db_gen_params_type *ds_profile_db_gen_params_p;
   uint16 profile_count
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_modify_hdlr
(
   ds_profile_db_profile_managed_type **profiles,
   uint16 profile_count
)
{
  uint16 profile_number = 0;
  ds_profile_db_ident_type *ident_ptr = NULL;
  ds_profile_db_ident_type *gen_ident_ptr = NULL;
  uint16 ident_count = 0;
  uint16 gen_ident_count = 0;
  int ident_index = 0;
  ds_profile_db_info_type db_info_type_s;
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr = NULL;
  ds_profile_db_gen_params_type *ds_profile_db_gen_params_ptr = NULL;
  ds_profile_db_tech_type tech_type = DB_TECH_3GPP;
  boolean apn_param_changed = FALSE;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  boolean apn_param_check = TRUE;
  ds_umts_pdp_profile_type *old_pdp_profile_data_ptr = NULL;
  ds_umts_pdp_profile_status_etype result = DS_UMTS_PDP_FAIL;
  ds_umts_pdp_type_enum_type old_pdp_type =  DS_UMTS_PDP_MAX;
  ds_profile_db_supported_tech_mask tech_mask = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*------------------------------------------------------------------------- 
     Get the array of changed identifiers and for every changed identifier
     get the changed parameters
  -------------------------------------------------------------------------*/

  if(profiles == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params is NULL");
    goto func_exit;
  }

  ds_3gpp_tech_params_ptr = (ds_profile_db_3gpp_tech_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_3gpp_tech_params_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_3gpp_tech_params_ptr failed");
    goto func_exit;
  }
  memset(ds_3gpp_tech_params_ptr, 0, sizeof(ds_profile_db_3gpp_tech_params_type));


  ds_profile_db_gen_params_ptr = (ds_profile_db_gen_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_gen_params_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_profile_db_gen_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_profile_db_gen_params_ptr failed");
    goto func_exit;
  }
  memset(ds_profile_db_gen_params_ptr, 0, sizeof(ds_profile_db_gen_params_type));

  memset((void *)&db_info_type_s,0,sizeof(ds_profile_db_info_type));

  result_type = ds_profile_db_get_tech_params(*profiles,tech_type,(void *)
                                                  ds_3gpp_tech_params_ptr,
                                                  ds_profile_db_gen_params_ptr);

  if(result_type != DB_RESULT_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Cannot get the C profile params from DB. DB Error: %d", 
                       result_type);
    goto func_exit;
  }

  subs_id =  dsumts_subs_mgr_get_subs_id_from_db_subs_id(ds_profile_db_gen_params_ptr->
                                                         subscription_id);

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     DS_3GPP_MSG1_ERROR("Invalid Subs id:%d",subs_id);
     goto func_exit;
  }

  profile_number = ds_profile_db_gen_params_ptr->profile_number;

/*--------------------------------------------------------------------------------- 
   Old profile_data before a particular value is overwritten
   Used to save old_pdp_profile_info
   --------------------------------------------------------------------------------*/

  old_pdp_profile_data_ptr = (ds_umts_pdp_profile_type*) 
                        modem_mem_alloc(sizeof(ds_umts_pdp_profile_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
   Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(old_pdp_profile_data_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to old_pdp_profile_data_ptr failed");
    goto func_exit;
  }
  memset(old_pdp_profile_data_ptr, 0, sizeof(ds_umts_pdp_profile_type));

  result = ds_umts_get_pdp_profile_all_data_per_subs(profile_number,
                                                     subs_id, 
                                                     old_pdp_profile_data_ptr);

  if(( result != DS_UMTS_PDP_SUCCESS ) || 
     (!old_pdp_profile_data_ptr->context.valid_flg))
  {
    DS_3GPP_MSG3_ERROR("Error in retrieving definition for prof number:%d"
                       "result:%d, valid_flag:%d",
                       profile_number,result,
                       old_pdp_profile_data_ptr->context.valid_flg);
  }
  else
  {

    old_pdp_type = old_pdp_profile_data_ptr->context.pdp_type;
	DS_3GPP_MSG1_HIGH("Old PDP type is %d",old_pdp_type);
  }

  result_type = ds_profile_db_get_changed_idents(*profiles,
                DB_TECH_TO_TECHMASK(tech_type),&ident_ptr,
                &ident_count,&gen_ident_ptr,&gen_ident_count);

  DS_3GPP_MSG2_HIGH("Ident_cnt:%d, Gen_Ident_cnt:%d",ident_count,gen_ident_count);

  if(result_type != DB_RESULT_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Cannot get the identifiers from Profile DB, DB Error: %d", 
                       result_type);
    goto func_exit;
  }

  /*--------------------------------------------------------------------------------- 
    Old profile_data before a particular value is overwritten
    Used to save old_pdp_profile_info
    --------------------------------------------------------------------------------*/

   result = ds_umts_get_pdp_profile_all_data_per_subs(profile_number,
                                                      subs_id, 
                                                      old_pdp_profile_data_ptr);

   if(( result != DS_UMTS_PDP_SUCCESS ) || 
      (!(old_pdp_profile_data_ptr->context.valid_flg)))
   {
     DS_3GPP_MSG3_ERROR("Error in retrieving definition for prof number:%d"
                        "result:%d, valid_flag:%d",
                        profile_number,result,
                        old_pdp_profile_data_ptr->context.valid_flg);
   }
   else
   {

     old_pdp_type = old_pdp_profile_data_ptr->context.pdp_type;
   }

  /*------------------------------------------------------------------------------- 
    Handle ident ptr for 3gpp common params
    -------------------------------------------------------------------------------*/
  if(ident_ptr != NULL)
  {
    for(ident_index = 0; ident_index < ident_count; ident_index++)
    {  
      if((ident_ptr + ident_index) == NULL)
      {
        continue;   
      }            
      dsi_profile_3gpp_modem_write_ident_info(profile_number,
                                      *(ident_ptr + ident_index),ds_3gpp_tech_params_ptr,
                                      ds_profile_db_gen_params_ptr,tech_mask,
				      apn_param_check,
				      old_pdp_profile_data_ptr);
    }
  }

    /*------------------------------------------------------------------------------- 
    Handle gen ident ptr
    -------------------------------------------------------------------------------*/
  if(gen_ident_ptr != NULL)
  {
    for(ident_index = 0; ident_index < gen_ident_count; ident_index++)
    {     
      if((gen_ident_ptr + ident_index) == NULL)
      {
        continue;   
      }          
      dsi_profile_3gpp_modem_write_ident_info(profile_number,
                                 *(gen_ident_ptr + ident_index),ds_3gpp_tech_params_ptr,
                                 ds_profile_db_gen_params_ptr,tech_mask,
				 apn_param_check,
				 old_pdp_profile_data_ptr);
    }
  }

  /*---------------------------------------------------------------------------------
    Get the tech_mask, If the profile is of EPC type(tech_mask == EPC_PROFILE_TECH_MASK), fetch the tech
    common parameters for non 3gpp portions too
    -------------------------------------------------------------------------------------------*/
  tech_mask = ds_profile_db_get_supported_tech_mask(*profiles);

  if(tech_mask == EPC_PROFILE_TECH_MASK)
  {
    tech_type = DB_TECH_COMMON;
    result_type = ds_profile_db_get_tech_params(*profiles,tech_type,(void *)
                                &(ds_3gpp_tech_params_ptr->tech_common_params),
                                                      ds_profile_db_gen_params_ptr);

    if(result_type != DB_RESULT_SUCCESS)
    {
      DS_3GPP_MSG1_ERROR("Cannot get the C profile params from DB. DB Result: %d",
                         result_type );
      goto func_exit;
    }

    ds_profile_db_free_idents(ident_ptr);
    ds_profile_db_free_idents(gen_ident_ptr);

    result_type = ds_profile_db_get_changed_idents(*profiles,
                                                       DB_TECH_TO_TECHMASK(tech_type),
                                                       &ident_ptr,
                                                       &ident_count,
                                                       &gen_ident_ptr,
                                                       &gen_ident_count);

    DS_3GPP_MSG2_HIGH("Ident_cnt:%d, Gen_Ident_cnt:%d",ident_count,gen_ident_count);

    if(result_type != DB_RESULT_SUCCESS)
    {
      DS_3GPP_MSG1_ERROR("Cannot get the identifiers from Profile DB. DB Result: %d",
                         result_type);
      goto func_exit;
    }

    /*------------------------------------------------------------------------------- 
      Handle ident ptr for tech common params(non 3gpp part)
    -------------------------------------------------------------------------------*/
    if(ident_ptr != NULL)
    {
      for(ident_index = 0; ident_index < ident_count; ident_index++)
      {    
        if((ident_ptr + ident_index) == NULL)
        {
          continue;   
        }          
        dsi_profile_3gpp_modem_write_ident_info(profile_number,
             *(ident_ptr + ident_index),ds_3gpp_tech_params_ptr,
              ds_profile_db_gen_params_ptr,tech_mask,
	      apn_param_check,
	      old_pdp_profile_data_ptr);
      }
    }
  }  

    /*-------------------------------------------------------------------------------- 
    Check if apn_param_changed has been set to TRUE, If it has been set
    post a cmd for 3gpp MH to handle it
    -------------------------------------------------------------------------------*/
  apn_param_changed = dsi_profile_3gpp_get_apn_changed_param();

  if((apn_param_changed) && 
     ds_3gpp_nv_manager_get_apn_param_chg_cfg((sys_modem_as_id_e_type)subs_id))
  {
    ds_3gpp_profile_post_param_chg_cmd_to_ds_task(profile_number,
                                                  subs_id,
                                                  (void *)old_pdp_type,
                                                  DS_3GPP_PROFILE_APN_PARAM_CHG);
  }

  dsi_profile_3gpp_reset_apn_changed_param();

  func_exit:
  /*-------------------------------------------------------------------------- 
     Free the identifiers and managed profile
  --------------------------------------------------------------------------*/
    if (old_pdp_profile_data_ptr != NULL) 
    {
      modem_mem_free(old_pdp_profile_data_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }

    if (ds_3gpp_tech_params_ptr != NULL) 
    {
      modem_mem_free(ds_3gpp_tech_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }

    if (ds_profile_db_gen_params_ptr != NULL) 
    {
      modem_mem_free(ds_profile_db_gen_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }
    ds_profile_db_free_idents(ident_ptr);
    ds_profile_db_free_idents(gen_ident_ptr);
    ds_profile_db_free_profiles(profiles,profile_count);
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_DELETE_CMD_HDLR

DESCRIPTION
   Posted by MH in response to profile_delete event received from Profile DB
                        
PARAMETERS: 
   ds_profile_db_profile_managed_type* profiles
   ds_profile_db_tech_type tech_type;
   ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_p;
   ds_profile_db_gen_params_type *ds_profile_db_gen_params_p;
   uint16 profile_count
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_reset_hdlr
(
   ds_profile_db_profile_managed_type **profiles,
   uint16 profile_count
)
{
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr = NULL;
  ds_profile_db_gen_params_type *ds_profile_db_gen_params_ptr = NULL;
  ds_profile_db_tech_type tech_type = DB_TECH_3GPP;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  ds_profile_db_supported_tech_mask tech_mask;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(profiles == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params is NULL");
    goto func_exit;
  }

  ds_3gpp_tech_params_ptr = (ds_profile_db_3gpp_tech_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_3gpp_tech_params_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_3gpp_tech_params_ptr failed");
    goto func_exit;
  }
  memset(ds_3gpp_tech_params_ptr, 0, sizeof(ds_profile_db_3gpp_tech_params_type));

  ds_profile_db_gen_params_ptr = (ds_profile_db_gen_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_gen_params_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_profile_db_gen_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_profile_db_gen_params_ptr failed");
    goto func_exit;
  }
  memset(ds_profile_db_gen_params_ptr, 0, sizeof(ds_profile_db_gen_params_type));

  result_type = ds_profile_db_get_tech_params(*profiles,tech_type,(void *)
                        ds_3gpp_tech_params_ptr,ds_profile_db_gen_params_ptr);


  if(result_type != DB_RESULT_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Cannot reset the cache entry. DB Result: %d", result_type);
    goto func_exit;
  }

  subs_id =  dsumts_subs_mgr_get_subs_id_from_db_subs_id(ds_profile_db_gen_params_ptr->
                                             subscription_id);

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     DS_3GPP_MSG1_ERROR("Invalid Subs id:%d",subs_id);
     goto func_exit;
  }

  ds_umts_pdp_profile_ptr =  ds_3gpp_profile_cache_convert_profile_db_params
  ((void *)ds_3gpp_tech_params_ptr,ds_profile_db_gen_params_ptr);

  if(ds_umts_pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cache ptr not populated");
    goto func_exit;
  }

  tech_mask = ds_profile_db_get_supported_tech_mask(*profiles);

  ds_3gpp_profile_cache_convert_profile_db_common_tech_params
    ((void *)ds_3gpp_tech_params_ptr,ds_umts_pdp_profile_ptr,tech_mask);

  func_exit:      
    if (ds_3gpp_tech_params_ptr != NULL) 
    {
      modem_mem_free(ds_3gpp_tech_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }

    if (ds_profile_db_gen_params_ptr != NULL) 
    {
      modem_mem_free(ds_profile_db_gen_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }
    ds_profile_db_free_profiles(profiles,profile_count);
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CLEAR_SUBS_HDLR

DESCRIPTION
   Posted by MH in response to profile_delete event received from Profile DB
                        
PARAMETERS: 
   ds_profile_db_profile_managed_type* profiles
   ds_profile_db_tech_type tech_type;
   ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_p;
   ds_profile_db_gen_params_type *ds_profile_db_gen_params_p;
   uint16 profile_count
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_clear_subs_hdlr
(
   ds_profile_db_profile_managed_type **profiles,
   uint16 profile_count,
   ds_umts_pdp_subs_e_type subs_id
)
{
  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     DS_3GPP_MSG1_ERROR("Invalid Subs id:%d",subs_id);
     return;
  }
  ds_3gpp_profile_cache_entry_delete_all(subs_id);
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_DELETE_CMD_HDLR

DESCRIPTION
   Posted by MH in response to profile_delete event received from Profile DB
                        
PARAMETERS: 
   ds_profile_db_profile_managed_type* profiles
   ds_profile_db_tech_type tech_type;
   ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_p;
   ds_profile_db_gen_params_type *ds_profile_db_gen_params_p;
   uint16 profile_count
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_delete_hdlr
(
   ds_profile_db_profile_managed_type **profiles,
   uint16 profile_count
)
{
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr = NULL;
  ds_profile_db_gen_params_type *ds_profile_db_gen_params_ptr = NULL;
  ds_profile_db_tech_type tech_type = DB_TECH_3GPP;
  ds_umts_pdp_subs_e_type subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(profiles == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params is NULL");
    goto func_exit;
  }

   ds_3gpp_tech_params_ptr = (ds_profile_db_3gpp_tech_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_3gpp_tech_params_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_3gpp_tech_params_ptr failed");
    goto func_exit;
  }
  memset(ds_3gpp_tech_params_ptr, 0, sizeof(ds_profile_db_3gpp_tech_params_type));

  ds_profile_db_gen_params_ptr = (ds_profile_db_gen_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_gen_params_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_profile_db_gen_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_profile_db_gen_params_ptr failed");
    goto func_exit;
  }
  memset(ds_profile_db_gen_params_ptr, 0, sizeof(ds_profile_db_gen_params_type));


  result_type = ds_profile_db_get_tech_params(*profiles,tech_type,(void *)
                        ds_3gpp_tech_params_ptr,ds_profile_db_gen_params_ptr);

  if(result_type != DB_RESULT_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Cannot delete the cache entry, DB Result: %d", result_type);
    goto func_exit;
  }

  subs_id =  dsumts_subs_mgr_get_subs_id_from_db_subs_id(ds_profile_db_gen_params_ptr->
                                                         subscription_id);

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     DS_3GPP_MSG1_ERROR("Invalid Subs id:%d",subs_id);
     goto func_exit;
  }

  ds_3gpp_profile_cache_entry_delete(
     ds_profile_db_gen_params_ptr->profile_number, 
     subs_id,
     !ds_profile_db_gen_params_ptr->persistent);
 
  func_exit:
    if (ds_3gpp_tech_params_ptr != NULL) 
    {
      modem_mem_free(ds_3gpp_tech_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }

    if (ds_profile_db_gen_params_ptr != NULL) 
    {
      modem_mem_free(ds_profile_db_gen_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
    }
    ds_profile_db_free_profiles(profiles,profile_count);
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_SETUP_DEFAULT_PROFILES

DESCRIPTION
   initialize default profiles by reading configuration file . Create default profiles
   if already does not exist.
                        
PARAMETERS: 
   subs_id: Subscription ID
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/


void ds_3gpp_profile_setup_default_profiles
(
   ds_umts_pdp_subs_e_type subs_id
)
{
  boolean emb_def_prof_present = FALSE;
  boolean tet_def_prof_present = FALSE;
  ds_profile_db_result_type status_type = DB_RESULT_FAIL;
  uint16 embedded_profile_num = DS_3GPP_DEF_EMBEDDED_PROFILE;
  uint16 tethered_profile_num =  DS_3GPP_DEF_TETHERED_PROFILE;
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  ds_profile_db_profile_managed_type* prf_ptr = NULL;
  ds_profile_db_creation_params_type params;

  if(ds_umts_pdp_check_subscription_id(subs_id) == DS_UMTS_PDP_INVALID_SUBS_ID)
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*Reset global cache and already read variable for nv refresh cases*/
  cached_embedded_call_profile_number[subs_id] = 0;
  embedded_call_profile_number_read[subs_id]  = FALSE;
  cached_tethered_call_profile_number[subs_id] = 0;
  tethered_call_profile_number_read[subs_id] = FALSE;
   /*--------------------------------------------------------------------------------------- 
     Read the embedded/tethered default profile numbers from the EFS
     IF the values don't exist init them to 1
    
     Now check if these profiles are present in the cache and if they were initialized as part
     of powerup event, If they are not present update both the values to 1 and invoke Profile
     DB to create the profile
   --------------------------------------------------------------------------------------*/
   status = ds_umts_get_default_profile_number_internal(
      DS_UMTS_EMBEDDED_PROFILE_FAMILY,subs_id, &embedded_profile_num);

   if (status != DS_UMTS_PDP_SUCCESS) 
   {
      DS_3GPP_MSG0_ERROR("Setting embedded profile to def value");
      embedded_profile_num = DS_3GPP_DEF_EMBEDDED_PROFILE;
      cached_embedded_call_profile_number[subs_id] = DS_3GPP_DEF_EMBEDDED_PROFILE;
      embedded_call_profile_number_read[subs_id]  = TRUE;
   }

   status = ds_umts_get_default_profile_number_internal(
      DS_UMTS_TETHERED_PROFILE_FAMILY, subs_id, &tethered_profile_num);

   if (status != DS_UMTS_PDP_SUCCESS) 
   {
      DS_3GPP_MSG0_ERROR("Setting embedded profile to def value");
      tethered_profile_num = DS_3GPP_DEF_TETHERED_PROFILE;
      cached_tethered_call_profile_number[subs_id] = DS_3GPP_DEF_TETHERED_PROFILE;
      tethered_call_profile_number_read[subs_id] = TRUE;
   }

   emb_def_prof_present = ds_3gpp_profile_cache_check_if_default_profile_exists
      (embedded_profile_num, subs_id);


   if (!emb_def_prof_present) 
   {
      DS_3GPP_MSG0_HIGH("Updating Embedded profile to the def value");
      cached_embedded_call_profile_number[subs_id] = DS_3GPP_DEF_EMBEDDED_PROFILE;
      embedded_call_profile_number_read[subs_id]  = TRUE;
   }

   tet_def_prof_present = ds_3gpp_profile_cache_check_if_default_profile_exists
                     (tethered_profile_num, subs_id);

   if (!tet_def_prof_present) 
   {
      DS_3GPP_MSG0_HIGH("Updating Tethered profile to the def value");
      cached_tethered_call_profile_number[subs_id] = DS_3GPP_DEF_TETHERED_PROFILE;
      tethered_call_profile_number_read[subs_id] = TRUE;
   }

   if ((!emb_def_prof_present) || (!tet_def_prof_present)) 
   {
      /* Create a non persistent profile*/
      DS_3GPP_MSG0_HIGH("Creating default profile as default profile is absent");
      ds_profile_db_init_creation_params(&params);
      params.subscription_id = (ds_profile_db_subs_type)(subs_id+1);
      prf_ptr = ds_profile_db_alloc(DB_TECH_TO_TECHMASK(DS_PROFILE_TECH_3GPP), params.subscription_id);

      if (prf_ptr != NULL)
      {
         params.persistent = TRUE;
         /*--------------------------------------------------------------------------- 
           Create profiles based on the embedded and tethered flags set.
           If both profiles are not present, then either default or embedded profiles
           can be used.
         ----------------------------------------------------------------------------*/
         if (emb_def_prof_present) 
         {
            params.profile_num = DS_3GPP_DEF_TETHERED_PROFILE;
         } 
         else 
         {
            params.profile_num = DS_3GPP_DEF_EMBEDDED_PROFILE;
         }

         status_type = ds_profile_db_create(&params, prf_ptr);

         /*Retry if File excess error*/
         if (status_type != DB_RESULT_SUCCESS) 
         {
            DS_3GPP_MSG1_ERROR("Persistent profile creation failed. DB Result: %d", 
                               status_type);
            status_type = ds_profile_db_dealloc(prf_ptr);
            if (status_type != DB_RESULT_SUCCESS) 
            {
              DS_3GPP_MSG1_ERROR("Profile dealloc failed, DB Result %d", status_type);
              ASSERT(0);
              goto clean_up;
            }
            ds_profile_db_init_creation_params(&params);
            params.subscription_id = (ds_profile_db_subs_type)(subs_id+1);
            if (emb_def_prof_present) 
            {
              params.profile_num = DS_3GPP_DEF_TETHERED_PROFILE;
            } 
            else 
            {
              params.profile_num = DS_3GPP_DEF_EMBEDDED_PROFILE;
            }
            prf_ptr = ds_profile_db_alloc(DB_TECH_TO_TECHMASK(DS_PROFILE_TECH_3GPP), params.subscription_id);
            params.persistent = FALSE;
            status_type = ds_profile_db_create(&params, prf_ptr);
         }
         if (status_type != DB_RESULT_SUCCESS) 
         {
            DS_3GPP_MSG1_ERROR("Profile creation failed, DB Result: %d", status_type);
            ASSERT(0);
            goto clean_up;
         }

clean_up:
         status_type = ds_profile_db_dealloc(prf_ptr);
         if (status_type != DB_RESULT_SUCCESS) 
         {
            DS_3GPP_MSG1_ERROR("Profile dealloc failed. DB result: %d", status_type);
            ASSERT(0);
            return;
         }
      } 
      else 
      {
         DATA_ERR_FATAL("prf_ptr mem allocation failed");
         return;
      }
   }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_POWER_UP_CMD_HDLR

DESCRIPTION
   Posted by MH in response to power_up event received from Profile DB
                        
PARAMETERS: 
   ds_profile_db_profile_managed_type* profiles
   ds_profile_db_tech_type tech_type;
   ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_p;
   ds_profile_db_gen_params_type *ds_profile_db_gen_params_p;
   uint16 profile_count
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_power_up_hdlr
(
   ds_profile_db_profile_managed_type **profiles,
   uint16 profile_count,
   ds_umts_pdp_subs_e_type subs_id
)
{
  ds_profile_db_profile_managed_type **temp_profiles = profiles;
  ds_profile_db_ident_type *ident_ptr = NULL;
  ds_profile_db_ident_type *gen_ident_ptr = NULL;
  uint16 ident_count = 0;
  uint16 gen_ident_count = 0;
  int ident_index = 0;
  uint16 profile_number = 0;
  int boot_up_profile_index = 0;
  boolean ret_val = FALSE;
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr = NULL;
  ds_profile_db_gen_params_type *ds_profile_db_gen_params_ptr = NULL;
  ds_profile_db_supported_tech_mask tech_mask = 0;
  ds_profile_db_tech_type tech_type = DB_TECH_3GPP;
  boolean apn_param_check = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     DS_3GPP_MSG1_ERROR("Invalid Subs id:%d",subs_id);
     return;
  }

  if (profile_count > (DS_UMTS_MAX_PDP_PROFILE_NUM - DS_UMTS_MAX_PDP_TRANSIENT_PROFILE_NUM)) 
  {
     DS_3GPP_MSG2_HIGH("profile_count: %d, max_supported: %d",profile_count, 
                       DS_UMTS_MAX_PDP_PROFILE_NUM - DS_UMTS_MAX_PDP_TRANSIENT_PROFILE_NUM);
  }

  ds_3gpp_tech_params_ptr = (ds_profile_db_3gpp_tech_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_3gpp_tech_params_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tech_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_3gpp_tech_params_ptr failed");
    goto func_exit;
  }

  ds_profile_db_gen_params_ptr = (ds_profile_db_gen_params_type*) 
                        modem_mem_alloc(sizeof(ds_profile_db_gen_params_type),
                        MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(ds_profile_db_gen_params_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to ds_profile_db_gen_params_ptr failed");
    goto func_exit;
  }


  for(boot_up_profile_index = 0;boot_up_profile_index < profile_count;
      boot_up_profile_index++, temp_profiles++)
  {

    tech_type = DB_TECH_3GPP;
    tech_mask = 0;
    ident_count = 0;
    gen_ident_count = 0;
    memset((void *)ds_3gpp_tech_params_ptr,0,
                   sizeof(ds_profile_db_3gpp_tech_params_type));

    memset((void *)ds_profile_db_gen_params_ptr,0,
                  sizeof(ds_profile_db_gen_params_type));


    tech_type = DB_TECH_3GPP;

    /*------------------------------------------------------------------------ 
     Now query the Profile DB to get the C specific structures and thereby get
     the profile number to update the cache
    --------------------------------------------------------------------------*/
    result_type = ds_profile_db_get_tech_params(*temp_profiles,tech_type,(void *)
                    ds_3gpp_tech_params_ptr,ds_profile_db_gen_params_ptr);

    if(result_type != DB_RESULT_SUCCESS)
    {
      DS_3GPP_MSG2_ERROR("Cannot update the cache for %d, DB Result: %d ",
                         ds_profile_db_gen_params_ptr->profile_number,
                         result_type);
       continue;
    }

    if (dsumts_subs_mgr_get_subs_id_from_db_subs_id(ds_profile_db_gen_params_ptr->subscription_id) != subs_id) 
    {
       DS_3GPP_MSG2_ERROR("Subscription ID in profile does not match"
                          "Subs for event: %d, profile subs_id: %d",subs_id,
                          ds_profile_db_gen_params_ptr->subscription_id - 1);
       continue;
    }

    ret_val = ds_3gpp_profile_cache_entry_create(
               ds_profile_db_gen_params_ptr->profile_number,
               dsumts_subs_mgr_get_subs_id_from_db_subs_id(ds_profile_db_gen_params_ptr->subscription_id),
               !ds_profile_db_gen_params_ptr->persistent);

    if(ret_val)
    {
      ds_3gpp_profile_cache_write_readonly_gen_params
                           (ds_profile_db_gen_params_ptr);

      profile_number = ds_profile_db_gen_params_ptr->profile_number;

      result_type = ds_profile_db_get_changed_idents(*temp_profiles,
                    DB_TECH_TO_TECHMASK(tech_type),&ident_ptr,
                    &ident_count,&gen_ident_ptr,&gen_ident_count);

      DS_3GPP_MSG2_HIGH("Ident_cnt:%d, Gen_Ident_cnt:%d",ident_count,gen_ident_count);

      if(result_type != DB_RESULT_SUCCESS)
      {
        DS_3GPP_MSG1_ERROR("Cannot get the identifiers from Profile DB. DB Result %d", 
                           result_type);
        continue;
      }

      /*------------------------------------------------------------------------------- 
        Handle ident ptr for 3gpp common params
        -------------------------------------------------------------------------------*/
      if(ident_ptr != NULL)
      {
        for(ident_index = 0; ident_index < ident_count; ident_index++)
        { 
          if((ident_ptr + ident_index) == NULL)
          {
            DS_3GPP_MSG0_ERROR("ident_ptr has less idents than ident_count indicates");
            continue;   
          }               
          dsi_profile_3gpp_modem_write_ident_info(profile_number,
                                          *(ident_ptr + ident_index),ds_3gpp_tech_params_ptr,
                                          ds_profile_db_gen_params_ptr,tech_mask,
					  apn_param_check,
				          NULL);
        }
        ds_profile_db_free_idents(ident_ptr);
        ident_ptr = NULL;
      }

        /*------------------------------------------------------------------------------- 
        Handle gen ident ptr
        -------------------------------------------------------------------------------*/

      if(gen_ident_ptr != NULL)
      {
        for(ident_index = 0; ident_index < gen_ident_count; ident_index++)
        {   
          if((gen_ident_ptr + ident_index) == NULL)
          {
            DS_3GPP_MSG0_ERROR("gen_ident_ptr has less idents than gen_ident_count indicates");
            continue;   
          }           
          dsi_profile_3gpp_modem_write_ident_info(profile_number,
                                     *(gen_ident_ptr + ident_index),ds_3gpp_tech_params_ptr,
                                     ds_profile_db_gen_params_ptr,tech_mask,
				     apn_param_check,
				     NULL);
        }
        ds_profile_db_free_idents(gen_ident_ptr);
        gen_ident_ptr = NULL;
      }
      /*---------------------------------------------------------------------------------
        Get the tech_mask, If the profile is of EPC type(tech_mask == 7), fetch the tech
        common parameters for non 3gpp portions too
        -------------------------------------------------------------------------------------------*/
      tech_mask = ds_profile_db_get_supported_tech_mask(*temp_profiles);

      if(tech_mask == EPC_PROFILE_TECH_MASK)
      {
        tech_type = DB_TECH_COMMON;
        result_type = ds_profile_db_get_tech_params(*temp_profiles,tech_type,(void *)
                                  &(ds_3gpp_tech_params_ptr->tech_common_params),
                                                      ds_profile_db_gen_params_ptr);

        if(result_type != DB_RESULT_SUCCESS)
        {
          DS_3GPP_MSG1_ERROR("Cannot get the C profile params from DB. DB Result: %d", 
                             result_type);
          continue;
        }


        result_type = ds_profile_db_get_changed_idents(*temp_profiles,
                                                       DB_TECH_TO_TECHMASK(tech_type),
                                                       &ident_ptr,
                                                       &ident_count,
                                                       &gen_ident_ptr,
                                                       &gen_ident_count);

        DS_3GPP_MSG2_HIGH("Ident_cnt:%d, Gen_Ident_cnt:%d",ident_count,gen_ident_count);

        if(result_type != DB_RESULT_SUCCESS)
        {
          DS_3GPP_MSG1_ERROR("Cannot get the identifiers from Profile DB. DB Result %d", 
                             result_type);
          continue;
        }

        /*------------------------------------------------------------------------------- 
          Handle ident ptr for tech common params(non 3gpp part)
        -------------------------------------------------------------------------------*/
        if(ident_ptr != NULL)
        {
          for(ident_index = 0; ident_index < ident_count; ident_index++)
          {     
            if((ident_ptr + ident_index) == NULL)
            {
              DS_3GPP_MSG0_ERROR("ident_ptr has less idents than ident_count indicates");
              continue;   
            }          
            dsi_profile_3gpp_modem_write_ident_info(profile_number,
                                                 *(ident_ptr + ident_index),ds_3gpp_tech_params_ptr,
                                                 ds_profile_db_gen_params_ptr,tech_mask,
						 apn_param_check,
		  		                 NULL);
           }
           ds_profile_db_free_idents(ident_ptr);
           ident_ptr = NULL;
        }
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Cannot can't get cache for profiles... continue");
      continue;
    }
  }

  ds_3gpp_profile_setup_default_profiles(subs_id);
  ds_profile_db_free_profiles(profiles,profile_count);

func_exit:
  if (ds_3gpp_tech_params_ptr != NULL) 
  {
    modem_mem_free(ds_3gpp_tech_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
  }

  if (ds_profile_db_gen_params_ptr != NULL) 
  {
    modem_mem_free(ds_profile_db_gen_params_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_EVENT_CB

DESCRIPTION
   Called by Profile DB whenever a profile is created, deleted or modified.
    
PARAMETERS: 
  ds_profile_db_event_type event,
  ds_profile_db_profile_managed_type* profiles,
  uint16 profiles_count,
  void* user_data 

DEPENDENCIES
  None.
  
RETURN VALUE 
  None 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_event_cb
(
   ds_profile_db_event_type event_name,
   ds_profile_db_profile_managed_type  **profiles,
   uint16 profile_count,
   void* user_data,
   ds_profile_db_subs_type subs
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
   Check the profile count for all events except for powerup event
   -------------------------------------------------------------------------*/
  if(event_name != DB_POWER_UP_EVENT &&
     event_name != DB_REFRESH_PROFILE_EVENT)
  {
    if(profile_count > 1)
    {
      DS_3GPP_MSG0_ERROR("Profile count invalid, Return");
      return; 
    }
  }

  switch(event_name)
  {
    case DB_CREATE_PROFILE_EVENT:
     DS_3GPP_MSG0_HIGH("Create profile ev callback");
     ds_3gpp_profile_create_hdlr(profiles,profile_count);
     break;

    case DB_MODIFY_PROFILE_EVENT:
     DS_3GPP_MSG0_HIGH("Update profile ev callback");
     ds_3gpp_profile_modify_hdlr(profiles,profile_count);
     break;

    case DB_DELETE_PROFILE_EVENT:
     DS_3GPP_MSG0_HIGH("Delete profile ev callback");
     ds_3gpp_profile_delete_hdlr(profiles,profile_count);
     break;

    case DB_POWER_UP_EVENT:
     DS_3GPP_MSG0_HIGH("Profile Power up event callback");
     ds_3gpp_profile_power_up_hdlr(profiles,profile_count, 
                                   dsumts_subs_mgr_get_subs_id_from_db_subs_id(subs));
     break;
    
    case DB_RESET_PROFILE_EVENT:
      DS_3GPP_MSG0_HIGH("Profile Reset callback event");
      ds_3gpp_profile_reset_hdlr(profiles,profile_count);
      break;

   case DB_REFRESH_PROFILE_EVENT:
     DS_3GPP_MSG0_HIGH("Profile NV Refresh event");
     ds_3gpp_profile_clear_subs_hdlr(profiles,profile_count, 
                                     dsumts_subs_mgr_get_subs_id_from_db_subs_id(subs));
     ds_3gpp_profile_power_up_hdlr(profiles,profile_count, 
                                   dsumts_subs_mgr_get_subs_id_from_db_subs_id(subs));
     break;


    default:
     DS_3GPP_MSG0_HIGH("No operation to be performed by the MH");
     break;
  }

}

#if 0
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_GET_DEFAULT_PROFILE_NUM

DESCRIPTION
  The function fetches the default profile number based on the family
  type passed
    
PARAMETERS: 
 void
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  void
  
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_get_default_profile_num
(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_family_type family
)
{
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_profile_status_etype status = DS_PROFILE_REG_RESULT_FAIL;
  uint32 profile_family = 0;
/*--------------------------------------------------------------------------*/
  profile_family = family;
  status = dsi_profile_3gpp_get_default_profile(profile_family,num);

  if (status != DS_PROFILE_REG_RESULT_FAIL) 
  {
     result_type = DB_RESULT_SUCCESS;
  }
  return result_type;
}

#endif

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_GET_DEFAULT_PROFILE_NUM_PER_SUBS

DESCRIPTION
  The function fetches the default profile number based on the family
  type passed and subs id
    
PARAMETERS: 
 void
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  void
  
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_get_default_profile_num_per_subs
(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs
)
{
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_profile_status_etype status = DS_PROFILE_REG_RESULT_FAIL;
  uint32 profile_family = 0;
/*--------------------------------------------------------------------------*/
  profile_family = family;
  status = dsi_profile_3gpp_get_default_profile_per_subs(profile_family,
                                    (ds_profile_subs_etype)subs,num);

  if (status == DS_UMTS_PDP_FAIL || DS_UMTS_PDP_INVALID_PROFILE_NUM == status) 
  {
    result_type = DB_RESULT_FAIL;
  }
  else
  {
    result_type = DB_RESULT_SUCCESS;
  }
  return result_type;
}


#if 0
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_SET_DEFAULT_PROFILE_NUM

DESCRIPTION
  The function sets the default profile number based on the family
  type passed
    
PARAMETERS: 
 void
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  void
  
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_set_default_profile_num
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type family
)
{
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_profile_status_etype status = DS_PROFILE_REG_RESULT_FAIL;
  uint32 profile_family = 0;
/*--------------------------------------------------------------------------*/
  profile_family = family;
  status = dsi_profile_3gpp_set_default_profile(profile_family,num);

  if (status != DS_PROFILE_REG_RESULT_FAIL) 
  {
    result_type = DB_RESULT_SUCCESS;
  }
  return result_type;
}

#endif

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_SET_DEFAULT_PROFILE_NUM_PER_SUBS

DESCRIPTION
  The function sets the default profile number based on the family
  type passed
    
PARAMETERS: 
 void
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  void
  
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_set_default_profile_num_per_subs
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs
)
{
  ds_profile_db_result_type result_type = DB_RESULT_FAIL;
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  uint32 profile_family = 0;
/*--------------------------------------------------------------------------*/
  profile_family = family;
  status = dsi_profile_3gpp_set_default_profile_number_per_subs
                (profile_family,
                dsumts_subs_mgr_get_subs_id_from_db_subs_id(subs),
                num);

  if (status == DS_UMTS_PDP_FAIL || DS_UMTS_PDP_INVALID_PROFILE_NUM == status) 
  {
    result_type = DB_RESULT_FAIL;
  }
  else
  {
    result_type = DB_RESULT_SUCCESS;
  }

  return result_type;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_API_INIT

DESCRIPTION
  The function registers for callback events with profile DB
    
PARAMETERS: 
 void
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  void
  
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_api_init
(
   void
)
{

/*--------------------------------------------------------------------------*/
  ds_3gpp_callback_id = ds_profile_db_register_for_notifications_MH
               (DB_UNSPECIFIED_PROFILE_NUM,(DB_TECH_TO_TECHMASK(DB_TECH_3GPP)| 
                DB_TECH_TO_TECHMASK(DB_TECH_COMMON)),TRUE,
                ds_3gpp_profile_event_cb,
                NULL);

  if(ds_3gpp_callback_id == 0)
  {
    DATA_ERR_FATAL("Unable to register with Profile DB");
  }

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_FUNCTION_TABLE_INIT

DESCRIPTION
  The function registers function table with profile DB
    
PARAMETERS: 
 void
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  void
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_function_table_init
(
   void
)
{
  tech_func_table_type tech_table_3gpp = {0};

  tech_table_3gpp.get_profile                      = &ds_3gpp_profile_get;
  tech_table_3gpp.validation_function              = &ds_3gpp_profile_validation_func;
  tech_table_3gpp.get_default_profile              = &ds_3gpp_profile_get_default_values;
  tech_table_3gpp.get_default_profile_num_per_subs = &ds_3gpp_profile_get_default_profile_num_per_subs;
  tech_table_3gpp.set_default_profile_num_per_subs = &ds_3gpp_profile_set_default_profile_num_per_subs;
  tech_table_3gpp.validate_general                 = &ds_3gpp_profile_validation_gen;

  if ( DB_RESULT_SUCCESS != ds_profile_db_register_function_table_MH(&tech_table_3gpp,
                                            DB_TECH_TO_TECHMASK(DB_TECH_3GPP)))
  {
    DS_3GPP_MSG0_ERROR("Unable to register with Profile DB");
    ASSERT(0);
  }

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATION_FUNCTION

DESCRIPTION
  The function retrieves the function to look up based on the identifier passed
  and validates appropriate parameters.
    
PARAMETERS: 
  ds_profile_db_ident_type  - Identifier type
  ds_profile_db_info_type   - Profile DB info type
  profile_params          - Parameters to validate
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  ds_profile_db_result_type
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validation_func
(
  ds_profile_db_ident_type        ident, 
  const ds_profile_db_info_type  *info,
  const void*                     profile_params,
  ...
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  validate_3gpp_fn_type validate_fn = NULL;
 /*--------------------------------------------------------------------------*/
  if(info == NULL)
  {
    DS_3GPP_MSG0_ERROR("Info ptr is NULL");
    ret_val = DB_RESULT_FAIL;
    return ret_val;
  }

  validate_fn = dsi_profile_3gpp_modem_get_validation_func(ident); 

  if(validate_fn != NULL)
  {
    ret_val = validate_fn(ident,info,profile_params);
  }

  return ret_val;
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATION_GEN

DESCRIPTION
  The function validates general tech specific parameters
    
PARAMETERS: 
  ds_profile_db_3gpp_tech_params_type* profile
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  ds_profile_db_result_type
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_validation_gen
(
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret_val = ds_3gpp_profile_validate_gen_params((ds_profile_db_3gpp_tech_params_type*)profile);

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_GET

DESCRIPTION
  The function extracts from cache the tech and gen params based on the profile
  number passed.
    
PARAMETERS: 
  ds_profile_db_profile_num_type num, 
  ds_profile_db_3gpp_tech_params_type* tech_params,
  ds_profile_db_gen_params_type* gen_params
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  ds_profile_db_result_type
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_get
(
  ds_profile_db_profile_num_type num, 
  void* tech_params,
  ds_profile_db_gen_params_type* gen_params,
  ds_profile_db_subs_type subs_id
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ret_val = ds_3gpp_profile_cache_convert_to_profile_db(num, 
                                                        subs_id , 
                                                        (ds_profile_db_3gpp_tech_params_type*)tech_params,
                                                        gen_params);
  return ret_val;

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_GET_DEFAULT_PARAMS

DESCRIPTION
  The function extracts from the default tech params 
    
PARAMETERS: 
   ds_profile_db_3gpp_tech_params_type* tech_profile_ptr

DEPENDENCIES
  None.
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_get_default_values
(
  void* tech_profile_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(tech_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("Tech profile ptr is NULL");
    return;
  }

  ds_3gpp_profile_cache_get_default_values((ds_profile_db_3gpp_tech_params_type*)tech_profile_ptr);
  return;

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_GET_SUB_ID

DESCRIPTION
  The function retrieves the subscription ID
    
PARAMETERS: 
  void

DEPENDENCIES
  None.
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_subs_type ds_3gpp_profile_get_default_sub_id
(
   void
)
{
  ds_umts_pdp_subs_e_type data_subs = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  /*- - - - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - - - - - */

  data_subs = dsumts_subs_mgr_get_subs_id(ds3g_get_ps_subs_id());


  if (data_subs ==DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE) 
  {
    data_subs = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1;
  }

  DS_3GPP_MSG1_HIGH("Data Subscription is : %d", data_subs);
  return (ds_profile_db_subs_type)(data_subs+1);
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_IS_EPC_TECH_PARAM_SET

DESCRIPTION
  The function extracts from cache if the tech param was set for a given EPC 
  common parameter
    
PARAMETERS: 
  ds_profile_db_profile_num_type num, 
  ds_profile_db_ident_type ident
  boolean* is_tech_param_set
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  ds_profile_db_result_type
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_is_epc_tech_param_set
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type subs_id, 
  ds_profile_db_ident_type       ident,
  boolean *is_tech_param_set
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ret_val = ds_3gpp_profile_cache_is_epc_tech_param_set(num, 
                             dsumts_subs_mgr_get_subs_id_from_db_subs_id(subs_id), 
                             ident, is_tech_param_set);
  DS_3GPP_MSG1_MED("Ret_val is %d",ret_val);
  return ret_val;
}

