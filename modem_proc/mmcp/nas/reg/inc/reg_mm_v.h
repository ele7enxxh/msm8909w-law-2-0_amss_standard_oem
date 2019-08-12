#ifndef REG_MM_V_H
#define REG_MM_V_H
/*==============================================================================


                     R E G  -  M M   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains message definitions for the REG to MM interface.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mm.h_v   1.12   07 Jun 2002 14:22:12   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/inc/reg_mm_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/01   jca     Fixed LINT errors.
05/25/01   sbk     Updated to reflect changes to REG<->MM interface spec.
08/16/01   ab      Renamed mmi_reg.h references to reg_cm.  Renamed MMI to CM.
02/13/02   kwa     CR19136 - Added reg_svc_type to mmr_service_ind.
02/20/02   kwa     Updated interface to support dual-mode design requirements.
05/20/02   kwa     Replaced mode_pref with dual_mode in mmr_reg_req.
05/29/02   kwa     Replaced rat_search_order with rat in mmr_reg_req.
06/05/02   kwa     Added update_equivalent_plmn_list to mmr_reg_cnf.
06/06/02   kwa     Replaced mode_pref with dual_mode and rat in
                   mmr_plmn_search_req.
06/27/02   kwa     Updated mmr_plmn_search_req, mmr_reg_req, mmr_reg_cnf, and
                   mmr_service_ind.
09/19/02   kwa     Added mmr_act_req.
12/31/02   kwa     Added update_equivalent_plmn_list and equivalent_plmn_list
                   to the mmr_service_ind message.
05/22/03   kwa     Modified message verification functions to pass const
                   pointers to eliminate lint warnings. 
08/12/03   kwa     Added MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED.
09/18/03   kwa     Added sys_band_mask_type to cm_act_req_s_type, 
                   cm_network_list_req_s_type, and cm_service_req_s_type.
12/03/03   kwa     Added search_completed parameter to mmr_plmn_search_cnf.
02/04/04   kwa     Added ps_for_request parameter to mmr_reg_req_s_type.
04/06/04   kwa     Added lai_forbidden parameter to mmr_reg_req_s_type and
                   mmr_service_ind_s_type.  This change is featurized using
                   FEATURE_NAS_REMAIN_ON_FORBIDDEN_LAI.
05/12/04   kwa     Added cs_for_request parameter to mmr_reg_req_s_type.
                   Added mmr_camped_ind.
07/19/04   kwa     Removed service_state parameter from mmr_camped_ind.
07/26/04   kwa     Added active_rat and active_band parameters to
                   mmr_camped_ind.
11/04/04   kwa     Added higher_priority_plmn_list parameter to
                   mmr_plmn_search_req.
07/15/05   up      Added function mmr_eq_plmn_change_ind_is_valid and primitive
                   MMR_EQ_PLMN_CHANGE_IND to indicate EPLMN change ind to MM.
10/24/05   up      Added network_selection_mode in mmr_plmn_search_cnf.
                   Kept under FEATURE_WTOW/WTOG_BACKGROUND_PLMN_SEARCH.
10/29/05   ajt     Added data-types to communicate emergency numbers (valid in 
                   the country of currently registered PLMN) from MM to REG.                   
12/07/05   up      Added MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN
                   casue in mmr_cause_e_type.
01/02/06   up      Added message mmr_plmn_search_abort_req_s_type and
                   primitive MMR_PLMN_SEARCH_ABORT_REQ.
01/18/06   up      transaction_id is added in mmr_plmn_search_req/cnf_s_type.
02/23/06   up      Added MMR_CAUSE_FAILURE_REJECT_REMAIN_ON_PLMN
04/06/06   sn      Added MMR_CELL_SERVICE_IND in reg_to_mm_cmd_type
                   Added mmr_cell_service_ind_s_type,
                   mmr_cell_service_ind_is_valid.
07/27/06   sn      Added Uniform OOS changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING
09/20/06   sn      Added MMR_CAUSE_MAX_ATTEMPTED.
10/12/06   sn      Added WCDMA_1X_support.
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
12/15/09   abhi   Removing UOOS code
09/16/11   hc      Added #include "sys_plmn_selection_v.h" to avoid sys related warnings
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "reg_mm.h"
#include "sys_plmn_selection_v.h"




/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_cause_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

RETURN VALUE

  boolean  Flag indicating whether the mmr_cause_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/

extern boolean mmr_cause_is_valid
(
  mmr_cause_e_type                     value
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_plmn_search_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_plmn_search_req_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_plmn_search_req_is_valid
(
  const mmr_plmn_search_req_s_type*          msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_reg_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_reg_req_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_reg_req_is_valid
(
  const mmr_reg_req_s_type*                  msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_sim_available_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_sim_available_req_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_sim_available_req_is_valid
(
  const mmr_sim_available_req_s_type*        msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_sim_not_available_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_sim_not_available_req_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_sim_not_available_req_is_valid
(
  const mmr_sim_not_available_req_s_type*    msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_stop_mode_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_stop_mode_req_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_stop_mode_req_is_valid
(
  const mmr_stop_mode_req_s_type*            msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_ph_status_change_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_ph_status_change_req_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_ph_status_change_req_is_valid
(
  const mmr_ph_status_change_req_s_type*            msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_eq_plmn_change_ind_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_eq_plmn_change_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_eq_plmn_change_ind_is_valid
(
  const mmr_eq_plmn_change_ind_s_type*        msg_p
);

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_camped_ind_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_camped_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_camped_ind_is_valid
(
  mmr_camped_ind_s_type*               msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_mm_information_ind_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_mm_information_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_mm_information_ind_is_valid
(
  const mmr_mm_information_ind_s_type*       msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_plmn_search_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_plmn_search_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_plmn_search_cnf_is_valid
(
  mmr_plmn_search_cnf_s_type*          msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_reg_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_reg_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_reg_cnf_is_valid
(
  mmr_reg_cnf_s_type*                  msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_service_ind_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_service_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_service_ind_is_valid
(
  mmr_service_ind_s_type*              msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_sim_available_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_sim_available_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_sim_available_cnf_is_valid
(
  const mmr_sim_available_cnf_s_type*        msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_sim_not_available_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_sim_not_available_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_sim_not_available_cnf_is_valid
(
  const mmr_sim_not_available_cnf_s_type*    msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_stop_mode_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_stop_mode_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_stop_mode_cnf_is_valid
(
  const mmr_stop_mode_cnf_s_type*            msg_p
);

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_emergency_num_list_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_emergency_num_list_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_emergency_num_list_is_valid
(
  const mmr_emergency_num_list_ind_s_type*            msg_p
);

extern boolean mmr_cell_service_ind_is_valid
(
  const mmr_cell_service_ind_s_type*            msg_p
);

#ifdef FEATURE_HSPA_CALL_STATUS_IND
extern boolean mmr_hspa_call_status_ind_is_valid
(
  const mmr_hspa_call_status_ind_s_type*            msg_p
);
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
extern boolean mmr_conn_mode_service_status_ind_is_valid
(
  const mmr_conn_mode_service_status_ind_s_type*            msg_p
);
#endif

extern boolean mmr_ph_status_change_cnf_is_valid
(
  const mmr_ph_status_change_cnf_s_type*            msg_p
);

extern boolean mmr_reg_reject_ind_is_valid
 (
   const mmr_reg_reject_ind_s_type*            msg_p
 );
#ifdef FEATURE_DUAL_SIM
extern boolean mmr_ds_stat_chgd_cnf_is_valid
(
  const mmr_ds_stat_chgd_cnf_s_type*           msg_p
);
#endif
extern boolean mmr_cell_access_ind_is_valid
(
   const mmr_cell_access_ind_s_type*           msg_p
);

extern boolean mmr_fplmn_list_change_ind_is_valid
(
 const mmr_fplmn_list_change_ind_s_type* msg_p
);

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================
FUNCTION NAME:  mmr_csg_sel_cfg_update_ind_is_valid

DESCRIPTION
  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE
  boolean:  True if parameters valid otherwise False. 
==============================================================================*/
extern boolean mmr_csg_sel_cfg_update_ind_is_valid
(
 const mmr_csg_select_config_update_ind_s_type* msg_p
);
#endif
#ifdef __cplusplus
}
#endif

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  mmr_clear_lai_reject_list_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the mmr_eq_plmn_change_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE).

==============================================================================*/

extern boolean mmr_clear_lai_reject_list_req_is_valid
(
  const mmr_clear_lai_reject_list_req_s_type*        msg_p
);


#if defined (FEATURE_SGLTE) || defined (FEATURE_1XSRLTE)||defined (FEATURE_SVLTE_DUAL_SIM)
extern boolean mmr_ue_mode_change_ind_is_valid
(
  const mm_ue_mode_ind_s_type*    msg_p
);
#endif

#ifdef FEATURE_DUAL_DATA
extern boolean mmr_data_priority_ind_is_valid
(
 const mmr_data_priority_ind_s_type *msg_p
);
#endif


extern boolean mmr_pseudo_list_update_is_valid
(
  const mmr_pseudo_list_update_ind_type*    msg_p
);

/*==============================================================================
FUNCTION NAME:  mmr_psm_ready_req_is_valid

DESCRIPTION
  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE
  boolean:  True if parameters valid otherwise False. 
==============================================================================*/
extern boolean mmr_psm_ready_req_is_valid
(
 const mmr_psm_ready_req_s_type* msg_p
);

#endif
