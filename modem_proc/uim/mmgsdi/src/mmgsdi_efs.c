/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                                 G S D I  T A S K

                                      A N D

          F I L E  S Y S T E M  I N T E R F A C E  F U N C T I O N S


GENERAL DESCRIPTION

  This source files contains the GSDI to EFS / FS Interface Functions used
  by the GSDI Task.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

                        COPYRIGHT INFORMATION

Copyright (c) 2001-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_efs.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/15   bcho    Use MCFG Context enum instead of typecast
09/07/15   av      Migrate to mcfg EFS APIs
11/05/14   av      Change dir permissions from 0x777 to 777 for mkdir
07/28/14   vv      Remove unused data types
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
01/16/14   df      Use v2.0 diag macros
10/16/13   df      Convert gsdi_efs_read/write/mkdir to return mmgsdi type
09/23/13   vr      qtf-crm MOB migration
04/26/12   av      Cleaned up test framework specific code as detouring of
                   efs calls on mob is now encapsulated in uim_stubs
03/16/12   yt      Remove inclusion of tmc.h
01/30/12   bcho    Function gsdi_efs_read_file_with_len() modified
12/27/11   bcho    Function added to read requested bytes from an EFS file
12/21/11   kk      Legacy GSDI removal updates
10/07/10   ms      Fixed Lint errors
09/23/10   ms      Enable perso features FEATURE_MMGSDI_PERSONALIZATION
                   and FEATURE_MMGSDI_PERSONALIZATION_ENGINE
07/23/10   rm      Fixed compilation errors for Q6
03/08/10   yt      Fixed compilation errors
09/14/09   rn      Fixed indentation lint errors
09/01/09   mib     Removed deprecated fs.h
07/20/09   mib     Replaced flag TEST_FRAMEWORK with FEATURE_UIM_TEST_FRAMEWORK
07/08/09   shr     Fixed Lint warnings
06/15/09   sg      Changed gsdi_efs_create_file function to use efs_creat for qtf
                   Changed gsdi_efs_write_file function to use efs_creat for qtf
01/22/09   nb      Added check for file existence before removing
01/16/09   nb      Replaced EFS1 API calls with EFS2 APIs
12/18/08   sun     Removed old Perso
11/12/08   kk      Removed code defined under
                   FEATURE_MMGSDI_PERSONALIZATION_NON_QC
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
03/08/06   jar     Merged in Branched SIM Lock.
11/09/05   sun     Fixed lint errors
05/13/05   sun     Removed file size limits on read/write
05/11/05   sst     Lint fixes
11/15/04   jar     Changed to use GSDI MSG ERROR DEBUG MACROS
06/20/04 jar/tml   Cleanup by removing variables not used.
                   Fixes for Write Operations
05/18/04   jar     Initial Revision

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "intconv.h"
#include "customer.h"
#include "string.h"
#include "err.h"
#include "mmgsdi_efs.h"
#include "mmgsdi.h"
#include "fs_errno.h"
#include "uim_msg.h"
#include "mcfg_fs.h"

#if defined(FEATURE_UIM_TEST_FRAMEWORK)
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* ------------------------------------------------------------------------*/
/*                   G S D I   E F S   I N T E R F A C E                   */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION GSDI_FS_GET_FILE_SIZE

DESCRIPTION
  Generic Function called by GSDI to retrieve the size of a file in FS.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
                   MMGSDI_SUCCESS:   File size was retrieved
                   MMGSDI_ERROR:     Failure occurred (Need to check EF Failure)

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_fs_get_file_size(
  const char                  * file_handle_ptr,
  int32                       * returned_data_len_ptr,
  uim_common_efs_context_type   context
)
{
  struct fs_stat         file_stat;

  memset(&file_stat, 0x00, sizeof(file_stat));
  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     2.  returned_data_len_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    UIM_MSG_ERR_0("NULL File Handle");
    return MMGSDI_ERROR;
  }
  if ( returned_data_len_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("NULL Returned Data Len Pointer");
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Call the FS Function to get the File Size
     ------------------------------------------------------------------------*/
  if(mcfg_fs_stat((const char *)file_handle_ptr, &file_stat,
                  MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context))
         == MCFG_FS_STATUS_OK)
  {
    *returned_data_len_ptr = uint32toint32(file_stat.st_size);
    return MMGSDI_SUCCESS;
  }
  else
  {
    UIM_MSG_ERR_0("EFS File or Dir Not Found ");
    *returned_data_len_ptr = 0;
    return MMGSDI_ERROR;
  }
} /* gsdi_fs_get_file_size */


