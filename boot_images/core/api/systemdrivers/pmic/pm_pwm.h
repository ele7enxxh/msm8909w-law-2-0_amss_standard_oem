#ifndef __PM_PWM_H__
#define __PM_PWM_H__
/*! \file pm_pwm.h
 *  \n
 *  \brief  PMIC PWM driver APIs
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC PWM module.
 *
 *
 *  Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_pwm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/14   al      Architecture update 
12/11/12   al	   New file
=============================================================================*/

//#include "pm_lib_err.h"
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"



/*===========================================================================
                     GLOBAL TYPE DEFINITIONS
===========================================================================*/

/*! \enum pm_pwm_bit_size_type
 *  \brief PWM Bit Size - 6 or 9 bit
 */
typedef enum{
    PM_PWM_SIZE__6BIT,
    PM_PWM_SIZE__9BIT,
    PM_PWM_SIZE__MAX
} pm_pwm_bit_size_type;


/*! \enum pm_pwm_clock_freq_type
 *  \brief PWM master clock frequency select
 */
typedef enum{
    PM_PWM_CLK__OFF,
    PM_PWM_CLK__1_0_KHZ,
    PM_PWM_CLK__32_0_KHZ,
    PM_PWM_CLK__19_2_MHZ,
    PM_PWM_CLK__MAX
} pm_pwm_clock_freq_type;


/*! \struct pm_pwm_status_type
 *  \brief PWM status
 */
typedef struct{
  boolean pwm_enable;
  /* PWM enabled or disabled */  
  uint16 pwm_value;
  /* Value set for PWM duty cycle */
  pm_pwm_bit_size_type bit_size; 
  /* PWM bit size */
  pm_pwm_clock_freq_type clk_freq;
  /* PWM master clock frequency */
  uint32 pre_div;
  /* Frequency predivide to divide down PWM master clock */
  uint32 exp;
  /* Exponent values to divide down PWM master clock */
  boolean glitch_removal;
  /* PWM glitch removal enabled or disabled */
}pm_pwm_status_type;



/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
FUNCTION  pm_pwm_enable_pwm                            EXTERNAL FUNCTION

DESCRIPTION
   This function enables/disables PWM module

PARAMETERS
 * @param [in] pmic_chip:  
 *                 PmicDeviceIndex - For primary 0 
 * @param [in] enable: 
 *                 TRUE : enable PWM
 *                 FALSE: disable PWM

RETURN VALUE
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG__PAR_OUT_OF_RANGE     = Input Parameter is out of range.  
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.

===========================================================================*/
pm_err_flag_type pm_pwm_enable_pwm(uint32 pmic_chip, 
                                   boolean enable 
                                   );



/*===========================================================================
FUNCTION  pm_pwm_set_pwm_value                      EXTERNAL FUNCTION

DESCRIPTION
   This function sets the PWM value used to calculate duty cycle
PARAMETERS
 * @param [in] pmic_chip: 
 *                 PmicDeviceIndex - For primary 0 
 * @param [in] pwm_value: 
 *                 uint16 PWM value used to calculate duty cycle 

RETURN VALUE
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG__PAR_OUT_OF_RANGE     = Input Parameter is out of range.  
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.

===========================================================================*/
pm_err_flag_type pm_pwm_set_pwm_value( uint32 pmic_chip,
                                       uint16 pwm_value 
                                       );



/*===========================================================================
FUNCTION  pm_pwm_clock_config                         EXTERNAL FUNCTION

DESCRIPTION
   This function configures the PWM clock
PARAMETERS
 * @param [in] pmic_chip: 
 *                 PmicDeviceIndex - For primary 0 
 * @param [in] bit_size: 
 *                 PWM size 6 or 9 bit. Refer enum pm_pwm_bit_size_type for more info.  
 * @param [in] clk_freq: 
 *                 PWM master clock frequency. Refer enum pm_pwm_clock_freq_type for more info. 
 * @param [in] pre_div: 
 *                 Frequency predivide to divide down pwm master clock.                    
 *                 Supported values are 1,3,5,6
 * @param [in] exp: 
 *                 Exponent values to divide down pwm master clock. 
 *                 Values range is 0 to 7
 *
RETURN VALUE
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG__PAR_OUT_OF_RANGE      = Input Parameter is out of range.  
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.

===========================================================================*/
pm_err_flag_type pm_pwm_clock_config( uint32 pmic_chip,
                                      pm_pwm_bit_size_type bit_size,
                                      pm_pwm_clock_freq_type clk_freq,
                                      uint32 pre_div, 
                                      uint32 exp
                                     );


/*===========================================================================
FUNCTION  pm_pwm_glitch_removal                         EXTERNAL FUNCTION

DESCRIPTION
   This function configures the PWM glitch removal.
PARAMETERS
 * @param [in] pmic_chip: : 
 *                 PmicDeviceIndex - For primary 0 
 * @param [in] glitch_removal: 
 *                 TRUE : enable glitch removal, PWM outputs are updated only on PWM period boundaries  
 *                 FALSE: no glitch removal, PWM outputs are updated immediately

        
RETURN VALUE
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG__PAR_OUT_OF_RANGE     = Input Parameter is out of range.  
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.

===========================================================================*/
pm_err_flag_type pm_pwm_glitch_removal(uint32 pmic_chip, 
                                       boolean glitch_removal
                                       );


/*===========================================================================
FUNCTION  pm_pwm_get_status                         EXTERNAL FUNCTION

DESCRIPTION
   This function reads and stored the current PWM settings
PARAMETERS
 * @param [in]  pmic_chip: : 
 *                 PmicDeviceIndex - For primary 0 
 * @param [out] pwm_status: 
 *                 Pointer to pm_pwm_status_type structure. Refer pm_pwm_status_type for more info


        
RETURN VALUE
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG__PAR_OUT_OF_RANGE     = Input Parameter is out of range.  
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.

===========================================================================*/
pm_err_flag_type pm_pwm_get_status (uint32 pmic_chip, 
                                    pm_pwm_status_type *pwm_status
                                    );

#endif // __PM_PWM_H__
