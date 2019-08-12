/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====
 *
 *                 Diagnostics Packet Processing
 *
 *
 *  Copyright (C) 2002-2014  QUALCOMM Technologies, Incorporated.
 *  All Rights Reserved.
 *
 *  Diagnostic packet processing routines for EFS2
 *
 *
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


 /*===========================================================================

                           Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_diag.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/15   mj      Check for buffer overreads in FS Diag handlers.
03/23/15   mj      Access check in truncate and set_reservation diag handlers.
10/13/14   mj      Added access check in multiple diag handlers.
10/07/14   mj      Normalize input path before compare in diag_deltree_handler
05/06/14   vm      Allow Golden Copy to be created multiple times.
04/26/14   vm      Use correct FS_MSG macros to save RAM.
10/10/13   rp      Fix LLVM compiler warnings.
10/09/13   nr      Replace banned memory functions with safer versions.
09/30/13   wek     Fix allocation size for rsp pkt in read/write file.
01/31/13   rp      When EFS is not dirty, make efs_sync_no_wait return error.
01/11/13   dks     Move writing to Golden partition outside of efs_sync.
01/11/13   nr      Add timeout errno in diag errno list.
10/18/12   wek     Make efs_raw_put/get wrappers around efs_put/get.
10/12/12   rp      Fix hotplug_device_info to return ENODEV.
06/05/12   nr      Fix buffer over read in diag benchmark start test handler.
05/18/12   wek     Remove jzap header file.
05/08/12   dks     Support storing/reading cookies to/from emmc partitions.
04/12/12   wek     Cleanup registering with diag in multiple processors.
03/20/12   wek     Cleanup customer header file inclusion.
03/08/12   wek     Rename macros and remove header files to compile for boot.
03/02/12   nr      Fix the diag error returned in the response packets.
11/07/11   dks     Fix compiler warnings in 9x15.
11/02/11   wek     Use library supplied strl cpy and cat instead of fs_strl*.
11/02/11   nr      Klocwork error fixes in Modem code.
10/04/11   dks     Make EFS VU independent of Hotplug and Hfat VUs.
08/14/11   wek     Get the errno from global header file.
06/30/11   rp      Add File System Image diag packets.
05/03/11   wek     Rename local variables named errno.
04/21/11   wek     Remove compiler warnings.
03/25/11   dks     Support Golden-Copy in RAM for fusion.
10/22/10    vr     Restore exact size validation removed in Klocwork fixes
10/11/10    vr     Klocwork Bug fixes
05/06/10   wek     Implement golden copy for remote storage page manager.
04/23/10   yog     Added diag packets for 64 bit APIs.
02/19/10    rp     Maintain same API files between 1.2 and 2.0 codebases.
01/19/10   dks     Added diag commands for efs sync APIs for remote fs.
11/04/09   dks     Changed fs_diag_device_info_handler to support all dev types
07/30/09    yg     Make max packet size definitions overridable.
07/08/09    ds     memcpy-ed the filename to rsp pkt in fs_diag_iter.
05/27/09   wek     Ported diag commands (code 89) to use EFS2 calls.
05/18/09    rp     Fix compiler warning for 8200 which has gcc with more -Ws.
05/15/09    sh     Corrected types for MD5 functions.
04/21/09    ds     Removed a compilation warning in fs_diag_open_handler.
01/12/09   yog     Add support for hotplug_dev_get_device_info diag packet.
12/12/08    yg     Included fs_diag_i.h internal header file.
12/04/08   yog     Add support for idle_device_event_duration diag packet.
10/06/08    np     Add support for efs_shred diagnostic packet.
09/19/08   wek     Changed fs_diag_hotplug_format to use hotplug_format().
08/22/08    yg     Remove all simulator specific preprocessor macros.
07/23/08   wek     Added support for hotplug format diagnostic packet.
05/22/08    rp     Added back PACKED and PACKED_POST for void * arguments.
05/20/08    eb     Removed PACKED and PACKED_POST for void * arguments
04/03/08    eb     Added PACKED_POST directive for GCC support.
03/20/08    sh     Check resonses for NULL.
03/14/08    rp     Fix memset() compiler error in fs_diag_statvfs_v2_handler.
01/11/08    eb     Added handler fs_diag_statvfs_v2_handler
02/22/08    np     Add support for md5sum calculation diag packet.
10/29/07    sch    Assigned 0 to diag_errno if deltree/truncate succeeds
08/30/07    np     Add support for efs_truncate diagnostic packets.
08/19/07    rp     Include fs_global_lock.h due to FS_GLOBAL_LOCK() cleanup.
03/20/07    sh     Avoid the use of the MIN macro.
02/12/07    sh     Don't pass packed strings to strlcpy.
01/23/07    sh     Correctly implement alternate Diag subsystem ID
01/12/07   dlb     Use safer string functions.
12/22/06    sh     Remove accidental Hotplug dependency.
12/21/06    sh     Lint refinement.
12/14/06    sh     Lint changes.
12/14/06    sh     Access validation cleanup.
12/13/06   umr     Add sequence_number support to efs_deltree handler.
12/12/06    sh     Lint fixes. (db)
12/06/06   umr     Added diag interface for Sequential [In Order] put / get
12/08/06   umr     Deprecated put / get request handlers.
10/19/06   umr     Added diag interface for efs_deltree ()
10/26/06    sh     Featurize CEFS Factory Image code (security risk).
10/26/06    sh     Perform Access Validation against READ/WRITE during OPEN.
10/10/06    sh     Allow target conf to override EFS Subsystem ID
09/19/06    sh     Rename all instances of response errno to diag_errno.
07/12/06    sh     Allow target to select processor for FS Diag subsys.
06/29/06    sh     Remove mistaken PACKED.
06/24/06    sh     Replace request->path with a non-__packed ptr.
06/23/06    sh     Fix benchmark packets so they can be built.
06/22/06   dlb     Validate paths for nul termination, for all paths.
06/20/06   dlb     Fix fstat handler for new descriptors.
06/19/06   dlb     Get max rx/tx packets from diagtune.h
06/14/06   dlb     Cleanup of operations involving handles or descriptors.
06/14/06   dlb     Change rsp/req to response/request for clarity.
06/09/06   dlb     More major cleanup and fixing: open, write, read.
06/08/06    sh     Moved fs_diag_efs2_error_rsp into fs_diag.h
05/16/06   dlb     Updates to compile again off target.
05/15/06    sh     Skip any directory entry that errors lstat.
05/08/06    sh     Fixed RVCT warning about packed path
05/07/06    sh     Removed superfluous break/return statements.
04/13/06    sh     Corrected case indenting
03/31/06    sh     Lint cleanup
03/28/06    sh     Removed all EFS1 calls from fs_diag, using only EFS2 now.
02/22/06   nrs     Convert pointers to correct type in new Q&R diag packets
02/06/06    sh     Correct diag seek to use the right file handle
02/01/06    sh     Correcting diag use of stdio
01/26/06    sh     Added packets for chown, and get/set reservation/quota.
12/12/05    sh     Lint cleanup.
12/12/05   nrs     Bug write_handler causes infinite loop
11/28/05   nrs     Merge 7500 changes
11/01/05    sh     Added fs_benchmark handlers
11/01/05   nrs     Fix bug for when MMC is in the build, but not USB
10/31/05   nrs     Fixed USB support in fs_statfs causing checks on MMC
10/04/05   nrs     Initialize file lookup table for stdio interface
10/12/05    sh     Removed some Access Validation checks (READ,WRITE,etc)
10/12/05    sh     Added ZAP macros for debug messages (disabled by default)
10/12/05    sh     Corrected error value for MMC iterator busy.
08/25/05    sh     Featurized stdio interface for MSM7500
08/19/05    sh     Fixed typo '==' instead of assignment.
08/08/05   sch     Added USB support in fs_diag_test_mmc_path
08/05/05    sh     Fix close_status warning in fs_diag_close_handler
08/01/05    sh     fs_diag_open_handler() needs to detect efs_fopen() failure
06/22/05   dlb     Add extended info query packet.
04/27/05   nrs     Re-fix uninitialized close status.
04/26/05   dlb     Fix uninitialized close status.
04/22/05   nrs     Explicitly set close response pointer to 0 in
                   fs_diag_close_handler on success.
04/14/05   nrs     Fix access permission checks in fs_diag_open_handler()
04/07/05   nrs     Implemented write buffering
03/22/05   dlb     Fix RVCT error in efs_get.
03/07/05   nrs     Ignore irrelevant lint warnings
03/04/05   as      Moved validation check before allocating close rsp pkt.
03/03/05   nrs     Fixed lint errors
02/24/05   nrs     Added Generic EFS2 Diag Error packet
02/15/05   nrs     Removed bytes written from put response packet.
02/10/05   nrs     Added packet structures for put and get
01/31/04   sch     Fixed the problem of overwriting an existing file in MMC
12/07/04   nrs     Added temporary fix for delays using QPST
10/14/04   nrs     Fixed leading slash compatability issue from EFS1
10/07/04   dlb     Whitespace cleanup.
06/23/04    gr     Changes to keep the RVCT 2.1 compiler happy.
06/15/04    gr     Disabled security check in the unit test environment.
05/18/04    as     Added security check to fs diag packets.
05/18/04    gr     Fixes for compiler warnings.
05/18/04    gr     Fixed some bad formatting of code.
05/17/04    gr     Set the fstype field correctly in the statfs response.
04/08/04    gr     Modifications to not attempt an MMC enumeration if an MMC
                   card is not present.
10/17/03    gr     Modifications to get this module to work on target as well
                   as in the unit test framework.
09/30/03    pg     Added fs_diag_get_fs_errno to convert efs1 error code to
                   efs2 error code.
                   Modified all handler to return more accurate error codes
                   from MMC operations.
09/14/03   adm     Added support for efs_image_prepare function
08/27/03    gr     Changes to get rid of warnings and errors produced by the
                   RVCT 2.0 compiler.
07/16/03    gr     Moved access validation code into its own file.
07/03/03   spf     Empty directory returned instead of error when MMC/SD card
                   not present.
06/17/03   jkl     Clean up code.
06/16/03   spf     Added factory image support.
                   Minor code cleanup.
06/12/03    gr     Added support for validating access to files and
                   directories.
05/30/03   spf     Added MMC support using EFS1 interface.
04/03/03    gr     Modified the mkdir and rmdir handlers to ignore trailing
                   slashes. This keeps the tools happy.
10/25/02    gr     Created
===========================================================================*/

#include "comdef.h"
#include "fs_config_i.h"
#include "fs_util.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "fs_diag_i.h"
#include "fs_err.h"
#include "fs_efs2.h"
#include "fs_stdio.h"
#include "fs_benchmark.h"
#include "assert.h"

#ifdef FEATURE_EFS2

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "fs_public.h"
#include "fs_errno.h"
#include "diag.h"
#include "fs_diag_access.h"
#include "fs_babylon.h"
#include "fs_global_lock.h"
#include "fs_util.h"
#include "fs_priv_funcs.h"
#include "fs_diag.h"
#include "fs_privatedir.h"
#include "stringl/stringl.h"
#include "fs_diag_golden_copy_handler.h"
#include "fs_stdlib.h"

#ifdef FEATURE_FS_HOTPLUG
  #include "fs_hotplug.h"
#endif

/*
 * Defined locally to avoid going through stdio buffering layer for diag
 */
#define FEATURE_NO_STDIO_FROM_DIAG


/*
 * The diag packets frequently contain a pathname as the last field.
 * This is listed in the structure as char foo[1], while the variable-length
 * pathname extends past the end of the structure.  Not surprisingly, lint
 * sees this as:
 * Error (Warning) 661: Possible access of out-of-bounds pointer
 * (1 beyond end of data) by operator '['
 * This is intentional, so the warning is avoided by casting to
 * dereferencing the string within.  Such is lint.
 */
#define LONG_STRING_CAST(x) ((char *)(void *****)(x))

/* By default, we use the well-defined EFS Subsystem ID
 * (DIAG_SUBSYS_FS) for all EFS diag commands.
 *
 * On multi-processor builds, the build may want to use an alternate
 * EFS Subsystem ID to address the second (slave) processor.  We allow
 * the build configuration to override the EFS Subsystem ID here, by
 * declaring EFS_DIAG_USE_THIS_SUBSYS_ID externally.  If none is
 * defined, we use the well-known value as our default.
 */
#ifdef            EFS_DIAG_USE_THIS_SUBSYS_ID
#define FS_SUBSYS EFS_DIAG_USE_THIS_SUBSYS_ID
#else
#define FS_SUBSYS DIAG_SUBSYS_FS
#endif

/* -------------------------------------------------------------------------
** Definitions
** ------------------------------------------------------------------------- */

typedef struct {
  uint32 targ_pkt_window;       /* Target window size in packets            */
  uint32 targ_byte_window;      /* Target window size in bytes              */
  uint32 host_pkt_window;       /* Host window size in packets              */
  uint32 host_byte_window;      /* Host window size in bytes                */
  uint32 iter_pkt_window;       /* Dir iteration window size in packets     */
  uint32 iter_byte_window;      /* Dir iteration window size in bytes       */
  int32  version;               /* Protocol version number                  */
  int32  min_version;           /* Minimum supported protocol version       */
  int32  max_version;           /* Maximum supported protocol version       */
  int32  feature_bits;          /* Bit mask of supported features           */
} fs_diag_params_type;

/* The maximum number of file descriptors open at any time
*/
#define FS_DIAG_MAX_FDS  6

/* The maximum number of directories open for reading at any time
*/
#define FS_DIAG_MAX_OPEN_DIRS  4

/* Lint wants parens around macro but compile will fail */
/*lint -save -e773 */
#define VOID_PACKED_PTR        PACKED void * PACKED_POST
#define DECLARE_DIAG_FN(fn)    VOID_PACKED_PTR fn (VOID_PACKED_PTR, uint16)
/*lint -restore */


DECLARE_DIAG_FN (fs_diag_hello_handler);
DECLARE_DIAG_FN (fs_diag_query_handler);
DECLARE_DIAG_FN (fs_diag_open_handler);
DECLARE_DIAG_FN (fs_diag_close_handler);
DECLARE_DIAG_FN (fs_diag_read_handler);
DECLARE_DIAG_FN (fs_diag_write_handler);
DECLARE_DIAG_FN (fs_diag_symlink_handler);
DECLARE_DIAG_FN (fs_diag_readlink_handler);
DECLARE_DIAG_FN (fs_diag_unlink_handler);
DECLARE_DIAG_FN (fs_diag_mkdir_handler);
DECLARE_DIAG_FN (fs_diag_rmdir_handler);
DECLARE_DIAG_FN (fs_diag_opendir_handler);
DECLARE_DIAG_FN (fs_diag_readdir_handler);
DECLARE_DIAG_FN (fs_diag_closedir_handler);
DECLARE_DIAG_FN (fs_diag_rename_handler);
DECLARE_DIAG_FN (fs_diag_stat_handler);
DECLARE_DIAG_FN (fs_diag_lstat_handler);
DECLARE_DIAG_FN (fs_diag_fstat_handler);
DECLARE_DIAG_FN (fs_diag_chmod_handler);
DECLARE_DIAG_FN (fs_diag_statfs_handler);
DECLARE_DIAG_FN (fs_diag_access_handler);
DECLARE_DIAG_FN (fs_diag_device_info_handler);
DECLARE_DIAG_FN (fs_diag_factimage_start_handler);
DECLARE_DIAG_FN (fs_diag_factimage_read_handler);
DECLARE_DIAG_FN (fs_diag_factimage_end_handler);
DECLARE_DIAG_FN (fs_diag_prep_factimage_handler);
DECLARE_DIAG_FN (fs_diag_put_deprecated_handler);
DECLARE_DIAG_FN (fs_diag_get_deprecated_handler);
DECLARE_DIAG_FN (fs_diag_put_handler);
DECLARE_DIAG_FN (fs_diag_get_handler);
DECLARE_DIAG_FN (fs_diag_extended_info_handler);
DECLARE_DIAG_FN (fs_diag_benchmark_start_test_handler);
DECLARE_DIAG_FN (fs_diag_benchmark_get_results_handler);
DECLARE_DIAG_FN (fs_diag_benchmark_init_handler);
DECLARE_DIAG_FN (fs_diag_chown_handler);
DECLARE_DIAG_FN (fs_diag_set_reservation_handler);
DECLARE_DIAG_FN (fs_diag_set_quota_handler);
DECLARE_DIAG_FN (fs_diag_get_group_info_handler);
DECLARE_DIAG_FN (fs_diag_deltree_handler);
DECLARE_DIAG_FN (fs_diag_truncate_handler);
DECLARE_DIAG_FN (fs_diag_ftruncate_handler);
DECLARE_DIAG_FN (fs_diag_statvfs_v2_handler);
DECLARE_DIAG_FN (fs_diag_md5sum_handler);
DECLARE_DIAG_FN (fs_diag_hotplug_format_handler);
DECLARE_DIAG_FN (fs_diag_shred_handler);
DECLARE_DIAG_FN (fs_diag_set_idle_dev_evt_dur_handler);
DECLARE_DIAG_FN (fs_diag_hotplug_device_info_handler);
DECLARE_DIAG_FN (fs_diag_sync_no_wait_handler);
DECLARE_DIAG_FN (fs_diag_sync_get_status_handler);
DECLARE_DIAG_FN (fs_diag_truncate64_handler);
DECLARE_DIAG_FN (fs_diag_ftruncate64_handler);
DECLARE_DIAG_FN (fs_diag_lseek64_handler);
DECLARE_DIAG_FN (fs_diag_make_golden_copy_handler);
DECLARE_DIAG_FN (fs_diag_filesystem_image_open);
DECLARE_DIAG_FN (fs_diag_filesystem_image_read);
DECLARE_DIAG_FN (fs_diag_filesystem_image_close);

/* -------------------------------------------------------------------------
** Local Data
** ------------------------------------------------------------------------- */
static fs_diag_params_type fs_diag_params;
static const fs_diag_params_type fs_diag_default_params = {
  FS_TARG_PKT_WINDOW_DEFAULT,
  FS_TARG_BYTE_WINDOW_DEFAULT,
  FS_HOST_PKT_WINDOW_DEFAULT,
  FS_HOST_BYTE_WINDOW_DEFAULT,
  FS_ITER_PKT_WINDOW_DEFAULT,
  FS_ITER_BYTE_WINDOW_DEFAULT,
  FS_DIAG_VERSION,
  FS_DIAG_MIN_VERSION,
  FS_DIAG_MAX_VERSION,
  FS_FEATURE_BITS
};

/* The descriptor used external to diag is merely an index into this array,
 * not a real EFS2 descriptor.  We only allow access by diag to files that
 * were opened through diag.  For each opened descriptor, there is a single
 * element here for this.  There will always be an EFS2 descriptor
 * associated with each open file, and that will sometimes be associated
 * with a buffered EFS_FILE pointer.  A -1 for the fd field indicates that
 * this entry is unused. */
typedef struct {
  int       fd;
  EFS_FILE *file;
} desc_entry;
static desc_entry fsd_desc_table[FS_DIAG_MAX_FDS];

/* Directory 'dirp' descriptors are entries into this table.  NULL entries
 * indicate a directory that is not open.  Because the protocol defines a
 * 'dirp' of 0 as an error condition, we access this table with an offset
 * of 1. */
typedef struct {
  EFSDIR        *dirp;
  int32         seqno;
  char          path[FS_PATH_MAX+1];
} dir_table_entry;
static dir_table_entry fsd_dir_table[FS_DIAG_MAX_OPEN_DIRS];

/*Temporary character buffer to store the normalized path*/
static char fs_diag_real_path[FS_PATH_MAX+1];


/* Handler declaration and diag table for legacy EFS1 packets. */
PACKED void *fs_diag_cmd (PACKED void *req_pkt, uint16 pkt_len);

static const diagpkt_user_table_entry_type fs_op_diag_tbl[] =
{
  {DIAG_FS_OP_F, DIAG_FS_OP_F, fs_diag_cmd}
};

