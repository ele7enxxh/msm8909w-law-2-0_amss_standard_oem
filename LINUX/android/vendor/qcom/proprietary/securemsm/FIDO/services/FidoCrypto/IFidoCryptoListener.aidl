/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.auth;

oneway interface IFidoCryptoListener {
  void onResult(in int resultCode, in byte[] uafResponse);
}
