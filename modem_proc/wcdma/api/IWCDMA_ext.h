/**
  @file IWCDMA_ext.h

  @brief  This is the header file that describes an interface to a component

*/
/*
  Copyright (c) 2014 Qualcomm Technologies, Inc.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/IWCDMA_ext.h#1 $
  $DateTime: 2016/12/13 08:00:12 $


 when         who     what, where, why
 ----------   ---     ------------------------------------------------------------
 07/01/14     hk      Header file inclusion cleanup
 10/17/13     hzhi    Support new FEATURE_DUAL_SIM on DIME3.0. 
 10/09/13     hzhi    Moved private api pointers for RFTECH_WCDMA/MMCP/Qchat here.
 01/01/13     ngeng   Created for segment loading.

===========================================================================*/
#ifndef _IWCDMA_EXT_H_
#define _IWCDMA_EXT_H_

#include "mcfg_seg_load.h"

/*======================================================================================
  editted by hzhi on 09/23/2013 to update WCDMA interface on DIME3.0 for segment loading. 
  ======================================================================================*/
#include "wcdma_common_api.h"
#include "l1lsmif.h"
#include "l1task.h"
#include "rrcintif.h"
#include "rrccmd.h"

#include "rrccu.h"	//non-public
#include "rrccui.h"	//non-public
#include "rrctmr.h"	//non-public
#include "seq.h"	//non-public
#include "l1extif.h"

/*=====================================================================================*/

/*  The component that is being used provides typedefs for pointers to each
    of its exported functions. 
*/

typedef void 
  (*fn_dl_dec_deregister_ctch_pdu_decode_error_cb)
    (
      void
    );	//private

typedef void 
  (*fn_dl_dec_register_ctch_pdu_decode_error_cb)
    (
      DL_DEC_CTCH_PDU_DECODE_ERROR_CB_FUNC_TYPE ctch_pdu_decode_error_cb
    );	//private

typedef boolean 
  (*fn_mdsp_sync_send_start_stop_cmd)
    (
      boolean start,
      mdsp_sync_start_cfg_enum_type start_mode,
      pre_cb_fn pre_cb,
      post_cb_fn post_cb
    );	//private

typedef void (*fn_drx_manager_wake_from_sleep)(void);	//private
typedef void (*fn_drx_sleep_abort)(void);		//private
typedef void (*fn_l1m_setup_next_cmd) ( void );		//private
typedef rrc_new_cell_status_e_type (*fn_rrc_ccm_get_serv_plmn_cellid)
(
 rrc_int_cell_id_type *cell_id, 
 rrc_plmn_identity_type *plmn_id
);	//private
typedef rrc_cmd_type * ( *fn_rrc_get_int_cmd_buf) (void);	//private
typedef rrc_state_e_type (*fn_rrc_get_state) ( void );		//private
typedef void (*fn_rrc_put_int_cmd)
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);	//private

#ifdef FEATURE_HSPA_CALL_STATUS_IND
typedef void (*fn_rrc_send_hspa_call_status_info) (void);   //private
#endif /* FEATURE_HSPA_CALL_STATUS_IND */

typedef void (*fn_rrccu_clear_procedure)(void);             //private

typedef uint32 (*fn_rrccu_convert_t302_to_ms)
(
  rrc_T_302 rrccu_t_302                    /* enum for Timer T302 */
);  //private

typedef rrccu_substate_e_type (*fn_rrccu_return_cu_substate) (void);//private

typedef void (*fn_rrccu_send_cell_update_initiate_req)
(
  rrc_cell_update_cause_e_type cause
);  //private

typedef rrccu_status_e_type (*fn_rrccu_send_cell_update_msg)
(    
  rlc_lc_id_type ul_lc_id,                 /* Logical Channel Id */
  rrccu_failure_status_e_type status       /* Indicates whether to include
                                              failure cause or not */
);  //private

typedef void (*fn_rrccu_set_cu_substate)
(
  rrccu_substate_e_type state
);  //private
typedef rlc_lc_id_type (*fn_rrclcm_check_ul_rlc_lc_id)
( 
  uecomdef_logchan_e_type ul_lc_type,
  rrc_RB_Identity rb_id,          
  uecomdef_logch_mode_e_type mode      
);  //private
typedef uint64 (*fn_rrctmr_get_remaining_time)
(
  rrctmr_timer_e_type timer                 /* Timer to stop */
);  //private
typedef void (*fn_rrctmr_start_timer)
(
  rrctmr_timer_e_type timer,                /* Timer to start */
  unsigned long timer_value                 /* Value for timer */                      
);  //private
typedef seq_align_struct_type (*fn_seq_get_align_type) ( void );        //private
typedef void (*fn_wl1_send_msg_to_controller) (rrc_cmd_type *cmd_ptr);  //private
typedef void 
  (*fn_sleep_abort)
    (
      void
    );  //private

