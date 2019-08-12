/*!
  @file
  ds_eps_pdn_throttle_sm.c


  @brief
  This module facilitate throttling state machine for APN throttling on LTE and handles enabling/disabling 
  of APN and RAT based on the UE and network behavior

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.
  
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/src/ds_eps_pdn_throttle_sm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/14   vs  Initial version.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_LTE

#include "ds_3gppi_utils.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_eps_pdn_context.h"
#include "ds_eps_pdn_throttle_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"

#include "modem_mem.h"
#include "dstask_v.h"
#include "ds3gcmif.h"
#include "ds3gmshif.h"
#include "ds_3gpp_cc_sp_throt_config.h"

/*===========================================================================

                  GLOBAL VARIABLES

===========================================================================*/
/*------------------------------------------------------------------------------
  This is the table that maintains PDN Throttling Info for all Subscriptions
------------------------------------------------------------------------------*/
static ds_eps_pdn_throttle_sm_apn_based_s*
         ds_eps_pdn_throttle_sm_apn_based_tbl_ptr[DS3GSUBSMGR_SUBS_ID_MAX] = {0};

static ds_eps_pdn_throttle_sm_timer_s 
                  plmn_unblock_timer[DS3GSUBSMGR_SUBS_ID_MAX] = {{0}};

static list_type ds_eps_pdn_throt_blocked_plmn_list[DS3GSUBSMGR_SUBS_ID_MAX];

/*===========================================================================

                           MACRO DEFINITIONS

===========================================================================*/ 
#define DS_3GPP_IS_UMTS_MODE(current_mode) \
	((current_mode != SYS_SYS_MODE_LTE)&&(DS_3GPP_IS_3GPP_CURRENT_MODE(current_mode)))

#define DS_3GPP_PS_DOMAIN(current_reg_srv_domain)\
	  (current_reg_srv_domain == SYS_SRV_DOMAIN_PS_ONLY || current_reg_srv_domain == SYS_SRV_DOMAIN_CS_PS)
	  

#define UMTS_PS_NOT_REGISTERED(current_mode, current_srv_status, current_reg_srv_domain) \
         (!DS_3GPP_IS_UMTS_MODE(current_mode)) || \
         (DS_3GPP_IS_UMTS_MODE(current_mode) && ((current_srv_status != SYS_SRV_STATUS_SRV) ||  \
         (!DS_3GPP_PS_DOMAIN(current_reg_srv_domain)))) 

#define UMTS_PS_REGISTERED_PDP_CONTEXT_NOT_UP(current_mode, current_srv_status, current_reg_srv_domain) \
             (DS_3GPP_IS_UMTS_MODE(current_mode)) && ((current_srv_status == SYS_SRV_STATUS_SRV) && \
             (DS_3GPP_PS_DOMAIN(current_reg_srv_domain))) &&\
             (!dsPdnContext_is_any_normal_pdn_coming_up())

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_ALLOCATE_APN_BASED_TBL
 
DESCRIPTION
  This function allocates APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
 
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the newly allocated APN based Tbl Info 
 
SIDE EFFECTS
  None.

===========================================================================*/

static ds_eps_pdn_throttle_sm_apn_based_s*
		      ds_eps_pdn_throt_allocate_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_APN_BASED_TBL
 
DESCRIPTION
  This function fetches the APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the APN based Tbl Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_eps_pdn_throttle_sm_apn_based_s* 
                                ds_eps_pdn_throt_get_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DS_EPS_PDN_THROT_SET_APN_BASED_TBL
 
DESCRIPTION
  This function sets the APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  2. apn_based_tbl_ptr
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE- on success
  FLASE - otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_eps_pdn_throt_set_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id,
  ds_eps_pdn_throttle_sm_apn_based_s *apn_based_tbl_ptr
);

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_GET_THROTTLE_SM_PLMN_BASED

DESCRIPTION
  This function is used to get/create APN based throttle sm table for given APN name

PARAMETERS   : 
  1. apn - APN name
  2. allow_create  -true/false  
  3. Subs Id

DEPENDENCIES
  None.

RETURN VALUE
  pointer to APN based throttling table

SIDE EFFECTS
  None.
===========================================================================*/
static ds_eps_pdn_throttle_sm_apn_based_dyn_s* 
                          ds_eps_pdn_throt_get_apn_index_ptr
(
  byte                     *apn,
  boolean                   allow_create, 
  sys_modem_as_id_e_type	subs_id  
);

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_GET_THROTTLE_SM_PLMN_BASED

DESCRIPTION
  This function is used to get/create APN based throttle sm table for given APN name

PARAMETERS   : 
  1. apn - APN name
  2. allow_create  -true/false  
  3. current_plmn 
  4. Subs Id

DEPENDENCIES
  None.

RETURN VALUE
  pointer to APN based throttling table

SIDE EFFECTS
  None.
===========================================================================*/
static ds_eps_pdn_throttle_sm_plmn_based_s* 
			  ds_eps_pdn_throt_get_throttle_sm_plmn_based
(
  ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr,
  boolean                                 allow_create, 
  sys_plmn_id_s_type                      current_plmn,
  sys_modem_as_id_e_type	              subs_id
);
/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_RESET_THROTTLING_ON_PMN

DESCRIPTION
  This function is used to clear throttling of all APN for given subscription
  
PARAMETERS  
 1. subs_id
 2. plmn_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
static void ds_eps_pdn_throt_reset_throttling_on_plmn
(
  sys_modem_as_id_e_type  subs_id,  
  sys_plmn_id_s_type	  plmn_id 
);
/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_LTE_UNBLOCK_ON_PLMN

DESCRIPTION
  This function unblocks all the blocked PLMNs.

PARAMETERS
 1. subs_id - SUBS_ID 
 2. plmn_id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
