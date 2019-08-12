/******************************************************************************
 * @file    SAMExecutor.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.controller;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Handler;
import android.util.Log;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.qualcomm.qti.sam.interfaces.SAMErrors;
import com.samautotester.controller.TestFunc;

public enum SAMExecutor {
    INSTANCE;

    private List<IndivTask> runningTasks = Collections
            .synchronizedList(new ArrayList<IndivTask>());

    private SAMManagerWrapper manager;
    private static final String LOG_TAG = SAMExecutor.class.getName();

    public interface ITask {
        public void run();

        public void updateUIData();
    }

    public String getResultValue(int returnValue) {
        switch (returnValue) {
        case SAMErrors.SUCCESS:
            return "SUCCESS";

        case SAMErrors.RADIO_NOT_AVAILABLE:
            return "RADIO_NOT_AVAILABLE";

        case SAMErrors.REQUEST_NOT_SUPPORTED:
            return "REQUEST_NOT_SUPPORTED";

        case SAMErrors.BUSY:
            return "BUSY";

        case SAMErrors.SIM_ABSENT:
            return "SIM_ABSENT";

        case SAMErrors.INVALID_PARAMETER:
            return "INVALID_PARAMETER";

        case SAMErrors.INIT_FAILURE:
            return "INIT_FAILURE";

        case SAMErrors.SERVICE_NOT_CONNECTED:
            return "SERVICE_NOT_CONNECTED";

        case SAMErrors.INTERNAL_FAILURE:
            return "INTERNAL_FAILURE";

        }

        return "UNKNOWN_ERROR";
    }

    public SAMManagerWrapper getSAMManagerWrapper() {
        return manager;
    }

    public IndivTask connect(final TestFunc testfunc, final Context context,
            final Handler callbackHandler, final int serviceConnectionStatusId,
            ProgressBar progressBar) {

        // creates dummy data for Individual test cases.. only when trying to
        // connect
        testfunc.createNewData();

        ITask task = new ITask() {
            private boolean connection_state;

            @Override
            public void run() {
                // creates new instance of SAMManagerWrapper & connect to
                // service
                manager = new SAMManagerWrapper(context);
                connection_state = manager.connectService(callbackHandler,
                        serviceConnectionStatusId);

                testfunc.set_ConnectService_returnValue(connection_state);
                Log.d(LOG_TAG, "Connection State - " + connection_state);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask isServiceConnected(final TestFunc testfunc,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            private boolean isServiceConnected;

            @Override
            public void run() {
                isServiceConnected = manager.isServiceConnected();
                Log.d(LOG_TAG, "isService Connected - " + isServiceConnected);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask disconnectService(final TestFunc testfunc,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            @Override
            public void run() {
                manager.disconnectService();
                Log.d(LOG_TAG, "Service Disconnected");

            }

            public void updateUIData() {
                // Not Required to update...

                // Extra's ..
                // stop & clear all running tasks
                for (IndivTask task : runningTasks)
                    task.cancel(true);

                testfunc.flushData();
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask registerCallback(final TestFunc testfunc,
            final ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {
                returnValue = manager.registerCallback();
                testfunc.set_RegisteredCallBack_returnValue(returnValue);

                Log.d(LOG_TAG, "registerCallback - returnValue = "
                        + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;

    }

    public IndivTask deregisterCallback(final TestFunc testfunc,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {
                returnValue = manager.deregisterCallback();
                Log.d(LOG_TAG, "deregisterCallback - returnValue = "
                        + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask getCardState(final TestFunc testfunc, final int slotId,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {
                returnValue = manager.getCardState(slotId, testfunc);
                testfunc.set_CardState_returnValue(returnValue);
                testfunc.set_CardState_result(getResultValue(returnValue));

                Log.d(LOG_TAG, "CardState - returnValue = " + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask getCardATR(final TestFunc testfunc, final int slotId,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {
                returnValue = manager.getCardATR(slotId, testfunc);
                testfunc.set_CardATR_returnValue(returnValue);
                testfunc.set_CardATR_result(getResultValue(returnValue));

                Log.d(LOG_TAG, "CardATR - returnValue = " + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask streamApdu(final TestFunc testfunc, final int slotId,
            final int class_type, final int instruction, final int param1,
            final int param2, final int param3, final String dataCmd,
            final ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {
                Log.d(LOG_TAG, "StreamApdu - dataCmd = " + dataCmd);
                returnValue = manager.streamApdu(slotId, class_type,
                        instruction, param1, param2, param3, dataCmd);

                testfunc.set_StreamApdu_returnValue(returnValue);
                testfunc.set_StreamApdu_result(getResultValue(returnValue));

                Log.d(LOG_TAG, "StreamApdu - returnValue = " + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask switchSlot(final TestFunc testfunc, final int[] slotList,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {
                returnValue = manager.switchSlot(slotList);
                testfunc.set_SwitchSlot_returnValue(returnValue);
                testfunc.set_SwitchSlot_result(getResultValue(returnValue));
                Log.d(LOG_TAG, "SwitchSlot - returnValue = " + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask getSlotMapping(final TestFunc testfunc,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {

                returnValue = manager.getSlotMapping(testfunc);
                testfunc.set_GetSlotMapping_returnValue(returnValue);

                Log.d(LOG_TAG, "SlotMapping  = " + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask cardPowerUp(final TestFunc testfunc, final int slotId,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {
                returnValue = manager.cardPowerUp(slotId);
                testfunc.set_CardPowerUp_returnValue(returnValue);
                testfunc.set_CardPowerUp_result(getResultValue(returnValue));
                Log.d(LOG_TAG, "CardPowerUp  - returnValue = " + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public IndivTask cardPowerDown(final TestFunc testfunc, final int slotId,
            ProgressBar progressBar) {

        ITask task = new ITask() {
            private int returnValue;

            @Override
            public void run() {
                returnValue = manager.cardPowerDown(slotId);
                testfunc.set_CardPowerDown_returnValue(returnValue);
                testfunc.set_CardPowerDown_result(getResultValue(returnValue));

                Log.d(LOG_TAG, "CardPowerDown  - returnValue = " + returnValue);
            }

            public void updateUIData() {
                // Not Required to update...
            }
        };

        IndivTask indivTask = new IndivTask(task, progressBar);

        return indivTask;
    }

    public void addTask(IndivTask task) {
        runningTasks.add(task);
    }

    public void removeTask(IndivTask task) {
        runningTasks.remove(task);
    }

}
