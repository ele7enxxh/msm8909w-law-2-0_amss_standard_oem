/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


       U I M   T = 1   T R A N S P O R T   L A Y E R   P R O T O C O L

GENERAL DESCRIPTION
  This module contains the procedures for the T=1 transport layer processing.
  The module handles the commands sent to the T=1 UIM card and also handles the
  responses from the card.  This module handles the transport layer for T=1
  including supervisory and receive ready blocks.  The resultant APDUs are
  passed top the UIM server for processing as with the T=0 protocol.

EXTERNALIZED FUNCTIONS

  uim_process_t_1_command
    Processes a UIM server command for the T=1 protocol

  uim_process_t_1_response
    Processes the UIM card responses for the T=1 protocol

  uim_process_t_1_timeout
    Processes a T=1 transport layer timeout

Copyright (c) 1999-2010, 2012-2014 by Qualcomm Technologies, Incorporated.  All Rights
Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimt1.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/15   ll      UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
09/18/14   akv     UIMDRV UT enhancements
04/16/14   akv     Fix to enable UART TX/RX while handling WWT timeouts
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
01/29/14   sam     Usage of newer style diag macros
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
06/24/13   js      TSTS changes
12/17/12   js      Fixed KW error
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (without features)
09/25/12   rm      WWT fixes over GCF test failures
07/25/12   nmb     End processing once SW 62 83 is encountered
03/20/12   rm      Changes for WWT
03/15/12   rm      Changes for DIME
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/22/11   rm      Fix to interpret 92 XX as successful response
12/03/10   ps      Fix compilation issue due to mem reduction changesk
10/23/10   ps      Adding workaround for unexpected data on FIFO
11/10/10   yt      Memory reduction changes
09/10/10   ps      Merge UIM server DSDS changes
09/06/10   yb      Fixed Lint error
07/18/10   ps      Fix to high Lint errors
01/18/10   rm      Fixes to delay sending the R-block
10/07/09   yb      Klockwork fix
09/26/09   kp      ZI memory reduction changes
09/16/09   yb      Klockwork FIX
09/16/09   yb      Klockwork FIX
05/02/09   ps      Fix 8200 Warnings
04/17/09   vs      Fixed featurization
10/06/08   rm      Cached select support for T=1 protocol
06/28/06   pv      Change sequence or start receiving fresh data upon
                   reception of abort.
06/05/06   pv      Change the number of retries to 2 instead of 3.
                   Allow only 2 retries for IFS request, if failed, reset
                   the interface 7816 - scenario 33, rule 7.1 and 7.4.1.
                   Initialize result to NULL.
                   Re-send the I-Block after a successful re-synch response.
                   Do appropriate validations for received s-block.
04/03/06   pv      Lint Fixes
11/14/05   pv      Lint Fixes
05/10/05   sst     Lint fixes
02/02/05   pv      Changed !=1 to <= 1 in is_t_1_block_S-Block could be of length
                   0 or 1.  Added additional condition to check uim_t_1_error_count < 2
                   before re-sending the I-Block.  If already got two garbage packets
                   re-sending the R block could be the way to get out.
01/28/04   pv      reset the uim_t_1_error_count variable each time we get
                   an acknowledge for the I-BLOCK sent
11/15/04   pv      Added is_t_1_block_received_valid function to check for validity of
                   the received I_Block, saved the length of last transmitted I_Block,
                   made uim_get_rx_block_type also return the NAD byte, check the
                   sequence number of received I-block and add the saved length of
                   last I_block sent.
02/20/04   ts      Fixed lint errors
03-07-16   tjw     Prevent compiler warning C3040E: no external declaration in translation unit
02/24/03   ts      Created T=1 module

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
/*lint -e766
   Inhibit lint warning for target.h not being used when T=1 support is not
   enabled
*/

#ifdef FEATURE_UIM_T_1_SUPPORT
#include "customer.h"
#include "uimdrv_msg.h"
#include "err.h"
#include "task.h"
#include "assert.h"
#include "uimt1.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimgen.h"
#include "uimglobals.h"
#include "uimdrv_uartdm.h"

/*===========================================================================

                Lint message inhibitions

===========================================================================*/
/*lint -esym(552, uim_apdu_req_buffer_ptr)
  uim_apdu_req_buffer_ptr is accessed all over in UIM.
*/
/*lint -esym(552, uim_apdu_request_bytes_to_send)
  uim_apdu_request_bytes_to_send is accessed all over UIM
*/
/*lint -esym(552, uim_t_1_apdu_req_buf_ptr)
  uim_t_1_apdu_req_buf_ptr is accessed all over UIM.
*/
/*lint -esym(552, uim_t_1_ifsc)
  uim_t_1_ifsc is accessed all over UIM.
*/
/*lint -esym(552, uim_t_1_ifsd)
  uim_t_1_ifsd is accessed all over UIM.
*/
/*lint -esym(552, uim_t_1_wtx)
  uim_t_1_wtx is accessed all over UIM.
*/


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
  I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
static void uim_t_1_resynch_w_card
(
  uim_instance_global_type *uim_ptr
);

static void uim_t_1_resend_s_block
(
  uim_instance_global_type *uim_ptr
);

static void uim_t_1_process_s_block
(
  uim_rsp_buf_type         *rsp_ptr, /* points to the response buffer */
  rex_sigs_type            *i_mask,  /* Mask of signals interested in */
  /* Contains the parameters of the received block */
  uim_block_params_type    uim_block_params,
  uim_instance_global_type *uim_ptr
);

static void uim_t_1_process_bad_block
(
  uim_instance_global_type *uim_ptr
);

static void uim_process_t_1_command_response
(
  uim_rsp_buf_type         *rsp_ptr, /* points to the response buffer */
  rex_sigs_type            *i_mask,   /* Mask of signals interested in */
  uim_instance_global_type *uim_ptr
);

static boolean is_t_1_block_received_valid
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
);

static uim_block_params_type uim_get_rx_block_type
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_GET_RX_BLOCK_TYPE

DESCRIPTION
  This function parses the received T=1 block.

DEPENDENCIES
  This function processes the prologue field saved by the driver in the
  global variable.

RETURN VALUE
  This function returns the parsed information of the block.

SIDE EFFECTS
  None.
===========================================================================*/
static uim_block_params_type uim_get_rx_block_type
(
  uim_instance_global_type *uim_ptr
)
{
  /* Holds return value */
  uim_block_params_type uim_block_params;

  /* Save the node address byte */
  uim_block_params.node_address_byte = uim_ptr->t1.rx_prologue_field_buffer.nad;
  /* -------------------------------------------------- */
  /* Determine if the block type is R or S */
  if (uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_PCB_R_BLOCK)
  {
    /* Determine if the block type is S */
    if ((uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_PCB_S_BLOCK) ==
         UIM_T_1_PCB_S_BLOCK)
    {
      /* The block type is S */
      uim_block_params.block_type = UIM_T_1_BLOCK_TYPE_S;
      uim_block_params.block_params.s_block =
        (uim_s_block_type)
        (uim_ptr->t1.rx_prologue_field_buffer.pcb & ~UIM_T_1_PCB_S_BLOCK);
    }
    else /* The block type is R */
    {
      uim_block_params.block_type = UIM_T_1_BLOCK_TYPE_R;
      uim_block_params.block_params.r_block.error_code =
         uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_PCB_ERR_CODE_MASK;
      /* Check the send sequence number */
      if (uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_PCB_NEXT_SEQ_NUM)
      {
        uim_block_params.block_params.r_block.send_seq_num = 1;
      }
      else /* the send seq number is cleared */
      {
        uim_block_params.block_params.r_block.send_seq_num = 0;
      } /* end if - the send seq number is set */
    } /* end if - the block type is S */
  }
  else /* The block type is I */
  {
    uim_block_params.block_type = UIM_T_1_BLOCK_TYPE_I;
    /* Check the send sequence number */
    if (uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_PCB_SEND_SEQ_NUM)
    {
      uim_block_params.block_params.i_block.send_seq_num = 1;
    }
    else /* the send seq number is cleared */
    {
      uim_block_params.block_params.i_block.send_seq_num = 0;
    } /* end if - the send seq number is set */

    /* Check the more bit */
    if (uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_PCB_MORE_DATA_BIT)
    {
      uim_block_params.block_params.i_block.more_bit = 1;
    }
    else /* the send seq number is cleared */
    {
      uim_block_params.block_params.i_block.more_bit = 0;
    } /* end if - the send seq number is set */
  } /* end if - The block type is R or S */

  return (uim_block_params);
} /* uim_get_rx_block_type */


