'''
==============================================================================

                M O D E M   C O N F I G   V I E W I N G

GENERAL DESCRIPTION
 
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/
'''

import sys
import os
import xml.etree.ElementTree as ET

# Python 2.7+ only standard lib modules
import argparse
from collections import OrderedDict


description = '''
MBN Viewing Tool
--------------------

This tool will parse all carrier configs in a PL and display them
by NV number or EFS filename.

Important: Script must be at mcfg/mcfg_tools/mcfg_mbn_viewer (create if it doesn't already exist).

Python 2.7+ (uses OrderedDict and argparse)
'''

EFS_ITEM_DATA = 'NvEfsItemData'
EFS_FILE = 'NvEfsFile'
NV_ITEM_DATA = 'NvItemData'

def get_xml_root():
    'Returns the path to mcfg/mcfg_gen/generic'

    #For slickedit execution
    # os.chdir("C:\\MBNViewer\\mcfg.mpss.4.6.ln_MBNViewer_workspace\\modem_proc\\mcfg\\mcfg_test\\mcfg_mbn_viewer")

    code_folder = os.path.split(os.path.abspath(__file__))[0]
    mcfg_test, mcfg_mbn_viewer = os.path.split(code_folder) # truncate 'mcfg_mbn_viewer' from path
    mcfg_folder, mcfg_test = os.path.split(mcfg_test)

    # in the future, add support for HW MBNs
    xml_root = os.path.join(mcfg_folder, 'mcfg_gen')
    return os.path.join(xml_root, 'generic')

def hex_to_byte(hexStr):
    """
    Convert a string hex byte values into a byte string. The Hex Byte values may
    or may not be space separated.
    """

    bytes = []

    hexStr = ''.join( hexStr.split(" ") )

    for i in range(0, len(hexStr), 2):
        bytes.append( chr( int (hexStr[i:i+2], 16 ) ) )

    return ''.join( bytes )


def parse_xml(file_path):
    '''
    Input: Path to an xml file
    Output: Parsed nv/efs/trl in dictionary format
    '''
    tree = ET.parse(file_path)
    root = tree.getroot()
    parsed = {
        'efs': [],
        'nv': []
    }
    for child in root:
        if child.tag == NV_ITEM_DATA:
            nv_number = child.attrib.get('id')
            parsed['nv'].append(nv_number)
        elif child.tag == EFS_ITEM_DATA:
            efs_target_path = child.attrib.get('fullpathname')
            parsed['efs'].append(efs_target_path)

        elif child.tag == EFS_FILE:  # this includes Data Profiles
            efs_target_path = child.attrib.get('targetPath')
            parsed['efs'].append(efs_target_path)

    trl = root.find('NvTrlRecord')
    for member in trl:
        base_10_list = member.text.strip().split(' ')
        if len(base_10_list) > 1:
            base_16_list = [hex(int(char))[2:] for char in base_10_list]
            base_16_string = ' '.join(base_16_list)
            parsed['carrier_name'] = hex_to_byte(base_16_string)
            break  # only the first member with space delimited items is carrier name
            

    return parsed

class MbnItemManager(object):
    "Helper class for creating consolidated NV/EFS lists"
    def __init__(self):
        self.item_list = OrderedDict()
        self.item_list['nv'] = OrderedDict()  # Separate structures because nv should be printed before efs
        self.item_list['efs'] = OrderedDict()

    def __getitem__(self, i):  # so you can use instances of this class as a dictionary['key']
        return self.item_list[i]

    def __iter__(self):  # so you can iterate over instances of this class
        return self.item_list.itervalues()

    # intentional redundancy for clarity
    def update_efs_item(self, identifier, source):
        'Adds the efs item into structured memory'
        if identifier not in self.item_list['efs']: # first time the item has been encountered
            self.item_list['efs'][identifier] = [source]
        else: 
            self.item_list['efs'][identifier].append(source)

    def update_nv_item(self, identifier, source):
        'Adds the nv item into structured memory'
        if identifier not in self.item_list['nv']: # first time the item has been encountered
            self.item_list['nv'][identifier] = [source]
        else: 
            self.item_list['nv'][identifier].append(source)


def get_items_in_pl(xml_root, sw_only=False, hw_only=False):
    """Input:
          path to xml root directory,
          sw_only=True to skip hardware configs,
          hw_only=True to skip software configs
       Output: Populated instance of MbnItemManager

    """
    mbn_item_list = MbnItemManager()
    for dirpath, dirnames, filenames in os.walk(xml_root):
        # dirpath is a string, the path to the directory
        # dirnames is a list of the names of the subdirectories in dirpath
        # filenames is a list of the names of the non-directory files in dirpath

        for file_name in filenames:
            if '.xml' not in file_name:  # only want xml files
                continue

            if sw_only and 'hw' in file_name:
                continue  # skipping over hardware xml
            if hw_only and 'sw' in file_name:
                continue  # skipping over software xml

            #print file_name
            file_path = os.path.join(dirpath, file_name)
            parsed_xml = parse_xml(file_path)

            for item_identifier in parsed_xml['nv']:
                mbn_item_list.update_nv_item(item_identifier, parsed_xml['carrier_name'])

            for item_identifier in parsed_xml['efs']:  # reduncancy necessary for ordered printing later
                mbn_item_list.update_efs_item(item_identifier, parsed_xml['carrier_name'])

    return mbn_item_list

