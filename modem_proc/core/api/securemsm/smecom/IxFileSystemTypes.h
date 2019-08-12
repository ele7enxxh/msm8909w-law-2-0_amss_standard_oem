#ifndef __IXFS_TYPES_H__
#define __IXFS_TYPES_H__

/**
   @file IxFileSystemTypes.h

   @brief
   Type definitions for IxFile file systems.

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/
/*===========================================================================
                  I X  F I L E S Y S T E M T Y P E S 
                       H E A D E R  F I L E

Copyright (c) 2007-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                     EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxFileSystemTypes.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
12/10/10   nk      Doxygenated.
11/01/07   jee     initial release

===========================================================================*/

/** @addtogroup SMECOMFILE 
@{ 
*/

/**
  Defines the maximum number of characters for the file name.
*/
#define IXFILE_NAME_MAX                512

/**
   @brief
   Placeholder for file information.
*/
typedef struct
{
   int  fileType;                 /**< Describes the file type.            */
   char name[IXFILE_NAME_MAX+1];  /**< Places a holder for the file name. */
} IxFileDirEntT;

/**
   Placeholder for file information in Open File Method (OFM) mode.

   This enumeration contains various meaningful bits in the mode parameter
   passed to the OpenFile method. Some commonly used values of the mode
   parameter are:

   - IxFile_OFM_READ -- Opens the file for reading. Fails if the file does not
                        exist.
   - IxFile_OFM_READWRITE -- Opens the file for reading and writing. Fails if
                             the file does not exist.
   - IxFile_OFM_READWRITE | IxFile_OFM_CREATE -- Opens the file for reading and
                                                 writing. Creates the file 
                                                 if it does not exist.
   - IxFile_OFM_CREATE | IxFile_OFM_TRUNCATE | IxFile_OFM_READWRITE -- Opens the
                         file for reading and writing. Truncates the file if it
                         already exists. Creates the file if it does not exist.
   - IxFile_OFM_APPEND -- Opens the file for reading and writing, with all writes
                          occurring after a seek to the end. Fails if the file
                          does not exist.
   - IxFile_OFM_APPEND | IxFile_OFM_CREATE -- Opens the file for reading and
                         writing, with all writes occurring after a seek to the
                         end. Creates the file if it does not exist.

*/
typedef enum {
   IxFile_OFM_READ      = 0x0001,    /**< Read mode.       */
   IxFile_OFM_READWRITE = 0x0002,    /**< Read/write mode. */
   IxFile_OFM_CREATE    = 0x0004,    /**< Create mode.     */
   IxFile_OFM_APPEND    = 0x0008,    /**< Append mode.     */
   IxFile_OFM_TRUNCATE  = 0x0010,    /**< Truncate mode.   */
   IxFile_OFM_BUFFERED  = 0x0020,    /**< Buffered mode.   */
   IxFile_OFM_RAM_FILE  = 0x0040     /**< RAM file. @latexonly \newpage @endlatexonly */
}IxFileOFMModeType;

/**
   Unsigned integer placeholder for File Open mode.
*/
typedef unsigned int IxFileOpenFileMode;

/**
   @name FA File Constants and Defines
@{ */

/** Defines the system Functional Area (FA) as normal.
*/
#define IXFILE_FA_NORMAL      (0x0)

/** Defines the system FA as hidden.
*/
#define IXFILE_FA_HIDDEN      (0x1)

/** Defines the system FA as a directory.
*/
#define IXFILE_FA_DIR         (0x2)

/** Defines the system FA as read only.
*/
#define IXFILE_FA_READONLY    (0x4)

/** Defines the system FA as a system.
*/
#define IXFILE_FA_SYSTEM      (0x8)

/** Defines the system FA as a constant. The file is in the ROM.
*/
#define IXFILE_FA_CONST       (0x10)

/** Defines the system FA as fixed. The file cannot be overridden.
*/
#define IXFILE_FA_FIXED       (0x20)

/** @} */ /* end_name FA File Constants and Defines */

/** @name Macros for File Operations
   The following values have been set to match those 
   used by EFS2. They do not need to match, but using 
   these values is convenient.
@{ */

#define IXFILE_S_IFCHR  0020000        /**< Character device.   */
#define IXFILE_S_IFDIR  0040000        /**< Directory.          */
#define IXFILE_S_IFBLK  0060000        /**< Block device.       */
#define IXFILE_S_IFREG  0100000        /**< Regular file.       */
#define IXFILE_S_IFLNK  0120000        /**< Symbolic link (symlink). */
#define IXFILE_S_IFITM  0160000        /**< Item file.          */
#define IXFILE_S_IFMT   0170000        /**< Mask of all values. */

/** @} */ /* end_name Macros for File Operations */

/** @name Macros for Checking File Type
   The following values have been set to match those 
   used by EFS2. They do not need to match, but using 
   these values is convenient.
@{ */

#define IXFILE_S_ISCHR(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFCHR)
/**< Character device. */

#define IXFILE_S_ISDIR(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFDIR)
/**< Directory. */

#define IXFILE_S_ISBLK(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFBLK)
/**< Block device. */

#define IXFILE_S_ISREG(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFREG)
/**< Regular file. */

#define IXFILE_S_ISLNK(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFLNK)
/**< Symlink. */

#define IXFILE_S_ISITM(m)      (((m) & IXFILE_S_IFMT) == IXFILE_S_IFITM)
/**< Item file. */

/** @} */ /* end_name Macros for Checking File Type */

/**
   @brief
   Placeholder for file status information, containing date, size, and type
   information for the file.
*/
typedef struct
{
unsigned long creationDate;   /**< Creation date. */
unsigned long size;           /**< File size.     */
int fileType;                 /**< File type.     */
} IxFileStatType;

/**
   @brief 
   Placeholder for file File System (FS) status information, containing total
   bytes, free bytes, bytes used, FS ID, and maximum file name length information
   for the file.
*/
typedef struct
{
unsigned long totalBytes;       /**< Total bytes.              */
unsigned long freeBytes;        /**< Free bytes.               */
unsigned long usedBytes;        /**< Bytes used.               */
unsigned long fsID;             /**< FS ID.                    */
unsigned long maxFileNameLen;   /**< Maximum file name length. */
} IxFileStatFSType;

/** @} */ /* end_group SMECOMFILE */

#endif /* __IXFS_TYPES_H__ */
