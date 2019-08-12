#ifndef __PM_XO_CORE_H__
#define __PM_XO_CORE_H__

/*! \file
*  \n
*  \brief  pm_xo_core.h PMIC-MEGA XO CORE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC MEGA XO module. 
*  \n &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_xo_core.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/12   hs      Initial version. 
========================================================================== */
#include "comdef.h"
#include "pm_lib_err.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef enum
{
    /**
     * Very low power mode.
     */
    PM_XO_CORE_PWR_MODE__VLPM,
    /**
     * Low power mode
     */
    PM_XO_CORE_PWR_MODE__LPM,
    /**
     * Normal power mode
     */
    PM_XO_CORE_PWR_MODE__NPM, 
    /**
     * High power mode
     */
    PM_XO_CORE_PWR_MODE__HPM
} pm_xo_core_power_mode_type;

/*===========================================================================

                 SMBC DRIVER FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION FUNCTION pm_mega_xo_set_power_mode                EXTERNAL FUNCTION

DESCRIPTION
This function sets the XO CORE power mode.

INPUT PARAMETERS
1,int internalResourceIndex) Parameter name: mode

- Valid Input:
PM_MEGA_XO_MODE__VLPM = very low power mode 
PM_MEGA_XO_MODE__LPM  = low power mode
PM_MEGA_XO_MODE__NPM  = normal power mode
PM_MEGA_XO_MODE__HPM  = high power mode

RETURN VALUE
PM_ERR_FLAG__SUCCESS               = If no error were detected
during the pfunction exec.
PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = drive_strength is invalid 
PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
version of the PMIC.  
PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to
communicate with the PMIC.

DEPENDENCIES
None

SIDE EFFECTS
Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_dev_xo_core_set_power_mode(unsigned pmic_device_index, 
                                           int externalResourceIndex, 
                                           pm_xo_core_power_mode_type mode);
//#pragma message("Do not use pm_xo_core_set_power_mode(), which is deprecated. Use pm_dev_xo_core_set_power_mode() instead.")
pm_err_flag_type pm_xo_core_set_power_mode(int externalResourceIndex, 
                                           pm_xo_core_power_mode_type mode);


/*===========================================================================

FUNCTION FUNCTION pm_xo_set_xo_trim                     EXTERNAL FUNCTION

DESCRIPTION
This function trims the 19.2MHz XO load capacitances 

INPUT PARAMETERS
1) Parameter name: trim_value
- Type: uint8
- Raw trim value to be written in the range of [0, 63] 

RETURN VALUE
PM_ERR_FLAG__SUCCESS               = The operation was successful.
PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
version of the PMIC.
PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
rflib_init() and pm_init() must have been called.

SIDE EFFECTS
Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_dev_mega_xo_set_xo_trim(unsigned pmic_device_index, int externalResourceIndex, uint8 trim_value);
//#pragma message("Do not use pm_mega_xo_set_xo_trim(), which is deprecated. Use pm_dev_mega_xo_set_xo_trim() instead.")
pm_err_flag_type pm_mega_xo_set_xo_trim(int externalResourceIndex, uint8 trim_value);
/*===========================================================================

FUNCTION FUNCTION pm_xo_get_xo_trim                     EXTERNAL FUNCTION

DESCRIPTION
This function gets the raw trim value of the load capacitances for
the 19.2MHz XO

INPUT PARAMETERS
None.

RETURN VALUE
1) Raw trim value
- Type: uint8
- Valid value in the range of [0, 63] 
- Value of 0xFF is an error code that indicates a read failure

DEPENDENCIES
rflib_init() and pm_init() must have been called.

SIDE EFFECTS
Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_dev_mega_xo_get_xo_trim(unsigned pmic_device_index, int externalResourceIndex, uint8* trim_value);
//#pragma message("Do not use pm_xo_core_get_xo_trim(), which is deprecated. Use pm_dev_xo_core_get_xo_trim() instead.")
pm_err_flag_type pm_xo_core_get_xo_trim(int externalResourceIndex, uint8* trim_value);
/**
 * 
 * 
 * @brief Force the XO core on if pm_xo_enable(TRUE) has been called.
 * @param xo_core_enable TRUE to force the XO core on
 *                       FALSE to leave the XO core off
 * 
 * @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
 */
pm_err_flag_type pm_dev_mega_xo_set_xo_core_force_on(unsigned pmic_device_index, int externalResourceIndex, boolean xo_core_enable);
//#pragma message("Do not use pm_xo_core_set_xo_core_force_on(), which is deprecated. Use pm_dev_xo_core_set_xo_core_force_on() instead.")
pm_err_flag_type pm_xo_core_set_xo_core_force_on(int externalResourceIndex, boolean xo_core_enable);

#endif /* __PM_XO_CORE_H__ */
