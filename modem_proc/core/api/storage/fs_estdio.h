/****************************************************************************
 * fs_estdio.h
 * Enhanced EFS Stdio implementation. Replacement for previous stdio library.
 *
 * Standard IO library for EFS.
 * Copyright (C) 2009-2010, Qualcomm Technologies, Inc.
 *
 * This Standard IO library is POSIX compliant except for:
 * - When a file is open with efs_efopen() the stream is considered
 *  Non-Buffered.
 * - Calling efs_esetvbuf() with input_buf = NULL it will not allocate the
 *  buffer, it will assume no buffering mode.
 * - There is no implementation for line buffering (EFS_IOLBF).
 *
 */

/*==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/fs_estdio.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when        who  what, where, why
 * ----------  ---  -------------------------------------------------------
 * 2010-11-11  wek  Improve documentation to remove compile time macros.
 * 2010-07-12  rp   Added defines for major and minor number for API files.
 * 2010-04-21  rp   Fix the prototype of efs_efseek64() API.
 * 2010-03-19  yog  Added support for files having max filesize upto 4GB.
 * 2009-08-10  wek  Fix comments for doxygen.
 * 2009-01-26  wek  Create. Replacement for fs_stdio API.
 *
 ==========================================================================*/

#ifndef __FS_ESTDIO_H__
#define __FS_ESTDIO_H__

#include "fs_sys_types.h"

/** Identifies the major version number of this API file. */
#define VER_FS_ESTDIO_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_ESTDIO_H_MINOR     1

/**
 * @defgroup stream_io Stream based EFS I/O
 * @ingroup efs
 *
 * EFS also supports stream based I/O. In stream based I/O the file is
 * treated as a continuous stream of data, it enables the changes to a file
 * to first go into a RAM buffer, and later committed to the file.
 *
 */

/**
 * @defgroup stream_api Stream API's
 * @ingroup stream_io
 *
 * The arguments to the stream based API's are almost similar to the EFS
 * API's
 *
 * Following are the stream based API's:\n
 * ::efs_efopen\n
 * ::efs_efdopen\n
 * ::efs_efread\n
 * ::efs_efwrite\n
 * ::efs_efclose\n
 * ::efs_efseek\n
 * ::efs_eftell\n
 * ::efs_efflush\n
 * ::efs_esetvbuf\n
 * ::efs_efileno\n
 *
 * @deprecated
 * ::efs_fopen\n
 * ::efs_fdopen\n
 * ::efs_fread\n
 * ::efs_fwrite\n
 * ::efs_fseek\n
 * ::efs_ftell\n
 * ::efs_set_sync\n
 * ::efs_set_async_params\n
 * ::efs_process_async_writes\n
 * ::efs_setvbuf\n
 * ::efs_fileno\n
 * ::efs_fpending\n
 *
 */

/**
 * @defgroup rollback  Rollback support
 * @ingroup stream_io
 *
 * Rollback support is available for stream based I/O. Users can supply an
 * optional 'u' as the mode parameter to the efs_efopen() API to indicate
 * rollback preference. When a file is open with rollback option, then all the
 * changes that are made to this file in the current session will be undone in
 * the case of power-failure, the file be restored to its previous contents
 * upon the next boot. Please see efs_efopen() API for more information
 * about the rollback feature.
 */

/**
 * File Stream Handler.
 */
typedef struct _efs_efile_io EFS_EFILE;

/** @brief Initializes library for first use.
 *
 *  It initializes critical sections, rollback, and data structures for stdio
 *  library. This function should be called once on boot.
 */
void efs_estdio_init (void);


