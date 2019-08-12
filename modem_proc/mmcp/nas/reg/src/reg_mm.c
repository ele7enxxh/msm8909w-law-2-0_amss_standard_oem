/*==============================================================================
                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mm.c_v   1.3   07 Jun 2002 14:22:46   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_mm.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/07/02   kwa     Initial version.
05/20/02   kwa     Removed mode_pref from validity checking of mmr_reg_req.
05/29/02   kwa     Replaced rat_search_order with rat in mmr_reg_req.
06/06/02   kwa     Replaced mode_pref with dual_mode and rat in
                   mmr_plmn_search_req.
06/27/02   kwa     Updated functions for checking the validity of the
                   mmr_plmn_search_req, mmr_reg_req, mmr_reg_cnf, and
                   mmr_service_ind.
09/19/02   kwa     Added mmr_act_req.
12/31/02   kwa     Added update_equivalent_plmn_list and equivalent_plmn_list
                   to the mmr_service_ind message.
05/22/03   kwa     Modified call to sys_mm_information_is_valid to pass a
                   pointer rather than a structure.  Modified message
                   verification functions to pass const pointers to eliminate
                   lint warnings. 
05/12/04   kwa     Added mmr_camped_ind.
07/19/04   kwa     Removed service_state parameter from mmr_camped_ind.
07/26/04   kwa     Added active_rat and active_band parameters to
                   mmr_camped_ind.
11/04/04   kwa     Added higher_priority_plmn_list parameter to
                   mmr_plmn_search_req.
07/15/05   up      Added mmr_eq_plmn_change_ind_is_valid.
11/07/05   ajt     Added function mmr_emergency_num_list_is_valid.
01/18/06   up      Updated mmr_plmn_search_req_is_valid to validate trans ID.
04/06/06   sn      Added mmr_cell_service_ind_is_valid.
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "reg_mm_v.h"

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

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_cause_is_valid

==============================================================================*/

