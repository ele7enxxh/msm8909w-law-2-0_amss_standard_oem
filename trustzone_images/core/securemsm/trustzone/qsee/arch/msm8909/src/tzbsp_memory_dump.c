/**
 * @file tzbsp_memory_dump.c
 * @brief Provides memory dumping related functionality.

*/

/*===========================================================================
   Copyright (c) 2011-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_memory_dump.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
10/04/11   mm       Added logs for Reset
02/25/11   tk       Initial version
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <HALhwio.h>
#include <HALqgic.h>
#include <string.h>
#include <busywait.h>
#include <string.h>
#include <stringl/stringl.h> /* For memscpy */

#include "tzbsp.h"
#include "tzbsp_chipset.h"
#include "tzbsp_syscall.h"
#include "tz_syscall_pub.h"
#include "tzbsp_errno.h"
#include "tzbsp_sys.h"
#include "tzbsp_mem.h"
#include "tzbsp_target.h"
#include "tzbsp_xpu.h"
#include "tzbsp_timer.h"
#include "tzbsp_log.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_diag.h"
#include "tzbsp_hwio.h"
#include "tzbsp_shared_imem.h"
#include "tzbsp_context_util.h"
#include "tzbsp_memory_dump.h"
#include "tzbsp_memory_map.h"
#include "tzbsp_config.h"
#include "crypto/tzbsp_hash.h"
#include "sha_shared.h"
#include "tzbsp_config.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Newer memory context dump buffer starts with a magic number to differentiate
 * it from the old buffer structure. Magic as bytes ['S', 'Y', 'D', 'B']
 */
#define TZBSP_DUMP_CTX_MAGIC        0x42445953

#define g_tzbsp_diag_len ((uint32) TZBSP_DIAG_SIZE)


/* Current memory context dump buffer version. Version is just a running
 * counter. */
#define TZBSP_DUMP_CTX_VERSION      0x11

/* Delay before the WDT bites and resets. In milliseconds. */
#define TZBSP_WDT_DELAY_MS          500

/* Status bits for the CPU status field in \c tzbsp_dump_buf_t. */
#define TZ_SC_STATUS_NS_BIT      0x01
#define TZ_SC_STATUS_WDT         0x02
#define TZ_SC_STATUS_SGI         0x04
/* If this flag is set, WDT bark occured during TZ warm boot. */
#define TZ_SC_STATUS_WARM_BOOT   0x08
#define TZ_SC_STATUS_DBI_RSVD    0x10  /* Reserved for Debug Image */
#define TZ_SC_STATUS_CTX_BY_TZ   0x20

/* Timeout, in us, to wait for QDSS TMCs to flush */
#define TZBSP_QDSS_TMC_US_TIMEOUT   100

/* Macro to wait for ETR or ETB to finish flushing */
#define TZBSP_QDSS_TMC_WAIT(exp)            \
  do {                                      \
    uint32 timeout;                         \
    for(timeout = TZBSP_QDSS_TMC_US_TIMEOUT;\
        timeout > 0 && (exp);               \
        timeout--)                          \
    {                                       \
      busywait(1);                          \
    }                                       \
  } while(0);

#define GET_SHARED_IMEM_ADDR()  (TZBSP_APPS_WDT_CTX_BUFFER_ADDR)

/* Magic indicating that the IMEM dump information, for SBL, is valid;
 * [‘T’, ‘Z’, ‘D’, ‘I’] */
#define TZBSP_IMEM_DUMP_MAGIC   0x49445a54

/* Cookie to indicate the XPU dump is valid; ['X', 'P', 'U', 'D'] */
#define XPU_DUMP_MAGIC    0x44555058

#define UPDATE_DEBUG_DUMP_FIELD(area_lc, area_uc, write_ptr, num_bytes) \
  if(num_bytes)                                                         \
  {                                                                     \
    tzbsp_debug_area->area_lc##_dump       = write_ptr;                 \
    tzbsp_debug_area->area_lc##_dump_size  = num_bytes;                 \
    tzbsp_debug_area->area_lc##_dump_magic = area_uc##_DUMP_MAGIC;      \
    write_ptr   += num_bytes;                                           \
    free_space  -= num_bytes;                                           \
    tzbsp_dsb();                                                        \
  }

/* Values to report the dump-ability of L1 and L2 to SDI; magic is ['L', 'C', 'V', 'S'] */
#define TZBSP_CACHE_DUMP_MAGIC      0x5356434C 
#define TZBSP_L2_CACHE_VALID        0x1

typedef unsigned int uintptr_t;
extern sysdbgCPUDumpType* sysdbg_get_cpudump();
extern void tzbsp_enable_xpu_config_clocks_boot(boolean enable);

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#define CPU_DEBUG_INFO_START_ADDRESS       (TZBSP_EBI1_TZ_BASE + 0x14E000)
typedef struct cpu_context_s
{
  uint32 r0;
  uint32 r1;
  uint32 r2;
  uint32 r3;
  uint32 r4;
  uint32 r5;
  uint32 r6;
  uint32 r7;
  uint32 r8;
  uint32 r9;
  uint32 r10;
  uint32 r11;
  uint32 r12;
  uint32 r13;
  uint32 lr;
}cpu_context_t;

