/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.service;

import android.os.RemoteException;


/*
 * The Class VoicePrintServiceState.
 */
/**
 * The Interface VoicePrintServiceState.
 */
public interface VoicePrintServiceState {

    /**
     * Match.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void match(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;

    /**
     * Match with id.
     *
     * @param stub the stub
     * @param id the id
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void matchWithId(VoicePrintServiceStub stub, String id, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Match secure.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void matchSecure(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;

    /**
     * Match any.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void matchAny(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Match any key phrase.
     *
     * @param stub the stub
     * @param keyphrase the keyphrase
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void matchAnyKeyPhrase(VoicePrintServiceStub stub, String keyphrase, IVoicePrintServiceCallback cb) throws RemoteException;

    /**
     * Match any secure.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void matchAnySecure(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Enroll.
     *
     * @param stub the stub
     * @param id the id
     * @param keyphrase the keyphrase
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void enroll(VoicePrintServiceStub stub, String id, String keyphrase, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Complete enroll.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void completeEnroll(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Cancel.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void cancel(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Update.
     *
     * @param stub the stub
     * @param id the id
     * @param phrase the phrase
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void update(VoicePrintServiceStub stub, String id, String phrase, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Complete update.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void completeUpdate(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Delete.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void delete(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;

    /**
     * Delete with id.
     *
     * @param stub the stub
     * @param id the id
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void deleteWithId(VoicePrintServiceStub stub, String id, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * Delete all.
     *
     * @param stub the stub
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void deleteAll(VoicePrintServiceStub stub, IVoicePrintServiceCallback cb) throws RemoteException;


    /**
     * On start audio capture.
     *
     * @param stub the stub
     * @param sampleRate the sample rate
     * @param numberOfChannels the number of channels
     * @param audioFormat the audio format
     * @throws RemoteException the remote exception
     */
    public void onStartAudioCapture(VoicePrintServiceStub stub, int sampleRate, int numberOfChannels, int audioFormat) throws RemoteException;


    /**
     * On stop audio capture.
     *
     * @param stub the stub
     * @throws RemoteException the remote exception
     */
    public void onStopAudioCapture(VoicePrintServiceStub stub) throws RemoteException;


    /**
     * Adds the frame.
     *
     * @param stub the stub
     * @param buffer the buffer
     * @throws RemoteException the remote exception
     */
    public void addFrame(VoicePrintServiceStub stub, byte[] buffer) throws RemoteException;


    /**
     * Adds the frame with id.
     *
     * @param stub the stub
     * @param id the id
     * @param buffer the buffer
     * @throws RemoteException the remote exception
     */
    public void addFrameWithId(VoicePrintServiceStub stub, int id, byte[] buffer) throws RemoteException;


    /**
     * Version.
     *
     * @param stub the stub
     * @return the string
     * @throws RemoteException the remote exception
     */
    public String version(VoicePrintServiceStub stub) throws RemoteException;

    /**
     * Rename.
     *
     * @param stub the stub
     * @param oldId the old id
     * @param newId the new id
     * @param cb the cb
     * @throws RemoteException the remote exception
     */
    public void rename(VoicePrintServiceStub stub, String oldId, String newId, IVoicePrintServiceCallback cb) throws RemoteException;
}
