#ifndef RRCCMD_V_H
#define RRCCMD_V_H

/*===========================================================================
                      R R C    C O M M A N D S

DESCRIPTION

  This module contains external definitions for the
  command queues used by RRC task. This file should be
  included by any external module sending commands to RRC's
  command queues.


Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrccmd_v.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/08/16   nr      CR973374: Made changes to not allow LTA if delay restriction IE is set
10/16/15   as      Added code to support LTA during activation time
11/12/14   sr      Code reduction to remove GET_ALL_SIBS handling
03/11/14   vi      Replaced feature FEATURE_PLI_NMR_UTRAN with FEATURE_WCDMA
05/30/11   rl      CMI de-featurization
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50 
12/02/10   geg     Created file

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "rrccmd.h"
#include "rrcdata_v.h"

/* Update rrc_cmd_int_q_u_type along with rrc_cmd_data_u_type 
   for new Internal commands
 */


typedef union
{

#ifdef FEATURE_WCDMA
  /* Union of individual command structures */
  /*--------------------------------------------------------*/
  /* Command structures for Internal RRC commands           */
  /*--------------------------------------------------------*/
  rrc_downlink_sdu_ind_type       downlink_sdu_ind;
  rrc_downlink_l2ack_ind_type     downlink_l2ack_ind;
  rrc_sib_event_cnf_type          get_sibs_cnf;
  rrc_get_specific_sibs_req_type  get_specific_sibs_req;
  rrc_redirect_req_type           redirect_req;
  rrc_redirect_cnf_type           redirect_cnf;
  rrc_new_state_ind_type          new_state_ind;
  rrc_state_change_ind_type       state_change_ind;
  rrc_cell_change_req_type        cell_change_req;
  rrc_cell_change_cnf_type        cell_change_cnf;
  rrc_new_cell_ind_type           new_cell_ind;
  rrc_llc_cmd_req_type            rrc_llc_req;
  rrc_llc_cmd_cnf_type            rrc_llc_cnf;
  rrc_channel_config_req_type     chan_config_req;
  rrc_channel_config_cnf_type     chan_config_cnf;
  rrc_update_sibs_ind_type        update_sibs_ind;
  rrc_initiate_ue_cap_req_type    initiate_ue_cap;
  rrc_initiate_cu_req_type        initiate_cell_update;
  rrc_initiate_uu_req_type        initiate_ura_update;
  rrc_cu_complete_cnf_type        cu_complete_cnf;
  rrc_disable_prev_freq_type      disable_prev_freq;
  rrc_cell_selected_meas_req_type cell_selected_meas_req;
  rrc_initiate_cell_sel_ind_type  initiate_cell_sel_ind;
  rrc_sib_change_ind_type         sib_change_ind;
  rrc_rnti_update_req_type        rnti_update_req;
  rrc_rnti_update_cnf_type        rnti_update_cnf;
  rrc_cell_reselection_req_type   cell_resel_req;
  rrc_cell_reselection_cnf_type   cell_resel_cnf;
  rrc_ul_suspend_req_type         rrc_ul_suspend_req;
  rrc_ul_resume_req_type          rrc_ul_resume_req;
  rrc_sig_conn_release_req_type   rrc_sig_conn_req;
  rrc_sig_conn_release_cnf_type   rrc_sig_conn_cnf;
  rrc_ciph_config_req_type        rrc_ciph_config_req;

  rrc_abort_ciph_config_req_type rrc_abort_ciph_config_req;

  rrc_ho_ciph_req_type            ho_ciph_req;
  rrc_stop_lower_layer_req_type   stop_lower_layer_req;
  rrc_stop_lower_layer_cnf_type   stop_lower_layer_cnf;
  rrc_mode_change_ind_type        mode_change_ind;

  rrc_sib7_update_status_ind_type sib7_update_status;
  rrc_change_mode_req_type        change_mode_req;
  rrc_change_mode_cnf_type        change_mode_cnf;

  rrc_delete_meas_req_type        delete_meas_id_req;

  rrc_csp_acq_req_type            rrc_csp_acq_req;
  rrc_csp_acq_cnf_type            rrc_csp_acq_cnf;
  rrc_ho_ciph_cnf_type            rrc_ho_ciph_cnf;

  rrc_lower_layer_deact_req_type  lower_layer_deact_req;
  rrc_re_est_rlc_srns_req_type    rrc_re_est_srns_req;
  rrc_re_est_rlc_srns_cnf_type    rrc_re_est_srns_cnf;
  rrc_llc_re_est_req_type         rrc_re_est_req;

  rrc_csp_camp_req_type           rrc_csp_camp_req;
  rrc_csp_camp_cnf_type           rrc_csp_camp_cnf;
  rrc_conn_setup_ind_type         rrc_conn_setup_ind;
  rrc_irat_abort_ind_type         rrc_irat_abort_ind;
  rrc_idt_complete_cnf            idt_complete_cnf;

  rrc_csp_bplmn_suspend_req_type    rrc_csp_bplmn_suspend_req;
  rrc_csp_bplmn_suspend_cnf_type    rrc_csp_bplmn_suspend_cnf;

  /* added internal structure for dual mode handling will be set by MCM to CSP*/
  rrc_mode_status_ind_type          rrc_mode_status_ind;

#ifdef FEATURE_WCDMA
  rrc_gstk_utran_nmr_info_req_type   rrc_gstk_utran_nmr_info_req;
#endif


#ifdef FEATURE_WCDMA_REL7_SPECRS 
  /*Internal commands added for requesting & confirmation of
    "Change of UE Capability" */
  rrc_couec_req_type rrc_couec_req;
  rrc_couec_cnf_type rrc_couec_cnf;
#endif

  rrc_cipher_config_cnf_type rrc_cipher_config_cnf;

#ifdef FEATURE_DUAL_SIM
  rrc_reset_l1_req_type rrc_reset_l1_req;
#endif  

 #endif

}rrc_cmd_int_q_u_type;





