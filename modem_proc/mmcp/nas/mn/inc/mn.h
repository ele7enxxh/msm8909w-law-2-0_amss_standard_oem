#ifndef mn_h
#define mn_h

/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited..

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn.h_v   1.33   17 Jun 2002 16:13:30   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/inc/mn.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
26-Mar-2001  VT     Added declaration for MN_put_mmi_call_connected_ind()
30-Apr-2001  VT     Removed #ifndef FEATURE_TREX_UNIT_TEST, which was enclosing 
                     MN function declarations

5/4/01       CD      Per CM request, included header files to make mn.h self-contained

5/4/01       CD     Changed function protoptype of MN_handle_mmi_abort_cc_req, 
                    MN_handle_MNCC_ERR_IND, MN_handle_MNCC_REEST_IND, 
                    MN_process_mmi_0_send, MN_process_mmi_1_send,
                    MN_process_mmi_2_send, MN_process_mmi_3_send,
                    MN_process_mmi_mo_end_call_req
                    MN_handle_mmi_check_for_smma_req
                    to correct compile warning C4100 - unreferenced formal parameter

    5/9/01       CD     Included mn_ss.h

5/15/01      CD     Added prototype for EncodeAnsInt()

5/21/01      CD     Removed redundant declaration of MN_put_MNCC_SETUP_RES 
                    to avoid lint error

6/7/01      CD     Changed function name of MN_route_SMS_MMI_to_MN_primitives()
                   to MN_route_UASMS_MMI_to_MN_primitives()
                  
                   Deleted function prototypes for 
                        MN_handle_mmi_erase_ram_sm_req()
                        MN_handle_mmi_confidure_SM_storage_req()
                        MN_handle_mmi_read_short_message_req()
                        MN_handle_mmi_submit_sm_from_sim_req()
                        MN_handle_mmi_submit_sm_req()
                        MN_handle_mmi_check_for_smma_req()
                        MN_handle_mmi_abort_sm_req()
                        MN_handle_mmi_sm_command_req()
                        MN_adjust_tpoa_length()
                        MN_put_tpdu_deliver()
                        MN_put_tpdu_status_report()
                        MN_get_free_record()
                        MN_get_free_SIM_record()
                        MN_get_free_SIM_record_quick()
                        MN_get_free_RAM_record()
                        MN_find_replacement()
                        MN_find_replacement_record_in_RAM()
                        MN_compare_sm_type()
                        MN_read_sms_status()
                        MN_write_sms_status()
                        MN_handle_mt_tl_sms()
                        MN_put_mmi_mt_sm_available_ind()
                        MN_put_mmi_sm_status_report_ind()
                        MN_put_mmi_mt_sms_ind()
                        MN_put_mmi_deliver_sm_conf()
                        MN_put_mmi_submit_sm_conf()
                        MN_put_mmi_check_for_smma_conf()

                   Added function prototype for 
                        MN_put_uasms_sm_data_ind()
                        MN_put_uasms_sm_report_ind()

6/8/01      CD     Added IMH_T header to SMR primitives.
                   Added SM_RL_MEMORY_AVAILABLE_CONF_T and SM_RL_ABORT_SM_REQ_T

7/5/01      CD     Added prototype for get_timer_name()

7/12/01     CD     Removed CC, SS and SMS data definitions 
                   to mn_cc.h, mn_ss.h and mn_sms.h respectively

                   Grouped function protoypes by module (CC/SS/SMS)

7/16/01     CD     Added prototype for show_rl_state()

7/17/01     CD     Changed function prototype of MN_process_SM_RL_MEM_AVIL_REQ. 
                   Passing the primitive is no longer needed

8/01/01     CD     Removed include for cc_iei.h.  It's now included by mn_cc.h

                   Added include for mn_cm_exp.h

8/07/01     CD     Removed function prototypes since they are not being used
                        MN_handle_CM_to_MN_primitives
                        MN_put_MNCC_SETUP_COMPL_RES
                        MN_put_mmi_mt_data_connected_ind
                        MN_put_ss_User_Data

08/16/01    AB     Renamed MMI references to CM.

08/21/01    CD     Added mnglobal.h

9/5/01      CD     Added function prototypes for QXDM logging functions

9/13/01     CD     Added function prototype for logging messages

9/18/01     CD     Changed function prototype for mn_log_message

10/08/01    AB     Change SS primitives function prototype to removed Lint 
                     casting error.

10/22/01    CD     Removed customer.h.

12/01/01    AB     Updated MN_start_ss_timer() prototype per interface changes.

01/22/02    AB     Added new MN_whichMultiCallIndex() function prototype.

01/29/02    RG     Change the prototype for MN_process_SM_RL_MEM_AVAIL_REQ() routine as a part of
                   giving the UASMS the ability to correlate the Memory Availability Requests
                   with the corresponding responses (the SM_RL_REPORT_IND).

01/28/02    CD     Added cm_ss_conf_e_type to MN_put_cm_interrogate_ss_conf()
                   and MN_handle_interrogate_ss_res()
                   Updated Copyright

02/18/02    AB     Updated SS Call independent confirmation routine 
                      prototype to a new IEI element interfaces changes.

03/12/02    AB     Added 4_SEND and 4_X_SEND handler function prototypes used for
                      Explicit call transfer and Call Defleection, respectively.

04/08/02    AB     Added following function prototypes:
                     MN_process_MNCC_DEFERRED_IND() 
                     MN_put_notify_deferred_ind(), and
                     MN_get_ect_indicator().
                       
05/06/02    AB     Removed unused MM_cm_release_ss_req() function prototypes.

05/31/02    CD     Added connection_id as argument to MN_get_bearer_capability_data()

06/03/02    CD     Merged in changes from revision 1.28.1.0
                   In the prototype for MN_disconnect_TCH(), added
                     parameter of type mn_vocoder_ctrl_e_type.

09/04/02    AB     Added MN_put_cm_manage_calls_x_conf() function prototype.

09/16/02    AB     Added MN_put_MNCC_REL_COMPL_REQ() function prototype.

10/06/02    AB     Added MN_process_cm_mo_end_x_call_req() prototype for
                     aborting specific 2nd or subsequence call.

02/02/03    AB     Added new function prototype, MN_get_name_indicator(), 
                     and MN_get_ccbs_indicator() prototype for processing
                     of SS notify message.

