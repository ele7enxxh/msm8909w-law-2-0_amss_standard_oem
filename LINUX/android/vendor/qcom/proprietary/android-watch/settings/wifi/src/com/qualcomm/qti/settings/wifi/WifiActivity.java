/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchwifi;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.LinearLayout;
import android.os.SystemProperties;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Comparator;
import java.util.Collections;

import static android.net.wifi.WifiManager.calculateSignalLevel;

public class WifiActivity extends Activity {

    private static final String APP_TAG = "WifiSettings";
    private static final String DISPLAY_TYPE = "ro.display.type";
    private final Context context = this;
    private ProgressDialog mWifiProgressDialog;
    private ListView listView;
    private boolean getNewScanResults;
    private boolean getSupplicantStatus;
    private TextView wifiStatus;
    private String PanelDisplay;
    private RelativeLayout mRectBackground;
    private RelativeLayout mRoundBackground;

    WifiManager mWifiManager;

    ArrayAdapter<AcessPointInfo> apScanArrayAdapter;
    ArrayList<AcessPointInfo> apInfoList = new ArrayList<AcessPointInfo>();

    //Used to calculate Access Point signal level in the range of 0-100%
    private static final int MAX_SIGNAL_LEVEL = 100;
    private LinearLayout wifiSwitch;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifi);

        if(mWifiProgressDialog == null){
            mWifiProgressDialog = new ProgressDialog(context);
            mWifiProgressDialog.setTitle(context.getString(
                R.string.wifi_scan_progress_title));
            mWifiProgressDialog.setMessage(context.getString(
                R.string.wifi_scan_progress_message));
            mWifiProgressDialog.setCancelable(false);
        }

        //Get instance of WifiManager class
        mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);

        //Switch for turning ON/OFF wifi
        wifiSwitch = (LinearLayout) findViewById(R.id.wifi_switch);
        wifiStatus = (TextView) findViewById(R.id.wifiStatus);

        //Set initial state of switch based on wifi state
        if (!mWifiManager.isWifiEnabled()) {
            wifiStatus.setText(R.string.wifi_switch_state_off);
        } else {
            wifiStatus.setText(R.string.wifi_switch_state_on);
        }

        wifiSwitch.setOnClickListener
            (new OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (!mWifiManager.isWifiEnabled()) {
                        boolean mEnableWifi = mWifiManager.setWifiEnabled(true);
                        Toast.makeText(context, R.string.enabling_wifi_now,
                        Toast.LENGTH_SHORT).show();
                        Log.d(APP_TAG,"Enabling Wifi now");
                    } else {
                        boolean mWifiDisabled = mWifiManager.setWifiEnabled(false);
                        Log.d(APP_TAG,"Disabling Wifi now");
                        apScanArrayAdapter.clear();
                        apInfoList.clear();
                    }
                }
            });

        class WifiListItemViewHolder {
            TextView apName;
            TextView apStrength;
            TextView apStatus;
        }

        //Use an array adapter to list all APs
        int resID = R.layout.wifi_list_item;
        apScanArrayAdapter = new ArrayAdapter<AcessPointInfo>(this, resID){
            @Override
            public View getView(int position, View convertView,
                ViewGroup parent) {

                final WifiListItemViewHolder viewHolder;
                LayoutInflater li = (LayoutInflater) context.getSystemService(
                    Context.LAYOUT_INFLATER_SERVICE);

                if (convertView == null) {
                    convertView = li.inflate(R.layout.wifi_list_item, null);

                    viewHolder = new WifiListItemViewHolder();

                    viewHolder.apName = (TextView) convertView.findViewById(
                        R.id.ap_name);
                    viewHolder.apStrength = (TextView) convertView.findViewById(
                        R.id.ap_strength);
                    viewHolder.apStatus = (TextView) convertView.findViewById(
                        R.id.ap_status);

                    convertView.setTag(viewHolder);

                } else {
                    viewHolder = (WifiListItemViewHolder) convertView.getTag();
                }

                if (!apInfoList.isEmpty()) {

                    AcessPointInfo apInfo = apInfoList.get(position);

                    viewHolder.apName.setText(apInfo.getApName());
                    viewHolder.apStrength.setText(String.valueOf(
                        "Strength = " + apInfo.getApStrength() + "%"));
                    viewHolder.apStatus.setText(apInfo.getApStatus());
                }

                return convertView;
            }

            //Show all options only if developer options is turned ON
            @Override
            public int getCount() {
                return apInfoList.size();
            }
        };

        listView = (ListView) findViewById(R.id.ap_list_view);

        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            int paddingDp = context.getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            listView.setPadding(paddingDp, 0, 0, paddingDp);
        }

        listView.setAdapter(apScanArrayAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                int position, long id) {

                final AcessPointInfo ap = apInfoList.get(position);
                final String capabilities = ap.getApCapabilities();

                final String connectedApName;
                final String selectedApName;
                final int netID;
                connectedApName = mWifiManager.getConnectionInfo().getSSID();
                selectedApName = String.format("\"%s\"", ap.getApName());
                netID = mWifiManager.getConnectionInfo().getNetworkId();
                if (connectedApName.equals(selectedApName) &&
                    ap.getApStatus().equals(context.getString(
                    R.string.connected))) {

                    AlertDialog.Builder alertDialog =
                        new AlertDialog.Builder(WifiActivity.this);

                    alertDialog.setTitle(R.string.forget_network_title);
                    alertDialog.setMessage(R.string.forget_network_message);

                    alertDialog
                        .setCancelable(true)
                        .setPositiveButton(R.string.yes,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                forgetConnectedNetwork(ap, netID, connectedApName);
                            }
                        })
                        .setNegativeButton(R.string.no, null)
                        .create()
                        .show();
                    return;
                }

                /**
                 * Get instance of WifiConfiguration class and initialize
                 * security configuration common to WAP/WAP2/WEP/OPEN networks
                 */
                final WifiConfiguration mWifiConfig = new WifiConfiguration();

                mWifiConfig.allowedGroupCiphers.set(
                    WifiConfiguration.GroupCipher.WEP40);
                mWifiConfig.allowedGroupCiphers.set(
                    WifiConfiguration.GroupCipher.WEP104);
                mWifiConfig.allowedPairwiseCiphers.set(
                    WifiConfiguration.PairwiseCipher.CCMP);
                mWifiConfig.allowedPairwiseCiphers.set(
                    WifiConfiguration.PairwiseCipher.TKIP);
                mWifiConfig.allowedProtocols.set(
                    WifiConfiguration.Protocol.RSN);
                mWifiConfig.allowedProtocols.set(
                    WifiConfiguration.Protocol.WPA);

                mWifiConfig.SSID = ap.getApName();

                if (capabilities.toUpperCase().contains("WPA") ||
                    capabilities.toUpperCase().contains("WPA2") ||
                    capabilities.toUpperCase().contains("WEP")) {

                    Log.d(APP_TAG,"WPA or WPA2 or WEP Network");
                    if (capabilities.toUpperCase().contains("WEP")) {
                        mWifiConfig.allowedAuthAlgorithms.set(
                            WifiConfiguration.AuthAlgorithm.OPEN);
                        mWifiConfig.allowedAuthAlgorithms.set(
                            WifiConfiguration.AuthAlgorithm.SHARED);
                        mWifiConfig.allowedKeyManagement.set(
                            WifiConfiguration.KeyMgmt.NONE);
                    } else {
                        mWifiConfig.allowedGroupCiphers.set(
                            WifiConfiguration.GroupCipher.CCMP);
                        mWifiConfig.allowedGroupCiphers.set(
                            WifiConfiguration.GroupCipher.TKIP);
                        mWifiConfig.allowedKeyManagement.set(
                            WifiConfiguration.KeyMgmt.WPA_PSK);
                    }
                    if (!(connectedApName.equals(selectedApName)) &&
                        ap.getApStatus().equals(context.getString(
                        R.string.saved))) {
                        //If the AP is already saved, Pop-up an Alert Dialog with "forget,cancel,connect"
                        //alert dialog
                        AlertDialog.Builder alertDialog =
                            new AlertDialog.Builder(WifiActivity.this);
                        alertDialog.setTitle(selectedApName);
                        alertDialog.setNeutralButton(R.string.forget,
                            new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                    int netId = findConfiguredNetworks(selectedApName);
                                    if(netId != -1){
                                        forgetSavedNetwork(ap,netId , selectedApName);
                                    }
                            }
                        })
                        .setCancelable(true)
                        .setPositiveButton(R.string.connect,
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    //Connect to chosen AP.Diconnect other APs
                                    int netId = findConfiguredNetworks(selectedApName);
                                    if(netId != -1){
                                            mWifiManager.disconnect();
                                            mWifiManager.enableNetwork(
                                                netId, true);
                                            getNewScanResults = false;
                                            getSupplicantStatus = true;

                                            updateApStatus(ap, context.getString(
                                                R.string.connecting));

                                            Toast.makeText(context, context.getString(
                                                R.string.connecting_to_ap) +
                                                selectedApName, Toast.LENGTH_SHORT).show();
                                        }
                                }
                        })
                        .setNegativeButton(R.string.cancel, null)
                        .create()
                        .show();
                        return;
                    } else {
                        //If AP is not saved earlier, Once password is provided, Connect to AP and disconnect other AP's
                        LayoutInflater mDialogInflater =
                        LayoutInflater.from(context);
                        View pwView = mDialogInflater.inflate(
                        R.layout.activity_alert_dialog, null);
                        AlertDialog.Builder mAlertDialogBuilder =
                            new AlertDialog.Builder(context);

                        //Set dialog message
                        mAlertDialogBuilder.setView(pwView);
                        mAlertDialogBuilder.setTitle(R.string.title);
                        final EditText mPassword = (EditText) pwView.findViewById(
                            R.id.alertDialog);
                        mPassword.setInputType(InputType.TYPE_CLASS_TEXT |
                            InputType.TYPE_TEXT_VARIATION_PASSWORD);
                        mPassword.setText("");
                        mAlertDialogBuilder
                        .setPositiveButton(R.string.connect,
                            new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                   int id) {
                                    mWifiConfig.preSharedKey = String.format(
                                        "\"%s\"", mPassword.getText()
                                            .toString());
                                    if (capabilities.toUpperCase()
                                        .contains("WEP")) {
                                        if (mPassword.length() != 0) {
                                            int length = mPassword.length();
                                            String password = mPassword.getText().toString();
                                            // WEP-40, WEP-104, and 256-bit WEP (WEP-232?)
                                            if ((length == 10 || length == 26 || length == 58)
                                                && password.matches("[0-9A-Fa-f]*")) {
                                                mWifiConfig.wepKeys[0] = password;
                                            }
                                            else{
                                                mWifiConfig.wepKeys[0] = '"' + password + '"';
                                            }
                                         }
                                     }
                                     //Connect to chosen AP.Diconnect other APs
                                     mWifiManager.disconnect();
                                     int networkId = mWifiManager.
                                         addNetwork(mWifiConfig);
                                     mWifiManager.saveConfiguration();
                                     mWifiManager.enableNetwork(
                                         networkId, true);
                                     getNewScanResults = false;
                                     getSupplicantStatus = true;

                                     updateApStatus(ap, context.getString(
                                         R.string.connecting));

                                     Toast.makeText(context, context.getString(
                                     R.string.connecting_to_ap) +
                                     selectedApName, Toast.LENGTH_SHORT).show();
                                }
                         })
                        .setNegativeButton(R.string.cancel, null);
                        final AlertDialog alertDialog = mAlertDialogBuilder.create();
                        alertDialog.show();
                        alertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);
                        mPassword.addTextChangedListener(new TextWatcher() {
                          @Override
                          public void onTextChanged(CharSequence s, int start, int before, int count) {
                          }

                          @Override
                          public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                          }

                          @Override
                          public void afterTextChanged(Editable s) {
                          if(s.length()< 8 )
                            alertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);
                          else
                            alertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(true);
                          }
                        });
                  }
                } else{
                    //Open Network
                    if(!(connectedApName.equals(selectedApName)) &&
                    ap.getApStatus().equals(context.getString(
                    R.string.saved))){
                        //If AP is already saved
                        AlertDialog.Builder alertDialog =
                            new AlertDialog.Builder(WifiActivity.this);
                        alertDialog.setTitle(selectedApName);
                        alertDialog.setNeutralButton(R.string.forget,
                            new DialogInterface.OnClickListener(){
                            public void onClick(DialogInterface dialog, int id){
                                    int netId = findConfiguredNetworks(selectedApName);
                                    if(netId != -1){
                                        forgetSavedNetwork(ap,netId , selectedApName);
                                    }
                            }
                        })
                        .setCancelable(true)
                        .setPositiveButton(R.string.connect,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                    int netId = findConfiguredNetworks(selectedApName);
                                    if(netId != -1){
                                        Toast.makeText(context, R.string.open_network,
                                            Toast.LENGTH_SHORT).show();
                                        mWifiConfig.allowedGroupCiphers.set(
                                            WifiConfiguration.GroupCipher.CCMP);
                                        mWifiConfig.allowedGroupCiphers.set(
                                            WifiConfiguration.GroupCipher.TKIP);
                                        mWifiConfig.allowedKeyManagement.set(
                                            WifiConfiguration.KeyMgmt.NONE);
                                        mWifiConfig.allowedAuthAlgorithms.clear();

                                        //Connect to chosen AP by disconnecting other APs
                                        mWifiManager.disconnect();
                                        mWifiManager.enableNetwork(netId, true);

                                        getNewScanResults = false;
                                        getSupplicantStatus = true;

                                        updateApStatus(ap, context.getString(R.string.connecting));

                                        Toast.makeText(context, context.getString(
                                            R.string.connecting_to_ap) + selectedApName,
                                            Toast.LENGTH_SHORT).show();
                                    }
                            }
                        })
                        .setNegativeButton(R.string.cancel, null)
                        .create()
                        .show();
                    return;
                    } else {
                        //Open Network, If not saved/connected earlier.
                        Toast.makeText(context, R.string.open_network,
                            Toast.LENGTH_SHORT).show();
                        mWifiConfig.allowedGroupCiphers.set(
                            WifiConfiguration.GroupCipher.CCMP);
                        mWifiConfig.allowedGroupCiphers.set(
                            WifiConfiguration.GroupCipher.TKIP);
                        mWifiConfig.allowedKeyManagement.set(
                            WifiConfiguration.KeyMgmt.NONE);
                        mWifiConfig.allowedAuthAlgorithms.clear();

                        //Connect to chosen AP by disconnecting other APs
                        mWifiManager.disconnect();
                        int networkId = mWifiManager.addNetwork(mWifiConfig);
                        mWifiManager.saveConfiguration();
                        mWifiManager.enableNetwork(networkId, true);

                        getNewScanResults = false;
                        getSupplicantStatus = true;

                        updateApStatus(ap, context.getString(R.string.connecting));

                        Toast.makeText(context, context.getString(
                            R.string.connecting_to_ap) + selectedApName,
                            Toast.LENGTH_SHORT).show();
                    }
                }
            }
        });
    }

    private int findConfiguredNetworks(String ssid){
    for (final WifiConfiguration w : mWifiManager.getConfiguredNetworks()) {
            if (w.SSID.equals(ssid)){
                return w.networkId;
            }
        }
        return -1;
    }

    private void forgetSavedNetwork(AcessPointInfo ap, int netID, String selectedApName){
        mWifiManager.disconnect();
        mWifiManager.disableNetwork(netID);
        mWifiManager.removeNetwork(netID);
        mWifiManager.saveConfiguration();

        updateApStatus(ap, context.getString(R.string.disconnecting));

        Toast.makeText(context, context.getString(R.string.disconnecting_ap) +
        selectedApName, Toast.LENGTH_SHORT).show();
    }

    private void forgetConnectedNetwork(AcessPointInfo ap, int netID, String connectedApName){
        mWifiManager.disconnect();
        mWifiManager.disableNetwork(netID);
        mWifiManager.removeNetwork(netID);
        mWifiManager.saveConfiguration();

        updateApStatus(ap, context.getString(R.string.disconnecting));

        Toast.makeText(context, context.getString(R.string.disconnecting_ap) +
        connectedApName, Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onResume() {
        super.onResume();
        wifiSwitch.setClickable(true);
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d(APP_TAG,"onStart : Registering receivers");
        getNewScanResults = true;
        getSupplicantStatus = false;

        //Register a broadcast receiver for scanning available APs
        IntentFilter mScanResultsAvailable =
            new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        registerReceiver(mScanResultsReceiver, mScanResultsAvailable);

        //Register a broadcast receiver for checking wifi state
        IntentFilter mWifiStateIntent = new IntentFilter(
            WifiManager.WIFI_STATE_CHANGED_ACTION);
        registerReceiver(mWifiStateReceiver, mWifiStateIntent);

        //Register a broacast receiver for checking supplicant status
        IntentFilter mSupplicantStateIntent = new IntentFilter(
            WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
        registerReceiver(mSupplicantStateReceiver, mSupplicantStateIntent);
    }

    //Unregister broadcast receiver while stopping activity
    @Override
    protected void onStop() {
        super.onStop();
        Log.d(APP_TAG,"onStop : Unregistering receivers");
        unregisterReceiver(mWifiStateReceiver);
        unregisterReceiver(mScanResultsReceiver);
        unregisterReceiver(mSupplicantStateReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mWifiProgressDialog = null;
    }

    //Set switch state as ON or OFF based on wifi state
    private BroadcastReceiver mWifiStateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context wifiState_context,
            Intent wifiState_intent) {

            String action = wifiState_intent.getAction();
            int wifiState = wifiState_intent.getIntExtra(
                WifiManager.EXTRA_WIFI_STATE, 0);

            switch(wifiState) {
                case WifiManager.WIFI_STATE_DISABLING:
                    Log.v(APP_TAG,"Wifi state : DISABLING");
                    break;
                case WifiManager.WIFI_STATE_DISABLED:
                    dismissProgressDialog();
                    //wifiSwitch.setChecked(false);
                    wifiStatus.setText(R.string.wifi_switch_state_off);
                    wifiSwitch.setClickable(true);
                    Log.v(APP_TAG,"Wifi state : DISABLED");
                    break;
                case WifiManager.WIFI_STATE_ENABLING:
                    Log.v(APP_TAG,"Wifi state : ENABLING");
                    break;
                case WifiManager.WIFI_STATE_ENABLED:
                    //wifiSwitch.setChecked(true);
                    wifiStatus.setText(R.string.wifi_switch_state_on);

                    Log.v(APP_TAG,"Wifi state : ENABLED");
                    getNewScanResults = true;
                    mWifiManager.startScan();
                    wifiSwitch.setClickable(true);
                    mWifiProgressDialog.show();

                    Log.v(APP_TAG,"Started scan for available APs");
                    break;
                case WifiManager.WIFI_STATE_UNKNOWN:
                    //wifiSwitch.setChecked(false);
                    wifiStatus.setText(R.string.wifi_switch_state_off);
                    wifiSwitch.setClickable(true);
                    Log.v(APP_TAG,"Wifi state : UNKNOWN");
                    break;
                default:
                    Log.e(APP_TAG, "Wifi state : BAD");
                    break;
            }
        }
    };

    private void dismissProgressDialog() {
        if (mWifiProgressDialog.isShowing()) {
            mWifiProgressDialog.dismiss();
        }
    }

    Comparator<ScanResult> comparator = new Comparator<ScanResult>() {
        @Override
        public int compare(ScanResult lhs, ScanResult rhs) {
            return (lhs.level <rhs.level ? -1 : (lhs.level==rhs.level ? 0 : 1));
        }
    };

    //Populate listview adapter with all APs info
    private BroadcastReceiver mScanResultsReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context scan_context, Intent scan_intent) {

                if (getNewScanResults == true) {

                List<ScanResult> wifiScanList = mWifiManager.getScanResults();
                HashMap<String, Integer> accessPointMap =
                    new HashMap<String, Integer>();
                //Clear listview before populating it with latest scan results
                if (!apInfoList.isEmpty()) {
                    apInfoList.clear();
                }
                Collections.sort(wifiScanList, comparator);

                int pos_key=0;

                for (int i = 0; i < wifiScanList.size(); i++) {
                    ScanResult AccessPoint = wifiScanList.get(i);
                    if (!AccessPoint.SSID.isEmpty()) {

                        String key = AccessPoint.SSID;
                        //Use a Hasmap to find out unique SSIDs
                        if(!accessPointMap.containsKey(key)){
                            accessPointMap.put(key, pos_key++);
                            //Convert level info to signal strength;range:0-100%
                            int strength = calculateSignalLevel(
                            wifiScanList.get(i).level, MAX_SIGNAL_LEVEL);

                            //Populate AP scan result in list
                            AcessPointInfo accessPointInfo =
                                new AcessPointInfo();
                            accessPointInfo.setApName(AccessPoint.SSID);
                            accessPointInfo.setApStrength(strength);
                            accessPointInfo.setApCapabilities(
                                AccessPoint.capabilities);
                            String connectedAp =
                                mWifiManager.getConnectionInfo().getSSID();
                            String apName = String.format("\"%s\"",
                                accessPointInfo.getApName());
                            if(connectedAp.equals(apName)) {
                                accessPointInfo.setApStatus(context.getString(
                                    R.string.connected));
                            } else {
                                int netId = findConfiguredNetworks(apName);
                                if(netId != -1){
                                    accessPointInfo.setApStatus(context.getString(
                                        R.string.saved));
                                }
                            }
                            apInfoList.add(accessPointInfo);
                            apScanArrayAdapter.notifyDataSetChanged();
                        }
                    }
                }
                for (int i=0; i < apInfoList.size(); i++) {
                    String apName = String.format("\"%s\"", apInfoList.get(i).getApName());
                    AcessPointInfo ap = apInfoList.get(i);
                    String connectedAp =
                    mWifiManager.getConnectionInfo().getSSID();
                    if(connectedAp.equals(apName)){
                        int pos_index = apInfoList.indexOf(ap);
                        if(pos_index != 0){
                            AcessPointInfo apInfoAtZero = apInfoList.get(0);
                            apInfoList.set(0,ap);
                            apInfoList.set(pos_index,apInfoAtZero);
                        }
                    }
                }
                apScanArrayAdapter.notifyDataSetChanged();
            }

            dismissProgressDialog();
        }
    };

    //Get supplicant state to know if connection to AP is successful or not
    private BroadcastReceiver mSupplicantStateReceiver =
        new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            String action  = intent.getAction();
            if(action.equals(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION)){
                SupplicantState supplicantState = ((SupplicantState)intent
                   .getParcelableExtra(WifiManager.EXTRA_NEW_STATE));
                if (supplicantState.equals(SupplicantState.COMPLETED)) {
                    String connectedAp = mWifiManager.getConnectionInfo().getSSID();

                    for (int i=0; i < apInfoList.size(); i++) {
                        String apName = String.format("\"%s\"", apInfoList.get(i).getApName());
                        AcessPointInfo ap = apInfoList.get(i);
                        int netId = findConfiguredNetworks(apName);
                        if (apName.contains(connectedAp)) {
                            updateApStatus(ap, context.getString(
                                R.string.connected));
                            int pos_index = apInfoList.indexOf(ap);
                            if(pos_index != 0){
                                AcessPointInfo apInfoAtZero = apInfoList.get(0);
                                apInfoList.set(0,ap);
                                apInfoList.set(pos_index,apInfoAtZero);
                                apScanArrayAdapter.notifyDataSetChanged();
                            }
                        } else if(netId != -1) {
                            updateApStatus(ap, context.getString(
                                    R.string.saved));
                        }
                        else if(ap.getApStatus() != context.getString(
                                R.string.connecting)){
                                updateApStatus(ap, context.getString(
                                    R.string.disconnected));
                        }
                    }
                    getNewScanResults = true;

                }

                boolean supplicant_has_error = intent.hasExtra(
                    WifiManager.EXTRA_SUPPLICANT_ERROR);

                if (supplicant_has_error && getSupplicantStatus) {

                    for (int i=0; i < apInfoList.size(); i++) {

                        String apName = String.format("\"%s\"", apInfoList.get(i).getApName());
                        AcessPointInfo ap = apInfoList.get(i);

                        if (ap.getApStatus().equals(context.getString(
                            R.string.connecting)))  {
                            int supplicant_error = intent.getIntExtra(
                                WifiManager.EXTRA_SUPPLICANT_ERROR, -1);
                                if (supplicant_error == WifiManager.
                                    ERROR_AUTHENTICATING) {

                                        Toast.makeText(context, R.string.
                                        supplicant_error, Toast.LENGTH_SHORT).show();

                                        updateApStatus(ap, context.getString(
                                        R.string.disconnected));

                                        Log.d(APP_TAG, "AUTHENTICATION ERROR : " +
                                        supplicant_error);

                                    break;
                            }
                        }
                    }

                    getSupplicantStatus = false;
                }
            }
        }
    };

    private void updateApStatus(AcessPointInfo ap, String status) {
        ap.setApStatus(status);
        apScanArrayAdapter.notifyDataSetChanged();
    }

    class AcessPointInfo {
        private String apName;
        private int apStrength;
        private String apStatus;
        private String apCapabilities;

        AcessPointInfo() {
            this.apStatus = context.getString(R.string.disconnected);
        }

        public void setApName(String name) {
            this.apName = name;
        }

        public String getApName() {
            return apName;
        }

        public void setApStrength(int strength) {
            this.apStrength = strength;
        }

        public int getApStrength() {
            return apStrength;
        }

        public void setApStatus(String status) {
            this.apStatus = status;
        }

        public String getApStatus() {
            return apStatus;
        }

        public void setApCapabilities(String capabilities) {
            this.apCapabilities = capabilities;
        }

        public String getApCapabilities() {
            return apCapabilities;
        }
    }
}
