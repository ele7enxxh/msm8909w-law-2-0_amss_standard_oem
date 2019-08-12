#! /usr/bin/python
#
#=============================================================================
#
# Opens ZIP file from QDST or CSMS and appends them into QCOM MBN format
# with header
#
# Copyright 2011 Qualcomm Technologies Incorporated. 
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
#=============================================================================
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/storage/tools/qdst/QDSTMBN.py#1 $
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#  $Change: 10727476 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     -------------------------------------------------------------------------
# 08/04/11   ah      Bug fix, ZIP filename not correct if SW_ID was changed
# 08/01/11   ah      Bug fix, output mbn didn't have header
# 07/13/11   ah      Initial Release
#
#=============================================================================

import os, sys
import getopt
import time,struct
import re
import binascii
from types import *

def PrependMBNHeader(ImageFilename,SigSize):
    OutputBinaryFile = "mbn_" + ImageFilename

    print "\nCreated Empty Container: " + OutputBinaryFile + "\n"

    size = os.path.getsize(ImageFilename)

    CertChainSize = 6*1024  # Assumed to be 6kB in length, will be padded with 0xFF to make sure it's this big

    opfile = open(OutputBinaryFile, "wb")

    Write4BytesToFile(opfile,"image_id",27)                     # image_id - Identifies the type of image this header represents (OEM SBL, AMSS, Apps boot loader, etc.).
    Write4BytesToFile(opfile,"header_vsn_num",3)                # Header version number
    Write4BytesToFile(opfile,"image_src",40)                    # Location of image in flash: 
    Write4BytesToFile(opfile,"image_dest_ptr",0)                # Pointer to location to store image in RAM. Also, entry point at which image execution begins.
    Write4BytesToFile(opfile,"image_size",size+SigSize+CertChainSize)   # Size of complete image in bytes 
    Write4BytesToFile(opfile,"code_size",size)                  # Size of code region of image in bytes 
    Write4BytesToFile(opfile,"signature_ptr",size)              # Pointer to images attestation signature 
    Write4BytesToFile(opfile,"signature_size",SigSize)          # Size of the attestation signature in bytes 
    Write4BytesToFile(opfile,"cert_chain_ptr",size+SigSize)     # Pointer to the chain of attestation certificates associated with the image
    Write4BytesToFile(opfile,"cert_chain_size",CertChainSize)   # Size of the attestation chain in bytes 

    ## Here is the original file to sign
    ipfile = open(ImageFilename, 'r+b')
    opfile.write(ipfile.read())
    ipfile.close()

    opfile.close()

    return OutputBinaryFile

def Write4BytesToFile(opfile,message,value):
   print "%-20s = %i" % (message,value)
   bytes = struct.pack("I",value)       
   opfile.write(bytes)

def unzip(filename):
    global names
    from zipfile import ZipFile
    z = ZipFile(filename, 'r')
    names = z.namelist()
    for name in names:
        outfile = file(name, 'wb')
        print "Inflating %s." % name
        outfile.write(z.read(name))
        outfile.close()
    z.close()

