#ifndef _SFS_H_
#define _SFS_H_
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
 
                           Secure File Sytem

                  I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This header file is the internal header file for the secure file system.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2004-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCVPath: $
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/sfs/shared/inc/sfs.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/13   yh      MPSS 3.0 SFS POR
11/10/11   yh      sfs transaction write
07/16/10   ejt     Added #if defined(T_MSM8660) since pmem_malloc does not work on this target
03/31/10   ejt     Added support for sfs_priv_meid_read and sfs_priv_meid_write APIs 
04/17/09   ejt     added sfs_seg_flush routine 
03/22/09   sm      removed FEATURE_SEC_SFS_SECURE_PARTITION and kept functionality
12/29/08   ejt     removed 32 byte data alignment and moved it to crypto unit
12/17/08   ejt     removed SFS_SYSTEM_INTERFACE from SFS files
12/10/08   ejt     merged in sfs_dev_key_get & sfs_construct_unique_key
12/05/08   ejt     added 32 byte alignment fix
09/24/08   ejt     fixed spelling of FEATURE_SEC_SFS_SECURE_PARTITION
03/28/07   df      support for 7500 hw key
04/04/07   df      adding MINIMAL_DUAL_PD featurization
03/26/07   df      migrate malloc to UxMalloc
03/01/07   df      support for seek
02/01/07   df      added swid sector
01/09/06   df      local support for GetInfo
12/22/06   df      support for usfs
12/20/06   avm     use extern C malloc definition
12/05/06   df      convert to cpp
                   CR107506 Heap corruption due to path length
11/08/06   df      added support for 7500 pmem_malloc
04/04/06   df      removed AEE_ macros
03/22/06   df      numerous checks added
                   changes to support seed sector on secure partition
01/08/06   df      changes for critical sections
                   added more secure partition files
12/11/05   df      removed OEMHeap dependancy for thinUI builds
11/12/05   df      support for secure flash partition
09/29/05   df      updated name to OEMHeap.h header file
07/25/05   df      do not allow sfs access via diag (sfs remote access check)
07/22/05   df      added FEATURE_SEC_SFS
                   added nv item access for 6250 (FEATURE_SEC_SFS_NV_ITEM)
                   move char arrays to end of handle struct (for better align)
02/27/05   df      added missing proto sfs_def_init()

===========================================================================*/


#include "core_variation.h"
#ifdef FEATURE_SEC_SFS
#include "comdef.h"
#include "secerrno.h"
#include "UxCommon.h"         //UxMalloc
#include "sfs_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "secapi.h"
#include "fs_public.h"
#include "oemsbl_sfs.h"

#ifdef __cplusplus
}
#endif

#ifdef errno
#undef errno
#endif

/* this is for malloc defs */
#define SFS_MALLOC(m) UxMalloc(m, UX_ALLOC_LOCAL)
#define SFS_FREE(m) if(m){ UxFree(m);}

//Need to add MSM8660 since pmem_malloc does not work on this target
#if defined(T_MSM8660)
 #define SFS_SEG_MALLOC SFS_MALLOC
 #define SFS_SEG_FREE   SFS_FREE
#elif defined(T_MSM7500)
 #include "pmem.h"
 #define SFS_SEG_MALLOC(s) pmem_malloc(s, PMEM_UNCACHED_ID)
 #define SFS_SEG_FREE pmem_free
#else
 #define SFS_SEG_MALLOC SFS_MALLOC
 #define SFS_SEG_FREE   SFS_FREE
#endif


#define SFS_MAX_PATH_SIZE     256    
#define SFS_MAX_DRIVE_NAME_SIZE 8    //usr_sfs: sys_sfs:

#define SFS_DIR_CHAR '/'
#define SFS_DIR_STRING "/"

#define SFS_MAX_IX_ENTRY   0xfffffffe
#define SFS_MAX_SEGS       0xfffffffe
#define SFS_DEF_SEG_SIZE   1024 // 1K 
#define SFS_IX_SEG_SIZE    SFS_DEF_SEG_SIZE
#define SFS_LIST_SIZE      16
#define SFS_SUFFIX_SIZE    13  // ".sfs00000001" including null term
#define SFS_IX_SUFFIX_SIZE 19  // ".index.sfs00000001" including null term
#define SFS_MANGLED_SIZE   28
#define SFS_MANGLED_STR_SIZE (SFS_MANGLED_SIZE + 1) // add the null terminator
#define SFS_ENDEC_ENCRYPT  0
#define SFS_ENDEC_DECRYPT  1
#define SFS_HMAC_CREATE    0
#define SFS_HMAC_VERIFY    1
#define SFS_EMPTY_ENTRY_SIZE  2

