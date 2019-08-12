/*============================================================================
  @file gnss_persistent_cfg.c

                  GNSS Persistent Configuration Parameter Module

GENERAL DESCRIPTION

  This file containts the implementation of the API betwen the LBS task's SDP 
  module and the NV storage medium where NV parameters are stored

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/src/gnss_persistent_cfg.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/10   kgj     Original

=============================================================================*/


/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "comdef.h"
#include "string.h"
#include "gnss_persistent_cfg.h"
#include "fs_public.h"
#include "aries_os_api.h"


#define MAX_FILEPATH_SZ   30

#define PERSISTENT_CFG_FILE_STRUCTURE_VERSION   1

/*
 *  The following structure is describes the header for all
 *  persistent config files.
 *
 *  The w_len field is the length of the file (in bytes), including the header
 *  
 *  The w_version field is the version of the file header.  It will probably 
 *  never change, but it's here "just in case".  This is NOT the
 *  version of the "payload".  Versioning of "payloads" is the responsibility
 *  of the caller.
 *
 *  The two reserved fields should be set to 0
 *
 *  The b_xsum is an 8-bit checksum.  When writing a file, the checksum
 *  is calculated as follows:
 *    1) Sum each byte of the header and payload EXCEPT for the b_xsum
 *        field (or set b_xsum to 0 before summing all bytes).
 *    2) Subtract the value calculated in step 1 from 0xFF.
 *
 *  When reading a file, the checksum is calculated by summing all bytes
 *  of the header and payload (including b_xsum) and comparing the result
 *  to 0xFF.
 */
typedef struct {
  uint16                      w_len;
  uint16                      w_version;
  uint16                      w_reserved1;
  uint8                       b_reserved2;
  uint8                       b_xsum;
} fileHdrT;

/* ---------------------------------------------------------------------------
 * Private Data
 * ---------------------------------------------------------------------------*/
static const char * cfgDirNames[3] = 
  {
    "/gnss/cfg/sdp/",
    "/gnss/cfg/nf/",
    "/gnss/cfg/lbs/"
  };

static const char * cfgFileNames[3] =
  {
      "runtime",
      "private",
      "public"
  };



/* ---------------------------------------------------------------------------
 * Private Functions
 * ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 *                          getFilename
 * ---------------------------------------------------------------------------*/
/**
@brief This function maps a client a block type to a file name

@param [in] e_client
    the client requesting this operation

@param [in] e_blockType 
    the configuration block type

@param [out] p_filePath
    pointer to the location in which to store the file name
*/
static void getFilename(  gnss_persistentCfgClientT   e_client,
                          gnss_persistentCfgBlockT    e_blockType,
                          char *                      p_filePath )
{
  (void) GNSS_SNPRINTF(  p_filePath,
                          MAX_FILEPATH_SZ, 
                          "%s%s",
                          cfgDirNames[ (int)e_client],
                          cfgFileNames[ (int)e_blockType ] );
}


/* ---------------------------------------------------------------------------
 *                          getDirname
 * ---------------------------------------------------------------------------*/
/**
@brief This function maps a client to an EFS directory name

@param [in] e_client
    the client requesting this operation

@return dirName
    returns a const pointer to a directory name
*/
static const char * getDirname(  gnss_persistentCfgClientT   e_client )
{
  return cfgDirNames[ (int) e_client ];
}



/* ---------------------------------------------------------------------------
 *                          getChecksum
 * ---------------------------------------------------------------------------*/
/**
@brief This function computes a checksum on a byte array

@param [in] p_data
    Pointer to byte array on which to calculate the checksum

@param [in] sz
    Number of bytes to be included in the checksum

@return xsum
    returns an 8-bit sum of an array of bytes
*/
static uint8  getChecksum( const void *  p_data, size_t  sz )
{
  const uint8 *   pB = (const uint8 *) p_data;
  uint8           xsum = 0;

  while( sz-- )
  {
    xsum += *pB++;
  }

  return xsum;
}



/* ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 *                          PUBLIC FUNCTIONS
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 *                          gnss_persistentCfgRead
 * ---------------------------------------------------------------------------*/
