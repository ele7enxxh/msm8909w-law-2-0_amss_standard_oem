#ifndef HDRERRNO_H
#define HDRERRNO_H
/*===========================================================================

           H D R   E R R O R   N U M B E R   D E F I N I T I O N S

DESCRIPTION
  This contains the definition of the HDR return codes (error numbers).
  Functions using this definition either return an error code, or set
  the global variable 'hdrerrno' to the appropriate value.
 

Copyright (c) 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrerrno.h_v   1.5   25 Sep 2001 15:32:58   aneufeld  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrerrno.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/11   smd     SU API cleanup.
08/08/07   wsh     Copied content of errno.h into this file to avoid filename
                   conflicts of errno.h in GSM build
09/25/01   ajn     Deprecated module.  Use errno.h instead.  This file exists
                   for backwards compatibility.
05/05/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#ifdef PLATFORM_LTK
#include <stdlib.h>
#endif

  /* Delegate hdr error numbers to errno.h */
#include "hdrcom_api.h"

/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/


#endif /* HDRERRNO_H */
