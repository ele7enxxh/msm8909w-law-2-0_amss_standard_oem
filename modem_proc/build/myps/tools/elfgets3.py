# A. Dambre
# ElfGet <ElfFile> <Address/symbol> [Size]
# Read the required number of bytes (default is 4) at the required virtual
# address in the ELF file.
# ElfGet <ElfFile> -m [Size]
# Print a map based on the physical addresses
# Optionaly Size indicate the memory space. Default value is 64 MB

# This version handles:
# - multiple commands (Next command specified with -n, except for -w)
# - Symbol table is read only once
# - multiple symbols update (syntax: elfgets3.py image.elf FirstSym-Add -w NewByteValue1 NBV2 -w SecondSym-Add NBV3 NBV4...)


import os
import sys
import glob
import array
from subprocess import call

DecToHex = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F']

_DecToHex = []

Esc={'\a':r'\a', '\b':r'\b', '\c':r'\c', '\f':r'\f', '\n':r'\n', '\r':r'\r', '\t':r'\t',
     '\v':r'\v', '\'':r'\'', '\"':r'\"', '\0':r'\0', '\1':r'\1', '\2':r'\2', '\3':r'\3',
     '\4':r'\4', '\5':r'\5', '\6':r'\6', '\7':r'\7', '\8':r'\8', '\9':r'\9'}

##############################################################################

def Raw(StrIn):
 StrOut=''
 for Char in StrIn:
  try: StrOut += Esc [Char]
  except KeyError: StrOut += Char
 return StrOut

##############################################################################

def GetBinData (BinFile, Offset, Size):
 BinFile.seek (Offset)
 BinLine = ElfFile.read (Size)
 return array.array('B', BinLine)

##############################################################################

def GetRawData (BinFile, Offset, Size):
 BinFile.seek (Offset)
 BinLine = ElfFile.read (Size)
 return Raw (BinLine)

##############################################################################

def ToAHex (Array):
 HexStr = ''
 for Value in Array:
  HexStr = HexStr + DecToHex[Value/16] + DecToHex[Value%16]       # Convert each value into ascii hexadecimal

 return HexStr

##############################################################################

def ToAHexCs (Array, Cs):
 HexStr = []
 _HexStr = ''
 Idx = 0

 for Value in Array:
  if (Cs == 1):
   _HexStr = _DecToHex[Value] + _HexStr
  else:
   _HexStr = _HexStr + _DecToHex[Value]

  Idx += 1
  if (Idx == 4):
   Idx = 0
   HexStr.append (_HexStr)
   _HexStr = ''

 return HexStr


##############################################################################

def _ToAHex (Array):
 HexStr = ''
 for Value in Array:
  HexStr = HexStr + ' ' + DecToHex[Value/16] + DecToHex[Value%16]       # Convert each value into ascii hexadecimal

 return HexStr

##############################################################################

def LiEndian (Array):
 NewArray = []

 for Item in Array:
  NewArray.insert(0, Item)
 return NewArray

##############################################################################

def DataDecode (Scheme, Data):
 if (Scheme == 1):
  return LiEndian (Data)
 else:
  return Data

##############################################################################

def ToValue (Array):
 Len = len (Array) - 1
 Value = 0

 for Digit in Array:
  Value += Digit * (2 ** (8 * Len))
  Len -= 1

 return Value

##############################################################################

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


##############################################################################

def FormatHex (HexValue):
 if (HexValue [-1] == 'L'):
  HexValue = HexValue [:-1]
 if (HexValue [0:2] == '0x'):
  HexValue = HexValue [2:]

 while (len (HexValue) < 8):
  HexValue = '0' + HexValue

 HexValue = '0x' + HexValue

 return HexValue

##############################################################################

def _FormatHex (HexValue, Size):
 if (HexValue [-1] == 'L'):
  HexValue = HexValue [:-1]
 if (HexValue [0:2] == '0x'):
  HexValue = HexValue [2:]

# print HexValue
# Z = '00000000'
# HexValue = Z [0:Size*2-len(HexValue)] + HexValue
 while (len (HexValue) < Size*2):
  HexValue = '0' + HexValue
# print HexValue

 HexValue = '0x' + HexValue

 return HexValue

##############################################################################

def GetSymbol (BinFile, Offset):
 BinFile.seek (Offset)
 BinLine = BinFile.read (20)

 while True:
  Part = BinLine.partition ('\0')
  if (len (Part [1]) > 0):
   break
  BinLine += BinFile.read (20)

 return Part [0]


##############################################################################

def ProcessList (List, SecDic, MaxPhysAdd):

 RoSize  = 0
 RwSize  = 0
 ZiSize  = 0
 PadSize = 0
 MemSize = 0
 Address = 0
 Size    = 0
 Pad     = 0
 LastSectionAddress = 0
 LastSectionSize    = 0

 List.sort ()

