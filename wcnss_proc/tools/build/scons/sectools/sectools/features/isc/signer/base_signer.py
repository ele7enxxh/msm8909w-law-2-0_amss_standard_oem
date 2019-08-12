#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

from abc import ABCMeta, abstractmethod
from sectools.common.crypto import crypto_functions
from sectools.features.isc.hasher import Hasher

class SignerOutput(object):

    def __init__(self):
        self.signature = None
        self.cert_chain = None
        self.root_cert = None
        self.attestation_ca_cert = None
        self.attestation_cert = None
        self.attestation_key = None
        self.root_cert_list = []

class BaseSigner(object):
    __metaclass__=ABCMeta

    @abstractmethod
    def sign(self, hash_to_sign, imageinfo, binary_to_sign = None):
        raise NotImplementedError()

    def validate_sig(self, to_sign, signature, cert_chain_der):
        hmac_params=crypto_functions.get_hmacparams_from_certificate_chain(cert_chain_der[0])
        cert_chain_pem = []
        for cert in cert_chain_der:
            cert_chain_pem.append(crypto_functions.cert_der_to_pem(cert))

        public_key = crypto_functions.get_public_key_from_cert_chain(cert_chain_pem)
        decrypted_hash =  crypto_functions.decrypt_with_public_key(signature, public_key)

        hasher = Hasher()
        image_hash = hasher.qcom_hmac(to_sign, hmac_params)

        return image_hash == decrypted_hash

    def validate(self, image):
        if image.is_signed():
            cert_chain_blob = image.cert_chain

            cert_chain_der = crypto_functions.split_certificate_blob_into_certs(cert_chain_blob)
            return self.validate_sig(image.data_to_sign,
                                     image.data_signature,
                                     cert_chain_der)
        else:
            raise RuntimeError("Image supplied is not signed.")

    #This routine only supports 2-level and 3-level cert chain
    def _get_signer_output(self, signature, cert_chain_list):
        signer_output = SignerOutput()
        signer_output.attestation_cert = cert_chain_list[0]
        if len(cert_chain_list)==3:
            signer_output.attestation_ca_cert = cert_chain_list[1]
            signer_output.root_cert = cert_chain_list[2]
        elif len(cert_chain_list)==2:
            signer_output.root_cert = cert_chain_list[2]
        else:
            raise RuntimeError(
                    "Only 2-level or 3-level cert chain is supported. Number of certificates found = {0}\n".
                                format(len(cert_chain_list)))

        cert_chain=crypto_functions.create_certificate_chain(cert_chain_list)

        signer_output.signature = signature
        signer_output.cert_chain = cert_chain

        return signer_output