/* The dispatch table for the EFS subsystem.
*/
static const diagpkt_user_table_entry_type fs_diag_tbl[] =
{
  {EFS2_DIAG_HELLO,    EFS2_DIAG_HELLO,    fs_diag_hello_handler},
  {EFS2_DIAG_QUERY,    EFS2_DIAG_QUERY,    fs_diag_query_handler},
  {EFS2_DIAG_OPEN,     EFS2_DIAG_OPEN,     fs_diag_open_handler},
  {EFS2_DIAG_CLOSE,    EFS2_DIAG_CLOSE,    fs_diag_close_handler},
  {EFS2_DIAG_READ,     EFS2_DIAG_READ,     fs_diag_read_handler},
  {EFS2_DIAG_WRITE,    EFS2_DIAG_WRITE,    fs_diag_write_handler},
  {EFS2_DIAG_SYMLINK,  EFS2_DIAG_SYMLINK,  fs_diag_symlink_handler},
  {EFS2_DIAG_READLINK, EFS2_DIAG_READLINK, fs_diag_readlink_handler},
  {EFS2_DIAG_UNLINK,   EFS2_DIAG_UNLINK,   fs_diag_unlink_handler},
  {EFS2_DIAG_MKDIR,    EFS2_DIAG_MKDIR,    fs_diag_mkdir_handler},
  {EFS2_DIAG_RMDIR,    EFS2_DIAG_RMDIR,    fs_diag_rmdir_handler},
  {EFS2_DIAG_OPENDIR,  EFS2_DIAG_OPENDIR,  fs_diag_opendir_handler},
  {EFS2_DIAG_READDIR,  EFS2_DIAG_READDIR,  fs_diag_readdir_handler},
  {EFS2_DIAG_CLOSEDIR, EFS2_DIAG_CLOSEDIR, fs_diag_closedir_handler},
  {EFS2_DIAG_RENAME,   EFS2_DIAG_RENAME,   fs_diag_rename_handler},
  {EFS2_DIAG_STAT,     EFS2_DIAG_STAT,     fs_diag_stat_handler},
  {EFS2_DIAG_LSTAT,    EFS2_DIAG_LSTAT,    fs_diag_lstat_handler},
  {EFS2_DIAG_FSTAT,    EFS2_DIAG_FSTAT,    fs_diag_fstat_handler},
  {EFS2_DIAG_CHMOD,    EFS2_DIAG_CHMOD,    fs_diag_chmod_handler},
  {EFS2_DIAG_STATFS,   EFS2_DIAG_STATFS,   fs_diag_statfs_handler},
  {EFS2_DIAG_ACCESS,   EFS2_DIAG_ACCESS,   fs_diag_access_handler},
  {EFS2_DIAG_DEV_INFO,
   EFS2_DIAG_DEV_INFO,
   fs_diag_device_info_handler},
#ifdef FEATURE_EFS_ENABLE_FACTORY_IMAGE_SECURITY_HOLE
  {EFS2_DIAG_FACT_IMAGE_START,
   EFS2_DIAG_FACT_IMAGE_START,
   fs_diag_factimage_start_handler},
  {EFS2_DIAG_FACT_IMAGE_READ,
   EFS2_DIAG_FACT_IMAGE_READ,
   fs_diag_factimage_read_handler},
  {EFS2_DIAG_FACT_IMAGE_END,
   EFS2_DIAG_FACT_IMAGE_END,
   fs_diag_factimage_end_handler},
  {EFS2_DIAG_PREP_FACT_IMAGE,
   EFS2_DIAG_PREP_FACT_IMAGE,
   fs_diag_prep_factimage_handler},
  {EFS2_DIAG_FILESYSTEM_IMAGE_OPEN,
   EFS2_DIAG_FILESYSTEM_IMAGE_OPEN,
   fs_diag_filesystem_image_open},
  {EFS2_DIAG_FILESYSTEM_IMAGE_READ,
   EFS2_DIAG_FILESYSTEM_IMAGE_READ,
   fs_diag_filesystem_image_read},
  {EFS2_DIAG_FILESYSTEM_IMAGE_CLOSE,
   EFS2_DIAG_FILESYSTEM_IMAGE_CLOSE,
   fs_diag_filesystem_image_close},
#endif /* FEATURE_EFS_ENABLE_FACTORY_IMAGE_SECURITY_HOLE */
  {EFS2_DIAG_PUT_DEPRECATED,
   EFS2_DIAG_PUT_DEPRECATED,
   fs_diag_put_deprecated_handler},
  {EFS2_DIAG_GET_DEPRECATED,
   EFS2_DIAG_GET_DEPRECATED,
   fs_diag_get_deprecated_handler},
  { EFS2_DIAG_EXTENDED_INFO,
    EFS2_DIAG_EXTENDED_INFO,
    fs_diag_extended_info_handler },
  {EFS2_DIAG_BENCHMARK_START_TEST,
   EFS2_DIAG_BENCHMARK_START_TEST,
   fs_diag_benchmark_start_test_handler},
  {EFS2_DIAG_BENCHMARK_GET_RESULTS,
   EFS2_DIAG_BENCHMARK_GET_RESULTS,
   fs_diag_benchmark_get_results_handler},
  {EFS2_DIAG_BENCHMARK_INIT,
   EFS2_DIAG_BENCHMARK_INIT,
   fs_diag_benchmark_init_handler},
  {EFS2_DIAG_CHOWN,
   EFS2_DIAG_CHOWN,
   fs_diag_chown_handler},
  {EFS2_DIAG_SET_RESERVATION,
   EFS2_DIAG_SET_RESERVATION,
   fs_diag_set_reservation_handler},
  {EFS2_DIAG_SET_QUOTA,
   EFS2_DIAG_SET_QUOTA,
   fs_diag_set_quota_handler},
  {EFS2_DIAG_GET_GROUP_INFO,
   EFS2_DIAG_GET_GROUP_INFO,
   fs_diag_get_group_info_handler},
  {EFS2_DIAG_DELTREE,
   EFS2_DIAG_DELTREE,
   fs_diag_deltree_handler},
  {EFS2_DIAG_PUT,
   EFS2_DIAG_PUT,
   fs_diag_put_handler},
  {EFS2_DIAG_GET,
   EFS2_DIAG_GET,
   fs_diag_get_handler},
  {EFS2_DIAG_TRUNCATE,
   EFS2_DIAG_TRUNCATE,
   fs_diag_truncate_handler},
  {EFS2_DIAG_FTRUNCATE,
   EFS2_DIAG_FTRUNCATE,
   fs_diag_ftruncate_handler},
  {EFS2_DIAG_STATVFS_V2,
   EFS2_DIAG_STATVFS_V2,
   fs_diag_statvfs_v2_handler},
  {EFS2_DIAG_MD5SUM,
   EFS2_DIAG_MD5SUM,
   fs_diag_md5sum_handler},
  {EFS2_DIAG_HOTPLUG_FORMAT,
   EFS2_DIAG_HOTPLUG_FORMAT,
   fs_diag_hotplug_format_handler},
  {EFS2_DIAG_SHRED,
   EFS2_DIAG_SHRED,
   fs_diag_shred_handler},
  {EFS2_DIAG_SET_IDLE_DEV_EVT_DUR,
   EFS2_DIAG_SET_IDLE_DEV_EVT_DUR,
   fs_diag_set_idle_dev_evt_dur_handler},
  {EFS2_DIAG_HOTPLUG_DEVICE_INFO,
   EFS2_DIAG_HOTPLUG_DEVICE_INFO,
   fs_diag_hotplug_device_info_handler},
  {EFS2_DIAG_SYNC_NO_WAIT,
   EFS2_DIAG_SYNC_NO_WAIT,
   fs_diag_sync_no_wait_handler},
  {EFS2_DIAG_SYNC_GET_STATUS,
   EFS2_DIAG_SYNC_GET_STATUS,
   fs_diag_sync_get_status_handler},
  {EFS2_DIAG_TRUNCATE64,
   EFS2_DIAG_TRUNCATE64,
   fs_diag_truncate64_handler},
  {EFS2_DIAG_FTRUNCATE64,
   EFS2_DIAG_FTRUNCATE64,
   fs_diag_ftruncate64_handler},
  {EFS2_DIAG_LSEEK64,
   EFS2_DIAG_LSEEK64,
   fs_diag_lseek64_handler},
  {EFS2_DIAG_MAKE_GOLDEN_COPY,
   EFS2_DIAG_MAKE_GOLDEN_COPY,
   fs_diag_make_golden_copy_handler},
};

/* The dispatch table can be registered before runtime if this file is
** being built as a C++ file.
*/
#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER (FS_SUBSYS, fs_diag_tbl);
#endif

/**********************************************************************
 * Packet memoization.
 *
 * The diag protocol is not robust, and therefore clients may retransmit
 * packets that were not acknowledged.  Unfortunately, this is not hidden
 * from us as it would be in a reasonable protocol.  Since many of the diag
 * EFS2 requests are not idempotent, it is necessary to detect these
 * retransmissions and return the same response, without redoing the work
 * itself.
 **********************************************************************/

/* The size of the largest possible request packet.  */
#ifdef  DIAG_MAX_RX_PKT_SIZ
  #define LARGEST_DIAG_PACKET     DIAG_MAX_RX_PKT_SIZ
#else
  #define LARGEST_DIAG_PACKET     0x800
#endif

#ifdef DIAG_MAX_TX_PKT_SIZ
  #define LARGEST_DIAG_RESPONSE   DIAG_MAX_TX_PKT_SIZ
#else
  #define LARGEST_DIAG_RESPONSE   0x800
#endif

static char fsd_last_packet[LARGEST_DIAG_PACKET];
static unsigned fsd_last_packet_len;
static char fsd_last_response[LARGEST_DIAG_RESPONSE];
static unsigned fsd_last_response_len;

/* Memoize this packet and response. */
static void fsd_memoize_packet
(PACKED const void * PACKED_POST request, unsigned req_len,
    PACKED const void *  PACKED_POST response, unsigned rsp_len);

/* Invalidate the last memoized packet.  Used for requests that are
 * idempotent to prevent memoization. */
static void fsd_invalidate_memoization (void);

/* Check if this request is the same as a previous one.  Will allocate a
 * response and return a copy of that last response.  Note that this macro
 * has a 'return' in it that will return a result if no work should be
 * done. */
#define FSD_MEMOIZED_PACKET_CHECK(pkt, len, code) \
  do { \
    if ((len) == fsd_last_packet_len && \
        memcmp ((void *)(pkt), fsd_last_packet, fsd_last_packet_len) == 0) \
    { \
      PACKED void *  PACKED_POST __rsp = diagpkt_subsys_alloc (FS_SUBSYS, \
          (code), fsd_last_response_len); \
      if (__rsp != NULL) \
        fs_memscpy ((void *) __rsp, fsd_last_response_len, fsd_last_response, \
                     fsd_last_response_len); \
      return __rsp; \
    } \
  } while (0)

static void
fsd_memoize_packet (PACKED const void * PACKED_POST request, unsigned req_len,
    PACKED const void * PACKED_POST response, unsigned rsp_len)
{
  /* If we somehow get too large of a packet or response, don't remember
   * it. */
  if (req_len > LARGEST_DIAG_PACKET ||
      rsp_len > LARGEST_DIAG_RESPONSE)
  {
    fsd_invalidate_memoization ();
  } else {
    fs_memscpy (fsd_last_packet, LARGEST_DIAG_PACKET, (void *) request,
                req_len);
    fsd_last_packet_len = req_len;

    fs_memscpy (fsd_last_response, LARGEST_DIAG_RESPONSE, (void *) response,
                rsp_len);
    fsd_last_response_len = rsp_len;
  }
}

/* Invalidate the last memoized packet.  Used for requests that are
 * idempotent to prevent memoization. */
static void
fsd_invalidate_memoization (void)
{
  fsd_last_packet_len = 0;
  fsd_last_response_len = 0;
}

/**********************************************************************
 * Packet pathname validation.
 *********************************************************************/

/* Count the number of nul characters in the given string. */
static unsigned
fsd_count_nulls (PACKED char *  PACKED_POST name, unsigned length)
{
  unsigned i;
  unsigned result = 0;

  for (i = 0; i < length; i++) {
    if (name[i] == '\0')
      result++;
  }

  return result;
}

/* Convenience macro for checking pathnames.  Takes its argument, and
 * verifies that it contains at least as many nulls.  Arguments are:
 *   request - The typed request packet.
 *   type    - The type of the request packet.
 *   field   - The name of the field containing the pathname
 *   req_len - The total length of the packet
 *   min_nul - Miminum number of nul characters in the string.
 * This macro contains a return, and will return a diag error packet
 * indicating a bad parameter if the request doesn't contain sufficient
 * nulls. */
#define FSD_CHECK_NULLS(_req, _type, _field, _req_len, _min_nul) \
  do { \
    if (fsd_count_nulls ((_req)->_field, \
          (_req_len) - offsetof (_type, _field)) < (_min_nul)) \
    { \
      return diagpkt_err_rsp (DIAG_BAD_PARM_F, \
          (void *) (_req), (_req_len)); \
    } \
  } while (0)


/**********************************************************************
 * Packet length validation.
 *********************************************************************/

static int
fs_diag_is_valid_pkt_len (uint32 pkt_len, uint32 min_pkt_len)
{
  /* Validate pkt_len range */
  if (pkt_len > LARGEST_DIAG_PACKET || pkt_len < min_pkt_len)
  {
    return 0;
  }
  return 1;
}

/**********************************************************************
 * Close all open handles.  Used to reset everything when a new connection
 * is established.
 *********************************************************************/

/* Close a single slot.  Returns zero on success or -1 on error (the result
 * of either efs_close or efs_fclose). */
static int
fsd_close_slot (unsigned slot)
{
  int result;

  ASSERT (slot < FS_DIAG_MAX_FDS);
  ASSERT (fsd_desc_table[slot].fd >= 0);

  if (fsd_desc_table[slot].file != NULL) {
    result = efs_fclose (fsd_desc_table[slot].file);
    /* efs_fclose also calls efs_close, so that isn't needed. */
  } else {
    result = efs_close (fsd_desc_table[slot].fd);
  }

  fsd_desc_table[slot].file = NULL;
  fsd_desc_table[slot].fd = -1;

  return result;
}

static void
fsd_close_open_handles (void)
{
  unsigned slot;

  for (slot = 0; slot < FS_DIAG_MAX_FDS; slot++) {
    if (fsd_desc_table[slot].fd >= 0)
      (void) fsd_close_slot (slot);
  }
}

static void
fsd_close_open_dirs (void)
{
  unsigned slot;

  for (slot = 0; slot < FS_DIAG_MAX_OPEN_DIRS; slot++) {
    if (fsd_dir_table[slot].dirp != NULL) {
      (void) efs_closedir (fsd_dir_table[slot].dirp);
      fsd_dir_table[slot].dirp = NULL;
    }
  }
}

/**********************************************************************
 * Convert the error code given in the target to the one published in
 * efs diag documentation
 *********************************************************************/
uint32
fs_get_diag_errno (uint32 posix_errno)
{
  uint32 diag_errno;

  switch (posix_errno)
  {
    case EPERM:
      diag_errno = 1;
      break;
    case ENOENT:
      diag_errno = 2;
      break;
    case EEXIST:
      diag_errno = 6;
      break;
    case EBADF:
      diag_errno = 9;
      break;
    case ENOMEM:
      diag_errno = 12;
      break;
    case EACCES:
      diag_errno = 13;
      break;
    case EBUSY:
      diag_errno = 16;
      break;
    case EXDEV:
      diag_errno = 18;
      break;
    case ENODEV:
      diag_errno = 19;
      break;
    case ENOTDIR:
      diag_errno = 20;
      break;
    case EISDIR:
      diag_errno = 21;
      break;
    case EINVAL:
      diag_errno = 22;
      break;
    case EMFILE:
      diag_errno = 24;
      break;
    case ETXTBSY:
      diag_errno = 26;
      break;
    case ENOSPC:
      diag_errno = 28;
      break;
    case ESPIPE:
      diag_errno = 29;
      break;
    case FS_ERANGE:
      diag_errno = 34;
      break;
    case ENAMETOOLONG:
      diag_errno = 36;
      break;
    case ENOTEMPTY:
      diag_errno = 39;
      break;
    case ELOOP:
      diag_errno = 40;
      break;
    case ETIMEDOUT:
      diag_errno = 110;
      break;
    case ESTALE:
      diag_errno = 116;
      break;
    case EDQUOT:
      diag_errno = 122;
      break;
    case ENOCARD:
      diag_errno = 301;
      break;
    case EBADFMT:
      diag_errno = 302;
      break;
    case ENOTITM:
      diag_errno = 303;
      break;
    case EROLLBACK:
      diag_errno = 304;
      break;
    case ENOTHINGTOSYNC:
      diag_errno = 306;
      break;
    case EEOF:
      diag_errno = 0x8000;
      break;
    case EUNKNOWN_SFAT:
      diag_errno = 0x8001;
      break;
    case EUNKNOWN_HFAT:
      diag_errno = 0x8002;
      break;
    default:
      /* Do Nothing if we dont need to convert */
      diag_errno = posix_errno;
      break;
  }

  return diag_errno;
}

/*===========================================================================

FUNCTION FS_DIAG_INIT
DESCRIPTION
  Initialization function for this module. Registers the packet handlers
  and sets up local data structures.
============================================================================*/
void fs_diag_init (void)
{
  unsigned i;

  fs_diag_params = fs_diag_default_params;

  for (i = 0; i < FS_DIAG_MAX_FDS; i++) {
    fsd_desc_table[i].fd = -1;
    fsd_desc_table[i].file = NULL;
  }

  for (i = 0; i < FS_DIAG_MAX_OPEN_DIRS; i++) {
    fsd_dir_table[i].dirp = NULL;
    fsd_dir_table[i].seqno = 0;
  }

  /* --- Register our EFS Diag Subsystem handler. --- */
#ifndef __cplusplus

  /* Register legacy packets */
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, fs_op_diag_tbl);

  /* Register EFS2 diag packets. */
  DIAGPKT_DISPATCH_TABLE_REGISTER (FS_SUBSYS, fs_diag_tbl);

#endif /* ! __cplusplus */

  return;
} /* END fs_diag_init */


/*===========================================================================

FUNCTION FS_DIAG_HELLO_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_HELLO packet. Initializes the EFS2 Diag interface
  and passes the values of some parameters back to the tool.
============================================================================*/
VOID_PACKED_PTR
fs_diag_hello_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_hello_req_type *request;
  efs2_diag_hello_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_hello_rsp_type);

  EFS_BAB_DEBUG_PRINTF ("_HELLO");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_hello_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_hello_req_type *) req_ptr;

  response = (efs2_diag_hello_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_HELLO,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Initialize this module: close any open handles. */
  fsd_close_open_handles ();
  fsd_close_open_dirs ();

  /*------------------------------------------------------
    Fill in the fields in the response packet
    ------------------------------------------------------*/
  response->targ_pkt_window  = fs_diag_params.targ_pkt_window;
  response->targ_byte_window = fs_diag_params.targ_byte_window;
  response->host_pkt_window  = fs_diag_params.host_pkt_window;
  response->host_byte_window = fs_diag_params.host_byte_window;
  response->iter_pkt_window  = fs_diag_params.iter_pkt_window;
  response->iter_byte_window = fs_diag_params.iter_byte_window;
  response->version          = fs_diag_params.version;
  response->min_version      = fs_diag_params.min_version;
  response->max_version      = fs_diag_params.max_version;
  response->feature_bits     = fs_diag_params.feature_bits;

  /* Update relevant parameters using values specified in the request
   * packet.  _EVERY_ single one of these fields is completely meaningless.
   * Why are we doing any of this?
  */
  if (fs_diag_params.targ_pkt_window > request->targ_pkt_window)
    fs_diag_params.targ_pkt_window = request->targ_pkt_window;
  if (fs_diag_params.targ_byte_window > request->targ_byte_window)
    fs_diag_params.targ_byte_window = request->targ_byte_window;
  if (fs_diag_params.host_pkt_window > request->host_pkt_window)
    fs_diag_params.host_pkt_window = request->host_pkt_window;
  if (fs_diag_params.host_byte_window > request->host_byte_window)
    fs_diag_params.host_byte_window = request->host_byte_window;
  if (fs_diag_params.iter_pkt_window > request->iter_pkt_window)
    fs_diag_params.iter_pkt_window = request->iter_pkt_window;
  if (fs_diag_params.iter_byte_window > request->iter_byte_window)
    fs_diag_params.iter_byte_window = request->iter_byte_window;
  fs_diag_params.version       = request->version;
  fs_diag_params.min_version   = request->min_version;
  fs_diag_params.max_version   = request->max_version;
  fs_diag_params.feature_bits &= request->feature_bits;

  return response;

} /* fs_diag_hello_handler */


/*===========================================================================

FUNCTION FS_DIAG_QUERY_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_QUERY packet. Passes back information about some
  ESF2 parameters.
============================================================================*/
VOID_PACKED_PTR
fs_diag_query_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_query_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_query_rsp_type);

  EFS_BAB_DEBUG_PRINTF ("_QUERY");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_query_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (efs2_diag_query_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_QUERY,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /*------------------------------------------------------
    Fill in the fields in the response packet
    ------------------------------------------------------*/
  response->max_name         = FS_NAME_MAX;
  response->max_path         = FS_PATH_MAX;
  response->max_link_depth   = 0;
  response->max_file_size    = 0;
  response->max_dir_entries  = FS_DIR_ENTRY_SIZE;
  response->max_mounts       = FS_MAX_MOUNTS;

  return response;

} /* fs_diag_query_handler */


/*===========================================================================

FUNCTION FS_DIAG_OPEN_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_OPEN packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_open_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_open_req_type *request;
  efs2_diag_open_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_open_rsp_type);
  const char *request_path;
  int32  fd;
  unsigned slot;

#ifndef FEATURE_NO_STDIO_FROM_DIAG
  char *buff_mode = "";
#endif

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len);
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_open_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_OPEN);

  request = (efs2_diag_open_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_open_req_type, path, pkt_len, 1);

  EFS_BAB_DEBUG_PRINTF ("_OPEN: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (OPEN, request_path);

  /* Old versions of fs_diag would perform Access Validation for each
   * and every Read/Write request, but this was incurring a
   * significant performance penalty.  Later (Oct 2005), these checks
   * were removed entirely from Read/Write packet handlers, which
   * violated some assumptions made by BREW SPAR.  Now we do one test
   * of READ and one test of WRITE during open to confirm that the
   * file mode is acceptable.  If any of these read/write permissions
   * are denied, then the file can not be opened. */
  switch (request->oflag & O_ACCMODE) {
    case O_RDONLY:
      FS_DIAG_VALIDATE_PATH_ACCESS (READ, request_path);
      break;

    case O_WRONLY:
      FS_DIAG_VALIDATE_PATH_ACCESS (WRITE, request_path);
      break;

    case O_RDWR:
    default:
      FS_DIAG_VALIDATE_PATH_ACCESS (READ, request_path);
      FS_DIAG_VALIDATE_PATH_ACCESS (WRITE, request_path);
      break;
  }

  response = (efs2_diag_open_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_OPEN,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Find a descriptor slot to use. */
  for (slot = 0; slot < FS_DIAG_MAX_FDS; slot++) {
    if (fsd_desc_table[slot].fd == -1)
      break;
  }

  /* If we are out of diag descriptors, don't try opening, just return the
   * error. */
  if (slot >= FS_DIAG_MAX_FDS) {
    response->fd = -1;
    response->diag_errno = FS_DIAG_TOO_MANY_OPEN_FILES;
    goto open_end;
  }

  /* Open the handle. */
  fd = efs_open (request_path, request->oflag, request->mode);

  /* Check for open failure. */
  if (fd < 0) {
    response->fd = -1;
    response->diag_errno = fs_get_diag_errno (efs_errno);
    goto open_end;
  }

  /* Update the desc slot. */
  fsd_desc_table[slot].fd = fd;
  fsd_desc_table[slot].file = NULL;

  response->fd = slot;
  response->diag_errno = 0;


#ifndef FEATURE_NO_STDIO_FROM_DIAG
  /* Open the stdio handle to use for reading and writing.  Determining the
   * mode from the oflags. */
  switch (request->mode & O_ACCMODE) {
    case O_RDONLY:
      buff_mode = "r";
      break;
    case O_WRONLY:
      buff_mode = "w";
      break;
    case O_RDWR:
    default:
      buff_mode = "r+";
      break;
  }

  fsd_desc_table[slot].file = efs_fdopen (fd, buff_mode);
  /* If we didn't get a handle, the file pointer in the slot will be NULL,
   * and the unbuffered operations will be used.  TODO: log a warning about
   * this? */
#endif /* FEATURE_NO_STDIO_FROM_DIAG */

  open_end:
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;

} /* END fs_diag_open_handler */


