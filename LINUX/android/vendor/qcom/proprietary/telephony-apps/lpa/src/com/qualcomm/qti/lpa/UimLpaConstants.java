/******************************************************************************
#  @file    UimLpaConstants.java
#  @brief   Lpa constants
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/
package com.qualcomm.qti.lpa;

public final class UimLpaConstants {
    public static final int UIM_LPA_ERROR_NONE                = 0;
    public static final int UIM_LPA_ERROR_GENERIC             = 1;
    public static final int UIM_LPA_ERROR_CONF_CODE_MISSING   = 2;

    public static final int UIM_LPA_ADD_PROFILE_STATUS_NONE                  = 0;
    public static final int UIM_LPA_ADD_PROFILE_STATUS_ERROR                 = 1;
    public static final int UIM_LPA_ADD_PROFILE_STATUS_DOWNLOAD_PROGRESS     = 2;
    public static final int UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_PROGRESS = 3;
    public static final int UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_COMPLETE = 4;

    public static final int UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NONE    = 0;
    public static final int UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_GENERIC = 1;
    public static final int UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_SIM     = 2;
    public static final int UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NETWORK = 3;
    public static final int UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_MEMORY  = 4;

    public static final int UIM_LPA_RADIO_UNAVAILABLE = 0;
    public static final int UIM_LPA_RADIO_OFF         = 1;
    public static final int UIM_LPA_RADIO_ON          = 2;
}

