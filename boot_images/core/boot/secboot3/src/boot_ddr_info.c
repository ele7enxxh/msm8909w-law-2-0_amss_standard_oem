/*=============================================================================

                       Boot DDR Information

GENERAL DESCRIPTION
  This module implements functions that update and share ddr info in SBL

Copyright 2010-2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_ddr_info.c#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

when       who             what, where, why
--------   ---         --------------------------------------------------
06/14/13   dh          Update SBL_DDR_SIZE to MAX_DDR_INTERFACE_NUM
05/23/11   plc         Add support for retrieving partition information 
                       for each DDR device
07/22/10   dh          Initial version created by Kedar  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_ddr_info.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#define MAX_DDR_INTERFACE_NUM     8

/* Structure to store DDR information*/
static sbl_if_shared_ddr_device_info_type boot_ddr_size_info;
boolean boot_ddr_info_set = FALSE;

/*=========================================================================
                            
                       FUNCTION DEFINITIONS

=========================================================================*/
/*===========================================================================

**  Function :  boot_share_ddr_info

** ==========================================================================
*/
/*!
* 
* @brief
*    This function gets the ddr information from ddr driver and adds it to 
*    shared data structure
* 
* @param[out] shared_dev_info Pointer to the shared device information
* @param[in] ddr_available Pointer to the DDR size information
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
void boot_share_ddr_info
(
  sbl_if_shared_ddr_device_info_type *shared_dev_info, 
  ddr_size_info *ddr_available
)
{
  static sbl_if_shared_ddr_info_type ddr_info[MAX_DDR_INTERFACE_NUM];
  BL_VERIFY( shared_dev_info != NULL , BL_ERR_NULL_PTR );

  shared_dev_info->ddr_info = ddr_info;
  shared_dev_info->noofddr = 0;
  /* Ensure that "ddr_partition_info*" is NULL for legacy DDR driver backwards compatibility */
  shared_dev_info->ddr_partition_info = NULL;
 
  if(ddr_available->sdram0_cs0 != 0)
  {
    shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->sdram0_cs0;
    shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->sdram0_cs0_addr;
    shared_dev_info->noofddr++;
  }
  if(ddr_available->sdram0_cs1 != 0)
  {
    shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->sdram0_cs1;
    shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->sdram0_cs1_addr;
    shared_dev_info->noofddr++;
  }
  if(ddr_available->sdram1_cs0 != 0)
  {
    shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->sdram1_cs0;
    shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->sdram1_cs0_addr;
    shared_dev_info->noofddr++;
  }
  if(ddr_available->sdram1_cs1 != 0)
  {
    shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->sdram1_cs1;
    shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->sdram1_cs1_addr;
    shared_dev_info->noofddr++;
  }  
}

/*===========================================================================

**  Function :  boot_share_extended_ddr_info

** ==========================================================================
*/
/*!
* 
* @brief
*    This function gets the ddr partition information from ddr driver and 
*    adds it to shared data structure, including number of partitions for 
*    each DDR device. 
* 
* @param[out] shared_dev_info Pointer to the shared device information
* @param[in] ddr_available Pointer to the DDR size information
* @param[in] ddr_available_partition Pointer to DDR partition information
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
void boot_share_extended_ddr_info
(
  sbl_if_shared_ddr_device_info_type *shared_dev_info, 
  ddr_size_info *ddr_available,
  ddr_size_partition *ddr_available_partition
)
{
  static sbl_if_shared_ddr_info_type ddr_info[MAX_DDR_INTERFACE_NUM];
  static sbl_if_shared_extended_ddr_info_type ddr_partition_info[MAX_DDR_INTERFACE_NUM];
  BL_VERIFY( shared_dev_info != NULL , BL_ERR_NULL_PTR );

  shared_dev_info->ddr_info = ddr_info;
  shared_dev_info->ddr_partition_info = ddr_partition_info;
  shared_dev_info->noofddr = 0;
 
  if(ddr_available->sdram0_cs0 != 0)
  {
    shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->sdram0_cs0;
    shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->sdram0_cs0_addr;
    shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].num_partitions = ddr_available_partition->sdram0_cs0.num_partitions;
    shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].sdram_addr = ddr_available_partition->sdram0_cs0.sdram_addr;
    shared_dev_info->noofddr++;
  }
  if(ddr_available->sdram0_cs1 != 0)
  {
    shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->sdram0_cs1;
    shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->sdram0_cs1_addr;
    shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].num_partitions = ddr_available_partition->sdram0_cs1.num_partitions;
    shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].sdram_addr = ddr_available_partition->sdram0_cs1.sdram_addr;
    shared_dev_info->noofddr++;
  }
  if(ddr_available->sdram1_cs0 != 0)
  {
    shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->sdram1_cs0;
    shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->sdram1_cs0_addr;
    shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].num_partitions = ddr_available_partition->sdram1_cs0.num_partitions;
    shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].sdram_addr = ddr_available_partition->sdram1_cs0.sdram_addr;
    shared_dev_info->noofddr++;
  }
  if(ddr_available->sdram1_cs1 != 0)
  {
    shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->sdram1_cs1;
    shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->sdram1_cs1_addr;
    shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].num_partitions = ddr_available_partition->sdram1_cs1.num_partitions;
    shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].sdram_addr = ddr_available_partition->sdram1_cs1.sdram_addr;
    shared_dev_info->noofddr++;
  }  
}

/*===========================================================================

**  Function :  boot_set_ddr_info

** ==========================================================================
*/
/*!
* 
* @brief
*   This function extracts pointer for DDR info from shared data structure.
*   should only be called after the ddr_shared_info pointer inside 
*   bl_shared_data is initialized
*   
* @param[in] bl_shared_data Pointer to the shared data structure 
*
* @par Dependencies
*   None
*   
* @retval
*   pointer to structure containing ddr information (ddr_size_info )
* 
* @par Side Effects
*   None
* 
*/
void boot_set_ddr_info 
( 
  struct bl_shared_data_type *bl_shared_data 
)
{
 
  BL_VERIFY( (bl_shared_data != NULL) &&
               (bl_shared_data->sbl_shared_data != NULL) &&
               (bl_shared_data->sbl_shared_data->ddr_shared_info != NULL), 
                BL_ERR_NULL_PTR_PASSED );
               
  boot_ddr_size_info = *(bl_shared_data->sbl_shared_data->ddr_shared_info);
  boot_ddr_info_set = TRUE;
          
}


/*===========================================================================

**  Function :  boot_get_ddr_info

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns information about DDR.
*  
* @par Dependencies
*   None
*   
* @retval
*   pointer to structure containing ddr information (ddr_size_info )
* 
* @par Side Effects
*   None
* 
*/
sbl_if_shared_ddr_device_info_type *boot_get_ddr_info ( void )
{
  BL_VERIFY( ( boot_ddr_info_set == TRUE), BL_ERR_SBL );
  return &boot_ddr_size_info;
}
