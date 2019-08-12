/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmessenger;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.provider.Telephony.Sms;

import java.text.SimpleDateFormat;
import java.util.ArrayList;

public class SmsComposer extends Activity {
    private TextView msgToTitle;
    private TextView msgTextTitle;
    private EditText phoneNum;
    private EditText msgBody;
    private Button sendButton;
    private SmsManager smsManager;
    private PendingIntent msgSentPendingIntent;
    private PendingIntent msgDeliveredPendingIntent;
    private Context context;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.sms_composer);
        context = this;

        smsManager = SmsManager.getDefault();
        msgToTitle = (TextView) findViewById(R.id.msgToTitle);
        msgTextTitle = (TextView) findViewById(R.id.msgTextTitle);

        phoneNum = (EditText) findViewById(R.id.phoneNum);
        msgBody = (EditText) findViewById(R.id.msgBody);
        sendButton = (Button) findViewById(R.id.sendButton);

        Intent reply = getIntent();
        if(reply !=null && reply.getStringExtra(Sms.ADDRESS)!=null) {
            phoneNum.setText(reply.getStringExtra(Sms.ADDRESS));
        }
        composeMsg();
    }

    public void composeMsg() {
        sendButton.setOnClickListener(new View.OnClickListener() {

            public void onClick(View view) {

                String phoneNumber = phoneNum.getText().toString();
                String msgText = msgBody.getText().toString();
                ArrayList<String> messages = null;
                int messageCount = 0;
                ArrayList<PendingIntent> sentIntents = null;
                ArrayList<PendingIntent> deliveredIntents = null;

                if (phoneNumber.isEmpty()) {
                    Toast.makeText(getApplicationContext(), context.getString(
                    R.string.enter_phone_num), Toast.LENGTH_SHORT).show();
                    return;
                }

                if (msgText.isEmpty()) {
                    Toast.makeText(getApplicationContext(), context.getString(
                    R.string.type_message_text), Toast.LENGTH_SHORT).show();
                    return;
                }

                Uri rowIdUri = Utils.updateProvider(context, phoneNumber, msgText);
                long rowIndex = Long.valueOf(ContentUris.parseId(rowIdUri));

                messages = smsManager.divideMessage(msgText);
                messageCount = messages.size();

                if(messageCount == 1){
                    Intent msgSentIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_SENT_ACTION);
                    msgSentIntent.putExtra("rowIndex", rowIndex);
                    msgSentPendingIntent = PendingIntent.getBroadcast(context, 0, msgSentIntent, PendingIntent.FLAG_UPDATE_CURRENT);

                    Intent msgDeliveredIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_DELIVERED_ACTION);
                    msgDeliveredIntent.putExtra("rowIndex", rowIndex);
                    msgDeliveredPendingIntent = PendingIntent.getBroadcast(context, 0, msgDeliveredIntent, PendingIntent.FLAG_UPDATE_CURRENT);

                    smsManager.sendTextMessage(phoneNumber, null, msgText,
                    msgSentPendingIntent, msgDeliveredPendingIntent);
                } else {
                    sentIntents = new ArrayList<PendingIntent>(messageCount);
                    deliveredIntents = new ArrayList<PendingIntent>(messageCount);

                    for(int i=0; i < messageCount; i++) {
                        Intent msgSentIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_SENT_ACTION);
                        msgSentIntent.putExtra("rowIndex", rowIndex);
                        msgSentPendingIntent = PendingIntent.getBroadcast(context, 0, msgSentIntent, PendingIntent.FLAG_UPDATE_CURRENT);
                        sentIntents.add(msgSentPendingIntent);

                        Intent msgDeliveredIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_DELIVERED_ACTION);
                        msgDeliveredIntent.putExtra("rowIndex", rowIndex);
                        msgDeliveredPendingIntent = PendingIntent.getBroadcast(context, 0, msgDeliveredIntent, PendingIntent.FLAG_UPDATE_CURRENT);
                        deliveredIntents.add(msgDeliveredPendingIntent);
                    }

                    smsManager.sendMultipartTextMessage(phoneNumber, null, messages, sentIntents, deliveredIntents);
                }

                finish();
            }
        });
    }
}
