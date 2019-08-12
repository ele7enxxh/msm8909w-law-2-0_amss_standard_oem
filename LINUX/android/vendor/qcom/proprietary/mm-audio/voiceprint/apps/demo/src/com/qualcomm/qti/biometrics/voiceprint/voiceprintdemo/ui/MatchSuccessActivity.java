/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.text.Html;
import android.view.View;
import android.widget.Button;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.OnAudioStateChangeListener;

public class MatchSuccessActivity extends VoicePrintActivity implements MicFragment.OnMicEvent {
    private MicFragment micFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.match_success);

        // Microphone should be disabled.
        setupMicFragment();

        Button done = (Button) findViewById(R.id.done);
        done.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                onDone();
            }
        });
    }


    private void setupMicFragment() {
        micFragment = (MicFragment) getFragmentManager().findFragmentById(R.id.micFragment);

        micFragment.setStyle(MicFragment.DARK_BG);

        // Set which text to show
        micFragment.showTextTop(true);
        micFragment.showTextBottom(true);


        // Setup the text
        micFragment.setTextTop(getString(R.string.got_it));
        micFragment.getTextBottom().setText(Html.fromHtml(String.format(
                getString(R.string.phrase_recognized),
                getVoicePrintApp().getCurrentKeyPhrase().getPhrase())));
        micFragment.setEnabled(false);

        micFragment.setOnMicEventListener(this);
    }

    private void onDone() {
        startActivity(new Intent(this, MainActivity.class));
    }

    @Override
    public void onMicButtonClick() {

    }

    @Override
    public void onRecordingStarted() {

    }

    @Override
    public void onRecordingStopped() {

    }

    @Override
    public void onAudioParametersChange() {

    }

    @Override
    public void onAudioFrame(int index, byte[] buffer, float energy) {

    }

    @Override
    public void onError(OnAudioStateChangeListener.AudioError error) {

    }

    @Override
    public void onResume() {
        super.onResume();

    }
}
