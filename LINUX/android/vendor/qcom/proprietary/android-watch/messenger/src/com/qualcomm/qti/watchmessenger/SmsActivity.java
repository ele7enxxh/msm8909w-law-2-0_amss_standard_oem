/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmessenger;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.Telephony.Sms;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.LinearLayout;
import android.widget.SimpleCursorAdapter;

import java.util.ArrayList;
import android.util.Log;

public class SmsActivity extends Activity {
    private Button newMsgButton;
    private ListView listView;
    private ArrayList<SmsData> smsData;
    SmsBroadcastEventHandler smsBroadcastEventHandler;
    SimpleCursorAdapter adapter;
    Cursor cursor;
    ContentResolver contentResolver;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sms);

        newMsgButton = (Button) findViewById(R.id.newMsgButton);

        String[] msgColumns = new String[] {
                Sms.ADDRESS,
                Sms.BODY,
                Sms.STATUS,
                Sms.DATE
        };

        int[] msgDetails = new int[] {
                R.id.msgFrom,
                R.id.msgText,
                R.id.msgStatus,
                R.id.msgDateTime,
        };

        adapter = new CursorAdapter(this, R.layout.sms_list_item, cursor,
        msgColumns, msgDetails, 0, getLoaderManager(), getContentResolver());

        listView = (ListView) findViewById(R.id.listView);
         // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            LinearLayout sms_layout = (LinearLayout) findViewById(R.id.sms_layout);
            sms_layout.setPadding(paddingDp,paddingDp,0,0);
            listView.setPadding(paddingDp, 0, 0, paddingDp);
        }

        listView.setAdapter(adapter);
        listView.setOnItemClickListener(
            (AdapterView.OnItemClickListener) adapter);
        listView.setOnItemLongClickListener(
            (AdapterView.OnItemLongClickListener) adapter);

        smsBroadcastEventHandler = new SmsBroadcastEventHandler(this);
    }

    protected void onResume() {
        super.onResume();
        adapter.notifyDataSetChanged();
    }

    public void onAdapterChanged() {
        adapter.notifyDataSetChanged();
    }

    protected void onStart() {
        super.onStart();
        smsBroadcastEventHandler.registerForEvents();

        newMsgButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Intent intent;
                intent = new Intent(SmsActivity.this, SmsComposer.class);
                startActivity(intent);
            }
        });
    }

    protected void onStop() {
        super.onStop();
        smsBroadcastEventHandler.unregisterForEvents();
    }
}
