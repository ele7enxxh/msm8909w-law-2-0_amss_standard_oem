/*==============================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_plmn_selection.c_v   1.0   10 May 2002 15:26:38   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/sys_plmn_selection.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/31/06   up      Use new enum SYS_PRIORITY_PLMN_LIST_MAX_LENGTH in place of
                   SYS_PLMN_LIST_MAX_LENGTH for sys_plmn_rat_list type.
11/04/04   kwa     Added the structures sys_plmn_rat_list_info_s_type
                   and sys_plmn_rat_list_s_type along with the
                   corresponding verification functions
                   sys_plmn_rat_list_info_is_valid and
                   sys_plmn_rat_list_is_valid.
08/12/04   kwa     Removed unused function sys_nreg_reason_is_valid.
07/14/03   bbd     Added FEATURE_WCDMA_PLT
10/18/02   ma, ao  Made linker happy by adding FEATURE_GSM_PLT
02/07/02   kwa     Initial version.
==============================================================================*/

/*==============================================================================

                     INCLUDE FILES FOR MODULE

==============================================================================*/
#include "mmcp_variation.h"
#include "sys_plmn_selection_v.h"
#include "amssassert.h"
/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

==============================================================================*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_selection_mode_is_valid

==============================================================================*/

boolean sys_network_selection_mode_is_valid
(
  sys_network_selection_mode_e_type value
)
{
  boolean valid = FALSE;

  valid = (boolean)(value > SYS_NETWORK_SELECTION_MODE_NONE &&
                    value < SYS_NETWORK_SELECTION_MODE_MAX);

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_mode_pref_is_valid

==============================================================================*/

boolean sys_mode_pref_is_valid
(
  sys_mode_pref_e_type value
)
{
  boolean valid = FALSE;

  switch(value)
  {
    case SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY:
    case SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY:
    case SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
    case SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
    case SYS_MODE_PREF_TDS_RADIO_ACCESS_ONLY:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif
#ifdef FEATURE_LTE
    case SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY:
#endif
      valid = TRUE;
      break;
    default:
      valid = FALSE;
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_stop_mode_reason_is_valid

==============================================================================*/

boolean sys_stop_mode_reason_is_valid
(
  sys_stop_mode_reason_e_type value
)
{
  boolean valid = FALSE;

  valid = (boolean)(value > SYS_STOP_MODE_REASON_NONE &&
                    value < SYS_STOP_MODE_REASON_MAX);

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_service_state_is_valid

==============================================================================*/

boolean sys_plmn_service_state_is_valid
(
  sys_plmn_service_state_s_type value
)
{
  boolean valid = FALSE;

  #if !defined(FEATURE_GSM_PLT) && !defined(FEATURE_WCDMA_PLT)
  valid = (boolean)(sys_sim_state_is_valid         ( value.sim_state ) &&
                    sys_plmn_id_is_valid           ( value.plmn ) &&
                    sys_srv_capability_is_valid    ( value.plmn_service_capability ) &&
                    sys_roam_status_is_valid       ( value.roaming_ind ) &&
                    sys_srv_status_is_valid        ( value.service_status ) &&
                    sys_srv_domain_is_valid        ( value.reg_service_domain ) &&
                    sys_radio_access_tech_is_valid ( value.active_rat ));
  #endif // FEATURE_GSM_PLT
  
  return valid;
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_rat_list_info_is_valid

==============================================================================*/
boolean sys_plmn_rat_list_info_is_valid
(
  sys_plmn_rat_list_info_s_type value
)
{
  boolean valid;

  valid = (boolean)( sys_plmn_id_is_valid                     ( value.plmn ) &&
                     sys_radio_access_tech_is_valid           ( value.rat ) );

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_rat_list_is_valid

==============================================================================*/
boolean sys_plmn_rat_list_is_valid
(
  const sys_plmn_rat_list_s_type* value_ptr
)
{
  uint32   i;
  boolean  valid = TRUE;

  if (value_ptr->length <= SYS_PRIORITY_PLMN_LIST_MAX_LENGTH)
  {
    for (i = 0; i < value_ptr->length; i++)
    {
      if (!sys_plmn_rat_list_info_is_valid (value_ptr->info[i]))
      {
        valid = FALSE;
        break;
      }
    }
  }
  else
  {
    valid = FALSE;
  }

  return valid;
}

#ifdef FEATURE_RAT_PRIORITY_LIST
boolean sys_mmss_rat_pri_list_is_valid
(
  const sys_rat_pri_list_info_s_type *value_ptr
)
{
  uint32 i;
  boolean valid = TRUE;  
  boolean is_3gpp_rat_present = FALSE;
  sys_sys_mode_e_type   acq_sys_mode;

  ASSERT(value_ptr != (void *)NULL);

  if(value_ptr->num_items > SYS_MAX_ACQ_PREF_RAT_LIST_NUM)
  {
    valid = FALSE;
  }

  for (i = 0; i < value_ptr->num_items; i++)
  {
    acq_sys_mode = value_ptr->priority_list_info[i].acq_sys_mode;

    if(acq_sys_mode == SYS_SYS_MODE_GW ||acq_sys_mode == SYS_SYS_MODE_GWL)
    {
      valid = FALSE;  
      break;
    }
    if(acq_sys_mode == SYS_SYS_MODE_GSM || 
         acq_sys_mode == SYS_SYS_MODE_WCDMA ||
         acq_sys_mode == SYS_SYS_MODE_TDS ||
         acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      is_3gpp_rat_present = TRUE;
    }             
  }
  return (valid && is_3gpp_rat_present);

}

#endif 
