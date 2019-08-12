/**
@file tzbsp_init.c
@brief Trustzone Init Handler

This file contains the routines for initializing TZBSP.

*/
/*===========================================================================
   Copyright (c) 2010-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/init/src/tzbsp_init.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <string.h>
#include <HALhwio.h>

#include "tzbsp_errno.h"
#include "tzbsp.h"
#include "tzbsp_chipset.h"
#include "tzbsp_sys.h"
#include "tzbsp_config.h"
#include "tzbsp_xpu.h"
#include "tzbsp_log.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_target.h"
#include "tzbsp_diag.h"
#include "tzbsp_exec.h"
#include "crypto/tzbsp_cipher.h"
#include "tzbsp_secboot.h"
#include "tzbsp_mem.h"
#include "tzbsp_hwio.h"
#include "qfprom_test.h"

#include "sysdbg_main.h"
#include "PlatformInfo.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define TZBSP_CPU_PC_STATE_PWRDWN  0
#define TZBSP_CPU_PC_STATE_PWRUP   1
#define TZBSP_CPU_PC_STATE_INVALID 2

/** ----------------------------------------------------------------------------
Stub out ARM routines included with standard C library
         routines that are not used in TZBSP
* ---------------------------------------------------------------------------*/

void   __rt_raise              (int signal, int type) {}
void   __default_signal_handler(int signal, int type) {}
void   _sys_exit               (int return_code)      {}

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

extern uint32 tzbsp_has_cpu_cold_booted[TZBSP_CPU_COUNT_MAX];
extern uint32 tzbsp_cpu_hlos_cold_start_addr[TZBSP_CPU_COUNT_MAX];
extern uint32 tzbsp_cpu_hlos_warm_start_addr[TZBSP_CPU_COUNT_MAX];
extern uint8  tzbsp_cpu_pc_state[TZBSP_CPU_COUNT_MAX];
extern uint32 Image$$TZBSP_UNCACHED_ZI$$Base;
extern uint32 Image$$TZBSP_UNCACHED_ZI$$Length;

static uint32 s_enable_dbg = 0;

/* The initial value for stack canary. */
unsigned int __stack_chk_guard[] = {0xDEADDEAD};
extern uint32 tzbsp_cpu_boot_pc_state[TZBSP_CPU_COUNT_MAX];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/** Defining tzos_entry as weak reference TZOS (QSEE or MobiCore) if present
 * are expected to implement tzos_entry */
__weak extern int tzos_entry(const tzbsp_memblock_t* a0,
                             const tzbsp_memblock_t* a1,
                             uint32                  a2);

/** TODO: This is temporary and should be removed when support for old chip
 *        revisions is dropped */
__weak extern void tzbsp_do_chip_ver_cfg(void);

extern int tzbsp_pil_init(void);
extern uint32 tzbsp_prng_getdata(uint8*, uint32);
extern void tzbsp_init_monitor(void);
extern int tzbsp_chipset_init(uint32 debug);
extern void tzbsp_read_ddr_size(void);
extern int tzbsp_secure_channel_key_init(void);
extern void tzbsp_memzi(uint32* addr, uint32 size);
extern int qsee_demand_paging_init(void);
extern int qsee_demand_paging_init_key(void);
extern int tzbsp_chipset_reinit(void);
extern int tzbsp_secboot_update_rollback_fuse_version(void);
__weak extern void tzbsp_sec_interrupts_add_target(uint32);

extern int tzbsp_milestone_set(uint32 jump_addr, uint32 mode);
extern boolean tzbsp_allow_memory_dump(void);
extern void tzbsp_disable_l2_as_tcm(void);
extern void tzbsp_init_cluster_cpu_var (void);
extern void tzbsp_enable_cluster_coherency_arch(void);
extern void tzbsp_disable_cluster_coherency_arch(void);

/**
   Check the state of the CPUs to make sure each CPU warmboot
   corresponds to a power collapse that terminated in TZBSP
 */
