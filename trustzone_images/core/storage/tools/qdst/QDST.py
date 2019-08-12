#! /usr/bin/python
#
#=============================================================================
#
# Secure Boot Certificate Generator
#
# Description: 
# Creates the certificates and signature of an image using SHA1 or SHA256
#   hashing.
#   
# Copyright 2010 Qualcomm Technologies Incorporated. 
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
#=============================================================================
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/storage/tools/qdst/QDST.py#1 $
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
# 08/22/11   ah      Minor change to how logging is done
# 08/03/11   ah      Now allowing openssl check to be skipped
# 12/23/10   ab      Corrected bug in parsing no-extension image filename from image file path
# 12/02/10   ab      Changed list of zipped up files to depend on certchainsize
# 12/01/10   ab      Corrected a bug in displaying the month in printDateTimeInLog()
# 11/24/10   ah      Corrected a bug where cert chain not verified with newer versions of openssl
# 11/23/10   ah      Corrected a bug when using sha256 where hash wasn't captured correctly
# 11/18/10   ab      Additional cert creation
# 09/29/10   ab      Initial revision - QDST.py - ported from FSB.pl
#
#=============================================================================

import os, sys
import getopt
import time
import re
import binascii
import subprocess as sub

def usage():
    print "\n\nERROR: No image was specified, see below for examples"
    print "python QDST.py hash_alg=SHA1 image=C:\\Dropbox\\secureboot\\qcomdevsigningtool\\dbl_SCTUSS_nonsec.mbn"
    print "python QDST.py hash_alg=SHA1 image=images/dbl_SCTUSS_nonsec.mbn xml=myconfig.xml"
    print "python QDST.py hash_alg=SHA256 image=dbl_SCTUSS_nonsec.mbn SW_ID=0x0000000000000002"
    print "python QDST.py hash_alg=SHA256 image=dbl_SCTUSS_nonsec.mbn SW_ID=0x0000000000000003"
    print "python QDST.py hash_alg=SHA256 image=dbl_SCTUSS_nonsec.mbn SW_ID=0x0000000000000004 CN=\"Your name here\""
    print "python QDST.py hash_alg=sha256 image=.\dbl_nonsec.mbn sw_id=0x12 msm_part=0x004200E1 OEM_ID=0x0001 MODEL_ID=0x0002"

    print "\nOther options:"
    print "-----------------"
    print "exponent=3                       : Takes value 3 or 65537, sets the exponent used in all RSA key generation"
    print "rootcakey=rootkey_file.key       : Specify root CA key file (.key)"
    print "rootcacert=rootcert_file.crt     : Specify root CA certificate as .cer (DER format)"
    print "attestcakey=attestkey_file.key   : Specify attest CA key file (.key)"
    print "attestcacert=attestcert_file.crt : Specify attest CA certificate in .cer (DER format)"
    print "certchainsize=3                  : Size of cert chain to sign with, default is 3 (sign with attest, signed by attestCA, signed by root)"
    print "certchainsize=2                  : Size of cert chain set to 2 (sign with attest, signed by root)"
    #print "certchainsize=1                  : Size of cert chain set to 1 (sign with root certificate)"


    print "\nNOTE: The output is stored in the same directory as the input image (actually a \"cert\" sub folder)"
    print "\nNOTE: If sha1 is not specified the default is sha256"

def external_call(command, capture_output=True):
    errors = None
    output = None
    try:
        if capture_output:
            p = sub.Popen(command, stdout=sub.PIPE, stderr=sub.PIPE, shell=True)
            output, errors = p.communicate()
        else:
            os.system(command)
    except Exception, e:
        print "Error executing command '%s'. Reason: %s" % (str(command), e)
        #clean_up()
        sys.exit(1)
    finally:
        #if not output is None:
        #    print "Result: %s" % output
        if (not errors is None) and (not errors == ""):
            print "Process stderr: %s" % errors
    return output

def unzip(filename):
    from zipfile import ZipFile
    z = ZipFile(filename, 'r')
    names = z.namelist()
    for name in names:
        outfile = file(name, 'wb')
        print "Inflating %s." % name
        LOG.write("Inflating %s." % name)
        outfile.write(z.read(name))
        outfile.close()
    z.close()

def zip_files(zip_name, file_list):
    from zipfile import ZipFile
    z = ZipFile(zip_name, 'w')
    for file_name in file_list:
        print "Zipping %s." % file_name
        LOG.write("Zipping %s." % file_name)
        z.write(file_name)
    z.close()
    print "Created file %s." % zip_name
    LOG.write("Created file %s." % zip_name)

def MySystem(temp, subtext=""):
    i = 0
    temp =  "%s | tee temp.txt" % temp

    print "\n%s" % os.getcwd()
    print "\nCalling SYSTEM with\n\"%s\" %s\n" % (temp, subtext )
    LOG.write("\nCalling SYSTEM with\n\"%s\" %s\n" % (temp, subtext))
    os.system(temp)

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

    for each_line in line:
        LOG.write(each_line)

def check_file_exists(filename):
    return os.path.isfile(filename) and os.path.getsize(filename) > 0

def gen_rsa_key(outfile, exponent=3, bits=2048):
    MySystem("openssl genrsa -out %s %s %d" % (outfile, exponent==3 and "-3" or "", bits))

def self_signed_cert(keyfile, outfile, subj='/C=US/ST=California/L="San Diego"/OU="General Use Test Key (for testing only)"/OU="CDMA Technologies"/O=QUALCOMM/CN="QCT Root CA 1"', configfile='opensslroot.cfg', days=7300, setserial=1):
    MySystem("openssl req -new -key %s -x509 -out %s -subj %s -days %d -set_serial %d -config %s" % (keyfile, outfile, subj, days, setserial, configfile))

def gen_signing_request(keyfile, outfile, subj, configfile='opensslroot.cfg', days=7300):
    MySystem("openssl req -new -key %s -out %s -subj %s -days %d -config %s" % (keyfile, outfile, subj, days, configfile))

def sign_request(infile, ca_file, ca_key_file, outfile, extfile, days=7300, setserial=5):
    MySystem("openssl x509 -req -in %s -CA %s -CAkey %s -out %s -set_serial %d -days %d -extfile %s" % (infile, ca_file, ca_key_file, outfile, setserial, days, extfile))

def pem_der_conversion(pem_to_der, infile, outfile):
    MySystem("openssl x509 -inform %s -in %s -outform %s -out %s" % ((pem_to_der and "PEM" or "DER"), infile, (pem_to_der and "DER" or "PEM"), outfile))

def chain_verify(ca_file, filename):
    MySystem("openssl verify -CAfile %s %s" % (ca_file, filename))

def generate_root_ca_files(rootcakey_file='qdst_rootca.key', rootcacert_file='qdst_rootca.crt'):
    gen_rsa_key(rootcakey_file)
    self_signed_cert("qdst_rootca.key", rootcacert_file, '/C=US/ST=California/L="San Diego"/OU="General Use Test Key (for testing only)"/OU="CDMA Technologies"/O=QUALCOMM/CN="QCT Root CA 1"')

