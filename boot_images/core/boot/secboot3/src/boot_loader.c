/*=============================================================================

                              Boot Loader

GENERAL DESCRIPTION
  This module performs binary image and AMSS loading.

Copyright 2010-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_loader.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/12/14   mohan   Added check code to verify integer overflow
11/10/14   aus     Added integer overflow checks
10/30/14   aus     Remove added local clobber holes after reading from flash
10/09/14   jz      Removed modified elf loading support
07/31/14   jz      Added support for modified elf loading
10/24/13   ck      Removed boot_rpm_cookie_sync, boot_init_rpm_entry_address,
                   and boot_notify_bl_jump_to_rpm
05/06/13   jz      Added log to indicate when RPM sync cookied is updated
03/21/13   dh      Add memory_barrier to boot_notify_bl_jump_to_rpm
02/28/13   kedara  Do not skip rpm execution for flashless boot.
11/28/12   dh      Add boot_rpm_cookie_sync
09/20/12   kedara  Image src field used to modify the offset from the image
                   header for binary images.
06/14/12   kedara  Added support to read FOTA partition and check for recovery
                   mode cookie
05/08/12   dh      substract the address offset between rpm and apps adress view from
                   rpm entry address because we load RPM FW in apps proc now
09/06/11   dh      Move boot_init_rpm_entry_addr and boot_notify_bl_jump_to_rpm 
                   to this file
04/29/11   dh      Add boot_load_image_header to support loading image header
03/04/11   dxiang  Support for loading general image types 
02/22/11   dxiang  Define boot_load_ssd_partition() for modem encryption
08/04/10   aus     Clean-up secboot 3
07/22/10   aus     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_loader.h"
#include "boot_error_handler.h"
#include "boot_sbl_if.h"
#include "boot_logger.h"
#include "boot_flash_dev_if.h"
#include "boot_dload.h"
#include "boot_pbl_v1.h"
#include "boot_target.h"
#include "boot_shared_imem_cookie.h"
#include "boot_extern_busywait_interface.h"
#include "boot_cache_mmu.h"


/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  boot_load_image_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the specified image header from
*   the boot device to a given ram location. 
* 
* @param[in]  image_id   Image ID of the image header to be loaded
* @param[out] header_ptr Pointer to location to load the image header to
*
* @par Dependencies
*   For GEN_IMG type and for GPT partition to be loaded, 
*   partition id should have been set by boot_flash_configure_target_image
*   prior to calling this API.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_load_image_header
(
  image_type image_id,
  mi_boot_image_header_type *header_ptr      /* image header       */
)
{
  boot_flash_trans_if_type *trans_if;      /* translation interface */
  boolean success;
  
  if ( header_ptr != NULL )
  {
    /* Create a flash translation interface for copying the image
       from flash to RAM */
    trans_if = boot_flash_dev_open_image( image_id );
    BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR );
    
    /* Allow the address range of the header structure to be written to */
    boot_clobber_add_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                 header_ptr, sizeof(*header_ptr) );
    
    /* Read the image header from the base of the image/partition */
    success = boot_flash_trans_read( trans_if, 
                                     header_ptr,
                                     0 /* offset */, 
                                     sizeof(*header_ptr) );
    
    /* Remove the local hole */
    boot_clobber_remove_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                    header_ptr, sizeof(*header_ptr) ); 

    BL_VERIFY( success, BL_ERR_OTHERS );

    /* Release the translation layer resource */
    boot_flash_dev_close_image( &trans_if );
  }
} /* boot_load_image_header() */


