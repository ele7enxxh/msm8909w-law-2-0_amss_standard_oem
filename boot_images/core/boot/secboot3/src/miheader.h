#ifndef MIHEADER_H
#define MIHEADER_H

/*===========================================================================

                          MULTI-IMAGE HEADER FILE

DESCRIPTION
  This header file contains declarations and type definitions for the
  Image Header Information.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/miheader.h#1 $

when       who                      what, where, why
--------   -------       ----------------------------------------------------------
03/17/14   ck             Changed SECBOOT_QHEE_SW_TYPE to proper 0x15 value
03/05/14   ck             Adjusted SECBOOT_QSEE_SW_TYPE and SECBOOT_QHEE_SW_TYPE to values given by secboot team
12/09/13   ck             Added SECBOOT_QSEE_SW_TYPE and SECBOOT_QHEE_SW_TYPE
12/03/13   ck             Added QSEE and QHEE to secboot_sw_type
04/17/12   kedara         Update sw type for MBA and Lpass image.
06/07/12   kedara         Added sw type for MBA, APPS, LPASS images 
04/05/11   dh     	  do not include customer.h
03/27/12   dh             Added secboot_sw_type define and SECBOOT_WDT_SW_TYPE
09/21/10   plc            Added in NAND Definitions for SB3.0
08/02/10   aus            Moved image_type to mibib.h
06/01/10   plc            Refactoring for SB3.0
03/12/10   plc            Reorder IMG enum's to keep consistent with previously 
                          built images
03/09/10   plc            Keep RPM_IMG at same location in array, so RPM image 
                          won't need to change it'd ID, since it builds remotely
03/03/10   plc            Added address for SBL1
01/05/10   aus            Changes for unified boot cookie and image type
                          APPS_KERNEL_IMG
03/11/09   msm            Added Q5 decoupling and removed offset for SCl_DBL_CODE_BASE
09/28/09   vtw            Added boot cookie header and RAMFS image types.
09/09/09   msm            Port for ehostdl.
06/12/09   mrunalm        Initial porting for msm7x30.

============================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#ifdef BUILD_BOOT_CHAIN
#include "boot_comdef.h"
#else /* BUILD_BOOT_CHAIN */
#include "comdef.h"
#endif /* BUILD_BOOT_CHAIN */

#include "mibib.h"
#endif  /* _ASM_ARM_ */


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_
/************************************************************/
/*      Image Type Enum definition is moved to mibib.h      */
/************************************************************/

/* Image component in order, code, signature, certificate */
typedef enum
{
  NONE_SEG =0,
  CODE_SEG,      /* Code        */
  SIGNATURE_SEG, /* Signature   */
  CERT_SEG,      /* Certificate */
  MAX_ITEM = 0x7FFFFFFF
}image_component_type;

/* enum for unified boot OS type */
typedef enum
{
  BMP_BOOT_OS= 0x0,
  WM_BOOT_OS = 0x1,
  ANDROID_BOOT_OS = 0x2,
  CHROME_BOOT_OS = 0x3,
  SYMBIAN_BOOT_OS = 0x4,
  LINUX_BOOT_OS = 0x5,
  MAX_OS = 0x7FFFFFFF
} boot_OS_type;


typedef struct
{
  image_type image_id;       /* Identifies the type of image this header
                                 represents (OEM SBL, AMSS, Apps boot loader,
                                 etc.). */
  uint32 header_vsn_num;     /* Header version number. */
  uint32 image_src;          /* Location of image in flash: Address of
                                 image in NOR or page/sector offset to image
                                 from page/sector 0 in NAND/SUPERAND. */
  uint8* image_dest_ptr;     /* Pointer to location to store image in RAM.
                                 Also, entry point at which image execution
                                 begins. */
  uint32 image_size;         /* Size of complete image in bytes */
  uint32 code_size;          /* Size of code region of image in bytes */
  uint8* signature_ptr;      /* Pointer to images attestation signature */
  uint32 signature_size;     /* Size of the attestation signature in
                                 bytes */
  uint8* cert_chain_ptr;     /* Pointer to the chain of attestation
                                 certificates associated with the image. */
  uint32 cert_chain_size;    /* Size of the attestation chain in bytes */

} mi_boot_image_header_type;

