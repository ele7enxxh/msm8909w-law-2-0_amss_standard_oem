/*
@file tzbsp_boot.c
@brief Warm and cold boot SYSCALL support

*/
/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/boot/src/tzbsp_boot.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
05/17/10   tk       Initial version.

===========================================================================*/


/*===========================================================================

          INCLUDE FILES

============================================================================*/
#include "tzbsp.h"
#include "tzbsp_errno.h"
#include "tzbsp_log.h"
#include "tzbsp_mem.h"
#include "tzbsp_syscall.h"
#include "tzbsp_chipset.h"
#include "tzbsp_target.h"
#include "tzbsp_xpu.h"
#include "tzbsp_context_util.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_image_version.h"
#include "ClockPIL.h"
#include "tzbsp_fuseprov.h"
#include "tzbsp_config.h"

/** CPU1 initial boot. */
#define TZ_BOOT_ADDR_CPU1_COLD             0x00000001
/** CPU1 subsequent boots/resets. */
#define TZ_BOOT_ADDR_CPU1_WARM             0x00000002
/** CPU0 subsequent boots/resets. */
#define TZ_BOOT_ADDR_CPU0_WARM             0x00000004
/** CPU2 initial boot. */
#define TZ_BOOT_ADDR_CPU2_COLD             0x00000008
/** CPU2 subsequent boots/resets. */
#define TZ_BOOT_ADDR_CPU2_WARM             0x00000010
/** CPU3 initial boot. */
#define TZ_BOOT_ADDR_CPU3_COLD             0x00000020
/** CPU3 subsequent boots/resets. */
#define TZ_BOOT_ADDR_CPU3_WARM             0x00000040
/** CPU0 initial boot.
    CPU 0 has an out of place value here as ealier CPU0 was assumed to be the
    boot CPU and didn't need to setup a cold boot address. In multi-cluster
    configuration, any cluster can be boot cluster and hence CPU0 of any 
    cluster can be boot CPU
*/
#define TZ_BOOT_ADDR_CPU0_COLD             0x00000080
/** CPU4 initial boot. */
#define TZ_BOOT_ADDR_CPU4_COLD             0x00000100
/** CPU4 subsequent boots/resets. */
#define TZ_BOOT_ADDR_CPU4_WARM             0x00000200
/** CPU5 initial boot. */
#define TZ_BOOT_ADDR_CPU5_COLD             0x00000400
/** CPU5 subsequent boots/resets. */
#define TZ_BOOT_ADDR_CPU5_WARM             0x00000800
/** CPU6 initial boot. */
#define TZ_BOOT_ADDR_CPU6_COLD             0x00001000
/** CPU6 subsequent boots/resets. */
#define TZ_BOOT_ADDR_CPU6_WARM             0x00002000
/** CPU7 initial boot. */
#define TZ_BOOT_ADDR_CPU7_COLD             0x00004000
/** CPU7 subsequent boots/resets. */
#define TZ_BOOT_ADDR_CPU7_WARM             0x00008000


extern void tzbsp_set_ns_return_addr(uint32 addr);
extern void tzbsp_disable_ns_mmu_after_sbl(void);
extern int qsee_demand_paging_post_milestone_init(void);
extern boolean tzbsp_allow_memory_dump(void);
extern int affl_to_cpu_num(uint32 al0, uint32 al1, uint32 al2, uint32 al3);
extern void smem_tz_set_partitions(void);

__weak extern void qsee_chipset_post_milestone_init(void);

/* Make the non-secure workspace available for clearing the general
 * purpose registers */
extern tzbsp_mon_cpu_ctx_t Workspace_N_Begin;

/** HLOS start addresses. */
uint32 tzbsp_cpu_hlos_cold_start_addr[TZBSP_CPU_COUNT_MAX] = {0};
uint32 tzbsp_cpu_hlos_warm_start_addr[TZBSP_CPU_COUNT_MAX] = {0};
uint32 tzbsp_cpu_hlos_in_aarch64[TZBSP_CPU_COUNT_MAX] = {0};

boolean tzbsp_boot_milestone_status = FALSE;
boolean tzbsp_appsbl_milestone_status = FALSE;


static void tzbsp_set_boot_addr_loop(uint32 boot_addr,
                                     const uint32 affl0_bmsk,
                                     const uint32 affl1_bmsk,
                                     const uint32 affl2_bmsk,
                                     const uint32 affl3_bmsk,
                                     uint32 flags)

