/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/sys_eplmn_list.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/10/03   kwa     Initial release.
01/18/04   cd      Added NV control for EPLMN support in sys_eplmn_list_equivalent_plmn()
12/07/04   kwa     Reversed the order for comparing the input PLMN with an
                   equivalent PLMN in sys_eplmn_list_equivalent_rplmn.  The
                   EPLMN is now treated as a PLMN broadcast by the network.
03/22/05   up      Updated sys_eplmn_list_equivalent_rplmn to use 
                   sys_mcc_same_country to check if preferred PLMN is of same 
                   country as ePLMN, included NA range 310-316.
03/22/05   up      Added sys_eplmn_list_compare_eplmn_list to compare a new
                   ePLMN list with the stored one.
01/25/06   sn      Updated sys_eplmn_list_compare_eplmn_list to return TRUE
                   when both list are empty.
09/23/11   ss      Klockwork fixes
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err.h"
#include "rex.h"
#include "sys_eplmn_list_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "amssassert.h"
#include "mm_v.h"


/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#ifdef FEATURE_SGLTE
extern mm_as_id_e_type mm_cs_stack_id;
extern mm_as_id_e_type mm_ps_stack_id;
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#include "sys_cnst_v.h"
static eplmn_as_id_e_type sys_eplmn_as_id = EPLMN_AS_ID_1;


#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
static sys_plmn_list_s_type sys_equivalent_plmn_list_sim[MAX_NAS_STACKS] = { {0}, {0}, {0} } ;
static sys_plmn_id_s_type   sys_equivalent_rplmn_sim[MAX_NAS_STACKS] = { {{0xFF, 0xFF, 0xFF}}, {{0xFF, 0xFF, 0xFF}}, {{0xFF, 0xFF, 0xFF}} } ;
static boolean nv_item_read_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE } ;
static boolean nv_ePLMN_enabled_sim[MAX_NAS_STACKS] = { TRUE, TRUE, TRUE } ;
static boolean sys_eplmn_list_valid_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE} ;
#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
static boolean sys_eplmn_csfb_cmcc_hk_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE} ;
#endif
#else
static sys_plmn_list_s_type sys_equivalent_plmn_list_sim[MAX_NAS_STACKS] = { {0}, {0} } ;
static sys_plmn_id_s_type   sys_equivalent_rplmn_sim[MAX_NAS_STACKS] = { {{0xFF, 0xFF, 0xFF}}, {{0xFF, 0xFF, 0xFF}} } ;
static boolean nv_item_read_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
static boolean nv_ePLMN_enabled_sim[MAX_NAS_STACKS] = { TRUE, TRUE } ;
static boolean sys_eplmn_list_valid_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
static boolean sys_eplmn_csfb_cmcc_hk_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
#endif
#endif

#define sys_equivalent_plmn_list (sys_equivalent_plmn_list_sim[sys_eplmn_as_id])
#define sys_equivalent_rplmn (sys_equivalent_rplmn_sim[sys_eplmn_as_id])
#define nv_item_read (nv_item_read_sim[sys_eplmn_as_id])
#define nv_ePLMN_enabled (nv_ePLMN_enabled_sim[sys_eplmn_as_id])
#define sys_eplmn_list_valid (sys_eplmn_list_valid_sim[sys_eplmn_as_id])
#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
#define sys_eplmn_csfb_cmcc_hk  (sys_eplmn_csfb_cmcc_hk_sim[sys_eplmn_as_id])
#endif
extern uint8 mm_nv_context_id_sim[MAX_AS_IDS];

//#define IS_NOT_VALID_AS_ID(x) (((x) < SYS_MODEM_AS_ID_1) && ((x) >= (SYS_MODEM_AS_ID_1 + MAX_AS_IDS)))

#else

static sys_plmn_list_s_type sys_equivalent_plmn_list = {0};
static sys_plmn_id_s_type   sys_equivalent_rplmn = {{0xFF, 0xFF, 0xFF}};
static boolean nv_item_read = FALSE;
static boolean nv_ePLMN_enabled = TRUE;
static boolean sys_eplmn_list_valid = FALSE;
#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
static boolean sys_eplmn_csfb_cmcc_hk = FALSE;
#endif
#endif /* FEATURE_DUAL_SIM*/