/*===========================================================================

FUNCTION FS_DIAG_CLOSE_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_CLOSE packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_close_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_close_req_type *request;
  efs2_diag_close_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_close_rsp_type);
  int result;

  EFS_BAB_DEBUG_PRINTF ("_CLOSE");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for exact length of packet */
  if (pkt_len != sizeof (efs2_diag_close_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_CLOSE);

  request = (efs2_diag_close_req_type *) req_ptr;

  response = (efs2_diag_close_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_CLOSE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->diag_errno = 0;

  /* If this handle is not open, or is out of range, return an error. */
  if (request->fd < 0 || request->fd >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fd].fd < 0)
  {
    response->diag_errno = fs_get_diag_errno (EBADF);
    goto close_done;
  }

  result = fsd_close_slot (request->fd);

  if (result != 0)
    response->diag_errno = fs_get_diag_errno (efs_errno);

  close_done:
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_close_handler */


/*===========================================================================

FUNCTION FS_DIAG_READ_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_READ packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_read_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_read_req_type *request;
  efs2_diag_read_rsp_type *response;
  desc_entry *slot;
  unsigned base_length = offsetof (efs2_diag_read_rsp_type, data);
  unsigned rsp_len = base_length;
  fs_ssize_t bytes_left;
  fs_off64_t   offset;
  fs_ssize_t count;

  EFS_BAB_DEBUG_PRINTF ("_READ");
  fsd_invalidate_memoization ();

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_read_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_read_req_type *) req_ptr;

  /* Validate the requested descriptor. */
  if (request->fd < 0 || request->fd >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fd].fd < 0)
  {
    response = (efs2_diag_read_rsp_type *)
      diagpkt_subsys_alloc (FS_SUBSYS,
          EFS2_DIAG_READ, base_length);
    if (response == NULL)
      return NULL;

    response->fd = request->fd;
    response->offset = request->fd;
    response->bytes_read = -1;
    response->diag_errno = fs_get_diag_errno (EBADF);
    goto read_end;
  }

  slot = &fsd_desc_table[request->fd];

  /* Begin by allocating a full-sized read packet.  We will shorten it if
   * we get an error, or a short read. */
  bytes_left = request->nbyte;
  if (bytes_left > FS_DIAG_MAX_READ_REQ)
    bytes_left = FS_DIAG_MAX_READ_REQ;

  rsp_len += bytes_left;
  response = (efs2_diag_read_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS, EFS2_DIAG_READ, rsp_len);
  if (response == NULL)
    return NULL;

  response->fd = request->fd;
  response->offset = request->offset;

  /* Seek to the specified location, and attempt the read. */
  if (slot->file == NULL) {
    /* Unbuffered. */
    offset = efs_lseek64 (slot->fd, (fs_off64_t) request->offset, SEEK_SET);
    if (offset != (fs_off64_t) request->offset) {
      response->bytes_read = -1;
      response->diag_errno = fs_get_diag_errno (efs_errno);
      diagpkt_shorten (response, base_length);
      goto read_end;
    }

    count = efs_read (slot->fd, (void *) response->data, bytes_left);
  } else {
    if (0 != efs_fseek (slot->file, request->offset, SEEK_SET)) {
      response->bytes_read = -1;
      response->diag_errno = fs_get_diag_errno (efs_errno);
      diagpkt_shorten (response, base_length);
      goto read_end;
    }

    count = efs_fread ((void *) response->data, 1, bytes_left, slot->file);
  }

  /* Fixup the response packet, depending on the result. */
  if (count < 0) {
    response->bytes_read = -1;
    response->diag_errno = fs_get_diag_errno (efs_errno);
  } else {
    response->bytes_read = count;
    response->diag_errno = 0;
    if (count < bytes_left)
      diagpkt_shorten (response, base_length + count);
  }

  read_end:
  return response;
} /* END fs_diag_read_handler */


/*===========================================================================

FUNCTION FS_DIAG_WRITE_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_WRITE packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_write_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_write_req_type *request;
  efs2_diag_write_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_write_rsp_type);
  fs_off64_t   offset;
  fs_ssize_t nbyte;
  fs_ssize_t bytes_written = 0;
  fs_ssize_t bytes_left;
  char      *write_locn;
  desc_entry *slot;

  EFS_BAB_DEBUG_PRINTF ("_WRITE");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_write_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_write_req_type *) req_ptr;

  response = (efs2_diag_write_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_WRITE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->fd     = request->fd;
  response->offset = request->offset;

  nbyte = (fs_ssize_t)
    (pkt_len - (uint16) offsetof (efs2_diag_write_req_type, data));

  if (request->fd < 0 || request->fd >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fd].fd < 0)
  {
    response->diag_errno = fs_get_diag_errno (EBADF);
    response->bytes_written = 0;
    goto write_end;
  }

  slot = &fsd_desc_table[request->fd];

  /* Seek to the specified offset and write the specified number of bytes. */

  if (slot->file == NULL) {
    /* Unbuffered */
    offset = efs_lseek64 (slot->fd, (fs_off64_t) request->offset, SEEK_SET);
    if (offset != (fs_off64_t) request->offset) {
      response->bytes_written = -1;
      response->diag_errno = fs_get_diag_errno (efs_errno);
      goto write_end;
    }
  } else {
    /* Buffered */
    if (0 != efs_fseek (slot->file, request->offset,
                        SEEK_SET)) {
      response->bytes_written = -1;
      response->diag_errno = fs_get_diag_errno (efs_errno);
      goto write_end;
    }
  }

  offset = (fs_off64_t) request->offset;
  bytes_left   = nbyte;
  write_locn   = (char *) request->data;

  while (bytes_left > 0) {
    if (slot->file == NULL) {
      bytes_written = efs_write (slot->fd, (void *) write_locn, bytes_left);
    } else {
      bytes_written = efs_fwrite ((void *) write_locn, 1, bytes_left,
          slot->file);
    }

    if (bytes_written <= 0) {
      /* XXX: This is an incorrect response, but I suspect at this point
       * there are external tools that depend on this behavior. */
      response->bytes_written = -1;
      response->diag_errno = fs_get_diag_errno (efs_errno);
      goto write_end;
    }

    bytes_left -= bytes_written;
    write_locn += bytes_written;
  }

  /* (bytes_written >= 0) when it gets here */
  response->bytes_written = nbyte;
  response->diag_errno         = 0;

  write_end:
  return response;
} /* END fs_diag_write_handler */


/*===========================================================================

FUNCTION FS_DIAG_SYMLINK_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_SYMLINK packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_symlink_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_symlink_req_type *request;
  efs2_diag_symlink_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_symlink_rsp_type);
  const char *oldpath;
  const char *newpath;
  int32  oldpath_len;
  int32  symlink_result;

  EFS_BAB_DEBUG_PRINTF ("_SYMLINK");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_symlink_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_SYMLINK);

  request = (efs2_diag_symlink_req_type *) req_ptr;

  FSD_CHECK_NULLS (request, efs2_diag_symlink_req_type,
      oldpath, pkt_len, 2);

  /* Do a sanity check on the input parameters.
   */
  oldpath = (const char *) request->oldpath;
  oldpath_len = (int32) strlen (oldpath);
  newpath = (const char *) (request->oldpath + oldpath_len + 1);
  if (oldpath_len + strlen (newpath) + 2 > pkt_len)
  {
    return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

  FS_DIAG_VALIDATE_PATH_ACCESS (SYMLINK, oldpath);
  FS_DIAG_VALIDATE_PATH_ACCESS (SYMLINK, newpath);

  response = (efs2_diag_symlink_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SYMLINK,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Try to create the symbolic link. Do not report an error if the
  ** creation fails because the link already exists.
  */
  symlink_result = efs_symlink (oldpath, newpath);

  if ((symlink_result == 0) || (efs_errno == EEXIST))
    response->diag_errno = 0;
  else
    response->diag_errno = fs_get_diag_errno (efs_errno);

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_symlink_handler */


/*===========================================================================

FUNCTION FS_DIAG_READLINK_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_READLINK packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_readlink_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_readlink_req_type *request;
  efs2_diag_readlink_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_readlink_rsp_type);
  char   buf[FS_PATH_MAX];
  int32  readlink_result;
  const char *request_path;

  buf[0] = 0;                   /* Praise lint */
  EFS_BAB_DEBUG_PRINTF ("_READLINK");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_readlink_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_readlink_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_readlink_req_type,
      path, pkt_len, 1);



  FS_DIAG_VALIDATE_PATH_ACCESS (READLINK, request_path);

  readlink_result = efs_readlink (request_path, buf, FS_PATH_MAX);

  if (readlink_result > 0)
  {
    rsp_len += readlink_result;
  }

  response = (efs2_diag_readlink_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_READLINK,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (readlink_result >= 0)
  {
    response->diag_errno = 0;
    fs_memscpy ((char *)response->name, readlink_result, buf, readlink_result);
    response->name[readlink_result] = '\0';
  }
  else
  {
    response->diag_errno = fs_get_diag_errno (efs_errno);
    response->name[0] = '\0';
  }

  return response;
} /* END fs_diag_readlink_handler */


/*===========================================================================

FUNCTION FS_DIAG_UNLINK_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_UNLINK packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_unlink_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_unlink_req_type *request;
  efs2_diag_unlink_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_unlink_rsp_type);
  int32  unlink_result;
  const char *request_path;

  EFS_BAB_DEBUG_PRINTF ("_UNLINK");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_unlink_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_UNLINK);

  request = (efs2_diag_unlink_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_unlink_req_type,
      path, pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (UNLINK, request_path);

  response = (efs2_diag_unlink_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_UNLINK,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Try to unlink the file. Do not report an error if the operation fails
  ** because the file or link does not exist.
  */
  unlink_result = efs_unlink (request_path);

  if ((unlink_result == 0) || (efs_errno == ENOENT))
    response->diag_errno = 0;
  else
    response->diag_errno = fs_get_diag_errno (efs_errno);

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_unlink_handler */


/*===========================================================================

FUNCTION FS_DIAG_MKDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_MKDIR packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_mkdir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_mkdir_req_type *request;
  efs2_diag_mkdir_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_mkdir_rsp_type);
  int32  mkdir_result;
  int dirname_len;
  int i;
  char *request_path;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_mkdir_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_MKDIR);

  request = (efs2_diag_mkdir_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  FSD_CHECK_NULLS (request, efs2_diag_mkdir_req_type,
      path, pkt_len, 1);

  EFS_BAB_DEBUG_PRINTF ("_MKDIR \"%s\"\n\r", request_path);

  /* EFS2 does not ignore trailing slashes at the end of directory names, but
   * tools expect this behavior. So remove trailing slashes here.
   */
  dirname_len = strlen (request_path);
  for (i = dirname_len-1; i > 0 && request_path[i] == '/'; i--)
    request_path[i] = '\0';

  FS_DIAG_VALIDATE_PATH_ACCESS (MKDIR, request_path);

  response = (efs2_diag_mkdir_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_MKDIR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Remove the directory and then create it. The remove will do nothing if
  ** the directory doesn't exist, but it won't hurt. If the directory exists
  ** and is not empty, the remove will fail and so will the mkdir call that
  ** follows.
  */


  (void) efs_rmdir (request_path);
  mkdir_result = efs_mkdir (request_path, request->mode);
  if (mkdir_result == -1)
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_mkdir_handler */


/*===========================================================================

FUNCTION FS_DIAG_RMDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_RMDIR packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_rmdir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_rmdir_req_type *request;
  efs2_diag_rmdir_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_rmdir_rsp_type);
  int32  rmdir_result;
  int i;
  int dirname_len;
  char *request_path;

  EFS_BAB_DEBUG_PRINTF ("_RMDIR");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_rmdir_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_RMDIR);

  request = (efs2_diag_rmdir_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  FSD_CHECK_NULLS (request, efs2_diag_rmdir_req_type,
      path, pkt_len, 1);

  /* EFS2 does not ignore trailing slashes at the end of directory names, but
   * tools expect this behavior. So remove trailing slashes here.
   */
  dirname_len = strlen (request_path);
  for (i = dirname_len-1; i > 0 && request_path[i] == '/'; i--)
    request_path[i] = '\0';

  FS_DIAG_VALIDATE_PATH_ACCESS (RMDIR, request_path);

  response = (efs2_diag_rmdir_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_RMDIR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Try to remove the directory. If the directory does not exist, do not
  ** report an error.
  */
  rmdir_result = efs_rmdir (request_path);
  if ((rmdir_result == 0) || (efs_errno == ENOENT))
    response->diag_errno = 0;
  else
    response->diag_errno = fs_get_diag_errno (efs_errno);

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_rmdir_handler */


/*===========================================================================

FUNCTION FS_DIAG_OPENDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_OPENDIR packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_opendir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_opendir_req_type *request;
  efs2_diag_opendir_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_opendir_rsp_type);
  EFSDIR *dirp;
  unsigned slot;
  const char *request_path;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_opendir_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_OPENDIR);

  request = (efs2_diag_opendir_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_opendir_req_type,
      path, pkt_len, 1);

  EFS_BAB_DEBUG_PRINTF ("_OPENDIR %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (OPENDIR, request_path);

  response = (efs2_diag_opendir_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_OPENDIR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Find a descriptor slot to use. */
  for (slot = 0; slot < FS_DIAG_MAX_OPEN_DIRS; slot++) {
    if (fsd_dir_table[slot].dirp == NULL)
      break;
  }

  /* If there are too many open directories, return an error. */
  if (slot >= FS_DIAG_MAX_OPEN_DIRS) {
    response->dirp = 0;
    response->diag_errno = FS_DIAG_TOO_MANY_OPEN_DIRS;
    goto opendir_end;
  }

  /* Check that the name length doesn't exceed the FS_PATH_MAX, since we're
   * about to copy it.
   * TODO: Check for a proper NULL termination of name. */
  if (strlen (request_path) > FS_PATH_MAX) {
    response->dirp = 0;
    response->diag_errno = fs_get_diag_errno (ENAMETOOLONG);
    goto opendir_end;
  }

  dirp = efs_opendir (request_path);

  if (dirp == 0)
  {
    response->dirp  = 0;
    response->diag_errno = fs_get_diag_errno (efs_errno);
    goto opendir_end;
  }

  fsd_dir_table[slot].dirp = dirp;

  /* The protocol expects the first directory entry to have a 'seqno' of
   * '1'.  This would give ".." seqno 0, and "." seqno -1.  The
   * fsd_dir_table 'seqno' field indicates the sequence number of the last
   * entry read.  Therefore, we set the last seqno to '-2', since that is
   * the logical entry before ".". */
  fsd_dir_table[slot].seqno = -2;
  strlcpy (fsd_dir_table[slot].path, request_path,
      sizeof (fsd_dir_table[slot].path));

  response->dirp = slot + 1;
  response->diag_errno = 0;

  opendir_end:
  EFS_BAB_DEBUG_PRINTF (" >>>OPENDIR: %d\n\r", response->diag_errno);
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_opendir_handler */

/*===========================================================================
 * Allocate a response packet for readdir, with the specified filename
 * length and error code.  'dirp' and 'seqno' will be filled in from the
 * request packet.
 *
 * NOTE:The error passed should be the POSIX or extended errors
 * only this function will perform the mapping before sending the response.
 *
 */
static efs2_diag_readdir_rsp_type *
fsd_readdir_alloc (efs2_diag_readdir_req_type *request,
    int the_errno,
    const char *entry_name)
{
  efs2_diag_readdir_rsp_type *response;
  unsigned name_length = strlen (entry_name);

  response = (efs2_diag_readdir_rsp_type *) diagpkt_subsys_alloc
    (FS_SUBSYS, EFS2_DIAG_READDIR,
     offsetof (efs2_diag_readdir_rsp_type, entry_name) + name_length + 1);
  if (response != NULL)
  {
    response->dirp = request->dirp;
    response->seqno = request->seqno;
    response->diag_errno = fs_get_diag_errno (the_errno);
    response->entry_type = 0;
    response->mode = 0;
    response->size = 0;
    response->atime = 0;
    response->mtime = 0;
    response->ctime = 0;
    strlcpy ((char *)response->entry_name, entry_name, name_length + 1);
  }

  return response;
}

/*===========================================================================

FUNCTION FS_DIAG_READDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_READDIR packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_readdir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_readdir_req_type *request;
  efs2_diag_readdir_rsp_type *response;
  dir_table_entry *slot;
  struct fs_dirent *dirent = NULL;
  struct fs_stat sbuf;
  static char path_buffer[FS_PATH_MAX + 1];
  int result;

  EFS_BAB_DEBUG_PRINTF ("_READDIR");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_readdir_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_readdir_req_type *) req_ptr;

  /* Validate the passed in dirp. */
  if (request->dirp < 1 || request->dirp > FS_DIAG_MAX_OPEN_DIRS ||
      fsd_dir_table[request->dirp - 1].dirp == NULL)
  {
    response = fsd_readdir_alloc (request, FS_DIAG_DIR_NOT_OPEN, "");
    goto readdir_end;
  }
  slot = &fsd_dir_table[request->dirp - 1];

  /* Validate the seqno. */
  if (request->seqno <= 0) {
    response = fsd_readdir_alloc (request, FS_DIAG_INVALID_SEQ_NO, "");
    goto readdir_end;
  }

  /* If this request is earlier than previous requests, close and reopen,
   * to reset the sequence number. */
  if (request->seqno <= slot->seqno) {
    (void) efs_closedir (slot->dirp);
    slot->dirp = efs_opendir (slot->path);

    /* If the directory vanish in the interim, return as if the dir failed
     * to open. */
    if (slot->dirp == NULL) {
      FS_MSG_MED_0 ("fs_diag_readdir: dir handle failed to reopen");
      response = fsd_readdir_alloc (request, FS_DIAG_DIR_NOT_OPEN, "");
      goto readdir_end;
    }

    /* Start at the ".." entry. */
    slot->seqno = -2;
  }

  /* Advance to the entry we are trying to read. */
  while (request->seqno > slot->seqno) {
    dirent = efs_readdir (slot->dirp);
    if (dirent == NULL) {
      /* Past end of directory.  Could also indicate an error, but the API
       * doesn't distinguish that. */
      response = fsd_readdir_alloc (request, 0, "");
      goto readdir_end;
    }
    slot->seqno++;
  }

  /* Check for logic error above. */
  ASSERT (dirent != NULL);

  /* Construct a full path to lstat the entry with. */
  if (strlen (slot->path) + strlen (dirent->d_name) + 1 > FS_PATH_MAX) {
    response = fsd_readdir_alloc (request, FS_DIAG_PATH_TOO_LONG, "");
    goto readdir_end;
  }

  strlcpy (path_buffer, slot->path, sizeof (path_buffer));
  strlcat (path_buffer, "/", sizeof (path_buffer));
  strlcat (path_buffer, dirent->d_name, sizeof (path_buffer));

  /* We have our filename, so we can allocate a response, no matter what it
   * will be. */
  response = fsd_readdir_alloc (request, 0, dirent->d_name);
  if (response == NULL)
    return NULL;
  result = efs_lstat (path_buffer, &sbuf);
  if (result != 0) {
    /* We were unable to stat the entry, so we know very little about it.
     * Fill in what we can. */
    response->entry_type = FS_DIAG_FTYPE_UNKNOWN;
  } else {
    /* Copy all of the stat information that we can. */
    response->mode = sbuf.st_mode;
    response->size = sbuf.st_size;
    response->atime = sbuf.st_atime;
    response->mtime = sbuf.st_mtime;
    response->ctime = sbuf.st_ctime;
    switch (sbuf.st_mode & S_IFMT) {
      case S_IFREG:
        response->entry_type = FS_DIAG_FTYPE_REG;
        break;

      case S_IFDIR:
        response->entry_type = FS_DIAG_FTYPE_DIR;
        break;

      case S_IFLNK:
        response->entry_type = FS_DIAG_FTYPE_LINK;
        break;

      default:
        response->entry_type = FS_DIAG_FTYPE_UNKNOWN;

        /* This used to be an error, but shouldn't be. */
        /* response->diag_errno = FS_DIAG_UNKNOWN_FILETYPE; */
        break;
    }
  }

  readdir_end:
  return response;
} /* END fs_diag_readdir_handler */


/*===========================================================================

FUNCTION FS_DIAG_CLOSEDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_CLOSEDIR packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_closedir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_closedir_req_type *request;
  efs2_diag_closedir_rsp_type *response;
  unsigned rsp_len = sizeof (efs2_diag_closedir_rsp_type);
  dir_table_entry *slot;
  int result;

  EFS_BAB_DEBUG_PRINTF ("_CLOSEDIR");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for exact length of packet */
  if (pkt_len != sizeof (efs2_diag_closedir_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_CLOSEDIR);

  request = (efs2_diag_closedir_req_type *) req_ptr;

  response = (efs2_diag_closedir_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_CLOSEDIR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Validate the slot. */
  if (request->dirp < 1 || request->dirp > FS_DIAG_MAX_OPEN_DIRS ||
      fsd_dir_table[request->dirp - 1].dirp == NULL)
  {
    response->diag_errno = FS_DIAG_DIR_NOT_OPEN;
    goto closedir_end;
  }
  slot = &fsd_dir_table[request->dirp - 1];

  result = efs_closedir (slot->dirp);
  slot->dirp = NULL;

  if (result == 0)
    response->diag_errno = 0;
  else
    response->diag_errno = fs_get_diag_errno (efs_errno);

  closedir_end:
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_closedir_handler */


/*===========================================================================

FUNCTION FS_DIAG_RENAME_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_RENAME packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_rename_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_rename_req_type *rename_req_ptr;
  efs2_diag_rename_rsp_type *rename_rsp_ptr;
  uint16 rsp_len = sizeof (efs2_diag_rename_rsp_type);
  const char *oldpath;
  const char *newpath;
  int32  oldpath_len;

  EFS_BAB_DEBUG_PRINTF ("_RENAME");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_rename_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_RENAME);

  rename_req_ptr = (efs2_diag_rename_req_type *) req_ptr;

  FSD_CHECK_NULLS (rename_req_ptr, efs2_diag_rename_req_type,
      oldpath, pkt_len, 2);

  /* Do a sanity check on the input parameters.
   */
  oldpath = (const char *) rename_req_ptr->oldpath;
  oldpath_len = (int32) strlen (oldpath);
  newpath = (const char *) (rename_req_ptr->oldpath + oldpath_len + 1);
  if (oldpath_len + strlen (newpath) + 2 > pkt_len)
  {
    return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

  FS_DIAG_VALIDATE_PATH_ACCESS (RENAME, oldpath);
  FS_DIAG_VALIDATE_PATH_ACCESS (RENAME, newpath);

  rename_rsp_ptr = (efs2_diag_rename_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_RENAME,
                          rsp_len);
  if (rename_rsp_ptr == NULL)
    return NULL;


  if (efs_rename (oldpath, newpath) == 0)
    rename_rsp_ptr->diag_errno = 0;
  else
    rename_rsp_ptr->diag_errno = fs_get_diag_errno (efs_errno);

  fsd_memoize_packet (req_ptr, pkt_len, rename_rsp_ptr, rsp_len);
  return rename_rsp_ptr;
} /* END fs_diag_rename_handler */


/*===========================================================================

FUNCTION FS_DIAG_STAT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_STAT packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_stat_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_stat_req_type *request;
  efs2_diag_stat_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_stat_rsp_type);
  struct fs_stat sbuf;
  const char *request_path;

  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_stat_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_stat_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_stat_req_type,
      path, pkt_len, 1);

  EFS_BAB_DEBUG_PRINTF ("_STAT: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (STAT, request_path);

  response = (efs2_diag_stat_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_STAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (efs_stat (request_path, &sbuf) == 0)
  {
    response->mode   = (int32) sbuf.st_mode;
    response->size   = (int32) sbuf.st_size;
    response->nlink  = (int32) sbuf.st_nlink;
    response->atime  = (int32) sbuf.st_atime;
    response->mtime  = (int32) sbuf.st_mtime;
    response->ctime  = (int32) sbuf.st_ctime;
    response->diag_errno  = 0;
  }
  else
  {
    response->mode   = 0;
    response->size   = 0;
    response->nlink  = 0;
    response->atime  = 0;
    response->mtime  = 0;
    response->ctime  = 0;
    response->diag_errno  = fs_get_diag_errno (efs_errno);
  }

  return response;
} /* END fs_diag_stat_handler */


/*===========================================================================

FUNCTION FS_DIAG_LSTAT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_LSTAT packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_lstat_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_lstat_req_type *request;
  efs2_diag_lstat_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_lstat_rsp_type);
  struct fs_stat sbuf;
  const char *request_path;

  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_lstat_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_lstat_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_lstat_req_type,
      path, pkt_len, 1);

  EFS_BAB_DEBUG_PRINTF ("_LSTAT: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (LSTAT, request_path);

  response = (efs2_diag_lstat_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_LSTAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* XXX: Figure out which of the following fields make sense for a
  ** symbolic link.
  */
  if (efs_lstat (request_path, &sbuf) == 0)
  {
    response->mode   = (int32) sbuf.st_mode;
    response->atime  = (int32) sbuf.st_atime;
    response->mtime  = (int32) sbuf.st_mtime;
    response->ctime  = (int32) sbuf.st_ctime;
    response->diag_errno  = 0;
  }
  else
  {
    response->mode   = 0;
    response->atime  = 0;
    response->mtime  = 0;
    response->ctime  = 0;
    response->diag_errno  = fs_get_diag_errno (efs_errno);
  }

  return response;
} /* END fs_diag_lstat_handler */


/*===========================================================================

FUNCTION FS_DIAG_FSTAT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_FSTAT packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_fstat_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_fstat_req_type *request;
  efs2_diag_fstat_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_fstat_rsp_type);
  desc_entry *slot;
  struct fs_stat sbuf;

  EFS_BAB_DEBUG_PRINTF ("_FSTAT");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_fstat_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_fstat_req_type *) req_ptr;
  response = (efs2_diag_fstat_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FSTAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Validate the requested descriptor. */
  if (request->fd < 0 || request->fd >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fd].fd < 0)
  {
    response->diag_errno = fs_get_diag_errno (EBADF);
    response->mode = 0;
    response->size = 0;
    response->nlink = 0;
    response->atime = 0;
    response->mtime = 0;
    response->ctime = 0;
    goto fstat_end;
  }

  slot = &fsd_desc_table[request->fd];

  if (efs_fstat (slot->fd, &sbuf) == 0) {
    response->mode   = (int32) sbuf.st_mode;
    response->size   = (int32) sbuf.st_size;
    response->nlink  = (int32) sbuf.st_nlink;
    response->atime  = (int32) sbuf.st_atime;
    response->mtime  = (int32) sbuf.st_mtime;
    response->ctime  = (int32) sbuf.st_ctime;
    response->diag_errno  = 0;
  } else {
    response->diag_errno  = fs_get_diag_errno (efs_errno);
    response->mode = 0;
    response->size = 0;
    response->nlink = 0;
    response->atime = 0;
    response->mtime = 0;
    response->ctime = 0;
  }

  fstat_end:
  return response;
}

/*===========================================================================

FUNCTION FS_DIAG_CHMOD_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_CHMOD packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_chmod_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_chmod_req_type *request;
  efs2_diag_chmod_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_chmod_rsp_type);
  int32  chmod_result;
  const char *request_path;

  EFS_BAB_DEBUG_PRINTF ("_CHMOD");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_chmod_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_chmod_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_chmod_req_type,
      path, pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (CHMOD, request_path);

  response = (efs2_diag_chmod_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_CHMOD,
                          rsp_len);
  if (response == NULL)
    return NULL;

  chmod_result = efs_chmod (request_path, request->mode);
  if (chmod_result == 0)
    response->diag_errno  = 0;
  else
    response->diag_errno  = fs_get_diag_errno (efs_errno);

  return response;
} /* END fs_diag_chmod_handler */


/*===========================================================================

FUNCTION FS_DIAG_STATFS_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_STATFS packet. This has been extended to
  EFS2_DIAG_STATVFS_V2_HANDLER which contains more filesystem specific info
============================================================================*/
VOID_PACKED_PTR
fs_diag_statfs_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_statfs_req_type *request;
  efs2_diag_statfs_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_statfs_rsp_type);
  struct fs_statvfs sbuf;
  const char *request_path;

  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_statfs_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_statfs_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_statfs_req_type,
      path, pkt_len, 1);

  EFS_BAB_DEBUG_PRINTF ("_STATFS: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (STATFS, request_path);

  response = (efs2_diag_statfs_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_STATFS,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (efs_statvfs (request_path, &sbuf) == 0)
  {
    /* XXX: Fill in all the fields here.
     */
    response->diag_errno         = 0;
    response->fs_id         = (int32) sbuf.f_fsid;
    memset ((void *)response->fs_type, 0x00, EFS_FSTYPE_LEN);
    response->block_size    = (int32) sbuf.f_bsize;
    response->total_blocks  = (int32) sbuf.f_blocks;
    response->avail_blocks  = (int32) sbuf.f_bavail;
    response->free_blocks   = (int32) sbuf.f_bfree;
    response->max_file_size = 0;
    response->nfiles        = 0;
    response->max_nfiles    = 0;
  }
  else
  {
    response->diag_errno  = fs_get_diag_errno (efs_errno);
  }

  return response;
} /* END fs_diag_statfs_handler */


/*===========================================================================

FUNCTION FS_DIAG_ACCESS_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_ACCESS packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_access_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_access_req_type *request;
  efs2_diag_access_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_access_rsp_type);
  const char *request_path;

  EFS_BAB_DEBUG_PRINTF ("_ACCESS");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_access_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_access_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_access_req_type,
      path, pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (ACCESS, request_path);

  response = (efs2_diag_access_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_ACCESS,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (efs_access (request_path, request->perm_bits) == 0)
    response->diag_errno  = 0;
  else
    response->diag_errno  = fs_get_diag_errno (efs_errno);

  return response;
} /* END fs_diag_access_handler */


/*===========================================================================

FUNCTION FS_DIAG_DEVICE_INFO_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_DEV_INFO packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_device_info_handler (VOID_PACKED_PTR req_ptr,
                             uint16 pkt_len)
{
  int error;
  efs2_diag_dev_info_req_type *dev_info_req_ptr;
  efs2_diag_dev_info_rsp_type *dev_info_rsp_ptr = NULL;
  uint16 rsp_len = sizeof (efs2_diag_dev_info_rsp_type);
  struct fs_device_attr device_attr;

  EFS_BAB_DEBUG_PRINTF ("_DEV_INFO");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED)
  {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_dev_info_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  dev_info_req_ptr = (efs2_diag_dev_info_req_type *)req_ptr;

  error = efs_get_device_info ("/", &device_attr);
  if (error == 0)
  {
    unsigned name_length = strlen (device_attr.device_name);
    rsp_len += name_length + 1;

    dev_info_rsp_ptr = (efs2_diag_dev_info_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_DEV_INFO,
                          rsp_len);

    if (dev_info_rsp_ptr == NULL)
      return NULL;

    if (device_attr.device_type == FS_DEV_TYPE_EFS_NAND)
    {
      dev_info_rsp_ptr->diag_errno = 0;
      dev_info_rsp_ptr->maker_id = device_attr.device_maker_id;
      dev_info_rsp_ptr->device_id = device_attr.device_id;
      dev_info_rsp_ptr->total_no_of_blocks = device_attr.block_count;
      dev_info_rsp_ptr->no_of_pages_per_block = device_attr.block_size;
      dev_info_rsp_ptr->page_size = device_attr.page_size;
      dev_info_rsp_ptr->total_page_size = device_attr.total_page_size;

      dev_info_rsp_ptr->device_type = FS_DIAG_NAND_DEV;

      /* Yes, we're passing the source length in.  We computed it above
       * when we allocated the buffer, so we know there is room here.  A
       * regular 'strcpy' would be safe, but that is forbidden. */
      strlcpy ((char*)dev_info_rsp_ptr->device_name,
        device_attr.device_name, name_length + 1);
    }
    else
     dev_info_rsp_ptr->diag_errno = FS_DIAG_NOT_NAND_FLASH;
  }
  else
  {
    return fs_diag_efs2_error_rsp (efs_errno, dev_info_req_ptr, pkt_len);
  }

  return dev_info_rsp_ptr;

} /* END fs_diag_device_info_handler */