typedef void (*fn_l1_free_cmd_buf)
(
  /* Command buffer to be freed */
  l1_ext_cmd_type* cmd_ptr
);

/*  The component then provides a vtable beginning with add_ref, release,
    and qi, followed by the functions it exports.
*/
/*  The component then provides a vtable beginning with add_ref, release,
    and qi, followed by the functions it exports.
*/
typedef struct
{
  add_ref_fn  add_ref;
  release_fn  release;
  qi_fn       qi;
  fn_dl_dec_deregister_ctch_pdu_decode_error_cb dl_dec_deregister_ctch_pdu_decode_error_cb;
  fn_dl_dec_register_ctch_pdu_decode_error_cb dl_dec_register_ctch_pdu_decode_error_cb;
} iWCDMA_mmcp_vtbl;

/*  The component provides a macro to get the pointer to its vtable from
    a pointer to an interface (defined in dynload.h).
*/
#define WCDMA_mmcp_vtbl(pi)   ((iWCDMA_mmcp_vtbl *) ((pi)->pVtable))
#define IWCDMA_dl_dec_deregister_ctch_pdu_decode_error_cb(pi) (WCDMA_mmcp_vtbl(pi))->dl_dec_deregister_ctch_pdu_decode_error_cb()
#define IWCDMA_dl_dec_register_ctch_pdu_decode_error_cb(pi,ctch_pdu_decode_error_cb) (WCDMA_mmcp_vtbl(pi))->dl_dec_register_ctch_pdu_decode_error_cb(ctch_pdu_decode_error_cb)


/*  The component then provides a vtable beginning with add_ref, release,
    and qi, followed by the functions it exports.
*/
typedef struct
{
  add_ref_fn  add_ref;
  release_fn  release;
  qi_fn       qi;
  fn_mdsp_sync_send_start_stop_cmd mdsp_sync_send_start_stop_cmd;
} iWCDMA_rftech_vtbl;

/*  The component provides a macro to get the pointer to its vtable from
    a pointer to an interface (defined in dynload.h).
*/
#define WCDMA_rftech_vtbl(pi)   ((iWCDMA_rftech_vtbl *) ((pi)->pVtable))
#define IWCDMA_mdsp_sync_send_start_stop_cmd(pi,start,start_mode,pre_cb,post_cb) (WCDMA_rftech_vtbl(pi))->mdsp_sync_send_start_stop_cmd(start,start_mode,pre_cb,post_cb)

/*  The component then provides a vtable beginning with add_ref, release,
    and qi, followed by the functions it exports.
*/
typedef struct
{
  add_ref_fn  add_ref;
  release_fn  release;
  qi_fn       qi;
  fn_drx_manager_wake_from_sleep drx_manager_wake_from_sleep;  
  fn_drx_sleep_abort drx_sleep_abort;
  fn_l1m_setup_next_cmd l1m_setup_next_cmd;
  fn_rrc_ccm_get_serv_plmn_cellid rrc_ccm_get_serv_plmn_cellid;
  fn_rrc_get_int_cmd_buf rrc_get_int_cmd_buf;
  fn_rrc_get_state rrc_get_state;
  fn_rrc_put_int_cmd rrc_put_int_cmd;
#ifdef FEATURE_HSPA_CALL_STATUS_IND
  fn_rrc_send_hspa_call_status_info rrc_send_hspa_call_status_info;
#endif /* FEATURE_HSPA_CALL_STATUS_IND */
  fn_rrccu_clear_procedure rrccu_clear_procedure;
  fn_rrccu_convert_t302_to_ms rrccu_convert_t302_to_ms;
  fn_rrccu_return_cu_substate rrccu_return_cu_substate;
  fn_rrccu_send_cell_update_initiate_req rrccu_send_cell_update_initiate_req;
  fn_rrccu_send_cell_update_msg rrccu_send_cell_update_msg;
  fn_rrccu_set_cu_substate rrccu_set_cu_substate;
  fn_rrclcm_check_ul_rlc_lc_id rrclcm_check_ul_rlc_lc_id;
  fn_rrctmr_get_remaining_time rrctmr_get_remaining_time;
  fn_rrctmr_start_timer rrctmr_start_timer;
  fn_seq_get_align_type seq_get_align_type;
  fn_wl1_send_msg_to_controller wl1_send_msg_to_controller;
  fn_sleep_abort sleep_abort;
  fn_l1_free_cmd_buf l1_free_cmd_buf;
} iWCDMA_qchat_vtbl;