/* Define the EPLMN REX critical section variable */
static rex_crit_sect_type  eplmn_crit_sect;
/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/
#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
// FR 31662
/*==============================================================================

FUNCTION NAME

  void sys_eplmn_per_nas_region_update

  DESCRIPTION
  This API will update the internal database with the argument passed in

==============================================================================*/
void sys_eplmn_per_nas_region_update_per_stack (boolean region, sys_modem_as_id_e_type as_id)
{	
    if(IS_NOT_VALID_STACK_ID(as_id))
    {
        ERR("Invalid STACK ID:%d", as_id+1,0,0);
        return;
    }
    sys_eplmn_as_id = (eplmn_as_id_e_type) as_id;
    sys_eplmn_csfb_cmcc_hk = region;	
}
#else
void sys_eplmn_per_nas_region_update(boolean region)
{		
#ifdef FEATURE_DUAL_SIM
	ERR("==Wrong API called for DUAL SIM: sys_eplmn_per_nas_region_update()==", 0, 0, 0);
	return ;
#endif

	sys_eplmn_csfb_cmcc_hk = region;	
}
#endif
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_update

==============================================================================*/

void sys_eplmn_per_nas_stacks_list_update
(
  sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p,
  sys_modem_as_id_e_type      as_id
)
{
  uint8 i;

  if(IS_NOT_VALID_STACK_ID(as_id))
  {
    ERR("Invalid STACK ID:%d", as_id+1,0,0);
    return;
  }

  ASSERT(equivalent_plmn_list_p->length <= SYS_PLMN_LIST_MAX_LENGTH);

  sys_eplmn_as_id = (eplmn_as_id_e_type) as_id;

  rex_enter_crit_sect(&eplmn_crit_sect);

  sys_equivalent_rplmn.identity[0] = rplmn.identity[0];
  sys_equivalent_rplmn.identity[1] = rplmn.identity[1];
  sys_equivalent_rplmn.identity[2] = rplmn.identity[2];

  sys_equivalent_plmn_list.length = MIN(equivalent_plmn_list_p->length,SYS_PLMN_LIST_MAX_LENGTH);

  for (i = 0; i < (int32)sys_equivalent_plmn_list.length; i++)
  {
    sys_equivalent_plmn_list.plmn[i].identity[0] = equivalent_plmn_list_p->plmn[i].identity[0];
    sys_equivalent_plmn_list.plmn[i].identity[1] = equivalent_plmn_list_p->plmn[i].identity[1];
    sys_equivalent_plmn_list.plmn[i].identity[2] = equivalent_plmn_list_p->plmn[i].identity[2];
  }

  rex_leave_crit_sect(&eplmn_crit_sect);
}


/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_rplmn

==============================================================================*/

sys_plmn_id_s_type sys_eplmn_per_nas_stacks_list_rplmn
(
    sys_modem_as_id_e_type      stack_id_nas
)
{
  sys_plmn_id_s_type rplmn;

  if(IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    ERR("Invalid STACK ID:%d", stack_id_nas+1,0,0);
    memset(&rplmn,0x00,sizeof(rplmn));
    return rplmn;
  }

  sys_eplmn_as_id = (eplmn_as_id_e_type) stack_id_nas;

  rex_enter_crit_sect(&eplmn_crit_sect);

  rplmn = sys_equivalent_rplmn;

  rex_leave_crit_sect(&eplmn_crit_sect);

  return rplmn;
}


/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_equivalent_rplmn

==============================================================================*/

