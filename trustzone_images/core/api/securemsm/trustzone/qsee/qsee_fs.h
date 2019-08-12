#ifndef QSEE_FS_H
#define QSEE_FS_H
/*===========================================================================
  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_fs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/14   rk      Added support for open_dir, read_dir
02/04/14   rk      Merged missing changes from fs.h while removing from qsapps/libs
11/16/11   cz      Added the file_dir_chown_chmod function
02/18/11   CM      Initial Version. 
03/11/11   jct     Defined command structures and additional types

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup playready
  @} */
#include <stddef.h>
#include <comdef.h>

/****************************************************************************
 copided from linuxbuild/libc/kernel/common/asm-generic/errno-base.h
****************************************************************************/
#define EPERM 1  
#define ENOENT 2  
#define ESRCH 3  
#define EINTR 4  
#define EIO 5  
#define ENXIO 6  
#define E2BIG 7  
#define ENOEXEC 8  
#define EBADF 9  
#define ECHILD 10  
#define EAGAIN 11  
#define ENOMEM 12  
#define EACCES 13  
#define EFAULT 14  
#define ENOTBLK 15  
#define EBUSY 16  
#define EEXIST 17  
#define EXDEV 18  
#define ENODEV 19  
#define ENOTDIR 20  
#define EISDIR 21  
#define EINVAL 22  
#define ENFILE 23  
#define EMFILE 24  
#define ENOTTY 25  
#define ETXTBSY 26  
#define EFBIG 27  
#define ENOSPC 28  
#define ESPIPE 29  
#define EROFS 30  
#define EMLINK 31  
#define EPIPE 32  
#define EDOM 33  
#define ERANGE 34  

/*
 copied from linuxbuild\bionic\libc\kernel\common\linux\linux.h
*/

#define S_IFMT 00170000
#define S_IFSOCK 0140000
#define S_IFLNK 0120000
#define S_IFREG 0100000
#define S_IFBLK 0060000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFIFO 0010000
#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000

#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

/*
 the following part are copied from linuxbuild\bionic\libc\kernel\common\asm-generic\fcntl.h
*/
#define O_ACCMODE 00000003
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_RDWR 00000002
#ifndef O_CREAT
#define O_CREAT 00000100  
#endif
#ifndef O_EXCL
#define O_EXCL 00000200  
#endif
#ifndef O_NOCTTY
#define O_NOCTTY 00000400  
#endif
#ifndef O_TRUNC
#define O_TRUNC 00001000  
#endif
#ifndef O_APPEND
#define O_APPEND 00002000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 00004000
#endif
#ifndef O_SYNC
#define O_SYNC 00010000
#endif
#ifndef FASYNC
#define FASYNC 00020000  
#endif
#ifndef O_DIRECT
#define O_DIRECT 00040000  
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE 00100000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 00200000  
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 00400000  
#endif
#ifndef O_NOATIME
#define O_NOATIME 01000000
#endif
#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK
#endif

/* Values for the WHENCE argument to lseek.  
   copied from unistd.h
*/
# define SEEK_SET 0 /* Seek from beginning of file.  */
# define SEEK_CUR 1 /* Seek from current position.  */
# define SEEK_END 2 /* Seek from end of file.  */

/*
 the following part are copied from linuxbuild\bionic\libc\include\sys\stat.h
*/
/* really matches stat64 in the kernel, hence the padding
 * Note: The kernel zero's the padded region because glibc might read them
 * in the hope that the kernel has stretched to using larger sizes.
 */

#ifndef fs_stat
typedef struct _fs_stat {
    unsigned long long  st_dev;          /* ID of device containing file */
    unsigned char       __pad0[4];
    unsigned long       __st_ino;
    unsigned int        st_mode;         /* protection */
    unsigned int        st_nlink;        /* number of hard links */
    unsigned long       st_uid;          /* user ID of owner */
    unsigned long       st_gid;          /* group ID of owner */
    unsigned long long  st_rdev;         /* device ID (if special file) */
    unsigned char       __pad3[4];
    long long           st_size;         /* total size, in bytes */
    unsigned long     st_blksize;      /* blocksize for filesystem I/O */
    unsigned long long  st_blocks;       /* number of blocks allocated */
    unsigned long       st_atime;        /* time of last access */
    unsigned long       st_atime_nsec;
    unsigned long       st_mtime;        /* time of last modification */
    unsigned long       st_mtime_nsec;
    unsigned long       st_ctime;        /* time of last status change */
    unsigned long       st_ctime_nsec;
    unsigned long long  st_ino;          /* inode number */
}__attribute__ ((packed)) fs_stat;

