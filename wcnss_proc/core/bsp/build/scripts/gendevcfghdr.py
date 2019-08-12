# Copyright (c) 2011
# Qualcomm Technologies Incorporated
#!/usr/bin/env python

import sys
import os
import shutil
from SCons.Script import *
from elfparserutil import *

INPUT_ELF = 'input_elf'
SECTION_NAME = 'section_name'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('gendevcfghdr_builder')

def generate(env):
    rootenv = env.get('IMAGE_ENV')   
    #rootenv.AddMethod(query_section_by_name_util, "QuerySectionByName")
    gendevcfghdr_act = env.GetBuilderAction(gendevcfghdr_builder)
    gendevcfghdr_bld = env.Builder(action = gendevcfghdr_act, 
                                        source_scanner = SCons.Scanner.C.CScanner(),
                                        emitter = gendevcfghdr_emitter
                                    )
    rootenv.Append(BUILDERS = {'GenerateDevCfgHdrs' : gendevcfghdr_bld})
    
    return

def gendevcfghdr_emitter(target, source, env):
    return (target, source)

def gendevcfghdr_builder(target, source, env):
    rootenv = env.get('IMAGE_ENV')
    pri_file_name = env.RealPath(str(source[0]))
    target_full = env.RealPath(str(target[0]))

    if 'SECTION_NAME' not in env:
        err_str = 'SECTION_NAME' + ' key required'
        raise RuntimeError, err_str
    pri_section_name = env.get('SECTION_NAME')
    
    DEBUG_PRINT("Querying Section details for: " + pri_section_name + " for ELF: " + pri_file_name)

    #Initialize Primary ELF
    pri_elf_fp = OPEN(pri_file_name, "rb")
    pri_elf_dict = preprocess_elf_file(pri_elf_fp, pri_file_name)
    if IS_DEBUG():
        readelf(pri_elf_dict)    

    elf_sec_list = query_section_by_name(pri_elf_dict, pri_section_name)
    # DEBUG_PRINT("Printing Section Details for Section Name: \"" + pri_section_name + "\"")
    # DEBUG_PRINT(
        # '\n\n'.join('\n'.join('    [' + k + ']: ' + str(hdr[k]) for k in hdr) for hdr in sec_hdrs))
    pri_elf_fp.close()

    if len(elf_sec_list) != 1:
        err_str = 'Error:There has to be only one section in the ELF file {0} ' \
                    'with section name:{1}'.format(pri_file_name, pri_section_name)
        env.PrintError(err_str)
        raise RuntimeError, err_str
   
    if elf_sec_list[0].has_key('addr') is False:
        err_str = 'Error:No \'Addr\' found in the ELF file {0} ' \
                    'with section name:{1}'.format(pri_file_name, pri_section_name)
        env.PrintError(err_str)
        raise RuntimeError, err_str
    else:
        devcfg_sec_vaddr = elf_sec_list[0]['addr']

    # Add devcfg_sec_vaddr to  devcfg_def.h
    devcfg_def_hdr_file_loc = target_full
    devcfg_def_hdr_file = open(devcfg_def_hdr_file_loc, 'w')
    devcfg_def_hdr_file.write("#ifndef DEVCFG_DEF_H\n")
    devcfg_def_hdr_file.write("#define DEVCFG_DEF_H\n")
    devcfg_def_hdr_file.write("#define DEVCFG_DATA_ADDR {0:#0x}\n".format(devcfg_sec_vaddr))
    devcfg_def_hdr_file.write("#endif /* ifndef DEVCFG_DEF_H */\n")
    devcfg_def_hdr_file.close()
    
    env.Append(CPPDEFINES = [('DEVCFG_DATA_ADDR', "{0:#0x}".format(devcfg_sec_vaddr))])
    
    return

def OPEN(file_name, mode):
    try:
        fp = open(file_name, mode)
    except IOError:
        raise RuntimeError, "The file could not be opened: " + file_name

    # File open has succeeded with the given mode, return the file object
    return fp

def query_section_by_name_util(env, targets, input_dict):
    rootenv = env.get('IMAGE_ENV')
    if INPUT_ELF not in input_dict:
        err_str = INPUT_ELF + ' key required'
        raise RuntimeError, err_str
    pri_file_name = input_dict[INPUT_ELF]
    pri_section_name = input_dict[SECTION_NAME]
    env.Depends(pri_section_name, pri_file_name)
    print "File name is : " + pri_file_name, pri_section_name + "\n"
    DEBUG_PRINT("Querying Section details for: " + pri_section_name + " for ELF: " + pri_file_name)

    #Initialize Primary ELF
    pri_elf_fp = OPEN(pri_file_name, "rb")
    pri_elf_dict = preprocess_elf_file(pri_elf_fp, pri_file_name)

    sec_hdrs = query_section_by_name(pri_elf_dict, pri_section_name)

    pri_elf_fp.close()
    
    return sec_hdrs
    