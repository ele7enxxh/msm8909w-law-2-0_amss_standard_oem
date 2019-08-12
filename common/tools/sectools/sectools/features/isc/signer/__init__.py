#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

from sectools.common.core.plugin import SecPluginMgr


# API to get a signer
def get_signer(config):
    signer = signer_mgr.get_signer(config.general_properties.selected_signer)
    return signer(config)


class SignerMgr(SecPluginMgr):

    def get_signer(self, signer_id):
        signers = self.get_map()
        if signer_id not in signers:
            raise RuntimeError('Signer unavailable for signer id: ' + str(signer_id) + '\n'
                               '    ' + 'Available signer ids are: ' + str(signers.keys()))
        return signers[signer_id]

signer_mgr = SignerMgr()


from sectools.features.isc.signer import cass_signer
from sectools.features.isc.signer import csms_signer
from sectools.features.isc.signer import openssl_signer
from sectools.features.isc.signer import secimage_remote_client
from sectools.features.isc.signer import secimage_remote_server