#endif

#ifndef mode_t
typedef unsigned short mode_t;
#endif

#ifndef off_t
typedef long off_t;
#endif

//#ifndef size_t
//typedef uint32 size_t;
//#endif

#ifndef ssize_t
typedef int32 ssize_t;
#endif

/*
 the following part are copied from linuxbuild\bionic\libc\include\fcntl.h
*/
int  open(const char*  path, int  mode, ...);
int  dbg_open(void *cmd, const char*  path, int  mode, ...);

int  openat(int fd, const char*  path, int  mode, ...);
int  unlinkat(int dirfd, const char *pathname, int flags);
int  fcntl(int   fd, int   command, ...);
int  creat(const char*  path, uint32  mode);

/*
 the following part are copied from linuxbuild\bionic\libc\include\unistd.h
*/
int32 read(int, void *, uint32);
int32 write(int, const void *, uint32);
int close(int);
long lseek(int, long, int);
int link(const char *, const char *);
int unlink(const char *);
int rmdir(const char *);

int fstat(int, fs_stat *);
int lstat(const char *, fs_stat *);
int mkdir (const char *, uint32);
int testdir(const char *);
int32 telldir(const char *);
int remove(const char *);
int fsync(int fd);
/**
 * @param pathname: path: e.g., /persist/playready/drmms/
 *                  word: e.g., "playerady" 
 *              word_len: strlen(playerady)
 *                 owner: e.g., "media.system"
 *             owner_len: e.g., strlen(media.system)
 *                   mod: e.g., "700" or "777"
 *               mod_len: e.g., strlen("777")
 *                 level: e.g., 0 : chown media.system /persist/playready and chmod 700 /persist/playready 
 *                 level: e.g., 1 : chown media.system /persist/playready and chmod 700 /persist/playready 
 *                                  chown media.system /persist/playready/ * and chmod 700 /persist/playready/ * 
 *                 level: e.g., 2 : chown media.system /persist/playready and chmod 700 /persist/playready 
 *                                  chown media.system /persist/playready/ * and chmod 700 /persist/playready/ * 
 *                                  chown media.system /persist/playready/ * / * and chmod 700 /persist/playready/ * / * 
 * @return int: 0 for success, -1 upon failure
 */
int file_dir_chown_chmod
(
  char*  path,
  uint32 path_len,
  char* word,
  uint32 word_len,
  char* owner,
  uint32 owner_len,
  char* mod,
  uint32 mod_len,
  uint32 level
);

/**
 * Rename file or directory
 *
 * @param oldfilename: name of file to be renamed from
 * @param newfilename: name of file to be renamed to
 *
 * @return int: 0 for success, -1 upon failure
 */
int frename(const char *, const char *);


/**
 * Open a directory
 *
 * @param path: path of the directory to open
 *
 * @return DIR*: directory stream pointer for success, NULL upon failure
 */
 typedef void DIR;
DIR* opendir(const char *path);


/**
 * Read a directory
 *
 * @param pdir: directory stream pointer
 * @param entry: pointer to directory entry struct to be filled as a result
 *
 * @return struct tzDirent*: same as entry for success, NULL upon failure
 */
struct tzDirent* readdir(DIR* pdir, struct tzDirent* entry);

/*
Custom Function
*/
int file_end(void);

/**
 * Return the free size of a specified partition 
 * @param pathname: 
                   [in]     path: e.g., "/persist/playready"
                            the function parses the partition name "/persist"
                   [in/out] size: the free size left of the partition persist
 * @return int: 0 for success, -1 upon failure
 */
int file_get_partition_free_size(const char *path, uint64 *size);

#endif //FS_H
