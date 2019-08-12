/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.primarycardcontroller;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.telephony.SubscriptionManager;
import android.telephony.Rlog;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.LinearLayout.LayoutParams;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.android.internal.telephony.Phone;

public class PrimarySubSetting extends Activity implements View.OnClickListener {
    private static final String TAG = "PrimarySubSetting";
    private TextView mRecognizeText, mSelectText;
    private RadioGroup mRadioGroup;
    private Button mOKbutton;
    private CheckBox mDdsChecBox;
    private ProgressDialog mProgressDialog;
    private PrimarySubSelectionController mPrimarySubSelectionController;
    private boolean mIsPrimaryLteSubEnabled;

    private static final int SET_LTE_SUB_MSG = 1;
    private static final String CONFIG_ACTION = "codeaurora.intent.action.ACTION_LTE_CONFIGURE";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.primary_sub_select);
        mPrimarySubSelectionController = PrimarySubSelectionController.getInstance();
        mIsPrimaryLteSubEnabled = mPrimarySubSelectionController.isPrimaryCardFeatureEnabled()
                && ((mPrimarySubSelectionController.isPrimarySetable()
                && mPrimarySubSelectionController.getPrefPrimarySlot() == -1)
                || mPrimarySubSelectionController.isDetect4gCardEnabled());
        mRecognizeText = (TextView) findViewById(R.id.recognize_text);
        mSelectText = (TextView) findViewById(R.id.lte_text);

        mRadioGroup = (RadioGroup) findViewById(R.id.radiogroup);
        for (int i = 0; i < PrimarySubSelectionController.PHONE_COUNT; i++) {
            RadioButton radioButton = new RadioButton(this);
            mRadioGroup.addView(radioButton, new LayoutParams(LayoutParams.WRAP_CONTENT,
                    LayoutParams.WRAP_CONTENT));
            radioButton.setEnabled(mIsPrimaryLteSubEnabled);
            radioButton.setTag(i);
            radioButton.setText(mPrimarySubSelectionController.getSimCardInfo(i));
            radioButton.setOnClickListener(this);
        }
        mDdsChecBox = (CheckBox) findViewById(R.id.lte_checkBox);
        mDdsChecBox.setEnabled(mIsPrimaryLteSubEnabled);

        if (mPrimarySubSelectionController.isDetect4gCardEnabled()) {
            mRecognizeText.setText(R.string.lte_recognize_summary_generic);
            mSelectText.setText(R.string.lte_select_summary_generic);
            mDdsChecBox.setVisibility(View.GONE);
        }
        if (CONFIG_ACTION.equals(getIntent().getAction())) {
            setTitle(R.string.lte_select_title);
            mRecognizeText.setVisibility(View.GONE);
        } else {
            setTitle(R.string.lte_recognition_title);
            mDdsChecBox.setVisibility(View.GONE);
        }

        mOKbutton = (Button) findViewById(R.id.select_ok_btn);
        mOKbutton.setOnClickListener(this);

        mProgressDialog = new ProgressDialog(this);
        mProgressDialog.setMessage(this.getString(R.string.lte_setting));
        mProgressDialog.setCancelable(false);

        logd("action = " + getIntent().getAction() + " Can enable UI ? " + mIsPrimaryLteSubEnabled);
    }

    @Override
    protected void onResume() {
        super.onResume();
        int primarySlot = mPrimarySubSelectionController.getPrimarySlot();
        if (mIsPrimaryLteSubEnabled) {
            if (primarySlot != -1) {
                mOKbutton.setTag(primarySlot);
            } else {
                mOKbutton.setEnabled(false);
            }
        } else {
            mOKbutton.setEnabled(false);
        }
        logd(" onResume, primary slot " + primarySlot);
        updateState();
    }

    private void updateState() {
        int primarySlot = mPrimarySubSelectionController.getPrimarySlot();
        mRadioGroup.clearCheck();
        for (int i = 0; i < mRadioGroup.getChildCount(); i++) {
            RadioButton radioButton = (RadioButton) mRadioGroup.getChildAt(i);
            radioButton.setChecked(primarySlot == i);
        }
        if (!mIsPrimaryLteSubEnabled) {
            Toast.makeText(this, getString(R.string.lte_switch_unavailable),
                   Toast.LENGTH_LONG).show();
        }
        logd("is primary sub enabled " + mIsPrimaryLteSubEnabled + " primaryslot = " + primarySlot);
    }

    @Override
    public boolean onNavigateUp() {
        finish();
        return true;
    }

    public void onClick(View v) {
        if (v instanceof RadioButton) {
            int slot = (Integer) v.getTag();
            mOKbutton.setTag(slot);
            mOKbutton.setEnabled(true);
        } else if (v == mOKbutton) {
            int slotId = (Integer) mOKbutton.getTag();
            logd("setting pref N/W mode  " + slotId);
            mProgressDialog.show();
            int targetSub = (Integer) mOKbutton.getTag();
            android.util.Log.d(TAG, "Set primary card to : " + targetSub);
            mPrimarySubSelectionController.setUserPrefPrimarySubIdInDB(
                    SubscriptionManager.getSubId(targetSub)[0]);
            mPrimarySubSelectionController.setPreferredNetwork(slotId,
                    mHandler.obtainMessage(SET_LTE_SUB_MSG, slotId, -1));
        }
    }

    private Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case SET_LTE_SUB_MSG:
                    int targetSub = (Integer) mOKbutton.getTag();
                    int primarySlot = msg.arg1;
                    logd(" primary slot = " + primarySlot + " user set sub = " + targetSub);
                    if (targetSub != primarySlot) {
                        showFailedDialog(targetSub);
                        updateState();
                        if (mProgressDialog != null && mProgressDialog.isShowing())
                            mProgressDialog.dismiss();
                    } else {
                        mPrimarySubSelectionController.savePrimarySlot(primarySlot);
                        int ddsSubId = SubscriptionManager.getDefaultDataSubId();
                        if (mDdsChecBox.isChecked() ||
                                mPrimarySubSelectionController.isDetect4gCardEnabled()) {
                            // After set NW mode done, in any case set dds to
                            // primary sub,
                            // if failed, then restore dds to primary sub once
                            // icc loaded done.
                            logd(" current dds subId = " + ddsSubId + " new dds slotId = " + targetSub);
                            ddsSubId = SubscriptionManager.getSubId(targetSub)[0];
                            mPrimarySubSelectionController.setRestoreDdsToPrimarySub(true);
                        } else {
                            logd(" Set DDS back to previous sub :" + ddsSubId);
                        }

                        SubscriptionManager.from(PrimarySubSetting.this)
                                .setDefaultDataSubId(ddsSubId);
                        mPrimarySubSelectionController.setUserPrefDataSubIdInDB(ddsSubId);
                        if (!mPrimarySubSelectionController.isDetect4gCardEnabled()) {
                            Toast.makeText(PrimarySubSetting.this, getString(R.string.reg_suc),
                                    Toast.LENGTH_LONG).show();
                        }
                        updateState();
                        if (mProgressDialog != null && mProgressDialog.isShowing()) {
                            mProgressDialog.dismiss();
                        }
                        finish();
                    }
                    break;
                default:
                    break;
            }
        }
    };

    private void showFailedDialog(int sub) {
        if (CONFIG_ACTION.equals(getIntent().getAction())) {
            AlertDialog alertDialog = new AlertDialog.Builder(this)
                    .setTitle(R.string.reg_failed)
                    .setMessage(
                            getString(R.string.reg_failed_msg,
                                    mPrimarySubSelectionController.getSimName(sub)))
                    .setNeutralButton(R.string.select_ok, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    })
                    .create();
            alertDialog.show();
        } else {
            AlertDialog alertDialog = new AlertDialog.Builder(this)
                    .setTitle(R.string.reg_failed)
                    .setMessage(
                            getString(R.string.reg_failed_msg,
                                    mPrimarySubSelectionController.getSimName(sub)))
                    .setNeutralButton(R.string.lte_set, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            String action = "com.android.settings.sim.SIM_SUB_INFO_SETTINGS";
                            Intent intent = new Intent(action);
                            startActivity(intent);
                            finish();
                        }
                    })
                    .setNegativeButton(R.string.select_cancel,
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int which) {
                                    finish();
                                }
                            })
                    .create();
            alertDialog.show();
        }
    }

    private void logd(String message) {
        Rlog.d(TAG, message);
    }
}
