/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC TO MAC INTERFACE
***
***
*** DESCRIPTION
***
***  This module implements the RLC functions to allow MAC to send
***  messages to RLC uplink task and RLC downlink task.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  grlc_get_cmd_buf_mac()
***  grlc_put_cmd_mac()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  RLC UL/DL tasks must be active and ready to receive messages from MAC
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcmac.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/29/01    hv     Created
*** 01/11/02    hv     Split single 'access' function into get/put functions
*** 09/19/12    hv     Added dual data space functionality
*** 10/12/12    hv     CR426461
*** 20/08/13    hv     CR531564
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "comdef.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "rex.h"
#include "gprs_mem.h"
#include "grlcutil.h"
#include "grlci.h"
#include "geran_msgs.h"
#include "grlcdl.h"
#include "grlcul.h"
#include "grlcmac.h"
#include "grlctsk.h"

#include "cfa.h"
#include "gs.h"
#include "ms.h"
#include "task.h"
#include "grlctx.h"
#include "geran_tasks.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/



/*****************************************************************************
***
***     Private Typedefs
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
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/



/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/



/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GRLC_GET_CMD_BUF_MAC()
===
===  DESCRIPTION
===
===   Return a pointer to a buffer for MAC to build a message destined
===   for RLC
===
===
===  DEPENDENCIES
===
===   none
===
===  RETURN VALUE
===
===   pointer to a MAC-to-RLC message buffer
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
mac_rlc_msg_t *grlc_get_cmd_buf_mac (void)
{
  return (mac_rlc_msg_t *)GPRS_MEM_MALLOC(BSIZE(mac_rlc_msg_t));
}

/*===========================================================================
===
===  FUNCTION      GRLC_PUT_CMD_MAC()
===
===  DESCRIPTION
===
===   Send the given message from MAC to RLC UL or RLC DL tasks
===
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_put_cmd_mac (gas_id_t gas_id, mac_rlc_msg_t *msg_ptr)
{
  if (msg_ptr == NULL)
  {
    ERR_GERAN_FATAL_0_G("GRLC MAC sends null-ptr-msg to RLC");
  }
  else
  {
    gmac_store_trace_send_msg_buf(gas_id, MS_MAC_RLC, msg_ptr);
    switch(msg_ptr->msg.sig)
    {
      /*----------- MAC MSG TO RLC UPLINK -----------*/

      case MR_UL_ALLOC_CNF:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_ALLOC_CNF,
          sizeof( mr_ul_alloc_cnf_t ),
          (uint8 *)&msg_ptr->msg.ul_alloc_cnf
        );
        break;

      case MR_UL_ACC_REJ_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_ACC_REJ_IND,
          sizeof( mr_ul_acc_rej_ind_t ),
          (uint8 *)&msg_ptr->msg.ul_acc_rej_ind
        );
        break;

      case MR_UL_CONTENTION_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_CONTENTION_IND,
          sizeof( mr_ul_tlli_ind_t ),
          (uint8 *)&msg_ptr->msg.ul_tlli_ind
        );
        break;

      case MR_UL_PKT_ACKNACK_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_PKT_ACKNACK_IND,
          sizeof( mr_ul_pkt_acknack_ind_t ),
          (uint8 *)&msg_ptr->msg.ul_pkt_acknack_ind
        );
        break;

      case MR_UL_TIMER_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_TIMER_IND,
          sizeof( mr_ul_timer_ind_t ),
          (uint8 *)&msg_ptr->msg.ul_timer_ind
        );
        break;

      case MR_UL_RELEASE_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_RELEASE_IND,
          sizeof( mr_ul_release_t ),
          (uint8 *)&msg_ptr->msg.ul_release_ind
        );
        break;

      case MR_UL_CELL_RESEL_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_CELL_RESEL_IND,
          sizeof( mr_ul_cell_resel_ind_t ),
          (uint8 *)&msg_ptr->msg.ul_cell_resel_ind
        );
        break;

      case MR_UL_RESET_REQ:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_RESET_REQ,
          sizeof( mr_ul_reset_t ),
          (uint8 *)&msg_ptr->msg.ul_reset_req
        );
        break;

      case MR_PS_ACCESS_REQ:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_PS_ACCESS_REQ,
          sizeof( mr_ps_access_req_t ),
          (uint8 *)&msg_ptr->msg.ps_access_req
        );
        break;

      case MR_NO_PS_ACCESS_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_NO_PS_ACCESS_IND,
          sizeof( mr_no_ps_access_ind_t ),
          (uint8 *)&msg_ptr->msg.no_ps_access_ind
        );
        break;

