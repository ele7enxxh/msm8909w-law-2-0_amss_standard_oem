/*============================================================================
  FILE:         uim_rxstates.c

  OVERVIEW:     File contains the uart interrupt service routines.

  DEPENDENCIES: N/A

                Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_rxstates.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
05/30/16    gm         F3 message removal
02/28/15    ks         Explicit MF selection logic revisited
02/05/15    ks         MF selection when stream select receives '6A 82'
11/12/14   sam         Added handling for new RX state - 
                       UIM_T_1_RX_LEV_RECEIVED_WITH_PARITY_ERRORS
09/18/14    akv        UIMDRV UT enhancements
08/06/14    js         Handle SW 6280, 63C1 for non-telecom ADFs for activate ADF
07/14/14    ssr        Allow GET RESPONSE upon warnings for GET STATUS
06/23/14    ssr        Fix for 0x6FXX status word handling for read and select 
                       request
06/10/14    ll         Move NULL counter out of the flag for easier debugging
06/04/14    na/js      Handling of case 4 commands for non telecom apps
03/31/14    ll         Change stop at infinite NULL flag name for readability 
02/20/14    ak         Fix compile errors due to strict compiler on 9x35
xx/xx/xx    akv        Initial Revision
============================================================================*/
#include "uimdrv_msg.h"
#include "uimglobals.h"
#include "uimdrv.h"
#include "uimt1.h"
#include "crc.h"
#include "uim_rxtx.h"
#include "uimdrv_uartdm.h"


/*===========================================================================
  I N T E R N A L  F U N C T I O N S  P R O T O T Y P E S
===========================================================================*/
boolean uimdrv_rx_event_cb
(
  uim_instance_global_type *uim_ptr,
  dword interrupt_processed,
  dword uart_status,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
boolean uimdrv_rx_sub_state_machine
(
  uim_instance_global_type *uim_ptr,
  dword interrupt_processed,
  dword uart_status,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
void uimdrv_rx_state_machine
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
void uim_rx_isr_process_idle
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
void uim_rx_isr_receive_atr
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
void uim_atr_check_for_inverse_convention
(
  uim_instance_global_type *uim_ptr,
  byte rx_value,
  boolean* reinit_rxinit_ptr
);
void uim_rx_isr_atr_process_format_bytes
(
  uim_instance_global_type *uim_ptr,
  uim_atr_pps_rx_mode_type receive_mode,
  byte rx_value,
  boolean* reinit_rxinit_ptr,
  boolean* error_processing_atr_bytes_ptr
);
void uim_rx_isr_receive_pps
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
void uim_rx_isr_pps_process_format_bytes
(
  uim_instance_global_type *uim_ptr,
  uim_atr_pps_rx_mode_type receive_mode,
  byte rx_value,
  boolean* reinit_rxinit_ptr,
  boolean* error_processing_pps_bytes_ptr
);
void uim_rx_isr_process_overrun
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
void uim_rx_isr_proc_procedure_bytes
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
void uim_rx_isr_evaluate_procedure_bytes
(
  uim_instance_global_type *uim_ptr,
  byte result,
  boolean* reinit_rxtransfer_ptr,
  boolean* error_processing_proc_bytes_ptr
);
void uim_rx_isr_process_sw2_byte
(
  uim_instance_global_type *uim_ptr,
  byte sw1,
  byte sw2
);
void uim_rx_isr_sw1_normal_end
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_fetch_end
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_dload_err
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_nfc
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_resp_end
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_busy
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_retries
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_reference
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_security
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_p3_bad
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_p1_p2_bad
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_ins_bad
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_problem
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_resend_apdu
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_warnings1
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_warnings2
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_execn_err2
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_cmd_not_allowed
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_wrong_params
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_se_fail
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_execn_err1
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_sw1_cla_not_supported
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_process_unknown_proc_byte
(
  uim_instance_global_type *uim_ptr
);
#if defined( FEATURE_UIM_T_1_SUPPORT )
void uim_rx_isr_recv_t_1_block
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
);
void uim_rx_isr_send_prologue
(
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_send_information
(
  uim_instance_global_type *uim_ptr
);
void uim_rx_isr_send_epilogue
(
  uim_instance_global_type *uim_ptr
);
#endif /* FEATURE_UIM_T_1_SUPPORT */


/**
 * This function is called by the UART RXTX module when it would
 * like to notify UIMDRV of any event that it has just
 * processed.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param interrupt_processed Interrupt processed by UARTDM.
 * @param uart_status UARTDM status evaluated for parity and
 *                    overrun errors.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 * @return rx_event_cb_err Indicating if there was any error
 *         while processing the rx_sub_states and states.
 *         Recovery could be triggered or the uart model could
 *         error fatal.
 */
boolean uimdrv_rx_event_cb
(
  uim_instance_global_type *uim_ptr,
  dword interrupt_processed,
  dword uart_status,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
)
{
  boolean rx_sub_state_err = FALSE;
  boolean rx_event_cb_err = FALSE;

  if (uim_ptr == NULL || uart_rx_buf_ptr == NULL)
  {
    rx_event_cb_err = TRUE;
    return rx_event_cb_err;
  }

  rx_sub_state_err = uimdrv_rx_sub_state_machine(uim_ptr, interrupt_processed, uart_status, uart_rx_buf_ptr, no_of_bytes_received);

  if(rx_sub_state_err == FALSE)
  {
    uimdrv_rx_state_machine(uim_ptr, uart_rx_buf_ptr, no_of_bytes_received);
    rx_event_cb_err = FALSE;
  }
  else
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"RX state machine not invoked - Error in rx sub state");
    rx_event_cb_err = TRUE;
  }
  return rx_event_cb_err;
} /* uimdrv_rx_event_cb */


/**
 * This function is called to process the UARTDM status for
 * any parity or overun errors. In the case of overrun errors
 * we log all the bytes that were received. In the case of
 * parity errors we update the rx_sub_state to be
 * UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS so we can handle
 * inverse convention cards.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param uart_status UARTDM status evaluated for parity
 *                            and overrun errors.
 */
void uimdrv_modify_rx_sub_state_for_overrun_parity_err(uim_instance_global_type *uim_ptr,
                                                       dword uart_status)
{
  switch (uart_status)
  {
    case MSMU_SR_PF_ERR:
      if (uim_ptr->rxtx_state_machine.rx_state == UIM_RX_RECEIVE_ATR)
      {
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS;
      }
      else if (uim_ptr->rxtx_state_machine.rx_state == UIM_RX_T_1_RECV_T_1_BLOCK_DM)
      {
        /* In the case of T=1 protocol we may receive a parity error in between the
           transaction and we will have to send an R block to the card */
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_T_1_RX_RECEIVED_WITH_PARITY_ERRORS;
      }
      else
      {
        /* NOTE - If we have received parity errors in any other state
           then in that case we would have enabled the parity error
           workaround in MR2 register by setting MSMU_MR2_RX_ERROR_CHAR_OFF.
           Hence it would not be loaded into the rx fifo.
           We would process it as a regular opertaion until the RXTX module
           finds the need to shutdown the interface. */
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"RX received with parity errors");

      }
      break;

    case MSMU_SR_OVR_ERR:
      uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_OVERRUN;
      break;

    default:
      break;
  }
}/* uimdrv_evaluate_for_overrun_and_parity_errors */


/**
 * This function is called to process the current Rx sub state
 * and progres the sub state based on the interrupt received
 * from the RXTX module that does the work of processing the
 * received interrupts from UARTDM.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param interrupt_processed Interrupt processed by UARTDM.
 * @param uart_status UARTDM status evaluated for parity and
 *                    overrun errors.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 * @return rx_sub_state_err Indicates if there was an error
 *         while processing the rx sub state.
 */
boolean uimdrv_rx_sub_state_machine
(
  uim_instance_global_type *uim_ptr,
  dword interrupt_processed,
  dword uart_status,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
)
{
  boolean rx_sub_state_err = FALSE;

  uimdrv_modify_rx_sub_state_for_overrun_parity_err(uim_ptr, uart_status);

  switch (uim_ptr->rxtx_state_machine.rx_sub_state)
  {
    case UIM_RX_WAIT_FOR_RXSTALE:
      if (interrupt_processed == MSMU_ISR_RXSTALE)
      {
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_RXSTALE;
      }
      else if ( interrupt_processed == MSMU_ISR_RXLEV)
      {
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_RXLEV;
      }
      break;

    case UIM_RX_WAIT_FOR_RXLEV:
      if (interrupt_processed == MSMU_ISR_RXLEV)
      {
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_RXLEV;
      }
      break;

    case UIM_RX_WAIT_FOR_TXREADY:
      if (interrupt_processed == MSMU_ISR_TX_READY)
      {
       uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_TXREADY;
      }
      break;

    case UIM_RX_WAIT_FOR_TXLEV:
      if (interrupt_processed == MSMU_ISR_TXLEV)
      {
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_TXLEV;
      }
      break;

    case UIM_RX_WAIT_FOR_TXCOMP:
      if (interrupt_processed == MSMU_ISR_TX_COMP)
      {
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_TXCOMP;
      }
      break;

    /* Special cases for parity and overrun errors */
    case UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS:
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"RX received with parity errors while waiting for ATR");
      break;

    case UIM_T_1_RX_RECEIVED_WITH_PARITY_ERRORS:
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"T=1: RX received with parity errors, to send R block");
      if (interrupt_processed == MSMU_ISR_RXLEV)
      {
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_T_1_RX_LEV_RECEIVED_WITH_PARITY_ERRORS;
      }
      break;

    case UIM_RX_OVERRUN:
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"RX state moved to overrun state to log RX bytes");
      uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_PROCESS_NONE;
      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_OVERRUN;
      break;

    default:
      UIMDRV_MSG_ERR_0(uim_ptr->id,"RX sub state invalid");
      rx_sub_state_err = TRUE;
      break;
  }
  return rx_sub_state_err;
} /* uimdrv_rx_sub_state_machine */


/**
 * This function is called to process the current Rx state and
 * progres the state based on the bytes received from the
 * UARTDM.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 */
void uimdrv_rx_state_machine(uim_instance_global_type *uim_ptr,
                             dword* uart_rx_buf_ptr,
                             uint32 no_of_bytes_received)
{
  /* Process the interrupt based on the current processing state */
  switch (uim_ptr->rxtx_state_machine.rx_state)
  {
    case UIM_RX_PROCESS_IDLE:
      {
        uim_rx_isr_process_idle(uim_ptr, uart_rx_buf_ptr, no_of_bytes_received);
      }
      break;

    case UIM_RX_RECEIVE_ATR:
      {
        uim_rx_isr_receive_atr(uim_ptr, uart_rx_buf_ptr, no_of_bytes_received);
      }
      break;

    case UIM_RX_RECEIVE_PPS:
      {
        uim_rx_isr_receive_pps(uim_ptr, uart_rx_buf_ptr, no_of_bytes_received);
      }
      break;

    case UIM_RX_OVERRUN:
      {
        uim_rx_isr_process_overrun(uim_ptr, uart_rx_buf_ptr, no_of_bytes_received);
      }
      break;

    case UIM_RX_PROC_PRCDR_BYTES:
      {
        uim_rx_isr_proc_procedure_bytes(uim_ptr, uart_rx_buf_ptr, no_of_bytes_received);
      }
      break;

#ifdef FEATURE_UIM_T_1_SUPPORT

    case UIM_RX_T_1_RECV_T_1_BLOCK_DM:
      {
        uim_rx_isr_recv_t_1_block(uim_ptr, uart_rx_buf_ptr, no_of_bytes_received);
      }
      break;

    case UIM_RX_T_1_SEND_PROLOGUE_ST:
      {
        uim_rx_isr_send_prologue(uim_ptr);
      }
      break;

    case UIM_RX_T_1_SEND_INFO_ST:
      {
        uim_rx_isr_send_information(uim_ptr);
      }
      break;

    case UIM_RX_T_1_SEND_EPILOGUE_ST:
      {
        uim_rx_isr_send_epilogue(uim_ptr);
      }
      break;
#endif /* FEATURE_UIM_T_1_SUPPORT */

    default:
      {
        uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

        if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                                     uim_ptr) == TRUE)
        {
          uim_log_put_last_byte(uim_ptr);
        }

        UIMDRV_MSG_HIGH_1(uim_ptr->id,"Invalid uim_rx_state %d",
                                   uim_ptr->rxtx_state_machine.rx_state);

        uim_clear_cmd_rsp_timer(uim_ptr);

        (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
      }

  } /* end switch(uim_ptr->card_cmd.rx_state) */
} /* uimdrv_rx_state_machine */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when no bytes are expected
 * from the UIM. If called, bytes were received from the UIM.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 */
void uim_rx_isr_process_idle
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
)
{
  uim_ptr->card_cmd.num_bytes_received = no_of_bytes_received;

  uim_ptr->debug.tot_idle_bytes_count = uim_ptr->debug.tot_idle_bytes_count +
                                        uim_ptr->card_cmd.num_bytes_received;
} /* uim_rx_isr_process_idle */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting an ATR from the
 * UIM. The ATR bytes are processed as they come in. If the ATR
 * is received without conflict, the UIM server is notified.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 */
