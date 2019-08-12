/*  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.location.izat.flp;

import java.com.qualcomm.location.izat.flp.IMaxPowerAllocatedCallback;

interface ITestService {

    void deleteAidingData(in long flags);

    void registerMaxPowerChangeCallback(in IMaxPowerAllocatedCallback cb);

    void unregisterMaxPowerChangeCallback(in IMaxPowerAllocatedCallback cb);
}
