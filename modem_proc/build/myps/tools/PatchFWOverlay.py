'''
=============================================================================

                  PatchFWOverlay . P Y

Copyright (c) 2012 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================
DESCRIPTION
    Elf modification script for adjusting FW overlay segment size.
=============================================================================


'''

import hashlib
import re
import time
import string
import struct
import csv
import os
import shutil
import sys

# ======================== start globals ========================

const = {
  # Constants as defined in the ELF specification
  # Nowhere near complete but sufficient for this program.

  # ELF Sizes
  'Elf32_Addr'    : 4,
  'Elf32_Half'    : 2,
  'Elf32_Off'     : 4,
  'Elf32_Sword'   : 4,
  'Elf32_Word'    : 4,
  # ELF Type
  'ET_NONE'       : 0,
  'ET_REL'        : 1,
  'ET_EXEC'       : 2,
  'ET_DYN'        : 3,
  'ET_CORE'       : 4,
  'ET_LOPROC'     : 0xff00, # lower processor-specific range
  'ET_HIPROC'     : 0xffff, # higher processor-specific range
  # ELF Machine
  'EM_NONE'       : 0, # Error?
  'EM_M32'        : 1,
  'EM_SPARC'      : 2,
  'EM_386'        : 3,
  'EM_68K'        : 4,
  'EM_88K'        : 5,
  'EM_860'        : 7,
  'EM_MIPS'       : 8,
  'EM_MIPS_RS4_BE': 10,
  'EM_ARM'        : 40, # Only ones that this script handles
  # ELF Version
  'EV_NONE'       : 0, # Error
  'EV_CURRENT'    : 1, # Good
  # ELF Identifier Index
  'EI_MAG0'       : 0,
  'EI_MAG1'       : 1,
  'EI_MAG2'       : 2,
  'EI_MAG3'       : 3,
  'EI_CLASS'      : 4, # File Class (32/64-bit)
  'EI_DATA'       : 5, # Data Encoding (Little/Big-endian)
  'EI_VERSION'    : 6, # Error/Current
  'EI_PAD'        : 7, # Start of padding.  Should be 0 for 8 bytes.
  'EI_NIDENT'     : 16, # Size of ELF Identifier
  # Magic Numbers.  Identify ELF files
  'ELFMAG0'       : 0x7f,
  'ELFMAG1'       : ord("E"),
  'ELFMAG2'       : ord("L"),
  'ELFMAG3'       : ord("F"),
  # 32-bit or 64-bit
  'ELFCLASSNONE'  : 0, # Error
  'ELFCLASS32'    : 1, # 32-bit
  'ELFCLASS64'    : 2, # 64-bit
  # Endian-ness
  'ELFDATANONE'   : 0, # Error
  'ELFDATA2LSB'   : 1, # Little-endian
  'ELFDATA2MSB'   : 2, # Big-endian
  # Elf Header Offset
  'ELFH_PHOFF_OFFSET'     : 28, # e_phOff
  'ELFH_SHOFF_OFFSET'     : 32, # e_shOff
  'ELFH_PHNUM_OFFSET'     : 44, # e_phNum
  'ELFH_SHNUM_OFFSET'     : 48, # e_phNum
  'ELFH_SHSTRINDEX_OFFSET'     : 50, # e_phNum
  # Section Header Offset
  'SECTIONH_OFFSET'     : 16, #sh_off
  'SECTIONH_SIZE'       : 20, #sh_size
  'SECTIONH_LINK'       : 24, #sh_link
  # Section Header Entry
  'SHN_UNDEF'     : 0, # should ignore
  'SHN_LORESERVE' : 0xff00, # lower reserved range
  'SHN_HIRESERVE' : 0xffff, # higher reserved range
  'SHN_LOPROC'    : 0xff00, # lower processor specific range
  'SHN_HIPROC'    : 0xff1f, # higher processor specific range
  'SHN_ABS'       : 0xfff1, # if symbol is relative to, use fixed address
  'SHN_COMMON'    : 0xfff2, # if symbol is relative to,treat as common symbol
  # Section Header Type
  'SHT_NULL'      : 0, # no section data
  'SHT_PROGBITS'  : 1, # holds data defined by program
  'SHT_SYMTAB'    : 2, # holds symbol table (same as SHT_DYNSYM ?)
  'SHT_STRTAB'    : 3, # holds string table
  'SHT_RELA'      : 4, # holds relocation entries with explicit addends
  'SHT_HASH'      : 5, # holds symbol hash table
  'SHT_DYNAMIC'   : 6, # holds dynamic linking information
  'SHT_NOTE'      : 7, # holds information on the file (labels?)
  'SHT_NOBITS'    : 8, # does not go into file. sh_offset not real
  'SHT_REL'       : 9, # holds relocation entries without explicit addends
  'SHT_SHLIB'     : 10, # reserved
  'SHT_DYNSYM'    : 11, # holds a symbol table (same as SHT_SYMTAB ?)
  'SHT_LOPROC'    : 0x70000000, # lower processor specific semantics range
  'SHT_HIPROC'    : 0x7fffffff, # higher processor specific semantics range
  'SHT_LOUSER'    : 0x80000000, # lower application reserve range
  'SHT_HIUSER'    : 0xffffffff, # higher application reserve range
  # Section Header Flags
  'SHF_WRITE'     : 0x1, # section is writable during execution
  'SHF_ALLOC'     : 0x2, # occupies memory during process execution
  'SHF_EXECINSTR' : 0x4, # executable machine instructions
  'SHF_MASKPROC'  : 0xf0000000 # processor-specific flags (just a mask)
}