void uim_rx_isr_receive_atr
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
)
{
  byte   rx_value_array[4];

  /* Boolean variable to indicate if a new RX initialization is required */
  boolean reinit_rxinit = TRUE;

  /* Boolean var indicating the need to stop processing pss bytes */
  boolean error_processing_atr_bytes = FALSE;

  /* Running index */
  uint16 i = 0;
  uint16 bytes_remaining = 0;
  uint16 uart_rx_buf_index = 0;

  /* Return, if the RX sub state is not RX_STALE, RX_LEV or ATR received with parity errors */
  if( !(uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXSTALE ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXLEV ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS) )
  {
    return;
  }

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_HANDLE_NO_ATR_IN_40000_CLK_CYCLES,
                               uim_ptr) == TRUE)
  {
    uim_ptr->atr.atr_byte_received = TRUE;
  }

  uim_ptr->card_cmd.num_bytes_received = no_of_bytes_received;

  bytes_remaining    = no_of_bytes_received;

  if(uim_ptr->card_cmd.num_bytes_received % 4)
  {
    uim_ptr->card_cmd.num_words_received = ((uim_ptr->card_cmd.num_bytes_received/4) + 1) ;
  }
  else
  {
    uim_ptr->card_cmd.num_words_received = uim_ptr->card_cmd.num_bytes_received/4 ;
  }

  /* Process all the bytes received in the uart_rx_buffer */

  while ( (uim_ptr->atr.num_bytes_to_skip != 0) &&
          (uim_ptr->card_cmd.num_words_received > 0) &&
          uart_rx_buf_index < uim_ptr->card_cmd.num_words_received )
  {
    rx_value_array[0] = (byte)(uart_rx_buf_ptr[uart_rx_buf_index]  & 0xFF);
    rx_value_array[1] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF00)     >> 8);
    rx_value_array[2] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF0000)   >> 16);
    rx_value_array[3] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF000000) >> 24);

    uart_rx_buf_index = uart_rx_buf_index + 1;

    for ( i = 0; i < 4 && bytes_remaining > 0; i++)
    {
      bytes_remaining--;

      if (uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS)
      {

         uim_atr_check_for_inverse_convention(uim_ptr, rx_value_array[i], &reinit_rxinit);
        /* Parity error...Do not process this byte. */
        continue;

      }

      if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                                   uim_ptr) == TRUE)
      {
        uim_log_put_byte(UIM_LOG_RX_DATA, rx_value_array[i], uim_ptr);
      }

      *uim_ptr->card_cmd.resp_buf_byte_ptr++ = rx_value_array[i];

      uim_ptr->card_cmd.num_resp_bytes_rcvd++;

      /* Another byte is processed.  Determine how many more bytes
         until this function needs to process a format byte. */
      uim_ptr->atr.num_bytes_to_skip--;

      /* Determine if this byte should be processed. */
      if (uim_ptr->atr.num_bytes_to_skip == 0)
      {
        /* Process the byte based on what is expected by this function */
       uim_rx_isr_atr_process_format_bytes(uim_ptr, uim_ptr->atr.receive_mode, rx_value_array[i], &reinit_rxinit, &error_processing_atr_bytes);

       if (error_processing_atr_bytes == TRUE)
       {
         UIMDRV_MSG_ERR_0(uim_ptr->id,"Invalid receive mode while processing ATR");

         uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

         uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

         uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PROBLEM;

         uim_command_response_callback(uim_ptr);

         return;
        }
      } /* end if - this function should process this byte */
    } /* end for */

    /* Make sure the maximum number of bytes are not exceeded.
       The maximum number of bytes are okay if the processing is complete */
    if ((uim_ptr->card_cmd.num_resp_bytes_rcvd >= MAX_NUM_ATR_BYTES) &&
        (uim_ptr->rxtx_state_machine.rx_state != UIM_RX_PROCESS_IDLE))
    {
      uim_ptr->atr.num_bytes_to_skip = 0;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PROBLEM;

      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

      uim_command_response_callback(uim_ptr);

      reinit_rxinit = FALSE;
    } /* end if - check for maximum number of bytes */
  } /* end while - process all the bytes in the receive FIFO. */

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                               uim_ptr) == TRUE)
  {
    uim_log_put_last_byte(uim_ptr);
  }

  /* Re-initialize a new RX data transfer if needed. */
  if(reinit_rxinit == TRUE)
  {
    reinit_rxinit = FALSE;

   /* This stale event might have happened while we were processing the
      RXLEV interrupt. In that case, we would like to preserve it. */
    if (uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXLEV)
    {
      uim_ptr->card_cmd.num_bytes_received = 0;
      uim_ptr->card_cmd.num_words_received = 0;
      uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;
      return;
    }

    /* Since we are going to Re-Init a new RX data transfer
       initialize variables to 0. */
    uim_ptr->card_cmd.num_bytes_received = 0;
    uim_ptr->card_cmd.num_words_received = 0;
    uim_ptr->card_cmd.total_bytes_read = 0;

    /* Re-initialize a new RX data transfer */
    UIM_DM_RX_INIT_SLOT(uim_ptr, MSMU_DEF_RX_INIT_VAL);

    UIM_ENABLE_RX_STALE_INT_SLOT(uim_ptr);

    uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;

    uim_start_cmd_rsp_timer( (uim_ptr->card_cmd.work_waiting_time +
                              UIM_UART_DM_WAITING_TIME_CORRECTION),
                             uim_ptr);
  }

  if(uim_ptr->rxtx_state_machine.rx_state == UIM_RX_PROCESS_IDLE)
  {
    /* Enable the parity error work-around only after the ATR is received */
    UIM_PRG_IPR_SLOT(uim_ptr, MSMU_IPR_DEF_STALE_TIMEOUT);

    if(uim_ptr->state.convention == UIM_DIRECT_CONVENTION)
    {
      UIM_PRG_MR2_SLOT(uim_ptr, (MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_EPAR | MSMU_MR2_RX_ERROR_CHAR_OFF | MSMU_MR2_RX_ERROR_ZERO_CHAR_OFF));
    }
    uim_reset_uart_slot(uim_ptr);

    UIM_ENABLE_UART_TX_SLOT (uim_ptr);

    UIM_ENABLE_UART_RX_SLOT (uim_ptr);
  }
} /* uim_rx_isr_receive_atr */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called to evaluate if we need to
 * switch to an inverse convention in software.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param rx_value Byte from the UART RX buffer with parity
 *                 error.
 * @param reinit_rxinit_ptr Pointer to the variable that is used
 *                          to reinit the rx transfer while
 *                          processing the ATR.
 */
void uim_atr_check_for_inverse_convention
(
  uim_instance_global_type *uim_ptr,
  byte rx_value,
  boolean* reinit_rxinit_ptr
)
{
  /* Check if this is the first byte */
  if (0 == uim_ptr->card_cmd.num_resp_bytes_rcvd)
  {
    /* Check if first byte indicates an inverse convention UIM card. */
    if (rx_value == INVERSE_CONVENTION_TS_DETECT)
    {
      UIM_PROGRAM_INVERSE_CONVENTION_SLOT(uim_ptr);

      UIM_RESET_RX_SLOT (uim_ptr);

      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CONVENTION_ERROR;

      uim_command_response_callback(uim_ptr);

      *reinit_rxinit_ptr = FALSE;
    }
    /* Check for an indication of a direct convention TS when in inverse
       mode */
    else if (rx_value == DIRECT_CONVENTION_TS_DETECT)
    {
      UIM_PROGRAM_DIRECT_CONVENTION_SLOT(uim_ptr);

      UIM_RESET_RX_SLOT (uim_ptr);

      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CONVENTION_ERROR;

      uim_command_response_callback(uim_ptr);

      *reinit_rxinit_ptr = FALSE;
    } /* end if - check for inverse convention byte. */

    /* Check if parity is received in ATR without any convention mismatch */
    else if( ( rx_value == UIM_ATR_TS_DIRECT_CONV &&
               uim_ptr->state.convention == UIM_DIRECT_CONVENTION
              ) ||
             ( rx_value == UIM_ATR_TS_INVERSE_CONV &&
               uim_ptr->state.convention == UIM_INVERSE_CONVENTION
             ) )
    {
      if(FALSE == uim_ptr->atr.is_parity_in_atr)
      {
        uim_ptr->atr.is_parity_in_atr = TRUE;

        uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CONVENTION_ERROR;

        uim_command_response_callback(uim_ptr);

        *reinit_rxinit_ptr = FALSE;
      }
    }
  } /* end if (0 == uim_ptr->card_cmd.num_resp_bytes_rcvd) */

} /* rx_isr_check_for_inverse_convention */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called while processing the format
 * bytes of the ATR.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param receive_mode Mode determing what byte we are waiting
 *                     on.
 * @param rx_value Byte being processed by the ATR.
 * @param reinit_rxinit_ptr Pointer to the variable that is used
 *                          to reinit the rx transfer while
 *                          processing the ATR.
 * @param error_processing_pps_bytes_ptr Pointer to the variable
 *                                       that decides if there
 *                                       was an error in the
 *                                       processing of ATR and
 *                                       the need to terminate
 *                                       the same.
 */
void uim_rx_isr_atr_process_format_bytes
(
  uim_instance_global_type *uim_ptr,
  uim_atr_pps_rx_mode_type receive_mode,
  byte rx_value,
  boolean* reinit_rxinit_ptr,
  boolean* error_processing_atr_bytes_ptr
)
{
  switch (receive_mode)
  {
    case UIM_WAITING_FOR_T0:    /* Expecting the T0 character */
      {
        *reinit_rxinit_ptr = TRUE;

        uim_ptr->atr.num_historical_chars = (rx_value & UIM_K_FIELD);

        /* Determine how many bytes follow this format byte. */
        uim_ptr->atr.num_bytes_to_skip = uim_y_format_table[rx_value >>
          UIM_Y_FIELD_SHIFT];

        if (rx_value & UIM_TD_PRESENT)
        {
          uim_ptr->atr.receive_mode = UIM_WAITING_FOR_TD;
        }
        else
        {
          /* The number of bytes to skip include the number of
             bytes that follow this format character and the
             number of historical characters. */
          uim_ptr->atr.num_bytes_to_skip +=
            uim_ptr->atr.num_historical_chars;

          /* There will NOT be a TCK.  Determine if any bytes
             follow in this ATR. */
          if (uim_ptr->atr.num_bytes_to_skip > 0)
          {
            /* Wait for the rest of the bytes of the ATR. */
            uim_ptr->atr.receive_mode = UIM_WAITING_FOR_END;
          }
          else /* There are no more bytes in the ATR. */
          {
            uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

            uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
              uim_ptr->card_cmd.num_resp_bytes_rcvd;

            uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

            uim_command_response_callback(uim_ptr);

            *reinit_rxinit_ptr = FALSE;
          } /* end if - more bytes to come. */
        } /* end if - TD byte is present in following bytes */
      } /* end case - UIM_WAITING_FOR_T0 */
      break;

    case UIM_WAITING_FOR_TD:    /* Expecting a TD character */
      {
        *reinit_rxinit_ptr = TRUE;

        /* Determine how many bytes follow this format byte. */
        uim_ptr->atr.num_bytes_to_skip =
          uim_y_format_table[rx_value >> UIM_Y_FIELD_SHIFT];

        /* Determine if there is a protocol other than 0. */
        if ((rx_value & UIM_T_FIELD) != 0)
        {
          /* This is indicates the TCK byte is expected */
          uim_ptr->atr.check_byte_expected = TRUE;
        } /* end if - a protocol other than 0 */

        /* Determine if there will NOT be another format byte. */
        if (!(rx_value & UIM_TD_PRESENT))
        {
          /* Determine if the check byte is expected */
          if (uim_ptr->atr.check_byte_expected)
          {
            /* The number of bytes to skip include the number of
               bytes that follow this format character and the
               number of historical characters.  The check byte
               is also included. */
            uim_ptr->atr.num_bytes_to_skip +=
              uim_ptr->atr.num_historical_chars + 1;

            /* Wait for the check byte. */
            uim_ptr->atr.receive_mode = UIM_WAITING_FOR_TCK;
          }
          else /* No check byte is expected */
          {
            /* The number of bytes to skip include the number of
               bytes that follow this format character and the
               number of historical characters. */
            uim_ptr->atr.num_bytes_to_skip +=
              uim_ptr->atr.num_historical_chars;

            /* There will NOT be a TCK.  Determine if any bytes
               follow in this ATR. */
            if (uim_ptr->atr.num_bytes_to_skip > 0)
            {
              /* Wait for the rest of the bytes of the ATR. */
              uim_ptr->atr.receive_mode = UIM_WAITING_FOR_END;
            }
            else /* There are no more bytes in the ATR. */
            {
              /* Give a good status. */
              uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

              /* Copy the number of bytes received. */
              uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
                uim_ptr->card_cmd.num_resp_bytes_rcvd;

              /* No more characters are expected at this time */
              uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

              /* Inform the UIM server that the ATR has been received */
              uim_command_response_callback(uim_ptr);

              *reinit_rxinit_ptr = FALSE;
            } /* end if - more bytes to come. */
          } /* end if - more bytes to come. */
        } /* end if - TD byte is present in following bytes */
      } /* end case - UIM_WAITING_FOR_TD */
      break;

    case UIM_WAITING_FOR_TCK:   /* Expecting the TCK character */
      {
        /* The ATR should have been completely received.  No other bytes
           are expected at this time                                     */
        uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
          uim_ptr->card_cmd.num_resp_bytes_rcvd;

        uim_command_response_callback(uim_ptr);

        *reinit_rxinit_ptr = FALSE;
      } /* end case - UIM_WAITING_FOR_TCK */
      break;

    case UIM_WAITING_FOR_END:   /* Expecting the last of the
                                    ATR with NO TCK character expected   */
      {
        /* The ATR should have been completely received.  No other bytes
           are expected at this time                                     */
        uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
          uim_ptr->card_cmd.num_resp_bytes_rcvd;

        uim_command_response_callback(uim_ptr);

        *reinit_rxinit_ptr = FALSE;
      } /* end case - UIM_WAITING_FOR_END */
      break;

    default:
      {
        *error_processing_atr_bytes_ptr = TRUE;
        *reinit_rxinit_ptr = FALSE;
      } /* end case - default */
      break;
  }/* end switch - receive_mode */
} /* uim_rx_isr_atr_process_format_bytes */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting to receive a
 * PPS response. The PPS response bytes are processed as they
 * come in. If the PPS response is received without conflict,
 * the UIM server is notified.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 */
