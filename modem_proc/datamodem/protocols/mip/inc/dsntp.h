#ifndef DS_NTP_H
#define DS_NTP_H

/*===========================================================================

        D A T A  S E R V I C E S  N T P  H E A D E R  F I L E
                   
DESCRIPTION
  This module contains functions that for converting a GPS format time stamp
  (returned by ts_get()) to a NTP format time stamp.

Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsntp.h_v   1.1   29 May 2001 20:47:26   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsntp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/01   jd      Added support for off-target (unix) builds
01/05/01   js      Created module.
===========================================================================*/
#include "comdef.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION DS_GET_NTP

DESCRIPTION
  This function converts the time stamp returned by ts_get (usually in GPS
  format) to NTP format.

DEPENDENCIES
  None. 

PARAMETERS
  This function takes two pointers to uint32. Any initial values can be can
  be passed in as they will be overwritten with the NTP value converted from
  the GPS time stamp.

  uint32 *secs   -  Ineteger part of the NTP format time stamp. 
  uint32 *msecs  -  Fraction part of the NTP format time stamp. 

RETURN VALUE
  NTP time stamp is returned via the intPart and fracPart pointers (with 
  each pointing to a uint32).

SIDE EFFECTS
  None.
===========================================================================*/
extern void ds_get_ntp 
(
  uint32 *secs,
  uint32 *msecs
);

#endif /* DS_NTP_H */