/*===========================================================================

FUNCTION UIM_T_1_RESYNCH_W_CARD

DESCRIPTION
  This function sends a resynch S-block to the card.

DEPENDENCIES
  This function must fail the current command (if there is one) because
  the transaction has not been completed.

RETURN VALUE
  None.

SIDE EFFECTS
  The UIM driver will send out the S-block and set the T=1 protocol state
  to expect the response.
===========================================================================*/
static void uim_t_1_resynch_w_card
(
  uim_instance_global_type *uim_ptr
)
{
  UIMDRV_MSG_LOW_0(uim_ptr->id, "UIM T=1 sending a Resynch" );

  /* Set the S-block parameters for resynch */
  uim_ptr->t1.t_1_s_block_code = UIM_T_1_PCB_RESYNC_REQ;
  uim_ptr->t1.t_1_s_block_data_size = 0;
  /* Set the T=1 protocol state for sending an S-block request */
  uim_ptr->t1.t_1_protocol_state = UIM_T_1_SENT_SUPERVISORY_BLOCK_ST;

  /* Send the resynch block*/
  uim_t_1_send_s_block (uim_ptr->t1.t_1_node_address,
                        uim_ptr->t1.t_1_s_block_code,
                        &uim_ptr->t1.t_1_s_block_data,
                        uim_ptr->t1.t_1_s_block_data_size,
                        uim_ptr);
} /* uim_t_1_resynch_w_card */


/*===========================================================================

FUNCTION UIM_T_1_RESEND_S_BLOCK

DESCRIPTION
  This function keeps track of the number of S-block re-sends.  After a
  maximum number of re-sends, it fails the command and causes the link to be
  reset.  Otherwise, it re-sends the S-block.

DEPENDENCIES
  This function processes this condition as an error for this APDU exchange.
  The last S-block is re-sent to try to rectify the problem.

RETURN VALUE
  None.

SIDE EFFECTS
  The UIM driver will send out the S-block or fail the command.  A response
  from the card is expected if the S-block is sent.
===========================================================================*/
static void uim_t_1_resend_s_block
(
  uim_instance_global_type *uim_ptr
)
{
  UIMDRV_MSG_LOW_0(uim_ptr->id, "UIM T=1 re-sending an S-block" );
  /* Process received block with errors */
  if (++uim_ptr->t1.t_1_error_count > UIM_MAX_BAD_BLOCK_RETRIES)
  {
    /* Determine if resynch was last sent */
    if (UIM_T_1_PCB_RESYNC_REQ == uim_ptr->t1.t_1_s_block_code ||
        UIM_T_1_PCB_IFS_REQ    == uim_ptr->t1.t_1_s_block_code )
    {
      /* An error count greater than UIM_MAX_BAD_BLOCK_RETRIES indicates the
         card needs to be reset */
      /* Fail the request through the time-out mechanism */
      /* Set the command response timeout signal */
      (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
      return;
    }
    else /* continue processing the S-block */
    {
      /* Re-synchronize with the card */
      uim_t_1_resynch_w_card (uim_ptr);
      /* Reset the error count */
      uim_ptr->t1.t_1_error_count = 0;
    } /* end if - resynch request in progress */
  }
  else /* More attempts needed */
  {
    /* Re-send the S-block */
    uim_t_1_send_s_block (uim_ptr->t1.t_1_node_address,
                          uim_ptr->t1.t_1_s_block_code,
                          &uim_ptr->t1.t_1_s_block_data,
                          uim_ptr->t1.t_1_s_block_data_size,
                          uim_ptr);
  } /* end if - max errors received */
} /* uim_t_1_resend_s_block */


/*===========================================================================

FUNCTION UIM_T_1_PROCESS_S_BLOCK

DESCRIPTION
  This function processes an S-block received from the card.

DEPENDENCIES
  This function processes the S-block based upon global variables that define
  the current state of the T=1 protocol.

RETURN VALUE
  None.

SIDE EFFECTS
  This function may change the state of the T=1 protocol.  The current command
  may fail.
===========================================================================*/
static void uim_t_1_process_s_block
(
  uim_rsp_buf_type          *rsp_ptr, /* points to the response buffer */
  rex_sigs_type             *i_mask,  /* Mask of signals interested in */
  /* Contains the parameters of the received block */
  uim_block_params_type     uim_block_params,
  uim_instance_global_type  *uim_ptr
)
{
  UIMDRV_MSG_LOW_0(uim_ptr->id, "UIM T=1 processing an S-block" );
  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_t_1_process_s_block rsp_ptr is NULL",0,0,0);
  }

  /* Determine if the protocol is waiting for a response */
  if (UIM_T_1_SENT_SUPERVISORY_BLOCK_ST == uim_ptr->t1.t_1_protocol_state)
  {
    /* Make sure this S-block is the response to the S-block sent */
    if ((int)uim_block_params.block_params.s_block ==
        (uim_ptr->t1.t_1_s_block_code | UIM_T_1_PCB_S_RSP))
    {
      /* Process the S-block response */
      switch (uim_block_params.block_params.s_block)
      {
        case UIM_S_BLOCK_RESYNCH_RESPONSE:
          {
            /* Reset the sequence numbers */
            uim_ptr->t1.me_send_sequence_num = UIM_T_1_INIT_SEQ_NUM;
            uim_ptr->t1.card_send_sequence_num = UIM_T_1_INIT_SEQ_NUM;

            /* Send out the I-block again */
            uim_t_1_send_i_block (uim_ptr->t1.t_1_node_address, uim_ptr);
          } /* end case - UIM_S_BLOCK_RESYNCH_RESPONSE */
          break;

        case UIM_S_BLOCK_IFS_RESPONSE:
          {
            /* Set the new IFS parameters, if valid */
            if ((rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes] >= UIM_T_1_MIN_IFS) &&
                (rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes] <= UIM_T_1_MAX_IFS))
            {
              uim_ptr->t1.t_1_ifsd = rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes];
            }

            /* Set the number of bytes received for IFS. */
            rsp_ptr->cmd_rsp_size = 1;

            /* Give a bad status. */
            rsp_ptr->cmd_status = UIM_DONE;

            /* Call the response callback function */
            uim_process_card_response(i_mask, uim_ptr);
          } /* end case - UIM_S_BLOCK_IFS_RESPONSE*/
          break;

        case UIM_S_BLOCK_ABORT_RESPONSE:
          {
            /* Fail the current command */
            /* Set the number of bytes received to zero. */
            rsp_ptr->cmd_rsp_size = 0;

            /* Give a bad status. */
            rsp_ptr->cmd_status = UIM_PROBLEM;

            /* Call the response callback function */
            uim_process_card_response(i_mask, uim_ptr);
          } /* end case - UIM_S_BLOCK_ABORT_RESPONSE */
          break;

        default:
          {
            /* Set the number of bytes received to 0. */
            rsp_ptr->cmd_rsp_size = 0;

            /* Give a bad status. */
            rsp_ptr->cmd_status = UIM_DONE;

            /* Call the response callback function */
            uim_process_card_response(i_mask, uim_ptr);
          } /* end case - default */
          break;
      } /* end switch - S-block type */
      return;
    }
    else /* The response does not match the request */
    {
      /* Re-send the S-block */
      uim_t_1_resend_s_block(uim_ptr);
    } /* end if - determine if the response matches the request */
  }
  else /* not waiting for an S-block response */
  {
    /* Process the S-block request */
    switch (uim_block_params.block_params.s_block)
    {
      case UIM_S_BLOCK_RESYNCH_REQUEST:
        {
          /* Reset the sequence numbers */
          uim_ptr->t1.me_send_sequence_num = 0;
          uim_ptr->t1.card_send_sequence_num = 0;
          /* Respond with a resynch response */
          /* Set the S-block parameters for resynch */
          uim_ptr->t1.t_1_s_block_code = UIM_T_1_PCB_RESYNC_RSP;
          uim_ptr->t1.t_1_s_block_data_size = 0;

          /* Send the resynch response block*/
          uim_t_1_send_s_block (uim_ptr->t1.t_1_node_address,
                                uim_ptr->t1.t_1_s_block_code,
                                &uim_ptr->t1.t_1_s_block_data,
                                uim_ptr->t1.t_1_s_block_data_size,
                                uim_ptr);
          /* Set the protocol state to reflect a failed command */
          uim_ptr->t1.t_1_protocol_state = UIM_T_1_FAILED_CMD_ST;
        } /* end case - UIM_S_BLOCK_RESYNCH_REQUEST */
        break;

      case UIM_S_BLOCK_IFS_REQUEST:
        {
          /* Set the new IFS parameters, if valid */
          if ((rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes] >= UIM_T_1_MIN_IFS) &&
              (rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes] <= UIM_T_1_MAX_IFS))
          {
            uim_ptr->t1.t_1_ifsc = rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes];
          }

          /* Respond with an IFS response block */
          /* Set the S-block parameters for IFS */
          uim_ptr->t1.t_1_s_block_code = UIM_T_1_PCB_IFS_RSP;
          uim_ptr->t1.t_1_s_block_data = rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes];
          uim_ptr->t1.t_1_s_block_data_size = 1;

          /* Send the IFS response block*/
          uim_t_1_send_s_block (uim_ptr->t1.t_1_node_address,
                                uim_ptr->t1.t_1_s_block_code,
                                &uim_ptr->t1.t_1_s_block_data,
                                uim_ptr->t1.t_1_s_block_data_size,
                                uim_ptr);
        } /* end case - UIM_S_BLOCK_IFS_REQUEST */
        break;

      case UIM_S_BLOCK_ABORT_REQUEST:
        {
          /* Respond with an ABORT response */
          /* Set the S-block parameters for ABORT */
          uim_ptr->t1.t_1_s_block_code = UIM_T_1_PCB_ABORT_RSP;
          uim_ptr->t1.t_1_s_block_data_size = 0;

          /* Send the ABORT response block*/
          uim_t_1_send_s_block (uim_ptr->t1.t_1_node_address,
                                uim_ptr->t1.t_1_s_block_code,
                                &uim_ptr->t1.t_1_s_block_data,
                                uim_ptr->t1.t_1_s_block_data_size,
                                uim_ptr);

          if ( uim_ptr->t1.t_1_protocol_state == UIM_T_1_SENT_CHAINED_APDU_ST )
          {
            /* If we are sending a chained block, we need to
             * start by sending from the beginning of the chained
             * block change the sequence number and wait for the r-block */
            uim_ptr->t1.me_send_sequence_num ^= uim_ptr->t1.t_1_changed_me_send_seq_number;

            /* Set the protocol state to reflect a failed command this
             * would end-up re-transmitting the entire request
             */
            uim_ptr->t1.t_1_protocol_state = UIM_T_1_FAILED_CMD_ST;
          }
          else if ( uim_ptr->t1.t_1_receiving_chained_block )
          {
            /* If we are receiving a chain, we need to abort the
             * current reception and start receiving fresh
             * chained block
             */
            uim_ptr->t1.t_1_changed_me_send_seq_number = 0;

            /* Reset the response buffer to the initial values */
            uim_ptr->t1.apdu_rsp_buffer_ptr = uim_ptr->t1.back_up_uim_apdu_rsp_buffer_ptr;

            uim_ptr->t1.apdu_rsp_num_bytes = 0;
          }
          else
          {
            /* Set the protocol state to reflect a failed command */
            uim_ptr->t1.t_1_protocol_state = UIM_T_1_FAILED_CMD_ST;
          }
        } /* end case - UIM_S_BLOCK_ABORT_REQUEST */
        break;

      case UIM_S_BLOCK_WTX_REQUEST:
        {
          /* Save the block waiting time extension */
          uim_ptr->t1.t_1_wtx = rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes];

          /* Respond with an IFS response */
          /* Set the S-block parameters for IFS */
          uim_ptr->t1.t_1_s_block_code = UIM_T_1_PCB_WTX_RSP;
          uim_ptr->t1.t_1_s_block_data = rsp_ptr->rsp.data[uim_ptr->t1.apdu_rsp_num_bytes];
          uim_ptr->t1.t_1_s_block_data_size = 1;

          /* Send the WTX response block*/
          uim_t_1_send_s_block (uim_ptr->t1.t_1_node_address,
                                uim_ptr->t1.t_1_s_block_code,
                                &uim_ptr->t1.t_1_s_block_data,
                                uim_ptr->t1.t_1_s_block_data_size,
                                uim_ptr);
        } /* end case - UIM_S_BLOCK_WTX_REQUEST */
        break;

      default:
        {
          /* Unexpected S-block */
          /* Re-synchronize with the card */
          uim_t_1_resynch_w_card (uim_ptr);
        } /* end case - default */
        break;
    } /* end switch - determine S-block request */
  } /* end if - waiting for an S-block response */
} /* uim_t_1_process_s_block */


