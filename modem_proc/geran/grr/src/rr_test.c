/*! \file rr_test.c

  This module contains SIM & loop-back test support.

                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_test.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_test.h"
#include "rr_l2.h"
#include "ms.h"
#include "rr_l2_send.h"
#include "msg.h"
#include "err.h"
#include "rr_general.h"
#include "rr_mm_send.h"
#include "rr_main.h"
#include "rr_ded_config.h"
#include "rr_sim.h"
#include "rr_multi_sim.h"

#ifndef PERLUTF
// Include definition of loopback types and gl1_msg_tch_loopback() prototype
#include "gl1_msg_g.h"
#else
#error code not present
#endif // PERLUTF

#ifdef FEATURE_CGPS_UMTS_CP_GSM
#define RR_NAS_RESET_GPS_MESSAGE_TYPE 0x48
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Loopback I is introduced in Rel5 spec.
bit 6 of subchannel octet is spare in Rel 99
bit 6 of subchannel octet is 0 in Rel 5 for all loopbacks A to F
bit 6 of subchannel octet is 1 in Rel 5 for loopback I
*/
static const gl1_defs_loopback_type loopback_type_table[] =
{                                             /* sub_channel */
   GL1_DEFS_LOOPBACK_TYPE_A,                  /* --00 000-   */
   GL1_DEFS_LOOPBACK_TYPE_B,                  /* --00 001-   */
   GL1_DEFS_LOOPBACK_TYPE_C,                  /* --00 010-   */
   GL1_DEFS_LOOPBACK_TYPE_C,                  /* --00 011-   */
   GL1_DEFS_LOOPBACK_TYPE_D,                  /* --00 100-   */
   GL1_DEFS_LOOPBACK_TYPE_D,                  /* --00 101-   */
   GL1_DEFS_LOOPBACK_TYPE_D,                  /* --00 110-   */
   GL1_DEFS_LOOPBACK_TYPE_D,                  /* --00 111-   */
   GL1_DEFS_LOOPBACK_TYPE_E,                  /* --01 000-   */
   GL1_DEFS_LOOPBACK_TYPE_E,                  /* --01 001-   */
   GL1_DEFS_LOOPBACK_TYPE_E,                  /* --01 010-   */
   GL1_DEFS_LOOPBACK_TYPE_E,                  /* --01 011-   */
   GL1_DEFS_LOOPBACK_TYPE_F,                  /* --01 100-   */
   GL1_DEFS_LOOPBACK_TYPE_F,                  /* --01 101-   */
   GL1_DEFS_LOOPBACK_TYPE_F,                  /* --01 110-   */
   GL1_DEFS_LOOPBACK_TYPE_F,                  /* --01 111-   */
   GL1_DEFS_LOOPBACK_TYPE_I,                  /* --11 100-   */
   GL1_DEFS_LOOPBACK_TYPE_I,                  /* --11 101-   */
   GL1_DEFS_LOOPBACK_TYPE_I,                  /* --11 110-   */
   GL1_DEFS_LOOPBACK_TYPE_I                   /* --11 111-   */
};

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  boolean                sim_present;
  gl1_defs_loopback_type loopback_type;
} rr_test_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_test_data_t rr_test_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static rr_test_data_t *rr_test_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_test_data[as_index];
}

/*!
 * \brief Determine the current operational mode, called when the SIM is inserted.
 *
 * \param gas_id
 */
static void rr_tm_handle_sim_in(
  const gas_id_t gas_id
)
{
  ms_operation_mode_T ms_operation_mode;
  rr_test_data_t *rr_test_data_ptr;

  rr_test_data_ptr = rr_test_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_test_data_ptr);

  MSG_GERAN_MED_0_G("RR: SIM Inserted");

  rr_test_data_ptr->sim_present = TRUE;

  ms_operation_mode = rr_siminfo_get_ms_opmode(gas_id);

  /*
  ** IF A NORMAL 'NETWORK' SIM HAS BEEN INSERTED,
  ** TERMINATE ALL LOOPBACK & TESTING MODES
  ** Also, Treat a failed SIM Read Operation
  ** as a normal operating mode sim insertion
  */
  if (ms_operation_mode == NORMAL_OPERATION)
  {
    rr_tm_open_tch_loop(gas_id);
  }

  MSG_GERAN_HIGH_1_G("RR: MS operation mode=%d", (int) ms_operation_mode);
}

