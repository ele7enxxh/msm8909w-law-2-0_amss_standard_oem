/*============================================================================
  FILE:         dcvs_nv_os.c
 
  OVERVIEW:     This file provides the os specific layer necessary for NV
                reading and writing.
 
  DEPENDENCIES: This code requires only the basic target NV support.
 
                Copyright (c) 2010 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/os/blast/dcvs_nv_os.c#1 $
  $DateTime: 2016/12/13 07:59:23 $

  when       who     what, where, why
  --------   ----    ---------------------------------------------------
  5/11/10   bjs     Initial version
========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "comdef.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "stringl/stringl.h"
#include "stdarg.h"
#include "DALStdDef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define MAX_NV_PATH_LENGTH 256

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
 @brief dcvs_create_path - Create the path in efs to the file in
        question.

 Create all necessary paths to the file in question.
 
 @param path: The full filename the paths will be created for.
 @param mode: The mode to create directories with.
 
 @return None
 
 */
static void dcvs_create_path(char * path, int mode)
{
  uint32 index;
  // Step through all directories, and make sure they 
  // exist.  If not, create them.
  // 
  if (strlen(path) > 0)
  {
    index = 1;
    while (path[index] != 0)
    {
      if (path[index] == '/')
      {
        path[index] = 0;
        efs_mkdir (path, mode);
        path[index] = '/';
      }
      index++;
    }
  }
}

/**
 @brief dcvs_file_open - Open a file for creation.

 Open a file.  If the file doesn't open, assume the path
 doesn't exist, and create the path.  This function should only
 be used to create files.  It doesn't make sense for just
 reading files.
 
 @param path: The full filename the paths will be created for.
 @param oflag: The flags to use when opening the file.
 @param mode: The mode to create directories with.
 
 @return int: efs file handle
 
 */
static int dcvs_file_open(char * path, int oflag, int mode)
{
  int handle;
  handle = efs_open (path, oflag, mode);
  if (handle >= 0)
  {
    return(handle);
  }
  // The file failed to open.  Create the path.
  dcvs_create_path(path, mode);
  return(efs_open (path, oflag, mode));
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

void DcvsNvJoin(char * path, uint32 pathLength, ...)
{
  va_list ap;
  char * nextStr;

  va_start(ap, pathLength);
  strlcpy(path, "", pathLength);
  nextStr = va_arg(ap, char *);
  while (nextStr != NULL)
  {
    strlcat(path, nextStr, pathLength - strlen(path));
    nextStr = va_arg(ap, char *);
    if (nextStr !=NULL)
    {
      strlcat(path, "/", pathLength - strlen(path));
    }
  }
  va_end(ap);
}

static void CreateFullPathname(char * filename, 
                               uint32 filenameLength, 
                               const char * path, 
                               const char * paramName,
                               const char * ext)
{
  // Create the full pathname plus filename
  DcvsNvJoin(filename, filenameLength, "", "nv", "item_files", path, paramName, NULL);
  // Add the file extension to the full filename.
  strlcat(filename, ext, filenameLength - strlen(filename));
}

DALBOOL DcvsNvStringRead(const char * path, 
                       const char * paramName, 
                       char * paramString,
                       uint32 paramStringLength)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  int length, efsHandle;
  DALBOOL result = TRUE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH, path, paramName, ".txt"); 

  efsHandle = efs_open (fullFilename, O_RDONLY);
  if (efsHandle >= 0)
  {
    // Read the string.
    length = efs_read (efsHandle, (void *)paramString, (fs_size_t)paramStringLength);

    if ((length >= paramStringLength) || 
        (length < 0))
    {
      result = FALSE;
    }
    else
    {
      // Null terminate the string.
      paramString[length] = 0;
    }

    efs_close (efsHandle);
  }
  else
  {
    result = FALSE;
  }

  return(result);
}

int32 DcvsNvStringWrite(const char * path, 
                        const char * paramName, 
                        const char * paramString)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  int length, strLength, efsHandle;
  DALBOOL result = TRUE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH, path, paramName, ".txt"); 

  efsHandle = dcvs_file_open (fullFilename, O_WRONLY | O_CREAT, ALLPERMS);
  if (efsHandle >= 0)
  {
    strLength = strlen(paramString);
    length = efs_write (efsHandle, (void *)paramString, (fs_size_t)strLength);
    if (length != strLength)
    {
      result = FALSE;
    }

    efs_close (efsHandle);
  }
  else
  {
    result = FALSE;
  }

  return(result);
}

int32 DcvsNvUint32Read(const char * path, 
                       const char * paramName, 
                       uint32 * paramValue)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  int length, efsHandle;
  DALBOOL result = TRUE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH, path, paramName, ".uint"); 

  efsHandle = efs_open (fullFilename, O_RDONLY);
  if (efsHandle >= 0)
  {
    length = efs_read (efsHandle, (void *)paramValue, (fs_size_t)sizeof(uint32));
    if (length != sizeof(uint32))
    {
      result = FALSE;
    }

    efs_close (efsHandle);
  }
  else
  {
    result = FALSE;
  }

  return(result);
}

int32 DcvsNvUint32Write(const char * path, 
                        const char * paramName, 
                        uint32 paramValue)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  int length, efsHandle;
  DALBOOL result = TRUE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH, path, paramName, ".uint"); 

  efsHandle = dcvs_file_open (fullFilename, O_WRONLY | O_CREAT, ALLPERMS);
  if (efsHandle >= 0)
  {
    length = efs_write (efsHandle, (void *)&paramValue, (fs_size_t)sizeof(uint32));
    if (length != sizeof(uint32))
    {
      result = FALSE;
    }

    efs_close (efsHandle);
  }
  else
  {
    result = FALSE;
  }

  return(result);
}