04/04/03    AB     Added reject cause value in the MN_put_cm_mo_call_conf(),
                     MN_put_cm_mt_call_end_ind() function. 

04/15/03   sbs     Added support for Location Services related SS messages

09/10/03    AB     Added MN_put_cm_rab_released_ind() function prototype.

09/16/03    AB     Added support to release LCS session.

05/03/05    HS     Added MN_get_element_length2() to correct element length calculation.

05/10/05    HS     Added support for FEATURE_CCBS

07/08/05    HS     MN_get_ccbs_indicator() not used anymore. Use MN_get_ccbs_feature() instead.

08/18/05    HS     MN_num_of_calls_in_group() added

08/28/06    NR     MN_release_invoke_ids() added for clearing invoke ids

01/11/07    NR     Adding fix for rejecting multiple USSD with retaining transactin id
                   inspite of invoke id being cleared

04/20/07    NR     Adding new function for releasing MO invoke ids
===========================================================================*/
#include "mmcp_variation.h"
#include "sys_cnst_v.h"
#include "sys_type.h"
#include "sys_stru_v.h"
#include "gs_v.h"

#include "mn_cm_exp_v.h"
#include "mn_cm.h"  
#include "mn_uasms_v.h"
#include "mn_sms_v.h"
#include "mn_cc_v.h"
#include "mn_ss_v.h"
#include "mnglobal.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

/***********************
 * Function Prototypes *
 ***********************/

/*****************************/
/**** Routing functions   ****/
/*****************************/

VOID_FUNC   MN_handle_MN_messaging(IMH_T   *MN_input_primitives);

VOID_FUNC   MN_handle_CM_to_MN_primitives(IMH_T   *CM_to_MN_primitives);

VOID_FUNC   MN_route_CC_CM_to_MN_primitives(IMH_T   *message);

VOID_FUNC   MN_route_SS_CM_to_MN_primitives(IMH_T  *);

VOID_FUNC   MN_route_SS_to_MN_primitives(IMH_T  *);

VOID_FUNC   MN_route_UASMS_to_MN_primitives(IMH_T *);

VOID_FUNC   MN_route_SMS_to_MN_primitives(IMH_T  *);

VOID_FUNC   MN_route_CC_to_MN_messages(IMH_T    *message);


/**********************************/
/**** Call Control functions   ****/
/**********************************/

boolean     MN_read_multicall_status(void);

VOID_FUNC   MN_request_mo_call_setup(IMH_T   *message);

VOID_FUNC   MN_process_cm_start_dtmf_req(IMH_T *message);

VOID_FUNC   MN_process_cm_stop_dtmf_req(IMH_T *message);

VOID_FUNC   MN_process_cm_0_send(cm_n_send_req_T *message);

VOID_FUNC   MN_process_cm_1_send(cm_n_send_req_T *message);

VOID_FUNC   MN_process_cm_1_x_send(cm_n_x_send_req_T  *message);

VOID_FUNC   MN_process_cm_2_send(cm_n_send_req_T *message);

VOID_FUNC   MN_process_cm_2_x_send(cm_n_x_send_req_T  *message);

VOID_FUNC   MN_process_cm_3_send(cm_n_send_req_T *message);

VOID_FUNC   MN_process_cm_4_send(cm_n_send_req_T    *pointer);

VOID_FUNC   MN_process_cm_4_x_send(cm_4_x_send_req_T    *pointer);

#ifdef FEATURE_CCBS
VOID_FUNC   MN_process_cm_5_send(cm_5_send_req_T    *pointer);
#endif /* FEATURE_CCBS */

VOID_FUNC   MN_process_cm_mo_end_call_req(void);

VOID_FUNC   MN_process_cm_mo_end_x_call_req(cm_n_x_send_req_T  *message);

VOID_FUNC   MN_process_cm_mo_user_data_req(IMH_T   *message);

byte        MN_put_USER_USER_DATA_REQ(connection_id_T connection_id);

VOID_FUNC   MN_put_MNCC_SETUP_RES(connection_id_T             connection_id, 
                                  cm_connected_subaddress_T  *connected_subadd_ptr);

VOID_FUNC   MN_disconnect_TCH(mn_vocoder_ctrl_e_type mn_voc_ctrl);
#if defined(FEATURE_DUAL_SIM )
VOID_FUNC   MN_disconnect_TCH_with_RAT(mn_vocoder_ctrl_e_type mn_voc_ctrl,sys_radio_access_tech_e_type current_rat, sys_modem_as_id_e_type as_id);
VOID_FUNC   MN_disconnect_TCH_with_AS_ID(mn_vocoder_ctrl_e_type mn_voc_ctrl,sys_modem_as_id_e_type as_id);
#endif

VOID_FUNC   MN_put_cm_call_progress_info_ind(connection_id_T  connection_id,
                                              byte             notification_indicator);

VOID_FUNC   MN_put_cm_call_alert_ind(connection_id_T connection_id);

#ifdef FEATURE_CCBS
VOID_FUNC  MN_put_cm_ccbs_info_ind (connection_id_T connection_id, boolean ccbs_allowed);
VOID_FUNC  MN_put_cm_recall_ind (connection_id_T connection_id, byte type);
#endif /* FEATURE_CCBS */

VOID_FUNC  MN_put_cm_srvcc_ho_complete_ind(boolean success);

VOID_FUNC  MN_put_MNCC_SRVCC_HANDOVER_FAIL_REQ(void);

VOID_FUNC  MN_put_cm_get_srvcc_context_req(sys_radio_access_tech_e_type  active_rat);



VOID_FUNC  MN_handle_srvcc_ho_complete(IMH_T *message_header);

VOID_FUNC   MN_put_cm_call_connected_ind(connection_id_T   connection_id);

VOID_FUNC   MN_put_cm_connected_number_ind(connection_id_T  connection_id);

VOID_FUNC   MN_put_cm_manage_calls_conf(boolean                call_ss_success,
#if defined(FEATURE_DUAL_SIM )
                                        sys_modem_as_id_e_type as_id,
#endif

                                         ie_cm_ss_error_T      cm_ss_error);

VOID_FUNC   MN_put_cm_manage_calls_x_conf(connection_id_T connection_id, 
                                          boolean             call_ss_success,
                                          ie_cm_ss_error_T   cm_ss_error);