static void ds_eps_pdn_throt_enable_lte_unblock_on_plmn
(
  sys_modem_as_id_e_type  subs_id,
  sys_plmn_id_s_type	  plmn_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_THROTTLE_SM_INT

DESCRIPTION
  This function called during powere for each subscription. this fucntion 
  allocates initilized APN based throttle table and plmn unblock timer

PARAMETERS
  1. subs_id 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throttle_sm_init
(
 sys_modem_as_id_e_type  subs_id
)
{
  ds_eps_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr = NULL;
  ds_eps_pdn_throttle_sm_apn_based_dyn_s  *per_apn_throt_sm_ptr = NULL;
  uint32 	                              apn_idx = 0;
  uint32                                  plmn_loop_index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_3GPP_MSG1_HIGH("ds_eps_pdn_throttle_sm_init: subs_id:%d", subs_id);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    subs_id = SYS_MODEM_AS_ID_1;
  }

  if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == FALSE)
  {
    DS_3GPP_MSG0_MED("ds_3gpp_throt_sm_is_cc_sp_throttling_enabled not enabled");
    return;
  }

  /*------------------------------------------------------------------------
  power up init: reset ds_eps_pdn_throttle_sm_apn_based_tbl_ptr table
  ------------------------------------------------------------------------*/
    
  ds_eps_pdn_throt_set_apn_based_tbl(subs_id, NULL);	


  apn_based_tbl_ptr = ds_eps_pdn_throt_allocate_apn_based_tbl(subs_id);
  
  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_LOW("apn_based_tbl_ptr is NULL");
    return;
  }
  
  for(apn_idx = 0; apn_idx < DS_MAX_APN_NUM;apn_idx++)
  {
    per_apn_throt_sm_ptr = 
     (ds_eps_pdn_throttle_sm_apn_based_dyn_s*) 
        modem_mem_alloc(sizeof(ds_eps_pdn_throttle_sm_apn_based_dyn_s),
                         MODEM_MEM_CLIENT_DATA);
  
    if(per_apn_throt_sm_ptr != NULL)
    {
      apn_based_tbl_ptr->apn_based_dyn_tbl[apn_idx]= per_apn_throt_sm_ptr;
  
      /*------------------------------------------------------------------------
        Initialize APN name to NULL
      ------------------------------------------------------------------------*/
      memset( (void*)&(per_apn_throt_sm_ptr->apn), 
               0, 
              (DS_UMTS_MAX_APN_STRING_LEN+1) );
      per_apn_throt_sm_ptr->valid = FALSE;
  
      /*------------------------------------------------------------------------
        Loop over all the PDN throt SMs (per APN)  
      ------------------------------------------------------------------------*/
      for(plmn_loop_index = 0; 
          plmn_loop_index < DS_3GPP_MAX_PLMN_SUPPORTED;
          plmn_loop_index++) 
      {
        per_apn_throt_sm_ptr->apn_throt_sm_plmn_ptr[plmn_loop_index] = NULL;
      } //loop over all PLMN throt sm pointers for a given APN
    }
    else
    {
      DS_3GPP_MSG0_LOW("Could not allocate Per APN throt SM. No PDN throt");
    }  
  }
  
  plmn_unblock_timer[subs_id].throt_timer_val = 0;
  plmn_unblock_timer[subs_id].is_throttled = FALSE;
  plmn_unblock_timer[subs_id].user_data = NULL;
  plmn_unblock_timer[subs_id].subs_id = subs_id;
  rex_def_timer_ex(&(plmn_unblock_timer[subs_id].throttle_timer),
                   (rex_timer_cb_type) ds_eps_pdn_throt_sm_plmn_unblock_timer_cb,
                   (unsigned long)(&plmn_unblock_timer));
	
  list_init(&(ds_eps_pdn_throt_blocked_plmn_list[subs_id]));
}/* ds_eps_pdn_throttle_sm_init */

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_ALLOCATE_APN_BASED_TBL
 
DESCRIPTION
  This function allocates APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  Subs Id
 
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the newly allocated APN based Tbl Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_eps_pdn_throttle_sm_apn_based_s*
         ds_eps_pdn_throt_allocate_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_eps_pdn_throttle_sm_apn_based_s*          apn_based_tbl_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if (ds_eps_pdn_throt_get_apn_based_tbl(subs_id)== NULL)
  {
    apn_based_tbl_ptr =
      (ds_eps_pdn_throttle_sm_apn_based_s*) 
         modem_mem_alloc(sizeof(ds_eps_pdn_throttle_sm_apn_based_s),
                          MODEM_MEM_CLIENT_DATA);

    if (apn_based_tbl_ptr == NULL)
    {
      DS_3GPP_MSG0_LOW("Memory allocation for APN Based Tbl Entry failed");
      return NULL;
    }
    if (ds_eps_pdn_throt_set_apn_based_tbl(subs_id, apn_based_tbl_ptr))
    {
      DS_3GPP_MSG0_LOW("Allocated new APN Based Tbl entry");
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Failure in setting APN Based TBl Ptr");
      modem_mem_free((void*)apn_based_tbl_ptr, MODEM_MEM_CLIENT_DATA);
      apn_based_tbl_ptr = NULL;
      return NULL;
    }
  }

  DS_3GPP_MSG2_MED("APN Based TBL Ptr: 0x%x for Subs Id: %d", 
                    apn_based_tbl_ptr, subs_id); 
  return apn_based_tbl_ptr;
}/* ds_eps_pdn_throt_allocate_apn_based_tbl*/
/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_GET_APN_BASED_TBL
 
DESCRIPTION
  This function fetches the APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the APN based Tbl Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_eps_pdn_throttle_sm_apn_based_s* 
                              ds_eps_pdn_throt_get_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_eps_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return apn_based_tbl_ptr;
  }

  apn_based_tbl_ptr = ds_eps_pdn_throttle_sm_apn_based_tbl_ptr[subs_id];

  DS_3GPP_MSG2_MED("APN Based TBL Ptr: 0x%x for Subs Id: %d", 
                   apn_based_tbl_ptr, subs_id);

  return apn_based_tbl_ptr;
}/* ds_eps_pdn_throt_get_apn_based_tbl*/

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_SET_APN_BASED_TBL
 
DESCRIPTION
  This function sets the APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  2. apn_based_tbl_ptr
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE- on success
  FLASE - otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_eps_pdn_throt_set_apn_based_tbl
(
  sys_modem_as_id_e_type              subs_id,
  ds_eps_pdn_throttle_sm_apn_based_s *apn_based_tbl_ptr
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  /* Do not set it to NULL if it already allocated */
 if(ds_eps_pdn_throttle_sm_apn_based_tbl_ptr[subs_id] == NULL)
 {
   ds_eps_pdn_throttle_sm_apn_based_tbl_ptr[subs_id] = apn_based_tbl_ptr;
   return TRUE;
 }

  return FALSE;
}/* ds_eps_pdn_throt_set_apn_based_tbl*/

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_GET_APN_INDEX_PTR

DESCRIPTION
  This function is used to get/create APN based throttle sm table for 
  given APN name.

PARAMETERS   : 
  1. apn - APN name
  2. allow_create - If APN entry is not present create if this parameter 
                    is TRUE
  3. Subs Id

DEPENDENCIES
  None.

RETURN VALUE
  pointer to APN based throttling table

SIDE EFFECTS
  None.
===========================================================================*/
static ds_eps_pdn_throttle_sm_apn_based_dyn_s* 
            ds_eps_pdn_throt_get_apn_index_ptr
(
  byte                         *apn,
  boolean                       allow_create, 
  sys_modem_as_id_e_type	subs_id  
)
{
  ds_eps_pdn_throttle_sm_apn_based_s *apn_based_tbl_ptr;
  ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr;
  uint16 apn_loop_idx = 0;
  uint16 first_free_apn_index = DS_MAX_APN_NUM;
/*-----------------------------------------------------------------------*/

  apn_based_tbl_ptr = ds_eps_pdn_throt_get_apn_based_tbl(subs_id);

  if(apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG1_ERROR("apn_based_tbl_ptr for subs_id:%d is NULL", subs_id);
	return NULL;
  }
  
  for(apn_loop_idx = 0; apn_loop_idx < DS_MAX_APN_NUM; apn_loop_idx++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_idx];
	if(apn_index_ptr != NULL)
    {
      if((apn_index_ptr->valid == TRUE)&&
         (strcasecmp((char*)apn_index_ptr->apn, (char*)apn) == 0))
      {
        DS_3GPP_MSG1_HIGH("APN matches at index: %d",apn_loop_idx);
        break;
      }
      else if((apn_index_ptr->valid == FALSE) && (first_free_apn_index == DS_MAX_APN_NUM))
      {
        first_free_apn_index = apn_loop_idx;
      }
   }
   else
   {
     DS_3GPP_MSG1_LOW("apn_index_ptr is NULL at index:%d", apn_loop_idx);
   }
  }/*for ...*/
  
  if(apn_loop_idx == DS_MAX_APN_NUM)
  {
    if( (first_free_apn_index == DS_MAX_APN_NUM) ||   
         (allow_create == FALSE) )
    {
      DS_3GPP_MSG2_HIGH("APN no match. First free index: %d, Allow create: %d",
                       first_free_apn_index, allow_create ); 
      return NULL;
    }
    else if((allow_create == TRUE) &&
 	   (first_free_apn_index >= ((0))) &&
	   (first_free_apn_index < DS_MAX_APN_NUM))
    {
      apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[first_free_apn_index];
	  apn_index_ptr->valid = TRUE;
      strlcpy( (char*)apn_index_ptr->apn, 
               (char*)apn, 
               (DS_UMTS_MAX_APN_STRING_LEN+1) );
    }    
  }
  else 
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_idx];
  }
  return apn_index_ptr;
}/* ds_eps_pdn_throt_get_apn_index_ptr*/

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_GET_THROTTLE_SM_PLMN_BASED

