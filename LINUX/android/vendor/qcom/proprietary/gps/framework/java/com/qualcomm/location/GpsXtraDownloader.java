/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

  Not a Contribution, Apache license notifications and
  license are retained for attribution purposes only.
=============================================================================*/

/*
 * Copyright (C) 2008 The Android Open Source Project
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

package com.qualcomm.location;

import android.text.TextUtils;
import android.util.Log;

import java.net.HttpURLConnection;
import java.net.URL;
import libcore.io.Streams;

import android.content.Context;
import android.os.Build;
import android.os.SystemClock;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionInfo;

import java.io.IOException;
import java.util.Properties;
import java.util.Random;
import java.util.List;
import java.util.ArrayList;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import java.util.HashMap;

/**
 * A class for downloading GPS XTRA data.
 *
 * {@hide}
 */
public class GpsXtraDownloader {

    private static final String TAG = "GpsXtraDownloader-Q";
    static final boolean DEBUG = Log.isLoggable(TAG, Log.VERBOSE);
    private static final String DEFAULT_USER_AGENT = "Android";
    private static final int HTTP_CONNECTION_READ_TIMEOUT_MS = 60000;
    private static final int MAX_XTRA_FILE_SIZE = 100 * 1024;  // max XTRA file size is 100KB

    private Context mContext;
    private final String[] mXtraServers;
    // to load balance our server requests
    private int mNextServerIndex;
    private final String mUserAgent;

    GpsXtraDownloader(Context context, Properties properties) {
        mContext = context;

        // read XTRA servers from the Properties object
        int count = 0;
        String server1 = properties.getProperty("XTRA_SERVER_1");
        String server2 = properties.getProperty("XTRA_SERVER_2");
        String server3 = properties.getProperty("XTRA_SERVER_3");
        if (server1 != null) {
            if (server1.length() != 0) count++;
        }
        if (server2 != null) {
            if (server2.length() != 0) count++;
        }
        if (server3 != null) {
            if (server3.length() != 0) count++;
        }

        // Set User Agent from properties, if possible.
        String agent = properties.getProperty("XTRA_USER_AGENT");
        if (TextUtils.isEmpty(agent)) {
            mUserAgent = DEFAULT_USER_AGENT;
        } else {
            mUserAgent = agent;
        }

        if (count == 0) {
            Log.e(TAG, "No XTRA servers were specified in the GPS configuration");
            mXtraServers = null;
        } else {
            mXtraServers = new String[count];
            count = 0;
            if (server1 != null) {
                if (server1.length() != 0) mXtraServers[count++] = server1;
            }
            if (server2 != null) {
                if (server2.length() != 0) mXtraServers[count++] = server2;
            }
            if (server3 != null) {
                if (server3.length() != 0) mXtraServers[count++] = server3;
            }

            // randomize first server
            Random random = new Random();
            mNextServerIndex = random.nextInt(count);
        }
    }

    byte[] downloadXtraData() {
        byte[] result = null;
        int startIndex = mNextServerIndex;

        if (mXtraServers == null) {
            return null;
        }

        // load balance our requests among the available servers
        while (result == null) {
            result = doDownload(mXtraServers[mNextServerIndex]);

            // increment mNextServerIndex and wrap around if necessary
            mNextServerIndex++;
            if (mNextServerIndex == mXtraServers.length) {
                mNextServerIndex = 0;
            }
            // break if we have tried all the servers
            if (mNextServerIndex == startIndex) break;
        }

        return result;
    }

