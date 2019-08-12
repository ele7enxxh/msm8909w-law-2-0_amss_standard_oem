#ifndef TZBSP_FS_H
#define TZBSP_FS_H
/*===========================================================================
  Copyright (c) 2008 - 2013 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_fs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/29/13   yh      Initial version

===========================================================================*/
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
typedef struct tzStat fs_stat;
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

int fstat(int, struct tzStat *);
int lstat(const char *, struct tzStat *);
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

int frename(const char *, const char *);

/*
Custom Function
*/
int file_end(void);

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
typedef struct tzStat {
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
    unsigned long	    st_blksize;      /* blocksize for filesystem I/O */
    unsigned long long  st_blocks;       /* number of blocks allocated */
    unsigned long       st_atime;        /* time of last access */
    unsigned long       st_atime_nsec;
    unsigned long       st_mtime;        /* time of last modification */
    unsigned long       st_mtime_nsec;
    unsigned long       st_ctime;        /* time of last status change */
    unsigned long       st_ctime_nsec;
    unsigned long long  st_ino;          /* inode number */
}__attribute__ ((packed)) tzStat_t;

#define TZ_CM_MAX_NAME_LEN          256   /* Fixed. Don't increase the size of TZ_CM_MAX_NAME_LEN*/
#define TZ_CM_MAX_DATA_LEN          20000

/**
  Commands for :
  HLOS file system services requested by TZ
    			 */
typedef enum
{
  
  /* TZ to HLOS commands - 
  ** Following commands represent services that TZ could request from the HLOS.
  ** Essentially, in these instances, TZ will be the client and HLOS will service the requests.
  */
  TZ_CM_CMD_FILE_START        = 0x00000201,
  TZ_CM_CMD_FILE_OPEN,
  TZ_CM_CMD_FILE_OPENAT,
  TZ_CM_CMD_FILE_UNLINKAT,
  TZ_CM_CMD_FILE_FCNTL,
  TZ_CM_CMD_FILE_CREAT,
  TZ_CM_CMD_FILE_READ,      /**< Read from a file */
  TZ_CM_CMD_FILE_WRITE,     /**< Write to a file */
  TZ_CM_CMD_FILE_CLOSE,     /**< Close a file opened for read/write */
  TZ_CM_CMD_FILE_LSEEK,     /**< Seek to a offset in file */
  TZ_CM_CMD_FILE_LINK,
  TZ_CM_CMD_FILE_UNLINK,
  TZ_CM_CMD_FILE_RMDIR,
  TZ_CM_CMD_FILE_FSTAT,
  TZ_CM_CMD_FILE_LSTAT,
  TZ_CM_CMD_FILE_MKDIR,
  TZ_CM_CMD_FILE_TESTDIR,
  TZ_CM_CMD_FILE_TELLDIR,
  TZ_CM_CMD_FILE_REMOVE,
  TZ_CM_CMD_FILE_CHOWN_CHMOD,
  TZ_CM_CMD_FILE_END,
  TZ_CM_CMD_FILE_SYNC,
  TZ_CM_CMD_FILE_RENAME, 
  
  TZ_CM_CMD_UNKNOWN           = 0x7FFFFFFF
} tz_common_cmd_type;

#if 0
/** Command structure for initializing shared buffers (SB_OUT
    and SB_LOG)
*/
typedef struct tz_init_sb_req_s
{
  /** First 4 bytes should always be command id */
  tz_common_cmd_type        cmd_id;
  /**<-- Pointer to the physical location of sb_out buffer */
  uint32                    sb_ptr;
  /**<-- length of shared buffer */
  uint32                    sb_len;
} __attribute__ ((packed)) tz_init_sb_req_t;

typedef struct tz_init_qsecom_sb_req_s
{
  /** First 4 bytes should always be command id */
  tz_common_cmd_type        cmd_id;
  /**<-- Pointer to the physical location of sb_out buffer */
  uint32                    sb_ptr;
  /**<-- length of shared buffer */
  uint32                    sb_len;
  /**<-- svc for which the shared buffer */
  uint32                    svc_id;
} __attribute__ ((packed)) tz_init_qsecom_sb_req_t;

