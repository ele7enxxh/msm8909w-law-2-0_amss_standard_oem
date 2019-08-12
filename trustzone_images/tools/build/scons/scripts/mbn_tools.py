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
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/tools/build/scons/scripts/mbn_tools.py#1 $
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 11/12/10   aus     Add EFS images into the table
# 11/11/10   dxiang  Add SB2 images into the table for backwards compatibility 
# 11/11/10   dxiang  No longer generate any PHDR program header type in ELF images 
# 11/08/10   dxiang  Fix in modify_elf_flags() 
# 11/03/10   dxiang  Remove hardcoded values when searching for program headers.
#                    Support custom code size in image_header. 
# 10/21/10   dxiang  Bug fixes for pboot_gen_elf() 
# 10/20/10   dxiang  Clean up and add comments for ELF program header flags
# 10/11/10   dxiang  Port modify_elf_flags() and updates to pboot_add_hash() 
# 10/04/10   dxiang  Add additional fields to the SBL header type 
# 09/28/10   dxiang  Fix to pboot_add_hash for custom address parameter passing 
# 09/09/10   dxiang  Updates to populate_dictionary() to parse C header file
#                    more thoroughly 
# 08/11/10   dxiang  Initial Version
#
#===============================================================================

import stat
import csv 
import itertools
import struct
import os
import shutil
import hashlib

#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------
PAD_BYTE_1                = 255             # Padding byte 1s
PAD_BYTE_0                = 0               # Padding byte 0s
SHA256_SIGNATURE_SIZE     = 256             # Support SHA256
CERT_CHAIN_MAXSIZE        = 6 * 1024        # Certificate Chain Max Size 6K
MAX_NUM_ROOT_CERTS        = 4               # Maximum number of OEM root certificates 
MI_BOOT_IMG_HDR_SIZE      = 40              # sizeof(mi_boot_image_header_type)
MI_BOOT_SBL_HDR_SIZE      = 80              # sizeof(sbl_header)
BOOT_HEADER_LENGTH        = 20              # Boot Header Number of Elements
SBL_HEADER_LENGTH         = 20              # SBL Header Number of Elements
FLASH_PARTI_VERSION       = 3               # Flash Partition Version Number
FLASH_CODE_WORD           = 0x844BDCD1      # Flash codeword

# ELF Definitions
ELF_HDR_SIZE              = 52          
ELF_PHDR_SIZE             = 32          
ELFINFO_MAG0_INDEX        = 0
ELFINFO_MAG1_INDEX        = 1
ELFINFO_MAG2_INDEX        = 2
ELFINFO_MAG3_INDEX        = 3
ELFINFO_MAG0              = '\x7f'
ELFINFO_MAG1              = 'E'
ELFINFO_MAG2              = 'L'
ELFINFO_MAG3              = 'F'
ELFINFO_CLASS_INDEX       = 4
ELFINFO_CLASS_32          = '\x01'
ELFINFO_VERSION_INDEX     = 6
ELFINFO_VERSION_CURRENT   = '\x01'
MAX_PHDRS_COUNT           = 100
ELF_BLOCK_ALIGN           = 0x1000
LOAD_TYPE                 = 0x1
PHDR_TYPE                 = 0x6

# Magic numbers filled in for boot headers 
UNIFIED_BOOT_COOKIE_MAGIC_NUMBER      = 0x33836685  
MAGIC_NUM                             = 0x73D71034 
AUTODETECT_PAGE_SIZE_MAGIC_NUM        = 0x7D0B435A

# Helper defines to help parse ELF program headers
MI_PROG_BOOT_DIGEST_SIZE              = 20
MI_PBT_FLAG_SEGMENT_TYPE_MASK         = 0x07000000
MI_PBT_FLAG_SEGMENT_TYPE_SHIFT        = 0x18
MI_PBT_FLAG_PAGE_MODE_MASK            = 0x00100000
MI_PBT_FLAG_PAGE_MODE_SHIFT           = 0x14
MI_PBT_FLAG_ACCESS_TYPE_MASK          = 0x00E00000
MI_PBT_FLAG_ACCESS_TYPE_SHIFT         = 0x15
MI_PBT_SWAPPED_SEGMENT                = 0x5
MI_PBT_HASH_SEGMENT                   = 0x2
MI_PBT_NON_PAGED_SEGMENT              = 0x0
MI_PBT_PAGED_SEGMENT                  = 0x1
MI_PBT_NOTUSED_SEGMENT                = 0x3
MI_PBT_SHARED_SEGMENT                 = 0x4

"""
The eight bits between 20 and 27 in the p_flags field in ELF program headers
is not used by the standard ELF format. We use this byte to hold OS and processor
specific fields as recommended by ARM. 

The bits in this byte are defined as follows:

                   Pool Indx    Segment type     Access type    Page/non page
  bits in p_flags /-----27-----/----26-24-------/---- 23-21----/------20-------/

After parsing segment description strings in the SCL file, the appropriate segment
flag values are chosen from the follow definitions. The mask defined below is then 
used to update the existing p_flags field in the program headers with the updated
values. 
"""
MI_PBT_FLAGS_MASK = 0x0FF00000

# ELF Segment Flag Definitions 
MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT  = 0x01200000
MI_PBT_ELF_AMSS_PAGED_RO_SEGMENT  = 0x01300000 
MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX0 = 0x06400000
MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX0 = 0x05300000 
MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX1 = 0x0E400000
MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX1 = 0x0D300000 
MI_PBT_ELF_AMSS_NON_PAGED_ZI_SEGMENT = 0x01400000
MI_PBT_ELF_AMSS_PAGED_ZI_SEGMENT = 0x01500000
MI_PBT_ELF_AMSS_NON_PAGED_RW_SEGMENT = 0x01000000
MI_PBT_ELF_AMSS_PAGED_RW_SEGMENT = 0x01100000
MI_PBT_ELF_AMSS_NON_PAGED_NOTUSED_SEGMENT = 0x01600000
MI_PBT_ELF_AMSS_PAGED_NOTUSED_SEGMENT = 0x01700000
MI_PBT_ELF_AMSS_NON_PAGED_SHARED_SEGMENT = 0x01800000
MI_PBT_ELF_AMSS_PAGED_SHARED_SEGMENT = 0x01900000
MI_PBT_ELF_HASH_SEGMENT = 0x02200000 
MI_PBT_ELF_BOOT_SEGMENT = 0x03200000 
MI_PBT_ELF_NON_PAGED_L4BSP_SEGMENT = 0x04000000 
MI_PBT_ELF_PAGED_L4BSP_SEGMENT = 0x04100000 
#----------------------------------------------------------------------------
# GLOBAL VARIABLES END
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# CLASS DEFINITIONS BEGIN 
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
# OS Type ID Class 
#----------------------------------------------------------------------------
class OSType: 	  	 
 	BMP_BOOT_OS        = 0 	  	 
	WM_BOOT_OS         = 1 	  	 
	ANDROID_BOOT_OS    = 2 	  	 
	CHROME_BOOT_OS     = 3 	  	 
	SYMBIAN_BOOT_OS    = 4 	  	 
	LINUX_BOOT_OS      = 5