DESCRIPTION
  This function is used to get/create PLMN based throttle sm table for given APN index pointer
  which is unique for given APN

PARAMETERS   : 
  1. apn_index_ptr - poiner to APN based throttle table
  2. allow_create  -true/false
  3. current_plmn - PLMN for which throttle SM needs to be created
  4. Subs Id

DEPENDENCIES
  None.

RETURN VALUE
  pointer to PLMN based throttling table

SIDE EFFECTS
  None.
===========================================================================*/
static ds_eps_pdn_throttle_sm_plmn_based_s* 
			  ds_eps_pdn_throt_get_throttle_sm_plmn_based
(
  ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr,
  boolean                                 allow_create, 
  sys_plmn_id_s_type                      current_plmn,
  sys_modem_as_id_e_type	              subs_id
)
{
  ds_eps_pdn_throttle_sm_plmn_based_s *plmn_based_ptr = NULL;
  uint16                               plmn_loop_idx = 0;
  uint16                               first_free_plmn_index = 
                                                  DS_3GPP_MAX_PLMN_SUPPORTED;
  /*----------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("ds_eps_pdn_throt_get_throttle_sm_plmn_based: entered");
  if(apn_index_ptr == NULL)
  {
   DS_3GPP_MSG0_ERROR("Failed create APN throttle sm pointer");
   return NULL;
  }

  for(plmn_loop_idx = 0; plmn_loop_idx< DS_3GPP_MAX_PLMN_SUPPORTED; plmn_loop_idx++)
  {
    if((apn_index_ptr->apn_throt_sm_plmn_ptr[plmn_loop_idx] == NULL) && 
		(first_free_plmn_index == DS_3GPP_MAX_PLMN_SUPPORTED  ))
    {
    	first_free_plmn_index = plmn_loop_idx;
    }
    else if((apn_index_ptr->apn_throt_sm_plmn_ptr[plmn_loop_idx] != NULL)&&
             (memcmp((void*)&(current_plmn),
             (void*)&(apn_index_ptr->apn_throt_sm_plmn_ptr[plmn_loop_idx]->plmn_id), 
              sizeof(sys_plmn_id_s_type)) == 0))
    {
      DS_3GPP_MSG1_HIGH("PLMN found at index:%d", plmn_loop_idx);
      break;
    }
  }
  if(plmn_loop_idx >= ((0)) && plmn_loop_idx < DS_3GPP_MAX_PLMN_SUPPORTED)
  {
    return apn_index_ptr->apn_throt_sm_plmn_ptr[plmn_loop_idx];
  }
  else if(first_free_plmn_index >=((0)) && first_free_plmn_index< DS_3GPP_MAX_PLMN_SUPPORTED&& allow_create)
  {
    apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index] = 
	 	(ds_eps_pdn_throttle_sm_plmn_based_s*)modem_mem_alloc(
                                              sizeof(ds_eps_pdn_throttle_sm_plmn_based_s),
                                                         MODEM_MEM_CLIENT_DATA);
    if(apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index] == NULL)
    {
      DS_3GPP_MSG2_LOW("Heap Mem Allocation failed for client: %d size: %d",
                  MODEM_MEM_CLIENT_DATA,(sizeof(ds_eps_pdn_throttle_sm_plmn_based_s)));
      return NULL;
    }

    apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index]->failure_count = 0;
    apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index]->is_throttled = FALSE;
    apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index]->subs_id = subs_id;
    apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index]->throt_timer_val = 0;	
    plmn_based_ptr = apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index];
    rex_def_timer_ex(&(plmn_based_ptr->throttle_timer),
                  (rex_timer_cb_type) ds_eps_pdn_throt_sm_throt_timer_cb,
                   (unsigned long)(plmn_based_ptr));
	 
    apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index]->apn_idx_ptr = (void*)apn_index_ptr;
    memscpy((void*)&(apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index]->plmn_id),
	    sizeof(sys_plmn_id_s_type),
            (void*)&(current_plmn), 
            sizeof(sys_plmn_id_s_type));
    DS_3GPP_MSG0_HIGH("Allocated PLMN based throttle table");	
    return apn_index_ptr->apn_throt_sm_plmn_ptr[first_free_plmn_index];
  }
  else if(first_free_plmn_index == DS_3GPP_MAX_PLMN_SUPPORTED || allow_create == FALSE)
  {
    DS_3GPP_MSG0_ERROR("entry not found or All 8 PLMNs are already used for this APN");
  	 return NULL;
  }
  return NULL ;  	
}/* ds_eps_pdn_throt_get_throttle_sm_plmn_based */

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_IS_PLMN_BLOCKED

DESCRIPTION
  This function unblocks all the blocked PLMNs.

PARAMETERS
 1. subs_id - SUBS_ID 
 2. plmn_id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_eps_pdn_throt_is_plmn_blocked
(
  sys_modem_as_id_e_type  subs_id,
  sys_plmn_id_s_type	  plmn_id
)
{
  ds_eps_pdn_throt_blocked_plmn_type   *curr_blocked_plmn_ptr;
  ds_eps_pdn_throt_blocked_plmn_type   *next_blocked_plmn_ptr;
  list_type                             *list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return FALSE;
   }   
  list_ptr = &ds_eps_pdn_throt_blocked_plmn_list[subs_id];
  
  curr_blocked_plmn_ptr = 
      (ds_eps_pdn_throt_blocked_plmn_type*)list_peek_front( list_ptr );
    
  while ( curr_blocked_plmn_ptr != NULL )
  {
    next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
  
    if(memcmp((void*)&(curr_blocked_plmn_ptr->plmn_id), (void*)&(plmn_id), 
               sizeof(sys_plmn_id_s_type)) == 0)
    {
     return TRUE;
    } 
    curr_blocked_plmn_ptr = next_blocked_plmn_ptr;  
  }

  return FALSE;
} /* ds_3gpp_pdn_cntxt_enable_lte_unblock_on_plmn */

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_HANDLE_PDN_CONN_REJ
 
DESCRIPTION
  This function gets called when attach is rejected and given 
  cause code is present in EFS configuration file
  