def print_items_in_pl(mbn_item_list, display_item_sources):
    '''Prints all NV/EFS items in the PL
    '''
    nv_item_count = 0
    for identifier in mbn_item_list['nv']:  # print NV items first
        nv_item_count += 1
        print identifier.zfill(5)  # fill with zeros if necessary - ex: NV 00850
        if display_item_sources:  # verboses
            for carrier in mbn_item_list['nv'][identifier]:
                print '\t- %s' % carrier
            print ' '
    print '%d NV items are present in this PL.' % nv_item_count
    print ' '

    efs_item_count = 0
    for identifier in sorted(mbn_item_list['efs'].iterkeys()): # then print EFS items, in alphabetical order
        efs_item_count += 1
        if not identifier:
            # GNSS SUpL Version missing target path in spreadsheet
            # For all CT test configs - results in formatting issues with script
            print "Target path may be missing for EFS items in the following carriers: (use verbose switch)"
        print identifier
        if display_item_sources:
            for carrier in mbn_item_list['efs'][identifier]:
                print '\t- %s' % carrier
            print ' '
    print '%d EFS items are present in this PL.' % efs_item_count
    print ' '
    
    print '%d total NV/EFS items are present in this PL.' % (nv_item_count + efs_item_count)


def print_duplicates(nv_duplicates, identifier):
    for occurrence in nv_duplicates:
        print occurrence[identifier]
        print len(occurrence[identifier]) * '-'
        print '  Occurrences in software'
        for sw_occurrence in occurrence['sw_occurrences']:
            print '    * ' + sw_occurrence

        print '  Occurrences in hardware'
        for hw_occurrence in occurrence['hw_occurrences']:
            print '    * ' + hw_occurrence
        print ' '

epilog = '''
Example Usage:

In all of these examples, you can use the verbose switch to get more info about where a specific nv/efs item is present.

1) python mcfg_mbn_viewer.py
This would ask you for input on what you want to do. 

2) python mcfg_mbn_viewer.py --find_duplicates
This would list all of the duplicate NV/EFS items that are present at least once in both HW and SW configs.

3) python mcfg_mbn_viewer.py --hw_only --verbose
This would list all of the nv and efs items that are present in at least one of the hardware configs (not sw).
The verbose flag lists the contents of the items you want to see.

'''


parser = argparse.ArgumentParser(description=description,
                                 formatter_class=argparse.RawTextHelpFormatter,
                                 epilog=epilog)
parser.add_argument('--hw_only', 
                   action='store_true',
                    help='Only list hardware items')
parser.add_argument('--sw_only', 
                   action='store_true',
                    help='Only list software items')
parser.add_argument('--all', 
                   action='store_true',
                    help='List all nv/efs items')
parser.add_argument('--find_duplicates',
                    action='store_true',
                    help='Find duplicate items that are in both sw and hw PLs. Supersedes other switches')
parser.add_argument('--verbose',
                    help='List xml file sources of nv/efs items',
                    action='store_true')

if __name__ == '__main__':
    args = parser.parse_args()

    xml_root = get_xml_root()

    if not args.all and not args.sw_only and not args.hw_only and not args.find_duplicates:
        print '1: List all SW items'
        print '2: List all HW items'
        print '3: List HW/SW duplicates'
        print '4: List all items'
        response = raw_input('> ')
        if response == '1':
            args.sw_only = True
        elif response == '2':
            args.hw_only = True
        elif response == '3':
            args.find_duplicates = True
        elif response != '4':
            print 'Invalid response'
            exit(1)


    if args.find_duplicates:
        print 'Listing all duplicates'
        hw_item_list = get_items_in_pl(xml_root, hw_only=True)
        sw_item_list = get_items_in_pl(xml_root, sw_only=True)

        nv_duplicates = []
        efs_duplicates = []

        for hw_nv_number in hw_item_list['nv']:
            occurrences = {
                'nv_number': hw_nv_number,
                'sw_occurrences': [],
                'hw_occurrences': hw_item_list['nv'][hw_nv_number]
            }

            for sw_nv_number in sw_item_list['nv']:
                if hw_nv_number == sw_nv_number:
                    carrier_list = sw_item_list['nv'][sw_nv_number]
                    occurrences['sw_occurrences'] = carrier_list
            if occurrences['sw_occurrences'] and occurrences['hw_occurrences']:
                nv_duplicates.append(occurrences)


        for hw_efs_file in hw_item_list['efs']:
            occurrences = {
                'efs_file': hw_efs_file,
                'sw_occurrences': [],
                'hw_occurrences': hw_item_list['efs'][hw_efs_file]
            }

            for sw_efs_file in sw_item_list['efs']:
                if hw_efs_file == sw_efs_file:
                    carrier_list = sw_item_list['efs'][sw_efs_file]
                    occurrences['sw_occurrences'] = carrier_list
            if occurrences['sw_occurrences'] and occurrences['hw_occurrences']:
                efs_duplicates.append(occurrences)


        if nv_duplicates:
            print 'NV ITEM DUPLICATES FOUND'
            print '========================' + '\n'
            print_duplicates(nv_duplicates, 'nv_number')
        else:
            print 'No NV Item duplicates found'

        if efs_duplicates:
            print 'EFS FILE DUPLICATES FOUND'
            print '=========================' + '\n'
            print_duplicates(efs_duplicates, 'efs_file')
        else:
            print 'No EFS File duplicates found'
    else:
        if args.all:
            args.sw_only = False
            args.hw_only = False
        if args.sw_only:
            print 'Listing only software items:'.upper()
        if args.hw_only:
            print 'Listing only hardware items'.upper()
        if not args.sw_only and not args.hw_only:
            print 'Listing all items'.upper()
        mbn_item_list = get_items_in_pl(xml_root, args.sw_only, args.hw_only)
        print_items_in_pl(mbn_item_list, args.verbose)