typedef struct tzbsp_cpu_debug_info_s
{
  uint32 magic_cookie;
  uint32 sgi_state;
//  uint32 pc_state;
  uint32 hlos_cpu_context_buffer_ptr;
  uint32 s_sctlr;
  uint32 ns_sctlr;
  uint32 actlr;
  uint32 scr;
  uint32 tzbsp_cpu_pc_state;
  uint32 tzbsp_cpus_per_cluster_in_lpm[TZBSP_CLUSTER_COUNT_MAX];
  cpu_context_t mon;
  uint32 has_cpu_cold_booted[TZBSP_CPU_COUNT_MAX];
  uint32 sdi_hint[3];
  uint32 dbgpcsr_ns_wdt_bite[4];
  uint32 dbgdscr_ns_wdt_bite[4];
  uint32 dbgpcsr_after_sgi[4];
  uint32 dbgdscr_after_sgi[4];
  uint32 dbgpcsr_in_wd_reset_loop[4];
  uint32 dbgdscr_in_wd_reset_loop[4];
} tzbsp_cpu_debug_info_t;

tzbsp_cpu_debug_info_t *g_debug_info[TZBSP_CPU_COUNT_MAX];

extern void save_context(uint32 *buf);
extern uint32 rcp15_sec_sctlr(void);
extern uint32 rcp15_nsec_sctlr(void);
extern uint32 rcp15_actlr(void);
extern uint32 rcp15_scr(void);
extern uint8  tzbsp_cpu_pc_state[TZBSP_CPU_COUNT_MAX];
extern uint32 tzbsp_cpus_per_cluster_in_lpm[];
extern uint32 tzbsp_has_cpu_cold_booted[TZBSP_CPU_COUNT_MAX];

 /* CPU general purpose register context, as used by the monitor, and additional
 * state information. */
typedef struct tzbsp_cpu_ctx_s
{
  tzbsp_mon_cpu_ctx_t saved_ctx;
  uint32 mon_sp;
  uint32 wdog_pc;
} tzbsp_cpu_ctx_t;

/* Structure of the entire non-secure context dump buffer. Because TZ is single
 * entry only a single secure context is saved. */
typedef struct tzbsp_dump_buf_s
{
  uint32 magic;
  uint32 version;
  uint32 cpu_count;
  uint32 sc_status[TZBSP_CPU_COUNT_MAX];
  tzbsp_cpu_ctx_t sc_ns[TZBSP_CPU_COUNT_MAX];
  tzbsp_mon_cpu_ctx_t sec;
  uint32 wdt_sts[TZBSP_CPU_COUNT_MAX];
} tzbsp_dump_buf_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static sysdbgCPUDumpType *sysdbgCPUDump;

static volatile tzbsp_dump_gicd_spm_reg_t g_tzbsp_dump_gicd_spm_reg;

/* Overlay structure \c tzbsp_mon_cpu_ctx_t starting from monitor's label
 * \c Workspace_N_Begin to access monitor's saved non-secure context. */
extern tzbsp_mon_cpu_ctx_t Workspace_N_Begin;
/* Overlay structure \c tzbsp_mon_cpu_ctx_t starting from monitor's label
 * \c Workspace_S_Begin to access monitor's saved secure context. */
extern tzbsp_mon_cpu_ctx_t Workspace_S_Begin;
extern uint32 tzbsp_smc_lock;
volatile int wait_for_reset = 1;

/* Snapshot of SCR at the context where FIQ handler was entered. Monitor
 * switches to SCR[NS]=0 for FIQ handling and for WDT context dumping we need
 * to know what was the SCR[NS] state when the FIQ triggered. */
extern uint32 tzbsp_scr[TZBSP_CPU_COUNT_MAX];

/* A dummy to implement infinite loop. */
extern volatile uint32 tzbsp_boot_lock;
extern void memory_barrier(void);

extern void tzbsp_enable_qdss_clocks(void);
extern void ubsp_rpm_send_halt(void);

/* CPU context dump buffer address. The context dump buffer is in non-secure
 * memory. */
tzbsp_dump_buf_t* g_tzbsp_ctx_buf_ns;

__attribute__((section ("TZBSP_UNCACHED_ZI_VARS")))
uint32 g_tzbsp_warm_boot[TZBSP_CPU_COUNT_MAX];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

extern void tzbsp_l1_dump(void);
extern void tzbsp_write_q6_nmis(void);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
void tzbsp_read_debug_regs(uint32 *dbgpcsr, uint32 *dbgdscr);

