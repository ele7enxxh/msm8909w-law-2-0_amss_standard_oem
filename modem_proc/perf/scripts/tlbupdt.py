# tlbupdt.py <Image.elf>
# Produce the file _image.elf with the table BLASTK_tlb_dump updated with the proper TLBs mapping the image
# Sections identified with __MMU_region_* symbols are given the specified attributes
# BLASTK_tlb_first_replaceable, image_vend, image_pend, image_vstart, image_pstart, image_end

# Apr 25 12: Qurt support
# Jun 28 12: Add main_stack_size to calculate the EndAddress

import sys
import glob
import math
from subprocess import call
import os
import re

#../../tools/myps/mem/tools/tlbupdt.py
PyPath = sys.argv [0]
PyPath = re.sub('tlbupdt.py','',PyPath)



EndAddressAlignment = (256 * 1024)


def FormatHex (HexValue):
 if (HexValue [-1] == 'L'):
  HexValue = HexValue [:-1]
 if (HexValue [0:2] == '0x'):
  HexValue = HexValue [2:]

 while (len (HexValue) < 8):
  HexValue = '0' + HexValue

 HexValue = '0x' + HexValue

 return HexValue


def ValueToUnitStr (Value, Unit, Len):
 if (Unit != 1):
  ValueUnit = Value*100 / Unit
  ValueUnitStr = str(ValueUnit)
  while (len (ValueUnitStr) < 2):
   ValueUnitStr = '0' + ValueUnitStr
  ValueStr = str (ValueUnit / 100) + '.' + ValueUnitStr [-2:]
 else:
  ValueStr = str (Value)

 ValueStr = ValueStr.rjust (Len)

 return ValueStr

def HexToLendianByteList (HexValue):
 ByteLst = []
 HexValue = HexValue [2:]
 Byte = ''
 for Hbyte in HexValue:
  if (Byte == ''):
   Byte = Hbyte
  else:
   ByteLst.append (Byte + Hbyte)
   Byte = ''

 ByteLst.reverse ()  # Little endian 

 return ByteLst
   
  

#PyPath = '../../tools/myps/mem/tools/'

if (len (sys.argv) < 2):
 print 'Usage: TlbUpdt <EllFile> [-t TempWriteLocation] [-q]'
 sys.exit (-1)

Fatal = 0
ElfFile = sys.argv [1]

Temp = ''
if '-t' in sys.argv:
 Index = 0
 for Item in sys.argv:
  if (Item == '-t') and (len(sys.argv) > Index+1):
   Temp = sys.argv [Index+1]
  Index += 1

ArgFile = open (Temp + 'Arg.lst', 'w')

if '-q' in sys.argv:
 Qurt = True
else:
 Qurt = False

qurtos_symbols = False
PrcsStdOut = open (Temp + 'qurtos_or_QURTK.txt', 'w')
RetCode = call(['python', PyPath + 'elfgets3.py', ElfFile, 'qurtos_main_stack_size'], stdout = PrcsStdOut)
PrcsStdOut = open (Temp + 'qurtos_or_QURTK.txt', 'rb')
for NewLine in PrcsStdOut:
 Split = NewLine.split ('\t')
 if Split[0] == 'qurtos_main_stack_size':
  qurtos_symbols = True
  break
PrcsStdOut.close ()

if Qurt == True:
 if qurtos_symbols is True:
  Commands = ['qurtos_app_heap_size', 'qurtos_main_stack_size', 'QURTK_tlb_dump', 'QURTK_tlb_first_replaceable', 'start', 'end', '__MMU_region_*', 'image_*', 'image_vend']
  Gen2symDic = {'app_heap_size':'qurtos_app_heap_size', 'main_stack_size':'qurtos_main_stack_size', 'tlb_dump':'QURTK_tlb_dump', 'tlb_first_replaceable':'QURTK_tlb_first_replaceable'}
 else:
  Commands = ['QURTK_app_heap_size', 'QURTK_main_stack_size', 'QURTK_tlb_dump', 'QURTK_tlb_first_replaceable', 'start', 'end', '__MMU_region_*', 'image_*', 'image_vend']
  Gen2symDic = {'app_heap_size':'QURTK_app_heap_size', 'main_stack_size':'QURTK_main_stack_size', 'tlb_dump':'QURTK_tlb_dump', 'tlb_first_replaceable':'QURTK_tlb_first_replaceable'}
 MaxTlbCnt = 62  # Assumes qurt targets Hex v5
