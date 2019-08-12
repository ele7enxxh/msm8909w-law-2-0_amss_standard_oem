#ifndef MMGSDI_EFS_H
#define MMGSDI_EFS_H
/*===========================================================================


            G S D I   T A S K   A N D  E F S  I N T E R F A C E

                                 H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001-2005,2008 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_efs.h#1 $$DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
-------------------------------------------------------------------------------
09/07/15   av      Migrate to uim_common EFS APIs
07/28/14   vv      Remove unused data types
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
10/16/13   df      Convert gsdi_efs_read/write/mkdir to return mmgsdi type
01/30/12   bcho    Function gsdi_efs_read_file_with_len() modified
12/27/11   bcho    Function added to read requested bytes from an EFS file
12/21/11   shr     Legacy GSDI removal updates
05/02/10   nb      Removed code under feature FEATURE_MMGSDI_PERSONALIZATION_NON_QC
09/01/09   mib     Removed deprecated fs.h
07/08/09   shr     Fixed Lint warnings
01/16/09   nb      Replaced EFS1 API calls with EFS2 APIs
12/18/08   sun     Removed old Perso
03/08/06   jar     Merged in Branched Lint fixes
05/11/05   sst     Lint fixes
05/25/04   jar     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "fs_public.h"
#include "mmgsdi.h"
#include "uim_common_efs.h"

#define GSDI_SIMSECURE_MAX_FILE_SIZE         604
#define GSDI_SIMSECURE_PARTITION_STATE_SIZE    4
#define GSDI_SIMSECURE_UINT8_STATE_OFFSET   0x00
#define GSDI_SIMSECURE_READ_PAGE_SIZE       0x200


/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  GSDI_SIM_PARTITION_NOT_INIT_S     = 0x00,
  GSDI_SIM_PARTITION_READ_WRITE_S,
  GSDI_SIM_PARTITION_READ_ONLY_S,
  GSDI_SIM_PARTITION_NOT_INIT_FF_S  = 0xFF
}gsdi_efs_sim_partition_enum_state;

/* ------------------------------------------------------------------------*/
/*                   G S D I   E F S   I N T E R F A C E                   */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/

typedef enum
{
  SIMSECURE_SUCCESS              =     0,
  SIMSECURE_NAND_DEVICE_FAIL     =    -1,  // Device initialization failed
  SIMSECURE_PARTITION_FAIL       =    -2   // Corrupt partition
}simsecure_status_t;

/*===========================================================================
FUNCTION GSDI_EFS_READ_FILE

DESCRIPTION
  Generic Function called by GSDI to read a file from EFS.  This function
  will valide the input parameters provided and return the Data requested.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                 GSDI_SUCCESS:  File was read
                 GSDI_ERROR:    Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_read_file(
  const char                  * file_handle,
  int32                       * returned_data_len_p,
  uint8                       * data_buffer_p,
  uim_common_efs_context_type   context
);

/*===========================================================================
FUNCTION GSDI_EFS_READ_FILE_WITH_LEN

DESCRIPTION
  Generic Function called by GSDI to read requested number of bytes from an
  EFS file. This function will validate the input parameters provided and
  return the Data requested.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                   MMGSDI_SUCCESS:  File was read
                   MMGSDI_ERROR:    Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_read_file_with_len(
  const char                  * file_handle,
  int32                         requested_data_len,
  uint8                       * data_buffer_p,
  uim_common_efs_context_type   context
);


/*===========================================================================
FUNCTION GSDI_EFS_MKDIR

DESCRIPTION
  Generic Function called by GSDI to test whether a directory or a file
  already exists within the specified file structure.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                   MMGSDI_SUCCESS:  File exists
                   MMGSDI_ERROR:    Error occurred

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_mkdir(
  const char *   handle
);

/*===========================================================================
FUNCTION GSDI_EFS_WRITE_FILE

DESCRIPTION
  Generic Function called by GSDI to write a file to EFS.  This function
  will valide the input parameters provided and return a status when
  completed.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                   MMGSDI_SUCCESS:  File was written
                   MMGSDI_ERROR:    Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_write_file(
  const char                  * file_handle,
  int32                         num_bytes,
  uint8                       * data_buffer_p,
  uim_common_efs_context_type   context
);

/*===========================================================================
FUNCTION GSDI_EFS_DELETE_FILE

DESCRIPTION
  Generic Function called by GSDI to delete a file to EFS.  This function
  will valide the input parameters provided and return a status when
  completed.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                   MMGSDI_SUCCESS:  File was deleted
                   MMGSDI_ERROR:    Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_delete_file(
  const char                  * file_handle_ptr,
  uim_common_efs_context_type   context
);


/*===========================================================================
FUNCTION GSDI_EFS_NAME_TEST

DESCRIPTION
  Generic Function called by GSDI to test whether a directory or a file
  already exists within the specified file structure.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                   MMGSDI_SUCCESS:  File exists
                   MMGSDI_ERROR:    Error occurred

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_name_test(
  const char                  * handle,
  uim_common_efs_context_type   context
);

/*===========================================================================
FUNCTION GSDI_EFS_CREATE_FILE

DESCRIPTION
  Generic Function called by GSDI to create a file in EFS.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                   MMGSDI_SUCCESS:  File was created
                   MMGSDI_ERROR:    Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_create_file(
  const char                  * file_handle,
  uim_common_efs_context_type   context
);

/*===========================================================================
FUNCTION GSDI_FS_GET_FILE_SIZE

DESCRIPTION
  Generic Function called by GSDI to retrieve the size of a file in FS.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                   MMGSDI_SUCCESS:   File size was retrieved
                   MMGSDI_NOT_FOUND: File name or directory is not existent in
                                   FS.
                   MMGSDI_ERROR:     Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_fs_get_file_size(
  const char                  * file_handle,
  int32                       * returned_data_len_p,
  uim_common_efs_context_type   context
);

#endif /* MMGSDI_EFS_H */

