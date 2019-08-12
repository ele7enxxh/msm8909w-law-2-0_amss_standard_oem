/*===========================================================================

                   P R E F E R R E D   U S E R   Z O N E   L I S T


DESCRIPTION
  This file makes up the PUZL component/module of the SD.

  This file implements the I-683B PUZL and encapsulates all the functionality
  required for SD.

  EXTERNALIZED FUNCTIONS

   puzl_init()
   puzl_find_record_with_sidnid()
   puzl_extract_uz_record()

INITIALIZATION AND SEQUENCING REQUIREMENTS
 SD20 module calls puzl_init() upon nam selection before calling other
 extern functions of PUZL.

Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/puzl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/12   gm      Mainline EFS feature - Remove FEATURE_EFS
02/10/11   am      Adding QTF support
05/01/11   rm      CMI-4: puzl.h changes
12/19/07   ka      Changes to support 16k PRL
05/09/06   jqi     Lint fixes.
05/04/06   jqi     Lint fixes.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/29/01   SJ      Initial release.

===========================================================================*/

/* ^L<EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */

/*===========================================================================
FUNCTION puzl_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
word puzl_dummy( word dummy_input )
{
  return dummy_input;
}