/* Resets the chip using secure watchdog. This function will never return. */
void tzbsp_wdt_reset(void)
{

  uint32 cpu = tzbsp_cur_cpu_num();

  /* TZ no longer cleans d-cache here. D-cache contents carry over chip reset
   * and L2 cache cleaning will be done in SBL2. */

  /* Previously the download mode magic numbers were written here. TZ no
   * longer sets the magic numbers by default, which allows HLOS to fully
   * control when to enter the download mode. For example, in production
   * devices wdog barking most likely needs to just reboot. */

  /* Write the dump address to IMEM before doing the NMI writes. There is no
   * guarantee that the NMI writes can be done without bus hang etc. */
  volatile uint32* imem_addr = (uint32*) TZBSP_APPS_WDT_CTX_BUFFER_ADDR;
  *imem_addr = (uint32) g_tzbsp_ctx_buf_ns;

  tzbsp_read_debug_regs(g_debug_info[cpu]->dbgpcsr_after_sgi, g_debug_info[cpu]->dbgdscr_after_sgi);

  /* Program the WDT to bite after a delay, a delay is needed to allow other
   * CPUs time to dump their context. Bite timeout < bark timeout, hence the
   * WDT will reset when it expires. */
  tzbsp_wdt_start(TZBSP_WDT_DELAY_MS * 2, TZBSP_WDT_DELAY_MS);
  g_debug_info[cpu]->sgi_state |= 0x4;
  
  /* Notify Q6 subsystems. WDT is started before the NMI writes to ensure
   * that a hanging NMI write will still do a WDT bite. */
  tzbsp_write_q6_nmis();
  g_debug_info[cpu]->sgi_state |= 0x8;

  /* Wait for WDT bite to take effect. */
  while(1)
  {
    /* It is always possible that the buffer's address in IMEM is being
     * overwritten, keep rewriting it until the WDT resets. This should
     * maximize the possibility of getting the correct address in download
     * mode memory dump. */
    volatile uint32* imem_addr = (uint32*) TZBSP_APPS_WDT_CTX_BUFFER_ADDR;
    *imem_addr = (uint32) g_tzbsp_ctx_buf_ns;
    g_debug_info[cpu]->sgi_state |= 0x10;
    /* Send a SGI to all other CPUs to stop them and make them dump their
     * current context. */
	if (!g_debug_info[((cpu+1)%g_tzbsp_cpu_count)]->sgi_state ||
	    !g_debug_info[((cpu+2)%g_tzbsp_cpu_count)]->sgi_state ||
		!g_debug_info[((cpu+3)%g_tzbsp_cpu_count)]->sgi_state)
	{
          uint32 i;
	  g_debug_info[cpu]->sgi_state |= 0x40;


         /* Send a SGI to all online CPUs to stop them and make them dump their
         * current context. */
        for (i=0; i<TZBSP_CPU_COUNT_MAX; i++)
        {
          if((tzbsp_cpu_pc_state[i]) && (i != cpu))
          {
             HAL_qgic_Generate(TZBSP_INT_SGI_RESET, HAL_QGIC_SECURE,
                            HAL_QGIC_TARGET_CPU(i));
          }
        }

	  g_debug_info[cpu]->sgi_state += 0x100;
      
	  tzbsp_read_debug_regs(g_debug_info[cpu]->dbgpcsr_in_wd_reset_loop, g_debug_info[cpu]->dbgdscr_in_wd_reset_loop);
	}
    g_debug_info[cpu]->sgi_state |= 0x20;
  }
}

/* Populates the current CPU's context in to the context dump buffer.
 *
 * @param [in] reason The cause of the reset (WDT bark or SGI).
 */
static void tzbsp_save_ctx_new_dump(sysdbg_cpu32_ctxt_regs_type *dst,  tzbsp_mon_cpu_ctx_t *src)
{
        dst->r0      = (uint64)src->usr_r0;
        dst->r1      = (uint64)src->usr_r1;
        dst->r2      = (uint64)src->usr_r2;
        dst->r3      = (uint64)src->usr_r3;
        dst->r4      = (uint64)src->usr_r4;
        dst->r5      = (uint64)src->usr_r5;
        dst->r6      = (uint64)src->usr_r6;
        dst->r7      = (uint64)src->usr_r7;
        dst->r8      = (uint64)src->usr_r8;
        dst->r9      = (uint64)src->usr_r9;
        dst->r10     = (uint64)src->usr_r10;
        dst->r11     = (uint64)src->usr_r11;
        dst->r12     = (uint64)src->usr_r12;
        dst->r13_usr = (uint64)src->usr_r13;
        dst->r14_usr = (uint64)src->usr_r14;
        dst->r13_irq = (uint64)src->irq_r13;
        dst->r14_irq = (uint64)src->irq_r14;
        dst->r13_svc = (uint64)src->svc_r13;
        dst->r14_svc = (uint64)src->svc_r14;
        dst->r13_abt = (uint64)src->abt_r13;
        dst->r14_abt = (uint64)src->abt_r14;
        dst->r13_und = (uint64)src->und_r13;
        dst->r14_und = (uint64)src->und_r14;
        dst->r8_fiq  = (uint64)src->fiq_r8;
        dst->r9_fiq  = (uint64)src->fiq_r9;
        dst->r10_fiq = (uint64)src->fiq_r10;
        dst->r11_fiq = (uint64)src->fiq_r11;
        dst->r12_fiq = (uint64)src->fiq_r12;
        dst->r13_fiq = (uint64)src->fiq_r13;
        dst->r14_fiq = (uint64)src->fiq_r14;
        dst->pc      = (uint64)src->mon_lr;
        dst->cpsr    = (uint64)src->mon_spsr;
        dst->r14_mon = (uint64)src->mon_lr;
}


/* Populates the current CPU's context in the context dump buffer.
 *
 * @param [in] reason The cause of the reset (WDT bark or SGI).
 */
