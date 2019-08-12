/*============================================================================
  FILE:         rr_rr.c

  OVERVIEW:     This module provides a function to send signals on the RR
                signalling interface (MS_RR_RR).

  DEPENDENCIES: None

                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_rr.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_message_set.h"
#include "gs.h"
#include "msg.h"
#include "ms.h"
#include "sys_stru.h"
#include "rr_gprs_debug.h"
#include "rr_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static void rr_send_to_self(rr_message_header_t *header_ptr,
                            uint8                message_id,
                            uint16               message_length,
                            rr_message_set_e     rr_message_set,
                            gas_id_t             gas_id)
{
  gs_status_T gs_status;

  header_ptr->imh.message_id  = message_id;
  header_ptr->imh.message_set = MS_RR_RR;

  PUT_IMH_LEN((message_length - sizeof(IMH_T)), &header_ptr->imh);

  /* fill in the header payload */
  header_ptr->rr_message_set = rr_message_set;

  rr_show_output_message("RR", &header_ptr->imh, MS_RR_RR, (int32)message_id,gas_id);

  gs_status = gs_send_message(GS_QUEUE_RR, (void *)header_ptr, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("Unable to send RR->RR message, GS status %d",(int)gs_status);
  }

  return;
}

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

/**
  @brief Sends a message on the RR signalling interface.

  This function will take the message indicated by the 'header' parameter,
  set up the remaining header fields (header length, message set, RR message
  set) using the indicated values and copy the message to the RR command queue.

  @param header_ptr:     Pointer to the header item of the message to be sent.
  @param message_id:     Message ID of the message to be sent.
  @param message_length: Length of the message to be sent.
  @param rr_message_set: RR message set of the message to be sent.
*/
void rr_rr_send(rr_message_header_t *header_ptr,
                uint8                message_id,
                uint16               message_length,
                rr_message_set_e     rr_message_set,
                const gas_id_t       gas_id)
{
  /* the active access stratum context is sending this message */
  header_ptr->gas_id = gas_id;

  /* use local worker function to queue the outbound message */
  rr_send_to_self(header_ptr, message_id, message_length, rr_message_set,gas_id);

  return;
}

/* EOF */