boolean mmr_cause_is_valid
(
  mmr_cause_e_type                     value
)
{
  boolean valid = FALSE;

  valid = (boolean)(value > MMR_CAUSE_NONE &&
                    value < MMR_CAUSE_MAX);

  return valid;
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_plmn_search_req_is_valid

==============================================================================*/

boolean mmr_plmn_search_req_is_valid
(
  const mmr_plmn_search_req_s_type*          msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = (boolean)(sys_network_selection_mode_is_valid ( msg_p->network_selection_mode ) &&
                      (msg_p->transaction_id < 0x10) &&
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
                      sys_radio_access_tech_is_valid      ( msg_p->rat ) &&
                      sys_plmn_rat_list_is_valid          ( &(msg_p->higher_priority_plmn_list) ) );
#else
                      sys_radio_access_tech_is_valid      ( msg_p->rat ) );
#endif
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_reg_req_is_valid

==============================================================================*/

boolean mmr_reg_req_is_valid
(
  const mmr_reg_req_s_type*                  msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = (boolean)(sys_network_selection_mode_is_valid ( msg_p->network_selection_mode ) &&
                      sys_plmn_id_is_valid                ( msg_p->plmn ) &&
                      sys_srv_domain_is_valid             ( msg_p->req_service_domain ) &&
                      sys_radio_access_tech_is_valid      ( msg_p->rat ) );
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_sim_available_req_is_valid

==============================================================================*/

boolean mmr_sim_available_req_is_valid
(
  const mmr_sim_available_req_s_type*        msg_p
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

  mmr_sim_not_available_req_is_valid

==============================================================================*/

boolean mmr_sim_not_available_req_is_valid
(
  const mmr_sim_not_available_req_s_type*    msg_p
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

  mmr_stop_mode_req_is_valid

==============================================================================*/

boolean mmr_stop_mode_req_is_valid
(
  const mmr_stop_mode_req_s_type*            msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = sys_stop_mode_reason_is_valid (msg_p->stop_mode_reason);
            
  }

  return valid;
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_ph_status_change_req_is_valid

==============================================================================*/

boolean mmr_ph_status_change_req_is_valid
(
  const mmr_ph_status_change_req_s_type*            msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = sys_stop_mode_reason_is_valid (msg_p->stop_mode_reason);
            
  }

  return valid;
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_eq_plmn_change_ind_is_valid

==============================================================================*/

boolean mmr_eq_plmn_change_ind_is_valid
(
  const mmr_eq_plmn_change_ind_s_type*        msg_p
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

  mmr_camped_ind_is_valid

==============================================================================*/

boolean mmr_camped_ind_is_valid
(
  mmr_camped_ind_s_type*               msg_p
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

  mmr_mm_information_ind_is_valid

==============================================================================*/

boolean mmr_mm_information_ind_is_valid
(
  const mmr_mm_information_ind_s_type*       msg_p
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

  mmr_plmn_search_cnf_is_valid

==============================================================================*/

boolean mmr_plmn_search_cnf_is_valid
(
  mmr_plmn_search_cnf_s_type*          msg_p
)
{
  boolean valid = TRUE;
  if(!msg_p)
  {
    valid = FALSE;
  }
  else if ((msg_p) && (msg_p->service_search!=SYS_SERVICE_SEARCH_PCI_PLMN))
  {
    valid = sys_detailed_plmn_list_is_valid ( &(msg_p->found_plmn_list.plmn_list) );
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_reg_cnf_is_valid

==============================================================================*/

boolean mmr_reg_cnf_is_valid
(
  mmr_reg_cnf_s_type*                  msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = (boolean)( mmr_cause_is_valid              ( msg_p->cause ) &&
                       sys_plmn_service_state_is_valid ( msg_p->service_state ) &&
                       sys_detailed_plmn_list_is_valid ( &(msg_p->available_plmn_list) )  &&
                       sys_plmn_list_is_valid          ( &(msg_p->equivalent_plmn_list) ) );
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_service_ind_is_valid

==============================================================================*/

boolean mmr_service_ind_is_valid
(
  mmr_service_ind_s_type*              msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = (boolean)( mmr_cause_is_valid              ( msg_p->cause ) &&
                       sys_plmn_service_state_is_valid ( msg_p->service_state ) &&
                       sys_plmn_list_is_valid          ( &(msg_p->equivalent_plmn_list) ) );
  }

  return valid;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_sim_available_cnf_is_valid

==============================================================================*/

boolean mmr_sim_available_cnf_is_valid
(
  const mmr_sim_available_cnf_s_type*        msg_p
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

  mmr_sim_not_available_cnf_is_valid

==============================================================================*/

boolean mmr_sim_not_available_cnf_is_valid
(
  const mmr_sim_not_available_cnf_s_type*    msg_p
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

  mmr_stop_mode_cnf_is_valid

==============================================================================*/

boolean mmr_stop_mode_cnf_is_valid
(
  const mmr_stop_mode_cnf_s_type*            msg_p
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

  mmr_emergency_num_list_is_valid

==============================================================================*/

boolean mmr_emergency_num_list_is_valid
(
  const mmr_emergency_num_list_ind_s_type*            msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}

boolean mmr_cell_service_ind_is_valid
(
  const mmr_cell_service_ind_s_type*            msg_p
)
{
  boolean valid = FALSE;

  if(msg_p)
  {
    valid = (boolean)((msg_p->hsdpa_hsupa_support > SYS_HS_IND_NONE && msg_p->hsdpa_hsupa_support < SYS_HS_IND_MAX)
                    &&(msg_p->dtm_support > SYS_DTM_SUPPORT_NONE && msg_p->dtm_support < SYS_DTM_SUPPORT_MAX) 
                    &&(msg_p->egprs_support > SYS_EGPRS_SUPPORT_NONE && msg_p->egprs_support < SYS_EGPRS_SUPPORT_MAX));
  }

  return valid;
}

#ifdef FEATURE_HSPA_CALL_STATUS_IND
boolean mmr_hspa_call_status_ind_is_valid
(
  const mmr_hspa_call_status_ind_s_type*            msg_p
)
{
  boolean valid = FALSE;

  if(msg_p)
  {
    valid = (boolean)((msg_p->hs_call_status_ind > SYS_HS_IND_NONE)  && (msg_p->hs_call_status_ind < SYS_HS_IND_MAX));
  }

  return valid;
}
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
boolean mmr_conn_mode_service_status_ind_is_valid
(
  const mmr_conn_mode_service_status_ind_s_type*            msg_p
)
{
  boolean valid = FALSE;

  if(msg_p)
  {
    valid = (boolean)(((msg_p->conn_mode_ss_ind.service_status > SYS_SRV_STATUS_NONE)  && (msg_p->conn_mode_ss_ind.service_status < SYS_SRV_STATUS_MAX))
                    &&(sys_plmn_id_is_valid(msg_p->conn_mode_ss_ind.selected_plmn))
                    &&((msg_p->conn_mode_ss_ind.rat > SYS_RAT_NONE) && (msg_p->conn_mode_ss_ind.rat < SYS_RAT_MAX))
                    &&((msg_p->conn_mode_ss_ind.signal_quality  > SYS_SIGNAL_QUALITY_NONE) && (msg_p->conn_mode_ss_ind.signal_quality < SYS_SIGNAL_QUALITY_MAX)));
  }

  return valid;
}
#endif

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_ph_status_change_cnf_is_valid

==============================================================================*/

boolean mmr_ph_status_change_cnf_is_valid
(
  const mmr_ph_status_change_cnf_s_type*            msg_p
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

  mmr_reg_reject_ind_is_valid

==============================================================================*/

boolean mmr_reg_reject_ind_is_valid
(
  const mmr_reg_reject_ind_s_type*            msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}

#ifdef FEATURE_DUAL_SIM
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_ds_stat_chgd_cnf_is_valid

==============================================================================*/
boolean mmr_ds_stat_chgd_cnf_is_valid
(
  const mmr_ds_stat_chgd_cnf_s_type*            msg_p
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

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_cell_access_ind_is_valid

==============================================================================*/
boolean mmr_cell_access_ind_is_valid
(
 const mmr_cell_access_ind_s_type* msg_p
)
{
  boolean valid = FALSE;
  if(msg_p)
  {
   valid = TRUE;
  }
  return valid;
}

#ifdef FEATURE_LTE
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_fplmn_list_change_ind_is_valid

==============================================================================*/
boolean mmr_fplmn_list_change_ind_is_valid
(
 const mmr_fplmn_list_change_ind_s_type* msg_p
)
{
  boolean valid = FALSE;
  if(msg_p)
  {
   valid = (boolean) (msg_p->list_type > SYS_FORBIDDEN_LIST_TYPE_NONE && msg_p->list_type < SYS_FORBIDDEN_LIST_TYPE_MAX);
  }
  return valid;
}
#endif 

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_clear_lai_reject_list_req_is_valid

==============================================================================*/

boolean mmr_clear_lai_reject_list_req_is_valid
(
  const mmr_clear_lai_reject_list_req_s_type*        msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    valid = TRUE;
  }

  return valid;
}

#if defined (FEATURE_SGLTE) || defined (FEATURE_1XSRLTE)|| defined (FEATURE_SVLTE_DUAL_SIM)
boolean mmr_ue_mode_change_ind_is_valid
(
  const mm_ue_mode_ind_s_type*    msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    if ( (msg_p->ue_mode>= SYS_UE_MODE_NORMAL ) 
            && (msg_p->ue_mode<= SYS_UE_MODE_1XSRLTE_ONLY) )
     {
       valid = TRUE;
     }
  }

  return valid;
}
#endif

#ifdef FEATURE_DUAL_DATA
boolean mmr_data_priority_ind_is_valid
(
 const mmr_data_priority_ind_s_type *msg_p
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

boolean mmr_pseudo_list_update_is_valid
(
  const mmr_pseudo_list_update_ind_type*    msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    if ( (msg_p->rat> SYS_RAT_NONE) 
            && (msg_p->rat< SYS_RAT_MAX) )
     {
       valid = TRUE;
     }
  }

  return valid;
}

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================
FUNCTION NAME:  mmr_csg_sel_cfg_update_ind_is_valid

DESCRIPTION
  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE
  boolean:  True if parameters valid otherwise False. 
==============================================================================*/

boolean mmr_csg_sel_cfg_update_ind_is_valid
(
  const mmr_csg_select_config_update_ind_s_type*  msg_p
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
FUNCTION NAME:  mmr_psm_ready_req_is_valid

DESCRIPTION
  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE
  boolean:  True if parameters valid otherwise False. 
==============================================================================*/

boolean mmr_psm_ready_req_is_valid
(
  const mmr_psm_ready_req_s_type*  msg_p
)
{
  boolean valid = FALSE;

  if(msg_p)
  {
    valid = TRUE;
  }

  return valid;
}
