#ifndef TM_UMTS_CP_SS_H
#define TM_UMTS_CP_SS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-CP-SS Header File

GENERAL DESCRIPTION
  Prototypes of functions defined in tm_umts_cp_ss.c and used by UMTS_CP and
  all UMTS CP protocols.
   
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/ss/inc/tm_umts_cp_ss.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $
  
  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/27/09   atien   Add SS MOLR Return Result error code constants here
  07/23/09   gk      Added handling of MOLR_COMPLETE
  05/03/07   LT      Initial version

============================================================================*/



#ifdef __cplusplus
extern "C" {
#endif

#define  SS_MOLR_ERRORS_SYSTEM_FAILURE 34
#define  SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE 36
#define  SS_MOLR_ERRORS_DATA_MISSING 35
#define  SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED 21
#define  SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION 19
#define  SS_MOLR_ERRORS_POSITION_METHOD_FAILURE 54
#define  SS_MOLR_ERRORS_UNDEFINED 0             

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              DATA TYPES                                 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              FUNCTION DECLARATIONS                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


void tm_umts_cp_ss_init (void);

void tm_umts_cp_ss_session_init(void);

boolean tm_umts_cp_ss_send_molr_invoke( prtl_start_sess_req_param_s_type*      start_sess_req_param,
                                        molr_type_e_type                       mo_lr_type,
                                        sm_InternalAssistDataStatusStructType* p_assist_data_status);

void tm_umts_cp_ss_lcs_molr_conf_proc (tm_cm_sups_data_s_type *p_sups_data);

void tm_umts_cp_ss_lcs_molr_proc(void);
void tm_umts_cp_ss_lcs_molr_comp_proc(void);

void tm_umts_cp_ss_lcs_loc_notif_ind_proc (tm_cm_sups_data_s_type* p_sup_data);

boolean tm_umts_ss_send_lcs_loc_notif_return_result(const pdsm_lcs_notification_resp_s_type *p_user_resp );

void tm_umts_cp_ss_ss_completion_req (void);

boolean tm_umts_cp_ss_is_molr_on_comp(void);


#ifdef __cplusplus
}
#endif

#endif /* TM_UMTS_CP_SS_H */

