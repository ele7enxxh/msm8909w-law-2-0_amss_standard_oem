 /******************************************************************************
 @file    ds_3gpp_profile_cache_hdlr.h
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
03/27/13   vb      Handle profile cache
=============================================================================*/

#ifndef DS_3GPP_PROFILE_CACHE_HDLR_H
#define DS_3GPP_PROFILE_CACHE_HDLR_H

#include "ds_profile_db.h"
#include "dsumtspdpreg.h"
#include "ds_profile_3gppi.h"
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ds_3gppi_utils.h"
#include <string.h>
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "dsumtspdpregint.h"
#include "dsati.h"
#include "ps_sys.h"

#define EPC_PROFILE_TECH_MASK 7
#define EPC_PROFILE_NUMBER_MIN 100

#include "modem_mem.h"

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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_MCC

DESCRIPTION
  The function updates the mnc value to cache
    
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

void ds_3gpp_profile_cache_write_mcc
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_APN_BEARER

DESCRIPTION
  The function updates the apn bearer value to cache
    
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


void ds_3gpp_profile_cache_write_apn_bearer
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_SRC_STAT_DESC_MIN

DESCRIPTION
  The function updates the src stat descriptor value to cache
    
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


void ds_3gpp_profile_cache_write_src_stat_desc_min
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_SRC_STAT_DESC_REQ

DESCRIPTION
  The function updates the src stat descriptor req value to cache
    
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


void ds_3gpp_profile_cache_write_src_stat_desc_req
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_APN_CLASS

DESCRIPTION
  The function updates the APN class descriptor value to cache
    
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

void ds_3gpp_profile_cache_write_apn_class
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_INACTIVITY_TIMER_VAL

DESCRIPTION
  The function updates the inactivity timer value to cache
    
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

void ds_3gpp_profile_cache_write_inactivity_timer_val
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_OPERATOR_RESERVED_PCO

DESCRIPTION
  The function updates the operator reserevd pco value to cache
    
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

void ds_3gpp_profile_cache_write_operartor_reserved_pco
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_APN_DISABLE_FLAG

DESCRIPTION
  The function updates the APN disable value to cache
    
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

void ds_3gpp_profile_cache_write_apn_disable_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_IM_CN_FLAG

DESCRIPTION
  The function updates the IM CN value to cache
    
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

void ds_3gpp_profile_cache_write_im_cn_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG

DESCRIPTION
  The function updates the request_pcscf_address_using_dhcp_flag to cache
    
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

void ds_3gpp_profile_cache_write_request_pcscf_address_using_dhcp_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_REQUEST_DNS_ADDRESS_USING_DHCP_FLAG

DESCRIPTION
  The function updates the request_dns_address_using_dhcp_flag to cache
    
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

void ds_3gpp_profile_cache_write_request_dns_address_using_dhcp_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);
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
  None.
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_write_failure_timer
(
   ds_profile_db_profile_num_type             profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask          tech_mask
);


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_REQUEST_PCSCF_ADDRESS_FLAG

DESCRIPTION
  The function updates the request_pcscf_address_flag to cache
    
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

void ds_3gpp_profile_cache_write_request_pcscf_address_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_AUTH_TYPE

DESCRIPTION
  The function updates the auth type to cache
    
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

void ds_3gpp_profile_cache_write_auth_type
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_AUTH_PASSWORD

DESCRIPTION
  The function updates the auth password to cache
    
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

void ds_3gpp_profile_cache_write_auth_password
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_AUTH_USERNAME

DESCRIPTION
  The function updates the auth username to cache
    
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

void ds_3gpp_profile_cache_write_auth_username
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_QOS_REQ

DESCRIPTION
  The function updates the UMTS QOS REQ
    
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

void ds_3gpp_profile_cache_write_umts_qos_req
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_QOS_REQ_EXTENDED

DESCRIPTION
  The function updates the UMTS QOS REQ Extended
    
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

void ds_3gpp_profile_cache_write_umts_qos_req_extended
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_QOS_MIN

DESCRIPTION
  The function updates the UMTS QOS MIN params
    
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

void ds_3gpp_profile_cache_write_umts_qos_min
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_QOS_MIN_EXTENDED

