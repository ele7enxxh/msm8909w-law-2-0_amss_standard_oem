#===============================================================================
#
# oem_pack.py
#
# GENERAL DESCRIPTION
#     Prepare a build for distribution to Sub-Contractors/Sub-Licensees.
#
#-------------------------------------------------------------------------------
#
#  Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.
#  All Rights Reserved.
#  QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/ms/oem_pack.py#1 $
#  $DateTime: 2016/12/13 07:59:25 $
#  $Change: 11985153 $
#
#===============================================================================
'''%prog [options]

     Remove L1, LTE and TD-SCDMA source, but not the object files.

     Procedure for running this script:

          1) Create a copy of your build to be packed by this script.

          2) Build "copy" to create necessary binary files.  This works for one
              build variant at a time.

          3) Run this script (oem_pack.py) from the build/ms directory with no
              parameters.

          4) Make a second copy of this build for compile testing.

          5) Test compile this new copy.

          6) Ship the first copy.'''

import sys
import os
import subprocess
import time
from optparse import OptionParser
from glob import glob

#######################################################################
# Exception for handling input errors.
#######################################################################
class InputError(Exception):
    def __init__(self, value):
        self.value = "oem_pack.py: Input Error: " + value
    def __str__(self):
        return repr(self.value)
# End of class InputError

#######################################################################
# Exception for handling Build errors.
#######################################################################
class BuildError(Exception):
    def __init__(self, value):
        self.value = "oem_pack.py: Build Error: Build returned " + str(value)
    def __str__(self):
        return repr(self.value)
# End of class InputError

#######################################################################
# Exception for handling unknown errors.
#######################################################################
class UnknownError(Exception):
    def __init__(self):
        self.value = "oem_pack.py: Unknown error."
    def __str__(self):
        return repr(self.value)
# End of class InputError


#######################################################################
# Log a message
#######################################################################
def log (*args):
    print "oem_pack.py:", ' '.join(args)
# end of log()

#######################################################################
# Detect the build command
#######################################################################
def detect_build_command ():

    log ("Trying to auto-detect build command.")

    # Start by seeing if there is a latest_command.txt
    if os.path.exists('latest_command.txt'):
        lcmd = open('latest_command.txt', 'r')
        build_command = lcmd.readline()
        lcmd.close()

        # Now prepend "build" or "build.sh" depending on the platform
        if 'linux' in sys.platform:
            build_command = 'build.sh ' + build_command
        else:
            build_command = 'build.cmd ' + build_command

        log ("Build command is:", build_command)
        return build_command

    else:
        raise InputError("Couldn't find latest_command.txt")

    # Code should never get here.
    raise UnknownError()

# End of detect_build_command()


#######################################################################
# Insert uses flags into the build command line.
#######################################################################

def insert_uses_into_build_cmd (build_cmd, uses):

    if type(uses) is not list:
        uses = [uses]

    if 'USES_FLAGS' in build_cmd:
        # Need to add flags to existing USES_FLAGS

        build_cmd_start, build_cmd_end = build_cmd.split('USES_FLAGS')

        # Strip leading spaces and '=' sign from build_cmd_end
        build_cmd_end = build_cmd_end.lstrip(' =')

        # Separate USES flags from anything after USES flags
        if ' ' in build_cmd_end:
            cmd_line_uses, build_cmd_end = build_cmd_end.split()
            # Get the space back
            build_cmd_end = ' ' + build_cmd_end
        else:
            cmd_line_uses = build_cmd_end
            build_cmd_end = ''

        # Turn cmd_line_uses into a list, and remove USES_NO_STRIP_NO_ODM, 
        # if present
        cmd_line_uses = cmd_line_uses.split(',')
        for i, u in enumerate(cmd_line_uses[:]):
            if u == 'USES_NO_STRIP_NO_ODM':
                del cmd_line_uses[i]

        # Put it all together
        uses.extend(cmd_line_uses)
        build_cmd = build_cmd_start + 'USES_FLAGS=' + ','.join(uses) + build_cmd_end
    else:
        # Can just append the USES_FLAGS to the end of the command line
        build_cmd = build_cmd + ' USES_FLAGS=' + ','.join(uses)

    return build_cmd

# End of insert_uses_into_build_cmd()


