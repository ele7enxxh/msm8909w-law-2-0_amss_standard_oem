/*=============================================================================

                                Boot Loader
                          NAND Flash Device Module

GENERAL DESCRIPTION
  Contains the implementation for the boot NAND flash device module.

Copyright 2010-2011,2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_flash_dev_nand.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/18/14   lm      code clean-up
08/28/14   sk      8909 SBL NAND changes
09/14/12   kedara  Move page buffer to DDR ZI for BAM based nand driver.
06/05/12   kedara  removed flash_boot_ops_setup.h
09/12/11   dh      Switch smem call to boot extern interface call
08/04/11   aus     Added support for storing partition table to SMEM
09/21/10   plc     Initial port into SB3.0
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_flash_dev_nand.h"
#include "boot_flash_trans_if.h"
#include "boot_error_if.h"
#include "boot_flash_partition.h"
#include "boot_util.h"
#include "miprogressive.h"
#include "miboot.h"
#include "miparti.h"
#include "mibib.h"
#include <string.h>
#include "boot_mc.h"
#include "flash.h"
#include "boot_flash_dev_dal.h"
#include "boot_sbl_if.h"
#include "boot_extern_smem_interface.h"
#include "boot_extern_flash_interface.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define BL_PAGE_NUM_BYTES 4096 /* Get this added to .builds */

#pragma arm section zidata = "BOOT_DDR_ZI_DATA_ZONE"
/* Temporary page buffer to use for partial page reads */
/* Use max page size including spare data */
/* Align page buffer on cache-line size of 32 bytes for DM efficiency */
/* Page buffer to be allocated in DDR. Since BAM cannot access
  it in L2 as TCM.
*/
uint8 __align(32) bl_page_buf[BL_PAGE_NUM_BYTES];
#pragma arm section zidata

/* String that defines the partition name */
char *partition_name = NULL;

/* Definition of private flash NAND device data structure */
typedef struct boot_flash_dev_nand_type
{
  /*-----------------------------------
   * Public module interface
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_flash_dev_nand_if_type       pub_if;

  /* NAND device public structure pointer used for all operations */
  fs_device_t                       nand_if;

  /* Pointer to the partition table */
  flash_partable_t                  partable_ptr;

  /* Pointer to the boot translation layer interface */
  boot_flash_trans_factory_if_type *trans_fact_if;

  /* indicates block size in bytes */
  uint32                            block_size_bytes_shift_bits;

} boot_flash_dev_nand_type;


/* The NAND private data structure */
static boot_flash_dev_nand_type dev_priv;

/* Structure for storage of NAND partition table. */
static struct flash_partition_table mibib_partition_tbl;


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

**  Function :  boot_flash_dev_nand_store_partition_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Store partition table in SMEM
*   
* @par Dependencies
*   SMEM needs to be initialized 
*      
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void boot_flash_dev_nand_store_partition_table
( 
  bl_shared_data_type *bl_shared_data 
)
{
	 flash_partable_t smem_aarm_partition_table;
     uint32 smem_parti_size = sizeof(*smem_aarm_partition_table);
    
     smem_aarm_partition_table = boot_smem_alloc( SMEM_AARM_PARTITION_TABLE,
                                               smem_parti_size );
										  
     BL_VERIFY( smem_aarm_partition_table != NULL, BL_ERR_NULL_PTR_PASSED );
  
     /* Fill the partition table in SMEM */
     boot_flash_boot_ops_get_parti_tbl((uint32 *)smem_aarm_partition_table, &smem_parti_size);
  
     /* Failed to update SMEM with partition table if smem_parti_size is 0 */
     BL_VERIFY( smem_parti_size != 0, BL_ERR_NULL_PTR_PASSED );

} /* boot_flash_dev_nand_store_partition_table() */

/*===========================================================================

**  Function :  boot_flash_dev_nand_load_partition_table

** ==========================================================================
*/
/*!
* 
* @brief
*   This function copies the NAND flash partition table.
* 
* @par Dependencies
*   None
* 
* @retval
*   None 
* 
* @par Side Effects
*   None
* 
*/
flash_partable_t boot_flash_dev_nand_load_partition_table
(
  bl_shared_data_type *bl_shared_data
)
{

  BL_VERIFY(( (bl_shared_data != NULL) &&
             (bl_shared_data->sbl_shared_data != NULL) &&
             (bl_shared_data->sbl_shared_data->sys_partition_pointer != NULL)) ,
              BL_ERR_NULL_PTR_PASSED );

  /* Copy partition table to buffer passed in. */
  qmemcpy( (void *) &mibib_partition_tbl,
          (void *) bl_shared_data->sbl_shared_data->sys_partition_pointer,
          sizeof( struct flash_partition_table ) );

  /* Fail boot if valid partition table does not exist. */
  BL_VERIFY( (mibib_partition_tbl.magic1 == FLASH_PART_MAGIC1) &&
             (mibib_partition_tbl.magic2 == FLASH_PART_MAGIC2) &&
             (mibib_partition_tbl.version == FLASH_PARTITION_VERSION),
             BL_ERR_PARTI_TABLE_NOT_FOUND );

  return &mibib_partition_tbl;

} /* boot_flash_dev_nand_load_partition_table() */