void uim_rx_isr_receive_pps
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
)
{
  byte   rx_value_array[4];

  /* Boolean variable to indicate if a new RX initialization is required */
  boolean reinit_rxinit = TRUE;

  /* Boolean var indicating the need to stop processing pss bytes */
  boolean error_processing_pps_bytes = FALSE;

  /* Running index */
  uint16 i = 0;
  uint16 bytes_remaining = 0;
  uint16 uart_rx_buf_index = 0;


  /* Wait for TX_READY event to get confirmation that the last PPS byte was
     sent */

  if ( uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_TXCOMP )
  {
    uim_start_cmd_rsp_timer((uim_ptr->card_cmd.work_waiting_time
                            + UIM_UART_DM_WAITING_TIME_CORRECTION),
                            uim_ptr);
    uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;
    return;
  }

  /* Return, if the RX sub state is not RX_STALE or RX_LEV */
  if( !(uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXSTALE ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXLEV) )
  {
    return;
  }

  uim_ptr->card_cmd.num_bytes_received = no_of_bytes_received;

  bytes_remaining    = no_of_bytes_received;

  if(uim_ptr->card_cmd.num_bytes_received % 4)
  {
    uim_ptr->card_cmd.num_words_received = ((uim_ptr->card_cmd.num_bytes_received/4) + 1) ;
  }
  else
  {
    uim_ptr->card_cmd.num_words_received = uim_ptr->card_cmd.num_bytes_received/4 ;
  }

  /* Process all the bytes received in the uart_rx_buffer */

  while( (uim_ptr->pps.num_bytes_to_skip != 0) &&
         (uim_ptr->card_cmd.num_words_received > 0)&&
         uart_rx_buf_index < uim_ptr->card_cmd.num_words_received )
  {
    rx_value_array[0] = (byte)(uart_rx_buf_ptr[uart_rx_buf_index]  & 0xFF);
    rx_value_array[1] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF00)     >> 8);
    rx_value_array[2] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF0000)   >> 16);
    rx_value_array[3] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF000000) >> 24);

    uart_rx_buf_index = uart_rx_buf_index + 1;

    for ( i = 0; i < 4 && bytes_remaining > 0; i++)
    {
       bytes_remaining--;

      if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                                   uim_ptr) == TRUE)
      {
        uim_log_put_byte(UIM_LOG_RX_DATA, rx_value_array[i], uim_ptr);
      }

      *uim_ptr->card_cmd.resp_buf_byte_ptr++ = rx_value_array[i];

      /* Keep a rolling value of the check byte. */
      uim_ptr->pps.check_byte ^= rx_value_array[i];

      uim_ptr->card_cmd.num_resp_bytes_rcvd++;

      /* Another byte is processed.  Determine how many more bytes
         until this function needs to process a format byte. */
      uim_ptr->pps.num_bytes_to_skip--;

      /* Determine if this byte should be processed. */
      if (uim_ptr->pps.num_bytes_to_skip == 0)
      {
         /* Process the byte based on what is expected by this function */
        uim_rx_isr_pps_process_format_bytes (uim_ptr, uim_ptr->pps.receive_mode, rx_value_array[i], &reinit_rxinit, &error_processing_pps_bytes);

        if (error_processing_pps_bytes == TRUE)
        {
          UIMDRV_MSG_ERR_0(uim_ptr->id,"Error processing PPS");

          UIM_RESET_RX_SLOT (uim_ptr);

          uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

          uim_ptr->pps.num_bytes_to_skip = 0;

          uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

          uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PROBLEM;

          uim_command_response_callback(uim_ptr);

          return;
        }

      } /* end if - this function should process this byte */

      /* Make sure the maximum number of bytes are not exceeded. */
      if ((uim_ptr->card_cmd.num_resp_bytes_rcvd >= MAX_NUM_PPS_BYTES) &&
          (uim_ptr->rxtx_state_machine.rx_state != UIM_RX_PROCESS_IDLE))
      {
        uim_ptr->pps.num_bytes_to_skip = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PROBLEM;

        uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

        uim_command_response_callback(uim_ptr);

        reinit_rxinit = FALSE;
      } /* end if - check for maximum number of bytes */
    } /* for (i = 0; i < 4) */
  } /* end for() - process all the bytes in the receive FIFO. */

  /* Re-initialize a new RX data transfer if needed. */
  if(reinit_rxinit == TRUE)
  {
    reinit_rxinit = FALSE;

    /* This stale event might have happened while we were processing the
       RXLEV interrupt. In that case, we would like to preserve it. */
    if (uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXLEV)
    {
      uim_ptr->card_cmd.num_bytes_received = 0;
      uim_ptr->card_cmd.num_words_received = 0;
      uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;
      return;
    }

    /* Since we are going to re-init a new RX data transfer
       initialize this variable to 0. */
    uim_ptr->card_cmd.num_bytes_received = 0;
    uim_ptr->card_cmd.num_words_received = 0;
    uim_ptr->card_cmd.total_bytes_read = 0;

    /* Re-initialize a new RX data transfer */
    UIM_DM_RX_INIT_SLOT(uim_ptr, MSMU_DEF_RX_INIT_VAL);

    /* Enable Stale event */
    UIM_ENABLE_RX_STALE_INT_SLOT(uim_ptr);

    uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;

    uim_start_cmd_rsp_timer( (uim_ptr->card_cmd.work_waiting_time +
                              UIM_UART_DM_WAITING_TIME_CORRECTION),
                             uim_ptr);
  }

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                               uim_ptr) == TRUE)
  {
    uim_log_put_last_byte(uim_ptr);
  }
} /* uim_rx_isr_receive_pps */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called while processing the format
 * bytes for pps response.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param receive_mode Mode determing what byte we are waiting
 *                     on.
 * @param rx_value Byte being processed by the PPS.
 * @param reinit_rxinit_ptr Pointer to the variable that is used
 *                          to reinit the rx transfer while
 *                          processing the pps response.
 * @param error_processing_pps_bytes_ptr Pointer to the variable
 *                                       that decides if there
 *                                       was an error in the
 *                                       processing of pps and
 *                                       the need to terminate
 *                                       the same.
 */
void uim_rx_isr_pps_process_format_bytes
(
  uim_instance_global_type *uim_ptr,
  uim_atr_pps_rx_mode_type receive_mode,
  byte rx_value,
  boolean* reinit_rxinit_ptr,
  boolean* error_processing_pps_bytes_ptr
)
{
  switch (receive_mode)
  {
    case UIM_WAITING_FOR_T0:    /* Expecting the T0 character */
      {
        /* Determine how many bytes follow this format byte. */
        uim_ptr->pps.num_bytes_to_skip =
                  uim_y_format_table[rx_value >> UIM_Y_FIELD_SHIFT] + 1;

        /* Wait for the check character. */
        uim_ptr->pps.receive_mode = UIM_WAITING_FOR_TCK;
        *reinit_rxinit_ptr = TRUE;
      } /* end case - UIM_WAITING_FOR_T0 */
      break;

    case UIM_WAITING_FOR_TCK:   /* Expecting the TCK character */
      {
        *reinit_rxinit_ptr = TRUE;
        /* The ATR should have been completely received.  No other bytes
           are expected at this time */
        uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

        /* Determine if the PPS was received without error */
        if (uim_ptr->pps.check_byte == 0)
        {
          uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

          uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
            uim_ptr->card_cmd.num_resp_bytes_rcvd;

          uim_command_response_callback(uim_ptr);

          *reinit_rxinit_ptr = FALSE;
        } /* end if - ATR was received without error */
        else
        {
          *error_processing_pps_bytes_ptr = TRUE;
          *reinit_rxinit_ptr = FALSE;
        }
      } /* end case - UIM_WAITING_FOR_TCK */
      break;

    case UIM_WAITING_FOR_TD:    /* Expecting a TD character */
    case UIM_WAITING_FOR_END:   /* Expecting the last of the
                               ATR with NO TCK character expected */
    default:
      {
        *error_processing_pps_bytes_ptr = TRUE;
        *reinit_rxinit_ptr = FALSE;
      } /* end case - default */
      break;
  } /* end switch - receive_mode */
} /* uim_rx_isr_pps_process_format_bytes */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when UART is overrun when the
 * driver state is not idle. This function reads the bytes from
 * the UART and flushes them.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 */
void uim_rx_isr_process_overrun
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
)
{

  /* Running index */
  uint16 i = 0;
  uint16 uart_rx_buf_index = 0;

  uim_ptr->card_cmd.num_bytes_received = no_of_bytes_received;

  if(uim_ptr->card_cmd.num_bytes_received % 4)
  {
    uim_ptr->card_cmd.num_words_received = ((uim_ptr->card_cmd.num_bytes_received/4) + 1) ;
  }
  else
  {
    uim_ptr->card_cmd.num_words_received = uim_ptr->card_cmd.num_bytes_received/4 ;
  }

  /* Log all the bytes received in the uart_rx_buffer */

  while ( uart_rx_buf_index < uim_ptr->card_cmd.num_words_received )
  {
    for (i = 0;i < 4; i++)
    {
      if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                                   uim_ptr) == TRUE)
      {
        uim_log_put_byte(UIM_LOG_RX_DATA, (byte)( (uart_rx_buf_ptr[uart_rx_buf_index] >> (i * 8)) & 0xFF), uim_ptr );
      }
    }
    uart_rx_buf_index = uart_rx_buf_index + 1;
  }

  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;
  uim_ptr->card_cmd.num_bytes_received = 0;
  uim_ptr->card_cmd.num_words_received = 0;
  uim_ptr->card_cmd.total_bytes_read = 0;
} /* uim_rx_isr_process_overrun */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when we would like the process
 * the received SW2 byte based on the received SW1 byte.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param sw1 Status Word 1.
 * @param sw2 Status Word 2.
 */
void uim_rx_isr_process_sw2_byte
(
  uim_instance_global_type *uim_ptr,
  byte sw1,
  byte sw2
)
{
  /* Determine what the first status word and process accordingly */
  switch (sw1)
  {
    case SW1_NORMAL_END:
      {
        uim_rx_isr_sw1_normal_end(sw2, uim_ptr);
      }
      break;

    case SW1_END_FETCH:
      {
        uim_rx_isr_sw1_fetch_end(sw2, uim_ptr);
      }
      break;

    case SW1_DLOAD_ERR:
      {
        uim_rx_isr_sw1_dload_err(sw2, uim_ptr);
      }
      break;

    case SW1_END_RESP:
      {
        /*  For STREAMING_APDU when SW1 = 0x9F is sent for non-ICC card
            we should just return the response and data if any
            available. */
        if (uim_processing_stream_apdu(uim_ptr) &&
            uim_ptr->debug.last_sent_apdu.uim_class != UIM_ICC_INSTRUCTION_CLASS)
        {
          uim_rx_isr_sw1_nfc(sw2, uim_ptr);
        }
        else
        {
          uim_ptr->card_cmd.get_resp_sw1 = SW1_END_RESP;
          uim_rx_isr_sw1_resp_end(sw2, uim_ptr);
        }
      }
      break;

    case SW1_BUSY:
      {
        uim_rx_isr_sw1_busy(sw2, uim_ptr);
      }
      break;

    case SW1_END_RETRY:
      {
        uim_rx_isr_sw1_retries(sw2, uim_ptr);
      }
      break;

    case SW1_REFERENCE:
      {
        uim_rx_isr_sw1_reference(sw2, uim_ptr);
      }
      break;

    case SW1_SECURITY:
      {
        uim_rx_isr_sw1_security(sw2, uim_ptr);
      }
      break;

    case SW1_P3_BAD:
      {
        uim_rx_isr_sw1_p3_bad(sw2, uim_ptr);
      }
      break;

    case SW1_P1_P2_BAD:
      {
        uim_rx_isr_sw1_p1_p2_bad(sw2, uim_ptr);
      }
      break;

    case SW1_INS_BAD:
      {
        uim_rx_isr_sw1_ins_bad(sw2, uim_ptr);
      }
      break;

    case SW1_CLA_BAD:
      {
        /* Save the status word in case it is a true status word response */
        uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

        if(uim_processing_stream_apdu(uim_ptr))
        {
          /* FOR NFC streaming APDUs pack the response data if any available
             and SWs regardless of SW2 value. Also set the APDU transaction
             result to UIM_DONE
          */
          uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
            uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

          uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
        }
        else
        {
          uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

          uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_WRONG_CLASS;
        }

        /* This flag is processed in the Normal end SW processing.
           It distinguishes between data download error and all other
           results. */
        uim_ptr->flag.processing_download_error = FALSE;

        uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

        uim_command_response_callback(uim_ptr);

      } /* end case - SW1_CLA_BAD */
      break;

    case SW1_PROBLEM:
      {
        uim_rx_isr_sw1_problem(sw2, uim_ptr);
      }
      break;

    case SW1_USIM_END_RESP:
      {
        if(uim_ptr->card_cmd.get_resp_sw1 == SW1_NO_STATUS)
        {
          uim_ptr->card_cmd.get_resp_sw1 = SW1_USIM_END_RESP;
        }
        uim_rx_isr_sw1_resp_end (sw2, uim_ptr);
      }
      break;

    case SW1_USIM_RESEND_APDU:
      {
        uim_rx_isr_sw1_resend_apdu (sw2, uim_ptr);
      }
      break;

    case SW1_WARNINGS1:
      {
          /* Update the flag to indicate that WARNINGS2 status
             is returned by the SIM */
          uim_ptr->card_cmd.warning1_indicator = TRUE;
          uim_ptr->rxtx_state_machine.rx_state = UIM_RX_SW1_WARNINGS1;

          if (uim_is_get_response_allowed(uim_ptr, SW1_WARNINGS1, sw2))
          {
            if (uim_processing_stream_apdu(uim_ptr))
            {
              /*
                For  NFC application we only do get response
                for SW combinations in the range 62 01 - 62 80
              */
              if(sw2 == SW2_NV_STATE_UNCHANGED || sw2 >= SW2_RET_DATA_MAY_BE_CORRUPTED)
              {
                uim_rx_isr_sw1_warnings1 ( sw2, uim_ptr );
              }
              else
              {
                uim_ptr->flag.sw1_warnings1_normal_end_occured = TRUE;

                uim_ptr->flag.force_get_response = TRUE;
                uim_ptr->card_cmd.get_resp_sw1   = SW1_WARNINGS1;
                uim_rx_isr_sw1_resp_end (sw2, uim_ptr);
                uim_ptr->flag.force_get_response = FALSE;
              }
            }
            else
            {
              uim_ptr->flag.force_get_response = TRUE;
              uim_ptr->card_cmd.get_resp_sw1 = SW1_WARNINGS1;
              uim_rx_isr_sw1_resp_end (sw2, uim_ptr);
              uim_ptr->flag.force_get_response = FALSE;
            }
          }
          /* Always allow Get Response for GetStatus Request */
          else if ((uim_ptr->state.last_instrn_case == UIM_INSTRN_CASE_2) &&
                   (uim_processing_stream_apdu(uim_ptr)) &&
                   (uim_ptr->debug.last_sent_apdu.instrn  == STATUS))
          {
            /*
              For  NFC application we only do get response
              for SW combinations in the range 62 01 - 62 80
            */
            if(sw2 == SW2_NV_STATE_UNCHANGED || sw2 >= SW2_RET_DATA_MAY_BE_CORRUPTED)
            {
              uim_rx_isr_sw1_warnings1 ( sw2, uim_ptr );
            }
            else
            {
              uim_ptr->flag.sw1_warnings1_normal_end_occured = TRUE;
              uim_ptr->flag.force_get_response = TRUE;
              uim_ptr->card_cmd.get_resp_sw1 = SW1_WARNINGS1;
              uim_rx_isr_sw1_resp_end (sw2, uim_ptr);
              uim_ptr->flag.force_get_response = FALSE;
            }
          }
          else
          {
            uim_rx_isr_sw1_warnings1 (sw2, uim_ptr);
          }
      }
      break;

    case SW1_WARNINGS2:
      {
          /* Update the flag to indicate that WARNINGS2 status
             is returned by the SIM */
          uim_ptr->card_cmd.warning2_indicator = TRUE;
          uim_ptr->rxtx_state_machine.rx_state = UIM_RX_SW1_WARNINGS2;
          /* Let us consider this as a normal response
           * but only with p3 = 0 for non GSM case-4 other than
           * GENERATE_ASY_KEY_PAIR WIM instruction
           */
          if (uim_is_get_response_allowed(uim_ptr, SW1_WARNINGS2, sw2))
          {
            uim_ptr->flag.force_get_response = TRUE;
            uim_ptr->card_cmd.get_resp_sw1 = SW1_WARNINGS2;
            uim_rx_isr_sw1_resp_end ( sw2, uim_ptr);
            uim_ptr->flag.force_get_response = FALSE;
          }
          /* Always allow Get Response for GetStatus Request */
          else if ((uim_ptr->state.last_instrn_case == UIM_INSTRN_CASE_2) &&
                   (uim_processing_stream_apdu(uim_ptr)) &&
                   (uim_ptr->debug.last_sent_apdu.instrn  == STATUS))
          {
            uim_ptr->flag.force_get_response = TRUE;
            uim_ptr->card_cmd.get_resp_sw1 = SW1_WARNINGS2;
            uim_rx_isr_sw1_resp_end (sw2, uim_ptr);
            uim_ptr->flag.force_get_response = FALSE;
          }
          else
          {
            uim_rx_isr_sw1_warnings2 (sw2, uim_ptr );
          }
      }
      break;

    case SW1_EXECUTION_ERR2:
      {
        uim_rx_isr_sw1_execn_err2 (sw2, uim_ptr);
      }
      break;

    case SW1_CMD_NOT_ALLOWED:
      {
        uim_rx_isr_sw1_cmd_not_allowed (sw2, uim_ptr);
      }
      break;

    case SW1_WRONG_PARAMS:
      {
        uim_rx_isr_sw1_wrong_params (sw2, uim_ptr);
      }
      break;

    case SW1_SE_FAIL:
      {
        uim_rx_isr_sw1_se_fail (sw2, uim_ptr);
      }
      break;

    case SW1_EXECUTION_ERR1:
      {
        uim_rx_isr_sw1_execn_err1 (sw2, uim_ptr);
      }
      break;

    case SW1_CLA_NOT_SUPPORTED:
      {
        uim_rx_isr_sw1_cla_not_supported (sw2, uim_ptr);
      }
      break;

    case SW1_95_NFC:
    case SW1_96_NFC:
    case SW1_97_NFC:
    case SW1_99_NFC:
    case SW1_9A_NFC:
    case SW1_9B_NFC:
    case SW1_9C_NFC:
    case SW1_9D_NFC:
       {
         uim_rx_isr_sw1_nfc(sw2, uim_ptr);
       }
       break;

    default:
      uim_ptr->flag.sw1_byte_rcvd = FALSE;
      uim_ptr->flag.sw2_byte_rcvd = FALSE;
      UIMDRV_MSG_ERR_0(uim_ptr->id,"Invalid SW1 status byte");
      break;
  }
} /* rx_isr_process_sw2_byte */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words or
 * response bytes from the UIM after the command header is sent
 * out.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 */
