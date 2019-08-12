/*=============================================================================
                            
                       Boot PBL Version 1 Header File

GENERAL DESCRIPTION
  This file access to routines and data exported by the PBL to other boot
  loaders.
  
Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_pbl_v1.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/28/14   ck      Added boot_pbl_log_milestones
05/28/14   ck      Added boot_pbl_milestone_names
02/03/12   dh      Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include BOOT_PBL_H
#include "boot_error_if.h"
#include "boot_util.h"
#include "boot_logger.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/* Shared data coming from PBL */
static boot_pbl_shared_data_type   boot_pbl_shared_data;
  

/* Until PBL starts supplying its milestone entry names this array will be used.
   The number of entries here must match the number of milestone entries in
   timestamps array. */
static char * boot_log_pbl_milestone_names[] =
{ "PBL, Start",
  "bootable_media_detect_entry, Start",
  "bootable_media_detect_success, Start",
  "elf_loader_entry, Start",
  "auth_hash_seg_entry, Start",
  "auth_hash_seg_exit, Start",
  "elf_segs_hash_verify_entry, Start",
  "PBL, End",
  "PBL, Delta"
};


/*=============================================================================   
                              FUNCTION DEFINITIONS
=============================================================================*/

/*=========================================================================

**  Function :  boot_pbl_get_pbl_version

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the PBL version from PBL shared data.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_pbl_get_pbl_version
(
  void
)
{
  return boot_pbl_shared_data.pbl_version;
}

/*=========================================================================

**  Function :  boot_pbl_get_clock_speed

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the clock speed set by PBL.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_pbl_get_clock_speed
(
  void
)
{
  return boot_pbl_shared_data.proc_clk_speed;
} /* boot_pbl_get_clock_speed */


/*===========================================================================

**  Function :  boot_pbl_get_flash_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the flash type detected by PBL
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_flash_type
* 
* @par Side Effects
*   None.
* 
*/
boot_flash_type boot_pbl_get_flash_type
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data.flash_shared_data != NULL, 
             BL_ERR_NULL_PTR_PASSED );
	
  return (boot_pbl_shared_data.flash_shared_data->type);
} /* boot_pbl_get_flash_type */


/*===========================================================================

**  Function :  boot_pbl_get_flash_shared_data

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the pointer to the flash related information found
*   by PBL.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer of boot_flash_shared_dev_info_type type
* 
* @par Side Effects
*   None.
* 
*/
boot_flash_shared_dev_info_type* boot_pbl_get_flash_shared_data
(
  void
)
{
  BL_VERIFY( boot_pbl_shared_data.flash_shared_data != NULL, 
             BL_ERR_NULL_PTR_PASSED );

  return (boot_pbl_shared_data.flash_shared_data);
} /* boot_pbl_get_flash_shared_data */


/*===========================================================================

**  Function :  boot_pbl_is_auth_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns whether PBL detected that auth was enabled.
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_boolean
* 
* @par Side Effects
*   None.
* 
*/
boot_boolean boot_pbl_is_auth_enabled(void)
{
  uint32 is_auth_enabled = 0;
  boot_pbl_shared_data.secboot_shared_data->
                        pbl_secboot_hw_ftbl.
                        secboot_hw_is_auth_enabled(SECBOOT_HW_APPS_CODE_SEGMENT, 
                                                   &is_auth_enabled);
  return is_auth_enabled;
}

/*===========================================================================

**  Function :  boot_pbl_interface_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Function that will initialize the target interface so that generic SBL
*   code can use it.
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_boolean
* 
* @par Side Effects
*   None
* 
*/
boot_boolean boot_pbl_interface_init
(
  boot_pbl_shared_data_type   *pbl_shared_data  /* Pointer to data from PBL */
)
{
  if (pbl_shared_data == NULL)
  {
    return FALSE;
  }

  qmemcpy(&boot_pbl_shared_data, pbl_shared_data, sizeof(boot_pbl_shared_data_type));

  return TRUE;
} /* boot_pbl_interface_init */


/*===========================================================================

**  Function :  boot_log_insert_pbl_milestones

** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will parse the PBL timestamp milestones passed to SBL
*   and insert them into the boot log.  Currently PBL's unit of measure is
*   clock ticks.  PBL does not pass the clock frequency yet so it is hard
*   wired to 19.2 Mhz.  Also PBL does not pass the names of each field so for
*   now reference a structure of our own to get the names which will have
*   logic ready for the day PBL starts passing them.
*
* @param[in]
*   boot_pbl_shared_data_type
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
void boot_pbl_log_milestones(boot_pbl_shared_data_type * pbl_shared_data)
{
  int count = 0;
  uint32 pbl_us_value = 0;
  double time_per_tick = 1.0 / (double)PBL_TIMESTAMP_TIMER_FREQ_HZ;


  /* PBL timestamp struct is nothing but uint32's */
  uint32 * pbl_timestamp = (uint32 *)&(pbl_shared_data->timestamps);


  for (count = 0;
       count < (sizeof(pbl_shared_data->timestamps) / sizeof(pbl_timestamp));
       count++, pbl_timestamp++)
  {
    pbl_us_value = (double)(*pbl_timestamp) * time_per_tick * 1000000;

    /*Logs message and time. */
    boot_log_message_raw(boot_log_pbl_milestone_names[count],
                         pbl_us_value,
                         LOG_MSG_TYPE_BOOT,
                         NULL);
  }
} /* boot_pbl_log_milestones */
