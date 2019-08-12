#ifndef __PM_LAB_H__
#define __PM_LAB_H__

/*! \file
*  \n
*  \brief  pm_uefi_lab.h PMIC-IBB MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  \n &copy;
*  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_lab.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/03/14   al      Updating to read VREG_OK
05/20/14   al      Architecture update 
05/29/14   al      Fixed history entry date for first entry  
05/29/14   al      Adding API to configure Vdsp 
04/18/14   al      Updated copyright 
04/18/14   al      Initial version. 
========================================================================== */
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


//#include "pm_lib_err.h"
//#include "pm_irq.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef struct
{
   boolean en_amoled_mode;        //false is LCD and true is amoled
   boolean lab_module_rdy;        //ready to be enabled
   boolean lab_module_en;         //module enabled or disabled
   boolean lab_ibb_en_rdy;        //allow IBB module to enable lab
   boolean en_fast_precharge;     //allow fast precharge
   uint32  max_prechg_time_usec;  //maximum pre_charge time
   boolean override_output_volt;  //override output voltage 
   uint32  output_volt;           //output voltage set
   boolean lab_vreg_ok;           //LAB VREG OK  
   boolean short_circuit;         //TRUE: no short circuit fault. FALSE: short circuit 
   boolean lab_status_en;         //TRUE: Current LAB status enabled; FALSE: Current LAB status disable                                                             
}pm_lab_status_type;




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
pm_err_flag_type pm_lab_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode);


 /**
 * @brief This function configures module to be enabled or not enabled
 * 
 * @details
 *  This API configures module to be enabled or not enabled
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] lab_rdy             TRUE if ready to be enabled else FALSE
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */

pm_err_flag_type pm_lab_lab_module_rdy(uint32 device_index, boolean lab_rdy);


 /**
 * @brief This function enables or disabled LAB module
 * 
 * @details
 *  This API enables or disabled LAB module
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] en_module           TRUE allows IBB module to enable LAB and FALSE won't
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_en_lab_module(uint32 device_index, boolean en_module);


 /**
 * @brief This function sets LAB to be enabled by IBB or not
 * 
 * @details
 *  This API allows LAB to be enabled by IBB or not
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ibb_en_rdyTRUE      TRUE allows IBB module to enable LAB and FALSE won't 
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_ibb_rdy_en(uint32 device_index, boolean ibb_rdy_en);


 /**
 * @brief This function configures precharge mode and sets max precharge time
 * 
 * @details
 *  This API configures precharge mode and sets max precharge time
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] fast_precharge     TRUE configures the fast_precharge and FLASE slow_precharge 
 * @param[in] max_time_usec      Max pre-charge time. Valid range is 200 to 500usec
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_config_precharge_ctrl(uint32 device_index, boolean fast_precharge, uint32 max_time_usec);


 /**
 * @brief This reads the LAB module setting status
 * 
 * @details
 *  This API reads the LAB module setting status
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] lab_status          Refer struct #pm_lab_status_type for more info
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_get_lab_status(uint32 device_index, pm_lab_status_type* lab_status);

 /**
 * @brief This function allows to select default voltage or set 
 *        output voltage for Vdisp
 *  
 * @details
 *  This API allows to select default voltage or set output 
 *  voltage for +Vdisp
 * 
 * @param[in] pmic_device_index.     Primary: 0 Secondary: 1
 * @param[in] override_output_volt   TRUE sets Output voltage given by SET_OUTPUT_VOLTAGE field otherwise default
 * @param[in] volt_millivolt         Output voltage. Range is 4600 to 6100 milli volt.
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_config_output_volt(uint32 device_index, boolean override_output_volt, uint32 volt_millivolt);
#endif /* __PM_LAB_H__*/

