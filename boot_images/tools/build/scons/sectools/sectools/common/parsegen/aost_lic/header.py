#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================


import defines as df
from sectools.common.utils.c_misc import properties_repr
from sectools.common.utils.struct_base import StructBase


class AOSTLicHdr(StructBase):

    def _unpack_data_list(self, unpacked):
        self._magic_number = unpacked[0]
        self._version_number = unpacked[1]
        self._feature_id = unpacked[2]
        self._oem_id = unpacked[3]
        self._chip_id = unpacked[4]
        self._product_id = unpacked[5]
        self._license_type = unpacked[6]
        self._evaluation_use_count = unpacked[7]
        self._metadata_size = unpacked[8]

    def _pack_data_list(self):
        return [self.magic_number,
                self.version_number,
                self.feature_id,
                self.oem_id,
                self.chip_id,
                self.product_id,
                self.license_type,
                self.evaluation_use_count,
                self.metadata_size,]

    @classmethod
    def get_format(cls):
        return ('>4sIIHIHHHH')

    def validate(self):
        # Check that the magic is correct
        if self.magic_number not in df.AOSTLMAG_DESCRIPTION.keys():
            raise RuntimeError('Invalid magic in the AOST license file: ' + self.magic_number + '\n'
                               'This is not a license file.')

        # Check that the version is correct
        if self.version_number not in df.AOSTLVER_DESCRIPTION.keys():
            raise RuntimeError('Unsupported version in the AOST license file: ' + hex(self.version_number))

        # Check that the license type is correct
        if self.license_type not in df.AOSTLTYPE_DESCRIPTION.keys():
            raise RuntimeError('Invalid license type in the AOST license file: ' + hex(self.license_type))

        # Check that the metadata size is in range
        if self.metadata_size > df.AOSTLMAXMETASIZE:
            raise RuntimeError('Metadata size is out of range in the AOST license file: ' + hex(self.metadata_size))

    def _repr_properties(self):
        properties = [
                      (df.AOSTLMAG_STRING, df.AOSTLMAG_DESCRIPTION, self.magic_number),
                      (df.AOSTLVER_STRING, df.AOSTLVER_DESCRIPTION, self.version_number),
                      ('Feature ID', {}, self.feature_id),
                      ('OEM ID', {}, hex(self.oem_id)),
                      ('Chip ID', {}, hex(self.chip_id)),
                      ('Product ID', {}, hex(self.product_id)),
                      (df.AOSTLTYPE_STRING, df.AOSTLTYPE_DESCRIPTION, self.license_type),
                      ('Evaluation Use Count', {}, self.evaluation_use_count),
                      ('Metadata Size', {}, self.metadata_size),
                     ]
        return [(attr, desc.get(val, val)) for attr, desc, val in properties]

    def __repr__(self):
        return properties_repr(self._repr_properties())

    def _range_check(self, value, bcnt, tag):
        max_val = int('0x' + 'FF'*bcnt, 16)
        if value < 0 or value > max_val:
            raise RuntimeError(tag + ' value: ' + value + ' is out of range: 0x0-' + hex(max_val))

    @property
    def magic_number(self):
        return self._magic_number

    @magic_number.setter
    def magic_number(self, value):
        if value not in df.AOSTLMAG_DESCRIPTION.keys():
            raise RuntimeError('Invalid magic: ' + value)
        self._magic_number = value

    @property
    def version_number(self):
        return self._version_number

    @version_number.setter
    def version_number(self, value):
        if value not in df.AOSTLVER_DESCRIPTION.keys():
            raise RuntimeError('Unsupported version: ' + hex(value))
        self._version_number = value

    @property
    def feature_id(self):
        return self._feature_id

    @feature_id.setter
    def feature_id(self, value):
        self._range_check(value, 4, 'feature_id')
        self._feature_id = value

    @property
    def oem_id(self):
        return self._oem_id

    @oem_id.setter
    def oem_id(self, value):
        self._range_check(value, 2, 'oem_id')
        self._oem_id = value

    @property
    def chip_id(self):
        return self._chip_id

    @chip_id.setter
    def chip_id(self, value):
        if value is not None:
            self._range_check(value, 4, 'chip_id')
        else:
            value = 0
        self._chip_id = value

    @property
    def product_id(self):
        return self._product_id

    @product_id.setter
    def product_id(self, value):
        if value is not None:
            self._range_check(value, 2, 'product_id')
        else:
            value = 0
        self._product_id = value

    @property
    def license_type(self):
        return self._license_type

    @license_type.setter
    def license_type(self, value):
        if value not in df.AOSTLTYPE_DESCRIPTION.keys():
            raise RuntimeError('Invalid license type: ' + hex(value))
        self._license_type = value

    @property
    def evaluation_use_count(self):
        return self._evaluation_use_count

    @evaluation_use_count.setter
    def evaluation_use_count(self, value):
        if value is not None:
            self._range_check(value, 2, 'evaluation_use_count')
        else:
            value = 0
        self._evaluation_use_count = value

    @property
    def metadata_size(self):
        return self._metadata_size

    @metadata_size.setter
    def metadata_size(self, value):
        if value > df.AOSTLMAXMETASIZE:
            raise RuntimeError('Metadata size is out of range: ' + hex(value))
        self._metadata_size = value


def extract_header(data):
    header = AOSTLicHdr(data)
    data = data[header.size:]
    return data, header

def create_empty_header():
    header = AOSTLicHdr()
    header.magic_number = df.AOSTLMAG
    header.version_number = df.AOSTLVERONE
    header.license_type = df.AOSTLTYPECOMM
    return header
