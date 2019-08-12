/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.service;

import android.os.Bundle;

 /*
 * AIDL (Android Interface Definition Language) programming interface for client
 * and service for inteprocess communication (IPC) and object marshalling.
 * These are direct function calls so threading must be properly handled.
 * The oneway keyword prevents blocking. The transaction is sent and immediately
 * returns.
 */

 oneway interface IVoicePrintServiceCallback {

	// Generic result for extensibility
	void onResult(int code, in Bundle data);

	// Generic error
	void onError(int error);

	// On a match or matchAny status. Field can be null!
	void onMatchStatus(int status, String user_id, String identifier, in Bundle bundle);

	// Enrollment status
	void onEnrollStatus(int status);


	// Delete Status. Fields can be null!
	void onDeleteStatus(int status, String user_id, String identifier);

	// Version info
	void onVersionInfo(long major, long minor);

	// Config setting status
	void onConfigUpdateStatus(int status);

	// Rename status
	void onRenameStatus(int status);

 }