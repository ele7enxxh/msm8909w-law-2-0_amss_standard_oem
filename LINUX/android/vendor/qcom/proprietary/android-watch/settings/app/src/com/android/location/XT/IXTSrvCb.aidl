/******************************************************************************
  @file    IXTSrvCb.aidl
  @brief   AIDL interface for IZat service

 Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ******************************************************************************/

package com.android.location.XT;

oneway interface IXTSrvCb
{
   void statusChanged(boolean status);
}
