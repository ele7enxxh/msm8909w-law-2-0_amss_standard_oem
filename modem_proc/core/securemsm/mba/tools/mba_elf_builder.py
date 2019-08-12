#===============================================================================
#
# MBN TOOLS
#
# GENERAL DESCRIPTION
#    Contains all MBN Utilities for image generation 
#
# Copyright 2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/tools/mba_elf_builder.py#1 $
#  $DateTime: 2016/12/13 07:59:23 $
#  $Author: mplcsds1 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================

import stat
import csv 
import itertools
import struct
import os
import shutil
import hashlib
import subprocess
import string
import re
import sys
import getopt
from optparse import OptionParser


#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------
PAD_BYTE                = 0               # Padding byte 0s

# ELF Definitions
ELF_HDR_SIZE              = 52          
ELF_PHDR_SIZE             = 32          
ELFINFO_MAG0_INDEX        = 0
ELFINFO_MAG1_INDEX        = 1
ELFINFO_MAG2_INDEX        = 2
ELFINFO_MAG3_INDEX        = 3
ELFINFO_MAG0              = '\x7F\x45\x4c\x46\x01\x01\x01'
ELFINFO_CLASS_INDEX       = 4
ELFINFO_CLASS_32          = '\x01'
ELFINFO_VERSION_INDEX     = 6
ELFINFO_VERSION_CURRENT   = '\x01'
MAX_PHDRS_COUNT           = 100
ELF_BLOCK_ALIGN           = 0x1000
LOAD_TYPE                 = 0x1
PHDR_TYPE                 = 0x6
PROGRAM_HEADER_LEN        = 8

#QDSP6OBJCOPY = "${QDSP6BIN}" + "/qdsp6-objcopy${EXE_EXT}"
QDSP6OBJCOPY = "${OBJCOPY}"
#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('mba_elf_builder')

def generate(env):
   mba_elf_generate(env)
   hexagon_bin_generate(env)
   
#===============================================================================
# PBN build rules
#===============================================================================
def mba_elf_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   mba_elf_act = env.GetBuilderAction(mba_elf_builder)
   mba_elf_bld = env.Builder(action = mba_elf_act,
                         suffix = '.elf')

   env.Append(BUILDERS = {'MbaElfBuilder' : mba_elf_bld})
   
#-------------------------------------------------------------------------------
# def builder for qcoreapp_booting.pbn
#
def mba_elf_builder(target, source, env):
   # init and fix variebles
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))
   #makes the elf header in the target file
   image_header(target_str, source_str, int(env.subst("${MBA_IMAGE_ENTRY}"), 16))

   fp = OPEN(target_str,'ab')  
   pad_file(fp, 4012, PAD_BYTE)
   fp.close()
   
   #Concatenate the elf header file with the source MBN file
   concat_files(target_str, source_str)   
   
   return None

#----------------------------------------------------------------------------
# Pad a file with specific number of bytes
# Note: Assumes the fp is seeked to the correct location of padding 
#----------------------------------------------------------------------------
def pad_file(fp, num_bytes, value):
    
   if num_bytes < 0:
      raise RuntimeError, "Number of bytes to pad must be greater than zero"
  
   while num_bytes > 0:
      fp.write('%c' % value)
      num_bytes -= 1

   return 
   
def concat_files (target, sources):
   if type(sources) is not list:
      sources = [sources]

   target_file = open(target,'ab')

   for fname in sources:
      file = open(fname,'rb')
      while True:
         bin_data = file.read(65536)
         if not bin_data:
            break
         target_file.write(bin_data)
      file.close()
   target_file.close()

#----------------------------------------------------------------------------
# Header Class Notes:
# In order to properly read and write the header structures as binary data,
# the Python Struct library is used to align and package up the header objects
# All Struct objects are initialized by a special string with the following
# notation. These structure objects are then used to decode binary data in order
# to fill out the appropriate class in Python, or they are used to package up
# the Python class so that we may write the binary data out. 
#----------------------------------------------------------------------------
"""
      Format | C Type         | Python Type | Standard Size
      -----------------------------------------------------
    1) 'X's  | char *         | string      | 'X' bytes
    2) H     | unsigned short | integer     | 2 bytes
    3) I     | unsigned int   | integer     | 4 bytes
     
"""

#----------------------------------------------------------------------------
# ELF Header Class 
#----------------------------------------------------------------------------
class Elf32_Ehdr:
   # Structure object to align and package the ELF Header
     
   def __init__(self, entry_address, init_val):
      self.e_ident = ELFINFO_MAG0
      self.e_type = 0x2
      self.e_machine = 0xA4
      self.e_version = 0x1
      self.e_entry = entry_address
      self.e_phoff = 52
      self.e_shoff = 0
      self.e_flags = 3
      self.e_ehsize = 52
      self.e_phentsize = 32
      self.e_phnum = 1
      self.e_shentsize = 0
      self.e_shnum = 0
      self.e_shstrndx = 0
      
   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def writePackedData(self, target):
      values = [self.e_ident,
                self.e_type,
                self.e_machine,
                self.e_version,
                self.e_entry,
                self.e_phoff,
                self.e_shoff,
                self.e_flags,
                self.e_ehsize,
                self.e_phentsize,
                self.e_phnum,
                self.e_shentsize,
                self.e_shnum,
                self.e_shstrndx
               ]

      # Write 10 entries(40B) or 20 entries(80B) of boot header       
      s = struct.Struct('16sHHIIIIIHHHHHH') 
      packed_data = s.pack(*values)
      
      fp = OPEN(target,'wb')
      fp.write(packed_data)
      fp.close()

      return s.size



