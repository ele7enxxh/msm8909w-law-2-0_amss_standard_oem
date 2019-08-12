/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   S T O R A G E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA storage functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/15   tkl     Added HTTP Cookies support
05/05/15   lxu     Fixed various issues
04/29/15   tl      Add support for GBA_U
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "fs_errno.h"
#include "fs_public.h"
#ifdef FEATURE_SEC_SFS
#include "sfs_api.h"
#endif /* FEATURE_SEC_SFS */

#include "gba.h"
#include "gba_lib.h"
#include "gba_platform.h"
#include "gba_storage.h"
#include "gba_util.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

#define GBA_STORAGE_GBA_DIRECTORY        "/gba"
#define GBA_STORAGE_3GPP_PRI             "/gba/3gpp_pri"
#define GBA_STORAGE_3GPP_SEC             "/gba/3gpp_sec"
#define GBA_STORAGE_ISIM_SLOT_1          "/gba/isim_slot_1"
#define GBA_STORAGE_ISIM_SLOT_2          "/gba/isim_slot_2"
#define GBA_STORAGE_HTTP_COOKIES         "/gba/http_cookies"

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_STORAGE_PUT_PROFILE

DESCRIPTION
  Function stores GBA profile information in SFS

DEPENDENCIES
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_storage_put_profile (
  gba_session_type                        session_type,
  const gba_storage_profile_data_type   * profile_data_ptr
)
{
  gba_result_enum_type   gba_status      = GBA_SUCCESS;
  const char           * file_handle_ptr = NULL;
#if defined(FEATURE_UIM_TEST_FRAMEWORK) || defined(FEATURE_SEC_SFS)
  int                    file_descriptor = 0;
#endif /* define(FEATURE_UIM_TEST_FRAMEWORK) || define(FEATURE_SEC_SFS) */

  if(profile_data_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  GBA_MSG_LOW_1("gba_storage_put_profile session: 0x%x", session_type);

  switch(session_type)
  {
    case GBA_3GPP_PROV_SESSION_PRI:
      file_handle_ptr = GBA_STORAGE_3GPP_PRI;
      break;
    case GBA_3GPP_PROV_SESSION_SEC:
      file_handle_ptr = GBA_STORAGE_3GPP_SEC;
      break;
    case GBA_NON_PROV_SESSION_SLOT_1:
      file_handle_ptr = GBA_STORAGE_ISIM_SLOT_1;
      break;
    case GBA_NON_PROV_SESSION_SLOT_2:
      file_handle_ptr = GBA_STORAGE_ISIM_SLOT_2;
      break;
    default:
      return GBA_GENERIC_ERROR;
  }

  gba_storage_clear_profile(session_type);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#elif defined(FEATURE_SEC_SFS)
  file_descriptor = sfs_open(file_handle_ptr, O_WRONLY|O_CREAT);
  if(!file_descriptor)
  {
    return GBA_GENERIC_ERROR;
  }

  if(sfs_write(file_descriptor,
               (const char *)profile_data_ptr,
               (int)sizeof(gba_storage_profile_data_type))
       != (int)sizeof(gba_storage_profile_data_type))
  {
    gba_status = GBA_GENERIC_ERROR;
  }

  (void)sfs_close(file_descriptor);
#else
  gba_status = GBA_UNSUPPORTED;
#endif /* FEATURE_UIM_TEST_FRAMEWORK || FEATURE_SEC_SFS */

  return gba_status;
} /* gba_storage_put_profile */


/*===========================================================================
FUNCTION GBA_STORAGE_GET_PROFILE

DESCRIPTION
  Function retrieves the GBA profile information from SFS

DEPENDENCIES
  None

RETURN VALUE
  gba_storage_profile_data_type *

SIDE EFFECTS
  None
===========================================================================*/
gba_storage_profile_data_type * gba_storage_get_profile (
  gba_session_type                session_type
)
{
  const char                     * file_handle_ptr         = NULL;
  int                              file_descriptor         = 0;
  gba_storage_profile_data_type  * profile_data_ptr        = NULL;
  gba_result_enum_type             gba_status              = GBA_SUCCESS;

  GBA_MSG_LOW_1("gba_storage_get_profile session: 0x%x", session_type);

  switch(session_type)
  {
    case GBA_3GPP_PROV_SESSION_PRI:
      file_handle_ptr = GBA_STORAGE_3GPP_PRI;
      break;
    case GBA_3GPP_PROV_SESSION_SEC:
      file_handle_ptr = GBA_STORAGE_3GPP_SEC;
      break;
    case GBA_NON_PROV_SESSION_SLOT_1:
      file_handle_ptr = GBA_STORAGE_ISIM_SLOT_1;
      break;
    case GBA_NON_PROV_SESSION_SLOT_2:
      file_handle_ptr = GBA_STORAGE_ISIM_SLOT_2;
      break;
    default:
      return NULL;
  }

  profile_data_ptr =
    (gba_storage_profile_data_type *)GBA_MALLOC(sizeof(gba_storage_profile_data_type));
  if(profile_data_ptr == NULL)
  {
    return NULL;
  }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#elif defined(FEATURE_SEC_SFS)
  file_descriptor = sfs_open(file_handle_ptr, O_RDONLY);
  if(!file_descriptor)
  {
    GBA_MSG_HIGH_0("sfs open failed");
    gba_status = GBA_GENERIC_ERROR;
    goto error;
  }

  /* Read the file into buffer provided */
  if (sfs_read(file_descriptor,
               (char *)profile_data_ptr,
               (int)sizeof(gba_storage_profile_data_type))
        != (int)sizeof(gba_storage_profile_data_type))
  {
    (void)sfs_close(file_descriptor);
    GBA_MSG_HIGH_0("sfs read failed");
    gba_status = GBA_GENERIC_ERROR;
    goto error;
  }

  /* Release the file handle */
  (void)sfs_close(file_descriptor);
#else
  return GBA_UNSUPPORTED;
#endif /* FEATURE_UIM_TEST_FRAMEWORK || FEATURE_SEC_SFS */

  return profile_data_ptr;

error:
  gba_storage_free_profile(&profile_data_ptr);

  return NULL;
} /* gba_storage_get_profile */


/*===========================================================================
FUNCTION GBA_STORAGE_CLEAR_PROFILE

DESCRIPTION
  Function clears the GBA profile data stored in SFS for a specific session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_storage_clear_profile (
  gba_session_type              session_type
)
{
  const char           * file_handle_ptr = NULL;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#elif defined(FEATURE_SEC_SFS)
  int                    file_descriptor = 0;
#endif /* FEATURE_UIM_TEST_FRAMEWORK || FEATURE_SEC_SFS*/

  switch(session_type)
  {
    case GBA_3GPP_PROV_SESSION_PRI:
      file_handle_ptr = GBA_STORAGE_3GPP_PRI;
      break;
    case GBA_3GPP_PROV_SESSION_SEC:
      file_handle_ptr = GBA_STORAGE_3GPP_SEC;
      break;
    case GBA_NON_PROV_SESSION_SLOT_1:
      file_handle_ptr = GBA_STORAGE_ISIM_SLOT_1;
      break;
    case GBA_NON_PROV_SESSION_SLOT_2:
      file_handle_ptr = GBA_STORAGE_ISIM_SLOT_2;
      break;
    default:
      return;
  }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#elif defined(FEATURE_SEC_SFS)
  /* If the file already exists remove it */
  file_descriptor = sfs_open(file_handle_ptr, O_RDONLY);
  if(file_descriptor)
  {
    (void)sfs_close(file_descriptor);

    if(sfs_rm(file_handle_ptr) != E_SUCCESS)
    {
      GBA_MSG_ERR_0("sfs file deletion failed");
    }
  }
#endif /* FEATURE_UIM_TEST_FRAMEWORK || FEATURE_SEC_SFS */
} /* gba_storage_clear_profile */