def generate_attestation_ca_files(attestcakey_file='"qdst_attestca.key"', attestcacert_file='qdst_rootca.crt'):
    gen_rsa_key(attestcakey_file)
    gen_signing_request("qdst_attestca.key", "qdst_attestca.csr", '/C=US/ST=CA/L="San Diego"/OU="CDMA Technologies"/O=QUALCOMM/CN="QUALCOMM Attestation CA"')
    sign_request("qdst_attestca.csr", "qdst_rootca.crt", "qdst_rootca.key", attestcacert_file, "v3.ext")

def GeneratePhaseOutput(XORwith, S, hash, filename):

    for i in range(len(XORwith)):
        S[i] = S[i] ^ XORwith[i]

    print "H(code_image) = 0x%s\n" % hash
    sys.stdout.write("            S = 0x")
    LOG.write("H(code_image) = 0x%s\n" % hash)
    LOG.write("            S = 0x")
    for i in range(len(S)):
        sys.stdout.write("%.2X" % S[i])
        LOG.write("%.2X" % S[i])

    ## combine these and write a binary file
    try:
        OUTF = open(filename, "wb")
    except Exception, e:
        print "\nCan't open %s for writing: %s\n" % (filename,  e)
        sys.exit(1)

    for i in range(len(S)):
        OUTF.write(chr(S[i]))

    for i in range(0, len(hash), 2):  ## 40 string characters
        temp = hash[i:i+2]
        OUTF.write(chr(int("0x%s" % temp, 16)))
        #print OUTF pack("c",$pad->[$i])      

    try:
        OUTF.close()
    except Exception, e:
        print "Can't close %s: %s\n" % (filename, e)
        sys.exit(1)

    print "\nStored in %s\n" % filename
    LOG.write("\nStored in %s\n" % filename)


def getData(FileName):
    buffer = None

    ## OPEN FILE ---------------------------------------------------------------------------------------

    try:
        IN = open(FileName, "rb")
    except Exception, e:
        print "\nCan't open %s for reading: %s\n" % (FileName, e)
        sys.exit(1)

    for i in range(8):
        buffer = IN.read(1)   # offset is 0
        PrintByte(buffer)
    
    print "  "
    for i in range(20):
        buffer = IN.read(1)   # offset is 0
        PrintByte(buffer)
    print "\n"
    for i in range(36):
        buffer = IN.read(1)   # offset is 0
        PrintByte(buffer)

    try:
        IN.close()
    except Exception, e:
        print "Can't close %s: %s\n" % (FileName, e)
        sys.exit(1)

    filesize = os.path.getsize(FileName)
    if filesize == 0:
        filesize = 1

    print "\n\nFile Size is %.2fB (%dbits)" % (filesize, filesize * 8)

    ## OPEN FILE ---------------------------------------------------------------------------------------
    ## end of GetData ##--------------------------------------------------------------------------


print "\n\n"

def PrintByte(buffer):
    print binascii.hexlify(buffer).upper(),


def GetCurrentTimeInSeconds():
   (year, month, dayOfMonth, hour, minute, second, dayOfWeek, dayOfYear, daylightSavings) = time.localtime()
   TimeInSeconds = hour*3600 + minute*60 + second


   ##print "\n\n-------------------------------------------------------------------------------------------------------"
   ##print "\nCurrent time is $TimeInSeconds seconds"
   ##print "\n-------------------------------------------------------------------------------------------------------\n\n"

   return TimeInSeconds
   

def Sha1(FileName):
    filesize = os.path.getsize(FileName)
    if filesize == 0:
        print "\n\nfilesize is 0, EXITING"
        CleanUp()
        LOG.close()
        sys.exit(1)

    print "\n\nSHA1 on \"%s\" of size %dBytes (%dbits)" % (FileName, filesize, filesize*8)
    MySystem("sha1sum %s" % FileName)

    ## temp.txt will look like this
    ## 64dcd3442435089474b845725dc9f746fef61e3e *dbl_SCTUSS_nonsec.mbn

    try:
        IN = open("temp.txt")
    except Exception, e:
        print "\nCan't open temp.txt for reading: %s\n" % e
        sys.exit(1);
    line = IN.readline()
    
    try:
        IN.close()
    except Exception, e:
        print "Can't close temp.txt: %s\n" % e
        sys.exit(1)

    temp = re.search(r'[\da-fA-F]{40}', line)
    if not temp is None:
        return temp.group(0)

    ## to get this far means hash didn't work
    print "\n\nsha1sum didn't work, are you sure OPENSSL is installed?"
    CleanUp()
    LOG.close()
    sys.exit(1);


def Sha256(FileName):
    filesize = os.path.getsize(FileName)
    if filesize == 0:
        print "\n\n\"%s\" filesize is 0, can't do Sha256 EXITING" % FileName
        LOG.write("\n\n\"%s\" filesize is 0, can't do Sha256 EXITING" % FileName)
        CleanUp()
        LOG.close()
        sys.exit(1)

    print "\nSHA256 on \"%s\" of size %dBytes (%dbits)\n" % (FileName, filesize,filesize * 8)
    LOG.write("\nSHA256 on \"%s\" of size %dBytes (%dbits)\n" % (FileName, filesize, filesize * 8))
    #$temp = "sha1sum $FileName > temp.txt"

    if "sha1".lower() in SigCalc["HASH_ALG"].lower():
        MySystem("openssl dgst -sha1 %s" % FileName)
    else:
        MySystem("openssl dgst -sha256 %s" % FileName)

    ## temp.txt will look like this
    ## 64dcd3442435089474b845725dc9f746fef61e3e *dbl_SCTUSS_nonsec.mbn

    try:
        IN = open("temp.txt")
    except Exception, e:
        print "\nCan't open temp.txt for reading: %s\n" % e
        sys.exit(1)
    
    line = IN.readline()
        
    try:
        IN.close()
    except Exception, e:
        print "Can't close temp.txt: %s\n" % e
        sys.exit(1)

    temp = re.search(r'([\da-fA-F]{64})', line)
    if not temp is None:
        return temp.group(0)

    ## to get this far means hash didn't work
    print "\n\nsha256 didn't work, are you sure OPENSSL is installed?"
    LOG.write("\n\nsha256 didn't work, are you sure OPENSSL is installed?")
    CleanUp()
    LOG.close()
    sys.exit(1)


