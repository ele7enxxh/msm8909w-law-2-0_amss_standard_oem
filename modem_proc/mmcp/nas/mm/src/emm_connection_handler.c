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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_connection_handler.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/23/15   Abhi    If MO CSFB call is not pending then only send release indication to CNM on 
                              EMM connection release 
07/10/10   MNK     Fix CR  295108 - 1xCSFB:  Load balance connection release received after sending ESR not treated as soft failure 
10/07/10   MNK     1XCSFB introduction 
08/30/10   MNK     CSFB changes
06/21/10   zren    Updated EMM to include correct MME routing info in conn est 
                   req for TAU after IRAT  
06/04/10   zren    Updated to call DS API to determin if there is data pending
                   when RRC connecion failure happens
06/02/10   zren    Updated EMM to be able to handle the scenario of RLF failing
                   ATTACH COMPLETE message gracefully
04/09/10   zren    Added combined procedure feature
03/16/10   zren    Updated MO detach procedure upon receiving MMR_STOP_MODE
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/07/10   zren    Updated release client and pended message processing
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/30/09   zren    Modified access barring timeout handling
10/20/09   RI      Added security context state update when in connected mode.
09/25/09   hnam    Added TLB support
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/14/09   zren    Modified procedure connection release handling
08/04/09   zren    Added EMM state check to ensure that TAU will not be 
                   triggered in DEREGESTERED or DEREGESTERED_INITIATED state 
                   when RRC_REL_IND with load balancing flag set is received 
07/31/09   tmai    Modified emm_process_normal_connection_confirmation to
                   start connection delay timer in every unsuccessful connection establishment
07/29/09   zren    Added support to always send SIG_CONN_RELEASED_IND to ESM 
                   when RRC_RELEASE_IND is received
07/27/09   zren    Added F3 messages for expiry of EMM barring timers
                   Added support to stop all barring timers upon successful 
                   connection establishment
07/21/09   zren    Added support to set high priority flag in RRC_CONN_EST_REQ
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
06/18/09   zren    Modified power off detach success CB function to abort 
                   signaling connection
06/13/09   RI      Added EMM ctrl data param to emm_send_rrc_data_request() 
06/12/09   zren    Modified EMM to move to REGISTERED_UPDATE_NEEDED state 
                   instead of starting TAU right away when received RELEASE_IND
                   with load balancing requirement
06/02/09   zren    Added SEMAPHORE to protect access_barring_flag
05/29/09   zren    Added support for TAU feature
05/28/09   zren    Added code to stop T3440 when receiving RRC_RELEASE_IND
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/15/09   zren    Completed MO detach feature
03/24/09   zren    Added support for service request and paging feature
03/13/09   RI      Changed the release client parameter handling in 
                   send message function.
02/23/09   vdr     Fixed Lint errors & warnings
02/11/09   vdr     Now providing RRC release indication for all EMM ongoing procedures
02/06/09   vdr     Modified lte_rrc_conn_est_cnf_s processing according to lte_rrc_ext_msg.h#54
01/31/09   mnk     Fixed compilation error
01/30/09   vdr     Updated handling of Access Class barring timers
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "comdef.h"
#include "emm.h"
#include "rex.h"
#include "emm_reg_handler.h"
#include "emm_connection_handler.h"
#include "emm_connection_type.h"
#include "emm_database.h"
#include "emm_utility.h"
#include "lte_rrc_ext_msg.h"
#include "emm_rrc_handler.h"
#include "emm_rrc_if.h"
#include "esm_emm_msg_if.h"
#include "esm_emm_msgr_enum_msg.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "pthread.h"
#include "emm_update_lib.h"
#include "mm_v.h"
#include "err.h"
#include "emm_esm_handler.h"
#include "emm_security.h"
#include "emm_cnm_handler.h"
#include "emm_csfb_handler.h"
#include "ds3gpp_api.h"
#include "stringl.h"
#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE)) 
 #error code not present
#endif // TEST_FRAMEWORK

emm_reg_container_type emm_reg_container = (emm_reg_container_type)NULL ;


#define EMM_RELEASE_CLIENT_ARRAY_MAX_LENGTH 10
static emm_release_client_type  \
                 emm_release_client_array[EMM_RELEASE_CLIENT_ARRAY_MAX_LENGTH];
static word  emm_release_client_array_length = 0 ;

/*===========================================================================
FUNCTION    emm_skip_ac_barring_chk

DESCRIPTION 
 
  This function check if the special handling for volte call is needed 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if special handling is needed, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_skip_ac_barring_chk(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
   if((EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED) && 
      (emm_ctrl_data_ptr->skip_acb_for_volte_call == TRUE) &&
      (emm_ctrl_data_ptr->call_type == SYS_LTE_REESTAB_CALLTYPE_VOLTE) &&
      (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)&& 
      (emm_ctrl_data_ptr->service_req_cause == LTE_RRC_EST_CAUSE_MO_DATA))
   {
      MSG_HIGH_DS_0(MM_SUB, "EMM: Skip access call class barring check for VOLTE call");
      return TRUE;
   }
     return FALSE;
}/* end of emm_skip_ac_barring_chk()*/

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
)
{  
  mmr_suitable_search_end_ind_s_type   mmr_search_end_ind;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= T311 timer is stopped/expired, RRC connection is released");
  mmr_search_end_ind.message_header.message_set = MS_MM_REG ;
  mmr_search_end_ind.message_header.message_id  = (int)MMR_SUITABLE_SEARCH_END_IND ;

  /*lint -save -e778 */
  PUT_IMH_LEN( sizeof(mmr_suitable_search_end_ind_s_type) - sizeof(IMH_T),&mmr_search_end_ind.message_header );
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent MMR_SUITABLE_SEARCH_END_IND to REG");
  mm_send_message( &mmr_search_end_ind.message_header, GS_QUEUE_REG );
  //event.emm_out_msg = (byte)MMR_SUITABLE_SEARCH_END_IND;
  //event_report_payload(EVENT_LTE_REG_OUTGOING_MSG,sizeof(event), (void*)&event);
}

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*===========================================================================
FUNCTION    EMM_SEEK_RELEASE_CLIENT

DESCRIPTION
  This function finds release client using the given release callback function
  pointer and returns its index 

DEPENDENCIES
  None

RETURN VALUE
  Index of release client with given callback funcation pointer
  or
  0xFFFF if release client can NOT be found 

SIDE EFFECTS
  None
===========================================================================*/
word emm_seek_release_client
(
  emm_release_cb_func_ptr  wanted_cb_func_ptr,
  word                     array_length
)
{
  word search_index = 0 ;

  for (search_index = 0; search_index < array_length; search_index ++)
  {
    if(wanted_cb_func_ptr == 
                            emm_release_client_array[search_index].cb_func_ptr)
    {
      /* Wanted client found, return its index */
      return search_index;
    }
  }

  /* Client can not be found in list, return 0xFFFF */
  return 0xFFFF;

} /* end of emm_seek_release_client() */

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
  emm_release_cb_func_ptr  new_release_cb_func_ptr
)
{
  if (new_release_cb_func_ptr != NULL)
  {
    /* Check whether the given client is existing or not */
    if (emm_seek_release_client(new_release_cb_func_ptr, 
                                emm_release_client_array_length) == 0xFFFF)
    {
      if(emm_release_client_array_length < EMM_RELEASE_CLIENT_ARRAY_MAX_LENGTH)
      {
        /* Add client */
        emm_release_client_array[emm_release_client_array_length].cb_func_ptr = 
                                                       new_release_cb_func_ptr;
        /* By default set the flag to FALSE */
        emm_release_client_array[emm_release_client_array_length].\
                                               process_upon_conn_abort = FALSE;
        emm_release_client_array_length ++;
      }
      else
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= Failed to add release client - MAX clients reached");
      }
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Release client has already been registered");
    }
  }
} /* end of emm_add_new_release_ind_client() */

