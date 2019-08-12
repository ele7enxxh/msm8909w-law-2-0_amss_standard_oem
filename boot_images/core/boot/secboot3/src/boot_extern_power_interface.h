#ifndef BOOT_EXTERN_POWER_INTERFACE_H
#define BOOT_EXTERN_POWER_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN POWER DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external power drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_power_interface.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when        who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/13   jz       Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

  FUNCTION RAILWAY_INIT

  DESCRIPTION
        This function initialize the railway driver and all of its associated rail control.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
#ifdef FEATURE_BOOT_EXTERN_POWER_COMPLETED
  void boot_railway_init(void);
#else
  static inline boot_railway_init(void)
  {}
#endif

/*===========================================================================

  FUNCTION CPR_INIT

  DESCRIPTION
        This function initializes the CPR driver.

  PARAMETERS
    None.

  DEPENDENCIES
    Needs to call railway_init first.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
#ifdef FEATURE_BOOT_EXTERN_POWER_COMPLETED
  void boot_cpr_init(void);
#else
  static inline void boot_cpr_init(void)
  {}
#endif

/*===========================================================================

**  Function :  boot_populate_cpr_settings

** ==========================================================================
*/
/*!
* 
* @brief
*   This function calls cpr_externalize_state to populate the CPR settings and voltages in smem   
* 
* @param
*   None
*
* @par Dependencies
*   smem must have been initialized.
*   
* @retval
*   None                                                             
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_EXTERN_POWER_COMPLETED
void boot_populate_cpr_settings(void);
#else
static inline void boot_populate_cpr_settings(void)
{}
#endif

/*===========================================================================

**  Function :  boot_rbcpr_set_cx_mx_to_safe_turbo_voltage

** ==========================================================================
*/
/*!
*
* @brief
*   This function calls rbcpr_set_cx_mx_to_safe_turbo_voltage to set cx & mx turbo voltage
*
* @param
*   None
*
* @par Dependencies
*   smem must have been initialized.
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void boot_rbcpr_set_cx_mx_to_safe_turbo_voltage(void);



#endif /* BOOT_EXTERN_POWER_INTERFACE_H */
