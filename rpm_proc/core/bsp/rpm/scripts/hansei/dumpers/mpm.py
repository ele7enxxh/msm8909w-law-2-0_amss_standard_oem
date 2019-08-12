#  ===========================================================================
#
#  Copyright (c) 2013 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function

import logging
logger = logging.getLogger(__name__)

import os
import itertools
import math
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *

#masks for length of bit region
masks = {
    0 : 0x00000000,
    1 : 0x00000001,
    2 : 0x00000003,
    3 : 0x00000007,
    4 : 0x0000000F,
    5 : 0x0000001F,
    6 : 0x0000003F,
    7 : 0x0000007F,
    8 : 0x000000FF,
    9 : 0x000001FF,
    10: 0x000003FF,
    11: 0x000007FF,
    12: 0x00000FFF,
    13: 0x00001FFF,
    14: 0x00003FFF,
    15: 0x00007FFF,
    16: 0x0000FFFF,
    17: 0x0001FFFF,
    18: 0x0003FFFF,
    19: 0x0007FFFF,
    20: 0x000FFFFF,
    21: 0x001FFFFF,
    22: 0x003FFFFF,
    23: 0x007FFFFF,
    20: 0x00FFFFFF,
}
    
# bitfields for commond types
mpm_common_types = {
    'MPM_CMD_REGISTER' : {
        0 : {
            'name' : 'SLAVE_DATA',
            'end'  : 7,
        },
        8 : {
            'name' : 'SLAVE_ADDR',
            'end'  : 23,
        },
        24: {
            'name' : 'SLAVE_ID',
            'end'  : 27,
        },
        28: {
            'name' : 'DELAY',
            'end'  : 31,
        },
    },
}

