#Copyright 2014 Qualcomm Technologies Inc. 
#All Rights Reserved.
#QUALCOMM Confidential and Proprietary
#This script uses the PyCrypto 3rd-party Python library
# available for Windows at http://www.voidspace.org.uk/downloads/pycrypto26/pycrypto-2.6.win32-py2.7.exe

import os, sys, random, struct, hashlib, array
from binascii import hexlify,unhexlify
from ecdsa import VerifyingKey, NIST256p, BadSignatureError

headerNotSigned = 80

def VerifyMacchiatoAuthMessage(fileName, challengeContents):
    file = open(fileName, "rb")
    fileSize = os.path.getsize(fileName)
	#print "Message is %s bytes " % fileSize
    
    formatVersion, = struct.unpack('>i', file.read(4))
    if (formatVersion != 1):
        print "Expected Format Version 1 but found %s, exiting..." % formatVersion
        sys.exit(0)
    
    messageId, = struct.unpack('>i', file.read(4))
    if (messageId != 1):
        print "Expected Message Id 1 but found %s, exiting..." % formatVersion
        sys.exit(0)
    
    messageLength, = struct.unpack('>i', file.read(4))
    if (messageId != 1):
        print "Expected Message Id 1 but found %s, exiting..." % formatVersion
        sys.exit(0)
    
    if (fileSize != messageLength):
        print "File Size and Message Length mismatch, exiting...."
        sys.exit(0)
    
    errorCode, = struct.unpack('>i', file.read(4))
    #print errorCode
    #if (errorCode != 0):
    #    print "Error Code was not 0, exiting...."
    #   sys.exit(0)
    
    #Read Signature (big-endian in file)
    signature = file.read(64)
    print "Signature:\n " + hexlify(signature)

    #Read Public Keys (big-endian in file)
    publicKeyAuth = file.read(64)
    print "PubKeyAuth:\n " + hexlify(publicKeyAuth)
    publicKeyEncr = file.read(64)
    print "PubKeyEncr:\n " + hexlify(publicKeyEncr)

    #Read TA Hash - ignore for now
    taHash = file.read(32)
    
    #Read Num ServiceIDs
    numSvcIds, = struct.unpack('>i', file.read(4))
    
    #Read Service IDs - ignore for now
    file.read(8 * numSvcIds)
    
    #Read Chip HW Version (little endian in file)
    chipHwVersion, = struct.unpack('>H', file.read(2))
    print "Chip HW Version %s" % hex(chipHwVersion)[2:].zfill(4)
    
    #Read Chip Serial Number (little endian in file)
    chipSerialNumber, = struct.unpack('>i', file.read(4))
    print "Chip Serial Number %s " % hex(chipSerialNumber)[2:].zfill(8)

    #Read OEM ID (little endian in file)
    chipOemID, = struct.unpack('>i', file.read(4))
    print "Chip OEM ID %s " % hex(chipOemID)[2:].zfill(8)
    
    #Read OEM PK Hash - ignore for now
    oemPkHash = file.read(32)    
    
    #Read Challenge
    challenge = file.read(16) 
    if (challenge != challengeContents):
        print "Challenge does **NOT** match...."
        #sys.exit(0)
    else:
        print "Challenge matched"
		  
    #Read response and opaque data
    response = file.read(16)
    
    #Read Opaque Data Length
    opaqueDataLen, = struct.unpack('>i', file.read(4))
    
    #Read Opaque Data
    opaqueData = file.read() # rest of file
    
    pubkey = VerifyingKey.from_string(publicKeyAuth,curve=NIST256p)
    
    #Backup and get the pat of the file covered by signature
    file.seek(headerNotSigned,0)
    messageToVerify = file.read()
    
    try:
        pubkey.verify(signature,messageToVerify,hashfunc=hashlib.sha256)
        print "Signature verifies!"
    except BadSignatureError:
          print "Signature does NOT match!"
		  
    #print hexlify(chipHwVersion)
    #print hexlify(chipSerialNumber)
    #print hexlify(publicKeyAuth)
    #print hexlify(publicKeyEncr)
    return unhexlify(hex(chipHwVersion)[2:].zfill(4)) + unhexlify(hex(chipSerialNumber)[2:].zfill(8)) + publicKeyAuth + publicKeyEncr