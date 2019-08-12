#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


import argparse
import glob
import itertools
import sys
import os
import logging
from hansei_utils import print_p

dump_file_types = [
    #
    # Core stuff, most important.
    #
    {   # The place all of the code lives
        'name'     : {'rpm_code_ram.bin', 'CODERAM.BIN', 'RPM0.bin'},
        'ext'      : {'.BIN', '.LST'},
        'basename' : {'RPM_CODE_RAM', 'CODERAM', 'RPM0'},
        'required' : True,
        'base'     : {
                        'badger' : 0x100000,
                        'bear'   : 0x000000,
                        'honey'  : 0x000000,
                     },
    },
    {   # The place all of the data lives
        'name'     : {'rpm_data_ram.bin', 'DATARAM.BIN', 'RPM1.bin'},
        'ext'      : {'.BIN', '.LST'},
        'basename' : {'RPM_DATA_RAM', 'DATARAM', 'RPM1'},
        'required' : True,
        'base'     : {
                        'badger' : 0x190000,
                        'bear'   : 0x090000,
                        'honey'  : 0x090000,
                     },
    },
    {   # The place all of IPC messages live
        'name'     : {'rpm_msg_ram.bin', 'MSGRAM.BIN', 'RPM2.bin'},
        'ext'      : {'.BIN', '.LST'},
        'basename' : {'RPM_MSG_RAM', 'MSGRAM', 'RPM2'},
        'required' : True, # really?  maybe not strictly required
        'base'     : {
                        'badger' : 0xFC428000,
                        'bear'   : 0x60060000,
                        'honey'  : 0x60068000,
                     },
    },
    #{   # The values of RPM registers. Only consulted if there was no core dump.
    #    'name'     : 'rpm_registers.cmm',
    #    # FIXME: need some parser here to make this file useful...
    #},

    #
    # Generic RPM peripherals, less important
    #
    {   # RPM's generic Cortex-M3 hardware.
        'name'     : 'rpm_scs.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0xE000E000,
                        'bear'   : 0xE000E000,
                        'honey'  : 0xE000E000,
                     },
    },
    {   # RPM's Qualcomm-written hardware.
        'name'     : 'rpm__dec.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0x180000,
                        'bear'   : 0x080000,
                        'honey'  : 0x080000,
                     },
    },
    {   # RPM's timer-related hardware (next 3)
        'name'     : 'rpm__qtmr_ac.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0x182000,
                        'bear'   : 0x082000,
                        'honey'  : 0x082000,
                     },
    },
    {
        'name'     : 'rpm__qtmr_f0.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0x183000,
                        'bear'   : 0x083000,
                        'honey'  : 0x083000,
                     },
    },
    {
        'name'     : 'rpm__qtmr_f1.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0x184000,
                        'bear'   : 0x084000,
                        'honey'  : 0x084000,
                     },
    },
    {   # RPM data RAM access control unit.
        'name'     : 'rpm__mpu.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0x186000,
                        'bear'   : -1, #does not exist on bear
                        'honey'  : 0x289000,
                     },
    },
    {   # RPM code RAM access control unit.
        'name'     : 'rpm__apu.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0x187000,
                        'bear'   : 0x087000,
                        'honey'  : 0x087000,
                     },
    },
    {   # RPM message RAM access control unit.
        'name'     : 'rpm__rpu.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0xFC420000,
                        'bear'   : -1, #does not exist on bear
                        'honey'  : -1,
                     },
    },
    #
    # System peripherals, vaguely RPM related.
    #
    {   # The bits of the MPM that control low power states.
        'name'     : 'mpm.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0xFC4A0000,
                        'bear'   : 0x604A0000,
                        'honey'  : 0x604A0000,
                     },
    },
    {   # The global clock controller.
        'name'     : 'clk_ctl.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0xFC400000,
                        'bear'   : 0x61800000, #not captured for bear
                        'honey'  : 0x00300000,
                     },
    },
    {   # Some parts of the TCSR.
        'name'     : 'tcsr_regs.bin',
        'ext'      : {''},
        'basename' : {''},
        'base'     : {
                        'badger' : 0xFD4A1300,
                        'bear'   : 0x61937300,
                        'honey'  : 0x007A0000,
                     },
    },
]

known_targets = {
    '8974' : 'badger',
    '9x25' : 'badger',
    '8x10' : 'badger',
    '8x26' : 'badger',
    '8084' : 'badger',
    '9x35' : 'badger',
    '8994' : 'badger',
    '8996' : 'honey',
    '8916' : 'bear',
    '8936' : 'bear',
    '8939' : 'bear',
    '9x45' : 'bear',
    '8909' : 'bear',
    '9x07' : 'bear',
}

known_families = {
    'badger'        : 'badger',
    'b'             : 'badger',
    'bear'          : 'bear',
    'bagheera'      : 'bear',
    'h'             : 'honey',
    'honeybadger'   : 'honey',
    'honey'         : 'honey',
}

'''
Parse arguments
'''

