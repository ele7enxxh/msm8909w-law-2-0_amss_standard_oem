/*=============================================================================

                     Boot Loader Clobber Protection

GENERAL DESCRIPTION
  This function contains the implementation for the boot-specific clobber
  protection module, to facilitate preventing the bootloader address spaces
  from being overwritten by a flash load.

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_clobber_prot.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/25/14   jz      Added boot_clobber_remove_global_protection_regions
11/10/14   aus     Fixed integer overflow checks
04/11/14   ck      Updated logic in boot_clobber_add_global_protection_region to
                   prevent duplicate entries.
10/17/13   ck      Changed SCL_BOOT_REGION_BASE to SCL_SBL1_DDR_DATA_BASE.
                   There is no reason to have two or three names for the same label.
09/03/13   aus     Removed clobber check on removal of whitelist table entry
                   to support when the entry does not exist
06/05/13   aus     Added support for clearing memory of a clobber table 
06/28/12   kedara  Added Null pointer check for whitelist table.
01/17/12   aus     Added support for importing whitelist table
12/01/11   aus     Fixed whitelist check
09/28/11   aus     Added whitelist table for clobber protection
02/06/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_clobber_prot.h"
#include "boot_util.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Data structure defining an array of memory regions to protect */
static boot_clobber_global_type clobber_table = { 0 };
  
/* Data structure defining an array of memory regions that can be written to */
static whitelist_region_type *whitelist_table = NULL; 
  
/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

**  Function :  boot_clobber_add_global_protection_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to add a region of memory to the global bootloader overwrite
*   protection.
* 
* @param[in] start_addr Start address if the region to protect
* @param[in] size Size in bytes of the region to protect
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if there was space left in the global array, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
boolean boot_clobber_add_global_protection_region
(
  const void *start_addr,  /* Start address if the region to protect */
  uint32 size              /* Size in bytes of the region to protect */
)
{
  boolean success = FALSE;
  uint32 current_entry = 0;

  /* First make sure the region size does not wrap the 32-bit address space;
   * consider this an error if it does */
  if ( ((uint32) start_addr + size) < (uint32) start_addr )
  {
    BOOT_CLOBBER_VERIFY( FALSE );
  }  
  
  /* If we have room left in the array, add to the end of the array */
  if ( clobber_table.num_entries < BOOT_CLOBBER_MAX_PROTECTED_REGIONS )
  {
    /* Don't add an entry for a region already protected. */
    for (current_entry = 0;
         current_entry < clobber_table.num_entries;
         current_entry++)
    {
      if ((start_addr >= clobber_table.region[current_entry].start_addr) &&
         (((uint32)start_addr + size) <= ((uint32)(clobber_table.region[current_entry].start_addr) +
                                           clobber_table.region[current_entry].size)))
      {
        break;
      }
    }

    /* If current_entry equals number of existing entries then
       existing region was not found and new entry should be added. */
    if (current_entry == clobber_table.num_entries)
    {
      clobber_table.region[clobber_table.num_entries].start_addr =
        (const uint8*) start_addr;
      clobber_table.region[clobber_table.num_entries].size = size;
      clobber_table.num_entries++;
    }

    /* No matter if we have to add the entry, or the entry was already there
       it's a success as the region is covered. */
    success = TRUE;
  }


  BOOT_CLOBBER_VERIFY( success );
  return success;
} /* boot_clobber_add_global_protection_region() */

