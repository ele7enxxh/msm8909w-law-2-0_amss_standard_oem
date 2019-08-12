/******************************************************************************
  @file    RadioConfigTestActivity.java

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfigtest;

import android.app.Activity;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ProgressBar;
import android.os.AsyncTask;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.util.Log;
import java.util.List;
import java.util.ArrayList;
import com.qualcomm.qti.radioconfiginterface.RadioConfig;
import com.qualcomm.qti.radioconfiginterface.RadioConfigItem;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Map;
import java.util.TreeMap;

public class RadioConfigTestActivity extends Activity {

    private static RadioConfig radioConfig;
    private static Context context;
    private static String configDetails;
    private static final String LOG_TAG = "RadioConfigTest";
    private static final int RADIO_CONFIG_TEST_SERVICE_CONNECTION_STATUS = 1;

    private Handler serviceStatusHandler = new Handler() {
        public void handleMessage (Message msg) {
            Log.i(LOG_TAG, "serviceStatusHandler: handleMessage()");
            //testHandler

            try {
                if(msg.what == RADIO_CONFIG_TEST_SERVICE_CONNECTION_STATUS){
                    Boolean status = (Boolean)msg.obj;
                    makeToast("Service " + (status ? "Connected" : "Disconnected"));
                    Log.d(LOG_TAG, "Service " + (status ? "Connected" : "Disconnected"));
                } else {
                    Log.e(LOG_TAG, "what is different that esxpected in the msg: " + msg.what);
                }
            } catch (Exception e) {
                Log.e(LOG_TAG, "error occured in the serviceStatusHandler!");
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        context = this;
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateProgressBar(false);
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(
                    new InputStreamReader(getAssets().open("ConfigDetails")));
            StringBuilder stringBuilder = new StringBuilder();
            String line;
            while((line = reader.readLine())!= null){
                stringBuilder.append(line + "\n");
            }
            reader.close();

            StringBuilder sb2 = new StringBuilder();
            for(int i = 0; i < stringBuilder.length(); i++){
               while(stringBuilder.charAt(i) == ' '){
                   i++; //skip white spaces
               }
               if( (stringBuilder.charAt(i) == '/' &&
                     (stringBuilder.charAt(i+1) == '*' || stringBuilder.charAt(i+1) == '/'))
                    || stringBuilder.charAt(i) == '*'){
                   //skip the line
                   while(stringBuilder.charAt(i) != '\n'){
                       i++;
                   }
               }
               else{
                   while(stringBuilder.charAt(i) != '\n'){
                       sb2.append(stringBuilder.charAt(i));
                       i++;
                   }
                   sb2.append('\n');
               }
            }
            configDetails = sb2.toString();
            TextView textView = (TextView) findViewById(R.id.itemDetailsDisplay);
            textView.setText(configDetails);
        }catch(Exception e){
            e.printStackTrace();
        }
        Log.e(LOG_TAG, "created the object!");
        radioConfig = new RadioConfig(this);
        Log.e(LOG_TAG, "Starting the config service!!");
        radioConfig.connectService(serviceStatusHandler, RADIO_CONFIG_TEST_SERVICE_CONNECTION_STATUS);
    }

    //on Set button click listener
    public void setConfigOnClick(View view) {
        EditText idText = (EditText) findViewById(R.id.itemId);
        EditText valText = (EditText) findViewById(R.id.itemValue);

        String ids = idText.getText().toString();
        String vals = valText.getText().toString();

        //turn of keyboard display
        InputMethodManager mgr = (InputMethodManager) getSystemService(this.INPUT_METHOD_SERVICE);
        mgr.hideSoftInputFromWindow(idText.getWindowToken(), 0);
        mgr.hideSoftInputFromWindow(valText.getWindowToken(), 0);

        if(ids == null || ids.equals("")){
            Toast.makeText(this, "null input!", Toast.LENGTH_LONG).show();
            return;

        }
        if(vals == null || vals.equals("")){
            Toast.makeText(this, "null input!", Toast.LENGTH_LONG).show();
            return;
        }
        String[] idArray = ids.split(",");
        String[] valArray = vals.split(",");


        Toast.makeText(this, "setConfig", Toast.LENGTH_LONG).show();

        if(idArray == null || idArray.length <= 0) {
            Toast.makeText(this, "enter item ids!", Toast.LENGTH_LONG).show();
            return;
        }
        if(valArray == null || valArray.length <= 0) {
            Toast.makeText(this, "enter item ids!", Toast.LENGTH_LONG).show();
            return;
        }
        if (idArray.length > valArray.length) {
            Toast.makeText(this, "num of ids are more than values!! Re-Enter!", Toast.LENGTH_LONG).show();
            return;
        }

        if (idArray.length > valArray.length) {
            Toast.makeText(this, "num of ids are more than values!! Re-Enter!", Toast.LENGTH_LONG).show();
        }
        List<RadioConfigItem<Integer>> input = new ArrayList<RadioConfigItem<Integer>>();
        RadioConfigItem<Integer> item;
        for (int i = 0; i < idArray.length; i++) {
            item = new RadioConfigItem<Integer>();
            item.setItem(Integer.parseInt(idArray[i]));
            item.setValue(Integer.parseInt(valArray[i]));
            input.add(item);
        }

        RadioConfigRequestAsyncHelper reqTask = new RadioConfigRequestAsyncHelper();
        reqTask.execute(input);
    }

    //on Get button click listener
    public void getConfigOnClick(View view){

        EditText idText = (EditText) findViewById(R.id.itemId);
        EditText valText = (EditText) findViewById(R.id.itemValue);

        String idVal = "GET: "+ idText.getText().toString() + " : " + valText.getText().toString();

        Toast.makeText(this, idVal, Toast.LENGTH_LONG).show();

        //turn of keyboard display
        InputMethodManager inputMethodManager = (InputMethodManager) getSystemService(this.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(idText.getWindowToken(), 0);
        inputMethodManager.hideSoftInputFromWindow(valText.getWindowToken(), 0);
    }

    //on refresh button click listener

    public void refreshOnClick(View view){

        TextView textView = (TextView) findViewById(R.id.itemDetailsDisplay);
        EditText idText = (EditText) findViewById(R.id.itemId);
        EditText valText = (EditText) findViewById(R.id.itemValue);

        idText.setText("");
        valText.setText("");
        textView.setText(configDetails);
    }

    private void updateProgressBar(boolean visible){
        ProgressBar progressBar = (ProgressBar) findViewById(R.id.progressBar);
        progressBar.setVisibility(visible? View.VISIBLE : View.INVISIBLE);
    }

    public void makeToast(String text){
        Log.d(LOG_TAG, "makeToast");
        Toast.makeText(context, text, Toast.LENGTH_LONG).show();
    }

    private void updateResponse(List<RadioConfigItem<Integer>> resp){
        TextView textView = (TextView) findViewById(R.id.itemDetailsDisplay);

        StringBuilder sb = new StringBuilder();
        sb.append("Click Refresh button to reset!");
        sb.append("\n Result: \n");

        for(RadioConfigItem<Integer> item : resp) {
            sb.append("Item: ");
            sb.append(mapItemidToString(item.getItem()));
            sb.append("   Result: ");
            sb.append(mapErrorToString(item.getError()));
            sb.append("\n---------\n");
        }
        textView.setText(sb.toString());
    }

    private class RadioConfigRequestAsyncHelper extends AsyncTask<List<RadioConfigItem<Integer>>, String, List<RadioConfigItem<Integer>>> {

        @Override
        protected void onPreExecute() {
            makeToast("Sending Request!");
            updateProgressBar(true);
        }

        @Override
        protected List<RadioConfigItem<Integer>> doInBackground(List<RadioConfigItem<Integer>>... params) {
            //Here we assume that there is only one params
            //which is the uri of the server.

            Log.i(LOG_TAG, "background thread!");
            List<RadioConfigItem<Integer>> resp = null;
            if(radioConfig.isServiceConnected()) {
                Log.d(LOG_TAG, "sending req in background!");
                resp = radioConfig.setIntConfig(params[0], 0);
            }

            return resp;
        }

        @Override
        protected void onPostExecute(List<RadioConfigItem<Integer>> result) {
            updateResponse(result);
            updateProgressBar(false);
        }
    }

    private String mapItemidToString(int itemid){
        String ret = "Invalid item id " + itemid;

        switch(itemid){
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_LOW:
            ret = "IN_CALL_LTE_RSRP_LOW";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_MID:
            ret ="IN_CALL_LTE_RSRP_MID";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_HIGH:
            ret ="IN_CALL_LTE_RSRP_HIGH";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_LOW:
            ret ="IN_CALL_WIFI_RSSI_THRESHOLD_LOW";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH:
            ret = "IN_CALL_WIFI_RSSI_THRESHOLD_HIGH";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_LOW:
            ret ="IN_CALL_WIFI_SINR_THRESHOLD_LOW";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_HIGH:
            ret ="IN_CALL_WIFI_SINR_THRESHOLD_HIGH";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_LOW:
            ret ="IDLE_LTE_RSRP_LOW";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_MID:
            ret ="IDLE_LTE_RSRP_MID";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_HIGH:
            ret ="IDLE_LTE_RSRP_HIGH";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_LOW:
            ret ="IDLE_WIFI_RSSI_THRESHOLD_LOW";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_HIGH:
            ret ="IDLE_WIFI_RSSI_THRESHOLD_HIGH";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_LOW:
            ret ="IDLE_WIFI_SINR_THRESHOLD_LOW";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_HIGH:
            ret ="IDLE_WIFI_SINR_THRESHOLD_HIGH";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_ECIO_1X_THRESHOLD_LOW:
            ret ="ECIO_1X_THRESHOLD_LOW";
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_ECIO_1X_THRESHOLD_HIGH:
            ret ="ECIO_1X_THRESHOLD_HIGH";
            break;
        }
        return ret;
    }

    private String mapErrorToString(int error){
        String ret = "Generic Failure";

        switch(error) {
        case RadioConfig.ErrorConstants.RADIO_CONFIG_SUCCESS:
            ret = "Successful!";
            break;
        default:
            ret = "Generic Failure!";
            break;
        }
        return ret;
    }
}
