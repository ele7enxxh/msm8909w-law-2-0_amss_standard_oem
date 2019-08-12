#ifndef OBDAPP_P_H
#define OBDAPP_P_H

/*=============================================================================

         O N - B O A R D   D I A G N O S T I C S   A P P 


GENERAL DESCRIPTION
  Provides public functions of OBDAPP module

EXTERNALIZED FUNCTIONS
  obd_init()
    Initializes the OBDApp module
  obd_handle_new_state()
    Handles the next action that needs to be performed based on the state.


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012QUALCOMM Technologies, Inc(QTI).
All rights reserved.
QUALCOMM Technologies Confidential and Proprietary

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/obdapp/inc/obdapp_p.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/20/12   at      Cleanup before initial checkin. 
10/20/12   ab      File created.

=============================================================================*/

#include "rex.h"

/*===========================================================================
FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION OBD_INIT

DESCRIPTION
  Initialisation function of OBDAPP.. Initializes required varaibles, obdapp 
  context structure and any other registrations with Diag etc.. Checks if OBDAPP
  Keys are present with key hanlder otherwise OBDAPP doesn't go forward..
  

PARAMETER

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

COMMENTS
  None
===========================================================================*/
boolean obd_init(rex_tcb_type * gsdi_task_tcb_ptr);


/*=============================================================================
FUNCTION OBD_HANDLE_NEW_STATE

DESCRIPTION 
   Handles the next action that needs to be performed based on the global state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void obd_handle_new_state(void);


#endif /* OBDAPP_P_H */
