# --------------------------------------------------------------------------- #
# Description   ::  Defines the SCons Builder for MSGR Parser
# --------------------------------------------------------------------------- #
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tools/build/msgr_builder.py#1 $
# --------------------------------------------------------------------------- #
# Copyright (c) 2014 Qualcomm Technologies Incorporated.
#
# All Rights Reserved. Qualcomm Confidential and Proprietary
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
# --------------------------------------------------------------------------- #

# --------------------------------------------------------------------------- #
#                       IMPORT MODULES & DEFINITONS                           #
# --------------------------------------------------------------------------- #
import os
import types
import sys
import re
import json
import math
import pprint
from datetime import date
from collections import defaultdict
pp = pprint.PrettyPrinter()

# --------------------------------------------------------------------------- #
#                       GLOBAL VARIABLE DEFINITIONS                           #
# --------------------------------------------------------------------------- #

# Verbosity is intended for stand alone invocation of the script. Set to 2 for 
# more information while debugging.
verbose = 0

MSGR_TECHS = [('MCS', 0), ('UMB', 1), ('MM', 2), ('WMX', 3), ('LTE', 4), 
              ('FTM', 5), ('RFA', 6), ('CDMA', 7), ('HDR', 8), ('GERAN', 9), 
              ('GPS', 10), ('WCDMA', 11), ('DS', 12), ('ONEX', 13), 
              ('C2K', 14), ('NAS', 15), ('UIM', 16), ('UTILS', 17), 
              ('TDSCDMA', 18), ('WMS', 19), ('CNE', 20), ('IMS', 21), 
              ('QMI', 22), ('ECALL', 23), ('POLICYMAN', 24), ('CORE', 25),
              ('RFLM', 26)]

MSGR_TYPES = ['SPR', 'CMD', 'REQ', 'RSP', 'IND', 'FLM', 'OTA', 'DLM', 'CNF', 
              'TMR', 'CMDI', 'REQI', 'RSPI', 'INDI', 'CNFI', 'TMRI', 'INTI']

NUM_SPARE_MODS_PER_TECH = 2

DEFINE_UMID_MACRO = 'MSGR_DEFINE_UMID'

# REGEX Defintions
# REGEX to match #define MSGR_DEFINE_UMID(...) <macro definition>
UMID_MACRO_DEF = re.compile(r'^#define\s+{0}.*?\\\n'.format(DEFINE_UMID_MACRO), re.MULTILINE)

# Regex to match MSGR_DEFINE_UMID(...) call
UMID_MACRO = re.compile(r'{0}\s*\((.*?\)*)\)'.format(DEFINE_UMID_MACRO))

SINGLE_LINE_COMMENT = re.compile(r'//.*')
MULTI_LINE_COMMENT = re.compile(r'/\*.*?\*/', re.MULTILINE)
WHITE_SPACE = re.compile(r'\s+')

# Matches the outermost parathesis in one line. 
GREEDY_PARANTHESIS = '(\(.*\))'

# Inversion of a whitelist of characters allowed in string that can be passed to eval (^ is 
# logical not of the the base regex)
NUM_EXPR_BLACK_LIST = re.compile(r'[^xXa-fA-F0-9\(\)\|\&\<\>]')

section1_static_args = '''/*!
  @file   __msgr_global_umids.h

  @brief Auto-generated file containing UMID mapping tables

  @detail
  This file is AUTO-GENERATED.  Do not modify!
  This auto-generated header file declares the message router table mapping
  UMIDs to their string equivalent

*/

/*===========================================================================

  Copyright (c) {0} Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/
'''

