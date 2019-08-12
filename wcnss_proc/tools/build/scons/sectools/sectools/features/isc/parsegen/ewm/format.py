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

from sectools.common.parsegen import PAD_BYTE_0
from sectools.common.parsegen.elf.header.format import Elf32_Ehdr
from sectools.common.parsegen.elf.segment.format import Elf32_Phdr
from sectools.common.utils.c_logging import logger
from sectools.features.isc.parsegen.base import SecParseGenBase
from sectools.features.isc.parsegen.ewm.header import EwmElf32_Ehdr
from sectools.features.isc.parsegen.ewm.segment import EwmElf32_Phdr,\
    ELF_DEFAULT_PHDR_OFFSET
from sectools.features.isc.parsegen.mbn import SecParseGenMbn


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

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt

        # Get the mbn data
        data = SecParseGenMbn.get_data(self, integrity_check, sign, encrypt)

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
        return (EwmElf32_Ehdr(self.image_entry).pack() +
                EwmElf32_Phdr(self.image_entry, len(data), self.relocatable).pack()
                ).ljust(ELF_DEFAULT_PHDR_OFFSET, PAD_BYTE_0) + data

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