#ifdef FEATURE_GPRS_PS_HANDOVER
      case MR_UL_PSHO_START_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_PSHO_START_IND,
          sizeof( mr_ul_psho_start_ind_t ),
          (uint8 *)&msg_ptr->msg.ul_psho_start_ind
        );
        break;

      case MR_UL_PSHO_COMPLETE_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_UL_PSHO_COMPLETE_IND,
          sizeof( mr_ul_psho_complete_ind_t ),
          (uint8 *)&msg_ptr->msg.ul_psho_complete_ind
        );
        break;

#endif /* FEATURE_GPRS_PS_HANDOVER */

      /*----------- MAC MSG TO RLC DOWNLINK -----------*/

      case MR_DL_ASS_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_DL, MS_MAC_RLC, (uint8)MR_DL_ASS_IND,
          sizeof( mr_dl_ass_ind_t ),
          (uint8 *)&msg_ptr->msg.dl_ass_ind
        );
        break;

      case MR_DL_TIMER_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_DL, MS_MAC_RLC, (uint8)MR_DL_TIMER_IND,
          sizeof( mr_dl_timer_ind_t ),
          (uint8 *)&msg_ptr->msg.dl_timer_ind
        );
        break;

      case MR_DL_RELEASE_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_DL, MS_MAC_RLC, (uint8)MR_DL_RELEASE_IND,
          sizeof( mr_dl_release_t ),
          (uint8 *)&msg_ptr->msg.dl_release_ind
        );
        break;

      case MR_DL_TEST_MODE_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_DL, MS_MAC_RLC, (uint8)MR_DL_TEST_MODE_IND,
          sizeof( mr_dl_test_mode_t ),
          (uint8 *)&msg_ptr->msg.dl_test_mode_ind
        );
        break;

      case MR_DL_RESET_REQ:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_DL, MS_MAC_RLC, (uint8)MR_DL_RESET_REQ,
          sizeof( mr_dl_reset_t ),
          (uint8 *)&msg_ptr->msg.dl_reset_req
        );
        break;

#ifdef FEATURE_GPRS_PS_HANDOVER
      case MR_DL_PSHO_START_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_DL_PSHO_START_IND,
          sizeof( mr_dl_psho_start_ind_t ),
          (uint8 *)&msg_ptr->msg.dl_psho_start_ind
        );
        break;

      case MR_DL_PSHO_COMPLETE_IND:
        cfa_log_packet_ex
        (
          GS_QUEUE_RLC_UL, MS_MAC_RLC, (uint8)MR_DL_PSHO_COMPLETE_IND,
          sizeof( mr_dl_psho_complete_ind_t ),
          (uint8 *)&msg_ptr->msg.dl_psho_complete_ind
        );
        break;

