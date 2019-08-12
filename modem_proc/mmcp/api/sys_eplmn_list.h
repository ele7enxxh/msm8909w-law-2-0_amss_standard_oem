#ifndef SYS_EPLMN_LIST_H
#define SYS_EPLMN_LIST_H
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sys_eplmn_list.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/10/03   kwa     Initial version.
03/22/05   up      Added sys_eplmn_list_compare_eplmn_list to compare a new
                   ePLMN list with the stored one.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "sys.h"
#include "sys_v.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/


/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_subs_list_equivalent_plmn

DESCRIPTION

  Function that determines if the input PLMN is contained in the equivalent
  PLMN list.
  This function is called by WRRC with the subscription ID

RETURN VALUE

  boolean  Flag indicating whether the PLMN is contained in the equivalent
           PLMN list (TRUE) or not (FALSE).

==============================================================================*/

boolean sys_eplmn_per_subs_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type      as_id
);

/*===========================================================================

FUNCTION NAME

  sys_eplmn_per_subs_list_get


  DESCRIPTION

  Function to get the eplmn_list per sub. For SGLTE sub, PS stack EPLMN list will be returned.


===========================================================================*/

extern void sys_eplmn_per_subs_list_get
(
  sys_plmn_list_s_type* eplmn_list_p,
  sys_plmn_id_s_type* rplmn_p,
  sys_modem_as_id_e_type      sub_id
);


/*==============================================================================

FUNCTION NAME

  sys_eplmn_per_stacks_list_equivalent_plmn

  DESCRIPTION

    This function is called by CM with sub_id and Stack_id in CM<->NAS interface format.

RETURN VALUE

  boolean  Flag indicating whether the PLMN is contained in the equivalent
           PLMN list (TRUE) or not (FALSE).

==============================================================================*/
extern boolean sys_eplmn_per_stacks_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
);


/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_equivalent_plmn

DESCRIPTION

  Function that determines if the input PLMN is contained in the equivalent
  PLMN list.

RETURN VALUE

  boolean  Flag indicating whether the PLMN is contained in the equivalent
           PLMN list (TRUE) or not (FALSE).

==============================================================================*/

boolean sys_eplmn_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn
);

boolean sys_eplmn_list_equivalent_plmn_sglte
(
  sys_plmn_id_s_type plmn,
  sys_radio_access_tech_e_type rat
);

/*==============================================================================

FUNCTION NAME

  sys_eplmn_list_equivalent_plmn_with_rat

DESCRIPTION

  Function that determines if the input PLMN is contained in the equivalent
  PLMN list. This functions uses rat parameter for SGLTE device and modifies
  the as_id. For Dual SIM device passed parameter of as_id is used directly.

RETURN VALUE

  boolean  Flag indicating whether the PLMN is contained in the equivalent
           PLMN list (TRUE) or not (FALSE).

==============================================================================*/
boolean sys_eplmn_list_equivalent_plmn_with_rat
(  
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type     as_id,
  sys_radio_access_tech_e_type current_rat
);



#endif
