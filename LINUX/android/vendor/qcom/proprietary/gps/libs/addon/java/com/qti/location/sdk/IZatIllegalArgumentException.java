/* ======================================================================
*  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

/**
 * An exception indicating illegal argument(s) in calling FLP service.
 * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 */
public class IZatIllegalArgumentException extends RuntimeException{
    public IZatIllegalArgumentException() {
    }

    public IZatIllegalArgumentException(String error) {
        super(error);
    }

    public IZatIllegalArgumentException(String error, Throwable cause) {
        super(error, cause);
    }
}
