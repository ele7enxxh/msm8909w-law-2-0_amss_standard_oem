/*!
  @file
  ds_3gpp_dyn_mem_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the expressf
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_dyn_mem_hdlr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/12   vb      Initialize IRAT variables, function pointers. This file
				   ensures bearer context avoids including umts/eps bearer context

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h" 
#include "comdef.h"
#include "ds_3gpp_dyn_mem_hdlr.h"
#include "ds_3gpp_dyn_mem_hdlr.h" 
#include "ds_3gpp_bearer_context.h" 
#include "ds_eps_bearer_flow_manager.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_eps_flow_manager.h"

#ifdef FEATURE_DATA_LTE
#include "ds_3gpp_pdn_limit_hdlr.h"
#endif /*FEATURE_DATA_LTE*/
/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_init

DESCRIPTION
 This function registers the necessary function pointers.
 It is invoked during DS Task power-up.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_init( void )
{
 /*-------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_LTE

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)

  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_LTE, free_umts_bearer, \
						  dsUmtsBearerCntxt_free_umts_bearer);

  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_LTE, allocate_umts_bearer, \
						  dsUmtsBearerCntxt_allocate_umts_bearer);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, free_umts_bearer_flow_manager, \
			  ds_umts_bearer_flow_manager_free_umts_bearer_flow_manager);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, allocate_umts_bearer_flow_manager, \
         		  ds_umts_bearer_flow_manager_allocate_umts_bearer_flow_manager);

  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_LTE, allocate_umts_pdn_context, \
                        dsUmtsPdnCntx_alloc_dyn_mem);

  DS_PDN_CTXT_VF_REG(SYS_SYS_MODE_LTE, free_umts_pdn_context, \
                        dsUmtsPdnCntx_free_dyn_mem);
 #endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

#ifdef FEATURE_WCDMA

  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, allocate_eps_bearer, \
                          ds_eps_bearer_cntxt_alloc_dyn_mem);

	DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_WCDMA, free_eps_bearer, \
                          ds_eps_bearer_cntxt_free_dyn_mem);

	DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA,allocate_eps_bearer_flow_manager,\
                      ds_eps_bearer_flow_manager_alloc_eps_bearer_flow_manager);

	DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA,free_eps_bearer_flow_manager,\
                      ds_eps_bearer_flow_manager_free_eps_bearer_flow_manager);

	DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA,allocate_eps_flow_manager,\
                      ds_eps_flow_manager_alloc_eps_flow_manager);

	DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA,free_eps_flow_manager,\
                      ds_eps_flow_manager_free_eps_flow_manager);

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_GPRS
  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, allocate_eps_bearer, \
                          ds_eps_bearer_cntxt_alloc_dyn_mem);

  DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, free_eps_bearer, \
                          ds_eps_bearer_cntxt_free_dyn_mem);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM,allocate_eps_bearer_flow_manager,\
                      ds_eps_bearer_flow_manager_alloc_eps_bearer_flow_manager);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM,free_eps_bearer_flow_manager,\
                      ds_eps_bearer_flow_manager_free_eps_bearer_flow_manager);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM,allocate_eps_flow_manager,\
                      ds_eps_flow_manager_alloc_eps_flow_manager);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM,free_eps_flow_manager,\
                      ds_eps_flow_manager_free_eps_flow_manager);

#endif /* FEATURE_GSM_GPRS */

#ifdef FEATURE_TDSCDMA
	DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, allocate_eps_bearer, \
                          ds_eps_bearer_cntxt_alloc_dyn_mem);

	DS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_TDS, free_eps_bearer, \
                          ds_eps_bearer_cntxt_free_dyn_mem);

	DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS,allocate_eps_bearer_flow_manager,\
                      ds_eps_bearer_flow_manager_alloc_eps_bearer_flow_manager);

	DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS,free_eps_bearer_flow_manager,\
                      ds_eps_bearer_flow_manager_free_eps_bearer_flow_manager);

	DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS,allocate_eps_flow_manager,\
                      ds_eps_flow_manager_alloc_eps_flow_manager);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS,free_eps_flow_manager,\
                      ds_eps_flow_manager_free_eps_flow_manager);

