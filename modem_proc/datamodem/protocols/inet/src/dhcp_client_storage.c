/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P _ C L I E N T _ S T O R A G E . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol client storage code.
  This file provides the shim layer between the core and the
  persistant file system to store lease infomation.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_client_storage.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/01/15    cx     Replaced EFS APIs with MCFG APIs
01/10/14    vl     Optimization of F3 messages
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
03/11/08    am     Included header for snprintf.
10/31/08    am     Fixed off-target compiler warnings.
09/26/08    am     Changed malloc/free names for off target support.
11/05/05    rt     Fixed coverity errors.
11/01/04    clp    Initial development work done.
12/30/04    clp    Code review: static functions now have dhcpi_ prefix
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "dhcp_clienti.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include <string.h>
#include <stdio.h>
#include "fs_public.h"
#include "ps_in.h" /* For byte order macros */

#include <stringl/stringl.h> /* For snprintf */
#include "ds_Utils_DebugMsg.h"
#include "mcfg_fs.h"

/* If the structure of the file changes for any reason, including
 * potentially compiler packing changes, then the version number MUST
 * be bumped up!  We have a 32 bit field for this, so it can be bumped
 * many times if needed.
 */
#define DCHP_CLIENT_STORAGE_VERSION (dss_htonl(0))

/* The path to the dhcp client directory. */
#define DHCP_CLIENT_PATH "/etc/dhcpc.d"

/* Since we may have to create the path to our directory, we list the
 * components here
 */
#define DHCP_CLIENT_PATH_COMPONENT "/etc"

/* The length of the path to the client directory */
#define DHCP_CLIENT_PATH_LEN sizeof(DHCP_CLIENT_PATH)

/* The name of the dhcp client version file, including the path */
#define DHCP_CLIENT_VERSION_FILE DHCP_CLIENT_PATH "/version"

/* Each lease lives in its own file, and the file name of the lease
 * files are based on the iface id to which the lease belongs. This
 * macro defines the basename of the lease files, including the path.
 */
#define DHCP_CLIENT_LEASE_FILE_BASE DHCP_CLIENT_PATH "/lease"

/* The lease file name is the combination of the lease file base and
 * the iface id.  The id is a 32 bit number expanded out in hex to 8
 * digits.  This macro defines the length needed to store the lease
 * file, including the NUL termination.
 */
#define DHCP_CLIENT_LEASE_FILE_LEN (sizeof(DHCP_CLIENT_LEASE_FILE_BASE) + 9)

typedef struct dhcp_client_storage_info {
  /* module init info */
  dhcp_client_storage_init init;

  /* If we can't store/read leases, DHCP can still run, so we use this
   * to just fail out gracefully
   */
  boolean efs_valid;

  /* handle to lease file */
  int lease_handle;

  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the shim is
   * started.
   */
  struct dhcp_client_storage_info * cookie_ptr;
  /* The cookie takes the value below when allocated in _new */
#define DHCP_CLIENT_STORAGE_COOKIE 0X5704A6E0
  uint32 cookie;
} dhcp_client_storage_info;


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_STORAGE_CHECK_PATH

Description:
  This function checks the given path and attempts to create it if it
  it doesn't exist.

Arguements:
  dhcp_client_storage_info * info - The handle to the client
  char * path - The path to check.

Return value:
  void.
  efs_valid may be updated on failure.

Dependencies:
  The efs_valid flag in info should be initialized before calling.
===========================================================================*/
static void
dhcpi_client_storage_check_path
(
  dhcp_client_storage_info * info,
  char * path
)
{
  mcfg_fs_status_e_type mcfg_ret;
  int efs_ret;
  struct fs_stat stat;

  mcfg_ret = mcfg_fs_stat(path,&stat,MCFG_FS_TYPE_EFS,MCFG_FS_SUBID_NONE);
  if ( MCFG_FS_STATUS_OK != mcfg_ret || ! S_ISDIR(stat.st_mode) )
  {
    /* The unlink may fail if there is no file, but that is ok */
    (void) mcfg_fs_delete( path,MCFG_FS_TYPE_EFS,MCFG_FS_SUBID_NONE);
    efs_ret = mcfg_mkdir( path, S_IREAD|S_IWRITE|S_IEXEC,MCFG_FS_TYPE_EFS);
    if ( -1 == efs_ret )
    {
      /* With the pointer value, we can go back and try to figure out
       * what static string is at that location.
       */
      LOG_MSG_ERROR_1( "Failed to make path component %p", path );
      info->efs_valid = FALSE;
    }
  }
} /* dhcpi_client_storage_check_path */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_STORAGE_VERSION_INIT

