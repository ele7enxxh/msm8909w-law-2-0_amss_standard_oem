#ifndef TZBSP_TARGET_COMMON_H
#define TZBSP_TARGET_COMMON_H

/*===========================================================================

                    T r u s t z o n e   T a r g e t
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the TZBSP
  that is target-specific

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/common/tzbsp_target.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/12   sg      Added TZBSP_AMT_MAX_BIT_POS
10/04/11   mm      Associated descriptive text with ISR
05/05/10   tk      DMOV related changes
04/22/10   ejt     removed comdef.h since it is not needed
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "customer.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Define the destination address and size of the TZBSP.
---------------------------------------------------------------------------*/
#define TZBSP_CODE_BASE          SCL_TZ_CODE_BASE

/*---------------------------------------------------------------------------
  Define static region sizes
---------------------------------------------------------------------------*/
#define TZBSP_DIAG_SIZE           0x1000
#define TZBSP_STACK_SIZE          0x1000
#define TZBSP_BOOT_STACK_SIZE     0x200
#define TZBSP_HEAP_SIZE           0x8000
#define TZBSP_UNCACHED_ZI_SIZE    0x1000
#define TZBSP_EXT_ELF_SIZE        0x80000

/* Stack size for the monitor mode (in bytes, for each CPU).
 *
 * Note that a customer cannot change monitor stack size, because binary
 * libraries depend on the monitor stack size and cannot be compiled by the
 * customer.
 */
#define TZBSP_MON_STACK_SIZE      1024

/*-------------------------------------------------------------------------*/
#define TZBSP_SYSTEM_IMEM_BASE    SCL_TZ_CODE_BASE
#define TZBSP_SYSTEM_IMEM_SIZE    SCL_TZ_TOTAL_SIZE

#define TZBSP_CLUSTER_COUNT_MAX          1
#define TZBSP_CPU_COUNT_MAX              4
#define TZBSP_CPU_COUNT_PER_CLUSTER  (TZBSP_CPU_COUNT_MAX / TZBSP_CLUSTER_COUNT_MAX)

#define TZBSP_NUM_DDR_REGIONS     2

#define KRAIT_CACHE_GRANULE_SZ    64

/* There are 70 debug registers, 4 bytes each */
#define KRAIT_DEBUG_REG_SZ        (70 * 4)

/* There are 54 trace registers, 4 bytes each */
#define KRAIT_TRACE_REG_SZ        (54 * 4)

/* Interrupts [0, 15] are Software Generated Interrupts (SGI). */
#define TZBSP_INT_SGI(xx) (xx)

/* Interrupts [16, 31] are Private Peripheral Interrupts (PPI). */
#define TZBSP_INT_PPI(xx) ((xx)+16)

/* Interrupts starting from 32 are Shared Peripheral Interrupts (SPI). */
#define TZBSP_INT_SPI(xx) ((xx)+32)

/* Macro for determining whether interrupt is a PPI. */
#define TZBSP_INT_IS_PPI(xx) ((16 <= (xx)) && ((xx) < 32))

/* QGIC INTNUM for VMIDMT Client Secure errors summary interrupt */
#define TZBSP_INT_VMIDMT_ERR_CLT_SEC            TZBSP_INT_SPI(198)
#define TZBSP_INT_VMIDMT_ERR_CLT_SEC_DESC       "SPI VMIDMT ERR Client Sec"

/* QGIC INTNUM for VMIDMT Client Non Secure errors summary interrupt */
#define TZBSP_INT_VMIDMT_ERR_CLT_NONSEC         TZBSP_INT_SPI(199)
#define TZBSP_INT_VMIDMT_ERR_CLT_NONSEC_DESC    "SPI VMIDMT ERR Client Sec"

/* QGIC INTNUM for VMIDMT Cfg Secure errors summary interrupt */
#define TZBSP_INT_VMIDMT_ERR_CFG_SEC            TZBSP_INT_SPI(200)
#define TZBSP_INT_VMIDMT_ERR_CFG_SEC_DESC       "SPI VMIDMT ERR CFG Sec"

/* QGIC INTNUM for VMIDMT Cfg Non Secure errors summary interrupt */
#define TZBSP_INT_VMIDMT_ERR_CFG_NONSEC         TZBSP_INT_SPI(201)
#define TZBSP_INT_VMIDMT_ERR_CFG_NONSEC_DESC    "SPI VMIDMT ERR CFG Sec"

/* QGIC INTNUM for XPU Secure summary interrupt, Krait SPI,
 * xpu_summary_irq, id 195. */
#define TZBSP_INT_XPU_SEC                       TZBSP_INT_SPI(195)
#define TZBSP_INT_XPU_SEC_DESC                  "SPI XPU Sec"

/* QGIC INTNUM for XPU Non Secure summary interrupt, Krait SPI,
 * xpu_summary_irq, id 196. */
#define TZBSP_INT_XPU_NON_SEC                   TZBSP_INT_SPI(196)
#define TZBSP_INT_XPU_NON_SEC_DESC              "SPI XPU NonSec"

/* QGIC INTNUM for Secure watchdog bark interrupt. */
#define TZBSP_INT_SEC_WDOG_BARK                 TZBSP_INT_SPI(181)
#define TZBSP_INT_SEC_WDOG_BARK_DESC            "SPI WDog Bark"