boolean sys_eplmn_per_nas_stacks_list_equivalent_rplmn
(
  sys_plmn_id_s_type preferred_plmn,
  uint32             rplmn_mcc,
  sys_modem_as_id_e_type      as_id
)
{
  boolean value = FALSE;
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  eplmn_mcc;
  uint32  eplmn_mnc;
  int32 i;

  if(IS_NOT_VALID_STACK_ID(as_id))
  {
    ERR("Invalid STACK ID:%d", as_id+1,0,0);
    return FALSE;
  }

  sys_eplmn_as_id = (eplmn_as_id_e_type) as_id;

  rex_enter_crit_sect(&eplmn_crit_sect);
  if(sys_eplmn_list_valid)
  {
    ASSERT(sys_equivalent_plmn_list.length <= SYS_PLMN_LIST_MAX_LENGTH);

    for (i = 0; i < (int32)(MIN(sys_equivalent_plmn_list.length,SYS_PLMN_LIST_MAX_LENGTH)); i++)
    {
      sys_plmn_get_mcc_mnc
      (
        sys_equivalent_plmn_list.plmn[i],
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &eplmn_mcc,
        &eplmn_mnc
      );

      /*
      ** If the preferred PLMN is on the equivalent PLMN list
      ** and the country code matches the country code of the RPLMN then
      ** the search for a higher priority PLMN is complete.
      */
      if ( (sys_plmn_match(preferred_plmn, sys_equivalent_plmn_list.plmn[i])) &&
           (sys_mcc_same_country(eplmn_mcc, rplmn_mcc)) )
      {
        value = TRUE;
        break;
      }
    }
  }

  rex_leave_crit_sect(&eplmn_crit_sect);

  return value;
}

/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_compare_eplmn_list

==============================================================================*/

boolean sys_eplmn_per_nas_stacks_list_compare_eplmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p,
  sys_modem_as_id_e_type      as_id
)
{
  boolean value = FALSE;
  uint8 i;

  if(IS_NOT_VALID_STACK_ID(as_id))
  {
    ERR("Invalid STACK ID:%d", as_id+1,0,0);
    return FALSE;
  }

  sys_eplmn_as_id = (eplmn_as_id_e_type) as_id;

  rex_enter_crit_sect(&eplmn_crit_sect);
  
  if((sys_equivalent_plmn_list.length == 0) && (equivalent_plmn_list_p->length == 0))
  {
    value = TRUE;
  }
  else if(!sys_eplmn_list_valid)
  {
    /* If eplmn list is invalid and non-zero, sent FALSE, so that EPLMN list can be updated correctly. */
    value = FALSE;
  }
  else if (sys_plmn_match(sys_equivalent_rplmn, rplmn))
  {
    ASSERT(sys_equivalent_plmn_list.length <= SYS_PLMN_LIST_MAX_LENGTH);
    if (sys_equivalent_plmn_list.length == equivalent_plmn_list_p->length)
    {
      value = TRUE;
      for (i = 0; i < (int32)MIN(equivalent_plmn_list_p->length,SYS_PLMN_LIST_MAX_LENGTH); i++)
      {
        if (!sys_plmn_list_found_plmn(&sys_equivalent_plmn_list, equivalent_plmn_list_p->plmn[i]))
        {
          value = FALSE;
          break;
        }
      }
    }
  }

  rex_leave_crit_sect(&eplmn_crit_sect);

  return value;
}

/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_validate

==============================================================================*/

boolean sys_eplmn_per_nas_stacks_list_validate
(
  boolean valid,
  sys_modem_as_id_e_type  stack_id_nas
)
{
  if(IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    ERR("Invalid STACK ID:%d", stack_id_nas+1,0,0);
    return FALSE;
  }
  sys_eplmn_as_id = (eplmn_as_id_e_type) stack_id_nas;

  if (sys_eplmn_list_valid == valid)
  {    
    return FALSE;
  }
  else
  {
    rex_enter_crit_sect(&eplmn_crit_sect);
    sys_eplmn_list_valid = valid;
    rex_leave_crit_sect(&eplmn_crit_sect);
    return TRUE;    
  }
}


/*===========================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_get


===========================================================================*/