static void tzbsp_populate_dump_ctx(uint32 reason)
{
  /* WDT bark or SGI15 occured before the NS dump buffer was being set. Go
   * directly to reset routine. */
  uint32 cpu = tzbsp_cur_cpu_num();
  sysdbgCPUCtxtType *sysdbgCPUCtxt;

  sysdbg_cpu32_ctxt_regs_type *sysdbg_ctxt_regs;
  tzbsp_mon_cpu_ctx_t tzbsp_mon_cpu_non_sec_ctx = {0};
  tzbsp_mon_cpu_ctx_t tzbsp_mon_cpu_sec_ctx = {0};

  /* Logging banked GICD registers for each CPU */
  g_tzbsp_dump_gicd_spm_reg.APCS_GICD_ISENABLER0_PRIV[cpu] = HWIO_INI(APCS_GICD_ISENABLERn, 0x0);
  g_tzbsp_dump_gicd_spm_reg.APCS_GICD_ISPENDR0_PRIV[cpu] = HWIO_INI(APCS_GICD_ISPENDRn, 0x0);
  
  sysdbgCPUDump = sysdbg_get_cpudump();

  if(sysdbgCPUDump->CPUDump[cpu] && sysdbgCPUDump->CPUDump[cpu]->magic != TZBSP_DUMP_CTX_MAGIC)
  {
    //uint32 cpu = tzbsp_cur_cpu_num();
    sysdbgCPUDump->CPUDump[cpu]->version = 0; //update version before dump
   // tzbsp_mon_cpu_ctx_t *tzbsp_mon_cpu_ctx = &Workspace_N_Begin;

    sysdbgCPUCtxt = (sysdbgCPUCtxtType*)(uintptr_t)sysdbgCPUDump->CPUDump[cpu]->start_addr;	
    sysdbg_ctxt_regs = (sysdbg_cpu32_ctxt_regs_type*)&(sysdbgCPUCtxt->cpu_regs);
    

    sysdbg_ctxt_regs->_reserved |= reason;
    sysdbg_ctxt_regs->_reserved |= (g_tzbsp_warm_boot[cpu]) ?
                                            TZ_SC_STATUS_WARM_BOOT: 0;

    /* Depending on SCR[NS] status, we might have to copy saved secure or
     * non-secure context to the non-secure dump buffer. */
    if(tzbsp_scr[cpu] & 0x1)
    {
      sysdbg_ctxt_regs->_reserved |= TZ_SC_STATUS_NS_BIT;

      /* Scorpion entered the WDT/SGI in non-secure mode. Just dump the secure
       * context here. If the lock is taken, then the other CPU is in secure
       * mode and dumping the saved secure context doesn't make sense, because
       * whatever is saved is different from what is currently active in the
       * other CPU. In case both CPUs are non-secure, then secure context
       * dumping is done twice, which is ok. */
      if(!tzbsp_smc_lock && tzbsp_allow_unlock_xpu())
      {
        tzbsp_save_ctx_new_dump((sysdbg_cpu32_ctxt_regs_type*)&(sysdbgCPUCtxt->__reserved3),
                        &Workspace_S_Begin);
      }

      /* The current context is non-secure. Dump the interrupted CPU context
       * to non-secure CPU context area in the dump. */
      tzbsp_fiq_get_cpu_context(&tzbsp_mon_cpu_non_sec_ctx);
      tzbsp_save_ctx_new_dump(sysdbg_ctxt_regs, &tzbsp_mon_cpu_non_sec_ctx);

      /* Copy the PC at time of failure, the monitor LR, into the PC field */
      sysdbg_ctxt_regs->pc = sysdbg_ctxt_regs->r14_mon;
    }
    else
    {
      /* Scorpion entered the WDT/SGI in secure mode. The current register file
       * contains secure registers. The non-secure context of this CPU is saved
       * in the monitor, dumping it here. Skipping the dumping if SGI or WDT
       * was trapped in warm boot (there's no non-secure context to save). */
      if(!g_tzbsp_warm_boot[cpu])
      {
        tzbsp_save_ctx_new_dump(sysdbg_ctxt_regs, &Workspace_N_Begin);
      }

      /* The current context is secure. Unless this is a CPU doing warm boot,
       * dump the interrupted CPU context to secure CPU context area in the
       * dump. */
      if(!g_tzbsp_warm_boot[cpu] && tzbsp_allow_unlock_xpu())
      {
        tzbsp_fiq_get_cpu_context(&tzbsp_mon_cpu_sec_ctx);
        tzbsp_save_ctx_new_dump((sysdbg_cpu32_ctxt_regs_type*)&(sysdbgCPUCtxt->__reserved3), 
                        &tzbsp_mon_cpu_sec_ctx);
      }
    }

    /* CPU context should now be in memory. Indicate that the CPU context was
     * completed by TZ. */
    sysdbg_ctxt_regs->_reserved |= TZ_SC_STATUS_CTX_BY_TZ;

    /* Finally, to signify content completion, write the magic number, version,
     * etc. */
    sysdbgCPUDump->CPUDump[cpu]->magic     = TZBSP_DUMP_CTX_MAGIC;
    sysdbgCPUDump->CPUDump[cpu]->version   = TZBSP_DUMP_CTX_VERSION;
  }

  /*SDI is not able to clean caches after secure wdog reset. Issue being debugged with HW teams 
   *In the mean time, flush the caches in TZ.
   */
  tzbsp_dcache_flush_all();
}

void tzbsp_read_debug_regs(uint32 *dbgpcsr, uint32 *dbgdscr)
{ 
  if ((NULL == dbgpcsr) || (NULL == dbgdscr))
  {
    return;
  }
}

/* Interrupt handler for non-secure WDT bark. When this handler is hit, the
 * non-secure side is in dysfunctional state and WDT timed out and took a bite. 
 * As a result TZ starts a sequence to dump all CPU contexts to memory and 
 * reset the device to download mode.
 *
 * @param [in] ctx    ISR context.
 *
 * @return The ISR context is passed through.
 */