def ParseDefaultsFromXML(FileName):
    try:
        IN = open(FileName)
    except Exception, e:
        return
    line = IN.readlines()
    try:
        IN.close()
    except Exception, e:
        print "Can't close %s: %s\n" % (FileName, e)
        sys.exit(1)

    global SigCalc
    NumLines = len(line)

    i = None

    # Start parsing the log file
    ## This below gets each line from the file at a time
    line_pattern_one = re.compile(r'<(.+)>0x([a-fA-F\d]+)<\/(.+)>')
    line_pattern_two = re.compile(r'<(.+)>(.+)<\/(.+)>')
    
    for i in range(NumLines):
        match_one = line_pattern_one.search(line[i])
        match_two = line_pattern_two.search(line[i])
        
        if not match_one is None:
            #print line[i]

            if match_one.groups()[0] == match_one.groups()[2]:
                ## to be here means I have an XML tag like OEM_ID (which is in $1 and $3)
                ## and $2 contains a string of hex digits. Every 2 digits is 1 hex value for the array

                print "\n\tArgument \"%s\" updated to 0x%s" % (match_one.groups()[0], match_one.groups()[1])
                LOG.write("\n\tArgument \"%s\" updated to 0x%s" % (match_one.groups()[0], match_one.groups()[1]))

                if match_one.groups()[0] == "DEBUG":
                    attestcert[match_one.groups()[0]] = match_one.groups()[1]
                    continue

                UpdateSigCalcHash(match_one.groups()[0],match_one.groups()[1])  ## key, input

                temp = []

                for j in range(0, len(match_one.groups()[1]), 2):
                    temp.append(int(match_one.groups()[1][j:j+2], 16))

                #$hash{$1} = @temp
                #$HoA{flintstones}[0] = "Fred"

                SigCalc[match_one.groups()[0]] = []   ## empty out old hash for new values

                SigCalc[match_one.groups()[0]].extend(temp)   ## $SigCalc["OEM_ID"] is an array
                ## $SigCalc["OEM_ID"][1] is the 2nd element of the array etc

                ## end of testing for HMAC
            
        elif not match_two is None:
            #print $line[$i]
            if (match_two.groups()[0] == "PART_NAME") or (match_two.groups()[0] == "HASH_ALG") or (match_two.groups()[0] == "SW_ID_NAME"):
                print "\n\tArgument \"%s\" updated to %s" % (match_two.groups()[0], match_two.groups()[1])
                LOG.write("\n\tArgument \"%s\" updated to %s" % (match_two.groups()[0], match_two.groups()[1]))
                SigCalc[match_two.groups()[0]]=match_two.groups()[1]
            elif match_two.groups()[0]==match_two.groups()[2]:
                print "\n\t**AttestCERT key (%s), updated to \"%s\""  % (match_two.groups()[0], match_two.groups()[1])
                LOG.write("\n\t**AttestCERT key (%s), updated to \"%s\""  % (match_two.groups()[0], match_two.groups()[1]))
                attestcert[match_two.groups()[0]] = match_two.groups()[1]

    ## end of GetData ##--------------------------------------------------------------------------


def MySub(var):
    var = var


def ArrayToHexString(a):
    LengthA = len(a)
    sz = ""

    for i in range(LengthA):
        sz = "%s%.2X" % (sz, a[i])

    return sz


def UpdateSigCalcHash(hashkey, hashinput):
    temp = []
    j = None

    for j in range(0, len(hashinput), 2):
        temp.append(int(hashinput[j:j+2], 16))

    #$hash{$1} = @temp
    #$HoA{flintstones}[0] = "Fred"

    SigCalc[hashkey] = []   ## empty out old hash for new values

    SigCalc[hashkey].extend(temp)   ## $SigCalc["OEM_ID"] is an array
    ## $SigCalc["OEM_ID"][1] is the 2nd element of the array etc


def PrintSigCalcHash():
    key = None
    value = None

    print "\n----------These values used for signature calculation------------"
    LOG.write("\n----------These values used for signature calculation------------")

    for key in SigCalc.keys():
        print "\nkey='%s'   \t" % key
        LOG.write("\nkey=%s   \t" % key)

        if key == "PART_NAME":
            print SigCalc[key]
            LOG.write(SigCalc[key])
            continue 
        if key == "HASH_ALG":
            print SigCalc[key]
            LOG.write(SigCalc[key])
            continue
        if key == "SW_ID_NAME":
            print SigCalc[key]
            LOG.write(SigCalc[key])
            continue
        
        sys.stdout.write("0x")
        LOG.write("0x")

        for value in SigCalc[key]:
            sys.stdout.write("%.2X" % value)
            LOG.write("%.2X" % value)
            #print " " #TODO check this
                
    print "\n-----------------------------------------------------------------\n\n"
    LOG.write("\n-----------------------------------------------------------------\n\n")


def PrintAttestCertHash():
    key = None
    value = None

    print "\n----------These values used for attestation certificate------------"
    LOG.write("\n----------These values used for attestation certificate------------")

    for key in attestcert.keys():
        if key == "PART_NAME":
            continue
        if key == "HASH_ALG":
            continue

        print "\nkey: %s   \t" % key
        LOG.write("\n%s   \t" % key)

        print attestcert[key]
        LOG.write(attestcert[key])

    print "\n-----------------------------------------------------------------\n\n"
    LOG.write("\n-----------------------------------------------------------------\n\n")


    filesize = os.path.getsize("qdst_attestca.key")

    if filesize == 0:
        print "\n\nERROR: qdst_attestca.key does not exist"
        print "\nWithout this KEY no image can be signed - EXITING"

        LOG.write("\n\nERROR: qdst_attestca.key does not exist")
        LOG.write("\nWithout this KEY no image can be signed - EXITING")
        CleanUp()
        LOG.close()
        sys.exit(1)

    print "\n\nSUCCESS: the certificate chain exists"
    LOG.write("\n\nSUCCESS: the certificate chain exists")


def TestForOpenSSL():
    global OpenSSLInstalled

    ## Test if the version I need is installed
    response = external_call("openssl version")


    if len(response) > 0:
        print "OpenSSL was found: ", response
    else:
        print "-"*78
        print "ERROR: OpenSSL was not found??\n\n"
        sys.exit()

    for line in response.strip("\r").split("\n"):
        print "\t"+line                              

    line_pattern = re.compile(r'OpenSSL (\d+).(\d+).(\d+)(.) (.+)',flags=re.IGNORECASE)
    line_pattern_match = line_pattern.search(response)

    if not line_pattern_match is None:
        # DOUBT - Versions
        VersionA = int(line_pattern_match.groups()[0])  # 0
        VersionB = int(line_pattern_match.groups()[1])  # 9
        VersionC = int(line_pattern_match.groups()[2])  # 8
        VersionD = line_pattern_match.groups()[3]       # m
        
        if VersionA > 0:
                OpenSSLInstalled = 1
        elif VersionA == 0 and VersionB > 9:
                OpenSSLInstalled = 1
        elif VersionA == 0 and VersionB == 9 and VersionC > 8:
                OpenSSLInstalled = 1

        elif sys.platform=='win32':   ## Needs 0.9.8m or better
            if VersionA == 0 and VersionB == 9 and VersionC == 8 and VersionD > "l":
                    OpenSSLInstalled = 1
        else:                       ## Linux needs 0.9.8k or better
            if VersionA == 0 and VersionB == 9 and VersionC == 8 and VersionD > "j":
                    OpenSSLInstalled = 1

        if OpenSSLInstalled==1:
            print "\n\nSUCCESS: An OpenSSL version that can do SHA256 is installed\n"
            LOG.write("\n\nSUCCESS: An OpenSSL version that can do SHA256 is installed\n")
            if ShowSteps:
                sleep(2)
        else:
            print "\n\nERROR: Didn't find an OpenSSL version that can do SHA256 is installed\n"

    else:
        print "\nERROR: Didn't match the line pattern\n\n"

