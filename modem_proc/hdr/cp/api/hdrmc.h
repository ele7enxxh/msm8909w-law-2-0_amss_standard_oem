#ifndef HDRMC_H
#define HDRMC_H

/*===========================================================================

        E X T E R N A L   H D R   S U B T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains the external declarations for the HDRMC Task.

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrmc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/09   wsh     Prepare for CMI phase 2/3 integration
07/10/09   wsh     CMI: Hacks to support non-CMI TMC for integration
03/25/09   wsh     CMI: Updated featurization for CMI
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     Removed hdrenc from public AU interface 
02/18/09   wsh     Created hdrmc ext-AU module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#ifdef FEATURE_CMI

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

#else
#include "hdrmc_v.h"
#endif
#endif /* HDRMC_H */