/*===========================================================================

FUNCTION FS_DIAG_FACTIMAGE_START_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_FACT_IMAGE_START packet. This function put the phone
  in offline mode prepares for image dump to start.
============================================================================*/
VOID_PACKED_PTR
fs_diag_factimage_start_handler (VOID_PACKED_PTR req_ptr,
                                 uint16 pkt_len)
{
  efs2_diag_factimage_start_rsp_type *factimage_start_rsp_ptr;
  uint16 rsp_len = sizeof (efs2_diag_factimage_start_rsp_type);

  EFS_BAB_DEBUG_PRINTF ("_FACT_IMAGE_START");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for exact length of packet */
  if (pkt_len != sizeof (efs2_diag_factimage_start_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_FACT_IMAGE_START);

  factimage_start_rsp_ptr = (efs2_diag_factimage_start_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FACT_IMAGE_START,
                          rsp_len);

  if (factimage_start_rsp_ptr == NULL)
    return NULL;

  /* Acquire exclusive access during factory image output */
  FS_GLOBAL_LOCK();

  factimage_start_rsp_ptr->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, factimage_start_rsp_ptr, rsp_len);

  return factimage_start_rsp_ptr;

} /* END fs_diag_factimage_start_handler */


/*===========================================================================

FUNCTION FS_DIAG_FACTIMAGE_READ_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_FACT_IMAGE_READ packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_factimage_read_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  int image_data_errno;
  struct fs_factimage_read_info read_info;
  byte *page_data;

  efs2_diag_factimage_read_req_type *image_data_req_ptr;
  efs2_diag_factimage_read_rsp_type *image_data_rsp_ptr;
  uint16 rsp_len = sizeof (efs2_diag_factimage_read_rsp_type);

  EFS_BAB_DEBUG_PRINTF ("_FACT_IMAGE_READ");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_factimage_read_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  image_data_req_ptr = (efs2_diag_factimage_read_req_type *)req_ptr;

  /* Filesystem page size is 512 bytes */
  rsp_len += 512;

  image_data_rsp_ptr = (efs2_diag_factimage_read_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FACT_IMAGE_READ,
                          rsp_len);
  if (image_data_rsp_ptr == NULL)
    return NULL;

  read_info.stream_state       = image_data_req_ptr->stream_state;
  read_info.info_cluster_sent  = image_data_req_ptr->info_cluster_sent;
  read_info.cluster_map_seqno  = image_data_req_ptr->cluster_map_seqno;
  read_info.cluster_data_seqno = image_data_req_ptr->cluster_data_seqno;

  page_data = (byte *) image_data_rsp_ptr->page;

  image_data_errno = efs_get_fs_data ("/", &read_info, page_data);

  if (image_data_errno != 0)
  {
    image_data_rsp_ptr->diag_errno = fs_get_diag_errno (efs_errno);
  }
  else
  {
    image_data_rsp_ptr->diag_errno = 0;
  }

  image_data_rsp_ptr->stream_state       = read_info.stream_state;
  image_data_rsp_ptr->info_cluster_sent  = read_info.info_cluster_sent;
  image_data_rsp_ptr->cluster_map_seqno  = read_info.cluster_map_seqno;
  image_data_rsp_ptr->cluster_data_seqno = read_info.cluster_data_seqno;

  return image_data_rsp_ptr;

} /* END fs_diag_factimage_read_handler */


/*===========================================================================

FUNCTION FS_DIAG_FACTIMAGE_END_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_FACT_IMAGE_END packet. This function handles phone
  state after image dump.
============================================================================*/
VOID_PACKED_PTR
fs_diag_factimage_end_handler (VOID_PACKED_PTR req_ptr,
                               uint16 pkt_len)
{
  efs2_diag_factimage_end_rsp_type *factimage_end_rsp_ptr;
  uint16 rsp_len = sizeof (efs2_diag_factimage_end_rsp_type);

  EFS_BAB_DEBUG_PRINTF ("_FACT_IMAGE_END");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for exact length of packet */
  if (pkt_len != sizeof (efs2_diag_factimage_end_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_FACT_IMAGE_END);

  factimage_end_rsp_ptr = (efs2_diag_factimage_end_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FACT_IMAGE_END,
                          rsp_len);
  if (factimage_end_rsp_ptr == NULL)
    return NULL;

  /* Release exclusive access */
  FS_GLOBAL_UNLOCK();

  factimage_end_rsp_ptr->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, factimage_end_rsp_ptr, rsp_len);

  return factimage_end_rsp_ptr;

} /* END fs_diag_factimage_end_handler */

/*===========================================================================

FUNCTION FS_DIAG_PREP_FACTIMAGE_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_PREP_FACT_IMAGE packet. This function helps indicate
    to file system to prepare for a factory image dump.
============================================================================*/
VOID_PACKED_PTR
fs_diag_prep_factimage_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  int32 status = 1;
  qword now;
  qword start_time;
  qword elapsed;

  efs2_diag_prep_factimage_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_prep_factimage_rsp_type);

  EFS_BAB_DEBUG_PRINTF ("_PREP_FACT_IMAGE");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_prep_factimage_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (efs2_diag_prep_factimage_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS, EFS2_DIAG_PREP_FACT_IMAGE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  fs_time_ms (start_time);

  while(status != 0)
  {
    status = efs_image_prepare ("/");
    if(status == 0)
      break;
    /* If elapsed time is too large (approx greater than 500 millisec)
       return */
    fs_time_ms (now);
    qw_sub (elapsed, now, start_time);
    if(qw_hi (elapsed) != 0 || qw_lo (elapsed) > 400)
      break;
  }

  if (status != 0)
  {
    response->diag_errno = fs_get_diag_errno (efs_errno);
  }
  else
  {
    response->diag_errno = 0;
  }

  return response;

} /* END fs_diag_prep_factimage_handler */


/*===========================================================================

FUNCTION FS_DIAG_GET_DEPRECATED_HANDLER
DESCRIPTION
  Handles the FS_DIAG_GET_DEPRECATED_HANDLER packet, which is a old way of
  EFS2_DIAG_GET with out any sequence number support.

============================================================================*/
VOID_PACKED_PTR
fs_diag_get_deprecated_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  efs2_diag_get_deprecated_req_type *request;
  efs2_diag_get_deprecated_rsp_type *response;
  unsigned rsp_len = 0;
  unsigned base_length = 0;
  fs_ssize_t bytes_read = 0;
  const char *request_path;

  EFS_BAB_DEBUG_PRINTF ("_GET_DEPRECATED");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_get_deprecated_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_get_deprecated_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_get_deprecated_req_type, path,
      pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (READ, request_path);

  if (request->data_length > FS_DIAG_MAX_READ_REQ) {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  base_length = offsetof (efs2_diag_get_deprecated_rsp_type, data);
  rsp_len = base_length + request->data_length;

  response = (efs2_diag_get_deprecated_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_GET_DEPRECATED,
                          rsp_len);
  if (response == NULL)
    return NULL;

  bytes_read = efs_get (request_path,
                        (void *) response->data,
                        request->data_length);

  if (bytes_read >= 0) {
    response->num_bytes  = bytes_read;
    response->diag_errno      = 0;

    if ((unsigned) bytes_read < request->data_length)
      diagpkt_shorten (response, base_length + bytes_read);
  } else {
    response->num_bytes  = -1;
    response->diag_errno = fs_get_diag_errno (efs_errno);
    diagpkt_shorten (response, base_length);
  }

  return response;
} /* END fs_diag_get_deprecated_handler */


/*===========================================================================

FUNCTION FS_DIAG_PUT_DEPRECATED_HANDLER
DESCRIPTION
  Handles the FS_DIAG_PUT_DEPRECATED_HANDLER packet, which is a old way of
  EFS2_DIAG_PUT with out any sequence number support.

============================================================================*/
VOID_PACKED_PTR
fs_diag_put_deprecated_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  efs2_diag_put_deprecated_req_type *request;
  efs2_diag_put_deprecated_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_put_deprecated_rsp_type);
  fs_ssize_t nbyte;
  fs_ssize_t bytes_written = 0;
  unsigned   data_offset;
  char      *put_locn;
  char      *put_path;

  EFS_BAB_DEBUG_PRINTF ("_PUT_DEPRECATED");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_put_deprecated_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_put_deprecated_req_type *) req_ptr;

  put_locn = (char *) request->data;
  nbyte    = request->data_length;
  put_path = (char *) request->data + nbyte;
  data_offset = offsetof (efs2_diag_put_deprecated_req_type, data);

  /* Validate this packet, more complicated than others, because there is a
   * null-terminated string after variable length data. */
  if ((nbyte + data_offset >= pkt_len) ||
      (fsd_count_nulls (put_path, pkt_len - (data_offset + nbyte)) < 1))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len));
  }

  FS_DIAG_VALIDATE_PATH_ACCESS (WRITE, put_path);

  response = (efs2_diag_put_deprecated_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_PUT_DEPRECATED,
                          rsp_len);
  if (response == NULL)
    return NULL;

  bytes_written = efs_put (put_path,
                           (void *) put_locn,
                           nbyte, request->flags,
                           0777);

  if (bytes_written < 0)
  {
    response->diag_errno = fs_get_diag_errno (efs_errno);
  }
  else
  {
    /* (bytes_written >= 0) when it gets here */
    response->diag_errno = 0;
  }

  return response;
} /* END fs_diag_put_deprecated_handler */


