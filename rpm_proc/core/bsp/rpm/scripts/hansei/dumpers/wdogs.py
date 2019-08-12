#  ===========================================================================
#
#  Copyright (c) 2014 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function

import logging
logger = logging.getLogger(__name__)

import os
import itertools
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *

def dump(dump_path, memory, debug_info, ocimem_load, pmic_rtc_load):
    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['rpm_core_dump'].vartype
    rpm_core_dump = decode_object('rpm_core_dump', address, dump_type, memory, debug_info)

    #import pdb; pdb.set_trace()
    rpm = cast(rpm_core_dump.rpmserver_state, 'SystemData', memory, debug_info)

    #save_logger_level = update_logger(logging.DEBUG, logging)
    dump_wdog(dump_path, rpm, memory, debug_info, ocimem_load, pmic_rtc_load)
    #update_logger(save_logger_level, logging)

def dump_wdog(dump_path, rpm, memory, debug_info, ocimem_load, pmic_rtc_load):
    wdog_file_name = os.path.join(dump_path, 'wdogs.txt')
    with open(wdog_file_name, 'w') as wdog_file:
        
        print("\n ~~RPM Watchdog Info~~", file=wdog_file)
        print("\n\t Stats:", file=wdog_file)
        rpm_dog_stats_type = debug_info.variables['dog_stats'].die
        rpm_dog_stats_address = debug_info.variables['dog_stats'].address
        rpm_dog_stats = decode_object('dog_stats', rpm_dog_stats_address, None, memory, debug_info, die=rpm_dog_stats_type)
        
        current_index = rpm_dog_stats.index-1
        if current_index < 0:
            current_index = 3
        
        for index_num in range(4):
            event = rpm_dog_stats.entries[index_num].dog_event
            esource = rpm_dog_stats.entries[index_num].dog_event_source
            timestamp = rpm_dog_stats.entries[index_num].timestamp
            if index_num == current_index:
                print("\t\t Entry %i [Last event]"%(index_num), file=wdog_file)
            else:
                print("\t\t Entry %i"%(index_num), file=wdog_file)
            print("\t\t\t\t-> dog event: %s"%event, file=wdog_file)
            print("\t\t\t\t-> dog event source: %s"%esource, file=wdog_file)
            print("\t\t\t\t-> timestamp: %#x"%timestamp, file=wdog_file)
        
        
        print("\n ~~PMIC Watchdog Info~~", file=wdog_file)
        
        try:
            pmic_dog_stats_type = debug_info.variables['pmic_dog_stats'].die
            pmic_dog_stats_address = debug_info.variables['pmic_dog_stats'].address
            pmic_dog_stats = decode_object('pmic_dog_stats', pmic_dog_stats_address, None, memory, debug_info, die=pmic_dog_stats_type)
        except:
            print("\t\tFailed to find pmic_dog_stats, RPM build likely does not have this feature.", file=wdog_file)
            return
            
        try:
            pmic_wdog_enable_type = debug_info.variables['pmic_wdog_enable'].die
            pmic_wdog_enable_address = debug_info.variables['pmic_wdog_enable'].address
            pmic_wdog_enable = decode_object('pmic_wdog_enable', pmic_wdog_enable_address, None, memory, debug_info, die=pmic_wdog_enable_type)
        except:
            print("\t\tFailed to find pmic_wdog_enable, RPM build likely does not have this feature.", file=wdog_file)
            return
            
        if pmic_wdog_enable == 0:
            print("\t\tPMIC wdog was disabled when dumps were taken. PMIC wdog likely not enabled on this target.", file=wdog_file)
            
        print("\n\t Stats:", file=wdog_file)
        current_index = pmic_dog_stats.index-1
        if current_index < 0:
            current_index = 3
        
        for index_num in range(4):
            event = pmic_dog_stats.entries[index_num].dog_event
            esource = pmic_dog_stats.entries[index_num].dog_event_source
            timestamp = pmic_dog_stats.entries[index_num].timestamp
            if index_num == current_index:
                print("\t\t Entry %i [Last event]"%(index_num), file=wdog_file)
            else:
                print("\t\t Entry %i"%(index_num), file=wdog_file)
            print("\t\t\t\t-> dog event: %s"%event, file=wdog_file)
            print("\t\t\t\t-> dog event source: %s"%esource, file=wdog_file)
            print("\t\t\t\t-> timestamp: %#x"%timestamp, file=wdog_file)
            
        try:
            pmic_timestamp_type = debug_info.variables['PMIC_timestamp'].die
            pmic_timestamp_address = debug_info.variables['PMIC_timestamp'].address
            pmic_timestamp = decode_object('PMIC_timestamp', pmic_timestamp_address, None, memory, debug_info, die=pmic_timestamp_type)
        except:
            print("\t\tFailed to find PMIC_timestamp, RPM build likely does not have this feature.", file=wdog_file)
            return
        
        print("\t\t RTC data:", file=wdog_file)
        pm_time = pmic_timestamp.pm_time
        rpm_time = pmic_timestamp.rpm_time
        print("\t\t\t -> PMIC RTC value: %u [hex: %#x] sec"%(pm_time,pm_time), file=wdog_file)
        print("\t\t\t -> RPM timestamp: %#x"%(rpm_time), file=wdog_file)
        rpm_start = pmic_timestamp.rpm_time
        rpm_end = pmic_dog_stats.entries[current_index].timestamp
        rpm_delta = (rpm_end-rpm_start)/19200000
        final_rtc = rpm_delta+pmic_timestamp.pm_time
        print("\t\t\t -> estimated PMIC RTC time at final PMIC wdog pet: %u sec"%final_rtc, file=wdog_file)
        
        # This is very experimental... warn that information may not be acurrate
        print("\n\t PMIC wdog fault parsing", file=wdog_file)
        print("\t\t WARNING: This parsing assumes that the PMIC RTC value is stored during the reboot following the PMIC wdog bite", file=wdog_file)
        
        from struct import unpack
        SDI_PMIC_RTC = 0
        SDI_PMIC_RTC_v2 = 0
        if ocimem_load == 1:
            try:
                SDI_PMIC_RTC = unpack('<I',memory.read(0x5774, 4))[0]
            except:
                print("\t\t WARNING: OCIMEM.BIN not loaded.", file=wdog_file)
                ocimem_load = 0
        
        if pmic_rtc_load == 1:
            try:
                SDI_PMIC_RTC_v2 = unpack('<I',memory.read(0x0, 4))[0]
            except:
                if OCIMEM == "fail":
                    print("\t\t ERRROR: PMIC_RTC.BIN not loaded, cannot read PMIC RTC value recorded by SDI.", file=wdog_file)
                    return
                pmic_rtc_load = 0
        
        print_RTC = 0
        
        if SDI_PMIC_RTC != SDI_PMIC_RTC_v2 and SDI_PMIC_RTC_v2 >= pmic_timestamp.pm_time:
            if pmic_rtc_load == 1 and ocimem_load == 1:
                print("\t\t WARNING: OCIMEM.BIN[0x5774] and PMIC_RTC.BIN report different PMIC RTC counter values on reboot.", file=wdog_file)
                print_RTC = 1
            elif ocimem_load == 0 and pmic_rtc_load == 1:
                # Use the value from PMIC_RTC if OCIMEM couldn't be loaded
                SDI_PMIC_RTC = SDI_PMIC_RTC_v2
                print_RTC = 1
        
        if SDI_PMIC_RTC < pmic_timestamp.pm_time:
            print("\t\t ERROR: Either SDI did not run, or PMIC RTC reading is not present in this SDI build", file=wdog_file)
            return
        
        if ocimem_load == 1:
            print("\n\t\t -> SDI RTC value[0x5774 in OCIMEM.BIN]: %u [hex: %#x] sec"%(SDI_PMIC_RTC, SDI_PMIC_RTC), file=wdog_file)
            crash_delta = SDI_PMIC_RTC-final_rtc
            print("\t\t -> Delta between last RPM PMIC pet and SDI run: %s [hex: %#x] sec"%(crash_delta,crash_delta), file=wdog_file)
        
        if print_RTC == 1:
            print("\n\t\t Using value from PMIC_RTC.bin", file=wdog_file)
            print("\t\t -> SDI RTC value[0x0 in PMIC_RTC.BIN]: %u [hex: %#x] sec"%(SDI_PMIC_RTC_v2, SDI_PMIC_RTC_v2), file=wdog_file)
            crash_delta = SDI_PMIC_RTC_v2-final_rtc
            print("\t\t -> Delta between last RPM PMIC pet and SDI run: %s [hex: %#x] sec"%(crash_delta,crash_delta), file=wdog_file)