#######################################################################
# Remove USES flags
#######################################################################
def remove_uses_flags (uses_flags, no_exec):
    # USES flags are defined in tcfg_<bid>.py scripts (where 'bid' stands for
    # "Build ID".  This function parses all of the tcfg_*.py scripts in the
    # current directory, and removes any references to the specified uses_flags.

    for tcfg_file in glob("tcfg_*.py"):
        tcfg = open(tcfg_file, 'r')
        tcfg_contents = tcfg.read()
        tcfg.close()

        for uses_flag in uses_flags:
            # Remove the specifified USES flag
            log ("Removing", uses_flag, "from", tcfg_file)
            tcfg_contents = tcfg_contents.replace("   env.AddUsesFlags('%s', from_builds_file = True)\n" % uses_flag, "")

        if not no_exec:
            tcfg = open(tcfg_file, 'w')
            tcfg.write(tcfg_contents)
            tcfg.close()

# end of remove_uses_flags()

#######################################################################
# Main function.
#######################################################################
def main (build_command, no_exec):

    ################################
    # Save the start time
    ################################
    start_time = time.time()

    ################################
    # Try to detect the build command, if it isn't specified
    ################################
    if build_command is None:
        build_command = detect_build_command()

    ################################
    # Remove USES_COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS
    ################################
    remove_uses_flags(['USES_COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS'],
                      no_exec)

    ################################
    # Execute a clean command
    ################################
    clean_command = build_command + " --clean"
    if no_exec:
        clean_command += " --no-exec"
#    log ("Executing:", clean_command)
#    ret = subprocess.call(clean_command.split())
#    if ret != 0:
#        raise BuildError("Build returned " + str(ret))

    ################################
    # Execute the cleanpack command
    ################################
    cleanpack_command = clean_command + " --cleanpack --force-build --log_name=build_oem_pack"
    cleanpack_command = insert_uses_into_build_cmd(cleanpack_command,
                                                   'CLEANPACK_TARGET')
    log ("Executing:", cleanpack_command)
    ret = subprocess.call(cleanpack_command.split())
    if ret != 0:
        raise BuildError("Build returned " + str(ret))

    ################################
    # Now cleanup build artifacts
    ################################

    # Remove files generated by the build
    for root, dirs, files in os.walk('../..'):
        for bfile in files:
            if (bfile.endswith('.pyc') or
                 bfile.endswith('.md5') or
                 bfile.endswith('.dblite') or
                 bfile == 'ppLkCmd.log' or
                 # Should be removed once QC-SCons can clean these files.
                 bfile.endswith('.umid') or
                 # Should be removed once QC-SCons can clean these files.
                 bfile.endswith('.i')
                ):
                if not no_exec:
                    os.remove(os.path.join(root, bfile))

    # Remove empty directories (e.g. directories that had binaries
    # that have since been cleaned.)
    for root, dirs, files in os.walk('../..', topdown=False):
        for dir_name in dirs:
            dir_path = os.path.join(root, dir_name)
            if os.path.exists(dir_path):
                if not os.listdir(dir_path): #to check whether the dir is empty
                    os.removedirs(dir_path)

    ################################
    # All Done!
    ################################

    delta_time = time.time() - start_time
    minutes = delta_time / 60
    seconds = delta_time % 60
    delta_time_str = "%d minutes, %d seconds" % (minutes, seconds)
    log ("oem_pack.py: Completed.  Elapsed time:", delta_time_str)

# End of main()


#######################################################################
# Allow this script to be called from the OS command shell
#######################################################################
if __name__ == "__main__":

    #######################################################
    # Print useful debugging info
    #######################################################

    log ("Start directory is:", os.getcwd())
    log ("Python Version is:", sys.version)
    log ("Platform is:", sys.platform)
    log ("Script Version is: $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/ms/oem_pack.py#1 $")
    if len(sys.argv) == 1:
        log ("No parameters")
    else:
        log ("Parameters:")
        for arg in sys.argv[1:]:
            log ("  ", arg)

    #######################################################
    # Parse parameters
    #######################################################

    opt_parser = OptionParser(usage = __doc__)
    opt_parser.add_option("-b", "--build_command", dest="build_command", 
        help="Build command.  If not specified, the tool will attempt to auto-detect the \
build command from the latest_command.txt file.")
    opt_parser.add_option("-n",
                          "--no_exec",
                          action='store_true',
                          dest="no_exec",
                          default=False,
                          help="Don't do anything, just print out a log of what\
                           would have been done (build log files are actually created).")

    (options, args) = opt_parser.parse_args()

    main(options.build_command, options.no_exec)
