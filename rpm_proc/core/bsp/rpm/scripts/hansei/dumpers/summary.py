#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================

from __future__ import print_function

import os
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object
from hansei_utils import *
import re

from sys import stdout

chips = {
    # family '0' -> Badger family
    0 : {
        0 : {
            'MSM_ID' : 'MSM8974',
            'num'    : '8974',
            'name'   : 'Aragorn',
            },      # chip '0' -> Aragorn / 8974
        1 : {
            'MSM_ID' : 'MDM9x25',
            'num'    : '9x25',
            'name'   : 'Elan',
            },      # chip '1' -> Elan / 9x25
        2 : {
            'MSM_ID' : 'MSM8x26',
            'num'    : '8x26',
            'name'   : 'Frodo',
            },      # chip '2' -> Frodo / 8x26
        3 : {
            'MSM_ID' : 'APQ8084',
            'num'    : '8084',
            'name'   : 'Gandalf',
            },      # chip '3' -> Gandalf / 8084
        4 : {
            'MSM_ID' : 'MSM8926',
            'num'    : '8926',
            'name'   : 'Gimli',
            },      # chip '4' -> Gimli / 8926
        5 : {
            'MSM_ID' : 'MSM8x62',
            'num'    : '8x62',
            'name'   : 'Thorin',
            },      # chip '5' -> Thorin / 8x62
        6 : {
            'MSM_ID' : 'MPQ8092',
            'num'    : '8092',
            'name'   : 'Istari',
            },      # chip '6' -> Istari / 8092H
        7 : {
            'MSM_ID' : 'MDM9x35',
            'num'    : '9x35',
            'name'   : 'Torino',
            },      # chip '7' -> Torino / 9x35
        8 : {
            'MSM_ID' : 'MSM8974 Pro',
            'num'    : '8974 Pro',
            'name'   : 'Aragorn Pro',
            },  # chip '8' -> Aragorn Pro / 8974 Pro
        9 : {
            'MSM_ID' : 'FSM9900',
            'num'    : '9900',
            'name'   : 'Trahira',
            },      # chip '9' -> Trahira / 9900 -> base Station modem, no RPM
        10: {
            'MSM_ID' : 'UNKNOWN',
            'num'    : '????',
            'name'   : 'Unknown Chip',
            },      # chip '10'-> Unknown Chip
        11: {
            'MSM_ID' : 'MSM8994',
            'num'    : '8994',
            'name'   : 'Elessar',
            },      # chip '11'-> Elessar / 8994
    },
    # family '1' -> also Badger Family
    1: {
        0 : {
            'MSM_ID' : 'MSM8x10',
            'num'    : '8x10',
            'name'   : 'Dino',
            },      # chip '0' -> Dino / 8x10
    },
    # family '32' -> Bear family
    32 : {
        0 : {
            'MSM_ID' : 'MSM8916',
            'num'    : '8916',
            'name'   : 'Bagheera',
            },      # chip '0' -> Bagheera / 8916
        1 : {
            'MSM_ID' : 'MSM8936',
            'num'    : '8936',
            'name'   : 'Shere',
            },      # chip '1' -> Shere / 8936
        2 : {
            'MSM_ID' : 'MDM9x45',
            'num'    : '9x45',
            'name'   : 'Tesla',
            },      # chip '2' -> Tesla / 9x45
        3 : {
            'MSM_ID' : 'MSM8909',
            'num'    : '8909',
            'name'   : 'Cheel',
            },      # chip '3' -> Cheel / 8909
	9 : {
            'MSM_ID' : 'MDM9x07',
            'num'    : '9x07',
            'name'   : 'Lykan',
            },      # chip '9' -> Lykan / 9x07
    },
    # family '48' -> HoneyBadger Family
    48: {
        0 : {
            'MSM_ID' : 'MSM8996',
            'num'    : '8996',
            'name'   : 'Istari',
            },      # chip '0' -> Istari / 8996
    },
}

