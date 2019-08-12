#ifndef __RFSA_CLIENT_H__
#define __RFSA_CLIENT_H__

/******************************************************************************
 * rfs_client.h
 *
 * Client API definition file for remote file system access
 *
 * Copyright (c) 2012
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/rfsa_client.h#1 $
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2012-02-22   rh      API file cleanup
=============================================================================*/

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** \details
 * Definition for which direction to move the file pointer when calling
 * the seek function
 */
#define RFSA_SEEK_SET                1
#define RFSA_SEEK_CUR                2
#define RFSA_SEEK_END                3

/** \details
 * Definition for file open mode
 */
#define O_CREAT     (0x1 << 4)  /* create file if none exists */
#define O_TRUNC     (0x1 << 5)  /* truncate file before each write */
#define O_APPEND    (0x1 << 6)  /* seek to end before each write */

#define O_NO_OPEN_CHK (0x1 << 17) /* Bypass stat check of the file on open */

#define O_RDONLY    0x0     /* open for read only */
#define O_WRONLY    0x1     /* open for write only */
#define O_RDWR      0x2     /* open for read/write */
#define O_ACCMODE   0x3     /* mask for file access modes */


/** \mainpage Remote File System access API Overview
 *
 * Remote file system access provides standard APIs utilized by libc to 
 * support file open and random read functionality.
 *
 * The actual storage is controlled by the app processor, and the the access
 * is established through the RPC mechanism
 *
 */

/** \details
 * This function initialize the data structure required for remote file system
 * access.  This function must be run before any other API are used
 *
 * @return 0 for success and -1 for failure
 */
int rfsa_client_init(void);

/** \details
 * This function free up all data stuctures used for remote file system access.
 *
 * @return 0 for success and -1 for failure
 */
//int rfsa_client_deinit(void);

/** \details
 * Open a remote file
 * The function do a mock open on the server side to see if the file can be
 * opened without error.  The file is closed immidiately after the open.
 * A internal handle is return to the caller if open is successful.
 * Note regarding file access redirection tag:
 * Depending on the server implementation, a redirection tag is required to ensure
 * cross platform compatibility.  The redirection tag is in the form of
 * "TAGS://" where the entire TAG will be replaced with a pre-allocated
 * directory location in the HLOS' file system. 
 *
 * @param[in] filename
 *    File name
 *
 * @param[in] flags
 *    Open mode flag.  The possible value for the flags can be:
 *    O_RDONLY - Read only operation
 *    O_WRONLY - Write only operation
 *    O_NO_OPEN_CHK - Open without checking if the file can be accessed 
 *                    on the server side
 *    O_CREAT - Combine with write operation to create an empty file for write
 *
 * @return -1 if the file can not be opened, a valid handle number otherwise
 *
 */
int32_t rfsa_open(const char *filename, int flags);

/** \details
 * Close a remote file
 * Calling this function will free up the handle
 *
 * @param[in] handle
 *    Integer value of the handle 
 *
 * @return 0 if close is successful, -1 otherwise
 *
 */
int32_t rfsa_close(int32_t handle);


/** \details
 * Get the size of the file 
 *
 * @param[in] handle
 *    Integer value of the handle 
 *
 * @return Size of the file in bytes
 *         -1 to indicate failure 
 *
 */
long rfsa_flen(int32_t handle);


/** \details
 * Get the size of the file 
 *
 * @param[in] filename
 *    File name
 *
 * @return Size of the file in bytes
 *         -1 to indicate failure 
 *
 */
long rfsa_plen (const char *filename);


/** \details
 * Get the current file position indicator
 * Since the file is not really opened on the server side, calling this
 * function simply return a pre-recorded file position indicator
 *
 * @param[in] handle
 *    Integer value of the handle 
 *
 * @return Current file position indicator
 *         -1 to indicate failure
 *
 */
long rfsa_ftell(int32_t handle);


/** \details
 * Read the content of the file described by file handle 
 *
 * @param[in] handle
 *    Integer value of the handle 
 * @param[out] *buffer
 *    Pointer to the buffer that the data will be written to
 * @param[in] size
 *    Number of bytes to read from the file
 *
 * @return -1 for error otherwise returns number of bytes read
 *
 */
int32_t rfsa_read(int32_t handle, char *buffer, int size);

/** \details
 * Write to the file described by file handle 
 *
 * @param[in] handle
 *    Integer value of the handle 
 * @param[out] *buffer
 *    Pointer to the buffer that the data will be written from
 * @param[in] size
 *    Number of bytes to write to the file
 *
 * @return -1 for error otherwise returns number of bytes read
 *
 */
int32_t rfsa_write (int32_t handle, char *buffer, int size);

/** \details
 * Set the current file position indicator
 * Since the file is not really opened on the server side, calling this
 * function simply set a file position variable stored on the client side
 *
 * @param[in] handle
 *    Integer value of the handle 
 *
 * @param[in] count
 *    The new file position, from the start of the file
 *
 * @param[in] way
 *    Which direction to seek, it can be RFSA_SEEKSET to seek reference
 *    to the beginning of the file, or RFSA_SEEK_CUR to seek from the 
 *    current position
 *
 * @return 0 if seek is successful, -1 otherwise
 *
 */
int rfsa_seek(int32_t handle, int32_t count, int32_t way);


#ifdef __cplusplus
}
#endif //__cplusplus


#endif /* __RFSA_CLIENT_H__ */

