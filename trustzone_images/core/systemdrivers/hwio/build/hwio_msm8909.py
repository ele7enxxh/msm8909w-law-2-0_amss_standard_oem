#===========================================================================
#
#  @file hwio_msm8916.py
#  @brief HWIO config file for the HWIO generation scripts for MSM8916
#
#  This file can be invoked by calling:
#
#    HWIOGen.py --cfg=hwio_msm8909.py --flat=..\..\..\api\systemdrivers\hwio\msm8909\ARM_ADDRESS_FILE.FLAT
#
#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/hwio/build/hwio_msm8909.py#1 $
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

CHIPSET = 'msm8909'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

bases = [
  '.*'  # We map everything for TZ for the moment.
]

base_resize = {
  'SYSTEM_NOC':                     0x13000,
  'RPM':                            0x89000,
  'BOOT_ROM_START_ADDRESS':             0x0,
  'BOOT_ROM_END_ADDRESS':               0x0,
  'BOOT_ROM':                      0x100000,
  'RPM_CODE_RAM_START_ADDRESS':         0x0,
  'RPM_CODE_RAM_END_ADDRESS':           0x0,
  'RPM_SS_MSG_RAM_START_ADDRESS':   0x60000,
  'RPM_SS_MSG_RAM_END_ADDRESS':         0x0,
  'RPM_MSG_RAM':                     0x5000,
  'RPM_CODE_RAM_SIZE':                  0x0,
  'CRYPTO0_CRYPTO_TOP':             0x40000,
  'BIMC':                           0x60000,
  'A7SS':                           0xba000,
  'CLK_CTL':                        0x82000,
  'MSS_TOP':                       0x38b000,
  'LPASS_LPM':                          0x0,
  'WCSS_WCSS':                     0x4c1000,
  'TLMM':                          0x301000,
  'OCIMEM_WRAPPER_CSR':              0x4000,
  'SPDM_WRAPPER_TOP':                0x5000,
  'PMIC_ARB':                     0x1906000,
  'EBI1_PHY_CFG':                   0x20000,
  'DEHR_BIMC_WRAPPER':               0x4000,
  'QDSS_QDSS':                      0x99000,
  'CORE_TOP_CSR':                   0x58000,
  'PC_NOC':                         0x11000,
  'BLSP1_BLSP':                     0x3b000,
  'CAMSS_CAMERA_SS':                0x43000,
  'GMEM':                            0x1000,
  'MDSS_MDSS_TOP':                  0xf2000,
  'OXILI_OXILI':                     0xf000,
  'PDM_PERPH_WEB':                   0x4000,
  'SDC1_SDCC5_TOP':                 0x25000,
  'SDC2_SDCC5_TOP':                 0x25000,
  'QDSS_WRAPPER_TOP':                0x6000,
  'SMMU_500_MPU_WRAPPER':            0x4000,
  'SMMU_500_REG_WRAPPER':          0x10c000,
  'QPIC_QPIC':                      0x32000,
  'VENUS0_VENUS':                   0xf1000,
  'XPU_CFG_PRNG_CFG_APU1132_1':                0x1000,
  'XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB':      0x1000,
  'XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB':     0x1000,
  'XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB':    0x1000,
  'USB2_HSIC_USB_OTG_HS_EP8_PIPES2_BAGHEERA': 0x20000,
  'USB2_PHY_CM_DWC_USB2_SW':                  0x1000,

  'GMEM_END':                           0x0,
  'GMEM_SIZE':                          0x0,
  'LPASS_LPM_END':                      0x0,
  'LPASS_LPM_SIZE':                     0x0,
  'BOOT_ROM_SIZE':                      0x0,
  'RPM_SS_MSG_RAM_SIZE':                0x0,
  'PRONTO_CMEM':                        0x0,
  'PRONTO_CMEM_SIZE':                   0x0,
  'PRONTO_CMEM_END':                    0x0,
  'SYSTEM_IMEM':                        0x0,
  'SYSTEM_IMEM_SIZE':                   0x0,
  'SYSTEM_IMEM_END':                    0x0,
  'MSS_TCM':                            0x0,
  'MSS_TCM_SIZE':                       0x0,
  'MSS_TCM_END':                        0x0,
  'KRAIT_L2_MEM':                       0x0,
  'KRAIT_L2_MEM_END':                   0x0,
  'KRAIT_L2_MEM_SIZE':                  0x0,
  'EBI1_MEM':                           0x0,
  'EBI1_MEM_END':                       0x0,
  'EBI1_MEM_SIZE':                      0x0,
  'EBI1_MEM_RPM_REGION0':               0x0,
  'EBI1_MEM_RPM_REGION0_END':           0x0,
  'EBI1_MEM_RPM_REGION0_SIZE':          0x0,
  'EBI1_MEM_RPM_REGION1':               0x0,
  'EBI1_MEM_RPM_REGION1_END':           0x0,
  'EBI1_MEM_RPM_REGION1_SIZE':          0x0,
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

