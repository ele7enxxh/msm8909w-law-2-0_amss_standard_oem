#ifndef TZBSP_SHARED_IMEM_H
#define TZBSP_SHARED_IMEM_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_shared_imem.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/12   tk      First version.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <target.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
 * Target header definition \c SHARED_IMEM_TZ_BASE points to the start of
 * Shared IMEM allocated to RPM. TZ writes the address of TZ diagnostics/debug
 * area to the first four bytes in RPM area.
 */
#define TZBSP_SHARED_IMEM_DIAG_ADDR             (SHARED_IMEM_TZ_BASE + 0x00)

/**
 * The base address and size for TZ Shared IMEM area.
 */
#define TZBSP_SHARED_IMEM_TZ_AREA_BASE          (SHARED_IMEM_BASE + 0x734)
#define TZBSP_SHARED_IMEM_TZ_AREA_SIZE          512 /* In bytes. */

#define TZBSP_IMEM_ADDR(offset) \
        (TZBSP_SHARED_IMEM_TZ_AREA_BASE + (offset))

/**
 * Memory map for the TZ area in Shared IMEM. TZ area starts at \c
 * TZBSP_SHARED_IMEM_TZ_AREA_BASE.
 */
#define TZBSP_SHARED_IMEM_L1_DUMP_ADDR          TZBSP_IMEM_ADDR(0x00)

/*
 * Unused Hole of 16 bytes. Can be reused when needed. It was created due to
 * need for saving mpm sleep ticks for 8 cores now instead of 4
 */

/**
 * TZ IMEM area to be dumped to DDR, by SBL, in non-secure/debug DLOAD mode
 */
#define TZBSP_SHARED_IMEM_DUMP_MAGIC            TZBSP_IMEM_ADDR(0x14)
#define TZBSP_SHARED_IMEM_DUMP_IMEM_ADDR        TZBSP_IMEM_ADDR(0x18)
#define TZBSP_SHARED_IMEM_DUMP_DDR_ADDR         TZBSP_IMEM_ADDR(0x1C)
#define TZBSP_SHARED_IMEM_DUMP_SIZE             TZBSP_IMEM_ADDR(0x20)

/**
 * L1 and L2 cache states, used by SDI:
 *  - Magic to indicate the cache states are valid; ['L', 'C', 'V', 'S'] = 0x5356434C
 *  - L1: one bit per CPU indicating if the L1 is valid (e.g. bit[0] == ‘1’ if CPU0 L1 is valid)
 *  - L2: If bit[0] == '1', L2 is valid
 */
#define TZBSP_SHARED_IMEM_CACHE_MAGIC           TZBSP_IMEM_ADDR(0x24)
#define TZBSP_SHARED_IMEM_L1_VALIDITY           TZBSP_IMEM_ADDR(0x28)
#define TZBSP_SHARED_IMEM_L2_VALIDITY           TZBSP_IMEM_ADDR(0x2C)

/* 8 WORDs reserved for SDI */
#define TZBSP_SHARED_IMEM_SDI_RSRVD_0           TZBSP_IMEM_ADDR(0x30)
#define TZBSP_SHARED_IMEM_SDI_RSRVD_1           TZBSP_IMEM_ADDR(0x34)
#define TZBSP_SHARED_IMEM_SDI_RSRVD_2           TZBSP_IMEM_ADDR(0x38)
#define TZBSP_SHARED_IMEM_SDI_RSRVD_3           TZBSP_IMEM_ADDR(0x3C)

#define TZBSP_SHARED_IMEM_SDI_RSRVD_4           TZBSP_IMEM_ADDR(0x40)
#define TZBSP_SHARED_IMEM_SDI_RSRVD_5           TZBSP_IMEM_ADDR(0x44)
#define TZBSP_SHARED_IMEM_SDI_RSRVD_6           TZBSP_IMEM_ADDR(0x48)
#define TZBSP_SHARED_IMEM_SDI_RSRVD_7           TZBSP_IMEM_ADDR(0x4C)

/* 8 WORDs reserved for TZ and SBL communication for seccfg */
#define TZBSP_SHARED_SBL_ZERO_PERIPH_0          TZBSP_IMEM_ADDR(0x50)
#define TZBSP_SHARED_SBL_ZERO_PERIPH_1          TZBSP_IMEM_ADDR(0x54)
#define TZBSP_SHARED_SBL_ZERO_PERIPH_2          TZBSP_IMEM_ADDR(0x58)
#define TZBSP_SHARED_SBL_ZERO_PERIPH_3          TZBSP_IMEM_ADDR(0x5C)

/* MPM Sleep ticks for power collapse debug */
#define TZBSP_SHARED_IMEM_MPM_SLEEP_TICKS_0     TZBSP_IMEM_ADDR(0x60)
#define TZBSP_SHARED_IMEM_MPM_SLEEP_TICKS_1     TZBSP_IMEM_ADDR(0x64)
#define TZBSP_SHARED_IMEM_MPM_SLEEP_TICKS_2     TZBSP_IMEM_ADDR(0x68)
#define TZBSP_SHARED_IMEM_MPM_SLEEP_TICKS_3     TZBSP_IMEM_ADDR(0x6C)
#define TZBSP_SHARED_IMEM_MPM_SLEEP_TICKS_4     TZBSP_IMEM_ADDR(0x70)
#define TZBSP_SHARED_IMEM_MPM_SLEEP_TICKS_5     TZBSP_IMEM_ADDR(0x74)
#define TZBSP_SHARED_IMEM_MPM_SLEEP_TICKS_6     TZBSP_IMEM_ADDR(0x78)
#define TZBSP_SHARED_IMEM_MPM_SLEEP_TICKS_7     TZBSP_IMEM_ADDR(0x7C)

/* shared imem addr used by UEFI for saving the address of the UEFI 
 * structure for holding the TZ diag area and HASH of the address to validate 0x80 to 0xA4..
 */
#define TZBSP_UEFI_SHARED_DUMP_BUFFER_ADDR      TZBSP_IMEM_ADDR(0x80)
#define TZBSP_UEFI_SHARED_DUMP_BUFFER_ADDR_HASH TZBSP_IMEM_ADDR(0x84)  /* 32 Bytes - 0x84..0xA4 */

/*
 * Space given to hyp. This info is not used in tz image but kept here
 * to avoid conflicts
 */
#define TZBSP_SHARED_IMEM_HYP_LOG_ADDR          TZBSP_IMEM_ADDR(0xE0)
#define TZBSP_SHARED_IMEM_HYP_LOG_SIZE          TZBSP_IMEM_ADDR(0xE4)

/**
 * Address of the location of WDT bark context buffer in Shared IMEM. This
 * address is in the Apps area. */
#define TZBSP_APPS_WDT_CTX_BUFFER_ADDR          (SHARED_IMEM_BASE + 0x658)

#endif /* TZBSP_SHARED_IMEM_H */