def InstallOpenSSL():
    print "\n\n-----------------------------------------------------"
    print "-------Verifying correct version of OpenSSL----------"
    print "---Qualcomm Development Signing Tool Code Signing----"
    print "-----------------------------------------------------"
    
    print "\n\nWARNING: This system *does not* have a version of OpenSSL installed that"
    print "can perform SHA256!! It needs to be installed. This will now be attempted."
    
    ## else to be this far means I need to install OpenSSL
    
    if sys.platform=='win32':
        print "\nYou are running Windows since sys.platform='",sys.platform,"'"
        print "\nNOTE: You must ensure that all versions of openssl.exe are removed"
        print "i.e. such as in your CYGWIN path!!\n"
        
        InstallFile1 = "vcredist_x86.exe"
        InstallFile2 = "Win32OpenSSL_Light-0_9_8m.exe"
        if os.path.exists(InstallFile1):
            print "\nInstalling '", InstallFile1, "'"
            MySystem(InstallFile1,' /q')

            if os.path.exists(InstallFile2):
                print "\nInstalling '", InstallFile2, "'"
                MySystem(InstallFile2)
            else:
                print "ERROR: Could not find '", InstallFile2, "', you need to install this to continue"
                sys.exit()

        else:
            print "ERROR: Could not find '", InstallFile1, "', you need to install this to continue"
            sys.exit()
    else:
        print "Not running Windows?? (running %s)" % sys.platform
        print "\nERROR: Sorry, this program cannot automatically install OpenSLL on this system"
        print "You must *manually* install an OpenSSL version that can do SHA256 '0.9.8k' or better to continue\n"

        print "Under Ubuntu Linux for example, use these commands"
        print "sudo apt-get install openssl"
        print "sudo apt-get -f upgrade openssl\n"
        sys.exit()

def PrintStep(sz):
    global CurrentStep
    CurrentStep = CurrentStep + 1

    print "\n\n-------------------------------------------------------------------------------------------------"
    print "\nStep %d: %s" % (CurrentStep, sz)
    print "\n-------------------------------------------------------------------------------------------------"
    ##print getcwd() . "\n"
    if ShowSteps:
        time.sleep(3)

    LOG.write("\n\n-------------------------------------------------------------------------------------------------")
    LOG.write("\nStep %d: %s" % (CurrentStep, sz))
    LOG.write("\n-------------------------------------------------------------------------------------------------\n")


def LookForOKinFile():
    try:
        IN = open("temp.txt")
    except Exception, e:
        print "\nCan't open temp.txt for reading: %s" % e
        sys.exit(1)
    

    for line in IN.readlines():
        print line
        if "OK" in line:
            print "\n\t** OK ** FOUND IT"
            IN.close()
            return 1

    IN.close()

    print "\n\t** FAILED - not verified **"
    sys.exit(1)
    return 0

def ParseCommandLine(ShowDebugOutput):
    global ImageFilename, XMLFilename, SigCalc, attestcert, exponent, rootcakey, rootcacert, attestcakey, attestcacert, certchainsize, testversion
    
    if ShowDebugOutput:
        PrintStep("Parsing command line arguments (if any)")

    argument = None
    
    line_pattern_build = re.compile(r'(image|build)=(.+)', flags=re.IGNORECASE)
    line_pattern_xml = re.compile(r'(xml)=(.+)', flags=re.IGNORECASE)
    line_pattern_part = re.compile(r'PART_NAME=(.+)', flags=re.IGNORECASE)
    line_pattern_hash = re.compile(r'HASH_ALG=(.+)', flags=re.IGNORECASE)
    line_pattern_swid = re.compile(r'=(0x[a-fA-F\d]+)', flags=re.IGNORECASE)
    line_pattern_exponent = re.compile(r'exponent=(\d+)', flags=re.IGNORECASE)
    line_pattern_rootcakey = re.compile(r'rootcakey=(.+\.key)', flags=re.IGNORECASE)
    line_pattern_rootcacert = re.compile(r'rootcacert=(.+\.cer)', flags=re.IGNORECASE)
    line_pattern_attestcakey = re.compile(r'attestcakey=(.+\.key)', flags=re.IGNORECASE)
    line_pattern_attestcacert = re.compile(r'attestcacert=(.+\.cer)', flags=re.IGNORECASE)
    line_pattern_certchainsize = re.compile(r'certchainsize=(\d)', flags=re.IGNORECASE)
    line_pattern_testversion = re.compile(r'testversion=(\d)', flags=re.IGNORECASE)
    
    for argument in sys.argv:
        match_build = line_pattern_build.search(argument)
        match_xml = line_pattern_xml.search(argument)
        match_exponent = line_pattern_exponent.search(argument)
        
        match_rootcakey = line_pattern_rootcakey.search(argument)
        match_rootcacert = line_pattern_rootcacert.search(argument)
        match_attestcakey = line_pattern_attestcakey.search(argument)
        match_attestcacert = line_pattern_attestcacert.search(argument)
        match_certchainsize = line_pattern_certchainsize.search(argument)
        match_testversion = line_pattern_testversion.search(argument)
        
        if not match_build is None:
            ImageFilename = match_build.groups()[1]
            continue

        if not match_xml is None:
            XMLFilename = match_xml.groups()[1]
            continue
            
        if not match_exponent is None:
            exponent = int(match_exponent.groups()[0])
            if not exponent in [3, 65537]:
                print "\n\nERROR: Exponent %d is invalid. Accepted values are 3 and 65537." % exponent
                LOG.write("\n\nERROR: Exponent %d is invalid. Accepted values are 3 and 65537." % exponent)
                LOG.close()
                sys.exit(1)
            continue

        if not match_rootcakey is None:
            rootcakey = match_rootcakey.groups()[0]
            if check_file_exists(rootcakey) == False:
                print "\n\nERROR: File name \"%s\" has file size 0 (or does not exist)" % rootcakey
                LOG.write("\n\nERROR: File name \"%s\" has file size 0 (or does not exist)" % rootcakey)
                LOG.close()
                sys.exit(1)
            continue
        if not match_rootcacert is None:
            rootcacert = match_rootcacert.groups()[0]
            if check_file_exists(rootcacert) == False:
                print "\n\nERROR: File name \"%s\" has file size 0 (or does not exist)" % rootcacert
                LOG.write("\n\nERROR: File name \"%s\" has file size 0 (or does not exist)" % rootcacert)
                LOG.close()
                sys.exit(1)
            else:
                pem_der_conversion(False, rootcacert, "%s.crt" % rootcacert[:rootcacert.rindex(".")])
                rootcacert = "%s.crt" % rootcacert[:rootcacert.rindex(".")]
            continue
        if not match_attestcakey is None:
            attestcakey = match_attestcakey.groups()[0]
            if check_file_exists(attestcakey) == False:
                print "\n\nERROR: File name \"%s\" has file size 0 (or does not exist)" % attestcakey
                LOG.write("\n\nERROR: File name \"%s\" has file size 0 (or does not exist)" % attestcakey)
                LOG.close()
                sys.exit(1)
            continue
        if not match_attestcacert is None:
            attestcacert = match_attestcacert.groups()[0]
            if check_file_exists(attestcacert) == False:
                print "\n\nERROR: File name \"%s\" has file size 0 (or does not exist)" % attestcacert
                LOG.write("\n\nERROR: File name \"%s\" has file size 0 (or does not exist)" % attestcacert)
                LOG.close()
                sys.exit(1)
            else:
                pem_der_conversion(False, attestcacert, "%s.crt" % attestcacert[:attestcacert.rindex(".")])
                attestcacert = "%s.crt" % attestcacert[:attestcacert.rindex(".")]
            continue
        if not match_certchainsize is None:
            certchainsize = int(match_certchainsize.groups()[0])
            if not certchainsize in [2,3]:
                print "\n\nERROR: Cert chain size %d is invalid. Accepted values are 1,2 and 3." % certchainsize
                LOG.write("\n\nERROR: Cert chain size %d is invalid. Accepted values are 1,2 and 3." % certchainsize)
                LOG.close()
                sys.exit(1)
            continue
        
        if not match_testversion is None:
            testversion = int(match_testversion.groups()[0])
            continue
                        
        for key in SigCalc.keys():
            if argument.lower().startswith(key.lower()):
                match_part = line_pattern_part.search(argument)
                match_hash = line_pattern_hash.search(argument)
                match_swid = line_pattern_swid.search(argument)
                
                if not match_part is None:
                    if ShowDebugOutput:
                        print "\n\tArgument \"%s\" updated to %s" % (key, match_part.groups()[0])
                        LOG.write("\n\tArgument \"%s\" updated to %s" % (key, match_part.groups()[0]))
                        
                    SigCalc[key] = match_part.groups()[0]
                    continue
                
                elif not match_hash is None:
                    if ShowDebugOutput:
                        print "\n\tArgument \"%s\" updated to %s" % (key, match_hash.groups()[0])
                        LOG.write("\n\tArgument \"%s\" updated to %s" % (key, match_hash.groups()[0]))
                    
                    SigCalc[key] = match_hash.groups()[0]
                    continue
                
                elif not match_swid is None:
                    ## requirement is to have SW_ID entered as 0x1 but needs to be actually 0x0000000000000001
                    temp = "0x%.*X" % (SigCalcByteLengths[key], int(match_swid.groups()[0], 16))

                    if ShowDebugOutput:
                        print "\n\tArgument \"%s\" updated to %s" % (key, temp)
                        LOG.write("\n\tArgument \"%s\" updated to %s" % (key, temp))
                    
                    temp  = temp.replace("0x", "")
                    UpdateSigCalcHash(key,temp)  ## key, input
                    continue
                
                else:
                    if ShowDebugOutput:
                        print "\nERROR: Argument for \"%s\" must be in HEX format, i.e. 0x1234ABCD" % key
                        LOG.write("\nERROR: Argument for \"%s\" must be in HEX format, i.e. 0x1234ABCD" % key)
                        CleanUp()
                        LOG.close()
                        sys.exit(1)


   
   
        for key in attestcert.keys():
            line_pattern_key = re.compile(r"^%s=(.+)" % key, flags=re.IGNORECASE)
            match_key = line_pattern_key.search(argument)
            if not match_key is None:
                if ShowDebugOutput:
                    print "\nAttestCERT key (%s), updated to \"%s\"" % (key, match_key.groups()[0])
                    LOG.write("\nAttestCERT key (%s), updated to \"%s\"" % (key, match_key.groups()[0]))

                attestcert[key] = match_key.groups()[0]
                continue
   

        # DOUBT: i = i + 1
   
   
    if ImageFilename == "empty":
        usage()

        LOG.write("\n\nERROR: No image was specified")
        LOG.close()
        sys.exit(1)


