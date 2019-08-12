/***********************************************************************
 * fs_zip.h
 *
 * FS ZIP Module.
 * Copyright (C) 2011, 2013 QUALCOMM Technologies, Inc.
 *
 * This module provides API to compress and decompress byte streams or
 * files.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/inc/fs_zip.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-02-28   nr    Update to the new zlib version (1.2.7) from services team.
2011-10-24   nr    Minor Cleanup and Fixes.
2011-06-23   nr    Create

===========================================================================*/


/************************************************************************
 *
 *  The GZIP HEADER IS DECRIBED IN THE GZIP FILE FORMAT RFC
 *
 ************************************************************************/

#ifndef __FS_ZIP_H__
#define __FS_ZIP_H__


#include "fs_config_i.h"
#include "fs_util.h"

#ifdef FEATURE_EFS_ZIP

/* header(10) + trailer (8) + (1) byte of min data */
#define FS_ZIP_MINIMUM_GZIP_SIZE 19


/* The default = DEF_MEM_LEVEL = 8 which gives better compression at cost of
 * higher memory.
 * We find that increasing the memory level increses the memory usage
 * exponentially and thus find that the memory level of 4 gives us resonable
 * compression without the cost of too much extra memory. Higher memory levels
 * require much more memory without a significant increase in compression.
 * We use MEM_LEVEL of 4 and set FS_ZIP_COMPRESSION_MEMORY
 * For this memory level we need at least 145092 bytes for ZLIB to work
 */
#define FS_ZIP_MEM_LEVEL 4
#define FS_ZIP_COMPRESSION_MEMORY 145092 /* Minimum memory usage in bytes
                              NOTE : This value also depends on window size */

/* The zlibs default compression level is Z_DEFAULT_COMPRESSION=6. We override
 * that value and pass it a new level using this macro. We set it to 1 for
 * minimal but fastest compression.
 */
#define FS_ZIP_COMPRESSION_LEVEL  1


/* The Zlib libray uses the absolute value of the window size to set the actual
 * size. A positive window size is used to indicate if Zlib should generate
 * a "header" and a "trailer".
 * A negative window size means the libray only compresses and gives us the
 * compressed data and the user is required to set his own header and trailer.
 * Since we are using a gzip format we do this ourselves and set the window
 * size as negative.
 * The window size is set to -MAX_WBITS = -15. The actual window size is two
 * raised to the window size given. Here it is equal to 1<<15 = 32k which is
 * the value indicated in the gzip format RFC.
 * The FS_ZIP_DECOMPRESSION_MEMORY depends only on the window size
 */
#define FS_ZIP_WINDOW_SIZE -15
#define FS_ZIP_DECOMPRESSION_MEMORY 42288 /* Mem usage in bytes*/


typedef struct fs_zip_handle_structure* fs_zip_handle;

enum fs_zip_mode
{
  FS_ZIP_MODE_COMPRESSION   = 'w', /* Open zip stream to compress data   */
  FS_ZIP_MODE_DECOMPRESSION = 'r'  /* Open zip stream to decompress data */
};

enum fs_zip_return_status
{
  FS_ZIP_STATUS_NEED_MORE_INPUT = 2,  /* The input has been consumed */
  FS_ZIP_STATUS_FINISHED        = 1,  /* End of zip operation */
  FS_ZIP_STATUS_SUCCESS         = 0,  /* The call was successful */
  FS_ZIP_STATUS_UNKNOWN_ERROR   = -1, /* Unknown error */
  FS_ZIP_STATUS_STATE_ERROR     = -2, /* The api's are called incorrectly */
  FS_ZIP_STATUS_UNUSED_STREAM   = -3, /* The stream is not in use (unopened) */
  FS_ZIP_STATUS_MODE_ERROR      = -4, /* Wrong api ie compress instead of
                                         decompress */
  FS_ZIP_STATUS_HEADER_ERROR    = -5, /* bad header or the not enough
                                         buffer given */
  FS_ZIP_STATUS_CRC_ERROR       = -6  /* Bad crc file is likey corrupt */
};


/**************************************************************************//**
 * @par Name:
 *   fs_zip_init - initilize the zip module.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip.h"
 *    void fs_zip_init (void);
 *  @endcode
 *
 * @par Description:
 *  Initilize the zip module.
 *
 *****************************************************************************/
void fs_zip_init (void);