DESCRIPTION
  The function updates the UMTS QOS MIN EXTENDED params
    
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

void ds_3gpp_profile_cache_write_umts_qos_min_extended
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_LTE_QOS_REQ

DESCRIPTION
  The function updates the LTE QOS params
    
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

void ds_3gpp_profile_cache_write_lte_qos_req
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_GPRS_QOS_REQ

DESCRIPTION
  The function updates the GPRS QOS REQ params
    
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

void ds_3gpp_profile_cache_write_gprs_qos_req
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_GPRS_QOS_MIN

DESCRIPTION
  The function updates the GPRS QOS MIN params
    
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

void ds_3gpp_profile_cache_write_gprs_qos_min
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_SUBSCRIPTION_ID

DESCRIPTION
  The function updates the subscription id
    
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

void ds_3gpp_profile_cache_write_subscription_id
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PROFILE_NAME

DESCRIPTION
  The function updates the profile name
    
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

void ds_3gpp_profile_cache_write_profile_name
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_TYPE

DESCRIPTION
  The function updates the pdp type
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info       --  Contains info about the parameters
   
 
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
);


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_LTE_ROAMING_PDP_TYPE

DESCRIPTION
  The function updates the lte roaming pdp type
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info       --  Contains info about the parameters
   
 
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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_UMTS_ROAMING_PDP_TYPE

DESCRIPTION
  The function updates the umts roaming pdp type
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info       --  Contains info about the parameters
   
 
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
);

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
void   ds_3gpp_profile_cache_write_override_home_pdp_type
(
   ds_profile_db_profile_num_type             profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask          tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_APN

DESCRIPTION
  The function updates the pdp context apn
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info       --  Contains info about the parameters
   
 
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
);

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
);

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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_h_COMP

DESCRIPTION
  The function updates the pdp_context_h_comp
    
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

void ds_3gpp_profile_cache_write_pdp_context_h_comp
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_SECONDARY_FLAG

DESCRIPTION
  The function updates the pdp_context_secondary_flag
    
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

void ds_3gpp_profile_cache_write_pdp_context_secondary_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_NUMBER

DESCRIPTION
  The function updates the pdp_context_number
    
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

void ds_3gpp_profile_cache_write_pdp_context_number
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type			 *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_ACCESS_CTRL_FLAG

DESCRIPTION
  The function updates the access control flag
    
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

void ds_3gpp_profile_cache_write_access_ctrl_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_PRIMARY_ID

DESCRIPTION
  The function updates the pdp_context_primary_id
    
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

void ds_3gpp_profile_cache_write_pdp_context_primary_id
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V4_DNS_ADDR_PRIMARY

DESCRIPTION
  The function updates the v4 primary dns address
    
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

void ds_3gpp_profile_cache_write_v4_dns_addr_primary
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type			 *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V4_DNS_ADDR_SECONDARY

DESCRIPTION
  The function updates the v4 secondary dns address
    
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

void ds_3gpp_profile_cache_write_v4_dns_addr_secondary
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V6_DNS_ADDR_PRIMARY

DESCRIPTION
  The function updates the v6 primary dns address
    
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

void ds_3gpp_profile_cache_write_v6_dns_addr_primary
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V6_DNS_ADDR_SECONDARY

DESCRIPTION
  The function updates the v6 secondary dns address
    
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

void ds_3gpp_profile_cache_write_v6_dns_addr_secondary
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V6_PDP_ADDR

DESCRIPTION
  The function updates the v6 pdp address of the pdp context
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *info      --  Contains info about the parameters
   
 
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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT_V4_PDP_ADDR

DESCRIPTION
  The function updates the v4 pdp address of the pdp context
    
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

void ds_3gpp_profile_cache_write_pdp_context_v4_pdp_addr
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_EMERGENCY_FLAG

DESCRIPTION
  The function updates the emergency flag info
    
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

void ds_3gpp_profile_cache_write_emergency_flag
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_LINGER_PARAMS

DESCRIPTION
  The function updates the linger parameters
    
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

void ds_3gpp_profile_cache_write_linger_params
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_TFT_FILTER_ID1

