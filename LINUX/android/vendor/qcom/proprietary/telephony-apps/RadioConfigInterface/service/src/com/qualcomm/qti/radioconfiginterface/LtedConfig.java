/******************************************************************************
  @file    LtedConfig.java

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import android.util.Log;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;
import java.util.UUID;
import android.os.Parcelable;
import android.os.Parcel;
import com.qualcomm.qti.radioconfiginterface.RadioConfigItem;

/**
 * LTE-D Configuration.
 * Contains the LTE-D configuration parameters including the announcing and monitoring policies,
 * APN to be used for LTED server communication, BAK password and OS ID.
 */
public class LtedConfig implements Parcelable {

    /**
     * PLMN.
     * Contains the PLMN details inluding MCC and MNC.
     */
    public static final class Plmn {
        private String mcc;
        private String mnc;

        public Plmn(String mcc, String mnc) {
            this.mcc = mcc;
            this.mnc = mnc;
        }

        /**
         * Get MCC.
         * Returns MCC field.
         *
         * @return MCC
         */
        public String getMcc() {
            return mcc;
        }

        /**
         * Set MCC.
         * Set MCC field.
         *
         * @return none
         */
        public void setMcc(String mcc) {
            this.mcc = mcc;
        }

        /**
         * Get MNC.
         * Returns MNC field.
         *
         * @return MNC
         */
        public String getMnc() {
            return mnc;
        }

        /**
         * Set MNC.
         * Set MNC field.
         *
         * @return none
         */
        public void setMnc(String mnc) {
            this.mnc = mnc;
        }

        /**
         * Convert PLMN to string.
         * Convert PLMN to readable string
         *
         * @return string
         */
        public String toString() {
            StringBuilder builder = new StringBuilder();
            builder.append("Plmn{");
            builder.append("mcc=").append(mcc);
            builder.append(", mnc=").append(mnc);
            builder.append("}");
            return builder.toString();
        }
    }

    /**
     * Range.
     * Announcing or monitorying range.
     */
    public static final class Range {
        public static final int INVALID = 0;
        public static final int SHORT   = 1;
        public static final int MEDIUM  = 2;
        public static final int LONG    = 3;

        private int range;

        public Range(int range) {
            this.range = range;
        }

        public void setRange(int range) {
            this.range = range;
        }

        public int getRange() {
            return range;
        }

        /**
         * Convert range to string.
         * Convert range to readable string
         *
         * @return string
         */
        public String toString() {
            StringBuilder builder = new StringBuilder();
            builder.append("Range{");
            builder.append("range=").append(range);
            builder.append("}");
            return builder.toString();
        }
    }


    /**
     * Announcing Policy.
     * Announcing policy details.
     */
    public static final class AnnouncingPolicy {
        private Plmn  mPlmn;
        private int   mValidityTime;
        private Range mRange;

        /**
         * Get PLMN.
         * Returns announcing policy PLMN.
         *
         * @return PLMN
         */
        public Plmn getPlmn() {
            return mPlmn;
        }

        /**
         * Set PLMN.
         * Set announcing policy PLMN.
         *
         * @return none
         */
        public void setPlmn(Plmn plmn) {
            mPlmn = plmn;
        }

        /**
         * Get Validity time.
         * Returns announcing policy validity time.
         *
         * @return validity time
         */
        public int getValidityTime() {
            return mValidityTime;
        }

        /**
         * Set Validity time.
         * Set announcing policy validity time.
         *
         * @return none
         */
        public void setValidityTime(int validityTime) {
            mValidityTime = validityTime;
        }

        /**
         * Get Range.
         * Returns announcing policy range
         *
         * @return range
         */
        public Range getRange() {
            return mRange;
        }

        /**
         * Set Range.
         * Set announcing policy range
         *
         * @return none
         */
        public void setRange(Range range) {
            mRange = range;
        }

        /**
         * Convert to string.
         * Convert announcing policy to readable string
         *
         * @return string
         */
        public String toString() {
            StringBuilder builder = new StringBuilder();
            builder.append("AnnouncingPolicy{");
            builder.append("plmn=").append(mPlmn);
            builder.append(", validityTime=").append(mValidityTime);
            builder.append(", range=").append(mRange);
            builder.append("}");
            return builder.toString();
        }
    }

    /**
     * Monitoring Policy.
     * Monitoring policy details.
     */
    public static final class MonitoringPolicy {
        private Plmn mPlmn;
        private int  mValidityTime;
        private int  mRemainingTime;

        /**
         * Get PLMN.
         * Returns monitoring policy PLMN.
         *
         * @return PLMN
         */
        public Plmn getPlmn() {
            return mPlmn;
        }