typedef struct tz_init_sb_rsp_s
{
  /** First 4 bytes should always be command id */
  tz_common_cmd_type        cmd_id;
  /**<-- Return code, 0 for success, Approp error code otherwise */
  int32                     ret;
} __attribute__ ((packed)) tz_init_sb_rsp_t;
#endif

/** Command structure for file open
*/
typedef struct tz_file_open_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Pointer to file name with complete path */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
  /** File status flags */
  int                     flags;
} __attribute__ ((packed)) tz_file_open_req_t;

typedef struct tz_file_open_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int                     ret;
} __attribute__ ((packed)) tz_file_open_rsp_t;


/** Command structure for file openat
*/
typedef struct tz_file_openat_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  int                     dirfd;
  /** Pointer to file name with complete path */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
  /** File status flags */
  int                     flags;
} __attribute__ ((packed)) tz_file_openat_req_t;

typedef struct tz_file_openat_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int                     ret;
} __attribute__ ((packed)) tz_file_openat_rsp_t;


/** Command structure for file unlinkat
*/
typedef struct tz_file_unlinkat_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int                     dirfd;
  /** Pathname of file */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
  /** Flags */
  int                     flags;
} __attribute__ ((packed)) tz_file_unlinkat_req_t;

typedef struct tz_file_unlinkat_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_unlinkat_rsp_t;


/** Command structure for file fcntl
*/
typedef struct tz_file_fcntl_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int                     fd;
  /** Operation to be performed */
  int                     cmd;
} __attribute__ ((packed)) tz_file_fcntl_req_t;

typedef struct tz_file_fcntl_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type       cmd_id;
  /** Return value depends on operation */
  int                     ret;
} __attribute__ ((packed)) tz_file_fcntl_rsp_t;


/** Command structure for file creat
*/
typedef struct tz_file_creat_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Pathname of file */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
  /** Access modes */
  uint32                  mode; //uint32 is typedef unsigned short in Jade's fs stub
} __attribute__ ((packed)) tz_file_creat_req_t;

typedef struct tz_file_creat_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_creat_rsp_t;


/** Command structure for file read
*/
typedef struct tz_file_read_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int                     fd;
  /** Number of bytes to read */
  uint32                  count;
} __attribute__ ((packed)) tz_file_read_req_t;

typedef struct tz_file_read_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Buffer containing read data */
  uint8                   buf[TZ_CM_MAX_DATA_LEN];
  /**<-- Number of bytes read */
  int32                   ret;
} __attribute__ ((packed)) tz_file_read_rsp_t;


/** Command structure for file write
*/
typedef struct tz_file_write_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int                     fd;
  /** Buffer to write from */
  uint8                   buf[TZ_CM_MAX_DATA_LEN];
  /** Number of bytes to write */
  uint32                  count;
} __attribute__ ((packed)) tz_file_write_req_t;

typedef struct tz_file_write_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type    cmd_id;
  /**<-- Number of bytes written */
  int32                 ret;
} __attribute__ ((packed)) tz_file_write_rsp_t;


/** Command structure for file close
*/
typedef struct tz_file_close_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int                     fd;
} __attribute__ ((packed)) tz_file_close_req_t;

typedef struct tz_file_close_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_close_rsp_t;


/** Command structure for file lseek
*/
typedef struct tz_file_lseek_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int                     fildes;
  /** New offset */
  int32                   offset;
  /** Directive */
  int                     whence;
} __attribute__ ((packed)) tz_file_lseek_req_t;

typedef struct tz_file_lseek_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Resulting offset */
  int32                   ret;
} __attribute__ ((packed)) tz_file_lseek_rsp_t;


/** Command structure for file link
*/
typedef struct tz_file_link_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Pathname of existing file */
  const char              oldpath[TZ_CM_MAX_NAME_LEN];
  /** Pathname of new link to existing file */
  const char              newpath[TZ_CM_MAX_NAME_LEN];
} __attribute__ ((packed)) tz_file_link_req_t;

typedef struct tz_file_link_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_link_rsp_t;


/** Command structure for file unlink
*/
typedef struct tz_file_unlink_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Pathname of file */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
} __attribute__ ((packed)) tz_file_unlink_req_t;

typedef struct tz_file_unlink_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_unlink_rsp_t;