section2_static = '''
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#ifndef MSGR_UMID_GEN
#error "__msgr_global_umids.h may only be included from msgr_umid.c"
#endif /* MSGR_UMID_GEN */

/* @brief Populate table entries based on flags set */
#ifdef TF_MSCGEN
  #ifndef TF_UMID_NAMES
    #error "Incorrect configuration - TF_UMID_NAMES must be defined"
  #else
    /* TF_MSCGEN & TF_UMID_NAMES is defined, so define complete table */
    #define MSGR_UMID_TABLE_ENTRY(hex_id, name, func_ptr) { hex_id, name, func_ptr }
  #endif
#elif defined TF_UMID_NAMES
  /* Only TF_UMID_NAMES is defined, so define UMID names only */
   #define MSGR_UMID_TABLE_ENTRY(hex_id, name, func_ptr) { hex_id, name, NULL }
#else
  /* TF_MSCGEN/TF_UMID_NAMES are not defined, so no definitions required
     This case is for final release*/
#endif

#ifdef TF_MSCGEN
#endif /* TF_MSCGEN */

/*! @brief Table mapping UMID to strings */
static const msgr_umid_name_entry_s msgr_umid_name_table[] = {
#if ((!defined TF_MSCGEN) && (!defined TF_UMID_NAMES))
  /* TF_MSCGEN/TF_UMID_NAMES are not defined, so no definitions */
  { 0x00000000, "", NULL },
#else
'''

# section3_dynamic is dynamically constructed by this script. 

section4_static = '''#endif /* ((!defined TF_MSCGEN) && (!defined TF_UMID_NAMES)) */
};

/*! @brief Table mapping TECH/MODULE to strings */
static const msgr_module_name_entry_s  msgr_module_name_table[] = {
#if ((!defined TF_MSCGEN) && (!defined TF_UMID_NAMES))
  /* TF_MSCGEN & TF_UMID_NAMES are not defined, so no definitions */
  { 0x0000, "" },
#else
'''

# section5_dynamic is dynamically constructed by this script. 

section6_static = '''#endif /* ((!defined TF_MSCGEN) && (!defined TF_UMID_NAMES)) */
};

'''

# section7_dynamic is dynamically constructed by this script. 

section8_static_args = '''
#define MSGR_NUM_TECHS {0}
static msgr_tech_jump_entry_s msgr_gen_jump_table[MSGR_NUM_TECHS] = 
{{
'''

# section9_dynamic is dynamically constructed by this script. 

section10_static_args = '''}};

#define MSGR_NUM_TOTAL_MSGS {0}


/* Defines Number of Unique Combinations of Tech/Mod/Type in the system 
   when MSGR gets compiled */

#define MSGR_NUM_TECH_MOD_TYPE {1}

/* Defines Number of Reg Nodes Slot Needed for Known UMIDS 
   when MSGR gets compiled */

#define MSGR_NUM_REGULAR_SLOTS {2}

/* The size of the top-level jump table,
   which points to the correct tech-specific table.*/
#define MSGR_JUMP_TABLE_SIZE  (MSGR_NUM_TECHS * sizeof(msgr_tech_jump_entry_s))

/* The total size of all the tech-specific tables. For each TECH, one row for 
   each module. Each jump table is a 2D array of max module rows and num type 
   columns each of which is an uint16 */
#define MSGR_TECH_JUMP_TABLE_SIZE  ({3} * MSGR_NUM_TYPES * sizeof(uint16))'''

#------------------------------------------------------------------------------
#                      SCONS HOOKS (exist/generate)
#------------------------------------------------------------------------------
def exists(env):
    '''Scons function to check if builder exists'''

    return env.Detect('msgr_builder')

def generate(env):
    '''Function to register actions with scons'''

    # Local import to prevent following import when executing from command line
    # where there is no scons framework
    import SCons.Scanner
    umid_act = env.GetBuilderAction(umid_builder)
    umid_bld = env.Builder(action = umid_act, 
                           source_scanner = SCons.Scanner.C.CScanner(),
                           emitter = umid_emitter,
                           suffix = '.umid', src_suffix = '.h')

    env.Append(BUILDERS = {'UMID' : umid_bld})

    msgr_act = env.GetBuilderAction(msgr_builder)
    msgr_bld = env.Builder(action = msgr_act, 
                           source_scanner = SCons.Scanner.C.CScanner(),
                           suffix = '.h', src_suffix = '.umid')

    env.Append(BUILDERS = {'MSGR' : msgr_bld})

