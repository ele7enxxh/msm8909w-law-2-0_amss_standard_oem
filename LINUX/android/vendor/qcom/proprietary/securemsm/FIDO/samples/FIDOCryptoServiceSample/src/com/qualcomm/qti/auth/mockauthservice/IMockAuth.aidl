/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.auth.mockauthservice;

import com.qualcomm.qti.auth.mockauthservice.IMockAuthListener;

interface IMockAuth {
  int giveAuthToken(int versionMajor, int versionMinor, int result, long userId, long uptime, in byte[] extData, long userEntityId);
  int verifyUser(in byte[] nonce, in String secAppName, in IMockAuthListener iMockAuthListener);
  int cancel(in byte[] nonce, in IMockAuthListener iMockAuthListener);
}
