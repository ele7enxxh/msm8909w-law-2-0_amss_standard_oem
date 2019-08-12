#ifndef CMWLL_H
#define CMWLL_H
/*===========================================================================

              C A L L   M A N A G E R   L O W E R   L A Y E R

                       H E A D E R   F I L E

DESCRIPTION
  This header file contains all the message definitions necessary for
  Lower Layers to interface with the Call Manager module.

Copyright (c) 2001 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cmwll.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/30/13   skk     Enhance Interface For Packet Attach/Detach
04/11/13   xs      FRAT support
09/19/12   skk     Added support for "ESM_SM_backoff timer" requirements
03/01/12   fj      Added support for DRX.
04/30/12   xs      Add support of sending FPLMN list
04/04/12   xs      Codec info propagation
04/02/12   gm      Remove unnecessary TMC references.
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/13/12   vk      Removal of feature flags added for CELL_DCH feature.
03/02/12   vk      Added feature to get DCH cell info.
01/24/12   fj      Add support for EUTRA detection status.
01/11/12   xs      ifdef removal for api folder
12/21/11   xs      L->W SRVCC support
11/23/11   rm      Removing GSDI related structures
10/25/11   vk      Changes related to manual search rejection in RLF
05/02/11   rm      Cell broadcast changes
03/11/11   rm      GSDI clean-up
02/07/11   rm      CMI-4: Removing #ifdef's
01/12/11   fj      Added bearer_ctxt_transfer_ind to the cmd union.
06/21/10   rk      Redesign of WRLF feature
02/16/10   sg      CM to send handover complete info to CM client (eCall feature requirement)
01/08/10   ay      Added cm_suitable_search_end_ind_s_type to the cmd union
09/25/09   ks      Adding changes for Nw Initiated QOS
12/22/08   fj      Separate header files included for GSM/WCDMA or GSM/WCDMA/LTE.
03/23/09   rm      Adding _v.h files from NAS
02/27/09   rm      Adding header files of other MOBs
02/25/09   aj      UMA GAN related changes
11/21/08   am      Fixing compilation error for Running off-target tests
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
09/18/08   aj      Add support for UMA-GAN feature
07/24/08   rn      Send regn reject ind to clients
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_PH_STAT_CHGD
03/27/08   rn/ic   Added support for new command CM_PH_STATUS_CHGD_REQ
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
02/15/08   rn      Added support for new command CM_PH_STATUS_CHGD_REQ
02/15/08   ks      Adding SIR, Pathloss Implementation
01/24/08   ks      Display of UE signal strength indication
10/03/07   rn      Added smreg_ps_sig_rel_cnf_T in cm_rpt_type
08/17/07   dm      Featurised to report HSPA call status indication
08/17/07   ka/dm   New field to report HS call in progress
05/15/07   rp      Adding ecio to cm_signal_status_type
05/02/07   rp      Adding FEATURE_CM_LCS
04/15/07   rp      Removing FEATURE_MMGPS
04/04/07   rp      Changed type for cell_change_ind
02/18/07   rp      Removed cm_wms_bc_gw_cb_filter
04/17/06   ka      Adding structure for cell services ind.
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
11/30/05   ka      Adding HPLMN srch cnf
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/02/05   ic      Mainlined FEATURE_CIPHER_IND
05/18/05   ka      Merging CIPHER_IND feature.
12/16/04   ic      In cm_rpt_type, put cb_data_ind and cb_duplicate_ind fields
                   under FEATURE_GSM_CB
12/03/04   ws      Support for Remote WMS.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
09/22/04   ka      Removing duplicate declaration for pdp activate sec conf
07/28/04   ka      Added support for Secodnary PDP context
07/21/04   sj      Added support for umts to 1x handover.
06/30/04   dk      Support for smreg_ps_sig_rel_ind_T primitive.
05/21/04   ka      Added new structure for cm_camped_ind
02/06/04   ws      Initial jaguar/mainline merge.
09/18/03   RI      Added cs_sig_rel_ind to cm_rpt_type.
07/08/03   sbs/lt  Added support for Location Services related SS messages
05/01/03   jar     Added gsdi_refresh_fcn_ind_type refresh_fcn_ind to
                   cm_rpt_type
04/11/03   prk     Removed references to FEATURE_SUPS and FEATURE_MMGSDI.
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
                   [CS]Added support for invalid SIM handling. Receive and send
                   GSDI events to CM task.
10/23/02   vt      Mainlined FEATURE_CM_USE_UMTS_REG_INTERFACE.
10/07/02   vt      Added sim state changed report.
07/17/02   vt      Updated it for multimode.
05/04/02   PRK     Modified cm_name_type and cmd to support new CM-REG interface.
04/25/02   RI      Added process_uss_data_conf field to cm_rpt_type.
04/24/02   RI      Added CM_RAB_REESTAB_FAIL and renamed CM_RAB_ESTABLISH_IND to
                   CM_RAB_REESTABLISH_IND in cm_name_type.
                   Added  rabm_rab_reestablish_fail_T message and
                   renamed rabm_rab_establish_ind_T to
                   rabm_rab_reestablish_ind_T in cm_rpt_type.
02/06/02   PRK     Added cm_tc_get_buf_else_err_fatal and cm_tc_send_cmd
                   functions.
02/04/02   RI      Modifications based on mn_cm_exp.h for Supplementary Services.
                     - removed mn_cm_exp_t.h
11/21/01   RI      Added rabm_rab_release_ind_T in cm_rpt_type
                   for PS Data.
10/15/01   RI      Added Supplementary Services declarations.
08/23/01   HD      NAS header file reorg changes + RSSI reporting for GSM/WCDMA.
04/27/01   HD      Initial release.

===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */
#include "queue.h"

