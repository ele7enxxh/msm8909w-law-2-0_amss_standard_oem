#ifndef _SFS_FLASH_FUNCS_H_
#define _SFS_FLASH_FUNCS_H_

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   A S W   S E C U R I T Y   S E R V I C E S
 
                        S F S    F L A S H    F U N C S

                  I N T E R N A L    H E A D E R    F I L E


GENERAL DESCRIPTION
   Header file for sfs flash functions

EXTERNAL FUNCTIONS


Copyright (c) 2009 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/sfs/environment/env/amss/inc/sfs_flash_funcs.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/09   yh      Public sfs_flash_erase_block to support sfp_erase_all
10/30/09   yh      create model
*/

#include "core_variation.h"
#ifdef FEATURE_SEC_SFS
#include "oemsbl_sfs.h"
#ifndef BUILD_BOOT_CHAIN
#include "sfs.h"
#include "sfs_private.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef BUILD_BOOT_CHAIN
#include "boot_comdef.h"

#define MSG_ERROR
#define MSG_MED
#define MSG_HIGH 
#define MSG_FATAL

#else
#include "comdef.h"
#include "msg.h"
#endif

#include "secerrno.h"

#ifndef FEATURE_SEC_EXCLUDE_SFP_10
#include "flash.h"

 
/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
typedef struct
{
#ifdef FLASH_USES_DAL
    flash_handle_t        nand_device;
#else
    fs_device_t           nand_device;
#endif
  uint32 cookie;
  int active_block;       /* active block, contains key */
  uint8 active_seq;      /* active block's sequence num */
  int alt_block;          /* alternate block, ping pong with active block */
  int block_size;
  int block_count;        /* number of erasable units in flash */
  int pages_in_block;     /* number of pages in block          */
  int page_size;          /* number of bytes in a page         */
  int max_pages;          /* number of pages in the device     */
}sfs_flash_dev;
#else
typedef struct
{ 
  uint32 dummy;
}sfs_flash_dev;
#endif // FEATURE_SEC_EXCLUDE_SFP_10

/* flash file information data struct */
typedef struct
{
  uint32 page_offset;     /* where this file saved, mandatory info */
  uint32 sector_size;     /* file size, mandatory info */
  uint16 group_id;        /* PMM group id, only set through PMM API */
  uint16 handle_id;       /* PMM handle id, only set through PMM API */
}sfs_flash_sector_type;

extern sfs_flash_dev         sfs_fdev;
extern sfs_flash_sector_type sfs_flash_sector_table[];

/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION SFS_FLASH_ISREADY

DESCRIPTION
checks to see if flash secure partition has been initialize with key sector

DEPENDENCIES
None

RETURN VALUE
E_SUCCESS or E_FAILURE

SIDE EFFECTS
None.
===========================================================================*/
boolean sfs_flash_isready(void);

/*===========================================================================

FUNCTION SFS_FLASH_DEV_INIT

DESCRIPTION
initializes the flash dev partion

DEPENDENCIES
None

RETURN VALUE
E_SUCCESS or E_FAILURE

SIDE EFFECTS
None.
===========================================================================*/
secerrno_enum_type sfs_flash_dev_init(sfs_flash_dev *fd);

/*===========================================================================

FUNCTION SFS_FLASH_WRITE ()

DESCRIPTION
  Write bytes to flash device.
  Will write a whole page at a time.
  The page will correspond to the current file (sector)
  which in turn points to a page offset.

  The segment number is added to this offset to get the actual page read.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes written.

SIDE EFFECTS
  None.
===========================================================================*/

int sfs_flash_write(sfs_handle_type *fh, const char *buf, int32 nbytes);

/*===========================================================================

FUNCTION SFS_FLASH_READ ()

DESCRIPTION
  Will read nbytes from flash.
  Typically will read a whole page at a time.
  The page read will correspond to the current file (sector)
  which in turn points to a page offset.

  The segment number is added to this offset to get the actual page read.

DEPENDENCIES
  For now if you dont know how many bytes to read, you may not get de-cipher
  the data correctly.  This is because there is no knowlege of how many bytes
  were originally written.  That is a TODO feature.

RETURN VALUE
  number of bytes read.

SIDE EFFECTS
  None.
===========================================================================*/

int sfs_flash_read(sfs_handle_type *fh, int32 nbytes);

/* After split sfs_ll_funcs, move below prototypes to header file,
   so they can be accessible from sfs_ll_funcs.cpp */ 
uint32 sfs_flash_alloc_size(sfs_handle_type *fh);

sfs_segfh_type sfs_flash_open(sfs_handle_type *fh);

secerrno_enum_type sfs_flash_close(sfs_handle_type *fh);

#ifndef BUILD_BOOT_CHAIN
secerrno_enum_type sfs_flash_erase_block(sfs_flash_dev *fd, int block);
secerrno_enum_type sfs_flash_key_get(sfs_handle_type *fh, uint8 *key);
#endif

#ifdef __cplusplus
}
#endif

#endif // FEATURE_SEC_SFS
#endif // _SFS_FLASH_FUNCS_H_