def CleanUp():
    PrintStep("Removing TEMPORARY FILES")
    if ShowSteps:
        time.sleep(5) ## extra 5 seconds here so I can BREAK if I need to
        
    file_list = ["opensslversion.txt", "%sPhaseOne.bin" % ImageFilenameNoExt, "%sPhaseTwo.bin" % ImageFilenameNoExt, "%sattest.csr" % ImageFilenameNoExt, "sighash.bin", "sha256rootcert.txt"]
    
    for some_file in file_list:
        if os.path.exists(some_file):
            os.unlink(some_file)

    if sys.platform=='win32':   ## Needs 0.9.8m or better
        MySystem("del %s-signature.bin %s-root_cert.cer %s-attestation_ca_cert.cer %s-attestation_cert.cer" % (OutputFilename, OutputFilename, OutputFilename, OutputFilename))
        MySystem("del all.crt %s-root_cert.crt %s-attestation_ca_cert.crt %s-attestation_cert.crt qdst_attestca.crt %sattest.key" % (OutputFilename, OutputFilename, OutputFilename, ImageFilenameNoExt))
    else:
        MySystem("rm %s-signature.bin %s-root_cert.cer %s-attestation_ca_cert.cer %s-attestation_cert.cer" % (OutputFilename, OutputFilename, OutputFilename, OutputFilename))
        MySystem("rm all.crt %s-root_cert.crt %s-attestation_ca_cert.crt %s-attestation_cert.crt qdst_attestca.crt %sattest.key" % (OutputFilename, OutputFilename, OutputFilename, ImageFilenameNoExt))


    #PrintStep("*MANUAL STEP* Move \"".$OutputFilename.".zip\" to the correct location - (MANUAL FOR NOW SORRY)")

    file_list = ["QcomDevSigningToolDefaults.xml", "qdst_attestca.cer", "qdst_attestca.key", "qdst_rootca.cer", "qdst_rootca.key", "opensslroot.cfg", "v3_attest.ext", "tee.exe", "temp.txt", "qdst_rootca.crt"]
    
    for some_file in file_list:
        if os.path.exists(some_file):
            os.unlink(some_file)


def printDateTimeInLog():
    months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
    weekDays = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
    (year, month, dayOfMonth, hour, minute, second, dayOfWeek, dayOfYear, daylightSavings) = time.localtime()
    theTime = "%d:%d:%d, %s %s %d, %d" % (hour, minute, second, weekDays[dayOfWeek], months[month-1], dayOfMonth, year)
    LOG.write("\n\n\n==============================================================")
    LOG.write("====================== BEGIN =================================")
    LOG.write("==============================================================\n")
    LOG.write("QDST.py started on %s" % theTime)

def CompareArrays(a, b):
    i = None
    j = None
    LengthA = len(a)
    LengthB = len(b)
    Errors = 0

    ## NOTE: Array is actually a number split up, i.e. 0x01 is 0,1
    ## I expect it to be the same length BUT if not, leading ZEROs are ok
    ## i.e. 0x0001 = 0x1

    while LengthA + LengthB > 0:
        if LengthA>0 and LengthB>0:
            LengthA = LengthA - 1
            LengthB = LengthB - 1

            if not a[LengthA] == b[LengthB]:
                Errors = 1
                break
        else:
            if LengthA==0:
                LengthB = LengthB - 1

                if not b[LengthB] == 0:
                    Errors = 1
                    break
            elif LengthB==0:
                LengthA = LengthA - 1
                if not a[LengthA] == 0:
                    Errors = 1
                    break

    return Errors   ## returns 0 if the arrays are the same

