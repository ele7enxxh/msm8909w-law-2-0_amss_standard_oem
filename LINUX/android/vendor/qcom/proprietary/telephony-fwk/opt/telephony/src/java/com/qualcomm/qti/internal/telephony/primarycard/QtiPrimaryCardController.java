/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.primarycard;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.UserHandle;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.RILConstants;
import com.android.internal.telephony.SubscriptionController;
import com.android.internal.telephony.TelephonyIntents;

import com.qualcomm.qti.internal.telephony.QtiUiccCardProvisioner;
import com.qualcomm.qti.internal.telephony.QtiSubscriptionController;

/**
 * This class has logic to set primary card on highest priority card.
 */
public class QtiPrimaryCardController extends Handler{
    private static final String LOG_TAG = "QtiPcController";
    private static final boolean VDBG = false;
    private static final boolean DBG = true;


    //*********Events*********
    private static final int EVENT_ALL_CARDS_INFO_AVAILABLE = 1;
    private static final int EVENT_SET_RADIO_CAPABILITY_DONE = 2;
    private static final int EVENT_SET_NWMODE_DONE = 3;
    private static final int EVENT_GET_NWMODE_DONE = 4;
    private static final int EVENT_PRIMARY_CARD_SET_DONE = 5;

    //*********Class Variables*********
    private static QtiPrimaryCardController sInstance;
    private CommandsInterface[] mCi;
    private Phone[] mPhone;
    QtiCardInfoManager mCardInfoMgr;
    QtiPrimaryCardPriorityHandler mPriorityHandler;
    QtiPrimaryCardUtils mPcUtils;
    private final Context mContext;
    private int mPrefPrimarySlot = QtiPrimaryCardUtils.SLOT_INVALID;
    private int[] mPrefNwModes;
    private Message mCmdMessage;
    private boolean mCardChanged = false;
    private boolean mPriorityMatch = false;
    private boolean mPcTriggeredFlexMapDone = false;
    private String[] mCurrentIccIds;

    private static final String ACTION_SET_PRIMARY_CARD_DONE =
            "org.codeaurora.intent.action.ACTION_SET_PRIMARY_CARD_DONE";
    private static final String PRIMARY_CARD_RESULT = "result";
    private static final String PRIMARYCARD_SUBSCRIPTION_KEY = "primarycard_sub";
    private static final int[] sCtIins = {898603, 898611};
    private static final int[] sCmccIins = {898600, 898602, 898607};

    public enum PrimaryCardState{
        IDLE,
        IN_PROGRESS,
        PENDING_DUE_TO_PC_IN_PROGRESS,
        PENDING_DUE_TO_FLEXMAP_IN_PROGRESS
    }

    private PrimaryCardState mPrimaryCardState = PrimaryCardState.IDLE;

    public static void init(Context context, Phone[] phones,
            CommandsInterface[] ci) {
        synchronized (QtiPrimaryCardController.class) {
            if (sInstance == null && QtiPrimaryCardUtils.isPrimaryCardFeatureEnabled()) {
                sInstance = new QtiPrimaryCardController(context, phones, ci);
            }
        }
    }

    public static QtiPrimaryCardController getInstance() {
        synchronized (QtiPrimaryCardController.class) {
            if (sInstance == null) {
                throw new RuntimeException("QtiPrimaryCardController was not initialized!");
            }
            return sInstance;
        }
    }