/*=========================================================================

**  Function :  boot_clobber_remove_global_protection_regions

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to remove a region (regions) of memory from the global bootloader overwrite
*   protection memory regions if falls in the specified range
* 
* @param[in] start_addr Start address of the range
* @param[in] size Size in bytes of the range
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
void boot_clobber_remove_global_protection_regions
(
  const void *start_addr,  /* Start address of the region to check */
  uint32 size              /* Size in bytes of the region to check */
)
{
  uint32 i =0, j = 1;

  /* First make sure the region size does not wrap the 32-bit address space;
   * consider this an error if it does */
  if ( ((uint32) start_addr + size) < (uint32) start_addr )
  {
    BOOT_CLOBBER_VERIFY( FALSE );
  }

  /* Loop through the global array */
  while ( i < clobber_table.num_entries )
  {
    /* If a protected region falls inside the given address range, remove the region */
    if ( ( (const uint8*) start_addr <= clobber_table.region[i].start_addr) &&
         ( (const uint8*) start_addr + size >=
           clobber_table.region[i].start_addr + clobber_table.region[i].size) )
    {
      while ( j < clobber_table.num_entries )
      {
        clobber_table.region[j-1].start_addr = clobber_table.region[j].start_addr;
        clobber_table.region[j-1].size = clobber_table.region[j].size;
        j++;
      }
	  
      clobber_table.region[j-1].start_addr = 0;
      clobber_table.region[j-1].size = 0;
      clobber_table.num_entries--;
      j = i + 1;
    }
    else
    {
      i++; j++;
    }
  }
} /* boot_clobber_remove_global_protection_regions() */

/*=========================================================================

**  Function :  boot_clobber_check_global_address_range

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to check that the given address does not overlap any of the
*   protection regions of memory.
* 
* @param[in] start_addr Start address if the region to check
* @param[in] size Size in bytes of the region to check
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the address range does not overlap a protected region,
*   FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
boolean boot_clobber_check_global_address_range
(
  const void *start_addr,  /* Start address if the region to check */
  uint32 size              /* Size in bytes of the region to check */
)
{
  uint32 i;
  boolean success = TRUE;

  /* First make sure the region size does not wrap the 32-bit address space;
   * consider this an error if it does */
  if ( ((uint32) start_addr + size) < (uint32) start_addr )
  {
    BOOT_CLOBBER_VERIFY( FALSE );
  }

  /* Loop through the global array */
  for ( i = 0; success && i < clobber_table.num_entries; i++ )
  {
    /* If the given address range overlaps a protected range, return FALSE */
    if ( ( (const uint8*) start_addr <
           clobber_table.region[i].start_addr + clobber_table.region[i].size ) &&
         ( (const uint8*) start_addr + size > 
           clobber_table.region[i].start_addr ) )
    {
      success = FALSE;
    }
  }

  /* If we get through the global array than we have successfully checked
     against all the regions */
  
  BOOT_CLOBBER_VERIFY( success );

  return success;
} /* boot_clobber_check_global_address_range() */

/*=========================================================================

**  Function :  boot_clobber_export_global_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to export the global clobber table structure.
* 
* @par Dependencies
*   None
*   
* @retval
*   Pointer to the global clobber table exported
* 
* @par Side Effects
*   None
* 
*/
const boot_clobber_global_type* boot_clobber_export_global_table( void )
{
  return &clobber_table;
}

/*=========================================================================

**  Function :  boot_clobber_import_global_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to import a clobber table structure.
*
* @param[in] table Pointer to the clobber protection table
* 
* @par Dependencies
*   None
*   
* @retval
*   TRUE if there was space left in the global array, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
boolean boot_clobber_import_global_table
(
  const boot_clobber_global_type *table
)
{
  int i = 0;
  boolean success = TRUE;
  
  while ( table != NULL && success && i < table->num_entries )
  {
    success = boot_clobber_add_global_protection_region( table->region[i].start_addr,
                                                         table->region[i].size );
    i++;
  }

  return success;
}


/*=========================================================================

**  Function :  boot_clobber_remove_global_whitelist_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to remove a region of memory from the global whitelist region
* 
* @param[in] start_addr Start address if the region to protect
* @param[in] size Size in bytes of the region to protect
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the region is successfully removed, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
boolean boot_clobber_remove_global_whitelist_region
(
  const void *start_addr,  /* Start address */
  uint32 size              /* Size in bytes */
)
{
  uint32 i=0;
  boolean success = FALSE;

  do
  {
    /* If the whitelist table is not defined, return success */
    if((whitelist_table == NULL) || 
       ((whitelist_table[0].start_addr == NULL) && 
        (whitelist_table[0].size == NULL))
      )
    {
      success = TRUE;
      break;
    }
      
    /* Ignore if size is 0. Implies there will be nothing written to this region */
    if (size == 0)
    {
      success = TRUE;
      break;
    }
      
    /* First make sure the region size does not wrap the 32-bit address space;
     * consider this an error if it does */  
    if ( (uint32) start_addr + size < (uint32) start_addr )
    {
      success = FALSE;
      break;
    }
      
    /* Loop through the global array to find the region */
    while((whitelist_table[i].start_addr != NULL) || (whitelist_table[i].size != NULL))
    {
      /* If the given address range lies in a particular memory range, return TRUE */
      if (((uint32)start_addr >= whitelist_table[i].start_addr) &&
         (((uint32)start_addr+size) <= (whitelist_table[i].start_addr+whitelist_table[i].size)))
      {          
        /* This entry is for the particular region. Make the size of the region 0, to indicate that 
           the region is not writable */
        whitelist_table[i].size = 0;
        success = TRUE;
        break;
      }
        
      /* Increment index */
      i++;
    }  
  } while(0);
  
  return success;
} /* boot_clobber_remove_global_whitelist_region() */