/**
 * @brief Opens a file.
 *
 * This function opens a file pointed by <b>path</b> and associates an stream
 * with it.
 * The argument <b>mode</b> points to a string which starts with the following:
 * - <tt> r rb </tt>    Open a file for reading.
 * - <tt> w wb wu </tt>    Truncate to zero length or create a file for
 *                           writing.
 * - <tt> a ab a </tt>    Append. Open or create the file if it does not
 *                           exists for writing at the end of file.
 * - <tt> r+ rb+ r+b </tt> Open the file for update. The stream
 *                           is positioned at the beginning.
 * - <tt> w+ wb+ w+b wu+ w+u </tt> Create the file if it does not exist,
 *                            otherwise it is truncated to zero length.
 * - <tt> a+ ab+ a+b </tt> Append. Creates the file if it does
 *                            not exist. Reads happen at the beginning of the
 *                            file, writes always go at the end.
 *
 * The character 'b' has no effect, "wbu+" is the same as "w+u" and "wu+",
 * allowed for ISO C standard conformance. The character 'u' requests for
 * a rollback file. All changes to the given file will be tracked by
 * maintaining a rollback-file. If power down ocurs before the file is closed,
 * upon reboot all writes to the file will get undone, the file will reset to
 * its original-contents that it had before the call to efs_open() was made.
 * If the file was a new-file and was created only in the previous boot
 * session, then it will get deleted.
 *
 * - Rollback is only supported for EFS2, and not supported on any
 *    other insertable-media. Do not create/open files with this 'u'
 *    option for any other media.
 * - The rollback-file grows proportional with the number of bytes
 *    that gets written efs_efwrite() calls. For example, after opening
 *    a 10-byte file, if we rewrite every byte 10-times, then the size
 *    of the rollback will be 100-bytes + overhead-bytes. The overhead
 *    bytes in this case would be (40-bytes + 120-bytes).
 * - The rollback-feature is in best use, if used on large-files that
 *    gets updated in small-sizes.
 * - The rollback-file is stored on EFS2, so please make sure that
 *    there is enough space on EFS2 for the rollback-file. The size
 *    of the rollback-file is proportional to the number of writes
 *    plus the amount of bytes being overwritten on the original file.
 *
 * Opening a file for reading ('r' at the first character of mode) will fail
 * if the file does not exist.
 *
 * When using append mode ('a' as the first character of mode) all write will
 * go to the end of the file, regardless of efs_efseek() calls.  Also, all
 * read operations will be unbuffered.
 *
 * If the '+' character is present both read and writes may be performed. If
 * the stream is buffered the application has to ensure that between every
 * write operation (efs_efwrite()) and the following read operation
 * (efs_efread()) there is a efs_efflush() or efs_efseek(). This will
 * guarantee that all the data in the buffer is flushed to disk. [POSIX 2004
 * Standard Sytem Interfaces, Issue 6 line 12882]
 *
 * All files operations using the same handle are thread safe. It's not
 * recommended to use the same file descriptor on different file handles or
 * mixing the APIs defined in fs_public.h with the ones defined here. If the
 * application decides doing this, it will need to guarantee no concurrent
 * access are made to the file descriptor.
 *
 * @note
 * Every opened file is considered unbuffered. The application needs to
 * call efs_esetvbuf() to set buffering mode. [Contrary to the 2004 POSIX
 * Standard System Interfaces, Issue 6 line 12886]
 *
 * @param path Path of file to be open.
 * @param mode Mode for opening the file.
 *
 * @return If successful, returns pointer to the object controlling the stream.
 *         On error NULL and the efs_errno is set accordingly.
 *
 * @par Errors:
 * ::EEXIST - Cannot create a file with the pathname because another file
 *            with the same name exists and an exclusive open is requested
 *            or a special (exclusive) file with the same pathname exists.\n
 * ::ENOENT - No entry for the pathname is found, the file cannot be
 *            opened (and a new file is not created because the O_CREAT
 *            flag is not supplied).\n
 * ::EMFILE - Maximum number of open descriptor is exceeded.\n
 * ::EISDIR - Opening a file with  a write flag (O_WRONLY or O_RDWR)
 *            denied because a directory with the same name exists.\n
 * ::ENOSPC - No space left on the device.\n
 * ::ENAMETOOLONG - Filename/directory name exceeded NAME_MAX limit or the
 *                  pathname exceeded PATH_MAX limit.\n
 * ::ENOMEM - No more dynamic memory is available.\n
 * ::ENODEV - The device does not exist.\n
 * ::ENOTDIR - The file could not be created under a path that is not
 *            a directory. Another possibility is an open with the
 *            O_CREAT flag tried to create a file in the rom file system.\n
 * ::EINVAL - Invalid Mode Argument.\n
 * ::EROLLBACK - Could not create Rollback File.
 *
 */
EFS_EFILE *efs_efopen (const char *path, const char *mode);

/**
 * @brief Create a stream from an existing file descriptor.
 *
 * The function associates an existing file descriptor obtained by using
 * efs_open() with a stream. The application has to ensure that the opened
 * file with efs_open() used the same mode than when efs_efdopen() is called,
 * with the exception of the rollback flag. When opening a file using modes
 * that begins with 'w' will not truncate the file. Requests to create a
 * rollback file will result on error, rollback files are only created by
 * efs_efopen().
 *
 * The possible modes and errors are the same as efs_efopen() and attempting
 * to create a rollback file.
 *
 * When using efs_efdopen(), efs_efopen() or efs_open() and creating multiple
 * handles for the same file, the application has to guarantee that only one
 * thread is doing file operations (e.g. read, write, seek) on the opened file
 * at a time. The file offset is handled differently when using efs_open() and
 * having a stream handler (by calling efs_efopen() or efs_efdopen()),
 * therefore the application has to ensure that efs_lseek() and the next
 * efs_read() or efs_write() are atomic. This is handled transparently for the
 * application when using stdio API's.
 *
 * @param filedes Descriptor returned by efs_open()
 * @param mode Mode for opening the file. Should be the same as used for
 *             efs_open().
 *
 * @return If successful returns a pointer to the object controlling the
 *         stream. On error returns NULL and efs_errno is set accordingly.
 *
 * @see efs_efopen()
 */