/*===========================================================================

FUNCTION FS_DIAG_GET_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_GET packet. The field sequence_number is used for the
  purpose of Synchronization / Correlation purpose. The sequence_number
  received is fetched out of request packet and tucked back into the response
  packet. This would benefit the application with synchronized requests and
  responses.

============================================================================*/
VOID_PACKED_PTR
fs_diag_get_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_get_req_type *request;
  efs2_diag_get_rsp_type *response;
  unsigned rsp_len = 0;
  unsigned base_length = 0;
  fs_ssize_t bytes_read = 0;
  const char *request_path;
  uint16 seq_num;

  EFS_BAB_DEBUG_PRINTF ("_GET");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (efs2_diag_get_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_get_req_type *) req_ptr;
  request_path = (const char *) request->path;
  /*
   * Fetch the Sequence number commonly used by the calling application
   * for Synchronization / Correlation purpose.
   */
  seq_num = request->sequence_number;

  FSD_CHECK_NULLS (request, efs2_diag_get_req_type, path,
      pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (READ, request_path);

  if (request->data_length > FS_DIAG_MAX_READ_REQ) {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  base_length = FPOS (efs2_diag_get_rsp_type, data);
  rsp_len = base_length + request->data_length;

  response = (efs2_diag_get_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_GET,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = seq_num;
  bytes_read = efs_get (request_path,
                        (void *) response->data,
                        request->data_length);

  if (bytes_read >= 0) {
    response->num_bytes  = bytes_read;
    response->diag_errno      = 0;

    if ((unsigned) bytes_read < request->data_length)
      diagpkt_shorten (response, base_length + bytes_read);
  } else {
    response->num_bytes  = -1;
    response->diag_errno = fs_get_diag_errno (efs_errno);
    diagpkt_shorten (response, base_length);
  }

  return response;
} /* END fs_diag_get_handler */


/*===========================================================================

FUNCTION FS_DIAG_PUT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_PUT packet. The field sequence_number is used for the
  purpose of Synchronization / Correlation purpose. The sequence_number
  received is fetched out of request packet and tucked back into the response
  packet. This would benefit the application with synchronized requests and
  responses.

============================================================================*/
VOID_PACKED_PTR
fs_diag_put_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_put_req_type *request;
  efs2_diag_put_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_put_rsp_type);
  fs_ssize_t nbyte;
  fs_ssize_t bytes_written = 0;
  unsigned   data_offset;
  char      *put_locn;
  char      *put_path;
  uint16 seq_num;

  EFS_BAB_DEBUG_PRINTF ("_PUT");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (efs2_diag_put_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_put_req_type *) req_ptr;

  put_locn = (char *) request->data;
  nbyte    = request->data_length;
  put_path = (char *) request->data + nbyte;
  data_offset = FPOS (efs2_diag_put_req_type, data);
  /*
   * Fetch the Sequence number usually used by the calling application
   * for Synchronization / Correlation purpose.
   */
  seq_num = request->sequence_number;

  /* Validate this packet, more complicated than others, because there is a
   * null-terminated string after variable length data. */
  if ((nbyte + data_offset >= pkt_len) ||
      (fsd_count_nulls (put_path, pkt_len - (data_offset + nbyte)) < 1))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len));
  }

  FS_DIAG_VALIDATE_PATH_ACCESS (WRITE, put_path);

  response = (efs2_diag_put_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_PUT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = seq_num;

  bytes_written = efs_put (put_path,
                           (void *) put_locn,
                           nbyte, request->flags,
                           0777);
  if (bytes_written < 0)
  {
    response->diag_errno = fs_get_diag_errno (efs_errno);
  }
  else
  {
    /* (bytes_written >= 0) when it gets here */
    response->diag_errno = 0;
  }
  return response;
} /* END fs_diag_put_handler */


/*
 * Populuates and returns the error respoonse packet.
 *
 * NOTE:The error passed should be the POSIX or extended errors
 * only this function will perform the mapping before sending the response.
 */
VOID_PACKED_PTR
fs_diag_efs2_error_rsp (int32 diag_errno, VOID_PACKED_PTR req_pkt,
                        uint16 req_len)
{
  efs2_diag_error_rsp_type *response;
  unsigned int rsp_len;

  /* Lint is too stupid to realize that offsetof() has no side
   * effects, so it prohibits the use of the MIN() macro here (error
   * 666).  So we spell it out using small words instead. */
  rsp_len = req_len + offsetof (efs2_diag_error_rsp_type, pkt);
  if (rsp_len > sizeof (efs2_diag_error_rsp_type))
    rsp_len = sizeof (efs2_diag_error_rsp_type);

  response = (efs2_diag_error_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_ERROR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->diag_errno = fs_get_diag_errno (diag_errno);

  fs_memscpy ((void *) response->pkt,
              rsp_len - offsetof (efs2_diag_error_rsp_type, pkt),
              (void *) req_pkt,
              rsp_len - offsetof (efs2_diag_error_rsp_type, pkt));

  return ((void *) response);
}/* diagkt_err_rsp */

/*===========================================================================

FUNCTION FS_DIAG_BENCHMARK_START_TEST_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_BENCHMARK_START_TEST packet. Initializes the EFS2 Diag
  interface and passes the values of some parameters back to the tool.

============================================================================*/
VOID_PACKED_PTR
fs_diag_benchmark_start_test_handler (VOID_PACKED_PTR req_ptr,
                                      uint16 pkt_len)
{
  efs2_diag_benchmark_start_test_rsp_type *response;
  efs2_diag_benchmark_start_test_req_type *request =
    (efs2_diag_benchmark_start_test_req_type *) req_ptr;
  const char *request_path;
  uint32 cmd_offset, path_offset;

  fsd_invalidate_memoization ();

  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Atleast an empty (Null only) path */
  if (pkt_len < sizeof (*request))
  {
    EFS_BAB_DEBUG_PRINTF ("Unexpected start_test packet length = %d, SIZEOF"
                          " = %d", pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  /* Compute the offset of the cmd structure and then use it to find the
   * offset of the path supplied in the packet. */
  cmd_offset = FPOS (efs2_diag_benchmark_start_test_req_type, cmd);
  path_offset = cmd_offset + FPOS (fs_benchmark_test_cmd, path);

  /* Ensure path is null terminated so we dont over read */
  if (fsd_count_nulls (request->cmd.path, (pkt_len - path_offset)) < 1)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
  }

  /* RVCT won't allow a packed string into strlcpy */
  request_path = (const char *) request->cmd.path;

  if (strlen (request_path) > FS_PATH_MAX)
  {
     return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
  }

  /* Make sure they're not benchmarking a forbidden pathname */
  FS_DIAG_VALIDATE_PATH_ACCESS (OPEN, request_path);

  response = (efs2_diag_benchmark_start_test_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_BENCHMARK_START_TEST,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  response->cmd = request->cmd;
  response->success = fs_benchmark_put_cmd (&request->cmd) ? 0 : 1;

  return response;
}

/*===========================================================================

FUNCTION FS_DIAG_BENCHMARK_GET_RESULTS_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_BENCHMARK_GET_RESULTS packet. Polls about the response
  queue from fs_benchmark().

============================================================================*/
VOID_PACKED_PTR
fs_diag_benchmark_get_results_handler (VOID_PACKED_PTR req_ptr,
                                       uint16 pkt_len)
{
  efs2_diag_benchmark_get_results_rsp_type *response;

  fsd_invalidate_memoization ();

  if (pkt_len != sizeof (efs2_diag_benchmark_get_results_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (efs2_diag_benchmark_get_results_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_BENCHMARK_GET_RESULTS,
                          sizeof(*response));
  if (response == NULL)
    return NULL;

  response->queue_status = fs_benchmark_get_rpt (&response->report);

  return response;
}

/*===========================================================================

FUNCTION FS_DIAG_BENCHMARK_INIT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_BENCHMARK_INIT packet. Resets the command and report
  queues. Needs further modification in fs_benchmark_reset_all() for proper
  functioning

============================================================================*/
VOID_PACKED_PTR
fs_diag_benchmark_init_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  efs2_diag_benchmark_init_rsp_type *response;

  fsd_invalidate_memoization ();

  if (pkt_len != sizeof (efs2_diag_benchmark_init_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (efs2_diag_benchmark_init_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_BENCHMARK_INIT,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  fs_benchmark_flush();

  /*
    Future implementation may address aborting a pending/running
    command.  The queues are emptied explicitly, but nothing is done
    to abort a command that got off the queue and is running Right
    Now.  That response will still appear after this.
  */
  return response;
}

VOID_PACKED_PTR
fs_diag_chown_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_errno_rsp_type *response;
  efs2_diag_chown_req_type *request = (efs2_diag_chown_req_type *) req_ptr;
  const char *request_path;

  fsd_invalidate_memoization ();

  /* Require SPC Unlock */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    EFS_BAB_DEBUG_PRINTF ("Unexpected packet length = %d, SIZEOF = %d",
                          pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_CHECK_NULLS (request, efs2_diag_chown_req_type,
      path, pkt_len, 1);

  request_path = (const char *) request->path;

  /* Make sure we're not chowning a forbidden pathname */
  FS_DIAG_VALIDATE_PATH_ACCESS (OPEN, request_path);

  response = (efs2_diag_errno_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_CHOWN,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  if (efs_chown (request_path, request->uid_val, request->gid_val))
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  return response;
}

VOID_PACKED_PTR
fs_diag_set_reservation_handler (VOID_PACKED_PTR req_ptr,
                                 uint16 pkt_len)
{
  efs2_diag_errno_rsp_type *response;
  efs2_diag_set_reservation_req_type *request =
    (efs2_diag_set_reservation_req_type *) req_ptr;
  const char *request_path;

  fsd_invalidate_memoization ();
  /* Require SPC Unlock */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    EFS_BAB_DEBUG_PRINTF ("Unexpected packet length = %d, SIZEOF = %d",
                          pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_CHECK_NULLS (request, efs2_diag_set_reservation_req_type,
      path, pkt_len, 1);

  request_path = (const char *) request->path;
  /*Path access check*/
  FS_DIAG_VALIDATE_PATH_ACCESS (SET_RESERVATION, request_path);
  response = (efs2_diag_errno_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SET_RESERVATION,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  if (efs_set_reservation (request_path, request->gid, request->bytes))
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  return response;
}


VOID_PACKED_PTR
fs_diag_set_quota_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_errno_rsp_type *response;
  efs2_diag_set_quota_req_type *request =
    (efs2_diag_set_quota_req_type *) req_ptr;
  const char *request_path;

  fsd_invalidate_memoization ();
  /* Require SPC Unlock */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    EFS_BAB_DEBUG_PRINTF ("Unexpected packet length = %d, SIZEOF = %d",
                          pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_CHECK_NULLS (request, efs2_diag_set_quota_req_type,
      path, pkt_len, 1);

  request_path = (const char *) request->path;

  /*Path access check*/
  FS_DIAG_VALIDATE_PATH_ACCESS (SET_QUOTA, request_path);

  response = (efs2_diag_errno_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SET_QUOTA,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  if (efs_set_quota (request_path, request->gid, request->bytes))
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  return response;
}

VOID_PACKED_PTR
fs_diag_get_group_info_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  efs2_diag_get_group_info_rsp_type *response;
  efs2_diag_get_group_info_req_type *request =
    (efs2_diag_get_group_info_req_type *) req_ptr;
  struct fs_group_info info;
  const char *request_path;

  fsd_invalidate_memoization ();
  /* Require SPC Unlock */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    EFS_BAB_DEBUG_PRINTF ("Unexpected packet length = %d, SIZEOF = %d",
                          pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_CHECK_NULLS (request, efs2_diag_get_group_info_req_type,
      path, pkt_len, 1);

  request_path = (const char *) request->path;

  /*Path access check*/
  FS_DIAG_VALIDATE_PATH_ACCESS (GET_GROUP_INFO, request_path);

  response = (efs2_diag_get_group_info_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_GET_GROUP_INFO,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  if (efs_get_group_info (request_path, request->gid, &info))
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  response->info.quota_size = info.quota_size;
  response->info.reservation_size = info.reservation_size;
  response->info.space_used = info.space_used;
  return response;
}

VOID_PACKED_PTR
fs_diag_deltree_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_deltree_req_type *request;
  efs2_diag_deltree_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_deltree_rsp_type);
  const char *request_path;
  const char *valid_path = "/public/";

  EFS_BAB_DEBUG_PRINTF ("_DELTREE");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_deltree_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_DELTREE);

  request = (efs2_diag_deltree_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_deltree_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_deltree_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_DELTREE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  memset(fs_diag_real_path,0,sizeof(fs_diag_real_path));
    /* Normalize the requested path */
  efs_realpath (request_path, fs_diag_real_path);

  /*
   * Check for "/public/" to be the root directory. None other than
   * /public is permitted to be deleted in EFS.
   */
  if ((strncmp (fs_diag_real_path, valid_path, strlen (valid_path)) == 0)
      && (strlen (fs_diag_real_path) > strlen (valid_path)))
  {
    /* Good to deltree the path. */
    if (efs_deltree (fs_diag_real_path) != 0)
      response->diag_errno = fs_get_diag_errno (efs_errno);
    else
      response->diag_errno = 0;
  }
  else
  {
    /* Does not match with "/public/" Return Error */
    response->diag_errno = fs_get_diag_errno (EPERM);
  }
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_deltree_handler */

VOID_PACKED_PTR
fs_diag_truncate_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_truncate_req_type *request;
  efs2_diag_truncate_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_truncate_rsp_type);
  const char *request_path;

  EFS_BAB_DEBUG_PRINTF ("_TRUNCATE");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                                 sizeof (efs2_diag_truncate_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_TRUNCATE);

  request = (efs2_diag_truncate_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_truncate_req_type,
                   path, pkt_len, 1);

  /*Path access check*/
  FS_DIAG_VALIDATE_PATH_ACCESS (TRUNCATE, request_path);
  response = (efs2_diag_truncate_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_TRUNCATE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* Good to truncate the path. */
  if (efs_truncate (request_path, request->length) != 0)
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_truncate_handler */

VOID_PACKED_PTR
fs_diag_ftruncate_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_ftruncate_req_type *request;
  efs2_diag_ftruncate_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_ftruncate_rsp_type);
  desc_entry *slot;

  EFS_BAB_DEBUG_PRINTF ("_FTRUNCATE");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                                 sizeof (efs2_diag_ftruncate_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_FTRUNCATE);

  request = (efs2_diag_ftruncate_req_type *) req_ptr;

  response = (efs2_diag_ftruncate_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FTRUNCATE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* Validate the requested descriptor. */
  if (request->fildes < 0 || request->fildes >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fildes].fd < 0)
  {
    response->diag_errno = fs_get_diag_errno (EBADF);
    goto ftruncate_end;
  }

  slot = &fsd_desc_table[request->fildes];
  /* Good to ftruncate the path. */
  if (efs_ftruncate (slot->fd, request->length) != 0)
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  ftruncate_end:
  return response;
} /* END fs_diag_ftruncate_handler */

/*===========================================================================

FUNCTION FS_DIAG_STATVFS_V2_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_STATVFS_V2 packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_statvfs_v2_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_statvfs_v2_req_type *request;
  efs2_diag_statvfs_v2_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_statvfs_v2_rsp_type);
  struct fs_statvfs sbuf;
  const char *request_path;

  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                                 sizeof (efs2_diag_statvfs_v2_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request = (efs2_diag_statvfs_v2_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_statvfs_v2_req_type,
      path, pkt_len, 1);

  EFS_BAB_DEBUG_PRINTF ("_STATVFS_V2: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (STATFS, request_path);

  response = (efs2_diag_statvfs_v2_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_STATVFS_V2,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  if (efs_statvfs (request_path, &sbuf) == 0)
  {
    /* Fill in packet with returned values */
    response->diag_errno         = 0;
    response->fs_id         = sbuf.f_fsid;
    memset ((void *)response->fs_type, 0, sizeof(response->fs_type));
    response->block_size    = sbuf.f_bsize;
    response->total_blocks  = sbuf.f_blocks;
    response->avail_blocks  = sbuf.f_bavail;
    response->free_blocks   = sbuf.f_bfree;
    response->max_file_size = sbuf.f_max_file_size;
    response->nfiles        = 0;
    response->max_nfiles    = 0;

    response->f_maxwrite           = sbuf.f_maxwrite;
    response->f_pathmax            = sbuf.f_pathmax;
    response->f_is_case_sensitive  = sbuf.f_is_case_sensitive;
    response->f_is_case_preserving = sbuf.f_is_case_preserving;
    response->f_max_file_size      = sbuf.f_max_file_size;
    response->f_max_file_size_unit = sbuf.f_max_file_size_unit;
    response->f_max_open_files     = sbuf.f_max_open_files;
    response->f_name_rule          = sbuf.f_name_rule;
    response->f_name_encoding      = sbuf.f_name_encoding;

    response->diag_errno = 0;
  }
  else
  {
    response->diag_errno  = fs_get_diag_errno (efs_errno);
  }

  return response;
} /* END fs_diag_statvfs_v2_handler */


VOID_PACKED_PTR
ext_simple_query (efs2_diag_extended_info_req_type *request, unsigned len);

/*======================================================================

FUNCTION FS_DIAG_EXTENDED_INFO_HANDLER
DESCRIPTION
  Handle the extended info request packet.
=====================================================================*/
VOID_PACKED_PTR
fs_diag_extended_info_handler (VOID_PACKED_PTR req_ptr,
                               uint16 pkt_len)
{
  efs2_diag_extended_info_req_type *request;

  fsd_invalidate_memoization ();

  /* Check security. */
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
    return diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len);

  if (pkt_len < sizeof (*request))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_extended_info_req_type *) req_ptr;

  /* Validate nul termination of path. */
  FSD_CHECK_NULLS (request, efs2_diag_extended_info_req_type,
      path, pkt_len, 1);

  switch (request->query)
  {
    case FS_DIAG_EXT_QUERY_DEVICE_NAME:
    case FS_DIAG_EXT_QUERY_DEVICE_ID:
    case FS_DIAG_EXT_QUERY_FILESYSTEM_BLOCKS:
    case FS_DIAG_EXT_QUERY_PAGES_PER_BLOCK:
    case FS_DIAG_EXT_QUERY_PAGE_SIZE:
    case FS_DIAG_EXT_QUERY_FLASH_BUS_WIDTH:
    case FS_DIAG_EXT_QUERY_TOTAL_BLOCKS:
      return ext_simple_query (request, pkt_len);

    default:
      return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }
}

VOID_PACKED_PTR
ext_simple_query (efs2_diag_extended_info_req_type *request, unsigned len)
{
  int result;
  unsigned rsp_len;
  efs2_diag_extended_info_rsp_type *response = NULL;
  PACKED uint32 * PACKED_POST item = NULL;
  static struct fs_device_attr attr;
  unsigned dev_name_length;

  result = efs_get_device_info ("/", &attr);
  if (result != 0)
    return fs_diag_efs2_error_rsp (efs_errno, (void *) request, len);

  if (request->query == FS_DIAG_EXT_QUERY_DEVICE_ID
      && request->data > 1)
    return fs_diag_efs2_error_rsp (FS_DIAG_UNAVAILABLE_INFO,
                                   (void *) request, len);

  rsp_len = sizeof (*response) + sizeof (uint32);

  dev_name_length = strlen (attr.device_name);
  if (request->query == FS_DIAG_EXT_QUERY_DEVICE_NAME)
    rsp_len = sizeof (*response) + dev_name_length + 1;

  response = (efs2_diag_extended_info_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_EXTENDED_INFO, rsp_len);
  if (response == NULL)
    return NULL;

  item = (uint32 *)
    ((void *)(response + 1)); /* lint: hop over void */
  response->kind = FS_DIAG_EXT_KIND_INT;

  switch (request->query) {
    case FS_DIAG_EXT_QUERY_DEVICE_ID:
      if (request->data == 0)
        *item = attr.device_maker_id;
      else
        *item = attr.device_id;
      break;

    case FS_DIAG_EXT_QUERY_FILESYSTEM_BLOCKS:
      *item = attr.block_count;
      break;

    case FS_DIAG_EXT_QUERY_PAGES_PER_BLOCK:
      *item = attr.block_size;
      break;

    case FS_DIAG_EXT_QUERY_PAGE_SIZE:
      *item = attr.page_size;
      break;

    case FS_DIAG_EXT_QUERY_TOTAL_BLOCKS:
      *item = attr.total_blocks;
      break;

    case FS_DIAG_EXT_QUERY_FLASH_BUS_WIDTH:
      *item = attr.bus_width;
      break;

    case FS_DIAG_EXT_QUERY_DEVICE_NAME:
      response->kind = FS_DIAG_EXT_KIND_STRING;
      strlcpy ((char *) (response + 1), attr.device_name,
          dev_name_length + 1);
      break;

    default:
      /* Something is wrong. */
      FS_MSG_HIGH_0 ("Internal error in ext_num_query");
      *item = 0xFFFFFFFF;
  }

  return response;
}

VOID_PACKED_PTR
fs_diag_md5sum_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  efs2_diag_md5sum_req_type *request;
  efs2_diag_md5sum_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_md5sum_rsp_type);
  const char *request_path;

  EFS_BAB_DEBUG_PRINTF ("_MD5SUM");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /*
   * Check for minimum length of packet
   */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_md5sum_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request = (efs2_diag_md5sum_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_md5sum_req_type,
                   path, pkt_len, 1);

  /*Path access check*/
  FS_DIAG_VALIDATE_PATH_ACCESS (MD5SUM, request_path);

  response = (efs2_diag_md5sum_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_MD5SUM,
                          rsp_len);

  if (response)
  {
    struct fs_md5sum md5sum;
    /* Acknowledge back with the Sequence number */
    response->sequence_number = request->sequence_number;

    /* md5 hash calculation */
    if (efs_md5sum (request_path, &md5sum) != 0)
      response->diag_errno = fs_get_diag_errno (efs_errno);
    else {
      /* Copy hash bytes into packed result struct */
      fs_memscpy ((void*) response->hash_result, sizeof(response->hash_result),
                  md5sum.hash, 16);
      response->diag_errno = 0;
    }
  }

  return response;
} /* END fs_diag_md5sum_handler */


/*===========================================================================

FUNCTION   FS_DIAG_HOTPLUG_FORMAT_HANDLER
DESCRIPTION
   Handles the request packet for hotplug format.

   This function receives a request packet to format a storage
   media. It returns a packet indicating if successful or if
   an error occurred.

PARAMETERS
   req_ptr  pointer to request packet
   pkt_len  received packet length

RETURN
 Pointer to response packet. If pointer is NULL packet could not be created.
 On success the packet should contain a zero in the error field. If format did
 not occurred because there is no device or hotplug_format failed the error
 code ENODEV is returned.

===========================================================================*/

VOID_PACKED_PTR
fs_diag_hotplug_format_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_hotplug_format_req_type *request;
  efs2_diag_hotplug_format_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_hotplug_format_rsp_type);
  const char *request_path;

  EFS_BAB_DEBUG_PRINTF ("_HOTPLUG_FORMAT");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                                 sizeof (efs2_diag_hotplug_format_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_HOTPLUG_FORMAT);

  request = (efs2_diag_hotplug_format_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_hotplug_format_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_hotplug_format_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_HOTPLUG_FORMAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

#ifdef FEATURE_FS_HOTPLUG
  if (hotplug_format (request_path))
  { /* 0 if it formats it, -1 if error
      we return no such device error. */
    response->diag_errno = fs_get_diag_errno (ENODEV);
  }
  else
  {
    response->diag_errno = 0;
  }
#else
  (void) request_path;
  response->diag_errno = fs_get_diag_errno (EPERM);
#endif
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return (response);
}/* End of fs_diag_hotplug_format_handler */

VOID_PACKED_PTR
fs_diag_shred_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  efs2_diag_shred_req_type *request;
  efs2_diag_shred_rsp_type *response;
  const char *request_path;
  uint16 rsp_len = sizeof (efs2_diag_shred_rsp_type);

  EFS_BAB_DEBUG_PRINTF ("_SHRED");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (efs2_diag_shred_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_SHRED);

  request = (efs2_diag_shred_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_shred_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_shred_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SHRED,
                          rsp_len);

  if (response)
  {
    /* Acknowledge back with the Sequence number */
    response->sequence_number = request->sequence_number;

    response->shred_result = efs_shred (request_path);
    if (response->shred_result < 0)
      response->diag_errno = fs_get_diag_errno (efs_errno);
    else
      response->diag_errno = 0;
    fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  }

  return response;
} /* END fs_diag_shred_handler */

VOID_PACKED_PTR
fs_diag_set_idle_dev_evt_dur_handler (
    VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_set_idle_dev_evt_dur_rsp_type *response;
  efs2_diag_set_idle_dev_evt_dur_req_type *request =
    (efs2_diag_set_idle_dev_evt_dur_req_type *) req_ptr;

  fsd_invalidate_memoization ();

  EFS_BAB_DEBUG_PRINTF ("_IDLE_DEVICE_EVENT_DURATION");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /*
   * Check for the length of packet
   */
  if (pkt_len != sizeof (*request))
  {
    EFS_BAB_DEBUG_PRINTF ("Unexpected packet length = %d, SIZEOF = %d",
                          pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  response = (efs2_diag_set_idle_dev_evt_dur_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SET_IDLE_DEV_EVT_DUR,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* call the hotplug_set_idle_device_event_dur api to set event_dur value. */
#ifdef FEATURE_FS_HOTPLUG
  hotplug_set_idle_device_event_duration (request->event_dur_minutes);
  response->diag_errno = 0;
#else
  response->diag_errno = fs_get_diag_errno (EPERM);
#endif

  return response;
} /* END fs_diag_set_idle_dev_evt_dur_handler. */

/*===========================================================================

FUNCTION FS_DIAG_HOTPLUG_DEVICE_INFO_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_HOTPLUG_DEVICE_INFO packet.
  This command is used to get the device info maintained by hotplug module
  referred by the given path string.
============================================================================*/
VOID_PACKED_PTR
fs_diag_hotplug_device_info_handler (VOID_PACKED_PTR req_ptr,
                                     uint16 pkt_len)
{
  efs2_diag_hotplug_device_info_req_type *request_ptr;
  efs2_diag_hotplug_device_info_rsp_type *response_ptr = NULL;
  const char *request_path;

  uint16 rsp_len = sizeof (efs2_diag_hotplug_device_info_rsp_type);

  EFS_BAB_DEBUG_PRINTF ("_HOTPLUG_DEVICE_INFO");

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED)
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                             sizeof (efs2_diag_hotplug_device_info_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request_ptr = (efs2_diag_hotplug_device_info_req_type *)req_ptr;
  request_path = LONG_STRING_CAST (request_ptr->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request_ptr, efs2_diag_hotplug_device_info_req_type,
                   path, pkt_len, 1);

  response_ptr = (efs2_diag_hotplug_device_info_rsp_type *)
                    diagpkt_subsys_alloc (FS_SUBSYS,
                                          EFS2_DIAG_HOTPLUG_DEVICE_INFO,
                                          rsp_len);

  if (response_ptr == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response_ptr->sequence_number = request_ptr->sequence_number;

#ifdef FEATURE_FS_HOTPLUG
  {
    struct hotplug_device *hdev;
    struct hotplug_device_info hdev_info;

    /* Set all the members of struct hdev_info with 0. */
    memset (&hdev_info, 0, sizeof (hdev_info));

    hdev = hotplug_open_device (request_path);
    if (hdev == NULL)
    {
      response_ptr->diag_errno = fs_get_diag_errno (ENODEV);
      return response_ptr;
    }

    /* call the hotplug api to populate the struct hotplug_device_info. */
    if (hotplug_dev_get_device_info (hdev, &hdev_info) == 0)
    {
      response_ptr->diag_errno = 0;
      /* populate the response_ptr struct from the
       * struct hotplug_device_info. */
      response_ptr->manufacturer_id = hdev_info.manufacturer_id;
      response_ptr->oem_id          = hdev_info.oem_id;
      response_ptr->prod_rev        = hdev_info.prod_rev;
      response_ptr->prod_serial_num = hdev_info.prod_serial_num;

      strlcpy ((char*) response_ptr->product_name,
                  (const char *) hdev_info.product_name,
                  sizeof (hdev_info.product_name));
      strlcpy ((char*) response_ptr->manufactured_date,
                  (const char *) hdev_info.manufactured_date,
                  sizeof (hdev_info.manufactured_date));
    }
    else
    {
      fs_diag_efs2_error_rsp (-1, request_ptr, pkt_len); /* unknown error */
    }

    hotplug_close_device (hdev);
  }
#else /* !FEATURE_FS_HOTPLUG */
  (void) request_path;
  response_ptr->diag_errno = fs_get_diag_errno (EPERM);
#endif

  return response_ptr;
} /* END fs_diag_hotplug_device_info_handler */

/****************************************************************************
  FUNCTION        FS_DIAG_SYNC_NO_WAIT_HANDLER

  DESCRIPTION     Handles the EFS2_DIAG_SYNC_NO_WAIT command.
                  This command is used to initiate request for synchronization
                  of the filesytem to the backing store without blocking the
                  caller unlike efs_sync which blocks the caller.
******************************************************************************/
VOID_PACKED_PTR
fs_diag_sync_no_wait_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_sync_no_wait_req_type *request;
  efs2_diag_sync_no_wait_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_sync_no_wait_rsp_type);
  int sync_result;
  fs_sync_token_t sync_token;
  enum fs_sync_result sync_status;
  const char *request_path;

  /* Check security, since this is a secure function */
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                                 sizeof (efs2_diag_sync_no_wait_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_SYNC_NO_WAIT);

  request = (efs2_diag_sync_no_wait_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_sync_no_wait_req_type, path, pkt_len, 1);
  response = (efs2_diag_sync_no_wait_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS, EFS2_DIAG_SYNC_NO_WAIT, rsp_len);

  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  sync_result = efs_sync_no_wait (request_path, &sync_token);

  if (sync_result == -1)
  {
    response->diag_errno = fs_get_diag_errno (efs_errno);
  }
  else
  {
    sync_result = efs_sync_get_status (request_path, sync_token, &sync_status);
    if ((sync_result == 0) &&
        (sync_status == FS_SYNC_COMPLETE))
    {
      response->diag_errno = fs_get_diag_errno (ENOTHINGTOSYNC);
      response->sync_token = sync_token;
    }
    else
    {
      response->diag_errno = 0;
      response->sync_token = sync_token;
    }
  }

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return (response);

} /* End of fs_diag_sync_no_wait_handler */


/******************************************************************************
  FUNCTION        FS_DIAG_SYNC_GET_STATUS_HANDLER

  DESCRIPTION     Handles the EFS2_DIAG_SYNC_GET_STATUS command.
                  The efs_sync_no_wait API returns a token number for a sync
                  request and the caller can qury on the status of an issues
                  sync using the token number returned.
******************************************************************************/
VOID_PACKED_PTR
fs_diag_sync_get_status_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_sync_get_status_req_type *request;
  efs2_diag_sync_get_status_rsp_type *response;
  const char *request_path;
  int sync_status_query_result;
  enum fs_sync_result sync_result;
  uint16 rsp_len = sizeof (efs2_diag_sync_get_status_rsp_type);

  /* Check security, since this is a secure function */
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                                 sizeof (efs2_diag_sync_get_status_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_SYNC_GET_STATUS);

  request = (efs2_diag_sync_get_status_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_sync_get_status_req_type, path, pkt_len,
                   1);
  response = (efs2_diag_sync_get_status_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS, EFS2_DIAG_SYNC_GET_STATUS, rsp_len);

  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  sync_status_query_result = efs_sync_get_status (request_path,
                                           request->sync_token, &sync_result);
  if (sync_status_query_result == -1)
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
  {
    response->diag_errno = 0;
    response->sync_status = (uint8) sync_result;
  }

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return (response);

} /* End of fs_diag_sync_get_status_handler */

/******************************************************************************
  FUNCTION        FS_DIAG_TRUNCATE64_HANDLER

  DESCRIPTION
      Handles the EFS2_DIAG_TRUNCATE64 packet.
      TRUNCATE64 is the diagnostic packet for truncating a file with a
      filename and a specified size.
      Main difference between packet TRUNCATE and TRUNCATE64 is that with
      TRUNCATE64 packet file can also be truncated for the length greater
      then 2GB.
      The upper limit for truncated length is (4GB -1) bytes.
******************************************************************************/
VOID_PACKED_PTR
fs_diag_truncate64_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_truncate64_req_type *request;
  efs2_diag_truncate64_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_truncate64_rsp_type);
  const char *request_path;
  fs_off64_t length;

  EFS_BAB_DEBUG_PRINTF ("_TRUNCATE64");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                                 sizeof (efs2_diag_truncate64_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_TRUNCATE64);

  request = (efs2_diag_truncate64_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_truncate64_req_type,
                   path, pkt_len, 1);

  /*Path access check*/
  FS_DIAG_VALIDATE_PATH_ACCESS (TRUNCATE64, request_path);

  response = (efs2_diag_truncate64_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_TRUNCATE64,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* Prepare 64-bit value from the MSB and LSB from the request packet. */
  length = request->length_msb;
  length <<= 32;
  length |= request->length_lsb;

  /* Good to truncate the path. */
  if (efs_truncate64 (request_path, length) != 0)
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_truncate64_handler */

/******************************************************************************
  FUNCTION        FS_DIAG_FTRUNCATE64_HANDLER

  DESCRIPTION
      Handles the EFS2_DIAG_FTRUNCATE64 packet.
      TRUNCATE64 is the diagnostic packet for truncating a file with a
      file descriptor and a specified size.
      Main difference between packet FTRUNCATE and FTRUNCATE64 is that with
      FTRUNCATE64 packet file can also be truncated for the length greater
      then 2GB.
      The upper limit for truncated length is (4GB -1) bytes.
******************************************************************************/
VOID_PACKED_PTR
fs_diag_ftruncate64_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_ftruncate64_req_type *request;
  efs2_diag_ftruncate64_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_ftruncate64_rsp_type);
  fs_off64_t length;
  desc_entry *slot;

  EFS_BAB_DEBUG_PRINTF ("_FTRUNCATE64");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Check for exact length of packet */
  if (pkt_len != sizeof (efs2_diag_ftruncate64_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_FTRUNCATE64);

  request = (efs2_diag_ftruncate64_req_type *) req_ptr;

  response = (efs2_diag_ftruncate64_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FTRUNCATE64,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* Validate the requested descriptor. */
  if (request->fildes < 0 || request->fildes >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fildes].fd < 0)
  {
    response->diag_errno = fs_get_diag_errno (EBADF);
    goto ftruncate_end;
  }

  slot = &fsd_desc_table[request->fildes];

  /* Prepare 64-bit length from the MSB and LSB from the request packet. */
  length = request->length_msb;
  length <<= 32;
  length |= request->length_lsb;

  /* Good to ftruncate the path. */
  if (efs_ftruncate64 (slot->fd, length) != 0)
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

ftruncate_end:
  return response;
} /* END fs_diag_ftruncate64_handler */


/******************************************************************************
  FUNCTION        FS_DIAG_LSEEK64_HANDLER

  DESCRIPTION
      Handles the EFS2_DIAG_LSEEK64 packet.
      LSEEK64 is the diagnostic packet for seeking to the requested offset for
      the given file descriptor and whence.
      With LSEEK64 returning file offset can be changed beyond 2GB offset.
      The upper limit of offset for LSEEK64 is (4GB -1) bytes.
******************************************************************************/
VOID_PACKED_PTR
fs_diag_lseek64_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_lseek64_req_type *request;
  efs2_diag_lseek64_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_lseek64_rsp_type);
  desc_entry *slot;
  fs_off64_t ret_offset, offset;
  uint32 ret_offset_msb, ret_offset_lsb;

  EFS_BAB_DEBUG_PRINTF ("_LSEEK64");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /* Check for exact length of packet */
  if (pkt_len != sizeof (efs2_diag_lseek64_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_LSEEK64);

  request = (efs2_diag_lseek64_req_type *) req_ptr;

  response = (efs2_diag_lseek64_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_LSEEK64,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* Validate the requested descriptor. */
  if (request->fildes < 0 || request->fildes >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fildes].fd < 0)
  {
    response->diag_errno = fs_get_diag_errno (EBADF);
    goto lseek_end;
  }

  /* Prepare 64-bit offset from the MSB and LSB from the request packet. */
  offset = request->offset_msb;
  offset <<= 32;
  offset |= request->offset_lsb;

  slot = &fsd_desc_table[request->fildes];
  /* Good to lseek for given desc with requested offset. */
  ret_offset = efs_lseek64 (slot->fd, offset, request->whence);

  if (ret_offset == -1)
    response->diag_errno = fs_get_diag_errno (efs_errno);
  else
    response->diag_errno = 0;

  ret_offset_msb = (uint32) (ret_offset >> 32);
  ret_offset_lsb = (uint32) (ret_offset);

  response->ret_offset_msb = ret_offset_msb;
  response->ret_offset_lsb = ret_offset_lsb;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

lseek_end:
  return response;
} /* END fs_diag_lseek64_handler */