#ifdef FEATURE_CCBS
VOID_FUNC   MN_put_cm_erase_cc_entry_conf ( boolean,
                                            byte, 
#if defined(FEATURE_DUAL_SIM )
                                            sys_modem_as_id_e_type,
#endif
                                            ss_info_T *);

VOID_FUNC   MN_put_cm_5_send_conf (connection_id_T, 
                                   cm_5_send_conf_e_type, 
                                   ie_ccbs_indicator_T *, 
#if defined(FEATURE_DUAL_SIM )
                                   sys_modem_as_id_e_type ,
#endif
                                   ie_cm_ss_error_T *);

#endif /* FEATURE_CCBS */

VOID_FUNC   MN_put_cm_mt_setup_ind(connection_id_T     connection_id);

VOID_FUNC   MN_put_cm_mo_call_conf(connection_id_T         connection_id,
                                   boolean                 cm_service_success,
                                   cm_bearer_capability_T  bearer_capability_1,
                                   cm_bearer_capability_T  bearer_capability_2,
                                   ie_cm_cc_cause_T        cc_cause,
#if defined(FEATURE_DUAL_SIM )
                                   sys_modem_as_id_e_type   as_id,  
#endif
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
                                   ie_cm_cc_reject_T          *cc_reject,
                                   cm_repeat_indicator_T      *repeat_indicator_bc,
                                   cm_network_cc_capability_T *network_cc_capability
#else
                                   ie_cm_cc_reject_T          *cc_reject
#endif    
                                  );

VOID_FUNC   MN_put_cm_mt_call_ind(
#if defined(FEATURE_DUAL_SIM )
                  sys_modem_as_id_e_type     as_id,
#endif
                  connection_id_T  connection_id);


#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
VOID_FUNC   MN_handle_cm_mo_modify_req(IMH_T  *message);

VOID_FUNC   MN_handle_cm_mt_modify_res(IMH_T *);

VOID_FUNC   MN_put_cm_mo_modify_complete_conf(cm_modify_complete_data_T  *cm_modify_complete_data);

VOID_FUNC   MN_put_cm_mt_modify_ind(connection_id_T, MNCC_MODIFY_data_T *);
#endif 

VOID_FUNC   MN_put_notify_deferred_ind( connection_id_T connection_id, 
                                        ie_cm_notify_ss_T  *cm_notify_ss_data);



VOID_FUNC   MN_put_cm_mt_call_ss_ind(connection_id_T          connection_id,
                                      byte                     msg_type,
                                      ie_cm_notify_ss_T       *cm_notify_ss_data,
                                      ie_cm_forward_charge_advice_req_T  
                                                               *cm_forward_charge_advice_data);

VOID_FUNC   MN_put_cm_mt_disc_ind(connection_id_T, ie_cm_cc_cause_T *, boolean);

VOID_FUNC   MN_put_cm_mt_end_call_ind(connection_id_T    connection_id,
                                      ie_cm_cc_cause_T   cm_cc_cause,
#if defined(FEATURE_DUAL_SIM )
                                      sys_modem_as_id_e_type     as_id,
#endif
                                      ie_cm_cc_reject_T  *cm_cc_reject);

VOID_FUNC   MN_put_cm_rab_released_ind(
#if defined(FEATURE_DUAL_SIM )
      IMH_T *
#else
      void
#endif
     );

VOID_FUNC   MN_put_cm_mt_user_data_ind(connection_id_T  connection_id,
                                        byte             user_user_data[MAX_USER_USER_DATA_LENGTH],
                                        byte             user_user_information_length,
                                        byte             user_user_protocol_discriminator,
                                        boolean          more_data,
                                        boolean          deferred);

VOID_FUNC   MN_put_cm_congestion_ind(connection_id_T  connection_id,
                                     byte             cause,
                                     boolean          receiver_ready);

VOID_FUNC   MN_put_cm_start_dtmf_conf(connection_id_T  connection_id,
#if defined(FEATURE_DUAL_SIM )
                                     ie_cm_cc_reject_T      cm_cc_reject,
                                     sys_modem_as_id_e_type as_id,
#endif
                                     boolean          dtmf_success);

VOID_FUNC   MN_put_cm_stop_dtmf_conf(
#if defined(FEATURE_DUAL_SIM )
                                     connection_id_T        connection_id,
                                     ie_cm_cc_reject_T      cm_cc_reject,
                                     sys_modem_as_id_e_type as_id
#else
                                     connection_id_T        connection_id

#endif
                                    );
VOID_FUNC   MN_put_MNCC_ALERT_REQ(connection_id_T connection_id);

VOID_FUNC   MN_put_MNCC_CALL_CONF_REQ(boolean             negotiation,
                                      connection_id_T     connection_id,
                                      ie_cm_cc_cause_T   *cm_cc_cause);

VOID_FUNC   MN_put_MNCC_CALL_RETRIEVE_REQ(connection_id_T connection_id);

VOID_FUNC   MN_put_MNCC_DISC_REQ(connection_id_T    connection_id, 
                                 ie_cm_cc_cause_T  *cm_cc_cause,
                                 ie_cm_cc_invoke_T *cm_cc_invoke);

VOID_FUNC   MN_put_MNCC_FACILITY_REQ(connection_id_T  connection_id,
                                     byte             components[MAX_COMPONENTS_LENGTH],
                                     int              components_length,
                                     byte             ss_version[MAX_SS_VERSION_IND_LENGTH],
                                     int              ss_version_length);

VOID_FUNC   MN_put_MNCC_CALL_HOLD_REQ(connection_id_T connection_id);

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
VOID_FUNC   MN_put_MNCC_MODIFY_ACCEPT(connection_id_T  connection_id, 
                                     boolean          accept);

VOID_FUNC   MN_put_MNCC_MODIFY_REQ(MNCC_MODIFY_data_T *modify_data_ptr);

VOID_FUNC   MN_put_MNCC_MODIFY_RES(connection_id_T      connection_id, 
                                   MNCC_MODIFY_data_T  *modify_data_ptr);

VOID_FUNC   MN_get_MNCC_MODIFY_CNF(MNCC_MODIFY_CNF_T   *message_ptr, 
                                   MNCC_MODIFY_data_T  *modify_data_ptr);

VOID_FUNC   MN_get_MNCC_MODIFY_data(int                 message_position, 
                                    int                 message_length,
                                    byte                *data_ptr,
                                    MNCC_MODIFY_data_T  *modify_data_ptr,
                                    byte * last_octet_data_ptr);

