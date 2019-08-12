/**
 * @file tzbsp_ns_debug_fiq.c
 * @brief Non-secure side debug feature where FIQ invokes a vector in
 *        non-secure side.
 */

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/common/src/tzbsp_ns_debug_fiq.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
06/22/12   tk       First version.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <string.h>
#include "tzbsp_target.h"
#include "tzbsp_mem.h"
#include "tzbsp_sys.h"
#include "tzbsp_isr.h"
#include "tzbsp_errno.h"
#include "tz_syscall_pub.h"
#include "tzbsp_context_util.h"
#include "tzbsp_mmu_cache.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define TZBSP_NSDF_VERSION_MAJOR         1
#define TZBSP_NSDF_VERSION_MINOR         0

#define TZBSP_NSDF_FLAG_NS_BIT           0x1

#define TZBSP_NSDF_INT_NAME              "NSDF"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Structure for the context passed to non-secure side debug FIQ handler.
 */
typedef struct tzbsp_ns_fiq_ctx_s
{
  /** Major version number increments indicate that the entire structure has
   * changed and is no longer compatible with any of the previous versions. */
  uint16 version_major;
  /** Minor version number increments indicate that spare or status bits have
   * changed, but existing bits and fields have not changed. */
  uint16 version_minor;
  /** State of reserved status bits should be ignored. */
  uint32 status;
  /* Interrupted CPU context. */
  tzbsp_mon_cpu_ctx_t cpu_ctx;
  /* Spare registers. */
  uint32 spare0;
  uint32 spare1;
  uint32 spare2;
  uint32 spare3;
  uint32 spare4;
  uint32 spare5;
  uint32 spare6;
  uint32 spare7;
} tzbsp_ns_fiq_ctx_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

uint32 g_tzbsp_ns_debug_vector;
uint32 g_tzbsp_ns_debug_stack;
tzbsp_ns_fiq_ctx_t* g_tzbsp_ns_debug_ctx;
uint32 g_tzbsp_ns_debug_thumb;
uint32 g_tzbsp_ns_debug_intnum;

/* Snapshot of SCR at the context where FIQ handler was entered. Monitor
 * switches to SCR[NS]=0 for FIQ handling and for WDT context dumping we need
 * to know what was the SCR[NS] state when the FIQ triggered. */
extern uint32 tzbsp_scr[TZBSP_CPU_COUNT_MAX];

/* Warm boot status of each CPU. */
extern uint32 g_tzbsp_warm_boot[TZBSP_CPU_COUNT_MAX];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/* The debug FIQ is OEM specific.*/
extern boolean tzbsp_is_allowed_debug_int(uint32 intnum);
extern void tzbsp_mon_configure_smc_for_nfdbg(void);
extern void tzbsp_mon_configure_smc_for_normal(void);
extern boolean tzbsp_allow_ns_debug_fiq(void);
extern void tzbsp_mon_save_banked_gp_regs(uint32* buf);
extern void tzbsp_hand_control_to_ns_nfdbg(void);

/**
 * Creates the general purpose register context for non-secure side.
 */
static void tzbsp_nfdbg_construct_ns_ctx(void)
{
  uint32 cpu = tzbsp_cur_cpu_num();

  memset((void*)g_tzbsp_ns_debug_ctx, 0, sizeof(tzbsp_ns_fiq_ctx_t));

  g_tzbsp_ns_debug_ctx->version_major = TZBSP_NSDF_VERSION_MAJOR;
  g_tzbsp_ns_debug_ctx->version_minor = TZBSP_NSDF_VERSION_MINOR;

  if(tzbsp_scr[cpu] & 0x1)
  {
    /* Debug FIQ interrupted non-secure context. */
    g_tzbsp_ns_debug_ctx->status |= TZBSP_NSDF_FLAG_NS_BIT;
  }

  tzbsp_fiq_get_cpu_context(&g_tzbsp_ns_debug_ctx->cpu_ctx);

  /* Ensure context buffer goes to memory. */
  tzbsp_dcache_flush_region((void*)g_tzbsp_ns_debug_ctx,
                             sizeof(tzbsp_ns_fiq_ctx_t));
}

