/*===========================================================================

                  H D R   L 1   W R A P P E R   M O D U L E 


DESCRIPTION
  This file is the external interface exported by HRC L1

EXTERNALIZED FUNCTIONS (Global)
  HDRTASK_DRIVER_INIT()

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.


Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrl1.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/12   smd     Removed power strobe APIs.
08/28/09   wsh     CMI: Removing conditional defines from API headers
07/21/09   wsh     Created module for HDR task reated functionalites

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdrl1.h"
#include "hdrsrchutil.h"
#include "hdrts.h"

#include "hdrmdsp.h"
#include "hdrdec.h"
#include "hdrenc.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRL1_TIME_INVALIDATE

DESCRIPTION
  This function asserts that HDR CDMA time is no longer reliable.  1x CDMA
  time, or Time-of-Day should be used instead.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Time is marked as invalid.
===========================================================================*/
void hdrl1_time_invalidate( void )
{
  hdrts_invalidate();
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRL1_TIME_GET

DESCRIPTION
  This function gets the current HDR time.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if HDR Time is valid, and return the timestamp in 'ts_val'
  FALSE if HDR Time is invalid.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrl1_time_get
( 
  hdr_timestamp_t ts 
)
{
  return hdrts_get(ts);
}

