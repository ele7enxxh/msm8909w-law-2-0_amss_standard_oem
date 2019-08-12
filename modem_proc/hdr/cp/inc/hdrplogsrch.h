#ifndef HDRPLOGSRCH_H
#define HDRPLOGSRCH_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   P R O T O C O L    L O G G I N G   I N T E R F A C E    
              F U N C T I O N S   W I T H   S E A R C H E R 

GENERAL DESCRIPTION

   
EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS 

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004, 2005, 2006, 2007 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrplog.h_v   1.7   29 Oct 2002 16:38:08   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrplogsrch.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/28/07   rmg     Updated comment in hdrplog_srch_debug_display_info()
05/11/04   mpa     Created module
==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdrlogi.h"

/*==========================================================================


                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

                                                      
                                                      
                                                         
                                                      
/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/
 
/*===========================================================================

FUNCTION HDRPLOG_SRCH_DEBUG_DISPLAY_INFO

DESCRIPTION
  This function fills in protocol related debug information for the 
  HDR Debug UI screen.  The information is also logged by the searcher.
      
DEPENDENCIES
  None
    
PARAMETERS
  log_ptr - pointer to the debug display struct

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_srch_debug_display_info
(
  hdrlog_debug_display_struct_subtype   *log_ptr
);

/* <EJECT> */
#endif /* HDRPLOGSRCH_H */
