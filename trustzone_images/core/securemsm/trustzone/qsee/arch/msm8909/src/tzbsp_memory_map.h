#ifndef TZBSP_MEMORY_MAP_H
#define TZBSP_MEMORY_MAP_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_memory_map.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/12   tk      First version.
============================================================================*/

#include "tzbsp_target.h"

/* Memory regiion claimed by TZ for A7SS POR start address */
#define TZBSP_A7SS_POR_BASE             0x80000000
#define TZBSP_A7SS_POR_END              0x80001000
#define TZBSP_A7SS_POR_SIZE             (TZBSP_A7SS_POR_END - \
                                         (TZBSP_A7SS_POR_BASE))

/* MSS Q6 SW memory range. */
#define TZBSP_EBI1_MSS_BASE             0x88800000
#define TZBSP_EBI1_MSS_END              0x8E200000
#define TZBSP_EBI1_MSS_SIZE             (TZBSP_EBI1_MSS_END - \
                                         (TZBSP_EBI1_MSS_BASE))

/*TZ non-relocatable partition range in DDR */
#define TZBSP_EBI1_TZ_BASE             0x87E80000
#define TZBSP_EBI1_TZ_END              0x88000000
#define TZBSP_EBI1_TZ_SIZE             (TZBSP_EBI1_TZ_END - \
                                         (TZBSP_EBI1_TZ_BASE))

/* HYP is not present in 8909 */
/*HYP non-relocatable partition range in DDR */
/*
#define TZBSP_EBI1_HYP_BASE             0x86400000
#define TZBSP_EBI1_HYP_END              0x86500000
#define TZBSP_EBI1_HYP_SIZE             (TZBSP_EBI1_HYP_END - \
                                     (TZBSP_EBI1_HYP_BASE))
*/


/*RFSA Buffer  partition range in DDR */
#define TZBSP_EBI1_RFSA_BASE             0x87CE0000
#define TZBSP_EBI1_RFSA_END              0x87D00000
#define TZBSP_EBI1_RFSA_SIZE             (TZBSP_EBI1_RFSA_END - \
                                         (TZBSP_EBI1_RFSA_BASE))

/* WCNSS memory range. */
#define TZBSP_EBI1_WCNSS_BASE           0x8B000000
#define TZBSP_EBI1_WCNSS_END            0x8B600000
#define TZBSP_EBI1_WCNSS_SIZE           (TZBSP_EBI1_LPASS_END - \
                                         (TZBSP_EBI1_LPASS_BASE))
/* EBI1 shared memory. */
#define TZBSP_EBI1_SHARED_MEM_BASE      0x87D00000
#define TZBSP_EBI1_SHARED_MEM_END       0x87E00000
#define TZBSP_EBI1_SHARED_MEM_SIZE      (TZBSP_EBI1_SHARED_MEM_END - \
                                         (TZBSP_EBI1_SHARED_MEM_BASE))

/* BOOT ROM MPU CONFIG ADDR bits as 8 bits. configuring address as below, but 
 * actual address is different. */
#define TZBSP_BOOT_ROM_MSS_BASE        0x00018000
#define TZBSP_BOOT_ROM_MSS_END         0x00024000
#define TZBSP_BOOT_ROM_MSS_SIZE       (TZBSP_BOOT_ROM_MSS_END - \
                                      (TZBSP_BOOT_ROM_MSS_BASE))
#define TZBSP_BOOT_ROM_APPS_BASE        0x00100000
#define TZBSP_BOOT_ROM_APPS_END         0x00118000
#define TZBSP_BOOT_ROM_APPS_SIZE       (TZBSP_BOOT_ROM_APPS_END - \
                                       (TZBSP_BOOT_ROM_APPS_BASE))
/* Used for xpu config only, can't use above _BASE, due to a
   bug,  xpu requires the 21st bit to be 0 */
#define TZBSP_BOOT_ROM_APPS_XPU_BASE    0x00000000
#define TZBSP_BOOT_ROM_APPS_XPU_END     0x00018000
									   
#define TZBSP_FIXED_SECURE_DDR_BASE     0x87EE0000
#define TZBSP_FIXED_SECURE_DDR_END      TZBSP_EBI1_TZ_END
#define TZBSP_FIXED_SECURE_DDR_SIZE    (TZBSP_FIXED_SECURE_DDR_END - \
                                        TZBSP_FIXED_SECURE_DDR_BASE)

/* SSD */
#define TZBSP_SSD_REGION_BASE             0x87F00000
#define TZBSP_SSD_REGION_END              0x87F02000
#define TZBSP_SSD_REGION_SIZE             (TZBSP_SSD_REGION_END - \
                                           TZBSP_SSD_REGION_BASE)

/* MobiCore */
#define TZBSP_EXT_ELF_BASE                0x87F02000
#define TZBSP_EXT_ELF_END                 (TZBSP_EXT_ELF_BASE + \
                                           TZBSP_EXT_ELF_SIZE)

#define TZBSP_EXT_ELF_TT_BASE             (TZBSP_ROUNDUP(uint32, \
                                           TZBSP_SSD_REGION_END, \
                                           (1 << TZBSP_L1_PT_ALIGN_LOOKUP(TZ_VAS_SIZE))))

#define TZBSP_EBI1_SECCHANNEL_MSS_BASE    (TZBSP_EBI1_TZ_BASE+ 0x17D000)
#define TZBSP_EBI1_SECCHANNEL_MSS_END     (TZBSP_EBI1_TZ_BASE+ 0x17E000)
#define TZBSP_EBI1_SECCHANNEL_MSS_SIZE    (TZBSP_EBI1_SECCHANNEL_MSS_END - \
                                           TZBSP_EBI1_SECCHANNEL_MSS_BASE)