/** Command structure for file rmdir
*/
typedef struct tz_file_rmdir_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Pathname of file */
  const char              path[TZ_CM_MAX_NAME_LEN];
} __attribute__ ((packed)) tz_file_rmdir_req_t;

typedef struct tz_file_rmdir_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_rmdir_rsp_t;


/** Command structure for file fstat
*/
typedef struct tz_file_fstat_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** File descriptor */
  int                     filedes;
} __attribute__ ((packed)) tz_file_fstat_req_t;

typedef struct tz_file_fstat_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Pointer to status structure */
  struct tzStat           buf;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_fstat_rsp_t;


/** Command structure for file lstat
*/
typedef struct tz_file_lstat_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Pathname of file */
  const char              path[TZ_CM_MAX_NAME_LEN];
} __attribute__ ((packed)) tz_file_lstat_req_t;

typedef struct tz_file_lstat_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Pointer to status structure */
  struct tzStat           buf;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_lstat_rsp_t;


/** Command structure for file mkdir
*/
typedef struct tz_file_mkdir_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Pathname of directory */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
  /** Permissions mode */
  uint32                  mode;
} __attribute__ ((packed)) tz_file_mkdir_req_t;

typedef struct tz_file_mkdir_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_mkdir_rsp_t;

/** Command structure for file testdir
*/
typedef struct tz_file_testdir_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Pathname of directory */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
} __attribute__ ((packed)) tz_file_testdir_req_t;

typedef struct tz_file_testdir_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_testdir_rsp_t;

/** Command structure for file telldir
*/
typedef struct tz_file_telldir_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Pathname of directory */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
} __attribute__ ((packed)) tz_file_telldir_req_t;

typedef struct tz_file_telldir_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Current location of directory stream */
  int32                   ret;
} __attribute__ ((packed)) tz_file_telldir_rsp_t;

/** Command structure for file remove
*/
typedef struct tz_file_remove_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Pathname of directory */
  const char              pathname[TZ_CM_MAX_NAME_LEN];
} __attribute__ ((packed)) tz_file_remove_req_t;

typedef struct tz_file_remove_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_remove_rsp_t;

/** Command structure for file chown and chmod
*/
typedef struct tz_file_chown_chmod_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** memory for path */
  char                    path[TZ_CM_MAX_NAME_LEN];
  /** the length of path */
  uint32                  path_len;
  /** memory for word */
  char                    word[TZ_CM_MAX_NAME_LEN];
  /** the length of word */
  uint32                  word_len;
  /** memory for owner e.g., media.system */
  char                    owner[TZ_CM_MAX_NAME_LEN];
  /** the length of owner */
  uint32                  owner_len;
  /** memory for mod, e.g., 777 */
  char                    mod[TZ_CM_MAX_NAME_LEN];
  /** the length of mod */
  uint32                  mod_len;
   /** the level */
  uint32                  level;
} __attribute__ ((packed)) tz_file_chown_chmod_req_t;

typedef struct tz_file_chown_chmod_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** 0 for Success/ -1 for failure */
  int                     ret;
} __attribute__ ((packed)) tz_file_chown_chmod_rsp_t;

/** Command structure for file end
*/
typedef struct tz_file_end_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
} __attribute__ ((packed)) tz_file_end_req_t;

typedef struct tz_file_end_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type          cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_end_rsp_t;

/* Command structure for file sync*/
typedef struct tz_file_sync_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** File descriptor */
  int              fd;
} __attribute__ ((packed)) tz_file_sync_req_t;

typedef struct tz_file_sync_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;
  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_sync_rsp_t;

/* Command structure for file rename*/
typedef struct tz_file_rename_req_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;

  /** file name to be changed from */
  const char              oldfilename[TZ_CM_MAX_NAME_LEN];

  /** file name to be changed to */
  const char              newfilename[TZ_CM_MAX_NAME_LEN];  
} __attribute__ ((packed)) tz_file_rename_req_t;

/* Command structure for file rename*/
typedef struct tz_file_rename_rsp_s
{
  /** First 4 bytes are always command id */
  tz_common_cmd_type      cmd_id;

  /** Success/failure value */
  int                     ret;
} __attribute__ ((packed)) tz_file_rename_rsp_t;

#endif //TZBSP_FS_H