    //******Constructor********
    private QtiPrimaryCardController(Context context, Phone[] phones,
            CommandsInterface[] ci) {
        mContext = context;
        mPhone = phones;
        mCi = ci;
        mPcUtils = QtiPrimaryCardUtils.init(mContext);
        mPriorityHandler = new QtiPrimaryCardPriorityHandler(mContext);
        mCardInfoMgr =  QtiCardInfoManager.init(mContext, ci);

        mCardInfoMgr.registerAllCardsInfoAvailable(this, EVENT_ALL_CARDS_INFO_AVAILABLE, null);
        mPrefNwModes = new int[QtiPrimaryCardUtils.PHONE_COUNT];
        mCurrentIccIds = new String[QtiPrimaryCardUtils.PHONE_COUNT];

        resetPrimaryCardParams();

        IntentFilter intentFilter = new IntentFilter(
                TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_DONE);
        mContext.registerReceiver(mReceiver, intentFilter);
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            logd("Recieved intent " + action);
            if (TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_DONE.equals(action)) {
                sendMessage(obtainMessage(EVENT_SET_RADIO_CAPABILITY_DONE));
            }
        }
    };

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case EVENT_ALL_CARDS_INFO_AVAILABLE:
                logd("on EVENT_ALL_CARDS_INFO_AVAILABLE");
                setPrimaryCardIfRequired();
                break;
            case EVENT_SET_RADIO_CAPABILITY_DONE:
                logd("on EVENT_SET_RADIO_CAPABILITY_DONE");
                handleSetRadioCapsDone();
                break;
            case EVENT_SET_NWMODE_DONE:
                logd("on EVENT_SET_NWMODE_DONE");
                handleSetNwModeDone(msg);
                break;
            case EVENT_GET_NWMODE_DONE:
                logd("on EVENT_GET_NWMODE_DONE");
                handleGetNwModeDone(msg);
                break;
            case EVENT_PRIMARY_CARD_SET_DONE:
                logd("on EVENT_PRIMARY_CARD_SET_DONE");
                handleOnSetPrimaryCardDone(msg);
                break;
        }
    }

    private void handleSetRadioCapsDone() {
        if (mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_FLEXMAP_IN_PROGRESS) {
            //If primary card is setting is pending due to explicit flex map in progress,
            //try again now.
            mPrimaryCardState = PrimaryCardState.IDLE;
            logd("Flex mapping completed, try setting primary card now");
            setPrimaryCardIfRequired();
        } else if (mPrimaryCardState == PrimaryCardState.IN_PROGRESS
                || mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_PC_IN_PROGRESS) {
            //Implicit flex mapping trigerred due to primary card setting is completed.
            logd("Primary card trigerred Flex Mapping completed.");
            mPcTriggeredFlexMapDone = true;
        }
    }

    private void handleSetNwModeDone(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;
        int index = (Integer) ar.userObj;
        logd("set " + mPrefNwModes[index] + " for slot " + index + " done, " + ar.exception);
        if (ar.exception != null) {
            for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                mPhone[i].getPreferredNetworkType(
                        obtainMessage(EVENT_GET_NWMODE_DONE, i));
            }
            sendSetPrimaryCardResult(RILConstants.GENERIC_FAILURE);
            return;
        }

        if (mPcTriggeredFlexMapDone || (index == mPrefPrimarySlot)) {
            //Primary Card Setting is completed, set DDS and send SUCCESS to target.
            setDdsOnPrimaryCardIfRequired();
            sendSetPrimaryCardResult(RILConstants.SUCCESS);
        } else {
            logd("set NwMode[" + mPrefNwModes[mPrefPrimarySlot] +
                    "] on Primarycard:" + mPrefPrimarySlot);
            mPhone[mPrefPrimarySlot].setPreferredNetworkType(mPrefNwModes[mPrefPrimarySlot],
                    obtainMessage(EVENT_SET_NWMODE_DONE, mPrefPrimarySlot));
        }
    }

    private void handleGetNwModeDone(Message msg) {
        int modemNwMode = -1;
        AsyncResult ar = (AsyncResult) msg.obj;
        int index = (Integer) ar.userObj;
        if (ar.exception == null) {
            modemNwMode = ((int[]) ar.result)[0];
            saveNwModesToDB(modemNwMode, index);
        }
        logd("got nwMode:" + modemNwMode + " on slot" + index + ", saved to DB, " + ar.exception);
    }

    private void setDdsOnPrimaryCardIfRequired() {
        SubscriptionController subCtrlr = SubscriptionController.getInstance();

        subCtrlr.setDefaultDataSubId(subCtrlr.getSubIdUsingSlotId(mPrefPrimarySlot)[0]);
    }

    private void sendSetPrimaryCardResult(int responseCode) {
        if (mCmdMessage != null) {
            Exception e = CommandException.fromRilErrno(responseCode);
            AsyncResult.forMessage(mCmdMessage, null, e);
            mCmdMessage.sendToTarget();
            mCmdMessage = null;
        }

        if (responseCode == RILConstants.SUCCESS) {
            saveCardIccIdInfoInSp();
            notifySetPrimaryCardDone(true);
        } else {
            notifySetPrimaryCardDone(false);
        }

        if (mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_PC_IN_PROGRESS) {
            mPrimaryCardState = PrimaryCardState.IDLE;
            logi("Primary Card request completed, check for pending reqeusts");
            setPrimaryCardIfRequired();
        } else if (mPrimaryCardState == PrimaryCardState.IN_PROGRESS) {
            mPrimaryCardState = PrimaryCardState.IDLE;
        }
    }

    private void notifySetPrimaryCardDone(boolean isPass) {
        logd("notifySetPrimaryCardDone: Set Primary Card SUCCESS: " + isPass);
        Intent intent = new Intent(ACTION_SET_PRIMARY_CARD_DONE);
        intent.putExtra(PRIMARY_CARD_RESULT, isPass ? 1 : 0);
        intent.putExtra(PhoneConstants.PHONE_KEY, mPrefPrimarySlot);
        mContext.sendBroadcast(intent);
    }

    private void handleOnSetPrimaryCardDone(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;
        int index = (Integer) ar.userObj;
        if (ar.exception == null) {
            //Set Primary card is completed Successfully, save the value to DB.
            QtiPrimaryCardUtils.savePrimarySlotToDB(index);

            int userSelectionMode = QtiPrimaryCardUtils.getUserSelectionMode();
            boolean enableUserSelection = false;

            //enable user selection if
            // 1.show user selection on every change is true or
            // 2.show user selection on priority match is true and currently there is priority
            //   match.
            if (userSelectionMode == QtiPrimaryCardUtils.SHOW_USER_SELECTION_FOR_EVERY_CHANGE ||
                    (userSelectionMode == QtiPrimaryCardUtils.SHOW_USER_SELECTION_ON_PRIORITY_MATCH
                    && mPriorityMatch)) {
                enableUserSelection = true;
            }
            logd("handleOnSetPrimaryCardDone: enableUserSelection =" + enableUserSelection +
                    ", mCardChanged:" + mCardChanged + ", numCardsValid:" + numCardsValid());

            QtiPrimaryCardUtils.saveEnableUserSelectioninDB(enableUserSelection);

            if (enableUserSelection && mCardChanged && numCardsValid() > 1) {
                Intent intent = new Intent("codeaurora.intent.action.ACTION_LTE_CONFIGURE");
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
                        | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
                mContext.startActivity(intent);
            }
        }
    }

    private int numCardsValid() {
        int numCount = 0;
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            if (mCardInfoMgr.getCardInfo(i).getIccId() != null) numCount++;
        }
        return numCount;
    }

    private void updateDdsPreferenceInDb() {
        boolean disableDds = false;
        if (QtiPrimaryCardUtils.disableDds()) {
            int numCtCards = 0;
            int numCmccCards = 0;
            for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                if (isCardMatchesIins(mCardInfoMgr.getCardInfo(i).getIccId(), sCtIins)) {
                    numCtCards++;
                } else if (isCardMatchesIins(mCardInfoMgr.getCardInfo(i).getIccId(), sCmccIins)) {
                    numCmccCards++;
                }
            }

            //Disable DDS in below conditions
            // 1. type 2 devices with one cmcc card and one ct card
            // 2. not a type2 device with one ct card.
            if ((!QtiPrimaryCardUtils.isPrimaryCardType2Enabled() && numCtCards == 1) ||
                    (QtiPrimaryCardUtils.isPrimaryCardType2Enabled() && numCtCards ==1 &&
                    numCmccCards == 1)) {
                logi("updateDdsPreferenceInDb: Disable DDS in UI.");
                disableDds = true;
            }
        }
        QtiPrimaryCardUtils.saveDisableDdsPreferenceInDB(disableDds);
    }

    private boolean isCardMatchesIins(String iccId, int[] iins) {
        // iin length is 6
        if (iccId == null || iccId.length() < 6) {
            return false;
        }
        int cardIin = Integer.parseInt(iccId.substring(0, 6));
        for (int iin : iins) {
            if (iin == cardIin) return true;
        }
        return false;
    }

    private void resetPrimaryCardParams() {
        mPriorityMatch = false;
        mCmdMessage = null;
        mPcTriggeredFlexMapDone = false;
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            mPrefNwModes[i] = QtiPrimaryCardUtils.getDefaultNwMode();
        }
    }

    /*
    * setPrimaryCardIfRequired: Method used to set primary card if required.
    */
    private void setPrimaryCardIfRequired() {
        int slotId = QtiPrimaryCardUtils.SLOT_INVALID;

        if (mPrimaryCardState == PrimaryCardState.IN_PROGRESS
                || mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_PC_IN_PROGRESS) {
            //If Primary card is in progrees or pending, set state as pending. So that once
            // primary card setting is completed it will check for pending state and initiate the
            // primary card sequence again.

            mPrimaryCardState = PrimaryCardState.PENDING_DUE_TO_PC_IN_PROGRESS;
            logi("Primary Card setting in progress. WAIT!");
            return;
        } else if (QtiUiccCardProvisioner.getInstance().isFlexMapInProgress()
                || mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_FLEXMAP_IN_PROGRESS) {
            //If we are here that means flex mapping is in progress and primary card is not in
            // progress. So set state to flex map pending.
            mPrimaryCardState = PrimaryCardState.PENDING_DUE_TO_FLEXMAP_IN_PROGRESS;
            logi("Flex Map in progress. WAIT!");
            return;
        } else if (QtiUiccCardProvisioner.getInstance().isAnyProvisionRequestInProgress()) {
            //Manual provsioning is in Progress, just ignore the event. We will anyways get the
            // indication again from cardInfoMgr once provisioning is completed.
            logi("Manual provisioning in progress. EXIT!");
            return;
        }

        boolean isCardChanged = isCardsInfoChanged();

        mPriorityHandler.loadCurrentPriorityConfigs(isCardChanged);

        if(!isCardChanged) {
            logd("primary card " + QtiPrimaryCardUtils.getCurrentPrimarySlotFromDB() +
                    " ,Cards not changed, IGNORE!!");
            return;
        }
        mCardChanged = isCardChanged;

        //Get Pref Primary slot from Priority config.
        mPrefPrimarySlot = mPriorityHandler.getPrefPrimarySlot();

        resetPrimaryCardParams();

        //Save Settable value in the DB.
        QtiPrimaryCardUtils.savePrimarySetable(
                mPrefPrimarySlot != QtiPrimaryCardUtils.SLOT_INVALID);

        //Save disableDDS option in DB.
        updateDdsPreferenceInDb();

        if (mPrefPrimarySlot == QtiPrimaryCardUtils.SLOT_PRIORITY_MATCH) {
            mPrefPrimarySlot = QtiPrimaryCardUtils.getDefaultPrimarySlot();
            mPriorityMatch = true;
        } else if (mPrefPrimarySlot < 0) {
            logi("Both slots do not have cards with priority config defined. EXIT!");
            return;
        }

        setPrimaryCardOnSlot(mPrefPrimarySlot,
                obtainMessage(EVENT_PRIMARY_CARD_SET_DONE, mPrefPrimarySlot));

    }

    //This method will be invoked from AIDL call.
    public void setPrimaryCardOnSlot(int slotId) {
        if (!QtiSubscriptionController.getInstance().isRadioInValidState()) {
            loge("setPrimaryCardOnSlot[" + slotId + "]: Radio is in Invalid State, EXIT!!!");
            sendSetPrimaryCardResult(RILConstants.GENERIC_FAILURE);
            return;
        }

        setPrimaryCardOnSlot(slotId, null);
    }

    synchronized private void setPrimaryCardOnSlot(int primarySlotId, Message msg) {
        SubscriptionController subCtrlr = SubscriptionController.getInstance();
        int subId = subCtrlr.getSubIdUsingPhoneId(primarySlotId);

        logd("setPrimaryCardOnSlot: for slotId:" + primarySlotId + ", Start.");

        if (mPrimaryCardState != PrimaryCardState.IDLE || !mPriorityHandler.isConfigLoadDone()
            || !SubscriptionManager.isValidSlotId(primarySlotId) || !subCtrlr.isActiveSubId(subId)) {
            loge("Primary Card State is not IDLE, mPrimaryCardState:" + mPrimaryCardState +
                    " subId: " + subId + ", or configs not yet loaded EXIT!");
            sendSetPrimaryCardResult(RILConstants.GENERIC_FAILURE);
            return;
        }

        // If request received from UI save IccIds in mCurrentIccIds cache, this would
        // be used in saveCardIccIdInfoInSp() to update in Shared pref.
        if (msg == null) {
            for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                mCurrentIccIds[i] = mCardInfoMgr.getCardInfo(i).getIccId();
            }
        }

        mPrimaryCardState = PrimaryCardState.IN_PROGRESS;

        //get pref nwModes on both slots as per config
        mPrefNwModes = mPriorityHandler.getNwModesFromConfig(primarySlotId);
        mPrefPrimarySlot = primarySlotId;
        mCmdMessage = msg;
        mPcTriggeredFlexMapDone = false;

        //Save PrefNwMode of both slots to DB before sending setPref request to phone.
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            saveNwModesToDB(mPrefNwModes[i], i);
        }

        //First set nwMode on secondary card.Once secondary card nwMode is done set nwMode on
        //Primary card and DDS
        for (int index = 0; index < QtiPrimaryCardUtils.PHONE_COUNT; index++) {
            if (index != primarySlotId) {
                logd("set NwMode[" + mPrefNwModes[index] + "]  on Secondary card:" + index);
                mPhone[index].setPreferredNetworkType(mPrefNwModes[index],
                        obtainMessage(EVENT_SET_NWMODE_DONE, index));
            }
        }
    }

    private void saveNwModesToDB(int nwMode, int slotId) {
        int[] subId = QtiSubscriptionController.getInstance().getSubId(slotId);
        if (subId != null) {
            logi("saveNwModesToDB: subId[" + slotId + "] = " + subId[0]
                    + ", new Nw mode = " + nwMode);
            if (QtiSubscriptionController.getInstance().isActiveSubId(subId[0])) {
                android.provider.Settings.Global.putInt(mContext.getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE + subId[0],
                        nwMode);
            }
        } else {
            loge("saveNwModesToDB: subId is null, do not save nwMode in subId based DB");
        }
        TelephonyManager.putIntAtIndex(mContext.getContentResolver(),
                android.provider.Settings.Global.PREFERRED_NETWORK_MODE, slotId,
                nwMode);
    }

    private boolean isCardsInfoChanged() {
        boolean cardChanged = false;
        for (int index = 0; index < QtiPrimaryCardUtils.PHONE_COUNT; index++) {
            if (isCardsInfoChanged(index)) {
                cardChanged = true;
            }
        }
        return cardChanged;
    }

    protected boolean isCardsInfoChanged(int phoneId) {
        String iccId = mCurrentIccIds[phoneId] = mCardInfoMgr.getCardInfo(phoneId).getIccId();
        String iccIdInSP = PreferenceManager.getDefaultSharedPreferences(mContext).getString(
                PRIMARYCARD_SUBSCRIPTION_KEY + phoneId, null);
        logd(" phoneId " + phoneId + " icc id = " + iccId + ", icc id in sp=" + iccIdInSP);
        return !TextUtils.equals(iccId, iccIdInSP);
    }

    private void saveCardIccIdInfoInSp() {
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            String iccId = mCurrentIccIds[i];

            logd("save IccId: " + iccId + ", on slotId:" + i + ", in SP.");
            PreferenceManager.getDefaultSharedPreferences(mContext).edit()
                    .putString(PRIMARYCARD_SUBSCRIPTION_KEY + i, iccId).commit();
        }
    }

    private void logd(String string) {
        if (DBG) Rlog.d(LOG_TAG, string);
    }

    private void logi(String string) {
        Rlog.i(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }
}

