/*==============================================================================
                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_cm.c_v   1.0   10 May 2002 14:45:20   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_cm.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/07/02   kwa     Initial version.
09/19/02   kwa     Added cm_act_req.
10/18/02   kwa     Added cm_deep_sleep_ind.
05/22/03   kwa     Modified call to sys_mm_information_is_valid to pass a
                   pointer rather than a structure.  Modified message
                   verification functions to pass const pointers to eliminate
                   lint warnings. 
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Replaced user_selected_plmn in cm_service_req_manual_s_type
                   with selected_plmn_type to allow CM to initiate a manual
                   search for the HPLMN in addition to the RPLMN and a user
                   specified PLMN.  Change was featurized using
                   FEATURE_MANUAL_HPLMN_SELECTION.
09/29/03   kwa     Added code to handle the additional enumerated value
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                   in the function cm_service_req_is_valid.
05/06/04   kwa     Added cm_camped_ind_is_valid.
07/19/04   kwa     Removed service_state parameter from cm_camped_ind.
07/26/04   kwa     Added active_rat and active_band parameters to cm_camped_ind.
01/11/05   up      Mainlining FEATURE_MANUAL_HPLMN_SELECTION.
02/17/05   up      Updated cm_service_req_is_valid() to include the check for
                   SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION.
09/19/05   up      Added cm_hplmn_search_req/cnf_is_valid to support on demand
                   HPLMN Search.
08/30/11   rajesh  Made changes not to check for validity of mode_pref in cm_service_req and 
                   cm_network_list_req when FEATURE_RAT_PRIORITY_LIST is defined.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "reg_cm_v.h"
#include "reg_mode.h"

#ifdef FEATURE_DUAL_SIM
reg_as_id_e_type reg_multimode_sub = REG_AS_ID_1;
#endif

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  reg_cm_manually_selected_plmn_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  reg_cm_manually_selected_plmn_e_type value     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the reg_cm_manually_selected_plmn_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean reg_cm_manually_selected_plmn_is_valid
(
  reg_cm_manually_selected_plmn_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > REG_CM_MANUALLY_SELECTED_NONE &&
                    value < REG_CM_MANUALLY_SELECTED_MAX);

  return valid;
}



/*==============================================================================   

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  reg_cm_scan_scop_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_scan_scope_e_type value     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the scan scope type 
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

boolean reg_cm_scan_scop_is_valid( sys_scan_scope_e_type  scan_scope)
{
  boolean valid;

  valid = (boolean)(scan_scope > SYS_SCAN_SCOPE_NONE &&
                    scan_scope < SYS_SCAN_SCOPE_MAX);

  return valid;
}
/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_network_list_req_is_valid

==============================================================================*/

boolean cm_network_list_req_is_valid
(
  const cm_network_list_req_s_type* msg_p
)
{
  boolean valid = TRUE;

  if (msg_p)
  {
 
    valid = (boolean)(sys_srv_domain_is_valid ( msg_p->req_service_domain ) &&
#ifndef FEATURE_RAT_PRIORITY_LIST
                      sys_mode_pref_is_valid  ( msg_p->mode_pref )
#else
                      sys_mmss_rat_pri_list_is_valid(&msg_p->rat_pri_list_info)
#endif /*FEATURE_RAT_PRIORITY_LIST*/
            );
  }

  return valid;
}
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_service_req_is_valid

==============================================================================*/

boolean cm_service_req_is_valid
(
  const cm_service_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
#ifdef FEATURE_RAT_PRIORITY_LIST
    if(!(sys_mmss_rat_pri_list_is_valid(&msg_p->rat_pri_list_info))) 
    {
      valid = FALSE;
    }else
#endif /*FEATURE_RAT_PRIORITY_LIST*/    
    if (sys_network_selection_mode_is_valid( msg_p->network_selection_mode ))
    {
      valid = (boolean)(sys_srv_domain_is_valid ( msg_p->req_service_domain )
#ifndef FEATURE_RAT_PRIORITY_LIST
                        && sys_mode_pref_is_valid  ( msg_p->mode_pref )
#endif
                       );

      switch (msg_p->network_selection_mode)
      {
        case (SYS_NETWORK_SELECTION_MODE_AUTOMATIC):
             if (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT)
                {
                    valid = (boolean) (valid &&
                                      sys_radio_access_tech_is_valid ( msg_p->parameters.automatic.rat ) &&
                                      sys_plmn_id_is_valid ( msg_p->parameters.automatic.plmn ));

                }
        case (SYS_NETWORK_SELECTION_MODE_LIMITED):
        case (SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION):
        case (SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY):
#ifdef FEATURE_FEMTO_CSG
        case (SYS_NETWORK_SELECTION_MODE_MANUAL_CSG):
#endif
          break;

        case (SYS_NETWORK_SELECTION_MODE_MANUAL):
        case (SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION):
        case (SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY):
          valid = (boolean)(valid && 
                            reg_cm_manually_selected_plmn_is_valid
                            (
                              msg_p->parameters.manual.selected_plmn_type
                            ) );

          if (valid &&
             (msg_p->parameters.manual.selected_plmn_type ==
              REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN) )
          {
            valid = (boolean)(valid && 
                              sys_plmn_id_is_valid( msg_p->parameters.manual.plmn ));
             if (valid &&
                (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT))
                {
                    valid = (boolean) (valid &&
                                      sys_radio_access_tech_is_valid ( msg_p->parameters.manual.rat ));
                }
          }

          break;

        default:
          valid = FALSE;
          break;
      }
    }
  }
  valid = (boolean)valid && reg_cm_scan_scop_is_valid(msg_p->scan_scope);

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_sim_available_req_is_valid

