#ifndef MM_COORD_H
#define MM_COORD_H
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm_coord.h_v   1.7   06 Jun 2002 10:14:20   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mm_coord.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/01   jca     Modified function prototypes.
04/24/01   sbk     Moved prototypes to mm_umts.h
05/23/01   jca     Externalized some mmcoord data items.
06/03/01   sbk     Externalized mmcoord_pending_reg_message
06/05/02   ks      Added support for Dual mode, consolidated the
                   mmcoord plmn,lac into one single Serving_plmn entity.
10/07/02   jca     Added regional variable mmcoord_mmr_reg_cnf.
11/25/03   jca     Added regional variable mmcoord_reg_req_pending.
11/24/11   gps     pended stop_mode_req until abort procedure completes
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "sys_cnst_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "rrccmd.h"
#include "mm_v.h"
#ifdef FEATURE_LTE
#include "emm_database.h"
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern mm_cmd_type          mmcoord_pending_reg_message_sim[MAX_NAS_STACKS];
extern mm_cmd_type          mmcoord_pending_reg_list_req_message_sim[MAX_NAS_STACKS];
extern mmr_reg_cnf_s_type   mmcoord_mmr_reg_cnf_sim[MAX_NAS_STACKS];
extern boolean              mmcoord_reg_req_pending_sim[MAX_NAS_STACKS];
extern boolean              mm_waiting_for_abort_cnf_sim[MAX_NAS_STACKS];
extern boolean              mm_stop_mode_req_pended_sim[MAX_NAS_STACKS];
extern boolean              mm_cell_changed_to_cs_capable_sim[MAX_NAS_STACKS];
extern boolean              mm_reg_req_revd_active_call_sim[MAX_NAS_STACKS];
extern boolean              mm_lu_pended_nmo_unknown_sim[MAX_NAS_STACKS];

#define  mmcoord_pending_reg_message mmcoord_pending_reg_message_sim[mm_as_id]
#define  mmcoord_pending_reg_list_req_message mmcoord_pending_reg_list_req_message_sim[mm_as_id]
#define  mmcoord_mmr_reg_cnf mmcoord_mmr_reg_cnf_sim[mm_as_id]
#define  mmcoord_reg_req_pending mmcoord_reg_req_pending_sim[mm_as_id]
#define  mm_waiting_for_abort_cnf mm_waiting_for_abort_cnf_sim[mm_as_id]
#define  mm_stop_mode_req_pended mm_stop_mode_req_pended_sim[mm_as_id]
#define  mm_reg_req_revd_active_call mm_reg_req_revd_active_call_sim[mm_as_id]

#define  mm_cell_changed_to_cs_capable (mm_cell_changed_to_cs_capable_sim[mm_as_id])
#define  mm_lu_pended_nmo_unknown mm_lu_pended_nmo_unknown_sim[mm_as_id]

#define MM_OTHER_SUB(as_id) ((as_id == MM_AS_ID_1) ? MM_AS_ID_2 : MM_AS_ID_1)

#if defined (FEATURE_WRLF_SYSTEM_SEL) || defined (FEATURE_LTE)
extern mm_rlf_stop_mode_pending_enum_T mm_rlf_stop_mode_pending_sim[MAX_NAS_STACKS];
#define mm_rlf_stop_mode_pending mm_rlf_stop_mode_pending_sim[mm_as_id]
#endif

#else

extern mm_cmd_type          mmcoord_pending_reg_message;
extern mm_cmd_type          mmcoord_pending_reg_list_req_message;
extern mmr_reg_cnf_s_type   mmcoord_mmr_reg_cnf;
extern boolean              mmcoord_reg_req_pending;
extern boolean mm_waiting_for_abort_cnf;
extern boolean mm_stop_mode_req_pended;
extern boolean mm_reg_req_revd_active_call;

#if defined (FEATURE_WRLF_SYSTEM_SEL) || defined (FEATURE_LTE)
extern mm_rlf_stop_mode_pending_enum_T mm_rlf_stop_mode_pending;
#endif
extern boolean              mm_cell_changed_to_cs_capable;
extern boolean              mm_lu_pended_nmo_unknown;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

extern boolean mm_reg_ind_sent;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern boolean mm_is_gsm_allowed_on_sub(void);
extern boolean mm_is_umts_allowed_on_sub(void);
#endif

#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)mm_sub_id)
#endif
#endif
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

void mmcoord_route_mm_message( mm_cmd_type *message,mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr );

boolean mmcoord_route_mm_emm_messages
(
 void                      *command_ptr, 
#ifdef FEATURE_LTE 
  emm_ctrl_data_type       *emm_ctrl_data, //Added this i/p parameter, just to make this function compatible to the function pointer
#endif
  mm_emm_dispatcher_fp     **mm_emm_dispatcher_fn_ptr
);

/*=================================================
           OOS/RLF function handlers
==================================================*/
#ifdef FEATURE_LTE 
boolean emm_process_oos_rlf_outgoing_esm_message
(  
  void                  *emm_message_router_cmd,
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
);

boolean emm_process_oos_rlf_rrc_primitive
(
  void                 *rrc_message_router_cmd,
  emm_ctrl_data_type   *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
);

boolean emm_get_reg_req_pending_status
(
  void
);
#endif

boolean mmcoord_route_oos_rlf_mm_emm_messages
(
  void                   *command_ptr,
#ifdef FEATURE_LTE    
  emm_ctrl_data_type     *emm_ctrl_data,
#endif
  mm_emm_dispatcher_fp   **mm_emm_dispatcher_fn_ptr
);
/*=================================================
           INTER_RAT function handlers
==================================================*/
#ifdef FEATURE_LTE 
boolean emm_process_irat_outgoing_esm_message
(  
  void                  *emm_message_router_cmd,
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
);

boolean emm_process_irat_rrc_primitive
(
  void                 *rrc_message_router_cmd,
  emm_ctrl_data_type   *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
);

#endif

/*=======================================================*/
void mm_send_mmcc_sig_conn_rel_ind( void );

#endif /* MM_COORD_H */
