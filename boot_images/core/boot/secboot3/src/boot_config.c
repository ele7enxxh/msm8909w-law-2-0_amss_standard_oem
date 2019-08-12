/**
 * @file boot_config.c
 * @brief
 * Source file for boot configuration logic. For every entry in a particular 
 * configuration table, the boot configuration logic parses the data and executes 
 * appropriately. 
 *
 * The functions defined in this file are:
 *  boot_config_process_bl() 
 *  boot_config_process_entry(). 
 *
 * Please see comments for more details. 
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/23/14   jz      Updated to skip image loading log if image is not loaded
10/23/14   jz      Added back boot_config_update_all_fuse_versions in SBL
07/15/14   jz      Fixed to update QSEE interface properly for flashless boot
06/06/14   ck      Updated NO_FLASH conditionals to use booting_from_storage_media
06/03/14   ck      Added logic to report bytes read when loading elf image
04/10/14   jz      Updated to populate QSEE interface info for both flash/flashless
03/17/14   ck      Removed call to and function boot_config_update_tz_xpu as tz executes after SBL
02/04/14   ck      Removed exit_ptr assignment as it's not needed in Bear family.
12/16/13   ck      Replaced TZ fastcall with stub for future securemsm library call.
12/09/13   jz      Added 64bit elf loading support
12/09/13   ck      Removed boot_config_update_all_fuse_versions as this logic is now in QSEE
12/03/13   ck      Added logic to populate SBL - QSEE interface
10/18/13   dhaval  explicit hash-verify elf-header after hash-table auth
03/19/13   kedara  Fix target_img_partition_id to support uint32 values.
03/11/13   kedara  Enable boot config entries to configure xpu settings per image.
02/19/13   kedara  Added boot_config_update_tz_xpu.
02/14/13   kedara  Added boot_config_update_all_fuse_versions.
11/28/12   dh      Call boot_rollback_update_fuse_version after image is executed
                   to blow roll back protection fuse
11/08/12   kedara  Updates to enable hashing and authentication of elf's.
09/21/12   jz      Fixed redundant call to boot_elf_load_init_hash_segment
09/20/12   kedara  Modified the return value of the auth function
08/17/12   kedara  Support loading/auth of elf images in chunks (ssa).
07/31/12   dh      Klocwork fix
01/17/12   aus     Import whitelist table before loading the image
08/24/11   aus     Added support for sahara secure boot
07/06/11   aus     Clean - up 
06/27/11   aus     Added flashless boot support
05/27/11   kpa     Add boot logger messages
03/18/11   dxiang  Support ELF image loading 
03/04/11   dxiang  Support loading of images with custom partition ID information
01/21/11   dxiang  Initial Creation 

============================================================================
                     Copyright 2014 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include <stdio.h>
#include "boot_config.h"
#include "boot_error_if.h"
#include "boot_loader.h"
#include "boot_authenticator.h"
#include "boot_sbl_shared.h"
#include "boot_sbl_if.h"
#include "boot_logger.h"
#include "boot_flash_dev_if.h"
#include "boot_sahara.h"
#include "boot_hash_buffer.h"
#include "boot_rollback_version.h"
#include "boot_elf_header.h"
#include "boot_sbl_qsee_interface.h"
#include "boot_util.h"

/*==========================================================================
                            FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
**  Function : boot_config_process_entry 
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to process and execute boot code based on information from a 
*   single entry in the configuration table. The current boot configuration
*   logic includes the follow steps for each entry:
*
*   1. Execute pre_procs ~ These are any procedures that should run before
*      the particular image is loaded.
*   2. Check for boot_load_cancel ~ Configuration table entries may optionally
*      define a function to check if loading should be canceled. In this certain
*      scenario, the logic will stop all loading. 
*   3. Load/Authenticate ~ Based on the target image type, we load and authenticate
*      the target image if necessary.
*   4. Execute ~ If the configuration table entry has execute as TRUE, we run a specific
*      execute routine which is defined in the configuration. This happens immediately
*      after loading/authentication and should be mutually exclusive with jumping.
*   5. Execute post_procs ~ These are any procedures that should run after the target
*      image has been loaded/authenticated/executed.
*   6. Jump ~ If the configuration table entry has jump as TRUE, we run a specific
*      jumping routine which is defined in the configuration. This always happens after
*      the post_procs have finished and should be mutually exclusive with executing. 
* 
* @param[in] bl_shared_data - Pointer to the shared data structure
* @param[in] boot_config_entry - One entry in the configuration table
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
static void boot_config_process_entry
(
  bl_shared_data_type * bl_shared_data,
  boot_configuration_table_entry * boot_config_entry
)
{
  /*------------------------------------------------------------------
   Local Variables 
  ------------------------------------------------------------------*/
  uint8 *hash_segment_ptr = NULL;
  mi_boot_image_header_type qc_image_header;
  uint32 config_start_time=0;
  bl_error_type error_status=BL_ERR_NONE;
  boot_images_entry * qsee_interface_image_entry = NULL;
  uint32 bytes_read = 0;
  char bootlog_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];
  qc_image_header.image_dest_ptr = NULL;
 
  
  
  /*------------------------------------------------------------------
   Perform Pre-Procedures 
  ------------------------------------------------------------------*/
  BL_VERIFY(boot_config_entry != NULL, BL_ERR_NULL_PTR );

  boot_do_procedures(bl_shared_data, boot_config_entry->pre_procs);

  /*------------------------------------------------------------------
   Preliminary check if loading should be canceled 
  ------------------------------------------------------------------*/
  if(boot_config_entry->boot_load_cancel != NULL)
  {
    if( boot_config_entry->boot_load_cancel(bl_shared_data) == TRUE )
    {
      boot_config_entry->load = FALSE;
    }
  }

  /*------------------------------------------------------------------
   If whitelist table defined, set the whitelist table for the image
  ------------------------------------------------------------------*/
  if(boot_config_entry->whitelist_table != NULL)
  {
    boot_clobber_import_whitelist_table(boot_config_entry->whitelist_table);
  }
  

  /*------------------------------------------------------------------
   Get the next image entry from the QSEE Interface and fill out
   preliminary info.
   -----------------------------------------------------------------*/
  qsee_interface_image_entry =
    &(bl_shared_data->sbl_qsee_interface.boot_image_entry[bl_shared_data->sbl_qsee_interface.number_images]);

  qsee_interface_image_entry->image_id = boot_config_entry->target_img_sec_type;
  
  bl_shared_data->sbl_qsee_interface.number_images++;


  /*------------------------------------------------------------------
   Image Loading and Authentication
  ------------------------------------------------------------------*/
  /* Reset boot statistics flash byte counter. */
  boot_statistics_reset_flash_byte_counter();

  if (boot_config_entry->load == TRUE)
  {
    config_start_time = boot_log_get_time();
    boot_log_message("Image Load, Start");

    switch(boot_config_entry->target_img_type)
    {
      /* QC Image Type */
      case CONFIG_IMG_QC:

        if(booting_from_storage_media())
        {
          /* For generic image type, configure partition ID of the target image */ 
          if(boot_config_entry->target_img_id == GEN_IMG)
          {
            /* In this scenario, the target image partition ID pointer must be defined */
            BL_VERIFY(boot_config_entry->target_img_partition_id != NULL, BL_ERR_NULL_PTR ); 
			 
            /* Configure the target image using custom partition ID information */ 
            boot_flash_configure_target_image(boot_config_entry->target_img_partition_id);
          }
	  
          /* Load the QC image */ 
          boot_load_header_and_image(boot_config_entry->target_img_id, &qc_image_header);
  
          if(boot_config_entry->auth == TRUE)
          {
            /* Prepare QC header for authentication */
            boot_auth_load_header(&qc_image_header);

            /* Authenticate the image */ 
            error_status = boot_auth_image(bl_shared_data,
                             (uint32)boot_config_entry->target_img_sec_type);
            BL_VERIFY( error_status == BL_ERR_NONE, error_status );
          }
        }
        else
        {
          /* Load image and authenticate it within Sahara for binary images
          Typecast target_img_partition_id to point to uint32 value since image id's in case of
          flashless boot are uint32 values and not strings.
          */
          sbl_sahara_entry(&qc_image_header, 
                           *((uint32*)boot_config_entry->target_img_partition_id), 
                          CONFIG_IMG_QC,
                          (uint32)boot_config_entry->target_img_sec_type,
                          boot_config_entry->auth,
                          boot_config_entry->jump,
                          bl_shared_data);
        }

        /* Copy the secboot_verified_info_type for the loaded image
             to the QSEE Interface image entry */
        qmemcpy(&(qsee_interface_image_entry->image_verified_info),
                    boot_get_sbl_auth_verified_info(bl_shared_data),
                    sizeof(secboot_verified_info_type));

        /* Assign entry point to QSEE Interface image entry */
        qsee_interface_image_entry->entry_point = (uint64)(qc_image_header.image_dest_ptr);

        break;  

      /* ELF Image Type */
      case CONFIG_IMG_ELF:
     
        if(booting_from_storage_media())
        {	  
          /* Enable subsys self auth (ssa) support if applicable. Set auth mode
             according to the config table entry */
          boot_elf_set_auth_and_ssa_enable( boot_config_entry->auth,
                                            boot_config_entry->boot_ssa_enabled);

          /* Target image partition ID pointer must be defined */
          BL_VERIFY(boot_config_entry->target_img_partition_id != NULL, BL_ERR_NULL_PTR );  
		 
          /* Initialize the hash data segment from the ELF image */
          boot_elf_load_init_hash_segment(boot_config_entry->target_img_partition_id,
                                          &qc_image_header); 

          /* For ELF images, authenticate the hash data segment first before loading any segments */ 
          if(boot_config_entry->auth == TRUE)
          {
            /* Prepare QC header for authentication */
            boot_auth_load_header(&qc_image_header);

           /* Get buffer where hash segment is loaded */
            hash_segment_ptr = boot_get_hash_segment_buffer();
            
            /*Update the authentication variables to point to hash segment */
            boot_update_auth_image_info(hash_segment_ptr);
            
            /* Authenticate the image */ 
            error_status = boot_auth_image(bl_shared_data,
                              (uint32)boot_config_entry->target_img_sec_type);
            BL_VERIFY( error_status == BL_ERR_NONE, error_status );
     
			/* set flag to track successful hash-table authentication */
			boot_set_hash_table_auth_result((error_status == BL_ERR_NONE));
			
            /* verify elf-hdr, funciton goes to error_handler if verificaiton 
               fails. */
            boot_elf_verify_elf_hdr();
          }

          /* Load the ELF segments and verify segments hash */ 
          boot_load_elf_image();

          /* Restore to Default state. Turn off subsys self auth mode */
          boot_elf_set_auth_and_ssa_enable(FALSE, FALSE);
		  
          /* Copy the secboot_verified_info_type for the loaded image
              to the QSEE Interface image entry */
          qmemcpy(&(qsee_interface_image_entry->image_verified_info),
                    boot_get_sbl_auth_verified_info(bl_shared_data),
                    sizeof(secboot_verified_info_type));

          /* Assign entry point and e_ident class to QSEE Interface image entry */
          qsee_interface_image_entry->entry_point = boot_get_elf_entry_address();

          qsee_interface_image_entry->e_ident = boot_get_elf_eident_class();

        }
        else
        {
          /* Enable subsys self auth support if applicable */
          if( boot_config_entry->boot_ssa_enabled == TRUE)
          {
            boot_sahara_set_is_ssa_enabled(TRUE);
          }
          /* For ELF images, during flashless boot we do parallel load and authenticate. Hence we 
          do not need to use the authentication routines from flash boot 
          Typecast target_img_partition_id to point to uint32 value since image id's in case of
          flashless boot are uint32 values and not strings.
          */
          sbl_sahara_entry(&qc_image_header, 
                           *((uint32*)boot_config_entry->target_img_partition_id), 
                           CONFIG_IMG_ELF,
                           (uint32)boot_config_entry->target_img_sec_type,
                           boot_config_entry->auth,
                           boot_config_entry->jump,
                           bl_shared_data);	

          /* Restore to Default state. Turn off subsys self auth mode */
          boot_sahara_set_is_ssa_enabled(FALSE);

          /* Copy the secboot_verified_info_type for the loaded image
             to the QSEE Interface image entry */
          qmemcpy(&(qsee_interface_image_entry->image_verified_info),
                    boot_get_sbl_auth_verified_info(bl_shared_data),
                    sizeof(secboot_verified_info_type));

          /* Assign entry point and e_ident class to QSEE Interface image entry */
          qsee_interface_image_entry->entry_point = boot_sahara_get_elf_entry_address();

          qsee_interface_image_entry->e_ident = boot_sahara_get_elf_eident_class();

        }

        break;
    
      /* Binary Image Type */
      case CONFIG_IMG_BIN:
        /* TODO */
        break;

      /* Unsupported Image */
      default:
        BL_ERR_FATAL(BL_ERR_SBL);
    }
  }

  BL_VERIFY(boot_config_entry->target_img_str != NULL,
            BL_ERR_NULL_PTR );
  
  if (boot_config_entry->load == TRUE)
  {
    /* Retreive number of bytes read from flash via boot statistics. */
    bytes_read = boot_statistics_get_flash_byte_counter();

    /* Format number of bytes read into string for boot log. */
    snprintf(bootlog_buffer,
             BOOT_LOG_TEMP_BUFFER_SIZE,
             "(%d Bytes)",
             bytes_read);


  boot_log_delta_time_optional_data(config_start_time,
                                    (char *)boot_config_entry->target_img_str,
                                    bootlog_buffer);
  }
  
  /*------------------------------------------------------------------
   Execute the loaded image 
  ------------------------------------------------------------------*/
  /* Jump and exec properties should be mutually exclusive */ 
  if(boot_config_entry->exec == TRUE && 
     boot_config_entry->jump == FALSE &&
     boot_config_entry->load == TRUE)
  {
    /* Image must be authenticated in order to execute */
    BL_VERIFY(boot_config_entry->auth == TRUE , BL_ERR_SBL); 

    /* Verify valid execute function pointer in the config entry */
    BL_VERIFY(boot_config_entry->exec_func != NULL , BL_ERR_NULL_PTR);

    /* Execute */
    (boot_config_entry->exec_func)(bl_shared_data);
  }
  
  /*------------------------------------------------------------------
   Perform Post-Procedures 
  ------------------------------------------------------------------*/
  boot_do_procedures(bl_shared_data, boot_config_entry->post_procs);
  
  /*------------------------------------------------------------------
   Jump to the loaded image 
  ------------------------------------------------------------------*/
  /* Jump and exec properties should be mutually exclusive */ 
  if(boot_config_entry->jump == TRUE && 
     boot_config_entry->exec == FALSE &&
     boot_config_entry->load == TRUE)
  {
    /* Image must be authenticated in order to jump */
    BL_VERIFY(boot_config_entry->auth == TRUE , BL_ERR_SBL); 

    /* Verify valid jump function pointer in the config entry */
    BL_VERIFY(boot_config_entry->jump_func != NULL , BL_ERR_NULL_PTR);

    /* Jump */
    (boot_config_entry->jump_func)(bl_shared_data);
  }

  return;
}


