/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.primarycardcontroller.cttuneaway;

import android.content.Context;
import android.os.Message;
import android.telephony.RadioAccessFamily;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.RadioCapability;
import com.qti.primarycardcontroller.PrefNetworkRequest;

public class CtTuneAwayPrefRequest extends PrefNetworkRequest {
    private static final String TAG = "CtTuneAwayPrefNetworkRequest";
    private static final int sPhoneCount = TelephonyManager.getDefault().getPhoneCount();
    private int mDdsSlot = 0;

    public CtTuneAwayPrefRequest(Context context, int ddsSlot, Message callback) {
        super(context, ddsSlot, Phone.NT_MODE_GSM_ONLY, callback);
    }

    @Override
    protected void prepare(int slot, int nwMode) {
        mDdsSlot = slot;
        RadioCapability[] radioCapabilities = new RadioCapability[sPhoneCount];
        for (int i = 0; i < sPhoneCount; i++) {
            radioCapabilities[i] = mPhones[i].getRadioCapability();
        }
        if (radioCapabilities[mDdsSlot] != null) {
            String uuid = radioCapabilities[mDdsSlot].getLogicalModemUuid();
            if (Integer.parseInt(uuid) == 0) {
                Rlog.d(TAG, "ignore mDdsSlot = " + mDdsSlot);
                return;
            }
        } else {
            Rlog.d(TAG, "radioCapabilities is null");
            return;
        }
        // Initiaze secondary slot to default network mode
        // So the flex mapping can be triggered
        int nonDddsSlot = 0;
        if (mDdsSlot != 0) {
            nonDddsSlot = mDdsSlot - 1;
        } else {
            nonDddsSlot = mDdsSlot + 1;
        }
        int secondaryNwMode = getNetworkModeFromDb(nonDddsSlot);
        savePrefNetworkInDb(nonDddsSlot, Phone.NT_MODE_GSM_ONLY);

        // Set a proper network mode for DDS sub
        // So priamry stack maps to DDS sub
        int primaryNwMode = getNetworkModeFromDb(mDdsSlot);
        boolean isRestoredForSecondarySlot = false;
        Rlog.d(TAG, "mDdsSlot = " + mDdsSlot + " primaryNwMode = " + primaryNwMode +
                " secondaryNwMode = " + secondaryNwMode);
        if (isPrimaryNwModeSupportedOnAllSlot(radioCapabilities, primaryNwMode)) {
            Rlog.d(TAG, "set global for dds slot");
            mCommands.add(new PrefNetworkSetCommand(mDdsSlot, Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
            // Restore NW mode for secondary slot
            if (secondaryNwMode != Phone.NT_MODE_GSM_ONLY) {
                Rlog.d(TAG, "Restore NW mode for secondary slot nwMode = " + secondaryNwMode);
                mCommands.add(new PrefNetworkSetCommand(nonDddsSlot, secondaryNwMode));
                isRestoredForSecondarySlot = true;
            }
        }
        Rlog.d(TAG, "set nwMode for dds slot: nwMode = " + primaryNwMode);
        mCommands.add(new PrefNetworkSetCommand(mDdsSlot, primaryNwMode));
        if (!isRestoredForSecondarySlot && secondaryNwMode != Phone.NT_MODE_GSM_ONLY) {
            Rlog.d(TAG, "Restore NW mode for secondary slot nwMode = " + secondaryNwMode);
            mCommands.add(new PrefNetworkSetCommand(nonDddsSlot, secondaryNwMode));
        }
    }

    private boolean isPrimaryNwModeSupportedOnAllSlot(RadioCapability[] radioCapabilities,
            int nwMode) {
        boolean supported = true;
        int[] numRafSupported = new int[sPhoneCount];
        int maxNumRafSupported = 0;

        //Get num of RAFs supported for this NwMode on all Stacks
        for (int i = 0; i < sPhoneCount; i++) {
            numRafSupported[i] = getNumOfRafSupportedForNwMode(nwMode,
                    radioCapabilities[i].getRadioAccessFamily());
            if (maxNumRafSupported < numRafSupported[i]) maxNumRafSupported = numRafSupported[i];
        }

        for (int i = 0; i < sPhoneCount; i++) {
            if (numRafSupported[i] != maxNumRafSupported) supported = false;
        }

        return supported;
    }

    private int getNumOfRafSupportedForNwMode(int nwMode, int radioAccessFamily) {
        int supportedRafMaskForNwMode = 0;
        if (radioAccessFamily == RadioAccessFamily.RAF_UNKNOWN) {
            Rlog.d(TAG, " Modem Capabilites are null, nwMode = " + nwMode);
            return supportedRafMaskForNwMode;
        }

        int nwModeRaf = RadioAccessFamily.getRafFromNetworkType(nwMode);

        supportedRafMaskForNwMode = radioAccessFamily & nwModeRaf;

        Rlog.d(TAG, "getNumOfRATsSupportedForNwMode: nwMode[" + nwMode + " nwModeRaf = " +
                nwModeRaf + "] raf = " + radioAccessFamily +
                " supportedRafMaskForNwMode:" + supportedRafMaskForNwMode);
        return Integer.bitCount(supportedRafMaskForNwMode);
    }

}