    protected byte[] doDownload(String url) {
        if (DEBUG) Log.d(TAG, "Downloading XTRA data from " + url);

        TelephonyManager phone = (TelephonyManager)mContext.getSystemService(
                                                   mContext.TELEPHONY_SERVICE);
        HttpURLConnection connection = null;
        try {
            connection = (HttpURLConnection) (new URL(url)).openConnection();
            connection.setRequestProperty(
                    "Accept",
                    "*/*, application/vnd.wap.mms-message, application/vnd.wap.sic");
            connection.setRequestProperty(
                    "x-wap-profile",
                    "http://www.openmobilealliance.org/tech/profiles/UAPROF/ccppschema-20021212#");

            String uaInit = getUserAgentString("");
            if (DEBUG) Log.d(TAG, "initial UA string:" + uaInit);

            StringBuilder ua = new StringBuilder(uaInit);

            String socVersions = null;

            if (uaInit == null || uaInit.isEmpty()) {
                if (DEBUG) Log.e(TAG, "cannot read initial UA string");
                ua.append(mUserAgent);

            } else {
                // carrier
                final int FIELD_NUM_CARRIER = 5;
                String carrier = getCarrier(phone, "-");
                if (carrier != null && !carrier.isEmpty() && !carrier.equals("-")) {
                    int startOffset = getIndexOfNthField(ua, FIELD_NUM_CARRIER); //
                    if (startOffset >= 0) {
                        ua.replace(startOffset, startOffset + 1, carrier);
                    } else {
                        if (DEBUG) Log.e(TAG, "cannot find carrier field in " + ua);
                    }
                }

                // TAC
                final int FIELD_NUM_TAC = 8;
                String TAC = getTAC(phone, "-");
                if (TAC != null && !TAC.isEmpty() && !TAC.equals("-")) {
                    int startOffset = getIndexOfNthField(ua, FIELD_NUM_TAC); //
                    if (startOffset >= 0) {
                        ua.replace(startOffset, startOffset + 1, TAC);
                    } else {
                        if (DEBUG) Log.e(TAG, "cannot find TAC field in " + ua);
                    }
                }

                // MCC+MNC
                final int FIELD_NUM_MCCMNC = 10;
                String operators = getMccMnc(phone, "-");
                if (operators != null && !operators.isEmpty() && !operators.equals("-")) {
                    int startOffset = getIndexOfNthField(ua, FIELD_NUM_MCCMNC);
                    if (startOffset >= 0) {
                        ua.replace(startOffset, startOffset + 1, operators);
                    } else {
                        if (DEBUG) Log.e(TAG, "cannot find MCCMNC field in " + ua);
                    }
                }

                // time since boot of AP
                final int FIELD_NUM_TIME_SINCE_BOOT_AP = 12;
                if (true) {
                    int startOffset = getIndexOfNthField(ua, FIELD_NUM_TIME_SINCE_BOOT_AP);
                    long timeSinceBootAp = SystemClock.elapsedRealtime () / 1000;
                    if (startOffset >= 0) {
                        ua.replace(startOffset, startOffset + 1, String.valueOf(timeSinceBootAp));
                    } else {
                        if (DEBUG) Log.e(TAG, "cannot find time since boot of AP field in " + ua);
                    }
                }

                // SOC image versions
                final int FIELD_NUM_SOC_VERSIONS = 14;
                socVersions = getSocVersionsForUpload("-");
                if (socVersions != null && !socVersions.isEmpty() && !socVersions.equals("-")) {
                    int startOffset = getIndexOfNthField(ua, FIELD_NUM_SOC_VERSIONS);
                    if (startOffset >= 0) {
                        ua.replace(startOffset, startOffset + 1, socVersions);
                    } else {
                        if (DEBUG) Log.e(TAG, "cannot find SOC VERSIONS field in " + ua);
                    }
                }

                int start = ua.indexOf(" ");
                while (start != -1) {
                    ua.setCharAt(start, '#');
                    start = ua.indexOf(" ", start);
                }
            }

            String useragent = ua.toString();
            connection.setRequestProperty("User-Agent", useragent);

            if (DEBUG) Log.d(TAG, "Downloading XTRA data from " +
                             url + ": User-Agent: " + useragent);

            connection.setConnectTimeout(HTTP_CONNECTION_READ_TIMEOUT_MS);
            connection.setReadTimeout(HTTP_CONNECTION_READ_TIMEOUT_MS);
            connection.connect();

            int statusCode = connection.getResponseCode();
            if (statusCode != HttpURLConnection.HTTP_OK) {
               if (DEBUG) Log.d(TAG, "HTTP error downloading gps XTRA: " + statusCode);
               return null;
            }

            int length = connection.getContentLength();
            if (length > MAX_XTRA_FILE_SIZE) {
               if (DEBUG) {
                  Log.e(TAG, "HTTP error: too large XTRA file of size "
                             + length + " (>" +MAX_XTRA_FILE_SIZE + ")");
               }
               return null;
            }

            // save uploaded SOC_VERSIONS
            if (socVersions != null && !socVersions.isEmpty() && !socVersions.equals("-")) {
                saveUploadedSocVersions(socVersions);
            }

            return Streams.readFully(connection.getInputStream());
        } catch (IOException ioe) {
            if (DEBUG) Log.d(TAG, "Error downloading gps XTRA: ", ioe);
        } catch (OutOfMemoryError oom) {
            if (DEBUG) Log.e(TAG, "Security alert. reason:" + oom);
            return null;
        } catch (Exception e) {
            if (DEBUG) Log.e(TAG, "exception. reason:" + e);
            return null;
        } finally {
            if (connection != null) {
                connection.disconnect();
            }
        }
        return null;
    }