CurrentStep = 0
ShowSteps = 0
## Where was this script started
dir = os.getcwd()

LogFileName = "QDST_Log.txt"

if dir[-1] == "/" or dir[-1] == "\\": ## makes it have to have an anchor at the end
    dir = dir[:-1]  ## Take the left of what matched - get rid of slash   

dir.replace("\\", "/")
#dir.replace("\", "/")

dir = "%s/" % dir

print "Script was started in '%s'\n" % dir

try:
    LOG = open(LogFileName, "w")
except Exception, e:
    print "\nCan't open %s for writing: %s" % (LogFileName, e)
    sys.exit(1)

printDateTimeInLog()


SigCalc = {
   'SW_ID'    : [ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ],
   'OEM_ID'   : [ 0x00, 0x00 ],
   'MODEL_ID' : [ 0x00, 0x00 ],
   'MSM_PART' : [ 0x00, 0x00, 0x00, 0x00 ],
   'ipad'     : [ 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36 ],
   'opad'     : [ 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C ],
   'PART_NAME': "DefaultPart",
   'HASH_ALG' : "SHA256",
   'SW_ID_NAME': "IMAGE",
}

SigCalcByteLengths = {
   'SW_ID'    : 16,
   'OEM_ID'   : 4,
   'MODEL_ID' : 4,
   'MSM_PART' : 8,
   'ipad'     : 16,
   'opad'     : 16,
}

attestcert = {
   'C'              : "US",
   'ST'             : "California",
   'L'              : "San Diego",
   'O'              : "ASIC",
   'CN'             : "Qualcomm",
   'DEBUG'          : "0x0000000000000000",   ## set to off; on is 0xF
}



i = None
j = None
key = None
ImageFilename = "empty"
XMLFilename = "empty"
exponent = 3
rootcakey = None
rootcacert = None
attestcakey = None
attestcacert = None
certchainsize = 3
testversion = 1

## There is a good chance this script will be called like this
## python c:\windows\QDST.py etc

## Therefore, I need to be in the c:\windows to run the script

PathToQDST = sys.path[0]

print "\nChanging to sys.path[0]=",PathToQDST
os.chdir(PathToQDST)
print "I'm currently in ",os.getcwd()


#path = os.environ['PATH']

#if not "openssl".lower() in path.lower():
#	if os.path.exists("c:\\OpenSSL\\bin"):
#		os.environ['PATH'] = "c:\\OpenSSL\\bin;%s" % path
#	elif os.path.exists("c:\\OpenSSL-Win32\\bin"):
#		os.environ['PATH'] = "c:\\OpenSSL-Win32\\bin;%s" % path
#	else:
        # DOUBT
#		print "Could not find location of OpenSSL bin folder!"


print "\n------------------------------------------------------------------"
print   "-------Qualcomm Development Signing Tool Code Signing V2.0--------"
print   "------------------------------------------------------------------"

ParseCommandLine(1) ## done here mostly to see if an image was provided

OpenSSLInstalled = 0

PrintStep("Testing if you have a current OpenSSL version installed")

if testversion==1:
    TestForOpenSSL()    ## if not installed, it tries to install it
else:
    OpenSSLInstalled = 1
    print "-"*78
    print "NOTE: OpenSSL version was NOT tested because \"testversion=0\" was specified\n"

if OpenSSLInstalled == 0:
    InstallOpenSSL()

    TestForOpenSSL()    ## test *now* if it is installed. If not, script bails here

    if OpenSSLInstalled==0:
        print "\nERROR: The correct version of OpenSSL is *not* installed and this"
        print "script could not install it.\n"
        
        LOG.write("\n\nERROR: The correct version of OpenSSL is *not* installed and this")
        LOG.write("\nscript could not install it.\n")
        
        LOG.close()
        sys.exit(1)

filesize = os.path.getsize("QDST.zip")
if filesize == 0:
    print "\n\n\"QDST.zip\" filesize is 0, can't continue without this file EXITING"
    LOG.write("\n\n\"QDST.zip\" filesize is 0, can't continue without this file EXITING")
    CleanUp()
    LOG.close()
    sys.exit(1)

## there is a chance the files are already unzipped, thus must remove them first
file_list = ["QcomDevSigningToolDefaults.xml", "qdst_attestca.cer", "qdst_attestca.key", "qdst_rootca.cer", "qdst_rootca.key", "opensslroot.cfg", "sha256rootcert.txt", "v3_attest.ext"]
for some_file in file_list:
    if os.path.exists(some_file):
        os.unlink(some_file)


#MySystem("unzip QDST.zip")
#TODO: This requires testing
unzip('QDST.zip')


if XMLFilename == "empty":
    PrintStep("Getting defaults from XML file \"QcomDevSigningToolDefaults.xml\"")
    ParseDefaultsFromXML("QcomDevSigningToolDefaults.xml")
else:
    PrintStep("Getting defaults from XML file \"%s\"" % XMLFilename)
    ParseDefaultsFromXML(XMLFilename)

ParseCommandLine(1) ## get command line arguments that will overwrite xml arguments

PrintStep("Showing the defaults to be used in the Signature Calculation")

print "\n---------------------------------------------------------------------"
print "A signature generation is applied to a code image to tie MSM HW info"
print "(MSM_ID,OEM_ID,PROD_ID etc) cryptographically into the image. The"
print "following performs the Hash Message Authentication Code (HMAC) to allow"
print "the image specified to be digitally signed"
print "---------------------------------------------------------------------"

PrintSigCalcHash()

if ShowSteps:
    time.sleep(2)

PrintStep("Verifying image provided exists")

#$ImageFilename
## Ex. $ImageFilename = c:\temp\dbl_SCTUSS_nonsec.mbn
temp = ImageFilename
## TODO: temp =~ s#/#\\#g ## replace  / with \

if sys.platform=='win32':   ## Needs 0.9.8m or better
    temp = temp.replace("/", "\\")
    # Note, there is a chance there are no slashes in this filename
    if(temp.rfind("\\") != -1):
        ImageFilenameNoExt = temp[temp.rindex("\\")+len("\\"):]
    else:
        ImageFilenameNoExt = temp

else:
    if(temp.rfind("/") != -1):
        ImageFilenameNoExt = temp[temp.rindex("/")+len("/"):]
    else:
        ImageFilenameNoExt = temp

if "." in ImageFilenameNoExt:
    ImageFilenameNoExt = ImageFilenameNoExt[0:ImageFilenameNoExt.rindex(".")]

print "ImageFilename =\t\t%s" % ImageFilename
print "ImageFilenameNoExt =\t%s\n\n" % ImageFilenameNoExt

ImagePath = temp.index(ImageFilenameNoExt)
ImagePath = "%scert/" % temp[0:ImagePath]

ImageFilenameNoExt = "QDST_%s_" % ImageFilenameNoExt
   
printDateTimeInLog()

