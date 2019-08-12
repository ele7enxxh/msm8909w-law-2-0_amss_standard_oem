#ifndef PM_RESOURCE_MANAGER_H
#define PM_RESOURCE_MANAGER_H

/*! \file
 *  
 *  \brief  modem_proc_resource_manager.h ----This file contains ResourceManager class definition
 *  \details ResourceManager class is the glue that ties the external functional
Resource names to the internal Pmic resource names.
This class is entirely generated from the access tables and
external resource tables in the spread sheet.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    This file contains code for Target specific settings and modes.
 *  
 *    &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/framework/inc/pm_resource_manager.h#1 $ 


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/11   wra     Fixed kloc work warnings 

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
