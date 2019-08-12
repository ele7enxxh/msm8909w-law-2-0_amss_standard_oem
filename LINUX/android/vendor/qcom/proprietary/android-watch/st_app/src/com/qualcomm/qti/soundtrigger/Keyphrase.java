/*
 * Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.soundtrigger;

import java.util.ArrayList;

import android.util.Log;

public class Keyphrase implements Comparable<Keyphrase> {
    private static final String TAG = "ListenLog.Keyphrase";
    String name;
    int id;
    boolean isUdk = false;
    int confidenceLevel = Global.getInstance().getSettingKeyPhraseConfidenceLevel();
    boolean launchGoogleNow = false;
    private ArrayList<User> users = new ArrayList<User>();

    Keyphrase(String inName, int inId) {
        this.name = inName;
        this.id = inId;
    }

    //TODO UDK change this to include UDK
    Keyphrase(String inName, int inConfidenceLevel, boolean inLaunchGoogleNow) {
        name = inName;
        confidenceLevel = inConfidenceLevel;
        launchGoogleNow = inLaunchGoogleNow;
    }

    public String getName() {
        return this.name;
    }

    public boolean getIsUdk() {
        return this.isUdk;
    }

    public void setConfidenceLevel (int inConfidenceLevel) {
        this.confidenceLevel = inConfidenceLevel;
    }

    public void toggleLaunch() {
        this.launchGoogleNow = !this.launchGoogleNow;
    }

    public int getConfidenceLevel () {
        return confidenceLevel;
    }

    public boolean getLaunch() {
        return launchGoogleNow;
    }

    //User functions
    public ArrayList<User> getUsers() {
        return users;
    }

    public void addUser(String inName, int inId) {
        users.add(new User(inName, inId));
    }

    //TODO remove if not needed
    /*public void addUser(String inName, int inConfidenceLevel, boolean inLaunchGoogleNow) {
        users.add(new User(inName, inConfidenceLevel, inLaunchGoogleNow));
    }*/

    //TODO implement equals() and hashCode() once I start comparing
    //     current SMs to saves ones

    @Override
    public int compareTo(Keyphrase another) {
        return this.name.compareToIgnoreCase(another.name);
    }
}