header = {
# global ELF header stat.  Set in initializeWithElfHeader 
  'endian'            : 0, # 1 = little endian  :: 2 = big endian
  'headerSize'        : 0, # size of the ELF Header. (always 52?)
  'programHeaderOff'  : 0, # start of program header table
  'programHeaderSize' : 0, # size of each program header entry
  'programHeaderNum'  : 0, # number of program header entries in table
  'sectionHeaderOff'  : 0, # start of section header table
  'sectionHeaderSize' : 0, # size of each section header entry
  'sectionHeaderNum'  : 0, # number of section header entries in table
  'entry'             : 0, # entry address into executable image
  'flags'             : 0,
  'sectionNameTable'  : 0  # Section Header String Table Index
}

class ElfProcessStateClass:
  '''
  Used to hold most variables for ElfProcessState.
  '''


# ======================== end globals ========================

#=========================================================================
# Function : checkEIDENT 
#
# Arguments
#  header: (str) Bytes of ELF header
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks the e_ident field for global state that needs to be set.  May exit
#  if flags show an unsupported type.
#
# Return Values:
#  None
#=========================================================================
def checkEIDENT(header, e_ident):
  # formatting print to separate runQshrink20 info 
  if __debug__:
    print ""
  checkEidentELF(e_ident)
  checkEidentClass(e_ident)
  getEidentData(header, e_ident)
  checkEidentVersion(e_ident)


#=========================================================================
# Function : checkEidentELF 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks to see if the ELF file is actually an ELF file.  Will exit if we are
#  not working with an ELF file.
#
# Return Values:
#  None
#=========================================================================
def checkEidentELF(e_ident):
  if e_ident[0:4] != "".join(['\x7f', "ELF"]) :
    print "NOT AN ELF FILE!"
    exit(1)

#=========================================================================
# Function : checkEidentClass 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks if ELF is 32-bit.  Exits if it is NONE, UNKNOWN, or 64-bit.
#
# Return Values:
#  None
#=========================================================================
def checkEidentClass(e_ident):
  num = ord(e_ident[const['EI_CLASS']])
  if num != const['ELFCLASS32'] :
    print "NEEDS TO BE ELF-32"
    if    num == const['ELFCLASSNONE'] : print "CURRENT: INVALID"
    elif  num == const['ELFCLASS64']   : print "CURRENT: ELF-64"
    else: print "UNKNOWN EI_CLASS:", str(num)
    exit()

#=========================================================================
# Function : getEidentData 
#
# Arguments
#  header: (str) Bytes of the ELF header
#  e_ident: (str) Bytes of the e_ident field.
#
# Description
#    Sets the global variable for endianness.  Modifies behavior of the
#   getWord(str) family of functions.
#
# Return Values:
#  None
#=========================================================================
def getEidentData(header, e_ident):
  num = ord(e_ident[const['EI_DATA']])
  if 1 <= num <= 2:
    if __debug__:
      if    num == 1 : print "LITTLE ENDIAN"
      elif  num == 2 : print "BIG ENDIAN"
    header['endian'] = num
    return
  print "INVALID EI_DATA:", str(num)
  exit()

