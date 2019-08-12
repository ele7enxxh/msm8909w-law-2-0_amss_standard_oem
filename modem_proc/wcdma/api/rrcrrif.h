#ifndef RRCRRIF_H
#define RRCRRIF_H
/*===========================================================================
         R R C   -   R R   C O M M A N D    I N T E R F A C E S

DESCRIPTION

  This module contains definitions for commands sent from the GSM RR to
  WCDMA RRC. The commands from the RR are queued into the RRC's
  RR command queue.


Copyright (c) 2000-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rrcrrif.h#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/14   bc      Made changes to add rrc_interrat_cc_order_abort_req_type to rrc_rr_cmd_data_u_type.
11/05/13   rmn     Changes for displaying Partial PLMN Search Results during Manual Search
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
08/31/12   rd      Transfer dedicated priority parameters to/from WCDMA and GERAN
                   during IRAT procedure
07/29/11   ad      Made changes for CMI Phase 4
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11   ad      Added changes to refeaturize the API files
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
08/03/09   sks     Added prototype for rrc_csp_get_gsm_rr_arfcns.
06/15/09   sks     Made changes to support messaging interface with RR for writing 
                   ACQ-DB to NV during power off.
05/08/09   ss      Updated Copyright Information
12/29/08   rm      Added support for 3G<->GAN Handover
12/18/08   sk      Made changes to set the flag ue_in_deep_sleep according to value 
                   sent by RR.
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
27/03/08   pk      Added support for feature FEATURE_IRAT_HO_INFO_COMPRESSED.
09/20/07   vm      Added support for feature FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
06/04/07   pk      Added support for "Partial Roaming Restrictions" under the feature 
                   flag FEATURE SUPPORT FOR PARTIAL ROAMING. For details please see
                   spec CR 151.
06/08/06   vm      Added another prototype of existing function rrc_is_arfcn_valid()
                   under feature flag FEATURE_MCC_AND_850_800_BAND_COMBINATION.
11/08/05   vm      Added support for WTOG Background PLMN Search. 
                   Added new structure rr_interrat_plmn_srch_cnf_type in
                   rrc_rr_cmd_data_u_type.
10/04/05   vr      Removed the feature flag for GTOW PCCO
06/30/05   vr      Support for FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
                   Added rr_interrat_cc_order_cnf and rr_interrat_cc_order_abort_cnf
                   types to rrc_rr_cmd_data_u_type
06/21/05   vm      Added more support for Tbarred feature. Added new enum type
                   rrc_rr_cell_bar_info_e_type and added prototype for new 
                   function rrc_rr_check_cell_barred()
04/04/05   bu      Added support for inter-RAT redirection feature
                   . Added new RRC<->RR command data type: rr_interrat_redirect_rej_type
                     and rr_interrat_redirect_abort_cnf_type.
02/16/05   bu      Added prototype for rrc_write_nv_acq_info().
12/03/04   bu      Added prototype for rrc_set_ue_in_deep_sleep().
11/02/04   bu      Added rrc_interrat_plmn_srch_req_type & 
                   rrc_interrat_plmn_srch_abort_req_type to
                   rrc_rr_cmd_data_u_type.
10/05/04   bu      Added prototypes for rrc_update_band_preference() and
                   rrc_is_arfcn_valid().
03/19/04   kc      Added G to W PCCO support.
10/14/03   kc      Synced up to new interface for get_classmark_info.
05/28/03   Rao     Added W to G PCCO support.
02/18/03   kc      Fixed lint errors and compilation warnings.
02/14/03   kc      Added types for GtoW Handover. Also added declarations for
                   classmark changes.
10/30/02   xfg     Reverted back the previous change upon a request from GSM RR
10/08/02   xfg     Changed interrat_reselection_req to interrat_reselect_req in
                   rrc_rr_cmd_data_u_type.
10/04/02   xfg     Remove rrc_interrat_reselect_rej_type from rrc_rr_cmd_data_u_type.
10/03/02   Rao     Added GSM to WCDMA Reselection Support.
09/20/02   xfg     Added command type rr_interrat_reselect_rej_type for 
                   inter-RAT cell reselection
09/09/02   Rao     Added WCDMA to GSM Reselection Feature Define.
24/07/02   xfg     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "rrc_rr_types.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* RRC RR command header. This is a part of all commands sent from the RR
** to RRC
*/
typedef struct
{
  q_link_type link;
  rrc_rr_cmd_e_type cmd_id;
} rrc_rr_cmd_hdr_type;