void uim_rx_isr_proc_procedure_bytes
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
)
{
  byte sw1 = 0;     /* Holds the first status word */
  byte sw2 = 0;     /* Holds the second status word */
  byte result;      /* This holds the result of the ACK byte xored with the
                       INS byte */

  byte rx_value_array[4];

  /* Boolean variable to indicate if a new RX initialization is required */
  boolean reinit_rxtransfer = TRUE;

  /* Boolean var indicating the need to stop processing procedure bytes */
  boolean error_processing_proc_bytes = FALSE;

  /* Running index */
  uint16 i = 0;
  uint16 bytes_remaining = 0;
  uint16 uart_rx_buf_index = 0;

  if ( uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_TXCOMP )
  {
    uim_start_cmd_rsp_timer((uim_ptr->card_cmd.work_waiting_time +
                             UIM_UART_DM_WAITING_TIME_CORRECTION),
                            uim_ptr);
    uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;
    return;
  }

  /* Return, if the RX sub state is not RX_STALE or RX_LEV */
  if( !(uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXSTALE ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXLEV) )
  {
    return;
  }

  uim_ptr->card_cmd.num_bytes_received = no_of_bytes_received;

  bytes_remaining    = no_of_bytes_received;

  if(uim_ptr->card_cmd.num_bytes_received % 4)
  {
    uim_ptr->card_cmd.num_words_received = ((uim_ptr->card_cmd.num_bytes_received/4) + 1) ;
  }
  else
  {
    uim_ptr->card_cmd.num_words_received = uim_ptr->card_cmd.num_bytes_received/4 ;
  }

  /* Process all the bytes received in the uart_rx_buffer */

  while( (uim_ptr->card_cmd.num_words_received > 0) &&
         (uart_rx_buf_index < uim_ptr->card_cmd.num_words_received) )
  {
    rx_value_array[0] = (byte)(uart_rx_buf_ptr[uart_rx_buf_index]  & 0xFF);
    rx_value_array[1] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF00)     >> 8);
    rx_value_array[2] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF0000)   >> 16);
    rx_value_array[3] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF000000) >> 24);

    uart_rx_buf_index = uart_rx_buf_index + 1;

    for ( i = 0; i < 4 && bytes_remaining > 0; i++)
    {
        bytes_remaining--;

        if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                                     uim_ptr) == TRUE)
        {
          uim_log_put_byte(UIM_LOG_RX_DATA, rx_value_array[i], uim_ptr);
        }

        /* The GCF 27.11.1.5 testcase generates bytes with parity
           errors in a pattern. Every 1st, 4th, 7th, 10th, 13th and so on,
           bytes are received with parity error. Therefore, the workaround for
           just GCF 27.11.1.5 testcase basically ignores the bytes with parity
           error as the UART_DM captures them in the RX FIFO. */
        if (TRUE == uim_ptr->setting.par_err_workaround)
        {
          /* Ignore 1st, 4th, 7th,..., bytes following the pattern */
          if(uim_ptr->card_cmd.gcf_byte_count % 3 == 0)
          {
            /* Increment the byte count */
            uim_ptr->card_cmd.gcf_byte_count +=1;
            /* Ignore the byte */
            continue;
          }

          /* Increment the byte count */
          uim_ptr->card_cmd.gcf_byte_count +=1;
        }

        /* The data that is being received is the expected data from the SIM */
        if(uim_ptr->flag.resp_bytes_being_rcvd == TRUE)
        {
          *uim_ptr->card_cmd.resp_buf_byte_ptr++ = rx_value_array[i];
          uim_ptr->card_cmd.num_resp_bytes_rcvd_total++;

          uim_ptr->card_cmd.num_resp_bytes_rcvd++;

          reinit_rxtransfer = TRUE;

          /* Determine if all the expected bytes have been received. */
          if (uim_ptr->card_cmd.num_resp_bytes_rcvd ==
              uim_ptr->card_cmd.num_resp_data_bytes_expected)
          {
            uim_ptr->flag.resp_bytes_being_rcvd = FALSE;
          }
          continue;
        }

        /* Check if we have received the SW1 status byte */
        if(uim_ptr->flag.sw1_byte_rcvd == FALSE)
        {
          if (uim_ptr->rxtx_state_machine.tx_state == UIM_TX_SEND_CMD_HDR)
          {
            uim_ptr->card_cmd.ins_byte = *(uim_ptr->card_cmd.request_buffer_ptr + 1);

            /* Set up the data to send just in case there is data to send. */
            uim_ptr->card_cmd.num_bytes_to_send =
                      uim_ptr->card_cmd.num_cmd_data_bytes_to_send;
            uim_ptr->card_cmd.request_buffer_ptr =
                      uim_ptr->card_cmd.apdu_command_data_ptr;

            uim_ptr->rxtx_state_machine.tx_state = UIM_TX_SND_CMD_DATA;
          }

          /* Save the status word in case it is a true status word response */
          uim_ptr->card_cmd.resp_buf_ptr->sw1 = rx_value_array[i];
          sw1 = uim_ptr->card_cmd.resp_buf_ptr->sw1;

          if(sw1 == SW1_NULL)
          {
            uim_ptr->card_cmd.infinite_null_count++;
          }

          /* Determine what the first status word and process accordingly */
          switch (sw1)
          {
            case SW1_NULL:          /* NULL procedure byte */
              /* This byte is sent by the UIM to keep the ME from the work waiting
                 time timeout. */
              /* NULL procedure byte is not a status byte */
              uim_ptr->flag.sw1_byte_rcvd = FALSE;
              /* Set the flag to re-initialize a new RX transfer if this is the last
              byte received in the current RX transfer */
              reinit_rxtransfer = TRUE;
              break;

            case SW1_NORMAL_END:
            case SW1_END_FETCH:
            case SW1_DLOAD_ERR:
            case SW1_END_RESP:
            case SW1_BUSY:
            case SW1_END_RETRY:
            case SW1_REFERENCE:
            case SW1_SECURITY:
            case SW1_P3_BAD:
            case SW1_P1_P2_BAD:
            case SW1_INS_BAD:
            case SW1_CLA_BAD:
            case SW1_PROBLEM:
            case SW1_USIM_END_RESP:
            case SW1_USIM_RESEND_APDU:
            case SW1_WARNINGS1:
            case SW1_WARNINGS2:
            case SW1_EXECUTION_ERR2:
            case SW1_CMD_NOT_ALLOWED:
            case SW1_WRONG_PARAMS:
            case SW1_SE_FAIL:
            case SW1_EXECUTION_ERR1:
            case SW1_CLA_NOT_SUPPORTED:
              {
                uim_ptr->flag.sw1_byte_rcvd = TRUE;
                /* set the flag to re-initialize a new RX transfer if this is the last
                byte received in the current RX transfer */
                reinit_rxtransfer = TRUE;
              }
            break;

            default:
              {
                /*
                  For streaming APDUs we want to handle SW1 = 0x95 to 0x97 and
                  SW1 = 0x99 to 0x9D

                */
                if ((uim_processing_stream_apdu(uim_ptr))
                              &&
                    ((sw1 == SW1_95_NFC || sw1 == SW1_96_NFC ||
                      sw1 == SW1_97_NFC || sw1 == SW1_99_NFC ||
                      sw1 == SW1_9A_NFC || sw1 == SW1_9B_NFC ||
                      sw1 == SW1_9C_NFC || sw1 == SW1_9D_NFC)))
                {
                  uim_ptr->flag.sw1_byte_rcvd = TRUE;
                  /* set the flag to re-initialize a new RX transfer if this is the last
                  byte received in the current RX transfer */
                  reinit_rxtransfer = TRUE;
                  break;
                }
                /* This may be an ACK byte */
                /* The next operation is dependent on the value of the Ack byte
                   with respect to the INS of the APDU.  The action is determined
                   by XORing the Ack byte with the INS                    */
                result = sw1 ^ uim_ptr->card_cmd.ins_byte;

                uim_rx_isr_evaluate_procedure_bytes(uim_ptr, result, &reinit_rxtransfer, &error_processing_proc_bytes);

                if (error_processing_proc_bytes == TRUE)
                {
                  uim_ptr->flag.bad_status_words_error = TRUE;

                  uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

                  uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PROBLEM;

                  uim_reset_receiver(uim_ptr);

                  UIM_PRG_IMR_SLOT(uim_ptr, MSMU_IMR_MASK_ALL );

                  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

                  uim_command_response_callback(uim_ptr);

                  return;
                }
              } /* end case - default */
              break;
          } /* end switch - the first status word */
        }
        else
        {
          /* No more reinit required as we got the SW2 status byte indicating
          the response from the SIM is completed */
          reinit_rxtransfer = FALSE;

          /* We got the complete response.
             Stop processing any more data if padded by the UART_DM */
          bytes_remaining = 0;

          if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                                       uim_ptr) == TRUE)
          {
            uim_log_put_last_byte(uim_ptr);
          }

          UIM_PRG_IMR_SLOT(uim_ptr, MSMU_IMR_MASK_ALL );

          uim_ptr->flag.sw2_byte_rcvd = TRUE;

          sw2 = rx_value_array[i];

          uim_rx_isr_process_sw2_byte(uim_ptr, uim_ptr->card_cmd.resp_buf_ptr->sw1, sw2);
        }
      } /* for loop */
  } /* while loop */

  /* If end of data event occurred and still some more data expected */
  if( reinit_rxtransfer == TRUE )
  {
      reinit_rxtransfer = FALSE;

      /* This stale event might have happened while we were processing the
         RXLEV interrupt. In that case, we would like to preserve it. */
      if (uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXLEV)
      {
        uim_ptr->card_cmd.num_bytes_received = 0;
        uim_ptr->card_cmd.num_words_received = 0;
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;
        return;
      }

      /* Since we are going to re-init a new RX data transfer
         initialize this variable to 0. */
      uim_ptr->card_cmd.num_bytes_received = 0;
      uim_ptr->card_cmd.num_words_received = 0;
      uim_ptr->card_cmd.total_bytes_read = 0;

      /* Re-initialize a new RX data transfer */
      UIM_DM_RX_INIT_SLOT(uim_ptr, MSMU_DEF_RX_INIT_VAL);

      /* Enable Stale event */
      UIM_ENABLE_RX_STALE_INT_SLOT(uim_ptr);

      /* NOTE - Only in the case of procedure bytes dm we could be
         reinitizing the uartdm to recieve further bytes while
         1. We are waiting on further bytes.
         2. We have received the ack  byte and we have to send
            some bytes after which we are expecting further bytes
            the card.
         Hence if we are here because we are expecting further bytes
         from the card we set the sub state to UIM_RX_WAIT_FOR_RXSTALE.
         Else we will wait on TX_COMP or TX_RDY after which we switch the
         rx_sub_state to wait for UIM_RX_WAIT_FOR_RXSTALE.
       */
      if(uim_ptr->rxtx_state_machine.rx_sub_state != UIM_RX_WAIT_FOR_TXREADY ||
         uim_ptr->rxtx_state_machine.rx_sub_state != UIM_RX_WAIT_FOR_TXCOMP )
      {
        uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;
      }

      uim_start_cmd_rsp_timer( (uim_ptr->card_cmd.work_waiting_time
                                + UIM_UART_DM_WAITING_TIME_CORRECTION),
                               uim_ptr);
  }

  return;
} /* uim_rx_isr_proc_procedure_bytes */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function evaluates each of the received bytes
 * while processing the procedure bytes to determine if the ACK
 * byte is received and accordingly decide if we expecting bytes
 * from the card or need to send out bytes to the card based on
 * the APDU INST case.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param result Result of xoring the received procedure bytes
 *               with the INST case.
 * @param reinit_rxtransfer_ptr Pointer to the variable that
 *                              decides if we need to reinit a
 *                              rx transfer in
 *                              rx_isr_proc_procedure_bytes_dm.
 * @param error_processing_proc_bytes_ptr Pointer to the
 *                                        variable that decides
 *                                        if we need to stop
 *                                        processing the
 *                                        procedure bytes in
 *                                        rx_isr_proc_procedure_bytes_dm
 *                                        due to an error.
 */