VOID_FUNC   MN_get_MNCC_MODIFY_IND(MNCC_MODIFY_IND_T   *message_ptr,
                                   MNCC_MODIFY_data_T  *modify_data_ptr);
#endif

VOID_FUNC   MN_put_MNCC_REJ_REQ(connection_id_T    connection_id,
                                ie_cm_cc_cause_T  *cm_cc_cause);

VOID_FUNC   MN_put_MNCC_REL_REQ(connection_id_T    connection_id,
                                ie_cm_cc_cause_T *cm_cc_cause_1,
                                ie_cm_cc_cause_T *cm_cc_cause_2,
                                int                no_of_causes
#ifdef FEATURE_CCBS
                               ,ie_facility_T    *p_facility
#endif /* FEATURE_CCBS */
                                );

VOID_FUNC   MN_put_MNCC_REL_COMPL_REQ(connection_id_T    connection_id,
                                      byte pd,
                                      byte cause,
                                      ie_facility_T *p_facility);

VOID_FUNC   MN_put_MNCC_SETUP_COMPL_REQ(connection_id_T connection_id);

VOID_FUNC   MN_put_MNCC_SETUP_REQ(connection_id_T connection_id);

VOID_FUNC   MN_put_MNCC_START_DTMF_REQ(connection_id_T   connection_id, 
                                       byte              keypad_information);

VOID_FUNC   MN_put_MNCC_STOP_DTMF_REQ(connection_id_T connection_id);

VOID_FUNC   MN_put_MNCC_USER_USER_DATA_REQ(connection_id_T  connection_id,
                                           ie_user_user_T   *ie_user_user_ptr,
                                           boolean          more_data);

VOID_FUNC MN_process_MNCC_USER_DATA_DEFERRED_IND(IMH_T  *message_ptr);

boolean MN_get_cm_mo_call_req(connection_id_T  new_connection_id, 
                               IMH_T            *message_ptr);

VOID_FUNC   MN_get_MNCC_CALL_PROC_IND(MNCC_CALL_PROC_IND_T   *message_ptr, 
                                      MNCC_CALL_PROC_data_T  *proc_data_ptr);

VOID_FUNC   MN_get_MNCC_REJ_IND(MNCC_REJ_IND_T     *message_ptr,
                                ie_cm_cc_cause_T  *cause_data_ptr);

VOID_FUNC   MN_get_MNCC_REL_CNF(MNCC_REL_CNF_T    *message_ptr,
                                ie_cm_cc_cause_T *cm_cc_cause_ptr);

int   MN_get_MNCC_REL_IND(MNCC_REL_IND_T      *message_ptr, 
                          ie_cm_cc_cause_T   *cause_data_ptr_1, 
                          ie_cm_cc_cause_T   *cause_data_ptr_2);

connection_id_T   MN_get_MNCC_SETUP_CNF(MNCC_SETUP_CNF_T *message_ptr);

connection_id_T   MN_get_MNCC_SETUP_IND(MNCC_SETUP_IND_T *message_ptr);

VOID_FUNC   MN_get_MNCC_DISC_IND(MNCC_DISC_IND_T           *message_ptr, 
                                 ie_cm_cc_cause_T         *cm_cc_cause_ptr,
                                 cm_progress_indicator_T  *cm_prog_ind_ptr
#ifdef FEATURE_CCBS
                                 ,ie_cm_allowed_actions_T *allowed_actions_ptr
#endif /* FEATURE_CCBS */
                                 );

VOID_FUNC   MN_get_MNCC_FACILITY_IND(byte *data_ptr,
                                     word message_length,
                                     MN_FACILITY_DATA_T *facility_data,
                                     byte *last_octet_data_pointer);

int    MN_get_bearer_capability_data(byte                     *data_ptr, 
                                     cm_bearer_capability_T  *cm_bearer_capability,
                                     connection_id_T         connection_id,
                                     byte *last_octet_data_ptr);

int    MN_get_cause_data(byte               *data_ptr, 
                         ie_cm_cc_cause_T  *cm_cc_cause,
                         byte                      *last_octet_data_ptr);

int    MN_get_llc_data(byte                            *data_ptr, 
                       cm_low_layer_compatibility_T   *cm_low_layer_compatibility,
                       byte                                       *last_octet_data_ptr);

int    MN_get_hlc_data(byte                            *data_ptr, 
                       cm_high_layer_compatibility_T  *cm_high_layer_compatibility,
                       byte *last_octet_data_ptr);
#ifdef FEATURE_CCBS
void MN_fill_setup_from_mn (cm_mo_normal_call_req_T *msg, byte connection_id);
#endif /* FEATURE_CCBS */

boolean   MN_put_call_on_hold(connection_id_T     connection_id);

VOID_FUNC   MN_execute_buildmpty_success(MN_FACILITY_DATA_T  *facility_data, 
                                         int                 component_number);

VOID_FUNC   MN_execute_buildmpty_error(MN_FACILITY_DATA_T    *facility_data, 
                                       int                   component_number,
                                       byte                  component_type);

VOID_FUNC   MN_execute_forward_charge_advice(MN_FACILITY_DATA_T  *facility_data_ptr,
                                             int                 component_number);

VOID_FUNC   MN_execute_holdmpty_success(MN_FACILITY_DATA_T  *facility_data, 
                                        int                 component_number);

VOID_FUNC   MN_execute_holdmpty_error(MN_FACILITY_DATA_T    *facility_data_ptr,
                                      int                   component_number,
                                      byte                  component_type);

VOID_FUNC   MN_execute_retrievempty_success(MN_FACILITY_DATA_T  *facility_data, 
                                            int                 component_number);

VOID_FUNC   MN_execute_retrievempty_error(MN_FACILITY_DATA_T    *facility_data_ptr, 
                                          int                   component_number,
                                          byte                  component_type);

VOID_FUNC   MN_execute_callDeflection_error(MN_FACILITY_DATA_T    *facility_data_ptr, 
                                          int                   component_number,
                                          byte                  component_type);

VOID_FUNC   MN_execute_splitmpty_success(MN_FACILITY_DATA_T     *facility_data, 
                                         int                    component_number);

VOID_FUNC   MN_execute_splitmpty_error(MN_FACILITY_DATA_T       *facility_data_ptr, 
                                       int                      component_number,
                                       byte                     component_type);

