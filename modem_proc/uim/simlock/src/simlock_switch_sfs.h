#ifndef SIMLOCK_SWITCH_SFS_H
#define SIMLOCK_SWITCH_SFS_H
/*===========================================================================


            S I M L O C K   S W I T C H   S F S   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_switch_sfs.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/29/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_common.h"
#include "simlock_modem.h"
#include "simlock_config.h"


/*===========================================================================
FUNCTION SIMLOCK_SET_REMOTE_SFS_REQUEST

DESCRIPTION
  This function processes the request to enable or disable the remote SFS
  storage

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_remote_sfs_request
(
  const simlock_set_remote_sfs_msg_req_type * msg_request_type_ptr
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_SWITCH_SFS_H */