static boolean tzbsp_init_check_boot_tamper( void )
{
  uint32 ii = 0;
  uint32 cpu = tzbsp_cur_cpu_num();

  tzbsp_cpu_pc_state[cpu]++;

  for (ii = 0; ii < g_tzbsp_cpu_count; ii++)
  {
    if (tzbsp_cpu_pc_state[ii] >= TZBSP_CPU_PC_STATE_INVALID)
    {
      TZBSP_LOG_DBG("init: Invalid CPU state for CPU %d\n", ii);
      return TRUE;
    }
  }

  return FALSE;
}

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Called when stack protection fails, detecting a corrupt stack*/
void __stack_chk_fail(void)
{
#ifdef TZBSP_WITH_TEST_SVC
  TZBSP_ERROR_CODE(STACK_CHECK_FAILED);
#else
  TZBSP_ERROR_CODE(STACK_CHECK_FAILED);
  tzbsp_loop_here();
#endif
}

/**
 * Cold boot flow init. This function is called when TZBSP is initialized for
 * the first time.
 */
uint32 appsbl_entry_pt = 0;
uint32 hypervisor_entry_pt = 0;
extern void tzbsp_setup_warmboot_ns_env(uint32 warmboot_addr);
extern void tzbsp_setup_initial_ns_env(uint32 hypervisor_entry_pt,uint32 appsbl_entry_pt, uint32 hyp_config_bmsk);
extern void tzbsp_oem_fuse_config(void);

extern uint32 tzbsp_cpus_per_cluster_in_lpm[TZBSP_CLUSTER_COUNT_MAX];

void tzbsp_init_lpm_cpu_count(void)
{
  uint32 cl_cnt;
  
  /* Initially all CPU's are offline except the boot CPU. The uncached region is zero-initialized
     in cold_init, so set the proper value here */
  for(cl_cnt=0; cl_cnt < g_tzbsp_cluster_count; cl_cnt++ )
  {
    tzbsp_cpus_per_cluster_in_lpm[cl_cnt] = TZBSP_CPU_COUNT_PER_CLUSTER; 
  }
  /* Boot cluster will have 1 CPU Up */
  tzbsp_cpus_per_cluster_in_lpm[g_tzbsp_boot_cluster_idx] -= 1;
}

#ifndef FIXME_8936_CCI_WA

void tzbsp_cold_init ( void )
{
  int err = E_SUCCESS;
  uint32 hyp_config_bmsk;

  do
  {

    /* This should be called first in case pointers/memory locations are
     * dependant on the chip version */
    tzbsp_do_chip_ver_cfg();

   /* Initialize CPU & Cluster Count Variables */
   tzbsp_init_cluster_cpu_var();

   tzbsp_has_cpu_cold_booted[g_tzbsp_boot_cpu_idx] = TRUE;

    /* Call the monitor to configure itself. */
    tzbsp_init_monitor();

    /* Initialize the Diag region.  This must be early to enable debug
       messages */
    tzbsp_diag_init();
    /* Adds DDR regions to default memory map before enabling MMU */
    err = tzbsp_ddr_init();
    if (err)
    {
      /* TZBSP_ERROR_CODE(INIT_DDR_INIT_FAILED); */
      break;
    }

    err = qsee_demand_paging_init();
    if (err)
    {
      /* TZBSP_ERROR_CODE(INIT_DEMAND_PAGE_FAILED); */
      break;
    }

    tzbsp_disable_l2_as_tcm();

#ifdef FIXME_8936
    if(g_tzbsp_boot_cluster_idx == tzbsp_cur_cluster_num()) /* Performance Cluster */
    {
      HWIO_OUTF(APCS_ALIAS1_L2_FLUSH_CTL, ACINACTM, 0);
      HWIO_OUT(APCS_CCI_SNOOPCONTROL4, 3);
      while(0 != HWIO_IN(APCS_CCI_STATUS));
    }
#endif

    err = tzbsp_mmu_cache_init();
    if (err)
    {
      /* TZBSP_ERROR_CODE(INIT_MMU_INIT_FAILED); */
      break;
    }

    /* Unmap any dynamically mapped peripherals */
    err = tzbsp_mem_unmap_all_devices();
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_DEVICE_UNMAP_FAILED);
      break;
    }

    s_enable_dbg = tzbsp_is_debug_enabled();

    err = tzbsp_configure_hw(s_enable_dbg);
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_HW_INIT_FAILED);
      break;
    }

    PlatformInfo_Init();
    err = tzbsp_chipset_init(s_enable_dbg);
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_CHIPSET_INIT_FAILED);
      break;
    }
    system_debug_main(); //first pass into sysdbg main

    /* update anti-rollback fuse for sbl1-loaded images */
    err = tzbsp_secboot_update_rollback_fuse_version();
    if(err)
    {
      TZBSP_ERROR_CODE(INIT_HW_INIT_FAILED);
      break;
    }

    tzbsp_oem_fuse_config();

