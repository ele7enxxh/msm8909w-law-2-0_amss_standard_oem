
#*****************************************************************************
#
# OTA Update Utility
#
#*****************************************************************************

'''
ota_update.py:

Description:

Run the ota_update utility on the required images.

Usage:

      python ota_update.py [apps_path] [result_path]

      apps_path:

         An optional parameter to the root of the apps build.
            
      result_path:

         An optional parameter indicating the path where success/failure .txt of fastboot would be created
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
import re
import logging
import thread
from time import sleep



currentpath = "c:\\OTA_DEBUG.log"
if os.path.exists(currentpath):
   f = open(currentpath, 'r+')
   f.truncate() 
   #os.remove(currentpath)
logging.basicConfig(filename=currentpath,level=logging.DEBUG)
console = logging.StreamHandler()
logging.getLogger('').addHandler(console)

CONST_NO_METABUILDID = -1
CONST_NO_OTAPROCESS = 0
CONST_USE_OTAPROCESS = 1
CONST_NO_DEVICEAPSSINFO = 3


APSS_VER = {'msm8916':'[\s\S]*eng\.lnxbuild\.([\d]*)[\s\S]*'}

APSS_IMAGE = {'msm8916':'msm8916_32',
              'msm8974':'msm8974'}

OTA_DICT = {'msm8916':'msm8916_32-ota-eng.lnxbuild.zip',
        'msm8226':'msm8226-ota-eng.lnxbuild.zip'}

def progressbar(threadname,filepath):
   
   statinfo = os.stat(filepath)
   OTASIZE = statinfo.st_size
   
   barsize = 20
   barh = OTASIZE/barsize

   upsize = 0   
   while (upsize < OTASIZE):
      sys.stdout.write('\r')
      sleep(10)
      execCmd = "adb shell ls -l /data/update.zip"
      ret = subprocess.Popen(execCmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      (stdoutData, stderrData) = ret.communicate()
      upstring = stdoutData 
      uplist = upstring.split()
      #print uplist
      if (len(uplist) > 3):
         upsize = int(uplist[3])
         bar = upsize/barh
         percent = upsize*100/OTASIZE
         sys.stdout.write("[%-20s] %d%%" % ('='*bar, percent))
      
     
   #print OTASIZE
   

def upgradecheck(build_path,product_name):
   
   #Check the apps path
   #product_name = mi.get_product_name()
   product_path = 'LINUX\\android\\out\\target\\product\\'
   productlist = product_name.split('.')
   key = productlist[0].lower()
   
   if not APSS_IMAGE.has_key(key):
      logging.info("APSS_IMAGE_DIRECTORY is not updated with:%s",key)
      time.sleep(60)
      sys.exit(0)
   product = APSS_IMAGE.get(key)   
   product_path = product_path + product
   apps_path = os.path.join(build_path, product_path)
   ota_path = apps_path
   logging.info("apps path in upgradecheck: %s",ota_path)
   if not os.path.exists(apps_path):
      logging.info("Invalid APSS path:%s",la_path)
      time.sleep(60)
      sys.exit(0)
   #APSS IMAGE directory check is successful
   logging.info("APSS IMAGE directory check is successful")   
   
   #Product PL has to be same for OTA process 
   buildprop_path = os.path.join(apps_path,'system\\build.prop')
   buildprop = open(buildprop_path)
   buildprop = str(buildprop.read())
   product_tag = re.findall('[\s\S]*ro\.board\.platform=(\w*\d*)[\s\S]*',buildprop)
   product_tag = product_tag[0].strip()
   logging.info(" product_tag: %s",product_tag)
   for count2 in range (0,5):
      logging.info("inside loop")
      output = subprocess.check_output("adb devices", shell=False)
      if (len(output) <= 29):
         logging.info("Device not found. Retrying in 3 seconds...")
         time.sleep(3)
         update_flag = CONST_NO_METABUILDID
         return CONST_NO_METABUILDID
      else:
         update_flag = CONST_NO_OTAPROCESS
         logging.info('trying adb root')
         os.system("adb root")
         time.sleep(4)
         break 
   logging.info('adb root successful')
   try:
      device_product_tag = str(subprocess.check_output("adb shell getprop  ro.board.platform", shell=False))
   except:
      logging.info("FAILED:adb shell getprop  ro.board.platform")
   device_product_tag = device_product_tag.strip()
   if not product_tag == device_product_tag:
      logging.info("Meta's are different device_product_tag:%s product_tag:%s",device_product_tag,product_tag)
      update_flag = CONST_NO_OTAPROCESS
      return update_flag,apps_path,ota_path
   logging.info("Product information is similar")
   
   #Check the OTA package in APSS build
   if not OTA_DICT.has_key(product_tag):
      logging.info("OTA for product:%s is not updated in Database",product_tag)
      update_flag = CONST_NO_OTAPROCESS
      return update_flag
   ota_zip = OTA_DICT.get(product_tag)
   if not ota_zip:
      logging.info('data not found')
      update_flag = CONST_NO_OTAPROCESS
      return update_flag,apps_path,ota_path
   ota_path = os.path.join(apps_path,ota_zip)
   logging.info("OTA_gen path:%s",ota_path)                           
   if not os.path.exists(ota_path):
      logging.info("OTA path doesn't exists:%s",ota_path)
      logging.info('ota_path:%s',ota_path)
      update_flag = CONST_NO_OTAPROCESS
      return update_flag,apps_path,ota_path
   if not APSS_VER.has_key(product_tag):
      logging.info("APSS_VER :%s is not updated in Database",product_tag)
      update_flag = CONST_NO_OTAPROCESS
      return update_flag
   try:
      output = subprocess.check_output("adb shell getprop  ro.build.version.incremental", shell=False)
   except:
      logging.info("FAILED:adb shell getprop  ro.build.version.incremental")
      logging.info("ro.build.version.incremental in hw ",version_str)
      update_flag = CONST_NO_OTAPROCESS
      return update_flag

   version_str = APSS_VER.get(product_tag)
   lnxbuild=re.findall(version_str,buildprop)
   if not lnxbuild:
      logging.info("%s not exists in build.prop of apss build",version_str)
      update_flag = CONST_NO_OTAPROCESS
      return update_flag
   logging.info("lnxid : %s",lnxbuild)
   logging.info('output:%s',output)
   devicebuild=re.findall(version_str,output)
   logging.info('devicebuild:%s',devicebuild)
   if( len(lnxbuild) & len(devicebuild)):
      logging.info('devicebuild:%s',devicebuild)
      logging.info('lnxbuild: %s',lnxbuild)
      if (lnxbuild[0] >= devicebuild[0]):
         logging.info('Use OTA Process')
         update_flag = CONST_USE_OTAPROCESS
      else:
         logging.info('don\'t use OTA')
         update_flag = CONST_NO_OTAPROCESS
   else:
      logging.info("Versions are missing ,proceed with old process:lnxbuild devicebuild")
      update_flag = CONST_NO_OTAPROCESS
      
   return update_flag,apps_path,ota_path


'''
   lnxids =  lnxbuild[0].split(".")
   devids =  devicebuild.split(".")
   logging.info("lnxids : %s",lnxids)
   logging.info("lnxids : %s",devids)
   
   update_flag = CONST_USE_OTAPROCESS;
   for lid,did in zip(lnxids[:4], devids[:4]):
      
      if (lid.strip() <did.strip()):
         logging.info('don\'t use OTA lnxbuild id:%s , Devicebuild id:%s',lnxbuild,devicebuild)
         update_flag = CONST_NO_OTAPROCESS
   
   for lid,did in zip(lnxids[4:], devids[4:]):
      
      if (lid.strip() != did.strip()):
         logging.info('don\'t use OTA lnxbuild id:%s , Devicebuild id:%s',lnxbuild,devicebuild)
         update_flag = CONST_NO_OTAPROCESS