        /**
         * Set PLMN.
         * Set monitoring policy PLMN.
         *
         * @return none
         */
        public void setPlmn(Plmn plmn) {
            mPlmn = plmn;
        }

        /**
         * Get validity time.
         * Returns monitoring policy validity time
         *
         * @return validity time
         */
        public int getValidityTime() {
            return mValidityTime;
        }

        /**
         * Set validity time.
         * Set monitoring policy validity time
         *
         * @return none
         */
        public void setValidityTime(int validityTime) {
            mValidityTime = validityTime;
        }

        /**
         * Get remaining time.
         * Returns monitoring policy remaining time
         *
         * @return remaining time
         */
        public int getRemainingTime() {
            return mRemainingTime;
        }

        /**
         * Set remaining time.
         * Set monitoring policy remaining time
         *
         * @return none
         */
        public void setRemainingTime(int remainingTime) {
            mRemainingTime = remainingTime;
        }

        /**
         * Convert to string.
         * Convert monitoring policy to string
         *
         * @return string
         */
        public String toString() {
            StringBuilder builder = new StringBuilder();
            builder.append("MonitoringPolicy{");
            builder.append("plmn=").append(mPlmn);
            builder.append(", validityTime=").append(mValidityTime);
            builder.append(", remainingTime=").append(mRemainingTime);
            builder.append("}");
            return builder.toString();
        }
    }

    private String mApn;
    private List<AnnouncingPolicy> mAnnouncingPolicyList;
    private List<MonitoringPolicy> mMonitoringPolicyList;
    private byte []mBakPassword;
    private UUID mOsId;

    /**
     * Get APN.
     *
     * @return The APN
     */
    public String getApn() {
        return mApn;
    }

    public void setApn(String apn) {
        mApn = apn;
    }

    /**
     * Get Announcing Policy count
     *
     * @return The count of Announcing Policies
     */
    public int getAnnouncingPolicyCount() {
        int count = 0;
        if (mAnnouncingPolicyList != null) {
            count = mAnnouncingPolicyList.size();
        }
        return count;
    }

    /**
     * Get Announcing Policy List.
     *
     * @return The list of Announcing Policies
     */
    public List<AnnouncingPolicy> getAnnouncingPolicyList() {
        return mAnnouncingPolicyList;
    }

    /**
     * Set Announcing Policy List.
     *
     * @return None
     */
    public void setAnnouncingPolicyList(List<AnnouncingPolicy> list) {
        mAnnouncingPolicyList = list;
    }

    /**
     * Get Monitoring Policy count.
     *
     * @return The count of Monitoring Policies
     */
    public int getMonitoringPolicyCount() {
        int count = 0;
        if (mMonitoringPolicyList != null) {
            count = mMonitoringPolicyList.size();
        }
        return count;
    }

    /**
     * Get Monitoring Policy List.
     *
     * @return The list of Monitoring Policies
     */
    public List<MonitoringPolicy> getMonitoringPolicyList() {
        return mMonitoringPolicyList;
    }

    /**
     * Set Monitoring Policy List.
     *
     * @return None
     */
    public void setMonitoringPolicyList(List<MonitoringPolicy> list) {
        mMonitoringPolicyList = list;
    }

    /**
     * Get BAK Password.
     *
     * @return The BAK Password
     */
    public byte []getBakPassword() {
        return mBakPassword;
    }

    /**
     * Set BAK Password.
     *
     * @return None
     */
    public void setBakPassword(byte []bakPassword) {
        mBakPassword = bakPassword;
    }

    /**
     * Get OS ID
     *
     * @return The OS ID
     */
    public UUID getOsId() {
        return mOsId;
    }

    /**
     * Set OS ID
     *
     * @return None
     */
    public void setOsId(UUID osId) {
        mOsId = osId;
    }

    /**
     * Copy from another object
     *
     * @return None
     */
    public void copyFrom(LtedConfig c) {
        mApn = c.mApn;
        mAnnouncingPolicyList = c.mAnnouncingPolicyList;
        mMonitoringPolicyList = c.mMonitoringPolicyList;
        mBakPassword = c.mBakPassword;
    }