/*===========================================================================
FUNCTION    EMM_CALL_CONNECTION_RELEASE_CLIENT

DESCRIPTION
  This function processes the release clients list
  - If the given release cause is NOT CONN_REL_LTE_RRC_CONN_REL_ABORTED, it
    calls all release clients and clears the release client list
  - If the given release cause is NOT CONN_REL_LTE_RRC_CONN_REL_ABORTED, it 
    only calls release clients with process flag set on and clear those clients
    from the list    

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_call_connection_release_client
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s redir_info ,
  lte_nas_tau_cause_type tau_cause 
)
{
  word  index = 0 ;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if(emm_ctrl_data_ptr->aborting_conn == TRUE)
  {
    while(index < emm_release_client_array_length)
    {
      /* Only process the client when processing flag is set to TRUE */
      if(emm_release_client_array[index].process_upon_conn_abort == TRUE)
      {
        emm_release_client_array[index].cb_func_ptr(release_cause,
                                                    emm_failure_cause, 
                                                    emm_ctrl_data_ptr,
                                                    redir_info,tau_cause
                                                    );
        /* Remove the client from list */
        /* Note: Since one client is removed and all clients after it are moved
           one index up, there is no need to increase index */
        emm_remove_release_client(emm_release_client_array[index].cb_func_ptr);
      }
      else
      {
        /* Increase the index to get next client */
        index ++;
      }
    }
  }
  else
  {
    /* Release cause other than ABORTED - process all clients */
    for(index = 0; index < emm_release_client_array_length; index++)
    {
      emm_release_client_array[index].cb_func_ptr(release_cause, 
                                                  emm_failure_cause,
                                                  emm_ctrl_data_ptr,
                                                  redir_info,tau_cause
                                                  );
    }
    /* Clean up Connection Release Indication Array */
    emm_release_client_array_length = 0 ;
  }
} /* end of emm_call_connection_release_client() */

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
  emm_release_cb_func_ptr    release_cb_func_ptr
)
{
  word   index_tobe_removed = 0;
  word   length_tobe_moved = 0;

  ASSERT(emm_release_client_array_length <= 
                                          EMM_RELEASE_CLIENT_ARRAY_MAX_LENGTH);

  if ((release_cb_func_ptr != NULL) && (emm_release_client_array_length > 0))
  {
    /* Locate the given client in the list */
    index_tobe_removed = emm_seek_release_client(release_cb_func_ptr,
                                              emm_release_client_array_length);
    /* Check whether the given cilent is found or not */
    if (index_tobe_removed != 0xFFFF)
    {
      /* Calculate the length of clients to be moved.
         - If the length_tobe_moved is 0 which means that there is only one 
           client in the list, there is no need to move other clients up. Just
           reduce the emm_release_client_array_length by one.
         - If the length_tobe_moved is bigger than 0, there are clients to be
           moved up by one index */
      length_tobe_moved = emm_release_client_array_length 
                          - index_tobe_removed - 1;

      if(length_tobe_moved > 0)
      {
        /* Move all the clients after the given client one index up to remove 
           the given client */  
        memsmove((void*)&emm_release_client_array[index_tobe_removed],
                 sizeof(emm_release_client_array) - sizeof(emm_release_client_type) * index_tobe_removed,
                 (void *)&emm_release_client_array[index_tobe_removed+1],
                 sizeof(emm_release_client_type)* length_tobe_moved);
      }
      /* Reduce the client list length by one */
      emm_release_client_array_length --;
    }
  }
} /* end of emm_remove_release_client() */

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
)
{
  word  index = 0 ;

  ASSERT(emm_ctrl_data_ptr != NULL) ;

  MSG_LOW_DS_0(MM_SUB, "=EMM= Mark rel client to be called upon reving conn abort rel ind");

  /* Traverse the release client list and set flags to TRUE for all clients */
  for (index = 0; index < emm_release_client_array_length; index ++)
  {
    emm_release_client_array[index].process_upon_conn_abort = TRUE;    
  }

} /* end of emm_conn_abort_mark_release_client */

void emm_setup_reg_container
(
  emm_reg_container_type reg_req_tobe_executed
)
{
  emm_reg_container = reg_req_tobe_executed ;
} /* end of emm_setup_reg_container() */