#endif /* FEATURE_TDSCDMA */

 #endif /* FEATURE_DATA_LTE */

  return;

}

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_alloc_dyn_mem

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate umts
 bearer context and eps bearer context dynamically.
 
 The 3gpp bearer context is not allocated here
 
PARAMETERS
  int index
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_alloc_bearers_dyn_mem
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
  int loop_index = index;
  boolean rval=FALSE;
/*-------------------------------------------------------------------------*/
  if((loop_index < 0) || (loop_index >= DS_3GPP_MAX_BEARER_CONTEXT)) 
  {
    DS_3GPP_MSG0_ERROR("Invalid index passed");
    return rval;
  }

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)
  DS_BEARER_CTXT_VFR_CALL
  (  
    rval,  
    allocate_umts_bearer,
    subs_id,
    loop_index
  );
#else
  rval=TRUE;
#endif /*FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)*/
/*--------------------------------------------------------------------------- 
  If umts bearer allocation succeeds only then try eps bearer allocation
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  if(rval)
  {
    DS_BEARER_CTXT_VFR_CALL
    (
       rval,  
       allocate_eps_bearer,
       subs_id,
       loop_index
    );
  }
#endif /*FEATURE_DATA_LTE*/

  return rval;

} /* ds_3gpp_dyn_mem_hdlr_alloc_bearers_dyn_mem */

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_bearers_dyn_mem

DESCRIPTION
 This function dispatches the appropriate function pointers to free umts
 bearer context and eps bearer context dynamically.
 
PARAMETERS
  ds_bearer_context_p - Bearer context pointer
  subs_id             - subscription id

  3gpp bearer context memory is not freed here

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_bearers_dyn_mem
(
  ds_bearer_context_s     *bearer_context_p,
  sys_modem_as_id_e_type   subs_id
)
{
  boolean rval = FALSE;
/*-------------------------------------------------------------------------*/

/*------------------------------------------------------------------------- 
  Here the cookie check is not necessary since we are anyways trying to
  free the memory
 --------------------------------------------------------------------------*/
  if((bearer_context_p != NULL) && 
     (bearer_context_p->ds_bearer_context_dyn_p != NULL))
  {
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)

    DS_BEARER_CTXT_VFR_CALL
    (  
      rval, 
      free_umts_bearer,
      subs_id,
      bearer_context_p
    );
    
    DS_3GPP_MSG0_MED("Freeing WCDMA umts bearer context");
    if(rval == FALSE )
    {
      DS_3GPP_MSG0_ERROR("Error in freeing UMTS bearer");
    }

#endif /*FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)*/

#ifdef FEATURE_DATA_LTE
    DS_BEARER_CTXT_VFR_CALL
    (
      rval,
      free_eps_bearer,
      subs_id,
      bearer_context_p
    );
    
    DS_3GPP_MSG0_MED("Freeing LTE eps bearer context");
    if(rval == FALSE )
    {
      DS_3GPP_MSG0_ERROR("Error in freeing LTE bearer");
    }

#endif /*FEATURE_DATA_LTE*/
  }
  else
  {
		DS_3GPP_MSG0_ERROR("Bearer context is invalid");
  }
} /* ds_3gpp_dyn_mem_hdlr_free_bearers_dyn_mem */


/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_alloc_dyn_mem_umts_bearer_flow_manager

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate umts
 bearer flow manager dynamically.
 
PARAMETERS
  int index
  subs_id Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_allocate_umts_bearer_flow_manager
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
  int loop_index = index;
  boolean rval=FALSE;
/*-------------------------------------------------------------------------*/
  if((loop_index < 0) || (loop_index >= DS_3GPP_MAX_BEARER_CONTEXT)) 
  {
    DS_3GPP_MSG0_ERROR("Invalid index passed");
    return rval;
  }

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)
  DS_BEARER_FLOW_MGR_VFR_CALL
  (  
    rval,  
    allocate_umts_bearer_flow_manager,
    subs_id,
    loop_index
  );
#endif /*FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)*/

