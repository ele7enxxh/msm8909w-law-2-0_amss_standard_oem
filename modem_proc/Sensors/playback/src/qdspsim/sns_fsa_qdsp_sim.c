/*============================================================================
  @file sns_fsa_qdsp_sim.c

  @brief
  This is the implementation of the Sensors file system abstraction for
  QDSP SIM on Windows.


  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-11  ae   Initial version

============================================================================*/


/*============================================================================

  INCLUDE FILES

  ============================================================================*/
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "sensor1.h"
#include "sns_common.h"
#include "sns_fsa.h"
#include "sns_debug_str.h"
#define PATH_MAX 4096

/*============================================================================
 *
  Function Definitions

  ============================================================================*/

/*============================================================================

  FUNCTION:   sns_fsa_open

  ============================================================================*/
void* sns_fsa_open( const char* pathname, const char* mode )
{
  FILE* fptr = NULL;

    fptr = fopen( pathname, mode );
    if( NULL != fptr ) {
      sns_fsa_flush( (void*)fptr );
    }

  return( (void*)fptr );
}

/*============================================================================

  FUNCTION:   sns_fsa_seek

  ============================================================================*/
sns_err_code_e sns_fsa_seek( void* file_handle, int64_t offset, int32_t origin )
{
  int err = SNS_SUCCESS;

  err = fseek( (FILE*)file_handle, offset, SEEK_SET );

  SNS_PRINTF_STRING_LOW_3( SNS_DBG_MOD_APPS_FSA,
                           "sns_fsa_seek: offset %d, origin %d, err: %d",
                           offset, origin, err );
  if( 0 != err ) {
    return SNS_ERR_FAILED;
  }
  else {
    return SNS_SUCCESS;
  }
}

/*============================================================================

  FUNCTION:   sns_fsa_read

  ============================================================================*/
sns_err_code_e sns_fsa_read( void* file_handle, uint8_t* buffer_ptr,
                             uint32_t num_bytes, uint32_t* bytes_read_ptr )
{
  size_t cnt;
  int    err;

  cnt = fread( (void*)buffer_ptr , sizeof(uint8_t), (size_t)num_bytes,
               (FILE*)file_handle );
  err = ferror( (FILE*)file_handle );

  SNS_PRINTF_STRING_LOW_2( SNS_DBG_MOD_APPS_FSA,
                           "sns_fsa_read: bytes read %d, err: %d", cnt, err );

  if( ( 0 == cnt ) || ( EOF == cnt ) || ( 0 != err ) )  {
    *bytes_read_ptr = 0;
    return SNS_ERR_FAILED;
  } else {
    *bytes_read_ptr = (uint32_t)cnt;
    return SNS_SUCCESS;
  }
}

/*============================================================================

  FUNCTION:   sns_fsa_write

  ============================================================================*/
sns_err_code_e sns_fsa_write( void const *file_handle, uint8_t const *buffer_ptr,
                              uint32_t num_bytes, uint32_t* bytes_written_ptr )
{
  size_t cnt;
  int    err;

  cnt = fwrite( (void*)buffer_ptr, sizeof(uint8_t), (size_t)num_bytes,
                (FILE*)file_handle );
  err = ferror( (FILE*)file_handle );

  if(( cnt != num_bytes ) || ( 0 != err )) {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_FSA, "fwrite failed, %d", err );
    *bytes_written_ptr = 0;
    return SNS_ERR_FAILED;
  } else {
    err = sns_fsa_flush( (FILE*)file_handle );

    if( SNS_SUCCESS != err ) {
      SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_FSA, "fflush failed, %d", err );
      *bytes_written_ptr = 0;
      return SNS_ERR_FAILED;
    } else {
      *bytes_written_ptr = (uint32_t)cnt;
      return SNS_SUCCESS;
    }
  }
}

/*============================================================================

  FUNCTION:   sns_fsa_close

  ============================================================================*/
/*!
  @brief Closes a file

  @return
  SNS_ERR_FAILED if operation failed; SNS_SUCCESS otherwise

  @sideeffects
  File will be closed
*/
/*============================================================================*/
sns_err_code_e sns_fsa_close ( void* file_handle )
{
  int err;

  err = fclose( (FILE*)file_handle );
  if( 0 != err )  {
    return SNS_ERR_FAILED;
  } else {
    return SNS_SUCCESS;
  }
}

/*============================================================================

  FUNCTION:   sns_fsa_stat

  ============================================================================*/
sns_err_code_e sns_fsa_stat( const char* pathname, sns_fsa_stat_s *fsa_stat )
{
  sns_err_code_e err = SNS_ERR_FAILED;
  struct stat win_stat;

  if( pathname != NULL &&
      fsa_stat != NULL &&
      0 == stat( pathname, &win_stat ) ) {
    fsa_stat->size = win_stat.st_size;
    err = SNS_SUCCESS;
  }

  return err;
}

/*============================================================================

  FUNCTION:   sns_fsa_flush

  ============================================================================*/
sns_err_code_e sns_fsa_flush( void* file_handle )
{
  int err;

  if ( NULL == file_handle ) {
    return SNS_ERR_BAD_PTR;
  }

  err = fflush( (FILE*)file_handle );
  if ( 0 != err ) {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_FSA, "fflush failed, %i",
                               errno );
    return SNS_ERR_FAILED;
  }

  return SNS_SUCCESS;
}