// Start RRC DECOUPLE

typedef struct
{
  /* Command header common for all commands */
  rrc_cmd_hdr_type    cmd_hdr;
  rrc_cmd_int_q_u_type cmd;
}rrc_cmd_int_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

#define   RRC_GET_POINTER_L1_REQ_VALUE(rrc_out_cmd_ptr,rrc_cmd_l1_req) \
{\
     rrc_out_cmd_ptr = \
        (l1_req_cmd *)rrc_malloc(sizeof(l1_req_cmd)); \
    if(rrc_out_cmd_ptr  == NULL) {\
        ERR_FATAL("l1_req_cmd not received",0,0,0); \
    }\
   rrc_cmd_l1_req = rrc_out_cmd_ptr;\
}
    
#define RRC_GET_POINTER_L2_REQ_VALUE(rrc_out_cmd_ptr,rrc_cmd_l2_req) \
{ \
     rrc_out_cmd_ptr = \
        (l2_ul_req_cmd *)rrc_malloc(sizeof(l2_ul_req_cmd)); \
    if(rrc_out_cmd_ptr  == NULL) {\
        ERR_FATAL("l2_req_cmd not received",0,0,0); \
    }\
   rrc_cmd_l2_req = rrc_out_cmd_ptr;\
}   

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
typedef struct 
{
  boolean rrc_buffer_ota_message;
  rrc_cmd_int_type rrc_buffered_ota;
  boolean rrc_buffered_ota_activation_time_expired_during_lta;
  rrc_proc_e_type procedure;
  uint8 transaction_id;
  boolean rrc_delay_restriction_flag;
}rrc_buffer_ota_info_type;

extern rrc_buffer_ota_info_type rrcdata_misc_info;
#endif
//End RRC DECOUPLE

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION RRC_GET_INT_CMD_BUF

DESCRIPTION

  This function returns a internal command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_int_cmd_buf( void );

/*===========================================================================

FUNCTION RRC_GET_CMD_BUF

DESCRIPTION

  This function returns a internal command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_cmd_buf( void );

/*===========================================================================

FUNCTION RRC_PUT_INT_CMD

DESCRIPTION

  This function puts a command buffer into RRC's internal
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_int_cmd_buf(). Also,
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
void rrc_put_int_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);


/*===========================================================================

FUNCTION rrccmd_free_internal_req_ptr

DESCRIPTION

  This function frees the internal pointers present in the internal commands

DEPENDENCIES


RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrccmd_free_internal_req_ptr
(
  rrc_cmd_type *cmd_buf 
);

/*===========================================================================
FUNCTION rrc_get_int_cmd_buf_from_heap_only


DESCRIPTION

  This function returns a internal command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.
 The function allocates memory from heap only and not from static buffer
DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_int_cmd_buf_from_heap_only
(
  void 
);


/*===========================================================================

FUNCTION rrc_free_embedd_buf_llc_req

DESCRIPTION

  This function frees the internal pointers present in the internal LLC commands

DEPENDENCIES


RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_free_embedd_buf_llc_req
(
  rrc_cmd_type *cmd_buf
);


#endif /* RRCCMD_V_H */

