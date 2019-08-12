#ifndef LPA_CMD_H
#define LPA_CMD_H
/*===========================================================================


            L P A   C O M M A N D   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_cmd.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"

#include "lpa.h"
#include "lpalib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ENABLE_PROFILE_REQ_TYPE

   DESCRIPTION:
     LPA enable profile request type

-----------------------------------------------------------------------------*/
typedef struct {
  lpa_iccid_type                                iccid;
} lpa_enable_profile_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_DISABLE_PROFILE_REQ_TYPE

   DESCRIPTION:
     LPA disable profile request type

-----------------------------------------------------------------------------*/
typedef struct {
  lpa_iccid_type                                iccid;
} lpa_disable_profile_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_DELETE_PROFILE_REQ_TYPE

   DESCRIPTION:
     LPA delete profile request type

-----------------------------------------------------------------------------*/
typedef struct {
  lpa_iccid_type                                iccid;
} lpa_delete_profile_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_GET_EID_REQ_TYPE

   DESCRIPTION:
     LPA get EID request type

-----------------------------------------------------------------------------*/
typedef struct {
  lpa_iccid_type                                iccid;
} lpa_get_eid_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ADD_PROFILE_REQ_TYPE

   DESCRIPTION:
     LPA add profile request type

-----------------------------------------------------------------------------*/
typedef struct {
  lpa_iccid_type                                iccid;
  lpa_data_type                                 activation_code;
  lpa_data_type                                 confirmation_code;
} lpa_add_profile_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ADD_OR_UPDATE_PROFILE_NICKNAME_REQ_TYPE

   DESCRIPTION:
     LPA add or update profile nickname request type

-----------------------------------------------------------------------------*/
typedef struct {
  lpa_iccid_type                                iccid;
  lpa_name_type                                 nickname;
} lpa_add_or_update_profile_nickname_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_CMD_DATA_TYPE

   DESCRIPTION:
     LPA command type, to be put into the mmgsdi response command queue

-------------------------------------------------------------------------------*/
typedef struct {
  lpa_message_enum_type                         cmd_enum;
  union {
    /* Request */
    lpa_enable_profile_req_type                 enable_profile_req;
    lpa_disable_profile_req_type                disable_profile_req;
    lpa_delete_profile_req_type                 delete_profile_req;
    lpa_get_eid_req_type                        get_eid_req;
    lpa_add_profile_req_type                    add_profile_req;
    lpa_add_or_update_profile_nickname_req_type add_or_update_profile_nickname_req;
  } cmd;
} lpa_cmd_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_CMD_DATA_TYPE

   DESCRIPTION:
     The common command header for all commands received by LPA task
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                                   link;
  lpa_callback_type                             lpa_cb_ptr;
  lpa_event_callback_type                       event_cb_ptr;
  void                                         *user_data_ptr;
  lpa_slot_id_enum_type                         slot_id;
  lpa_cmd_data_type                             cmd;
} lpa_cmd_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION LPA_CMD_SET_SIG

DESCRIPTION
  This function sets the LPA command signal.

DEPENDENCIES
  LPA task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_cmd_set_sig (
);

/*===========================================================================
FUNCTION LPA_CMD_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in lpa_main, is called to process the LPA command

DEPENDENCIES
  LPA task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_cmd_handle_cmd_sig (
  void
);

/*===========================================================================
   FUNCTION:      LPA_CMD_QUEUE_CMD

   DESCRIPTION:
     This function allows client to queue a LPA command to be processed
     in LPA task context.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type  lpa_cmd_queue_cmd (
  lpa_cmd_type                          *lpa_cmd_ptr,
  lpa_slot_id_enum_type                  lpa_slot
);

/*===========================================================================
FUNCTION LPA_CMD_CLEANUP

DESCRIPTION
  This function flushes the lpa cmd queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_cmd_cleanup (
  lpa_slot_id_enum_type                  lpa_slot
);

/*===========================================================================
FUNCTION LPA_CMD_INIT

DESCRIPTION
  This function, called in lpa_main, is called to initialize the command
  module.

DEPENDENCIES
  LPA task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_cmd_init (
  void
);

#endif /* LPA_CMD_H */

