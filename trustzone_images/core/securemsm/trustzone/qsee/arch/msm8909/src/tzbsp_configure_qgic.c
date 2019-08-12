/**
@file TZBSP_CONFIGURE_HW.c
@brief Trustzone HW Configuration routines for initialization

This file contains the routines for initializing the secure HW.

Contains the public/private methods and related data types.

*/
/*===========================================================================
   Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_configure_qgic.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
04/22/10   tk      Improved QGIC config, initial system register config.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <HALhwio.h>
#include <HALqgic.h>

#include "tzbsp.h"
#include "tzbsp_errno.h"
#include "tzbsp_sys.h"
#include "tzbsp_target.h"
#include "tzbsp_isr.h"
#include "tzbsp_xpu.h"
#include "tzbsp_config.h"
#include "tzbsp_log.h"
#include "tzbsp_hwio.h"
#include "tzbsp_context_util.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Ensure that physical addresses are used. */
#if ((APCS_QGIC2_REG_BASE) != 0x0b000000)
#error "TZBSP: illegal addressing for system registers detected"

#endif

void* tzbsp_xpu_isr(void* ctx);
void* tzbsp_ns_wdt_isr(void* ctx);
void* tzbsp_sgi15_isr(void* ctx);
void* tzbsp_err_fatal_isr(void* ctx);
extern void tzbsp_stop_and_flush_etb(void);

/**
 * Helper function to determine if the ETB needs to be stopped
 * and flushed, depending on the interrupt being serviced
 */
static void tzbsp_fiq_etb_check(uint32 intnum)
{
  if(intnum == TZBSP_INT_NON_SEC_WDOG_BITE ||
     intnum == TZBSP_INT_AHB_TIMEOUT ||
     intnum == TZBSP_INT_RPM_WDOG_BITE ||
     intnum == TZBSP_INT_RPM_ERR_IND ||
     intnum == TZBSP_INT_MM_NOC_ERR ||
     intnum == TZBSP_INT_PERIPH_NOC_ERR ||
     intnum == TZBSP_INT_SYS_NOC_ERR ||
     intnum == TZBSP_INT_CONF_NOC_ERR
    )
  {
    tzbsp_stop_and_flush_etb();
  }
}

/**
 * Default FIQ handler function, called from the monitor IRQ vector
 * and executed in monitor mode. After this function returns the
 * context interrupted by the FIQ will be restored. This function is
 * supposed to acknowledge/end FIQs.
 *
 * Note: This function is executed in ISR context, touching any
 * non-intlocked state could corrupt the interrupted context.
 */
static int tzbsp_fiq_handler(void)
{
  int ret = E_SUCCESS;
  HAL_qgic_InterruptType intnum;
  HAL_qgic_CPUIdType     cpu;

  HAL_qgic_AckInterrupt(&intnum, &cpu);

  /* Process the interrupt, if an interrupt was pending (i.e. not spurious) */
  if(intnum <= HAL_QGIC_IRQ_MAX)
  {
    /* For interrupts signifying fatal error conditions, stop and flush
     * the ETB before it fills up with FIQ handler calls */
    tzbsp_fiq_etb_check(intnum);

#ifdef TZBSP_WITH_TBASE
    if(0 == tzbsp_cur_cpu_num())
#endif
      /* QSEE context is only saved when running on Core0 */
      tzbsp_fiq_construct_qsee_ctx();

    ret = tzbsp_dispatch_int(intnum);
  }

  HAL_qgic_EndOfInterrupt(intnum, cpu);
  tzbsp_dsb();

  return ret;
}

/* Context variables to be passed to xPU violation isr */
extern int xpu_err_sec_ctx;
extern int xpu_err_non_sec_ctx;

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/**
 * Configures a secure interrupt on the interrupt controller.
 *
 * @param [in] intnum   The interrupt to be configured.
 * @param [in] target   Target processor for the raised interrupt, either
 *                      a CPU number or \c TZBSP_INT_TARGET_SELF.
 * @param [in] flags    Either \c TZBSP_INTF_TRIGGER_LEVEL or
 *                             \c TZBSP_INTF_TRIGGER_EDGE.
 * @param [in] enable   Initial interrupt enable/disable status on the
 *                      interrupt controller.
 */
