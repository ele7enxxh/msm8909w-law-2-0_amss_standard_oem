
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/modem_sec/inc/modem_rollback.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
01/09/13   rsahay       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/* IDs for MBA and Modem Rollback Versions */
#define MODEM_SEC_MBA_ID           1
#define MODEM_SEC_MODEM_ID         2

struct modem_version_rollback_img_set
{
  /* List of images in current image set*/
  const uint32 image_id;

  /* Base address of the fuse that defines the LSB of version number of current image set*/
  const uint32 feature_enable_mask;
  
  /* Bit Mask that should be applies to the fuse that defines anti rollback feature for current image set*/
  const uint32 * const feature_enable_base_addr;
  
  /* Bit Mask that should be applies to the LSB fuse to extract LSB of version number*/
  const uint32 version_mask;
  
  /* Bit Shift that should be applies to the LSB fuse to extract LSB of version number*/
  const uint32 version_shift;
  
  /* Base address of the fuse that defines the LSB of version number of current image set*/
  const uint32 * const version_lsb_base_addr_pa;
 
  /* Base address of the fuse that defines the LSB of version number of current image set*/
  const uint32 * const version_lsb_base_addr_va;

  /* variable that store the current version(obtain from secboot cert after authentication) */
  uint32 current_version_num;
  
};

int modem_rollback_version_main(void);
