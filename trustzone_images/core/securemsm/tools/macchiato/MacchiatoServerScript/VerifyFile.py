#Copyright 2014 Qualcomm Technologies Inc. 
#All Rights Reserved.
#QUALCOMM Confidential and Proprietary

from binascii import hexlify,unhexlify
from MacchiatoVerify import VerifyMacchiatoAuthMessage

chal = '00000000000000000000000000000000'

VerifyMacchiatoAuthMessage(r"c:\users\smoskovi\desktop\auth_msg.macc",unhexlify(chal))