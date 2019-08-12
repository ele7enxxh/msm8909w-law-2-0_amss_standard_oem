#ifndef _SYS_PLMN_SELECTION_V_H_
#define _SYS_PLMN_SELECTION_V_H_
/*==============================================================================

       S Y S T E M   P L M N   S E L E C T I O N   H E A D E R   F I L E

DESCRIPTION

  This header file contains the types associated with the process of selecting
  a Public Land Mobile Network (PLMN) for service.


         COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
         EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_plmn_selection.h_v   1.0   10 May 2002 15:26:34   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/sys_plmn_selection_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/22/10   abhi    Updating PLMN service state to include RAC/MME code
09/21/09   MNK     Changes due to IRAT design
07/30/08   rm      CMI defeaturisation
06/25/07   vm      Corrected the comments and the feature name
06/25/07   vm      Added uoos_rem_sleep_time and uoos_rem_awake_time in 
                   sys_plmn_service_state_s_type structure.
04/11/07   ic      Fixed several compiler errors
02/07/02   kwa     Initial version.
10/28/02   kwa     Added SYS_STOP_MODE_REASON_DEEP_SLEEP.
02/28/03   kwa     Added ps_data_suspend to sys_plmn_service_state_s_type.
09/25/03   ks      Added enumeration SYS_STOP_MODE_REASON_MODE_CHANGE_FTM.
09/26/03   kwa     Added sys_band_mask_e_type and sys_lac_type to
                   sys_plmn_service_state_s_type.
09/29/03   kwa     Added SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                   to sys_network_selection_mode_e_type to differentiate
                   between attempts to find limited service when an emergency
                   call needs to be made from attempts to simply acquire some
                   form of service.
08/12/04   kwa     Removed the unused enumerated type sys_nreg_reason_e_type
                   and corresponding function sys_nreg_reason_is_valid.
11/04/04   kwa     Added the structures sys_plmn_rat_list_info_s_type
                   and sys_plmn_rat_list_s_type along with the
                   corresponding verification functions
                   sys_plmn_rat_list_info_is_valid and
                   sys_plmn_rat_list_is_valid.
02/16/05   up      Added SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION
                   to sys_network_selection_mode_e_type to acquire service on
                   requested PLMN if available otherwise LIMITED on other PLMN.
01/31/06   up      Use new enum SYS_PRIORITY_PLMN_LIST_MAX_LENGTH in place of
                   SYS_PLMN_LIST_MAX_LENGTH for sys_plmn_rat_list type.
06/09/06   sn      Added STOP_MODE_REASON_LPM.
10/16/06   sn      Added WCDMA_1X implementation.
04/11/07   pk      Reverting the earlier check-in, checked-in the changes by
                   mistake
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "sys_plmn_selection.h"

/*==============================================================================

                             DATA DECLARATIONS

==============================================================================*/
#ifdef FEATURE_FEMTO_CSG
/*
**Structure that defines a PLMN, CSG Id pair
*/
typedef struct
{
  sys_plmn_id_s_type    plmn;
  sys_csg_id_type   csg_id;  
  dword  timer_count; 
}sys_csg_list_info_s_type;

/*
**Define a strutcure for CSG list 
*/
typedef struct
{
  sys_csg_list_info_s_type    *csg;
  uint16 length;
}sys_csg_list_s_type;

#endif 



/*==============================================================================

              Types for the service state on the selected network

==============================================================================*/

/*
** Define a structure that contains the collection of information that
** fully defines the service state of the mobile on a PLMN.
**
** If the PLMN_service_capability is
** SYS_SVC_CAPABILITY_NOT_AVAILABLE, then the plmn field
** is undefined.
*/





/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_selection_mode_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_network_selection_mode_e_type value        Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_network_selection_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_network_selection_mode_is_valid
(
  sys_network_selection_mode_e_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_mode_pref_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_mode_pref_e_type value                     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_mode_pref_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_mode_pref_is_valid
(
  sys_mode_pref_e_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_stop_mode_reason_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_stop_mode_reason_e_type value              Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_stop_mode_reason_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_stop_mode_reason_is_valid
(
  sys_stop_mode_reason_e_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_service_state_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_plmn_service_state_s_type value            Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_service_state_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean sys_plmn_service_state_is_valid
(
  sys_plmn_service_state_s_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_rat_list_info_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_plmn_rat_list_info_s_type value            Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_rat_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_plmn_rat_list_info_is_valid
(
  sys_plmn_rat_list_info_s_type value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_rat_list_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_plmn_rat_list_s_type* value_ptr            Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_rat_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_plmn_rat_list_is_valid
(
  const sys_plmn_rat_list_s_type* value_ptr
);

#ifdef FEATURE_RAT_PRIORITY_LIST
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_mmss_rat_pri_list_is_valid

DESCRIPTION

  Function that verifies that the mmss rat priority list  is having valid data

PARAMETERS

  sys_mmss_rat_pri_list_is_valid *value_ptr       Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_rat_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_mmss_rat_pri_list_is_valid
(
  const sys_rat_pri_list_info_s_type *value_ptr
);
#endif /* FEATURE_RAT_PRIORITY_LIST*/


#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  void sys_eplmn_per_nas_region_update

  DESCRIPTION
  This API will update the internal database with the arguments passed in


==============================================================================*/
void sys_eplmn_per_nas_region_update_per_stack (boolean region, sys_modem_as_id_e_type as_id);
#else
void sys_eplmn_per_nas_region_update (boolean region);
#endif
#endif

#endif /* #ifndef _SYS_PLMN_SELECTION_H_ */