all_target_mpm_breakdown = {
    'MPM2_default' : {
        0 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_1',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        1 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_2',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        2 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_3',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        3 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_4',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        4 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_5',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        5 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_6',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        6 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_7',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        7 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_8',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        8 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_9',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        9 : {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_10',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        10: {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_11',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        11: {
            'name' : 'MPM2_MPM_MPM_PMIC_VDD_ON_CFG_12',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        12: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_1',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        13: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_2',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        14: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_3',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        15: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_4',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        16: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_5',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        17: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_6',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        18: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_7',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        19: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_8',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        20: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_9',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        21: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_10',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        22: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_11',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        23: {
            'name' : 'MPM_PMIC_VDD_OFF_CFG_12',
            'type' : 'MPM_CMD_REGISTER',
            'bitfield' : {},            
        },
        24: {
            'name' : 'MPM_SPMI_CMD_CFG',
            'type' : 'MPM_SPMI_CMD_CFG',
            'bitfield' : {
                0 : {
                    'name' : 'NUM_CMD_ON',
                    'end'  : 3,
                },
                4 : {
                    'name' : 'NUM_CMD_OFF',
                    'end'  : 7,
                },
                8 : {
                    'name' : 'RESERVED_BITS31_8',
                    'end'  : 31,
                },
            },            
        },
        25: {
            'name' : 'MPM_SPMI_CDIV_CNTRL',
            'type' : 'MPM_SPMI_CDIV_CNTRL',
            'bitfield' : {
                0 : {
                    'name' : 'RO_DIV',
                    'end'  : 8,
                },
                9 : {
                    'name' : 'XO_DIV',
                    'end'  : 10,
                },
                11: {
                    'name' : 'RESERVED_BITS31_11',
                    'end'  : 31,
                },
            },            
        },
        26: {
            'name' : 'MPM_CXO_CTRL',
            'type' : 'MPM_CXO_CTRL',
            'bitfield' : {
                0 : {
                    'name' : 'CXO_DIS',
                    'end'  : 0,
                },
                1 : {
                    'name' : 'CXOPAD_DIS',
                    'end'  : 1,
                },
                2 : {
                    'name' : 'RESERVED_BITS31_2',
                    'end'  : 31,
                },
            },            
        },
        27: {
            'name' : 'MPM_PXO_OSC_CTRL',
            'type' : 'MPM_PXO_OSC_CTRL',
            'bitfield' : {
                0 : {
                    'name' : 'PXO_OSC_RF_BYPASS',
                    'end'  : 0,
                },
                1 : {
                    'name' : 'PXOPAD_HV_DIS',
                    'end'  : 1,
                },
                2 : {
                    'name' : 'PXO_OSC_SLEEP',
                    'end'  : 2,
                },
                3 : {
                    'name' : 'PXOPAD_CORE_DIS',
                    'end'  : 3,
                },
                4 : {
                    'name' : 'PXO_OSC_GAIN',
                    'end'  : 6,
                },
                7 : {
                    'name' : 'RESERVED_BITS31_7',
                    'end'  : 31,
                },
            },            
        },
        28: {
            'name' : 'MPM_LOW_POWER_CFG',
            'type' : 'MPM_LOW_POWER_CFG',
            'bitfield' : {
                0 : {
                    'name' : 'CXO_SD_EN',
                    'end'  : 0,
                },
                1 : {
                    'name' : 'PXO_SD_EN',
                    'end'  : 1,
                },
                2 : {
                    'name' : 'VDDCX_PC_EN',
                    'end'  : 2,
                },
                3 : {
                    'name' : 'VDDCX_MIN_EN',
                    'end'  : 3,
                },
                4 : {
                    'name' : 'VDDMX_PC_EN',
                    'end'  : 4,
                },
                5 : {
                    'name' : 'VDDMX_MIN_EN',
                    'end'  : 5,
                },
                6 : {
                    'name' : 'SW_EBI1_CTL_VALUE',
                    'end'  : 6,
                },
                7 : {
                    'name' : 'SW_EBI1_CTL_ENABLE',
                    'end'  : 7,
                },
                8 : {
                    'name' : 'DEBUG_ON',
                    'end'  : 8,
                },
                9 : {
                    'name' : 'DISABLE_FREEZE',
                    'end'  : 9,
                },
                10: {
                    'name' : 'DISABLE_CLAMPS',
                    'end'  : 10,
                },
                11: {
                    'name' : 'DISABLE_CORE_RESET',
                    'end'  : 11,
                },
                12: {
                    'name' : 'DISABLE_TRST_N',
                    'end'  : 12,
                },
                13: {
                    'name' : 'RESERVED_BIT13',
                    'end'  : 13,
                },
                14: {
                    'name' : 'ENABLE_CXOPAD_GATING',
                    'end'  : 14,
                },
                15: {
                    'name' : 'RESERVED_BIT15',
                    'end'  : 15,
                },
                16: {
                    'name' : 'DISABLE_HW_RESTORED',
                    'end'  : 16,
                },
                17: {
                    'name' : 'DISABLE_CLAMP_MEM',
                    'end'  : 17,
                },
                18: {
                    'name' : 'DISABLE_FREEZE_IO_M',
                    'end'  : 18,
                },
                19: {
                    'name' : 'SMEM_EN',
                    'end'  : 19,
                },
                20: {
                    'name' : 'RESERVED_BITS31_20',
                    'end'  : 31,
                },
            },
        },
        29: {
            'name' : 'MPM_LOW_POWER_STATUS',
            'type' : 'MPM_LOW_POWER_STATUS',
            'bitfield' : {
                0 : {
                    'name' : 'FSM_STATE',
                    'end'  : 3,
                },
                4 : {
                    'name' : 'POWER_DOWN',
                    'end'  : 4,
                },
                5 : {
                    'name' : 'CXO_DOWN',
                    'end'  : 5,
                },
                6 : {
                    'name' : 'PXO_DOWN',
                    'end'  : 6,
                },
                7 : {
                    'name' : 'VDDCX_DOWN',
                    'end'  : 7,
                },
                8 : {
                    'name' : 'VDDMX_DOWN',
                    'end'  : 8,
                },
                9 : {
                    'name' : 'SMEM_FSM_STATE',
                    'end'  : 11,
                },
                12: {
                    'name' : 'SPMI_SEQ_FSM',
                    'end'  : 13,
                },
                14: {
                    'name' : 'RESERVED_BITS31_14',
                    'end'  : 31,
                },
            },
        },
        30: {
            'name' : 'MPM_CXO_POWER_RAMPUP_TIME',
            'type' : 'MPM_CXO_POWER_RAMPUP_TIME',
            'bitfield' : {
                0 : {
                    'name' : 'POWER_RAMPUP_TIME',
                    'end'  : 15,
                },
                16: {
                    'name' : 'CXO_WARMUP_TIME',
                    'end'  : 31,
                },
            },
        },
        31: {
            'name' : 'MPM_PXO_WARMUP_TIME',
            'type' : 'MPM_PXO_WARMUP_TIME',
            'bitfield' : {
                0 : {
                    'name' : 'PXO_WARMUP_TIME',
                    'end'  : 15,
                },
                16: {
                    'name' : 'RESERVED_BITS31_16',
                    'end'  : 31,
                },
            },
        },
        32: {
            'name' : 'MPM_HARDWARE_RESTORED',
            'type' : 'MPM_HARDWARE_RESTORED',
            'bitfield' : {
                0 : {
                    'name' : 'DATA {WRITE-ONLY}',
                    'end'  : 0,
                },
                1 : {
                    'name' : 'RESERVED_BITS31_1',
                    'end'  : 31,
                },
            },
        },
        33: {
            'name' : 'MPM_INT_CLEAR_1',
            'type' : 'MPM_INT_CLEAR_1',
            'bitfield' : {
                0 : {
                    'name' : 'INT_CLEAR_1',
                    'end'  : 31,
                },
            },
        },
        34: {
            'name' : 'MPM_INT_CLEAR_2',
            'type' : 'MPM_INT_CLEAR_2',
            'bitfield' : {
                0 : {
                    'name' : 'INT_CLEAR_2',
                    'end'  : 31,
                },
            },
        },
        35: {
            'name' : 'MPM_INT_EN_1',
            'type' : 'MPM_INT_EN_1',
            'bitfield' : {
                0 : {
                    'name' : 'INT_EN_1',
                    'end'  : 31,
                },
            },
        },
        36: {
            'name' : 'MPM_INT_EN_2',
            'type' : 'MPM_INT_EN_2',
            'bitfield' : {
                0 : {
                    'name' : 'INT_EN_2',
                    'end'  : 31,
                },
            },
        },
        37: {
            'name' : 'MPM_INT_POLARITY_1',
            'type' : 'MPM_INT_POLARITY_1',
            'bitfield' : {
                0 : {
                    'name' : 'INT_POLARITY_1',
                    'end'  : 31,
                },
            },
        },
        38: {
            'name' : 'MPM_INT_POLARITY_2',
            'type' : 'MPM_INT_POLARITY_2',
            'bitfield' : {
                0 : {
                    'name' : 'INT_POLARITY_2',
                    'end'  : 31,
                },
            },
        },
        39: {
            'name' : 'MPM_DETECT_CTL_1',
            'type' : 'MPM_DETECT_CTL_1',
            'bitfield' : {
                0 : {
                    'name' : 'DETECT_CTL_1',
                    'end'  : 31,
                },
            },
        },
        40: {
            'name' : 'MPM_DETECT_CTL_2',
            'type' : 'MPM_DETECT_CTL_2',
            'bitfield' : {
                0 : {
                    'name' : 'DETECT_CTL_2',
                    'end'  : 31,
                },
            },
        },
        41: {
            'name' : 'MPM_DETECT_CTL_3',
            'type' : 'MPM_DETECT_CTL_3',
            'bitfield' : {
                0 : {
                    'name' : 'DETECT_CTL_3',
                    'end'  : 31,
                },
            },
        },
        42: {
            'name' : 'MPM_DETECT_CTL_4',
            'type' : 'MPM_DETECT_CTL_4',
            'bitfield' : {
                0 : {
                    'name' : 'DETECT_CTL_4',
                    'end'  : 31,
                },
            },
        },
        43: {
            'name' : 'MPM_INT_STATUS_1',
            'type' : 'MPM_INT_STATUS_1',
            'bitfield' : {
                0 : {
                    'name' : 'INT_STATUS_1',
                    'end'  : 31,
                },
            },
        },
        44: {
            'name' : 'MPM_INT_STATUS_2',
            'type' : 'MPM_INT_STATUS_2',
            'bitfield' : {
                0 : {
                    'name' : 'INT_STATUS_2',
                    'end'  : 31,
                },
            },
        },
        45: {
            'name' : 'MPM_INT_WKUP_CLK_SEL_1',
            'type' : 'MPM_INT_WKUP_CLK_SEL_1',
            'bitfield' : {
                0 : {
                    'name' : 'MPM_INT_WKUP_CLK_SEL_1',
                    'end'  : 31,
                },
            },
        },
        46: {
            'name' : 'MPM_INT_WKUP_CLK_SEL_2',
            'type' : 'MPM_INT_WKUP_CLK_SEL_2',
            'bitfield' : {
                0 : {
                    'name' : 'MPM_INT_WKUP_CLK_SEL_2',
                    'end'  : 31,
                },
            },
        },
        47: {
            'name' : 'MPM_DEBUG_BUS_EN',
            'type' : 'MPM_DEBUG_BUS_EN',
            'bitfield' : {
                0 : {
                    'name' : 'DEBUG_EN',
                    'end'  : 0,
                },
                1 : {
                    'name' : 'DEBUG_EN_1',
                    'end'  : 1,
                },
                2 : {
                    'name' : 'TEST_BUS_M_EN',
                    'end'  : 2,
                },
                3 : {
                    'name' : 'TEST_BUS_S_EN',
                    'end'  : 3,
                },
                4 : {
                    'name' : 'QTMR_TEST_BUS_EN',
                    'end'  : 4,
                },
                5 : {
                    'name' : 'RESERVED_BITS31_5',
                    'end'  : 31,
                },
            },
        },
        48: {
            'name' : 'MPM_PEN_DEBOUNCE_CTL',
            'type' : 'MPM_PEN_DEBOUNCE_CTL',
            'bitfield' : {
                0 : {
                    'name' : 'TIME',
                    'end'  : 2,
                },
                3 : {
                    'name' : 'ENABLE',
                    'end'  : 3,
                },
                4 : {
                    'name' : 'RESERVED_BITS31_4',
                    'end'  : 31,
                },
            },
        },
        49: {
            'name' : 'MPM_WARM_BOOT_CFG',
            'type' : 'MPM_WARM_BOOT_CFG',
            'bitfield' : {
                0 : {
                    'name' : 'ENABLE_EBI1',
                    'end'  : 0,
                },
                1 : {
                    'name' : 'ENABLE_EBI2',
                    'end'  : 1,
                },
                2 : {
                    'name' : 'RESERVED_BITS31_2',
                    'end'  : 31,
                },
            },
        },
    },
}

