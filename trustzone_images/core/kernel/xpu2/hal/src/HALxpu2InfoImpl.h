#ifndef HALXPU2INFOIMPL_H
#define HALXPU2INFOIMPL_H
/*
===========================================================================

FILE:         HALxpu2InfoImpl.h

DESCRIPTION: Internal XPU2 types used by device config properties

===========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/xpu2/hal/src/HALxpu2InfoImpl.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   sc      Updated xPU2 defines
09/10/13   sc      Updated xPU2 defines
08/09/13   sc      Updated xPU2 defines
05/02/13   sc      Updated xPU2 defines
12/04/12   sc      Created

===========================================================================
Copyright (c) 2012, 2013
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================
*/
/** @file HALxpu2InfoImpl.h
 *  @brief Internal header file for HAL XPU2.
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <HALxpu2.h>

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
// The following definitions have to match the sequence of
// enum type HAL_xpu2_XPU2Type.
#define XPU2_KPSS_MPU              0
#define XPU2_APCS_MPU              0
#define XPU2_KPSS_L2               1
#define XPU2_BIMC_APU              2
#define XPU2_BIMC_MPU0             3
#define XPU2_BIMC_MPU1             4
#define XPU2_BOOT_ROM              5
#define XPU2_CRYPTO0_BAM           6
#define XPU2_CRYPTO1_BAM           7
#define XPU2_DEHR                  8
#define XPU2_CLK_CTL               9
#define XPU2_LPASS_COMMON         10
#define XPU2_LPASS_CORE_CSR       11
#define XPU2_LPASS_LCC            12
#define XPU2_LPASS_LPAIF          13
#define XPU2_LPASS_LPM            14
#define XPU2_LPASS_QDSP6SS        15
#define XPU2_LPASS_RSMPLR         16
#define XPU2_LPASS_SLIMBUS        17
#define XPU2_LPASS_TOP            18
#define XPU2_CAMERA_SS            19
#define XPU2_MMSS_A_VBIF_JPEG     20
#define XPU2_MMSS_A_VBIF_VFE      21
#define XPU2_MMSS_DM_APU          22
#define XPU2_MMSS_DM_MPU          23
#define XPU2_MMSS_OCMEM_MPU0      24
#define XPU2_MMSS_OCMEM_MPU1      25
#define XPU2_MMSS_OCMEM_APU       26
#define XPU2_MMSS_OXILI_CX        27
#define XPU2_MMSS_OXILI_VBIF      28
#define XPU2_MMSS_SNOC            29
#define XPU2_MMSS_MDSS_VBIF       30
#define XPU2_MMSS_VENUS           31
#define XPU2_MMSS_VENUS_VBIF      32
#define XPU2_MDSS_APU             33
#define XPU2_MMSS_CC              34
#define XPU2_MMSS_MISC            35
#define XPU2_MMSS_MNOC            36
#define XPU2_MMSS_ONOC            37
#define XPU2_MMSS_RBCPR           38
#define XPU2_MPM2                 39
#define XPU2_MSS                  40
#define XPU2_IMEM_APU             41
#define XPU2_IMEM_MPU             42
#define XPU2_PERIPH_SS_APU        43
#define XPU2_BAM_DMA              44
#define XPU2_BAM_BLSP1_DMA        45
#define XPU2_BAM_BLSP2_DMA        46
#define XPU2_PNOC_MPU             47
#define XPU2_BAM_SDCC1            48
#define XPU2_BAM_SDCC2            49
#define XPU2_BAM_SDCC3            50
#define XPU2_BAM_SDCC4            51
#define XPU2_TSIF                 52
#define XPU2_USB1_HS              53
#define XPU2_USB2_HSIC            54
#define XPU2_PMIC_ARB             55
#define XPU2_PMIC_ARB_SPMI        56
#define XPU2_RPM_APU              57
#define XPU2_RPM_M3               58
#define XPU2_RPM_MSG_RAM          59
#define XPU2_SEC_CTRL_APU         60
#define XPU2_SPDM_APU             61
#define XPU2_TCSR_MUTEX           62
#define XPU2_TCSR_REGS            63
#define XPU2_TLMM                 64
#define XPU2_USB30                65
#define XPU2_PRONTO               66
#define XPU2_EBI1_CFG             67
#define XPU2_RBCR_QDSS            68
#define XPU2_SNOC_CFG             69
#define XPU2_QPIC_BAM             70
#define XPU2_QPIC_NAND            71
#define XPU2_IPA                  72
#define XPU2_CRYPTO2_BAM          73
#define XPU2_VBIF_APU             74
#define XPU2_BCSS_APU             75
#define XPU2_EMAC                 76
#define XPU2_KLM                  77
#define XPU2_MMSS_VPU_MAPLE       78
#define XPU2_MMSS_VPU_VBIF_APU    79
#define XPU2_MMSS_VCAP_APU        80
#define XPU2_MMSS_VCAP_VBIF_APU   81
#define XPU2_PCIE20_MPU           82
#define XPU2_PCIE20_APU           83
#define XPU2_USB_HS_SEC           84
#define XPU2_SATA_APU             85
#define XPU2_MMSS_APU             86
#define XPU2_USB3_HSIC            87
#define XPU2_SPSS_APU             88
#define XPU2_LPASS_CORE           89
#define XPU2_LPASS_QDSP6          90
#define XPU2_MMSS_MM_AVSYNC       91
#define XPU2_PCIE0_APU            92
#define XPU2_PCIE1_APU            93
#define XPU2_SMMUV2               94
#define XPU2_UFS                  95
#define XPU2_PCIE0_MPU            96
#define XPU2_PCIE1_MPU            97
#define XPU2_LPASS_SB1            98
#define XPU2_MMSS_A_RICA          99
#define XPU2_MMSS_A_VBIF_CPP_VBIF 100
#define XPU2_FD_XPU2              101
#define XPU2_UFS_ICE              102
#define XPU2_MMSS_A_VBIF_FD_VBIF  103
#define XPU2_MMSS_A_VBIF_RICA_VBIF 104
#define XPU2_PCNOC_CFG            105
#define XPU2_PRNG_CFG             106
#define XPU2_RPM_CFG              107
#define XPU2_SMMU_500_DIME_MPU    108
#define XPU2_SMMU_500_GPS_MPU     109
#define XPU2_SMMU_500_QDSP_MPU    110
#define XPU2_SMMU_500_TCU_MPU     111
#define XPU2_QPIC_QPIC            112
#define XPU2_QPIC_QPIC_MPU        113
#define XPU2_SMMU_500_MCDMA_MPU   114
#define XPU2_SEC_CTRL_XPU         115
#define XPU2_EMAC_0_EMAC		  116
#define XPU2_Q6_BIMC_MPU          117

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/** xPU2 internal device parameters struct */
typedef struct
{
  HAL_xpu2_XPUDeviceType eXpu2Type;           /**< XPU2 type */
  uint32                 uResourceGroupCount; /**< Total number of Resource Groups */
  uint32                 uNumVMID;            /**< number of VMID */
  uint32                 uClientAddrWidth;    /**< Width of client port address (for MPUs) */
  uint32                 uRevision;           /**< XPU2 revision */
  uint8                  uMSB;                /**< MSB for resource group selection */
  uint8                  uLSB;                /**< LSB for resource group selection */
  boolean                bAMTSupport : 1;     /**< Support access mask table? (for MPUs) */
  boolean                bMSASupport : 1;     /**< Support modem self authentication? */
  boolean                bMultiVMID : 1;      /**< Multiple VMID support? */
  boolean                bFullPerm : 1;       /**< Full permissions? */
} HAL_xpu2_IntXPU2DeviceParamsType;

/** device params and mem storage for registration */
typedef struct
{
  uint32                          uBaseAddr;
    /**< base address of xPU2 instance */
  const char                     *szXpu2Name;
    /**< pointer to XPU2 string name */
  HAL_xpu2_IntXPU2DeviceParamsType  devParams; /**< xPU2 device parameters */
#if defined(FEATURE_XPU2_SAVE_RESTORE)
  bool32                          bSaveConfigValid;
  HAL_xpu2_XPU2StateType *        pSaveConfig;
  uint32                          uSaveConfigLen;
#endif
  /*As of now, all XPU2s have burst access. Remove the field to save space.*/
#if defined(XPU2_BURST_ACCESS)
  bool32                          bBurstAccess;
#endif
} HAL_xpu2_InfoType;

#endif /* #ifndef HALXPU2INFOIMPL_H */
