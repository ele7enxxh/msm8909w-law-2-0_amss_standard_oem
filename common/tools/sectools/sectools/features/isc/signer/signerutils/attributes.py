#===============================================================================
#
# Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

import json
import os
import re

from sectools.common.crypto import crypto_functions
from sectools.common.crypto.utility_functions import store_data_to_temp_file
from sectools.features.isc.signer.signerutils import get_hmac_params_from_config


class Certificate(object):

    def __init__(self):
        pass

    @classmethod
    def get_text(cls, cert_data):
        cert_path = store_data_to_temp_file(cert_data)
        try:
            retval = crypto_functions.get_der_certificate_text(cert_path)
        finally:
            os.remove(cert_path)
        return retval

    @classmethod
    def get_subject_list_from_text(cls, text):
        match = re.search('Subject: (.*?)\n', text, re.IGNORECASE)
        if match is None:
            raise RuntimeError('Could not get the SUBJECT field from the attestation certificate')
        subject = match.group(1)
        return [s.strip().split('=') for s in subject.lower().split(',')]

    @classmethod
    def get_exponent_from_text(cls, text):
        match = re.search('Exponent: (\d+) \(.*?\)\n', text, re.IGNORECASE)
        if match is None:
            raise RuntimeError('Could not get the EXPONENT field from the attestation certificate')
        exponent = match.group(1)
        return int(exponent, 10)


class AttestationCertificate(Certificate):

    def __init__(self, cert_data):
        self.cert_data = cert_data
        self.cert_text = self.get_text(cert_data)
        self.subject_list = self.get_subject_list_from_text(self.cert_text)
        subject_dict, ou_dict = self.process_subject_list(self.subject_list)
        self.subject_dict = subject_dict
        self.subject_dict['exponent'] = self.get_exponent_from_text(self.cert_text)
        self.ou_dict = ou_dict

    def process_ou_field(self, ou_str):
        match = re.match('([^ ]+) ([^ ]+) ([^ ]+)', ou_str)
        if match is None:
            raise RuntimeError('OU field OU=' + ou_str + ' is incorrectly formatted')
        ou_number = match.group(1)
        ou_value = match.group(2)
        ou_id = match.group(3)
        return ou_number, ou_value, ou_id

    def process_subject_list(self, subject_list):
        subject_dict = {}
        ou_dict = {}

        for k, v in subject_list:
            # Update OU fields
            if k == 'ou':

                # Get the OU decomposition
                ou_number, ou_value, ou_id = self.process_ou_field(v)

                # Update the OU dictionary
                ou_dict_value = {
                                    'number' : ou_number,
                                    'value'  : ou_value,
                                }
                ou_dict[ou_id] = ou_dict_value

                # Update the key and value
                k = ou_id
                v = ou_value

            # Update the sha value
            if k.startswith('sha'):
                sha_mapping = {
                                'sha1'  : '0000',
                                'sha256': '0001',
                              }
                if (k, v) not in sha_mapping.items():
                    raise RuntimeError('Invalid sha value: ' + k + ':' + v)
                v = k
                k = 'hash_algorithm'

            # Update the hex values
            elif k in ['oem_id', 'model_id', 'hw_id', 'sw_id', 'app_id',
                       'debug', 'crash_dump', 'rot_en', ]:
                v = '0x' + v

            # Update the boolean values
            elif k in ['in_use_soc_hw_version',
                       'use_serial_number_in_signing', ]:
                v = int(v)

            subject_dict[k] = v

        return subject_dict, ou_dict


class SigningAttributes(object):

    ATTR_STR_DICT = \
    {
        'exponent' : None,
        'hash_algorithm' : None,
        'msm_part' : None,
        'oem_id' : None,
        'model_id' : None,
        'hw_id' : None,
        'sw_id' : None,
        'app_id' : None,
        'debug' : None,
        'crash_dump' : None,
        'rot_en' : None,
        'in_use_soc_hw_version' : None,
        'use_serial_number_in_signing' : None,
    }

    EXCLUDE_NON_ATTEST = ['msm_part']
    OPTIONAL_BOOLS = ['in_use_soc_hw_version']

    def __init__(self):
        self.exponent = None
        self.hash_algorithm = None
        self.msm_part = None
        self.oem_id = None
        self.model_id = None
        self.hw_id = None
        self.sw_id = None
        self.app_id = None
        self.debug = None
        self.crash_dump = None
        self.rot_en = None
        self.in_use_soc_hw_version = None
        self.use_serial_number_in_signing = None

    def sanitize(self, tag, val):
        return val

    def update_from_attest_cert(self, cert_data):
        # Create the attestation cert object
        cert = AttestationCertificate(cert_data)

        # Update from the certificate
        for attr in self.ATTR_STR_DICT:
            val = cert.subject_dict.get(attr, None)
            val = self.sanitize(attr, val)
            setattr(self, attr, val)

    def update_from_image_info_attrs(self, attrs):
        def sanitize(attr, val):
            if attr == 'hash_algorithm' and val is None:
                val = 'sha256'
            elif attr == 'hw_id':
                val = '0x' + get_hmac_params_from_config(attrs).msm_id_str
            val = self.sanitize(attr, val)
            return val

        # Update from the imageinfo
        for attr in self.ATTR_STR_DICT:
            val = getattr(attrs, attr, None)
            val = sanitize(attr, val)
            setattr(self, attr, val)

    def update_image_info_attrs(self, attrs):
        # Update the imageinfo
        for attr in self.ATTR_STR_DICT:
            val = getattr(self, attr)
            setattr(attrs, attr, val)

    def update_from_json(self, json_data):
        json_attrs = json.loads(json_data)
        for attr in self.ATTR_STR_DICT:
            json_attr = json_attrs.get(attr, None)
            if isinstance(json_attr, unicode):
                json_attr = str(json_attr)
            setattr(self, attr, json_attr)

    def get_dict(self, exclude=[]):
        retval = {}
        for attr in self.ATTR_STR_DICT:
            if attr in exclude:
                continue
            retval[attr] = getattr(self, attr)
        return retval

    def get_json(self, exclude=[]):
        return json.dumps(self.get_dict(exclude))

    def compare(self, other, exclude=[]):
        mismatch = []
        for attr in self.ATTR_STR_DICT:
            if attr in exclude:
                continue
            value_self = getattr(self, attr)
            value_other = getattr(other, attr)
            def clean_value(attr, value):
                if isinstance(value, str):
                    value = value.lower()
                    if value.startswith('0x'):
                        value = hex(int(value, 16))
                if attr in self.OPTIONAL_BOOLS and value is None:
                    value = 0
                return value
            value_self = clean_value(attr, value_self)
            value_other = clean_value(attr, value_other)
            if value_self != value_other:
                mismatch.append((attr, value_self, value_other))
        return mismatch
