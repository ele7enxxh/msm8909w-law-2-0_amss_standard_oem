#===============================================================================
#
# NOR CMM file builder
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2010-2011, 2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Confidential and Proprietary
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/bsp/tools/flash/nor/build/nor_cmm_builder.py#1 $
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
# 03/19/14   whc      Change I2C NOR to default
# 07/21/11   sv       Add flag to decide SPI/I2C NOR
# 09/03/10   sv       Initial Revision 
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
   return env.Detect('nor_cmm_builder')

def generate(env):
   nor_param_generate(env)
   flash_prog_error_generate(env)

#===============================================================================
# nor_param.cmm build rules
#===============================================================================
def nor_param_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for file_name.cmm
   #
   nor_param_act = env.GetBuilderAction(nor_param_builder)
   nor_param_bld = env.Builder(action = nor_param_act, 
      emitter = nor_param_emitter,
      suffix = '.cmm')
   
   env.Append(BUILDERS = {'NorParamBuilder' : nor_param_bld})

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def nor_param_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# def builder for nor_param.cmm
#
def nor_param_builder(target, source, env):
   nor_param_template = '''
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  GENERATED FILE - DO NOT EDIT
;;
;; generated:  DATE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global &NorTargetFamily
&NorTargetFamily="FLASH_CFG_ID"
global &NorMsmId
&NorMsmId="MSMID_TYPE"
global &NorCodeAddr
&NorCodeAddr=NOR_CODE_ADDR
global &NorParamAddr
&NorParamAddr=NOR_PARAM_ADDR
global &NorBootRomBegin
&NorBootRomBegin=NOR_BOOT_BEGIN
global &NorBootRomEnd
&NorBootRomEnd=NOR_BOOT_END
global &NorCodeRomBegin
&NorCodeRomBegin=NOR_CODE_BEGIN
global &NorCodeRomEnd
&NorCodeRomEnd=NOR_CODE_END
global &NorConfigDir
&NorConfigDir="FLASH_NOR_TOOLS_CFG"
global &NorSrcRoot
&NorSrcRoot="SRCROOT"
global &UseTerm
&UseTerm="USES_PRINTF"
global &BuildId
&BuildId="BUILDID"
global &NorType
&NorType="NOR_TYPE"
'''
   # relpace items in template
   #import pdb; pdb.set_trace()
   nor_param_template = nor_param_template.replace('DATE', str(datetime.now().ctime()))
   nor_param_template = nor_param_template.replace('FLASH_CFG_ID', env['FLASH_CFG_ID'])
   nor_param_template = nor_param_template.replace('MSMID_TYPE', env['MSMID_TYPE'])
   nor_param_template = nor_param_template.replace('NOR_CODE_ADDR', env['NOR_CODE_ADDR'])
   nor_param_template = nor_param_template.replace('NOR_PARAM_ADDR', env['NOR_PARAM_ADDR'])
   nor_param_template = nor_param_template.replace('NOR_BOOT_BEGIN', env['NOR_BOOT_BEGIN'])
   nor_param_template = nor_param_template.replace('NOR_BOOT_END', env['NOR_BOOT_END'])
   nor_param_template = nor_param_template.replace('NOR_CODE_BEGIN', env['NOR_CODE_BEGIN'])
   nor_param_template = nor_param_template.replace('NOR_CODE_END', env['NOR_CODE_END'])
   nor_param_template = nor_param_template.replace('FLASH_NOR_TOOLS_CFG', env['FLASH_NOR_TOOLS_CFG'])
   nor_param_template = nor_param_template.replace('BUILDID', env.subst("${BUILD_ID}"))
   nor_param_template = nor_param_template.replace('SRCROOT', env['SRCROOT'])
   if env.has_key('USES_PRINTF'):
      nor_param_template = nor_param_template.replace('USES_PRINTF', 'YES')
   else:
      nor_param_template = nor_param_template.replace('USES_PRINTF', 'NO')   

   if env.has_key('USE_SPI_NOR'):
      nor_param_template = nor_param_template.replace('NOR_TYPE', 'SPI_NOR')
   else:
      nor_param_template = nor_param_template.replace('NOR_TYPE', 'I2C_NOR')

   list_fname = str(target[0])
   list_file = open (list_fname, "w")
   list_file.write(nor_param_template)
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
