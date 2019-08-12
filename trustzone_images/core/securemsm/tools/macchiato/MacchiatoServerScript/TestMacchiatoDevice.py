#Copyright 2014 Qualcomm Technologies Inc. 
#All Rights Reserved.
#QUALCOMM Confidential and Proprietary
#Uses ADB automation library from https://github.com/sch3m4/pyadb

import os, sys, platform
from adbclient import AdbClient
from MacchiatoVerify import VerifyMacchiatoAuthMessage
from SearchWhitelist import SearchMacchiatoWhitelist
from binascii import hexlify,unhexlify

challengeOnDeviceFile=r"/data/local/tmp/challenge.macc"
challengeLocalFile=r"c:\temp\challenge.bin"
messageOnDeviceFile=r"/data/local/tmp/auth_msg.macc"
messageLocalFile=r"c:\temp\messageLocal.bin"

#Remove the local files so we can make sure it's copying new ones
if os.path.exists(messageLocalFile):
    os.remove(messageLocalFile)
if os.path.exists(challengeLocalFile):
    os.remove(challengeLocalFile)

adb = AdbClient()
devices = adb.GetDevices()
print devices
if len(devices._device_map) == 0:
    print "No device connected, exiting..."
    sys.exit(0)
else:
    device = devices[1]
    print "Connecting to Device %s " % device
    

#Create challenge file
print "Creating Challenge"
challengeBytes = os.urandom(16)
open(challengeLocalFile,"wb").write(challengeBytes)

#Copy challenge to device
print "Copying challenge"
device.Push(challengeLocalFile,challengeOnDeviceFile)

print "Running client"
print device.ShellCommandOutput("macchiato_sample_client")

print "Getting response"
device.Pull(messageLocalFile,messageOnDeviceFile)

#If Windows, replace 0d0a with oa
if (platform.system().lower() == 'windows'):
    print "Fixing CRLF"
    contents = open(messageLocalFile,"rb").read()
    os.remove(messageLocalFile)
    file = open(messageLocalFile,"wb")
    file.write(contents.replace("\r\n","\n"))
    file.close()

print "Verifying response"
deviceInfo = VerifyMacchiatoAuthMessage(messageLocalFile,challengeBytes)
#print hexlify(deviceInfo)
SearchMacchiatoWhitelist(deviceInfo)