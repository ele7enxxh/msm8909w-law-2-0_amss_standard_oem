/******************************************************************************
  @file    IXTSrv.aidl
  @brief   AIDL interface for IZat service

  ---------------------------------------------------------------------------
  Copyright (C) 2015 Qualcomm Technologies, Inc.All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
 ******************************************************************************/
package com.android.location.XT;

import java.com.android.location.XT.IXTSrvCb;

// Methods below are used to communicate between XTSrv and its clients
interface IXTSrv
{
    boolean disable();
    boolean getStatus();
    // updates persistent value and modem with FinalEula value
    void updateEulaStatus(boolean finalEulaStatus);
    String  getText(int which);
    void showDialog();
    void registerCallback(IXTSrvCb cb);
    void unregisterCallback(IXTSrvCb cb);
}
