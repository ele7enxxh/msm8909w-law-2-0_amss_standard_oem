#ifndef SBL1_HW_H
#define SBL1_HW_H

/*=============================================================================

                         SBL1 Hardware Initialization

GENERAL DESCRIPTION
  This file does basic hardware initialization at power up.

Copyright 2010-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/msm8909/sbl1/sbl1_hw.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
02/25/15   tj        Parameter changed from void to shared data
12/03/14   xl        Add sbl1_ddr_set_params_enhance
06/06/13   dh        Add sbl1_hw_get_reset_status
04/03/13   sl        Remove DDR_SPEED_IN_KHZ
08/01/12   dh        Add sbl1_hw_deinit
07/23/12   dh        Add sbl1_set_ddr_training_data and sbl1_ddr_training
06/01/12   dh        Add sbl1_check_device_temp
05/29/12   dh        Remove sbl1_clk_regime_apps_init
10/18/11   dh        Initial revision
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

**  Function :  sbl1_hw_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This function performs hardware initialization.
* 
*   Only common hardware that is needed for flash
*   initialization is initialized here.
* 
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
void sbl1_hw_init(bl_shared_data_type *bl_shared_data);


/*===========================================================================

**  Function :  sbl1_ddr_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initialize DDR device.
* * 
* @par Dependencies 
*  Following Api's need to be called first:
*  	sbl2_hw_init : To have ddr clocks setup.
*  	sbl2_ddr_set_params : To have DDR config parameters setup.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_init(bl_shared_data_type *bl_shared_data);


/*===========================================================================

**  Function :  sbl1_hw_dload_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes the necessary clocks for dload.
*   We only do so when auth is disabled 
* 
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
void sbl1_hw_dload_init(bl_shared_data_type *bl_shared_data);


/*===========================================================================

**  Function :  sbl1_hw_init_secondary

** ==========================================================================
*/
/*!
* 
* @brief
*   This function performs secondary hardware initialization.
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
void sbl1_hw_init_secondary( bl_shared_data_type *bl_shared_data );

/*===========================================================================

**  Function :  sbl1_hw_get_reset_status

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of reset status register saved from this boot
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
uint32 sbl1_hw_get_reset_status( void );

/*===========================================================================

**  Function :  sbl2_ddr_set_params

** ==========================================================================
*/
/*!
* 
* @brief
*   Set DDR configuration parameters.
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
*  	This Api needs to be called :
*  Before: sbl2_ddr_init, sbl2_ddr_init_for_dload and tz_exec_func
*  After: boot_config_data_table_init
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_set_params(  bl_shared_data_type *bl_shared_data  );

/*===========================================================================

**  Function :  sbl1_ddr_set_params_enhance

** ==========================================================================
*/
/*!
* 
* @brief
*   Set DDR DC Timing parameters which are stored in CDB2 and CDB3
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_set_params_enhance(  bl_shared_data_type *bl_shared_data  );

/*===========================================================================

**  Function :  sbl1_check_device_temp

** ==========================================================================
*/
/*!
* 
* @brief
*   This function does a temperature check. If the device is too hot
*   we wait few seconds to allow it to cool below certain threshold.
* 
* @param[in] 
*   None 
*     
* @par Dependencies
*   Must be called after DALSYS_InitMod
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_check_device_temp( void );


/*===========================================================================

**  Function :  sbl1_load_ddr_training_data

** ==========================================================================
*/
/*!
* 
* @brief
*   If ddr training is required, read the ddr training data from partition
*   and pass it to ddr driver.
* 
* @param[in] 
*   None 
*     
* @par Dependencies
*   Must be called before sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_load_ddr_training_data(void);


/*===========================================================================

**  Function :  sbl1_wait_for_ddr_training

** ==========================================================================
*/
/*!
* 
* @brief
*   Execute DDR training sequence. If ddr training data is updated, save it
*   to storage device.
* 
* @param[in] 
*   None 
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
void sbl1_wait_for_ddr_training(void);


/*===========================================================================

**  Function :  sbl1_hw_deinit

** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinit the hardware that's not needed beyond sbl1
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_deinit(void);

#endif /* SBL1_HW_H */
