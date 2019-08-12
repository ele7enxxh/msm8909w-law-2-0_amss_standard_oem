#ifndef PM_RESOURCE_MANAGER_H
#define PM_RESOURCE_MANAGER_H

/*! \file pm_resource_manager.h
 *  
 *  \brief  This file contains the function definitions for PMIC resource manager.
 *  
 *  &copy; Copyright 2010-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/inc/pm_resource_manager.h#1 $ 

when       who     what, where, why
--------	---		------------------------------------------------
09/10/13   rk      Code refactoring.
02/27/13   hs      Code refactoring.
===========================================================================*/

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*! \brief This function initializes the drivers for the PMIC peripherals on the chip and 
 *         applicable to the image.
 *  \param[in] None
 *
 *  \return None.
 *
 *  <b>Dependencies</b>
 *  \li pm_processor_init_comms().
 *  \li pm_target_information_init();
 *  \li pm_processor_init();
 */
void pm_resource_manager_init(void);

#endif //PM_RESOURCE_MANAGER_H
