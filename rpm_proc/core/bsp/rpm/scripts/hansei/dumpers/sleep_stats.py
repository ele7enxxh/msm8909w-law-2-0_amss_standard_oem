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
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *

def dump(dump_path, memory, debug_info):
    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['rpm_core_dump'].vartype
    rpm_core_dump = decode_object('rpm_core_dump', address, dump_type, memory, debug_info)

    rpm = cast(rpm_core_dump.rpmserver_state, 'SystemData', memory, debug_info)
    
    if (check_member('sleep_stats', rpm_core_dump)):
        #sleep_stats in in core dump
        sleep = rpm_core_dump.sleep_stats
        sleep = cast(sleep, 'sleep_stats_type', memory, debug_info)
    else:
        try:
            #not in core dump, but elf is close enough to provide a correct pointer
            sleep_type = debug_info.variables['sleep_stats'].die
            sleep_address = debug_info.variables['sleep_stats'].address # FIXME: get right address by magic
            sleep = decode_object('sleep_stats', sleep_address, None, memory, debug_info, die=sleep_type)
        except:
            #not correct in elf, find absolute address (very hacky)
            sleep_type = debug_info.variables['sleep_stats'].die
            sleep_address = find_value(0x0019DBA0, 4, 0x00190000, 0x0019FFFF, memory)
            sleep = decode_object('sleep_stats', sleep_address, None, memory, debug_info, die=sleep_type)
    
    '''
    print(debug_info.variables['sleep_stats'].die)
    #sleep_address = 0x0019DBA0
    sleep_address = debug_info.variables['sleep_stats'].address
    sleep = decode_object('sleep_stats', sleep_address, sleep_type, memory, debug_info)
    '''

    #save_logger_level = update_logger(logging.DEBUG, logging)
    dump_sleep(dump_path, rpm, sleep, memory, debug_info)
    #update_logger(save_logger_level, logging)

def dump_sleep(dump_path, rpm, sleep, memory, debug_info):
    sleep_file_name = os.path.join(dump_path, 'sleep_stats.txt')
    with open(sleep_file_name, 'w') as sleep_file:
        
        print("\n ~~Sleep Stats Dump~~", file=sleep_file)
        print("\n\tXO Shutdown:", file=sleep_file)
        try:
            print('\t\tsleep_count:            %d' % sleep[0].count, file=sleep_file)
            print('\t\tlast_entered_at:        %s' % hex(sleep[0].last_entered_at), file=sleep_file)
            print('\t\tlast_exited_at:         %s' % hex(sleep[0].last_exited_at), file=sleep_file)
            print('\t\taccumulated_duration:   %s' % hex(sleep[0].accumulated_duration), file=sleep_file)
        except:
            print('\t\tERROR READING sleep_stats[0]', file=sleep_file)
        print("\n\tVDD Min:", file=sleep_file)
        try:
            print('\t\tsleep_count:            %d' % sleep[1].count, file=sleep_file)
            print('\t\tlast_entered_at:        %s' % hex(sleep[1].last_entered_at), file=sleep_file)
            print('\t\tlast_exited_at:         %s' % hex(sleep[1].last_exited_at), file=sleep_file)
            print('\t\taccumulated_duration:   %s' % hex(sleep[1].accumulated_duration), file=sleep_file)
        except:
            print('\t\tERROR READING sleep_stats[1]', file=sleep_file)
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            