PARAMETERS   : 
  1. rejected_pdn_cntx_p 
  2. esm_cause - PDN reject cause
  3. Subs Id
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_handle_pdn_conn_rej
( 
  ds_pdn_context_s *rejected_pdn_cntx_p, 
  esm_cause_enum_T          esm_cause,
  sys_modem_as_id_e_type  subs_id
)
{
  sys_plmn_id_s_type                        current_plmn;
  ds_eps_pdn_throttle_sm_plmn_based_s      *pdn_throt_ptr; 
  sys_plmn_id_s_type                        zero_plmn_id;
  ds_eps_pdn_throttle_sm_apn_based_dyn_s   *apn_index_ptr;
  list_type*                                list_ptr = NULL;  
  ds_eps_pdn_throt_blocked_plmn_type       *new_blocked_plmn_ptr = NULL;
  cm_plmn_blocking_info_s_type       plmn_blocking_info;
/*-------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }

  if(!ds_3gpp_cc_sp_throt_is_cause_code_configured(esm_cause, subs_id))
  {
    DS_3GPP_MSG1_ERROR("No throttling for cause code:", esm_cause);
    return ;
  }

  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  if(memcmp((void*)&(current_plmn),
		  (void*)&(zero_plmn_id), 
		  sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG0_LOW("PLMN being inspected is zero");
    return ;
  }
  
  apn_index_ptr= ds_eps_pdn_throt_get_apn_index_ptr(
	              rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
	              TRUE, subs_id);
  pdn_throt_ptr = ds_eps_pdn_throt_get_throttle_sm_plmn_based(
                    apn_index_ptr, 
                    TRUE, 
                    current_plmn, 
                    subs_id);
  
  if(pdn_throt_ptr == NULL)
  {
    DS_3GPP_MSG0_LOW("failed created PDN throttle SM, returning "); 
    return;
  }

  DS_3GPP_MSG2_HIGH("is_throttle: %d, fail_count:%d", 				 
  	    pdn_throt_ptr->is_throttled, pdn_throt_ptr->failure_count );
	
  pdn_throt_ptr->failure_count++;
  pdn_throt_ptr->subs_id = subs_id;
 
  memscpy((void*)&(pdn_throt_ptr->plmn_id),
	    sizeof(sys_plmn_id_s_type),
           (void*)&(current_plmn), 
            sizeof(sys_plmn_id_s_type));
		
  if(ds_eps_attach_sm_is_ue_attached(subs_id)&&
  	(pdn_throt_ptr->failure_count < DS_EPS_PDN_THROT_MAX_FAIL_COUNT))
  {
     DS_3GPP_MSG1_HIGH("starting throttling timer for %dsecs", DS_EPS_PDN_THROT_TIMER);
     pdn_throt_ptr->is_throttled = TRUE;

     pdn_throt_ptr->throt_timer_val = DS_EPS_PDN_THROT_TIMER*1000 ;
   
     (void)rex_set_timer(&(pdn_throt_ptr->throttle_timer), 
  					pdn_throt_ptr->throt_timer_val);
  }
  
  if(pdn_throt_ptr->failure_count >= DS_EPS_PDN_THROT_MAX_FAIL_COUNT)  
  {     
    pdn_throt_ptr->is_throttled = TRUE;   
    DS_3GPP_MSG1_HIGH("Blocking PLMN for %d sec ", DS_3GPP_PDN_THROT_INFINITE_THROT);     
    ds3g_print_plmn_id(&current_plmn);    

    if(ds_eps_pdn_throt_is_plmn_blocked(subs_id, current_plmn) == FALSE)
    {
      list_ptr = &ds_eps_pdn_throt_blocked_plmn_list[subs_id];
      new_blocked_plmn_ptr  = 
       (ds_eps_pdn_throt_blocked_plmn_type *)modem_mem_alloc
         (sizeof(ds_eps_pdn_throt_blocked_plmn_type),MODEM_MEM_CLIENT_DATA);
      if(new_blocked_plmn_ptr == NULL)
      {
          DS_3GPP_MSG0_LOW("Could not allocate memory");
          return;
      }
      new_blocked_plmn_ptr->plmn_id = current_plmn;
       rex_def_timer_ex(&(new_blocked_plmn_ptr->throttle_timer),
			   (rex_timer_cb_type) ds_eps_pdn_throt_t3402_timer_exp_cb,
			   (unsigned long)(new_blocked_plmn_ptr));
      /*----------------------------------------------------------------------
        Push the list item into the list
      ----------------------------------------------------------------------*/
      DS_3GPP_MSG0_LOW("debug: adding current plmn to the list");
      list_push_front(list_ptr, &(new_blocked_plmn_ptr->link));
    }

	plmn_blocking_info.asubs_id = subs_id;
	plmn_blocking_info.blocking_interval = DS_3GPP_PDN_THROT_INFINITE_THROT;
	plmn_blocking_info.plmn = current_plmn;
	plmn_blocking_info.request_pref = CM_PLMN_BLOCKING_PREF_PLMN_BLOCKING_ONLY;
	plmn_blocking_info.cause = SYS_BLOCK_PLMN_CAUSE_PRIORITY;
    if(ds3g_msh_disable_lte_per_plmn_ex(plmn_blocking_info) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Failed to block PLMN");
    }

    if(ds_3gpp_pdn_throt_is_any_apn_throttled_for_max_failure(subs_id))
    {
      DS_3GPP_MSG1_HIGH("max rejects already happened on 3G, start timer for:%d"
                        " msecs to unblock PLMN", DS_EPS_PDN_THROT_2_HOURS_TIMER);
      ds_eps_pdn_throt_start_lte_unblock_plmn_timer(DS_EPS_PDN_THROT_2_HOURS_TIMER, subs_id);
    }	
    else 
    {		
      DS_3GPP_MSG0_HIGH("start t3402 timer, LTE will be enabled on timer expiry, If UE stays on LTE");
      ds_eps_pdn_throt_t3402_timer_start(new_blocked_plmn_ptr, subs_id);
    }
  }  	

  DS_3GPP_MSG2_HIGH("is_throttle: %d, fail_count:%d", 				 
  	  pdn_throt_ptr->is_throttled,pdn_throt_ptr->failure_count );
  
}/* ds_eps_pdn_throt_handle_pdn_conn_rej */

/*===========================================================================
FUNCTION DS_EPS_PDN_CLEAR_THROTTLE_STATE_FOR_APN_ON_CURRENT_PLMN
 
DESCRIPTION
  This function is used to clear the throttling of given APN  
  
PARAMETERS   : 
  1. apn        - for which throttling needs to be cleared 
  2. Subs Id  
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_clear_throttle_state_for_apn_on_current_plmn
(
  byte                   *apn, 
  sys_modem_as_id_e_type  subs_id
)
{
  sys_plmn_id_s_type 		         current_plmn;
  ds_eps_pdn_throttle_sm_plmn_based_s    *pdn_throt_ptr; 
  sys_plmn_id_s_type		          zero_plmn_id;
  ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr;  
  uint16                                  loop_idx = 0;
/*-------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  if(ds_3gpp_pdn_throt_is_apn_globally_throttled(apn, subs_id) == TRUE)
  {
    DS_3GPP_MSG1_ERROR("APN globally blocked on Subs Id: %d,"
		              " not clearing throttling ", subs_id);
    return;
  }

  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn,subs_id);

  if(memcmp((void*)&(current_plmn),
		  (void*)&(zero_plmn_id), 
		  sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG0_LOW("PLMN being inspected is zero");
    return ;
  }
  
   apn_index_ptr= ds_eps_pdn_throt_get_apn_index_ptr(
	              apn, 
	              TRUE, subs_id);
  if(apn_index_ptr == NULL)
  {
    DS_3GPP_MSG1_LOW("apn_index_ptr for subs_id: %d is NULL", subs_id);
    return;
  }
  for(loop_idx=0; loop_idx < DS_3GPP_MAX_PLMN_SUPPORTED; loop_idx++ )
  {
    if((apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx] != NULL)&&
		(memcmp((void*)&(current_plmn),
  	      (void*)&(apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx]->plmn_id), 
  	       sizeof(sys_plmn_id_s_type)) == 0))
    {
      if(ds_3gpp_pdn_throt_is_apn_globally_throttled((apn_index_ptr->apn),
 	 	   apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx]->subs_id) == TRUE)
      {
        DS_3GPP_MSG1_ERROR("APN globally blocked on Subs Id: %d,"
	 	              " not clearing throttling ", 
        apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx]->subs_id);
      }
      else
      {
  	pdn_throt_ptr = apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx];
        pdn_throt_ptr->failure_count = 0;
        pdn_throt_ptr->is_throttled = 0;
	  
        if(rex_get_timer(&(pdn_throt_ptr->throttle_timer) ) > 0)
        {
          rex_clr_timer(&(pdn_throt_ptr->throttle_timer));
        }
	pdn_throt_ptr->throt_timer_val = 0;     
        break;
      }
    }
  }
    
}/*ds_eps_pdn_clear_throttle_state_for_apn_on_current_plmn*/

