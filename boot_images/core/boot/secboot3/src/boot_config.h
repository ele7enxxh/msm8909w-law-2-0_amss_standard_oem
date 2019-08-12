#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

/**
 * @file boot_config.h
 * @brief
 * Header file for bootloader configuration logic. For every image loaded by
 * a bootloader, a specific configuration table entry must exist which provides
 * specific information unique to the image. In this file we define the structure
 * for the boot configuration table entry as well declaring the externalized function
 * boot_config_process_bl() which should be called once for every bootloader. 
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_config.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/23/14   jz      Added back boot_config_update_all_fuse_versions in SBL
03/17/14   ck      Removed boot_config_update_tz_xpu
12/09/13   ck      Removed boot_config_update_all_fuse_versions as this logic is now in QSEE
03/11/13   kedara  Added defines for per image xpu configuration.
02/19/13   kedara  Added boot_config_update_tz_xpu.
02/14/13   kedara  Added boot_config_update_all_fuse_versions.
10/25/12   dh      Add flag to indicate use version rollback protection or not
08/16/12   kedara  Added flag to load elf in chunks (feature ssa).
06/14/12   kedara  Added whitelist clobber table entry 
03/27/12   dh      include miheader.h instead of secboot_types.h
06/03/11   kedara  Added string variable to aid logger messages identify images
03/04/11   dxiang  Support loading of images with custom partition ID information 
01/21/11   dxiang  Initial Creation 

============================================================================
                     Copyright 2011-2012 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"
#include "boot_mc.h"
#include "boot_sbl_shared.h"
#include "mibib.h"
#include "miheader.h"
#include "boot_clobber_prot.h"

/*==========================================================================
                          PUBLIC DATA DECLARATIONS
===========================================================================*/

/**
 * Image types 
 */
typedef enum
{
  CONFIG_IMG_DEFAULT = ~0,                /**< No specific image type */
  CONFIG_IMG_QC = 0,                      /**< Qualcomm image type */
  CONFIG_IMG_ELF,                         /**< ELF formatted image type */
  CONFIG_IMG_BIN,                         /**< Binary image type */
  CONFIG_IMG_MAX_SIZE = 0x7FFFFFFF
}config_image_type;

/**
 * Boot configuration table entry 
 */
typedef struct
{
  image_type host_img_id;                 /**< Image ID of the host */
  config_image_type host_img_type;        /**< Image type of the host */ 
  image_type target_img_id;               /**< Image ID of the target */ 
  config_image_type target_img_type;      /**< Image type of the target */ 
  secboot_sw_type target_img_sec_type;    /**< Secure Software ID of the target */
  boot_boolean load;                      /**< Target will be loaded */
  boot_boolean auth;                      /**< Target will be authenticated */
  boot_boolean exec;                      /**< Target will execute immediately after
                                               loading/authentication */
  boot_boolean jump;                      /**< Target will be jumped to after
                                               post procedures are complete */ 
  boot_procedure_func_type exec_func;     /**< Function pointer to execute function.
                                               Must be defined if exec is TRUE */ 
  boot_procedure_func_type jump_func;     /**< Function pointer to jump function.
                                               Must be defined if jump is TRUE */ 
  boot_procedure_func_type *pre_procs;    /**< Function pointer array to pre-procedures */
  boot_procedure_func_type *post_procs;   /**< Function pointer array to post-procedures */
  boot_logical_func_type boot_load_cancel;/**< Function pointer to cancel loading */ 
  uint8 * target_img_partition_id;        /**< Target image partition ID information */
  uint8 * target_img_str;                  /**< Target image name information  */
  whitelist_region_type * whitelist_table; /**< Whitelist table */
  boot_boolean boot_ssa_enabled;           /**< Subsystem self authentication (ssa)*/
  boot_boolean enable_rollback_protection; /**< Enable Roll back protection feature or not */
  boot_boolean enable_xpu;                 /**< Enable XPU configuration for the image */
  uint32 xpu_proc_id;                      /**< XPU proc id to be passed to TZ */
  uint32 RESERVED_1;                      /**< RESERVED */
  uint32 RESERVED_2;                      /**< RESERVED */
  uint32 RESERVED_3;                      /**< RESERVED */
  uint32 RESERVED_4;                      /**< RESERVED */
}boot_configuration_table_entry;


/*==========================================================================
                          PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
**  Function :  boot_config_process_bl
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to process and execute boot code based on information from the  
*   configuration table 
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
);

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
* @param[in]
*  None
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
);

/*===========================================================================
**  Function : boot_config_update_tz_xpu 
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to call into TZ to update XPU lock.
* 
* @param[in] proc_id: Processor ID for whose memory region XPU updates
*                     are to be applied.  
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
void boot_config_update_tz_xpu(uint32 proc_id);

#endif /* BOOT_CONFIG_H */