#include "nv.h"

#include "mn_cnmtask.h"
#include "mn_cnmtask_v.h"
#include "mn_uasms.h"

#include "mn_cm.h"
#include "reg_task.h"    /* Interface to REG */
#include "reg_task_v.h"
#include "cb_task.h"    /* Interface to CB */
#include "cb_task_v.h"
#include "rabmtask.h"   /* Interface to RABM */
#include "smtask.h"     /* Interface to SM  */
#include "smtask_v.h"
#include "cbwmsif.h"

#include "cmll.h"
#include "cmll_v.h"
#include "cmgan.h"     /* Interface to GAN controller */

#include "reg_task.h"    /* Interface to REG */
#include "reg_cm.h"     /* Interface to REG */

#include "tctask.h"     /* Interface to Test Control (TC) */
#include "tctask_v.h"


#include "mmgsdilib.h"
#include "mmgsdilib_v.h"


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/* message types */

typedef struct {

  int16  signal_strength;

  byte  bit_error_rate;
  int16  ecio;
    /* ECIO valid only for WCDMA. Should be defaulted to
    ** CMSS_ECIO_VALUE_INVALID_SIGNAL for GSM
    */
  int16 pathloss;
    /* Pathloss is applicable only in Connected mode (otherwise 0xFF)
    */
  int16 sir;
    /* SIR applicable and valid (otherwise 0xFF)
    ** only in WCDMA connected mode when DPCCH is active
    */
  int16 rscp;
    /* Signal strength in RSSI, change for WCDMA only (otherwise -125)
    */
  boolean use_ecio_for_display;
    /* TRUE if UE is in a data call, else FALSE
    */

  /* Periodic Average CQI info */
  byte avg_cqi_val;
    /* Avarage CQI value, changes for WCDMA only. Range 0 - 30
    */

  uint16 avg_cqi_num_sample;
    /* Number of samples
    */

  sys_eutra_status_enum_type eutra_detect_status;

} cm_signal_status_type;


/* Type that associates the parameters for requesting UMTS -> CDMA
** handover.
*/
typedef struct {

  sys_is2000_msg_s_type  ho_msg;

} cm_cdma_handover_req_type;



/* Type for CM_SIM_STATE_CHGD_F
*/
typedef struct {
  sys_sim_state_e_type  sim_state;                      /* New SIM state.  */
  sd_ss_e_type          ss;

} cm_sim_state_chgd_type;

/* Type for CM_DATA_SUSPEND_CHGD_F
*/
typedef struct {
  boolean               data_suspend;                   /* New data suspend state.  */
  sd_ss_e_type          ss;
} cm_data_suspend_chgd_type;

/* Type for SI_LIST_CNF */
typedef struct
{
  sys_plmn_id_s_type                                  rplmn; /* RPLMN */
  sys_plmn_list_status_e_type                         status; /* PLMN LIST STATUS */
  //sys_detailed_plmn_list_s_type                       available_plmn_list; /* list of available PLMN'x */
  sys_found_plmn_list_u_type                          found_plmn_list;
  sys_modem_as_id_e_type                              as_id; /* Active subcription ID */
} cm_si_network_list_cnf_s_type;


/*-------------------------------------------------------------------------*/

/* Union of all lower layer report types.  The header is always present and it      */
/* specifies the command type and attributes.  If the command has          */
/* arguments they follow in the union.                                     */


