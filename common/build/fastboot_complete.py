
#*****************************************************************************/
#
# Fastboot All Utility
#
# Version information
# $Header: //source/qcom/qct/tools/meta/00/01/common/build/fastboot_complete.py#9 $
# $DateTime: 2014/10/08 21:51:31 $
#
#*****************************************************************************

'''fastboot_complete.py:

Description:

fastboot_complete.py flashes all the binaries that are required for for an android Boot up.

Usage:

      python fastboot_complete.py [--ap=apps_path] [--rp=result_path] [--pf=product_flavor] [--st=storage_type] [-w] [--sn=serial_number]

      apps_path:
         An optional parameter '--ap=<apps_root_path>' to the flash user specified apps build.
            
      result_path:
         An optional parameter '--rp=<destination_of_result_path>' indicating the path where success/failure .txt of fastboot would be created

      product_flavor:
         An optional parameter '--pf=<product_flavor>' that specifies the flavor of the build needed to be flashed.
        
      storage_type:
         An optional parameter '--st=<storage_type>' to specify a storage_type (like ufs / emmc ). If not specified emmc will be selected as default.

      wait_enable:
         An optional parameter '-w'; if present in the arguments , the wait of 60 secs (in the end) will be enabled.

	 serial_number:
	     An optional parameter '--sn=<serial_number>' to specify a serial number of a fastboot device, to which the loading process should be targeted to.
'''


#---------------------------------------------------------#
# Import libraries                                        #
#---------------------------------------------------------#
import sys
import os
import subprocess
import time
import signal
from optparse import OptionParser
from glob import glob
sys.path.append(os.path.join(os.path.dirname(__file__),'../tools/meta'))
import meta_lib as ml
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
# Function to flash apps images                           #
#---------------------------------------------------------#
def fastboot_flash_apps(img_name,img_path):
   if options.serial_number :
      fastboot_command = [fastboot,'-s',serial_number,'flash', img_name,img_path]
   else :
      fastboot_command = [fastboot, 'flash', img_name,img_path]
   print ' '.join(fastboot_command)
   stuff = subprocess.Popen(fastboot_command, stdout=subprocess.PIPE,stderr=subprocess.STDOUT).stdout.read()
   print stuff
   fastboot_check(stuff)
   return

def flash_partition():
   common_build_path = os.path.join(common_path,'gpt_both0.bin')
   fastboot_flash_apps('partition',common_build_path)

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

#----------------------------------------------------------------#
# Function to find fastboot utility from APPS build              #
#----------------------------------------------------------------#
def find_fastboot_from_build(la_path) : 
   if sys.platform.find("win") != -1:
      print "Executing fastboot on windows"
      fastboot = os.path.join(la_path, 'vendor/qcom/proprietary/usb/host/windows/prebuilt/fastboot.exe')
      if not os.path.exists(fastboot):
         fastboot = 'fastboot.exe'
         print "Cannot find fastboot.exe in Apps path"
   elif sys.platform.find("linux") != -1:
      print "Executing fastboot on Linux"
      fastboot = os.path.join(la_path,'out/host/linux-x86/bin/fastboot')
      if not os.path.exists(fastboot):
         fastboot = 'fastboot'  
         print "Cannot find fastboot in Apps path" 
   return fastboot


#-----------------------------------------------------------#
# Function to detect if any fastboot devices 
#-----------------------------------------------------------#
def check_fastboot_devices(fastboot) :
   retry_count = 0
   while 1:
      #check if fastboot utility is available. If not, ask user to install it.
      try:
         stuff = subprocess.Popen([fastboot, 'devices'], stdout=subprocess.PIPE).stdout.read()
      except:
         print "\nFastboot is not installed."
         print "Please install fastboot utilities and add to system path. Then reload the build."
         fastboot_file_create("FAILURE_FASTBOOT")
         time.sleep(60)
         sys.exit(0)
      #check if it's in fastboot mode      
      if stuff.count('fastboot') >= 1:
         break
      else:
         print  "Please check if USB is connected."
         print  "If USB is connected, Power cycle the device to fastboot."
         print  "Retry attempt ",retry_count
         time.sleep(20)
         retry_count += 1
         if retry_count == 5:
            print  "Apps Boot Loader is not in the FastBoot state."
            print  "Retry Timeout !!! Exiting"
            fastboot_file_create("FAILURE_FASTBOOT")
            if not (options.wait_disable) :
               time.sleep(30)
            sys.exit(0)

#------------------------------------------------------#
# main                                                 #
#------------------------------------------------------#
parser = OptionParser()
parser.add_option("--ap","-a",action="store", type="string",dest="apps_path",help="APPS PATH")
parser.add_option("--rp","-r",action="store", type="string",dest="result_path",help="RESULT PATH")
parser.add_option("--pf","-p",action="store", type="string",dest="product_flavor",help="PRODUCT FLAVOR")
parser.add_option("-w",action="store_true", default=False,dest="wait_enable",help="ENABLE WAIT")
parser.add_option("--st","-s",action="store", type="string",dest="storage_type",help="STORAGE TYPE")
parser.add_option("--sn","-n",action="store", type="string",dest="serial_number",help="SERIAL NUMBER")
(options, args) = parser.parse_args()
storage_type=''

