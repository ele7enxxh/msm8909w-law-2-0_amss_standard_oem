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
   return env.Detect('elfmergerutil_builder')

def generate(env):
   rootenv = env.get('IMAGE_ENV')
   #rootenv.AddMethod(query_section_by_name_util, "QuerySectionByName")
   
   elfmergerutil_act = env.GetBuilderAction(elfmergerutil_builder)
   elfmergerutil_bld = env.Builder(action = elfmergerutil_act, 
                         source_scanner = SCons.Scanner.C.CScanner(),
                         emitter = elfmergerutil_emitter
                         )

   rootenv.Append(BUILDERS = {'ElfMergerUtilBuilder' : elfmergerutil_bld})

def elfmergerutil_emitter(target, source, env):
    return (target, source)

def merge_elf_util(pri_file_name, pri_section_name, sec_file_name, 
                    sec_section_name, out_file_name, verbose=False):
    global VERBOSE
    VERBOSE = verbose
    DEBUG_PRINT("Running merge on Primary ELF:[" + pri_file_name + "] at section containing "   \
                "name:[\"" + pri_section_name + "\"] and Secondary ELF:[" + sec_file_name +     \
                "] from section(s) containing name:[\"" + sec_section_name + "\"]. "            \
                "Destination Output ELF:[" + out_file_name + "].")

    sec_elf_fp = OPEN(sec_file_name, "rb")
    sec_elf_dict = preprocess_elf_file(sec_elf_fp, sec_file_name)
    #readelf(sec_elf_dict)

    shutil.copyfile(pri_file_name, out_file_name)
    out_elf_fp = OPEN(out_file_name, "r+b")
    out_elf_dict = preprocess_elf_file(out_elf_fp, out_file_name)

    #if IS_DEBUG():
        #readelf(out_elf_dict)

    no_error = False
    try:
        merge_sections_by_name(out_elf_dict, sec_elf_dict, pri_file_name,
                            pri_section_name, sec_section_name)
        no_error = True
    except RuntimeError as err_str:
        raise RuntimeError, err_str
    finally:
        sec_elf_fp.close()
        out_elf_fp.close()
        if False == no_error:
            os.remove(out_file_name)

    return None

def elfmergerutil_builder(target, source, env):
    pri_elf = env.RealPath(str(source[0]))
    sec_elf = env.RealPath(str(source[1]))
    pri_sec_name = env['PRIMARY_SECTION_NAME']
    sec_sec_name = env['SECONDARY_SECTION_NAME']
    target_full = env.RealPath(str(target[0]))
    
    merge_elf_util(pri_elf, pri_sec_name, sec_elf, sec_sec_name, target_full, True)
    
    return