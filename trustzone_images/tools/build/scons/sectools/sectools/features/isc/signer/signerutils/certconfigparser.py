#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

from abc import ABCMeta, abstractmethod
from sectools.common.crypto import utility_functions
import os
import sectools.features.isc.signer.signerutils as signerutils

class BaseCertConfigParser(object):
    __metaclass__=ABCMeta

    def __init__(self, cert_config, signing_attributes, general_properties):
        self.cert_config = cert_config
        self.signing_attributes = signing_attributes
        self.general_properties = general_properties

    @abstractmethod
    def get_crypto_params(self):
        raise NotImplementedError()

    def _certificate_properties_parse(self, cert_info):
        if cert_info.params_list:
            params = utility_functions.normalize_param_list_into_dict(cert_info.params_list.cert_param)
        else:
            params = {}
            cert_path_exponent_replaced = signerutils.macro_replace(cert_info.preexisting_cert.cert_path,
                                                             "exponent",
                                                             str(self.signing_attributes.exponent))
            params['certificate_path'] = signerutils.macro_replace(cert_path_exponent_replaced,
                                                             "key_size",
                                                             str(self.general_properties.key_size))

            private_key_exponent_replaced = signerutils.macro_replace(cert_info.preexisting_cert.private_key_path,
                                                             "exponent",
                                                             str(self.signing_attributes.exponent))
            params['private_key_path'] = signerutils.macro_replace(private_key_exponent_replaced,
                                                             "key_size",
                                                             str(self.general_properties.key_size))

        return params


class CertConfigParser(object):
    def __init__(self, cert_config, signing_attributes, general_properties):
        '''
        Constructor
        '''
        if cert_config.multirootcert:
            self.cert_config_parser = MultiCertConfigParser(cert_config,
                                                            signing_attributes,
                                                            general_properties)
        else:
            self.cert_config_parser = GenericCertConfigParser(cert_config,
                                                              signing_attributes,
                                                              general_properties)


    def get_crypto_params(self):
        return self.cert_config_parser.get_crypto_params()


class MultiCertConfigParser(BaseCertConfigParser):
    def __init__(self, cert_config, signing_attributes, general_properties):
        BaseCertConfigParser.__init__(self, cert_config,
                                      signing_attributes,
                                      general_properties)

    def _multirootcert_properties_parse(self, multirootcert_config, config_cert_name, config_key_name):
        params = {}
        cert_name = signerutils.macro_replace(config_cert_name, "index",
                                        str(multirootcert_config.index),
                                        isMandatory=True)
        key_name = signerutils.macro_replace(config_key_name, "index",
                                        str(multirootcert_config.index),
                                        isMandatory=True)
        params['certificate_path'] = signerutils.macro_replace(
                                     os.path.join(multirootcert_config.directory, cert_name),
                                     "exponent",
                                     str(self.signing_attributes.exponent))
        params['private_key_path'] = signerutils.macro_replace(
                                    os.path.join(multirootcert_config.directory, key_name),
                                    "exponent",
                                    str(self.signing_attributes.exponent))

        return params

    def get_crypto_params(self):
        crypto_params_dict = {}

        crypto_params_dict['root_certificate_properties'] = \
                self._multirootcert_properties_parse(self.cert_config.multirootcert,
                                                     self.cert_config.multirootcert.root_cert_name,
                                                     self.cert_config.multirootcert.root_key_name)
        crypto_params_dict['attest_ca_certificate_properties'] = \
                self._multirootcert_properties_parse(self.cert_config.multirootcert,
                                                     self.cert_config.multirootcert.attest_ca_cert_name,
                                                     self.cert_config.multirootcert.attest_ca_key_name)
        crypto_params_dict['attest_certificate_properties'] = self._certificate_properties_parse(self.cert_config.attest_cert)

        return crypto_params_dict

class GenericCertConfigParser(BaseCertConfigParser):
    def __init__(self, cert_config, signing_attributes, general_properties):
        BaseCertConfigParser.__init__(self, cert_config,
                                      signing_attributes,
                                      general_properties)

    def get_crypto_params(self):
        crypto_params_dict = {}

        crypto_params_dict['root_certificate_properties'] = \
                self._certificate_properties_parse(self.cert_config.root_cert)
        if self.cert_config.attest_ca_cert != None:
            crypto_params_dict['attest_ca_certificate_properties'] = \
                self._certificate_properties_parse(self.cert_config.attest_ca_cert)
        crypto_params_dict['attest_certificate_properties'] = \
                self._certificate_properties_parse(self.cert_config.attest_cert)

        return crypto_params_dict