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
from sectools.features.isc.parsegen.mbn import SecParseGenMbn


# Mbn_Bin file type
MBN_BIN_FILE_TYPE               = 'mbn_bin'


class SecParseGenMbnBin(SecParseGenMbn):
    
    def __init__(self, data, imageinfo=None, mbn_properties=None, 
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
        
        SecParseGenMbn.__init__(self, data, imageinfo=imageinfo,
                                mbn_properties=mbn_properties,
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
        return MBN_BIN_FILE_TYPE
    
    def get_data(self, sign=None, encrypt=None):
        # Resolve the operation
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt
            
        # Allow base to do any checks
        SecParseGenBase.get_data(self, sign=sign, encrypt=False)
        
        # Get the signed data from the elf parser
        data = SecParseGenMbn.get_data(self, sign=sign, encrypt=False)
        
        # Add encryption params if encrypt is true
        if encrypt:
            data = self.encryption_params + self.encrypt_segment(data, 0)
            
        return data
    
    @property
    def data_to_sign(self):
        encrypt = self.encrypt
        self.encrypt = False
        
        # Get the data_to_sign from the elf parser
        data_to_sign = SecParseGenMbn.data_to_sign.fget(self)
        
        self.encrypt = encrypt
        return data_to_sign
    
    
# Register the parsegen with the factory
SecParseGenMbnBin.register()
    