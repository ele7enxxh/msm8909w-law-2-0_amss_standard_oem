/*============================================================================
@file TabControl.java

@brief
Main launch point for the QSensorTest application.  Manages all tabs.

Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.qsensortest;

import com.qualcomm.qti.sensors.core.qsensortest.CommandReceiver;
import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;
import com.qualcomm.qti.sensors.core.qsensortest.SettingsDatabase;
import com.qualcomm.qti.sensors.qsensortest.R;

import android.app.ActionBar;
import android.app.ActionBar.Tab;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.Intent;
import android.database.SQLException;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.Window;
import android.widget.Toast;
import android.widget.Button;
import android.view.View.OnClickListener;

import com.qualcomm.qti.sensors.ui.attributes.SensorAttributesActivity;
import com.qualcomm.qti.sensors.ui.graph.SensorGraphActivity;
import com.qualcomm.qti.sensors.ui.selftest.SelfTestActivity;
import com.qualcomm.qti.sensors.ui.snsregistry.SNSRegistryActivity;
import com.qualcomm.qti.sensors.ui.stream.StreamingActivity;
import com.qualcomm.qti.sensors.ui.stream.WearStreamingActivity;
import com.qualcomm.qti.sensors.ui.threshold.ThresholdActivity;
import com.qualcomm.qti.sensors.ui.usercal.UserCalActivity;
import com.qualcomm.qti.sensors.core.sensortest.SensorTest;
import com.qualcomm.qti.sensors.core.sensortest.SensorThresh;
import com.qualcomm.qti.sensors.core.sensortest.SensorsReg;
import com.qualcomm.qti.sensors.core.sensortest.AR;

import java.io.IOException;

public class TabControl extends Activity {
  static final private String TAG = QSensorContext.TAG;
  static private Context context;
  static private Activity activity;
  static public Context getContext() { return TabControl.context; }
  static public Activity getActivity() { return TabControl.activity; }
  public static boolean EnableWearQSTP = true;

  public Intent intent;
  private TabControlReceiver TabControlReceiver;
  private CommandReceiver commandReceiver;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    TabControl.context = this;
    TabControl.activity = this;

    QSensorContext.init(this);
    try {
        EnableWearQSTP = QSensorContext.readProperty("ro.sensors.wearqstp").equals("1");
        Log.d(QSensorContext.TAG,"Enable WearQSTP " +EnableWearQSTP);
    } catch(IOException e) {
        e.printStackTrace();
    }
    if (EnableWearQSTP) {
        Log.d(QSensorContext.TAG,"Enable WearQSTP " +EnableWearQSTP);
        Toast.makeText(getActivity(),"Started Streaming WearQSTA",Toast.LENGTH_SHORT).show();
        intent = new Intent(TabControl.this, WearStreamingActivity.class);
        startActivity(intent);
    } else {
        ActionBar actionBar = getActionBar();
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);
        actionBar.setDisplayShowTitleEnabled(true);

        Tab tab = actionBar.newTab()
            .setText(R.string.fragment_name_stream)
            .setTabListener(new TabListener<StreamingActivity>(
                this, "streaming", StreamingActivity.class));
        if(SettingsDatabase.getSettings().getFragmentEnabled(this.getResources().getResourceEntryName(R.string.fragment_name_stream)))
            actionBar.addTab(tab);

        tab = actionBar.newTab()
                .setText(R.string.fragment_name_graph)
                .setTabListener(new TabListener<SensorGraphActivity>(
                    this, "graph", SensorGraphActivity.class));
        if(SettingsDatabase.getSettings().getFragmentEnabled(this.getResources().getResourceEntryName(R.string.fragment_name_graph)))
            actionBar.addTab(tab);

        tab = actionBar.newTab()
            .setText(R.string.fragment_name_cal)
            .setTabListener(new TabListener<UserCalActivity>(
                this, "user_cal", UserCalActivity.class));
        if(SettingsDatabase.getSettings().getFragmentEnabled(this.getResources().getResourceEntryName(R.string.fragment_name_cal)))
            actionBar.addTab(tab);

        tab = actionBar.newTab()
            .setText(R.string.fragment_name_test)
            .setTabListener(new TabListener<SelfTestActivity>(
                this, "self_test", SelfTestActivity.class));
        try {
            new SensorTest();
            if(SettingsDatabase.getSettings().getFragmentEnabled(this.getResources().getResourceEntryName(R.string.fragment_name_test)))
                actionBar.addTab(tab);
        } catch(LinkageError e) {
            Toast.makeText(this, "Self Test tab disabled", Toast.LENGTH_LONG).show();
        }

        tab = actionBar.newTab()
            .setText(R.string.fragment_name_registry)
            .setTabListener(new TabListener<SNSRegistryActivity>(
                this, "registry", SNSRegistryActivity.class));
        try {
            new SensorsReg();
            if(SettingsDatabase.getSettings().getFragmentEnabled(this.getResources().getResourceEntryName(R.string.fragment_name_registry)))
                actionBar.addTab(tab);
        } catch(LinkageError e) {
            Toast.makeText(this, "Registry tab disabled", Toast.LENGTH_LONG).show();
        }

        tab = actionBar.newTab()
            .setText(R.string.fragment_name_thresh)
            .setTabListener(new TabListener<ThresholdActivity>(
                this, "thresh", ThresholdActivity.class));
        try {
            new SensorThresh();
            if(SettingsDatabase.getSettings().getFragmentEnabled(this.getResources().getResourceEntryName(R.string.fragment_name_thresh)))
                actionBar.addTab(tab);
        } catch(LinkageError e) {
            Toast.makeText(this, "Threshold tab disabled", Toast.LENGTH_LONG).show();
        }

        tab = actionBar.newTab()
            .setText(R.string.fragment_name_ar)
            .setTabListener(new TabListener<AR>(this, "Ar", AR.class));
            actionBar.addTab(tab);

        tab = actionBar.newTab()
            .setText(R.string.fragment_name_info)
            .setTabListener(new TabListener<SensorAttributesActivity>(
                this, "info", SensorAttributesActivity.class));
        if(SettingsDatabase.getSettings().getFragmentEnabled(this.getResources().getResourceEntryName(R.string.fragment_name_info)))
            actionBar.addTab(tab);
        }
  }

  /**
   * Load the options menu (defined in xml)
   *
   * @see android.app.Activity#onCreateOptionsMenu(android.view.Menu)
   */
  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    MenuInflater inflater = this.getMenuInflater();
    inflater.inflate(R.menu.qsensortest_menu, menu);

    MenuItem wlMenuItem = menu.findItem(R.id.wake_lock_toggle);
    String holdWL = SettingsDatabase.getSettings().getProperty("hold_wake_lock");
    wlMenuItem.setChecked(null != holdWL && holdWL.equals("1"));

    MenuItem smMenuItem = menu.findItem(R.id.suspend_monitor_toggle);
    smMenuItem.setChecked(QSensorContext.suspendMonitor().active());

    MenuItem opMenuItem = menu.findItem(R.id.optimize_power);
    opMenuItem.setChecked(QSensorContext.optimizePower);

    return super.onCreateOptionsMenu(menu);
  }

  /**
   * Defines what occurs when the user selects one of the menu options.
   *
   * @see android.app.Activity#onOptionsItemSelected(android.view.MenuItem)
   */
  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
    case R.id.exit:
      boolean closeEnabled = SettingsDatabase.getSettings().getMenuItemEnabled(
          TabControl.context.getResources().getResourceEntryName(R.id.exit));
      if(closeEnabled){
        this.finish();
      }
      return true;
    case R.id.suspend_monitor_toggle:
      boolean monitorEnabled = SettingsDatabase.getSettings().getMenuItemEnabled(
          TabControl.context.getResources().getResourceEntryName(R.id.suspend_monitor_toggle));
      if(monitorEnabled){
        if(QSensorContext.suspendMonitor().active())
          QSensorContext.suspendMonitor().stop();
        else
          QSensorContext.suspendMonitor().start();
        item.setChecked(QSensorContext.suspendMonitor().active());
      }
      return true;
    case R.id.wake_lock_toggle:
      boolean wlEnabled = SettingsDatabase.getSettings().getMenuItemEnabled(
          TabControl.getContext().getResources().getResourceEntryName(R.id.wake_lock_toggle));
      if(wlEnabled)
      {
        try{
          SettingsDatabase.getSettings().setProperty(
              "hold_wake_lock", item.isChecked() ? "0" : "1");
          item.setChecked(!item.isChecked());
        } catch(SQLException e) {
          Toast.makeText(TabControl.getActivity(), "Unable to change setting", Toast.LENGTH_LONG).show();
        }

        if(item.isChecked())
          QSensorContext.wakeLock().acquire();
        else
          QSensorContext.wakeLock().release();
      }
      return true;
    case R.id.optimize_power:
      boolean menuItemEnabled = SettingsDatabase.getSettings().getMenuItemEnabled(
          TabControl.getContext().getResources().getResourceEntryName(R.id.optimize_power));
      if(menuItemEnabled) {
        QSensorContext.optimizePower = !QSensorContext.optimizePower;
        item.setChecked(QSensorContext.optimizePower);
      }
      return true;
    default:
      return super.onOptionsItemSelected(item);
    }
  }

  @Override
  public void onResume() {
    super.onResume();
    if(null == this.TabControlReceiver)
      this.TabControlReceiver = new TabControlReceiver();
    TabControl.getContext().registerReceiver(this.TabControlReceiver, this.TabControlReceiver.getIntentFilter());
    if(null == this.commandReceiver)
      this.commandReceiver = new CommandReceiver();
    TabControl.getContext().registerReceiver(this.commandReceiver, this.commandReceiver.getIntentFilter());
  }

  @Override
  public void onPause() {
    super.onPause();
    if(null != this.TabControlReceiver)
      TabControl.getContext().unregisterReceiver(this.TabControlReceiver);
    if(null != this.commandReceiver)
      TabControl.getContext().unregisterReceiver(this.commandReceiver);
  }

  private static class TabListener<T extends Fragment> implements ActionBar.TabListener {
    private Fragment fragment;
    private final Activity activity;
    private final String tag;
    private final Class<T> fragmentClass;

    /** Constructor used each time a new tab is created.
     *
     * @param activity  The host Activity, used to instantiate the fragment
     * @param tag  The identifier tag for the fragment
     * @param fragmentClass  The fragment's Class, used to instantiate the fragment
     */
    public TabListener(Activity activity, String tag, Class<T> fragmentClass) {
      this.activity = activity;
      this.tag = tag;
      this.fragmentClass = fragmentClass;
    }
    @Override
    public void onTabSelected(Tab tab, FragmentTransaction ft) {
      if(null == this.fragment) {
        this.fragment = Fragment.instantiate(this.activity, this.fragmentClass.getName());
        ft.add(android.R.id.content, this.fragment, this.tag);
      }
      else
        ft.attach(this.fragment);
    }
    @Override
    public void onTabUnselected(Tab tab, FragmentTransaction ft) {
      if (this.fragment != null)
        ft.detach(this.fragment);
    }
    @Override
    public void onTabReselected(Tab tab, FragmentTransaction ft) {}
  }
}
