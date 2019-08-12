
/*===========================================================================

                        D S U M T S  S U B S  M G R 

                          
GENERAL DESCRIPTION
  This file contains data and function definitions specific to Dual Slot
  Dual Standby Feature. This modeule is included in PDP Profile registry 
  if the Dual SIM feature is enabled
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  dsumts_subs_mgr_init() should be called at task / system startup. 

 
Copyright (c) 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/dsprofile/src/dsumts_subs_mgr.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/29/11   ttv     Merged the fix for crash due to invalid EFS contents.
02/16/11   ttv     Added DSDS changes for consolidated profile family.
11/22/10   ttv     Creation of file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include <stringl/stringl.h>

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE)
#include "msg.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "cm.h"
#include "dsumts_subs_mgr.h"
#include "dsumtspdpreg.h"
#include "dsumtspdpregint.h"
#include "ds_3gppi_utils.h"
#include "amssassert.h"
#include "modem_mem.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*---------------------------------------------------------------------------
  Maximum number of families.
---------------------------------------------------------------------------*/
#if 0
#define DSUMTS_SUBS_MGR_MAX_NUM_PROFILE_FAMILY 2

#define DSUMTS_SUBS_MGR_PROFILE_NUM_DIR_NAME \
               "pdp_profiles" 

#define DSUMTS_SUBS_MGR_PROFILE_NUM_FOR_EMBEDDED_FILE_NAME \
               "embedded_call_prof_nums"

#define DSUMTS_SUBS_MGR_PROFILE_NUM_FOR_TETHERED_FILE_NAME \
               "tethered_call_prof_nums"

#define DSUMTS_SUBS_MGR_DIR_LEN 12

#define DSUMTS_SUBS_MGR_EMBEDDED_FILE_NAME_LEN 23

#define DSUMTS_SUBS_MGR_TETHERED_FILE_NAME_LEN 23

#ifndef FEATURE_DUAL_SIM
#define DSUMTS_SUBS_MGR_MAX_NUM_DEF_PROFILES (DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_MAX)
#else
#define DSUMTS_SUBS_MGR_MAX_NUM_DEF_PROFILES (DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_MAX - 1)
#endif /* FEATURE_DUAL_SIM */

#define DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX 0

#define DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX 1

#define DSUMTS_SUBS_MGR_SUBS1_INDEX 0

#define DSUMTS_SUBS_MGR_SUBS2_INDEX 1

#define DSUMTS_SUBS_MGR_SUBS3_INDEX 2

#define DSUMTS_SUBS_MGR_MAX_PROFILE_LEN 2

#define DSUMTS_SUBS_MGR_MAX_PDP_PROFILE_NUM 16

#define IS_VALID_PROFFILE_NUM(profile_number)\
           ( (profile_number >= 0) &&\
             (profile_number <= DSUMTS_SUBS_MGR_MAX_PDP_PROFILE_NUM) )

/*---------------------------------------------------------------------------
  BUFFER_SIZE = 2 + 1 + 2 + 1 ("12,13\0")
  Both 2s are the size of profile numbers.
  First 1 is for comma and second 1 is for null character. 
---------------------------------------------------------------------------*/
#define BUFFER_SIZE (3 * (DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_MAX - 1))

typedef struct
{
  ds_umts_pdp_subs_e_type             active_subs_id;
  byte                                def_profile_num;
} dsumts_subs_cached_profile_mapping_table;

static dsumts_subs_cached_profile_mapping_table 
         subs_def_profile_info[DSUMTS_SUBS_MGR_MAX_NUM_PROFILE_FAMILY]
                              [DSUMTS_SUBS_MGR_MAX_NUM_DEF_PROFILES] 
                                = {{{DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE, 0}}};

/*---------------------------------------------------------------------------
  Cosolidated prfoile directory length + length for + 
  length for NULL character 
---------------------------------------------------------------------------*/
static char dsumts_subs_mgr_dir_name[\
               DSUMTS_SUBS_MGR_DIR_LEN + \
               1 + 1] = {0};

/*---------------------------------------------------------------------------
  Fully qualified EFS file name for the embedded call profile number file
  i.e., "pdp_profiles/embedded_call_prof_nums_dsds
---------------------------------------------------------------------------*/
static char dsumts_subs_mgr_fully_qual_embedded_file_name[\
              DSUMTS_SUBS_MGR_DIR_LEN + \
              1 + \
              DSUMTS_SUBS_MGR_EMBEDDED_FILE_NAME_LEN + \
              1 ] = {0};

/*---------------------------------------------------------------------------
  Fully qualified EFS file name for the tethered call profile number file
  i.e., "pdp_profiles/tethered_call_prof_nums_dsds"
---------------------------------------------------------------------------*/
static char dsumts_subs_mgr_fully_qual_tethered_file_name[\
              DSUMTS_SUBS_MGR_DIR_LEN + \
              1 + \
              DSUMTS_SUBS_MGR_TETHERED_FILE_NAME_LEN + \
              1 ] = {0};

static boolean dsumts_subs_mgr_cache_value_valid[DSUMTS_SUBS_MGR_MAX_NUM_PROFILE_FAMILY] = 
                      {FALSE, FALSE};

/*===========================================================================
                    Local Function Definitions
===========================================================================*/
/*===========================================================================
FUNCTION DSUMTS_SUBS_MGR_EFS_WRITE

DESCRIPTION
This function invokes efs_write and checks to make sure the number of bytes
written matches the requested number of bytes.

PARAMETERS
fd - file descriptor of the file from which to write
*buf - pointer to the bytes of data written
nbyte - number of bytes written

DEPENDENCIES
  none

RETURN VALUE
FALSE - nbyte mismatch with efs_write return value.
TRUE - SUCCESS

SIDE EFFECTS
  none
===========================================================================*/
static boolean dsumts_subs_mgr_efs_write
(
  int fd,
  const void *buf,
  uint32 nbyte
)
{
  fs_ssize_t fs_res;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------- 
  efs_write returns number of bytes written if successful, -1 indicates an
  error and 0 means the file system is full.
  ----------------------------------------------------------------------*/
  fs_res = efs_write (fd, buf, (fs_size_t)nbyte);
  if (fs_res == -1) 
  {
    DS_3GPP_MSG3_ERROR("efs_write failed for fd=%d errno=%d", fd, efs_errno, 0);
    return FALSE;
  }
  /*lint -e737*/
  else if (fs_res != nbyte)
  {
    DS_3GPP_MSG3_ERROR("efs_write wrote wrong num of bytes=%d errno=%d", fs_res, efs_errno, 0);
    return FALSE;
  }
  /*lint +e737*/
  return TRUE;
}

