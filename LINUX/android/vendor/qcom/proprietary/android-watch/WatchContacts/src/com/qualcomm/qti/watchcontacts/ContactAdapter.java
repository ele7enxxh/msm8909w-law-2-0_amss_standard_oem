/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchcontacts;

import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.provider.ContactsContract;
import android.support.v4.widget.CursorAdapter;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class ContactAdapter extends CursorAdapter {

    private ContentResolver mContentResolver;

    public ContactAdapter(Context context, Cursor c, boolean autoRequery) {
        super(context, c, autoRequery);
        mContentResolver = context.getContentResolver();
    }

    @Override
    public void bindView(View view, Context context, Cursor cursor) {
        ListViewHolder listViewHolder = (ListViewHolder) view.getTag();
        if (listViewHolder == null) {
            listViewHolder = new ListViewHolder();
            listViewHolder.mDisplayName = (TextView) view
                    .findViewById(R.id.name);
            view.setTag(listViewHolder);
        }
        String displayName = cursor.getString(cursor
                .getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME_PRIMARY));
        listViewHolder.mDisplayName.setText(displayName);
    }

    @Override
    public View newView(Context context, Cursor cursor, ViewGroup parent) {
        final LayoutInflater inflater = LayoutInflater.from(context);
        return inflater.inflate(R.layout.contact_list_item, parent, false);
    }
}
