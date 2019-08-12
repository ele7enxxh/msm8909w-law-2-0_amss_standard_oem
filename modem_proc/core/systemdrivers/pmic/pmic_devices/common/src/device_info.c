/*! \file device_info.c
*   \n
*   \brief This file contains initialization function for PMIC Device Info.
*   \n
*   \n &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/pmic_devices/common/src/device_info.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/10   wra     RPM Memory reduction effort.
11/15/09   wra     File created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "device_info.h"

/*=========================================================================
      Function Definitions
==========================================================================*/

void pmiC_DeviceInfo_Init(pmiC_DeviceInfo *deviceInfo, DeviceType deviceType, DeviceIndex deviceIndex)
{
   deviceInfo->mPmicModel = pm_get_pmic_model((uint8)deviceIndex);
   deviceInfo->mDeviceIndex = deviceIndex;
   deviceInfo->mPmicAllLayerRevision = pm_get_pmic_revision((uint8)deviceIndex);

   /* Slave ID index is 0 to get PMIC device Primary Slave Id */
   pm_get_slave_id((uint8)deviceIndex, 0, &(deviceInfo->mSlaveId[0]));

   /* Slave ID index is 1 to get PMIC device Secondary Slave Id */
   pm_get_slave_id((uint8)deviceIndex, 1, &(deviceInfo->mSlaveId[1]));

   deviceInfo->mDeviceType = deviceType;
   deviceInfo->mMaxRegisterSize = 1024;
}
