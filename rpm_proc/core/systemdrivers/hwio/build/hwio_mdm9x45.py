#===========================================================================
#
#  @file hwio_mdm9x45.py
#  @brief HWIO config file for the HWIO generation scripts for MDM9x45.
#
#  This file can be invoked by calling:
#
#    HWIOGen.py --cfg=hwio_mdm9x45.py --flat=..\..\..\api\systemdrivers\hwio\mdm9x45\ARM_ADDRESS_FILE.FLAT
#
#  ===========================================================================
#
#  Copyright (c) 2014 Qualcomm Technologies Incorporated.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hwio/build/hwio_mdm9x45.py#1 $
#  $DateTime: 2015/02/25 02:53:43 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

CHIPSET = 'mdm9x45'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

bases = [
  'RPM_CODE_RAM_START_ADDRESS',
  'RPM',
  'RPM_MSG_RAM',
  'SECURITY_CONTROL',
  'SPDM_WRAPPER_TOP',
  'MPM2_MPM',
  'PRNG_PRNG',
  'PMIC_ARB',
  'QDSS_APB_DEC_QDSS_APB',
  'QDSS_AHB_DEC_QDSS_AHB',
  'CLK_CTL',
  'CORE_TOP_CSR',
  'CRYPTO0_CRYPTO_TOP',
  'SYSTEM_NOC',
  'PC_NOC',
  'DEHR_BIMC_WRAPPER',
  'TLMM',
  'BIMC',
]

base_resize = {
  'RPM_CODE_RAM_START_ADDRESS':         0x0,
  'RPM':                            0x89000,
  'RPM_MSG_RAM':                     0x5000,
  'SECURITY_CONTROL':                0x8000,
  'SPDM_WRAPPER_TOP':                0x5000,
  'MPM2_MPM':                        0xc000,
  'PRNG_PRNG':                       0x1000,
  'PMIC_ARB':                     0x1906000,
  'QDSS_APB_DEC_QDSS_APB':          0x30000,
  'QDSS_AHB_DEC_QDSS_AHB':          0x30000,
  'CLK_CTL':                        0x90000,
  'CORE_TOP_CSR':                   0x58000,
  'CRYPTO0_CRYPTO_TOP':             0x40000,
  'SYSTEM_NOC':                     0x14000,
  'PC_NOC':                         0x11000,
  'DEHR_BIMC_WRAPPER':               0x4000,
  'TLMM':                          0x301000,
  'BIMC':                           0x70000,
}

HWIO_BASE_FILES = [
  {
    'filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwiobase.h',
    'bases': bases,
    'map-type': 'physical',
    'resize': base_resize,
    'check-sizes': True,
    'check-for-overlaps': True,
  }
]


# ============================================================================
# HWIO_REGISTER_FILES
# ============================================================================

HWIO_REGISTER_FILES = [
  {
    'filename': '../hw/' + CHIPSET + '/msmhwioreg.h',
    'bases': bases,
    'filter-exclude': ['RESERVED', 'DUMMY'],
    'header': '''
#error This file is for reference only and cannot be included.  See "go/hwio" or mail corebsp.sysdrv.hwio for help.
'''
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
  hwiogen = Popen(["\\\\ben\\corebsp_labdata_0001\\sysdrv\\hwio\\HWIOGen.py", "--cfg=hwio_" + CHIPSET + ".py", "--flat=../../../api/systemdrivers/hwio/" + CHIPSET + "/RPM_ADDRESS_FILE.FLAT"], shell=True)
  hwiogen.wait()

