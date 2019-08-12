#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

from ..hasher import Hasher
import signerutils

class Signer(object):
    '''
    Objects of this class will discover the signing implementations available on the system.
    The objects implement the sign method that call into the discovered impl's sign function, that
    performs the actual signing.

    Usage by client:

    signer=Signer(config)
    signer_output=signer.sign(to_sign, config.signing_config.attributes)

    '''

    def __init__(self, config):
        '''
        Discover what the signing impl is going to be.
        '''
        self.config=config
        self._signer_impl=self._discover_signingfunctionprovider(config.signing.selected_signer)
        #self._validator_impl=self._discover_validationfunctionprovider()    # This is a reference to the localopenssl impl as CASS cannot
                                                                            # perform validation at this time

    def sign(self, binary_to_sign, imageinfo):
        '''
        This function returns a SignerOutput object which has all the security assets generated
        by the signer.
        '''
        hasher = Hasher()
        hmacParams = signerutils.get_hmac_params_from_config(imageinfo.signing_attributes)
        hash_to_sign=hasher.qcom_hmac(binary_to_sign, hmacParams)
        return self._signer_impl.sign(hash_to_sign, imageinfo, binary_to_sign)

    def validate(self, image):
        return self._signer_impl.validate(image)

    def _discover_signingfunctionprovider(self,selected_signer):
        if selected_signer == "cass":
            import sectools.features.isc.signer.cass_signer as cass_signer
            return cass_signer.CassSigner(self.config)
        elif selected_signer == "csms":
            import CsmsSigner
            return CsmsSigner.CsmsSigner(self.config)
        else:
            import OpenSSLSigner
            return OpenSSLSigner.OpenSSLSigner(self.config)


# class Certificate(object):
#     '''
#     Certificate class
#     '''
#     def __init__(self):