Description:
  This function handles the cases that the version is incorrect or
  doens't exist.  In either case, we delete everything in the dhcp
  client directory, and attempt to create a new version file.

Arguements:
  dhcp_client_storage_info * info - The handle to the client

Return value:
  void.
  efs_valid may be updated on failure.

Dependencies:
  The efs_valid flag in info should be initialized before calling.
  The client path should already exist.
===========================================================================*/
static void
dhcpi_client_storage_version_init
(
  dhcp_client_storage_info * info
)
{
  int efs_ret;
  EFSDIR * efsdir;
  struct fs_dirent * dirent;
  uint32 version = DCHP_CLIENT_STORAGE_VERSION; /*lint !e572 */

  /* First, clear out all the files in the directory */
  if ( NULL != (efsdir = mcfg_fs_opendir(DHCP_CLIENT_PATH, MCFG_FS_TYPE_EFS)) )
  {
    while ( NULL != (dirent = efs_readdir(efsdir)) )
    {
      /* The unlink will not work for the directory pointers (., ..),
       * but we don't really mind that
       */
      (void) mcfg_fs_delete( dirent->d_name,MCFG_FS_TYPE_EFS,MCFG_FS_SUBID_NONE);
    }
    if ( -1 == mcfg_fs_closedir(efsdir, MCFG_FS_TYPE_EFS) ) 
    {
      LOG_MSG_ERROR_0( "Failed to close the client path directory");
    }
  }
  else
  {
    /* If the directoy path doesn't exist at this point, it never will */
    info->efs_valid = FALSE;
    return;
  }
  /* Now we can create the new version file */
  efs_ret = mcfg_fopen(DHCP_CLIENT_VERSION_FILE,
                     O_WRONLY|O_CREAT|O_TRUNC,
                       S_IREAD|S_IWRITE,MCFG_FS_TYPE_EFS,MCFG_FS_SUBID_NONE);
  if ( -1 == efs_ret )
  {
    LOG_MSG_ERROR_0( "Failed to open new version file" );
    info->efs_valid = FALSE;
    return;
  }
  if ( (fs_ssize_t)sizeof(uint32) != /* Cast because lint is stupid */
    mcfg_fwrite(efs_ret, &version, sizeof(uint32),MCFG_FS_TYPE_EFS) )
  {
    LOG_MSG_ERROR_0( "Failed to write version file" );
    info->efs_valid = FALSE;
    return;
  }
  if ( -1 == mcfg_fclose( efs_ret,MCFG_FS_TYPE_EFS ))
  {
    LOG_MSG_ERROR_0( "Failed to close written version file" );
  }
} /* dhcpi_client_storage_version_init */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_STORAGE_WRITE_RECOVER

Description:
  This function handles the error cases in the write function.
  We were not able write something, and need to clean up so there is
  not a corrupt lease.  To do this, we close the handle, remove the
  lease file and open it again.

Arguements:
  dhcp_client_storage_info * info - The handle to the client

Return value:
  void.
  efs_valid may be updated on failure.

Dependencies:
  The client path should already exist.
  This function should only be called if the efs_valid flag is TRUE.
