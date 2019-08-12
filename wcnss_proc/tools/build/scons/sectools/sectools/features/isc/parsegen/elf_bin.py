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

from sectools.features.isc.parsegen.base import SecParseGenBase
from sectools.features.isc.parsegen.elf import SecParseGenElf


# Elf_Bin file type
ELF_BIN_FILE_TYPE               = 'elf_bin'


class SecParseGenElfBin(SecParseGenElf):

    def __init__(self, data, imageinfo=None, elf_properties=None,
        general_properties=None,
        encdec=None,
        debug_dir=None,
        debug_prefix=None,
        debug_suffix=None):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)

        # Check if file is encrypted
        bin_encryption_params = ''
        if self.encdec is not None:
            data, bin_encryption_params = self.encdec.extract_encryption_parameters(data)
            if bin_encryption_params:
                decryptor = self.encdec.get_decryptor(bin_encryption_params, self.encdec.get_dummy_key())
                data = decryptor.decrypt_segment(data, 0)

        SecParseGenElf.__init__(self, data, imageinfo=imageinfo,
                                elf_properties=elf_properties,
                                general_properties=general_properties,
                                encdec=self.encdec,
                                debug_dir=debug_dir,
                                debug_prefix=debug_prefix,
                                debug_suffix=debug_suffix)
        self.encryption_params = bin_encryption_params

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def file_type(cls):
        return ELF_BIN_FILE_TYPE

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check=integrity_check,
                                 sign=sign, encrypt=False)

        # Get the signed data from the elf parser
        data = SecParseGenElf.get_data(self, integrity_check=integrity_check,
                                       sign=sign, encrypt=False)

        # Add encryption params if encrypt is true
        if encrypt:
            data = self.encryption_params + self.encrypt_segment(data, 0)

        return data

    @property
    def data_to_sign(self):
        encrypt = self.encrypt
        self.encrypt = False

        # Get the data_to_sign from the elf parser
        data_to_sign = SecParseGenElf.data_to_sign.fget(self)

        self.encrypt = encrypt
        return data_to_sign

# Register the parsegen with the factory
SecParseGenElfBin.register()
