#ifndef PM_RPM_TARGET_H
#define PM_RPM_TARGET_H

/*! \file pm_rpm_proc_npa_device_cfg.h
*  \n
*  \brief This file contains PMIC function prototypes for sleep related functions.
*  \n
*  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/inc/pm_rpm_target.h#1 $

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/inc/pm_rpm_target.h#1 $  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/14   kt      Updated comments header and added doxygen comments.
08/16/13   hs      Initial version. 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_npa_device.h"
#include "pm_err_flags.h"

/*===========================================================================

                        MACRO DEFINITIONS 

===========================================================================*/

#define PM_MAX_SLEEP_MODE  2  

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/

/** 
 * @name pm_rpm_sleep_init 
 *  
 * @brief This function initializes the sleep settings LUT.
 * 
 * @param None
 *
 * @return None 
 *  
 * @dependencies Sleep settings should be available in config.
 *
 */
void pm_rpm_sleep_init(void);

/** 
 * @name pm_rpm_platform_init 
 *  
 * @brief This function initialize the platform specific 
 *        settings.
 * 
 * @param None
 *
 * @return None 
 *  
 * @sideeffects None
 *
 */
void pm_rpm_platform_init(void);

/** 
 * @name pm_npa_rpm_ldo_hr_config 
 *  
 * @brief This configures the headroom for ldo2 and 3 based on DDR freq
 * 
 * @param ddr_freq_mode
 *
 * @return pm_err_flag_type 
 *  
 * @sideeffects None
 *
 */
pm_err_flag_type pm_npa_rpm_ldo_hr_config (unsigned  ddr_freq_mode);


#endif /* PM_RPM_TARGET_H */