#ifndef TZBSP_VIRTIO
    /* Populate the stack protection canary with random data. */
    tzbsp_prng_getdata((uint8*)__stack_chk_guard, 4);
#endif

    err = tzbsp_pil_init();
    if(err)
    {
      TZBSP_ERROR_CODE(INIT_PIL_INIT_FAILED);
      break;
    }
    /* Enable interrupts. IRQs are not enabled at this point, we will first
     * have to do a function style return to boot loaders. IRQs on secure side
     * are enabled when the first system call is executed. */
    tzbsp_int_enable(TZBSP_EA_BIT | TZBSP_FIQ_BIT);

    /* Initializes TZOS (LK or MobiCore)*/
    err = tzbsp_reset_exec((uint32) tzos_entry);
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_RESET_EXEC_FAILED);
      break;
    }

    err = tzbsp_init_check_boot_tamper();
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_BOOT_TAMPER_CHECK_FAILED);
      break;
    }
  } while (0);

  if(err)
  {
    tzbsp_loop_here();
  }

  tzbsp_milestone_set(appsbl_entry_pt, 0);
#ifdef FIXME_8909
//TO DO: remove when hypervisor entry will be removed form the SBL shared structure
 hypervisor_entry_pt =NULL;
 #endif
 
  if(NULL != hypervisor_entry_pt)
   {
  // bit #0 indicates MMU should be in bypass mode in HYP
#if (defined(FEATURE_NO_XPU) ||  defined(TZBSP_WITH_TEST_SVC) )
  hyp_config_bmsk = 1;
#else
  hyp_config_bmsk = tzbsp_allow_memory_dump()? 1: 0;
#endif
     tzbsp_setup_initial_ns_env(hypervisor_entry_pt, appsbl_entry_pt,hyp_config_bmsk);
   }

#ifdef TZBSP_WITH_TEST_SVC
    
  /*Call to qfprom test framework*/
  boot_qfprom_test();
#endif // TZBSP_WITH_TEST_SVC

  tzbsp_init_lpm_cpu_count();
} /* tzbsp_cold_init() */

#else // FIXME_8936_CCI_WA defined

volatile extern uint32 tzbsp_cci_reset_done_flag[1];
extern void tzbsp_set_boot_remap( void );
extern uint32 tzbsp_do_l2_init[TZBSP_CLUSTER_COUNT_MAX];
extern uint32 tzbsp_cur_cluster_num( void );

