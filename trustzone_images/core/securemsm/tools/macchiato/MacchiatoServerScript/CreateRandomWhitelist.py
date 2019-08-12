#Copyright 2014 Qualcomm Technologies Inc. 
#All Rights Reserved.
#QUALCOMM Confidential and Proprietary
#This script uses the PyCrypto 3rd-party Python library
# available for Windows at http://www.voidspace.org.uk/downloads/pycrypto26/pycrypto-2.6.win32-py2.7.exe

import os, sys, calendar, time, string, hashlib
from KDF import Kdf8994AndCe
from binascii import hexlify,unhexlify
from Crypto.Signature import PKCS1_PSS
from Crypto.PublicKey import RSA
from Crypto.Hash import SHA256

def int_to_bytes(n, length, endianess='big'):
    h = '%x' % n
    s = ('0'*(len(h) % 2) + h).zfill(length*2).decode('hex')
    return s if endianess == 'big' else s[::-1]

hashLength = 32
dbSeedLength = 32
numRecords = 1000000
headerReservedLength = 48
filename = r"whitelist.bin"
rsaprivatekey = "macc.rsa"

if os.path.exists(filename):
	print "%s Exists already, exiting...." % filename
	sys.exit(0)

#Create the DB Seed for the Whitelist
dbseed = os.urandom(dbSeedLength)
print 'DBSeed: ' + hexlify(dbseed)

#create the list of hashes
hashes = []

#Read the known PHKs
phks = open("phks.txt")
for line in phks.readlines():
    if (line[0] != '#'):
        portions = line.strip().split(',')
        hwid = unhexlify(portions[0])
        serial = unhexlify(portions[1])
        phk = portions[2]
        authPub,encrPub = Kdf8994AndCe(phk,serial)
	#print 'PHK: %s' % phk
	#print 'Auth: %s' % hexlify(authPub)
	#print 'Encr: %s' % hexlify(encrPub)
        recordToHash = hwid + serial + authPub + encrPub + dbseed
        hashes.append(hashlib.sha256(recordToHash).digest())
        print "Record contents for %s: \n %s" % (portions[1],hexlify(recordToHash))
        print "Hash for %s: %s" % (portions[1], hexlify(hashlib.sha256(recordToHash).digest()))        
        
#Add up to a million random hashes
#for i in range (len(hashes),numRecords):
# 	hashes.append(os.urandom(hashLength))

#sort the list
hashes.sort()

#write the hashes to the file

hashfile=open(filename,"wb")

#write an empty header
#TODO -- actual header contents
#Magic 'MACC'
hashfile.write(bytearray('MACC'))
               
#Format Version
hashfile.write(int_to_bytes(1, 4))              

#Whitelist Seq (fixed to 1 for this script)
hashfile.write(int_to_bytes(1, 4))              

#DateTime (seconds since epoch)
hashfile.write(int_to_bytes(calendar.timegm(time.gmtime()), 4))

#DB Seed
#print dbseed
hashfile.write(dbseed)

#Num Records
hashfile.write(int_to_bytes(numRecords,4))

#Padding 
hashfile.write(bytearray(headerReservedLength))

for hash in hashes:
	hashfile.write(hash)

#Close the file since we need to open it up for reading
hashfile.close()

#Open the file for reading for hashing
hashfile=open(filename,"rb")
hashfile.seek(0,0)
contents = bytearray(hashfile.read())

#Calculate the RSA-PSS with SHA256 hash, requires PyCrypto
h = SHA256.new()
h.update(contents)
key = RSA.importKey(open("macc.rsa").read())
signer = PKCS1_PSS.new(key)
signature = signer.sign(h)
hashfile.close()

#Write the signature to the end of the file
hashfile=open(filename,"a+b")
hashfile.seek(0,2)
hashfile.write(signature)
hashfile.close()
