#ifndef SYS_EPLMN_LIST_V_H
#define SYS_EPLMN_LIST_V_H
/*==============================================================================


              S Y S - E P L M N   L I S T   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains functions for managing and using the equivalent
  PLMN list.  Functions are provided to load the equivalent PLMN list from
  NVRAM, save the equivalent PLMN list to NVRAM, and to determine if a PLMN
  is contained in the equivalent PLMN list.

                             COPYRIGHT INFORMATION

Copyright (c) 2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/sys_eplmn_list_v.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/10/03   kwa     Initial version.
03/22/05   up      Added sys_eplmn_list_compare_eplmn_list to compare a new
                   ePLMN list with the stored one.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/


#include "mmcp_variation.h"
#include "sys_eplmn_list.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/**
** Enumeration of Active Subscription ID's 
*/
typedef enum {

  EPLMN_AS_ID_1 = 0,
    /**< Subscription ID 1 */

  EPLMN_AS_ID_2,
    /**< Subscription ID 2 */

  EPLMN_AS_ID_3
    /**< Subscription ID 3 */
  
} eplmn_as_id_e_type;
#endif /*FEATURE_DUAL_SIM*/
/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_update

DESCRIPTION

  Function that updates the equivalent PLMN list stored in RAM for use by the
  tasks calling the other functions in this file.

RETURN VALUE

  None

==============================================================================*/

void sys_eplmn_per_nas_stacks_list_update
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p,
  sys_modem_as_id_e_type      as_id
);


/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_rplmn

DESCRIPTION

  Function that returns the RPLMN that provided the equivalent PLMN list.

RETURN VALUE

  sys_plmn_id_s_type  PLMN id of the the registered PLMN that provided the
                      equivalent PLMN list.

==============================================================================*/

sys_plmn_id_s_type sys_eplmn_per_nas_stacks_list_rplmn
(
  sys_modem_as_id_e_type      as_id
);
/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_equivalent_rplmn

DESCRIPTION

  Function that determines if the input PLMN is contained in the equivalent
  PLMN list and is in the same country as the RPLMN.

RETURN VALUE

  boolean  Flag indicating whether the PLMN is contained in the equivalent PLMN
           list and is in the same country as the RPLMN (TRUE) or not (FALSE).

==============================================================================*/

boolean sys_eplmn_per_nas_stacks_list_equivalent_rplmn
(
  sys_plmn_id_s_type preferred_plmn,
  uint32             rplmn_mcc,
  sys_modem_as_id_e_type      as_id
);


/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_compare_eplmn_list

DESCRIPTION

  Function that compares if the input equivalent PLMN list is matching with
  the stored equivalent PLMN list. RPLMN in both the lists should also match.

RETURN VALUE

  boolean  Flag indicating whether the given equivalent PLMN list matches with
           the stored equivalent PLMN list (TRUE) or not (FALSE).

==============================================================================*/

boolean sys_eplmn_per_nas_stacks_list_compare_eplmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p,
  sys_modem_as_id_e_type      as_id
);
#endif


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
);

/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_validate

DESCRIPTION

  Function to make EPLMN list valid / invalid. Updated sys_eplmn_per_subs_list_valid flag.

RETURN VALUE
  None
  
==============================================================================*/

boolean sys_eplmn_per_nas_stacks_list_validate
(
  boolean valid,
  sys_modem_as_id_e_type stack_id_nas
);

/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_nas_stacks_list_get

DESCRIPTION

  Function to fill the input pointers with the current RPLMN and its EPLMN list.

RETURN VALUE
  None
  
==============================================================================*/

void sys_eplmn_per_nas_stacks_list_get
(
  sys_plmn_list_s_type* eplmn_list_p, 
  sys_plmn_id_s_type* rplmn_p,
  sys_modem_as_id_e_type      as_id
);

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_update

DESCRIPTION

  Function that updates the equivalent PLMN list stored in RAM for use by the
  tasks calling the other functions in this file.

RETURN VALUE

  None

==============================================================================*/

void sys_eplmn_list_update
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p
);


/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_rplmn

DESCRIPTION

  Function that returns the RPLMN that provided the equivalent PLMN list.

RETURN VALUE

  sys_plmn_id_s_type  PLMN id of the the registered PLMN that provided the
                      equivalent PLMN list.

==============================================================================*/

sys_plmn_id_s_type sys_eplmn_list_rplmn
(
  void
);

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_equivalent_rplmn

DESCRIPTION

  Function that determines if the input PLMN is contained in the equivalent
  PLMN list and is in the same country as the RPLMN.

RETURN VALUE

  boolean  Flag indicating whether the PLMN is contained in the equivalent PLMN
           list and is in the same country as the RPLMN (TRUE) or not (FALSE).

==============================================================================*/

boolean sys_eplmn_list_equivalent_rplmn
(
  sys_plmn_id_s_type preferred_plmn,
  uint32             rplmn_mcc
);


/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_compare_eplmn_list

DESCRIPTION

  Function that compares if the input equivalent PLMN list is matching with
  the stored equivalent PLMN list. RPLMN in both the lists should also match.

RETURN VALUE

  boolean  Flag indicating whether the given equivalent PLMN list matches with
           the stored equivalent PLMN list (TRUE) or not (FALSE).

==============================================================================*/

boolean sys_eplmn_list_compare_eplmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* equivalent_plmn_list_p
);

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_validate

DESCRIPTION

  Function to make EPLMN list valid / invalid. Updated sys_eplmn_list_valid flag.

RETURN VALUE
  None
  
==============================================================================*/

boolean sys_eplmn_list_validate
(
  boolean valid
);

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_get

DESCRIPTION

  Function to fill the input pointers with the current RPLMN and its EPLMN list.

RETURN VALUE
  None
  
==============================================================================*/

void sys_eplmn_list_get
(
  sys_plmn_list_s_type* eplmn_list_p, 
  sys_plmn_id_s_type* rplmn_p
);

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_init_crit_sect

DESCRIPTION

  Function to initialize the critical section variable for eplmn.

RETURN VALUE
  None
  
==============================================================================*/
void sys_eplmn_list_init_crit_sect(void);

#endif
