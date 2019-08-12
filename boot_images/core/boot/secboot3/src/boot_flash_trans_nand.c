/*=============================================================================

                                   Boot Loader
                         NAND Flash Translation Module

GENERAL DESCRIPTION
  This file contains the implementation for the boot-specific NAND flash
  translation module/layer.

Copyright 2010 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_flash_trans_nand.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/22/14   jz      Remove unused sloppy_read and sloppy_copy related code
01/21/15   jz      Moved the (misplaced) check after clobber check to avoid potential integer overflow
06/30/14   jz      Added workaround for the issue of missing connectivity between QPIC and RPM in Bear h/w
06/06/12   kedara  Fix compilation issue for boot_flash_trans_vtbl_type.
09/21/10   plc     Initial port in for SB3.0
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "boot_flash_target.h"
#include "boot_flash_trans_nand.h"
#include "boot_flash_dev_nand_if.h"
#include "boot_util.h"
#include "miparti.h"
#include "flash.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/* General purpose page buffer, must be defined externally */
extern uint8 bl_page_buf[];


/* Definition of private NAND flash translation module data structure */
typedef struct boot_flash_trans_nand_type
{
  /*----------------------------------- 
   * Public module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_flash_trans_if_type pub_if;

  /* NAND flash module variables */
  struct fs_device_data *nand_if;       /* Pointer to NAND flash driver */
  uint32 region_base_addr;              /* Base offset of the flash region
                                           in bytes */
  uint32 region_size;                   /* Maximum size of the flash region
                                           in bytes */

  /* Cached NAND flash module variables */
  uint32 block_size_bytes_shift_bits;
  uint32 block_count;
  uint32 ru_size_bytes;
  uint32 ru_size_shift_bits;
  
  /* Variables for maintaining a callback function in between page reads */
  boot_flash_trans_copy_callback_func_type copy_callback_func;
  void *copy_callback_data;

  /* Variables to optimize flash offset to physical address conversions */
  uint32 last_converted_logical_offset;  /* The last logical offset that was
                                            converted to a physical address */
  uint32 last_converted_physical_addr;   /* The last physical address that was
                                            converted from a logical offset */
} boot_flash_trans_nand_type;


/* The boot translation factory private data structure */
typedef struct boot_flash_trans_nand_factory_private_type
{
  /*----------------------------------- 
   * Public module interface 
   *   Note: This _must_ be the first element of the structure.
   *---------------------------------*/
  boot_flash_trans_factory_if_type pub_if;

  /* Array of the boot translation private data structures allocated
   * by factory */
  boot_flash_trans_nand_type trans_priv[BOOT_FLASH_TRANS_MAX_INSTANCES];

  /* Array indicating which elements of the boot translation private data
   * array are in use */
  boolean trans_priv_inuse[BOOT_FLASH_TRANS_MAX_INSTANCES];

} boot_flash_trans_nand_factory_private_type;


/* The NAND translation factory private data */
static boot_flash_trans_nand_factory_private_type fact_priv;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

FUNCTION  GET_NEXT_GOOD_FLASH_AREA

DESCRIPTION
  Function to advance the given physical flash address to the start of
  the next good area of flash, by skipping over any bad blocks.

DEPENDENCIES
  None
  
RETURN VALUE
  The size of the next good area of flash if one was found, otherwise zero.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 get_next_good_flash_area
(
  boot_flash_trans_nand_type *trans_priv,  /* Pointer to our private type */
  uint32 *phys_flash_addr                  /* Location where the physical
                                              flash address is stored */
)
{
  /* Get a shorter convenience pointer to the NAND device interface */
  uint32 block, good_flash_addr;
  struct fs_device_data * const nand_if = trans_priv->nand_if;

    
  /* Compute the block number corresponding to the physical flash address */
  block = *phys_flash_addr >> trans_priv->block_size_bytes_shift_bits;


  /* Loop through the bad block check until we find a good block */
  while ( ( block < trans_priv->block_count ) && 
            nand_if->bad_block_check( nand_if, block ) != FS_DEVICE_OK )
  {
      ++block;
  }

      /* make sure that we haven't passed the end of the flash */
      if( block >= trans_priv->block_count )
      {
        /* We have gone past the end of the NAND */
        return 0;
      }

      /* Set the next good address equal to the next good block plus the
         offset into the good block (e.g. FSBL/OSBL images uses the first page
         of every block to store bad block magic) */
      good_flash_addr = ( block << trans_priv->block_size_bytes_shift_bits );
   
  /* If the next good address is greater than the current address, use the
     next good address (this accounts for *phys_flash_addr potentially not
     being block-aligned) */
  if ( good_flash_addr > *phys_flash_addr )
  {
    *phys_flash_addr = good_flash_addr;
  }

  /* Return the size of the remainder of the good block */
  return ( ( block + 1 ) << trans_priv->block_size_bytes_shift_bits ) - 
         *phys_flash_addr;
} /* get_next_good_flash_area() */

