/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmusicplayer;

import java.io.Serializable;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.AdapterView;
import android.widget.LinearLayout;
import android.widget.SimpleCursorAdapter;
import android.widget.Toast;
import java.util.ArrayList;

public class MusicPlayer extends Activity {

    private static final String TAG = "MusicPlayer";
    //song list variables
    private ArrayList<Song> songList;
    private ListView songListView;

    // Intent Receiver
    ComponentName mMediaButtonReceiverComponent;
    private MusicPlayerStateReceiver mMusicIntentReceiver;
    // Buttons
    ImageButton mPlayBtn;
    ImageButton mPrevBtn;
    ImageButton mNextBtn;
    ImageButton mCloseBtn;

    private int currSongIdx = -1;

    private PlayerState playerState = PlayerState.Stopped;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "MusicPlayer: onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player);

        songListView = (ListView)findViewById(R.id.song_list);
        if(Utils.IsDisplayTypeCircular()) {
            LinearLayout musicplr = (LinearLayout) findViewById(R.id.musicplr);
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            musicplr.setPadding(0, paddingDp, 0, 0);
            songListView.setPadding(paddingDp,0,0,paddingDp);
        }


        songList = new ArrayList<Song>();
        retrieveMusic();

        /*
           If we found at least one song to play then initialize current
           song to 0.
           */
        if (songList.size() > 0) {
            setSongsList();
            currSongIdx = 0;
        }

        Uri uri = android.provider.MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;

        Cursor cur = getContentResolver().query(uri, null, MediaStore.Audio.Media.IS_MUSIC +  "= 1",
                null, null);

        String[] fromColumns = {MediaStore.Audio.Media.TITLE,
            MediaStore.Audio.Media.ARTIST
        };
        int[] toViews = {R.id.song_title, R.id.song_artist};

        SimpleCursorAdapter adapter = new SimpleCursorAdapter(this,
                R.layout.song, cur, fromColumns, toViews, 0);

