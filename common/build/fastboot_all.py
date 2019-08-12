
#*****************************************************************************/
#
# Fastboot All Utility
#
# Version information
# $Header: //source/qcom/qct/tools/meta/00/01/common/build/fastboot_all.py#8 $
# $DateTime: 2014/06/19 14:17:54 $
#
#*****************************************************************************

'''fastboot_all.py:

Description:
Run the fastboot_all utility on the required images.

Usage:
      python fastboot_all.py [apps_path] [result_path] -k

      apps_path:
         An optional parameter to the root of the apps build.
            
      result_path:
         An optional parameter indicating the path where success/failure .txt of fastboot would be created.

      -k / --kl:
         Use one of these flags to set the "keep_local" option.
         This skips the cleanup of copied files at the end.
'''

#---------------------------------------------------------#
# Import libraries                                        #
#---------------------------------------------------------#
import sys
import os
import subprocess
import time
import signal
import tempfile
import shutil
from optparse import OptionParser
from glob import glob

#---------------------------------------------------------#
# Define a handler to exit when Ctrl+C is pressed         #
#---------------------------------------------------------#
def interrupt_handler(signum, frame):
   sys.exit("Exiting")

#---------------------------------------------------------#
# Function to handle fastboot failure case                #
#---------------------------------------------------------#
def fastboot_check(stuff):
   if stuff.count('OKAY') < 2:
      print "Failed to flash images"
      print "Exiting"
      fastboot_file_create("FAILURE_FASTBOOT")
      print "This could be a network latency issue, please retry with command : \nfastboot_all.py --copy_local"
      time.sleep(60)
      sys.exit("Exiting")
   return

#---------------------------------------------------------#
# Function to create success/failure fastboot files       #
#---------------------------------------------------------#
def fastboot_file_create(result):
   if result_path:
      outcome_fastboot = os.path.join(result_path,result + '.txt')
      FILE = open(outcome_fastboot,'w')
      FILE.close()
   return

#---------------------------------------------------------#
# Function to get fastboot.exe binary from apps           #
#---------------------------------------------------------#
def get_fastboot(la_path):   
   if sys.platform.find("win") != -1:
      print "Executing fastboot on windows"
      fastboot = os.path.join(la_path, 'vendor/qcom/proprietary/usb/host/windows/prebuilt/fastboot.exe')
      if not os.path.exists(fastboot):
         fastboot = 'fastboot.exe'
         print "Cannot find fastboot.exe in Apps path. Host environment's fastboot.exe will be used"
      else:
         print "Making use of fastboot.exe found in APPS path :\n"+fastboot
   elif sys.platform.find("linux") != -1:
      print "Executing fastboot on Linux"
      fastboot = os.path.join(la_path,'out/host/linux-x86/bin/fastboot')
      if not os.path.exists(fastboot):
         fastboot = 'fastboot'  
         print "Cannot find fastboot in Apps path. Host environment's fastboot will be used" 
      else:
         print "Making use of fastboot found in APPS path :\n"+fastboot
   return fastboot

#---------------------------------------------------------#
# Function to verify the target state                     #
#---------------------------------------------------------#
def verify_fastboot(fastboot_dir):
   retry_count = 0
   print "\nfastboot_all.py: Checking target state."
   while 1:
      #check if fastboot utility is available. If not, ask user to install it.
      try:
         stuff = subprocess.Popen([fastboot, 'devices'], cwd=fastboot_dir, stdout=subprocess.PIPE).stdout.read()
      except:
         print "\nFastboot is not installed."
         print "Please install fastboot utilities and add to system path. Then reload the build."
         fastboot_file_create("FAILURE_FASTBOOT")
         return False
      #check if it's in fastboot mode      
      if stuff.count('fastboot') >= 1:
         break
      else:
         print  "Please check if USB is connected."
         print  "If USB is connected, Power cycle the device to fastboot."
         print  "Retry attempt ",retry_count
         time.sleep(20)
         retry_count += 1
         if retry_count == 6:
            print  "Apps Boot Loader is not in the FastBoot state."
            print  "Retry Timeout !!! Exiting"
            fastboot_file_create("FAILURE_FASTBOOT")
            return False
   return True
#---------------------------------------------------------#
# Function to flash apps images                           #
#---------------------------------------------------------#
def fastboot_flash_apps(partition_name,file):
   if options.write_to_temp_file :
      fastboot_file.write(fastboot+ ' flash ' + partition_name + ' ' + file + '\n')
      print "Copying .. ", file
      shutil.copy(file,fastboot_dir)
   else: 
      print "Loading ", os.path.split(file)[1]," in to ",partition_name," partition. Please wait..." 
      stuff = subprocess.Popen([fastboot, 'flash', partition_name,file], stdout=subprocess.PIPE,stderr=subprocess.STDOUT).stdout.read()
      print file
      print stuff
      fastboot_check(stuff)
   return

