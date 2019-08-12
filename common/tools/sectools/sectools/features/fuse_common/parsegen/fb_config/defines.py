#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

"""Contains constants related to the cfgparser package.

Constants relate to:

#. Common constants
#. Auto-generated FB XML Parser - auto_gen_xml_config
"""


from sectools.features.fuse_common.datamodel import defines as dm_def


#------------------------------------------------------------------------------
# Config Data Model Structure
#------------------------------------------------------------------------------
OBJ_STRUCTURE = \
{
    'metadata' :
    {
        'chipset' : '',
        'version' : '',
    },

    'module' : dm_def.OBJ_STRUCTURE['module'],
}


ENTRIES_STRUCTURE = {
                        'metadata' :
                        {
                            'chipset' : '',
                            'version' : '',
                        },

                        'secdat' :
                        {
                            'file_info' : '',
                            'file_version' : (0,),
                            'fuse_version' : (0,),
                        },

                        'module' :
                        {
                            'entry':
                            [
                                {
                                    'description':'',
                                    'name' : '',
                                    'value' : '',
                                    'ignore' : (True,),
                                }
                            ]
                        }
                    }

# FEC bits are 7 bits long, mask 0x7F : 0111 1111
FEC_BIT_MASK=0x7F

# FEC DATA region bit mask, 32 lower bits, mask 0xFFFFFFFF
FEC_DATA_32BIT_MASK=0xFFFFFFFF

# FEC DATA region bit mask, 56 lower bits, mask 0xFFFFFFFFFFFFFF
FEC_DATA_56BIT_MASK=0xFFFFFFFFFFFFFF

# FEC DATA region bit mask, 64 lower bits, mask 0xFFFFFFFFFFFFFFFF
FEC_DATA_64BIT_MASK=0xFFFFFFFFFFFFFFFF

# chipsets that have FEC_EN bit in row instead of FEC_EN region
FEC_IN_ROW = ['8016','8916','8039','8939','8909','8929','8952','401x','8909W']

# chipsets that have FEC_EN bit in row instead of FEC_EN region
DIFF_FEC_REG = ['8976']

# chipsets that support ROT feature
ROT_SUPPORTED = ['8992','8994','8996']