void sys_eplmn_per_nas_stacks_list_get
(
  sys_plmn_list_s_type* eplmn_list_p,
  sys_plmn_id_s_type* rplmn_p,
  sys_modem_as_id_e_type      as_id
)
{
  uint32 i;

  if(IS_NOT_VALID_STACK_ID(as_id))
  {
    ERR("Invalid STACK ID:%d", as_id+1,0,0);
    return;
  }

  sys_eplmn_as_id = (eplmn_as_id_e_type) as_id;

  rex_enter_crit_sect(&eplmn_crit_sect);
  
  if(sys_eplmn_list_valid)
  {
     *rplmn_p = sys_equivalent_rplmn;
     ASSERT(sys_equivalent_plmn_list.length <= SYS_PLMN_LIST_MAX_LENGTH);
     for (i = 0; i < MIN(sys_equivalent_plmn_list.length,SYS_PLMN_LIST_MAX_LENGTH); i++)
     {
       eplmn_list_p->plmn[i] = sys_equivalent_plmn_list.plmn[i];
     }
     eplmn_list_p->length = MIN(sys_equivalent_plmn_list.length,SYS_PLMN_LIST_MAX_LENGTH);
  }
  else
  {
     /* If eplmn list is invalid and non-zero, send FALSE, so that EPLMN list can be updated correctly. */
     eplmn_list_p->length = 0;
     rplmn_p->identity[0] = 0xFF;
     rplmn_p->identity[1] = 0xFF;
     rplmn_p->identity[2] = 0xFF;
  }

  rex_leave_crit_sect(&eplmn_crit_sect);
  return;

}

#endif


/*===========================================================================

FUNCTION NAME

  sys_eplmn_per_subs_list_get


  DESCRIPTION

  Function to get the eplmn_list per sub. For SGLTE sub, PS stack EPLMN list will be returned.


===========================================================================*/

void sys_eplmn_per_subs_list_get
(
  sys_plmn_list_s_type* eplmn_list_p,
  sys_plmn_id_s_type* rplmn_p,
  sys_modem_as_id_e_type      sub_id
)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
   (void)sub_id;
   return sys_eplmn_list_get(eplmn_list_p, rplmn_p);
#else
   sys_modem_as_id_e_type stack_id_nas = SYS_MODEM_AS_ID_1;
#ifdef FEATURE_DUAL_SIM
   if(IS_NOT_VALID_SUB_ID(sub_id))
   {
     ERR("Invalid SUB ID:%d", sub_id+1,0,0);
     return;
   }
   stack_id_nas = sub_id;
#endif
 
#ifdef FEATURE_SGLTE
   if(MM_SUB_IS_SGLTE_SUB(sub_id))
   {
     stack_id_nas = mm_ps_stack_id;
   }
#endif 
   sys_eplmn_per_nas_stacks_list_get(eplmn_list_p,rplmn_p,stack_id_nas);
#endif
}


/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_equivalent_plmn
    This function is only called by NAS. Third parameter here is stack id in NAS format.

==============================================================================*/

boolean sys_eplmn_per_nas_stacks_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type      sub_id,
  sys_modem_as_id_e_type      stack_id_nas
)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
  (void)sub_id;
  (void)stack_id_nas;
  return sys_eplmn_list_equivalent_plmn(plmn);
#else
  boolean value = FALSE;
#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  rplmn_mcc;
  uint32  rplmn_mnc;
  uint32  plmn_mcc;
  uint32  plmn_mnc;
#endif
  if(IS_NOT_VALID_STACK_ID(stack_id_nas) ||
     IS_NOT_VALID_SUB_ID(sub_id))
  {
    ERR("Invalid STACK ID:%d or SUB ID %d", stack_id_nas+1,sub_id+1,0);
    return FALSE;
  }

  sys_eplmn_as_id = (eplmn_as_id_e_type) stack_id_nas;

#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
  if (sys_eplmn_csfb_cmcc_hk == TRUE)
  {
      /* UE is registered in CMCC or HK area*/
            
      sys_plmn_get_mcc_mnc
      (
        sys_equivalent_rplmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &rplmn_mcc,
        &rplmn_mnc
      );

      sys_plmn_get_mcc_mnc
      (
        plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &plmn_mcc,
        &plmn_mnc
      );

	  //if (! sys_mcc_same_country(rplmn_mcc, plmn_mcc))
       if ((rplmn_mcc != plmn_mcc) && ((plmn_mcc == 460) || (plmn_mcc == 454)))
       {
	  /* Cross country case between CMCC and HK*/
	  /* case of a move CMCC-> HK or HK -> CMCC*/
          MSG_HIGH_DS_0(MM_SUB,"=MM= FR3166 Crossover, return FALSE");
	      return FALSE;
       }
  }
