
/*!
  @file
  emm_update_lib.h

  @brief
  EMM system definitions.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_update_lib.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
30/08/11   MNK     Add dual RX CSFB support 
10/07/10   MNK     1XCSFB introduction 
08/30/10   MNK     CSFB changes
04/28/10   zren    Fixed lint warnings
03/16/10   zren    Updated MO detach procedure upon receiving MMR_STOP_MODE
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
10/06/09   vdr     Compiler warnings fixed
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/14/09   zren    Modified procedure connection release handling
05/29/09   zren    Added support for TAU feature
05/17/09   hnam    Added the code to support security
05/11/09   zren    Added support for MT detach procedure 
04/21/09   hnam    Added T3440 timer support (required during attach and service reject)
03/24/09   zren    Added support for service request and paging feature
03/12/09   RI      Added support for SMC.
02/23/09   vdr     Fixed Lint errors & warnings
02/05/09   MNK     Added function pointer definitions
===========================================================================*/
#ifndef _EMM_UPDATE_LIB_H_
#define _EMM_UPDATE_LIB_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "lte_nas_emm_message_description.h"
#include "lte_nas_emm_message_type.h"
#include "emm_connection_handler.h"
#include "emm_database.h"
#include "lte_nas.h"
#include "lte_nas_security_info.h"

#define MAX_NUMBER_ATTACH_OF_ATTEMPTS  5

#define MAX_NUMBER_ESM_FAILURE_ATTEMPTS  3

#define MAX_NUMBER_DETACH_OF_ATTEMPTS  5

#define MAX_NUMBER_DETACH_ACCEPT_ATTEMPTS  1

#define MAX_NUMBER_TAU_OF_ATTEMPTS     5

typedef enum
{
  EMM_INITIAL_UPDATE,
  EMM_UPDATE_RECOVERY,
  EMM_UPDATE_RESTART
} emm_update_attempt_type ;

typedef word (encode_nas_msg_func_ptr)(lte_nas_outgoing_msg_type *out_buf,
                                       size_t out_buf_len,
                                       byte *ota_buf,
                                       size_t ota_buf_len,
                                       lte_nas_ul_security_info_type *ul_security_info);
typedef byte (decode_nas_msg_func_ptr)(byte *buffer,word  buffer_length,
                                       lte_nas_dl_security_info_type *dl_security_info,
                                       struct emm_ctrl_data_tag  *emm_ctrl_data_ptr,
                                       lte_nas_incoming_msg_type *nas_incoming_msg_ptr);
typedef lte_nas_protocol_discriminator (nas_retrieve_msg_func_ptr)(byte index,lte_nas_incoming_msg_type *buffer);

extern encode_nas_msg_func_ptr* encode_nas_msg_fp;
extern decode_nas_msg_func_ptr* decode_nas_msg_fp;
extern nas_retrieve_msg_func_ptr* nas_retrieve_msg_fp;


/*===========================================================================

                        TYPE DEFINITION

===========================================================================*/


/*===========================================================================

                        EXTERNAL DEFINITION

===========================================================================*/
void emm_init_attach_procedure
(
  emm_update_attempt_type  attach_attemp_type,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);

extern
void emm_start_attach_procedure
(
  emm_update_attempt_type  attach_attemp_type,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
) ;

extern
boolean emm_detach_failed_routine
(
  lte_emm_connection_cause_type  cause,
  dword                          trans_id,
  emm_failure_type               emm_failure_cause,
  lte_nas_tai_lst1_type          tai,
  struct emm_pended_msg_list_tag *message_ptr,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) ;

extern
void emm_start_mo_detach
(
  emm_mo_detach_type          detach_type,
  boolean                     power_off_indicator,
  rrc_l2_callback_type        l2_ack,
  rrc_connection_failure_type tx_failed,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
) ;

extern
void emm_process_attach_accept
(
  lte_nas_emm_attach_accept_type *emm_attach_accept,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) ;

extern
void emm_process_attach_reject
(
  lte_nas_emm_attach_rej_type *attach_reject,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) ;

extern
void emm_process_detach_accept
(
  lte_nas_emm_mt_detach_accept_type *emm_detach_accept,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
) ;

extern
void emm_timer_t3410_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
) ;

extern
void emm_timer_t3440_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
) ;

extern
void emm_attach_timer_t3402_t3411_expire
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  timer_id_T           timer_id 
) ;

extern
void emm_forbidden_tai_clear_timer_expiry
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION  EMM_POWERDOWN_TIMER_EXPIRE

DESCRIPTION
  This function processes the power down timer expiry for switch off MO detach 
  procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_powerdown_timer_expire
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

extern
void emm_t3421_expire
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION  EMM_ABORT_MO_DETACH

DESCRIPTION
  This function aborts an ongoing MO detach procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_abort_mo_detach
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  boolean              abort_connection

);