else:
 Commands = ['BLASTK_app_heap_size', 'BLASTK_main_stack_size', 'BLASTK_tlb_dump', 'BLASTK_tlb_first_replaceable', 'start', 'end', '__MMU_region_*', 'image_*', 'image_vend']
 Gen2symDic = {'app_heap_size':'BLASTK_app_heap_size', 'main_stack_size':'BLASTK_main_stack_size', 'tlb_dump':'BLASTK_tlb_dump', 'tlb_first_replaceable':'BLASTK_tlb_first_replaceable'}
 MaxTlbCnt = 32  # Assumes non qurt targets Hex v4 and previous versions

ParserLst = []
SectSizeDic = {}
PagedSectionSize = 0
LastRegionAddress = 0


ArgFile.write (ElfFile + ' ' + Commands [0] + ' \n')
ParserLst.append (ElfFile + ' ' + Commands [0])

for Idx in range (1, len(Commands)):
 ArgFile.write ('-n ' + Commands [Idx] + '\n')
 ParserLst.append (ElfFile + ' ' + Commands [Idx])

ArgFile.close ()

PrcsStdOut = open (Temp + 'map.lst', 'wb')
RetCode = call(['python', PyPath + 'elfgets3.py', '-f', Temp + 'Arg.lst'], stdout = PrcsStdOut)

PrcsStdOut.close ()
PrcsStdOut = open (Temp + 'map.lst', 'rb')

ParsedCmd = ''
SectLst = []


AddressDic = {}
PadEndDic   = {}
ValueDic = {}

PrevVAaddr=0
PrevPAaddr=0

for NewLine in PrcsStdOut:
 Split = NewLine.split ('\t')

 for Idx in range (0, len(Split)):
  Split [Idx] = Split [Idx].strip ()

 if (len (Split) == 1) and (Split [0] in ParserLst):
  ParsedCmd = Split [0]
  Address = ''
  continue

 if (ParsedCmd == '') or (len (Split) == 0):
  continue

 elif (ParsedCmd == ElfFile + ' ' + Gen2symDic ['app_heap_size']):
  if (Split [0] == Gen2symDic ['app_heap_size']):
   Address = Split [1]
  elif (Address  == Split [0]):
   try:
     AddressDic [Gen2symDic ['app_heap_size']] = long (Split [2], 16)
   except IndexError:
     AddressDic [Gen2symDic ['app_heap_size']] = 0

 elif (ParsedCmd == ElfFile + ' ' + Gen2symDic ['main_stack_size']):
  if (Split [0] == Gen2symDic ['main_stack_size']):
   Address = Split [1]
  elif (Address  == Split [0]):
   AddressDic [Gen2symDic ['main_stack_size']] = long (Split [2], 16)

 elif (ParsedCmd == ElfFile + ' ' + Gen2symDic ['tlb_dump']):
  if (Split [0] == Gen2symDic ['tlb_dump']):
   AddressDic [Gen2symDic ['tlb_dump']] = long (Split [1], 16)

 elif (ParsedCmd == ElfFile + ' ' + Gen2symDic ['tlb_first_replaceable']):
  if (Split [0] == Gen2symDic ['tlb_first_replaceable']):
   AddressDic [Gen2symDic ['tlb_first_replaceable']] = long (Split [1], 16)

 elif (ParsedCmd == ElfFile + ' start'):
  if (Split [0] == 'start'):
   AddressDic ['start'] = long (Split [1], 16)

 elif (ParsedCmd == ElfFile + ' end'):
  if (Split [0] == 'end'):
   AddressDic ['end'] = long (Split [1], 16)

 elif (ParsedCmd == ElfFile + ' __MMU_region_*'):
  if (Split [0].find ('__MMU_region_') != -1):
   AddressDic [Split [0]] = long (Split [1], 16)

 elif (ParsedCmd == ElfFile + ' image_*'):
  if (Split [0].find ('image_') != -1):
   AddressDic [Split [0]] = long (Split [1], 16)

 elif (ParsedCmd == ElfFile + ' image_vend'):
  if (Split [0] == 'image_vend'):
   Address = Split [1]
  elif (Address  == Split [0]):
   ValueDic ['image_vend'] = long (Split [2], 16)

