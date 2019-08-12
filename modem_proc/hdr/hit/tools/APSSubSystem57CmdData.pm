#===========================================================================
#                    APSSubSystem57CmdData.pm
#  DESCRIPTION:
#     CmdData for APSSubSystem3
#
#
#  VERSION   DATE          WHO         DESCRIPTION
#  -------   ----------    --------    -----------
#  1.0.000                 vhegde      first version
#===========================================================================

# Use / Require
#-----------------
use strict;
use warnings;
no  warnings qw(once);

#===========================================================================
#                    Package APSSubSystem57CmdData
#  DESCRIPTION:
#     This package has cmddata for SubSystem 57 - HIT
#
#===========================================================================
package APSSubSystem57CmdData;
use AsiaAPSConstants;
use AsiaPerlConstants;
use MetadataConstants;
use APSCmdDataHIT;
use APSCmdDataHIThdr;
#add new test application before this line: use APSCmdDataHIT<TESTAPP>;


# contains the 2nd layer abstraction which is used in the scripts
our $CmdData =
[
   @${APSCmdDataHIT::CmdData},
   @${APSCmdDataHIThdr::CmdData},

#add new test application before this line: @${APSCmdDataHIT<TESTAPP>::CmdData},
];

our $RspData =
{
   (%${APSCmdDataHIT::RspData}),
   (%${APSCmdDataHIThdr::RspData}),

#add new test application before this line: @${APSCmdDataHIT<TESTAPP>::RspData},
};

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

1;
