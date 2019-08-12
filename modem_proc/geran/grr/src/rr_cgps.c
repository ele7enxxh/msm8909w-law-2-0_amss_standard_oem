/*!
 * \file rr_cgps.c 
 *  
 * This module contains interface functionality between GRR and location software (CGPS).
 *  
 *              Copyright (c) 2014 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cgps.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "rr_lsm_if.h"  // Need for APIs defined outside of featurisation

#ifdef FEATURE_CGPS_UMTS_CP_GSM

#include "sys.h"
#include "nas_exp.h"
#include "rr_cgps.h"
#include "rr_multi_sim.h"
#include "rr_task.h"
#include "rr_l2.h"
#include "rr_l2_l1.h"
#include "rr_l2_send.h"
#include "rr_defs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  boolean                   apdu_in_progress;
  boolean                   lcs_active;
} rr_cgps_module_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

// Module data per instance
static rr_cgps_module_data_t  rr_cgps_module_data[NUM_GERAN_DATA_SPACES];

// Only one call-back regardless of num of instances
static rr_cgps_event_cb_type  rr_cgps_event_cb;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 * 
 * \param as_id (in)
 * 
 * \return rr_cgps_module_data_t* 
 */
static rr_cgps_module_data_t *rr_cgps_get_data_ptr(const sys_modem_as_id_e_type as_id)
{
#ifdef FEATURE_TRIPLE_SIM
  if (as_id == SYS_MODEM_AS_ID_3)
  {
    return &rr_cgps_module_data[GERAN_DATA_SPACE_INDEX_3];
  }
#endif // FEATURE_TRIPLE_SIM

#ifdef FEATURE_DUAL_SIM
  if (as_id == SYS_MODEM_AS_ID_2)
  {
    return &rr_cgps_module_data[GERAN_DATA_SPACE_INDEX_2];
  }
#endif // FEATURE_DUAL_SIM

  return &rr_cgps_module_data[GERAN_DATA_SPACE_INDEX_1];
}

/*!
 * \brief This functions sends a simple EVENT (with no data) to CGPS/LSM.
 * 
 * \param event_id 
 */
