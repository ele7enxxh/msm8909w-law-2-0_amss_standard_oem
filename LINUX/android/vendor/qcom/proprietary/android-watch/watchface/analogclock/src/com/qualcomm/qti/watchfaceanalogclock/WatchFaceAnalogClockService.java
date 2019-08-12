/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchfaceanalogclock;

import android.os.Bundle;
import android.os.Handler;
import android.service.wallpaper.WallpaperService;
import android.util.Log;
import android.view.SurfaceHolder;
import android.content.Intent;

public class WatchFaceAnalogClockService extends WallpaperService {
    private static final String SERVICE_TAG = "WatchFaceAnalogClock";

    @Override
    public Engine onCreateEngine() {
        return new WatchFaceEngine();
    }

    private class WatchFaceEngine extends Engine {
        private boolean mVisible = false;
        private final Handler mHandler = new Handler();

        private final Runnable mRefreshDisplay = new Runnable() {
            @Override
            public void run() {
                Log.i(SERVICE_TAG, "Place holder for invoking analog clock drawing");
            }
        };

        @Override
        public void onVisibilityChanged(boolean visible) {
            super.onVisibilityChanged(visible);
            Log.i(SERVICE_TAG, "onVisibilityChanged");

            mVisible = visible;

            /**
             * Watchface visibility has changed. Send the REQUEST_STATE
             * intent so that the home activity can send the appropriate
             * command to the wallpaper based on the current contextual
             * state.
             */
            Intent requestStateIntent = new Intent(
                    "com.google.android.wearable.watchfaces.action.REQUEST_STATE");

            WatchFaceAnalogClockService.this.sendBroadcast(requestStateIntent);

            if (visible) {
                Log.i(SERVICE_TAG, "Place holder for registering receiver and drawing");
            } else {
                Log.i(SERVICE_TAG, "Place holder for unregistering receiver");
                mHandler.removeCallbacks(mRefreshDisplay);
            }
        }

        @Override
        public void onSurfaceChanged(SurfaceHolder holder, int format,
            int width, int height) {
            super.onSurfaceChanged(holder, format, width, height);
            Log.i(SERVICE_TAG, "onSurfaceChanged");
        }

        @Override
        public void onSurfaceDestroyed(SurfaceHolder holder) {
            super.onSurfaceDestroyed(holder);
            Log.i(SERVICE_TAG, "onSurfaceDestroyed");
            mVisible = false;
            mHandler.removeCallbacks(mRefreshDisplay);
        }

        @Override
        public void onDestroy() {
            super.onDestroy();
            Log.i(SERVICE_TAG, "onDestroy");
            mVisible = false;
            mHandler.removeCallbacks(mRefreshDisplay);
        }

        @Override
        public Bundle onCommand(String action, int x, int y, int z,
            Bundle extras, boolean resultRequested) {
            super.onCommand(action, x, y, z, extras, resultRequested);

            if (action.matches(
                "com.google.android.wearable.action.BACKGROUND_ACTION")) {
                boolean ambient_mode = extras.getBoolean(
                    "ambient_mode", false);
                //Redraw analog clock in black and white during ambient mode
                if (ambient_mode) {
                    Log.i(SERVICE_TAG, "onCommand. Entering ambient mode");
                } else { //Redraw analog clock in green during non-ambient mode
                    Log.i(SERVICE_TAG, "onCommand. Exiting ambient mode");
                }
            }
            //Update and redraw analog clock every minute
            else if (action.matches("com.google.android.wearable.action.AMBIENT_UPDATE")) {
                Log.i(SERVICE_TAG, "onCommand. Updating in ambient mode");
            }
            return extras;
        }
    }
}
