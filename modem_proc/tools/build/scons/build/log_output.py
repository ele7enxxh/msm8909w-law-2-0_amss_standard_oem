'''Log Output'''
#===============================================================================
#
# Log Ouput
#
# GENERAL DESCRIPTION
#    Split output from a command to both stdout and a log file.
# Copyright (c) 2011-2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tools/build/scons/build/log_output.py#1 $
#  $DateTime: 2016/12/13 07:57:53 $
#  $Change: 11985114 $
#
#===============================================================================

USAGE = '''log_ouput.py:

Usage:

      log_ouput.py <log_file_name> <command_to_execute>

   Example:

      log_output.py build-log.txt -- %~dp0..\SCons\scons %*
'''

import sys
import subprocess

try:
    LOG_FN = sys.argv[1]
    CMD = sys.argv[3:]
except:
    print "Error, Invalid Command Line:", sys.argv, '\n'
    print USAGE 
    sys.exit(1)

LOG = open(LOG_FN, 'a')

TASK = subprocess.Popen(CMD, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
CHAR = TASK.stdout.read(1)
while CHAR:
    LOG.write(CHAR)
    sys.stdout.write(CHAR)
    CHAR = TASK.stdout.read(1)
TASK.wait()

LOG.close()
sys.exit(TASK.returncode)
