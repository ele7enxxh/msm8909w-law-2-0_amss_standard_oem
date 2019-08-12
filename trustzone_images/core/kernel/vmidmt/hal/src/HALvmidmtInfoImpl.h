#ifndef HALVMIDMTINFOIMPL_H
#define HALVMIDMTINFOIMPL_H
/*
===========================================================================

FILE:         HALvmidmtInfoImpl.h

DESCRIPTION: Internal VMIDMT types used by device config properties. 

===========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/vmidmt/hal/src/HALvmidmtInfoImpl.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
12/04/12     sc     Created

===========================================================================
Copyright (c) 2012, 2013
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================
*/
/** @file HALvmidmtInfoImpl.h
 *  @brief Internal header file for HAL VMIDMT.
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <HALvmidmt.h>

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
// The following definitions have to match the sequence of
// enum type HAL_vmidmt_InstanceType.
#define VMIDMT_CRYPTO0_AXI          0
#define VMIDMT_CRYPTO1_AXI          1
#define VMIDMT_CRYPTO0_BAM          2
#define VMIDMT_CRYPTO1_BAM          3
#define VMIDMT_DEHR                 4
#define VMIDMT_LPASS_DM             5
#define VMIDMT_LPASS_LPAIF          6
#define VMIDMT_LPASS_MIDI           7
#define VMIDMT_LPASS_Q6AHB          8
#define VMIDMT_LPASS_Q6AXI          9
#define VMIDMT_LPASS_RESAMPLER     10
#define VMIDMT_LPASS_SLIMBUS       11
#define VMIDMT_CAMERA_SS           12
#define VMIDMT_CAMSS_VBIF_JPEG     13
#define VMIDMT_CAMSS_VBIF_VFE      14
#define VMIDMT_MMSS_DDR            15
#define VMIDMT_OCMEM               16
#define VMIDMT_OXILI               17
#define VMIDMT_VENUS_CPUSS         18
#define VMIDMT_VENUS_VBIF          19
#define VMIDMT_MSS_A2BAM           20
#define VMIDMT_MSS_NAV_CE          21
#define VMIDMT_MSS_Q6              22
#define VMIDMT_BAM_DMA             23
#define VMIDMT_BAM_BLSP1_DMA       24
#define VMIDMT_BAM_BLSP2_DMA       25
#define VMIDMT_BAM_SDCC1           26
#define VMIDMT_BAM_SDCC2           27
#define VMIDMT_BAM_SDCC3           28
#define VMIDMT_BAM_SDCC4           29
#define VMIDMT_TSIF                30
#define VMIDMT_USB1_HS             31
#define VMIDMT_USB2_HSIC           32
#define VMIDMT_QDSS_VMIDDAP        33
#define VMIDMT_QDSS_VMIDETR        34
#define VMIDMT_RPM_MSGRAM          35
#define VMIDMT_SPDM_WRAPPER        36
#define VMIDMT_USB30               37
#define VMIDMT_PRONTO              38
#define VMIDMT_QPIC_BAM            39
#define VMIDMT_IPA                 40
#define VMIDMT_APCS                41
#define VMIDMT_CRYPTO2_AXI         42
#define VMIDMT_CRYPTO2_BAM         43
#define VMIDMT_EMAC                44
#define VMIDMT_LPASS_HDMI          45
#define VMIDMT_LPASS_SPDIF         46
#define VMIDMT_VENUS_VBIF2         47
#define VMIDMT_MMSS_VPU_MAPLE      48
#define VMIDMT_USB_HS_SEC          49
#define VMIDMT_SATA                50
#define VMIDMT_PCIE20              51
#define VMIDMT_PCIE0               52
#define VMIDMT_PCIE1               53
#define VMIDMT_USB3_HSIC           54
#define VMIDMT_UFS                 55
#define VMIDMT_LPASS_SB1           56
#define VMIDMT_QPIC                57

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/** internal device parameters struct */
typedef struct
{
  uint16 uEntryCount; /**< Total number of table entries */
  uint16 uNumVmid; /**< number of VMID supported by the core */
  uint8 uNumSSDIndexBits; /**< number of SSD_Index bits for the SSDT indexing */
  uint8 uNumStreamIDBits; /**< number of SID bits for the VMID context indexing */
  uint8 uInputAddrSize; /**< input address size for client port. GFEAR uses this info */
  uint8 bStreamMatchSupport; /**< support stream match? If yes, SMRn is used. */
} HAL_vmidmt_IntVMIDMTDevParamsType;

/** device params and mem storage for registration */
typedef struct 
{
  uint32                             uBaseAddr;
  HAL_vmidmt_IntVMIDMTDevParamsType  devParams;
#if defined(FEATURE_VMIDMT_SAVE_RESTORE)
  bool32                             bSaveConfigValid;
  HAL_vmidmt_VMIDMTStateType        *pSaveConfig;
  uint32                             uSaveConfigLen;
#endif
} HAL_vmidmt_InfoType;

#endif /* #ifndef HALVMIDMTINFOIMPL_H */
