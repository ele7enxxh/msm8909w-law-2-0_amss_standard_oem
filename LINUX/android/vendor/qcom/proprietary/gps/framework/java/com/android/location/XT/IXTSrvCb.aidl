/******************************************************************************
  @file    IXTSrvCb.aidl
  @brief   AIDL interface for IZat service

  ---------------------------------------------------------------------------
  Copyright (C) 2015 Qualcomm Technologies, Inc.All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
 ******************************************************************************/
package com.android.location.XT;

oneway interface IXTSrvCb
{
   void statusChanged(boolean status, boolean user_eula_status);
}
