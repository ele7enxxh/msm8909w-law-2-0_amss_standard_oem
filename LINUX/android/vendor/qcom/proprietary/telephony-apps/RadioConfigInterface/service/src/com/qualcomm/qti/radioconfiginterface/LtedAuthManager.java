/******************************************************************************
  @file    LtedAuthManager.java

  ---------------------------------------------------------------------------
  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import android.content.pm.Signature;
import android.content.pm.PackageManager;
import android.content.res.XmlResourceParser;
import android.content.Context;
import android.os.SystemProperties;
import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;


/**
 * Verify if the applications are authorized to use the LTE-D provisioning APIs.
 */
public class LtedAuthManager {
    private final String LOG_TAG = "LtedAuthManager";

    private static class Application {
        public String name;
        public String key;
        public boolean parsingFail;
    }

    private List<String> mLtedWhiteList = new ArrayList<String>();
    private Context mContext;
    private boolean mIgnoreCertCheck = false;

    public LtedAuthManager(Context context) {
        mContext = context;
        // Initialize
        getWhiteList();
        mIgnoreCertCheck = SystemProperties.getBoolean("persist.lted.prov_ignore_cert", false);
    }

    private Application readApplication(XmlResourceParser parser)
        throws XmlPullParserException, IOException {
        parser.require(XmlPullParser.START_TAG, null, "Application");
        Application app = new Application();
        int eventType = parser.next();
        while(eventType != XmlPullParser.END_TAG) {
            if(eventType != XmlPullParser.START_TAG) {
                app.parsingFail = true;
                Log.e(LOG_TAG, "parse fail");
                break;
            }
            String tagName = parser.getName();
            if(tagName.equals("PackageName")){
                eventType = parser.next();
                if(eventType == XmlPullParser.TEXT){
                    app.name = parser.getText();
                    eventType = parser.next();
                }
                if((eventType != XmlPullParser.END_TAG) || !(parser.getName().equals("PackageName"))){
                     //Invalid tag or invalid xml format
                    app.parsingFail = true;
                    Log.e(LOG_TAG, "parse fail");
                    break;
                }
            } else {
                 app.parsingFail = true;
                 Log.e(LOG_TAG, "parse fail" + tagName);
                 break;
            }
            eventType = parser.next();
        }
        if((eventType != XmlPullParser.END_TAG) || !(parser.getName().equals("Application"))){
            //End Tag that ended the loop is not Application
            app.parsingFail = true;
        }
        return app;
    }

    private void getWhiteList(){
        Log.d(LOG_TAG, "getWhiteList");
        try {
            String name = null;
            Application app = null;
            boolean fail = false;
            int eventType;
            ArrayList<String> table = new ArrayList<String>();

            XmlResourceParser parser = mContext.getResources().getXml(R.xml.lted_applist);
            parser.next();

            //Get the parser to point to Entries Tag.
            if(parser.getEventType() == XmlPullParser.START_DOCUMENT){
                name = parser.getName();
                while(name == null ||
                       (name != null && !name.equals("Entries"))){
                    parser.next();
                    name = parser.getName();
                }
            }

            parser.require(XmlPullParser.START_TAG, null, "Entries");
            eventType = parser.next();

            //Loop until END_TAG is encountered
            while(eventType != XmlPullParser.END_TAG) {

                //If the TAG is not a START_TAG, break the loop
                //with Failure.
                if(eventType != XmlPullParser.START_TAG) {
                    fail = true;
                    Log.e(LOG_TAG, "parse fail");
                    break;
                }

                name = parser.getName();
                if(name.equals("Application")) {
                    app = readApplication(parser);
                    if(app.parsingFail){
                        fail = true;
                        Log.e(LOG_TAG, "parse fail");
                        break;
                    } else if(app.name != null) {
                        table.add(app.name);
                        Log.e(LOG_TAG, "appname: " + app.name);
                    }
                } else {
                    fail = true;
                    Log.e(LOG_TAG, "parse fail" + name);
                    break;
                }
                eventType = parser.next();
            }
            if(fail || eventType != XmlPullParser.END_TAG ||
                         !(parser.getName().equals("Entries"))){
                //parsing failure
                Log.e(LOG_TAG, "FAIL");
            }
            else if(!table.isEmpty()) {
                 mLtedWhiteList = Collections.unmodifiableList(table);
            }
        }
        catch(Exception e) {
            Log.e(LOG_TAG, "Exception: "+ e);
        }
    }

    public boolean verifyAuthenticity(int uid){
        boolean ret = false;

        Log.d(LOG_TAG, "uid = " + uid);

        if(mLtedWhiteList == null) {
            Log.d(LOG_TAG, "empty white list");
            return ret;
        }
        String[] packageNames = mContext.getPackageManager().getPackagesForUid(uid);
        for(String packageName : packageNames){
            Log.d(LOG_TAG, "packageName = " + packageName);
            if(mLtedWhiteList.contains(packageName)){
               Log.d(LOG_TAG, "lted_applist contains packageName = " + packageName);
               if (mIgnoreCertCheck) {
                   Log.d(LOG_TAG, "Ignore certificate check!!!");
                   ret = true;
                   break;
               }

               try {
                   Signature[] sigs = mContext.getPackageManager().getPackageInfo(
                           packageName, PackageManager.GET_SIGNATURES).signatures;
                   for(Signature sig: sigs) {

                       //get the raw certificate into input stream
                       final byte[] rawCert = sig.toByteArray();
                       InputStream certStream = new ByteArrayInputStream(rawCert);

                       //Read the X.509 Certificate into certBytes
                       final CertificateFactory certFactory =
                           CertificateFactory.getInstance("X509");
                       final X509Certificate x509Cert =
                           (X509Certificate)certFactory.generateCertificate(certStream);
                       byte[] certBytes = x509Cert.getEncoded();

                       if(certBytes != null) {
                          Log.d(LOG_TAG, "verifyAuthenticity: valid certificate found");
                          ret = true;
                          break;
                       }
                   }
                } catch(Exception e) {
                        Log.e(LOG_TAG, "Exception reading client data!" + e);
                }
            }
            if (ret) {
                break;
            }
        }
        Log.d(LOG_TAG, "verifyAuthenticity: return : " + ret);
        return ret;
    }
}