return rval;

} /* ds_3gpp_dyn_mem_hdlr_allocate_umts_bearer_flow_manager */

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_umts_bearer_flow_manager

DESCRIPTION
 This function dispatches the appropriate function pointers to free umts
 bearer flow manager.
 
PARAMETERS
  ds_bearer_context_p - Bearer context pointer
  subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_umts_bearer_flow_manager
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
  boolean rval = FALSE;
  int free_index = index;
/*-------------------------------------------------------------------------*/

  if((free_index < 0) || (free_index >= DS_3GPP_MAX_BEARER_CONTEXT)) 
  {
    DS_3GPP_MSG0_ERROR("Invalid index passed");
    return;
  }

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)
  DS_BEARER_FLOW_MGR_VFR_CALL
  (  
    rval, 
    free_umts_bearer_flow_manager,
    subs_id,
    free_index
  );
#else
  rval = TRUE;
#endif /*FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)*/

  if(rval == FALSE )
  {
    DS_3GPP_MSG0_ERROR("Error in freeing UMTS bearer flow manager");
  }
  return;
} /* ds_3gpp_dyn_mem_hdlr_free_umts_bearer_flow_manager */

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_alloc_dyn_mem_eps_bearer_flow_manager

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate eps
 bearer flow manager dynamically.
 
PARAMETERS
  int index
  subs_id Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_allocate_eps_bearer_flow_manager
(
   int                    alloc_index,
   sys_modem_as_id_e_type subs_id
)
{
#ifdef FEATURE_DATA_LTE
  boolean ret_val = FALSE;
/*-------------------------------------------------------------------------*/

  if((alloc_index < 0) && (alloc_index >= DS_3GPP_MAX_BEARER_CONTEXT))
  {
    DS_3GPP_MSG0_ERROR("Index out of bounds");
    return ret_val;
  }

  DS_BEARER_FLOW_MGR_VFR_CALL
  (  
    ret_val,  
    allocate_eps_bearer_flow_manager,
    subs_id,
    alloc_index
  );

  return ret_val;
#endif

  return TRUE;
} /* ds_3gpp_dyn_mem_hdlr_allocate_eps_bearer_flow_manager */

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_eps_bearer_flow_manager

DESCRIPTION
 This function frees eps bearer flow maanger
 
PARAMETERS
  index
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_eps_bearer_flow_manager
(
  int                    free_index,
  sys_modem_as_id_e_type subs_id
)
{
#ifdef FEATURE_DATA_LTE 
  boolean rval = FALSE;

/*-------------------------------------------------------------------------*/
  DS_BEARER_FLOW_MGR_VFR_CALL
  (  
    rval, 
    free_eps_bearer_flow_manager,
    subs_id,
    free_index
  );

  if(rval == FALSE )
  {
    DS_3GPP_MSG0_ERROR("Error in freeing eps bearer flow manager");
  }           
#endif

  return;
} /* ds_3gpp_dyn_mem_hdlr_free_eps_bearer_flow_manager */

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_flow_mgr_dyn_mem

DESCRIPTION
This function frees umts/eps bearer context, bearer context
apn tbl and pdn context in that order.
 
