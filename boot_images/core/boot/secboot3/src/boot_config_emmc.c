/*===========================================================================

                    BOOT CONFIG DATA TABLE UPDATE FUNCTION

DESCRIPTION
  Contains function to update config data table with data stored in eMMC

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_config_emmc.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/14   aus     Remove added local clobber holes after reading from flash
08/28/14   sk      8909 SBL NAND changes
06/06/14   ck      Updated NO_FLASH_conditionals to use booting_from_storage_media
06/18/13   sl      Check flash type inside boot_update_config_data_table()
12/12/12   jz      Handle CDT not programmed in eMMC
12/05/12   jz      Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_config_data.h"
#include "boot_util.h"
#include "boot_error_if.h"
#include "boot_sbl_if.h"
#include "boot_logger.h"
#include "boot_flash_dev_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/**
*  size of the cdt header in bytes
*/
#define CDT_HEADER_SIZE sizeof(struct cdt_header)

/* max number of CDBs */
#define MAX_NUM_CDBS 10

/* size of the meta data in bytes */
#define META_DATA_SIZE (MAX_NUM_CDBS * sizeof(struct cdb_meta))

/* buffer to hold initial data (CDT header and meta data) read from eMMC */
static uint8 cdt_buf[CDT_HEADER_SIZE + META_DATA_SIZE];

#if (!defined(BOOT_PRE_SILICON)) && defined(BOOT_ERROR_ON_CDT_READ_FAIL)
static volatile boot_boolean error_on_read_fail = TRUE;
#else
static volatile boot_boolean error_on_read_fail = FALSE;
#endif

/* CDT partition ID */
extern uint8 cdt_partition_id[];

/*===========================================================================

**  Function :  boot_update_config_data_table

** ==========================================================================
*/
/*!
* 
* @brief
*    Read the cdt from eMMC and update the default config_data_table array
* 
* @param[in] cdt_info* pointer to cdt info (its cdt_ptr points to the default CDT)
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
void boot_update_config_data_table(struct cdt_info *cdt_info_ptr)
{ 
  uint32 cdb_start_offset = 0;
  uint32 cdb_size = 0;
  uint32 cdb_meta_offset = 0;
  uint32 cdb_index = 0;
  uint8 *cdb_ptr = NULL;
  struct cdb_meta *cdb_meta_ptr = NULL;
  boot_flash_trans_if_type *trans_if = NULL;

  BL_VERIFY(cdt_info_ptr != NULL && cdt_info_ptr->cdt_ptr != NULL, BL_ERR_NULL_PTR);
  
  do
  {
    if (!booting_from_storage_media())
    {
      /* No flash detected, use defaults */
      return;
    }

    /* Setup for loading CDT image */
    boot_flash_configure_target_image(cdt_partition_id);

    /* Create a flash translation interface for copying the image
       from flash to RAM */
    trans_if = boot_flash_dev_open_image(GEN_IMG);
    if (trans_if == NULL)
    {
      /* CDT not programmed, use defaults */
      return;
    }

    /* Allow the address range of the cdt_buf to be written to */
    boot_clobber_add_local_hole(boot_flash_trans_get_clobber_tbl_ptr(trans_if),
                                cdt_buf, sizeof(cdt_buf));  

    /* Read the CDT header and meta data */
    if (boot_flash_trans_read(trans_if, cdt_buf, 0, sizeof(cdt_buf)) != TRUE)
    {
      /* Only error on read failure if flag is set */
      if(error_on_read_fail == TRUE)
      {
        boot_log_message("Error: eMMC read fail");
        CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);        
      }
      break;
    }

	/* Remove the clobber hole */
    boot_clobber_remove_local_hole(boot_flash_trans_get_clobber_tbl_ptr(trans_if),
                                   cdt_buf, sizeof(cdt_buf)); 	
	
    /* Check the magic number in CDT header */
    if (((struct cdt_header *)cdt_buf)->magic != CONFIG_DATA_MAGIC)
    {
      boot_log_message("Error: CDT is not programmed");	
      CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);
    }
    
    /* Check if the first offset to CDB points to valid address. 
       if it does not, enter error handler */
    cdb_meta_ptr = (struct cdb_meta *)(cdt_buf + CDT_HEADER_SIZE);
    cdb_start_offset = cdb_meta_ptr->offset;
    cdt_info_ptr->size = cdb_start_offset;
    if(cdb_start_offset == 0 || cdb_start_offset >= CONFIG_DATA_TABLE_MAX_SIZE)
    {
      boot_log_message("Error: Platform ID is not programmed");
      CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);
    }
    
    /* Loop through the meta data to 
       locate/read the CDBs and update the default CDT */
    for (cdb_index = 0, cdb_meta_offset = CDT_HEADER_SIZE;
         cdb_meta_offset < cdb_start_offset;
         cdb_index++, cdb_meta_ptr++, cdb_meta_offset+=sizeof(struct cdb_meta))
    {
      /* Copy the meta_data to replace the default CDT directly */
      qmemcpy((cdt_info_ptr->cdt_ptr + cdb_meta_offset),
              cdb_meta_ptr, sizeof(*cdb_meta_ptr));
              
      /* Get the CDB to be updated in the default CDT */
      cdb_ptr = boot_get_config_data_block(cdt_info_ptr->cdt_ptr, 
                                           cdb_index,
                                           &cdb_size);

      /* Allow the address range of the default CDT to be written to */
      boot_clobber_add_local_hole(boot_flash_trans_get_clobber_tbl_ptr(trans_if),
                                  cdb_ptr, cdb_meta_ptr->size);  

      /* Read the CDB data in eMMC to replace the default CDT directly */
      if (boot_flash_trans_read(trans_if,
                                cdb_ptr,
                                cdb_meta_ptr->offset,
                                cdb_meta_ptr->size) != TRUE)
      {
        /* Only error on read failure if flag is set */
        if(error_on_read_fail == TRUE)
        {
          boot_log_message("Error: eMMC read fail");
          CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);        
        }
        break;
      }

	  /* Remove the clobber hole */
	  boot_clobber_remove_local_hole(boot_flash_trans_get_clobber_tbl_ptr(trans_if),
                                  cdb_ptr, cdb_meta_ptr->size); 
								  
      /* update the total size of the cdt */
      cdt_info_ptr->size += cdb_meta_ptr->size;
    }  
  } while (0);
  
  /* Release the translation layer resource */
  boot_flash_dev_close_image(&trans_if);
}
