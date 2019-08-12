#===========================================================================
#
#  @file hwio_mdm9x07.py
#  @brief HWIO config file for the HWIO generation scripts for MSM8916
#
#  This file can be invoked by calling:
#
#    HWIOGen.py --cfg=hwio_mdm9x07.py --flat=..\..\..\api\systemdrivers\hwio\mdm9x07\ARM_ADDRESS_FILE.FLAT
#
#  ===========================================================================
#
#  Copyright (c) 2015 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/hwio/build/hwio_mdm9x07.py#1 $
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

CHIPSET = 'mdm9x07'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

bases = [
  '.*'  # We map everything for TZ for the moment.
]

base_resize = {
  'PRNG_PRNG':                              0x1000,
  'RPM':                                   0x89000,
  'BOOT_ROM':                             0x100000,
  'CRYPTO0_CRYPTO_TOP':                    0x40000,
  'BIMC':                                  0x60000,
  'BIMC_MPU_MPU0032A_20_M31L10_AXI':        0x1000,
  'A7SS':                                  0x2A000,
  'CLK_CTL':                               0x82000,
  'MSS_TOP':                              0x38B000,
  'TLMM':                                 0x310000,
  'OCIMEM_WRAPPER_CSR':                     0x4000,
  'SPDM_WRAPPER_TOP':                       0x5000,
  'PMIC_ARB':                            0x1906000,
  'EBI1_PHY_CFG':                          0x20000,
  'EMAC_0_EMAC_SGMII_TOP_WRAPPER_LYKAN':   0x1E000,
  'DEHR_BIMC_WRAPPER':                      0x4000,
  'QDSS_QDSS_APB':                         0x27000,
  'QDSS_QDSS_AHB':                         0x29000,
  'QDSS_WRAPPER_TOP':                          0x0,
  'CORE_TOP_CSR':                          0x58000,
  'PC_NOC':                                0x16000,
  'BLSP1_BLSP':                            0x3b000,
  'PDM_PERPH_WEB':                          0x4000,
  'SDC1_SDCC5_TOP':                        0x25000,
  'SDC2_SDCC5_TOP':                        0x25000,
  'SECURITY_CONTROL':                       0xF000,
  'QDSS_WRAPPER_TOP':                       0x6000,
  'SMMU_500_MPU_WRAPPER':                   0x4000,
  'SMMU_500_REG_WRAPPER':                 0x10c000,
  'QPIC_QPIC':                             0x38000,
  'XPU_CFG_PRNG_CFG_APU1132_1':             0x1000,
  'XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB':   0x1000,
  #'XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB':  0x1000,
  'XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB': 0x1000,
  'USB2_HSIC_USB_OTG_HS_HSIC_TOP_NO_SEC':   0x1000,
  'USB_OTG_HS_EP16_PIPES2':                0x1A000,
  'USB2_PHY_CM_DWC_USB2_SW':                0x1000,
  'PCNOC_0_BUS_TIMEOUT':                    0x1000,
  'PCNOC_1_BUS_TIMEOUT':                    0x1000,
  'PCNOC_3_BUS_TIMEOUT':                    0x1000,
  'PCNOC_4_BUS_TIMEOUT':                    0x1000,
  'PCNOC_5_BUS_TIMEOUT':                    0x1000,
  'PCNOC_6_BUS_TIMEOUT':                    0x1000,
  'PCNOC_7_BUS_TIMEOUT':                    0x1000,
  'RBCPR_WRAPPER':                          0x1000,
  'ULTAUDIO_CORE':                         0x19000,

  'A7SS_L2':                                   0x0,
  'A7SS_L2_END':                               0x0,
  'A7SS_L2_SIZE':                              0x0,
  'BOOT_ROM_START_ADDRESS':                    0x0,
  'BOOT_ROM_END_ADDRESS':                      0x0,
  'BOOT_ROM_SIZE':                             0x0,
  'CATS_64BIT':                                0x0,
  'CATS_64BIT_END':                            0x0,
  'CATS_64BIT_SIZE':                           0x0,
  'DDR':                                       0x0,
  'DDR_END':                                   0x0,
  'DDR_SIZE':                                  0x0,
  'RPM_CODE_RAM_START_ADDRESS':                0x0,
  'RPM_CODE_RAM_END_ADDRESS':                  0x0,
  'RPM_SS_MSG_RAM_START_ADDRESS':              0x0,
  'RPM_SS_MSG_RAM_END_ADDRESS':                0x0,
  'RPM_MSG_RAM':                               0x0,
  'RPM_CODE_RAM_SIZE':                         0x0,
  'RPM_SS_MSG_RAM_SIZE':                       0x0,
  'MSS_TCM':                                   0x0,
  'MSS_TCM_SIZE':                              0x0,
  'MSS_TCM_END':                               0x0,
  'IMEM':                                      0x0,
  'IMEM_END':                                  0x0,
  'IMEM_SIZE':                                 0x0,
  'QDSS_STM':                                  0x0,
  'QDSS_STM_END':                              0x0,
  'QDSS_STM_SIZE':                             0x0,
  'LPASS_LPM':                                 0x0,
  'LPASS_LPM_END':                             0x0,
  'LPASS_LPM_SIZE':                            0x0,
}

HWIO_BASE_FILES = [
  {
    'filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/phys/msmhwiobase.h',
    'bases': bases,
    'map-type': 'physical',
    'resize': base_resize,
    'check-sizes': True,
    'check-for-overlaps': True,
  },
]


# ============================================================================
# HWIO_REGISTER_FILES
# ============================================================================

# ============================================================================
# HWIO_T32VIEW_FILES
# ============================================================================

HWIO_T32VIEW_FILES = [
  {
    'symbol-filename': '../scripts/' + CHIPSET + '/hwio.cmm',
    'limit-array-size': [ 10, 4 ],
    'per-filename': '../scripts/' + CHIPSET + '/hwioreg',
    'filter-exclude': ['RESERVED', 'DUMMY']
  },
]


# ============================================================================
# Main
#
# Entry point when invoking this directly.
# ============================================================================

if __name__ == "__main__":
  from subprocess import Popen
  Popen(["\\\\ben\\corebsp_labdata_0001\\sysdrv\\hwio\\HWIOGen.py", "--cfg=hwio_" + CHIPSET + ".py", "--flat=../../../api/systemdrivers/hwio/" + CHIPSET + "/ARM_ADDRESS_FILE.FLAT"], shell=True)