print '\nAdding MMU mapping'
print 'Outstanding symbols:'
addr_sym_pairs = sorted([('0x%08x' % AddressDic[i], i) for i in AddressDic])
for addr, sym in addr_sym_pairs:
  print '%s\t%s' % (addr, sym)

mmu_addresses = [AddressDic[i] for i in AddressDic if '__MMU_region_' in i]

if len(mmu_addresses) != len(set(mmu_addresses)):
  # multiple MMU entries exist with the same address

  # filter out the segment loading MMU entries and retry this check
  mmu_addr_wo_segload = [AddressDic[i] for i in AddressDic
    if '__MMU_region_' in i and
    '_text_wcdma_' not in i and
    '_text_tdscdma_' not in i]

  if len(mmu_addr_wo_segload) != len(set(mmu_addr_wo_segload)):
    # non-segload MMU entry collision, abort
    print '\nTwo or more MMU entries were found to have the same address.'
    print 'Cannot determine which MMU entry to apply.'
    print 'Please look at the outstanding symbols and fix.'
    print 'Aborting.'

    sys.exit(-1)

  else:
    # remove segload MMU entries
    for k in AddressDic.keys():
      if '_text_wcdma_' in k or '_text_tdscdma_' in k:
        print 'Removing unused segment loading MMU entry "%s"' % k
        del AddressDic[k]

# begin continueOffsetMappingAtNext changes
OffsetMappingDic = {
  '4k': 1 << 12,
  '16k': 1 << 14,
  '64k': 1 << 16,
  '256k': 1 << 18,
  '1m': 1 << 20,
  '4m': 1 << 22,
  '16m': 1 << 24}

adjusted_items = []

for item in AddressDic:
  if 'continueOffsetMappingAtNext_'.lower() in item.lower():
    # adjust the address to move more entries into the previous MMU entry
    split = item.lower().split('_')
    alignment = 1

    try:
      alignment = split[split.index('continueOffsetMappingAtNext'.lower()) + 1]
      alignment = OffsetMappingDic[alignment]

    except KeyError:
      print 'Continue offset mapping at next...what?'
      sys.exit(-1)

    new_address = (AddressDic[item] + alignment - 0x1) & ~(alignment - 0x1)

    adjusted_items.append((item, AddressDic[item], new_address))

    AddressDic[item] = new_address

if len(adjusted_items) > 0:
  print '\nAdjusted MMU entries:'
  print '%-12s%-12s%s' % ('Old addr', 'New addr', 'Name')

for name, old_address, new_address in adjusted_items:
  print '%-12s%-12s%s' % (
    '0x%08x' % old_address,
    '0x%08x' % new_address,
    name)
# end continueOffsetMappingAtNext changes

PageSizeLst = [4*1024, 16*1024, 64*1024, 256*1024, 1024*1024, 4*1024*1024, 16*1024*1024]
PageSize2IdDic = {4*1024:1, 16*1024:(1<<1), 64*1024:(1<<2), 256*1024:(1<<3), 1024*1024:(1<<4), 4*1024*1024:(1<<5), 16*1024*1024:(1<<6)}

PageSizeLst.reverse ()
TLBCnt = 0

TLBAdd2SizeIdDic = {}
TLBAddLst = []

RegionAddLst = []
RegionAdd2SizeDic = {}

if (len (AddressDic) != 0):
 if (ValueDic ['image_vend'] != 0) and (ValueDic ['image_vend'] != 0xFFFFFFFF):
  EndAddress = ValueDic ['image_vend']
  AddressDic ['end'] = EndAddress		# Qshrink has updated image_vend, end doesn't contains the actual image boundary anymore. Lets's restore it.
 else:
  EndAddress = AddressDic ['end']

