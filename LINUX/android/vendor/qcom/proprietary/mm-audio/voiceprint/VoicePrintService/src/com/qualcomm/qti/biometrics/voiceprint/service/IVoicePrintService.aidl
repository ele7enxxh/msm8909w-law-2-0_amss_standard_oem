/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.service;

import com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback;


 /*
 * AIDL (Android Interface Definition Language) programming interface for client
 * and service for inteprocess communication (IPC) and object marshalling.
 * These are direct function calls so threading must be properly handled.
 */
 interface IVoicePrintService {

 	oneway void   match(IVoicePrintServiceCallback cb);
 	oneway void   matchWithId(String id, IVoicePrintServiceCallback cb);
 	oneway void   matchSecure(IVoicePrintServiceCallback cb);

 	oneway void matchAny(IVoicePrintServiceCallback cb);
 	oneway void matchAnyKeyPhrase(String keyphrase, IVoicePrintServiceCallback cb);
 	oneway void matchAnySecure(IVoicePrintServiceCallback cb);

 	oneway void enroll(String id, String keyphrase, int threshold, IVoicePrintServiceCallback cb);
 	oneway void completeEnroll(IVoicePrintServiceCallback cb);
 	oneway void cancel(IVoicePrintServiceCallback cb);

 	oneway void update(String id, String phrase, IVoicePrintServiceCallback cb);
 	oneway void setConfig(String id, in Bundle config, IVoicePrintServiceCallback cb);
    oneway void completeUpdate(IVoicePrintServiceCallback cb);

    oneway void delete(IVoicePrintServiceCallback cb);
 	oneway void deleteWithId(String id, IVoicePrintServiceCallback cb);
 	oneway void deleteAll(IVoicePrintServiceCallback cb);

 	oneway void onStartAudioCapture(int sampleRate, int numberOfChannels, int audioFormat, IVoicePrintServiceCallback cb);
    oneway void onStopAudioCapture(IVoicePrintServiceCallback cb);
    oneway void addFrame(in Bundle data);
    oneway void addFrameWithId(int id, in Bundle data);

    oneway void rename(String oldId, String newId, IVoicePrintServiceCallback cb);

 	String version();

 	boolean isLibraryAvailable();
 }
