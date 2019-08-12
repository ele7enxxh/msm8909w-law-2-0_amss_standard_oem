#ifndef __SDCC_OSAL_BOOT_H
#define __SDCC_OSAL_BOOT_H
/******************************************************************************
 * sdcc_osal_boot.h
 *
 * This file provides SDCC OS Abstraction layer definitions for Non-OS(BOOT).
 *
 * Copyright (c) 2009-2013
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_osal_boot.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
2013-08-12   bn      Code Clean up
2009-07-24   vj      undef the intlocks for compiling in DBL
2009-04-01   sp      Include sdcc_boot_init and add HWIO dummy defines
2009-03-09   vj      Added boot support
2009-01-22   vj      Initial version.
=============================================================================*/

#define FEATURE_SDCC_FOR_BOOT
#include "comdef.h"
#include "HALhwio.h"
#include "string.h"

/*---------------------------------------------------------------------------
  Standard Types
---------------------------------------------------------------------------*/

/* The following definitions are the same accross platforms.  The first
   group are the sanctioned types.  */

typedef  unsigned char      boolean;     /* Boolean value type. */

typedef  unsigned long long uint64;      /* Unsigned 64 bit value */
typedef  unsigned int       uint32;      /* Unsigned 32 bit value */
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */

typedef  signed int         int32;       /* Signed 32 bit value */
typedef  signed short       int16;       /* Signed 16 bit value */
typedef  signed char        int8;        /* Signed 8  bit value */

#define sdcc_osal_sleep(ms)               sdcc_mdelay(ms)

#define sdcc_udelay(us)                   sdcc_bsp_hw_clk_busy_wait(us)
#define sdcc_mdelay(ms)                   sdcc_bsp_hw_clk_busy_wait(1000*ms)

#endif /* ifndef __SDCC_OSAL_BOOT_H */

