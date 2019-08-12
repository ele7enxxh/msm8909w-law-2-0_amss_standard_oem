#ifndef _DS_QMI_PWRMGMT_SHIM_H
#define _DS_QMI_PWRMGMT_SHIM_H
/*===========================================================================

                  D S _ Q M I _ P W R M G M T _ S H I M . H

DESCRIPTION

 The Data Services QMI Services Source file. Contains declarations and
 prototypes for power management shim layer between QMI and external
 subsystems.

EXTERNALIZED FUNCTIONS

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_pwrmgmt_shim.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/14/10    ar     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"


/*===========================================================================
  FUNCTION QMI_PWRMGMT_SHIM_INIT

  DESCRIPTION
    Function to initialize QMI power management shim layer elements.
 
  PARAMETERS
    None
   
  RETURN VALUE
    TRUE on successful operation
    FALSE otherwise
   
  DEPENDENCIES
    External subsystems must be initialized.

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pwrmgmt_shim_init( void );

#endif /* _DS_QMI_PWRMGMT_SHIM_H */
