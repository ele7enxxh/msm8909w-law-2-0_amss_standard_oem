/******************************************************************************
 * @file    UimLpaDownloadProgress.java
 * @brief   This interface provides the APIs to enable Local Profile Assistant
 *          for eUICC
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/


package com.qualcomm.qti.lpa;

import java.util.List;
import android.os.Parcelable;
import android.os.Parcel;

public class UimLpaDownloadProgress implements Parcelable {
    private int status;
    private int cause;
    private int progress;

    public UimLpaDownloadProgress() {
    }

    public UimLpaDownloadProgress(int status, int cause, int progress) {
        this.status = status;
        this.cause = cause;
        this.progress = progress;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public int getStatus() {
        return status;
    }

    public void setProgress(int progress) {
        this.progress = progress;
    }

    public int getProgress() {
        return progress;
    }

    public void setCause(int cause) {
        this.cause = cause;
    }

    public int getCause() {
        return cause;
    }

    public UimLpaDownloadProgress(Parcel in) {
        this.status   = in.readInt();
        this.cause    = in.readInt();
        this.progress = in.readInt();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(cause);
        dest.writeInt(progress);
    }

    public static final Parcelable.Creator<UimLpaDownloadProgress> CREATOR =
                                            new Parcelable.Creator<UimLpaDownloadProgress>() {

        public UimLpaDownloadProgress createFromParcel(Parcel in) {
            return new UimLpaDownloadProgress(in);
        }

        public UimLpaDownloadProgress[] newArray(int size) {
            return new UimLpaDownloadProgress[size];
        }
    };
}