/*===========================================================================
FUNCTION DS_EPS_PDN_CLEAR_THROTTLE_STATE_FOR_APN_ON_ALL_PLMNS
 
DESCRIPTION
  This function is used to clear the throttling of given APN on all PLMNs
  
PARAMETERS   : 
  1.  apn_index_ptr - pointer to APN throttle table for given APN  
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_clear_throttle_state_for_apn_on_all_plmns
(
  ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr
)
{
  uint16 loop_idx = 0;
  /*------------------------------------------------------------------*/
  if(apn_index_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_index_ptr in NULL "); 
    return;
  }
   
  for(loop_idx=0;loop_idx < DS_3GPP_MAX_PLMN_SUPPORTED; loop_idx++ )
  {
    if(apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx] != NULL)
    {
	  	 	
      modem_mem_free(apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx], 
    			   MODEM_MEM_CLIENT_DATA);
      apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx] = NULL;
     }
  }
  
  memset((void*)&(apn_index_ptr->apn), 0, (DS_UMTS_MAX_APN_STRING_LEN+1));
  apn_index_ptr->valid = FALSE;

}/*ds_eps_pdn_clear_throttle_state_for_apn_on_all_plmns*/

/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_RESET_THROTTLING

DESCRIPTION
  This function is used to clear throttling of all APN for given subscription
  
PARAMETERS  
  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
void ds_eps_pdn_throt_reset_throttling
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_eps_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  uint16                                  apn_loop_idx =0;
/*--------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  apn_based_tbl_ptr = ds_eps_pdn_throt_get_apn_based_tbl(subs_id);
  
  if(apn_based_tbl_ptr == NULL)
  {
   DS_3GPP_MSG1_ERROR("apn_based_tbl_ptr for subs_id:%d is NULL", subs_id);
   return;
  }
  
  for(apn_loop_idx = 0; apn_loop_idx < DS_MAX_APN_NUM; apn_loop_idx++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_idx];
    if(apn_index_ptr != NULL)
    {       
       ds_eps_pdn_clear_throttle_state_for_apn_on_all_plmns(apn_index_ptr);
    } 
  }
}/*ds_eps_pdn_throt_reset_throttling*/

/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_RESET_THROTTLING_ON_PMN

DESCRIPTION
  This function is used to clear throttling of all APN for given subscription
  
PARAMETERS  
  1. subs_id
  2. plmn_id  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
void ds_eps_pdn_throt_reset_throttling_on_plmn
(
  sys_modem_as_id_e_type  subs_id,  
  sys_plmn_id_s_type	 plmn_id 
)
{
  ds_eps_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  uint16                                  apn_loop_idx =0;
  uint16                                  loop_idx = 0;
  uint16                                  null_entry = 0;
/*--------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  apn_based_tbl_ptr = ds_eps_pdn_throt_get_apn_based_tbl(subs_id);
  
  if(apn_based_tbl_ptr == NULL)
  {
   DS_3GPP_MSG1_ERROR("apn_based_tbl_ptr for subs_id:%d is NULL", subs_id);
   return;
  }
  
  for(apn_loop_idx = 0; apn_loop_idx < DS_MAX_APN_NUM; apn_loop_idx++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_idx];
    if((apn_index_ptr != NULL)&&
		(ds_3gpp_pdn_throt_is_apn_globally_throttled((apn_index_ptr->apn),
 	 	subs_id) == FALSE))
    {       
      for(loop_idx=0;loop_idx < DS_3GPP_MAX_PLMN_SUPPORTED; loop_idx++ )
      {
        if((apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx] != NULL)&&
           (memcmp((void*)&(plmn_id),
             (void*)&(apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx]->plmn_id), 
  	       sizeof(sys_plmn_id_s_type)) == 0))
        {
				  
          modem_mem_free(apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx], 
                         MODEM_MEM_CLIENT_DATA);
          apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx] = NULL;
          null_entry++;
    } 
        else if(apn_index_ptr->apn_throt_sm_plmn_ptr[loop_idx] == NULL)
        {
          null_entry++;
  }
     }
     if(null_entry == DS_3GPP_MAX_PLMN_SUPPORTED) 
     {
       memset((void*)&(apn_index_ptr->apn), 0, (DS_UMTS_MAX_APN_STRING_LEN+1));
       apn_index_ptr->valid = FALSE;
     }
   } 
  }
}/*ds_eps_pdn_throt_reset_throttling_on_plmn*/

/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_CLEAR_PLMN_BLOCKING

DESCRIPTION
  This function is used to clear LTE PLMN blocking on the given subscription
  
PARAMETERS  
  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  If anything more than such light operation (changing the state of the SM)
  were needed, post a command and process in DS context instead of working in
  the timer context.
  
===========================================================================*/
void ds_eps_pdn_throt_sm_clear_plmn_blocking
(
  sys_modem_as_id_e_type	subs_id
)
{
  ds_eps_pdn_throttle_sm_timer_s *plmn_unblock_timer_p = NULL;
/*------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  plmn_unblock_timer_p = &plmn_unblock_timer[subs_id];
  plmn_unblock_timer_p->subs_id = subs_id;
  ds_eps_pdn_throt_plmn_unblock_timer_exp_hdlr((unsigned long)plmn_unblock_timer_p);
}/*ds_eps_pdn_throt_sm_clear_plmn_blocking*/

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_SM_PLMN_UNBLOCK_TIMER_CB

DESCRIPTION
  This function is the timer call back for the unblock PLMN timer.
  This changes the state of the SM from throttled to unthrottled. Since this
  runs in the timer context
  
PARAMETERS  
  callback_data - points to the  throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  If anything more than such light operation (changing the state of the SM)
  were needed, post a command and process in DS context instead of working in
  the timer context.
  
===========================================================================*/
void ds_eps_pdn_throt_sm_plmn_unblock_timer_cb
(
  unsigned long callback_data
)
{
  ds_cmd_type    *cmd_ptr     = NULL;
  unsigned long  *data_ptr    = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(unsigned long))) == NULL )
  {
    DATA_ERR_FATAL(" can't get cmd buf from DS task");
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PLMN_BLOCK_TMR_EXP_CMD;
	
	data_ptr = (unsigned long*)cmd_ptr->cmd_payload_ptr;
    *data_ptr= callback_data;
    ds_put_cmd_ext(cmd_ptr);

  }
}/*ds_eps_pdn_throt_sm_plmn_unblock_timer_cb*/
/*============================================================================
FUNCTION DS_EPS_PDN_THROT_SM_TIMER_EXP_HDLR

DESCRIPTION
  This function gets called on expiry of unblock_plmn timer, throttling 
  will be cleared and PLMN will be unblocked in this function
  
PARAMETERS  
  callback_data - points to the  throttling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None
  
===========================================================================*/
void ds_eps_pdn_throt_plmn_unblock_timer_exp_hdlr
(
  unsigned long callback_data
)
{
  uint32                        remaining_timer_val = 0;
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE ;
  ds_eps_pdn_throttle_sm_timer_s *plmn_unblock_timer_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_LOW("ds_eps_pdn_throt_plmn_unblock_timer_exp_hdlr");
  plmn_unblock_timer_p = (ds_eps_pdn_throttle_sm_timer_s*)callback_data;
  if(plmn_unblock_timer_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_eps_pdn_throt_plmn_unblock_timer_exp_hdlr: null pointer passed");
    return;
  }
  subs_id = plmn_unblock_timer_p->subs_id;
  
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  plmn_unblock_timer_p->is_throttled = FALSE;
  plmn_unblock_timer_p->throt_timer_val = 0;
  remaining_timer_val = rex_get_timer(&(plmn_unblock_timer_p->throttle_timer));
  DS_3GPP_MSG1_HIGH("plmn_unblock_timer_exp_hdlr:Enabling LTE PLMN and clearing APN" 
                    "throttling for non-LTE RATs for subs_id:%d", subs_id);
  if(rex_get_timer(&(plmn_unblock_timer_p->throttle_timer) ) > 0)
  { 
    DS_3GPP_MSG0_ERROR("debug:plmn_unblock_timer is non-zero ");
    rex_clr_timer(&(plmn_unblock_timer_p->throttle_timer));
  }
  
  ds_eps_pdn_throt_reset_throttling(subs_id);
  ds_eps_pdn_throt_enable_lte_unblock_all_plmns(subs_id);
  ds_3gpp_pdn_throt_reset_pdn_throttle(subs_id);
}/*ds_eps_pdn_throt_plmn_unblock_timer_exp_hdlr*/
/*============================================================================
FUNCTION DS_EPS_PDN_THROT_SM_STOP_PLMN_UNBLOCK_TMR

DESCRIPTION
  This function stops plmn unblock timer if its running.
  
PARAMETERS  
  subs_id - subscription id.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None
  
===========================================================================*/
void ds_eps_pdn_throt_sm_stop_plmn_unblock_tmr
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_eps_pdn_throttle_sm_timer_s *plmn_unblock_timer_p = NULL;
  /*------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Subs Id: %d is invalid", subs_id);
    return ;
  }
  
  plmn_unblock_timer_p = &plmn_unblock_timer[subs_id];
  plmn_unblock_timer_p->subs_id = subs_id;

  if(rex_get_timer(&(plmn_unblock_timer_p->throttle_timer) ) > 0)
  { 
    DS_3GPP_MSG0_MED("Stopping PLMN unblock timer");
    rex_clr_timer(&(plmn_unblock_timer_p->throttle_timer));
    ds_3gpp_pdn_throt_reset_pdn_throttle(subs_id);
  }
}/* ds_eps_pdn_throt_sm_stop_plmn_unblock_tmr */
/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_IS_PLMN_UNBLOCK_TIMER_RUNNING