#ifdef FEATURE_NAS_EAB
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
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  /* When EAB barring is active it bars MO Signaling, MO Data, MO CSFB and Delay
     Tolerant call types. EAB barring is not applicable for call types Emergency
     and MT Access. */

  if((emm_ctrl_data_ptr->eab_barring == TRUE) 
     &&
     ((lte_rrc_est_cause == LTE_RRC_EST_CAUSE_MO_SIGNALING) ||
      (lte_rrc_est_cause == LTE_RRC_EST_CAUSE_MO_DATA) ||
      (lte_rrc_est_cause == LTE_RRC_EST_CAUSE_MO_CSFB) ||
      (lte_rrc_est_cause == LTE_RRC_EST_CAUSE_DELAY_TOLERANT_ACCESS)))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= UE is EAB barred");
    return TRUE;
  }

  return FALSE;
}
#endif

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
)
{
  boolean             call_is_barred = FALSE;
  rex_timer_type     *timer_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 


  /* First check if call is barred due to EAB barring. If not then continue to check if
     call is barred due to ACB barring. There is no timer started if EAB barring is
     active hence set the remaining timer value to LTE_RRC_INDEFINITE_BARRING_TIME */
#ifdef FEATURE_NAS_EAB
  if(emm_chk_eab_barring(lte_rrc_est_cause, emm_ctrl_data_ptr) == TRUE)
  {
    if(rem_value_ptr!=NULL)
	{
      *rem_value_ptr = (rex_timer_cnt_type)LTE_RRC_INDEFINITE_BARRING_TIME;
	}
    return TRUE;
  }
#endif

  /* PROTECT BY SEMAPHORE */
  (void)pthread_mutex_lock(emm_ctrl_data_ptr->emm_mutex_ptr);

  switch (lte_rrc_est_cause)
  {
    case LTE_RRC_EST_CAUSE_EMERGENCY:
      call_is_barred = (0 != (emm_ctrl_data_ptr->access_barring_flag & (byte)EMM_EMERGENCY_BARRED)) ? TRUE : FALSE ;
      timer_ptr = &emm_ctrl_data_ptr->emm_access_barring_emergency_timer;
      break ;
    case LTE_RRC_EST_CAUSE_MT_ACCESS:
      call_is_barred = (0 != (emm_ctrl_data_ptr->access_barring_flag & (byte)EMM_MT_ACCESS_BARRED)) ? TRUE : FALSE ;
      timer_ptr = &emm_ctrl_data_ptr->emm_access_barring_mt_access_timer;
      break ;
    case LTE_RRC_EST_CAUSE_MO_SIGNALING:
      call_is_barred = (0 != (emm_ctrl_data_ptr->access_barring_flag & (byte)EMM_MO_SIGNALING_BARRED)) ? TRUE : FALSE ;
      timer_ptr = &emm_ctrl_data_ptr->emm_access_barring_mo_signaling_timer;
      break ;
    case LTE_RRC_EST_CAUSE_MO_DATA:
      call_is_barred = (0 != (emm_ctrl_data_ptr->access_barring_flag & (byte)EMM_MO_DATA_BARRED)) ? TRUE : FALSE ;
      timer_ptr = &emm_ctrl_data_ptr->emm_access_barring_mo_data_timer;
      break ;
#ifdef FEATURE_LTE_REL10
    case LTE_RRC_EST_CAUSE_MO_CSFB:
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
      {
        call_is_barred = (0 != (emm_ctrl_data_ptr->access_barring_flag & (byte)EMM_MO_CSFB_BARRED)) ? TRUE : FALSE ;
        timer_ptr = &emm_ctrl_data_ptr->emm_access_barring_mo_csfb_timer;
      }
      break ;

    case LTE_RRC_EST_CAUSE_DELAY_TOLERANT_ACCESS:
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
      {
        call_is_barred = (0 != (emm_ctrl_data_ptr->access_barring_flag & (byte)EMM_DELAY_TOLERANT_BARRED)) ? TRUE : FALSE ;
        timer_ptr = &emm_ctrl_data_ptr->emm_access_barring_delay_tolerant_timer;
      }
      break;
#endif

    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal call type barring",0,0,0) ;
      break ;
  }

  if((call_is_barred == TRUE) && (rem_value_ptr != NULL))
  {
    /* Get the remainning access barring timer value */
    *rem_value_ptr = rex_get_timer(timer_ptr);
  }
 
  (void)pthread_mutex_unlock(emm_ctrl_data_ptr->emm_mutex_ptr);
  /* PROTECT BY SEMAPHORE */

  return call_is_barred ;

} /* end of emm_connection_establishment_barred() */

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
)
{
  emm_pended_msg_list_type  *cached_message = NULL ;
  boolean skip_ac_barring_chk = FALSE;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if (EMM_GET_CONNECTION_STATE()!=EMM_IDLE_STATE)
  {
     MSG_FATAL_DS(MM_SUB, "=EMM= EMM_GET_CONNECTION_STATE NOT IDLE", 0,0,0 );
  }
  
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Start RRC connection establishment");

  if(emm_ctrl_data_ptr->sib2_ac_barring_info.ac_Barring_r12_present == TRUE)
  {
    if(((EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED) && 
        (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)&& 
        (emm_ctrl_data_ptr->service_req_cause == LTE_RRC_EST_CAUSE_MO_DATA))
        ||((EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED) && 
        (lte_rrc_est_cause == LTE_RRC_EST_CAUSE_MO_SIGNALING)))
    {
        skip_ac_barring_chk = emm_set_call_type_based_on_sib2(emm_ctrl_data_ptr);
    }
  }
  else
  {
    skip_ac_barring_chk = emm_skip_ac_barring_chk(emm_ctrl_data_ptr);
  }

  if((skip_ac_barring_chk == FALSE)&&
     (emm_connection_establishment_barred(lte_rrc_est_cause, 
                                           rem_value_ptr,
                                           emm_ctrl_data_ptr) == TRUE))
  {
    return FALSE ;
  }

  cached_message = emm_cache_new_message(emm_message_ptr, sequence_number, 
                                         client, l2_ack, tx_failure,
                                         emm_ctrl_data_ptr);

  cached_message->message_sent_indicator = TRUE ;


  emm_send_rrc_connection_establishment_request(lte_rrc_est_cause, 
                                                emm_message_ptr, 
                                                sequence_number, 
                                                routing_info,
                                                emm_ctrl_data_ptr) ;

    /*Reset call type back to normal*/
  if (emm_ctrl_data_ptr->call_type != SYS_LTE_REESTAB_CALLTYPE_NORMAL)
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_NORMAL; 
  }

  EMM_MOVES_TO_WAITING_FOR_RRC_CONFIRMATION_STATE() ;  

  return TRUE ;
} /* end of emm_connection_setup() */

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
)
{
  emm_pended_msg_list_type      *startup_message = NULL ;
  lte_rrc_est_cause_e            rrc_est_cause;
  lte_nas_conn_req_routing_info  routing_info = ROUTING_INFO_STMSI;
  rex_timer_cnt_type             rem_value = 0;
  emm_failure_type               emm_failure_cause;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr->client_list != (emm_client_list_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr->client_list->connection_setup_message != (emm_pended_msg_list_type *)NULL) ; 

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  startup_message = emm_ctrl_data_ptr->client_list->connection_setup_message ;

  switch(EMM_GET_STATE())
  {
    case EMM_SERVICE_REQUEST_INITIATED:
      rrc_est_cause = emm_ctrl_data_ptr->service_req_cause;
      break;
    case EMM_DEREGISTERED_INITIATED:
      rrc_est_cause = LTE_RRC_EST_CAUSE_MO_SIGNALING;
      break;
    default:
#ifdef FEATURE_LTE_REL9
      if((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) 
         ||
         (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == TRUE))
      {
        rrc_est_cause = LTE_RRC_EST_CAUSE_EMERGENCY;
      }
      else
      {
#endif
        rrc_est_cause = LTE_RRC_EST_CAUSE_MO_SIGNALING;
#ifdef FEATURE_LTE_REL9
      }
#endif
  }

  if(emm_connection_establishment_barred(rrc_est_cause, 
                                         &rem_value,
                                         emm_ctrl_data_ptr) == TRUE)
  {
    lte_emm_connection_cause_type cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED;

    emm_failure_cause.cause_type              = LTE_NAS_IRAT_ACCESS_BARRING;
    emm_failure_cause.access_barring_rem_time = rem_value;

#ifdef FEATURE_NAS_EAB
    if(emm_chk_eab_barring(rrc_est_cause, emm_ctrl_data_ptr) == TRUE)
    {
      cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB;
    }
#endif

    emm_process_unsent_messages(cause, 
                                emm_failure_cause,
                                emm_ctrl_data_ptr);

    /* Process all pending irat uplink messages */
    emm_process_pending_irat_msg(LTE_NAS_IRAT_ACCESS_BARRING,
                                 LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                 rem_value,
                                 emm_ctrl_data_ptr);

    MSG_ERROR_DS_0(MM_SUB, "=EMM= Connection establishment is barred, remaining in EMM IDLE state");
      
    return ;
  }

  startup_message->message_sent_indicator = TRUE ;

  if(EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED)
  {
    routing_info = emm_get_tau_req_mme_routing_info(emm_ctrl_data_ptr);
  }
  else if(EMM_GET_STATE() == EMM_REGISTERED_INITIATED)
  {
    routing_info = ROUTING_INFO_GUMMEI;
  }

  emm_send_rrc_connection_establishment_request(rrc_est_cause,
                                             &startup_message->nas_out_message,
                                              startup_message->sequence_number,
                                              routing_info,
                                              emm_ctrl_data_ptr) ;

  emm_ctrl_data_ptr->client_list->connection_setup_message  = NULL ;

  EMM_MOVES_TO_WAITING_FOR_RRC_CONFIRMATION_STATE() ;  
} /* end of emm_autonomous_connection_setup() */