def find_version_info(str_name, mem_start, mem_end, memory):
    output = ""
    for addr in range(mem_start, mem_end, 1):
        if (memory.read(addr, len(str_name)) == str_name):
            offset = len(str_name)
            while (memory.read(addr+offset, 1) != "\0" and (addr+offset) < mem_end):
                output+=memory.read(addr+offset, 1)
                offset+=1
    if (output):
        return output
    else:
        return "NOT FOUND"

def dump(dump_path, memory, debug_info, base_address):
    
    version_address_start = base_address + 0x0
    version_address_end   = version_address_start + 0x1000
    
    #regex to match the RPM branch name:
    regex = '^RPM\.BF\.\d\.\d.*(?=-)'
    
    dump = open(os.path.join(dump_path, 'rpm-summary.txt'), 'w')

    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['rpm_core_dump'].vartype
    rpm_core_dump = decode_object('rpm_core_dump', address, dump_type, memory, debug_info)

    hw_info = rpm_core_dump.hw_version.parts
    chip_name = '<unknown device -> family %d device %d>' % (hw_info.family_number, hw_info.device_number)
    target_chip = chips.get(hw_info.family_number, {}).get(hw_info.device_number, {})
    chip_name = target_chip.get('MSM_ID', chip_name)
    print('\nCollected from %s %d.%d\n' % (chip_name, hw_info.major_version, hw_info.minor_version), file=dump)

    rpm_ver_type = debug_info.variables['rpm_core_dump'].vartype
    
    rpm_build_date_type = debug_info.variables['gBuildDate'].die
    rpm_build_date_address = debug_info.variables['gBuildDate'].address
    rpm_build_time_type = debug_info.variables['gBuildTime'].die
    rpm_build_time_address = debug_info.variables['gBuildTime'].address
    rpm_build_date = decode_object('gBuildDate', rpm_build_date_address, None, memory, debug_info, die=rpm_build_date_type)
    rpm_build_time = decode_object('gBuildTime', rpm_build_time_address, None, memory, debug_info, die=rpm_build_time_type)
    
    QC_IMAGE_VERSION = find_version_info("QC_IMAGE_VERSION_STRING=",version_address_start, version_address_end, memory)
    print("QC_IMAGE_VERSION_STRING:  %s" % QC_IMAGE_VERSION, file=dump)
    print("IMAGE_VARIANT_STRING:     %s" % find_version_info("IMAGE_VARIANT_STRING=",version_address_start, version_address_end, memory), file=dump)
    print("OEM_IMAGE_VERSION_STRING: %s" % find_version_info("OEM_IMAGE_VERSION_STRING=",version_address_start, version_address_end, memory), file=dump)
    
    print("\nBuild Date: %s" % pointer_to_str(rpm_build_date), file=dump)
    print("Build Time: %s" % pointer_to_str(rpm_build_time), file=dump)
    
    if rpm_core_dump.dumped_at == 0:
        print('\nThe RPM is ok.', file=dump)
        # return chip name and RPM branch
        return [target_chip.get('num', '????'), re.compile(regex).match(QC_IMAGE_VERSION).group()]
    
    print('\nThe RPM crashed (@ time = 0x%x)' % rpm_core_dump.dumped_at, file=dump)
    xpsr = rpm_core_dump.registers.xPSR
    
    ipsr = xpsr & 0xff
    if ipsr == 0:
        print('\tNot in an exception context; vanilla fatal error scenario.', file=dump)
    elif ipsr < 16:
        fault_types = {
            2  : 'NMI',
            3  : 'Hard Fault',
            4  : 'Memory Management Fault',
            5  : 'Bus Fault',
            6  : 'Usage Fault',
            11 : 'SVCall',
            12 : 'Debug Monitor',
            14 : 'PendSV',
            15 : 'SysTick',
        }
        print('\tIn a fault context -> %s' % fault_types[ipsr], file=dump)
    else:
        print('\tFatal error inside ISR #%d' % (ipsr - 16), file=dump)
    
    
    def decode_bitfield(name, bit_definitions, data, joiner = ' | '):
        known_bits = 0
        for id in bit_definitions:
            known_bits |= (1 << id)
        unknown_data = data - (data & known_bits)
        string = joiner.join(['[' + bit_definitions[x] + ']' for x in bit_definitions if (1 << x) & data])
        if unknown_data:
            if string:
                string += ' ...and '
            multi_or_single = ''
            if log(unknown_data, 2) != int(log(unknown_data, 2)):
                multi_or_single = 's'
            string += 'unknown %s%s 0x%0.8x' % (name, multi_or_single, unknown_data)
        return string
    
    
    fault_regs = rpm_core_dump.fault_detail
    
    shcas_bits = {
        0  : 'Memory fault in progress',
        1  : 'Bus fault in progress',
        3  : 'Usage fault in progress',
        7  : 'SVCall in progress',
        8  : 'Debug Monitor in progress',
        10 : 'PendSV in progress',
        11 : 'SysTick in progress',
        12 : 'Usage fault pended',
        13 : 'Memory management fault pended',
        14 : 'Bus fault pended',
        15 : 'SVCall pended',
        16 : 'Memory management fault enabled (this is not a problem)',
        17 : 'Bus fault enabled (this is not a problem)',
        18 : 'Usage fault enabled (this is not a problem)',
    }
    
    print('\nSystem handler status {\n\t%s\n}' % decode_bitfield('bits', shcas_bits, fault_regs.SystemHandlerCtrlAndState, '\n\t'), file=dump)
    
    cfs_bits = {
        0  : 'Attempt to fetch an instruction from a non-executable region.',
        1  : 'Attempt to load or store a location that does not permit it.',
        3  : 'Unstacking from an exception return has caused one or more access violations.',
        4  : 'Stacking for an exception has caused one or more access violations.',
        7  : 'Memory manage address register (MMAR) is valid.',
        8  : 'Instruction bus error.',
        9  : 'Precise data bus error.',
        10 : 'Imprecise data bus error.',
        11 : 'Unstack from exception has caused one or more bus errors.',
        12 : 'Stacking for exception has caused one or more bus errors.',
        15 : 'Bus fault address register (BFAR) is valid.',
        16 : 'Undefined instruction exception.',
        17 : 'Invalid state exception.',
        18 : 'Illegal attempt to load EXC_RETURN into PC.',
        19 : 'Attempt to use a coprocessor instruction.',
        24 : 'Unaligned memory access.',
        25 : 'Divide by zero.',
    }
    
    print('\nConfigurable fault status {\n\t%s\n}' % decode_bitfield('bits', cfs_bits, fault_regs.ConfigurableFaultStatus, '\n\t'), file=dump)
    
    hfs_bits = {
        0  : 'Fault on vector table read during exception processing.',
        30 : 'Hard Fault activated because a configurable fault was received and cannot activate because of priority or it is disabled.',
        31 : 'Fault related to debug.',
    }
    
    print('\nHard fault status {\n\t%s\n}' % decode_bitfield('bits', hfs_bits, fault_regs.HardFaultStatus, '\n\t'), file=dump)
        
    dfs_bits = {
        0  : 'Halt requested by NVIC.',
        1  : 'BKPT instruction execution.',
        2  : 'DWT match.',
        3  : 'Vector fetch occurred.',
        4  : 'EDBGRQ signal asserted.',
    }
    
    print('\nDebug fault status {\n\t%s\n}' % decode_bitfield('bits', hfs_bits, fault_regs.DebugFaultStatus, '\n\t'), file=dump)
    
    if 0 != (fault_regs.ConfigurableFaultStatus & (1 << 7)):
        print('\nMemory manage fault address: 0x%0.8x' % fault_regs.MemManageAddress, file=dump)
    
    if 0 != (fault_regs.ConfigurableFaultStatus & (1 << 15)):
        print('\nMemory manage fault address: 0x%0.8x' % fault_regs.BusFaultAddress, file=dump)
    
    print('\nAuxilary fault address register: 0x%0.8x' % fault_regs.AuxFaultStatus, file=dump)
    dump.close()
    
    # return chip name and RPM branch
    return [target_chip.get('num', '????'), re.compile(regex).match(QC_IMAGE_VERSION).group()]
    