void tzbsp_cold_init ( void )
{
   int i=0;

    /* This should be called first in case pointers/memory locations are
     * dependant on the chip version */
    tzbsp_do_chip_ver_cfg();

   /* Initialize CPU & Cluster Count Variables */
   tzbsp_init_cluster_cpu_var();

   tzbsp_has_cpu_cold_booted[g_tzbsp_boot_cpu_idx] = TRUE;

   tzbsp_disable_l2_as_tcm();

   tzbsp_cci_reset_done_flag[0] = 0;
   tzbsp_do_l2_init[1] = 1;
   tzbsp_cpus_per_cluster_in_lpm[0] = TZBSP_CPU_COUNT_PER_CLUSTER;
   tzbsp_cpus_per_cluster_in_lpm[1] = TZBSP_CPU_COUNT_PER_CLUSTER;

    /* Call the monitor to configure itself. */
    tzbsp_init_monitor();

    /* Initialize the Diag region.  This must be early to enable debug
       messages */
    tzbsp_diag_init();
    
    ////////////////////////////////////////////////////////////////////
    // Configure CCI SPM sequence
    ////////////////////////////////////////////////////////////////////

    HWIO_OUT(CCI_SAW2_SECURE, 0x7);
    HWIO_OUT(CCI_SAW2_CFG, 0x14);
    HWIO_OUTF(CCI_SAW2_SPM_CTL, SPM_SYS_PC_MODE, 0x0);
    HWIO_OUTF(CCI_SAW2_SPM_CTL, SPM_START_ADR, 0x00);
    HWIO_OUTF(CCI_SAW2_SPM_CTL, ISAR, 0x01);
    HWIO_OUTF(CCI_SAW2_SPM_CTL, WAKEUP_CONFIG, 0x03);
    HWIO_OUT(CCI_SAW2_SPM_DLY, 0x3C102800);

    HWIO_OUTI(CCI_SAW2_SPM_SLP_SEQ_ENTRY_n,0,0x60700360);
    HWIO_OUTI(CCI_SAW2_SPM_SLP_SEQ_ENTRY_n,1,0x00000F70);

    //HWIO_OUTI(CCI_SAW2_SPM_SLP_SEQ_ENTRY_n,0,0x0F720370);

    HWIO_OUTF(CCI_SAW2_SPM_CTL, SPM_EN, 0x01);

    ////////////////////////////////////////////////////////////////////
    // Configure CORE SPM sequence
    ////////////////////////////////////////////////////////////////////

    //=================  APCS SPM SEQ  ===========================//
    HWIO_OUT(APCS_BANKED_SAW2_SECURE, 0x00000007); 	// Enable Non Secure Read for RPM
    HWIO_OUT(APCS_BANKED_SAW2_CFG, 0x01);
    HWIO_OUTF(APCS_BANKED_SAW2_SPM_CTL, SPM_SYS_PC_MODE, 0x0);
    HWIO_OUTF(APCS_BANKED_SAW2_SPM_CTL, SPM_START_ADR, 0x0);
    HWIO_OUTF(APCS_BANKED_SAW2_SPM_CTL, ISAR, 0x01);
    HWIO_OUTF(APCS_BANKED_SAW2_SPM_CTL, WAKEUP_CONFIG, 0x03);	//sys_spm_wakeup or rising edge of sys_spm_dbg_nopwrdwn	
    HWIO_OUT(APCS_BANKED_SAW2_SPM_DLY, 0x3C102800);

    HWIO_OUTI(APCS_BANKED_SAW2_SPM_SLP_SEQ_ENTRY_n,0,0x03603020);
    HWIO_OUTI(APCS_BANKED_SAW2_SPM_SLP_SEQ_ENTRY_n,1,0x30200B60);
    HWIO_OUTI(APCS_BANKED_SAW2_SPM_SLP_SEQ_ENTRY_n,2,0x0000000F);

    //HWIO_OUTI(APCS_BANKED_SAW2_SPM_SLP_SEQ_ENTRY_n,0,0x60036030);
    //HWIO_OUTI(APCS_BANKED_SAW2_SPM_SLP_SEQ_ENTRY_n,1,0x00000F30);

    HWIO_OUTF(APCS_BANKED_SAW2_SPM_CTL, SPM_EN, 0x01);


    ////////////////////////////////////////////////////////////////////
    // Configure L2 SPM sequence
    ////////////////////////////////////////////////////////////////////
    //=================  L2 SPM SEQ  ===========================//
    HWIO_OUT(APCS_CLUS1_L2_SAW2_SECURE, 0x7);
    HWIO_OUT(APCS_CLUS1_L2_SAW2_CFG, 0x14);
    HWIO_OUTF(APCS_CLUS1_L2_SAW2_SPM_CTL, SPM_SYS_PC_MODE, 0x00);
    HWIO_OUTF(APCS_CLUS1_L2_SAW2_SPM_CTL, SPM_START_ADR, 0x0);
    HWIO_OUTF(APCS_CLUS1_L2_SAW2_SPM_CTL, ISAR, 0x01);
    HWIO_OUTF(APCS_CLUS1_L2_SAW2_SPM_CTL, WAKEUP_CONFIG, 0x03);	
    HWIO_OUT(APCS_CLUS1_L2_SAW2_SPM_DLY, 0x3C102800);


    HWIO_OUTI(APCS_CLUS1_L2_SAW2_SPM_SLP_SEQ_ENTRY_n,0,0x501B0330); 
    HWIO_OUTI(APCS_CLUS1_L2_SAW2_SPM_SLP_SEQ_ENTRY_n,1,0x000F5030);

    //HWIO_OUTI(APCS_CLUS1_L2_SAW2_SPM_SLP_SEQ_ENTRY_n,0,0x0F520350); 

    HWIO_OUTF(APCS_CLUS1_L2_SAW2_SPM_CTL, SPM_EN, 0x01);	


    //Configure Boot-remapper to TZ location 
    tzbsp_set_boot_remap();
    
    ///////////////////////////////////////////////////////////////////////////
    // Configure QTimer for delay for around 100 us to generate the interrupt
    // Configure QTimer-0 Physical timer frame-0 interrupt
    ///////////////////////////////////////////////////////////////////////////

    //QGIC init 
    HWIO_OUT(APCS_GICD_CTLR, 0x1);
    HWIO_OUT(APCS_GICC_PMR, 0xF8);
    HWIO_OUT(APCS_GICD_CGCR, 0xFFFFFFFF);

    //disable all interrupts
    for(i=0; i<=HWIO_APCS_GICD_ICENABLERn_MAXn; i++) { 
       HWIO_OUTI(APCS_GICD_ICENABLERn, i, 0xFFFFFFFF); 
    }; 

    //Configure and enable timer interrupt in QGIC
    HWIO_OUTI(APCS_GICD_ICFGRn, 18, 0x3<<6);
    HWIO_OUTI(APCS_GICD_ITARGETSRn, 0x48, 0x10000000);
    HWIO_OUTI(APCS_GICD_ISENABLERn, 9, 0x8);

    //Set the timer frequency to 19.2 MHz
    HWIO_OUT(APCS_QTMR1_QTMR_AC_CNTFRQ,0x124F800);
 
    //Set permissions for CVAL
    HWIO_OUTI(APCS_QTMR1_QTMR_AC_CNTACR_n,0,0x3F);
 
    //Set matching count to around 100us
    HWIO_OUT(APCS_QTMR1_F2V1_QTMR_V1_CNTP_TVAL,0x800);
 
    //Enable Timer && Interrupt
    HWIO_OUT(APCS_QTMR1_F2V1_QTMR_V1_CNTP_CTL,0x01);
 
    //Disable QGIC CPU interface
    HWIO_OUT(APCS_GICC_CTLR, 0x00000000);
 
    //Set ACINATCM bit for cluster-0 to disable snoop
    HWIO_OUT(APCS_CCI_SNOOPCONTROL4, 0);
    HWIO_OUTF(APCS_ALIAS1_L2_FLUSH_CTL, ACINACTM, 1);
    while(0 != HWIO_IN(APCS_CCI_STATUS));

    //Enter into WFI state to trigger the SPM signals
    asm("WFI");

} /* tzbsp_cold_init() */
extern uint32* g_tzbsp_entry_counters[TZBSP_CPU_COUNT_MAX];
void tzbsp_cold_init_post_wa ( void )
{
  int err = E_SUCCESS;
  uint32 hyp_config_bmsk;

  tzbsp_cci_reset_done_flag[0] = 1;
  
  *g_tzbsp_entry_counters[tzbsp_cur_cpu_num()]=0;

  do
  {

    //=================  DISABLE L2, CCI & A53 SPM ======================//
    HWIO_OUTF(CCI_SAW2_SPM_CTL, SPM_EN, 0x0);
    HWIO_OUTF(APCS_BANKED_SAW2_SPM_CTL, SPM_EN, 0x0);
    HWIO_OUTF(APCS_CLUS1_L2_SAW2_SPM_CTL, SPM_EN, 0x0);

    
    /* This should be called first in case pointers/memory locations are
     * dependant on the chip version */
    tzbsp_do_chip_ver_cfg();

    /* Call the monitor to configure itself. */
    tzbsp_init_monitor();

    /* Initialize the Diag region.  This must be early to enable debug
       messages */
    //tzbsp_diag_init();
    /* Adds DDR regions to default memory map before enabling MMU */
    err = tzbsp_ddr_init();
    if (err)
    {
      /* TZBSP_ERROR_CODE(INIT_DDR_INIT_FAILED); */
      break;
    }

    err = qsee_demand_paging_init();
    if (err)
    {
      /* TZBSP_ERROR_CODE(INIT_DEMAND_PAGE_FAILED); */
      break;
    }

    //tzbsp_disable_l2_as_tcm();

    tzbsp_enable_cluster_coherency_arch();

    err = tzbsp_mmu_cache_init();
    if (err)
    {
      /* TZBSP_ERROR_CODE(INIT_MMU_INIT_FAILED); */
      break;
    }

    /* Unmap any dynamically mapped peripherals */
    err = tzbsp_mem_unmap_all_devices();
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_DEVICE_UNMAP_FAILED);
      break;
    }

    s_enable_dbg = tzbsp_is_debug_enabled();

    err = tzbsp_configure_hw(s_enable_dbg);
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_HW_INIT_FAILED);
      break;
    }


    err = tzbsp_chipset_init(s_enable_dbg);
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_CHIPSET_INIT_FAILED);
      break;
    }
    system_debug_main(); //first pass into sysdbg main

    /* update anti-rollback fuse for sbl1-loaded images */
    err = tzbsp_secboot_update_rollback_fuse_version();
    if(err)
    {
      TZBSP_ERROR_CODE(INIT_HW_INIT_FAILED);
      break;
    }

    tzbsp_oem_fuse_config();

