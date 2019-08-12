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
from sectools.common.utils.c_logging import logger


BIN_FILE_TYPE = 'bin'


class SecParseGenBin(SecParseGenBase):
    
    def __init__(self, data, imageinfo=None, bin_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)
        
        # Check the arguments
        if imageinfo is not None:
            bin_properties = imageinfo.image_type.bin_properties
        if bin_properties is None:
            raise RuntimeError('BIN properties must not be None.')
        
        # Extract the encryption params
        self.data, self.encryption_params = self.extract_encryption_params(data)
            
    def __repr__(self):
        return ('Base Properties: ' + '\n' + SecParseGenBase.__repr__(self))
    
    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def file_type(cls):
        return BIN_FILE_TYPE
    
    def get_data(self, sign=None, encrypt=None):
        # Resolve the operation
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt
    
        # Allow base to do any checks
        SecParseGenBase.get_data(self, sign, encrypt)
        if sign:
            logger.error('Bin Images do not support signing. Returning the raw image.')
        return self._get_data_int(False, encrypt)     
    
    @property
    def data_to_sign(self):       
        raise RuntimeError('Bin Images do not support signing')
    
    #--------------------------------------------------------------------------
    # Helper methods
    #--------------------------------------------------------------------------
    def _get_data_int(self, sign, encrypt):
        retval = self.data
        if encrypt:
            retval = self.encryption_params + self.encrypt_segment(retval, 
                                                                   segment_num=0)
        return retval
    
    
# Register the parsegen with the factory
SecParseGenBin.register()
    
    