#!/usr/bin/python
#===========================================================================
     
#  This script creates a singleimage.bin

# REFERENCES

#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/ptool/singleimage.py#1 $
#  $DateTime: 2015/09/01 00:30:35 $ 
#  $Author: pwbldsvc $

# when          who     what, where, why 
# --------      ---     ------------------------------------------------------- 
# 2011-11-10    ah      Will now also attempt to run checksparse.py if present
# 2011-09-07    ah      Added logging
# 2011-08-09    ah      Handle output directory -t, size in gb -g, more feedback
# 2011-07-26    ah      first revision

# Copyright (c) 2007-2010
# Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
# ===========================================================================*/

import struct, os, sys, getopt
import math
import re
import codecs
from types import *
import time
import subprocess as sub
from time import strftime, localtime

def reset_device_log():
    try:
        log_fp = open('log_singleimage.txt', 'w')
    except:
        print "\nERROR: Can't create the file log_singleimage.txt"
        print "This might be because the file is open and locked"
        print "Or because you are running this from a read-only location\n"
        sys.exit()

    print "\nCREATED log_singleimage.txt\n"
    log_fp.close()

def device_log(message, display=1):
    try:
        log_fp = open('log_singleimage.txt', 'a')
    except:
        print "ERROR: could not open 'log_singleimage.txt'"
        return

    try:
        log_fp.write("%s %s\n" % (strftime("%H:%M:%S", localtime()),message))
    except:
        print "ERROR: could not write to 'log_singleimage.txt'"
        return


    if display==1:
        print message

    log_fp.close()

##import pdb; pdb.set_trace()
reset_device_log()
device_log("\nsingleimage.py is running from CWD: %s\n" % os.getcwd())

def PrintBanner(sz):
    device_log("\n"+"="*78)
    device_log(sz)
    device_log("="*78+"\n")

def find_file(filename, search_paths,BailIfFileNotFound=1):

    if filename is None:
        return None

    device_log("\n\n\tLooking for '%s'"%filename)
    device_log("\t"+"-"*40)
    for x in search_paths:
        device_log("\tSearching '%s'"%x)
        temp = os.path.join(x, filename)
        if os.path.exists(temp):
            device_log("\n\t**Found %s (%i bytes)" % (temp,os.path.getsize(temp)))
            return temp

    ## search cwd last
    device_log("\tSearching '%s'"%os.getcwd())
    if os.path.exists(filename):
        device_log("\n\t**Found %s (%i bytes)" % (filename,os.path.getsize(filename)))
        return filename

    PrintBigError("\tCound't find file\n",BailIfFileNotFound)
    return None

def PrintBigError(sz,Bail=1):
    device_log("\t _________________ ___________ ")
    device_log("\t|  ___| ___ \\ ___ \\  _  | ___ \\")
    device_log("\t| |__ | |_/ / |_/ / | | | |_/ /")
    device_log("\t|  __||    /|    /| | | |    / ")
    device_log("\t| |___| |\\ \\| |\\ \\\\ \\_/ / |\\ \\ ")
    device_log("\t\\____/\\_| \\_\\_| \\_|\\___/\\_| \\_|\n")

    if len(sz)>0:
        device_log(sz)

    if Bail==1:
        device_log("\nsingleimage.py exiting - Log is log_singleimage.txt\n")
        sys.exit(1)
    else:
        device_log("\nNOT EXITING on this error, you can probably live without it\n")




def EnsureDirectoryExists(filename):
    dir = os.path.dirname(filename)

    try:
        os.stat(dir)
    except:
        os.makedirs(dir)


def Usage():
    device_log("\nUsage: Mass Storage Programmer - destructively writes data to disks!!\n")

    sudo = ""
    drive= ""
    if sys.platform.startswith("linux"):
        sudo = "sudo"
        drive= "/dev/sdb"
    else:
        device_log("\tYou must run as Administrator under Win7")
        device_log("\tYou must run as Administrator under Win7")
        device_log("\tYou must run as Administrator under Win7\n")
        drive="\\\\.\\PHYSICALDRIVE1"

    device_log("%-40s\t python singleimage.py" % "Display this info")
    device_log("%-40s\t%s python singleimage.py -x singleimage_partition_8960.xml" % ("Create normal singleimage.bin",sudo))
    device_log("%-40s\t%s python singleimage.py -d 8192 -x singleimage_partition_8960.xml" % ("Create a 4MB singleimage.bin",sudo))
    device_log("%-40s\t%s python singleimage.py -d 204800 -x singleimage_partition_8960.xml" % ("Create a 100MB singleimage.bin",sudo))
    device_log("%-40s\t%s python singleimage.py -g 4 -x singleimage_partition_8960.xml" % ("Create a 4GB singleimage.bin",sudo))
    device_log("\n%-40s\t%s python singleimage.py -s c:\windows -s d:\ -x singleimage_partition_8960.xml" % ("Search this path for files (-s)",sudo))


## ==============================================================================================
## ==============================================================================================
## ==============================================================================================
## =====main()===================================================================================
## ==============================================================================================
## ==============================================================================================
## ==============================================================================================

try:
    opts, args = getopt.getopt(sys.argv[1:], "x:s:d:g:t:", ["xml=","search_path=","size_in_sectors=","size_in_GB=","location="])
