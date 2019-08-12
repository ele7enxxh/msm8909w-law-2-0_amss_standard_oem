/* ======================================================================
*  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

/**
 * An exception indicating location service is unavailable.
 * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 */
public class IZatServiceUnavailableException extends RuntimeException{
    public IZatServiceUnavailableException() {
    }

    public IZatServiceUnavailableException(String error) {
        super(error);
    }

    public IZatServiceUnavailableException(String error, Throwable cause) {
        super(error, cause);
    }
}