typedef struct
{

  cm_hdr_type                         hdr;       /* header */

  sys_modem_as_id_e_type              asubs_id;
    /* Active subscription ID */

  union
  {

    cm_mo_call_conf_T                   call_conf;
    cm_call_progress_info_ind_T         prog_info;
    cm_call_connected_ind_T             call_connected;
    cm_mt_setup_ind_T                   setup_ind;
    cm_mt_disc_ind_T                    disc_ind;
    cm_mt_end_call_ind_T                end_call_ind;
    cm_connected_number_ind_T           connected_num;
    cm_mt_call_ind_T                    call_ind;
    cm_mo_call_alert_ind_T              call_alert_ind;
    cm_mt_user_data_ind_T               user_data;
    cm_user_congestion_ind_T            user_congestion;
    cm_mm_information_ind_s_type        mm_information_ind;
    cm_network_list_cnf_s_type          net_list_cnf;
    cm_service_cnf_s_type               service_cnf;
    cm_service_ind_s_type               service_ind;
    cm_camped_ind_s_type                camped_ind;
    cm_suitable_search_end_ind_s_type   lte_connected_mode_ind;
    cm_sim_available_cnf_s_type         sim_available_cnf;
    cm_sim_not_available_cnf_s_type     sim_not_available_cnf;
    cm_sim_state_update_ind_s_type      sim_state_update_ind;
    cm_stop_mode_cnf_s_type             stop_mode_cnf;
    cm_ph_status_chgd_cnf_s_type        ph_status_chgd_cnf;
    cm_hplmn_search_cnf_s_type          hplmn_serach_cnf;
    cm_mm_ps_data_suspend_ind_s_type    data_suspend_ind;
    cm_ps_detach_cnf_s_type             ps_detach_cnf;
    cm_si_network_list_cnf_s_type       si_net_list_cnf;

    cm_manage_calls_conf_T              mng_calls_conf;
    cm_mt_call_ss_ind_T                 call_ss_ind;
    cm_register_ss_conf_T               register_conf;
    cm_erase_ss_conf_T                  erase_conf;
    cm_activate_ss_conf_T               activate_conf;
    cm_deactivate_ss_conf_T             deactivate_conf;
    cm_interrogate_ss_conf_T            interrogate_conf;
    cm_register_password_conf_T         reg_password_conf;
    cm_process_unstructured_ss_conf_T   process_uss_conf;
    cm_get_password_ind_T               get_password_ind;
    cm_unstructured_ss_ind_T            uss_ind;
    cm_unstructured_ss_notify_ind_T     uss_notify_ind;
    cm_forward_check_ss_ind_T           fwd_check_ind;
    cm_forward_check_ss_ind_T           rel_uss_ind;
    cm_process_unstructured_ss_data_conf_T
                                      process_uss_data_conf;

    cm_lcs_location_notification_ind_T      lcs_location_notification_ind;
    cm_lcs_molr_conf_T                  lcs_molr_conf;

    cm_start_dtmf_conf_T                dtmf_start_conf;
    cm_stop_dtmf_conf_T                 dtmf_stop_conf;
    smreg_pdp_activate_cnf_T            pdp_act_conf;
    smreg_pdp_activate_rej_T            pdp_act_rej;
    smreg_pdp_activate_ind_T            pdp_act_ind;
    smreg_pdp_deactivate_cnf_T          pdp_deact_conf;
    smreg_pdp_deactivate_ind_T          pdp_deact_ind;
    smreg_pdp_modify_ind_T              pdp_modify_ind;
    smreg_pdp_modify_cnf_T              pdp_modify_conf;
    smreg_pdp_modify_rej_T              pdp_modify_rej;
    smreg_pdp_deactivate_rej_T          pdp_deact_rej;
    smreg_pdp_activate_sec_cnf_T        pdp_act_sec_conf;
    smreg_pdp_activate_sec_rej_T        pdp_act_sec_rej;
    smreg_ps_sig_rel_ind_T              ps_sig_rel_ind;
    smreg_ps_sig_rel_cnf_T              ps_sig_rel_cnf;
    smreg_pdp_deact_promote_ind_T       pdp_deact_prom_ind;
    smreg_pdp_deact_promote_cnf_T       pdp_deact_prom_cnf;
    rabm_rab_reestablish_cnf_T          rab_reestab_conf;
    rabm_rab_reestablish_rej_T          rab_reestab_rej;
    rabm_rab_reestablish_ind_T          rab_reestab_ind;
    rabm_rab_reestablish_fail_T         rab_reestab_fail;
    rabm_rab_release_ind_T              rab_release_ind;
    cm_signal_status_type               signal_status;
    cm_cdma_handover_req_type           umts_cdma_handover_req;
    cm_change_test_mode_ind_T           tc_change_test_mode_ind;

    cm_sim_state_chgd_type              sim_state_chgd;
    cm_data_suspend_chgd_type           data_suspend_chgd;

    cm_cs_sig_rel_ind_T                 cm_sig_rel_ind;

    SM_RL_DATA_IND_T                    rl_data_ind;
    SM_RL_REPORT_IND_T                  rl_report_ind;
    SM_RL_LINK_CONTROL_REPORT_T         rl_link_report_ind;
    cb_data_ind_type                    cb_data_ind;
    cb_duplication_detection_ind_type   cb_duplicate_ind;
    cb_cell_change_ind_type             cell_change_ind;
    cm_cipher_ind_s_type                cipher_ind;
      /* ciphering status (CS ciphered, PS ciphered) */

    cm_ccbs_info_ind_T                  ccbs_info;
      /*  Carries information if ccbs is allowed on call */

    cm_5_send_conf_T                    ccbs_send_conf;
      /* Response to request for CCBS activation */

    cm_recall_ind_T                     recall_ind;
      /* Indication from network for CCBS recall */

    cm_erase_cc_entry_conf_T            erase_cc_entry_conf;
      /* Response to EraseCCEntry request */

    cm_modify_complete_conf_T           modify_complete_conf;
      /* Confirmation to MO modify request */

    cm_mt_modify_ind_T                  mt_modify_ind;
      /* Indication for network initiated modification */

    cm_emergency_num_list_ind_s_type     emerg_num_list;
      /* Emergency call list from network */

    cm_cell_service_ind_s_type           cell_service_ind;
    cm_hspa_call_status_ind_s_type       hspa_call_status_ind;

    cm_conn_mode_ss_ind_s_type    conn_mode_srv_status;

    sys_cell_info_s_type                cell_info;
      /* Indication for cell_info */

    cm_reg_reject_ind_s_type            reg_reject_ind;
      /* Indication for registration reject */

    cm_cell_access_ind_s_type          cell_access_ind;
      /* Indication for Domain Specific Access Control
         (DSAC) info */

    cmgan_gw_gan_rpt_s_type               gan_rpt;
      /* UMA GAN report*/

    mmgsdi_event_data_type              card_status_mmgsdi;
      /* For card staus and Refresh events from MMGSDI */

    smreg_pdp_activate_sec_ind_T        pdp_act_sec_ind;
      /* Indication for MT Sec pdp  */

    smreg_pdp_nw_modify_req_T           pdp_nw_modify_req;
      /* Network send request for pdp modification */

    cm_handover_complete_ind_T          handover_complete_ind;
      /* Indication from NAS for handover complete */
    cm_disable_bplmn_ind_s_type         disable_bplmn_ind;
    cm_bearer_ctxt_transfer_ind_s_type  bearer_ctxt_transfer_ind;

    sys_cell_broadcast_info_s_type      cell_bc_info;
      /* Indication for cell broadcast info */

    sys_codec_ho_info                   codec_info;

    cm_srvcc_handover_complete_ind_T    srvcc_handover_complete_ind;
     /* Indication for SRVCC(L->W) handover complete
     */
    cm_mm_srvcc_handover_failure_ind_s_type srvcc_handover_failure_ind;
     /* Indication for SRVCC(L->W) handover failure
     */

	cm_get_srvcc_context_req_T        nas_srvcc_get_req_ind;
	/* Indication from NAS to fill SRVCC call context 
     */

    cm_fplmn_list_ind_s_type            fplmn_list_info;
      /* convey the fplmn list info */

    cm_get_drx_cnf_s_type               get_drx_cnf;
      /* Cnf for get DRX request */

    smreg_pdp_graceful_abort_cnf_T      pdp_graceful_abort_cnf;
    /* cnf for graceful pdp abort */

    cm_set_drx_cnf_s_type               set_drx_cnf;
      /* Cnf for set DRX request */

    smreg_unblock_all_apn_ind_T        unblock_all_apn;
     /* Indication for unblock all apn */
    cm_get_hplmn_search_timer_cnf_s_type  get_hplmn_timer_cnf;

    cm_ps_attach_failed_ind_s_type         ps_attach_failed_ind;
      /* PS Attach failed indication */

    cm_stack_deact_ind_s_type          stack_deact_ind;
     /* Indication for stack deactivation */

    cm_stack_act_ind_s_type            stack_act_ind;
     /* Indication for stack activation */

    cm_service_ind_s_type              service_stat_ind;

    cm_mm_mt_call_fail_ind_s_type       call_fail_after_page_info;
      /* Info for call fail after pager received */

    cm_home_nodeb_name_ind_s_type      home_nodeb_name;
      /* Hone nodeB name ind from NAS */

    sys_pch_state_s_type                pch_state_info;

    cm_csg_nw_sel_mode_update_ind_s_type csg_nw_sel_mode_update_ind;
      /* PCH state indication */
    cm_csg_search_trigger_ind_s_type csg_search_trigger_ind;

    cm_abort_cc_conf_T               cm_abort_cc_cnf;

    cm_nas_t3346_timer_ind_s_type    t3346_timer_ind;
      /* T3346 timer indication */

	cm_plmn_list_change_ind_s_type   plmn_list_chng_ind;
	 /* update change in PLMN list type*/

    cm_psm_ready_rsp_s_type          psm_ready_rsp;
    /* To respond CM that NAS is ready to enter PSM */

    cm_psm_ready_ind_s_type          psm_ready_ind;
    /* To indicate CM that NAS is ready to enter PSM */

  } cmd;

}  cm_rpt_type;