/*===========================================================================
**  Function :  boot_load_header_and_image
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads in the specified image and image header from
*   the boot device to RAM.  The header_ptr parameter is optional, if
*   not NULL it will be filled in with the image header data.
* 
* @param[in]  image_id   Image ID of the image to be loaded
* @param[out] header_ptr Pointer to the image header that is read from the image
*
* @par Dependencies
*   For GEN_IMG type and for GPT partition to be loaded, 
*   partition id should have been set by boot_flash_configure_target_image
*   prior to calling this API.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_load_header_and_image
(
  image_type image_id,
  mi_boot_image_header_type *header_ptr      /* image header       */
)
{
  mi_boot_image_header_type image_header;  /* temporary header */
  boot_flash_trans_if_type *trans_if;      /* translation interface */
  uint32 image_offset = 0;                 /* offset to start reading the image */
  boolean success;
  uint32 derived_image_size;

  /* Kick the dog before we start */
  SBL_KICK_WATCHDOG();

  /* Use the local image header structure if the argument is NULL */
  if ( header_ptr == NULL )
  {
    header_ptr = &image_header;
  }

  /* Create a flash translation interface for copying the image
   * from flash to RAM */
  trans_if = boot_flash_dev_open_image( image_id );
  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR );

  /* First try to load the image header from the MIBIB.  This will succeed
   * on a NOR non-partition table device, for example. */
  success = boot_flash_dev_load_mibib_header( image_id, header_ptr );

  /* If we couldn't load the header from the MIBIB, assume that the header
   * is located at the base of the image/partition */
  if ( ! success )
  {
    /* Allow the address range of the header structure to be written to */
    boot_clobber_add_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                 header_ptr, sizeof(*header_ptr) );

    /* Read the image header from the base of the image/partition */
    success = boot_flash_trans_read( trans_if, header_ptr,
                                     0 /* offset */, sizeof(*header_ptr) );

    /* Remove the local hole */
    boot_clobber_remove_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                    header_ptr, sizeof(*header_ptr) ); 
    
    /* The GEN_IMG type is defined to add support for loading custom images based
     * on partition ID information utilized in the boot configuration table. The
     * image header of a general image may not necessarily have GEN_IMG as its image
     * ID field so skip the image ID check in this case. */ 
    if ( image_id != GEN_IMG )
    {
      BL_VERIFY( success && ( header_ptr->image_id == image_id ), BL_ERR_OTHERS );
    }

    /* Adjust the image offset to start reading the image data following
    * the image header at the base of the image/partition Image_src indicates
    * the image offset after the header
    */
	BL_VERIFY( ( (uint32)header_ptr->image_src < sizeof(*header_ptr)+(uint32)header_ptr->image_src ),
               BL_ERR_IMG_SIZE_INVALID );
    image_offset = sizeof(*header_ptr) + (uint32)header_ptr->image_src;;
  }

  /* Sanity check the image header. */
  BL_VERIFY( header_ptr->image_size != 0, BL_ERR_IMG_SIZE_ZERO );
 
  /* Check for integer overflow */
  if (((header_ptr->code_size + header_ptr->cert_chain_size) < header_ptr->code_size) ||
      ((header_ptr->code_size + header_ptr->cert_chain_size + header_ptr->signature_size) 
                                                            < header_ptr->signature_size))
  {
    BL_VERIFY( FALSE, BL_ERR_IMG_SIZE_TOO_BIG);
  }
   
  /* Verify the image_size is equal to total of code_size, cert_chain_size
     and signature size. We may have added 1 to 3 bytes to the image if the
     total image size is not multiple of four. We should account for that
     also
  */  
  derived_image_size = header_ptr->code_size +
                       header_ptr->cert_chain_size +
                       header_ptr->signature_size;
  if( derived_image_size % 4 == 0)
  {
    BL_VERIFY( (header_ptr->image_size == derived_image_size),
                BL_ERR_IMG_SIZE_INVALID );
  }
  else
  {
    BL_VERIFY( (header_ptr->image_size ==
               (derived_image_size + 4 - (derived_image_size % 4))),
                BL_ERR_IMG_SIZE_INVALID );
  }

  /* Kick the dog before we start */
  SBL_KICK_WATCHDOG();

  /* Copy the whole image from flash to RAM */
  success = boot_flash_trans_read( trans_if,
                                   header_ptr->image_dest_ptr,
                                   image_offset,
                                   header_ptr->image_size );
  
  BL_VERIFY( success, BL_ERR_OTHERS );

  /* Release the translation layer resource */
  boot_flash_dev_close_image( &trans_if );

} /* boot_load_header_and_image() */

