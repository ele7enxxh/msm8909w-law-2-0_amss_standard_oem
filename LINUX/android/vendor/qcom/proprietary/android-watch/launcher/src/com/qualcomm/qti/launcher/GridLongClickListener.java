/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */


package com.qualcomm.qti.launcher;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;

/**
 * This class handles long press on any application in the grid view
 */
public class GridLongClickListener implements AdapterView.OnItemLongClickListener {
    private static final String APP_TAG = "GridLongClick";

    private Context mContext;
    private MainActivity.PackageDetail[] installedPackages; // array of installed apps

    /**
     *  Constructor to store context and the array of packages
     */
    public GridLongClickListener(Context c, MainActivity.PackageDetail[] pacs) {
        mContext = c;
        installedPackages = pacs;
    }

    @Override
    public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
        // get the package details of the package where long click was done
        MainActivity.PackageDetail thisPackage = installedPackages[position];

        Log.v(APP_TAG, "Uninstalling package: " + thisPackage.toString());
        deletePackage(thisPackage);

        return true; // indicates this event is handled and should not be passed to single click listener
    }

    /**
     *  Uninstall this package from android apps
     */
    private void deletePackage(MainActivity.PackageDetail thisPackage) {
        // Send a delete intent to a package manager to uninstall this package
        Uri packageURI = Uri.parse("package:" + thisPackage.name);
        Intent uninstallIntent = new Intent(Intent.ACTION_DELETE, packageURI);
        mContext.startActivity(uninstallIntent);
    }
}