/*===========================================================================

FUNCTION UIM_T_1_PROCESS_BAD_BLOCK

DESCRIPTION
  This function processes the reception of a bad T=1 block.  There is either
  something wrong with the format of the block or a block was received
  unexpectedly.

DEPENDENCIES
  This function processes this condition as an error for this APDU exchange.
  An R-block is sent to try to rectify the problem.

RETURN VALUE
  None.

SIDE EFFECTS
  The UIM driver will send out the R-block.  A response from the card is
  expected.
===========================================================================*/
static void uim_t_1_process_bad_block
(
  uim_instance_global_type *uim_ptr
)
{
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "UIM T=1 processing a bad block" );
  /* Process received block with errors */
  if (++uim_ptr->t1.t_1_error_count > UIM_MAX_BAD_BLOCK_RETRIES)
  {
    /* Re-synchronize with the card */
    uim_t_1_resynch_w_card (uim_ptr);
    /* Reset the error count */
    uim_ptr->t1.t_1_error_count = 0;
  }
  else /* More attempts are needed */
  {
    /* Send an R-block */
    uim_t_1_send_r_block (uim_ptr->t1.t_1_node_address,
                          UIM_T_1_PCB_EDC_ERROR,
                          uim_ptr);
  } /* end if - max errors received */
} /* uim_t_1_process_bad_block */