/*!
 * \brief Function to set operations modes back to normal, called when the SIM is removed.
 *
 * \param gas_id (in)
 */
static void rr_tm_handle_sim_out(
  const gas_id_t gas_id
)
{
  ms_operation_mode_T ms_operation_mode;
  rr_test_data_t *rr_test_data_ptr;

  rr_test_data_ptr = rr_test_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_test_data_ptr);

  ms_operation_mode = rr_siminfo_get_ms_opmode(gas_id);

  MSG_GERAN_MED_0_G("RR: Sim Removed");

  rr_test_data_ptr->sim_present = FALSE;

  /*
  ** TERMINATE ALL LOOPBACK & TESTING MODES
  */

  rr_tm_open_tch_loop(gas_id);

  MSG_GERAN_HIGH_1_G("RR: MS operation mode=%d",(int) ms_operation_mode);
}

/*!
 * \brief Function to send a close_tch_ack to network/tester.
 *
 *
 * \param gas_id (in)
 */
static void rr_tm_send_close_tch_ack(
  const gas_id_t gas_id
)
{
  byte l3_msg[CLOSE_TCH_LOOP_ACK_LEN];

  l3_msg[0] = TEST_MANAGEMENT;
  l3_msg[1] = CLOSE_TCH_LOOP_ACK;

  (void) rr_send_dl_data_req(
    l3_msg,                   // layer3_message
    CLOSE_TCH_LOOP_ACK_LEN,   // length
    SAPI0,                    // SAPI
    DCCH,                     // l2_channel_type
    gas_id                    // gas_id
  );
}

/*!
 * \brief Function to send open loop command ack to network/tester.
 *
 * \param gas_id (in)
 */
static void rr_tm_send_open_tch_ack(
  const gas_id_t gas_id
)
{
  byte l3_msg[OPEN_LOOP_CMD_ACK_LEN];

  l3_msg[0] = TEST_MANAGEMENT;
  l3_msg[1] = OPEN_LOOP_CMD;
  l3_msg[2] = OPEN_LOOP_CMD_ACK;

  (void) rr_send_dl_data_req(
    l3_msg,                  // layer3_message
    OPEN_LOOP_CMD_ACK_LEN,   // length
    SAPI0,                   // SAPI
    DCCH,                    // l2_channel_type
    gas_id                   // gas_id
  );
}

/*!
 * \brief Closes the TCH loop.
 *
 * \param sub_channel (in)
 * \param gas_id (in)
 */