/*=========================================================================

**  Function :  dev_nand_open_image

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize an instance of the boot translation module, using
*   information obtained from the underlying boot device.  The base address
*   and region size used to initialize the translation layer are obtained
*   from the boot device (e.g. partition table) based on the given image type.
* 
* @par Dependencies
*   dev_nand_configure_target_image() needs to be called prior to this function
* 
* @retval
*   Pointer to the boot translation layer public interface.
* 
* @par Side Effects
*   None
* 
*/
static boot_flash_trans_if_type* dev_nand_open_image
(
  image_type image_id                  /* Image type to open */
)
{

  /* Check parameters first */
  BL_VERIFY( partition_name != NULL, BL_ERR_NULL_PTR_PASSED );

  dev_priv.nand_if = flash_open( partition_name, 0 );

  if (dev_priv.nand_if == FS_NO_DEVICE)
    return NULL;

  return boot_flash_trans_factory_open( dev_priv.trans_fact_if,
                                        0,
                                        0 );
} /* dev_nand_open_image() */

/*=========================================================================

**  Function :  dev_nand_open_partition

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize an instance of the boot translation module, using
*   information obtained from the underlying boot device.  The base address
*   and region size used to initialize the translation layer are obtained
*   from the boot device (e.g. partition table) based on the given partition
*   name.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the boot translation layer public interface.
* 
* @par Side Effects
*   None
* 
*/
static boot_flash_trans_if_type* dev_nand_open_partition
(
  const char *parti_name                  /* partition to open */
)
{
  /* Pointer to partition table entry */
  flash_partentry_ptr_type parti_entry_ptr;

  /* First verify our input parameters */
  BL_VERIFY( parti_name != NULL, BL_ERR_WRG_IMGHD_REQUESTED );

  /* Look up the partition table entry corresponding to the partition name */
  parti_entry_ptr =
    boot_flash_find_partition_entry( dev_priv.partable_ptr, parti_name );

  /* If the entry does not exist within the table, fail the boot. */
  BL_VERIFY( parti_entry_ptr != NULL, BL_ERR_PARTI_ENTRY_NOT_FOUND );

  /* Initialize the translation interface based on the partition
     table offset and length */
  return boot_flash_trans_factory_open( dev_priv.trans_fact_if,
                                        ( parti_entry_ptr->offset <<
                                          dev_priv.block_size_bytes_shift_bits ),
                                        ( parti_entry_ptr->length <<
                                          dev_priv.block_size_bytes_shift_bits ) );

} /* dev_nand_open_partition() */

/*=========================================================================

**  Function :  dev_nand_close_image

** ==========================================================================
*/
/*!
* 
* @brief
*   Close an instance of the boot translation layer.
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void dev_nand_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_trans_factory_close( dev_priv.trans_fact_if, p_trans_if );
  BL_VERIFY( (flash_close(dev_priv.nand_if) == 0 ), BL_ERR_BOOT_FLASH_ACCESS );
} /* dev_nand_close_image() */

/*===========================================================================

**  Function :  dev_nand_load_mibib_header

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to load an image header from the MIBIB of the flash device
*   based on the given image type passed in the image header.  This function
*   returns false if the device does not have headers in the MIBIB area.
* 
* @par Dependencies
* 
* @retval
*   FALSE since SB3.0 NAND devices don't have image headers in the MIBIB
* 
* @par Side Effects
*   None
* 
*/
static boolean dev_nand_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *image_header_ptr  /* Pointer to image header */
)
{
  /* SB3.0 NAND devices don't have image headers in the MIBIB */
  return FALSE;

} /* dev_nand_load_mibib_header() */

/*===========================================================================

**  Function :  dev_nand_load_mibib_header_with_cookie

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to load an image header from the MIBIB of the flash device
*   based on the given image type passed in the image header.  This function
*   returns false if the device does not have headers in the MIBIB area.
* 
* @par Dependencies
* 
* @retval
*   FALSE since SB3.0 NAND devices don't have image headers in the MIBIB
* 
* @par Side Effects
*   None
* 
*/
static boolean dev_nand_load_mibib_header_with_cookie
(
  image_type image_id,
  mi_boot_header_cookie_type *image_header_ptr  /* Pointer to image header */
)
{
  /* SB3.0 NAND devices don't have image headers in the MIBIB */
  return FALSE;

} /* dev_nand_load_mibib_header_with_cookie() */

