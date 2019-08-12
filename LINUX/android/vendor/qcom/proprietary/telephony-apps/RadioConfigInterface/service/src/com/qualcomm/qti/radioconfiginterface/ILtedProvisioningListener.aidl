/******************************************************************************
 * @file    ILtedProvisioningListener.aidl
 * @brief   LTED Provisioning status change listener
 *
 * @version 00.00.01
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

/**
 * Listener type for receiving the asynchronous responses and unsolicited indications.
 */
oneway interface ILtedProvisioningListener {
    /**
     * Notifies that the BAK password (PSK) in LTED provisioning req
     * has expired and is no longer valid
     *
     * @param   none
     * @return  none
     *
     */
    void onPskExpired ();
}