VOID_PACKED_PTR
fs_diag_make_golden_copy_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_make_golden_copy_req_type *request;
  efs2_diag_make_golden_copy_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_make_golden_copy_rsp_type);
  const char *request_path;
  int result;

  EFS_BAB_DEBUG_PRINTF ("_MAKE_GOLDEN_COPY");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
                                 sizeof (efs2_diag_make_golden_copy_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_MAKE_GOLDEN_COPY);

  request = (efs2_diag_make_golden_copy_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_make_golden_copy_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_make_golden_copy_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_MAKE_GOLDEN_COPY,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;
  /* By default return error. */
  response->diag_errno = fs_get_diag_errno (EPERM);

  if (fs_rmts_can_create_golden_copy() == 1)
  {
    result = efs_make_golden_copy (request_path);
    if (result == 0)
    {
      response->diag_errno = 0;
    }
    else
    {
      response->diag_errno = fs_get_diag_errno (efs_errno);
      /* Since we failed to make the golden copy, erase the item file to
       * allow future packets to create a golden copy. */
      result = efs_unlink (MADE_GOLDEN_COPY_PATH);
    }
  }

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return (response);
}/* End of fs_diag_make_golden_copy_handler */



/**************************************************************************//**
 * @par Name:
 *   fs_diag_filesystem_image_open -
 *           Handles the EFS2_DIAG_FILESYSTEM_IMAGE_OPEN packet.
 *
 * @par Description:
 *  This function prepare for the dump of the File System image in the
 *  format requested.
 *
 * @param req_ptr
 *  Received diag packet, should match the
 *  efs2_diag_filesystem_image_open_req_type.
 *
 * @param pkt_len
 *  How many bytes are present in above packet.
 *
 * @return
 *  Returns NULL if error or else valid response packet.
 *
******************************************************************************/
VOID_PACKED_PTR
fs_diag_filesystem_image_open (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_filesystem_image_open_req_type *req_pkt;
  efs2_diag_filesystem_image_open_rsp_type *rsp_pkt;
  enum fs_filesystem_image_type fs_image_type;
  uint16 rsp_len;
  int result;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED)
  {
    return diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len);
  }

  if (req_ptr == NULL)
  {
    return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

  /* Check for exact length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len,
        sizeof (efs2_diag_filesystem_image_open_req_type)) )
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr,pkt_len,EFS2_DIAG_FILESYSTEM_IMAGE_OPEN);

  req_pkt = (efs2_diag_filesystem_image_open_req_type *)req_ptr;
  FSD_CHECK_NULLS (req_pkt, efs2_diag_filesystem_image_open_req_type,
                   path, pkt_len, 1);

  rsp_len = sizeof (efs2_diag_filesystem_image_open_rsp_type);
  rsp_pkt = (efs2_diag_filesystem_image_open_rsp_type *)
              diagpkt_subsys_alloc (FS_SUBSYS,
                                    EFS2_DIAG_FILESYSTEM_IMAGE_OPEN, rsp_len);
  if (rsp_pkt == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  rsp_pkt->sequence_number = req_pkt->sequence_number;
  rsp_pkt->image_handle = 0;
  rsp_pkt->diag_errno = 0;

  fs_image_type = (enum fs_filesystem_image_type)req_pkt->image_type;
  result = efs_filesystem_image_open (req_pkt->path, fs_image_type);
  rsp_pkt->image_handle = result;
  if (result == -1)
    rsp_pkt->diag_errno = fs_get_diag_errno (efs_errno);

  fsd_memoize_packet (req_ptr, pkt_len, rsp_pkt, rsp_len);

  return rsp_pkt;
}



/**************************************************************************//**
 * @par Name:
 *   fs_diag_filesystem_image_read -
 *           Handles the EFS2_DIAG_FILESYSTEM_IMAGE_READ packet.
 *
 * @par Description:
 *  This function is used to read the next page of the file system image
 *  in the requested format with the OPEN diag packet.
 *
 * @param req_ptr
 *  Received diag packet, should match the
 *  efs2_diag_filesystem_image_read_req_type.
 *
 * @param pkt_len
 *  How many bytes are present in above packet.
 *
 * @return
 *  Returns NULL if error or else valid response packet.
 *
******************************************************************************/
VOID_PACKED_PTR
fs_diag_filesystem_image_read (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_filesystem_image_read_req_type *req_pkt;
  efs2_diag_filesystem_image_read_rsp_type *rsp_pkt;
  uint16 rsp_len;
  int result;
  int image_handle;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED)
  {
    return diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len);
  }

  if (req_ptr == NULL)
  {
    return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

  /* Check for exact length of packet */
  if (pkt_len != sizeof (efs2_diag_filesystem_image_read_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr,pkt_len,EFS2_DIAG_FILESYSTEM_IMAGE_READ);

  req_pkt = (efs2_diag_filesystem_image_read_req_type *)req_ptr;

  rsp_len = sizeof (efs2_diag_filesystem_image_read_rsp_type);
  rsp_len += (512-1); /* One page payload size */

  rsp_pkt = (efs2_diag_filesystem_image_read_rsp_type *)
              diagpkt_subsys_alloc (FS_SUBSYS, EFS2_DIAG_FILESYSTEM_IMAGE_READ,
                                    rsp_len);
  if (rsp_pkt == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  rsp_pkt->sequence_number = req_pkt->sequence_number;
  rsp_pkt->diag_errno = 0;

  image_handle = req_pkt->image_handle;
  result = efs_filesystem_image_read (image_handle, rsp_pkt->data, 512);
  rsp_pkt->bytes_read = result;
  if (result < 0)
    rsp_pkt->diag_errno = fs_get_diag_errno (efs_errno);

  fsd_memoize_packet (req_ptr, pkt_len, rsp_pkt, rsp_len);

  return rsp_pkt;
}



/**************************************************************************//**
 * @par Name:
 *   fs_diag_filesystem_image_close -
 *           Handles the EFS2_DIAG_FILESYSTEM_IMAGE_CLOSE packet.
 *
 * @par Description:
 *  This function is used to indicate the file system to end the output of the
 *  file system image reading process.
 *
 * @param req_ptr
 *  Received diag packet, should match the
 *  efs2_diag_filesystem_image_close_req_type.
 *
 * @param pkt_len
 *  How many bytes are present in above packet.
 *
 * @return
 *  Returns NULL if error or else valid response packet.
 *
******************************************************************************/
VOID_PACKED_PTR
fs_diag_filesystem_image_close (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_filesystem_image_close_req_type *req_pkt;
  efs2_diag_filesystem_image_close_rsp_type *rsp_pkt;
  uint16 rsp_len;
  int result;
  int image_handle;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED)
  {
    return diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len);
  }

  if (req_ptr == NULL)
  {
    return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

  /* Check for exact length of packet */
  if (pkt_len != sizeof (efs2_diag_filesystem_image_close_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr,pkt_len,EFS2_DIAG_FILESYSTEM_IMAGE_CLOSE);

  req_pkt = (efs2_diag_filesystem_image_close_req_type *)req_ptr;

  rsp_len = sizeof (efs2_diag_filesystem_image_close_rsp_type);
  rsp_pkt = (efs2_diag_filesystem_image_close_rsp_type *)
              diagpkt_subsys_alloc (FS_SUBSYS,
                                    EFS2_DIAG_FILESYSTEM_IMAGE_CLOSE, rsp_len);
  if (rsp_pkt == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  rsp_pkt->sequence_number = req_pkt->sequence_number;
  rsp_pkt->diag_errno = 0;

  image_handle = req_pkt->image_handle;

  result = efs_filesystem_image_close (image_handle);
  if (result < 0)
    rsp_pkt->diag_errno = fs_get_diag_errno (efs_errno);

  fsd_memoize_packet (req_ptr, pkt_len, rsp_pkt, rsp_len);

  return rsp_pkt;
}



/****************************************************************************
 *
 *                       EFS1 LEGACY DIAG PACKETS OPS
 *
 * The diag ops below are only for legacy systems. When developing an
 * application please use the packet/handlers declared above. The use of
 * these packets/ops is not recommended.
 *
 * The diag ops below were originally developed when we used EFS1. Many of the
 * diag ops called one or more EFS1 APIs. The diag ops below were updated to
 * use EFS2 APIs and show the same behavior as before. Most of operations do
 * the same thing: check packet length, check file access, do requested
 * operation, build and return response packet.
 *
 * When adding a new diag packet please consider adding them to fs_diag_tbl
 * and adding them before this title. We should not expand the packet handlers
 * declared below.
 *
 ***************************************************************************/

/* Stores the file descriptor between read/write  diag packets. */
static int fs_diag_fd = -1;

/* Stores what is the next sequence number when reading/writing. */
static uint8 fs_diag_next_seq_num = 0;

/* State variable. 0 when reading, 1 when writing. We do some extra operations
 * when cleaning up when writing. */
static uint8 fs_diag_writing = 0;

/* Stores current file position, it is used to know how much of the file
 * we have read/written. */
static fs_size_t fs_diag_filepos;

/* Holds the file size we are currently serving. */
static fs_size_t fs_diag_filesize;

/* Temporary buffer to build the path for calling efs_stat. */
static char fs_diag_pathbuf[FS_DIAG_MAX_FILENAME_SIZE];

/* Stores what is the file we are currently writing */
static char fs_diag_writepath[FS_DIAG_MAX_FILENAME_SIZE];

/* Stores the directory we are iterating in. */
static char fs_diag_dirpath[FS_DIAG_MAX_FILENAME_SIZE];

/* EFS2 returns the same error code for file or directory, EFS1 does not.
 * This is used to differentiate between them when needed. */
#define FS_DIAG_TYPE_DIR   'd'
#define FS_DIAG_TYPE_FILE  'f'


/*
 * Maps EFS2 error codes to EFS1 error codes
 *
 * Params:
 *   filetype   See the definitions above for FS_DIAG_TYPE_*.
 *   efs2_errno errno returned by efs in efs_errno variable.
 *
 */
static uint8
fs_efs2_to_efs1_err (int efs2_errno, char filetype)
{
  enum efs1_error efs1_errno = EFS1_FAIL_S;

  if (filetype == FS_DIAG_TYPE_FILE)
  {
    switch (efs2_errno)
    {
      case EPERM:
        efs1_errno = EFS1_PARAMETER_ERROR_S;
        break;

      case ENOENT:
      case EACCES:
      case EISDIR:
        efs1_errno = EFS1_NONEXISTENT_FILE_S;
        break;

      case EEXIST:
        efs1_errno = EFS1_FILE_ALREADY_EXISTS_S;
        break;

      case EBUSY:
        efs1_errno = EFS1_BUSY_S;
        break;

      case EBADF:
        efs1_errno = EFS1_FILE_NOT_OPEN_S;
        break;

      case ETXTBSY:
        efs1_errno = EFS1_FILE_OPEN_S;
        break;

      case EINVAL:
        efs1_errno = EFS1_ILLEGAL_OPERATION_S;
        break;

      case ENAMETOOLONG:
        efs1_errno = EFS1_BAD_FILE_NAME_S;
        break;

      case EEOF:
        efs1_errno = EFS1_EOF_S;
        break;

      case ENOSPC:
        efs1_errno = EFS1_SPACE_EXHAUSTED_S;
        break;

      case EMFILE:
        efs1_errno = EFS1_OPEN_TABLE_FULL_S;
        break;

      case ENOTDIR:
        efs1_errno = EFS1_NONEXISTENT_DIR_S;
        break;

      default:
        efs1_errno = EFS1_FAIL_S;
        break;
    }
  }
  else if (filetype == FS_DIAG_TYPE_DIR)
  {
    switch (efs2_errno)
    {
      case EBUSY:
        efs1_errno = EFS1_BUSY_S;
        break;

      case EEXIST:
        efs1_errno = EFS1_DIR_ALREADY_EXISTS_S;
        break;

      case ENOENT:
      case ENOTDIR:
      case EACCES:
        efs1_errno = EFS1_NONEXISTENT_DIR_S;
        break;

      case ENOTEMPTY:
        efs1_errno = EFS1_DIR_NOT_EMPTY_S;
        break;

      case EINVAL:
        efs1_errno = EFS1_ILLEGAL_OPERATION_S;
        break;

      case EPERM:
        efs1_errno = EFS1_PARAMETER_ERROR_S;
        break;

      case ENOSPC:
        efs1_errno = EFS1_DIR_SPACE_EXHAUSTED_S;
        break;


      case EISDIR:
        efs1_errno = EFS1_NONEXISTENT_FILE_S;
        break;

      default:
        efs1_errno = EFS1_FAIL_S;
        break;
    }
  }
  else
    FS_ERR_FATAL ("Unknown file type %c", filetype, 0, 0);

  return (uint8)efs1_errno;
}


/* Creates a directory.
 *
 * mkdir, rmdir and remove file are basically the same (request and response
 * are exactly the same) they just perform a different operation.
 */
PACKED void *
fs_diag_mkdir (PACKED void *req_pkt, uint16 pkt_len)
{
  fs_diag_mkdir_rsp_type *rsp_pkt;
  fs_diag_mkdir_req_type *mkdir_pkt = (fs_diag_mkdir_req_type *) req_pkt;

  fsd_invalidate_memoization ();

  /* Check for valid packet length */
  if (pkt_len != (mkdir_pkt->filename.len + sizeof (fs_diag_mkdir_req_type)
                  - sizeof (mkdir_pkt->filename.file)))
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

  /* Check parameters. */
  if (mkdir_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
      mkdir_pkt->filename.len != (strlen (mkdir_pkt->filename.file) + 1))
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  if (fs_diag_access_check ((const char *)mkdir_pkt->filename.file,
                            DIAG_FS_CHECK_MKDIR) == FALSE)
    return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

  rsp_pkt = (fs_diag_mkdir_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_mkdir_rsp_type));

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_mkdir.");
    return ((PACKED void *) rsp_pkt);
  }

  if (efs_mkdir ((const char *)mkdir_pkt->filename.file,
                 S_IREAD | S_IWRITE | S_IEXEC))
    rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno, FS_DIAG_TYPE_DIR);
  else
    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;

  rsp_pkt->header.op = FS_DIAG_MKDIR;
  rsp_pkt->header.cmd_code = mkdir_pkt->header.cmd_code;

  return ((PACKED void *) rsp_pkt);

} /* END fs_diag_mkdir */