==============================================================================*/

boolean cm_sim_available_req_is_valid
(
  const cm_sim_available_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
#ifdef FEATURE_DUAL_SIM
    valid = (boolean) ( msg_p->session_type < MMGSDI_MAX_SESSION_TYPE_ENUM);
#else
    valid = TRUE;
#endif
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_sim_not_available_req_is_valid

==============================================================================*/

boolean cm_sim_not_available_req_is_valid
(
  const cm_sim_not_available_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_stop_mode_req_is_valid

==============================================================================*/

boolean cm_stop_mode_req_is_valid
(
  const cm_stop_mode_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = sys_stop_mode_reason_is_valid ( msg_p->stop_mode_reason );
  }

  return valid;
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_ph_status_chgd_req_is_valid

==============================================================================*/
boolean cm_ph_status_chgd_req_is_valid
(
  const cm_ph_status_chgd_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = sys_oprt_mode_is_valid ( msg_p->oprt_mode );
  }

  return valid;
}

#ifdef FEATURE_OOSC_USER_ACTION
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_exit_from_pwr_save_is_valid

==============================================================================*/

boolean cm_exit_from_pwr_save_is_valid
(
  const cm_exit_from_pwr_save_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
#endif

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_hplmn_search_req_is_valid

==============================================================================*/

boolean cm_hplmn_search_req_is_valid
(
  const cm_hplmn_search_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
#endif

#ifdef FEATURE_DUAL_SIM
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_ds_stat_chgd_req_is_valid

==============================================================================*/

boolean cm_ds_stat_chgd_req_is_valid
(
  const cm_ds_stat_chgd_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
#endif

#if defined(FEATURE_DUAL_SIM)
/*==============================================================================

FUNCTION NAME

  cm_ds_tuneaway_status_change_ind_is_valid

==============================================================================*/
boolean cm_ds_tuneaway_status_change_ind_is_valid
(
  const cm_ds_tuneaway_stat_chgd_ind_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}



/*==============================================================================

FUNCTION NAME

  cm_multimode_subs_chgd_req_is_valid

==============================================================================*/
boolean cm_multimode_subs_chgd_req_is_valid
(
  const cm_multimode_subs_chgd_req_s_type* msg_p
)
{
  boolean valid = TRUE;
  boolean multimode_sub_present = FALSE;
  uint8 i;

  for(i = 0; i < MAX_AS_IDS; i++)
  {
    if((msg_p->subs_capability[i] <= SUBS_CAPABILITY_NO_CHANGE) ||
       (msg_p->subs_capability[i] >= SUBS_CAPABILITY_MAX) ||
       ((msg_p->subs_capability[i] == SUBS_CAPABILITY_MULTIMODE) &&
        (multimode_sub_present == TRUE)))
    {
      valid = FALSE;
      break;
    }
    else if(msg_p->subs_capability[i] == SUBS_CAPABILITY_MULTIMODE)
    {
      multimode_sub_present = TRUE;
    
	  reg_multimode_sub = (reg_as_id_e_type)i;
	}
  }
  return (valid && multimode_sub_present);
}

/*==============================================================================

FUNCTION NAME

  cm_dds_switch_ind_is_valid

==============================================================================*/

boolean cm_dds_switch_ind_is_valid
(
  const cm_dds_switch_ind_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
    if(IS_NOT_VALID_SUB_ID(msg_p->target_dds_sub))
    {
      valid = FALSE; 
    }
  }

  return valid;
}


#endif

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_camped_ind_is_valid

==============================================================================*/

boolean cm_camped_ind_is_valid
(
  const cm_camped_ind_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = sys_radio_access_tech_is_valid ( msg_p->active_rat );
  }

  return valid;
}
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_mm_information_ind_is_valid

==============================================================================*/

boolean cm_mm_information_ind_is_valid
(
  const cm_mm_information_ind_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = sys_mm_information_is_valid ( &(msg_p->info) );
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_service_cnf_is_valid

==============================================================================*/

boolean cm_service_cnf_is_valid
(
  const cm_service_cnf_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = sys_plmn_service_state_is_valid ( msg_p->service_state );
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_service_ind_is_valid

==============================================================================*/

boolean cm_service_ind_is_valid
(
  const cm_service_ind_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = sys_plmn_service_state_is_valid ( msg_p->service_state );
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_sim_available_cnf_is_valid

==============================================================================*/

boolean cm_sim_available_cnf_is_valid
(
  const cm_sim_available_cnf_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_sim_not_available_cnf_is_valid

==============================================================================*/

boolean cm_sim_not_available_cnf_is_valid
(
  const cm_sim_not_available_cnf_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_stop_mode_cnf_is_valid

==============================================================================*/

boolean cm_stop_mode_cnf_is_valid
(
  const cm_stop_mode_cnf_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_hplmn_search_cnf_is_valid

==============================================================================*/

boolean cm_hplmn_search_cnf_is_valid
(
  const cm_hplmn_search_cnf_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
#endif

#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  cm_ps_detach_req_is_valid

==============================================================================*/
boolean cm_ps_detach_req_is_valid
(
  const cm_ps_detach_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    if(msg_p->detach_type > SYS_PS_DETACH_TYPE_NONE &&
        msg_p->detach_type < SYS_PS_DETACH_TYPE_MAX)
    {
      valid = TRUE;
    }
  }

  return valid;
}

/*==============================================================================

FUNCTION NAME

  cm_block_plmn_req_is_valid

==============================================================================*/

boolean cm_block_plmn_req_is_valid
(
  const cm_block_plmn_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {  
    valid = (boolean)((msg_p->type == SYS_BLOCK_PLMN_BLOCK) ||
                      (msg_p->type == SYS_BLOCK_PLMN_UNBLOCK) ||
                      (msg_p->type == SYS_BLOCK_PLMN_UNBLOCK_FORCE_PREF)||              
                      (msg_p->type == SYS_BLOCK_PLMN_RESET));
  }

  return valid;
}

/*==============================================================================

FUNCTION NAME

  cm_set_drx_req_is_valid

==============================================================================*/

boolean cm_set_drx_req_is_valid
(
  const cm_set_drx_req_s_type* msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = (boolean) ((msg_p->drx_coefficient == SYS_DRX_NOT_SPECIFIED_BY_MS) ||
                                 ((msg_p->drx_coefficient >= SYS_DRX_CN_COEFFICIENT_S1_CN6_T32) &&
                                 (msg_p->drx_coefficient <= SYS_DRX_CN_COEFFICIENT_S1_CN9_T256)));
  }

  return valid;
}

#endif /*FEATURE_LTE*/

/*==============================================================================

FUNCTION NAME

  cm_set_hplmn_irat_timer_req_is_valid

==============================================================================*/
boolean cm_set_hplmn_irat_timer_req_is_valid
(
  const cm_set_hplmn_irat_search_timer_req_s_type *msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}

#if defined (FEATURE_SGLTE) || defined (FEATURE_1XSRLTE) || defined (FEATURE_SVLTE_DUAL_SIM)
boolean cm_mode_change_ind_is_valid
(
 const cm_ue_mode_ind_s_type *msg_p
)
{
  boolean valid=FALSE;
  if(msg_p)
  {
    if((msg_p->ue_mode>=SYS_UE_MODE_NORMAL) &&
                    (msg_p->ue_mode<=SYS_UE_MODE_1XSRLTE_ONLY))
     {
        valid=TRUE;
     }
  }
  return valid; 
}
#endif

#ifdef FEATURE_DUAL_DATA
boolean cm_data_priority_ind_is_valid
(
 const cm_data_priority_ind_s_type *msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
#endif

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================
FUNCTION NAME:  cm_csg_select_config_update_ind_is_valid

DESCRIPTION
  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE
  boolean:  True if parameters valid otherwise False. 
==============================================================================*/
boolean cm_csg_select_config_update_ind_is_valid
(
 const cm_csg_select_config_update_ind_s_type *msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
#endif


/*==============================================================================
FUNCTION NAME:  cm_psm_ready_req_is_valid

DESCRIPTION
  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE
  boolean:  True if parameters valid otherwise False. 
==============================================================================*/
boolean cm_psm_ready_req_is_valid
(
 const cm_psm_ready_req_s_type *msg_p
)
{
  boolean valid = FALSE;

  if(msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