#define SFS_SEG_FILE_SIZE(seg_size) ((seg_size) + \
                                     SFS_AES_BLOCK_SIZE + SFS_HMAC_SIZE)

#define SFS_FLASH_DEV_NAME             "_/sfs_flash_dev/"
#define SFS_FLASH_DEV_NAME_SIM         "_/sfs_flash_dev/sim"
#define SFS_FLASH_DEV_NAME_IMEI        "_/sfs_flash_dev/imei"
#define SFS_FLASH_DEV_NAME_PFILE       "_/sfs_flash_dev/pfile"
#define SFS_FLASH_DEV_NAME_RESV        "_/sfs_flash_dev/resv"
#define SFS_FLASH_DEV_NAME_MEID        "_/sfs_flash_dev/meid"
#define SFS_FLASH_DEV_NLEN             (sizeof(SFS_FLASH_DEV_NAME) - 1) 


typedef enum
{
  SFS_NO_ERROR = 0,
  SFS_OPEN_ERROR,
  SFS_CLOSE_ERROR,
  SFS_READ_ERROR,
  SFS_WRITE_ERROR,
  SFS_VERIFICATION_ERROR,
  SFS_KEY_ACCESS_ERROR,
  SFS_EOF,
  SFS_RESV = 0x7fffffff
}sfs_errno_enum_type;


typedef struct
{
  uint32 cookie;
  uint32 num_segs;           // total number of segments used by index file, always 1
  uint32 num_entries;        // total number of entries in index file, including NULL entry
  uint32 max_entry;          // total number of user files + 1(index file)
  char   last_write_file_name[SFS_MAX_FILENAME_SIZE];
}sfs_ix_entry0_type;

typedef struct
{
  uint32 cookie;
  uint32 seg_size;           // index file, data file seg_size are different
  uint32 file_size;          // actual number of "data" bytes in file
  char   file_name[SFS_MAX_FILENAME_SIZE];
  uint8  hmac[SFS_HMAC_SIZE];
}sfs_ix_entry1_type;

typedef struct
{
  union
  {
    sfs_ix_entry0_type en0;  // the zeroth entry has general entry info
    sfs_ix_entry1_type en;   // the first entry has index file info
  }entry;                    // the second entry has reg file info
}sfs_ix_entry_type;

#define SFS_IX_ENTRY_SIZE sizeof(sfs_ix_entry)

typedef struct
{
  /* file info */
  uint32  cookie;
  sfs_errno_enum_type _errno;
  uint32  flags;             /* O_READ, O_WRITE, O_CREATE etc */
  uint32  mode;              /* read = 0 / write = 1*/
  uint32  type;              /* normal, index, flashdev, etc */
  uint32  curr_pos;          /* 0..file_size - 1 */
  uint32  file_size;         /* number of bytes in file (can be zero) */
  uint32  dirty_file;        /* if file is dirty, update index file on close */
  uint8   master_hmac[SFS_HMAC_SIZE];       /* hmac over all segments */
  uint32  is_key_valid;      /* '1' if key field from below is valid. '0' otherwise.
                                 Key field is always valid when this is User SFS
                                 Key field is valid on System SFS if
                                  - Using MSM Device key on 7K targets on Modem ARM
                                  - Using MSM Device key on 7K targets on Apps ARM (here
                                    the key is a derivation of device key)
                                  - Using OEM Key as defined in sec_oem* layer.
                                  - Using a Key from Secure Partition, as controlled by
                                    sec_oem* function.
                             */
  uint8   key[SFS_SYS_KEY_SIZE];
  uint8   hmac_key[SFS_SYS_KEY_SIZE];

  /* segment info */
  uint8* seg_buf;               // must be malloc'd for each instance, 32 byte aligned pointer
  uint32 seg_size;              // N * block size (multiple block sizes)
  uint32 curr_seg;              // 0..M-1
  uint32 opened_seg;            //   for now reserve 0xffffffff for initializing
  uint32 decrypted_seg;
  uint32 num_segs;              // M
  uint32 seg_pos;

  uint32 dirty_seg;
  uint32 ver_seg;
  uint32 ix_num;           /* entry number in index file */
  sfs_segfh_type  seg_fh;


  /* seg names */
  char    seg_name[SFS_MAX_FILENAME_SIZE];     // "filename.sfs0"
  char    seg_b_name[SFS_MAX_FILENAME_SIZE];   // "filename.b.sfs0"
  char    m_seg_npath[SFS_MAX_FILENAME_SIZE];  // "root_dir\mdir\m_seg_name"
  char    m_seg_b_npath[SFS_MAX_FILENAME_SIZE];// "root_dir\mdir\m_seg_b_name"
  char    m_seg_name[SFS_MANGLED_STR_SIZE];    // m"filename.sfs0"
  char    m_seg_b_name[SFS_MANGLED_STR_SIZE];  // m"filename.b.sfs0"

  /* path and dir info */
  char  file_name[SFS_MAX_FILENAME_SIZE];     //  "filename"
  char path[SFS_MAX_PATH_SIZE];        // path given to open command
  char  root_dir[SFS_MAX_FILENAME_SIZE];      // the "root dir"
  char dir[SFS_MAX_FILENAME_SIZE];     // cwd to be mangled
  char mdir[SFS_MANGLED_STR_SIZE];     // mangled dir name includes null term
  char  tmp_path[SFS_MAX_FILENAME_SIZE];    // temp or scratch path

  /* index file info */
  char   ix_file_name[SFS_MAX_FILENAME_SIZE]; // "dir.index"
  char   mix_file_name[SFS_MANGLED_STR_SIZE]; // m"dir.index"
  char   mix_file_b_name[SFS_MANGLED_STR_SIZE];// "dir.index.b"
  char   mix_file_d_name[SFS_MANGLED_STR_SIZE];// m"dir.index.sfs0...n"

}sfs_handle_type;