/*===========================================================================

FUNCTION UIM_PROCESS_T_1_COMMAND

DESCRIPTION
  This function processes a request to send an APDU to the UIM card while
  operating in T=1 transport protocol.

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.  This
  function uses global variables to set up the UIM driver and server transfers.

RETURN VALUE
  None.

SIDE EFFECTS
  The UIM driver will continue to send out the APDU.  All the stages of
  sending the APDU are handled by the UIM driver.  Then the driver expects
  the T=1 block response.  Additional blocks are sent out when processing
  the block responses from the card.

===========================================================================*/
void uim_process_t_1_command
(
  uim_req_buf_type *uim_req, /* This points to the APDU command buffer, the
                               command data buffer, the response buffer, the
                               APDU status and the callback function.      */
  uim_instance_global_type *uim_ptr
)
{
  UIMDRV_MSG_LOW_0(uim_ptr->id, "uim_process_t_1_command" );
  if (uim_req == NULL)
  {
    ERR_FATAL("uim_process_t_1_command uim_req is NULL",0,0,0);
  }

  if(uim_ptr->id >= UIM_INSTANCE_INVALID)
  {
    UIM_MSG_ERR_1("Invalid input param instance id 0x%x", uim_ptr->id);
    return;
  }

  /* Save the APDU buffer pointer */
  uim_ptr->t1.t_1_apdu_req_buf_ptr = uim_req;
  uim_ptr->t1.t_1_apdu_resp_buf = uim_req->rsp_ptr;

  /* Initialize the APDU request buffer pointer */
  uim_ptr->t1.apdu_req_buffer_ptr = (byte *)&uim_req->apdu_hdr;

  /* Initialize the APDU response buffer pointer and response size */
  uim_ptr->t1.apdu_rsp_buffer_ptr = (byte *)&uim_req->rsp_ptr->rsp;

  /* back-up the initial response buffer pointer */
  uim_ptr->t1.back_up_uim_apdu_rsp_buffer_ptr = uim_ptr->t1.apdu_rsp_buffer_ptr;

  uim_ptr->t1.apdu_rsp_num_bytes = 0;

  /* Initialize the transaction error count */
  uim_ptr->t1.t_1_error_count = 0;

  /* Determine the case of the instruction */
  switch (uim_req->instrn_case)
  {
    case UIM_INSTRN_CASE_1:
      {
        /* Send out only four bytes of header - P3 is not sent */
        uim_ptr->t1.apdu_request_bytes_to_send = NUM_BYTES_APDU_HDR - 1;
      } /* end case - UIM_INSTRN_CASE_1 */
      break;

    case UIM_INSTRN_CASE_2:
      {
        /* Send only the APDU */
        uim_ptr->t1.apdu_request_bytes_to_send = NUM_BYTES_APDU_HDR;
      } /* end case - UIM_INSTRN_CASE_2 */
      break;

    case UIM_INSTRN_CASE_3:
      {
        /* Send the APDU and the data */
        uim_ptr->t1.apdu_request_bytes_to_send = NUM_BYTES_APDU_HDR;
        uim_ptr->t1.apdu_request_bytes_to_send += uim_req->apdu_hdr.p3;
      } /* end case - UIM_INSTRN_CASE_3 */
      break;

    case UIM_INSTRN_CASE_4:
      {
        /* Send the APDU and the data */
        uim_ptr->t1.apdu_request_bytes_to_send = NUM_BYTES_APDU_HDR;
        uim_ptr->t1.apdu_request_bytes_to_send += uim_req->apdu_hdr.p3;
        /* Set the last byte in the data field to 0 to indicate
           the maximum size expected in the response */
        if (uim_req->apdu_hdr.p3 < UIM_MAX_TRANS_SIZE_OUT)
        {
          uim_req->req.data[uim_req->apdu_hdr.p3] = 0;
        }
        uim_ptr->t1.apdu_request_bytes_to_send++;
      } /* end case - UIM_INSTRN_CASE_4 */
      break;

    case UIM_INSTRN_IFS_BLOCK:
      {
        /* Set the S-block parameters for resynch */
        uim_ptr->t1.t_1_s_block_code = UIM_T_1_PCB_IFS_REQ;
        uim_ptr->t1.t_1_s_block_data_size = 1;
        uim_ptr->t1.t_1_s_block_data = uim_req->req.data[0];

        /* Set the T=1 protocol state for sending an S-block request */
        uim_ptr->t1.t_1_protocol_state = UIM_T_1_SENT_SUPERVISORY_BLOCK_ST;

        /* Send the resynch block*/
        uim_t_1_send_s_block (uim_ptr->t1.t_1_node_address,
                              uim_ptr->t1.t_1_s_block_code,
                              &uim_ptr->t1.t_1_s_block_data,
                              uim_ptr->t1.t_1_s_block_data_size,
                              uim_ptr);
        /* exit routine now */
        return;
      } /* end case - UIM_INSTRN_CASE_4 */
      //break;

    default:
      {
        /* Bad parameter from the command */
        UIMDRV_MSG_ERR_1(uim_ptr->id, "Received a bad instrn case from instn 0x%x",
                         uim_req->apdu_hdr.instrn);
        /* Emulate an error for this command */
        uim_ptr->t1.t_1_apdu_resp_buf->rsp.data[0] = (int)SW1_PROBLEM;
        uim_ptr->t1.t_1_apdu_resp_buf->rsp.data[1] = (int)SW1_PROBLEM;
        uim_command_response_callback(uim_ptr);
        return;
      } /* end case - default */
      //break;
  }
  /* The transmitted I-block has yet to be been acknowledged.  Change the
     sequence number for the next received I-block */
  uim_ptr->t1.t_1_changed_me_send_seq_number = 1;
  /* Send the first block of this APDU */
  uim_t_1_send_i_block (uim_ptr->t1.t_1_node_address, uim_ptr);
} /* uim_process_t_1_command */