def umid_emitter(target, source, env):
    '''UMID Generation emitter function'''

    if type(source) != types.ListType:
        source = [source]

    # Limit the following changes for Bolt only.  Clean this up once Bolt is branched
    env.PrintDebugInfo('msgr', "umid_emitter: target = " + str(target[0]))

    # Preprocess each of the source files
    dotis = []
    for src in source:
        # For each src file progressively walk up the src path until a build directory 
        # is found inside the parent dir.         
        parentdir = os.path.dirname(str(src))
        while not os.path.exists(os.path.join(parentdir, 'build')):
            parentdir = os.path.dirname(parentdir)

        if os.path.exists(os.path.join(parentdir, 'build')):
            # Split on filename to remove the path to it. Next remove the file extension
            src_filename = os.path.splitext(os.path.split(str(src))[1])[0]
            doti_file = os.path.join(parentdir, 'build/${BUILDPATH}/', src_filename + '.i')
        else:
            # This case should never be hit as the modme_proc/build definitely exists.
            env.PrintError('Could not find a build folder in in any fo the parent dirs for file: ' + str(src))
            sys.exit(1)

        # Preprocess the .h -> .i
        doti = env.PreProcess(doti_file, src)
        
        # Dependency between target and the preprocessed file
        env.Depends(target, doti)
        dotis.append(doti)

    # Append the list of .i files
    source.extend(dotis)

    return (target, source)

def umid_builder(target, source, env):
    '''UMID Generation builder function'''

    verbose = int(env.GetOption('verbose'))
    target_list = [str(target[0])]
    assert len(target_list) == 1

    # Find the list of .i and .h files
    dotis = []
    doths = []
    for src in source:
        srcname = str(src)
        ext = os.path.splitext(srcname)[1]
        if ext == '.i':
            dotis.append(srcname)
        if ext == '.h':
            doths.append(srcname)

    if verbose >= 2:
        env.PrintInfo('Calling umid_builder_impl({0}, {1}, {2})'.format(dotis, doths, target_list))
    umid_builder_impl(dotis, doths, target_list)

def msgr_builder(target, source, env):
    '''MSGR Generation builder function'''

    verbose = int(env.GetOption('verbose'))
    source_list = [str(s) for s in source]
    target_list = [str(target[0])]
    assert len(target_list) == 1

    if verbose >= 2:
        env.PrintInfo('Calling msgr_builder_impl({0}, {1})'.format(source_list, target_list))
    msgr_builder_impl(source_list, target_list)

#-------------------------------------------------------------------------------
# Internal functions
#-------------------------------------------------------------------------------

def umid_builder_impl(dotis, doths, target):
    '''Underlying umid builder implementation'''

    # Parse the header files
    umid_data = {}
    for each_file in doths:
        with open(each_file, 'r') as fh:
            file_str = fh.read()
        filename = os.path.split(each_file)[1]
        umid_data.update(extract_umid_names(file_str, filename))

    # Parse the preprocessed output files
    if umid_data is not None:
        for each_file in dotis:
            with open(each_file, 'r') as fh:
                file_str = fh.read()
            umid_data = extract_umid_values(file_str, umid_data)

    # Write the .umid output: An intermediate json formatted output file.
    # Key: String representation of UMID of the form TECH_MOD_TYPE_ID
    # Value: A dict containing the keys described below
    # src_file: The .h file containing the UMID definition
    # raw: List of args to MSGR_DEFINE_UMID macro from the .h
    # umid_alt_str: Alternate UMID representation of the form TECH_MOD_ID_TYPE
    # raw_value: Raw string that represents the MSGR_DEFINE_UMID macro 
    #            evaluation from the .i
    # int_value: Decimal integer value derived raw_value
    # field_value: List of decimal integer values for TECH, MOD, TYPE, ID
    with open(target[0], 'w+') as fh:
        json.dump(umid_data, fh, indent=2)

    if verbose >= 2:
        print 'msgr_builder.py: Done UmidBldrImpl'