void uim_rx_isr_evaluate_procedure_bytes
(
  uim_instance_global_type *uim_ptr,
  byte result,
  boolean* reinit_rxtransfer_ptr,
  boolean* error_processing_proc_bytes_ptr
)
{
  /* Determine what to do next */
  switch (result)

  {
    case PAUSE_REMAINING_BYTES:   /* Send the remaining bytes of the
                                     data                            */
    case PROG_REMAINING_BYTES:    /* Send the remaining bytes of the
                                     data                            */
      {
        /* Determine if command data bytes are to be sent */
        if ( (uim_ptr->card_cmd.num_cmd_data_bytes_to_send > 0) &&
             (uim_ptr->card_cmd.num_bytes_to_send  > 0) )
        {
          if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                                       uim_ptr) == TRUE)
          {
            uim_log_put_last_byte(uim_ptr);
          }

          uim_ptr->rxtx_state_machine.tx_state = UIM_TX_SND_CMD_DATA;

          uim_ptr->flag.send_all_remaining_bytes = TRUE;

          /* Set the flag to re-initialize a new RX transfer if this is the last
          byte received in the current RX transfer */
          *reinit_rxtransfer_ptr = TRUE;

          /* Wait long enough before responding to procedure byte */
          uim_clk_busy_wait( uim_ptr->setting.response_byte_delay );

          if(uim_ptr->card_cmd.num_resp_data_bytes_expected == 0)
          {
            /* Set the rx state to wait for procedure bytes */
            /* We are entering in to UIM_RX_PROC_PRCDR_BYTES since we are expecting only
               status words */
            uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROC_PRCDR_BYTES;

            *reinit_rxtransfer_ptr = TRUE;

            /* Intialize the byte count */
            if (uim_ptr->setting.par_err_workaround == TRUE)
            {
              uim_ptr->card_cmd.gcf_byte_count = 0;
            }

            UIM_PRG_IMR_SLOT(uim_ptr, ( MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK | \
                            MSMU_IMR_TX_COMP | MSMU_IMR_WWT) );

            /* Wait for TXCOMP event to make sure the bytes have been
               seen successfully */
            uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_TXCOMP;

            UIM_NUM_CHARS_TO_SEND_SLOT(uim_ptr,uim_ptr->card_cmd.num_bytes_to_send);

            uim_ptr->card_cmd.num_cmd_data_bytes_to_send =
                   uim_ptr->card_cmd.num_cmd_data_bytes_to_send - uim_ptr->card_cmd.num_bytes_to_send;

            /* Clear the work waiting timer when transmitting bytes */
            uim_clear_cmd_rsp_timer(uim_ptr);

            uim_tx(uim_ptr->card_cmd.request_buffer_ptr,
                              uim_ptr->card_cmd.num_bytes_to_send,
                              uim_ptr);

            uim_start_cmd_rsp_timer( (uim_ptr->card_cmd.work_waiting_time
                                      + UIM_UART_DM_WAITING_TIME_CORRECTION),
                                     uim_ptr);

            uim_ptr->card_cmd.num_bytes_to_send = 0;
            uim_ptr->card_cmd.num_cmd_data_bytes_to_send = 0;

          }
          else
          {
            // This condition is not possible
          }
        }
        /* Determine if a response is expected */
        else if (uim_ptr->card_cmd.num_resp_data_bytes_expected != 0)
        {
          uim_ptr->flag.resp_bytes_being_rcvd = TRUE;
          *reinit_rxtransfer_ptr = TRUE;
        }
        else /* Neither command data to send nor response expected
                AND we are processing an ACK.  Something went wrong  */
        {
          *error_processing_proc_bytes_ptr = TRUE;
          return;
        } /* end if - there are command bytes to send */
      } /* end case - REMAINING_BYTES */
      break;

    case PAUSE_NEXT_BYTE:         /* Send only the next byte and wait
                                     for Ack                         */
    case PROG_NEXT_BYTE:          /* Send only the next byte and wait
                                     for Ack                         */
      {
        /* Determine if command data bytes are to be sent */
        if ( (uim_ptr->card_cmd.num_cmd_data_bytes_to_send > 0) &&
             (uim_ptr->card_cmd.num_bytes_to_send  > 0) )
        {
          if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                                       uim_ptr) == TRUE)
          {
            uim_log_put_last_byte(uim_ptr);
          }

          uim_ptr->rxtx_state_machine.tx_state = UIM_TX_SND_CMD_DATA;

          /* Indicate only send one byte at a time */
          uim_ptr->flag.send_all_remaining_bytes = FALSE;

          *reinit_rxtransfer_ptr = TRUE;

          uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROC_PRCDR_BYTES;

          /* Initialize the byte count */
          if (uim_ptr->setting.par_err_workaround == TRUE)
          {
            uim_ptr->card_cmd.gcf_byte_count = 0;
          }

          UIM_PRG_IMR_SLOT(uim_ptr, ( MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK | \
                           MSMU_IMR_TX_COMP | MSMU_IMR_WWT) );

          /* Wait for TXCOMP event to make sure the bytes have been
             seen successfully */
          uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_TXCOMP;

          UIM_NUM_CHARS_TO_SEND_SLOT(uim_ptr, 1);

          /* Clear the work waiting timer when transmitting bytes */
          uim_clear_cmd_rsp_timer(uim_ptr);

          uim_tx(uim_ptr->card_cmd.request_buffer_ptr,
                            1,
                            uim_ptr);

          --uim_ptr->card_cmd.num_bytes_to_send;

          /* Send the next byte */
          uim_ptr->card_cmd.request_buffer_ptr++;

        }
        /* Determine if a response is expected */
        else if (uim_ptr->card_cmd.num_resp_data_bytes_expected != 0)
        {
          // do nothing
        }
        else /* Neither command data to send nor response expected
                AND we are processing an ACK.  Something went wrong  */
        {
          *error_processing_proc_bytes_ptr = TRUE;
          return;
        } /* end if - there are command bytes to send */
      } /* end case - NEXT_BYTE */
      break;

    default:
      {
        *error_processing_proc_bytes_ptr = TRUE;
        return;
      } /* end case - default */
  }  /* end switch - result of Ack */
} /* rx_isr_evaluate_procedure_bytes_dm */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when we receive an unknown
 * procedure byte. We either indicate error and call the command
 * response callback or we fake a timeout to retry the command.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_process_unknown_proc_byte(uim_instance_global_type *uim_ptr)
{
  UIMDRV_MSG_HIGH_0(uim_ptr->id,"Process Unknown procedure byte");

  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

  uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

  uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PROBLEM;

  uim_reset_receiver(uim_ptr);

  uim_clear_cmd_rsp_timer(uim_ptr);

  if(uim_nv_is_feature_enabled(
     UIMDRV_FEATURE_HANDLE_UNKNOWN_PROC_BYTES_AS_CMD_TIMEOUT,
     uim_ptr) == TRUE)
  {
     uim_ptr->flag.bad_status_words_error = FALSE;

    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
  }
  else
  {
    uim_ptr->flag.bad_status_words_error = TRUE;
    uim_command_response_callback(uim_ptr);
  }
} /* uim_rx_isr_process_unknown_proc_byte */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting the second
 * status word after a normal end status word.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_normal_end
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  /* Make sure the second status word is ok */
  if (sw2 == (int)SW2_NORMAL_END)
  {
    /* Place the number of bytes read in the response buffer */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

    /* Check if we are processing an ENVELOPE APDU:
       If TRUE and warnings were received earlier, set the
       appropriate SW1 WARNING status. */

      if(uim_ptr->card_cmd.warning1_indicator == TRUE)
      {
        uim_ptr->card_cmd.get_resp_sw1 = SW1_WARNINGS1;
      }
      else if(uim_ptr->card_cmd.warning2_indicator == TRUE)
      {
        uim_ptr->card_cmd.get_resp_sw1 = SW1_WARNINGS2;
      }

    /* Reset the warning1 indicators */
    uim_ptr->card_cmd.warning1_indicator = FALSE;

    /* Reset the warning2 indicators */
    uim_ptr->card_cmd.warning2_indicator = FALSE;

    uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  } /* end if - the second status word is proper */
  else
  {

    if(uim_processing_stream_apdu(uim_ptr))
    {
      /* In case of stream apdu:
        for non-spec defined combinations of SW2 send response data
        and SW to the client.
      */
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
        uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;
    }
    else
    {
      /* Something went wrong */
      uim_ptr->flag.processing_download_error = FALSE;
      uim_rx_isr_process_unknown_proc_byte(uim_ptr);
      return;

    }
  }
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_normal_end */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_fetch_end
(
  byte                      sw2, /* Second status byte */
  uim_instance_global_type *uim_ptr
)
{
  /* Save the status word.  It contains the size of the proactive command. */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  /* Place the number of bytes read in the response buffer */
  uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

  uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

  /* Check if this from processing a download error */
  if (uim_ptr->flag.processing_download_error)
  {
    uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_DLOAD_ERR;
    uim_ptr->card_cmd.resp_buf_ptr->sw2 = uim_ptr->card_cmd.cmd_req.apdu_hdr.p3;
  } /* end if - Finished processing a data download error */

  /* Check if we are processing an ENVELOPE APDU:
     If TRUE and warnings were received earlier, set the
     appropriate SW1 WARNING status. */
  if(uim_ptr->card_cmd.warning1_indicator == TRUE)
  {
    uim_ptr->card_cmd.get_resp_sw1 = SW1_WARNINGS1;
  }
  else if(uim_ptr->card_cmd.warning2_indicator == TRUE)
  {
    uim_ptr->card_cmd.get_resp_sw1 = SW1_WARNINGS2;
  }

  /* Reset the warning1 indicators */
  uim_ptr->card_cmd.warning1_indicator = FALSE;

  /* Reset the warning2 indicators */
  uim_ptr->card_cmd.warning2_indicator = FALSE;

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_fetch_end */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_dload_err
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  uim_ptr->card_cmd.get_resp_sw1 = SW1_DLOAD_ERR;
  uim_ptr->card_cmd.get_resp_sw2 = (uim_sw2_type)sw2;

  /* Determine if the local buffer is full */
  if (uim_ptr->card_cmd.num_resp_bytes_rcvd_total < UIM_MAX_CHARS)
  {
    /* SW1/SW2 9E xx is defined only for GSM card. For a non ICC card if we get
       9E XX for a streaming APDU we should just return response data
       and SWs. */
    if(uim_processing_stream_apdu(uim_ptr) &&
       uim_ptr->debug.last_sent_apdu.uim_class != UIM_ICC_INSTRUCTION_CLASS)
    {
      /* End this transaction */
      uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_DLOAD_ERR;
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = (uim_sw2_type)sw2;

      /* Place the number of bytes read in the response buffer */
      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

      /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
      uim_ptr->flag.processing_download_error = FALSE;

      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

      uim_command_response_callback(uim_ptr);

      return;
    }
    /* Determine how much will fit into the buffer */
    if ((uim_ptr->card_cmd.num_resp_bytes_rcvd_total + sw2) > UIM_MAX_CHARS)
    {
      sw2 = (byte)(UIM_MAX_CHARS - uim_ptr->card_cmd.num_resp_bytes_rcvd_total);
    } /* end if - requested size is too big for local buffer */

    /* Build the GET RESPONSE command */
    /* This needs to be modified to work with WIM */
    if ((uim_ptr->card_cmd.uimdrv_protocol == UIM_UICC) ||
        (uim_ptr->card_cmd.uimdrv_protocol == UIM_WCDMA))
    {
      uim_ptr->card_cmd.cmd_req.apdu_hdr.uim_class =
        uim_ptr->debug.last_sent_apdu.uim_class & UIM_GET_RESPONSE_CLA_MASK;
    }
    else
    {
      uim_ptr->card_cmd.cmd_req.apdu_hdr.uim_class = 0xA0;
    }

    uim_ptr->card_cmd.cmd_req.protocol        = uim_ptr->card_cmd.uimdrv_protocol;
    uim_ptr->card_cmd.cmd_req.apdu_hdr.instrn = GET_RESPONSE;
    uim_ptr->card_cmd.cmd_req.apdu_hdr.p1     = 0x00;
    uim_ptr->card_cmd.cmd_req.apdu_hdr.p2     = 0x00;
    uim_ptr->card_cmd.cmd_req.apdu_hdr.p3     = sw2;
    uim_ptr->card_cmd.cmd_req.instrn_case     = UIM_INSTRN_CASE_2;
    uim_ptr->card_cmd.cmd_req.rsp_ptr         = uim_ptr->card_cmd.resp_buf_ptr;

    /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
    uim_ptr->flag.processing_download_error = TRUE;

    /* Wait long enough before responding to procedure byte */
    uim_clk_busy_wait( uim_ptr->setting.response_byte_delay );

    /* Send the GET RESPONSE command */
    uim_send_apdu ( &uim_ptr->card_cmd.cmd_req, uim_ptr);
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
    uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_DLOAD_ERR;
    if(uim_processing_stream_apdu(uim_ptr) &&
       uim_ptr->debug.last_sent_apdu.uim_class != UIM_ICC_INSTRUCTION_CLASS)
    {
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;
    }
    else
    {
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = uim_ptr->card_cmd.cmd_req.apdu_hdr.p3;
    }
    /* Place the number of bytes read in the response buffer */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

    uim_ptr->flag.processing_download_error = FALSE;

    uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

    uim_command_response_callback(uim_ptr);

  } /* end if - the local buffer is full */
} /* uim_rx_isr_sw1_dload_err */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_nfc
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;


  if(uim_processing_stream_apdu(uim_ptr))
  {
    uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

  }
  else
  {
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

    /* Reset the receiver and leave it disabled.  This will ignore any
       more characters received from the UIM.        */
    UIM_RESET_RX_SLOT(uim_ptr);
  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_nfc */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting the second
 * status word after a response end status word.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_resp_end
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Send a get_response to this command */
  /* Get the length of the GET RESPONSE command response. */
  uim_ptr->card_cmd.get_resp_sw2 = (uim_sw2_type)sw2;

  /* Determine if the local buffer is full */
  if (uim_ptr->card_cmd.num_resp_bytes_rcvd_total < UIM_MAX_CHARS)
  {

    if(uim_processing_stream_apdu(uim_ptr))
    {
      uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_USIM_END_RESP;
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;
      /* Indicate APDU result */
      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

      /* Place the number of bytes read in the response buffer */
      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
      /* Change to idle */
      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

      /* Call the response callback function */
      uim_command_response_callback(uim_ptr);
    }
    else
    {
      /* Determine how much will fit into the buffer */
      if ((uim_ptr->card_cmd.num_resp_bytes_rcvd_total + sw2) > UIM_MAX_CHARS)
      {
        sw2 = (byte)(UIM_MAX_CHARS - uim_ptr->card_cmd.num_resp_bytes_rcvd_total);
      } /* end if - requested size is too big for local buffer */

      /* Build the GET RESPONSE command */
      /* This needs to be modified to work with WIM */
      if ((uim_ptr->card_cmd.uimdrv_protocol == UIM_UICC) ||
          (uim_ptr->card_cmd.uimdrv_protocol == UIM_WCDMA))
      {
        uim_ptr->card_cmd.cmd_req.apdu_hdr.uim_class =
          uim_ptr->debug.last_sent_apdu.uim_class & UIM_GET_RESPONSE_CLA_MASK;
      }
      else
      {
        uim_ptr->card_cmd.cmd_req.apdu_hdr.uim_class = 0xA0;
      }

      uim_ptr->card_cmd.cmd_req.protocol        = uim_ptr->card_cmd.uimdrv_protocol;
      uim_ptr->card_cmd.cmd_req.apdu_hdr.instrn = GET_RESPONSE;
      uim_ptr->card_cmd.cmd_req.apdu_hdr.p1     = 0x00;
      uim_ptr->card_cmd.cmd_req.apdu_hdr.p2     = 0x00;
      uim_ptr->card_cmd.cmd_req.instrn_case     = UIM_INSTRN_CASE_2;
      uim_ptr->card_cmd.cmd_req.rsp_ptr         = uim_ptr->card_cmd.resp_buf_ptr;

      /* If this was a warning let us set the length to 0 */
      if (uim_ptr->rxtx_state_machine.rx_state == UIM_RX_SW1_WARNINGS1 ||
          uim_ptr->rxtx_state_machine.rx_state == UIM_RX_SW1_WARNINGS2)
      {
        uim_ptr->card_cmd.cmd_req.apdu_hdr.p3     = 0;
      }
      else
      {
        uim_ptr->card_cmd.cmd_req.apdu_hdr.p3     = sw2;
      }

      /* Wait long enough before responding to procedure byte */
      uim_clk_busy_wait( uim_ptr->setting.response_byte_delay );

      /* Send the GET RESPONSE command */
      uim_send_apdu ( &uim_ptr->card_cmd.cmd_req, uim_ptr);
    }
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
    /* Check if this from processing a download error */
    if (uim_ptr->flag.processing_download_error)
    {
      uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_DLOAD_ERR;
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = uim_ptr->card_cmd.cmd_req.apdu_hdr.p3;
    } /* end if - Finished processing a data download error */
    else /* Emulate a normal end */
    {
      uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_NORMAL_END;
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = (int)SW2_NORMAL_END;
    }
    /* Place the number of bytes read in the response buffer */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

    /* Change to idle */
    uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

    /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
    uim_ptr->flag.processing_download_error = FALSE;

    /* Call the response callback function */
    uim_command_response_callback(uim_ptr);
  } /* end if - the local buffer is full */
} /* uim_rx_isr_sw1_resp_end */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_busy
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  /* Make sure the second status word is proper */
  if (sw2 == (int)SW2_NORMAL_END)
  {
    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_TOOLKIT_BUSY;

    /* Set the number of bytes received to zero. */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

    /* Change to idle */
    uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;
  } /* end if - the second status word is proper */
  else if ( (sw2 != (int)SW2_NORMAL_END ) &&
            (!uim_processing_stream_apdu(uim_ptr)) )
  {
    /* Something went wrong.
       Set the state to UIM_RX_UNKNOWN_PROC_BYTE_RCVD. */

    /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
    uim_ptr->flag.processing_download_error = FALSE;
    uim_rx_isr_process_unknown_proc_byte(uim_ptr);
    return;
  }

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate the task to ignore the following timeout */
    uim_ptr->flag.bad_status_words_error = FALSE;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

    /* Change to idle */
    uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;
  }
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_busy */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_retries
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  /* Determine if it is a memory problem */
  if (sw2 == (int)SW2_MEM_PROBLEM)
  {
    /* Set the number of bytes received to zero. */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_MEMORY_PROBLEM;
  }
  else /* successful after retries */
  {
    /* Place the number of bytes read in the response buffer */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  } /* end if - the second status word indicates a memory problem */

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_retries */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_reference
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  /* Process the second status word */
  switch (sw2)
  {
    case SW2_NO_EF:
      {
        /* Set the number of bytes received to zero. */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_NO_EF_SELECTED;
      }
      break;

    case SW2_OUT_OF_RANGE:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_OUT_OF_RANGE;
      }
      break;

    case SW2_NOT_FOUND:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_NOT_FOUND;
      }
      break;

    case SW2_INCONSISTENT:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_INCONSISTENT_WITH_COMMAND;
      }
      break;

    case SW2_ALGORITHM_NOT_SUPPORTED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_ALGORITHM_NOT_SUPPORTED;
      }
      break;

    case SW2_INVALID_KEY_CHECK_VALUE:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_INVALID_KEY_CHECK_VALUE;
      }
      break;

    default:
      {
        /* Set the number of bytes received to zero. */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM. */
        UIM_RESET_RX_SLOT (uim_ptr);
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_reference */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_security
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  /* Process the second status word */
  switch (sw2)
  {
    case SW2_NO_CHV:
      {
        /* Set the number of bytes received to zero. */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_NO_CHV_INITIALIZED;
      }
      break;

    case SW2_NOT_FULFILLED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_ACCESS_NOT_FULFILLED;
      }
      break;

    case SW2_CONTRADICTION:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CONTRADICTION_W_CHV_STATUS;
      }
      break;

    case SW2_INVALIDATION:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status =
          UIM_CONTRADICTION_W_INVALID_STATUS;
      }
      break;

    case SW2_SEQ_PROBLEM:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_SSD_UPDATE_SEQ_PROBLEM;
      }
      break;

    case SW2_BLOCKED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CHV_BLOCKED;
      }
      break;

    case SW2_MAX_REACHED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_MAX_VALUE_REACHED;
      }
      break;

    case SW2_APPLN_SPECIFIC_AUTH_ERR:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_APPLCN_SPECIFIC_AUTH_ERROR;
      }
      break;

    default:
      {
        /* Set the number of bytes received to zero. */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM. */
        UIM_RESET_RX_SLOT (uim_ptr);
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_security */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_p3_bad
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

  uim_ptr->card_cmd.get_resp_sw1 = SW1_P3_BAD;
  uim_ptr->card_cmd.get_resp_sw2 = (uim_sw2_type)sw2;

  if (sw2 == 0x00)
  {
    /* Save the status word in case it is a true status word response */
    uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

    if(uim_processing_stream_apdu(uim_ptr))
    {
      /* Set the number of bytes received to the bytes received. */
      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
        uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
    }
    else
    {
      /* Set the number of bytes received to zero. */
      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_INCORRECT_PARAM_P3;
    }

    /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
    uim_ptr->flag.processing_download_error = FALSE;

   uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

    uim_command_response_callback(uim_ptr);

  }
  /* If SW2 is XX !=0, then resend the cmd with SW2 as P3 */
  else
  {
    /* Determine if the local buffer is full */
    if (uim_ptr->card_cmd.num_resp_bytes_rcvd_total < UIM_MAX_CHARS)
    {
      if(uim_processing_stream_apdu(uim_ptr))
      {
        /* In case of stream apdu:
           1. Report bad p3 status words to the clients.
           2. Do not perform resend apdu internally.
        */
        /* Save the status word in case it is a true status word response */
        uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

        /* Set the number of bytes received to the bytes received. */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
          uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

        uim_ptr->flag.processing_download_error = FALSE;

        uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

        uim_command_response_callback(uim_ptr);

        return;
      }
      else
      {
        /* Determine how much will fit into the buffer */
        if ((uim_ptr->card_cmd.num_resp_bytes_rcvd_total + sw2) > UIM_MAX_CHARS)
        {
          sw2 = (byte)(UIM_MAX_CHARS - uim_ptr->card_cmd.num_resp_bytes_rcvd_total);
        } /* end if - requested size is too big for local buffer */

        /* Resend the last APDU with p3 equal to SW2 */
        uim_ptr->card_cmd.cmd_req.protocol        = uim_ptr->card_cmd.uimdrv_protocol;
        uim_ptr->card_cmd.cmd_req.apdu_hdr        = uim_ptr->debug.last_sent_apdu;
        uim_ptr->card_cmd.cmd_req.apdu_hdr.p3     = sw2;
        uim_ptr->card_cmd.cmd_req.instrn_case     = uim_ptr->state.last_instrn_case;
        uim_ptr->card_cmd.cmd_req.rsp_ptr         = uim_ptr->card_cmd.resp_buf_ptr;

        /* Wait long enough before responding to procedure byte */
        uim_clk_busy_wait( uim_ptr->setting.response_byte_delay );

        uim_send_apdu ( &uim_ptr->card_cmd.cmd_req, uim_ptr);
      }
    }
    else /* The local buffer is full */
    {
      /* End this transaction */
      /* Check if this from processing a download error */
      if (uim_ptr->flag.processing_download_error)
      {
        uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_DLOAD_ERR;
        uim_ptr->card_cmd.resp_buf_ptr->sw2 = uim_ptr->card_cmd.cmd_req.apdu_hdr.p3;
      }
      else /* Emulate a normal end */
      {
        uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_NORMAL_END;
        uim_ptr->card_cmd.resp_buf_ptr->sw2 = (int)SW2_NORMAL_END;
      }
      /* Place the number of bytes read in the response buffer */
      uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

      uim_ptr->flag.processing_download_error = FALSE;

      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

      uim_command_response_callback(uim_ptr);

    } /* end if - the local buffer is full */
  } /* SW2 is 0 */
} /* uim_rx_isr_sw1_p3_bad */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_p1_p2_bad
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }
  else
  {
    /* Set the number of bytes received to zero. */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_INCORRECT_PARAM_P1_2;
  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_p1_p2_bad */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_ins_bad
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }
  else
  {
    /* Set the number of bytes received to zero. */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_UNKNOWN_INS;
  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_ins_bad */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_problem
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

  /* Indicate APDU result */
  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_SELECT_F:
    case UIM_READ_BIN_CMD_F:
    case UIM_READ_REC_CMD_F:
    {
      uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PROBLEM;
    }
    break;

    default:
    {
      if( uim_ptr->card_cmd.resp_buf_ptr->sw2 != SW2_SW1_PROBLEM)
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
      }
      else
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PROBLEM;
      }
    }
  }      

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_problem */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_resend_apdu
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
  /* Determine if the local buffer is full */
  if (uim_ptr->card_cmd.num_resp_bytes_rcvd_total < UIM_MAX_CHARS)
  {
    /* Determine how much will fit into the buffer */
    if ((uim_ptr->card_cmd.num_resp_bytes_rcvd_total + sw2) > UIM_MAX_CHARS)
    {
      sw2 = (byte)(UIM_MAX_CHARS - uim_ptr->card_cmd.num_resp_bytes_rcvd_total);
    } /* end if - requested size is too big for local buffer */

    /* Resend the last APDU with p3 equal to SW2*/
    uim_ptr->card_cmd.cmd_req.protocol        = uim_ptr->card_cmd.uimdrv_protocol;
    uim_ptr->card_cmd.cmd_req.apdu_hdr        = uim_ptr->debug.last_sent_apdu;
    uim_ptr->card_cmd.cmd_req.apdu_hdr.p3     = sw2;
    uim_ptr->card_cmd.cmd_req.instrn_case     = uim_ptr->state.last_instrn_case;
    uim_ptr->card_cmd.cmd_req.rsp_ptr         = uim_ptr->card_cmd.resp_buf_ptr;

    /* Wait long enough before responding to procedure byte */
    uim_clk_busy_wait( uim_ptr->setting.response_byte_delay );

    /* Send the last command command */
    uim_send_apdu ( &uim_ptr->card_cmd.cmd_req, uim_ptr);
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
    /* Check if this from processing a download error */
    if (uim_ptr->flag.processing_download_error)
    {
      uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_DLOAD_ERR;
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = uim_ptr->card_cmd.cmd_req.apdu_hdr.p3;
    } /* end if - Finished processing a data download error */
    else /* Emulate a normal end */
    {
      uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_NORMAL_END;
      uim_ptr->card_cmd.resp_buf_ptr->sw2 = (int)SW2_NORMAL_END;
    }
    /* Place the number of bytes read in the response buffer */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

    /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
    uim_ptr->flag.processing_download_error = FALSE;

    /* Change to idle */
    uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

    /* Call the response callback function */
    uim_command_response_callback(uim_ptr);

  } /* end if - the local buffer is full */

} /* uim_rx_isr_sw1_resend_apdu */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_warnings1
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  switch (sw2)
  {
    case SW2_NV_STATE_UNCHANGED:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_NO_INFO_AND_NV_UNCHANGED;
      }
      break;

    case SW2_RET_DATA_MAY_BE_CORRUPTED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PART_DATA_MAY_BE_CORRUPTED;
      }
      break;

    case SW2_EOF_REACHED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_EOF_BEFORE_READING_LE_BYTES;
      }
      break;

    case SW2_SEL_EF_INVALIDATED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status   = UIM_DONE;
      }
      break;

    case SW2_INCORRECT_FCI_FORMAT:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_INCORRECT_FCI_FORMAT;
      }
      break;

    case SW2_AUTH_RSP_DATA:
    case SW2_MORE_DATA_AVAILABLE:
    case SW2_MORE_DATA:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
      }
      break;

    default:
      {
        if (uim_processing_stream_apdu(uim_ptr) != TRUE)
        {
          /* Indicate APDU result */
          uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

          /* Reset the receiver and leave it disabled.  This will ignore any
             more characters received from the UIM. */
          UIM_RESET_RX_SLOT (uim_ptr);
        }
      }
      /* If the final SW for activate aid for a non-telecom ADF are 62 80 then we need to
         mark transaction complete and send any response data if available */
      if ((UIM_SELECT_F                == uim_ptr->command.cmd_ptr->hdr.command)&&
         (UIM_ADF                      == uim_ptr->command.cmd_ptr->select.file_type) &&
         (FALSE                        == uim_is_telecom_aid(uim_ptr->command.cmd_ptr->select.aid))&&
         (SW2_BAD_PARAMS_IN_DATA_FIELD == sw2))
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status   = UIM_DONE;
      }
      break;

  } /* end switch - sw2 */
   /* Always allow to send response 0n 0x62 warnings */
  if (uim_processing_stream_apdu(uim_ptr) == TRUE)
  {
    uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_WARNINGS1;
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status   = UIM_DONE;
  }
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_warnings1 */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_warnings2
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

  /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  switch (sw2)
  {
    case SW2_NV_STATE_CHANGED:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_NO_INFO_AND_NV_CHANGED;
      }
      break;

    case SW2_FILE_FILLED_BY_LAST_WRITE:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_FILE_FILLED_UP_BY_LAST_WRITE;
      }
      break;

    case SW2_MORE_DATA_AVAILABLE:
    case SW2_MORE_DATA:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
      }
      break;

    default:
      {
        if ((sw2 & 0xF0) == (int)SW2_CMD_SUCCESS_INT_X_RETRIES)
        {
           uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CMD_SUCCESS_WITH_X_RETRIES;

         /* If the final SW for activate aid for a non-telecom ADF are 63 C1 then we need to
            mark transaction complete and send any response data if available */
          if ((UIM_SELECT_F        == uim_ptr->command.cmd_ptr->hdr.command)&&
              (UIM_ADF             == uim_ptr->command.cmd_ptr->select.file_type) &&
              (FALSE               == uim_is_telecom_aid(uim_ptr->command.cmd_ptr->select.aid)) &&
              (SW2_WARNING_NFC_C1) == sw2)
          {
            uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
            uim_ptr->card_cmd.resp_buf_ptr->cmd_status   = UIM_DONE;
          }
        }
        else
        {
           if (uim_processing_stream_apdu(uim_ptr) != TRUE)
           {
              uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

              /* Reset the receiver and leave it disabled.  This will ignore any
               more characters received from the UIM. */
              UIM_RESET_RX_SLOT (uim_ptr);
           }
        }
      }
      break;

  } /* end switch - sw2 */
  /* Always allow to send response 0n 0x62 warnings */
  if (uim_processing_stream_apdu(uim_ptr) == TRUE)
  {
    uim_ptr->card_cmd.resp_buf_ptr->sw1 = (int)SW1_WARNINGS2;
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status   = UIM_DONE;
  }
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_warnings2 */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_execn_err2
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  switch (sw2)
  {
    case SW2_NV_STATE_CHANGED:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_EXEC_ERR_AND_NV_CHANGED;
      }
      break;

    case SW2_MEMORY_PROBLEM:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_EXEC_ERR_MEMORY_PROBLEM;
      }
      break;

    default:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM. */
        UIM_RESET_RX_SLOT (uim_ptr);
      }
      break;

  } /* end switch - sw2 */

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_execn_err2 */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_cmd_not_allowed
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  switch (sw2)
  {
    case SW2_NO_INFO_GIVEN:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CMD_ERR_NO_INFO;
      }
      break;

    case SW2_FILE_STRUCT_INCOMPATIBLE:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status =
          UIM_CMD_INCOMPATIBLE_WITH_FILE_STRUCT;
      }
      break;


    case SW2_SECURITY_NOT_SATISFIED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_SEC_STATUS_NOT_SATISFIED;
      }
      break;

    case SW2_PIN_BLOCKED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_AUTH_PIN_BLOCKED;
      }
      break;

    case SW2_REF_DATA_INVALIDATED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_REF_DATA_INVALIDATED;
      }
      break;

    case SW2_CONDITIONS_NOT_SATISFIED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_USE_CONDNS_NOT_SATISFIED;
      }
      break;

    case SW2_NO_EF_SELECTED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CMD_ERR_NO_EF_SELECTED;
      }
      break;

    case SW2_UNDEF_99:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_WRONG_CLASS;
      }
      break;

    default:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM. */
        UIM_RESET_RX_SLOT (uim_ptr);
      }
      break;

  } /* end switch - sw2 */

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_cmd_not_allowed */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_wrong_params
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  switch (sw2)
  {
    case SW2_BAD_PARAMS_IN_DATA_FIELD:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_INCORRECT_PARAM_IN_DATA_FIELD;
      }
      break;

    case SW2_FUNCTION_NOT_SUPPORTED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_FUNCTION_NOT_SUPPORTED;
      }
      break;

    case SW2_FILE_NOT_FOUND:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_FILE_NOT_FOUND;
      }
      break;

    case SW2_RECORD_NOT_FOUND:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_RECORD_NOT_FOUND;
      }
      break;

    case SW2_INCONSISTENT_LC:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_LC_INCONSISITENT_WITH_TLV;
      }
      break;

    case SW2_BAD_PARAMS_P1_P2:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_INCORRECT_PARAMS_P1_P2;
      }
      break;

    case SW2_LC_INCONSISTENT_WITH_P1_P2:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_LC_INCONSISITENT_WITH_P1_P2;
      }
      break;

    case SW2_REF_DATA_NOT_FOUND:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_REF_DATA_NOT_FOUND;
      }
      break;

    case SW2_NOT_ENOUGH_MEMORY_SPACE:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_NOT_ENOUGH_MEMORY_SPACE;
      }
      break;

    default:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM. */
        UIM_RESET_RX_SLOT (uim_ptr);
      }
      break;

  } /* end switch - sw2 */

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    if((SW2_FILE_NOT_FOUND == sw2) &&
        /* Check if the command is streamed in default channel */
       (!(uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET] 
          & (UIM_LOGICAL_CHANNEL_MASK | UIM_UICC_EXTENDED_LC_CLASS) ))
       )
    {
      /* Select MF if stream select on default channel gets FILE NOT FOUND */
      /* No 'MF selection' for non-zero channels to avoid performance issues */
      uim_ptr->state.mf_selection.required = TRUE;
      uim_ptr->state.mf_selection.channel = 0;
    }

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_wrong_params */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_se_fail
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* Set the number of bytes received to zero. */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }
  else
  {
    /* Set the number of bytes received to zero. */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_SE_NOT_SET;
  }

  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_se_fail */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_execn_err1
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{
    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  switch (sw2)
  {
    case SW2_NV_STATE_UNCHANGED:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_EXEC_ERR_AND_NV_UNCHANGED;
      }
      break;

    case SW2_MORE_DATA_AVAILABLE:
    case SW2_MORE_DATA:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd_total;
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
      }
      break;

    default:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM. */
        UIM_RESET_RX_SLOT (uim_ptr);
      }
      break;

  } /* end switch - sw2 */

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */

    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_execn_err1 */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called when expecting status words
 * from the UIM. The response comes as the result of an APDU.
 *
 * @param sw2 Second Status Word received from UIM.
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_sw1_cla_not_supported
(
  byte                      sw2,
  uim_instance_global_type *uim_ptr
)
{

    /* Save the status word in case it is a true status word response */
  uim_ptr->card_cmd.resp_buf_ptr->sw2 = sw2;

  switch (sw2)
  {
    case SW2_NO_INFO_GIVEN:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_CLA_ERR_NO_INFO;
      }
      break;

    case SW2_LOGICAL_CHAN_NOT_SUPPORTED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_LOGICAL_CHAN_NOT_SUP;
      }
      break;


    case SW2_SECURE_MSG_NOT_SUPPORTED:
      {
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_SECURE_MESSAGING_NOT_SUP;
      }
      break;

    default:
      {
        /* Indicate APDU result */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM. */
        UIM_RESET_RX_SLOT (uim_ptr);
      }
      break;

  } /* end switch - sw2 */

  if(uim_processing_stream_apdu(uim_ptr))
  {
    /* FOR NFC streaming APDUs pack the response data if any available
       and SWs regardless of SW2 value. Also set the APDU transaction
       result to UIM_DONE
    */

    uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size =
      uim_ptr->card_cmd.num_resp_bytes_rcvd_total;

    /* Indicate APDU result */
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;
  }
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  uim_ptr->flag.processing_download_error = FALSE;

  /* Change to idle */
  uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;

  /* Call the response callback function */
  uim_command_response_callback(uim_ptr);

} /* uim_rx_isr_sw1_cla_not_supported */