#if 0
/*===========================================================================
FUNCTION DSUMTS_SUBS_MGR_EFS_READ

DESCRIPTION
This function invokes efs_read and checks to make sure the number of bytes
read matches the requested number of bytes.

PARAMETERS
fd - file descriptor of the file to be read
*buf - pointer to the bytes of data read
nbyte - number of bytes to be read

DEPENDENCIES
  none

RETURN VALUE
FALSE - Read nbyte mismatch with efs_read return value.
TRUE - SUCCESS

SIDE EFFECTS
  none
===========================================================================*/
static boolean dsumts_subs_mgr_efs_read
(
  int fd,
  void *buf,
  uint32 nbyte
)
{   
  fs_ssize_t fs_res;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------- 
  efs_read returns number of bytes read if successful, -1 indicates an
  error and 0 means end of file has been reached.
  ----------------------------------------------------------------------*/
  fs_res = efs_read (fd, buf, (fs_size_t)nbyte);
  if (fs_res == -1) 
  {
    DS_3GPP_MSG3_ERROR("efs_read failed for fd=%d errno=%d", fd, efs_errno, 0);
    return FALSE;
  }
  /*lint -e737*/
  else if (fs_res != nbyte)
  {
    DS_3GPP_MSG3_ERROR("efs_read read wrong num of bytes=%d errno=%d", fs_res, efs_errno, 0);
    return FALSE;
  }
  /*lint +e737*/
  return TRUE;
}

