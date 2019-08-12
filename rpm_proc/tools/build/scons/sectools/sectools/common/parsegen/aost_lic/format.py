#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

import defines as df
from sectools.common.parsegen.aost_lic.header import extract_header,\
    create_empty_header
from sectools.common.utils.c_misc import store_debug_data_to_file


class ParseGenAOSTLic(object):

    def __init__(self, data=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 ):
        # Public properties
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix

        # Internal variables
        self._metadata = ''

        # Handle empty license creation
        if data is None:
            data = self.create_empty_lic()

        # Store the original image
        self.store_debug_data(df.FILE_DATA_IN, data)

        # Extract the header
        data, self.header = extract_header(data)

        # Extract the metadata
        data, self.metadata = extract_metadata(data, self.header)
        self.store_debug_data(df.FILE_METADATA_IN, self.metadata)

        # Extract the signing attributes
        data, self.sign = extract_sign(data)
        self.store_debug_data(df.FILE_SIGN_IN, self.sign)
        data, self.cert_chain = extract_cert_chain(data)
        self.store_debug_data(df.FILE_CERT_CHAIN_IN, self.cert_chain)

    def __repr__(self):
        return ('Header: ' + '\n' + repr(self.header))

    @property
    def metadata(self):
        return self._metadata

    @metadata.setter
    def metadata(self, value):
        value_len = len(value)
        if value_len > df.AOSTLMAXMETASIZE:
            raise RuntimeError('Length of metadata ' + hex(value_len) + ' '
                               'is more than the allowed value ' + hex(df.AOSTLMAXMETASIZE))
        self._metadata = value

    def store_debug_data(self, file_name, data, prefix=None, suffix=None):
        if prefix is None:
            prefix = self.debug_prefix
        if suffix is None:
            suffix = self.debug_suffix
        if prefix is not None and suffix is not None:
            store_debug_data_to_file(prefix + '_' + file_name + suffix,
                                     data, self.debug_dir)

    def create_empty_lic(self):
        header = create_empty_header()
        return header.pack()

    def get_data(self):
        # Get the prog data
        self.header.metadata_size = len(self.metadata)
        data = self.header.pack() + self.metadata + self.sign + self.cert_chain
        return data


def extract_metadata(data, header):
    if len(data) < header.metadata_size:
        raise RuntimeError('Metadata Size is greater than the metadata in the file.')

    metadata = data[:header.metadata_size]
    data = data[header.metadata_size:]
    return data, metadata

def extract_sign(data):
    data_len = len(data)
    sign = ''
    if data_len == 0:
        pass
    elif data_len < df.AOSTLSIGNSIZE:
        raise RuntimeError('Partial data of length ' + str(data_len) + ' detected in place of signature')
    else:
        sign = data[:df.AOSTLSIGNSIZE]
        data = data[df.AOSTLSIGNSIZE:]
    return data, sign

def extract_cert_chain(data):
    return '', data
