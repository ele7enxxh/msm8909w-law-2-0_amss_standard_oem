/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package verizon.net.sip.internal;

/**
 * {@hide}
 */
oneway interface ISipRegistrationListener {

    void onRegistering(String localProfileUri);

    void onRegistrationCompleted(String localProfileUri, long expiryTime);

    void onRegistrationFailed(String localProfileUri, int errorCode, String errorMessage);
}
