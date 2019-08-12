#ifndef __PCIE_TARGET_HWIO_H__
#define __PCIE_TARGET_HWIO_H__
/*
===========================================================================
*/
/**
  @file pcie_target_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules outside PCIe:
    SECURITY_CONTROL_CORE
	
	Also contains other hardware dependent #defines

  'Include' filters applied: BOOT_CONFIG[SECURITY_CONTROL_CORE] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/pcie/pcie_target_hwio.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "msmhwiobase.h"
#include "HALhwio.h"

#define PCIE_PCIE20_WRAPPER_AHB_BASE                                          PCIE_0_PCIE20_WRAPPER_AHB_BASE
#define PCIE_PCIE20_WRAPPER_AHB_BASE_SIZE                                     PCIE_0_PCIE20_WRAPPER_AHB_BASE_SIZE
#define PCIE_PCIE20_WRAPPER_AHB_BASE_PHYS                                     PCIE_0_PCIE20_WRAPPER_AHB_BASE_PHYS

#define PCIE_PCIE20_WRAPPER_AXI_BASE                                          PCIE_0_PCIE20_WRAPPER_AXI_BASE
#define PCIE_PCIE20_WRAPPER_AXI_BASE_SIZE                                     PCIE_0_PCIE20_WRAPPER_AXI_BASE_SIZE
#define PCIE_PCIE20_WRAPPER_AXI_BASE_PHYS                                     PCIE_0_PCIE20_WRAPPER_AXI_BASE_PHYS

#define PCIE_CM_SIZE  PCIE_PHY_SIZE

#define PCIE_0_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE              (PCIE_0_PCIE20_WRAPPER_AHB_BASE      + 0x00006000)
#define PCIE_0_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS         0x00006000


#define PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE                PCIE_0_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE
#define PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_PHYS           (PCIE_0_PCIE20_WRAPPER_AHB_BASE_PHYS + 0x00006000)
#define PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS           PCIE_0_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS


/* Define offset */
#define PCIE20_ELBI_REG_BASE_OFFS                                        0x00000f20
#define PCIE20_PARF_REG_BASE_OFFS                                        0x00000000
#define PCIE20_MHI_REG_BASE_OFFS                                         0x00007000
#define DWC_PCIE_DM_REG_BASE_OFFS                                        0x3FFFE000


/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                        (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                   0x00000000

#define HWIO_BOOT_CONFIG_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000602c)
#define HWIO_BOOT_CONFIG_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000602c)
#define HWIO_BOOT_CONFIG_RMSK                                                                      0x3ff
#define HWIO_BOOT_CONFIG_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, HWIO_BOOT_CONFIG_RMSK)
#define HWIO_BOOT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, m)
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_BMSK                                                        0x200
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_SHFT                                                          0x9
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_DISABLE_FVAL                                                  0x0
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_ENABLE_FVAL                                                   0x1
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_BMSK                                                    0x100
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_SHFT                                                      0x8
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_DOES_NOT_FORCE_CODE_AUTHENTICATION_FOR_SECURE_BOOT_FVAL   0x0
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_FORCE_CODE_AUTHENTICATION_FOR_SECURE_BOOT_FVAL            0x1
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                   0xc0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                    0x6
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_XO_FVAL                                                 0x0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_384_MHZ_FVAL                                       0x1
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_614_4_MHZ_FVAL                                     0x2
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_998_4_MHZ_FVAL                                     0x3
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                    0x20
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                     0x5
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                             0x1e
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                              0x1
#define HWIO_BOOT_CONFIG_FAST_BOOT_DEFAULT_FVAL                                                      0x0
#define HWIO_BOOT_CONFIG_FAST_BOOT_SD_SDC2_EMMC_SDC1_FVAL                                            0x1
#define HWIO_BOOT_CONFIG_FAST_BOOT_EMMC_SDC1_FVAL                                                    0x2
#define HWIO_BOOT_CONFIG_FAST_BOOT_USB_FVAL                                                          0x3
#define HWIO_BOOT_CONFIG_FAST_BOOT_HSIC_FVAL                                                         0x4
#define HWIO_BOOT_CONFIG_FAST_BOOT_NAND_FVAL                                                         0x5
#define HWIO_BOOT_CONFIG_FAST_BOOT_SPI_ON_BLSP1_QUP1_FVAL                                            0x6
#define HWIO_BOOT_CONFIG_FAST_BOOT_SPI_ON_BLSP1_QUP2_FVAL                                            0x7
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_GPIO_CONTROLLED_FVAL                                                0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_ENABLE_WDOG_FVAL                                                    0x1

/*----------------------------------------------------------------------------
 * MODULE: BIMC_GLOBAL2
 *--------------------------------------------------------------------------*/

#define BIMC_GLOBAL2_REG_BASE                                              (BIMC_BASE      + 0x00002000)

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000220 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RMSK                     0xfff0040f
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_MAXn                              2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_10_SHFT                    0xa
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RFU_BMSK                        0xc
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RFU_SHFT                        0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_TYPE_BMSK                       0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_TYPE_SHFT                       0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ENABLE_BMSK                     0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ENABLE_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000228 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_RMSK                     0xfff00400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MAXn                              2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_10_SHFT                    0xa

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                                   (TLMM_BASE      + 0x00000000)


#define HWIO_TLMM_GPIO_OE_0_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, HWIO_TLMM_GPIO_OE_0_RMSK)
#define HWIO_TLMM_GPIO_OE_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, m)
#define HWIO_TLMM_GPIO_OE_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR,m,v,HWIO_TLMM_GPIO_OE_0_IN)
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                                           0x0


#define HWIO_TLMM_GPIO_OUT_0_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, HWIO_TLMM_GPIO_OUT_0_RMSK)
#define HWIO_TLMM_GPIO_OUT_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR,m,v,HWIO_TLMM_GPIO_OUT_0_IN)
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                                         0x0

#ifdef PCIE_BOOT
/*----------------------------------------------------------------------------
 * BASE: IPA_WRAPPER
 *--------------------------------------------------------------------------*/

#define IPA_WRAPPER_BASE                                                       0x07920000
#define IPA_WRAPPER_BASE_PHYS                                                  0x07920000

#define IPA_REG_BASE                                                           (IPA_WRAPPER_BASE      + 0x00020000)
#define IPA_REG_BASE_PHYS                                                      (IPA_WRAPPER_BASE_PHYS + 0x00020000)
#define IPA_REG_BASE_OFFS                                                      0x00020000

#define HWIO_IPA_COMP_SW_RESET_ADDR                                            (IPA_REG_BASE      + 0x0000003c)
#define HWIO_IPA_COMP_SW_RESET_PHYS                                            (IPA_REG_BASE_PHYS + 0x0000003c)
#define HWIO_IPA_COMP_SW_RESET_OFFS                                            (IPA_REG_BASE_OFFS + 0x0000003c)
#define HWIO_IPA_COMP_SW_RESET_RMSK                                                   0x1
#define HWIO_IPA_COMP_SW_RESET_POR                                             0x00000000
#define HWIO_IPA_COMP_SW_RESET_ATTR                                                   0x2
#define HWIO_IPA_COMP_SW_RESET_OUT(v)      \
        out_dword(HWIO_IPA_COMP_SW_RESET_ADDR,v)
#define HWIO_IPA_COMP_SW_RESET_SW_RESET_BMSK                                          0x1
#define HWIO_IPA_COMP_SW_RESET_SW_RESET_SHFT                                          0x0


#define HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n)                                      (IPA_REG_BASE      + 0x000002c0 + 0x4 * (n))
#define HWIO_IPA_ENDP_INIT_MODE_n_PHYS(n)                                      (IPA_REG_BASE_PHYS + 0x000002c0 + 0x4 * (n))
#define HWIO_IPA_ENDP_INIT_MODE_n_OFFS(n)                                      (IPA_REG_BASE_OFFS + 0x000002c0 + 0x4 * (n))
#define HWIO_IPA_ENDP_INIT_MODE_n_RMSK                                          0xffff1f3
#define HWIO_IPA_ENDP_INIT_MODE_n_MAXn                                                 19
#define HWIO_IPA_ENDP_INIT_MODE_n_POR                                          0x00000000
#define HWIO_IPA_ENDP_INIT_MODE_n_ATTR                                                0x3
#define HWIO_IPA_ENDP_INIT_MODE_n_INI(n)        \
        in_dword_masked(HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n), HWIO_IPA_ENDP_INIT_MODE_n_RMSK)