DESCRIPTION
  This function checks is PLMN unblock timer is running or not

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE - if PLMN unblock timer is running
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_pdn_throt_is_plmn_unblock_timer_running
(
  sys_modem_as_id_e_type		subs_id  
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  if(rex_get_timer(&( plmn_unblock_timer[subs_id].throttle_timer)) > 0)
  {
    DS_3GPP_MSG0_MED("timer is running ");
    return TRUE;
  }
  return FALSE;
} /*ds_eps_pdn_throt_is_plmn_unblock_timer_running */

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_SM_THROT_TIMER_CB

DESCRIPTION
  This function is the timer call back for the LTE acqiosition timer.
  This is used to reset  UMTS throttling if running, this timer is to make sure 
  UE tries to attach on LTE before UMTS on RAU complete IND
PARAMETERS  
  callback_data - points to the  throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
void ds_eps_pdn_throt_sm_throt_timer_cb
(
  unsigned long callback_data
)
{
  ds_cmd_type    *cmd_ptr 	= NULL;
  unsigned long  *data_ptr  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(unsigned long))) == NULL )
  {
    DATA_ERR_FATAL(" can't get cmd buf from DS task");
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EPS_THROT_TMR_EXP_CMD;
	data_ptr = (unsigned long*)cmd_ptr->cmd_payload_ptr;
    *data_ptr= callback_data;
    ds_put_cmd_ext(cmd_ptr);  
  }
}/* ds_eps_pdn_throt_sm_throt_timer_cb */
/*============================================================================
FUNCTION DS_EPS_PDN_THROT_SM_TIMER_EXP_HDLR

DESCRIPTION
  This function gets called on expiry of unblock_plmn timer, throttling 
  will be cleared and PLMN will be unblocked in this function
  
PARAMETERS  
  callback_data - points to the  throttling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None
  
===========================================================================*/
void ds_eps_pdn_throt_sm_timer_exp_hdlr
(
  unsigned long callback_data
)
{
  ds_eps_pdn_throttle_sm_plmn_based_s*    plmn_based_throt_p = NULL;
  ds_eps_pdn_throttle_sm_apn_based_dyn_s* apn_idx_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_3GPP_MSG0_HIGH( "ds_eps_pdn_throt_sm_throt_timer_cb ");
  plmn_based_throt_p = (ds_eps_pdn_throttle_sm_plmn_based_s*)callback_data;
  if(plmn_based_throt_p == NULL)
  {
    DS_LTE_MSG0_ERROR( "Call back data is NULL");
    return;
  }
  apn_idx_ptr = plmn_based_throt_p->apn_idx_ptr ;
  
  if(apn_idx_ptr != NULL && apn_idx_ptr->valid) 
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
        "Clearing the is_throt_flag in core sm for apn: %s for below PLMN",
  		apn_idx_ptr->apn);
  } 
  ds3g_print_plmn_id(&(plmn_based_throt_p->plmn_id)); 
  
  plmn_based_throt_p->is_throttled = FALSE;
  plmn_based_throt_p->throt_timer_val = 0;

}/*ds_eps_pdn_throt_sm_timer_exp_hdlr*/

/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_START_LTE_UNBLOCK_PLMN_TIMER

DESCRIPTION
  This function start a timer for given timer value for given subscription
  on expiry of this timer PLMN will be unblocked
  
PARAMETERS  
  1.  timer_val -timer value
  2.  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
void ds_eps_pdn_throt_start_lte_unblock_plmn_timer
(
  uint32 timer_val, 
  sys_modem_as_id_e_type	subs_id
)
{
  uint32 conf_tmr_val = 0;
/*-------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  conf_tmr_val = ds_3gpp_cc_sp_throt_get_plmn_unblock_timer_val();

  /*If timer is configured in EFS use that value else use passed value*/
  if(conf_tmr_val == 0)
  {
     conf_tmr_val = timer_val; 
  }

  if(rex_get_timer(&(  plmn_unblock_timer[subs_id].throttle_timer))>0)
  {
    DS_3GPP_MSG0_ERROR("timer already running not starting again");
    return;
  }
  plmn_unblock_timer[subs_id].is_throttled = TRUE;
  plmn_unblock_timer[subs_id].throt_timer_val = conf_tmr_val;
  plmn_unblock_timer[subs_id].subs_id = subs_id;
  (void)rex_set_timer(&(plmn_unblock_timer[subs_id].throttle_timer), 
  					plmn_unblock_timer[subs_id].throt_timer_val);
  
  DS_3GPP_MSG1_HIGH("Started throttling timer for %d ms",
  				   plmn_unblock_timer[subs_id].throt_timer_val);

}/*ds_eps_pdn_throt_start_lte_unblock_plmn_timer*/
/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_RAU_COMPLETE_IND

DESCRIPTION
  This function will be caused when RAU happens due to routing area change,
  to clear the throttling
  
PARAMETERS  

  1.  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
