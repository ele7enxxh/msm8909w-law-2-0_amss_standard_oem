/*============================================================================
@file ThresholdInstanceID.java

@brief
Value-type class to store the instace ID returned from threshold algorithm.

Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.core.sensortest;

public class ThresholdInstanceID {
   private int instanceID;
   public ThresholdInstanceID(int instanceID) {
      this.instanceID = instanceID;
   }
   public int instanceID() { return this.instanceID; }
}
