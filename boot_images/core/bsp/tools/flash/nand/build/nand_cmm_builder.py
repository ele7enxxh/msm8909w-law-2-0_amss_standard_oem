#===============================================================================
#
# NAND CMM file builder
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Confidential and Proprietary
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/bsp/tools/flash/nand/build/nand_cmm_builder.py#1 $
#  $DateTime: 2015/09/01 00:30:35 $
#  $Author: pwbldsvc $
#  $Change: 8935858 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/11/10   eo       Export BUILD_ID in nand_param.cmm
# 04/12/09   mh       Add FLASH_DUAL_NAND_INTERLEAVE
# 11/04/09   op       Export MSM Id in nand_param.cmm 
# 08/12/09   mh       Branched 
#===============================================================================
import os
import subprocess
import string
import re
from datetime import datetime

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('nand_cmm_builder')

def generate(env):
   nand_param_generate(env)
   flash_prog_error_generate(env)

#===============================================================================
# nand_param.cmm build rules
#===============================================================================
def nand_param_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for file_name.cmm
   #
   nand_param_act = env.GetBuilderAction(nand_param_builder)
   nand_param_bld = env.Builder(action = nand_param_act, 
      emitter = nand_param_emitter,
      suffix = '.cmm')
   
   env.Append(BUILDERS = {'NandParamBuilder' : nand_param_bld})

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def nand_param_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# def builder for nand_param.cmm
#
def nand_param_builder(target, source, env):
   nand_param_template = '''
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  GENERATED FILE - DO NOT EDIT
;;
;; generated:  DATE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global &NandTargetFamily
&NandTargetFamily="FLASH_CFG_ID"
global &NandMsmId
&NandMsmId="MSMID_TYPE"
global &NandCodeAddr
&NandCodeAddr=NAND_CODE_ADDR
global &NandParamAddr
&NandParamAddr=NAND_PARAM_ADDR
global &NandBootRomBegin
&NandBootRomBegin=NAND_BOOT_BEGIN
global &NandBootRomEnd
&NandBootRomEnd=NAND_BOOT_END
global &NandCodeRomBegin
&NandCodeRomBegin=NAND_CODE_BEGIN
global &NandCodeRomEnd
&NandCodeRomEnd=NAND_CODE_END
global &NandConfigDir
&NandConfigDir="FLASH_NAND_TOOLS_CFG"
global &NandSrcRoot
&NandSrcRoot="SRCROOT"
global &UseTerm
&UseTerm="USES_PRINTF"
global &NandDualMode
&NandDualMode="FLASH_DUAL_NAND_INTERLEAVE"
global &BuildId
&BuildId="BUILDID"
'''
   # relpace items in tamplate
   #import pdb; pdb.set_trace()
   nand_param_template = nand_param_template.replace('DATE', str(datetime.now().ctime()))
   nand_param_template = nand_param_template.replace('FLASH_CFG_ID', env['FLASH_CFG_ID'])
   nand_param_template = nand_param_template.replace('MSMID_TYPE', env['MSMID_TYPE'])
   nand_param_template = nand_param_template.replace('NAND_CODE_ADDR', env['NAND_CODE_ADDR'])
   nand_param_template = nand_param_template.replace('NAND_PARAM_ADDR', env['NAND_PARAM_ADDR'])
   nand_param_template = nand_param_template.replace('NAND_BOOT_BEGIN', env['NAND_BOOT_BEGIN'])
   nand_param_template = nand_param_template.replace('NAND_BOOT_END', env['NAND_BOOT_END'])
   nand_param_template = nand_param_template.replace('NAND_CODE_BEGIN', env['NAND_CODE_BEGIN'])
   nand_param_template = nand_param_template.replace('NAND_CODE_END', env['NAND_CODE_END'])
   nand_param_template = nand_param_template.replace('FLASH_NAND_TOOLS_CFG', env['FLASH_NAND_TOOLS_CFG'])
   nand_param_template = nand_param_template.replace('FLASH_DUAL_NAND_INTERLEAVE', env['FLASH_DUAL_NAND_INTERLEAVE'])
   nand_param_template = nand_param_template.replace('BUILDID', env.subst("${BUILD_ID}"))
   nand_param_template = nand_param_template.replace('SRCROOT', env['SRCROOT'])
   if env.has_key('USES_PRINTF'):
      nand_param_template = nand_param_template.replace('USES_PRINTF', 'YES')
   else:
      nand_param_template = nand_param_template.replace('USES_PRINTF', 'NO')   

   list_fname = str(target[0])
   list_file = open (list_fname, "w")
   list_file.write(nand_param_template)
   list_file.close()
   
   return None

#===============================================================================
# flash_prog_error.cmm build rules
#===============================================================================
def flash_prog_error_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   flash_prog_error_act = env.GetBuilderAction(flash_prog_error_builder)
   flash_prog_error_bld = env.Builder(action = flash_prog_error_act, 
      emitter = flash_prog_error_emitter,
      suffix = '.cmm')
   
   env.Append(BUILDERS = {'FlashProgErrBuilder' : flash_prog_error_bld})

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def flash_prog_error_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# def builder for qcoreapp_booting.pbn
#
def flash_prog_error_builder(target, source, env):
   # init and fix variebles
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))
   
   #import pdb; pdb.set_trace()   
   source_contents = source[0].get_contents()
   
   # search for patter, example
   #ERR_OK=0x00,              /* Success. Send an acknowledgement.            */
  
   entry_re = re.compile(r'^\s*(\S+)\s*=\s*(\S+)\s*,\s*\/\*\s*(.*)\*\/', re.M)
   entries = entry_re.findall(source_contents)
   
   if len(entries) > 0:
      cmm_lines = [
         ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;",
         ";;  GENERATED FILE - DO NOT EDIT",
         ";;",
         ";; generated:  " + str(datetime.now().ctime()),
         ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;",
         "",
         "",
         "local &ExtError",
         "",
         ";==========================================",
         "; Get the arguments passed in.",
         ";==========================================",
         "ENTRY &ExtError",
         "",
         "",
         "",
         "print \"  \"",
         "print \"  \"",
         "print \"Extended error is &ExtError\"",
         "",
         "",
         "",
      ]
      
      for entry in entries:
         cmm_lines.append ([
            "if &ExtError==" + entry[1],
            "(",
            "  print \"Error name is " + entry[0] + "\"",
            "  print \"      " + entry[2] + "\"",
            "  enddo",
            ")",
            " ",
         ])
      
      cmm_lines.append ([
         "",
         "",
         "enddo",
         "",
      ])
      
      # since we added sub arrays to the original array, flattent it first
      cmm_lines = env.Flatten(cmm_lines)
      
      # write cmm_file and add new line char to each line
      cmm_file = open(target_str, 'w')
      cmm_file.writelines([line+'\n' for line in cmm_lines])
      cmm_file.close()
   
   return None   
