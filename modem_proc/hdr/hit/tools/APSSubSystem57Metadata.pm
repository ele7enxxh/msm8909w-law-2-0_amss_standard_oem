#===========================================================================
#  APS Metadata record definitions template file for sub system 57 APPS.
#
#  VERSION   DATE       WHO         DESCRIPTION
#  1.0.000              vhegde      initial version
#===========================================================================
#===========================================================================
package APSSubSystem57Metadata;
use MetadataConstants;
use strict;
use warnings;
no  warnings qw(once);


# ALL THE LOG PACKETS ARE ADDED IN THIS ARRAY.
# FOLLOW THE METADATA SPECIFICATION TO DEFINE THESE
# LOG PACKETS, SUBRECORDS etc.
our @LOG_57 =
(
[
   ["LOG_HIT"],
],
);

#######################################
# add FTM request responses here
#######################################
our @CMD_57 =
(
[
   ["CMD_HIT"],
],
[#================== hit_generic_icd_hit_command ================================
   ['hit_generic_icd_hit_command'],
   [
      ['CMD_CODE',                  BYTE,    1,    1,    HIDDEN|READONLY,   128],
      ['SUBSYS_ID',                 BYTE,    1,    1,    HIDDEN|READONLY,   57 ],
      ['SUBSYS_CMD_CODE',           BYTE,    2,    1,    HIDDEN|READONLY,   0  ],
      ['REQUEST',                   PACKED,  1,    '0..'                       ] 
   ],
   ['hit_generic_icd_hit_command Response Message'],
   [
      ['CMD_CODE',                  BYTE,    1,    1,    HIDDEN                ],
      ['SUBSYS_ID',                 BYTE,    1,    1,    HIDDEN                ],
      ['SUBSYS_CMD_CODE',           BYTE,    2,    1,    HIDDEN                ],
      ['STATUS',                    BYTE,    4,    1                           ],
      ['DELAYED_RESPONSE_ID',       BYTE,    2,    1                           ],
      ['RESPONSE_COUNT',            BYTE,    2,    1                           ],
      ['RESPONSE',                  PACKED,  '0..',1                           ],
   ]
],
[#================== hit_generic_icd_hdr_command ================================
   ['hit_generic_icd_hdr_command'],
   [
      ['CMD_CODE',                  BYTE,    1,    1,    HIDDEN|READONLY,   128],
      ['SUBSYS_ID',                 BYTE,    1,    1,    HIDDEN|READONLY,   57 ],
      ['SUBSYS_CMD_CODE',           BYTE,    2,    1,    HIDDEN|READONLY,   1  ],
      ['REQUEST',                   PACKED,  1,    '0..'                       ] 
   ],
   ['hit_generic_icd_hdr_command Response Message'],
   [
      ['CMD_CODE',                  BYTE,    1,    1,    HIDDEN                ],
      ['SUBSYS_ID',                 BYTE,    1,    1,    HIDDEN                ],
      ['SUBSYS_CMD_CODE',           BYTE,    2,    1,    HIDDEN                ],
      ['STATUS',                    BYTE,    4,    1                           ],
      ['DELAYED_RESPONSE_ID',       BYTE,    2,    1                           ],
      ['RESPONSE_COUNT',            BYTE,    2,    1                           ],
      ['RESPONSE',                  PACKED,  '0..',1                           ],
   ]
],

#add new test application before this line
);

#==========================================================================
#
# QUALCOMM Proprietary
#
# Export of this software and/or technology may be controlled
# by the U.S. Government.  Diversion contrary to U.S. law
# prohibited.
#
# Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
#
# All data and information contained in or disclosed by this document
# is confidential and proprietary information of QUALCOMM Incorporated
# and all rights therein are expressly reserved.  By accepting this
# material the recipient agrees that this material and the information
# contained therein is held in confidence and in trust and will not be
# used, copied, reproduced in whole or in part, nor its contents
# revealed in any manner to others without the express written
# permission of QUALCOMM Incorporated.
#
# End of Module
#
# The following is the module terminator - this statement is required
#==========================================================================

# end of the package
1;
