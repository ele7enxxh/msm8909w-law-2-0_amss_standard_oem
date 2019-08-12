/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package verizon.net.sip.internal;

import verizon.net.sip.internal.ICameraListener;

/**
 * {@hide}
 */
oneway interface ICamera {
    void setListener(in ICameraListener listener);

    void startSmoothZoom(int value);

    void stopSmoothZoom();
}
