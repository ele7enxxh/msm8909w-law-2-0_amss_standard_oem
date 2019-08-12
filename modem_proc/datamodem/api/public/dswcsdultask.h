#ifndef DSWCSDULTASK_H
#define DSWCSDULTASK_H
/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A
                           
                          U P L I N K   T A S K

                          H E A D E R   F I L E

DESCRIPTION
  This file contains data declarations and function prototypes for WCDMA 
  Circuit-Switched Data Uplink Task. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2009 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsdultask.h_v   1.6   06 May 2002 14:55:30   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dswcsdultask.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/09   sa     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION DSWCSD_UL_TASK

DESCRIPTION
  This is the entry point for the WCDMA Circuit-Switched Data Uplink Task. 
  This function contains the main processing loop that waits for events 
  (signals or commands) and dispatches each event to the appropriate entity 
  for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void  dswcsd_ul_task
(
  uint32 ignored
    /* lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task() 
    */
);

#endif /* DSWCSDULTASK_H */

