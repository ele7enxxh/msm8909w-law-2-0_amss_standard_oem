
/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.os.SystemProperties;

public class Utils {


    private static enum DisplayType{
        CIRCULAR,
        RECTANGULAR
    }
    private static DisplayType displayPannelType;
    private static final String DISPLAY_TYPE = "ro.display.type";

    public static void displayTypeInit() {
        String pannelType = SystemProperties.get(DISPLAY_TYPE);
        if(pannelType.equals("CIRC")) {
             displayPannelType = DisplayType.CIRCULAR;
        } else {
             displayPannelType = DisplayType.RECTANGULAR;
        }
    }

    public static boolean IsDisplayTypeCircular() {
        displayTypeInit();
        if(DisplayType.CIRCULAR == displayPannelType) {
            return true;
        }
        return false;
    }
}
