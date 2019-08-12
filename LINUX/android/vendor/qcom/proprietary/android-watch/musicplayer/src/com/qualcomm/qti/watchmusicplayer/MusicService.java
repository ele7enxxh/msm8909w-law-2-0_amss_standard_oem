/*
 * Copyright (c) 2015-16 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmusicplayer;

import java.util.ArrayList;


import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.ContentUris;
import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.media.AudioManager.OnAudioFocusChangeListener;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Binder;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;

public class MusicService extends Service implements
MediaPlayer.OnPreparedListener, MediaPlayer.OnErrorListener,
    MediaPlayer.OnCompletionListener {

        private MediaPlayer mMediaPlayer;
        private AudioManager mAudioManager;
        /**
         * songsList is set by MusicPlayer once during activity life cycle.
         * songsList is set so that MusicService can act on Media Button actions.
         */
        private ArrayList<Song> songsList;
        private int mCurrSongIdx = 0;
        private boolean mAudioFocusGranted = false;
        private Context mContext = null;
        private OnAudioFocusChangeListener mOnAudioFocusChangeListener = null;
        private final IBinder mMusicBinder = new MusicBinder();

        public static final String ACTION_TOGGLE_PLAYBACK ="com.qualcomm.qti.watchmusicplayer.action.TOGGLE_PLAYBACK";
        public static final String ACTION_START_SERVICE = "com.qualcomm.qti.watchmusicplayer1.action.ACTION_START_SERVICE";
        public static final String ACTION_STOP_SERVICE = "com.qualcomm.qti.watchmusicplayer1.action.ACTION_STOP_SERVICE";
        public static final String ACTION_GET_PLYER_STATE = "com.qualcomm.qti.watchmusicplayer1.action.ACTION_GET_PLYER_STATE";
        public static final String ACTION_SET_PLYER_STATE = "com.qualcomm.qti.watchmusicplayer.action.ACTION_SET_PLYER_STATE";
        public static final String ACTION_SET_SONG_LIST = "com.qualcomm.qti.watchmusicplayer1.action.SET_SONG_LIST";
        public static final String ACTION_PLAY = "com.qualcomm.qti.watchmusicplayer.action.PLAY";
        public static final String ACTION_PAUSE = "com.qualcomm.qti.watchmusicplayer.actionc.PAUSE";
        public static final String ACTION_STOP = "com.qualcomm.qti.watchmusicplayer.action.STOP";
        public static final String ACTION_NEXT = "com.qualcomm.qti.watchmusicplayer.action.NEXT";
        public static final String ACTION_PREVIOUS = "com.qualcomm.qti.watchmusicplayer.action.PREVIOUS";
        private static final String TAG = "MusicService";
        private static int NOTFICATION_ID = 1001;
        private Song mCurSong = null;
        private boolean mPausedByTransientLossOfFocus = false;

        PlayerState playerState = PlayerState.Stopped;

        public void onCreate(){
            Log.e("MusicService", "onCreate");
            super.onCreate();

            mCurrSongIdx =0;
            songsList = new ArrayList<Song>();
            mContext = getApplicationContext();

            /**
             * Create  and initialize MediaPlayer used for all media playback.
             */
            initMusicPlayer();
        }

        public void initMusicPlayer(){
            Log.e(TAG, "init Music Player");
            mMediaPlayer = new MediaPlayer();
            mAudioManager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
            mMediaPlayer.setWakeMode(getApplicationContext(),
                    PowerManager.PARTIAL_WAKE_LOCK);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            //setVolumeControlStream(AudioManager.STREAM_MUSIC);

            //Register for required callbacks.
            mMediaPlayer.setOnPreparedListener(this);
            mMediaPlayer.setOnCompletionListener(this);
            mMediaPlayer.setOnErrorListener(this);
            mOnAudioFocusChangeListener =
                    new AudioManager.OnAudioFocusChangeListener() {
                        public void onAudioFocusChange(int focusChange) {
                            if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT) {
                                // Pause playback
                                Log.d(TAG, "AUDIOFOCUS_LOSS_TRANSIENT");
                                if(playerState == PlayerState.Playing){
                                    mPausedByTransientLossOfFocus = true;
                                }else {
                                    mPausedByTransientLossOfFocus = false;
                                }
                                pauseSong();
                            } else if (focusChange == AudioManager.AUDIOFOCUS_GAIN) {
                                // Resume playback
                                Log.d(TAG, "AUDIOFOCUS_GAIN");
                                if(playerState == PlayerState.Paused && mPausedByTransientLossOfFocus){
                                    mPausedByTransientLossOfFocus = false;
                                    restartSong();
                                }
                            } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS) {
                                // Stop playback
                                Log.d(TAG, "AUDIOFOCUS_LOSS");
                                stopSong();
                            }
                        }
                    };
        }

        public void setList(ArrayList<Song> songs){
            songsList = songs;
        }

        public class MusicBinder extends Binder {
            MusicService getService() {
                return MusicService.this;
            }
        }

        //activity will bind to service
        @Override
        public IBinder onBind(Intent intent) {
            return mMusicBinder;
        }

        //release resources when unbind
        @Override
        public boolean onUnbind(Intent intent){
            return false;
        }

        @Override
        public void onDestroy() {
            stopSong();
            super.onDestroy();
        }

        private void releaseMediaPlayer() {
            if(mMediaPlayer != null) {
                mMediaPlayer.stop();
                mMediaPlayer.release();
                mMediaPlayer = null;
            }
        }

        //play a song
        public void playSong(){
            //play
            if (mMediaPlayer == null) {
                initMusicPlayer();
            } else {
                mMediaPlayer.reset();
            }

            if (mCurrSongIdx >= songsList.size() || mCurrSongIdx < 0 ) {
                Log.e(TAG, "No such song index: " + mCurrSongIdx);
                mCurrSongIdx = 0;
                return;
            }

            //get song
            mCurSong = songsList.get(mCurrSongIdx);
            //get id
            long currSong = mCurSong.getID();
            //set uri
            Uri trackUri = ContentUris.withAppendedId(
                    android.provider.MediaStore.Audio.Media.EXTERNAL_CONTENT_URI,
                    currSong);
            //set the data source
            try{
                mMediaPlayer.setDataSource(getApplicationContext(), trackUri);
            }
            catch(Exception e){
                Log.e(TAG, "Error setting data source", e);
            }
            if(requestAudioFocus()) {
                mMediaPlayer.prepareAsync();
            }
        }

        //pause song
        public void pauseSong(){
            if(mMediaPlayer != null) {
                mMediaPlayer.pause();
                playerState = PlayerState.Paused;
            }
            sendPlayerState();
        }

        // restart previously paused song
        public void restartSong(){
            if(requestAudioFocus()) {
                mMediaPlayer.start();
                playerState = PlayerState.Playing;
                sendPlayerState();
            }
        }
        //stop song
        public void stopSong(){
            releaseMediaPlayer();
            abandonAudioFocus();
            playerState = PlayerState.Stopped;
            sendPlayerState();
        }
        //set the song
        public void setSong(int songIndex){
            if (songIndex >= 0 && songIndex < songsList.size()) {
                mCurrSongIdx = songIndex;
            }
        }

        public void playNextSong() {
            if (mCurrSongIdx < songsList.size()-1) {
                mCurrSongIdx++;
            } else {
                mCurrSongIdx = 0;
            }
            playSong();
        }

        public void playPrevSong() {
            if (mCurrSongIdx > 0) {
                mCurrSongIdx--;
            } else {
                mCurrSongIdx = songsList.size()-1;
            }
            playSong();
        }

        @Override
        public void onCompletion(MediaPlayer mp) {
            playNextSong();
        }

        @Override
        public boolean onError(MediaPlayer mp, int what, int extra) {
            playerState = PlayerState.Stopped;
            sendPlayerState();
            return false;
        }

        @Override
        public void onPrepared(MediaPlayer mp) {
            //start playback
            mp.start();
            playerState = PlayerState.Playing;
            sendPlayerState();
        }

        /**
         * Called when we receive an Intent. When we receive an intent sent to us via startService(),
         * this is the method that gets called. So here we react appropriately depending on the
         * Intent's action, which specifies what is being requested of us.
         */
        @Override
        public int onStartCommand(Intent intent, int flags, int startId) {
            if (intent != null) {
                if (intent.getAction() != null) {
                    String action = intent.getAction();
                    int statusTextID = R.string.playing;
                    Intent notificationIntent = new Intent(this, MusicPlayer.class);
                    PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0);
                    Notification.Builder builder = new Notification.Builder(this);
                    builder.setSmallIcon(R.drawable.ic_launcher);
                    builder.setContentTitle(getText(R.string.app_name));

                    if (action.equals(ACTION_SET_SONG_LIST)) {
                        songsList = intent.getParcelableArrayListExtra("data");
                    }
                    else if (action.equals(ACTION_TOGGLE_PLAYBACK)) {
                        Log.e(TAG, "ACTION_TOGGLE_PLAYBACK");
                    }
                    else if (action.equals(ACTION_PLAY)) {
                        int data = intent.getIntExtra("data", -1);
                        if (data >= 0) {
                            setSong(data);
                            playSong();
                        } else {
                            restartSong();
                        }

                    }
                    else if (action.equals(ACTION_PAUSE)) {
                        pauseSong();
                        statusTextID = R.string.paused;
                    }
                    else if (action.equals(ACTION_NEXT)) {
                        playNextSong();
                        statusTextID = R.string.playing;
                    }
                    else if (action.equals(ACTION_STOP)) {
                        stopSong();
                        statusTextID = R.string.stopped;
                    }
                    else if (action.equals(ACTION_PREVIOUS)) {
                        playPrevSong();
                        statusTextID = R.string.playing;
                    }
                    else if (action.equals(ACTION_START_SERVICE)) {
                        statusTextID = R.string.stopped;
                    }
                    else if(action.equals(ACTION_STOP_SERVICE)) {
                        stopForeground(true);
                        releaseMediaPlayer();
                        stopSelf();
                        return START_NOT_STICKY;
                    }
                    else if(action.equals(ACTION_GET_PLYER_STATE)) {
                        sendPlayerState();
                    }

                    if(!action.equals(ACTION_STOP_SERVICE)) {
                        String displayTrackInfo = getText(statusTextID).toString();
                        if(mCurSong != null) {
                            displayTrackInfo += " " + mCurSong.getTitle();
                        }

                        builder.setContentText(displayTrackInfo);
                        builder.setContentIntent(pendingIntent);
                        Notification notification = builder.build();
                        startForeground(NOTFICATION_ID, notification);
                    }
                }
            }
            return START_STICKY;
        }

        private void sendPlayerState() {
            Intent i = new Intent();
            i.setAction(ACTION_SET_PLYER_STATE);
            i.putExtra("state", playerState);
            mContext.sendBroadcast(i);
        }

        private boolean requestAudioFocus() {
            if (!mAudioFocusGranted) {
                // Request audio focus for play back
                int result = mAudioManager.requestAudioFocus(mOnAudioFocusChangeListener,
                        AudioManager.STREAM_MUSIC,
                        AudioManager.AUDIOFOCUS_GAIN);

                if (result == AudioManager.AUDIOFOCUS_REQUEST_GRANTED) {
                    Log.d(TAG," Requst audio focus GRANTED");
                    mAudioFocusGranted = true;
                } else {
                    Log.e(TAG,"Request audio focus FAILED");
                }
            }
            return mAudioFocusGranted;
        }

        private void abandonAudioFocus() {
            int result = mAudioManager.abandonAudioFocus(mOnAudioFocusChangeListener);
            if (result == AudioManager.AUDIOFOCUS_REQUEST_GRANTED) {
                Log.d(TAG, "Abandon audio focus GRANTED");
                mAudioFocusGranted = false;
            } else {
                Log.e(TAG, "Abandon audio focus FAILED");
            }
            mOnAudioFocusChangeListener = null;
        }

}
