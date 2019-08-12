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
from hansei_utils import *

def dump(dump_path, memory, debug_info):
    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['rpm_core_dump'].vartype
    rpm_core_dump = decode_object('rpm_core_dump', address, dump_type, memory, debug_info)

    #import pdb; pdb.set_trace()
    rpm = cast(rpm_core_dump.rpmserver_state, 'SystemData', memory, debug_info)

    #save_logger_level = logging.getLogger('dwarf').getEffectiveLevel()
    #print(save_logger_level)
    #logging.getLogger('dwarf').setLevel(logging.DEBUG)
    #print(logging.getLogger('dwarf').getEffectiveLevel())
    dump_railway(dump_path, rpm, memory, debug_info)
    #logging.getLogger('dwarf').setLevel(save_logger_level)

def dump_railway(dump_path, rpm, memory, debug_info):
    ee_status_file_name = os.path.join(dump_path, 'railway.txt')
    
    rail_config_data_type = debug_info.variables['RAILWAY_CONFIG_DATA'].die
    rail_config_data_address = debug_info.variables['RAILWAY_CONFIG_DATA'].address
    rail_config_data = decode_object('rail_config_data', rail_config_data_address, None, memory, debug_info, die=rail_config_data_type)
    
    num_rails = rail_config_data.num_rails
    
    with open(ee_status_file_name, 'w') as ee_status_file:
        railway_type = debug_info.variables['railway'].die
        railway_address = debug_info.variables['railway'].address
        railway = decode_object('railway', railway_address, None, memory, debug_info, die=railway_type)
        
        for rail_num in range(num_rails):
            try:
                rail = railway.rail_state[rail_num]
                rail_name = pointer_to_str(rail_config_data.rails[rail_num].vreg_name)
                print('\n\n ~~ RAIL %s ~~' % rail_name, file=ee_status_file)
                
                print('\tCURRENT ACTIVE:', file=ee_status_file)
                print('\t\tmode:                 %s' % rail.current_active.mode, file=ee_status_file)
                print('\t\tmicrovolts:           %d' % rail.current_active.microvolts, file=ee_status_file)
                if check_member("off_corner", rail.current_active):
                    print('\t\toff_corner:           %s' % rail.current_active.off_corner, file=ee_status_file)
                else:
                    print('\t\toff_corner:           value not present', file=ee_status_file)
                
                print('\tUNCONSTRAINED TARGET:', file=ee_status_file)
                print('\t\tmode:                 %s' % rail.unconstrained_target.mode, file=ee_status_file)
                print('\t\tmicrovolts:           %d' % rail.unconstrained_target.microvolts, file=ee_status_file)
                if check_member("off_corner", rail.unconstrained_target):
                    print('\t\toff_corner:           %s' % rail.unconstrained_target.off_corner, file=ee_status_file)
                else:
                    print('\t\toff_corner:           value not present', file=ee_status_file)
                
                print('\tCONSTRAINED TARGET:', file=ee_status_file)
                print('\t\tmode:                 %s' % rail.constrained_target.mode, file=ee_status_file)
                print('\t\tmicrovolts:           %d' % rail.constrained_target.microvolts, file=ee_status_file)
                if check_member("off_corner", rail.constrained_target):
                    print('\t\toff_corner:           %s' % rail.constrained_target.off_corner, file=ee_status_file)
                else:
                    print('\t\toff_corner:           value not present', file=ee_status_file)
                
                print('\tVOTER LIST:', file=ee_status_file)
                voter = rail.voter_list_head
                voter = cast(voter, 'railway_voter_s', memory, debug_info)
                tabs = "\t\t"
                while(voter.address != 0):
                    try:
                        print('%svoltage_corner:          %s' % (tabs, voter.voltage_corner), file=ee_status_file)
                        print('%sactive_floor:            %s' % (tabs, voter.active_floor), file=ee_status_file)
                        print('%sexplicite_voltage_in_uv: %d' % (tabs, voter.explicit_voltage_in_uv), file=ee_status_file)
                        print('%ssuppressible:            %s' % (tabs, voter.suppressible), file=ee_status_file)
                        print('%sid:                      %s' % (tabs, voter.id), file=ee_status_file)
                        print('%srail:                    %d' % (tabs, voter.rail), file=ee_status_file)
                        print('%ssw_enable:               %s' % (tabs, voter.sw_enable), file=ee_status_file)
                        print('%s-------------' % tabs, file=ee_status_file)
                        voter = voter.voter_link
                        voter = cast(voter, 'railway_voter_s', memory, debug_info)
                        tabs+="\t"
                    except:
                        break
            except:
                print('\t Failed to parse current state of rail #%d' % rail_num, file=ee_status_file)