#=========================================================================
# Function : checkEidentVersion 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field.
#
# Description
#   Checks the ELF file version.  Exits on anything other than a Current one.
#
# Return Values:
#  None
#=========================================================================
def checkEidentVersion(e_ident):
  num = ord(e_ident[const['EI_VERSION']])
  if num == 1:
    return
  elif num == 0:  print "EIDENT VERSION: INVALID"
  else :          print "EIDENT VERSION: UNKNOWN", str(num)
  exit()


#=========================================================================
# Function : initializeArmElfProcessState 
#
# Arguments
#  elfFile, outputFile, hashFile,
#  sclFile, logFile, anomalyListFile
#
# Description
#  Initialize the global ElfProcessState
#
# Return Values:
#  None
#=========================================================================
def initializeArmElfProcessState(elfFile, logFile):
  global ElfProcessState
  ElfProcessState = ElfProcessStateClass()
  #used to hold variables shared between the processing methods

  ElfProcessState.sectionNameHeader = None
  # Header of the section with the names of the other sections

  ElfProcessState.sectionNameData = None
  # Data of the section with the names of the other sections.

  ElfProcessState.path = elfFile
  # ELF file path

  ElfProcessState.path = elfFile
  ElfProcessState.logPath = logFile
  
  ElfProcessState.sectionHeaderDict = {}

#=========================================================================
# Function : runPatch 
#
# Arguments
#  elfFile: (str) ELF file to read in
#  logFile: (str) Holds the log of changes.  name:index:address:bytes
#
# Description
#    Main entry function .Runs the ELF editor.  Gets its parameters from
#
# Return Values:
#  None
#=========================================================================
def runPatch(elfFile, logFile):

  # keep track of total time
  start = time.clock()

  initializeArmElfProcessState(elfFile, logFile )
  initializeWithElfHeader()

  #ElfProcessState.bytesDict['QSR_STRING'] = getSectionData('QSR_STRING')


#=========================================================================
# Function : symbolTableEntryGenerator 
#
# Arguments
#   bytes: (str) String of bytes which contains the Symbol Table
#   strTabData: (str) String of bytes which contains the String table
#
# Description
#   Creates a generator over the given bytes (should be the Symbol Table) that 
#   yields the next 16 bytes as a Symbol Table Entry.
#
# Return Values:
#  (generator:SymbolTableEntry) Iterable which has SymbolTableEntry
#          objects according to bytes
#=========================================================================
def symbolTableEntryGenerator(bytes, strTabData):
  assert len(bytes) % 16 == 0
  for i in xrange(0, len(bytes), 16) :
    yield SymbolTableEntry(bytes[i : i + 16], strTabData)

#=========================================================================
# Function : initializeWithElfHeader 
#
# Arguments
#
# Description
#   @warning: May suddenly exit script if bad header information is found 
#   Sets global data according to the ELF header.
#
# Return Values:
#=========================================================================
def initializeWithElfHeader():
  global ElfProcessState

  # set global header's entries according to ELF header
  try: 
    input = open(ElfProcessState.path, 'rb')
  except IOError:
    print "Error, Could not open input Elf File, ", ElfProcessState.path
    exit(1)
  else:
    elf_header = input.read(52)
    processHeader(elf_header)
    input.close()

  # get all section headers and map by index and name
  ElfProcessState.sectionNameHeader = getSectionHeader(header['sectionNameTable'])
  ElfProcessState.sectionNameData = getSectionData(header['sectionNameTable'])
  for i in xrange(0, header['sectionHeaderNum']) :
    ElfProcessState.sectionHeaderDict[i] = getSectionHeader(i)
    ElfProcessState.sectionHeaderDict[getSectionHeader(i).name] = ElfProcessState.sectionHeaderDict[i]
  
  overlaySize = 0
  symbolTableData = getSectionData('.symtab')
  stringTableData = getSectionData('.strtab')
  for entry in symbolTableEntryGenerator(symbolTableData, stringTableData):
    if (entry.name == "__bss_overlay_size"):
      overlaySize = entry.value
      print '__bss_overlay_size = %d' % overlaySize
  
  if overlaySize == 0 :
    return
    
  for i in xrange(0, header['sectionHeaderNum']) :
    if ElfProcessState.sectionHeaderDict[i].name.find('.bss.overlay.') != -1 :
      OverlayHeader = ElfProcessState.sectionHeaderDict[i]
      OverlayOffset = OverlayHeader.data['sh_offset']
      print 'found sec %s, at ELF offset %d' % (OverlayHeader.name, OverlayOffset)
      break

  input.close()
  for i in range(0, int(header['programHeaderNum'])):
    ph = ProgramHeader(i)
    if ph.data['p_offset'] == OverlayOffset :
      print 'found PH %d, at ELF offset %d' % (i, OverlayOffset)
      print 'output file %s' % (ElfProcessState.path)
      output = open(ElfProcessState.path, 'r+b')
      elfoffset = header['programHeaderOff']
      elfoffset += header['programHeaderSize'] * i
      elfoffset += (const['Elf32_Off'] + 2 * const['Elf32_Word'] +
                      2 * const['Elf32_Addr'])
      output.seek(elfoffset)
      output.write(convertToElf('Elf32_Word', overlaySize))
      output.flush()
      output.close()
      return
   
