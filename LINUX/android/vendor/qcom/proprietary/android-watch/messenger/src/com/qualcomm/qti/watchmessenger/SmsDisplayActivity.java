/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmessenger;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.provider.Telephony.Sms;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Button;

public class SmsDisplayActivity extends Activity {
    TextView msgTextView;
    Button replyButton;
    String sender;
    SmsBroadcastEventHandler smsBroadcastEventHandler;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.sms_display);

        msgTextView = (TextView) findViewById(R.id.msgContents);
        replyButton = (Button) findViewById(R.id.replyButton);

        String msgContents = getIntent().getExtras().getString("msg");
        sender = getIntent().getExtras().getString(Sms.ADDRESS);
        msgTextView.setText(msgContents);

        smsBroadcastEventHandler = new SmsBroadcastEventHandler(this);
    }

    protected void onStart() {
        super.onStart();
        smsBroadcastEventHandler.registerForEvents();

        replyButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Intent intent;
                intent = new Intent(SmsDisplayActivity.this, SmsComposer.class);
                intent.putExtra(Sms.ADDRESS,sender);
                startActivity(intent);
                finish();
            }
        });
    }

    protected void onStop() {
        super.onStop();
        smsBroadcastEventHandler.unregisterForEvents();
    }
}