# if EndAddress < AddressDic ['end']:
#  print 'image_vend value is inconsistent'
#  print 'image_vend: ' + hex (ValueDic ['image_vend'])
#  print 'end:        ' + hex (AddressDic ['end'])
#  sys.exit (-1)

 if qurtos_symbols is True:
  EndAddress      = (EndAddress + (EndAddressAlignment - 1))  & ~(EndAddressAlignment -1)
 else:
  EndAddress      = (EndAddress + AddressDic [Gen2symDic ['app_heap_size']] + AddressDic [Gen2symDic ['main_stack_size']] + (EndAddressAlignment - 1))  & ~(EndAddressAlignment -1)

 Region1StartAdd = AddressDic ['start']

# RegionAddLst.append (AddressDic ['start'])
# RegionAddLst.append (EndAddress)
 

 if (Region1StartAdd & 0x1000)  != 0:
  print 'Start address is not aligned: ' + FormatHex (hex (Region1StartAdd))
  sys.exit (-1)

 Address2RegionDic = {}
 for Item in AddressDic:
  if (Item.find ('__MMU_region_') == 0):
   RegionAddLst.append (AddressDic [Item])
   Address2RegionDic [AddressDic [Item]] = Item
  if AddressDic [Item] not in Address2RegionDic:   # 'Start' region must not overrides a mmu_start region
   Address2RegionDic [AddressDic [Item]] = Item


 if AddressDic ['start'] not in RegionAddLst:	   # 'Start' region added only if no mmu_start region exists at the same address
  RegionAddLst.append (AddressDic ['start'])

 RegionAddLst.append (EndAddress)

 RegionAddLst.sort ()
 
 PrevAdd = ''
 for Address in RegionAddLst:  
  if (PrevAdd != ''):
   RegionAdd2SizeDic [PrevAdd] = Address - PrevAdd
  PrevAdd = Address

 RefAttrDic = {	'name':'',
                'lock':'0',
                'VA':0,
                'PA':0,
                'CCCC':7,
                'A0':0,
		'U':1,
  		'R':1,
  		'W':1,
  		'X':1,
  		'ASID':0,
  		'G':1,
  		'V':1,
      'continueOffsetMappingAtNext':''}


 FmtAttrDic = {	'name':'',
                'lock':'',
                'VA':'x',
                'PA':'x',
                'CCCC':'',
                'A0':'',
		'U':'',
  		'R':'',
  		'W':'',
  		'X':'',
  		'ASID':'',
  		'G':'',
  		'V':''}

 RefAttrLst = [	'name',
                'lock',
                'VA',
                'PA',
                'CCCC',
                'A0',
		'U',
  		'R',
  		'W',
  		'X',
  		'ASID',
  		'G',
  		'V']

 NonTLBAttrLst = ['name','lock','continueOffsetMappingAtNext']

 RegionAddtoAttrDic = {}

 for Address in RegionAddLst:
  if (Address in RegionAdd2SizeDic) and (Address in Address2RegionDic):
