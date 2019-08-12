#ifndef TZBSP_XPU_DEF_H
#define TZBSP_XPU_DEF_H

/*===========================================================================
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_xpu_def.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/10   tk      First version.
============================================================================*/

#include "tzbsp_hwio.h"

/* EBI channel interleaving related constants. TODO Not the correct place? */
#define TZBSP_EBI_CH_NONE             0x0
#define TZBSP_EBI_CH_0                0x1
#define TZBSP_EBI_CH_1                0x2
#define TZBSP_EBI_CH_BOTH             0x3 /* Interleaved. */

/* Resource group(s) for WLAN APU. */
#define TZBSP_WLAN_APU_RG_WLAN                  0

/* Resource groups for SLIMBUS APU. */
#define TZBSP_BAM_SLIMBUS_APU_RG_CONTROL            0
#define TZBSP_BAM_SLIMBUS_APU_RG_DEBUG              1
#define TZBSP_BAM_SLIMBUS_APU_RG_TRUST              2
#define TZBSP_BAM_SLIMBUS_APU_RG_EE0                3
#define TZBSP_BAM_SLIMBUS_APU_RG_EE1                4
#define TZBSP_BAM_SLIMBUS_APU_RG_EE2                5
#define TZBSP_BAM_SLIMBUS_APU_RG_EE3                6
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_TRUSTED         7
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_TEST            8
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_MANAGER         9
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_NGD1            10
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_NGD2            11
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_PRTIRQEE0       12
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_PRTIRQEE1       13
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_PRTIRQEE2       14
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_PCVFR0_1        15
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_PCVFR2_3        16
#define TZBSP_BAM_SLIMBUS_APU_RG_SB_PCVFR4_5        17
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE0              18
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE1              19
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE2              20
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE3              21
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE4              22
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE5              23
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE6              24
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE7              25
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE8              26
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE9              27
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE10             28
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE11             29
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE12             30
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE13             31
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE14             32
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE15             33
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE16             34
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE17             35
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE18             36
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE19             37
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE20             38
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE21             39
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE22             40
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE23             41
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE24             42
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE25             43
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE26             44
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE27             45
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE28             46
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE29             47
#define TZBSP_BAM_SLIMBUS_APU_RG_PIPE30             48

/* Resource groups for BAM DMA APU. */
#define TZBSP_BAM_APU_RG_BAM_CONTROL            0
#define TZBSP_BAM_APU_RG_EE0                    1
#define TZBSP_BAM_APU_RG_EE1                    2
#define TZBSP_BAM_APU_RG_EE2                    3
#define TZBSP_BAM_APU_RG_EE3                    4
#define TZBSP_BAM_APU_RG_DEBUG                  5
#define TZBSP_BAM_APU_RG_TRUST                  6
#define TZBSP_BAM_APU_RG_NON_PIPE_0             7
#define TZBSP_BAM_APU_RG_NON_PIPE_1             8
#define TZBSP_BAM_APU_RG_NON_PIPE_2             9
#define TZBSP_BAM_APU_RG_NON_PIPE_3             10
#define TZBSP_BAM_APU_RG_NON_PIPE_4             11
#define TZBSP_BAM_APU_RG_NON_PIPE_5             12
#define TZBSP_BAM_APU_RG_NON_PIPE_6             13
#define TZBSP_BAM_APU_RG_NON_PIPE_7             14
#define TZBSP_BAM_APU_RG_NON_PIPE_8             15
#define TZBSP_BAM_APU_RG_NON_PIPE_9             16
#define TZBSP_BAM_APU_RG_NON_PIPE_10            17
#define TZBSP_BAM_APU_RG_NON_PIPE_11            18
#define TZBSP_BAM_APU_RG_PIPE_0                 19
#define TZBSP_BAM_APU_RG_PIPE_1                 20
#define TZBSP_BAM_APU_RG_PIPE_2                 21
#define TZBSP_BAM_APU_RG_PIPE_3                 22
#define TZBSP_BAM_APU_RG_PIPE_4                 23
#define TZBSP_BAM_APU_RG_PIPE_5                 24
#define TZBSP_BAM_APU_RG_PIPE_6                 25
#define TZBSP_BAM_APU_RG_PIPE_7                 26
#define TZBSP_BAM_APU_RG_PIPE_8                 27
#define TZBSP_BAM_APU_RG_PIPE_9                 28
#define TZBSP_BAM_APU_RG_PIPE_10                29
#define TZBSP_BAM_APU_RG_PIPE_11                30
#define TZBSP_BAM_APU_RG_PIPE_12                31
#define TZBSP_BAM_APU_RG_PIPE_13                32
#define TZBSP_BAM_APU_RG_PIPE_14                33
#define TZBSP_BAM_APU_RG_PIPE_15                34
#define TZBSP_BAM_APU_RG_PIPE_16                35
#define TZBSP_BAM_APU_RG_PIPE_17                36
#define TZBSP_BAM_APU_RG_PIPE_18                37
#define TZBSP_BAM_APU_RG_PIPE_19                38

/* Converts from GCC RG address to RG index. */
#define GCC_A_TO_I(xx) \
    ((uint8)(((xx) - HWIO_GCC_SPARE_GPLL_ENA_VOTE_ADDR) / 0x1000))

#define TZBSP_GCC_RG_SPARE_GPLL_ENA_VOTE                GCC_A_TO_I(HWIO_GCC_SPARE_GPLL_ENA_VOTE_ADDR)
#define TZBSP_GCC_RG_BLSP1                              GCC_A_TO_I(HWIO_GCC_BLSP1_BCR_ADDR)
#define TZBSP_GCC_RG_BLSP1_QUP1                         GCC_A_TO_I(HWIO_GCC_BLSP1_QUP1_BCR_ADDR)
#define TZBSP_GCC_RG_BLSP1_QUP2_I2C_APPS_CMD            GCC_A_TO_I(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ADDR)
#define TZBSP_GCC_RG_BLSP1_QUP3_I2C_APPS_CMD            GCC_A_TO_I(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ADDR)
#define TZBSP_GCC_RG_BLSP1_QUP4_I2C_APPS_CMD            GCC_A_TO_I(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ADDR)
#define TZBSP_GCC_RG_BLSP1_QUP5_I2C_APPS_CMD            GCC_A_TO_I(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ADDR)
#define TZBSP_GCC_RG_BLSP1_QUP6_I2C_APPS_CMD            GCC_A_TO_I(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ADDR)
#define TZBSP_GCC_RG_GP1                                GCC_A_TO_I(HWIO_GCC_GP1_CBCR_ADDR)
#define TZBSP_GCC_RG_GP2                                GCC_A_TO_I(HWIO_GCC_GP2_CBCR_ADDR)
#define TZBSP_GCC_RG_GP3                                GCC_A_TO_I(HWIO_GCC_GP3_CBCR_ADDR)
#define TZBSP_GCC_RG_IMEM                               GCC_A_TO_I(HWIO_GCC_IMEM_BCR_ADDR)
#define TZBSP_GCC_RG_VENUS_RESTART                      GCC_A_TO_I(HWIO_GCC_VENUS_RESTART_ADDR)
#define TZBSP_GCC_RG_WCSS_RESTART                       GCC_A_TO_I(HWIO_GCC_WCSS_RESTART_ADDR)
#define TZBSP_GCC_RG_SMMU                               GCC_A_TO_I(HWIO_GCC_SMMU_BCR_ADDR)
#define TZBSP_GCC_RG_PRNG                               GCC_A_TO_I(HWIO_GCC_PRNG_BCR_ADDR)
#define TZBSP_GCC_RG_RESET_DEBUG                        GCC_A_TO_I(HWIO_GCC_RESET_DEBUG_ADDR)
#define TZBSP_GCC_RG_FLUSH_ETR_DEBUG_TIMER              GCC_A_TO_I(HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_ADDR)
#define TZBSP_GCC_RG_CRYPTO                             GCC_A_TO_I(HWIO_GCC_CRYPTO_BCR_ADDR)
#define TZBSP_GCC_RG_NOC_CONF_XPU_AHB                   GCC_A_TO_I(HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_ADDR)
#define TZBSP_GCC_RG_SEC_CTR                            GCC_A_TO_I(HWIO_GCC_SEC_CTRL_BCR_ADDR)
#define TZBSP_GCC_RG_MSS_Q6_GPLL_ENA_VOTE               GCC_A_TO_I(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR)
#define TZBSP_GCC_RG_ULTAUDIO_PCNOC_MPORT               GCC_A_TO_I(HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_ADDR)
#define TZBSP_GCC_RG_BLSP1_DEHR                         GCC_A_TO_I(HWIO_GCC_DEHR_BCR_ADDR)