except getopt.GetoptError, err:
    # print help information and exit:
    device_log(str(err)) # will print something like "option -a not recognized"
    Usage()
    device_log("\nsingleimage.py exiting - Log is log_singleimage.txt\n")
    sys.exit(1)

search_paths    = []
partition       = None
size_in_sectors = 0
OutputFolder    = None

for o, a in opts:
    if o in ("-s", "--search_path"):
        ## also allow seperating commas
        for x in a.strip("\n").split(","):
            search_paths.append(x)
    elif o in ("-x", "--xml"):
        partition = a
    elif o in ("-d", "--size_in_sectors"):
        size_in_sectors = a
    elif o in ("-g", "--size_in_GB"):
        # i.e. 2GB = 2*1024*1024*1024
        size_in_sectors = int(a)*1024*1024*1024/512
    elif o in ("-t", "--location"):
        OutputFolder = a
        OutputFolder = re.sub(r"\\$","",OutputFolder)    # remove final slash if it exists
        OutputFolder = re.sub(r"/$","",OutputFolder)     # remove final slash if it exists

        OutputFolder += "/"     # slashes will all be corrected below

        if sys.platform.startswith("linux"):
            OutputFolder = re.sub(r"\\","/",OutputFolder)   # correct slashes
        else:
            OutputFolder = re.sub(r"/","\\\\",OutputFolder) # correct slashes

        device_log("OutputFolder='%s'"%OutputFolder)
        EnsureDirectoryExists(OutputFolder) # only need to call once
    else:
        assert False, "unhandled option"


if partition is None:
    Usage()
    device_log("\nsingleimage.py exiting - Log is log_singleimage.txt\n")
    sys.exit(1)


device_log("\nSearch Paths")
device_log(search_paths)

## Step 1 - call ptool

device_log("\t       _               _               ")
device_log("\t      | |             | |              ")
device_log("\t _ __ | |_  ___   ___ | |  _ __  _   _ ")
device_log("\t| '_ \\| __|/ _ \\ / _ \\| | | '_ \\| | | |")
device_log("\t| |_) | |_| (_) | (_) | |_| |_) | |_| |")
device_log("\t| .__/ \\__|\\___/ \\___/|_(_) .__/ \\__, |")
device_log("\t| |                       | |     __/ |")
device_log("\t|_|                       |_|    |___/ ")

ptool       = find_file("ptool.py", search_paths)

partition   = find_file(partition, search_paths)

if OutputFolder is not None:
    sz = "python %s -g 1 -x %s -t %s" % (ptool,partition,OutputFolder)
else:
    sz = "python %s -g 1 -x %s " % (ptool,partition)

PrintBanner(sz)
os.system(sz)


device_log("\n\n\t      _               _                                                ")
device_log("\t     | |             | |                                               ")
device_log("\t  ___| |__   ___  ___| | _____ _ __   __ _ _ __ ___  ___   _ __  _   _ ")
device_log("\t / __| '_ \\ / _ \\/ __| |/ / __| '_ \\ / _` | '__/ __|/ _ \\ | '_ \\| | | |")
device_log("\t| (__| | | |  __/ (__|   <\\__ \\ |_) | (_| | |  \\__ \\  __/_| |_) | |_| |")
device_log("\t \\___|_| |_|\\___|\\___|_|\\_\\___/ .__/ \\__,_|_|  |___/\\___(_) .__/ \\__, |")
device_log("\t                              | |                         | |     __/ |")
device_log("\t                              |_|                         |_|    |___/ ")

checksparse = find_file("checksparse.py", search_paths,0)
rawprogram  = find_file("rawprogram0.xml", search_paths)

if checksparse is None:
    device_log("\nPlease NOTE: Without checksparse.py you cannot check for images that are sparse\n")
    device_log("This might not be a big deal if your singleimage creation is only combining boot loaders\n")
    device_log("If however you are trying to create a complete HLOS singleimage, you MIGHT BE IN TROUBLE!!! So be WARNED!!\n")

if checksparse is not None:
    if OutputFolder is not None:
        sz = "python %s -i %s -t %s " % (checksparse,rawprogram,OutputFolder)
    else:
        sz = "python %s -i %s " % (checksparse,rawprogram)
    
    for path in search_paths:
        sz += "-s %s " % path
    
    PrintBanner(sz)
    os.system(sz)


device_log("\n\n")
device_log("\t _ __ ___  ___ _ __    _ __  _   _ ")
device_log("\t| '_ ` _ \\/ __| '_ \\  | '_ \\| | | |")
device_log("\t| | | | | \\__ \\ |_) |_| |_) | |_| |")
device_log("\t|_| |_| |_|___/ .__/(_) .__/ \\__, |")
device_log("\t              | |     | |     __/ |")
device_log("\t              |_|     |_|    |___/ ")

msp         = find_file("msp.py", search_paths)
patch       = find_file("patch0.xml", search_paths)

if OutputFolder is not None:
    sz = "python %s -r %s -p %s -d %d -t %s " % (msp,rawprogram,patch,size_in_sectors,OutputFolder)
else:
    sz = "python %s -r %s -p %s -d %d " % (msp,rawprogram,patch,int(size_in_sectors))

for path in search_paths:
    sz += "-s %s " % path

PrintBanner(sz)
os.system(sz)

device_log("\nsingleimage.py exiting - Log is log_singleimage.txt\n")