/**
@brief This function fetches a parameter block from non-volatile storage

@param e_client
    the client requesting this operation

@param e_blockType 
    the block type to be read

@param p_paramBuffer 
    pointer to location in which to store the parameter block

@param w_paramBufferSz 
    max size of the parameter block to be read (in bytes)

@param pw_numBytesRead 
    actual number of bytes read

@par Errors:
    GPC_ERROR_NONE : no error
    GPC_ERROR_NOEXIST : The parameter block does not exist in NV memory
    GPC_ERROR_MISC_READ : Could not read the parameter block from NV

@return
    Returns an error code telling the user if the operation was successful
    or what type of error occurred.
*/
gnss_persistentCfgErrorT  gnss_persistentCfgRead(
                        gnss_persistentCfgClientT   e_client,
                        gnss_persistentCfgBlockT    e_blockType,
                        void *                      p_paramBuffer,
                        uint16                      w_paramBufferSz,
                        uint16 *                    pw_numBytesRead )
{
  int           fd;
  char          filePath[ MAX_FILEPATH_SZ];
  fileHdrT      fileHdr;
  uint16        numBytesRead;
  uint8         b_xsum;
  uint16        w_payloadLen;

  /*
   *  Initialize the number of bytes read to 0
   */
  *pw_numBytesRead = 0;
 
  /*
   *  Select the correct filename based upon e_blockType
   */
  getFilename( e_client, e_blockType, filePath );

  /*
   *  Attempt to open the file
   *  Assume that any failure here means that the file does not exist
   *     
   */
  fd = efs_open( filePath, O_RDONLY );
  if( 0 > fd )
  {
    return GPC_ERROR_NOEXIST;
  }

  /*
   *  OK, the file exists.
   *  Attempt to read the header
   */
  numBytesRead = (uint16) efs_read( fd, &fileHdr, sizeof(fileHdr) );
  if( sizeof(fileHdr) != numBytesRead )
  {
    return GPC_ERROR_MISC_READ;
  }

  /*
   *  The payload size is the total file size minus the header
   */
  w_payloadLen = fileHdr.w_len - sizeof(fileHdr);

  /*
   *  Verify that the header LEN and VERSION matches what is expected
   */
  if( (w_paramBufferSz < w_payloadLen ) ||
      (PERSISTENT_CFG_FILE_STRUCTURE_VERSION != fileHdr.w_version) )
  {
    return GPC_ERROR_MISC_READ;
  }

  /*
   *  Attempt to read the "payload"
   */
  numBytesRead = (uint16) efs_read( fd, p_paramBuffer, w_payloadLen );
  if( w_payloadLen != numBytesRead )
  {
    return GPC_ERROR_MISC_READ;
  }
  *pw_numBytesRead = numBytesRead;
 
  /* 
   *  Verify the checksum is as expected
   */
  b_xsum = getChecksum( &fileHdr, sizeof(fileHdr) );
  b_xsum += getChecksum( p_paramBuffer, w_payloadLen );

  if( b_xsum != 0xFF )
  {
    return GPC_ERROR_MISC_READ;
  }

  /*
   *  Close the file.  We are done with it. 
   */
  if( efs_close( fd ) < 0 )
  {
    return GPC_ERROR_MISC_READ;
  }
 
  return GPC_ERROR_NONE;
}

/* ---------------------------------------------------------------------------
 *                       gnss_persistentCfgWrite
 * ---------------------------------------------------------------------------*/
/**
@brief This function writes a parameter block to non-volatile storage

@param e_client
    the client requesting this operation

@param e_blockType 
    the block type to be written

@param p_paramBuffer 
    pointer to parameter block to be written

@param w_paramBufferSz 
    size of the parameter block to be written (in bytes)

@param pw_numBytesWritten 
    actual number of bytes written

@par Errors:
    GPC_ERROR_NONE : no error
    GPC_ERROR_MISC_WRITE : Could not write the parameter block to NV

@return
    Returns an error code telling the user if the operation was successful
*/
gnss_persistentCfgErrorT  gnss_persistentCfgWrite(
                              gnss_persistentCfgClientT   e_client,
                              gnss_persistentCfgBlockT    e_blockType,
                              const void *                p_paramBuffer,
                              uint16                      w_paramBufferSz,
                              uint16 *                    pw_numBytesWritten )
 
{
  const char *  p_dirname;
  int           fd;
  char          filePath[ MAX_FILEPATH_SZ];
  fileHdrT      fileHdr;
  uint16        numBytesWritten;
  uint8         b_xsum;

  /*
   *  Initialize the number of bytes written to 0
   */
  *pw_numBytesWritten = 0;
 
  /*
   *  Select the correct dirname and filename based upon e_client & e_blockType
   */
  p_dirname = getDirname( e_client );
  getFilename( e_client, e_blockType, filePath );

  /*
   *  Attempt to create the root directory and the "cfg" directory
   *  under that.  Ignore errors on failure and just assume that 
   *  the directory already existed.
   */
  (void) efs_mkdir( "/gnss/", 0664 );
  (void) efs_mkdir( "/gnss/cfg/", 0664 );
 
  /*
   *  Attempt to create the module specific directory.  Ignore errors 
   *  on failure and just assume that the directory already existed.
   */
  (void) efs_mkdir( p_dirname, 0664 );

  /*
   *  Open the filename for WRITE and TRUCate if it exists
   */  
  fd = efs_open( filePath, O_CREAT | O_WRONLY | O_TRUNC, 0644 );
  if( 0 > fd )
  {
    return GPC_ERROR_MISC_WRITE;
  }

  /*
   *  Build a file header and write it to the file
   */
  memset( &fileHdr, 0, sizeof(fileHdr) );

  fileHdr.w_len       = sizeof(fileHdr) + w_paramBufferSz;
  fileHdr.w_version   = PERSISTENT_CFG_FILE_STRUCTURE_VERSION;

  b_xsum = getChecksum( &fileHdr, sizeof(fileHdr) );
  b_xsum += getChecksum( p_paramBuffer, w_paramBufferSz );

  fileHdr.b_xsum = (0xFF - b_xsum);

  /*
   *  Attempt to write w_paramBufferSz bytes to the file
   *  IF actual number of bytes written is not equal to w_paramBufferSz THEN
   *    return an error
   *  ENDIF
   */
  numBytesWritten = (uint16) efs_write( fd, &fileHdr, sizeof(fileHdr) );
  if( numBytesWritten != sizeof(fileHdr) )
    return GPC_ERROR_MISC_WRITE;
 
  numBytesWritten = (uint16) efs_write( fd, p_paramBuffer, w_paramBufferSz );
  if( numBytesWritten != w_paramBufferSz )
    return GPC_ERROR_MISC_WRITE;
  *pw_numBytesWritten = numBytesWritten;

  /*
   *  Close the file.  We are done with it. 
   */
  if( efs_close( fd ) < 0 )
  {
    return GPC_ERROR_MISC_WRITE;
  }
 
  /*
   *  All seemed to go well, so return SUCCESS
   */
  return GPC_ERROR_NONE;
}