/*===========================================================================
FUNCTION GBA_STORAGE_PUT_COOKIES

DESCRIPTION
  Function stores GBA http cookies information in EFS. Parsing logic of
  the EFS file is implemented in gba_http_cookie. EFS file format:
  | data size (uint16)| data (uint8 * data size)| ie. total EFS size
  data size + sizeof(uint16).

DEPENDENCIES
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_storage_put_cookies (
  const void   * data_ptr,
  uint16         data_size
)
{
  gba_result_enum_type   gba_status      = GBA_SUCCESS;
  const char           * file_handle_ptr = GBA_STORAGE_HTTP_COOKIES;
  int                    file_descriptor = 0;

  if(data_ptr == NULL || data_size == 0)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_storage_clear_cookies();

  file_descriptor = efs_open(file_handle_ptr, O_WRONLY|O_CREAT);
  if(file_descriptor < 0)
  {
    return GBA_GENERIC_ERROR;
  }

  /* EFS file format: | data size (uint16)| data (uint8 * data size)| */
  if((efs_write(file_descriptor,
                (const char *)&data_size,
                (fs_size_t)(sizeof(uint16)))
       != (fs_ssize_t)(sizeof(uint16))) ||
     (efs_write(file_descriptor,
                (const char *)data_ptr,
                (fs_size_t)data_size)
       != (fs_ssize_t)data_size))
  {
    gba_status = GBA_GENERIC_ERROR;
  }

  (void)efs_close(file_descriptor);

  return gba_status;
} /* gba_storage_put_cookies */