#if defined( FEATURE_UIM_T_1_SUPPORT )
/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called while sending the prologue
 * field of a T=1 block.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_send_prologue
(
  uim_instance_global_type *uim_ptr
)
{
  unsigned char *byte_ptr = NULL;

  uint16 i = 0;

  if( !(uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_TXREADY ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_TXLEV) )
  {
    return;
  } /* If its not UIM_RX_TXREADY or UIM_RX_TXLEV then return */

  byte_ptr = uim_ptr->card_cmd.request_buffer_ptr;

  /* Keep track of EDC */
  if (UIM_T_1_EDC_LRC == uim_ptr->t1.t_1_edc)
  {
    for(i = 0; i< uim_ptr->card_cmd.num_bytes_to_send; i++)
    {
      uim_ptr->t1.tx_epilogue_field.lrc ^= byte_ptr[i];
    }
  }
  else /* The EDC is CRC */
  {
    for(i = 0; i< uim_ptr->card_cmd.num_bytes_to_send; i++)
    {
      /* Compute the CRC with this byte */
      uim_ptr->t1.tx_epilogue_field.crc =
        crc_16_l_table[ (uim_ptr->t1.tx_epilogue_field.crc ^
                         byte_ptr[i]) & 0x00ff ]
         ^ (uim_ptr->t1.tx_epilogue_field.crc >> 8);
    }
  } /* end if - the EDC is LRC */

  /* Program the NO_CHARS_FOR_TX register with the size of TX bytes
     to be sent */
  UIM_NUM_CHARS_TO_SEND_SLOT(uim_ptr,uim_ptr->card_cmd.num_bytes_to_send);

  /* Write the TX bytes into the TX FIFO */
  uim_tx(uim_ptr->card_cmd.request_buffer_ptr,
                    uim_ptr->card_cmd.num_bytes_to_send,
                    uim_ptr);

  uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_TXREADY;
  uim_ptr->card_cmd.num_bytes_to_send = 0;

  /* Determine if this is the last byte for this buffer */
  if (uim_ptr->card_cmd.num_bytes_to_send == 0)
  {
    /* Determine if there is an information field to send */
    if (uim_ptr->card_cmd.num_cmd_data_bytes_to_send != 0)
    {
      /* Send the information field */
      uim_ptr->card_cmd.num_bytes_to_send =
        uim_ptr->card_cmd.num_cmd_data_bytes_to_send;
      uim_ptr->card_cmd.request_buffer_ptr = uim_ptr->card_cmd.apdu_command_data_ptr;
      uim_ptr->rxtx_state_machine.rx_state = UIM_RX_T_1_SEND_INFO_ST;
    }
    else /* no information field to send out */
    {
      /* Send the epilogue field */
      uim_ptr->card_cmd.num_bytes_to_send = uim_ptr->t1.epilogue_field_size;
      /* Determine if CRC is used for EDC */
      if (UIM_T_1_EDC_CRC == uim_ptr->t1.t_1_edc)
      {
        /* Complement the CRC field */
        uim_ptr->t1.tx_epilogue_field.crc = ~uim_ptr->t1.tx_epilogue_field.crc;
      } /* end if - check if we are using CRC for EDC */
      uim_ptr->card_cmd.request_buffer_ptr = (byte *)&uim_ptr->t1.tx_epilogue_field;
     uim_ptr->rxtx_state_machine.rx_state = UIM_RX_T_1_SEND_EPILOGUE_ST;
    } /* end if - there is an information field to send */
    /* Exit this loop */
    return;
  }
} /* uim_rx_isr_send_prologue */