# print List

 print 'PAddr      VAddr\tType\tSize(KB)\tLevel(MB)  Pad(KB)    Level(MB)'

 for Item in List:
  Split = Item.split ()

  if (long (Split [0], 16) < MaxPhysAdd):
   LastSectionAddress = long (Split [0], 16)
   LastSectionSize    = long (Split [2], 16)

   if (Address != 0): # and (LastSectionAddress == long (Split [0], 16)):
    Pad = LastSectionAddress - (Address + Size)
    if (Pad != 0):
#    print 'Pad before section ' + Split [3] + ': ' + str(Pad) + ' bytes'
     PadSize += Pad
  else:
   Pad = 0

  Address = long (Split [0], 16)
  Size = long (Split [2], 16)
  MemSize += Size

  ExtType = str (hex (long (Split [3])))
  Split [3] = str (long (Split [3]) & 0xF)

#  print Split [0] + '-' + Split [1] + '\t' + Split [3]+ '\t' + ValueToUnitStr (Size, 1024, 8) + '\t' + ValueToUnitStr (MemSize, 1024*1024, 4) + '\t' + ValueToUnitStr (Pad, 1024, 8) + '\t' + ValueToUnitStr (PadSize, 1024*1024, 5) + '\t' + ExtType
  print Split [0] + '-' + Split [1] + '\t' + Split [3]+ '\t' + ValueToUnitStr (Size, 1024, 8) + '\t' + ValueToUnitStr (MemSize, 1024*1024, 4) + '\t' + ValueToUnitStr (Pad, 1024, 8) + '\t' + ValueToUnitStr (PadSize, 1024*1024, 5)

  if (SecDic [Split [3]] == 'Ro'):
   RoSize += Size
  elif (SecDic [Split [3]] == 'Rw'):
   RwSize += Size
  elif (SecDic [Split [3]] == 'Zi'):
   ZiSize += Size
  elif (SecDic [Split [3]] == 'Pd'):
   if (Size != 0):
    print 'Pad section ' + Split [3] + ': ' + str(Size) + ' bytes'
    PadSize += Size
  else:
   print 'Not found: ' + Split [3]

 print '\nStart address:\t' + FormatHex(List [0].split()[0])
# print 'Last address:  ' + str(hex(long(List [len(List)-1].split()[0], 16) + long(List [len(List)-1].split()[1], 16)))[:-1]
 if (long(List [0].split()[0], 16) > MaxPhysAdd):
  print 'Start address beyond memory boundary (' + str(MaxPhysAdd/(1024*1024)) + ' MB)'
 else:
  print 'Last address:\t' + FormatHex(str(hex(LastSectionAddress + LastSectionSize)))
# Size = long(List [len(List)-1].split()[0], 16) + long(List [len(List)-1].split()[1], 16) - long (List [0].split()[0], 16)
  Size = LastSectionAddress + LastSectionSize - long (List [0].split()[0], 16)
  print 'Phys mem size:\t' + FormatHex(str(hex(Size))[:-1]) + '\t' + ValueToUnitStr (Size, 1024, 6) + ' kB\t'  + ValueToUnitStr (Size, (1024*1024), 3) + ' MB\n'
 print 'Ro \t' + str (RoSize).rjust(10) + '\t' + ValueToUnitStr (RoSize, 1024, 6) + ' kB\t'  + ValueToUnitStr (RoSize, (1024*1024), 3) + ' MB'
 print 'Rw \t' + str (RwSize).rjust(10) + '\t' + ValueToUnitStr (RwSize, 1024, 6) + ' kB\t'  + ValueToUnitStr (RwSize, (1024*1024), 3) + ' MB'
 if (ZiSize > 0):
  print 'Zi \t' + str (ZiSize).rjust(10) + '\t' + ValueToUnitStr (ZiSize, 1024, 6) + ' kB\t'  + ValueToUnitStr (ZiSize, (1024*1024), 3) + ' MB'

 Total = RoSize + RwSize + ZiSize

 print 'Total \t' + str (Total).rjust(10) + '\t' + ValueToUnitStr (Total, 1024, 6) + ' kB\t'  + ValueToUnitStr (Total, (1024*1024), 3) + ' MB'

 print 'Pad \t' + str (PadSize).rjust(10) + '\t' + ValueToUnitStr (PadSize, 1024, 6) + ' kB\t'  + ValueToUnitStr (PadSize, (1024*1024), 3) + ' MB'




##############################################################################
def FormatResult (Address, Size, FmtLst, ElfFile, SectStartVA, SectOffset):

 TmpAddress = Address

 LenFmtLst = len (FmtLst)
 SubIdx = 0
 Res = ''
 Stop = False

 if (FmtLst [0][0] != '#'):
  while (Stop == False):
   Offset = TmpAddress - SectStartVA
   Step = long (FmtLst [SubIdx].split ()[1])
   Result = GetBinData (ElfFile, SectOffset + Offset, Step)