def msgr_builder_impl(source, target):
    '''Underlying msgr builder implementation'''

    # Parse the .umid files and merge them into one data struct
    umid_data = {}
    missing_umid_evals = []
    for each_file in source:
        with open(each_file, 'r') as fh:
            umid_dict = json.load(fh)

        # umid_data dict is a modification fo the dict inside the .umid file.
        # The new dict is hashed to the integer value of the umid as opposed to 
        # the string value in the .umid file to make the rest of the code easier.
        for (key, value) in umid_dict.iteritems():
            try:
                umid_data[value['int_value']] = value
            except KeyError:
                missing_umid_evals.append(key)
    
    if verbose >= 2:
        print 'msgr_builder.py: Missing umids: ', missing_umid_evals

    # Generate the global umid header file
    write_global_umids_file(target[0], umid_data)
    if verbose >= 2:
        print 'msgr_builder.py: Done MsgrBldrImpl'

def extract_umid_names(file_str, filename):
    '''Extract all umids in the header defined using the MSGR macro'''

    # Step 1: First ensure that the MSGR macro definition itself is not present 
    # in the file by deleting all occurances (cannot be more than 1 occurance 
    # of the same macro per C file)
    file_str = re.sub(UMID_MACRO_DEF, '', file_str)

    # Step 2: Remove comments as they may have been inserted in the middle of 
    # a multi line MSGR macro call
    file_str = re.sub(SINGLE_LINE_COMMENT, '', file_str)
    file_str = re.sub(MULTI_LINE_COMMENT, '', file_str)

    # Step 3: Remove newlines and spaces as MSGR macro calls may span multiple lines
    file_str = re.sub(WHITE_SPACE, '', file_str)

    # Step 4: Find all MSGR macro calls
    umid_defs = re.findall(UMID_MACRO, file_str)

    # Step 5: Create a dict{umidstr: {'raw':'the args passed to define UMID macro'}} for 
    # each UMID found
    umid_dict = {}
    for each_umid in umid_defs:
        split = each_umid.split(',')
        # The key is UMID of the form TECH_MOD_TYPE_ID
        key = '_'.join(split[:4])
        # This is different from key and is the form TECH_MOD_ID_TYPE as 
        # used widely in client code
        umid_alt_str =  '_'.join([split[0], split[1], split[3], split[2]])
        umid_dict[key] = {'raw': split, 'umid_alt_str': umid_alt_str, 'src_file': filename}
    return umid_dict