#ifdef FEATURE_CCBS
VOID_FUNC   MN_execute_accessRegisterCCEntry_success( MN_FACILITY_DATA_T *facility_data, int component_number);
VOID_FUNC   MN_execute_accessRegisterCCEntry_error( MN_FACILITY_DATA_T *facility_data, int component_number,
                                                   ie_cm_ss_error_T   *cm_ss_error);
#endif /* FEATURE_CCBS */

VOID_FUNC   MN_connect_TCH(byte  information_transfer_capability);

boolean    MN_call_over(connection_id_T connection_id);

boolean    MN_compare_bearer_capability(cm_bearer_capability_T  *bearer_capability_1,
                                        cm_bearer_capability_T  *bearer_capability_2, int direction);

VOID_FUNC  MN_handle_MNCC_ALERT_IND(IMH_T *);

VOID_FUNC  MN_handle_MNCC_CALL_PROC_IND(IMH_T *message);

VOID_FUNC  MN_handle_MNCC_DISC_IND(IMH_T *message);

VOID_FUNC  MN_handle_MNCC_ERR_IND(
#if defined(FEATURE_DUAL_SIM )
   IMH_T *message
#else
   void
#endif
  );


#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
VOID_FUNC  MN_handle_MNCC_MODIFY_CNF(IMH_T *message);

VOID_FUNC  MN_handle_MNCC_MODIFY_IND(IMH_T *message);
#endif

VOID_FUNC  MN_handle_MNCC_NOTIFY_IND(IMH_T *message);

VOID_FUNC  MN_handle_MNCC_PROGRESS_IND(IMH_T *message);

VOID_FUNC  MN_handle_MNCC_REEST_IND(void);

VOID_FUNC  MN_handle_MNCC_REJ_IND(IMH_T *message);

VOID_FUNC  MN_handle_MNCC_REL_CNF(IMH_T *message);

VOID_FUNC  MN_handle_MNCC_REL_IND(IMH_T *message);

VOID_FUNC  MN_handle_MNCC_SYNC_IND(IMH_T *message);

VOID_FUNC  MN_handle_cm_abort_cc_req(cm_abort_cc_req_T *message_header);

VOID_FUNC  MN_handle_cm_mt_setup_res(IMH_T *message_header);

VOID_FUNC  MN_handle_cm_mt_call_res(IMH_T *message_header);

#ifdef FEATURE_CCBS
VOID_FUNC MN_handle_cm_recall_rsp(IMH_T *message_header);
#endif /* FEATURE_CCBS */

#if defined(FEATURE_DUAL_SIM )
VOID_FUNC MN_handle_cm_asid_2_vsid_mapping_ind(IMH_T *message_header);
#endif

VOID_FUNC MN_handle_cm_transfer_srvcc_call_context_res(IMH_T *message_header);

VOID_FUNC  MN_handle_mt_call_answer(IMH_T *message_header);

VOID_FUNC  MN_handle_mt_call_setup(IMH_T *message_header);

VOID_FUNC  MN_process_MNCC_CALL_HOLD_CNF(IMH_T   *message);

VOID_FUNC  MN_process_MNCC_CALL_HOLD_REJ(IMH_T   *message);

VOID_FUNC  MN_process_MNCC_CALL_RETRIEVE_CNF(IMH_T   *message);

VOID_FUNC  MN_process_MNCC_CALL_RETRIEVE_REJ(IMH_T   *message);

VOID_FUNC  MN_process_MNCC_FACILITY_IND(IMH_T    *message);

VOID_FUNC MN_process_MNCC_NOTIFY_DEFERRED_IND(IMH_T *message_ptr);

VOID_FUNC  MN_process_MNCC_SETUP_CNF(IMH_T    *message);

VOID_FUNC  MN_process_MNCC_START_DTMF_CNF(IMH_T    *message);

VOID_FUNC  MN_process_MNCC_STOP_DTMF_CNF(IMH_T    *message);

#ifdef FEATURE_UUS
VOID_FUNC  MN_process_MNCC_USER_USER_DATA_IND(IMH_T    *message);

VOID_FUNC  MN_process_MNCC_CONGESTION_IND(IMH_T    *message);
#endif

#ifdef FEATURE_CCBS
VOID_FUNC  MN_process_MNCC_RECALL_IND(IMH_T    *message);
VOID_FUNC  MN_process_MNCC_CC_EST_IND (IMH_T *message);
#endif /* FEATURE_CCBS */

int   MN_put_bearer_capability(byte                     *data_ptr, 
                               cm_bearer_capability_T  *cm_bearer_capability,
                               byte                    *last_octet_data_ptr);

int   MN_put_cause_data(byte               *data_ptr,
                        ie_cm_cc_cause_T  *cm_cc_cause,
                        byte  *last_octet_data_ptr);

int   MN_put_invoke_cd_data(byte *data_ptr, 
                            ie_cm_cc_invoke_T *cm_cc_invoke,
                            byte *last_octet_data_ptr);

int   MN_put_llc_data(byte                           *data_ptr, 
                      cm_low_layer_compatibility_T  *cm_low_layer_compatibility,
                      byte                          *last_octet_data_ptr);

int   MN_put_hlc_data(byte                           *data_ptr, 
                      cm_high_layer_compatibility_T *cm_high_layer_compatibility,
                      byte                          *last_octet_data_ptr);

boolean    MN_manage_CC_auxiliary_state(connection_id_T  connection_id,
                                        int              c_call_event);

byte       MN_request_invoke_id(byte   ss_operation_code);

byte       MN_retrieve_ss_operation(byte     invoke_id, 
                                    boolean  delete_entry);

byte       MN_retrieve_ss_connection_id(byte     invoke_id, 
                                    boolean  delete_entry);

VOID_FUNC  MN_retrieve_call(byte Multicall_Status_index);

boolean MN_reserve_call_object(connection_id_T new_connection_id );

VOID_FUNC  MN_clear_call_data(connection_id_T  connection_id);

VOID_FUNC  MN_put_cm_abort_cc_conf(
#if defined(FEATURE_DUAL_SIM )
   sys_modem_as_id_e_type              as_id
#else
   void
#endif
  );
return_size_T MN_get_element_length2(byte*, int, int);

return_size_T        MN_get_element_length(byte *,int, int);

byte        MN_put_charge_advice(byte                  *position, 
                                 charge_advice_data_T  *advice);

