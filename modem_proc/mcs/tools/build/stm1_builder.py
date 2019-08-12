# --------------------------------------------------------------------------- #
# Description   ::  Defines the SCons Builder for STM 1.0 Parser
# --------------------------------------------------------------------------- #
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tools/build/stm1_builder.py#1 $
# --------------------------------------------------------------------------- #
# Copyright (c) 2013 Qualcomm Technologies Incorporated.
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
import SCons.Scanner
import os

# --------------------------------------------------------------------------- #
#                       LOCAL VARIABLE DEFINITIONS                            #
# --------------------------------------------------------------------------- #
#Uncomment the next line and remove below dynamic path changes at a later date
#STM_DIR = "${BUILD_ROOT}/mcs/stm/tools"

#------------------------------------------------------------------------------
#                      SCONS HOOKS (exist/generate)
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('stm1_builder')

def generate(env):
   stm_act = env.GetBuilderAction(stm_builder)
   stm_bld = env.Builder(action = stm_act, 
                             source_scanner = SCons.Scanner.C.CScanner(),
                             emitter = stm_emitter,
                             suffix = '.smh', src_suffix = '.smf')

   env.Append(BUILDERS = {'STM1' : stm_bld})


#-------------------------------------------------------------------------------
# STM1 Compiler emitter function
#
def stm_emitter(target, source, env):
    # Iterate through the list of targets looking for the SMH and SMT files
    smt_file = None
    smh_file = None
    for tgt in target:
        env.PrintDebugInfo('STM1', "tgt: " + str(tgt))
        ext = os.path.splitext(str(tgt))[1]
        if ext == '.smh':
            smh_file = str(tgt)
        if ext == '.smt':
            smt_file = str(tgt)

    env.PrintDebugInfo('STM1', "smh: " + smh_file)
    env.PrintDebugInfo('STM1', "smt: " + smt_file)
    # Add the SMI and TLA files
    base = os.path.splitext(smh_file)[0]
    smi_file = base + '.smi'
    tla_file = base + '.tla'

    # Preprocess the SMF -> SMI
    smi = env.PreProcess(smi_file, source[0])
    env.Depends(target, smi)

    # Update the target list
    target = [env.File(smh_file),
              env.File(smt_file),
              env.File(tla_file),
              ]
    # Update the source list
    source.append(smi)
    
    return (target, source)


#-------------------------------------------------------------------------------
# STM1 Compiler builder function
#
def stm_builder(target, source, env):

    if env.PathExists('${BUILD_ROOT}/mcs/stm/tools'):
        STM_DIR = '${BUILD_ROOT}/mcs/stm/tools'
    else:    
        STM_DIR = '${BUILD_ROOT}/modem/mcs/stm/tools'

    smf_file  = str(source[0])
    smi_file  = str(source[1])
    smh_file  = str(target[0])
    smt_file  = str(target[1])
    tla_file  = str(target[2])

    if env['PLATFORM'] == 'linux':
        cmd = "perl " + STM_DIR + "/smcomp.pl"
    else:
        cmd = STM_DIR + "/smcomp.exe"

    # build command
    output_dir = os.path.dirname(smh_file)
    stm_cmd = cmd + ' -t -a tla -d ' + output_dir + ' ' + smi_file
    
    # run command(s)
    data, err, rv = env.RunCmds([env.File(smh_file), env.File(smt_file), env.File(tla_file)],
                                [env.File(smi_file)],
                                stm_cmd)