void tzbsp_config_qgic_int(uint32 intnum, uint32 target, uint32 flags,
                           boolean enable)
{
  HAL_qgic_CPUIdType cpu;
  HAL_qgic_TriggerType trigger = (TZBSP_INTF_TRIGGER_EDGE & flags) ?
                                 HAL_QGIC_TRIGGER_EDGE: HAL_QGIC_TRIGGER_LEVEL;

  if(TZBSP_INT_TARGET_SELF == target)
    cpu = HAL_QGIC_TARGET_CPU(tzbsp_cur_cpu_num());
  else if(TZBSP_INT_TARGET_ALL == target)
    cpu = (1 << g_tzbsp_cpu_count) - 1;
  else
    cpu = HAL_QGIC_TARGET_CPU(target);

  HAL_qgic_SetTrigger(intnum, trigger);
  HAL_qgic_SetSecurity(intnum, HAL_QGIC_SECURE);
  HAL_qgic_SetPriority(intnum, HAL_QGIC_PRIORITY_HIGHEST);
  HAL_qgic_SetTargets(intnum, cpu);
  if(enable)
    HAL_qgic_Enable(intnum);
  else
    HAL_qgic_Disable(intnum);
}

/**
 * Saves an interrupt configuration.
 *
 * @param [in] intnum         The interrupt to be saved.
 * @param [out] pEnabled      Interrupt is enabled.
 * @param [out] pTriggerEdge  Whether trigger is \c TZBSP_INTF_TRIGGER_EDGE.
 * @param [out] pSecure       Whether interrupt is secure.
 * @param [out] pPriority     Interrupt priority.
 * @param [out] pTargets      Targets processors for the interrupt.
 */
void tzbsp_save_qgic_int(uint32   intnum,       boolean *pEnabled,
                         boolean *pTriggerEdge, boolean *pSecure,
                         uint32  *pPriority,    uint32  *pTargets)
{
  HAL_qgic_TriggerType trigger;
  HAL_qgic_SecureType  secure;
  HAL_qgic_PriorityType priority;

  *pEnabled = HAL_qgic_IsEnabled(intnum);

  HAL_qgic_GetTrigger(intnum,&trigger);
  *pTriggerEdge = (trigger == HAL_QGIC_TRIGGER_EDGE) ? TRUE : FALSE;

  HAL_qgic_GetSecurity(intnum,&secure);
  *pSecure = (secure == HAL_QGIC_SECURE) ? TRUE : FALSE;

  HAL_qgic_GetPriority(intnum,&priority);
  *pPriority = (uint32)priority;

  HAL_qgic_GetTargets(intnum,pTargets);
}

/**
 * Restores an interrupt configuration.
 *
 * @param [in] intnum       The interrupt to restore.
 * @param [in] enabled      Whether the interrupt is enabled
 * @param [in] triggerEdge  Whether trigger is \c TZBSP_INTF_TRIGGER_EDGE.
 * @param [in] secure       Whether interrupt is secure.
 * @param [in] priority     Interrupt priority.
 * @param [in] targets      Targets processors for the interrupt.
 */
void tzbsp_restore_qgic_int(uint32 intnum, boolean enabled, boolean triggerEdge,
                            boolean secure, uint32 priority, uint32 targets)
{
  HAL_qgic_TriggerType trigger = (triggerEdge == TRUE) ?
                                 HAL_QGIC_TRIGGER_EDGE : HAL_QGIC_TRIGGER_LEVEL;
  HAL_qgic_SecureType  security = (secure == TRUE) ?
                                  HAL_QGIC_SECURE : HAL_QGIC_NON_SECURE;
  
  HAL_qgic_SetTrigger(intnum, trigger);
  HAL_qgic_SetSecurity(intnum, security);
  HAL_qgic_SetPriority(intnum, (HAL_qgic_PriorityType)priority);
  HAL_qgic_SetTargets(intnum, targets);
  if (enabled)
  {
    HAL_qgic_Enable(intnum);
  }
  else
  {
    HAL_qgic_Disable(intnum);
  }
}

/**
 * Disables the given interrupt on the interrupt controller.
 *
 * @param [in] intnum   The interrupt to be disabled.
 */
void tzbsp_disable_qgic_int(uint32 intnum)
{
  HAL_qgic_Disable(intnum);
}

/**
 * Enables the given interrupt on the interrupt controller.
 *
 * @param [in] intnum   The interrupt to be enabled.
 */
void tzbsp_enable_qgic_int(uint32 intnum)
{
  HAL_qgic_Enable(intnum);
}

