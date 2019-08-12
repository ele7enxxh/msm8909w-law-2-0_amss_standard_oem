#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

#------------------------------------------------------------------------------
# AOST LIC Header
#------------------------------------------------------------------------------
# magic values
AOSTLMAG        = 'QCLF'

AOSTLMAG_STRING = 'Magic'
AOSTLMAG_DESCRIPTION = \
{
    AOSTLMAG      : AOSTLMAG,
}

# version values
AOSTLVERONE     = 1

AOSTLVER_STRING = 'Version'
AOSTLVER_DESCRIPTION = \
{
    AOSTLVERONE : '1',
}

# type values
AOSTLTYPECOMM   = 0x0001
AOSTLTYPEEVAL   = 0xFFFF

AOSTLTYPE_STRING  = 'License Type'
AOSTLTYPE_DESCRIPTION = \
{
    AOSTLTYPECOMM : 'Commercial',
    AOSTLTYPEEVAL : 'Evaluation',
}

# oem_id max
AOSTLMAXOEMID = 0xFFFF

# metadata_size
AOSTLMAXMETASIZE = 0x0800

# signature_size
AOSTLSIGNSIZE = 0x100

#------------------------------------------------------------------------------
# File names for debugging
#------------------------------------------------------------------------------
FILE_DATA_IN                        = 'aost_lic_in_0_data'

FILE_HDR_REMOVED                    = 'aost_lic_in_1_hdr_removed'
FILE_HDR_IN                         = 'aost_lic_in_1_hdr'
FILE_HDR_IN_REPR                    = 'aost_lic_in_1_hdr_repr'
FILE_HDR_IN_REPR_SUFFIX             = '.txt'

FILE_METADATA_IN                    = 'aost_lic_in_2_metadata'
FILE_SIGN_IN                        = 'aost_lic_in_3_sign'
FILE_CERT_CHAIN_IN                  = 'aost_lic_in_4_cert_chain'
