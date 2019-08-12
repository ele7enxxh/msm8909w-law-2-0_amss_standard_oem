#ifndef PM_MODEM_PROC_NPA_H
#define PM_MODEM_PROC_NPA_H

/*! \file
 *  
 *  \brief  pm_modem_proc_npa.h ----This file contains prototype definitions processor npa layer
 *  \details This file contains prototype definitions processor npa layer
 *          which consists mainly of the initialization function prototype
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

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/inc/pm_modem_proc_npa.h#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                        DEFINITIONS

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

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
void pm_modem_proc_npa_init(void);

void pm_modem_proc_core_vol_init(void);


#endif //PM_MODEM_PROC_NPA_H



