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

rbcpr_rail_list_1 = {
    0 : 'VDD_CX',
}

#for 8909 and 9x45
rbcpr_rail_list_2 = {
    0 : 'VDD_MX',
    1 : 'VDD_CX'
}

corner_list_3 = {
	0: 'SVS_SOC',
	1: 'NOMINAL',
	2: 'SUPER_TURBO'
}

#8909
corner_list_4 = {
	0: 'SVS_SOC',
	1: 'SVS_HIGH',
	2: 'NOMINAL',
	3: 'TURBO'
}

#9x07
corner_list_5 = {
	0: 'SVS_SOC',
	1: 'SVS_HIGH',
	2: 'NOMINAL',
	3: 'NOMINAL_HIGH',
	4: 'TURBO'
}

def dump(dump_path, memory, debug_info):
    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['rpm_core_dump'].vartype
    rpm_core_dump = decode_object('rpm_core_dump', address, dump_type, memory, debug_info)
    dev_num = rpm_core_dump.hw_version.parts.device_number

    rpm = cast(rpm_core_dump.rpmserver_state, 'SystemData', memory, debug_info)

    if 'rpm_image_section_array' not in debug_info.variables:
        print_p ('\t rpm_image_section_array not found in debug')

    else:
        try:
            rbcpr_type = debug_info.variables['rbcpr_stats_header'].vartype
            rbcpr_address = debug_info.variables['rpm_image_section_array'].address 
            rbcpr_address = rbcpr_address + 24
            rbcpr_stats = decode_object('rbcpr_stats', rbcpr_address, rbcpr_type, memory, debug_info)
        except:
            rbcpr_die = debug_info.variables['rbcpr_stats_header'].die
            rbcpr_address = debug_info.variables['rpm_image_section_array'].address 
            rbcpr_address = rbcpr_address + 24
            rbcpr_stats = decode_object('rbcpr_stats', rbcpr_address, None, memory, debug_info, die=rbcpr_die)
    

    dump_rbcpr(dump_path, rpm, rbcpr_stats, memory, debug_info,dev_num)


def dump_rbcpr(dump_path, rpm, rbcpr_stats, memory, debug_info,dev_num):
    rbcpr_file_name = os.path.join(dump_path, 'rbcpr.txt')

    rail_list_name =  rbcpr_rail_list_1
    corner_list_name = corner_list_3

    if dev_num==2:
        #9x45
        rail_list_name = rbcpr_rail_list_2
        corner_list_name = corner_list_3
    elif dev_num==3:
        #8909
        rail_list_name = rbcpr_rail_list_2
        corner_list_name = corner_list_4
    
    with open(rbcpr_file_name, 'w') as rbcpr_file:
    
        num_rails = rbcpr_stats.count
        status = rbcpr_stats.status

        print("\n RBCPR STATS (num_rails = %d)(status = %d)" %( num_rails, status), file=rbcpr_file)

        header = rbcpr_stats
        header.address = header.address + header._get_size(header._elemtype) 
        header.pointer = header.address

        header = cast (header, 'rbcpr_rail_stats_header_type', memory, debug_info)

        for num in range(num_rails):
            num_corners = header.num_corners
            num_latest_recommends = header.num_latest_recommends 

            print("\n\t %s Rail Data (num_corners = %d)(num_latest_recommends = %d)" %( rail_list_name[num], num_corners, num_latest_recommends), file=rbcpr_file)

            corner = header
            header_die = debug_info.types['rbcpr_rail_stats_header_type']
            corner.address = corner.address + corner._get_size(header_die)
            corner.pointer = corner.address

            corner = cast (corner, 'rbcpr_stats_corner_type', memory, debug_info)

            for cor_num in range(num_corners):

                print("\t\t Corner Data: %s (efuse_adjustment: %d)(programmed_voltage: %d)(isr_count: %d)(min_count: %d)(max_count: %d)" %( corner_list_name[cor_num],corner.fuse_microvolts, corner.programmed_microvolts, corner.isr_counter, corner.min_counter, corner.max_counter), file=rbcpr_file)
                print("\t\t\t Latest Recommends:", file=rbcpr_file) 

                for rec_num in range(num_latest_recommends):
                    print("\t\t\t\t (microvolts: %d)  (timestamp: 0x%x)" %(corner.recommended[rec_num].microvolts, corner.recommended[rec_num].timestamp), file=rbcpr_file)


                corner_die = debug_info.types['rbcpr_stats_corner_type']
                corner.address = corner.address + corner._get_size(corner_die)
                corner.pointer = corner.address 

            # when we exit the loop the corner would already be pointing to the footer starting address
            footer = cast(corner, 'rbcpr_rail_stats_footer_type', memory, debug_info)

            print("\n\t\t (current_corner: %s)(railway_voltage: %d)(off_corner: %d)(margin: %d)" % (footer.current_corner, footer.railway_microvolts,footer.off_corner,footer.margin), file=rbcpr_file)

            #change the header to point to the next header entry
            header = footer
            footer_die = debug_info.types['rbcpr_rail_stats_footer_type']
            header.address = header.address + header._get_size(footer_die)
            header.pointer = header.address
            header = cast (header, 'rbcpr_rail_stats_header_type', memory, debug_info)