def createbinaryoutput(OutputFilename, file_list):

    OriginalOutputFilename = OutputFilename
    # strip off the mbn_ from OutputFilename (if it's there)
    m = re.search("^mbn_(.+)", OutputFilename)
    if type(m) is not NoneType:
        OutputFilename = m.group(1)

    OutputBinaryFile = OutputFilename + ".mbn"

    print "\n\n*********************************************************"
    print "---------------------------------------------------------"
    print "       %s needs to be created " % OutputBinaryFile
    print "---------------------------------------------------------"
    print "*********************************************************"

    print "\n\n"
    print "              @@#'+@@         "    
    print "            ##''''':,:@       "    
    print "           #''#@':'@+,,@      "    
    print "           +'@       @,,@     "    
    print "          @'+        `+,#     "    
    print "          @'#         @'#     "    
    print "        `,@'+;;;;;::,,@'#     "    
    print "       @,,@'+,,,,,,,,,@'#`@   "    
    print "       @@,;@@,,,,,,,,,#@+``.  "    
    print "       #,+;'''''''''''''''''  "    
    print "       #,+,,,,,,,@@@,,,,,,.'  "    
    print "       #,+,,,,,,@MBN@,,,,,`'  "    
    print "       #,',,,,,,@@@@@,,,,,`'  "    
    print "       #,',,,,,,,MBN,,,,,,`'  "    
    print "       #,',,,,,,+@@@',,,,``'  "    
    print "       #,',,,,,,@MBN@,,,,``'  "    
    print "       #,',,,,,,@@@@@,,,```'  "    
    print "       #,;,,,,,@@@@@@@,````'  "    
    print "       ';;,,,,,,,,,,,.`````'  \n\n"


    opfile = open(OutputBinaryFile, "wb")
  
    ## Here is the original file (possibly with mbn header)
    ipfile = open(OriginalOutputFilename, 'r+b')
    opfile.write(ipfile.read())
    ipfile.close()
    size = os.path.getsize(OriginalOutputFilename)
    print "Adding %s of size %d bytes." % (OriginalOutputFilename,size)
    tally = size
      
    # Here is the digital signature and all the certs      
    for file_name in file_list:
        size = os.path.getsize(file_name)
        print "Adding %s of size %d bytes (0x%X)." % (file_name,size,tally)
        tally += size
        ipfile = open(file_name, 'r+b')
        opfile.write(ipfile.read())
        ipfile.close()

    # Get the real cert chain size, i.e. sum of 3 certs filesizes      
    CertChainSize = 0
    for i in range(1,len(file_list)) :
        CertChainSize += os.path.getsize(file_list[i])

    if CertChainSize>(6*1024):
        print "ERROR: cert chain is > 6KB. Can't handle this"
        sys.exit(1)

    PadSize = (6*1024) - CertChainSize  # need to know how many 0xFF's to pad at the end
    PAD = [0xFF]*PadSize
        
    print "Adding padding of size %d bytes (0x%X)." % (PadSize,tally)
    bytes = struct.pack("%iB" % PadSize,*PAD)
    opfile.write(bytes)

    opfile.close()


    print "\n\n*********************************************************"
    print "---------------------------------------------------------"
    print "       %s created " % OutputBinaryFile
    print "---------------------------------------------------------"
    print "*********************************************************"

def MySystem(temp, subtext=""):
    i = 0
    RedirectionFound = 0
    
    #temp =  "%s | tee temp.txt" % temp
    
    temp2 = re.search(r'>', temp)
    if temp2 is None:
        #print "No redirection char found"
        temp =  "%s > temp.txt" % temp
    else:
        #print "Redirection char found"
        RedirectionFound = 1
        
    print "\n%s" % os.getcwd()
    print "\nCalling SYSTEM with\n\"%s\" %s\n" % (temp, subtext )
    os.system(temp)

    if RedirectionFound==0:
            try:
                IN = open("temp.txt")
            except Exception, e:
                print "\nCan't open temp.txt for reading: %s\n" % e
                sys.exit(1)
            line = IN.readlines()
            try:
                IN.close()
            except Exception, e:
                print "Can't close temp.txt: %s\n" % e
                sys.exit(1)

def check_file_exists(filename):
    return os.path.isfile(filename) and os.path.getsize(filename) > 0

def GetCurrentTimeInSeconds():
   (year, month, dayOfMonth, hour, minute, second, dayOfWeek, dayOfYear, daylightSavings) = time.localtime()
   TimeInSeconds = hour*3600 + minute*60 + second


   ##print "\n\n-------------------------------------------------------------------------------------------------------"
   ##print "\nCurrent time is $TimeInSeconds seconds"
   ##print "\n-------------------------------------------------------------------------------------------------------\n\n"

   return TimeInSeconds
   
## Where was this script started
dir = os.getcwd()

