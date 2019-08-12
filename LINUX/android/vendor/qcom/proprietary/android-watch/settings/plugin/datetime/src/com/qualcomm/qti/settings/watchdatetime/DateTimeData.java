/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchdatetime;

import java.util.Calendar;
import android.content.Context;
import android.content.ContentResolver;
import java.util.TimeZone;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.util.Log;

public class DateTimeData {

    public boolean getAutoDateTime(String name, Context c) {
    try{
       return Settings.Global.getInt(c.getContentResolver(), name) > 0;
     } catch (SettingNotFoundException snfe) {
       return false;
       }
    }

    public void setAutoDateTime(int enable, Context c) {
      Settings.Global.putInt(c.getContentResolver(),
          Settings.Global.AUTO_TIME, enable);
    }

    public void setAutoTimeZone(int enable, Context c) {
      Settings.Global.putInt(c.getContentResolver(),
          Settings.Global.AUTO_TIME_ZONE, enable);
    }

    public static String getGmtOffset(String mTimeZoneId) {
        TimeZone mTimeZone = TimeZone.getTimeZone(mTimeZoneId);
        Calendar calendar = Calendar.getInstance(mTimeZone);
        int mOffsetInMillis = mTimeZone.getOffset(calendar.getTimeInMillis());

        //Calculate offset in hh:min format
        String mOffset = String.format("%02d:%02d",
            Math.abs(mOffsetInMillis / 3600000),
            Math.abs((mOffsetInMillis / 60000) % 60));
        mOffset = "GMT" + (mOffsetInMillis >= 0 ? "+" : "-") + mOffset;

        return mOffset;
    }
}