/* Resource groups under Peripheral SS APU */
#define TZBSP_PERIPH_SS_RG_PRNG         0

/* Indices for OCMEM NOC MPU0 & MPU1 resource groups */
#define TZBSP_OCMEM_OCMEM_NOC_RG_0      0       /* On the fly MPU protection */
#define TZBSP_OCMEM_OCMEM_NOC_RG_1      1       /* On the fly MPU protection */
#define TZBSP_OCMEM_OCMEM_NOC_RG_2      2       /* On the fly MPU protection */
#define TZBSP_OCMEM_OCMEM_NOC_RG_3      3       /* On the fly MPU protection */
#define TZBSP_OCMEM_OCMEM_NOC_RG_4      4       /* On the fly MPU protection */
#define TZBSP_OCMEM_OCMEM_NOC_RG_5      5       /* On the fly MPU protection */
#define TZBSP_OCMEM_OCMEM_NOC_RG_6      6       /* On the fly MPU protection */
#define TZBSP_OCMEM_OCMEM_NOC_RG_7      7       /* On the fly MPU protection */


/* Indices for OCMEM SYSNOC MPU resource groups */
#define TZBSP_OCMEM_SYS_NOC_RG_0        0       /* On the fly MPU protection */
#define TZBSP_OCMEM_SYS_NOC_RG_1        1       /* On the fly MPU protection */
#define TZBSP_OCMEM_SYS_NOC_RG_2        2       /* On the fly MPU protection */
#define TZBSP_OCMEM_SYS_NOC_RG_3        3       /* On the fly MPU protection */
#define TZBSP_OCMEM_SYS_NOC_RG_4        4       /* On the fly MPU protection */
#define TZBSP_OCMEM_SYS_NOC_RG_5        5       /* On the fly MPU protection */
#define TZBSP_OCMEM_SYS_NOC_RG_6        6       /* On the fly MPU protection */
#define TZBSP_OCMEM_SYS_NOC_RG_DEBUG    7       /* On the fly MPU protection */


/* Indices for OCMEM DM MPU resource groups */
#define TZBSP_OCMEM_DM_RG_0             0       /* On the fly MPU protection */
#define TZBSP_OCMEM_DM_RG_1             1       /* On the fly MPU protection */
#define TZBSP_OCMEM_DM_RG_2             2       /* On the fly MPU protection */
#define TZBSP_OCMEM_DM_RG_3             3       /* On the fly MPU protection */
#define TZBSP_OCMEM_DM_RG_4             4       /* On the fly MPU protection */
#define TZBSP_OCMEM_DM_RG_5             5       /* On the fly MPU protection */
#define TZBSP_OCMEM_DM_RG_6             6       /* On the fly MPU protection */
#define TZBSP_OCMEM_DM_RG_7             7       /* On the fly MPU protection */


/* Indices for OCMEM core resource group */
#define TZBSP_OCMEM_CORE_APU_RG_UNPROTECTED     0
#define TZBSP_OCMEM_CORE_APU_RG_PROTECTED       1

/* Indices for OCMEM DM */
#define TZBSP_OCMEM_DM_APU_RG_UNPROTECTED       0
#define TZBSP_OCMEM_DM_APU_RG_PROTECTED         1

/* QFPROM region ID bit masks. */
#define TZBSP_QFPROM_CUSTOMER_PRIVATE_KEY1      (1UL << 31)
#define TZBSP_QFPROM_CUSTOMER_PRIVATE_KEY0      (1UL << 30)
#define TZBSP_QFPROM_HDCP_KEYS                  (1UL << 29)
#define TZBSP_QFPROM_CUSTOMER_KEY               (1UL << 28)
/* Regions 22-27 are unused spare regions. */
#define TZBSP_QFPROM_CALIBRATION                (1UL << 21)
#define TZBSP_QFPROM_MEMORY_ACCELERATION        (1UL << 20)
#define TZBSP_QFPROM_USB_VID_PID                (1UL << 19)
#define TZBSP_QFPROM_DEBUG_KEY                  (1UL << 18)
#define TZBSP_QFPROM_MEMORY_REDUNDANCY          (1UL << 17)
#define TZBSP_QFPROM_QC_SECURE_BOOT             (1UL << 16)
#define TZBSP_QFPROM_OEM_SECURE_BOOT            (1UL << 15)
#define TZBSP_QFPROM_OEM_PK_HASH                (1UL << 14)
#define TZBSP_QFPROM_PRIMARY_HW_KEY             (1UL << 13)
#define TZBSP_QFPROM_SECONDARY_HW_KEY           (1UL << 12)
#define TZBSP_QFPROM_FEATURE_CONFIG             (1UL << 11)
#define TZBSP_QFPROM_OEM_CONFIG                 (1UL << 10)
#define TZBSP_QFPROM_IMEI_ESN2                  (1UL << 9)
#define TZBSP_QFPROM_IMEI_ESN1                  (1UL << 8)
#define TZBSP_QFPROM_IMEI_ESN0                  (1UL << 7)
#define TZBSP_QFPROM_OEM_ANTI_REPLAY            (1UL << 6)
#define TZBSP_QFPROM_BOOT_ROM_PATCH             (1UL << 5)
#define TZBSP_QFPROM_PTE                        (1UL << 4)
#define TZBSP_QFPROM_WRITE_PERM                 (1UL << 3)
#define TZBSP_QFPROM_READ_PERM                  (1UL << 2)
#define TZBSP_QFPROM_JTAG_ID                    (1UL << 1)
#define TZBSP_QFPROM_ACC_PRIVATE                (1UL << 0)

/* Indices for TCSR Registers resource groups. */
#define TZBSP_TCSR_REG_RG_QRIB_1            0
#define TZBSP_TCSR_REG_RG_QRIB_2            1
#define TZBSP_TCSR_REG_RG_ACC_CLT_CFG_INTR  2
#define TZBSP_TCSR_REG_RG_MSA_INTR          3
#define TZBSP_TCSR_REG_RG_CLK_GATING        4
#define TZBSP_TCSR_REG_RG_TIMEOUT_INTR      5
#define TZBSP_TCSR_REG_RG_TZ_WONCE          6
#define TZBSP_TCSR_REG_RG_MMSS_RPM_INTR     7
#define TZBSP_TCSR_REG_RG_TBU_BYPASS        8 
#define TZBSP_TCSR_REG_RG_DEBUG_BYPASS      9 
#define TZBSP_TCSR_REG_RG_QPDI_CFG          10
#define TZBSP_TCSR_REG_RG_ACC               11
#define TZBSP_TCSR_REG_RG_APC1_ACC          12
#define TZBSP_TCSR_REG_RG_APC2_ACC          13
#define TZBSP_TCSR_REG_RG_UNUSED_14         14
#define TZBSP_TCSR_REG_RG_UNUSED_15         15

/* Indices for TCSR Registers resource groups. */
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG0      0
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG1      1
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG2      2
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG3      3
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG4      4
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG5      5
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG6      6
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG7      7
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG8      8
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG9      9
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG10     10
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG11     11
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG12     12
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG13     13
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG14     14
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG15     15
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG16     16
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG17     17
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG18     18
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG19     19
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG20     20
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG21     21
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG22     22
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG23     23
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG24     24
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG25     25
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG26     26
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG27     27
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG28     28
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG29     29
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG30     30
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG31     31
#define TZBSP_TCSR_MUTEX_RG_CRYPTO0_HALT    32
#define TZBSP_TCSR_MUTEX_RG_CRYPTO1_HALT    33
#define TZBSP_TCSR_MUTEX_RG_LPASS_HALT      34
#define TZBSP_TCSR_MUTEX_RG_MSS_Q6_HALT     35
#define TZBSP_TCSR_MUTEX_RG_MSS_MODEM_HALT  36
#define TZBSP_TCSR_MUTEX_RG_MSS_NC_HALT     37
#define TZBSP_TCSR_MUTEX_RG_WCSS_HALT       38
#define TZBSP_TCSR_MUTEX_RG_MUTEX_REG_RESET 39