#----------------------------------------------------------------------------
# ELF Program Header Class 
#----------------------------------------------------------------------------
class Elf32_Phdr:

   # Structure object to align and package the ELF Program Header 

   
   def __init__(self, entry_address, load_size, init_val): 
      self.p_type         = 1
      self.p_offset       = 0x001000
      self.p_vaddr        = entry_address
      self.p_paddr        = entry_address
      self.p_filesz       = load_size
      self.p_memsz        = load_size
      self.p_flags        = 7
      self.p_align        = 0
   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getLength(self):
      return PROGRAM_HEADER_LEN		 
	  
   def writePackedData(self, target):
      values = [self.p_type,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_flags,
                self.p_align
               ]
      s = struct.Struct('I' * self.getLength()) 
      packed_data = s.pack(*values)
      
      fp = OPEN(target,'ab')
      fp.write(packed_data)
      fp.close()
      return s.size


#----------------------------------------------------------------------------
# image_header 
#----------------------------------------------------------------------------
def image_header(output_file_name, mbn_file_name, entry_address):
   #import pdb;pdb.set_trace()
   st = os.stat(mbn_file_name)
   
   mba_elf_header = Elf32_Ehdr(entry_address, init_val = int('0x0',16))
   mba_prg_header = Elf32_Phdr(entry_address, st.st_size, init_val = int('0x0',16))
   
   # Package up the header and write to output file
   mba_elf_header.writePackedData(target = output_file_name)
   mba_prg_header.writePackedData(target = output_file_name)

   return 0 


#----------------------------------------------------------------------------
# Helper functions to open a file and return a valid file object 
#----------------------------------------------------------------------------
def OPEN(file_name, mode):
    try:
       fp = open(file_name, mode)
    except IOError:
       raise RuntimeError, "The file could not be opened: " + file_name
    
    # File open has succeeded with the given mode, return the file object
    return fp



#===============================================================================
# PBN build rules
#===============================================================================
def hexagon_bin_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   hexagon_bin_act = env.GetBuilderAction(hexagon_bin_builder)
   hexagon_bin_bld = env.Builder(
						 action = hexagon_bin_act,
					     emitter = hexagon_bin_emitter,
                         suffix = '.bin')

   env.Append(BUILDERS = {'HexagonBinBuilder' : hexagon_bin_bld})
   

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def hexagon_bin_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   env.Depends(target, env.subst(QDSP6OBJCOPY))
   return (target, source)

#-------------------------------------------------------------------------------
# def builder for qcoreapp_booting.pbn
#
def hexagon_bin_builder(target, source, env):
   # init and fix variebles
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))
   target_tmp = target_str+'.tmp'

   env.Replace(QDSP6OBJCOPY = QDSP6OBJCOPY)
   env.PrintInfo("QDSP6OBJCOPY       = ${QDSP6OBJCOPY}")
   
   env.Depends(target_str, source_str)
     
   cmds = ["$QDSP6OBJCOPY -S -O binary " + source_str + " " +target_str]

   data, err, rv = env.ExecCmds(cmds, target=target_str)
     
   return None

#------------------------------------------------------------------------------
# Parse command line arguments
#------------------------------------------------------------------------------
def parse_args():
   usage = "usage: %prog [options]"
   version = "%prog 1.0"
   
   parser = OptionParser(usage=usage, version=version)
   parser.add_option("-f", "--in_file", dest="input_filename",
                  help="Read preprocess data from FILE", metavar="FILE")
   parser.add_option("-o", "--outfile", dest="output_filename",
                  help="Write output to FILE", metavar="FILE")
   parser.add_option("-i", "--image_entry", dest="mba_image_entry",
                  help="Image entry address", metavar="FILE")       
   (options, args) = parser.parse_args()
   
   if options.input_filename is None:
      parser.error("Usage: -f input_file -o output_file -i image_entry")
      sys.exit(2)
   if options.output_filename is None:
      parser.error("Usage: -f input_file -o output_file -i image_entry")
      sys.exit(2)               
   if options.mba_image_entry is None:
      parser.error("Usage: -f input_file -o output_file -i image_entry")
      sys.exit(2)                    
   return (options, args)
   
#------------------------------------------------------------------------------
# main
#------------------------------------------------------------------------------
if __name__ == "__main__":  
   # get args from cmd line
   (options, args) = parse_args()
   
   # init and fix variebles
   source_str = options.input_filename
   target_str = options.output_filename
   mba_image_entry = options.mba_image_entry
   
   #makes the elf header in the target file
   image_header(target_str, source_str, int(mba_image_entry,16))

   fp = OPEN(target_str,'ab')  
   pad_file(fp, 4012, PAD_BYTE)
   fp.close()
   
   #Concatenate the elf header file with the source MBN file
   concat_files(target_str, source_str)  
   