DESCRIPTION
  The function updates the tft filter id parameters
    
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

void ds_3gpp_profile_cache_write_tft_filter_id1
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_TFT_FILTER_ID2

DESCRIPTION
  The function updates the tft filter id parameters
    
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

void ds_3gpp_profile_cache_write_tft_filter_id2
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_MAX_PDN_CONN_PER_BLOCK

DESCRIPTION
  The function updates the pdp context apn
    
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

void ds_3gpp_profile_cache_write_max_pdn_conn_per_block
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDN_CONN_TIMER

DESCRIPTION
  The function updates the pdp context apn
    
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

void ds_3gpp_profile_cache_write_pdn_conn_timer
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDN_REQ_WAIT_TIMER

DESCRIPTION
  The function updates the pdp context apn
    
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

void ds_3gpp_profile_cache_write_pdn_req_wait_timer
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_USER_APP_DATA

DESCRIPTION
  The function updates the pdp context apn
    
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

void ds_3gpp_profile_cache_write_user_app_data
(
   ds_profile_db_profile_num_type  profile_number,
   const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
   const ds_profile_db_gen_params_type       *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);


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
);


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
);

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
);

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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_ENTRY_CREATE

DESCRIPTION
  The function finds the cache entry corresponding to the profile number
  passed and marks the corresponding context as valid
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  subs_id -- Subscription Id of the profile
  transient_profile -- TRUE if profile is non_persistent, FALSE otherwise 
 
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
   ds_umts_pdp_subs_e_type              subs_id,
   boolean transient_profile
);

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
);

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
);


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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_READONLY_GEN_PARAMS

DESCRIPTION
  The function will copy readonly general parameter to umts_cache
    
PARAMETERS:             
 ds_profile_db_gen_params_type           *gen_params 
 
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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_DEFAULT_VALUES

DESCRIPTION
  The function converts cache params into profile DB params
    
PARAMETERS: 
	ds_profile_db_3gpp_tech_params_type* profile
 
DEPENDENCIES
  None.
  
RETURN VALUE 
 void
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_profile_cache_get_default_values
(
   ds_profile_db_3gpp_tech_params_type* tech_profile_ptr
);

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
);
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
);

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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_CONVERT_PROFILE_DB_COMMON_TECH_PARAMS

DESCRIPTION
  The function converts the profile DB common tech parameters into a
	format recognized by 3gpp MH cache
    
PARAMETERS: 
 void*                                     tech_specific_params,              
 ds_umts_pdp_profile_type 						 	 *ds_umts_pdp_profile_ptr
 
DEPENDENCIES
  None.
  
RETURN VALUE 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/

void ds_3gpp_profile_cache_convert_profile_db_common_tech_params
(
   void                                    *tech_specific_params_ptr,             
   ds_umts_pdp_profile_type              	 *ds_umts_pdp_profile_ptr,
   ds_profile_db_supported_tech_mask 	      tech_mask
);

#if 0
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_PDP_CONTEXT

DESCRIPTION
  The function updates the pdp type
    
PARAMETERS: 
  profile_number  --  Profile number passed by profile DB.
  *ds_3gpp_tech_params_ptr     	  --  ds_profile_db_3gpp_tech_params_type
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
   const ds_profile_db_gen_params_type			 *gen_params,
   ds_profile_db_supported_tech_mask tech_mask
);

#endif

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
);

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
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_IPV6_DELEGATION

DESCRIPTION
  The function updates the IPV6 DELEGATION 
    
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
  ds_profile_db_profile_num_type  profile_number,
  const ds_profile_db_3gpp_tech_params_type *ds_3gpp_tech_params_ptr,
  const ds_profile_db_gen_params_type	   *gen_params,
  ds_profile_db_supported_tech_mask tech_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_WRITE_LTE_ROAMING_PDP_TYPE

DESCRIPTION
  The function updates the LTE roaming pdp type

PARAMETERS: 
  profile_number  --  Profile number passed by profile DB
  *info           --  Contains info about the parameters

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
);

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
);

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
);

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
);

#endif /*DS_3GPP_PROFILE_CACHE_HDLR_H*/