/*===========================================================================
FUNCTION DSUMTS_SUBS_MGR_READ_PROF_NUMS

DESCRIPTION
 This function reads the profile numbers from a given string, which are 
 seperated by comma.

PARAMETERS 
 buffer       : string, which contains profile numbers
 length       : length of the buffer
 prof_num_ptr : [OUT] stores the profile numbers 
 num_profiles : number of profile numbers needed to be read from buffer

DEPENDENCIES 
 None

RETURN VALUE 
  TRUE  - reading was successfull
  FALSE - reading failed

SIDE EFFECTS 
  None
===========================================================================*/
static boolean dsumts_subs_mgr_read_prof_nums
(
  char*  buffer,
  uint8  length,
  uint8* prof_num_ptr,
  uint8  num_profiles
)
{
  char temp_buffer[DSUMTS_SUBS_MGR_MAX_PROFILE_LEN+1]    = {0};
  int  buffer_index      = 0;
  int  prof_num_count    = 0;
  int  temp_buffer_index = 0;
  int  temp_profile_num  = 0;

  if( (buffer == NULL) || (prof_num_ptr == NULL) || (length == 0) )
  {
    DS_3GPP_MSG3_ERROR("Invalid arguments passed.", 0, 0, 0);
    return FALSE;
  }

/*-----------------------------------------------------------------------------
  The following logic will retrieve profile numbers from a string.
  Example: if the input is "12,3", this will store numbers 12 and 3 in
  profile_num_ptr.
-----------------------------------------------------------------------------*/

  for( buffer_index = 0; 
       buffer_index < length && prof_num_count < num_profiles &&
       temp_buffer_index <= DSUMTS_SUBS_MGR_MAX_PROFILE_LEN; 
       buffer_index++)
  {
    if( *( buffer + buffer_index ) == '\0' ||
         *( buffer + buffer_index ) == ',')
    {
      if( (temp_buffer_index > 0) &&
          ((temp_profile_num = atoi(temp_buffer))!=0) &&
          (IS_VALID_PROFFILE_NUM(temp_profile_num)) )
      {
        prof_num_ptr[prof_num_count] = temp_profile_num;
        prof_num_count++;
      }
      else
      {
        DS_3GPP_MSG3_ERROR("Profile number is invalid"
                  "temp_profile_num = %d", temp_profile_num, 0, 0);
        return FALSE;
      }

      if( *( buffer + buffer_index ) == '\0' )
      {
        break;
      }
      else
      {
        memset( temp_buffer, 0x0, sizeof(temp_buffer) );
        temp_buffer_index = 0;
      }
    }
    else
    {
      temp_buffer[temp_buffer_index++] = *( buffer + buffer_index );
    }
  }
    
  if(prof_num_count != num_profiles)
  {
    DS_3GPP_MSG3_ERROR("Invalid EFS contents. num_profiles = %d, prof_num_count = %d", 
              num_profiles, prof_num_count, 0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION DSUMTS_SUBS_MGR_CACHE_MAPPING_INFO

DESCRIPTION
  This function reads the content of the default profile files and stores it
  in a structure for temporary future use
   
PARAMETERS 
  file_name  : name of the file where default profile numbers are stored.
  family     : embedded or tethered.

DEPENDENCIES 
 None

RETURN VALUE 
  TRUE  - caching was successfull
  FALSE - caching failed

SIDE EFFECTS 
  None
===========================================================================*/
static boolean dsumts_subs_mgr_cache_mapping_info
(
  const char                    file_name[],
  ds_umts_profile_family_e_type family
)
{
  uint8      subs_index;
  int        fs_handle;
  char       file_buffer[BUFFER_SIZE];
  uint8      def_profile[DSUMTS_SUBS_MGR_MAX_NUM_DEF_PROFILES] = { 0 };

  DS_3GPP_MSG3_MED("Cache Mapping table information",0,0,0);

  memset( file_buffer, 0, sizeof(file_buffer) );
  fs_handle = efs_open( file_name, O_RDONLY );
  if ( fs_handle == -1)
  {
    DS_3GPP_MSG3_ERROR("efs_open failed for %d errno=%d.", fs_handle, efs_errno, 0);
    return FALSE;
  }

  /*-------------------------------------------------------------------
    read the contents of the file : 2 comma separated values are 
    stored. Hence 3 bytes.
    -------------------------------------------------------------------*/
  if (!dsumts_subs_mgr_efs_read(fs_handle,
                            (void*)file_buffer,
                            (BUFFER_SIZE - 1)*sizeof(byte)))
  {
    return FALSE;
  }
  
  /*-------------------------------------------------------------------
    Extract the contents of file buffer
  -------------------------------------------------------------------*/
  if( !dsumts_subs_mgr_read_prof_nums(
                                      file_buffer,
                                      (uint8)sizeof(file_buffer),
                                      def_profile,
                                      (uint8)DSUMTS_SUBS_MGR_MAX_NUM_DEF_PROFILES) )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------
  Close the file
  -------------------------------------------------------------------*/
  if (efs_close (fs_handle) == -1)
  {
    DS_3GPP_MSG3_ERROR("efs_close failed for %d errno = %d", fs_handle, efs_errno, 0);
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Update the cached structure for later use
  ------------------------------------------------------------------------ */
 for (subs_index = 0; 
       subs_index < DSUMTS_SUBS_MGR_MAX_NUM_DEF_PROFILES; subs_index++)
  {
    subs_def_profile_info[family][subs_index].active_subs_id = 
        (ds_umts_pdp_subs_e_type)(subs_index + 1);
    subs_def_profile_info[family][subs_index].def_profile_num = 
         def_profile[subs_index];
  }

  dsumts_subs_mgr_cache_value_valid[family] = TRUE;

#ifdef FEATURE_TRIPLE_SIM
  DS_3GPP_MSG3_MED("Caching completed for TRIPLE SIM",0,0,0);
#else
  DS_3GPP_MSG3_MED("Caching completed for DUAL SIM", 0, 0, 0);
#endif 
  return TRUE;
}

#endif

/*===========================================================================
FUNCTION DSUMTS_SUBS_MGR_CREATE_AND_INIT_SUBS_MAPPING_INFO_FOR_EMBEDDED

DESCRIPTION
  Internal function to create the default profile file for embedded calls 
  in EFS if it is not present already. If it is already present, it just 
  caches the value in a structure.
   
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  TRUE  - initialization was successfull
  FALSE - initialization failed

SIDE EFFECTS 
  None
===========================================================================*/
static boolean dsumts_subs_mgr_create_and_init_mapping_info_for_embedded( void )
{
  uint8          subs_index;

  DS_3GPP_MSG3_MED("Create and Init mapping info for Embedded calls",0,0,0);
  /*lint -e534 ignoring return code from function*/
#if 0
  if(strlcpy(dsumts_subs_mgr_dir_name,
                 DSUMTS_SUBS_MGR_PROFILE_NUM_DIR_NAME,
                 sizeof(dsumts_subs_mgr_dir_name) ) >=
     (int)sizeof(dsumts_subs_mgr_dir_name) )
  {
    return FALSE;
  }
  
  dsumts_subs_mgr_dir_name [DSUMTS_SUBS_MGR_DIR_LEN] = DS_FS_DIRECTORY_CHAR;
  dsumts_subs_mgr_dir_name [DSUMTS_SUBS_MGR_DIR_LEN +1] = '\0';
  /*lint +e534*/

  /*-------------------------------------------------------------------------
    Init the fully qualified profile file name to 
    pdp_profiles/ directory
  -------------------------------------------------------------------------*/
  /*lint -e534 ignoring return code from function*/
  if( strlcpy( dsumts_subs_mgr_fully_qual_embedded_file_name,
                   dsumts_subs_mgr_dir_name,
                   sizeof(dsumts_subs_mgr_fully_qual_embedded_file_name) ) >=
     (int)sizeof(dsumts_subs_mgr_fully_qual_embedded_file_name) )
  {
    return FALSE;
  }

  strlcat( dsumts_subs_mgr_fully_qual_embedded_file_name,
               DSUMTS_SUBS_MGR_PROFILE_NUM_FOR_EMBEDDED_FILE_NAME,
               sizeof(dsumts_subs_mgr_fully_qual_embedded_file_name) );
  /*lint +e534*/

  efs_res = efs_stat((char*) dsumts_subs_mgr_fully_qual_embedded_file_name, 
                     &sbuf);
  
  if( ( efs_res != 0 ) ||
      (FALSE == dsumts_subs_mgr_cache_mapping_info(
                       dsumts_subs_mgr_fully_qual_embedded_file_name, 
                       DS_UMTS_EMBEDDED_PROFILE_FAMILY)) )
  {
    /*-----------------------------------------------------------------------
      Profile file is not present. Create the profile and write profile nums
    -----------------------------------------------------------------------*/

    fs_handle = efs_open(dsumts_subs_mgr_fully_qual_embedded_file_name, 
                         O_CREAT | O_TRUNC | O_WRONLY,
                         DEFFILEMODE);
    if ( fs_handle == -1)
    {
      DS_3GPP_MSG3_ERROR("efs_open failed for fd=%d errno=%d.", fs_handle, efs_errno, 0);
      return FALSE;
    }
  
    /*-------------------------------------------------------------------------
      Write the default profile numbers for embedded call
      Format of file contents:
      <Default profile number for Subs1>,<Default profile number for Subs2>
      (eg) 3,2
    -------------------------------------------------------------------------*/
#ifdef FEATURE_TRIPLE_SIM
    (void)snprintf(file_buffer,
                         BUFFER_SIZE*sizeof(byte),
                         "%d,%d,%d",
                         DSUMTS_DEF_PROFILE_FOR_EMBEDDED_SUB1,
                         DSUMTS_DEF_PROFILE_FOR_EMBEDDED_SUB2,
                         DSUMTS_DEF_PROFILE_FOR_EMBEDDED_SUB3);
#else
    (void)snprintf(file_buffer,
                         BUFFER_SIZE*sizeof(byte),
                         "%d,%d",
                         DSUMTS_DEF_PROFILE_FOR_EMBEDDED_SUB1,
                         DSUMTS_DEF_PROFILE_FOR_EMBEDDED_SUB2); 
#endif /* FEATURE_TRIPLE_SIM */

    if ( dsumts_subs_mgr_efs_write(fs_handle,
                         (void *)(&file_buffer[0]),
                         (BUFFER_SIZE - 1)*sizeof(byte)) == FALSE )
    {
      return FALSE;
    }
  
    /*-------------------------------------------------------------------------
      Close the file
    -------------------------------------------------------------------------*/
    if (efs_close (fs_handle) == -1)
    {
      DS_3GPP_MSG3_ERROR("efs_close failed for fd=%d errno=%d", fs_handle, efs_errno, 0);
      return FALSE;
    }
#endif

    DS_3GPP_MSG3_HIGH("Subscription based Embedded file created",0,0,0);
    /*------------------------------------------------------------------------
      Update the cached structure for later use
    ------------------------------------------------------------------------ */
    for (subs_index = 0; 
          subs_index < DSUMTS_SUBS_MGR_MAX_NUM_DEF_PROFILES; subs_index++)
    {
      subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][subs_index].active_subs_id = 
           (ds_umts_pdp_subs_e_type)(subs_index + 1);
      subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][subs_index].def_profile_num = 
           (subs_index + 1);
    }

    dsumts_subs_mgr_cache_value_valid[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX] = TRUE;

  return TRUE;
}/* dsumts_subs_mgr_create_and_init_mapping_info_embedded */

/*===========================================================================
FUNCTION DSUMTS_SUBS_MGR_CREATE_AND_INIT_SUBS_MAPPING_INFO_FOR_TETHERED

DESCRIPTION
  Internal function to create the default profile file for tethered calls
  in EFS if it is not present already. If it is already present, it just 
  caches the value in a structure.
   
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  TRUE  - initialization was successfull
  FALSE - initialization failed

SIDE EFFECTS 
  None
===========================================================================*/
static boolean dsumts_subs_mgr_create_and_init_mapping_info_for_tethered( void )
{
  uint8          subs_index;

  DS_3GPP_MSG3_MED("Create and Init mapping info for Tethered calls",0,0,0);
  /*lint -e534 ignoring return code from function*/
#if 0
  if(strlcpy(dsumts_subs_mgr_dir_name,
                 DSUMTS_SUBS_MGR_PROFILE_NUM_DIR_NAME,
                 sizeof(dsumts_subs_mgr_dir_name) ) >=
     (int)sizeof(dsumts_subs_mgr_dir_name) )
  {
    return FALSE;
  }
  
  dsumts_subs_mgr_dir_name [DSUMTS_SUBS_MGR_DIR_LEN] = DS_FS_DIRECTORY_CHAR;
  dsumts_subs_mgr_dir_name [DSUMTS_SUBS_MGR_DIR_LEN +1] = '\0';
  /*lint +e534*/

   /*-------------------------------------------------------------------------
    Init the fully qualified profile file name to pdp_profiles/ directory
  -------------------------------------------------------------------------*/
  /*lint -e534 ignoring return code from function*/
  if(strlcpy(dsumts_subs_mgr_fully_qual_tethered_file_name,
         dsumts_subs_mgr_dir_name,
         sizeof(dsumts_subs_mgr_fully_qual_tethered_file_name) ) >=
         (int)sizeof(dsumts_subs_mgr_fully_qual_tethered_file_name) )
  {
    return FALSE;
  }

  strlcat( dsumts_subs_mgr_fully_qual_tethered_file_name,
               DSUMTS_SUBS_MGR_PROFILE_NUM_FOR_TETHERED_FILE_NAME,
               sizeof(dsumts_subs_mgr_fully_qual_tethered_file_name) );
  /*lint +e534*/

  efs_res = efs_stat((char*) dsumts_subs_mgr_fully_qual_tethered_file_name, 
                     &sbuf);
  
  if( (efs_res != 0) || 
      (FALSE == dsumts_subs_mgr_cache_mapping_info(
                       dsumts_subs_mgr_fully_qual_tethered_file_name, 
                       DS_UMTS_TETHERED_PROFILE_FAMILY)) )
  {
    /*-----------------------------------------------------------------------
      Profile file is not present. Create the profile and write profile nums
    -----------------------------------------------------------------------*/

    fs_handle = efs_open(dsumts_subs_mgr_fully_qual_tethered_file_name, 
                         O_CREAT | O_TRUNC | O_WRONLY,
                         DEFFILEMODE);
    if ( fs_handle == -1)
    {
      DS_3GPP_MSG3_ERROR("efs_open failed for fd=%d errno=%d.", fs_handle, efs_errno, 0);
      return FALSE;
    }
  
    /*-------------------------------------------------------------------------
      Write the default profile numbers for tethered call
      Format of file contents:
      <Default profile number for Subs1>,<Default profile number for Subs2>
      (eg) 3,2
    -------------------------------------------------------------------------*/
#ifdef FEATURE_TRIPLE_SIM
    (void)snprintf(file_buffer,
                         BUFFER_SIZE*sizeof(byte),
                         "%d,%d,%d", 
                         DSUMTS_DEF_PROFILE_FOR_TETHERED_SUB1,
                         DSUMTS_DEF_PROFILE_FOR_TETHERED_SUB2,
                         DSUMTS_DEF_PROFILE_FOR_TETHERED_SUB3
                        );
#else
    (void)snprintf(file_buffer,
                         BUFFER_SIZE*sizeof(byte),
                         "%d,%d",
                         DSUMTS_DEF_PROFILE_FOR_TETHERED_SUB1,
                         DSUMTS_DEF_PROFILE_FOR_TETHERED_SUB2
                        );
#endif /* FEATURE_TRIPLE_SIM */

    if ( dsumts_subs_mgr_efs_write(fs_handle,
                         (void *)(&file_buffer[0]),
                         (BUFFER_SIZE - 1)*sizeof(byte)) == FALSE )
    {
      return FALSE;
    }
  
    /*-------------------------------------------------------------------------
      Close the file
    -------------------------------------------------------------------------*/
    if (efs_close (fs_handle) == -1)
    {
      DS_3GPP_MSG3_ERROR("efs_close failed for fd=%d errno=%d", fs_handle, efs_errno, 0);
      return FALSE;
    }

#endif
    DS_3GPP_MSG3_HIGH("Subscription based tethered file created",0,0,0);
    /*------------------------------------------------------------------------
      Update the cached structure for later use
    ------------------------------------------------------------------------ */
    for (subs_index = 0; 
          subs_index < DSUMTS_SUBS_MGR_MAX_NUM_DEF_PROFILES; subs_index++)
    {
      subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][subs_index].active_subs_id = 
           (ds_umts_pdp_subs_e_type)(subs_index + 1);
      subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][subs_index].def_profile_num = 
           (subs_index + 1);
    }

    dsumts_subs_mgr_cache_value_valid[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX] = TRUE;

  return TRUE;
}/* dsumts_subs_mgr_create_and_init_mapping_info_tethered */

/*===========================================================================
                    Forward Function Definitions
===========================================================================*/

/*===========================================================================
FUNCTION DSUMTS_SUBS_MGR_INIT

DESCRIPTION
  This function initializes the DS Subscription manager. It creates the mapping 
  table in EFS with default profile numbers for embedded/tethered calls.
   
PARAMETERS 
  none

DEPENDENCIES 
  Should be called only ONCE during startup.

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS  - initialization was successfull
  DS_UMTS_PDP_FAIL     - initialization failed

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype dsumts_subs_mgr_init(void)
{
  /*-------------------------------------------------------------
    Init mapping table for embedded and tethered calls
  --------------------------------------------------------------*/
  if( dsumts_subs_mgr_create_and_init_mapping_info_for_embedded() &&
      dsumts_subs_mgr_create_and_init_mapping_info_for_tethered() )
  {
    DS_3GPP_MSG3_MED("DSUMTS SUBS MGR Init complete",0,0,0);
    return DS_UMTS_PDP_SUCCESS;
  }
  else {
    return DS_UMTS_PDP_FAIL;
  }
  
}/* dsumts_subs_mgr_init */

#endif
/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_SUBS_ID

DESCRIPTION

  This function returns the subscription ID internal to Data services that maps
  to the id from CM.
  CM : DS subscription ID mapping
  SYS_MODEM_AS_ID_1 : DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1
  SYS_MODEM_AS_ID_2 : DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_2
  other             : DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE


PARAMETERS
  as_id   - Subscription ID from CM

DEPENDENCIES
  None

RETURN VALUE
 pdp subscription type

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_subs_e_type dsumts_subs_mgr_get_subs_id
(
  sys_modem_as_id_e_type as_id
)
{
  switch(as_id) {
    case SYS_MODEM_AS_ID_1:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1;

    case SYS_MODEM_AS_ID_2:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_2;

#ifdef FEATURE_TRIPLE_SIM
    case SYS_MODEM_AS_ID_3:
     return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_3; 
#endif

    default:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  }
}/* dsumts_subs_mgr_get_subs_id */


/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_SUBS_ID_FROM_DB_SUBS_ID

DESCRIPTION

  This function returns ds_profile_db_subs_type from internal
  ds_umts_pdp_subs_e_type


PARAMETERS
  ds_umts_pdp_subs_e_type   -- Internal Subscription ID

DEPENDENCIES
  None

RETURN VALUE
 ds_profile_subs_etype   -ds profile subscrition type

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_subs_e_type dsumts_subs_mgr_get_subs_id_from_ds_profile_subs_id
(
   ds_profile_subs_etype subs_id
)
{
   switch(subs_id) 
   {
    case  DB_PROFILE_ACTIVE_SUBSCRIPTION_1:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1;

    case  DB_PROFILE_ACTIVE_SUBSCRIPTION_2:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_2;

#ifdef FEATURE_TRIPLE_SIM
    case  DB_PROFILE_ACTIVE_SUBSCRIPTION_3:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_3;
#endif

    default:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  }
}

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_AS_ID

DESCRIPTION

  This function returns sys_modem_as_id_e_type from internal
  ds_umts_pdp_subs_e_type
  CM : DS subscription ID mapping
  SYS_MODEM_AS_ID_1 : DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1
  SYS_MODEM_AS_ID_2 : DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_2
  other             : DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE


PARAMETERS
  as_id   - Subscription ID from CM

DEPENDENCIES
  None

RETURN VALUE
 pdp subscription type

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type dsumts_subs_mgr_get_as_id
(
   ds_umts_pdp_subs_e_type subs_id
)
{
  switch(subs_id) {
    case  DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1:
      return SYS_MODEM_AS_ID_1;

    case  DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_2:
      return SYS_MODEM_AS_ID_2;

#ifdef FEATURE_TRIPLE_SIM
    case  DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_3:
     return SYS_MODEM_AS_ID_3; 
#endif

    default:
      return SYS_MODEM_AS_ID_NONE;
  }
}/* dsumts_subs_mgr_get_subs_id */


/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_SUBS_ID_FROM_DB_SUBS_ID

DESCRIPTION

  This function returns ds_umts_pdp_subs_e_type from 
  ds_profile_db_subs_type


PARAMETERS
  ds_profile_db_subs_type   -ds profile db subscrition type 

DEPENDENCIES
  None

RETURN VALUE
 ds_umts_pdp_subs_e_type   - Subscription ID from CM

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_subs_e_type dsumts_subs_mgr_get_subs_id_from_db_subs_id
(
   ds_profile_db_subs_type subs_id
)
{
  switch(subs_id) {
    case  DB_PROFILE_ACTIVE_SUBSCRIPTION_1:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1;

    case  DB_PROFILE_ACTIVE_SUBSCRIPTION_2:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_2;

#ifdef FEATURE_TRIPLE_SIM
    case  DB_PROFILE_ACTIVE_SUBSCRIPTION_3:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_3;
#endif

    default:
      return DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  }
}/* dsumts_subs_mgr_get_subs_id */


/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_SUBS_ID_FROM_DB_SUBS_ID 
 
DESCRIPTION

  This function returns ds_umts_pdp_subs_e_type from 
  ds_profile_db_subs_type


PARAMETERS
  uint32   - ps policy

DEPENDENCIES
  None

RETURN VALUE
 ds_umts_pdp_subs_e_type   -- Internal Subscription ID

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_subs_e_type dsumts_subs_mgr_get_subs_id_from_policy_subs_id
(
   uint32 subs_id
)
{
   return (ds_umts_pdp_subs_e_type)(subs_id - 1);
}

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_DB_SUBS_ID

DESCRIPTION

  This function returns ds_profile_db_subs_type from internal
  ds_umts_pdp_subs_e_type


PARAMETERS
  ds_umts_pdp_subs_e_type   - Subscription ID from CM

DEPENDENCIES
  None

RETURN VALUE
 ds_profile_db_subs_type   -ds profile db subscrition type

SIDE EFFECTS
  None

===========================================================================*/
ds_profile_db_subs_type dsumts_subs_mgr_get_db_subs_id
(
   ds_umts_pdp_subs_e_type subs_id
)
{
  switch(subs_id) {
    case  DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1:
      return DB_PROFILE_ACTIVE_SUBSCRIPTION_1;

    case  DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_2:
      return DB_PROFILE_ACTIVE_SUBSCRIPTION_2;

#ifdef FEATURE_TRIPLE_SIM
    case  DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_3:
     return DB_PROFILE_ACTIVE_SUBSCRIPTION_3; 
#endif

    default:
      return DB_ACTIVE_SUBSCRIPTION_NONE;
  }
}/* dsumts_subs_mgr_get_db_subs_id */

#if 0
/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_PROCESS_SUBS_CHANGE_EVENT

DESCRIPTION

  This function processes the events from CM that conveys a change in active
  subscription. Main processing is done in DSMGR. This function has the 
  umtsps specific additional processing

PARAMETERS
  as_id   - Active subscription ID from CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsumts_subs_mgr_process_subs_change_event
(
  sys_modem_as_id_e_type as_id
)
{
  uint16 profile_num;
  ds_umts_pdp_profile_status_etype result = DS_UMTS_PDP_FAIL;
  ds_umts_pdp_subs_e_type dsumts_data_subs;

  if(as_id == SYS_MODEM_AS_ID_NONE) {
    DS_3GPP_MSG3_ERROR("Active subscription is None",0,0,0);
    return;
  }

  /*----------------------------------------------------------------
  Get the internal DS subscription ID from as_id
  ------------------------------------------------------------------*/
  dsumts_data_subs = dsumts_subs_mgr_get_subs_id(as_id);
  DS_3GPP_MSG3_MED("DSUMTS SUBS MGR: Received Subscription change. "
          "Active Data subscription is %d", dsumts_data_subs, 0, 0);
  /*-----------------------------------------------------------------
    Get the default profile corresponding to dsumts_data_subs 
    for embedded
  ------------------------------------------------------------------*/
  result = dsumts_subs_mgr_get_default_profile_num_per_subs_for_embedded(
                           dsumts_data_subs, &profile_num );
  DS_3GPP_MSG3_HIGH("Default profile for embedded and subscription %d is %d ", 
           dsumts_data_subs, profile_num, 0);
  /*------------------------------------------------------------------
    Update dsumtspdpreg
  --------------------------------------------------------------------*/
  if (result == DS_UMTS_PDP_SUCCESS) {
    DS_3GPP_MSG3_MED("Updating the profile file for embedded in dsumtspdpreg",0,0,0);
    (void)ds_umts_set_pdp_profile_num_for_embedded_calls(profile_num);
  }
  else {
    DS_3GPP_MSG3_ERROR("Could not get default profile error: %d",result,0,0);
    return;
  }

  result = DS_UMTS_PDP_FAIL;
  /*-----------------------------------------------------------------
    Get the default profile corresponding to dsumts_data_subs 
    for tethered
  ------------------------------------------------------------------*/
  result = dsumts_subs_mgr_get_default_profile_num_per_subs_for_tethered(
                           dsumts_data_subs, &profile_num );
  DS_3GPP_MSG3_HIGH("Default profile for tethered and subscription %d is %d", 
           dsumts_data_subs, profile_num, 0);
  /*------------------------------------------------------------------
    Update dsumtspdpreg
  --------------------------------------------------------------------*/
  if (result == DS_UMTS_PDP_SUCCESS) 
  {
    DS_3GPP_MSG3_MED("Updating the profile filefor rmnet in dsumtspdpreg",0,0,0);
    (void)ds_umts_set_pdp_profile_num_for_tethered_calls(profile_num);
  }
  else 
  {
    DS_3GPP_MSG3_ERROR("Could not get default profile error: %d",result,0,0);
    return;
  }

}/* dsumts_subs_mgr_process_subs_change_event */

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_SET_DEFAULT_PROFILE_NUM_PER_SUBS_FOR_EMBEDDED

DESCRIPTION
  This function is called by dsumtspdpregint to set the default profile
  that the user specifies for each subscription

PARAMETERS
  profile_num  -   Holds the default profile number set by user
  subs_id      -   The subscription for which this profile is default

DEPENDENCIES
  None

RETURN VALUE
  DS_UMTS_PDP_FAIL          : suns id is invalid
  DS_UMTS_PDP_ACCESS_ERROR  : Error in updating the file
  DS_UMTS_PDP_SUCCESS       : The operation completed successfully

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_profile_status_etype 
   dsumts_subs_mgr_set_default_profile_num_per_subs_for_embedded
(
  ds_umts_pdp_subs_e_type           subs_id,
  uint16                            profile_num
)
{
  char                      file_buffer[BUFFER_SIZE] = {0};
  int                       fs_handle;
  int32                     file_size;
  byte                      profile_number;
  int                       index;
  uint32                    sub_index;

  profile_number = (byte) profile_num; 
  DS_3GPP_MSG3_MED("DSUMTS_SUBS_MGR: Set default profile per subs for embedded called",
          0,0,0);

  if( (subs_id <= DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE) || 
      (subs_id >= DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_MAX) ) 
  {
    DS_3GPP_MSG3_ERROR("Active subscription ID is invalid",0,0,0);
    return DS_UMTS_PDP_FAIL;
  }

  //Index starts at zero
  sub_index = ((uint32)subs_id - 1);

  /*-------------------------------------------------------------------------
    Init the fully qualified profile file name to pdp_profiles directory
  -------------------------------------------------------------------------*/

  if( strlcpy(dsumts_subs_mgr_fully_qual_embedded_file_name,
                  dsumts_subs_mgr_dir_name,
                  sizeof(dsumts_subs_mgr_fully_qual_embedded_file_name) ) >=
      (int)sizeof(dsumts_subs_mgr_fully_qual_embedded_file_name) )
  {
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

  (void)strlcat(dsumts_subs_mgr_fully_qual_embedded_file_name,
                    DSUMTS_SUBS_MGR_PROFILE_NUM_FOR_EMBEDDED_FILE_NAME,
                    sizeof(dsumts_subs_mgr_fully_qual_embedded_file_name));
  /*lint +e534*/    

  /*-------------------------------------------------------------------------
    Open the file for write 
  -------------------------------------------------------------------------*/
  fs_handle = efs_open(dsumts_subs_mgr_fully_qual_embedded_file_name,
                       O_RDWR);
  if ( fs_handle == -1)
  {
    DS_3GPP_MSG3_ERROR("efs_open failed fd=%d errno=%d.", fs_handle, efs_errno, 0);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
 /*--------------------------------------------------------------------------
    Get the subscription whose default profile has to be set
    Update only that profile number in EFS
    Below step will also cache the latest values for profile numbers 
  -------------------------------------------------------------------------*/
  subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][sub_index].def_profile_num = 
           profile_number;

  /*-------------------------------------------------------------------------
      Write the profile numbers specified by user
      Format of file contents:
      <Default profile number for Subs1>,<Default profile number for Subs2>
      (eg) 3,2
  -------------------------------------------------------------------------*/
  /* Clean the efs contents. */
  for(index = 0; index < BUFFER_SIZE; index++)
  {
    file_buffer[index] = '\0';
  }
  if ( dsumts_subs_mgr_efs_write(fs_handle,
                       (void *)(&file_buffer[0]),
                       (BUFFER_SIZE - 1)*sizeof(byte)) == FALSE )
  {
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

  file_size = efs_lseek(fs_handle,
                        0,
                        SEEK_SET);
  if (file_size == -1) 
  {
    DS_3GPP_MSG3_ERROR( "efs_lseek failed for %d errno=%d", fs_handle, efs_errno, 0 );
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

#ifdef FEATURE_TRIPLE_SIM
  (void)snprintf
           (
            file_buffer,
            BUFFER_SIZE*sizeof(byte),
            "%d,%d,%d", 
            subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS1_INDEX].def_profile_num,
            subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS2_INDEX].def_profile_num,
            subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS3_INDEX].def_profile_num
           );
#else
  (void)snprintf
           (
            file_buffer,
            BUFFER_SIZE*sizeof(byte),
            "%d,%d",
            subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS1_INDEX].def_profile_num,
            subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS2_INDEX].def_profile_num
           );