/*-------------------------------------------------------------------------*/
/*                 RRC RR COMMAND                                          */
/*-------------------------------------------------------------------------*/
typedef union
{
  rr_interrat_handover_cnf_type        interrat_handover_cnf;
  rr_interrat_handover_abort_cnf_type  interrat_handover_abort_cnf;
  rr_interrat_reselect_rej_type        interrat_reselection_rej;
  rr_interrat_cc_order_rej_type        interrat_cc_order_rej;

  rr_interrat_redirect_rej_type        interrat_redirect_rej;
  rr_interrat_redirect_abort_cnf_type  interrat_redirect_abort_cnf;

  rrc_interrat_reselect_req_type       interrat_reselection_req;

  rrc_interrat_handover_req_type       interrat_handover_req;
  rrc_interrat_handover_abort_req_type interrat_handover_abort_req;
  rrc_nas_data_ind_type                interrat_nas_data_ind;

  rrc_interrat_cc_order_req_type       interrat_cc_order_req;
  rrc_interrat_cc_order_rej_type       interrat_pcco_order_rej;

  rrc_interrat_plmn_srch_req_type      interrat_plmn_srch_req;

  rr_interrat_plmn_srch_cnf_type       interrat_plmn_srch_cnf;

  rrc_interrat_plmn_srch_abort_req_type interrat_plmn_srch_abort_req;

  rrc_interrat_cc_order_abort_req_type  interrat_cc_order_abort_req;

  rr_interrat_cc_order_cnf_type        interrat_cc_order_cnf; 
  rr_interrat_cc_order_abort_cnf_type  interrat_cc_order_abort_cnf;

// Reselection to GAN
  rrc_reselect_to_gan_cnf_type          reselect_to_gan_cnf;
  rrc_reselect_to_gan_rej_type          reselect_to_gan_rej;

// Service related
  rr_gan_activation_ind_type          gan_activation_ind;
  rr_gan_mode_ind_type          gan_mode_ind;
  rrc_gan_cell_info_cnf_type          gan_cell_info_cnf;

  rrc_reselect_from_gan_req_type reselect_from_gan_req;
  rrc_reselect_from_gan_rej_type reselect_from_gan_rej;
  rrc_go_to_idle_detached_req_type rrc_go_to_idle_detached_req;
  rrc_gan_wake_up_req_type rrc_gan_wake_up_req;
  rrc_gan_act_req_type rrc_gan_act_req;
  rrc_gan_act_rej_type rrc_gan_act_rej;
  rrc_imr_from_gan_req_type rrc_imr_from_gan_req;

  rrc_interrat_redirect_req_type      interrat_redirect_req;

  rrc_rr_stop_wcdma_req_type rrc_rr_stop_wcdma_req;
  rrc_rr_stop_wcdma_cnf_type rrc_rr_stop_wcdma_cnf;
  rr_rrc_stop_gsm_cnf_type rr_rrc_stop_gsm_cnf;
  rr_rrc_stop_gsm_req_type rr_rrc_stop_gsm_req;
  rr_dedicated_priorities_rsp_type dedicated_priorities_rsp;
  rrc_dedicated_priorities_req_type        dedicated_priorities_req;
  rrc_clear_dedicated_priorities_ind_type  clear_dedicated_priorities_ind;
  rrc_interrat_reselection_abort_req_type  interrat_reselection_abort_req;
  rr_interrat_reselection_abort_cnf_type   interrat_reselection_abort_cnf;
  rr_interrat_plmn_srch_abort_cnf_type     interrat_plmn_srch_abort_cnf;
  rr_cipher_sync_ind_type                  cipher_sync_ind;
  rrc_interrat_redirect_abort_req_type     interrat_redirect_abort_req;
} rrc_rr_cmd_data_u_type;

