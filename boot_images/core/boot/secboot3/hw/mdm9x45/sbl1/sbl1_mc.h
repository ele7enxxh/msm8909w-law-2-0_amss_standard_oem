#ifndef SBL1_MC_H
#define SBL1_MC_H

/*===========================================================================

                      SBL1 Main Control Header File

GENERAL DESCRIPTION
  This file contains the main control for SBL1 execution.

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/sbl1/sbl1_mc.h#1 $ 
$DateTime: 2015/09/01 00:30:35 $ 
$Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
09/08/14     jz      Added support for fast USB enumeration
06/20/14     ck      Renamed sbl1_cleanse_qsee_region to sbl1_cleanse_security_regions
05/14/14     ck      Added sbl1_cleanse_qsee_region
04/14/14     jz      Removed sbl1_populate_initial_mem_map, sbl1_dead_battery_charge
03/26/14     jz      Ported changes to move sbl_save_regs to boot_error_handler.h
02/24/14     jz      Updated for Bear
07/10/13     aus     Added support to save the registers and flush the L2 TCM to DDR
                     on SBL errors
06/19/13     kedara  Initial reversion

===========================================================================*/


/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include BOOT_PBL_H

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
**  Function :  sbl1_main_ctl
** ==========================================================================
*/
/*!
* 
* @brief
*   The RPM Main Controller performs the following functions:
*       - Initializes indirect vector table in IMEM
*       - Always calls the cold boot process. (warm boot not supported)
* 
* @param[in] pbl_shared Pointer to shared data
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function never returns.
* 
*/
void sbl1_main_ctl(boot_pbl_shared_data_type *pbl_shared);


/*===========================================================================
**  Function :  sbl1_boot_logger_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function stops the boot logger and dumps the log information. Should
*   be called right before exiting the bootloader 
*
* @param[in] None
*        
* @par Dependencies
*   Called after sbl1_boot_logger_init()
* 
*/
void sbl1_boot_logger_deinit( void );


/*===========================================================================
**  Function :  sbl1_hw_platform_pre_ddr
** ==========================================================================
*/
/*!
* 
* @brief
*     	Parse cdt table to get start of sw-platform id CDB.
*       Call platform-id API and pass pointer to start of sw-platform id CDB.
*       Call hw_init_smem to store platform id to SMEM
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
void sbl1_hw_platform_pre_ddr(bl_shared_data_type* bl_shared_data);


/*===========================================================================
**  Function :  sbl1_hw_platform_smem
** ==========================================================================
*/
/*!
* 
* @brief
*     	Parse cdt table to get start of sw-platform id CDB.
*       Call platform-id API and pass pointer to start of sw-platform id CDB.
*       Call hw_init_smem to store platform id to SMEM
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
void sbl1_hw_platform_smem(bl_shared_data_type* bl_shared_data);


/*===========================================================================
**  Function :  sbl1_post_ddr_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will initialize the data section in DDR, populate ddr size information
*   then relocate page table from IMEM to DDR and map all memory regions required by SBL1 
*   in page table. After execution of this function SBL1 will enter Sahara in stead of PBL
*   emergency dload mode in case of error
*
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_post_ddr_init(bl_shared_data_type  *shared_data);


/*===========================================================================
**  Function :  sbl1_signal_qsee_sbl_done
** ==========================================================================
*/
/*!
* 
* @brief
*   This api makes a smc call to qsee to signal end of sbl boot.
*   QSEE will jump to the address passed in the second argument.
*   This function will not return!
*
* @param[in] bl_shared_data Pointer to shared data
*        
* @par Dependencies
*  none
*   
*/
void sbl1_signal_qsee_sbl_done( bl_shared_data_type *bl_shared_data );


/*===========================================================================
**  Function :  boot_set_qsee_execute_status
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set the flag to indicate if QSEE image has executed.
*
* @param[in] status Indicates whether QSEE image has executed and is ready.
*        
* @par Dependencies
*   Called after QSEE image has been loaded and executed.
* 
*/
void boot_set_qsee_execute_status(boolean status);


/*===========================================================================
**  Function :  sbl1_efs_handle_cookies
** ==========================================================================
*/
/*!
* 
* @brief
*   Calls efs cookie handling api to perform efs backup/restore action
* 
* @param[in] bl_shared_data Pointer to shared data
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
void sbl1_efs_handle_cookies(bl_shared_data_type* bl_shared_data);


/*===========================================================================
**  Function :  sbl1_tlmm_init
** ==========================================================================
*/
/*!
* 
* @brief
*   SBL1 wrapper to Initialize Tlmm and gpio for low power config
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   must be called after boot_smem_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_tlmm_init(bl_shared_data_type *bl_shared_data);

/*===========================================================================
**  Function :  sbl1_qfprom_test
** ==========================================================================
*/
/*!
* 
* @brief
*   This api invokes qfprom test framework
*
* @param[in] bl_shared_data Pointer to shared data
*        
* @par Dependencies
*  Must be invoked before sbl1_signal_tz_sbl_done
*   
*/
void sbl1_qfprom_test( bl_shared_data_type *bl_shared_data );


/*===========================================================================
**  Function :  sbl1_rpm_sync
** ==========================================================================
*/
/*!
* 
* @brief
*   SBL1 needs to wait for rpm to finish its xpu initialization.
*   We will wait for a certain amount of time and if rpm sync cookie is not set force SBL1 to dload mode
*
*        
* @par Dependencies
*  None
*   
*/
void sbl1_rpm_sync(void);

/*===========================================================================
**  Function :  sbl1_update_all_rollback_fuses
** ==========================================================================
*/
/*!
* 
* @brief
*  Blow version rollback fuses for all images.
*
* @par Dependencies
*  Has to execute *after* sbl1_rpm_sync api. 
*   
*/
void sbl1_update_all_rollback_fuses(void);

/*===========================================================================
**  Function :  sbl1_update_sbl1_rollback_fuse
** ==========================================================================
*/
/*!
* 
* @brief
*   Update the sbl1 roll back protection version fuse
*
*        
* @par Dependencies
*  Must be called after TZ is initialized
*   
*/
void sbl1_update_sbl1_rollback_fuse(bl_shared_data_type *bl_shared_data);


/*===========================================================================
**  Function :  sbl1_cleanse_security_regions
** ==========================================================================
*/
/*!
* 
* @brief
*   Zero out the QSEE and QHEE DDR region for security if needed.
*
* @param[in]
*   bl_shared_data           Pointer to shared data
*        
* @par Dependencies
*   None
*   
*/
void sbl1_cleanse_security_regions(bl_shared_data_type * bl_shared_data);


/*===========================================================================
**  Function :  sbl1_qusb_al_do_fast_enum
** ==========================================================================
*/
/*!
* 
* @brief
*   SBL1 wrapper to check for QUSB fast enumeration   
*
* @param[in]
*   bl_shared_data           Pointer to shared data
*        
* @par Dependencies
*   must be called after sbl1_ddr_data_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_qusb_al_do_fast_enum(bl_shared_data_type *bl_shared_data);

#endif  /* SBL1_MC_H */