/*===========================================================================

FUNCTION   EMM_POST_ACCESS_BARRING_TIMER_EXPIRY

DESCRIPTION
  This function posts access barring timer expiry message to mm message queue 
  to notify the expiry of EMM_ACCESS_BARRING_TIMER.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined FEATURE_DUAL_SIM
void emm_post_access_barring_timer_expiry
(
/*For Emm Sub id and as id are same as lte will be on main stack always*/
  mm_as_id_e_type as_id
)
#else
void emm_post_access_barring_timer_expiry(void)
#endif
{
  mm_cmd_type    *cmd_ptr;

  /* Get command buffer from reg task */
  cmd_ptr = mm_get_cmd_buf();

  if(cmd_ptr == NULL)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Unable to alloc mem to post access barring timer expiry",
               0,0,0 );
  }

  /* Construct message */
  cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
  cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

  PUT_IMH_LEN ((sizeof(cmd_ptr->cmd.timer_expiry) - 
                sizeof(cmd_ptr->cmd.timer_expiry.IMH)),
               &cmd_ptr->cmd.timer_expiry.IMH);
#if defined  FEATURE_DUAL_SIM 
/*For EMM sub id and as id are same, so passing the sub id with timer expiry*/
     cmd_ptr->cmd.timer_expiry.data = as_id;
#endif

  cmd_ptr->cmd.timer_expiry.timer_id = EMM_ACCESS_BARRING_TIMER;

  MSG_LOW_DS_0(MM_LOCAL_SUB, "=EMM= Posting access barring timer expiry message");
  mm_put_cmd(cmd_ptr);

} /* emm_post_access_barring_timer_expiry */

/*===========================================================================

FUNCTION   EMM_PROCESS_ACCESS_BARRING_TIMER_EXPIRY

DESCRIPTION
  This function processes the access barring timer expiry message. Attach, TAU
  or MO Detach procedure may be started.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_access_barring_timer_expiry
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  emm_update_attempt_type update_type = EMM_INITIAL_UPDATE;

  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_LOW_DS_0(MM_SUB, "=EMM= Processing posted access barring timer expiry message");
  
  if(emm_ctrl_data_ptr->ongoing_detach_procedure == TRUE)
  {
    /* 5.5.2.2.4 a: Restart DETACH when access is not barred any more */
    emm_start_mo_detach(emm_ctrl_data_ptr->ongoing_detach_type, FALSE, NULL, 
                        emm_detach_failed_routine, emm_ctrl_data_ptr) ;
  }
  else if((EMM_GET_STATE() == EMM_REGISTERED) &&
          (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_UPDATE_NEEDED))
  {
    /*reset the flag*/
    emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;

    if(emm_ctrl_data_ptr->emm_tau_attempt_counter != 0)
    {
      update_type = EMM_UPDATE_RECOVERY;
    }
    /* 5.5.3.2.6 a: Restart TAU when access is not barred any more 
       Note: Delayed TAU or TAU retry. No need to set TAU cause again in emm 
       data base */
    emm_initiate_tau_procedure(update_type, 
                               emm_get_eps_update_type(emm_ctrl_data_ptr), 
                               FALSE, 
                               emm_ctrl_data_ptr);
  }
  else if((EMM_GET_STATE() == EMM_DEREGISTERED) && 
          (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_DEREGISTERED_ATTACH_NEEDED))
  {
    if(emm_ctrl_data_ptr->emm_attach_attempt_counter != 0)
    {
      update_type = EMM_UPDATE_RECOVERY;
    }
    /* 5.5.3.2.6 a: Restart attach when access is not barred any more */
    emm_init_attach_procedure(update_type, emm_ctrl_data_ptr);
  }

} /* emm_process_access_barring_timer_expiry */

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
)
{
  emm_ctrl_data_type        *emm_ctrl_data_ptr = NULL;
  boolean                    post_timer_expiry_msg = FALSE;

#if defined  FEATURE_DUAL_SIM 
  /*need to find the multimode sub so that correct value can be assigned to emm_ctrl_data_ptr*/
  mm_as_id_e_type sub_id = MM_AS_ID_1;
  for (sub_id = MM_AS_ID_1; sub_id < (mm_as_id_e_type)MAX_AS_IDS; sub_id++)
  {
    if(mm_sub_capability_sim[sub_id] == SUBS_CAPABILITY_MULTIMODE)
    {
      break;
    }
  }
  if(sub_id >= (mm_as_id_e_type)MAX_AS_IDS)
  {
    return ;
  }
  emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs(sub_id);
#else
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif


  /* PROTECT BY SEMAPHORE */
  (void)pthread_mutex_lock(emm_ctrl_data_ptr->emm_mutex_ptr);
  emm_ctrl_data_ptr->access_barring_flag &= ~barring_mask ; // Stop barring
  (void)pthread_mutex_unlock(emm_ctrl_data_ptr->emm_mutex_ptr);
  /* PROTECT BY SEMAPHORE */

  switch(barring_mask)
  {
    case EMM_EMERGENCY_BARRED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Barring timer for EMERGENCY calls has expired");
      break;
    case EMM_MT_ACCESS_BARRED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Barring timer for MT ACCESS calls has expired");
      break;
    case EMM_MO_SIGNALING_BARRED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Barring timer for MO SIGNALING calls has expired");
      break;
    case EMM_MO_DATA_BARRED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Barring timer for MO DATA calls has expired");
      break;
#ifdef FEATURE_LTE_REL10
    case EMM_MO_CSFB_BARRED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Barring timer for MO CSFB calls has expired");
      break;
    case EMM_DELAY_TOLERANT_BARRED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Barring timer for Delay Tolerant calls has expired");
      break;
#endif
    default:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Illigal call type barring call back");
      break;
  }

  /* Check on state, whether Attach or Detach needs to be performed then set appropriate signal */
  if((barring_mask == (dword)EMM_MO_SIGNALING_BARRED) ||
     (barring_mask == (dword)EMM_DELAY_TOLERANT_BARRED))
  {
    if(emm_ctrl_data_ptr->ongoing_detach_procedure == TRUE)
    {
      post_timer_expiry_msg = TRUE;
    }
    else if((emm_ctrl_data_ptr->emm_state == EMM_REGISTERED) &&
            (emm_ctrl_data_ptr->emm_substate == (emm_substate_type)EMM_REGISTERED_UPDATE_NEEDED))
    {
      post_timer_expiry_msg = TRUE;
    }
    else if((emm_ctrl_data_ptr->emm_state == EMM_DEREGISTERED) && 
            (emm_ctrl_data_ptr->emm_substate == (emm_substate_type)EMM_DEREGISTERED_ATTACH_NEEDED))
    {
      post_timer_expiry_msg = TRUE;
    }

    if(post_timer_expiry_msg == TRUE)
    {
      /* EMM needs to start MO Detach/TAU/Attach procedure. Notify EMM the
         expiry of access barring timer and let mm context to handle it.*/
#if defined  FEATURE_DUAL_SIM 
      emm_post_access_barring_timer_expiry(sub_id);
#else
      emm_post_access_barring_timer_expiry();
#endif
    }
  }
} /* end of emm_barring_call_back() */

