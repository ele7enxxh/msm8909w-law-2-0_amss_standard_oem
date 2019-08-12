#ifndef NOR_CONFIG_H
#define NOR_CONFIG_H
/*=======================================================================
 * FILE:        nor_core.h
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * DESCRIPTION
 *   This files contains the hardware specific defines.
 *
 * Copyright(c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/config_v1/nor_config.h#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ 
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/07/11     jz      Removed wdog related include files due to hardcoded wdog addr
 * 06/22/11     eo      Use kick wdog register address on 9x15
 * 05/19/11     jz      Include msm.h to avoid missing HALhwioMSSTimers.h issue
 * 03/21/11     jz      Added handling of compile flag FLASH_WATCHDOG_DISABLED
 * 10/22/10     sv      Use HWIO for Watchdog
 * 09/02/10     sv      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/


#include "flash_prog.h"
  
#if !defined(BUILD_FOR_X86) && !defined(BOOT_WATCHDOG_DISABLED) && \
    !defined(IMAGE_APPS_PROC) && !defined(FLASH_WATCHDOG_DISABLED)
  /* Watchdog reset defined in ..\tools\src\nor\build\SConscript */
    #define  KICK_WATCHDOG() \
      *((uint32 *)FLASH_WATCHDOG_RESET_ADDR) = 1; /* RPM watchdog reset */
#else
  #define KICK_WATCHDOG()
#endif

/*
 * This macro is just a place holder.  Only used in ARMPRG
 */
#define  CHECK_FOR_DATA()

#define KICK_DOG_AND_CHECK_DATA()    \
  do {                     \
    KICK_WATCHDOG();   \
    CHECK_FOR_DATA();  \
  } while (0)
       
#endif /* NOR_CONFIG_H */