void* tzbsp_ns_wdt_isr(void* ctx)
{
  uint32 cpu = tzbsp_cur_cpu_num();
  uint32 i;
  
  g_debug_info[cpu]->sgi_state = 0;
  save_context((uint32 *)&(g_debug_info[cpu]->mon));
  g_debug_info[cpu]->hlos_cpu_context_buffer_ptr = (uint32)g_tzbsp_ctx_buf_ns;
  g_debug_info[cpu]->s_sctlr = rcp15_sec_sctlr();
  g_debug_info[cpu]->ns_sctlr = rcp15_nsec_sctlr();
  g_debug_info[cpu]->actlr = rcp15_actlr();
  g_debug_info[cpu]->scr = rcp15_scr();
  for (i=0; i < g_tzbsp_cluster_count; i++)
  {
    g_debug_info[cpu]->tzbsp_cpus_per_cluster_in_lpm[i] =
                                              tzbsp_cpus_per_cluster_in_lpm[i];
  }
  g_debug_info[cpu]->tzbsp_cpu_pc_state = *(uint32 *)tzbsp_cpu_pc_state;
  for (i=0; i<g_tzbsp_cpu_count; i++)
  {
    g_debug_info[cpu]->has_cpu_cold_booted[i] = tzbsp_has_cpu_cold_booted[i];
  }
  g_debug_info[cpu]->sdi_hint[0] = *(uint32 *)TZBSP_SHARED_IMEM_CACHE_MAGIC;
  g_debug_info[cpu]->sdi_hint[1] = *(uint32 *)TZBSP_SHARED_IMEM_L1_VALIDITY;
  g_debug_info[cpu]->sdi_hint[2] = *(uint32 *)TZBSP_SHARED_IMEM_L2_VALIDITY;
  tzbsp_read_debug_regs(g_debug_info[cpu]->dbgpcsr_ns_wdt_bite, g_debug_info[cpu]->dbgdscr_ns_wdt_bite);

  /* Collect the WDT counter value for debug analysis. */
  if(g_tzbsp_ctx_buf_ns)
  {
    //uint32 cpu = tzbsp_cur_cpu_num();
    g_tzbsp_ctx_buf_ns->wdt_sts[cpu] = HWIO_IN(APCS_WDOG_STATUS);
  }
  g_debug_info[cpu]->sgi_state |= 0x1;

  tzbsp_wdt_stop(); /* Disable the WDT. */
  g_debug_info[cpu]->sgi_state |= 0x2;

  /* Never returns; will restart the device. */
  tzbsp_err_fatal(TZBSP_ERR_FATAL_NON_SECURE_WDT);

  return ctx;
}

/* Some other CPU got the WDT bark and sent out a SGI to other CPUs.
 *
 * @param [in] ctx    ISR context.
 *
 * @return The ISR context is passed through.
 */
void* tzbsp_sgi15_isr(void* ctx)
{
  uint32 cpu = tzbsp_cur_cpu_num();
  uint32 i;
  
  g_debug_info[cpu]->sgi_state = 1;
  save_context((uint32 *)&(g_debug_info[cpu]->mon));
  g_debug_info[cpu]->hlos_cpu_context_buffer_ptr = (uint32)g_tzbsp_ctx_buf_ns;
  g_debug_info[cpu]->s_sctlr = rcp15_sec_sctlr();
  g_debug_info[cpu]->ns_sctlr = rcp15_nsec_sctlr();
  g_debug_info[cpu]->actlr = rcp15_actlr();
  g_debug_info[cpu]->scr = rcp15_scr();
  for (i=0; i<g_tzbsp_cpu_count; i++)
  {
    g_debug_info[cpu]->has_cpu_cold_booted[i] = tzbsp_has_cpu_cold_booted[i];
  }
  g_debug_info[cpu]->sdi_hint[0] = *(uint32 *)TZBSP_SHARED_IMEM_CACHE_MAGIC;
  g_debug_info[cpu]->sdi_hint[1] = *(uint32 *)TZBSP_SHARED_IMEM_L1_VALIDITY;
  g_debug_info[cpu]->sdi_hint[2] = *(uint32 *)TZBSP_SHARED_IMEM_L2_VALIDITY;  
  
  /* Collect the WDT counter value for debug analysis. */
  if(g_tzbsp_ctx_buf_ns)
  {
    //uint32 cpu = tzbsp_cur_cpu_num();
    g_tzbsp_ctx_buf_ns->wdt_sts[cpu] = HWIO_IN(APCS_WDOG_STATUS);

    /* Indicate to the debug image that the context has been dumped */
    g_tzbsp_ctx_buf_ns->magic = TZBSP_DUMP_CTX_MAGIC;
  }

  /* This CPU got the SGI about the WDT bark. Dump the CPU context. */
  tzbsp_populate_dump_ctx(TZ_SC_STATUS_SGI);
  g_debug_info[cpu]->sgi_state |= 0x2;

  /* Dump L0/L1 caches. */
  tzbsp_l1_dump();
  g_debug_info[cpu]->sgi_state |= 0x4;

  /* Release the lock tzbsp_boot_lock to allow other CPUs to enter the warm boot
   * path (SGI is pending on other CPUs as well). */
  memory_barrier();
  tzbsp_boot_lock = 0;
  memory_barrier();
  g_debug_info[cpu]->sgi_state |= 0x8;

  /* Start waiting for the device to be reset by the CPU that got the WDT
   * originally. */
  while(wait_for_reset);

  return ctx; /* Will never be reached. */
}

void tzbsp_memory_dump(tzbsp_err_fatal_e err)
{
  uint32 sc_status = (err == TZBSP_ERR_FATAL_NON_SECURE_WDT) ? TZ_SC_STATUS_WDT : 0;

  /* This CPU got the WDT. Dump the CPU context. */
  tzbsp_populate_dump_ctx(sc_status);

  /* Dump L0/L1 caches. */
  tzbsp_l1_dump();
}

