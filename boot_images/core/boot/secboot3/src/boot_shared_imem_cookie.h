#ifndef BOOT_SHARED_IMEM_COOKIE_H
#define BOOT_SHARED_IMEM_COOKIE_H

/*===========================================================================

                                Boot Shared Imem Cookie
                                Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for Boot's shared 
  imem cookies. Those cookies are shared between boot and other subsystems 

Copyright  2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   --------     ------------------------------------------------------
01/13/15   c_mvanim     Add ABNORMAL_RESET_ENABLED
09/17/14   jz           Removed rpm_sync_cookie
06/20/14   ck           Added QHEE shared imem offset and magic number for backup logic
06/20/14   ck           Renamed qsee_dload_dump_shared_imem_cookie_type to security_region_dump_shared_imem_cookie_type
05/14/14   ck           Restored qsee_dload_dump_shared_imem_cookie_type
05/14/14   ck           Restored shared imem offset and magic number used for QSEE dload backup
04/14/14   jz           Added back BOOT_RPM_SYNC_MAGIC_NUM for MDM support in Bear
03/21/14   ck           Added 32bit padding under 64bit pointer so 8 bytes are consumed
03/21/14   ck           Added reset_status_register to boot_shared_imem_cookie_type
02/13/14   ck           Removed unused RPM sync and TZ dload magic cookie values
02/12/14   ck           Updated etb_buf_addr and l2_cache_jump_buff_ptr to 64 bit
                        Removed dload_magic_1 and 2 because there is a register for this in Bear
                        Added a version number to shared imem structure as a safety for UEFI and others
                        Removed tz_dload_dump_shared_imem_cookie_type as it's no longer needed in Bear
10/22/13   ck           Removed rpm_sync_cookie from boot_shared_imem_cookie_type
                        as Bear boot does not interface with RPM
03/19/13   dh           Add UEFI_RAW_RAM_DUMP_MAGIC_NUM,
                        Allocate abnormal_reset_occurred cookie for UEFI 
02/28/13   kedara       Added feature flag FEATURE_TPM_HASH_POPULATE
02/21/13   dh           Move SHARED_IMEM_TPM_HASH_REGION_OFFSET to boot_shared_imem_cookie.h
02/15/13   dh           Add tz_dload_dump_shared_imem_cookie_type
11/27/12   dh           Add rpm_sync_cookie
07/24/12   dh           Add ddr_training_cookie
06/07/11   dh           Initial creation
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* 
 * Following magic number indicates the boot shared imem region is initialized
 * and the content is valid
 */
#define BOOT_SHARED_IMEM_MAGIC_NUM        0xC1F8DB40

/*
 * Version number to indicate what cookie structure is being used
 */
#define BOOT_SHARED_IMEM_VERSION_NUM   0x1

/* 
 * Magic number for UEFI crash dump
 */
#define UEFI_CRASH_DUMP_MAGIC_NUM         0x1

/* 
 * Abnormal reset occured 
 */
#define ABNORMAL_RESET_ENABLED         0x1

/* 
 * Magic number for raw ram dump
 */
#define BOOT_RAW_RAM_DUMP_MAGIC_NUM       0x2

/* Default value used to initialize shared imem region */
#define SHARED_IMEM_REGION_DEF_VAL  0xFFFFFFFF 

/* Shared IMEM offset and magic number used by QSEE to indicate to
   SBL that it needs to be backed up for delayed ramdump scenario */
#define QSEE_DLOAD_DUMP_SHARED_IMEM_MAGIC_NUM  0x49445a54
#define QSEE_DLOAD_DUMP_SHARED_IMEM_OFFSET     0x748


/* Shared IMEM offset and magic number used by QHEE to indicate to
   SBL that it needs to be backed up for delayed ramdump scenario */
#define QHEE_DLOAD_DUMP_SHARED_IMEM_MAGIC_NUM  0xc1f8db42
#define QHEE_DLOAD_DUMP_SHARED_IMEM_OFFSET     0xb20


#ifdef FEATURE_TPM_HASH_POPULATE
/* offset in shared imem to store image tp hash */
#define SHARED_IMEM_TPM_HASH_REGION_OFFSET 0x834
#define SHARED_IMEM_TPM_HASH_REGION_SIZE   256

#endif /* FEATURE_TPM_HASH_POPULATE */

/* 
 * Following structure defines all the cookies that have been placed 
 * in boot's shared imem space.
 * The size of this struct must NOT exceed SHARED_IMEM_BOOT_SIZE
 */
struct boot_shared_imem_cookie_type
{
  /* Magic number which indicates boot shared imem has been initialized
     and the content is valid.*/ 
  uint32 shared_imem_magic;

  /* Number to indicate what version of this structure is being used */
  uint32 shared_imem_version;
  
  /* Pointer that points to etb ram dump buffer, should only be set by HLOS */
  uint64 etb_buf_addr;
  
  /* Region where HLOS will write the l2 cache dump buffer start address */
  uint64 *l2_cache_dump_buff_ptr;

  /* When SBL which is A32 allocates the 64bit pointer above it will only
     consume 4 bytes.  When HLOS running in A64 mode access this it will over
     flow into the member below it.  Adding this padding will ensure 8 bytes
     are consumed so A32 and A64 have the same view of the remaining members. */
  uint32 a64_pointer_padding;

  /* Magic number for UEFI ram dump, if this cookie is set along with dload magic numbers,
     we don't enter dload mode but continue to boot. This cookie should only be set by UEFI*/
  uint32 uefi_ram_dump_magic;
  
  uint32 ddr_training_cookie;
  
  /* Abnormal reset cookie used by UEFI */
  uint32 abnormal_reset_occurred;
  
  /* Reset Status Register */
  uint32 reset_status_register;
  
  /* Please add new cookie here, do NOT modify or rearrange the existing cookies*/
};


/* 
 * Following structure defines the memory dump cookies QSEE and QHEE
 * will place in its shared imem space.
 */
struct security_region_dload_dump_shared_imem_cookie_type
{
  /* Magic number which indicates dload dump shared imem has been initialized
     and the content is valid.*/
  uint32 magic_num;
  
  /* Source address that we should copy from */
  uint32 *src_addr;
  
  /* Destination address that we should dump to */
  uint32 *dst_addr;
  
  /* Total size we should copy */
  uint32 size;
};


/*  Global pointer points to the boot shared imem cookie structure  */
extern struct boot_shared_imem_cookie_type *boot_shared_imem_cookie_ptr;


#endif /* BOOT_SHARED_IMEM_COOKIE_H */

