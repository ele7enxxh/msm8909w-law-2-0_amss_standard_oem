#ifndef DS707_JCDMA_APPRATE
#define DS707_JCDMA_APPRATE
/*===========================================================================


                     D S 7 0 7 _ J C D M A _ A P P R A T E
                     
GENERAL DESCRIPTION

 This file contains the API's and functionality to implement the JCDMA
 functionality for an application to set it's data rate.

 Copyright (c) 2002 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_jcdma_apprate.h_v   1.2   23 Oct 2002 10:09:20   sramacha  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/jcdma/inc/ds707_jcdma_apprate.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/30/11    ms     Global variable cleanup.
10/22/02    rsl    Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/10/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


/*===========================================================================
                         DECLARATIONS & TYPEDEFS
===========================================================================*/
typedef enum
{
  DS707_JCDMA_APPRATE_MIN = -1,         /* used only for bounds checking   */
  DS707_JCDMA_APPRATE_LOW,              /* lowspeed SO's only              */
  DS707_JCDMA_APPRATE_MED,              /* only Fwd SCH with SO 33         */
  DS707_JCDMA_APPRATE_HIGH,             /* allow fwd and rev SCH with SO 33*/
  DS707_JCDMA_APPRATE_MAX               /* used only for bounds checking   */ 
} ds707_jcdma_apprate_type;


/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
==========================================================================*/
/*===========================================================================
FUNCTION      DS707_JCDMA_APPRATE_INIT

DESCRIPTION   Called once, at phone startup. Initializes the app rate to
              the default (HIGH).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_apprate_init(void);

/*===========================================================================
FUNCTION      DS707_JCDMA_APPRATE_SET_RATE

DESCRIPTION   Called by application to set its data rate.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_apprate_set_rate
(
  ds707_jcdma_apprate_type rate
);

/*===========================================================================
FUNCTION      DS707_JCDMA_APPRATE_GET_RATE

DESCRIPTION   Called by application to get its data rate.

DEPENDENCIES  None

RETURN VALUE  ds707_jcdma_apprate_val

SIDE EFFECTS  None
===========================================================================*/
ds707_jcdma_apprate_type ds707_jcdma_apprate_get_rate
(
  void
);

#endif /* DS707_JCDMA_APPRATE */