    /**
     * Convert the LtedConfig to readable string
     *
     * @return LtedConfig in readable string
     */
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append("LtedConfig{");
        builder.append("apn=").append(mApn);
        builder.append(", AnnouncingPolicyList=[");
        if (mAnnouncingPolicyList != null) {
            for(AnnouncingPolicy item : mAnnouncingPolicyList) {
                builder.append(item).append(",");
            }
        }
        builder.append("]");
        builder.append(", MonitoringPolicyList=[");
        if (mMonitoringPolicyList != null) {
            for(MonitoringPolicy item : mMonitoringPolicyList) {
                builder.append(item).append(",");
            }
        }
        builder.append("]");
        builder.append("}");
        return builder.toString();
    }

    /**
     * Default constuctor
     */
    public LtedConfig(){
    }

    /**
     * Constuctor.
     *
     * @param Parcel object
     */
    public LtedConfig(Parcel in){
        readFromParcel(in);
    }

    /**
     * Read from parcel object.
     *
     * @param Parcel object
     *
     * @return None
     */
    public void readFromParcel(Parcel in) {
        // APN
        if (in.readInt() == 1) {
            mApn = in.readString();
        }

        mAnnouncingPolicyList = new ArrayList<AnnouncingPolicy>();

        int size = 0;

        // Announcing Policy size
        size = in.readInt();
        for(int i =0; i < size; i++) {
            AnnouncingPolicy item = new AnnouncingPolicy();

            // plmn
            if (in.readInt() == 1) {
                String mcc = in.readString();
                String mnc = in.readString();
                item.mPlmn = new Plmn(mcc, mnc);
            }
            // validityTime
            item.mValidityTime = in.readInt();
            // Range
            if (in.readInt() == 1) {
                item.mRange = new Range(in.readInt());
            }

            mAnnouncingPolicyList.add(item);
        }

        mMonitoringPolicyList = new ArrayList<MonitoringPolicy>();

        // Monitoring Policy size
        size = in.readInt();
        for(int i =0; i < size; i++) {
            MonitoringPolicy item = new MonitoringPolicy();

            // plmn
            if (in.readInt() == 1) {
                String mcc = in.readString();
                String mnc = in.readString();
                item.mPlmn = new Plmn(mcc, mnc);
            }
            // validityTime
            item.mValidityTime = in.readInt();
            // remainingTime
            item.mRemainingTime = in.readInt();

            mMonitoringPolicyList.add(item);
        }

        // BAK Password
        size = in.readInt();
        if (size > 0) {
            mBakPassword = new byte[size];
            in.readByteArray(mBakPassword);
        }

        if (in.readInt() == 1) {
            long lsb = in.readLong();
            long msb = in.readLong();
            mOsId = new UUID(msb, lsb);
        }
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        // APN
        if (mApn != null) {
            out.writeInt(1);
            out.writeString(mApn);
        } else {
            out.writeInt(0);
        }

        int size = 0;

        // Announcing Policy size
        if (mAnnouncingPolicyList != null) {
            size = mAnnouncingPolicyList.size();
        }
        out.writeInt(size);

        if (size > 0) {
            for(AnnouncingPolicy item : mAnnouncingPolicyList) {
                if (item.mPlmn != null) {
                    out.writeInt(1);
                    out.writeString(item.mPlmn.mcc);
                    out.writeString(item.mPlmn.mnc);
                } else {
                    out.writeInt(0);
                }
                out.writeInt(item.mValidityTime);
                if (item.mRange != null) {
                    out.writeInt(1);
                    out.writeInt(item.mRange.range);
                } else {
                    out.writeInt(0);
                }
            }
        }

        // Monitoring Policy size
        if (mMonitoringPolicyList != null) {
            size = mMonitoringPolicyList.size();
        } else {
            size = 0;
        }
        out.writeInt(size);

        if (size > 0) {
            for (MonitoringPolicy item : mMonitoringPolicyList) {
                if (item.mPlmn != null) {
                    out.writeInt(1);
                    out.writeString(item.mPlmn.mcc);
                    out.writeString(item.mPlmn.mnc);
                } else {
                    out.writeInt(0);
                }
                out.writeInt(item.mValidityTime);
                out.writeInt(item.mRemainingTime);
            }
        }

        // BAK Password
        if (mBakPassword != null) {
            size = mBakPassword.length;
        } else {
            size = 0;
        }
        out.writeInt(size);

        if (size > 0) {
            out.writeByteArray(mBakPassword);
        }

        if (mOsId != null) {
            out.writeInt(1);
            out.writeLong(mOsId.getLeastSignificantBits());
            out.writeLong(mOsId.getMostSignificantBits());
        } else {
            out.writeInt(0);
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<LtedConfig> CREATOR =
        new Parcelable.Creator<LtedConfig>() {

            public LtedConfig createFromParcel(Parcel in) {
                return new LtedConfig(in);
            }

            public LtedConfig[] newArray(int size) {
                return new LtedConfig[size];
            }
        };
}