## $ImageFilenameNoExt = dbl_SCTUSS_nonsec
#print "\n\n$ImageFilenameNoExt and ".index($ImageFilename, ".") . " and " . length($ImageFilename)

#$ImageFilename provided, but does it exist?
ImageFileSize = os.path.getsize(ImageFilename)

if ImageFileSize == 0:
    print "\n\nERROR: image name \"%s\" has file size 0 (or does not exist)" % ImageFilename
    print "\nEx."
    print "\npython QDST.py image=dbl_SCTUSS_nonsec.mbn\n\n"

    LOG.write("\n\nERROR: image name \"%s\" has file size 0 (or does not exist)" % ImageFilename)
    LOG.write("\nEx.")
    LOG.write("\npython QDST.py image=dbl_SCTUSS_nonsec.mbn\n\n")
    CleanUp()
    LOG.close()
    sys.exit(1)
else:
    print "\nImage to sign exists \"%s\" image (%d bytes)\n" % (ImageFilename, ImageFileSize)
    LOG.write("\nImage to sign exists \"%s\" image (%d bytes)\n" % (ImageFilename, ImageFileSize))

OutputFilename = "empty"

if CompareArrays(SigCalc["SW_ID"],[0,0,0,0,0,0,0,0]) == 0:
    OutputFilename = "%s_SBL" % SigCalc["PART_NAME"]
elif CompareArrays(SigCalc["SW_ID"],[0,0,0,0,0,0,0,2]) == 0:
    OutputFilename = "%s_AMSS_HASH_TABLE" % SigCalc["PART_NAME"]
elif CompareArrays(SigCalc["SW_ID"],[0,0,0,0,0,0,0,3]) == 0:
    OutputFilename = "%s_FLASH_PRG" % SigCalc["PART_NAME"]
elif CompareArrays(SigCalc["SW_ID"],[0,0,0,0,0,0,0,4]) == 0:
    OutputFilename = "%s_DSP_HASH_TABLE" % SigCalc["PART_NAME"]
else:
    OutputFilename = "%s_%s" % (SigCalc["PART_NAME"], SigCalc["SW_ID_NAME"])

OutputFilename = "QDST_%s" % OutputFilename
## Final step is to create the MSM_ID, which is a concat of the MSM_PART, OEM_ID and MODEL_ID

# If generation of qdst_rootca.cer and/or qdst_attestca.cer is required, it can be called here
if rootcakey is None or check_file_exists(rootcakey) == False:
    rootcakey = "qdst_rootca.key"
if rootcacert is None or check_file_exists(rootcacert) == False:
    rootcacert = "qdst_rootca.crt"
if attestcakey is None or check_file_exists(attestcakey) == False:
    attestcakey = "qdst_attestca.key"
if attestcacert is None or check_file_exists(attestcacert) == False:
    attestcacert = "qdst_attestca.crt"

PrintStep("Preparing ROOTCA and ATTESTCA certificate names to match image name (as per CSMS website)")
if sys.platform=='win32':   ## Needs 0.9.8m or better
    MySystem("copy %s.cer %s-root_cert.cer" % (rootcacert[:rootcacert.rindex(".")], OutputFilename))
    MySystem("copy %s.cer %s-attestation_ca_cert.cer" % (attestcacert[:attestcacert.rindex(".")], OutputFilename))
else:
    MySystem("cp %s.cer %s-root_cert.cer" % (rootcacert[:rootcacert.rindex(".")], OutputFilename))
    MySystem("cp %s.cer %s-attestation_ca_cert.cer" % (attestcacert[:attestcacert.rindex(".")], OutputFilename))

MySystem("openssl x509 -inform DER -in %s.cer -outform PEM -out %s.crt" % (rootcacert[:rootcacert.rindex(".")], rootcacert[:rootcacert.rindex(".")]))
MySystem("openssl x509 -inform DER -in %s.cer -outform PEM -out %s.crt" % (attestcacert[:attestcacert.rindex(".")], attestcacert[:attestcacert.rindex(".")]))

temp = [ SigCalc["MSM_PART"][0], SigCalc["MSM_PART"][1], SigCalc["MSM_PART"][2], SigCalc["MSM_PART"][3], SigCalc["OEM_ID"][0], SigCalc["OEM_ID"][1], SigCalc["MODEL_ID"][0], SigCalc["MODEL_ID"][1] ]
if "MSM_ID" in SigCalc.keys():
    SigCalc["MSM_ID"].extend(temp)   ## $SigCalc["OEM_ID"] is an array
else:
    SigCalc["MSM_ID"] = temp

PrintStep("Generating \"Phase 1\" of signature (see Figure 3-1 Keyed Hash Algorithm)")
if "sha1".lower() in SigCalc["HASH_ALG"].lower():
    GeneratePhaseOutput(SigCalc["SW_ID"] ,SigCalc["ipad"], Sha1(ImageFilename), "%sPhaseOne.bin" % ImageFilenameNoExt)
else:
    GeneratePhaseOutput(SigCalc["SW_ID"], SigCalc["ipad"], Sha256(ImageFilename), "%sPhaseOne.bin" % ImageFilenameNoExt)

PrintStep("Generating \"Phase 2\" of signature (see Figure 3-1 Keyed Hash Algorithm)")
if "sha1".lower() in SigCalc["HASH_ALG"].lower():
    GeneratePhaseOutput(SigCalc["MSM_ID"], SigCalc["opad"], Sha1("%sPhaseOne.bin" % ImageFilenameNoExt), "%sPhaseTwo.bin" % ImageFilenameNoExt)
else:
    GeneratePhaseOutput(SigCalc["MSM_ID"], SigCalc["opad"], Sha256("%sPhaseOne.bin" % ImageFilenameNoExt), "%sPhaseTwo.bin" % ImageFilenameNoExt)

filesize = os.path.getsize("%sPhaseTwo.bin" % ImageFilenameNoExt)

print "\n%sPhaseTwo.bin is ready to be signed (%d bytes)" % (ImageFilenameNoExt, filesize)
#print Sha256($ImageFilenameNoExt."PhaseTwo.bin")


## CERTIFICATE CODE BEGINS HERE

if "sha1".lower() in SigCalc["HASH_ALG"].lower():
    attestcert["OU07"] = "07 0000 SHA1"
else:
    attestcert["OU07"] = "07 0001 SHA256"

if int(attestcert["DEBUG"], 16) == 0xF:
    attestcert["OU03"]  = "03 000000000000000F DEBUG"
else:
    attestcert["OU03"]  = "03 0000000000000000 DEBUG"

attestcert["OU05"] = "05 %.8X SW_SIZE" % ImageFileSize

attestcert["OU06"] = "06 %s MODEL_ID" % ArrayToHexString(SigCalc["MODEL_ID"])
attestcert["OU04"] = "04 %s OEM_ID" % ArrayToHexString(SigCalc["OEM_ID"])
attestcert["OU02"] = "02 %s HW_ID" % ArrayToHexString(SigCalc["MSM_ID"])
attestcert["OU01"] = "01 %s SW_ID" % ArrayToHexString(SigCalc["SW_ID"])

