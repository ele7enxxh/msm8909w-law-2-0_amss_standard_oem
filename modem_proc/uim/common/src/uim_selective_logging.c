/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             U I M    S E L E C T I V E    L O G G I N G


GENERAL DESCRIPTION

  This source file contains the selective logging functionality

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/common/src/uim_selective_logging.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_selective_logging.h"


/*===========================================================================

                                      GLOBAL VARIABLES

===========================================================================*/

uint8   uim_selective_logging[UIM_MAX_ITEM_IN_SELECT_LOG] = {0};


/*===========================================================================
   FUNCTION:      UIM_SELECTIVE_LOGGING_INIT

   DESCRIPTION:
     This function initialise the selective logging
     functionality by reading NV item 73880

   DEPENDENCIES:
     None

   LIMITATIONS:
     None


   RETURN VALUE:
    Void

   SIDE EFFECTS:
     None

=============================================================================*/
void uim_selective_logging_init
(
  void
)
{
  /* Check UIM PSM NV is enabled on not */
  (void)uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_SELECTIVE_LOGGING,
                            UIM_COMMON_EFS_ITEM_FILE_TYPE,
                            UIM_COMMON_EFS_DEVICE,
                            (uint8 *)uim_selective_logging,
                            sizeof(uim_selective_logging));
} /* uim_selective_logging_init */
