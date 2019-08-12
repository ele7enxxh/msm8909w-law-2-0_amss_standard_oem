#ifndef _OEMSBL_SFS_H_
#define _OEMSBL_SFS_H_
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
 
                           Secure File Sytem

                  I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  Part of sfs_flash_funcs.c APIS (flash file size query and falsh file read functionality)
  are required to be built in oem boot loader.
  This header file only contains the data types required by sfs_flash_funcs.c
  oem boot loader APIs.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2009 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCVPath: $
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/sfs/environment/env/sbl/inc/oemsbl_sfs.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/10   ejt     Added support for sfs_priv_meid_read and sfs_priv_meid_write APIs 
11/02/09   yh      Create Model, grab from sfs.h 
===========================================================================*/

#ifdef BUILD_BOOT_CHAIN
#include "boot_comdef.h"

#define MSG_ERROR
#define MSG_MED
#define MSG_HIGH 
#define MSG_FATAL

#include "IxErrno.h"
#include "fs_fcntl.h"

typedef errno_enum_type secerrno_enum_type;
#endif

/* Predefined secure flash partition sectors */
typedef enum
{
  SFS_FLASH_SIMLOC_SECTOR,
  SFS_FLASH_IMEI_SECTOR,
  SFS_FLASH_PRIV_FILE_1_SECTOR,
  SFS_FLASH_PRIV_FILE_2_SECTOR,
  SFS_FLASH_PRIV_FILE_3_SECTOR,
  SFS_FLASH_PRIV_FILE_4_SECTOR,
  SFS_FLASH_MEID_SECTOR,          /* From SIMLOC to MEID are reserved for historic sfs_private.h APIs */
  SFS_FLASH_PRIV_FILE_6_SECTOR,   /* Sector 6 to sector 14 is reserved for SSD */
  SFS_FLASH_PRIV_FILE_7_SECTOR,
  SFS_FLASH_PRIV_FILE_8_SECTOR,
  SFS_FLASH_PRIV_FILE_9_SECTOR,
  SFS_FLASH_PRIV_FILE_10_SECTOR,
  SFS_FLASH_PRIV_FILE_11_SECTOR,
  SFS_FLASH_PRIV_FILE_12_SECTOR,
  SFS_FLASH_PRIV_FILE_13_SECTOR,
  SFS_FLASH_PRIV_FILE_14_SECTOR,
  SFS_FLASH_PRIV_FILE_15_SECTOR,
  SFS_FLASH_PRIV_FILE_16_SECTOR,
  SFS_FLASH_PRIV_FILE_17_SECTOR,
  SFS_FLASH_PRIV_FILE_18_SECTOR,
  SFS_FLASH_PRIV_FILE_19_SECTOR,
  SFS_FLASH_PRIV_FILE_20_SECTOR,
  SFS_FLASH_PRIV_FILE_21_SECTOR, 
  SFS_FLASH_PRIV_FILE_22_SECTOR, 
  SFS_FLASH_PRIV_FILE_23_SECTOR, 
  SFS_FLASH_PRIV_FILE_24_SECTOR, 
  SFS_FLASH_PRIV_FILE_25_SECTOR, 
  SFS_FLASH_PRIV_FILE_26_SECTOR,
  SFS_FLASH_PRIV_FILE_27_SECTOR,
  SFS_FLASH_PRIV_FILE_28_SECTOR,
  SFS_FLASH_PRIV_FILE_29_SECTOR,
  SFS_FLASH_PRIV_FILE_30_SECTOR,
  SFS_FLASH_PRIV_FILE_31_SECTOR,
  SFS_FLASH_PRIV_FILE_32_SECTOR,
  SFS_FLASH_PRIV_FILE_33_SECTOR,
  SFS_FLASH_PRIV_FILE_34_SECTOR,
  SFS_FLASH_PRIV_FILE_35_SECTOR,
  SFS_FLASH_PRIV_FILE_36_SECTOR,
  SFS_FLASH_PRIV_FILE_37_SECTOR,
  SFS_FLASH_PRIV_FILE_38_SECTOR,
  SFS_FLASH_PRIV_FILE_39_SECTOR,
  SFS_FLASH_PRIV_FILE_40_SECTOR,
  SFS_FLASH_PRIV_FILE_41_SECTOR, 
  SFS_FLASH_PRIV_FILE_42_SECTOR, 
  SFS_FLASH_PRIV_FILE_43_SECTOR, 
  SFS_FLASH_PRIV_FILE_44_SECTOR, 
  SFS_FLASH_PRIV_FILE_45_SECTOR,/* From FILE_6 to FILE_45 are used by sfp.h APIs */
  SFS_FLASH_RESERVED_SECTOR,     /* Virtual sector to contain sfp saved data size */
  SFS_FLASH_KEY_SECTOR,          /* The last page(64th) of sfp reserved as key page */
  SFS_FLASH_MAX_SECTOR           /* Max mark */
}sfs_sector_enum_type;

