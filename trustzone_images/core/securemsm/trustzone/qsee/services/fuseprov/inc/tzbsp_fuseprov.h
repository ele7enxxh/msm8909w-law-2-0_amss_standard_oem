/**
@file tzbsp_fuseprov.h
@brief Trustzone Init Handler

This file contains the routines for initializing TZBSP.

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/fuseprov/inc/tzbsp_fuseprov.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2013/09/23     mic         Initial version
=============================================================================*/

#ifndef TZBSP_FUSEPROV_H
#define TZBSP_FUSEPROV_H

#include "comdef.h"
#include "IxErrno.h"

#define FUSEPROV_INFO_MAX_SIZE        16
#define FUSEPROV_SECDAT_MAGIC1        0x3B7251CA
#define FUSEPROV_SECDAT_MAGIC2        0x2A126F29
#define FUSEPROV_SECDAT_REVISION      0x1

/* Known fuses regions */
typedef enum
{
  FUSEPROV_REGION_TYPE_OEM_SEC_BOOT     = 0x00000000,
  FUSEPROV_REGION_TYPE_OEM_PK_HASH      = 0x00000001,
  FUSEPROV_REGION_TYPE_SEC_HW_KEY       = 0x00000002,
  FUSEPROV_REGION_TYPE_OEM_CONFIG       = 0x00000003,
  FUSEPROV_REGION_TYPE_READ_WRITE_PERM  = 0x00000004,
  FUSEPROV_REGION_TYPE_SPARE_REG19      = 0x00000005,
  FUSEPROV_REGION_TYPE_GENERAL          = 0x00000006,
  FUSEPROV_REGION_TYPE_FEC_EN           = 0x00000007,
  FUSEPROV_REGION_TYPE_ANTI_ROLLBACK_2  = 0x00000008,
  FUSEPROV_REGION_TYPE_ANTI_ROLLBACK_3  = 0x00000009,
  FUSEPROV_REGION_TYPE_MAX              = 0x7FFFFFFF
} fuseprov_region_etype;

typedef enum
{
  FUSEPROV_OPERATION_TYPE_BLOW           = 0x00000000,  /* Blow the fuse */
  FUSEPROV_OPERATION_TYPE_VERIFYMASK0    = 0x00000001,  /* Read the fuse and verify the mask bits are 0 */
  FUSEPROV_OPERATION_TYPE_MAX            = 0x7FFFFFFF
} fuseprov_operation_etype;

/* Apart from the few valid error conditions (stated in comments below), error's will cause device to reset */
/* and go into SBL download mode, where TZ DIAG logs can be retrieved for more information */
typedef enum
{
  FUSEPROV_SUCCESS                      = 0x00000000,   /* No error */
  FUSEPROV_FAILURE                      = 0x00000001,   /* General failure (triggers SBL dload mode) */
  FUSEPROV_INVALID_ARG                  = 0x00000002,   /* Invalid arguments passed (triggers SBL dload mode) */
  FUSEPROV_NO_MEMORY                    = 0x00000003,   /* Could not allocated memory (triggers SBL dload mode) */
  FUSEPROV_SECDAT_MAGIC_MISMATCH        = 0x00000004,   /* Invalid magic number in secdat header (triggers SBL dload mode) */
  FUSEPROV_SECDAT_REV_NOT_SUPPORTED     = 0x00000005,   /* Revision of secdat header not supported (triggers SBL dload mode) */
  FUSEPROV_SECDAT_SIZE_LEN_MISMATCH     = 0x00000006,   /* Invalid size in secdat header (triggers SBL dload mode) */
  FUSEPROV_QFPROM_READ_ERROR            = 0x00000007,   /* Error returned from QFPROM driver during fuse read operation */
                                                        /* (triggers SBL dload mode) */
  FUSEPROV_QFUSE_REV_NOT_SUPPORTED      = 0x00000008,   /* Revision of qfuse list header not supported (triggers SBL dload mode) */
  FUSEPROV_INVALID_HASH                 = 0x00000009,   /* Invalid secdat, hash does not match hash in footer */
                                                        /* (triggers SBL dload mode) */
  FUSEPROV_TZBSP_HASH_FAIL              = 0x0000000A,   /* Error returned by hash API (triggers SBL dload mode) */
  FUSEPROV_SECDAT_LOCK_BLOWN            = 0x0000000B,   /* OEM SECBOOT write disable perm fuse already blown.*/
                                                        /* Device continues boot up as normal without parsing sec.dat */
  FUSEPROV_QFPROM_WRITE_ERROR           = 0x0000000C,   /* Error returned from QFPROM driver during fuse write operation */
                                                        /* (triggers SBL dload mode) */
  FUSEPROV_SHK_RD_WR_DISABLE_BLOWN      = 0x0000000D,   /* SHK already provisioned, read/write permission fuses blown */
                                                        /* fuse blowing ignores SHK fuses */
  FUSEPROV_SHK_GENERATION_FAILED        = 0x0000000E,   /* SHK could not be generated. PRNG failure */
                                                        /* (triggers SBL dload mode) */
  FUSEPROV_SHK_ALRDY_BLOWN              = 0x0000000F,   /* SHK is already provisioned and does not need to be regenerated */
                                                        /* fuse blowing ignores SHK fuses */
  FUSEPROV_SHK_RD_WR_MISMATCH           = 0x00000010,   /* Only 1 of the SHK permission fuses is blown -  */
                                                        /* either r/w perm disable fuse blown. Both fuses must be blown */
                                                        /* together (triggers SBL dload mode) */
  FUSEPROV_SECDAT_DEFAULT_NOFUSES       = 0x00000011,   /* There are no fuse entries in sec.dat, nothing to blow */
                                                        /* This is the default sec.dat. Device continues boot up as normal */
  FUSEPROV_ERR_TYPE_MAX                 = 0x7FFFFFFF
} fuseprov_error_etype;