/* Indices for Crypto APU resource grous */
#define TZBSP_CRYPTO0_RG_BAM_CONTROL    0
#define TZBSP_CRYPTO0_RG_BAM_DEBUG      1
#define TZBSP_CRYPTO0_RG_BAM_TRUST      2
#define TZBSP_CRYPTO0_RG_BAM_EE0        3
#define TZBSP_CRYPTO0_RG_BAM_EE1        4
#define TZBSP_CRYPTO0_RG_BAM_EE2        5
#define TZBSP_CRYPTO0_RG_BAM_EE3        6
#define TZBSP_CRYPTO0_RG_CFG            7
#define TZBSP_CRYPTO0_RG_QC_KEY         8
#define TZBSP_CRYPTO0_RG_OEM_KEY        9
#define TZBSP_CRYPTO0_RG_LEGACY_SW_KEY  10
#define TZBSP_CRYPTO0_RG_PIPE_SW_KEY    11
#define TZBSP_CRYPTO0_RG_DEBUG          12
#define TZBSP_CRYPTO0_RG_BAM_PIPE0      13
#define TZBSP_CRYPTO0_RG_BAM_PIPE1      14
#define TZBSP_CRYPTO0_RG_BAM_PIPE2      15
#define TZBSP_CRYPTO0_RG_BAM_PIPE3      16
#define TZBSP_CRYPTO0_RG_BAM_PIPE4      17
#define TZBSP_CRYPTO0_RG_BAM_PIPE5      18
#define TZBSP_CRYPTO0_RG_BAM_PIPE6      19
#define TZBSP_CRYPTO0_RG_BAM_PIPE7      20

#define TZBSP_QPIC_RG_BAM_CONTROL    0
#define TZBSP_QPIC_RG_BAM_DEBUG      1
#define TZBSP_QPIC_RG_BAM_TRUST      2
#define TZBSP_QPIC_RG_BAM_EE0        3
#define TZBSP_QPIC_RG_BAM_EE1        4
#define TZBSP_QPIC_RG_BAM_EE2        5
#define TZBSP_QPIC_RG_BAM_EE3        6
#define TZBSP_QPIC_RG_CFG            7
#define TZBSP_QPIC_RG_OPERATIONAL    8
#define TZBSP_QPIC_RG_SUPERVISOR     9
#define TZBSP_QPIC_RG_PROTECTED      10
#define TZBSP_QPIC_RG_LCDc           11
#define TZBSP_QPIC_RG_OBSOLETE       12
#define TZBSP_QPIC_RG_MPU_CONFIG     13
#define TZBSP_QPIC_RG_BAM_PIPE0      14
#define TZBSP_QPIC_RG_BAM_PIPE1      15
#define TZBSP_QPIC_RG_BAM_PIPE2      16
#define TZBSP_QPIC_RG_BAM_PIPE3      17
#define TZBSP_QPIC_RG_BAM_PIPE4      18
#define TZBSP_QPIC_RG_BAM_PIPE5      19
#define TZBSP_QPIC_RG_BAM_PIPE6      20

#define TZBSP_SEC_CTRL_RG0              0
#define TZBSP_SEC_CTRL_RG1              1
#define TZBSP_SEC_CTRL_RG2              2
#define TZBSP_SEC_CTRL_RG3              3
#define TZBSP_SEC_CTRL_RG4              4
#define TZBSP_SEC_CTRL_RG5              5
#define TZBSP_SEC_CTRL_RG6              6
#define TZBSP_SEC_CTRL_RG7              7
#define TZBSP_SEC_CTRL_RG8              8
#define TZBSP_SEC_CTRL_RG9              9

#define TZBSP_BOOTROM_RG_MSS            1
#define TZBSP_BOOTROM_RG_APPS           2
#define TZBSP_BOOTROM_RG_UNUSED         0

#define TZBSP_BAM_BLSP1_RG_CONTROL      0
#define TZBSP_BAM_BLSP1_RG_DEBUG        1
#define TZBSP_BAM_BLSP1_RG_TRUST        2
#define TZBSP_BAM_BLSP1_RG_EE0          3
#define TZBSP_BAM_BLSP1_RG_EE1          4
#define TZBSP_BAM_BLSP1_RG_EE2          5
#define TZBSP_BAM_BLSP1_RG_EE3          6
#define TZBSP_BAM_BLSP1_RG_UART0        7
#define TZBSP_BAM_BLSP1_RG_UART1        8
#define TZBSP_BAM_BLSP1_RG_QUP0         9
#define TZBSP_BAM_BLSP1_RG_QUP1         10
#define TZBSP_BAM_BLSP1_RG_QUP2         11
#define TZBSP_BAM_BLSP1_RG_QUP3         12
#define TZBSP_BAM_BLSP1_RG_QUP4         13
#define TZBSP_BAM_BLSP1_RG_QUP5         14
#define TZBSP_BAM_BLSP1_RG_PIPE0        15
#define TZBSP_BAM_BLSP1_RG_PIPE1        16
#define TZBSP_BAM_BLSP1_RG_PIPE2        17
#define TZBSP_BAM_BLSP1_RG_PIPE3        18
#define TZBSP_BAM_BLSP1_RG_PIPE4        19
#define TZBSP_BAM_BLSP1_RG_PIPE5        20
#define TZBSP_BAM_BLSP1_RG_PIPE6        21
#define TZBSP_BAM_BLSP1_RG_PIPE7        22
#define TZBSP_BAM_BLSP1_RG_PIPE8        23
#define TZBSP_BAM_BLSP1_RG_PIPE9        24
#define TZBSP_BAM_BLSP1_RG_PIPE10       25
#define TZBSP_BAM_BLSP1_RG_PIPE11       26
#define TZBSP_BAM_BLSP1_RG_PIPE12       27
#define TZBSP_BAM_BLSP1_RG_PIPE13       28
#define TZBSP_BAM_BLSP1_RG_PIPE14       29
#define TZBSP_BAM_BLSP1_RG_PIPE15       30

#define TZBSP_TLMM_RG_GPIO              0
#define TZBSP_TLMM_RG_DIR_CONN1         1
#define TZBSP_TLMM_RG_DIR_CONN2         2
#define TZBSP_TLMM_RG_DIR_CONN3         3
#define TZBSP_TLMM_RG_DIR_CONN4         4
#define TZBSP_TLMM_RG_RFEE_CTRL         5
#define TZBSP_TLMM_RG_PMIC_HDRV_PULL    6
#define TZBSP_TLMM_RG_MISC_RPM          7