===========================================================================*/
static void
dhcpi_client_storage_write_recover
(
  dhcp_client_storage_info * info
)
{
  char lease_filename[DHCP_CLIENT_LEASE_FILE_LEN];
  int efs_ret;

  /* Now if we have a valid version, open the lease file */
  ASSERT( TRUE == info->efs_valid );

  if ( 0 != mcfg_fclose(info->lease_handle,MCFG_FS_TYPE_EFS) )
  {
    LOG_MSG_ERROR_0( "Write error recovery: unable to close file");
    /* continue to stumble along */
  }
  /* Figure out the file name */
  (void) snprintf(lease_filename, DHCP_CLIENT_LEASE_FILE_LEN - 1,
                  DHCP_CLIENT_LEASE_FILE_BASE "%08lx", info->init.iface);
  /* Attempt to open the file again, truncating the corrupt lease
   * information.
   */
  if ( -1 == (efs_ret = mcfg_fopen(lease_filename,
                                 O_RDWR|O_TRUNC,
                                   S_IWRITE|S_IWRITE,MCFG_FS_TYPE_EFS,
                                   MCFG_FS_SUBID_NONE)) )
  {
    info->efs_valid = FALSE;
    LOG_MSG_ERROR_0("Failed to open truncate lease file");
    /* Ok, we can't truncate the file, can we unlink it? */
    if ( MCFG_FS_STATUS_OK != mcfg_fs_delete(lease_filename,
                                             MCFG_FS_TYPE_EFS,
                                             MCFG_FS_SUBID_NONE) )
    {
      DATA_ERR_FATAL("Write_recover, unable to delete file!");
      /* What else can we do? */
    }
  }
  else
  {
    info->lease_handle = efs_ret;
  }
} /* dhcpi_client_storage_write_recover */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_NEW

Description:
  This function initializes the storage layer with internal data and
  returns a handle to the core object.

Arguements:
  dhcp_client_storage_init * init - The information needed to
    initialize the storage layer.

Return value:
  void * - A handle to the new storage object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other
  storage functions are called.  The handle must be started (see
  dhcp_clienti_storage_start() below) before the  layer is
  fully functional.
===========================================================================*/
void *
dhcp_clienti_storage_new
(
   dhcp_client_storage_init * init
)
{
  dhcp_client_storage_info * handle;
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT STORAGE INIT ---" );

  if (NULL == init)
  {
    DATA_ERR_FATAL("init NULL?");
    return NULL;
  }
  if (NULL == init->d_malloc || NULL == init->d_free)
  {
    LOG_MSG_ERROR_2("malloc or free NULL %d %d", init->d_malloc, init->d_free);
    ASSERT(0);
    return NULL;
  }

  handle = init->d_malloc(sizeof(dhcp_client_storage_info));
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("Insufficent memory to init DHCP storage layer");
    return NULL;
  }
  memset( handle, 0, sizeof(dhcp_client_storage_info) );
  handle->init = *init; /* struct copy */
  handle->cookie = DHCP_CLIENT_STORAGE_COOKIE;

  return handle;
} /* dhcp_clienti_storage_new */

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_START

Description:
  This function configures the storage layer with the information on how
  to interact with the other client modules.

Arguements:
  void * handle - Handle to the storage object to start.

Return value:
  Boolean - TRUE if success, otherwise FALSE.

Dependencies:
  The handle must be have been allocated by dhcp_clienti_storage_new()
  above.
===========================================================================*/
boolean
dhcp_clienti_storage_start
(
  void * handle
)
{
  dhcp_client_storage_info * info = handle;
  int efs_ret;
  uint32 version;
  char lease_filename[DHCP_CLIENT_LEASE_FILE_LEN];

  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT STORAGE START ---" );
  /* sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if (DHCP_CLIENT_STORAGE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_STORAGE_COOKIE);
    ASSERT(0);
    return FALSE;
  }

  /* Assume that we will be successful. If we fail this will be updated */
  info->efs_valid = TRUE;

  /* Check/build the first path component */
  dhcpi_client_storage_check_path( info, DHCP_CLIENT_PATH_COMPONENT );

  /* Check/build the dhcp path component */
  dhcpi_client_storage_check_path( info, DHCP_CLIENT_PATH );

  /* Attempt to open and read the DHCP version file. */
  efs_ret = mcfg_fopen(DHCP_CLIENT_VERSION_FILE, 
                       O_RDONLY, S_IWRITE|S_IWRITE,
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_NONE);
  if ( -1 == efs_ret )
  {
    dhcpi_client_storage_version_init( info );
  }
  else
  {
    if ( (fs_ssize_t)sizeof(uint32) != /* Cast because lint is stupid */
         mcfg_fread(efs_ret, &version, sizeof(uint32),MCFG_FS_TYPE_EFS) ||
         DCHP_CLIENT_STORAGE_VERSION != version ) /*lint !e572 */
    {
      dhcpi_client_storage_version_init( info );
    }
    if ( -1 == mcfg_fclose( efs_ret,MCFG_FS_TYPE_EFS ) )
    {
      LOG_MSG_ERROR_0( "Failed to close version file" );
    }
  }
  /* Now if we have a valid version, open the lease file */
  if ( TRUE == info->efs_valid )
  {
    (void) snprintf(lease_filename, DHCP_CLIENT_LEASE_FILE_LEN - 1,
                    DHCP_CLIENT_LEASE_FILE_BASE "%08lx", info->init.iface);
    efs_ret = mcfg_fopen( lease_filename, 
                          O_RDWR | O_CREAT, 
                          S_IWRITE|S_IREAD,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE );
    if ( -1 == efs_ret )
    {
      info->efs_valid = FALSE;
      LOG_MSG_ERROR_0("Failed to open lease file");
    }
    else
    {
      info->lease_handle = efs_ret;
    }
  }
  info->cookie_ptr = info;
  return TRUE;
} /* dhcp_clienti_storage_start */

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_STOP

