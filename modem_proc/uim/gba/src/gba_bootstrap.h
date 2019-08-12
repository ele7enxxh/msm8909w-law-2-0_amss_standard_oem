#ifndef GBA_BOOTSTRAP_H
#define GBA_BOOTSTRAP_H
/*===========================================================================


            G B A   B O O T S T R A P   H E A D E R


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
07/09/15   tkl     Check EF GBABP for Ks validity
05/12/15   tkl     Moved request_id up to callback function
04/06/15   tl      Add support for GBA_U
03/27/15   tl      Add support for GBA cancel
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"

#include "gba.h"
#include "gba_cmd.h"
#include "gba_lib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

#define GBA_INVALID_REFERENCE_ID         0

/* ----------------------------------------------------------------------------
   ENUM:      GBA_BOOTSTRAP_CMD_ENUM_TYPE

   DESCRIPTION:
     Indicates the bootstrapping command to be processed.
-------------------------------------------------------------------------------*/
typedef enum {
  GBA_RETRIEVE_IDENTITY_CMD,
  GBA_RETRIEVE_KS_VALIDITY_CMD,
  GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD,
  GBA_UICC_AUTHENTICATE_RESPONSE_CMD,
  GBA_HTTP_RESPONSE_FINAL_CMD,
  GBA_GEN_KEY_CMD
} gba_bootstrap_cmd_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_BOOTSTRAP_CALLBACK_DATA_TYPE

   DESCRIPTION:
     Contains the bootstrap callback data.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_bootstrap_cmd_enum_type                         cmd_type;
  union {
    gba_identity_data_type                            identity;
    gba_ks_validity_data_type                         ks_validity;
    gba_http_authenticate_data_type                   auth;
    gba_auth_status_data_type                         auth_status;
    gba_http_200_ok_data_type                         http_200_ok;
    gba_ks_naf_type                                   ks_ext_naf;
    gba_ks_naf_type                                   tmpi_ks_naf;
  } data;
} gba_bootstrap_callback_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_BOOTSTRAP_CMD_TYPE

   DESCRIPTION:
     Contains information for handling GBA commands
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                                         link;
  gba_result_enum_type                                status;
  uint32                                              reference_id;
  gba_bootstrap_callback_data_type                    cmd_data;
} gba_bootstrap_cmd_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      GBA_BOOTSTRAP_CALLBACK_TYPE
-----------------------------------------------------------------------------*/
/** @brief Bootstrap callback function.

    The parameter to the callback function contains the results of the
    GBA bootstrap procedural command.
*/
typedef void (*gba_bootstrap_callback_type) (gba_result_enum_type                     status,
                                             const gba_bootstrap_callback_data_type * resp_ptr,
                                             uint32                                   reference_id);

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_RESET

DESCRIPTION
  Function resets the global bootstrap data.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_reset (
  void
);

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_EXTERNAL_CMD

DESCRIPTION
  Entry function into the GBA bootstrap procedure/state machine. Performs
  some basic initalization before triggering the GBA MMGSDI module to
  retrieve the IMPI.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_bootstrap_process_external_cmd (
  gba_bootstrap_data_type             * gba_bootstrap_ptr,
  uint32                                request_id
);

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_SESSION_CLEANUP

DESCRIPTION
  This function will clean the application specific information from
  the storage module and if the application being cleared matches the
  application on which the current GBA procedure is being run, the
  cached identity information will be cleared as well.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_session_cleanup (
  uint32                                reference_id,
  gba_session_type                      session_type
);

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in gba_main, handles all bootstrap command
  processing for each GBA command. This function will also maintain
  the state machine and handle invalid states appropriately.

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_handle_cmd_sig (
  void
);

/*===========================================================================
   FUNCTION:      GBA_QUEUE_BOOTSTRAP_CMD

   DESCRIPTION:
     This function queues commands to GBA to processes the bootstrap procedure.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type  gba_queue_bootstrap_cmd (
  gba_bootstrap_cmd_type         * task_cmd_ptr
);

/*===========================================================================
FUNCTION GBA_BOOT_STRAP_CLEANUP

DESCRIPTION
  This function flushes the gba bootstrap queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_cleanup (
  void
);

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_INIT

DESCRIPTION
  This function, called in gba_main, is called to initialize the bootstrap
  module and globals.

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_init (
  void
);

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_CANCEL

DESCRIPTION

DEPENDENCIES
  Nonce

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_bootstrap_cancel (
  uint32                    request_id
);

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_GET_REQUEST_ID

DESCRIPTION
  Returns a valid command request id.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 gba_bootstrap_get_request_id (
  void
);

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_GET_CURRENT_REFERENCE_ID

DESCRIPTION
  Returns the reference id in progress.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 gba_bootstrap_get_current_reference_id (
  void
);

#endif /* GBA_BOOTSTRAP_H */

