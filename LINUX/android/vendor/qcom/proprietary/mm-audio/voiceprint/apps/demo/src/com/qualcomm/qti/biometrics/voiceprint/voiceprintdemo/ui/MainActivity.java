/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ProgressBar;


import com.qualcomm.qti.biometrics.voiceprint.sdk.OnMatchListener;
import com.qualcomm.qti.biometrics.voiceprint.sdk.VoicePrintManager;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultBase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.Consts;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.AudioRecordManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.OnAudioStateChangeListener;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseAction;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseList;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintPreferences;


public class MainActivity extends VoicePrintActivity implements KeyPhraseSelectAdapter.OnKeyPhraseSelectionChange,
    MicFragment.OnMicEvent {


    private MicFragment micFragment;
    private LinearLayout detailView;
    private boolean collapsed = true;
    private LinearLayout phraseView;

    private KeyPhraseSelectAdapter mAdapter;
    private PhraseListView phraseListView;

    private final int ADD_VERIFICATION = 0;
    private final int EDIT_VERIFICATION = 1;
    private ProgressBar voiceLevel;
    private ProgressBar backgroundLevel;
    private ProgressBar voiceLevelSmall;
    private ProgressBar backgroundLevelSmall;

    private ProgressDialog progress;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "MainActivity: onCreate()");
        VoicePrintManager test = VoicePrintManager.newInstance(this);

        if (mManager == null || mManager.isVoicePrintAvailable() == false) {
            Intent intent = new Intent(this, VoicePrintNotAvailable.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_NO_HISTORY);
            startActivityForResult(intent, Consts.RESULT_CLOSE_ALL);
            finish();
        } else {
            setContentView(R.layout.match_test);
            setupMicFragment();
            setupPhraseList();
            setupDetailView();

            Log.d(TAG, "Have selection=" + (phraseListView.hasSelection()));
            micFragment.setEnabled(phraseListView.hasSelection());

            //progress = new ProgressDialog(this);


        }
    }

    @Override
    protected void onPause() {
        if (AudioRecordManager.getInstance().isRecording()) {
            AudioRecordManager.getInstance().stopRecording();

            mManager.cancel();
        }


        super.onPause();
    }

    private void setupDetailView() {
        detailView = (LinearLayout) findViewById(R.id.detailView);
        final View collapsedView = findViewById(R.id.collapsedDetail);
        final View expandedView = findViewById(R.id.expandedDetail);
        final float detailViewHeight = detailView.getHeight();

        voiceLevel = (ProgressBar) findViewById(R.id.voiceLevel);
        backgroundLevel = (ProgressBar) findViewById(R.id.backgroundNoise);
        voiceLevelSmall = (ProgressBar) findViewById(R.id.voiceLevelSmall);
        backgroundLevelSmall = (ProgressBar) findViewById(R.id.backgroundNoiseSmall);

        detailView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (collapsed) {
                    // Animate expand
                    detailView.animate().translationY(0);

                    collapsedView.setVisibility(View.GONE);
                    expandedView.setVisibility(View.VISIBLE);
                } else {
                    // Animate collapse
                    detailView.animate().translationY(expandedView.getHeight() / 6f);

                    collapsedView.setVisibility(View.VISIBLE);
                    expandedView.setVisibility(View.GONE);
                }
                collapsed = !collapsed;
            }
        });
    }

    private void setupPhraseList() {
        //  phraseListFragment = (PhraseListFragment) getFragmentManager().findFragmentById(R.id.phraseList);
        phraseListView = (PhraseListView) findViewById(R.id.phraseList);
        KeyPhraseList list = new KeyPhraseList(getVoicePrintApp(), VoicePrintDatabase.getInstance().getKeyPhrases());

        phraseListView.setPhraseList(list);

        // Give extra scrolling space
        phraseListView.addFooterPadding(80);

        // Find last selected phrase
        phraseListView.select(VoicePrintPreferences.getLastSelectedKeyPhrase(getVoicePrintApp()));
        getVoicePrintApp().setKeyPhrase(phraseListView.getSelected());
        phraseListView.smoothScrollToPosition(phraseListView.getSelectedItemPosition());

        // Get selection changes
        phraseListView.setOnSelectionChangeListener(this);
    }

    private void setupMicFragment() {
        micFragment = (MicFragment) getFragmentManager().findFragmentById(R.id.micFragment);

        // Which text to show
        micFragment.showTextBottom(true);

        micFragment.setStyle(MicFragment.DARK_BG);

        // Handle recording events
        micFragment.setOnMicEventListener(this);

        min = micFragment.getVoiceMin();
        max = micFragment.getVoiceMax();


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        if (micFragment == null || micFragment.isRecording()) {
            return false;
        }
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.match_test, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        switch (id) {
        case R.id.action_add: {
            Intent intent = new Intent(this, PinActivity.class);
            getVoicePrintApp().setKeyPhrase(new KeyPhrase());
            startActivityForResult(intent, ADD_VERIFICATION);
            break;
        }
        case R.id.action_settings: {

            break;
        }
        case R.id.action_edit_key_phrases: {
            Intent intent = new Intent(this, PinActivity.class);
            startActivityForResult(intent, EDIT_VERIFICATION);
            break;
        }
        case R.id.action_about: {
            onAbout();
            break;
        }
        case R.id.delete_all:
            showDialog(this, R.string.delete_phrase,
                       R.string.delete_all_msg,
                       R.string.cap_ok,
                       R.string.cancel,
            new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    onDeleteAll();
                }
            },
            new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
            break;
        default:
            break;
        }

        return super.onOptionsItemSelected(item);
    }

    private void onDeleteAll() {
        VoicePrintDatabase.getInstance().deleteAll();
        mManager.deleteAll();
        startActivity(new Intent(this, MainActivity.class));
    }

    /**
     * On about.
     */
    public void onAbout() {
        try {
            Log.d(TAG, "onAbout");

            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            StringBuilder msg = new StringBuilder();
            msg.append("Voice Print Demo");

            builder.setTitle(R.string.about);
            builder.setMessage(msg.toString()).setNegativeButton(R.string.cap_ok,
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    // User cancelled the dialog
                }
            });
            builder.create();
            builder.show();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "MainActivity received " + requestCode);

        // Check for request code and handle
        if (resultCode == Consts.RESULT_CLOSE_ALL) {
            setResult(Consts.RESULT_CLOSE_ALL);
            finish();
            return;
        }

        if (requestCode == ADD_VERIFICATION || requestCode == EDIT_VERIFICATION) {
            if (resultCode == Consts.RESULT_SUCCESS) {
                startActivity(new Intent(this, requestCode == ADD_VERIFICATION ?
                                         NewKeyPhraseActivity.class : EditKeyPhrasesListActivity.class));
            } else {
                Log.d(TAG, "Verification failure. resultCode=" + resultCode);
            }
        } else {
            Log.d(TAG, "Unknown request result received. request="
                  + requestCode + ", resultCode=" + resultCode);
        }

        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onResume() {
        super.onResume();
        micFragment.setEnabled(phraseListView.hasSelection());
    }

    @Override
    public void onSelectionChange(int position, KeyPhrase newSelected, boolean isChecked) {

        Log.d(TAG, "Saving selection change.");

        // Save new selection
        VoicePrintPreferences.saveLastSelectedKeyPhrase(getVoicePrintApp(), newSelected.getId());

        // For use with app
        getVoicePrintApp().setKeyPhrase(newSelected);

        micFragment.setEnabled(phraseListView.hasSelection());
    }

    @Override
    public void onMicButtonClick() {
        invalidateOptionsMenu();
    }

    @Override
    public void onRecordingStarted() {

        Log.d(TAG, "MainActivity: onRecordingStarted");
        invalidateOptionsMenu();

        // Start the match request
        if (getVoicePrintApp().getCurrentKeyPhrase() == null) {
            // TODO match all
            Log.d(TAG, "No current key phrase selected.");
            showDialog(this, "No Keyphrase Selected", "Please select a keyphrase first.", "OK", null);
        } else {
            AudioRecordManager audioMgr = AudioRecordManager.getInstance();
            Log.d(TAG, "Attempting to match.");
            mManager.match(getVoicePrintApp().getCurrentKeyPhrase().getId(), new OnMatchListener() {
                @Override
                public void onMatchStatus(boolean success, Bundle extra) {
                    getVoicePrintApp().update(extra);
                    onMatch(success);
                }

                @Override
                public void onError(int errorCode) {
                    onMatch(false);
                }
            });

            // Audio data will catch the error in the match listener.
            mManager.onStartAudioCapture(audioMgr.getAudioSampleRate(),
                                         audioMgr.getInputChannelConfig(),
                                         audioMgr.getAudioFormat(), null);
        }
    }

    private void onMatch(boolean match) {
        // if ( progress.isShowing() == false ) {
        //     progress.dismiss();
        // }
        if (AudioRecordManager.getInstance().isRecording()) {
            AudioRecordManager.getInstance().stopRecording();
        }

        if ( false == match ) {
            startActivity(new Intent(this, MatchFailActivity.class));
            return;
        }
        int actionId = getVoicePrintApp().getCurrentKeyPhrase().getActionId();
        switch (actionId) {
        case KeyPhraseAction.NO_ACTION:
            startActivity(new Intent(this, MatchSuccessActivity.class));
            break;
        case KeyPhraseAction.BROWSER: {
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            intent.addCategory(Intent.CATEGORY_APP_BROWSER);
            startActivity(intent);
        }
        break;
        case KeyPhraseAction.EMAIL: {
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            intent.addCategory(Intent.CATEGORY_APP_EMAIL);
            startActivity(intent);
        }
        break;
        case KeyPhraseAction.SMS: {
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            intent.addCategory(Intent.CATEGORY_APP_MESSAGING);
            startActivity(intent);
        }
        break;
        default:
            Log.d(TAG, "Unknown action: " + actionId);
            break;
        }

    }

    @Override
    public void onRecordingStopped() {
        // Match listener will catch error
        mManager.onStopAudioCapture(null);
        // if ( progress.isShowing() == false ) {
        //     progress.show();
        // }
        micFragment.setEnabled(false);
    }

    @Override
    public void onAudioParametersChange() {

    }

    @Override
    public void onAudioFrame(int index, byte[] buffer, float energy) {
        mManager.addFrame(index, buffer);
        update(energy);
    }

    @Override
    public void onError(OnAudioStateChangeListener.AudioError error) {

        Log.d(TAG, "onError received " + error);
        mManager.cancel();
        onMatch(false);
    }

    /**
     * TODO: clean up the following.
     */
    private final static float MIN_ENERGY = 9.0f;
    private final static float MAX_ENERGY = 13.0f;
    private final static float BASE_ENERGY = MAX_ENERGY - MIN_ENERGY;
    //private final static float HIGH_LEVEL_NOISE = 9.74f;    // galaxy S4
    private final static float HIGH_LEVEL_NOISE = 16f;
    private float noiseAverage = 0.0f;
    private long count = 0;
    private final static int INITIAL_COUNT = 5;
    private float max;
    private float min;

    public void update(float energy) {
        count++;

        // initial stage
        if (count < INITIAL_COUNT) {
            noiseAverage += energy;
            return;
        }
        // ready to work
        else if (count == INITIAL_COUNT) {
            noiseAverage += energy;
            noiseAverage /= INITIAL_COUNT;
        } else {
            noiseAverage = (noiseAverage * 0.95f) + (energy * 0.05f);
        }

        float noise = getPercentageNoise(energy);
        //  Log.v(TAG, "Noise Average: " + noiseAverage + ", %=" + noise);

        float voicelevel = getPercentageRate(energy) * 100f;

        voiceLevel.setProgress((int) voicelevel);
        voiceLevelSmall.setProgress((int) voicelevel);

        if (energy > HIGH_LEVEL_NOISE) {
            noise = (noise - voicelevel / 100f) * 100f;
            // noise = (noise - getPercentageRate(energy -HIGH_LEVEL_NOISE)) * 100f;
            //   Log.d(TAG, "High Noise %=" + noise);
            backgroundLevel.setProgress((int) noise);
            backgroundLevelSmall.setProgress((int) noise);
        } else {
            backgroundLevel.setProgress(0);
            backgroundLevelSmall.setProgress(0);
        }
    }

    private float getPercentageRate(float value) {
        if (min >= value) {
            return 0.0f;
        } else if (max <= value) {
            return 1.0f;
        } else {
            value -= min;
            return value / (max - min);
        }
    }

    private float getPercentageNoise(float energy) {
        if (MIN_ENERGY >= energy) {
            return 0.f;
        } else if (MAX_ENERGY <= energy) {
            return 1.f;
        } else {
            return (energy - MIN_ENERGY) / BASE_ENERGY;
        }
    }

    public boolean isNoiseTooHigh() {
        return (HIGH_LEVEL_NOISE <= noiseAverage) ? true : false;
    }



}
