/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TO RLC INTERFACE
***
***
*** DESCRIPTION
***
***  This module implements the MAC functions to allow RLC to send
***  messages to MAC.
***
***  Added code in mac_put_cmd_rlc() to intercept, convert and divert messages
***  for back-to-back test purposes
*** 
*** EXTERNALIZED FUNCTIONS
***
***  mac_get_cmd_buf_rlc()
***  mac_put_cmd_rlc()
***  mac_get_contention_status()
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  MAC task must be active and ready to receive messages from RLC
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacrlc.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 04/04/01    hv     Created MAC access function stub.
*** 06/04/01    hv     Added gprs_mac_contention_status() stub.
*** 01/11/02    hv     Split single 'access' function into get/put functions
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "geran_tasks.h"

#include "cfa.h"
#include "gs.h"
#include "ms.h"
#include "task.h"

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "gmacdef.h"
#include "gmactransfer_if.h"
#include "gmacproc.h"
#include "gmacds.h"

#include "gmutex.h"


/* Critical section to access control GMAC messages */

extern rex_crit_sect_type gmac_msg_crit_sec[NUM_GERAN_DATA_SPACES];



/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      MAC_GET_CMD_BUF_RLC()
===
===  DESCRIPTION
===
===   Return a pointer to a buffer for RLC to build a message destined 
===   for MAC
===    
===
===  DEPENDENCIES
===
===   none
===  
===  RETURN VALUE
===
===   pointer to rlc_mac_msg_t buffer
===
===  SIDE EFFECTS
===
===   none
===  
===========================================================================*/
rlc_mac_msg_t *mac_get_cmd_buf_rlc(void)
{
  return (rlc_mac_msg_t *)GPRS_MEM_MALLOC(BSIZE(rlc_mac_msg_t));
}


/*===========================================================================
===
===  FUNCTION      MAC_PUT_CMD_RLC()
===
===  DESCRIPTION
===
===   Send a message from RLC to MAC.
===
===   NOTE - This function is extended when DEBUG_GSM_GPRS_DS_DSPE_TEST or
===   DEBUG_GSM_GPRS_DS_TGT_TEST is defined, to intercept messages from
===   RLC to MAC. During these tests the MAC entity does not exist,
===   hence all messages RLC send to MAC to request to open a TBF or to
===   request for resources are being dealt with here.
===    
===
===  DEPENDENCIES
===
===    None
===  
===  RETURN VALUE
===
===    Pointer to MAC to RLC message type
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void mac_put_cmd_rlc(gas_id_t gas_id, rlc_mac_msg_t *rlc_msg_ptr)
{
/* ======================================================================= */
  geran_task_id_t task_id = GERAN_TASK_ID_GMAC; /*default Task ID*/
  gmac_global_data_t *global_data_ptr = NULL;

  task_id = gmac_map_gas_id_to_task_id(gas_id);
  global_data_ptr = gmac_get_global_data_ptr(gas_id);

  switch (rlc_msg_ptr->msg.sig)
  {
    case RM_UL_EST_TBF_REQ:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_UL_EST_TBF_REQ,
        sizeof( rm_ul_est_tbf_req_t ),
        (uint8 *)&rlc_msg_ptr->msg.ul_est_tbf_req
      );
      break;

    case RM_UL_REALLOC_REQ:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_UL_REALLOC_REQ,
        sizeof( rm_ul_realloc_req_t ),
        (uint8 *)&rlc_msg_ptr->msg.ul_realloc_req
      );
      break;

    case RM_UL_RELEASE_IND:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_UL_RELEASE_IND,
        sizeof( rm_ul_release_t ),
        (uint8 *)&rlc_msg_ptr->msg.ul_release_ind
      );
      break;

    case RM_UL_N3104_EXPIRY_IND:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_UL_N3104_EXPIRY_IND,
        sizeof( rm_ul_n3104_expiry_ind_t ),
        (uint8 *)&rlc_msg_ptr->msg.ul_n3104_expiry_ind
      );
      break;

    case RM_UL_RESET_CNF:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_UL_RESET_CNF,
        sizeof( rm_ul_reset_t ),
        (uint8 *)&rlc_msg_ptr->msg.ul_reset_cnf
      );
      break;

    case RM_DL_PKT_ACKNACK_REQ:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_DL_PKT_ACKNACK_REQ,
        sizeof( rm_dl_pkt_acknack_req_t ),
        (uint8 *)&rlc_msg_ptr->msg.dl_pkt_acknack_req
      );
      break;

    case RM_DL_PKT_CTRL_ACK_REQ:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_DL_PKT_CTRL_ACK_REQ,
        sizeof( rm_dl_pkt_ctrl_ack_req_t ),
        (uint8 *)&rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req
      );
      break;

    case RM_DL_FIRST_BLOCK_IND:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_DL_FIRST_BLOCK_IND,
        sizeof( rm_dl_first_block_ind_t ),
        (uint8 *)&rlc_msg_ptr->msg.dl_first_block_ind
      );
      break;

    case RM_DL_RELEASE_IND:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_DL_RELEASE_IND,
        sizeof( rm_dl_release_t ),
        (uint8 *)&rlc_msg_ptr->msg.dl_release_ind
      );
      break;

    case RM_DL_RESET_CNF:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_DL_RESET_CNF,
        sizeof( rm_dl_reset_t ),
        (uint8 *)&rlc_msg_ptr->msg.dl_reset_cnf
      );
      break;

    case RM_PS_ACCESS_CNF:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RLC, (uint8)RM_PS_ACCESS_CNF,
        sizeof( rm_dl_release_t ),
        (uint8 *)&rlc_msg_ptr->msg.dl_release_ind
      );
      break;

    default:
      MSG_GERAN_ERROR_0_G("MAC error CFA LOG unknown message");
      break;
  }

  /* add task_id which is not set by RLC */
  rlc_msg_ptr -> task_id = MS_MAC_RLC;
  
  GMAC_MSG_LOCK(gas_id);
     
  /* init q item by pointing to data area */
  (void)q_link(rlc_msg_ptr, &rlc_msg_ptr->link);
  
  /* Put the message on the queue */
  q_put(&global_data_ptr->mac_q, &(rlc_msg_ptr->link));

  /* Set the command queue signal for receive task */
  (void)rex_set_sigs(geran_tcb_read(task_id), GPRS_MAC_MASTER_SIG);

  GMAC_MSG_UNLOCK(gas_id);

} /* end of mac_put_cmd_rlc() */


/*** EOF: don't remove! ***/