/*===========================================================================

FUNCTION UIM_PROCESS_T_1_COMMAND_RESPONSE

DESCRIPTION
  This function processes the APDU received from the card for the T=1
  transport protocol.  This function re-formats the response for the UIM
  server response processing.

DEPENDENCIES
  This function affects the global buffers passed in the parameter list to
  respond to the pending UIM command.

RETURN VALUE
  None.

SIDE EFFECTS
  The response is sent back to the originator.

===========================================================================*/
static void uim_process_t_1_command_response
(
  uim_rsp_buf_type         *rsp_ptr, /* points to the response buffer */
  rex_sigs_type            *i_mask,  /* Mask of signals interested in */
  uim_instance_global_type *uim_ptr
)
{
  word response_buffer_index;

  /* -------------------------------------------------------------- */

  UIMDRV_MSG_LOW_0(uim_ptr->id, "UIM process t=1 response" );

  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_process_t_1_command_response rsp_ptr is NULL",0,0,0);
  }
  /* number of bytes in response apdu should less than max and larger than minimum size 2*/
  if(uim_ptr->t1.apdu_rsp_num_bytes > UIM_MAX_CHARS || uim_ptr->t1.apdu_rsp_num_bytes < 2)
  {
    UIMDRV_MSG_ERR_1(uim_ptr->id,"Invalid number of bytes in response apdu num_bytes = 0x%x",
                     uim_ptr->t1.apdu_rsp_num_bytes);
    /* Clear the global command pointer. */
    uim_ptr->command.cmd_ptr = NULL;
    return;
  }
  /* Process the status words from the response */
  /* The status words are the last two bytes of the response */
  rsp_ptr->cmd_rsp_size = uim_ptr->t1.apdu_rsp_num_bytes - 2;
  response_buffer_index = uim_ptr->t1.apdu_rsp_num_bytes - 2;
  rsp_ptr->sw1 = rsp_ptr->rsp.data[response_buffer_index];
  response_buffer_index++;
  rsp_ptr->sw2 = rsp_ptr->rsp.data[response_buffer_index];

  /* Set the command status based upon the status words */
  /* Determine what the first status word and process accordingly */
  switch (rsp_ptr->sw1)
  {
    case SW1_NORMAL_END:    /* Normal ending of the command */
      {
        /* Make sure the second status word is proper */
        if (rsp_ptr->sw2 == (int)SW2_NORMAL_END)
        {
          /* Indicate APDU result */
          rsp_ptr->cmd_status = UIM_DONE;
        }
        else /* The status word is not proper */
        {
          /* Set the number of bytes received to zero. */
          rsp_ptr->cmd_rsp_size = 0;

          /* Give a bad status. */
          rsp_ptr->cmd_status = UIM_PROBLEM;
        } /* end if - the second status word is proper */
      } /* end case - SW1_NORMAL_END */
      break;

    case SW1_END_FETCH:     /* Normal ending of the command with extra
                               command                            */
      {
        /* Indicate APDU result */
        rsp_ptr->cmd_status = UIM_DONE;
      } /* end case - SW1_END_FETCH */
      break;

    case SW1_END_RESP:      /* Normal ending of the command with a rsp */
      {
        /* The status word is not expected.  Fail the command */
        rsp_ptr->cmd_status = UIM_PROBLEM;
        UIMDRV_MSG_ERR_0(uim_ptr->id,
                         "Response procedure byte received in T=1 protocol");
      } /* end case - SW1_END_RESP */
      break;

    case SW1_BUSY:          /* SIM App toolkit is busy */
      {
        /* Make sure the second status word is proper */
        if (rsp_ptr->sw2 == (int)SW2_NORMAL_END)
        {
          /* Indicate APDU result */
          rsp_ptr->cmd_status = UIM_TOOLKIT_BUSY;

          /* Set the number of bytes received to zero. */
          rsp_ptr->cmd_rsp_size = 0;
        }
        else /* The status word is not proper */
        {
          /* Set the number of bytes received to zero. */
          rsp_ptr->cmd_rsp_size = 0;

          /* Give a bad status. */
          rsp_ptr->cmd_status = UIM_PROBLEM;
        } /* end if - the second status word is proper */
      } /* end case - SW1_BUSY */
      break;

    case SW1_END_RETRY:     /* Command successful with update retries or
                               memory error                       */
      {
        /* Determine if it is a memory problem */
        if (rsp_ptr->sw2 == (int)SW2_MEM_PROBLEM)
        {
          /* Set the number of bytes received to zero. */
          rsp_ptr->cmd_rsp_size = 0;

          /* Indicate APDU result */
          rsp_ptr->cmd_status = UIM_MEMORY_PROBLEM;
        }
        else /* successful after retries */
        {
          /* Place the number of bytes read in the response buffer */
          rsp_ptr->cmd_rsp_size = uim_ptr->t1.apdu_rsp_num_bytes;

          /* Indicate APDU result */
          rsp_ptr->cmd_status = UIM_DONE;
        } /* end if - the second status word indicates a memory problem */
      } /* end case - SW1_END_RETRY */
      break;

    case SW1_REFERENCE:     /* A reference management problem */
      {
        /* Process the second status word */
        switch (rsp_ptr->sw2)
        {
          case SW2_NO_EF:           /* No EF selected */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_NO_EF_SELECTED;
            } /* end case - SW2_NO_EF */
            break;

          case SW2_OUT_OF_RANGE:    /* Out of range (invalid address) */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_OUT_OF_RANGE;
            } /* end case - SW2_OUT_OF_RANGE */
            break;

          case SW2_NOT_FOUND:       /* File ID/pattern not found */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_NOT_FOUND;
            } /* end case - SW2_NOT_FOUND */
            break;

          case SW2_INCONSISTENT:    /* File inconsistent with command */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_INCONSISTENT_WITH_COMMAND;
            } /* end case - SW2_INCONSISTENT */
            break;

          default:
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_REFERENCE */
      break;

    case SW1_SECURITY:      /* A security problem */
      {
        /* Process the second status word */
        switch (rsp_ptr->sw2)
        {
          case SW2_NO_CHV:          /* No CHV initialized */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_NO_CHV_INITIALIZED;
            } /* end case - SW2_NO_CHV */
            break;

          case SW2_NOT_FULFILLED:   /* Access condition not fulfilled */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_ACCESS_NOT_FULFILLED;
            } /* end case - SW2_NOT_FULFILLED */
            break;

          case SW2_CONTRADICTION:   /* In contradiction with CHV status */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_CONTRADICTION_W_CHV_STATUS;
            } /* end case - SW2_CONTRADICTION */
            break;

          case SW2_INVALIDATION:    /* In contradiction with invalidation status */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_CONTRADICTION_W_INVALID_STATUS;
            } /* end case - SW2_INVALIDATION */
            break;

          case SW2_SEQ_PROBLEM:     /* Sequence problem */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_SSD_UPDATE_SEQ_PROBLEM;
            } /* end case - SW2_SEQ_PROBLEM */
            break;

          case SW2_BLOCKED:         /* Access blocked */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_CHV_BLOCKED;
            } /* end case - SW2_BLOCKED */
            break;

          case SW2_MAX_REACHED:     /* Increase cannot be performed; Max value
                                       reached                          */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_MAX_VALUE_REACHED;
            } /* end case - SW2_MAX_REACHED */
            break;

          case SW2_APPLN_SPECIFIC_AUTH_ERR:  /* Applcn specific auth error */
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_APPLCN_SPECIFIC_AUTH_ERROR;
            } /* end case - SW2_APPLN_SPECIFIC_AUTH_ERR */
            break;

          default:
            {
              /* Set the number of bytes received to zero. */
              rsp_ptr->cmd_rsp_size = 0;

              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_SECURITY */
      break;

    case SW1_P3_BAD:        /* Incorrect parameter P3 */
      {
        /* Set the number of bytes received to zero. */
        rsp_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        rsp_ptr->cmd_status = UIM_INCORRECT_PARAM_P3;
      } /* end case - SW1_P3_BAD */
      break;

    case SW1_P1_P2_BAD:     /* Incorrect parameter P1 or P2 */
      {
        /* Set the number of bytes received to zero. */
        rsp_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        rsp_ptr->cmd_status = UIM_INCORRECT_PARAM_P1_2;
      } /* end case - SW1_P1_P2_BAD */
      break;

    case SW1_INS_BAD:       /* Unknown instruction code */
      {
        /* Set the number of bytes received to zero. */
        rsp_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        rsp_ptr->cmd_status = UIM_UNKNOWN_INS;
      } /* end case - SW1_INS_BAD */
      break;

    case SW1_CLA_BAD:       /* Wrong instruction class given in command */
      {
        /* Set the number of bytes received to zero. */
        rsp_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        rsp_ptr->cmd_status = UIM_WRONG_CLASS;
      } /* end case - SW1_CLA_BAD */
      break;

    case SW1_PROBLEM:       /* Technical problem; no diagnostic given */
      {
        /* Set the number of bytes received to zero. */
        rsp_ptr->cmd_rsp_size = 0;

        /* Indicate APDU result */
        rsp_ptr->cmd_status = UIM_PROBLEM;
      } /* end case - SW1_PROBLEM */
      break;

    case SW1_USIM_END_RESP:
      {
        /* The status word is not expected.  Fail the command */
        rsp_ptr->cmd_status = UIM_PROBLEM;
        UIMDRV_MSG_ERR_0(uim_ptr->id,
                         "Response procedure byte received in T=1 protocol");
      } /* end case - SW1_USIM_END_RESP */
    break;

    case SW1_USIM_RESEND_APDU:
      {
        /* Process the second status word */
        switch (rsp_ptr->sw2)
        {
          case SW2_NV_STATE_UNCHANGED:          /* No info with NV unchanged */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_NO_INFO_AND_NV_UNCHANGED;
            } /* end case - SW2_NV_STATE_UNCHANGED */
            break;

          case SW2_RET_DATA_MAY_BE_CORRUPTED:   /* Part of ret data may be corrupt */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PART_DATA_MAY_BE_CORRUPTED;
            } /* end case - SW2_RET_DATA_MAY_BE_CORRUPTED */
            break;

          case SW2_EOF_REACHED:            /* EOF reached before reading Le bytes  */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_EOF_BEFORE_READING_LE_BYTES;
            } /* end case - SW2_EOF_REACHED */
            break;

          case SW2_SEL_EF_INVALIDATED:    /* Selected file invalidated */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_SELECTED_FILE_INVALIDATED;
            } /* end case - SW2_SEL_EF_INVALIDATED */
            break;

          case SW2_INCORRECT_FCI_FORMAT:     /* FCI not formatted correctly */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_INCORRECT_FCI_FORMAT;
            } /* end case - SW2_INCORRECT_FCI_FORMAT */
            break;

          default:
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_USIM_END_RESP */
    break;

    case SW1_WARNINGS1:
      {
        /* Process the second status word */
        switch (rsp_ptr->sw2)
        {
          case SW2_NV_STATE_UNCHANGED:          /* No info with NV unchanged */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_NO_INFO_AND_NV_UNCHANGED;
            } /* end case - SW2_NV_STATE_UNCHANGED */
            break;

          case SW2_RET_DATA_MAY_BE_CORRUPTED:   /* Part of ret data may be corrupt */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PART_DATA_MAY_BE_CORRUPTED;
            } /* end case - SW2_RET_DATA_MAY_BE_CORRUPTED */
            break;

          case SW2_EOF_REACHED:            /* EOF reached before reading Le bytes  */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_EOF_BEFORE_READING_LE_BYTES;
            } /* end case - SW2_EOF_REACHED */
            break;

          case SW2_SEL_EF_INVALIDATED:    /* Selected file invalidated */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_DONE;
            } /* end case - SW2_SEL_EF_INVALIDATED */
            break;

          case SW2_INCORRECT_FCI_FORMAT:     /* FCI not formatted correctly */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_INCORRECT_FCI_FORMAT;
            } /* end case - SW2_INCORRECT_FCI_FORMAT */
            break;

          default:
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_WARNINGS1 */
    break;

    case SW1_WARNINGS2:
      {
        switch (rsp_ptr->sw2)                 /* Process the second status word */
        {
          case SW2_NV_STATE_CHANGED:          /* No info with NV changed */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_NO_INFO_AND_NV_CHANGED;
            } /* end case -  */
            break;

          case SW2_FILE_FILLED_BY_LAST_WRITE:   /* File filled up by last write */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_FILE_FILLED_UP_BY_LAST_WRITE;
            } /* end case -  */
            break;

          default:
            {
              if ((rsp_ptr->sw2 & 0xF0) == (int)SW2_CMD_SUCCESS_INT_X_RETRIES)
              {
                 /* Indicate APDU result */
                 rsp_ptr->cmd_status = UIM_CMD_SUCCESS_WITH_X_RETRIES;
              }
              else
              {
                 /* Indicate APDU result */
                 rsp_ptr->cmd_status = UIM_PARITY;
              }
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_WARNINGS2 */
    break;

    case SW1_EXECUTION_ERR1:
      {
        switch (rsp_ptr->sw2)                 /* Process the second status word */
        {
          case SW2_NV_STATE_UNCHANGED:          /* No info with NV changed */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_EXEC_ERR_AND_NV_UNCHANGED;
            } /* end case - SW2_NV_STATE_UNCHANGED */
            break;

          default:
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_EXECUTION_ERR1 */
    break;

    case SW1_EXECUTION_ERR2:
      {
        switch (rsp_ptr->sw2)                 /* Process the second status word */
        {
          case SW2_NV_STATE_CHANGED:          /* No info with NV changed */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_EXEC_ERR_AND_NV_CHANGED;
            } /* end case - SW2_NV_STATE_CHANGED */
            break;

          case SW2_MEMORY_PROBLEM:            /* Memory problem  */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_EXEC_ERR_MEMORY_PROBLEM;
            } /* end case -  */
            break;

          default:
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_EXECUTION_ERR2 */
    break;

    case SW1_CLA_NOT_SUPPORTED:
      {
        switch (rsp_ptr->sw2)                 /* Process the second status word */
        {
          case SW2_NO_INFO_GIVEN:          /* No info given  */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_CLA_ERR_NO_INFO;
            } /* end case - No info given */
            break;

          case SW2_LOGICAL_CHAN_NOT_SUPPORTED:   /* Logical channel not supported */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_LOGICAL_CHAN_NOT_SUP;
            } /* end case - SW2_LOGICAL_CHAN_NOT_SUPPORTED */
            break;


          case SW2_SECURE_MSG_NOT_SUPPORTED:     /* Secure messaging not supported */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_SECURE_MESSAGING_NOT_SUP;
            } /* end case -  SW2_SECURE_MSG_NOT_SUPPORTED */
            break;

          default:
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_CLA_NOT_SUPPORTED */
    break;

    case SW1_CMD_NOT_ALLOWED:
      {
        switch (rsp_ptr->sw2)                 /* Process the second status word */
        {
          case SW2_NO_INFO_GIVEN:          /* No info given  */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_CMD_ERR_NO_INFO;
            } /* end case - SW2_NO_INFO_GIVEN */
            break;

          case SW2_FILE_STRUCT_INCOMPATIBLE:  /* Cmd incompatible with file struct */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_CMD_INCOMPATIBLE_WITH_FILE_STRUCT;
            } /* end case -  SW2_FILE_STRUCT_INCOMPATIBLE */
            break;


          case SW2_SECURITY_NOT_SATISFIED:     /* Security status not satisifed */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_SEC_STATUS_NOT_SATISFIED;
            } /* end case - SW2_SECURITY_NOT_SATISFIED  */
            break;

          case SW2_PIN_BLOCKED:     /* Authentication/PIN method blocked */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_AUTH_PIN_BLOCKED;
            } /* end case - UIM_SEC_STATUS_NOT_SATISFIED */
            break;

          case SW2_REF_DATA_INVALIDATED:     /* Referenced data invalidated */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_REF_DATA_INVALIDATED;
            } /* end case - SW2_REF_DATA_INVALIDATED */
            break;

          case SW2_CONDITIONS_NOT_SATISFIED:  /* Conditions of used not satisfied */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_USE_CONDNS_NOT_SATISFIED;
            } /* end case - SW2_CONDITIONS_NOT_SATISFIED */
            break;

          case SW2_NO_EF_SELECTED:     /* Cmd not allowed - No EF selected */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_CMD_ERR_NO_EF_SELECTED;
            } /* end case - SW2_NO_EF_SELECTED */
            break;

          default:
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_CMD_NOT_ALLOWED */
    break;

    case SW1_WRONG_PARAMS:
      {
        switch (rsp_ptr->sw2)                 /* Process the second status word */
        {
          case SW2_BAD_PARAMS_IN_DATA_FIELD:  /* Incorrect params in the data field */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_INCORRECT_PARAM_IN_DATA_FIELD;
            } /* end case - SW2_BAD_PARAMS_IN_DATA_FIELD */
            break;

          case SW2_FUNCTION_NOT_SUPPORTED:  /* Function not supported */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_FUNCTION_NOT_SUPPORTED;
            } /* end case - SW2_FUNCTION_NOT_SUPPORTED */
            break;

          case SW2_FILE_NOT_FOUND:     /* File not found */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_FILE_NOT_FOUND;
            } /* end case - SW2_FILE_NOT_FOUND */
            break;

          case SW2_RECORD_NOT_FOUND:     /* Record not found */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_RECORD_NOT_FOUND;
            } /* end case - SW2_RECORD_NOT_FOUND */
            break;

          case SW2_INCONSISTENT_LC:     /* Lc inconsistent with TLV structure */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_LC_INCONSISITENT_WITH_TLV;
            } /* end case - SW2_INCONSISTENT_LC */
            break;

          case SW2_BAD_PARAMS_P1_P2:  /* Incorrect parameters P1_P2 */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_INCORRECT_PARAMS_P1_P2;
            } /* end case - SW2_BAD_PARAMS_P1_P2 */
            break;

          case SW2_LC_INCONSISTENT_WITH_P1_P2:  /* Lc inconsistent with P1 and P2 */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_LC_INCONSISITENT_WITH_P1_P2;
            } /* end case - SW2_LC_INCONSISTENT_WITH_P1_P2 */
            break;

          case SW2_REF_DATA_NOT_FOUND:  /* Referenced data not found */
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_REF_DATA_NOT_FOUND;
            } /* end case - SW2_REF_DATA_NOT_FOUND */
            break;

          default:
            {
              /* Indicate APDU result */
              rsp_ptr->cmd_status = UIM_PARITY;
            } /* end case - default */
            break;

        } /* end switch - sw2 */
      } /* end case - SW1_WRONG_PARAMS */
    break;

    default:
      {
        /* Log the error */
        UIMDRV_MSG_ERR_1(uim_ptr->id,"The status word is not expected - %x",
                         rsp_ptr->sw1);

        /* Set the number of bytes received to zero. */
        rsp_ptr->cmd_rsp_size = 0;

        /* Give a bad status. */
        rsp_ptr->cmd_status = UIM_PROBLEM;

      } /* end case - default */
      break;
  } /* end switch - the first status word */

  /* Call the response callback function */
  uim_process_card_response(i_mask, uim_ptr);
} /* uim_process_t_1_command_response */