#=========================================================================
# Function : getString 
#
# Arguments
#  bytes: (str) Bytes that contain a null character.
#  begin: (int) Where to start in the given string of bytes.
#  end: (int) Where to end, non-inclusive, in the given string of bytes
#
# Description
#   Gets the first null-terminated string from bytes[begin : end].  If end is
#   not given, then it will search the bytes[begin : ].  If no null is found,
#   then it will return bytes[begin : ]
#
# Return Values:
#   All characters from begin to either end or the first null char.
#=========================================================================
def getString(bytes, begin=0, end= -1):
  index = begin
  if end == -1 : end = len(bytes)
  while (index < end and bytes[index] != '\x00'):
#         and bytes[index] != '\x01'):
    index += 1
  return bytes[begin:index]
  
#=========================================================================
# Function : getStringGenerator 
#
# Arguments
#   bytes: (str) Bytes to get a string from
#   begin: (int) Start offset into bytes
#   end: (int) End offset into bytes.  Non-inclusive.
#
# Description
#   Deprecated.  Requires only a single-null byte between strings and a
#   contiguous string section.  Only yields strings in order.  Not used since
#   strings are not guaranteed to be in order.
#   Returns a generator that yields the next string in bytes. 
#
# Return Values:
#   (generator:str) Iterable which has strings from bytes
#=========================================================================
def getStringGenerator(bytes, begin=0, end= -1):
  index = begin
  if end == -1 : end = len(bytes)
  while index < end :
    while index < end and bytes[index] != '\x00' :
      index += 1
    yield bytes[begin:index]
    index += 1
    begin = index

#=========================================================================
# Function : getSectionData 
#
# Arguments
#   section: ([str|int]) Index of the section whose data you want
#
# Description
#   Reads the input ELF file and grabs the section's data.  Does not cache to 
#   avoid a Memory Error when part of a build.
#
# Return Values:
#   section data
#=========================================================================
def getSectionData(section):
  secheader = getSectionHeader(section)
  offset = secheader.data['sh_offset']
  size = secheader.data['sh_size']
  try:
    input = open(ElfProcessState.path, 'rb')
  except IOError:
    print "Error: Could not open input ELF file, ",ElfProcessState.path
    exit(1)
  else:
    input.seek(offset)
    ret = input.read(size)
    input.close()
  return ret