/**
 * This function runs within the context of the Rx ISR of the UART.
 * This function is called while receiving a T=1 block.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 * @param uart_rx_buf_ptr Pointer to the UART RX buffer.
 * @param no_of_bytes_received Number of bytes received in this
 *                             transaction.
 */
void uim_rx_isr_recv_t_1_block
(
  uim_instance_global_type *uim_ptr,
  dword* uart_rx_buf_ptr,
  uint32 no_of_bytes_received
)
{
  byte  rx_value_array[4];

  /* Boolean variable to indicate if a new RX initialization is required */
  boolean reinit_rxtransfer = TRUE;

  /* Running index */
  uint16 i = 0;
  uint16 bytes_remaining = 0;
  uint16 uart_rx_buf_index = 0;

  /* Wait for TX_COMP event to get confirmation that the epilogue
     bytes are sent */
  if ( uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_TXCOMP )
  {
    /* Set the ISR timer to the block waiting time */
    uim_ptr->card_cmd.work_waiting_time = uim_ptr->t1.t_1_bwt * uim_ptr->t1.t_1_wtx;
    uim_ptr->card_cmd.work_waiting_time_etus = uim_ptr->t1.t_1_bwt_etus * uim_ptr->t1.t_1_wtx;
    /* Reset the waiting time extension */
    uim_ptr->t1.t_1_wtx = 1;
    uim_start_cmd_rsp_timer((uim_ptr->card_cmd.work_waiting_time +
                             UIM_UART_DM_WAITING_TIME_CORRECTION),
                            uim_ptr);
    uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;
    uim_ptr->rxtx_state_machine.rx_t1_block_sub_state = UIM_RX_T_1_RX_PROLOGUE_ST;
    return;
  }

  /* Return, if the RX sub state is not RX_STALE, RX_LEV, ATR received with parity errors
     or parity errors in T=1 block.
     In the case of parity errors in T=1 block, we have to send an R block to the card */
  if( !(uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXSTALE ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXLEV ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_T_1_RX_RECEIVED_WITH_PARITY_ERRORS ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_T_1_RX_LEV_RECEIVED_WITH_PARITY_ERRORS) )
  {
    return;
  }

  if(uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS ||
     uim_ptr->rxtx_state_machine.rx_sub_state == UIM_T_1_RX_RECEIVED_WITH_PARITY_ERRORS ||
     uim_ptr->rxtx_state_machine.rx_sub_state == UIM_T_1_RX_LEV_RECEIVED_WITH_PARITY_ERRORS)
  {
    uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
  }

  uim_ptr->card_cmd.num_bytes_received = no_of_bytes_received;

  bytes_remaining    = no_of_bytes_received;

  if(uim_ptr->card_cmd.num_bytes_received % 4)
  {
    uim_ptr->card_cmd.num_words_received = ((uim_ptr->card_cmd.num_bytes_received/4) + 1) ;
  }
  else
  {
    uim_ptr->card_cmd.num_words_received = uim_ptr->card_cmd.num_bytes_received/4 ;
  }

  /* Process all the bytes received in the uart_rx_buffer */

  while( (uim_ptr->card_cmd.num_words_received > 0)&&
         uart_rx_buf_index < uim_ptr->card_cmd.num_words_received )
  {
    rx_value_array[0] = (byte)(uart_rx_buf_ptr[uart_rx_buf_index]  & 0xFF);
    rx_value_array[1] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF00)     >> 8);
    rx_value_array[2] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF0000)   >> 16);
    rx_value_array[3] = (byte)((uart_rx_buf_ptr[uart_rx_buf_index] & 0xFF000000) >> 24);

    uart_rx_buf_index = uart_rx_buf_index + 1;

    for ( i = 0; i < 4 && bytes_remaining > 0; i++)
    {
      bytes_remaining--;

      /* Sub state waiting for PROLOGUE bytes */
      if(uim_ptr->rxtx_state_machine.rx_t1_block_sub_state == UIM_RX_T_1_RX_PROLOGUE_ST)
      {
        /* Reset the waiting time extension */
        uim_ptr->t1.t_1_wtx = 1;
        /* Default command status is a successful command */
        uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_DONE;

        /* Set the ISR timer to the character waiting time */
        uim_ptr->card_cmd.work_waiting_time = uim_ptr->t1.t_1_cwt;
        uim_ptr->card_cmd.work_waiting_time_etus = uim_ptr->t1.t_1_cwt_etus;
        uim_start_cmd_rsp_timer((uim_ptr->card_cmd.work_waiting_time +
                                 UIM_UART_DM_WAITING_TIME_CORRECTION),
                                uim_ptr);

          /* Make sure there is no problem with the received byte */
         if (uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS ||
             uim_ptr->rxtx_state_machine.rx_sub_state == UIM_T_1_RX_RECEIVED_WITH_PARITY_ERRORS ||
             uim_ptr->rxtx_state_machine.rx_sub_state == UIM_T_1_RX_LEV_RECEIVED_WITH_PARITY_ERRORS)
         {
           /* Indicate APDU result */
           uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
         }

        /* Process error detection code for this byte */
        if (UIM_T_1_EDC_LRC == uim_ptr->t1.t_1_edc)
        {
          uim_ptr->t1.error_detection_code ^= rx_value_array[i];
        }
        else /* The EDC is CRC */
        {
          /* Compute the CRC with this byte */
          uim_ptr->t1.error_detection_code =
            crc_16_l_table[ (uim_ptr->t1.error_detection_code ^ rx_value_array[i]) & 0x00ff ]
             ^ (uim_ptr->t1.error_detection_code >> 8);
        } /* The EDC is LRC */

        /* Place the byte in the prologue field buffer. */
        uim_ptr->t1.rx_prologue_field_buffer_ptr[uim_ptr->card_cmd.num_resp_bytes_rcvd] = rx_value_array[i];

        /* Keep track of the number of bytes received. */
        uim_ptr->card_cmd.num_resp_bytes_rcvd++;

        /* Determine if all the expected bytes have been received. */
        if (uim_ptr->card_cmd.num_resp_bytes_rcvd == UIM_T_1_NUM_BYTES_PROLOGUE)
        {
          /* Initialize the number of bytes received */
          uim_ptr->card_cmd.num_resp_bytes_rcvd = 0;

          /* Let us receive the entire APDU but throw it away and request
           * a re-transmission by indicating a failure
           */
          if (uim_ptr->t1.rx_prologue_field_buffer.len > uim_ptr->t1.t_1_ifsd)
          {
            /* Indicate APDU result */
            uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
          }

          /* determine if there are any information bytes */
          if (uim_ptr->t1.rx_prologue_field_buffer.len > 0)
          {
            /* Prepare to receive the information field */
            uim_ptr->rxtx_state_machine.rx_t1_block_sub_state = UIM_RX_T_1_RX_INFO_ST;
            uim_ptr->card_cmd.num_resp_data_bytes_expected =
              uim_ptr->t1.rx_prologue_field_buffer.len;
          }
          else /* no information bytes are expected */
          {
            /* Prepare to receive the epilogue field */
            uim_ptr->rxtx_state_machine.rx_t1_block_sub_state = UIM_RX_T_1_RX_EPILOGUE_ST;

            /* Place the number of bytes read in the response buffer */
            uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = 0;

            /* Determine if CRC is used for EDC */
            if (UIM_T_1_EDC_CRC == uim_ptr->t1.t_1_edc)
            {
              /* Complement the CRC field */
              uim_ptr->t1.error_detection_code = ~uim_ptr->t1.error_detection_code;
            } /* end if - check if we are using CRC for EDC */
          } /* end if - There are information bytes to process */
          continue;
        } /* end if - all the expected bytes have been received */
      }

      /* Sub state waiting for INFORMATION bytes */
      if(uim_ptr->rxtx_state_machine.rx_t1_block_sub_state == UIM_RX_T_1_RX_INFO_ST)
      {
        /* Process error detection code for this byte */
        if (UIM_T_1_EDC_LRC == uim_ptr->t1.t_1_edc)
        {
          uim_ptr->t1.error_detection_code ^= rx_value_array[i];
        }
        else /* The EDC is CRC */
        {
          /* Compute the CRC with this byte */
          uim_ptr->t1.error_detection_code =
            crc_16_l_table[ (uim_ptr->t1.error_detection_code ^ rx_value_array[i]) & 0x00ff ]
             ^ (uim_ptr->t1.error_detection_code >> 8);
        } /* The EDC is LRC */

        /* Place the byte in the response buffer. */
        *uim_ptr->card_cmd.resp_buf_byte_ptr++ = rx_value_array[i];

        /* Keep track of the number of bytes received. */
        uim_ptr->card_cmd.num_resp_bytes_rcvd++;

        /* Determine if all the expected bytes have been received. */
        if (uim_ptr->card_cmd.num_resp_bytes_rcvd ==
            uim_ptr->card_cmd.num_resp_data_bytes_expected)
        {
          /* Set the number of bytes received in the response buffer */
          uim_ptr->card_cmd.resp_buf_ptr->cmd_rsp_size = uim_ptr->card_cmd.num_resp_bytes_rcvd;

          /* Initialize the number of bytes received */
          uim_ptr->card_cmd.num_resp_bytes_rcvd = 0;

          /* Prepare to receive the epilogue field */
          uim_ptr->rxtx_state_machine.rx_t1_block_sub_state = UIM_RX_T_1_RX_EPILOGUE_ST;

          /* Determine if CRC is used for EDC */
          if (UIM_T_1_EDC_CRC == uim_ptr->t1.t_1_edc)
          {
            /* Complement the CRC field */
            uim_ptr->t1.error_detection_code = ~uim_ptr->t1.error_detection_code;
          } /* end if - check if we are using CRC for EDC */
          continue;
        } /* end if - all the expected bytes have been received */
      }

      /* Sub state waiting for EPILOGUE bytes */
      if(uim_ptr->rxtx_state_machine.rx_t1_block_sub_state == UIM_RX_T_1_RX_EPILOGUE_ST)
      {
        /* Place the byte in the epilogue field buffer. */
        ((byte *)&uim_ptr->t1.tx_epilogue_field)[uim_ptr->card_cmd.num_resp_bytes_rcvd] =
          rx_value_array[i];

        /* Keep track of the number of bytes received. */
        uim_ptr->card_cmd.num_resp_bytes_rcvd++;

        /* Determine if all the expected bytes have been received. */
        if (uim_ptr->card_cmd.num_resp_bytes_rcvd == uim_ptr->t1.epilogue_field_size)
        {
          /* Check the error detection code */
          if (uim_ptr->t1.error_detection_code != uim_ptr->t1.tx_epilogue_field.crc)
          {
            /* Indicate APDU result */
            uim_ptr->card_cmd.resp_buf_ptr->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
          } /* end if - Check the error detection result */

          /* Change to idle */
          uim_ptr->rxtx_state_machine.rx_state = UIM_RX_PROCESS_IDLE;
          uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_PROCESS_IDLE;

          reinit_rxtransfer = FALSE;

          /* Disable RXLEV and RXSTALE */
          UIM_PRG_IMR_SLOT(uim_ptr, MSMU_IMR_RXBREAK );

          /* Reset the UART receiver only if there is a comunication error
             and that too after all the bytes are received */
          if( uim_ptr->card_cmd.resp_buf_ptr->cmd_status ==
              UIM_T_1_BLOCK_RX_FAILURE )
          {
            /* Clear the statuses in UART */
            uim_reset_receiver(uim_ptr);
          }

          /* Call the response callback function */
          uim_command_response_callback(uim_ptr);

          /* Clear command response timer */
          uim_clear_cmd_rsp_timer(uim_ptr);

        } /* end if - all the expected bytes have been received */
      }
    } /* for loop */
  } /* while loop */

  /* If end of data event occurred and still some more data expected */
  if( reinit_rxtransfer == TRUE )
  {
    reinit_rxtransfer = FALSE;

    /* This stale event might have happened while we were processing the
       RXLEV interrupt. In that case, we would like to preserve it. */
    if (uim_ptr->rxtx_state_machine.rx_sub_state == UIM_RX_RXLEV ||
        uim_ptr->rxtx_state_machine.rx_sub_state == UIM_T_1_RX_LEV_RECEIVED_WITH_PARITY_ERRORS)
    {
      uim_ptr->card_cmd.num_bytes_received = 0;
      uim_ptr->card_cmd.num_words_received = 0;
      uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;
      return;
    }

    /* Since we are going to re-init a new RX data transfer,
       initialize this variable to 0. */
    uim_ptr->card_cmd.num_bytes_received = 0;
    uim_ptr->card_cmd.num_words_received = 0;
    uim_ptr->card_cmd.total_bytes_read = 0;

    /* Re-initialize a new RX data transfer */
    UIM_DM_RX_INIT_SLOT(uim_ptr, MSMU_DEF_RX_INIT_VAL);

    /* Enable Stale event */
    UIM_ENABLE_RX_STALE_INT_SLOT(uim_ptr);

    uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_RXSTALE;

    /* start CMD RSP timer */
    uim_start_cmd_rsp_timer( (uim_ptr->card_cmd.work_waiting_time +
                              UIM_UART_DM_WAITING_TIME_CORRECTION),
                             uim_ptr);
  }
  return;
} /* uim_rx_isr_recv_t_1_block */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called while sending the information
 * field of a T=1 block.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_send_information
