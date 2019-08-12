/*
 * Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package org.codeaurora.ims;

import android.app.Notification;
import android.app.NotificationManager;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.UserHandle;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;

import com.qualcomm.ims.utils.Log;

import java.util.List;

/**
 * This ImsServiceStateReceiver is generic class which is used to handle
 * ims service state and do required process.
 */
public class ImsServiceStateReceiver {
    private static final String LOG_TAG = "ImsServiceStateReceiver";
    //This flag will be set to false during initialization.
    private static boolean mShowHDIcon = true;
    private static boolean mShowVOLTEIcon = false;

    private static NotificationManager mNotificationMgr = null;

    private static boolean mIsWifiCallIntentSent = true;

    private static final String ACTION_WIFI_CONNECTION_STATE_CHANGED =
            "org.codeaurora.wifibearer.WIFI_CONNECTION_STATE";
    private static final String WIFI_STATUS = "Wifi_Status";

    //Unique number to show HD icon on notification bar
    private static final int IMS_HD_ICON = 1000;

    public static void updateHDIcon(Context context, int phoneId, boolean isVideoCapable,
            boolean isVoiceCapable) {
        Log.i(LOG_TAG, "updateHDIcon isVideo : " + isVideoCapable + " isVoice : " +
                isVoiceCapable + " phoneId: " + phoneId + " show HD Icon: " + mShowHDIcon);
        if (ImsCallUtils.isCarrierOneSupported()) {
            return;
        }

        if (isShowHDIconSupported(context, phoneId)) {
            showHDIcon(context, isVideoCapable || isVoiceCapable);
        } else {
            // Remove the HD icon during phone process crash/SIM Remove.
            showHDIcon(context, false);
        }
    }

    public static void overrideNotificationAppName(Context context, Notification.Builder n) {
        final Bundle extras = new Bundle();
        extras.putString(Notification.EXTRA_SUBSTITUTE_APP_NAME,
                context.getResources().getString(R.string.hd_icon_header_app_name));
        n.addExtras(extras);
    }

    private static void showHDIcon(Context context, boolean showHDIcon) {
        if (mShowHDIcon == showHDIcon) return;

        if (mNotificationMgr == null) {
            mNotificationMgr = (NotificationManager)context.getSystemService(
                    Context.NOTIFICATION_SERVICE);
        }

        mShowHDIcon = showHDIcon;
        if (mShowHDIcon) {
            Notification.Builder builder = new Notification.Builder(context);
            builder.setPriority(Notification.PRIORITY_HIGH);
            if (mShowVOLTEIcon) {
                builder.setContentTitle(context.getResources().getString(
                        R.string.device_is_volte_capable));
                builder.setSmallIcon(R.drawable.volte_icon);
            } else {
                builder.setContentTitle(context.getResources().getString(
                        R.string.device_is_hd_capable));
                builder.setSmallIcon(R.drawable.ims_state);
            }
            builder.setShowWhen(false);
            overrideNotificationAppName(context, builder);
            Notification notification = builder.build();
            notification.flags |= Notification.FLAG_NO_CLEAR;
            mNotificationMgr.notifyAsUser(null, IMS_HD_ICON, notification,
                    UserHandle.ALL);
        } else {
            mNotificationMgr.cancelAsUser(null, IMS_HD_ICON, UserHandle.ALL);
        }
    }

    private static boolean isShowHDIconSupported(Context context, int phoneId) {
        boolean showHDIcon = false;
        SubscriptionInfo subInfo = SubscriptionManager.from(context).
                getActiveSubscriptionInfoForSimSlotIndex(phoneId);
        if (subInfo != null && SubscriptionManager.isValidSubscriptionId(
                subInfo.getSubscriptionId())) {
            Resources subRes = SubscriptionManager.getResourcesForSubId(context,
                    subInfo.getSubscriptionId());
            showHDIcon = subRes.getBoolean(R.bool.config_update_service_status);
            mShowVOLTEIcon = subRes.getBoolean(R.bool.config_update_volte_icon);
        }
        Log.i(LOG_TAG, "isShowHDIconSupported config showHDIcon : " + showHDIcon);
        return showHDIcon;
    }

    public static void validateAndUpdateWifiIconStatus(ImsServiceSub serviceSub,
                Context context, boolean isVoiceSupportedOverWifi,
                boolean isVideoSupportedOverWifi) {
        boolean wifiIconStatus = false;
        if (!isVoiceSupportedOverWifi && !isVideoSupportedOverWifi) {
            wifiIconStatus = false;
        } else if (isCallExistsWithState(serviceSub, DriverCallIms.State.ACTIVE) ||
                isCallExistsWithState(serviceSub, DriverCallIms.State.HOLDING)) {
            wifiIconStatus = true;
        }
        //ACTION_WIFI_CONNECTION_STATE_CHANGED intent will be broadcasted if there is
        //volte/vt call present on WiFi.
        sendWifiIconIntent(context, wifiIconStatus);
    }

    /**
     * check volte/vt call exists with DriverCallIms.State.
     */
    private static boolean isCallExistsWithState(ImsServiceSub serviceSub,
            DriverCallIms.State state) {
        List<ImsCallSessionImpl> sessionList = serviceSub.getCallSessionByState(state);
        return sessionList.isEmpty() ? false : true;
    }

    /**
     * Broadcast intent with wifi icon visibility depends on volte/vt calls present on WiFi.
     * Clients can listen and take action depends on wifi icon status.
     */
    private static void sendWifiIconIntent(Context context, boolean showWifiIcon) {
        if (mIsWifiCallIntentSent == showWifiIcon) return;
        Log.i(LOG_TAG, "sendWifiIconIntent with showWifiIcon : " + showWifiIcon);
        mIsWifiCallIntentSent = showWifiIcon;
        Intent intent = new Intent(ACTION_WIFI_CONNECTION_STATE_CHANGED);
        intent.putExtra(WIFI_STATUS, showWifiIcon);
        context.sendBroadcastAsUser(intent, UserHandle.ALL);
    }
}