#endif  /* FEATURE_TRIPLE_SIM */

  if (dsumts_subs_mgr_efs_write(fs_handle,
                                (void *)(&file_buffer[0]),
                                (BUFFER_SIZE - 1)*sizeof(byte)) == FALSE)
  {
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

 /*--------------------------------------------------------------------------
    Truncate any contents after the current position if any
  -------------------------------------------------------------------------*/
  file_size = efs_lseek(fs_handle,
                        0,
                        SEEK_CUR);
  if (file_size == -1) 
  {
    DS_3GPP_MSG3_ERROR( "efs_lseek failed for %d errno=%d", fs_handle, efs_errno, 0 );
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

  /*lint -e737*/
  if( ((BUFFER_SIZE - 1)*sizeof(byte)) != file_size )
  {
    DS_3GPP_MSG3_ERROR( "config :file_pos/struct_size mismatch ",
               file_size, 0, 0 );
    (void)efs_close (fs_handle);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  /*lint +e737*/
  
  if (efs_ftruncate(fs_handle,file_size) == -1)
  {
    DS_3GPP_MSG3_ERROR( "config :fs_truncate failed fd=%d errno=", 
               fs_handle, efs_errno, 0 );
    (void)efs_close (fs_handle);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------------*/
  if (efs_close (fs_handle) == -1)
  {
    DS_3GPP_MSG3_ERROR("efs_close failed for %d errno=%d", fs_handle, efs_errno, 0);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  return DS_UMTS_PDP_SUCCESS;
}/* dsumts_subs_mgr_set_default_profile_num_per_subs_for_embedded */

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_SET_DEFAULT_PROFILE_NUM_PER_SUBS_FOR_TETHERED

DESCRIPTION
  This function is called by dsumtspdpregint to set the default profile
  that the user specifies for each subscription

PARAMETERS
  profile_num  -   Holds the default profile number set by user
  subs_id      -   The subscription for which this profile is default

DEPENDENCIES
  None

RETURN VALUE
  DS_UMTS_PDP_FAIL          : suns id is invalid
  DS_UMTS_PDP_ACCESS_ERROR  : Error in updating the file
  DS_UMTS_PDP_SUCCESS       : The operation completed successfully

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_profile_status_etype 
dsumts_subs_mgr_set_default_profile_num_per_subs_for_tethered
(
  ds_umts_pdp_subs_e_type           subs_id,
  uint16                            profile_num
)
{
  char                      file_buffer[BUFFER_SIZE] = {0};
  int                       fs_handle;
  int32                     file_size;
  byte                      profile_number;
  uint32                    sub_index;

  profile_number = (byte) profile_num; 
  DS_3GPP_MSG3_MED("DSUMTS_SUBS_MGR: Set default profile per subs for tethered called",
          0,0,0);

  if( (subs_id <= DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE) || 
      (subs_id >= DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_MAX) ) 
  {
    DS_3GPP_MSG3_ERROR("Active subscription ID is invalid",0,0,0);
    return DS_UMTS_PDP_FAIL;
  }

  //Index starts at zero
  sub_index = ((uint32)subs_id - 1);
  
  /*-------------------------------------------------------------------------
    Init the fully qualified profile file name to pdp_profiles directory
  -------------------------------------------------------------------------*/

  if( strlcpy(dsumts_subs_mgr_fully_qual_tethered_file_name,
                  dsumts_subs_mgr_dir_name,
                  sizeof(dsumts_subs_mgr_fully_qual_tethered_file_name) ) >=
      (int)sizeof(dsumts_subs_mgr_fully_qual_tethered_file_name) )
  {
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

  (void)strlcat(dsumts_subs_mgr_fully_qual_tethered_file_name,
                    DSUMTS_SUBS_MGR_PROFILE_NUM_FOR_TETHERED_FILE_NAME,
                    sizeof(dsumts_subs_mgr_fully_qual_tethered_file_name));
  /*lint +e534*/    

  /*-------------------------------------------------------------------------
    Open the file for write 
  -------------------------------------------------------------------------*/
  fs_handle = efs_open(dsumts_subs_mgr_fully_qual_tethered_file_name,
                       O_RDWR);
  if ( fs_handle == -1)
  {
    DS_3GPP_MSG3_ERROR("efs_open failed fd=%d errno=%d.", fs_handle, efs_errno, 0);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
 /*--------------------------------------------------------------------------
    Get the subscription whose default profile has to be set
    Update only that profile number in EFS
    Below step will also cache the latest values for profile numbers 
  -------------------------------------------------------------------------*/
  subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][sub_index].def_profile_num = profile_number;

  /*-------------------------------------------------------------------------
      Write the profile numbers specified by user
      Format of file contents:
      <Default profile number for Subs1>,<Default profile number for Subs2>
      (eg) 3,2
  -------------------------------------------------------------------------*/
#ifdef FEATURE_TRIPLE_SIM
  (void)snprintf
            (
             file_buffer,
             BUFFER_SIZE*sizeof(byte),
             "%d,%d,%d", 
             subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS1_INDEX].def_profile_num, 
             subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS2_INDEX].def_profile_num,
             subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS3_INDEX].def_profile_num
            );
#else
  (void)snprintf
            (
             file_buffer,
             BUFFER_SIZE*sizeof(byte),
             "%d,%d", 
             subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS1_INDEX].def_profile_num, 
             subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][DSUMTS_SUBS_MGR_SUBS2_INDEX].def_profile_num
            );
#endif /* FEATURE_TRIPLE_SIM */

  if (dsumts_subs_mgr_efs_write(fs_handle,
                            (void *)(&file_buffer[0]),
                            (BUFFER_SIZE - 1)*sizeof(byte)) == FALSE)
  {
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

 /*--------------------------------------------------------------------------
    Truncate any contents after the current position if any
  -------------------------------------------------------------------------*/
  file_size = efs_lseek(fs_handle,
                        0,
                        SEEK_CUR);
  if (file_size == -1) 
  {
    DS_3GPP_MSG3_ERROR( "efs_lseek failed for %d errno=%d", fs_handle, efs_errno, 0 );
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

  /*lint -e737*/
  if( ((BUFFER_SIZE - 1)*sizeof(byte)) != file_size )
  {
    DS_3GPP_MSG3_ERROR( "config :file_pos/struct_size mismatch ",
               file_size, 0, 0 );
    (void)efs_close (fs_handle);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  /*lint +e737*/
  
  if (efs_ftruncate(fs_handle,file_size) == -1)
  {
    DS_3GPP_MSG3_ERROR( "config :fs_truncate failed fd=%d errno=", 
               fs_handle, efs_errno, 0 );
    (void)efs_close (fs_handle);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------------*/
  if (efs_close (fs_handle) == -1)
  {
    DS_3GPP_MSG3_ERROR("efs_close failed for %d errno=%d", fs_handle, efs_errno, 0);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  return DS_UMTS_PDP_SUCCESS;
}/* dsumts_subs_mgr_set_default_profile_num_per_subs_for_tethered */

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_DEFAULT_PROFILE_NUM_PER_SUBS_FOR_EMBEDDED

DESCRIPTION
  This function is called by dsumtspdpregint to get the default profile
  for embedded family and specified subscription

PARAMETERS
  profile_num  -  OUT paramenter. Returns the default profile
  subs_id      -  The subscription for which default profile is required

DEPENDENCIES
  None

RETURN VALUE
  DS_UMTS_PDP_FAIL     : Subs id is invalid
  DS_UMTS_PDP_SUCCESS  : The operation completed successfully

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_profile_status_etype 
dsumts_subs_mgr_get_default_profile_num_per_subs_for_embedded
(
  ds_umts_pdp_subs_e_type             subs_id,
  uint16*                             profile_num
)
{
  uint32 sub_index = 0;
  if( (subs_id <= DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE) || 
      (subs_id >= DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_MAX) ) 
  {
    DS_3GPP_MSG3_ERROR("Active subscription ID is invalid",0,0,0);
    return DS_UMTS_PDP_FAIL;
  }

  if (NULL == profile_num)
  {
    DS_3GPP_MSG3_ERROR("Invalid profile_num ptr", 0,0,0);
    return DS_UMTS_PDP_FAIL;
  }

  //Index starts from zero
  sub_index = ((uint32)subs_id - 1);

  //Check for valid flag
  if(dsumts_subs_mgr_cache_value_valid[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX] 
     == TRUE)
    {
        *profile_num = 
       (uint16)subs_def_profile_info[DSUMTS_SUBS_MGR_EMBEDDED_FAMILY_INDEX][sub_index].def_profile_num;
       
  }
  else {
    DATA_ERR_FATAL("Cache values not valid");
  }
  return DS_UMTS_PDP_SUCCESS;
}/* dsumts_subs_mgr_get_default_profile_num_per_subs_for_embedded */

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_DEFAULT_PROFILE_NUM_PER_SUBS_FOR_TETHERED

DESCRIPTION
  This function is called by dsumtspdpregint to get the default profile
  number for tethered family and specified subscription.

PARAMETERS
  profile_num  -  OUT paramenter. Returns the default profile
  subs_id      -  The subscription for which default profile is required

DEPENDENCIES
  None

RETURN VALUE
  DS_UMTS_PDP_FAIL     : Subs id is invalid
  DS_UMTS_PDP_SUCCESS  : The operation completed successfully

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_profile_status_etype 
dsumts_subs_mgr_get_default_profile_num_per_subs_for_tethered
(
  ds_umts_pdp_subs_e_type             subs_id,
  uint16*                             profile_num
)
{
  uint32 sub_index = 0;

  if( (subs_id <= DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE) || 
      (subs_id >= DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_MAX) ) 
  {
    DS_3GPP_MSG3_ERROR("Active subscription ID is invalid",0,0,0);
    return DS_UMTS_PDP_FAIL;
  }

  if (NULL == profile_num)
  {
    DS_3GPP_MSG3_ERROR("Invalid profile_num ptr", 0,0,0);
    return DS_UMTS_PDP_FAIL;
  }

  //Index starts from zero
  sub_index = ((uint32)subs_id - 1);

  /* Check for valid flag */
  if(dsumts_subs_mgr_cache_value_valid[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX] == TRUE)
    {
        *profile_num = 
      (uint16)subs_def_profile_info[DSUMTS_SUBS_MGR_TETHERED_FAMILY_INDEX][sub_index].def_profile_num;
  }
  else {
    DATA_ERR_FATAL(" Tethered cache value is not valid");
  }
  return DS_UMTS_PDP_SUCCESS;
}/* dsumts_subs_mgr_get_default_profile_num_per_subs_for_tethered */

#endif

#endif /*  (FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
