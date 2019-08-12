
/*!
  @file
  emm_connection_handler.h

  @brief
  EMM internal data structure definitions.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_connection_handler.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
07/10/10   MNK     Fix CR  295108 - 1xCSFB:  Load balance connection release received after sending ESR not treated as soft failure 
10/07/10   MNK     1XCSFB introduction 
08/30/10   MNK     CSFB changes
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
01/07/10   zren    Updated release client and pended message processing
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/30/09   zren    Modified access barring timeout handling
10/06/09   RI      Fix new line warning at EOF.
10/06/09   vdr     Compiler warnings fixed
09/25/09   hnam    Added TLB support
08/14/09   zren    Modified procedure connection release handling
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
03/24/09   zren    Added support for service request and paging feature
02/11/09   vdr     Modified emm_connection_setup() & emm_send_message() interfaces
                   in order to register emm_release_ind_type callback
02/06/09   vdr     Now passing message pointer into failure delivery callback
01/30/29   vdr     Added new function prototype emm_start_connection_delay_timers()
===========================================================================*/

#ifndef _EMM_CONNECTION_HANDLER_H_
#define _EMM_CONNECTION_HANDLER_H_

#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "comdef.h"
#include "emm_connection_type.h"
#include "emm_database.h"
#include "lte_nas_emm_message_description.h"
#include "reg_mm_v.h"
#include "emm_reg_handler.h"
#include "lte_rrc_ext_msg.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "lte_nas.h"
#include "emm_irat_if_msg.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/
typedef
void (*rrc_l2_callback_type)
  (
    dword trans_id
  ) ; /* end of rrc_l2_callback_type() */

/* 
** Forward declaration of the emm_pended_msg_list_type.
** This declaration is required in order to declare function 
** callback rrc_connection_failure_type
*/
typedef struct emm_pended_msg_list_tag emm_pended_msg_list_type ;

typedef
boolean (*rrc_connection_failure_type)
  (
    lte_emm_connection_cause_type  cause,
    dword                          trans_id,
    emm_failure_type               emm_failure_cause,  
    lte_nas_tai_lst1_type          tai, /* valid only with TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO */
    struct emm_pended_msg_list_tag *message_ptr,
    emm_ctrl_data_type             *emm_ctrl_data_ptr
  ) ; /* end of rrc_connection_failure_type() */

struct emm_pended_msg_list_tag
{
  lte_nas_outgoing_msg_type       nas_out_message ;

  dword                           sequence_number ;
  /* Flag indicating if the message is sent out */
  boolean                         message_sent_indicator ;
  /* Callback function to be called upon receiving successful RRC data cnf */
  rrc_l2_callback_type            msg_delivery_callback ;
  /* Callback function to be called upon trasmission failure */
  rrc_connection_failure_type     msg_tx_failure_callback ;
  emm_ctrl_data_type             *emm_ctrl_data_pen_ptr ;
  /* Poniter to next message in the list */
  struct emm_pended_msg_list_tag *next_message ;
  /* Flag indicating that the message should be processed upon receiving 
     release indication due to connection abortion */
  boolean                         process_upon_conn_abort;
} ;

typedef struct emm_agent_list_tag
{
  msgr_tech_module_type      client ;
  struct emm_agent_list_tag *next_client ;
  emm_pended_msg_list_type  *next_message ;
  emm_pended_msg_list_type  *connection_setup_message ;
} emm_client_list_type ;

typedef
void (*emm_release_cb_func_ptr)
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
); 

typedef struct emm_release_client
{
  /* Callback function pointer */
  emm_release_cb_func_ptr  cb_func_ptr;
  /* Flag indicating that the client should be processed upon receiving 
     release indication due to connection abortion */
  boolean                  process_upon_conn_abort;
} emm_release_client_type;

/*===========================================================================

                        EXTERNAL DEFINITION

===========================================================================*/
extern
emm_reg_container_type emm_reg_container ;

extern
void emm_setup_reg_container
(
  emm_reg_container_type reg_req_tobe_executed
) ;

extern
void emm_process_connection_confirmation
(
  lte_rrc_conn_est_cnf_s *rrc_conn_est_cnf,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
) ;

