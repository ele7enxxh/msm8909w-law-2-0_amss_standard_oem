'''
=============================================================================

                     I M A G E S T A T S B U I L D E R . P Y

Copyright (c) 2010,2015 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================
DESCRIPTION
    Builder to create image size statistics from .map/.elf.
=============================================================================

'''
import sys,os
import re
import optparse
from rvct_mapscan import runMapScan

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('imagestats_builder')

def generate(env):
   imageStats_generate(env)
   
def imageStats_generate(env):
   imageStats_act = env.GetBuilderAction(imageStats_builder)
   imageStats_bld = env.Builder(action = imageStats_act,
                        emitter = imageStats_emitter, 
                         suffix = '.ok')

   env.Append(BUILDERS = {'ImageStats' : imageStats_bld})
   #sys.path += [os.getcwd()]

def imageStats_emitter(target, source, env):
   env.Depends(target, source)  
   return (target, source) 
   
def imageStats_builder(env, target, source, **kwargs):
   
#   import sys
   #sys.path += [os.getcwd()]
   #from rvct_mapscan import runMapScan

   build_id = os.getenv("BUILD_ID", None)
   build_root = os.getenv("BUILD_ROOT",None)
   runMapScan("From Builder",build_id,build_root,os.path.dirname(env.RealPath(str(source[0]))),6000000,1)
   return None

#----------------------------------------------------------------------------
# Main function
#----------------------------------------------------------------------------

def main():
    print "imageStats:main"
    from rvct_mapscan import runMapScan

    usage = "usage: %prog [options] <mapfile directory>"
    cmdArgs = optparse.OptionParser(version="1.0",usage=usage)
    cmdArgs.add_option('-l','--limit',  type="int",
        help="define image size limit. If image exceeds this, exit with error")
    cmdArgs.add_option('-L','--Limit', type="int",
        help="define image size limit. If image size is below this, create file <build_id>_filesize.ok. Otherwise, exit with error")
    cmdArgs.add_option('-s','--stats', action='store_const', const=1, 
        help="save statistics .crv files")
    cmdArgs.add_option('-b','--buildid',
        help="build id")

    (options, args) = cmdArgs.parse_args()

    if len(args) != 1:
        cmdArgs.print_help()
        exit(-1)

    build_id = os.getenv("BUILD_ID", None)
    build_root = os.getenv("BUILD_ROOT",None)

    #override build id in envirnment
    if options.buildid:
        build_id = options.buildid

    if not build_id:
        # If not otherwise defined, extract build id from mapfile directory name.
        path = os.path.abspath(args[0])
        build_id = os.path.basename(path)

#    if options.limit or options.Limit:
    if options.limit:
        limit = options.limit
    else:
        limit = options.Limit
    runMapScan("From main", build_id, build_root,args[0],limit,options.stats)

if __name__ == '__main__':
    print "imageStats"
    main()

