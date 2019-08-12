/*
 * Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.soundtrigger.ux;

import com.qualcomm.qti.soundtrigger.Global;


import android.content.Context;
import android.content.Intent;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Toast;
import android.os.SystemProperties;
import android.app.AlertDialog;

public class OptionsListClickListener implements AdapterView.OnItemClickListener{
	 //private final static String TAG = "ListenLog.OptionsListClickListener";
	 private final String[] listItemNames;
	 private final Context context;

	 public OptionsListClickListener(Context context, String[] values) {

	        this.context = context;
	        this.listItemNames = values;
	    }

	    @Override
	    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

	        Intent intent;

	        /*if (listItemNames[position].equals(context.getString(R.string.detection_menu_settings))) {
	        	//intent = new Intent(context, SettingsActivity.class);
	           // startActivity(intent);
	           return;
	        }

	        if (listItemNames[position].equals(context.getString(R.string.detection_menu_version))) {
	        	//openAlertDialog("Version Number", Global.getInstance().getVersionNumber());
	        	return;
	        }*/
	        	
	
    }
	    
	   
}