#-----------------------------------------------------------#
# Function to remove files that doesn't have selected storage_type
#-----------------------------------------------------------#
def storage_type_filter(file_list,storage_type) :
   if storage_type == "emmc" :
      not_storage_type = "ufs"
   elif storage_type == "ufs" :
      not_storage_type = "emmc"
   else :                                     # Should not be landed here
      print "Unknown storage type specified. Exiting."  
      sys.exit(0)
   file_list_out = {}
   for each_key in file_list :
      file_list_out[each_key]=list()
      for each_file in file_list[each_key] :
         if not_storage_type in each_file : 
            continue
         else : 
            file_list_out[each_key].append(each_file)
   return file_list_out



#---------------------------------------------------------#
# Function to clean-up the local temp directory           #
#---------------------------------------------------------#
def fastboot_cleanup(k):
   if k:
      print "\nfastboot_all.py: Option to keep files in local directory"
      print "Binaries and fastboot files stored in temp folder", fastboot_dir
   else:
      print "\nfastboot_all.py: Cleaning up files..."
      print "Deleting the temp folder", fastboot_dir
      shutil.rmtree(fastboot_dir)

#---------------------------------------------------------#
#-------------------      MAIN      ----------------------#
#---------------------------------------------------------#

parser = OptionParser()
parser.add_option("--ap","-a",action="store", type="string",dest="apps_path",help="APPS PATH")
parser.add_option("--rp","-r",action="store", type="string",dest="result_path",help="RESULT PATH")
parser.add_option("--pf","-p",action="store", type="string",dest="product_flavor",help="PRODUCT FLAVOR")
parser.add_option("--kl","-k",action="store_true", dest="keep_local", default=False, help="KEEP LOCAL/TEMP DIRECTORY CONTAINING REQUIRED BINARIES")
parser.add_option("--st","-s",action="store", type="string",dest="storage_type",help="STORAGE TYPE")
parser.add_option("--copy_local",action="store_true", dest="write_to_temp_file", default=False, help="CREATE A LOCAL/TEMP FOLDER TO COPY REQUIRED BINARIES")
(options, args) = parser.parse_args()

storage_type=''
#---------------------------------------------------------#
# Print some diagnostic info                              #
#---------------------------------------------------------#
print "Platform is:", sys.platform
print "Python Version is:", sys.version
print "Current directory is:", os.getcwd()

#---------------------------------------------------------#
# Load the Meta-Info file                                 #
#---------------------------------------------------------#
print "\nfastboot_all.py: Loading the Meta-Info file"
sys.path.append(os.path.join(os.path.dirname(__file__) + '/../tools/meta'))
import meta_lib as ml
mi = ml.meta_info()

#---------------------------------------------------------#
# Process input parameters #
#---------------------------------------------------------#
print "\nfastboot_all.py: Finding paths"
if options.apps_path or options.result_path or options.product_flavor or options.keep_local or options.storage_type or options.write_to_temp_file:
   if options.apps_path:
      app = True
      apps_path = options.apps_path
      print "New Parameters Specified. Apps path:"+apps_path
   else:
      app = False
      apps_path = mi.get_build_path('apps')
   if options.storage_type :
      storage_type = options.storage_type
      print "Specified storage type: "+storage_type
   result_path = options.result_path
   product_flavor = options.product_flavor
else:
   if len(sys.argv) >= 2 :
      app = True
      apps_path = sys.argv[1]
      print "New Parameters Specified. Apps path:"+apps_path
   else:
      app = False
      apps_path = mi.get_build_path('apps')
   if len(sys.argv) >= 3 :
      result_path = sys.argv[2]
   else:
      result_path = None
   product_flavor = options.product_flavor

#------------------------------------------------------------------------------------#
# Create local temp directory 'fastboot_dir'                                         #
# (or) Point 'fastboot_dir' to current working directory                             #
#------------------------------------------------------------------------------------#
if options.write_to_temp_file: 
   temp_dir = tempfile.gettempdir()
   
   try:
      fastboot_dir = os.path.join(temp_dir, 'fastboot_' + mi.get_build_id('common','builds_flat'))
   except TypeError:   #Exception because of empty BuildID i.e., minimized build 
       fastboot_dir = os.path.join(temp_dir, 'fastboot_all')
   print "\nfastboot_all.py: Creating temp directory", fastboot_dir, "to copy fastboot files locally"
   try:
      os.mkdir(fastboot_dir)
   except OSError:
      print "Directory with same name exists"
      print "Cleaning up and recreating", os.path.basename(fastboot_dir)
      try: 
         shutil.rmtree(fastboot_dir)
         os.mkdir(fastboot_dir)
      except:
         print "\nUnable to create temp directory. Hence skip local copying of binaries.\n"
         options.write_to_temp_file = False
