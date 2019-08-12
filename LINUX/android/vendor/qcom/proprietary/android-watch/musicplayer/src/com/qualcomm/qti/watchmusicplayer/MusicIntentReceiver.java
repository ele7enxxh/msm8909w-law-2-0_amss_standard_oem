/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmusicplayer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Toast;

/**
 * MusicIntentReceiver is setup to handle ACTION_AUDIO_BECOMING_NOISY and
 * ACTION_MEDIA_BUTTON.
 */
public class MusicIntentReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent.getAction().equals(android.media.AudioManager.ACTION_AUDIO_BECOMING_NOISY)) {
            Log.e("MUSIC SERVICE", "BT disconnected. ACTION_AUDIO_BECOMING_NOISY");
            // send an intent to our MusicService to pause the audio
            Intent action = new Intent(context, MusicService.class);
            action.setAction(MusicService.ACTION_PAUSE);
            context.startService(action);
        } else if (intent.getAction().equals(Intent.ACTION_MEDIA_BUTTON)) {
            KeyEvent keyEvent = (KeyEvent) intent.getExtras().get(Intent.EXTRA_KEY_EVENT);
            if (keyEvent.getAction() != KeyEvent.ACTION_DOWN)
                return;
            String cmd = "";
            switch (keyEvent.getKeyCode()) {
                case KeyEvent.KEYCODE_HEADSETHOOK:
                case KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE: {
                    cmd = MusicService.ACTION_TOGGLE_PLAYBACK;
                    break;
                }
                case KeyEvent.KEYCODE_MEDIA_PLAY: {
                    cmd = MusicService.ACTION_PLAY;
                    break;
                }
                case KeyEvent.KEYCODE_MEDIA_PAUSE: {
                    cmd = MusicService.ACTION_PAUSE;
                    break;
                }
                case KeyEvent.KEYCODE_MEDIA_STOP: {
                    cmd = MusicService.ACTION_STOP;
                    break;
                }
                case KeyEvent.KEYCODE_MEDIA_NEXT: {
                    cmd = MusicService.ACTION_NEXT;
                    break;
                }
                case KeyEvent.KEYCODE_MEDIA_PREVIOUS: {
                    cmd = MusicService.ACTION_PREVIOUS;
                    break;
                }
            }
            Intent action = new Intent(context, MusicService.class);
            action.setAction(cmd);
            context.startService(action);
        }
    }
}
