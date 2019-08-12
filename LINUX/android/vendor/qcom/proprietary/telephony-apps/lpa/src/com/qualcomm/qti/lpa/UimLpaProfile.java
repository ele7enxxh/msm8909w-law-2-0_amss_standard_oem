/******************************************************************************
 * @file    UimLpaProfile.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/


package com.qualcomm.qti.lpa;

import android.os.Parcelable;
import android.os.Parcel;
import java.util.Arrays;


public class UimLpaProfile implements Parcelable {

    public final int UIM_LPA_ICON_TYPE_JPEG = 0;
    public final int UIM_LPA_ICON_TYPE_PNG = 1;

    public int      profileState;
    public String   nickname;
    public String   profileName;
    public byte[]   iccid;
    public String   profileServiceProviderName;
    public int      iconType;
    public byte[]   icon;

    public UimLpaProfile() {
    }

    //decode from parcel
    public UimLpaProfile(Parcel in) {

        int iconLen = 0;
        int iccidLen = 0;

        //profileName
        this.profileName = in.readString();

        //nickname
        this.nickname = in.readString();

        //profileState
        this.profileState = in.readInt();

        //iccid
        iccidLen = in.readInt();
        if (iccidLen != 0) {
            this.iccid = new byte[iccidLen];
            in.readByteArray(this.iccid);
        }

        //profileServiceProviderName
        this.profileServiceProviderName = in.readString();

        //iconType
        this.iconType = in.readInt();

        //icon
        iconLen = in.readInt();
        if (iconLen != 0) {
            this.icon = new byte[in.readInt()];
            in.readByteArray(this.icon);
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        //profileName
        dest.writeString(this.profileName);

        //nickname
        dest.writeString(this.nickname);

        //profileState
        dest.writeInt(this.profileState);

        //iccid
        if(this.iccid != null) {
            dest.writeInt(this.iccid.length);
            dest.writeByteArray(this.iccid);
        } else {
            dest.writeInt(0);
        }

        //profileServiceProviderName
        dest.writeString(this.profileServiceProviderName);

        //iconType
        dest.writeInt(this.iconType);

        //icon
        if (this.icon != null) {
            dest.writeInt(this.icon.length);
            dest.writeByteArray(this.icon);
        } else {
            dest.writeInt(0);
        }
    }

    public static final Creator<UimLpaProfile> CREATOR =
                                      new Creator<UimLpaProfile>() {

        public UimLpaProfile createFromParcel(Parcel in) {
            return new UimLpaProfile(in);
        }

        public UimLpaProfile[] newArray(int size) {
            return new UimLpaProfile[size];
        }
    };

    public void setProfileState(int profileState) {
        this.profileState = profileState;
    }

    public int getProfileState() {
        return profileState;
    }

    public void setNickname(String nickname) {
        this.nickname = nickname;
    }

    public String getNickname() {
        return nickname;
    }

    public void setProfilename(String profileName) {
        this.profileName = profileName;
    }

    public String getProfileName() {
        return profileName;
    }

    public void setIccid(byte[] iccid) {
        if(iccid != null) {
            this.iccid = Arrays.copyOf(iccid, iccid.length);
        }
    }

    public byte[] geticcid() {
        return iccid;
    }

    public void setProfileServiceProviderName(String profileServiceProviderName) {
        this.profileServiceProviderName = profileServiceProviderName;
    }

    public String getProfileServiceProviderName() {
        return profileServiceProviderName;
    }

    public void setIconType(int iconType) {
        this.iconType = iconType;
    }

    public int getIconType() {
        return iconType;
    }

    public void setIcon(byte[] icon) {
        if (icon != null) {
            this.icon = Arrays.copyOf(icon, icon.length);
        }
    }

    public byte[] getIcon() {
        return icon;
    }

    @Override
    public String toString() {
        if (this.profileName != null) {
            return this.profileName.toString();
        } else {
            return new String("No name");
        }
    }

}

