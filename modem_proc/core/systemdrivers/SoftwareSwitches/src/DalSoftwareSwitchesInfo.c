/*==============================================================================

FILE:      DalSoftwareSwitchesFwk.c

DESCRIPTION: This file enables the DAL subsystem to Attach to SoftwareSwitches
             DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
SoftwareSwitches_DalSoftwareSwitches_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalSoftwareSwitches_DeviceId[1] = {DALDEVICEID_SOFTWARESWITCHES};

const DALREG_DriverInfo 
DALSoftwareSwitches_DriverInfo = { SoftwareSwitches_DalSoftwareSwitches_Attach,
     					   1,
						   DalSoftwareSwitches_DeviceId
                         };


