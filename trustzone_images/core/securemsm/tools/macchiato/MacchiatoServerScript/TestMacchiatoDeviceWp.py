#Copyright 2014 Qualcomm Technologies Inc. 
#All Rights Reserved.
#QUALCOMM Confidential and Proprietary
#Uses ADB automation library from https://github.com/sch3m4/pyadb

import os, sys, platform, subprocess
from adbclient import AdbClient
from MacchiatoVerify import VerifyMacchiatoAuthMessage
from SearchWhitelist import SearchMacchiatoWhitelist
from binascii import hexlify,unhexlify

print "30-Oct-2014"

AL = 1
WP = 2

plat = WP

if plat == AL:
    challengeOnDeviceFile=r"/data/local/tmp/challenge.macc"
    challengeLocalFile=r"c:\temp\challenge.bin"
    messageOnDeviceFile=r"/data/local/tmp/auth_msg.macc"
    messageLocalFile=r"c:\temp\messageLocal.bin"
elif plat == WP:
    challengeLocalFile=r"challenge.bin"
    challengeOnDeviceFile=r"c:\test\challenge.macc"
    messageLocalFile=r"messageLocal.bin"
    messageOnDeviceFile=r"c:\test\auth_msg.macc"
    keywrapLocalFile=r"eciesad.bin"
    keywrapOnDeviceFile=r"c:\test\eciesad.macc"
else:
    sys.exit(0)


#Remove the local files so we can make sure it's copying new ones
if os.path.exists(messageLocalFile):
    os.remove(messageLocalFile)
if os.path.exists(challengeLocalFile):
    os.remove(challengeLocalFile)

#Create challenge file
print "Creating Challenge"
challengeBytes = os.urandom(16)
open(challengeLocalFile,"wb").write(challengeBytes)

if plat == AL:
    adb = AdbClient()
    devices = adb.GetDevices()
    print devices
    if len(devices._device_map) == 0:
        print "No device connected, exiting..."
        sys.exit(0)
    else:
        device = devices[1]
        print "Connecting to Device %s " % device
    
    #Copy challenge to device
    print "Copying challenge"
    device.Push(challengeLocalFile,challengeOnDeviceFile)


    print "Running client"
    print device.ShellCommandOutput("macchiato_sample_client")

    print "Getting response"
    device.Pull(messageLocalFile,messageOnDeviceFile)
elif plat == WP:
    #Send Filenames to powershell script by way of a file
    #maybe find a way to do this passing as arguments
    file = open("FileList.txt","w")
    file.write(challengeLocalFile+"\n")
    file.write(challengeOnDeviceFile+"\n")
    file.write(messageLocalFile+"\n")
    file.write(messageOnDeviceFile+"\n")
    file.write(keywrapLocalFile+"\n")
    file.write(keywrapOnDeviceFile+"\n")
    file.close()
    subprocess.call(["C:\\Windows\\syswow64\\WindowsPowerShell\\v1.0\\powershell.exe", ". \"./qcMacchiatoWpTest.ps1\";",
                     "&StartTest('"'FileList.txt'"')"])
    subprocess.call(["C:\\Windows\\syswow64\\WindowsPowerShell\\v1.0\\powershell.exe", ". \"./qcMacchiatoWpTest.ps1\";",
                     "&AuthTest('"'FileList.txt'"')"])
    subprocess.call(["C:\\Windows\\syswow64\\WindowsPowerShell\\v1.0\\powershell.exe", ". \"./qcMacchiatoWpTest.ps1\";",
                     "&ChallengeTest('"'FileList.txt'"')"])
#    subprocess.call(["C:\\Windows\\syswow64\\WindowsPowerShell\\v1.0\\powershell.exe", ". \"./qcMacchiatoWpTest.ps1\";",
#                     "&ProvisionKeyTest('"'FileList.txt'"')"])
    subprocess.call(["C:\\Windows\\syswow64\\WindowsPowerShell\\v1.0\\powershell.exe", ". \"./qcMacchiatoWpTest.ps1\";",
                     "&Close('"'FileList.txt'"')"])

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