{
  uint32 a0_ii, a1_ii, a2_ii, a3_ii;
  uint32 a0_bm, a1_bm, a2_bm, a3_bm;

  for (a3_ii = 0, a3_bm = affl3_bmsk; a3_bm; a3_bm &= ~(1 << a3_ii++))
  {
    if (a3_bm & (1 << a3_ii))
    {
      for (a2_ii = 0, a2_bm = affl2_bmsk; a2_bm; a2_bm &= ~(1 << a2_ii++))
      {
        if (a2_bm & (1 << a2_ii))
        {
          for (a1_ii = 0, a1_bm = affl1_bmsk; a1_bm; a1_bm &= ~(1 << a1_ii++))
          {
            if (a1_bm & (1 << a1_ii))
            {
              for (a0_ii = 0, a0_bm = affl0_bmsk;
                   a0_bm;
                   a0_bm &= ~(1 << a0_ii++))
              {
                if (a0_bm & (1 << a0_ii))
                {
                  int cpu_ii = affl_to_cpu_num(a0_ii, a1_ii, a2_ii, a3_ii);

                  if (cpu_ii >= 0 && cpu_ii < g_tzbsp_cpu_count)
                  {
                    if (flags & TZ_BOOT_ADDR_COLD)
                    {
                      tzbsp_cpu_hlos_cold_start_addr[cpu_ii] = boot_addr;
                    }
                    if (flags & TZ_BOOT_ADDR_WARM)
                    {
                      tzbsp_cpu_hlos_warm_start_addr[cpu_ii] = boot_addr;
                    }
                    if (flags & TZ_BOOT_ADDR_AARCH64)
                    {
                      tzbsp_cpu_hlos_in_aarch64[cpu_ii] = TRUE;
                    }
                  }
                } /* affl0 check */
              } /* affl0 loop */
            } /* affl1 check */
          } /* affl1 loop */
        } /* affl2 check */
      } /* affl2 loop */
    } /* affl3 check */
  } /* affl3 loop */
}


