/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchcontacts;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.content.Loader;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.ContactsContract;
import android.provider.ContactsContract.Contacts;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ListView;

import com.qualcomm.qti.watchcontacts.ContactsCursorLoaderCallbacks.OnLoadFinishedListener;

import java.io.File;
import java.util.ArrayList;

public class ContactsActivity extends Activity {

    private static final String TAG = "watch-contacts:";
    private ContactAdapter mAdapter;
    private Activity mActivity;
    private ListView contactListView;
    private ArrayList<Contacts> contactList;
    private ContactsCursorLoaderCallbacks mCursorLoaderCallbacks;
    private int CONTACT_LOADER_ID = 1001;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.contacts_list_view);

        ImageButton.OnClickListener btnNewContactListner = new ImageButton.OnClickListener() {
            @Override
            public void onClick(View view) {
                readFronVCF();
            }
        };

        ImageButton.OnClickListener deleteContactsListner = new ImageButton.OnClickListener() {
            @Override
            public void onClick(View view) {
                deleteAllContacts();
            }
        };

        Button newContactBtn = (Button) findViewById(R.id.new_contact);
        newContactBtn.setOnClickListener(btnNewContactListner);
        newContactBtn.setVisibility(View.GONE);

        Button deleteContactsBtn = (Button) findViewById(R.id.delete_contacts_but);
        deleteContactsBtn.setOnClickListener(deleteContactsListner);

        contactListView = (ListView)findViewById(R.id.contact_list);
        contactList = new ArrayList<Contacts>();

        mAdapter = new ContactAdapter(this, null, false);
        mActivity = this;

        //TODO: display Progress dialog

        contactListView.setAdapter(mAdapter);
        contactListView.setOnItemClickListener(new ContactListItemClickListener());
        setupAdapter();
    }

    private void setupAdapter() {
        mCursorLoaderCallbacks = new ContactsCursorLoaderCallbacks(mActivity, mAdapter);
        mCursorLoaderCallbacks.setOnLoadFinishedListener(new OnLoadFinishedListener() {
            @Override
            public void onLoadFinished(Loader<Cursor> loader, Cursor cursor) {
                //TODO: Remove loader animation.
                if(cursor != null && cursor.getCount() > 0) {
                    log("Contacts count = " + cursor.getCount());
                } else {
                    updateEmptyListView();
                }
            }
         });

        if (getLoaderManager().getLoader(CONTACT_LOADER_ID) == null) {
            getLoaderManager().initLoader(CONTACT_LOADER_ID, new Bundle(),
                    mCursorLoaderCallbacks);
        } else {
            getLoaderManager().restartLoader(CONTACT_LOADER_ID, new Bundle(),
                    mCursorLoaderCallbacks);
        }
    }

    private void log(String logStr) {
        Log.i(TAG, logStr);
    }

    private void updateEmptyListView() {
        Button importContactsBtn = (Button) findViewById(R.id.new_contact);
        importContactsBtn.setVisibility(View.VISIBLE);
    }

    private void readFronVCF() {
        Button newContactBtn = (Button) findViewById(R.id.new_contact);
        newContactBtn.setVisibility(View.GONE);

        final String vfile = "PIM.vcf";
        String vfsFilePath = Environment.getExternalStorageDirectory().toString() +
                File.separator + vfile;
        Log.i(TAG, "vcf file path="+vfsFilePath);
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setDataAndType(Uri.fromFile(new File(vfsFilePath)), "text/x-vcard");
        startActivity(intent);
    }

    class ContactListItemClickListener implements AdapterView.OnItemClickListener{

        @Override
        public void onItemClick(AdapterView<?> adapterView, View view,
                int position, long id) {
            ListViewHolder viewHolder = (ListViewHolder) view.getTag();
            if(viewHolder != null) {
                String name = String.valueOf(viewHolder.mDisplayName.getText());
                Log.d(TAG, "Name " + name);

                Intent intent = new Intent(ContactsActivity.this, ContactDetailsActivity.class);
                intent.putExtra(ContactUtils.CONTACT_ID, String.valueOf(id));
                intent.putExtra(ContactUtils.CONTACT_NAME, name);
                startActivity(intent);
            }
        }
    }

    private void deleteAllContacts() {
        log("deleteAllContacts()");
        Button deleteContactsBtn = (Button) findViewById(R.id.delete_contacts_but);
        deleteContactsBtn.setVisibility(View.GONE);

        ContentResolver cr = getContentResolver();
        Cursor cur = cr.query(ContactsContract.Contacts.CONTENT_URI,
                null, null, null, null);
        if(cur !=null) {
            log("contacts count = " + cur.getCount());
            int count = 0;
            while (cur.moveToNext()) {
                try {
                    String lookupKey = cur.getString(cur.getColumnIndex(
                            ContactsContract.Contacts.LOOKUP_KEY));
                    Uri uri = Uri.withAppendedPath(
                            ContactsContract.Contacts.CONTENT_LOOKUP_URI, lookupKey);
                    log("           The uri is " + uri.toString());
                    cr.delete(uri, null, null);
                }
                catch(Exception e) {
                    log(e.getMessage());
                }
            }
        }
    }
}