/*=========================================================================

FUNCTION  GET_PHYSICAL_FLASH_ADDR

DESCRIPTION
  Function to convert the given logical flash offset to a physical flash
  address, by skipping over any bad blocks.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The physical flash address corresponding to the given logical offset

SIDE EFFECTS
  None

===========================================================================*/
static uint32 get_physical_flash_addr
(
  boot_flash_trans_nand_type *trans_priv,  /* Pointer to our private type */
  uint32 byte_offset                       /* Flash offset to convert to a
                                              physical address */
)
{
  /* Initialize the current physical flash address to the base of the region */
  uint32 phys_flash_addr = trans_priv->region_base_addr;
  /* The size of the current good flash area */
  uint32 area_size;
 
  /* First check if the flash offset we want to convert follows the previous
     flash offset that we converted.  If so, then we can just start where
     the previous logical to physical conversion left off. */
  if ( byte_offset >= trans_priv->last_converted_logical_offset )
  {
    byte_offset -= trans_priv->last_converted_logical_offset;
    phys_flash_addr = trans_priv->last_converted_physical_addr;

    /* Save the flash offset we are converting, to save time if the next
       conversion follows this one */
    trans_priv->last_converted_logical_offset += byte_offset;
  }
  else
  {
    /* Save the flash offset we are converting, to save time if the next
       conversion follows this one */
    trans_priv->last_converted_logical_offset = byte_offset;
  }
  
  /* Keep looping while the remaining flash offset is greater than zero */
  while ( byte_offset > 0 )
  {
    /* Find the next good area of flash (next good block) */
    area_size = get_next_good_flash_area( trans_priv, &phys_flash_addr );
    BFA_NAND_VERIFY2( area_size > 0, NULL );

    /* If the current good area is greater than the remaining flash offset,
       adjust down the current area size */
    if ( area_size > byte_offset )
    {
      area_size = byte_offset;
    }

    /* Subtract the current area size from the flash offset and advance
       the physical flash address */
    phys_flash_addr += area_size;
    byte_offset -= area_size;
  }

  /* Return the physical address and save the converted physical address
     to save time if the next conversion follows this one sequentially. */
  return ( trans_priv->last_converted_physical_addr = phys_flash_addr );

} /* get_physical_flash_addr() */