boolean     MN_check_for_inband_message(connection_id_T);

boolean MN_check_for_inband_message_subs(connection_id_T connection_id, sys_modem_as_id_e_type as_id);

/********************************************/
/**** Supplementary Services functions   ****/
/********************************************/


boolean MN_get_bsg_list(byte *, int *,basic_service_group_list_T *,byte *);

boolean MN_get_ff_list(byte *, int *,ie_forwarding_feature_list_T *, byte *last_octet_parameters_buf);

#ifdef FEATURE_CCBS
boolean MN_get_ccbs_feature (byte *param, int *msg_idx, ie_ccbs_indicator_T *ccbs, byte *last_octet_data_pointer);
boolean MN_get_ccbs_list(byte *, int *, ccbs_info_T *, byte *last_octet_parameters_buf);
#endif /* FEATURE_CCBS */

boolean MN_get_ft_number(byte *, int *,cm_called_party_bcd_no_T *, int);

boolean MN_get_ect_indicator(ie_cm_notify_ss_T *notify_data, byte *parameter_pointer, int *notify_index, byte *last_octet_data_pointer );

boolean MN_get_name_indicator(ie_cm_notify_ss_T *notify_data, byte *parameter_pointer, int *notify_index, byte *last_octet_data_pointer );
#ifndef FEATURE_CCBS
boolean MN_get_ccbs_indicator(ie_cm_notify_ss_T *notify_data, byte *parameter_pointer, int *notify_index, byte *last_octet_data_pointer );
#endif
VOID_FUNC MN_get_ss_status(ie_ss_status_T *, byte *);


boolean MN_release_ci_invoke_id(byte, byte, connection_id_T *,boolean);

boolean MN_request_ci_invoke_id(byte *, 
                                connection_id_T *, 
                                boolean *, 
                                byte , 
                                byte );

VOID_FUNC MN_select_ss_error(byte, 
                             byte, 
#if defined(FEATURE_DUAL_SIM )
                             sys_modem_as_id_e_type,
#endif
                             ie_cm_ss_error_T);

VOID_FUNC MN_get_MNSS_BEGIN_IND(MNSS_BEGIN_IND_T *);

VOID_FUNC MN_get_MNSS_END_IND(MNSS_END_IND_T *);

VOID_FUNC MN_get_MNSS_FACILITY_IND(MNSS_FACILITY_IND_T *);

VOID_FUNC MN_get_MNSS_REJ_IND(MNSS_REJ_IND_T *);

VOID_FUNC MN_put_cm_interrogate_ss_conf(boolean, 
                                        byte ,
#if defined(FEATURE_DUAL_SIM )
                                        sys_modem_as_id_e_type,
#endif
                                        cm_ss_conf_e_type, 
                                        InterrogateSS_Res_T *);

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
VOID_FUNC MN_put_cm_lcs_molr_conf(boolean ss_success, 
                                  byte cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                                  sys_modem_as_id_e_type as_id,
#endif 
                                  lcs_molr_res_T *lcs_molr_res);
#endif

VOID_FUNC MN_put_cm_register_ss_conf(boolean, 
                                     byte , 
#if defined(FEATURE_DUAL_SIM )
                                     sys_modem_as_id_e_type,
#endif
                                     ss_info_T *);

VOID_FUNC MN_put_cm_erase_ss_conf(boolean,
                                  byte,
#if defined(FEATURE_DUAL_SIM )
                                  sys_modem_as_id_e_type,
#endif
                                  ss_info_T *);

VOID_FUNC MN_put_cm_activate_ss_conf(boolean, 
                                     byte,
#if defined(FEATURE_DUAL_SIM )
                                     sys_modem_as_id_e_type,
#endif
                                     ss_info_T *);

VOID_FUNC MN_put_cm_deactivate_ss_conf(boolean,
                                       byte, 
#if defined(FEATURE_DUAL_SIM )
                                       sys_modem_as_id_e_type,
#endif
                                       ss_info_T *);

VOID_FUNC MN_put_cm_register_password_conf(boolean,
                                           byte,
#if defined(FEATURE_DUAL_SIM )
                                           sys_modem_as_id_e_type,
#endif
                                           ss_password_info_T *);

VOID_FUNC MN_put_cm_process_unstructured_ss_data_conf(boolean, 
                                                      byte,
#if defined(FEATURE_DUAL_SIM )
                                                      sys_modem_as_id_e_type,
#endif
                                                      ss_UserData_info_T *);

VOID_FUNC MN_put_cm_process_unstructured_ss_conf(boolean, 
                                                 byte,
#if defined(FEATURE_DUAL_SIM )
                                                 sys_modem_as_id_e_type,
#endif
                                                 ss_data_info_T *);

VOID_FUNC MN_handle_error(MN_FACILITY_DATA_T *, 
                          byte,
#if defined(FEATURE_DUAL_SIM )
                          sys_modem_as_id_e_type,
#endif
                          byte);

VOID_FUNC MN_handle_invoke(MN_FACILITY_DATA_T *
#if defined(FEATURE_DUAL_SIM )
                             , sys_modem_as_id_e_type
#endif

);

VOID_FUNC MN_handle_cm_abort_ss_req(cm_abort_ss_req_T *message);

VOID_FUNC MN_handle_cm_activate_ss_req(cm_activate_ss_req_T *);

VOID_FUNC MN_handle_cm_deactivate_ss_req(cm_deactivate_ss_req_T *);

VOID_FUNC MN_handle_cm_erase_ss_req(cm_erase_ss_req_T *);

VOID_FUNC MN_handle_cm_unstructured_ss_res(cm_unstructured_ss_res_T *);

VOID_FUNC MN_handle_cm_unstructured_ss_notify_res(cm_unstructured_ss_notify_res_T *);

VOID_FUNC MN_handle_cm_process_unstructured_ss_data(cm_process_unstructured_ss_data_T *);

VOID_FUNC MN_handle_cm_process_unstructured_ss_req(cm_process_unstructured_ss_req_T *);

VOID_FUNC MN_handle_cm_release_ss_req(cm_release_ss_req_T *);

VOID_FUNC MN_handle_cm_interrogate_ss_req(cm_interrogate_ss_req_T *);

VOID_FUNC MN_handle_cm_register_password_req(cm_register_password_req_T *);

