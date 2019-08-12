/******************************************************************************
 * @file    IndivTask.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.controller;

import android.os.AsyncTask;
import android.view.View;
import android.widget.ProgressBar;

public class IndivTask extends AsyncTask<Void, Void, Void> {
    private ProgressBar mProgressBar;
    private SAMExecutor.ITask mTask;
    private IHandler handler;

    public interface IHandler {
        // runs on UI thread
        public void refreshUI();
    }

    public void uihandler(IHandler handler) {
        this.handler = handler;
    }

    private boolean hasRegisteredCallBack() {
        if (handler != null)
            return true;
        return false;
    }

    public IndivTask(SAMExecutor.ITask task, ProgressBar progressBar) {
        this.mTask = task;
        this.mProgressBar = progressBar;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();

        if (mProgressBar != null)
            mProgressBar.setVisibility(View.INVISIBLE);
    }

    @Override
    protected Void doInBackground(Void[] params) {
        SAMExecutor.INSTANCE.addTask(this);

        mTask.run();

        return null;
    }

    @Override
    protected void onCancelled() {
        super.onCancelled();
        unregisterSelf();
    }

    @Override
    protected void onPostExecute(Void o) {
        super.onPostExecute(o);

        mTask.updateUIData();

        if (mProgressBar != null)
            mProgressBar.setVisibility(View.INVISIBLE);

        unregisterSelf();

        if (hasRegisteredCallBack())
            handler.refreshUI();

    }

    private void unregisterSelf() {
        SAMExecutor.INSTANCE.removeTask(this);
    }
}