/*=========================================================================

**  Function :  boot_flash_trans_nand_read

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address
*   to the destination address.  Given the logical range to copy,
*   this function converts that request to page reads and skips
*   bad blocks, while ensuring that the copy does not overwrite
*   a protected bootloader memory region.
* 
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static boolean boot_flash_trans_nand_read
(
  flash_trans_if_type *trans_if,  /* Pointer to the translation interface */
  void   *dest_addr,              /* Destination address */
  uint32  byte_offset,            /* Logical source byte address */
  uint32  size                    /* Size to copy in bytes */
)
{
  /* Cast the public interface to our private type */
  boot_flash_trans_nand_type * const trans_priv =
    (boot_flash_trans_nand_type*)trans_if;
  /* Get a shorter convenience pointer to the flash device interface */
  struct fs_device_data * const nand_if = trans_priv->nand_if;
  uint8 *data_ptr = (uint8*) dest_addr;  /* The current destination pointer */
  uint32 phys_flash_addr;  /* The current physical flash offset in bytes */
  uint8 *callback_dest_addr;  /* The current read pointer to pass to callback */
  uint32 callback_dest_size;  /* The current read size to pass to callback */
  boolean status;
  int rc;

  BFA_NAND_VERIFY( trans_if != NULL );

  /* First make sure the copy is to a valid memory range */
  status = boot_clobber_check_local_address_range( &trans_priv->pub_if.clobber_tbl,
                                                   dest_addr, size );
  BFA_NAND_VERIFY( status );

  if (trans_priv->copy_callback_data)
  {
    /* Update the data_ptr if the region to be read falls within the RPM code or data RAM */ 
    if (((uint32)dest_addr >= SCL_RPM_CODE_RAM_BASE && 
  	((uint32)dest_addr + size) <= (SCL_RPM_CODE_RAM_BASE + SCL_RPM_CODE_RAM_SIZE)) ||
  	((uint32)dest_addr >= SCL_RPM_DATA_RAM_BASE && 
  	((uint32)dest_addr + size) <= (SCL_RPM_DATA_RAM_BASE + SCL_RPM_DATA_RAM_SIZE)))
    {
      data_ptr = trans_priv->copy_callback_data;
    }
  }

  BFA_NAND_VERIFY(((uint32)data_ptr + size) > size);

  /* Next translate the logical flash offset to the physical flash address */
  phys_flash_addr = get_physical_flash_addr( trans_priv, byte_offset );

  /* Copy pages from flash while there is data left to copy */
  while ( size > 0 )
  {
    uint32 area_size, copy_size;
    uint32 page_offset, page, num_pages;

    /* Get the next good area of flash */
    area_size = get_next_good_flash_area( trans_priv, &phys_flash_addr );
    BFA_NAND_VERIFY( area_size > 0 );

    /* Make sure that we have not exceeded to size of the region */
    if ( trans_priv->region_size > 0 )
    {
      BFA_NAND_VERIFY( ( phys_flash_addr + size ) <= 
                       ( trans_priv->region_base_addr + 
                         trans_priv->region_size ) );
    }

    /* Save the flash offset corresponding to the current physical address,
       to save time if the next logical to physical conversion follows
       the current flash offset sequentially */
    trans_priv->last_converted_physical_addr = phys_flash_addr;
    trans_priv->last_converted_logical_offset = byte_offset;
    byte_offset += area_size;
    
    /* Adjust the good area size down if it is larger than the total copy size */
    if ( size < area_size )
    {
      area_size = size;
    }

    /* Save the RAM address and size that we are going to process in this
       iteration of the loop, and pass this to the callback function after
       we have read the pages from NAND */
    callback_dest_addr = data_ptr;
    callback_dest_size = area_size;
    
    /* Decrement the total copy size here while we have the full size of the
       good area */
    size -= area_size;

    /* Compute the page number and offset into the first page based on the 
       current physical flash address */
    page = phys_flash_addr >> trans_priv->ru_size_shift_bits;
    page_offset = phys_flash_addr - ( page << trans_priv->ru_size_shift_bits );

    /* First, if the initial offset is not page-aligned, then we need to load
       the data into a page buffer and copy the data from the buffer into memory */
    if ( page_offset > 0 )
    {
      /* Set the copy size to be the remainder of the first page */
        copy_size = trans_priv->ru_size_bytes - page_offset;
        if ( copy_size > area_size )
        {
          copy_size = area_size;
        }

      /* Copy the partial page to a temporary buffer and then to the dest */
      rc = nand_if->read_page( nand_if, page, bl_page_buf );
      qmemcpy( data_ptr, bl_page_buf + page_offset, copy_size );
    
      /* If all we had to copy was the first partial page, then break
         here explicitly for efficieny */
      if ( size == 0 && area_size == copy_size )
      {
        break;
      }
      area_size -= copy_size;
      data_ptr += copy_size;
      phys_flash_addr += copy_size;
      ++page;      
    }

    /* Next we read as many whole pages as are left in the current good area */
    num_pages = area_size >> trans_priv->ru_size_shift_bits;
    copy_size = num_pages << trans_priv->ru_size_shift_bits;

    /* Perform a multi-page read for contiguous whole pages */
    if ( num_pages > 0 )
    {
      rc = nand_if->read_mpages( nand_if, page, num_pages, data_ptr, NULL );
      BFA_NAND_VERIFY( rc == FS_DEVICE_DONE );

      area_size -= copy_size;
      data_ptr += copy_size;
      phys_flash_addr += copy_size;
      page += num_pages;

    }
    
    /* Next, if there is a partial page left in the good flash area, we need
       to load the data into a page buffer and copy from the buffer into memory */
    if ( area_size > 0 )
    {
      copy_size = area_size;

      /* Copy the partial page to a temporary buffer and then to the dest */
      rc = nand_if->read_page( nand_if, page, bl_page_buf );
      qmemcpy( data_ptr, bl_page_buf, copy_size );

      data_ptr += copy_size;
      phys_flash_addr += copy_size;
    }

    /* If the callback function has been set, call the callback function
       with the location and size of the area of RAM that was just read
       from flash */
    if ( trans_priv->copy_callback_func != NULL )
    {
      trans_priv->copy_callback_func( dest_addr, 
                                      callback_dest_addr, 
                                      callback_dest_size );

      dest_addr = (uint8 *)dest_addr + callback_dest_size;
    }
  }

  return TRUE;
} /* boot_flash_trans_nand_read() */