/*===========================================================================
FUNCTION UIM_PROCESS_T_1_BLOCK

DESCRIPTION
  This function checks all the rules of a block to see if the
  block is valid.  The check includes parity, etc.

DEPENDENCIES
   None
RETURN VALUE
  None.
===========================================================================*/
static boolean is_t_1_block_received_valid
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  /* Holds the parsed parameters from a received T=1 block */
  uim_block_params_type uim_block_params;

  if (rsp_ptr == NULL)
  {
    ERR_FATAL("is_t_1_block_received_valid rsp_ptr is NULL",0,0,0);
  }

  /* Retrieve the information to uim_block_params*/
  uim_block_params = uim_get_rx_block_type(uim_ptr);

  /* check if we have the right NAD with which we have transmitted */
  if ( uim_block_params.node_address_byte !=
       uim_ptr->t1.tx_prologue_field.nad )
  {
    /* The received block should be of the same NAD */
    UIMDRV_MSG_ERR_2(uim_ptr->id,
      "Block received with node address %x (should be %x)",
                     uim_block_params.node_address_byte,
                     uim_ptr->t1.tx_prologue_field.nad);
    return FALSE;
  }


  /* Check to see if this is a S block */
  if (uim_block_params.block_type == UIM_T_1_BLOCK_TYPE_S)
  {
    /* check to see if the s block has the response bit on while
     * we have not send out an s-block*/
    if (uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_PCB_S_RSP  &&
        uim_ptr->t1.t_1_protocol_state != UIM_T_1_SENT_SUPERVISORY_BLOCK_ST )
    {
      /* The s block should not be a response */
      UIMDRV_MSG_ERR_0(uim_ptr->id,
                       "S block received with response bit set (should be a request)");
      return FALSE;
    }

    if ( uim_block_params.block_params.s_block == UIM_S_BLOCK_IFS_REQUEST  ||
         uim_block_params.block_params.s_block == UIM_S_BLOCK_IFS_RESPONSE ||
         uim_block_params.block_params.s_block == UIM_S_BLOCK_WTX_REQUEST  ||
         uim_block_params.block_params.s_block == UIM_S_BLOCK_WTX_RESPONSE
       )
    {
      if ( rsp_ptr->cmd_rsp_size != 1 )
      {
        /* S IFS or WTX block request should be of size 1 7816 9.4.3 */
        UIMDRV_MSG_ERR_1(uim_ptr->id,"S block IFS OR WTX received with length %x (should be 1)",
                         rsp_ptr->cmd_rsp_size);
        return FALSE;
      }
    }
    else if ( rsp_ptr->cmd_rsp_size != 0 )
    {
      /* S block RESYNC, ERROR, ABORT should be of size 0 7816 9.4.3 */
      UIMDRV_MSG_ERR_1(uim_ptr->id,"S block RESYNC OR ERROR or ABORT received with length %x (should be 0)",
                       rsp_ptr->cmd_rsp_size);
      return FALSE;
    }

    if ( ( uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_SBLK_B1_B5_MASK ) >
         UIM_T_1_SBLK_B1_B5_MAX_VAL )
    {
      /* Value encoded is RFU */
      UIMDRV_MSG_ERR_1(uim_ptr->id,"S block received with RFU values encoded",
                       (uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_SBLK_B1_B5_MASK));
      return FALSE;
    }
  } /* Check to see if this is a R block */
  else if (uim_block_params.block_type == UIM_T_1_BLOCK_TYPE_R)
  {
    if ( rsp_ptr->cmd_rsp_size != 0 )
    {
      /* R block request should be of size 0 */
      UIMDRV_MSG_ERR_1(uim_ptr->id,"R block received with length %x (should be 0)",
                       rsp_ptr->cmd_rsp_size);
      return FALSE;
    }
    else if ( ( uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_RBLK_B1_B6_MASK ) >
                UIM_T_1_RBLK_B1_B6_MAX_VAL )
    {
      /* RFU bits populated in the PCB */
      UIMDRV_MSG_ERR_1(uim_ptr->id,"R block received with RFU values encoded",
                       (uim_ptr->t1.rx_prologue_field_buffer.pcb & UIM_T_1_RBLK_B1_B6_MASK));
      return FALSE;
    }
  }


  UIMDRV_MSG_LOW_0(uim_ptr->id,"Validated the received t_1 block returning TRUE");
  return TRUE;
} /*lint !e715 */