VOID_FUNC MN_handle_cm_register_ss_req(cm_register_ss_req_T *);

#ifdef FEATURE_UUS
VOID_FUNC MN_handle_cm_activate_user_data_req(cm_mo_user_data_req_T *);
#endif

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
VOID_FUNC MN_handle_cm_lcs_molr_req(cm_lcs_molr_req_T *message_ptr);
VOID_FUNC MN_handle_cm_lcs_molr_completed(cm_lcs_molr_compl_T *message_ptr);
#endif

#ifdef FEATURE_CCBS
VOID_FUNC MN_handle_cm_erase_cc_entry_req(cm_erase_cc_entry_req_T *message_ptr);
#endif /* FEATURE_CCBS */

VOID_FUNC MN_put_cm_unstructured_ss_ind(byte, 
#if defined(FEATURE_DUAL_SIM )
                                        sys_modem_as_id_e_type ,
#endif
                                        uss_data_T *);

VOID_FUNC MN_put_cm_unstructured_ss_notify_ind(byte, 
#if defined(FEATURE_DUAL_SIM )
                                        sys_modem_as_id_e_type ,
#endif
                                               uss_data_T *);

VOID_FUNC MN_put_facility(byte *, ie_facility_T *, byte *last_octet_component_pointer);

VOID_FUNC MN_put_invoke(byte *, int, byte, byte, byte *);

int MN_put_ss_ForBS(ss_ForBS_T, byte *);

VOID_FUNC MN_put_error(ie_cm_ss_error_T, byte, byte *, int, byte *);

boolean   MN_handle_result(const MN_FACILITY_DATA_T *, 
                           byte, 
#if defined(FEATURE_DUAL_SIM )
                           sys_modem_as_id_e_type ,
#endif
                           byte);

VOID_FUNC MN_select_cc_cause(byte, 
                             byte, 
#if defined(FEATURE_DUAL_SIM )
                             sys_modem_as_id_e_type ,
#endif

                             ie_cm_cc_cause_T);

VOID_FUNC MN_put_reject(ss_reject_T, byte, byte *);

int MN_put_registerSS_ARG(byte *, registerSS_ARG_T);

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
int MN_put_lcs_molr_ARG(byte *argument, lcs_molr_ARG_T *lcs_molr_ARG);
#endif

#ifdef FEATURE_CCBS
int MN_put_eraseCCEntry_ARG (byte *, cm_erase_cc_entry_req_T *);
#endif /* FEATURE_CCBS */

int MN_put_ss_code(byte, byte *);

int MN_put_ussd_Res(uss_data_T *, byte *, byte);

int MN_put_ussd_Arg(uss_data_T, byte *, byte);

VOID_FUNC MN_put_MNSS_END_REQ(connection_id_T, ie_facility_T *);

VOID_FUNC MN_put_MNSS_BEGIN_REQ(connection_id_T, ie_facility_T *);

VOID_FUNC MN_put_MNSS_FACILITY_REQ(connection_id_T, ie_facility_T *);

boolean   MN_handle_ss_info(byte *, ss_info_T *, byte *last_octet_parameters_buf);

VOID_FUNC MN_execute_ss_notify(MN_FACILITY_DATA_T *, int);

VOID_FUNC MN_put_crss_reject(byte, byte, connection_id_T, byte);

boolean   MN_load_invoke_id(connection_id_T, byte, byte,byte);


VOID_FUNC MN_put_cm_get_password_ind(byte, 
#if defined(FEATURE_DUAL_SIM )
                             sys_modem_as_id_e_type ,
#endif
                                     byte);


VOID_FUNC MN_put_cm_forward_check_ss_ind(
#if defined(FEATURE_DUAL_SIM )
                           sys_modem_as_id_e_type 
#else
  void
#endif
                                 );

VOID_FUNC   MN_put_cm_forward_check_or_release_USSD_ss_ind(
                           byte                  message_id
#if defined(FEATURE_DUAL_SIM )
                           ,sys_modem_as_id_e_type as_id
#endif
);

int       MN_encode_error_parameters(ie_cm_ss_error_T  *, byte *);

boolean   MN_handle_interrogate_ss_res(byte *, cm_ss_conf_e_type *, InterrogateSS_Res_T *, byte *last_octet_parameters_buf);

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
boolean MN_handle_lcs_molr_res(byte *param_ptr, lcs_molr_res_T *lcs_molr_res, byte *last_octet_parameters_buf);
VOID_FUNC MN_handle_cm_lcs_location_notification_res(cm_lcs_location_notification_res_T *message_ptr);
#endif

VOID_FUNC   MN_handle_cm_get_password_res(cm_get_password_res_T *);

VOID_FUNC   MN_put_element_length(byte *, int *, int);

VOID_FUNC   MN_decode_error_parameters(ie_cm_ss_error_T *, MN_FACILITY_DATA_T *);

VOID_FUNC   MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
                           sys_modem_as_id_e_type 
#else
  void
#endif
                                 );


VOID_FUNC   MN_handle_cm_mt_call_ss_res(cm_mt_call_ss_res_T *);

VOID_FUNC   MN_put_cm_connection_state_ind(
#if defined(FEATURE_DUAL_SIM )
          sys_modem_as_id_e_type       as_id,
#endif
           boolean);

boolean     MN_check_for_parallel_SS(connection_id_T);

boolean     MN_find_associated_invoke_id(byte , byte , byte  *);

boolean     MN_release_ci_invoke_id_end(connection_id_T );

VOID_FUNC   MN_release_invoke_ids(connection_id_T );

boolean     MN_release_mo_invoke_id(byte, byte, connection_id_T *, boolean);

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
VOID_FUNC MN_put_cm_lcs_location_notification_ind(byte  invoke_id, 
#if defined(FEATURE_DUAL_SIM )
                                                sys_modem_as_id_e_type as_id,
#endif
                                                  location_notification_T *location_notification);
#endif

/********************************************/
/**** Short Message Services functions   ****/
/********************************************/

control_sm_rl_return_data_T
      MN_control_SMS_RL(connection_id_T   connection_id, 
                        byte              sms_rl_stimulus);

VOID_FUNC  MN_handle_MNSMS_DATA_IND(IMH_T *message_header);

VOID_FUNC  MN_handle_MNSMS_ERROR_IND(IMH_T *message_header);

VOID_FUNC  MN_handle_MNSMS_EST_IND(IMH_T *message_header);

