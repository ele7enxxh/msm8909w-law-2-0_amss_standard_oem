/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchdialer;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Bundle;
import android.telecom.TelecomManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.lang.reflect.Method;


public class DialerActivity extends Activity {

    public static final String APP_TAG = "WatchDialer";
    TelecomManager tm;
    EditText phoneNumber;
    Button dialButton,clearButton;
    PhoneEventsHandler eventsHandler;
    RelativeLayout dialer_layout;
    Context context = this;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if(Utils.IsDisplayTypeCircular()){
        setContentView(R.layout.circular_activity_dialer);
        dialer_layout = (RelativeLayout) findViewById(R.id.dialer_layout);

        int paddingDp = context.getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
        dialer_layout.setPadding(paddingDp, paddingDp, paddingDp, 0);

            clearButton = (Button) findViewById(R.id.back_button);

        clearButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String curr_num = phoneNumber.getText().toString();
                int cursor_position = phoneNumber.getSelectionStart();
            
                if(cursor_position==1)
                {
                    if(curr_num.length()==1){
                        phoneNumber.setText("");    
                    }
                    else{
                        curr_num = curr_num.substring(1);
                        phoneNumber.setText(curr_num);
                        phoneNumber.setSelection(0);
                    }
                }
                else if(cursor_position!=0)
                {
                    String left = curr_num.substring(0,cursor_position-1);
                    String right = curr_num.substring(cursor_position);
                    curr_num = left+right;
                    phoneNumber.setText(curr_num);
                    phoneNumber.setSelection(cursor_position-1);
                }
            }
        });
    }
    else{
        setContentView(R.layout.activity_dialer);
    }

        tm =  (TelecomManager) getSystemService(TELECOM_SERVICE);
        dialButton = (Button) findViewById(R.id.button);
        phoneNumber = (EditText) findViewById(R.id.phoneNumberEditText);

        phoneNumber.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {

                int isDial = getResources().getInteger(R.integer.dial_value);
                Log.v(APP_TAG, " Action: " + actionId + " " + isDial);
                if(actionId == isDial) {
                    Log.d(APP_TAG, "Keyboard done");
                    dialNumber(phoneNumber.getText().toString());
                }
                return true;
            }
        });

        dialButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(APP_TAG, "Dialing " + phoneNumber.getText().toString());
                dialNumber(phoneNumber.getText().toString());
            }
        });
    }

    @Override
    protected void onStart() {
        phoneNumber.requestFocus();
        tm.showInCallScreen(tm.isInCall());
        super.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        Log.d(APP_TAG, "Touched outside keyboard" + phoneNumber.getText().toString());
        phoneNumber.clearFocus();
        dialButton.requestFocus();
        return super.onTouchEvent(event);
    }

    private void dialNumber(String number) {

        Uri directNumber;

        if (number.contentEquals("1")){
            directNumber = Uri.fromParts("voicemail", "", null);
        } else {
            directNumber = Uri.parse("tel:" + number);
        }

        try {
            Log.d(APP_TAG, "Starting Dialing.. ");
            Method method = tm.getClass().getMethod("placeCall", Uri.class, Bundle.class);
            method.invoke(tm,directNumber, null);
        } catch (Exception e) {
            e.printStackTrace();
        }
    finish();
    }

    public static Intent getCallIntent(String number) {
        return getCallIntent(getCallUri(number));
    }

    public static Intent getCallIntent(Uri uri) {
        return new Intent(Intent.ACTION_CALL, uri);
    }

    public static Uri getCallUri(String number) {
       return Uri.fromParts("tel:", number, null);
     }

    void registerForEvents() {
        IntentFilter phoneEventsHandler = new IntentFilter();
        phoneEventsHandler.addAction("android.intent.action.NEW_OUTGOING_CALL");
        this.registerReceiver(eventsHandler, phoneEventsHandler);
        Log.v(APP_TAG, "registered for " + phoneEventsHandler.toString());
    }

}
