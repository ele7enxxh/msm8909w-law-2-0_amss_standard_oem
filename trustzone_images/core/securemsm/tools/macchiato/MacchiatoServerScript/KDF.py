#Copyright 2014 Qualcomm Technologies Inc. 
#All Rights Reserved.
#QUALCOMM Confidential and Proprietary

from binascii import hexlify,unhexlify
import hmac, hashlib
from Crypto.Cipher import AES
from ecdsa import VerifyingKey, SigningKey, NIST256p

def Kdf8994(phk):
  #Input is byte array with a PHK (32 bytes)
  #KDF is an HMAC with a fixed text string and the PHK as key
  text = '0003000000aa0000000000000000000000000000000000000000000000000000000000000000000000000300'
  return hmac.new(phk, unhexlify(text), hashlib.sha256).digest()

#Output is the Macchiato Seed    
def MacchiatoCe(bpk,mode):
  ptAuth = '0000000055555555555555555555555555555555555555555555555555555555'
  ptEncr = '00000000AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA'
  iv = bytes(bytearray(16))
  if (mode == 'auth'):
     message = ptAuth
  elif (mode == 'encr'):
     message = ptEncr
  else:
     print "Unknown mode, exiting..."
     sys.exit(0)
  return AES.new(bpk,AES.MODE_CBC,iv).encrypt(unhexlify(message))

def GetPubKeyFromPrivate(bpk):
    return SigningKey.from_string(bpk,curve=NIST256p).get_verifying_key().to_string()

def Kdf8994AndCe(phk,serial,verbose=None):

    if verbose:
        print 'PHK %s - Serial %s' %(phk,serial)
    #001B0000633F - PHK: ' + phk
    
    if (phk == '0000000000000000000000000000000000000000000000000000000000000000'):
	bpk = unhexlify(phk)
    else:
        bpk = Kdf8994(unhexlify(phk))
    if verbose:
        print 'KDF Output:\n ' + hexlify(bpk)

    #print 'MACC Seed Auth:\n ' + hexlify(MacchiatoCe(bpk,'auth'))
    authPub = GetPubKeyFromPrivate(MacchiatoCe(bpk,'auth'))
    if verbose:
        print "Auth Pub Key x:\n " + hexlify(authPub)[0:64]
        print "Auth Pub Key y:\n " + hexlify(authPub)[64:128]

    #print 'MACC Seed Encr:\n ' + hexlify(MacchiatoCe(bpk,'encr'))
    encrPub = GetPubKeyFromPrivate(MacchiatoCe(bpk,'encr'))
    if verbose:
        print "Encr Pub Key x:\n " + hexlify(encrPub)[0:64]
        print "Encr Pub Key y:\n " + hexlify(encrPub)[64:128]
        print

    return (authPub,encrPub)

if __name__ == "__main__":
    Kdf8994AndCe('0000000000000000000000000000000000000000000000000000000000000000','001B0000633F','true')
    Kdf8994AndCe('0102030405060708090A0B0C0D0E0F10102030405060708090A0B0C0D0E0F100','001B00112E0B','true')
    Kdf8994AndCe('FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF','001B00112E0C','true')