#   print FmtLst [SubIdx].split ()[0] + '\t' + _FormatHex(str(hex(ToValue (LiEndian (Result)))), Step)
   Res += FmtLst [SubIdx].split ()[0] + '\t' + _FormatHex(str(hex(ToValue (LiEndian (Result)))), Step) + '\n'
   Size    -= Step
   TmpAddress += Step
   SubIdx  += 1
   if (SubIdx == LenFmtLst):
    SubIdx = 0
    if (Size <= 0):
     Stop = True
 else:
  for Item in FmtLst:
   if (Item [0][0] != '#'):
    Res += Item.split ()[0] + '\t'

  while (Size > 0):
   Res += '\n'
   for Item in FmtLst:
    if (Item [0][0] == '#'):
     Offset = TmpAddress - SectStartVA
     Step = long (FmtLst [SubIdx].split ()[1])
     Result = GetBinData (ElfFile, SectOffset + Offset, Step)
     Result = ToValue (LiEndian (Result))
     Size       -= Step
     TmpAddress += Step
     Pos = 0
    else:
     NbBits = long (Item.split ()[1])
     Field  = (Result >> Pos) & ((1 << NbBits)-1)
     Pos += NbBits
#     print hex(Field)
     Res += _FormatHex(str(hex(Field)), (NbBits+7)/8) + '\t'



 return Res


##############################################################################
#  Program entry
##############################################################################

Symb2AddAndSizeDic = {}
SymbolLoaded = False

WAddressLst   = []
WSizeLst      = []
WDataLstOfLst = []



if len(sys.argv) < 3:
 print "ElfGet <ElfFile> <-m/Address/-t> [Size/Fmt/string] or [-w data list]"

 print "-m                      print program headers"
 print "-h                      print section headers"
 print "-ha                     print all section headers"
 print "-s                      produce the symbol file"
 print "Address/symbol Size/Fmt read the address/symbol"
 print " ... -w  Byte1 Byte2... update with new data at the specified address"
 print "-t Pattern              find pattern in any section (from aligned addresses only)"

 sys.exit()

if (sys.argv [1] == '-f'):
 try:
  ArgFile = open (sys.argv [2], 'r')
 except IOError:
  print 'Unable to open the file ' + sys.argv [2]
  sys.exit (-1)

 ArgLst = 'none'
 for NewLine in ArgFile:
  ArgLst += ' ' + NewLine

 ArgLst =  ArgLst.split ()
else:
 ArgLst = sys.argv

ElfFileName = ArgLst [1]


try:
 ElfFile = open(ElfFileName, 'rb')
except IOError:
 print 'Unable to open the file ' + ElfFileName
 sys.exit (-1)

ArgIdx = 2

while (ArgIdx < len (ArgLst)):

 Args = []
 Args.append (ArgLst [0])
 Args.append (ArgLst [1])

 while (ArgIdx < len (ArgLst)) and (ArgLst [ArgIdx] != '-n'):
  Args.append (ArgLst [ArgIdx])
  ArgIdx += 1

 ArgIdx += 1

 ArgsStr = ''
 for Idx in range (1, len (Args)):
  ArgsStr += Args [Idx] + ' '

 print '\n' + ArgsStr


 Map = False
 MaxPhysAdd = 0xFFFFFFFF
 
 Address    = 0xFFFFFFFF
 FileOffset = 0xFFFFFFFF
 FindSymbAdd = ''
 DataLst    = []
 ElfUpdate  = False
 Size       = 0
 FmtFileName = ''
 SizeDic    = {}
 SymDic     = {}
 FindStr = ''
 SymbolFile = False
 
 if (Args [2].find ('-m') == -1) and (Args [2].find ('-h') == -1) and (Args [2].find ('-o') == -1) and (Args [2].find ('-t') == -1) and (Args [2].find ('-s') == -1):
  try:
   Address = long (Args [2], 16)
  except ValueError:
   FindSymbAdd = Args [2]
 
  if (len (Args) > 3) and (Args [3] != '-w'):
   if (len(Args) == 4):
    try:
     Size = long (Args [3])
    except ValueError:
     FmtFileName = Args [3]
 
  elif (len (Args) > 3) and (Args [3] == '-w'):
   # So here it's going to be a little bit bizarre since the initial implentation didn't support multiple write:
   # the first write remains compatible with the original interface: elgets.py elf Address -w B1 B2 ...
   # Subsequent writes occur with the write address following -w: 
   # elgets.py elf Address1 -w B1 B2 ... -w Address2 Bx By ... -w Address3 Bz ...

   ElfUpdate = True

   # Well let's fix the arg list first to get it consistent:
   TmpSaved = Args [2]
   Args [2] = Args [3]
   Args [3] = TmpSaved

   WIndex = 0
   ArgIndex = 2

   while ArgIndex < len (Args):
    if (Args [ArgIndex] != '-w'):
     break
    
    ArgIndex += 1

    WAddressLst.append (Args [ArgIndex])

    CurLen = 0
    while (ArgIndex + CurLen + 1 < len (Args)) and (Args [ArgIndex + CurLen + 1][0] != '-'):
     CurLen += 1

    WDataLstOfLst.append ([])
    Size = 0

    try:
     Value = long (Args [ArgIndex+1], 16)
     # The list of values is provided on the command line.
     for Idx in range (ArgIndex+1, ArgIndex+1+CurLen):
      WDataLstOfLst [WIndex].append (Args [Idx])
      Size += 1
    except ValueError:
     ValueFileName = Args [ArgIndex+1]
     try:
      ValueFile = open (ValueFileName, 'r')
     except IOError:
      print 'Unable to open the file ' + ValueFileName
      sys.exit (-1)
 
     for NewLine in ValueFile:
      Split = NewLine.split ()
      for Value in Split:
#       DataLst.append (Value)
       WDataLstOfLst [WIndex].append (Value)
       Size += 1
   
    WSizeLst.append (Size)

    ArgIndex += CurLen + 1
    WIndex += 1    
    
  if (FindSymbAdd == '') and (Size == 0):
   Size = 4
 
 elif (Args [2].find ('-o') != -1):
  FileOffset = long (Args [3], 16)
  if (len(Args) >= 5):
   try:
    Size = long (Args [4])
   except ValueError:
    FmtFileName = Args [4]
 
   if (len(Args) == 6):
    Size = long (Args [5])
  else:
   Size = 4
 
 elif (Args [2].find ('-t') != -1):
  FindStr = Args [3]
 elif (Args [2].find ('-s') != -1):
  SymbolFile = True
 else:
  Map = True
  if (len(Args) == 4):
   MaxPhysAdd = long (Args [3]) * 1024 * 1024
  else:
   MaxPhysAdd = 0xFFFFFFFF
 
 OutFileName  = 'res'
 
 ElfFile.seek (0)
 
 ElfSig = 'ELF'
 
 Verbose = False
 
 ElfHeaderTup = (
  ['e_entry',     24, 4],        # Virtual address of the entry point of the image
  ['e_phoff',     28, 4],        # Program header table's file offset in bytes
  ['e_shoff',     32, 4],        # Section header table's file offset in bytes
 # ['flags',       36, 4],
  ['e_ehsize',    40, 2],        # Elf header size in bytes
  ['e_phentsize', 42, 2],        # Size of one entry in the program header table
  ['e_phnum',     44, 2],        # Number of entries in the program header table
  ['e_shentsize', 46, 2],        # Size of one entry on the section header table
  ['e_shnum',     48, 2],        # Number of entries in the section header table
  ['e_shstrndx',  50, 2])        # Section header table index
 
 SectHeaderTup = (
  ['sh_name',       0, 4],       # Name of the section. Index in the header string
  ['sh_type',       4, 4],       # Section content
  ['sh_flags',      8, 4],       # Misc attributes
  ['sh_addr',      12, 4],       # Address of the section in the memory image
  ['sh_offset',    16, 4],       # Section's file offset in bytes
  ['sh_size',      20, 4],       # Section's size in bytes
  ['sh_link',      24, 4],
  ['sh_info',      28, 4],
  ['sh_addralign', 32, 4],
  ['sh_entsize',   36, 4])       # Table's fixed entry' size in bytes (when any )
 
 PrgHeaderTup = (
  ['p_type',    0, 4],
  ['p_offset',  4, 4],
  ['p_vaddr',   8, 4],
  ['p_paddr',  12, 4],
  ['p_filesz', 16, 4],
  ['p_memsz',  20, 4],
  ['p_flags',  24, 4],
  ['p_align',  28, 4])
 
 
 
 SectDesc = []
 PrgDesc = []
 
 for Idx in range (0, 256):
  _DecToHex.append (_FormatHex (str (hex(Idx)), 1)[2:])
 
 BinLine = ElfFile.read (len (ElfSig) + 1)
 
 Result = array.array('B', BinLine [0])
 
 if (Result [0] == 0x7f) and (BinLine [1:] == ElfSig):
  CodeScheme = GetBinData (ElfFile, 5, 1) [0]
 
 # Result = GetBinData (ElfFile, 24, 100)
 # print ToAHex (Result)
 
  for Header in ElfHeaderTup:
   Result = GetBinData (ElfFile, Header [1], Header [2])
   Header.append (ToValue (DataDecode (CodeScheme, Result)))
   if (Verbose == True):
    print Header [0] + ' ' + ToAHex (DataDecode (CodeScheme, Result))
 
   if (Header [0][0:4] == 'e_ph'):  # collect Offset + Num + size for program header
    PrgDesc.append (Header [3])
   elif (Header [0] == 'e_shstrndx'):
    SHStrIdx = long (ToAHex (DataDecode (CodeScheme, Result)), 16)
   elif (Header [0][0:4] == 'e_sh'):  # collect Offset + Num + size for section header
    SectDesc.append (Header [3])
 
  # Let's discover the Section headers
 
  FmtStr = '\n'
  for Header in SectHeaderTup:
   FmtStr += '\t' + Header [0]
 
  if (Verbose == True):
   print FmtStr
 
  NbSectHd = 0
 
 # print SectDesc
 
  SHName2AddRange = {}
 
  # Let's discover the Section headers
  for Idx in range (0, SectDesc [2]):
   Offset = SectDesc [0] + (Idx * SectDesc [1])
   ValueStr = ''
   for Header in SectHeaderTup:
    Result = GetBinData (ElfFile, Offset + Header [1], Header [2])
 
    Value = ToValue (DataDecode (CodeScheme, Result))
    Header.append (Value)  # ?
    ValueStr += '\t' + str(hex(Value))
 
    if Header [0] == 'sh_name':
     SHName = Value
    if Header [0] == 'sh_addr':
     SHAddr = Value
    if Header [0] == 'sh_size':
     SHSize = Value
 
   if SHSize > 0:
 #   if StrTabSec != '':
 #    SHName = StrTabSec [StrTabSec:StrTabSec.find ('\0')]
 #    print SHName
    SHName2AddRange [SHName] = str(hex(SHAddr)) + '\t' + str(hex(SHAddr + SHSize))
 
   if (Verbose == True) and (ValueStr != ''):
    print ValueStr
 
  # Let's build the address to symbol dictionary
 
  SymTableIdx = 0
  StringTableIdx = 0
  SHStringTableIdx = 0
 
  for Idx in range (3, len(SectHeaderTup [1])):
   if (SectHeaderTup [1][Idx] == 2):
    SymTableIdx = Idx
   elif (SectHeaderTup [1][Idx] == 3):
    if (SymTableIdx == 0) or (StringTableIdx != 0):
     SHStringTableIdx = Idx
    else:
     StringTableIdx = Idx
 
  if (SymTableIdx == 0) or (StringTableIdx == 0):
   print 'No Symbol'
  else:
   SymTabSecOffset = SectHeaderTup [4][SymTableIdx]              # Symbol table
   SymTabSecSize   = SectHeaderTup [5][SymTableIdx]
   SymTabSecEntrySize = SectHeaderTup [9][SymTableIdx]
 
   StrTabSecOffset = SectHeaderTup [4][StringTableIdx]           # String table
   StrTabSecSize   = SectHeaderTup [5][StringTableIdx]
 
   if (SHStringTableIdx != 0):
    SHStrTabSecOffset = SectHeaderTup [4][SHStringTableIdx]      # Section header name string table
    SHStrTabSecSize   = SectHeaderTup [5][SHStringTableIdx]
 
    ElfFile.seek (SHStrTabSecOffset)
    SHStrTabSec = ElfFile.read (SHStrTabSecSize)
 
    SHNameStr2AddRange = {}
 
    for SHNameOffset in SHName2AddRange:
     SHNameStr = SHStrTabSec [SHNameOffset:SHNameOffset+SHStrTabSec [SHNameOffset:].find('\0')]
     SHNameStr2AddRange [SHNameStr] = SHName2AddRange [SHNameOffset]
 
 #  print hex (SymTabSecOffset)
 #  Result = GetBinData (ElfFile, SymTabSecOffset, 2*SymTabSecEntrySize)
 #  print ToAHex (Result)
 
   if (SymbolLoaded == False) and ((FindSymbAdd != '') or (SymbolFile == True)):
    SymbolLoaded = True
  
    # Let's discover the symbol table. We need it to find the address
    # that we want to dump.
 
    SymTabSec = GetBinData (ElfFile, SymTabSecOffset, SymTabSecSize)
    SymTabSec = ToAHexCs (SymTabSec, CodeScheme)
 
    ElfFile.seek (StrTabSecOffset)
    StrTabSec = ElfFile.read (StrTabSecSize)
 
    Pos = 0
    Step = SymTabSecEntrySize/4
    Symbol = ''
    Limit = len (SymTabSec)
 
    while True:
     StrOffset = long (SymTabSec [Pos], 16)
 
     SymLen = 0
     while StrTabSec [StrOffset+SymLen] != '\0':
      SymLen +=1
 
     Symbol = StrTabSec [StrOffset:StrOffset+SymLen]
 
     Symb2AddAndSizeDic [Symbol] = SymTabSec [Pos+1:Pos+4]

     Pos += Step
 
     if (Pos >= Limit):
      break


   if ((FindSymbAdd != '') or (SymbolFile == True)):
 
    if (FindSymbAdd != '') and (FindSymbAdd.find ('**') != -1):
     FindSymbAdd = FindSymbAdd.partition ('**') [0]
     WildRead = True
     WildChar = False
    elif (FindSymbAdd != '') and (FindSymbAdd.find ('*') != -1):
     FindSymbAdd = FindSymbAdd.partition ('*') [0]
     WildRead = False
     WildChar = True
    else:
     WildChar = False
     WildRead = False

    for Symbol in Symb2AddAndSizeDic:     
     Address = long (Symb2AddAndSizeDic [Symbol][0], 16)
 
     Size = long (Symb2AddAndSizeDic [Symbol][1], 16)
 
     Attribute = Symb2AddAndSizeDic [Symbol][2]
 
     if (SymbolFile):
      print Symbol + '\t' + FormatHex(str(hex(Address))) + '\t' + str (Size) + '\t' + Attribute
     else: # if (Size > 0): # and (Attribute != '00010003'):
      if ((WildChar == True) and (Symbol.find (FindSymbAdd) == 0)):
       print Symbol + '\t' + FormatHex(str(hex(Address))) +'\t' + str(Size)
 #      print Symbol + ': ' + FormatHex(str(hex(Address))) + ' (' + str (Size) + ')'
      elif (Symbol == FindSymbAdd) or ((WildRead == True) and (Symbol.find (FindSymbAdd) == 0)):
       SizeDic [Address] = Size
       SymDic [Address] = Symbol
   
    if (SymbolFile) or (WildChar): # or (WildRead) => reading the data occurs later in that case
     continue

    if (len (SizeDic) == 0) and (WildRead == True):
     print 'Cant find ' + FindSymbAdd
     continue

  # Let's discover the Program headers
   
  FmtStr = '\n'
  for Header in PrgHeaderTup:
   FmtStr += '\t' + Header [0]
 
  if (Verbose == True):
   print FmtStr
 
  NbPgrHd = 0
 
 # print PrgDesc
 
  for Idx in range (0, PrgDesc [2]):
   Offset = PrgDesc [0] + (Idx * PrgDesc [1])
   ValueStr = ''
   for Header in PrgHeaderTup:
    Result = GetBinData (ElfFile, Offset + Header [1], Header [2])
    if (Header [0] == 'p_type'):
     if (ToValue (DataDecode (CodeScheme, Result)) == 0):
      break
     else:
      NbPgrHd += 1
 
    Header.append (ToValue (DataDecode (CodeScheme, Result)))
    ValueStr += '\t' + str(hex(ToValue (DataDecode (CodeScheme, Result))))
 
   if (Verbose == True) and (ValueStr != ''):
    print ValueStr
 
  TypeIdx   = 0
  OffsetIdx = 1
  VAddrIdx  = 2
  PAddrIdx  = 3
  FileSzIdx = 4
  MemSzIdx  = 5
  FlagsIdx  = 6
  AlignIdx  = 7
 
 
  if (Map == False):
 
   if (FindStr != ''):
    print 'Look for string: ' + FindStr
 
    Step = 4
 
    for Idx in range (0, NbPgrHd):
     Offset = 0
 
     while (Offset < PrgHeaderTup [FileSzIdx][3 + Idx]):
 #     if (PrgHeaderTup [VAddrIdx][3 + Idx] <= Address) and (PrgHeaderTup [VAddrIdx][3 + Idx] + PrgHeaderTup [FileSzIdx][3 + Idx] >= Address):
 
      if (FindStr == GetRawData (ElfFile, PrgHeaderTup [OffsetIdx][3 + Idx] + Offset, len (FindStr))):
       print 'Found: ' + FindStr +  ' at address: ' + FormatHex(str(hex(PrgHeaderTup [VAddrIdx][3 + Idx] + Offset)))
 
      Offset += Step
 
 #   for Idx in range (0, NbPgrHd):
 #    Str = GetRawData (ElfFile, PrgHeaderTup [OffsetIdx][3 + Idx], PrgHeaderTup [FileSzIdx][3 + Idx])
 #
 #    Offset = Str.find (FindStr)
 #    PrevOffset = 0
 #
 #    while (Offset != -1):
 #     print 'Found: ' + FindStr +  ' at address: ' + FormatHex(str(hex(PrgHeaderTup [VAddrIdx][3 + Idx] + Offset)))
 #     PrevOffset += Offset + len (FindStr)
 #     Offset = Str[:PrevOffset].find (FindStr)
 
 
   elif (FileOffset == 0xFFFFFFFF):
    # Read the content of the specified address
 
    FmtLst = []
 
    if (FmtFileName != ''):
     try:
      FmtFile = open (FmtFileName, 'r')
     except IOError:
      print 'Unable to open the file ' + FmtFileName
      sys.exit (-1)

     for Line in FmtFile:
      Split = Line.split ()
      for Item in Split:
       FmtLst.append (Item.replace (':', ' '))
 
 
    if (ElfUpdate == True):
     # We are going to update the Elf file with the provided data later
     # First, let's convert the symbolic addresses into actual addresses if needed

     for WAddIdx in range (0, len (WAddressLst)):         
      Size = -1
      try:
       Address = long (WAddressLst [WAddIdx], 16)
      except ValueError:
       if WAddressLst [WAddIdx] in Symb2AddAndSizeDic:
        Address = long (Symb2AddAndSizeDic [WAddressLst [WAddIdx]][0], 16)
        Size = long (Symb2AddAndSizeDic [WAddressLst [WAddIdx]][1], 16)
       else:
        print WAddressLst [WAddIdx] + ' cannot be found'
        sys.exit ()
         
      if (Size == -1):
       SizeDic [Address] = WSizeLst [WAddIdx]
      else:
       SizeDic [Address] = Size

      SymDic [Address] = WAddressLst [WAddIdx]
      WAddressLst [WAddIdx] = Address

     WAddress2OffsetDic = {}


    if (len (SizeDic) == 0):
     SizeDic [Address] = Size
     SymDic [Address] = ''
 
    for Address in SizeDic:
     Found = False
     for Idx in range (0, NbPgrHd):
      Size = SizeDic [Address]
 
      if (PrgHeaderTup [VAddrIdx][3 + Idx] <= Address) and (PrgHeaderTup [VAddrIdx][3 + Idx] + PrgHeaderTup [FileSzIdx][3 + Idx] >= Address):
       TmpAddress = Address
       Found = True
 
       if (len (FmtLst) > 0):   # Follow the format file indication
        print SymDic [Address] + '\t' + FormatHex(str(hex(Address))) + '\t' + str(Size)
        print FormatResult (Address, Size, FmtLst, ElfFile, PrgHeaderTup [VAddrIdx][3 + Idx], PrgHeaderTup [OffsetIdx][3 + Idx])
 
       else:
        print SymDic [Address] + '\t' + FormatHex(str(hex(Address))) + '\t' + str(Size)
 
        Step = 4
        while (Size > 0):
         if (Size >= Step):
          Offset = TmpAddress - PrgHeaderTup [VAddrIdx][3 + Idx]
          Result = GetBinData (ElfFile, PrgHeaderTup [OffsetIdx][3 + Idx] + Offset, Step)
          Str = FormatHex(str(hex(TmpAddress))[:-1]) + '\t'
          Str += _ToAHex (Result).ljust(12)
 
          Str +=  '\t' + FormatHex(str(hex(ToValue (LiEndian (Result))))) # .rjust(10)
          print Str + '\t' + GetRawData (ElfFile, PrgHeaderTup [OffsetIdx][3 + Idx] + Offset, Step)
          TmpAddress += Step
          Size -= Step
         else:
          Step /= 2

       if (ElfUpdate == True):
        WAddress2OffsetDic [Address] = Address - PrgHeaderTup [VAddrIdx][3 + Idx] + PrgHeaderTup [OffsetIdx][3 + Idx]

     if (Found == False):
      print SymDic [Address] + '\t' + FormatHex(str(hex(Address))) + '\t' + str(Size)

    if (ElfUpdate == True):
     # We are going to update the Elf file now
        
     #_ElfFileName = ElfFileName.replace ('\\', '/')
     #if _ElfFileName.find ('/') != -1:
     # _ElfFileName = _ElfFileName.replace ('/', '/_')
     #else:
     # _ElfFileName = '_' + ElfFileName
     _ElfFileName = os.path.split(ElfFileName)
     _ElfFileName = os.path.join(
       _ElfFileName[0],
       ''.join(['_', _ElfFileName[1]]))
     
     try:
      _ElfFile = open(_ElfFileName, 'wb')
     except IOError:
      print 'Unable to open the file ' + _ElfFileName
      sys.exit (-1)

     ElfFile.seek (0)
     CurOffset = 0
     BlockSize = 1024

     WAdd2IndexDic = {}
     WAddIdx  = 0
     for Address in WAddressLst:
      WAdd2IndexDic [Address] = WAddIdx
      WAddIdx += 1

     WAddressLst.sort ()        
     WAddIdx  = 0
     Address  = WAddressLst [WAddIdx]
     Offset   = WAddress2OffsetDic [Address]
     DataLst  = WDataLstOfLst [WAdd2IndexDic [Address]]
     WBlkSize = len (DataLst)
 
     while (True):
      if (CurOffset + BlockSize <= Offset) or (WBlkSize == 0):
       Result = GetBinData (ElfFile, CurOffset, BlockSize)
       CurOffset += BlockSize
      elif (CurOffset == Offset):
       Result = GetBinData (ElfFile, CurOffset, WBlkSize)
       CurOffset += WBlkSize
       for Idx2 in range (0, len (Result)):
        Result [Idx2] = long (DataLst [Idx2], 16)
       
       WAddIdx += 1
       if (WAddIdx < len(WAddressLst)):
        Address  = WAddressLst [WAddIdx]
        Offset   = WAddress2OffsetDic [Address]
        DataLst  = WDataLstOfLst [WAdd2IndexDic [Address]]
        WBlkSize = len (DataLst)
       else:   
        WBlkSize = 0
      elif (WBlkSize > 0):
       Result = GetBinData (ElfFile, CurOffset, Offset - CurOffset)
       CurOffset += Offset - CurOffset

      if (len (Result) > 0):
       _ElfFile.write (Result)
      else:
       break
 
 #    break
 
 
   else:
    # Just read the file at the specified offset
 
    Step = 4
    while (Size > 0):
     if (Size >= Step):
      Result = GetBinData (ElfFile, FileOffset, Step)
      Str = str(hex(FileOffset))[:-1].rjust(10) + '\t'
      Str += _ToAHex (Result).ljust(12)
 
      Str +=  '\t' + FormatHex(str(hex(ToValue (LiEndian (Result))))) # .rjust(10)
      print Str + '\t' + GetRawData (ElfFile, FileOffset, Step)
      FileOffset += Step
      Size -= Step
     else:
      Step /= 2
 
  else:
   if ('-h' in Args) or ('-ha' in Args):
    if '-ha' in Args:
     AllSections = True
    else:
     AllSections = False
 
    # print the section headers
 
    SHLst = []
    PrevAdd = 0
    Pad = 0
 
    for SHName in SHNameStr2AddRange:
     Split = SHNameStr2AddRange [SHName].split ()
     if (long (Split [0], 16) != 0) or (AllSections == True):
      SHLst.append (FormatHex (Split [0]) + '\t' + FormatHex (Split [1]) + '\t' + SHName)
 
    SHLst.sort ()
 
    print 'Start       \tEnd      \tName \tSize \tPad'
 
    for Item in SHLst:
     Split = Item.split ()
     Size = long (Split [1], 16) - long (Split [0], 16)
 
     if PrevAdd != 0:
      Pad = long (Split [0], 16) - PrevAdd
 
     PrevAdd = long (Split [1], 16)
 
     print Item  + '\t' + str(Size) + '\t' + str (Pad)
 
   else:
    # Print a summary of the map (program headers)
 
    SectLst = []
 
    print '\nMemory map (Type: b2:R b1:W b0:E)\n'
 
    for Idx in range (3, NbPgrHd+3):
 
     if (PrgHeaderTup [TypeIdx][Idx] == 1): # and ((PrgHeaderTup [FlagsIdx][Idx] & 7) > 0):   # Select load segment only
      Sector = FormatHex (str(hex (PrgHeaderTup [PAddrIdx][Idx]))) + '\t' + FormatHex (str(hex (PrgHeaderTup [VAddrIdx][Idx]))) + '\t' + FormatHex (str(hex (PrgHeaderTup [MemSzIdx][Idx]))) + '\t' + str ((PrgHeaderTup [FlagsIdx][Idx])) + '\t' + str ((PrgHeaderTup [TypeIdx][Idx]))
      SectLst.append (Sector)
  #    print Sector
    # Bit 0 1 2
    #     E W R
 
    SectDic = {'0': 'Ro',   # Actually unknown
               '1': 'Ro',
               '2': 'Ro',
               '3': 'Ro',
               '4': 'Ro',
               '5': 'Ro',
               '6': 'Rw',
               '7': 'Ro'}
 
    ProcessList (SectLst, SectDic, MaxPhysAdd)
 
 elif (FileOffset != 0xFFFFFFFF):
  # Just read the file at the specified offset
 
  FmtLst = []
 
  if (FmtFileName != ''):
   try:
    FmtFile = open (FmtFileName, 'r')
   except IOError:
    print 'Unable to open the file ' + FmtFileName
    sys.exit (-1)

   for Line in FmtFile:
    Split = Line.split ()
    for Item in Split:
     FmtLst.append (Item.replace (':', ' '))
 
  if (len (FmtLst) > 0):   # Follow the format file indication
   print FormatResult (FileOffset, Size, FmtLst, ElfFile, 0, 0)
  else:
   Step = 4
   while (Size > 0):
    if (Size >= Step):
     Result = GetBinData (ElfFile, FileOffset, Step)
     Str = str(hex(FileOffset))[:-1].rjust(10) + '\t'
     Str += _ToAHex (Result).ljust(12)
 
     Str +=  '\t' + FormatHex(str(hex(ToValue (LiEndian (Result))))) # .rjust(10)
     print Str + '\t' + GetRawData (ElfFile, FileOffset, Step)
     FileOffset += Step
     Size -= Step
    else:
     Step /= 2
 