void emm_handle_barring_timer
(
  dword          *access_barring_flag,
  dword           barring_mask,
  dword           timer_length,
  rex_timer_type *timer
)
{
  boolean            stop_timer = TRUE ;

  ASSERT(access_barring_flag != (dword *)NULL) ; 

  ASSERT(timer != (rex_timer_type *)NULL) ; 

  if (timer_length == 0)
  {
    *access_barring_flag &= ~barring_mask ; // Stop barring

    stop_timer = TRUE ; // Stop timer
  }
  else if (timer_length == 0xFFFFFFFF) 
  {
    *access_barring_flag |= barring_mask ; // Keep barring

    stop_timer = TRUE ; // Stop timer
  }
  else
  {
    *access_barring_flag |= barring_mask ; // Keep barring

    stop_timer = FALSE ; // Start timer
  }

  if (rex_get_timer(timer) > 0)
  {
    (void)rex_clr_timer(timer) ;
  }

  if (stop_timer == FALSE)
  {
    (void)rex_set_timer(timer,timer_length) ;
  }

} /* end of emm_handle_barring_timer() */

void emm_start_connection_delay_timers
(
  byte                rrc_call_barring_info_field,
  dword               barring_timer_length,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  word parsing_index = 0;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  /* PROTECT BY SEMAPHORE */
  (void)pthread_mutex_lock(emm_ctrl_data_ptr->emm_mutex_ptr);

  for (parsing_index=0; parsing_index<TOTAL_NUMBER_OF_CALL_TYPES; parsing_index++)
  {
    if (((1<<parsing_index) & rrc_call_barring_info_field) != 0)
    {
      switch (parsing_index)
      {
        case LTE_RRC_EST_CAUSE_EMERGENCY:
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Start barring timer for EMERGENCY calls (%u msec)", barring_timer_length);

          emm_handle_barring_timer(&emm_ctrl_data_ptr->access_barring_flag,
              (dword)EMM_EMERGENCY_BARRED,barring_timer_length,&emm_ctrl_data_ptr->emm_access_barring_emergency_timer) ;
          break ;
        case LTE_RRC_EST_CAUSE_MT_ACCESS:
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Start barring timer for MT ACCESS calls (%u msec)", barring_timer_length);

          emm_handle_barring_timer(&emm_ctrl_data_ptr->access_barring_flag,
              (dword)EMM_MT_ACCESS_BARRED,barring_timer_length,&emm_ctrl_data_ptr->emm_access_barring_mt_access_timer) ;
          break ;
        case LTE_RRC_EST_CAUSE_MO_SIGNALING:
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Start barring timer for MO SIGNALING calls (%u msec)", barring_timer_length);

          emm_handle_barring_timer(&emm_ctrl_data_ptr->access_barring_flag,
              (dword)EMM_MO_SIGNALING_BARRED,barring_timer_length,&emm_ctrl_data_ptr->emm_access_barring_mo_signaling_timer) ;
          break ;
        case LTE_RRC_EST_CAUSE_MO_DATA:
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Start barring timer for MO DATA calls (%u msec)", barring_timer_length);

          emm_handle_barring_timer(&emm_ctrl_data_ptr->access_barring_flag,
              (dword)EMM_MO_DATA_BARRED,barring_timer_length,&emm_ctrl_data_ptr->emm_access_barring_mo_data_timer) ;
          break ;
#ifdef FEATURE_LTE_REL10
        case LTE_RRC_EST_CAUSE_MO_CSFB:
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            MSG_HIGH_DS_1(MM_SUB, "=EMM= Start barring timer for MO CSFB calls (%u msec)", barring_timer_length);
  
            emm_handle_barring_timer(&emm_ctrl_data_ptr->access_barring_flag,
                (dword)EMM_MO_CSFB_BARRED,barring_timer_length,&emm_ctrl_data_ptr->emm_access_barring_mo_csfb_timer) ;
          }
          break ;

        case LTE_RRC_EST_CAUSE_DELAY_TOLERANT_ACCESS:
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            MSG_HIGH_DS_1(MM_SUB, "=EMM= Start barring timer for Delay Tolerant calls (%u msec)", barring_timer_length);
  
            emm_handle_barring_timer(&emm_ctrl_data_ptr->access_barring_flag,
                (dword)EMM_DELAY_TOLERANT_BARRED,barring_timer_length,&emm_ctrl_data_ptr->emm_access_barring_delay_tolerant_timer) ;
          }
          break ;
#endif
        default:
          MSG_FATAL_DS(MM_SUB,"=EMM= Illigal call type barring",0,0,0) ;
          break ;
      }
    }
  }
  (void)pthread_mutex_unlock(emm_ctrl_data_ptr->emm_mutex_ptr);
  /* PROTECT BY SEMAPHORE */

} /* end of emm_start_connection_delay_timers() */


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
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL); 

 
  if(emm_ctrl_data_ptr->eab_barring == FALSE)
  {
    /* PROTECT BY SEMAPHORE */
    (void)pthread_mutex_lock(emm_ctrl_data_ptr->emm_mutex_ptr);

    emm_ctrl_data_ptr->access_barring_flag |= EMM_EAB_BARRING_BITMASK;

    (void)pthread_mutex_unlock(emm_ctrl_data_ptr->emm_mutex_ptr);
    /* PROTECT BY SEMAPHORE */

    /* There is a chance that MO_CSFB barring timer might be running due to ACB
       Hence make sure to stop the timer as well */
    if(rex_get_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_csfb_timer) > 0)
    {
      (void)rex_clr_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_csfb_timer) ;
    }

    emm_ctrl_data_ptr->eab_barring = TRUE;
  }
}

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
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL); 

 
  if(emm_ctrl_data_ptr->eab_barring == TRUE)
  {
    /* PROTECT BY SEMAPHORE */
    (void)pthread_mutex_lock(emm_ctrl_data_ptr->emm_mutex_ptr);

    emm_ctrl_data_ptr->access_barring_flag &= ~EMM_EAB_BARRING_BITMASK;

    (void)pthread_mutex_unlock(emm_ctrl_data_ptr->emm_mutex_ptr);
    /* PROTECT BY SEMAPHORE */

    emm_ctrl_data_ptr->eab_barring = FALSE;
    emm_process_access_barring_timer_expiry(emm_ctrl_data_ptr);
  }
}
#endif

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
)
{
  boolean result = FALSE;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  /* Start timer T3346 if lower layers provided a non-zero value for extendedWaitTime,
     and the conection establishment cause was delay tolerant access, else ignore the
     extendedWaitTime from lower layers */
  if((emm_ctrl_data_ptr->extended_wait_time != 0) &&
     (emm_ctrl_data_ptr->delay_tolerant_access == TRUE))
  {
    mm_stop_timer(TIMER_T3346);
    mm_start_timer(TIMER_T3346, (emm_ctrl_data_ptr->extended_wait_time * 1000));
    mm_is_lu_to_be_initiated_t3346_ltogw = TRUE;
    if(EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED)
    {
      gmm_set_gmm_is_congestion_due_to_rau(TRUE);
    }
    emm_save_t3346_plmn(emm_ctrl_data_ptr);
    result = TRUE;
  }

  return result;
} /* end emm_handle_extended_wait_time() */
#endif