parser = argparse.ArgumentParser(description='Generate introspective RPM logs.')
parser.add_argument('files', metavar='dumpfile', nargs='+', type=str, help='dumped ram files (*.bin, *.lst) [required]')
parser.add_argument('--elf', metavar='rpm.elf', default='', help='.elf with (at least closely) matching symbols. Defaults to elf in current build [required]')
parser.add_argument('--output', '-o', metavar='logs_path', default='./output', help='location to place output logs [optional]')
parser.add_argument('--verbose', '-v', action='count', default=0, help='verbosity (more v\'s == more messages) [optional]')
parser.add_argument('--parser', '-p', metavar='parse_path', default='', help='absolute path to folder containing rpm_log_bfam.py [optional]')
parser.add_argument('--parser_rel', '-r', action='count', default=0, help='notes that the path given for the --parser argument is a relative path to folder containing rpm_log_bfam.py from folder containing hansei.py [optional]')
parser.add_argument('--target', '-t', metavar='target', default='', help='Target dumps are being parsed for. ex 8974 [optional]')
parser.add_argument('--family', '-f', metavar='family', default='', help='Target family dumps are being parsed for. May be one of (b)adger, bear/bagheera, or ((h)oney)badger) [optional]')
parser.add_argument('--fill_char', metavar='fill_char', default='space', help='fill character for mpm dump, avaliable options are: underscore, dot, space, zero, or any single character [optional]')

args = parser.parse_args(sys.argv[1:])

target_family = known_families.get(args.family.lower(), "UNKNOWN FAMILY - Assuming Bear")
target_family = known_targets.get(args.target, target_family)
target_target = known_targets.get(args.target, '????')
if target_target != '????':
    target_target = args.target


print_p("",fill="-")                             # "#------------------------------------------------------------------------------#"
print_p("")                                      # "#                                                                              #"
print_p("Welcome to RPM Hansei",format="center") # "#                            Welcome to RPM Hansei                             #"
print_p("")                                      # "#                                                                              #"
print_p("",fill="-")                             # "#------------------------------------------------------------------------------#"
print_p("\tParsing dumps for family: %s"%target_family)
if target_family == "UNKNOWN FAMILY - Assuming Bear":
    target_family = 'bear'
print_p("",fill="-")                             # "#------------------------------------------------------------------------------#"
print_p("...Checking for required tools:")
try:
    error_msg = '\tERROR: Failed to find pyelftools. Please ensure you have this package.    '
    from elftools.elf.elffile import ELFFile
    print_p('\tpyelftools... found')
except:
    print_p(error_msg)
    print_p("",fill="-")                         # "#------------------------------------------------------------------------------#"
    exit(1)
print_p("",fill="-")                             # "#------------------------------------------------------------------------------#"

from memory import Memory
from dwarf import DebugInfo, decode_object


temp_files = [f if not os.path.isdir(f) else os.path.join(f, '*.bin') for f in args.files] # if a directory is specified, look for dumps *in* it
#hack for when .lst files are given
args.files = temp_files + [f if not os.path.isdir(f) else os.path.join(f, '*.lst') for f in args.files]
args.files = set(itertools.chain(*[glob.glob(f) for f in args.files]))

if not os.path.exists(args.output):
    os.makedirs(args.output)

verbosity = [logging.ERROR, logging.WARNING, logging.INFO, logging.DEBUG]
logging.basicConfig(format='%(levelname)-8s | %(name)-20s | %(message)s', level=verbosity[min(len(verbosity)-1, args.verbose)])
logger = logging.getLogger(__name__)

print_p("\tReading dump files...")
required = {dft['base'][target_family] for dft in dump_file_types if dft.get('required')}
system = Memory()
read_bases = {-1}
for dumpfile in args.files:
    dfn = os.path.basename(dumpfile)
    dfs = os.path.getsize(dumpfile)
    dfe = os.path.splitext(dumpfile)[1]
    dfne = os.path.basename(os.path.splitext(dumpfile)[0])
    #import pdb; pdb.set_trace()
    
    for dft in dump_file_types:
        #try for known names
        if dfn in dft['name']:
            logger.debug("Reading %s into 0x%0.8x" % (dumpfile, dft['base'][target_family]))
            with open(dumpfile, 'rb') as data:
                if dft['base'][target_family] not in read_bases:
                    read_bases.add(dft['base'][target_family])
                    system.load_hunk(dft['base'][target_family], data.read())
                    required -= {dft['base'][target_family]}
                    
        # try for combinations of known base names and extensions
        elif (dfe.upper() in dft['ext']) and (dfne.upper() in dft['basename']):
            logger.debug("Reading %s into 0x%0.8x" % (dumpfile, dft['base'][target_family]))
            with open(dumpfile, 'rb') as data:
                if dft['base'][target_family] not in read_bases:
                    read_bases.add(dft['base'][target_family])
                    system.load_hunk(dft['base'][target_family], data.read())
                    required -= {dft['base'][target_family]}

