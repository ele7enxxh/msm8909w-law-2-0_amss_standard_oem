/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
   All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.launcher;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.res.Configuration;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.SystemProperties;
import android.util.Log;
import android.view.View;
import android.widget.ListView;

import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

public class MainActivity extends Activity {

    private static final String APP_TAG = "LauncherActivity";
    private static final String removeAospPackages = "debug.removeAospPackages";
    private String PanelDisplay;

    /**
     * Class used for storing necessary package information to be
     * displayed in the app launcher grid
     */
    class PackageDetail {
        Drawable icon; // app icon
        String name;   // the package name like x.y.app
        String label;  // the name to be shown to the user
    }

    private PackageDetail[] packageDetails; // array to store details of all packages
    private PackageManager pm;
    private ListView listView;
    private GridAdapter gridAdapter;
    private IntentFilter packageUpdatesFilter; // intent filter for package updates
    private PackageUpdatesReceiver packageUpdatesReceiver; // braodcast receiver for package events

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        pm = getPackageManager();
        listView = (ListView) findViewById(R.id.content);

        setPackageDetails(); // get required details about each package

        testSystemFeature(); // debug logging to tell whether watch mode is set
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d(APP_TAG, "App Start called");

        // add slide in animation
        this.overridePendingTransition(android.R.anim.slide_in_left, android.R.anim.slide_out_right);
    }

    @Override
    protected void onResume() {
        super.onResume();
        registerForPackageUpdates();

        // hide the status bar to make view full screen
        getWindow().getDecorView().setSystemUiVisibility(View.
            SYSTEM_UI_FLAG_FULLSCREEN);
    }

    @Override
    protected void onPause() {
        Log.d(APP_TAG, "Un un-registering package update receiver");
        unregisterReceiver(packageUpdatesReceiver);
        super.onPause();
    }

    @Override
    protected void onStop() {
        Log.v(APP_TAG, "Main Activity exiting..");
        finish();
        super.onStop();
    }

    /**
     * Extract the required details of each package which should be shown in the launcher and
     * update the grid view
     */
    void setPackageDetails() {
        // find out all the application with MAIN and LAUNCHER intent
        final Intent mainIntent = new Intent(Intent.ACTION_MAIN, null);
        mainIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        List<ResolveInfo> packageList = pm.queryIntentActivities(mainIntent,0);

        removeSelfPackage(packageList);
        removeExcludedPackages(packageList);

        // store package details in array
        packageDetails = new PackageDetail[packageList.size()];
        for (int i = 0; i < packageList.size(); ++i) {
            packageDetails[i] = new PackageDetail();
            packageDetails[i].icon = packageList.get(i).loadIcon(pm);
            packageDetails[i].name = packageList.get(i).activityInfo.packageName;
            packageDetails[i].label = packageList.get(i).loadLabel(pm).toString();

            Log.v(APP_TAG, "Packages    " + i + " " + packageDetails[i].name);
        }

        // sort the array in ascending order of its labels
        Arrays.sort(packageDetails, new AppDisplayOrderComparator());

        // update the grid view with package details
        setGridData();
    }

    /**
     * This function removes the current package from the list of input packages
     * @param pacList list of packages from which this current package should be removed
     */
    private void removeSelfPackage(List<ResolveInfo> pacList) {
        Log.v(APP_TAG, "Total Packages found " + pacList.size());
        // iterate through the packages and remove this launcher package from the list
        for (int i = 0; i < pacList.size(); ++i) {
            if (pacList.get(i).activityInfo.packageName.equals(MainActivity.class.getPackage().getName())) {
                Log.v(APP_TAG, "Ignoring package " + pacList.get(i).activityInfo.packageName);
                pacList.remove(i);
                break;
            }
        }
    }

    /**
     * This function removes predefined packages which should not be shown in the launcher
     * @param pacList list of packages from which predefined packages should be removed
     */
    private void removeExcludedPackages(List<ResolveInfo> pacList) {
        String[] excludePackageList = getResources().getStringArray(
            R.array.excludePackageList);
        String[] excludeAospPackageList = getResources().getStringArray(
            R.array.excludeAospPackageList);

        for (int index = 0; index < excludePackageList.length; ++index) {
            for (ResolveInfo packageIter : pacList) {
                if (excludePackageList[index].equals(packageIter.activityInfo.packageName)) {
                    Log.v(APP_TAG, "Excluding Package " + packageIter.activityInfo.packageName);
                    pacList.remove(packageIter);
                    break;
                }
            }
        }

        if (SystemProperties.getBoolean(removeAospPackages, true)) {
            for (int index = 0; index < excludeAospPackageList.length; ++index) {
                for (ResolveInfo packageIter : pacList) {
                    if (excludeAospPackageList[index].equals(
                        packageIter.activityInfo.packageName)) {
                            Log.v(APP_TAG, "Excluding Package " +
                                packageIter.activityInfo.packageName);
                            pacList.remove(packageIter);
                            break;
                    }
                }
            }
        }
    }

    /**
     * Update grid view with new package information
     */
    private void setGridData() {
        gridAdapter = new GridAdapter(this, packageDetails);
        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            listView.setPadding(paddingDp, paddingDp, 0, paddingDp);
        }

        listView.setAdapter(gridAdapter);

        /* Register for single click event which launches application */
        listView.setOnItemClickListener(new GridClickListener(this, packageDetails, pm));

        /* set long press click listener on grid view. Long press deletes applications */
        listView.setLongClickable(true);
        listView.setOnItemLongClickListener(new GridLongClickListener(this, packageDetails));

    }

    /**
     * Debug method to print UI mode type and hardware type.
     */
    private void testSystemFeature() {

        //hardware type configuration test
        if (pm.hasSystemFeature(PackageManager.FEATURE_WATCH)) {
            Log.d(APP_TAG, "This is a hardware type watch");
        }
        else {
            Log.w(APP_TAG, "This is not a hardware type watch");
        }

        // ui mode type test
        Configuration config = getResources().getConfiguration();
        if ((config.uiMode & Configuration.UI_MODE_TYPE_MASK) == Configuration.UI_MODE_TYPE_WATCH) {
            Log.d(APP_TAG, "This is a UI Mode Watch " + config.uiMode);
        }
        else {
            Log.w(APP_TAG, "This is a NOT UI Mode Watch " + config.uiMode);
        }
    }

    /**
     * Register for package events. If any package is added, deleted or updated, the grid view
     * needs to be updated as well.
     */
    private void registerForPackageUpdates() {
        packageUpdatesFilter = new IntentFilter(Intent.ACTION_PACKAGE_ADDED);
        packageUpdatesFilter.addAction(Intent.ACTION_PACKAGE_REMOVED);
        packageUpdatesFilter.addAction(Intent.ACTION_PACKAGE_CHANGED);
        packageUpdatesFilter.addDataScheme("package");
        packageUpdatesReceiver = new PackageUpdatesReceiver();

        Log.d(APP_TAG, "registering package update receiver");
        registerReceiver(packageUpdatesReceiver, packageUpdatesFilter);

    }

    /**
     * This class handles package update broadcasts.
     */
    public class PackageUpdatesReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context c, Intent i) {
            Log.v(APP_TAG, "Received package update event");
            setPackageDetails(); // update the grid view with new package information
        }
    }

    /**
     * This class is used to compare two package details labels.
     */
    public class AppDisplayOrderComparator implements Comparator<PackageDetail> {
        @Override
        public int compare(PackageDetail lhs, PackageDetail rhs) {
            if (rhs.label == null || lhs.label == null) {
                Log.d(APP_TAG, "No Label found for this application..");
                return 0;
            }
            return lhs.label.compareTo(rhs.label);
        }
    }
}
