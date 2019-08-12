/******************************************************************************
 * @file    MainActivity.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.view;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.AdapterView;
import android.widget.CompoundButton;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;
import android.util.Log;

import com.samautotester.R;
import com.samautotester.controller.IndivTask;
import com.samautotester.controller.SAMExecutor;
import com.samautotester.controller.TestFunc;

public class MainActivity extends AppCompatActivity {

    private static final String LOG_TAG = MainActivity.class.getName();
    private IndivFunctionalityFragment mIndivFunctionalityFragment = null;
    private MultiFunctionalityFragment mMultiFunctionalityFragment = null;
    private AutomatedFunctionalityFragment mAutomatedFunctionalityFragment = null;

    private Handler mServiceCallBackHandler = new Handler() {
        @Override
        public void dispatchMessage(Message msg) {
            super.dispatchMessage(msg);

            Log.d(LOG_TAG, "mServiceCallBackHandler - what msg - " + msg.what);

            switch (msg.what) {

            case 1:
                // once the service is connected.. register for sam
                // unsolicitedCallback
                IndivTask registerCallBack_task = (SAMExecutor.INSTANCE
                        .registerCallback(TestFunc.INDIVIDUAL, null));
                registerCallBack_task.uihandler(new IndivTask.IHandler() {
                    @Override
                    public void refreshUI() {
                        int returnValue = TestFunc.INDIVIDUAL
                                .get_RegisteredCallBack_returnValue();
                        Toast.makeText(
                                MainActivity.this.getApplicationContext(),
                                "RegisteredCallBack - returnValue = "
                                        + returnValue, Toast.LENGTH_SHORT)
                                .show();
                    }
                });

                registerCallBack_task.execute();

                break;

            default:
                // something went wrong...
                break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ((Switch) findViewById(R.id.serviceToggle))
                .setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                    @Override
                    public void onCheckedChanged(CompoundButton buttonView,
                            boolean isChecked) {
                        if (isChecked) {

                            // connect SAM service
                            IndivTask connectService_task = SAMExecutor.INSTANCE
                                    .connect(TestFunc.INDIVIDUAL,
                                            MainActivity.this
                                                    .getApplicationContext(),
                                            mServiceCallBackHandler, 1, null);
                            connectService_task
                                    .uihandler(new IndivTask.IHandler() {
                                        @Override
                                        public void refreshUI() {
                                            boolean connected = TestFunc.INDIVIDUAL
                                                    .get_ConnectService_returnValue();

                                            if (connected) {
                                                (findViewById(R.id.spinnerTestMode))
                                                        .setVisibility(View.VISIBLE);
                                                (findViewById(R.id.dummyLine))
                                                        .setVisibility(View.VISIBLE);
                                                (findViewById(R.id.fragmentContainer))
                                                        .setVisibility(View.VISIBLE);
                                                Toast.makeText(
                                                        MainActivity.this
                                                                .getApplicationContext(),
                                                        MainActivity.this
                                                                .getResources()
                                                                .getString(
                                                                        R.string.sam_service_connected),
                                                        Toast.LENGTH_SHORT)
                                                        .show();
                                            } else {
                                                Toast.makeText(
                                                        MainActivity.this
                                                                .getApplicationContext(),
                                                        MainActivity.this
                                                                .getResources()
                                                                .getString(
                                                                        R.string.sam_service_unable_to_connect),
                                                        Toast.LENGTH_SHORT)
                                                        .show();
                                            }
                                        }
                                    });

                            connectService_task.execute();

                        } else {
                            // disconnect SAM service
                            IndivTask disconnectService_task = SAMExecutor.INSTANCE
                                    .disconnectService(TestFunc.INDIVIDUAL,
                                            null);
                            disconnectService_task
                                    .uihandler(new IndivTask.IHandler() {
                                        @Override
                                        public void refreshUI() {

                                            (findViewById(R.id.spinnerTestMode))
                                                    .setVisibility(View.GONE);
                                            (findViewById(R.id.dummyLine))
                                                    .setVisibility(View.GONE);
                                            (findViewById(R.id.fragmentContainer))
                                                    .setVisibility(View.GONE);

                                            Toast.makeText(
                                                    MainActivity.this
                                                            .getApplicationContext(),
                                                    MainActivity.this
                                                            .getResources()
                                                            .getString(
                                                                    R.string.sam_service_disconnected),
                                                    Toast.LENGTH_SHORT).show();
                                        }
                                    });
                            disconnectService_task.execute();
                        }
                    }
                });

        ((Spinner) findViewById(R.id.spinnerTestMode))
                .setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                    Fragment dummyFragment = new Fragment();

                    @Override
                    public void onItemSelected(AdapterView<?> parent,
                            View view, int position, long id) {
                        FragmentTransaction transaction = getSupportFragmentManager()
                                .beginTransaction();
                        switch (position) {
                        case 0:
                            transaction.replace(R.id.fragmentContainer,
                                    dummyFragment);
                            transaction.commitAllowingStateLoss();
                            break;

                        case 1:
                            if (mIndivFunctionalityFragment == null)
                                mIndivFunctionalityFragment = new IndivFunctionalityFragment();

                            transaction.replace(R.id.fragmentContainer,
                                    mIndivFunctionalityFragment);
                            transaction.commit();

                            break;

                        case 2:
                            if (mAutomatedFunctionalityFragment == null)
                                mAutomatedFunctionalityFragment = new AutomatedFunctionalityFragment();

                            transaction.replace(R.id.fragmentContainer,
                                    mAutomatedFunctionalityFragment);
                            transaction.commit();
                            break;

                        case 3:
                            if (mMultiFunctionalityFragment == null)
                                mMultiFunctionalityFragment = new MultiFunctionalityFragment();

                            transaction.replace(R.id.fragmentContainer,
                                    mMultiFunctionalityFragment);
                            transaction.commit();
                            break;

                        }
                    }

                    @Override
                    public void onNothingSelected(AdapterView<?> parent) {

                    }
                });

    }

    @Override
    protected void onResume() {
        super.onResume();

    }
}
