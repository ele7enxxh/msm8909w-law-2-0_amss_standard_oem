#ifndef SIMLOCK_MODEM_H
#define SIMLOCK_MODEM_H
/*===========================================================================


            S I M L O C K   D E F I N I T I O N S

                      A N D   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -----------------------------------------------------------
12/24/15   stv     Send events to client from engine msg handling 
07/22/15   lm      Include uim_stubs_sigs.h
06/29/15   stv     Start time in temp unlock req as current time
                   Graceful handling of PMIC reset during temp unlock
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Support for RSU in simlock
05/15/15   stv     SIMLOCK time module related changes
11/10/14   vv      Add support for setting emergency mode for all the RATs
11/05/14   am      Changes to eliminate memory leak
10/10/14   vv      Add support for remote simlock configuration
09/29/14   vv      Added support for the remote SFS
08/29/14   vv      Added support for temporary unlock
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
06/04/14   vv      Remove FEATURE_SIMLOCK
06/03/14   vv      Added support for OTA unlock
03/25/14   tl      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "simlock_common.h"
#include "simlock_modem_lib.h"
#include "simlock_modem_p.h"
#include "rex.h"
#include "queue.h"
#include "rcinit.h"
#include "simlock_time.h"
#include "simlock_time_gps.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* Need this as we are referring to UIM task handle */
#define UIM_SIMLOCK_TCB                   rcinit_lookup_rextask("simlock")

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define SIMLOCK_CMD_Q_SIG                    0x00000001
#define SIMLOCK_NV_READ_SIG                  0x00000002
#define SIMLOCK_REMOTE_SFS_RESPONSE_SIG      0x00000004
#define SIMLOCK_REMOTE_SFS_SANITY_TIMER_SIG  0x00000008
#define SIMLOCK_CM_RESPONSE_SIG              0x00000010
#define SIMLOCK_CM_RESPONSE_SANITY_TIMER_SIG 0x00000020
#define SIMLOCK_RCEVT_CM_TASK_READY_SIG      0x00000040
#define SIMLOCK_TASK_STOP_SIG                0x00004000
#define SIMLOCK_TIME_QMI_LOC_READY_SIG       0x00010000
#define SIMLOCK_TIME_QMI_LOC_RESPONSE_SIG    0x00020000

/* SIM Task priority should be reduced for time intensive command
   processing like unlock and lock CK as to not starve other tasks. */
#define SIMLOCK_TASK_REDUCED_PRIORITY             10

/* ===========================================================================
   MACRO:       SIMLOCK_ENTER_TASK_STATE_CRIT_SECT
   DESCRIPTION: To enter rex critical section for SIMLOCK task state
   ===========================================================================*/
#define SIMLOCK_ENTER_TASK_STATE_CRIT_SECT                                   \
        rex_enter_crit_sect(simlock_task_state_crit_sect_ptr);

/* ===========================================================================
   MACRO:       SIMLOCK_LEAVE_TASK_STATE_CRIT_SECT
   DESCRIPTION: To leave rex critical section for SIMLOCK task state
   ===========================================================================*/
#define SIMLOCK_LEAVE_TASK_STATE_CRIT_SECT                                   \
        rex_leave_crit_sect(simlock_task_state_crit_sect_ptr);

extern boolean                   simlock_task_stopped;

extern rex_crit_sect_type      * simlock_task_state_crit_sect_ptr;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_SCOPE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the scope of the command data,
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_ENGINE_CMD,
  SIMLOCK_REGISTER_CMD,
  SIMLOCK_OTA_UNLOCK_CMD,
  SIMLOCK_TEMPORARY_UNLOCK_TIMER_EXPIRY_CMD,
  SIMLOCK_SET_REMOTE_SFS_CMD,
  SIMLOCK_TIME_HANDLE_GPS_TIMER_EXIPIRY_CMD,
  SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD,
  SIMLOCK_RSU_CMD,
  SIMLOCK_TEMPORARY_UNLOCK_START_CMD,
  SIMLOCK_TEMPORARY_UNLOCK_BACKUP_TIME_UPDATE_CMD
} simlock_cmd_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_RSU_MESSAGE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the message types in RSU
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_RSU_GET_KEY_MSG,
  SIMLOCK_RSU_SET_CONFIGURATION_MSG
} simlock_rsu_message_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_REGISTER_MSG_REQ_TYPE

   DESCRIPTION:
-------------------------------------------------------------------------------*/
typedef struct{
  simlock_registration_enum_type             operation;
  simlock_registration_request_enum_type     request_type;
  union{
    simlock_evt_cb_type                      simlock_evt_cb;
    simlock_remote_sfs_request_cb_type       simlock_remote_req_cb;
  }cb_type;
} simlock_register_msg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_OTA_UNLOCK_MSG_REQ_TYPE

   DESCRIPTION:
     The structure contains OTA unlock client data
-------------------------------------------------------------------------------*/
typedef struct{
  simlock_ota_unlock_data_type     unlock_req;
  simlock_slot_enum_type           slot;
  simlock_ota_callback_type        simlock_ota_cb_ptr;
} simlock_ota_unlock_msg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TIMER_EXPIRY_MSG_REQ_TYPE

   DESCRIPTION:
     The structure contains temporary unlock timer expiry data
-------------------------------------------------------------------------------*/
typedef struct{
  simlock_slot_enum_type           slot;
  simlock_category_enum_type       category;
} simlock_timer_expiry_msg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_SET_REMOTE_SFS_MSG_REQ_TYPE

   DESCRIPTION:
     This structure is for the SIMLock request message to enable or disable
     the remote SFS storage
-------------------------------------------------------------------------------*/
typedef struct{
  simlock_remote_sfs_request_enum_type     request_type;
  simlock_set_remote_sfs_callback_type     remote_sfs_cb_ptr;
}simlock_set_remote_sfs_msg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_MSG_REQ_TYPE

   DESCRIPTION:
     This structure is for the handling the RSU request messages
-------------------------------------------------------------------------------*/
typedef struct{
  simlock_rsu_callback_type      rsu_cb_ptr;
  simlock_rsu_message_enum_type  msg_type;
  union {
    simlock_data_type            rsu_data;
  }message;
}simlock_rsu_msg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TASK_CMD_DATA_TYPE

   DESCRIPTION:
     The common command header for all commands received by SIMLOCK task
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                                         link;
  simlock_callback_type                               simlock_cb_ptr;
  void                                              * user_data_ptr;
  simlock_cmd_enum_type                               cmd_type;
  union {
    simlock_message_request_data_type                 engine;
    simlock_register_msg_req_type                     register_msg;
    simlock_ota_unlock_msg_req_type                   ota_unlock_msg;
    simlock_timer_expiry_msg_req_type                 timer_expiry_msg;
    simlock_set_remote_sfs_msg_req_type               remote_sfs_msg;
    simlock_time_source_time_msg_req_type             source_time_info_msg;
    simlock_rsu_msg_req_type                          rsu_msg;
  } data;
} simlock_task_cmd_type;

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION:      SIMLOCK_QUEUE_CMD

DESCRIPTION:
  This function allows client to put command into the SIMLOCK command queue
  specific to a SIMLOCK task

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  simlock_result_enum_type

  SIMLOCK_SUCCESS:          The command structure was properly generated
                            and queued onto the SIMLOCK Command Queue.

SIDE EFFECTS:
  None
===========================================================================*/
simlock_result_enum_type  simlock_queue_cmd(
  simlock_task_cmd_type * task_cmd_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_WAIT

DESCRIPTION
  Main wait routine for the SIMLOCK task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals). The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type simlock_wait
(
  rex_sigs_type requested_mask
);

/*===========================================================================
FUNCTION SIMLOCK_SEND_TEMPORARY_UNLOCK_EVENT

DESCRIPTION
  This function create an temporary unlock event message and call
  the event callback to send the information back to the client.

DEPENDENCIES
  Client must have registered for simlock event cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_send_temporary_unlock_event
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
);

/*===========================================================================
FUNCTION SIMLOCK_SEND_EVENT

DESCRIPTION
  This function creates a event message based on the message type
  in the engine command and calls the event callback to send the
  information back to the client.

DEPENDENCIES
  Client must have registered for simlock event cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_send_event(
  simlock_result_enum_type                  status,
  const simlock_message_request_data_type  *req_msg_ptr,
  const simlock_message_response_data_type *resp_data_ptr
);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#endif /* SIMLOCK_MODEM_H */