    private static String getUserAgentString(String def) {
        final String USER_AGENT_FILE = "/data/misc/location/xtra/useragent.txt";

        File file = new File(USER_AGENT_FILE);
        if (!file.exists()) {
            if (DEBUG) Log.d(TAG, "USER_AGENT_FILE not exist.");
            return def;
        }

        byte[] bytes = new byte[(int)file.length()];
        FileInputStream stream = null;

        try {
            stream = new FileInputStream(file);
            stream.read(bytes);

        } catch (FileNotFoundException e) {
            if (DEBUG) Log.e(TAG, "cannot open USER_AGEENT file");
            return def;

        } catch (IOException e) {
            if (DEBUG) Log.e(TAG, "I/O error in reading USER_AGENT file");
            return def;

        } finally {
            if (stream != null) {
                try {
                    stream.close();

                } catch (IOException e) {
                    if (DEBUG) Log.e(TAG, "I/O error in close USER_AGENT file");
                }
            }

        }

        String userAgent = new String(bytes);

        return userAgent;
    }

    private static final String SOC_VERSIONS_FILE = "/data/misc/location/xtra/soc_versions.txt";

    private void saveUploadedSocVersions(String socVersions) {
       FileOutputStream stream = null;
       File file = new File(SOC_VERSIONS_FILE);
       file.getParentFile().mkdirs();

       try {
           stream = new FileOutputStream(file);
           stream.write(socVersions.getBytes());

       } catch (FileNotFoundException e) {
           if (DEBUG) Log.e(TAG, "cannot open SOC_VERSIONS file");

       } catch (IOException e) {
           if (DEBUG) Log.e(TAG, "I/O error in writing SOC_VERSIONS file");

       } finally {
           if (stream != null) {
               try {
                   stream.close();

               } catch (IOException e) {
                   if (DEBUG) Log.e(TAG, "I/O error in close SOC_VERSIONS file");
               }
           }
       }
    }

    private String getSocVersionsForUpload(String def) {
        String socVersions = getSocVersions(def);
        if (socVersions.equals(def)) {
            // SOC_VERSIONS not accessible
            return def;
        }

        File file = new File(SOC_VERSIONS_FILE);
        if (!file.exists()) {
            // first upload after factory reset / OTA upgrade
            return socVersions;
        }

        byte[] bytes = new byte[(int)file.length()];
        FileInputStream stream = null;

        try {
            stream = new FileInputStream(file);
            stream.read(bytes);

        } catch (FileNotFoundException e) {
            if (DEBUG) Log.e(TAG, "cannot open SOC_VERSIONS file");
            return def;

        } catch (IOException e) {
            if (DEBUG) Log.e(TAG, "I/O error in reading SOC_VERSIONS file");
            return def;

        } finally {
            if (stream != null) {
                try {
                    stream.close();

                } catch (IOException e) {
                    if (DEBUG) Log.e(TAG, "I/O error in close SOC_VERSIONS file");
                }
            }
        }

        String prevSocVersions = new String(bytes);
        if (socVersions.equals(prevSocVersions)) {
            // SOC_VERSIONS already uploaded
            return def;
        }

        return socVersions;
    }

