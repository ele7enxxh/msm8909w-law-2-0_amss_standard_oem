 /******************************************************************************
 @file    ds_3gpp_profile_validator.h
 @brief   

  DESCRIPTION
  Tech specific implementation of 3GPP Profile Management  

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
****************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/13   vb      Validate 3gpp parameters
=============================================================================*/

#ifndef DS_3GPP_PROFILE_VALIDATOR_H
#define DS_3GPP_PROFILE_VALIDATOR_H

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_INACTIVITY_TIMER_VAL

DESCRIPTION
  The function validates 3gpp inactivity timer value.
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_validate_inactivity_timer_val
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MNC

DESCRIPTION
  The function validates 3gpp mnc
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_mnc
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MCC

DESCRIPTION
  The function validates 3gpp mcc
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_mcc
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_OPERATOR_PCO

DESCRIPTION
  The function validates 3gpp operator pco
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_operator_pco
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_APN_BEARER

DESCRIPTION
  The function validates 3gpp apn bearer
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_apn_bearer
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_SRC_STAT_DESC

DESCRIPTION
  The function validates 3gpp src stat descriptor for both req and min
  qos
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_src_stat_desc
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_VERSION

DESCRIPTION
  The function validates 3gpp version
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_version
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_LINGER_PARAMS

DESCRIPTION
  The function validates 3gpp linger params
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_linger_params
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_UMTS_GPRS_QOS_PARAMS

DESCRIPTION
  The function validates gprs qos parameters
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the gprs qos parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_gprs_qos_params
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_UMTS_QOS_PARAMS

DESCRIPTION
  The function validates umts qos parameters
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the umts qos parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_umts_qos_params
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);



/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_LTE_QOS_PARAMS

DESCRIPTION
  The function validates lte qos parameters
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the lte qos parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_lte_qos_params
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_GEN_PARAMS

DESCRIPTION
  The function validates general params
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  RESULT_SUCCESS - success
  RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_gen_params
(
   const ds_profile_db_3gpp_tech_params_type *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_H_COMP

DESCRIPTION
  The function validates Header compression 
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the lte qos parameters
 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_h_comp
(
  ds_profile_db_ident_type          ident,
  const ds_profile_db_info_type    *info,
  const void                       *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_PDP_TYPE

DESCRIPTION
  The function validates pdp type 
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the lte qos parameters
 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_pdp_type
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MAX_PDN_CONN_PER_BLK

DESCRIPTION
  The function validates maximum pdn connections per block
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the pdn connections per block
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_max_pdn_conn_per_blk
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MAX_PDN_CONN_TIME

DESCRIPTION
  The function validates maximum pdn conn time
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the pdn connections per block
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_max_pdn_conn_time
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MAX_PDN_REQ_WAIT_TIME

DESCRIPTION
  The function validates maximum pdn req wait time
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the pdn connections per block
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_max_pdn_req_wait_time
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type    *info,
   const void *profile
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_ROAMING_PDP_TYPE

DESCRIPTION
  The function validates roaming pdp type 
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the roaming PDP parameters


DEPENDENCIES
  None.

RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE - otherwise

SIDE EFFECTS 
  None 

===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_roaming_pdp_type
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type  *info,
   const void                     *profile
);
#endif /*DS_3GPP_PROFILE_VALIDATOR_H*/

