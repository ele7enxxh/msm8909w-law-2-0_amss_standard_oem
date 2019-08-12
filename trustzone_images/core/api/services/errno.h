#ifndef ERRNO_H
/*lint -e749 -e750 -e751*/
#define ERRNO_H
/*===========================================================================

               E R R O R   N U M B E R   D E F I N I T I O N S

DESCRIPTION
  This contains the definition of the return codes (error numbers).
  Functions using this definition either return an error code, or set
  a global variable to the appropriate value.
 

Copyright (c) 2000 - 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MM_DATA/vcs/errno.h_v   1.0   08 Aug 2002 11:19:54   akhare  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/errno.h#1 $ 
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/06    sb     Need to include stdlib.h for LTK (.Net) builds
03/07/06    rv     Temporary fix for M6280XXX3003 builds
15/01/06    gr     Replaced body of errno.h with inclusion of the IxErrno.h
                   file. This is to help avoid conflicts with system errno
                   files. 
04/30/03    gr     Added inclusion of errno_codes.h and definition of errno
                   macro.
04/23/02   rwh     Added the E_BAD_DATA value and grouping from secerrno.h
09/24/01   ajn     Renamed the file from hdrerrno.h to errno.h, since it
                   contains no HDR specific content and is useful for IS-95
                   A & B, and IS2000 1x.
05/05/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef PLATFORM_LTK
#include <stdlib.h>
#endif

#include "IxErrno.h"

#endif /* ERRNO_H */