/**
 * Configures the given interrupt to be non-secure on the interrupt controller.
 *
 * @param [in] intnum   The interrupt to be made non-secure.
 */
void tzbsp_set_non_secure_qgic_int(uint32 intnum)
{
  HAL_qgic_SetSecurity(intnum, HAL_QGIC_NON_SECURE);
}

/**
 * Initialize secure QGIC functionality common to all CPUs. Called by
 * CPU0 only during warm and cold boots.
 */
static void tzbsp_configure_qgic_common ( void )
{
  HAL_qgic_BaseAddressType base;
  base.nGICDBaseAddress = 0xb000000;
  base.nGICCBaseAddress = 0xb002000;
  base.nGICHBaseAddress = 0xb001000;
  base.nGICVBaseAddress = 0xb004000;

  HAL_qgic_Init(&base);

  /* Mark all interrupts as non-secure. This also sets CPU0 banked
   * interrupts as non-secure. */
  HAL_qgic_SetSecurityAll(HAL_QGIC_NON_SECURE);

  /* Configure the IPRIORITY to allow non-secure access. This also
   * sets CPU0 banked interrupt priorities. */
  HAL_qgic_SetPriorityAll(0x80);

  /* Allow non-secure access via ANSACR. TODO: Controls GICD_CGCR
   * which controls the QGIC clock gating, the final configuration
   * doesn't allow non-secure access. */
  HAL_qgic_SetClockSecurity(HAL_QGIC_NON_SECURE);

  /* Enables secure side distributor only. */
  HAL_qgic_EnableDistributor();
} /* tzbsp_configure_qgic_common() */

/**
 * CPU specific QGIC configuration. Interrupts from ID0 to ID32 are
 * CPU specific banked interrupts which must be configured separately
 * for each CPU.
 *
 * Only the secure banked CPU interface configuration registers are
 * touched (GICC_CTLR, GICC_PMR, GICC_BPR). Non-secure banked
 * registers are configured by the non-secure side tramp.
 *
 * @param [in] cpunum The number of the current CPU context.
 */
static void tzbsp_configure_qgic_cpu_specific(uint32 cpunum)
{
  uint32 n;
  /*----------------------------------------------------------------------*/

  /* ====== CPU specific distributor configuration ======= */

  /* Mark all SGIs and PPIs non-secure. Note: ARM strongly recommends
   * that all processors reserve interrupts ID8-ID15 for secure
   * interrupts. */
  for(n=0; n < 32; n++)
  {
    HAL_qgic_SetSecurity(n, HAL_QGIC_NON_SECURE);
    HAL_qgic_SetPriority(n, 0x80);
  }

  /* ====== CPU interface configuration ====== */

  /* Zero the CPU interface control register. */
  HWIO_OUT(APCS_GICC_CTLR, 0x00000000);

  /* Non-cooperative prioritization scheme is used; non-secure
   * interrupts cannot pre-empt secure interrupts
   * (GICC_CTLR.SBPR=0). */
  HAL_qgic_SetBinaryPointMode(HAL_QGIC_BINARY_POINT_MODE_BANKED);

  /* No priority grouping. */
  HAL_qgic_SetBinaryPoint(0x00000000);

  /* Secure read of the ICCIAR doesn't acknowledge non-secure
   * interrupts (GICC_CTLR.AckCtl=0). */
  HAL_qgic_SetSecureAckNS(FALSE);

  /* Secure interrupts are always FIQs (GICC_CTLR.FIQEn=1). Non-secure
   * interrupts are IRQs by default. */
  HAL_qgic_SetSecureDest(HAL_QGIC_PORT_FIQ);

  /* Allow all interrupts. */
  HAL_qgic_SetPriorityMask(HAL_QGIC_PRIORITY_LOWEST);

  /* And finally enable the CPU interface. Note that IRQs and FIQs are
   * masked at ARM level at this point and cannot be unmasked until
   * TZBSP is ready to receive requests. */
  HAL_qgic_EnableInterrupts();
}