/*===========================================================================
FUNCTION    EMM_PROCESS_RRC_RELEASE_INDICATION

DESCRIPTION
  This function checks the EMM state upon receiving release indication from RRC
  and calls function emm_process_normal_connection_release to process it if EMM
  is in a valid state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_release_indication
(
  lte_rrc_conn_rel_ind_s *rrc_conn_rel_ind,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
) ;

/*===========================================================================
FUNCTION    EMM_SEND_MESSAGE

DESCRIPTION
  This function caches given message in message list and sends the message 
  to RRC if connection is existing

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Message cached or sent successfully
  FALSE: Message not cached or sent because of no connection

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_send_message
(
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  msgr_tech_module_type        client,
  rrc_l2_callback_type         message_callback,
  rrc_connection_failure_type  connection_failure_callback,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
) ;

/*===========================================================================
FUNCTION    EMM_CONNECTION_SETUP

DESCRIPTION
  This function caches given message in pended message list and sends the 
  message together with RRC_CONN_EST_REQ to RRC to setup the connection

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Message sent successfully
  FALSE: Access barred

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_connection_setup
(
  lte_rrc_est_cause_e               lte_rrc_est_cause,
  lte_nas_outgoing_msg_type        *emm_message_ptr,
  dword                             sequence_number,
  msgr_tech_module_type             client,
  rrc_l2_callback_type              l2_ack, 
  rrc_connection_failure_type       tx_failure,
  lte_nas_conn_req_routing_info     routing_info,
  rex_timer_cnt_type               *rem_value_ptr,
  emm_ctrl_data_type               *emm_ctrl_data_ptr
) ;

/*===========================================================================
FUNCTION    EMM_SEND_NO_ACK_MSG

DESCRIPTION
  This function sends the message to RRC if connection is existing

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Message sent successfully
  FALSE: Message not sent because of no connection

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_send_no_ack_msg
(
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_BARRING_CALL_BACK

DESCRIPTION
  This function processes the expiry of EMM barring timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_barring_call_back
(
  dword barring_mask
) ;

extern
void emm_start_connection_delay_timers
(
  byte                rrc_call_barring_info_field,
  dword               barring_timer_length,
  emm_ctrl_data_type *emm_ctrl_data_ptr
) ;


/*===========================================================================
FUNCTION    EMM_ADD_NEW_RELEASE_CLIENT

DESCRIPTION
  This function adds a release client to the client list with given callback
  function pointer if it is not existing in the list already  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_add_new_release_client
(
  emm_release_cb_func_ptr  new_release_ind_client
);

/*===========================================================================
FUNCTION    EMM_REMOVE_RELEASE_CLIENT

DESCRIPTION
  This function removes a release client from the list with the given callback 
  function pointer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_remove_release_client
(
  emm_release_cb_func_ptr  release_ind_client
);

void emm_process_access_barring_timer_expiry
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_CONN_ABORT_MARK_RELEASE_CLIENT

DESCRIPTION
  This function marks all existing release clients to be processed upon 
  receiving release indication with cause of aborted 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_conn_abort_mark_release_client
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_AUTONOMOUS_CONNECTION_SETUP

DESCRIPTION
  This function tries to set up the RRC connection when EMM moves to IDLE and 
  there are messages pending.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_autonomous_connection_setup
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION    EMM_CONNECTION_ESTABLISHMENT_BARRED

DESCRIPTION
  This function checks if the given connection establishment type is barred

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_connection_establishment_barred
(
  lte_rrc_est_cause_e  lte_rrc_est_cause,
  rex_timer_cnt_type  *rem_value_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_SEND_REG_T311_TIMER_EXPIRY_IND

DESCRIPTION
  This function sends T311 timer expiry indication to REG

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_reg_t311_timer_expiry_ind
(
  void
);

#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION    EMM_HANDLE_EXTENDED_WAIT_TIME

DESCRIPTION
  This function handles extended wait time received from lower layers

DEPENDENCIES
  None

RETURN VALUE
  True if extended wait time is processed else False

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_handle_extended_wait_time
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);
#endif

#ifdef FEATURE_NAS_EAB
/*===========================================================================
FUNCTION    EMM_START_EAB_BARRING

DESCRIPTION
  This function starts EAB barring

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_start_eab_barring
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_STOP_EAB_BARRING

DESCRIPTION
  This function stops EAB barring

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_stop_eab_barring
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_CHK_EAB_BARRING

DESCRIPTION
  This function checks if the given connection establishment type is barred
  due to EAB barring

DEPENDENCIES
  None

RETURN VALUE
  TRUE if barred else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_chk_eab_barring
(
  lte_rrc_est_cause_e  lte_rrc_est_cause,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);
#endif

#endif /* _EMM_CONNECTION_HANDLER_H_ */

#endif /*FEATURE_LTE*/

