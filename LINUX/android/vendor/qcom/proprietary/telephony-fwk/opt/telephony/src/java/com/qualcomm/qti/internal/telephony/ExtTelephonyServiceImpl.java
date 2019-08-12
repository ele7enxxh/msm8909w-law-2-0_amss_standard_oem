/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.IccCard;
import org.codeaurora.internal.IDepersoResCallback;
import org.codeaurora.internal.IDsda;
import org.codeaurora.internal.IExtTelephony;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.uicc.UiccController;
import com.android.internal.telephony.uicc.UiccCard;
import com.qualcomm.qti.internal.telephony.primarycard.QtiPrimaryCardController;
/**
 * This class implements IExtTelephony aidl interface.
 */
public class ExtTelephonyServiceImpl extends IExtTelephony.Stub {
    private static final String LOG_TAG = "ExtTelephonyServiceImpl";
    private static final boolean DBG = true;

    private static final String TELEPHONY_SERVICE_NAME = "extphone";
    private static Context mContext;

    private static ExtTelephonyServiceImpl sInstance = null;

    public static ExtTelephonyServiceImpl init(Context context) {
        synchronized (ExtTelephonyServiceImpl.class) {
            mContext = context;
            if (sInstance == null) {
                sInstance = new ExtTelephonyServiceImpl();
            } else {
                Log.wtf(LOG_TAG, "init() called multiple times!  sInstance = " + sInstance);
            }
            return sInstance;
        }
    }

    public static ExtTelephonyServiceImpl getInstance() {
        if (sInstance == null) {
            Log.wtf(LOG_TAG, "getInstance null");
        }
        return sInstance;
    }

    private ExtTelephonyServiceImpl() {
        if (DBG) logd("init constructor ");

        if (ServiceManager.getService(TELEPHONY_SERVICE_NAME) == null) {
            ServiceManager.addService(TELEPHONY_SERVICE_NAME, this);
        }
    }

    @Override
    public int getCurrentUiccCardProvisioningStatus(int slotId) {
        return QtiUiccCardProvisioner.getInstance()
                .getCurrentUiccCardProvisioningStatus(slotId);
    }

    @Override
    public int getUiccCardProvisioningUserPreference(int slotId) {
        return QtiUiccCardProvisioner.getInstance()
                .getUiccCardProvisioningUserPreference(slotId);
    }

    @Override
    public int activateUiccCard(int slotId) {
        return QtiUiccCardProvisioner.getInstance().activateUiccCard(slotId);
    }

    @Override
    public int deactivateUiccCard(int slotId) {
        return QtiUiccCardProvisioner.getInstance().deactivateUiccCard(slotId);
    }

    @Override
    public boolean isSMSPromptEnabled() {
        if (QtiSubscriptionController.getInstance() == null) {
            Log.wtf(LOG_TAG, "QtiSubscriptionController getInstance is null");
        }
        return QtiSubscriptionController.getInstance().isSMSPromptEnabled();
    }

    @Override
    public void setSMSPromptEnabled(boolean enabled) {
        if (QtiSubscriptionController.getInstance() == null) {
            Log.wtf(LOG_TAG, "QtiSubscriptionController getInstance is null");
        }
        QtiSubscriptionController.getInstance().setSMSPromptEnabled(enabled);
    }

    @Override
    public int getPhoneIdForECall() {
        return QtiEmergencyCallHelper.getPhoneIdForECall();
    }

    @Override
    public int getPrimaryStackPhoneId() {
        return QtiEmergencyCallHelper.getPrimaryStackPhoneId();
    }

    private IDsda mDsda = null;
    public void setDsdaAdapter(IDsda a) {
        if (DBG) logd("setDsdaAdapter:" + a);
        mDsda = a;
    }

    public void switchToActiveSub(int sub) {
        if (DBG) logd("switchToActiveSub:" + sub + " mDsda:" + mDsda);
        try {
            mDsda.switchToActiveSub(sub);
        } catch (RemoteException ex) {
            if (DBG) logd("switchToActiveSub:" + ex);
        }
    }

