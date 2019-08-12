#===========================================================================
#
#  @file hwio_msm8916.py
#  @brief HWIO config file for the HWIO generation scripts for MSM8916.
#
#  This file can be invoked by calling:
#
#    HWIOGen.py --cfg=hwio_msm8916.py --flat=..\..\..\api\systemdrivers\hwio\msm8916\ARM_ADDRESS_FILE.FLAT
#
#  ===========================================================================
#
#  Copyright (c) 2012 Qualcomm Technologies Incorporated.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/hwio/build/hwio_msm8909.py#1 $
#  $DateTime: 2015/09/01 00:30:35 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

CHIPSET = 'msm8909'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

bases = [
  'BOOT_ROM',
  'BOOT_ROM_START_ADDRESS',
  'BOOT_ROM_END_ADDRESS',
  'RPM',
  'RPM_SS_MSG_RAM_START_ADDRESS',
  'SECURITY_CONTROL',
  'SPDM_WRAPPER_TOP',
  'MPM2_MPM',
  'MSS_TOP',
  'PMIC_ARB',
  'QDSS_QDSS',
  'CLK_CTL',
  'CORE_TOP_CSR',
  'CRYPTO0_CRYPTO_TOP',
  'SYSTEM_NOC',
  'TLMM',
  'BIMC',
  'WCSS_WCSS',
  'A7SS',
  'ULTAUDIO_CORE',
  'PRNG_PRNG',
  'PC_NOC',
  'BLSP1_BLSP',
  'PDM_PERPH_WEB',
  'DEHR_BIMC_WRAPPER',
  'SDC1_SDCC5_TOP',
  'SDC2_SDCC5_TOP',
  'QDSS_WRAPPER_TOP',
  'QPIC_QPIC'
]

base_resize = {
  'MPM2_MPM':                        0xC000,
  'RPM':                            0x89000,
  'BOOT_ROM':                      0x100000,
  'BOOT_ROM_START_ADDRESS':             0x0,
  'BOOT_ROM_END_ADDRESS':               0x0,
  'RPM_SS_MSG_RAM_START_ADDRESS':    0x5000,
  'CLK_CTL':                        0x90000,
  'PMIC_ARB':                     0x1906000,
  'MSS_TOP':                       0x400000,
  'QDSS_QDSS':                      0xa0000,
  'CORE_TOP_CSR':                   0x58000,
  'CRYPTO0_CRYPTO_TOP':             0x40000,
  'SYSTEM_NOC':                     0x21000,
  'SECURITY_CONTROL':                0x8000,
  'SPDM_WRAPPER_TOP':                0x6000,
  'TLMM':                          0x301000,
  'BIMC':                           0x70000,
  'WCSS_WCSS':                     0x4c1000,
  'A7SS':                          0xc0000,
  'ULTAUDIO_CORE':                  0x1d000,
  'PRNG_PRNG':                       0x1000,
  'PC_NOC':                         0x11000,
  'BLSP1_BLSP':                     0x3b000,
  'PDM_PERPH_WEB':                   0x4000,
  'DEHR_BIMC_WRAPPER':               0x4000,
  'SDC1_SDCC5_TOP':                 0x25000,
  'SDC2_SDCC5_TOP':                 0x25000,
  'QDSS_WRAPPER_TOP':                0x6000,
  'QPIC_QPIC':                      0x32000
}

HWIO_BASE_FILES = [
  {
    'filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwiobase.h',
    'bases': bases,
    'map-type': 'physical',
    'resize': base_resize,
    'map-filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwiomap.h',
    'check-sizes': True,
    'check-for-overlaps': True,
    #'ignore-overlaps': [ ('LPASS_CSR', 'LPASS_M2VMT'),
    #                     ('LPASS_CSR', 'LPASS_M2VMT_Q6SS'),
    #                     ('LPASS_CSR', 'LPASS_AHBTM') ],
  }
]


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
  hwiogen = Popen(["\\\\ben\\corebsp_labdata_0001\\sysdrv\\hwio\\HWIOGen.py", "--cfg=hwio_" + CHIPSET + ".py", "--flat=../../../api/systemdrivers/hwio/" + CHIPSET + "/ARM_ADDRESS_FILE.FLAT"], shell=True)
  hwiogen.wait()


