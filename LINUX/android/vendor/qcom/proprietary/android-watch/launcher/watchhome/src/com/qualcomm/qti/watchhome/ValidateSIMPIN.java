/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.qualcomm.qti.watchhome;

import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.PhoneConstants;
import android.os.ServiceManager;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.telephony.TelephonyManager;
import android.text.Editable;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.view.textservice.TextInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.os.RemoteException;

public class ValidateSIMPIN extends Activity{

    private static final String TAG = "Watch_SIM_PIN";

    private EditText mSimPinEntry = null;
    private Button mSimPinOKButton = null;
    private TextView mSimPinMessage = null;
    private CheckSimPin mCheckSimPinThread = null;
    private Handler mHandler = null;
    private int mSubId = 0;//support only SIM 1
    private int mRemainingAttempts =-1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        mHandler = new Handler();
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_validate_sim_pin);

        mSimPinEntry = (EditText)findViewById(R.id.sim_pin_edit_text);
        mSimPinOKButton = (Button)findViewById(R.id.sim_pin_ok);
        mSimPinMessage = (TextView)findViewById(R.id.sim_pin_message);
        mSimPinMessage.setSelected(true);

        mSimPinMessage.setText(R.string.kg_sim_pin_instructions);
        mSimPinEntry.requestFocus();
        InputMethodManager imm = (InputMethodManager) getSystemService(getApplicationContext().INPUT_METHOD_SERVICE);

        mSimPinEntry.setOnEditorActionListener(new OnEditorActionListener() {

            @Override
            public boolean onEditorAction(TextView view, int actionId, KeyEvent keyEvent) {
                if (view.getId() == R.id.sim_pin_edit_text) {
                    InputMethodManager imm = (InputMethodManager) getSystemService(getApplicationContext().INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(mSimPinEntry.getWindowToken(), 0);
                    verifyPasswordAndUnlock(mSimPinEntry.getText().toString());
                    return true;
                }
                return false;
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        InputMethodManager imm = (InputMethodManager) getSystemService(getApplicationContext().INPUT_METHOD_SERVICE);
        imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY);
    }

    @Override
    protected void onPause() {
        super.onPause();
        InputMethodManager imm = (InputMethodManager) getSystemService(getApplicationContext().INPUT_METHOD_SERVICE);
        imm.hideSoftInputFromWindow(mSimPinEntry.getWindowToken(), 0);
    }

    public void onSIMPINOKClicked(View view) {
        verifyPasswordAndUnlock(mSimPinEntry.getText().toString());
    }

    private void resetPINEntryText() {
        mSimPinEntry.setText("");
    }

    private String getPinPasswordErrorMessage(int attemptsRemaining, boolean isDefault) {
        Context context = getApplicationContext();
        String displayMessage;
        int msgId;
        if (attemptsRemaining == 0) {
            displayMessage = context.getString(R.string.kg_password_wrong_pin_code_pukked);
        } else if (attemptsRemaining > 0) {
            msgId = isDefault ? R.plurals.kg_password_default_pin_message :
                     R.plurals.kg_password_wrong_pin_code;
            displayMessage = context.getResources()
                    .getQuantityString(msgId, attemptsRemaining, attemptsRemaining);
        } else {
            msgId = isDefault ? R.string.kg_sim_pin_instructions : R.string.kg_password_pin_failed;
            displayMessage = context.getString(msgId);
        }
        Log.d(TAG, "getPinPasswordErrorMessage:"
                + " attemptsRemaining=" + attemptsRemaining + " displayMessage=" + displayMessage);
        return displayMessage;
    }

    public void verifyPasswordAndUnlock(String PIN) {
        Log.d(TAG, "PIN " + PIN);
        if (PIN.length() < 4) {
            Log.d(TAG, "PIN len < 4");
            // otherwise, display a message to the user, and don't submit.
            mSimPinMessage.setText(R.string.kg_invalid_sim_pin_hint);
            resetPINEntryText();
            return;
        }

        //TODO:
        //getSimUnlockProgressDialog().show();

        if (mCheckSimPinThread == null) {
            mCheckSimPinThread = new CheckSimPin(PIN, mSubId) {
                void onSimCheckResponse(final int result, final int attemptsRemaining) {
                    mHandler.post(new Runnable() {
                        public void run() {
                            Context context = getApplicationContext();
                            mRemainingAttempts = attemptsRemaining;
                            //if (mSimUnlockProgressDialog != null) {
                            //    mSimUnlockProgressDialog.hide();
                            //}
                            resetPINEntryText();
                            if (result == PhoneConstants.PIN_RESULT_SUCCESS) {
                                Log.d(TAG, "PIN_RESULT_SUCCESS");
                                setResult(Activity.RESULT_OK, getIntent());
                                finish();
                                //KeyguardUpdateMonitor.getInstance(getContext())
                                //        .reportSimUnlocked(mSubId);
                                mRemainingAttempts = -1;
                            } else {
                                //mShowDefaultMessage = false;
                                if (result == PhoneConstants.PIN_PASSWORD_INCORRECT) {
                                    Log.d(TAG, "PIN_PASSWORD_INCORRECT");
                                    if (attemptsRemaining <= 2) {
                                        // this is getting critical - show dialog
                                        showMessageDialog(context.getString(R.string.sim_pin_dialog_title), getPinPasswordErrorMessage(attemptsRemaining, false));
                                   } else {
                                        // show message
                                        mSimPinMessage.setText(
                                                getPinPasswordErrorMessage(
                                                attemptsRemaining, false));
                                   }
                                } else {
                                    // "PIN operation failed!" - no idea what this was and no way to
                                    // find out. :/
                                    mSimPinMessage.setText(context.getString(
                                            R.string.kg_password_pin_failed));
                                    Log.d(TAG, "PIN failed");
                                }
                                Log.d(TAG, "verifyPasswordAndUnlock "
                                        + " CheckSimPin.onSimCheckResponse: " + result
                                        + " attemptsRemaining=" + attemptsRemaining);
                            }
                            mCheckSimPinThread = null;
                        }
                    });
                }
            };
            mCheckSimPinThread.start();
        }
    }

    private void showMessageDialog(String title, String message) {
        AlertDialog.Builder mAlertDialogBuilder =
                new AlertDialog.Builder(getApplicationContext());

        mAlertDialogBuilder.setTitle(title);

        mAlertDialogBuilder.setMessage(message);
        mAlertDialogBuilder.setNeutralButton(R.string.sim_pin_ok, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int arg1) {
                dialog.dismiss();
            }

        });
        mAlertDialogBuilder.create().show();

    }

    /**
     * Since the IPC can block, we want to run the request in a separate thread
     * with a callback.
     */
    private abstract class CheckSimPin extends Thread {
        private static final String TAG = "CheckSimPin: ";
        private final String mPin;
        private int mSubId;
        private Handler mHandler = null;

        protected CheckSimPin(String pin, int subId) {
            mPin = pin;
            mSubId = subId;
            mHandler = new Handler();
        }

        abstract void onSimCheckResponse(final int result, final int attemptsRemaining);

        @Override
        public void run() {
            try {
                    Log.v(TAG, "call supplyPinReportResultForSubscriber(subid=" + mSubId + ")");
                    Log.d(TAG, "PIN " + mPin);
                    final int[] result = ITelephony.Stub.asInterface(ServiceManager
                        .checkService("phone")).supplyPinReportResultForSubscriber(mSubId, mPin);

                    Log.v(TAG, "supplyPinReportResult returned: " + result[0] + " " + result[1]);

                mHandler.post(new Runnable() {
                    public void run() {
                        onSimCheckResponse(result[0], result[1]);
                    }
                });
            } catch (RemoteException e) {
                Log.e(TAG, "RemoteException for supplyPinReportResult:", e);
                mHandler.post(new Runnable() {
                    public void run() {
                        onSimCheckResponse(PhoneConstants.PIN_GENERAL_FAILURE, -1);
                    }
                });
            }
        }
    };
}