    private static final String SOC_IMAGES = "/sys/devices/soc0/images";
    private static final Pattern SOC_IMAGES_VERSION = Pattern.compile("^\\s*CRM:\\s*(.*?)\\s*$");
    /** the image versions file contains sections in the following format:
        <Image ID>:
        CRM:            <Image ID and Image Version>
        Variant:        <Variant>
        Version:        <OEM Version>
    */

    private String getSocVersions(String def) {
        BufferedReader socData = null;
        List<String> images = new ArrayList<String>();

        try {
            FileReader dataFile = new FileReader(SOC_IMAGES);
            socData = new BufferedReader(dataFile);
            String line;
            Matcher matcher;

            while((line = socData.readLine()) != null) {
                if (line.isEmpty()) {
                    continue;
                }

                if ((matcher = SOC_IMAGES_VERSION.matcher(line)).matches()) {
                    String imgIdVersion  = matcher.group(1);

                    images.add(imgIdVersion);
                }
            }

        } catch (Exception e) {
            if (DEBUG) Log.d(TAG, "error " + e);
            return def;

        } finally {
            if( socData != null) {
                try {
                    socData.close();
                } catch (IOException e) {
                    if (DEBUG) Log.d(TAG, "error " + e);
                }
            }
        }

        if(images.isEmpty()) {
            if (DEBUG) Log.d(TAG, "SOC_IMAGE file empty");
            return def;

         } else {
            return encode(TextUtils.join("+", images));
         }
    }

    private String getCarrier(TelephonyManager phone, String def) {
        return phone != null ? encode(phone.getNetworkOperatorName()) : def;
    }

    private String getTAC(TelephonyManager phone, String def) {
        String IMEI = phone.getDeviceId();
        if( IMEI == null || IMEI.length() < 8 ) {
            return def;

         } else {
            return IMEI.substring(0, 8);
         }
    }

    private String getMccMnc(TelephonyManager phone, String def) {
        List<String> mccmnc = new ArrayList<String>();

        String operator  = phone.getNetworkOperator();
        if (operator == null || operator.isEmpty()) {
            mccmnc.add(def);

        } else {
            // note: first 3 characters are for MCC
            String mccmnc_formatted =
                    String.format("%s|%s", operator.substring(0,3), operator.substring(3));
            mccmnc.add(mccmnc_formatted);
        }

        SubscriptionManager sm = SubscriptionManager.from(mContext);
        List<SubscriptionInfo> siList = sm.getActiveSubscriptionInfoList();
        if (siList == null || siList.size() == 0) {
            return TextUtils.join("+", mccmnc);
        }

        for (SubscriptionInfo si : siList) {
            String mccmnc_formatted = String.format("%d|%02d", si.getMcc(), si.getMnc());
            mccmnc.add(mccmnc_formatted);
        }

        return TextUtils.join("+", mccmnc);
    }

    private int getIndexOfNthField(StringBuilder str, int n) {
        if (n == 0) {
            return 0;
        }

        int pos = -1;
        while (n-- > 0) {
            if ((pos = str.indexOf("/", pos + 1)) < 0) {
                break;
            }
        }

        if (pos < 0) {
            // not found
            return -1;
        }

        return pos + 1;
    }

    /**
     *  encode the given string value such that all separator characters ('/','+','|','%')
     *  in the string are replaced by their corresponding encodings (%2F","%2B","%7C", "%25")
     */
    private static final HashMap<Character, String> encodings = new HashMap<Character, String>() {
        HashMap<Character, String> init() {
            put('/', "%2F");
            put('+', "%2B");
            put('|', "%7C");
            put('%', "%25");
            return this;
        }
    }.init();

    private String encode(String value) {
        StringBuilder sb = new StringBuilder();

        for(int pos = 0; pos < value.length(); pos++) {
            char ch = value.charAt(pos);

            if (encodings.containsValue(ch)) {
                sb.append(encodings.get(ch));
            } else {
                sb.append(ch);
            }
        }

        return sb.toString();
    }
}