/*=========================================================================

**  Function :  boot_clobber_check_global_whitelist_range

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to check that the given address lies in the given array of memory
*   regions
* 
* @param[in] start_addr Start address if the region to check
* @param[in] size Size in bytes of the region to check
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the address range lies in the whitelist regions,
*   FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
boolean boot_clobber_check_global_whitelist_range
(
  const void *start_addr,  /* Start address of the region to check */
  uint32 size              /* Size in bytes of the region to check */
)
{
  uint32 i=0;
  boolean success = FALSE;

  do
  {
    /* If the whitelist table is not defined, return success */
    if((whitelist_table == NULL) || 
       ((whitelist_table[0].start_addr == NULL) && 
        (whitelist_table[0].size == NULL))
       )
    {
      success = TRUE;
      break;
    }
      
    /* Ignore if size is 0. Implies there will be nothing written to this region */
    if (size == 0)
    {
      success = TRUE;
      break;
    }
      
    /* First make sure the region size does not wrap the 32-bit address space;
     * consider this an error if it does */  
    if ( (uint32) start_addr + size < (uint32) start_addr )
    {
      success = FALSE;
      break;
    }
  
    /* Loop through the global array */
    while((whitelist_table[i].start_addr != NULL) || (whitelist_table[i].size != NULL))
    {
      /* If the given address range lies in a particular memory range, return TRUE */
          if (((uint32)start_addr >= whitelist_table[i].start_addr) &&
         (((uint32)start_addr+size) <= (whitelist_table[i].start_addr+whitelist_table[i].size)))
      {
        success = TRUE;
        break;
      }
        
      /* Increment index */
      i++;
    }
  } while(0);
  
  /* Check if the check was successful */  
  BOOT_CLOBBER_VERIFY( success );

  return success;
} /* boot_clobber_check_global_whitelist_range() */

/*=========================================================================

**  Function :  boot_clobber_import_whitelist_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to import a clobber whitelist table structure.
*
* @param[in] table Pointer to the clobber whitelist table
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
void boot_clobber_import_whitelist_table
(
  whitelist_region_type *table
)
{
    whitelist_table = table; 
} /* boot_clobber_import_whitelist_table */


/*=========================================================================

**  Function :  boot_clobber_clear_whitelist_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to clear the memory of a whitelist table.
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
void boot_clobber_clear_whitelist_table ( void )
{
    uint32 i=0;

    /* Remove SBL Boot region in DDR if it exists in the whitelist table as we do not 
       want to clear this region */
    boot_clobber_remove_global_whitelist_region ((void*) SCL_SBL1_DDR_DATA_BASE, 
                                                         SCL_SBL1_DDR_DATA_SIZE);

    do 
    {
       /* If the whitelist table is not defined, return */
       if(whitelist_table == NULL)
       {
         break;
       }

       /* Loop through the global array */
       while ((whitelist_table[i].start_addr != NULL) || (whitelist_table[i].size != NULL))
       {
         /* Clear the whitelist table entry */
         qmemset((void *)whitelist_table[i].start_addr, 0x0, whitelist_table[i].size);

         /* Increment index */
         i++;
       }
    } while(0);

} /* boot_clobber_import_whitelist_table */