#ifndef TZBSP_VIRTIO
    /* Populate the stack protection canary with random data. */
    tzbsp_prng_getdata((uint8*)__stack_chk_guard, 4);
#endif

    err = tzbsp_pil_init();
    if(err)
    {
      TZBSP_ERROR_CODE(INIT_PIL_INIT_FAILED);
      break;
    }
    /* Enable interrupts. IRQs are not enabled at this point, we will first
     * have to do a function style return to boot loaders. IRQs on secure side
     * are enabled when the first system call is executed. */
    tzbsp_int_enable(TZBSP_EA_BIT | TZBSP_FIQ_BIT);

    /* Initializes TZOS (LK or MobiCore)*/
    err = tzbsp_reset_exec((uint32) tzos_entry);
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_RESET_EXEC_FAILED);
      break;
    }

    err = tzbsp_init_check_boot_tamper();
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_BOOT_TAMPER_CHECK_FAILED);
      break;
    }
  } while (0);

  if(err)
  {
    tzbsp_loop_here();
  }

  tzbsp_milestone_set(appsbl_entry_pt, 0);

  // bit #0 indicates MMU should be in bypass mode in HYP
#ifdef FEATURE_NO_XPU
  hyp_config_bmsk = 1;
#else
  hyp_config_bmsk = tzbsp_allow_memory_dump()? 1: 0;