Description:
  This function stops a DHCP client storage layer object and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.

Arguements:
  void ** handle - The handle to the storage layer object to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_storage_new().
===========================================================================*/
void
dhcp_clienti_storage_stop
(
  void ** handle
)
{
  dhcp_client_storage_info * info;

  /* sanity check parameters */
  if (NULL == handle)
  {
    DATA_ERR_FATAL("handle pointer NULL?");
    return;
  }
  info = *handle;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if (DHCP_CLIENT_STORAGE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_STORAGE_COOKIE);
    ASSERT(0);
    return;
  }
  if (info != info->cookie_ptr)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }

  if ( TRUE ==  info->efs_valid )
  {
    if ( -1 == mcfg_fclose( info->lease_handle,MCFG_FS_TYPE_EFS ) )
    {
      LOG_MSG_ERROR_1("Failed to close lease handle %d", info->lease_handle);
    }
  }
  info->init.d_free(info);
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT STORAGE STOP ---" );
  *handle = NULL;
} /* dhcp_clienti_storage_stop */

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_GET_LEASE

Description:
  This function reads the current lease information for the given handle and
  returns it.   Lease information is stored by dss_iface_id_type.

Arguements:
  void * handle - Handle to the storage object.
  dhcp_client_lease * lease - The lease to fill in and return.

Return value:
  boolean - TRUE if lease found, valid and returned, FALSE otherwise.
  dhcp_client_lease * lease - Filled out lease info if found and valid.

Dependencies:
  The storage handle must have been allocated by dhcp_clienti_storage_new() and
  started by dhcp_clienti_storage_start().
  The lease pointer must point to valid memory to be filled in.
===========================================================================*/
boolean
dhcp_clienti_storage_get_lease
(
  void * handle,
  dhcp_client_lease * lease
)
{
  dhcp_client_storage_info * info = handle;
  fs_off_t efs_off_ret;
  fs_ssize_t efs_size_ret;
  uint8  byte_val;
  uint32 word_val;

  /* sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if (DHCP_CLIENT_STORAGE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_STORAGE_COOKIE);
    ASSERT(0);
    return FALSE;
  }
  if (info != info->cookie_ptr)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return FALSE;
  }
  if ( NULL == lease )
  {
    LOG_MSG_ERROR_0("lease NULL?");
    ASSERT(0);
    return FALSE;
  }

  if ( FALSE == info->efs_valid )
  {
    LOG_MSG_INFO1_0("In get_lease with not valid");
    return FALSE;
  }
  /* Seek to beginning of file, in case we were not there. */
  efs_off_ret = mcfg_lseek( info->lease_handle, 0, SEEK_SET,MCFG_FS_TYPE_EFS );
  if ( 0 != efs_off_ret )
  {
    LOG_MSG_ERROR_2("get_lease lseek failed %d %d",
                    efs_off_ret, info->lease_handle );
    return FALSE;
  }
  /* read in the lease information. If the format of anything is this
   * changes, go bump the version number at the top of this file!
   */
  efs_size_ret = mcfg_fread( info->lease_handle, 
                             &byte_val, 
                             sizeof(uint8), 
                             MCFG_FS_TYPE_EFS );
  if ( efs_size_ret != (fs_ssize_t) sizeof(uint8) )
  {
    LOG_MSG_INFO1_0("Failed to read valid field");
    return FALSE;
  }
  lease->valid = byte_val;
  efs_size_ret = mcfg_fread( info->lease_handle, 
                             &word_val, sizeof(uint32), 
                             MCFG_FS_TYPE_EFS );
  if ( efs_size_ret != (fs_ssize_t) sizeof(uint32) )
  {
    LOG_MSG_INFO1_0("Failed to read ip_addr field");
    return FALSE;
  }
  lease->ip_addr = word_val;
  efs_size_ret = mcfg_fread( info->lease_handle, 
                             &word_val, sizeof(uint32),
                             MCFG_FS_TYPE_EFS );
  if ( efs_size_ret != (fs_ssize_t) sizeof(uint32) )
  {
    LOG_MSG_INFO1_0("Failed to read server_id field");
    return FALSE;
  }
  lease->server_id = word_val;
  return TRUE;
} /* dhcp_clienti_storage_get_lease */