/* Removes a directory.
 *
 * mkdir, rmdir and remove file are basically the same (request and response
 * are exactly the same) they just perform a different operation.
 */
PACKED void *
fs_diag_rmdir (PACKED void *req_pkt, uint16 pkt_len)
{
  fs_diag_rmdir_rsp_type *rsp_pkt;
  fs_diag_rmdir_req_type *rmdir_pkt = (fs_diag_rmdir_req_type *) req_pkt;

  fsd_invalidate_memoization ();

  /* Check for valid packet length */
  if (pkt_len != (rmdir_pkt->filename.len + sizeof (fs_diag_rmdir_req_type)
                  - sizeof (rmdir_pkt->filename.file)))
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

  /* Check parameters. */
  if (rmdir_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
      rmdir_pkt->filename.len != (strlen (rmdir_pkt->filename.file) + 1))
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  if (fs_diag_access_check ((const char *)rmdir_pkt->filename.file,
                            DIAG_FS_CHECK_RMDIR) == FALSE)
    return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

  rsp_pkt = (fs_diag_rmdir_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_rmdir_rsp_type));

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_rmdir.");
    return ((PACKED void *) rsp_pkt);
  }

  if (efs_rmdir ((const char *) rmdir_pkt->filename.file))
    rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno, FS_DIAG_TYPE_DIR);
  else
    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;

  rsp_pkt->header.op = FS_DIAG_RMDIR;
  rsp_pkt->header.cmd_code = rmdir_pkt->header.cmd_code;

  return ((PACKED void *) rsp_pkt);

}/* END fs_diag_rmdir */

PACKED void *
fs_diag_disp (PACKED void *req_pkt, uint16 pkt_len)
{
  EFSDIR *dirp;
  struct fs_stat sbuf;
  struct fs_dirent *dirent;
  uint32 fmode;
  char *data, *entryname;
  uint16 remain;
  fs_diag_disp_rsp_type *rsp_pkt;
  fs_diag_disp_req_type *disp_pkt = (fs_diag_disp_req_type *) req_pkt;
  /* Indicates what is the exit status.
   * no error, EFS1_OKAY_S
   * erro EFS1_NONEXISTENT_DIR_S on specific cases
   * everything else, return error in efs_errno  */
  enum efs1_error status = EFS1_FAIL_S;
  uint16 num_items = 0;

  fsd_invalidate_memoization ();

  entryname = disp_pkt->filename.file;

  /* Check for valid packet length */
  if (pkt_len != (disp_pkt->filename.len + sizeof (fs_diag_disp_req_type)
                  - sizeof (disp_pkt->filename.file)))
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

  /* Check parameters. */
  if (disp_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
      disp_pkt->filename.len != (strlen (disp_pkt->filename.file) + 1))
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  if (disp_pkt->header.op == FS_DIAG_DISP_DIRS &&
      fs_diag_access_check (entryname, DIAG_FS_CHECK_DISP_DIRS) == FALSE)
    return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

  if (disp_pkt->header.op == FS_DIAG_DISP_FILES &&
      fs_diag_access_check (entryname, DIAG_FS_CHECK_DISP_FILES) == FALSE)
    return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

  rsp_pkt = (fs_diag_disp_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_disp_rsp_type));

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_disp.");
    return ((PACKED void *) rsp_pkt);
  }

  rsp_pkt->header.op = disp_pkt->header.op;
  rsp_pkt->header.cmd_code = disp_pkt->header.cmd_code;
  data = rsp_pkt->block.data;

  /* Subtract 1 to account for the extra NULL characters at the end of the
   * block */
  remain = FS_DIAG_MAX_BLOCK_SIZE_BYTES - 1;
  *data = 0;

  dirp = efs_opendir (entryname);
  if (dirp == NULL)
    goto disp_end;

  /* Ignore the first two entries "." and ".." */
  dirent = efs_readdir (dirp);
  if (dirent == NULL)
    goto disp_end;

  dirent = efs_readdir (dirp);
  if (dirent == NULL)
    goto disp_end;


  while ((dirent = efs_readdir (dirp)) != NULL)
  {
    char *pathbuf = fs_diag_pathbuf;
    uint32 len;

    len = sizeof (fs_diag_pathbuf);
    /* Copy to temporary buffer, if path is too long we quit. */
    if (strlcpy (pathbuf, entryname, len) >= len ||
        strlcat (pathbuf, "/", len)       >= len ||
        strlcat (pathbuf, dirent->d_name, len) >= len)
    {
      status = EFS1_NONEXISTENT_DIR_S;;
      goto disp_end;
    }

    /* Check if mode is returned, otherwise call stat to get mode. */
    if (FS_MASK_CHECK (dirent->d_stats_present, FS_DIRENT_HAS_ST_MODE))
      fmode = dirent->d_stat.st_mode;
    else
    {
      if (efs_stat (pathbuf, &sbuf))
      {
        status = EFS1_NONEXISTENT_DIR_S;
        goto disp_end;
      }
      fmode = sbuf.st_mode;
    }

    /* Take into account the NULL character */
    len = strlen (pathbuf) + 1;

    /* If we don't have space for the next entry, quit */
    if (remain < len)
    {
      status = EFS1_NONEXISTENT_DIR_S;
      goto disp_end;
    }

    if ((S_ISDIR(fmode) && disp_pkt->header.op == FS_DIAG_DISP_DIRS) ||
        (S_ISREG (fmode) && disp_pkt->header.op == FS_DIAG_DISP_FILES))
    {
      /* We already made sure that pathbuf will fit in data block. */
      (void) strlcpy (data, pathbuf, remain);
      data += len;
      remain -= len;
      num_items++;
    }
  }
  /* Indicate we list directories with no errors. */
  status = EFS1_OKAY_S;

  /* Special case when we don't return any files/dir. We need to have two
   * NULL at the end, we already set the first one, move pointer to set the
   * next one.  */
  if (num_items == 0)
  {
    data++;
    remain--;
  }

disp_end:

  if (dirp != NULL)
    (void) efs_closedir (dirp);

  /* Write the extra NULL character at the end of the block */
  *(data++) = 0;
  rsp_pkt->block.len = data - &rsp_pkt->block.data[0];

  rsp_pkt->num_items = num_items;

  if (status == EFS1_OKAY_S)
  {
    diagpkt_shorten (rsp_pkt, sizeof (fs_diag_disp_rsp_type) - remain);
    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;
  }
  else
  {
    diagpkt_shorten (rsp_pkt, sizeof (struct fs_diag_rsp_header));

    if (status == EFS1_NONEXISTENT_DIR_S)
      rsp_pkt->header.status = (uint8) EFS1_NONEXISTENT_DIR_S;
    else if (disp_pkt->header.op == FS_DIAG_DISP_DIRS)
      rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno,
                                                     FS_DIAG_TYPE_DIR);
    else
      rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno,
                                                     FS_DIAG_TYPE_FILE);
  }

  return ((PACKED void *) rsp_pkt);
}/* END fs_diag_disp */

/* When an error occurs on reading a file this will return the error packet for
 * diag. It will also reset the global file descriptor. It does not close the
 * file. This is because an error may occur before the file is opened. */
static PACKED void *
fs_diag_read_error_packet (void *rsp_pkt)
{
  struct fs_diag_rsp_header *rsp = (struct fs_diag_rsp_header *) rsp_pkt;
  fs_diag_fd = -1;
  rsp->status = fs_efs2_to_efs1_err (efs_errno, FS_DIAG_TYPE_FILE);
  diagpkt_shorten (rsp, sizeof (struct fs_diag_rsp_header));
  return ((PACKED void *) rsp);
}

/* Cleans up all global counters used in fs_diag_read and fs_diag_write,
 * closes any opened files and removes if write didn't finish successfully. */
static void
fs_diag_cleanup_rw_state (void)
{
  if (fs_diag_fd >= 0)
  {
    (void) efs_close (fs_diag_fd);
    if (fs_diag_writing == 1)
    {
      (void) efs_unlink (fs_diag_writepath);
      memset (fs_diag_writepath, 0, sizeof (fs_diag_writepath));
    }
  }

  fs_diag_next_seq_num = 0;
  fs_diag_fd = -1;
  fs_diag_filepos = 0;
  fs_diag_filesize = 0;
  fs_diag_writing = 0;
}

/* Process a read packet from Diag.
 *
 * The first packet is a special packet, it is the only one with sequence
 * number of zero. When this packet is received we have to return some
 * extra data (total file length) and open the file. After this is done
 * the behavior is the same, read data from the file and send it back
 * on the packet block field. */
PACKED void *
fs_diag_read_file (PACKED void *req_pkt, uint16 pkt_len)
{
  int read_count;
  fs_diag_read_req_type *read_pkt = (fs_diag_read_req_type *) req_pkt;
  fs_diag_read_next_rsp_type *rsp_pkt;
  uint16 rsp_len;
  /* Block is a pointer to the data block field of each packet. */
  struct fs_diag_data_block *block;

  /* Check if packet was memoized and needs to be retransmitted. */
  if (pkt_len == fsd_last_packet_len &&
      memcmp ((void *)req_pkt, fsd_last_packet, fsd_last_packet_len) == 0)
  {
    PACKED void *rsp = diagpkt_alloc (DIAG_FS_OP_F, fsd_last_response_len);

    if (rsp != NULL)
      fs_memscpy ((void *)rsp, pkt_len, fsd_last_response,
                  fsd_last_response_len);

    return rsp;
  }

  fsd_invalidate_memoization ();

  /* It is the first packet in the sequence. */
  if (read_pkt->seq_num == 0)
  {
    fs_diag_read_first_rsp_type *first_pkt;
    struct fs_stat sbuf;

    FSD_CHECK_NULLS (read_pkt, fs_diag_read_req_type,
      filename.file, pkt_len, 1);
    /* Check packet length. */
    if (pkt_len != (read_pkt->filename.len + sizeof (fs_diag_read_req_type)
                    - sizeof (read_pkt->filename.file)))
      return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

    /* Check parameters. */
    if (read_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
        read_pkt->filename.len != (strlen (read_pkt->filename.file) + 1))
      return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

    if (fs_diag_access_check (read_pkt->filename.file, DIAG_FS_CHECK_READ)
        == FALSE)
      return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

    first_pkt = (fs_diag_read_first_rsp_type *)
      diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_read_first_rsp_type));

    if (first_pkt == NULL)
    {
      FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_read.");
      return ((PACKED void *) first_pkt);
    }

    first_pkt->header.cmd_code = read_pkt->header.cmd_code;
    first_pkt->header.op = read_pkt->header.op;

    if (efs_stat (read_pkt->filename.file, &sbuf))
      /* Failed to stat file. */
      return (fs_diag_read_error_packet ((void *)first_pkt));

    fs_diag_cleanup_rw_state ();

    /* Got file size, put it in response packet. */
    first_pkt->file_size = fs_diag_filesize = sbuf.st_size;;

    fs_diag_fd = efs_open (read_pkt->filename.file, O_RDONLY);

    if (fs_diag_fd < 0)
    {
      fs_diag_cleanup_rw_state ();
      /* failed to open the file, return error. */
      return (fs_diag_read_error_packet ((void *)first_pkt));
    }

    block = &first_pkt->block;
    rsp_pkt = (fs_diag_read_next_rsp_type *) ((PACKED void *)first_pkt);

  }
  /* Check if we got the next sequence number. */
  else if (fs_diag_next_seq_num == read_pkt->seq_num)
  {
    /* Check packet length. Packets with seq_num!=0 do not have filename. */
    if (pkt_len != sizeof (fs_diag_read_req_type)
                   - sizeof (read_pkt->filename))
    {
      fs_diag_cleanup_rw_state ();
      return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
    }

    rsp_pkt = (fs_diag_read_next_rsp_type *)
      diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_read_first_rsp_type));

    if (rsp_pkt == NULL)
    {
      FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_read.");
      return ((PACKED void *) rsp_pkt);
    }

    block = &rsp_pkt->block;

  }
  /* Sequence Number provided was invalid. Return error */
  else
  {
    fs_diag_cleanup_rw_state ();

    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  /* All the packets need to read some data from the file and fill the
   * block field of the packet with it. */
  rsp_pkt->header.op = FS_DIAG_READ_FILE;
  rsp_pkt->header.cmd_code = read_pkt->header.cmd_code;
  rsp_pkt->seq_num = fs_diag_next_seq_num;

  read_count = efs_read (fs_diag_fd, (void *)block->data,
                         FS_DIAG_MAX_BLOCK_SIZE_BYTES);

  if (read_count < 0)
  {
    /* Failed to read the file, return error. */
    fs_diag_cleanup_rw_state ();

    return (fs_diag_read_error_packet ((void *)rsp_pkt));
  }

  fs_diag_filepos += read_count;
  block->len = read_count;

  /* Set next expected seq number and more field accordingly */
  if (fs_diag_filepos < fs_diag_filesize)
  {
    rsp_pkt->more = 1;
    ++fs_diag_next_seq_num;

    /* Handle seq num wrap around, if it reaches zero it should be set to 1. */
    if (fs_diag_next_seq_num == 0)
      ++fs_diag_next_seq_num;
  }
  else
  {
    rsp_pkt->more = 0;
    fs_diag_cleanup_rw_state ();
  }

  rsp_pkt->header.status = (uint8) EFS1_OKAY_S;

  /* Shorten the packet to include only the data we read and no more. */
  if (rsp_pkt->seq_num == 0)
    rsp_len = sizeof (fs_diag_read_first_rsp_type)
              - sizeof (rsp_pkt->block.data) + read_count;
  else
    rsp_len = sizeof (fs_diag_read_next_rsp_type)
              - sizeof (rsp_pkt->block.data) + read_count;

  diagpkt_shorten (rsp_pkt, rsp_len);
  fsd_memoize_packet (req_pkt, pkt_len, rsp_pkt, rsp_len);

  return ((PACKED void *) rsp_pkt);
}/* END fs_diag_read_file */


/* Process a write packet from diag.
 *
 * Similarly to the read operation, write has an special packet with sequence
 * number of 0. When this packet is received is interpreted as the first packet
 * and any previous read/write operations will be canceled.
 *
 * When the first packet is received we store the total file size file
 * name and open the file. After this is done the rest is the same for all
 * packets: Write the data in the block field into the file.
 *
 */