#define TZBSP_BAM_BLSP2_RG_CONTROL      0
#define TZBSP_BAM_BLSP2_RG_DEBUG        1
#define TZBSP_BAM_BLSP2_RG_TRUST        2
#define TZBSP_BAM_BLSP2_RG_EE0          3
#define TZBSP_BAM_BLSP2_RG_EE1          4
#define TZBSP_BAM_BLSP2_RG_EE2          5
#define TZBSP_BAM_BLSP2_RG_EE3          6
#define TZBSP_BAM_BLSP2_RG_UART0        7
#define TZBSP_BAM_BLSP2_RG_UART1        8
#define TZBSP_BAM_BLSP2_RG_UART2        9
#define TZBSP_BAM_BLSP2_RG_UART3        10
#define TZBSP_BAM_BLSP2_RG_UART4        11
#define TZBSP_BAM_BLSP2_RG_UART5        12
#define TZBSP_BAM_BLSP2_RG_QUP0         13
#define TZBSP_BAM_BLSP2_RG_QUP1         14
#define TZBSP_BAM_BLSP2_RG_QUP2         15
#define TZBSP_BAM_BLSP2_RG_QUP3         16
#define TZBSP_BAM_BLSP2_RG_QUP4         17
#define TZBSP_BAM_BLSP2_RG_QUP5         18
#define TZBSP_BAM_BLSP2_RG_PIPE0        19
#define TZBSP_BAM_BLSP2_RG_PIPE1        20
#define TZBSP_BAM_BLSP2_RG_PIPE2        21
#define TZBSP_BAM_BLSP2_RG_PIPE3        22
#define TZBSP_BAM_BLSP2_RG_PIPE4        23
#define TZBSP_BAM_BLSP2_RG_PIPE5        24
#define TZBSP_BAM_BLSP2_RG_PIPE6        25
#define TZBSP_BAM_BLSP2_RG_PIPE7        26
#define TZBSP_BAM_BLSP2_RG_PIPE8        27
#define TZBSP_BAM_BLSP2_RG_PIPE9        28
#define TZBSP_BAM_BLSP2_RG_PIPE10       29
#define TZBSP_BAM_BLSP2_RG_PIPE11       30
#define TZBSP_BAM_BLSP2_RG_PIPE12       31
#define TZBSP_BAM_BLSP2_RG_PIPE13       32
#define TZBSP_BAM_BLSP2_RG_PIPE14       33
#define TZBSP_BAM_BLSP2_RG_PIPE15       34
#define TZBSP_BAM_BLSP2_RG_PIPE16       35
#define TZBSP_BAM_BLSP2_RG_PIPE17       36
#define TZBSP_BAM_BLSP2_RG_PIPE18       37
#define TZBSP_BAM_BLSP2_RG_PIPE19       38
#define TZBSP_BAM_BLSP2_RG_PIPE20       39
#define TZBSP_BAM_BLSP2_RG_PIPE21       40
#define TZBSP_BAM_BLSP2_RG_PIPE22       41
#define TZBSP_BAM_BLSP2_RG_PIPE23       42

#define TZBSP_BAM_SDCC1_RG_CONTROL      0
#define TZBSP_BAM_SDCC1_RG_DEBUG        1
#define TZBSP_BAM_SDCC1_RG_TRUST        2
#define TZBSP_BAM_SDCC1_RG_EE0          3
#define TZBSP_BAM_SDCC1_RG_NONPIPE0     4
#define TZBSP_BAM_SDCC1_RG_PIPE0        5
#define TZBSP_BAM_SDCC1_RG_PIPE1        6
#define TZBSP_BAM_SDCC1_RG_PIPE2        7
#define TZBSP_BAM_SDCC1_RG_PIPE3        8
#define TZBSP_BAM_SDCC1_RG_PIPE4        9
#define TZBSP_BAM_SDCC1_RG_PIPE5        10

#define TZBSP_BAM_SDCC2_RG_CONTROL      0
#define TZBSP_BAM_SDCC2_RG_DEBUG        1
#define TZBSP_BAM_SDCC2_RG_TRUST        2
#define TZBSP_BAM_SDCC2_RG_EE0          3
#define TZBSP_BAM_SDCC2_RG_NONPIPE0     4
#define TZBSP_BAM_SDCC2_RG_PIPE0        5
#define TZBSP_BAM_SDCC2_RG_PIPE1        6
#define TZBSP_BAM_SDCC2_RG_PIPE2        7
#define TZBSP_BAM_SDCC2_RG_PIPE3        8
#define TZBSP_BAM_SDCC2_RG_PIPE4        9
#define TZBSP_BAM_SDCC2_RG_PIPE5        10

#define TZBSP_BAM_SDCC3_RG_CONTROL      0
#define TZBSP_BAM_SDCC3_RG_DEBUG        1
#define TZBSP_BAM_SDCC3_RG_TRUST        2
#define TZBSP_BAM_SDCC3_RG_EE0          3
#define TZBSP_BAM_SDCC3_RG_NONPIPE0     4
#define TZBSP_BAM_SDCC3_RG_PIPE0        5
#define TZBSP_BAM_SDCC3_RG_PIPE1        6
#define TZBSP_BAM_SDCC3_RG_PIPE2        7
#define TZBSP_BAM_SDCC3_RG_PIPE3        8
#define TZBSP_BAM_SDCC3_RG_PIPE4        9
#define TZBSP_BAM_SDCC3_RG_PIPE5        10

#define TZBSP_BAM_SDCC4_RG_CONTROL      0
#define TZBSP_BAM_SDCC4_RG_DEBUG        1
#define TZBSP_BAM_SDCC4_RG_TRUST        2
#define TZBSP_BAM_SDCC4_RG_EE0          3
#define TZBSP_BAM_SDCC4_RG_NONPIPE0     4
#define TZBSP_BAM_SDCC4_RG_PIPE0        5
#define TZBSP_BAM_SDCC4_RG_PIPE1        6
#define TZBSP_BAM_SDCC4_RG_PIPE2        7
#define TZBSP_BAM_SDCC4_RG_PIPE3        8
#define TZBSP_BAM_SDCC4_RG_PIPE4        9
#define TZBSP_BAM_SDCC4_RG_PIPE5        10

#define TZBSP_BAM_TSIF_RG_CONTROL      0
#define TZBSP_BAM_TSIF_RG_DEBUG        1
#define TZBSP_BAM_TSIF_RG_TRUST        2
#define TZBSP_BAM_TSIF_RG_EE0          3
#define TZBSP_BAM_TSIF_RG_NONPIPE0     4
#define TZBSP_BAM_TSIF_RG_PIPE0        5
#define TZBSP_BAM_TSIF_RG_PIPE1        6
#define TZBSP_BAM_TSIF_RG_PIPE2        7
#define TZBSP_BAM_TSIF_RG_PIPE3        8
#define TZBSP_BAM_TSIF_RG_PIPE4        9
#define TZBSP_BAM_TSIF_RG_PIPE5        10
#define TZBSP_BAM_TSIF_RG_PIPE6        11
#define TZBSP_BAM_TSIF_RG_PIPE7        12
#define TZBSP_BAM_TSIF_RG_PIPE8        13
#define TZBSP_BAM_TSIF_RG_PIPE9        14
#define TZBSP_BAM_TSIF_RG_PIPE10       15
#define TZBSP_BAM_TSIF_RG_PIPE11       16
#define TZBSP_BAM_TSIF_RG_PIPE12       17
#define TZBSP_BAM_TSIF_RG_PIPE13       18
#define TZBSP_BAM_TSIF_RG_PIPE14       19
#define TZBSP_BAM_TSIF_RG_PIPE15       20

#define TZBSP_BAM_DMA_RG_CONTROL      0
#define TZBSP_BAM_DMA_RG_DEBUG        1
#define TZBSP_BAM_DMA_RG_TRUST        2
#define TZBSP_BAM_DMA_RG_EE0          3
#define TZBSP_BAM_DMA_RG_EE1          4
#define TZBSP_BAM_DMA_RG_EE2          5
#define TZBSP_BAM_DMA_RG_EE3          6
#define TZBSP_BAM_DMA_RG_EE4          7
#define TZBSP_BAM_DMA_RG_EE5          8
#define TZBSP_BAM_DMA_RG_EE6          9
#define TZBSP_BAM_DMA_RG_EE7          10
#define TZBSP_BAM_DMA_RG_DMA_ENA      11
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG0 12
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG1 13
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG2 14
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG3 15
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG4 16
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG5 17
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG6 18
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG7 19
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG8 20
#define TZBSP_BAM_DMA_RG_DMA_CHL_CFG9 21
#define TZBSP_BAM_DMA_RG_DMA_REV_CFG  22
#define TZBSP_BAM_DMA_RG_PIPE0        23
#define TZBSP_BAM_DMA_RG_PIPE1        24
#define TZBSP_BAM_DMA_RG_PIPE2        25
#define TZBSP_BAM_DMA_RG_PIPE3        26
#define TZBSP_BAM_DMA_RG_PIPE4        27
#define TZBSP_BAM_DMA_RG_PIPE5        28
#define TZBSP_BAM_DMA_RG_PIPE6        29
#define TZBSP_BAM_DMA_RG_PIPE7        30
#define TZBSP_BAM_DMA_RG_PIPE8        31
#define TZBSP_BAM_DMA_RG_PIPE9        32
#define TZBSP_BAM_DMA_RG_PIPE10       33
#define TZBSP_BAM_DMA_RG_PIPE11       34
#define TZBSP_BAM_DMA_RG_PIPE12       35
#define TZBSP_BAM_DMA_RG_PIPE13       36
#define TZBSP_BAM_DMA_RG_PIPE14       37
#define TZBSP_BAM_DMA_RG_PIPE15       38
#define TZBSP_BAM_DMA_RG_PIPE16       39
#define TZBSP_BAM_DMA_RG_PIPE17       40
#define TZBSP_BAM_DMA_RG_PIPE18       41
#define TZBSP_BAM_DMA_RG_PIPE19       42