'''   

'''
   version_str = APSS_VER.get(product_tag)
   lnxbuild=re.findall(version_str,buildprop)
   devicebuild=re.findall(version_str,output)
   if( len(lnxbuild) & len(devicebuild)):
      logging.info('devicebuild:%s',devicebuild)
      logging.info('lnxbuild: %s',lnxbuild)
      if((lnxbuild[0][0] == devicebuild[0][0]) & (lnxbuild[0][1] >= devicebuild[0][1])):
         logging.info('Use OTA Process')
         update_flag = CONST_USE_OTAPROCESS
      else:
         logging.info('don\'t use OTA')
         update_flag = CONST_NO_OTAPROCESS
   else:
       logging.info("Versions are missing ,proceed with old process:lnxbuild devicebuild")
       update_flag = CONST_NO_OTAPROCESS
'''       
   
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
   stuff = subprocess.Popen(['fastboot', 'flash', img_name,img_path], stdout=subprocess.PIPE,stderr=subprocess.STDOUT).stdout.read()
   print stuff
   fastboot_check(stuff)
   return


parser = OptionParser()
parser.add_option("--ap","-a",action="store", type="string",dest="apps_path",help="APPS PATH")
parser.add_option("--rp","-r",action="store", type="string",dest="result_path",help="RESULT PATH")
parser.add_option("--pf","-p",action="store", type="string",dest="product_flavor",help="PRODUCT FLAVOR")
(options, args) = parser.parse_args()


#---------------------------------------------------------#
# Print some diagnostic info                              #
#---------------------------------------------------------#
logging.info("Platform is: %s", sys.platform)
logging.info("Python Version is: %s", sys.version)
logging.info("Current directory is: %s", os.getcwd())

#---------------------------------------------------------#
# Load the Meta-Info file                                 #
#---------------------------------------------------------#

logging.info("ota_update.py: Loading the Meta-Info file")

sys.path.append(os.path.join(os.path.dirname(__file__) + '/../tools/meta'))

import meta_lib as ml
mi = ml.meta_info()
product_name = mi.get_product_name()

#---------------------------------------------------------#
# Find the fastboot utility on the Apps build.            #
#---------------------------------------------------------#
logging.info("ota_update.py: Finding paths")
if options.apps_path or options.result_path or options.product_flavor:
   print "New Parameters Specified."
   if options.apps_path:
      app = True
      apps_path = options.apps_path
   else:
      app = False
      apps_path = mi.get_build_path('apps')
   result_path = options.result_path
   product_flavor = options.product_flavor
else:
   if len(sys.argv) >= 2 :
      app = True
      apps_path = sys.argv[1]
   else:
      app = False
      apps_path = mi.get_build_path('apps')
      logging.info("apps_pathis :%s",apps_path)
   if len(sys.argv) >= 3 :
      result_path = sys.argv[2]
   else:
      result_path = None
   product_flavor = options.product_flavor

update_flag,apss_path,ota_path = upgradecheck(apps_path,product_name);




	 
   
if update_flag == CONST_USE_OTAPROCESS:
   logging.info("Update Package path is :%s", ota_path)
else:
   update_flag = CONST_NO_OTAPROCESS
   logging.info("OTA package not found. Rolling back to normal flashing...")
   #---------------------------------------------------------#
   # Restarting in fastboot                                  #
   #---------------------------------------------------------#
os.system("adb reboot-bootloader")
time.sleep(5)
   
#---------------------------------------------------------#
# Check for fastboot                                      #
#---------------------------------------------------------#

logging.info("\nota_update.py: Checking target state.")

retry_count = 0
img_failure = ''

#---------------------------------------------------------#
# Check for fastboot                                      #
#---------------------------------------------------------#
while 1:
   #check if fastboot utility is available. If not, ask user to install it.
   try:
      stuff = subprocess.Popen(['fastboot', 'devices'], stdout=subprocess.PIPE).stdout.read()
   except:
      logging.info( "\nFastboot is not installed.")
      logging.info( "Please install fastboot utilities and add to system path. Then reload the build.")
      fastboot_file_create("FAILURE_FASTBOOT")
      time.sleep(60)
      sys.exit(0)
   #check if it's in fastboot mode      
   if stuff.count('fastboot') >= 1:
      break
   else:
      logging.info("Please check if USB is connected.")
      logging.info("If USB is connected, Power cycle the device to fastboot.")
      print  "Retry attempt ",retry_count
      time.sleep(10)
      retry_count += 1
      if retry_count == 5:
         logging.info("Apps Boot Loader is not in the FastBoot state.")
         logging.info("Retry Timeout !!! Exiting")
         fastboot_file_create("FAILURE_FASTBOOT")
         time.sleep(30)
         sys.exit(0)

#------------------------------------------------------------------------------------------------------#
# Gets a dictionary that maps the fastboot var values to the files and fastbooting them.
# Value of the fastboot attribute is "true" or the name of the image
# The first parameter of fastboot_flash_apps is the image name. 
# It is the first part of the filename if fastboot = "true" or the value of the fastboot attribute
#------------------------------------------------------------------------------------------------------#
logging.info( "Fastboot  check in progress")
 
if app == True:
   logging.info( "app is true")
   var_list = dict()
   var_list = mi.get_file_vars(attr='fastboot_complete',flavor=product_flavor)
   for var in var_list:               
      for file in var_list[var] :
         for glob_file in glob(file):
            if var == 'true':
               src_path, src_file = os.path.split(glob_file)
               glob_file = os.path.join(apps_path,src_file)
               print "Loading ", src_file,",Please wait...",
               fastboot_flash_apps(src_file.split('.')[0], glob_file)  
            else:
               src_path, src_file = os.path.split(glob_file)
               print "Loading ", src_file,",Please wait...",
               fastboot_flash_apps(var, glob_file)

   var_list = mi.get_file_vars(attr='fastboot',flavor=product_flavor)
   for var in var_list:               
      for file in var_list[var] :
         for glob_file in glob(file):
            if var == 'true':
               src_path, src_file = os.path.split(glob_file)
               if (update_flag == CONST_USE_OTAPROCESS):
                  if(src_file == "NON-HLOS.bin"):
                     print "Loading ", src_file,",Please wait...",
                     fastboot_flash_apps(src_file.split('.')[0], glob_file)
                     break
               else:                 
                  glob_file = os.path.join(apps_path,src_file)
                  print "Loading ", src_file,",Please wait...",
                  fastboot_flash_apps(src_file.split('.')[0], glob_file)  
            else:
               src_path, src_file = os.path.split(glob_file)
               if (update_flag == CONST_USE_OTAPROCESS):
                  if(src_file == "NON-HLOS.bin"):                    
                     print "Loading ", src_file,",Please wait...",
                     fastboot_flash_apps(var, glob_file)
                     break
               else:
                  print "Loading ", src_file,",Please wait...",
                  fastboot_flash_apps(var, glob_file)
else:
   logging.info( "app is false")
   var_list = dict()     
   var_list = mi.get_file_vars(attr='fastboot_complete',flavor=product_flavor)
   
   for var in var_list:               
      for file in var_list[var] :
         for glob_file in glob(file):
            if var == 'true':
               src_path, src_file = os.path.split(glob_file)
               print "Loading ", src_file,",Please wait...",
               fastboot_flash_apps(src_file.split('.')[0], glob_file)  
            else:
               src_path, src_file = os.path.split(glob_file)
               print "Loading ", src_file,",Please wait...",
               fastboot_flash_apps(var, glob_file)
               logging.info("loading done")
                  
   var_list = mi.get_file_vars(attr='fastboot',flavor=product_flavor)
   
   for var in var_list:               
      for file in var_list[var] :
         for glob_file in glob(file):
            if var == 'true':
               src_path, src_file = os.path.split(glob_file)
               
               if (update_flag == CONST_USE_OTAPROCESS):
                  if(src_file == "NON-HLOS.bin"):
                     print "Loading ", src_file,",Please wait...",
                     fastboot_flash_apps(src_file.split('.')[0], glob_file)
                     break
               else: 
                  print "Loading ", src_file,",Please wait...",
                  fastboot_flash_apps(src_file.split('.')[0], glob_file)  
            else:
               src_path, src_file = os.path.split(glob_file)
               if (update_flag == CONST_USE_OTAPROCESS):
                  if(src_file == "NON-HLOS.bin"):                        
                     print "Loading ", src_file,",Please wait...",
                     fastboot_flash_apps(var, glob_file)
                     break
               else:
                  print "Loading ", src_file,",Please wait...",
                  fastboot_flash_apps(var, glob_file)
                  logging.info("loading done") 
#---------------------------------------------------------#
# Checking if we are updating by zip method               #
#---------------------------------------------------------#
if( update_flag == CONST_USE_OTAPROCESS):
   #---------------------------------------------------------#
   # Pushing update.zip and other commands                   #
   # Updating the phone                                      #
   #---------------------------------------------------------#
   try:
      os.system("fastboot continue")
      time.sleep(10)
   #---------------------------------------------------------#
   # Checking if phone restarted in ADB Mode                 #
   #---------------------------------------------------------#
      count=0
      for count in range (0,10):
         output = subprocess.check_output("adb devices", shell=True)
         if (len(output) <= 29):
            print "Device not found. Retrying in 10 seconds..."
            time.sleep(10)
         else:
            break
   #---------------------------------------------------------#
   # Restarting in root mode                                 #
   #---------------------------------------------------------#
      os.system("adb root")
      time.sleep(7)
   #---------------------------------------------------------#
   # Checking if phone restarted in ADB Mode                 #
   #---------------------------------------------------------#
      counter=0
      for counter in range (0,3):
         output2 = subprocess.check_output("adb devices", shell=True)
         if (len(output2) <= 29):
            print "Device not found. Retrying in 5 seconds..."
            time.sleep(5)
         else:
            break
   #---------------------------------------------------------#
   # Updating the phone                                      #
   #---------------------------------------------------------#
      os.system("adb shell rm /data/update.zip")
      logging.info("Pushing update.zip, Please wait...")
      try:
         thread.start_new_thread(progressbar,("Copyupdatezip",ota_path))
      except:   
         print "Error: Unable to start thread"
      os.system("adb push "+ota_path+" /data/update.zip")

      
      
      logging.info("Updating the Device...")
      os.system("adb shell rm -rf /cache/recovery")
      os.system("adb shell rm -rf /data/recovery")
      os.system("adb shell mkdir /cache/recovery")
      os.system("adb shell mkdir /data/recovery")
      os.system("echo --update_package=/data/update.zip> c:/command")
      os.system("adb push c:/command /data/recovery/command")
      os.system("adb push c:/command /cache/recovery/command")
      os.system("adb shell sync")
   #---------------------------------------------------------#
   # Saving Linux version in a file                          #
   #---------------------------------------------------------#         
      '''
      file_ups = open("c:/linux_info.txt",'w')
      file_ups.write(pl_meta)
      file_ups.close()
      os.system("adb shell rm -rf /data/verinfo")
      os.system("adb shell mkdir /data/verinfo")
      os.system("adb push c:/linux_info.txt /data/verinfo/linux_info.txt")
      file_up = open("c:/ver_info.txt",'w')
      file_up.write(metabuildid)
      file_up.close()
      os.system("adb push c:/ver_info.txt /data/verinfo/ver_info.txt")
      '''  
      os.system("adb shell reboot recovery")
      logging.info("OTA update started!!!! will complete in 2 min")
      
   except Exception:
      logging.info("Error: Device taking too long to restart or device connectivity lost")
else:
   os.system("fastboot continue")

#---------------------------------------------------------#
# Fastboot images                                         #
# Signal the interrupt handler to handle ctrl + C         #
#---------------------------------------------------------#

signal.signal(signal.SIGINT, interrupt_handler)
fastboot_file_create("SUCCESS_FASTBOOT")
if img_failure != '':
   print "\nCannot find " + img_failure
print "Hit Ctrl+C to exit"
time.sleep(60)
