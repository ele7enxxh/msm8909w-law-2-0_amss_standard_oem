/******************************************************************************
 * @file    AutomatedFunctionalityFragment.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.view;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.sax.StartElementListener;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.view.ViewPager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebView.FindListener;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.support.v4.content.ContextCompat;

import com.samautotester.R;
import com.samautotester.controller.BackgroundService;
import com.samautotester.controller.MyReceiver;
import com.samautotester.controller.MyReceiver.Receiver;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class AutomatedFunctionalityFragment extends Fragment implements
        View.OnClickListener, Receiver {
    private MyReceiver receiver;
    private Intent serviceIntent;
    public static final String RECEIVER_TAG = "reciever-tag";
    public static final int FILE_NOT_AVAILABLE = 0;
    public static final int SUCCESSFUL_CODE = 1;
    public static final int UNABLE_TO_PARSE_FILE = 2;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.automated_testscript_fragment,
                container, false);
        view.findViewById(R.id.start_stop_button).setOnClickListener(this);

        return view;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        receiver = new MyReceiver(new Handler());
        receiver.setReceiver(this);

        // TODO start the task here .....
        serviceIntent = new Intent(getActivity(), BackgroundService.class);
        serviceIntent.putExtra(RECEIVER_TAG, receiver);

        int permissionCheck = ContextCompat.checkSelfPermission(getActivity(),
                Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (permissionCheck == PackageManager.PERMISSION_GRANTED) {
            Log.d(BackgroundService.class.getSimpleName(),
                    "Permission Granted... ");
        } else {
            Log.d(BackgroundService.class.getSimpleName(),
                    "Permission Denied... ");

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                getActivity().requestPermissions(
                        new String[] {
                                Manifest.permission.READ_EXTERNAL_STORAGE,
                                Manifest.permission.WRITE_EXTERNAL_STORAGE },
                        11);
            }

            // request for permission...
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
        case R.id.start_stop_button:
            if (((TextView) v).getText().equals(
                    getActivity().getResources().getString(R.string.start))) {
                ((TextView) v).setText(getActivity().getResources().getString(
                        R.string.stop));
                (getActivity().findViewById(R.id.automatedScriptProgressBar))
                        .setVisibility(View.VISIBLE);
                (getActivity().findViewById(R.id.txtMessage))
                        .setVisibility(View.INVISIBLE);

                // start the task....
                BackgroundService.shouldContinue = true;
                getActivity().startService(serviceIntent);

            } else {
                ((TextView) v).setText(getActivity().getResources().getString(
                        R.string.start));
                (getActivity().findViewById(R.id.automatedScriptProgressBar))
                        .setVisibility(View.INVISIBLE);
                (getActivity().findViewById(R.id.txtMessage))
                        .setVisibility(View.VISIBLE);

                // TODO stop the task here .....
                BackgroundService.shouldContinue = false;
            }

            break;
        }
    }

    @Override
    public void onReceiveResult(int resultCode, Bundle resultData) {
        (getActivity().findViewById(R.id.automatedScriptProgressBar))
                .setVisibility(View.INVISIBLE);
        (getActivity().findViewById(R.id.txtMessage))
                .setVisibility(View.VISIBLE);
        ((TextView) (getActivity().findViewById(R.id.start_stop_button)))
                .setText(getActivity().getResources().getString(R.string.start));

        switch (resultCode) {
        case FILE_NOT_AVAILABLE:
            ((TextView) (getActivity().findViewById(R.id.txtMessage)))
                    .setText("'input_sam.txt' is not available at path '"
                            + Environment.getExternalStorageDirectory()
                                    .getAbsolutePath() + "'");
            break;
        case SUCCESSFUL_CODE:
            ((TextView) (getActivity().findViewById(R.id.txtMessage)))
                    .setText("Successfully generated 'output.txt' at '"
                            + Environment.getExternalStorageDirectory()
                                    .getAbsolutePath() + "'");
            break;
        case UNABLE_TO_PARSE_FILE:
            ((TextView) (getActivity().findViewById(R.id.txtMessage)))
                    .setText("Unable to Parse input file : "
                            + resultData.getSerializable("message"));
            break;
        default:
            ((TextView) (getActivity().findViewById(R.id.txtMessage)))
                    .setText("No resultCode.. "
                            + resultData.getSerializable("message"));
            break;
        }

        // ((TextView)(getActivity().findViewById(R.id.txtMessage))).setText("I have received it just now ... ");
        // Toast.makeText(getActivity().getApplicationContext(), "Received... ",
        // Toast.LENGTH_SHORT).show();
    }

}