#define TZBSP_BAM_USB1_HS_RG_CONTROL  0
#define TZBSP_BAM_USB1_HS_RG_DEBUG    1
#define TZBSP_BAM_USB1_HS_RG_TRUST    2
#define TZBSP_BAM_USB1_HS_RG_EE0      3
#define TZBSP_BAM_USB1_HS_RG_NONPIPE0 4
#define TZBSP_BAM_USB1_HS_RG_PIPE0    5
#define TZBSP_BAM_USB1_HS_RG_PIPE1    6
#define TZBSP_BAM_USB1_HS_RG_PIPE2    7
#define TZBSP_BAM_USB1_HS_RG_PIPE3    8
#define TZBSP_BAM_USB1_HS_RG_PIPE4    9
#define TZBSP_BAM_USB1_HS_RG_PIPE5    10
#define TZBSP_BAM_USB1_HS_RG_PIPE6    11
#define TZBSP_BAM_USB1_HS_RG_PIPE7    12
#define TZBSP_BAM_USB1_HS_RG_PIPE8    13
#define TZBSP_BAM_USB1_HS_RG_PIPE9    14
#define TZBSP_BAM_USB1_HS_RG_PIPE10   15
#define TZBSP_BAM_USB1_HS_RG_PIPE11   16
#define TZBSP_BAM_USB1_HS_RG_PIPE12   17
#define TZBSP_BAM_USB1_HS_RG_PIPE13   18
#define TZBSP_BAM_USB1_HS_RG_PIPE14   19
#define TZBSP_BAM_USB1_HS_RG_PIPE15   20

#define TZBSP_BAM_QDSS_RG_RBCPR       0
#define TZBSP_BAM_QDSS_RG_QDSS        1

#define TZBSP_PMIC_ARB_RG_CH0         0
#define TZBSP_PMIC_ARB_RG_CH1         1
#define TZBSP_PMIC_ARB_RG_CH2         2
#define TZBSP_PMIC_ARB_RG_CH3         3
#define TZBSP_PMIC_ARB_RG_CH4         4
#define TZBSP_PMIC_ARB_RG_CH5         5
#define TZBSP_PMIC_ARB_RG_ARB_CFG     6
#define TZBSP_PMIC_ARB_RG_SPMI_CFG    7

/* note: as SNOC CFG xPU config address width is 9, and client address access
 * is 7, leading in address miss config. i.e.. instead of 0x00587000 config as 0x00007000 */
#define TZBSP_SNOC_CFG_RG_ERR                0
#define TZBSP_SNOC_CFG_RG_ERR_BASE           0x00000000
#define TZBSP_SNOC_CFG_RG_ERR_END            0x00007000
#define TZBSP_SNOC_CFG_RG_QOS_MSS            1
#define TZBSP_SNOC_CFG_RG_QOS_MSS_BASE       0x00007000
#define TZBSP_SNOC_CFG_RG_QOS_MSS_END        0x00009000
#define TZBSP_SNOC_CFG_RG_MSA_BLOCK          2
#define TZBSP_SNOC_CFG_RG_MSA_BLOCK_BASE     0x0000B000
#define TZBSP_SNOC_CFG_RG_MSA_BLOCK_END      0x00013000

/* note: as PCNOC CFG xPU config address width is 9, leading 
 * in address miss configuration. i.e.. instead of 00501000 config as 0x0001000 */
#define TZBSP_PCNOC_CFG_RG_ERR                0
#define TZBSP_PCNOC_CFG_RG_ERR_BASE           0x00000000
#define TZBSP_PCNOC_CFG_RG_ERR_END            0x00001000
#define TZBSP_PCNOC_PROF                      1
#define TZBSP_PCNOC_PROF_RG_BASE              0x00001000
#define TZBSP_PCNOC_PROF_RG_END               0x00007000
#define TZBSP_PCNOC_RG2                       2
#define TZBSP_PCNOC_RG3                       3

#define TZBSP_MSS_APU_RG_RMB          0
#define TZBSP_MSS_APU_RG_A2BAM        1
#define TZBSP_MSS_APU_RG_Q6_RST       2
#define TZBSP_MSS_APU_RG_MSS_CFG      3
#define TZBSP_MSS_APU_RG_ACC_SELECT   4

#define TZBSP_DEHR_APU_RG_SEC         0
#define TZBSP_DEHR_APU_RG_CFG         1

#define TZBSP_EBI1_CFG_RG_PHY         0
#define TZBSP_EBI1_CFG_RG_DLL         1

/*
 * Peripheral MPU addresses need to be masked because of a bug in HW due to
 * which MSB is not taken into consideration at client interface, MSB being
 * 15 for this xPU.
 */
#define TZBSP_PERIPH_MPU_RG_OBS_ERR_LOG      0
#define TZBSP_PERIPH_MPU_RG_OBS_ERR_LOG_BASE 0x00500000 
#define TZBSP_PERIPH_MPU_RG_OBS_ERR_LOG_END  0x00501000
#define TZBSP_PERIPH_MPU_RG_PERFMON          1
#define TZBSP_PERIPH_MPU_RG_PERFMON_BASE     0x00501000 
#define TZBSP_PERIPH_MPU_RG_PERFMON_END      0x00507000 

#define TZBSP_RPM_MPU_RG_HW_RES              0
#define TZBSP_RPM_MPU_RG_HW_RES_BASE         0xFC180000
#define TZBSP_RPM_MPU_RG_HW_RES_END          0xFC189000
#define TZBSP_RPM_MPU_RG_HW_DATA_INT         1
#define TZBSP_RPM_MPU_RG_HW_DATA_INT_BASE    0xFC190000
#define TZBSP_RPM_MPU_RG_HW_DATA_INT_END     0xFC19DC00
#define TZBSP_RPM_MPU_RG_HW_DATA_SHARED      2
#define TZBSP_RPM_MPU_RG_HW_DATA_SHARED_BASE 0xFC19DC00
#define TZBSP_RPM_MPU_RG_HW_DATA_SHARED_END  0xFC1A0000

#define TZBSP_BIMC_APU_RG_GLB_SLVWAY1 0
#define TZBSP_BIMC_APU_RG_GLB_SLVWAY2 1
#define TZBSP_BIMC_APU_RG_PERFMON     2
#define TZBSP_BIMC_APU_RG_MP0         3
#define TZBSP_BIMC_APU_RG_MP1         4
#define TZBSP_BIMC_APU_RG_MP2         5
#define TZBSP_BIMC_APU_RG_MP3         6
#define TZBSP_BIMC_APU_RG_MP4         7
#define TZBSP_BIMC_APU_RG_MP5         8
#define TZBSP_BIMC_APU_RG_MP6         9
#define TZBSP_BIMC_APU_RG_UNUSED10    10
#define TZBSP_BIMC_APU_RG_UNUSED11    11
#define TZBSP_BIMC_APU_RG_UNUSED12    12
#define TZBSP_BIMC_APU_RG_UNUSED13    13
#define TZBSP_BIMC_APU_RG_UNUSED14    14
#define TZBSP_BIMC_APU_RG_UNUSED15    15