#define  SFS_FLASH_PRIV_FILE_BOUNDARY           SFS_FLASH_MEID_SECTOR 
#define  SFS_FILE_TYPE_NORM                     0      // normal file
#define  SFS_FILE_TYPE_IX                       1      // index file
#define  SFS_FILE_TYPE_FLASH_DEV                0x100  // flash dev
#define  SFS_FILE_TYPE_FLASH_NO_ENCRYPT         0x1000 // do not encrypt the content
#define  SFS_FILE_TYPE_FLASH_NO_SEC_SIZE_CHECK  0x2000 // do not check sector size
#define  SFS_FILE_TYPE_FLASH_DELETE             0x4000 // delete a file and it's record from flash


/* TODO, review with shital, do we need this enum at all?*/
#if 0
typedef enum
{
  SFS_FILE_TYPE_NORM      = 0,           // normal file
  SFS_FILE_TYPE_IX        = 1,           // index file
  SFS_FILE_TYPE_FLASH_DEV = 0x100,       // flash dev
  SFS_FILE_TYPE_FLASH_DEV_SIMLOC      =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_SIMLOC_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_IMEI        =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_IMEI_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_1 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_1_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_2 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_2_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_3 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_3_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_4 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_4_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_5 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_5_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_6 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_6_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_7 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_7_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_8 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_8_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_9 =  (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_9_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_10 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_10_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_11 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_11_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_12 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_12_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_13 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_13_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_14 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_14_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_15 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_15_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_16 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_16_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_17 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_17_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_18 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_18_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_19 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_19_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_20 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_20_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_21 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_21_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_22 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_22_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_23 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_23_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_24 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_24_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_25 = (SFS_FILE_TYPE_FLASH_DEV | SFS_FLASH_PRIV_FILE_25_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_RESV,
  SFS_FILE_TYPE_FLASH_NO_ENCRYPT       = 0x1000,// do not encrypt the content
  SFS_FILE_TYPE_FLASH_NO_SEC_SIZE_CHECK= 0x2000,// do not check sector size
  SFS_FILE_TYPE_FLASH_DELETE           = 0x4000,// delete a file and it's record from flash
  SFS_FILE_TYPE_FLASH_DEV_MAX          = (SFS_FILE_TYPE_FLASH_DEV_RESV | SFS_FILE_TYPE_FLASH_DELETE)
}sfs_file_type_enum_type;
#endif

#define SFS_AES_BLOCK_SIZE   16
#define SFS_SYS_KEY_SIZE     SFS_AES_BLOCK_SIZE
#define SFS_SHA_SIZE         20
#define SFS_COOKIE           0xaa7e7e55
#define SFS_HMAC_SIZE        20

typedef int sfs_segfh_type;

/* This is a simplified version of sfs_handle_type, contains enough information
   for sfs_flash_read to read data from flash partition directly.
   The complete version of sfs_handle_type which defined variable for 
   data encryption,hmac is in sfs.h
   The data type define decision is controlled by BUILD_BOOT_CHAIN */
#ifdef BUILD_BOOT_CHAIN
typedef struct
{
  /* file info */
  uint32                  cookie;
  uint32                  type;       /* normal, index, flashdev, etc */
  uint32                  flags;       /* O_READ, O_WRITE, O_CREATE etc */
  uint32                  file_size;   /* number of bytes in file (can be zero) */


  /* segment info */
  uint8                  *seg_buf;   /* must be malloc'd for each instance, 32 byte aligned pointer */
  uint32                  seg_size;  /* N * block size (multiple block sizes) */
  uint32                  curr_seg;  /* 0..M-1 */
  sfs_segfh_type          seg_fh;    /* sfs_flash_dev pointer */

}sfs_handle_type;
#endif

#endif // _OEMSBL_SFS_H_