/*===========================================================================
**  Function :  boot_check_fota_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This function opens FOTA partition and checks if we need to boot into 
*   recovery mode
*
* @param[in] uint8 * - Partition ID for FOTA
*
* @par Dependencies
*   None
* 
* @retval
*   boolean - TRUE - recovery mode
*             FALSE - normal mode
* 
* @par Side Effects
*   None
* 
*/
boolean boot_check_fota_partition
( 
  uint8 * img_partition_id
)
{
   boot_flash_trans_if_type *trans_if;      /* translation interface */
   static boolean success = FALSE, done = FALSE;
   uint32 fota_mode;

   do
   {
      /* If we did the check before, we do not want to re-open the partition and 
       read it again. Just use the value from the last read */
      if(done == TRUE)
        break;

      /* Configure the target image */
      boot_flash_configure_target_image(img_partition_id);
    
      /* Initialize flash translation interface to read FOTA partition */
      trans_if = boot_flash_dev_open_image(GEN_IMG);
   
      /* If partition not found, boot normally */
      if(trans_if == NULL)   
      {
        success = FALSE;
        break;
      }

      /* Allow the address range of fota_mode variable to be written to */
      boot_clobber_add_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                   &fota_mode, sizeof(fota_mode) );

      /* Read the recovery cookie from the base of the partition */
      success = boot_flash_trans_read( trans_if, 
                                       &fota_mode,
                                       0 /* offset */, 
                                       sizeof(fota_mode) );   

      /* Remove the local hole */
      boot_clobber_remove_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                      &fota_mode, sizeof(fota_mode) ); 

      BL_VERIFY( success, BL_ERR_OTHERS );

      /* close device handle associated with image_id */
      boot_flash_dev_close_file( GEN_IMG );

      /* Release the translation layer resource */
      boot_flash_dev_close_image( &trans_if );

      /* Check if the cookie is present */
      success = (fota_mode == FOTA_RECOVERY_COOKIE) ? TRUE : FALSE;

    } while(0);

   done = TRUE;
   return success;
}


/*===========================================================================
**  Function :  boot_load_ssd_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the Secure Software Download encryption keys into a 
*   pre-determined location in memory to be used by TrustZone. This function 
*   will read SCL_SSD_PARTITION_SIZE bytes from the the SSD partition, and move
*   the contents into memory at SCL_SSD_PARTITION_BASE. If either of these 
*   definitions do not exist, the function does nothing. 
*
* @param[in] bl_shared_data Pointer to the shared data 
*
* @par Dependencies
*   SCL_SSD_PARTITION_SIZE and SCL_SSD_PARTITION_BASE must be defined.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_load_ssd_partition( bl_shared_data_type *bl_shared_data )
{
#if (defined(SCL_SSD_PARTITION_SIZE) && (defined(SCL_SSD_PARTITION_BASE)))
  boot_flash_trans_if_type *trans_if = NULL;      
  boolean success = FALSE;
  extern uint8 ssd_partition_id[];
  
  if (SCL_SSD_PARTITION_SIZE != 0)
  {
    /* Configure the target image as SSD*/
    boot_flash_configure_target_image(ssd_partition_id);
    
    /* Initialize flash translation interface to read SSD partition */
    trans_if = boot_flash_dev_open_image(GEN_IMG);

    BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR );

    /* Copy SCL_SSD_PARTITION_SIZE bytes into memory at SCL_SSD_PARTITION_BASE */
    success = boot_flash_trans_read(trans_if,
                                    (uint8 *)SCL_SSD_PARTITION_BASE,
                                    0,
                                    SCL_SSD_PARTITION_SIZE);

    BL_VERIFY( success, BL_ERR_OTHERS );

    /* Release the translation layer resource */
    boot_flash_dev_close_image( &trans_if );
  }
#endif
}