/*===========================================================================

FUNCTION UIM_PROCESS_T_1_BLOCK

DESCRIPTION
  This function processes the block response from a T=1 protocol transaction.
  a request to send an APDU to the UIM card while
  operating in T=1 transport protocol.

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.  This
  function uses global variables to set up the UIM driver and server transfers.

RETURN VALUE
  None.

SIDE EFFECTS
  The UIM driver will continue to send out the APDU.  All the stages of
  sending the APDU are handled by the UIM driver.  Then the driver expects
  the T=1 block response.  Additional blocks are sent out when processing
  the block responses from the card.

===========================================================================*/
void uim_process_t_1_block
(
  uim_rsp_buf_type         *rsp_ptr,
  rex_sigs_type            *i_mask,   /* Mask of signals interested in */
  uim_instance_global_type *uim_ptr
)
{
  /* Holds the parsed parameters from a received T=1 block */
  uim_block_params_type uim_block_params;
  /* Holds if the block received is valid or not */
  boolean received_block_is_valid;

  /*-----------------------------------------------------------------*/

  UIMDRV_MSG_LOW_0(uim_ptr->id, "uim_process_t_1_block" );
  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_process_t_1_block rsp_ptr is NULL",0,0,0);
  }

  /* Process the block based upon the current state of the protocol */
  switch (uim_ptr->t1.t_1_protocol_state)
  {
    /* The command has failed.  Waiting for card response to fail command */
    case UIM_T_1_FAILED_CMD_ST:
      {
        /* Check if the block was received properly */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Process a bad block */
          uim_t_1_process_bad_block (uim_ptr);
        }
        else
        {
          uim_block_params = uim_get_rx_block_type(uim_ptr);
          if (uim_block_params.block_type == UIM_T_1_BLOCK_TYPE_S)
          {
            /* Process the S-block */
            uim_t_1_process_s_block (rsp_ptr,
                                           i_mask,
                                           uim_block_params,
                                           uim_ptr);
          }
          else
          {
            /* Fail the current command */
            /* Set the number of bytes received to zero. */
            rsp_ptr->cmd_rsp_size = 0;

            /* Give a bad status. */
            rsp_ptr->cmd_status = UIM_PROBLEM;

            /* Call the response callback function */
            uim_process_card_response(i_mask, uim_ptr);
          }
        }
      } /* end case - UIM_T_1_FAILED_CMD_ST */
      break;

    case UIM_T_1_CACHED_SEL_RSP_ST: /* The SELECT response was already cached */
      {
        /* Set the protocol state back to idle */
        uim_ptr->t1.t_1_protocol_state = UIM_T_1_FAILED_CMD_ST;

        /* Process the received response for the upper layers */
        uim_process_t_1_command_response(rsp_ptr, i_mask, uim_ptr);
      }/* end case - UIM_T_1_CACHED_SEL_RSP_ST */
      break;

    case UIM_T_1_SENT_APDU_ST:  /* An APDU response is expected */
      {
        received_block_is_valid = is_t_1_block_received_valid(
           rsp_ptr, uim_ptr);

        /* Check if the block was received properly */
        if (rsp_ptr->cmd_status != UIM_DONE || !received_block_is_valid)
        {
          /* Process a bad block */
          uim_t_1_process_bad_block (uim_ptr);
        }
        else /* This is a good block */
        {
          /* Process the block according to block type */
          uim_block_params = uim_get_rx_block_type(uim_ptr);
          switch (uim_block_params.block_type)
          {
            case UIM_T_1_BLOCK_TYPE_I:
              {
                /* check if this is the ack for the I-block sent */
                if (uim_block_params.block_params.i_block.send_seq_num ==
                    uim_ptr->t1.card_send_sequence_num)
                {
                  /* The I-block sent out has been acknowledged */
                  /* Change the send sequence number for the next I-block */
                  uim_ptr->t1.me_send_sequence_num ^= uim_ptr->t1.t_1_changed_me_send_seq_number;

                  /* Change the send sequence number for the next I-block from
                     the card */
                  uim_ptr->t1.card_send_sequence_num ^= UIM_T_1_CHANGE_SEQ_NUM_MASK;

                  /* Adjust the response buffer pointer for the next block */
                  uim_ptr->t1.apdu_rsp_buffer_ptr += rsp_ptr->cmd_rsp_size;

                  /* Keep a total of the total number of bytes in the
                     response */
                  uim_ptr->t1.apdu_rsp_num_bytes += rsp_ptr->cmd_rsp_size;

                  /* The I-block is the response to the I-block transmitted
                     to the card.  Check if the response is chained */
                  if (uim_block_params.block_params.i_block.more_bit)
                  {
                    /* receiving chained block */
                    uim_ptr->t1.t_1_receiving_chained_block = TRUE;

                    /* The transmitted I-block has been acknowledged.  Do not
                       change the sequence number for the next received
                       I-block */
                    uim_ptr->t1.t_1_changed_me_send_seq_number = 0;
                    /* Send an R-block for the next card I-block */
                    uim_t_1_send_r_block (uim_ptr->t1.t_1_node_address,
                                          UIM_T_1_PCB_ERROR_FREE,
                                          uim_ptr);
                  }
                  else /* This is the entire response */
                  {
                    /* not receiving chain any more */
                    uim_ptr->t1.t_1_receiving_chained_block = FALSE;

                    /* Set the protocol state back to idle */
                    uim_ptr->t1.t_1_protocol_state = UIM_T_1_FAILED_CMD_ST;

                    /* Process the received response for the upper layers */
                    uim_process_t_1_command_response(rsp_ptr, i_mask, uim_ptr);
                  } /* end if - the received I-block is chained */
                }
                else
                {
                  /*Not the expected sequence number for the I-block sent*/
                  uim_t_1_send_r_block (uim_ptr->t1.t_1_node_address,
                                        UIM_T_1_PCB_OTHER_ERROR,
                                        uim_ptr);
                }
              } /* end case - UIM_T_1_BLOCK_TYPE_I */
              break;

            case UIM_T_1_BLOCK_TYPE_R:
              {
                /* Check if the send sequence number requires a
                   re-transmission */
                if ((uim_block_params.block_params.r_block.send_seq_num ==
                    uim_ptr->t1.me_send_sequence_num) && uim_ptr->t1.t_1_error_count < 2 )
                {
                  /* Send out the I-block again */
                  uim_t_1_send_i_block (uim_ptr->t1.t_1_node_address, uim_ptr);
                }
                else /* the send sequence number is not expected */
                {
                  /* We were expecting a response */
                  /* Ask for the response */
                  uim_t_1_send_r_block (uim_ptr->t1.t_1_node_address,
                                        UIM_T_1_PCB_OTHER_ERROR,
                                        uim_ptr);
                } /* end if - the send sequence number is proper */
              } /* end case - UIM_T_1_BLOCK_TYPE_R */
              break;

            case UIM_T_1_BLOCK_TYPE_S:
              {
                /* Process the supervisory block */
                uim_t_1_process_s_block (rsp_ptr, i_mask, uim_block_params, uim_ptr);
              } /* end case - UIM_T_1_BLOCK_TYPE_S */
              break;

            default:
              {
                /* The PCB byte is not valid.  Process as a bad block */
                uim_t_1_process_bad_block (uim_ptr);
              } /* end case - default */
              break;
          } /* end switch - received block type */
        } /* end if - block had errors */
      } /* end case - UIM_T_1_SENT_APDU_ST */
      break;

    case UIM_T_1_SENT_CHAINED_APDU_ST:  /* An R-block is expected */
      {
        received_block_is_valid = is_t_1_block_received_valid(
           rsp_ptr, uim_ptr);

        /* Check if the block was received properly */
        if (rsp_ptr->cmd_status != UIM_DONE || !received_block_is_valid)
        {
          /* Process a bad block */
          uim_t_1_process_bad_block (uim_ptr);
        }
        else /* This is a good block */
        {
          /* Process the block according to block type */
          uim_block_params = uim_get_rx_block_type(uim_ptr);
          switch (uim_block_params.block_type)
          {
            case UIM_T_1_BLOCK_TYPE_I:
              {
                /* We did not expect an I-block at this time as we have not
                   finished sending the APDU command */
                /* Re-synchronize with the card */
                uim_t_1_resynch_w_card (uim_ptr);
              } /* end case - UIM_T_1_BLOCK_TYPE_I */
              break;

            case UIM_T_1_BLOCK_TYPE_R:
              {
                /* Check if the send sequence number requires a
                   re-transmission */
                if ((uim_block_params.block_params.r_block.send_seq_num ==
                    uim_ptr->t1.me_send_sequence_num)  && uim_ptr->t1.t_1_error_count < 2 )
                {
                  /* Send out the I-block again */
                  uim_t_1_send_i_block (uim_ptr->t1.t_1_node_address, uim_ptr);
                }
                else /* I-block is acknowledged, send next I-block */
                {
                  /* We need to re-set the error count since we have got
                   *  a valid response
                   */
                  uim_ptr->t1.t_1_error_count=0;
                  /* The I-block is acknowledged */
                  /* Change the send sequence number for the next I-block */
                  uim_ptr->t1.me_send_sequence_num ^= UIM_T_1_CHANGE_SEQ_NUM_MASK;
                  /* Update the pointer and size of the request buffer */
                  uim_ptr->t1.apdu_req_buffer_ptr += uim_ptr->t1.last_tx_i_block_length;
                  uim_ptr->t1.apdu_request_bytes_to_send -= uim_ptr->t1.last_tx_i_block_length;

                  /* Send the next I-block */
                  uim_t_1_send_i_block (uim_ptr->t1.t_1_node_address, uim_ptr);
                } /* end if - the send sequence number is proper */
              } /* end case - UIM_T_1_BLOCK_TYPE_R */
              break;

            case UIM_T_1_BLOCK_TYPE_S:
              {
                /* Process the supervisory block */
                uim_t_1_process_s_block (rsp_ptr, i_mask, uim_block_params, uim_ptr);
              } /* end case - UIM_T_1_BLOCK_TYPE_S */
              break;

            default:
              {
                /* Something is very wrong.  Process as a bad block */
                uim_t_1_process_bad_block (uim_ptr);
              } /* end case - default */
              break;
          } /* end switch - received block type */
        } /* end if - block had errors */
      } /* end case - UIM_T_1_SENT_CHAINED_APDU_ST */
      break;

    case UIM_T_1_SENT_SUPERVISORY_BLOCK_ST: /* A supervisory response is
                                               expected */
      {
        received_block_is_valid = is_t_1_block_received_valid(
           rsp_ptr, uim_ptr);
        /* Check if the block was received properly */
        if (rsp_ptr->cmd_status != UIM_DONE || !received_block_is_valid)
        {
          /* Re-send the S-block */
          uim_t_1_resend_s_block(uim_ptr);
        }
        else /* This is a good block */
        {
          /* Process the block according to block type */
          uim_block_params = uim_get_rx_block_type(uim_ptr);
          switch (uim_block_params.block_type)
          {
            case UIM_T_1_BLOCK_TYPE_I:
              {
                /* We did not expect an I-block at this time as we have sent
                   out an S-block.  Re-send the S-block */
                uim_t_1_resend_s_block(uim_ptr);
              } /* end case - UIM_T_1_BLOCK_TYPE_I */
              break;

            case UIM_T_1_BLOCK_TYPE_R:
              {
                /* Re-send the S-block */
                uim_t_1_resend_s_block(uim_ptr);
              } /* end case - UIM_T_1_BLOCK_TYPE_R */
              break;

            case UIM_T_1_BLOCK_TYPE_S:
              {
                /* Process the supervisory block */
                uim_t_1_process_s_block (rsp_ptr, i_mask, uim_block_params, uim_ptr);
              } /* end case - UIM_T_1_BLOCK_TYPE_S */
              break;

            default:
              {
                /* Re-send the S-block */
                uim_t_1_resend_s_block(uim_ptr);
              } /* end case - default */
              break;
          } /* end switch - received block type */
        } /* end if - block had errors */
      } /* end case - UIM_T_1_SENT_SUPERVISORY_BLOCK_ST */
      break;

    default:
      {
        /* The T=1 protocol state is out of range */
        UIMDRV_MSG_ERR_0(uim_ptr->id,
                         "UIM T=1 protocol state is out of range");

        /* Set the protocol state back to idle */
        uim_ptr->t1.t_1_protocol_state = UIM_T_1_FAILED_CMD_ST;

        /* Re-synchronize with the card */
        uim_t_1_resynch_w_card (uim_ptr);
      } /* end case - default */
      break;
  } /* end switch - uim_ptr->t1.t_1_protocol_state */
} /* uim_process_t_1_block */


