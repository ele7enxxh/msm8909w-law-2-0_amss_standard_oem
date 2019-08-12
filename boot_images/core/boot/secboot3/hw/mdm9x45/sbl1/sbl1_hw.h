#ifndef SBL1_HW_H
#define SBL1_HW_H

/*=============================================================================

                         SBL1 Hardware Initialization

GENERAL DESCRIPTION
  This file does basic hardware initialization at power up.

Copyright 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/sbl1/sbl1_hw.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
09/25/14   jz        Added sbl1_hw_get_usb_speed
04/14/14   jz        Added functions to release RPM out of reset, removed LPASS related functions
06/19/13   kedara    Branch from 9x25 sbl1_config, make changes for 9x35 nand
                     & flashless boot.
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"
#include "qusb_common.h"

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
* 
*/
void sbl1_hw_init(void);


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
void sbl1_ddr_init(void);


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

**  Function :   sbl1_hw_set_boot_done_gpio

** ==========================================================================
*/
/*!
* 
* @brief
*   This function pulls GPIO 68 low to indicate that Bootloader is completed
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
void  sbl1_hw_set_boot_done_gpio( void );

/*===========================================================================

**  Function :  sbl1_hw_write_override_register

** ==========================================================================
*/
/*!
* 
* @brief
*   Writes to the one-time write OVERRIDE register
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
void sbl1_hw_write_override_register( void );

/*===========================================================================

**  Function :  sbl1_hw_release_rpm_proc

** ==========================================================================
*/
/*!
* 
* @brief
*   This function releases the RPM processor out of reset
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
void sbl1_release_rpm_proc( bl_shared_data_type *bl_shared_data );

/*===========================================================================

**  Function :  sbl1_hw_jtag_enable_lpass

** ==========================================================================
*/
/*!
* 
* @brief
*   Check the certificate to see if JTAG over-ride register needs to be 
*   programmed to enable access via JTAG.
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
void sbl1_hw_jtag_enable_lpass( bl_shared_data_type *bl_shared_data );

/*===========================================================================

**  Function :  sbl1_hw_jtag_enable_rpm

** ==========================================================================
*/
/*!
* 
* @brief
*   Check the certificate to see if JTAG over-ride register needs to be 
*   programmed to enable access via JTAG.
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
void sbl1_hw_jtag_enable_rpm
( 
   bl_shared_data_type *bl_shared_data 
);

/*===========================================================================

**  Function :  sbl1_hw_jtag_enable_apps

** ==========================================================================
*/
/*!
* 
* @brief
*   Check the certificate to see if JTAG over-ride register needs to be 
*   programmed to enable access via JTAG.
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
void sbl1_hw_jtag_enable_apps
( 
   bl_shared_data_type *bl_shared_data 
);

/*===========================================================================

**  Function :  sbl1_hw_jtag_enable_modem

** ==========================================================================
*/
/*!
* 
* @brief
*   Check the certificate to see if JTAG over-ride register needs to be 
*   programmed to enable access via JTAG.
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
void sbl1_hw_jtag_enable_modem
(
  bl_shared_data_type *bl_shared_data
);

/*===========================================================================

**  Function :  sbl1_release_modem_proc

** ==========================================================================
*/
/*!
* 
* @brief
*  This function brings modem proc out of reset
* 
* @param[in] 
*  bl_shared_data Pointer to shared data
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void sbl1_release_modem_proc( bl_shared_data_type *bl_shared_data );

/*===========================================================================

**  Function :  sbl1_check_modem_status

** ==========================================================================
*/
/*!
* 
* @brief
*  This function Checks if modem PBL and MBA images are able to boot 
*  successfuly.
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
* 
*/
void sbl1_check_modem_status( void );

/*===========================================================================

**  Function :  sbl1_hw_get_boot_option

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads the fast boot option register. 
*
* @par Dependencies
*   None
* 
* @retval
*   boot_select_option_enum_type as read from the register
* 
* @par Side Effects
*   None
* 
*/
boot_select_option_enum_type sbl1_hw_get_boot_option( void );

/*===========================================================================

**  Function :  sbl1_hw_get_usb_speed

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads the usb speed option register. 
*
* @par Dependencies
*   None
* 
* @retval
*   qusb_max_speed_required_t as read from the register
* 
* @par Side Effects
*   None
* 
*/
qusb_max_speed_required_t sbl1_hw_get_usb_speed(void);

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

/*===========================================================================

**  Function :  sbl1_set_vbar

** ==========================================================================
*/
/*!
* 
* @brief
*  Configure VBAR, vector table base register.
* 
* @param[in] 
*  uint32 vbar_addr:  VBAR address to be set.
* 
* @par Dependencies
*  None
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void sbl1_set_vbar( uint32 vbar_addr );

/*===========================================================================
**  Function :  sbl1_post_ddr_hw_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will initialize hw /modules who have a dependency on 
*   DDR being initialized.
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
void sbl1_post_ddr_hw_init(bl_shared_data_type *bl_shared_data);

#endif /* SBL1_HW_H */
