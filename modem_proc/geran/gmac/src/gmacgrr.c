/*****************************************************************************
***
*** TITLE
***
*** GPRS GRR-MAC ACCESS FUNCTION 
***
***
*** DESCRIPTION
***
***  This module provides the access function implementations to support the
***  GRR to MAC interface.
***
*** 
*** EXTERNALIZED FUNCTIONS
***
***  gprs_mac_grr_acc_func()
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
*** 
***  none
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacgrr.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 06/25/01   tlx     Created
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
#include "geran_tasks.h"
#include "gmacdef.h"
#include "gmacsig.h"
#include "task.h"
#include "cfa.h"
#include "gs.h"
#include "ms.h"
#include "msg.h"
#include "gprs_mem.h"
#include "string.h"
#include "gmacgrr.h"
#include "gmactask.h"
#include "gmactransfer_if.h"
#include "gmacproc.h"
#include "gmacds.h"

#include "gmutex.h"


/* Critical section to access control GMAC messages */

extern rex_crit_sect_type gmac_msg_crit_sec[NUM_GERAN_DATA_SPACES];


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      mac_get_cmd_buf_grr_mac()
===
===  DESCRIPTION
===  Allocate memory for message from GRR to MAC 
===   
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
grr_mac_sig_t *mac_get_cmd_buf_grr_mac(void)
{
  return ((grr_mac_sig_t *)GPRS_MEM_MALLOC(sizeof(grr_mac_sig_t) / sizeof(uint8)));
}

/*===========================================================================
===
===  FUNCTION      MAC_PUT_CMD_GRR_MAC()
===
===  DESCRIPTION
===   MAC access function. Used by GRR to send essages to MAC.
===
===   The function makes a copy of the message structure constructed by GRR
===   and places the copy into MAC's message queue.
===
===   INPUT
===   Pointer to MAC message structure of the message to be sent
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void mac_put_cmd_grr_mac(gas_id_t gas_id, grr_mac_sig_t *sig_ptr)
{
  geran_task_id_t task_id = GERAN_TASK_ID_GMAC;
  gmac_global_data_t *global_data_ptr = NULL;

  task_id = gmac_map_gas_id_to_task_id(gas_id);
  global_data_ptr = gmac_get_global_data_ptr(gas_id);

  switch (sig_ptr->sig_id)
  {
    case GRR_MAC_SEND_MEAS_RPT:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RR, (uint8)GRR_MAC_SEND_MEAS_RPT,
        sizeof( gm_send_meas_rpt_t ),
        (uint8 *)&sig_ptr->msg.gm_send_meas_rpt
      );
      break;

    case GRR_MAC_SEND_PCCF:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RR, (uint8)GRR_MAC_SEND_PCCF,
        sizeof( gm_send_pccf_t ),
        (uint8 *)&sig_ptr->msg.gm_send_pccf
      );
      break;

    case GRR_MAC_DL_TBF_EST:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RR, (uint8)GRR_MAC_DL_TBF_EST,
        sizeof( gm_dl_tbf_est_t ),
        (uint8 *)&sig_ptr->msg.gm_dl_tbf_est
      );
      break;

    case GRR_MAC_DL_SB_EST:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RR, (uint8)GRR_MAC_DL_SB_EST,
        sizeof( gm_dl_sb_est_t ),
        (uint8 *)&sig_ptr->msg.gm_dl_sb_est
      );
      break;

    case GRR_MAC_UL_TBF_EST:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RR, (uint8)GRR_MAC_UL_TBF_EST,
        sizeof( gm_ul_tbf_est_t ),
        (uint8 *)&sig_ptr->msg.gm_ul_tbf_est
      );
      break;

    case GRR_MAC_UL_SB_EST:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RR, (uint8)GRR_MAC_UL_SB_EST,
        sizeof( gm_ul_sb_est_t ),
        (uint8 *)&sig_ptr->msg.gm_ul_sb_est
      );
      break;

    case GRR_MAC_RACH_REQ:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_RR, (uint8)GRR_MAC_RACH_REQ,
        sizeof( gm_rach_req_t ),
        (uint8 *)&sig_ptr->msg.gm_rach_req
      );
      break;

    case GRR_MAC_NO_PS_ACCESS_IND:
    case GRR_MAC_PS_ACCESS_REQ: 
    case GRR_MAC_SEND_PSS_REQ:
    case GRR_MAC_SEND_PCCN_REQ:
    case GRR_MAC_NACC_RELEASE_REQ:
    case GRR_MAC_RESET_REQ:

#ifdef FEATURE_GSM_DTM
    case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
    case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
    case GRR_MAC_DTM_UL_DCCH_IND:
    case GRR_MAC_DTM_UL_DCCH_CNF:  
#endif /*FEATURE_GSM_DTM*/
#ifdef FEATURE_GSM_EDTM
    case GRR_MAC_EDTM_PKT_CS_EST_REQ:
#endif /*FEATURE_GSM_EDTM*/
#ifdef FEATURE_GPRS_PS_HANDOVER
    case GRR_MAC_PSHO_REQ:
    case GRR_MAC_PSHO_COMPLETE_IND:
#endif /*FEATURE_GPRS_PS_HANDOVER*/
      /* just ignore these messages for now, no CFA handler exists */
      break;

    default:
      MSG_GERAN_ERROR_0_G("MAC error CFA LOG unknown message");
      break;
  }

  GMAC_MSG_LOCK(gas_id);

  /* First initialize the queue link. This is always 
     done before putting an item in a queue. */
  (void)q_link(sig_ptr, &(sig_ptr->link));
  
  /* Put the message on the queue */
  q_put(&global_data_ptr->mac_q, &(sig_ptr->link));
  
  /* Set the command queue signal for receive task */
  (void)rex_set_sigs(geran_tcb_read(task_id), GPRS_MAC_MASTER_SIG);

  GMAC_MSG_UNLOCK(gas_id);
}

/*** EOF: don't remove! ***/