EFS_EFILE *efs_efdopen (int filedes, const char *mode);

/**
 * @brief Reads from a file.
 *
 * It attempts to read a number of items ('nitems') of certain size ('size')
 * into the array ptr from an opened file. Effectively it tries to read
 * nitems*size. The position of the file is advanced accordingly depending on
 * the number of read bytes. When an error occurs the resulting file position
 * and the number of bytes read from the last item are uncertain.
 *
 * If using buffered mode and there is unwritten data in the buffer the
 * application has to make sure to call efs_efflush() or efs_efseek() before
 * calling efs_efread(). If this rule is not followed, buffered data is
 * discarded producing partial writes.
 *
 * @param ptr Array to fill with read data.
 * @param size Size in bytes of each item.
 * @param nitems Number of items to read.
 * @param stream Pointer to the stream object that handles the file.
 *
 * @return Number of items successfully read.
 */
fs_size_t efs_efread (void *ptr, fs_size_t size, fs_size_t nitems,
  EFS_EFILE *stream);

/**
 * @brief Writes data to a file.
 *
 * It attempts to write the data in the array pointed by 'ptr' to a file.
 * This data is interpreted as a certain number of elements 'nitems' of the
 * given 'size' bytes. When an error occurs the resulting file position and
 * the number of bytes read for the last item are uncertain.
 *
 * When using buffered mode the data may not be written to disk until
 * efs_efflush() or efs_efseek() is called. If nitems or size is 0, the API
 * will return 0. Otherwise it will return the number of items written. On
 * error it will return 0 and set efs_errno accordingly.
 *
 * @param ptr Array containing data to be written.
 * @param size Size in bytes of each item.
 * @param nitems Number of items to write.
 * @param stream Pointer to the stream object that handles the file.
 *
 * @return Number of items successfully written. Zero (0) on error or if size
 *         or nitems is zero. On error the error indicator of the stream is set
 *         and efs_errno is set accordingly.
 */
fs_size_t efs_efwrite (void *ptr, fs_size_t size, fs_size_t nitems,
  EFS_EFILE *stream);

/**
 * @brief Closes a stream
 *
 * Disassociates the stream with its underlying file handle object. Any
 * unwritten buffered data is written to the file. It will always disassociate
 * any buffer set by efs_esetvbuf() to the stream and the stream with the
 * file. Rollback data is removed and erased from device.
 *
 * Error may occur when writing the buffered data to disk, there is no direct
 * procedure to re-open the file and regain buffered data.
 *
 * @param stream Pointer to the stream object that handles the file.
 *
 * @return 0 on success, EOF on error, efs_errno is set accordingly.
 */
int efs_efclose (EFS_EFILE *stream);

/**
 * @brief Move the file read/write position.
 *
 * Sets the file position for the stream pointed by <b>stream</b>. The new
 * position, measured in bytes, is obtained by adding <b>offset</b> to the
 * position specified by whence. The possible values of whence are SEEK_SET,
 * SEEK_CURR and SEEK_END indicating beginning of the file, current file
 * position and end of file respectively.
 *
 * This API allows the file position be moved beyond the end of file. If data
 * is written beyond the end of file, the gap is assumed to be filled with 0
 * until data is written to the gap. Depending on the file system (e.g. FAT,
 * EFS2), the zero filled gap may or may not be written to the device.
 *
 * If the stream is writable and is in buffered mode (by calling
 * efs_esetvbuf()) any unwritten data will be flushed to the file. [POSIX
 * 2004 Standard Sytem Interfaces, Issue 6 line 14894]
 *
 * @param stream Pointer to the stream object that handles the file.
 * @param offset Number of bytes to move the file position indicator.
 * @param whence Base position for the offset. Possible values are:
 *               - SEEK_SET : Beginning of the file.
 *               - SEEK_CUR : Current file position.
 *               - SEEK_END : End of file.
 *
 * @return 0 on success. -1 if error occurred, efs_errno is set to reflect the
 *         error.
 */
int efs_efseek (EFS_EFILE *stream, long offset, int whence);