PARAMETERS
  ds_bearer_context_p - Bearer context pointer
 
  3gpp bearer context memory is not freed here

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_flow_mgr_dyn_mem
(
   ds_pdn_context_s *pdn_context_p
)
{
  ds_bearer_context_s *bearer_context_p = NULL;
  int 				   bearer_index = 0;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_DATA_LTE
  uint8                   index = 0;
#endif /*FEATURE_DATA_LTE*/
/*-------------------------------------------------------------------------*/
  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
     return;
  }

  /*Find the bearers from the pdn context and free every bearer*/
  for(bearer_index = 0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {
    bearer_context_p = 
     (ds_bearer_context_s *)pdn_context_p->ds_pdn_context_dyn_p->
       bearer_context_ptr_tbl[bearer_index];

    if((bearer_context_p != NULL) && 
       (bearer_context_p->ds_bearer_context_dyn_p != NULL))
    {
      ds_3gpp_dyn_mem_hdlr_free_bearers_dyn_mem(bearer_context_p, subs_id);
      ds_bearer_cntx_free_dyn_mem(bearer_context_p);
    } 
  }

  /* Now free the apn table and pdn context*/
  ds_pdn_cntxt_remove_entry_in_apn_table(pdn_context_p);
  
#ifdef FEATURE_DATA_LTE
  /* Find an entry in the PDN Limit Table.
     If an entry is found, validate if the entry can be cleared at this point.
     If yes, clear the entry */
  if((index = ds_3gpp_pdn_limit_tbl_find_entry
       ((byte*)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
         subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
  }
#endif /*FEATURE_DATA_LTE*/

	/* Undefine the timers before freeing the pdn cntxt*/
  rex_undef_timer(&(pdn_context_p->ds_pdn_context_dyn_p->pdn_inactivity_timer));

  rex_undef_timer(&(pdn_context_p->ds_pdn_context_dyn_p->polling_timer));

  ds_3gpp_pdn_cntx_free_dyn_mem(pdn_context_p, subs_id);
}

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem

DESCRIPTION
This function frees umts/eps bearer flow managers, bearer flow manager,
bearers and then if the bearer is default also frees apn and pdn
 
PARAMETERS
  ds_bearer_context_p - Bearer context pointer
 
  3gpp bearer context memory is not freed here

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem
(
  ds_bearer_context_s *bearer_context_p
)
{
  ds_pdn_context_s           *pdn_context_p = NULL;
  ds_bearer_flow_manager_s   *bflow_manager_p = NULL;
  ds_flow_manager_s          *flow_manager_p = NULL;
  int                         index = 0;
  boolean                     free_all_dyn_mem = FALSE;
  sys_modem_as_id_e_type      subs_id = SYS_MODEM_AS_ID_NONE;
/*-------------------------------------------------------------------------*/
  if((bearer_context_p == NULL) || 
     (bearer_context_p->ds_bearer_context_dyn_p == NULL))
  {
    DS_3GPP_MSG0_HIGH("Invalid bearer context passed");
    return;
  }

  pdn_context_p = (ds_pdn_context_s *)bearer_context_p->
                    ds_bearer_context_dyn_p->pdn_context_ptr;

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id)== FALSE)
  {
    return;
  }

  for(index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT;index++)
  {
    bflow_manager_p = &ds_bearer_flow_manager_tbl[index];
    if((bflow_manager_p != NULL) &&
       (bflow_manager_p->ds_bearer_flow_manager_dyn_p != NULL) &&
       (bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr == bearer_context_p))
    {
      /*------------------------------------------------------------------------- 
        Free UMTS and EPS bearer flow mgrs and bearer flow manager
      --------------------------------------------------------------------------*/
      ds_3gpp_dyn_mem_hdlr_free_eps_bearer_flow_manager
        (bflow_manager_p->ds_bearer_flow_manager_dyn_p->index, subs_id);

      ds_3gpp_dyn_mem_hdlr_free_umts_bearer_flow_manager
        (bflow_manager_p->ds_bearer_flow_manager_dyn_p->index, subs_id);

      ds_bearer_flow_manager_free_dyn_mem(bflow_manager_p);
      break;
    }
  }

  /*----------------------------------------------------------------------------- 
    Free all bearer contexts
    If its a default bearer context then free flow manager, apn tbl and
    pdn context
  -----------------------------------------------------------------------------*/
   free_all_dyn_mem = bearer_context_p->ds_bearer_context_dyn_p->is_default;

   ds_3gpp_dyn_mem_hdlr_free_bearers_dyn_mem(bearer_context_p, subs_id);

   ds_bearer_cntx_free_dyn_mem(bearer_context_p);

   if(free_all_dyn_mem)
   {
     for(index = 0;index < DS_3GPP_MAX_PDN_CONTEXT;index++)
     {
       flow_manager_p = &ds_flow_manager_tbl[index];
       if((flow_manager_p != NULL) &&
          (flow_manager_p->ds_flow_manager_dyn_p != NULL) &&
          (flow_manager_p->ds_flow_manager_dyn_p->pdn_ptr == pdn_context_p))
       {
         /*--------------------------------------------------------------------- 
           Free EPS flow manager, follow it with Flow mgr
         ---------------------------------------------------------------------*/
         ds_3gpp_dyn_mem_hdlr_free_eps_flow_manager
          (flow_manager_p->ds_flow_manager_dyn_p->index, subs_id);
         ds_flow_manager_free_dyn_mem(flow_manager_p);
         break;
       }
     }

     DS_3GPP_MSG0_HIGH("Free all dyn mem");
     ds_3gpp_dyn_mem_hdlr_free_flow_mgr_dyn_mem(pdn_context_p);
   }
} /* ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem */


