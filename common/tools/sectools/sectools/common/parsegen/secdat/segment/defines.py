#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================
'''
Created on Apr 7, 2014

@author: hraghav
'''

from sectools.common.parsegen.secdat.segment.type_fuse.format import ParseGenSegmentFuse


# Segment Version Values
SEG_VER1                    = 1
SEG_VER2                    = 2

# Segment Type Values
SEG_TYPE_EFUSE              = 0
SEG_TYPE_KEYPROV            = 1
SEG_TYPE_ROT                = 2

SEG_TYPE_DESCRIPTION = \
{
        SEG_TYPE_EFUSE      : 'Efuse Segment',
        SEG_TYPE_KEYPROV    : 'Keyprovision Segment',
        SEG_TYPE_ROT        : 'Root Of Trust Segment',
}

SEG_TYPE_CLASS = \
{
        SEG_TYPE_EFUSE      : ParseGenSegmentFuse,
        SEG_TYPE_KEYPROV    : ParseGenSegmentFuse,
        SEG_TYPE_ROT        : ParseGenSegmentFuse,
}