/*===========================================================================


**  Function : pointer  dev_nand_get_nand_if

** ==========================================================================
*/
/*!
* 
* @brief
*   Returns a pointer to the NAND flash driver interface.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the NAND flash driver interface, NULL if an error occurred
* 
* @par Side Effects
*   None
* 
*/
static struct fs_device_data *dev_nand_get_nand_if( void )
{
  return dev_priv.nand_if;

} /* dev_nand_get_nand_if() */

/*===========================================================================

**  Function :  dev_nand_close_file_by_image_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This is a stub function to support eMMC boot from FAT.
* 
* @par Dependencies
*   None
* 
* @retval
*   None.
* 
* @par Side Effects
*   None
* 
*/
static void dev_nand_close_file_by_image_type( image_type image_id )
{
  /* Do nothing */

} /* dev_nand_close_file_by_image_type() */

/*===========================================================================

**  Function :  dev_nand_close_file_by_handle

** ==========================================================================
*/
/*!
* 
* @brief
*   This is a stub function to support eMMC boot from FAT.
* 
* @par Dependencies
*   None
* 
* @retval
*   None.
* 
* @par Side Effects
*   None
* 
*/
static void dev_nand_close_file_by_by_handle( int16 file_handle )
{
  /* Do nothing */

} /* dev_nand_close_file_by_by_handle() */

/*===========================================================================

**  Function :  dev_nand_configure_target_image 

** ==========================================================================
*/
/*!
* 
* @brief
*   Initialize the value of partition_name to the correct partition.
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
*/
static void dev_nand_configure_target_image(uint8 * partition_id)
{

  /* Check parameters first */
  BL_VERIFY( partition_id != NULL, BL_ERR_NULL_PTR_PASSED );  
  partition_name = (char *) partition_id;
  
} /* dev_nand_configure_target_image() */

/*=========================================================================

**  Function :  boot_flash_dev_nand_init_and_register

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize the NAND flash device module
* 
* @par Dependencies
* 
* @retval
*   None
* 
* @par Side Effects
* 
*/
void boot_flash_dev_nand_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  bl_shared_data_type *bl_shared_data
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_DEV_NAND_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_DEV_NAND_IF_VERSION,
                         BOOT_FLASH_DEV_NAND_VERSION )
  };
  static boot_flash_dev_nand_vtbl_type vtbl =
  {
    dev_nand_open_image,
    dev_nand_open_partition,
    dev_nand_close_image,
    dev_nand_close_image,
    dev_nand_load_mibib_header,
    dev_nand_load_mibib_header_with_cookie,
    boot_flash_dev_default_stub,
    boot_flash_dev_default_stub,
    dev_nand_close_file_by_image_type,      
    dev_nand_close_file_by_by_handle,       
    dev_nand_configure_target_image,	
    dev_nand_get_nand_if
  };

  /* First verify our input parameters */
  BL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Setup the module version structure and virtual function table */
  dev_priv.pub_if.vsn = &vsn;
  dev_priv.pub_if.vtbl = &vtbl;

  /* Open the NAND flash driver for access to the whole device.    *
   * which_flash parameter is hard coded to 0.                     *
   *                                                               *
   * Open MIBIB Partition to make sure device exists, and to query *
   * device specific information                                   */
  dev_priv.nand_if = flash_open( MIBIB_PARTI_NAME, 0 );

  BL_VERIFY( ( dev_priv.nand_if != FS_NO_DEVICE ), BL_ERR_BOOT_FLASH_ACCESS );

  /* Calculate additional NAND parameters from data retrieved. */
  dev_priv.block_size_bytes_shift_bits =
    boot_get_bit_set_position( dev_priv.nand_if->block_size( dev_priv.nand_if ) ) +
    boot_get_bit_set_position( dev_priv.nand_if->page_size( dev_priv.nand_if ) );

  /* Register the singletons, do it here before we try to load the partition
   * table */
  boot_flash_dev_if_register_singleton(
    boot_flash_dev_nand_cast_parent_if( &dev_priv.pub_if ) );
  boot_flash_dev_nand_if_register_singleton( &dev_priv.pub_if );

  /* Finally, initialize the translation layer factory */
  dev_priv.trans_fact_if = trans_factory_init();
  BL_VERIFY( dev_priv.trans_fact_if != NULL, BL_ERR_NULL_PTR );

} /* boot_flash_dev_nand_init_and_register() */

