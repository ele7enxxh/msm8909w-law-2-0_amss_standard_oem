#ifndef GBA_RESPONSE_H
#define GBA_RESPONSE_H
/*===========================================================================


            G B A   R E S P O N S E   H E A D E R


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
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gba.h"
#include "gba_lib.h"

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_SEND_RESPONSE

DESCRIPTION
  This function will send a response to the client by invoking the callback.
  Then clears all data corresponding to the current command.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_send_response (
  gba_result_enum_type                 status,
  gba_response_data_type             * gba_resp_ptr
);

/*===========================================================================
FUNCTION GBA_RESPONSE_SAVE_CLIENT_DATA

DESCRIPTION
  Function saves the client data that needs to be saved throughout the
  duration of the command.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_response_save_client_data (
  gba_callback_type                  callback,
  const void                       * user_data_ptr
);

/*===========================================================================
FUNCTION GBA_RESPONSE_RESET

DESCRIPTION
  Function clears the saved response data.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_response_reset (
  void
);

#endif /* GBA_RESPONSE_H */