/*===========================================================================
FUNCTION GSDI_EFS_READ_FILE

DESCRIPTION
  Generic Function called by GSDI to read a file from EFS.  This function
  will validate the input parameters provided and return the Data requested.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
               GSDI_SUCCESS:  File was read
               GSDI_ERROR:    Failure occurred (Need to check EF Failure)
                              or the Input parameters were not valid.

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_read_file(
  const char                  * file_handle_ptr,
  int32                       * returned_data_len_ptr,
  uint8                       * data_buffer_ptr,
  uim_common_efs_context_type   context
)
{
  fs_size_t             file_size       = 0;
  struct fs_stat        file_stat;
  int                   file_descriptor = 0;

  memset(&file_stat, 0x00, sizeof(file_stat));

  /* --------------------------------------------------------------------------
   Perform basic param checking.
   1.  file_handle_ptr can not be NULL
   2.  returned_data_len_ptr can not be NULL
   3.  data_buffer_ptr can not be NULL
   -------------------------------------------------------------------------*/
  if (file_handle_ptr       == NULL ||
      returned_data_len_ptr == NULL ||
      data_buffer_ptr       == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Input parameter is NULL");
    return MMGSDI_ERROR;
  }

  if(mcfg_fs_stat((const char *)file_handle_ptr, &file_stat,
                  MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    MMGSDI_DEBUG_MSG_ERROR_1("efs stat procedure failed: %x",
                             file_stat.st_size);
    return MMGSDI_ERROR;
  }

  file_size = file_stat.st_size;
  /* --------------------------------------------------------------------------
     Open the File in EFS for Read Only
     -------------------------------------------------------------------------*/
  file_descriptor = mcfg_fopen(file_handle_ptr, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS,
                               MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context));
  if(file_descriptor < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs open Procedure Failed ");
    return MMGSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Read the file into buffer provided
     ----------------------------------------------------------------------*/
  if(mcfg_fread(file_descriptor, (void *)data_buffer_ptr, file_size, MCFG_FS_TYPE_EFS) != file_size)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs read procedure failed ");
    return MMGSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Data has been successfully copied in data_buffer_ptr
     Need to assign the returned_data_len_ptr value and close this
     file
     ----------------------------------------------------------------------*/
  *returned_data_len_ptr = (int32)file_size;

  if(mcfg_fclose(file_descriptor, MCFG_FS_TYPE_EFS) != 0)
  {
    UIM_MSG_HIGH_1("efs close procedure failed 0x%x", file_descriptor);
  }

  return MMGSDI_SUCCESS;
}  /* gsdi_efs_read_file */

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
                                    or the Input parameters were not valid.

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_read_file_with_len(
  const char                  * file_handle_ptr,
  int32                         requested_data_len,
  uint8                       * data_buffer_ptr,
  uim_common_efs_context_type   context
)
{
  fs_size_t             file_size       = 0;
  struct fs_stat        file_stat;
  int                   file_descriptor = 0;

  memset(&file_stat, 0x00, sizeof(file_stat));

  /* --------------------------------------------------------------------------
   Perform basic param checking.
   1.  file_handle_ptr can not be NULL
   2.  data_buffer_ptr can not be NULL
   -------------------------------------------------------------------------*/
  if (file_handle_ptr       == NULL ||
      data_buffer_ptr       == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Input parameter is NULL");
    return MMGSDI_ERROR;
  }

  if(mcfg_fs_stat((const char *)file_handle_ptr, &file_stat,
                  MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    MMGSDI_DEBUG_MSG_ERROR_1("efs stat procedure failed: %x",
                             file_stat.st_size);
    return MMGSDI_ERROR;
  }

  file_size = file_stat.st_size;
  if(requested_data_len <= file_size)
  {
    file_size = requested_data_len;
  }
  else
  {
    MMGSDI_DEBUG_MSG_ERROR_2("requested len %x is greater than file size %x",
                             requested_data_len, file_size);
    return MMGSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Open the File in EFS for Read Only
     -------------------------------------------------------------------------*/
  file_descriptor = mcfg_fopen(file_handle_ptr, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS,
                               MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context));
  if(file_descriptor < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs open Procedure Failed ");
    return MMGSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Read the file into buffer provided
     ----------------------------------------------------------------------*/
  if(mcfg_fread(file_descriptor, (void *)data_buffer_ptr, file_size, MCFG_FS_TYPE_EFS) != file_size)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs read procedure failed ");
    return MMGSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Data has been successfully copied in data_buffer_ptr
     ----------------------------------------------------------------------*/

  if(mcfg_fclose(file_descriptor, MCFG_FS_TYPE_EFS) != 0)
  {
    UIM_MSG_HIGH_1("efs close procedure failed 0x%x", file_descriptor);
  }

  return MMGSDI_SUCCESS;
}  /* gsdi_efs_read_file_with_len */


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
                                    or the input params were not valid.

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type gsdi_efs_create_file(
  const char                  * file_handle_ptr,
  uim_common_efs_context_type   context
)
{
  int                     file_descriptor = 0;

  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("NULL File Handle");
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     To create a file in EFS, you must use the mcfg_fopen Method.
     Provide the Parameter:  O_CREAT to create the file.
     ------------------------------------------------------------------------*/
  file_descriptor = mcfg_fopen(file_handle_ptr, MCFG_FS_O_CREAT, MCFG_FS_ALLPERMS,
                               MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context));
  if(file_descriptor < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs open Procedure Failed ");
    return MMGSDI_ERROR;
  }
  /* ------------------------------------------------------------------------
     Need to close the file used in the open.
     If the close fails, we'll assume it is still usable for now.
     ----------------------------------------------------------------------*/
  if(mcfg_fclose(file_descriptor, MCFG_FS_TYPE_EFS) != 0)
  {
    /* We will flag the Error but will not fail the entire
    ** Procedures since the handle could not be closed
    */
    UIM_MSG_HIGH_1("efs close procedure failed 0x%x", file_descriptor);
  }

  return MMGSDI_SUCCESS;
} /* gsdi_efs_create_file */

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
  const char                  * file_handle_ptr,
  uim_common_efs_context_type   context
)
{
  struct fs_stat          file_stat;

  memset(&file_stat, 0x00, sizeof(file_stat));

  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR_0 ("NULL Handle Provided");
    return MMGSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     To test the existence of the file, we must use the fs_name_test()
     function provided.
     ------------------------------------------------------------------------*/

  /* --------------------------------------------------------------------------
     Criteria for success is if the file is found
     ------------------------------------------------------------------------*/
  if(mcfg_fs_stat(file_handle_ptr, &file_stat,
                  MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context))
         == MCFG_FS_STATUS_OK)
  {
    UIM_MSG_HIGH_0("efs stat procedure success");
    return MMGSDI_SUCCESS;
  }
  else
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs stat procedure failed ");
    return MMGSDI_ERROR;
  }
} /* gsdi_efs_name_test*/


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
  const char *   file_handle_ptr
)
{
  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR_0 ("NULL Handle Provided");
    return MMGSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     To create the directory...we'll use the mcfg_mkdir() function.
     ------------------------------------------------------------------------*/
  if(mcfg_mkdir(file_handle_ptr, 0777, MCFG_FS_TYPE_EFS) < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs mkdir procedure failed");
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* gsdi_efs_mkdir */


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
  const char                  * file_handle_ptr,
  int32                         num_bytes,
  uint8                       * data_buffer_ptr,
  uim_common_efs_context_type   context
)
{
  mmgsdi_return_enum_type mmgsdi_status   = MMGSDI_SUCCESS;
  uint32                  bytes_to_write  = 0;
  int                     file_descriptor = 0;

  bytes_to_write  = int32touint32(num_bytes);

  /* --------------------------------------------------------------------------
     Open the File in EFS for Create as a New File
     -------------------------------------------------------------------------*/
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("NULL File Handle");
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Open the File in EFS for Create as a New File
     -------------------------------------------------------------------------*/
  file_descriptor = mcfg_fopen(file_handle_ptr, MCFG_FS_O_WRONLY|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC,
                               MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context));
  if(file_descriptor < 0)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs open Procedure Failed ");
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
    Write the Data to the File
    -------------------------------------------------------------------------*/
  if(mcfg_fwrite(file_descriptor, (void *)data_buffer_ptr, bytes_to_write, MCFG_FS_TYPE_EFS) != bytes_to_write)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("efs write failed ");
    mmgsdi_status = MMGSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Data has been successfully written to FS.  Ok to close up the file
     ----------------------------------------------------------------------*/
  if(mcfg_fclose(file_descriptor, MCFG_FS_TYPE_EFS) != 0)
  {
    UIM_MSG_HIGH_1("efs close procedure failed 0x%x", file_descriptor);
  }

  return mmgsdi_status;
}  /* gsdi_efs_write_file */


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
)
{
  if ( file_handle_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("NULL File Handle");
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Remove the file.
     ------------------------------------------------------------------------*/
  if(mcfg_fs_delete(file_handle_ptr, MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context))
         != MCFG_FS_STATUS_OK)
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Failure to remove file");
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
}/* gsdi_efs_delete_file */

