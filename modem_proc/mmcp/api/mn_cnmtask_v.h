#ifndef mn_cnmtask_v_h
#define mn_cnmtask_v_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/mn_cnmtask.h_v   1.12   22 Apr 2002 09:48:00   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mn_cnmtask_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/23/03   sbs     Added cm_lcs_molr_completed to mn_cnm_cmd_type
 
22-Mar-2001  VT    Introduced extern cm_reserve_connection_id()

4/24/01     CD     Added function prototype for mn_cm_get_cmd_buffer
                   Added IMH_T to the 'cmd' union in the definition of mn_cnm_cmd_type

23-May-2001  VT    Added include for "cc_iei.h" to make this file self-contained

6/8/01      CD     Removed mmi-sms commands and added SM_RL_ABORT_SM_REQ

7/12/01     CD     Removed unnecesary includes cm_mn.h, mn_sms.h, mn_ss.h, cc_iei.h

7/18/01     CD     Added external declaration of is_gsm_mode and is_umts_mode

7/19/01     CD     Added include of queue.h

7/31/01     JC     Add watch dog reporting

8/02/01     CD     Replaced mn.h by mn_exp.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

01/25/02    CD     Removed #include for mn_exp.h and included mn header 
                   files instead since mn_exp.h is now empty

04/19/02    CD     Added MMCC_REEST_REJ primitive per new MM-CNM interface
                   Added MMCNM_EST_REJ primitive per new MM-CNM interface
                   Added MMCNM_REL_CNF primitive per new MM-CNM interface

07/19/02    AB     Added cm_4_x_send_req_T into mn_cnm_cmd_type, to reflect 
                   the new member of this union command buffer type.  This
                   command buffer is generally requested by CM/UI to build
                   and send command to MN.
 
07/29/02    AB     Added the mn_cnm_main_task() prototypes for ANSI C 
                   compliant.
03/21/03    KS     Renamed the mn_cnm_main_task() to mn_cnm_main().


04/15/03   sbs     Added support for Location Services related SS messages

05/24/05    HS     Added support for FEATURE_CCBS 
 
04/17/12   hc      Added Prototype for mn_has_nas_already_acquired_voc(void) 
===========================================================================*/


#include "queue.h"
#include "timers.h"
#include "ms_timer.h"

