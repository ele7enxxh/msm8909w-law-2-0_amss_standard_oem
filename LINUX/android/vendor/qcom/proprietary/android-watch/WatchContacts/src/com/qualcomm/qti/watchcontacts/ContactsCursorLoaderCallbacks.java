/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchcontacts;

import android.app.Activity;
import android.app.LoaderManager;
import android.content.CursorLoader;
import android.content.Loader;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.util.Log;

public class ContactsCursorLoaderCallbacks implements LoaderManager.LoaderCallbacks<Cursor> {

    private static final String TAG = "ContactsCursorLoaderCallbacks";
    private Activity mActivity;
    private ContactAdapter mAdapter;
    protected OnLoadFinishedListener mOnLoadFinishedListener;
    protected OnLoaderResetListener mOnLoaderResetListener;
    private CursorLoader mCursorLoader = null;

    public ContactsCursorLoaderCallbacks(Activity activity, ContactAdapter adapter) {
        mActivity = activity;
        mAdapter = adapter;
    }

    @Override
    public Loader<Cursor> onCreateLoader(int id, Bundle bundle) {
        Uri contentUri = ContactsContract.Contacts.CONTENT_URI;
        String[] projection = new String[] {
                ContactsContract.Contacts._ID,
                ContactsContract.Contacts.DISPLAY_NAME_PRIMARY};
        if(mCursorLoader == null) {
            // Starts the query
            CursorLoader cursorLoader =  new CursorLoader(
                    mActivity,
                    contentUri,
                    projection,
                    null,
                    null,
                    ContactsContract.Data.DISPLAY_NAME_PRIMARY + " ASC"
            );
            mCursorLoader = cursorLoader;
        }
        return mCursorLoader;
    }

    @Override
    public void onLoadFinished(Loader<Cursor> loader, Cursor newCursor) {
        if (mAdapter != null) {
            mAdapter.changeCursor(newCursor);
         } else {
            Log.d(TAG, "adapter is null on onLoadFinished");
         }
         if (mOnLoadFinishedListener != null) {
            mOnLoadFinishedListener.onLoadFinished(loader, newCursor);
         }
    }

    @Override
    public void onLoaderReset(Loader<Cursor> loader) {
        if(mAdapter != null) {
            mAdapter.changeCursor(null);
         } else {
            Log.d(TAG, "adapter is null on onLoadReset");
         }
         if(mOnLoaderResetListener != null) {
            mOnLoaderResetListener.onLoaderReset(loader);
         }
    }

    public void setOnLoadFinishedListener(OnLoadFinishedListener onLoadFinishedListener) {
        mOnLoadFinishedListener = onLoadFinishedListener;
     }

    public void setOnLoaderResetListener(OnLoaderResetListener onLoaderResetListener) {
        mOnLoaderResetListener = onLoaderResetListener;
    }

    /**
     * Interface used to allow the creator of a CursorLoaderCallBacks to run
     * some code when LoaderManager's onLoadFinished callback is called
     *
     */
    public static interface OnLoadFinishedListener {
        void onLoadFinished(Loader<Cursor> loader, Cursor cursor);
    }

    /**
     * Interface used to allow the creator of a CursorLoaderCallBacks to run
     * some code when LoaderManager's  onLoaderReset callback is called
     */
    public static interface OnLoaderResetListener {
        void onLoaderReset(Loader<Cursor> loader);
    }
}