#endif

  if(!nv_item_read)
  {
   if (ghdi_nvmem_read_per_subs(NV_EPLMN_ENABLED_I,
            (ghdi_nvmem_data_T *)&nv_ePLMN_enabled, mm_nv_context_id_sim[sub_id]) != GHDI_SUCCESS)
   {
     nv_ePLMN_enabled = TRUE;
     ERR("Unable to access NV to read NV_EPLMN_ENABLED_I",0,0,0);
   }
   nv_item_read = TRUE;
  }

  if(nv_ePLMN_enabled)
  {

     rex_enter_crit_sect(&eplmn_crit_sect);

     if ((sys_plmn_list_found_plmn(&sys_equivalent_plmn_list, plmn) && sys_eplmn_list_valid == TRUE) ||
         sys_plmn_match(sys_equivalent_rplmn, plmn))
     {
        value = TRUE;
     }

     rex_leave_crit_sect(&eplmn_crit_sect);
  }
  
  return value;
#endif
}


/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_subs_list_equivalent_plmn
    This function is called by WRRC with the subscription ID

==============================================================================*/
boolean sys_eplmn_per_subs_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type      sub_id
)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
  (void)sub_id;
  return sys_eplmn_list_equivalent_plmn(plmn);
#else
  sys_modem_as_id_e_type stack_id_nas = SYS_MODEM_AS_ID_1;
  if(IS_NOT_VALID_SUB_ID(sub_id))
  {
    ERR("Invalid SUB ID:%d", sub_id+1,0,0);
    return FALSE;
  }

#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
  else
#endif 
  {
    stack_id_nas = sub_id;
  }
  return sys_eplmn_per_nas_stacks_list_equivalent_plmn(plmn,sub_id,stack_id_nas);
#endif
}

/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_stacks_list_equivalent_plmn
    This function is called by CM with sub_id and Stack_id in CM<->NAS interface format.

==============================================================================*/
boolean sys_eplmn_per_stacks_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
)
{
  sys_modem_as_id_e_type stack_id_nas = mm_get_nas_stack_id(sub_id,stack_id);
  return sys_eplmn_per_nas_stacks_list_equivalent_plmn(plmn,sub_id,stack_id_nas);
}



/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_update

==============================================================================*/

void sys_eplmn_list_update
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p
)
{
#ifdef FEATURE_DUAL_SIM
  ERR("==Wrong API called for DUAL SIM: sys_eplmn_list_update()==", 0, 0, 0);
#else
  uint32 i;

  ASSERT(equivalent_plmn_list_p->length <= SYS_PLMN_LIST_MAX_LENGTH);

  rex_enter_crit_sect(&eplmn_crit_sect);

  sys_equivalent_rplmn.identity[0] = rplmn.identity[0];
  sys_equivalent_rplmn.identity[1] = rplmn.identity[1];
  sys_equivalent_rplmn.identity[2] = rplmn.identity[2];

  sys_equivalent_plmn_list.length = MIN(equivalent_plmn_list_p->length,SYS_PLMN_LIST_MAX_LENGTH);

  for (i = 0; i < sys_equivalent_plmn_list.length; i++)
  {
    sys_equivalent_plmn_list.plmn[i].identity[0] = equivalent_plmn_list_p->plmn[i].identity[0];
    sys_equivalent_plmn_list.plmn[i].identity[1] = equivalent_plmn_list_p->plmn[i].identity[1];
    sys_equivalent_plmn_list.plmn[i].identity[2] = equivalent_plmn_list_p->plmn[i].identity[2];
  }

  rex_leave_crit_sect(&eplmn_crit_sect);
#endif
}


/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_rplmn

==============================================================================*/