if dir[-1] == "/" or dir[-1] == "\\": ## makes it have to have an anchor at the end
    dir = dir[:-1]  ## Take the left of what matched - get rid of slash   

dir.replace("\\", "/")
#dir.replace("\", "/")

dir = "%s/" % dir

print "Script was started in '%s'\n" % dir

PathToQDST = sys.path[0]

print "\nChanging to sys.path[0]=",PathToQDST
os.chdir(PathToQDST)
print "I'm currently in ",os.getcwd()

if len(sys.argv)<=1:
    print "need path/filename to append certs to"
    sys.exit(1)

FILE = sys.argv[1]
ZIP = "empty"
if len(sys.argv)<=2:
    print "\nNOTE: No path to a ZIP file was included, will call QDST to perform signing"
else:
    ZIP  = sys.argv[2]

prependmbn = 0

m = re.search("mbn$", FILE)
if type(m) is NoneType:
    prependmbn = 1
    print "\nNOTE: '%s' does not have an MBN extension, will prepend an MBN header *before* signing" % FILE

filesize = os.path.getsize(FILE)
print "\n%s is of size %i bytes" % (FILE,filesize)

if filesize == 0:
    print "\n\nFilesize of signed image is 0, can't continue without this file EXITING"
    sys.exit(1)

if prependmbn==1:
    FILE = PrependMBNHeader(FILE,256)
    size = os.path.getsize(FILE)
    print "\nMBN header prepended: Created '%s' of size %i bytes\n" % (FILE,size)

if ZIP is "empty":
    print "Need to call QDST to sign"
    os.system("python QDST.py image=%s" % FILE)

    # Don't know the name of the output ZIP file just created by QDST.py
    for filename in os.listdir("cert"):
        print  filename
        m = re.search("zip$", filename,re.IGNORECASE)
        if type(m) is not NoneType:
            ZIP = "cert/"+filename

    if ZIP is "empty":
        print "-"*78
        print "ERROR: Could not located the ZIP file created by QDST.py\n"
        sys.exit()
        
filesize = os.path.getsize(ZIP)
print "%s is of size %i\n" % (ZIP,filesize)

if filesize == 0:
    print "\n\nFilesize of ZIP is 0, can't continue without this file EXITING"
    sys.exit(1)

unzip(ZIP)

File_Signature_Size  = ""
File_Root_Size       = ""
File_CA_Size         = ""
File_Cert_Size       = ""

File_Signature  = ""
File_Root       = ""
File_CA         = ""
File_Cert       = ""


certchainsize = 0

i=0
for name in names:
    i+=1
    m = re.search("\.bin$", name)
    if type(m) is not NoneType:
        File_Signature      = name
        File_Signature_Size = os.path.getsize(name)
    m = re.search("root_cert", name)
    if type(m) is not NoneType:
        certchainsize+=1
        File_Root       = name
        File_Root_Size  = os.path.getsize(name)
    m = re.search("attestation_ca_cert", name)
    if type(m) is not NoneType:
        certchainsize+=1
        File_CA         = name
        File_CA_Size    = os.path.getsize(name)
    m = re.search("attestation_cert", name)
    if type(m) is not NoneType:
        certchainsize+=1
        File_Cert       = name
        File_Cert_Size  = os.path.getsize(name)


print "\nFile_Signature",File_Signature
print "File_Root",File_Root
print "File_CA",File_CA
print "File_Cert",File_Cert

bin_file_list = [File_Signature]

if certchainsize == 1:
    bin_file_list.append(File_Root)
if certchainsize == 2:
    bin_file_list.append(File_Cert)
    bin_file_list.append(File_Root)
elif certchainsize == 3:
    bin_file_list.append(File_Cert)
    bin_file_list.append(File_CA)
    bin_file_list.append(File_Root)

##OutputFilename = PrependHeader(FILE,File_Signature_Size)

createbinaryoutput(FILE, bin_file_list)

sys.exit(1)


