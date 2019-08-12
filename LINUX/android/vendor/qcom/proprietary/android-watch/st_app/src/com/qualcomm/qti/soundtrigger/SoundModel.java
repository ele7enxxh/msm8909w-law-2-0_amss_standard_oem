/*
 * Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc. 
 */

package com.qualcomm.qti.soundtrigger;

import java.util.ArrayList;

import android.util.Log;

public class SoundModel implements Comparable<SoundModel> {
    private final static String TAG = "ListenLog.SoundModel";
    private String name;
    private String prettyName;
    private ArrayList<Keyphrase> keyphrases = new ArrayList<Keyphrase>();

    SoundModel(String inName) {
        name = inName;
        prettyName = Global.getInstance().getSmRepo().getSmPrettyName(inName);
    }

    public String getName() {
        return name;
    }

    public String getPrettyName() {
        return prettyName;
    }

    public ArrayList<Keyphrase> getKeyphrases() {
        return keyphrases;
    }

    public ArrayList<User> getUsers() {
        ArrayList<User> users = new ArrayList<User>();
        for (Keyphrase keyphrase : keyphrases) {
            users.addAll(keyphrase.getUsers());
        }
        return users;
    }

    public void addKeyphrase(Keyphrase inKeyphrase) {
        //TODO add check for already existing keyword
        keyphrases.add(inKeyphrase);
    }

    // Not currently being used.
    /*public void addUser(String keywordName, String newUser) {
        for (Keyphrase keyword : keyPhrases) {
            if (keyword.getName().equals(keywordName)) {
                //TODO add check for already existing user
                keyword.addUser(newUser);
            }
        }
    }*/

    public boolean userExistsForKeyword(String inKeywordName, String inUserName) {
        for (Keyphrase keyword : this.keyphrases) {
            if (keyword.getName().equals(inKeywordName)) {
                for (User user : keyword.getUsers()) {
                    if (user.getName().equals(inUserName)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    public boolean getLaunch(String keywordNameToSet, String userNameToSet) {
        for (Keyphrase keyword : keyphrases) {
            if (keyword.getName().equals(keywordNameToSet)) {
                if (null == userNameToSet) {
                    Log.v(TAG, "Launch for keyword: " + keyword.name + "=" + keyword.getLaunch());
                    return keyword.getLaunch();
                } else {
                    for (User user : keyword.getUsers()) {
                        if (user.getName().equals(userNameToSet)) {
                            Log.v(TAG, "Launch for keyword: " + keyword.name +
                                    " user: " + user.getName() + "=" + user.getLaunch());
                            return user.getLaunch();
                        }
                    }
                }
            }
        }
        Log.e(TAG, "Confidence level could not be set because keyword or user could not be found");
        return false;
    }

    public boolean toggleLaunch(String keywordNameToSet, String userNameToSet) {
        for (Keyphrase keyword : keyphrases) {
            if (keyword.getName().equals(keywordNameToSet)) {
                if (null == userNameToSet) {
                    Log.v(TAG, "Launch set for keyword: " + keyword.name);
                    keyword.toggleLaunch();
                    return true;
                } else {
                    for (User user : keyword.getUsers()) {
                        if (user.getName().equals(userNameToSet)) {
                            Log.v(TAG, "Launch set for keyword: " + keyword.name +
                                    " user: " + user.getName());
                            user.toggleLaunch();
                            return true;
                        }
                    }
                }
            }
        }
        Log.e(TAG, "Confidence level could not be set because keyword or user could not be found");
        return false;
    }

    public int getConfidenceLevel(String inKeywordName, String inUserName) {
        if (null == inKeywordName) {
            Log.e(TAG, "Confidence level could not be set because keyword was null");
            return -1;
        }
        for (Keyphrase keyword : keyphrases) {
            if (keyword.getName().equals(inKeywordName)) {
                if (null == inUserName) {
                    Log.v(TAG, "Confidence level for keyword: " + keyword.name + "=" +
                            keyword.getConfidenceLevel());
                    return keyword.getConfidenceLevel();
                } else {
                    for (User user : keyword.getUsers()) {
                        if (user.getName().equals(inUserName)) {
                            Log.v(TAG, "Confidence level for keyword: " + keyword.name +
                                    " user: " + user.getName() + "=" + user.getConfidenceLevel());
                            return user.getConfidenceLevel();
                        }
                    }
                }
            }
        }
        Log.e(TAG, "Confidence level could not be set because keyword or user could not be found");
        return -1;
    }

    public boolean setConfidenceLevel(String keywordNameToSet, String userNameToSet,
                                      int inConfidenceLevel) {
        for (Keyphrase keyword : keyphrases) {
            if (keyword.getName().equals(keywordNameToSet)) {
                if (null == userNameToSet) {
                    Log.v(TAG, "Launch set for keyword: " + keyword.name);
                    keyword.setConfidenceLevel(inConfidenceLevel);
                    return true;
                } else {
                    for (User user : keyword.getUsers()) {
                        if (user.getName().equals(userNameToSet)) {
                            Log.v(TAG, "Launch set for keyword: " + keyword.name +
                                    " user: " + user.getName());
                            user.setConfidenceLevel(inConfidenceLevel);
                            return true;
                        }
                    }
                }
            }
        }
        Log.e(TAG, "Confidence level could not be set because keyword or user could not be found");
        return false;
    }

    //TODO remove if not needed
    /*public void addKeyword(String inName) {
        //TODO add check for already existing keyword
        keywords.add(new Keyword(inName));
    }*/

    //TODO remove if not needed
    /*public void addKeyword(String inName, int inConfidenceLevel, boolean inLaunchGoogleNow) {
        keywords.add(new Keyword(inName, inConfidenceLevel, inLaunchGoogleNow));
    }*/

    //TODO uncomment this when I start comparing current SMs to saved ones
    /*@Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + getOuterType().hashCode();
        result = prime * result + confidenceLevel;
        result = prime * result + (launchGoogleNow ? 1231 : 1237);
        result = prime * result + ((name == null) ? 0 : name.hashCode());
        result = prime * result + ((users == null) ? 0 : users.hashCode());
        return result;
    }*/

    public static enum Setting {
        Session,
        ConfidenceThreshold,
        LaunchPreference
    }

    // TODO add toString method
    /*@Override
    public String toString() {

    }*/

    //TODO switch to the eclipse implementation of equals once I start comparing
    //     current SMs to saves ones
    @Override
    public boolean equals(Object object) {
        boolean isSameObject = false;
        if (null != object && object instanceof SoundModel) {
            isSameObject = (this.name.equals(((SoundModel) object).getName()));
        }
        return isSameObject;
    }

    @Override
    public int compareTo(SoundModel another) {
        return this.name.compareToIgnoreCase(another.name);
    }
}
