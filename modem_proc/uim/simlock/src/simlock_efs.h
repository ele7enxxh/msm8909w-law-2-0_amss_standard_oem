#ifndef SIMLOCK_EFS_H
#define SIMLOCK_EFS_H
/*===========================================================================


            S I M L O C K   E F S  F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_efs.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/15/15   vv      Support for RSU in simlock
12/11/14   vv      Perform efs_sync() before writing the data
09/09/14   vv      Perform efs_sync() after writing the data
06/20/14   vv      Added support for diag protection 
01/07/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "fs_public.h"
#include "simlock.h"

/*===========================================================================
FUNCTION SIMLOCK_EFS_GET_FILE_SIZE

DESCRIPTION
  This function retrieves the size of a file in EFS.

DEPENDENCIES
  NONE

RETURN VALUE
  simlock_result_enum_type:

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_efs_get_file_size(
  const char *   file_handle_ptr,
  uint32     *   file_size_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_EFS_MKDIR

DESCRIPTION 
  This function creates a directory in EFS.

DEPENDENCIES
  NONE

RETURN VALUE
  simlock_result_enum_type
 
SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_efs_mkdir(
  const char* file_handle_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_EFS_WRITE_FILE

DESCRIPTION
  This function writes the data to a file in EFS.

DEPENDENCIES
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_efs_write_file(
  const char *   file_handle_ptr,
  uint32         num_bytes,
  uint8 *        data_buffer_p
);

/*===========================================================================
FUNCTION SIMLOCK_EFS_READ_FILE

DESCRIPTION
  This function reads the data from a file in EFS.

DEPENDENCIES
  NONE

RETURN VALUE 
  simlock_result_enum_type 
  
SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_efs_read_file(
  const char *   file_handle_ptr,
  uint32         data_len,
  uint8      *   data_buffer_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_EFS_NAME_TEST

DESCRIPTION
  This function tests whether a directory or a file
  already exists within the specified file structure.

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_name_test
(
  const char * file_name_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_EFS_SYNC

DESCRIPTION
  This function performs an EFS sync operation, which commits the
  data to the filesystem.

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_sync
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_EFS_CHECK_EFS_STATUS

DESCRIPTION
  This function checks EFS status by performing certain EFS operations
  and finally an EFS sync operation, which commits the data to the
  filesystem. This is done to ensure that an EFS operation can be performed
  when it is needed.

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_check_efs_status
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_EFS_DELETE_FILE

DESCRIPTION
  This function deletes the file

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_delete_file
(
  const char * file_name_ptr
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_EFS_H */