/* Function to stop and flush the ETB; used in the dead-on-arrival FIQs */
void tzbsp_stop_and_flush_etb(void)
{
  tzbsp_enable_qdss_clocks();

  HWIO_OUT(QDSS_ETFETB_LAR, 0xC5ACCE55);
  tzbsp_dsb();
  if(HWIO_INF(QDSS_ETFETB_CTL, TRACECAPTEN) && (!HWIO_INF(QDSS_ETFETB_MODE, MODE)))
  {
    HWIO_OUTF(QDSS_ETFETB_FFCR, STOPONFL, 0x1);
    HWIO_OUTF(QDSS_ETFETB_FFCR, FLUSHMAN_W, 0x1);
    TZBSP_QDSS_TMC_WAIT(HWIO_INF(QDSS_ETFETB_FFCR, FLUSHMAN_R));
    TZBSP_QDSS_TMC_WAIT(!HWIO_INF(QDSS_ETFETB_STS, TMCREADY));
    tzbsp_dsb();
  }
  HWIO_OUT(QDSS_ETFETB_LAR, 0x0);

  HWIO_OUT(QDSS_ETR_LAR, 0xC5ACCE55);
  tzbsp_dsb();
  if(HWIO_INF(QDSS_ETR_CTL, TRACECAPTEN) && (!HWIO_INF(QDSS_ETR_MODE, MODE)))
  {
    HWIO_OUTF(QDSS_ETR_FFCR, STOPONFL, 0x1);
    HWIO_OUTF(QDSS_ETR_FFCR, FLUSHMAN_W, 0x1);
    TZBSP_QDSS_TMC_WAIT(HWIO_INF(QDSS_ETR_FFCR, FLUSHMAN_R));
    TZBSP_QDSS_TMC_WAIT(!HWIO_INF(QDSS_ETR_STS, TMCREADY));
    tzbsp_dsb();
  }
  HWIO_OUT(QDSS_ETR_LAR, 0x0);
}

/**
 * System call for non-secure side to set the WDT reset context dump buffer
 * address.
 *
 * @param [in] addr   Address of the buffer, must be in non-secure memory.
 * @param [in] size   Size of the buffer in bytes.
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_set_cpu_ctx_buf(uint32 addr, uint32 size)
{
  int err = -E_BAD_ADDRESS;

  /* Special case, allow setting the context dump buffer address to \c NULL. */
  if(NULL == addr)
  {
    uint32* imem_addr = (uint32*) TZBSP_APPS_WDT_CTX_BUFFER_ADDR;
    *imem_addr = NULL;
    g_tzbsp_ctx_buf_ns = NULL;
    return E_SUCCESS;
  }

  if(TZBSP_ALIGN_4BYTE(addr) != addr) /* Address must be 4 byte aligned. */
  {
    err = -E_INVALID_ARG;
  }
  else if(size < sizeof(tzbsp_dump_buf_t)) /* Buffer must be large enough. */
  {
    err = -E_INVALID_ARG;
  }
  else if(tzbsp_is_ns_range((void*)addr, size))
  {
    /* Set the IMEM buffer address to NULL so that non-WDT restart can be
     * detected. */
    uint32* imem_addr = (uint32*) TZBSP_APPS_WDT_CTX_BUFFER_ADDR;
    *imem_addr = NULL;

    g_tzbsp_ctx_buf_ns = (tzbsp_dump_buf_t*) addr;
    err = E_SUCCESS;

    /* Make sure the context dump memory is cleared to avoid carbage appearing
     * in the dump. */
    memset((void*)g_tzbsp_ctx_buf_ns, 0, sizeof(tzbsp_dump_buf_t));

    /* Ensure buffer contents are in memory. */
    tzbsp_dcache_flush_region((void*)g_tzbsp_ctx_buf_ns,
                              sizeof(tzbsp_dump_buf_t));
  }

  return err;
}

/**
 * Initialization routine for the Memory Dump/Debug framework
 */
void tzbsp_mem_dump_init(void)
{
  int i;

  /* Set up variables for SBL to dump TZ region */
  *((volatile uint32*)TZBSP_SHARED_IMEM_DUMP_IMEM_ADDR) = TZBSP_CODE_BASE;
  *((volatile uint32*)TZBSP_SHARED_IMEM_DUMP_DDR_ADDR)  = TZBSP_DEBUG_AREA_BASE;
  *((volatile uint32*)TZBSP_SHARED_IMEM_DUMP_SIZE)  = TZBSP_TZ_REGION_SIZE;
  *((volatile uint32*)TZBSP_SHARED_IMEM_DUMP_MAGIC) = TZBSP_IMEM_DUMP_MAGIC;

  /* Set up the variables which indicate if the L1 and L2 caches can be dumped */
  *((volatile uint32*)TZBSP_SHARED_IMEM_L1_VALIDITY) = (1 << tzbsp_cur_cpu_num());
  *((volatile uint32*)TZBSP_SHARED_IMEM_L2_VALIDITY) = TZBSP_L2_CACHE_VALID;
  *((volatile uint32*)TZBSP_SHARED_IMEM_CACHE_MAGIC) = TZBSP_CACHE_DUMP_MAGIC;

  /* TODO: remove after CPU context issue is resolved */
  for (i=0; i<g_tzbsp_cpu_count; i++) 
  {
    g_debug_info[i] = (tzbsp_cpu_debug_info_t *)(CPU_DEBUG_INFO_START_ADDRESS + i*sizeof(tzbsp_cpu_debug_info_t));
    if (g_debug_info[i]->magic_cookie != TZBSP_IMEM_DUMP_MAGIC) 
    {
      memset(g_debug_info[i], 0, sizeof(tzbsp_cpu_debug_info_t));
      g_debug_info[i]->magic_cookie = TZBSP_IMEM_DUMP_MAGIC;
    }
  }
}

