/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchcontacts;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class ContactDetailsActivity extends Activity {

    ArrayList<PhoneNumberItem> mContactsList = null;
    public static final String TAG = "ContactDetails";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.phonenumber_details_activity);
        Intent intent = getIntent();
        String contactId = intent.getStringExtra(ContactUtils.CONTACT_ID);
        String contactName = intent.getStringExtra(ContactUtils.CONTACT_NAME);

        TextView contactNameTv = (TextView) findViewById(R.id.contact_name);
        ListView contactListView = (ListView) findViewById(R.id.multi_contact_list);

        mContactsList = new ArrayList<PhoneNumberItem>();
        contactNameTv.setText(contactName);

        log("fetching contact details: Name=" + contactName);
        String[] projection = new String[] {
                ContactsContract.CommonDataKinds.Phone.NUMBER,
                ContactsContract.CommonDataKinds.Phone.TYPE};
        String selection = ContactsContract.CommonDataKinds.Phone.CONTACT_ID
                + " = ?";
        String[] selectionArgs = new String[]{contactId};
        Cursor pCur = getContentResolver().query(
                ContactsContract.CommonDataKinds.Phone.CONTENT_URI,
                projection,
                selection,
                selectionArgs,
                null);

        while (pCur != null && pCur.moveToNext()) {
            PhoneNumberItem phoneNumItem = new PhoneNumberItem();
            phoneNumItem.phoneNumber = pCur.getString(pCur.getColumnIndex(
                    ContactsContract.CommonDataKinds.Phone.NUMBER));
            phoneNumItem.phoneType = pCur.getString(pCur.getColumnIndex(
                    ContactsContract.CommonDataKinds.Phone.TYPE));
            mContactsList.add(phoneNumItem);
        }
        contactListView.setAdapter(new PhoneNumListAdapter());
        contactListView.setOnItemClickListener(new ContactListItemClickListener());
    }

    private void log(String val) {
        Log.d(TAG, val);
    }

    class ContactListItemClickListener implements AdapterView.OnItemClickListener{

        @Override
        public void onItemClick(AdapterView<?> adapterView, View view,
                                int position, long id) {
           PhoneNumListAdapter.ViewHolder viewHolder = (PhoneNumListAdapter.ViewHolder) view.getTag();
            if(viewHolder != null) {
                Intent intent = new Intent(Intent.ACTION_CALL);
                intent.setData(Uri.parse("tel:" + viewHolder.phoneNumView.getText()));
                startActivity(intent);
            }
        }
    }

    class PhoneNumListAdapter extends BaseAdapter {

        private LayoutInflater inflater = null;
        PhoneNumberItem phNumItem = null;

        public PhoneNumListAdapter() {
            inflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override
        public int getCount() {
            return mContactsList.size();
        }

        @Override
        public Object getItem(int i) {
            return mContactsList.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {

            View vi = view;
            ViewHolder holder;
            if (view == null) {
                vi = inflater.inflate(R.layout.phone_list_item, null);
                holder = new ViewHolder();
                holder.phoneTypeView = (TextView) vi.findViewById(R.id.phone_type_item);
                holder.phoneNumView = (TextView) vi.findViewById(R.id.phone_num_item);
                vi.setTag(holder);
            } else {
                holder = (ViewHolder) vi.getTag();
            }
            phNumItem = (PhoneNumberItem) mContactsList.get(i);
            String phoneType = ContactUtils.PHONE_TYPES.get(phNumItem.phoneType);
            if (phoneType == null) {
                phoneType = ContactUtils.PHONE_TYPES_OTHERS;
            }
            holder.phoneTypeView.setText(phoneType);
            holder.phoneNumView.setText(phNumItem.phoneNumber);
            return vi;
        }

        class ViewHolder {

            TextView phoneTypeView;
            TextView phoneNumView;

        }
    }

    class PhoneNumberItem {

        String phoneType;
        String phoneNumber;
    }
}