sys_plmn_id_s_type sys_eplmn_list_rplmn
(
  void
)
{
  sys_plmn_id_s_type rplmn;

#ifdef FEATURE_DUAL_SIM
  ERR("==Wrong API called for DUAL SIM: sys_eplmn_list_rplmn()==", 0, 0, 0);
  memset(&rplmn,0x00,sizeof(rplmn));
  return rplmn;
#else
  rex_enter_crit_sect(&eplmn_crit_sect);


  rplmn = sys_equivalent_rplmn;

  rex_leave_crit_sect(&eplmn_crit_sect);

  return rplmn;
#endif
}

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_equivalent_plmn

==============================================================================*/

boolean sys_eplmn_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#ifdef FEATURE_DUAL_SIM
  ERR("==Wrong API called for DUAL SIM: sys_eplmn_list_equivalent_plmn()==", 0, 0, 0);
  return FALSE;
#else
  return sys_eplmn_per_nas_stacks_list_equivalent_plmn(plmn,SYS_MODEM_AS_ID_1,mm_ps_stack_id);
#endif
#else

  boolean value = FALSE;
#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  rplmn_mcc;
  uint32  rplmn_mnc;
  uint32  plmn_mcc;
  uint32  plmn_mnc;
#endif
  
#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
  if (sys_eplmn_csfb_cmcc_hk == TRUE)
  {
      /* UE is registered in CMCC or HK area*/
            
     sys_plmn_get_mcc_mnc
      (
        sys_equivalent_rplmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &rplmn_mcc,
        &rplmn_mnc
      );

	  sys_plmn_get_mcc_mnc
      (
        plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &plmn_mcc,
        &plmn_mnc
      );

	  //if (! sys_mcc_same_country(rplmn_mcc, plmn_mcc))
      if ((rplmn_mcc != plmn_mcc) && ((plmn_mcc == 460) || (plmn_mcc == 454)))
      {
	  /* Cross country case between CMCC and HK*/
	  /* case of a move CMCC-> HK or HK -> CMCC*/
	  return FALSE;
      }
  }
#endif

  if(!nv_item_read)
  {
   if (ghdi_nvmem_read(NV_EPLMN_ENABLED_I,
            (ghdi_nvmem_data_T *)&nv_ePLMN_enabled) != GHDI_SUCCESS)
   {
     nv_ePLMN_enabled = TRUE;
     ERR("Unable to access NV to read NV_EPLMN_ENABLED_I",0,0,0);
   }
   nv_item_read = TRUE;
  }

  if(nv_ePLMN_enabled)
  {

     rex_enter_crit_sect(&eplmn_crit_sect);

     if ((sys_plmn_list_found_plmn(&sys_equivalent_plmn_list, plmn) && sys_eplmn_list_valid == TRUE) ||
         sys_plmn_match(plmn , sys_equivalent_rplmn))
     {
        value = TRUE;
     }

     rex_leave_crit_sect(&eplmn_crit_sect);
  }

  return value;
#endif
}


/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_equivalent_rplmn

==============================================================================*/

boolean sys_eplmn_list_equivalent_rplmn
(
  sys_plmn_id_s_type preferred_plmn,
  uint32             rplmn_mcc
)
{
#ifdef FEATURE_DUAL_SIM
  ERR("==Wrong API called for DUAL SIM: sys_eplmn_list_equivalent_rplmn()==", 0, 0, 0);
  return FALSE;
#else
  boolean value = FALSE;
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  eplmn_mcc;
  uint32  eplmn_mnc;

  int32 i;

  rex_enter_crit_sect(&eplmn_crit_sect);
  if(sys_eplmn_list_valid)
  {
    ASSERT(sys_equivalent_plmn_list.length <= SYS_PLMN_LIST_MAX_LENGTH);
    for (i = 0; i < (int32)(MIN(sys_equivalent_plmn_list.length,SYS_PLMN_LIST_MAX_LENGTH)); i++)
    {
      sys_plmn_get_mcc_mnc
      (
        sys_equivalent_plmn_list.plmn[i],
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &eplmn_mcc,
        &eplmn_mnc
      );

      /*
      ** If the preferred PLMN is on the equivalent PLMN list
      ** and the country code matches the country code of the RPLMN then
      ** the search for a higher priority PLMN is complete.
      */
      if ( (sys_plmn_match(preferred_plmn, sys_equivalent_plmn_list.plmn[i])) &&
           (sys_mcc_same_country(eplmn_mcc, rplmn_mcc)) )
      {
        value = TRUE;
        break;
      }
    }
  }

  rex_leave_crit_sect(&eplmn_crit_sect);

  return value;
#endif
}

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_compare_eplmn_list

