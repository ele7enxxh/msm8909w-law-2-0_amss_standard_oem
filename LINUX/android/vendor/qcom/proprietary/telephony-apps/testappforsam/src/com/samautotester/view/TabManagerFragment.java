/******************************************************************************
 * @file    TabManagerFragment.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.view;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

public class TabManagerFragment extends Fragment {
    public static String FRAGMENT_ID = "fragmentNumber";
    private int mFragmentNumber;

    public static TabManagerFragment newInstance(int fragmentNumber) {
        Bundle args = new Bundle();
        args.putInt(FRAGMENT_ID, fragmentNumber);

        TabManagerFragment fragment = new TabManagerFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mFragmentNumber = getArguments().getInt(FRAGMENT_ID);
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater,
            @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return super.onCreateView(inflater, container, savedInstanceState);
    }

    @Override
    public void onResume() {
        super.onResume();
        // Toast.makeText(getContext(),"FragmentNo. "+mFragmentNumber,Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onPause() {
        super.onPause();
        Toast.makeText(getContext(), "FragmentNo. " + mFragmentNumber,
                Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Toast.makeText(getContext(),
                "Destroy - FragmentNo. " + mFragmentNumber, Toast.LENGTH_SHORT)
                .show();
    }
}