def extract_umid_values(file_str, umid_data):
    '''Extract all the umid evaluated in the preprocessed file'''

    if umid_data is None:
        return umid_data

    # Step 1: Remove spaces (add why)
    file_str = re.sub(' ', '', file_str)

    for each_umid in umid_data.keys():

        # Step 2: Find the preprocessed RHS expression for UMID
        pattern1 = '{0}={1},*'.format(each_umid, GREEDY_PARANTHESIS)
        umid_value = re.search(pattern1, file_str)

        # If UMID is missing in .i file it could be conditionally compiled out
        if umid_value is not None:
            raw_value = umid_value.group(1)

            # Step 3: Remove typecasts msgr_umid_type and uint16
            # from macro eval.
            raw_value = re.sub('\(msgr_umid_type\)|\(uint16\)', '', raw_value)

            # Step 4: If necessary, replace id enum with its value. Design 
            # allows ids to be specified as enum type only if they have 
            # hardcoded values specified in the source file. 
            # Eg. enum{ID1=1, ID2=2} is okay but not enum{ID1, ID2} 
            # Pattern to match the enum type = value in the preprocessed file 
            if re.search(NUM_EXPR_BLACK_LIST, 
                         umid_data[each_umid]['raw'][4]) is not None:
                pattern2 = r'{0}\s*=\s*([^,\s]+)'.format(umid_data[each_umid]['raw'][4])
                id_value = re.search(pattern2, file_str)

                # If the above regex returned None it may just imply that ID 
                # value is not an enum. It could have been a macro that was 
                # already evaluated in the .i needing no further action 
                # or was already a numeric literal. No else case necessary as 
                # this is an optional substitution only for enums.
                if id_value is not None:
                    raw_value = re.sub(umid_data[each_umid]['raw'][4], 
                                       id_value.group(1), raw_value)

            # Step 5: Convert the macro eval to an int value using 
            # restricted eval for safety
            int_value = restricted_eval(raw_value)
            if int_value < 0:
                sys.exit('msgr_builder.py: ERROR: Unexpected negative UMID value')
            field_value = [(int_value >> 24)& 0xFF, (int_value >> 16)& 0xFF, 
                           (int_value >> 8)& 0xFF, int_value & 0xFF,
                           (int_value >> 16)& 0xFFFF]

            # Step 6: Store value into dict
            umid_data[each_umid]['raw_value'] = raw_value
            umid_data[each_umid]['int_value'] = int_value
            umid_data[each_umid]['field_value'] = field_value
    return umid_data

