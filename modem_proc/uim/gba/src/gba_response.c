/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   R E S P O N S E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA callback functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/15   tkl     Moved request_id up to callback function
05/07/15   lxu     Fix crash of refresh on EF GBABP
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"

#include "gba.h"
#include "gba_bootstrap.h"
#include "gba_lib.h"
#include "gba_platform.h"
#include "gba_response.h"
#include "gba_state.h"
#include "gba_timer.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

static gba_callback_type                  gba_callback                 = NULL;
static const void                       * gba_user_data_ptr            = NULL;

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
)
{
  if(gba_callback)
  {
    uint32   ref_id  = gba_bootstrap_get_current_reference_id();
    if(ref_id != GBA_INVALID_REFERENCE_ID)
    {
      gba_callback(status,
                   ref_id,
                   gba_resp_ptr,
                   gba_user_data_ptr);
    }
  }

  gba_clear_timer();
} /* gba_send_response */


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
)
{
  gba_callback = callback;
  gba_user_data_ptr = user_data_ptr;

  return GBA_SUCCESS;
} /* gba_response_save_client_data */


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
)
{
  gba_callback       = NULL;
  gba_user_data_ptr  = NULL;
} /* gba_response_reset */

