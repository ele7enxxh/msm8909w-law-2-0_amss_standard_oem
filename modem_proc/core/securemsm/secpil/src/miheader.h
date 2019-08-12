#ifndef MIHEADER_H
#define MIHEADER_H

/*===========================================================================

                          MULTI-IMAGE HEADER FILE

DESCRIPTION
  This header file contains declarations and type definitions for the
  Image Header Information.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2009-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/src/miheader.h#1 $

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
#include "comdef.h"
#include "mibib.h"
#endif  /* _ASM_ARM_ */


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* ----------------------------------------------------- */
/*   Image Header Defintion                              */
/* ----------------------------------------------------- */

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

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/


#endif  /* _ASM_ARM_ */
#endif  /* MIHEADER_H */