/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_alloc_dyn_mem_eps_flow_manager

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate eps
 flow manager dynamically.
 
PARAMETERS
  int index
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_allocate_eps_flow_manager
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
#ifdef FEATURE_DATA_LTE
  boolean ret_val = FALSE;
  int alloc_index = index;
/*-------------------------------------------------------------------------*/
  if((alloc_index < 0) && (alloc_index >= DS_3GPP_MAX_PDN_CONTEXT))
  {
    DS_3GPP_MSG0_ERROR("Index out of bounds");
    return ret_val;
  }

  DS_FLOW_MGR_VFR_CALL
  (  
    ret_val,  
    allocate_eps_flow_manager,
    subs_id,
    alloc_index
  );

  return ret_val;
#endif

  return TRUE;  
} /* ds_3gpp_dyn_mem_hdlr_allocate_eps_flow_manager */

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_eps_flow_manager

DESCRIPTION
 This function frees eps flow maanger
 
PARAMETERS
  index
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_eps_flow_manager
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
#ifdef FEATURE_DATA_LTE 
  boolean rval = FALSE;
  int free_index = index;
/*-------------------------------------------------------------------------*/
  DS_FLOW_MGR_VFR_CALL
  (  
    rval, 
    free_eps_flow_manager,
    subs_id,
    free_index
  );

  if(rval == FALSE )
  {
    DS_3GPP_MSG0_ERROR("Error in freeing eps flow manager");
  }          
#endif

  return;

} /* ds_3gpp_dyn_mem_hdlr_free_eps_flow_manager */

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_free_umts_pdn_context

DESCRIPTION
 This function dispatches the appropriate function pointers to free umts
pdn context dynamically
 
PARAMETERS
  index   -  index in umts_pdn_c
  subs_id -  subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/


void ds_3gpp_dyn_mem_hdlr_free_umts_pdn_context
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
  boolean rval = FALSE;
  int     loop_index = index;
/*-------------------------------------------------------------------------*/

  if((loop_index < 0) || (loop_index >= DS_3GPP_MAX_PDN_CONTEXT)) 
  {
    DS_3GPP_MSG0_ERROR("Invalid index passed");
    return;
  }

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)
  DS_PDN_CTXT_VFR_CALL
  (     
    rval, 
    free_umts_pdn_context,
    subs_id,
    loop_index
  );
#else
  rval = TRUE;
#endif /*FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)*/

  if(rval == FALSE )
  {
    DS_3GPP_MSG0_ERROR("Error in freeing umts pdn context");
  }
  return;
} /* ds_3gpp_dyn_mem_hdlr_free_umts_pdn_context */

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_allocate_umts_pdn_context

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate umts
pdn context dynamically.
 
PARAMETERS
  int     index
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_allocate_umts_pdn_context
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
  int     loop_index = index;
  boolean rval=FALSE;
/*-------------------------------------------------------------------------*/
  if((loop_index < 0) || (loop_index >= DS_3GPP_MAX_PDN_CONTEXT)) 
  {
    DS_3GPP_MSG0_ERROR("Invalid index passed");
    return rval;
  }

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)
  DS_PDN_CTXT_VFR_CALL
  (  
    rval,  
    allocate_umts_pdn_context,
    subs_id,
    loop_index
  );
#endif /*FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)*/

  return rval;
} /* ds_3gpp_dyn_mem_hdlr_allocate_umts_pdn_context */


