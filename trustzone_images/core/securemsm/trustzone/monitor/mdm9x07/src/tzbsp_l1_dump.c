/**
 * @file tzbsp_l1_dump.c
 * @brief Provides L1 d-cache and i-cache dumping related functionality.
 */

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/mdm9x07/src/tzbsp_l1_dump.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
02/29/12   tk       First version. And what a date.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <string.h>
#include <HALhwio.h>

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
#include "tzbsp_hwio.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/** Magic number for the L1 dump buffer. Magic as bytes ['Q', 'A', 'L', '1'], a
 * short for Qualcomm Apps Level 1. */
#define TZBSP_L1_DUMP_MAGIC        0x314C4151

/** Current L1 dump buffer version, which is just an integer incremented for
 * each new version. */
#define TZBSP_L1_DUMP_VERSION      1

/** Status bits for the L1 dump call context, for \c tzbsp_l1_dump_buf_t field
 * \c flags. */
enum {
  /** L1 dump was done in WDT bark context. */
  TZBSP_L1_DUMP_FLAGS_WDT_BIT = 0x01,
  /** L1 dump was done by HLOS/non-secure request. */
  TZBSP_L1_DUMP_FLAGS_NS_BIT  = 0x02,
};

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** Structure for the L1 dump. */
typedef struct tzbsp_l1_dump_buf_s
{
  uint32 magic;   /**< Dump magic number, set to \c TZBSP_L1_DUMP_MAGIC. */
  uint32 version; /**< Dump version number, set to \c TZBSP_L1_DUMP_VERSION. */
  /** If context was WDT bark, bit [0]=1, if L1 dump was requested by HLOS, bit
   * [1]=1 */
  uint32 flags;
  uint32 cpu_count;  /**< The number of CPUs in the system. */
  uint32 i_tag_size; /**< Size of i-cache line tag in bytes. */
  /** Size of i-cache line in bytes. L0/L1 i-cache line size is double the
   * architectural size as both ICRDR0 and ICRDR1 are read for each i-cache
   * word. */
  uint32 i_line_size;
  uint32 i_num_sets;  /**< Number of i-cache sets. */
  uint32 i_num_ways;  /**< Number of ways in each i-cache set. */
  uint32 d_tag_size;  /**< Size of d-cache tag in bytes. */
  uint32 d_line_size; /**< Size of d-cache line in bytes. */
  uint32 d_num_sets;  /**< Number of d-cache sets. */
  uint32 d_num_ways;  /**< Number of ways in each d-cache set. */
  uint32 saw2_spm_ctl[TZBSP_CPU_COUNT_MAX];  /**< SAW2_SPM_CTL for each CPU. */
  uint32 spare[32 - TZBSP_CPU_COUNT_MAX];    /**< Spare fields for future use. */
  /**
   * L0/L1 dumps start from \c lines. Full CPU0 cache dump is the first, then
   * CPU1, and so forth. The layout of the cache dump for each CPU is as
   * follows:
   * - L0 i-cache dump.
   * - L1 i-cache dump.
   * - L0 d-cache dump.
   * - L1 d-cache dump.
   *
   * Both L0 i-cache and d-cache are 1/4th of the size of corresponding L1
   * caches.
   *
   * Formulas to calculate each dump size based on the fields in this
   * structure, the arithmetic is in bytes:
   *
   * The size of L0 i-cache dump per CPU:
   *   i0_dump_size = (i_tag_size + i_line_size) * i_num_sets
   *
   * The size of L1 i-cache dump per CPU:
   *   i1_dump_size = (i_tag_size + i_line_size) * i_num_sets * i_num_ways
   *
   * The size of L0 d-cache dump per CPU:
   *   d0_dump_size = (d_tag_size + d_line_size) * d_num_sets
   *
   * The size of d-cache dump per CPU:
   *   d1_dump_size = (d_tag_size + d_line_size) * d_num_sets * d_num_ways
   */
  uint8 lines[];
} tzbsp_l1_dump_buf_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/** L1 dump buffer address. The dump buffer is in non-secure memory. */
/* Not used yet
static tzbsp_l1_dump_buf_t* g_l1_buf_ns;
*/

/** Monitor will set this to 1, if it receives a L1 dump request from HLOS. */
volatile uint32 tzbsp_l1_dump_request_state;

/** Start address of the monitor stack area. */
extern uint8 MonitorStack$$Base[];

/* Putting the flags needed in L1 dumping function to non-cached area to ensure
 * any cache cleaning mistake doesn't break secure/non-secure state handling.
 * Any CPU can dump its L1, hence flag field allocated for each CPU. */
static __attribute__((section ("TZBSP_UNCACHED_ZI_VARS")))
uint32 g_l1_flags[TZBSP_CPU_COUNT_MAX];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Sets the L0/L1 dump IMEM buffer address. By default, the IMEM buffer address
 * should be initialized to zero so that restart where dump isn't present
 * can be detected.
 *
 * @param [in] addr  The address stored as the L0/L1 dump buffer address in
 *                   shared IMEM.
 */
void tzbsp_l1_dump_buf_set_imem_addr(uint32 addr)
{
  uint32* imem_addr = (uint32*) TZBSP_SHARED_IMEM_L1_DUMP_ADDR;
  *imem_addr = addr;
}

/**
 * Dumps the L1 d-cache and i-cache. This function is called from ISR contexts.
 * Either the call comes from WDT bark handler (FIQ context) or from TZ
 * monitor.
 *
 * @pre Interrupts must be disabled.
 */
void tzbsp_l1_dump(void)
{
  /* TODO_8x26 Implement. */
}

/**
 * System call for non-secure side to set the L1 dump buffer address.
 *
 * @param [in] addr   Address of the buffer, must be in non-secure memory.
 * @param [in] size   Size of the buffer in bytes.
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_set_l1_dump_buf(uint32 addr, uint32 size)
{
  /* TODO_8x26 Implement. */
  return -E_NOT_SUPPORTED;
}

/**
 * System call for non-secure side to query L1 dump buffer size.
 *
 * @param [in]  spare         Spare, should be zero.
 * @param [out] buf_size      Pointer in non-secure word to store the dump
 *                            buffer size in bytes.
 * @param [in]  buf_size_sz   Size of the response buffer.
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_query_l1_dump_buf_size(uint32 spare, uint32* buf_size,
                                 uint32 buf_size_sz)
{
  /* TODO_8x26 Implement. */
  return -E_NOT_SUPPORTED;
}
