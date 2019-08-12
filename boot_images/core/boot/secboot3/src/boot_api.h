#ifndef BOOT_API_H
#define BOOT_API_H

/*===========================================================================

                   B o o t   A P I   H e a d e r  F i l e

DESCRIPTION
  This header file contains Boot API definitions used external to Boot
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2009-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/09  as       Fixing Compilation Error.
08/21/09  dk       Branched for cmi activities.
08/11/09  as       Creating external header file

============================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION  BOOT_ERR_FATAL

DESCRIPTION
  This function calls the SBL error handler.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void boot_err_fatal( void );

/*===========================================================================

FUNCTION  BOOT_HW_KICK_WATCH_DOG

DESCRIPTION
  This function will reset the WDOG counter.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void boot_hw_kick_watch_dog( void );

/* 
 * This should be the preferable Macro to be called by external module
 * which are executing in boot context. 
 */
#define BOOT_ERR_FATAL() boot_err_fatal()

/*
** This watchdog kick macro could be replaced by api later.
*/
#define BOOTHW_KICK_WATCHDOG() boot_hw_kick_watch_dog()
            


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif  /* BOOT_API_H */