#endif /* FEATURE_GPRS_PS_HANDOVER */


      default:
        MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_112, "GRLC error CFA LOG unknown message");
        break;
    } /* switch on sig */

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* init q item by pointing to data area
    */
    (void)q_link(msg_ptr, &msg_ptr->link);

    /* Parse the signal according to UL or DL operation
    */
    switch (msg_ptr->msg.sig)
    {
      /* Assign value of grlc_ul_block_counter to packet uplink acknack message
      */
      case MR_UL_PKT_ACKNACK_IND:
        msg_ptr->msg.ul_pkt_acknack_ind.blk_cnt = grlc_ul_block_counter[gas_id];
        /* add command to rlc ul MAC queue and set the appropriate signal
        */
        GRLC_UL_MSG_LOCK(gas_id);

        q_put(&UMQ(gas_id), &msg_ptr->link);
        RLC_UL_SET_MAC_SIG(gas_id);

        GRLC_UL_MSG_UNLOCK(gas_id);
        break;

      case MR_UL_CONTENTION_IND:
        msg_ptr->msg.ul_tlli_ind.blk_cnt = grlc_ul_block_counter[gas_id];
        /* add command to rlc ul MAC queue and set the appropriate signal
        */
        GRLC_UL_MSG_LOCK(gas_id);

        q_put(&UMQ(gas_id), &msg_ptr->link);
        RLC_UL_SET_MAC_SIG(gas_id);

        GRLC_UL_MSG_UNLOCK(gas_id);
        break;

      case MR_UL_ALLOC_CNF:
      case MR_UL_ACC_REJ_IND:
      case MR_UL_TIMER_IND:
      case MR_UL_RELEASE_IND:
      case MR_UL_CELL_RESEL_IND:
      case MR_UL_RESET_REQ:
/********************************************************
*                     IPA NEW SIGNALS                   *
********************************************************/
      case MR_PS_ACCESS_REQ:
      case MR_NO_PS_ACCESS_IND:
#ifdef FEATURE_GPRS_PS_HANDOVER
      case MR_UL_PSHO_START_IND:
      case MR_UL_PSHO_COMPLETE_IND:
#endif /* FEATURE_GPRS_PS_HANDOVER */
        /* add command to rlc ul MAC queue and set the appropriate signal
        */
        GRLC_UL_MSG_LOCK(gas_id);

        q_put(&UMQ(gas_id), &msg_ptr->link);
        RLC_UL_SET_MAC_SIG(gas_id);

        GRLC_UL_MSG_UNLOCK(gas_id);
        break;


      /* MAC indicates to RLC of: DL ASS, TIMER IND or RELEASE IND
      */
      case MR_DL_ASS_IND:
      case MR_DL_TIMER_IND:
      case MR_DL_RELEASE_IND:
      case MR_DL_TEST_MODE_IND:
      case MR_DL_RESET_REQ:

#ifdef FEATURE_GPRS_PS_HANDOVER
      case MR_DL_PSHO_START_IND:
      case MR_DL_PSHO_COMPLETE_IND:
#endif /* FEATURE_GPRS_PS_HANDOVER */

        /* add command to rlc dl MAC queue and set the appropriate signal
        */
        GRLC_UL_MSG_LOCK(gas_id);

        q_put(&DMQ(gas_id), &msg_ptr->link);
        RLC_DL_SET_MAC_SIG(gas_id);

        GRLC_UL_MSG_UNLOCK(gas_id);
        break;

      default:
        /* Unexpected MAC signal
        */
        MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_113, "Invalid MAC sig %d",msg_ptr->msg.sig);
        break;
    } /* end switch */
  } /* msg_ptr OK */
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_IS_CV_ACTIVE()
===
===  DESCRIPTION
===
===   This access function is intended for MAC to check if RLC is in transfer
===   and counting down in E-DTM operation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===   Return TRUE if RLC is in uplink transfer mode and transmission has
===   reached a data block with an active countdown value, ie values other
===   than 15. Return FALSE otherwise
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
boolean grlc_ul_is_cv_active ( gas_id_t gas_id )
{
  return ( ( (ul[gas_id].state == US_ACK_XFER || ul[gas_id].state == US_UNACK_XFER) &&
             IS_TX_CV_ACTIVE(gas_id)
             &&
             !(IS_EXT_UTBF_ACTIVE(gas_id))
             ) ?
           TRUE :
           FALSE
         );
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_RESTART_AWAIT_ASS_GUARD()
===
===  DESCRIPTION
===
===   This access function is intended for MAC to restart RLC_AWAIT_ASS_GUARD
===   by the time in milliseconds of the start time rcvd in UL assignment msg.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
void grlc_ul_restart_await_ass_guard (gas_id_t gas_id, uint32 time_ms)
{
  if (TMR_ACTIVE == ul[gas_id].timer_await_ass_guard.state)
  {
    (void)rex_set_timer(&(ul[gas_id].timer_await_ass_guard.rex_timer), time_ms);
    MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_114, "GRLU re-start await_ass_guard timer. Time in ms %d", time_ms);
  }
} /* grlc_ul_restart_await_ass_guard */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_STOP_AWAIT_ASS_GUARD()
===
===  DESCRIPTION
===
===   This access function is intended for MAC to stop RLC_AWAIT_ASS_GUARD
===   when it is waiting for a downlink acknack message to piggy back the uplink
===   request. Since a PDAN may not exist, the timer may expire yet no request
===   had been sent
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
void grlc_ul_stop_await_ass_guard (gas_id_t gas_id)
{
  TIMER_STOP_AWAIT_ASS_GUARD(gas_id);
} /* grlc_ul_restart_await_ass_guard */



/*** EOF: don't remove! ***/

