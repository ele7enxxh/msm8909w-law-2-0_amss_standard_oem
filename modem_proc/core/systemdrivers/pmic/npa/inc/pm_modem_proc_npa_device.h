#ifndef PM_MODEM_PROC_NPA_DEVICE_H
#define PM_MODEM_PROC_NPA_DEVICE_H

/*! \file
 *
 *  \brief  pm_modem_proc_npa_device.h ----This file contains prototype definitions npa device layer
 *  \details This file contains prototype definitions npa device layer
 *          and the #DEFINE for the devices
 *
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: MSM8974 branch 1.0 ver 01_31_2013 - Approved
 *    PMIC code generation NPA Device Setting Value Version: MSM8974 branch 1.0 ver 01_31_2013 - Approved
 *    This file contains code for Target specific settings and modes.
 *
 *    &copy; Copyright (c) 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/inc/pm_modem_proc_npa_device.h#1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_lib_api.h"         /* interface files for all library APIs */
#include "npa.h"
#include "npa_resource.h"


//#define PMIC_NPA_RMT_PROTOCOL_RPM_TYPE    "/protocol/rpm/rpm"


/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/
/* Device Remote Resource */

/*===========================================================================

FUNCTION pm_modem_proc_npa_init

DESCRIPTION
    This function initializes the NPA for modem.

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
void pm_modem_proc_npa_register_resources(void);


#endif //PM_MODEM_PROC_NPA_DEVICE_H


