/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchfaceanalogdigitalclock;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.Handler;
import android.service.wallpaper.WallpaperService;
import android.util.Log;
import android.view.SurfaceHolder;

import java.text.SimpleDateFormat;
import java.util.Calendar;


public class WatchFaceAnalogDigitalClockService extends WallpaperService {
    private static final String SERVICE_TAG = "watchfaceanalogdigitalclock";

    @Override
    public Engine onCreateEngine() {
        return new WatchFaceEngine();
    }

    private class WatchFaceEngine extends Engine {
        //Digital clock color in interactive mode
        private int color = Color.GREEN;
        private Paint paint = new Paint();
        private boolean mVisible = false;
        private static final String TIME_FORMAT_12_HOUR = "hh:mm a";
        private boolean mRegisteredTimeTickReceiver = false;

        private final Handler mHandler = new Handler();

        private final Runnable mRefreshDisplay = new Runnable() {
            @Override
            public void run() {
                draw(color);
            }
        };

        private void draw(int color) {
            SurfaceHolder mSurfaceHolder = getSurfaceHolder();
            Canvas mCanvas = null;

            try {
                mCanvas = mSurfaceHolder.lockCanvas();
                if (mCanvas != null) {
                    //Set text size for digital clock
                    paint.setTextSize(30);
                    paint.setAntiAlias(true);

                    //Get current system time
                    String text = new SimpleDateFormat(TIME_FORMAT_12_HOUR)
                            .format(Calendar.getInstance().getTime());

                    //Get x and y co-ordinates for placing digital clock
                    float w = paint.measureText(text, 0, text.length());
                    int offset = (int) w / 2;
                    int x = mCanvas.getWidth() / 2 - offset;
                    int y = mCanvas.getHeight() - 20;

                    //Paint the canvas
                    paint.setColor(Color.BLACK);
                    mCanvas.drawRect(0, 0,
                        mCanvas.getWidth(), mCanvas.getHeight(), paint);
                    paint.setColor(color);
                    mCanvas.drawText(text, x, y, paint);
                }
            } finally {
                if (mCanvas != null)
                    mSurfaceHolder.unlockCanvasAndPost(mCanvas);
            }
            mHandler.removeCallbacks(mRefreshDisplay);
        }

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

            WatchFaceAnalogDigitalClockService.this.sendBroadcast(requestStateIntent);

            if (visible) {
                /**
                 * Register a broadcast receiver to receive time ticks
                 * every minute
                 */
                registerReceiver();
                draw(color);
            } else {
                //Unregister when it's no more visible
                unregisterReceiver();
                mHandler.removeCallbacks(mRefreshDisplay);
            }
        }

        @Override
        public void onSurfaceChanged(SurfaceHolder holder, int format,
            int width, int height) {
            super.onSurfaceChanged(holder, format, width, height);
            Log.i(SERVICE_TAG, "onSurfaceChanged");
            draw(color);
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
                //Redraw digital clock in black and white during ambient mode
                if (ambient_mode) {
                    Log.i(SERVICE_TAG,
                        "In ambient mode. Unregister broadcast receiver");
                    color = Color.WHITE;
                    draw(color);
                    unregisterReceiver();
                } else if (mVisible) {//Redraw digital clock in green during non-ambient mode
                    Log.i(SERVICE_TAG,
                        "Exiting ambient mode.Register broadcast receiver");
                    registerReceiver();
                    color = Color.GREEN;
                    draw(color);
                }
            }
            //Update and redraw digital clock every minute
            else if (action.matches(
                "com.google.android.wearable.action.AMBIENT_UPDATE")) {
                Log.i(SERVICE_TAG, "onCommand. Updating in ambient mode");
                draw(color);
            }
            return extras;
        }

        //Broadcast receiver to update clock time every minute
        private BroadcastReceiver mClockTimeReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context c, Intent i) {
                Log.i(SERVICE_TAG, "Broadcast receiver. Received time tick ");
                draw(color);
            }
        };

        //Register broadcast receiver
        private void registerReceiver() {
            if (mRegisteredTimeTickReceiver) {
                return;
            }
            mRegisteredTimeTickReceiver = true;

            Log.i(SERVICE_TAG, "Registering broadcast receiver");
            IntentFilter mClockTimeTick = new IntentFilter();

            mClockTimeTick.addAction(Intent.ACTION_TIME_TICK);
            mClockTimeTick.addAction(Intent.ACTION_TIME_CHANGED);
            mClockTimeTick.addAction(Intent.ACTION_TIMEZONE_CHANGED);

            WatchFaceAnalogDigitalClockService.this.registerReceiver(
                mClockTimeReceiver, mClockTimeTick);
        }

        //Unregister broadcast receiver
        private void unregisterReceiver() {
            if (!mRegisteredTimeTickReceiver) {
                return;
            }
            mRegisteredTimeTickReceiver = false;

            Log.i(SERVICE_TAG, "Unregistering broadcast receiver");
            WatchFaceAnalogDigitalClockService.this.unregisterReceiver(
                mClockTimeReceiver);
        }
    }
}
