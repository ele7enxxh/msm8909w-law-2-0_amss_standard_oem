#ifndef BOOTHW_TARGET_H
#define BOOTHW_TARGET_H
/*===========================================================================

                        Boot Hardware Target
                          Header File 

DESCRIPTION
 Contains common defintion used by boot downloader.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2014 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boothw_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/15   jz      Expose boot_get_qsee_execute_status
03/18/14   ck      Removed boot_hw_warm_reset as it is redundant
03/18/14   ck      Added enum for reset type and added to boot_hw_reset parameter list
01/14/14   ck      Added boot_switch_to_aarch64
07/24/12   aus     Added boot_hw_warm_reset
04/04/13   dh      Add boot_hw_powerdown
10/26/09   msm     Changes for CMI.
===========================================================================*/

#include "target.h"
#include "comdef.h"

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  BOOT_HARD_RESET_TYPE = 0,
  BOOT_WARM_RESET_TYPE = 1
} boot_hw_reset_type;


/*===========================================================================

  FUNCTION boot_switch_to_aarch64()

  DESCRIPTION
    This function configures and then switches the processor to 64 bit mode
    (AARCH64).  

  PARAMETERS
    entry_point = The address of where the processor should start executing
    from after the switch to AARCH64

    carried_parameter = Parameter that will be passed to entry point after
    the switch to AARCH64.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    This function will never return.
    
===========================================================================*/
void boot_switch_to_aarch64(uint32 entry_point,
                            uint32 carried_parameter);


/*===========================================================================

  FUNCTION boot_hw_reset()

  DESCRIPTION
    This function resets the phone.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
    
===========================================================================*/
void boot_hw_reset(boot_hw_reset_type reset_type);


/*===========================================================================

**  Function :  boot_hw_powerdown

** ==========================================================================
*/
/*!
* 
* @brief
*   This function powers off the PMIC
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   Target is switched off
* 
*/
void boot_hw_powerdown(void);


/*===========================================================================

**  Function :  boot_enable_aarch64

** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the A64 bit in the RMR register.
* 
* @par Parameters
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_enable_aarch64(void);


/*===========================================================================

**  Function :  boot_generate_warm_reset

** ==========================================================================
*/
/*!
* 
* @brief
*   This function causes the processor to generate a warm reset.
* 
* @par Parameters
*   carried_value = Value to be carried in R0 across reset.
*
* @retval
*   None
* 
* @par Side Effects
*   Function does not return
* 
*/
void boot_generate_warm_reset(uint32 carried_value);


/*===========================================================================

**  Function :  boot_configure_vbar

** ==========================================================================
*/
/*!
* 
* @brief
*   This function configures VBAR to a different base address.
* 
* @par Parameters
*   vbar_addr = Address to which VBAR will be configured to.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_configure_vbar(uint32 vbar_addr);

/*===========================================================================
**  Function :  boot_get_qsee_execute_status
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will get the flag to indicate if TZ image has executed.
*
* @param[in] None
*        
* @par Dependencies
* 
*/
boolean boot_get_qsee_execute_status(void);

#endif /*BOOTHW_TARGET_H*/
