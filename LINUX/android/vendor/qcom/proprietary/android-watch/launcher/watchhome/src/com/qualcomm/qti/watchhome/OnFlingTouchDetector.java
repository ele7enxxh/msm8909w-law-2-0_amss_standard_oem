/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchhome;

import android.content.Context;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;

public class OnFlingTouchDetector extends
    GestureDetector.SimpleOnGestureListener {

    private static final String GESTURE_TAG = "GestureDetector";

    //Set velocity threshold as 2000. Max velocity=8000. Min velocity=500
    private static final int FLING_VELOCITY_THRESHOLD = 2000;

    // Left swipe only if swiped distance is greater than 1/3rd of layout width
    private static int FLING_DISTANCE_THRESHOLD;

    //Interface with home activity
    interface FlingGestureListenerInterface {
        public abstract void goToLauncher();
        public abstract void goToWallPaperChooser();
    }

    FlingGestureListenerInterface cb;

    public OnFlingTouchDetector(Context context,
        FlingGestureListenerInterface cb, int width, int height) {
        this.cb = cb;
        FLING_DISTANCE_THRESHOLD = (width/3);
    }

    @Override
    public boolean onDown(MotionEvent e) {
        return true;
    }

    @Override
    public void onLongPress(MotionEvent e) {
        Log.d(GESTURE_TAG, "Long press. Launch the custom launcher");
        cb.goToLauncher();
    }

    protected void onFlingLeft() {
        Log.d(GESTURE_TAG, "Left fling. Launch wallpaper menu");
        cb.goToWallPaperChooser();
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2,
        float velocityX, float velocityY) {

        boolean result = false;
        try {
            float distanceX = e2.getX() - e1.getX();
            float distanceY = e2.getY() - e1.getY();

            if (Math.abs(distanceX) > Math.abs(distanceY) &&
                Math.abs(distanceX) > FLING_DISTANCE_THRESHOLD &&
                    Math.abs(velocityX) > FLING_VELOCITY_THRESHOLD) {
                        if (distanceX < 0) {
                            onFlingLeft();
                        }
            result = true;
            }
        } catch (Exception mException) {
            mException.printStackTrace();
        }
        return result;
    }
}
