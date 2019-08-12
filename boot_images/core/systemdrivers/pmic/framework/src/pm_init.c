/*! \file pm_init.c
*   \n
*   \brief This file contains PMIC initialization function which initializes the PMIC Comm
*   \n layer, PMIC drivers and PMIC applications.
*   \n
*   \n &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/src/pm_init.c#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/15/13   aab     Fixed KW error
05/10/11   jtn     Fix RPM init bug for 8960
07/01/10   umr     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_resource_manager.h"
#include "pm_sbl_boot.h"
#include "pm_comm.h"


pm_err_flag_type pm_driver_init( void )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    pm_resource_manager_init();

    pm_driver_post_init ();

    return err_flag;
}

pm_err_flag_type pm_oem_init ( void )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    
    return err_flag;
}
