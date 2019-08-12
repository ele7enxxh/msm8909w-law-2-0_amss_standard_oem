#ifndef BOOT_SBL_SHARED_H
#define BOOT_SBL_SHARED_H

/*===========================================================================

                          Boot Shared Header File

GENERAL DESCRIPTION
 Contains common defintion used by BOOT module.

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_sbl_shared.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/15   tj      Added enum for device programmer
02/04/14   ck      Removed exit_ptr as it's not needed in Bear family
01/15/14   ck      Changed sbl_exit_ptr_type to uint64
12/02/13   ck      Added SBL - QSEE interface structure
09/01/11   dh      Remove miheader.h from include headers
08/04/10   aus     Merged the bl_shared_data structure for all SBLs
07/22/10   aus     Merged the bl_shared_data structure for sbl2 and sbl3
05/03/10   plc     Initial creation for secboot 3.0
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include BOOT_PBL_H
#include "boot_comdef.h"
#include "boot_msm.h"
#include "boot_sbl_qsee_interface.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Forward declarations so that we don't have to include the full
 * header file for each module here */
struct bl_shared_data_target_type;

/* Function pointer type that will transfer control to AMSS. */
typedef uint64 (*sbl_exit_ptr_type)(void*);

/* Path to execute determined by build flags */
typedef enum {
    SBL_BUILD_SBL,
    SBL_BUILD_DEVICEPROGRAMMER_LITE,
    SBL_BUILD_DEVICEPROGRAMMER_DDR
} sbl_build_type;


/*---------------------------------------------------------------------------
  SBL SHARED DATA STRUCTURE
  
  NOTE: Only data that needs to be shared between modules should go
        in this structure
---------------------------------------------------------------------------*/
typedef struct bl_shared_data_type
{  
  /* Data shared from previous SBL */
  struct boot_sbl_if_shared_info_type         *sbl_shared_data;

  /* Target-specific shared data structure, if any */
  struct bl_shared_data_target_type           *target;

  /* SBL - QSEE Interface Structure */
  struct boot_sbl_qsee_interface               sbl_qsee_interface;
  
    /* Path to execute determined by build flags */
  sbl_build_type                              build_type;
  
} bl_shared_data_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Macro :  sbl_kick_watchdog

** ==========================================================================
*/
/*!
* 
* @brief
*    Watchdog timer circuit reset macro.
* 
* @par Dependencies
*   The watchdog needs to be kicked at least once every TBDms.
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
#if !defined(BOOT_WATCHDOG_DISABLED)
#define SBL_KICK_WATCHDOG() \
          HWIO_OUT( WDOG_RESET, HWIO_FMSK( WDOG_RESET, WATCH_DOG ) )
#else
#define SBL_KICK_WATCHDOG()
#endif /* BOOT_WATCHDOG_DISABLED */
 
/*===========================================================================

**  Function :  sbl_loop_here

** ==========================================================================
*/
/*!
* 
* @brief
*  This function is called by boot_error_handler only. We perform
*  clean up of the registers and loop until JTAG is connected.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl_loop_here( void );

#endif /* BOOT_SBL_SHARED_H */
