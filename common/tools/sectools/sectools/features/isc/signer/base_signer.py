#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

from abc import ABCMeta, abstractmethod
import hashlib

from sectools.common.core.plugin import SecPluginIntf_Abs
from sectools.common.crypto import crypto_functions
from sectools.common.utils.c_attribute import Attribute, BaseAttribute
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import TablePrinter
from sectools.features.isc.hasher import Hasher
from sectools.features.isc.signer import signerutils, signer_mgr
from sectools.features.isc.signer.signerutils.attributes import SigningAttributes
from sectools.features.isc.signer.signerutils.certificate import Certificate


DEBUG_DISABLED = 2

class SignerOutput(object):

    def __init__(self):
        self.signature = None
        self.cert_chain = None
        self.root_cert = None
        self.attestation_ca_cert = None
        self.attestation_cert = None
        self.root_key = None
        self.attestation_ca_key = None
        self.attestation_key = None
        self.root_cert_list = []


"""Internal exception classes
"""
class ConfigError(RuntimeError):
    pass
class ExternalSignerError(RuntimeError):
    pass


class BaseSigner(SecPluginIntf_Abs):
    __metaclass__ = ABCMeta

    MSG_INVALID_32_INTEGER = "{0} {1}:{2} is not a valid 32 bit integer"

    def __init__(self, config):
        '''
        Discover what the signing impl is going to be.
        '''
        self.config = config
        self.config_overrides = signerutils.get_signing_config_overrides()

    @classmethod
    def get_plugin_id(cls):
        return cls.signer_id()

    @classmethod
    def get_plugin_manager(cls):
        return signer_mgr

    def sign(self, binary_to_sign, imageinfo, debug_dir=None, is_hash=False):
        '''
        This function returns a SignerOutput object which has all the security assets generated
        by the signer.
        '''
        self.validate_config(imageinfo)
        sha_algo = signerutils.get_sha_algo_from_config(imageinfo.signing_attributes)

        if is_hash:
            hash_to_sign = binary_to_sign
            binary_to_sign = None
        else:
            hasher = Hasher()
            hmacParams = signerutils.get_hmac_params_from_config(imageinfo.signing_attributes)
            hash_to_sign = hasher.qcom_hmac(binary_to_sign, hmacParams, sha_algo)

        signer_output = self.sign_hash(hash_to_sign, imageinfo, binary_to_sign, debug_dir, sha_algo)

        # print certificate properties
        attestation_cert_obj = Certificate(signer_output.attestation_cert)
        logger.info('\nAttestation Certificate Properties:\n' + str(attestation_cert_obj))

        return signer_output

    def validate(self, image, root_cert_hash=None, imageinfo=None):
        if image.is_signed():
            # Extract the cert chain list
            cert_chain_blob = image.cert_chain
            cert_chain_der = crypto_functions.split_certificate_blob_into_certs(cert_chain_blob)

            # Create error string
            errstr = []

            # Root cert hash validation
            if not self.validate_sig(image.data_to_sign, image.data_signature, cert_chain_der):
                errstr.append('Signature is invalid')

            # OID Validation
            if not self.validate_oid_from_certs(cert_chain_der[1], cert_chain_der[0]):
                errstr.append('OID values in the certificate are invalid')

            # Root cert hash validation
            if root_cert_hash and not self.validate_root_cert_hash(cert_chain_der, root_cert_hash):
                errstr.append('Root certificate from image does not match the given root cert hash value')

            # Signing attributes vaidation
            if imageinfo is not None:
                mismatch = self.validate_signing_attributes(cert_chain_der, imageinfo)
                if mismatch:
                    tp = TablePrinter()
                    tp.insert_data(0, 0, 'Attribute')
                    tp.insert_data(0, 1, 'Attestation Cert')
                    tp.insert_data(0, 2, 'Config File')

                    for i, m in enumerate(mismatch):
                        tp.insert_data(i + 1, 0, m[0])
                        tp.insert_data(i + 1, 1, m[1])
                        tp.insert_data(i + 1, 2, m[2])

                    errstr.append('Following signing attributes do not match: \n          ' +
                                  '\n          '.join(tp.get_data().split('\n')))

            if errstr:
                raise RuntimeError('Following validations failed for the image:\n       ' +
                                   '\n       '.join([(str(i + 1) + '. ' + e) for i, e in enumerate(errstr)]))
            return True
        else:
            raise RuntimeError("Image supplied is not signed.")

    def validate_signing_attributes(self, cert_chain_der, imageinfo):
        if not cert_chain_der:
            raise RuntimeError('Cert chain must contain at least 1 certificate.')
        attest_cert = cert_chain_der[0]
        attr_config = SigningAttributes()
        attr_config.update_from_image_info_attrs(imageinfo.signing_attributes)
        attr_attest = SigningAttributes()
        attr_attest.update_from_attest_cert(attest_cert)
        return attr_attest.compare(attr_config, attr_attest.EXCLUDE_NON_ATTEST)

    @abstractmethod
    def sign_hash(self, hash_to_sign, imageinfo, binary_to_sign=None, debug_dir=None, sha_algo=None):
        raise NotImplementedError()

    @abstractmethod
    def validate_config(self, imageinfo):
        raise NotImplementedError()

    def _validate_oid_values(self, signing_attributes, general_properties, mandatory=True):
        if self._is_oid_supported(signing_attributes) is False:
            return

        oid_name = signing_attributes.object_id.name
        oid_min = signing_attributes.object_id.min
        oid_max = signing_attributes.object_id.max

        config_params_32bits = {
                      "min": oid_min,
                      "max": oid_max,
                      }

        for key in config_params_32bits.keys():
            valid = False
            if config_params_32bits[key] and \
                (Attribute.validate(num_bits=32, string=config_params_32bits[key]) is True):
                    valid = True
            elif (not config_params_32bits[key]) and \
                (mandatory is False):
                valid = True

            if valid == False:
                raise ConfigError(self.MSG_INVALID_32_INTEGER.
                                    format(oid_name,
                                           key, config_params_32bits[key]))

        if oid_min and oid_max and \
            (Attribute.init(num_bits=32, string=oid_min).value > \
            Attribute.init(num_bits=32, string=oid_max).value):
            valid = False
            raise ConfigError('{0} min must be less than max, min={1} max={2}' \
                                .format(oid_name, oid_min, oid_max))

    def _validate_attributes_with_oid_rule(self, attest_cert_obj):
        isValid = True
        # Enforce TCG rules
        attributes_zero_list = [
                      Certificate.SIGNATTR_SW_ID,
                      Certificate.SIGNATTR_HW_ID,
                      Certificate.SIGNATTR_OEM_ID,
                      Certificate.SIGNATTR_MODEL_ID,
                      ]

        attributes_none_list = [
                      Certificate.SIGNATTR_APP_ID,
                      Certificate.SIGNATTR_CRASH_DUMP,
                      Certificate.SIGNATTR_ROT_EN,
                      ]

        if attest_cert_obj.tcg_min and attest_cert_obj.tcg_max:
            # Only enforce TCG rules currently
            for attr_name in attributes_zero_list:
                attr = attest_cert_obj.get_attr(attr_name)
                if attr.value != 0:
                    logger.debug("{0} should be 0 under TCG validation rules. Current value is {1}".\
                                 format(attr_name, attr.str))
                    isValid = False

            for attr_name in attributes_none_list:
                attr = attest_cert_obj.get_attr(attr_name)
                if attr != None:
                    logger.debug("{0} should be None under TCG validation rules. Current value is {1}".\
                                 format(attr_name, attr.str))
                    isValid = False

            attr = attest_cert_obj.get_attr(Certificate.SIGNATTR_DEBUG)
            if attr is not None and attr.value != DEBUG_DISABLED:
                logger.debug("{0} should be 2 under TCG validation rules. Current value is {1}".\
                             format(Certificate.SIGNATTR_DEBUG, attr.str))
                isValid = False

        return isValid

    def validate_oid_from_certs(self, ca_cert_der, attest_cert_der):

        attest_cert_obj = Certificate(attest_cert_der)
        ca_cert_obj = Certificate(ca_cert_der)



        isValid = self._validate_attributes_with_oid_rule(attest_cert_obj) and \
                    self._validate_oid_raw(attest_cert_obj.tcg_min,
                                     attest_cert_obj.tcg_max,
                                     ca_cert_obj.tcg_min,
                                     ca_cert_obj.tcg_max) and \
                    self._validate_oid_raw(attest_cert_obj.fid_min,
                                     attest_cert_obj.fid_max,
                                     ca_cert_obj.fid_min,
                                     ca_cert_obj.fid_max)

        return isValid

    def validate_oid_from_config(self, ca_cert_path, signing_attributes):
        ca_cert_obj = Certificate(path=ca_cert_path)

        min_str = signing_attributes.object_id.min
        max_str = signing_attributes.object_id.max

        min_attest = Attribute.init(num_bits=32, string=min_str)
        max_attest = Attribute.init(num_bits=32, string=max_str)

        if signing_attributes.object_id.name == "tcg":
            min_ca = ca_cert_obj.tcg_min
            max_ca = ca_cert_obj.tcg_max
        elif signing_attributes.object_id.name == "feature_id":
            min_ca = ca_cert_obj.fid_min
            max_ca = ca_cert_obj.fid_max

        return self._validate_oid_raw(min_attest,
                                     max_attest,
                                     min_ca,
                                     max_ca)

    def _validate_oid_raw(self, min_attest, max_attest,
                                min_ca, max_ca):
        tcg_ok = False

        if (self.config.general_properties.num_certs_in_certchain == 2) and \
            (self.config.general_properties.object_id.name == "opendsp"):
            logger.error("2-level certificate chain is not supported for opendsp signature")
            return False

        if min_attest is not None:
            assert(isinstance(min_attest, BaseAttribute))
        if max_attest is not None:
            assert(isinstance(max_attest, BaseAttribute))
        if min_ca is not None:
            assert(isinstance(min_ca, BaseAttribute))
        if max_ca is not None:
            assert(isinstance(max_ca, BaseAttribute))

        if ((min_attest is None) and (max_attest is None) and
            (min_ca is None) and (max_ca is None)):
            # This is ok. No TCGs in attest cert.
            tcg_ok = True
            logger.debug("\nNo TCGs found in Attestation cert or CA cert. This is OK.")
        elif ((min_attest is not None) and (max_attest is not None) and
            (min_ca is None) and (max_ca is None)):
            logger.error("\nTCGs found in Attestation cert, but not in CA cert. This is invalid.")
        elif ((min_attest is None) and (max_attest is None) and
              (min_ca is not None) and (max_ca is not None)):
            logger.error("\nNo TCGs found in Attestation cert, but there are TCGs in CA cert. This is invalid.")
        elif ((min_attest is not None) and (max_attest is not None) and
              (min_ca is not None) and (max_ca is not None)):
            if (min_ca.value <= min_attest.value <=
                  max_attest.value <= max_ca.value):
                tcg_ok = True
                logger.debug("\nTCG values fall within CA constraints.")
            else:
                logger.error("\nTCG values are outside the CA constraints.")
        else:
            logger.error("\nInvalid TCG values")

        tcg_log_mesg = "\nAttestation cert : tcg_min={0} tcg_max={1}". \
                                format(min_attest, max_attest) + \
                        "\nCA cert (allowed): tcg_min={0} tcg_max={1}\n". \
                                format(min_ca, max_ca)
        if (tcg_ok is False):
            logger.error(tcg_log_mesg)
        else:
            logger.debug(tcg_log_mesg)

        return tcg_ok

    def validate_sig(self, to_sign, signature, cert_chain_der):
        # Check that openssl is available
        try:
            crypto_functions.are_available([crypto_functions.MOD_OPENSSL])
        except Exception as e:
            raise RuntimeError('Cannot validate signing: ' + str(e))

        hmac_params = crypto_functions.get_hmacparams_from_certificate_chain(cert_chain_der[0])
        hash_algo = crypto_functions.get_hash_algorithm_from_certicate_chain(cert_chain_der[0])

        cert_chain_pem = []
        for cert in cert_chain_der:
            cert_chain_pem.append(crypto_functions.cert_der_to_pem(cert))

        public_key = crypto_functions.get_public_key_from_cert_chain(cert_chain_pem)
        decrypted_hash = crypto_functions.decrypt_with_public_key(signature, public_key)

        hasher = Hasher()
        image_hash = hasher.qcom_hmac(to_sign, hmac_params, hash_algo)

        return image_hash == decrypted_hash

    def validate_root_cert_hash(self, cert_chain_der, expected_root_cert_hash):
        if len(cert_chain_der) != 3:
            raise RuntimeError('Cert chain must contain 3 certificates.')
        root_cert = cert_chain_der[2].rstrip(chr(0xff))
        hash_obj = hashlib.sha256()
        hash_obj.update(root_cert)
        root_cert_hash = hash_obj.hexdigest()
        return root_cert_hash == expected_root_cert_hash

    def _is_oid_supported(self, signing_attributes):
        isSupported = False
        if signing_attributes.object_id:
            isSupported = True

        return isSupported

    # This routine only supports 2-level and 3-level cert chain
    def _get_signer_output(self, signature, cert_chain_list):
        signer_output = SignerOutput()
        signer_output.attestation_cert = cert_chain_list[0]
        if len(cert_chain_list) == 3:
            signer_output.attestation_ca_cert = cert_chain_list[1]
            signer_output.root_cert = cert_chain_list[2]
        elif len(cert_chain_list) == 2:
            signer_output.root_cert = cert_chain_list[2]
        else:
            raise RuntimeError(
                    "Only 2-level or 3-level cert chain is supported. Number of certificates found = {0}\n".
                                format(len(cert_chain_list)))

        cert_chain = crypto_functions.create_certificate_chain(cert_chain_list)

        signer_output.signature = signature
        signer_output.cert_chain = cert_chain

        return signer_output