#define TZBSP_MPM2_APU_RG_REGS           0
#define TZBSP_MPM2_APU_RG_GLB_CTL_CTR    1
#define TZBSP_MPM2_APU_RG_GLB_RD_CTR     2
#define TZBSP_MPM2_APU_RG_SLEEP_CTR      3
#define TZBSP_MPM2_APU_RG_QTMR_AC        4
#define TZBSP_MPM2_APU_RG_QTMR_V1        5
#define TZBSP_MPM2_APU_RG_TSYNC          6
#define TZBSP_MPM2_APU_RG_TEMP_SENSOR    7
#define TZBSP_MPM2_APU_RG_TEMP_SENSOR_TM 8
#define TZBSP_MPM2_APU_RG_WDOG           9
#define TZBSP_MPM2_APU_RG_PSHOLD         10

#define TZBSP_WLAN_RG_GEN                0
#define TZBSP_WLAN_RG_BOOT               1

#define TZBSP_IMEM_APU_RG_NSEC           0
#define TZBSP_IMEM_APU_RG_SEC            1

#define TZBSP_VENUS_RG_VOCODEC            0
#define TZBSP_VENUS_RG_SEC_AND_ARM9_RST   1
#define TZBSP_VENUS_RG_SEC_VBIF           2

#define TZBSP_VENUS_VBIF_RG_VBIF0         0
#define TZBSP_VENUS_VBIF_RG_VBIF1         1
#define TZBSP_VENUS_VBIF_RG_SMMU_GLB0     2
#define TZBSP_VENUS_VBIF_RG_SMMU_GLB1     3
#define TZBSP_VENUS_VBIF_RG_SMMU_IMP_DEF0 4
#define TZBSP_VENUS_VBIF_RG_SMMU_IMP_DEF1 5
#define TZBSP_VENUS_VBIF_RG_SMMU_PERF_MON 6
#define TZBSP_VENUS_VBIF_RG_SMMU_SSD      7
#define TZBSP_VENUS_VBIF_RG_SMMU_CB0      8
#define TZBSP_VENUS_VBIF_RG_SMMU_CB1      9
#define TZBSP_VENUS_VBIF_RG_SMMU_CB2      10
#define TZBSP_VENUS_VBIF_RG_SMMU_CB3      11

#define TZBSP_MDSS_RG_MDSS_CFG            0
#define TZBSP_MDSS_RG_RG1                 1

#define TZBSP_MDSS_VBIF_RG_VBIF0          0
#define TZBSP_MDSS_VBIF_RG_VBIF1          1
#define TZBSP_MDSS_VBIF_RG_SMMU_GLB0      2
#define TZBSP_MDSS_VBIF_RG_SMMU_GLB1      3
#define TZBSP_MDSS_VBIF_RG_SMMU_IMP_DEF0  4
#define TZBSP_MDSS_VBIF_RG_SMMU_IMP_DEF1  5
#define TZBSP_MDSS_VBIF_RG_SMMU_PERF_MON  6
#define TZBSP_MDSS_VBIF_RG_SMMU_SSD       7
#define TZBSP_MDSS_VBIF_RG_SMMU_CB0       8
#define TZBSP_MDSS_VBIF_RG_SMMU_CB1       9
#define TZBSP_MDSS_VBIF_RG_SMMU_CB2       10
#define TZBSP_MDSS_VBIF_RG_SMMU_CB3       11

#define TZBSP_MMSS_MNOC_RG_ERROR          0
#define TZBSP_MMSS_MNOC_RG_ERROR_BASE     0xFC478000
#define TZBSP_MMSS_MNOC_RG_ERROR_END      0xFC478FFF
#define TZBSP_MMSS_MNOC_RG_PROFILING      1
#define TZBSP_MMSS_MNOC_RG_PROFILING_BASE 0xFC479000
#define TZBSP_MMSS_MNOC_RG_PROFILING_END  0xFC47AFFF
#define TZBSP_MMSS_MNOC_RG_GEMINIQOS      2
#define TZBSP_MMSS_MNOC_RG_GEMINIQOS_BASE 0xFC47B000
#define TZBSP_MMSS_MNOC_RG_GEMINIQOS_END  0xFC47B07F
#define TZBSP_MMSS_MNOC_RG_MDPQOS         3
#define TZBSP_MMSS_MNOC_RG_MDPQOS_BASE    0xFC47B080
#define TZBSP_MMSS_MNOC_RG_MDPQOS_END     0xFC47B0FF
#define TZBSP_MMSS_MNOC_RG_OXILI0QOS      4
#define TZBSP_MMSS_MNOC_RG_OXILI0QOS_BASE 0xFC47B100
#define TZBSP_MMSS_MNOC_RG_OXILI0QOS_END  0xFC47B17F
#define TZBSP_MMSS_MNOC_RG_OXILI1QOS      5
#define TZBSP_MMSS_MNOC_RG_OXILI1QOS_BASE 0xFC47B180
#define TZBSP_MMSS_MNOC_RG_OXILI1QOS_END  0xFC47B1FF
#define TZBSP_MMSS_MNOC_RG_VENUS0QOS      6
#define TZBSP_MMSS_MNOC_RG_VENUS0QOS_BASE 0xFC47B200
#define TZBSP_MMSS_MNOC_RG_VENUS0QOS_END  0xFC47B27F
#define TZBSP_MMSS_MNOC_RG_VENUS1QOS      7
#define TZBSP_MMSS_MNOC_RG_VENUS1QOS_BASE 0xFC47B280
#define TZBSP_MMSS_MNOC_RG_VENUS1QOS_END  0xFC47B2FF
#define TZBSP_MMSS_MNOC_RG_VFEQOS         8
#define TZBSP_MMSS_MNOC_RG_VFEQOS_BASE    0xFC47B300
#define TZBSP_MMSS_MNOC_RG_VFEQOS_END     0xFC47B3FF

#define TZBSP_MMSS_ONOC_RG_ERROR          0
#define TZBSP_MMSS_ONOC_RG_ERROR_BASE     0xFC470000
#define TZBSP_MMSS_ONOC_RG_ERROR_END      0xFC470FFF
#define TZBSP_MMSS_ONOC_RG_PROFILING      1
#define TZBSP_MMSS_ONOC_RG_PROFILING_BASE 0xFC471000
#define TZBSP_MMSS_ONOC_RG_PROFILING_END  0xFC472FFF
#define TZBSP_MMSS_ONOC_RG_GEMINIQOS      2
#define TZBSP_MMSS_ONOC_RG_GEMINIQOS_BASE 0xFC473000
#define TZBSP_MMSS_ONOC_RG_GEMINIQOS_END  0xFC4730FF
#define TZBSP_MMSS_ONOC_RG_VENUS0QOS      3
#define TZBSP_MMSS_ONOC_RG_VENUS0QOS_BASE 0xFC473100
#define TZBSP_MMSS_ONOC_RG_VENUS0QOS_END  0xFC47317F
#define TZBSP_MMSS_ONOC_RG_VENUS1QOS      4
#define TZBSP_MMSS_ONOC_RG_VENUS1QOS_BASE 0xFC473180
#define TZBSP_MMSS_ONOC_RG_VENUS1QOS_END  0xFC4731FF
#define TZBSP_MMSS_ONOC_RG_VFEQOS         5
#define TZBSP_MMSS_ONOC_RG_VFEQOS_BASE    0xFC473200
#define TZBSP_MMSS_ONOC_RG_VFEQOS_END     0xFC47327F

#define TZBSP_MMSS_MISC_RG_DEFAULT        0

#define TZBSP_MMSS_CC_RG_PLL         0
#define TZBSP_MMSS_CC_RG_VENUS       1
#define TZBSP_MMSS_CC_RG_MDSS        2
#define TZBSP_MMSS_CC_RG_CAMERA      3
#define TZBSP_MMSS_CC_RG_OXILIOCMEM  4
#define TZBSP_MMSS_CC_RG_AHBAXIOCMEM 5

#define TZBSP_SPDM_RG_SREG           0

//#define TZBSP_LPASS_CORE_RG_MIDI     0
#define TZBSP_LPASS_CORE_RG_AVSYNC   1
#define TZBSP_LPASS_CORE_RG_AHBI     2
#define TZBSP_LPASS_CORE_RG_SPDM     3