/* TODO: remove after CPU context issue is resolved */
void tzbsp_mem_dump_init_in_milestone_call(void)
{
  int i;
  
  for (i=0; i<g_tzbsp_cpu_count; i++) 
  {
    g_debug_info[i] = (tzbsp_cpu_debug_info_t *)(CPU_DEBUG_INFO_START_ADDRESS + i*sizeof(tzbsp_cpu_debug_info_t));
    memset(g_debug_info[i], 0, sizeof(tzbsp_cpu_debug_info_t));
    g_debug_info[i]->magic_cookie = TZBSP_IMEM_DUMP_MAGIC;
  }
}

/**
 * System call for non-secure side to set the address of the UEFI structure at the 
 * shared location in IMEM which will be used to dump the TZ diag for UEFI
 *
 * @param [in] addr   Address of the buffer
 * @param [in] size   Size of the buffer in bytes.
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */ 

int tzbsp_set_address_to_dump_tz_diag_for_uefi(uint32 addr,uint32 size)
{
  uint32 *shared_imem_addr = (uint32 *)TZBSP_UEFI_SHARED_DUMP_BUFFER_ADDR;
  uint8  *shared_imem_addr_hash = (uint8 *)TZBSP_UEFI_SHARED_DUMP_BUFFER_ADDR_HASH;
  uint8  digest[TZBSP_SHA256_HASH_SZ];
  int32  ret;
  
  if(NULL == addr)
  {
    return -E_BAD_ADDRESS;
  }
  if(size < g_tzbsp_diag_len)
  {
    return -E_OUT_OF_RANGE;
  }
  if(FALSE == tzbsp_is_ns_range((void *)addr,size))
  {
    return -E_BAD_ADDRESS;
  }
  *shared_imem_addr = addr;
  ret = tzbsp_hash(TZBSP_HASH_SHA256,(uint8 *)shared_imem_addr, sizeof(addr), digest, sizeof(digest));
  if( 0 > ret)
  { 
    TZBSP_LOG(TZBSP_MSG_DEBUG,"UEFI Address Hashing Failed!");
    return -E_FAILURE;
  }
  memscpy((void *)shared_imem_addr_hash,32, (void *)digest,sizeof(digest));
  
  return E_SUCCESS;
}


/* dump the Tz diag to temp location in TZ which is not used till now using the word based copy because MMU has
 * not been initiailized upto this point
 */ 
int tzbsp_dump_tz_diag_to_temp_for_uefi(void)
{
  uint32 counter = 0;
  uint32 * temp_dump_addr = (uint32 *)TZBSP_DIAG_DUMP_TEMP_FOR_UEFI;
  uint32 *tz_diag_addr = (uint32 *)g_tzbsp_diag;
  
  /**coping diag to temp location which will be used to dump the diag area to
   *UEFI location
   */
  /* used pointer to copy rather than memcpy because MMU is not initialized */
  while(counter < g_tzbsp_diag_len)
  {
    *temp_dump_addr = *tz_diag_addr;
    temp_dump_addr +=1;
    tz_diag_addr +=1;
    counter+=4;
  }
  return E_SUCCESS;
}

/** tzbsp_dump_tz_diag_for_uefi function to copy the tzbsp diag to UEFI structure by first matching the 
  *  the hash value to check if  value is not corrupted. for the first time cold boot 
  * device hash will not be there so nothing will be copied 
  *
  */

int tzbsp_dump_tz_diag_for_uefi(void)
{
  uint8              digest[TZBSP_SHA256_HASH_SZ];
  int32              ret;
  uint32 *uefi_addr = (uint32 *)TZBSP_UEFI_SHARED_DUMP_BUFFER_ADDR;
  uint8 *shared_imem_addr_hash = (uint8 *)TZBSP_UEFI_SHARED_DUMP_BUFFER_ADDR_HASH;
  tzbsp_diag_t *temp_tz_dump_addr = (tzbsp_diag_t *)TZBSP_DIAG_DUMP_TEMP_FOR_UEFI;
   
  if(NULL == *uefi_addr)
  {
    return -E_FAILURE;
  }
  if(FALSE == tzbsp_is_ns_range((void *)(*uefi_addr),g_tzbsp_diag_len))
  {
    return -E_BAD_ADDRESS;
  }
  /* Turn on Crypto clocks requred by _hash */
  HWIO_OUTF(GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE,CRYPTO_CLK_ENA, 0x1);
  HWIO_OUTF(GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE,CRYPTO_AXI_CLK_ENA, 0x1);
  HWIO_OUTF(GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE,CRYPTO_AHB_CLK_ENA, 0x1);

  ret = tzbsp_hash(TZBSP_HASH_SHA256, (uint8 *)uefi_addr,  sizeof(uefi_addr), digest, sizeof(digest));

  HWIO_OUTF(GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE,CRYPTO_CLK_ENA, 0x0);
  HWIO_OUTF(GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE,CRYPTO_AXI_CLK_ENA, 0x0);
  HWIO_OUTF(GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE,CRYPTO_AHB_CLK_ENA, 0x0);

  if(0 > ret)
  { 
   TZBSP_LOG(TZBSP_MSG_DEBUG,"Hashing Failed!");
   return -E_FAILURE;
  }

  if(0 == memcmp((void *)digest,(void *)shared_imem_addr_hash,sizeof(digest)))
  {
    memscpy((void *)(*uefi_addr),g_tzbsp_diag_len,(void *)temp_tz_dump_addr,g_tzbsp_diag_len);
    return E_SUCCESS;
  }
  
  return -E_FAILURE;
}
 
