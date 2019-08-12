#============================================================================
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to create MBN file
#
# Copyright 2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.5.c12.3/bsp/build/scripts/QurtImageSconsBuilder.py#1 $
#  $DateTime: 2016/12/13 07:59:23 $
#  $Author: mplcsds1 $
#  $Change: 11985146 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 10/03/11   dxiang    Clean up 
#============================================================================
import os
import subprocess
import string
import re
import zipfile
import glob
import shutil
import fnmatch
from SCons.Script import *
  
  
#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------
QDST_ROOT = "${BUILD_ROOT}/core/storage/tools/qdst"
QurtImageTool = "${COREBSP_ROOT}/kernel/qurt/scripts/qurt-image-build.py"
QurtImageTool2 = "${COREBSP_ROOT}/kernel/qurt/build/core/qdsp6/${BINARY_BUILDPATH}/install/modemv5/scripts/qurt-image-build.py"
QurtImageTool3 = "${COREBSP_ROOT}/kernel/qurt/build/mapss_b/qdsp6/${BINARY_BUILDPATH}/install/modemv5/scripts/qurt-image-build.py"
QurtImageTool4 = "${COREBSP_ROOT}/kernel/qurt/install/modemv56/scripts/qurt-image-build.py"
suitablequrttool=[]

#don't use: QurtImageTool3 = "${COREBSP_ROOT}/kernel/qurt/kernel/scripts/qurt-image-build.py"



#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('qurtimagesconsbuilder')
   
def generate(env):
   qurtimage_generate(env)   
   
def qurtimage_generate(env):
   qurtimage_act = env.GetBuilderAction(qurtimage_builder)
   qurtimage_bld = env.Builder(action = qurtimage_act,
                                      emitter = qurtimage_emitter,
                                      suffix = '.elf')

   env.Append(BUILDERS = {'QurtImageBuilder' : qurtimage_bld})


def qurtimage_builder(target, source, env):
   source_path, source_file = os.path.split(str(source[0]))
   target_path, target_file = os.path.split(str(target[0]))
   source_base = os.path.splitext(str(source[0]))[0]
   target_base = os.path.splitext(str(target[0]))[0]

   target_pbn = str(target[0])
   source_elf = str(source[0])   
   #source_tool = str(source[1])   
   #source_for_signature = env.get('PHYS_ADDR')


  

   #env.qurtimage(source_tool, source_elf, source_for_signature, target_pbn)
   
   
#def qurtimage(source, target, env, for_signature):
   ''' Find the "best" python interpreter to use '''
   ''' Execute the following strings one at a time,
       catching errors on a per line basis, until we get an interpreter '''
   interp = None
   for s in ['interp = env["PYTHON_INTERP"]',
             'import SCons',
             'interp = SCons.Util.python_interpreter_command()',
             'import sys',
             'interp = sys.executable',
             'interp = SCons.Util.WhereIs("python")',
             'interp = "python"']:
      if not interp:
         try:
            exec(s)
         except Exception:
            pass
            
   #qurt-image-build.py should be in one of the listed directories. Moves around due to strip/pack. Will be fixed with kernel change to hardcode location of script
   
   if   os.path.isfile( env.RealPath(QurtImageTool)   ):
		tool=env.RealPath(QurtImageTool) 
   elif os.path.isfile ( env.RealPath(QurtImageTool2) ):
		tool=env.RealPath(QurtImageTool2) 
   elif os.path.isfile ( env.RealPath(QurtImageTool3) ):
		tool=env.RealPath(QurtImageTool3)
   elif os.path.isfile ( env.RealPath(QurtImageTool4) ):
		tool=env.RealPath(QurtImageTool4)
   else:
		
		#try and search for it

		filepatterns=['qurt-image-build.py']
		sourcetool=env.RealPath("${COREBSP_ROOT}/kernel/qurt")
		for root, dirs, files in os.walk(sourcetool):
			for pattern in filepatterns:
				for file in fnmatch.filter(files, pattern):
				#import pdb; pdb.set_trace()
					currentfile = env.RealPath(os.path.join(root,file)).replace('\\','/')
					
					#don't use the mba version of the file
					if 'mba' not in currentfile:
						suitablequrttool.append(currentfile)
					

					
	
  
	#if we had to find a qurttool, use that
   if not len(suitablequrttool) < 1:
		tool=env.RealPath(suitablequrttool[0])
	
   

   
   		
   cmd = ' '.join(Flatten([interp, tool,
                            [str(w) for w in source if str(w).endswith('.elf')],
                            '-o',
                            [str(w) for w in target if str(w).endswith('.elf')],
                            #target_pbn,
                            '-p',
                            env.get('PHYS_ADDR')]))
                            
                            
   std_out, std_err, return_val = env.ExecCmds(cmd)                            

   
   
def qurtimage_emitter(target, source, env):
   ''' For each foo.elf in the sources, add foo_reloc.elf in the targets '''
   ntargets = ['_reloc.'.join(str(w).rsplit('.',1))
               for w in source if str(w).endswith('.elf')]
  
   #return (target + ntargets, source)
   return (target, source)
