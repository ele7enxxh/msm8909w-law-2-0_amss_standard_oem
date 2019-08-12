/*
 * Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.soundtrigger;

public class User implements Comparable<User> {
    String name;
    int id;
    int confidenceLevel = Global.getInstance().getSettingUserConfidenceLevel();
    boolean launchGoogleNow = false;

    User(String inName, int inId) {
        this.name = inName;
        this.id = inId;
    }

    User(String inName, int inConfidenceLevel, boolean inLaunchGoogleNow) {
        name = inName;
        confidenceLevel = inConfidenceLevel;
        launchGoogleNow = inLaunchGoogleNow;
    }

    public String getName() {
        return name;
    }

    public void setConfidenceLevel (int inConfidenceLevel) {
        this.confidenceLevel = inConfidenceLevel;
    }

    public int getConfidenceLevel () {
        return confidenceLevel;
    }

    public void toggleLaunch() {
        this.launchGoogleNow = !this.launchGoogleNow;
    }

    public boolean getLaunch() {
        return launchGoogleNow;
    }

    @Override
    public int compareTo(User another) {
        return this.name.compareToIgnoreCase(another.name);
    }

    //TODO implement equals() and hashCode() once I start comparing
    //     current SMs to saves ones
    @Override
    public boolean equals(Object object) {
        boolean isSameObject = false;
        if (null != object && object instanceof SoundModel) {
            isSameObject = (this.name.equals(((SoundModel) object).getName()));
        }
        return isSameObject;
    }
}
