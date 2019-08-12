#! /usr/bin/python

# This script generates single bootloader image from the following images
# sbl1, modem, rpm, tz, and emmc_appsboot
# 
# It is assumed that this script is run from root of meta build.
# make_image_tool.py aboot <apps path>/emmc_appsboot.mbn tz <tz path>\tz.mbn sbl1 <boot path>/sbl1.mbn modem <meta path>\NON-HLOS.bin rpm <rpm path>/rpm.mbn -v <version>

import sys
import os
import subprocess
import pprint
from ctypes import *
import struct
from optparse import OptionParser

  
sys.path.append('../tools/meta')
import meta_lib as ml
import meta_log as lg 


logfilename = os.path.join("gen_bootloader_image")
print logfilename

lg=lg.Logger(logfilename)
lg.log("Current working directory" + os.getcwd())

# Log the OS environment
lg.log("OS Environment:")
for var in os.environ:
   lg.log("   " + var + ' = ' + os.environ[var])
   

mi = ml.meta_info(logger=lg)

var_list = {}
var_list_1 = mi.get_file_vars(attr="bootloader_image")
for dict in [var_list_1]:   
    for k, v in dict.iteritems():
        var_list.setdefault(k,list())
        var_list[k] += v

command_line_arg=" "

for var in var_list:
   for file in var_list[var] :  
      file_path, file_name_w_ext = os.path.split(file) 
      file_name = os.path.splitext(file_name_w_ext)[0]
      partition_name = var
      #print "partiton name:",partition_name," File path:", file
      #lg.log("partiton name:" + partition_name + " File path:"+ file)
      #file = file.replace('/','\\')
      command_line_arg += "".join(partition_name)+" "+"".join(file)+" "

version_id = mi.get_build_id('common', 'builds_flat')
command_line_arg += "-v " + version_id

#print "command_line_arg:", command_line_arg
lg.log("command_line_arg:" + command_line_arg)

#getting apps build path
build_path = mi.get_build_path("apps");
lg.log("apps build path:" + build_path)

lg.log_exec(['python', build_path +'/LINUX/android/device/qcom/msm8909w/common/meta_image/make_image_tool.py', command_line_arg])