static void rr_cgps_send_simple_cgps_event(rr_cgps_event_type event_id, const gas_id_t gas_id)
{
  if (rr_cgps_event_cb != NULL)
  {
    rr_cgps_event_data_type   apdu_pkt;

    apdu_pkt.event_id = event_id;
    apdu_pkt.as_id = geran_map_gas_id_to_nas_id(gas_id);

    rr_cgps_event_cb( &apdu_pkt );
  }
  else
  {
    MSG_GERAN_ERROR_0("RR_CGPS: No rr_cgps_event_cb registered");
  }

  return;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Process a CGPS event.
 * 
 * \param new_message_ptr 
 * \param gas_id 
 */
void rr_cgps_process_cgps_event_req(const rr_cmd_bdy_type *message_ptr, const gas_id_t gas_id)
{
  const rr_cgps_event_req_T *cgps_event_req;
  rr_cgps_module_data_t *module_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(message_ptr);

  cgps_event_req = &message_ptr->rr_cgps_event_req;

  // Obtain a pointer to the module data
  module_data_ptr = rr_cgps_get_data_ptr(RR_GAS_ID_TO_AS_ID);

  if (cgps_event_req->send_rr_event == TRUE)
  {
    /* Note lcs_active can transition from TRUE to TRUE when
    an active LCS session was interrupted by the start of another
    LCS session */
    module_data_ptr->lcs_active = TRUE;
  }
  else
  {
    module_data_ptr->lcs_active = FALSE;
  }

  return;
}

/*!
 * \brief Responsible for segmenting an APDU from the application layer according to GSM 44.018 and sending 
 *        the segments to the L2 layer. 
 * 
 * \param apdu_id (in)
 * \param new_message_ptr (in)
 * \param gas_id (in)
 */
void rr_cgps_process_rr_apdu_req(
  rr_apdu_id_T apdu_id,
  const rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  byte   apdu_buff[256], *p_buff = apdu_buff;
  uint16 num_segments;
  uint16 seg_cnt;
  uint16 apdu_len;
  const uint8 *apdu_ptr;
  uint8  byte_cnt;
  uint8  num_bytes_remaining;
  uint8  segment_length;
  uint8  flags;

  RR_NULL_CHECK_RETURN_VOID(message_ptr);

  apdu_len = message_ptr->rr_apdu_req.apdu_len;
  apdu_ptr = &message_ptr->rr_apdu_req.apdu_data[0];

  /* If length is zero, simply return */
  if ( apdu_len == 0 )
     return;

  /* Determine number of segments and remaining bytes */
  num_segments = apdu_len / (MAX_OCTETS_L3_MESSAGE - 4);
  num_bytes_remaining = apdu_len % (MAX_OCTETS_L3_MESSAGE - 4);

  if ( num_bytes_remaining )
  {
    num_segments++;
  }

  for ( seg_cnt = 0; seg_cnt < num_segments; seg_cnt++ )
  {
    *p_buff++ = RADIO_RESSOURCES;           /* RR Protocol Descriptor, Skip Indicator 0 */
    *p_buff++ = APPLICATION_INFORMATION;    /* Application Information Message Type */
    *p_buff   = (byte)apdu_id;              /* APDU ID */

    /* APDU Flag Byte */
    if ( num_segments == 1 )
    {
      /* Only segment ( both first and last ) */
      flags = RR_APDU_COMMAND_OR_FINAL_RESP;
    }
    else
    if ( seg_cnt == 0 )
    {
      /* First of a multi segment APDU */
      flags = RR_APDU_COMMAND_OR_FINAL_RESP | RR_APDU_LAST_FLAG;
    }
    else
    if ( seg_cnt == num_segments - 1 )
    {
      /* Last of a multi segment apdu */
      flags = RR_APDU_COMMAND_OR_FINAL_RESP | RR_APDU_FIRST_FLAG;
    }
    else
    {
      /* Inner segment of a multi segment apdu */
      flags = RR_APDU_COMMAND_OR_FINAL_RESP | RR_APDU_FIRST_FLAG | RR_APDU_LAST_FLAG;
    }

    *p_buff++ |= (byte) (flags << 4);

    /* Segment Length */
    if ( num_segments == 1 )
    {
      /* Only Segment */
      segment_length = (uint8)apdu_len;
    }
    else
    if ( seg_cnt == num_segments - 1 )
    {
      /* Last Segment Of A Multi-Part APDU */
      segment_length = num_bytes_remaining;
    }
    else
    {
      /* Full Size Segment */
      segment_length = MAX_OCTETS_L3_MESSAGE - 4;
    }

    *p_buff++ = segment_length;

    /* Segment Data */
    for ( byte_cnt = 0; byte_cnt < segment_length; byte_cnt++ )
    {
      *p_buff++ = *apdu_ptr++;
    }

    /* Send Data to L2 */
    (void) rr_send_dl_data_req(
      &apdu_buff[0],                  // layer3_message
      (uint8) (segment_length + 4),   // length
      SAPI0,                          // SAPI
      DCCH,                           // l2_channel_type
      gas_id                          // gas_id
    );

    p_buff = &apdu_buff[0];
  }

  return;
}

/*!
 * \brief Responsible for passing a received APDU segment to the application layer for processing.
 * 
 * \param new_message_ptr (in)
 * \param gas_id (in)
 */
void rr_cgps_process_apdu_data_seg(const rr_cmd_bdy_type *message_ptr, const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id;
  rr_cgps_module_data_t *module_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(message_ptr);

  as_id = geran_map_gas_id_to_nas_id(gas_id);

  // Obtain a pointer to the module data
  module_data_ptr = rr_cgps_get_data_ptr(as_id);

  if (((message_ptr->dl_data_ind.layer3_message[2] & 0xF) == (int)RR_APDU_ID_RRLP) &&
      (message_ptr->dl_data_ind.layer3_message[3] <= (MAX_OCTETS_L3_MESSAGE-4)))
  {
    if (rr_cgps_event_cb != NULL)
    {
      const uint8 *             apdu_ptr;
      rr_cgps_event_data_type   apdu_pkt;

      apdu_ptr = message_ptr->dl_data_ind.layer3_message;

      /* Skip to byte 2 of the L3 message */
      apdu_ptr += 2;

      apdu_pkt.event_id       = RR_CGPS_APDU_MSG_EVENT;
      apdu_pkt.u.apdu.flags   = (*apdu_ptr & 0xF0) >> 4;
      apdu_pkt.u.apdu.id      = *apdu_ptr++ & 0x0F;
      apdu_pkt.u.apdu.length  = *apdu_ptr++;
      apdu_pkt.u.apdu.p_data  = apdu_ptr;
      apdu_pkt.as_id          = as_id;
      /* Keep track of whether a multi segment APDU is in
         progress. This status is used in rr_main to determine
           whether or not to indicate to LSM that another L3
             message type has been received */

      if ( !(apdu_pkt.u.apdu.flags & RR_APDU_FIRST_FLAG ) )
      {
        module_data_ptr->apdu_in_progress = TRUE;
      }

      if ( !(apdu_pkt.u.apdu.flags & RR_APDU_LAST_FLAG ) )
      {
        module_data_ptr->apdu_in_progress = FALSE;
      }
      rr_cgps_event_cb( &apdu_pkt );
    }
    else
    {
      MSG_GERAN_ERROR_0_G("RR_CGPS: No rr_cgps_event_cb registered");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unsupported APDU ID or invalid length - message discarded");
  }

  return;
}

/*!
 * \brief If an apdu re-segmentation is in progress, this function checks each received L3 message to see if 
 *        it is on SAPI0 and the main DCCH. If so and this message is not another APDU segment, this function
 *        notifies the application of this event so it can discard the previous APDU segments.
 * 
 * \param new_message (in)
 * \param gas_id (in)
 */
void rr_cgps_apdu_in_progress_check(const rr_cmd_bdy_type* new_message, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_cgps_module_data_t *module_data_ptr = rr_cgps_get_data_ptr(RR_GAS_ID_TO_AS_ID);

  /* As defined in GSM04.18 Release 99, if we are currently in the middle
     of receiving an application protocol data unit and another L3 message
     comes in on SAPI0 on the main DCCH then the APDU segment must be discarded.
     Examine the Protocol Discriminator and message type of this message. If
     this message is not an APDU segment while we are in the process of
     reconstructing an APDU, then the Location Services Manager (LSM) task must
     be notified of this event */

  if ( ( module_data_ptr->apdu_in_progress == TRUE ) && ( new_message->dl_data_ind.SAPI == SAPI0 ) &&
       ( new_message->dl_data_ind.l2_channel_type == DCCH ) )
  {
    byte protocol_discriminator = new_message->dl_data_ind.layer3_message[0] & 0xF;
    byte message_type = new_message->dl_data_ind.layer3_message[1];

    if ( ( protocol_discriminator != RADIO_RESSOURCES ) ||
         ( message_type != APPLICATION_INFORMATION ) )
    {
       /* Notify LSM of this event */
      rr_cgps_send_simple_cgps_event( RR_CGPS_APDU_ABORT_EVENT, gas_id );
      module_data_ptr->apdu_in_progress = FALSE;
    }
  }

  return;
}

/*===========================================================================

FUNCTION  rr_cgps_lcs_activity_check

DESCRIPTION  If an LCS session is currently active or expected to be active
the lcs_active flag will be set to TRUE. The function checks if an RR procedure
is initiated when the LCS is active (i.e lcs_active flag is TRUE)

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/
void rr_cgps_lcs_activity_check(const rr_cmd_bdy_type *message_ptr, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_cgps_module_data_t *module_data_ptr = rr_cgps_get_data_ptr(RR_GAS_ID_TO_AS_ID);

  /* 03.71 Release 99:
   A target MS shall terminate any positioning procedure or the transfer
   of RRLP positioning assistance data without sending any response to the
   SMLC if any RR message is received from the BSC that starts some other
   RR management procedure, including a new positioning procedure.
   The new RR procedure shall then be executed by the MS.
 */

  if ( ( module_data_ptr->lcs_active == TRUE ) && ( message_ptr->dl_data_ind.SAPI == SAPI0 ) &&
       ( message_ptr->dl_data_ind.l2_channel_type == DCCH ) )
  {
    byte protocol_discriminator = message_ptr->dl_data_ind.layer3_message[0] & 0xF;
    byte message_type = message_ptr->dl_data_ind.layer3_message[1];

    if ( ( protocol_discriminator == RADIO_RESSOURCES ) &&
         ( message_type != APPLICATION_INFORMATION ) )
    {
      MSG_GERAN_HIGH_0_G("RR Management Procedure - Aborting LCS Activity");
      /* Notify LSM of this event */
      rr_cgps_send_simple_cgps_event( RR_CGPS_SESSION_ABORT_EVENT, gas_id );
      /* lcs_active will be set to false when the rr_lsm_event_req message
      is received from LSM */
    }
  }

  return;
}

/*!
 * \brief Initialisation function. Note: this doesn't clear any registered call-backs.
 */
void rr_cgps_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_cgps_module_data_t *module_data_ptr = rr_cgps_get_data_ptr(RR_GAS_ID_TO_AS_ID);

  module_data_ptr->apdu_in_progress = FALSE;
  module_data_ptr->lcs_active = FALSE;

  return;
}

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
void rr_cgps_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_cgps_module_data, 0, sizeof(rr_cgps_module_data));

  // Clear the registered call-back
  rr_cgps_event_cb = NULL;

  return;
}

/* ----------------------------  Public APIs  -------------------------------- */

/*!
 * \brief This function is called by CGPS to register a callback which will be used by RR to provide events 
 *        and/or data back to CGPS 
 * 
 * \param cb_fp (in)
 */
void geran_grr_register_cgps_event_cb(rr_cgps_event_cb_type cb_fp)
{
  rr_cgps_event_cb = cb_fp;

  return;
}

#else // FEATURE_CGPS_UMTS_CP_GSM

/*!
 * \brief This function is called by CGPS to register a callback which will be used by RR to provide events 
 *        and/or data back to CGPS 
 * 
 * \param cb_fp (in)
 */
void geran_grr_register_cgps_event_cb(rr_cgps_event_cb_type cb_fp)
{
  (void)cb_fp;
  return;
}

#endif // FEATURE_CGPS_UMTS_CP_GSM

/* EOF */