/*===========================================================================
FUNCTION    EMM_PROCESS_NORMAL_CONNECTION_CONFIRMATION

DESCRIPTION
  This function processes the normal RRC connection establishment confirmation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_normal_connection_confirmation
(
  lte_rrc_conn_est_cnf_s *rrc_conn_est_cnf,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  emm_security_context_info *context_ptr = NULL;
  word over_flow_cnt = 0;
  emm_failure_type               emm_failure_cause;

  ASSERT(rrc_conn_est_cnf != (lte_rrc_conn_est_cnf_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  switch (rrc_conn_est_cnf->est_status)
  {
    case LTE_RRC_CONN_EST_SUCCESS:
      EMM_MOVES_TO_CONNECTED_STATE_STATE() ;
      /* Stop all EMM barring timers */
      emm_stop_cell_barring_timers(emm_ctrl_data_ptr
#ifdef FEATURE_NAS_EAB
                                   ,FALSE
#endif      
                                  );
      /* Stop Periodic TAU timer (T3412) */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Stop T3412 - Connected State");
      mm_stop_timer(TIMER_T3412);
      mm_stop_timer(TIMER_T3423);

      if((EMM_GET_STATE() != EMM_SERVICE_REQUEST_INITIATED) &&
         (EMM_GET_STATE() != EMM_TRACKING_AREA_UPDATING_INITIATED))
      {
        if(emm_ctrl_data_ptr->aborting_conn == TRUE)
        { 
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting connection - Do not send pending msg");
        }
        else
        {
          /* Send out all pended messages */
          emm_send_all_pended_messages(emm_ctrl_data_ptr); 
          emm_send_all_pending_irat_msg(emm_ctrl_data_ptr);
        }
      } 

      MSG_HIGH_DS_0(MM_SUB, "=EMM= RRC connection has been established successfully ");
      emm_invalidate_srvcc_ck_ik();
      break ;
    case LTE_RRC_CONN_EST_FAILURE_ABORTED:
    case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
    case LTE_RRC_CONN_EST_FAILURE:
    case LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
    case LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
    case LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
    case LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
    case LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
    case LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
    case LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
    case LTE_RRC_CONN_EST_FAILURE_TAI_CHANGE:                  
#if defined  FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
    case LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
#endif 
#ifdef FEATURE_NAS_EAB
    case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
      emm_ctrl_data_ptr->extended_wait_time = rrc_conn_est_cnf->extended_wait_time;
      MSG_HIGH_DS_2(MM_SUB, "=EMM= RRC connection est failed - %u, ext_wait_time = %d",
                    rrc_conn_est_cnf->est_status, emm_ctrl_data_ptr->extended_wait_time);

       /* Roll back the UL NAS count by 1 */
      context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);
      if( context_ptr != NULL  )
      {
        MSG_HIGH_DS_4(MM_SUB, "=EMM= context_ptr->nas_ul_cnt[0] = %d , context_ptr->nas_ul_cnt[1] = %d context_ptr->nas_ul_cnt[2] context_ptr->nas_ul_cnt[3] = %d", 
                                     context_ptr->nas_ul_cnt[0],context_ptr->nas_ul_cnt[1],context_ptr->nas_ul_cnt[2],context_ptr->nas_ul_cnt[3]);


        context_ptr->nas_ul_cnt[0] = context_ptr->nas_ul_cnt[0] - 1;
                
        over_flow_cnt = ( (word )context_ptr->nas_ul_cnt[2] << 8) | context_ptr->nas_ul_cnt[1];
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Since CONN Establishement failed, roll back the UL NAS count by 1 ,over_flow_cnt = %d",over_flow_cnt);
        if ((context_ptr->nas_ul_cnt[0] == 255) && (over_flow_cnt != 0))
        {
          over_flow_cnt--;
          context_ptr->nas_ul_cnt[1] = (byte )(over_flow_cnt & 0xFF);
          context_ptr->nas_ul_cnt[2] = (byte )((over_flow_cnt & 0xFF00) >> 8);
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Decrement over_flow_cnt to %d", over_flow_cnt);
        }
      
        MSG_HIGH_DS_4(MM_SUB, "=EMM= context_ptr->nas_ul_cnt[0] = %d , context_ptr->nas_ul_cnt[1] = %d context_ptr->nas_ul_cnt[2] context_ptr->nas_ul_cnt[3] = %d", 
                                   context_ptr->nas_ul_cnt[0],context_ptr->nas_ul_cnt[1],context_ptr->nas_ul_cnt[2],context_ptr->nas_ul_cnt[3]);

      }

      /*EMM IDLE event need not be sent to GSTK, as RRC est failed*/
      EMM_MOVES_TO_IDLE_STATE() ;
      if(rrc_conn_est_cnf->barring_info_is_present == FALSE)
      {
        rrc_conn_est_cnf->barring_timer_value = 0;
#ifdef FEATURE_NAS_EAB
        if(rrc_conn_est_cnf->est_status == LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB)
        {
          emm_start_eab_barring(emm_ctrl_data_ptr);
          rrc_conn_est_cnf->barring_timer_value = LTE_RRC_INDEFINITE_BARRING_TIME;
        }
#endif
      }
      else
      {
        emm_start_connection_delay_timers(rrc_conn_est_cnf->call_type_bitmask,
              (dword)rrc_conn_est_cnf->barring_timer_value, emm_ctrl_data_ptr);
      }
      emm_failure_cause.cause_type       = LTE_NAS_IRAT_CONN_EST_FAILURE;
      emm_failure_cause.cause.est_status = rrc_conn_est_cnf->est_status;
      emm_failure_cause.access_barring_rem_time =  
                                  rrc_conn_est_cnf->barring_timer_value;
      emm_process_unsent_messages(emm_translate_cnf_cause(rrc_conn_est_cnf->est_status),
                                  emm_failure_cause,
                                  emm_ctrl_data_ptr);

#ifdef FEATURE_LTE_REL10
      /* delay_tolerant_access flag is used to handle extendedWaitTime received from lower layers.
         extendedWaitTime received from lower layers will be already processed by now during
         emm_process_unsent_messages() call above. Reset delay_tolerant_access flag now. */
      emm_ctrl_data_ptr->delay_tolerant_access = FALSE;
#endif

      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_CONN_EST_FAILURE,
                                   (byte)rrc_conn_est_cnf->est_status,
                                   rrc_conn_est_cnf->barring_timer_value,
                                   emm_ctrl_data_ptr);

      if(emm_ctrl_data_ptr->aborting_conn == TRUE)
      { 
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved CONN CNF as rsp of CONN ABORT - Deact pending %d",
                      emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending);

        /* Reset this flag */
        emm_ctrl_data_ptr->aborting_conn = FALSE;

        /* Send rrc deact req out if it is pending upon aborting connection */
        if(emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE)
        {
          emm_send_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, emm_ctrl_data_ptr);
          emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;
        }

        if(emm_reg_container != NULL)
        {
          if (emm_ctrl_data_ptr->client_list != NULL)
          {
            emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
            emm_process_unsent_messages(CONN_CNF_LTE_RRC_CONN_EST_FAILURE,
                                        emm_failure_cause, 
                                        emm_ctrl_data_ptr);

            /* Process all pending irat uplink messages */
            emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                         LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                         LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                         emm_ctrl_data_ptr);
          }
          emm_reg_container(emm_ctrl_data_ptr) ;
        }
        else if (emm_ctrl_data_ptr->client_list != NULL)
        {
          emm_autonomous_connection_setup(emm_ctrl_data_ptr) ;
        }
      }

      if((mm_psm_ready_req_rejected == TRUE) &&
         (emm_check_ready_for_psm(emm_ctrl_data_ptr) == SYS_PSM_STATUS_READY))
      {
        mm_send_mmr_psm_ready_ind();
      }

      break ;
    default:
      /*changed to ERR_FATAL to catch the incompatibility between RRC and EMM*/
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal connection status %d",rrc_conn_est_cnf->est_status,0,0) ;
      break ;        
  }

} /* end of emm_process_normal_connection_confirmation() */