#=========================================================================
# Function : processHeader 
#
# Arguments
#   elf_header: (str) Bytes of the header
#
# Description
#   Goes through the ELF header and sets entries in the global header dict
#   accordingly.
#
# Return Values:
#=========================================================================
def processHeader(elf_header):

  global header
  header_data = header # local name is faster
  e_ident = elf_header[0:16]
  checkEIDENT(header_data, e_ident)
  if __debug__:
    print ""
  checkElfHeaderType    (convertFromElf('Elf32_Half', elf_header[16:18]))
  checkElfHeaderMachine (convertFromElf('Elf32_Half', elf_header[18:20]))
  checkElfHeaderVersion (convertFromElf('Elf32_Word', elf_header[20:24]))
  header_data["entry"] = convertFromElf('Elf32_Addr', elf_header[24:28])
  header_data["programHeaderOff"] = convertFromElf(
                                       'Elf32_Off' , elf_header[28:32])
  header_data["sectionHeaderOff"] = convertFromElf(
                                       'Elf32_Off' , elf_header[32:36])
  getElfHeaderFlags(header_data, convertFromElf(
                                       'Elf32_Word', elf_header[36:40]))
  header_data["headerSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[40:42])
  header_data["programHeaderSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[42:44])
  header_data["programHeaderNum"] = convertFromElf(
                                       'Elf32_Half', elf_header[44:46])
  header_data["sectionHeaderSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[46:48])
  header_data["sectionHeaderNum"] = convertFromElf(
                                       'Elf32_Half', elf_header[48:50])
  header_data["sectionNameTable"] = convertFromElf(
                                       'Elf32_Half', elf_header[50:52])

#=========================================================================
# Function : getElfHeaderFlags 
#
# Arguments
#   header: (str) Global header dictionary
#   e_flags: (str) Bytes of the e_flags field
#
# Description
#   Print out known flags in the ELF file.  No real reason for this to exist
#   as of yet.
#
# Return Values:
#=========================================================================
def getElfHeaderFlags(header, e_flags):

  header['flags'] = e_flags
  #if __debug__:
    #print "Flags:", header['flags'], hex(header['flags'])
    #if header['flags'] & 0x2: print "Flag: EF_ARM_HASENTRY"
    #if header['flags'] & 0x4: print "Flag: EF_ARM_SYMSARESORTED"
    #if header['flags'] & 0x8: print "Flag: EF_ARM_DYNSYMSUSESEGIDX"
    #if header['flags'] & 0x10: print "Flag: EF_ARM_MAPSYMSFIRST"
    #if header['flags'] & 0xff000000 == 0x02000000 :
      #print "Flag: EF_ARM_EABI -> Version 2"
    #elif header['flags'] & 0xff000000 == 0 :
      #print "Flag: EABI Unknown Conformance"
    #print "Flags only relevant to ARM/Thumb Architecture"
    #print ""

#=========================================================================
# Function : checkElfHeaderVersion 
#
# Arguments
#   e_version: number read from ELF header
#
# Description
#   checks version against constants
#
# Return Values:
#=========================================================================
def checkElfHeaderVersion(e_version):
  num = e_version
  if    num == const['EV_NONE']    : print "Invalid Version"
  elif  num == const['EV_CURRENT'] :
    pass
#    print "Current Version"
  else:
    print "Unknown Version: ", num

#=========================================================================
# Function : checkElfHeaderVersion 
#
# Arguments
#   e_machine: number read from ELF header
#
# Description
#   Checks if machine type is compatable with this script.
#
# Return Values:
#=========================================================================
def checkElfHeaderMachine(e_machine):
  num = e_machine
  if    num == const['EM_NONE'] :
    pass
#    print "No Machine", "Cannot Check Flags"
  elif  num == const['EM_M32'] :
    pass
#    print "AT&T WE 32100", "Cannot Check Flags"
  elif  num == const['EM_SPARC'] :
    pass
#    print "SPARC", "Cannot Check Flags"
  elif  num == const['EM_386'] :
    pass
#    print "Intel Architecture", "Cannot Check Flags"
  elif  num == const['EM_68K'] :
    pass
#    print "Motorola 68000", "Cannot Check Flags"
  elif  num == const['EM_88K'] :
    pass
#    print "Motorola 88000", "Cannot Check Flags"
  elif  num == const['EM_860'] :
    pass
#    print "Intel 80860", "Cannot Check Flags"
  elif  num == const['EM_MIPS'] :
    pass
#    print "MIPS RS3000 Big-Endian", "Cannot Check Flags"
  elif  num == const['EM_MIPS_RS4_BE'] :
    pass
#    print "MIPS RS4000 Big-Endian", "Cannot Check Flags"
  elif  num == const['EM_ARM'] :
    pass
#    print "ARM/Thumb Architecture"
  else:
    print "Machine: ", num

#=========================================================================
# Function : checkElfHeaderType 
#
# Arguments
#   e_type: number read from ELF header
#
# Description
#   Checks if machine type is compatable with this script.
#
# Return Values:
#=========================================================================
def checkElfHeaderType(e_type):
  num = e_type
  if num != const['ET_EXEC'] :
    print "ELF Type Must Be Executable to Modify"
    exit()
#  if    num == const['ET_NONE']  : print "No File Type"
#  elif  num == const['ET_REL']   : print "Re-Locatable File"
#  elif  num == const['ET_EXEC']  : print "Executable File"
#  elif  num == const['ET_DYN']   : print "Shared Object File"
#  elif  num == const['ET_CORE']  : print "Core File"
#  elif  const['ET_LOPROC'] <= num <= const['ET_HIPROC']: 
#    print "Processor Specific:", str(num)
#  else:
#    print "Unknown Type:", str(num)
#    exit()

#=========================================================================
# Function : convertFromElf 
#
# Arguments
#   typeOrLen: str|int - either name of Elf32 type or length in bytes
#   byteString: to be converted to an unsigned number
#   endian: should endianness be adjusted from ELF file? 
#
# Description
#   This accounts for endianness.
#
# Return Values:
#   a number based on the given format
#=========================================================================
def convertFromElf(typeOrLen, byteString, endian=True):
  endianness = ">" #big by default, like english, MSB first
  if endian == True:
    if header['endian'] == 1:
      endianness = "<" #little
    else:
      endianness = ">" #big

  size = 0
  if typeOrLen in const:
    size = const[typeOrLen]
  else:
    size = typeOrLen
  assert size == len(byteString)

  if size == 4:
    format = "L"
  elif size == 2:
    format = "H"
  elif size == 1:
    format = "B"
  elif size == 8:
    format = "Q"
  else:
    raise Exception("bad convertFromElf input: " +
                    str(typeOrLen) + " " + byteString)

  return struct.unpack(endianness + format, byteString)[0]

#=========================================================================
# Function : convertToElf 
#
# Arguments
#   typeOrLen: str|int - either name of Elf32 type or length in bytes
#   number: unsigned number to be converted
#   endian: should endianness be adjusted to match ELF file? 
#
# Description
#   This accounts for endianness.
#
# Return Values:
#   a byte string in the correct format
#=========================================================================
def convertToElf(typeOrLen, number, endian=True):

  endianness = ">" #big by default, like english, MSB first
  if endian == True:
    if header['endian'] == 1:
      endianness = "<" #little
    else:
      endianness = ">" #big

  size = 0
  if typeOrLen in const:
    size = const[typeOrLen]
  else:
    size = typeOrLen

  if size == 4:
    format = "L"
  elif size == 2:
    format = "H"
  elif size == 1:
    format = "B"
  elif size == 8:
    format = "Q"
  else:
    raise Exception("bad convertToElf input: " +
                    str(typeOrLen) + " " + str(number))

  return struct.pack(endianness + format, number)

#=========================================================================
# Function : getSectionHeader 
#
# Arguments
#   args: ([str|int]) If empty, then return an empty SectionHeaderClass.
#                           Otherwise, if first arg is a str, return a 
#                           SectionHeaderClass by name.  If first arg is an
#                           int, return a SectionHeaderClass by index.
#                           Otherwise, return an empty SectionHeaderClass.
#
# Description
#   Wrapper around SectionHeaderClass's constructor to manage the
#   ElfProcessState.sectionHeaderDict caches.
# Return Values:
#   a byte string in the correct format
#=========================================================================
def getSectionHeader(*args):

  global ElfProcessState
  if len(args) != 0:
    if args[0] in ElfProcessState.sectionHeaderDict :
      return ElfProcessState.sectionHeaderDict[args[0]]
    else:
      return SectionHeaderClass(args[0])
  return SectionHeaderClass()

#=========================================================================
# Class : SectionHeaderClass 
#
# Arguments
#
# Description
#   Initializes data dictionary and grabs header data if index or name
#   is given to the constructor.
#
# Return Values:
#   Section Header class dictionary
#=========================================================================
class SectionHeaderClass:
  def __init__(self, *args):
    '''
    @param *args: Eats up optional parameters.  If they are given, there must
                  be two of them.  The first one is used as the path and the 
                  second the index in a call to readIndex right after the 
                  creation of the data dictionary.
                  
    Initializes data dictionary and grabs header data if index or name
    is given to the constructor.
    '''
    self.name = "NO_NAME"
    self.data = {
      'sh_name'      : 0, # index in section header string table with name
      'sh_type'      : 0, # section's contents and semantics type
      'sh_flags'     : 0,
      'sh_addr'      : 0, # address of section's first byte in image
      'sh_offset'    : 0, # offset from beginning of file to section data
      'sh_size'      : 0, # size of section's data
      'sh_link'      : 0, # section header table index link. depends on type
      'sh_info'      : 0, # depends on type
      'sh_addralign' : 0, # alignment of the entire section
      'sh_entsize'   : 0  # size of entries. only for certain types  
    }
    self.index = 0
    if len(args) != 0 :
      if (type(args[0]) is int
        and 0 <= args[0] <= header['sectionHeaderNum']):
        self.readIndex(args[0])
      elif type(args[0]) is str :
        self.getByName(args[0])
      else: assert False


  def getByName(self, name):
    for i in range(header['sectionHeaderNum'] - 1, 0, -1) :
      self.readIndex(i)
      if self.name == name : return
    print "SectionHeaderClass : getByName(", name, "): Bad Name"
    assert False

  def readIndex(self, index):
    '''
    @param path: The path to the file to read through
    @param index: The index of the section header to read.  File offset is 
                  calculated based off of this.  0 is the null header.
    Gets header data by index.  Also assigns index to object.  Not all data
    is needed and so is not read to save time.
    '''
    global ElfProcessState
    assert 0 <= index <= header['sectionHeaderNum']
    offset = header['sectionHeaderOff'] + index * header['sectionHeaderSize']
    try :
      input = open(ElfProcessState.path, 'rb')
    except IOError:
      print "Error: Could not open Input, elf file", ElfProcessState.path
      exit(1)
    else:
      input.seek(offset)
      bytes = input.read(header['sectionHeaderSize'])
      self.data['sh_name'] = convertFromElf('Elf32_Word', bytes[0:4])
#      self.data['sh_type'] = convertFromElf( 'Elf32_Word', bytes[4:8] )
#      self.data['sh_flags'] = convertFromElf( 'Elf32_Word', bytes[8:12] )
      self.data['sh_addr'] = convertFromElf('Elf32_Addr', bytes[12:16])
      self.data['sh_offset'] = convertFromElf('Elf32_Off', bytes[16:20])
      self.data['sh_size'] = convertFromElf('Elf32_Word', bytes[20:24])
      self.data['sh_link'] = convertFromElf( 'Elf32_Word', bytes[24:28] )
#      self.data['sh_info'] = convertFromElf( 'Elf32_Word', bytes[28:32] )
      self.data['sh_addralign'] = convertFromElf('Elf32_Word', bytes[32:36])
#      self.data['sh_entsize'] = getFromElf( 'Elf32_Word', bytes[36:40] )
      input.close()
    self.index = index
    # need this check in case this header is used to find sectionNameData
    if ElfProcessState.sectionNameData is not None :
      self.name = getString(ElfProcessState.sectionNameData, self.data['sh_name'])

  def printData(self):
    print "SECTION HEADER:", self.index, self.name
    flags = self.data['sh_flags']
    if flags & 0x1 != 0 :         print "FLAG: SHF_WRITE"
    if flags & 0x2 != 0 :         print "FLAG: SHF_ALLOC"
    if flags & 0x4 != 0 :         print "FLAG: SHF_EXECINSTR"
    if flags & 0xf0000000 != 0 :  print "FLAG: SHF_MASKPROC"

    type = self.data['sh_type']
    if   type == const['SHT_NULL']:     print "TYPE: SHT_NULL"
    elif type == const['SHT_PROGBITS']: print "TYPE: SHT_PROGBITS"
    elif type == const['SHT_SYMTAB']:   print "TYPE: SHT_SYMTAB"
    elif type == const['SHT_STRTAB']:   print "TYPE: SHT_STRTAB"
    elif type == const['SHT_RELA']:     print "TYPE: SHT_RELA"
    elif type == const['SHT_HASH']:     print "TYPE: SHT_HASH"
    elif type == const['SHT_DYNAMIC']:  print "TYPE: SHT_DYNAMIC"
    elif type == const['SHT_NOTE']:     print "TYPE: SHT_NOTE"
    elif type == const['SHT_NOBITS']:   print "TYPE: SHT_NOBITS"
    elif type == const['SHT_REL']:      print "TYPE: SHT_REL"
    elif type == const['SHT_SHLIB']:    print "TYPE: SHT_SHLIB"
    elif type == const['SHT_DYNSYM']:   print "TYPE: SHT_DYNSYM"
    elif const['SHT_LOPROC'] <= type <= const['SHT_HIPROC']:
      print "TYPE: PROCESSOR SPECIFIC"
    elif const['SHT_LOUSER'] <= type <= const['SHT_HIUSER']:
      print "TYPE: RESERVED FOR APPLICATIONS"

    for key in self.data :
      if key != "sh_flags" and key != "sh_type" :
        print key, ":", self.data[key], hex(self.data[key])
    print ""

#=========================================================================
# Class : ProgramHeader 
#
# Arguments
#
# Description: Program Header Dictionary
#
# Return Values:
#=========================================================================
class ProgramHeader:
  def __init__(self, *args):
    self.data = {
      'p_type'    : 0,
      'p_offset'  : 0,
      'p_vaddr'   : 0,
      'p_paddr'   : 0,
      'p_filesz'  : 0,
      'p_memsz'   : 0,
      'p_flags'   : 0,
      'p_align'   : 0,
    }
    self.index = 0
    if len(args) != 0:
      self.readIndex(args[0])

  def readIndex(self, index):
    global ElfProcessState
    assert (index < header['programHeaderNum'])

    self.offset = header['programHeaderOff'] + index * header['programHeaderSize']
    try:
      input = open(ElfProcessState.path, 'rb')
    except IOError:
      print "Error: Could not open Input, elf file", ElfProcessState.path
      exit(1)
    else:
      input.seek(self.offset)
      bytes = input.read(header['programHeaderSize'])
      self.data['p_type'] = convertFromElf('Elf32_Word', bytes[0:4])
      self.data['p_offset'] = convertFromElf('Elf32_Off', bytes[4:8])
      self.data['p_vaddr'] = convertFromElf('Elf32_Addr', bytes[8:12])
      self.data['p_paddr'] = convertFromElf('Elf32_Addr', bytes[12:16])
      self.data['p_filesz'] = convertFromElf('Elf32_Word', bytes[16:20])
      self.data['p_memsz'] = convertFromElf('Elf32_Word', bytes[20:24])
      self.data['p_flags'] = convertFromElf('Elf32_Word', bytes[24:28])
      self.data['p_align'] = convertFromElf('Elf32_Word', bytes[28:32])
      input.close()
    self.index = index

  def printData(self):
    print "Program Header:", self.index
    for key in self.data :
      print key, ":", self.data[key], hex(self.data[key])
    print ""

#=========================================================================
# Class : SymbolTableEntry 
#
# Arguments
#    bytes: Entry in bytes
#   stringTable : To get the symbol names
#
# Description: Symbol Table Entry 
#
# Return Values:
#=========================================================================
class SymbolTableEntry:
  def __init__(self, bytes, stringTable):
    self.data = {
      'st_name'   : 0,
#      'st_value'  : 0,
#      'st_size'   : 0,
#      'st_info'   : 0,
      'st_other'  : 0,
#      'st_shndx'  : 0
    }
#    self.data['st_name'] = convertFromElf( 'Elf32_Word', bytes[0:4] )
#    self.data['st_value'] = convertFromElf( 'Elf32_Addr', bytes[4:8] )
    self.value = convertFromElf('Elf32_Addr', bytes[4:8])
#    self.data['st_size'] = convertFromElf( 'Elf32_Word', bytes[8:12] )
    self.size = convertFromElf('Elf32_Word', bytes[8:12])
#    self.data['st_info'] = convertFromElf( 1, bytes[12] ,False)
#    self.st_info = convertFromElf( 1, bytes[12] ,False)
#    self.data['st_other'] = convertFromElf( 1, bytes[13] ,False)
#    self.data['st_shndx'] = convertFromElf( 'Elf32_Half', bytes[14:16] )
    self.sectionIndex = convertFromElf('Elf32_Half', bytes[14:16])
    self.name = getString(stringTable, convertFromElf('Elf32_Word', bytes[0:4]))

  def printData(self):
    print "Symbol Table Entry:", self.name
    for key in self.data :
      print key, ":", self.data[key], hex(self.data[key])
    print ""


if __name__ == '__main__':
  runPatch(sys.argv[1], 'log.txt')
  
  
