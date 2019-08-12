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
#  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hwio/build/hwio_msm8909.py#1 $
#  $DateTime: 2016/12/13 07:59:23 $
#  $Author: mplcsds1 $
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
  'ULTAUDIO_CORE',
  'PRNG_PRNG',
  'PC_NOC',
  'BLSP1_BLSP',
  'PDM_PERPH_WEB',
  'DEHR_BIMC_WRAPPER',
  'XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB',
  'QPIC_QPIC'
]

base_resize = {
  'MPM2_MPM':                                0xc000,
  'RPM':                                     0x89000,
  'BOOT_ROM':                                0x100000,
  'BOOT_ROM_START_ADDRESS':                  0x0,
  'BOOT_ROM_END_ADDRESS':                    0x0,
  'RPM_SS_MSG_RAM_START_ADDRESS':            0x5000,
  'CLK_CTL':                                 0x82000,
  'PMIC_ARB':                                0x1906000,
  'MSS_TOP':                                 0x38b000,                  
  'QDSS_QDSS':                               0x99000,
  'CORE_TOP_CSR':                            0x58000,
  'CRYPTO0_CRYPTO_TOP':                      0x40000,
  'SYSTEM_NOC':                              0x13000,
  'SECURITY_CONTROL':                        0x8000,
  'SPDM_WRAPPER_TOP':                        0x5000,
  'TLMM':                                    0x301000,
  'BIMC':                                    0x5a000,
  'WCSS_WCSS':                               0x4c1000,
  'ULTAUDIO_CORE':                           0x1d000,
  'PRNG_PRNG':                               0x1000,
  'PC_NOC':                                  0x11000,
  'BLSP1_BLSP':                              0x3b000,
  'PDM_PERPH_WEB':                           0x4000,
  'DEHR_BIMC_WRAPPER':                       0x4000,
  'XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB':   0x1000,
  'QPIC_QPIC':                               0x32000
}

HWIO_BASE_FILES = [
  {
    'filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwiobase.h',
    'bases': bases,
    'map-type': 'qurt',
    'virtual-address-start': 0xE0000000,
    'virtual-address-end': 0xF0000000,
    'resize': base_resize,
    'qurt-memsection-filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwioplat.xml',
     'qurt-memsection-attributes': {'PRNG_PRNG': 'tlb_lock="boot" glob="1"', 'CLK_CTL': 'tlb_lock="boot" glob="1"'},
    'default-cache-policy': 'device',
    'devcfg-filename': '../config/' + CHIPSET + '/HWIOBaseMap.c',
    'check-sizes': True,
    'check-for-overlaps': True,

    # Modem firmware needs MSS_TOP to have fixed virtual 
    # address to set  special cache attributes. This is  
    # handled in the top-level qdsp6.xml file.
    'fixed-virtual-address': { 'MSS_TOP': 0xEC000000 },
    'skip-memsection': ['MSS_TOP'],
  }
]


# ============================================================================
# HWIO_REGISTER_FILES
# ============================================================================

HWIO_REGISTER_FILES = [
  {
    'filename': '../hw/' + CHIPSET + '/msmhwioreg.h.ref',
    'bases': bases,
    'filter-exclude': ['RESERVED', 'DUMMY']
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
    'filter-exclude': ['RESERVED', 'DUMMY'],
    'header': '''
#error This file is for reference only and cannot be included.  See "go/hwio" or mail corebsp.sysdrv.hwio for help.
'''
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


