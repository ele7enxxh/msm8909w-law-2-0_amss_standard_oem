/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.auth.securesampleauthservice;

public class VerifyUserResponse {
  public int result;
  public String authenticatorName;
  public long userId;
  public long userEntityId;
  public byte[] encapsulatedResult;
}
