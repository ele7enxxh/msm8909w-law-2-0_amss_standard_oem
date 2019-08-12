#ifndef DS_DSD_FMC_IF_H
#define DS_DSD_FMC_IF_H
/*===========================================================================
                     D S _ D S D _ L T E S M . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2011 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_fmc_if.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/11    sk    Created Module 
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

/*===========================================================================

                         GLOBAL DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                        LOCAL FUNCTION DEFINITION

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_FMC_IF_FMC_AVAILABLE

DESCRIPTION   Utility function
 
DEPENDENCIES  None.

RETURN VALUE  SO MASK.

SIDE EFFECTS  NONE
===========================================================================*/
boolean ds_dsd_fmc_if_get_fmc_state(void);

#endif /* DS_DSD_FMC_IF_H */


