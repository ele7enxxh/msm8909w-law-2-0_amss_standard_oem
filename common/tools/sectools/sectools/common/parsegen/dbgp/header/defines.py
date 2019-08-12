#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

#------------------------------------------------------------------------------
# Debug Policy Header
#------------------------------------------------------------------------------
# magic values
DBGPMAG                  = 'DBGP'

DBGPMAG_STRING           = 'Magic'
DBGPMAG_DESCRIPTION      = \
{
            DBGPMAG      : DBGPMAG,
}

# version values
DBGPVERONE               = 1
DBGPVERTWO               = 2

DBGPVER_STRING           = 'Version'
DBGPVER_DESCRIPTION      = \
{
            DBGPVERONE   : '1',
            DBGPVERTWO   : '2',
}

# constants
IMAGEBIT_MAP_MAX_VALUE         = 0xFFFFFFFF
SERNUM_START_END_MATCH  = 1

#------------------------------------------------------------------------------
# File names for debugging
#------------------------------------------------------------------------------
FILE_DATA_IN                    = 'dbgp_in_data'
FILE_ROOTCERTHASH_IN            = 'dbgp_in_rootcerthashdata'