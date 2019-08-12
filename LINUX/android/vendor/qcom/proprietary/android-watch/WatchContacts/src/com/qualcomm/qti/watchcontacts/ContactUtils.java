/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchcontacts;

import java.util.HashMap;

public class ContactUtils {

    public static final HashMap<String, String> PHONE_TYPES = new HashMap<String, String>();
    static {
        PHONE_TYPES.put("1", "Home");
        PHONE_TYPES.put("2", "Mobile");
        PHONE_TYPES.put("3", "Work");
    }
    public static final String PHONE_TYPES_OTHERS = "Other";
    public static final String CONTACT_ID = "CONTACT_ID";
    public static final String CONTACT_NAME = "CONTACT_NAME";
}
