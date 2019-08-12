/* ======================================================================
*  Copyright (c) 2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qualcomm.location.izat.flp;

oneway interface IMaxPowerAllocatedCallback
{
    void onMaxPowerAllocatedChanged(in double power_mW);
}