#define HWIO_IPA_ENDP_INIT_MODE_n_INMI(n,mask)    \
        in_dword_masked(HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n), mask)
#define HWIO_IPA_ENDP_INIT_MODE_n_OUTI(n,val)    \
        out_dword(HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n),val)
#define HWIO_IPA_ENDP_INIT_MODE_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n),mask,val,HWIO_IPA_ENDP_INIT_MODE_n_INI(n))
#define HWIO_IPA_ENDP_INIT_MODE_n_BYTE_THRESHOLD_BMSK                           0xffff000
#define HWIO_IPA_ENDP_INIT_MODE_n_BYTE_THRESHOLD_SHFT                                 0xc
#define HWIO_IPA_ENDP_INIT_MODE_n_DEST_PIPE_INDEX_BMSK                              0x1f0
#define HWIO_IPA_ENDP_INIT_MODE_n_DEST_PIPE_INDEX_SHFT                                0x4
#define HWIO_IPA_ENDP_INIT_MODE_n_MODE_BMSK                                           0x3
#define HWIO_IPA_ENDP_INIT_MODE_n_MODE_SHFT                                           0x0


#define HWIO_IPA_COMP_CFG_ADDR                                                 (IPA_REG_BASE      + 0x00000038)
#define HWIO_IPA_COMP_CFG_PHYS                                                 (IPA_REG_BASE_PHYS + 0x00000038)
#define HWIO_IPA_COMP_CFG_OFFS                                                 (IPA_REG_BASE_OFFS + 0x00000038)
#define HWIO_IPA_COMP_CFG_RMSK                                                        0x3
#define HWIO_IPA_COMP_CFG_POR                                                  0x00000000
#define HWIO_IPA_COMP_CFG_ATTR                                                        0x3
#define HWIO_IPA_COMP_CFG_IN          \
        in_dword_masked(HWIO_IPA_COMP_CFG_ADDR, HWIO_IPA_COMP_CFG_RMSK)
#define HWIO_IPA_COMP_CFG_INM(m)      \
        in_dword_masked(HWIO_IPA_COMP_CFG_ADDR, m)
#define HWIO_IPA_COMP_CFG_OUT(v)      \
        out_dword(HWIO_IPA_COMP_CFG_ADDR,v)
#define HWIO_IPA_COMP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IPA_COMP_CFG_ADDR,m,v,HWIO_IPA_COMP_CFG_IN)
#define HWIO_IPA_COMP_CFG_BAM_SNOC_BYPASS_DIS_BMSK                                    0x2
#define HWIO_IPA_COMP_CFG_BAM_SNOC_BYPASS_DIS_SHFT                                    0x1
#define HWIO_IPA_COMP_CFG_ENABLE_BMSK                                                 0x1
#define HWIO_IPA_COMP_CFG_ENABLE_SHFT                                                 0x0
#endif /* PCIE_BOOT */

#endif /* __PCIE_TARGET_HWIO_H__ */