/*===========================================================================

FUNCTION UIM_PROCESS_T_1_TIMEOUT

DESCRIPTION
  This function processes the timeouts that happen when running the T=1
  protocol.

DEPENDENCIES
  This function uses global variables to determine how to process the timout.

RETURN VALUE
  A boolean value that indicates whether the timout processing is fully
  handled within the T=1 protocol timeout processing.
  FALSE - timeout is not fully handled, task timeout needs to be processed.
  TRUE - timeout is fully handled within T=1 processing.

SIDE EFFECTS
  Either another request is sent to the card or this function indicates
  to the task level timeout processing to process the timeout.

===========================================================================*/
boolean uim_process_t_1_timeout
(
  uim_instance_global_type *uim_ptr
)
{
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "uim_process_t_1_timeout" );

  /* An error count greater than UIM_MAX_BAD_BLOCK_RETRIES indicates the
     card needs to be reset */
  if (uim_ptr->t1.t_1_error_count > UIM_MAX_BAD_BLOCK_RETRIES)
  {
    /* Indicate the card needs to be reset */
    return( FALSE );
  }

  /* Before we attempt to send a S block or R block we enable the RX/TX
     and setup the baud rate as during the timeout handling we disable
     the RX/TX and reset the UART */
  UIM_ENABLE_UART_TX_SLOT (uim_ptr);
  UIM_ENABLE_UART_RX_SLOT (uim_ptr);
  SET_UIM_BAUD_RATE_SLOT (uim_ptr->id, uim_ptr->state.op_params_buf.FI, uim_ptr->state.op_params_buf.DI);

  /* Determine if driver is waiting for an S-block response */
  if (UIM_T_1_SENT_SUPERVISORY_BLOCK_ST == uim_ptr->t1.t_1_protocol_state)
  {
    /* Re-send the S-block */
    uim_t_1_resend_s_block(uim_ptr);
  }
  else /* The current request is not a supervisory request */
  {
    /* Process a bad block */
    uim_t_1_process_bad_block (uim_ptr);
  } /* end if - current request is a supervisory request */

  /* Indicate the card does not need to be reset */
  return( TRUE );
} /* uim_process_t_1_timeout */

#else
void feature_uim_t_1_support_dummy(void) {}    /* To prevent compiler warning C3040E: no external declaration in translation unit */
#endif /* FEATURE_UIM_T_1_SUPPORT */
