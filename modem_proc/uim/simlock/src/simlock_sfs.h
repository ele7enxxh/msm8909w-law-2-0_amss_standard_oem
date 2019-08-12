#ifndef SIMLOCK_SFS_H
#define SIMLOCK_SFS_H
/*===========================================================================


            S I M L O C K   S F S   H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_sfs.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK

#include "simlock.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION SIMLOCK_SFS_WRITE_FILE

DESCRIPTION
  This function gets the size of the requested file by calling the SFS based
  get file size function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_write_file(
  uint8      * data_buf_ptr,
  uint32       data_buf_len,
  const char * sfs_file_name
);

/*===========================================================================
FUNCTION SIMLOCK_SFS_GET_FILE_SIZE

DESCRIPTION
  This function gets the size of the requested file by calling the SFS based
  get file size function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_get_file_size(
  const char  * sfs_file_name,
  uint32      * size
);

/*===========================================================================
FUNCTION SIMLOCK_SFS_READ_FILE

DESCRIPTION
  This function reads the data from the requested file by calling the SFS 
  based read function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_read_file(
  uint8      * buffer_ptr,
  uint32       len,
  const char * sfs_file_name
);

/*===========================================================================
FUNCTION SIMLOCK_SFS_MKDIR

DESCRIPTION
  This function creates a directory by calling the SFS based make dir 
  function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_mkdir(
  const char * sfs_file_name
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_SFS_H */