/*=========================================================================

**  Function :  boot_flash_trans_nand_set_copy_callback

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to set the callback function that will be called intermittently
*   when copying data from flash; the pointer to the last data read from flash
*   and the data size will be passed to the callback when it is called.
*   
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static boolean boot_flash_trans_nand_set_copy_callback
(
  boot_flash_trans_if_type *trans_if, /* Pointer to the translation interface */
  boot_flash_trans_copy_callback_func_type callback_func, /* Callback func ptr */
  void *callback_data                 /* Data to pass to callback function */
)
{
  /* Cast the pointer to the public interface to our private type */
  boot_flash_trans_nand_type * const trans_priv =
    (boot_flash_trans_nand_type*)trans_if;

  /* Verify out input parameter */
  BFA_NAND_VERIFY( trans_if != NULL );

  /* Save the callback function and data */
  trans_priv->copy_callback_func = callback_func;
  trans_priv->copy_callback_data = callback_data;

  return TRUE;
  
} /* boot_flash_trans_nand_set_copy_callback() */


/*=========================================================================

**  Function :  boot_copy_data

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy data from ram_addr to where the callback_data points to.
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
#ifdef BOOT_TEMP_BASE
static void boot_copy_data
(
  void *callback_data,  /* Pointer to data associated with callback instance */
  void *ram_addr,       /* Pointer to last data read from flash */
  uint32 ram_size       /* Size of last data read from flash */
)
{
  if ( callback_data && ram_addr && ((uint32)callback_data != (uint32)ram_addr))
    qmemcpy(callback_data, ram_addr, ram_size);
}
#endif
/*=========================================================================

**  Function :  boot_flash_trans_nand_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize an instance of the boot NAND flash translation module.
* 
* @par Dependencies
*   None
*   
* @retval
*   Pointer to the boot flash translation public interface.
* 
* @par Side Effects
*   None
* 
*/
static void boot_flash_trans_nand_init
(
  boot_flash_trans_nand_type *trans_priv,  /* Pointer to the private boot
                                              NAND flash translation structure */
  uint32 region_base_offset,               /* Base offset of the flash region */
  uint32 region_size                       /* Maximum size of the flash region */
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_TRANS_NAND_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_TRANS_IF_VERSION,
                         BOOT_FLASH_TRANS_NAND_VERSION )
  };
  static boot_flash_trans_vtbl_type vtbl =
  {
    boot_flash_trans_nand_read,
    NULL, /* sloppy_read unused */
    NULL, /* get_logical_addr() not set*/
    boot_flash_trans_nand_set_copy_callback
  };

  BL_VERIFY( trans_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Setup the module version structure and virtual function table */
  trans_priv->pub_if.vsn = &vsn;
  trans_priv->pub_if.vtbl = &vtbl;

  /* Cache the pointer to the NAND flash driver interface */
  trans_priv->nand_if = boot_flash_dev_nand_get_nand_if();
  BFA_NAND_VERIFY2( trans_priv->nand_if != NULL, NULL );

  /* Cache NAND flash module variables */
  trans_priv->ru_size_bytes =
    trans_priv->nand_if->page_size( trans_priv->nand_if );
  trans_priv->ru_size_shift_bits = 
    boot_get_bit_set_position( trans_priv->ru_size_bytes );
  trans_priv->block_size_bytes_shift_bits =
    boot_get_bit_set_position( 
      trans_priv->nand_if->block_size( trans_priv->nand_if ) ) + 
    trans_priv->ru_size_shift_bits;
  trans_priv->block_count =
    trans_priv->nand_if->block_count( trans_priv->nand_if );

  /* Initialize the clobber protection local table */
  boot_clobber_local_init( &trans_priv->pub_if.clobber_tbl );

  /* Store the location and size of the flash region */
  trans_priv->region_base_addr = region_base_offset;
  trans_priv->region_size = region_size;

  /* Initialize the copy callback function pointer and data */
#ifdef BOOT_TEMP_BASE
  trans_priv->copy_callback_func = boot_copy_data;
  trans_priv->copy_callback_data = (uint8 *)BOOT_TEMP_BASE;
#else
  trans_priv->copy_callback_func = NULL;
  trans_priv->copy_callback_data = NULL;
#endif

  /* Reset the flash offset optimization variables */
  trans_priv->last_converted_physical_addr = trans_priv->region_base_addr;
  trans_priv->last_converted_logical_offset = 0;

} /* boot_flash_trans_nand_init() */