# Print some diagnostic info                              #

print "Platform is:", sys.platform
print "Python Version is:", sys.version
print "Current directory is:", os.getcwd()

# Load the Meta-Info file                                 #
print "fastboot_complete.py: Loading the Meta-Info file"
mi = ml.meta_info()

# Process the input arguments                             #
print "fastboot_complete.py: Finding paths"
if options.apps_path or options.result_path or options.product_flavor or options.wait_enable or options.storage_type or options.serial_number:
   print "New Parameters Specified."
   if options.apps_path:
      app = True
      apps_path = options.apps_path
   else:
      app = False
      apps_path = mi.get_build_path('apps')
   result_path = options.result_path
   product_flavor = options.product_flavor
   if options.storage_type :
      storage_type = options.storage_type
      print "Specified storage type: "+storage_type
   if options.serial_number :
      serial_number = options.serial_number
      print "Specified serial number: "+serial_number
else:
   if len(sys.argv) >= 2 :
      app = True
      apps_path = sys.argv[1]
   else:
      app = False
      apps_path = mi.get_build_path('apps')
   if len(sys.argv) >= 3 :
      result_path = sys.argv[2]
   else:
      result_path = None
   product_flavor = options.product_flavor
   
la_path = os.path.join(apps_path, 'LINUX','android')
if os.path.exists(la_path):
   la_path_exists = True
else : 
   la_path_exists = False

fastboot = find_fastboot_from_build(la_path)

common_path = mi.get_build_path('common')
common_path = os.path.join(common_path,'common','build')
print "\nApps path is:  ", apps_path
print "Common path is:", common_path

#check for emmc path. If exists make emmc as default storage_type
if (storage_type=='') and (os.path.exists(os.path.join(common_path,'emmc','bin'))) :
   storage_type = 'emmc'
   print "\nDefaulting to emmc storage type."
#This is a temporary fix; to make a default flavor for targets which got different storage_types.

print "\nfastboot_complete.py: Checking target state."
img_failure = ''
check_fastboot_devices(fastboot)  # Check connected fastboot devices
print "\nFound device in Fastboot mode. Flashing the partitions...\n"

#------------------------------------------------------------------------------------------------------#
# Gets a list of all file_paths with "fastboot" or "fastboot_complete" attribute
# If the file contains "apps root path" and if user specified a new apps : 
#   then remove the apps root path from the file
#   If LINUX/android is not there in the user specified apps path , then remove it from file 
#   append user specied path to file to make it a complete usable path
# If fastboot attribute value is "true"
#   partition name = file name
# else 
#   partition name = fastboot attribute value 
#------------------------------------------------------------------------------------------------------#
var_list = {}
var_list_1 = mi.get_file_vars(attr="fastboot_complete",flavor=product_flavor)
var_list_2 = mi.get_file_vars(attr="fastboot",flavor=product_flavor)
for dict in [var_list_1,var_list_2]:   
    for k, v in dict.iteritems():
        var_list.setdefault(k,list())
        var_list[k] += v
if storage_type :
   var_list = storage_type_filter(var_list,storage_type)
   if storage_type == "emmc":
      if os.path.exists(os.path.join(common_path,'emmc','bin')):
         common_path = os.path.join(common_path,'emmc')
   elif storage_type == "ufs":
      if os.path.exists(os.path.join(common_path,'ufs','bin')):
         common_path = os.path.join(common_path,'ufs')
   print "Detected storage type "+storage_type+" at "+common_path


flash_partition() 

for var in var_list:
   for file in var_list[var] :  
      if (mi.get_build_path('apps') in file ) and (app == True):  
         file=file.replace(mi.get_build_path('apps'),'')  
         if ('LINUX/android/' in file) and not(la_path_exists) : 
            file=file.replace('LINUX/android/','')
         file=os.path.join(apps_path,file)
      file_path, file_name_w_ext = os.path.split(file) 
      file_name = os.path.splitext(file_name_w_ext)[0]
      partition_name = file_name if (var == 'true') else var
      print "Loading ", file_name_w_ext," in to ",partition_name," partition. Please wait..." 
      fastboot_flash_apps(partition_name,file)


#---------------------------------------------------------#
# Fastboot images                                         #
# Signal the interrupt handler to handle ctrl + C         #
#---------------------------------------------------------#
signal.signal(signal.SIGINT, interrupt_handler)
fastboot_file_create("SUCCESS_FASTBOOT")
if img_failure != '':
   print "\nCannot find " + img_failure 

if (options.wait_enable): 
    print "fastboot_complete.py: Loading complete,Window will be closed in 1 minute"
    print "Hit Ctrl+C to exit"
    time.sleep(60)
else : 
   print "fastboot_complete.py: Loading complete. "
   time.sleep(5)