extern
void emm_start_mt_detach
(
  lte_nas_emm_mt_detach_req_type     *mt_detach_request,
  emm_ctrl_data_type                 *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION    EMM_TAU_REQUEST_FAILED

DESCRIPTION
  This function is called when RRC indicates that TAU Request message 
  delivery has been failed.

DEPENDENCIES
  This is a call back function which should not be called by name

RETURN VALUE
  TRUE  - indicates that TAU procedure is required before another EMM 
          or ESM procedures may be resumed
  FALSE - indicates that TAU is not required

SIDE EFFECTS
  This function modifies value of the corresponding boolean flag 
  message_sent_indicator:
    TRUE  - current message CAN be deleted from the cache memory
    FALSE - current message CANNOT be deleted from the cache memory
===========================================================================*/
extern
boolean emm_tau_request_failed
(
  /* RRC failure cause */
  lte_emm_connection_cause_type   cause,

  /* Message ID that this callback is assosiated with */
  dword                           trans_id,

  /* Failure cause type and cause */
  emm_failure_type                emm_failure_cause,

  /* The following TAI is valid only with 
     TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO */
  lte_nas_tai_lst1_type           tai,

  /* Pointer to the massage which failed to be tx */
  struct emm_pended_msg_list_tag *message_ptr,

  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION    EMM_START_TAU_PROCEDURE

DESCRIPTION
  This function starts TAU procedure as described in TS 24.301 5.5.3.2.2

DEPENDENCIES
  This function shall be not called when UE is in DEREGISTERED state

RETURN VALUE
  None

SIDE EFFECTS
  This function moves the EMM into EMM_TRACKING_AREA_UPDATING_INITIATED state

===========================================================================*/
extern
void emm_start_tau_procedure
(
  /* TAU attempt type */
  emm_update_attempt_type  tau_attemp_type,

  /* TAU update type */
  lte_nas_emm_eps_update_val_type   emm_current_eps_ta_update,

  /* 
    Indicates that there are ongoing EPS activities
    either in DS or ESM
  */
  boolean eps_active_flag,

  /* EMM common control database */
  emm_ctrl_data_type      *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION    EMM_INITIATE_TAU_PROCEDURE

DESCRIPTION
  This function initiates TAU procedure attempt if procedure is allowed 
  according to 24.301 5.5.3

DEPENDENCIES
  This function shall be not called when UE is in DEREGISTERED state

RETURN VALUE
  None

SIDE EFFECTS
  Triggers TAU procedure

===========================================================================*/
extern
void emm_initiate_tau_procedure
(
  /* TAU attempt type */
  emm_update_attempt_type  tau_attemp_type,

  /* TAU update type */
  lte_nas_emm_eps_update_val_type   emm_current_eps_ta_update,

  /* Flag to indicate enforce TAU */
  boolean  enforce_tau,

  /* EMM common control database */
  emm_ctrl_data_type      *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION    EMM_TAU_RELEASE_INDICATION

DESCRIPTION
  This function is called when existing RRC connection which has been used for
  TAU procedure is release by RRC. See 24.301 5.5.3.2.6 b) for details.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function 

===========================================================================*/
extern
void emm_tau_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION    EMM_ENFORCE_NORMAL_SERVICE

DESCRIPTION
  This function enforces transition into NORMAL SERVICE substate of the 
  REGISTERED state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  o EMM moves into EMM_MOVES_TO_REGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE)
  o TAU timers (T3402/T3411) are stopped if they are running
  o T3412 is started
  The following Global Control Vars are updated:
    o last_registered_tai_valid
    o last_registered_tai
    o plmn_service_state
===========================================================================*/
extern
void emm_enforce_normal_service
(
  lte_nas_tai_lst1_type *current_tai,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
) ;

void emm_timer_t3412_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

void emm_process_tau_reject
(
  lte_nas_emm_tau_rej         *tau_rej_ptr,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
);

void emm_process_tau_accept
(
  lte_nas_emm_tau_accept_type *emm_tau_accept_ptr,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
);

void emm_start_tau_recovery_procedure
(
  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
);

void emm_timer_t3430_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

void emm_poweroff_detach_release_ind
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
);

void emm_attach_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
);

void emm_tau_request_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
);

void emm_process_attach_failure_multimode
(  
  lte_nas_emm_cause_type emm_cause,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

void emm_process_rau_sr_reject
(  
  lte_nas_message_id       msg_id,
  lte_nas_emm_cause_type emm_cause,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_MT_DETACH_INVALIDATE_MOBILITY

DESCRIPTION
  This function invalidates EMM mobility information depending on given emm
  cause upon reattach not required MT detach

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_mt_detach_invalidate_mobility
(
  lte_nas_emm_cause_type   emm_cause,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_PROCESS_CS_REJECT_CAUSE

DESCRIPTION
  This function processes EMM cause for EPS only successful Attach Accept

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_process_cs_reject_cause
(
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);

extern void emm_timer_t3423_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

#ifdef FEATURE_1XSRLTE
void emm_process_cm_dual_rx_tau_cmd(emm_ctrl_data_type   *emm_ctrl_data_ptr);
#endif


/*===========================================================================

FUNCTION    EMM_PROCESS_UE_CAPABILITIES_INFO

DESCRIPTION
  This function set SRVCC value on fly and write new value into EFS 

DEPENDENCIES
  CM should send this command only if airplane mode on

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void emm_process_ue_capabilities_info
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

#ifdef FEATURE_LTE_REL9
void emm_process_ims_voice_tag_reg_status_ind
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);
#endif

void emm_process_mmtel_call_status_info
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);


void emm_process_update_ftai_list_cmd
(
  cm_mm_update_ftai_list_T  *update_ftai_list_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);


void emm_check_possible_tau_for_mmtel_and_sms
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

void emm_process_wms_sms_info
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

extern void emm_complete_mo_detach
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);


#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION    EMM_TIMER_T3346_EXPIRE

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
extern void emm_timer_t3346_expire
(
  boolean             active_flag,   
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

#endif

#ifdef FEATURE_SGLTE
/*===========================================================================
FUNCTION    EMM_TIMER_DELAY_TAU_EXPIRE

DESCRIPTION
  start TAU on timer expiry. Started on SRVCC HO in SGLTE config

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
extern void emm_timer_delay_tau_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);
#endif 

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION    emm_timer_policy_change_pending_expire

DESCRIPTION
   Policy man chagne pending timer expiry

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
extern void emm_timer_policy_change_pending_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);
#endif 

#endif /* _EMM_UPDATE_LIB_H_ */

#endif /*FEATURE_LTE*/