/*===========================================================================
FUNCTION GBA_STORAGE_GET_COOKIES

DESCRIPTION
  Function retrieves the list of http cookies from EFS. Parsing logic of
  the EFS file is implemented in gba_http_cookie. EFS file format:
  | data size (uint16)| data (uint8 * data size)| ie. total EFS size
  data size + sizeof(uint16).

DEPENDENCIES
  None

RETURN VALUE
  void *

SIDE EFFECTS
  None
===========================================================================*/
void * gba_storage_get_cookies (
  uint16      * data_size_ptr
)
{
  const char          * file_handle_ptr = GBA_STORAGE_HTTP_COOKIES;
  int                   file_descriptor = 0;
  void                * data_ptr        = NULL;

  if(data_size_ptr == NULL)
  {
    return NULL;
  }

  /* Open the File in EFS for Read Only */
  file_descriptor = efs_open(file_handle_ptr, O_RDONLY);
  if(file_descriptor < 0)
  {
    GBA_MSG_MED_0("Error opening file with cookies for GBA");
    return NULL;
  }

  do
  {
    /* EFS file format: | data size (uint16)| data (uint8 * data size)|
    ie. total EFS size = data size + sizeof(uint16).
    read first byte of file to get data size */
    if(efs_read(file_descriptor, data_size_ptr, (fs_size_t)sizeof(uint16))
       != (fs_ssize_t)sizeof(uint16) || *data_size_ptr <= 0)
    {
      break;
    }

    data_ptr = (void *)GBA_MALLOC(*data_size_ptr);
    if(data_ptr == NULL)
    {
      break;
    }

    /* Read the file into buffer provided */
    if(efs_read(file_descriptor,
                data_ptr,
                (fs_size_t)(*data_size_ptr))
       != (fs_ssize_t)(*data_size_ptr))
    {
      GBA_FREE(data_ptr);
    }
  } while(0);

  /* Release the file handle */
  (void)efs_close(file_descriptor);

  return data_ptr;
} /* gba_storage_get_cookies */


/*===========================================================================
FUNCTION GBA_STORAGE_CLEAR_COOKIES

DESCRIPTION
  Function clears the cookie list data stored in SFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_storage_clear_cookies (
  void
)
{
  const char           * file_handle_ptr = GBA_STORAGE_HTTP_COOKIES;
  struct fs_stat         file_stat       = {0};

  /* If the file already exists remove it */
  if(efs_stat(file_handle_ptr, &file_stat) == 0)
  {
    (void)efs_unlink(file_handle_ptr);
  }
} /* gba_storage_clear_cookies */


/*===========================================================================
FUNCTION GBA_STORAGE_CREATE_DIR

DESCRIPTION
  Function creates the GBA EFS/SFS directory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_storage_create_dir (
  void
)
{
  (void)efs_mkdir(GBA_STORAGE_GBA_DIRECTORY, 0x777);
#if !defined(FEATURE_UIM_TEST_FRAMEWORK) && defined(FEATURE_SEC_SFS)
  (void)sfs_mkdir(GBA_STORAGE_GBA_DIRECTORY);
#endif /* FEATURE_SEC_SFS */
} /* gba_storage_create_dir */


/*===========================================================================
FUNCTION GBA_STORAGE_INIT

DESCRIPTION
  Function initalizes the SFS storage module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_storage_init (
  void
)
{
  gba_session_type session_type = GBA_3GPP_PROV_SESSION_PRI;

  for(session_type = GBA_3GPP_PROV_SESSION_PRI; session_type < GBA_SESSION_MAX; session_type++)
  {
    (void)gba_storage_clear_profile(session_type);
  }

  /* Make sure GBA storage directory is present in EFS.
     not reading cookie list from storage unless Ub is required */
  gba_storage_create_dir();
} /* gba_storage_init */


/*===========================================================================
FUNCTION GBA_STORAGE_FREE_PROFILE

DESCRIPTION
  Function clears and then frees the storage profile data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_storage_free_profile (
  gba_storage_profile_data_type  ** profile_data_pptr
)
{
  if(profile_data_pptr == NULL ||
     *profile_data_pptr == NULL)
  {
    return;
  }

  /* Clear the profile data before free so sensitive information (Ks) is not left
     lingering within memory where it could potentially be attacked. */
  memset(*profile_data_pptr, 0x00, sizeof(gba_storage_profile_data_type));
  GBA_FREE(*profile_data_pptr);
} /* gba_storage_free_profile */

