/*!
 * \file rr_qsh_dump.c
 *
 * This module contains functionality to interface to QSH.
 *
 *              Copyright (c) 2015 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_qsh_dump.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"

#ifdef FEATURE_QSH_DUMP

#include "qsh.h"
#include "grr_qsh_ext.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// Define a structure with QSH dump header and all TLV structures (for T32 debugging)
typedef struct
{
  qsh_dump_hdr_s hdr;
  grr_qsh_dump_tag_data_s grr_qsh_dump_tag_data;
  grr_qsh_dump_tag_data_s grr_qsh_dump_tag_data_2;
} rr_qsh_tc_dump_collect_s;

typedef struct
{
  void *data_ptr;
  uint16 size_bytes;
} rr_dump_tag_def_s;

// Pointer declared as type of QSH data structure for parser
grr_qsh_dump_tag_data_s  *grr_qsh_dump_tag_0_ptr;
grr_qsh_dump_tag_data_s  *grr_qsh_dump_tag_1_ptr;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static uint8 rr_dump_tag_data_seq_num;
static timetick_type rr_dump_tag_data_timestamp;

static grr_qsh_dump_tag_data_s rr_dump_tag_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Constant Variable Definitions
 * -------------------------------------------------------------------------*/

rr_dump_tag_def_s dump_tag_def[GRR_QSH_DUMP_TAG_COUNT] =
{
  // GRR_QSH_DUMP_TAG_DATA
  {
    &rr_dump_tag_data[0],
    sizeof(grr_qsh_dump_tag_data_s)
  },
  // GRR_QSH_DUMP_TAG_DATA_2
  {
    &rr_dump_tag_data[1],
    sizeof(grr_qsh_dump_tag_data_s)
  }
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Get next sequence number for logging messages. If the timestamp is unchanged, then the sequence
 *        number is incremented, otherwise it will be zero.
 *
 * \param timestamp
 *
 * \return uint8 - sequence number
 */
__attribute__((section(".uncompressible.text")))
uint8 get_seq_num(timetick_type timestamp)
{
  if (timestamp == rr_dump_tag_data_timestamp)
  {
    rr_dump_tag_data_seq_num++;
  }
  else
  {
    rr_dump_tag_data_seq_num = 0;
  }

  return rr_dump_tag_data_seq_num;
}

/*!
 * \brief Writes the data for the given dump tag to the given buffer.
 *
 * \param dump_tag
 * \param buffer_ptr
 * \param bytes_remaining
 *
 * \return uint16
 */
__attribute__((section(".uncompressible.text")))
uint16 write_dump_tag_data_to_buffer(
  grr_qsh_dump_tag_e dump_tag,
  uint8 *buffer_ptr,
  uint16 bytes_remaining
)
{
  // Convert the dump tag ID into an index
  uint8 dump_tag_def_index = (uint8)dump_tag;

  // Look-up the definition of the dump tag
  rr_dump_tag_def_s *dump_tag_def_ptr = &dump_tag_def[dump_tag_def_index];

  // Check whether the size of the dump tag data fits the bytes remaining in the buffer
  if (dump_tag_def_ptr->size_bytes <= bytes_remaining)
  {
    // Load pointer to dump tag data
    uint8 *dump_tag_data_ptr = (uint8 *)dump_tag_def_ptr->data_ptr;

    // Call the helper function to populate the header
    qsh_client_dump_tag_hdr_init(
      (qsh_dump_tag_hdr_s *)dump_tag_data_ptr,
      (qsh_dump_tag_t)dump_tag,
      (size_t)dump_tag_def_ptr->size_bytes
    );

    // Copy the data into the given buffer
    memscpy(
      buffer_ptr,
      bytes_remaining,
      dump_tag_data_ptr,
      dump_tag_def_ptr->size_bytes
    );

    // Return indicating the number of bytes written
    return dump_tag_def_ptr->size_bytes;
  }

  // Return indicating zero bytes wtirrwn
  return 0;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Performs the DUMP COLLECT call-back action
 *
 * \param params_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_dump_perform_cb_action(qsh_client_cb_params_s *cb_params_ptr)
{
  uint8 i;

  // Short-cut to dump collection params
  qsh_client_dump_collect_s *dump_collect_params_ptr = &cb_params_ptr->action_params.dump_collect;

  // Bit-mask indicating dump tags required
  uint32 dump_tag_mask = (uint32)dump_collect_params_ptr->dump_tag_mask;

  // Count of bytes remaining, updated after the data for each dump tag is written to the buffer
  int16 bytes_remaining = (int16)dump_collect_params_ptr->dump_iovec.size_bytes;

  // Buffer address to write dump tag data
  uint8 *buffer_ptr = dump_collect_params_ptr->dump_iovec.addr;

  // Loop through the number of GRR defined dump tags
  for (i = 0; i < (uint8)GRR_QSH_DUMP_TAG_COUNT; i++)
  {
    grr_qsh_dump_tag_e dump_tag = (grr_qsh_dump_tag_e)i;

    // See if that dump tag is required
    if (QSH_DUMP_TAG_ENABLED(dump_tag_mask, dump_tag))
    {
      // Write that dump tag data to the buffer - this returns zero if it doesn't fit
      uint16 bytes_written = write_dump_tag_data_to_buffer(dump_tag, buffer_ptr, bytes_remaining);

      if (bytes_written > 0)
      {
        // Update the pointer to point to the next free space
        buffer_ptr += bytes_written;

        // Update the bytes remaining by subtracting the number of bytes written
        bytes_remaining -= bytes_written;
      }
      else
      {
        bytes_remaining = 0;
      }

      // If the buffer is full, exit
      if (bytes_remaining <= 0)
      {
        break;
      }
    }
  }

  // Indicate call-back action done
  {
    qsh_client_action_done_s action_done;

    qsh_client_action_done_init(&action_done);

    action_done.cb_params_ptr = cb_params_ptr;
    action_done.params.dump_collect.size_written_bytes = (buffer_ptr - dump_collect_params_ptr->dump_iovec.addr);
    action_done.action_mode_done = QSH_ACTION_MODE_DONE_SYNC;

    qsh_client_action_done(&action_done);
  }

  return;
}

/* --------------------------  Dump logging APIs  -------------------------- */

/*!
 * \brief Add a message entry into the QSH dump data
 *
 * \param timestamp
 * \param msg_set
 * \param msg_id
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_dump_log_msg(timetick_type timestamp, uint8 msg_set, uint32 msg_id, const gas_id_t gas_id)
{
  // SUB-ID as a look-up index
  uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  grr_qsh_dump_msg_buf_s *msg_buf_ptr = &rr_dump_tag_data[sub_index].msg_buf;

  switch (msg_set)
  {
    case MS_RR_L1:
    {
      msg_buf_ptr->gl1_msg_buf[msg_buf_ptr->gl1_msg_buf_index].timestamp = timestamp;
      msg_buf_ptr->gl1_msg_buf[msg_buf_ptr->gl1_msg_buf_index].seq_num = get_seq_num(timestamp);
      msg_buf_ptr->gl1_msg_buf[msg_buf_ptr->gl1_msg_buf_index].msg_id = msg_id;

      msg_buf_ptr->gl1_msg_buf_index++;

      if (msg_buf_ptr->gl1_msg_buf_index >= ARR_SIZE(msg_buf_ptr->gl1_msg_buf))
      {
        msg_buf_ptr->gl1_msg_buf_index = 0;
      }
      break;
    }

    case MS_MAC_RR:
    {
      msg_buf_ptr->gmac_msg_buf[msg_buf_ptr->gmac_msg_buf_index].timestamp = timestamp;
      msg_buf_ptr->gmac_msg_buf[msg_buf_ptr->gmac_msg_buf_index].seq_num = get_seq_num(timestamp);
      msg_buf_ptr->gmac_msg_buf[msg_buf_ptr->gmac_msg_buf_index].msg_id = msg_id;

      msg_buf_ptr->gmac_msg_buf_index++;

      if (msg_buf_ptr->gmac_msg_buf_index >= ARR_SIZE(msg_buf_ptr->gmac_msg_buf))
      {
        msg_buf_ptr->gmac_msg_buf_index = 0;
      }
      break;
    }

    case MS_MM_RR:
    {
      msg_buf_ptr->nas_msg_buf[msg_buf_ptr->nas_msg_buf_index].timestamp = timestamp;
      msg_buf_ptr->nas_msg_buf[msg_buf_ptr->nas_msg_buf_index].seq_num = get_seq_num(timestamp);
      msg_buf_ptr->nas_msg_buf[msg_buf_ptr->nas_msg_buf_index].msg_id = msg_id;

      msg_buf_ptr->nas_msg_buf_index++;

      if (msg_buf_ptr->nas_msg_buf_index >= ARR_SIZE(msg_buf_ptr->nas_msg_buf))
      {
        msg_buf_ptr->nas_msg_buf_index = 0;
      }
      break;
    }

    case MS_RRC_RR:
    {
      msg_buf_ptr->wrrc_msg_buf[msg_buf_ptr->wrrc_msg_buf_index].timestamp = timestamp;
      msg_buf_ptr->wrrc_msg_buf[msg_buf_ptr->wrrc_msg_buf_index].seq_num = get_seq_num(timestamp);
      msg_buf_ptr->wrrc_msg_buf[msg_buf_ptr->wrrc_msg_buf_index].msg_id = msg_id;

      msg_buf_ptr->wrrc_msg_buf_index++;

      if (msg_buf_ptr->wrrc_msg_buf_index >= ARR_SIZE(msg_buf_ptr->wrrc_msg_buf))
      {
        msg_buf_ptr->wrrc_msg_buf_index = 0;
      }
      break;
    }

    case MS_TDSRRC_RR:
    {
      msg_buf_ptr->trrc_msg_buf[msg_buf_ptr->trrc_msg_buf_index].timestamp = timestamp;
      msg_buf_ptr->trrc_msg_buf[msg_buf_ptr->trrc_msg_buf_index].seq_num = get_seq_num(timestamp);
      msg_buf_ptr->trrc_msg_buf[msg_buf_ptr->trrc_msg_buf_index].msg_id = msg_id;

      msg_buf_ptr->trrc_msg_buf_index++;

      if (msg_buf_ptr->trrc_msg_buf_index >= ARR_SIZE(msg_buf_ptr->trrc_msg_buf))
      {
        msg_buf_ptr->trrc_msg_buf_index = 0;
      }
      break;
    }

    case MS_MSGR_RR:
    {
      msg_buf_ptr->msgr_msg_buf[msg_buf_ptr->msgr_msg_buf_index].timestamp = timestamp;
      msg_buf_ptr->msgr_msg_buf[msg_buf_ptr->msgr_msg_buf_index].seq_num = get_seq_num(timestamp);
      msg_buf_ptr->msgr_msg_buf[msg_buf_ptr->msgr_msg_buf_index].msg_id = msg_id;

      msg_buf_ptr->msgr_msg_buf_index++;

      if (msg_buf_ptr->msgr_msg_buf_index >= ARR_SIZE(msg_buf_ptr->msgr_msg_buf))
      {
        msg_buf_ptr->msgr_msg_buf_index = 0;
      }
      break;
    }

    default:
    {
      MSG_GERAN_ERROR_1_G("QSH: Unknown msg_set %d", msg_set);
    }
  }
}

/*!
 * \brief Add an internal message entry into the QSH dump data
 *
 * \param timestamp
 * \param msg_set
 * \param msg_id
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_dump_log_imsg(timetick_type timestamp, uint8 msg_set, uint8 msg_id, const gas_id_t gas_id)
{
  // SUB-ID as a look-up index
  uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  grr_qsh_dump_msg_buf_s *msg_buf_ptr = &rr_dump_tag_data[sub_index].msg_buf;

  msg_buf_ptr->grr_imsg_buf[msg_buf_ptr->grr_imsg_buf_index].timestamp = timestamp;
  msg_buf_ptr->grr_imsg_buf[msg_buf_ptr->grr_imsg_buf_index].seq_num = get_seq_num(timestamp);
  msg_buf_ptr->grr_imsg_buf[msg_buf_ptr->grr_imsg_buf_index].msg_set = msg_set;
  msg_buf_ptr->grr_imsg_buf[msg_buf_ptr->grr_imsg_buf_index].msg_id = msg_id;

  msg_buf_ptr->grr_imsg_buf_index++;

  if (msg_buf_ptr->grr_imsg_buf_index >= ARR_SIZE(msg_buf_ptr->grr_imsg_buf))
  {
    msg_buf_ptr->grr_imsg_buf_index = 0;
  }
}

/*!
 * \brief Add an event entry into the QSH dump data
 *
 * \param timestamp
 * \param event
 * \param sm
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_dump_log_event(
  timetick_type timestamp,
  uint16 event,
  uint8 sm,
  uint8 state,
  const gas_id_t gas_id
)
{
  // SUB-ID as a look-up index
  uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  grr_qsh_dump_event_buf_s *event_buf_ptr = &rr_dump_tag_data[sub_index].event_buf;

  event_buf_ptr->event_buf[event_buf_ptr->event_buf_index].timestamp = timestamp;
  event_buf_ptr->event_buf[event_buf_ptr->event_buf_index].event = event;
  event_buf_ptr->event_buf[event_buf_ptr->event_buf_index].sm = sm;
  event_buf_ptr->event_buf[event_buf_ptr->event_buf_index].state = state;

  event_buf_ptr->event_buf_index++;

  if (event_buf_ptr->event_buf_index >= ARR_SIZE(event_buf_ptr->event_buf))
  {
    event_buf_ptr->event_buf_index = 0;
  }
}

/*!
 * \brief Initialisation function called from RR-QSH when dump functionality is required.
 */
__attribute__((section(".uncompressible.text")))
qsh_client_dump_info_s rr_qsh_dump_init(void)
{
  qsh_client_dump_info_s dump_info;

  // Zero data structure across all subs
  memset(rr_dump_tag_data, 0, sizeof(rr_dump_tag_data));

  rr_dump_tag_data_seq_num = 0;
  rr_dump_tag_data_timestamp = 0;

  dump_info.max_size_bytes = sizeof(rr_dump_tag_data);

  return dump_info;
}
#endif // FEATURE_QSH_DUMP

/* EOF */

