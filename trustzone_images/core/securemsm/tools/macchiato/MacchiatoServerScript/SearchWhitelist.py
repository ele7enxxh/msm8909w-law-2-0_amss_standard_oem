#Copyright 2014 Qualcomm Technologies Inc. 
#All Rights Reserved.
#QUALCOMM Confidential and Proprietary
#This script uses the PyCrypto 3rd-party Python library
# available for Windows at http://www.voidspace.org.uk/downloads/pycrypto26/pycrypto-2.6.win32-py2.7.exe

import os, sys, random, hashlib
from binascii import hexlify,unhexlify
from Crypto.Signature import PKCS1_PSS
from Crypto.PublicKey import RSA
from Crypto.Hash import SHA256
from Crypto.Util import number

wlfilename = "whitelist.bin"
recordsize = 32
headersize = 100
signaturesize = 256

def SearchMacchiatoWhitelist(deviceInfo, filename=wlfilename, ignoreSignature=None):

    numRecords = 0
    
    if not os.path.exists(filename):
	    print "Couldn't open %s, exiting...." % filename
	    sys.exit(0)
    file = open(filename,"rb")
    
    filesize = os.path.getsize(filename)
    if ((filesize-headersize-signaturesize) % 32 != 0):
	    print "Invalid size for a whitelist file, exiting...."
	    sys.exit(0)
    else:
	    numRecords = (os.path.getsize(filename)-headersize-signaturesize)/32  

    #Verify the signature
    file.seek(0,0)
    contents = bytearray(file.read(filesize - signaturesize))
    file.seek(signaturesize*-1,2)
    signature = file.read(256)
    
    #Calculate the RSA-PSS with SHA256 hash, requires PyCrypto
    
    h = SHA256.new()
    h.update(contents)
    key = RSA.importKey(open("maccpub.rsa").read())
    verifier = PKCS1_PSS.new(key)
    if verifier.verify(h, (signature),): #Pass tuple to verify
        print "Signature on Whitelist validated..." 
    else:
        print "Signature did not validate, exiting..."
        sys.exit(0)
    
	#Read the DB Seed
    file.seek(16,0)
    dbSeed = file.read(32)
	
    #print "DBSeed %s " % hexlify(dbSeed)
	
    #Calculate the Record Hash
    searchingFor = hashlib.sha256(deviceInfo+dbSeed).digest()
    #print "Searching for %s " % hexlify(searchingFor)
    

    #binary search
    
    first = 0
    last = numRecords-1
    while (first <= last):
	    mid = first + (last - first) / 2
	    file.seek(100 + mid*32,0)
	    record = bytearray(file.read(32))
	    
	    if searchingFor == record:
		    print "Found record in Whitelist"
		    file.close()
		    sys.exit(0)
	    elif record < searchingFor:
		    first = mid + 1;
	    else:
		    last = mid - 1;
	    #print "Checking against %s " % hexlify(record)
    
    print "Record *Not* found in Whitelist"
    file.close()
    

if __name__ == "__main__":
    if len(sys.argv) != 2:
	        print "ERROR: Command line argument is the hash to search for. (Specify random to pick one at random)"
	        sys.exit(0)
    else:
            hash = sys.argv[1]

    if not os.path.exists(wlfilename):
	    print "Couldn't open %s, exiting...." % wlfilename
	    sys.exit(0)
    file = open(wlfilename,"rb")

    filesize = os.path.getsize(wlfilename)
    if ((filesize-headersize-signaturesize) % 32 != 0):
	    print "Invalid size for a whitelist file, exiting...."
	    sys.exit(0)
    else:
	    numRecords = (os.path.getsize(wlfilename)-headersize-signaturesize)/32  

    if (hash == "random"):
            randomIndex = random.randrange(0,numRecords-1)
            file.seek(100 + randomIndex*32,0)
            searchingFor = bytearray(file.read(32))
            print "Searching for random hash found from the file: %s " % hexlify(searchingFor)
    elif len(hash) != 64:
            hash = hash.zfill(64)
            print "Length of the hash was less than 64 characters, created %s " % hash
            searchingFor = unhexlify(hash)
    
    first = 0
    last = numRecords-1
    while (first <= last):
	    mid = first + (last - first) / 2
	    file.seek(100 + mid*32,0)
	    record = bytearray(file.read(32))
	    
	    if searchingFor == record:
		    print "Found record in Whitelist"
		    file.close()
		    sys.exit(0)
	    elif record < searchingFor:
		    first = mid + 1;
	    else:
		    last = mid - 1;
	    #print "Checking against %s " % hexlify(record)
    
    print "Record *Not* found in Whitelist"
    file.close()