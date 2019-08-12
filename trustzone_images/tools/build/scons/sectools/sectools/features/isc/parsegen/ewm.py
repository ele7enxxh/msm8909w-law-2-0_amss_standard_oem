#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#=============================================================================== 
'''
Created on Nov 25, 2013

@author: hraghav
'''

from sectools.common.parsegen import PAD_BYTE_0, elf
from sectools.common.parsegen.elf.header.format import Elf32_Ehdr
from sectools.common.parsegen.elf.segment.format import Elf32_Phdr
from sectools.common.utils.c_logging import logger
from sectools.features.isc.parsegen.base import SecParseGenBase
from sectools.features.isc.parsegen.mbn import SecParseGenMbn


# ELF default header values
ELF_DEFAULT_HDR_MACHINE     = 0xa4
ELF_DEFAULT_HDR_PHOFF       = 52
ELF_DEFAULT_HDR_SHOFF       = 0
ELF_DEFAULT_HDR_FLAGS       = 3
ELF_DEFAULT_HDR_EHSIZE      = 52
ELF_DEFAULT_HDR_PHENTSIZE   = 32
ELF_DEFAULT_HDR_PHNUM       = 1
ELF_DEFAULT_HDR_SHENTSIZE   = 0
ELF_DEFAULT_HDR_SHNUM       = 0
ELF_DEFAULT_HDR_SHSTRNDX    = 0

# ELF default pheader values
ELF_DEFAULT_PHDR_TYPE       = 1
ELF_DEFAULT_PHDR_OFFSET     = 0x1000
ELF_DEFAULT_PHDR_FLAGS      = 7
ELF_DEFAULT_PHDR_ALIGN      = 0
ELF_RELOCATABLE_PHDR_FLAGS  = 0x8000007
ELF_RELOCATABLE_PHDR_ALIGN  = 0x100000


EWM_FILE_TYPE = 'ewm'


