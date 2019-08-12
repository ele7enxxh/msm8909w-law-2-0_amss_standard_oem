/*=============================================================================

                       Boot Loader Flash Partition

GENERAL DESCRIPTION
  This file implements functions and data used to access partitions 
  for flash parts

Copyright 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_flash_partition.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/04/07   MJS     Ported from MDM7800.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_error_if.h"
#include "boot_flash_partition.h"
#include "miparti.h"
#include <string.h>

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_flash_find_partition_entry

** ==========================================================================
*/
/*!
* 
* @brief
*   This function finds the requested partition table entry within
*   the partition table and returns a pointer to it. 
* 
* @param[in] parti_tbl_ptr Partition table pointer
* @param[in] parti_entry_name Name of partition table entry to find
*											   
* @par Dependencies
*   Partition table must be loaded in RAM.
* 
* @retval
*   Pointer to partition table entry or NULL if not found.
* 
* @par Side Effects
*   Executes the error handler and never returns if an error is detected.
* 
*/
flash_partentry_ptr_type boot_flash_find_partition_entry
(
  flash_partable_t parti_tbl_ptr,           /* Partition table pointer */
  const char *parti_entry_name              /* Name of partition table entry
                                               to find */
)
{
  uint32 i = 0;

  /* Check the input parameters. */
  BL_VERIFY( parti_tbl_ptr != NULL, BL_ERR_NULL_PTR_PASSED );
  BL_VERIFY( parti_entry_name[0] == '0', BL_ERR_WRG_PARTI_ENTRY_NAME_FMT );
  BL_VERIFY( parti_entry_name[1] == ':', BL_ERR_WRG_PARTI_ENTRY_NAME_FMT );

  /* Search the table for the partition entry. */
  for ( i = 0; i < FLASH_NUM_PART_ENTRIES; i++ )
  {
    if( !strcmp( parti_tbl_ptr->part_entry[i].name, parti_entry_name ) )
    {
      break;
    }
  } /* for */

  if ( i < FLASH_NUM_PART_ENTRIES )
  {
    return &parti_tbl_ptr->part_entry[i];
  }
  else
  {
    return NULL;
  }

} /* boot_flash_find_partition_entry() */

