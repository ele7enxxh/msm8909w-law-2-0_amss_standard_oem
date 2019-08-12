#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_tools/mcfg_factory/mcfg_factory.py#1 $
#* $DateTime: 2016/12/13 07:58:21 $
#*
#* 
#******************************************************************************
#*
#* Copyright (c) 2015 Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#******************************************************************************
import os
import hashlib
import argparse
import struct
from shutil import copy2

description = '''
MCFG Factory Tool
-----------------

This script generate the MCFG directory on EFS from MCFG mbns for factory use
'''

epilog = '''
Example Usage:
python  mcfg_factory.py --type mcfg_sw  --configs c:\\mcfg\\att\mcfg_sw.mbn@"AT&T",\\\\remote\\mcfg\\vzw\mcfg_sw.mbn@Verizon,Sprint\\mcfg_sw.mbn
'''


parser = argparse.ArgumentParser(description=description,
                                 formatter_class=argparse.RawTextHelpFormatter,
                                 epilog=epilog)
parser.add_argument('--type',
                    help='Choose hardware or software config',
                    choices=['mcfg_sw', 'mcfg_hw'],
                    required=True)
parser.add_argument('--configs', 
                    help='<mbn_path_1[@config_id1],mbn_path_2[@config_id2],...> ',
                    required=True)
parser.add_argument('--dir',
                    help='Output folder: Defaults to current working directory',
                    default=os.getcwd())

args = parser.parse_args()

base_dir_path = args.dir
config_type = args.type
configs = args.configs

def write_map_entry(fh, identifier):
    identifier_length = len(identifier)
    identifier_container_size = 124  # bytes
    identifier_container_padding = identifier_container_size - identifier_length

    fh.write('gFcM')
    fh.write(struct.pack('<I', identifier_length))  # little endian 4 byte unsigned int == <I
    fh.write(identifier)
    fh.write('\x00' * identifier_container_padding)

if not os.path.isdir(base_dir_path):
    raise IOError('"%s" is not a valid base directory!' % base_dir_path)

# Create path to cfg_00/cfg_01 dir
nvm_dir_path = os.path.join(base_dir_path, 'nvm')
mcfg_dir_path = os.path.join(nvm_dir_path, 'mcfg')
if config_type == 'mcfg_hw':
    config_dir_path = os.path.join(mcfg_dir_path, 'cfg_00')
else:  # must be 'mcfg_sw'
    config_dir_path = os.path.join(mcfg_dir_path, 'cfg_01')

# Build path to config dir if necessary
if not os.path.isdir(config_dir_path):
    os.makedirs(config_dir_path)

map_path = os.path.join(config_dir_path, 'map')

configs = configs.split(',')  # configs delimited by commas

if os.listdir(config_dir_path) == []:  # if config dir is empty
    map_fh = open(map_path, 'wb') # Creates binary file for writing if it doesn't exist, overwrites if it does
    write_map_entry(map_fh, '')  # empty active id
    write_map_entry(map_fh, '')  # empty pending id
    for config in configs:
        config = config.split('@')  # ['mcfg_sw.mbn', 'c43ebaf48b67bf418d9fb55e6fdad3e4'] or ['mcfg_sw.mbn']
        mbn_src_path = config[0]
        if not os.path.isfile(mbn_src_path):
            raise IOError('"%s" is not a valid src path for an mbn!' % mbn_src_path)

        config_id = config[1] if len(config) > 1 else None

        if not config_id:
            fh = open(mbn_src_path, 'rb')
            mbn_contents = fh.read()
            config_id = hashlib.sha1(mbn_contents).digest()
            fh.close()
        filename_id = hashlib.md5(config_id).hexdigest().upper()
        
        mbn_filename = '%s%s' % (config_type, filename_id)
        mbn_dst_path = os.path.join(config_dir_path, mbn_filename)
        

        copy2(mbn_src_path, mbn_dst_path)
        print('Copied "%s" to "%s"' % (mbn_src_path, mbn_dst_path))
        write_map_entry(map_fh, config_id)
    write_map_entry(map_fh, '')  # empty terminator id
    print 'Map file written'
    map_fh.close()
else:
    print '%s is not empty. Please clean up first.' % config_dir_path


