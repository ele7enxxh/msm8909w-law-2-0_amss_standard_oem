#ifndef TZBSP_CHIPSET_H
#define TZBSP_CHIPSET_H

/**
@file tzbsp_chipset.h
@brief Provide the interface the chipset level API's that must be implemented
       to support the core TZBSP.
*/

/*===========================================================================
   Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_chipset.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
03/23/12   amen     Added tzbsp_video_config
09/12/11   nkazi    Added tzbsp_pmic_init.
10/22/10   cap      Initial version.

===========================================================================*/
#include <stddef.h>
#include <comdef.h>
#include "tz_syscall_pub.h"

/*----------------------------------------------------------------------------
 * Forward Declarations
 * -------------------------------------------------------------------------*/
typedef struct tzbsp_elf_info_s tzbsp_elf_info_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * The scope of a subsystem tear down.
 */
typedef enum
{
  TZBSP_TEAR_DOWN_FULL = 0, /* The entire subsystem is torn down. */
  TZBSP_TEAR_DOWN_CPU  = 1, /* Only subsystem CPU is torn down. */
} tzbsp_tear_down_scope_et;

/**
 * Brings up a subsystem. If this function succeeds, it doesn't indicate
 * whether the subsystem fully booted up correctly.
 *
 * @param [in] proc             Subsystem identifier.
 * @param [in] start_addr       Subsystem execution start address.
 *
 * @return \c E_SUCCESS if subsystem was started, error code otherwise.
 */
int tzbsp_subsys_bring_up(tz_pil_proc_e_type proc, uint32 start_addr);

/**
 * Tears down a subsystem.
 *
 * @param [in] proc             Subsystem identifier.
 * @param [in] scope            Scope of tear down,
 *                              @see \c tzbsp_tear_down_scope_et.
 *
 * @return \c E_SUCCESS if subsystem was started, error code otherwise.
 */
int tzbsp_subsys_tear_down(tz_pil_proc_e_type proc,
                           tzbsp_tear_down_scope_et scope);

/**
 * Checks whether TZ supports a subsystem.
 *
 * @return \c TRUE if TZ supports a subsystem for PIL, \c FALSE otherwise.
 */
boolean tzbsp_subsys_is_supported(uint32 proc);

/**
 * Checks whether TZ mandates TZ PIL for a subsystem.
 *
 * @return \c TRUE if TZ requires the use of TZ PIL due to XPU protection, \c
 * FALSE otherwise.
 */
boolean tzbsp_subsys_is_mandated(uint32 proc);

/**
 * Checks if the address range for a subsystem has been set
 *
 * @param [in] proc             Subsystem identifier.
 *
 * @return \c TRUE if subsystem address range set, \c FALSE otherwise
 */
boolean tzbsp_subsys_is_memory_range_set(tz_pil_proc_e_type proc);

/**
 * Sets the address range for a subsystem
 *
 * @param [in] proc             Subsystem identifier.
 * @param [in] start            Start address of subsystem memory (inclusive)
 * @param [in] end              End address of subsystem memory (exclusive)
 *
 * @return \c E_SUCCESS if subsystem address range updated, error otherwise:
 *         \c -E_NOT_SUPPORTED if subsystem is not supported
 *         \c -E_INVALID_ARG if start >= end
 *         \c -E_ADDR_IN_USE if address range is occupied by another subsystem
 */
int tzbsp_subsys_set_memory_range(tz_pil_proc_e_type proc, uint32 start, 
                                  uint32 end);

/**
 * Returns the start and end addresses of the given subsystem
 *
 * @param[in]   proc    Subsystem identifier.
 * @param[out]  start   Pointer to memory to store the start address
 * @param[out]  end     Pointer to memory to store the end address
 *
 * @return \c E_SUCCESS on success, negative error code otherwise.
 */
int tzbsp_subsys_get_memory_range(tz_pil_proc_e_type proc, uint32 *start,
                                  uint32 *end);

/**
 * Returns the start and end address of the relocatable PIL region
 *
 * @param[out]  start   Start address of the region, inclusive
 * @param[out]  end     End address of the region, exclusive
 *
 * @return  E_SUCCESS on success, negative error code otherwise
 */
int tzbsp_subsys_get_reloc_pil_range(uint32 *start, uint32 *end);

/**
 * Marks the start time of PIL operation. By default this is not compiled in.
 *
 * @param [in] command  The PIL operation command ID.
 * @param [in] subsys   Subsystem identifier.
 */
__weak void tzbsp_pil_timing_start(uint32 command, uint32 subsys);

/**
 * Marks the end time of PIL operation and prints the total operation time in
 * milliseconds to TZ logs. By default this is not compiled in.
 *
 * @param [in] command  The PIL operation command ID.
 * @param [in] subsys   Subsystem identifier.
 */
__weak void tzbsp_pil_timing_end(uint32 command, uint32 subsys);

/**
 * @brief Parse the elf_info structure pointer
 */
__weak int tzbsp_video_config(const tzbsp_elf_info_t* elf_info);

/**
 * @brief Parse the elf_info structure pointer
 */
__weak int tzbsp_vpu_config(const tzbsp_elf_info_t* elf_info);

/**
 * @brief Initialize the PRNG harware.
 */
void tzbsp_prng_init(void);

/**
 * @brief Init PMIC System.
 */
int tzbsp_pmic_init(void);

/** 
 * Initialize OCMEM
 */
int tzbsp_ocmem_init(void);


/**
 * Write to QFPROM row
 */
int tzbsp_fuse_write
(
  uint32  raw_row_address,
  uint32* row_data,
  uint32  bus_clk_khz,
  uint32* qfprom_api_status
);

/**
 * Read to QFPROM row
 */
int tzbsp_fuse_read
(
  uint32    row_address,
  int32     addr_type,
  uint32*   row_data,
  uint32*   qfprom_api_status
);

/**
 * Provide override for OVERRIDE_4 jtag fuse
 */
int tzbsp_jtag_override(uint32 debug);

/**
 * Checks if cache dumping is allowed.
 *
 * @return \c TRUE if fuse and debug settings allow memory dumping for debug
 *         purposes.
 */
boolean tzbsp_allow_cache_dumping(void);

/**
 * Checks if HLOS can affect the behaviour of the secure watchdog
 *
 * @return \c TRUE if debug settings allow HLOS to affect secure watchdog,
 *         \c FALSE otherwise
 */
boolean tzbsp_allow_sec_wdog_debug(void);

/**
 * Votable clocks. The values are bits that can be ORed together.
 */
typedef enum
{
  TZBSP_CLK_VOTE_PRNG       = 0x01,
  TZBSP_CLK_VOTE_APCS_HCLK  = 0x02,
  TZBSP_CLK_VOTE_ADM        = 0x04,
  TZBSP_CLK_VOTE_MSS_XPU    = 0x08, /**< For 8960/8930, ignored for 8064. */
  TZBSP_CLK_VOTE_MPM_AHB    = 0x10,
  TZBSP_CLK_VOTE_BAM        = 0x20,
  TZBSP_CLK_VOTE_BLSP1_AHB  = 0x40,
} tzbsp_clk_vote_et;

/**
 * Votes for clocks. The clock vote register is shared between Apps CPUs on
 * some targets, hence an API needs to be used for clock voting to avoid any
 * concurrent access issues to the vote register.
 *
 * @param [in] enabled  \c TRUE to enable the votes, \c FALSE to disable.
 * @param [in] vote     Clock bits to be voted, @see tzbsp_clk_vote_et.
 */
void tzbsp_clk_vote_set_enabled(boolean enabled, tzbsp_clk_vote_et vote);

#endif