/*===========================================================================
FUNCTION DHCP_CLIENTI_STORAGE_SET_LEASE

Description:
  This function sets the lease for the given handle.  This function can also
  be used to mark a lease invalid by passing the lease pointer as NULL.
  Lease information is stored by dss_iface_id_type.

Arguements:
  void * handle - Handle to the storage object.
  dhcp_client_lease * lease - The lease to store.

Return value:
  None.

Dependencies:
  The storage handle must have been allocated by dhcp_clienti_storage_new() and
  started by dhcp_clienti_storage_start().
===========================================================================*/
void
dhcp_clienti_storage_set_lease
(
  void * handle,
  dhcp_client_lease * lease
)
{
  dhcp_client_storage_info * info = handle;
  fs_off_t efs_off_ret;
  fs_ssize_t efs_size_ret;
  uint8 valid; /* Because lint and 2.1 are borken */

  /* sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if (DHCP_CLIENT_STORAGE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_STORAGE_COOKIE);
    ASSERT(0);
    return;
  }
  if (info != info->cookie_ptr)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }
  if ( NULL == lease )
  {
    DATA_ERR_FATAL("lease NULL?");
    return;
  }

  if ( FALSE == info->efs_valid )
  {
    LOG_MSG_INFO1_0("In set_lease with not valid");
    return;
  }
  /* Seek to beginning of file, in case we were not there. */
  efs_off_ret = mcfg_lseek( info->lease_handle, 
                            0, 
                            SEEK_SET, 
                            MCFG_FS_TYPE_EFS );
  if ( 0 != efs_off_ret )
  {
    LOG_MSG_ERROR_2("set_lease lseek failed %d %d",
                    efs_off_ret, info->lease_handle );
    return;
  }
  /* write out the lease information. If the format of anything is
   * this changes, go bump the version number at the top of this file!
   * Also, this must match with the read above.
   */
  valid = (uint8) lease->valid;
  efs_size_ret = mcfg_fwrite(info->lease_handle, 
                             &valid, 
                             sizeof(uint8), 
                             MCFG_FS_TYPE_EFS);
  if ( efs_size_ret != (fs_ssize_t) sizeof(uint8) )
  {
    LOG_MSG_INFO1_0("Failed to write valid field");
    dhcpi_client_storage_write_recover(info);
    return;
  }
  efs_size_ret = mcfg_fwrite(info->lease_handle, 
                             &lease->ip_addr, 
                             sizeof(uint32), 
                             MCFG_FS_TYPE_EFS);
  if ( efs_size_ret != (fs_ssize_t) sizeof(uint32) )
  {
    LOG_MSG_INFO1_0("Failed to write ip_addr field");
    dhcpi_client_storage_write_recover(info);
    return;
  }
  efs_size_ret = mcfg_fwrite( info->lease_handle, 
                              &lease->server_id,
                              sizeof(uint32), 
                              MCFG_FS_TYPE_EFS );
  if ( efs_size_ret != (fs_ssize_t) sizeof(uint32) )
  {
    LOG_MSG_INFO1_0("Failed to write server_id field");
    dhcpi_client_storage_write_recover(info);
    return;
  }
  return;
} /* dhcp_clienti_storage_set_lease */
