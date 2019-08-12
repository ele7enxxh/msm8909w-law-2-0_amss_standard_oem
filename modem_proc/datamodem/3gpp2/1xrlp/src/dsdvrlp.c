/*===========================================================================

  D A T A   S E R V I C E S   R L P   

DESCRIPTION
   This file contains functions for the
   Radio Link Protocol implentation that supports EVDV.


EXTERNALIZED FUNCTIONS
   dsdvrlp_init()
     Called at powerup to initialize RLP.


INITIALIZATION AND SEQUENCING REQUIREMENTS
   dsdvrlp_init() should be calle at powerup before any other RLP processing
   can be done.

Copyright (c) 1999-2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsdvrlp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/05   atp     Added include of msg.h and err.h.
05/07/04   atp     Initial version. First cut at supporting EVDV (IS707-B).

===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"

#include "dsdvrlp.h"
#include "dsdvrlpi_rx.h"
#include "dsdvrlp_rx.h"

/*===========================================================================
            EXTERNAL DEFINITIONS    
===========================================================================*/

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/



/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION DSDVRLP_INIT

DESCRIPTION
  Initializes RLP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlp_init
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

} /* dsdvrlp_init() */


/*===========================================================================

FUNCTION DSDVRLP_CLEANUP

DESCRIPTION
  Cleans up RLP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlp_cleanup
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
} /* dsdvrlp_cleanup() */



