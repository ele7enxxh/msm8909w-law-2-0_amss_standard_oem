#ifndef __PM_IBB_H__
#define __PM_IBB_H__

/*! \file
*  \n
*  \brief  pm_uefi_ibb.h PMIC-IBB MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  \n &copy;
*  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_ibb.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/16/14   al      Add API to configure delay between LAB and IBB during pwrup and pwrdwn
05/20/14   al      Architecture update 
04/18/14   al      Updated copyright 
09/11/12   al      Initial version. 
========================================================================== */
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


//#include "pm_lib_err.h"
//#include "pm_irq.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef enum
{
  PM_IBB_PWRUP_DLY_SRC_BIAS_ACTIVE, /*IBB power stage delay source is after IBB's bias active*/
  PM_IBB_PWRUP_DLY_SRC_LAB_VREG_OK, /*IBB power stage delay source is after VREG_OK from LAB and IBB's bias active 0x0 : BIAS_ACTIVE */
  PM_IBB_PWRUP_DLY_SRC_INVALID      /*Invalid*/
}pm_ibb_pwrup_dly_src_type;

typedef enum
{
  PM_IBB_PWRDN_DLY_SRC_IBB_DISABLE,   /*LAB power-down delay source is after IBB disable */
  PM_IBB_PWRDN_DLY_SRC_IBB_DISCHARGE, /*LAB power-down delay is after IBB's discharged*/
  PM_IBB_PWRDN_DLY_SRC_INVALID        /*Invalid*/
}pm_ibb_pwrdn_dly_src_type;



typedef struct
{
  uint32                    pwrdn_dly_ms;           /*delay between IBB and LAB while powering down. Valid values are 1,2,4,8 ms*/
  pm_ibb_pwrdn_dly_src_type pwrdn_dly_src;          /*refer pm_ibb_pwrdn_dly_src_type for more info*/
  uint32                    pwrup_dly_ms;           /*delay between IBB and LAB while powering up. Valid values are 1,2,4,8 ms*/
  boolean                   en_pwrup_dly;           /*enable disable powerup delay*/
  pm_ibb_pwrup_dly_src_type pwrup_dly_src;          /*refer pm_ibb_pwrup_dly_src_type for more info*/
} pm_ibb_pwrup_pwrdn_dly_type; 

typedef struct
{
  boolean                     en_amoled_mode;        /*false is LCD and true is amoled           */
  boolean                     ibb_module_rdy;        /*ready to be enabled                       */
  boolean                     ibb_module_en;         /*module enabled or disabled                */
  boolean                     swire_rdy;             /*swire_rdy                                 */
  uint32                      chgr_resistor_kohms;   /*charger resistor                          */
  uint32                      swire_pulse_mv;        /*swire output voltage pulse in milli volt  */
  boolean                     override_output_volt;  /*override output voltage                   */
  uint32                      output_volt;           /*output voltage set                        */
  pm_ibb_pwrup_pwrdn_dly_type ibb_dly_config;        /*IBB delay config type                     */
  boolean                     ibb_vreg_ok;           /*IBB VREG OK                               */ 
  boolean                     short_circuit;         /*IBB Module short circuit detect           */
  boolean                     ilimit_error;          /*current limit error                       */
}pm_ibb_status_type;

/*===========================================================================

                 SMBCHG OTG DRIVER FUNCTION PROTOTYPES

===========================================================================*/

 /**
 * @brief This function configures module for AMOLED or LCD
 * 
 * @details
 *  This API configures module for AMOLED or LCD
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] en_amoled_mode      TRUE enables for AMOLED and FALSE for LCD 
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode);


 /**
 * @brief This function prepares module to be enabled or not
 * 
 * @details
 *  This API prepares module to be enabled or not.
 *  When this is set to low, the IBB module cannot be enabled 
 *  (powered-up) through S-Wire or IBB_ENABLE register
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ibb_rdy             TRUE if ready to be enabled else FALSE
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_ibb_module_rdy(uint32 device_index, boolean ibb_rdy);


 /**
 * @brief This function enables/disables module and makes S-wire ready for listening
 * 
 * @details
 *  This API enables/disables module and makes S-wire ready for listening
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] en_module      TRUE enables module and false disables module
 * @param[in] swire_rdy      TRUE ready to liste to to S-wire and FALSE not ready to listen to S-Wire
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_config_ibb_ctrl(uint32 device_index, boolean en_module, boolean swire_rdy);



 /**
 * @brief This function sets the charging resistance for soft start
 * 
 * @details
 *  This API sets the charging resistance for soft start.  
 *  Time constant is ref_cap*charging resistor.
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] resistor_kohms     Charging resistor value. Values are 16 to 300 KOhms
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_set_soft_strt_chgr_resistor(uint32 device_index, uint32 resistor_kohms);



 /**
 * @brief This function sets startup voltage when there is only
 *        one s-wire voltage programming pulse
 * 
 * @details
 *  This API sets startup voltage when there is only one s-wire 
 *        voltage programming pulse
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] volt_millivolt      Valid value range is 1400 to 7700 milli volt
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_set_swire_output_pulse(uint32 device_index, uint32 volt_millivolt);


 /**
 * @brief This function allows to select default voltage and set 
 *        output voltage
 *  
 * @details
 *  This API allows to select default voltage and set output 
 *  voltage
 * 
 * @param[in] pmic_device_index.     Primary: 0 Secondary: 1
 * @param[in] override_output_volt   TRUE sets Output voltage given by SET_OUTPUT_VOLTAGE field otherwise default
 * @param[in] volt_millivolt         Output voltage. Range is 1400 to 7700 milli volt.
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_config_output_volt(uint32 device_index, boolean override_output_volt, uint32 volt_millivolt);


 /**
 * @brief This configures the delay for IBB module 
 * 
 * @details
 *  This API configures delay required between IBB and LAB vregs and also delay source
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ibb_status          Refer struct #pm_ibb_pwrup_pwrdn_dly_type for more info
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_config_pwrup_pwrdn_dly(uint32 device_index, pm_ibb_pwrup_pwrdn_dly_type *delay_config);

 /**
 * @brief This reads the IBB module setting status
 * 
 * @details
 *  This API reads the IBB module setting status
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ibb_status          Refer struct #pm_ibb_status_type for more info
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
 pm_err_flag_type pm_ibb_get_ibb_status(uint32 device_index, pm_ibb_status_type* ibb_status);
#endif /* __PM_IBB_H__*/