#define TZBSP_LPASS_TOP_RG_TCSR         0
#define TZBSP_LPASS_TOP_RG_AHBE         1
#define TZBSP_LPASS_TOP_RG_BLK_REMAPPER 2
#define TZBSP_LPASS_TOP_RG_BUS_TIMEOUT  3

#define TZBSP_LPASS_CSR_RG0             0
#define TZBSP_LPASS_CSR_RG1             1
#define TZBSP_LPASS_CSR_RG2             2
#define TZBSP_LPASS_CSR_RG3             3
#define TZBSP_LPASS_CSR_RG4             4
#define TZBSP_LPASS_CSR_RG5             5
#define TZBSP_LPASS_CSR_RG6             6

#define TZBSP_LPASS_LCC_RG0             0
#define TZBSP_LPASS_LCC_RG1             1
#define TZBSP_LPASS_LCC_RG2             2
#define TZBSP_LPASS_LCC_RG3             3
#define TZBSP_LPASS_LCC_RG4             4
#define TZBSP_LPASS_LCC_RG5             5
#define TZBSP_LPASS_LCC_RG6             6
#define TZBSP_LPASS_LCC_RG7             7
#define TZBSP_LPASS_LCC_RG8             8
#define TZBSP_LPASS_LCC_RG9             9
#define TZBSP_LPASS_LCC_RG10            10
#define TZBSP_LPASS_LCC_RG11            11
#define TZBSP_LPASS_LCC_RG12            12
#define TZBSP_LPASS_LCC_RG13            13
#define TZBSP_LPASS_LCC_RG14            14
#define TZBSP_LPASS_LCC_RG15            15
#define TZBSP_LPASS_LCC_RG16            16
#define TZBSP_LPASS_LCC_RG17            17
#define TZBSP_LPASS_LCC_RG18            18
#define TZBSP_LPASS_LCC_RG19            19
#define TZBSP_LPASS_LCC_RG20            20
#define TZBSP_LPASS_LCC_RG21            21
#define TZBSP_LPASS_LCC_RG22            22
#define TZBSP_LPASS_LCC_RG23            23
#define TZBSP_LPASS_LCC_RG24            24
#define TZBSP_LPASS_LCC_RG25            25
#define TZBSP_LPASS_LCC_RG26            26
#define TZBSP_LPASS_LCC_RG27            27
#define TZBSP_LPASS_LCC_RG28            28
#define TZBSP_LPASS_LCC_RG29            29
#define TZBSP_LPASS_LCC_RG30            30
#define TZBSP_LPASS_LCC_RG31            31
#define TZBSP_LPASS_LCC_RG32            32
#define TZBSP_LPASS_LCC_RG33            33
#define TZBSP_LPASS_LCC_RG34            34
#define TZBSP_LPASS_LCC_RG35            35
#define TZBSP_LPASS_LCC_RG36            36
#define TZBSP_LPASS_LCC_RG37            37
#define TZBSP_LPASS_LCC_RG38            38
#define TZBSP_LPASS_LCC_RG39            39
#define TZBSP_LPASS_LCC_RG40            20
#define TZBSP_LPASS_LCC_RG41            41
#define TZBSP_LPASS_LCC_RG42            42
#define TZBSP_LPASS_LCC_RG43            43
#define TZBSP_LPASS_LCC_RG44            44
#define TZBSP_LPASS_LCC_RG45            45
#define TZBSP_LPASS_LCC_RG46            46
#define TZBSP_LPASS_LCC_RG47            47
#define TZBSP_LPASS_LCC_RG48            48
#define TZBSP_LPASS_LCC_RG49            49
#define TZBSP_LPASS_LCC_RG50            50
#define TZBSP_LPASS_LCC_RG51            51
#define TZBSP_LPASS_LCC_RG52            52
#define TZBSP_LPASS_LCC_RG53            53

#define TZBSP_LPASS_LPAIF_RG0           0
#define TZBSP_LPASS_LPAIF_RG1           1
#define TZBSP_LPASS_LPAIF_RG2           2
#define TZBSP_LPASS_LPAIF_RG3           3
#define TZBSP_LPASS_LPAIF_RG4           4
#define TZBSP_LPASS_LPAIF_RG5           5
#define TZBSP_LPASS_LPAIF_RG6           6
#define TZBSP_LPASS_LPAIF_RG7           7
#define TZBSP_LPASS_LPAIF_RG8           8
#define TZBSP_LPASS_LPAIF_RG9           9
#define TZBSP_LPASS_LPAIF_RG10          10
#define TZBSP_LPASS_LPAIF_RG11          11
#define TZBSP_LPASS_LPAIF_RG12          12
#define TZBSP_LPASS_LPAIF_RG13          13
#define TZBSP_LPASS_LPAIF_RG14          14
#define TZBSP_LPASS_LPAIF_RG15          15
#define TZBSP_LPASS_LPAIF_RG16          16
#define TZBSP_LPASS_LPAIF_RG17          17
#define TZBSP_LPASS_LPAIF_RG18          18
#define TZBSP_LPASS_LPAIF_RG19          19
#define TZBSP_LPASS_LPAIF_RG20          20
#define TZBSP_LPASS_LPAIF_RG21          21
#define TZBSP_LPASS_LPAIF_RG22          22
#define TZBSP_LPASS_LPAIF_RG23          23
#define TZBSP_LPASS_LPAIF_RG24          24
#define TZBSP_LPASS_LPAIF_RG25          25
#define TZBSP_LPASS_LPAIF_RG26          26
#define TZBSP_LPASS_LPAIF_RG27          27
#define TZBSP_LPASS_LPAIF_RG28          28
#define TZBSP_LPASS_LPAIF_RG29          29
#define TZBSP_LPASS_LPAIF_RG30          30
#define TZBSP_LPASS_LPAIF_RG31          31

#define TZBSP_LPASS_RSMPLR_RG0          0
#define TZBSP_LPASS_RSMPLR_RG1          1
#define TZBSP_LPASS_RSMPLR_RG2          2
#define TZBSP_LPASS_RSMPLR_RG3          3
#define TZBSP_LPASS_RSMPLR_RG4          4
#define TZBSP_LPASS_RSMPLR_RG5          5
#define TZBSP_LPASS_RSMPLR_RG6          6

#define TZBSP_USB2_HSIC_RG0             0


#define TZBSP_MMSS_OXILI_RBBM           0
#define TZBSP_MMSS_OXILI_CP_BLOCK_0     1
#define TZBSP_MMSS_OXILI_RESERVED_2     2
#define TZBSP_MMSS_OXILI_BLOCK_0        3
#define TZBSP_MMSS_OXILI_RESERVED_4     4
#define TZBSP_MMSS_OXILI_BLOCK_1_2      5
#define TZBSP_MMSS_OXILI_RESERVED_6     6
#define TZBSP_MMSS_OXILI_HLSQ_DEBUG     7
#define TZBSP_MMSS_OXILI_RESERVED_8     8

#define TZBSP_MMSS_RBCPR_RG0            0

#define TZBSP_MMSS_OXILI_VBIF_BIF_NONSECURE  0
#define TZBSP_MMSS_OXILI_VBIF_BIF_SECURE     1
#define TZBSP_MMSS_OXILI_VBIF_MMU_GLOBAL_0   2
#define TZBSP_MMSS_OXILI_VBIF_MMU_GLOBAL_1   3
#define TZBSP_MMSS_OXILI_VBIF_MMU_IMPL_DEF_0 4
#define TZBSP_MMSS_OXILI_VBIF_MMU_PERF_MON   5
#define TZBSP_MMSS_OXILI_VBIF_MMU_SSD        6
#define TZBSP_MMSS_OXILI_VBIF_MMU_CBANK_0    7
#define TZBSP_MMSS_OXILI_VBIF_MMU_CBANK_1    8
#define TZBSP_MMSS_OXILI_VBIF_MMU_CBANK_2    9
#define TZBSP_MMSS_OXILI_VBIF_MMU_CBANK_3    10
#define TZBSP_MMSS_OXILI_VBIF_MMU_CBANK_4    11

#define TZBSP_KPSS_RG0       0
#define TZBSP_KPSS_RG0_START 0xF9011000
#define TZBSP_KPSS_RG0_END   0xF9013000
#define TZBSP_KPSS_RG1       1
#define TZBSP_KPSS_RG1_START 0xF9088000
#define TZBSP_KPSS_RG1_END   0xF9089000