==============================================================================*/

boolean sys_eplmn_list_compare_eplmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p
)
{
#ifdef FEATURE_DUAL_SIM
  ERR("==Wrong API called for DUAL SIM: sys_eplmn_list_compare_eplmn_list()==", 0, 0, 0);
  return FALSE;
#else
  boolean value = FALSE;
  uint32 i;

  rex_enter_crit_sect(&eplmn_crit_sect);
  
  if((sys_equivalent_plmn_list.length == 0) && (equivalent_plmn_list_p->length == 0))
  {
    value = TRUE;
  }
  else if(!sys_eplmn_list_valid)
  {
    /* If eplmn list is invalid and non-zero, sent FALSE, so that EPLMN list can be updated correctly. */
    value = FALSE;
  }
  else if (sys_plmn_match(sys_equivalent_rplmn, rplmn))
  {
    ASSERT(sys_equivalent_plmn_list.length <= SYS_PLMN_LIST_MAX_LENGTH);
    if (sys_equivalent_plmn_list.length == equivalent_plmn_list_p->length)
    {
      value = TRUE;
      for (i = 0; i < MIN(equivalent_plmn_list_p->length,SYS_PLMN_LIST_MAX_LENGTH); i++)
      {
        if (!sys_plmn_list_found_plmn(&sys_equivalent_plmn_list, equivalent_plmn_list_p->plmn[i]))
        {
          value = FALSE;
          break;
        }
      }
    }
  }

  rex_leave_crit_sect(&eplmn_crit_sect);

  return value;
#endif
}

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_validate

==============================================================================*/

boolean sys_eplmn_list_validate
(
  boolean valid
)
{

#ifdef FEATURE_DUAL_SIM
  ERR("==Wrong API called for DUAL SIM: sys_eplmn_list_validate()==", 0, 0, 0);
  return FALSE;
#else

  if (sys_eplmn_list_valid == valid)
  {    
    return FALSE;
  }
  else
  {
    rex_enter_crit_sect(&eplmn_crit_sect);
    sys_eplmn_list_valid = valid;
    rex_leave_crit_sect(&eplmn_crit_sect);   
    return TRUE;    
  }
#endif
}

/*===========================================================================

FUNCTION NAME

  sys_eplmn_list_get


===========================================================================*/

void sys_eplmn_list_get(sys_plmn_list_s_type* eplmn_list_p, sys_plmn_id_s_type* rplmn_p)
{
#ifdef FEATURE_DUAL_SIM
  ERR("==Wrong API called for DUAL SIM: sys_eplmn_list_get()==", 0, 0, 0);
#else
  uint32 i;

  rex_enter_crit_sect(&eplmn_crit_sect);
  
  if(sys_eplmn_list_valid)
  {
     *rplmn_p = sys_equivalent_rplmn;
     ASSERT(sys_equivalent_plmn_list.length <= SYS_PLMN_LIST_MAX_LENGTH);
     for (i = 0; i < MIN(sys_equivalent_plmn_list.length,SYS_PLMN_LIST_MAX_LENGTH); i++)
     {
       eplmn_list_p->plmn[i] = sys_equivalent_plmn_list.plmn[i];
     }
     eplmn_list_p->length = MIN(sys_equivalent_plmn_list.length,SYS_PLMN_LIST_MAX_LENGTH);
  }
  else
  {
     /* If eplmn list is invalid and non-zero, send FALSE, so that EPLMN list can be updated correctly. */
     eplmn_list_p->length = 0;
     rplmn_p->identity[0] = 0xFF;
     rplmn_p->identity[1] = 0xFF;
     rplmn_p->identity[2] = 0xFF;
  }

  rex_leave_crit_sect(&eplmn_crit_sect);
  return;
#endif

}
/*===========================================================================

FUNCTION NAME

  sys_eplmn_list_init_crit_sect


===========================================================================*/
void sys_eplmn_list_init_crit_sect(void)
{
  /* Initialize the Critical Section */
  rex_init_crit_sect(&eplmn_crit_sect);
}