/*  The component provides a macro to get the pointer to its vtable from
    a pointer to an interface (defined in dynload.h).
*/
#define WCDMA_qchat_vtbl(pi)   ((iWCDMA_qchat_vtbl *) ((pi)->pVtable))
#define IWCDMA_drx_manager_wake_from_sleep(pi)     (WCDMA_qchat_vtbl(pi))->drx_manager_wake_from_sleep()
#define IWCDMA_drx_sleep_abort(pi)                 (WCDMA_qchat_vtbl(pi))->drx_sleep_abort()
#define IWCDMA_l1m_setup_next_cmd(pi)               (WCDMA_qchat_vtbl(pi))->l1m_setup_next_cmd()
#define IWCDMA_rrc_ccm_get_serv_plmn_cellid(pi,cell_id,plmn_id) (WCDMA_qchat_vtbl(pi))->rrc_ccm_get_serv_plmn_cellid(cell_id,plmn_id)
#define IWCDMA_rrc_get_int_cmd_buf(pi) (WCDMA_qchat_vtbl(pi))->rrc_get_int_cmd_buf() 
#define IWCDMA_rrc_get_state(pi)         (WCDMA_qchat_vtbl(pi))->rrc_get_state()
#define IWCDMA_rrc_put_int_cmd(pi,cmd_buf) (WCDMA_qchat_vtbl(pi))->rrc_put_int_cmd(cmd_buf)
#define IWCDMA_rrc_send_hspa_call_status_info(pi)   (WCDMA_qchat_vtbl(pi))->rrc_send_hspa_call_status_info()
#define IWCDMA_rrccu_clear_procedure(pi) (WCDMA_qchat_vtbl(pi))->rrccu_clear_procedure()
#define IWCDMA_rrccu_convert_t302_to_ms(pi,rrccu_t_302) (WCDMA_qchat_vtbl(pi))->rrccu_convert_t302_to_ms(rrccu_t_302)
#define IWCDMA_rrccu_return_cu_substate(pi) (WCDMA_qchat_vtbl(pi))->rrccu_return_cu_substate()
#define IWCDMA_rrccu_send_cell_update_initiate_req(pi,cause) (WCDMA_qchat_vtbl(pi))->rrccu_send_cell_update_initiate_req(cause)
#define IWCDMA_rrccu_send_cell_update_msg(pi,ul_lc_id,status) (WCDMA_qchat_vtbl(pi))->rrccu_send_cell_update_msg(ul_lc_id,status)
#define IWCDMA_rrccu_set_cu_substate(pi,state) (WCDMA_qchat_vtbl(pi))->rrccu_set_cu_substate(state)
#define IWCDMA_rrclcm_check_ul_rlc_lc_id(pi,ul_lc_type,rb_id,mode)        (WCDMA_qchat_vtbl(pi))->rrclcm_check_ul_rlc_lc_id(ul_lc_type,rb_id,mode)
#define IWCDMA_rrctmr_get_remaining_time(pi,timer) (WCDMA_qchat_vtbl(pi))->rrctmr_get_remaining_time(timer)
#define IWCDMA_rrctmr_start_timer(pi,timer_timer_value) (WCDMA_qchat_vtbl(pi))->rrctmr_start_timer(timer_timer_value)
#define IWCDMA_seq_get_align_type(pi)                      (WCDMA_qchat_vtbl(pi))->seq_get_align_type()
#define IWCDMA_wl1_send_msg_to_controller(pi,cmd_ptr) (WCDMA_qchat_vtbl(pi))->wl1_send_msg_to_controller(cmd_ptr)
#define IWCDMA_sleep_abort(pi) (WCDMA_qchat_vtbl(pi))->sleep_abort()
#define IWCDMA_l1_free_cmd_buf(pi,cmd_ptr) (WCDMA_qchat_vtbl(pi))->l1_free_cmd_buf(cmd_ptr)
#endif /* _IWCDMA_EXT_H_ */