/******************************************************************************
 * @file    MultiFunctionalityFragment.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.view;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.view.ViewPager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.samautotester.R;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MultiFunctionalityFragment extends Fragment implements
        View.OnClickListener {

    /*
     * private FragmentTabAdapter mPagerAdapter = null; private Fragment
     * mSlot1Fragment,mSlot2Fragment,mSlot3Fragment,mSlot4Fragment;
     */

    private WeakReference<View> dummy_slots = null;
    private WeakReference<View> dummy_switchslot = null;

    private enum ButtonClicked {
        SLOT_123, SWITCH_SLOT;
    };

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.multifunctionality_fragment,
                container, false);

        view.findViewById(R.id.btnSlot1).setOnClickListener(this);
        view.findViewById(R.id.btnSlot2).setOnClickListener(this);
        view.findViewById(R.id.btnSlot3).setOnClickListener(this);
        view.findViewById(R.id.btnSlot4).setOnClickListener(this);

        // ViewPager pager = (ViewPager)view.findViewById(R.id.viewPager);

        // mPagerAdapter = new FragmentTabAdapter(getFragmentManager());
        // pager.setAdapter(mPagerAdapter);

        return view;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    @Override
    public void onClick(View v) {
        ButtonClicked buttonClicked = null;

        switch (v.getId()) {
        case R.id.btnSlot1:
        case R.id.btnSlot2:
        case R.id.btnSlot3:
            buttonClicked = ButtonClicked.SLOT_123;
            break;
        case R.id.btnSlot4:
            buttonClicked = ButtonClicked.SWITCH_SLOT;
            break;
        }

        inflateChildViews(buttonClicked);
    }

    private void inflateChildViews(ButtonClicked buttonClicked) {

        ((FrameLayout) getView().findViewById(R.id.frameLayout))
                .removeAllViews();

        switch (buttonClicked) {
        case SLOT_123:

            if (dummy_slots == null || dummy_slots.get() == null)
                dummy_slots = new WeakReference<View>(getLayoutInflater(null)
                        .inflate(R.layout.slots_layout, null));

            // load the data

            View dummy_view1 = dummy_slots.get();
            if (dummy_view1.getParent() != null)
                ((ViewGroup) dummy_view1.getParent()).removeView(dummy_view1);

            ((FrameLayout) getView().findViewById(R.id.frameLayout))
                    .addView(dummy_slots.get());

            break;

        case SWITCH_SLOT:

            if (dummy_switchslot == null || dummy_switchslot.get() == null)
                dummy_switchslot = new WeakReference<View>(getLayoutInflater(
                        null).inflate(R.layout.switchslot_multi_layout, null));

            // load the data

            View dummy_view2 = dummy_switchslot.get();
            if (dummy_view2.getParent() != null)
                ((ViewGroup) dummy_view2.getParent()).removeView(dummy_view2);

            ((FrameLayout) getView().findViewById(R.id.frameLayout))
                    .addView(dummy_view2);

            break;

        }
    }
}