def write_global_umids_file(target, umid_data):
    '''Generate the MSGR global header'''

    # Step 1: Initialize variables for substitution
    YEAR = date.today().year
    MSGR_NUM_TECHS = len(MSGR_TECHS)
    MSGR_NUM_TOTAL_MSGS = len(umid_data.keys())
    MSGR_NUM_TECH_MOD_TYPE = 0
    MSGR_NUM_REGULAR_SLOTS = 0
    MSGR_NUM_TOTAL_MODS = 0

    # Although the following variables were initialized in global scope they 
    # cant be modified in local scope without declaring them global. 
    # Instead the following are new local variables that have the same name.
    # The ones in global scope are just place holders to show relation with 
    # static string variables that are appended to the file and contain 
    # comments etc.
    section3_dynamic = ''
    section5_dynamic = ''
    section7_dynamic = ''
    section9_dynamic = ''

    section3a_substr = '  /* {0}, payload: {1} */\n  MSGR_UMID_TABLE_ENTRY ( 0x{2:08x}, "{0}", NULL ),\n\n'
    section5a_substr = '  /* {1} */\n  {{ 0x{0:04x}, "MSGR_{1}" }},\n\n'
    section7a_substr = '''
#define MSGR_MAX_{0}_MODULES {1}
static uint16 msgr_{2}_jump_table[MSGR_MAX_{0}_MODULES][MSGR_NUM_TYPES] = 
{{ /*   SPR,    CMD,    REQ,    RSP,    IND,    FLM,    OTA,    DLM,    CNF,    TMR,   CMDI,   REQI,   RSPI,   INDI,   CNFI,   TMRI,   INTI,  */
'''
    section7b_substr = '''  /* {0} (0x{1:04x}) */
  {{ 0x{2:04x}, 0x{3:04x}, 0x{4:04x}, 0x{5:04x}, 0x{6:04x}, 0x{7:04x}, 0x{8:04x}, 0x{9:04x}, 0x{10:04x}, 0x{11:04x}, 0x{12:04x}, 0x{13:04x}, 0x{14:04x}, 0x{15:04x}, 0x{16:04x}, 0x{17:04x}, 0x{18:04x}, }},\n'''
    section7c_substr = '''  /* Unused (0x{0:04x})*/
  {{ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, }},\n'''
    section7d_substr = '''};
'''
    section9a_substr = '  {{ (uint16*)msgr_{0}_jump_table, {1} }},\n'
    section9b_substr = '  {{ NULL, 0 }},/* {0} (no messages) */\n'

    techmod_list = []

    # Umid with lower nibble of the id zeroed is useful in counting the number 
    # of regular slots needed. 
    umid_list_lowernibble_zeroed = []

    # Data structure for couting the number of umids per unique combination of 
    # tech+mod+type. For reference, a defaultdict is a python container that is
    # a plain dict with the added functionality that if a key missing in the 
    # dict is accessed, it is initialized to a default value instead of a KeyError.
    ids_per_techmodtype = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))

    # Step 2: Generate a list of hex umids sorted
    umid_sorted = sorted(umid_data.keys())

    for each_umid in umid_sorted:
        # Step 3: Loop through the list of UMIDs to:
        # a. Create a list of TECH MODs
        # b. Create a list of UMIDs with lower nibbled zeroed out
        # c. Format the umid hex value and string name to populate the 
        # msgr_umid_name_table
        umid_list_lowernibble_zeroed.append(each_umid & ~0xF)
        raw = umid_data[each_umid]["raw"]
        field_value = umid_data[each_umid]['field_value']
        section3_dynamic = ''.join((section3_dynamic, section3a_substr.format(
            umid_data[each_umid]['umid_alt_str'], raw[5], 
            each_umid)))

        techmod = get_techmod(field_value[0], field_value[1])
        techmod_list.append((techmod, '_'.join((raw[0], raw[1]))))

        (ids_per_techmodtype[raw[0]][(raw[1], field_value[1])][raw[2]]).append(each_umid)

    # Step 4: Obtain the list of unique tech + mod + type + upper id nibble 
    # combinations to calculating the number of regular slots necessary 
    sorted_techmod = sorted(set(techmod_list), key = lambda x: x[0])
    techmod_dict = dict(sorted_techmod)
    MSGR_NUM_REGULAR_SLOTS = len(set(umid_list_lowernibble_zeroed))

    # Step 5: For each tech mod populate the msgr_module_name_table
    for each_techmod in sorted_techmod:
        section5_dynamic = ''.join(
            (section5_dynamic, section5a_substr.format(
                each_techmod[0], each_techmod[1])))

    # Step 6: For each TECH:
    # a. Count the number of umids for a given ombination of TECH+MOD+TYPE
    # b. Compute the max modules defined for ech tech with allowance for spares
    # that may be defined in run time
    for each_tech in MSGR_TECHS:
        if each_tech[0] in ids_per_techmodtype.keys():
            mods_per_tech = ids_per_techmodtype[each_tech[0]].keys()
            sorted_mods_per_tech = sorted(mods_per_tech, key = lambda x: x[1])
            
            max_modules = max(mods_per_tech, key = lambda x: x[1])[1] + \
                NUM_SPARE_MODS_PER_TECH + 1
            MSGR_NUM_TOTAL_MODS += max_modules

            section7_dynamic = ''.join((section7_dynamic, section7a_substr.format(
                each_tech[0], max_modules, each_tech[0].lower())))

            # Step 7: Format the jump table for each TECH. In addition, 
            # calculate if the client tech may be using the umid number 
            # space in efficiently with gaps between individial ids.
            for module_num in xrange(max_modules):
                mod = [x for x in sorted_mods_per_tech if module_num == x[1]]
                if len(mod) == 1:
                    types_per_techmod = ids_per_techmodtype[each_tech[0]][mod[0]]
                    MSGR_NUM_TECH_MOD_TYPE += len(types_per_techmod)
                    num_ids = [0xffff] * len(MSGR_TYPES) 
                    # For each tech, mod, type there could be 256 ids. For each 
                    # allocated upper nibble in the id, memory for 16 ids is 
                    # added to MSGR_REG_NODES. So the most efficient way for 
                    # clients to allcoate ids is to start with 0 and 
                    # increment sequentially without any gaps. The check here 
                    # flags inefficiencies if the max id defined exceeds a 
                    # multiple of 16 just greater than the count.
                    for each_type in MSGR_TYPES:
                        if each_type in types_per_techmod:
                            num_ids[MSGR_TYPES.index(each_type)] = max(types_per_techmod[each_type])&0xFF
                            x = types_per_techmod[each_type]
                            y = math.ceil(len(x)/16.0)*16
                            if ((max(x)&0xFF) > y):
                                inefficient_techmod = techmod_dict[get_techmod(each_tech[1], module_num)]
                                print 'msgr_builder.py: {0}_{1} Num ids: {2} Max id: {3}'.format(
                                    inefficient_techmod, each_type, len(x), max(x)&0xFF)
                    section7_dynamic = ''.join((section7_dynamic, 
                        section7b_substr.format(mod[0][0], module_num, *num_ids)))

                elif len(mod) == 0:
                    section7_dynamic = ''.join((section7_dynamic, 
                        section7c_substr.format(module_num)))
                else:
                    print 'msgr_builder.py: ERROR: Two modules with same module number'
                    sys.exit(1)


            section7_dynamic = ''.join((section7_dynamic, section7d_substr))

            # Step 8: Format the TECH jump table if umids have been defined for
            # the TECH
            section9_dynamic = ''.join((section9_dynamic, section9a_substr.format(
                each_tech[0].lower(), max_modules)))

        else:
            # Step 9: Format the TECH jump table entry to null if no umids are 
            # defined for the TECH
            section9_dynamic = ''.join((section9_dynamic, section9b_substr.format(
                each_tech[0])))

    # Step 10: Assemble the different sections of the file in the right order 
    # with variable substitution
    file_str_list = [section1_static_args.format(YEAR),
                     section2_static, section3_dynamic, section4_static,
                     section5_dynamic, section6_static, section7_dynamic, 
                     section8_static_args.format(MSGR_NUM_TECHS), 
                     section9_dynamic, 
                     section10_static_args.format(MSGR_NUM_TOTAL_MSGS,
                                                  MSGR_NUM_TECH_MOD_TYPE,
                                                  MSGR_NUM_REGULAR_SLOTS,
                                                  MSGR_NUM_TOTAL_MODS)]

    file_str = ''.join(file_str_list)

    # Step 11: Write output to file
    with open(target, 'w+') as fh:
        fh.write(file_str)
    if verbose >= 2:
        print 'msgr_builder.py: Done writing __msgr_global_umids.h'