/* SFS data is split into segment, default segment size can be set inside sfs_init function */
extern uint32 sfs_default_seg_size;

/* externalized functions */
void sfs_register_remote_access_check (void);

// client side interface
secerrno_enum_type sfs_open_seg(sfs_handle_type* fh);
secerrno_enum_type sfs_close_seg(sfs_handle_type* fh);
secerrno_enum_type sfs_seg_hmac (sfs_handle_type *fh, int mode);
secerrno_enum_type sfs_mangle_seg_npath (sfs_handle_type* fh, uint32 curr_seg);
secerrno_enum_type sfs_seg_endec (sfs_handle_type *fh, int mode);
secerrno_enum_type sfs_mangle (char *out, char *in, int in_len);
secerrno_enum_type sfs_close_mfile(sfs_handle_type* fh);
secerrno_enum_type sfs_trunc(sfs_handle_type* fh);
secerrno_enum_type sfs_seg_flush(sfs_handle_type* fh);
secerrno_enum_type sfs_delete(const char *path);
secerrno_enum_type sfs_def_init (uint32 seg_size);
secerrno_enum_type sfs_get_size (sfs_handle_type* fh, uint32* size);
secerrno_enum_type sfs_dir_create(const char *path);
secerrno_enum_type sfs_ixfile_dir_create
(
  char *new_path, 
  const char* path,
  uint32 new_path_len
);

sfs_handle_type *sfs_open_mfile(const char* path, int flags);
int sfs_write_bytes(sfs_handle_type* fh, const char* buf, int num_bytes);
int sfs_read_bytes(sfs_handle_type* fh, char* buf, int num_bytes);
int sfs_seek_pos(sfs_handle_type* fh, int32 offset,int whence);
void sfs_backup(sfs_handle_type *fh, uint32 num_segs);
void sfs_rm_backup(sfs_handle_type *fh, uint32 num_segs);
void sfs_rollback_write_transaction(sfs_handle_type *fh);
secerrno_enum_type sfs_key_get (sfs_handle_type* fh);
secerrno_enum_type sfs_generate_dir_name(const char* path, char* sfs_path);
secerrno_enum_type sfs_get_path (sfs_handle_type* fh, const char* path);

#ifdef __cplusplus
extern "C"
{
#endif
#include "rex.h"
typedef rex_crit_sect_type sfs_mutex_type;
#define SFS_MUTEX_INIT(m)  rex_init_crit_sect(&m)
#define SFS_MUTEX_ENTER(m) rex_enter_crit_sect(&m)
#define SFS_MUTEX_EXIT(m)  rex_leave_crit_sect(&m)

/* mutex for secure partition access */
extern sfs_mutex_type        sfs_sp_mutex;
extern sfs_mutex_type        sfs_ix_mutex;

secerrno_enum_type sfs_read_hw_keys(void *key, int len);
secerrno_enum_type sfs_dev_key_get(uint8 *key, unsigned int *key_len);
secerrno_enum_type sfs_construct_unique_key(uint8 *key, unsigned int key_len);

#ifdef __cplusplus
}
#endif

#endif // FEATURE_SEC_SFS
#endif // _SFS_H_