#pragma pack(push)
#pragma pack(1)

/*
SEC.DAT file will contain
fuseprov_secdat_hdr + fuseprov_qfuse_list_hdr + list of fuseprov_qfuse_entry +
                                                fuseprov_secdat_footer
                             OR
fuseprov_secdat_hdr + fuseprov_secdat_footer (i.e no fuse info - default sec.dat)
In future sec.dat could contain other information apart from qfuse's
*/
typedef struct
{
  uint32  magic1;                       /* First Magic: FUSEPROV_SECDAT_MAGIC1 */
  uint32  magic2;                       /* Second signature: FUSEPROV_SECDAT_MAGIC2 */
  uint32  revision;                     /* revision: FUSEPROV_SECDAT_REVISION */
  uint32  size;                         /* size in bytes after this header field */
  uint8   info[FUSEPROV_INFO_MAX_SIZE]; /* information string */
  uint32  reserved[4];                  /* Reserved for future use */
} fuseprov_secdat_hdr_type;

typedef struct
{
  uint32  revision;                     /* revision: FUSEPROV_QFUSE_LIST_HDR_REVISION */
  uint32  size;                         /* size in bytes of the fuse data: fuse_count * sizeof(fuseprov_qfuse_entry) + */
                                        /* it is NOT valid to have size field as 0. If this header */
                                        /* is present, it means fuse information follows */
  uint32  fuse_count;                   /* Number of fuses to process. Must be >= 1 */
  uint32  reserved[4];                  /* Reserved for future use */
} fuseprov_qfuse_list_hdr_type;

typedef struct
{
  fuseprov_region_etype     region_type;     /* Fuse region type */
  uint32                    raw_row_address; /* Fuse Address */
  uint32                    lsb_val;         /* Fuse LSB value */
  uint32                    msb_val;         /* Fuse MSB value */
  fuseprov_operation_etype  operation;       /* Operation */
} fuseprov_qfuse_entry;

typedef struct
{
  uint8   hash[32];
} fuseprov_secdat_footer_type;              /* Hash of sec.dat file starting from the main header till before this footer */

#pragma pack(pop)

/**
 * Blows the fuses listed in the secdat buffer loaded in TZ DDR memory
 * secdat is only consumed if the OEM SEC BOOT write perm disable fuse
 * has not been blown
 *
 * Device will reset if any fuse is blown or on error
 *
 */
void
tzbsp_blow_fuses_and_reset(void);

#endif