if required:
    print_p("ERROR FATAL: Did not find all required RAM dumps!")
    print "\t(Missing dumps for regions starting at [%s].)" % (' '.join(hex(base) for base in required))
    print_p("ABORTING")
    print('#------------------------------------------------------------------------------#')
    sys.exit(-1)
    
print('#------------------------------------------------------------------------------#')
'''
Find and read ELF file
'''
if args.elf == '':
    elf_dir = os.path.realpath(__file__).split('rpm_proc')[0]+'rpm_proc\\core\\bsp\\rpm\\build'
    elf_file = '';
    for file in os.listdir(elf_dir):
        if file.endswith(".elf"):
            elf_file = file
    if elf_file == '':
        args.elf = elf_dir+"\\"+elf_file

if args.elf == '':
    print_p("ERROR FATAL: No .elf file given and no .elf file found in current build... Aborting")
    print('#------------------------------------------------------------------------------#')
    sys.exit(-1)

print_p('\tReading ELF...')
di = DebugInfo(args.elf)

print('#------------------------------------------------------------------------------#')
'''
Parse dumps
'''
print_p('\tDumping summary...')
import dumpers.summary as summary
[temp_target, branch] = summary.dump(args.output, system, di, dump_file_types[1]['base'][target_family])
if temp_target != target_target:
    if temp_target != '8974 Pro' and target_target != '????':
        print_p('\t\t...WARNING!!! TARGET MISMATCH, SPECIFIED TARGET %s'%target_target)
    target_target = temp_target
print_p('\t\t...Discovered target: %s'%target_target)

'''
Find ulog parsers
'''
if (args.parser != '' and args.parser_rel != 0):
    args.parser = os.path.realpath(__file__).split(os.path.basename(__file__))[0]+args.parser
if (args.parser == ''):
    args.parser = os.path.realpath(__file__).split('rpm_proc')[0]+'rpm_proc/core/power/rpm/debug/scripts/'
    #try server location if build path doesn't work
    if (os.path.isfile(args.parser+'rpm_log_bfam.py')==False):
        args.parser=os.path.realpath(__file__).split(os.path.basename(__file__))[0]+'../bfam_log_parser/'
    if (os.path.isfile(args.parser+'rpm_log_bfam.py')==False):
        print_p('\t\t-Failed to find ulog parser, please specifiy location with --parser(-p).')
        print_p('\t\t\tFor path relative to location of hansei.py, also include -r.')
if (args.parser[len(args.parser)-1] != '/'):
    args.parser = args.parser+'/'

print_p('\tDumping ulogs...')
import dumpers.rpm_ulog as rpm_ulog
rpm_ulog.dump(args.output, args.parser, system, di, target_target, branch)

print_p('\tDumping NPA state...')
import dumpers.npa as npa
npa.dump(args.output, system, di)

print_p('\tDumping railway state...')
import dumpers.railway as railway
railway.dump(args.output, system, di)

print_p('\tDumping mpm registers...')
import dumpers.mpm as mpm
mpm.dump(args.output, system, di, args.fill_char)

print_p('\tDumping sleep stats...')
import dumpers.sleep_stats as sleep_stats
sleep_stats.dump(args.output, system, di)

print_p('\tDumping RPM server state...')
import dumpers.rpmserver as rpmserver
rpmserver.dump(args.output, system, di, target_family)

#print_p('\tDumping rpbcr state...')
#import dumpers.rbcpr as rbcpr
#rbcpr.dump(args.output, system, di)

print_p('\tDumping watchdog events...')

#this is really just a hack to get SDI data loaded for PMIC wdog stuff
ocimem_load = 0
pmic_rtc_load = 0
if target_family == 'badger':
    for dumpfile in args.files:
        dfn = os.path.basename(dumpfile)
        #import pdb; pdb.set_trace()
        
        if dfn in ['OCIMEM.BIN','ocimem.bin']:
            print_p('\t\tAttemping to load OCIMEM at 0x0...')
            logger.debug("Reading %s into 0x%0.8x" % (dumpfile, 0x0))
            with open(dumpfile, 'rb') as data:
                system.load_hunk(0x0, data.read())
            print_p('\t\t...done')
            ocimem_load = 1
        if dfn in ['PMIC_RTC.BIN']:
            print_p('\t\tAttemping to load PMIC_RTC at 0x0...')
            logger.debug("Reading %s into 0x%0.8x" % (dumpfile, 0x0))
            with open(dumpfile, 'rb') as data:
                system.overwrite(0x0, data.read())
            print_p('\t\t...done')
            ocimem_load = 1

import dumpers.wdogs as wdogs
wdogs.dump(args.output, system, di, ocimem_load, pmic_rtc_load)

print('#------------------------------------------------------------------------------#')
print_p('\tHansei finished')
print('#------------------------------------------------------------------------------#')
print("Files/directories used during processing:")
print "\tElf: "+args.elf
print "\tUlog parser Directory: "+args.parser
print "\tOutput Directory: %s"%args.output