/*=========================================================================

**  Function :  boot_flash_trans_nand_factory_open

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to open an instance of the boot translation module.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the opened boot translation interface
* 
* @par Side Effects
*   None
* 
*/
static boot_flash_trans_if_type *boot_flash_trans_nand_factory_open
(
  uint32 region_base_offset,      /* Base offset of the flash region */
  uint32 region_size              /* Maximum size of the flash region */
)
{
  int i;
  boot_flash_trans_nand_type *trans_priv = NULL;  /* Pointer to the private
                                                     data structure */

  for ( i = 0; i < BOOT_FLASH_TRANS_MAX_INSTANCES; i++ )
  {
    if ( !fact_priv.trans_priv_inuse[i] )
    {
      trans_priv = &fact_priv.trans_priv[i];
      fact_priv.trans_priv_inuse[i] = TRUE;
      break;
    }
  }
  BL_VERIFY( trans_priv != NULL, BL_ERR_RESOURCE_NOT_AVAILABLE );

  /* Initialize the NAND translation module */
  boot_flash_trans_nand_init( trans_priv, region_base_offset, region_size );

  return &trans_priv->pub_if;
} /* boot_flash_trans_nand_factory_open() */

/*=========================================================================
**  Function :  boot_flash_trans_nand_factory_close

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to close an instance of the boot translation module.
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
static void boot_flash_trans_nand_factory_close
(
  boot_flash_trans_if_type **p_trans_if
)
{
  int i;

  BL_VERIFY( p_trans_if != NULL, BL_ERR_NULL_PTR_PASSED );

  for ( i = 0; i < BOOT_FLASH_TRANS_MAX_INSTANCES; i++ )
  {
    if ( *p_trans_if == (boot_flash_trans_if_type*) &fact_priv.trans_priv[i] )
    {
      BL_VERIFY( fact_priv.trans_priv_inuse[i], BL_ERR_OTHERS );
      fact_priv.trans_priv_inuse[i] = FALSE;
      *p_trans_if = NULL;
      return;
    }
  }

  /* If we have gotten here then the specified translation interface cannot be
   * found in the array */
  BL_ERR_FATAL( BL_ERR_OTHERS );
} /* boot_flash_trans_nand_factory_close() */

/*=========================================================================

**  Function :  boot_flash_trans_nand_factory_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the NAND boot translation factory.
* 
* @par Dependencies
*   None
*   
* @retval
*   Pointer to the NAND translation factory interface
* 
* @par Side Effects
*   None
* 
*/
boot_flash_trans_factory_if_type* boot_flash_trans_nand_factory_init( void )
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_TRANS_NAND_FACTORY_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_TRANS_FACTORY_IF_VERSION,
                         BOOT_FLASH_TRANS_NAND_FACTORY_VERSION )
  };
  static boot_flash_trans_factory_vtbl_type vtbl = 
  {
    boot_flash_trans_nand_factory_open,
    boot_flash_trans_nand_factory_close
  };
  static boolean init_done = FALSE;

  /* Only initialize the factory module once, if init() is called subsequently
   * it will return a pointer to the live module interface */
  if ( !init_done )
  {
    /* Setup the module version structure and virtual function table */
    fact_priv.pub_if.vsn = &vsn;
    fact_priv.pub_if.vtbl = &vtbl;

    /* Zero out the inuse marker for the private translation structure */
    memset( fact_priv.trans_priv_inuse, 0, sizeof(fact_priv.trans_priv_inuse) );

    init_done = TRUE;
  }

  /* Return pointer to the one NAND translation factory interface */
  return &fact_priv.pub_if;
} /* boot_flash_trans_nand_factory_init() */

