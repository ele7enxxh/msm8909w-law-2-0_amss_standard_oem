/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                                MCS PROFILING

GENERAL DESCRIPTION

  This file implements memory-only profiling to help diagnose performance
  problems in which MCS APIs are implicated


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mcsprofile/src/mcsprofile.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
01/27/2015   tl      Move MCS profiling into its own source file

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES FOR MODULE
=============================================================================*/

#include "mcsprofile.h"

/*=============================================================================
                   DEFINITIONS AND DECLARATIONS FOR MODULE
=============================================================================*/

#ifndef T_WINNT
#ifndef MCS_MARKER_USES_TTL

unsigned int mcs_profile_index = 0;
mcs_profile_array_type mcs_profile_array[MCS_PROFILE_COUNT];

#endif /* !MCS_MARKER_USES_TTL */
#endif /* !T_WINNT */