#define TZBSP_EBI1_SECCHANNEL_LPASS_BASE  (TZBSP_EBI1_TZ_BASE+ 0x17E000)
#define TZBSP_EBI1_SECCHANNEL_LPASS_END   (TZBSP_EBI1_TZ_BASE+ 0x17F000)
#define TZBSP_EBI1_SECCHANNEL_LPASS_SIZE  (TZBSP_EBI1_SECCHANNEL_LPASS_END - \
                                           TZBSP_EBI1_SECCHANNEL_LPASS_BASE)

#define TZBSP_EBI1_SECCHANNEL_WCNSS_BASE  (TZBSP_EBI1_TZ_BASE+ 0x17F000)
#define TZBSP_EBI1_SECCHANNEL_WCNSS_END   (TZBSP_EBI1_TZ_BASE+ 0x180000)
#define TZBSP_EBI1_SECCHANNEL_WCNSS_SIZE  (TZBSP_EBI1_SECCHANNEL_WCNSS_END - \
                                           TZBSP_EBI1_SECCHANNEL_WCNSS_BASE)

#define TZBSP_DEBUG_REGION_BASE           (TZBSP_EBI1_TZ_BASE+ 0x60000)
#define TZBSP_DEBUG_REGION_END            (TZBSP_EBI1_TZ_BASE+ 0x80000)
#define TZBSP_DEBUG_REGION_SIZE           (TZBSP_DEBUG_REGION_END - \
                                           TZBSP_DEBUG_REGION_BASE)

#define TZBSP_SBL1_REGION_BASE            0x87C00000
#define TZBSP_SBL1_REGION_END             0x87CE0000
#define TZBSP_SBL1_REGION_SIZE           (TZBSP_SBL1_REGION_END - \
                                           TZBSP_SBL1_REGION_BASE)

/* IMEM */ 
#define TZBSP_IMEM_USB_TRACE_PIPE_BASE    0x08603800
#define TZBSP_IMEM_USB_TRACE_PIPE_END     0x08604000

#define TZBSP_IMEM_TZ_BASE                0x08601000
#define TZBSP_IMEM_TZ_END                 0x08603800

#define TZBSP_IMEM_SHARED_BASE            0x08600000
#define TZBSP_IMEM_SHARED_END             0x08601000

/* TLMM */

#define TZBSP_TLMM_GPIO_START             0x01101000
#define TZBSP_TLMM_GPIO_END               0x01102000

#define TZBSP_TLMM_DIR_CONN1_START        0x01102000
#define TZBSP_TLMM_DIR_CONN1_END          0x01103000

#define TZBSP_TLMM_DIR_CONN2_START        0x01103000
#define TZBSP_TLMM_DIR_CONN2_END          0x01104000

#define TZBSP_TLMM_DIR_CONN3_START        0x01104000
#define TZBSP_TLMM_DIR_CONN3_END          0x01106000

#define TZBSP_TLMM_DIR_CONN4_START        0x01106000
#define TZBSP_TLMM_DIR_CONN4_END          0x01107000

#define TZBSP_TLMM_RFEE_START             0x01108000
#define TZBSP_TLMM_RFEE_END               0x01109000

#define TZBSP_TLMM_PMIC_HDRV_PULL_START   0x0110B000
#define TZBSP_TLMM_PMIC_HDRV_PULL_END     0x0110C000
 
#define TZBSP_TLMM_MISC_RPM_START         0x01100000
#define TZBSP_TLMM_MISC_RPM_END           0x01101000
 
#if ((TZBSP_EBI1_TZ_BASE > TZBSP_SSD_REGION_BASE) || \
     (TZBSP_EBI1_TZ_END  < TZBSP_SSD_REGION_END))
  #error "SSD region not within secure DDR region"
#endif

/* Area in DDR reserved for debugging information. SBL will dump TZ 
 * here when entering DLOAD mode.
 * (TZBSP_EBI1_TZ_BASE + 0x088000)--(TZBSP_EBI1_TZ_BASE + 0x14C000) is the unused region.
 */
#define TZBSP_DEBUG_AREA_BASE           (TZBSP_EBI1_TZ_BASE + 0xCC000)


/* TZ will use to dump the diag area of previous run of device, which will be 
 * used by tbsp_dump_tz_diag_for_uefi()  
 * We always use the area above debug-area for this purpose and maintain the same in
 * future chipsets as well.
 * ********* THIS AREA CAN BE REUSED AFTER MILESTONE  ****************
 */
#define TZBSP_DIAG_DUMP_TEMP_FOR_UEFI     (TZBSP_DEBUG_AREA_BASE - TZBSP_DIAG_SIZE)


/* TZ Diagnostics area, within the TZ DDR non-relocatable region;
 * defined in tzbsp_target.h, but included here for completeness
 */
#define TZBSP_DIAG_END                  (TZBSP_DIAG_BASE + TZBSP_DIAG_SIZE)
/*
#if ((TZBSP_EBI1_TZ_BASE > TZBSP_DIAG_BASE) || \
     (TZBSP_EBI1_TZ_END  < TZBSP_DIAG_END))
  #error "TZBSP diagnostic region placement doesn't match memory map!"
#endif
*/

#endif /* TZBSP_MEMORY_MAP_H */
