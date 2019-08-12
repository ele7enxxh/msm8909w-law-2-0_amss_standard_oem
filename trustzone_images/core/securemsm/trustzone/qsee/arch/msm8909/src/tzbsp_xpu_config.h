#ifndef TZBSP_XPU_CONFIG_H
#define TZBSP_XPU_CONFIG_H

/*===========================================================================
Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_xpu_config.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/12   sg      Add BAM VMIDMT configurations
07/05/12   sg      MSA partitions need to be the highest priority
05/25/12   sg      Updated blist allocation defines
05/25/12   sg      Correcting VMID names for camss.
05/18/12   sg      VMIDs for 8974
03/08/12   amen    Video FW mem
09/14/10   tk      First version.
============================================================================*/

/* IDs for blacklist areas */
#define TZBSP_BLIST_DEVICE_MEMORY       0       /* Non-DDR memory, set at boot */
#define TZBSP_BLIST_WLAN                1       /* WLAN Area, dynamic */
#define TZBSP_BLIST_LPASS               2       /* LPASS area, dynamic. */
#define TZBSP_BLIST_VIDEO               3       /* Video Core, dynamic */
#define TZBSP_BLIST_SECCHANNEL_MSS      4       /* MSS Secure Channel */
#define TZBSP_BLIST_SECCHANNEL_LPASS    5       /* LPASS Secure Channel */
#define TZBSP_BLIST_SECCHANNEL_WLAN     6       /* WLAN Secure Channel */
#define TZBSP_BLIST_ALLOC_0             7       /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_1             8       /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_2             9       /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_3             10      /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_4             11      /* Dynamically allocated. */
#define TZBSP_BLIST_FIXED_SEC_DDR       12      /* Fixed secure DDR, static */
#define TZBSP_BLIST_SHARED_IMEM         13      /* Shared imem, static. */
#define TZBSP_BLIST_TZ_DDR              14      /* Statically allocated. */

/* Indices for EBI1 resource groups. */
#define TZBSP_EBI1_RG_VIDEO             0       /* Video Core */
#define TZBSP_EBI1_RG_WLAN              1       /* Riva memory, dynamic. */
#define TZBSP_EBI1_RG_TZ                2       /* With current TZ Code memory  */
#define TZBSP_EBI1_RG_RFSA              3       /* RFSA buffer  */
#define TZBSP_EBI1_RG_TZ_NON_RELOC      4       /* With current TZ Code memory  */
#define TZBSP_EBI1_RG_UNUSED_0            5       /* Unused */
#define TZBSP_EBI1_RG_SHARED            6       /* SMEM Legacy, static. */
#define TZBSP_EBI1_RG_MSS_WCNSS_SMD     7       /* Modem WCNSS SMEM */
#define TZBSP_EBI1_RG_AP_WCNSS_SMD      8       /* AP WCNSS SMEM */
#define TZBSP_EBI1_RG_MSS_AP_SMD        9       /* Modem AP SMEM */
#define TZBSP_EBI1_RG_SMD_TOC           10      /* Table of SMD descriptors*/
#define TZBSP_EBI1_RG_ALLOC_0           11      /* Dynamically allocated. */
#define TZBSP_EBI1_RG_ALLOC_1           12      /* Dynamically allocated. */
#define TZBSP_EBI1_RG_ALLOC_2           13      /* Dynamically allocated. */
#define TZBSP_EBI1_RG_ALLOC_3           14      /* Dynamically allocated. */
#define TZBSP_EBI1_RG_ALLOC_4           15      /* Dynamically allocated. */
#define TZBSP_EBI1_RG_UNUSED_1            16      /* Unused. */
#define TZBSP_EBI1_RG_SECCHANNEL_MSS    17      /* Secure Channel MSS */
#define TZBSP_EBI1_RG_SECCHANNEL_LPASS  18      /* Secure Channel LPASS */
#define TZBSP_EBI1_RG_DYN_TMP_1         19      /* On the fly MPU protection */
#define TZBSP_EBI1_RG_TZ_DIAG           20      /* TZ DIAG buffer */
/* MSA partitions need to be the highest */
#define TZBSP_EBI1_RG_MSA1              21      /* MSA 1st partition. */
#define TZBSP_EBI1_RG_MSA2              22      /* MSA 2nd partition. */
#define TZBSP_EBI1_RG_MSA3              23      /* MSA 3rd partition. */
#define TZBSP_EBI1_RG_MSA4              24      /* MSA 4th partition. */
#define TZBSP_EBI1_RG_MAX               25      /* Maximum resource group */

/* Indices for LPASS resource groups. */
#define TZBSP_LPASS_LPM_RG_FW           1       /* Firmware, dynamic. */

/* Indices for System IMEM resource groups. */
#define TZBSP_SYS_IMEM_RG_APP          0
#define TZBSP_SYS_IMEM_RG_TZ           1
#define TZBSP_SYS_IMEM_RG_SHARED       2

/* Indices for QPIC NAND resource groups. */
#define TZBSP_QPIC_NAND_RG_SBL          0       /* start of Flash to end of SBL */
#define TZBSP_QPIC_NAND_RG_MIBIB        1       /* Start of MIBIB to end of MIBIB */
#define TZBSP_QPIC_NAND_RG_RPM          2       /* Start of SDI to end of RPM */
#define TZBSP_QPIC_NAND_RG_MSS          3       /* start of QDSP to end of ADSP */
#define TZBSP_QPIC_NAND_RG_APPS         4       /* start of APPSBL to end of flash */
#define TZBSP_QPIC_NAND_RG_CACHE        5       /* Start of CHACHE to end of flash */
#define TZBSP_QPIC_NAND_RG_UNUSED_4     6       /* Unused partition */
#define TZBSP_QPIC_NAND_RG_EFS          7       /* start of EFS2 to end of EFS2 */

#endif /* TZBSP_XPU_CONFIG_H */
