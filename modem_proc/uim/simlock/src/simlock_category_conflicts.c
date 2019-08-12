/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   C A T E G O R Y   C O N F L I C T S

                            F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock category conflicts functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_category_conflicts.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
06/17/16   vdc     Added support for SPN based SIMLOCK feature
05/12/14   vv      Fix compiler errors
04/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_util.h"
#include "simlock_category.h"
#include "simlock_category_conflicts.h"

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_CODES_IN_CATEGORIES

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  a 3GPP or 3GPP2 category lock. It checks all the currently enabled
  categories between the requested begin and end category types.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE 
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_codes_in_categories
(
  const simlock_category_code_data_type  * lock_code_data_ptr,
  const simlock_config_data_type         * config_data_ptr,
  simlock_category_enum_type               begin_category,
  simlock_category_enum_type               end_category,
  simlock_category_enum_type               lock_category,
  simlock_slot_enum_type                   slot
)
{
  simlock_result_enum_type            simlock_status     = SIMLOCK_SUCCESS;
  simlock_category_enum_type          category           = begin_category;
  uint8                               code_cnt           = 0;
  boolean                             code_valid         = TRUE;
  boolean                             match_found        = FALSE;
  simlock_category_file_data_type     enabled_category_data;

  if((lock_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(simlock_util_check_category_supported(lock_category) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_UNSUPPORTED;
  }

  /* check the slot */
  if(simlock_util_check_slot(slot) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  for(category = begin_category;
      category <= end_category; category++)
  {
    memset(&enabled_category_data, 0x00,
              sizeof(simlock_category_file_data_type));

    simlock_status = simlock_util_check_category_supported(category);
    if(simlock_status == SIMLOCK_UNSUPPORTED)
    {
      continue;
    }

    /* if the categories are same, continue */
    if(category == lock_category)
    {
      continue;
    }

    if(!simlock_util_check_category_enabled(config_data_ptr,
                                            category,
                                            slot))
    {
      continue;
    }

    /* reset the match found */
    match_found = FALSE;

    /* read the category code data */
    simlock_status = simlock_category_read_lock_data(category,
                                                     slot,
                                                     &enabled_category_data);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_0("read lock data failed");

      /* this frees the code data in the category */
      simlock_category_free_code_data(&enabled_category_data);
      return SIMLOCK_GENERIC_ERROR;
    }

    /* if autolock is on for other category,
       we should not accept code data for this category */
    if(enabled_category_data.category_header.auto_lock)
    {
      SIMLOCK_MSG_ERR_1("autolock is enabled for the category, 0x%x",
                        category);

      /* this frees the code data in the category */
      simlock_category_free_code_data(&enabled_category_data);
      return SIMLOCK_GENERIC_ERROR;
    }

    if(enabled_category_data.code_data_ptr == NULL)
    {
      SIMLOCK_MSG_ERR_1("NULL code data for the category, 0x%x",
                        category);

      /* this frees the code data in the category */
      simlock_category_free_code_data(&enabled_category_data);
      return SIMLOCK_GENERIC_ERROR;
    }

    for(code_cnt = 0; code_cnt < enabled_category_data.category_header.num_of_codes;
        code_cnt++)
    {
      match_found = simlock_util_check_if_codes_match(&enabled_category_data.code_data_ptr[code_cnt],
                                                      lock_code_data_ptr,
                                                      category,
                                                      lock_category);
      if(match_found)
      {
        break;
      }
    }

    /* this frees the code data in the category */
    simlock_category_free_code_data(&enabled_category_data);

    code_valid = match_found;

    /* if the code is not valid in this category, no need to
       check the other enabled categories */
    if(!match_found)
    {
      break;
    }
  }

  return (code_valid ? SIMLOCK_SUCCESS : SIMLOCK_CONFLICTING_LOCK_DATA);
} /* simlock_category_check_codes_in_categories */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_RUIM_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the RUIM category lock

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_ruim_codes_for_conflicts
(
  const simlock_category_data_type  * ruim_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  uint8                               ruim_code_cnt    = 0;
  simlock_category_code_data_type     ruim_code_data;

  if((ruim_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if atleast one mcc-mnc is present in all the enabled categories,
     then codes are good */
  for(ruim_code_cnt = 0; ruim_code_cnt < ruim_code_data_ptr->code_data.ruim_3gpp2_code_list.num_sim_codes;
      ruim_code_cnt++)
  {
    memset(&ruim_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(ruim_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    ruim_code_data_ptr->code_data.ruim_3gpp2_code_list.sim_code_data[ruim_code_cnt].nw_code.mcc,
                    SIMLOCK_MCC_LEN);

    ruim_code_data.mnc =
         ruim_code_data_ptr->code_data.ruim_3gpp2_code_list.sim_code_data[ruim_code_cnt].nw_code.mnc;

    ruim_code_data.msin =
         ruim_code_data_ptr->code_data.ruim_3gpp2_code_list.sim_code_data[ruim_code_cnt].imsi_msin;

    /* check if the code is valid in all the enabled 3GPP2 categories */
    simlock_status = simlock_category_check_codes_in_categories(&ruim_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP2_NW_TYPE1,
                                                                SIMLOCK_CATEGORY_3GPP2_RUIM,
                                                                SIMLOCK_CATEGORY_3GPP2_RUIM,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_ruim_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_NW_TYPE2_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the NW_TYPE2 category lock
 
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_nw_type2_codes_for_conflicts
(
  const simlock_category_data_type     * nw_type2_code_data_ptr,
  const simlock_config_data_type       * config_data_ptr,
  simlock_slot_enum_type                 slot
)
{
  simlock_result_enum_type            simlock_status      = SIMLOCK_SUCCESS;
  uint8                               nw_type2_code_cnt   = 0;
  simlock_category_code_data_type     nw_type2_code_data;

  if((nw_type2_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  for(nw_type2_code_cnt = 0; nw_type2_code_cnt < nw_type2_code_data_ptr->code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes;
      nw_type2_code_cnt++)
  {
    memset(&nw_type2_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(nw_type2_code_data.msin.msin_data,
                    SIMLOCK_MSIN_MAX,
                    nw_type2_code_data_ptr->code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[nw_type2_code_cnt].irm,
                    SIMLOCK_IRM_LEN);

    /* check if the code is valid in all the enabled 3GPP2 categories */
    simlock_status = simlock_category_check_codes_in_categories(&nw_type2_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP2_NW_TYPE1,
                                                                SIMLOCK_CATEGORY_3GPP2_RUIM,
                                                                SIMLOCK_CATEGORY_3GPP2_NW_TYPE2,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_nw_type2_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_NW_TYPE1_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the NW_TYPE1 category lock
 
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_nw_type1_codes_for_conflicts
(
  const simlock_category_data_type  * nw_type1_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status      = SIMLOCK_SUCCESS;
  uint8                               nw_type1_code_cnt   = 0;
  simlock_category_code_data_type     nw_type1_code_data;

  if((nw_type1_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if atleast one mcc-mnc is present in all the enabled categories,
     then codes are good */
  for(nw_type1_code_cnt = 0; nw_type1_code_cnt < nw_type1_code_data_ptr->code_data.nw_type1_3gpp2_code_list.num_nw_codes;
      nw_type1_code_cnt++)
  {
    memset(&nw_type1_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(nw_type1_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    nw_type1_code_data_ptr->code_data.nw_type1_3gpp2_code_list.nw_code_data[nw_type1_code_cnt].mcc,
                    SIMLOCK_MCC_LEN);

    nw_type1_code_data.mnc =
           nw_type1_code_data_ptr->code_data.nw_type1_3gpp2_code_list.nw_code_data[nw_type1_code_cnt].mnc;

    /* check if the code is valid in all the enabled 3GPP categories */
    simlock_status = simlock_category_check_codes_in_categories(&nw_type1_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP2_NW_TYPE2,
                                                                SIMLOCK_CATEGORY_3GPP2_RUIM,
                                                                SIMLOCK_CATEGORY_3GPP2_NW_TYPE1,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_nw_type1_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_SIM_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the SIM category lock

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_sim_codes_for_conflicts
(
  const simlock_category_data_type  * sim_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  uint8                               sim_code_cnt     = 0;
  simlock_category_code_data_type     sim_code_data;

  if((sim_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if atleast one mcc-mnc is present in all the enabled categories,
     then codes are good */
  for(sim_code_cnt = 0; sim_code_cnt < sim_code_data_ptr->code_data.sim_3gpp_code_list.num_sim_codes;
      sim_code_cnt++)
  {
    memset(&sim_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(sim_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    sim_code_data_ptr->code_data.sim_3gpp_code_list.sim_code_data[sim_code_cnt].nw_code.mcc,
                    SIMLOCK_MCC_LEN);

    sim_code_data.mnc =
        sim_code_data_ptr->code_data.sim_3gpp_code_list.sim_code_data[sim_code_cnt].nw_code.mnc;

    sim_code_data.msin =
        sim_code_data_ptr->code_data.sim_3gpp_code_list.sim_code_data[sim_code_cnt].imsi_msin;

    /* check if the code is valid in all the enabled 3GPP categories */
    simlock_status = simlock_category_check_codes_in_categories(&sim_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP_NW,
                                                                SIMLOCK_CATEGORY_3GPP_SIM,
                                                                SIMLOCK_CATEGORY_3GPP_SIM,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_sim_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_SPN_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the SPN category lock

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_spn_codes_for_conflicts
(
  const simlock_category_data_type  * sim_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  uint8                               spn_code_cnt     = 0;
  simlock_category_code_data_type     spn_code_data;

  if((sim_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if atleast one mcc-mnc is present in all the enabled categories,
     then codes are good */
  for(spn_code_cnt = 0; spn_code_cnt < sim_code_data_ptr->code_data.spn_3gpp_code_list.num_spn_codes;
      spn_code_cnt++)
  {
    memset(&spn_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(spn_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    sim_code_data_ptr->code_data.spn_3gpp_code_list.spn_code_data[spn_code_cnt].nw_code.mcc,
                    SIMLOCK_MCC_LEN);

    spn_code_data.mnc =
        sim_code_data_ptr->code_data.spn_3gpp_code_list.spn_code_data[spn_code_cnt].nw_code.mnc;

    /* check if the code is valid in all the enabled 3GPP categories */
    simlock_status = simlock_category_check_codes_in_categories(&spn_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP_NW,
                                                                SIMLOCK_CATEGORY_3GPP_SIM,
                                                                SIMLOCK_CATEGORY_3GPP_SPN,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_spn_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_ICCID_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the ICCID category lock

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_iccid_codes_for_conflicts
(
  const simlock_category_data_type  * sim_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  uint8                               iccid_code_cnt     = 0;
  simlock_category_code_data_type     iccid_code_data;

  if((sim_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if atleast one mcc-mnc is present in all the enabled categories,
     then codes are good */
  for(iccid_code_cnt = 0; iccid_code_cnt < sim_code_data_ptr->code_data.iccid_3gpp_code_list.num_iccid_codes;
      iccid_code_cnt++)
  {
    memset(&iccid_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(iccid_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    sim_code_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[iccid_code_cnt].nw_code.mcc,
                    SIMLOCK_MCC_LEN);

    iccid_code_data.mnc =
        sim_code_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[iccid_code_cnt].nw_code.mnc;

    /* check if the code is valid in all the enabled 3GPP categories */
    simlock_status = simlock_category_check_codes_in_categories(&iccid_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP_NW,
                                                                SIMLOCK_CATEGORY_3GPP_SIM,
                                                                SIMLOCK_CATEGORY_3GPP_ICCID,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_iccid_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_CP_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the CP category lock
 
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_cp_codes_for_conflicts
(
  const simlock_category_data_type  * cp_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  uint8                               cp_code_cnt      = 0;
  simlock_category_code_data_type     cp_code_data;

  if((cp_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if atleast one mcc-mnc is present in all the enabled categories,
     then codes are good */
  for(cp_code_cnt = 0; cp_code_cnt < cp_code_data_ptr->code_data.cp_3gpp_code_list.num_cp_codes;
      cp_code_cnt++)
  {
    memset(&cp_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(cp_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    cp_code_data_ptr->code_data.cp_3gpp_code_list.cp_code_data[cp_code_cnt].nw_code.mcc,
                    SIMLOCK_MCC_LEN);

    cp_code_data.mnc =
           cp_code_data_ptr->code_data.cp_3gpp_code_list.cp_code_data[cp_code_cnt].nw_code.mnc;

    cp_code_data.gid1 =
          cp_code_data_ptr->code_data.cp_3gpp_code_list.cp_code_data[cp_code_cnt].gid1;

    /* check if the code is valid in all the enabled 3GPP categories */
    simlock_status = simlock_category_check_codes_in_categories(&cp_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP_NW,
                                                                SIMLOCK_CATEGORY_3GPP_SIM,
                                                                SIMLOCK_CATEGORY_3GPP_CP,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_cp_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_SP_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the SP category lock
 
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_sp_codes_for_conflicts
(
  const simlock_category_data_type  * sp_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  uint8                               sp_code_cnt      = 0;
  simlock_category_code_data_type     sp_code_data;

  if((sp_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if atleast one mcc-mnc is present in all the enabled categories,
     then codes are good */
  for(sp_code_cnt = 0; sp_code_cnt < sp_code_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes;
      sp_code_cnt++)
  {
    memset(&sp_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(sp_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    sp_code_data_ptr->code_data.sp_3gpp_code_list.sp_code_data[sp_code_cnt].nw_code.mcc,
                    SIMLOCK_MCC_LEN);

    sp_code_data.mnc =
       sp_code_data_ptr->code_data.sp_3gpp_code_list.sp_code_data[sp_code_cnt].nw_code.mnc;

    /* check if the code is valid in all the enabled 3GPP categories */
    simlock_status = simlock_category_check_codes_in_categories(&sp_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP_NW,
                                                                SIMLOCK_CATEGORY_3GPP_SIM,
                                                                SIMLOCK_CATEGORY_3GPP_SP,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_sp_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_NS_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the NS category lock
 
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_ns_codes_for_conflicts
(
  const simlock_category_data_type  * ns_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  uint8                               ns_code_cnt      = 0;
  simlock_category_code_data_type     ns_code_data;

  if((ns_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  for(ns_code_cnt = 0; ns_code_cnt < ns_code_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes;
      ns_code_cnt++)
  {
    memset(&ns_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(ns_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    ns_code_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[ns_code_cnt].nw_code.mcc,
                    SIMLOCK_MCC_LEN);

    ns_code_data.mnc =
       ns_code_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[ns_code_cnt].nw_code.mnc;

    ns_code_data.msin.msin_data[0] =
      ns_code_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[ns_code_cnt].imsi_digit6;

    ns_code_data.msin.msin_data[1] =
      ns_code_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[ns_code_cnt].imsi_digit7;

    ns_code_data.msin.msin_len = 0x02;

    /* check if the code is valid in all the enabled 3GPP categories */
    simlock_status = simlock_category_check_codes_in_categories(&ns_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP_NW,
                                                                SIMLOCK_CATEGORY_3GPP_SIM,
                                                                SIMLOCK_CATEGORY_3GPP_NS,
                                                                slot);
    /* If atleast one code is present in all the enabled categories or no
       categories are enabled, then accept all the codes in case the
       simlock_status returns as a success or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_ns_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_CHECK_NW_CODES_FOR_CONFLICTS

DESCRIPTION
  This function checks if the codes provided can be added for enabling
  the NW category lock
 
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_check_nw_codes_for_conflicts
(
  const simlock_category_data_type  * nw_code_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  uint8                               nw_code_cnt      = 0;
  simlock_category_code_data_type     nw_code_data;

  if((nw_code_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if atleast one mcc-mnc is present in all the enabled categories,
     then codes are good */
  for(nw_code_cnt = 0; nw_code_cnt < nw_code_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes;
      nw_code_cnt++)
  {
    memset(&nw_code_data, 0x00, sizeof(simlock_category_code_data_type));

    simlock_memscpy(nw_code_data.mcc,
                    SIMLOCK_MCC_LEN,
                    nw_code_data_ptr->code_data.nw_3gpp_code_list.nw_code_data[nw_code_cnt].mcc,
                    SIMLOCK_MCC_LEN);

    nw_code_data.mnc =
       nw_code_data_ptr->code_data.nw_3gpp_code_list.nw_code_data[nw_code_cnt].mnc;

    /* check if the code is valid in all the enabled 3GPP categories */
    simlock_status = simlock_category_check_codes_in_categories(&nw_code_data,
                                                                config_data_ptr,
                                                                SIMLOCK_CATEGORY_3GPP_NS,
                                                                SIMLOCK_CATEGORY_3GPP_SIM,
                                                                SIMLOCK_CATEGORY_3GPP_NW,
                                                                slot);
    /* if atleast one code is present in all the enabled
       categories or no categories are enabled, then
       accept all the codes */
    /* in case the simlock_status returns as a success
       or failure break. */
    if(simlock_status != SIMLOCK_CONFLICTING_LOCK_DATA)
    {
      break;
    }
  }

  return simlock_status;
} /* simlock_category_check_nw_codes_for_conflicts */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_VERIFY_CODES_FOR_CONFLICTS

DESCRIPTION
  This function verifies if the lock codes being added for a category do not
  have any conflicts with the already enabled codes.
  For example, if the 3GPP NW category is enabled for mcc1-mnc1 then the
  3GPP SP category can be enabled for mcc1-mnc1-sp1.
 
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_verify_codes_for_conflicts
(
  const simlock_category_data_type  * category_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
)
{
  simlock_result_enum_type    simlock_status = SIMLOCK_SUCCESS;

  if((category_data_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_category_verify_codes_for_conflicts");

  switch(category_data_ptr->category_type)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      simlock_status = simlock_category_check_nw_codes_for_conflicts(category_data_ptr,
                                                                     config_data_ptr,
                                                                     slot);
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
      simlock_status = simlock_category_check_ns_codes_for_conflicts(category_data_ptr,
                                                                     config_data_ptr,
                                                                     slot);
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
      simlock_status = simlock_category_check_sp_codes_for_conflicts(category_data_ptr,
                                                                     config_data_ptr,
                                                                     slot);
      break;

    case SIMLOCK_CATEGORY_3GPP_CP:
      simlock_status = simlock_category_check_cp_codes_for_conflicts(category_data_ptr,
                                                                     config_data_ptr,
                                                                     slot);
      break;

    case SIMLOCK_CATEGORY_3GPP_SIM:
      simlock_status = simlock_category_check_sim_codes_for_conflicts(category_data_ptr,
                                                                      config_data_ptr,
                                                                      slot);
      break;

    case SIMLOCK_CATEGORY_3GPP_SPN:
      simlock_status = simlock_category_check_spn_codes_for_conflicts(category_data_ptr,
                                                                      config_data_ptr,
                                                                      slot);
      break;

    case SIMLOCK_CATEGORY_3GPP_ICCID:
      simlock_status = simlock_category_check_iccid_codes_for_conflicts(category_data_ptr,
                                                                        config_data_ptr,
                                                                        slot);
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      simlock_status = simlock_category_check_nw_type1_codes_for_conflicts(category_data_ptr,
                                                                           config_data_ptr,
                                                                           slot);
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      simlock_status = simlock_category_check_nw_type2_codes_for_conflicts(category_data_ptr,
                                                                           config_data_ptr,
                                                                           slot);
      break;

    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      simlock_status = simlock_category_check_ruim_codes_for_conflicts(category_data_ptr,
                                                                       config_data_ptr,
                                                                       slot);
      break;

    default:
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
  }

  return simlock_status;
} /* simlock_category_verify_codes_for_conflicts */

#endif /* FEATURE_SIMLOCK */