def dump(dump_path, memory, debug_info, fill_char):
    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['rpm_core_dump'].vartype
    rpm_core_dump = decode_object('rpm_core_dump', address, dump_type, memory, debug_info)

    #import pdb; pdb.set_trace()
    rpm = cast(rpm_core_dump.rpmserver_state, 'SystemData', memory, debug_info)

    #save_logger_level = update_logger(logging.DEBUG, logging)
    dump_mpm(dump_path, rpm, memory, debug_info, fill_char)
    #update_logger(save_logger_level, logging)

def dump_mpm(dump_path, rpm, memory, debug_info, fill_char):
    mpm_file_name = os.path.join(dump_path, 'mpm.txt')
    with open(mpm_file_name, 'w') as mpm_file:
        
        print("\n ~~MPM Register Dump~~", file=mpm_file)
        
        try:
            mpm_type = debug_info.variables['rpm_mpm_registers'].die
            mpm_address = debug_info.variables['rpm_mpm_registers'].address
        except:
            print("Failed to find mpm register dump variable", file=mpm_file)
            return
        
        # This will break if mpm_register type ever changes...
        mpm_type2 = debug_info.types['uint32_t']
        mpm_array = darray('mpm_registers', mpm_address, mpm_type2, 50, memory, debug_info)
        
        '''
        mpm_reg_0 = decode_object('rpm_mpm_registers', mpm_address, None, memory, debug_info, die=mpm_type2)
        test = memory.read(mpm_address+(0*4), 4)
        test[3].encode('hex')+test[2].encode('hex')+test[1].encode('hex')+test[0].encode('hex')
        memory.read(mpm_address+(x*4), 4)[3].encode('hex')+memory.read(mpm_address+(x*4), 4)[2].encode('hex')+memory.read(mpm_address+(x*4), 4)[1].encode('hex')+memory.read(mpm_address+(x*4), 4)[0].encode('hex')
        mpm_regs = decode_object('rpm_mpm_registers', mpm_address, None, memory, debug_info, die=mpm_type)
        '''
        
        mpm_info = all_target_mpm_breakdown['MPM2_default']
        
        
        for reg_num in range(50):
            reg = mpm_array[reg_num]
            print("\n\tREGISTER[%02.0u] = 0x%0.8x" % (reg_num, reg), file=mpm_file)
            
            try:
                mpm_reg = mpm_info[reg_num]
                print("\t\tRegister Name: %s"%(mpm_reg['name']), file=mpm_file)
                if mpm_reg['name'] != mpm_reg['type'] :
                    bitfield = mpm_common_types[mpm_reg['type']]
                else:
                    bitfield = mpm_reg['bitfield']
                print("\t\tRegister Decode:", file=mpm_file)
                index = 0
                while index < 32 :
                    region = bitfield[index]
                    data = mpm_array[reg_num]
                    data = data >> index
                    mask = 1
                    for i in range(index, region['end']):
                        mask = (mask << 1) + 1
                    data = data & mask
                    hex_digits = int(math.ceil(float(region['end']-index)/4))
                    fill_char = {
                        fill_char       : fill_char[0],
                        "underscore"    : "_",
                        "dot"           : ".",
                        "space"         : " ",
                        "zero"          : "0",
                        ""              : " ",
                    }[fill_char]
                    hex_out = ("{0:%s>8s}"%(fill_char)).format(("{0:0>%dX}"%(hex_digits)).format(data))
                    print("\t\t\t[{0:2d}:{1:2d}] {2:>25s} = 0x{3:>8s}".format(index, region['end'], region['name'], hex_out ), file=mpm_file)
                    
                    
                    index = region['end']+1
            except:
                print("\t\t!!! Failure in accessing mpm register decode. !!!\n", file=mpm_file)
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