/* QGIC INTNUM for SPI 159. This SPI is used for LPASS signalling TZ to have
 * it's security HW configured. IRQ Info:
 *    q6ss_irq_out(7) -> lpass_irq_out_apcs[3] -> SYS_apcsQgicSPI[159]
 *    Shared Memory Driver (SMD) low priority traffic I/F interrupt
 */
#define TZBSP_INT_LPASS_RESTORE_SEC_CFG         TZBSP_INT_SPI(159)
#define TZBSP_INT_LPASS_RESTORE_SEC_CFG_DESC    "SPI LPASS Sec Cfg"

/* QGIC INTNUM for NS WDOG Bite, id 4. */
#define TZBSP_INT_NON_SEC_WDOG_BITE             TZBSP_INT_SPI(4)
#define TZBSP_INT_NON_SEC_WDOG_BITE_DESC        "SPI WDOG NS Bite"

/* QGIC INTNUM for AHB timeout interrupt. */
#define TZBSP_INT_AHB_TIMEOUT                   TZBSP_INT_SPI(197)
#define TZBSP_INT_AHB_TIMEOUT_DESC              "AHB Timeout"

/* QGIC INTNUM for secure physical CP15 QTimer (frame 2). */
#define TZBSP_INT_CPU_TIMER_SECURE              TZBSP_INT_PPI(2)
#define TZBSP_INT_CPU_TIMER_SECURE_DESC         "QTIMER CPU SEC"

/* QGIC INTNUM for secure KPSS memory mapped QTimer (frame 2). */
#define TZBSP_INT_APCS_TIMER_SECURE             TZBSP_INT_SPI(10)
#define TZBSP_INT_APCS_TIMER_SECURE_DESC        "QTIMER APCS SEC"

/* QGIC INTNUM for RPM Error Indication, id 174. */
#define TZBSP_INT_RPM_ERR_IND                   TZBSP_INT_SPI(174)
#define TZBSP_INT_RPM_ERR_IND_DESC              "SPI RPM Error Ind"

/* QGIC INTNUM for RPM WDOG Bite, id 237. */
#define TZBSP_INT_RPM_WDOG_BITE                 TZBSP_INT_SPI(237)
#define TZBSP_INT_RPM_WDOG_BITE_DESC            "SPI RPM WDOG Bite"

/* QGIC INTNUM for SGI 15. This SGI is used for signalling a reset from the
 * CPU that got a fatal error */
#define TZBSP_INT_SGI_RESET                     TZBSP_INT_SGI(15)
#define TZBSP_INT_SGI_RESET_DESC                "SGI Reset"

/* QGIC INTNUM for SGI14. This SGI is used for signalling a 3rd party TEE 
 * by the CPU that received and FIQ for the TEE */ 
#define TZBSP_INT_SGI_NOTIFY_TEE                TZBSP_INT_SGI(14) 
#define TZBSP_INT_SGI_NOTIFY_TEE_DESC           "SGI Notify" 

/* The following list of interrupts are registered/handled by modules outside
 * of TZ-ownership, but are fatal errors that need the ETB to be stopped */
#define TZBSP_INT_AHB_TIMEOUT                   TZBSP_INT_SPI(197)
#define TZBSP_INT_MM_NOC_ERR                    TZBSP_INT_SPI(154)
#define TZBSP_INT_PERIPH_NOC_ERR                TZBSP_INT_SPI(155)
#define TZBSP_INT_SYS_NOC_ERR                   TZBSP_INT_SPI(193)
#define TZBSP_INT_CONF_NOC_ERR                  TZBSP_INT_SPI(194)

#define TZBSP_1TB                       0x10000000000
#define TZBSP_8GB                       0x00200000000
#define TZBSP_4GB                       0x00100000000
#define TZBSP_2GB                       0x00080000000
#define TZBSP_1GB                       0x00040000000
#define TZBSP_512MB                     0x00020000000
#define TZBSP_256MB                     0x00010000000
#define TZBSP_128MB                     0x00008000000
#define TZBSP_64MB                      0x00004000000
#define TZBSP_32MB                      0x00002000000
#define TZBSP_16MB                      0x00001000000
#define TZBSP_2MB                       0x00000200000
#define TZBSP_1MB                       0x00000100000
#define TZBSP_4KB                       0x00000001000

#define TZBSP_DDR_START_ADDR                SCL_EBI1_MEM_BASE
/** Memory including this address and above is peripheral memory. */
#define TZBSP_DDR_MAX_ADDR                  (SCL_EBI1_MEM_BASE + \
                                             SCL_EBI1_MEM_TOTAL_SIZE)

#define TZBSP_AMT_MAX_BIT_POS 4096

/**Defines the first SPI interrupt number */ 
#define QGIC_SPI_INTERRUPT_BASE_NUM 32

/** register for bringing RPM out of reset */
#ifdef TZBSP_WITH_TBASE
#define GCC_APSS_MISC_ADDR          (0x1860000 + 0x04800000)
#else
#define GCC_APSS_MISC_ADDR          0x1860000
#endif


#define RPM_RESET_REMOVAL_BMSK      0x6

#endif  /* TZBSP_TARGET_H */
