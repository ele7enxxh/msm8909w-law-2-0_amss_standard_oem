#ifndef SIMLOCK_SFS_REMOTE_H
#define SIMLOCK_SFS_REMOTE_H
/*===========================================================================


            S I M L O C K   S F S   R E M O T E   H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_sfs_remote.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

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
#include "simlock_modem_p.h"
#include "simlock_modem.h"


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_DEREGISTER_REMOTE_CB

DESCRIPTION
  This function deregisters the callback used in remote SFS operations

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_sfs_remote_deregister_remote_cb
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_REGISTER_REMOTE_CB

DESCRIPTION
  This function registers the callback used in remote SFS operations

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_sfs_remote_register_remote_cb
(
  simlock_remote_sfs_request_cb_type     remote_cb
);

/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_READ_FILE

DESCRIPTION
  This function reads the data from the requested file by calling the remote
  SFS function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_remote_read_file(
  simlock_file_type                     file,
  simlock_data_type                   * data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_WRITE_FILE

DESCRIPTION
  This function writes the data to the requested file by calling the remote
  SFS function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_remote_write_file(
  simlock_file_type                     file,
  simlock_data_type                   * data_ptr
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_SFS_REMOTE_H */