/* Subroutine for handling the HLOS start address request.
 *
 * @param req Pointer to the request structure.
 *
 * @return \c E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_set_boot_addr(uint32 boot_addr, uint32 affl0_bmsk, uint32 affl1_bmsk,
                        uint32 affl2_bmsk, uint32 affl3_bmsk, uint32 flags)
{
  /* TODO: is_ns_range needs to be able to take 64 bit address
     eventually... */
  /* HLOS start address must be non-secure address space. */
  if (!tzbsp_is_ns_range((void*) boot_addr, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  /* One of these flags must be set */
  if (!(flags & TZ_BOOT_ADDR_COLD) && !(flags & TZ_BOOT_ADDR_WARM))
  {
    return -E_FAILURE;
  }

  /* Something must be set in each of the bitmasks, otherwise the loop
     makes no sense. */
  if (!affl0_bmsk || !affl1_bmsk || !affl2_bmsk || !affl3_bmsk)
  {
    return -E_FAILURE;
  }

  /* Loop through each affl bitmask and save the relevant info. */
  tzbsp_set_boot_addr_loop(boot_addr, affl0_bmsk, affl1_bmsk, affl2_bmsk,
                           affl3_bmsk, flags);

  /* Flush the cold start and warm start structures to ensure they are
   * available with the MMU off */
  tzbsp_dcache_flush_region(tzbsp_cpu_hlos_cold_start_addr,
                            sizeof(tzbsp_cpu_hlos_cold_start_addr));
  tzbsp_dcache_flush_region(tzbsp_cpu_hlos_warm_start_addr,
                            sizeof(tzbsp_cpu_hlos_warm_start_addr));
  tzbsp_dcache_flush_region(tzbsp_cpu_hlos_in_aarch64,
                            sizeof(tzbsp_cpu_hlos_in_aarch64));

  return E_SUCCESS;
}

/* Subroutine for handling the HLOS start address request.
 *
 * @param req Pointer to the request structure.
 *
 * @return \c E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_set_boot_addr_old(int flags, uint32 addr)
{
  /* HLOS start address must be non-secure address space. */
  if(!tzbsp_is_ns_range((void*)addr, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  if (flags & TZ_BOOT_ADDR_CPU0_COLD)
  {
    tzbsp_cpu_hlos_cold_start_addr[0] = addr;
  }
  if (flags & TZ_BOOT_ADDR_CPU0_WARM)
  {
    tzbsp_cpu_hlos_warm_start_addr[0] = addr;
  }

#if (TZBSP_CPU_COUNT_MAX > 1)
  if (flags & TZ_BOOT_ADDR_CPU1_COLD)
  {
    tzbsp_cpu_hlos_cold_start_addr[1] = addr;
  }

  if (flags & TZ_BOOT_ADDR_CPU1_WARM)
  {
    tzbsp_cpu_hlos_warm_start_addr[1] = addr;
  }
#endif

#if (TZBSP_CPU_COUNT_MAX > 2)
  if (flags & TZ_BOOT_ADDR_CPU2_COLD)
  {
    tzbsp_cpu_hlos_cold_start_addr[2] = addr;
  }

  if (flags & TZ_BOOT_ADDR_CPU2_WARM)
  {
    tzbsp_cpu_hlos_warm_start_addr[2] = addr;
  }
#endif

#if (TZBSP_CPU_COUNT_MAX > 3)
  if (flags & TZ_BOOT_ADDR_CPU3_COLD)
  {
    tzbsp_cpu_hlos_cold_start_addr[3] = addr;
  }

  if (flags & TZ_BOOT_ADDR_CPU3_WARM)
  {
    tzbsp_cpu_hlos_warm_start_addr[3] = addr;
  }
#endif

#if (TZBSP_CPU_COUNT_MAX > 4)
  if (flags & TZ_BOOT_ADDR_CPU4_COLD)
  {
    tzbsp_cpu_hlos_cold_start_addr[0] = addr; /* FIXME_8936. Temp hack for TZ & Kernel CPU NO count mismatch logic.*/
    tzbsp_cpu_hlos_cold_start_addr[4] = addr;
  }

  if (flags & TZ_BOOT_ADDR_CPU4_WARM)
  {
    tzbsp_cpu_hlos_warm_start_addr[4] = addr;
  }
#endif

#if (TZBSP_CPU_COUNT_MAX > 5)
  if (flags & TZ_BOOT_ADDR_CPU5_COLD)
  {
    tzbsp_cpu_hlos_cold_start_addr[5] = addr;
  }

  if (flags & TZ_BOOT_ADDR_CPU5_WARM)
  {
    tzbsp_cpu_hlos_warm_start_addr[5] = addr;
  }
#endif

#if (TZBSP_CPU_COUNT_MAX > 6)
  if (flags & TZ_BOOT_ADDR_CPU6_COLD)
  {
    tzbsp_cpu_hlos_cold_start_addr[6] = addr;
  }

  if (flags & TZ_BOOT_ADDR_CPU6_WARM)
  {
    tzbsp_cpu_hlos_warm_start_addr[6] = addr;
  }
#endif

#if (TZBSP_CPU_COUNT_MAX > 7)
  if (flags & TZ_BOOT_ADDR_CPU7_COLD)
  {
    tzbsp_cpu_hlos_cold_start_addr[7] = addr;
  }

  if (flags & TZ_BOOT_ADDR_CPU7_WARM)
  {
    tzbsp_cpu_hlos_warm_start_addr[7] = addr;
  }
#endif
  /* Flush the cold start and warm start structures to ensure they are
   * available with the MMU off */
  tzbsp_dcache_flush_region(tzbsp_cpu_hlos_cold_start_addr,
                            sizeof(tzbsp_cpu_hlos_cold_start_addr));
  tzbsp_dcache_flush_region(tzbsp_cpu_hlos_warm_start_addr,
                            sizeof(tzbsp_cpu_hlos_warm_start_addr));

  return E_SUCCESS;
}

/* Subroutine used by external bootlaoders to indicate end
 * of bootloading stage (begin HLOS). This call will handle
 * disabling the non-secure MMU and setting a new non-secure
 * return address
 *
 * @param jump_addr Address to return to after completing the Milestone call
 *
 * @return \c E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_milestone_set(uint32 jump_addr, uint32 mode)
{



  if(!tzbsp_is_ns_range((void*)jump_addr, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }



  tzbsp_boot_milestone_status = TRUE;

  /* allow the fuse provisioning service to check for sec.dat and blow fuses if needed */
  /* device will reset if any fuse is blown, in order for fuse to take affect */
  tzbsp_blow_fuses_and_reset();

  /* populate boot image version in the SMEM_IMAGE_VERSION_TABLE */
  if (E_SUCCESS != tzbsp_image_version_populate_version(TZBSP_IMAGE_INDEX_TZ))
  {
    /* Log the error, but continue on */
    TZBSP_ERROR_CODE(IMAGE_VERSION_POPULATE_FAILED);
  }

 
  tzbsp_disable_ns_mmu_after_sbl();
  tzbsp_set_ns_return_addr( jump_addr );

  /* Clear the SP registers */
  Workspace_N_Begin.usr_r13 = 0;
  Workspace_N_Begin.svc_r13 = 0;

  if(E_SUCCESS != qsee_demand_paging_post_milestone_init())
  {
    tzbsp_loop_here();
  }

  /* In dload mode, don't config smem prtns since static config is disabled */ 
  /* If device is in dload mode at this point, then customer is having dump feature in LK rather than SBL1*/
  if(FALSE == tzbsp_allow_memory_dump())
  {
 
    smem_tz_set_partitions();
  
  }

  qsee_chipset_post_milestone_init();

  return E_SUCCESS;
}

/* Subroutine to indicate the end of the appsbl stage, which is 
 * the beginning of HLOS booting.
 *
 * @param none.
 *
 * @return \c E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_appsbl_milestone_set(void)
{
  if(!tzbsp_boot_milestone_status)
  {
     /* Milestone is not set yet, return error code */
     return -E_NOT_ALLOWED;
  }

  tzbsp_appsbl_milestone_status = TRUE;

  return E_SUCCESS;
}
