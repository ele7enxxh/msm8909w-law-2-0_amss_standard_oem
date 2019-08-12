#ifndef SIMLOCK_RSU_H
#define SIMLOCK_RSU_H
/*===========================================================================


            S I M L O C K    R S U    H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/06/16   stv     Remote simlock support
05/15/15   vv      Initial revision
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#if defined (FEATURE_SIMLOCK) && (defined (FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK))

#include "simlock_common.h"
#include "simlock_modem.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_RSU_PROCESS_MESSAGE

DESCRIPTION
  This is the function which processes the RSU requests in the SIMLock.
  It is the entry point to the various requests in RSU feature.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_process_message(
  const simlock_rsu_msg_req_type          * msg_req_ptr,
  simlock_data_type                       * msg_resp_ptr
);

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */

#endif /* SIMLOCK_RSU_H */