class SecParseGenEwm(SecParseGenMbn):
    
    def __init__(self, data, imageinfo=None, ewm_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)
        
        # Check the arguments
        if imageinfo is not None:
            ewm_properties = imageinfo.image_type.ewm_properties
            general_properties = imageinfo.general_properties
        if ewm_properties is None:
            raise RuntimeError('EWM properties must not be None.')
        
        # Initialize internal properties
        self._image_entry = 0
        self._relocatable = False
        
        # Set properties from the config file
        self.image_entry = int(ewm_properties.image_entry, 16)
        self.relocatable = ewm_properties.relocatable
        
        # Remove elf header if it already exists
        data = self.extract_elf_wrapper(data)
        
        # Initialize the base mbn parser
        SecParseGenMbn.__init__(self, data, None, ewm_properties, general_properties,
                                self.encdec, debug_dir, debug_prefix, debug_suffix)
    
    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def file_type(cls):
        return 'ewm'
    
    def get_data(self, sign=None, encrypt=None):
        # Resolve the operation
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt
    
        # Get the mbn data
        data = SecParseGenMbn.get_data(self, sign, encrypt)
        
        # Wrap the data in elf
        data = self.inject_elf_wrapper(data)
        
        return data
        
    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'image_entry'
    #--------------------------------------------------------------------------
    @property
    def image_entry(self):
        return self._image_entry
    
    @image_entry.setter
    def image_entry(self, value):
        self.validate_image_entry(value)
        self._image_entry = value
        
    def validate_image_entry(self, value):
        pass
        
    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'relocatable'
    #--------------------------------------------------------------------------
    @property
    def relocatable(self):
        return self._relocatable
    
    @relocatable.setter
    def relocatable(self, value):
        self.validate_relocatable(value)
        self._relocatable = value
        
    def validate_relocatable(self, value):
        pass
    
    #--------------------------------------------------------------------------
    # Helper functions
    #-------------------------------------------------------------------------- 
    def inject_elf_wrapper(self, data):
        return (self._get_elf_hdr(self.image_entry) +
                self._get_elf_phdr(self.image_entry, len(data))
                ).ljust(ELF_DEFAULT_PHDR_OFFSET, PAD_BYTE_0) + data
     
    def _get_elf_hdr(self, image_entry):
        elf_hdr = Elf32_Ehdr()
        elf_hdr.e_ident_mag = elf.header.ident.ELFMAG
        elf_hdr.e_ident_class = elf.header.ident.ELFCLASS32
        elf_hdr.e_ident_data = elf.header.ident.ELFDATA2LSB
        elf_hdr.e_ident_ver = elf.header.ident.ELFVERCURRENT
        elf_hdr.e_type = elf.header.type.ET_EXEC
        elf_hdr.e_machine = ELF_DEFAULT_HDR_MACHINE
        elf_hdr.e_version = elf.header.version.EV_CURRENT
        elf_hdr.e_entry = image_entry
        elf_hdr.e_phoff = ELF_DEFAULT_HDR_PHOFF
        elf_hdr.e_shoff = ELF_DEFAULT_HDR_SHOFF
        elf_hdr.e_flags = ELF_DEFAULT_HDR_FLAGS
        elf_hdr.e_ehsize = ELF_DEFAULT_HDR_EHSIZE
        elf_hdr.e_phentsize = ELF_DEFAULT_HDR_PHENTSIZE
        elf_hdr.e_phnum = ELF_DEFAULT_HDR_PHNUM
        elf_hdr.e_shentsize = ELF_DEFAULT_HDR_SHENTSIZE
        elf_hdr.e_shnum = ELF_DEFAULT_HDR_SHNUM
        elf_hdr.e_shstrndx = ELF_DEFAULT_HDR_SHSTRNDX  
        return elf_hdr.pack()
    
    def _get_elf_phdr(self, image_entry, image_size):
        elf_phdr = Elf32_Phdr()
        elf_phdr.p_type = elf.segment.type.PT_LOAD
        elf_phdr.p_offset = ELF_DEFAULT_PHDR_OFFSET
        elf_phdr.p_vaddr = image_entry
        elf_phdr.p_paddr = image_entry
        elf_phdr.p_filesz = image_size
        elf_phdr.p_memsz = image_size
        elf_phdr.p_flags = ELF_RELOCATABLE_PHDR_FLAGS if self.relocatable else ELF_DEFAULT_PHDR_FLAGS
        elf_phdr.p_align = ELF_RELOCATABLE_PHDR_ALIGN if self.relocatable else ELF_DEFAULT_PHDR_ALIGN
        return elf_phdr.pack()
    
    def extract_elf_wrapper(self, data):
        updated_data = data
        
        try:
            elf_hdr = Elf32_Ehdr(updated_data[:Elf32_Ehdr.get_size()])
        except Exception:
            logger.note('EWM file is not wrapped')
        else:    
            updated_data = updated_data[Elf32_Ehdr.get_size():]
            
            elf_phdr = Elf32_Phdr(updated_data[:Elf32_Phdr.get_size()])
            updated_data = updated_data[Elf32_Phdr.get_size():]
            
            offset = elf_phdr.p_offset - Elf32_Phdr.get_size() - Elf32_Ehdr.get_size()
            padding = updated_data[:offset]
            updated_data = updated_data[offset:]
            
            logger.debug('Elf Wrapper Info:' + 
                         '\n' + repr(elf_hdr) +
                         '\n' + repr(elf_phdr) + 
                         '\n' + 'Padding: ' +
                         '\n' + padding + '\n')
            
            self.store_debug_data('ewm_elf_header', elf_hdr.pack())
            self.store_debug_data('ewm_elf_pheader', elf_phdr.pack())
            self.store_debug_data('ewm_elf_wrapper', (elf_hdr.pack() + 
                                                      elf_phdr.pack() +
                                                      padding))
            self.store_debug_data('ewm_mbn', updated_data)
        
        return updated_data
    
SecParseGenEwm.register()
