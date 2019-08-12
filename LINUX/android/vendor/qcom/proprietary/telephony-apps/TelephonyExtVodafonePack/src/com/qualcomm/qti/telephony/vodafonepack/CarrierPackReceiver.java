/*
 * Copyright (c) 2015, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.telephony.vodafonepack;

import android.app.Notification;
import android.app.NotificationManager;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.qualcomm.qcrilhook.IQcRilHook;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;
import com.qualcomm.qti.telephony.vodafonepack.CarrierpackApplication;

/**
 * This CarrierPackReceiver is generic receiver which is used to handle any
 * broad cast intents and do required process.
 */
public class CarrierPackReceiver extends BroadcastReceiver{

    private static final boolean DBG = true;

    private static final String LOG_TAG = "CarrierPackReceiver";
    private static final String ACTION_WIFI_CONNECTION_STATE_CHANGED =
            "org.codeaurora.wifibearer.WIFI_CONNECTION_STATE";
    private static final String ACTION_CSG_ID_CHANGED_IND =
            "qualcomm.intent.action.ACTION_CSG_ID_CHANGED_IND";
    private static final String ACTION_BOOT_COMPLETED =
            "android.intent.action.BOOT_COMPLETED";

    private static final String WIFI_STATUS = "Wifi_Status";
    private static final String CSG_ID = "csgId";

    //Unique number to show icon on notification bar
    private static final int WIFI_CALLING = 100;

    private Context mContext;

    @Override
    public void onReceive(Context context, Intent intent) {
        mContext = context;
        boolean vodafonePackEnabled = mContext.getResources()
                .getBoolean(R.bool.enable_vodafone_pack);
        log("onReceive vodafonePackEnabled="+vodafonePackEnabled+", intent="+intent.getAction());
        if (vodafonePackEnabled &&
                ACTION_WIFI_CONNECTION_STATE_CHANGED.equals(intent.getAction())) {
            boolean showWifiIcon = intent.getBooleanExtra(WIFI_STATUS, false);
            updateWifiIcon(showWifiIcon);
        } else if (vodafonePackEnabled && ACTION_CSG_ID_CHANGED_IND.equals(intent.getAction())) {
            int csgId = intent.getIntExtra(CSG_ID, CarrierpackApplication.INVALID_CSG_ID);
            log("ACTION_CSG_ID_CHANGED_IND received : " + csgId);
            Handler cpHandler = CarrierpackApplication.getInstance().getHandler();
            cpHandler.sendMessage(cpHandler
                    .obtainMessage(CarrierpackApplication.EVT_CSG_CHANGED, new Integer(csgId)));
        } else if (vodafonePackEnabled && ACTION_BOOT_COMPLETED.equals(intent.getAction())) {
            log("ACTION_BOOT_COMPLETED received");
            Handler cpHandler = CarrierpackApplication.getInstance().getHandler();
            cpHandler.sendMessage(cpHandler
                    .obtainMessage(CarrierpackApplication.EVT_BOOT_COMPLETED));
        }
    }

    private void updateWifiIcon(boolean showWifiIcon) {
        log("updateWifiIcon showWifiIcon : " + showWifiIcon);
        NotificationManager notificationMgr =
                (NotificationManager) mContext.getSystemService(Context.NOTIFICATION_SERVICE);
        if (showWifiIcon) {
            Notification.Builder builder = new Notification.Builder(mContext);
            builder.setOngoing(true);
            builder.setPriority(Notification.PRIORITY_HIGH);
            builder.setContentTitle(mContext.getResources().getString(R.string.calls_are_on_wifi));
            builder.setSmallIcon(R.drawable.wifi);
            Notification notification = builder.build();
            notificationMgr.notify(WIFI_CALLING, notification);
        } else {
            notificationMgr.cancel(WIFI_CALLING);
        }
    }

    private void log(String string) {
        Log.d(LOG_TAG, string);
    }

}
