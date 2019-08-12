/* Copyright (c) 2016 Qualcomm Technologies, Inc.
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
import android.os.RemoteException;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

public class ValidateSIMPUK extends Activity{

    private static final String TAG = "Watch_SIM_PUK";

    private EditText mSimPinEntry = null;
    private Button mSimPinOKButton = null;
    private TextView mSimPinMessage = null;
    private CheckSimPuk mCheckSimPukThread = null;
    private Handler mHandler = null;
    private int mSubId = 0;//support only SIM 1
    private String mPukText = null;
    private String mPinText = null;
    private StateMachine mStateMachine;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        mHandler = new Handler();
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_validate_sim_pin);

        mSimPinEntry = (EditText)findViewById(R.id.sim_pin_edit_text);
        mSimPinOKButton = (Button)findViewById(R.id.sim_pin_ok);
        mSimPinMessage = (TextView)findViewById(R.id.sim_pin_message);
        mSimPinMessage.setSelected(true);

        mStateMachine = new StateMachine();
        mStateMachine.reset();

        mSimPinEntry.requestFocus();
        InputMethodManager imm = (InputMethodManager) getSystemService(getApplicationContext().INPUT_METHOD_SERVICE);

        mSimPinEntry.setOnEditorActionListener(new OnEditorActionListener() {

        @Override
        public boolean onEditorAction(TextView view, int actionId, KeyEvent keyEvent) {
            Log.d(TAG, "on Enter Pressed" + view.getId() +" == " + R.id.sim_pin_edit_text);
            if (view.getId() == R.id.sim_pin_edit_text) {
                Log.d(TAG, "on Enter Pressed");
                InputMethodManager imm = (InputMethodManager) getSystemService(getApplicationContext().INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(mSimPinEntry.getWindowToken(), 0);
                verifyPasswordAndUnlock();
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

    public void onSIMPINOKClicked(View view) {
        verifyPasswordAndUnlock();
    }

    private void verifyPasswordAndUnlock() {
        mStateMachine.next();
    }

    /**
     * Since the IPC can block, we want to run the request in a separate thread
     * with a callback.
     */
    private abstract class CheckSimPuk extends Thread {

        private final String mPin, mPuk;
        private int mSubId = 0;
        private Handler mHandler = null;

        protected CheckSimPuk(String puk, String pin, int subId) {
            mPuk = puk;
            mPin = pin;
            mSubId = subId;
            mHandler = new Handler();
        }

        abstract void onSimLockChangedResponse(final int result, final int attemptsRemaining);

        @Override
        public void run() {
            try {
                Log.v(TAG, "call supplyPukReportResult()");
                final int[] result = ITelephony.Stub.asInterface(ServiceManager
                    .checkService("phone")).supplyPukReportResultForSubscriber(mSubId, mPuk, mPin);
                Log.v(TAG, "supplyPukReportResult returned: " + result[0] + " " + result[1]);

                mHandler.post(new Runnable() {
                    public void run() {
                        onSimLockChangedResponse(result[0], result[1]);
                    }
                });
            } catch (RemoteException e) {
                Log.e(TAG, "RemoteException for supplyPukReportResult:", e);
                mHandler.post(new Runnable() {
                    public void run() {
                        onSimLockChangedResponse(PhoneConstants.PIN_GENERAL_FAILURE, -1);
                    }
                });
            }
        }
    }//End of CheckSimPuk class

    private boolean checkPuk() {
        // make sure the puk is at least 8 digits long.
        if (mSimPinEntry.getText().length() == 8) {
            mPukText = mSimPinEntry.getText().toString();
            return true;
        }
        return false;
    }

    private boolean checkPin() {
        // make sure the PIN is between 4 and 8 digits
        int length = mSimPinEntry.getText().length();
        if (length >= 4 && length <= 8) {
            mPinText = mSimPinEntry.getText().toString();
            return true;
        }
        return false;
    }

    public boolean confirmPin() {
        return mPinText.equals(mSimPinEntry.getText().toString());
    }

    private void resetPINEntryText() {
        mSimPinEntry.setText("");
    }

    private String getPukPasswordErrorMessage(int attemptsRemaining) {
        String displayMessage;
        Context context = getApplicationContext();

        if (attemptsRemaining == 0) {
            displayMessage = context.getString(R.string.kg_password_wrong_puk_code_dead);
        } else if (attemptsRemaining > 0) {
            displayMessage = context.getResources()
                    .getQuantityString(R.plurals.kg_password_wrong_puk_code, attemptsRemaining,
                            attemptsRemaining);
        } else {
            displayMessage = context.getString(R.string.kg_password_puk_failed);
        }
        Log.d(TAG, "getPukPasswordErrorMessage:"
                + " attemptsRemaining=" + attemptsRemaining + " displayMessage=" + displayMessage);
        return displayMessage;
    }

    private void updateSim() {
        //TODO:
        //getSimUnlockProgressDialog().show();

        if (mCheckSimPukThread == null) {
            mCheckSimPukThread = new CheckSimPuk(mPukText, mPinText, mSubId) {
                void onSimLockChangedResponse(final int result, final int attemptsRemaining) {
                    mHandler.post(new Runnable() {
                        public void run() {
                            Context context = getApplicationContext();
                            //if (mSimUnlockProgressDialog != null) {
                            //    mSimUnlockProgressDialog.hide();
                            //}
                            resetPINEntryText();
                            if (result == PhoneConstants.PIN_RESULT_SUCCESS) {
                                setResult(Activity.RESULT_OK, getIntent());
                                finish();
                            } else {
                                if (result == PhoneConstants.PIN_PASSWORD_INCORRECT) {
                                    if (attemptsRemaining <= 2) {
                                        // this is getting critical - show dialog
                                        showMessageDialog(context.getString(R.string.sim_pin_dialog_title),
                                                getPukPasswordErrorMessage(attemptsRemaining));
                                    } else {
                                        // show message
                                        mSimPinMessage.setText(
                                                getPukPasswordErrorMessage(attemptsRemaining));
                                    }
                                } else {
                                    mSimPinMessage.setText(context.getString(
                                            R.string.kg_password_puk_failed));
                                }
                                Log.d(TAG, "verifyPasswordAndUnlock "
                                        + " UpdateSim.onSimCheckResponse: "
                                        + " attemptsRemaining=" + attemptsRemaining);
                                mStateMachine.reset();
                            }
                            mCheckSimPukThread = null;
                        }
                    });
                }
            };
            mCheckSimPukThread.start();
        }
    }

    //State Machine
    private class StateMachine {
        final int ENTER_PUK = 0;
        final int ENTER_PIN = 1;
        final int CONFIRM_PIN = 2;
        final int DONE = 3;
        private int state = ENTER_PUK;

        public void next() {
            int msg = 0;
            if (state == ENTER_PUK) {
                if (checkPuk()) {
                    state = ENTER_PIN;
                    msg = R.string.kg_puk_enter_pin_hint;
                } else {
                    msg = R.string.kg_invalid_sim_puk_hint;
                }
            } else if (state == ENTER_PIN) {
                if (checkPin()) {
                    state = CONFIRM_PIN;
                    msg = R.string.kg_enter_confirm_pin_hint;
                } else {
                    msg = R.string.kg_invalid_sim_pin_hint;
                }
            } else if (state == CONFIRM_PIN) {
                if (confirmPin()) {
                    state = DONE;
                    msg = R.string.keyguard_sim_unlock_progress_dialog_message;
                    updateSim();
                } else {
                    state = ENTER_PIN; // try again?
                    msg = R.string.kg_invalid_confirm_pin_hint;
                }
            }
            resetPINEntryText();
            if (msg != 0) {
                mSimPinMessage.setText(msg);
            }
        }

        void reset() {
            mPinText="";
            mPukText="";
            state = ENTER_PUK;
            mSubId = 0;
            mSimPinMessage.setText(getApplicationContext().getString(R.string.kg_puk_enter_puk_hint));
            mSimPinEntry.requestFocus();
        }
    }//End State Machine
}
