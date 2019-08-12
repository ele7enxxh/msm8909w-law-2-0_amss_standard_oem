#ifndef PM_RPM_PROC_NPA_DEVICE_H
#define PM_RPM_PROC_NPA_DEVICE_H

/*! \file
 *  
 *  \brief  pm_rpm_proc_npa_device.h ----This file contains prototype definitions npa device layer
 *  \details This file contains prototype definitions npa device layer
 *          and the #DEFINE for the devices
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: MSM8974 branch 1.0 ver 8_21_2012 - Not Approved
 *    PMIC code generation NPA Device Setting Value Version: MSM8974 branch 1.0 ver 8_21_2012 - Not Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *    &copy; Copyright (c) 2010-2012 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/inc/pm_rpm_npa_device.h#1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

/*===========================================================================

                     DEFINES 

===========================================================================*/

/*===========================================================================
FUNCTION pm_rpm_proc_npa_init

DESCRIPTION
    This function initializes the NPA for rpm.

    It does the following:
    1)  It initializes the PMIC NPA software driver for nodes and resources.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pm_rpm_proc_npa_register_resources(void);


#endif //PM_RPM_PROC_NPA_DEVICE_H