void emm_process_connection_confirmation
(
  lte_rrc_conn_est_cnf_s *rrc_conn_est_cnf,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  emm_connection_state_type emm_connection_state = EMM_IDLE_STATE ;

  ASSERT(rrc_conn_est_cnf != (lte_rrc_conn_est_cnf_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  emm_connection_state = EMM_GET_CONNECTION_STATE() ;

  switch(emm_connection_state)
  {
    case EMM_IDLE_STATE:
    case EMM_CONNECTED_STATE:
    case EMM_RELEASING_RRC_CONNECTION_STATE:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= LTE_RRC_CONN_EST_CNF primitive is out of sequence, discarded");
      break ;
    case EMM_WAITING_FOR_RRC_CONFIRMATION_STATE:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Received LTE_RRC_CONN_EST_CNF");
      emm_process_normal_connection_confirmation(rrc_conn_est_cnf, emm_ctrl_data_ptr) ;
      break ;
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM Connection state",0,0,0) ;
      break ;
  }
} /* end emm_process_connection_confirmation() */

/*===========================================================================
FUNCTION    EMM_PROCESS_NORMAL_CONNECTION_RELEASE

DESCRIPTION
  This function processes release indication sent from RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_normal_connection_release
(
  lte_rrc_conn_rel_ind_s *rrc_conn_rel_ind,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  lte_nas_tau_cause_type         tau_cause = TAU_CAUSE_INVALID;
  lte_emm_connection_cause_type  connection_cause;
  emm_connection_state_type      connection_state;
  emm_state_type                 emm_state = EMM_INVALID_STATE;
  emm_failure_type               emm_failure_cause;

  ASSERT(rrc_conn_rel_ind != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  memset(&emm_failure_cause,0,sizeof(emm_failure_type));

  connection_state = EMM_GET_CONNECTION_STATE();

  /* Double check the connection state */
  ASSERT(connection_state != EMM_IDLE_STATE);
  ASSERT(connection_state != EMM_WAITING_FOR_RRC_CONFIRMATION_STATE);

  emm_ctrl_data_ptr->extended_wait_time = rrc_conn_rel_ind->extended_wait_time;
  MSG_HIGH_DS_2(MM_SUB, "=EMM= connection_state = %d, ext_wait_time = %d ",
                connection_state, emm_ctrl_data_ptr->extended_wait_time);

  /* Connection is being released. Radio bearer will be configured by NW upon 
     next connection setup. Need to to sync drb status anymore. */
  emm_ctrl_data_ptr->sync_drb_status = FALSE;

  if(connection_state == EMM_CONNECTED_STATE)
  {
    /* 33.401, CR 0457 */
    /* Update USIM only when moving to deregistered or when transitioning away from Deregistered */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Moving to RELEASING_RRC_CONNECTION_STATE ");
    EMM_MOVES_TO_RELEASING_RRC_CONNECTION_STATE() ;
  }

  /*Stop MAK & SQN timers if running and bar the active cell*/
  if((rrc_conn_rel_ind->rel_reason == LTE_RRC_CONN_REL_NORMAL) &&
     (emm_stop_auth_timers(emm_ctrl_data_ptr) == TRUE))
  {
    emm_send_rrc_cell_bar_req(emm_ctrl_data_ptr);
  }

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Cancel Authentication");
  emm_cancel_authentication();
  
  /* EMM state may be updated during procedure release client processing and
     unsent message processing.
     The check on whether TAU should be started must be done before the above
     two processings */

  emm_state = EMM_GET_STATE();

  /* 24.301 5.5.3.2.2 e - Load balancing TAU is required */
  if(rrc_conn_rel_ind->load_balancing_tau_is_reqd == TRUE)
  {
    /* Trigger TAU if there is no EMM specific procedure ongoing. Otherwise 
       UE should follow the specific action for EMM specific procedure listed
       as abnormal case upon connection release */
    if((emm_state == EMM_REGISTERED) || 
       (emm_state == EMM_SERVICE_REQUEST_INITIATED))
    {
      tau_cause = TAU_CAUSE_LOAD_BALANCING;
    }
  }
  /* 24.301 5.5.3.2.2 i - RRC connection failure and no user data pending */
  else if((rrc_conn_rel_ind->rel_reason == LTE_RRC_CONN_REL_RLF) || 
          (rrc_conn_rel_ind->rel_reason == LTE_RRC_CONN_REL_CRE_FAILURE) ||
          (rrc_conn_rel_ind->rel_reason == LTE_RRC_CONN_REL_OOS_DURING_CRE))
  {
    /* Trigger TAU if there is no uplink user data pending in REGISTERED or 
       SERVICE_REQUEST_INITIATED state. For other state UE should follow the 
       specific action for EMM specific procedure listed as abnormal case 
       upon connection release */
    if(((emm_state == EMM_REGISTERED) || 
        (emm_state == EMM_SERVICE_REQUEST_INITIATED)) && 
        (ds3gpp_per_subs_is_ps_data_available((sys_modem_as_id_e_type)mm_sub_id) == FALSE) &&
        (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))
    {
      tau_cause = TAU_CAUSE_LOW_LAYER_FAILURE;
    }
  }

  emm_failure_cause.cause_type       = LTE_NAS_IRAT_CONN_REL;
  emm_failure_cause.cause.rel_reason = rrc_conn_rel_ind->rel_reason;

  connection_cause = emm_translate_rel_cause(rrc_conn_rel_ind->rel_reason);

  emm_call_connection_release_client(connection_cause, emm_failure_cause,
                                     emm_ctrl_data_ptr,
                                     rrc_conn_rel_ind->redir_info,tau_cause
                                     );  

#ifdef FEATURE_LTE_REL10
      /* delay_tolerant_access flag is used to handle extendedWaitTime received from lower layers.
         extendedWaitTime received from lower layers will be already processed by now during
         emm_call_connection_release_client() call above. Reset delay_tolerant_access flag now. */
      emm_ctrl_data_ptr->delay_tolerant_access = FALSE;
#endif
  
  emm_process_unsent_messages(connection_cause, emm_failure_cause, emm_ctrl_data_ptr);
  /* Process all pending irat uplink messages */
  emm_process_pending_irat_msg(LTE_NAS_IRAT_CONN_REL,
                               (byte)rrc_conn_rel_ind->rel_reason,
                               LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                               emm_ctrl_data_ptr);
  mm_stop_timer(TIMER_T3440);

  /* Connection was aborted */       
  if(emm_ctrl_data_ptr->aborting_conn == TRUE)
  {
    MSG_HIGH_DS(MM_SUB,"=EMM= Rcved CONN REL as rsp of CONN ABORT - Deact pending %d",
             emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending,0,0);

    /* Reset this flag */
    emm_ctrl_data_ptr->aborting_conn = FALSE;
  
    /* Send rrc deact req out if it is pending upon aborting connection */
    if(emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE)
    {
      emm_send_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, 
                             emm_ctrl_data_ptr);
      emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;
    }
  }

  /* If TAU needs to be started, check EMM state again since the state may be 
     updated in procedure replease client and usent message processing. 
     TAU should only be started if EMM is in REGISTERED state here. 
     i.e. RLF fails Attach Complete message. Attach retry is ongoing and EMM 
     state is no longer REGISTERED */
  emm_state = EMM_GET_STATE();

  /* Check if TAU needs to be triggered */
  if((emm_state == EMM_REGISTERED) && (tau_cause != TAU_CAUSE_INVALID))
  {
    emm_ctrl_data_ptr->tau_cause = tau_cause;
    /* Enter EMM_REGISTERED_UPDATE_NEEDED state. TAU will be initiated when RRC
       sends the RRC_SERVICE_IND that associated with the connection release */
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_UPDATE_NEEDED);
  }
  else
  {
    if (emm_reg_container != (emm_reg_container_type)NULL)
    {
      emm_reg_container(emm_ctrl_data_ptr) ;
    }
  }

  /* Send rel ind to CNM if necessary */
  if(emm_ctrl_data_ptr->emm_sms_active == TRUE)
  {
    /* send release indication to CNM to indicate failure of SMS started in LTE*/
    emm_send_mmcnm_rel_ind(emm_ctrl_data_ptr, PS_SMS_FAIL);
    emm_ctrl_data_ptr->emm_sms_active = FALSE;
  }

  /* Send ESM connection release indication */
  emm_esm_release_indication(connection_cause, emm_ctrl_data_ptr);

  /*If EMM is waiting for ESM response after receiving attach accept
      and connection get released then move to deregistred state.
      Attach will be started on receiving service indication */
  if ((emm_ctrl_data_ptr->emm_state == EMM_REGISTERED_INITIATED)
       && (emm_ctrl_data_ptr->emm_substate == EMM_WAITING_FOR_ESM_RESPONSE))
  {
    emm_build_and_send_detach_ind(EMM_DETACHED);
    emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
    emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_LIMITED;
    emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
  }

  if((emm_ctrl_data_ptr->t3412_value != 0) &&
     ((emm_state == EMM_REGISTERED) ||
      (emm_state == EMM_SERVICE_REQUEST_INITIATED) ||
      (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
      (emm_state == EMM_DEREGISTERED_INITIATED)))
  {
    /* Start Periodic TAU timer (T3412) */
    mm_start_timer(TIMER_T3412,emm_ctrl_data_ptr->t3412_value);
    MSG_HIGH_DS_1(MM_SUB,"=EMM= T3412 started, %d ms - Conn Rel", 
                  emm_ctrl_data_ptr->t3412_value);
  }
  else
  {
    MSG_HIGH_DS_2(MM_SUB,"=EMM= T3412 not started, %d ms, EMM state %d - Conn Rel", 
                  emm_ctrl_data_ptr->t3412_value,
                  EMM_GET_STATE());
  }
  
  if((rrc_conn_rel_ind->redir_info.is_redir == TRUE) &&
     (rrc_conn_rel_ind->redir_info.redir_rat == LTE_RRC_REDIR_RAT_CDMA2000_HRPD))
  {
    /* L to eHRPD redirection is going on. If redirection succeeds, CM will 
      issue STOP MODE IRAT. If redirection fails, RRC will send no servie ind.
      In both cases, RRC is inactive for NAS pespective. RRC will be active 
      again upon next service ind. */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= LTE to eHRPD redirection is active");
    emm_ctrl_data_ptr->rrc_active = FALSE;  
  }

  if((rrc_conn_rel_ind->redir_info.is_redir == TRUE) &&
     (rrc_conn_rel_ind->redir_info.redir_rat == LTE_RRC_REDIR_RAT_GERAN))
  {
    if (mm_csfb_get_state() == MM_CSFB_STATE_MO_CALL_ORIG)
    {
      emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = TRUE;
    }
  }
  else 
  { 
    emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;
  }

  emm_ctrl_data_ptr->esr_est_conn = FALSE;

  emm_invalidate_srvcc_ck_ik();

  if((mm_psm_ready_req_rejected == TRUE) &&
     (emm_check_ready_for_psm(emm_ctrl_data_ptr) == SYS_PSM_STATUS_READY))
  {
    mm_send_mmr_psm_ready_ind();
  }

} /* end of emm_process_normal_connection_release() */

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
)
{
  emm_connection_state_type emm_connection_state = EMM_IDLE_STATE;

  ASSERT(rrc_conn_rel_ind != (lte_rrc_conn_rel_ind_s *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  emm_connection_state = EMM_GET_CONNECTION_STATE() ;

  switch(emm_connection_state)
  {
    case EMM_IDLE_STATE:
    case EMM_WAITING_FOR_RRC_CONFIRMATION_STATE:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= LTE_RRC_CONN_REL_IND primitive is out of sequence, discarded");
      break ;
    case EMM_RELEASING_RRC_CONNECTION_STATE:
    case EMM_CONNECTED_STATE:
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Received LTE_RRC_CONN_REL_IND (%d)", 
                    rrc_conn_rel_ind->rel_reason);

      /* During RLF if RRC release connection, send SUITABLE_SEARCH_END_IND */
      if((emm_ctrl_data_ptr->last_rrc_service_ind_ptr->svc_status ==
                                              LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
         &&
         ((emm_ctrl_data_ptr->plmn_service_state->active_rat == 
                                                   SYS_RAT_LTE_RADIO_ACCESS) ||
          (emm_ctrl_data_ptr->aborting_conn == FALSE)))
      {
        if((emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&(rrc_conn_rel_ind->rel_reason != LTE_RRC_CONN_REL_RLF))
        {
          emm_ctrl_data_ptr->suitable_search_end_pending = TRUE;
        }
        else
        {
         emm_send_reg_t311_timer_expiry_ind();
        }
      }
      emm_process_normal_connection_release(rrc_conn_rel_ind, emm_ctrl_data_ptr) ;
      break ;
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM Connection state",0,0,0) ;
      break ;
  }
} /* end of emm_process_rrc_release_indication() */


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
)
{
  emm_connection_state_type connection_state = EMM_IDLE_STATE;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
    
  connection_state = EMM_GET_CONNECTION_STATE() ;

  if (connection_state == EMM_IDLE_STATE)
  {
    return FALSE ;
  }
  else
  {
    if (connection_state == EMM_CONNECTED_STATE)
    {
      emm_send_rrc_data_request(emm_message_ptr, sequence_number, 
                                emm_ctrl_data_ptr);      
      return TRUE;
    }
  }
  return FALSE;
}

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
)
{
  emm_connection_state_type connection_state ;
  emm_pended_msg_list_type *cached_message = NULL ;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
    
  connection_state = EMM_GET_CONNECTION_STATE() ;

  if (connection_state == EMM_IDLE_STATE)
  {
    return FALSE ;
  }
  else
  {
    cached_message = emm_cache_new_message(emm_message_ptr, sequence_number,
                                           client, message_callback, 
                                           connection_failure_callback, 
                                           emm_ctrl_data_ptr) ;
    
    if (connection_state == EMM_CONNECTED_STATE)
    {
      emm_send_rrc_data_request(emm_message_ptr, sequence_number, 
                                emm_ctrl_data_ptr);

      cached_message->message_sent_indicator = TRUE ;
    }
  }

  return TRUE ;
} /* end of emm_send_message() */

#endif /*FEATURE_LTE*/

