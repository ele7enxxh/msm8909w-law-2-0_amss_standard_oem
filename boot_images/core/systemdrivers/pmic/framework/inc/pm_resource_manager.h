#ifndef PM_RESOURCE_MANAGER_H
#define PM_RESOURCE_MANAGER_H

/*! \file pm_resource_manager.h
 *  
 *  \brief  This file contains the function definitions for PMIC resource manager.
 *  
 *  &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/inc/pm_resource_manager.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/13   kt      PMIC Framework update
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
