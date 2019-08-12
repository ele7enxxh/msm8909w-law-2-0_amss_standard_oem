/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R     O E M    L A Y E R   I N T E R F A C E


GENERAL DESCRIPTION

  This module contains functions to send debug display information from the 
  HDR to the UI

EXTERNALIZED FUNCTIONS
  hdrlogoem_copy_debug_display_info() Copies debug display information  
  
  hdrlogoem_debug_display_oem()  Sends debug display information to UI

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrlogoem.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -------------------------------------------------------
12/19/2013   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/27/2009   wsh     Fixed compiler warning for ARM compiler
05/18/2004   ds      Implemented OEM info functionality for HDR log
05/14/2004   ds      Created this file
==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "comdef.h"
#include "hdrlogoem.h"
#include <string.h>
#include "custhdr.h"


/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

extern hdrlog_debug_display_struct_subtype    hdr_debug_disp;
  /* This is the global structure to store the debug display info. This
     needs to be passed on to the OEM layer for display on the UI */


/* <EJECT> */

/*=========================================================================

FUNCTION     : HDRLOGOEM_DEBUG_DISPLAY_OEM

DESCRIPTION  : Sends the debug display record to UI. It copies
               the latest debug display structure information into a 
               structure to be used by UI.

DEPENDENCIES : None

PARAMETERS   : ptr - pointer to the debug display record
  
RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrlogoem_debug_display_oem 
( 
  hdrlog_debug_display_struct_subtype *ptr 
)
{
 
/*- - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_MEMCPY_REMOVAL
  *ptr = hdr_debug_disp;
#else
  memcpy( (void * ) ( ptr ),
          ( void * ) ( &hdr_debug_disp),
          sizeof ( hdr_debug_disp ) ); 
#endif /* FEATURE_MEMCPY_REMOVAL */ 

}/* hdrlogoem_debug_display_oem() */

/* <EJECT> */