def get_techmod(tech, mod):
    '''Convert input tech, mod bytes to a 16 bit tech+mod value'''

    return (tech << 8) | mod

def restricted_eval(raw):
    '''Convert input string to int. Input string can have only 0-9, a-f, A-F, 
    Xx, (), &, |'''

    value = -1
    if re.search(NUM_EXPR_BLACK_LIST, raw) is None:
        value = eval(raw)
    else:
        print 'msgr_builder.py: ERROR: Potentially unsafe argument to eval. Chars from blacklist found in: {0}'.format(raw)
    return value

#-------------------------------------------------------------------------------
# Command line invocation
# Build with --verbose=2. Note down the umid_builder/ msgr_builder <blob> logged
# Invoke from command line with python msgr_builder.py <blob>
# Set verbosity at the top of this file if more detail is necessary.
#-------------------------------------------------------------------------------

# main() for command line invocation of the builders
def main(argv):
    if verbose >= 2:
        print 'msgr_builder.py: Invoking cmd:', argv[1]
    else:
        print 'msgr_builder.py: Invoking cmd...'
    eval(argv[1])
    print 'msgr_builder.py: Done'

# Facilitate command line invocation for debugging
# Pass the function and args to execute as a string argument to 
# msgr_builder.py on the command line.
if __name__ == '__main__':
    main(sys.argv)