#include "nas_exp.h"
#include "reg_cm.h"
#include "mn_cm_exp.h"
#include "mn_cm.h"
#include "mn_uasms.h"
#include "mn_sms.h"
#include "mn_cc.h"
#include "mn_ss.h"
#include "ms_cc_mm.h"


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef struct{
    
q_link_type      link;

union{
    IMH_T       hdr;
/* --------------------------------------------------------- */
/*            CM (Call Manager) to MN_CM Messages           */
/* --------------------------------------------------------- */

  cm_mo_normal_call_req_T         cm_mo_normal_call_req;
    cm_mo_emergency_call_req_T      cm_mo_emergency_call_req;
    cm_mo_end_call_req_T            cm_mo_end_call_req;
    cm_start_dtmf_req_T             cm_start_dtmf_req;
    cm_stop_dtmf_req_T              cm_stop_dtmf_req;
    cm_n_send_req_T                 cm_n_send_req;
    cm_n_x_send_req_T               cm_n_x_send_req;
  cm_4_x_send_req_T               cm_4_x_send_req;

    cm_5_send_req_T                 cm_5_send_req;
    cm_asid_2_vsid_mapping_ind_T    cm_asid_to_vsid_mapping;
    cm_srvcc_call_context_T         cm_srvcc_call_context_res;
    
  cm_recall_rsp_T                 cm_recall_rsp;
  cm_erase_cc_entry_req_T         cm_erase_cc_entry_req;

  cm_abort_cc_req_T               cm_abort_cc_req;

    cm_mo_user_data_req_T           cm_mo_user_data_req;

    cm_mo_modify_req_T              cm_mo_modify_req;
    cm_register_ss_req_T            cm_register_ss_req;
    cm_activate_ss_req_T            cm_activate_ss_req;
    cm_deactivate_ss_req_T          cm_deactivate_ss_req;
    cm_erase_ss_req_T               cm_erase_ss_req;
    cm_interrogate_ss_req_T         cm_interrogate_ss_req;
    cm_register_password_req_T      cm_register_password_req;
    cm_process_unstructured_ss_req_T
                                    cm_process_unstructured_ss_req;
    cm_process_unstructured_ss_data_T
                                    cm_process_unstructured_ss_data;
    cm_abort_ss_req_T               cm_abort_ss_req;
    cm_release_ss_req_T             cm_release_ss_req;
    cm_mt_call_res_T                cm_mt_call_res;
    cm_mt_setup_res_T               cm_mt_setup_res;
    cm_mt_modify_res_T              cm_mt_modify_res;
    cm_mt_call_ss_res_T             cm_mt_call_ss_res;
    cm_unstructured_ss_notify_res_T cm_unstructured_ss_notify_res;
    cm_unstructured_ss_res_T        cm_unstructured_ss_res;
    cm_get_password_res_T           cm_get_password_res;

    cm_lcs_location_notification_res_T cm_lcs_location_notification_res;
    cm_lcs_molr_req_T                  cm_lcs_molr_req;
    cm_lcs_molr_compl_T                cm_lcs_molr_completed;

/* --------------------------------------------------------- */
/*                      MM to MN_CM Messages                 */
/* --------------------------------------------------------- */

    /*mmcnm_abort_ind_T                 mmcnm_abort_ind;*/
    mmcnm_data_ind_T                  mmcnm_data_ind;
    mmcnm_est_rej_T                   mmcnm_est_rej;
    mmcnm_est_cnf_T                   mmcnm_est_cnf;
    mmcnm_rel_ind_T                   mmcnm_rel_ind;
        mmcnm_rel_cnf_T                   mmcnm_rel_cnf;
    mmcc_reest_cnf_T                  mmcc_reest_cnf;
        mmcc_reest_rej_T                  mmcc_reest_rej;
    mmcc_sync_ind_T                   mmcc_sync_ind;

/* ---------------------------------------------------------*/
/*                     UASMS to MN Messages                 */
/* ---------------------------------------------------------*/
    SM_RL_LINK_CONTROL_REPORT_T      sm_rl_link_control_report;
    SM_RL_LINK_CONTROL_REQ_T         sm_rl_link_control_req;
    SM_RL_REPORT_IND_T               sm_rl_report_ind;
    SM_RL_REPORT_REQ_T               sm_rl_report_req;
    SM_RL_DATA_IND_T                 sm_rl_data_ind;
    SM_RL_DATA_REQ_T                 sm_rl_data_req;
    SM_RL_MEMORY_AVAILABLE_REQ_T     sm_rl_memory_available_req;
    SM_RL_ABORT_SM_REQ_T             sm_rl_abort_sm_req;
    SM_RL_UPDATE_TIMERF_T            sm_rl_update_timerf;

/* --------------------------------------------------------- */
/*                     MN_CM Internal Messages               */
/* --------------------------------------------------------- */

/*                     MN Internal PRIMITIVES                */
/*  mn_call_information_T            mn_call_information;  */
    MNCC_SETUP_data_T                mncc_setup_data;
    MNCC_MODIFY_data_T               mncc_modify_data;
    MNCC_CALL_PROC_data_T            mncc_call_proc_data;
    MN_FACILITY_DATA_T               mn_facility_data;
/*  cm_modify_complete_data_T       cm_modify_complete_data; */

/*                       MN to CC PRIMITIVES                 */
    MNCC_ALERT_REQ_T                 mncc_alert_req;
    MNCC_CALL_CONF_REQ_T             mncc_call_conf_req;
    MNCC_CALL_HOLD_REQ_T             mncc_call_hold_req;
    MNCC_CALL_RETRIEVE_REQ_T         mncc_call_retrieve_req;
    MNCC_DISC_REQ_T                  mncc_disc_req;
    MNCC_FACILITY_REQ_T              mncc_facility_req;
    MNCC_REJ_REQ_T                   mncc_rej_req;
    MNCC_REL_REQ_T                   mncc_rel_req;
    MNCC_MODIFY_REQ_T                mncc_modify_req;
    MNCC_MODIFY_RES_T                mncc_modify_res;
    MNCC_SETUP_COMPL_REQ_T           mncc_setup_compl_req;
    MNCC_SETUP_COMPL_RES_T           mncc_setup_compl_res;
    MNCC_SETUP_RES_T                 mncc_setup_res;
    MNCC_SETUP_REQ_T                 mncc_setup_req;
    MNCC_START_DTMF_REQ_T            mncc_start_dtmf_req;
    MNCC_STOP_DTMF_REQ_T             mncc_stop_dtmf_req;
    MNCC_USER_USER_DATA_REQ_T        mncc_user_user_data_req;
    MNCC_MODIFY_ACCEPT_T             mncc_modify_accept;

/*                       CC to MN PRIMITIVES                 */
        MNCC_ALERT_IND_T                 mncc_alert_ind;
    MNCC_CALL_HOLD_CNF_T             mncc_call_hold_cnf;
    MNCC_CALL_HOLD_REJ_T             mncc_call_hold_rej;
    MNCC_CALL_PROC_IND_T             mncc_call_proc_ind;
    MNCC_CALL_RETRIEVE_CNF_T         mncc_call_retrieve_cnf;
    MNCC_CALL_RETRIEVE_REJ_T         mncc_call_retrieve_rej;
    MNCC_DISC_IND_T                  mncc_disc_ind;
    MNCC_ERR_IND_T                   mncc_err_ind;
    MNCC_FACILITY_IND_T              mncc_facility_ind;
    MNCC_MODIFY_CNF_T                mncc_modify_cnf;
    MNCC_MODIFY_IND_T                mncc_modify_ind;
    MNCC_NOTIFY_IND_T                mncc_notify_ind;
    MNCC_PROGRESS_IND_T              mncc_progress_ind;
    MNCC_REEST_IND_T                 mncc_reest_ind;
    MNCC_REJ_IND_T                   mncc_rej_ind;
    MNCC_REL_CNF_T                   mncc_rel_cnf;
    MNCC_REL_IND_T                   mncc_rel_ind;
    MNCC_SETUP_CNF_T                 mncc_setup_cnf;
    MNCC_SETUP_COMPL_IND_T           mncc_setup_compl_ind;
    MNCC_SETUP_IND_T                 mncc_setup_ind;
    MNCC_START_DTMF_CNF_T            mncc_start_dtmf_cnf;
    MNCC_STOP_DTMF_CNF_T             mncc_stop_dtmf_cnf;
    MNCC_SYNC_IND_T                  mncc_sync_ind;

    MNCC_USER_USER_DATA_IND_T        mncc_user_user_data_ind;


/*                       MN to SMS PRIMITIVES                */
        MNSMS_EST_REQ_T                  mnsms_est_req;
        MNSMS_DATA_REQ_T                 mnsms_data_req;
    MNSMS_ABORT_REQ_T                mnsms_abort_req;
    MNSMS_REL_REQ_T                  mnsms_rel_req;

/*                       SMS to MN PRIMITIVES                */
    MNSMS_DATA_IND_T                 mnsms_data_ind;
    MNSMS_EST_IND_T                  mnsms_est_ind;
    MNSMS_ERROR_IND_T                mnsms_error_ind;

/*                       MN to SS PRIMITIVES                 */
        MNSS_BEGIN_REQ_T                 mnss_begin_req;
    MNSS_END_REQ_T                   mnss_end_req;
    MNSS_FACILITY_REQ_T              mnss_facility_req;
    
/*                       SS to MN PRIMITIVES                 */
        MNSS_BEGIN_IND_T                 mnss_begin_ind;
    MNSS_END_IND_T                   mnss_end_ind;
    MNSS_FACILITY_IND_T              mnss_facility_ind;
    MNSS_REJ_IND_T                   mnss_rej_ind;

    timer_expiry_T                   timer_expiry;


} cmd;
    
}  mn_cnm_cmd_type;



/* =======================================================================
**                            Function Definitions
** ======================================================================= */



extern mn_cnm_cmd_type *mn_cm_get_cmd_buf(byte cmd_id);
extern mn_cnm_cmd_type *mn_uasms_get_cmd_buf(byte cmd_id);
extern void mn_cnm_put_cmd(mn_cnm_cmd_type *cmd_ptr);
extern void mn_cnm_put_internal_cmd(mn_cnm_cmd_type *cmd_ptr);

extern boolean is_cs_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_wcdma_mode( sys_modem_as_id_e_type as_id );

extern boolean is_umts_mode(void);

extern boolean mn_has_nas_already_acquired_voc(void);

extern boolean mn_has_nas_already_acquired_voc_subs(sys_modem_as_id_e_type as_id);

extern nas_vote_for_reselection_e_type mn_voice_call_vote_for_reselection(sys_modem_as_id_e_type as_id);
#endif  /* ifdef mn_cnmtask_v_h */