#ifndef PERLUTF
static
#endif /*PERLUTF*/
void rr_tm_close_tch_loop(
  byte sub_channel,
  const gas_id_t gas_id
)
{
  gl1_defs_loopback_type  loopback_type;
  boolean                 loopback_valid = TRUE;
  boolean                 sim_check_valid = FALSE;
  ms_operation_mode_T     ms_operation_mode;
  rr_test_data_t         *rr_test_data_ptr;

  rr_test_data_ptr = rr_test_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_test_data_ptr);

  ms_operation_mode = rr_siminfo_get_ms_opmode(gas_id);

  MSG_GERAN_HIGH_0_G("RR: close TCH loop");

  /*
  ** CHECK THAT A NORMAL 'NETWORK' SIM IS NOT PRESENT.
  *
  *  Loopback can be performed with a test SIM OR when no SIM is inserted.
  */

  /* 44.014 Sections 1 and 5.1.8 */
  if (rr_test_data_ptr->sim_present)
  {
    if ((int) ms_operation_mode & (int) TYPE_APPROVAL_OPERATIONS)
    {
      sim_check_valid = TRUE;
      MSG_GERAN_MED_0_G("RR Close TCH Loop - SIM present and Valid");
    }
    else
    {
      sim_check_valid = FALSE;
      MSG_GERAN_MED_0_G("RR Close TCH Loop - SIM present and Invalid");
    }
  }
  else
  {
    sim_check_valid = TRUE;
    MSG_GERAN_MED_0_G("RR Close TCH Loop - SIM not present");
  }

  if (sim_check_valid)
  {
    /*
    ** ARE WE ALREADY LOOPING BACK ?
    */

    if (rr_test_data_ptr->loopback_type == GL1_DEFS_NO_LOOPBACK)
    {
      channel_spec_T *chan_spec_ptr = rr_get_channel_spec(NULL, gas_id);

      /*
      ** WHICH TYPE OF LOOPBACK ?
      */
      byte loopback_table_index = (sub_channel & BAZY_MASK) / 2;

      if (loopback_table_index <= 15)
      {
        loopback_type = loopback_type_table[loopback_table_index];
      }
      else if ((loopback_table_index >= 28) && (loopback_table_index <= 31))
      {
        loopback_type = loopback_type_table[loopback_table_index - 12];
      }
      else
      {
        /* added this block to cope with LINT warning about non-initialised loopback_type */
        MSG_GERAN_ERROR_0_G("No loopback type : table index not valid");
        loopback_type = GL1_DEFS_NO_LOOPBACK;
        loopback_valid = FALSE;
      }

      /*
      Loopback D, E, F are only valid for regular Half Rate.
      If a TCH different from a TCH/HS is active, the MS shall ignore a
      CLOSE_TCH_LOOP_CMD message containing a request to close a Type D Loop.
      */
      if (
          (
           (loopback_type == GL1_DEFS_LOOPBACK_TYPE_D) ||
           (loopback_type == GL1_DEFS_LOOPBACK_TYPE_E) ||
           (loopback_type == GL1_DEFS_LOOPBACK_TYPE_F)
          )
          &&
          (
           (chan_spec_ptr != NULL) &&
           (chan_spec_ptr->channel_info_1_after.channel_type != HALF_RATE_TRAFFIC)
          )
         )
      {
        MSG_GERAN_ERROR_0_G("Loopback D,E,F are only valid for regular Half Rate");
        loopback_valid = FALSE;
      }
      /*
      If a TCH different from a TCH/AFS or TCH/AHS is active, the MS shall ignore a
      CLOSE_TCH_LOOP_CMD message containing a request to close a Type I Loop.
      */ 
      else if ((loopback_type == GL1_DEFS_LOOPBACK_TYPE_I) &&
               (!IS_AMR_CHANNEL_MODE(chan_spec_ptr->channel_mode_1))
              )
      {
        MSG_GERAN_ERROR_1_G("Loopback I is only valid for AMR Half or Full rate but channel mode has %d" ,chan_spec_ptr->channel_mode_1);
        loopback_valid = FALSE;
      }

      if (loopback_valid)
      {
        /*Tell NW that we received the loopback command*/
        rr_tm_send_close_tch_ack(gas_id);

        /*********************************************************/
        /* Blocking wait for T200. For loopback C we can't start */
        /* the loopback until we finish sending the ack, may be  */
        /* moved to L1                                           */
        /*********************************************************/
        rr_timed_wait(120);

        gl1_msg_tch_loopback(loopback_type, gas_id);

        rr_test_data_ptr->loopback_type = loopback_type;

#ifdef FEATURE_TC
        geran_send_cm_test_mode_on(gas_id);
#endif /* FEATURE_TC */
      } /* loopback_valid == TRUE */
    } /* there is no existing loopback */
    else
    {
      MSG_GERAN_ERROR_1_G("Loopback type(%d) is not NO_LOOPBACK",
                (int)rr_test_data_ptr->loopback_type);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Loopback failed, SIM check failed, ms_op_mode=%d",(int) ms_operation_mode);
  }
}

/*!
 * \brief Function to handle the test data message from the SS.
 *
 * \param msg (in)
 * \param gas_id (in)
 */
