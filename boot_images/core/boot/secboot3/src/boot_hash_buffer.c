
/*===========================================================================

                    BOOT HASH BUFFER DEFINITIONS

DESCRIPTION
 This module Contains buffer to hold the hash segment for loading elf images.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/26/13   ck      New functions added boot_get_hash_buffer_size, boot_set_hash_table_auth_result
                   and boot_get_hash_table_auth_result
12/09/13   jz      Ported from fix for blacklist CR 562437
11/08/12   kedara  Added boot_get_hash_segment_buffer.
11/01/13   plc     Update for Arm6 syntax (no "inline")
09/24/12   kedara  Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "boot_hash_buffer.h"
#include "boot_comdef.h"

/*=============================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains global definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* A global buffer to use for loading hash segment. This is shared by both
  elf loader and sahara modules
 */
#pragma arm section zidata = "BOOT_DDR_ZI_DATA_ZONE"
/* A global buffer to use for loading hash segment. This is shared by both
  elf loader and sahara modules
 */
static uint8 ALIGN(32) boot_hash_segment[HASH_SEGMENT_SIZE];
static boolean is_elf_hash_table_auth_result = FALSE;
#pragma arm section zidata

/*===========================================================================

**  Function : boot_get_hash_segment_buffer

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the address of buffer to be used for loading hash
*    segment
*
* @param[in] 
*  None
* 
* @par Dependencies
*  None
* 
* @retval
*  (uint8 *) Pointer to buffer for loading hash segment.
* 
* @par Side Effects
*    None
* 
*/
uint8 *boot_get_hash_segment_buffer( void )
{
  return boot_hash_segment;
}

/*===========================================================================

**  Function : boot_get_hash_buffer_size

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the size of buffer to be used for loading hash
*    segment
*
* @param[in] 
*  None
* 
* @par Dependencies
*  None
* 
* @retval
*  uint32 size of the hash segment buffer.
* 
* @par Side Effects
*    None
* 
*/
uint32 boot_get_hash_buffer_size( void )
{
  return HASH_SEGMENT_SIZE;
}

/*===========================================================================

**  Function : boot_set_hash_table_auth_result

** ==========================================================================
*/
/*!
* 
* @brief
*    This function sets the hash table authentication result (TRUE, FALSE)
*
* @param[in] 
*  uint32  result  -  caller sets TRUE, FALSE for this result variable 
* 
* @par Dependencies
*  None
* 
* @retval
*  None
* 
* @par Side Effects
*    None
* 
*/
void boot_set_hash_table_auth_result(boolean result)
{
  is_elf_hash_table_auth_result = result;
}

/*===========================================================================

**  Function : boot_get_hash_table_auth_result

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the variable holding status for hash table
*    authentication result (TRUE, FALSE).
*
* @param[in] 
*  None 
* 
* @par Dependencies
*  None
* 
* @retval
*  boolean is_elf_hash_table_auth_result   
* 
* @par Side Effects
*    None
* 
*/
boolean boot_get_hash_table_auth_result( void )
{
  return is_elf_hash_table_auth_result;
}