else : 
   fastboot_dir = os.getcwd()
#---------------------------------------------------------------------#
# Check for existence of LINUX/android folder structure in APPS path  #
#---------------------------------------------------------------------#
la_path = os.path.join(apps_path, 'LINUX/android')
if os.path.exists(la_path):
   la_path_exists=1
else:
   la_path_exists=0
   la_path =apps_path

#---------------------------------------------------------------------------------------#
# Obtain fastboot.exe path into 'fastboot' varible and update the fastboot_dir          #
#---------------------------------------------------------------------------------------#
fastboot = get_fastboot(la_path)   
if ( '/' in fastboot or '\\' in fastboot ) and (options.write_to_temp_file):
   shutil.copy(fastboot,fastboot_dir)
   fastboot = os.path.join(fastboot_dir,'fastboot.exe')
elif ( '/' in fastboot or '\\' in fastboot ) :
   fastboot_dir = os.path.split(fastboot)[0]
else:                                             #fastboot is not found in APPS path. Falling back to local environment
   fastboot = 'fastboot.exe'  

common_path = mi.get_build_path('common')
common_path = os.path.join(common_path,'common/build')
print "\nApps path is:  ", apps_path
print "Common path is:", common_path
#print 'current working directory :\n '+fastboot_dir+'\n'

#check for emmc path. If exists make emmc as default storage_type
if (storage_type=='') and (os.path.exists(os.path.join(common_path,'emmc','bin'))) :
   storage_type = 'emmc'
   common_path=os.path.join(common_path,'emmc')
   print "\nDefault storage_type emmc detected."
#This is a temporary fix; to make a default flavor for targets which got different storage_types.

img_failure = ''

#---------------------------------------------------------#
# Create a temp. file to write in fastboot commands       #
#---------------------------------------------------------#
if (options.write_to_temp_file) :
   fastboot_file = tempfile.NamedTemporaryFile(prefix='fastboot_flash_', dir=fastboot_dir, delete=False)


#---------------------------------------------------------#
# Verify that device is in fastboot mode                  #
#---------------------------------------------------------#

fastboot_device_detected = verify_fastboot (fastboot_dir)
if not(fastboot_device_detected) :
   sys.exit(0)
else: 
   print "\nFound device in Fastboot mode. Flashing the partitions...\n"
#------------------------------------------------------------------------------------------------------# 
# Obtain relative paths of files that are needed to be fastboot'ed.
# If user specifies an apps path, update build path in meta-info object with user specified  apps path
# If fastboot attribute is 'true': 
#     name of partition is name of binary
# Else : 
#     name of partition is value of 'fastboot attribute'
#------------------------------------------------------------------------------------------------------#

var_list = mi.get_file_vars(attr='fastboot',flavor=product_flavor,abs=True)

apps_file_list_rel = mi.get_files(build='apps',attr='fastboot',abs=False)

if storage_type :
   var_list = storage_type_filter(var_list,storage_type)

for var in var_list:
   for file in var_list[var] :  
      if (mi.get_build_path('apps') in file ) and (app == True):  
         file_path_rel=file.replace(mi.get_build_path('apps'),'')  
         file_name_w_ext = os.path.split(file)[1]
         if any(file_name_w_ext in each_rel_path for each_rel_path in  apps_file_list_rel) :
            if ('LINUX/android/' in file_path_rel) and not(la_path_exists) : 
               file_path_rel=file_path_rel.replace('LINUX/android/','')
            file=os.path.join(apps_path,file_path_rel)
      file_path, file_name_w_ext = os.path.split(file) 
      file_name = os.path.splitext(file_name_w_ext)[0]
      partition_name = file_name if (var == 'true') else var
      #print "Loading ", file_name_w_ext," in to ",partition_name," partition. Please wait..." 
      fastboot_flash_apps(partition_name,file)


if fastboot_device_detected and options.write_to_temp_file:
   print "\nfastboot_all.py: Fastboot Device detected. Running fastboot flash commands: \n"
   fastboot_file.seek(0)
   for cmd in fastboot_file:
      #print "Executing", cmd.strip()
      stuff = subprocess.Popen(cmd.split(), cwd=fastboot_dir, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read()
      print stuff
      fastboot_check(stuff)
   fastboot_file.close()
   fastboot_cleanup(options.keep_local)

#---------------------------------------------------------#
# Fastboot images                                         #
# Signal the interrupt handler to handle ctrl + C         #
#---------------------------------------------------------#
signal.signal(signal.SIGINT, interrupt_handler)
fastboot_file_create("SUCCESS_FASTBOOT")
if img_failure != '':
   print "\nCannot find " + img_failure 
print "fastboot_all.py: Loading complete,Window will be closed in 1 minute"
print "Hit Ctrl+C to exit"
time.sleep(60)