#ifdef FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_subs_list_update

DESCRIPTION

  Function that updates the equivalent PLMN list stored in RAM for use by the
  tasks calling the other functions in this file.This function used only in SGLTE .

RETURN VALUE

  None

==============================================================================*/
void sys_eplmn_list_update_sglte
(
  sys_plmn_id_s_type    rplmn_sglte,
  const sys_plmn_list_s_type* equivalent_plmn_list_sglte_p,
  sys_radio_access_tech_e_type current_rat
)
{
  if ( SYS_RAT_GSM_RADIO_ACCESS == current_rat )
  {
    sys_eplmn_per_nas_stacks_list_update(rplmn_sglte, equivalent_plmn_list_sglte_p, (sys_modem_as_id_e_type)mm_cs_stack_id);
  }
   else if((SYS_RAT_UMTS_RADIO_ACCESS == current_rat) || (SYS_RAT_TDS_RADIO_ACCESS == current_rat ) ||
               (SYS_RAT_LTE_RADIO_ACCESS == current_rat))

  {
    sys_eplmn_per_nas_stacks_list_update(rplmn_sglte, equivalent_plmn_list_sglte_p, (sys_modem_as_id_e_type)mm_ps_stack_id);
  }
}

boolean sys_eplmn_list_equivalent_plmn_sglte(  sys_plmn_id_s_type plmn,sys_radio_access_tech_e_type current_rat)
{
  if ( SYS_RAT_GSM_RADIO_ACCESS == current_rat )
  {
   return sys_eplmn_per_nas_stacks_list_equivalent_plmn(plmn,SYS_MODEM_AS_ID_1, (sys_modem_as_id_e_type)mm_cs_stack_id);
  }
  else
 /*    if((SYS_RAT_UMTS_RADIO_ACCESS == current_rat) || (SYS_RAT_TDS_RADIO_ACCESS == current_rat ) ||
               (SYS_RAT_LTE_RADIO_ACCESS == current_rat))
*/
  {
    return sys_eplmn_per_nas_stacks_list_equivalent_plmn(plmn,SYS_MODEM_AS_ID_1, (sys_modem_as_id_e_type)mm_ps_stack_id);
  }
 
}



#endif

/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_subs_list_update

DESCRIPTION

  Function that updates the equivalent PLMN list stored in RAM for use by the
  tasks calling the other functions in this file.

RETURN VALUE

  None

==============================================================================*/
void sys_eplmn_list_update_with_rat
(
  sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p,
  sys_modem_as_id_e_type      sub_id,
  sys_radio_access_tech_e_type current_rat
)
{
#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM)
  sys_modem_as_id_e_type stack_id_nas = sub_id;

  if(IS_NOT_VALID_SUB_ID(sub_id))
  {
    ERR("Invalid SUB ID:%d", sub_id+1,0,0);
    return;
  }

#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    if ( SYS_RAT_GSM_RADIO_ACCESS == current_rat )
    {
      stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
    }
     else   
    {
      stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
    }
  }
#endif 

  sys_eplmn_per_nas_stacks_list_update(rplmn, equivalent_plmn_list_p,stack_id_nas);
#else
  sys_eplmn_list_update(rplmn, equivalent_plmn_list_p);
#endif 
}

boolean sys_eplmn_list_equivalent_plmn_with_rat
(  
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type     sub_id,
  sys_radio_access_tech_e_type current_rat
)
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#ifdef FEATURE_SGLTE
  if(IS_NOT_VALID_SUB_ID(sub_id))
  {
    ERR("Invalid SUB ID:%d", sub_id+1,0,0);
    return FALSE;
  }
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    if ( SYS_RAT_GSM_RADIO_ACCESS == current_rat )
    {
      stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
    }
    else
    {
      stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
    }
  }
#endif 
  return sys_eplmn_per_nas_stacks_list_equivalent_plmn(plmn,sub_id,stack_id_nas);
}