/*===========================================================================
**  Function :  boot_config_process_bl
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to process and execute boot code based on information from the  
*   configuration table. This parses through the entire table and calls
*   boot_config_process_entry() on each entry corresponding to the host 
*   image in order. 
* 
* @param[in] bl_shared_data - Pointer to the shared data structure
* @param[in] host_img - Image ID of the host boot loader 
* @param[in] boot_config_table - Bootloader specific configuration table
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
void boot_config_process_bl 
( 
  bl_shared_data_type *bl_shared_data, 
  image_type host_img, 
  boot_configuration_table_entry * boot_config_table 
)
{
  boot_configuration_table_entry *curr_entry = NULL;

  BL_VERIFY( bl_shared_data != NULL && boot_config_table != NULL,
             BL_ERR_NULL_PTR);

  /* For every entry in the boot configuration table */
  for(curr_entry = boot_config_table;
      curr_entry->host_img_id != NONE_IMG;
      curr_entry++)
  {
    /* Process entries sequentially only for the specific host_img */
    if(curr_entry->host_img_id == host_img)
    {
      boot_config_process_entry(bl_shared_data,
                                curr_entry);
    }
  }
 
  return;
}

/*===========================================================================
**  Function : boot_config_update_all_fuse_versions 
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to iterate over the boot config table and update version number
*   fuses where applicable. 
* 
* @param[in] host_img: Type of host image
* @param[in] boot_config_table: Pointer to boot config data table
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
void boot_config_update_all_fuse_versions
( 
  image_type host_img, 
  boot_configuration_table_entry * boot_config_table 
)
{
  boot_configuration_table_entry *curr_entry = NULL;

  BL_VERIFY( boot_config_table != NULL, BL_ERR_NULL_PTR);

  /* For every entry in the boot configuration table */
  for(curr_entry = boot_config_table; curr_entry->host_img_id != NONE_IMG; curr_entry++)
  {
    /* Process entries sequentially only for the specific host_img.
      If auth and rollback protection are both enabled, we need
      to blow the roll back protection fuses for that image 
    */
      if( curr_entry->host_img_id == host_img &&
          curr_entry->auth == TRUE && 
          curr_entry->enable_rollback_protection == TRUE)
      {
        BL_VERIFY( boot_rollback_update_fuse_version(curr_entry->target_img_sec_type) 
                  == BL_ERR_NONE, 
                  BL_ERR_CODE_ROLLBACK_VERSION_BLOW_FAIL);
      }
  }
}