#----------------------------------------------------------------------------
# Image Type ID Class - These values must be kept consistent with mibib.h
#----------------------------------------------------------------------------
class ImageType:
   NONE_IMG           = 0
   OEM_SBL_IMG        = 1
   AMSS_IMG           = 2
   QCSBL_IMG          = 3
   HASH_IMG           = 4
   APPSBL_IMG         = 5
   APPS_IMG           = 6
   HOSTDL_IMG         = 7
   DSP1_IMG           = 8 
   FSBL_IMG           = 9
   DBL_IMG            = 10
   OSBL_IMG           = 11
   DSP2_IMG           = 12
   EHOSTDL_IMG        = 13
   NANDPRG_IMG        = 14
   NORPRG_IMG         = 15
   RAMFS1_IMG         = 16
   RAMFS2_IMG         = 17
   ADSP_Q5_IMG        = 18
   APPS_KERNEL_IMG    = 19
   BACKUP_RAMFS_IMG   = 20
   SBL1_IMG           = 21
   SBL2_IMG           = 22
   RPM_IMG            = 23
   SBL3_IMG           = 24
   TZ_IMG             = 25

#----------------------------------------------------------------------------
# Global Image Type Table
# Format of the look-up table:
# KEY -  IMAGE_TYPE string as passed into mbn_builder.py
# VALUE - [Specific ImageType ID enum, Template key string, MBN Type] 
#----------------------------------------------------------------------------
image_id_table = {
   'appsbl': [ImageType.APPSBL_IMG, 'APPSBL_IMG', 'bin'],
   'dbl': [ImageType.DBL_IMG, 'DBL_IMG', 'bin'],
   'osbl': [ImageType.OSBL_IMG, 'OSBL_IMG', 'bin'],
   'amss': [ImageType.AMSS_IMG, 'AMSS_IMG', 'elf'],
   'amss_mbn': [ImageType.HASH_IMG, 'HASH_IMG', 'elf'],
   'apps': [ImageType.APPS_IMG, 'APPS_IMG', 'bin'],
   'hostdl': [ImageType.HOSTDL_IMG, 'HOSTDL_IMG', 'bin'],
   'ehostdl': [ImageType.EHOSTDL_IMG, 'EHOSTDL_IMG', 'bin'],
   'emmcbld': [ImageType.EHOSTDL_IMG, 'EMMCBLD_IMG', 'bin'],
   'qdsp6fw': [ImageType.DSP1_IMG, 'DSP1_IMG', 'elf'],
   'qdsp6sw': [ImageType.DSP2_IMG, 'DSP2_IMG', 'elf'],
   'qdsp5': [ImageType.ADSP_Q5_IMG, 'ADSP_Q5_IMG', 'bin'],
   'tz': [ImageType.TZ_IMG, 'TZ_IMG', 'bin'],
   'rpm': [ImageType.RPM_IMG, 'RPM_IMG', 'bin'],
   'sbl1': [ImageType.SBL1_IMG, 'SBL1_IMG', 'bin'],
   'sbl2': [ImageType.SBL2_IMG, 'SBL2_IMG', 'bin'],
   'sbl3': [ImageType.SBL3_IMG, 'SBL3_IMG', 'bin'],
   'efs1': [ImageType.RAMFS1_IMG, 'RAMFS1_IMG', 'bin'],
   'efs2': [ImageType.RAMFS2_IMG, 'RAMFS2_IMG', 'bin'],
   # Add any additional image information here 
} 

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
   s = struct.Struct('16sHHIIIIIHHHHHH') 
   
   def __init__(self, data): 
      unpacked_data       = (Elf32_Ehdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.e_ident        = unpacked_data[0]
      self.e_type         = unpacked_data[1]
      self.e_machine      = unpacked_data[2]
      self.e_version      = unpacked_data[3]
      self.e_entry        = unpacked_data[4]
      self.e_phoff        = unpacked_data[5]
      self.e_shoff        = unpacked_data[6]
      self.e_flags        = unpacked_data[7]
      self.e_ehsize       = unpacked_data[8]
      self.e_phentsize    = unpacked_data[9]
      self.e_phnum        = unpacked_data[10]
      self.e_shentsize    = unpacked_data[11]
      self.e_shnum        = unpacked_data[12]
      self.e_shstrndx     = unpacked_data[13]

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getPackedData(self):
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
      
      return (Elf32_Ehdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF Program Header Class 
#----------------------------------------------------------------------------
class Elf32_Phdr:

   # Structure object to align and package the ELF Program Header 
   s = struct.Struct('I' * 8) 
   
   def __init__(self, data): 
      unpacked_data       = (Elf32_Phdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.p_type         = unpacked_data[0]
      self.p_offset       = unpacked_data[1]
      self.p_vaddr        = unpacked_data[2]
      self.p_paddr        = unpacked_data[3]
      self.p_filesz       = unpacked_data[4]
      self.p_memsz        = unpacked_data[5]
      self.p_flags        = unpacked_data[6]
      self.p_align        = unpacked_data[7]

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value
            
   def getPackedData(self):
      values = [self.p_type,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_flags,
                self.p_align
               ]
      
      return (Elf32_Phdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF Segment Information Class 
#----------------------------------------------------------------------------
class SegmentInfo:
   def __init__(self):
      self.flag  = 0
      self.start_addr = 0
   def printValues(self):
      print 'Flag: ' + str(self.flag)
      print 'Start Address: ' + str(hex(self.start_addr))
 
#----------------------------------------------------------------------------
# Regular Boot Header Class 
#----------------------------------------------------------------------------
class Boot_Hdr:
   def __init__(self, init_val):
      self.image_id = ImageType.NONE_IMG
      self.flash_parti_ver = FLASH_PARTI_VERSION
      self.image_src = init_val  
      self.image_dest_ptr = init_val  
      self.image_size = init_val  
      self.code_size = init_val 
      self.sig_ptr = init_val  
      self.sig_size = init_val  
      self.cert_chain_ptr = init_val  
      self.cert_chain_size = init_val  
      self.magic_number1 = init_val  
      self.version = init_val  
      self.OS_type = init_val 
      self.boot_apps_parti_entry = init_val  
      self.boot_apps_size_entry = init_val  
      self.boot_apps_ram_loc = init_val 
      self.reserved_ptr = init_val  
      self.reserved_1 = init_val 
      self.reserved_2 = init_val 
      self.reserved_3 = init_val 
   
   def getLength(self):
      return BOOT_HEADER_LENGTH
 
   def writePackedData(self, target, write_full_hdr):  
      values = [self.image_id,
                self.flash_parti_ver,
                self.image_src,
                self.image_dest_ptr,
                self.image_size,
                self.code_size ,
                self.sig_ptr,
                self.sig_size,
                self.cert_chain_ptr,
                self.cert_chain_size,
                self.magic_number1,
                self.version,
                self.OS_type,
                self.boot_apps_parti_entry,
                self.boot_apps_size_entry,
                self.boot_apps_ram_loc,
                self.reserved_ptr,
                self.reserved_1,
                self.reserved_2,
                self.reserved_3 ]
     
      # Write 10 entries(40B) or 20 entries(80B) of boot header 
      if write_full_hdr is False:
         s = struct.Struct('I'* 10)
         values = values[:10]
      else:
         s = struct.Struct('I' * self.getLength())
     
      packed_data = s.pack(*values)
      
      fp = OPEN(target,'wb')
      fp.write(packed_data)
      fp.close()

      return s.size

#----------------------------------------------------------------------------
# SBL Boot Header Class 
#----------------------------------------------------------------------------
class Sbl_Hdr:
   def __init__(self, init_val):
      self.codeword = init_val
      self.magic = init_val
      self.reserved_0 = init_val  
      self.reserved_1 = init_val  
      self.reserved_2 = init_val 
      self.image_src = init_val 
      self.image_dest_ptr = init_val  
      self.image_size = init_val 
      self.code_size = init_val  
      self.sig_ptr = init_val  
      self.sig_size = init_val 
      self.cert_chain_ptr = init_val 
      self.cert_chain_size = init_val 
      self.oem_root_cert_sel = init_val
      self.oem_num_root_certs = init_val 
      self.reserved_5 = init_val 
      self.reserved_6 = init_val 
      self.reserved_7 = init_val 
      self.reserved_8 = init_val  
      self.reserved_9 = init_val 
   
   def getLength(self):
      return SBL_HEADER_LENGTH
 
   def writePackedData(self, target): 
      values = [self.codeword,
                self.magic,
                self.reserved_0,
                self.reserved_1,
                self.reserved_2,
                self.image_src,
                self.image_dest_ptr,
                self.image_size,
                self.code_size,
                self.sig_ptr,
                self.sig_size,
                self.cert_chain_ptr,
                self.cert_chain_size,
                self.oem_root_cert_sel,
                self.oem_num_root_certs,
                self.reserved_5,
                self.reserved_6,
                self.reserved_7,
                self.reserved_8,
                self.reserved_9 ]
                 
      s = struct.Struct('I' * self.getLength())
      packed_data = s.pack(*values)
      
      fp = OPEN(target,'wb')
      fp.write(packed_data)
      fp.close()

      return s.size

#----------------------------------------------------------------------------
# CLASS DEFINITIONS END 
#----------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('mbn_tools')

def generate(env):

   #----------------------------------------------------------------------------
   # Generate Global Dictionary 
   #----------------------------------------------------------------------------
   # Get file names for 'cust' and 'targ' auto-generated files inside 'build/ms'
   if 'CUST_H' in env:
      cust_h = env.subst('CUST${BUILD_ID}.H').lower()
      targ_h = env.subst('TARG${BUILD_ID}.H').lower()
      cust_file_name = str(env.FindFile(cust_h, "${INC_ROOT}/build/ms"))
      targ_file_name = str(env.FindFile(targ_h, "${INC_ROOT}/build/ms")) 

      # Check that files are present 
      if (os.path.exists(cust_file_name) is True) and \
         (os.path.exists(targ_file_name) is True):

         # Populate the dictionary from the auto-generated files 
         global_dict = populate_dictionary(env, targ_file_name, cust_file_name)
      else:
         global_dict = {}   

   # Add the dictionary to the environment
   env.Replace(GLOBAL_DICT = global_dict)

   # Add methods to environment
   env.AddMethod(filter_dictionary, "FilterDictionary")
   env.AddMethod(populate_dictionary, "PopulateDictionary")
   env.AddMethod(image_auth, "ImageAuth")  
   env.AddMethod(image_header, "ImageHeader") 
   env.AddMethod(pboot_gen_elf, "PBootGenElf")
   env.AddMethod(pboot_add_hash, "PBootAddHash") 
   env.AddMethod(modify_elf_flags, "ModifyElfFlags") 
   
   return None

#----------------------------------------------------------------------------
# BOOT TOOLS BEGIN 
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# image_header 
#----------------------------------------------------------------------------
def image_header(env, gen_dict, 
                      code_file_name, 
                      output_file_name, 
                      secure_type,
                      header_format = 'reg', 
                      requires_preamble = False, 
                      preamble_file_name = None,
                      elf_file_name = None, 
                      write_full_hdr = False,
                      in_code_size = None):
   
   # Preliminary checks
   if (requires_preamble is True) and (preamble_file_name is None):
      raise RuntimeError, "Image Header requires a preamble file" 

   if (gen_dict['IMAGE_KEY_MBN_TYPE'] == 'elf') and (elf_file_name is None):
      raise RuntimeError, "ELF Image Headers require an elf file"

   if (in_code_size is None) and (os.path.exists(code_file_name) is False):
      raise RuntimeError, "Code size unavailable, and input file does not exist"

   # Initialize
   if in_code_size is not None:
      code_size = in_code_size
   else:
      code_size = os.path.getsize(code_file_name)

   image_dest = 0
   image_source = 0
   
   # If secure build, set signature and cert chain sizes
   if secure_type == 'secure':
      signature_size = SHA256_SIGNATURE_SIZE
      cert_chain_size = CERT_CHAIN_MAXSIZE 
      image_size = code_size + cert_chain_size + signature_size
      if (image_size % 4) != 0:
         image_size += (4 - (image_size % 4))
   else:
      signature_size = 0
      cert_chain_size = 0
      image_size = code_size
 
   # For ELF or hashed images, image destination will be determined from an ELF input file
   if gen_dict['IMAGE_KEY_MBN_TYPE'] == 'elf':
      image_dest = get_hash_address(elf_file_name) + MI_BOOT_IMG_HDR_SIZE 
   elif gen_dict['IMAGE_KEY_MBN_TYPE'] == 'bin':
      image_dest = gen_dict['IMAGE_KEY_IMAGE_DEST']
      image_source = gen_dict['IMAGE_KEY_IMAGE_SOURCE']        

   # Build the header based on format specified  
   if header_format == 'sbl':
      boot_sbl_header = Sbl_Hdr(init_val = int('0xFFFFFFFF',16))
      boot_sbl_header.codeword = FLASH_CODE_WORD 
      boot_sbl_header.magic = MAGIC_NUM
      boot_sbl_header.image_src = MI_BOOT_SBL_HDR_SIZE
      boot_sbl_header.image_dest_ptr = image_dest
      boot_sbl_header.image_size = image_size
      boot_sbl_header.code_size = code_size
      boot_sbl_header.sig_ptr = image_dest + code_size
      boot_sbl_header.sig_size = signature_size
      boot_sbl_header.cert_chain_ptr = image_dest + code_size + signature_size
      boot_sbl_header.cert_chain_size = cert_chain_size 
      boot_sbl_header.oem_root_cert_sel = gen_dict['IMAGE_KEY_OEM_ROOT_CERT_SEL'] 
      boot_sbl_header.oem_num_root_certs = gen_dict['IMAGE_KEY_OEM_NUM_ROOT_CERTS'] 
   
      # If preamble is required, output the preamble file and update the boot_sbl_header 
      if requires_preamble is True:
         boot_sbl_header = image_preamble(gen_dict, preamble_file_name, boot_sbl_header) 

      # Package up the header and write to output file
      boot_sbl_header.writePackedData(target = output_file_name)

   elif header_format == 'reg':
      boot_header = Boot_Hdr(init_val = int('0x0',16))
      boot_header.image_id = gen_dict['IMAGE_KEY_IMAGE_ID']
      boot_header.image_src = image_source 
      boot_header.image_dest_ptr = image_dest 
      boot_header.image_size = image_size
      boot_header.code_size = code_size
      boot_header.sig_ptr = image_dest + code_size
      boot_header.sig_size = signature_size
      boot_header.cert_chain_ptr = image_dest + code_size + signature_size
      boot_header.cert_chain_size = cert_chain_size

      # Package up the header and write to output file
      boot_header.writePackedData(target = output_file_name, write_full_hdr = write_full_hdr) 
   
   return 0 


#----------------------------------------------------------------------------
# pboot_gen_elf
#----------------------------------------------------------------------------
def pboot_gen_elf(env, elf_in_file_name,
                       hash_out_file_name,
                       elf_out_file_name = None,
                       secure_type = 'non_secure',
                       hash_seg_max_size = None,
                       last_phys_addr = None):

   # Open Files
   elf_in_fp = OPEN(elf_in_file_name, "rb")
   hash_out_fp = OPEN(hash_out_file_name, "wb+")

   if elf_out_file_name is not None:
      elf_out_fp = OPEN(elf_out_file_name, "wb+")

   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name) 
   num_phdrs = elf_header.e_phnum
   phdr_total_size = num_phdrs * elf_header.e_phentsize
   phdr_size = elf_header.e_phentsize
   hashhdr_phdr_found = False
   hashhdr_phdr_index = 0
   hashtable_size = 0
   hashtable_shift = 0
   file_offset = 0
   last_paddr = 0
   last_paddr_segment = 0
   curr_phdr = Elf32_Phdr('\0' * ELF_PHDR_SIZE)
   hash = '\0' * MI_PROG_BOOT_DIGEST_SIZE 

   # Process program headers if an output elf is specified 
   if elf_out_file_name is not None:

      # Search through program headers for hash table header
      for i in range(num_phdrs):
         idx_phdr = phdr_table[i]
         if idx_phdr.p_flags == MI_PBT_HASH_SEGMENT: 
            hashhdr_phdr_found = True
            hashhdr_phdr_index = i
            break

      # If hash table program header is not found, make sure to include it
      if hashhdr_phdr_found is False: 
         elf_header.e_phnum += 1 
         
      # First copy the new ELF header to the destination file
      elf_out_fp.seek(0)
      elf_out_fp.write(elf_header.getPackedData())

      # Create an empty hash entry for PHDR_TYPE
      hash_out_fp.write('\0' * MI_PROG_BOOT_DIGEST_SIZE)    
      hashtable_size += MI_PROG_BOOT_DIGEST_SIZE

   # Begin hash table generation
   for i in range(num_phdrs):
      curr_phdr = phdr_table[i]

      if (MI_PBT_PAGE_MODE_VALUE(curr_phdr.p_flags) == MI_PBT_PAGED_SEGMENT):
         seg_offset = curr_phdr.p_offset
         seg_size = curr_phdr.p_filesz
         hash_size = 0

         # Check if the vaddr is page aligned
         off = curr_phdr.p_vaddr & (ELF_BLOCK_ALIGN - 1)
         if off is not 0:
            seg_size -= (ELF_BLOCK_ALIGN - off)
            seg_offset += (ELF_BLOCK_ALIGN - off)

         # Seg_size should be page aligned
         if (seg_size & (ELF_BLOCK_ALIGN - 1)) is not 0:
            raise RuntimeError, "seg_size: " + hex(seg_size) + " is not ELF page aligned!"
    
         off = seg_offset + seg_size

         while seg_offset < off:

            if seg_offset < ELF_BLOCK_ALIGN:
               hash_size = seg_offset
            else:
               hash_size = ELF_BLOCK_ALIGN
            
            elf_in_fp.seek(seg_offset)
            fbuf = elf_in_fp.read(hash_size)
          
            if MI_PBT_CHECK_FLAG_TYPE(curr_phdr.p_flags) is True:    
               hash = generate_sha1_hash(fbuf) 
            else:
               hash = '\0' * MI_PROG_BOOT_DIGEST_SIZE
      
            # Write hash to file
            hash_out_fp.write(hash)
      
            hashtable_size += MI_PROG_BOOT_DIGEST_SIZE 
            seg_offset += ELF_BLOCK_ALIGN
      
      elif(MI_PBT_PAGE_MODE_VALUE(curr_phdr.p_flags) == MI_PBT_NON_PAGED_SEGMENT):
         # Read full hash entry into buffer
         elf_in_fp.seek(curr_phdr.p_offset)
         data_len = curr_phdr.p_filesz
         file_buff = elf_in_fp.read(data_len)
        
         if (MI_PBT_CHECK_FLAG_TYPE(curr_phdr.p_flags) is True) and (data_len > 0):
            hash = generate_sha1_hash(file_buff)
         else:
            hash = '\0' *  MI_PROG_BOOT_DIGEST_SIZE
        
         # Write hash to file
         hash_out_fp.write(hash)
        
         hashtable_size += MI_PROG_BOOT_DIGEST_SIZE
   # End hash table generation
   
   # Generate the rest of the ELF output file if specified
   if elf_out_file_name is not None:
      if secure_type == 'secure':
         hashtable_size += SHA256_SIGNATURE_SIZE + CERT_CHAIN_MAXSIZE 
 
      # Initialize the hash table program header
      [hash_Phdr, pad_hash_segment, hash_tbl_end_addr, hash_tbl_offset] = \
         initialize_hash_phdr(elf_in_file_name, hashtable_size, MI_BOOT_IMG_HDR_SIZE, ELF_BLOCK_ALIGN)

      # Check if hash segment max size parameter was passed
      if (hash_seg_max_size is not None):
         # Error checking for hash segment size validity
         if hashtable_size > hash_seg_max_size:
            raise RuntimeError, "Hash table exceeds maximum hash segment size: " + hex(hash_seg_max_size) 
         if (hash_seg_max_size & (ELF_BLOCK_ALIGN-1)) is not 0:
            raise RuntimeError, "Hash segment size passed is not ELF Block Aligned: " + hex(hash_seg_max_size)

      # Check if hash physical address parameter was passed
      if last_phys_addr is not None:
        hash_Phdr.p_vaddr = last_phys_addr
        hash_Phdr.p_paddr = last_phys_addr
       
      # Check if hash segment max size was passed
      if hash_seg_max_size is not None:
         hash_Phdr.p_memsz = hash_seg_max_size
     
      # Output the program header for the hash table
      file_offset = elf_header.e_phoff
      elf_out_fp.seek(file_offset)
      elf_out_fp.write(hash_Phdr.getPackedData())
      file_offset += phdr_size 

      # Check if remaining program headers need to shift
      if hashhdr_phdr_found is True:
         if hashhdr_phdr_index == 0:
            hash_tbl_shift_index = 1
         else:
            raise RuntimeError, "Hash table program header should be the first program header"
      else:
         hash_tbl_shift_index = 0
 
      curr_phdr = phdr_table[hash_tbl_shift_index]

      if curr_phdr.p_offset < (hash_tbl_offset + hashtable_size + pad_hash_segment):
         hashtable_shift = (hash_Phdr.p_filesz & ~(ELF_BLOCK_ALIGN-1)) + ELF_BLOCK_ALIGN

      # Output remaining program headers and ELF segments
      for i in range(num_phdrs):

         # Ignore PHDR_TYPE header and hash table if they're already exist
         if (hashhdr_phdr_found is False) or (i != hashhdr_phdr_index):
            
            elf_out_fp.seek(file_offset)
      
            # Increment the file offset before writing to the destination file
            curr_phdr = phdr_table[i]
            src_offset = curr_phdr.p_offset
            curr_phdr.p_offset += hashtable_shift
      
            # Copy the program header
            elf_out_fp.write(curr_phdr.getPackedData())
            
            # Copy the ELF segment
            file_copy_offset(elf_in_fp, src_offset, elf_out_fp, curr_phdr.p_offset, curr_phdr.p_filesz)

            # Update file_offset
            file_offset += phdr_size

      
      # Recalculate hash of ELF + program headers and output to hash output file
      elfhdr_buff_size = ELF_HDR_SIZE + (elf_header.e_phnum * phdr_size)
      elf_out_fp.seek(0)
      elfhdr_buff = elf_out_fp.read(elfhdr_buff_size)
      hash = generate_sha1_hash(elfhdr_buff)

      # Write hash to file as first hash table entry
      hash_out_fp.seek(0)
      hash_out_fp.write(hash)

   # Close files
   elf_in_fp.close()
   hash_out_fp.close()

   if elf_out_file_name is not None:
      elf_out_fp.close()

   return 0
    

#----------------------------------------------------------------------------
# pboot_add_hash 
#----------------------------------------------------------------------------
def pboot_add_hash(env, elf_in_file_name, 
                        hash_tbl_file_name, 
                        elf_out_file_name,
                        hash_seg_max_size = None,
                        last_phys_addr = None):
   # Open files
   elf_in_fp = OPEN(elf_in_file_name, "rb") 
   hash_tbl_fp = OPEN(hash_tbl_file_name, "rb")
   elf_out_fp = OPEN(elf_out_file_name, "wb+")  

   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name) 

   phdr_size = elf_header.e_phentsize
   hash_size = os.path.getsize(hash_tbl_file_name) 
   hash_tbl_size = hash_size - MI_BOOT_IMG_HDR_SIZE
   hash_tbl_shift = 0
   hash_hdr_offset = 0
   hash_seg_end = 0
   bytes_to_pad = 0
   phdr_offset = 0
   hash_segment_found = False
 
   # Check if hash segment max size parameter was passed
   if (hash_seg_max_size is not None):
      # Error checking for hash segment size validity
      if hash_size > hash_seg_max_size:
         raise RuntimeError, "Hash table exceeds maximum hash segment size: " + hex(hash_seg_max_size) 
      if (hash_seg_max_size & (ELF_BLOCK_ALIGN-1)) is not 0:
         raise RuntimeError, "Hash segment size passed is not ELF Block Aligned: " + hex(hash_seg_max_size)

   # Attempt to find the location of the hash program header 
   for i in range(elf_header.e_phnum):
      curr_phdr = phdr_table[i]
      if curr_phdr.p_flags == MI_PBT_ELF_HASH_SEGMENT:
         hash_segment_found = True
         break 

   if hash_segment_found is True: 
      # Copy input file to output file
      shutil.copyfileobj(elf_in_fp, elf_out_fp, os.path.getsize(elf_in_file_name))
      
      # Update ELF to insert hash table at corresponding file offset
      hash_hdr_offset = curr_phdr.p_offset
      file_copy_offset(hash_tbl_fp, 0, elf_out_fp, hash_hdr_offset, hash_size)
      
   else:
      hash_hdr_offset = ELF_BLOCK_ALIGN
      [hash_Phdr, pad_hash_segment, hash_tbl_end_addr, hash_tbl_offset] = \
         initialize_hash_phdr(elf_in_file_name, hash_tbl_size, MI_BOOT_IMG_HDR_SIZE, hash_hdr_offset)
      
      # Check if hash physical address parameter was passed
      if last_phys_addr is not None:
        hash_Phdr.p_vaddr = last_phys_addr
        hash_Phdr.p_paddr = last_phys_addr
       
      # Check if hash segment max size was passed
      if hash_seg_max_size is not None:
         hash_Phdr.p_memsz = hash_seg_max_size

      # Increment the number of program headers since we are adding the hash header
      elf_header.e_phnum += 1
       
      # First copy the new ELF header to the destination file
      elf_out_fp.seek(0)
      elf_out_fp.write(elf_header.getPackedData())
 
      # Copy the hash table program header to the destination file
      if phdr_table[0].p_type == PHDR_TYPE:
         phdr_offset = elf_header.e_phoff
         file_offset = phdr_offset + phdr_size 
         file_copy_offset(elf_in_fp, phdr_offset, elf_out_fp, phdr_offset, phdr_size)
      else:
         file_offset = elf_header.e_phoff

      # Write the next program header as the hash segment
      elf_out_fp.seek(file_offset)
      elf_out_fp.write(hash_Phdr.getPackedData())
      file_offset += phdr_size
      
      # Copy the hash header and table into the ELF
      file_copy_offset(hash_tbl_fp, 0, elf_out_fp, hash_hdr_offset, hash_size)

      # Pad hash table segment with zeros to align with ELF block size
      if hash_seg_max_size is not None:
         bytes_to_pad = hash_seg_max_size - hash_size 
      elif pad_hash_segment > 0:
         bytes_to_pad = ELF_BLOCK_ALIGN - pad_hash_segment
      
      elf_out_fp.seek(hash_tbl_end_addr)
      pad_file(elf_out_fp, bytes_to_pad, PAD_BYTE_0)  
      hash_seg_end = hash_hdr_offset + hash_size + bytes_to_pad
 
      # Copy rest of the program headers and segments to the destination ELF
      for i in range(elf_header.e_phnum - 1):
         i += 1
        
         # Calculate offsets
         elf_in_fp.seek(file_offset - phdr_size)
         elf_out_fp.seek(file_offset)

         # Get the segment description
         temp_phdr = Elf32_Phdr(elf_in_fp.read(phdr_size))
         src_offset = temp_phdr.p_offset

         # See if we need to shift the segment down to fit the hash table
         if (i == 1) and (temp_phdr.p_offset < hash_seg_end):
            hash_tbl_shift = hash_seg_end - temp_phdr.p_offset 

         # Increment the offset before writing to the destination file
         temp_phdr.p_offset += hash_tbl_shift
      
         # Copy the program header
         elf_out_fp.write(temp_phdr.getPackedData())

         # Copy the ELF segment
         file_copy_offset(elf_in_fp, src_offset, elf_out_fp, temp_phdr.p_offset, temp_phdr.p_filesz)

         # Update file_offset
         file_offset += phdr_size
        
   # Close files
   elf_in_fp.close()
   hash_tbl_fp.close()
   elf_out_fp.close() 

   return 0

#----------------------------------------------------------------------------
# image_auth 
#----------------------------------------------------------------------------
def image_auth(env, *args):
               
   if len(args) != 7:
      raise RuntimeError, "Usage Invalid"
      
   # Initialize File Names 
   binary_in            = args[0]
   signature            = args[1]
   attestation_cert     = args[2]
   attestation_ca_cert  = args[3]
   root_cert            = args[4]
   cert_chain_out       = args[5]
   signed_image_out     = args[6]
  
   # Create the Certificate Chain 
   concat_files (cert_chain_out, [attestation_cert, attestation_ca_cert, root_cert])

   # Pad to ensure Certificate Chain Size is CERT_CHAIN_MAX_SIZE
   cert_size = os.path.getsize(cert_chain_out)
 
   if cert_size < CERT_CHAIN_MAXSIZE:
      bytes_to_pad = CERT_CHAIN_MAXSIZE - cert_size
      cert_fp = OPEN(cert_chain_out,'ab')
      pad_file(cert_fp, bytes_to_pad, PAD_BYTE_1) 
      cert_fp.close() 
   else:
      raise RuntimeError, "Certificate Size too large: " + str(cert_size) 

   # Create the Final Signed Image File
   concat_files (signed_image_out, [binary_in, signature, cert_chain_out])
   
   return 0

#----------------------------------------------------------------------------
# modify_elf_flags 
#----------------------------------------------------------------------------
def modify_elf_flags(env, elf_in_file_name, 
                          scl_file_name):

   curr_phdr = Elf32_Phdr('\0' * ELF_PHDR_SIZE) 
   # Offset into program header where the p_flags field is stored  
   phdr_flag_off = 24  
 
   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name)
   segment_list = readSCL(scl_file_name, env['GLOBAL_DICT'])
 
   # Open files
   elf_in_fp = OPEN(elf_in_file_name, "r+") 
  
   # Check for corresponding number of segments
   if len(segment_list) is not elf_header.e_phnum:
      raise RuntimeError, 'SCL file and ELF file have different number of segments!'
   
   # Go to the start of the p_flag entry in the first program header 
   file_offset = elf_header.e_phoff + phdr_flag_off 

   # Change each program header flag in the ELF file based off the SCL file 
   for i in range(elf_header.e_phnum):
      # Seek to correct location and create new p_flag value  
      elf_in_fp.seek(file_offset)
      curr_phdr = phdr_table[i]
      new_flag = (curr_phdr.p_flags & ~MI_PBT_FLAGS_MASK) | (segment_list[i].flag)

      # Create structure to package new flag field
      s = struct.Struct('I') 
      new_flag_bytes = s.pack(new_flag)

      # Write the new flag value and increment offset
      elf_in_fp.write(new_flag_bytes)  
      file_offset += elf_header.e_phentsize 

   # Close files
   elf_in_fp.close()

   return 0 

#----------------------------------------------------------------------------
# BOOT TOOLS END
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# HELPER FUNCTIONS BEGIN
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Create a list to hold all segment information from an input SCL file 
#----------------------------------------------------------------------------
def readSCL(filename, global_dict):
    
  scl_fp = OPEN(filename,'r')
  
  # Initialize
  file_data = scl_fp.readlines()
  num_lines = len(file_data)
  current_line = ''
  previous_line = ''
  strip_chars = '(){}[]'
  i = 0
  bracket_counter = 0
  seg_list = []

  # Parse through all lines
  while i < num_lines:
 
     # Save the last line read
     previous_line = current_line
     current_line = file_data[i]

     # Look for the symbol '{' for the line to read. 
     # Use bracket counter to skip nested '{ }'
     if ('{' in current_line):
        if bracket_counter is 0: 
           # Create a new SegmentInfo class and set up tokens
           new_scl_entry = SegmentInfo() 
           previous_line = previous_line.strip()
           tokens = previous_line.split(' ')
          
           # Check that at least two tokens were parsed
           # Token 1: Segment Name
           # Token 2: Start Address
           if len(tokens) < 2:
              raise RuntimeError, 'SCL Segment Syntax malformed: ' + previous_line

           # Get the segment flags corresponding to the segment name description
           new_scl_entry.flag = getSegmentFlag(tokens[0].strip(strip_chars))

           # Get the start address of the segment based off the SCL string
           start_addr = tokens[1].strip(strip_chars)
           try:
              start_addr_final = eval(start_addr)
           # Catch NameError and attempt to locate start address from dictionary
           except NameError:
              if start_addr in global_dict:
                 return global_dict[start_addr]
              else:
                 raise RuntimeError, 'Unrecognized start address: ' + start_addr

           new_scl_entry.start_addr = start_addr_final 
           seg_list.append(new_scl_entry)  
        
        bracket_counter += 1 
     elif '}' in current_line:
        bracket_counter -= 1
      
     i+=1
 
  scl_fp.close() 
  return seg_list

#----------------------------------------------------------------------------
# Given a string parsed from a SCL file, returns the ELF segment flags  
#----------------------------------------------------------------------------
def getSegmentFlag(seg_info):

   ret_val = None

   # Define string values for various types of segments
   RO = "RO"
   RW = "RW"
   ZI = "ZI"
   PAGEABLE = "PAGED"
   NOTPAGEABLE = "NOTPAGED"
   SWAPABLE = "SWAPPED"
   SWAP_POOL = "SWAP_POOL"
   RESERVED = "RESERVED"
   HASHTBL = "HASH"
   SHARED = "SHARED"
   NOTUSED = "NOTUSED"
   BOOT_SEGMENT = "BOOT_SEGMENT"
   CODE = "CODE"
   L4BSP = "L4BSP"
   POOL_INDEX_0 = "INDEX_0"
   POOL_INDEX_1 = "INDEX_1"

   if seg_info is None or len(seg_info) is 0:
      raise RuntimeError, 'Invalid segment information passed: ' + seg_info

   # Conditional checks and assignments of the corresponding segment flag values
   if NOTPAGEABLE in seg_info:
      if RO in seg_info: 
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT
      elif CODE in seg_info:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT
      elif ZI in seg_info: 
         if SWAP_POOL in seg_info:
            if POOL_INDEX_0 in seg_info:
               ret_val = MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX0
            else:
               ret_val = MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX1
         else:
            ret_val = MI_PBT_ELF_AMSS_NON_PAGED_ZI_SEGMENT          

      elif NOTUSED in seg_info:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_NOTUSED_SEGMENT 
     
      elif SHARED in seg_info:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_SHARED_SEGMENT
      elif HASHTBL in seg_info:
         ret_val = MI_PBT_ELF_HASH_SEGMENT
      elif BOOT_SEGMENT in seg_info:
         ret_val = MI_PBT_ELF_BOOT_SEGMENT
      elif L4BSP in seg_info:
         ret_val = MI_PBT_ELF_NON_PAGED_L4BSP_SEGMENT
      else:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_RW_SEGMENT 

   elif PAGEABLE in seg_info:
      if RO in seg_info or CODE in seg_info:
         if SWAPABLE in seg_info:
            if POOL_INDEX_0 in seg_info:
               ret_val = MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX0 
            else:
               ret_val = MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX1 
         else:
            ret_val = MI_PBT_ELF_AMSS_PAGED_RO_SEGMENT
      elif ZI in seg_info:
         ret_val = MI_PBT_ELF_AMSS_PAGED_ZI_SEGMENT

      elif NOTUSED in seg_info:
         ret_val = MI_PBT_ELF_AMSS_PAGED_NOTUSED_SEGMENT 
      elif SHARED in seg_info:
         ret_val = MI_PBT_ELF_AMSS_PAGED_SHARED_SEGMENT
      elif L4BSP in seg_info:
         ret_val = MI_PBT_ELF_PAGED_L4BSP_SEGMENT
      else:
         ret_val = MI_PBT_ELF_AMSS_PAGED_RW_SEGMENT       
  
   else: 
      raise RuntimeError, 'The segment name is wrongly defined in the SCL file: ' + seg_info
   
   return ret_val

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

#----------------------------------------------------------------------------
# Concatenates the files listed in 'sources' in order and writes to 'target'
#----------------------------------------------------------------------------
def concat_files (target, sources):
   if type(sources) is not list:
      sources = [sources]

   target_file = OPEN(target,'wb')

   for fname in sources:
      file = OPEN(fname,'rb')
      while True:
         bin_data = file.read(65536)
         if not bin_data:
            break
         target_file.write(bin_data)
      file.close()
   target_file.close()


#----------------------------------------------------------------------------
# Populate the dictionary from a list of input files 
#----------------------------------------------------------------------------
def populate_dictionary(env, *args):
    
   if len(args) < 1:
      raise RuntimeError, "At least 1 file must be specified as an input"
   
   global_dict = {}
   Fields = ["Define", "Key", "Value"]
 
   # For each input file 
   for i in range(len(args)):
    
      template_file_path = args[i] 
      instream = OPEN(template_file_path, 'r')
      # Tokenize each line with a white space
      values = csv.DictReader(instream, Fields, delimiter=" ")

      for values in itertools.izip(values):
         new_entry = values[0]
         # Verify the parsed tokens 
         if (new_entry['Define'] == '#define') and \
            (new_entry['Key'] != None) and \
            (new_entry['Value'] != None):
          
            new_key   = new_entry['Key'].strip()
            new_value = new_entry['Value'].strip()
            
            # If value pair is empty string, assume feature definition is true
            if new_value == '':
               new_value = 'yes'
            
            # Check for and handle text replacements as we parse 
            if global_dict is not None and len(global_dict.keys()) > 0:
               for key in global_dict:
                  new_value = new_value.replace(key, str(global_dict.get(key)))
                   
            # Attempt to evaluate value
            try:
               new_value = eval(new_value)
            # Catch exceptions and do not evaluate
            except:
               pass
             
            # Add to global dictionary
            global_dict[new_key] = new_value
      instream.close()
    
   return global_dict

#----------------------------------------------------------------------------
# Filter out a generic dictionary from the global dictionary
#----------------------------------------------------------------------------
def filter_dictionary(env, global_dict, **kwargs):
 
   # Parse the argument list for necessary build arguments 
   flash_type = kwargs.get('FLASH_TYPE')
   image_type = kwargs.get('IMAGE_TYPE')

   # Check for MBN Type
   # If MBN_TYPE parameter is not provided, default to 'elf'
   if not kwargs.has_key('MBN_TYPE'):
      mbn_type = 'elf'
   else:
      mbn_type = kwargs.get('MBN_TYPE')
      if mbn_type != 'elf' and mbn_type != 'bin':
         raise RuntimeError, "MBN_TYPE currently not supported: " + mbn_type 

   # Check for Image ID
   # If IMAGE_ID parameter is not provided, default to ID 0
   if not kwargs.has_key('IMAGE_ID'):
      image_id = ImageType.NONE_IMG 
   else:
      image_id = kwargs.get('IMAGE_ID')
      if type(image_id) is not int:
         raise RuntimeError, "IMAGE_ID must be of integer type."

   # Initialize
   gen_dict = {}
   image_dest = 0
   image_source = 0
   
   # Check for image_type 
   if image_type not in image_id_table:
      id = image_id
      id_match_str = image_type.upper() + "_IMG"
      id_mbn_type = mbn_type
   else:
      id = image_id_table[image_type][0]
      id_match_str = image_id_table[image_type][1]
      id_mbn_type = image_id_table[image_type][2]  

   # Handle MBN Type and assign image destination address 
   if id_mbn_type is 'elf':
      pass
   elif id_mbn_type is 'bin':
      template_key_match = 'IMAGE_KEY_' + id_match_str + "_DEST_ADDR"  
      if template_key_match in global_dict:
         image_dest = global_dict[template_key_match]
      else:
         raise RuntimeError, "Builds file does not have IMAGE_KEY pair for: " + image_type
   else:
      raise RuntimeError, "MBN_TYPE currently not supported: " + mbn_type  

   # Assign generic dictionary key/value pairs
   gen_dict['IMAGE_KEY_IMAGE_ID'] = id
   gen_dict['IMAGE_KEY_IMAGE_DEST'] = image_dest
   gen_dict['IMAGE_KEY_IMAGE_SOURCE'] = image_source
   gen_dict['IMAGE_KEY_FLASH_TYPE'] = flash_type 
   gen_dict['IMAGE_KEY_MBN_TYPE'] = id_mbn_type 
   gen_dict['IMAGE_KEY_ID_MATCH_STR'] = id_match_str 
   gen_dict['IMAGE_KEY_FLASH_AUTO_DETECT_MAX_PAGE'] = \
      get_dict_value(global_dict,'FLASH_AUTO_DETECT_MAX_PAGE', 8192) 
   gen_dict['IMAGE_KEY_FLASH_AUTO_DETECT_MIN_PAGE'] = \
      get_dict_value(global_dict,'FLASH_AUTO_DETECT_MIN_PAGE', 2048) 
   gen_dict['IMAGE_KEY_MAX_SIZE_OF_VERIFY_BUFFER'] = \
      get_dict_value(global_dict,'MAX_SIZE_OF_VERIFY_BUFFER', 8192) 
   gen_dict['IMAGE_KEY_BOOT_SMALL_PREAMBLE'] = \
      get_dict_value(global_dict,'BOOT_SMALL_PREAMBLE', 1) 
   
   # Get OEM root certificate select and number
   oem_root_cert_sel = get_dict_value(global_dict,'OEM_ROOT_CERT_SEL', 1) 
   oem_num_root_certs = get_dict_value(global_dict,'OEM_NUM_ROOT_CERTS', 1) 

   # Error checking for OEM configurable values
   if oem_root_cert_sel in range(1, MAX_NUM_ROOT_CERTS + 1) and \
      oem_num_root_certs in range(1, MAX_NUM_ROOT_CERTS + 1) and \
      oem_root_cert_sel <= oem_num_root_certs:
      
      gen_dict['IMAGE_KEY_OEM_ROOT_CERT_SEL'] = oem_root_cert_sel
      gen_dict['IMAGE_KEY_OEM_NUM_ROOT_CERTS'] = oem_num_root_certs 
   
   else: 
      raise RuntimeError, "Invalid OEM root certificate configuration values" 
 
   # Assign additional dictionary key/values pair as needed by tools. 

   return gen_dict 


#----------------------------------------------------------------------------
# Get index value from dictionary if exists, otherwise return default 
#----------------------------------------------------------------------------
def get_dict_value(dict, key_string, default):
    
   key = 'IMAGE_KEY_' + key_string
    
   if key in dict:
      return dict[key]
   else:
      return default
    
#----------------------------------------------------------------------------
# Preprocess an ELF file and return the ELF Header Object and an 
# array of ELF Program Header Objects
#----------------------------------------------------------------------------
def preprocess_elf_file(elf_file_name):

   # Initialize
   elf_fp = OPEN(elf_file_name, 'rb')
   elf_header = Elf32_Ehdr(elf_fp.read(ELF_HDR_SIZE))
   phdr_table = []

   # Verify ELF header information
   if verify_elf_header(elf_header) is False:
      raise RuntimeError, "ELF file failed verification: " + elf_file_name

   # Get program header size
   phdr_size = elf_header.e_phentsize

   # Find the program header offset 
   file_offset = elf_header.e_phoff
   elf_fp.seek(file_offset) 
   
   # Read in the program headers
   for i in range(elf_header.e_phnum):
      phdr_table.append(Elf32_Phdr(elf_fp.read(phdr_size))) 

   elf_fp.close()
   return [elf_header, phdr_table]

#----------------------------------------------------------------------------
# Get the hash table address from an input ELF file
#----------------------------------------------------------------------------
def get_hash_address(elf_file_name):

   [elf_header, phdr_table] = preprocess_elf_file(elf_file_name)

   last_paddr = 0
   last_paddr_segment = 0
 
   # Find the segment with the largest physical address.
   # Hash segment's physical address will be immediately after this segment. 
   for i in range(elf_header.e_phnum):
      curr_phdr = phdr_table[i]
      if curr_phdr.p_paddr > last_paddr:
         # Skip the demand paging segment as it would be outside the physical RAM location 
         if MI_PBT_SEGMENT_TYPE_VALUE(curr_phdr.p_flags) != MI_PBT_SWAPPED_SEGMENT:
            last_paddr = curr_phdr.p_paddr;
            last_paddr_segment = i;
  
   max_phdr = phdr_table[last_paddr_segment]
  
   ret_val  = (((max_phdr.p_paddr + max_phdr.p_memsz - 1) & \
              ~(ELF_BLOCK_ALIGN-1)) + ELF_BLOCK_ALIGN)

   return ret_val 

#----------------------------------------------------------------------------
# Verify ELF header contents from an input ELF file
#----------------------------------------------------------------------------
def verify_elf_header(elf_header):
   if (elf_header.e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0) or \
      (elf_header.e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1) or \
      (elf_header.e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2) or \
      (elf_header.e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3) or \
      (elf_header.e_ident[ELFINFO_CLASS_INDEX] != ELFINFO_CLASS_32) or \
      (elf_header.e_ident[ELFINFO_VERSION_INDEX] != ELFINFO_VERSION_CURRENT):

      return False
   else:
      return True

#----------------------------------------------------------------------------
# Perform file copy given offsets and the number of bytes to copy
#----------------------------------------------------------------------------
def file_copy_offset(in_fp, in_off, out_fp, out_off, num_bytes):
   in_fp.seek(in_off)
   out_fp.seek(out_off)
   shutil.copyfileobj(in_fp, out_fp, num_bytes) 
 
   return num_bytes
  
#----------------------------------------------------------------------------
# SHA1 hash routine wrapper 
#----------------------------------------------------------------------------
def generate_sha1_hash(in_buf):
   # Initialize a SHA1 object from the Python hash library
   m = hashlib.sha1()
   # Set the input buffer and return the output digest
   m.update(in_buf)
   return m.digest()

#----------------------------------------------------------------------------
# Initialize the hash program header.
#----------------------------------------------------------------------------
def initialize_hash_phdr(elf_in_file_name, hash_tbl_size, hdr_size, hdr_offset):
   # Set hash header offset to page size boundary. Hash table will be
   # located at first segment of elf image.
   hash_hdr_size =  hdr_size 
   hash_hdr_offset = hdr_offset
   hash_tbl_offset = hash_hdr_offset + hash_hdr_size
   hash_tbl_end_addr = hash_tbl_offset + hash_tbl_size;
   pad_hash_segment = (hash_tbl_end_addr) & (ELF_BLOCK_ALIGN-1)
    
   # Update the hash table program header
   hash_Phdr = Elf32_Phdr('\0'*32) 
   hash_Phdr.p_flags = MI_PBT_ELF_HASH_SEGMENT
   hash_Phdr.p_align = ELF_BLOCK_ALIGN
   hash_Phdr.p_offset = hash_hdr_offset
   hash_Phdr.p_memsz = hash_hdr_size + hash_tbl_size + (ELF_BLOCK_ALIGN - pad_hash_segment)
   hash_Phdr.p_filesz = hash_hdr_size + hash_tbl_size
   hash_Phdr.p_type = LOAD_TYPE
   hash_Phdr.p_vaddr = get_hash_address(elf_in_file_name) 
   hash_Phdr.p_paddr = hash_Phdr.p_vaddr

   return [hash_Phdr, pad_hash_segment, hash_tbl_end_addr, hash_tbl_offset]

#----------------------------------------------------------------------------
# image_preamble 
#----------------------------------------------------------------------------
def image_preamble(gen_dict, preamble_file_name, boot_sbl_header):
   # Generate the preamble file 
   preamble_fp = OPEN(preamble_file_name, 'wb')      
    
   # Initialize 
   max_size_verify = gen_dict['IMAGE_KEY_MAX_SIZE_OF_VERIFY_BUFFER'] 
   flash_max_page = gen_dict['IMAGE_KEY_FLASH_AUTO_DETECT_MAX_PAGE']
   flash_min_page = gen_dict['IMAGE_KEY_FLASH_AUTO_DETECT_MIN_PAGE']
   autodetectpage = [int('0xFFFFFFFF',16)] * max_size_verify 
    
   # The first three entries in the preamble must include the following values
   autodetectpage[0] = FLASH_CODE_WORD 
   autodetectpage[1] = MAGIC_NUM
   autodetectpage[2] = AUTODETECT_PAGE_SIZE_MAGIC_NUM
 
   # Package the list into binary data to be written to the preamble 
   s = struct.Struct('I' * max_size_verify)
   packed_data = s.pack(*autodetectpage)

   # Output preamble pages based on maximum/minimum page size support
   for i in range(flash_max_page/flash_min_page):
      preamble_fp.write(packed_data[:flash_min_page])

   # Determine appropriate amount of padding for the preamble and
   # update the boot_sbl_header accordingly
   if gen_dict['IMAGE_KEY_BOOT_SMALL_PREAMBLE'] == 1:
      boot_sbl_header.image_src += (flash_max_page + flash_min_page)
      amount_to_write = flash_min_page
   else:
      boot_sbl_header.image_src += flash_max_page * 2 
      amount_to_write = flash_max_page
        
   pad_file(preamble_fp, amount_to_write, PAD_BYTE_1) 
   preamble_fp.close()

   return boot_sbl_header

#----------------------------------------------------------------------------
# Helper functions to parse ELF program headers 
#----------------------------------------------------------------------------
def MI_PBT_SEGMENT_TYPE_VALUE(x):
    return ( ((x) & MI_PBT_FLAG_SEGMENT_TYPE_MASK) >> MI_PBT_FLAG_SEGMENT_TYPE_SHIFT )

def MI_PBT_PAGE_MODE_VALUE(x):
    return ( ((x) & MI_PBT_FLAG_PAGE_MODE_MASK) >> MI_PBT_FLAG_PAGE_MODE_SHIFT ) 

def MI_PBT_ACCESS_TYPE_VALUE(x): 
    return ( ((x) & MI_PBT_FLAG_ACCESS_TYPE_MASK) >> MI_PBT_FLAG_ACCESS_TYPE_SHIFT )

def MI_PBT_CHECK_FLAG_TYPE(x):
    return (MI_PBT_SEGMENT_TYPE_VALUE(x) != MI_PBT_HASH_SEGMENT) and \
           (MI_PBT_ACCESS_TYPE_VALUE(x) != MI_PBT_NOTUSED_SEGMENT) and \
           (MI_PBT_ACCESS_TYPE_VALUE(x) != MI_PBT_SHARED_SEGMENT)


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


#----------------------------------------------------------------------------
# HELPER FUNCTIONS END 
#----------------------------------------------------------------------------





