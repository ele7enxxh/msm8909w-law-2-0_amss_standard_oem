#ifndef __PM_CLK_BUFFER_H__
#define __PM_CLK_BUFFER_H__

/*! \file
*  \n
*  \brief  pm_clk_buffer.h PMIC-MEGA XO CLK BUFFER RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC MEGA XO module. 
*  \n &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_clk_buffer.h#1 $

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
    * 
    */
    PM_CLK_BUFF_OUT_DRV__1X,
    /**
    * 
    */
    PM_CLK_BUFF_OUT_DRV__2X,
    /**
    * 
    */
    PM_CLK_BUFF_OUT_DRV__3X,
    /**
    * 
    */
    PM_CLK_BUFF_OUT_DRV__4X,
} pm_clk_buff_output_drive_strength_type;

/*===========================================================================

SMBC DRIVER FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION FUNCTION pm_clk_buff_set_output_drive_strength                EXTERNAL FUNCTION

DESCRIPTION
This function sets the output buffer drive strength.

INPUT PARAMETERS
@param pmic_device_index Selects the device in which the buffers being controlled are located. Device index starts with zero
1,int internalResourceIndex) Parameter name: drive_strength

- Valid Input:
PM_CLK_BUFF_OUT_DRV__1X = Same strength
PM_CLK_BUFF_OUT_DRV__2X  = twice strength
PM_CLK_BUFF_OUT_DRV__3X  = triple strength
PM_CLK_BUFF_OUT_DRV__4X  = quadruple strength

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
pm_err_flag_type pm_dev_clk_buff_set_output_drive_strength(unsigned pmic_device_index, 
                                                           int resourceIndex, 
                                                           pm_clk_buff_output_drive_strength_type drive_strength);
//#pragma message("Do not use pm_clk_buff_set_output_drive_strength(), which is deprecated. Use pm_dev_clk_buff_set_output_drive_strength() instead.")
pm_err_flag_type pm_clk_buff_set_output_drive_strength(int resourceIndex, 
                                                       pm_clk_buff_output_drive_strength_type drive_strength);

/**
* 
* 
* @brief Enables the clk.
* @param pmic_device_index Selects the device in which the buffers being controlled are located. Device index starts with zero
* @param resourceIndex Selects which buffer is being targeted by the API
* @param enable TRUE to enable the clock
*               FALSE to not force the clock to be on
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_dev_clk_buff_enable(unsigned pmic_device_index, int resourceIndex, boolean enable);
//#pragma message("Do not use pm_clk_buff_enable(), which is deprecated. Use pm_dev_clk_buff_enable() instead.")
pm_err_flag_type pm_clk_buff_enable(int resourceIndex, boolean enable);

/**
* 
* 
* @brief Enables the buffer pin control functionality.
* @param pmic_device_index Selects the device in which the buffers being controlled are located. Device index starts with zero
* @param resourceIndex Selects which buffer is being targeted by the API
* @param enable TRUE to enable the clocks pin control
*               FALSE to disable the clocks pin control
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_dev_clk_buff_pin_control_enable(unsigned pmic_device_index, int resourceIndex, boolean pin_control_enable);
//#pragma message("Do not use pm_clk_buff_pin_control_enable(), which is deprecated. Use pm_dev_clk_buff_pin_control_enable() instead.")
pm_err_flag_type pm_clk_buff_pin_control_enable(int resourceIndex, boolean enable);

/**
* 
* @brief Control the polarity of the pin control functionality.
*
* @param pmic_device_index Selects the device in which the buffers being controlled are located. Device index starts with zero
* @param resourceIndex Selects which buffer is being targeted by the API
* @param is_low_enabled TRUE = Clock will be enabled when PC signal is low
*                       FALSE = Clock will be enabled when PC signal is high, 
*                        
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_dev_clk_buff_pin_control_polarity(unsigned pmic_device_index, int resourceIndex, boolean is_low_enabled);

#endif /* __PM_CLK_BUFFER_H__ */
