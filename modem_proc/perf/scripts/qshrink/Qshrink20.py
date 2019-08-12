'''
=============================================================================

                          Q S H R I N K 3 0. P Y

Copyright (c) 2010 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================
DESCRIPTION
    Interface for QShrinkElf.py when called from a build.  Also handles
    command-line options.
=============================================================================

=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/perf/scripts/qshrink/Qshrink20.py#1 $
$Date: 2016/12/13 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13/03/15   sh      Added QShrink 4.0 support
13/03/13   sh      Added QSHrink 3.0 support
11/04/11   sh      Added SCONS script support
07/21/11   sh      QShrink20 commercial version release
08/11/10   kf      now long option names for build log readability
07/21/10   jw      Initial Version

=============================================================================
'''
import getopt, sys
import re

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('qsr20_builder')

def generate(env):
   qsr20_generate(env)
   
def  qsr20_generate(env):
   qsr20_act = env.GetBuilderAction(qsr20_builder)
   qsr20_bld = env.Builder(action = qsr20_act,
                        emitter = qsr20_emitter)

   env.Append(BUILDERS = {'Qshrink20' : qsr20_bld})
   # BUILD_SCRIPTS_ROOT: the invoking script dir, in our case the dir that
   # contains pplkcmd.py
   sys.path += [env.subst("${BUILD_SCRIPTS_ROOT}/../../../qshrink/src")]
   sys.path += [env.subst("${BUILD_ROOT}/scripts/myps/qshrink")] 

def qsr20_emitter(target, source, env):
   env.Depends(target, source)  
   return (target, source) 
   
def qsr20_builder(env, target, source, **kwargs):
   import os, sys, inspect
   sys.path += [env.subst("${BUILD_SCRIPTS_ROOT}/../../../qshrink/src")]
   sys.path += [env.subst("${BUILD_ROOT}/scripts/myps/qshrink")] 
   sys.path += [
     os.path.abspath(
       os.path.split(
         inspect.getframeinfo(
           inspect.currentframe()
         ).filename
       )[0]
     )
   ]
   from QShrinkElf import runQShrink

   inputFile = env.RealPath(str(source[0]))
   hashFile=env.RealPath(str(source[1]))
   buildOutputPath = ""
   logFile = ""
   anomalyListFile = ""
   buildOutputPath=env.RealPath("${BUILD_ROOT}/build/myps/qshrink")
   outputFile=env.RealPath(str(target[0]))
   if len(target) == 1:
     qdbFile = ""
   else:
     qdbFile=env.RealPath(str(target[1]))
   replace_only = 1
   use_linker_end_symbol = 1
   runQShrink(inputFile,
              outputFile,
              buildOutputPath,
              hashFile,
              qdbFile,
              logFile,
              anomalyListFile,
              replace_only,
              use_linker_end_symbol) 
   return None

#----------------------------------------------------------------------------
# Main function
#----------------------------------------------------------------------------

def main():
  from QShrinkElf import runQShrink

  opts, args = getopt.gnu_getopt(sys.argv[1:], "",
                                 ['output=','buildOutputPath=', 'hashfile=', 'qdbFile=', 
                                  'log=', 'discovery','discover_only',
                                  'update_linker_script','mapFile=', 'lcsFile=', 
                                  'image_end_from_elfheader',
                                  'validate'])
  optDict = {
    '--output' : [],
    '--buildOutputPath' : [],
    '--hashfile' : [],
    '--qdbFile' : [],
    '--log' : [],
    '--discovery' : [],
    '--discover_only' : [],
    '--update_linker_script': [],
    '--mapFile' : [],
    '--lcsFile' : [],
    '--image_end_from_elfheader' : [],
    '--validate' : []}

  for i in opts :
    optDict[i[0]].append(i[1])

  update_linker_script_opt = 0

  if len(opts) !=0:
    if  len(optDict['--update_linker_script']) == 1:
       update_linker_script_opt = 1
       if ((len(optDict['--mapFile']) != 1)  or
          (len(optDict['--lcsFile']) != 1) ):
          print 'Usage:\n + Qshrink20.py --update_linker_script  --mapFile=<mapFile> --lcsFile=<lcsFile>'
          print '\t PS: Input elf file not needed'
          exit()
       else:
          if len(optDict['--update_linker_script']) == 1:
             mapFile = optDict['--mapFile'][0]
             lcsFile = optDict['--lcsFile'][0]
             from QElfValidation import updateLinkerScripts
             updateLinkerScripts(mapFile=mapFile,lcsFile=lcsFile)
             exit(0)

   
  if (update_linker_script_opt ==0) and len(args) == 0:
          print 'Usage:\n + Qshrink20.py "inputElfFile"'
          print '\t --output="outElfFile"'
          print '\t --buildOutputPath="output Directory for logs,generated hashfile"'
          print '\t --hashfile="msg hashfile"'
          print '\t --qdbFile="Qshrink DB loaded in device"'
          print '\t --log="logfile"'
          print '\t --discovery   discover and replace mode flag, else just replacement mode'
          print '\t --discover_only  discover new strings for QShrink 2.0/3.0, Elf file not modified'
          print '\t --update_linker_script   Option for updating linker script using mapfile, linker script as input'
          print '\t --image_end_from_elfheader  calculate image end address from elf file, else from linker end symbol'
          print '\t --validate    validate changes made'    
          exit()

  for k in optDict :
    assert len(optDict[k]) <= 1

  output = args[0] + ".shrunk"
  if len(optDict['--output']) == 1 :
    output = optDict['--output'][0]

  hashFile = "msg_hash.txt"
  if len(optDict['--hashfile']) == 1 :
    hashFile = optDict['--hashfile'][0]

  qdbFile= ""
  if len(optDict['--qdbFile']) == 1 :
    qdbFile = optDict['--qdbFile'][0]
  
  logFile = ""
  if len(optDict['--log']) == 1 :
    logFile = optDict['--log'][0]

  buildOutputPath = ""
  if len(optDict['--buildOutputPath']) == 1 :
    buildOutputPath = optDict['--buildOutputPath'][0]

  anomalyListFile = ""

  replace_only = 1
  if len(optDict['--discovery']) == 1:
    replace_only = 0

  if len(optDict['--discover_only']) == 1:
    replace_only = 2

  linker_end_symbol = 1
  if len(optDict['--image_end_from_elfheader']) == 1:
    linker_end_symbol = 0

  validate = 0
  if len(optDict['--validate']) == 1:
    validate = 1

  runQShrink(elfFile=args[0],
              outputFile=output,
              buildOutputPath=buildOutputPath,
              hashFile=hashFile,
              qdbFile=qdbFile,
              logFile=logFile,
              anomalyListFile=anomalyListFile,
              replace_only_mode=replace_only,
              use_linker_end_symbol=linker_end_symbol)
 
  if validate == 1:
    from QElfValidation import validateStructs
    validateStructs(outputFile=output,hashFile=hashFile,qdbFile=qdbFile)

if __name__ == '__main__':
    main()
