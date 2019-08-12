/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.service;

import android.os.Bundle;

/**
 * The Interface IJniCallback.
 */
public interface IJniCallback {

/**
 * On result. Generic result for extensibility
 *
 * @param code
 *            the code
 * @param data
 *            the data
 */
void onResult(int code);

/**
 * On error. Generic error
 *
 * @param error
 *            the error
 */
void onError(int error);

/**
 * On a match or matchAny status. Field can be null!
 *
 * @param status
 *            the status of request
 * @param user_id
 *            the user_id or null if not provided
 * @param identifier
 *            the identifier or null if not provided
 */
void onMatchStatus(int status, String user_id, String identifier, int index, double sentence_score, double ver_score, float spoofScore);

/**
 * On enroll status.
 *
 * @param status
 *            the status code
 */
void onEnrollStatus(int status);

/**
 * On delete status. Delete Status. Fields can be null!
 *
 * @param status
 *            the status code
 * @param user_id
 *            the user_id or null if not provided
 * @param identifier
 *            the identifier or null if not provided
 */
void onDeleteStatus(int status, String user_id, String identifier);

/**
 * On version info.
 *
 * @param major
 *            the major
 * @param minor
 *            the minor
 */
void onVersionInfo(long major, long minor);

/**
 * On config update status.
 *
 * @param status
 *            the status code
 */
void onConfigUpdateStatus(int status);

/**
 * On rename status.
 *
 * @param status
 *            the status code
 */
void onRenameStatus(int status);

}