/* Logging spm status, spm control and gicd registers during tzbsp error fatal for debugging */
void tzbsp_dump_gicd_spm_reg(void)
{
  int i;

  /* Log MPM2 Time */
  g_tzbsp_dump_gicd_spm_reg.MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL = HWIO_IN(MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL);

  /* Logging SPM status & SPM control for power cluster CPU */  
  g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_SAW2_SPM_STS[0] = HWIO_IN(APCS_ALIAS0_SAW2_SPM_STS);
  g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_SAW2_SPM_STS[1] = HWIO_IN(APCS_ALIAS1_SAW2_SPM_STS);
  if(g_tzbsp_cpu_count >= 4)
  {
    g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_SAW2_SPM_STS[2] = HWIO_IN(APCS_ALIAS2_SAW2_SPM_STS);
    g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_SAW2_SPM_STS[3] = HWIO_IN(APCS_ALIAS3_SAW2_SPM_STS);
  }
  g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_SAW2_SPM_CTL[0] = HWIO_IN(APCS_ALIAS0_SAW2_SPM_CTL);
  g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_SAW2_SPM_CTL[1] = HWIO_IN(APCS_ALIAS1_SAW2_SPM_CTL);
  if(g_tzbsp_cpu_count >= 4)
  {
    g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_SAW2_SPM_CTL[2] = HWIO_IN(APCS_ALIAS2_SAW2_SPM_CTL);
    g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_SAW2_SPM_CTL[3] = HWIO_IN(APCS_ALIAS3_SAW2_SPM_CTL);
  }
  /* Logging Power status & Power gate status for performance cluster CPU */
  g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_APC_PWR_STATUS[0] = HWIO_IN(APCS_ALIAS0_APC_PWR_STATUS);
  g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_APC_PWR_STATUS[1] = HWIO_IN(APCS_ALIAS1_APC_PWR_STATUS);
  if(g_tzbsp_cpu_count >= 4)
  {
    g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_APC_PWR_STATUS[2] = HWIO_IN(APCS_ALIAS2_APC_PWR_STATUS);
    g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_APC_PWR_STATUS[3] = HWIO_IN(APCS_ALIAS3_APC_PWR_STATUS);
  }
  g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_APC_PWR_GATE_STATUS[0] = HWIO_IN(APCS_ALIAS0_APC_PWR_GATE_STATUS);
  g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_APC_PWR_GATE_STATUS[1] = HWIO_IN(APCS_ALIAS1_APC_PWR_GATE_STATUS);
  if(g_tzbsp_cpu_count >= 4)
  {
    g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_APC_PWR_GATE_STATUS[2] = HWIO_IN(APCS_ALIAS2_APC_PWR_GATE_STATUS);
    g_tzbsp_dump_gicd_spm_reg.APCS_ALIASn_APC_PWR_GATE_STATUS[3] = HWIO_IN(APCS_ALIAS3_APC_PWR_GATE_STATUS);
  }
  /* Logging SPM status & SPM control for performance cluster L2 */
  g_tzbsp_dump_gicd_spm_reg.APCS_L2_SAW2_SPM_STS = HWIO_IN(APCS_L2_SAW2_SPM_STS);
  g_tzbsp_dump_gicd_spm_reg.APCS_L2_SAW2_SPM_CTL = HWIO_IN(APCS_L2_SAW2_SPM_CTL);


  /* Logging L2 Power status */
  g_tzbsp_dump_gicd_spm_reg.APCS_L2_PWR_STATUS = HWIO_IN(APCS_L2_PWR_STATUS);


  for(i=1; i < 7; i++)
  {
    g_tzbsp_dump_gicd_spm_reg.APCS_GICD_ISENABLERn_PUB[i] = HWIO_INI(APCS_GICD_ISENABLERn, i);
    g_tzbsp_dump_gicd_spm_reg.APCS_GICD_ISPENDRn_PUB[i] = HWIO_INI(APCS_GICD_ISPENDRn, i);
  }
}

/* Syscall API to halt RPM and requrest for online dumps
 * @Param[in]  spare  not used kept it for future use
 */

int tzbsp_rpm_online_dump(uint32 spare)
{
  HAL_xpu2_Status ret;
  
  //sending interrupt to RPM to halt
  ubsp_rpm_send_halt();

  //enabling XPU clocks
  tzbsp_enable_xpu_config_clocks_boot(TRUE);

  //resetting the RPM XPU to default state
  ret = HAL_xpu2_Reset(HAL_XPU2_RPM_CFG);
  if(ret != HAL_XPU2_NO_ERROR)
  {  
     return E_FAILURE;
  }
  
  ret = HAL_xpu2_Reset(HAL_XPU2_RPM_APU);
  if(ret != HAL_XPU2_NO_ERROR)
  {  
     return E_FAILURE;
  }
  //disabling the XPU clock
  tzbsp_enable_xpu_config_clocks_boot(FALSE);

  return E_SUCCESS;

}
