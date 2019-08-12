/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.watchmessenger;

import java.text.SimpleDateFormat;

import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.net.Uri;
import android.provider.Telephony.Sms;
import android.os.SystemProperties;

public class Utils {
    public static final String TIME_FORMAT_12_HOUR = "MMM dd  hh:mm a";
    public static final String READ = "Read";
    public static final String UNREAD = "Unread";
    private static enum DisplayType{
        CIRCULAR,
        RECTANGULAR
    }
    private static DisplayType displayPannelType;
    private static final String DISPLAY_TYPE = "ro.display.type";

    public static Uri updateProvider(Context context, String phoneNumber, String msgText){
        ContentValues contentValues = new ContentValues();
        contentValues.put(Sms.ADDRESS, phoneNumber);
        contentValues.put(Sms.BODY, msgText);
        contentValues.put(Sms.DATE,new SimpleDateFormat(Utils.TIME_FORMAT_12_HOUR).format(System.currentTimeMillis()));
        contentValues.put(Sms.STATUS, context.getString(R.string.status_sending));

        Uri rowIdUri = context.getContentResolver().insert(Sms.CONTENT_URI, contentValues);

        return rowIdUri;
    }

    // return a semicolon separated list of phone numbers from a smsto: uri.
    public static String getSmsRecipients(final Uri uri) {
        String recipients = uri.getSchemeSpecificPart();
        final int pos = recipients.indexOf('?');
        if (pos != -1) {
            recipients = recipients.substring(0, pos);
        }
        recipients = replaceUnicodeDigits(recipients).replace(',', ';');
        return recipients;
    }

    /**
     * Replace arabic/unicode digits with decimal digits.
     * @param number
     *            the number to be normalized.
     * @return the replaced number.
     */
    private static String replaceUnicodeDigits(final String number) {
        final StringBuilder normalizedDigits = new StringBuilder(number.length());
        for (final char c : number.toCharArray()) {
            final int digit = Character.digit(c, 10);
            if (digit != -1) {
                normalizedDigits.append(digit);
            } else {
                normalizedDigits.append(c);
            }
        }
        return normalizedDigits.toString();
    }

    /**
     * Convert the string time to TIME_FORMAT_12_HOUR.
     * @param time
     *            the time to be in time_format_12_hour.
     * @return the string for time format.
     */
    public static String getMsgDateFormat(final String time) {
        return (new SimpleDateFormat(Utils.TIME_FORMAT_12_HOUR).format(Long.parseLong(time))).toString();
    }

    /**
     * Convert the boolean read to readable message status.
     * @param read
     *            message status = read/unread.
     * @return the string for boolean read status.
     */
    public static String getMsgStatus(final int read, final String status) {
        String msg_status=UNREAD;
        if(status.equals("-1")) {
            if(read!=0) {
                msg_status = READ;
            }
        return msg_status;
        }
        return status;
    }

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
