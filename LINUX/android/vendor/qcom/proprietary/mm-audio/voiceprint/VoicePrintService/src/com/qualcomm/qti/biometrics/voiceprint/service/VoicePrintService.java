/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.service;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import java.io.IOException;

/**
 * The Class QVoicePrintService.
 */
public class VoicePrintService extends Service {

	/**
	 * The Constant Logging TAG.
	 */
	private final static String TAG = "VoicePrintService";

// The following should match the Service

	/** The Constant SUCCESS. */
	public final static int SUCCESS = 0;

	/** The Constant FAILURE. */
	public final static int FAILURE = 1;

	/** The Constant TIMEOUT. */
	public final static int TIMEOUT = 2;

	/** The Constant ERROR. */
	public final static int ERROR = 3;

	/** The Constant RESOURCE_UNAVAILABLE. */
	public final static int RESOURCE_UNAVAILABLE = 4;

	/** The Constant PERMISSION_DENIED. */
	public static final int PERMISSION_DENIED = 5;


	/** The _stub. */
	private VoicePrintServiceStub _stub = null;

	/*
	 * (non-Javadoc)
	 *
	 * @see android.app.Service#onBind(android.content.Intent)
	 */
	@Override
	public IBinder onBind(Intent intent) {
		Log.d(TAG, "onBind()");
// First client
		if (_stub == null) {
			_stub = new VoicePrintServiceStub(this);
		}

		return _stub;
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see android.app.Service#onStartCommand(android.content.Intent, int, int)
	 */
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.d(TAG, "onStartCommand()");
		return START_STICKY;
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see android.app.Service#onUnbind(android.content.Intent)
	 */
	@Override
	public boolean onUnbind(Intent intent) {
		Log.d(TAG, "onUnbind()");
// Last client disconnection, close the IBinder
		try {
			if (_stub != null) {
				_stub.close();
			}
			_stub = null;
		} catch (IOException e) {

			e.printStackTrace();
		}

		return super.onUnbind(intent);
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see android.app.Service#onDestroy()
	 */
	@Override
	public void onDestroy() {
		Log.d(TAG, "onDestroy()");
		try {
			if (_stub != null) {
				_stub.close();
			}
			_stub = null;
		} catch (IOException e) {

			e.printStackTrace();
		}

		super.onDestroy();
	}
}
