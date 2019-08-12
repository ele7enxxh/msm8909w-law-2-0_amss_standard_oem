#ifndef __NAND_CONFIG_H__
#define __NAND_CONFIG_H__

/*=======================================================================
 * FILE:        nand_config.h
 *
 * SERVICES:    Target specific tool configurations
 *
 * DESCRIPTION
 *   This file includes the correct hardware specific include files so that
 *   the definitions for accessing the NAND controller will be satisfied
 *   correctly on the target MSM
 *
 * Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/config_v2/nand_config.h#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ 
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/21/11     sv      watchdog changes for for 9x15 
 * 08/23/11     jz      Cleanup 
 * 09/21/10     sv      kick watchdog using register addr for 9x15 
 * 09/21/10     sv      Disable wdog when FLASH_WATCHDOG_DISABLED is defined
 * 06/10/10     sb      Disabling watchdog when BOOT_WATCHDOG_DISABLED is defined
 * 03/19/10     sb      Fix for KICK_WATCHDOG compilation error on MDM9K .
 * 02/03/10     op      Fix Header History alignment.
 * 01/12/10     op      Add common watchdog for all targets.
 * 12/05/09     op      Add T_MSM8650 flags to share KICK_WATCHDOG macro with boot code
 * 07/27/09     op      Support 8K target
 * 02/27/09     jz      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

/*---------------------------------------------------------------------------

                    MACRO KICK_WATCHDOG

This macro resets the watchdog timer circuit.
---------------------------------------------------------------------------*/
/* Need to include HALhwio and HALhwioMSSTimers headers inorder to have 
   common interface to access watchdog register */
#if !defined(BUILD_FOR_X86) && !defined(BOOT_WATCHDOG_DISABLED) && \
    !defined(IMAGE_APPS_PROC) && !defined(FLASH_WATCHDOG_DISABLED)
   #define  KICK_WATCHDOG() \
     *((uint32 *)FLASH_WATCHDOG_RESET_ADDR) = 1; /* RPM watchdog reset */
#else
  #define KICK_WATCHDOG()
#endif
#endif /* __NAND_CONFIG_H__ */
