#ifndef TRM_INIT_H
#define TRM_INIT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

                Transceiver Resource Manager Init Header File

GENERAL DESCRIPTION

  This file provides management of the RF Rx/Tx resources


EXTERNALIZED FUNCTIONS

  trm_init() - Must be called before any other function, to initialize module.

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  trm_init() must be called before any other function.

  Before using any RF functions, a client must be granted permission via
  trm_request( ) or trm_request_and_notify().

  When the client is finished using the RF, it must release its hold on the
  RF chain either directly via a call to trm_release(), or indirectly by
  calling trm_reserve_at(), trm_request_and_notify(), or trm_request().


Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/trm_init.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------- 
03/09/2010   ag      Initial version.

============================================================================*/


/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include <rex.h>


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================
CALLBACK TRM_WAIT_CALLBACK_T

DESCRIPTION
  The prototype for a rex_wait type callback function.

  The callback may add additional signals to the wait_mask, and process
  those signals before returning.  For example, the callback may watch for
  dog report timer expiry signals, and report to the dog task in response
  to that signal.

  The callback may actually be "(trm_wait_callback_t) rex_wait".
  
DEPENDENCIES
  The callback will be called by trm_init when waiting for a read from NV.
  It will be called from the context of the task calling trm_init.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_wait_callback_t)
(
  /* The signal to wait for */
  rex_sigs_type                   wait_mask
);



/*============================================================================

FUNCTION TRM_INIT

DESCRIPTION
  Initialize the Transceiver Resource Manager
  
DEPENDENCIES
  Must be called before any other TRM function.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_init
(
  /* Calling task's TCB pointer */
  rex_tcb_type                  * tcb,

  /* Signal to wait for, for NV reading */
  rex_sigs_type                   sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  trm_wait_callback_t             wait
);


#endif /* TRM_INIT_H */

