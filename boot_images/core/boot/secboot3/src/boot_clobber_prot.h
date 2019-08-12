#ifndef BOOT_CLOBBER_PROTECTION_H
#define BOOT_CLOBBER_PROTECTION_H

/*=============================================================================

                     Boot Loader Clobber Protection

GENERAL DESCRIPTION
  This function contains the implementation for the boot-specific clobber
  protection module, to facilitate preventing the bootloader address spaces
  from being overwritten by a flash load.

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_clobber_prot.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/25/14   jz      Added boot_clobber_remove_global_protection_region
04/11/14   ck      Increased BOOT_CLOBBER_MAX_PROTECTED_REGIONS
06/05/13   aus     Added support for clearing memory of a clobber table 
01/17/12   aus     Added support for importing whitelist table
09/28/11   aus     Added whitelist table for clobber protection
08/04/10   aus     Clean-up for secboot 3
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_msm.h"
#include "boot_target.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* This defines the maximum number of protected regions in the global
   protection array */
#ifndef BOOT_CLOBBER_MAX_PROTECTED_REGIONS
#define BOOT_CLOBBER_MAX_PROTECTED_REGIONS 32
#endif /* BOOT_CLOBBER_MAX_PROTECTED_REGIONS */

#ifndef WHITELIST_REGION_TABLE
#define WHITELIST_REGION_TABLE 0
#endif /* WHITELIST_REGION_TABLE */

typedef struct boot_clobber_global_type
{
  uint32 num_entries;  /* Number of protected regions present in the array */
  
  struct
  {
    const uint8 *start_addr;  /* The start address of a protected region */
    uint32 size;              /* The size of a protected region          */
  } region[BOOT_CLOBBER_MAX_PROTECTED_REGIONS];
  
} boot_clobber_global_type;

typedef struct whitelist_region 
{ 
      uint32 start_addr;
      uint32 size;
} whitelist_region_type;

/*=========================================================================

**  Macro :  boot_clobber_verify

** ==========================================================================
*/
/*!
* 
* @brief
*   Macro to verify a condition and transition to an error state if the
*   condition is not met.  Default error bahavior is to return FALSE to
*   the caller.  This behavior can be over-ridden by re-defining the macro
*   in BOOT_MSM_H.
* 
* @par Dependencies
*   None
*   
* @retval
*   FALSE if the condition was not met.
* 
* @par Side Effects
*   None
* 
*/
#ifndef BOOT_CLOBBER_VERIFY
#define BOOT_CLOBBER_VERIFY( cond ) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return FALSE; \
    } \
  } while(0)
#endif /* BOOT_CLOBBER_VERIFY */

/*=========================================================================

**  Macro :  boot_clobber_global_addr_check

** ==========================================================================
*/
/*!
* 
* @brief
*   Macro to perform a global address verification via macro test.  This 
*   macro is primarily for backwards-compatibility with existing QCSBL
*   functionality.  The default behavior is to do nothing, but this can
*   be over-ridden by re-defining the macro in BOOT_MSM_H.
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
#ifndef BOOT_CLOBBER_GLOBAL_ADDR_CHECK
#define BOOT_CLOBBER_GLOBAL_ADDR_CHECK( addr )
#endif /* BOOT_CLOBBER_GLOBAL_ADDR_CHECK */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
);

/*=========================================================================

**  Function :  boot_clobber_remove_global_protection_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to remove a region (regions) of memory from the global bootloader overwrite
*   protection memory regions.
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
  const void *start_addr,  /* Start address if the region to check */
  uint32 size              /* Size in bytes of the region to check */
);

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
);

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
);

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
const boot_clobber_global_type* boot_clobber_export_global_table( void );


/*=========================================================================

**  Function :  boot_clobber_remove_global_whitelist_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to remove a region of memory from the global whitelist region
* 
* @param[in] start_addr Start address of the region to protect
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
);


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
);

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
);


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
void boot_clobber_clear_whitelist_table 
( 
  void 
);

#endif /* BOOT_CLOBBER_PROTECTION_H */

