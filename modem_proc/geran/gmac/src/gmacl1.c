/*****************************************************************************
***
*** TITLE
***
***  GPRS L1-MAC ACCESS FUNCTION
***
***
*** DESCRIPTION
***
***  This module provides the access function implementations to support the
***  L1 to MAC interface.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  gprs_mac_l1_acc_func()
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacl1.c#1 $
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
#include "geran_tasks.h"

#include <stdlib.h>
#include "string.h"

#include "gprs_mem.h"
#include "gmacl1.h"

#include "cfa.h"
#include "gs.h"
#include "ms.h"

#ifdef PL1_FW_SIM
#include "pl1_test_harness.h"
#else
#include "nas_exp.h"
#include "ms.h"
#include "gmactask.h"
#include "task.h"
#include "queue.h"
#include "geran_msgs.h"
#include "gmacdef.h"
#include "gmactransfer_if.h"
#include "gmacproc.h"
#include "gmacds.h"
#include "stringl/stringl.h"


#ifdef FEATURE_FTM_EGPRS_BER
#include "geran_test_mode_api.h"
#include "ftm_egprs_ber.h"
extern boolean L1_ftm_mode_enabled( void);

#endif

/* Uncomment for use with test harness */
// #define PL1_TGT_TEST

#endif /* PL1_FW_SIM */

#include "gmutex.h"


/* Critical section to access control GMAC messages */

extern rex_crit_sect_type gmac_msg_crit_sec[NUM_GERAN_DATA_SPACES];


l1_mac_sig_t *mac_get_cmd_buf_l1_mac(void);
void mac_put_cmd_l1_mac(l1_mac_sig_t *sig_ptr, gas_id_t gas_id);

#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */


/*****************************************************************************
***
***     Private Definitions
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GPRS_MAC_l1_ACC_FUNC()
===
===  DESCRIPTION
===    MAC access function. Used by L1 to send L1 messages to MAC.
===
===    The function makes a copy of the message structure constructed by L1
===    and places the copy into MAC's message queue.
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
/* local fn to fill out the message for L1/MAC or L1/FTM */
static boolean fill_out_the_message(l1_mac_sig_t *common_msg_ptr, uint8 *pacch_data,
                                 l1_mac_sig_t *l1_mac_sig_ptr, uint32 bfr_size)
{
  boolean status = TRUE;
  if (l1_mac_sig_ptr != NULL)
  {
    /* Clear out buffer */
    memset((void *)l1_mac_sig_ptr, 0, bfr_size);

    /* copy param to the QUEUE */
    memscpy(l1_mac_sig_ptr, bfr_size,common_msg_ptr, bfr_size);

    /* pacch_data present therefore must be MAC_PH_DATA_IND */
    if (pacch_data != NULL)
    {
      /* Copy 23 bytes of pacch_data onto MAC queue */
      memscpy(l1_mac_sig_ptr->msg.data_ind.dl_ctrl_block,
             sizeof(l1_mac_sig_ptr->msg.data_ind.dl_ctrl_block),
             pacch_data, 23);
    }
  }
  else
  {
    ERR_GERAN_FATAL_0("ERROR: Unable to send message to MAC");
    status = FALSE;
  }
  return status;
}

void gprs_mac_l1_acc_func (l1_mac_sig_t *common_msg_ptr, uint8 *pacch_data,
                           gas_id_t gas_id)
{
  l1_mac_sig_t *l1_mac_sig_ptr;

  #ifdef FEATURE_FTM_EGPRS_BER
  if( L1_ftm_mode_enabled( ) == TRUE)
  {
    l1_mac_sig_ptr = ( l1_mac_sig_t *) ftm_egprs_ber_get_cmd_buf();

    /* FTM uses a subset of the l1_mac_sig_t union
       so reduce the buffer size */
    if (fill_out_the_message(common_msg_ptr, pacch_data,
                             l1_mac_sig_ptr, sizeof(geran_test_mode_api_l1_mac_sig_t)))
    {
      ftm_egprs_ber_send_msg_to_ftm((void *) l1_mac_sig_ptr);
    }
  }
  else
  #endif

  /* Memory type determines the type of pointer returned. */
  /* If dynamic_memory_type == TRUE then allocate dynamic memory */
  {
     l1_mac_sig_ptr = mac_get_cmd_buf_l1_mac();

    if (l1_mac_sig_ptr != NULL)
    {
      if (fill_out_the_message(common_msg_ptr, pacch_data,
                               l1_mac_sig_ptr, sizeof(l1_mac_sig_t)))
      {
        mac_put_cmd_l1_mac(l1_mac_sig_ptr, gas_id);
      }
    }
  }
}