VOID_FUNC  MN_handle_MNSMS_CPACK_IND(IMH_T *message_header);

connection_id_T MN_get_new_sms_connection_id(void);

#if defined(FEATURE_DUAL_SIM )
boolean MN_validate_sms_as_id(sys_modem_as_id_e_type    as_id, 
                                   word  short_message_identifier, 
                                   boolean send_rsp);
#endif

byte            MN_get_rl_mo_mr(void);

VOID_FUNC       MN_get_rp_report(SM_RL_REPORT_REQ_T *);

VOID_FUNC       MN_get_cp_user_data(connection_id_T   connection_id,
                                    int               message_length,
                                    rpdu_T            cp_user_data,
									nas_msg_transport_type_e_type    sms_transport_type);

VOID_FUNC   MN_put_rp_ack(byte              rel_req,
                          connection_id_T   connection_id,
                          SM_RL_REPORT_REQ_T *SM_RL_REPORT_REQ);

VOID_FUNC   MN_put_rp_error( rp_cause_T      rp_cause,
                             connection_id_T connection_id,
                             byte            rel_req,
                             SM_RL_REPORT_REQ_T *SM_RL_REPORT_REQ);

VOID_FUNC   MN_put_uasms_sm_data_ind(SM_RL_DATA_IND_T *SM_RL_DATA_IND);

VOID_FUNC   MN_put_uasms_sm_report_ind(SM_RL_REPORT_IND_T *SM_RL_REPORT_IND);

VOID_FUNC   MN_put_uasms_link_control_report_ind (
                                    sms_link_control_mode_T mode, 
#if defined(FEATURE_DUAL_SIM )
                                    sys_modem_as_id_e_type as_id,
#endif
                                   sms_link_control_status_T status);

VOID_FUNC   MN_process_SM_RL_DATA_REQ(SM_RL_DATA_REQ_T *message_ptr);

VOID_FUNC   MN_process_SM_RL_MEM_AVAIL_REQ(word short_message_identifier
#if defined(FEATURE_DUAL_SIM )
                                                       ,sys_modem_as_id_e_type  as_id
#endif
                                                       ,nas_msg_transport_type_e_type    sms_transport_type
  );

VOID_FUNC   MN_process_SM_RL_LINK_CONTRL_REQ(SM_RL_LINK_CONTROL_REQ_T *sms_link_control_req);


VOID_FUNC   MN_put_MNSMS_ABORT_REQ(connection_id_T   connection_id, 
                                   byte              cause_value);

VOID_FUNC   MN_put_MNSMS_DATA_REQ(connection_id_T   connection_id,
                                  byte              *rpdu,
                                  byte              rpdu_length);

VOID_FUNC   MN_put_MNSMS_EST_REQ(void);

VOID_FUNC   MN_put_MNSMS_REL_REQ(connection_id_T  connection_id,
                                 byte             cause_value);

char *  show_rl_state( byte rl_state );

/***********************************/
/**** Timer control functions   ****/
/***********************************/

VOID_FUNC  MN_handle_timeouts(IMH_T *message);

VOID_FUNC  MN_start_cc_timer(connection_id_T connection_id, byte message_IEI);

VOID_FUNC  MN_start_ss_timer(connection_id_T connection_id, byte invoke_id, milli_secs_T time);

VOID_FUNC  MN_stop_cc_timer(connection_id_T connection_id);

VOID_FUNC  MN_stop_ss_timer(byte invoke_id);

VOID_FUNC  MN_start_sms_timer(connection_id_T , byte);

VOID_FUNC  MN_stop_sms_timer(connection_id_T , byte);

char * get_timer_name(timer_id_T timer_id);


/***********************************/
/**** Miscellaneous functions   ****/
/***********************************/

byte        EncodeAsnInt ( byte *buf_ptr, unsigned long int data);
gs_status_T MN_send_message( gs_queue_id_T, void *, boolean );
VOID_FUNC   MN_handle_test(IMH_T   *message);
byte        MN_whereami(connection_id_T connection_id);
byte        MN_whichMultiCallIndex (connection_id_T connection_id);
byte        MN_num_of_calls_in_group(int group);

#ifdef FEATURE_UUS
cm_mo_user_data_req_T * MN_alloc_user_data(void);
void                    MN_free_user_data(cm_mo_user_data_req_T *user_data);
cm_mo_user_data_req_T * MN_get_user_data(connection_id_T connection_id);
void                    MN_put_user_data(connection_id_T connection_id, cm_mo_user_data_req_T *user_data);
#endif


/***********************************/
/**** QXDM logging functions    ****/
/***********************************/
boolean mn_log_cs_call_setup_info(uint8 connection_id, uint8 call_type, uint8 orig_direction);
boolean mn_log_cs_call_release_info(uint8 connection_id, uint8 release_cause);
boolean mn_log_cs_call_change_info(uint8 connection_id);
boolean mn_log_cs_connection_bc_info(uint8 connection_id, 
                                     cm_bearer_capability_T bearer_capability);

/**************/
/* F3 logging */
/**************/

VOID_FUNC      mn_log_message(gs_queue_id_T queue_id,IMH_T *message_p);

#define MN_MULTICALL_LOG(toomany_calls)  { \
                                           if(toomany_calls) \
                                              MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN_read_multicall_status() got too many calls\n"\
                                              );}

VOID_FUNC    MN_flush_queued_dtmf_requests(connection_id_T  connection_id);
VOID_FUNC    MN_handle_ALT_proc_upon_call_release(connection_id_T  connection_id
#if defined(FEATURE_DUAL_SIM )
, sys_modem_as_id_e_type as_id
#endif
                                                  );

#if defined(FEATURE_DUAL_SIM )
boolean MN_validate_ss_as_id(sys_modem_as_id_e_type as_id, byte cm_ss_ref, byte ss_operation, boolean send_rsp);
#endif

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
void mn_store_rab_modify_status(void);
#endif
void mn_store_rab_status ( void );

byte mn_get_no_auto_answer_on_hold(void);
byte mn_get_mt_call_reject_cause(void);
byte mn_get_als_itc_aux_speech(void);

boolean MN_is_MO_SMS_in_progress(void);

void CNM_set_buffer_transient_triggers(srvcc_trigger_e_type trigger, connection_id_T connection_id);
void CNM_reset_buffer_transient_triggers(void);
#endif