int tzbsp_check_pend_disable_gicc(void)
{
  HAL_qgic_InterruptType pnInterrupt;
  HAL_qgic_CPUIdType     pnCPUId;

  HAL_qgic_SetSecureAckNS(TRUE);
  HAL_qgic_GetHighestPending(&pnInterrupt, &pnCPUId);
  HAL_qgic_SetSecureAckNS(FALSE);

  /*Note that there is possibility of interrupt becoming pending 
   * in between checking and disabling GICC. Those interrupts will 
   * get fired after warm boot and there is no early exit. 
   */
  if(pnInterrupt == 1023)
  {
    /*No interrupt pending*/
    HAL_qgic_DisableInterrupts();
    HAL_qgic_DisableInterruptsNS();
    return 0;
  } 
  else 
  {
    /*some interrupt pending*/
    return 1;
  }
}

/**
 * Configures QGIC distributor and CPU interfaces for secure use
 * (non-cooperative scheme). Also configures the interrupts handled by TZ as
 * FIQs.
 */
int tzbsp_configure_qgic(void)
{
  uint32 cpunum = tzbsp_cur_cpu_num();

  /* Configure QGIC distributor and CPU interfaces. */
  if(0 == cpunum)
  {
    /* Connect a FIQ handler function to monitor. By default, IRQ intercept
     * function is not installed. */
    tzbsp_install_fiq_function(tzbsp_fiq_handler);
    tzbsp_configure_qgic_common();
  }

  tzbsp_configure_qgic_cpu_specific(cpunum);

  return 0;
}

void tzbsp_reconfigure_qgic(void)
{
  HAL_qgic_EnableInterrupts();
  HAL_qgic_EnableInterruptsNS();  
}
int tzbsp_configure_qgic_interrupts(void)
{
  uint32 cpunum = tzbsp_cur_cpu_num();
  int err = E_SUCCESS;

  /* Configure secure interrupts. */
  if(0 == cpunum)
  {

    /* XPU error interrupt is routed to CPU0 only */
    err = tzbsp_register_isr(TZBSP_INT_XPU_SEC, TZBSP_INT_XPU_SEC_DESC,
                              tzbsp_xpu_isr, &xpu_err_sec_ctx,
                              TZBSP_INTF_TRIGGER_LEVEL, TRUE);

    err |= tzbsp_register_isr(TZBSP_INT_XPU_NON_SEC, TZBSP_INT_XPU_NON_SEC_DESC,
                              tzbsp_xpu_isr, &xpu_err_non_sec_ctx,
                              TZBSP_INTF_TRIGGER_LEVEL, TRUE);

    /* WDT is trapped as FIQ in TZ although it is owned by HLOS. */
    err |= tzbsp_register_isr(TZBSP_INT_NON_SEC_WDOG_BITE,
                              TZBSP_INT_NON_SEC_WDOG_BITE_DESC, tzbsp_ns_wdt_isr,
                              NULL, TZBSP_INTF_TRIGGER_EDGE | TZBSP_INTF_ALL_CPUS, TRUE);

    /* SGI[15] is used by other CPUs to signal about fatal errors. */
    err |= tzbsp_register_isr(TZBSP_INT_SGI_RESET,
                              TZBSP_INT_SGI_RESET_DESC, tzbsp_sgi15_isr,
                              NULL, TZBSP_INTF_TRIGGER_EDGE, TRUE);

    /* RPM watchdog and error interrupts */
    err |= tzbsp_register_isr(TZBSP_INT_RPM_WDOG_BITE,
                              TZBSP_INT_RPM_WDOG_BITE_DESC, tzbsp_err_fatal_isr,
                              (void*)TZBSP_ERR_FATAL_RPM_WDOG,
                              TZBSP_INTF_TRIGGER_EDGE | TZBSP_INTF_ALL_CPUS, TRUE);

    err |= tzbsp_register_isr(TZBSP_INT_RPM_ERR_IND,
                              TZBSP_INT_RPM_ERR_IND_DESC, tzbsp_err_fatal_isr,
                              (void*)TZBSP_ERR_FATAL_RPM_ERR,
                              TZBSP_INTF_TRIGGER_EDGE | TZBSP_INTF_ALL_CPUS, TRUE);
  }
  else
  {
    /* SGI[15] is used by other CPUs to signal about fatal errors. */
    err |= tzbsp_register_isr(TZBSP_INT_SGI_RESET,
                              TZBSP_INT_SGI_RESET_DESC,  tzbsp_sgi15_isr,
                              NULL, TZBSP_INTF_TRIGGER_EDGE, TRUE);
  }

  if(err)
    return -E_FAILURE;
  else
    return E_SUCCESS;
}