        songListView.setAdapter(adapter);
        songListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                view.setSelected(true);
                songSelected(i);
            }
        });

        ImageButton.OnClickListener btnListner = new ImageButton.OnClickListener() {
            @Override
            public void onClick(View view) {
                handleBtnEvent(view);
            }
        };

        mPlayBtn = (ImageButton) findViewById(R.id.play_btn);
        mPlayBtn.setOnClickListener(btnListner);
        mPrevBtn = (ImageButton) findViewById(R.id.prev_btn);
        mPrevBtn.setOnClickListener(btnListner);
        mNextBtn = (ImageButton) findViewById(R.id.next_btn);
        mNextBtn.setOnClickListener(btnListner);
        mCloseBtn = (ImageButton) findViewById(R.id.close_btn);
        mCloseBtn.setOnClickListener(btnListner);

        mMusicIntentReceiver =  new MusicPlayerStateReceiver();
        registerReceiver(mMusicIntentReceiver, new IntentFilter(MusicService.ACTION_SET_PLYER_STATE));

        mMediaButtonReceiverComponent = new ComponentName(this, MusicIntentReceiver.class);
    }

    public void retrieveMusic(){
        Uri uri = android.provider.MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;

        Cursor cur = getContentResolver().query(uri, null, MediaStore.Audio.Media.IS_MUSIC +  "= 1",
                null, null);

        if(cur==null) {
            Log.e(TAG, "Failed to retrieve music content");
            Toast.makeText(this, "Failed to retrieve music content.", Toast.LENGTH_SHORT).show();
        }
        else if (!cur.moveToFirst()) {
            Log.e(TAG, "No music content found");
            Toast.makeText(this, "No music content found.", Toast.LENGTH_SHORT).show();
        } else {
            int titleColumn = cur.getColumnIndex(MediaStore.Audio.Media.TITLE);
            int idColumn = cur.getColumnIndex(MediaStore.Audio.Media._ID);
            int artistColumn = cur.getColumnIndex(MediaStore.Audio.Media.ARTIST);
            int albumColumn = cur.getColumnIndex(MediaStore.Audio.Media.ALBUM);
            int durationColumn = cur.getColumnIndex(MediaStore.Audio.Media.DURATION);
            do {
                long id = cur.getLong(idColumn);
                String title = cur.getString(titleColumn);
                String artist = cur.getString(artistColumn);
                String album = cur.getString(albumColumn);
                int duration = cur.getInt(durationColumn);
                songList.add(new Song(id, title, artist, album, duration));
            }
            while (cur.moveToNext());
        }
    }

    private boolean isMusicPlayerServiceRunning(Class<?> serviceClass) {
        ActivityManager manager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (serviceClass.getName().equals(service.service.getClassName())) {
                return true;
            }
        }
        return false;
    }

    @Override
    protected void onStart() {
        super.onStart();
        if(!isMusicPlayerServiceRunning(MusicService.class)) {
            Intent intent = new Intent(this, MusicService.class);
            intent.setAction(MusicService.ACTION_START_SERVICE);
            this.startService(intent);

            setSongsList();
        } else {
            sendMusicIntent(MusicService.ACTION_GET_PLYER_STATE);
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    //user song select
    public void songSelected(int id){
        if (id >= 0 && id < songList.size()) {
            currSongIdx = id;
            Log.d(TAG, "MusicPlayer: currSongIdx=" + currSongIdx);
            playSong();
        } else {
            Log.e(TAG, "Invalid item selected");
        }
    }

    public void playPauseSong() {

        if (playerState == PlayerState.Playing) {
            Log.d(TAG, "MusicPlayer: pause song");
            pauseSong();
        } else if (playerState == PlayerState.Paused) {
            Log.d(TAG, "MusicPlayer: restart paused song");
            restartSong();
        } else {
            Log.d(TAG, "MusicPlayer: play song");
            playSong();
        }
    }

    private void setSongsList() {
        Intent intent = new Intent(this, MusicService.class);
        intent.setAction(MusicService.ACTION_SET_SONG_LIST);
        intent.putParcelableArrayListExtra("data", songList);
        this.startService(intent);
    }

    public void restartSong() {
        sendMusicIntent(MusicService.ACTION_PLAY);
    }

    public void playSong() {
        sendMusicIntent(MusicService.ACTION_PLAY, currSongIdx);
    }

    public void pauseSong()
    {
        sendMusicIntent(MusicService.ACTION_PAUSE);
    }

    protected void sendMusicIntent(String action, int data) {
        Intent intent = new Intent(this, MusicService.class);
        intent.setAction(action);
        intent.putExtra("data", data);
        this.startService(intent);
    }

    protected void sendMusicIntent(String action) {
        Intent intent = new Intent(this, MusicService.class);
        intent.setAction(action);
        this.startService(intent);
    }

    public void handleBtnEvent(View view) {
        Log.e(TAG, "btnPressed="+view.getId());
        if (currSongIdx >= 0) {
            if (view == mPlayBtn) {
                playPauseSong();
            } else if (view == mPrevBtn) {
                sendMusicIntent(MusicService.ACTION_PREVIOUS);
            } else if (view == mNextBtn) {
                sendMusicIntent(MusicService.ACTION_NEXT);
            } else if (view == mCloseBtn) {
                closeServiceAndApp();
            }
        }
    }

    private void closeServiceAndApp() {
        sendMusicIntent(MusicService.ACTION_STOP_SERVICE);
        finish();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mMusicIntentReceiver);
    }

    public void setPlayerState(Serializable state) {
        Log.d(TAG, "Received Player State: " + state);
        setPlayerState((PlayerState)state);
    }

    private void setPlayerState(PlayerState state) {
        playerState = state;
        ImageButton btn = (ImageButton) findViewById(R.id.play_btn);

        if(playerState == PlayerState.Playing) {
            btn.setImageResource(android.R.drawable.ic_media_pause);
        } else if(playerState == PlayerState.Paused) {
            btn.setImageResource(android.R.drawable.ic_media_play);
        }
    }

    public class MusicPlayerStateReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(MusicService.ACTION_SET_PLYER_STATE)) {
                setPlayerState(intent.getSerializableExtra("state"));
            }
        }
    }
}

