#ifndef MIHEADER_H
#define MIHEADER_H

/*===========================================================================

                          MULTI-IMAGE HEADER FILE

DESCRIPTION
  This header file contains declarations and type definitions for the
  Image Header Information.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2009-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/boot/mibib/src/miheader.h#1 $

when       who                      what, where, why
--------   -------       ----------------------------------------------------------
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

#include "customer.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* ----------------------------------------------------- */
/*   Image Header Defintion                              */
/* ----------------------------------------------------- */

/*===========================================================================
 Image address constants used for MMC/SDRAM.
===========================================================================*/

#define MI_SDCC_SBL1_IMG_DEST_ADDR   SCL_SBL1_CODE_BASE

#define MI_SDCC_SBL2_IMG_DEST_ADDR     SCL_SBL2_CODE_BASE

#define MI_SDCC_SBL3_IMG_DEST_ADDR     SCL_SBL3_CODE_BASE

#define MI_SDCC_RPM_IMG_DEST_ADDR      SCL_RPM_CODE_BASE

#define MI_SDCC_TZ_IMG_DEST_ADDR       SCL_TZ_CODE_BASE

#define MI_SDCC_AMSS_IMG_DEST_ADDR     SCL_MODEM_CODE_BASE

#define MI_SDCC_APPSBL_IMG_DEST_ADDR   SCL_APPS_BOOT_BASE

#define MI_SDCC_APPS_IMG_DEST_ADDR     SCL_APPS_CODE_BASE

/*===========================================================================
 Image address constants used for NAND/SDRAM.
===========================================================================*/

#define MI_NAND_AMSS_IMG_DEST_ADDR     SCL_MODEM_CODE_BASE

#define MI_NAND_APPSBL_IMG_DEST_ADDR   SCL_APPS_BOOT_BASE

#define MI_NAND_APPS_IMG_DEST_ADDR     SCL_APPS_CODE_BASE

#define MI_NAND_ADSP_Q5_DEST_ADDR      SCL_ADSP_Q5_CODE_BASE

#define MI_NAND_SBL1_IMG_DEST_ADDR     SCL_SBL1_CODE_BASE

#define MI_NAND_SBL2_IMG_DEST_ADDR     SCL_SBL2_CODE_BASE

#define MI_NAND_SBL3_IMG_DEST_ADDR     SCL_SBL3_CODE_BASE

#define MI_NAND_RPM_IMG_DEST_ADDR      SCL_RPM_CODE_BASE

#define MI_NAND_TZ_IMG_DEST_ADDR       SCL_TZ_CODE_BASE

/* This is used to see the size of each hash value;20 bytes per hash result */
#define SECBOOT_SIGNATURE_SIZE  20   /* SHA-1 digest size  */

/* These are the PFLAG values for R,W,X in the ELF header (bits 0-3) as defined
 * in the latest ELF documentation is at
 * http://www.caldera.com/developers/gabi/latest/ch5.pheader.html
 * Used QCSBL to determine if a segment is writeable or not
 */
#define MI_ELF_PFLAGS_R            0x4
#define MI_ELF_PFLAGS_W            0x2
#define MI_ELF_PFLAGS_X            0x1

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

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/


#endif  /* _ASM_ARM_ */
#endif  /* MIHEADER_H */

