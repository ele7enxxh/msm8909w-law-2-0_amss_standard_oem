/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;


import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;

import com.android.location.XT.IXTSrv;
import com.android.location.XT.IXTSrvCb;

public class LocationIzatHandler {

    private static final String APP_TAG = "WatchSettings";
    private static final String IzatPackageName = "com.qualcomm.location.XT";

    private static final int POPUP_BOX_DISAGREE = 0;
    private static final int POPUP_BOX_AGREE = 1;
    private static final int PRINT = 1;
    private static final int IZat_MENU_TEXT_TITLE = 0;

    private IXTSrv izatService = null;
    private XTServiceConnection izatServiceConnection = null;

    private String izatMenuTitle = null;

    private LocationActivity locationActivity;

    LocationIzatHandler(LocationActivity locationActivity) {

        this.locationActivity = locationActivity;
        izatMenuTitle = locationActivity.getString(R.string.location_izat_title);
    }

    void toggleIzat() {
        if (izatService == null) {
            Log.v(APP_TAG, "izatService Null");
            return;
        }

        try {
            if (izatService.getStatus()) {
                izatService.disable();
            } else {
                izatService.showDialog();
            }

        } catch (RemoteException e) {
            e.printStackTrace();
        }

        locationActivity.updateRowValue();
    }

    String isIzatEnabled() {

        if (izatService == null) {
            Log.v(APP_TAG, "IZAT is null in isEnabled");
            return locationActivity.getString(R.string.location_mode_disabled);
        }

        try {
            if (izatService.getStatus()) {
                return  locationActivity.getString(R.string.location_mode_enabled);
            }
            else {
                return locationActivity.getString(R.string.location_mode_disabled);
            }
        }  catch (RemoteException e) {
            e.printStackTrace();
        }

        return locationActivity.getString(R.string.location_mode_disabled);
    }

    String getTitle() {
        return izatMenuTitle;
    }

    void shutdownService() {
        locationActivity.unbindService(izatServiceConnection);
    }

    void bindIzatService() {
        if (izatServiceConnection == null) {
            izatServiceConnection = new XTServiceConnection();
        }
        Intent i = new Intent(IXTSrv.class.getName());
        i.setPackage(IzatPackageName);
        locationActivity.bindService(i, izatServiceConnection, Context.BIND_AUTO_CREATE);
    }

    IBinder.DeathRecipient mDeathRecipient = new IBinder.DeathRecipient() {
        @Override
        public void binderDied() {
            bindIzatService();
        }
    };

    private IXTSrvCb mCallback = new IXTSrvCb.Stub() {
        public void statusChanged(boolean status) {
            if (false == status) {
                mHandler.sendMessage(mHandler.obtainMessage(PRINT, 0, 0));
            } else {
                mHandler.sendMessage(mHandler.obtainMessage(PRINT, 1, 0));
            }
        }
    };

    private class XTServiceConnection implements ServiceConnection {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            izatService = IXTSrv.Stub.asInterface((IBinder) service);
            Log.d(APP_TAG, "onServiceConnected, service=" + izatService);
            try {
                if (null != izatService) {
                    izatMenuTitle = izatService.getText(IZat_MENU_TEXT_TITLE);

                    izatService.registerCallback(mCallback);
                    service.linkToDeath(mDeathRecipient, 0);
                }
            } catch (RemoteException e) {
                Log.d(APP_TAG, "Failed connecting service!");
            }

            locationActivity.updateRowValue();
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            if (null != izatService) {
                try {
                    izatService.unregisterCallback(mCallback);
                } catch (RemoteException e) {
                }
                izatService = null;
            }
        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case PRINT:
                    if (POPUP_BOX_DISAGREE == msg.arg1) {
                        Log.v(APP_TAG, "Izat Eula disagree");
                    } else if (POPUP_BOX_AGREE == msg.arg1) {
                        Log.v(APP_TAG, "Izat Eula Agree");
                    }

                    locationActivity.updateRowValue();
                    break;
                default:
                    super.handleMessage(msg);
            }
        }
    };
}
