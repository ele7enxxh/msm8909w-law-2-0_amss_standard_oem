/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.auth.securesampleauthservice;

import com.qualcomm.qti.auth.securesampleauthservice.ISecureSampleAuthListener;

interface ISecureSampleAuth {
  int verifyUser(in byte[] nonce, in String secAppName, in ISecureSampleAuthListener iSampleAuthListener);
  int cancel(in byte[] nonce, in ISecureSampleAuthListener iSecureSampleAuthListener);
  int getEnrollmentStatus(long userId, in ISecureSampleAuthListener iSecureSampleAuthListener);
}
