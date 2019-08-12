#ifndef REG_CM_V_H
#define REG_CM_V_H
/*==============================================================================


                     R E G  -  C M   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains message definitions for the REG and CM interface.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_cm.h_v   1.3   10 May 2002 15:12:18   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/inc/reg_cm_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/25/01   jca     Fixed Lint errors.
05/23/01   sbk     Updated to reflect changes to REG<->MM interface.
08/16/01   ab      Renamed mmi_reg.h to cm_reg.h.
08/22/01   TS      Removed enums of inter-task commands going to Call Manager.
                   These are now located in cmll.h.
02/07/02   kwa     Updated interface to support dual mode design requirements.
09/19/02   kwa     Added cm_act_req.
10/18/02   kwa     Added cm_deep_sleep_ind.
05/22/03   kwa     Modified message verification functions to pass const
                   pointers to eliminate lint warnings. 
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added sys_band_mask_type to cm_act_req_s_type, 
                   cm_network_list_req_s_type, and cm_service_req_s_type.
09/18/03   kwa     Replaced user_selected_plmn in cm_service_req_manual_s_type
                   with selected_plmn_type to allow CM to initiate a manual
                   search for the HPLMN in addition to the RPLMN and a user
                   specified PLMN.  Change was featurized using
                   FEATURE_MANUAL_HPLMN_SELECTION.
11/07/03   kwa     Removed support for REG_CM_MANUALLY_SELECTED_RPLMN.  The
                   capability for CM to request that REG search for the RPLMN
                   at power up in manual network selection mode is provided by
                   sending an undefined PLMN along with
                   REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN.
02/04/04   kwa     Added ps_for_request parameter to cm_service_req_s_type.
05/06/04   kwa     Added cs_for_request parameter to cm_service_req_s_type.
                   Added cm_camped_ind.
07/19/04   kwa     Removed service_state parameter from cm_camped_ind.
07/28/04   kwa     Added CM_REG_CMD_NONE to cm_to_reg_cmd_type.
                   Added active_rat and active_band parameters to cm_camped_ind.
01/11/05   up      Mainlining FEATURE_MANUAL_HPLMN_SELECTION.
03/31/05   hj      Added support for CM_CIPHER_IND. 
04/12/05   hj      Reverting enum type for cipher_domain in cm_cipher_ind_s_type
                   to sys_srv_domain_e_type.
09/19/05   up      Added cm_hplmn_search_req_s_type, cm_hplmn_search_cnf_s_type
                   and CM_HPLMN_SEARCH_REQ to support on demand HPLMN Search.
10/29/05   ajt     Added data-types to send emergency numbers (valid in this country)
                   to CM.                   
12/06/05   up      Added status field in cm_network_list_cnf_s_type to indicate
                   the abort of manual plmn search.
                   Added new enum reg_cm_network_list_status_e_type.
04/06/06   sn      Added cm_cell_service_ind_s_type
07/27/06   sn      Added Uniform OOS changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH 
12/15/09   abhi  Removing UOOS code
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/
#include "mmcp_variation.h"
#include "sys_plmn_selection_v.h"
#include "sys_stru_v.h"
#include "reg_cm.h"


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_network_list_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_network_list_req_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_network_list_req_is_valid
(
  const cm_network_list_req_s_type* msg_p
);

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_service_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_service_req_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_service_req_is_valid
(
  const cm_service_req_s_type* msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_sim_available_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_sim_available_req_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_sim_available_req_is_valid
(
  const cm_sim_available_req_s_type* msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_sim_not_available_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_sim_not_available_req_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_sim_not_available_req_is_valid
(
  const cm_sim_not_available_req_s_type* msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_stop_mode_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_stop_mode_req_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_stop_mode_req_is_valid
(
  const cm_stop_mode_req_s_type* msg_p
);

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_ph_status_chgd_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_ph_status_chgd_req_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_ph_status_chgd_req_is_valid
(
  const cm_ph_status_chgd_req_s_type* msg_p
);

#ifdef FEATURE_OOSC_USER_ACTION
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_exit_from_pwr_save_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_exit_from_pwr_save_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_exit_from_pwr_save_is_valid
(
  const cm_exit_from_pwr_save_s_type* msg_p
);
#endif

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_hplmn_search_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_hplmn_search_req_is_valid
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

boolean cm_hplmn_search_req_is_valid
(
  const cm_hplmn_search_req_s_type* msg_p
);
#endif

#ifdef FEATURE_DUAL_SIM 
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_ds_stat_chgd_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_ds_stat_chgd_req_is_valid
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

boolean cm_ds_stat_chgd_req_is_valid
(
  const cm_ds_stat_chgd_req_s_type* msg_p
);
#endif

#if defined(FEATURE_DUAL_SIM)
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_ds_tuneaway_status_change_ind_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_ds_tuneaway_status_change_ind_is_valid
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/
boolean cm_ds_tuneaway_status_change_ind_is_valid
(
  const cm_ds_tuneaway_stat_chgd_ind_s_type* msg_p
);

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_multimode_subs_chgd_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_multimode_subs_chgd_req is valid
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/
boolean cm_multimode_subs_chgd_req_is_valid
(
  const cm_multimode_subs_chgd_req_s_type* msg_p
);

extern boolean cm_dds_switch_ind_is_valid
(
  const cm_dds_switch_ind_s_type* msg_p
);

#endif

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_camped_ind_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_camped_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_camped_ind_is_valid
(
  const cm_camped_ind_s_type* msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_mm_information_ind_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_mm_information_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_mm_information_ind_is_valid
(
  const cm_mm_information_ind_s_type* msg_p
);

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_service_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_service_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_service_cnf_is_valid
(
  const cm_service_cnf_s_type* msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_service_ind_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_service_ind_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_service_ind_is_valid
(
  const cm_service_ind_s_type* msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_sim_available_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_sim_available_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_sim_available_cnf_is_valid
(
  const cm_sim_available_cnf_s_type* msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_sim_not_available_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_sim_not_available_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_sim_not_available_cnf_is_valid
(
  const cm_sim_not_available_cnf_s_type* msg_p
);


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_stop_mode_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_stop_mode_cnf_s_type
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

extern boolean cm_stop_mode_cnf_is_valid
(
  const cm_stop_mode_cnf_s_type* msg_p
);
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  cm_hplmn_search_cnf_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_hplmn_search_cnf_is_valid
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

boolean cm_hplmn_search_cnf_is_valid
(
  const cm_hplmn_search_cnf_s_type* msg_p
);
#endif

#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  cm_ps_detach_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_ps_detach_req_is_valid
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/
boolean cm_ps_detach_req_is_valid
(
  const cm_ps_detach_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  cm_block_plmn_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the CM_BLOCK_PLMN_REQ
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/

boolean cm_block_plmn_req_is_valid
(
  const cm_block_plmn_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  cm_set_drx_req_is_valid

DESCRIPTION

  Function that verifies that the input message structure contains valid data.
  A message is considered valid if all of its parameters are valid.

RETURN VALUE

  boolean  Flag indicating whether the cm_ps_detach_req_is_valid
           contains valid data (TRUE) or invalid data (FALSE). 

==============================================================================*/
boolean cm_set_drx_req_is_valid
(
  const cm_set_drx_req_s_type* msg_p
);

#endif /*FEATURE_LTE*/


/*==============================================================================

FUNCTION NAME

  cm_set_hplmn_irat_timer_req_is_valid

==============================================================================*/
extern boolean cm_set_hplmn_irat_timer_req_is_valid
(
  const cm_set_hplmn_irat_search_timer_req_s_type *msg_p
);

#if defined (FEATURE_SGLTE) || defined (FEATURE_1XSRLTE)|| defined FEATURE_SVLTE_DUAL_SIM
extern boolean cm_mode_change_ind_is_valid
(
 const cm_ue_mode_ind_s_type *msg_p
);
#endif

#ifdef FEATURE_DUAL_DATA
extern boolean cm_data_priority_ind_is_valid
(
 const cm_data_priority_ind_s_type *msg_p
);
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
extern boolean cm_csg_select_config_update_ind_is_valid
(
 const cm_csg_select_config_update_ind_s_type *msg_p
);
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
);

#ifdef __cplusplus
}
#endif


#endif