static void rr_tm_handle_data_ind(
  dl_data_ind_T * msg,
  const gas_id_t gas_id
)
{

  /*
  ** THE SYSTEM SIMULATOR HAS SENT A TEST MESSAGE TO THE MS
  */

  /*
  ** VALID MESSAGE ?
  */

  if (msg->layer3_message[0] == TEST_MANAGEMENT)
  {
    /*
    ** MESSAGE IS OK SO FAR
    */

    switch (msg->layer3_message[1])
    {
      case CLOSE_TCH_LOOP_CMD:
        rr_tm_close_tch_loop(
          msg->layer3_message[2],
          gas_id
        );
        break;
      case OPEN_LOOP_CMD:
        rr_tm_open_tch_loop(gas_id);
        break;
#ifdef FEATURE_CGPS_UMTS_CP_GSM
        /* forward this Assisted-GPS message to NAS */
      case RR_RESET_UE_POSITIONING_INFO:
        /* map to 3G GPS message type so that MM handles it */
        msg->layer3_message[1] = RR_NAS_RESET_GPS_MESSAGE_TYPE;
        rr_send_data_ind(
          msg->layer3_message,
          msg->l3_message_length,
          gas_id
        );
        break;
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
      default:
        MSG_GERAN_HIGH_1_G("Unhandled test mode message type 0x%02X",
                 (int)msg->layer3_message[1]);
        break;
    }
  }
}

/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

/*!
 * \brief Handler for test-related messages.
 *
 * \param new_message (in)
 * \param gas_id (in)
 */
void rr_handle_test_messages(
  void *new_message,
  const gas_id_t gas_id
)
{

  IMH_T* msg = new_message;

  /*
  ** READ TEST MESSAGES FROM THE SYSTEM SIMULATOR
  ** --------------------------------------------
  ** There are 4 types of messages passed to this module:
  **
  ** 1. SIM inserted/removed messages,
  ** 2. Test loopback messages,
  ** 3. Test DAI messages,
  ** 4. Test EMMI messages.
  */


  MSG_GERAN_MED_0_G("RR: Handling Test Messages");

  switch ( msg->message_set )
  {
    case MS_MM_RR:

      /*
      ** MESSAGE FROM MM
      */

      switch ( msg->message_id )
      {
        case RR_SIM_INSERTED_REQ:
          rr_tm_handle_sim_in(gas_id);
          break;
        case RR_INVALIDATE_SIM_DATA_REQ:
          rr_tm_handle_sim_out(gas_id);
          break;
        default:
          break;
      }
      break;

    case MS_RR_L2:

      /*
      ** MESSAGE FROM L2
      */

      switch ( msg->message_id )
      {
        case DL_DATA_IND:
          rr_tm_handle_data_ind((dl_data_ind_T*) new_message, gas_id);
          break;
        default:
          break;
      }
      break;

    default:
      break;
  }
}

/*!
 * \brief Opens the TCH loop.
 *
 * \param gas_id (in)
 */
void rr_tm_open_tch_loop(
  const gas_id_t gas_id
)
{
  rr_test_data_t *rr_test_data_ptr = rr_test_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_test_data_ptr);

  if (rr_test_data_ptr->loopback_type != GL1_DEFS_NO_LOOPBACK)
  {
    MSG_GERAN_HIGH_0_G("RR: Open TCH loop");

    gl1_msg_tch_loopback(GL1_DEFS_NO_LOOPBACK, gas_id);

    if (rr_test_data_ptr->loopback_type == GL1_DEFS_LOOPBACK_TYPE_C)
    {
      // Do not send if L2 link not present
      if (rr_get_link_status(gas_id) != NO_LINK)
      {
        rr_tm_send_open_tch_ack(gas_id);
      }
    }

#ifdef FEATURE_TC
    geran_send_cm_test_mode_off(gas_id);
#endif /* FEATURE_TC */

    rr_test_data_ptr->loopback_type = GL1_DEFS_NO_LOOPBACK;
  }
}

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
void rr_test_task_start_init(
  void
)
{
  // Zero the control/data structure(s) (across all data-spaces)
  memset(rr_test_data, 0, sizeof(rr_test_data));
}

/* EOF */

