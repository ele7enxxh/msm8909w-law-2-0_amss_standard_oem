#ifndef SIMLOCK_FILE_H
#define SIMLOCK_FILE_H
/*===========================================================================


            S I M   L O C K   F I L E   H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_file.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/15   stv     Perform efs sync before calling client call back
05/15/15   vv      Support for RSU in simlock
12/17/14   vv      Relocate the simlock data files
09/29/14   vv      Added support for the remote SFS
09/09/14   vv      Perform efs_sync() after writing the data
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_SIMLOCK

#include "simlock.h"

/*=============================================================================

                        DATA DECLARATIONS

=============================================================================*/
#define SIMLOCK_FILE_PATH_LEN     50

typedef char  simlock_filename_type[SIMLOCK_FILE_PATH_LEN];

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_FILE_READ

DESCRIPTION
  This function reads the data from the requested file by calling the SFS or
  EFS based read functions

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_read(
  uint8                       ** data_buf_pptr,
  uint32                       * data_buf_len_ptr,
  simlock_data_file_enum_type    file_enum,
  simlock_sfs_enum_type          sfs_type
);

/*===========================================================================
FUNCTION SIMLOCK_FILE_WRITE

DESCRIPTION
  This function writes the data to the requested file by calling the SFS or
  EFS based write functions

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_write(
  uint8                        * data_buf_ptr,
  uint32                         data_buf_len,
  simlock_data_file_enum_type    file_enum,
  simlock_sfs_enum_type          sfs_type
);

/*===========================================================================
FUNCTION SIMLOCK_FILE_CHECK

DESCRIPTION
  This function checks if the requested file exists by calling the SFS or
  EFS based get file size functions

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_check(
   const char * sfs_file_ptr,
   uint32     * sfs_file_len_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_FILE_MKDIR

DESCRIPTION
  This function creates a directory in the file system

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_mkdir(
   const char * sfs_file_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_FILE_COMMIT_TO_FS_IF_NEEDED

DESCRIPTION
  This function checks the status of the variable, which indicates whether
  a write to the file system has happened which requires an efs sync.
  It performs efs sync operation if required.
  It also resets the variable value.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_file_commit_to_fs_if_needed(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_FILE_MKDIR_EFS

DESCRIPTION
  This function creates a directory in the EFS

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_mkdir_efs(
  const char * efs_file_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_FILE_SECURE

DESCRIPTION
  This function copies the file data from a non-secured location (e.g. EFS)
  to a secured location (e.g. SFS). It then deletes the file from the
  non-secured location

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_secure(
  const char                   * src_file_name_ptr,
  uint32                         src_file_len_expected,
  simlock_data_file_enum_type    dest_file_enum
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_FILE_H */