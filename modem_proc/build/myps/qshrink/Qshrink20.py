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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/myps/qshrink/Qshrink20.py#1 $
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
   sys.path += [env.subst("${BUILD_ROOT}/build/myps/qshrink")] 

def qsr20_emitter(target, source, env):
   env.Depends(target, source)  
   return (target, source) 
   
def qsr20_builder(env, target, source, **kwargs):
   import sys
   sys.path += [env.subst("${BUILD_SCRIPTS_ROOT}/../../../qshrink/src")]
   sys.path += [env.subst("${BUILD_ROOT}/build/myps/qshrink")] 
   from QShrinkElf import runQShrink

   inputFile = env.RealPath(str(source[0]))
   hashFile=env.RealPath(str(source[1]))
   outputFile=env.RealPath(str(target[0]))
   if len(target) == 1:
     qdbFile = ""
   else:
     qdbFile=env.RealPath(str(target[1]))
   logFile = re.split('\msg_hash.txt', hashFile)[0] +"qsr2" 
   anomalyListFile = re.split('\msg_hash.txt', hashFile)[0] +"qsr2_an" 
   replace_only = 1
   use_linker_end_symbol = 1
   runQShrink(inputFile,
              outputFile,
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
                                 ['output=', 'hashfile=', 'qdbFile=', 
                                  'log=', 'discovery','discover_only',
                                  'image_end_from_elfheader',
                                  'validate'])
  optDict = {
    '--output' : [],
    '--hashfile' : [],
    '--qdbFile' : [],
    '--log' : [],
    '--discovery' : [],
    '--discover_only' : [],
    '--image_end_from_elfheader' : [],
    '--validate' : []}

  if len(args) == 0:
    print 'Usage:\n + Qshrink20.py "inputElfFile"'
    print '\t --output="outElfFile"'
    print '\t --hashfile="msg hashfile"'
    print '\t --qdbFile="Qshrink DB loaded in device"'
    print '\t --log="logfile"'
    print '\t --discovery   discover and replace mode flag, else just replacement mode'
    print '\t --discover_only  discover new strings, Elf file not modified'
    print '\t --image_end_from_elfheader  calculate image end address from elf file, else from linker end symbol'
    print '\t --validate    validate changes made'    
    exit()

  for i in opts :
    optDict[i[0]].append(i[1])
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
  
  logFile = "qsr2_" + re.split('\.', args[0])[0]
  if len(optDict['--log']) == 1 :
    logFile = optDict['--log'][0]

  anomalyListFile = logFile + "_" + "qsr2_anlist"

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
              hashFile=hashFile,
              qdbFile=qdbFile,
              logFile=logFile,
              anomalyListFile=anomalyListFile,
              replace_only_mode=replace_only,
              use_linker_end_symbol=linker_end_symbol)
  
  if validate == 1:
    from QElfValidation import validateStructs
    validateStructs(outputFile=output,hashFile=hashFile)

if __name__ == '__main__':
    main()