typedef struct
{
  image_type image_id;       /* Identifies the type of elf image this header
                                 represents (OEM SBL, AMSS, Apps boot loader,
                                 etc.). */
  uint32 image_src;          /* Location of elf image in flash: Address of
                                 image in NOR or page/sector offset to image
                                 from page/sector 0 in NAND/SUPERAND. */
} elf_image_info_type;

typedef struct
{
  uint32         magic_number1;         /* magic number 1 */
  uint32         version;               /* cookie version number */
  boot_OS_type   OS_type;               /* the OS type to boot into */
  uint32         boot_apps_parti_entry; /* start address in storage device of the APPS partition;
                                         NAND build from partition.c; eMMC build from MBR */
  uint32         boot_apps_size_entry;  /* Size in Bytes of APPS Kernel in APPS partition to be loaded */
  uint32         boot_apps_ram_loc;     /* location to copy APPS Kernel into RAM */
  void*          reserved_ptr;          /* reserve pointer */
  uint32         reserved_1;            /* reserve for future use */
  uint32         reserved_2;            /* reserve for future use */
  uint32         reserved_3;            /* reserve for future use */
} mi_boot_cookie_type;

typedef struct
{
    mi_boot_image_header_type image_header;
    mi_boot_cookie_type       boot_cookie;
}mi_boot_header_cookie_type;


/*---------------------------------------------------------------------------
  Software Type identifiying image being authenticated. These values
  correspond to the code signing tools (CSMS) Software ID field which has
  lower 32 bits for Software type and upper 32 bits for Software version.
---------------------------------------------------------------------------*/
typedef enum
{
  SECBOOT_SBL_SW_TYPE                = 0,
  SECBOOT_SBL1_SW_TYPE               = 0,
  SECBOOT_AMSS_SW_TYPE               = 1, 
  SECBOOT_DMSS_SW_TYPE               = 1,
  SECBOOT_MBA_SW_TYPE                = 1, /* Modem boot authenticator image */
  SECBOOT_AMSS_HASH_TABLE_SW_TYPE    = 2,
  SECBOOT_FLASH_PRG_SW_TYPE          = 3,
  SECBOOT_EHOSTD_SW_TYPE             = 3,
  SECBOOT_DSP_HASH_TABLE_SW_TYPE     = 4,
  SECBOOT_LPASS_HASH_TABLE_TYPE      = 4, /* Lpass hash table */
  SECBOOT_SBL2_SW_TYPE               = 5,
  SECBOOT_SBL3_SW_TYPE               = 6,
  SECBOOT_TZ_KERNEL_SW_TYPE          = 7, /* TZBSP Image */
  SECBOOT_QSEE_SW_TYPE               = 7, /* TZ is now called QSEE */
  SECBOOT_HOSTDL_SW_TYPE             = 8,
  SECBOOT_APPSBL_SW_TYPE             = 9,
  SECBOOT_RPM_FW_SW_TYPE             = 10,
  SECBOOT_SPS_HASH_TABLE_TYPE        = 11,
  SECBOOT_TZ_EXEC_HASH_TABLE_TYPE    = 12, /* Playready or TZ Executive Image */
  SECBOOT_RIVA_HASH_TABLE_TYPE       = 13,
  SECBOOT_APPS_HASH_TABLE_TYPE       = 14, /* Apps Image */
  SECBOOT_WDT_SW_TYPE                = 18, /* Wdog debug image */
  SECBOOT_QHEE_SW_TYPE               = 0x15,
  SECBOOT_MAX_SW_TYPE                = 0x7FFFFFFF /* force to 32 bits*/
} secboot_sw_type;


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/


#endif  /* _ASM_ARM_ */
#endif  /* MIHEADER_H */

