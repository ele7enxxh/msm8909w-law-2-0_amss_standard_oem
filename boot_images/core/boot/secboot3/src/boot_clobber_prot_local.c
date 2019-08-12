/*=============================================================================

                    Boot Loader Clobber Protection

GENERAL DESCRIPTION
  This function contains the implementation for the boot-specific clobber
  protection module, to facilitate preventing the bootloader address spaces
  from being overwritten by a flash load.

Copyright 2007, 2012, 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_clobber_prot_local.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/10/14   aus     Added integer overflow checks
10/30/14   aus     Added clobber_remove_local_hole API
09/28/11   aus     Added whitelist table for clobber protection
02/06/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_clobber_prot_local.h"

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

**  Function :  boot_clobber_local_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize a local clobber protection data structure.
* 
* @param[in] clobber_tbl_ptr Pointer to the local table
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
void boot_clobber_local_init
(
  boot_clobber_local_type *clobber_tbl_ptr  /* Pointer to the local table */
)
{
  /* All we need to do is set the number of holes to zero */
  if ( clobber_tbl_ptr )
  {
    clobber_tbl_ptr->num_holes = 0;
  }
} /* boot_clobber_local_init() */


/*=========================================================================

**  Function :  boot_clobber_add_local_hole

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to add a local hole to the bootloader overwrite protection regions.
* 
* @param[in] clobber_tbl_ptr Pointer to the local table
* @param[in] start_addr Start address of the local hole to add
* @param[in] size Size of the local hole to add in bytes
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
void boot_clobber_add_local_hole
(
  boot_clobber_local_type *clobber_tbl_ptr,  /* Pointer to the local table */
  const void *start_addr,  /* Start address of the local hole to add */
  uint32 size              /* Size of the local hole to add in bytes */
)
{
  boolean success;

  /* First make sure the region size does not wrap the 32-bit address space;
   * consider this an error if it does */  
  if ( ((uint32) start_addr + size) < (uint32) start_addr )
  {
    BOOT_CLOBBER_VERIFY( FALSE );
  }    
  
  if ( clobber_tbl_ptr && 
       clobber_tbl_ptr->num_holes < BOOT_CLOBBER_MAX_LOCAL_REGION_HOLES )
  {
    /* If we have room left in the array then add this hole to the end */
    clobber_tbl_ptr->region_hole[clobber_tbl_ptr->num_holes].start_addr =
      (const uint8*) start_addr;
    clobber_tbl_ptr->region_hole[clobber_tbl_ptr->num_holes].size = size;
    clobber_tbl_ptr->num_holes++;
    success = TRUE;
  }
  else
  {
    /* If we don't have room left in the local array */
    success = FALSE;
  }

  BOOT_CLOBBER_VERIFY( success );
} /* boot_clobber_add_local_hole() */

/*=========================================================================

**  Function :  boot_clobber_remove_local_hole

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to remove a previously added local hole.
* 
* @param[in] clobber_tbl_ptr Pointer to the local table
* @param[in] start_addr Start address of the local hole to remove
* @param[in] size Size of the local hole to remove in bytes
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
void boot_clobber_remove_local_hole
(
  boot_clobber_local_type *clobber_tbl_ptr,  /* Pointer to the local table */
  const void *start_addr,  /* Start address of the local hole to remove */
  uint32 size              /* Size of the local hole to remove in bytes */
)
{
  boolean success = FALSE;
  
  do 
  {
    /* No hole to remove */
    if(size == 0)
    {
        success = TRUE;
        break;
    }

    /* Check validity of the clobber table */ 
    if ((clobber_tbl_ptr == NULL) || (clobber_tbl_ptr->num_holes == 0))
    {
        success = FALSE;
        break;
    }
    
    /* Assume that the last added local hole is the one that needs to be removed */
    if ((clobber_tbl_ptr->region_hole[clobber_tbl_ptr->num_holes-1].start_addr ==
        (const uint8*) start_addr) && 
        (clobber_tbl_ptr->region_hole[clobber_tbl_ptr->num_holes-1].size == size))
    {
        /* Remove the last entry */
        clobber_tbl_ptr->num_holes = clobber_tbl_ptr->num_holes - 1;
        success = TRUE;
        break;     
    }
    else
    {
        success = FALSE;
        break;
    }

  } while(0);

  BOOT_CLOBBER_VERIFY( success );
} /* boot_clobber_remove_local_hole() */


/*=========================================================================

**  Function :  boot_clobber_check_local_address_range

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to check that the given address range either fits inside one of
*   the local holes defined in the protected address regions, or that the
*   given address range does not overlap any of the protected address regions.
* 
* @param[in] clobber_tbl_ptr Pointer to the local table
* @param[in] start_addr Start address of the region to check
* @param[in] size Size of the region to check in bytes
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the given address range completely falls within a local hole or
*   if the given address range does not overlap with a protected region,
*   FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
boolean boot_clobber_check_local_address_range
(
  boot_clobber_local_type *clobber_tbl_ptr,  /* Pointer to the local table */
  const void *start_addr,  /* Start address of the region to check */
  uint32 size              /* Size of the region to check in bytes */
)
{
  uint32 i;

  /* Verify our input parameter */
  BOOT_CLOBBER_VERIFY( clobber_tbl_ptr != NULL );

  /* First make sure the region size does not wrap the 32-bit address space;
   * consider this an error if it does */  
  if ( ((uint32) start_addr + size) < (uint32) start_addr )
  {
    BOOT_CLOBBER_VERIFY( FALSE );
  }  
  
  /* Check to see if the address range fits inside any of the locally defined
     holes within the protected space */
  for ( i = 0; i < clobber_tbl_ptr->num_holes; i++ )
  {
    if ( ( (const uint8*) start_addr >=
           clobber_tbl_ptr->region_hole[i].start_addr ) &&
         ( (const uint8*) start_addr + size <= 
           ( clobber_tbl_ptr->region_hole[i].start_addr +
             clobber_tbl_ptr->region_hole[i].size ) ) &&
         ( size <= clobber_tbl_ptr->region_hole[i].size ) )
    {
      /* If the address range to check fits, then return TRUE */
      return TRUE;
    }
  }

  /* Check the whitelist table to see if the entry exists */
  boot_clobber_check_global_whitelist_range( start_addr, size ); 

  /* If we get here then the address range does not fit inside any of the holes,
     and so we need to make sure that it does not overlap any of the 
     protected regions. */
  return boot_clobber_check_global_address_range( start_addr, size );
  
} /* boot_clobber_check_local_address_range() */