void ds_eps_pdn_throt_rau_complete_ind
(
  sys_modem_as_id_e_type	subs_id 
)
{
  ds_eps_pdn_throt_reset_throttling(subs_id);
  ds_eps_pdn_throt_sm_clear_plmn_blocking(subs_id);
}/*ds_eps_pdn_throt_rau_complete_ind*/
/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_IS_APN_THROTTLE_ON_CURR_PLMN

DESCRIPTION
  This function check if the given apn is throttled on current PLMN
  
PARAMETERS  
  1.  apn -apn name
  2.  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
boolean ds_eps_pdn_throt_is_apn_throttled_on_curr_plmn
(
  byte                          *apn, 
  sys_modem_as_id_e_type	subs_id
)
{
  sys_plmn_id_s_type                     current_plmn;
  ds_eps_pdn_throttle_sm_plmn_based_s	*pdn_throt_ptr; 
  sys_plmn_id_s_type                     zero_plmn_id;
  ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr;  
  /*--------------------------------------------------------------------*/

  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);
  
  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  
  if(memcmp((void*)&(current_plmn),
  	 (void*)&(zero_plmn_id), 
  	 sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG0_LOW("PLMN being inspected is zero");
    return FALSE;
  }
  
  apn_index_ptr= ds_eps_pdn_throt_get_apn_index_ptr(
  			 apn,FALSE, subs_id);
  pdn_throt_ptr = ds_eps_pdn_throt_get_throttle_sm_plmn_based(
  			   apn_index_ptr, 
  			   FALSE, 
  			   current_plmn, 
  			   subs_id);
  if(pdn_throt_ptr!=NULL && pdn_throt_ptr->is_throttled)
  {
    return TRUE;
  }
  return FALSE;
} /*ds_eps_pdn_throt_is_apn_throttled_on_curr_plmn*/
/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_LTE_UNBLOCK_ALL_PLMNS

DESCRIPTION
  This function unblocks all the blocked PLMNs.

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_eps_pdn_throt_enable_lte_unblock_all_plmns
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_eps_pdn_throt_blocked_plmn_type   *curr_blocked_plmn_ptr;
  ds_eps_pdn_throt_blocked_plmn_type   *next_blocked_plmn_ptr;
  list_type                             *list_ptr = NULL;
  sys_plmn_id_s_type                    zero_plmn_id;
  sys_plmn_id_s_type                    current_plmn;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return;
   }
   memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));

   ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn,subs_id);
   
  /*--------------------------------------------------------------------------   
    Here we loop through all the blocked PLMNs and unblock them one by one.
  ----------------------------------------------------------------------------*/
  if((ds_eps_pdn_throt_is_any_lte_plmn_blocked(subs_id) == FALSE)&&
     (ds_3gpp_pdn_cntxt_is_plmn_blocked(current_plmn, subs_id) == FALSE))
  {
    DS_3GPP_MSG0_HIGH("No PLMN is blocked, forcing preference for current PLMN");
	ds3g_print_plmn_id(&current_plmn);
    ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(current_plmn, 
  	   (uint16)CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_FORCE_PREF, subs_id);
  }
  else
  {
    list_ptr = &ds_eps_pdn_throt_blocked_plmn_list[subs_id];
  
    curr_blocked_plmn_ptr = 
      (ds_eps_pdn_throt_blocked_plmn_type*)list_peek_front( list_ptr );
    
    while ( curr_blocked_plmn_ptr != NULL )
    {
      next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
  	
  	  if(memcmp((void*)&(curr_blocked_plmn_ptr->plmn_id),
  		  (void*)&(zero_plmn_id), 
  		  sizeof(sys_plmn_id_s_type)) == 0)
        {
          DS_3GPP_MSG0_LOW("PLMN being inspected is zero");
          continue ;
        }
    	  ds3g_print_plmn_id(&(curr_blocked_plmn_ptr->plmn_id));
  
      if(ds_3gpp_pdn_cntxt_is_plmn_blocked(
           (curr_blocked_plmn_ptr->plmn_id),subs_id) == FALSE)		
      {
        ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(curr_blocked_plmn_ptr->plmn_id, 
    	                    (uint16)CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_FORCE_PREF, subs_id);
        curr_blocked_plmn_ptr->throt_timer_val = 0;
        if(rex_get_timer(&(curr_blocked_plmn_ptr->throttle_timer) ) > 0)
        { 
          DS_3GPP_MSG0_LOW("debug:t3402 is non-zero ");
          rex_clr_timer(&(curr_blocked_plmn_ptr->throttle_timer));
        }
  	  
        list_pop_item(list_ptr, &(curr_blocked_plmn_ptr->link) );
        DS_3GPP_MSG0_HIGH("enabling plmn: "
                          "poping PLMN from the list");
        modem_mem_free(curr_blocked_plmn_ptr,MODEM_MEM_CLIENT_DATA);
      }
      else
      {
        DS_3GPP_MSG0_ERROR("plmn blocked due to t3396, not enabling it.");	
      }  
      curr_blocked_plmn_ptr = next_blocked_plmn_ptr;  
    }
  }
} /* ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns */
/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_LTE_UNBLOCK_ON_PLMN

DESCRIPTION
  This function unblocks all the blocked PLMNs.

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_eps_pdn_throt_enable_lte_unblock_on_plmn
(
  sys_modem_as_id_e_type  subs_id,
  sys_plmn_id_s_type	  plmn_id
)
{
  ds_eps_pdn_throt_blocked_plmn_type   *curr_blocked_plmn_ptr;
  ds_eps_pdn_throt_blocked_plmn_type   *next_blocked_plmn_ptr;
  list_type                             *list_ptr = NULL;
  sys_plmn_id_s_type                    zero_plmn_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return;
   }
   memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
    
  list_ptr = &ds_eps_pdn_throt_blocked_plmn_list[subs_id];
  
  curr_blocked_plmn_ptr = 
      (ds_eps_pdn_throt_blocked_plmn_type*)list_peek_front( list_ptr );
    
  while ( curr_blocked_plmn_ptr != NULL )
  {
    next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
  
    if((memcmp((void*)&(curr_blocked_plmn_ptr->plmn_id), (void*)&(plmn_id), 
               sizeof(sys_plmn_id_s_type)) == 0)&&
  	   (ds_3gpp_pdn_cntxt_is_plmn_blocked(
              (curr_blocked_plmn_ptr->plmn_id),subs_id) == FALSE))
    {
      DS_3GPP_MSG0_HIGH("enabling plmn, poping PLMN from the list");
	  
      ds3g_print_plmn_id(&(curr_blocked_plmn_ptr->plmn_id));
      ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(curr_blocked_plmn_ptr->plmn_id, 
    	                    (uint16)CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_FORCE_PREF, subs_id);
      curr_blocked_plmn_ptr->throt_timer_val = 0;
      if(rex_get_timer(&(curr_blocked_plmn_ptr->throttle_timer) ) > 0)
      { 
        rex_clr_timer(&(curr_blocked_plmn_ptr->throttle_timer));
      }	  
      list_pop_item(list_ptr, &(curr_blocked_plmn_ptr->link) );
      modem_mem_free(curr_blocked_plmn_ptr,MODEM_MEM_CLIENT_DATA);
		
      ds_eps_pdn_throt_reset_throttling_on_plmn(subs_id, plmn_id);
      break;
    }
    else
    {
      DS_3GPP_MSG0_ERROR("plmn blocked due to t3396, not enabling it.");	
    }  
    curr_blocked_plmn_ptr = next_blocked_plmn_ptr;  
  }
} /* ds_3gpp_pdn_cntxt_enable_lte_unblock_on_plmn */

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_IS_LTE_PLMN_BLOCKED

DESCRIPTION
  This function is used to check if given PLMN ID is blocked on the current subscription
