#ifndef _DSDVRLP_H
#define _DSDVRLP_H

/*===========================================================================

  D A T A   S E R V I C E S   R L P   E X T E R N A L   H E A D E R  F I L E

DESCRIPTION
   This file contains external variable and function declarations for the
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/dsdvrlp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/04   atp     After lint.
06/08/04   atp     Support for upper bound on DDW and REXMIT_TIMER.
05/07/04   atp     Initial version. First cut at supporting EVDV (IS707-B).

===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "queue.h"


/*===========================================================================
                      EXTERNAL DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  There can be a maximum of 10 frames possible in a 1.25 ms slot on PDCH.
---------------------------------------------------------------------------*/
#define DSDVRLP_MAX_FR_IN_SLOT  10
/*---------------------------------------------------------------------------
  16 slots of 1.25ms make a 20ms interval.
---------------------------------------------------------------------------*/
#define DSDVRLP_MAX_FR_IN_20_MS 160

/*---------------------------------------------------------------------------
  DDW specified by the BS is upper bound. This is the max value we support.
---------------------------------------------------------------------------*/
#define DSDVRLP_MAX_DDW_SUPPORTED 600

/*---------------------------------------------------------------------------
  REXMIT specified by the BS is upper bound. This is the max value we support.
---------------------------------------------------------------------------*/
#define DSDVRLP_MAX_REXMIT_SUPPORTED 600

/*---------------------------------------------------------------------------
  Implementation defined alternative to DDW is to count number of encoder pkts
---------------------------------------------------------------------------*/
#define DSDVRLP_MAX_PKTS_RXED_AFTER 4000

/*===========================================================================
                      EXTERNAL FUNCTION DECLARATIONS
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
); 

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
);

#endif  /* _DSDVRLP_H */

