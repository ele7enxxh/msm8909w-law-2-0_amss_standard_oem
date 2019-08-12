/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.launcher;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.view.View;
import android.widget.AdapterView;


/**
 * This class handles single click on application grid and launches the appropriate app
 */
public class GridClickListener implements AdapterView.OnItemClickListener {

    private Context mContext;
    private MainActivity.PackageDetail[] pacsForAdapter;
    private PackageManager pmForListener;

    public GridClickListener(Context c, MainActivity.PackageDetail[] pacs, PackageManager pm) {
        mContext = c;
        pacsForAdapter = pacs;
        pmForListener = pm;
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        // Send a LAUNCH intent for the application which was clicked on the grid view
        Intent launchIntent = pmForListener.getLaunchIntentForPackage(pacsForAdapter[position].name);
        mContext.startActivity(launchIntent);
    }
}