#   print 'Region ' +  Address2RegionDic [Address] + ' ' + str(hex (Address)) + ' size ' + str(RegionAdd2SizeDic [Address])
   if Address2RegionDic [Address].find ('__MMU_region_unmapped') == 0:
    print '\nRegion unmapped ' + FormatHex (str(hex (Address))) + ' size ' + str(RegionAdd2SizeDic [Address])
    continue  

   RegionAddtoAttrDic [Address] = {}
   for Item in RefAttrDic:
    RegionAddtoAttrDic [Address][Item] = RefAttrDic [Item]

   if Address2RegionDic [Address].find ('__MMU_region_start') == 0:
    RegionAddtoAttrDic [Address]['PA']   = Address    # Set the default PA. Can be overriden next.
    RegionAddtoAttrDic [Address]['VA']   = Address    
    Split = Address2RegionDic [Address].partition ('__MMU_region_start')[2].split ('_')
    Found = ''
    for Item in Split:
     if Item =='continueOffsetMapping':
       RegionAddtoAttrDic [Address]['PA'] = PrevPAaddr+RegionAddtoAttrDic[Address]['VA']-PrevVAaddr
     if Found in RefAttrDic:
      if Found not in NonTLBAttrLst:
       if (Item.find ('0x') != -1):
        RegionAddtoAttrDic [Address][Found] = long (Item, 16)
       else:
        RegionAddtoAttrDic [Address][Found] = long (Item)
      else:
       if Found == 'continueOffsetMappingAtNext':
         RegionAddtoAttrDic [Address]['PA'] = PrevPAaddr+RegionAddtoAttrDic[Address]['VA']-PrevVAaddr
       else:
         RegionAddtoAttrDic [Address][Found] = Item
      Found = ''
     else:
      Found = Item
   else:
    RegionAddtoAttrDic [Address]['PA']   = Address
    RegionAddtoAttrDic [Address]['VA']   = Address    
   
   PrevVAaddr =RegionAddtoAttrDic[Address]['VA']
   PrevPAaddr =RegionAddtoAttrDic[Address]['PA']

   DataString = '\n'
   for Item in RefAttrLst:
    if Item == 'name':
     Tail = '\n'
    else:
     Tail = ' '
    if FmtAttrDic [Item] == 'x':
     DataString += Item + ':' + FormatHex (hex(RegionAddtoAttrDic [Address][Item])) + Tail
    else:
     DataString += Item + ':' + str(RegionAddtoAttrDic [Address][Item])  + Tail

   print DataString
   
#   print '\n' + RegionAddtoAttrDic [Address]['name'] + '\tPA\t' + FormatHex (hex(RegionAddtoAttrDic [Address]['PA'])) + '\tLock ' + RegionAddtoAttrDic [Address]['lock']
#   for Item in RefAttrDic:
#    if Item != 'name':
#     print Item + '\t' + hex(RegionAddtoAttrDic [Address][Item])
#    else:
#     print Item + '\t' + RegionAddtoAttrDic [Address][Item]


   RegionStartAdd = Address
   RegionLen      = RegionAdd2SizeDic [Address]  

   if ((RegionStartAdd & (0x1000-1)) != 0):
    print 'The address ' + FormatHex (hex(RegionStartAdd)) + ' is not 4k aligned'
    sys.exit (-1)

   if ((RegionLen & (0x1000-1)) != 0):
    print 'The size ' + hex(RegionLen) + ' is not a multiple of 4k'
    sys.exit (-1)

   while RegionLen > 0:
#    print RegionLen
    for Item in PageSizeLst:
     if ((RegionStartAdd & ~((0xFFFFFFFF) << long(math.log (Item, 2)))) == 0) and (RegionLen >= Item):
      print FormatHex (hex (RegionStartAdd)) +'\tSize\t' + hex(Item)    
      TLBAdd2SizeIdDic [RegionStartAdd] = Item
      TLBAddLst.append (RegionStartAdd)
      RegionStartAdd += Item
      RegionLen      -= Item  
      TLBCnt += 1
      break

 print '\nNb TLB: ' + str (TLBCnt)
 
 if (TLBCnt > MaxTlbCnt):
  print 'Too many TLB used entries'
  sys.exit (-1)
 
 LastRegionAddress  = AddressDic [Gen2symDic ['tlb_dump']]

 if LastRegionAddress != 0:
  Arg2File = open (Temp + 'Arg2.lst', 'w')

  Arg2File.write (ElfFile + ' ' + FormatHex (hex(LastRegionAddress)) + ' -w ')

  TLBLockLst = []
  TLBUnlockLst = []

  for Address in TLBAddLst:
   if Address in RegionAddLst:
    CurRegionAddtoAttrDic = RegionAddtoAttrDic [Address]
    PhysAddress = CurRegionAddtoAttrDic ['PA']

   PA = PhysAddress >> 12
   VA = Address >> 12   
   PhysAddress += TLBAdd2SizeIdDic [Address]

   TLBLsw = (PA << 1) 
   TLBLsw |= PageSize2IdDic [TLBAdd2SizeIdDic [Address]]
   TLBLsw |= (CurRegionAddtoAttrDic ['CCCC'] << 24) 
   TLBLsw |= (CurRegionAddtoAttrDic ['U'] << 28) 
   TLBLsw |= (CurRegionAddtoAttrDic ['R'] << 29) 
   TLBLsw |= (CurRegionAddtoAttrDic ['W'] << 30) 
   TLBLsw |= (CurRegionAddtoAttrDic ['X'] << 31)
   TLBMsw = (VA) 
   TLBMsw |= (CurRegionAddtoAttrDic ['ASID'] << 20) 
   TLBMsw |= (CurRegionAddtoAttrDic ['A0'] << 27) 
   TLBMsw |= (CurRegionAddtoAttrDic ['G'] << 30) 
   TLBMsw |= (CurRegionAddtoAttrDic ['V'] << 31)

   if CurRegionAddtoAttrDic ['lock'] == '1':
    TLBLockLst.append ((TLBMsw << 32) | TLBLsw)
   else:
    TLBUnlockLst.append ((TLBMsw << 32) | TLBLsw)

  # Locked entries must be mapped first
  TLBAllLst = [TLBLockLst, TLBUnlockLst]
    
  for TLBLst in TLBAllLst:
   for TLBItem in TLBLst:
    TLBLsw = TLBItem & 0xFFFFFFFF
    TLBMsw = TLBItem >> 32

