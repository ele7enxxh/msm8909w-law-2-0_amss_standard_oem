#ifndef GBA_CMD_H
#define GBA_CMD_H
/*===========================================================================


            G B A   C O M M A N D   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/15   vdc     Added support for APN for bootstrapping
03/27/15   tl      Add support for GBA cancel
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"

#include "gba_lib.h"
#include "gba.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_BOOTSTRAP_DATA_TYPE

   DESCRIPTION:
     Contains the information required for GBA bootstrap command processing
-------------------------------------------------------------------------------*/
typedef struct {
  gba_naf_id_type                                     naf_id;
  gba_aid_type                                        aid;
  gba_session_type                                    session_type;
  gba_apn_type                                        apn_info;
  boolean                                             force_bootstrap;
} gba_bootstrap_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_CMD_TYPE

   DESCRIPTION:
     Contains information for handling GBA external commands
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                                         link;
  uint32                                              request_id;
  gba_callback_type                                   gba_cb_ptr;
  const void                                        * user_data_ptr;
  gba_bootstrap_data_type                             gba_bootstrap;
} gba_cmd_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_CMD_SET_SIG

DESCRIPTION
  This function sets the GBA command signal.

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_cmd_set_sig (
  void
);

/*===========================================================================
FUNCTION GBA_CMD_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in gba_main, is called to process the GBA command

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_cmd_handle_cmd_sig (
  void
);

/*===========================================================================
   FUNCTION:      GBA_QUEUE_CMD

   DESCRIPTION:
     This function allows client to queue a GBA command to be processed
     in GBA task context.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type  gba_queue_cmd (
  gba_cmd_type         * task_cmd_ptr
);

/*===========================================================================
FUNCTION GBA_CMD_CLEANUP

DESCRIPTION
  This function flushes the gba cmd queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_cmd_cleanup (
  void
);

/*===========================================================================
FUNCTION GBA_CMD_INIT

DESCRIPTION
  This function, called in gba_main, is called to initialize the command
  module.

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_cmd_init (
  void
);

/*===========================================================================
FUNCTION GBA_CMD_CANCEL

DESCRIPTION
  Command cancels a command currently running in the queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_cmd_cancel (
  uint32                             request_id
);

#endif /* GBA_CMD_H */

