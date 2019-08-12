/*
 * Copyright (c) 2015, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.telephony.vodafonepack;

import android.app.Application;
import android.app.StatusBarManager;
import android.content.Context;
import android.content.res.Resources;
import android.os.AsyncResult;
import android.os.Handler;
import android.util.Log;

import com.qualcomm.qcrilhook.IQcRilHook;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;

public class CarrierpackApplication extends Application{

    public static final String TAG = "CarrierpackApplication";
    private static final boolean DBG = true;
    private static CarrierpackApplication mInstance = null;
    private Context mContext = null;

    private QcRilHook mQcRilHookInterface;
    private StatusBarManager mStatusBarManager;
    private boolean mIsShowingFemtoIcon = false;

    //Unique string to find icon slot on status bar
    private static final String FEMTO_ICON = "femtoicon";

    static final int INVALID_CSG_ID = 0xFFFFFFFF;
    static final int EVT_CSG_CHANGED = 1;
    static final int EVT_BOOT_COMPLETED = 2;

    private int mCsgId = INVALID_CSG_ID;

    /**
     * Called when connection to QcrilMsgTunnelService has been established.
     */
    private QcRilHookCallback mQcrilHookCb = new QcRilHookCallback() {
        public void onQcRilHookReady() {
            getCurrentCsgId();
        }

        public void onQcRilHookDisconnected() {
            mCsgId = INVALID_CSG_ID;
        }
    };

    Handler mHandler = new Handler(){
        public void handleMessage(android.os.Message msg) {
            switch(msg.what) {
                case EVT_CSG_CHANGED:
                    Integer csgId = (Integer)msg.obj;
                    handleCsgIdChangedInd(csgId);
                    break;

                case EVT_BOOT_COMPLETED:
                    getCurrentCsgId();
                    break;
            }
        };
    };

    public static CarrierpackApplication getInstance() {
        return mInstance;
    }

    public Handler getHandler() {
        return mHandler;
    }

    @Override
    public void onCreate() {
        log("App Started. onCreate");
        mInstance = this;
        mContext = getApplicationContext();
        boolean vodafonePackEnabled = mContext.getResources()
                .getBoolean(R.bool.enable_vodafone_pack);
        if (!vodafonePackEnabled)
            return;
        mQcRilHookInterface = new QcRilHook(mContext, mQcrilHookCb);
        mStatusBarManager = (StatusBarManager) mContext.
                getSystemService(Context.STATUS_BAR_SERVICE);
    }

    private void handleCsgIdChangedInd(int csgId) {
        log("HandleCsgIdChangedInd returned : " + csgId + ", current is: " + mCsgId);
        if (mCsgId != csgId) {
            mCsgId = csgId;
            updateFemtoCellIcon();
        }
    }

    private void getCurrentCsgId() {
        int csgId = mQcRilHookInterface.qcRilGetCsgId();
        log("getCurrentCsgId returned : " + csgId + ", current is: " + mCsgId);
        handleCsgIdChangedInd(csgId);
    }

    private void updateFemtoCellIcon() {
        boolean vodafonePackEnabled = mContext.getResources()
                .getBoolean(R.bool.enable_vodafone_pack);
        if (!vodafonePackEnabled)
            return;

        log("updateFemtoCellIcon : " + (mCsgId != INVALID_CSG_ID));
        if (mCsgId != INVALID_CSG_ID) {
            if (!mIsShowingFemtoIcon) {
                mStatusBarManager.setIcon(
                        FEMTO_ICON,
                        R.drawable.femtocell,
                        0,  /* iconLevel */
                        mContext.getResources().getString(R.string.camped_on_femto));
                mIsShowingFemtoIcon = true;
            }
        } else if (mIsShowingFemtoIcon){ /* remove icon only if its already showing */
            mStatusBarManager.removeIcon(FEMTO_ICON);
            mIsShowingFemtoIcon = false;
        }
    }

    public void log(String str) {
        if(DBG) {
            Log.d(TAG, str);
        }
    }
}