#    print hex (TLBLsw)
#    print hex (TLBMsw)
#    print FormatHex (hex(TLBMsw)) + FormatHex (hex(TLBLsw))[2:]

    ByteRes = HexToLendianByteList (FormatHex (hex(TLBLsw)))
    for Item in ByteRes:
     Arg2File.write (Item + ' ')

    ByteRes = HexToLendianByteList (FormatHex (hex(TLBMsw)))
    for Item in ByteRes:
     Arg2File.write (Item + ' ')

  if qurtos_symbols is False:
   image_vend = EndAddress
   Arg2File.write ('-w ' + FormatHex (hex(AddressDic ['image_vend'])) + ' ')
   ByteRes = HexToLendianByteList (FormatHex (hex(image_vend)))
   for Item in ByteRes:
    Arg2File.write (Item + ' ')

   image_pend = image_vend 
   Arg2File.write ('-w ' + FormatHex (hex(AddressDic ['image_pend'])) + ' ')
   ByteRes = HexToLendianByteList (FormatHex (hex(image_pend)))
   for Item in ByteRes:
    Arg2File.write (Item + ' ')

   image_end = AddressDic ['end']
   Arg2File.write ('-w ' + FormatHex (hex(AddressDic ['image_end'])) + ' ')
   ByteRes = HexToLendianByteList (FormatHex (hex(image_end)))
   for Item in ByteRes:
    Arg2File.write (Item + ' ')


   image_vstart = AddressDic ['start']
   Arg2File.write ('-w ' + FormatHex (hex(AddressDic ['image_vstart'])) + ' ')
   ByteRes = HexToLendianByteList (FormatHex (hex(image_vstart)))
   for Item in ByteRes:
    Arg2File.write (Item + ' ')

   image_pstart = image_vstart
   Arg2File.write ('-w ' + FormatHex (hex(AddressDic ['image_pstart'])) + ' ')
   ByteRes = HexToLendianByteList (FormatHex (hex(image_pstart)))
   for Item in ByteRes:
    Arg2File.write (Item + ' ')

   BLASTK_tlb_first_replaceable = len(TLBLockLst)
   Arg2File.write ('-w ' + FormatHex (hex(AddressDic [Gen2symDic ['tlb_first_replaceable']])) + ' ')
   ByteRes = HexToLendianByteList (FormatHex (hex(BLASTK_tlb_first_replaceable)))
   for Item in ByteRes:
    Arg2File.write (Item + ' ')



  Arg2File.close ()

  PrcsStdOut = open (Temp + 'map2.lst', 'w')
  RetCode = call(['python', PyPath + 'elfgets3.py', '-f', Temp + 'Arg2.lst'], stdout = PrcsStdOut)
  if RetCode != 0:
    print 'elfgets3 write failed'
    sys.exit(RetCode)

  print str(len(TLBLockLst)) + ' locked entries'  