static void* tzbsp_nfdbg_isr(void* arg)
{
  uint32 cpu = tzbsp_cur_cpu_num();

  /* Debug FIQ should never be possible without installing a handler in TZ.
   * Checking the existense of the handler here anyway to guard against
   * programming errors. */
  if(!g_tzbsp_ns_debug_vector || !g_tzbsp_ns_debug_stack ||
     !g_tzbsp_ns_debug_ctx)
  {
    return arg;
  }

  /* Fuses and debug certificate configuration must allow debug FIQ. Debug FIQ
   * must be security controlled as TZ hands control to non-secure side while
   * FIQs are masked. */
  if(!tzbsp_allow_ns_debug_fiq())
  {
    return arg;
  }

  /* If debug FIQ woke up the CPU and caused a warm boot, then we just ignore
   * the FIQ as non-secure side isn't ready to serve the debug FIQ (i.e. MMU is
   * not set up yet). */
  if(g_tzbsp_warm_boot[cpu])
  {
    return arg;
  }

  /* Construct the context that gets passed to non-secure side in r0. */
  tzbsp_nfdbg_construct_ns_ctx();

  /* Switch through monitor needs to be handled differently for non-secure
   * debug FIQ. The context we jump into is generated on the fly. On SMC calls
   * from non-secure side we must only return to the TZ context. */
  tzbsp_mon_configure_smc_for_nfdbg();

  /* Hand control to the non-secure debug FIQ handler. */
  tzbsp_hand_control_to_ns_nfdbg();

  /* After non-secure side returns, execution will continue here. Restoring the
   * normal SMC handler operation. */
  tzbsp_mon_configure_smc_for_normal();

  return arg;
}

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * System call for configuring secure side FIQ debug interrupt and the
 * non-secure handler context.
 *
 * @param [in]  vector   Base address of the non-secure side handler.
 * @param [in]  stack    Base address of the non-secure side handler stack.
 * @param [out] ctx_buf  Address of the buffer where TZ will place the
 *                       interrupted context state.
 * @param [in]  ctx_size Context buffer size in bytes.
 * @param [in]  intnum   The debug interrupt number on QGIC.
 * @param [in]  flags    1 for edge triggered, 0 for level triggered.
 *
 * @return \c E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_nfdbg_config(const uint32* vector, const uint32* stack,
                       uint32* ctx_buf, uint32 ctx_size, uint32 intnum,
                       uint32 flags)
{
  uint32 isr_flags;

  if(!tzbsp_is_allowed_debug_int(intnum))
  {
    return -E_NOT_ALLOWED;
  }

  /* Only allow debug PPIs on CPU0. */
  if(TZBSP_INT_IS_PPI(intnum) && tzbsp_cur_cpu_num() != 0)
  {
    return -E_NOT_ALLOWED;
  }

  /* Special case: allow uninstalling the handler and FIQ if the vector is
   * NULL. Note that the interrupt number is checked to be ok above. */
  if(NULL == vector)
  {
    /* Make sure non-secure side doesn't cheat. The uninstalled interrupt
     * must not only be allowed, it must be the one previously installed. */
    if(g_tzbsp_ns_debug_intnum != intnum)
    {
      return -E_NOT_ALLOWED;
    }

    /* Intentionally wipe out the current handler context before unregister.
     * If undregister for some reason fails, it is better to have no debug FIQ
     * capability. */
    g_tzbsp_ns_debug_vector = NULL;
    g_tzbsp_ns_debug_stack  = NULL;
    g_tzbsp_ns_debug_ctx    = NULL;
    g_tzbsp_ns_debug_thumb  = 0;

    if(tzbsp_unregister_isr(g_tzbsp_ns_debug_intnum,
                            TZBSP_INTF_INT_FLAG_NON_SECURE) < 0)
    {
      return -E_FAILURE;
    }

    g_tzbsp_ns_debug_intnum = 0;
    return E_SUCCESS;
  }

  /* Don't allow installing a new handler unless old handler is uninstalled
   * first. */
  if(NULL != g_tzbsp_ns_debug_vector)
  {
    return -E_NOT_AVAILABLE;
  }

  if(!tzbsp_is_ns_range(vector, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  /* This check is not strictly needed for security as TZ will not follow this
   * address. This check is more for robustness (i.e. error codes preferred to
   * debugging XPU violations). */
  if(!tzbsp_is_ns_range(stack, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  if(!tzbsp_is_ns_range(ctx_buf, sizeof(tzbsp_ns_fiq_ctx_t)))
  {
    return -E_BAD_ADDRESS;
  }

  /* Context buffer has four byte aligned fields, enforce that alignment to
   * avoid potential data aborts when manipulatin the structure fields if the
   * compiler generates instructions that assume four byte alignment.
   *
   * The non-secure vector address isn't being checked as it is executed in
   * non-secure side. */
  if((uint32)ctx_buf != TZBSP_ALIGN_4BYTE((uint32)ctx_buf))
  {
    return -E_BAD_ADDRESS;
  }

  if(ctx_size < sizeof(tzbsp_ns_fiq_ctx_t))
  {
    return -E_INVALID_ARG;
  }

  /* Interrupt ok, try to install an ISR. */
  isr_flags = TZBSP_INTF_CPU0_ONLY |
                (flags & TZ_NS_DEBUG_FIQ_EDGE_TRIGGERED) ?
                  TZBSP_INTF_TRIGGER_EDGE: TZBSP_INTF_TRIGGER_LEVEL;

  if(tzbsp_register_isr(intnum, TZBSP_NSDF_INT_NAME, tzbsp_nfdbg_isr, NULL,
                        isr_flags, TRUE) < 0)
  {
    return -E_FAILURE;
  }

  g_tzbsp_ns_debug_vector = (uint32) vector;
  g_tzbsp_ns_debug_stack  = (uint32) stack;
  g_tzbsp_ns_debug_ctx    = (tzbsp_ns_fiq_ctx_t*) ctx_buf;
  g_tzbsp_ns_debug_thumb  = (flags & TZ_NS_DEBUG_FIQ_THUMB) ? 1: 0;
  g_tzbsp_ns_debug_intnum = intnum;

  return E_SUCCESS;
}

/**
 * System call for querying the minimum size of context buffer for non-secure
 * FIQ handler.
 *
 * @param [out] ctx_size   Pointer in non-secure word to store the context
 *                         buffer size in bytes.
 * @param [in]  rsp_size   Size of the response buffer.
 *
 * @return \c E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_nfdbg_ctx_size(uint32* ctx_size, uint32 rsp_size)
{
  if(rsp_size < sizeof(uint32))
  {
    return -E_INVALID_ARG;
  }

  if(!tzbsp_is_ns_range(ctx_size, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  *ctx_size = sizeof(tzbsp_ns_fiq_ctx_t);

  return E_SUCCESS;
}

/**
 * System call for querying whether an interrupt can be registered as the
 * non-secure FIQ debug interrupt.
 *
 * @param [in]  intnum    Number of the queried QGIC interrupt.
 * @param [out] verdict   Pointer in non-secure word to store the verdict.
 * @param [in]  rsp_size  Size of the response buffer.
 *
 * @return \c E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_nfdbg_is_int_ok(uint32 intnum, uint32* verdict, uint32 rsp_size)
{
  if(rsp_size < sizeof(uint32))
  {
    return -E_INVALID_ARG;
  }

  if(!tzbsp_is_ns_range(verdict, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  *verdict = tzbsp_is_allowed_debug_int(intnum);

  return E_SUCCESS;
}