#define TZBSP_MMSS_CAMERA_RG_CAMSS_TOP 0
#define TZBSP_MMSS_CAMERA_RG_MICRO     1
#define TZBSP_MMSS_CAMERA_RG_CSID      2
#define TZBSP_MMSS_CAMERA_RG_CCI       3
#define TZBSP_MMSS_CAMERA_RG_VFE0      4
#define TZBSP_MMSS_CAMERA_RG_VFE1      5
#define TZBSP_MMSS_CAMERA_RG_CPP       6
#define TZBSP_MMSS_CAMERA_RG_JPEG0     7
#define TZBSP_MMSS_CAMERA_RG_JPEG1     8
#define TZBSP_MMSS_CAMERA_RG_JPEG2     9

#define TZBSP_SMMU_DIME_RG_SECURE      0
#define TZBSP_SMMU_DIME_START          0x00000000
#define TZBSP_SMMU_DIME_END            0x80000000

#define TZBSP_SMMU_GPS_RG_SECURE       0
#define TZBSP_SMMU_GPS_START           0x00000000 
#define TZBSP_SMMU_GPS_END             0x80000000

#define TZBSP_SMMU_QDSP_RG0            0
#define TZBSP_SMMU_QDSP_RG0_START      0x04800000
#define TZBSP_SMMU_QDSP_RG0_END        0x80000000 
#define TZBSP_SMMU_QDSP_RG1            1
#define TZBSP_SMMU_QDSP_RG1_START      0x01980000
#define TZBSP_SMMU_QDSP_RG1_END        0x01EF3000
#define TZBSP_SMMU_QDSP_RG2            2
#define TZBSP_SMMU_QDSP_RG2_START      0x01EF4000
#define TZBSP_SMMU_QDSP_RG2_END        0x02000000
#define TZBSP_SMMU_QDSP_RG3            3
#define TZBSP_SMMU_QDSP_RG3_START      0x01820000
#define TZBSP_SMMU_QDSP_RG3_END        0x01900000
#define TZBSP_SMMU_QDSP_RG4            4
#define TZBSP_SMMU_QDSP_RG4_START      0x0040E000
#define TZBSP_SMMU_QDSP_RG4_END        0x004A0000
#define TZBSP_SMMU_QDSP_RG5            5
#define TZBSP_SMMU_QDSP_RG5_START      0x00200000
#define TZBSP_SMMU_QDSP_RG5_END        0x0040C000
#define TZBSP_SMMU_QDSP_RG6            6
#define TZBSP_SMMU_QDSP_RG6_START      0x00062000
#define TZBSP_SMMU_QDSP_RG6_END        0x00068000
#define TZBSP_SMMU_QDSP_RG7            7
#define TZBSP_SMMU_QDSP_RG7_START      0x00060000
#define TZBSP_SMMU_QDSP_RG7_END        0x00061000
#define TZBSP_SMMU_QDSP_RG8            8 
#define TZBSP_SMMU_QDSP_RG8_START      0x00023000
#define TZBSP_SMMU_QDSP_RG8_END        0x00058000


#define TZBSP_SMMU_RG_GLOB0            0
#define TZBSP_SMMU_GLOB0_START         0x01E00000
#define TZBSP_SMMU_GLOB0_END           0x01E20000
#define TZBSP_SMMU_RG_STAGE2           1
#define TZBSP_SMMU_STAGE2_START        0x01E20000
#define TZBSP_SMMU_STAGE2_END          0x01E22000
#define TZBSP_SMMU_RG_STAGE1           2
#define TZBSP_SMMU_STAGE1_START        0x01E30000
#define TZBSP_SMMU_STAGE1_END          0x01E38000
#define TZBSP_SMMU_RG_SECURE           3
#define TZBSP_SMMU_SECURE_START        0x01EF0000
#define TZBSP_SMMU_SECURE_END          0x01EF1000
#define TZBSP_SMMU_RG_HYP              4
#define TZBSP_SMMU_HYP_START           0x01EF1000
#define TZBSP_SMMU_HYP_END             0x01EF2000
#define TZBSP_SMMU_RG_MSS              5
#define TZBSP_SMMU_MSS_START           0x01EF3000
#define TZBSP_SMMU_MSS_END             0x01EF4000
#define TZBSP_SMMU_RG_MMSS             6
#define TZBSP_SMMU_MMSS_START          0x01EF4000
#define TZBSP_SMMU_MMSS_END            0x01EFE000
#define TZBSP_SMMU_RG_GLOB1            7
#define TZBSP_SMMU_GLOB1_START         0x01F00000
#define TZBSP_SMMU_GLOB1_END           0x01F08000
#define TZBSP_SMMU_RG_GFX              8
#define TZBSP_SMMU_GFX_START           0x01F08000
//TODO FIXME_8916 end changing from 0x01F09000 to 0x01F10000
#define TZBSP_SMMU_GFX_END             0x01F09000

#define TZBSP_RPM_APU_RG0              0
#define TZBSP_RPM_APU_RG1              1

/* note: as RPM CFG xPU config address width is 9, leading 2
 * in address removed. i.e.. instead of 000280000 config as 0x00080000 */
#define TZBSP_RPM_CFG_RG_CSR           0
#define TZBSP_RPM_CFG_RG_CSR_BASE      0x00080000
#define TZBSP_RPM_CFG_RG_CSR_END       0x00089000
#define TZBSP_RPM_CFG_RG_DATA_RAM      1
#define TZBSP_RPM_CFG_RG_DATA_RAM_BASE 0x00090000
#define TZBSP_RPM_CFG_RG_DATA_RAM_END  0x000A0000

#define TZBSP_SPMI_RG_CFG                   0
#define TZBSP_SPMI_RG_CFG_BASE              (0x02000000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_CFG_END               (0x02010000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_CH                1 
#define TZBSP_SPMI_RG_RPM_CH_START          (0x024F8000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_CH_END            (0x0264E000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_CH                2
#define TZBSP_SPMI_RG_MSS_CH_BASE           (0x02650000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_CH_END            (0x026D6000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RO_CH                 3
#define TZBSP_SPMI_RG_RO_CH_BASE            (0x02778000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RO_CH_END             (0x027D6000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT0              4
#define TZBSP_SPMI_RG_RPM_INT0_BASE         (0x03800000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT0_END          (0x03801000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_INT0              5
#define TZBSP_SPMI_RG_MSS_INT0_BASE         (0x0380A000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_INT0_END          (0x0380B000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT1              6
#define TZBSP_SPMI_RG_RPM_INT1_BASE         (0x0381F000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT1_END          (0x03825000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT2              7
#define TZBSP_SPMI_RG_RPM_INT2_BASE         (0x03828000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT2_END          (0x0384C000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_INT1              8
#define TZBSP_SPMI_RG_MSS_INT1_BASE         (0x0384C000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_INT1_END          (0x0384D000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT3              9
#define TZBSP_SPMI_RG_RPM_INT3_BASE         (0x0384D000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT3_END          (0x0384E000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_INT2              10
#define TZBSP_SPMI_RG_MSS_INT2_BASE         (0x0384E000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_INT2_END          (0x03851000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT5              11
#define TZBSP_SPMI_RG_RPM_INT5_BASE         (0x03854000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT5_END          (0x0385B000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_INT3              12
#define TZBSP_SPMI_RG_MSS_INT3_BASE         (0x0385F000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_INT3_END          (0x03860000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT6              13
#define TZBSP_SPMI_RG_RPM_INT6_BASE         (0x0386F000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_INT6_END          (0x03874000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_ACC               14
#define TZBSP_SPMI_RG_MSS_ACC_BASE          (0x03902000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_MSS_ACC_END           (0x03904000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_ACC               15
#define TZBSP_SPMI_RG_RPM_ACC_BASE          (0x03904000 & 0x1FFFFFF)
#define TZBSP_SPMI_RG_RPM_ACC_END           (0x03905000 & 0x1FFFFFF)
#endif /* TZBSP_XPU_DEF_H */