/**************************************************************************//**
 * @par Name:
 *   fs_zip_open - Open and return a handle for the zip stream created.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip.h"
 *    fs_zip_handle
 *    fs_zip_open (enum fs_zip_mode mode, void *heap_start, uint32 heap_size);
 *  @endcode
 *
 * @par Description:
 *  Open zip stream and return a handle.
 *
 * @param mode
 *  The mode to open the zip stream
 *  i.e. FS_ZIP_MODE_COMPRESSION\FS_ZIP_MODE_DECOMPRESSION.
 *
 * @param heap_start
 *  the pointer to the start of the heap to use in the ZIP DEFLATE operation.
 *
 * @param heap_size
 *  Size of the previous argument.
 *
 * @return
 * Returns NULL if already in use or some other error occurs else returns
 * the pointer to the handle of the stream. The errors include invalid mode
 * and errors in zlib library.
 *
******************************************************************************/
fs_zip_handle
fs_zip_open (enum fs_zip_mode mode, void *heap_start, uint32 heap_size);


/**************************************************************************//**
 * @par Name:
 *   fs_zip_set_input - Set input to use.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip.h"
 *    enum fs_zip_return_status
 *    fs_zip_set_input (fs_zip_handle handle, void *input, uint32 size);
 *  @endcode
 *
 * @par Description:
 *  Set input to use with zip operations.
 *
 * @param handle
 *  The zip stream handle pointer returned by fs_zip_open.
 *
 * @param input
 *  The input buffer.
 *
 * @param size
 *  The size of input buffer.
 *
 * @return
 * Returns a status message of type enum fs_zip_return_status.
 *
******************************************************************************/
enum fs_zip_return_status
fs_zip_set_input (fs_zip_handle handle, void *input, uint32 size);


/**************************************************************************//**
 * @par Name:
 *   fs_zip_compress - Compress the data set in fs_zip_set_input.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip.h"
 *    enum fs_zip_return_status
 *    fs_zip_compress (fs_zip_handle handle, void *output,
 *             uint32 output_buff_size, uint32 *available_output);
 *  @endcode
 *
 * @par Description:
 *  Compress the data set in fs_zip_set_input and return the output if possible
 *  in the output buffer.
 *
 * @param handle
 *  The zip stream handle pointer returned by fs_zip_open.
 *
 * @param output
 *  The output buffer.
 *
 * @param output_buff_size
 *  The size of output buffer.
 *
 * @param available_output
 *  The size of output actually filled in the call (out param).
 *
 * @return
 * Returns a status message of type enum fs_zip_return_status;
 *
******************************************************************************/
enum fs_zip_return_status
fs_zip_compress (fs_zip_handle handle, void *output, uint32 output_buff_size,
                 uint32 *available_output);


/**************************************************************************//**
 * @par Name:
 *   fs_zip_Decompress - Decompress the data set in fs_zip_set_input.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip.h"
 *    enum fs_zip_return_status
 *    fs_zip_decompress (fs_zip_handle handle, void *output,
 *                  uint32 output_buff_size, uint32 *available_output);
 *
 *  @endcode
 *
 * @par Description:
 *  Decompress the data set in fs_zip_set_input and return the output
 *  if possible in the output buffer.
 *
 * @param handle
 *  The zip stream handle pointer returned by fs_zip_open.
 *
 * @param output
 *  The output buffer.
 *
 * @param output_buff_size
 *  The size of output buffer.
 *
 * @param available_output
 *  The size of output actually filled in the call (out param).
 *
 * @return
 * Returns a status message of type enum fs_zip_return_status;
 *
******************************************************************************/
enum fs_zip_return_status
fs_zip_decompress (fs_zip_handle handle, void *output, uint32 output_buff_size,
                   uint32 *available_output);

/**************************************************************************//**
 * @par Name:
 *   fs_zip_close - Close the zip stream.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip.h"
 *    enum fs_zip_return_status fs_zip_close (void *handle_ptr);
 *  @endcode
 *
 * @par Description:
 *  Open zip stream and return a handle.
 *
 * @param handle
 *  The zip stream handle pointer returned by fs_zip_open.
 *
 * @return
 * Returns a status message of type enum fs_zip_return_status;
 *
******************************************************************************/
enum fs_zip_return_status
fs_zip_close (fs_zip_handle handle);


/**************************************************************************//**
 * @par Name:
 *   fs_zip_is_this_a_gzip_file - Check if file is possibly gzip.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip.h"
 *    int fs_zip_is_this_a_gzip_file(uint8 *buffer, uint32 buffer_size);
 *  @endcode
 *
 * @par Description:
 *  Check if the file is a gzip file.
 *
 * @param buffer
 *  The buffer containing the first frame of the file.
 *
 * @param buffer_size
 *  Size of the previous argument.
 *
 * @return
 * Returns 0 if the file is not a gzip file and 1 if it is a possible
 * gzip file.
 *
******************************************************************************/
int
fs_zip_is_this_a_gzip_file (void *buffer, uint32 buffer_size);

#else /* NOT FEATURE_EFS_ZIP */

 static inline void
 fs_zip_init (void)
 {
 }

#endif /* FEATURE_EFS_ZIP */

#endif /* not __FS_ZIP_H__ */



