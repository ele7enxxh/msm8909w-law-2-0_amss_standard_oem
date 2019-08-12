#ifndef BOOT_RAM_PARTITION_H
#define BOOT_RAM_PARTITION_H

/*===========================================================================

              RAM Partition Table Function Definitions Header File

GENERAL DESCRIPTION
  This header file gives boot specific external definition for RAM 
  Partition Table feature implementation.
  
Copyright 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_ram_partition.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/13   ck      Removing unnecessary macro redirections.
09/12/11   dh      Clean up the address defines
11/03/10   dh      Added IMEM and IRAM size and base
07/21/10   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include RAM_PARTITION_TABLE_H
#include "boot_sbl_if.h"
#include "boot_sbl_shared.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_populate_ram_partition_table

** ==========================================================================
*/
/*!
* 
* @brief
*    Function initializes usable_ram_partition_table in shared memory.
*    Then for each type of memory (system memories,boot memory and appsbl memory),
*    this function adds a partition entry in the ram partition table
*    Once complete, function writes magic numbers and version number into table. 
* 
* @param[in] bl_shared_data Pointer to the shared data structure
*     
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_populate_ram_partition_table ( bl_shared_data_type *bl_shared_data );

  
/*===========================================================================

**  Function :  get_partition_category

** ==========================================================================
*/
/*!
* 
* @brief
*    Given a memory address mem_base function returns the
*    partition_category this address belongs to. This function can only be 
*    used after all system memory partitions are added into the table.
* 
* @param[in]  usable_ram_part_tbl_ptr Pointer to table containing usable memory 
*                                     partitions.
* @param[in]  mem_base                Base of the memory address
* @param[out] partition_category      Returned value of partition category
*   
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS                - if function is successful.
*   RAM_PART_CATEGORY_NOT_EXIST_ERR - if function can't retrieve partition
*                                     category.
* 
* @par Side Effects
*   None
*/
ram_partition_return_type get_partition_category
(
  usable_ram_part_table_t , uint32 , uint32 *
);

/*===========================================================================

**  Function :  add_usable_ram_partitions

** ==========================================================================
*/
/*!
* 
* @brief
*    Function adds boot memory region as a partition into the ram partition table.
*    It should be implemented by target specific funtion
* 
* @param[in] usable_ram_part_tbl_ptr Pointer to the ram partition table that 
*                                    should be populated
*   
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS        - if function is successful.
*   RAM_PART_TABLE_FULL_ERR - if table is full
* 
* @par Side Effects
*   None
*/
ram_partition_return_type add_boot_region_ram_partition
(
  usable_ram_part_table_t
);

#endif /* BOOT_RAM_PARTITION_H */