(
  uim_instance_global_type *uim_ptr
)
{
  unsigned char *byte_ptr = NULL;
  uint16  i = 0;

  if(uim_ptr->rxtx_state_machine.rx_sub_state != UIM_RX_TXREADY)
  {
    return;
  } /* UIM_RX_TXREADY */

  byte_ptr = uim_ptr->card_cmd.request_buffer_ptr;

  /* Keep track of EDC */
  if (UIM_T_1_EDC_LRC == uim_ptr->t1.t_1_edc)
  {
    for(i = 0; i< uim_ptr->card_cmd.num_bytes_to_send; i++)
    {
      uim_ptr->t1.tx_epilogue_field.lrc ^= byte_ptr[i];
    }
  }
  else /* The EDC is CRC */
  {
    for(i = 0; i< uim_ptr->card_cmd.num_bytes_to_send; i++)
    {
      /* Compute the CRC with this byte */
      uim_ptr->t1.tx_epilogue_field.crc =
        crc_16_l_table[ (uim_ptr->t1.tx_epilogue_field.crc ^
                         byte_ptr[i]) & 0x00ff ]
         ^ (uim_ptr->t1.tx_epilogue_field.crc >> 8);
    }
  } /* end if - the EDC is LRC */

  /* Program the NO_CHARS_FOR_TX register with the size of TX bytes
     to be sent */
  UIM_NUM_CHARS_TO_SEND_SLOT(uim_ptr, uim_ptr->card_cmd.num_bytes_to_send);

  /* Write the TX bytes into the TX FIFO */
  uim_tx(uim_ptr->card_cmd.request_buffer_ptr,
                    uim_ptr->card_cmd.num_bytes_to_send,
                    uim_ptr);
  uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_TXREADY;
  uim_ptr->card_cmd.num_bytes_to_send = 0;

  /* Determine if this is the last byte for this buffer */
  if (uim_ptr->card_cmd.num_bytes_to_send == 0)
  {
    /* Send the epilogue field */
    uim_ptr->card_cmd.num_bytes_to_send = uim_ptr->t1.epilogue_field_size;
    /* Determine if CRC is used for EDC */
    if (UIM_T_1_EDC_CRC == uim_ptr->t1.t_1_edc)
    {
      /* Complement the CRC field */
      uim_ptr->t1.tx_epilogue_field.crc = ~uim_ptr->t1.tx_epilogue_field.crc;
    } /* end if - check if we are using CRC for EDC */
    uim_ptr->card_cmd.request_buffer_ptr = (byte *)&uim_ptr->t1.tx_epilogue_field;
   uim_ptr->rxtx_state_machine.rx_state = UIM_RX_T_1_SEND_EPILOGUE_ST;
    /* Exit the loop */
    return;
  }
} /* uim_rx_isr_send_information */


/**
 * This function runs within the context of the Rx ISR of the
 * UART. This function is called while sending the epilogue
 * field of a T=1 block.
 *
 * @param uim_ptr Pointer to the UIM instance specific globals.
 */
void uim_rx_isr_send_epilogue
(
  uim_instance_global_type *uim_ptr
)
{
  /* Initialize the RXLEV count for every APDU transaction */
  uim_ptr->debug.rxlev_count = 0;

  /* Reset the RX byte count to zero */
  uim_ptr->card_cmd.num_bytes_received = 0;
  uim_ptr->card_cmd.num_words_received = 0;

  if(uim_ptr->rxtx_state_machine.rx_sub_state != UIM_RX_TXREADY)
  {
    return;
  } /* UIM_RX_TXREADY */

  /* Initialize the a new RX data transfer of size MSMU_DEF_RX_INIT_VAL */
  UIM_DM_RX_INIT_SLOT(uim_ptr, MSMU_DEF_RX_INIT_VAL);

  /* Program the NO_CHARS_FOR_TX register with the size of TX bytes
     to be sent */
  UIM_NUM_CHARS_TO_SEND_SLOT(uim_ptr, uim_ptr->card_cmd.num_bytes_to_send);

  /* Write the TX bytes into the TX FIFO */
  uim_tx(uim_ptr->card_cmd.request_buffer_ptr,
                    uim_ptr->card_cmd.num_bytes_to_send,
                    uim_ptr);

  uim_ptr->card_cmd.num_bytes_to_send = 0;

  /* Determine if this is the last byte for this buffer */
  if (uim_ptr->card_cmd.num_bytes_to_send == 0)
  {
    /* Program the RX water mark level */
    UIM_RX_WATERLEVEL_SLOT(uim_ptr, MSMU_DEF_RFWR_VAL );

    /* Program the stale timeout value */
    UIM_PRG_IPR_SLOT(uim_ptr, MSMU_IPR_DEF_STALE_TIMEOUT);

    /* Enable the receive interrupts - stale character timeout,
       receive break condition, parity frame error , TX complete IRQ, WWT IRQ */
    UIM_PRG_IMR_SLOT(uim_ptr, ( MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK | MSMU_IMR_RXLEV | \
                    MSMU_IMR_TX_COMP | MSMU_IMR_WWT) );

    /* Enable Stale event */
    UIM_ENABLE_RX_STALE_INT_SLOT(uim_ptr);

    /* Receive the prologue field from the card */
    uim_ptr->card_cmd.num_resp_bytes_rcvd = 0;
    uim_ptr->rxtx_state_machine.rx_state = UIM_RX_T_1_RECV_T_1_BLOCK_DM;

    /* Wait for TXCOMP (transfer completion) event to make sure the APDU
       command bytes have been seen successfully */
    uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_WAIT_FOR_TXCOMP;

    /* Initialize the EDC fields */
    if (UIM_T_1_EDC_LRC == uim_ptr->t1.t_1_edc)
    {
      uim_ptr->t1.error_detection_code = 0;
      /* Initialize the crc field as it initializes both bytes of the EDC
         field including the lrc field */
      uim_ptr->t1.tx_epilogue_field.crc = 0;
    }
    else /* The EDC is CRC */
    {
      uim_ptr->t1.error_detection_code = CRC_16_L_SEED;
      uim_ptr->t1.tx_epilogue_field.crc = 0;
    } /* The EDC is LRC */
    /* Exit this loop */
    return;
  }
} /* uim_rx_isr_send_epilogue */
#endif /* FEATURE_UIM_T_1_SUPPORT */