#ifndef TEST_FRAMEWORK
/*===========================================================================
===
===  FUNCTION      mac_get_cmd_buf_l1_mac()
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
l1_mac_sig_t *mac_get_cmd_buf_l1_mac(void)
{
#ifndef PL1_FW_SIM
  return (l1_mac_sig_t *)GPRS_MEM_MALLOC(sizeof(l1_mac_sig_t) / sizeof(uint8));
#else
  return (l1_mac_sig_t *)malloc( sizeof(l1_mac_sig_t) / sizeof(byte) );
#endif
}

/*===========================================================================
===
===  FUNCTION      mac_put_cmd_l1_mac()
===
===  DESCRIPTION
===  Add MAC message to MAC signal queue.
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
void mac_put_cmd_l1_mac(l1_mac_sig_t *sig_ptr, gas_id_t gas_id)
{
  geran_task_id_t task_id = GERAN_TASK_ID_GMAC;
  gmac_global_data_t *global_data_ptr = NULL;

  task_id = gmac_map_gas_id_to_task_id(gas_id);
  global_data_ptr = gmac_get_global_data_ptr(gas_id);

  switch (sig_ptr->sig_id)
  {
    case L1_MAC_FREQ_ASS_CNF:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)L1_MAC_FREQ_ASS_CNF,
        sizeof( l1_mac_tbf_freq_ass_cnf_t ),
        (uint8 *)&sig_ptr->msg.tbf_freq_ass_cnf
      );
      break;

    case L1_MAC_USF_DETECTED:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)L1_MAC_USF_DETECTED,
        0,
        NULL
      );
      break;

    case L1_MAC_QUAL_MEASURMENTS:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)L1_MAC_QUAL_MEASURMENTS,
        sizeof( l1_mac_qual_measurements_t ),
        (uint8 *)&sig_ptr->msg.qual_measurements
      );
      break;

    case L1_MAC_1ST_DATA_BLOCK_SENT:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)L1_MAC_1ST_DATA_BLOCK_SENT,
        0,
        NULL
      );
      break;

    case L1_MAC_51_SB_FAILURE:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)L1_MAC_51_SB_FAILURE,
        0,
        NULL
      );
      break;

    case L1_MAC_TBF_REL_CONFIRM:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)L1_MAC_TBF_REL_CONFIRM,
        0,
        NULL
      );
      break;

    case MAC_PH_CONNECT_IND:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)MAC_PH_CONNECT_IND,
        sizeof( mac_ph_connect_ind_t ),
        (uint8 *)&sig_ptr->msg.connect_ind
      );
      break;

    case MAC_PH_DATA_IND:
#if 0
    /* due to excessive output, this has been removed from CFA logging */
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)MAC_PH_DATA_IND,
        sizeof( mac_ph_data_ind_t ),
        (uint8 *)&sig_ptr->msg.data_ind
      );
#endif
      break;

    case MAC_PH_RANDOM_ACCESS_CONF:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)MAC_PH_RANDOM_ACCESS_CONF,
        sizeof( mac_ph_random_access_cnf_t ),
        (uint8 *)&sig_ptr->msg.random_access_cnf
      );
      break;

    case L1_MAC_RRBP_FAILED_IND:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)L1_MAC_RRBP_FAILED_IND,
        0, NULL);
      break;

    case L1_MAC_SINGLE_BLOCK_SENT:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)L1_MAC_SINGLE_BLOCK_SENT,
        0, NULL);
      break;

#ifdef FEATURE_GSM_EDTM
    case MAC_PH_CS_CONNECT_IND:
      cfa_log_packet_ex
      (
        GS_QUEUE_MAC, MS_MAC_L1, (uint8)MAC_PH_CS_CONNECT_IND,
        0, NULL);
      break;
#endif /*FEATURE_GSM_EDTM*/

    default:
      MSG_GERAN_ERROR_1_G("MAC error CFA LOG unknown message :0x%x",sig_ptr->sig_id);
      break;
  }

   /* First initialize the queue link. This is always done
      before putting an item in a queue. */
   (void)q_link(sig_ptr, &(sig_ptr->link));

   /* Ensure the q_put( ) and rex_set_sigs( ) calls are atomic */

   GMAC_MSG_LOCK(gas_id);

   /* Put the message on the queue */
#ifdef PL1_FW_SIM
   q_put(&l1_th_q1, &(sig_ptr->link));

   /* send to test harness on MAC QUEUE */
     (void)rex_set_sigs(&tcb_pl1_test, TH_MSG_SIG1);
#else

#ifdef PL1_TGT_TEST
   #error code not present
#else
   /* Set the command queue signal for */
   /* receive task                     */
   q_put(&global_data_ptr->mac_q, &(sig_ptr->link));

   (void)rex_set_sigs(geran_tcb_read(task_id), GPRS_MAC_MASTER_SIG);

#endif /* PL1_TGT_TEST */

#endif /* PL1_FW_SIM */

   GMAC_MSG_UNLOCK(gas_id);
}
#endif /* TEST_FRAMEWORK */

/*** EOF: don't remove! ***/