/**
 * @brief Move the file read/write position and gives support upto
 * (4GB-1) bytes filesize.
 *
 * Similar to efs_efseek (). The new position, is obtained by adding
 * <b>offset</b> to the position specified by whence.
 * This API gives support to move the file position beyond 2GB limit
 * with max support upto (4GB-1) filesize limit.
 * The possible values of whence are SEEK_SET, SEEK_CUR and SEEK_END indicating
 * beginning of the file, current file position and end of file respectively.
 *
 * This API allows the file position be moved beyond the end of file. If data
 * is written beyond the end of file, the gap is assumed to be filled with 0
 * until data is written to the gap. Depending on the file system (e.g. FAT,
 * EFS2), the zero filled gap may or may not be written to the device.
 *
 * If the stream is writable and is in buffered mode (by calling
 * efs_esetvbuf()) any unwritten data will be flushed to the file. [POSIX
 * 2004 Standard Sytem Interfaces, Issue 6 line 14894]
 *
 * @param stream Pointer to the stream object that handles the file.
 * @param offset Number of bytes to move the file position indicator.
 * @param whence Base position for the offset. Possible values are:
 *               - SEEK_SET : Beginning of the file.
 *               - SEEK_CUR : Current file position.
 *               - SEEK_END : End of file.
 *
 * @return 0 on success. -1 if error occurred, efs_errno is set to reflect the
 *         error.
 */
int efs_efseek64 (EFS_EFILE *stream, fs_off64_t offset, int whence);

/**
 * @brief Return the current file position.
 *
 * Return the current file position of the stream pointed by <b>stream</b>.
 *
 * @param stream Pointer to the stream object that handles the file.
 *
 * @return On success returns file position.
 *         On error returns -1 and efs_errno is set accordingly.
 */
long efs_eftell (EFS_EFILE *stream);

/**
 * @brief Return the current file position. If filesize is greater then 2GB
 * use this API to get the current file position.
 *
 * Return the current file position of the stream pointed by <b>stream</b>,
 * max file position upto (4GB-1) bytes is returned.
 *
 * @param stream Pointer to the stream object that handles the file.
 *
 * @return On success returns file position.
 *         On error returns -1 and efs_errno is set accordingly.
 */
fs_off64_t efs_eftell64 (EFS_EFILE *stream);

/**
 * @brief Assign buffering to a stream
 *
 * Using a buffer allows read and writes to go into an intermediate memory
 * before writing it to the device. This enables file operations to be in
 * bigger chunks speeding up the write process; while giving up reliability
 * when there is data in the buffer. It is useful in applications where you
 * don't care about reliability or need high read/write throughput. There are
 * two buffering modes:
 *  - EFS_IOFBF fully buffered.
 *  - EFS_IONBF no buffering, same as writing every time to the device. Default
 *              mode when opening a file.
 *  - Line buffering is not implemented (EFS_IOLBF).
 *
 * When a file is opened it's assumed to be in no buffering mode. In fully
 * buffered mode the data is written to the device if the buffer is full or if
 * a efs_efflush(), efs_close(), efs_efseek() or efs_esetvbuf() command is
 * called.
 *
 * If <b>input_buf</b> is a NULL pointer, setvbuf will assume no buffering mode
 * (difference with POSIX standard). If not, it will use the user supplied
 * buf and size. For best performance size should be a multiple of the block
 * size of the device we are writing to. If this information is not known the
 * value of the field f_maxwrite returned by efs_statvfs() or efs_fstatvfs()
 * be used instead. The maximum size of the buffer is 2^32 = 4GBytes.
 *
 * If setvbuf is called and we already are in buffered mode, the buffer is
 * flushed before changing the buffer.
 *
 * When in append mode and using the buffer, all writes are buffered
 * and reads are unbuffered. The file position indicator will move
 * if we read/write but writes will always go at the end of the file in
 * append mode.
 *
 * @param stream File pointer.
 * @param input_buf Buffer to store the data.
 * @param mode Mode of operation (_IOFBF,_I0NFB or _IOLBF).
 * @param size size in bytes of the buffer.
 * @return 0 On success. Different from 0 otherwise.
 *
 */
int efs_esetvbuf (EFS_EFILE *stream, char *input_buf, int mode,
                   fs_size_t size);

/**
 * @brief Flushes the stream to the device.
 *
 * Writes to the device all the buffered data for the given <b>stream</b>. The
 * read/write/append status of the file is not affected.
 *
 * If the <b>stream</b> is NULL, all the files currently open by efs_efopen()
 * are flushed to the device.
 *
 * This blocks until the write process is completed.
 *
 * @param stream Pointer to the file.
 *
 * @return Upon successful completion 0 is returned. Otherwise
 *         EEOF is return and efs_errno and the stream error
 *         are set to indicate the error.
 *
 * @see efs_esetvbuf()
 * @see efs_close()
 * @see efs_write()
 *
 */
int efs_efflush (EFS_EFILE *stream);

/**
 * @brief Return the file descriptor of the stream.
 *
 * Returns the integer of the file descriptor associated with <b>stream</b>.
 *
 * @param stream Pointer to the stream that handles the file.
 *
 * @return On success returns the file descriptor. On error returns -1
 */
int efs_efileno (EFS_EFILE *stream);


#endif /* not __FS_ESTDIO_H__ */