PARAMETERS  
  PLMN id
  Susbcription id
  PLMN ID
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE, if PLMN is blocked
  FALSE, otherwise
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_eps_pdn_throt_is_any_lte_plmn_blocked
(
  sys_modem_as_id_e_type subs_id
)
{
  uint16 ret=0;
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  if((ret=list_size(&ds_eps_pdn_throt_blocked_plmn_list[subs_id]))>0)
  {
    DS_3GPP_MSG1_HIGH("%d: PLMNs blocked", ret);
    return TRUE;
  }
  return FALSE;	
} /* ds_3gpp_pdn_cntxt_is_any_lte_plmn_blocked */

/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_T3402_TIMER_START

DESCRIPTION
  This function starts the t3402 timer 

PARAMETERS
   blocked_plmn_ptr - pointer to blocked PLMN timer structure 
   subs_id            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_eps_pdn_throt_t3402_timer_start
(
  ds_eps_pdn_throt_blocked_plmn_type* blocked_plmn_ptr,
  sys_modem_as_id_e_type	  subs_id  
)
{
  uint32 ds_t3402_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((!ds3gsubsmgr_is_subs_id_valid(subs_id)) || (blocked_plmn_ptr == NULL))
  {
    return ;
  }
  if(rex_get_timer(&(  blocked_plmn_ptr->throttle_timer))>0)
  {
    DS_3GPP_MSG0_ERROR("timer already running not starting again");
    return;
  }

  if((ds_t3402_val = ds_eps_pdn_cntxt_get_t3402_timer_val()) > 0)
  {
    DS_3GPP_MSG1_HIGH("ds_t3402 timer val is %d",ds_t3402_val);
    blocked_plmn_ptr->throt_timer_val = ds_t3402_val;
    blocked_plmn_ptr->subs_id = subs_id;
    /*------------------------------------------------------------------------ 
      Start the T3402 timer 
    -------------------------------------------------------------------------*/
    (void)rex_set_timer(&(blocked_plmn_ptr->throttle_timer), 
                          ds_t3402_val);
  }
  return;
}

/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_T3402_TIMER_EXP_CB

DESCRIPTION
  This function posts a cmd to take specific action based on tr3402 timer expiry

PARAMETERS
   call back data ptr - which points to t3402 timer table            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_t3402_timer_exp_cb
(
     unsigned long callback_data 
)
{
  ds_cmd_type    *cmd_ptr     = NULL;
  unsigned long  *data_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(unsigned long))) == NULL )
  {
    DATA_ERR_FATAL(" can't get cmd buf from DS task");
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EPS_THROT_T3402_TMR_EXP_CMD;
	data_ptr = (unsigned long*)cmd_ptr->cmd_payload_ptr;
    *data_ptr= callback_data;
    ds_put_cmd_ext(cmd_ptr);
  }
  return;
}

/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_T3402_TIMER_EXPIRY_CMD_HDLR

DESCRIPTION
  This function resets the sm on timer expiry

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_t3402_timer_expiry_cmd_hdlr
(
  unsigned long 	  client_data_ptr
)
{
  ds_eps_pdn_throt_blocked_plmn_type *t3402_timer_p = NULL;
  sys_sys_mode_e_type             current_mode = SYS_SYS_MODE_NONE;
  sys_modem_as_id_e_type          subs_id;    
  sys_srv_domain_e_type           current_reg_srv_domain;  
  sys_srv_status_e_type           current_srv_status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("In ds_eps_pdn_throt_t3402_timer_expiry_cmd_hdlr()");

  t3402_timer_p = (ds_eps_pdn_throt_blocked_plmn_type*)client_data_ptr;

  if(t3402_timer_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("client_data_ptr is NULL, returning!!!");
	return;
  }
  subs_id = t3402_timer_p->subs_id;
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }

  t3402_timer_p->throt_timer_val = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  in the bellow scenario start PLMN unblock timer: 
   1. LTE only scenario 
   2. WCDMA PS attach failed
   3. WCDMA PS attached but No PDP brought up
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  current_reg_srv_domain = ds3g_get_current_regd_srv_domain_ex(subs_id);
  current_mode = ds3g_get_current_network_mode_ex(subs_id);
  current_srv_status= ds3g_get_current_srv_status_ex(subs_id);
		 
  if ( (UMTS_PS_NOT_REGISTERED(current_mode, 
  	             current_srv_status, current_reg_srv_domain) == TRUE) ||
	   (UMTS_PS_REGISTERED_PDP_CONTEXT_NOT_UP(current_mode, 
	                current_srv_status, current_reg_srv_domain) == TRUE)) 
  {
    ds_eps_pdn_throt_enable_lte_unblock_on_plmn(subs_id, t3402_timer_p->plmn_id);
	
  }

  return;
} /*ds_eps_pdn_throt_t3402_timer_expiry_cmd_hdlr*/

/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_CLEAR_T3402_TIMER

DESCRIPTION
  This function clears the t3402 timer if it is running

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_clear_t3402_timer
(
  sys_modem_as_id_e_type	  subs_id  
)
{
  ds_eps_pdn_throt_blocked_plmn_type   *curr_blocked_plmn_ptr;
  ds_eps_pdn_throt_blocked_plmn_type   *next_blocked_plmn_ptr;
  list_type                             *list_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  list_ptr = &ds_eps_pdn_throt_blocked_plmn_list[subs_id];

  curr_blocked_plmn_ptr = 
      (ds_eps_pdn_throt_blocked_plmn_type*)list_peek_front( list_ptr );
  
  /* Clear T3402 timer for all blocked PLMNs */
  while ( curr_blocked_plmn_ptr != NULL )
  {
    next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));

    if(rex_get_timer(&(curr_blocked_plmn_ptr->throttle_timer) ) > 0)
    { 
      DS_LTE_MSG0_HIGH("Clear T3402 timer for blocked PLMN:");
      ds3g_print_plmn_id(&(curr_blocked_plmn_ptr->plmn_id));
      rex_clr_timer(&(curr_blocked_plmn_ptr->throttle_timer));
    }
    curr_blocked_plmn_ptr = next_blocked_plmn_ptr;  
  }

}/*ds_eps_pdn_throt_clear_t3402_timer*/
/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_IS_T3402_TIMER_RUNNING

DESCRIPTION
  This function checks is  t3402 timer is running or not

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE - if t3402 timer is running
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_pdn_throt_is_t3402_timer_running
(
  sys_modem_as_id_e_type            subs_id,  
  sys_plmn_id_s_type		    plmn_id
)
{
   ds_eps_pdn_throt_blocked_plmn_type	*curr_blocked_plmn_ptr;
   ds_eps_pdn_throt_blocked_plmn_type	*next_blocked_plmn_ptr;
   list_type							 *list_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
 
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  list_ptr = &ds_eps_pdn_throt_blocked_plmn_list[subs_id];

  curr_blocked_plmn_ptr = 
	   (ds_eps_pdn_throt_blocked_plmn_type*)list_peek_front( list_ptr );

  while ( curr_blocked_plmn_ptr != NULL )
  {
    next_blocked_plmn_ptr = 
		 list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
   
    if(memcmp((void*)&(curr_blocked_plmn_ptr->plmn_id), (void*)&(plmn_id), 
    		sizeof(sys_plmn_id_s_type)) == 0)
    {
      return (rex_get_timer(&(curr_blocked_plmn_ptr->throttle_timer) ) > 0); 	 
    }
    curr_blocked_plmn_ptr = next_blocked_plmn_ptr;  
  }

  return FALSE;
}/*ds_eps_pdn_throt_is_t3402_timer_running*/

#endif /* FEATURE_DATA_LTE*/