PACKED void *
fs_diag_write_file (PACKED void *req_pkt, uint16 pkt_len)
{
  fs_diag_write_next_req_type *wrt_pkt;
  fs_diag_write_rsp_type *rsp_pkt;
  struct fs_diag_data_block *block;
  uint16 written;
  uint16 rsp_len = sizeof (fs_diag_write_rsp_type);
  int result;

  /* Indicates if we successfully processed the packet, with no errors. */
  uint8 success = 0;

  /* Check if packet was memoized and needs to be retransmitted. */
  if (pkt_len == fsd_last_packet_len &&
      memcmp ((void *)req_pkt, fsd_last_packet, fsd_last_packet_len) == 0)
  {
    PACKED void *rsp = diagpkt_alloc (DIAG_FS_OP_F, fsd_last_response_len);

    if (rsp != NULL)
      fs_memscpy ((void *)rsp, pkt_len, fsd_last_response,
                  fsd_last_response_len);

    return rsp;
  }

  fsd_invalidate_memoization ();

  wrt_pkt = (fs_diag_write_next_req_type *) req_pkt;

  /* Check if it is the first packet in the sequence. */
  if (wrt_pkt->seq_num == 0)
  {
    int oflag;
    fs_diag_write_first_req_type *w_pkt;
    w_pkt = (fs_diag_write_first_req_type *) req_pkt;


    /* The first write packet has variable length filename and a variable
     * length data block. We need to set the pointer to block data. */
    block = (struct fs_diag_data_block *)
            ((&w_pkt->filename.file[0]) + w_pkt->filename.len);
    FSD_CHECK_NULLS (w_pkt, fs_diag_write_first_req_type,
      filename.file, pkt_len, 1);

    /* Check packet length. */
    if (pkt_len != (sizeof (fs_diag_write_first_req_type)/* Complete packet */
                    + w_pkt->filename.len - sizeof (w_pkt->filename.file)
                    + block->len + sizeof (block->len)))
      return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

    /* Check parameters. */
    if (w_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
        w_pkt->filename.len != (strlen (w_pkt->filename.file) + 1))
      return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

    if (fs_diag_access_check (w_pkt->filename.file, DIAG_FS_CHECK_WRITE)
        == FALSE)
      return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

    if (w_pkt->truncate == 1)
      oflag = O_WRONLY | O_CREAT | O_TRUNC;
    else
      oflag = O_WRONLY | O_CREAT | O_EXCL;

    fs_diag_cleanup_rw_state ();

    /* The packet has some attributes flags, attributes were completely
     * ignored when we moved to EFS2 */

    (void) strlcpy (fs_diag_writepath, w_pkt->filename.file,
                        FS_DIAG_MAX_FILENAME_SIZE);

    fs_diag_fd = efs_open (fs_diag_writepath, oflag);
    if (fs_diag_fd < 0)
      goto fs_diag_write_done;

    fs_diag_filesize = w_pkt->file_size;

    fs_diag_writing = 1;
  }
  /* Next Sequence number. */
  else if (wrt_pkt->seq_num == fs_diag_next_seq_num)
  {
    /* Check packet length. */
    if (pkt_len != sizeof (fs_diag_write_next_req_type) + wrt_pkt->block.len
                   - sizeof (wrt_pkt->block.data))
    {
      fs_diag_cleanup_rw_state ();
      return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
    }

    block = &wrt_pkt->block;
  }
  /* Not valid seq number */
  else
  {
    fs_diag_cleanup_rw_state ();
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  /* Now we need to write the block into the file. */
  if (block->len > FS_DIAG_MAX_BLOCK_SIZE_BYTES)
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  fs_diag_filepos += block->len;

  /* Check if we are attempting to write more than originally stated */
  if (fs_diag_filepos > fs_diag_filesize)
  {
    fs_diag_cleanup_rw_state ();
    goto fs_diag_write_done;
  }

  /* Write to file and increment next seq number. */
  written = 0;
  result = 0;

  /* Loop until all the data is written or error occurs */
  do
  {
    result = efs_write (fs_diag_fd, (void *)(block->data + written),
                        block->len - written);

    if (result < 0)
      break;

    written += result;
  } while (written < block->len);

  if (result < 0)
  {
    fs_diag_cleanup_rw_state ();
    goto fs_diag_write_done;
  }

  /* Check if this is the last packet. */
  if (wrt_pkt->more == 0)
  {
    fs_diag_writing = 0;
    fs_diag_cleanup_rw_state ();
  }
  else
  {
    ++fs_diag_next_seq_num;

    /* Handle seq num wrap around, if it reaches zero it should be set to 1. */
    if (fs_diag_next_seq_num == 0)
      ++fs_diag_next_seq_num;
  }

  success = 1;

fs_diag_write_done:
  rsp_pkt = (fs_diag_write_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, rsp_len);

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_write.");
    return ((PACKED void *) rsp_pkt);
  }

  rsp_pkt->header.cmd_code = wrt_pkt->header.cmd_code;
  rsp_pkt->header.op = FS_DIAG_WRITE_FILE;
  rsp_pkt->seq_num = wrt_pkt->seq_num;

  /* If we tried to write more than expected return error. This error comes
   * from the original implementation of fsdiag */
  if (fs_diag_filepos > fs_diag_filesize)
    rsp_pkt->header.status = (uint8) EFS1_ILLEGAL_OPERATION_S;
  else if (success == 1)
    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;
  else
    rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno,FS_DIAG_TYPE_FILE);

  fsd_memoize_packet (req_pkt, pkt_len, rsp_pkt, rsp_len);
  return ((PACKED void *) rsp_pkt);
}/* END fs_diag_write_file */

/* Removes a file.
 *
 * mkdir, rmdir and remove file are basically the same (request and response
 * are exactly the same) they just perform a different operation.
 */
PACKED void *
fs_diag_remove_file (PACKED void *req_pkt, uint16 pkt_len)
{
  fs_diag_rmfile_req_type *del_pkt = (fs_diag_rmfile_req_type *) req_pkt;
  fs_diag_rmfile_rsp_type *rsp_pkt;

  FSD_CHECK_NULLS (del_pkt, fs_diag_rmfile_req_type,
      filename.file, pkt_len, 1);
  /* Check packet length. */
  if (pkt_len != (sizeof (fs_diag_rmfile_req_type) + del_pkt->filename.len
                  - sizeof (del_pkt->filename.file)))
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

  /* Check parameters. */
  if (del_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
      del_pkt->filename.len != (strlen (del_pkt->filename.file) + 1))
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  if (fs_diag_access_check (del_pkt->filename.file, DIAG_FS_CHECK_DELETE)
      == FALSE)
    return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

  rsp_pkt = (fs_diag_rmfile_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_rmfile_rsp_type));

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_remove_file.");
    return ((PACKED void *) rsp_pkt);
  }

  fs_diag_cleanup_rw_state ();

  if (efs_unlink (del_pkt->filename.file))
    rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno,FS_DIAG_TYPE_FILE);
  else
    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;

  rsp_pkt->header.cmd_code = del_pkt->header.cmd_code;
  rsp_pkt->header.op = FS_DIAG_REMOVE_FILE;

  return ((PACKED void *) rsp_pkt);

}/* END fs_diag_remove_file */


/* Get file attributes.
 *
 * For the file attributes we just returned the default values. EFS1 file
 * attributes don't have meaning in EFS2.
 */
PACKED void *
fs_diag_get_attr (PACKED void *req_pkt, uint16 pkt_len)
{
  fs_diag_get_attr_req_type *get_pkt = (fs_diag_get_attr_req_type *)req_pkt;
  fs_diag_get_attr_rsp_type *rsp_pkt;
  struct fs_stat sbuf;

  fsd_invalidate_memoization ();
  FSD_CHECK_NULLS (get_pkt, fs_diag_get_attr_req_type,
      filename.file, pkt_len, 1);

  /* Check packet length. */
  if (pkt_len != (sizeof (fs_diag_get_attr_req_type) + get_pkt->filename.len
                  - sizeof (get_pkt->filename.file)))
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

  /* Check parameters. */
  if (get_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
      get_pkt->filename.len != (strlen (get_pkt->filename.file) + 1))
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  if (fs_diag_access_check (get_pkt->filename.file, DIAG_FS_CHECK_GET_ATTRIB)
      == FALSE)
    return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

  rsp_pkt = (fs_diag_get_attr_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_get_attr_rsp_type));

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_get_attr.");
    return ((PACKED void *) rsp_pkt);
  }

  rsp_pkt->header.cmd_code = get_pkt->header.cmd_code;
  rsp_pkt->header.op = FS_DIAG_GET_ATTR;

  if (efs_stat (get_pkt->filename.file, &sbuf))
  {
    /* Stat failed */
    rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno,FS_DIAG_TYPE_FILE);
    diagpkt_shorten (rsp_pkt, sizeof (struct fs_diag_rsp_header));
  }
  else
  {
    /* stat successful. */
    fs_time_t ctime;

    rsp_pkt->file_size = sbuf.st_size;

    /* Sometime back this diag packet was returning change time instead
     * of creation time. For compatibility reasons return ctime. Furthermore,
     * it was returning the time with respect to CDMA time instead of POSIX
     * time. We need to adjust POSIX time to CDMA time. */
    ctime = sbuf.st_ctime;
    if (ctime > CDMA_TO_FS_TIME_DELTA)
      ctime -= CDMA_TO_FS_TIME_DELTA;
    else
      ctime  = 0;

    rsp_pkt->crt_date = ctime;

    /* Return the default attributes from EFS1:
     * Attribute UNRESTRICTED:   0x00FF (Two Bytes)
     * buffering option PROHIBIT:  0x00 (Next byte)
     * Cleanup option CLOSE:       0x00 (Last two bytes) */
    rsp_pkt->attr = 0x00FF;
    rsp_pkt->buf_opt = 0x00;
    rsp_pkt->clean_opt = 0x00;

    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;
  }

  return ((PACKED void *) rsp_pkt);
}/* END fs_diag_get_attr */

/* Set file attributes (Fake).
 *
 * File attributes as defined in EFS1 are not used in EFS2. We just ignore all
 * the file attributes and return success. */
PACKED void *
fs_diag_set_attr (PACKED void *req_pkt, uint16 pkt_len)
{
  fs_diag_set_attr_req_type *set_pkt = (fs_diag_set_attr_req_type *)req_pkt;
  fs_diag_set_attr_rsp_type *rsp_pkt;

  fsd_invalidate_memoization ();

  FSD_CHECK_NULLS (set_pkt, fs_diag_set_attr_req_type,
      filename.file, pkt_len, 1);
  /* Check packet length. */
  if (pkt_len != (sizeof (fs_diag_set_attr_req_type) + set_pkt->filename.len
                  - sizeof (set_pkt->filename.file)))
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

  /* Check parameters. */
  if (set_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
      set_pkt->filename.len != (strlen (set_pkt->filename.file) + 1))
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  if (fs_diag_access_check (set_pkt->filename.file, DIAG_FS_CHECK_SET_ATTRIB)
      == FALSE)
    return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

  rsp_pkt = (fs_diag_set_attr_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_set_attr_rsp_type));

  if (rsp_pkt)
  {
    rsp_pkt->header.cmd_code = set_pkt->header.cmd_code;
    rsp_pkt->header.op = FS_DIAG_SET_ATTR;
    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;
  }
  else
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_set_attr.");
  }

  return ((PACKED void *) rsp_pkt);
}/* END fs_diag_set_attr */

/* Iterate files/directories
 *
 * With each packet received we send the next file/directory for the directory
 * we are iterating. When the first packet is received (sequence number 0)
 * we need to open the directory. The application will keep sending packets
 * to get the next file/directory until there are no more and we return
 * an error code of enum done.
 *
 */
PACKED void *
fs_diag_iter (PACKED void *req_pkt, uint16 pkt_len)
{
  static EFSDIR *dirp = NULL;
  struct fs_stat sbuf;
  struct fs_stat *sbufp;
  struct fs_dirent *dirent = NULL;
  uint32 fmode;
  char *pathbuf = fs_diag_pathbuf;
  char *dir_path = fs_diag_dirpath;
  uint8  finished = 0;
  uint8  rsp_len = 0;
  fs_diag_iter_req_type *iter_pkt = (fs_diag_iter_req_type *)req_pkt;
  fs_diag_iter_rsp_type *rsp_pkt;

  fsd_invalidate_memoization ();
  FSD_CHECK_NULLS (iter_pkt, fs_diag_iter_req_type,
    filename.file, pkt_len, 1);

  /* Check packet length. */
  if (pkt_len != (sizeof (fs_diag_iter_req_type) + iter_pkt->filename.len
                  - sizeof (iter_pkt->filename.file)))
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

  /* Check parameters. */
  if (iter_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
      iter_pkt->filename.len != (strlen (iter_pkt->filename.file) + 1))
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  /* Check if first packet, open enumeration */
  if (iter_pkt->seq_num == 0)
  {
    if (fs_diag_access_check (iter_pkt->filename.file, DIAG_FS_CHECK_ITERATE)
        == FALSE)
      return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

    /* If a previous directory was open, close it. */
    if (dirp != NULL)
      (void) efs_closedir (dirp);

    dirp = efs_opendir (iter_pkt->filename.file);
    if (dirp == NULL)
      goto fs_diag_iter_err;

    strlcpy (dir_path, iter_pkt->filename.file, sizeof(fs_diag_dirpath));

    /* The first two entries are . and .. we need to jump over them. */
    if (efs_readdir (dirp) == NULL)
      goto fs_diag_iter_err;

    if (efs_readdir (dirp) == NULL)
      goto fs_diag_iter_err;

  }

  do
  {
    uint32 len;

    dirent = efs_readdir(dirp);
    if (dirent == NULL)
    {
      finished = 1;
      goto fs_diag_iter_err;
    }

    len = sizeof (fs_diag_pathbuf);
    /* Copy to temporary buffer, if path is too long we quit. */
    if (strlcpy (pathbuf, dir_path, len) >= len ||
        strlcat (pathbuf, "/", len)     >= len ||
        strlcat (pathbuf, dirent->d_name, len) >= len)
      goto fs_diag_iter_err;

    /* Check if mode is returned, otherwise call stat to get mode. */
    if (FS_MASK_CHECK (dirent->d_stats_present, FS_DIRENT_HAS_ST_MODE))
    {
      fmode = dirent->d_stat.st_mode;
      sbufp = &dirent->d_stat;
    }
    else
    {
      if (efs_stat (pathbuf, &sbuf))
        goto fs_diag_iter_err;

      fmode = sbuf.st_mode;
      sbufp = &sbuf;
    }

    if ((S_ISDIR (fmode) && iter_pkt->header.op == FS_DIAG_ITER_DIRS) ||
        (S_ISREG (fmode) && iter_pkt->header.op == FS_DIAG_ITER_FILES))
      finished = 1;

  } while (finished == 0);

  if (strlen (pathbuf) >= FS_DIAG_MAX_FILENAME_SIZE)
    goto fs_diag_iter_err;

  rsp_len = sizeof (fs_diag_iter_rsp_type) - sizeof (iter_pkt->filename.file);
  rsp_len += strlen (pathbuf);

  rsp_pkt = (fs_diag_iter_rsp_type *) diagpkt_alloc (DIAG_FS_OP_F, rsp_len);

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_iter.");
    return ((PACKED void *) rsp_pkt);
  }

  rsp_pkt->header.cmd_code = iter_pkt->header.cmd_code;
  rsp_pkt->header.op = iter_pkt->header.op;
  rsp_pkt->header.status = (uint8) EFS1_OKAY_S;

  rsp_pkt->seq_num = iter_pkt->seq_num;

  rsp_pkt->entry_size = sbufp->st_size;
  rsp_pkt->dir_len = strlen (dir_path) + 1;

  rsp_len = rsp_pkt->filename.len = strlen (pathbuf);
  fs_memscpy ((void *)rsp_pkt->filename.file,strlen(pathbuf), pathbuf,rsp_len);

  /* The original implementation of this diag packet used fs_compat_enum_move
   * in fs_compat.c. Interestingly this function did not set the fields for
   * attributes, creation date, buffering option, cleanup option and physical
   * size. Therefore the values returned by the original diag packet depended
   * on the previous state of the structure and not the state of the entry.
   * We will set the packets with the correct data but it will defer from the
   * original packet. */

  /* Fill with default attributes. */
  rsp_pkt->attr = 0x00FF;
  rsp_pkt->buf_opt = 0x00;
  rsp_pkt->clean_opt = 0x00;

  /* We don't store creation date in EFS2 */
  rsp_pkt->crt_date = 0;

  rsp_pkt->size_on_dev = sbufp->st_blksize * sbufp->st_blocks;

  return ((PACKED void *) rsp_pkt);

fs_diag_iter_err:
  rsp_pkt = (fs_diag_iter_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, sizeof (struct fs_diag_rsp_header));

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_iter.");
    return ((PACKED void *) rsp_pkt);
  }

  rsp_pkt->header.cmd_code = iter_pkt->header.cmd_code;
  rsp_pkt->header.op = iter_pkt->header.op;
  if (finished == 0)
    rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno,FS_DIAG_TYPE_FILE);
  else
  {
    (void) efs_closedir (dirp);
    dirp = NULL;
    rsp_pkt->header.status = (uint8) EFS1_ENUM_DONE_S;
  }

  return ((PACKED void *) rsp_pkt);
}/* END fs_diag_iter */


/* Get how much space is available on the device. */
PACKED void *
fs_diag_space_avail (PACKED void *req_pkt, uint16 pkt_len)
{
  fs_diag_space_avail_rsp_type *rsp_pkt;
  fs_diag_space_avail_req_type *space_pkt;
  struct fs_statvfs efs_info;

  space_pkt = (fs_diag_space_avail_req_type *) req_pkt;
  (void) pkt_len;

  fsd_invalidate_memoization ();

  rsp_pkt = (fs_diag_space_avail_rsp_type *)
           diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_space_avail_rsp_type));

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_space_avail.");
    return ((PACKED void *) rsp_pkt);
  }

  rsp_pkt->header.cmd_code = space_pkt->header.cmd_code;
  rsp_pkt->header.op = FS_DIAG_SPACE_AVAIL;

  if (efs_statvfs ("/", &efs_info))
  {
    rsp_pkt->space = 0;
    rsp_pkt->header.status = fs_efs2_to_efs1_err (efs_errno,FS_DIAG_TYPE_DIR);
  }
  else
  {
    rsp_pkt->space = efs_info.f_bsize * efs_info.f_bavail;
    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;
  }

  return ((PACKED void *) rsp_pkt);
}/* END fs_diag_space_avail */


/* Formats a device.
 *
 * Attempts to format a device at a given path.
 */
PACKED void *
fs_diag_format (PACKED void *req_pkt, uint16 pkt_len)
{
  fs_diag_format_req_type *for_pkt = (fs_diag_format_req_type *) req_pkt;
  fs_diag_format_rsp_type *rsp_pkt;

  fsd_invalidate_memoization ();

  FSD_CHECK_NULLS (for_pkt, fs_diag_format_req_type,
      filename.file, pkt_len, 1);
  /* Check packet length. */
  if (pkt_len != (sizeof (fs_diag_format_req_type) + for_pkt->filename.len
                  - sizeof (for_pkt->filename.file)))
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));

  /* Check parameters. */
  if (for_pkt->filename.len > FS_DIAG_MAX_FILENAME_SIZE ||
      for_pkt->filename.len != (strlen (for_pkt->filename.file) + 1))
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  if (fs_diag_access_check (for_pkt->filename.file, DIAG_FS_CHECK_FORMAT)
      == FALSE)
    return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len));

  rsp_pkt = (fs_diag_format_rsp_type *)
            diagpkt_alloc (DIAG_FS_OP_F, sizeof (fs_diag_format_rsp_type));

  if (rsp_pkt == NULL)
  {
    FS_MSG_HIGH_0 ("Diagpkt_alloc returned NULL in fs_diag_format.");
    return ((PACKED void *) rsp_pkt);
  }

  rsp_pkt->header.cmd_code = for_pkt->header.cmd_code;
  rsp_pkt->header.op = FS_DIAG_FORMAT;

#ifdef FEATURE_FS_HOTPLUG
  if (hotplug_format (for_pkt->filename.file))
    rsp_pkt->header.status = (uint8) EFS1_FAIL_S;
  else
    rsp_pkt->header.status = (uint8) EFS1_OKAY_S;
#else
  rsp_pkt->header.status = (uint8) EFS1_FAIL_S;
#endif
  return ((PACKED void *) rsp_pkt);
}/* END fs_diag_format */


/* Main EFS1 packet handler.
 *
 * Takes a packet and routes it to the correct handler.
 */
PACKED void *
fs_diag_cmd (PACKED void *req_pkt, uint16 pkt_len)
{
  struct fs_diag_req_header *head = (struct fs_diag_req_header *) req_pkt;
  PACKED void * rsp_pkt;

  /* Check security. */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED)
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len));

  /* Check for minimum length of packet */
  if (!fs_diag_is_valid_pkt_len (pkt_len, sizeof (struct fs_diag_req_header)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len);
  }

  switch (head->op)
  {
    case FS_DIAG_MKDIR:
      rsp_pkt = fs_diag_mkdir (req_pkt, pkt_len);
      break;
    case FS_DIAG_RMDIR:
      rsp_pkt = fs_diag_rmdir (req_pkt, pkt_len);
      break;
    case FS_DIAG_DISP_DIRS:
    case FS_DIAG_DISP_FILES:
      rsp_pkt = fs_diag_disp (req_pkt, pkt_len);
      break;
    case FS_DIAG_READ_FILE:
      rsp_pkt = fs_diag_read_file (req_pkt, pkt_len);
      break;
    case FS_DIAG_WRITE_FILE:
      rsp_pkt = fs_diag_write_file (req_pkt, pkt_len);
      break;
    case FS_DIAG_REMOVE_FILE:
      rsp_pkt = fs_diag_remove_file (req_pkt, pkt_len);
      break;
    case FS_DIAG_GET_ATTR:
      rsp_pkt = fs_diag_get_attr (req_pkt, pkt_len);
      break;
    case FS_DIAG_SET_ATTR:
      rsp_pkt = fs_diag_set_attr (req_pkt, pkt_len);
      break;
    case FS_DIAG_ITER_DIRS:
    case FS_DIAG_ITER_FILES:
      rsp_pkt = fs_diag_iter (req_pkt, pkt_len);
      break;
    case FS_DIAG_SPACE_AVAIL:
      rsp_pkt = fs_diag_space_avail (req_pkt, pkt_len);
      break;
    case FS_DIAG_FORMAT:
      rsp_pkt = fs_diag_format (req_pkt, pkt_len);
      break;
    default:
      rsp_pkt = diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
  }

  if (rsp_pkt == NULL)
    FS_MSG_HIGH_0 ("Response pointer returned to fs_diag_cmd is NULL.");

  return (rsp_pkt);
}/* END fs_diag_cmd */

#endif /* FEATURE_EFS2 */