/*===========================================================================

FUNCTION CM_PUT_CMD

DESCRIPTION
  Allows MN to queue up messages to the CM.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cm_put_cmd(cm_rpt_type *cmd_ptr);



/*===========================================================================

FUNCTION cm_get_cmd_buf

DESCRIPTION
  Allocate a CM Task command buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM command buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_rpt_type *cm_get_cmd_buf(void);

/*===========================================================================

FUNCTION cm_mn_get_buf_else_err_fatal

DESCRIPTION
  Allocate a MN Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MN command buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern mn_cnm_cmd_type *cm_mn_get_buf_else_err_fatal( byte );


/*===========================================================================

FUNCTION cm_mn_send_cmd

DESCRIPTION
  Send a command to the MN task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_mn_send_cmd( mn_cnm_cmd_type  *mn_cmd_ptr );


/*===========================================================================

FUNCTION cm_reg_get_buf_else_err_fatal

DESCRIPTION
  Allocate a REG Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated REG command buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern reg_cmd_type *cm_reg_get_buf_else_err_fatal( cm_to_reg_cmd_type cmd );

/*===========================================================================

FUNCTION cm_reg_send_cmd

DESCRIPTION
  Send a command to the REG task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_reg_send_cmd( reg_cmd_type  *reg_cmd_ptr );

/*===========================================================================

FUNCTION cm_sm_get_buf_else_err_fatal

DESCRIPTION
  Allocate a SM Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated SM command buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern sm_cmd_type *cm_sm_get_buf_else_err_fatal( sm_cmd_enum_type cmd );


/*===========================================================================

FUNCTION cm_sm_send_cmd

DESCRIPTION
  Send a command to the SM task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_sm_send_cmd( sm_cmd_type *sm_cmd_ptr);


/*===========================================================================

FUNCTION cm_rabm_get_buf_else_err_fatal

DESCRIPTION
  Allocate a RABM Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated RABM command buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern rabm_cmd_type *cm_rabm_get_buf_else_err_fatal( rabm_cmd_enum_type cmd );


/*===========================================================================

FUNCTION cm_rabm_send_cmd

DESCRIPTION
  Send a command to the RABM task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_rabm_send_cmd(rabm_cmd_type *rabm_cmd_ptr);


/*===========================================================================

FUNCTION cm_tc_get_buf_else_err_fatal

DESCRIPTION
  Allocate a TC Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated TC command buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern tc_cmd_type *cm_tc_get_buf_else_err_fatal( tc_cmd_enum_type  cmd );



/*===========================================================================

FUNCTION cm_tc_send_cmd

DESCRIPTION
  Send a command to the TC task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_tc_send_cmd( tc_cmd_type *tc_cmd_ptr );



/*===========================================================================

FUNCTION cmwll_send_rrc_cdma_cap_update

DESCRIPTION
  Queues the CDMA capability update request to CM.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmwll_send_rrc_cdma_cap_update(

     const sys_is2000_msg_s_type  *cap_ptr
);

/*===========================================================================

FUNCTION cmwll_cmd_rrc_get_dch_cell_info_req

DESCRIPTION
  Queues DCH CELL INFO REQ to RRC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmwll_send_rrc_get_dch_cell_info_req(
       void
);

/*===========================================================================

FUNCTION cmwll_send_rrc_cdma_handover_cnf

DESCRIPTION
  Queues the CDMA2000 handover confirmation.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmwll_send_rrc_cdma_handover_cnf(

     sys_umts_cdma_handover_e_type    status
       /* Handover confirmation status.
       */
);

#endif /* CMWLL_H */
