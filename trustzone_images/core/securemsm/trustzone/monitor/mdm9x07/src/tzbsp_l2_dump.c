/**
 * @file tzbsp_l2_dump.c
 * @brief Provides L2 cache dumping related functionality.
 */

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/mdm9x07/src/tzbsp_l2_dump.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
03/22/12   tk       First version.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <string.h>

#include "tzbsp.h"
#include "tzbsp_chipset.h"
#include "tzbsp_syscall.h"
#include "tz_syscall_pub.h"
#include "tzbsp_errno.h"
#include "tzbsp_sys.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_cp15.h"
#include "tzbsp_mem.h"
#include "tzbsp_target.h"
#include "tzbsp_xpu.h"
#include "tzbsp_shared_imem.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define TZBSP_L2_DUMP_VERSION_NUMBER          0x1
#define TZBSP_L2_DUMP_MAGIC_NUMBER            0xcac1ecac

#define TZBSP_L2_WORD_FILLER                  0xdeadbeef

/* TODO_8x26 Needs Sparrow specific value. */
#define TZBSP_L2_CACHE_LINE_WORDS             (128/4)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Structure for L2 error status register dump. Identical to the SBL2 one.
 */
typedef struct tzbsp_l2_error_regs_s
{
  uint32 l2esr;
  uint32 l2esynr0;
  uint32 l2esynr1;
  uint32 l2ear0;
  uint32 l2ear1;
} tzbsp_l2_error_regs_t;

/**
 * Structure for L2 cache line dump. Identical to the SBL2 one.
 */
typedef struct tzbsp_l2_line_s
{
  uint32 l2dcrtr0;
  uint32 l2dcrtr1;
  uint32 line_data[TZBSP_L2_CACHE_LINE_WORDS];
  uint32 mem_data[TZBSP_L2_CACHE_LINE_WORDS];
} tzbsp_l2_line_t;

/**
 * Main structure for the L2 cache dump. Identical to the SBL2 one.
 */
typedef struct tzbsp_l2_dump_buf_s
{
  uint32 magic;
  uint32 version;
  uint32 tag_size; /* In bytes. */
  uint32 line_size; /* In bytes. */
  uint32 num_lines; /* Number of sets * number of lines. */
  tzbsp_l2_line_t lines[];
  /* After the lines come L2 error registers, @see
   * tzbsp_dump_l2_error_registers, @see tzbsp_l2_error_regs_t. */
} tzbsp_l2_dump_buf_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/**
 * L2 dump buffer address. The dump buffer is in non-secure memory. Note that
 * TZ doesn't store the L2 dump buffer address to Shared System IMEM. L2 dump
 * from TZ isn't something that is needed for watchdog bark memory dump
 * analysis.
 */
/* Not used yet
static tzbsp_l2_dump_buf_t* g_l2_buf_ns;
*/

/* Putting the flags needed in L2 dumping function to non-cached area to ensure
 * any cache cleaning mistake doesn't break secure/non-secure state handling.
 * Only a single CPU is supposed to do L2 dumping, but we cannot really trust
 * non-secure side to do that, hence allocating a flag field for each CPU. */
static __attribute__((section ("TZBSP_UNCACHED_ZI_VARS")))
uint32 g_l2_flags[TZBSP_CPU_COUNT_MAX];

/** Start address of the monitor stack area. */
extern uint8 MonitorStack$$Base[];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Dumps the L2 cache. This function is called from monitor context.
 *
 * L2 is a shared resource and there are multiple cores in the system that can
 * access L2 cache simultaneously. It is a requirement that HLOS ensures other
 * CPU cores or multimedia cores using SMMU L2 indirection don't interfere with
 * L2 cache dumping. If there are other cores modifying L2 cache contents
 * simultaneous to dumping, then the dumped result is most likely corrupted.
 *
 * @pre Interrupts must be disabled.
 */
void tzbsp_l2_dump(void)
{
  /* TODO_8x26 Implement. */
}

/**
 * System call for non-secure side to set the L2 dump buffer address.
 *
 * @param [in] addr   Address of the buffer, must be in non-secure memory.
 * @param [in] size   Size of the buffer in bytes.
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_set_l2_dump_buf(uint32 addr, uint32 size)
{
  /* TODO_8x26 Implement. */
  return -E_NOT_SUPPORTED;
}

/**
 * System call for non-secure side to query L2 dump buffer size.
 *
 * @param [in]  spare         Spare, should be zero.
 * @param [out] buf_size      Pointer in non-secure word to store the dump
 *                            buffer size in bytes.
 * @param [in]  buf_size_sz   Size of the response buffer.
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_query_l2_dump_buf_size(uint32 spare, uint32* buf_size,
                                 uint32 buf_size_sz)
{
  /* TODO_8x26 Implement. */
  return -E_NOT_SUPPORTED;
}