PrintStep("Ready to create \"Attestation Certificate\" - showing the values to be used")
PrintAttestCertHash()
if ShowSteps:
    time.sleep(3)

PrintStep("Creating a NEW 2048 bit private/public key pair")

MySystem("openssl genrsa -out %sattest.key %s 2048" % (ImageFilenameNoExt, exponent==3 and "-3" or ""))

PrintStep("Making a certificate signing request (CSR) (will be in PEM format)")

temp = "openssl req -new -key %sattest.key -out %sattest.csr -subj " % (ImageFilenameNoExt, ImageFilenameNoExt)

for key in attestcert.keys():
    if "DEBUG".lower() in key.lower():
        continue
    if not re.search(r'OU\d\d', key) is None:
        temp = "%s/OU=\"%s\"" % (temp, attestcert[key])
    else:
        temp = "%s/%s=\"%s\"" % (temp, key, attestcert[key])

temp = "%s -days 7300 -config opensslroot.cfg" % temp

MySystem(temp)

PrintStep("Creating the ATTESTATION certificate (signed with AttestCA key)")

if certchainsize == 2:
    MySystem("openssl x509 -req -in %sattest.csr -CA %s -CAkey %s -outform DER -out %s-attestation_cert.cer -days 7300 -set_serial 38758 -extfile v3_attest.ext" % (ImageFilenameNoExt, rootcacert, rootcakey, OutputFilename))
else:
    MySystem("openssl x509 -req -in %sattest.csr -CA %s -CAkey %s -outform DER -out %s-attestation_cert.cer -days 7300 -set_serial 38758 -extfile v3_attest.ext" % (ImageFilenameNoExt, attestcacert, attestcakey, OutputFilename))

PrintStep("Creating the **SIGNATURE** for %sPhaseTwo.bin based on new ATTESTATION CERTIFICATE" % ImageFilenameNoExt)

if "sha1".lower() in SigCalc["HASH_ALG"].lower():
    MySystem("openssl dgst -sha1 -binary %sPhaseTwo.bin > sighash.bin" % ImageFilenameNoExt)
else:
    MySystem("openssl dgst -sha256 -binary %sPhaseTwo.bin > sighash.bin" % ImageFilenameNoExt)

if certchainsize == 1:
    MySystem("openssl rsautl -sign -pkcs -in sighash.bin -inkey %s -out %s-signature.bin" % (rootcakey, OutputFilename))
else:
    MySystem("openssl rsautl -sign -pkcs -in sighash.bin -inkey %sattest.key -out %s-signature.bin" % (ImageFilenameNoExt, OutputFilename))


PrintStep("VERIFYING THE CERTIFICATE CHAIN (optional) - Need to first covert all certs to PEM format")
MySystem("openssl x509 -inform DER -in %s-root_cert.cer -outform PEM -out %s-root_cert.crt" % (OutputFilename, OutputFilename))
MySystem("openssl x509 -inform DER -in %s-attestation_ca_cert.cer -outform PEM -out %s-attestation_ca_cert.crt" % (OutputFilename, OutputFilename))
MySystem("openssl x509 -inform DER -in %s-attestation_cert.cer -outform PEM -out %s-attestation_cert.crt" % (OutputFilename, OutputFilename))

PrintStep("VERIFYING THE CERTIFICATE CHAIN (optional) - Combine AttestCA + Attest CERT into ALL.CRT")
if sys.platform=='win32':   ## Needs 0.9.8m or better
    MySystem("copy %s-attestation_ca_cert.crt+%s-attestation_cert.crt all.crt" % (OutputFilename, OutputFilename))
else:
    MySystem("cat %s-attestation_ca_cert.crt %s-attestation_cert.crt > all.crt" % (OutputFilename, OutputFilename))


PrintStep("VERIFYING THE CERTIFICATE CHAIN (optional) - VERIFY ROOT CERT - output should say \"OK\"")
MySystem("openssl verify -CAfile %s-root_cert.crt %s-root_cert.crt" % (OutputFilename, OutputFilename))

LookForOKinFile()

PrintStep("VERIFYING THE CERTIFICATE CHAIN (optional) - VERIFY ATTEST CA CERT - output should say \"OK\"")
MySystem("openssl verify -CAfile %s-root_cert.crt %s-attestation_ca_cert.crt" % (OutputFilename, OutputFilename))
LookForOKinFile()

PrintStep("VERIFYING THE CERTIFICATE CHAIN (optional) - VERIFY ATTEST CERT (all.crt) - output should say \"OK\"")
if certchainsize == 2:
    MySystem("openssl verify -CAfile %s-root_cert.crt %s-attestation_cert.crt" % (OutputFilename, OutputFilename))
else:
    MySystem("openssl verify -CAfile %s-root_cert.crt all.crt" % OutputFilename)
LookForOKinFile()


## ZIP FILE IS HERE ----------------------------------------------------------
PrintStep("Final phase is Zipping up the contents")
print "\n\%s=%s\n" % (OutputFilename, OutputFilename)

#MySystem("zip zipfile %s-signature.bin %s-root_cert.cer %s-attestation_ca_cert.cer %s-attestation_cert.cer %sattest.key" % (OutputFilename, OutputFilename, OutputFilename, OutputFilename, ImageFilenameNoExt))
zip_file_list = ["%s-signature.bin" % OutputFilename, "%s-root_cert.cer" % OutputFilename, "%sattest.key" % ImageFilenameNoExt]
if certchainsize == 2:
    zip_file_list.append("%s-attestation_cert.cer" % OutputFilename)
elif certchainsize == 3:
    zip_file_list.append("%s-attestation_cert.cer" % OutputFilename)
    zip_file_list.append("%s-attestation_ca_cert.cer" % OutputFilename)
zip_files("zipfile.zip", zip_file_list)

PrintStep("Renaming default ZIP file to %s.zip" % OutputFilename)
if sys.platform=='win32':   ## Needs 0.9.8m or better
    MySystem("ren zipfile.zip %s.zip" % OutputFilename)
else:
    MySystem("mv zipfile.zip %s.zip" % OutputFilename)

## Final step is to move the zip file to the correct directory, i.e. $ImagePath
## First make sure old file doesn't exist there, if so delete it
if not os.path.exists(ImagePath):
    os.mkdir(ImagePath)
else:
    if os.path.exists("%s%s.zip" % (ImagePath, OutputFilename)):
        os.unlink("%s%s.zip" % (ImagePath, OutputFilename))

if sys.platform=='win32':   ## Needs 0.9.8m or better
    MySystem("move %s.zip %s%s.zip" % (OutputFilename, ImagePath, OutputFilename))
else:
    MySystem("mv %s.zip %s%s.zip" % (OutputFilename, ImagePath, OutputFilename))


CleanUp()

PrintStep("OUTPUT STORED AT: %s%s.zip" % (ImagePath, OutputFilename))
PrintStep("Check %s for LOG" % LogFileName)

print "\n\n----------------------------------------------------------"
print "\n DONE Qualcomm Development Signing Tool Code Signing V2.0"
print "\n----------------------------------------------------------\n"

print "\n"
LOG.close()
sys.exit(0)