    public int getActiveSubscription() {
        if (DBG) logd("getActiveSubscription mDsda:" + mDsda);
        try {
            return mDsda.getActiveSubscription();
        } catch (RemoteException ex) {
            if (DBG) logd("getActiveSubscription:" + ex);
        }
        return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
    }
    @Override
    public boolean isFdnEnabled() {
        IccCard card = PhoneFactory.getDefaultPhone().getIccCard();
        if (card != null) {
            return card.getIccFdnEnabled();
        }
        return false;
    }

    public int getUiccApplicationCount(int slotId) {
        UiccCard card = UiccController.getInstance().getUiccCard(slotId);
        int count = 0;
        if (card != null) {
            count = card.getNumApplications();
        }
        return count;
    }

    public void
    supplyIccDepersonalization(String netpin, String type, IDepersoResCallback callback,
            int phoneId) {
        logd("supplyIccDepersonalization");
        QtiDepersoSupplier.getInstance().
            supplyIccDepersonalization(netpin, type, callback, phoneId);
    }

    /**
     * Returns the application type as Integer.
     * Supported return values:
     * '0' - APPTYPE_UNKNOWN
     * '1' - APPTYPE_SIM
     * '2' - APPTYPE_USIM
     * '3' - APPTYPE_RUIM
     * '4' - APPTYPE_CSIM
     * '5' - APPTYPE_ISIM
     */
    public int getUiccApplicationType(int slotId, int appIndex) {
        UiccCard card = UiccController.getInstance().getUiccCard(slotId);
        int appType = 0;  //AppType.APPTYPE_UNKNOWN
        if (card != null) {
            appType = card.getApplicationIndex(appIndex).getType().ordinal();
        }
        return appType;
    }

    /**
     * Returns the application type as Integer.
     * Supported return values:
     * '0' - APPSTATE_UNKNOWN
     * '1' - APPSTATE_DETECTED
     * '2' - APPSTATE_PIN
     * '3  - APPSTATE_PUK
     * '4' - APPSTATE_SUBSCRIPTION_PERSO
     * '5' - APPSTATE_READY
     */
    public int getUiccApplicationState(int slotId, int appIndex) {
        UiccCard card = UiccController.getInstance().getUiccCard(slotId);
        int appState = 0;  //AppState.APPSTATE_UNKNOWN
        if (card != null) {
            appState = card.getApplicationIndex(appIndex).getState().ordinal();
        }
        return appState;
    }

    @Override
    public void setPrimaryCardOnSlot(int slotId) {
        QtiPrimaryCardController.getInstance().setPrimaryCardOnSlot(slotId);
    }

    @Override
    public boolean isEmergencyNumber(String number) {
        return QtiEmergencyCallHelper.isEmergencyNumber(number);
    }

    @Override
    public boolean isLocalEmergencyNumber(String number) {
        return QtiEmergencyCallHelper.isLocalEmergencyNumber(mContext, number);
    }

    @Override
    public boolean isPotentialEmergencyNumber(String number) {
        return QtiEmergencyCallHelper.isPotentialEmergencyNumber(number);
    }

    @Override
    public boolean isPotentialLocalEmergencyNumber(String number) {
        return QtiEmergencyCallHelper.isPotentialLocalEmergencyNumber(mContext, number);
    }

    @Override
    public boolean isDeviceInSingleStandby() {
        return QtiEmergencyCallHelper.isDeviceInSingleStandby();
    }

    public boolean setLocalCallHold(int subscriptionId, boolean enable) {
        int phoneId = SubscriptionManager.getPhoneId(subscriptionId);
        Phone phone = PhoneFactory.getPhone(phoneId);
        if (DBG) logd("setLocalCallHold:" + phoneId + " enable:" + enable);
        return ((QtiGsmCdmaPhone) phone).setLocalCallHold(enable);
    }

    public boolean isDsdaEnabled() {
        return (TelephonyManager.getDefault().getMultiSimConfiguration()
                == TelephonyManager.MultiSimVariants.DSDA);
    }

    private void logd(String string) {
        Rlog.d(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }
}