typedef struct
{
  /* RRC RR command header */
  rrc_rr_cmd_hdr_type cmd_hdr;

  /* RRC RR command data */
  rrc_rr_cmd_data_u_type cmd;
} rrc_rr_cmd_type;

/* This is used to inform RR if the WCDMA cell is barred, or 
 * WCDMA frequency is barred, or the active set is barred
 */
typedef enum
{
  WCDMA_NOT_BARRED,
  WCDMA_PSC_BARRED,
  WCDMA_FREQ_BARRED
}rrc_rr_cell_bar_info_e_type;



/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*===========================================================================

FUNCTION RRC_GET_CLASSMARK_CHANGE_HANDOVER_INFO_SUB_ID

DESCRIPTION

  This function builds a INTERRAT HANDOVER INFO message type which will be
  sent by GSM to UTRAN thru GERAN.  An ASN1 encoded message and message
  length is populated by RRC, the message and msg_length is used by RR to
  send this as part of Classmark Change to UTRAN via GERAN
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/
void rrc_get_classmark_change_handover_info_sub_id
(
  byte ** msg_ptr,  /* The command to be sent to UTRAN */
  uint8 * msg_len,  /* Length of the message */
  boolean irat_ho_info_compressed,
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION RRC_GET_CLASSMARK_CHANGE_HANDOVER_INFO

DESCRIPTION

  This function builds a INTERRAT HANDOVER INFO message type which will be
  sent by GSM to UTRAN thru GERAN.  An ASN1 encoded message and message
  length is populated by RRC, the message and msg_length is used by RR to
  send this as part of Classmark Change to UTRAN via GERAN
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/
void rrc_get_classmark_change_handover_info
(
  byte ** msg_ptr,    /* The command to be sent to UTRAN */
  uint8 * msg_len,    /* Length of the message           */
  boolean irat_ho_info_compressed
);

/*===========================================================================

FUNCTION RRC_DELETE_CLASSMARK_CHANGE_HANDOVER_INFO_SUB_ID

DESCRIPTION

  This function deletes the CLASSMARK_CHANGE_HANDOVER_INFO asn1 type
  which was previously allocated to send to UTRAN via GERAN.
  RR calls this function after it has copied the contents of the
  Handover To Utran Info contents locally.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_delete_classmark_change_handover_info_sub_id(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION RRC_DELETE_CLASSMARK_CHANGE_HANDOVER_INFO

DESCRIPTION

  This function deletes the CLASSMARK_CHANGE_HANDOVER_INFO asn1 type
  which was previously allocated to send to UTRAN via GERAN.
  RR calls this function after it has copied the contents of the
  Handover To Utran Info contents locally.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_delete_classmark_change_handover_info(void);

/*===========================================================================

FUNCTION RRC_GET_RR_CMD_BUF

DESCRIPTION

  This function returns a RR command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_rr_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_rr_cmd_type *rrc_get_rr_cmd_buf( void );



/*===========================================================================

FUNCTION RRC_PUT_RR_CMD

DESCRIPTION

  This function puts a command buffer into RRC's RR
  command queue. Note that the command buffer must have been
  requested through a call to rrc_get_rr_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_rr_cmd
(
  rrc_rr_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);


/*===========================================================================

FUNCTION RRC_UPDATE_BAND_PREFERENCE_SUB_ID

DESCRIPTION

  This function is called by RR to update the band preference
  in RRC.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
===========================================================================*/
void rrc_update_band_preference_sub_id
(
  sys_band_mask_type band_pref_mask,  /* band mask */
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================

FUNCTION RRC_UPDATE_BAND_PREFERENCE

DESCRIPTION

  This function is called by RR to update the band preference
  in RRC.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
===========================================================================*/
void rrc_update_band_preference
(
  sys_band_mask_type band_pref_mask  /* band mask */
);


/*===========================================================================

FUNCTION RRC_IS_ARFCN_VALID_SUB_ID

DESCRIPTION

  This function is called by RR to validate the WCDMA ARFCN it
  reads from the SIs and the PLMN the UE is currently camped on. 
  RRC validates the range and band of the ARFCN and plmn combination.
  This function is called with plmn_id as argument only when the 
  feature FEATURE_MCC_AND_850_800_BAND_COMBINATION is enabled, otherwise
  the function is called without passing plmn_id as an argument and for
  this there is a separate prototype.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If ARFCN and PLMN_id combination is valid,
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_arfcn_valid_sub_id
(
  uint32 UARFCN,
  PLMN_id_T plmn_id,
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION RRC_IS_ARFCN_VALID

DESCRIPTION

  This function is called by RR to validate the WCDMA ARFCN it
  reads from the SIs and the PLMN the UE is currently camped on. 
  RRC validates the range and band of the ARFCN and plmn combination.
  This function is called with plmn_id as argument only when the 
  feature FEATURE_MCC_AND_850_800_BAND_COMBINATION is enabled, otherwise
  the function is called without passing plmn_id as an argument and for
  this there is a separate prototype.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If ARFCN and PLMN_id combination is valid,
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_arfcn_valid
(
  uint32 UARFCN,
  PLMN_id_T plmn_id
);

/*===========================================================================

FUNCTION RRC_SET_UE_IN_DEEP_SLEEP_SUB_ID

DESCRIPTION

  This function is called by RR to inform RRC that UE is going to DEEP SLEEP
  state.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_set_ue_in_deep_sleep_sub_id( boolean value,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION RRC_SET_UE_IN_DEEP_SLEEP

DESCRIPTION

  This function is called by RR to inform RRC that UE is going to DEEP SLEEP
  state.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_set_ue_in_deep_sleep( boolean value);

/*===========================================================================

FUNCTION RRC_WRITE_NV_ACQ_INFO

DESCRIPTION

  This function is called by RR when it gets RR_DEACT_REQ from MM,
  so that RRC can write the acquisition database information to NV.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_write_nv_acq_info( void );


/*===========================================================================

FUNCTION: RRC_RR_CHECK_CELL_BARRED_SUB_ID

DESCRIPTION:
  This function is called by RR to check if a particular WCDMA cell is 
  barred or not

DEPENDENCIES:
  NONE

RETURN VALUE:
  WCDMA cell is not barred, or only the PSC is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
rrc_rr_cell_bar_info_e_type rrc_rr_check_cell_barred_sub_id
(
  uint16 freq, uint16 scr_code, sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION: RRC_RR_CHECK_CELL_BARRED

DESCRIPTION:
  This function is called by RR to check if a particular WCDMA cell is 
  barred or not

DEPENDENCIES:
  NONE

RETURN VALUE:
  WCDMA cell is not barred, or only the PSC is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
rrc_rr_cell_bar_info_e_type rrc_rr_check_cell_barred
(
  uint16 freq, uint16 scr_code
);

/*===========================================================================

FUNCTION: rrc_rr_unbar_flai_cells

DESCRIPTION:
           This function is called by RR upon the initiation of any cell selection, to unbar all the barred frequencies 
           because of partial roaming restrictions

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
=============================================================================*/
void rrc_rr_unbar_flai_cells(void);

/*===========================================================================

FUNCTION: RR_RRC_GET_ACQ_CHNL_INFO_SUB_ID

DESCRIPTION:
          This functin is called by GSM-RR to get WCDMA channel info. GSM-RR
          uses WCDMA channel info to eliminate GSM channels for faster scanning

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rr_get_rrc_acq_chnl_info_sub_id(rrc_rr_wcdma_acq_chnl_info_type *acq_chnl_info,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION: RR_RRC_GET_ACQ_CHNL_INFO

DESCRIPTION:
          This functin is called by GSM-RR to get WCDMA channel info. GSM-RR
          uses WCDMA channel info to eliminate GSM channels for faster scanning

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rr_get_rrc_acq_chnl_info(rrc_rr_wcdma_acq_chnl_info_type *acq_chnl_info);



/*===========================================================================

FUNCTION: RRC_CLEAR_ACQ_CHNL_INFO_SUB_ID

DESCRIPTION:
          This function is called by both RRC and GSM-RR to clear
          WCDMA Acq channel info so that no stale information is used
          later on during frequency scanning. This function is called 
          when UE camps or RRC/RR receive service request, plmn_list_req, etc.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrc_clear_acq_chnl_info_sub_id(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION: RRC_CLEAR_ACQ_CHNL_INFO

DESCRIPTION:
          This function is called by both RRC and GSM-RR to clear
          WCDMA Acq channel info so that no stale information is used
          later on during frequency scanning. This function is called 
          when UE camps or RRC/RR receive service request, plmn_list_req, etc.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrc_clear_acq_chnl_info(void);

/*===========================================================================

FUNCTION: RRC_CSP_GET_GSM_RR_ARFCNS

DESCRIPTION:
          This function is called by both RRC to get the 
          GSM-RR ARFCNS.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrc_csp_get_gsm_rr_arfcns(void);

/*Funtcion called by RRC, to check if a ARFCN is for GAN cell*/

/*===========================================================================

FUNCTION: RR_RRC_IS_GAN_CELL

DESCRIPTION:
          This function is called by both RRC to check if the ARFCN is for the GAN cell

DEPENDENCIES: NONE

RETURN VALUE: TRUE if the ARFCN is for GAN cell
                      FALSE otherwise

SIDE EFFECTS: NONE
=============================================================================*/
boolean rr_rrc_is_gan_cell(uint16 ARFCN);
/*===========================================================================

FUNCTION          wrrc_get_inter_rat_bplmn_prtl_result

DESCRIPTION       RR Calls this API to get Partial PLMN results from WRRC during ongoing GtoW
                  Manual PLMN Search

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean wrrc_get_inter_rat_bplmn_prtl_result(sys_detailed_plmn_list_s_type *available_plmn_list);

/*===========================================================================

FUNCTION          wrrc_get_inter_rat_bplmn_prtl_result

DESCRIPTION       RR Calls this API to get Partial PLMN results from WRRC during ongoing GtoW
                  Manual PLMN Search

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean wrrc_get_inter_rat_bplmn_prtl_result_sub_id(sys_detailed_plmn_list_s_type *available_plmn_list, sys_modem_as_id_e_type as_id);
/*===========================================================================

FUNCTION          rr_get_inter_rat_bplmn_prtl_result

DESCRIPTION       WRRC Calls this API to get Partial PLMN results from RR during ongoing WtoG
                  Manual PLMN Search

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean rr_get_inter_rat_bplmn_prtl_result(sys_detailed_plmn_list_s_type *available_plmn_list);

/*===========================================================================

FUNCTION          rr_get_inter_rat_bplmn_prtl_result_sub_id

DESCRIPTION       WRRC Calls this API to get Partial PLMN results from RR during ongoing WtoG
                  Manual PLMN Search

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean rr_get_inter_rat_bplmn_prtl_result_sub_id(sys_detailed_plmn_list_s_type *available_plmn_list, sys_modem_as_id_e_type as_id);

#endif /* RRCRRIF_H */
