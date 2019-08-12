/******************************************************************************
 * @file    FragmentTabAdapter.java
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
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;

import java.util.Arrays;
import java.util.List;

public class FragmentTabAdapter extends FragmentStatePagerAdapter {

    List<String> mTitles = Arrays.asList("Slot-1", "Slot-2", "Slot-3",
            "Switch-Slot");

    public FragmentTabAdapter(FragmentManager fm) {
        super(fm);
    }

    @Override
    public Fragment getItem(int position) {
        /*
         * switch (position){ case 0: return new Slot1Fragment(); }
         */
        return TabManagerFragment.newInstance(position);
        // return Fragment f = (Fragment) getAdapter().instantiateItem(this,
        // getCurrentItem());
    }

    @Override
    public int getCount() {
        return mTitles.size();
    }

    @Override
    public CharSequence getPageTitle(int position) {
        return mTitles.get(position);
    }
}