#endif
  tzbsp_setup_initial_ns_env(hypervisor_entry_pt, appsbl_entry_pt, 
        hyp_config_bmsk);


#ifdef TZBSP_WITH_TEST_SVC
    
  /*Call to qfprom test framework*/
  boot_qfprom_test();
#endif // TZBSP_WITH_TEST_SVC

  tzbsp_init_lpm_cpu_count();
} /* tzbsp_cold_init_post_wa () */

#endif // FIXME_8936_CCI_WA
/**
 * CPU init, called when cold booting secondary CPU(s) and warm booting any of
 * the CPUs. When this function is called TZBSP has been initialized once
 * already (@see tzbsp_cold_init).
 */

uint32 tzbsp_cpu_init (void)
{
  int err = E_SUCCESS;
  uint32 cpu = tzbsp_cur_cpu_num();
  /* It is not possible to return from this function without getting a jump
   * address or jump instruction as \c tzbsp_loop_here is entered under error
   * conditions.
   *
   * Initializing to zero to avoid compiler and Klockwork warnings. */
  uint32 jump_addr  = 0;
  uint32 jump_instr = 0;

  do
  {
    if (cpu >= g_tzbsp_cpu_count)
    {
      err = -E_NOT_SUPPORTED;
      break;
    }

    /* CPU Boot Milestone Update - 0x000000XX */
    tzbsp_cpu_boot_pc_state[cpu] = 0xA0;

    /* This must happen before re-enabling the MMU and before
     * enabling FIQs.
     */
    err = tzbsp_init_check_boot_tamper();
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_BOOT_TAMPER_CHECK_FAILED);
      break;
    }

    /* CPU Boot Milestone Update - 0x000000XX */
    tzbsp_cpu_boot_pc_state[cpu] = 0xA1;

    tzbsp_enable_cluster_coherency_arch();

    /* Call the monitor to configure itself. */
    tzbsp_init_monitor();

    /* CPU Boot Milestone Update - 0x000000XX */
    tzbsp_cpu_boot_pc_state[cpu] = 0xA2;

    /* The jump address is a physical memory address located inside the
     * HLOS. Read jump address before the MMU has been enabled to ensure that
     * the instruction that exists at the physical jump address location can
     * be accessed for debug purposes. */
    if ((g_tzbsp_boot_cpu_idx != cpu) && (FALSE == tzbsp_has_cpu_cold_booted[cpu]))
    {
      jump_addr = tzbsp_cpu_hlos_cold_start_addr[cpu];
    }
    else
    {
      jump_addr = tzbsp_cpu_hlos_warm_start_addr[cpu];
    }
    if (jump_addr)
    {
      jump_instr = *((uint32*)jump_addr);
    }

    tzbsp_mmu_cache_re_enable();

    /* CPU Boot Milestone Update - 0x000000XX */
    tzbsp_cpu_boot_pc_state[cpu] = 0xA3;

    tzbsp_sec_interrupts_add_target(cpu);

    /* CPU Boot Milestone Update - 0x000000XX */
    tzbsp_cpu_boot_pc_state[cpu] = 0xA4;


    /* Determine the jump address.
       CPU 0 isn't cold booted through this function */
    if ((g_tzbsp_boot_cpu_idx != cpu) && (FALSE == tzbsp_has_cpu_cold_booted[cpu]))
    {
      tzbsp_has_cpu_cold_booted[cpu] = TRUE;
    }
    else
    {
      tzbsp_exec_warmboot_notifier();
    }

    /* CPU Boot Milestone Update - 0x000000XX */
    tzbsp_cpu_boot_pc_state[cpu] = 0xA5;

    err = tzbsp_configure_hw(s_enable_dbg);
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_HW_INIT_FAILED);
      break;
    }
    /* CPU Boot Milestone Update - 0x000000XX */
    tzbsp_cpu_boot_pc_state[cpu] = 0xA6;

    err = tzbsp_chipset_reinit();
    if (err)
    {
      TZBSP_ERROR_CODE(INIT_CHIPSET_REINIT_FAILED);
      break;
    }

    /* CPU Boot Milestone Update - 0x000000XX */
    tzbsp_cpu_boot_pc_state[cpu] = 0xAF;

    tzbsp_int_enable(TZBSP_EA_BIT | TZBSP_FIQ_BIT);

  } while(0);

  if(err)
  {
    tzbsp_loop_here();
  }

  /* Log the both the jump address and the instruction at the jump address to
   * detect DDR memory problems. */
  if(jump_addr)
  {
    TZBSP_DIAG_WARM_JMP_ADDR(jump_addr);
    TZBSP_DIAG_WARM_JMP_INSTR(jump_instr);
  }

  tzbsp_setup_warmboot_ns_env(jump_addr);

   if(NULL == hypervisor_entry_pt)
  {
     return jump_addr;
  }
  else
  {
  return hypervisor_entry_pt;
  }
}
